//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   BootMenu.c
//;
//; Abstract:
//;
//;   Boot menu relative functions
//;

#include "BootMenu.h"
#include EFI_PROTOCOL_DEFINITION (ComponentName)
#if (EFI_SPECIFICATION_VERSION >= 0x00020000 && defined(EFI_COMPONENT_NAME2_PROTOCOL_GUID))
#include EFI_PROTOCOL_DEFINITION (ComponentName2)
#endif
#include EFI_PROTOCOL_DEFINITION (LegacyBios)
#include EFI_PROTOCOL_DEFINITION (UsbIo)
#include EFI_PROTOCOL_DEFINITION (BlockIo)
#include EFI_PROTOCOL_DEFINITION (DiskInfo)
#include EFI_PROTOCOL_CONSUMER (BootOptionPolicy)
#include EFI_GUID_DEFINITION (EfiShell)

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
GetDevTypeCount (
  IN STRING_PTR                             *AdvBootDevString,
  IN UINTN                                  AdvDevCount,
  IN UINTN                                  BbsDevTableTypeCount,
  IN UINT16                                 *SupportBootDevType
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
  IN EFI_HII_UPDATE_DATA                       *UpdateDevData
);

STATIC
EFI_STATUS
GetBootDevList (
  IN     UINT16                             *BootOrder,
  IN OUT STRING_PTR                         **BootOrderString,
  IN OUT UINT16                             **TempBootOrder,
  IN     UINT16                             BootDeviceNum
  );

STATIC
EFI_STATUS
GetOrderByType (
  IN     UINTN                              LegacyAdvBootDeviceNum,
  IN     UINTN                              BbsDevTableTypeCount,
  IN     UINT16                             *SupportBootType,
  IN OUT UINT16                             *LegacyAdvBootOrder,
  IN OUT STRING_PTR                         *LegacyBootOrderString
);

STATIC
EFI_STATUS
SyncBootOrder (
  IN SYSTEM_CONFIGURATION                   *SetupVariable,
  IN UINTN                                  AdvBootDeviceNum,
  IN UINT16                                 *AdvBootOrder,
  IN STRING_PTR                             *AdvBootString,
  IN UINTN                                  LegacyBootDeviceNum,
  IN UINT16                                 *LegacyAdvBootOrder,
  IN STRING_PTR                             *LegacyAdvBootString,
  IN UINTN                                  EfiBootDeviceNum,
  IN UINT16                                 *EfiBootOrder,
  IN STRING_PTR                             *EfiBootString
);

STATIC
EFI_STATUS
GetNewBootOrder (
  IN     UINT16                             CurrentBootDevFlag,
  IN     UINT16                             BootDevCount,
  IN     STRING_PTR                         *AdvBootOrderString,
  IN OUT UINT16                             *CurrentBootCount,
  IN OUT UINT16                             **TempBootOrder,
  IN OUT STRING_PTR                         **TempBootOrderString
);

STATIC
BOOLEAN
IsUsbDevice (
  IN BBS_TABLE              *CurrentBbsTable
  );

EFI_STATUS
GetBbsName (
  IN     EFI_HII_HANDLE                     HiiHandle,
  IN OUT SYSTEM_CONFIGURATION               *SetupVariable,
  OUT    STRING_PTR                         **BootTypeOrderString,
  OUT    STRING_PTR                         **AdvBootOrderString,
  OUT    STRING_PTR                         **LegacyAdvString,
  OUT    STRING_PTR                         **EfiBootOrderString
  )
{
  UINTN                                     Index;
  UINTN                                     DeviceTypeIndex;
  EFI_STATUS                                Status;
  UINT16                                    *BootOrder;
  UINT16                                    *EfiBootOrder;
  UINT16                                    *TempBootOrder;
  UINT16                                    *LegacyAdvBootOrder;
  UINT16                                    BbsDevTableTypeCount;
  UINT16                                    *LegacyBootDevType;
  UINT16                                    *AdvBootDeviceNum;
  UINT16                                    *LegacyBootDeviceNum;
  UINT16                                    *EfiBootDeviceNum;
  UINT16                                    DeviceCount;
  UINTN                                     NewStringLength;
  UINT16                                    CurrentBootDevFlag;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo             = SuBrowser->SUCInfo;
  BootOrder           = SUCInfo->BootOrder;
  DeviceCount         = SUCInfo->AdvBootDeviceNum;
  AdvBootDeviceNum    = &(SUCInfo->AdvBootDeviceNum);
  LegacyBootDeviceNum = &(SUCInfo->LegacyBootDeviceNum);
  EfiBootDeviceNum    = &(SUCInfo->EfiBootDeviceNum);

  BbsDevTableTypeCount = SUCInfo->LegacyBootDevTypeCount;
  LegacyBootDevType    = SUCInfo->LegacyBootDevType;

  //
  //get Adv boot option order and string
  //
  GetBootDevList (
    BootOrder,
    AdvBootOrderString,
    &TempBootOrder,
    *AdvBootDeviceNum
    );

  if (*AdvBootDeviceNum > DeviceCount) {
    *AdvBootDeviceNum = DeviceCount;
  }

  //
  // Sort Boot order (sort the order of boot device type)
  //
  *BootTypeOrderString = EfiLibAllocateZeroPool (sizeof (STRING_PTR) * MAX_BOOT_ORDER_NUMBER);
  for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    for (DeviceTypeIndex = 0; DeviceTypeIndex < BbsDevTableTypeCount; DeviceTypeIndex++) {
      if (SetupVariable->BootTypeOrder[Index] == mBbsTypeTable[LegacyBootDevType[DeviceTypeIndex]].DeviceType) {
      	NewStringLength = MAX_STRING_SIZE;
      	(*BootTypeOrderString)[Index].pString        = EfiLibAllocateZeroPool (NewStringLength);
        (*BootTypeOrderString)[Index].BootOrderIndex = mBbsTypeTable[LegacyBootDevType[DeviceTypeIndex]].DeviceType;
        Status = GetStringFromHandle (
                   HiiHandle,
                   mBbsTypeTable[LegacyBootDevType[DeviceTypeIndex]].StrToken,
                   &(*BootTypeOrderString)[Index].pString
                   );
        break;
      }
    }
    if (DeviceTypeIndex == BbsDevTableTypeCount) {
      (*BootTypeOrderString)[Index].BootOrderIndex = 0xffff;  //oxff means the boot device is unknown type
    }
  }

  //
  // Sort boot sequence of each device type
  //
  CurrentBootDevFlag = LEGACY_BOOT_DEV;

  GetNewBootOrder (
    CurrentBootDevFlag,
    *AdvBootDeviceNum,
    *AdvBootOrderString,
    LegacyBootDeviceNum,
    &LegacyAdvBootOrder,
    LegacyAdvString
    );

  //
  //  build mBbsTypeTable
  //
  GetOrderByType (
    *LegacyBootDeviceNum,
    BbsDevTableTypeCount,
    LegacyBootDevType,
    LegacyAdvBootOrder,
    *LegacyAdvString
    );
  //
  // get Efi BootOrder string
  //
  CurrentBootDevFlag = EFI_BOOT_DEV;
  GetNewBootOrder (
    CurrentBootDevFlag,
    *AdvBootDeviceNum,
    *AdvBootOrderString,
    EfiBootDeviceNum,
    &EfiBootOrder,
    EfiBootOrderString
    );
  //
  // accord to BootMenu type flag, set AdvBootOrder as relative BootOrder
  //
  SyncBootOrder (
    SetupVariable,
    *AdvBootDeviceNum,
    TempBootOrder,
    *AdvBootOrderString,
    *LegacyBootDeviceNum,
    LegacyAdvBootOrder,
    *LegacyAdvString,
    (UINT16) *EfiBootDeviceNum,
    EfiBootOrder,
    *EfiBootOrderString
    );

  EfiCopyMem (
    BootOrder,
    TempBootOrder,
    sizeof(UINT16) * (*AdvBootDeviceNum)
    );

  gBS->FreePool (TempBootOrder);
  gBS->FreePool (EfiBootOrder);
  gBS->FreePool (LegacyAdvBootOrder);
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

EFI_STATUS
GetAllHwBootDeviceInfo (
  OUT UINTN                          *HwBootDeviceInfoCount,
  OUT HARDWARE_BOOT_DEVICE_INFO      *HwBootDeviceInfo
  )
/*++

Routine Description:

  Get all hardware boot device information (BlockIo device path and device name).

Arguments:

  HwBootDeviceInfoCount  - Number of hardware boot device
  HwBootDeviceInfo       - Array pointer of hardware boot device information

Returns:

  EFI_SUCCESS            - Get information successfully
  Otherwise              - Invalid input parameter.
                           Locate protocol fail or get usb device information fail

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
          SetupUtilityLibUpdateAtaString (
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
UnpackDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevPath
  )
/*++

Routine Description:

  Function unpacks a device path data structure so that all the nodes
  of a device path are naturally aligned.

Arguments:

  DevPath        - A pointer to a device path data structure

Returns:

  If the memory for the device path is successfully allocated, then a
  pointer to the new device path is returned.  Otherwise, NULL is returned.

--*/
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
  NewPath = EfiLibAllocateZeroPool (Size);
  if (NewPath) {

    ASSERT (((UINTN) NewPath) % MIN_ALIGNMENT_SIZE == 0);

    //
    // Copy each node
    //
    Src   = DevPath;
    Dest  = NewPath;
    for (;;) {
      Size = DevicePathNodeLength (Src);
      EfiCopyMem (Dest, Src, Size);
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

BOOLEAN
MatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL   *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
/*++

Routine Description:
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

Arguments:
  BlockIoDevicePath     - Multi device path instances which need to check
  HardDriveDevicePath - A device path which starts with a hard drive media device path.

Returns:
  TRUE  - There is a matched device path instance
  FALSE -There is no matched device path instance


--*/
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
      Match = EfiCompareGuid ((EFI_GUID *)TmpHdPath->Signature, (EFI_GUID *)TempPath->Signature);
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
CompareBlockIoDevicePath (
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

EFI_STATUS
BootOptionStrAppendDeviceName (
  IN     UINT16                         EfiBootDeviceNum,
  IN OUT STRING_PTR                     *UpdateEfiBootString
  )
/*++

Routine Description:

  Append device name to boot option description.

Arguments:

  EfiBootDeviceNum      - Number of EFI boot option
  UpdateEfiBootString   - Array pointer of STRING_PTR which store each boot option information

Returns:

  EFI_SUCCESS           - Append device name successfully
  EFI_INVALID_PARAMETER - Invalid input parameter

--*/
{
  UINTN                              Index;
  UINTN                              Index2;
  HARDWARE_BOOT_DEVICE_INFO          HwBootDeviceInfo[MAX_BOOT_DEVICES_NUMBER];
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

  GetAllHwBootDeviceInfo (&HwBootDeviceCount, HwBootDeviceInfo);

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
      VarData = SetupUtilityLibGetVariableAndSize (
                  L"WindowsToGo",
                  &gEfiGenericVariableGuid,
                  &Size
                  );
      if (VarData != NULL) {
        Ptr         = VarData;
        DevicePath  = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
        Ptr        += EfiDevicePathSize (DevicePath);
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
        Size = EfiStrSize (UpdateEfiBootString[Index].pString) + EfiStrSize (L" (") + EfiStrSize (HwDeviceName) + EfiStrSize (L",...") + EfiStrSize (L")");
      } else {
        Size = EfiStrSize (UpdateEfiBootString[Index].pString) + EfiStrSize (L" (") + EfiStrSize (HwDeviceName) + EfiStrSize (L")");
      }
      String = EfiLibAllocateZeroPool (Size);
      if (String != NULL) {
        EfiStrCat (String, UpdateEfiBootString[Index].pString);
        EfiStrCat (String, L" (");
        EfiStrCat (String, HwDeviceName);
        if (DeviceCount > 1) {
          EfiStrCat (String, L",...");
        }
        EfiStrCat (String, L")");

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
  }

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateBootMenu (
  IN     EFI_HII_HANDLE                         BootHiiHandle,
  IN OUT SYSTEM_CONFIGURATION                   *SetupVariable
  )
/*++

Routine Description:

  Update all of the boot devices in platform to boot page

Arguments:

  BootHiiHandle   - Hii hanlde for Boot page
  SetupVariable   - Pointer to SYSTEM_CONFIGURATION instance

Returns:

  EFI_SUCCESS    - Update boot menu successful
  Other          - Some error occured in update boot menu

--*/
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_HII_HANDLE                        HiiHandle;
  STRING_REF                            LastToken;
  UINT8                                 IfrFlag;
  EFI_HII_UPDATE_DATA                   *UpdateBootTypeOrderData;
  EFI_HII_UPDATE_DATA                   *UpdateAdvData;
  EFI_HII_UPDATE_DATA                   *UpdateAdvLegacyData;
  EFI_HII_UPDATE_DATA                   *UpdateEfiData;
  EFI_HII_UPDATE_DATA                   *UpdateBootDeviceData[MAX_BOOT_ORDER_NUMBER];
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
  UINT8                                 *LocalBuffer;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Initial the flag of showing from.
  //
  SetupVariable->NoAdvBootDev    = 0;
  SetupVariable->NoBootTypeOrder = 0;
  SetupVariable->NoLegacyBootDev = 0;
  SetupVariable->NoEfiBootDev    = 0;

  for(Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
#if MAX_BOOT_ORDER_NUMBER == 16
      mBootConfig.NoBootDevs[Index] = 0;
#else
      SetupVariable->NoBootDevs[Index] = 0;
#endif
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
             SetupVariable,
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
  UpdateBootTypeOrderData = EfiLibAllocateZeroPool (sizeof (EFI_HII_UPDATE_DATA));
  UpdateAdvData           = EfiLibAllocateZeroPool (sizeof (EFI_HII_UPDATE_DATA));
  UpdateAdvLegacyData     = EfiLibAllocateZeroPool (sizeof (EFI_HII_UPDATE_DATA));
  UpdateEfiData           = EfiLibAllocateZeroPool (sizeof (EFI_HII_UPDATE_DATA));
  IfrLibInitUpdateData (UpdateBootTypeOrderData, 0x1000);
  IfrLibInitUpdateData (UpdateAdvData, 0x1000);
  IfrLibInitUpdateData (UpdateAdvLegacyData, 0x1000);
  IfrLibInitUpdateData (UpdateEfiData, 0x1000);

  for (Index = 0; Index < LegacyBootTypeOrderCount; Index++) {
    UpdateBootDeviceData[TempBootType[Index]] = EfiLibAllocateZeroPool (sizeof (EFI_HII_UPDATE_DATA));
    IfrLibInitUpdateData (UpdateBootDeviceData[TempBootType[Index]], 0x1000);
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
    SetupVariable->NoBootTypeOrder = 1;
  }
  for (Index = 0; Index < (UINT16) LegacyBootTypeOrderCount; Index++, LastToken = 0) {
    IfrLibNewString (HiiHandle, &LastToken, UpdateBootTypeOrderString[Index].pString);
    if (Index == 0) {
      SUCInfo->BootTypeTokenRecord = LastToken;
    }
    LocalBuffer = (UINT8 *) UpdateBootTypeOrderData->Data + UpdateBootTypeOrderData->Offset;
    CreateActionOpCode (
      (EFI_QUESTION_ID) (KEY_BOOT_TYPE_ORDER_BASE + Index),
      LastToken,
      0,
      0,
      0,
      UpdateBootTypeOrderData
      );
    ((EFI_IFR_ACTION *) LocalBuffer)->Question.Flags = EFI_IFR_CHECKBOX_DEFAULT_MFG;
  }

  //
  //We need remove the same data we update to HII database before.
  //
  IfrLibUpdateForm (HiiHandle, NULL, BOOT_DEVICE_LEG_NOR_BOOT_ID, BOOT_ORDER_LABEL, TRUE, UpdateBootTypeOrderData);

  //
  // update Legacy adv boot form
  //
  LastToken = 0;

  if (LegacyBootDeviceNum != 0) {
    SetupVariable->NoLegacyBootDev = 1;
  }

  if (EfiBootDeviceNum != 0) {
    SetupVariable->NoEfiBootDev = 1;
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
    UpdateAdvLegacyData
    );
  SUCInfo->LegacyAdvanceTokenRecord = LastToken;
  //
  // update boot device form by type
  //
  for (Index = 0; Index < LegacyBootTypeOrderCount; Index++) {
    LastToken = 0;

    if (mBbsTypeTable[TempBootType[Index]].DeviceTypeCount != 0) {
#if MAX_BOOT_ORDER_NUMBER == 16
        mBootConfig.NoBootDevs[TempBootType[Index]]= 1;
#else
        SetupVariable->NoBootDevs[TempBootType[Index]]= 1;
#endif
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
      UpdateBootDeviceData[TempBootType[Index]]
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
    UpdateEfiData
    );
  SUCInfo->EfiTokenRecord = LastToken;
  for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    if (UpdateBootTypeOrderString[Index].pString != NULL) {
      gBS->FreePool (UpdateBootTypeOrderString[Index].pString);
    }
  }
  if (UpdateBootTypeOrderString != NULL) {
    gBS->FreePool (UpdateBootTypeOrderString);
  }

  IfrLibFreeUpdateData (UpdateBootTypeOrderData);
  IfrLibFreeUpdateData (UpdateAdvData);
  IfrLibFreeUpdateData (UpdateAdvLegacyData);
  IfrLibFreeUpdateData (UpdateEfiData);
  gBS->FreePool (UpdateBootTypeOrderData);
  gBS->FreePool (UpdateAdvData);
  gBS->FreePool (UpdateAdvLegacyData);
  gBS->FreePool (UpdateEfiData);
  for (Index = 0; Index < SuBrowser->SUCInfo->AdvBootDeviceNum; Index++) {
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
  if (UpdateAdvBootString != NULL) {
    gBS->FreePool (UpdateAdvBootString);
  }

  for (Index = 0; Index < LegacyBootTypeOrderCount; Index++) {
     IfrLibFreeUpdateData (UpdateBootDeviceData[TempBootType[Index]]);
     gBS->FreePool (UpdateBootDeviceData[TempBootType[Index]]);
  }
  gBS->FreePool (UpdateLegacyAdvBootString);
  for (Index = 0; Index < EfiBootDeviceNum; Index++) {
    if (UpdateEfiBootString[Index].pString != NULL) {
      gBS->FreePool (UpdateEfiBootString[Index].pString);
    }
  }
  gBS->FreePool (UpdateEfiBootString);
  return EFI_SUCCESS;
}

VOID
MoveBootType (
  IN     BOOLEAN   UpShift,
  IN     UINT8     UpperType,
  IN     UINT8     LowerType,
  IN OUT UINT8     *BootTypeOrder,
  IN     UINTN     TypeCount
  )
/*++

Routine Description:

  Move one boot type in the BootTypeOrder.

Arguments:

  UpShift       - TRUE : move LowerType upon UpperType
                  FALSE: move UpperType below LowerType
  UpperType     - Upper boot type
  LowerType     - Lower boot type
  BootTypeOrder - Pointer to boot type order
  TypeCount     - Boot type count

Returns:

  None

--*/
{
  UINT8       *OldBootTypeOrder;
  UINTN       OldBootTypeIndex;
  UINTN       BootTypeIndex;

  OldBootTypeOrder = EfiLibAllocateZeroPool (sizeof (BootTypeOrder));
  EfiCopyMem (
    OldBootTypeOrder,
    BootTypeOrder,
    sizeof (BootTypeOrder)
    );

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
  IN OUT SYSTEM_CONFIGURATION                   *SetupVariable
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

  BootCallBackInfo     = EFI_BOOT_CALLBACK_INFO_FROM_THIS (This);
  HiiHandle            = BootCallBackInfo->HiiHandle;
  SUCInfo              = SuBrowser->SUCInfo;
  BbsDevTableTypeCount = SUCInfo->LegacyBootDevTypeCount;
  TempBootType         = SUCInfo->LegacyBootDevType;
  BootMenuType         = 0;

  for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    DeviceNumBase[Index]       = 0;
    DeviceNumBaseByType[Index] = 0;
    BootTypeOrderBase[Index]   = 0;
  }


  if (SetupVariable->BootMenuType == 0 && SetupVariable->LegacyNormalMenuType == 0) {
    for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
      for (Index1 = 0; Index1 < BbsDevTableTypeCount; Index1++) {
        if (SetupVariable->BootTypeOrder[Index] == mBbsTypeTable[TempBootType[Index1]].DeviceType) {
          DeviceNumBase[Index] = mBbsTypeTable[TempBootType[Index1]].DeviceTypeCount;
          break;
        }
      }
      if (Index1 == BbsDevTableTypeCount) {
        return EFI_INVALID_PARAMETER;
      }
      if (Index == 0) {
        if (SetupVariable->BootNormalPriority == EFI_FIRST) {
          DeviceNumBaseByType[Index] = DeviceNum + SUCInfo->EfiBootDeviceNum;
        } else {
          DeviceNumBaseByType[Index] = DeviceNum;
        }
      } else {
      	TempDeviceNumBase = 0;
      	for (Index2 = 0; Index2 < Index; Index2++) {
          TempDeviceNumBase = TempDeviceNumBase + DeviceNumBase[Index2];
        }
        if (SetupVariable->BootNormalPriority == EFI_FIRST) {
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
    if (SetupVariable->BootNormalPriority == EFI_FIRST) {
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
    if (SetupVariable->BootNormalPriority == LEGACY_FIRST) {
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
  for (Index; Index < BbsDevTableTypeCount; Index++) {
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
      Temp     = SetupVariable->BootTypeOrder[DeviceNum - 1];
      SetupVariable->BootTypeOrder[DeviceNum - 1] = SetupVariable->BootTypeOrder[DeviceNum];
      SetupVariable->BootTypeOrder[DeviceNum]     = (UINT8) Temp;
    }
  } else {
    SecondToken = (STRING_REF)(FirstToken + 1);
    if (DeviceBaseAddress != 0xff) {
      Temp    = TempBootOrder[DeviceBaseAddress + 1];
      TempBootOrder[DeviceBaseAddress + 1] = TempBootOrder[DeviceBaseAddress];
      TempBootOrder[DeviceBaseAddress]     = Temp;
    } else {
      Temp = SetupVariable->BootTypeOrder[DeviceNum + 1];
      SetupVariable->BootTypeOrder[DeviceNum + 1] = SetupVariable->BootTypeOrder[DeviceNum];
      SetupVariable->BootTypeOrder[DeviceNum]     = (UINT8) Temp;
    }
  }
  GetStringFromHandle (
    HiiHandle,
    SecondToken,
    &SecondUpdateStr
    );

  GetStringFromHandle (
    HiiHandle,
    FirstToken,
    &FirstUpdateStr
    );
  IfrLibSetString (HiiHandle, FirstToken, SecondUpdateStr);
  IfrLibSetString (HiiHandle, SecondToken, FirstUpdateStr);

  gBS->FreePool (FirstUpdateStr);
  gBS->FreePool (SecondUpdateStr);
  //
  // sync the BootOrder between legacy normal and advance boot mode
  //
  switch (BootMenuType) {

  case ADV_BOOT_MENU :
  case LEGACY_ADV_MENU :
    BootIndex = 0;
    TempBootTypeOrder = EfiLibAllocateZeroPool (sizeof (SetupVariable->BootTypeOrder));
    EfiSetMem (TempBootTypeOrder, sizeof (SetupVariable->BootTypeOrder), 0xff);
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
    OldBootTypeOrder = EfiLibAllocateZeroPool (sizeof (SetupVariable->BootTypeOrder));
    OldBootTypeIndex = 0;

    for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
      for (Index1 = 0; Index1 < BootIndex; Index1++) {
        if (SetupVariable->BootTypeOrder[Index] == TempBootTypeOrder[Index1]) {
          OldBootTypeOrder[OldBootTypeIndex++] = SetupVariable->BootTypeOrder[Index];
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
        SetupVariable->BootTypeOrder,
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
    if (SetupVariable->BootNormalPriority == EFI_FIRST) {
      BootIndex = SUCInfo->EfiBootDeviceNum;
    } else {
      BootIndex = 0;
    }
    for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
      for (Index1 = 0; Index1 < BbsDevTableTypeCount; Index1++) {
      	if (SetupVariable->BootTypeOrder[Index] ==  mBbsTypeTable[TempBootType[Index1]].DeviceType) {
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

STATIC
EFI_STATUS
GetOrderByType (
  IN     UINTN                              LegacyAdvBootDeviceNum,
  IN     UINTN                              BbsDevTableTypeCount,
  IN     UINT16                             *SupportBootType,
  IN OUT UINT16                             *LegacyAdvBootOrder,
  IN OUT STRING_PTR                         *LegacyBootOrderString
)
{
  UINTN                                    Index;
  UINTN                                    Index1;
  UINTN                                    Index2;
  BOOLEAN                                  OtherTypeFlag;
  STRING_PTR                               *TempString;

  //
  // Get the count of each boot device type
  //
  GetDevTypeCount (
    LegacyBootOrderString,
    LegacyAdvBootDeviceNum,
    BbsDevTableTypeCount,
    SupportBootType
    );

  for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
    TempString = EfiLibAllocateZeroPool (sizeof (STRING_PTR) * (mBbsTypeTable[SupportBootType[Index]].DeviceTypeCount));
    if (TempString == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    if ((VOID *) (mBbsTypeTable[SupportBootType[Index]].StringPtr) != NULL) {
      gBS->FreePool ((VOID *)(mBbsTypeTable[SupportBootType[Index]].StringPtr));
    }
    mBbsTypeTable[SupportBootType[Index]].StringPtr = (UINTN) TempString;
  }


  for (Index1 = 0; Index1 < BbsDevTableTypeCount; Index1++) {
    mBbsTypeTable[SupportBootType[Index1]].CurrentIndex = 0;
    for (Index = 0; Index < LegacyAdvBootDeviceNum; Index++) {
      if (LegacyBootOrderString[Index].BbsEntry == NULL) {
        continue;
      }
      if (IsUsbDevice (LegacyBootOrderString[Index].BbsEntry)) {
        if (mBbsTypeTable[SupportBootType[Index1]].DeviceType == BBS_USB) {
      	   Index2 = (UINT16) mBbsTypeTable[SupportBootType[Index1]].CurrentIndex;
      	   TempString = (STRING_PTR *) mBbsTypeTable[SupportBootType[Index1]].StringPtr;
           TempString[Index2].BootOrderIndex = LegacyBootOrderString[Index].BootOrderIndex;
           TempString[Index2].EfiBootDevFlag = LegacyBootOrderString[Index].EfiBootDevFlag;
           TempString[Index2].pString        = LegacyBootOrderString[Index].pString;
           TempString[Index2].BbsEntry       = LegacyBootOrderString[Index].BbsEntry;
           (mBbsTypeTable[SupportBootType[Index1]].CurrentIndex)++;
        }
      } else if (LegacyBootOrderString[Index].BbsEntry->DeviceType == mBbsTypeTable[SupportBootType[Index1]].DeviceType) {
      	Index2 = (UINT16) mBbsTypeTable[SupportBootType[Index1]].CurrentIndex;
      	TempString = (STRING_PTR *) mBbsTypeTable[SupportBootType[Index1]].StringPtr;
        TempString[Index2].BootOrderIndex = LegacyBootOrderString[Index].BootOrderIndex;
        TempString[Index2].EfiBootDevFlag = LegacyBootOrderString[Index].EfiBootDevFlag;
        TempString[Index2].pString        = LegacyBootOrderString[Index].pString;
        TempString[Index2].BbsEntry       = LegacyBootOrderString[Index].BbsEntry;
        (mBbsTypeTable[SupportBootType[Index1]].CurrentIndex)++;
      }
    }
  }

  //
  // for other boot device type
  //
  mBbsTypeTable[OTHER_TYPE_INDEX].CurrentIndex = 0;
  for (Index = 0; Index < LegacyAdvBootDeviceNum; Index++) {
    OtherTypeFlag = TRUE;
    if (LegacyBootOrderString[Index].BbsEntry == NULL) {
      continue;
    }
    for (Index1 = 0; Index1 < BbsDevTableTypeCount; Index1++) {
      if (((LegacyBootOrderString[Index].BbsEntry->DeviceType == mBbsTypeTable[SupportBootType[Index1]].DeviceType) &&
           (!IsUsbDevice (LegacyBootOrderString[Index].BbsEntry))) ||
          ((mBbsTypeTable[SupportBootType[Index1]].DeviceType == BBS_USB) &&
           (IsUsbDevice (LegacyBootOrderString[Index].BbsEntry)))) {
      	OtherTypeFlag = FALSE;
      	break;
      }
    }
    if (OtherTypeFlag == TRUE) {
      Index2 = (UINT16) mBbsTypeTable[OTHER_TYPE_INDEX].CurrentIndex;
      TempString = (STRING_PTR *) mBbsTypeTable[OTHER_TYPE_INDEX].StringPtr;
      EfiCopyMem (
        &(TempString[Index2]),
        &(LegacyBootOrderString[Index]),
        sizeof(STRING_PTR)
        );
      (mBbsTypeTable[OTHER_TYPE_INDEX].CurrentIndex)++;
    }
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetNewBootOrder (
  IN     UINT16                                 CurrentBootDevFlag,
  IN     UINT16                                 BootDevCount,
  IN     STRING_PTR                             *AdvBootOrderString,
  IN OUT UINT16                                 *CurrentBootCount,
  IN OUT UINT16                                 **TempBootOrder,
  IN OUT STRING_PTR                             **TempBootOrderString
)
{
  UINTN                                         Index;
  UINTN                                         Index1;

  *TempBootOrderString = EfiLibAllocateZeroPool (sizeof(STRING_PTR) * BootDevCount);
  *TempBootOrder       = EfiLibAllocateZeroPool (sizeof(UINT16) * BootDevCount);
  EfiSetMem (*TempBootOrderString, sizeof(STRING_PTR) * BootDevCount, 0xff);
  EfiSetMem (*TempBootOrder, sizeof(UINT16) * BootDevCount, 0xff);

  *CurrentBootCount = 0;
  Index1 = 0;
  for (Index = 0; Index < BootDevCount; Index++) {
    if (AdvBootOrderString[Index].EfiBootDevFlag == CurrentBootDevFlag) {
      (*TempBootOrder)[Index1] = AdvBootOrderString[Index].BootOrderIndex;
      (*TempBootOrderString)[Index1].BootOrderIndex = AdvBootOrderString[Index].BootOrderIndex;
      (*TempBootOrderString)[Index1].EfiBootDevFlag = AdvBootOrderString[Index].EfiBootDevFlag;
      (*TempBootOrderString)[Index1].pString        = EfiLibAllocateZeroPool (EfiStrSize (AdvBootOrderString[Index].pString));
      EfiCopyMem ((*TempBootOrderString)[Index1].pString, AdvBootOrderString[Index].pString, EfiStrSize (AdvBootOrderString[Index].pString));
      (*TempBootOrderString)[Index1].BbsEntry       = AdvBootOrderString[Index].BbsEntry;
      (*TempBootOrderString)[Index1].DevicePath     = AdvBootOrderString[Index].DevicePath;
      (*CurrentBootCount)++;
      Index1++;
    }
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
SyncBootOrder (
  IN SYSTEM_CONFIGURATION                   *SetupVariable,
  IN UINTN                                  AdvBootDeviceNum,
  IN UINT16                                 *AdvBootOrder,
  IN STRING_PTR                             *AdvBootString,
  IN UINTN                                  LegacyBootDeviceNum,
  IN UINT16                                 *LegacyAdvBootOrder,
  IN STRING_PTR                             *LegacyAdvBootString,
  IN UINTN                                  EfiBootDeviceNum,
  IN UINT16                                 *EfiBootOrder,
  IN STRING_PTR                             *EfiBootString
)
{
  UINTN                                     Index;
  UINTN                                     Index1;
  UINTN                                     Index2;
  UINTN                                     Index3;
  UINT8                                     FrontBootDevFlag;
  UINT8                                     BackBootDevFlag;
  UINT16                                    BbsDevTableTypeCount;
  UINTN                                     FrontBootDevCount;
  UINTN                                     BackBootDevCount;
  UINT16                                    *BootType;
  UINT8                                     *BootTypeOrder;
  UINT16                                    *UpdateAdvBootOrder;
  STRING_PTR                                *UpdateAdvBootString;
  UINT16                                    *FrontBootOrder;
  STRING_PTR                                *FrontBootString;
  UINT16                                    *BackBootOrder;
  STRING_PTR                                *BackBootString;
  UINT16                                    *TempBootOrder;
  STRING_PTR                                *TempBootOrderString;
  UINTN                                     AdvBootDevCount;
  STRING_PTR                                *TempString;
  SETUP_UTILITY_CONFIGURATION               *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  EFI_STATUS                                Status;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo              = SuBrowser->SUCInfo;
  BbsDevTableTypeCount = SUCInfo->LegacyBootDevTypeCount;
  BootType             = SUCInfo->LegacyBootDevType;
  BootTypeOrder        = SetupVariable->BootTypeOrder;

  FrontBootDevCount    = 0;
  BackBootDevCount     = 0;
  FrontBootDevFlag     = LEGACY_BOOT_DEV;
  BackBootDevFlag      = EFI_BOOT_DEV;

  if (SetupVariable->BootMenuType == NORMAL_MENU) {
    if (SetupVariable->LegacyNormalMenuType == LEGACY_NORMAL_MENU) {
      Index2 = 0;
      for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
      	for (Index3 = 0 ; Index3 < BbsDevTableTypeCount; Index3++) {
      	  if (BootTypeOrder[Index] ==  mBbsTypeTable[BootType[Index3]].DeviceType) {
            TempString =  (STRING_PTR *) mBbsTypeTable[BootType[Index3]].StringPtr;
            for (Index1 = 0; Index1 < mBbsTypeTable[BootType[Index3]].DeviceTypeCount; Index1++) {
              LegacyAdvBootOrder[Index2] = TempString[Index1].BootOrderIndex;
              Index2++;
            }
          }
        }
      }
    }

    if (SetupVariable->BootNormalPriority == LEGACY_FIRST) {
      FrontBootDevFlag  = LEGACY_BOOT_DEV;
      FrontBootDevCount = LegacyBootDeviceNum;
      FrontBootOrder    = LegacyAdvBootOrder;
      FrontBootString   = LegacyAdvBootString;

      BackBootDevFlag   = EFI_BOOT_DEV;
      BackBootDevCount  = EfiBootDeviceNum;
      BackBootOrder     = EfiBootOrder;
      BackBootString    = EfiBootString;
    } else {
      FrontBootDevFlag  = EFI_BOOT_DEV;
      FrontBootDevCount = EfiBootDeviceNum;
      FrontBootOrder    = EfiBootOrder;
      FrontBootString   = EfiBootString;

      BackBootDevFlag   = LEGACY_BOOT_DEV;
      BackBootDevCount  = LegacyBootDeviceNum;
      BackBootOrder     = LegacyAdvBootOrder;
      BackBootString    = LegacyAdvBootString;
    }
  } else {
    //
    // current BootMenu is Adv BootMenu
    //
    return EFI_SUCCESS;
  }

  AdvBootDevCount     = AdvBootDeviceNum;
  UpdateAdvBootString = AdvBootString;
  UpdateAdvBootOrder  = AdvBootOrder;

  TempBootOrderString = EfiLibAllocateZeroPool (sizeof(STRING_PTR) * AdvBootDevCount);
  TempBootOrder       = EfiLibAllocateZeroPool (sizeof(UINT16) * AdvBootDevCount);

  EfiSetMem (TempBootOrderString, sizeof(STRING_PTR) * AdvBootDevCount, 0xff);
  EfiSetMem (TempBootOrder, sizeof(UINT16) * AdvBootDevCount, 0xff);

  //
  // merge Legacy and Efi BootOrder
  //
  for (Index = 0; Index < FrontBootDevCount; Index++) {
    for (Index1 = 0; Index1 < AdvBootDevCount; Index1++ ) {
      if (UpdateAdvBootString[Index1].BootOrderIndex == FrontBootOrder[Index]) {
        TempBootOrder[Index]                      = UpdateAdvBootOrder[Index1];
        TempBootOrderString[Index].BootOrderIndex = UpdateAdvBootString[Index1].BootOrderIndex;
        TempBootOrderString[Index].EfiBootDevFlag = UpdateAdvBootString[Index1].EfiBootDevFlag;
        TempBootOrderString[Index].pString        = UpdateAdvBootString[Index1].pString;
        TempBootOrderString[Index].BbsEntry       = UpdateAdvBootString[Index1].BbsEntry;
        TempBootOrderString[Index].DevicePath     = UpdateAdvBootString[Index1].DevicePath;
        break;
      }
    }
  }

  Index2 = Index;
  for (Index = 0; Index < BackBootDevCount; Index++) {
    for (Index1 = 0; Index1 < AdvBootDevCount; Index1++ ) {
      if (UpdateAdvBootString[Index1].BootOrderIndex == BackBootOrder[Index]) {
        TempBootOrder[Index2]                      = UpdateAdvBootOrder[Index1];
        TempBootOrderString[Index2].BootOrderIndex = UpdateAdvBootString[Index1].BootOrderIndex;
        TempBootOrderString[Index2].EfiBootDevFlag = UpdateAdvBootString[Index1].EfiBootDevFlag;
        TempBootOrderString[Index2].pString        = UpdateAdvBootString[Index1].pString;
        TempBootOrderString[Index2].BbsEntry       = UpdateAdvBootString[Index1].BbsEntry;
        TempBootOrderString[Index2].DevicePath     = UpdateAdvBootString[Index1].DevicePath;
        Index2++;
        break;
      }
    }
  }

  EfiCopyMem (
    UpdateAdvBootOrder,
    TempBootOrder,
    sizeof (UINT16) * AdvBootDevCount
    );

  EfiCopyMem (
    UpdateAdvBootString,
    TempBootOrderString,
    sizeof (STRING_PTR) * AdvBootDevCount
    );

  gBS->FreePool(TempBootOrder);
  gBS->FreePool(TempBootOrderString);
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
  IN EFI_HII_UPDATE_DATA                       *UpdateDevData
)
{
  UINTN                                        Index;
  STRING_REF                                   TempLastToken;
  SETUP_UTILITY_BROWSER_DATA                   *SuBrowser;
  EFI_STATUS                                   Status;
   UINT8                                       *LocalBuffer;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  TempLastToken = *LastToken;
  for (Index = 0; Index < BootDevCount; Index++, TempLastToken = 0) {

    IfrLibNewString (HiiHandle, &TempLastToken, BootString[Index].pString);
    if (Index == 0) {
      *LastToken = TempLastToken;
    }
    LocalBuffer = (UINT8 *) UpdateDevData->Data + UpdateDevData->Offset;
    CreateActionOpCode (
      (EFI_QUESTION_ID) (DeviceKeyBase + Index),
      TempLastToken,
      0,
      0,
      0,
      UpdateDevData
      );
    ((EFI_IFR_ACTION *) LocalBuffer)->Question.Flags = EFI_IFR_CHECKBOX_DEFAULT_MFG;
  }

  IfrLibUpdateForm (HiiHandle, NULL, FormId, BootDevLabel, TRUE, UpdateDevData);
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetDevTypeCount (
  IN STRING_PTR                            *AdvBootDevString,
  IN UINTN                                 AdvDevCount,
  IN UINTN                                 BbsDevTableTypeCount,
  IN UINT16                                *SupportBootDevType
)
{
  UINTN                                    DeviceIndex;
  UINTN                                    DeviceTypeIndex;
  UINTN                                    Index;
  BOOLEAN                                  IsOtherType;

  for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
    mBbsTypeTable[SupportBootDevType[Index]].DeviceTypeCount = 0;
    for (DeviceIndex = 0; DeviceIndex < AdvDevCount; DeviceIndex++) {
      if (AdvBootDevString[DeviceIndex].BbsEntry == NULL) {
        continue;
      }
      if (IsUsbDevice (AdvBootDevString[DeviceIndex].BbsEntry)) {
        if (mBbsTypeTable[SupportBootDevType[Index]].DeviceType == BBS_USB) {
           mBbsTypeTable[SupportBootDevType[Index]].DeviceTypeCount++;
        }
      } else if (AdvBootDevString[DeviceIndex].BbsEntry->DeviceType == mBbsTypeTable[SupportBootDevType[Index]].DeviceType) {
        mBbsTypeTable[SupportBootDevType[Index]].DeviceTypeCount++;
      }
    }

    if ( mBbsTypeTable[SupportBootDevType[Index]].DeviceTypeCount > MAX_BOOT_DEVICES_NUMBER) {
      mBbsTypeTable[SupportBootDevType[Index]].DeviceTypeCount = MAX_BOOT_DEVICES_NUMBER;
    }
  }

  //
  // for other boot device type
  //
  for (Index = 0; Index < AdvDevCount; Index++) {
    IsOtherType = TRUE;
    if (AdvBootDevString[Index].BbsEntry == NULL) {
      continue;
    }
    for (DeviceTypeIndex = 0; DeviceTypeIndex < BbsDevTableTypeCount; DeviceTypeIndex++) {
      if (((AdvBootDevString[Index].BbsEntry->DeviceType == mBbsTypeTable[SupportBootDevType[DeviceTypeIndex]].DeviceType) &&
          (!IsUsbDevice (AdvBootDevString[Index].BbsEntry)))||
          ((mBbsTypeTable[SupportBootDevType[DeviceTypeIndex]].DeviceType == BBS_USB) &&
          (IsUsbDevice (AdvBootDevString[Index].BbsEntry)))) {
      	IsOtherType = FALSE;
      	break;
      }
    }
    if (IsOtherType) {
      mBbsTypeTable[OTHER_TYPE_INDEX].DeviceTypeCount++;
    }
  }
  if ( mBbsTypeTable[OTHER_TYPE_INDEX].DeviceTypeCount > MAX_BOOT_DEVICES_NUMBER) {
    mBbsTypeTable[OTHER_TYPE_INDEX].DeviceTypeCount = MAX_BOOT_DEVICES_NUMBER;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
GetBbsTypeTableSize (
  UINT16                                    *BbsDevTableTypeCount,
  UINT16                                    **TempBootType
)
{
  UINT8                                      BootTypeOrderSquence[] = {BOOT_TYPE_ORDER_SEQUENCE};
  UINT16                                     Index;
  UINT16                                     DeviceTypeIndex;
  UINT16                                     TypeNum;

  TypeNum = 0;
  *BbsDevTableTypeCount = sizeof (BootTypeOrderSquence) / sizeof (UINT8);
  *TempBootType   = EfiLibAllocateZeroPool (*BbsDevTableTypeCount * sizeof (UINT16));
  for (Index = 0; Index < *BbsDevTableTypeCount; Index++) {
    for (DeviceTypeIndex = 0; DeviceTypeIndex < MAX_BOOT_ORDER_NUMBER; DeviceTypeIndex++) {
      if (BootTypeOrderSquence[Index] == mBbsTypeTable[DeviceTypeIndex].DeviceType) {
        (*TempBootType)[TypeNum++] = DeviceTypeIndex;
        break;
      }
    }
    if (DeviceTypeIndex == MAX_BOOT_ORDER_NUMBER)  {
      return EFI_INVALID_PARAMETER;
    }
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetBootDevList (
  IN     UINT16                             *BootOrder,
  IN OUT STRING_PTR                         **BootOrderString,
  IN OUT UINT16                             **TempBootOrder,
  IN     UINT16                             BootDeviceNum
)
{
  UINT16                                    Index;
  UINT16                                    BootOptionName[20];
  UINTN                                     BootOptionSize;
  UINT8                                     *BootOption;
  UINT8                                     *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  UINTN                                     DeviceNameSize;
  UINT16                                    DevPathLen;


  *BootOrderString = EfiLibAllocateZeroPool (sizeof (STRING_PTR) * BootDeviceNum);
  *TempBootOrder   = EfiLibAllocateZeroPool (sizeof (UINT16) * BootDeviceNum);

  Index = 0;
  while (Index < BootDeviceNum) {
    if (Index >= MAX_BOOT_COUNT) {
      return EFI_OUT_OF_RESOURCES;
    }
    SPrint (
      BootOptionName,
      sizeof (BootOptionName),
      L"Boot%04x",
      BootOrder[Index]
      );

    BootOption = SetupUtilityLibGetVariableAndSize (
                   BootOptionName,
                   &gEfiGlobalVariableGuid,
                   &BootOptionSize
                   );

    if (BootOption == NULL) {
      gBS->FreePool (BootOrder);
      return EFI_OUT_OF_RESOURCES;
    }

    TempPtr = BootOption;
    TempPtr += sizeof (UINT32);
    DevPathLen = *(UINT16 *) TempPtr;
    TempPtr += sizeof (UINT16);
    DeviceNameSize = EfiStrSize ((UINT16 *) TempPtr);
    (*BootOrderString)[Index].BootOrderIndex = BootOrder[Index];
    (*TempBootOrder)[Index]                  = BootOrder[Index];
    (*BootOrderString)[Index].pString        = EfiLibAllocateZeroPool (DeviceNameSize);
    EfiCopyMem (
      (*BootOrderString)[Index].pString,
      TempPtr,
      DeviceNameSize
      );

    TempPtr += DeviceNameSize;
    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    (*BootOrderString)[Index].DevicePath = EfiLibAllocateZeroPool (DevPathLen);
    EfiCopyMem ((*BootOrderString)[Index].DevicePath, DevicePath, DevPathLen);

    if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
      (*BootOrderString)[Index].EfiBootDevFlag = LEGACY_BOOT_DEV;
      TempPtr += DevPathLen;
      (*BootOrderString)[Index].BbsEntry = EfiLibAllocateZeroPool (sizeof (BBS_TABLE));
      EfiCopyMem (
        (*BootOrderString)[Index].BbsEntry,
        TempPtr,
        sizeof (BBS_TABLE)
        );
    } else {
      (*BootOrderString)[Index].EfiBootDevFlag = EFI_BOOT_DEV;
      (*BootOrderString)[Index].BbsEntry = NULL;
    }
    gBS->FreePool (BootOption);
    Index++;
  }
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
SortBbsTypeTable (
  UINTN                                     BbsDevTableTypeCount,
  UINT16                                    BbsTotalCount,
  UINT16                                    *TempBootType,
  UINT16                                    *TempBootOrder,
  UINT16                                    BbsAdvCount
)
/*++

Routine Description:

  To sort the BootOrder by BootTypeOrder

Arguments:

  BbsDevTableTypeCount              The number of legacy boot device type
  BbsTotalCount                     The number of Bbs entry
  TempBootType                      The pointer of array that SUC support boot device type
  TempBootOrder                     The pointer of BootOrder
  BbsAdvCount                       The number of BootOption

Returns:

  EFI_SUCCESS - Success sort the BootOrder

--*/
{
  UINT16                                     Index;
  UINT16                                     AdvDeviceNum;
  UINT16                                     DeviceTypeIndex;
  UINT16                                     CurrentIndex;
  STRING_PTR                                 *WorkBbsString;
  STRING_PTR                                 *BbsString;

  WorkBbsString = EfiLibAllocateZeroPool (sizeof (STRING_PTR) * BbsTotalCount);
  for (Index = 0; Index < BbsDevTableTypeCount; Index++) {
    mBbsTypeTable[TempBootType[Index]].CurrentIndex = 0;
    EfiSetMem (WorkBbsString, (sizeof (STRING_PTR) * BbsTotalCount), 0xff);
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
    EfiCopyMem (
      BbsString,
      WorkBbsString,
      sizeof(STRING_PTR) * mBbsTypeTable[TempBootType[Index]].DeviceTypeCount
      );
  }
  gBS->FreePool (WorkBbsString);
  return EFI_SUCCESS;
}

STATIC
BOOLEAN
IsUsbDevice (
  IN BBS_TABLE              *CurrentBbsTable
  )
/*++

Routine Description:

   Check the specific BBS Table entry is USB device

Arguments:
   CurrentBbsTable   - Pointer to current BBS table start address

Returns:

  TRUE    - It is USB device
  FALSE   - It isn't USB device

--*/
{
  if ((CurrentBbsTable->Class == PCI_CLASS_SERIAL) &&
      (CurrentBbsTable->SubClass == PCI_CLASS_SERIAL_USB)) {
    return TRUE;
  }
  return FALSE;

}

STATIC
BOOLEAN
IsEfiDevice (
  IN     UINT16    NumberOfDevice
  )
/*++

Routine Description:

  Check the Bootxxxx number of device is EFI device or legacy device.

Arguments:

  NumberOfDevice - The device number in BootOrder variable (ex: Boot0001,
                   then NumberOfDevice is 1)

Returns:

  TRUE  - The device is EFI device.
  FALSE - The device is legacy device.

--*/
{
  UINT16                    BootOptionName[20];
  UINTN                     BootOptionSize;
  UINT8                     *BootOption;
  UINT8                     *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   IsEfiDevice;

  SPrint (
    BootOptionName,
    sizeof (BootOptionName),
    L"Boot%04x",
    NumberOfDevice
    );
  BootOption = SetupUtilityLibGetVariableAndSize (
                 BootOptionName,
                 &gEfiGlobalVariableGuid,
                 &BootOptionSize
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
  WorkingPtr += (UINTN) EfiStrSize ((UINT16 *) WorkingPtr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;

  IsEfiDevice = TRUE;
  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    IsEfiDevice = FALSE;
  }

  return IsEfiDevice;
}

STATIC
BOOLEAN
IsEfiShellBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
/*++
Routine Description:

  Check if the boot option is a EFI shell boot option which is put in ROM image.

Arguments:

  DevicePath     - The device path need to be processed.

Returns:
  TRUE           - It is a EFI shell boot option in ROM image.
  FALSE          - It is not a EFI shell boot option in ROM image.

**/
{
  EFI_DEVICE_PATH_PROTOCOL     *LastDeviceNode;
  EFI_DEVICE_PATH_PROTOCOL     *WorkDevicePath;
  BOOLEAN                      IsEfiShell;
  EFI_GUID                     *FileGuid;

  IsEfiShell = FALSE;
  WorkDevicePath = DevicePath;
  LastDeviceNode = DevicePath;

  while (!EfiIsDevicePathEnd (WorkDevicePath)) {
    LastDeviceNode = WorkDevicePath;
    WorkDevicePath = EfiNextDevicePathNode (WorkDevicePath);
  }
  FileGuid = EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode);
  if (FileGuid != NULL && (EfiCompareGuid (FileGuid, &gEfiShellFileGuid) || EfiCompareGuid (FileGuid, &gEfiMiniShellFileGuid))) {
    IsEfiShell = TRUE;
  }

  return IsEfiShell;
}

EFI_STATUS
AdjustBootOrder (
  IN      BOOLEAN      EfiDeviceFirst,
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
/*++

Routine Description:

  According to input parameter to adjust boot order to EFI device first or
  legacy device first.

Arguments:

  EfiDeviceFirst -  TRUE : Indicate EFI device first.
                    FALSE: Indicate legacy device first.
  DeviceCount    -  Total device count
  BootOrder      -  Pointer to BootOrder.

Returns:

  EFI_SUCCESS           - Adjust boot order successful.
  EFI_INVALID_PARAMETER - Input parameter is invalid.

--*/
{
  UINTN               BootOrderIndex;
  UINT16              *EfiDevicOrder;
  UINTN               EfiDeviceIndex;
  UINT16              *LegacyDeviceOrder;
  UINTN               LegacyDeviceIndex;

  if (DeviceCount == 0 || BootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiDevicOrder = EfiLibAllocateZeroPool (DeviceCount * sizeof (CHAR16));
  LegacyDeviceOrder = EfiLibAllocateZeroPool (DeviceCount * sizeof (CHAR16));
  EfiDeviceIndex = 0;
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
    EfiCopyMem (BootOrder, EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
    EfiCopyMem (&BootOrder[EfiDeviceIndex], LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
  } else {
    EfiCopyMem (BootOrder, LegacyDeviceOrder, LegacyDeviceIndex * sizeof (CHAR16));
    EfiCopyMem (&BootOrder[LegacyDeviceIndex], EfiDevicOrder, EfiDeviceIndex * sizeof (CHAR16));
  }

  gBS->FreePool (EfiDevicOrder);
  gBS->FreePool (LegacyDeviceOrder);
  return EFI_SUCCESS;
}

EFI_STATUS
SortEfiDevices (
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
/*++

Routine Description:

  According to EFI device priority to sort all of EFI devices.

Arguments:

  DeviceCount    -  Total EFI device count
  BootOrder      -  Pointer to EFI device BootOrder.

Returns:

  EFI_SUCCESS           - Change to auto boot order policy successful.
  EFI_INVALID_PARAMETER - Input parameter is invalid.
  EFI_UNSUPPORTED       - doesn't support auto EFI Boot option oreder algorithm

--*/
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
                &BootOptionPolicy
                );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  BootPriority = EfiLibAllocateZeroPool (DeviceCount * sizeof (EFI_BOOT_ORDER_PRIORITY));
  for (Index = 0; Index < DeviceCount; Index++) {
    SPrint (
      BootOptionName,
      sizeof (BootOptionName),
      L"Boot%04x",
      BootOrder[Index]
      );
    BootOption = SetupUtilityLibGetVariableAndSize (
                   BootOptionName,
                   &gEfiGlobalVariableGuid,
                   &BootOptionSize
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
    Size += sizeof (UINT16) + (UINT16) EfiStrSize ((UINT16 *) WorkingPtr);
    WorkingPtr += EfiStrSize ((UINT16 *) WorkingPtr);
    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
    WorkingPtr += EfiDevicePathSize (DevicePath);
    CreateByOS = TRUE;

    if (((BootOptionSize - Size == 2) && (EfiAsciiStrnCmp (WorkingPtr, "RC", 2) == 0)) ||
        (IsEfiShellBootOption (DevicePath) && (BootOptionSize - Size >= 2) && (EfiAsciiStrnCmp (WorkingPtr, "RC", 2) == 0))) {
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


EFI_STATUS
ChangeToAutoBootOrder (
  IN      UINTN        DeviceCount,
  IN OUT  UINT16       *BootOrder
  )
/*++

Routine Description:

  Change boot order to suit auto boot option order policy.

Arguments:

  DeviceCount    -  Total device count
  BootOrder      -  Pointer to BootOrder.

Returns:

  EFI_SUCCESS           - Change to auto boot order policy successful.
  EFI_INVALID_PARAMETER - Input parameter is invalid.

--*/
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

  EfiDeviceOrder = EfiLibAllocateZeroPool (EfiDeviceNum * sizeof (CHAR16));
  EfiCopyMem (EfiDeviceOrder, &BootOrder[EfiStartIndex], EfiDeviceNum * sizeof (UINT16));
  //
  // According to EFI device priority to sort EFI devices
  //
  SortEfiDevices (EfiDeviceNum, EfiDeviceOrder);

  //
  // copy sorted EFI device order to boot order
  //
  EfiCopyMem (&BootOrder[EfiStartIndex], EfiDeviceOrder, EfiDeviceNum * sizeof (UINT16));

  gBS->FreePool (EfiDeviceOrder);
  return EFI_SUCCESS;
}
