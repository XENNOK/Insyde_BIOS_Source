/** @file
  Entry and initial functions for BootOptionPolicy driver

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

#include "BootOptionPolicy.h"

STATIC
BOOLEAN
IsUsbDevice (
  IN BBS_TABLE              *CurrentBbsTable
  );

STATIC
EFI_STATUS
ConvertDeviceType (
  IN OUT  UINT8                    *DevType,
  IN      BOOT_OPTION_POLICY_DATA  *BootOptionPolicyData
  );

STATIC
EFI_STATUS
GetNormalTypeBootPosition (
  IN      BOOT_OPTION_POLICY_DATA       *BootOptionPolicyData,
  IN      UINT16                        *BootOrderList,
  IN      UINTN                         RegisterOptionNumber,
  IN      UINTN                         BootOptionNum,
  IN OUT  UINTN                         *NewPosition
  );

STATIC
BOOLEAN
IsEfiShellBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  );

/**
  Initialize BootOptionPolicy driver.

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval EFI_SUCCESS    The BootOptionPolicy module is initialized correctly..
  @return Other value if failed to initialize the BootOptionPolicy module.

**/
EFI_STATUS
EFIAPI
InitBootOptionPolicy (
  IN EFI_HANDLE                             ImageHandle,
  IN EFI_SYSTEM_TABLE                       *SystemTable
  )
{
  EFI_STATUS                                Status;
  BOOT_OPTION_POLICY_DATA                   *BootOptionPolicyData;
  KERNEL_CONFIGURATION                      KernelConfiguration;

  Status = GetKernelConfiguration (&KernelConfiguration);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // implement worker function
  //
  BootOptionPolicyData = AllocateZeroPool (sizeof(BOOT_OPTION_POLICY_DATA));
  if (BootOptionPolicyData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BootOptionPolicyData->Signature                                       = EFI_BOOT_OPTION_POLICY_SIGNATURE;
  BootOptionPolicyData->NewPositionPolicy                               = KernelConfiguration.NewPositionPolicy;
  BootOptionPolicyData->BootNormalPriority                              = KernelConfiguration.BootNormalPriority;
  BootOptionPolicyData->LegacyNormalMenuType                            = KernelConfiguration.LegacyNormalMenuType;
  CopyMem (BootOptionPolicyData->BootTypeOrder, KernelConfiguration.BootTypeOrder, MAX_BOOT_ORDER_NUMBER);
  BootOptionPolicyData->BootOptionPolicy.FindPositionOfNewBootOption    = FindPositionOfNewBootOption;
  BootOptionPolicyData->BootOptionPolicy.GetEfiOptionPriority           = GetEfiOptionPriority;
  //
  // Install Setup Utility
  //
  BootOptionPolicyData->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &BootOptionPolicyData->Handle,
                  &gEfiBootOptionPolicyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &BootOptionPolicyData->BootOptionPolicy
                  );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (BootOptionPolicyData);
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FindPositionOfNewBootOption (
  IN     EFI_BOOT_OPTION_POLICY_PROTOCOL    * This,
  IN     EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  IN     UINT16                             *BootOrder,
  IN     UINTN                              BootOptionType,
  IN     UINTN                              BootOptionNum,
  IN     UINTN                              RegisterOptionNumber,
  IN OUT UINTN                              *NewPosition
  )
{
  BOOT_OPTION_POLICY_DATA                   *BootOptionPolicyData;
  UINTN                                     Index;
  UINTN                                     BootOption;
  CHAR16                                    BootOptionString[20];
  UINTN                                     BootOptionSize;
  VOID                                      *BootOptionVar;
  UINTN                                     NewPositionPolicy;
  BOOLEAN                                   FindFlag;
  UINTN                                     BootOptionTypePolicy = 1;
  UINTN                                     CurrentBootOptionType;
  EFI_DEVICE_PATH_PROTOCOL                  *OptionDevicePath;
  UINT8                                     *TempPtr;
  UINTN                                     LegacyNormalMenuType;
  EFI_STATUS                                Status;
  EFI_BOOT_ORDER_PRIORITY                   WorkBootPriority;
  EFI_BOOT_ORDER_PRIORITY                   CurrentBootPriority;
  UINTN                                     LastEfiBootIndex;
  BOOLEAN                                   CreateByOS;
  UINTN                                     Size;

  BootOptionPolicyData = EFI_BOOT_OPTION_POLICY_FROM_THIS (This);

  NewPositionPolicy    = BootOptionPolicyData->NewPositionPolicy;
  BootOptionTypePolicy = BootOptionPolicyData->BootNormalPriority;
  LegacyNormalMenuType = BootOptionPolicyData->LegacyNormalMenuType;
  //
  // find the first Legacy BootOPtion in BootOrder
  //
  BootOption = 0;
  FindFlag   = FALSE;
  CurrentBootPriority = OtherEfiBootOption;
  LastEfiBootIndex    = 0xFFFFFFFF;
  if (BootOptionType == EFI_BOOT_DEV && NewPositionPolicy == IN_AUTO) {
    Status = GetEfiOptionPriority (
               This,
               FALSE,
               DevicePath,
               &CurrentBootPriority
               );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  for (Index = 0; Index < BootOptionNum; Index++) {
    BootOption = BootOrder[Index];
    UnicodeSPrint (
      BootOptionString,
      sizeof (BootOptionString),
      L"Boot%04x",
      BootOption
      );

    BootOptionSize = 0;
    BootOptionVar = NULL;
    Status = CommonGetVariableDataAndSize (
               BootOptionString,
               &gEfiGlobalVariableGuid,
               &BootOptionSize,
               (VOID**) &BootOptionVar
               );
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }

    Size = 0;
    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32);
    Size += sizeof (UINT32) + (UINT16) (*TempPtr);
    TempPtr += sizeof (UINT16);
    Size += sizeof (UINT16) + (UINT16) StrSize ((UINT16 *) TempPtr);
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    TempPtr += GetDevicePathSize (OptionDevicePath);
    if ((BBS_DEVICE_PATH == OptionDevicePath->Type) && (BBS_BBS_DP == OptionDevicePath->SubType)) {
      CurrentBootOptionType = LEGACY_BOOT_DEV;
    } else {
      CurrentBootOptionType = EFI_BOOT_DEV;
    }

    if (CurrentBootOptionType == BootOptionType) {
      //
      // if the Legacy Normal menu type is set and BootOptionType is legacy boot device
      // ,get the boot order new position according device type
      //
      if ((LegacyNormalMenuType == 0) && (BootOptionType == LEGACY_BOOT_DEV)) {
         Status = GetNormalTypeBootPosition (
                    BootOptionPolicyData,
                    BootOrder,
                    RegisterOptionNumber,
                    BootOptionNum,
                    NewPosition
                    );
        if (!EFI_ERROR (Status)) {
          Index = BootOptionNum;
          FindFlag = TRUE;
        }
      } else {
        switch (NewPositionPolicy) {

        case IN_FIRST :
          if (BootOptionType == BootOptionTypePolicy) {
            *NewPosition = 0;
          } else {
            *NewPosition = Index;
          }
          Index = BootOptionNum;
          FindFlag = TRUE;
          break;

        case IN_LAST :
          *NewPosition = Index + 1;
          FindFlag = TRUE;
          break;

        case IN_AUTO:
          if (BootOptionType == LEGACY_BOOT_DEV) {
            //
            // Legacy devices boot order is same as IN_FIRST
            //
            if (BootOptionType == BootOptionTypePolicy) {
              *NewPosition = 0;
            } else {
              *NewPosition = Index;
            }
            Index = BootOptionNum;
            FindFlag = TRUE;
          } else {
            CreateByOS = TRUE;
            if (((BootOptionSize - Size == 2) && (AsciiStrnCmp ((CHAR8 *)TempPtr, "RC", 2) == 0)) ||
                (IsEfiShellBootOption (DevicePath) && (BootOptionSize - Size >= 2) && (AsciiStrnCmp ((CHAR8 *)TempPtr, "RC", 2) == 0))) {
              CreateByOS = FALSE;
            }
            Status = GetEfiOptionPriority (
                       This,
                       CreateByOS,
                       OptionDevicePath,
                       &WorkBootPriority
                       );
            ASSERT_EFI_ERROR (Status);
            LastEfiBootIndex = Index;
            if (CurrentBootPriority <= WorkBootPriority) {
              *NewPosition = Index;
              Index = BootOptionNum;
              FindFlag = TRUE;
            }
          }

        break;

        default :
          break;
        }
      }
    }
    gBS->FreePool (BootOptionVar);
  }

  if (!FindFlag) {
    if (BootOptionType == BootOptionTypePolicy) {
      *NewPosition = (LastEfiBootIndex == 0xFFFFFFFF) ? 0 : LastEfiBootIndex + 1;
    } else {
      *NewPosition = BootOptionNum;
    }
  }

  return EFI_SUCCESS;
}


/**
  Check the specific BBS Table entry is USB device

  @param CurrentBbsTable         Pointer to current BBS table start address

  @retval TRUE                   It is USB device
  @retval FALSE                  It isn't USB device

**/
STATIC
BOOLEAN
IsUsbDevice (
  IN BBS_TABLE              *CurrentBbsTable
  )
{
  if ((CurrentBbsTable->Class == PCI_CLASS_SERIAL) &&
      (CurrentBbsTable->SubClass == PCI_CLASS_SERIAL_USB)) {
    return TRUE;
  }
  return FALSE;

}

/**
  convert the device the device type according supported device type.
  if the device type isn't in supported Boot type order, change this
  type to BOOT_POLICY_OTHER_DEVICE

  @param  DevType                IN : a pointer point to original device type
                                 OUT: a pointer point to converted device type
  @param  BootOptionPolicyData   a pointer point to BOOT_OPTION_POLICY_DATA

  @retval EFI_SUCCESS            Convert device type successfully.
  @retval EFI_INVALID_PARAMETER  Invalid input parameter.

**/
STATIC
EFI_STATUS
ConvertDeviceType (
  IN OUT  UINT8                    *DevType,
  IN      BOOT_OPTION_POLICY_DATA  *BootOptionPolicyData
  )
{
  UINT8         DevTypeOrder[MAX_BOOT_ORDER_NUMBER];
  UINTN         DeviceTypeCount;
  UINTN         Index;
  BOOLEAN       SupportedType;

  if ((DevType == NULL) || (BootOptionPolicyData == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  //calculate the supported device type count
  //
  for (DeviceTypeCount = 0; DeviceTypeCount < MAX_BOOT_ORDER_NUMBER; DeviceTypeCount++) {
    if (BootOptionPolicyData->BootTypeOrder[DeviceTypeCount] == 0) {
      break;
    }
  }

  ZeroMem (DevTypeOrder, MAX_BOOT_ORDER_NUMBER);
  CopyMem (DevTypeOrder, BootOptionPolicyData->BootTypeOrder, DeviceTypeCount);
  SupportedType = FALSE;

  for (Index = 0; Index <  DeviceTypeCount; Index++) {
    if (*DevType == DevTypeOrder[Index]) {
      SupportedType = TRUE;
    }
  }

  if (!SupportedType) {
    *DevType = BOOT_POLICY_OTHER_DEVICE;
  }
  return EFI_SUCCESS;
}

/**
  Get the new postion for new register option number in boot order according
  boot type and new position policy (IN_FIRST or IN_LAST)

  @param  BootOptionPolicyData   a pointer point to BOOT_OPTION_POLICY_DATA
  @param  BootOrderList          a pointer point to boot order list
  @param  RegisterOptionNumber   the new boot option number which want to insert to boot order
  @param  BootOptionNum          the total boot option count
  @param  NewPosition            IN : a valid pointer which doesn't point to NULL
                                 OUT: a pointer point to the position which want to insert to boot order

  @retval EFI_SUCCESS            get new boot position is success
  @retval EFI_INVALID_PARAMETER  input parameter is invalid
  @retval EFI_UNSUPPORTED        1. the new register boot option is not legacy device
                                 2. the NewPositionPolicy is invalid

**/
STATIC
EFI_STATUS
GetNormalTypeBootPosition (
  IN      BOOT_OPTION_POLICY_DATA       *BootOptionPolicyData,
  IN      UINT16                        *BootOrderList,
  IN      UINTN                         RegisterOptionNumber,
  IN      UINTN                         BootOptionNum,
  IN OUT  UINTN                         *NewPosition
  )
{
  EFI_STATUS                       Status;
  UINT16                           BootOption[100];
  UINT8                            *BootOptionVar;
  UINT8                            *Ptr;
  UINT16                           DevPathLen;
  UINT8                            DevType;
  UINTN                            Index;
  UINTN                            Index2;
  UINT8                            DevTypeOrder[MAX_BOOT_ORDER_NUMBER];
  UINTN                            DeviceTypeCount;
  UINTN                            BehindCurrentDevTypeCount;
  UINTN                            LastLegacyBootOptionNum;
  EFI_DEVICE_PATH_PROTOCOL         *DevPath;


  if ((BootOptionPolicyData == NULL) ||
      (BootOptionPolicyData->LegacyNormalMenuType != 0)) {
    return EFI_INVALID_PARAMETER;
  }
  if ((BootOrderList == NULL) || (NewPosition == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  BehindCurrentDevTypeCount = 0;
  LastLegacyBootOptionNum = 0;
  //
  //Calculate the supported device type count
  //
  for (DeviceTypeCount = 0; DeviceTypeCount < MAX_BOOT_ORDER_NUMBER; DeviceTypeCount++) {
    if (BootOptionPolicyData->BootTypeOrder[DeviceTypeCount] == 0) {
      break;
    }
  }

  ZeroMem (DevTypeOrder, MAX_BOOT_ORDER_NUMBER);
  //
  //Get the device type from this new boot option
  //
  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", RegisterOptionNumber);
  BootOptionVar = CommonGetVariableData (
                    BootOption,
                    &gEfiGlobalVariableGuid
                    );
  if (BootOptionVar == NULL) {
    return EFI_NOT_FOUND;
  }

  Ptr = BootOptionVar;
  Ptr += sizeof (UINT32);
  DevPathLen = *(UINT16 *) Ptr;
  Ptr += sizeof (UINT16);
  Ptr += StrSize ((UINT16 *) Ptr);
  DevPath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  //
  //Check is legacy boot device?
  //
  if (BBS_DEVICE_PATH != DevPath->Type || BBS_BBS_DP != DevPath->SubType) {
    gBS->FreePool (BootOptionVar);
    return EFI_UNSUPPORTED;
  }
  Ptr += DevPathLen;
  if (IsUsbDevice ((BBS_TABLE *) Ptr)) {
    DevType = BBS_USB;
  } else {
    DevType = (UINT8) (((BBS_TABLE *) Ptr)->DeviceType);
  }
  Status = ConvertDeviceType (&DevType, BootOptionPolicyData);

  //
  //Get all the devices type which behind the device type of new register boot option
  //if the boot policy is IN_FIRST, the Saved devices type includes the boot type
  //of new register boot option. if the boot policy is IN_LAST, the Saved devices type
  //doesn't include the boot type
  //
  for (Index = 0; Index < DeviceTypeCount; Index++) {
    if (DevType == BootOptionPolicyData->BootTypeOrder[Index]) {
      switch (BootOptionPolicyData->NewPositionPolicy) {

      case IN_FIRST:
      case IN_AUTO:
        BehindCurrentDevTypeCount = DeviceTypeCount - Index;
        CopyMem (
               DevTypeOrder,
               &(BootOptionPolicyData->BootTypeOrder[Index]),
               DeviceTypeCount - Index
               );
        break;

      case IN_LAST:
        if ((DeviceTypeCount - (Index + 1)) != 0) {
          BehindCurrentDevTypeCount = DeviceTypeCount - (Index + 1);
          CopyMem (
                 DevTypeOrder,
                 &(BootOptionPolicyData->BootTypeOrder[Index + 1]),
                 DeviceTypeCount - (Index + 1)
                 );
        }
        break;

      default:
        gBS->FreePool (BootOptionVar);
        return EFI_UNSUPPORTED;;
      }
      break;
    }
  }
  gBS->FreePool (BootOptionVar);

  //
  //Find the first boot option which match the any saved devices type
  //
  for (Index = 0; Index < BootOptionNum; Index++) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrderList[Index]);
    BootOptionVar = CommonGetVariableData (
                      BootOption,
                      &gEfiGlobalVariableGuid
                      );
    Ptr = BootOptionVar;
    Ptr += sizeof (UINT32);
    DevPathLen = *(UINT16 *) Ptr;
    Ptr += sizeof (UINT16);
    Ptr += StrSize ((UINT16 *) Ptr);
    DevPath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
    //
    //Check is legacy boot device? skip check EFI device
    //
    if (BBS_DEVICE_PATH != DevPath->Type || BBS_BBS_DP != DevPath->SubType) {
      if (BootOptionVar != NULL) {
        gBS->FreePool (BootOptionVar);
        BootOptionVar = NULL;
      }
      continue;
    }
    LastLegacyBootOptionNum = Index + 1;
    Ptr += DevPathLen;
  if (IsUsbDevice ((BBS_TABLE *) Ptr)) {
    DevType = BBS_USB;
  } else {
    DevType = (UINT8) (((BBS_TABLE *) Ptr)->DeviceType);
  }
    Status = ConvertDeviceType (&DevType, BootOptionPolicyData);
    //
    //search the match devices type
    //
    for (Index2 = 0; Index2 < BehindCurrentDevTypeCount; Index2++) {
      if (DevType == DevTypeOrder[Index2]) {
        *NewPosition = Index;
        if (BootOptionVar != NULL) {
          gBS->FreePool (BootOptionVar);
          BootOptionVar = NULL;
        }
        return EFI_SUCCESS;
      }
    }
    if (BootOptionVar != NULL) {
      gBS->FreePool (BootOptionVar);
      BootOptionVar = NULL;
    }
  }
  //
  //if not find any boot option to match saved boot devices type, insert the
  //boot option in the last of the list of legacy boot device
  //
  *NewPosition = LastLegacyBootOptionNum;
  return EFI_SUCCESS;
}

/**
  Function unpacks a device path data structure so that all the nodes
  of a device path are naturally aligned.

  @param  DevPath                A pointer to a device path data structure

  @return Pointer to dynamically allocated memory that contains the new device path
          or NULL if allocate memory fail.

**/
STATIC
EFI_DEVICE_PATH_PROTOCOL *
UnpackDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Src;
  EFI_DEVICE_PATH_PROTOCOL  *Dest;
  EFI_DEVICE_PATH_PROTOCOL  *NewPath;
  UINTN                     Size;

  //
  // Walk device path and round sizes to valid boundries
  //
  Src   = DevPath;
  Size  = 0;
  for (;;) {
    Size += DevicePathNodeLength (Src);
    Size += ALIGN_SIZE (Size);

    if (IsDevicePathEnd (Src)) {
      break;
    }

    Src = NextDevicePathNode (Src);
  }
  //
  // Allocate space for the unpacked path
  //
  NewPath = AllocateZeroPool (Size);
  if (NewPath) {

    ASSERT (((UINTN) NewPath) % MIN_ALIGNMENT_SIZE == 0);

    //
    // Copy each node
    //
    Src   = DevPath;
    Dest  = NewPath;
    for (;;) {
      Size = DevicePathNodeLength (Src);
      CopyMem (Dest, Src, Size);
      Size += ALIGN_SIZE (Size);
      SetDevicePathNodeLength (Dest, Size);
      Dest->Type |= EFI_DP_TYPE_UNPACKED;
      Dest = (EFI_DEVICE_PATH_PROTOCOL *) (((UINT8 *) Dest) + Size);

      if (IsDevicePathEnd (Src)) {
        break;
      }

      Src = NextDevicePathNode (Src);
    }
  }

  return NewPath;
}

/**
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

  @param  BlockIoDevicePath      Multi device path instances which need to check
  @param  HardDriveDevicePath    A device path which starts with a hard drive media device path.

  @retval TRUE                   There is a matched device path instance
  @retval FALSE                  There is no matched device path instance

**/
STATIC
BOOLEAN
MatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
{
  HARDDRIVE_DEVICE_PATH     *TmpHdPath;
  HARDDRIVE_DEVICE_PATH     *TempPath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   Match;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoHdDevicePathNode;

  if ((BlockIoDevicePath == NULL) || (HardDriveDevicePath == NULL)) {
    return FALSE;
  }
  //
  // Make PreviousDevicePath == the device path node before the end node
  //
  DevicePath          = BlockIoDevicePath;
  BlockIoHdDevicePathNode = NULL;

  //
  // find the partition device path node
  //
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP)
        ) {
      BlockIoHdDevicePathNode = DevicePath;
      break;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (BlockIoHdDevicePathNode == NULL) {
    return FALSE;
  }
  //
  // See if the harddrive device path in blockio matches the orig Hard Drive Node
  //
  TmpHdPath = (HARDDRIVE_DEVICE_PATH *) BlockIoHdDevicePathNode;
  TempPath  = (HARDDRIVE_DEVICE_PATH *) UnpackDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
  Match = FALSE;
  //
  // Check for the match
  //
  if ((TempPath != NULL) &&
      (TmpHdPath->MBRType       == TempPath->MBRType) &&
      (TmpHdPath->SignatureType == TempPath->SignatureType)) {
    switch (TmpHdPath->SignatureType) {

    case SIGNATURE_TYPE_GUID:
      Match = CompareGuid ((EFI_GUID *)TmpHdPath->Signature, (EFI_GUID *)TempPath->Signature);
      break;

    case SIGNATURE_TYPE_MBR:
      Match = (BOOLEAN)(*((UINT32 *)(&(TmpHdPath->Signature[0]))) == *(UINT32 *)(&(TempPath->Signature[0])));
      break;

    default:
      Match = FALSE;
      break;
    }
  }

  if (TempPath != NULL) {
    gBS->FreePool (TempPath);
  }

  return Match;
}

/**
  If input is a hard drive device path, append it to corresponding BlockIo device path.
  If input is not a hard drive device path, output NULL.

  @param  HardDriveDevicePath    Input device path

  @return Pointer to device path which combines BlockIo and hard drive device path
          or NULL if input is not hard disk device.

**/
STATIC
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
  Check if the boot option is a EFI shell boot option which is put in ROM image.

  @param  DevicePath             The device path need to be processed.

  @retval TRUE                   It is a EFI shell boot option in ROM image.
  @retval FALSE                  It is not a EFI shell boot option in ROM image.

**/
STATIC
BOOLEAN
IsEfiShellBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *LastDeviceNode;
  EFI_DEVICE_PATH_PROTOCOL     *WorkDevicePath;
  BOOLEAN                      IsEfiShell;
  EFI_GUID                     *FileGuid;

  IsEfiShell = FALSE;
  WorkDevicePath = DevicePath;
  LastDeviceNode = DevicePath;

  while (!IsDevicePathEnd (WorkDevicePath)) {
    LastDeviceNode = WorkDevicePath;
    WorkDevicePath = NextDevicePathNode (WorkDevicePath);
  }
  FileGuid = EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode);
  if (FileGuid != NULL && (CompareGuid (FileGuid, PcdGetPtr(PcdShellFile)) || CompareGuid (FileGuid, PcdGetPtr(PcdMiniShellFile)))) {
    IsEfiShell = TRUE;
  }
  return IsEfiShell;

}

/**
  Check if the boot option is a EFI network boot option.

  @param  DevicePath             The device path need to be processed.

  @retval TRUE                   It is a EFI network boot option.
  @retval FALSE                  It is not a EFI network boot option.

**/
STATIC
BOOLEAN
IsEfiNetWorkBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *Node;

  for (Node = DevicePath; !IsDevicePathEndType (Node); Node = NextDevicePathNode (Node)) {
    if (DevicePathType (Node) == MESSAGING_DEVICE_PATH) {
      switch (DevicePathSubType (Node)) {
      case MSG_MAC_ADDR_DP:
      case MSG_IPv4_DP:
      case MSG_IPv6_DP:
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**
  Check if the boot option is a EFI hard drive boot option.

  @param  DevicePath             The device path need to be processed.

  @retval TRUE                   It is a EFI hard drive boot option.
  @retval FALSE                  It is not a EFI hard drive boot option.

**/
STATIC
BOOLEAN
IsEfiHarddriveBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *Node;

  for (Node = DevicePath; !IsDevicePathEndType (Node); Node = NextDevicePathNode (Node)) {
    if (DevicePathType (Node) == MEDIA_DEVICE_PATH &&
        DevicePathSubType (Node) == MEDIA_HARDDRIVE_DP) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Check if the boot option is a EFI USB boot option.

  @param  DevicePath             The device path need to be processed.

  @retval TRUE                   It is a EFI USB boot option.
  @retval FALSE                  It is not a EFI USB boot option.

**/
STATIC
BOOLEAN
IsEfiUsbBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *Node;

  for (Node = DevicePath; !IsDevicePathEndType (Node); Node = NextDevicePathNode (Node)) {
    if (DevicePathType (Node) == MESSAGING_DEVICE_PATH &&
        DevicePathSubType (Node) == MSG_USB_DP) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Use device path to get the boot priority of EFI boot option. The priority policy as below
  1. permanent OS created Boot Variables
  2. Firmware created "Widnows Boot Loader" for recovery when OS hard disk boot item is missing.
  3. Removable media with EFI spec boot \EFI\Boot\BootX64.efi or \EFI\Boot\BootIA32.efi
  4. Network load image items when EFI Network Boot is enabled.
  5. EFI Shell if found in ROM image

  @param  This                   A pointer to the EFI_BOOT_OPTION_POLICY_PROTOCOL instance.
  @param  CreateByOS             A Flag to indicate this boot otpion is create by OS or not.
  @param  DevicePath             A pointer to the EFI_DEVICE_PATH_PROTOCOL instance.
  @param  OptionPriority         A pointer to save boot option prioriy.

  @retval EFI_SUCCESS            Get option priority successful.
  @retval EFI_UNSUPPORTED        Input device path isn't valid or is a BBS device path.
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.

**/
EFI_STATUS
EFIAPI
GetEfiOptionPriority (
  IN     EFI_BOOT_OPTION_POLICY_PROTOCOL    *This,
  IN     BOOLEAN                            CreateByOS,
  IN     EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  OUT    EFI_BOOT_ORDER_PRIORITY            *OptionPriority
  )
{
  EFI_STATUS               Status;
  EFI_DEVICE_PATH_PROTOCOL *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  EFI_HANDLE               Handle;
  EFI_BLOCK_IO_PROTOCOL    *BlkIo;

  if (This == NULL || DevicePath == NULL || OptionPriority == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BBS_DEVICE_PATH == DevicePathType (DevicePath) && BBS_BBS_DP == DevicePathSubType (DevicePath)) {
    return EFI_UNSUPPORTED;
  }

  *OptionPriority = OtherEfiBootOption;
  //
  // Check USB Entry for Portable Workspace first
  //
  if (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP) {

    *OptionPriority = CreateByOS ? OsCreateEfiBootOption : RecoveryCreateEfiBootOption;
  } else {
    //
    // Get hard Driver Full device path
    //
    NewDevicePath = AppendHardDrivePathToBlkIoDevicePath ((HARDDRIVE_DEVICE_PATH *) DevicePath);
    if (NewDevicePath == NULL) {
      NewDevicePath = DevicePath;
    }
    TempDevicePath = NewDevicePath;
    Status = gBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &TempDevicePath,
                    &Handle
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->HandleProtocol (
                      Handle,
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );
      if (!EFI_ERROR (Status)) {
        if (!BlkIo->Media->RemovableMedia &&
            !IsEfiUsbBootOption (NewDevicePath) &&
            IsEfiHarddriveBootOption (NewDevicePath)) {
          *OptionPriority = CreateByOS ? OsCreateEfiBootOption : RecoveryCreateEfiBootOption;
        } else {
          *OptionPriority = RemovableEfiBootOption;
        }
      }
    } else if (IsEfiNetWorkBootOption (DevicePath)) {
      //
      // Check this boot option is whether EFI network boot option
      //
      *OptionPriority = NetWorkEfiBootOption;
    } else if (IsEfiShellBootOption (DevicePath)) {
      //
      // Check this boot option is wheter shell found in ROM image
      //
      *OptionPriority = ShellEfiBootOption;
    }
    if (NewDevicePath != DevicePath) {
      gBS->FreePool (NewDevicePath);
    }
  }

  return EFI_SUCCESS;
}

