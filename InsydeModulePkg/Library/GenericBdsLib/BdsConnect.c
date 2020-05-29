/** @file
 BDS Lib functions which relate with connect the device

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#include "InternalBdsLib.h"

/**
  This function will connect all the system driver to controller
  first, and then special connect the default console, this make
  sure all the system controller available and the platform default
  console connected.

**/
VOID
EFIAPI
BdsLibConnectAll (
  VOID
  )
{
  //
  // Connect the platform console first
  //
  BdsLibConnectAllDefaultConsoles ();

  //
  // Generic way to connect all the drivers
  //
  BdsLibConnectAllDriversToAllControllers ();

  //
  // Here we have the assumption that we have already had
  // platform default console
  //
  BdsLibConnectAllDefaultConsoles ();
}


/**
  This function will connect all the system drivers to all controllers
  first, and then connect all the console devices the system current
  have. After this we should get all the device work and console available
  if the system have console device.

**/
VOID
BdsLibGenericConnectAll (
  VOID
  )
{

  //
  // Most generic way to connect all the drivers
  //
  BdsLibConnectAllDriversToAllControllers ();
  BdsLibConnectAllConsoles ();
}


/**
  This function will create all handles associate with every device
  path node. If the handle associate with one device path node can not
  be created success, then still give one chance to do the dispatch,
  which load the missing drivers if possible.

  @param  DevicePathToConnect   The device path which will be connected, it can be
                                a multi-instance device path

  @retval EFI_SUCCESS           All handles associate with every device path  node
                                have been created
  @retval EFI_OUT_OF_RESOURCES  There is no resource to create new handles
  @retval EFI_NOT_FOUND         Create the handle associate with one device  path
                                node failed

**/
EFI_STATUS
EFIAPI
BdsLibConnectDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePathToConnect
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *CopyOfDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  EFI_DEVICE_PATH_PROTOCOL  *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Next;
  EFI_HANDLE                Handle;
  EFI_HANDLE                PreviousHandle;
  UINTN                     Size;

  if (DevicePathToConnect == NULL) {
    return EFI_SUCCESS;
  }

  DevicePath        = DuplicateDevicePath (DevicePathToConnect);
  if (DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyOfDevicePath  = DevicePath;

  do {
    //
    // The outer loop handles multi instance device paths.
    // Only console variables contain multiple instance device paths.
    //
    // After this call DevicePath points to the next Instance
    //
    Instance  = GetNextDevicePathInstance (&DevicePath, &Size);
    if (Instance == NULL) {
      FreePool (CopyOfDevicePath);
      return EFI_OUT_OF_RESOURCES;
    }

    Next      = Instance;
    while (!IsDevicePathEndType (Next)) {
      Next = NextDevicePathNode (Next);
    }

    SetDevicePathEndNode (Next);

    //
    // Start the real work of connect with RemainingDevicePath
    //
    PreviousHandle = NULL;
    do {
      //
      // Find the handle that best matches the Device Path. If it is only a
      // partial match the remaining part of the device path is returned in
      // RemainingDevicePath.
      //
      RemainingDevicePath = Instance;
      Status              = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &RemainingDevicePath, &Handle);

      if (!EFI_ERROR (Status)) {
        if (Handle == PreviousHandle) {
          //
          // If no forward progress is made try invoking the Dispatcher.
          // A new FV may have been added to the system an new drivers
          // may now be found.
          // Status == EFI_SUCCESS means a driver was dispatched
          // Status == EFI_NOT_FOUND means no new drivers were dispatched
          //
          Status = gDS->Dispatch ();
        }

        if (!EFI_ERROR (Status)) {
          PreviousHandle = Handle;
          //
          // Connect all drivers that apply to Handle and RemainingDevicePath,
          // the Recursive flag is FALSE so only one level will be expanded.
          //
          // Do not check the connect status here, if the connect controller fail,
          // then still give the chance to do dispatch, because partial
          // RemainingDevicepath may be in the new FV
          //
          // 1. If the connect fail, RemainingDevicepath and handle will not
          //    change, so next time will do the dispatch, then dispatch's status
          //    will take effect
          // 2. If the connect success, the RemainingDevicepath and handle will
          //    change, then avoid the dispatch, we have chance to continue the
          //    next connection
          //
          gBS->ConnectController (Handle, NULL, RemainingDevicePath, FALSE);
        }
      }
      //
      // Loop until RemainingDevicePath is an empty device path
      //
    } while (!EFI_ERROR (Status) && !IsDevicePathEnd (RemainingDevicePath));

  } while (DevicePath != NULL);

  if (CopyOfDevicePath != NULL) {
    FreePool (CopyOfDevicePath);
  }
  //
  // All handle with DevicePath exists in the handle database
  //
  return Status;
}


/**
  This function will connect all current system handles recursively.

  gBS->ConnectController() service is invoked for each handle exist in system handler buffer.
  If the handle is bus type handler, all childrens also will be connected recursively
  by gBS->ConnectController().

  @retval EFI_SUCCESS           All handles and it's child handle have been connected
  @retval EFI_STATUS            Error status returned by of gBS->LocateHandleBuffer().

**/
EFI_STATUS
EFIAPI
BdsLibConnectAllEfi (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN       HandleCount;
  EFI_HANDLE  *HandleBuffer;
  UINTN       Index;

  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE);
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
  return EFI_SUCCESS;
}

/**
  This function will disconnect all current system handles.

  gBS->DisconnectController() is invoked for each handle exists in system handle buffer.
  If handle is a bus type handle, all childrens also are disconnected recursively by
  gBS->DisconnectController().

  @retval EFI_SUCCESS           All handles have been disconnected
  @retval EFI_STATUS            Error status returned by of gBS->LocateHandleBuffer().

**/
EFI_STATUS
EFIAPI
BdsLibDisconnectAllEfi (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN       HandleCount;
  EFI_HANDLE  *HandleBuffer;
  UINTN       Index;

  //
  // Disconnect all
  //
  Status = gBS->LocateHandleBuffer (
                  AllHandles,
                  NULL,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->DisconnectController (HandleBuffer[Index], NULL, NULL);
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  return EFI_SUCCESS;
}


/**
  Connects all drivers to all controllers.
  This function make sure all the current system driver will manage
  the correspoinding controllers if have. And at the same time, make
  sure all the system controllers have driver to manage it if have.

**/
VOID
EFIAPI
BdsLibConnectAllDriversToAllControllers (
  VOID
  )
{
  EFI_STATUS  Status;

  do {
    //
    // Connect All EFI 1.10 drivers following EFI 1.10 algorithm
    //
    BdsLibConnectAllEfi ();

    //
    // Check to see if it's possible to dispatch an more DXE drivers.
    // The BdsLibConnectAllEfi () may have made new DXE drivers show up.
    // If anything is Dispatched Status == EFI_SUCCESS and we will try
    // the connect again.
    //
    Status = gDS->Dispatch ();

  } while (!EFI_ERROR (Status));
}


/**
  Connect the specific Usb device which match the short form device path,
  and whose bus is determined by Host Controller (Uhci or Ehci).

  @param  HostControllerPI      Uhci (0x00) or Ehci (0x20) or Both uhci and ehci
                                (0xFF)
  @param  RemainingDevicePath   a short-form device path that starts with the first
                                element  being a USB WWID or a USB Class device
                                path

  @return EFI_INVALID_PARAMETER  RemainingDevicePath is NULL pointer.
                                 RemainingDevicePath is not a USB device path.
                                 Invalid HostControllerPI type.
  @return EFI_SUCCESS            Success to connect USB device
  @return EFI_NOT_FOUND          Fail to find handle for USB controller to connect.

**/
EFI_STATUS
EFIAPI
BdsLibConnectUsbDevByShortFormDP(
  IN UINT8                      HostControllerPI,
  IN EFI_DEVICE_PATH_PROTOCOL   *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *HandleArray;
  UINTN                                 HandleArrayCount;
  UINTN                                 Index;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  UINT8                                 Class[3];
  BOOLEAN                               AtLeastOneConnected;

  //
  // Check the passed in parameters
  //
  if (RemainingDevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((DevicePathType (RemainingDevicePath) != MESSAGING_DEVICE_PATH) ||
      ((DevicePathSubType (RemainingDevicePath) != MSG_USB_CLASS_DP)
      && (DevicePathSubType (RemainingDevicePath) != MSG_USB_WWID_DP)
      )) {
    return EFI_INVALID_PARAMETER;
  }

  if (HostControllerPI != 0xFF &&
      HostControllerPI != 0x00 &&
      HostControllerPI != 0x20) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find the usb host controller firstly, then connect with the remaining device path
  //
  AtLeastOneConnected = FALSE;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleArrayCount,
                  &HandleArray
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < HandleArrayCount; Index++) {
      Status = gBS->HandleProtocol (
                      HandleArray[Index],
                      &gEfiPciIoProtocolGuid,
                      (VOID **)&PciIo
                      );
      if (!EFI_ERROR (Status)) {
        //
        // Check whether the Pci device is the wanted usb host controller
        //
        Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 0x09, 3, &Class);
        if (!EFI_ERROR (Status)) {
          if ((PCI_CLASS_SERIAL == Class[2]) &&
              (PCI_CLASS_SERIAL_USB == Class[1])) {
            if (HostControllerPI == Class[0] || HostControllerPI == 0xFF) {
              Status = gBS->ConnectController (
                              HandleArray[Index],
                              NULL,
                              RemainingDevicePath,
                              FALSE
                              );
              if (!EFI_ERROR(Status)) {
                AtLeastOneConnected = TRUE;
              }
            }
          }
        }
      }
    }

    if (AtLeastOneConnected) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


/**
  Connect USB HID to all controller

**/
VOID
BdsLibConnectUsbHID (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINTN                 Index;
  UINTN                 HandleCount;
  EFI_HANDLE            *HandleBuffer;
  UINT8                 UsbClassCReg[3];
  LIST_ENTRY            ConnectList[3];
  LIST_ENTRY            *Handle;
  LIST_ENTRY            *List;
  STATIC UINT8          UsbHidDevicePath[sizeof(USB_DEVICE_PATH) + sizeof(USB_CLASS_DEVICE_PATH) + END_DEVICE_PATH_LENGTH] = {
                          MESSAGING_DEVICE_PATH, MSG_USB_DP                    , sizeof(USB_DEVICE_PATH)      , 0, 0, 0,
                          MESSAGING_DEVICE_PATH, MSG_USB_CLASS_DP              , sizeof(USB_CLASS_DEVICE_PATH), 0, 0, 0, 0, 0, 3, 0, 0,
                          END_DEVICE_PATH_TYPE , END_ENTIRE_DEVICE_PATH_SUBTYPE, END_DEVICE_PATH_LENGTH       , 0
                        };

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  InitializeListHead (&ConnectList[0]);
  InitializeListHead (&ConnectList[1]);
  InitializeListHead (&ConnectList[2]);

  //
  // Enumerate PciIo to collect USB devices
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    if (EFI_ERROR (Status)) {
      return;
    }
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint8,
                          9,
                          3,
                          UsbClassCReg
                          );
    if (EFI_ERROR (Status)) {
      return;
    }

    //
    // Test whether the controller belongs to USB type
    //
    if (UsbClassCReg[2] == PCI_CLASS_SERIAL && UsbClassCReg[1] == PCI_CLASS_SERIAL_USB) {
      Status = gBS->AllocatePool (
                      EfiBootServicesData,
                      sizeof (LIST_ENTRY) + sizeof (EFI_HANDLE),
                      (VOID **) &Handle
                      );
      if (EFI_ERROR (Status)) {
        return;
      }

      //
      // Sort the USB HC connection sequence as 3.0->2.0->1.1
      //
      *(EFI_HANDLE*) (Handle + 1) = HandleBuffer[Index];

      if (UsbClassCReg[0] == 0x30) {
        List = &ConnectList[0];
      } else if (UsbClassCReg[0] == 0x20) {
        List = &ConnectList[1];
      } else {
        List = &ConnectList[2];
      }

      InsertTailList (List, Handle);
    }
  }

  gBS->FreePool (HandleBuffer);

  for (Index = 0; Index < 3; Index ++) {
    while (!IsListEmpty (&ConnectList[Index])) {
      Handle = GetFirstNode (&ConnectList[Index]);
      gBS->ConnectController (
             *(EFI_HANDLE*) (Handle + 1),
             NULL,
             (EFI_DEVICE_PATH_PROTOCOL*) UsbHidDevicePath,
             FALSE
             );
      RemoveEntryList (Handle);
      gBS->FreePool (Handle);
    }
  }
}


/**
  Connet all of legacy option ROMs

**/
VOID
BdsLibConnectLegacyRoms (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios;

  //
  // PostCode = 0x26, Dispatch option ROMs
  //
  POST_CODE (BDS_CONNECT_LEGACY_ROM);
  if (BdsLibGetBootType () == EFI_BOOT_TYPE) {
    return;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (LegacyBios != NULL) {
    LegacyBios->ShadowAllLegacyOproms (LegacyBios);
  }

  return;
}

/**
 Connect all SATA controller.

 @retval EFI_SUCCESS  Connect SATA controller successfully.
 @retval Other        Connect SATA controller failed.
**/
EFI_STATUS
EFIAPI
BdsLibConnectSataController (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleIndex;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  UINTN                                 Device;
  UINTN                                 Seg;
  UINTN                                 Bus;
  UINTN                                 Function;
  UINT8                                 SataClassCReg[3];

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (Status != EFI_SUCCESS) {
      continue;
    }
      
    Status = PciIo->GetLocation (
                      PciIo,
                      &Seg,
                      &Bus,
                      &Device,
                      &Function
                      );
    if (Status != EFI_SUCCESS || 
        (!IsOnBoardPciDevice((UINT32)Bus, (UINT32)Device, (UINT32)Function) && PciIo->RomImage != NULL)) {
      continue;
    }

    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint8,
                          PCI_CLASSCODE_OFFSET,
                          3,
                          SataClassCReg
                          );
    if (Status != EFI_SUCCESS) {
      continue;
    }

    //
    // Test whether the controller belongs to IDE, AHCI or RAID type
    //
    if (SataClassCReg[2] == PCI_CLASS_MASS_STORAGE &&
        ((SataClassCReg[1] == PCI_CLASS_MASS_STORAGE_IDE) ||
         (SataClassCReg[1] == PCI_CLASS_MASS_STORAGE_SATADPA && SataClassCReg[0] == PCI_IF_MASS_STORAGE_AHCI) ||
         (SataClassCReg[1] == PCI_CLASS_MASS_STORAGE_RAID))
       ) {
      gBS->ConnectController (
             HandleBuffer[HandleIndex],
             NULL,
             NULL,
             FALSE
             );
    }
  }

  gBS->FreePool (HandleBuffer);

  return Status;
}

/**
  Connect target boot device which is first boot option in BootOrder.

  @retval EFI_SUCCESS     Connect target device success.
  @retval EFI_NOT_FOUND   BootOrder or Boot#### variable does not exist.

**/
EFI_STATUS
BdsLibConnectTargetDev (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT16                                *BootOrder;
  UINTN                                 Size;
  UINT16                                BootOption[BOOT_OPTION_MAX_CHAR];
  UINT8                                 *BootOptionVar;
  UINT8                                 *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL              *OptionDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *FullDevicePath;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL     *HddPasswordService;

  FullDevicePath = NULL;
  BlockIoDevicePath = NULL;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &Size
                );
  if (BootOrder == NULL) {
    return EFI_NOT_FOUND;
  }

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[0]);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    &Size
                    );
  if (BootOptionVar == NULL) {
    gBS->FreePool (BootOrder);
    return EFI_NOT_FOUND;
  }


  TempPtr = BootOptionVar;
  TempPtr += sizeof (UINT32) + sizeof (UINT16);
  TempPtr += StrSize ((CHAR16 *) TempPtr);
  OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

  if ((DevicePathType (OptionDevicePath) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (OptionDevicePath) == MEDIA_HARDDRIVE_DP)) {
    BlockIoDevicePath = BdsLibGetVariableAndSize (
                          L"TargetHddDevPath",
                          &gEfiGenericVariableGuid,
                          &Size
                          );
    if (BlockIoDevicePath != NULL &&
        MatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath)) {
      //
      // Append the file path infomration
      //
      DevicePath = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) OptionDevicePath);
      FullDevicePath = AppendDevicePath (BlockIoDevicePath, DevicePath);
    }
  }

  if (FullDevicePath == NULL) {
    FullDevicePath = OptionDevicePath;
  }

  BdsLibConnectDevicePath (FullDevicePath);

  if (FullDevicePath != OptionDevicePath) {
    gBS->FreePool (FullDevicePath);
  }
  if (BlockIoDevicePath != NULL) {
    gBS->FreePool (BlockIoDevicePath);
  }
  gBS->FreePool (BootOrder);
  gBS->FreePool (BootOptionVar);

  Status = gBS->LocateProtocol (
                  &gEfiHddPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **)&HddPasswordService
                  );
  if (Status == EFI_SUCCESS && HddPasswordService != NULL) {
    BdsLibConnectSataController ();
  }

  return EFI_SUCCESS;
}

/**
 Notify function to perform USB connection.

 @param[in] Event     The Event this notify function registered to.
 @param[in] Context   Pointer to the context data registered to the Event.
**/
VOID
EFIAPI
BdsLibConnectUsbHIDNotifyFunc (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  gBS->CloseEvent (Event);

  BdsLibConnectUsbHID ();
}

/**
 Register USB enumeration protocol event.
 User can install protocol to trigger event to do USB connection.

 @retval EFI_SUCCESS  Notify register success.
 @retval Other        Notify register fail.
**/
EFI_STATUS
EFIAPI
BdsLibConnectUsbHIDNotifyRegister (
  VOID
  )
{
  EFI_STATUS              Status;
  EFI_EVENT               Event;
  VOID                    *Registration;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  BdsLibConnectUsbHIDNotifyFunc,
                  NULL,
                  &Event
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiUsbEnumerationGuid,
                    Event,
                    &Registration
                    );
  }

  return Status;
}

