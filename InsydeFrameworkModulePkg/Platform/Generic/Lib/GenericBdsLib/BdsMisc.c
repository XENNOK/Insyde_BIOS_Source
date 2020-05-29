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

Copyright (c) 2004 - 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  BdsMisc.c

Abstract:

  Misc BDS library function

--*/

#include "GenericBdsLib.h"
#include "SetupConfig.h"
//#include "UsbCore.h"
#include EFI_PROTOCOL_DEFINITION (ComponentName)
#if (EFI_SPECIFICATION_VERSION >= 0x00020000 && defined(EFI_COMPONENT_NAME2_PROTOCOL_GUID))
#include EFI_PROTOCOL_DEFINITION (ComponentName2)
#endif
#include EFI_PROTOCOL_DEFINITION (UnicodeCollation)
#if ((EFI_SPECIFICATION_VERSION >= 0x00020000 && defined(EFI_UNICODE_COLLATION2_PROTOCOL_GUID)) || (EFI_SPECIFICATION_VERSION >= 0x0002000A))
#include EFI_PROTOCOL_DEFINITION (UnicodeCollation2)
#endif
#include EFI_PROTOCOL_DEFINITION (UsbIo)
#include EFI_PROTOCOL_DEFINITION (DiskInfo)
#include EFI_PROTOCOL_CONSUMER (FileInfo)

#define MAX_STRING_LEN        200

static BOOLEAN   mFeaturerSwitch = TRUE;
static BOOLEAN   mResetRequired = FALSE;
STATIC EFI_GUID  mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;;
extern UINT16    gPlatformBootTimeOutDefault;
typedef
BOOLEAN
(EFIAPI *CHECK_BOOT_OPTION_TYPE) (
  IN  UINT16     OptionNum
  );
//
// OptionNum : Dummy function for specific type (ex: dummy usb number is DummyUsbBootOptionNum).
// CheckOptionType : Function uses option number to check option type.
// Found : Record this option is whether found.
//
typedef struct _DUMMY_OPTION_TYPE_MAP {
  DUMMY_BOOT_OPTION_NUM        OptionNum;
  CHECK_BOOT_OPTION_TYPE       CheckOptionType;
  BOOLEAN                      Found;
} DUMMY_OPTION_TYPE_MAP;

STATIC
BOOLEAN
IsUefiUsbBootOption (
  IN  UINT16     OptionNum
  );

STATIC
BOOLEAN
IsUefiCdBootOption (
  IN  UINT16     OptionNum
  );

STATIC
BOOLEAN
IsUefiNetworkBootOption (
  IN  UINT16     OptionNum
  );

DUMMY_OPTION_TYPE_MAP   mOptionTypeTable[] = {{DummyUsbBootOptionNum,     IsUefiUsbBootOption,     FALSE},
                                              {DummyCDBootOptionNum,      IsUefiCdBootOption,      FALSE},
                                              {DummyNetwokrBootOptionNum, IsUefiNetworkBootOption, FALSE}
                                            };
#define OPTION_TYPE_TABLE_COUNT (sizeof (mOptionTypeTable) / sizeof (DUMMY_OPTION_TYPE_MAP))

UINT16
BdsLibGetTimeout (
  VOID
  )
/*++

Routine Description:

  Return the default value for system Timeout variable.

Arguments:

  None

Returns:

  Timeout value.

--*/
{
  UINT16      Timeout;
  UINTN       Size;
  EFI_STATUS  Status;

  //
  // Return Timeout variable or 0xffff if no valid
  // Timeout variable exists.
  //
  Size    = sizeof (UINT16);
  Status  = gRT->GetVariable (L"Timeout", &gEfiGlobalVariableGuid, NULL, &Size, &Timeout);
  if (!EFI_ERROR (Status)) {
    return Timeout;
  }
  //
  // To make the current EFI Automatic-Test activity possible, just add
  // following code to make AutoBoot enabled when this variable is not
  // present.
  // This code should be removed later.
  //
  Timeout = gPlatformBootTimeOutDefault;

  //
  // Notes: Platform should set default variable if non exists on all error cases!!!
  //
  Status = gRT->SetVariable (
                  L"Timeout",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof (UINT16),
                  &Timeout
                  );
  return Timeout;
}

VOID
BdsLibLoadDrivers (
  IN EFI_LIST_ENTRY               *BdsDriverLists
  )
/*++

Routine Description:

  The function will go through the driver optoin link list, load and start
  every driver the driver optoin device path point to.

Arguments:

  BdsDriverLists   - The header of the current driver option link list

Returns:

  None

--*/
{
  EFI_STATUS                Status;
  EFI_LIST_ENTRY            *Link;
  BDS_COMMON_OPTION         *Option;
  EFI_HANDLE                ImageHandle;
  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;
  UINTN                     ExitDataSize;
  CHAR16                    *ExitData;
  BOOLEAN                   ReconnectAll;

  ReconnectAll = FALSE;

  //
  // Process the driver option
  //
  for (Link = BdsDriverLists->ForwardLink; Link != BdsDriverLists; Link = Link->ForwardLink) {
    Option = CR (Link, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
    //
    // If a load option is not marked as LOAD_OPTION_ACTIVE,
    // the boot manager will not automatically load the option.
    //
    if (!IS_LOAD_OPTION_TYPE (Option->Attribute, LOAD_OPTION_ACTIVE)) {
      continue;
    }
    //
    // If a driver load option is marked as LOAD_OPTION_FORCE_RECONNECT,
    // then all of the EFI drivers in the system will be disconnected and
    // reconnected after the last driver load option is processed.
    //
    if (IS_LOAD_OPTION_TYPE (Option->Attribute, LOAD_OPTION_FORCE_RECONNECT)) {
      ReconnectAll = TRUE;
    }
    //
    // Make sure the driver path is connected.
    //
    BdsLibConnectDevicePath (Option->DevicePath);

    //
    // Load and start the image that Driver#### describes
    //
    Status = gBS->LoadImage (
                    FALSE,
                    mBdsImageHandle,
                    Option->DevicePath,
                    NULL,
                    0,
                    &ImageHandle
                    );

    if (!EFI_ERROR (Status)) {
      gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, &ImageInfo);

      //
      // Verify whether this image is a driver, if not,
      // exit it and continue to parse next load option
      //
      if (ImageInfo->ImageCodeType != EfiBootServicesCode && ImageInfo->ImageCodeType != EfiRuntimeServicesCode) {
        gBS->Exit (ImageHandle, EFI_INVALID_PARAMETER, 0, NULL);
        continue;
      }

      if (Option->LoadOptionsSize != 0) {
        ImageInfo->LoadOptionsSize  = Option->LoadOptionsSize;
        ImageInfo->LoadOptions      = Option->LoadOptions;
      }
      //
      // Before calling the image, enable the Watchdog Timer for
      // the 5 Minute period
      //
      gBS->SetWatchdogTimer (5 * 60, 0x0000, 0x00, NULL);

      Status = gBS->StartImage (ImageHandle, &ExitDataSize, &ExitData);
      DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Driver Return Status = %r\n", Status));

      //
      // Clear the Watchdog Timer after the image returns
      //
      gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
    }
  }
  //
  // Process the LOAD_OPTION_FORCE_RECONNECT driver option
  //
  if (ReconnectAll) {
    BdsLibDisconnectAllEfi ();
    BdsLibConnectAll ();
  }

}

UINT16
BdsLibGetFreeOptionNumber (
  IN  CHAR16    *VariableName
  )
/*++

Routine Description:
  Get the Option Number that does not used
  Try to locate the specific option variable one by one untile find a free number

Arguments:
  VariableName - Indicate if the boot#### or driver#### option

Returns:
  The Minimal Free Option Number

--*/
{
  UINT16        Number;
  UINTN         Index;
  CHAR16        StrTemp[10];
  UINT16        *OptionBuffer;
  UINTN         OptionSize;

  //
  // Try to find the minimum free number from 0, 1, 2, 3....
  //
  Index = 0;
  do {
    if (*VariableName == 'B') {
      SPrint (StrTemp, sizeof (StrTemp), L"Boot%04x", Index);
    } else {
      SPrint (StrTemp, sizeof (StrTemp), L"Driver%04x", Index);
    }
    //
    // try if the option number is used
    //
    OptionBuffer = BdsLibGetVariableAndSize (
              StrTemp,
              &gEfiGlobalVariableGuid,
              &OptionSize
              );
    if (OptionBuffer == NULL) {
      break;
    }
    Index++;
  } while (1);

  Number = (UINT16) Index;
  return Number;
}

EFI_STATUS
BdsLibUpdateOptionVar (
  IN  CHAR16                         *OptionName,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  )
/*++

Routine Description:

  Update the boot#### or driver#### variable.

Arguments:

  OptionName       - String part of EFI variable name
  DevicePath       - The device path of the boot#### or driver####
  Description      - The description of the boot#### or driver####
  OptionalData     - Optional data of the boot#### or driver####
  OptionalDataSize - Optional data size of the boot#### or driver####

Returns:

  EFI_SUCCESS      - The boot#### or driver#### have been success updated.
  EFI_ABORTED      - Allocate memory resource fail.
  otherwise        - Set variable fail.

--*/
{
  EFI_STATUS                Status;
  UINTN                     OptionSize;
  VOID                      *OptionPtr;
//  UINT8                     *TempPtr;

  Status  = BdsLibCreateLoadOption (
              DevicePath,
              Description,
              OptionalData,
              OptionalDataSize,
              &OptionPtr,
              &OptionSize
              );
  if (EFI_ERROR (Status)) {
    return Status;
  }
//if (OptionalData == NULL) {
//  OptionalDataSize = 0;
//}
//
//OptionSize          = sizeof (UINT32) + sizeof (UINT16) + EfiStrSize (Description) + EfiDevicePathSize (DevicePath) + OptionalDataSize;
//OptionPtr           = EfiLibAllocateZeroPool (OptionSize);
//if (OptionPtr == NULL) {
//  return EFI_ABORTED;
//}
//
//TempPtr             = OptionPtr;
//*(UINT32 *) TempPtr = LOAD_OPTION_ACTIVE;
//TempPtr += sizeof (UINT32);
//*(UINT16 *) TempPtr = (UINT16) EfiDevicePathSize (DevicePath);
//TempPtr += sizeof (UINT16);
//EfiCopyMem (TempPtr, Description, EfiStrSize (Description));
//TempPtr += EfiStrSize (Description);
//EfiCopyMem (TempPtr, DevicePath, EfiDevicePathSize (DevicePath));
//TempPtr += EfiDevicePathSize (DevicePath);
//EfiCopyMem (TempPtr, OptionalData, OptionalDataSize);
  Status = gRT->SetVariable (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  OptionSize,
                  OptionPtr
                  );
  gBS->FreePool (OptionPtr);

  return Status;
}

EFI_STATUS
BdsLibRegisterNewOption (
  IN  EFI_LIST_ENTRY                 *BdsOptionList,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *String,
  IN  CHAR16                         *VariableName,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  )
/*++

Routine Description:

  This function will register the new boot#### or driver#### option base on
  the VariableName. The new registered boot#### or driver#### will be linked
  to BdsOptionList and also update to the VariableName. After the boot#### or
  driver#### updated, the BootOrder or DriverOrder will also be updated.

Arguments:

  BdsOptionList    - The header of the boot#### or driver#### link list

  DevicePath       - The device path which the boot####
                     or driver#### option present

  String           - The description of the boot#### or driver####

  VariableName     - Indicate if the boot#### or driver#### option

  OptionalData     - Pointer to a optional data

  OptionalDataSize - The size of optional data


Returns:

  EFI_SUCCESS      - The boot#### or driver#### have been success registered

  EFI_STATUS       - Return the status of gRT->SetVariable ().

--*/
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    RegisterOptionNumber;
  UINT16                    *TempOptionPtr;
  UINTN                     TempOptionSize;
  UINT16                    *OptionOrderPtr;
  VOID                      *OptionPtr;
  UINTN                     OptionSize;
  UINT8                     *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  CHAR16                    *Description;
  CHAR16                    OptionName[10];
  BOOLEAN                   UpdateDescription;
  UINTN                     BootOptionNum = 0;
  EFI_BOOT_OPTION_POLICY_PROTOCOL           *BootOptionPolicy;
  UINTN                                     NewPosition = 0;
  UINTN                                     BootOptionType = 0;

  OptionPtr             = NULL;
  OptionSize            = 0;
  TempPtr               = NULL;
  OptionDevicePath      = NULL;
  Description           = NULL;
  OptionOrderPtr        = NULL;
  UpdateDescription     = FALSE;

  EfiZeroMem (OptionName, sizeof (OptionName));

  TempOptionSize = 0;
  TempOptionPtr = BdsLibGetVariableAndSize (
                    VariableName,
                    &gEfiGlobalVariableGuid,
                    &TempOptionSize
                    );
  //
  // Compare with current option variable
  //
  BootOptionNum = TempOptionSize / sizeof (UINT16);
  for (Index = 0; Index < BootOptionNum; Index++) {
    //
    // Got the max option#### number
    //
    // remark these and get option number in GetNewBootOptionNo function.

    if (*VariableName == 'B') {
      SPrint (OptionName, sizeof (OptionName), L"Boot%04x", TempOptionPtr[Index]);
    } else {
      SPrint (OptionName, sizeof (OptionName), L"Driver%04x", TempOptionPtr[Index]);
    }

    OptionPtr = BdsLibGetVariableAndSize (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  &OptionSize
                  );
    TempPtr = OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += EfiStrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    //
    // Notes: the description may will change base on the GetStringToken
    //
    if (EfiCompareMem (OptionDevicePath, DevicePath, EfiDevicePathSize (OptionDevicePath)) == 0) {
      if (EfiCompareMem (Description, String, EfiStrSize (Description)) == 0 ) {
        //
        // Got the option, so just return
        //
        gBS->FreePool (OptionPtr);
        gBS->FreePool (TempOptionPtr);
        return EFI_SUCCESS;
      } else {
        //
        // Option description changed, need update.
        //
        UpdateDescription = TRUE;
        gBS->FreePool (OptionPtr);
        break;
      }
    //
    // Vista will multiply the partition start lba by 512,
    // so only compare partition type and signature
    //
    } else if (MatchPartitionDevicePathNode (DevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath) &&
               BdsLibMatchFilePathDevicePathNode (DevicePath, OptionDevicePath)) {
      if (EfiCompareMem (Description, String, EfiStrSize (Description)) == 0 ) {
        //
        // Got the option, so just return
        //
        gBS->FreePool (OptionPtr);
        gBS->FreePool (TempOptionPtr);
        return EFI_SUCCESS;
      } else {
        //
        // Option description changed, need update.
        //
        UpdateDescription = TRUE;
        gBS->FreePool (OptionPtr);
        break;
      }
    }
    gBS->FreePool (OptionPtr);
  }

  Status  = BdsLibCreateLoadOption (
            DevicePath,
            String,
            OptionalData,
            OptionalDataSize,
            &OptionPtr,
            &OptionSize
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (OptionalData == NULL) {
    OptionalDataSize = 0;
  }

//  OptionSize          = sizeof (UINT32) + sizeof (UINT16) + EfiStrSize (String) + EfiDevicePathSize (DevicePath) + OptionalDataSize;
//  OptionPtr           = EfiLibAllocateZeroPool (OptionSize);
//  TempPtr             = OptionPtr;
//  *(UINT32 *) TempPtr = LOAD_OPTION_ACTIVE;
//  TempPtr += sizeof (UINT32);
//  *(UINT16 *) TempPtr = (UINT16) EfiDevicePathSize (DevicePath);
//  TempPtr += sizeof (UINT16);
//  EfiCopyMem (TempPtr, String, EfiStrSize (String));
//  TempPtr += EfiStrSize (String);
//  EfiCopyMem (TempPtr, DevicePath, EfiDevicePathSize (DevicePath));
//  TempPtr += EfiDevicePathSize (DevicePath);
//  EfiCopyMem (TempPtr, OptionalData, OptionalDataSize);

  if (UpdateDescription) {
    //
    // The number in option#### to be updated
    //
    RegisterOptionNumber = TempOptionPtr[Index];
  } else {
    //
    // get new BootOption number
    //
    GetNewBootOptionNo (
      TempOptionPtr,
      BootOptionNum,
      &RegisterOptionNumber
      );
  }

  if (*VariableName == 'B') {
    SPrint (OptionName, sizeof (OptionName), L"Boot%04x", RegisterOptionNumber);
  } else {
    SPrint (OptionName, sizeof (OptionName), L"Driver%04x", RegisterOptionNumber);
  }

    Status = gRT->SetVariable (
                    OptionName,
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    OptionSize,
                    OptionPtr
                    );

  if (EFI_ERROR (Status) || UpdateDescription) {
    gBS->FreePool (OptionPtr);
    gBS->FreePool (TempOptionPtr);
    return Status;
  }

  gBS->FreePool (OptionPtr);

  //
  // Update the option order variable
  //
  //
  // When a new device is detected, it should be added to the BootOrder variable in the Highest Priority position.
  //
  Status = gBS->LocateProtocol (
                  &gEfiBootOptionPolicyProtocolGuid,
                  NULL,
                  &BootOptionPolicy
                  );

  if (!EFI_ERROR (Status)) {
    BootOptionType = EFI_BOOT_DEV;
    BootOptionPolicy->FindPositionOfNewBootOption (
                        BootOptionPolicy,
                        DevicePath,
                        TempOptionPtr,
                        BootOptionType,
                        BootOptionNum,
                        RegisterOptionNumber,
                        &NewPosition
                        );
  } else {
    NewPosition = 0;
  }

  BdsLibNewBootOptionPolicy(
    &OptionOrderPtr,
    TempOptionPtr,
    BootOptionNum,
    RegisterOptionNumber,
    NewPosition
    );

  Status = gRT->SetVariable (
                  VariableName,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  (Index + 1) * sizeof (UINT16),
                  OptionOrderPtr
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (TempOptionPtr);
    gBS->FreePool (OptionOrderPtr);
    return Status;
  }

  gBS->FreePool (TempOptionPtr);
  gBS->FreePool (OptionOrderPtr);

  return EFI_SUCCESS;
}

BOOLEAN
BdsLibIsBiosCreatedOption (
  IN UINT8                  *Variable,
  IN UINTN                  VariableSize
  )
/*++

Routine Description:

 Check if option variable is created by BIOS or not.

Arguments:

  Variable             - Pointer to option variable
  VariableSize         - Option variable size

Returns:

  TRUE                 - Option varible is created by BIOS
  FALSE                - Option varible is not created by BIOS

--*/
{
  UINT8                     *Ptr;
  UINT32                    Attribute;
  UINT16                    FilePathSize;
  CHAR16                    *Description;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  VOID                      *LoadOptions;
  UINT32                    LoadOptionsSize;

  if (Variable == NULL || VariableSize == 0) {
    return FALSE;
  }

  Ptr = Variable;
  Attribute = *(UINT32 *) Variable;
  Ptr += sizeof (UINT32);
  FilePathSize = *(UINT16 *) Ptr;
  Ptr += sizeof (UINT16);
  Description = (CHAR16 *) Ptr;
  Ptr += EfiStrSize ((CHAR16 *) Ptr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  Ptr += FilePathSize;

  LoadOptions     = Ptr;
  LoadOptionsSize = (UINT32) (VariableSize - (UINTN) (Ptr - Variable));

  //
  // RC signature in optional data means it is created by BIOS.
  //
  if ((LoadOptionsSize == 2 || LoadOptionsSize == SHELL_OPTIONAL_DATA_SIZE) &&
      (EfiAsciiStrnCmp (LoadOptions, "RC", 2) == 0)) {
    return TRUE;
  }

  return FALSE;
}

BDS_COMMON_OPTION *
BdsLibVariableToOption (
  IN OUT EFI_LIST_ENTRY               *BdsCommonOptionList,
  IN  CHAR16                          *VariableName
  )
/*++

Routine Description:

  Build the boot#### or driver#### option from the VariableName, the
  build boot#### or driver#### will also be linked to BdsCommonOptionList

Arguments:

  BdsCommonOptionList - The header of the boot#### or driver#### option link list

  VariableName - EFI Variable name indicate if it is boot#### or driver####

Returns:

  BDS_COMMON_OPTION    - Get the option just been created

  NULL                 - Failed to get the new option

--*/
{
  UINT32                    Attribute;
  UINT16                    FilePathSize;
  UINT8                     *Variable;
  UINT8                     *TempPtr;
  UINTN                     VariableSize;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BDS_COMMON_OPTION         *Option;
  VOID                      *LoadOptions;
  UINT32                    LoadOptionsSize;
  CHAR16                    *Description;
  UINT8                     NumOff;
  //
  // Read the variable. We will never free this data.
  //
  Variable = BdsLibGetVariableAndSize (
              VariableName,
              &gEfiGlobalVariableGuid,
              &VariableSize
              );
  if (Variable == NULL) {
    return NULL;
  }
  //
  // Notes: careful defined the variable of Boot#### or
  // Driver####, consider use some macro to abstract the code
  //
  //
  // Get the option attribute
  //
  TempPtr   = Variable;
  Attribute = *(UINT32 *) Variable;
  TempPtr += sizeof (UINT32);

  //
  // Get the option's device path size
  //
  FilePathSize = *(UINT16 *) TempPtr;
  TempPtr += sizeof (UINT16);

  //
  // Get the option's description string
  //
  Description = (CHAR16 *) TempPtr;

  //
  // Get the option's description string size
  //
  TempPtr += EfiStrSize ((CHAR16 *) TempPtr);

  //
  // Get the option's device path
  //
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
  TempPtr += FilePathSize;

  LoadOptions     = TempPtr;
  LoadOptionsSize = (UINT32) (VariableSize - (UINTN) (TempPtr - Variable));

  //
  // The Console variables may have multiple device paths, so make
  // an Entry for each one.
  //
  Option = EfiLibAllocateZeroPool (sizeof (BDS_COMMON_OPTION));
  if (Option == NULL) {
    return NULL;
  }

  Option->Signature   = BDS_LOAD_OPTION_SIGNATURE;
  Option->DevicePath  = EfiLibAllocateZeroPool (EfiDevicePathSize (DevicePath));
  EfiCopyMem (Option->DevicePath, DevicePath, EfiDevicePathSize (DevicePath));
  Option->Attribute   = Attribute;
  Option->Description = EfiLibAllocateZeroPool (EfiStrSize (Description));
  EfiCopyMem (Option->Description, Description, EfiStrSize (Description));

  if (LoadOptionsSize == 0 || BdsLibIsBiosCreatedOption (Variable, VariableSize)) {
    Option->LoadOptions     = NULL;
    Option->LoadOptionsSize = 0;
  } else {
    if (DevicePathType (DevicePath) == BBS_DEVICE_PATH &&
        DevicePathSubType (DevicePath) == BBS_BBS_DP &&
        LoadOptionsSize > sizeof (BBS_TABLE) + sizeof (UINT16)) {
      //
      // For legacy option, optional data (device path) is only used for firmware. Just set BBS_TABLE and bbs index in LoadOptions.
      //
      LoadOptionsSize = sizeof (BBS_TABLE) + sizeof (UINT16);
    }

    Option->LoadOptions = EfiLibAllocateZeroPool (LoadOptionsSize);
    EfiCopyMem (Option->LoadOptions, LoadOptions, LoadOptionsSize);
    Option->LoadOptionsSize = LoadOptionsSize;
  }

  //
  // Get the value from VariableName Unicode string
  // since the ISO standard assumes ASCII equivalent abbreviations, we can be safe in converting this
  // Unicode stream to ASCII without any loss in meaning.
  //
  if (*VariableName == 'B') {
    NumOff = sizeof (L"Boot")/sizeof(CHAR16) -1 ;
    Option->BootCurrent =  (VariableName[NumOff]  -'0') * 0x1000;
    Option->BootCurrent += (VariableName[NumOff+1]-'0') * 0x100;
    Option->BootCurrent += (VariableName[NumOff+2]-'0') * 0x10;
    Option->BootCurrent += (VariableName[NumOff+3]-'0');
  }
  //
  // Insert active entry to BdsDeviceList
  //
  if ((Option->Attribute & LOAD_OPTION_ACTIVE) == LOAD_OPTION_ACTIVE) {
    InsertTailList (BdsCommonOptionList, &Option->Link);
    gBS->FreePool (Variable);
    return Option;
  }

  gBS->FreePool (Variable);
  gBS->FreePool (Option);
  return NULL;

}

EFI_STATUS
BdsLibBuildOptionFromVar (
  IN  EFI_LIST_ENTRY                  *BdsCommonOptionList,
  IN  CHAR16                          *VariableName
  )
/*++

Routine Description:

  Process BootOrder, or DriverOrder variables, by calling
  BdsLibVariableToOption () for each UINT16 in the variables.

Arguments:

  BdsCommonOptionList - The header of the option list base on variable
                        VariableName

  VariableName - EFI Variable name indicate the BootOrder or DriverOrder

Returns:

  EFI_SUCCESS - Success create the boot option or driver option list

  EFI_OUT_OF_RESOURCES - Failed to get the boot option or driver option list

--*/
{
  UINT16            *OptionOrder;
  UINTN             OptionOrderSize;
  UINTN             Index;
  BDS_COMMON_OPTION *Option;
  CHAR16            OptionName[20];

  //
  // Zero Buffer in order to get all BOOT#### variables
  //
  EfiZeroMem (OptionName, sizeof (OptionName));

  //
  // Read the BootOrder, or DriverOrder variable.
  //
  OptionOrder = BdsLibGetVariableAndSize (
                  VariableName,
                  &gEfiGlobalVariableGuid,
                  &OptionOrderSize
                  );
  if (OptionOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < OptionOrderSize / sizeof (UINT16); Index++) {
    if (*VariableName == 'B') {
      SPrint (OptionName, sizeof (OptionName), L"Boot%04x", OptionOrder[Index]);
    } else {
      SPrint (OptionName, sizeof (OptionName), L"Driver%04x", OptionOrder[Index]);
    }

    Option              = BdsLibVariableToOption (BdsCommonOptionList, OptionName);
    Option->BootCurrent = OptionOrder[Index];

  }

  gBS->FreePool (OptionOrder);

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibGetBootMode (
  OUT EFI_BOOT_MODE       *BootMode
  )
/*++

Routine Description:

  Get boot mode by looking up configuration table and parsing HOB list

Arguments:

  BootMode - Boot mode from PEI handoff HOB.

Returns:

  EFI_SUCCESS - Successfully get boot mode

  EFI_NOT_FOUND - Can not find the current system boot mode

--*/
{
  VOID        *HobList;
  EFI_STATUS  Status;

  //
  // Get Hob list
  //
  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Hob list not found\n"));
    *BootMode = 0;
    return EFI_NOT_FOUND;
  }

  Status = GetHobBootMode (HobList, BootMode);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

VOID *
BdsLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
/*++

Routine Description:

  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

Arguments:

  Name       - String part of EFI variable name

  VendorGuid - GUID part of EFI variable name

  VariableSize - Returns the size of the EFI variable that was read

Returns:

  Dynamically allocated memory that contains a copy of the EFI variable.
  Caller is responsible freeing the buffer.

  NULL - Variable was not read

--*/
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  VOID        *Buffer;

  Buffer = NULL;

  //
  // Pass in a zero size buffer to find the required buffer size.
  //
  BufferSize  = 0;
  Status      = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //
    Buffer = EfiLibAllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      return NULL;
    }
    //
    // Read variable into the allocated buffer.
    //
    Status = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      BufferSize = 0;
      gBS->FreePool (Buffer);
      Buffer = NULL;
    }
  }

  *VariableSize = BufferSize;
  return Buffer;
}

EFI_DEVICE_PATH_PROTOCOL *
BdsLibDelPartMatchInstance (
  IN     EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN     EFI_DEVICE_PATH_PROTOCOL  *Single
  )
/*++

Routine Description:

  Delete the instance in Multi which matches partly with Single instance

Arguments:

  Multi        - A pointer to a multi-instance device path data structure.

  Single       - A pointer to a single-instance device path data structure.

Returns:

  This function will remove the device path instances in Multi which partly
  match with the Single, and return the result device path. If there is no
  remaining device path as a result, this function will return NULL.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempNewDevicePath;
  UINTN                     InstanceSize;
  UINTN                     SingleDpSize;
  UINTN                     Size;

  NewDevicePath     = NULL;
  TempNewDevicePath = NULL;

  if (Multi == NULL || Single == NULL) {
    return Multi;
  }

  Instance        =  EfiDevicePathInstance (&Multi, &InstanceSize);
  SingleDpSize    =  EfiDevicePathSize (Single) - END_DEVICE_PATH_LENGTH;
  InstanceSize    -= END_DEVICE_PATH_LENGTH;

  while (Instance != NULL) {

    Size = (SingleDpSize < InstanceSize) ? SingleDpSize : InstanceSize;

    if ((EfiCompareMem (Instance, Single, Size) != 0)) {
      //
      // Append the device path instance which does not match with Single
      //
      TempNewDevicePath = NewDevicePath;
      NewDevicePath = EfiAppendDevicePathInstance (NewDevicePath, Instance);
      if (TempNewDevicePath != NULL) {
        gBS->FreePool(TempNewDevicePath);
      }
    }
    gBS->FreePool (Instance);
    Instance = EfiDevicePathInstance (&Multi, &InstanceSize);
    InstanceSize  -= END_DEVICE_PATH_LENGTH;
  }

  return NewDevicePath;
}

BOOLEAN
BdsLibMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  )
/*++

Routine Description:

  Function compares a device path data structure to that of all the nodes of a
  second device path instance.

Arguments:

  Multi        - A pointer to a multi-instance device path data structure.

  Single       - A pointer to a single-instance device path data structure.

Returns:

  TRUE   - If the Single is contained within Multi

  FALSE  - The Single is not match within Multi


--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;

  if (!Multi || !Single) {
    return FALSE;
  }

  DevicePath      = Multi;
  DevicePathInst  = EfiDevicePathInstance (&DevicePath, &Size);

  //
  // Search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst != NULL) {
    //
    // If the single device path is found in multiple device paths,
    // return success
    //
    if (EfiCompareMem (Single, DevicePathInst, Size) == 0) {
      gBS->FreePool (DevicePathInst);
      return TRUE;
    }

    gBS->FreePool (DevicePathInst);
    DevicePathInst = EfiDevicePathInstance (&DevicePath, &Size);
  }

  return FALSE;
}

EFI_STATUS
BdsLibOutputStrings (
  IN EFI_SIMPLE_TEXT_OUT_PROTOCOL   *ConOut,
  ...
  )
/*++

Routine Description:

  This function prints a series of strings.

Arguments:

  ConOut               - Pointer to EFI_SIMPLE_TEXT_OUT_PROTOCOL

  ...                  - A variable argument list containing series of strings,
                         the last string must be NULL.

Returns:

  EFI_SUCCESS          - Success print out the string using ConOut.

  EFI_STATUS           - Return the status of the ConOut->OutputString ().

--*/
{
  VA_LIST     args;
  EFI_STATUS  Status;
  CHAR16      *String;

  Status = EFI_SUCCESS;
  VA_START (args, ConOut);

  while (!EFI_ERROR (Status)) {
    //
    // If String is NULL, then it's the end of the list
    //
    String = VA_ARG (args, CHAR16 *);
    if (!String) {
      break;
    }

    Status = ConOut->OutputString (ConOut, String);

    if (EFI_ERROR (Status)) {
      break;
    }
  }

  return Status;
}

EFI_STATUS
BdsLibNewBootOptionPolicy(
  UINT16                                     **NewBootOrder,
  UINT16                                     *OldBootOrder,
  UINTN                                      OldBootOrderCount,
  UINT16                                     NewBootOptionNo,
  UINTN                                      Policy
)
/*++

Routine Description:

  insert new BootOption number to BootOrder by policy

Arguments:

  NewBootOrder                    the new BootOrder
  OldBootOrder                    current BootOrder
  OldBootOrderCount               the count of elements in current BootOrder
  NewBootOptionNo                 the number of BootOption that would be insert in BootOrder
  Policy                          the index of BootOrder that the NewBootOptionNo would be insert in .

Returns:

  EFI_SUCCESS - Success insert new BootOption number in BootOrder

--*/
{
  UINTN                                      Front = 0;
  UINTN                                      Back  = 0;
  UINT16                                     *TempPtr1 = NULL;
  UINT16                                     *TempPtr2 = NULL;
  *NewBootOrder = EfiLibAllocateZeroPool ((OldBootOrderCount + 1) * sizeof (UINT16));
  if (NULL == *NewBootOrder) {
    return EFI_OUT_OF_RESOURCES;
  }

  (*NewBootOrder)[Policy] = NewBootOptionNo;

  if (OldBootOrderCount > 0) {
    //
    // in front of the Policy
    //
    if (Policy > 0) {
      Front = Policy;
      Front = Front * sizeof(UINT16);
      EfiCopyMem (*NewBootOrder, OldBootOrder, Front);
    }

    //
    // in back of the Policy
    //
    if (Policy < OldBootOrderCount) {
      TempPtr2 = OldBootOrder + Policy;
      Back = OldBootOrderCount - Policy;
      Back = Back * sizeof(UINT16);
      ++Policy;
      TempPtr1  = (*NewBootOrder) + Policy;
      EfiCopyMem (TempPtr1, TempPtr2, Back);
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
GetNewBootOptionNo (
  IN     UINT16                                     *BootOrder,
  IN     UINTN                                      BootOptionNum,
  IN OUT UINT16                                     *CurrentBootOptionNo
)
/*++

Routine Description:

  Get the number of new BootOption

Arguments:

  BootOrder                         The pointer of BootOrder
  BootOptionNum                     The count of BootOption
  CurrentBootOptionNo               The number of new BootOption

Returns:

  EFI_SUCCESS - Success get new BootOption number

--*/
{
  UINT16                                     Index = 0;
  UINT16                                     BootOrderIndex;
  BOOLEAN                                    IndexNotFound = TRUE;

  for (Index = 0; Index < BootOptionNum; Index++) {
    IndexNotFound = TRUE;
    for (BootOrderIndex = 0; BootOrderIndex < BootOptionNum; BootOrderIndex++) {
      if (BootOrder[BootOrderIndex] == Index) {
        IndexNotFound = FALSE;
        break;
      }
    }

    if (!IndexNotFound) {
      continue;
    } else {
      break;
    }
  }

  *CurrentBootOptionNo = Index;

  return EFI_SUCCESS;
}


//
//  Following are BDS Lib functions which  contain all the code about setup browser reset reminder feature.
//  Setup Browser reset reminder feature is that an reset reminder will be given before user leaves the setup browser  if
//  user change any option setting which needs a reset to be effective, and  the reset will be applied according to  the user selection.
//

VOID
EnableResetReminderFeature (
  VOID
  )
/*++

Routine Description:

  Enable the setup browser reset reminder feature.
  This routine is used in platform tip. If the platform policy need the feature, use the routine to enable it.

Arguments:

  VOID

Returns:

  VOID

--*/
{
  mFeaturerSwitch = TRUE;
}

VOID
DisableResetReminderFeature (
  VOID
  )
/*++

Routine Description:

  Disable the setup browser reset reminder feature.
  This routine is used in platform tip. If the platform policy do not want the feature, use the routine to disable it.

Arguments:

  VOID

Returns:

  VOID

--*/
{
  mFeaturerSwitch = FALSE;
}

VOID
EnableResetRequired (
  VOID
  )
/*++

Routine Description:

   Record the info that  a reset is required.
   A  module boolean variable is used to record whether a reset is required.

Arguments:

  VOID

Returns:

  VOID

--*/
{
  mResetRequired = TRUE;
}

VOID
DisableResetRequired (
  VOID
  )
/*++

Routine Description:

   Record the info that  no reset is required.
   A  module boolean variable is used to record whether a reset is required.

Arguments:

  VOID

Returns:

  VOID

--*/
{
  mResetRequired = FALSE;
}

BOOLEAN
IsResetReminderFeatureEnable (
  VOID
  )
/*++

Routine Description:

  Check whether platform policy enable the reset reminder feature. The default is enabled.

Arguments:

  VOID

Returns:

  VOID

--*/
{
  return mFeaturerSwitch;
}

BOOLEAN
IsResetRequired (
  VOID
  )
/*++

Routine Description:

  Check if  user changed any option setting which needs a system reset to be effective.

Arguments:

  VOID

Returns:

  VOID

--*/
{
  return mResetRequired;
}

VOID
SetupResetReminder (
  VOID
  )
/*++

Routine Description:

  Check whether a reset is needed, and finish the reset reminder feature.
  If a reset is needed, Popup a menu to notice user, and finish the feature
  according to the user selection.

Arguments:

  VOID

Returns:

  VOID

--*/
{
//#ifndef UEFI_SETUP_SUPPORT
#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
  EFI_STATUS                    Status;
  EFI_FORM_BROWSER_PROTOCOL     *Browser;
#endif
  EFI_INPUT_KEY                 Key;
  CHAR16                        *StringBuffer1;
  CHAR16                        *StringBuffer2;


  //
  //check any reset required change is applied? if yes, reset system
  //
  if (IsResetReminderFeatureEnable ()) {
    if (IsResetRequired ()) {

//#ifndef UEFI_SETUP_SUPPORT
#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
      Status = gBS->LocateProtocol (
                      &gEfiFormBrowserProtocolGuid,
                      NULL,
                      &Browser
                      );
#endif

      StringBuffer1 = EfiLibAllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16));
      ASSERT (StringBuffer1 != NULL);
      StringBuffer2 = EfiLibAllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16));
      ASSERT (StringBuffer2 != NULL);
      EfiStrCpy (StringBuffer1, L"Configuration changed. Reset to apply it Now ? ");
      EfiStrCpy (StringBuffer2, L"Enter (YES)  /   Esc (NO)");
      //
      // Popup a menu to notice user
      //
      do {
//#ifndef UEFI_SETUP_SUPPORT
#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
        Browser->CreatePopUp (2, TRUE, 0, NULL, &Key, StringBuffer1, StringBuffer2);
#else
        IfrLibCreatePopUp (2, &Key, StringBuffer1, StringBuffer2);
#endif
      } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

      gBS->FreePool (StringBuffer1);
      gBS->FreePool (StringBuffer2);
      //
      // If the user hits the YES Response key, reset
      //
      if ((Key.UnicodeChar == CHAR_CARRIAGE_RETURN)) {
        gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
      }
      gST->ConOut->ClearScreen (gST->ConOut);
    }
  }
}
//#ifndef UEFI_SETUP_SUPPORT
#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
EFI_STATUS
BdsLibGetHiiHandles (
  IN     EFI_HII_PROTOCOL *Hii,
  IN OUT UINT16           *HandleBufferLength,
  OUT    EFI_HII_HANDLE   **HiiHandleBuffer
  )
/*++

Routine Description:

  Determines the handles that are currently active in the database.
  It's the caller's responsibility to free handle buffer.

Arguments:

  This                  - A pointer to the EFI_HII_PROTOCOL instance.
  HandleBufferLength    - On input, a pointer to the length of the handle buffer. On output,
                          the length of the handle buffer that is required for the handles found.
  HiiHandleBuffer       - Pointer to an array of EFI_HII_PROTOCOL instances returned.

Returns:

  EFI_SUCCESS           - Get an array of EFI_HII_PROTOCOL instances successfully.
  EFI_INVALID_PARAMETER - Hii is NULL.
  EFI_NOT_FOUND         - Database not found.

--*/
{
  UINT16      TempBufferLength;
  EFI_STATUS  Status;

  TempBufferLength = 0;

  //
  // Try to find the actual buffer size for HiiHandle Buffer.
  //
  Status = Hii->FindHandles (Hii, &TempBufferLength, *HiiHandleBuffer);

  if (Status == EFI_BUFFER_TOO_SMALL) {
      *HiiHandleBuffer = EfiLibAllocateZeroPool (TempBufferLength);
      Status = Hii->FindHandles (Hii, &TempBufferLength, *HiiHandleBuffer);
      //
      // we should not fail here.
      //
      ASSERT_EFI_ERROR (Status);
  }

  *HandleBufferLength = TempBufferLength;

  return Status;

}
#endif

VOID
EFIAPI
BdsSetMemoryTypeInformationVariable (
  EFI_EVENT  Event,
  VOID       *Context
  )
/*++

Routine Description:

  This routine is a notification function for legayc boot or exit boot
  service event. It will adjust the memory information for different
  memory type and save them into the variables for next boot

Arguments:

  Event    - The event that triggered this notification function
  Context  - Pointer to the notification functions context

Returns:

  None.

--*/
{
  EFI_STATUS                   Status;
  EFI_MEMORY_TYPE_INFORMATION  *PreviousMemoryTypeInformation;
  EFI_MEMORY_TYPE_INFORMATION  *CurrentMemoryTypeInformation;
  UINTN                        VariableSize;
  BOOLEAN                      UpdateRequired;
  UINTN                        Index;
  UINTN                        Index1;
  UINT32                       Previous;
  UINT32                       Current;
  UINT32                       Next;
  VOID                         *HobList;

  UpdateRequired = FALSE;

  //
  // Retrieve the current memory usage statistics.  If they are not found, then
  // no adjustments can be made to the Memory Type Information variable.
  //
  Status = EfiLibGetSystemConfigurationTable (
             &gEfiMemoryTypeInformationGuid,
             &CurrentMemoryTypeInformation
             );
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Get the Memory Type Information settings from Hob if they exist,
  // PEI is responsible for getting them from variable and build a Hob to save them.
  // If the previous Memory Type Information is not available, then set defaults
  //
  EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  Status = GetNextGuidHob (&HobList, &gEfiMemoryTypeInformationGuid, &PreviousMemoryTypeInformation, &VariableSize);
  if (EFI_ERROR (Status) || PreviousMemoryTypeInformation == NULL) {
  	//
  	// If Platform has not built Memory Type Info into the Hob, just return.
  	//
    return;
  }

  //
  // Use a heuristic to adjust the Memory Type Information for the next boot
  //
  for (Index = 0; PreviousMemoryTypeInformation[Index].Type != EfiMaxMemoryType; Index++) {

    Current = 0;
    for (Index1 = 0; CurrentMemoryTypeInformation[Index1].Type != EfiMaxMemoryType; Index1++) {
      if (PreviousMemoryTypeInformation[Index].Type == CurrentMemoryTypeInformation[Index1].Type) {
        Current = CurrentMemoryTypeInformation[Index1].NumberOfPages;
        break;
      }
    }

    if (CurrentMemoryTypeInformation[Index1].Type == EfiMaxMemoryType) {
      continue;
    }

    Previous = PreviousMemoryTypeInformation[Index].NumberOfPages;

    //
    // Write next varible to 125% * current when current size is larger than previous (increase)
    // or smaller than previous/2 (shrink).
    // Inconsistent Memory Reserved across bootings may lead to S4 fail.
    //
    if (Current > Previous || Current < (Previous >> 1)) {
      Next = Current + (Current >> 2);
    } else {
      Next = Previous;
    }
    if (Next > 0 && Next < 4) {
      Next = 4;
    }

    if (Next != Previous) {
      PreviousMemoryTypeInformation[Index].NumberOfPages = Next;
      UpdateRequired = TRUE;
    }

  }

  //
  // If any changes were made to the Memory Type Information settings, then set the new variable value
  //
  if (UpdateRequired) {
    Status = gRT->SetVariable (
          EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
          &gEfiMemoryTypeInformationGuid,
          EFI_VARIABLE_NON_VOLATILE  | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
          VariableSize,
          PreviousMemoryTypeInformation
          );
  }

  return;
}

VOID
EFIAPI
BdsLibSaveMemoryTypeInformation (
  VOID
  )
/*++

Routine Description:

  This routine register a function to adjust the different type memory page number just before booting
  and save the updated info into the variable for next boot to use

Arguments:

  None

Returns:

  None.

--*/
{
  EFI_STATUS                   Status;
  EFI_EVENT                    ReadyToBootEvent;

  Status = EfiCreateEventReadyToBoot (
           EFI_TPL_CALLBACK,
           BdsSetMemoryTypeInformationVariable,
           NULL,
           &ReadyToBootEvent
           );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR,"Bds Set Memory Type Informationa Variable Fails\n"));
  }

}

EFI_TPL
BdsLibGetCurrentTpl (
  VOID
  )
/*++

Routine Description:

  return the current TPL, copied from the EDKII glue lib.

Arguments:

  VOID

Returns:

  Current TPL

--*/
{
  EFI_TPL                 Tpl;

  Tpl = gBS->RaiseTPL (EFI_TPL_HIGH_LEVEL);
  gBS->RestoreTPL (Tpl);

  return Tpl;
}

EFI_STATUS
BdsLibUpdateInvalidBootOrder (
  IN OUT UINT16        **BootOrderPtr,
  IN     UINTN         Index,
  IN OUT UINTN         *BootOrderSize
  )
/*++

Routine Description:

  The *BootOrderPtr[Index] hasn't Boot option, so delete the *BootOrderPtr[Index] and
  set the new L"BootOrder" variable

Arguments:

  BootOrderPtr  - IN  - the double pointer point to the original BootOrder
                  OUT - the double pointer point to the Update BootOrder
  Index         - Indicate the Offset of BootOrder is invalid
  BootOrderSize - IN  - The pointer of oringinal Boot Order size
                  OUT - The pointer of udpate Boot Order size
Returns:

  EFI_SUCCESS           - Udate the New L"BootOrder" variable successful
  EFI_INVALID_PARAMETER - The inupt parameter is invalid

--*/
{
  UINTN                     Index2;
  UINTN                     Index3;
  UINT16                    *BootOrder;
  EFI_STATUS                Status;

  if (BootOrderPtr == NULL || *BootOrderPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootOrderSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BootOrder = *BootOrderPtr;
  BootOrder[Index] = 0xffff;
  Index2 = 0;
  for (Index3 = 0; Index3 < *BootOrderSize / sizeof (UINT16); Index3++) {
    if (BootOrder[Index3] != 0xffff) {
      BootOrder[Index2] = BootOrder[Index3];
      Index2 ++;
    }
  }
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Index2 * sizeof (UINT16),
                  BootOrder
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->FreePool (BootOrder);

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                BootOrderSize
                );
  *BootOrderPtr = BootOrder;

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibStartSetupUtility (
  BOOLEAN       PasswordCheck
  )
/*++

Routine Description:
  Start Setup Utility

Arguments:

Returns:

--*/
{
  EFI_STATUS                            Status;
  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid,
                                NULL,
                                &SetupUtility
                                );
  if (!EFI_ERROR (Status)) {
    if (PasswordCheck) {
      Status = SetupUtility->PowerOnSecurity (SetupUtility);
    } else {
#ifdef SETUP_SUPPORT
#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
      if (BdsLibIsBootOrderHookEnabled ()) {
        BdsLibRestoreBootOrderFromPhysicalBootOrder ();
      }
#endif
      Status = SetupUtility->StartEntry (SetupUtility);
#endif
    }
  }

  return Status;
}

EFI_STATUS
BdsLibDisplayDeviceReplace (
  OUT    BOOLEAN          *SkipOriginalCode
  )
/*++

Routine Description:

  check need update platform display device or not

Arguments:

  SkipOriginalCode - TRUE: skip platform display device update
                     FALSE: need do platform display device update
Returns:

  EFI_SUCCESS -  Get Platform display device policy successful
  Other       -  Cannot get Platform display device policy successful

--*/
{
  EFI_STATUS                    Status;
  OEM_SERVICES_PROTOCOL         *OemServices;


  if (SkipOriginalCode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *SkipOriginalCode = FALSE;

  Status = gBS->LocateProtocol (
                  &gOemServicesProtocolGuid,
                  NULL,
                  &OemServices
                  );

  if (!EFI_ERROR (Status)) {
    Status = OemServices->Funcs[COMMON_BOOT_DISPLAY_DEVICE_REPLACE] (
                            OemServices,
                            COMMON_BOOT_DISPLAY_DEVICE_REPLACE_ARG_COUNT,
                            SkipOriginalCode
                            );
  }

  return Status;
}

EFI_STATUS
BdsLibOnStartOfBdsDiagnostics (
  VOID
  )
{
  EFI_STATUS                        Status;

  Status = gBS->InstallProtocolInterface (
                  &mBdsImageHandle,
                  &gEfiStartOfBdsDiagnosticsProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibOnEndOfBdsBootSelection (
  void
  )
{
  EFI_STATUS                        Status;

  //
  // PostCode = 0x28, End of boot selection
  //
  POSTCODE (BDS_END_OF_BOOT_SELECTION);
  Status = gBS->InstallProtocolInterface (
                  &mBdsImageHandle,
                  &gEfiEndOfBdsBootSelectionProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  gDS->Dispatch();

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibShowOemStringInTextMode (
  IN BOOLEAN                       AfterSelect,
  IN UINT8                         SelectedStringNum
  )
{
  EFI_STATUS                        Status;
  UINTN                             LocX;
  UINTN                             LocY;
  UINTN                             CursorRow;
  UINTN                             CursorCol;
  CHAR16                            *StringData;
  UINTN                             StringCount;
  UINTN                             Index;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL  *Badging;
  UINTN                             MaxX;
  UINTN                             MaxY;
  EFI_UGA_PIXEL                     Foreground;
  EFI_UGA_PIXEL                     Background;

  CursorRow = gST->ConOut->Mode->CursorRow;
  CursorCol = gST->ConOut->Mode->CursorColumn;

  LocX = 0;
  LocY = 0;

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, &Badging);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = Badging->GetStringCount(Badging, &StringCount);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = gST->ConOut->QueryMode (
                          gST->ConOut,
                          gST->ConOut->Mode->Mode,
                          &MaxX,
                          &MaxY
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MaxY = MaxY - StringCount - 1;
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, MaxY);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  for (Index = 0; Index < StringCount ; Index++) {

    if (Badging->GetOemString(Badging, Index, AfterSelect, SelectedStringNum, &StringData, &LocX, &LocY, &Foreground, &Background) ) {
      BdsLibOutputStrings (gST->ConOut, L"\n\r", StringData, NULL);
      gBS->FreePool (StringData);
    }

  }
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, CursorCol);

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibSkipEbcDispatch (
  VOID
  )
/*++

Routine Description:

  Unininstall all of gEfiEbcProtocolGuid protocols
Arguments:

  NOne

Returns:
  EFI_SUCCESS - Uninstall all of gEfiEbcProtocolGuid protocols
  Other       - Some error occured in this function.
--*/
{
  EFI_EBC_PROTOCOL            *EbcProtocol;
  EFI_STATUS                  Status;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       NumHandles;
  UINTN                       Index;

  HandleBuffer  = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiEbcProtocolGuid,
                  NULL,
                  &NumHandles,
                  &HandleBuffer
                  );
  if (Status == EFI_SUCCESS) {
    //
    // Loop through the handles
    //
    for (Index = 0; Index < NumHandles; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiEbcProtocolGuid,
                      (VOID **) &EbcProtocol
                      );
      if (Status == EFI_SUCCESS) {
        gBS->UninstallProtocolInterface (
               HandleBuffer[Index],
               &gEfiEbcProtocolGuid,
               EbcProtocol
               );
      }
    }
  }

  if (HandleBuffer != NULL) {
    gBS->FreePool (HandleBuffer);
    HandleBuffer = NULL;
  }

  return Status;
}

BOOLEAN
BdsLibIsLegacyBootOption (
  IN UINT8                 *BootOptionVar,
  OUT BBS_TABLE            **BbsEntry,
  OUT UINT16               *BbsIndex
  )
{
  UINT8                     *Ptr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   Ret;
  UINT16                    DevPathLen;

  Ptr = BootOptionVar;
  Ptr += sizeof (UINT32);
  DevPathLen = *(UINT16 *) Ptr;
  Ptr += sizeof (UINT16);
  Ptr += EfiStrSize ((UINT16 *) Ptr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    Ptr += DevPathLen;
    *BbsEntry = (BBS_TABLE *) Ptr;
    Ptr += sizeof (BBS_TABLE);
    *BbsIndex = *(UINT16 *) Ptr;
    Ret       = TRUE;
  } else {
    *BbsEntry = NULL;
    Ret       = FALSE;
  }

  return Ret;
}

UINTN
BdsLibGetBootType (
  VOID
  )
/*++

Routine Description:

  Get current boot type (DUAL_BOOT_TYPE, LEGACY_BOOT_TYPE, or EFI_BOOT_TYPE)

Arguments:

  NOne

Returns:

   Current supported boot type.

--*/
{
  UINTN                        Size;
  UINT8                        BootType;

  Size = sizeof (UINT8);
  BootType = DUAL_BOOT_TYPE;
  gRT->GetVariable (
         L"BootType",
         &mSystemConfigurationGuid,
         NULL,
         &Size,
         &BootType
         );

  return BootType;

}

STATIC
INTN
StriCmp (
  IN  CHAR16      *Str1,
  IN  CHAR16      *Str2
  )
/*++

Routine Description:

  Performs a case-insensitive comparison of two Null-terminated strings.

Arguments:

  Str1   - A pointer to a Null-terminated string.
  Str2   - A pointer to a Null-terminated string.

Returns:

  0      - Str1 is equivalent to Str2.
  > 0    - Str1 is lexically greater than Str2.
  < 0    - Str1 is lexically less than Str2.

--*/
{
  EFI_STATUS                         Status;
#if ((EFI_SPECIFICATION_VERSION >= 0x00020000 && defined(EFI_UNICODE_COLLATION2_PROTOCOL_GUID)) || (EFI_SPECIFICATION_VERSION >= 0x0002000A))
  EFI_UNICODE_COLLATION2_PROTOCOL    *UnicodeCollation;
#else
  EFI_UNICODE_COLLATION_PROTOCOL     *UnicodeCollation;
#endif



  Status = gBS->LocateProtocol (
#if ((EFI_SPECIFICATION_VERSION >= 0x00020000 && defined(EFI_UNICODE_COLLATION2_PROTOCOL_GUID)) || (EFI_SPECIFICATION_VERSION >= 0x0002000A))
                  &gEfiUnicodeCollation2ProtocolGuid,
#else
                  &gEfiUnicodeCollationProtocolGuid,
#endif
                  NULL,
                  (VOID**) &UnicodeCollation
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    //
    // EFI_UNICODE_COLLATION_PROTOCOL or  EFI_UNICODE_COLLATION2_PROTOCOL must be existence. If the protocol
    // doesn't exist, EFI DEBUG will be asserted and return -1 to indicate the two strings are different.
    //
    return -1;
  }

  return UnicodeCollation->StriColl (UnicodeCollation, Str1, Str2);
}


BOOLEAN
BdsLibMatchFilePathDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *FirstDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL   *SecondDevicePath
  )
/*++

Routine Description:
  Check the two device paths whether have the same file path.

Arguments:
  FirstDevicePath   - First multi device path instances which need to check
  SecondDevicePath  - Second multi device path instances which need to check

Returns:
  TRUE  - There is a matched device path instance
  FALSE -There is no matched device path instance


--*/
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FirstFileDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *SecondFileDevicePath;


  if ((FirstDevicePath == NULL) || (SecondDevicePath == NULL)) {
    return FALSE;
  }
  FirstFileDevicePath  = NULL;
  SecondFileDevicePath = NULL;
  //
  // find the file device path node
  //
  DevicePath = FirstDevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_FILEPATH_DP)
        ) {
      FirstFileDevicePath = DevicePath;
      break;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (FirstFileDevicePath == NULL) {
    return FALSE;
  }

  DevicePath = SecondDevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_FILEPATH_DP)
        ) {
      SecondFileDevicePath = DevicePath;
      break;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }


  if (SecondFileDevicePath == NULL) {
    return FALSE;
  }

  if (StriCmp ((CHAR16 *) (FirstFileDevicePath + 1),  (CHAR16 *) (SecondFileDevicePath + 1)) == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
BOOLEAN
IsAdministerSecureBootSupport (
  VOID
  )
/*++

Routine Description:

  Check is whether support administer secure boot or not.

Arguments:

  None

Returns:

  TRUE    - Administer Secure boot is enabled.
  FALSE   - Administer Secure boot is disabled.

--*/
{
  UINT8                          AdmiSecureBoot;
  EFI_STATUS                     Status;
  BOOLEAN                        AdmiSecureBootEnable;
  UINTN                          BufferSize;

  AdmiSecureBootEnable = FALSE;

  BufferSize = sizeof (UINT8);
  Status = gRT->GetVariable (
                   EFI_ADMINISTER_SECURE_BOOT_NAME,
                   &gEfiGenericVariableGuid,
                   NULL,
                   &BufferSize,
                   &AdmiSecureBoot
                   );
  if (!EFI_ERROR (Status) && AdmiSecureBoot == 1) {
    AdmiSecureBootEnable = TRUE;
  }

  return AdmiSecureBootEnable;

}

EFI_STATUS
BdsLibDeleteInvalidBootOptions (
  VOID
  )
/*++

Routine Description:

  Delete all of invalid boot options which option number is saved
  in L"BootOrder" variable.

Arguments:

  None

Returns:

  EFI_SUCCESS  - Delete invalid boot options successful.
  Other        - Delete invalid boot options failed.

--*/
{
  UINT16                    *BootOrder;
  UINT16                    FilePathListLength;
  UINT16                    DevicePathSize;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT8                     *TempPtr;
  CHAR16                    *Description;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_SUCCESS;
  }
  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      Index++;
      continue;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32);
    FilePathListLength = *((UINT16 *) TempPtr);
    //
    // If file path length is lager than whole variable size, it indicates
    // this boot option is invalid.
    //
    if (FilePathListLength > BootOptionSize) {
      BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }
    TempPtr += sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += EfiStrSize ((CHAR16 *) TempPtr);
    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    DevicePathSize = sizeof (EFI_DEVICE_PATH_PROTOCOL);
    while (!EfiIsDevicePathEnd (DevicePath)) {
      if (EfiDevicePathNodeLength (DevicePath) == 0) {
        break;
      }
      DevicePathSize += EfiDevicePathNodeLength (DevicePath);
      DevicePath = EfiNextDevicePathNode (DevicePath);
    }
    //
    // Delete boot option if the file path list length is incorrect.
    //
    if (!EfiIsDevicePathEnd (DevicePath) || FilePathListLength != DevicePathSize) {
      BdsDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      gBS->FreePool (BootOptionVar);
      continue;
    }
    gBS->FreePool (BootOptionVar);
    Index++;
  }

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  gBS->FreePool (BootOrder);

  return Status;
}
VOID
BdsLibAsciiToUnicodeSize (
  IN UINT8              *a,
  IN UINTN              Size,
  OUT UINT16            *u
  )
/*++

  Routine Description:

    Translate the first n characters of an Ascii string to
    Unicode characters. The count n is indicated by parameter
    Size. If Size is greater than the length of string, then
    the entire string is translated.

  Arguments:

    a         - Pointer to input Ascii string.
    Size      - The number of characters to translate.
    u         - Pointer to output Unicode string buffer.

  Returns:

    None

--*/
{
  UINTN i;

  i = 0;
  while (a[i] != 0) {
    u[i] = (CHAR16) a[i];
    if (i == Size) {
      break;
    }

    i++;
  }
  u[i] = 0;
}

EFI_STATUS
BdsLibUpdateAtaString (
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  )
/*++

Routine Description:
  Function to update the ATA strings into Model Name -- Size

Arguments:

Returns:
  Will return model name and size (or ATAPI if non-ATA)
--*/
{
  CHAR8                                     *TempString;
//  UINT32                                    DriveSize;
//  CHAR16                                    SizeString[20];
  UINT16                                    Index;
//  UINT16                                    Index1;
  CHAR8                                     Temp8;

  TempString = EfiLibAllocateZeroPool (0x100);

  EfiCopyMem (
    TempString,
    IdentifyDriveInfo->AtapiData.ModelName,
    sizeof(IdentifyDriveInfo->AtapiData.ModelName)
    );

  //
  // Swap the IDE string since Identify Drive format is inverted
  //
  Index = 0;
  while (TempString[Index] != 0 && TempString[Index+1] != 0) {
    Temp8 = TempString[Index];
    TempString[Index] = TempString[Index+1];
    TempString[Index+1] = Temp8;
    Index +=2;
  }
  BdsLibAsciiToUnicodeSize(TempString, BufSize, *BootString);
  if (TempString != NULL) {
    gBS->FreePool(TempString);
  }

  return EFI_SUCCESS;
}

VOID
CleanSpaceChar (
  IN    CHAR16    *Str
  )
/*++

Routine Description:

  Clean space character in the front and back of input string.

Arguments:

  Str   - Input string

Returns:

--*/
{
  UINTN     StrLen;
  UINTN     Start;
  UINTN     End;
  UINTN     ValidLen;
  CHAR16    SpaceChar = ' ';

  if (Str == NULL) {
    return;
  }

  StrLen = EfiStrLen (Str);
  if (StrLen == 0) {
    return;
  }

  Start = 0;
  End   = StrLen - 1;

  while (Str[Start] == SpaceChar) {
    Start++;
  }
  if (Start == StrLen) {
    //
    // All chars are space char, no need to remove space chars.
    //
    return;
  }

  while (Str[End] == SpaceChar) {
    End--;
  }

  ValidLen = End - Start + 1;
  if (ValidLen < StrLen) {
    gBS->CopyMem (&Str[0], &Str[Start], ValidLen * sizeof(CHAR16));
    gBS->SetMem (&Str[ValidLen], (StrLen - ValidLen) * sizeof(CHAR16), 0);
  }
}

VOID
AsciiToUnicode (
  IN    CHAR8     *AsciiStr,
  OUT   CHAR16    *UnicodeStr,
  IN    UINTN     AsciiStrLen
  )
/*++

Routine Description:

  Convert ASCII string to Unicode string in fixed length.

Arguments:

  AsciiStr        - Input ASCII string
  UnicodeStr      - Output Unicode string
  AsciiStrLen     - The string length of ASCII

Returns:

--*/
{
  UINT8           Index;

  if (AsciiStr == NULL || UnicodeStr == NULL) {
    return;
  }

  for (Index = 0; Index < AsciiStrLen; Index++) {
    UnicodeStr[Index] = (CHAR16) AsciiStr[Index];
  }
}

EFI_STATUS
BdsLibGetAllHwBootDeviceInfo (
  OUT UINTN                          *HwBootDeviceInfoCount,
  OUT HARDWARE_BOOT_DEVICE_INFO      *HwBootDeviceInfo
  )
/*++

Routine Description:

  Get all hardware boot device information (Block IO device path and device name).

Arguments:

  HwBootDeviceInfoCount  - Number of hardware boot device information
  HwBootDeviceInfo       - Array pointer of hardware boot device information

Returns:

  EFI_SUCCESS            - Get information successfully
  Otherwise              - Locate protocol fail or get usb device information fail

--*/
{
  EFI_STATUS                Status;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     Index;
  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathNode;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePathNode;
  PCI_DEVICE_PATH           *PciDevicePath;
  UINT32                    IdeChannel;
  UINT32                    IdeDevice;
  ATAPI_IDENTIFY            IdentifyDrive;
  UINT32                    Size;
  UINTN                     DeviceNameSize;
  CHAR16                    *DeviceName;
//EFI_GUID                  EfiUsbCoreProtocolGuid = EFI_USB_CORE_PROTOCOL_GUID;
//USB_MASS_STORAGE_DEVICE   **UsbMassStorageDevices;
//EFI_USB_CORE_PROTOCOL     *UsbCore;
//UINTN                     UsbMassStorageDevicesCount;
//EFI_USB_IO_PROTOCOL       *UsbIoInstance;
//UINTN                     Index2;
  BOOLEAN                        Found;
#if (EFI_SPECIFICATION_VERSION >= 0x00020000 && defined(EFI_COMPONENT_NAME2_PROTOCOL_GUID))
  EFI_COMPONENT_NAME_PROTOCOL    *ComponentName;
#else
  EFI_COMPONENT_NAME2_PROTOCOL   *ComponentName;
#endif
  CHAR16                         *DriverName;

  if (HwBootDeviceInfoCount == NULL || HwBootDeviceInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HwBootDeviceInfoCount = 0;
  HandleCount = 0;
  PciDevicePath = NULL;
  DeviceNameSize = 100;
  DeviceName = EfiLibAllocateZeroPool (DeviceNameSize);

  //
  // Collect all disk device information
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiDiskInfoProtocolGuid,
          NULL,
         &HandleCount,
         &HandleBuffer
         );

  for (Index = 0; Index < HandleCount; Index++) {
    gBS->HandleProtocol (
           HandleBuffer[Index],
           &gEfiDiskInfoProtocolGuid,
           &DiskInfo
           );

//  if (EfiCompareGuid (&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid)) {
    if ((EfiCompareGuid (&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid)) ||
        (EfiCompareGuid (&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid))) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID *) &DevicePath
                      );
      if (EFI_ERROR(Status)) {
        continue;
      }

      DevicePathNode = DevicePath;
      while (!IsDevicePathEnd (DevicePathNode)) {
        TempDevicePathNode = NextDevicePathNode (DevicePathNode);

        if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
            (DevicePathSubType (DevicePathNode) == HW_PCI_DP) &&
            (DevicePathType (TempDevicePathNode) == MESSAGING_DEVICE_PATH) &&
            ((DevicePathSubType (TempDevicePathNode) == MSG_ATAPI_DP)||
             (DevicePathSubType (TempDevicePathNode) == MSG_SATA_DP) ||
             (DevicePathSubType (TempDevicePathNode) == MSG_SCSI_DP))) {
          PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
          break;
        }

        DevicePathNode = NextDevicePathNode (DevicePathNode);
      }

      if (PciDevicePath == NULL) {
        continue;
      }

      Status = DiskInfo->WhichIde (DiskInfo, &IdeChannel, &IdeDevice);
      if (!EFI_ERROR (Status)) {
        Size = sizeof (ATAPI_IDENTIFY);
        Status = DiskInfo->Identify (
                             DiskInfo,
                             &IdentifyDrive,
                             &Size
                             );
        if (!EFI_ERROR (Status)) {
          BdsLibUpdateAtaString (
            (EFI_IDENTIFY_DATA *) &IdentifyDrive,
            DeviceNameSize,
            &DeviceName
            );
          CleanSpaceChar (DeviceName);

          HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName = EfiLibAllocateZeroPool (EfiStrSize (DeviceName));
          gBS->CopyMem (
                 HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName,
                 DeviceName,
                 EfiStrSize (DeviceName));

          HwBootDeviceInfo[*HwBootDeviceInfoCount].BlockIoDevicePath = DevicePath;
          (*HwBootDeviceInfoCount)++;
        }
      }
    }
  }

  if (HandleCount) {
    gBS->FreePool (HandleBuffer);
  }
  gBS->FreePool (DeviceName);

  //
  // Find out Usb Mass Storage Driver which can provide controller name of USB device.
  //
  ComponentName = NULL;
  Found = FALSE;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
#if (EFI_SPECIFICATION_VERSION >= 0x00020000 && defined(EFI_COMPONENT_NAME2_PROTOCOL_GUID))
                  &gEfiComponentName2ProtocolGuid,
#else
                  &gEfiComponentNameProtocolGuid,
#endif
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
#if (EFI_SPECIFICATION_VERSION >= 0x00020000 && defined(EFI_COMPONENT_NAME2_PROTOCOL_GUID))
                      &gEfiComponentName2ProtocolGuid,
#else
                      &gEfiComponentNameProtocolGuid,
#endif
                      &ComponentName
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      Status = ComponentName->GetDriverName (
                                ComponentName,
                                LANGUAGE_CODE_ENGLISH,
                                &DriverName
                                );
      if (!EFI_ERROR (Status) && !EfiStrCmp (DriverName, L"Usb Mass Storage Driver")) {
        Found = TRUE;
        break;
      }
    }
  }

  if (HandleCount) {
    gBS->FreePool (HandleBuffer);
  }

  //
  // Collect all USB device information
  //
//Status = gBS->LocateProtocol (
//                &EfiUsbCoreProtocolGuid,
//                 NULL,
//                &UsbCore
//                );
//if (EFI_ERROR(Status)) {
//  gBS->FreePool (DeviceName);
//  return Status;
//}
//
//Status = UsbCore->GetUsbDevices (
//                    USB_CORE_USB_MASS_STORAGE,
//                    &UsbMassStorageDevicesCount,
//                    (USB_DEVICE***) &UsbMassStorageDevices
//                    );
//if (EFI_ERROR(Status)) {
//  gBS->FreePool (DeviceName);
//  return Status;
//}

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiUsbIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (!EFI_ERROR (Status) && Found) {
//  //
//  // Compare UsbIoProtocol instance. If the same, save device path and device name.
//  //
    for (Index = 0; Index < HandleCount; Index++) {
//    Status = gBS->HandleProtocol (
//                    HandleBuffer[Index],
//                    &gEfiUsbIoProtocolGuid,
//                    &UsbIoInstance
//                    );
//
//  for (Index2 = 0; Index2 < UsbMassStorageDevicesCount; Index2 ++) {
//    if (UsbMassStorageDevices[Index2]->UsbIo == UsbIoInstance) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID *) &DevicePath
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }
      Status = ComponentName->GetControllerName (
                                ComponentName,
                                HandleBuffer[Index],
                                NULL,
                                LANGUAGE_CODE_ENGLISH,
                                &DeviceName
                                );
      if (EFI_ERROR (Status)) {
        continue;
      }

      HwBootDeviceInfo[*HwBootDeviceInfoCount].BlockIoDevicePath = DevicePath;
      HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName      = EfiLibAllocateCopyPool (EfiStrSize (DeviceName), DeviceName);
//    HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName = EfiLibAllocateZeroPool (25 * sizeof (CHAR16));
//    AsciiToUnicode (
//      (CHAR8 *) UsbMassStorageDevices[Index2]->Manufacturer,
//      HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName,
//      8);
//    AsciiToUnicode (
//      (CHAR8 *) UsbMassStorageDevices[Index2]->Product,
//      &HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName[8],
//      16);
      CleanSpaceChar (HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName);

      (*HwBootDeviceInfoCount)++;
//    }
    }
  }

  if (HandleCount) {
    gBS->FreePool (HandleBuffer);
  }
//gBS->FreePool (DeviceName);

  return EFI_SUCCESS;
}

EFI_DEVICE_PATH_PROTOCOL *
AppendHardDrivePathToBlkIoDevicePath (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
/*++

Routine Description:

  If input is a hard drive device path, append it to corresponding BlockIo device path.
  If input is not a hard drive device path, output NULL.

Arguments:

  HardDriveDevicePath      - Input device path

Returns:

  Device path protocol interface   - Device path which combines BlockIo and hard drive device path
  NULL                             - Otherwise

--*/
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UINTN                     Index;

  if (HardDriveDevicePath == NULL) {
    return NULL;
  }

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    return NULL;
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0) {
    return NULL;
  }

  for (Index = 0; Index < BlockIoHandleCount; Index++) {
    Status = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }

    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Combine the Block IO and Hard Drive Device path together.
      //
      DevicePath = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      NewDevicePath = EfiAppendDevicePath (BlockIoDevicePath, DevicePath);

      gBS->FreePool (BlockIoBuffer);
      return NewDevicePath;
    }
  }

  gBS->FreePool (BlockIoBuffer);
  return NULL;
}

BOOLEAN
BdsLibCompareBlockIoDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *BlockIoDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
/*++

Routine Description:

  Compare with each BlockIo device path.

Arguments:

  HardDriveDevicePath      - BlockIo device path
  DevicePath               - BlockIo device path or hard drive device path

Returns:

  TRUE   - BlockIo device paths are the same
  FALSE  - BlockIo device paths are different

--*/
{
  EFI_DEVICE_PATH_PROTOCOL *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  BOOLEAN                  Match;

  Match = TRUE;

  if (BlockIoDevicePath == NULL || DevicePath == NULL || IsDevicePathEnd (BlockIoDevicePath)) {
    return FALSE;
  }

  NewDevicePath = AppendHardDrivePathToBlkIoDevicePath ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  if (NewDevicePath == NULL) {
    NewDevicePath = DevicePath;
  }

  TempDevicePath = NewDevicePath;
  while (!IsDevicePathEnd (BlockIoDevicePath)) {
    if (EfiCompareMem (BlockIoDevicePath, TempDevicePath, DevicePathNodeLength (BlockIoDevicePath)) != 0) {
      Match = FALSE;
      break;
    }

    BlockIoDevicePath = NextDevicePathNode (BlockIoDevicePath);
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  if (NewDevicePath != DevicePath) {
    gBS->FreePool (NewDevicePath);
  }

  if (Match) {
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
BdsLibIsDummyBootOption (
  UINT16       OptionNum
  )
/*++

Routine Description:

  According to option number to check this boot option is whether
  dummy boot option.

Arguments:

  OptionNum    - UINT16 to save boot option number.

Returns:

  TRUE         - This boot option is dummy boot option.
  FALSE        - This boot option isn't dummy boot option.

--*/
{
  if (OptionNum > DummyBootOptionStartNum && OptionNum < DummyBootOptionEndNum) {
    return TRUE;
  }

  return FALSE;
}



EFI_STATUS
BdsLibCreateLoadOption (
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize,
  OUT VOID                           **LoadOption,
  OUT UINTN                          *LoadOptionSize
  )
/*++

Routine Description:

  This function allocates memory to generate load option. It is caller's responsibility
  to free load option if caller no longer requires the content of load option.

Arguments:

  DevicePath               - Pointer to a packed array of UEFI device paths.
  Description              - The user readable description for the load option.
  OptionalData             - Pointer to optional data for load option.
  OptionalDataSize         - The size of optional data.
  LoadOption               - Double pointer to load option.
  LoadOptionSize           - The load option size by byte.

Returns:

  EFI_INVALID_PARAMETER    - Any input parameter is invalid.
  EFI_OUT_OF_RESOURCES     - Allocate memory failed.
  EFI_SUCCESS              - Generate load option successful.

--*/
{
  UINTN             OptionSize;
  VOID              *OptionPtr;
  UINT8             *WorkingPtr;

  if (DevicePath == NULL || Description == NULL || LoadOption == NULL || LoadOptionSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (OptionalData == NULL) {
    OptionalDataSize = 0;
  }

  OptionSize = sizeof (UINT32) + sizeof (UINT16) + EfiStrSize (Description) + EfiDevicePathSize (DevicePath) + OptionalDataSize;
  OptionPtr  = EfiLibAllocateZeroPool (OptionSize);
  if (OptionPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  WorkingPtr = (UINT8 *) OptionPtr;
  *(UINT32 *) WorkingPtr = LOAD_OPTION_ACTIVE;
  WorkingPtr += sizeof (UINT32);
  *(UINT16 *) WorkingPtr = (UINT16) EfiDevicePathSize (DevicePath);
  WorkingPtr += sizeof (UINT16);
  EfiCopyMem (WorkingPtr, Description, EfiStrSize (Description));
  WorkingPtr += EfiStrSize (Description);
  EfiCopyMem (WorkingPtr, DevicePath, EfiDevicePathSize (DevicePath));
  WorkingPtr += EfiDevicePathSize (DevicePath);
  EfiCopyMem (WorkingPtr, OptionalData, OptionalDataSize);

  *LoadOption = OptionPtr;
  *LoadOptionSize = OptionSize;

  return EFI_SUCCESS;
}

#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
EFI_STATUS
BdsLibRestoreBootOrderFromPhysicalBootOrder (
  VOID
  )
/*++

Routine Description:

  This function restores the contents of PHYSICAL_BOOT_ORDER_NAME variable to
  "BootOrder"  variable.

Arguments:

  None

Returns:

  EFI_SUCCESS    - Restore the contents of "BootOrder" variable successful.
  Other          - Any error occurred while restoring data to "BootOrder" variable.

--*/
{
  EFI_STATUS      Status;
  UINT16          *PhysicalBootOrder;
  UINTN           PhysicalBootOrderSize;

  BdsLibDisableBootOrderHook ();
  PhysicalBootOrder = BdsLibGetVariableAndSize (
                        PHYSICAL_BOOT_ORDER_NAME,
                        &gEfiGenericVariableGuid,
                        &PhysicalBootOrderSize
                        );

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  PhysicalBootOrderSize,
                  PhysicalBootOrder
                  );
  if (PhysicalBootOrder != NULL) {
    gBS->FreePool (PhysicalBootOrder);
  }

  return Status;
}

EFI_STATUS
BdsLibSyncPhysicalBootOrder (
  VOID
  )
/*++

Routine Description:

  This function uses to synchronize the contents of PHYSICAL_BOOT_ORDER_NAME variable with
  the contents of "BootOrder" variable.

Arguments:

  None

Returns:

  EFI_SUCCESS    - Synchronize PHYSICAL_BOOT_ORDER_NAME variable successful.
  Other          - Any error occurred while Synchronizing PHYSICAL_BOOT_ORDER_NAME variable.

--*/
{
  EFI_STATUS      Status;
  UINT16          *BootOrder;
  UINTN           BootOrderSize;
  BOOLEAN         IsPhysicalBootOrder;

  if (BdsLibIsBootOrderHookEnabled ()) {
    return EFI_SUCCESS;
  }

  BootOrder = BdsLibGetVariableAndSize (
                        L"BootOrder",
                        &gEfiGlobalVariableGuid,
                        &BootOrderSize
                        );
  if (BootOrder == NULL) {
    return EFI_SUCCESS;
  }

  Status = BdsLibGetBootOrderType (BootOrder, BootOrderSize / sizeof (UINT16), &IsPhysicalBootOrder);
  if (!EFI_ERROR (Status) && IsPhysicalBootOrder) {
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    BootOrderSize,
                    BootOrder
                    );
  }

  if (BootOrder != NULL) {
    gBS->FreePool (BootOrder);
  }

  return Status;

}
#else
EFI_STATUS
BdsLibRestoreBootOrderFromPhysicalBootOrder (
  VOID
  )
/*++

Routine Description:

  This function restores the contents of PHYSICAL_BOOT_ORDER_NAME variable to
  "BootOrder"  variable.

Arguments:

  None

Returns:

  EFI_SUCCESS    - Restore the contents of "BootOrder" variable successful.
  Other          - Any error occurred while restoring data to "BootOrder" variable.

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibSyncPhysicalBootOrder (
  VOID
  )
/*++

Routine Description:

  This function uses to synchronize the contents of PHYSICAL_BOOT_ORDER_NAME variable with
  the contents of "BootOrder" variable.

Arguments:

  None

Returns:

  EFI_SUCCESS    - Synchronize PHYSICAL_BOOT_ORDER_NAME variable successful.
  Other          - Any error occurred while Synchronizing PHYSICAL_BOOT_ORDER_NAME variable.

--*/
{
  return EFI_SUCCESS;
}
#endif

EFI_STATUS
BdsLibGetBootOrderType (
  IN  UINT16        *BootOrder,
  IN  UINTN         BootOrderNum,
  OUT BOOLEAN       *PhysicalBootOrder
  )
/*++

Routine Description:

  According to input boot order to get the boot order type.

Arguments:

  BootOrder              - Pointer to BootOrder array.
  BootOrderNum           - The boot order number in boot order array.
  PhysicalBootOrder      - BOOLEAN value to indicate is whether physical boot order.

Returns:

  EFI_INVALID_PARAMETER  - BootOrder is NULL, BootOrderNum is 0 or PhysicalBootOrder is NULL.
  EFI_SUCCESS            - Determine the boot order is physical boot order or virtual boot order successful.

--*/
{
  UINTN          Index;

  if (BootOrder == NULL || BootOrderNum == 0 || PhysicalBootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *PhysicalBootOrder = TRUE;
  for (Index = 0; Index < BootOrderNum; Index++) {
    if (BdsLibIsDummyBootOption (BootOrder[Index])) {
      *PhysicalBootOrder = FALSE;
      break;
    }
  }

  return EFI_SUCCESS;
}

CHAR16 *
BdsLibGetDescriptionFromBootOption (
  IN UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to get the description from this boot option.
  It is caller's responsibility to free the description if caller no longer requires
  the content of description.

Arguments:

  OptionNum       - The boot option number.


Returns:

  NULL            - Get description from boot option failed.
  Other           - Get description from boot option successful.

--*/
{
  UINT16                     BootOption[BOOT_OPTION_MAX_CHAR];
  VOID                       *BootOptionVar;
  UINTN                      VariableSize;
  UINT8                      *WorkingPtr;
  CHAR16                     *Description;
  UINTN                      DescriptionSize;

  SPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNum);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    &VariableSize
                    );
  if (BootOptionVar == NULL) {
    return NULL;
  }

  //
  // Get the description start address in Boot option variable
  //
  WorkingPtr = (UINT8 *) BootOptionVar;
  WorkingPtr += (sizeof (UINT32) + sizeof (UINT16));
  DescriptionSize = EfiStrSize ((CHAR16 *) WorkingPtr);

  //
  // Copy the contents of device path to allocated memory
  //
  Description = EfiLibAllocateZeroPool (DescriptionSize);
  EfiCopyMem (Description, WorkingPtr, DescriptionSize);
  gBS->FreePool (BootOptionVar);

  return Description;
}

EFI_DEVICE_PATH_PROTOCOL *
BdsLibGetDevicePathFromBootOption (
  IN  UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to get the device path from this boot option.
  It is caller's responsibility to free the device path if caller no longer requires
  the content of device path.

Arguments:

  OptionNum       - The boot option number.


Returns:

  NULL            - Get device path from boot option failed.
  Other           - Get device path from boot option successful.

--*/
{
  UINT16                     BootOption[BOOT_OPTION_MAX_CHAR];
  VOID                       *BootOptionVar;
  UINTN                      VariableSize;
  UINT8                      *WorkingPtr;
  CHAR16                     *Description;
  UINT16                     DevicePathLength;
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;

  SPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNum);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    &VariableSize
                    );
  if (BootOptionVar == NULL) {
    return NULL;
  }

  //
  // Get the device path start address in Boot option variable
  //
  WorkingPtr = (UINT8 *) BootOptionVar;
  WorkingPtr += sizeof (UINT32);
  DevicePathLength = *((UINT16 *) WorkingPtr);
  WorkingPtr += sizeof (UINT16);
  Description = (UINT16 *) WorkingPtr;
  WorkingPtr += EfiStrSize (Description);

  //
  // Copy the contents of device path to allocated memory
  //
  DevicePath = EfiLibAllocateZeroPool (DevicePathLength);
  EfiCopyMem (DevicePath, WorkingPtr, DevicePathLength);
  gBS->FreePool (BootOptionVar);

  return DevicePath;
}

BOOLEAN
IsVirtualBootOption (
  IN  UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to check this boot option is whether a virtual boot option.

Arguments:

  OptionNum   - The boot option number.

Returns:

  TRUE        - This is a virtual boot option.
  FALSE       - This isn't a virtual boot option.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  BOOLEAN                    IsVritual;
  UINT32                     DevicePathType;

  if (BdsLibIsDummyBootOption (OptionNum)) {
    return TRUE;
  }

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsVritual = FALSE;
  if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP)) {
    //
    // Boot option for windows to go is virtual boot option
    //
    IsVritual = TRUE;
  }
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (DevicePathType == BDS_EFI_MEDIA_FV_FILEPATH_BOOT || DevicePathType == BDS_EFI_MEDIA_HD_BOOT ||
      DevicePathType == BDS_LEGACY_BBS_BOOT) {
    IsVritual = TRUE;
  }

  gBS->FreePool (DevicePath);
  return IsVritual;
}

STATIC
BOOLEAN
IsUefiUsbBootOption (
  IN  UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to check this boot option is whether an EFI USB boot option.

Arguments:

  OptionNum   - The boot option number.

Returns:

  TRUE        - This is an EFI USB boot option.
  FALSE       - This isn't an EFI USB boot option.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  BOOLEAN                    IsUsb;
  UINT32                     DevicePathType;

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsUsb = FALSE;
  if (!((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP))) {
    DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
    if (DevicePathType == BDS_EFI_MESSAGE_USB_DEVICE_BOOT) {
      IsUsb = TRUE;
    }
  }

  gBS->FreePool (DevicePath);
  return IsUsb;
}

STATIC
BOOLEAN
IsUefiCdBootOption (
  IN  UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to check this boot option is whether an EFI DVD/CDROM boot option.

Arguments:

  OptionNum   - The boot option number.

Returns:

  TRUE        - This is an EFI DVD/CDROM boot option.
  FALSE       - This isn't an EFI DVD/CDROM boot option.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  BOOLEAN                    IsCd;
  UINT32                     DevicePathType;

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsCd = FALSE;
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (DevicePathType == BDS_EFI_MEDIA_CDROM_BOOT) {
    IsCd = TRUE;
  }

  gBS->FreePool (DevicePath);
  return IsCd;
}

STATIC
BOOLEAN
IsUefiNetworkBootOption (
  IN  UINT16     OptionNum
  )
/*++

Routine Description:

  According to boot option number to check this boot option is whether an EFI network boot option.

Arguments:

  OptionNum   - The boot option number.

Returns:

  TRUE        - This is an EFI network boot option.
  FALSE       - This isn't an EFI network boot option.

--*/
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  BOOLEAN                    IsNetwork;
  UINT32                     DevicePathType;

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsNetwork = FALSE;
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (DevicePathType == BDS_EFI_MESSAGE_MAC_BOOT) {
    IsNetwork = TRUE;
  }

  gBS->FreePool (DevicePath);
  return IsNetwork;
}

#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
EFI_STATUS
BdsLibChangeToVirtualBootOrder (
  VOID
  )
/*++

Routine Description:

  This function uses to remove all of physical boot options and add virtual boot
  options to "BootOrder' variable

Arguments:

  None

Returns:

  EFI_SUCCESS     - Change the contents of "BootOrder" successful
  EFI_NOT_FOUND   - "BootOrder" variable doesn't exist.
  Other           - Set "BootOrder" variable failed.

--*/
{
  UINT16                  *BootOrder;
  UINTN                   BootOrderSize;
  UINTN                   BootOrderNum;
  UINT16                  *VirtualBootOrder;
  UINTN                   VirtualBootOrderNum;
  EFI_STATUS              Status;
  BOOLEAN                 IsPhysicalBootOrder;
  UINTN                   Index;
  UINTN                   TableIndex;


  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );

  BdsLibDisableBootOrderHook ();
  if (BootOrder == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Intialize option type maping table
  //
  for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
    mOptionTypeTable[TableIndex].Found = FALSE;
  }

  BootOrderNum = BootOrderSize / sizeof (UINT16);
  Status = BdsLibGetBootOrderType (BootOrder, BootOrderNum, &IsPhysicalBootOrder);
  if (!EFI_ERROR (Status) && IsPhysicalBootOrder) {
    VirtualBootOrder    = EfiLibAllocateZeroPool (BootOrderSize + sizeof (UINT16) * (DummyBootOptionEndNum - DummyBootOptionStartNum));
    VirtualBootOrderNum = 0;

    for (Index = 0; Index < BootOrderNum; Index++) {
      if (IsVirtualBootOption (BootOrder[Index])) {
        VirtualBootOrder[VirtualBootOrderNum] = BootOrder[Index];
        VirtualBootOrderNum++;
      } else {
        //
        // According to the order of corresponding option type to put dummy option
        //
        for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
          if (!mOptionTypeTable[TableIndex].Found &&
              mOptionTypeTable[TableIndex].CheckOptionType (BootOrder[Index])) {
            mOptionTypeTable[TableIndex].Found = TRUE;
            VirtualBootOrder[VirtualBootOrderNum] = mOptionTypeTable[TableIndex].OptionNum;
            VirtualBootOrderNum++;
            break;
          }
        }
      }
    }
    //
    // Put all of other dummy boot options at the end of boot order.
    //
    for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
      if (!mOptionTypeTable[TableIndex].Found ) {
        mOptionTypeTable[TableIndex].Found = TRUE;
        VirtualBootOrder[VirtualBootOrderNum] = mOptionTypeTable[TableIndex].OptionNum;
        VirtualBootOrderNum++;
      }
    }

    Status = gRT->SetVariable (
                    L"BootOrder",
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VirtualBootOrderNum * sizeof (UINT16),
                    VirtualBootOrder
                    );
    gBS->FreePool (VirtualBootOrder);
  }

  gBS->FreePool (BootOrder);
  return Status;
}
#else
EFI_STATUS
BdsLibChangeToVirtualBootOrder (
  VOID
  )
/*++

Routine Description:

  This function uses to remove all of physical boot options and add virtual boot
  options to "BootOrder' variable

Arguments:

  None

Returns:

  EFI_SUCCESS     - Change the contents of "BootOrder" successful
  EFI_NOT_FOUND   - "BootOrder" variable doesn't exist.
  Other           - Set "BootOrder" variable failed.

--*/
{
  return EFI_SUCCESS;
}
#endif

EFI_STATUS
BdsLibGetMappingBootOptions (
  IN    UINT16         OptionNum,
  OUT   UINTN          *OptionCount,
  OUT   UINT16         **OptionOrder
  )
/*++

Routine Description:

  This function uses to get all of corresponding mapping boot option numbers from "BootOrder" variable.

Arguments:

  OptionNum               - Input option number
  OptionCount             - Boot option count.
  OptionOrder             - Double pointer to option order to saved all of UEFI USB boot option.

Returns:

  EFI_SUCCESS             - Get UEFI USB boot option from "BootOrder" variable successful
  EFI_INVALID_PARAMETER   - OptionCount, OptionOrder is NULL or OptionNum is not a dummy boot option.
  EFI_NOT_FOUND           - Cannot find corresponding option type check function.

--*/
{
  UINTN                BootOrderSize;
  UINT16               *BootOrder;
  UINT16               *MappingBootOrder;
  UINTN                MappingCount;
  UINTN                Index;
  UINTN                TableIndex;

  if (OptionCount == NULL || OptionOrder == NULL || !BdsLibIsDummyBootOption (OptionNum)) {
    return EFI_INVALID_PARAMETER;
  }

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    OptionCount = 0;
    *OptionOrder = NULL;
  } else {
    //
    // Find corresponding function to check boot option
    //
    for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
      if (mOptionTypeTable[TableIndex].OptionNum == OptionNum) {
        break;
      }
    }
    ASSERT (TableIndex < OPTION_TYPE_TABLE_COUNT);
    if (TableIndex == OPTION_TYPE_TABLE_COUNT) {
      gBS->FreePool (BootOrder);
      return EFI_NOT_FOUND;
    }
    //
    // According to check function to get boot option count and boot option order.
    //
    MappingCount = 0;
    MappingBootOrder = EfiLibAllocateZeroPool (BootOrderSize);
    for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
      if (mOptionTypeTable[TableIndex].CheckOptionType (BootOrder[Index])) {
        MappingBootOrder[MappingCount] = BootOrder[Index];
        MappingCount++;
      }
    }
    gBS->FreePool (BootOrder);
    *OptionCount = MappingCount;
    *OptionOrder = MappingBootOrder;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
UpdateTargetHddVariable (
  VOID
  )
/*++

Routine Description:

  Update the BlockIo device path of target boot device into TargetHddDevPath variable.
  Assume target boot device is HDD in the first boot option in BootOrder.

Arguments:

Returns:

  EFI_SUCCESS            - Update target HDD variable success.
  EFI_NOT_FOUND          - BootOrder or Boot#### variable does not exist.

--*/
{
  EFI_STATUS                Status;
  UINT16                    *BootOrder;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  UINTN                     BlockIndex;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;


  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &Size
                );
  if (BootOrder == NULL) {
    return EFI_NOT_FOUND;
  }

  OptionDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[0]);
  if (OptionDevicePath == NULL) {
    gBS->FreePool (BootOrder);
    return EFI_NOT_FOUND;
  }

  if (((DevicePathType (OptionDevicePath) == MEDIA_DEVICE_PATH) &&
       (DevicePathSubType (OptionDevicePath) == MEDIA_HARDDRIVE_DP))) {
    BlockIoBuffer = NULL;
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiBlockIoProtocolGuid,
                    NULL,
                    &BlockIoHandleCount,
                    &BlockIoBuffer
                    );
    if (!EFI_ERROR (Status)) {
      for (BlockIndex = 0; BlockIndex < BlockIoHandleCount; BlockIndex++) {
        Status = gBS->HandleProtocol (
                        BlockIoBuffer[BlockIndex],
                        &gEfiDevicePathProtocolGuid,
                        &BlockIoDevicePath
                        );
        if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
          continue;
        }

        if (MatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath)) {
          gRT->SetVariable (
                 L"TargetHddDevPath",
                 &gEfiGenericVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                 EfiDevicePathSize (BlockIoDevicePath),
                 BlockIoDevicePath
                 );
          break;
        }
      }

      if (BlockIoBuffer != NULL) {
        gBS->FreePool (BlockIoBuffer);
      }
    }
  }

  gBS->FreePool (OptionDevicePath);
  gBS->FreePool (BootOrder);

  return EFI_SUCCESS;
}

BOOLEAN
TargetDevIsValid (
  VOID
  )
/*++

Routine Description:

  Check target device is valid or not by comparing device path from TargetHddDevPath variable.

Arguments:

Returns:

  TRUE                   - Target device is valid.
  FALSE                  - Target device is not valid.

--*/
{
  BOOLEAN                   Valid;
  UINT16                    *BootOrder;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;


  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &Size
                );
  if (BootOrder == NULL) {
    return FALSE;
  }

  OptionDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[0]);
  if (OptionDevicePath == NULL) {
    gBS->FreePool (BootOrder);
    return FALSE;
  }

  Valid = FALSE;

  if (((DevicePathType (OptionDevicePath) == MEDIA_DEVICE_PATH) &&
       (DevicePathSubType (OptionDevicePath) == MEDIA_HARDDRIVE_DP))) {
    BlockIoDevicePath = BdsLibGetVariableAndSize (
                          L"TargetHddDevPath",
                          &gEfiGenericVariableGuid,
                          &Size
                          );
    if (BlockIoDevicePath != NULL &&
        MatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath)) {
      Valid = TRUE;
      gBS->FreePool (BlockIoDevicePath);
    }
  }

  gBS->FreePool (OptionDevicePath);
  gBS->FreePool (BootOrder);

  return Valid;
}

BOOLEAN
BdsLibIsWin8FastBootActive (
  VOID
  )
/*++

Routine Description:

  Check Win 8 Fast Boot feature is active or not.

Arguments:

Returns:

  TRUE                   - Target device is valid.
  FALSE                  - Target device is not valid.

--*/
{
  EFI_STATUS                         Status;
  EFI_SETUP_UTILITY_PROTOCOL         *SetupUtility;
  SYSTEM_CONFIGURATION               *SystemConfiguration = NULL;
  UINT16                             BootNext;
  UINTN                              Size;

  Size = sizeof (BootNext);
  Status = gRT->GetVariable (
                  L"BootNext",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &BootNext
                  );
  if (!EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  &SetupUtility
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  SystemConfiguration = (SYSTEM_CONFIGURATION *) SetupUtility->SetupNvData;

  if (SystemConfiguration->Win8FastBoot == 0 &&
      BdsLibGetBootType () == EFI_BOOT_TYPE) {

    switch (SystemConfiguration->NewPositionPolicy) {

    case ADD_POSITION_AUTO:
      if (TargetDevIsValid ()) {
        return TRUE;
      }
      break;

    case ADD_POSITION_FIRST:
    case ADD_POSITION_LAST:
    default:
      break;
    }
  }

  return FALSE;
}

EFI_STATUS
SetSysPasswordCheck (
  VOID
  )
/*++

Routine Description:

  After checking system password, the checking flag is set to disable.
  Using this function to set the checking flag enable

Arguments:

  None

Returns:

  EFI_SUCCESS           - set the checking flag successful.

--*/
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService;
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;
  UINT32                                  SetupBits;

  SetupBits = 0;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  &SysPasswordService
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  SetupInfoBuffer.HaveSysPasswordCheck = TRUE;

  SetupBits = SYS_PASSWORD_CHECK_BIT;

  Status = SysPasswordService->SetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer,
                                 SetupBits
                                 );

  return Status;
}

DUMMY_BOOT_OPTION_NUM
BdsLibGetDummyBootOptionNum (
  IN  UINT16                 OptionNum
  )
/*++

Routine Description:

  Based on the boot option number, return the dummy boot option number.
  If return DummyBootOptionEndNum, it means this boot option does not belong to dummy boot option.

Arguments:

  OptionNum   - The boot option number.

Returns:

  Dummy boot option number or DummyBootOptionEndNum if input boot option does not belong to dummy boot option.

--*/
{
  UINTN          Index;

  for (Index = 0; Index < OPTION_TYPE_TABLE_COUNT; Index++) {
    if (mOptionTypeTable[Index].CheckOptionType (OptionNum)) {
      return mOptionTypeTable[Index].OptionNum;
    }
  }

  return DummyBootOptionEndNum;
}

STATIC
EFI_STATUS
UninstallSpecificProtocols (
  EFI_GUID         *Protocol
  )
/*++

Routine Description:

  Internal function to uninstall all of specific protocols in all of handles.

Arguments:

  Protocol    - Provides the protocol to remove.

Returns:

  EFI_SUCCESS - Uninstall all of protcols successfully.
  Other       - Any other error occurred while uninstalling protocols.

--*/
{
  EFI_STATUS       Status;
  UINTN            NoHandles;
  EFI_HANDLE       *Buffer;
  UINTN            Index;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Protocol,
                  NULL,
                  &NoHandles,
                  &Buffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NoHandles; Index++) {
      Status = gBS->UninstallProtocolInterface (
                      Buffer[Index],
                      Protocol,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        gBS->FreePool (Buffer);
        return Status;
      }
    }
    gBS->FreePool (Buffer);
  }

  return EFI_SUCCESS;

}

#ifdef AUTO_CREATE_DUMMY_BOOT_OPTION
EFI_STATUS
BdsLibEnableBootOrderHook (
  VOID
  )
/*++

Routine Description:

  Enable BootOrder variable hook mechanism.

Arguments:

  None

Returns:

  EFI_SUCCESS - Enable BootOrder variable hook mechanism successfully.
  Other       - Enable BootOrder variable hook mechanism failed.

--*/
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  VOID                *Interface;


  Status = UninstallSpecificProtocols (&gBootOrderHookDisableGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gBootOrderHookEnableGuid,
                  NULL,
                  &Interface
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Handle = NULL;
  return gBS->InstallProtocolInterface (
                &Handle,
                &gBootOrderHookEnableGuid,
                EFI_NATIVE_INTERFACE,
                NULL
                );
}

EFI_STATUS
BdsLibDisableBootOrderHook (
  VOID
  )
/*++

Routine Description:

  Disable BootOrder variable hook mechanism.

Arguments:

  None

Returns:

  EFI_SUCCESS - Disable BootOrder variable hook mechanism successfully.
  Other       - Disable BootOrder variable hook mechanism failed.

--*/
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  VOID                *Interface;

  Status = UninstallSpecificProtocols (&gBootOrderHookEnableGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gBootOrderHookDisableGuid,
                  NULL,
                  &Interface
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Handle = NULL;
  return gBS->InstallProtocolInterface (
                &Handle,
                &gBootOrderHookDisableGuid,
                EFI_NATIVE_INTERFACE,
                NULL
                );
}
#else
EFI_STATUS
BdsLibEnableBootOrderHook (
  VOID
  )
/*++

Routine Description:

  Enable BootOrder variable hook mechanism.

Arguments:

  None

Returns:

  EFI_SUCCESS - Enable BootOrder variable hook mechanism successfully.
  Other       - Enable BootOrder variable hook mechanism failed.

--*/
{
  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibDisableBootOrderHook (
  VOID
  )
/*++

Routine Description:

  Disable BootOrder variable hook mechanism.

Arguments:

  None

Returns:

  EFI_SUCCESS - Disable BootOrder variable hook mechanism successfully.
  Other       - Disable BootOrder variable hook mechanism failed.

--*/
{
  return EFI_SUCCESS;
}
#endif

BOOLEAN
BdsLibIsBootOrderHookEnabled (
  VOID
  )
/*++

Routine Description:

  Function uses to check BootOrder variable hook mechanism is whether enabled.

Arguments:

  None

Returns:

  TRUE   - BootOrder variable hook mechanism is enabled.
  FALSE  - BootOrder variable hook mechanism is disabled.

--*/
{
  EFI_STATUS      Status;
  VOID            *BootOrderHook;

  Status = gBS->LocateProtocol (
                  &gBootOrderHookEnableGuid,
                  NULL,
                  &BootOrderHook
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
EFIAPI
BdsLibGetImageHeader (
  IN  EFI_HANDLE                            Device,
  IN  CHAR16                                *FileName,
  OUT EFI_IMAGE_DOS_HEADER                  *DosHeader,
  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr
  )
/*++

Routine Description:

  Get the headers (dos, image, optional header) from an image

Arguments:

  Device              - SimpleFileSystem device handle
  FileName            - File name for the image
  DosHeader           - Pointer to dos header
  Hdr                 - The buffer in which to return the PE32, PE32+, or TE header.

Returns:

  EFI_SUCCESS         - Successfully get the machine type.
  EFI_NOT_FOUND       - The file is not found.
  EFI_LOAD_ERROR      - File is not a valid image file.

--*/
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root;
  EFI_FILE_HANDLE                  ThisFile;
  UINTN                            BufferSize;
  UINT64                           FileSize;
  EFI_FILE_INFO                    *Info;

  Root     = NULL;
  ThisFile = NULL;
  //
  // Handle the file system interface to the device
  //
  Status = gBS->HandleProtocol (
                  Device,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID *) &Volume
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = Volume->OpenVolume (
                     Volume,
                     &Root
                     );
  if (EFI_ERROR (Status)) {
    Root = NULL;
    goto Done;
  }
  ASSERT (Root != NULL);
  Status = Root->Open (Root, &ThisFile, FileName, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  ASSERT (ThisFile != NULL);

  //
  // Get file size
  //
  BufferSize  = SIZE_OF_EFI_FILE_INFO + 200;
  do {
    Info   = NULL;
    Status = gBS->AllocatePool (EfiBootServicesData, BufferSize, (VOID **) &Info);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    Status = ThisFile->GetInfo (
                         ThisFile,
                         &gEfiFileInfoGuid,
                         &BufferSize,
                         Info
                         );
    if (!EFI_ERROR (Status)) {
      break;
    }
    if (Status != EFI_BUFFER_TOO_SMALL) {
      gBS->FreePool (Info);
      goto Done;
    }
    gBS->FreePool (Info);
  } while (TRUE);

  FileSize = Info->FileSize;
  gBS->FreePool (Info);

  //
  // Read dos header
  //
  BufferSize = sizeof (EFI_IMAGE_DOS_HEADER);
  Status = ThisFile->Read (ThisFile, &BufferSize, DosHeader);
  if (EFI_ERROR (Status) ||
      BufferSize < sizeof (EFI_IMAGE_DOS_HEADER) ||
      FileSize <= DosHeader->e_lfanew ||
      DosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Move to PE signature
  //
  Status = ThisFile->SetPosition (ThisFile, DosHeader->e_lfanew);
  if (EFI_ERROR (Status)) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Read and check PE signature
  //
  BufferSize = sizeof (EFI_IMAGE_OPTIONAL_HEADER_UNION);
  Status = ThisFile->Read (ThisFile, &BufferSize, Hdr.Pe32);
  if (EFI_ERROR (Status) ||
      BufferSize < sizeof (EFI_IMAGE_OPTIONAL_HEADER_UNION) ||
      Hdr.Pe32->Signature != EFI_IMAGE_NT_SIGNATURE) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Check PE32 or PE32+ magic
  //
  if (Hdr.Pe32->OptionalHeader.Magic != EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
      Hdr.Pe32->OptionalHeader.Magic != EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

 Done:
  if (ThisFile != NULL) {
    ThisFile->Close (ThisFile);
  }
  if (Root != NULL) {
    Root->Close (Root);
  }
  return Status;
}

BOOLEAN
IsOnBoardPciDevice (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function
  )
/*++

Routine Description:

  According the Bus, Device, Function to check this controller is in Port Number Map table or not.

Arguments:

  Bus -             - PCI bus number
  Device            - PCI device number
  Function          - PCI function number

Returns:

  TRUE            - This is a on board PCI device.
  FALSE           - Not on board device.

--*/
{
  EFI_STATUS                            Status;
  PORT_NUMBER_MAP                       *PortMappingTable;
  PORT_NUMBER_MAP                       EndEntry;
  UINTN                                 NoPorts;
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL     *ChipsetLibServices;

  Status = gBS->LocateProtocol (
                  &gEfiChipsetLibServicesProtocolGuid,
                  NULL,
                  &ChipsetLibServices
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = ChipsetLibServices->GetPortNumberMapTable (&PortMappingTable, &NoPorts);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  NoPorts = 0;
  while (EfiCompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    if ((PortMappingTable[NoPorts].Bus == Bus) &&
        (PortMappingTable[NoPorts].Device == Device) &&
        (PortMappingTable[NoPorts].Function == Function)) {
      return TRUE;
    }
    NoPorts++;
  }

  return FALSE;
}

