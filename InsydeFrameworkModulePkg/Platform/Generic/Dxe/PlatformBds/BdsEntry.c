//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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
#include "FrontPage.h"
#include "SetupConfig.h"
#include "PostCode.h"
#include "CmosLib.h"
#include "Legacy16.h"
#include EFI_PROTOCOL_DEFINITION (LegacyRegion)
#include EFI_PROTOCOL_DEFINITION (LegacyBios)
#include EFI_PROTOCOL_CONSUMER (SkipScanRemovableDev)

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

  DxeInitializeDriverLib (ImageHandle, SystemTable);

  mBdsImageHandle = ImageHandle;

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
  EFI_SKIP_SCAN_REMOVABLE_DEV_PROTOCOL  *SkipScanRemovableDev;
  EFI_STATUS                            SkipSacnStatus;
  BOOLEAN                               SkipSacnEnable;

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

    //
    // Add the boot next boot option
    //
    SPrint (Buffer, sizeof (Buffer), L"Boot%04x", *mBootNext);
    BootOption = BdsLibVariableToOption (&BootLists, Buffer);
  } else if (BootMode == BOOT_ON_S4_RESUME) {
    CmosData = EfiReadCmos8 (LastBootDevice);

    //
    // Add the boot next boot option
    //
    SPrint (Buffer, sizeof (Buffer), L"Boot%04x", EfiReadCmos8 (LastBootDevice));
    BootOption = BdsLibVariableToOption (&BootLists, Buffer); 
    BootOption->BootCurrent = (UINT16)(UINTN)CmosData;
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
    //
    // 1. skip the header of the link list, becuase it has no boot option
    // 2. skip the efi boot option, if not support UEFI boot function
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
    //
    // After boot, it should enumernate all devices when SkipScanRemovableDev feature is on.
    //
    SkipSacnEnable = FALSE;
    SkipSacnStatus = gBS->LocateProtocol (&gEfiSkipScanRemovableDevProtocolGuid, NULL, &SkipScanRemovableDev);
    if (!EFI_ERROR (SkipSacnStatus)) {
      SkipScanRemovableDev->GetSkipScanState (&SkipSacnEnable);
      if (SkipSacnEnable) {
        SkipScanRemovableDev->SetSkipScanState (FALSE);
      
        InitializeListHead (&BootLists);
        BdsLibEnumerateAllBootOption (TRUE, &BootLists);
        BdsLibBuildOptionFromVar (&BootLists, L"BootOrder");
      }
    }
    if (EFI_ERROR (Status)) {
      //
      // Call platform action to indicate the boot fail
      //
      PlatformBdsBootFail (BootOption, Status, ExitData, ExitDataSize);

      //
      // If SkipScanRemovableDev feature is on, it should check the first option of BootOrder which is just be updated.
   	  // Otherwise, check the next boot option.
      //
      if (SkipSacnEnable) {
        Link = BootLists.ForwardLink;
      } else {
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
      BdsLibEnumerateAllBootOption (TRUE, &BootLists);
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

  POSTCODE (BDS_ENTER_BDS); //PostCode = 0x10, Enter BDS entry
  //
  // Insert the performance probe
  //
  PERF_END (0, DXE_TOK, NULL, 0);
  PERF_START (0, BDS_TOK, NULL, 0);

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
  // Do the platform init, can be customized by OEM/IBV
  //
  PERF_START (0, L"PlatformBds", L"BDS", 0);
  PlatformBdsInit (PrivateData);
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

  Status = OemServices->Funcs[COMMON_LOAD_DEFAULT_SETUP_MENU] (
                          OemServices,
                          COMMON_LOAD_DEFAULT_SETUP_MENU_ARG_COUNT,
                          &LoadDefault
                          );
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
