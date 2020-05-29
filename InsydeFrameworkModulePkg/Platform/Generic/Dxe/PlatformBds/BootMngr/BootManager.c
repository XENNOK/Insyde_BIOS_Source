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

  BootManager.c

Abstract:

  The platform boot manager reference implement

--*/

#include "BootManager.h"
#include "SetupConfig.h"
#include "BBSsupport.h"
#include "LegacyBios.h"
#include "IdeControllerInit.h"
#include "UsbCore.h"
#include EFI_PROTOCOL_DEFINITION (DiskInfo)
UINT16                            mKeyInput;
EFI_LIST_ENTRY                    *mBootOptionsList;
BDS_COMMON_OPTION                 *gOption;
EFI_HII_HANDLE                    gBootManagerHandle;
EFI_HANDLE                        BootManagerCallbackHandle;
EFI_FORM_CALLBACK_PROTOCOL        BootManagerCallback;
EFI_GUID                          gBmGuid = BOOT_MANAGER_GUID;

extern UINT8                      BootManagerVfrBin[];
extern UINT8                      BdsStrings[];
BOOLEAN                           gConnectAllHappened;

EFI_STATUS
EFIAPI
BootManagerCallbackRoutine (
  IN EFI_FORM_CALLBACK_PROTOCOL       *This,
  IN UINT16                           KeyValue,
  IN EFI_IFR_DATA_ARRAY               *DataArray,
  OUT EFI_HII_CALLBACK_PACKET         **Packet
  )
/*++

Routine Description:

  This is the function that is called to provide results data to the driver.  This data
  consists of a unique key which is used to identify what data is either being passed back
  or being asked for.

Arguments:

  KeyValue -        A unique value which is sent to the original exporting driver so that it
                    can identify the type of data to expect.  The format of the data tends to
                    vary based on the op-code that geerated the callback.

  Data -            A pointer to the data being sent to the original exporting driver.

Returns:

--*/
{
  BDS_COMMON_OPTION       *Option;
  EFI_LIST_ENTRY          *Link;
  UINT16                  KeyCount;
  EFI_HII_CALLBACK_PACKET *DataPacket;

  //
  // Initialize the key count
  //
  KeyCount = 0;

  for (Link = mBootOptionsList->ForwardLink; Link != mBootOptionsList; Link = Link->ForwardLink) {
    Option = CR (Link, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);

    KeyCount++;

    gOption = Option;

    //
    // Is this device the one chosen?
    //
    if (KeyCount == KeyValue) {
      //
      // Assigning the returned Key to a global allows the original routine to know what was chosen
      //
      mKeyInput = KeyValue;

      *Packet   = EfiLibAllocateZeroPool (sizeof (EFI_HII_CALLBACK_PACKET) + 2);
      ASSERT (*Packet != NULL);

      //
      // Assign the buffer address to DataPacket
      //
      DataPacket                        = *Packet;

      DataPacket->DataArray.EntryCount  = 1;
      DataPacket->DataArray.NvRamMap    = NULL;
      DataPacket->DataArray.Data->Flags = EXIT_REQUIRED | NV_NOT_CHANGED;
      return EFI_SUCCESS;
    } else {
      continue;
    }
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
  CHAR16    *TempStr;
  CHAR16    SpaceChar = ' ';

  if (Str == NULL) {
    return;
  }

  StrLen = EfiStrLen (Str);
  TempStr = EfiLibAllocateZeroPool ((StrLen + 1) * sizeof(CHAR16));

  Start = 0;
  End   = StrLen - 1;

  while (Str[Start] == SpaceChar) {
    Start++;
  }

  while (Str[End] == SpaceChar) {
    End--;
  }

  gBS->CopyMem (TempStr, &Str[Start], (End - Start + 1) * sizeof(CHAR16));
  gBS->CopyMem (Str, TempStr, (StrLen + 1) * sizeof(CHAR16));

  gBS->FreePool (TempStr);
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
GetAllHwBootDeviceInfo (
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
  EFI_GUID                  EfiUsbCoreProtocolGuid = EFI_USB_CORE_PROTOCOL_GUID;
  USB_MASS_STORAGE_DEVICE   **UsbMassStorageDevices;
  EFI_USB_CORE_PROTOCOL     *UsbCore;
  UINTN                     UsbMassStorageDevicesCount;
  EFI_USB_IO_PROTOCOL       *UsbIoInstance;
  UINTN                     Index2;

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
          UpdateAtaString (
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

  //
  // Collect all USB device information
  //
  Status = gBS->LocateProtocol (
                  &EfiUsbCoreProtocolGuid,
                   NULL,
                  &UsbCore
                  );
  if (EFI_ERROR(Status)) {
    gBS->FreePool (DeviceName);
    return Status;
  }

  Status = UsbCore->GetUsbDevices (
                      USB_CORE_USB_MASS_STORAGE,
                      &UsbMassStorageDevicesCount,
                      (USB_DEVICE***) &UsbMassStorageDevices
                      );
  if (EFI_ERROR(Status)) {
    gBS->FreePool (DeviceName);
    return Status;
  }

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiUsbIoProtocolGuid,
         NULL,
         &HandleCount,
         &HandleBuffer
         );
  //
  // Compare UsbIoProtocol instance. If the same, save device path and device name.
  //
  for (Index = 0; Index < HandleCount; Index ++) {
    gBS->HandleProtocol (
           HandleBuffer[Index],
           &gEfiUsbIoProtocolGuid,
           &UsbIoInstance
           );

    for (Index2 = 0; Index2 < UsbMassStorageDevicesCount; Index2 ++) {
      if (UsbMassStorageDevices[Index2]->UsbIo == UsbIoInstance) {
        Status = gBS->HandleProtocol (
                        HandleBuffer[Index],
                        &gEfiDevicePathProtocolGuid,
                        (VOID *) &DevicePath
                        );
        if (EFI_ERROR(Status)) {
          continue;
        }

        HwBootDeviceInfo[*HwBootDeviceInfoCount].BlockIoDevicePath = DevicePath;

        HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName = EfiLibAllocateZeroPool (25 * sizeof (CHAR16));
        AsciiToUnicode (
          (CHAR8 *) UsbMassStorageDevices[Index2]->Manufacturer,
          HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName,
          8);
        AsciiToUnicode (
          (CHAR8 *) UsbMassStorageDevices[Index2]->Product,
          &HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName[8],
          16);
        CleanSpaceChar (HwBootDeviceInfo[*HwBootDeviceInfoCount].HwDeviceName);

        (*HwBootDeviceInfoCount)++;
      }
    }
  }

  if (HandleCount) {
    gBS->FreePool (HandleBuffer);
  }
  gBS->FreePool (DeviceName);

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

VOID
CallBootManager (
  VOID
  )
/*++

Routine Description:
  Hook to enable UI timeout override behavior.

Arguments:
  BdsDeviceList - Device List that BDS needs to connect.

  Entry - Pointer to current Boot Entry.

Returns:
  NONE

--*/
{
  EFI_STATUS          Status;
  EFI_HII_PACKAGES    *PackageList;
  BDS_COMMON_OPTION   *Option;
  EFI_LIST_ENTRY      *Link;
  EFI_HII_UPDATE_DATA *UpdateData;
  CHAR16              *ExitData;
  UINTN               ExitDataSize;
  STRING_REF          Token;
  STRING_REF          LastToken;
  EFI_INPUT_KEY       Key;
  UINT8               *Location;
  EFI_GUID            BmGuid;
  EFI_LIST_ENTRY      BdsBootOptionList;
  BOOLEAN	            BootMngrMenuResetRequired;
#ifdef ENABLE_CONSOLE_EX
  EFI_KEY_DATA                       KeyData;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *SimpleTextInEx = NULL;
#endif
  HARDWARE_BOOT_DEVICE_INFO          HwBootDeviceInfo[MAX_BOOT_DEVICES_NUMBER];
  UINTN                              HwBootDeviceCount;
  UINTN                              Index;
  CHAR16                             *BootOptionStr;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  UINTN                             VarSize;
  CHAR16                            *HwDeviceName;
  CHAR16                            NoDeviceStr[] = L"No Device";
  EFI_FORM_BROWSER_PROTOCOL          *BootManagerBrowser;

  BootOptionStr = NULL;

  Token = 0;

  gOption = NULL;
  InitializeListHead (&BdsBootOptionList);

  //
  // Connect all prior to entering the platform setup menu.
  //
  if (!gConnectAllHappened) {
    BdsLibConnectAllDriversToAllControllers ();
    gConnectAllHappened = TRUE;
  }
  //
  // BugBug: Here we can not remove the legacy refresh macro, so we need
  // get the boot order every time from "BootOrder" variable.
  // Recreate the boot option list base on the BootOrder variable
  //
  BdsLibEnumerateAllBootOption (TRUE, &BdsBootOptionList);

  //
  // This GUID must be the same as what is defined in BootManagerVfr.vfr
  //
  BmGuid            = gBmGuid;

  mBootOptionsList  = &BdsBootOptionList;

  //
  // Post our VFR to the HII database
  //
  PackageList = PreparePackages (2, &BmGuid, BootManagerVfrBin, BdsStrings);
  Status      = Hii->NewPack (Hii, PackageList, &gBootManagerHandle);
  UpdateBvdtToHii (gBootManagerHandle);
  gBS->FreePool (PackageList);

  //
  // This example does not implement worker functions
  // for the NV accessor functions.  Only a callback evaluator
  //
  BootManagerCallback.NvRead    = NULL;
  BootManagerCallback.NvWrite   = NULL;
  BootManagerCallback.Callback  = BootManagerCallbackRoutine;

  //
  // Install protocol interface
  //
  BootManagerCallbackHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &BootManagerCallbackHandle,
                  &gEfiFormCallbackProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &BootManagerCallback
                  );
  ASSERT_EFI_ERROR (Status);

  LastToken = 0;
  Hii->NewString (Hii, NULL, gBootManagerHandle, &LastToken, L" ");

  //
  // Allocate space for creation of UpdateData Buffer
  //
  UpdateData = EfiLibAllocateZeroPool (0x1000);
  ASSERT (UpdateData != NULL);

  //
  // Flag update pending in FormSet
  //
  UpdateData->FormSetUpdate = TRUE;
  //
  // Register CallbackHandle data for FormSet
  //
  UpdateData->FormCallbackHandle = (EFI_PHYSICAL_ADDRESS) (UINTN) BootManagerCallbackHandle;
  UpdateData->FormUpdate  = FALSE;
  UpdateData->FormTitle   = 0;
  UpdateData->DataCount   = 1;

  //
  // Create blank space.  Since when we update the contents of IFR data at a label, it is
  // inserted at the location of the label.  So if you want to add a string with an empty
  // space afterwards, you need to add the space first and then the string like below.
  //
  Status = CreateSubTitleOpCode (
            LastToken,        // Token Value for the string
            &UpdateData->Data // Buffer containing created op-code
            );

  Hii->UpdateForm (Hii, gBootManagerHandle, (EFI_FORM_LABEL) 0x0000, TRUE, UpdateData);

  //
  // Create "Boot Option Menu" title
  //
  Status = CreateSubTitleOpCode (
            STRING_TOKEN (STR_BOOT_OPTION_BANNER),  // Token Value for the string
            &UpdateData->Data                       // Buffer containing created op-code
            );

  Hii->UpdateForm (Hii, gBootManagerHandle, (EFI_FORM_LABEL) 0x0000, TRUE, UpdateData);

  Token                 = LastToken;
  mKeyInput             = 0;

  UpdateData->DataCount = 0;
  Location              = (UINT8 *) &UpdateData->Data;
  GetAllHwBootDeviceInfo (&HwBootDeviceCount, HwBootDeviceInfo);
  DevicePath = NULL;
  DevicePath = BdsLibGetVariableAndSize (
                 L"PortableWorkSpace",
                 &gEfiGenericVariableGuid,
                 &VarSize
                 );
  for (Link = BdsBootOptionList.ForwardLink; Link != &BdsBootOptionList; Link = Link->ForwardLink) {
    Option = CR (Link, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);
    //
    // At this stage we are creating a menu entry, thus the Keys are reproduceable
    //
    mKeyInput++;
    Token++;
    //
    // Append device name to boot option description if BlkIo device path is match.
    //
    HwDeviceName = NULL;

    for (Index = 0; Index < HwBootDeviceCount; Index++) {
      if ((DevicePathType (Option->DevicePath) == MESSAGING_DEVICE_PATH) &&
          (DevicePathSubType (Option->DevicePath) == MSG_USB_CLASS_DP)) {
        if (DevicePath == NULL) {
          HwDeviceName = NoDeviceStr;
        } else if (CompareBlockIoDevicePath (HwBootDeviceInfo[Index].BlockIoDevicePath, DevicePath)) {
          HwDeviceName = HwBootDeviceInfo[Index].HwDeviceName;
        }
      } else if (CompareBlockIoDevicePath (HwBootDeviceInfo[Index].BlockIoDevicePath, Option->DevicePath)) {
        HwDeviceName = HwBootDeviceInfo[Index].HwDeviceName;
      }
      if (HwDeviceName != NULL) {
        BootOptionStr = EfiLibAllocateZeroPool (
                          EfiStrSize (Option->Description) + EfiStrSize (L" (") +
                          EfiStrSize (HwDeviceName) + EfiStrSize (L")"));
        EfiStrCat (BootOptionStr, Option->Description);
        EfiStrCat (BootOptionStr, L" (");
        EfiStrCat (BootOptionStr, HwDeviceName);
        EfiStrCat (BootOptionStr, L")");
        Hii->NewString (Hii, NULL, gBootManagerHandle, &Token, BootOptionStr);
        gBS->FreePool (BootOptionStr);
        break;
     }
    }

    if (HwBootDeviceCount == 0 &&
        DevicePath == NULL &&
        (DevicePathType (Option->DevicePath) == MESSAGING_DEVICE_PATH) &&
        (DevicePathSubType (Option->DevicePath) == MSG_USB_CLASS_DP)) {
      HwDeviceName = NoDeviceStr;
      BootOptionStr = EfiLibAllocateZeroPool (
                        EfiStrSize (Option->Description) + EfiStrSize (L" (") +
                        EfiStrSize (HwDeviceName) + EfiStrSize (L")"));
      EfiStrCat (BootOptionStr, Option->Description);
      EfiStrCat (BootOptionStr, L" (");
      EfiStrCat (BootOptionStr, HwDeviceName);
      EfiStrCat (BootOptionStr, L")");
      Hii->NewString (Hii, NULL, gBootManagerHandle, &Token, BootOptionStr);
      gBS->FreePool (BootOptionStr);
      Index = HwBootDeviceCount + 1;
    }

    if (Index == HwBootDeviceCount) {
      Status = Hii->NewString (Hii, NULL, gBootManagerHandle, &Token, Option->Description);
    }

    //
    // If we got an error it is almost certainly due to the token value being invalid.
    // Therefore we will set the Token to 0 to automatically add a token.
    //
    if (EFI_ERROR (Status)) {
      Token   = 0;
      if (Index == HwBootDeviceCount) {
        Status = Hii->NewString (Hii, NULL, gBootManagerHandle, &Token, Option->Description);
      } else {
        Status = Hii->NewString (Hii, NULL, gBootManagerHandle, &Token, BootOptionStr);
        gBS->FreePool (BootOptionStr);
      }
    }

    Status = CreateGotoOpCode (
              0x1000, // Form ID
              Token,  // Token Value for the string
              0,      // Help String (none)
              EFI_IFR_FLAG_INTERACTIVE | EFI_IFR_FLAG_NV_ACCESS,  // The Op-Code flags
              mKeyInput,                                          // The Key to get a callback on
              Location  // Buffer containing created op-code
              );

    UpdateData->DataCount++;
    Location = Location + ((EFI_IFR_OP_HEADER *) Location)->Length;

  }

  Hii->UpdateForm (Hii, gBootManagerHandle, (EFI_FORM_LABEL) 0x0001, TRUE, UpdateData);

  UpdateData->DataCount = 1;

  //
  // Create "Boot Option Menu" title
  //
  Status = CreateSubTitleOpCode (
            STRING_TOKEN (STR_HELP_FOOTER), // Token Value for the string
            &UpdateData->Data               // Buffer containing created op-code
            );

  Hii->UpdateForm (Hii, gBootManagerHandle, (EFI_FORM_LABEL) 0x0002, TRUE, UpdateData);

  Status = CreateSubTitleOpCode (
            LastToken,                      // Token Value for the string
            &UpdateData->Data               // Buffer containing created op-code
            );

  Hii->UpdateForm (Hii, gBootManagerHandle, (EFI_FORM_LABEL) 0x0002, TRUE, UpdateData);

  gBS->FreePool (UpdateData);
  if (DevicePath != NULL) {
    gBS->FreePool (DevicePath);
  }
  if (HwBootDeviceCount != 0) {
    for (Index = 0; Index < HwBootDeviceCount; Index++) {
      gBS->FreePool (HwBootDeviceInfo[Index].HwDeviceName);
    }
  }

  //
  // Drop the TPL level from EFI_TPL_DRIVER to EFI_TPL_APPLICATION
  //
  gBS->RestoreTPL (EFI_TPL_APPLICATION);

  BootManagerBrowser = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiFormBrowserProtocolGuid,
                  NULL,
                  &BootManagerBrowser
                  );
  ASSERT (BootManagerBrowser);

  BootMngrMenuResetRequired = FALSE;
  BootManagerBrowser->SendForm (
                        BootManagerBrowser,
                        TRUE,
                        &gBootManagerHandle,
                        1,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        &BootMngrMenuResetRequired
                        );

  if (BootMngrMenuResetRequired) {
    EnableResetRequired ();
  }

  Hii->ResetStrings (Hii, gBootManagerHandle);

  if (gOption == NULL) {
    return ;
  }

  //
  // Will leave browser, check any reset required change is applied? if yes, reset system
  //
  SetupResetReminder ();

  //
  // BugBug: This code looks repeated from the BDS. Need to save code space.
  //
  gBS->RaiseTPL (EFI_TPL_DRIVER);
  //
  // parse the selected option
  //
  Status = BdsLibBootViaBootOption (gOption, gOption->DevicePath, &ExitDataSize, &ExitData);

  if (!EFI_ERROR (Status)) {
    PlatformBdsBootSuccess (gOption);
  } else {
    PlatformBdsBootFail (gOption, Status, ExitData, ExitDataSize);

#ifdef ENABLE_CONSOLE_EX
    gBS->HandleProtocol (
                 gST->ConsoleInHandle,
                 &gEfiSimpleTextInputExProtocolGuid,
                 &SimpleTextInEx
                 );
    if (SimpleTextInEx != NULL) {
      SimpleTextInEx->ReadKeyStrokeEx (SimpleTextInEx, &KeyData);
      Key = KeyData.Key;
    } else {
#endif
      gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
#ifdef ENABLE_CONSOLE_EX
    }
#endif
  }
}
