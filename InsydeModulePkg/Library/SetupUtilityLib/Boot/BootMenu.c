/** @file
  Boot menu relative functions

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

#include "BootMenu.h"
#include <Library/PrintLib.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/BootOptionPolicy.h>
#include <Protocol/UsbIo.h>
#include <Protocol/BlockIo.h>

STATIC
EFI_STATUS
SortBbsTypeTable (
  UINTN                                     BbsDevTableTypeCount,
  UINT16                                    BbsTotalCount,
  UINT16                                    *TempBootType,
  UINT16                                    *TempPriorityList,
  UINT16                                    BbsAdvCount
);

STATIC
EFI_STATUS
InitBbsTypeTableDevTypeCount (
  IN STRING_PTR                             *LegacyBootOrderStringPtr,
  IN UINTN                                  LegacyBootDeviceNum,
  IN UINTN                                  LegacyBootDevTypeCount,
  IN UINT16                                 *LegacyBootDevType
  );

STATIC
EFI_STATUS
InitBbsTypeTableStringPtr (
  IN STRING_PTR                             *LegacyBootOrderStringPtr,
  IN UINTN                                  LegacyBootDevNum,
  IN UINTN                                  LegacyBootDevTypeCount,
  IN UINT16                                 *LegacyBootDevType
  );

STATIC
EFI_STATUS
UpdateNewForm (
  IN EFI_HII_HANDLE                            HiiHandle,
  IN UINT8                                     IfrFlag,
  IN STRING_REF                                *LastToken,
  IN STRING_PTR                                *BootString,
  IN UINTN                                     BootDevCount,
  IN UINT16                                    DeviceKeyBase,
  IN UINT16                                    BootDevLabel,
  IN EFI_FORM_ID                               FormId,
  IN VOID                                      *OpCodeHandle
);

STATIC
EFI_STATUS
GetBootDevList (
  IN     UINT16                             *BootOrder,
  IN     UINT16                             BootDeviceNum,
  IN OUT STRING_PTR                         **BootOrderString,
  IN OUT UINT16                             **TempBootOrder
  );

STATIC
EFI_STATUS
InitBbsTypeTable (
  IN UINTN                                  LegacyBootDeviceNum,
  IN UINTN                                  LegacyBootDevTypeCount,
  IN UINT16                                 *LegacyBootDevType,
  IN STRING_PTR                             *LegacyBootOrderStringPtr
  );

STATIC
EFI_STATUS
SyncBootOrder (
  IN     KERNEL_CONFIGURATION               *KernelConfig,
  IN     UINTN                              AdvBootDevNum,
  IN OUT UINT16                             *AdvBootOrder,
  IN OUT STRING_PTR                         *AdvBootDevStringPtr,
  IN     UINTN                              LegacyBootDevNum,
  IN OUT UINT16                             *LegacyBootOrder,
  IN     UINTN                              EfiBootDevNum,
  IN     UINT16                             *EfiBootOrder
  );

STATIC
EFI_STATUS
GetNewBootOrder (
  IN  UINT16                                CurrentBootDevFlag,
  IN  UINT16                                BootDevCount,
  IN  STRING_PTR                            *AdvBootOrderStringPtr,
  OUT UINT16                                *TargetBootDevCount,
  OUT UINT16                                **TargetBootOrder,
  OUT STRING_PTR                            **TargetBootOrderStringPtr
  );

EFI_STATUS
GetLegacyBootDevTypeOrderInfo (
  IN  EFI_HII_HANDLE                        HiiHandle,
  IN  KERNEL_CONFIGURATION                  *KernelConfig,
  IN  UINT16                                *LegacyBootDevType,
  IN  UINT16                                LegacyBootDevTypeCount,
  OUT STRING_PTR                            **BootTypeOrderStringPtr
  );

/**
 Get STRING_PTR information for legacy boot device type order, boot devices order, legacy boot device order and
 EFI boot device order

 @retval EFI_SUCCESS              Get STRING_PTR information successfully
 @retval Other                    Locate setup utility private data fail
**/
EFI_STATUS
GetBbsName (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN OUT KERNEL_CONFIGURATION               *KernelConfig,
  OUT    STRING_PTR                         **BootTypeOrderString,
  OUT    STRING_PTR                         **AdvBootOrderString,
  OUT    STRING_PTR                         **LegacyAdvString,
  OUT    STRING_PTR                         **EfiBootOrderString
  )
{
  EFI_STATUS                                Status;
  UINT16                                    *BootOrder;
  UINT16                                    *EfiBootOrder;
  UINT16                                    *TempBootOrder;
  UINT16                                    *LegacyAdvBootOrder;
  UINT16                                    LegacyBootDevTypeCount;
  UINT16                                    *LegacyBootDevType;
  UINT16                                    AdvBootDeviceNum;
  UINT16                                    *LegacyBootDeviceNum;
  UINT16                                    *EfiBootDeviceNum;
  UINT16                                    CurrentBootDevFlag;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo             = SuBrowser->SUCInfo;
  BootOrder           = SUCInfo->BootOrder;
  AdvBootDeviceNum    = SUCInfo->AdvBootDeviceNum;
  LegacyBootDeviceNum = &(SUCInfo->LegacyBootDeviceNum);
  EfiBootDeviceNum    = &(SUCInfo->EfiBootDeviceNum);

  LegacyBootDevTypeCount = SUCInfo->LegacyBootDevTypeCount;
  LegacyBootDevType      = SUCInfo->LegacyBootDevType;
  TempBootOrder        = NULL;
  EfiBootOrder         = NULL;
  LegacyAdvBootOrder   = NULL;
  //
  // get Adv boot option order and string
  //
  Status = GetBootDevList (
             BootOrder,
             AdvBootDeviceNum,
             AdvBootOrderString,
             &TempBootOrder
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetLegacyBootDevTypeOrderInfo (
             HiiHandle,
             KernelConfig,
             LegacyBootDevType,
             LegacyBootDevTypeCount,
             BootTypeOrderString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Sort boot sequence of each device type
  //
  CurrentBootDevFlag = LEGACY_BOOT_DEV;
  Status = GetNewBootOrder (
             CurrentBootDevFlag,
             AdvBootDeviceNum,
             *AdvBootOrderString,
             LegacyBootDeviceNum,
             &LegacyAdvBootOrder,
             LegacyAdvString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = InitBbsTypeTable (
             *LegacyBootDeviceNum,
             LegacyBootDevTypeCount,
             LegacyBootDevType,
             *LegacyAdvString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // get Efi BootOrder string
  //
  CurrentBootDevFlag = EFI_BOOT_DEV;
  Status = GetNewBootOrder (
             CurrentBootDevFlag,
             AdvBootDeviceNum,
             *AdvBootOrderString,
             EfiBootDeviceNum,
             &EfiBootOrder,
             EfiBootOrderString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // according to BootMenu type flag, set AdvBootOrder as relative BootOrder
  //
  SyncBootOrder (
    KernelConfig,
    AdvBootDeviceNum,
    TempBootOrder,
    *AdvBootOrderString,
    *LegacyBootDeviceNum,
    LegacyAdvBootOrder,
    *EfiBootDeviceNum,
    EfiBootOrder
    );

  if (TempBootOrder != NULL) {
    CopyMem (
      BootOrder,
      TempBootOrder,
      sizeof(UINT16) * AdvBootDeviceNum
      );

    gBS->FreePool (TempBootOrder);
  }
  if (EfiBootOrder != NULL) {
    gBS->FreePool (EfiBootOrder);
  }
  if (LegacyAdvBootOrder != NULL) {
    gBS->FreePool (LegacyAdvBootOrder);
  }
  return EFI_SUCCESS;
}

/**
  Clean space character in the front and back of input string.

  @param Str             Input string

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
  Get all hardware boot device information (BlockIo device path and device name).

  @param HwBootDeviceInfoCount   Hardware boot device information count
  @param HwBootDeviceInfo        Array pointer of hardware boot device information

  @retval EFI_SUCCESS            Get information successfully
  @return Other                  Invalid input parameter.
                                 Locate protocol fail or get usb device information fail

**/
EFI_STATUS
GetAllHwBootDeviceInfo (
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
          SetupUtilityLibUpdateAtaString (
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

  if (HandleCount != 0) {
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
                                 "en-US",
                                 &DriverName
                                 );
      if (!EFI_ERROR (Status) && !StrCmp (DriverName, L"Usb Mass Storage Driver")) {
        Found = TRUE;
        break;
      }
    }
  }

  if (HandleCount != 0) {
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
                                 "en-US",
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

  if (HandleCount != 0) {
    gBS->FreePool (HandleBuffer);
  }

  if (*HwBootDeviceInfoCount == 0) {
    gBS->FreePool (*HwBootDeviceInfo);
    *HwBootDeviceInfo = NULL;
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

/**
  Function unpacks a device path data structure so that all the nodes
  of a device path are naturally aligned.

  @param DevPath         A pointer to a device path data structure

  @return pointer to the new device path or NULL if allocate memory fail.

**/
EFI_DEVICE_PATH_PROTOCOL *
UnpackDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Src;
  EFI_DEVICE_PATH_PROTOCOL  *Dest;
  EFI_DEVICE_PATH_PROTOCOL  *NewPath;
  UINTN                     Size;

  if (DevPath == NULL) {
    return NULL;
  }
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

  @param BlockIoDevicePath       Multi device path instances which need to check
  @param HardDriveDevicePath     A device path which starts with a hard drive media device path.

  @retval TRUE                   There is a matched device path instance
  @retval FALSE                  There is no matched device path instance

**/
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

  @param HardDriveDevicePath     Input device path

  @return pointer to the device path which combines BlockIo and hard disk device path
          or NULL if it is not hard disk device.

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

  @param  HardDriveDevicePath    BlockIo device path
  @param  DevicePath             BlockIo device path or hard drive device path

  @retval TRUE                   BlockIo device paths are the same
  @retval FALSE                  BlockIo device paths are different

**/
BOOLEAN
CompareBlockIoDevicePath (
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
  Append device name to boot option description.

  @param  EfiBootDeviceNum       Number of EFI boot option
  @param  UpdateEfiBootString    Array pointer of STRING_PTR which store each boot option information

  @retval EFI_SUCCESS            Append device name successfully
  @retval EFI_INVALID_PARAMETER  Invalid input parameter

**/
EFI_STATUS
BootOptionStrAppendDeviceName (
  IN     UINT16                         EfiBootDeviceNum,
  IN OUT STRING_PTR                     *UpdateEfiBootString
  )
{
  UINTN                              Index;
  UINTN                              Index2;
  HARDWARE_BOOT_DEVICE_INFO          *HwBootDeviceInfo;
  UINTN                              HwBootDeviceCount;
  CHAR16                             *String;
  EFI_DEVICE_PATH_PROTOCOL           *DevicePath;
  UINTN                              Size;
  CHAR16                             *HwDeviceName;
  CHAR16                             NoDeviceStr[] = L"No Device";
  UINTN                              DeviceCount;
  UINT8                              *Ptr;
  UINT8                              *VarData;

  if (EfiBootDeviceNum == 0 || UpdateEfiBootString == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GetAllHwBootDeviceInfo (&HwBootDeviceCount, &HwBootDeviceInfo);

  for (Index = 0; Index < EfiBootDeviceNum; Index++) {
    DevicePath = UpdateEfiBootString[Index].DevicePath;
    if (!((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH &&
           DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP) ||
          (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH &&
           DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP))) {
      continue;
    }

    VarData      = NULL;
    HwDeviceName = NULL;
    DeviceCount  = 1;
    if (DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH &&
        DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP) {
      //
      // For Windows To Go, assign no device string as device name if device number is 0.
      //
      VarData = CommonGetVariableData (
                  L"WindowsToGo",
                  &gEfiGenericVariableGuid
                  );
      if (VarData != NULL) {
        Ptr         = VarData;
        DevicePath  = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
        Ptr        += GetDevicePathSize (DevicePath);
        DeviceCount = *(UINT16 *) Ptr;
      } else {
        DeviceCount = 0;
      }

      if (DeviceCount == 0) {
        HwDeviceName = NoDeviceStr;
      }
    }

    //
    // Append device name to boot option description if BlkIo device path is match.
    //
    if (HwDeviceName == NULL) {
      for (Index2 = 0; Index2 < HwBootDeviceCount; Index2++) {
        if (CompareBlockIoDevicePath (HwBootDeviceInfo[Index2].BlockIoDevicePath, DevicePath)) {
          HwDeviceName = HwBootDeviceInfo[Index2].HwDeviceName;
          break;
        }
      }
    }

    if (HwDeviceName != NULL) {
      if (DeviceCount > 1) {
        Size = StrSize (UpdateEfiBootString[Index].pString) + StrSize (L" (") + StrSize (HwDeviceName) + StrSize (L",...") + StrSize (L")");
      } else {
        Size = StrSize (UpdateEfiBootString[Index].pString) + StrSize (L" (") + StrSize (HwDeviceName) + StrSize (L")");
      }
      String = AllocateZeroPool (Size);
      if (String != NULL) {
        StrCat (String, UpdateEfiBootString[Index].pString);
        StrCat (String, L" (");
        StrCat (String, HwDeviceName);
        if (DeviceCount > 1) {
          StrCat (String, L",...");
        }
        StrCat (String, L")");

        gBS->FreePool (UpdateEfiBootString[Index].pString);
        UpdateEfiBootString[Index].pString = String;
      }
    }

    if (VarData != NULL) {
      gBS->FreePool (VarData);
    }
  }

  if (HwBootDeviceCount != 0) {
    for (Index2 = 0; Index2 < HwBootDeviceCount; Index2++) {
      gBS->FreePool (HwBootDeviceInfo[Index2].HwDeviceName);
    }
    gBS->FreePool (HwBootDeviceInfo);
  }

  return EFI_SUCCESS;
}

/**
  Update boot order related menu option in action opcode in platform to boot page

  @param  BootHiiHandle          Hii hanlde for Boot page
  @param  SetupVariable          Pointer to SYSTEM_CONFIGURATION instance

  @retval EFI_SUCCESS            Update boot menu successful
  @retval Other                  Some error occured in update boot menu

**/
EFI_STATUS
UpdateBootOrderToActionOpcode (
  IN     EFI_HII_HANDLE                         BootHiiHandle,
  IN OUT KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_HII_HANDLE                        HiiHandle;
  STRING_REF                            LastToken;
  UINT8                                 IfrFlag;
  VOID                                  *UpdateBootTypeOrderOpCodeHandle;
  VOID                                  *UpdateAdvOpCodeHandle;
  VOID                                  *UpdateAdvLegacyOpCodeHandle;
  VOID                                  *UpdateEfiOpCodeHandle;
  VOID                                  *UpdateBootDeviceOpCodeHandle[MAX_BOOT_ORDER_NUMBER];
  EFI_IFR_GUID_LABEL                    *StartLabel;
  UINT16                                LegacyBootTypeOrderCount;
  UINT16                                AdvBootDeviceNum;
  UINT16                                LegacyBootDeviceNum;
  UINT16                                EfiBootDeviceNum;
  STRING_PTR                            *UpdateAdvBootString;
  STRING_PTR                            *UpdateLegacyAdvBootString;
  STRING_PTR                            *UpdateEfiBootString;
  STRING_PTR                            *UpdateBootTypeOrderString;
  UINT16                                BootDevLabel;
  UINT16                                DevKeyBase;
  UINT16                                *TempBootType;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_IFR_ACTION                        *ActionOpCode;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Initial the flag of showing from.
  //
  KernelConfig->NoAdvBootDev    = 0;
  KernelConfig->NoBootTypeOrder = 0;
  KernelConfig->NoLegacyBootDev = 0;
  KernelConfig->NoEfiBootDev    = 0;

  for(Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    mBootConfig.NoBootDevs[Index] = 0;
  }

  //
  // if there is no BootOption, skip to build from.
  //
  if (SuBrowser->SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  HiiHandle = BootHiiHandle;
  SUCInfo   = SuBrowser->SUCInfo;

  UpdateAdvBootString       = NULL;
  UpdateLegacyAdvBootString = NULL;
  UpdateEfiBootString       = NULL;
  UpdateBootTypeOrderString = NULL;

  Status = GetBbsName (
             HiiHandle,
             KernelConfig,
             &UpdateBootTypeOrderString,
             &UpdateAdvBootString,
             &UpdateLegacyAdvBootString,
             &UpdateEfiBootString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LegacyBootTypeOrderCount = SUCInfo->LegacyBootDevTypeCount;
  TempBootType             = SUCInfo->LegacyBootDevType;
  LegacyBootDeviceNum      = SUCInfo->LegacyBootDeviceNum;
  EfiBootDeviceNum         = SUCInfo->EfiBootDeviceNum;
  AdvBootDeviceNum         = SUCInfo->AdvBootDeviceNum;

  //
  // Allocate space for creation of Buffer
  //
  UpdateBootTypeOrderOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (UpdateBootTypeOrderOpCodeHandle != NULL);

  UpdateAdvOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (UpdateAdvOpCodeHandle != NULL);

  UpdateAdvLegacyOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (UpdateAdvLegacyOpCodeHandle != NULL);

  UpdateEfiOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (UpdateEfiOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (UpdateBootTypeOrderOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = BOOT_ORDER_LABEL;

  for (Index = 0; Index < LegacyBootTypeOrderCount; Index++) {
    UpdateBootDeviceOpCodeHandle[TempBootType[Index]] = HiiAllocateOpCodeHandle ();
    ASSERT (UpdateBootDeviceOpCodeHandle[TempBootType[Index]] != NULL);
  }

  //
  // assign data address by type
  //
  IfrFlag = EFI_IFR_CHECKBOX_DEFAULT_MFG;
  //
  //update Boot Type Order form
  //
  LastToken = 0;
  if (LegacyBootTypeOrderCount != 0) {
    KernelConfig->NoBootTypeOrder = 1;
  }

  if (UpdateBootTypeOrderString != NULL) {
    for (Index = 0; Index < (UINT16) LegacyBootTypeOrderCount; Index++, LastToken = 0) {
      LastToken = HiiSetString (HiiHandle, 0, UpdateBootTypeOrderString[Index].pString, NULL);
      if (Index == 0) {
        SUCInfo->BootTypeTokenRecord = LastToken;
      }

      ActionOpCode = (EFI_IFR_ACTION *) HiiCreateActionOpCode (
                                          UpdateBootTypeOrderOpCodeHandle,
                                          (EFI_QUESTION_ID) (KEY_BOOT_TYPE_ORDER_BASE + Index),
                                          LastToken,
                                          0,
                                          0,
                                          0
                                          );
      ActionOpCode->Question.Flags = EFI_IFR_CHECKBOX_DEFAULT_MFG;
    }
  }

  //
  //We need remove the same data we update to HII database before.
  //
  HiiUpdateForm (
    HiiHandle,
    NULL,
    BOOT_DEVICE_LEG_NOR_BOOT_ID,
    UpdateBootTypeOrderOpCodeHandle,
    NULL
    );

  //
  // update Legacy adv boot form
  //
  LastToken = 0;

  if (LegacyBootDeviceNum != 0) {
    KernelConfig->NoLegacyBootDev = 1;
  }

  if (EfiBootDeviceNum != 0) {
    KernelConfig->NoEfiBootDev = 1;
  }

  BootDevLabel = BOOT_LEGACY_ADV_BOOT_LABEL;
  DevKeyBase = KEY_ADV_LEGACY_BOOT_BASE;
  UpdateNewForm (
    HiiHandle,
    IfrFlag,
    &LastToken,
    UpdateLegacyAdvBootString,
    LegacyBootDeviceNum,
    DevKeyBase,
    BootDevLabel,
    BOOT_OPTION_FORM_ID,
    UpdateAdvOpCodeHandle
    );
  SUCInfo->LegacyAdvanceTokenRecord = LastToken;

  //
  // update boot device form by type
  //
  for (Index = 0; Index < LegacyBootTypeOrderCount; Index++) {
    LastToken = 0;

    if (mBbsTypeTable[TempBootType[Index]].DeviceTypeCount != 0) {
      mBootConfig.NoBootDevs[TempBootType[Index]]= 1;
    }
    UpdateNewForm (
      HiiHandle,
      IfrFlag,
      &LastToken,
      (STRING_PTR *) mBbsTypeTable[TempBootType[Index]].StringPtr,
      mBbsTypeTable[TempBootType[Index]].DeviceTypeCount,
      mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase,
      mBbsTypeTable[TempBootType[Index]].BootDevicesLabel,
      mBbsTypeTable[TempBootType[Index]].FormId,
      UpdateBootDeviceOpCodeHandle[TempBootType[Index]]
      );
    SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]] = LastToken;
  }

  //
  // update EFI boot form
  //
  LastToken = 0;
  SUCInfo->EfiTokenRecord = LastToken;

  BootDevLabel = EFI_BOOT_DEVICE_LABEL;
  DevKeyBase   = KEY_EFI_BOOT_DEVICE_BASE;

  BootOptionStrAppendDeviceName (EfiBootDeviceNum, UpdateEfiBootString);
  UpdateNewForm (
    HiiHandle,
    IfrFlag,
    &LastToken,
    UpdateEfiBootString,
    EfiBootDeviceNum,
    DevKeyBase,
    BootDevLabel,
    BOOT_DEVICE_EFI_FORM_ID,
    UpdateEfiOpCodeHandle
    );
  SUCInfo->EfiTokenRecord = LastToken;

  if (UpdateBootTypeOrderString != NULL) {
    for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
      if (UpdateBootTypeOrderString[Index].pString != NULL) {
        gBS->FreePool (UpdateBootTypeOrderString[Index].pString);
      }
    }
    gBS->FreePool (UpdateBootTypeOrderString);
  }

  HiiFreeOpCodeHandle (UpdateBootTypeOrderOpCodeHandle);
  HiiFreeOpCodeHandle (UpdateAdvOpCodeHandle);
  HiiFreeOpCodeHandle (UpdateAdvLegacyOpCodeHandle);
  HiiFreeOpCodeHandle (UpdateEfiOpCodeHandle);

  if (UpdateAdvBootString != NULL) {
    for (Index = 0; Index < AdvBootDeviceNum; Index++) {
      if (UpdateAdvBootString[Index].BbsEntry != NULL) {
        gBS->FreePool (UpdateAdvBootString[Index].BbsEntry);
      }
      if (UpdateAdvBootString[Index].pString != NULL) {
        gBS->FreePool (UpdateAdvBootString[Index].pString);
      }
      if (UpdateAdvBootString[Index].DevicePath != NULL) {
        gBS->FreePool (UpdateAdvBootString[Index].DevicePath);
      }
    }
    gBS->FreePool (UpdateAdvBootString);
  }

  for (Index = 0; Index < LegacyBootTypeOrderCount; Index++) {
     HiiFreeOpCodeHandle (UpdateBootDeviceOpCodeHandle[TempBootType[Index]]);
  }
  gBS->FreePool (UpdateLegacyAdvBootString);

  if (UpdateEfiBootString != NULL) {
    for (Index = 0; Index < EfiBootDeviceNum; Index++) {
      if (UpdateEfiBootString[Index].pString != NULL) {
        gBS->FreePool (UpdateEfiBootString[Index].pString);
      }
    }
    gBS->FreePool (UpdateEfiBootString);
  }

  return EFI_SUCCESS;
}

/**
  Move one boot type in the BootTypeOrder.

  @param  UpShift                TRUE : move LowerType upon UpperType
                                 FALSE: move UpperType below LowerType
  @param  UpperType              Upper boot type
  @param  LowerType              Lower boot type
  @param  BootTypeOrder          Pointer to boot type order
  @param  TypeCount              Boot type count

**/
VOID
MoveBootType (
  IN     BOOLEAN   UpShift,
  IN     UINT8     UpperType,
  IN     UINT8     LowerType,
  IN OUT UINT8     *BootTypeOrder,
  IN     UINTN     TypeCount
  )
{
  UINT8       *OldBootTypeOrder;
  UINTN       OldBootTypeIndex;
  UINTN       BootTypeIndex;

  if (TypeCount == 0) {
    return;
  }
  OldBootTypeOrder = AllocateZeroPool (TypeCount * sizeof (UINT8));
  if (OldBootTypeOrder == NULL) {
    return;
  }
  CopyMem (OldBootTypeOrder, BootTypeOrder, TypeCount * sizeof (UINT8));

  BootTypeIndex = 0;
  if (UpShift) {
    for (OldBootTypeIndex = 0; OldBootTypeIndex < TypeCount; OldBootTypeIndex++) {
      if (OldBootTypeOrder[OldBootTypeIndex] != UpperType &&
          OldBootTypeOrder[OldBootTypeIndex] != LowerType) {
        BootTypeOrder[BootTypeIndex++] = OldBootTypeOrder[OldBootTypeIndex];
      } else if (OldBootTypeOrder[OldBootTypeIndex] == UpperType) {
        BootTypeOrder[BootTypeIndex++] = LowerType;
        BootTypeOrder[BootTypeIndex++] = UpperType;
      }
    }
  } else {
    for (OldBootTypeIndex = 0; OldBootTypeIndex < TypeCount; OldBootTypeIndex++) {
      if (OldBootTypeOrder[OldBootTypeIndex] != UpperType &&
          OldBootTypeOrder[OldBootTypeIndex] != LowerType) {
        BootTypeOrder[BootTypeIndex++] = OldBootTypeOrder[OldBootTypeIndex];
      } else if (OldBootTypeOrder[OldBootTypeIndex] == LowerType) {
        BootTypeOrder[BootTypeIndex++] = LowerType;
        BootTypeOrder[BootTypeIndex++] = UpperType;
      }
    }
  }

  gBS->FreePool (OldBootTypeOrder);
}

EFI_STATUS
Shiftitem (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL      *This,
  IN     UINTN                                  LastTokenRecord,
  IN     UINTN                                  DeviceNum,
  IN     BOOLEAN                                UpShift,
  IN OUT KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  CHAR16                                    *FirstUpdateStr;
  CHAR16                                    *SecondUpdateStr;
  STRING_REF                                FirstToken;
  STRING_REF                                SecondToken;
  UINT16                                    Temp;
  UINTN                                     Index;
  UINTN                                     Index1;
  UINTN                                     Index2;
  UINT16                                    *TempBootType;
  UINTN                                     DeviceNumBase[MAX_BOOT_ORDER_NUMBER];
  UINTN                                     DeviceNumBaseByType[MAX_BOOT_ORDER_NUMBER];
  UINTN                                     BootTypeOrderBase[MAX_BOOT_ORDER_NUMBER];
  UINTN                                     TempDeviceNumBase;
  UINTN                                     DeviceBaseAddress;
  UINT16                                    BbsDevTableTypeCount;
  UINT16                                    *TempBootOrder;
  EFI_STATUS                                Status;
  UINTN                                     Index3;
  UINTN                                     BootIndex;
  STRING_PTR                                *TempPtr;
  UINT8                                     *TempBootTypeOrder;
  BOOLEAN                                   NewBootDeviceType;
  UINT8                                     MoveType[2];
  UINT8                                     *OldBootTypeOrder;
  UINTN                                     OldBootTypeIndex;
  UINTN                                     BootMenuType;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  EFI_CALLBACK_INFO                         *BootCallBackInfo;
  EFI_HII_HANDLE                            HiiHandle;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BootCallBackInfo     = EFI_CALLBACK_INFO_FROM_THIS (This);
  HiiHandle            = BootCallBackInfo->HiiHandle;
  SUCInfo              = SuBrowser->SUCInfo;
  BbsDevTableTypeCount = SUCInfo->LegacyBootDevTypeCount;
  TempBootType         = SUCInfo->LegacyBootDevType;
  BootMenuType         = 0;

  ZeroMem (DeviceNumBase, sizeof (DeviceNumBase));
  ZeroMem (DeviceNumBaseByType, sizeof (DeviceNumBaseByType));
  ZeroMem (BootTypeOrderBase, sizeof (BootTypeOrderBase));

  if (KernelConfig->BootMenuType == 0 && KernelConfig->LegacyNormalMenuType == 0) {
    for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
      for (Index1 = 0; Index1 < BbsDevTableTypeCount; Index1++) {
        if (KernelConfig->BootTypeOrder[Index] == mBbsTypeTable[TempBootType[Index1]].DeviceType) {
          DeviceNumBase[Index] = mBbsTypeTable[TempBootType[Index1]].DeviceTypeCount;
          break;
        }
      }
      if (Index1 == BbsDevTableTypeCount) {
        return EFI_INVALID_PARAMETER;
      }
      if (Index == 0) {
        if (KernelConfig->BootNormalPriority == EFI_FIRST) {
          DeviceNumBaseByType[Index] = DeviceNum + SUCInfo->EfiBootDeviceNum;
        } else {
          DeviceNumBaseByType[Index] = DeviceNum;
        }
      } else {
      	TempDeviceNumBase = 0;
      	for (Index2 = 0; Index2 < Index; Index2++) {
          TempDeviceNumBase = TempDeviceNumBase + DeviceNumBase[Index2];
        }
        if (KernelConfig->BootNormalPriority == EFI_FIRST) {
          DeviceNumBaseByType[Index] = TempDeviceNumBase + DeviceNum + SUCInfo->EfiBootDeviceNum;
        } else {
          DeviceNumBaseByType[Index] = TempDeviceNumBase + DeviceNum;
        }
      }
      BootTypeOrderBase[TempBootType[Index1]] = DeviceNumBaseByType[Index];
    }
  }

  FirstToken        = 0;
  Index             = 0;
  DeviceBaseAddress = 0;
  //
  // is it Boot device type menu?
  //
  if (LastTokenRecord == SUCInfo->BootTypeTokenRecord) {
    FirstToken = (STRING_REF) (SUCInfo->BootTypeTokenRecord + DeviceNum);
    DeviceBaseAddress = 0xff;
    Index = BbsDevTableTypeCount + 2;
    BootMenuType = LEGACY_DEVICE_TYPE_MENU;
  }

  //
  // is it legacy advance menu?
  //
  if (LastTokenRecord == SUCInfo->LegacyAdvanceTokenRecord) {
    FirstToken    = (STRING_REF) (SUCInfo->LegacyAdvanceTokenRecord + DeviceNum);
    if (KernelConfig->BootNormalPriority == EFI_FIRST) {
      DeviceBaseAddress = DeviceNum + SUCInfo->EfiBootDeviceNum;
    } else {
      DeviceBaseAddress = DeviceNum;
    }
    Index = BbsDevTableTypeCount + 2;
    BootMenuType = LEGACY_ADV_MENU;
  }

  //
  // is it Efi device menu?
  //
  if (LastTokenRecord == SUCInfo->EfiTokenRecord) {
    FirstToken    = (STRING_REF)(SUCInfo->EfiTokenRecord + DeviceNum);
    if (KernelConfig->BootNormalPriority == LEGACY_FIRST) {
      DeviceBaseAddress = DeviceNum + SUCInfo->LegacyBootDeviceNum;
    } else {
      DeviceBaseAddress = DeviceNum;
    }
    Index = BbsDevTableTypeCount + 2;
    BootMenuType = EFI_BOOT_MENU;
  }

  //
  // is it advance menu(show all boot device)?
  //
  if (LastTokenRecord == SUCInfo->AdvanceTokenRecord) {
    FirstToken        = (STRING_REF) (SUCInfo->AdvanceTokenRecord + DeviceNum);
    DeviceBaseAddress = DeviceNum;
    Index             = BbsDevTableTypeCount + 2;
    BootMenuType      = ADV_BOOT_MENU;
  }

  //
  // is it legacy normal menu(show all boot device)?
  //
  for (; Index < BbsDevTableTypeCount; Index++) {
    if (LastTokenRecord == SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]]) {
       FirstToken        = (STRING_REF) (SUCInfo->LegacyNormalTokenRecord[TempBootType[Index]] + DeviceNum);
       DeviceBaseAddress = BootTypeOrderBase[TempBootType[Index]];
       BootMenuType      = LEGACY_NORMAL_MENU;
       break;
    }
  }

  if (Index == BbsDevTableTypeCount) {
     return EFI_INVALID_PARAMETER;
  }

  TempBootOrder = (UINT16 *) SUCInfo->BootOrder;
  if (UpShift) {
    SecondToken = (STRING_REF) (FirstToken - 1);
    if (DeviceBaseAddress != 0xff) {
      Temp    = TempBootOrder[DeviceBaseAddress - 1];
      TempBootOrder[DeviceBaseAddress - 1]  = TempBootOrder[DeviceBaseAddress];
      TempBootOrder[DeviceBaseAddress]      = Temp;
    } else {
      Temp     = KernelConfig->BootTypeOrder[DeviceNum - 1];
      KernelConfig->BootTypeOrder[DeviceNum - 1] = KernelConfig->BootTypeOrder[DeviceNum];
      KernelConfig->BootTypeOrder[DeviceNum]     = (UINT8) Temp;
    }
  } else {
    SecondToken = (STRING_REF)(FirstToken + 1);
    if (DeviceBaseAddress != 0xff) {
      Temp    = TempBootOrder[DeviceBaseAddress + 1];
      TempBootOrder[DeviceBaseAddress + 1] = TempBootOrder[DeviceBaseAddress];
      TempBootOrder[DeviceBaseAddress]     = Temp;
    } else {
      Temp = KernelConfig->BootTypeOrder[DeviceNum + 1];
      KernelConfig->BootTypeOrder[DeviceNum + 1] = KernelConfig->BootTypeOrder[DeviceNum];
      KernelConfig->BootTypeOrder[DeviceNum]     = (UINT8) Temp;
    }
  }

  SecondUpdateStr = HiiGetString (HiiHandle, (EFI_STRING_ID) SecondToken, NULL);
  FirstUpdateStr  = HiiGetString (HiiHandle, (EFI_STRING_ID) FirstToken, NULL);

  HiiSetString (HiiHandle, (EFI_STRING_ID) FirstToken, SecondUpdateStr, NULL);
  HiiSetString (HiiHandle, (EFI_STRING_ID) SecondToken, FirstUpdateStr, NULL);

  gBS->FreePool (FirstUpdateStr);
  gBS->FreePool (SecondUpdateStr);

  //
  // sync the BootOrder between legacy normal and advance boot mode
  //
  switch (BootMenuType) {

  case ADV_BOOT_MENU :
  case LEGACY_ADV_MENU :
    BootIndex = 0;
    TempBootTypeOrder = AllocateZeroPool (sizeof (KernelConfig->BootTypeOrder));
    if (TempBootTypeOrder == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    SetMem (TempBootTypeOrder, sizeof (KernelConfig->BootTypeOrder), 0xff);
    for (Index = 0; Index < SUCInfo->AdvBootDeviceNum; Index++) {
      for (Index1 = 0; Index1 < BbsDevTableTypeCount; Index1++) {
        TempPtr = (STRING_PTR *) mBbsTypeTable[TempBootType[Index1]].StringPtr;
        for (Index2 = 0; Index2 < mBbsTypeTable[TempBootType[Index1]].DeviceTypeCount; Index2++) {
          if (TempPtr[Index2].BootOrderIndex == TempBootOrder[Index]) {
            NewBootDeviceType = TRUE;
            for (Index3 = 0; Index3 < BootIndex; Index3++) {
              if (TempBootTypeOrder[Index3] == mBbsTypeTable[TempBootType[Index1]].DeviceType) {
                NewBootDeviceType = FALSE;
                break;
              }
            }
            if (NewBootDeviceType == TRUE ) {
              TempBootTypeOrder[BootIndex++] = mBbsTypeTable[TempBootType[Index1]].DeviceType;
            }
            break;
          }
        }
      }
    }

    //
    // First, find out oringial boot type order which show in Boot/Legacy/Advance.
    // Then comparing with new boot type order to find out which one boot type shift.
    //
    OldBootTypeOrder = AllocateZeroPool (sizeof (KernelConfig->BootTypeOrder));
    if (OldBootTypeOrder == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    OldBootTypeIndex = 0;

    for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
      for (Index1 = 0; Index1 < BootIndex; Index1++) {
        if (KernelConfig->BootTypeOrder[Index] == TempBootTypeOrder[Index1]) {
          OldBootTypeOrder[OldBootTypeIndex++] = KernelConfig->BootTypeOrder[Index];
          break;
        }
      }
      if (OldBootTypeIndex == BootIndex) {
        break;
      }
    }

    Index1 = 0;
    for (Index = 0; Index < BootIndex; Index++) {
      if (OldBootTypeOrder[Index] != TempBootTypeOrder[Index]) {
        MoveType[Index1++] = OldBootTypeOrder[Index];
      }
      if (Index1 == 2) {
        break;
      }
    }

    if (Index1 != 0) {
      MoveBootType (
        UpShift,
        MoveType[0],
        MoveType[1],
        KernelConfig->BootTypeOrder,
        BbsDevTableTypeCount
        );
    }
    gBS->FreePool (OldBootTypeOrder);
    gBS->FreePool (TempBootTypeOrder);

  case LEGACY_NORMAL_MENU :
    Status = SortBbsTypeTable (
               BbsDevTableTypeCount,
               MAX_BOOT_DEVICES_NUMBER,
               TempBootType,
               TempBootOrder,
               SUCInfo->AdvBootDeviceNum
               );
  break;

  case LEGACY_DEVICE_TYPE_MENU :
    if (KernelConfig->BootNormalPriority == EFI_FIRST) {
      BootIndex = SUCInfo->EfiBootDeviceNum;
    } else {
      BootIndex = 0;
    }
    for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
      for (Index1 = 0; Index1 < BbsDevTableTypeCount; Index1++) {
      	if (KernelConfig->BootTypeOrder[Index] ==  mBbsTypeTable[TempBootType[Index1]].DeviceType) {
      	  TempPtr = (STRING_PTR *) mBbsTypeTable[TempBootType[Index1]].StringPtr;
          for (Index2 = 0; Index2 < mBbsTypeTable[TempBootType[Index1]].DeviceTypeCount; Index2++) {
       	    TempBootOrder[BootIndex++] = TempPtr[Index2].BootOrderIndex;
          }
          break;
        }
      }
    }
    break;

  default :
    break;
  }

  return EFI_SUCCESS;
}

/**
 Get the index value of BBS type table for specific device type

 @param[in] DevType                  Target device type

 @return the index value of BBS type table or the count of BBS type table if not found
**/
UINTN
GetIndexOfBbsTypeTableByDevType (
  IN UINT8                                  DevType
  )
{
  UINTN                                     Index;
  UINTN                                     BbsTypeTableCount;

  BbsTypeTableCount = mBbsTypeTableCount;

  for (Index = 0; Index < BbsTypeTableCount; Index++) {
    if (mBbsTypeTable[Index].DeviceType == DevType) {
      break;
    }
  }

  return Index;
}

/**
 Initialize module variable mBbsTypeTable[]

 @param[in] LegacyBootDevNum         Number of legacy boot device
 @param[in] LegacyBootDevTypeCount   Number of legacy boot device type
 @param[in] LegacyBootDevType        Pointer to the legacy boot device type order
 @param[in] LegacyBootOrderStringPtr Pointer to the legacy boot device information

 @retval EFI_SUCCESS                 Update module variable successfully
**/
STATIC
EFI_STATUS
InitBbsTypeTable (
  IN UINTN                                  LegacyBootDevNum,
  IN UINTN                                  LegacyBootDevTypeCount,
  IN UINT16                                 *LegacyBootDevType,
  IN STRING_PTR                             *LegacyBootOrderStringPtr
  )
{
  InitBbsTypeTableDevTypeCount (
    LegacyBootOrderStringPtr,
    LegacyBootDevNum,
    LegacyBootDevTypeCount,
    LegacyBootDevType
    );

  InitBbsTypeTableStringPtr (
    LegacyBootOrderStringPtr,
    LegacyBootDevNum,
    LegacyBootDevTypeCount,
    LegacyBootDevType
    );

  return EFI_SUCCESS;
}

/**
 Update module variable mBbsTypeTable[].StringPtr and mBbsTypeTable[].CurrentIndex

 @param[in] LegacyBootOrderStringPtr Pointer to the legacy boot device information
 @param[in] LegacyBootDevNum         Number of legacy boot device
 @param[in] LegacyBootDevTypeCount   Number of legacy boot device type
 @param[in] LegacyBootDevType        Pointer to the legacy boot device type order

 @retval EFI_SUCCESS                 Update module variable successfully
 @retval EFI_OUT_OF_RESOURCES        Allocate poll fail
**/
STATIC
EFI_STATUS
InitBbsTypeTableStringPtr (
  IN STRING_PTR                             *LegacyBootOrderStringPtr,
  IN UINTN                                  LegacyBootDevNum,
  IN UINTN                                  LegacyBootDevTypeCount,
  IN UINT16                                 *LegacyBootDevType
  )
{
  UINTN                                     Index;
  UINTN                                     DevTypeIndex;
  UINTN                                     CurrentIndex;
  UINTN                                     OtherDeviceTypeIndex;
  STRING_PTR                                *TempString;
  UINTN                                     BbsTypeTableCount;

  OtherDeviceTypeIndex = GetIndexOfBbsTypeTableByDevType (OTHER_DRIVER);
  BbsTypeTableCount    = mBbsTypeTableCount;

  for (Index = 0; Index < BbsTypeTableCount; Index++) {
    mBbsTypeTable[Index].CurrentIndex = 0;

    if (mBbsTypeTable[Index].DeviceTypeCount == 0) {
      continue;
    }

    TempString = AllocateZeroPool (sizeof (STRING_PTR) * (mBbsTypeTable[Index].DeviceTypeCount));
    if (TempString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    if ((VOID *) (mBbsTypeTable[Index].StringPtr) != NULL) {
      gBS->FreePool ((VOID *)(mBbsTypeTable[Index].StringPtr));
    }

    mBbsTypeTable[Index].StringPtr = (UINTN) TempString;
  }

  for (Index = 0; Index < LegacyBootDevNum; Index++) {
    if (LegacyBootOrderStringPtr[Index].BbsEntry == NULL) {
      continue;
    }

    for (DevTypeIndex = 0; DevTypeIndex < LegacyBootDevTypeCount; DevTypeIndex++) {
      if (IsUsbDevice (LegacyBootOrderStringPtr[Index].BbsEntry)) {
        if (mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].DeviceType == BBS_USB) {
          CurrentIndex                            = (UINT16)       mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].CurrentIndex;
          TempString                              = (STRING_PTR *) mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].StringPtr;
          TempString[CurrentIndex].BootOrderIndex = LegacyBootOrderStringPtr[Index].BootOrderIndex;
          TempString[CurrentIndex].EfiBootDevFlag = LegacyBootOrderStringPtr[Index].EfiBootDevFlag;
          TempString[CurrentIndex].pString        = LegacyBootOrderStringPtr[Index].pString;
          TempString[CurrentIndex].BbsEntry       = LegacyBootOrderStringPtr[Index].BbsEntry;
          (mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].CurrentIndex)++;
          break;
        }
      } else if (LegacyBootOrderStringPtr[Index].BbsEntry->DeviceType == mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].DeviceType) {
        CurrentIndex                              = (UINT16)       mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].CurrentIndex;
        TempString                                = (STRING_PTR *) mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].StringPtr;
        TempString[CurrentIndex].BootOrderIndex   = LegacyBootOrderStringPtr[Index].BootOrderIndex;
        TempString[CurrentIndex].EfiBootDevFlag   = LegacyBootOrderStringPtr[Index].EfiBootDevFlag;
        TempString[CurrentIndex].pString          = LegacyBootOrderStringPtr[Index].pString;
        TempString[CurrentIndex].BbsEntry         = LegacyBootOrderStringPtr[Index].BbsEntry;
        (mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].CurrentIndex)++;
        break;
      }
    }

    if (DevTypeIndex == LegacyBootDevTypeCount && OtherDeviceTypeIndex < BbsTypeTableCount) {
      CurrentIndex = (UINT16)       mBbsTypeTable[OtherDeviceTypeIndex].CurrentIndex;
      TempString   = (STRING_PTR *) mBbsTypeTable[OtherDeviceTypeIndex].StringPtr;
      CopyMem (
        &(TempString[CurrentIndex]),
        &(LegacyBootOrderStringPtr[Index]),
        sizeof(STRING_PTR)
        );
      (mBbsTypeTable[OtherDeviceTypeIndex].CurrentIndex)++;
    }
  }
  return EFI_SUCCESS;
}


/**
  Get boot device order and information for EFI or Legacy type.

  @param[in]  CurrentBootDevFlag         Boot device type (EFI or Legacy)
  @param[in]  BootDevCount               Number of boot device
  @param[in]  AdvBootOrderStringPtr      Pointer to all boot devices information
  @param[out] TargetBootDevCount         Pointer to the number of boot device for target boot type
  @param[out] TargetBootOrder            Double pointer to the boot device order for target boot type
  @param[out] TargetBootOrderStringPtr   Double pointer to the boot devices information for target boot type

  @retval EFI_SUCCESS                    Get boot device order and information successful.
  @retval EFI_OUT_OF_RESOURCES           Allocate pool fail

**/
STATIC
EFI_STATUS
GetNewBootOrder (
  IN  UINT16                                CurrentBootDevFlag,
  IN  UINT16                                BootDevCount,
  IN  STRING_PTR                            *AdvBootOrderStringPtr,
  OUT UINT16                                *TargetBootDevCount,
  OUT UINT16                                **TargetBootOrder,
  OUT STRING_PTR                            **TargetBootOrderStringPtr
  )
{
  UINTN                                     Index;
  UINTN                                     Count;

  if (BootDevCount == 0) {
    *TargetBootDevCount = 0;
    return EFI_SUCCESS;
  }

  *TargetBootOrderStringPtr = AllocateZeroPool (sizeof(STRING_PTR) * BootDevCount);
  if (*TargetBootOrderStringPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  *TargetBootOrder = AllocateZeroPool (sizeof(UINT16) * BootDevCount);
  if (*TargetBootOrder == NULL) {
    gBS->FreePool (*TargetBootOrderStringPtr);
    *TargetBootOrderStringPtr = NULL;
    return EFI_OUT_OF_RESOURCES;
  }
  SetMem (*TargetBootOrderStringPtr, sizeof(STRING_PTR) * BootDevCount, 0xFF);
  SetMem (*TargetBootOrder         , sizeof(UINT16)     * BootDevCount, 0xFF);

  *TargetBootDevCount = 0;
  Count = 0;
  for (Index = 0; Index < BootDevCount; Index++) {
    if (AdvBootOrderStringPtr[Index].EfiBootDevFlag == CurrentBootDevFlag) {
      (*TargetBootOrder)[Count]                         = AdvBootOrderStringPtr[Index].BootOrderIndex;
      (*TargetBootOrderStringPtr)[Count].BootOrderIndex = AdvBootOrderStringPtr[Index].BootOrderIndex;
      (*TargetBootOrderStringPtr)[Count].EfiBootDevFlag = AdvBootOrderStringPtr[Index].EfiBootDevFlag;
      (*TargetBootOrderStringPtr)[Count].pString        = AllocateCopyPool (
                                                            StrSize (AdvBootOrderStringPtr[Index].pString),
                                                            AdvBootOrderStringPtr[Index].pString
                                                            );
      if ((*TargetBootOrderStringPtr)[Count].pString == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      (*TargetBootOrderStringPtr)[Count].BbsEntry       = AdvBootOrderStringPtr[Index].BbsEntry;
      (*TargetBootOrderStringPtr)[Count].DevicePath     = AdvBootOrderStringPtr[Index].DevicePath;
      (*TargetBootDevCount)++;
      Count++;
    }
  }

  return EFI_SUCCESS;
}

/**
 Update legacy boot device order by legacy boot device type order

 @param[in]      LegacyBootDevTypeOrder  Pointer to legacy boot device type order
 @param[in]      LegacyBootDevType       Pointer to legacy boot device type list
 @param[in]      LegacyBootDevTypeCount  Number of device in legacy boot device type list
 @param[in, out] LegacyBootOrder         Pointer to legacy boot device order

 @retval EFI_SUCCESS                     Update legacy boot device order successful
**/
EFI_STATUS
UpdateLegacyBootOrderByTypeOrder (
  IN     UINT8                              *LegacyBootDevTypeOrder,
  IN     UINT16                             *LegacyBootDevType,
  IN     UINT16                             LegacyBootDevTypeCount,
  IN OUT UINT16                             *LegacyBootOrder
  )
{
  UINTN                                     Index;
  UINTN                                     OrderIndex;
  UINTN                                     DevIndex;
  UINTN                                     DevCount;
  UINTN                                     Count;
  STRING_PTR                                *DevTypeStringPtr;

  Count = 0;

  for (OrderIndex = 0; OrderIndex < LegacyBootDevTypeCount; OrderIndex++) {
    for (Index = 0 ; Index < LegacyBootDevTypeCount; Index++) {
      if (mBbsTypeTable[LegacyBootDevType[Index]].DeviceType != LegacyBootDevTypeOrder[OrderIndex]) {
        continue;
      }

      DevTypeStringPtr = (STRING_PTR *) mBbsTypeTable[LegacyBootDevType[Index]].StringPtr;
      DevCount         =                mBbsTypeTable[LegacyBootDevType[Index]].DeviceTypeCount;

      for (DevIndex = 0; DevIndex < DevCount; DevIndex++) {
        LegacyBootOrder[Count++] = DevTypeStringPtr[DevIndex].BootOrderIndex;
      }
      break;
    }
  }

  return EFI_SUCCESS;
}

/**
 Based on kernel configuration, sync boot order, STRING_PTR data and legacy boot order

 @param[in]      KernelConfig            Pointer to kernel configuration data
 @param[in]      AdvBootDevNum           Boot device number
 @param[in, out] AdvBootOrder            Pointer to boot device order
 @param[in, out] AdvBootDevStringPtr     Pointer to STRING_PTR data of boot device
 @param[in]      LegacyBootDevNum        Legacy boot device number
 @param[in, out] LegacyBootOrder         Pointer to legacy boot device order
 @param[in]      EfiBootDevNum           EFI boot device number
 @param[in]      EfiBootOrder            Pointer to EFI boot device order

 @retval EFI_SUCCESS                     Sync boot order, STRING_PTR data and legacy boot order successful
 @retval EFI_OUT_OF_RESOURCES            Allocate pool fail
 @retval Other                           Get setup browser data fail
**/
STATIC
EFI_STATUS
SyncBootOrder (
  IN     KERNEL_CONFIGURATION               *KernelConfig,
  IN     UINTN                              AdvBootDevNum,
  IN OUT UINT16                             *AdvBootOrder,
  IN OUT STRING_PTR                         *AdvBootDevStringPtr,
  IN     UINTN                              LegacyBootDevNum,
  IN OUT UINT16                             *LegacyBootOrder,
  IN     UINTN                              EfiBootDevNum,
  IN     UINT16                             *EfiBootOrder
  )
{
  UINTN                                     Index;
  UINTN                                     DevIndex;
  UINTN                                     Count;
  UINTN                                     FrontBootDevCount;
  UINTN                                     BackBootDevCount;
  UINT16                                    *FrontBootOrder;
  UINT16                                    *BackBootOrder;
  UINT16                                    *TempBootOrder;
  STRING_PTR                                *TempBootDevStringPtr;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_STATUS                                Status;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo           = SuBrowser->SUCInfo;
  FrontBootDevCount = 0;
  BackBootDevCount  = 0;

  if (KernelConfig->BootMenuType == NORMAL_MENU) {
    if (KernelConfig->LegacyNormalMenuType == LEGACY_NORMAL_MENU) {
      UpdateLegacyBootOrderByTypeOrder (
        KernelConfig->BootTypeOrder,
        SUCInfo->LegacyBootDevType,
        SUCInfo->LegacyBootDevTypeCount,
        LegacyBootOrder
        );
    }

    if (KernelConfig->BootNormalPriority == LEGACY_FIRST) {
      FrontBootDevCount = LegacyBootDevNum;
      FrontBootOrder    = LegacyBootOrder;

      BackBootDevCount  = EfiBootDevNum;
      BackBootOrder     = EfiBootOrder;
    } else {
      FrontBootDevCount = EfiBootDevNum;
      FrontBootOrder    = EfiBootOrder;

      BackBootDevCount  = LegacyBootDevNum;
      BackBootOrder     = LegacyBootOrder;
    }
  } else {
    //
    // current BootMenu is Adv BootMenu
    //
    return EFI_SUCCESS;
  }

  if (AdvBootDevNum == 0) {
    return EFI_SUCCESS;
  }

  TempBootDevStringPtr = AllocatePool (sizeof(STRING_PTR) * AdvBootDevNum);
  if (TempBootDevStringPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  TempBootOrder = AllocatePool (sizeof(UINT16) * AdvBootDevNum);
  if (TempBootOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SetMem (TempBootDevStringPtr, sizeof(STRING_PTR) * AdvBootDevNum, 0xFF);
  SetMem (TempBootOrder       , sizeof(UINT16)     * AdvBootDevNum, 0xFF);

  //
  // Merge Legacy and Efi BootOrder into boot order by KernelConfig->BootNormalPriority setting.
  //
  Count = 0;
  for (Index = 0; Index < FrontBootDevCount; Index++) {
    for (DevIndex = 0; DevIndex < AdvBootDevNum; DevIndex++) {
      if (AdvBootDevStringPtr[DevIndex].BootOrderIndex == FrontBootOrder[Index]) {
        TempBootOrder[Count] = AdvBootOrder[DevIndex];
        CopyMem (
          &TempBootDevStringPtr[Count],
          &AdvBootDevStringPtr[DevIndex],
          sizeof (STRING_PTR)
          );
        Count++;
        break;
      }
    }
  }

  for (Index = 0; Index < BackBootDevCount; Index++) {
    for (DevIndex = 0; DevIndex < AdvBootDevNum; DevIndex++) {
      if (AdvBootDevStringPtr[DevIndex].BootOrderIndex == BackBootOrder[Index]) {
        TempBootOrder[Count] = AdvBootOrder[DevIndex];
        CopyMem (
          &TempBootDevStringPtr[Count],
          &AdvBootDevStringPtr[DevIndex],
          sizeof (STRING_PTR)
          );
        Count++;
        break;
      }
    }
  }

  CopyMem (AdvBootOrder       , TempBootOrder       , sizeof (UINT16)     * AdvBootDevNum);
  CopyMem (AdvBootDevStringPtr, TempBootDevStringPtr, sizeof (STRING_PTR) * AdvBootDevNum);

  gBS->FreePool(TempBootOrder);
  gBS->FreePool(TempBootDevStringPtr);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UpdateNewForm (
  IN EFI_HII_HANDLE                            HiiHandle,
  IN UINT8                                     IfrFlag,
  IN STRING_REF                                *LastToken,
  IN STRING_PTR                                *BootString,
  IN UINTN                                     BootDevCount,
  IN UINT16                                    DeviceKeyBase,
  IN UINT16                                    BootDevLabel,
  IN EFI_FORM_ID                               FormId,
  IN VOID                                      *OpCodeHandle
  )
{
  UINTN                                        Index;
  STRING_REF                                   TempLastToken;
  SETUP_UTILITY_BROWSER_DATA                   *SuBrowser;
  EFI_STATUS                                   Status;
  EFI_IFR_GUID_LABEL                           *StartLabel;
  EFI_IFR_ACTION                               *ActionOpCode;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (OpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = BootDevLabel;

  TempLastToken = *LastToken;
  for (Index = 0; Index < BootDevCount; Index++, TempLastToken = 0) {
    TempLastToken = HiiSetString (
                      HiiHandle,
                      0,
                      BootString[Index].pString,
                      NULL
                      );
    if (Index == 0) {
      *LastToken = TempLastToken;
    }

    ActionOpCode = (EFI_IFR_ACTION *) HiiCreateActionOpCode (
                                        OpCodeHandle,
                                        (EFI_QUESTION_ID) (DeviceKeyBase + Index),
                                        TempLastToken,
                                        0,
                                        0,
                                        0
                                        );
    ActionOpCode->Question.Flags = EFI_IFR_CHECKBOX_DEFAULT_MFG;
  }

  //
  //We need remove the same data we update to HII database before.
  //
  HiiUpdateForm (
    HiiHandle,
    NULL,
    FormId,
    OpCodeHandle,
    NULL
    );

  return EFI_SUCCESS;
}

/**
 Update module variable mBbsTypeTable[].DeviceTypeCount

 @param[in] LegacyBootOrderStringPtr Pointer to the legacy boot device information
 @param[in] LegacyBootDeviceNum      Number of legacy boot device
 @param[in] LegacyBootDevTypeCount   Number of legacy boot device type
 @param[in] LegacyBootDevType        Pointer to the legacy boot device type order

 @retval EFI_SUCCESS                 Update module variable successfully
**/
STATIC
EFI_STATUS
InitBbsTypeTableDevTypeCount (
  IN STRING_PTR                             *LegacyBootOrderStringPtr,
  IN UINTN                                  LegacyBootDeviceNum,
  IN UINTN                                  LegacyBootDevTypeCount,
  IN UINT16                                 *LegacyBootDevType
  )
{
  UINTN                                     DeviceTypeIndex;
  UINTN                                     Index;
  UINTN                                     BbsTypeTableCount;
  UINTN                                     OtherDeviceTypeIndex;

  OtherDeviceTypeIndex = GetIndexOfBbsTypeTableByDevType (OTHER_DRIVER);
  BbsTypeTableCount    = mBbsTypeTableCount;

  for (Index = 0; Index < BbsTypeTableCount; Index++) {
    mBbsTypeTable[Index].DeviceTypeCount = 0;
  }

  for (Index = 0; Index < LegacyBootDeviceNum; Index++) {
    if (LegacyBootOrderStringPtr[Index].BbsEntry == NULL) {
      continue;
    }

    for (DeviceTypeIndex = 0; DeviceTypeIndex < LegacyBootDevTypeCount; DeviceTypeIndex++) {
      if (IsUsbDevice (LegacyBootOrderStringPtr[Index].BbsEntry)) {
        if (mBbsTypeTable[LegacyBootDevType[DeviceTypeIndex]].DeviceType == BBS_USB) {
          mBbsTypeTable[LegacyBootDevType[DeviceTypeIndex]].DeviceTypeCount++;
          break;
        }
      } else if (LegacyBootOrderStringPtr[Index].BbsEntry->DeviceType == mBbsTypeTable[LegacyBootDevType[DeviceTypeIndex]].DeviceType) {
        mBbsTypeTable[LegacyBootDevType[DeviceTypeIndex]].DeviceTypeCount++;
        break;
      }
    }

    if (DeviceTypeIndex == LegacyBootDevTypeCount && OtherDeviceTypeIndex < BbsTypeTableCount) {
      mBbsTypeTable[OtherDeviceTypeIndex].DeviceTypeCount++;
    }
  }

  for (Index = 0; Index < BbsTypeTableCount; Index++) {
    if (mBbsTypeTable[Index].DeviceTypeCount > MAX_BOOT_DEVICES_NUMBER) {
      mBbsTypeTable[Index].DeviceTypeCount = MAX_BOOT_DEVICES_NUMBER;
    }
  }

  return EFI_SUCCESS;
}

/**
  Get legacy boot device type order

  @param[out] LegacyBootDevTypeCount   Pointer to the number of legacy boot device type
  @param[out] LegacyBootDevType        Double pointer to the legacy boot device type order

  @retval EFI_SUCCESS              Get legacy boot device type order successfully
  @retval EFI_OUT_OF_RESOURCES     Allocate pool fail
  @retval EFI_INVALID_PARAMETER    There is a undefined boot device type in default legacy boot device type table
**/
EFI_STATUS
GetLegacyBootDevType (
  OUT UINT16                                 *LegacyBootDevTypeCount,
  OUT UINT16                                 **LegacyBootDevType
  )
{
  UINT8                                      *BootTypeOrderSquence;
  UINT16                                     Index;
  UINT16                                     DeviceTypeIndex;
  UINT16                                     TypeNum;

  BootTypeOrderSquence = (UINT8 *) PcdGetPtr (PcdLegacyBootTypeOrder);
  if (BootTypeOrderSquence == NULL || BootTypeOrderSquence[0] == 0) {
    *LegacyBootDevType      = NULL;
    *LegacyBootDevTypeCount = 0;
    return EFI_SUCCESS;
  }

  Index = 0;
  while (BootTypeOrderSquence[Index] != 0) {
    Index++;
  }

  *LegacyBootDevTypeCount = Index;
  *LegacyBootDevType      = AllocateZeroPool (*LegacyBootDevTypeCount * sizeof (UINT16));
  if (*LegacyBootDevType == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0, TypeNum = 0; Index < *LegacyBootDevTypeCount; Index++) {
    for (DeviceTypeIndex = 0; DeviceTypeIndex < MAX_BOOT_ORDER_NUMBER; DeviceTypeIndex++) {
      if (BootTypeOrderSquence[Index] == mBbsTypeTable[DeviceTypeIndex].DeviceType) {
        (*LegacyBootDevType)[TypeNum++] = DeviceTypeIndex;
        break;
      }
    }

    if (DeviceTypeIndex == MAX_BOOT_ORDER_NUMBER)  {
      return EFI_INVALID_PARAMETER;
    }
  }

  return EFI_SUCCESS;
}


/**
  Get all boot devices information and order from BootOrder

  @param[in]  BootOrder            Pointer to BootOrder
  @param[in]  BootDeviceNum        Number of boot devices in BootOrder
  @param[out] BootOrderString      Double pointer to all boot devices information
  @param[out] TempBootOrder        Double pointer to temp boot order

  @retval EFI_SUCCESS              Get all boot devices information successfully
  @retval EFI_OUT_OF_RESOURCES     Allocate pool fail or number of boot device exceed maximum boot device count
**/
STATIC
EFI_STATUS
GetBootDevList (
  IN  UINT16                                *BootOrder,
  IN  UINT16                                BootDeviceNum,
  OUT STRING_PTR                            **BootOrderString,
  OUT UINT16                                **TempBootOrder
  )
{
  UINT16                                    Index;
  UINT16                                    BootOptionName[20];
  UINT8                                     *BootOption;
  UINT8                                     *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  CHAR16                                    *DeviceName;
  UINTN                                     DeviceNameSize;
  UINT16                                    DevPathLen;


  if (BootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  *BootOrderString = AllocateZeroPool (sizeof (STRING_PTR) * BootDeviceNum);
  if (*BootOrderString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *TempBootOrder   = AllocateZeroPool (sizeof (UINT16) * BootDeviceNum);
  if (*TempBootOrder == NULL) {
    gBS->FreePool (*BootOrderString);
    *BootOrderString = NULL;
    return EFI_OUT_OF_RESOURCES;
  }


  Index = 0;
  while (Index < BootDeviceNum) {
    if (Index >= MAX_BOOT_COUNT) {
      return EFI_OUT_OF_RESOURCES;
    }

    UnicodeSPrint (
      BootOptionName,
      sizeof (BootOptionName),
      L"Boot%04x",
      BootOrder[Index]
      );

    BootOption = CommonGetVariableData (
                   BootOptionName,
                   &gEfiGlobalVariableGuid
                   );
    if (BootOption == NULL) {
      gBS->FreePool (BootOrder);
      return EFI_OUT_OF_RESOURCES;
    }

    TempPtr = BootOption;
    TempPtr += sizeof (UINT32);
    DevPathLen = *(UINT16 *) TempPtr;
    TempPtr += sizeof (UINT16);
    DeviceName = (CHAR16 *) TempPtr;
    DeviceNameSize = StrSize (DeviceName);
    TempPtr += DeviceNameSize;
    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    (*TempBootOrder)[Index]                    = BootOrder[Index];
    (*BootOrderString)[Index].BootOrderIndex   = BootOrder[Index];
    (*BootOrderString)[Index].pString          = AllocateCopyPool (DeviceNameSize, DeviceName);
    if ((*BootOrderString)[Index].pString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    (*BootOrderString)[Index].DevicePath       = AllocateCopyPool (DevPathLen, DevicePath);
    if ((*BootOrderString)[Index].DevicePath == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
      (*BootOrderString)[Index].EfiBootDevFlag = LEGACY_BOOT_DEV;
      TempPtr += DevPathLen;
      (*BootOrderString)[Index].BbsEntry       = AllocateCopyPool (sizeof (BBS_TABLE), TempPtr);
      if ((*BootOrderString)[Index].BbsEntry == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
    } else {
      (*BootOrderString)[Index].EfiBootDevFlag = EFI_BOOT_DEV;
      (*BootOrderString)[Index].BbsEntry       = NULL;
    }

    gBS->FreePool (BootOption);
    Index++;
  }

  return EFI_SUCCESS;
}

/**
  To sort the BootOrder by BootTypeOrder

  @param  BbsDevTableTypeCount   The number of legacy boot device type
  @param  BbsTotalCount          The number of Bbs entry
  @param  TempBootType           The pointer of array that SUC support boot device type
  @param  TempBootOrder          The pointer of BootOrder
  @param  BbsAdvCount            The number of BootOption


  @retval EFI_SUCCESS            Success sort the BootOrder

**/
STATIC
EFI_STATUS
SortBbsTypeTable (
  UINTN                                     BbsDevTableTypeCount,
  UINT16                                    BbsTotalCount,
  UINT16                                    *TempBootType,
  UINT16                                    *TempBootOrder,
  UINT16                                    BbsAdvCount
  )
{
  UINT16                                     Index;
  UINT16                                     AdvDeviceNum;
  UINT16                                     DeviceTypeIndex;
  UINT16                                     CurrentIndex;
  STRING_PTR                                 *WorkBbsString;
  STRING_PTR                                 *BbsString;

  if (BbsTotalCount == 0) {
    return EFI_SUCCESS;
  }

  WorkBbsString = AllocateZeroPool (sizeof (STRING_PTR) * BbsTotalCount);
  if (WorkBbsString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
    mBbsTypeTable[TempBootType[Index]].CurrentIndex = 0;
    SetMem (WorkBbsString, (sizeof (STRING_PTR) * BbsTotalCount), 0xff);
    BbsString = (STRING_PTR *) mBbsTypeTable[TempBootType[Index]].StringPtr;
    for (AdvDeviceNum = 0; AdvDeviceNum < BbsAdvCount; AdvDeviceNum++) {
      for (DeviceTypeIndex = 0; DeviceTypeIndex < mBbsTypeTable[TempBootType[Index]].DeviceTypeCount; DeviceTypeIndex++) {
        if ((BbsString[DeviceTypeIndex].BootOrderIndex == TempBootOrder[AdvDeviceNum]) &&
            (TempBootOrder[AdvDeviceNum] != 0xff)) {
          ASSERT (mBbsTypeTable[TempBootType[Index]].CurrentIndex < BbsTotalCount);
          CurrentIndex = mBbsTypeTable[TempBootType[Index]].CurrentIndex;
          WorkBbsString[CurrentIndex].BootOrderIndex = BbsString[DeviceTypeIndex].BootOrderIndex;
          WorkBbsString[CurrentIndex].pString        = BbsString[DeviceTypeIndex].pString;
          mBbsTypeTable[TempBootType[Index]].CurrentIndex++;
          break;
        }
      }
    }
    mBbsTypeTable[TempBootType[Index]].DeviceTypeCount = mBbsTypeTable[TempBootType[Index]].CurrentIndex;
    CopyMem (
      BbsString,
      WorkBbsString,
      sizeof(STRING_PTR) * mBbsTypeTable[TempBootType[Index]].DeviceTypeCount
      );
  }

  gBS->FreePool (WorkBbsString);

  return EFI_SUCCESS;
}

/**
  Check the specific BBS Table entry is USB device

  @param  CurrentBbsTable        Pointer to current BBS table start address

  @retval TRUE                   It is USB device
  @retval FALSE                  It isn't USB device

**/
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
  Check the Bootxxxx number of device is EFI device or legacy device.

  @param  NumberOfDevice         The device number in BootOrder variable (ex: Boot0001, then NumberOfDevice is 1)

  @retval TRUE                   The device is EFI device.
  @retval FALSE                  The device is legacy device.

**/
BOOLEAN
IsEfiDevice (
  IN     UINT16    NumberOfDevice
  )
{
  UINT16                    BootOptionName[20];
  UINT8                     *BootOption;
  UINT8                     *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   IsEfiDevice;

  UnicodeSPrint (
    BootOptionName,
    sizeof (BootOptionName),
    L"Boot%04x",
    NumberOfDevice
    );

  BootOption = CommonGetVariableData (
                 BootOptionName,
                 &gEfiGlobalVariableGuid
                 );
  ASSERT (BootOption != NULL);
  if (BootOption == NULL) {
    return FALSE;
  }

  //
  // Find device path in Bootxxxx variable
  //
  WorkingPtr = BootOption;
  WorkingPtr += sizeof (UINT32);
  WorkingPtr += sizeof (UINT16);
  WorkingPtr += (UINTN) StrSize ((UINT16 *) WorkingPtr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;

  IsEfiDevice = TRUE;
  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    IsEfiDevice = FALSE;
  }

  return IsEfiDevice;
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
  According to input parameter to adjust boot order to EFI device first or legacy device first.

  @param  EfiDeviceFirst         TRUE : Indicate EFI device first.
                                 FALSE: Indicate legacy device first.
  @param  DeviceCount            Total device count
  @param  BootOrder              Pointer to BootOrder.

  @retval EFI_SUCCESS            Adjust boot order successful.
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.

**/
EFI_STATUS
AdjustBootOrder (
  IN      BOOLEAN      EfiDeviceFirst,
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
{
  UINTN               BootOrderIndex;
  UINT16              *EfiDevicOrder;
  UINTN               EfiDeviceIndex;
  UINT16              *LegacyDeviceOrder;
  UINTN               LegacyDeviceIndex;

  if (DeviceCount == 0 || BootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiDevicOrder     = AllocateZeroPool (DeviceCount * sizeof (CHAR16));
  if (EfiDevicOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  LegacyDeviceOrder = AllocateZeroPool (DeviceCount * sizeof (CHAR16));
  if (LegacyDeviceOrder == NULL) {
    gBS->FreePool (EfiDevicOrder);
    return EFI_OUT_OF_RESOURCES;
  }
  EfiDeviceIndex    = 0;
  LegacyDeviceIndex = 0;

  //
  // According boot type (EFI or legacy) to put boot order respective buffer
  //
  for (BootOrderIndex = 0; BootOrderIndex < DeviceCount; BootOrderIndex++) {
    if (IsEfiDevice (BootOrder[BootOrderIndex])) {
      EfiDevicOrder[EfiDeviceIndex++] = BootOrder[BootOrderIndex];
    } else {
      LegacyDeviceOrder[LegacyDeviceIndex++] = BootOrder[BootOrderIndex];
    }
  }

  //
  // Adjust boot order depend on EFI device first or legacy device first
  //
  if (EfiDeviceFirst) {
    CopyMem (BootOrder, EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
    CopyMem (&BootOrder[EfiDeviceIndex], LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
  } else {
    CopyMem (BootOrder, LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
    CopyMem (&BootOrder[LegacyDeviceIndex], EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
  }

  gBS->FreePool (EfiDevicOrder);
  gBS->FreePool (LegacyDeviceOrder);
  return EFI_SUCCESS;
}

/**
  According to EFI device priority to sort all of EFI devices.

  @param  DeviceCount            Total EFI device count
  @param  BootOrder              Pointer to EFI device BootOrder.

  @retval EFI_SUCCESS            Change to auto boot order policy successful.
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.
  @retval EFI_UNSUPPORTED        doesn't support auto EFI Boot option oreder algorithm

**/
EFI_STATUS
SortEfiDevices (
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
{
  UINTN                               Index;
  UINTN                               SortIndex;
  UINT16                              BootOptionName[20];
  UINTN                               BootOptionSize;
  UINT8                               *BootOption;
  UINT8                               *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_BOOT_ORDER_PRIORITY             *BootPriority;
  UINT16                              Size;
  EFI_STATUS                          Status;
  UINT16                              BootNumber;
  UINT8                               BootPriorityLevel;
  EFI_BOOT_OPTION_POLICY_PROTOCOL     *BootOptionPolicy;
  BOOLEAN                             CreateByOS;

  Status = gBS->LocateProtocol (
                &gEfiBootOptionPolicyProtocolGuid,
                NULL,
                (VOID **)  &BootOptionPolicy
                );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  BootPriority = AllocateZeroPool (DeviceCount * sizeof (EFI_BOOT_ORDER_PRIORITY));
  if (BootPriority == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; Index < DeviceCount; Index++) {
    UnicodeSPrint (
      BootOptionName,
      sizeof (BootOptionName),
      L"Boot%04x",
      BootOrder[Index]
      );
    BootOption     = NULL;
    BootOptionSize = 0;
    Status = CommonGetVariableDataAndSize (
               BootOptionName,
               &gEfiGlobalVariableGuid,
               &BootOptionSize,
               (VOID **) &BootOption
               );
    ASSERT (BootOption != NULL);
    if (BootOption == NULL) {
      return EFI_NOT_FOUND;
    }

    //
    // Find device path in Bootxxxx variable
    //
    Size = 0;
    WorkingPtr = BootOption;
    WorkingPtr += sizeof (UINT32) ;
    Size += sizeof (UINT32) + (UINT16) (*WorkingPtr);
    WorkingPtr += sizeof (UINT16) ;
    Size += sizeof (UINT16) + (UINT16) StrSize ((UINT16 *) WorkingPtr);
    WorkingPtr += StrSize ((UINT16 *) WorkingPtr);
    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
    WorkingPtr += GetDevicePathSize (DevicePath);
    CreateByOS = TRUE;

    if (((BootOptionSize - Size == 2) && (AsciiStrnCmp ((CHAR8 *) WorkingPtr, "RC", 2) == 0)) ||
        (IsEfiShellBootOption (DevicePath) && (BootOptionSize - Size >= 2) && (AsciiStrnCmp ((CHAR8 *) WorkingPtr, "RC", 2) == 0))) {
      CreateByOS = FALSE;
    }
    Status = BootOptionPolicy->GetEfiOptionPriority (
                                 BootOptionPolicy,
                                 CreateByOS,
                                 DevicePath,
                                 &BootPriority[Index]
                                 );
    ASSERT_EFI_ERROR (Status);
  }
  //
  // Sort EFI boot options based on EFI boot option priority policy
  //
  for (Index = 0; Index < DeviceCount; Index++) {
    for (SortIndex = Index + 1; SortIndex < DeviceCount; SortIndex++) {
      if (BootPriority[Index] > BootPriority[SortIndex]) {
        BootPriorityLevel = BootPriority[Index];
        BootPriority[Index] = BootPriority[SortIndex];
        BootPriority[SortIndex] = BootPriorityLevel;
        BootNumber = BootOrder[Index];
        BootOrder[Index] = BootOrder[SortIndex];
        BootOrder[SortIndex] = BootNumber;
      }
    }
  }

  gBS->FreePool (BootPriority);

  return EFI_SUCCESS;
}

/**
  Change boot order to suit auto boot option order policy.

  @param  DeviceCount            Total device count
  @param  BootOrder              Pointer to BootOrder.

  @retval EFI_SUCCESS            Change to auto boot order policy successful.
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.

**/
EFI_STATUS
ChangeToAutoBootOrder (
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
{
  UINTN            EfiDeviceNum;
  UINTN            EfiStartIndex;
  UINT16           *EfiDeviceOrder;
  UINTN            Index;

  if (DeviceCount == 0 || BootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiDeviceNum = 0;
  EfiStartIndex = 0;
  for (Index = 0; Index < DeviceCount; Index++) {
    if (IsEfiDevice (BootOrder[Index])) {
      if (EfiDeviceNum == 0) {
        EfiStartIndex = Index;
      }
      EfiDeviceNum++;
    }
  }

  if (EfiDeviceNum == 0) {
    return EFI_SUCCESS;
  }
  EfiDeviceOrder = AllocateZeroPool (EfiDeviceNum * sizeof (CHAR16));
  if (EfiDeviceOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (EfiDeviceOrder, &BootOrder[EfiStartIndex], EfiDeviceNum * sizeof (UINT16));
  //
  // According to EFI device priority to sort EFI devices
  //
  SortEfiDevices (EfiDeviceNum, EfiDeviceOrder);

  //
  // copy sorted EFI device order to boot order
  //
  CopyMem (&BootOrder[EfiStartIndex], EfiDeviceOrder, EfiDeviceNum * sizeof (UINT16));

  gBS->FreePool (EfiDeviceOrder);

  return EFI_SUCCESS;
}


/**
 Get legacy boot device type order information

 @param[in]  HiiHandle               HII hanlde for HII get string function
 @param[in]  KernelConfig            Pointer to kernel configuration
 @param[in]  LegacyBootDevType       Pointer to legacy boot device type list
 @param[in]  LegacyBootDevTypeCount  Number of device in legacy boot device type list
 @param[out] BootTypeOrderStringPtr  Double pointer to legacy boot device type information

 @retval EFI_SUCCESS                 Get legacy boot device type order information successful.
 @retval EFI_OUT_OF_RESOURCES        Allocate pool fail
**/
EFI_STATUS
GetLegacyBootDevTypeOrderInfo (
  IN  EFI_HII_HANDLE                        HiiHandle,
  IN  KERNEL_CONFIGURATION                  *KernelConfig,
  IN  UINT16                                *LegacyBootDevType,
  IN  UINT16                                LegacyBootDevTypeCount,
  OUT STRING_PTR                            **BootTypeOrderStringPtr
  )
{
  UINTN                                     Index;
  UINTN                                     DevTypeIndex;


  *BootTypeOrderStringPtr = AllocateZeroPool (sizeof (STRING_PTR) * MAX_BOOT_ORDER_NUMBER);
  if (*BootTypeOrderStringPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    for (DevTypeIndex = 0; DevTypeIndex < LegacyBootDevTypeCount; DevTypeIndex++) {
      if (KernelConfig->BootTypeOrder[Index] == mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].DeviceType) {
        (*BootTypeOrderStringPtr)[Index].BootOrderIndex = mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].DeviceType;
        (*BootTypeOrderStringPtr)[Index].pString        = HiiGetString (HiiHandle, mBbsTypeTable[LegacyBootDevType[DevTypeIndex]].StrToken, NULL);
        break;
      }
    }
    if (DevTypeIndex == LegacyBootDevTypeCount) {
      (*BootTypeOrderStringPtr)[Index].BootOrderIndex = 0xFFFF;  //0xFFFF means the boot device is unknown type
    }
  }

  return EFI_SUCCESS;
}

