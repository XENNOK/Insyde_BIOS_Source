//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
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

  BBSsupport.c

Abstract:

  This function deal with the legacy boot option, it create, delete
  and manage the legacy boot option, all legacy boot option is getting from
  the legacy BBS table.

--*/

#include "BBSsupport.h"
#include "GenericBdsLib.h"
#include EFI_PROTOCOL_DEFINITION (LegacyBios)
#include EFI_PROTOCOL_DEFINITION (EfiSetupUtility)
#include "SetupConfig.h"
#include EFI_PROTOCOL_DEFINITION (ChipsetLibServices)

EFI_DEVICE_PATH_PROTOCOL  EndDevicePath[] = {
  END_DEVICE_PATH_TYPE,
  END_ENTIRE_DEVICE_PATH_SUBTYPE,
  END_DEVICE_PATH_LENGTH,
  0
};


UINTN
UnicodeToAscii (
  IN  CHAR16  *UStr,
  IN  UINTN   Length,
  OUT CHAR8   *AStr
  )
/*++
Routine Description:

  change a Unicode string t ASCII string

Arguments:

  UStr   - Unicode string
  Lenght - most possible length of AStr
  AStr   - ASCII string to pass out

Returns:

  Actuall length

--*/
{
  UINTN Index;

  //
  // just buffer copy, not character copy
  //
  for (Index = 0; Index < Length; Index++) {
    *AStr++ = (CHAR8) *UStr++;
  }

  return Index;
}

VOID
BdsBuildLegacyDevNameString (
  IN BBS_TABLE                 *CurBBSEntry,
  IN HDD_INFO                  *HddInfo,
  IN UINTN                     Index,
  IN UINTN                     BufSize,
  OUT CHAR16                   *BootString
  )
{
  UINT8                        *StringDesc;
  UINT16                       Channel;
  UINT16                       Device;

  EfiZeroMem (BootString, BufSize);

  if (Index == 0) {
    SPrint (BootString, BufSize, L"%s", L"Floppy");
    return;
  }

  //
  // If current BBS entry has its description then use it.
  //
  StringDesc = (UINT8*)(UINTN)((CurBBSEntry->DescStringSegment << 4) +
                 CurBBSEntry->DescStringOffset);
  if (StringDesc != NULL) {
    //
    // Only get fisrt 32 characters, this is suggested by BBS spec
    //
    BdsLibAsciiToUnicodeSize (
      StringDesc,
      32,
      BootString
      );
  //
  // Maximum Onboard devices 8 IDE * 2 (Master/Slave) + Floppy
  //
  } else if (Index < (8 * 2 + 1)) {
    //
    // Get the drive information data. Odd indices are masters and even indices are slaves.
    //
    Channel = (UINT16)((Index - 1) / 2);
    Device  = (UINT16)((Index - 1) % 2);
    BdsLibUpdateAtaString (
      (EFI_IDENTIFY_DATA *) &HddInfo[Channel].IdentifyDrive[Device],
      BufSize,
      &BootString
      );
  } else {
    SPrint (
      BootString,
      BufSize, L"%s",
      L"Unknow Device"
      );
  }

  return;
}

EFI_STATUS
BdsCreateLegacyBootOption (
  IN BBS_TABLE                        *CurrentBbsEntry,
  IN HDD_INFO                         *HddInfo,
  IN EFI_DEVICE_PATH_PROTOCOL         *CurrentBbsDevPath,
  IN UINTN                            Index,
  IN OUT UINT16                       **BootOrderList,
  IN OUT UINTN                        *BootOrderListSize
  )
/*++

  Routine Description:

    Create a legacy boot option for the specified entry of
    BBS table, save it as variable, and append it to the boot
    order list.

  Arguments:

    CurrentBbsEntry        - Pointer to current BBS table.
    CurrentBbsDevPath      - Pointer to the Device Path Protocol instance of BBS
    Index                  - Index of the specified entry in BBS table.
    BootOrderList          - On input, the original boot order list.
                             On output, the new boot order list attached with the
                             created node.
    BootOrderListSize      - On input, the original size of boot order list.
                           - On output, the size of new boot order list.

  Returns:

    EFI_SUCCESS            - Boot Option successfully created.
    EFI_OUT_OF_RESOURCES   - Fail to allocate necessary memory.
    Other                  - Error occurs while setting variable.

--*/
{
  EFI_STATUS  Status;
  UINT16      CurrentBootOptionNo;
  UINT16      BootString[10];
  UINT16      BootDesc[100];
  UINT16      *NewBootOrderList;
  UINTN       BufferSize;
  VOID        *Buffer;
  UINT8       *Ptr;
  UINT16      CurrentBbsDevPathSize;
  UINTN                                      BootOptionNum = 0;
  EFI_BOOT_OPTION_POLICY_PROTOCOL           *BootOptionPolicy;
  UINTN                                      NewPosition = 0;
  UINTN                                      BootOptionType = 0;
  UINTN                                      DevicePathProtocolSize;
  EFI_DEVICE_PATH_PROTOCOL                   *DevicePathProtocol;

  if (NULL == (*BootOrderList)) {
    CurrentBootOptionNo = 0;
  } else {
    BootOptionNum = (UINTN) (*BootOrderListSize) / sizeof (UINT16);
    GetNewBootOptionNo (
      *BootOrderList,
      BootOptionNum,
      &CurrentBootOptionNo
      );
  }

  SPrint (
    BootString,
    sizeof (BootString),
    L"Boot%04x",
    CurrentBootOptionNo
    );

  BdsBuildLegacyDevNameString (CurrentBbsEntry, HddInfo, Index, sizeof (BootDesc), BootDesc);

  CurrentBbsDevPathSize = (UINT16) (EfiDevicePathSize (CurrentBbsDevPath));

  DevicePathProtocolSize = 0;
  DevicePathProtocol = (EFI_DEVICE_PATH_PROTOCOL *) (UINTN) CurrentBbsEntry->IBV2;
  if (DevicePathProtocol != NULL) {
    DevicePathProtocolSize = EfiDevicePathSize (DevicePathProtocol);
  }

  BufferSize = sizeof (UINT32) +
    sizeof (UINT16) +
    EfiStrSize (BootDesc) +
    CurrentBbsDevPathSize +
    sizeof (BBS_TABLE) +
    sizeof (UINT16) +
    DevicePathProtocolSize;

  Buffer = EfiAllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr               = (UINT8 *) Buffer;

  *((UINT32 *) Ptr) = LOAD_OPTION_ACTIVE;
  Ptr += sizeof (UINT32);

  *((UINT16 *) Ptr) = CurrentBbsDevPathSize;
  Ptr += sizeof (UINT16);

  EfiCopyMem (
    Ptr,
    BootDesc,
    EfiStrSize (BootDesc)
    );
  Ptr += EfiStrSize (BootDesc);

  EfiCopyMem (
    Ptr,
    CurrentBbsDevPath,
    CurrentBbsDevPathSize
    );
  Ptr += CurrentBbsDevPathSize;

  EfiCopyMem (
    Ptr,
    CurrentBbsEntry,
    sizeof (BBS_TABLE)
    );

  Ptr += sizeof (BBS_TABLE);
  *((UINT16 *) Ptr) = (UINT16) Index;

  if (DevicePathProtocolSize != 0) {
    Ptr += sizeof (UINT16);
    EfiCopyMem (
      Ptr,
      DevicePathProtocol,
      DevicePathProtocolSize
      );
  }

  Status = gRT->SetVariable (
                  BootString,
                  &gEfiGlobalVariableGuid,
                  VAR_FLAG,
                  BufferSize,
                  Buffer
                  );

  SafeFreePool (Buffer);
  Buffer = NULL;

  //
  // find the first Legacy BootOption in BootOrder
  //
  Status = gBS->LocateProtocol (
                  &gEfiBootOptionPolicyProtocolGuid,
                  NULL,
                  &BootOptionPolicy
                  );

  if (!EFI_ERROR (Status)) {
    BootOptionType = LEGACY_BOOT_DEV;
    BootOptionPolicy->FindPositionOfNewBootOption (
                        BootOptionPolicy,
                        CurrentBbsDevPath,
                        *BootOrderList,
                        BootOptionType,
                        BootOptionNum,
                        CurrentBootOptionNo,
                        &NewPosition
                        );
  } else {
    NewPosition = 0;
  }

   //
   // Insert new BootOption number to BootOrder
   //
  BdsLibNewBootOptionPolicy(
    &NewBootOrderList,
    *BootOrderList,
    BootOptionNum,
    CurrentBootOptionNo,
    NewPosition
    );

  *BootOrderListSize += sizeof (UINT16);
  SafeFreePool (*BootOrderList);
  *BootOrderList = NewBootOrderList;

  return Status;
}

EFI_STATUS
BdsDeleteBootOption (
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

  SPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNumber);
  Status = EfiLibDeleteVariable (BootOption, &gEfiGlobalVariableGuid);
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

EFI_DEVICE_PATH_PROTOCOL *
GetDevicePathProtocolFromVar (
  IN UINT8                         *BootOptionVar,
  IN UINTN                         BootOptionVarSize
  )
/*++

Routine Description:

  Get device path data from optional data of boot option variable.

Arguments:

  BootOptionVar          - Pointer to boot option variable
  BootOptionVarSize      - Size of boot option variable

Returns:

  Pointer to device path protocol or NULL if variable is not support or invalid parameter.

--*/
{
  UINT8                     *Ptr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT16                    DevPathSize;

  if (BootOptionVar == NULL || BootOptionVarSize == 0) {
    return NULL;
  }

  Ptr = BootOptionVar;
  Ptr += sizeof (UINT32);
  DevPathSize = *(UINT16 *) Ptr;
  Ptr += sizeof (UINT16);
  Ptr += EfiStrSize ((UINT16 *) Ptr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;

  if ((DevicePath->Type != BBS_DEVICE_PATH) || (DevicePath->SubType != BBS_BBS_DP)) {
    return NULL;
  }

  Ptr += (DevPathSize + sizeof (BBS_TABLE) + sizeof (UINT16));
  if ((UINTN) (Ptr - BootOptionVar) == BootOptionVarSize) {
    return NULL;
  }

  return (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
}

EFI_STATUS
GetBbsIndexByDevicePathProtocol (
  IN  BBS_TABLE                    *BbsTable,
  IN  UINT16                       BbsCount,
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePathProtocol,
  OUT UINT16                       *BbsIndex
  )
/*++

Routine Description:

  Get bbs index from bbs table by comparing device path.

Arguments:

  BbsTable               - Pointer to bbs table
  BbsCount               - Number of bbs table
  DevicePathProtocol     - Pointer to device path protocol.
  BbsIndex               - Pointer to bbs index

Returns:

  EFI_SUCCESS            - Get bbs index successfully.
  EFI_INVALID_PARAMETER  - Bbs table pointer is NULL or device path pointer is NULL.
  EFI_NOT_FOUND          - There is no match bbs index.

--*/
{
  UINT16                           Index;
  UINTN                            DevicePathProtocolSize;
  EFI_DEVICE_PATH_PROTOCOL         *BbsTableDevicePathProtocol;

  if (BbsTable == NULL || DevicePathProtocol == NULL || BbsIndex == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevicePathProtocolSize = EfiDevicePathSize (DevicePathProtocol);

  for (Index = 0; Index < BbsCount; Index++) {
    if ((BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) ||
        (BbsTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM)) {
      continue;
    }

    BbsTableDevicePathProtocol = (EFI_DEVICE_PATH_PROTOCOL *) (UINTN) BbsTable[Index].IBV2;

    if (BbsTableDevicePathProtocol != NULL &&
        EfiDevicePathSize (BbsTableDevicePathProtocol) == DevicePathProtocolSize &&
        EfiCompareMem (BbsTableDevicePathProtocol, DevicePathProtocol, DevicePathProtocolSize) == 0) {
      *BbsIndex = Index;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

BOOLEAN
CheckLegacyBootVar (
  IN UINT16                 BbsIndex,
  IN UINT16                 *BootDesc,
  IN UINT16                 DeviceType
  )
/*++

Routine Description:
  According BBS table index, device string and device type to check this
  legacy device is whether saved in Bootxxxx variable.

Arguments:

  BbsIndex - The index of Bbs Table.
  BootDesc - The device description string
  DeviceType - The type of this device

Returns:
  TRUE - This legacy device has been saved to Bootxxxx variable.
  FALSE- This legacy device hasn't been saved to Bootxxxx variable.

--*/
{
  UINTN                     BootOrderSize;
  UINT16                    *BootOrder;
  UINTN                     Index;
  UINT16                    BootOption[10];
  UINT8                     *BootOptionVar;
  UINTN                     BootOptionSize;
  BBS_TABLE                 *BbsEntry;
  UINT16                    TempBbsIndex;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );

  if (BootOrder == NULL) {
    return FALSE;
  }

  for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      continue;
    }
    if (!BdsLibIsLegacyBootOption (BootOptionVar, &BbsEntry, &TempBbsIndex)) {
      continue;
    }
    if ((TempBbsIndex == BbsIndex) && (BbsEntry->DeviceType == DeviceType) &&
        (EfiStrCmp (BootDesc, (UINT16*)(BootOptionVar + sizeof (UINT32) + sizeof (UINT16))) == 0)) {
      return TRUE;
    }
  }

  return FALSE;
}

EFI_STATUS
UpdateBootVar (
  IN BBS_TABLE              *BbsItem,
  IN CHAR16                 *BootDesc,
  IN CHAR16                 *BootString,
  IN UINTN                  TempIndex
  )
{

  BBS_BBS_DEVICE_PATH       BbsDevPathNode;
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  UINT16                    CurrentBbsDevPathSize;
  UINTN                     BufferSize;
  VOID                      *Buffer;
  UINT8                     *Ptr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathProtocol;
  UINTN                     DevicePathProtocolSize;

  DevPath                       = NULL;

  gBS->SetMem (&BbsDevPathNode, sizeof (BBS_BBS_DEVICE_PATH), 0);
  BbsDevPathNode.Header.Type    = BBS_DEVICE_PATH;
  BbsDevPathNode.Header.SubType = BBS_BBS_DP;
  SetDevicePathNodeLength (&BbsDevPathNode.Header, sizeof (BBS_BBS_DEVICE_PATH));
  BbsDevPathNode.DeviceType = BbsItem->DeviceType;
  EfiCopyMem (&BbsDevPathNode.StatusFlag, &BbsItem->StatusFlags, sizeof (UINT16));

  DevicePathProtocolSize = 0;
  DevicePathProtocol = (EFI_DEVICE_PATH_PROTOCOL *) (UINTN) BbsItem->IBV2;
  if (DevicePathProtocol != NULL) {
    DevicePathProtocolSize = EfiDevicePathSize (DevicePathProtocol);
  }

  DevPath = EfiAppendDevicePathNode (
              EndDevicePath,
              (EFI_DEVICE_PATH_PROTOCOL *) &BbsDevPathNode
              );
  if (NULL == DevPath) {
    return EFI_OUT_OF_RESOURCES;
  }

  CurrentBbsDevPathSize = (UINT16) (EfiDevicePathSize (DevPath));

  BufferSize = sizeof (UINT32) +
               sizeof (UINT16) +
               EfiStrSize (BootDesc) +
               CurrentBbsDevPathSize +
               sizeof (BBS_TABLE) +
               sizeof (UINT16) +
               DevicePathProtocolSize;

  Buffer = EfiAllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    SafeFreePool (DevPath);
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr               = (UINT8 *) Buffer;

  *((UINT32 *) Ptr) = LOAD_OPTION_ACTIVE;
  Ptr += sizeof (UINT32);

  *((UINT16 *) Ptr) = CurrentBbsDevPathSize;
  Ptr += sizeof (UINT16);

  EfiCopyMem (
    Ptr,
    BootDesc,
    EfiStrSize (BootDesc)
    );
  Ptr += EfiStrSize (BootDesc);

  EfiCopyMem (
    Ptr,
    DevPath,
    CurrentBbsDevPathSize
    );
  Ptr += CurrentBbsDevPathSize;

  EfiCopyMem (
    Ptr,
    BbsItem,
    sizeof (BBS_TABLE)
    );

  Ptr += sizeof (BBS_TABLE);
  *((UINT16 *) Ptr) = (UINT16) TempIndex;

  if (DevicePathProtocolSize != 0) {
    Ptr += sizeof (UINT16);
    EfiCopyMem (
      Ptr,
      DevicePathProtocol,
      DevicePathProtocolSize
      );
  }

  Status = gRT->SetVariable (
                  BootString,
                  &gEfiGlobalVariableGuid,
                  VAR_FLAG,
                  BufferSize,
                  Buffer
                  );

  SafeFreePool (DevPath);
  SafeFreePool (Buffer);

  return Status;

}

BOOLEAN
UpdateBootVarIfChangeSataMode (
  IN UINTN                                BbsIndexStart,
  IN CHAR16                               *BootOptionStr,
  IN UINT16                               BbsIndex,
  IN EFI_DEVICE_PATH_PROTOCOL             *DevicePathProtocol
  )
/*++

Routine Description:

  Update boot option variable if user change sata mode.

Arguments:

  BbsIndexStart            - Start index of bbs table
  BootOptionStr            - Pointer to boot option string
  BbsIndex                 - BBS table index
  DevicePathProtocol       - Pointer to device path protocol

Returns:

  TRUE  - Sync boot variable successfully.
  FALSE - No need to sync boot variable.

--*/
{
  EFI_STATUS                              Status;
  BOOLEAN                                 OverwriteBootVar;
  UINT8                                   *BootOptionVar;
  UINTN                                   BootOptionSize;
  UINTN                                   Index;
  UINT16                                  BootDesc[100];
  UINT16                                  HddCount;
  HDD_INFO                                *LocalHddInfo;
  BBS_TABLE                               *BbsTable;
  UINT16                                  BbsCount;
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL       *ChipsetLibServices;
  EFI_LEGACY_BIOS_PROTOCOL                *LegacyBios;
  EFI_DEVICE_PATH_PROTOCOL                *BbsTableDevicePathProtocol;

  OverwriteBootVar = FALSE;

  if (BbsIndex == 0) {
    return OverwriteBootVar;
  }

  Status = gBS->LocateProtocol (
                  &gEfiChipsetLibServicesProtocolGuid,
                  NULL,
                  &ChipsetLibServices
                  );
  if (EFI_ERROR (Status)) {
    return OverwriteBootVar;
  }

  BbsCount     = 0;
  BbsTable     = NULL;
  LocalHddInfo = NULL;
  Status = EfiLibLocateProtocol (&gEfiLegacyBiosProtocolGuid, &LegacyBios);
  if (!EFI_ERROR (Status)) {
    Status = LegacyBios->GetBbsInfo (
                           LegacyBios,
                           &HddCount,
                           &LocalHddInfo,
                           &BbsCount,
                           &BbsTable
                           );
  }
  if (EFI_ERROR (Status)) {
    return OverwriteBootVar;
  }

  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOptionStr,
                    &gEfiGlobalVariableGuid,
                    &BootOptionSize
                    );
  if (BootOptionVar == NULL) {
    return OverwriteBootVar;
  }
  
  for (Index = BbsIndexStart; Index < BbsCount; Index++) {
    //
    // skip unused BBS table to make sure doesn't overwrite the boot option with null description string
    //
    if (((BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) ||
         (BbsTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM) ||
         (BbsTable[Index].BootPriority == BBS_LOWEST_PRIORITY))) {
       continue;
    }
    BdsBuildLegacyDevNameString (
      &BbsTable[Index],
      LocalHddInfo,
      Index,
      sizeof(BootDesc),
      BootDesc
      );


    //
    // Because some option will add the "space" character to the end of string to make sure
    // the string length is 0x40. Using this function to synchronize this kind of adjustment
    //
    if ((ChipsetLibServices->DifferentSizeStrCmp (BootDesc, (UINT16*)(BootOptionVar + sizeof (UINT32) + sizeof (UINT16))) == 0)) {
      //
      // If in the same SATA mode by checking device path, do not update boot variable.
      //
      BbsTableDevicePathProtocol = (EFI_DEVICE_PATH_PROTOCOL *) (UINTN) BbsTable[Index].IBV2;
      if (BbsTableDevicePathProtocol != NULL && 
          DevicePathProtocol         != NULL &&
          ((BdsLibCheckDevicePath (BbsTableDevicePathProtocol, MESSAGING_DEVICE_PATH, MSG_ATAPI_DP) &&
            BdsLibCheckDevicePath (DevicePathProtocol        , MESSAGING_DEVICE_PATH, MSG_ATAPI_DP)) ||
           (BdsLibCheckDevicePath (BbsTableDevicePathProtocol, MESSAGING_DEVICE_PATH, MSG_SATA_DP) &&
            BdsLibCheckDevicePath (DevicePathProtocol        , MESSAGING_DEVICE_PATH, MSG_SATA_DP)))) {
        continue;
      }
      //
      // According BBS table index, device string and device type to check this
      // legacy device is whether saved in Bootxxxx variable. If this legacy device
      // is saved in any Bootxxxx variable, skip this over write to prevent the different
      // Bootxxxx variables have the same component.
      //
      if (!CheckLegacyBootVar ((UINT16) Index, BootDesc, BbsTable[Index].DeviceType)) {
        UpdateBootVar (&BbsTable[Index], BootDesc, BootOptionStr, Index);
        OverwriteBootVar = TRUE;
        break;
      }
    }
  }

  gBS->FreePool (BootOptionVar);
  
  return OverwriteBootVar;  
}

EFI_STATUS
BdsDeleteAllInvalidLegacyBootOptions (
  VOID
  )
/*++

  Routine Description:

    Delete all the invalid legacy boot options.

  Arguments:

    None.

  Returns:

    EFI_SUCCESS            - All invalide legacy boot options are deleted.
    EFI_OUT_OF_RESOURCES   - Fail to allocate necessary memory.
    EFI_NOT_FOUND          - Fail to retrive variable of boot order.
    Other                  - Error occurs while setting variable or locating
                             protocol.

--*/
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINT16                    HddCount;
  UINT16                    BbsCount;
  HDD_INFO                  *LocalHddInfo;
  BBS_TABLE                 *LocalBbsTable;
  BBS_TABLE                 *BbsEntry;
  UINT16                    BbsIndex;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  UINTN                     Index;
  UINT16                    BootOption[10];
  UINT16                    BootDesc[100];
  BOOLEAN                   DescStringMatch;
  BOOLEAN                   BbsEntryMatch;
  BOOLEAN                   OverwriteBootVar;
  UINTN                     TempStart = 0;
  EFI_SETUP_UTILITY_PROTOCOL *SetupUtility;
  SYSTEM_CONFIGURATION      *SystemConfiguration = NULL;
  UINTN                     Index2;
  UINTN                     Size;
  OPROM_STORAGE_DEVICE_INFO *OpromStorageDev;
  UINTN                     OpromStorageDevCount;
  UINT8                     *DisableOpromStorageDevBoot;
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL       
                            *ChipsetLibServices;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathProtocol;
  UINT16                    CurrentBbsIndex;
  BOOLEAN                   DoUpdateIfChangeSataMode;
  BOOLEAN                   BbsIndexMatch;
  
  Status        = EFI_SUCCESS;
  BootOrder     = NULL;
  BootOrderSize = 0;
  HddCount      = 0;
  BbsCount      = 0;
  LocalHddInfo  = NULL;
  LocalBbsTable = NULL;
  BbsEntry      = NULL;
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid,
                                NULL,
                                &SetupUtility
                               );
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (SYSTEM_CONFIGURATION *)SetupUtility->SetupNvData;
  }
  Status = gBS->LocateProtocol (
                  &gEfiChipsetLibServicesProtocolGuid,
                  NULL,
                  &ChipsetLibServices
                  );
  ASSERT_EFI_ERROR (Status);                        

  Status = EfiLibLocateProtocol (&gEfiLegacyBiosProtocolGuid, &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LegacyBios->GetBbsInfo (
                LegacyBios,
                &HddCount,
                &LocalHddInfo,
                &BbsCount,
                &LocalBbsTable
                );

  //
  // Patch the AHCI CDROM DeviceType in Ahci BBS(produced by AHCIOprom)
  //
#ifndef SEAMLESS_AHCI_SUPPORT    
  ChipsetLibServices->UpdateBBSDeviceType (SystemConfiguration, LocalBbsTable, &TempStart);
#endif
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    return EFI_NOT_FOUND;
  }

  //
  // If DisableOpromStorageDevBoot variable exist, get Oprom info then delete Oprom storage device.
  //
  OpromStorageDev = NULL;
  OpromStorageDevCount = 0;
  DisableOpromStorageDevBoot = BdsLibGetVariableAndSize (
                                 L"DisableOpromStorageDevBoot",
                                 &gEfiGenericVariableGuid,
                                 &Size
                                 );
  if (DisableOpromStorageDevBoot != NULL) {
    BdsLibGetOpromStorageDevInfo (&OpromStorageDev, &OpromStorageDevCount);
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
      //
      //If the Boot Device is not exit, we should dynamically adjust the BootOrder
      //
      BdsLibUpdateInvalidBootOrder (&BootOrder, Index, &BootOrderSize);
      continue;
    }

    if (!BdsLibIsLegacyBootOption (BootOptionVar, &BbsEntry, &BbsIndex)) {
      SafeFreePool (BootOptionVar);
      Index++;
      continue;
    }

    if (BbsEntry->Class == PCI_CLASS_NETWORK &&
        BbsEntry->SubClass == PCI_CLASS_NETWORK_ETHERNET &&
        SystemConfiguration != NULL && 
        SystemConfiguration->PxeBootToLan == 0) {
      SafeFreePool (BootOptionVar);
      BdsDeleteBootOption (
        BootOrder[Index],
        BootOrder,
        &BootOrderSize
        );
      continue;
    }
    
    if (DisableOpromStorageDevBoot != NULL &&
        BbsEntry->Class == PCI_CLASS_MASS_STORAGE) {
      for (Index2 = 0; Index2 < OpromStorageDevCount; Index2++) {
        if (BbsEntry->Bus      == (UINT32) OpromStorageDev[Index2].Bus &&
            BbsEntry->Device   == (UINT32) OpromStorageDev[Index2].Device &&
            BbsEntry->Function == (UINT32) OpromStorageDev[Index2].Function) {
          break;
        }
      }
      
      if (Index2 < OpromStorageDevCount) {
        SafeFreePool (BootOptionVar);
        BdsDeleteBootOption (
          BootOrder[Index],
          BootOrder,
          &BootOrderSize
          );
        continue;
      }
    }

    //
    // Check if BBS Description String is changed
    //
    DescStringMatch = FALSE;
    BbsEntryMatch = FALSE;
    BbsIndexMatch = TRUE;

    DoUpdateIfChangeSataMode = TRUE;
    DevicePathProtocol = GetDevicePathProtocolFromVar (BootOptionVar, BootOptionSize);
    if (DevicePathProtocol != NULL) {     
      //
      // Get correct bbs Index by comparing device path to prevent two devices have the same boot description strings.
      //
      Status = GetBbsIndexByDevicePathProtocol (LocalBbsTable, BbsCount, DevicePathProtocol, &CurrentBbsIndex);
      if (!EFI_ERROR(Status)) {
        DoUpdateIfChangeSataMode = FALSE;
        
        if (CurrentBbsIndex != BbsIndex) {
          BbsIndexMatch = FALSE;
          BbsIndex = CurrentBbsIndex;
        }
      } else {
        BbsIndexMatch = FALSE;
        if (!BdsLibCheckDevicePath (DevicePathProtocol, MESSAGING_DEVICE_PATH, MSG_ATAPI_DP) &&
            !BdsLibCheckDevicePath (DevicePathProtocol, MESSAGING_DEVICE_PATH, MSG_SATA_DP)) {
          SafeFreePool (BootOptionVar);
          BdsDeleteBootOption (
            BootOrder[Index],
            BootOrder,
            &BootOrderSize
            );
          continue;
        }
      }
    }
    
    BdsBuildLegacyDevNameString (
      &LocalBbsTable[BbsIndex],
      LocalHddInfo,
      BbsIndex,
      sizeof(BootDesc),
      BootDesc
      );

    if (EfiStrCmp (BootDesc, (UINT16*)(BootOptionVar + sizeof (UINT32) + sizeof (UINT16))) == 0) {
      DescStringMatch = TRUE;
    }
    //
    // Skip check IBV2 of bba table because it is a memory address which may be change every boot.
    //
    if (EfiCompareMem (&LocalBbsTable[BbsIndex], BbsEntry, sizeof (BBS_TABLE) - sizeof(UINT32)) == 0) {
      BbsEntryMatch = TRUE;
    }

    if (!((LocalBbsTable[BbsIndex].BootPriority == BBS_IGNORE_ENTRY) ||
          (LocalBbsTable[BbsIndex].BootPriority == BBS_DO_NOT_BOOT_FROM) ||
          (LocalBbsTable[BbsIndex].BootPriority == BBS_LOWEST_PRIORITY)) &&
        (LocalBbsTable[BbsIndex].DeviceType == BbsEntry->DeviceType) &&
        DescStringMatch && 
        BbsEntryMatch &&
        BbsIndexMatch) {
      Index++;
      continue;
    }

    if (ChipsetLibServices->NeedToOverWriteVar () && DoUpdateIfChangeSataMode) {
      OverwriteBootVar = UpdateBootVarIfChangeSataMode (
                           TempStart,
                           BootOption,
                           BbsIndex,
                           DevicePathProtocol
                           );
      if (OverwriteBootVar) {
        SafeFreePool (BootOptionVar);
        Index++;
        continue;
      }

      if (DevicePathProtocol != NULL) {
        SafeFreePool (BootOptionVar);
        BdsDeleteBootOption (
          BootOrder[Index],
          BootOrder,
          &BootOrderSize
          );
        continue;
      }
    }

    //
    // If only the content of the BBS_TABLE is different, just need update current Bootxxxx variable
    //
    if (!((LocalBbsTable[BbsIndex].BootPriority == BBS_IGNORE_ENTRY) ||
          (LocalBbsTable[BbsIndex].BootPriority == BBS_DO_NOT_BOOT_FROM) ||
          (LocalBbsTable[BbsIndex].BootPriority == BBS_LOWEST_PRIORITY)) &&
        (LocalBbsTable[BbsIndex].DeviceType == BbsEntry->DeviceType) &&
        DescStringMatch && 
        (!BbsEntryMatch || !BbsIndexMatch)) {
      BdsBuildLegacyDevNameString (
        &LocalBbsTable[BbsIndex],
        LocalHddInfo,
        BbsIndex,
        sizeof(BootDesc),
        BootDesc
        );
      UpdateBootVar (&LocalBbsTable[BbsIndex], BootDesc, BootOption, BbsIndex);
      SafeFreePool (BootOptionVar);
      Index++;
      continue;
    }

    SafeFreePool (BootOptionVar);
    //
    // should delete
    //
    BdsDeleteBootOption (
      BootOrder[Index],
      BootOrder,
      &BootOrderSize
      );
  }

  if (BootOrderSize) {
    Status = gRT->SetVariable (
                    L"BootOrder",
                    &gEfiGlobalVariableGuid,
                    VAR_FLAG,
                    BootOrderSize,
                    BootOrder
                    );
  } else {
    EfiLibDeleteVariable (L"BootOrder", &gEfiGlobalVariableGuid);
  }

  SafeFreePool (BootOrder);
  SafeFreePool (DisableOpromStorageDevBoot);
  SafeFreePool (OpromStorageDev);

  return Status;
}

BOOLEAN
BdsFindLegacyBootOptionByDevType (
  IN UINT16                 *BootOrder,
  IN UINTN                  BootOptionNum,
  IN UINT16                 DevIndex,
  OUT UINT32                *Attribute,
  OUT UINT16                *BbsIndex,
  OUT UINTN                 *OptionNumber
  )
{
  UINTN     Index;
  UINTN     BootOrderIndex;
  UINT16    BootOption[100];
  UINTN     BootOptionSize;
  UINT8     *BootOptionVar;
  BBS_TABLE *BbsEntry;
  BOOLEAN   Found;

  BbsEntry  = NULL;
  Found     = FALSE;

  if (NULL == BootOrder) {
    return Found;
  }

  for (BootOrderIndex = 0; BootOrderIndex < BootOptionNum; BootOrderIndex++) {
    Index = (UINTN) BootOrder[BootOrderIndex];
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", Index);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );

    if (NULL == BootOptionVar) {
      continue;
    }

    if (!BdsLibIsLegacyBootOption (BootOptionVar, &BbsEntry, BbsIndex)) {
      SafeFreePool (BootOptionVar);
      continue;
    }

    if (DevIndex != *BbsIndex) {
      SafeFreePool (BootOptionVar);
      continue;
    }

    *Attribute    = *(UINT32 *) BootOptionVar;
    *OptionNumber = Index;
    Found         = TRUE;
    SafeFreePool (BootOptionVar);
    break;
  }

  return Found;
}

EFI_STATUS
BdsCreateOneLegacyBootOption (
  IN BBS_TABLE              *BbsItem,
  IN HDD_INFO               *HddInfo,
  IN UINTN                  Index,
  IN OUT UINT16             **BootOrderList,
  IN OUT UINTN              *BootOrderListSize
  )
{
  BBS_BBS_DEVICE_PATH       BbsDevPathNode;
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;

  DevPath                       = NULL;

  gBS->SetMem (&BbsDevPathNode, sizeof (BBS_BBS_DEVICE_PATH), 0);
  BbsDevPathNode.Header.Type    = BBS_DEVICE_PATH;
  BbsDevPathNode.Header.SubType = BBS_BBS_DP;
  SetDevicePathNodeLength (&BbsDevPathNode.Header, sizeof (BBS_BBS_DEVICE_PATH));
  BbsDevPathNode.DeviceType = BbsItem->DeviceType;
  EfiCopyMem (&BbsDevPathNode.StatusFlag, &BbsItem->StatusFlags, sizeof (UINT16));

  DevPath = EfiAppendDevicePathNode (
              EndDevicePath,
              (EFI_DEVICE_PATH_PROTOCOL *) &BbsDevPathNode
              );
  if (NULL == DevPath) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = BdsCreateLegacyBootOption (
            BbsItem,
            HddInfo,
            DevPath,
            Index,
            BootOrderList,
            BootOrderListSize
            );
    BbsItem->BootPriority = (UINT16)(*BootOrderListSize / sizeof(UINT16));

  gBS->FreePool (DevPath);

  return Status;
}

EFI_STATUS
BdsAddNonExistingLegacyBootOptions (
  VOID
  )
/*++

Routine Description:

  Add the legacy boot options from BBS table if they do not exist.

Arguments:

  None.

Returns:

  EFI_SUCCESS       - The boot options are added successfully or they are already in boot options.
  others            - An error occurred when creating legacy boot options.

--*/
{
  UINT16                    *BootOrder;
  UINTN                     BootOrderSize;
  EFI_STATUS                Status;
  UINT16                    HddCount;
  UINT16                    BbsCount;
  HDD_INFO                  *LocalHddInfo;
  BBS_TABLE                 *LocalBbsTable;
  UINT16                    BbsIndex;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  UINTN                     Index;
  UINT32                    Attribute;
  UINTN                     OptionNumber;
  BOOLEAN                   Ret;
#ifndef SEAMLESS_AHCI_SUPPORT  
  EFI_CHIPSET_LIB_SERVICES_PROTOCOL       
                            *ChipsetLibServices;
#endif  

  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;
  SYSTEM_CONFIGURATION                  *SystemConfiguration = NULL;
  UINTN                                 StartCount = 0;
  UINTN                                 Index2;
  UINTN                                 Size;
  OPROM_STORAGE_DEVICE_INFO             *OpromStorageDev;
  UINTN                                 OpromStorageDevCount;
  UINT8                                 *DisableOpromStorageDevBoot;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid,
                                NULL,
                                &SetupUtility
                               );
  if (!EFI_ERROR (Status)) {
    SystemConfiguration = (SYSTEM_CONFIGURATION *)SetupUtility->SetupNvData;
  }
  BootOrder     = NULL;
  HddCount      = 0;
  BbsCount      = 0;
  LocalHddInfo  = NULL;
  LocalBbsTable = NULL;

  Status        = EfiLibLocateProtocol (&gEfiLegacyBiosProtocolGuid, &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LegacyBios->GetBbsInfo (
                LegacyBios,
                &HddCount,
                &LocalHddInfo,
                &BbsCount,
                &LocalBbsTable
                );

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (NULL == BootOrder) {
    BootOrderSize = 0;
  }
#ifndef SEAMLESS_AHCI_SUPPORT  
  Status = gBS->LocateProtocol (
                        &gEfiChipsetLibServicesProtocolGuid,
                        NULL,
                        &ChipsetLibServices
                        );
  ASSERT_EFI_ERROR (Status);                        

  ChipsetLibServices->UpdateBBSDeviceType (SystemConfiguration, LocalBbsTable, &StartCount);
#endif

  OpromStorageDev = NULL;
  OpromStorageDevCount = 0;
  DisableOpromStorageDevBoot = BdsLibGetVariableAndSize (
                                 L"DisableOpromStorageDevBoot",
                                 &gEfiGenericVariableGuid,
                                 &Size
                                 );
  if (DisableOpromStorageDevBoot != NULL) {
    BdsLibGetOpromStorageDevInfo (&OpromStorageDev, &OpromStorageDevCount);
  }
  
  for (Index = 0; Index < BbsCount; Index++) {
    if ((LocalBbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) ||
        (LocalBbsTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM)
        ) {
      continue;
    }

    Ret = BdsFindLegacyBootOptionByDevType (
            BootOrder,
            BootOrderSize / sizeof (UINT16),
            (UINT16) Index,
            &Attribute,
            &BbsIndex,
            &OptionNumber
            );
    if ((Index < StartCount) && (Index !=0)) {
      if ((BbsIndex < StartCount) && (Index !=0)) {
        BdsDeleteBootOption (
              OptionNumber,
              BootOrder,
              &BootOrderSize
              );
      }
      continue;
    }
    if (Ret && (Attribute & LOAD_OPTION_ACTIVE) != 0) {
      continue;
    }

    if (Ret) {
      if (Index != BbsIndex) {
        BdsDeleteBootOption (
          OptionNumber,
          BootOrder,
          &BootOrderSize
          );
      } else {
        continue;
      }
    }
    if (LocalBbsTable[Index].Class == PCI_CLASS_NETWORK &&
        LocalBbsTable[Index].SubClass == PCI_CLASS_NETWORK_ETHERNET &&
        SystemConfiguration != NULL && 
        SystemConfiguration->PxeBootToLan == 0) {
      continue;
    }

    if (DisableOpromStorageDevBoot != NULL &&
        LocalBbsTable[Index].Class == PCI_CLASS_MASS_STORAGE) {
      for (Index2 = 0; Index2 < OpromStorageDevCount; Index2++) {
        if (LocalBbsTable[Index].Bus      == (UINT32) OpromStorageDev[Index2].Bus &&
            LocalBbsTable[Index].Device   == (UINT32) OpromStorageDev[Index2].Device &&
            LocalBbsTable[Index].Function == (UINT32) OpromStorageDev[Index2].Function) {
          break;
        }
      }
      
      if (Index2 < OpromStorageDevCount) {
        continue;
      }
    }
    //
    // Not found such type of legacy device in boot options or we found but it's disabled
    // so we have to create one and put it to the tail of boot order list
    //
    Status = BdsCreateOneLegacyBootOption (
              &LocalBbsTable[Index],
              LocalHddInfo,
              Index,
              &BootOrder,
              &BootOrderSize
              );
    if (EFI_ERROR (Status)) {
      break;
    }
  }

  if (BootOrderSize > 0) {
    Status = gRT->SetVariable (
                    L"BootOrder",
                    &gEfiGlobalVariableGuid,
                    VAR_FLAG,
                    BootOrderSize,
                    BootOrder
                    );
  } else {
    EfiLibDeleteVariable (L"BootOrder", &gEfiGlobalVariableGuid);
  }

  if (BootOrder != NULL) {
    SafeFreePool (BootOrder);
  }
  SafeFreePool (DisableOpromStorageDevBoot);
  SafeFreePool (OpromStorageDev);

  return Status;
}

UINT16 *
BdsFillDevOrderBuf (
  IN BBS_TABLE                    *BbsTable,
  IN BBS_TYPE                     BbsType,
  IN UINTN                        BbsCount,
  IN UINT16                       *Buf
  )
{
  UINTN Index;

  for (Index = 0; Index < BbsCount; Index++) {
    if (BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
      continue;
    }

    if (BbsTable[Index].DeviceType != BbsType) {
      continue;
    }

    *Buf = (UINT16) (Index & 0xFF);
    Buf++;
  }

  return Buf;
}

EFI_STATUS
BdsCreateDevOrder (
  IN BBS_TABLE                  *BbsTable,
  IN UINT16                     BbsCount
  )
{
  UINTN       Index;
  UINTN       FDCount;
  UINTN       HDCount;
  UINTN       CDCount;
  UINTN       NETCount;
  UINTN       BEVCount;
  UINTN       TotalSize;
  UINTN       HeaderSize;
  UINT8       *DevOrder;
  UINT8       *Ptr;
  EFI_STATUS  Status;

  FDCount     = 0;
  HDCount     = 0;
  CDCount     = 0;
  NETCount    = 0;
  BEVCount    = 0;
  TotalSize   = 0;
  HeaderSize  = sizeof (BBS_TYPE) + sizeof (UINT16);
  DevOrder    = NULL;
  Ptr         = NULL;
  Status      = EFI_SUCCESS;

  for (Index = 0; Index < BbsCount; Index++) {
    if (BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
      continue;
    }

    switch (BbsTable[Index].DeviceType) {
    case BBS_FLOPPY:
      FDCount++;
      break;

    case BBS_HARDDISK:
      HDCount++;
      break;

    case BBS_CDROM:
      CDCount++;
      break;

    case BBS_EMBED_NETWORK:
      NETCount++;
      break;

    case BBS_BEV_DEVICE:
      BEVCount++;
      break;

    default:
      break;
    }
  }

  TotalSize += (HeaderSize + sizeof (UINT16) * FDCount);
  TotalSize += (HeaderSize + sizeof (UINT16) * HDCount);
  TotalSize += (HeaderSize + sizeof (UINT16) * CDCount);
  TotalSize += (HeaderSize + sizeof (UINT16) * NETCount);
  TotalSize += (HeaderSize + sizeof (UINT16) * BEVCount);

  DevOrder = EfiAllocateZeroPool (TotalSize);
  if (NULL == DevOrder) {
    return EFI_OUT_OF_RESOURCES;
  }

  Ptr                 = DevOrder;

  *((BBS_TYPE *) Ptr) = BBS_FLOPPY;
  Ptr += sizeof (BBS_TYPE);
  *((UINT16 *) Ptr) = (UINT16) (sizeof (UINT16) + FDCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);
  if (FDCount) {
    Ptr = (UINT8 *) BdsFillDevOrderBuf (BbsTable, BBS_FLOPPY, BbsCount, (UINT16 *) Ptr);
  }

  *((BBS_TYPE *) Ptr) = BBS_HARDDISK;
  Ptr += sizeof (BBS_TYPE);
  *((UINT16 *) Ptr) = (UINT16) (sizeof (UINT16) + HDCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);
  if (HDCount) {
    Ptr = (UINT8 *) BdsFillDevOrderBuf (BbsTable, BBS_HARDDISK, BbsCount, (UINT16 *) Ptr);
  }

  *((BBS_TYPE *) Ptr) = BBS_CDROM;
  Ptr += sizeof (BBS_TYPE);
  *((UINT16 *) Ptr) = (UINT16) (sizeof (UINT16) + CDCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);
  if (CDCount) {
    Ptr = (UINT8 *) BdsFillDevOrderBuf (BbsTable, BBS_CDROM, BbsCount, (UINT16 *) Ptr);
  }

  *((BBS_TYPE *) Ptr) = BBS_EMBED_NETWORK;
  Ptr += sizeof (BBS_TYPE);
  *((UINT16 *) Ptr) = (UINT16) (sizeof (UINT16) + NETCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);
  if (NETCount) {
    Ptr = (UINT8 *) BdsFillDevOrderBuf (BbsTable, BBS_EMBED_NETWORK, BbsCount, (UINT16 *) Ptr);
  }

  *((BBS_TYPE *) Ptr) = BBS_BEV_DEVICE;
  Ptr += sizeof (BBS_TYPE);
  *((UINT16 *) Ptr) = (UINT16) (sizeof (UINT16) + BEVCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);
  if (BEVCount) {
    Ptr = (UINT8 *) BdsFillDevOrderBuf (BbsTable, BBS_BEV_DEVICE, BbsCount, (UINT16 *) Ptr);
  }

  Status = gRT->SetVariable (
                  VarLegacyDevOrder,
                  &EfiLegacyDevOrderGuid,
                  VAR_FLAG,
                  TotalSize,
                  DevOrder
                  );
  SafeFreePool (DevOrder);

  return Status;
}

EFI_STATUS
BdsUpdateLegacyDevOrder (
  VOID
  )
/*++
Format of LegacyDevOrder variable:
|-----------------------------------------------------------------------------------------------------------------
| BBS_FLOPPY | Length | Index0 | Index1 | ... | BBS_HARDDISK | Length | Index0 | Index1 | ... | BBS_CDROM | Length | Index0 | ...
|-----------------------------------------------------------------------------------------------------------------

Length is a 16 bit integer, it indicates how many Indexes follows, including the size of itself.
Index# is a 16 bit integer, the low byte of it stands for the index in BBS table
           the high byte of it only have two value 0 and 0xFF, 0xFF means this device has been
           disabled by user.
--*/
{
  UINT8                     *DevOrder;
  UINT8                     *NewDevOrder;
  UINTN                     DevOrderSize;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  EFI_STATUS                Status;
  UINT16                    HddCount;
  UINT16                    BbsCount;
  HDD_INFO                  *LocalHddInfo;
  BBS_TABLE                 *LocalBbsTable;
  UINTN                     Index;
  UINTN                     Index2;
  UINTN                     *Idx;
  UINTN                     FDCount;
  UINTN                     HDCount;
  UINTN                     CDCount;
  UINTN                     NETCount;
  UINTN                     BEVCount;
  UINTN                     TotalSize;
  UINTN                     HeaderSize;
  UINT8                     *Ptr;
  UINT8                     *NewPtr;
  UINT16                    *NewFDPtr;
  UINT16                    *NewHDPtr;
  UINT16                    *NewCDPtr;
  UINT16                    *NewNETPtr;
  UINT16                    *NewBEVPtr;
  UINT16                    *NewDevPtr;
  UINT16                    Length;
  UINT16                    tmp;
  UINTN                     FDIndex;
  UINTN                     HDIndex;
  UINTN                     CDIndex;
  UINTN                     NETIndex;
  UINTN                     BEVIndex;

  LocalHddInfo  = NULL;
  LocalBbsTable = NULL;
  Idx           = NULL;
  FDCount       = 0;
  HDCount       = 0;
  CDCount       = 0;
  NETCount      = 0;
  BEVCount      = 0;
  TotalSize     = 0;
  HeaderSize    = sizeof (BBS_TYPE) + sizeof (UINT16);
  FDIndex       = 0;
  HDIndex       = 0;
  CDIndex       = 0;
  NETIndex      = 0;
  BEVIndex      = 0;
  NewDevPtr     = NULL;

  Status        = EfiLibLocateProtocol (&gEfiLegacyBiosProtocolGuid, &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LegacyBios->GetBbsInfo (
                LegacyBios,
                &HddCount,
                &LocalHddInfo,
                &BbsCount,
                &LocalBbsTable
                );

  DevOrder = (UINT8 *) BdsLibGetVariableAndSize (
                        VarLegacyDevOrder,
                        &EfiLegacyDevOrderGuid,
                        &DevOrderSize
                        );
  if (NULL == DevOrder) {
    return BdsCreateDevOrder (LocalBbsTable, BbsCount);
  }
  //
  // First we figure out how many boot devices with same device type respectively
  //
  for (Index = 0; Index < BbsCount; Index++) {
    if ((LocalBbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) ||
        (LocalBbsTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM) ||
        (LocalBbsTable[Index].BootPriority == BBS_LOWEST_PRIORITY)
        ) {
      continue;
    }

    switch (LocalBbsTable[Index].DeviceType) {
    case BBS_FLOPPY:
      FDCount++;
      break;

    case BBS_HARDDISK:
      HDCount++;
      break;

    case BBS_CDROM:
      CDCount++;
      break;

    case BBS_EMBED_NETWORK:
      NETCount++;
      break;

    case BBS_BEV_DEVICE:
      BEVCount++;
      break;

    default:
      break;
    }
  }

  TotalSize += (HeaderSize + FDCount * sizeof (UINT16));
  TotalSize += (HeaderSize + HDCount * sizeof (UINT16));
  TotalSize += (HeaderSize + CDCount * sizeof (UINT16));
  TotalSize += (HeaderSize + NETCount * sizeof (UINT16));
  TotalSize += (HeaderSize + BEVCount * sizeof (UINT16));

  NewDevOrder = EfiAllocateZeroPool (TotalSize);
  if (NULL == NewDevOrder) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewFDPtr  = (UINT16 *) (NewDevOrder + HeaderSize);
  NewHDPtr  = (UINT16 *) ((UINT8 *) NewFDPtr + FDCount * sizeof (UINT16) + HeaderSize);
  NewCDPtr  = (UINT16 *) ((UINT8 *) NewHDPtr + HDCount * sizeof (UINT16) + HeaderSize);
  NewNETPtr = (UINT16 *) ((UINT8 *) NewCDPtr + CDCount * sizeof (UINT16) + HeaderSize);
  NewBEVPtr = (UINT16 *) ((UINT8 *) NewNETPtr + NETCount * sizeof (UINT16) + HeaderSize);

  //
  // copy FD
  //
  Ptr                     = DevOrder;
  NewPtr                  = NewDevOrder;
  *((BBS_TYPE *) NewPtr)  = *((BBS_TYPE *) Ptr);
  Ptr += sizeof (BBS_TYPE);
  NewPtr += sizeof (BBS_TYPE);
  Length                = *((UINT16 *) Ptr);
  *((UINT16 *) NewPtr)  = (UINT16) (sizeof (UINT16) + FDCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);

  for (Index = 0; Index < Length / sizeof (UINT16) - 1; Index++) {
    if (LocalBbsTable[*Ptr].BootPriority == BBS_IGNORE_ENTRY ||
        LocalBbsTable[*Ptr].BootPriority == BBS_DO_NOT_BOOT_FROM ||
        LocalBbsTable[*Ptr].BootPriority == BBS_LOWEST_PRIORITY ||
        LocalBbsTable[*Ptr].DeviceType != BBS_FLOPPY
        ) {
      Ptr += sizeof (UINT16);
      continue;
    }

    NewFDPtr[FDIndex] = *(UINT16 *) Ptr;
    FDIndex++;
    Ptr += sizeof (UINT16);
  }
  //
  // copy HD
  //
  NewPtr                  = (UINT8 *) NewHDPtr - HeaderSize;
  *((BBS_TYPE *) NewPtr)  = *((BBS_TYPE *) Ptr);
  Ptr += sizeof (BBS_TYPE);
  NewPtr += sizeof (BBS_TYPE);
  Length                = *((UINT16 *) Ptr);
  *((UINT16 *) NewPtr)  = (UINT16) (sizeof (UINT16) + HDCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);

  for (Index = 0; Index < Length / sizeof (UINT16) - 1; Index++) {
    if (LocalBbsTable[*Ptr].BootPriority == BBS_IGNORE_ENTRY ||
        LocalBbsTable[*Ptr].BootPriority == BBS_DO_NOT_BOOT_FROM ||
        LocalBbsTable[*Ptr].BootPriority == BBS_LOWEST_PRIORITY ||
        LocalBbsTable[*Ptr].DeviceType != BBS_HARDDISK
        ) {
      Ptr += sizeof (UINT16);
      continue;
    }

    NewHDPtr[HDIndex] = *(UINT16 *) Ptr;
    HDIndex++;
    Ptr += sizeof (UINT16);
  }
  //
  // copy CD
  //
  NewPtr                  = (UINT8 *) NewCDPtr - HeaderSize;
  *((BBS_TYPE *) NewPtr)  = *((BBS_TYPE *) Ptr);
  Ptr += sizeof (BBS_TYPE);
  NewPtr += sizeof (BBS_TYPE);
  Length                = *((UINT16 *) Ptr);
  *((UINT16 *) NewPtr)  = (UINT16) (sizeof (UINT16) + CDCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);

  for (Index = 0; Index < Length / sizeof (UINT16) - 1; Index++) {
    if (LocalBbsTable[*Ptr].BootPriority == BBS_IGNORE_ENTRY ||
        LocalBbsTable[*Ptr].BootPriority == BBS_DO_NOT_BOOT_FROM ||
        LocalBbsTable[*Ptr].BootPriority == BBS_LOWEST_PRIORITY ||
        LocalBbsTable[*Ptr].DeviceType != BBS_CDROM
        ) {
      Ptr += sizeof (UINT16);
      continue;
    }

    NewCDPtr[CDIndex] = *(UINT16 *) Ptr;
    CDIndex++;
    Ptr += sizeof (UINT16);
  }
  //
  // copy NET
  //
  NewPtr                  = (UINT8 *) NewNETPtr - HeaderSize;
  *((BBS_TYPE *) NewPtr)  = *((BBS_TYPE *) Ptr);
  Ptr += sizeof (BBS_TYPE);
  NewPtr += sizeof (BBS_TYPE);
  Length                = *((UINT16 *) Ptr);
  *((UINT16 *) NewPtr)  = (UINT16) (sizeof (UINT16) + NETCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);

  for (Index = 0; Index < Length / sizeof (UINT16) - 1; Index++) {
    if (LocalBbsTable[*Ptr].BootPriority == BBS_IGNORE_ENTRY ||
        LocalBbsTable[*Ptr].BootPriority == BBS_DO_NOT_BOOT_FROM ||
        LocalBbsTable[*Ptr].BootPriority == BBS_LOWEST_PRIORITY ||
        LocalBbsTable[*Ptr].DeviceType != BBS_EMBED_NETWORK
        ) {
      Ptr += sizeof (UINT16);
      continue;
    }

    NewNETPtr[NETIndex] = *(UINT16 *) Ptr;
    NETIndex++;
    Ptr += sizeof (UINT16);
  }
  //
  // copy BEV
  //
  NewPtr                  = (UINT8 *) NewBEVPtr - HeaderSize;
  *((BBS_TYPE *) NewPtr)  = *((BBS_TYPE *) Ptr);
  Ptr += sizeof (BBS_TYPE);
  NewPtr += sizeof (BBS_TYPE);
  Length                = *((UINT16 *) Ptr);
  *((UINT16 *) NewPtr)  = (UINT16) (sizeof (UINT16) + BEVCount * sizeof (UINT16));
  Ptr += sizeof (UINT16);

  for (Index = 0; Index < Length / sizeof (UINT16) - 1; Index++) {
    if (LocalBbsTable[*Ptr].BootPriority == BBS_IGNORE_ENTRY ||
        LocalBbsTable[*Ptr].BootPriority == BBS_DO_NOT_BOOT_FROM ||
        LocalBbsTable[*Ptr].BootPriority == BBS_LOWEST_PRIORITY ||
        LocalBbsTable[*Ptr].DeviceType != BBS_BEV_DEVICE
        ) {
      Ptr += sizeof (UINT16);
      continue;
    }

    NewBEVPtr[BEVIndex] = *(UINT16 *) Ptr;
    BEVIndex++;
    Ptr += sizeof (UINT16);
  }

  for (Index = 0; Index < BbsCount; Index++) {
    if ((LocalBbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) ||
        (LocalBbsTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM) ||
        (LocalBbsTable[Index].BootPriority == BBS_LOWEST_PRIORITY)
        ) {
      continue;
    }

    switch (LocalBbsTable[Index].DeviceType) {
    case BBS_FLOPPY:
      Idx       = &FDIndex;
      NewDevPtr = NewFDPtr;
      break;

    case BBS_HARDDISK:
      Idx       = &HDIndex;
      NewDevPtr = NewHDPtr;
      break;

    case BBS_CDROM:
      Idx       = &CDIndex;
      NewDevPtr = NewCDPtr;
      break;

    case BBS_EMBED_NETWORK:
      Idx       = &NETIndex;
      NewDevPtr = NewNETPtr;
      break;

    case BBS_BEV_DEVICE:
      Idx       = &BEVIndex;
      NewDevPtr = NewBEVPtr;
      break;

    default:
      Idx = NULL;
      break;
    }
    //
    // at this point we have copied those valid indexes to new buffer
    // and we should check if there is any new appeared boot device
    //
    if (Idx) {
      for (Index2 = 0; Index2 < *Idx; Index2++) {
        if ((NewDevPtr[Index2] & 0xFF) == (UINT16) Index) {
          break;
        }
      }

      if (Index2 == *Idx) {
        //
        // Index2 == *Idx means we didn't find Index
        // so Index is a new appeared device's index in BBS table
        // save it.
        //
        NewDevPtr[*Idx] = (UINT16) (Index & 0xFF);
        (*Idx)++;
      }
    }
  }

  if (FDCount) {
    //
    // Just to make sure that disabled indexes are all at the end of the array
    //
    for (Index = 0; Index < FDIndex - 1; Index++) {
      if (0xFF00 != (NewFDPtr[Index] & 0xFF00)) {
        continue;
      }

      for (Index2 = Index + 1; Index2 < FDIndex; Index2++) {
        if (0 == (NewFDPtr[Index2] & 0xFF00)) {
          tmp               = NewFDPtr[Index];
          NewFDPtr[Index]   = NewFDPtr[Index2];
          NewFDPtr[Index2]  = tmp;
          break;
        }
      }
    }
  }

  if (HDCount) {
    //
    // Just to make sure that disabled indexes are all at the end of the array
    //
    for (Index = 0; Index < HDIndex - 1; Index++) {
      if (0xFF00 != (NewHDPtr[Index] & 0xFF00)) {
        continue;
      }

      for (Index2 = Index + 1; Index2 < HDIndex; Index2++) {
        if (0 == (NewHDPtr[Index2] & 0xFF00)) {
          tmp               = NewHDPtr[Index];
          NewHDPtr[Index]   = NewHDPtr[Index2];
          NewHDPtr[Index2]  = tmp;
          break;
        }
      }
    }
  }

  if (CDCount) {
    //
    // Just to make sure that disabled indexes are all at the end of the array
    //
    for (Index = 0; Index < CDIndex - 1; Index++) {
      if (0xFF00 != (NewCDPtr[Index] & 0xFF00)) {
        continue;
      }

      for (Index2 = Index + 1; Index2 < CDIndex; Index2++) {
        if (0 == (NewCDPtr[Index2] & 0xFF00)) {
          tmp               = NewCDPtr[Index];
          NewCDPtr[Index]   = NewCDPtr[Index2];
          NewCDPtr[Index2]  = tmp;
          break;
        }
      }
    }
  }

  if (NETCount) {
    //
    // Just to make sure that disabled indexes are all at the end of the array
    //
    for (Index = 0; Index < NETIndex - 1; Index++) {
      if (0xFF00 != (NewNETPtr[Index] & 0xFF00)) {
        continue;
      }

      for (Index2 = Index + 1; Index2 < NETIndex; Index2++) {
        if (0 == (NewNETPtr[Index2] & 0xFF00)) {
          tmp               = NewNETPtr[Index];
          NewNETPtr[Index]  = NewNETPtr[Index2];
          NewNETPtr[Index2] = tmp;
          break;
        }
      }
    }
  }

  if (BEVCount) {
    //
    // Just to make sure that disabled indexes are all at the end of the array
    //
    for (Index = 0; Index < BEVIndex - 1; Index++) {
      if (0xFF00 != (NewBEVPtr[Index] & 0xFF00)) {
        continue;
      }

      for (Index2 = Index + 1; Index2 < BEVIndex; Index2++) {
        if (0 == (NewBEVPtr[Index2] & 0xFF00)) {
          tmp               = NewBEVPtr[Index];
          NewBEVPtr[Index]  = NewBEVPtr[Index2];
          NewBEVPtr[Index2] = tmp;
          break;
        }
      }
    }
  }

  SafeFreePool (DevOrder);

  Status = gRT->SetVariable (
                  VarLegacyDevOrder,
                  &EfiLegacyDevOrderGuid,
                  VAR_FLAG,
                  TotalSize,
                  NewDevOrder
                  );
  SafeFreePool (NewDevOrder);

  return Status;
}

EFI_STATUS
BdsSetBootPriority4SameTypeDev (
  IN UINT16                                              DeviceType,
  IN OUT BBS_TABLE                                       *LocalBbsTable,
  IN OUT UINT16                                          *Priority
  )
/*++
DeviceType           - BBS_FLOPPY, BBS_HARDDISK, BBS_CDROM and so on
LocalBbsTable       - BBS table instance
Priority                 - As input arg, it is the start point of boot priority, as output arg, it is the start point of boot
                              priority can be used next time.
--*/
{
  UINT8   *DevOrder;

  UINT8   *OrigBuffer;
  UINT16  *DevIndex;
  UINTN   DevOrderSize;
  UINTN   DevCount;
  UINTN   Index;

  DevOrder = BdsLibGetVariableAndSize (
              VarLegacyDevOrder,
              &EfiLegacyDevOrderGuid,
              &DevOrderSize
              );
  if (NULL == DevOrder) {
    return EFI_OUT_OF_RESOURCES;
  }

  OrigBuffer = DevOrder;
  while (DevOrder < OrigBuffer + DevOrderSize) {
    if (DeviceType == * (BBS_TYPE *) DevOrder) {
      break;
    }

    DevOrder += sizeof (BBS_TYPE);
    DevOrder += *(UINT16 *) DevOrder;
  }

  if (DevOrder >= OrigBuffer + DevOrderSize) {
    SafeFreePool (OrigBuffer);
    return EFI_NOT_FOUND;
  }

  DevOrder += sizeof (BBS_TYPE);
  DevCount  = (*((UINT16 *) DevOrder) - sizeof (UINT16)) / sizeof (UINT16);
  DevIndex  = (UINT16 *) (DevOrder + sizeof (UINT16));
  //
  // If the high byte of the DevIndex is 0xFF, it indicates that this device has been disabled.
  //
  for (Index = 0; Index < DevCount; Index++) {
    if ((DevIndex[Index] & 0xFF00) == 0xFF00) {
      //
      // LocalBbsTable[DevIndex[Index] & 0xFF].BootPriority = BBS_DISABLED_ENTRY;
      //
    } else {
      LocalBbsTable[DevIndex[Index] & 0xFF].BootPriority = *Priority;
      (*Priority)++;
    }
  }

  SafeFreePool (OrigBuffer);
  return EFI_SUCCESS;
}

VOID
PrintBbsTable (
  IN BBS_TABLE                      *LocalBbsTable
  )
{
  UINT16  Idx;

  DEBUG ((EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_ERROR, " NO  Prio bb/dd/ff cl/sc Type Stat segm:offs\n"));
  DEBUG ((EFI_D_ERROR, "=============================================\n"));
  for (Idx = 0; Idx < MAX_BBS_ENTRIES; Idx++) {
    if ((LocalBbsTable[Idx].BootPriority == BBS_IGNORE_ENTRY) ||
        (LocalBbsTable[Idx].BootPriority == BBS_DO_NOT_BOOT_FROM) ||
        (LocalBbsTable[Idx].BootPriority == BBS_LOWEST_PRIORITY)
        ) {
      continue;
    }

    DEBUG (
      (EFI_D_ERROR,
      " %02x: %04x %02x/%02x/%02x %02x/02%x %04x %04x %04x:%04x\n",
      (UINTN) Idx,
      (UINTN) LocalBbsTable[Idx].BootPriority,
      (UINTN) LocalBbsTable[Idx].Bus,
      (UINTN) LocalBbsTable[Idx].Device,
      (UINTN) LocalBbsTable[Idx].Function,
      (UINTN) LocalBbsTable[Idx].Class,
      (UINTN) LocalBbsTable[Idx].SubClass,
      (UINTN) LocalBbsTable[Idx].DeviceType,
      (UINTN) * (UINT16 *) &LocalBbsTable[Idx].StatusFlags,
      (UINTN) LocalBbsTable[Idx].BootHandlerSegment,
      (UINTN) LocalBbsTable[Idx].BootHandlerOffset,
      (UINTN) ((LocalBbsTable[Idx].MfgStringSegment << 4) + LocalBbsTable[Idx].MfgStringOffset),
      (UINTN) ((LocalBbsTable[Idx].DescStringSegment << 4) + LocalBbsTable[Idx].DescStringOffset))
      );
  }

  DEBUG ((EFI_D_ERROR, "\n"));
}

EFI_STATUS
BdsRefreshBbsTableForBoot (
  IN BDS_COMMON_OPTION        *Entry
  )
{
  EFI_STATUS                Status;
  UINT16                    HddCount;
  UINT16                    BbsCount;
  HDD_INFO                  *LocalHddInfo;
  BBS_TABLE                 *LocalBbsTable;
  UINT16                    DevType;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  UINTN                     Index;
  UINT16                    Priority;
  UINT16                    *BootOrder;
  UINTN                     BootOrderSize;
  UINT8                     *BootOptionVar;
  UINTN                     BootOptionSize;
  UINT16                    BootOption[100];
  UINT8                     *Ptr;
  UINT16                    DevPathLen;
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;

  HddCount      = 0;
  BbsCount      = 0;
  LocalHddInfo  = NULL;
  LocalBbsTable = NULL;
  DevType       = BBS_UNKNOWN;

  Status        = EfiLibLocateProtocol (&gEfiLegacyBiosProtocolGuid, &LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LegacyBios->GetBbsInfo (
                LegacyBios,
                &HddCount,
                &LocalHddInfo,
                &BbsCount,
                &LocalBbsTable
                );
  //
  // First, set all the present devices' boot priority to BBS_UNPRIORITIZED_ENTRY
  // We will set them according to the settings setup by user
  //
  for (Index = 0; Index < BbsCount; Index++) {
    if (!((BBS_IGNORE_ENTRY == LocalBbsTable[Index].BootPriority) ||
        (BBS_DO_NOT_BOOT_FROM == LocalBbsTable[Index].BootPriority) ||
         (BBS_LOWEST_PRIORITY == LocalBbsTable[Index].BootPriority))) {
      LocalBbsTable[Index].BootPriority = BBS_UNPRIORITIZED_ENTRY;
    }
  }
  //
  // boot priority always starts at 0
  //
  Priority = 0;
  if (Entry->LoadOptionsSize == sizeof (BBS_TABLE) + sizeof (UINT16)) {
    //
    // If Entry stands for a legacy boot option, we prioritize the devices with the same type first.
    //
    DevType = ((BBS_TABLE *) Entry->LoadOptions)->DeviceType;
    Status = BdsSetBootPriority4SameTypeDev (
              DevType,
              LocalBbsTable,
              &Priority
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  //
  // we have to set the boot priority for other BBS entries with different device types
  //
  BootOrder = (UINT16 *) BdsLibGetVariableAndSize (
                          L"BootOrder",
                          &gEfiGlobalVariableGuid,
                          &BootOrderSize
                          );
  for (Index = 0; BootOrder && Index < BootOrderSize / sizeof (UINT16); Index++) {
    SPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      continue;
    }

    Ptr = BootOptionVar;

    Ptr += sizeof (UINT32);
    DevPathLen = *(UINT16 *) Ptr;
    Ptr += sizeof (UINT16);
    Ptr += EfiStrSize ((UINT16 *) Ptr);
    DevPath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
    if (BBS_DEVICE_PATH != DevPath->Type || BBS_BBS_DP != DevPath->SubType) {
      SafeFreePool (BootOptionVar);
      continue;
    }

    Ptr += DevPathLen;
    if (DevType == ((BBS_TABLE *) Ptr)->DeviceType) {
      //
      // We don't want to process twice for a device type
      //
      SafeFreePool (BootOptionVar);
      continue;
    }

    Status = BdsSetBootPriority4SameTypeDev (
              ((BBS_TABLE *) Ptr)->DeviceType,
              LocalBbsTable,
              &Priority
              );
    SafeFreePool (BootOptionVar);
    if (EFI_ERROR (Status)) {
      break;
    }
  }

  if (BootOrder) {
    SafeFreePool (BootOrder);
  }
  //
  // For debug
  //
  PrintBbsTable (LocalBbsTable);

  return Status;
}
