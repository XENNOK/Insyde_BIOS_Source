/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Misc BDS library function

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalBdsLib.h"
#include <Protocol/DynamicHotKey.h>


#define MAX_STRING_LEN        200

BOOLEAN   mFeaturerSwitch = TRUE;
BOOLEAN   mResetRequired  = FALSE;

//extern UINT16 gPlatformBootTimeOutDefault;
typedef
BOOLEAN
(*CHECK_BOOT_OPTION_TYPE) (
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

/**
  The function will go through the driver option link list, load and start
  every driver the driver option device path point to.

  @param  BdsDriverLists        The header of the current driver option link list

**/
VOID
EFIAPI
BdsLibLoadDrivers (
  IN LIST_ENTRY                   *BdsDriverLists
  )
{
  EFI_STATUS                Status;
  LIST_ENTRY                *Link;
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
                    gImageHandle,
                    Option->DevicePath,
                    NULL,
                    0,
                    &ImageHandle
                    );

    if (!EFI_ERROR (Status)) {
      gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &ImageInfo);

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
      DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Driver Return Status = %r\n", Status));

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

/**
  Get the Option Number that does not used.
  Try to locate the specific option variable one by one utile find a free number.

  @param  VariableName          Indicate if the boot#### or driver#### option

  @return The Minimal Free Option Number

**/
UINT16
BdsLibGetFreeOptionNumber (
  IN  CHAR16    *VariableName
  )
{
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
      UnicodeSPrint (StrTemp, sizeof (StrTemp), L"Boot%04x", Index);
    } else {
      UnicodeSPrint (StrTemp, sizeof (StrTemp), L"Driver%04x", Index);
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
  } while (TRUE);

  return ((UINT16) Index);
}


/**
  Update the boot#### or driver#### variable.

  @param  OptionName         String part of EFI variable name
  @param  DevicePath         The device path of the boot#### or driver####
  @param  Description        The description of the boot#### or driver####
  @param  OptionalData       Optional data of the boot#### or driver####
  @param  OptionalDataSize   Optional data size of the boot#### or driver####

  @retval EFI_SUCCESS   The boot#### or driver#### have been success updated.
  @retval EFI_ABORTED   Allocate memory resource fail.
  @retval Other         Set variable fail.

**/
EFI_STATUS
BdsLibUpdateOptionVar (
  IN  CHAR16                         *OptionName,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  )
{
  EFI_STATUS                Status;
  UINTN                     OptionSize;
  VOID                      *OptionPtr;

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


/**
  This function will register the new boot#### or driver#### option base on
  the VariableName. The new registered boot#### or driver#### will be linked
  to BdsOptionList and also update to the VariableName. After the boot#### or
  driver#### updated, the BootOrder or DriverOrder will also be updated.

  @param  BdsOptionList         The header of the boot#### or driver#### link list
  @param  DevicePath            The device path which the boot#### or driver####
                                option present
  @param  String                The description of the boot#### or driver####
  @param  VariableName          Indicate if the boot#### or driver#### option

  @retval EFI_SUCCESS           The boot#### or driver#### have been success
                                registered
  @retval EFI_STATUS            Return the status of gRT->SetVariable ().

**/
EFI_STATUS
EFIAPI
BdsLibRegisterNewOption (
  IN  LIST_ENTRY                     *BdsOptionList,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *String,
  IN  CHAR16                         *VariableName,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  )
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

  ZeroMem (OptionName, sizeof (OptionName));

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
  for (Index = 0; (TempOptionPtr != NULL) && (Index < BootOptionNum); Index++) {
    //
    // Got the max option#### number
    //
    // remark these and get option number in GetNewBootOptionNo function.

    if (*VariableName == 'B') {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", TempOptionPtr[Index]);
    } else {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Driver%04x", TempOptionPtr[Index]);
    }

    OptionPtr = BdsLibGetVariableAndSize (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  &OptionSize
                  );
    TempPtr = OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    //
    // Notes: the description may will change base on the GetStringToken
    //
    if (CompareMem (OptionDevicePath, DevicePath, GetDevicePathSize (OptionDevicePath)) == 0) {
      if (CompareMem (Description, String, StrSize (Description)) == 0 ) {
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
      if (CompareMem (Description, String, StrSize (Description)) == 0 ) {
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

  if (UpdateDescription) {
    //
    // The number in option#### to be updated
    //
    RegisterOptionNumber = TempOptionPtr[Index];
  } else {
    //
    // get new BootOption number
    //
    if (TempOptionPtr != NULL) {
      GetNewBootOptionNo (
        TempOptionPtr,
        BootOptionNum,
        &RegisterOptionNumber
        );
    } else {
      RegisterOptionNumber = 0;
    }
  }

  if (*VariableName == 'B') {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", RegisterOptionNumber);
  } else {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Driver%04x", RegisterOptionNumber);
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
    if (TempOptionPtr != NULL) {
      gBS->FreePool (TempOptionPtr);
    }
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
                  (VOID **) &BootOptionPolicy
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

  BdsLibNewBootOptionPolicy (
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
  if (TempOptionPtr != NULL) {
    gBS->FreePool (TempOptionPtr);
  }
  gBS->FreePool (OptionOrderPtr);

  return Status;
}


/**
  Returns the size of a device path in bytes.

  This function returns the size, in bytes, of the device path data structure specified by DevicePath including the end of device path node.
  If DevicePath is NULL, then 0 is returned.
  If the length of the device path is bigger than MaxSize, also return 0 to indicate this is an invalidate device path.

  @param  DevicePath         A pointer to a device path data structure.
  @param  MaxSize            Max valid device path size. If big than this size, return error.

  @retval 0                  An invalid device path.
  @retval Others             The size of a device path in bytes.

**/
UINTN
GetDevicePathSizeEx (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINTN                           MaxSize
  )
{
  UINTN  Size;
  UINTN  NodeSize;

  if (DevicePath == NULL) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Size = 0;
  while (!IsDevicePathEnd (DevicePath)) {
    NodeSize = DevicePathNodeLength (DevicePath);
    if (NodeSize < END_DEVICE_PATH_LENGTH) {
      return 0;
    }

    Size += NodeSize;
    if (Size > MaxSize) {
      return 0;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  Size += DevicePathNodeLength (DevicePath);
  if (Size > MaxSize) {
    return 0;
  }

  return Size;
}


/**
  Returns the length of a Null-terminated Unicode string. If the length is
  bigger than MaxStringLen, return length 0 to indicate that this is an
  invalidate string.

  This function returns the number of Unicode characters in the Null-terminated
  Unicode string specified by String.

  If String is NULL, then ASSERT().
  If String is not aligned on a 16-bit boundary, then ASSERT().

  @param  String           A pointer to a Null-terminated Unicode string.
  @param  MaxStringLen     Max string len in this string.

  @retval 0                An invalid string.
  @retval Others           The length of String.

**/
UINTN
StrSizeEx (
  IN      CONST CHAR16              *String,
  IN      UINTN                     MaxStringLen
  )
{
  UINTN                             Length;

  ASSERT (String != NULL && MaxStringLen != 0);
  ASSERT (((UINTN) String & BIT0) == 0);

  for (Length = 0; *String != L'\0' && MaxStringLen != Length; String++, Length++);

  if (*String != L'\0' && MaxStringLen == Length) {
    return 0;
  }

  return (Length + 1) * sizeof (*String);
}

/**
  Validate the EFI Boot#### variable (VendorGuid/Name)

  @param  Variable              Boot#### variable data.
  @param  VariableSize          Returns the size of the EFI variable that was read

  @retval TRUE                  The variable data is correct.
  @retval FALSE                 The variable data is corrupted.

**/
BOOLEAN
ValidateOption (
  UINT8                     *Variable,
  UINTN                     VariableSize
  )
{
  UINT16                    FilePathSize;
  UINT8                     *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempPath;
  UINTN                     TempSize;

  //
  // Skip the option attribute
  //
  TempPtr    = Variable;
  TempPtr   += sizeof (UINT32);

  //
  // Get the option's device path size
  //
  FilePathSize  = *(UINT16 *) TempPtr;
  TempPtr      += sizeof (UINT16);

  //
  // Get the option's description string size
  //
  TempSize = StrSizeEx ((CHAR16 *) TempPtr, VariableSize);
  TempPtr += TempSize;

  //
  // Get the option's device path
  //
  DevicePath =  (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
  TempPtr    += FilePathSize;

  //
  // Validation boot option variable.
  //
  if ((FilePathSize == 0) || (TempSize == 0)) {
    return FALSE;
  }

  if (TempSize + FilePathSize + sizeof (UINT16) + sizeof (UINT16) > VariableSize) {
    return FALSE;
  }

  TempPath = DevicePath;
  while (FilePathSize > 0) {
    TempSize = GetDevicePathSizeEx (TempPath, FilePathSize);
    if (TempSize == 0) {
      return FALSE;
    }

    FilePathSize = (UINT16) (FilePathSize - TempSize);
    TempPath    += TempSize;
  }

  return TRUE;
}


/**
  Check if option variable is created by BIOS or not.

  @param[in]  Variable          Pointer to option variable
  @param[in]  VariableSize      Option variable size

  @retval     TRUE              Option varible is created by BIOS
  @retval     FALSE             Option varible is not created by BIOS
**/
BOOLEAN
BdsLibIsBiosCreatedOption (
  IN UINT8                  *Variable,
  IN UINTN                  VariableSize
  )
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
  Ptr += StrSize ((CHAR16 *) Ptr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  Ptr += FilePathSize;

  LoadOptions     = Ptr;
  LoadOptionsSize = (UINT32) (VariableSize - (UINTN) (Ptr - Variable));

  //
  // RC signature in optional data means it is created by BIOS.
  //
  if ((LoadOptionsSize == 2 || LoadOptionsSize == SHELL_OPTIONAL_DATA_SIZE) &&
      (AsciiStrnCmp (LoadOptions, "RC", 2) == 0)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Build the boot#### or driver#### option from the VariableName, the
  build boot#### or driver#### will also be linked to BdsCommonOptionList.

  @param  BdsCommonOptionList   The header of the boot#### or driver#### option
                                link list
  @param  VariableName          EFI Variable name indicate if it is boot#### or
                                driver####

  @retval BDS_COMMON_OPTION     Get the option just been created
  @retval NULL                  Failed to get the new option

**/
BDS_COMMON_OPTION *
EFIAPI
BdsLibVariableToOption (
  IN OUT LIST_ENTRY                   *BdsCommonOptionList,
  IN  CHAR16                          *VariableName
  )
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
  TempPtr   =  Variable;
  Attribute =  *(UINT32 *) Variable;
  TempPtr   += sizeof (UINT32);

  //
  // Get the option's device path size
  //
  FilePathSize =  *(UINT16 *) TempPtr;
  TempPtr      += sizeof (UINT16);

  //
  // Get the option's description string
  //
  Description = (CHAR16 *) TempPtr;

  //
  // Get the option's description string size
  //
  TempPtr     += StrSize ((CHAR16 *) TempPtr);

  //
  // Get the option's device path
  //
  DevicePath =  (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
  TempPtr    += FilePathSize;

  LoadOptions     = TempPtr;
  LoadOptionsSize = (UINT32) (VariableSize - (UINTN) (TempPtr - Variable));

  //
  // The Console variables may have multiple device paths, so make
  // an Entry for each one.
  //
  Option = AllocateZeroPool (sizeof (BDS_COMMON_OPTION));
  if (Option == NULL) {
    return NULL;
  }

  Option->Signature   = BDS_LOAD_OPTION_SIGNATURE;
  Option->DevicePath  = AllocateZeroPool (GetDevicePathSize (DevicePath));
  ASSERT(Option->DevicePath != NULL);
  CopyMem (Option->DevicePath, DevicePath, GetDevicePathSize (DevicePath));

  Option->Attribute   = Attribute;
  Option->Description = AllocateZeroPool (StrSize (Description));
  ASSERT(Option->Description != NULL);
  CopyMem (Option->Description, Description, StrSize (Description));

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
    Option->LoadOptions = AllocateZeroPool (LoadOptionsSize);
    ASSERT(Option->LoadOptions != NULL);
    CopyMem (Option->LoadOptions, LoadOptions, LoadOptionsSize);
    Option->LoadOptionsSize = LoadOptionsSize;
  }

  //
  // Get the value from VariableName Unicode string
  // since the ISO standard assumes ASCII equivalent abbreviations, we can be safe in converting this
  // Unicode stream to ASCII without any loss in meaning.
  //
  if (*VariableName == 'B') {
    NumOff = (UINT8) (sizeof (L"Boot") / sizeof(CHAR16) - 1);
    Option->BootCurrent = (UINT16) ((VariableName[NumOff]  -'0') * 0x1000);
    Option->BootCurrent = (UINT16) (Option->BootCurrent + ((VariableName[NumOff+1]-'0') * 0x100));
    Option->BootCurrent = (UINT16) (Option->BootCurrent +  ((VariableName[NumOff+2]-'0') * 0x10));
    Option->BootCurrent = (UINT16) (Option->BootCurrent + ((VariableName[NumOff+3]-'0')));
  }
  //
  // Insert active entry to BdsDeviceList
  //
  if ((Option->Attribute & LOAD_OPTION_ACTIVE) == LOAD_OPTION_ACTIVE) {
    InsertTailList (BdsCommonOptionList, &Option->Link);
    FreePool (Variable);
    return Option;
  }

  FreePool (Variable);
  FreePool (Option);
  return NULL;
}

/**
  Process BootOrder, or DriverOrder variables, by calling
  BdsLibVariableToOption () for each UINT16 in the variables.

  @param  BdsCommonOptionList   The header of the option list base on variable
                                VariableName
  @param  VariableName          EFI Variable name indicate the BootOrder or
                                DriverOrder

  @retval EFI_SUCCESS           Success create the boot option or driver option
                                list
  @retval EFI_OUT_OF_RESOURCES  Failed to get the boot option or driver option list

**/
EFI_STATUS
EFIAPI
BdsLibBuildOptionFromVar (
  IN  LIST_ENTRY                      *BdsCommonOptionList,
  IN  CHAR16                          *VariableName
  )
{
  UINT16            *OptionOrder;
  UINTN             OptionOrderSize;
  UINTN             Index;
  BDS_COMMON_OPTION *Option;
  CHAR16            OptionName[20];

  //
  // Zero Buffer in order to get all BOOT#### variables
  //
  ZeroMem (OptionName, sizeof (OptionName));

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
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", OptionOrder[Index]);
    } else {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Driver%04x", OptionOrder[Index]);
    }

    Option              = BdsLibVariableToOption (BdsCommonOptionList, OptionName);
    if (Option != NULL) {
      Option->BootCurrent = OptionOrder[Index];
    }
  }

  FreePool (OptionOrder);

  return EFI_SUCCESS;
}

/**
  Get boot mode by looking up configuration table and parsing HOB list

  @param  BootMode              Boot mode from PEI handoff HOB.

  @retval EFI_SUCCESS           Successfully get boot mode

**/
EFI_STATUS
EFIAPI
BdsLibGetBootMode (
  OUT EFI_BOOT_MODE       *BootMode
  )
{
  *BootMode = GetBootModeHob ();

  return EFI_SUCCESS;
}

/**
  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

  @param  Name                  String part of EFI variable name
  @param  VendorGuid            GUID part of EFI variable name
  @param  VariableSize          Returns the size of the EFI variable that was read

  @return                       Dynamically allocated memory that contains a copy of the EFI variable
                                Caller is responsible freeing the buffer.
  @retval NULL                  Variable was not read

**/
VOID *
EFIAPI
BdsLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
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
    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      *VariableSize = 0;
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

/**
  Delete the instance in Multi which matches partly with Single instance

  @param  Multi                 A pointer to a multi-instance device path data
                                structure.
  @param  Single                A pointer to a single-instance device path data
                                structure.

  @return This function will remove the device path instances in Multi which partly
          match with the Single, and return the result device path. If there is no
          remaining device path as a result, this function will return NULL.

**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
BdsLibDelPartMatchInstance (
  IN     EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN     EFI_DEVICE_PATH_PROTOCOL  *Single
  )
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

  Instance        =  GetNextDevicePathInstance (&Multi, &InstanceSize);
  SingleDpSize    =  GetDevicePathSize (Single) - END_DEVICE_PATH_LENGTH;
  InstanceSize    -= END_DEVICE_PATH_LENGTH;

  while (Instance != NULL) {

    Size = (SingleDpSize < InstanceSize) ? SingleDpSize : InstanceSize;

    if ((CompareMem (Instance, Single, Size) != 0)) {
      //
      // Append the device path instance which does not match with Single
      //
      TempNewDevicePath = NewDevicePath;
      NewDevicePath = AppendDevicePathInstance (NewDevicePath, Instance);
      if (TempNewDevicePath != NULL) {
        FreePool(TempNewDevicePath);
      }
    }
    FreePool(Instance);
    Instance = GetNextDevicePathInstance (&Multi, &InstanceSize);
    InstanceSize  -= END_DEVICE_PATH_LENGTH;
  }

  return NewDevicePath;
}

/**
  Function compares a device path data structure to that of all the nodes of a
  second device path instance.

  @param  Multi                 A pointer to a multi-instance device path data
                                structure.
  @param  Single                A pointer to a single-instance device path data
                                structure.

  @retval TRUE                  If the Single device path is contained within Multi device path.
  @retval FALSE                 The Single device path is not match within Multi device path.

**/
BOOLEAN
EFIAPI
BdsLibMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;

  if (Multi == NULL || Single  == NULL) {
    return FALSE;
  }

  DevicePath      = Multi;
  DevicePathInst  = GetNextDevicePathInstance (&DevicePath, &Size);

  //
  // Search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst != NULL) {
    //
    // If the single device path is found in multiple device paths,
    // return success
    //
    if (CompareMem (Single, DevicePathInst, Size) == 0) {
      FreePool (DevicePathInst);
      return TRUE;
    }

    FreePool (DevicePathInst);
    DevicePathInst = GetNextDevicePathInstance (&DevicePath, &Size);
  }

  return FALSE;
}

/**
  This function prints a series of strings.

  @param  ConOut                Pointer to EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
  @param  ...                   A variable argument list containing series of
                                strings, the last string must be NULL.

  @retval EFI_SUCCESS           Success print out the string using ConOut.
  @retval EFI_STATUS            Return the status of the ConOut->OutputString ().

**/
EFI_STATUS
EFIAPI
BdsLibOutputStrings (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut,
  ...
  )
{
  VA_LIST     Args;
  EFI_STATUS  Status;
  CHAR16      *String;

  Status = EFI_SUCCESS;
  VA_START (Args, ConOut);

  while (!EFI_ERROR (Status)) {
    //
    // If String is NULL, then it's the end of the list
    //
    String = VA_ARG (Args, CHAR16 *);
    if (String == NULL) {
      break;
    }

    Status = ConOut->OutputString (ConOut, String);

    if (EFI_ERROR (Status)) {
      break;
    }
  }

  VA_END(Args);
  return Status;
}


/**
  insert new BootOption number to BootOrder by policy

  @param  NewBootOrder         the new BootOrder
  @param  OldBootOrder         current BootOrder
  @param  OldBootOrderCount    the count of elements in current BootOrder
  @param  NewBootOptionNo      the number of BootOption that would be insert in BootOrder
  @param  Policy               the index of BootOrder that the NewBootOptionNo would be insert in .

  @retval EFI_SUCCESS   Success insert new BootOption number in BootOrder

**/
EFI_STATUS
BdsLibNewBootOptionPolicy(
  UINT16                                     **NewBootOrder,
  UINT16                                     *OldBootOrder,
  UINTN                                      OldBootOrderCount,
  UINT16                                     NewBootOptionNo,
  UINTN                                      Policy
  )
{
  UINTN                                      Front = 0;
  UINTN                                      Back  = 0;
  UINT16                                     *TempPtr1 = NULL;
  UINT16                                     *TempPtr2 = NULL;

  *NewBootOrder = AllocateZeroPool ((OldBootOrderCount + 1) * sizeof (UINT16));
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
      CopyMem (*NewBootOrder, OldBootOrder, Front);
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
      CopyMem (TempPtr1, TempPtr2, Back);
    }
  }

  return EFI_SUCCESS;
}


/**
  Get the number of new BootOption

  @param  BootOrder             The pointer of BootOrder
  @param  BootOptionNum         The count of BootOption
  @param  CurrentBootOptionNo   The number of new BootOption

  @retval EFI_SUCCESS   Success get new BootOption number

**/
EFI_STATUS
GetNewBootOptionNo (
  IN     UINT16                                     *BootOrder,
  IN     UINTN                                      BootOptionNum,
  IN OUT UINT16                                     *CurrentBootOptionNo
  )
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
//  Following are BDS Lib functions which contain all the code about setup browser reset reminder feature.
//  Setup Browser reset reminder feature is that an reset reminder will be given before user leaves the setup browser  if
//  user change any option setting which needs a reset to be effective, and  the reset will be applied according to  the user selection.
//


/**
  Enable the setup browser reset reminder feature.
  This routine is used in platform tip. If the platform policy need the feature, use the routine to enable it.

**/
VOID
EFIAPI
EnableResetReminderFeature (
  VOID
  )
{
  mFeaturerSwitch = TRUE;
}


/**
  Disable the setup browser reset reminder feature.
  This routine is used in platform tip. If the platform policy do not want the feature, use the routine to disable it.

**/
VOID
EFIAPI
DisableResetReminderFeature (
  VOID
  )
{
  mFeaturerSwitch = FALSE;
}

/**
  Record the info that  a reset is required.
  A  module boolean variable is used to record whether a reset is required.

**/
VOID
EFIAPI
EnableResetRequired (
  VOID
  )
{
  mResetRequired = TRUE;
}

/**
  Record the info that  no reset is required.
  A  module boolean variable is used to record whether a reset is required.

**/
VOID
EFIAPI
DisableResetRequired (
  VOID
  )
{
  mResetRequired = FALSE;
}


/**
  Check whether platform policy enable the reset reminder feature. The default is enabled.

**/
BOOLEAN
EFIAPI
IsResetReminderFeatureEnable (
  VOID
  )
{
  return mFeaturerSwitch;
}


/**
  Check if  user changed any option setting which needs a system reset to be effective.

**/
BOOLEAN
EFIAPI
IsResetRequired (
  VOID
  )
{
  return mResetRequired;
}


/**
  Check whether a reset is needed, and finish the reset reminder feature.
  If a reset is needed, Popup a menu to notice user, and finish the feature
  according to the user selection.

**/
VOID
EFIAPI
SetupResetReminder (
  VOID
  )
{
  EFI_INPUT_KEY                 Key;
  CHAR16                        *StringBuffer1;
  CHAR16                        *StringBuffer2;


  //
  //check any reset required change is applied? if yes, reset system
  //
  if (IsResetReminderFeatureEnable ()) {
    if (IsResetRequired ()) {

      StringBuffer1 = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16));
      ASSERT (StringBuffer1 != NULL);
      if (StringBuffer1 == NULL) {
        return;
      }
      StringBuffer2 = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16));
      ASSERT (StringBuffer2 != NULL);
      if (StringBuffer2 == NULL) {
        return;
      }
      StrCpy (StringBuffer1, L"Configuration changed. Reset to apply it Now ? ");
      StrCpy (StringBuffer2, L"Enter (YES)  /   Esc (NO)");
      //
      // Popup a menu to notice user
      //
      do {
        CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
      } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

      FreePool (StringBuffer1);
      FreePool (StringBuffer2);
      //
      // If the user hits the YES Response key, reset
      //
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
      }
      gST->ConOut->ClearScreen (gST->ConOut);
    }
  }
}

/**
  Get the headers (dos, image, optional header) from an image

  @param  Device                SimpleFileSystem device handle
  @param  FileName              File name for the image
  @param  DosHeader             Pointer to dos header
  @param  Hdr                   The buffer in which to return the PE32, PE32+, or TE header.

  @retval EFI_SUCCESS           Successfully get the machine type.
  @retval EFI_NOT_FOUND         The file is not found.
  @retval EFI_LOAD_ERROR        File is not a valid image file.

**/
EFI_STATUS
EFIAPI
BdsLibGetImageHeader (
  IN  EFI_HANDLE                  Device,
  IN  CHAR16                      *FileName,
  OUT EFI_IMAGE_DOS_HEADER        *DosHeader,
  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr
  )
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
      FreePool (Info);
      goto Done;
    }
    FreePool (Info);
  } while (TRUE);

  FileSize = Info->FileSize;
  FreePool (Info);

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

/**
  This routine adjusts the memory information for different memory type and
  saves them into the variables for next boot. It conditionally resets the
  system when the memory information changes. Platform can reserve memory
  large enough (125% of actual requirement) to avoid the reset in the first boot.
**/
VOID
EFIAPI
BdsSetMemoryTypeInformationVariable (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{
  EFI_STATUS                   Status;
  EFI_MEMORY_TYPE_INFORMATION  *PreviousMemoryTypeInformation;
  EFI_MEMORY_TYPE_INFORMATION  *CurrentMemoryTypeInformation;
  UINTN                        VariableSize;
  UINTN                        Index;
  UINTN                        Index1;
  UINT32                       Previous;
  UINT32                       Current;
  UINT32                       Next;
  EFI_HOB_GUID_TYPE            *GuidHob;
  BOOLEAN                      MemoryTypeInformationModified;
  BOOLEAN                      MemoryTypeInformationVariableExists;
  EFI_BOOT_MODE                BootMode;

  MemoryTypeInformationModified       = FALSE;
  MemoryTypeInformationVariableExists = FALSE;


  BootMode = GetBootModeHob ();
  //
  // In BOOT_IN_RECOVERY_MODE, Variable region is not reliable.
  //
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    return;
  }

  //
  // Only check the the Memory Type Information variable in the boot mode
  // other than BOOT_WITH_DEFAULT_SETTINGS because the Memory Type
  // Information is not valid in this boot mode.
  //
  if (BootMode != BOOT_WITH_DEFAULT_SETTINGS) {
    VariableSize = 0;
    Status = gRT->GetVariable (
                    EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                    &gEfiMemoryTypeInformationGuid,
                    NULL,
                    &VariableSize,
                    NULL
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      MemoryTypeInformationVariableExists = TRUE;
    }
  }

  //
  // Retrieve the current memory usage statistics.  If they are not found, then
  // no adjustments can be made to the Memory Type Information variable.
  //
  Status = EfiGetSystemConfigurationTable (
             &gEfiMemoryTypeInformationGuid,
             (VOID **) &CurrentMemoryTypeInformation
             );
  if (EFI_ERROR (Status) || CurrentMemoryTypeInformation == NULL) {
    return;
  }

  //
  // Get the Memory Type Information settings from Hob if they exist,
  // PEI is responsible for getting them from variable and build a Hob to save them.
  // If the previous Memory Type Information is not available, then set defaults
  //
  GuidHob = GetFirstGuidHob (&gEfiMemoryTypeInformationGuid);
  if (GuidHob == NULL) {
    //
    // If Platform has not built Memory Type Info into the Hob, just return.
    //
    return;
  }
  PreviousMemoryTypeInformation = GET_GUID_HOB_DATA (GuidHob);
  VariableSize = GET_GUID_HOB_DATA_SIZE (GuidHob);

  //
  // Use a heuristic to adjust the Memory Type Information for the next boot
  //
  DEBUG ((EFI_D_INFO, "Memory  Previous  Current    Next   \n"));
  DEBUG ((EFI_D_INFO, " Type    Pages     Pages     Pages  \n"));
  DEBUG ((EFI_D_INFO, "======  ========  ========  ========\n"));

  for (Index = 0; PreviousMemoryTypeInformation[Index].Type != EfiMaxMemoryType; Index++) {

    for (Index1 = 0; CurrentMemoryTypeInformation[Index1].Type != EfiMaxMemoryType; Index1++) {
      if (PreviousMemoryTypeInformation[Index].Type == CurrentMemoryTypeInformation[Index1].Type) {
        break;
      }
    }
    if (CurrentMemoryTypeInformation[Index1].Type == EfiMaxMemoryType) {
      continue;
    }

    //
    // Previous is the number of pages pre-allocated
    // Current is the number of pages actually needed
    //
    Previous = PreviousMemoryTypeInformation[Index].NumberOfPages;
    Current  = CurrentMemoryTypeInformation[Index1].NumberOfPages;
    Next     = Previous;

    //
    // Write next varible to 125% * current and Inconsistent Memory Reserved across bootings may lead to S4 fail
    //
    if (Current < Previous) {
      if (BootMode == BOOT_WITH_DEFAULT_SETTINGS) {
        Next = Current + (Current >> 2);
      } else if (!MemoryTypeInformationVariableExists) {
        Next = MAX (Current + (Current >> 2), Previous);
      }
    } else if (Current > Previous) {
      Next = Current + (Current >> 2);
    }
    if (Next > 0 && Next < 4) {
      Next = 4;
    }

    if (Next != Previous) {
      PreviousMemoryTypeInformation[Index].NumberOfPages = Next;
      MemoryTypeInformationModified = TRUE;
    }

    DEBUG ((EFI_D_INFO, "  %02x    %08x  %08x  %08x\n", PreviousMemoryTypeInformation[Index].Type, Previous, Current, Next));
  }

  //
  // If any changes were made to the Memory Type Information settings, then set the new variable value;
  // Or create the variable in first boot.
  //
  if (MemoryTypeInformationModified || !MemoryTypeInformationVariableExists) {
    Status = gRT->SetVariable (
                    EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                    &gEfiMemoryTypeInformationGuid,
                    EFI_VARIABLE_NON_VOLATILE  | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    VariableSize,
                    PreviousMemoryTypeInformation
                    );

    //
    // If the Memory Type Information settings have been modified, then reset the platform
    // so the new Memory Type Information setting will be used to guarantee that an S4
    // entry/resume cycle will not fail.
    //
    if (MemoryTypeInformationModified && PcdGetBool (PcdResetOnMemoryTypeInformationChange)) {
      DEBUG ((EFI_D_INFO, "Memory Type Information settings change. Warm Reset!!!\n"));
      gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
    }
  }
}

/**
  This routine is kept for backward compatibility.
**/
VOID
EFIAPI
BdsLibSaveMemoryTypeInformation (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_EVENT                    ReadyToBootEvent;

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             BdsSetMemoryTypeInformationVariable,
             NULL,
             &ReadyToBootEvent
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR,"Bds Set Memory Type Informationa Variable Fails\n"));
  }
}


/**
  Identify a user and, if authenticated, returns the current user profile handle.

  @param[out]  User           Point to user profile handle.

  @retval EFI_SUCCESS         User is successfully identified, or user identification
                              is not supported.
  @retval EFI_ACCESS_DENIED   User is not successfully identified

**/
EFI_STATUS
EFIAPI
BdsLibUserIdentify (
  OUT EFI_USER_PROFILE_HANDLE         *User
  )
{
  EFI_STATUS                          Status;
  EFI_USER_MANAGER_PROTOCOL           *Manager;

  Status = gBS->LocateProtocol (
                  &gEfiUserManagerProtocolGuid,
                  NULL,
                  (VOID **) &Manager
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  return Manager->Identify (Manager, User);
}


/**
  The *BootOrderPtr[Index] hasn't Boot option, so delete the *BootOrderPtr[Index] and
  set the new L"BootOrder" variable

  @param  BootOrderPtr    IN  - the double pointer point to the original BootOrder
                          OUT - the double pointer point to the Update BootOrder
  @param  Index           Indicate the Offset of BootOrder is invalid
  @param  BootOrderSize   IN  - The pointer of oringinal Boot Order size
                          OUT - The pointer of udpate Boot Order size

  @retval EFI_SUCCESS               Udate the New L"BootOrder" variable successful
  @retval EFI_INVALID_PARAMETER     The inupt parameter is invalid

**/
EFI_STATUS
BdsLibUpdateInvalidBootOrder (
  IN OUT UINT16        **BootOrderPtr,
  IN     UINTN         Index,
  IN OUT UINTN         *BootOrderSize
  )
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


/**
  Deletete the Boot Option from EFI Variable. The Boot Order Arrray
  is also updated.

  @param OptionNumber    The number of Boot option want to be deleted.
  @param BootOrder       The Boot Order array.
  @param BootOrderSize   The size of the Boot Order Array.

  @retval  EFI_SUCCESS           The Boot Option Variable was found and removed
  @retval  EFI_UNSUPPORTED       The Boot Option Variable store was inaccessible
  @retval  EFI_NOT_FOUND         The Boot Option Variable was not found
**/
EFI_STATUS
EFIAPI
BdsLibDeleteBootOption (
  IN UINTN                       OptionNumber,
  IN OUT UINT16                  *BootOrder,
  IN OUT UINTN                   *BootOrderSize
  )
{
  UINT16      BootOption[100];
  UINTN       Index;
  EFI_STATUS  Status;
  UINTN       Index2Del;

  Status    = EFI_SUCCESS;
  Index2Del = 0;

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNumber);
  Status = gRT->SetVariable (
                  BootOption,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  0,
                  NULL
                  );

  //
  // adjust boot order array
  //
  for (Index = 0; Index < *BootOrderSize / sizeof (UINT16); Index++) {
    if (BootOrder[Index] == OptionNumber) {
      Index2Del = Index;
      break;
    }
  }

  if (Index != *BootOrderSize / sizeof (UINT16)) {
    for (Index = 0; Index < *BootOrderSize / sizeof (UINT16) - 1; Index++) {
      if (Index >= Index2Del) {
        BootOrder[Index] = BootOrder[Index + 1];
      }
    }

    *BootOrderSize -= sizeof (UINT16);
  }

  return Status;
}

/**
  Call Setup Utility by loading Setup Utility application.

  @retval EFI_SUCCESS    Call Setup Utility application successful
  @retval EFI_NOT_FOUND  Can not find Setup Utility application or load image fail
**/
EFI_STATUS
BdsLibCallScuApp (
  VOID
  )
{
  EFI_STATUS                          Status;
  UINTN                               HandleCount;
  EFI_HANDLE                          *HandleBuffer;
  EFI_GUID                            FileNameGuid = {0x3935B0A1, 0xA182, 0x4887, {0xbc, 0x56, 0x67, 0x55, 0x28, 0xe7, 0x88, 0x77}};
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH   FvFilePath;
  UINTN                               Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL       *Fv;
  UINTN                               Size;
  EFI_FV_FILETYPE                     Type;
  EFI_FV_FILE_ATTRIBUTES              Attributes;
  UINT32                              AuthenticationStatus;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *SetupAppDevicePath;
  EFI_HANDLE                          ImageHandle;
  UINTN                               ExitDataSize;
  CHAR16                              *ExitData;

  HandleCount  = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (Status != EFI_SUCCESS || HandleBuffer == NULL) {
    return EFI_NOT_FOUND;
  }

  EfiInitializeFwVolDevicepathNode (&FvFilePath, &FileNameGuid);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (!EFI_ERROR (Status)) {
      Status = Fv->ReadFile (
                     Fv,
                     &FileNameGuid,
                     NULL,
                     &Size,
                     &Type,
                     &Attributes,
                     &AuthenticationStatus
                     );
    }
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Create device path of setup utility application
    //
    DevicePath = DevicePathFromHandle (HandleBuffer[Index]);
    if (DevicePath == NULL) {
      continue;
    }
    SetupAppDevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &FvFilePath);
    if (SetupAppDevicePath == NULL) {
      continue;
    }

    Status = gBS->LoadImage (
                    TRUE,
                    gImageHandle,
                    SetupAppDevicePath,
                    NULL,
                    0,
                    &ImageHandle
                    );
    FreePool (SetupAppDevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }

    gBS->StartImage (ImageHandle, &ExitDataSize, &ExitData);
    break;
  }

  FreePool (HandleBuffer);

  return Status;
}

/**
  Start Setup Utility

  @retval EFI_SUCCESS
  @retval Other

**/
EFI_STATUS
BdsLibStartSetupUtility (
  BOOLEAN       PasswordCheck
  )
{
  EFI_STATUS                            Status;
  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;

  if (PasswordCheck) {
    Status = gBS->LocateProtocol (
                    &gEfiSetupUtilityProtocolGuid,
                    NULL,
                    (VOID **) &SetupUtility
                    );
    if (!EFI_ERROR (Status)) {
      Status = SetupUtility->PowerOnSecurity (SetupUtility);
    }
  } else {
    if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && BdsLibIsBootOrderHookEnabled ()) {
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
    Status = BdsLibCallScuApp ();
  }

  return Status;
}


/**
  check need update platform display device or not

  @param  SkipOriginalCode      TRUE : skip platform display device update
                                FALSE: need do platform display device update

  @retval EFI_SUCCESS   Get Platform display device policy successful
  @retval Other         Cannot get Platform display device policy successful

**/
EFI_STATUS
BdsLibDisplayDeviceReplace (
  OUT    BOOLEAN          *SkipOriginalCode
  )
{
  EFI_STATUS                    Status;

  if (SkipOriginalCode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *SkipOriginalCode = FeaturePcdGet (PcdBootDisplayDeviceReplace);

  //
  // OemServices
  //
  Status = OemSvcBootDisplayDeviceReplace (SkipOriginalCode);

  return Status;
}


EFI_STATUS
BdsLibOnStartOfBdsDiagnostics (
  VOID
  )
{
  EFI_STATUS                        Status;

  Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
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
  POST_CODE (BDS_END_OF_BOOT_SELECTION);
  Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
                  &gEndOfBdsBootSelectionProtocolGuid,
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

  Status = gST->ConOut->QueryMode (
                          gST->ConOut,
                          gST->ConOut->Mode->Mode,
                          &MaxX,
                          &MaxY
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (FeaturePcdGet (PcdDynamicHotKeySupported)) {
    DYNAMIC_HOTKEY_PROTOCOL         *DynamicHotKey;

    DynamicHotKey = NULL;
    Status = gBS->LocateProtocol (&gDynamicHotKeyProtocolGuid, NULL, (VOID **)&DynamicHotKey);
    if (!EFI_ERROR (Status)) {
      //
      //  Get POST string count
      //
      Status = DynamicHotKey->GetDynamicStringCount (DynamicHotKey, AfterSelect, &StringCount);
      if (!EFI_ERROR (Status)) {
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);

        for (Index = 0 ; Index < StringCount ; Index++) {
          //
          //  Get POST string
          //
          Status = DynamicHotKey->GetDynamicString (DynamicHotKey, Index, AfterSelect, &StringData, &LocX, &LocY);
          if (!EFI_ERROR (Status)) {
            DynamicHotKey->AdjustStringPosition (DynamicHotKey, FALSE, FALSE, (MaxY / 2), StringData, &LocX, &LocY);
            gST->ConOut->SetCursorPosition (gST->ConOut, LocX, LocY);
            BdsLibOutputStrings (gST->ConOut, StringData, NULL);
            gBS->FreePool (StringData);
          }
        }
        DynamicHotKey->AdjustStringPosition (DynamicHotKey, TRUE, FALSE, 0, NULL, NULL, NULL);
      }
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, CursorCol);

      return Status;
    }
  }

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  Status = Badging->GetStringCount (Badging, &StringCount);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
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
  Ptr += StrSize ((UINT16 *) Ptr);
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
{
  UINTN                        Size;
  UINT8                        BootType;

  Size = sizeof (UINT8);
  BootType = DUAL_BOOT_TYPE;
  gRT->GetVariable (
         L"BootType",
         &gSystemConfigurationGuid,
         NULL,
         &Size,
         &BootType
         );

  return BootType;
}

/**
  Performs a case-insensitive comparison of two Null-terminated strings.

  @param[in] Str1  A pointer to a Null-terminated string.
  @param[in] Str2  A pointer to a Null-terminated string.

  @retval 0        Str1 is equivalent to Str2.
  @return > 0      Str1 is lexically greater than Str2..
  @return < 0      Str1 is lexically less than Str2.

**/
STATIC
INTN
StriCmp (
  IN  CHAR16      *Str1,
  IN  CHAR16      *Str2
  )
{
  EFI_STATUS                         Status;
  EFI_UNICODE_COLLATION_PROTOCOL     *UnicodeCollation;

  Status = gBS->LocateProtocol (
                  &gEfiUnicodeCollation2ProtocolGuid,
                  NULL,
                  (VOID**) &UnicodeCollation
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    //
    // EFI_UNICODE_COLLATION2_PROTOCOL must be existence. If the protocol
    // doesn't exist, EFI DEBUG will be asserted and return -1 to indicate the two strings are different.
    //
    return -1;
  }

  return UnicodeCollation->StriColl (UnicodeCollation, Str1, Str2);
}

/**
  Check the two device paths whether have the same file path.

  @param  FirstDevicePath       First multi device path instances which need to check
  @param  SecondDevicePath      Second multi device path instances which need to check

  @retval TRUE   There is a matched device path instance
  @retval FALSE  There is no matched device path instance

**/
BOOLEAN
BdsLibMatchFilePathDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *FirstDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL   *SecondDevicePath
  )
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


/**
  Check is whether support administer secure boot or not.

  @param  Level         The memory test intensive level.

  @retval TRUE          Administer Secure boot is enabled.
  @retval FALSE         Administer Secure boot is disabled.

**/
BOOLEAN
IsAdministerSecureBootSupport (
  VOID
  )
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


/**
  Get next language from language code list (with separator ';').

  If LangCode is NULL, then ASSERT.
  If Lang is NULL, then ASSERT.

  @param  LangCode    On input: point to first language in the list. On
                                 output: point to next language in the list, or
                                 NULL if no more language in the list.
  @param  Lang           The first language in the list.

**/
VOID
EFIAPI
BdsLibGetNextLanguage (
  IN OUT CHAR8      **LangCode,
  OUT CHAR8         *Lang
  )
{
  UINTN  Index;
  CHAR8  *StringPtr;

  ASSERT (LangCode != NULL);
  ASSERT (*LangCode != NULL);
  ASSERT (Lang != NULL);

  Index = 0;
  StringPtr = *LangCode;
  while (StringPtr[Index] != 0 && StringPtr[Index] != ';') {
    Index++;
  }

  CopyMem (Lang, StringPtr, Index);
  Lang[Index] = 0;

  if (StringPtr[Index] == ';') {
    Index++;
  }
  *LangCode = StringPtr + Index;
}


/**

  Show progress bar with title above it. It only works in Graphics mode.


  @param TitleForeground Foreground color for Title.
  @param TitleBackground Background color for Title.
  @param Title           Title above progress bar.
  @param ProgressColor   Progress bar color.
  @param Progress        Progress (0-100)
  @param PreviousValue   The previous value of the progress.

  @retval  EFI_STATUS       Success update the progress bar

**/
EFI_STATUS
BdsLibShowProgress (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleForeground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleBackground,
  IN CHAR16                        *Title,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ProgressColor,
  IN UINTN                         Progress,
  IN UINTN                         PreviousValue
  )
{
  EFI_STATUS                     Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL          *UgaDraw;
  UINT32                         SizeOfX;
  UINT32                         SizeOfY;
  UINT32                         ColorDepth;
  UINT32                         RefreshRate;
  UINTN                          PosY;
  CHAR16                         *PrintStr;
  CHAR16                         *TmpStr;
  CHAR16                         StrPercent[16];

  if (Progress > 100) {
    return EFI_INVALID_PARAMETER;
  }

  UgaDraw = NULL;
  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput
                  );
  if (EFI_ERROR (Status) && FeaturePcdGet (PcdUgaConsumeSupport)) {
    GraphicsOutput = NULL;

    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiUgaDrawProtocolGuid,
                    (VOID **) &UgaDraw
                    );
  }
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  SizeOfX = 0;
  SizeOfY = 0;
  if (GraphicsOutput != NULL) {
    SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;
  } else if (UgaDraw != NULL) {
    Status = UgaDraw->GetMode (
                        UgaDraw,
                        &SizeOfX,
                        &SizeOfY,
                        &ColorDepth,
                        &RefreshRate
                        );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  PosY        = SizeOfY * 48 / 50;

  PrintXY (
    (SizeOfX - StrLen (Title) * EFI_GLYPH_WIDTH) / 2,
    PosY - EFI_GLYPH_HEIGHT - 1,
    &TitleForeground,
    &TitleBackground,
    Title
    );

  //
  // Show Memory test progress percentage
  //
  TmpStr = BdsLibGetStringById (STRING_TOKEN (STR_MEMORY_TEST_PERCENT));
  if (TmpStr != NULL) {
    UnicodeValueToString (StrPercent, 0, Progress, 0);
    StrCat (StrPercent, L"%");
    PrintStr = AllocateZeroPool (StrSize (StrPercent) + StrSize (TmpStr));
    if (PrintStr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (PrintStr, StrPercent, StrSize (StrPercent));
    StrCat (PrintStr, TmpStr);
    PrintXY (
      (SizeOfX - StrLen (PrintStr) * EFI_GLYPH_WIDTH) / 2,
      PosY - 1,
      &TitleForeground,
      &TitleBackground,
      PrintStr
      );
    gBS->FreePool (TmpStr);
    gBS->FreePool (PrintStr);
  }

  return EFI_SUCCESS;
}


/**
  Delete all of invalid boot options which option number is saved
  in L"BootOrder" variable.

  @retval EFI_SUCCESS   Delete invalid boot options successful.
  @retval Other         Delete invalid boot options failed.

**/
EFI_STATUS
BdsLibDeleteInvalidBootOptions (
  VOID
  )
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];


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
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      Index++;
      continue;
    }

    if (!ValidateOption (BootOptionVar, BootOptionSize)) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
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


/**
  Translate the first n characters of an Ascii string to
  Unicode characters. The count n is indicated by parameter
  Size. If Size is greater than the length of string, then
  the entire string is translated.

  @param  a             Pointer to input Ascii string.
  @param  Size          The number of characters to translate.
  @param  u             Pointer to output Unicode string buffer.

**/
VOID
BdsLibAsciiToUnicodeSize (
  IN UINT8              *a,
  IN UINTN              Size,
  OUT UINT16            *u
  )
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


/**
  Function to update the ATA strings into Model Name -- Size

  @param  IdentifyDriveInfo
  @param  BufSize
  @param  BootString

  @retval EFI_SUCCESS   Will return model name and size (or ATAPI if non-ATA)
  @retval Other

**/
EFI_STATUS
BdsLibUpdateAtaString (
  IN      EFI_IDENTIFY_DATA                 *IdentifyDriveInfo,
  IN      UINTN                             BufSize,
  IN OUT  CHAR16                            **BootString
  )
{
  CHAR8                                     *TempString;
  UINT16                                    Index;
  CHAR8                                     Temp8;

  TempString = AllocateZeroPool (0x100);
  if (TempString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (
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
  BdsLibAsciiToUnicodeSize((UINT8 *) TempString, BufSize, *BootString);
  if (TempString != NULL) {
    gBS->FreePool(TempString);
  }

  return EFI_SUCCESS;
}


/**
  Clean space character in the front and back of input string.

  @param  Str           Input string

**/
VOID
CleanSpaceChar (
  IN    CHAR16    *Str
  )
{
  UINTN     StrLength;
  UINTN     Start;
  UINTN     End;
  UINTN     ValidLength;
  CHAR16    SpaceChar = ' ';

  if (Str == NULL) {
    return;
  }

  StrLength = StrLen (Str);
  if (StrLength == 0) {
    return;
  }

  Start = 0;
  End   = StrLength - 1;

  while (Str[Start] == SpaceChar) {
    Start++;
  }
  if (Start == StrLength) {
    //
    // All chars are space char, no need to remove space chars.
    //
    return;
  }

  while (Str[End] == SpaceChar) {
    End--;
  }

  ValidLength = End - Start + 1;
  if (ValidLength < StrLength) {
    gBS->CopyMem (&Str[0], &Str[Start], ValidLength * sizeof(CHAR16));
    gBS->SetMem (&Str[ValidLength], (StrLength - ValidLength) * sizeof(CHAR16), 0);
  }
}


/**
  Get all hardware boot device information (Block IO device path and device name).

  @param  HwBootDeviceInfoCount    Number of hardware boot device information
  @param  HwBootDeviceInfo         Array pointer of hardware boot device information

  @retval EFI_SUCCESS   Get information successfully
  @retval Other         Locate protocol fail or get usb device information fail

**/
EFI_STATUS
BdsLibGetAllHwBootDeviceInfo (
  OUT UINTN                          *HwBootDeviceInfoCount,
  OUT HARDWARE_BOOT_DEVICE_INFO      **HwBootDeviceInfo
  )
{
  EFI_STATUS                     Status;
  UINTN                          HandleCount;
  EFI_HANDLE                     *HandleBuffer;
  UINTN                          Index;
  EFI_DISK_INFO_PROTOCOL         *DiskInfo;
  EFI_DEVICE_PATH_PROTOCOL       *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *DevicePathNode;
  EFI_DEVICE_PATH_PROTOCOL       *TempDevicePathNode;
  PCI_DEVICE_PATH                *PciDevicePath;
  UINT32                         IdeChannel;
  UINT32                         IdeDevice;
  ATAPI_IDENTIFY                 IdentifyDrive;
  UINT32                         Size;
  UINTN                          DeviceNameSize;
  CHAR16                         *DeviceName;
  BOOLEAN                        Found;
  EFI_COMPONENT_NAME2_PROTOCOL   *ComponentName2;
  CHAR16                         *DriverName;
  UINTN                          HwBootDeviceInfoLength;


  if (HwBootDeviceInfoCount == NULL || HwBootDeviceInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HwBootDeviceInfoCount = 0;
  HwBootDeviceInfoLength = MAX_BOOT_DEVICES_NUMBER;
  *HwBootDeviceInfo = AllocatePool (HwBootDeviceInfoLength * sizeof(HARDWARE_BOOT_DEVICE_INFO));
  if (*HwBootDeviceInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  HandleCount = 0;
  PciDevicePath = NULL;
  DeviceNameSize = 100;
  DeviceName = AllocateZeroPool (DeviceNameSize);
  if (DeviceName == NULL) {
    gBS->FreePool (*HwBootDeviceInfo);
    *HwBootDeviceInfo = NULL;
    return EFI_OUT_OF_RESOURCES;
  }


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
           (VOID **) &DiskInfo
           );
    if ((CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid)) ||
        (CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid))) {
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

          if ((*HwBootDeviceInfoCount) >= HwBootDeviceInfoLength) {
            *HwBootDeviceInfo = ReallocatePool (
                                  HwBootDeviceInfoLength * sizeof(HARDWARE_BOOT_DEVICE_INFO),
                                  (HwBootDeviceInfoLength + MAX_BOOT_DEVICES_NUMBER) * sizeof(HARDWARE_BOOT_DEVICE_INFO),
                                  *HwBootDeviceInfo
                                  );
            HwBootDeviceInfoLength += MAX_BOOT_DEVICES_NUMBER;
          }

          (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].HwDeviceName = AllocateZeroPool (StrSize (DeviceName));
          gBS->CopyMem (
                 (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].HwDeviceName,
                 DeviceName,
                 StrSize (DeviceName)
                 );

          (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].BlockIoDevicePath = DevicePath;
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
  ComponentName2 = NULL;
  Found = FALSE;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiComponentName2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiComponentName2ProtocolGuid,
                      (VOID **) &ComponentName2
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      Status = ComponentName2->GetDriverName (
                                 ComponentName2,
                                 LANGUAGE_CODE_ENGLISH_RFC4646,
                                 &DriverName
                                 );
      if (!EFI_ERROR (Status) && !StrCmp (DriverName, L"Usb Mass Storage Driver")) {
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
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiUsbIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (!EFI_ERROR (Status) && Found) {
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID *) &DevicePath
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }
      Status = ComponentName2->GetControllerName (
                                 ComponentName2,
                                 HandleBuffer[Index],
                                 NULL,
                                 LANGUAGE_CODE_ENGLISH_RFC4646,
                                 &DeviceName
                                 );
      if (EFI_ERROR (Status)) {
        continue;
      }

      if ((*HwBootDeviceInfoCount) >= HwBootDeviceInfoLength) {
        *HwBootDeviceInfo = ReallocatePool (
                              HwBootDeviceInfoLength * sizeof(HARDWARE_BOOT_DEVICE_INFO),
                              (HwBootDeviceInfoLength + MAX_BOOT_DEVICES_NUMBER) * sizeof(HARDWARE_BOOT_DEVICE_INFO),
                              *HwBootDeviceInfo
                              );
        HwBootDeviceInfoLength += MAX_BOOT_DEVICES_NUMBER;
      }

      (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].BlockIoDevicePath = DevicePath;
      (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].HwDeviceName      = AllocateCopyPool (StrSize (DeviceName), DeviceName);
      CleanSpaceChar ((*HwBootDeviceInfo)[*HwBootDeviceInfoCount].HwDeviceName);

      (*HwBootDeviceInfoCount)++;
    }
  }

  if (HandleCount) {
    gBS->FreePool (HandleBuffer);
  }

  if (*HwBootDeviceInfoCount == 0) {
    gBS->FreePool (*HwBootDeviceInfo);
    *HwBootDeviceInfo = NULL;
  }

  return EFI_SUCCESS;
}


/**
  If input is a hard drive device path, append it to corresponding BlockIo device path.
  If input is not a hard drive device path, output NULL.

  @param  HardDriveDevicePath         Input device path

  @return Device path which combines BlockIo and hard drive device path

**/
EFI_DEVICE_PATH_PROTOCOL *
AppendHardDrivePathToBlkIoDevicePath (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
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
      NewDevicePath = AppendDevicePath (BlockIoDevicePath, DevicePath);

      gBS->FreePool (BlockIoBuffer);
      return NewDevicePath;
    }
  }

  gBS->FreePool (BlockIoBuffer);
  return NULL;
}


/**
  Compare with each BlockIo device path.

  @param  HardDriveDevicePath      BlockIo device path
  @param  DevicePath               BlockIo device path or hard drive device path

  @retval TRUE           BlockIo device paths are the same
  @retval FALSE          BlockIo device paths are different

**/
BOOLEAN
BdsLibCompareBlockIoDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *BlockIoDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
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
    if (CompareMem (BlockIoDevicePath, TempDevicePath, DevicePathNodeLength (BlockIoDevicePath)) != 0) {
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


/**
  According to option number to check this boot option is whether
  dummy boot option.

  @param  OptionNum      UINT16 to save boot option number.

  @retval TRUE           This boot option is dummy boot option.
  @retval FALSE          This boot option isn't dummy boot option.

**/
BOOLEAN
BdsLibIsDummyBootOption (
  UINT16       OptionNum
  )
{
  if (OptionNum > DummyBootOptionStartNum && OptionNum < DummyBootOptionEndNum) {
    return TRUE;
  }

  return FALSE;
}


/**
  This function allocates memory to generate load option. It is caller's responsibility
  to free load option if caller no longer requires the content of load option.

  @param  DevicePath         Pointer to a packed array of UEFI device paths.
  @param  Description        The user readable description for the load option.
  @param  OptionalData       Pointer to optional data for load option.
  @param  OptionalDataSize   The size of optional data.
  @param  LoadOption         Double pointer to load option.
  @param  LoadOptionSize     The load option size by byte.

  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory failed.
  @retval EFI_SUCCESS            Generate load option successful.

**/
EFI_STATUS
BdsLibCreateLoadOption (
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize,
  OUT VOID                           **LoadOption,
  OUT UINTN                          *LoadOptionSize
  )
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

  OptionSize = sizeof (UINT32) + sizeof (UINT16) + StrSize (Description) + GetDevicePathSize (DevicePath) + OptionalDataSize;
  OptionPtr  = AllocateZeroPool (OptionSize);
  if (OptionPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  WorkingPtr = (UINT8 *) OptionPtr;
  *(UINT32 *) WorkingPtr = LOAD_OPTION_ACTIVE;
  WorkingPtr += sizeof (UINT32);
  *(UINT16 *) WorkingPtr = (UINT16) GetDevicePathSize (DevicePath);
  WorkingPtr += sizeof (UINT16);
  CopyMem (WorkingPtr, Description, StrSize (Description));
  WorkingPtr += StrSize (Description);
  CopyMem (WorkingPtr, DevicePath, GetDevicePathSize (DevicePath));
  WorkingPtr += GetDevicePathSize (DevicePath);
  CopyMem (WorkingPtr, OptionalData, OptionalDataSize);

  *LoadOption = OptionPtr;
  *LoadOptionSize = OptionSize;

  return EFI_SUCCESS;
}


/**
  This function restores the contents of PHYSICAL_BOOT_ORDER_NAME variable to
  "BootOrder"  variable.

  @retval EFI_SUCCESS      Restore the contents of "BootOrder" variable successful.
  @retval Other            Any error occurred while restoring data to "BootOrder" variable.

**/
EFI_STATUS
BdsLibRestoreBootOrderFromPhysicalBootOrder (
  VOID
  )
{
  EFI_STATUS      Status;
  UINT16          *PhysicalBootOrder;
  UINTN           PhysicalBootOrderSize;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }
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


/**
  This function uses to synchronize the contents of PHYSICAL_BOOT_ORDER_NAME variable with
  the contents of "BootOrder" variable.

  @retval EFI_SUCCESS      Synchronize PHYSICAL_BOOT_ORDER_NAME variable successful.
  @retval Other            Any error occurred while Synchronizing PHYSICAL_BOOT_ORDER_NAME variable.

**/
EFI_STATUS
BdsLibSyncPhysicalBootOrder (
  VOID
  )
{
  EFI_STATUS      Status;
  UINT16          *BootOrder;
  UINTN           BootOrderSize;
  BOOLEAN         IsPhysicalBootOrder;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }
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


/**
  According to input boot order to get the boot order type.

  @param  BootOrder              Pointer to BootOrder array.
  @param  BootOrderNum           The boot order number in boot order array.
  @param  PhysicalBootOrder      BOOLEAN value to indicate is whether physical boot order.

  @retval EFI_INVALID_PARAMETER  BootOrder is NULL, BootOrderNum is 0 or PhysicalBootOrder is NULL.
  @retval EFI_SUCCESS            Determine the boot order is physical boot order or virtual boot order successful.

**/
EFI_STATUS
BdsLibGetBootOrderType (
  IN  UINT16        *BootOrder,
  IN  UINTN         BootOrderNum,
  OUT BOOLEAN       *PhysicalBootOrder
  )
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


/**
  According to boot option number to get the description from this boot option.
  It is caller's responsibility to free the description if caller no longer requires
  the content of description.

  @param  OptionNum              The boot option number.

  @return NULL                   Get description from boot option failed.
  @return Other                  Get description from boot option successful.

**/
CHAR16 *
BdsLibGetDescriptionFromBootOption (
  IN UINT16     OptionNum
  )
{
  UINT16                     BootOption[BOOT_OPTION_MAX_CHAR];
  VOID                       *BootOptionVar;
  UINTN                      VariableSize;
  UINT8                      *WorkingPtr;
  CHAR16                     *Description;
  UINTN                      DescriptionSize;

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNum);
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
  DescriptionSize = StrSize ((CHAR16 *) WorkingPtr);

  //
  // Copy the contents of device path to allocated memory
  //
  Description = AllocateZeroPool (DescriptionSize);
  CopyMem (Description, WorkingPtr, DescriptionSize);
  gBS->FreePool (BootOptionVar);

  return Description;
}


/**
  According to boot option number to get the device path from this boot option.
  It is caller's responsibility to free the device path if caller no longer requires
  the content of device path.

  @param  OptionNum              The boot option number.

  @return NULL                   Get device path from boot option failed.
  @return Other                  Get device path from boot option successful.

**/
EFI_DEVICE_PATH_PROTOCOL *
BdsLibGetDevicePathFromBootOption (
  IN  UINT16     OptionNum
  )
{
  UINT16                     BootOption[BOOT_OPTION_MAX_CHAR];
  VOID                       *BootOptionVar;
  UINTN                      VariableSize;
  UINT8                      *WorkingPtr;
  CHAR16                     *Description;
  UINT16                     DevicePathLength;
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNum);
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
  WorkingPtr += StrSize (Description);

  //
  // Copy the contents of device path to allocated memory
  //
  DevicePath = AllocateZeroPool (DevicePathLength);
  CopyMem (DevicePath, WorkingPtr, DevicePathLength);
  gBS->FreePool (BootOptionVar);

  return DevicePath;
}


/**
  According to boot option number to check this boot option is whether a virtual boot option.

  @param  OptionNum              The boot option number.

  @retval TRUE                   This is a virtual boot option.
  @retval FALSE                  This isn't a virtual boot option.

**/
BOOLEAN
IsVirtualBootOption (
  IN  UINT16     OptionNum
  )
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  BOOLEAN                    IsVritual;
  UINT32                     DevPathType;

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
  DevPathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (DevPathType == BDS_EFI_MEDIA_FV_FILEPATH_BOOT || DevPathType == BDS_EFI_MEDIA_HD_BOOT ||
      DevPathType == BDS_LEGACY_BBS_BOOT) {
    IsVritual = TRUE;
  }

  gBS->FreePool (DevicePath);
  return IsVritual;
}


/**
  According to boot option number to check this boot option is whether an EFI USB boot option.

  @param  OptionNum              The boot option number.

  @retval TRUE                   This is an EFI USB boot option.
  @retval FALSE                  This isn't an EFI USB boot option.

**/
STATIC
BOOLEAN
IsUefiUsbBootOption (
  IN  UINT16     OptionNum
  )
{
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;
  BOOLEAN                    IsUsb;
  UINT32                     DevPathType;

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsUsb = FALSE;
  if (!((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP))) {
    DevPathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
    if (DevPathType == BDS_EFI_MESSAGE_USB_DEVICE_BOOT) {
      IsUsb = TRUE;
    }
  }

  gBS->FreePool (DevicePath);
  return IsUsb;
}


/**
  According to boot option number to check this boot option is whether an EFI DVD/CDROM boot option.

  @param  OptionNum              The boot option number.

  @retval TRUE                   This is an EFI DVD/CDROM boot option.
  @retval FALSE                  This isn't an EFI DVD/CDROM boot option.

**/
STATIC
BOOLEAN
IsUefiCdBootOption (
  IN  UINT16     OptionNum
  )
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


/**
  According to boot option number to check this boot option is whether an EFI network boot option.

  @param  OptionNum              The boot option number.

  @retval TRUE                   This is an EFI network boot option.
  @retval FALSE                  This isn't an EFI network boot option.

**/
STATIC
BOOLEAN
IsUefiNetworkBootOption (
  IN  UINT16     OptionNum
  )
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


/**
  This function uses to remove all of physical boot options and add virtual boot
  options to "BootOrder' variable

  @retval EFI_SUCCESS             Change the contents of "BootOrder" successful
  @retval EFI_NOT_FOUND           "BootOrder" variable doesn't exist.
  @retval Other                   Set "BootOrder" variable failed.

**/
EFI_STATUS
BdsLibChangeToVirtualBootOrder (
  VOID
  )
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

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }
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
    VirtualBootOrder = AllocateZeroPool (BootOrderSize + sizeof (UINT16) * (DummyBootOptionEndNum - DummyBootOptionStartNum));
    if (VirtualBootOrder == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

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


/**
  This function uses to get all of corresponding mapping boot option numbers from "BootOrder" variable.

  @param  OptionNum               Input option number
  @param  OptionCount             Boot option count.
  @param  OptionOrder             Double pointer to option order to saved all of UEFI USB boot option.

  @retval EFI_SUCCESS             Get UEFI USB boot option from "BootOrder" variable successful
  @retval EFI_INVALID_PARAMETER   OptionCount, OptionOrder is NULL or OptionNum is not a dummy boot option.
  @retval EFI_NOT_FOUND           Cannot find corresponding option type check function.

**/
EFI_STATUS
BdsLibGetMappingBootOptions (
  IN    UINT16         OptionNum,
  OUT   UINTN          *OptionCount,
  OUT   UINT16         **OptionOrder
  )
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
    *OptionCount = 0;
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
    MappingBootOrder = AllocateZeroPool (BootOrderSize);
    if (MappingBootOrder == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

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


/**
  Update the BlockIo device path of target boot device into TargetHddDevPath variable.
  Assume target boot device is HDD in the first boot option in BootOrder.

  @retval EFI_SUCCESS             Update target HDD variable success.
  @retval EFI_NOT_FOUND           BootOrder or Boot#### variable does not exist.

**/
EFI_STATUS
UpdateTargetHddVariable (
  VOID
  )
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
                        (VOID **) &BlockIoDevicePath
                        );
        if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
          continue;
        }

        if (MatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath)) {
          gRT->SetVariable (
                 L"TargetHddDevPath",
                 &gEfiGenericVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                 GetDevicePathSize (BlockIoDevicePath),
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


/**
  Check target device is valid or not by comparing device path from TargetHddDevPath variable.

  @retval TRUE                    Target device is valid.
  @retval FALSE                   Target device is not valid.

**/
BOOLEAN
TargetDevIsValid (
  VOID
  )
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


/**
  Check Win 8 Fast Boot feature is active or not.

  @retval TRUE                    Win 8 Fast Boot feature is active.
  @retval FALSE                   Win 8 Fast Boot feature is not active.

**/
BOOLEAN
BdsLibIsWin8FastBootActive (
  VOID
  )
{
  EFI_STATUS                         Status;
  KERNEL_CONFIGURATION               SystemConfiguration;
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

  Status = GetKernelConfiguration (&SystemConfiguration);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (SystemConfiguration.Win8FastBoot == 0 &&
      BdsLibGetBootType () == EFI_BOOT_TYPE) {

    switch (SystemConfiguration.NewPositionPolicy) {

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

/**
  After checking system password, the checking flag is set to disable.
  Using this function to set the checking flag enable

  @retval EFI_SUCCESS             set the checking flag successful.

**/
EFI_STATUS
SetSysPasswordCheck (
  VOID
  )
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService;
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;
  UINT32                                  SetupBits;

  SetupBits = 0;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SysPasswordService
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

/**
 Based on the boot option number, return the dummy boot option number.
 If return DummyBootOptionEndNum, it means this boot option does not belong to dummy boot option.

 @param[in] OptionNum           The boot option number.

 @return Dummy boot option number or DummyBootOptionEndNum if input boot option does not belong to dummy boot option.
**/
DUMMY_BOOT_OPTION_NUM
BdsLibGetDummyBootOptionNum (
  IN  UINT16                 OptionNum
  )
{
  UINTN          Index;

  for (Index = 0; Index < OPTION_TYPE_TABLE_COUNT; Index++) {
    if (mOptionTypeTable[Index].CheckOptionType (OptionNum)) {
      return mOptionTypeTable[Index].OptionNum;
    }
  }

  return DummyBootOptionEndNum;
}

/**
  Internal function to uninstall all of specific protocols in all of handles.

  @param[in] Protocol    Provides the protocol to remove.

  @retval EFI_SUCCESS    Uninstall all of protcols successfully.
  @return Other          Any other error occurred while uninstalling protocols.
**/
STATIC
EFI_STATUS
UninstallSpecificProtocols (
  EFI_GUID         *Protocol
  )
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

/**
  Enable BootOrder variable hook mechanism.

  @retval EFI_SUCCESS    Enable BootOrder variable hook mechanism successfully.
  @return Other          Enable BootOrder variable hook mechanism failed.
**/
EFI_STATUS
BdsLibEnableBootOrderHook (
  VOID
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  VOID                *Interface;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }

  Status = UninstallSpecificProtocols (&gBootOrderHookDisableGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gBootOrderHookEnableGuid,
                  NULL,
                  (VOID **)&Interface
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

/**
  Disable BootOrder variable hook mechanism.

  @retval EFI_SUCCESS    Disable BootOrder variable hook mechanism successfully.
  @return Other          Disable BootOrder variable hook mechanism failed.
**/
EFI_STATUS
BdsLibDisableBootOrderHook (
  VOID
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  VOID                *Interface;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }

  Status = UninstallSpecificProtocols (&gBootOrderHookEnableGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gBootOrderHookDisableGuid,
                  NULL,
                  (VOID **)&Interface
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

/**
  Function uses to check BootOrder variable hook mechanism is whether enabled.

  @retval TRUE    BootOrder variable hook mechanism is enabled.
  @retval FALSE   BootOrder variable hook mechanism is disabled
**/
BOOLEAN
BdsLibIsBootOrderHookEnabled (
  VOID
  )
{
  EFI_STATUS      Status;
  VOID            *BootOrderHook;

  Status = gBS->LocateProtocol (
                  &gBootOrderHookEnableGuid,
                  NULL,
                  (VOID **)&BootOrderHook
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}

/**
  According the Bus, Device, Function to check this controller is in Port Number Map table or not.

  @param  Bus                   PCI bus number
  @param  Device                PCI device number
  @param  Function              PCI function number

  @return TRUE                  This is a on board PCI device.
  @return FALSE                 Not on board device.

**/
BOOLEAN
IsOnBoardPciDevice (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function
  )
{
  PORT_NUMBER_MAP       *PortMappingTable;
  PORT_NUMBER_MAP       EndEntry;
  UINTN                 NoPorts;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    if ((PortMappingTable[NoPorts].Bus == Bus) &&
        (PortMappingTable[NoPorts].Device == Device) &&
        (PortMappingTable[NoPorts].Function == Function)) {
      return TRUE;
    }
    NoPorts++;
  }

  return FALSE;
}
