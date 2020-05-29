/** @file
  Usb Bus Peim

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbBus.h"
#include "Hub.h"
#include "UsbHelper.h"

//
// UsbIo PPI interface function
//
STATIC PEI_USB3_IO_PPI        mUsbIoPpi = {
  PeiUsbControlTransfer,
  PeiUsbBulkTransfer,
  PeiUsbSyncInterruptTransfer,
  PeiUsbGetInterfaceDescriptor,
  PeiUsbGetEndpointDescriptor,
  PeiUsbPortReset,
  PeiUsbClearEndpointHalt
};

STATIC EFI_PEI_PPI_DESCRIPTOR mUsbIoPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiUsb3IoPpiGuid,
  NULL
};

//
// Helper functions
//
STATIC
EFI_STATUS
PeiUsbEnumeration (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB3_HOST_CONTROLLER_PPI   *UsbHcPpi
  );

/**

  Initializes the Usb Io PPI

  @param  PeiServices           General purpose services available to every PEIM.
 
  @retval EFI_UNSUPPORTED       Can't find required PPI
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory resource
  @retval EFI_SUCCESS           Success

**/
EFI_STATUS
UsbBusPeimEntry (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  PEI_USB3_HOST_CONTROLLER_PPI *UsbHcPpi;
  //
  // Shadow this PEIM to run from memory
  //
  if (!EFI_ERROR (PeiServicesRegisterForShadow (FileHandle))) {
    return EFI_SUCCESS;
  }
  Index = 0;
  while (TRUE) {
    //
    // Get UsbHcPpi at first.
    //
    Status = (**PeiServices).LocatePpi (
                               PeiServices,
                               &gPeiUsb3HostControllerPpiGuid, // GUID
                               Index,                          // INSTANCE
                               NULL,                           // EFI_PEI_PPI_DESCRIPTOR
                               (VOID **)&UsbHcPpi                       // PPI
                               );
    if (EFI_ERROR (Status)) {
      //
      // No more host controller, break out
      //
      break;
    }
    PeiUsbEnumeration ((EFI_PEI_SERVICES**)PeiServices, UsbHcPpi);
    Index++;
  }
  return EFI_SUCCESS;
}

/**

  Set Transaction Translator parameter

  @param  ParentHub             Parent Hub device
  @param  ParentPort            Number of parent port
  @param  Device                This device

  @retval EFI_SUCCESS           Success
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate resources

**/
STATIC
EFI_STATUS
UsbSetTransactionTranslator (
  IN PEI_USB_DEVICE               *ParentHub,
  IN UINT8                        ParentPort,
  IN OUT PEI_USB_DEVICE           *Device
  )
{
  UINT8 DeviceAddress;
  UINT8 Port;
  UINT8 MultiTT;
  //
  // Inherit Route String from parent
  //
  if (!ParentHub) {
    //
    // Level 1
    //
    Device->Translator.RouteString = 0;
    Device->HubDepth = 0;
  } else {
    //
    // Below Level 1
    //
    Device->Translator.RouteString = ParentHub->Translator.RouteString;  
    Device->HubDepth = ParentHub->HubDepth + 1;
    //
    // Initial the route string for XHC
    //
    if (ParentHub->IsHub && Device->HubDepth != 0) {
      Device->Translator.RouteString &= ~(0x0f << (4 * (Device->HubDepth - 1)));
      Device->Translator.RouteString |= (ParentPort + 1) << (4 * (Device->HubDepth - 1));
    }
  }
  //
  // Inherit RootHub Port Number from parent
  //
  if (Device->HubDepth == 0) {
    //
    // Level 1 device, records the RootHub Port Number
    //
    Device->Translator.RootHubPortNumber = ParentPort + 1;
  } else {
    //
    // Not level 1 device, inherit RootHub Port Number from parent
    //
    Device->Translator.RootHubPortNumber = ParentHub->Translator.RootHubPortNumber;
    if (Device->DeviceSpeed == EFI_USB_SPEED_LOW || Device->DeviceSpeed == EFI_USB_SPEED_FULL) {
      //
      // Only full/low speed device need 
      //
      if (EFI_USB_SPEED_HIGH == ParentHub->DeviceSpeed) {
        //
        // Parent is high speed, then parent is our translator
        //
        DeviceAddress = ParentHub->DeviceAddress;
        Port          = ParentPort + 1;
        MultiTT       = ParentHub->MultiTT;
      } else {
        //
        // Use parent's translator.
        //
        DeviceAddress = ParentHub->Translator.TranslatorHubAddress;
        Port          = ParentHub->Translator.TranslatorPortNumber;
        MultiTT       = ParentHub->Translator.MultiTT;
      }
      Device->Translator.TranslatorHubAddress  = DeviceAddress;
      Device->Translator.TranslatorPortNumber  = Port;
      Device->Translator.MultiTT               = MultiTT;
    }
  }
  return EFI_SUCCESS;
}

/**

  Get the start position of next wanted descriptor.

  @param  Buffer                Buffer containing data to parse
  @param  Length                Buffer length
  @param  DescType              Descriptor type
  @param  DescLength            Descriptor  length
  @param  ParsedBytes           Bytes has been parsed

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
GetExpectedDescriptor (
  IN  UINT8       *Buffer,
  IN  UINTN       Length,
  IN  UINT8       DescType,
  IN  UINT8       DescLength,
  OUT UINTN       *ParsedBytes
  )
{
  UINT16  DescriptorHeader;
  UINT8   Len;
  UINT8   *ptr;
  UINTN   Parsed;

  Parsed  = 0;
  ptr     = Buffer;

  while (TRUE) {
    //
    // Buffer length should not less than Desc length
    //
    if (Length < DescLength) {
      return EFI_DEVICE_ERROR;
    }

    DescriptorHeader  = *ptr + ((*(ptr + 1)) << 8);

    Len               = Buffer[0];

    //
    // Check to see if it is a start of expected descriptor
    //
    if (DescriptorHeader == ((DescType << 8) | DescLength)) {
      break;
    }

    if ((UINT8) (DescriptorHeader >> 8) == DescType) {
      if (Len > DescLength) {
        return EFI_DEVICE_ERROR;
      }
    }
    //
    // Descriptor length should be at least 2
    // and should not exceed the buffer length
    //
    if (Len < 2) {
      return EFI_DEVICE_ERROR;
    }

    if (Len > Length) {
      return EFI_DEVICE_ERROR;
    }
    //
    // Skip this mismatch descriptor
    //
    Length -= Len;
    ptr += Len;
    Parsed += Len;
  }

  *ParsedBytes = Parsed;

  return EFI_SUCCESS;
}

/**

  Pei Usb Get All Configuration

  @param  PeiServices           EFI_PEI_SERVICES
  @param  PeiUsbDevice          PEI_USB_DEVICE

  @retval EFI_SUCCESS           Success

**/
STATIC
EFI_STATUS
PeiUsbGetAllConfiguration (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN PEI_USB_DEVICE     *PeiUsbDevice
  )
{
  EFI_STATUS                Status;
  EFI_USB_CONFIG_DESCRIPTOR *ConfigDesc;
  PEI_USB3_IO_PPI           *UsbIoPpi;
  UINT16                    ConfigDescLength;
  UINT8                     *ptr;
  UINTN                     SkipBytes;
  UINTN                     LengthLeft;
  UINTN                     i;
  UINTN                     NumOfEndpoint;

  UsbIoPpi = &PeiUsbDevice->UsbIoPpi;

  //
  // First get its 4-byte configuration descriptor
  //
  Status = PeiUsbGetDescriptor (
             PeiServices,
             UsbIoPpi,
             0x0200, // Value
             0,      // Index
             4,      // Length
             PeiUsbDevice->ConfigurationData
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ConfigDesc        = (EFI_USB_CONFIG_DESCRIPTOR *) PeiUsbDevice->ConfigurationData;
  ConfigDescLength  = ConfigDesc->TotalLength;

  //
  // Then we get the total descriptors for this configuration
  //
  Status = PeiUsbGetDescriptor (
             PeiServices,
             UsbIoPpi,
             0x0200,
             0,
             ConfigDescLength,
             PeiUsbDevice->ConfigurationData
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Parse this configuration descriptor
  // First get the current config descriptor;
  //
  Status = GetExpectedDescriptor (
             PeiUsbDevice->ConfigurationData,
             ConfigDescLength,
             USB_DESC_TYPE_CONFIG,
             sizeof (EFI_USB_CONFIG_DESCRIPTOR),
             &SkipBytes
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ptr                       = PeiUsbDevice->ConfigurationData + SkipBytes;
  PeiUsbDevice->ConfigDesc  = (EFI_USB_CONFIG_DESCRIPTOR *) ptr;

  ptr += sizeof (EFI_USB_CONFIG_DESCRIPTOR);
  LengthLeft = ConfigDescLength - SkipBytes - sizeof (EFI_USB_CONFIG_DESCRIPTOR);

  //
  // Get the first interface descriptor
  //
  Status = GetExpectedDescriptor (
             ptr,
             LengthLeft,
             USB_DESC_TYPE_INTERFACE,
             sizeof (EFI_USB_INTERFACE_DESCRIPTOR),
             &SkipBytes
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ptr += SkipBytes;
  PeiUsbDevice->InterfaceDesc = (EFI_USB_INTERFACE_DESCRIPTOR *) ptr;

  ptr += sizeof (EFI_USB_INTERFACE_DESCRIPTOR);
  LengthLeft -= SkipBytes;
  LengthLeft -= sizeof (EFI_USB_INTERFACE_DESCRIPTOR);

  //
  // Parse all the endpoint descriptor within this interface
  //
  NumOfEndpoint = PeiUsbDevice->InterfaceDesc->NumEndpoints;
  for (i = 0; i < NumOfEndpoint; i++) {
    //
    // Get the endpoint descriptor
    //
    Status = GetExpectedDescriptor (
               ptr,
               LengthLeft,
               USB_DESC_TYPE_ENDPOINT,
               sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
               &SkipBytes
               );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    ptr += SkipBytes;
    PeiUsbDevice->EndpointDesc[i] = (EFI_USB_ENDPOINT_DESCRIPTOR *) ptr;

    ptr += sizeof (EFI_USB_ENDPOINT_DESCRIPTOR);
    LengthLeft -= SkipBytes;
    LengthLeft -= sizeof (EFI_USB_ENDPOINT_DESCRIPTOR);
  }

  return EFI_SUCCESS;
}

/**

  Pei Configure new detected Usb Device

  @param  PeiServices           EFI_PEI_SERVICES
  @param  PeiUsbDevice          PEI_USB_DEVICE
  @param  Port                  Port number
  @param  DeviceAddress         Device Address

  @retval EFI_SUCCESS           Success

**/
STATIC
EFI_STATUS
PeiConfigureUsbDevice (
  IN PEI_USB_DEVICE     *PeiUsbDevice,
  IN UINT8              Port,
  IN OUT UINT8          *DeviceAddress
  )
{
  EFI_PEI_SERVICES            **PeiServices;
  EFI_USB_DEVICE_DESCRIPTOR   DeviceDescriptor;
  EFI_STATUS                  Status;
  PEI_USB3_IO_PPI             *UsbIoPpi;
  UINT8                       Retry;

  PeiServices = PeiUsbDevice->PeiServices;
  UsbIoPpi    = &PeiUsbDevice->UsbIoPpi;
  Status      = EFI_SUCCESS;
  ZeroMem (&DeviceDescriptor, sizeof (EFI_USB_DEVICE_DESCRIPTOR));
  //
  // Get USB device descriptor
  //

  for (Retry = 0; Retry < 3; Retry ++) {

    PeiUsbDevice->MaxPacketSize0 = 8;

    Status = PeiUsbGetDescriptor (
               PeiServices,
               UsbIoPpi,
               0x0100,
               0,
               8,
               &DeviceDescriptor
               );

    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (Retry == 3) {
    return Status;
  }

  //
  // Stall 1ms after GetDescriptor for specific device compatibility(0.1ms minimum)
  //
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            1000
                            );

  PeiUsbDevice->MaxPacketSize0 = DeviceDescriptor.MaxPacketSize0;

  (*DeviceAddress) ++;
 
  Status = PeiUsbSetDeviceAddress (
             PeiServices,
             UsbIoPpi,
             *DeviceAddress
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  PeiUsbDevice->DeviceAddress = *DeviceAddress;

  //
  // According to spec, stall 10 millisecond after SetAddress
  //
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            10 * 1000
                            );
  //
  // Get whole USB device descriptor
  //
  Status = PeiUsbGetDescriptor (
             PeiServices,
             UsbIoPpi,
             0x0100,
             0,
             sizeof (EFI_USB_DEVICE_DESCRIPTOR),
             &DeviceDescriptor
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get its default configuration and its first interface
  //
  Status = PeiUsbGetAllConfiguration (
             PeiServices,
             PeiUsbDevice
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // According to spec, stall 1 millisecond after GetAllConfiguration
  //
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            1000
                            );

  Status = PeiUsbSetConfiguration (
             PeiServices,
             UsbIoPpi
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**

  The Hub Enumeration just scans the hub ports one time. It also
  doesn't support hot-plug.

  @param  PeiUsbDevice          PEI_USB_DEVICE
  @param  CurrentAddress        DeviceAddress
 
  @retval EFI_UNSUPPORTED       Can't find required PPI
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory resource
  @retval EFI_SUCCESS           Success
  
**/
STATIC
EFI_STATUS
PeiHubEnumeration (
  IN PEI_USB_DEVICE                 *PeiUsbDevice,
  IN UINT8                          *CurrentAddress
  )
{
  UINTN                 i;
  EFI_STATUS            Status;
  PEI_USB3_IO_PPI       *UsbIoPpi;
  EFI_USB_PORT_STATUS   PortStatus;
  BOOLEAN               PortChanged;
  UINTN                 MemPages;
  EFI_PHYSICAL_ADDRESS  AllocateAddress;
  PEI_USB_DEVICE        *NewPeiUsbDevice;
  EFI_PEI_SERVICES      **PeiServices;

  PortChanged = FALSE;
  PeiServices = PeiUsbDevice->PeiServices;
  UsbIoPpi    = &PeiUsbDevice->UsbIoPpi;
  for (i = 0; i < PeiUsbDevice->DownStreamPortNo; i++) {
    Status = PeiHubGetPortStatus (
               PeiServices,
               UsbIoPpi,
               (UINT8) (i + 1),
               (UINT32 *) &PortStatus
               );
    if (EFI_ERROR (Status)) {
      continue;
    }
    if (IsPortConnectChange (PortStatus.PortChangeStatus)) {
      PortChanged = TRUE;
      PeiHubClearPortFeature (
        PeiServices,
        UsbIoPpi,
        (UINT8) (i + 1),
        EfiUsbPortConnectChange
        );
      PeiUsbDevice->StallPpi->Stall (
                                (CONST EFI_PEI_SERVICES **)PeiServices,
                                PeiUsbDevice->StallPpi,
                                100 * 1000
                                );
      if (IsPortConnect (PortStatus.PortStatus)) {
        PeiHubGetPortStatus (
          PeiServices,
          UsbIoPpi,
          (UINT8) (i + 1),
          (UINT32 *) &PortStatus
          );
        //
        // Begin to deal with the new device
        //
        MemPages = sizeof (PEI_USB_DEVICE) / EFI_PAGE_SIZE + 1;
        Status = (*PeiServices)->AllocatePages (
                                   (CONST EFI_PEI_SERVICES **)PeiServices,
                                   EfiBootServicesData,
                                   MemPages,
                                   &AllocateAddress
                                   );
        if (EFI_ERROR (Status)) {
          return EFI_OUT_OF_RESOURCES;
        }
        NewPeiUsbDevice = (PEI_USB_DEVICE *) ((UINTN) AllocateAddress);
        ZeroMem (NewPeiUsbDevice, sizeof (PEI_USB_DEVICE));
        NewPeiUsbDevice->Signature        = PEI_USB_DEVICE_SIGNATURE;
        NewPeiUsbDevice->PeiServices      = PeiServices;
        NewPeiUsbDevice->StallPpi         = PeiUsbDevice->StallPpi;
        NewPeiUsbDevice->DeviceAddress    = 0;
        NewPeiUsbDevice->MaxPacketSize0   = 8;
        NewPeiUsbDevice->DataToggle       = 0;
        NewPeiUsbDevice->UsbIoPpi         = mUsbIoPpi;
        NewPeiUsbDevice->UsbIoPpiList     = mUsbIoPpiList;
        NewPeiUsbDevice->UsbIoPpiList.Ppi = &NewPeiUsbDevice->UsbIoPpi;
        NewPeiUsbDevice->AllocateAddress  = (UINTN) AllocateAddress;
        NewPeiUsbDevice->UsbHcPpi         = PeiUsbDevice->UsbHcPpi;
        NewPeiUsbDevice->DeviceSpeed      = EFI_USB_SPEED_FULL;
        NewPeiUsbDevice->IsHub            = 0x0;
        NewPeiUsbDevice->DownStreamPortNo = 0x0;

        ResetHubPort (PeiUsbDevice, (UINT8)(i + 1));

        //
        // Get port status again due to it will be changed by hub port reset
        //
        PeiHubGetPortStatus (
          PeiServices,
          UsbIoPpi,
          (UINT8) (i + 1),
          (UINT32 *) &PortStatus
          );
        if (PortStatus.PortStatus & USB_PORT_STAT_LOW_SPEED) {
          NewPeiUsbDevice->DeviceSpeed = EFI_USB_SPEED_LOW;
        } else if (PortStatus.PortStatus & USB_PORT_STAT_HIGH_SPEED) {
          NewPeiUsbDevice->DeviceSpeed = EFI_USB_SPEED_HIGH;
        } else if (PortStatus.PortStatus & USB_PORT_STAT_SUPER_SPEED) {
          NewPeiUsbDevice->DeviceSpeed = EFI_USB_SPEED_SUPER;
        } else {
          NewPeiUsbDevice->DeviceSpeed = EFI_USB_SPEED_FULL;
        }
        UsbSetTransactionTranslator (
          PeiUsbDevice,
          (UINT8)i,
          NewPeiUsbDevice
          );
        //
        // Configure that Usb Device
        //
        Status = PeiConfigureUsbDevice (
                   NewPeiUsbDevice,
                   (UINT8) (i + 1),
                   CurrentAddress
                   );

        if (EFI_ERROR (Status)) {
          continue;
        }
        Status = (**PeiServices).InstallPpi (
                                   (CONST EFI_PEI_SERVICES **)PeiServices,
                                   &NewPeiUsbDevice->UsbIoPpiList
                                   );
        if (NewPeiUsbDevice->InterfaceDesc->InterfaceClass == CLASS_CODE_HUB) {
          NewPeiUsbDevice->IsHub  = 0x1;
          Status                  = DoHubConfig (NewPeiUsbDevice);
          if (EFI_ERROR (Status)) {
            return Status;
          }
          PeiHubEnumeration (NewPeiUsbDevice, CurrentAddress);
        }
      }
    }
  }
  return EFI_SUCCESS;
}

/**

  Enumeration routine to detect device change

  @param  PeiServices           EFI_PEI_SERVICES
  @param  UsbHcPpi              PEI_USB3_HOST_CONTROLLER_PPI

  @retval EFI_UNSUPPORTED       Can't find required PPI
  @retval EFI_OUT_OF_RESOURCES  Can't allocate memory resource
  @retval EFI_SUCCESS           Success

**/
STATIC
EFI_STATUS
PeiUsbEnumeration (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_USB3_HOST_CONTROLLER_PPI   *UsbHcPpi
  )
{
  UINT8                 NumOfRootPort;
  EFI_STATUS            Status;
  UINT8                 i;
  EFI_USB_PORT_STATUS   PortStatus;
  PEI_USB_DEVICE        *PeiUsbDevice;
  UINTN                 MemPages;
  EFI_PHYSICAL_ADDRESS  AllocateAddress;
  UINT8                 CurrentAddress;
  EFI_PEI_STALL_PPI     *StallPpi;

  (**PeiServices).LocatePpi (
                    (CONST EFI_PEI_SERVICES **)PeiServices,
                    &gEfiPeiStallPpiGuid,
                    0,
                    NULL,
                    (VOID **)&StallPpi
                    );

  CurrentAddress = 0;
  UsbHcPpi->GetRootHubPortNumber (
              PeiServices,
              UsbHcPpi,
              (UINT8 *) &NumOfRootPort
              );

  for (i = 0; i < NumOfRootPort; i++) {
    //
    // First get root port status to detect changes happen
    //
    UsbHcPpi->GetRootHubPortStatus (
                PeiServices,
                UsbHcPpi,
                (UINT8) i,
                &PortStatus
                );

    if (IsPortConnectChange (PortStatus.PortChangeStatus)) {
      //
      // Changes happen, first clear this change status
      //
      UsbHcPpi->ClearRootHubPortFeature (
                  PeiServices,
                  UsbHcPpi,
                  (UINT8) i,
                  EfiUsbPortConnectChange
                  );
      StallPpi->Stall (
                  (CONST EFI_PEI_SERVICES **)PeiServices,
                  StallPpi,
                  100 * 1000
                  );
      if (IsPortConnect (PortStatus.PortStatus)) {
        UsbHcPpi->GetRootHubPortStatus (
                    PeiServices,
                    UsbHcPpi,
                    (UINT8) i,
                    &PortStatus
                    );
        //
        // Connect change happen
        //
        MemPages = sizeof (PEI_USB_DEVICE) / EFI_PAGE_SIZE + 1;
        Status = (*PeiServices)->AllocatePages (
                                   (CONST EFI_PEI_SERVICES **)PeiServices,
                                   EfiBootServicesData,
                                   MemPages,
                                   &AllocateAddress
                                   );
        if (EFI_ERROR (Status)) {
          return EFI_OUT_OF_RESOURCES;
        }
        PeiUsbDevice = (PEI_USB_DEVICE *) ((UINTN) AllocateAddress);
        ZeroMem (PeiUsbDevice, sizeof (PEI_USB_DEVICE));
        PeiUsbDevice->Signature         = PEI_USB_DEVICE_SIGNATURE;
        PeiUsbDevice->PeiServices       = PeiServices;
        PeiUsbDevice->StallPpi          = StallPpi;
        PeiUsbDevice->DeviceAddress     = 0;
        PeiUsbDevice->MaxPacketSize0    = 8;
        PeiUsbDevice->DataToggle        = 0;
        PeiUsbDevice->UsbIoPpi          = mUsbIoPpi;
        PeiUsbDevice->UsbIoPpiList      = mUsbIoPpiList;
        PeiUsbDevice->UsbIoPpiList.Ppi  = &PeiUsbDevice->UsbIoPpi;
        PeiUsbDevice->AllocateAddress   = (UINTN) AllocateAddress;
        PeiUsbDevice->UsbHcPpi          = UsbHcPpi;
        PeiUsbDevice->DeviceSpeed       = EFI_USB_SPEED_FULL;
        PeiUsbDevice->IsHub             = 0x0;
        PeiUsbDevice->DownStreamPortNo  = 0x0;
        
        //
        // Configure that Usb Device
        //
        ResetRootPort (PeiUsbDevice, i);

        //
        // Get port status again due to it will be changed by port reset
        //
        UsbHcPpi->GetRootHubPortStatus (
                    PeiServices,
                    UsbHcPpi,
                    (UINT8) i,
                    &PortStatus
                    );
        //
        // Re-enumerate the port due to XHCI may route the port to USB 3.0 after port reset
        //
        if (!IsPortConnect (PortStatus.PortStatus)) {
          i = 0xff;
          continue;
        }
        if (PortStatus.PortStatus & USB_PORT_STAT_LOW_SPEED) {
          PeiUsbDevice->DeviceSpeed = EFI_USB_SPEED_LOW;
        } else if (PortStatus.PortStatus & USB_PORT_STAT_HIGH_SPEED && PortStatus.PortStatus & USB_PORT_STAT_ENABLE) {
          PeiUsbDevice->DeviceSpeed = EFI_USB_SPEED_HIGH;
        } else if (PortStatus.PortStatus & USB_PORT_STAT_SUPER_SPEED) {
          PeiUsbDevice->DeviceSpeed = EFI_USB_SPEED_SUPER;
        } else {
          PeiUsbDevice->DeviceSpeed = EFI_USB_SPEED_FULL;
        }
        UsbSetTransactionTranslator (
          NULL,
          i,
          PeiUsbDevice
          );
        Status = PeiConfigureUsbDevice (
                   PeiUsbDevice,
                   i,
                   &CurrentAddress
                   );

        if (EFI_ERROR (Status)) {
          continue;
        }

        Status = (**PeiServices).InstallPpi (
                                  (CONST EFI_PEI_SERVICES **)PeiServices,
                                  &PeiUsbDevice->UsbIoPpiList
                                  );

        if (PeiUsbDevice->InterfaceDesc->InterfaceClass == CLASS_CODE_HUB) {
          PeiUsbDevice->IsHub = 0x1;
          Status              = DoHubConfig (PeiUsbDevice);
          if (EFI_ERROR (Status)) {
            return Status;
          }
          PeiHubEnumeration (PeiUsbDevice, &CurrentAddress);
        }
      }
    }
  }
  return EFI_SUCCESS;
}

/**

  Reset RootPort

  @param  PeiUsbDevice          PEI_USB_DEVICE
  @param  PortNum               PortNum
  @param  RetryIndex            Retry times

**/
VOID
ResetRootPort (
  IN PEI_USB_DEVICE                 *PeiUsbDevice,
  IN UINT8                          PortNum
  )
{
  EFI_STATUS                    Status;
  EFI_PEI_SERVICES              **PeiServices;
  PEI_USB3_HOST_CONTROLLER_PPI  *UsbHcPpi;
  
  PeiServices = PeiUsbDevice->PeiServices;
  UsbHcPpi    = PeiUsbDevice->UsbHcPpi;
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            200 * 1000
                            );
  //
  // reset root port
  //
  Status = UsbHcPpi->SetRootHubPortFeature (
                       PeiServices,
                       UsbHcPpi,
                       PortNum,
                       EfiUsbPortReset
                       );
 
  if (EFI_ERROR (Status)) {
    return;
  }
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            200 * 1000
                            );
  //
  // clear reset root port
  //
  Status = UsbHcPpi->ClearRootHubPortFeature (
                       PeiServices,
                       UsbHcPpi,
                       PortNum,
                       EfiUsbPortReset
                       );
 
  if (EFI_ERROR (Status)) {
    return;
  }
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            1 * 1000
                            );
  UsbHcPpi->ClearRootHubPortFeature (
              PeiServices,
              UsbHcPpi,
              PortNum,
              EfiUsbPortConnectChange
              );
  //
  // Set port enable
  //
  UsbHcPpi->SetRootHubPortFeature(
              PeiServices,
              UsbHcPpi,
              PortNum,
              EfiUsbPortEnable
              );
 
  UsbHcPpi->ClearRootHubPortFeature (
              PeiServices,
              UsbHcPpi,
              PortNum,
              EfiUsbPortEnableChange
              );
 
  PeiUsbDevice->StallPpi->Stall (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            PeiUsbDevice->StallPpi,
                            50 * 1000
                            );
}
