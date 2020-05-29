//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2004, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  BdsEntry.c

Abstract:

  The entry of the bds

--*/

#include "Bds.h"
#include "GenericBdsLib.h"
#include "BdsPlatform.h"
#include "UefiFrontPage.h"
#include "SetupConfig.h"
#include "PostCode.h"
#include "CmosLib.h"
#include "Legacy16.h"
#include "SensitiveSetupConfigLib.h"
#include EFI_PROTOCOL_DEFINITION (LegacyRegion)
#include EFI_PROTOCOL_DEFINITION (LegacyBios)
#include EFI_PROTOCOL_CONSUMER (SkipScanRemovableDev)
#include EFI_PROTOCOL_CONSUMER (VariableLock)
#include EFI_ARCH_PROTOCOL_CONSUMER (Timer)
//
// Prototypes
//
VOID
EFIAPI
LoadDefaultCheck (
  );

EFI_STATUS
UpdateBootMessage (
  );

#define BOOT_FAIL_STRING_LENTH 79

typedef struct {
  UINT32        Signiture;
  UINT16        StrSegment;
  UINT16        StrOffset;
  UINT32        Attributes;
  UINT8         String;
} BOOT_MESSAGE;

EFI_BDS_ARCH_PROTOCOL_INSTANCE  gBdsInstanceTemplate = {
  EFI_BDS_ARCH_PROTOCOL_INSTANCE_SIGNATURE,
  NULL,
  {BdsEntry},
  0xFFFF,
  TRUE,
  0,
  EXTENSIVE
};


UINT16                          *mBootNext = NULL;
EFI_HANDLE                      mBdsImageHandle;
STATIC EFI_GUID                 mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
STATIC CHAR16                   *mBootTypeStrs[3] = {
                                   L"Dual Boot Type",
                                   L"Legacy Boot Type",
                                   L"UEFI Boot Type"
                                 };


STATIC
CHAR16 *
GetBootTypeStr (
  UINT8      BootType
  )
{
  if (BootType > 2) {
    return NULL;
  }
  return mBootTypeStrs[BootType];

}

STATIC
VOID
ProcessTimeoutChange (
  VOID
  )
/*++

Routine Description:

  Check timeout is whether larger than EFI_TIMEOUT_VARIABLE_MAX.

Arguments:

  None

Returns:

  None

--*/
{
  UINT16                          Timeout;
  UINTN                           Size;
  EFI_STATUS                      Status;
  EFI_OEM_FORM_BROWSER2_PROTOCOL  *OemFormBrowser;
  CHAR16                          *String;
  UINTN                           StringSize;
  EFI_INPUT_KEY                   Key;
  CHAR16                          *ChangeStr;

  Size = sizeof (UINT16);
  Status = gRT->GetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &Timeout
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  if (Timeout <= EFI_TIMEOUT_VARIABLE_MAX) {
    return;
  }
  Status = gBS->LocateProtocol (
                  &gEfiOemFormBrowser2ProtocolGuid,
                  NULL,
                  (VOID **) &OemFormBrowser
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }

  ChangeStr  = GetStringById (STRING_TOKEN (STR_TIMEOUT_CHANGE));
  StringSize = 0x100 + EfiStrSize (ChangeStr);
  String     = EfiLibAllocateZeroPool (StringSize);
  if (String == NULL || ChangeStr == NULL) {
     return;
  }
  SPrint (String, StringSize, ChangeStr, Timeout, EFI_TIMEOUT_VARIABLE_MAX, EFI_TIMEOUT_VARIABLE_DEFAULT);
  DisableQuietBoot ();
  OemFormBrowser->CreatePopUp (
                    DlgOk,
                    FALSE,
                    0,
                    NULL,
                    &Key,
                    String
                    );
  gBS->FreePool (String);
  gBS->FreePool (ChangeStr);
  Timeout = EFI_TIMEOUT_VARIABLE_DEFAULT;
  Status = gRT->SetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof (UINT16),
                  &Timeout
                  );
}

STATIC
VOID
ProcessBootTypeChange (
  VOID
  )
/*++

Routine Description:

  Check Boot type is whether changed by other tools.

Arguments:

  None

Returns:

  None

--*/
{
  SYSTEM_CONFIGURATION            *SetupConfig;
  UINT8                           BootType;
  UINTN                           Size;
  EFI_STATUS                      Status;
  EFI_OEM_FORM_BROWSER2_PROTOCOL  *OemFormBrowser;
  CHAR16                          *String;
  UINTN                           StringSize;
  EFI_INPUT_KEY                   Key;
  CHAR16                          *OrgType;
  CHAR16                          *NewType;
  EFI_SETUP_UTILITY_PROTOCOL      *SetupUtility;
  CHAR16                          *ChangeConfirmStr;
  CHAR16                          *ChangeStr;

  Size = sizeof (UINT8);
  Status = gRT->GetVariable (
                  L"BootType",
                  &mSystemConfigurationGuid,
                  NULL,
                  &Size,
                  &BootType
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  SetupConfig = BdsLibGetVariableAndSize (
                  L"Setup",
                  &mSystemConfigurationGuid,
                  &Size
                  );
  ASSERT (SetupConfig != NULL);
  if (SetupConfig == NULL) {
    return;
  }


  if (SetupConfig->BootTypeReserved != BootType) {
    Status = gBS->LocateProtocol (
                    &gEfiOemFormBrowser2ProtocolGuid,
                    NULL,
                    (VOID **) &OemFormBrowser
                    );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      gBS->FreePool (SetupConfig);
      return;
    }
    DisableQuietBoot ();
    NewType = GetBootTypeStr (SetupConfig->BootTypeReserved);
    OrgType = GetBootTypeStr (BootType);
    ChangeConfirmStr = NULL;
    if (OrgType == NULL) {
      DEBUG ((EFI_D_ERROR, "The value of Boot type in BootType variable is incorrect.\n"));
      ASSERT (FALSE);
      return;
    } else if (NewType == NULL) {
      DEBUG ((EFI_D_ERROR, "The value of Boot type in Setup variable is incorrect.\n"));
      ASSERT (FALSE);
      return;
    } else {
      ChangeConfirmStr = GetStringById (STRING_TOKEN (STR_BOOT_TYPE_CHANGE_CONFIRM));
      StringSize = EfiStrSize (NewType) + EfiStrSize (OrgType) + EfiStrSize (ChangeConfirmStr);
      String = EfiLibAllocateZeroPool (StringSize);
      if (String == NULL || ChangeConfirmStr == NULL) {
        return;
      }

      SPrint (String, StringSize, ChangeConfirmStr, OrgType, NewType);
    }
    OemFormBrowser->CreatePopUp (
                      DlgYesNo,
                      FALSE,
                      0,
                      NULL,
                      &Key,
                      String
                      );
    gBS->FreePool (String);
    gBS->FreePool (ChangeConfirmStr);
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      Status = SetVariableToSensitiveVariable (
                 L"BootType",
                 &mSystemConfigurationGuid,
                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                 sizeof (UINT8),
                 &SetupConfig->BootTypeReserved
                 );
      ChangeStr = GetStringById (STRING_TOKEN (STR_BOOT_TYPE_CHANGE));
      StringSize = EfiStrSize (NewType) + EfiStrSize (OrgType) + EfiStrSize (ChangeStr);
      String = EfiLibAllocateZeroPool (StringSize);
      if (String == NULL || ChangeStr == NULL) {
        return;
      }
      SPrint (String, StringSize, ChangeStr, OrgType, NewType);
      OemFormBrowser->CreatePopUp (
                        DlgOk,
                        FALSE,
                        0,
                        NULL,
                        &Key,
                        String
                        );
      gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    } else {
      SetupConfig->BootTypeReserved = BootType;
      Status = gRT->SetVariable (
                      L"Setup",
                      &mSystemConfigurationGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      Size,
                      SetupConfig
                      );
      Status = gBS->LocateProtocol (
                      &gEfiSetupUtilityProtocolGuid,
                      NULL,
                      (VOID **) &SetupUtility
                      );
      if (!EFI_ERROR (Status)) {
        ((SYSTEM_CONFIGURATION *) SetupUtility->SetupNvData)->BootTypeReserved = BootType;
      }
    }
  }
  gBS->FreePool (SetupConfig);
}

VOID
EFIAPI
ProcessChangeCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
/*++

Routine Description:

  Callback function to check variable is whether modified by tool and variable value is whether valid.

Arguments:

  Event    - Event whose notification function is being invoked.
  Context  - Pointer to the notification function's context.

Returns:

  None

--*/
{
  EFI_STATUS                      Status;
  VOID                            *BdsDiagnostics;

  Status = gBS->LocateProtocol (
                  &gEfiStartOfBdsDiagnosticsProtocolGuid,
                  NULL,
                  &BdsDiagnostics
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent (Event);

  ProcessTimeoutChange ();
  ProcessBootTypeChange ();
}

EFI_DRIVER_ENTRY_POINT (BdsInitialize)

EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
/*++

Routine Description:

  Install Boot Device Selection Protocol

Arguments:

  (Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT)

Returns:

  EFI_SUCEESS - BDS has finished initializing.
                Rerun the
                dispatcher and recall BDS.Entry

  Other       - Return value from EfiLibAllocatePool()
                or gBS->InstallProtocolInterface

--*/
{
  EFI_STATUS  Status;
  VOID        *Registration;

  DxeInitializeDriverLib (ImageHandle, SystemTable);

  mBdsImageHandle = ImageHandle;

  EfiLibCreateProtocolNotifyEvent (
    &gEfiStartOfBdsDiagnosticsProtocolGuid,
    EFI_TPL_CALLBACK,
    ProcessChangeCallback,
    NULL,
    &Registration
    );

  LoadDefaultCheck ();

  //
  // Install protocol interface
  //
  Status = gBS->InstallProtocolInterface (
                  &gBdsInstanceTemplate.Handle,
                  &gEfiBdsArchProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gBdsInstanceTemplate.Bds
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
AutoFailoverPolicyBehavior (
  IN BOOLEAN    AutoFailover,
  IN CHAR16     *Message
  )
/*++

Routine Description:

  Show boot fail message. If auto failover is disable, it will go into firmware UI.

Arguments:

  AutoFailover - Auto failover polciy
  Message      - Boot fail message

Returns:

  EFI_SUCEESS           - Perform auto failover policy success.
  EFI_INVALID_PARAMETER - String pointer is NULL.
  Other                 - Locate protocol fail or pop message fail.

--*/
{
  EFI_STATUS                        Status;
  EFI_INPUT_KEY                     Key;
  EFI_OEM_FORM_BROWSER2_PROTOCOL    *OemFormBrowser;
  EFI_LIST_ENTRY                    BootLists;
#ifdef FRONTPAGE_SUPPORT
  UINT16                            Timeout;
#endif
#ifdef ENABLE_CONSOLE_EX
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx;

  SimpleTextInEx = NULL;
  gBS->HandleProtocol (
         gST->ConsoleInHandle,
         &gEfiSimpleTextInputExProtocolGuid,
         &SimpleTextInEx
         );
#endif

  if (Message == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gEfiOemFormBrowser2ProtocolGuid,
                  NULL,
                  &OemFormBrowser
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DisableQuietBoot ();

  Status = OemFormBrowser->CreatePopUp (2, FALSE, 0, NULL, &Key, Message);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  gST->ConOut->ClearScreen (gST->ConOut);

  //
  // If auto failover is disable, go into firmware UI.
  //
  if (!AutoFailover) {
    InitializeListHead (&BootLists);
    BdsLibConnectAll ();
    BdsLibEnumerateAllBootOption (TRUE, &BootLists);
#ifdef FRONTPAGE_SUPPORT
    Timeout = 0xffff;
    PlatformBdsEnterFrontPage (Timeout, FALSE);
#else
    CallBootManager ();
#endif
  }

  return EFI_SUCCESS;
}

UINT8
CalulateAddParity (
  UINT8         Data
)
{
  UINTN         Index;
  UINT8         TempData;
  BOOLEAN       ClearParityBit;

  TempData = Data;

  ClearParityBit = FALSE;
  for (Index = 0; Index < 7; Index++) {
    ClearParityBit = (BOOLEAN) ((TempData & 1) ^ ClearParityBit);
    TempData >>= 1;
  }
  if (ClearParityBit) {
    Data &= 0x7f;
  } else {
    Data |= 0x80;
  }

  return Data;

}

EFI_STATUS
SetSimpBootFlag (
  VOID
)
{
  UINT8     Data8;
  UINT8     TempData8;

  Data8 = EfiReadCmos8 (SimpleBootFlag);
  //
  // check BOOT bit. If BOOT bit enable, enable DIAG bit. otherwise, disable DIAG bit.
  // then enable BOOT bit      BOOT bit = Bit1  DIAG bit = bit2 SUPPERSSBOOTDISPLAY bit = bit3
  //
  TempData8 = Data8 & 0x02;
  if (TempData8 == 0) {
    Data8 &= 0xFB;
  } else {
    Data8 |= 0x04;
  }
  Data8 |= 0x02;

  //
  // diable SUPPERSSBOOTDISPLAY Flag
  //
  Data8 &= 0xF7;
  Data8 = CalulateAddParity (Data8);

  EfiWriteCmos8 (SimpleBootFlag, Data8);
  return EFI_SUCCESS;
}

VOID
CreateNoMappingBootOptionPopUp (
  IN  UINT16      OptionNum
  )
/*++

Routine Description:

  Popup a message for user to indicate no mapping boot option to boot.

Arguments:

  OptionNum    - Input boot option number.

Returns:

  None.

--*/
{
  EFI_OEM_FORM_BROWSER2_PROTOCOL        *OemFormBrowser;
  EFI_INPUT_KEY                         Key;
  EFI_STATUS                            Status;
  CHAR16                                *String;

  if (!BdsLibIsDummyBootOption (OptionNum)) {
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEfiOemFormBrowser2ProtocolGuid,
                  NULL,
                  &OemFormBrowser
                  );
  ASSERT_EFI_ERROR (Status);

  DisableQuietBoot ();
  String = NULL;
  if (OptionNum == DummyUsbBootOptionNum) {
    String = GetStringById (STRING_TOKEN (STR_USB_OPTION_INEXISTENCE));
  } else if (OptionNum == DummyCDBootOptionNum){
    String = GetStringById (STRING_TOKEN (STR_CD_OPTION_INEXISTENCE));
  } else if (OptionNum == DummyNetwokrBootOptionNum){
    String = GetStringById (STRING_TOKEN (STR_NETWORK_OPTION_INEXISTENCE));
  }
  ASSERT (String != NULL);
  if (String == NULL) {
    return;
  }
  OemFormBrowser->CreatePopUp (
                    DlgOk,
                    FALSE,
                    0,
                    NULL,
                    &Key,
                    String
                    );
  gBS->FreePool (String);

  return;
}

EFI_STATUS
SelectMappingBootOption (
  IN       UINT16      OptionNum,
  IN       UINTN       OptionCount,
  IN       UINT16      *OptionOrder,
  OUT      UINT16      *SelectedOpitonNum
  )
/*++

Routine Description:

  Popup a option list for user to select which mapping boot option want to boot.

Arguments:

  OptionNum           - Input boot option number.
  OptionCount         - The total number of boot option in option order.
  OptionOrder         - Pointer to option order.
  SelectedOpitonNum   - The option number which user selected.

Returns:

  EFI_SUCCESS            - User selects a specific mapping boot option successful.
  EFI_INVALID_PARAMETER  - OptionCount is 0, OptionOrder is NULL, SelectedOpitonNum is NULL or OptinNum isn't
                           a dummy boot option.
  EFI_NOT_FOUND          - Cannot find corresponding title string for specific boot option type.
  EFI_ABORTED            - User presses ESC key to skip boot option selection menu.

--*/
{
  EFI_OEM_FORM_BROWSER2_PROTOCOL        *OemFormBrowser;
  EFI_STATUS                            Status;
  CHAR16                                **DescriptionOrder;
  UINTN                                 Index;
  CHAR16                                *TitleString;
  BOOLEAN                               BootOptionSelected;
  EFI_INPUT_KEY                         Key;
  UINTN                                 MaxStrLen;

  if (OptionCount == 0 || OptionOrder == NULL || SelectedOpitonNum == NULL || !BdsLibIsDummyBootOption (OptionNum)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
                  &gEfiOemFormBrowser2ProtocolGuid,
                  NULL,
                  &OemFormBrowser
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TitleString = NULL;
  if (OptionNum == DummyUsbBootOptionNum) {
    TitleString = GetStringById (STRING_TOKEN (STR_SELECT_USB_OPTION));
  } else if (OptionNum == DummyCDBootOptionNum) {
    TitleString = GetStringById (STRING_TOKEN (STR_SELECT_CD_OPTION));
  } else if (OptionNum == DummyNetwokrBootOptionNum) {
    TitleString = GetStringById (STRING_TOKEN (STR_SELECT_NETWORK_OPTION));
  }
  if (TitleString == NULL) {
    return EFI_NOT_FOUND;
  }

  DescriptionOrder = EfiLibAllocateZeroPool (OptionCount * sizeof (CHAR16 *));
  MaxStrLen = 0;
  for (Index = 0; Index < OptionCount; Index++) {
    DescriptionOrder[Index] = BdsLibGetDescriptionFromBootOption (OptionOrder[Index]);
    MaxStrLen = EfiStrLen (DescriptionOrder[Index]) > MaxStrLen ? EfiStrLen (DescriptionOrder[Index]) : MaxStrLen;
  }

  DisableQuietBoot ();
  BootOptionSelected = FALSE;

  OemFormBrowser->OptionIcon (
                    OptionCount,
                    FALSE,
                    NULL,
                    &Key,
                    MaxStrLen,
                    TitleString,
                    &Index,
                    DescriptionOrder,
                    0
                    );
  if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
    BootOptionSelected = TRUE;
    *SelectedOpitonNum = OptionOrder[Index];
  }

  gBS->FreePool (TitleString);
  for (Index = 0; Index < OptionCount; Index++) {
    gBS->FreePool (DescriptionOrder[Index]);
  }

  Status = BootOptionSelected ? EFI_SUCCESS : EFI_ABORTED;
  return Status;
}

BOOLEAN
IsLegacyBootOption (
  IN  UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to check this boot option is whether a Legacy boot option.

Arguments:

  OptionNum   - The boot option number.

Returns:

  TRUE        - This is a Legacy boot option.
  FALSE       - This isn't a Legacy boot option.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  BOOLEAN                    IsLegacyBoot;
  UINT32                     DevicePathType;

  if (OptionNum == 0xFF) {
    return FALSE;
  }

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsLegacyBoot = FALSE;
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (DevicePathType == BDS_LEGACY_BBS_BOOT) {
    IsLegacyBoot = TRUE;
  }

  gBS->FreePool (DevicePath);
  return IsLegacyBoot;
}

VOID
BdsBootDeviceSelect (
  VOID
  )
/*++

Routine Description:

  In the loop of attempt to boot for the boot order

Arguments:

  None.

Returns:

  None.

--*/
{
  EFI_STATUS        Status;
  EFI_LIST_ENTRY    *Link;
  BDS_COMMON_OPTION *BootOption;
  UINTN             ExitDataSize;
  CHAR16            *ExitData;
#ifdef FRONTPAGE_SUPPORT
  UINT16            Timeout;
#endif
  EFI_LIST_ENTRY    BootLists;
  CHAR16            Buffer[20];
  BOOLEAN           BootNextExist;
  EFI_LIST_ENTRY    *LinkBootNext;
  EFI_SETUP_UTILITY_PROTOCOL         *SetupUtility = NULL;
  SYSTEM_CONFIGURATION               *SystemConfiguration = NULL;
  EFI_BOOT_MODE                BootMode;
  UINT8                        CmosData = 0;
  BOOLEAN                               AlreadyConnectAll = FALSE;
  BOOLEAN                               LinkUpdated;
  UINTN                                 MappingOpitonCount;
  UINT16                                *MappingOpitonOrder;
  UINT16                                SelectedOptionNum;
  UINTN                                 VariableSize;
  VOID                                  *VariablePtr;
  BDS_COMMON_OPTION                     *TempBootOption;
  CHAR16                                *String;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;

  POSTCODE (BDS_BOOT_DEVICE_SELECT);  //PostCode = 0x2B, Try to boot system to OS
  Status = BdsLibGetBootMode (&BootMode);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Got the latest boot option
  //
  BootNextExist = FALSE;
  LinkBootNext  = NULL;
  InitializeListHead (&BootLists);

  //
  // First check the boot next option
  //
  EfiZeroMem (Buffer, sizeof (Buffer));

  if (mBootNext != NULL) {
    //
    // Indicate we have the boot next variable, so this time
    // boot will always have this boot option
    //
    BootNextExist = TRUE;

    //
    // Clear the this variable so it's only exist in this time boot
    //
    gRT->SetVariable (
          L"BootNext",
          &gEfiGlobalVariableGuid,
          EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
          0,
          mBootNext
          );

    if (BdsLibIsDummyBootOption (*mBootNext)) {
       BdsLibGetMappingBootOptions (*mBootNext, &MappingOpitonCount, &MappingOpitonOrder);
       if (MappingOpitonCount == 0) {
         //
         // Show no USB boot option message for user and then go to Boot Manager menu for user to select boot option.
         //
         CreateNoMappingBootOptionPopUp (*mBootNext);
         VariableSize = 0;
         VariablePtr = NULL;
         Status  = gRT->GetVariable (L"BootOrder", &gEfiGlobalVariableGuid, NULL, &VariableSize, VariablePtr);
         if (Status == EFI_BUFFER_TOO_SMALL) {
           CallBootManager ();
#ifdef FRONTPAGE_SUPPORT
           Timeout = 0xffff;
           PlatformBdsEnterFrontPage (Timeout, TRUE);
#endif
         }
         BootNextExist = FALSE;
       } else if (MappingOpitonCount == 1) {
         //
         // System has one mapping boot option, boot this boot option direclty.
         //
         SPrint (Buffer, sizeof (Buffer), L"Boot%04x", MappingOpitonOrder[0]);
         BootOption = BdsLibVariableToOption (&BootLists, Buffer);
       } else {
         //
         // If system has more than 1 mapping boot option, let user to select specific mapping boot option.
         //
         Status = SelectMappingBootOption (*mBootNext, MappingOpitonCount, MappingOpitonOrder, &SelectedOptionNum);
         if (!EFI_ERROR (Status)) {
           SPrint (Buffer, sizeof (Buffer), L"Boot%04x", SelectedOptionNum);
           BootOption = BdsLibVariableToOption (&BootLists, Buffer);
         } else {
           BootNextExist = FALSE;
         }
       }
       if (MappingOpitonOrder != NULL) {
         gBS->FreePool (MappingOpitonOrder);
       }
    } else {
      //
      // Add the boot next boot option
      //
      SPrint (Buffer, sizeof (Buffer), L"Boot%04x", *mBootNext);
      BootOption = BdsLibVariableToOption (&BootLists, Buffer);
    }
  } else if (BootMode == BOOT_ON_S4_RESUME) {
    CmosData = EfiReadCmos8 (LastBootDevice);
    if (IsLegacyBootOption ((UINT16)CmosData)) {
      //
      // Add the boot next boot option
      //
      SPrint (Buffer, sizeof (Buffer), L"Boot%04x", EfiReadCmos8 (LastBootDevice));
      BootOption = BdsLibVariableToOption (&BootLists, Buffer);
      BootOption->BootCurrent = (UINT16)CmosData;
    }
  }
  //
  // Parse the boot order to get boot option
  //
  BdsLibBuildOptionFromVar (&BootLists, L"BootOrder");
  Link = BootLists.ForwardLink;

  //
  // Parameter check, make sure the loop will be valid
  //
  if (Link == NULL) {
    return ;
  }
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid,
                                NULL,
                                &SetupUtility
                               );
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (SYSTEM_CONFIGURATION *)SetupUtility->SetupNvData;
  }
  //
  // Here we make the boot in a loop, every boot success will
  // return to the front page
  //
  for (;;) {
    //
    // Check the boot option list first
    //
    if (Link == &BootLists) {
      //
      // There are two ways to enter here:
      // 1. There is no active boot option, give user chance to
      //    add new boot option
      // 2. All the active boot option processed, and there is no
      //    one is success to boot, then we back here to allow user
      //    add new active boot option
      //
      String = GetStringById (STRING_TOKEN (STR_AUTO_FAILOVER_NO_BOOT_DEVICE));
      if (String != NULL) {
        AutoFailoverPolicyBehavior (TRUE, String);
        gBS->FreePool(String);
      }

#ifdef FRONTPAGE_SUPPORT
      Timeout = 0xffff;
      PlatformBdsEnterFrontPage (Timeout, FALSE);
#else
      CallBootManager ();
#endif
      InitializeListHead (&BootLists);
      BdsLibBuildOptionFromVar (&BootLists, L"BootOrder");
      Link = BootLists.ForwardLink;
      continue;
    }
    //
    // Get the boot option from the link list
    //
    BootOption = CR (Link, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);

    //
    // According to EFI Specification, if a load option is not marked
    // as LOAD_OPTION_ACTIVE, the boot manager will not automatically
    // load the option.
    //
    if (!IS_LOAD_OPTION_TYPE (BootOption->Attribute, LOAD_OPTION_ACTIVE)) {
      Link = Link->ForwardLink;
      continue;
    }
    //
    // Make sure the boot option device path connected,
    // but ignore the BBS device path
    //
    if (DevicePathType (BootOption->DevicePath) != BBS_DEVICE_PATH) {
      //
      // Notes: the internal shell can not been connected with device path
      // so we do not check the status here
      //
      BdsLibConnectDevicePath (BootOption->DevicePath);
    }
    Status = BdsLibBootViaBootOption (BootOption, BootOption->DevicePath, &ExitDataSize, &ExitData);
    LinkUpdated = FALSE;
    if (BdsLibIsWin8FastBootActive () && !AlreadyConnectAll) {
      //
      // For Win 8 Fast Boot, do one time ConnectAll to connect all boot devices and point Link to next boot device.
      //
      AlreadyConnectAll = TRUE;
      InitializeListHead (&BootLists);
      BdsLibConnectUsbHID ();
      BdsLibConnectAll ();
      BdsLibEnumerateAllBootOption (TRUE, &BootLists);

      //
      // If last boot is fail, update Link for next boot.
      //
      if (EFI_ERROR (Status)) {
        Link = BootLists.ForwardLink;
        if (!IsListEmpty (&BootLists)) {
          //
          // If next boot device path is the same as previous one, skip to next.
          //
          DevicePath = BootOption->DevicePath;
          TempBootOption = CR (Link, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
          if (EfiCompareMem (DevicePath, TempBootOption->DevicePath, EfiDevicePathSize (DevicePath)) == 0) {
            Link = Link->ForwardLink;
          }
        }
        LinkUpdated = TRUE;
      }
#ifndef WIN_8_FAST_BOOT_ERROR_MESSAGE_SUPPORT
      if (EFI_ERROR (Status)) {
        continue;
      }
#endif
    }
    if (EFI_ERROR (Status)) {
      //
      // Call platform action to indicate the boot fail
      //
      //
      // If boot default device (not including Windows To Go) fail and Auto Failover is disable, go into firmware UI.
      //
      if (SystemConfiguration != NULL &&
          SystemConfiguration->AutoFailover == 0 &&
          !((DevicePathType (BootOption->DevicePath) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType (BootOption->DevicePath) == MSG_USB_CLASS_DP))) {
        String = GetStringById (STRING_TOKEN (STR_AUTO_FAILOVER_DISABLE_BOOT_FAIL));
        if (String != NULL) {
          AutoFailoverPolicyBehavior (FALSE, String);
          gBS->FreePool(String);
        }
      } else {
        if (!(((DevicePathType (BootOption->DevicePath) == MESSAGING_DEVICE_PATH) &&
               (DevicePathSubType (BootOption->DevicePath) == MSG_USB_CLASS_DP)) &&
              Status != EFI_SECURITY_VIOLATION)) {
          PlatformBdsBootFail (BootOption, Status, ExitData, ExitDataSize);
        }
      }

      if (!LinkUpdated) {
        Link = Link->ForwardLink;
      }
    } else {
      //
      // Call platform action to indicate the boot success
      //
      PlatformBdsBootSuccess (BootOption);

      //
      // Boot success, then stop process the boot order, and
      // present the boot manager menu, front page
      //
#ifdef FRONTPAGE_SUPPORT
      Timeout = 0xffff;
      InitializeListHead (&BootLists);
      BdsLibEnumerateAllBootOption (FALSE, &BootLists);
      PlatformBdsEnterFrontPage (Timeout, FALSE);
#else
      CallBootManager ();
#endif

      //
      // Rescan the boot option list, avoid pertential risk of the boot
      // option change in front page
      //
      if (BootNextExist) {
        LinkBootNext = BootLists.ForwardLink;
      }

      InitializeListHead (&BootLists);
      if (LinkBootNext != NULL) {
        //
        // Reserve the boot next option
        //
        InsertTailList (&BootLists, LinkBootNext);
      }

      BdsLibBuildOptionFromVar (&BootLists, L"BootOrder");
      Link = BootLists.ForwardLink;
    }
  }

  return ;

}

VOID
BdsAllocateMemoryForPerformanceData (
  VOID
  )
/*++

Routine Description:

  Allocate a block of memory that will contain performance data to OS.

Arguments:

  None

Returns:

  None

--*/
{
  EFI_STATUS                    Status;
  EFI_PHYSICAL_ADDRESS          AcpiLowMemoryBase;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLock;

  AcpiLowMemoryBase = 0x0FFFFFFFFULL;

  //
  // Allocate a block of memory that will contain performance data to OS.
  //
  Status = gBS->AllocatePages (
                  AllocateMaxAddress,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES (PERF_DATA_MAX_LENGTH),
                  &AcpiLowMemoryBase
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Save the pointer to variable for use in S3 resume.
    //
    Status = gRT->SetVariable (
                    L"PerfDataMemAddr",
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (UINT32),
                    &AcpiLowMemoryBase
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // Mark L"PerfDataMemAddr" variable to read-only if the Variable Lock protocol exists
    //
    Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
    if (!EFI_ERROR (Status)) {
      Status = VariableLock->RequestToLock (VariableLock, L"PerfDataMemAddr", &gEfiGenericVariableGuid);
      ASSERT_EFI_ERROR (Status);
    }
  }
}

EFI_STATUS
LockSensitiveVariables (
  VOID
  )
{
  EFI_STATUS                       Status;
  EDKII_VARIABLE_LOCK_PROTOCOL     *VariableLock;
  UINTN                            Index;
  SENSITIVE_SYSTEM_CONFIGURATION   *SensitiveSystemConfig;

  SensitiveSystemConfig = GetSensitiveSetupConfig ();
  if (SensitiveSystemConfig == NULL) {
    return EFI_SUCCESS;
  }
  //
  // Lock setup sensitive variables
  //
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
  if (EFI_ERROR (Status)) {
    return Status;
  }

   for (Index = 0; SensitiveSystemConfig[Index].VariableName != NULL; Index++) {
    Status = VariableLock->RequestToLock (VariableLock, SensitiveSystemConfig[Index].VariableName, &mSystemConfigurationGuid);
  }

  return Status;
}



EFI_STATUS
EFIAPI
BdsEntry (
  IN EFI_BDS_ARCH_PROTOCOL  *This
  )
/*++

Routine Description:

  Service routine for BdsInstance->Entry(). Devices are connected, the
  consoles are initialized, and the boot options are tried.

Arguments:

  This - Protocol Instance structure.

Returns:

  EFI_SUCEESS - BDS->Entry has finished executing.

--*/
{
  EFI_BDS_ARCH_PROTOCOL_INSTANCE  *PrivateData;
  EFI_LIST_ENTRY                  DriverOptionList;
  EFI_LIST_ENTRY                  BootOptionList;
  UINTN                           BootNextSize;
  EFI_STATUS                      Status;
  EFI_TIMER_ARCH_PROTOCOL         *Timer;

  POSTCODE (BDS_ENTER_BDS); //PostCode = 0x10, Enter BDS entry
  //
  // Insert the performance probe
  //
  PERF_END (0, DXE_TOK, NULL, 0);
  PERF_START (0, BDS_TOK, NULL, 0);

  PERF_CODE (
    BdsAllocateMemoryForPerformanceData ();
  );
  LockSensitiveVariables ();
  //
  // Initialize the global system boot option and driver option
  //
  InitializeListHead (&DriverOptionList);
  InitializeListHead (&BootOptionList);

  //
  // Get the BDS private data
  //
  PrivateData = EFI_BDS_ARCH_PROTOCOL_INSTANCE_FROM_THIS (This);

  //
  // Set the timer tick to 100hz to make whole timer interrupt handler operate in desire frequency
  //
  Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &Timer);
  if (!EFI_ERROR (Status)) {
    Timer->SetTimerPeriod (Timer, 100000);
  }

  //
  // Do the platform init, can be customized by OEM/IBV
  //
  PERF_START (0, L"PlatformBds", L"BDS", 0);
  PlatformBdsInit (PrivateData);

  //
  // BdsLibGetTimeout() will initialize Timeout variable if variable is not exist.
  //
  BdsLibGetTimeout ();
  UpdateBootMessage ();
  //
  // Set up the device list based on EFI 1.1 variables
  // process Driver#### and Load the driver's in the
  // driver option list
  //
  BdsLibBuildOptionFromVar (&DriverOptionList, L"DriverOrder");
  if (!IsListEmpty (&DriverOptionList)) {
    BdsLibLoadDrivers (&DriverOptionList);
  }
  //
  // Check if we have the boot next option
  //
  mBootNext = BdsLibGetVariableAndSize (
                L"BootNext",
                &gEfiGlobalVariableGuid,
                &BootNextSize
                );
  SetSimpBootFlag ();
  if (BdsLibGetBootType () == LEGACY_BOOT_TYPE) {
    BdsLibSkipEbcDispatch ();
  }
  BdsLibGetBootMode (&PrivateData->BootMode);
  PlatformBdsPolicyBehavior (PrivateData, &DriverOptionList, &BootOptionList);
  PERF_END (0, L"PlatformBds", L"BDS", 0);

  //
  // BDS select the boot device to load OS
  //
  BdsBootDeviceSelect ();

  //
  // Only assert here since this is the right behavior, we should never
  // return back to DxeCore.
  //
  ASSERT (FALSE);

  return EFI_SUCCESS;
}

VOID
EFIAPI
LoadDefaultCheck (
  )
/*++

  Routine Description:

    Query OemServices to determine if it request to load default setup menu.

  Arguments:

    None

  Returns:

    None

--*/
{
  EFI_STATUS                      Status;
  SYSTEM_CONFIGURATION            SetupBuffer;
  OEM_SERVICES_PROTOCOL           *OemServices;
  UINTN                           VariableSize;
  BOOLEAN                         LoadDefault;
  EFI_GUID                        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

  OemServices     = NULL;
  LoadDefault     = FALSE;

  Status = gBS->LocateProtocol (&gOemServicesProtocolGuid, NULL, &OemServices);
  if (EFI_ERROR (Status)) {

    return;
  }

  Status = OemServices->Funcs[COMMON_LOAD_DEFAULT_SETUP_MENU] (OemServices, 1, &LoadDefault);
  if (!EFI_ERROR (Status) && LoadDefault) {
    //
    // Is "Setup" variable existence?
    //
    VariableSize = sizeof (SYSTEM_CONFIGURATION);
    Status = gRT->GetVariable (
                    L"Setup",
                    &SystemConfigurationGuid,
                    NULL,
                    &VariableSize,
                    &SetupBuffer
                    );
    if (EFI_ERROR (Status)) {
      //
      // Setup menu is default setting.
      //
      return;
    }
    //
    // Delete "Setup" variable. It will load default in GetSystemConfigurationVar() of SetupUtility.c
    //
    VariableSize = 0;
    Status = gRT->SetVariable (
                    L"Setup",
                    &SystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    VariableSize,
                    &SetupBuffer
                    );
    if (EFI_ERROR (Status)) {

      return;
    }
    //
    // Delete Boot Type variable
    //
    VariableSize = 0;
    Status = gRT->SetVariable (
                    L"BootType",
                    &mSystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    VariableSize,
                    NULL
                    );

    //
    //Reset start system.
    //
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  return;
}


STATIC
VOID
ConvertChar16ToChar8 (
  IN CHAR8      *Dest,
  IN CHAR16     *Src
  )
{
  while (*Src) {
    *Dest++ = (UINT8) (*Src++);
  }

  *Dest = 0;
}

EFI_STATUS
UpdateBootMessage (
  )
{
  EFI_LEGACY_REGION_PROTOCOL        *LegacyRegion;
  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios;
  EFI_COMPATIBILITY16_TABLE         *Table = NULL;
  EFI_IA32_REGISTER_SET             Regs;
  EFI_STATUS                        Status;
  UINT8                             *Ptr;
  BOOT_MESSAGE                      *BootMsg = NULL;
  BOOLEAN                           Find = FALSE;
  CHAR16                            *NewString = NULL;
  UINTN                             StringLen = 0;
  UINT8                             *NewStringAddress;
  //
  // See if the Legacy Region Protocol is available
  //
  Status = gBS->LocateProtocol (&gEfiLegacyRegionProtocolGuid, NULL, &LegacyRegion);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Ptr = (UINT8 *)((UINTN)0xF0000); Ptr < (UINT8 *) ((UINTN) 0x100000); Ptr += 0x10) {
    if (*(UINT32 *) Ptr == EFI_SIGNATURE_32 ('B', 'D', 'E', 'S')) {
      BootMsg = (BOOT_MESSAGE *) Ptr;
      Find = TRUE;
      break;
    }
  }

  if (Find) {
    NewString = (VOID *)(UINTN)GetStringById (STRING_TOKEN (STR_BOOT_DEVICE_ERROR_MESSAGE));
    StringLen = EfiStrLen (NewString);
    if (NewString == NULL || StringLen == 0) {
      return EFI_NOT_READY;
    }
    Status = LegacyRegion->UnLock (LegacyRegion, 0xF0000, 0x10000, NULL);
    if (StringLen > BOOT_FAIL_STRING_LENTH) {
      for (Ptr = (UINT8 *)((UINTN)0xFE000); Ptr < (UINT8 *) ((UINTN) 0x100000); Ptr += 0x10) {
      if (*(UINT32 *) Ptr == EFI_SIGNATURE_32 ('I', 'F', 'E', '$')) {
        Table   = (EFI_COMPATIBILITY16_TABLE *) Ptr;
        break;
        }
      }
      Status = LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x10000, NULL);
      EfiZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
      Regs.X.AX = Legacy16GetTableAddress;
      Regs.X.BX = 0x0002;
      Regs.X.CX = (UINT16)StringLen;
      Regs.X.DX = 1;
      LegacyBios->FarCall86 (LegacyBios,
                    Table->Compatibility16CallSegment,
                    Table->Compatibility16CallOffset,
                    &Regs,
                    NULL,
                    0);

      NewStringAddress = (UINT8 *)(UINTN)(Regs.X.DS*16 + Regs.X.BX);

      BootMsg->StrSegment = (UINT16)(((UINTN)NewStringAddress >> 4) & 0xF000);
      BootMsg->StrOffset = (UINT16)(UINTN)NewStringAddress;

      EfiZeroMem (NewStringAddress, StringLen);
      ConvertChar16ToChar8 (NewStringAddress, NewString);
      Status = LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x10000, NULL);
    } else {
      EfiZeroMem (&BootMsg->String, BOOT_FAIL_STRING_LENTH);
      ConvertChar16ToChar8 ((UINT8 *)&BootMsg->String, NewString);
    }
    Status = LegacyRegion->Lock (LegacyRegion, 0xF0000, 0x10000, NULL);
  }

  return Status;
}
