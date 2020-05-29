/** @file
  Helper functions for USB

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
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

/**

  Get the start position of next wanted descriptor.
  
  @param  Buffer                Buffer to parse
  @param  Length                Buffer length 
  @param  DescType              Descriptor type 
  @param  DescLength            Descriptor length
  @param  ParsedBytes           Parsed Bytes to return

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
GetExpectedDescriptor (
  IN  UINT8                             *Buffer,
  IN  UINTN                             Length,
  IN  UINT8                             DescType,
  IN  UINT8                             DescLength,
  OUT UINTN                             *ParsedBytes
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
    //
    // DescriptorHeader = *((UINT16 *)ptr), compatible with IPF
    //
    DescriptorHeader  = (UINT16) ((*(ptr + 1) << 8) | *ptr);

    Len               = ptr[0];

    //
    // Check to see if it is a start of expected descriptor
    //
    if ((UINT8) (DescriptorHeader >> 8) == DescType) {
      if (Len > DescLength && DescType != USB_DESC_TYPE_ENDPOINT && DescType != USB_DESC_TYPE_HID) {
        return EFI_DEVICE_ERROR;
      }
      break;
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

  Get the start position of next wanted endpoint descriptor.

  @param  EndpointEntry         ENDPOINT_DESC_LIST_ENTRY
  @param  Buffer                Buffer to parse 
  @param  BufferLength          Buffer Length
  @param  ParsedBytes           Parsed Bytes to return

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
ParseThisEndpoint (
  IN  ENDPOINT_DESC_LIST_ENTRY          *EndpointEntry,
  IN  UINT8                             *Buffer,
  IN  UINTN                             BufferLength,
  OUT UINTN                             *ParsedBytes
  )
{
  UINT8       *ptr;
  EFI_STATUS  Status;
  UINTN       SkipBytes;

  //
  // Skip some data for this interface
  //
  Status = GetExpectedDescriptor (
            Buffer,
            BufferLength,
            USB_DESC_TYPE_ENDPOINT,
            sizeof (EFI_USB_ENDPOINT_DESCRIPTOR),
            &SkipBytes
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ptr           = Buffer + SkipBytes;
  *ParsedBytes  = SkipBytes;

  CopyMem (
    &EndpointEntry->EndpointDescriptor,
    ptr,
    sizeof (EFI_USB_ENDPOINT_DESCRIPTOR)
    );

  *ParsedBytes += *ptr;

  return EFI_SUCCESS;
}

/**

  Get the start position of next wanted interface descriptor.

  @param  InterfaceEntry        INTERFACE_DESC_LIST_ENTRY
  @param  Buffer                Buffer to parse 
  @param  BufferLength          Buffer Length
  @param  ParsedBytes           Parsed Bytes to return

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
ParseThisInterface (
  IN  INTERFACE_DESC_LIST_ENTRY         *InterfaceEntry,
  IN  UINT8                             *Buffer,
  IN  UINTN                             *BufferLen,
  OUT UINTN                             *ParsedBytes
  )
{
  UINT8                     *ptr;
  UINTN                     SkipBytes;
  UINTN                     Index;
  UINTN                     Length;
  UINTN                     Parsed;
  ENDPOINT_DESC_LIST_ENTRY  *EndpointEntry;
  EFI_STATUS                Status;
  
  Parsed = 0;

  //
  // Skip some data for this interface
  //
  Status = GetExpectedDescriptor (
            Buffer,
            *BufferLen,
            USB_DESC_TYPE_INTERFACE,
            sizeof (EFI_USB_INTERFACE_DESCRIPTOR),
            &SkipBytes
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ptr           = Buffer + SkipBytes;
  *ParsedBytes  = SkipBytes;

  //
  // Copy the interface descriptor
  //
  CopyMem (
    &InterfaceEntry->InterfaceDescriptor,
    ptr,
    sizeof (EFI_USB_INTERFACE_DESCRIPTOR)
    );

  ptr          += sizeof (EFI_USB_INTERFACE_DESCRIPTOR);
  *ParsedBytes += sizeof (EFI_USB_INTERFACE_DESCRIPTOR);

  InitializeListHead (&InterfaceEntry->EndpointDescListHead);

  Length = *BufferLen - SkipBytes - sizeof (EFI_USB_INTERFACE_DESCRIPTOR);

  if (InterfaceEntry->InterfaceDescriptor.InterfaceClass == CLASS_HID) {
    //
    // Extract HID descriptor if available
    //
    Status = GetExpectedDescriptor (
               Buffer,
               *BufferLen,
                USB_DESC_TYPE_HID,
               sizeof (EFI_USB_HID_DESCRIPTOR),
               &SkipBytes
               );
    if (!EFI_ERROR (Status)) {
      //
      // Copy the HID descriptor
      //
      CopyMem (
        &InterfaceEntry->HidDescriptor,
        Buffer + SkipBytes,
        *(UINT8*)(Buffer + SkipBytes)
        );
    }
  }
  for (Index = 0; Index < InterfaceEntry->InterfaceDescriptor.NumEndpoints; Index++) {
    
    Status = mPrivate->UsbCore->AllocateBuffer(
                                  sizeof (ENDPOINT_DESC_LIST_ENTRY),
                                  ALIGNMENT_32,
                                  (VOID **)&EndpointEntry
                                  );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    
    //
    // Parses all the endpoint descriptors within this interface.
    //
    Status = ParseThisEndpoint (EndpointEntry, ptr, Length, &Parsed);

    if (EFI_ERROR (Status)) {
      mPrivate->UsbCore->FreeBuffer (
                           sizeof (ENDPOINT_DESC_LIST_ENTRY),
                           EndpointEntry
                           );
      return Status;
    }

    InsertTailList (
      &InterfaceEntry->EndpointDescListHead,
      &EndpointEntry->Link
      );
    //
    // Setup Smm address convert table for Smm security policy
    //
    mPrivate->UsbCore->InsertAddressConvertTable (
                         ACT_INSTANCE_BODY,
                         EndpointEntry,
                         sizeof (ENDPOINT_DESC_LIST_ENTRY)
                         );
    Length -= Parsed;
    ptr += Parsed;
    *ParsedBytes += Parsed;
  }

  return EFI_SUCCESS;
}

/**

  Parse the current configuration descriptior.

  @param  ConfigDescEntry       CONFIG_DESC_LIST_ENTRY
  @param  Buffer                Buffer to parse 
  @param  Length                Buffer Length

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
STATIC
EFI_STATUS
ParseThisConfig (
  IN CONFIG_DESC_LIST_ENTRY             *ConfigDescEntry,
  IN UINT8                              *Buffer,
  IN UINTN                              Length
  )
{
  UINT8                     *ptr;
  UINT8                     NumInterface;
  UINT8                     ActualInterfaces;
  INTERFACE_DESC_LIST_ENTRY *InterfaceEntry;
  UINTN                     SkipBytes;
  INTN                      Parsed;
  EFI_STATUS                Status;
  INTN                      LengthLeft;

  Parsed = 0;
  //
  //  First skip the current config descriptor;
  //
  Status = GetExpectedDescriptor (
            Buffer,
            Length,
            USB_DESC_TYPE_CONFIG,
            sizeof (EFI_USB_CONFIG_DESCRIPTOR),
            &SkipBytes
            );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ptr = Buffer + SkipBytes;
  ActualInterfaces = 0;
  CopyMem (
    &ConfigDescEntry->CongfigDescriptor,
    ptr,
    sizeof (EFI_USB_CONFIG_DESCRIPTOR)
    );

  NumInterface = ConfigDescEntry->CongfigDescriptor.NumInterfaces;

  //
  // Skip size of Configuration Descriptor
  //
  ptr += sizeof (EFI_USB_CONFIG_DESCRIPTOR);

  LengthLeft = Length - SkipBytes - sizeof (EFI_USB_CONFIG_DESCRIPTOR);

  do {
    //
    // Parse all Interface
    //
    Status = mPrivate->UsbCore->AllocateBuffer(
                                  sizeof (INTERFACE_DESC_LIST_ENTRY),
                                  ALIGNMENT_32,
                                  (VOID **)&InterfaceEntry
                                  );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = ParseThisInterface (InterfaceEntry, ptr, (UINTN *)&LengthLeft, (UINTN *)&Parsed);
    if (EFI_ERROR (Status)) {
      mPrivate->UsbCore->FreeBuffer (
                           sizeof (INTERFACE_DESC_LIST_ENTRY),
                           InterfaceEntry
                           );
      return (ActualInterfaces >= NumInterface) ? EFI_SUCCESS : Status;
    }

    InsertTailList (
      &ConfigDescEntry->InterfaceDescListHead,
      &InterfaceEntry->Link
      );
    //
    // Setup Smm address convert table for Smm security policy
    //
    mPrivate->UsbCore->InsertAddressConvertTable (
                         ACT_INSTANCE_BODY,
                         InterfaceEntry,
                         sizeof (INTERFACE_DESC_LIST_ENTRY)
                         );
    mPrivate->UsbCore->InsertAddressConvertTable (
                         ACT_LINKING_LIST,
                         &InterfaceEntry->EndpointDescListHead,
                         1
                         );
    //
    // Parsed for next interface
    //
    LengthLeft -= Parsed;
    ptr += Parsed;
    ActualInterfaces ++;
  } while (LengthLeft > 0);
  //
  // Parse for additional alt setting;
  //
  return EFI_SUCCESS;
}

/**

  Get the string table stored in a usb device.
  
  @param  UsbIoDev              USB_IO_DEVICE device structure.
  
  @retval EFI_SUCCESS
  @retval EFI_UNSUPPORTED
  @retval EFI_OUT_OF_RESOURCES
  
**/
EFI_STATUS
UsbGetStringtable (
  IN USB_IO_DEVICE                      *UsbIoDev
  )
{
  EFI_STATUS                    Status;
  UINT32                        Result;
  EFI_USB_SUPPORTED_LANGUAGES   *LanguageTable;
  UINT8                         *Buffer;
  UINT8                         *ptr;
  UINTN                         Index;
  UINTN                         LangTableSize;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  UINT16                        TempBuffer;
  EFI_USB_CORE_PROTOCOL         *UsbCore;

  UsbIo   = &(UsbIoDev->UsbController[0]->UsbIo);
  UsbCore = mPrivate->UsbCore;

  //
  // We get first 2 byte of langID table,
  // so we can have the whole table length
  //
  Status = UsbCore->UsbGetString (
                      UsbIo,
                      0,
                      0,
                      &TempBuffer,
                      2,
                      &Result
                      );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  LanguageTable = (EFI_USB_SUPPORTED_LANGUAGES *) &TempBuffer;

  if (LanguageTable->Length == 0) {
    return EFI_UNSUPPORTED;
  }
  //
  // If length is 2, then there is no string table
  //
  if (LanguageTable->Length == 2) {
    return EFI_UNSUPPORTED;
  }

  Status = UsbCore->AllocateBuffer(
                      LanguageTable->Length,
                      ALIGNMENT_32,
                      (VOID **)&Buffer
                      );
  
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  //
  // Now we get the whole LangID table
  //
  Status = UsbCore->UsbGetString (
                      UsbIo,
                      0,
                      0,
                      Buffer,
                      LanguageTable->Length,
                      &Result
                      );
  if (EFI_ERROR (Status)) {
    UsbCore->FreeBuffer (
               LanguageTable->Length,
               Buffer
               );
    return EFI_UNSUPPORTED;
  }

  LanguageTable = (EFI_USB_SUPPORTED_LANGUAGES *) Buffer;

  //
  // ptr point to the LangID table
  //
  ptr           = Buffer + 2;
  LangTableSize = (LanguageTable->Length - 2) / 2;

  for (Index = 0; Index < LangTableSize && Index < USB_MAXLANID; Index++) {
    UsbIoDev->LangID[Index] = *((UINT16 *) ptr);
    ptr += 2;
  }

  UsbCore->FreeBuffer (
             LanguageTable->Length,
             Buffer
             );
  return EFI_SUCCESS;
}

/**

  This function is to parse all the configuration descriptor.
  
  @param  UsbIoDev              USB_IO_DEVICE device structure.
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  @retval EFI_OUT_OF_RESOURCES  

**/
EFI_STATUS
UsbGetAllConfigurations (
  IN USB_IO_DEVICE                      *UsbIoDev
  )
{
  EFI_STATUS                    Status;
  UINT32                        Result;
  UINTN                         Index;
  UINTN                         TotalLength;
  UINT8                         *Buffer;
  CONFIG_DESC_LIST_ENTRY        *ConfigDescEntry;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  EFI_USB_CORE_PROTOCOL         *UsbCore;

  InitializeListHead (&UsbIoDev->ConfigDescListHead);
  UsbIo   = &(UsbIoDev->UsbController[0]->UsbIo);
  UsbCore = mPrivate->UsbCore;

  for (Index = 0; Index < UsbIoDev->DeviceDescriptor.NumConfigurations; Index++) {
    Status = UsbCore->AllocateBuffer(
                        sizeof (CONFIG_DESC_LIST_ENTRY),
                        ALIGNMENT_32,
                       (VOID **)&ConfigDescEntry
                        );
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto exit;
    }
    //
    // Stall 1ms between two GetDescriptor command for specific device compatibility(0.1ms minimum)
    //
    UsbCore->Stall (1000);
    //
    // 1st only get 1st 8 bytes config descriptor, so we can know the whole length.
    // Don't set the length less then 8 due to some device may return incorrect content
    //
    Status = UsbCore->UsbGetDescriptor (
                        UsbIo,
                        (UINT16) ((USB_DESC_TYPE_CONFIG << 8) | Index),
                        0,
                        sizeof (EFI_USB_CONFIG_DESCRIPTOR),
                        &ConfigDescEntry->CongfigDescriptor,
                        &Result
                        );
    if (EFI_ERROR (Status)) {
      DEBUG ((gUSBErrorLevel, "First get config descriptor error\n"));
      Status = EFI_DEVICE_ERROR;
      goto free_config_desc_entry;
    }

    TotalLength = ConfigDescEntry->CongfigDescriptor.TotalLength;
    Status = UsbCore->AllocateBuffer(
                        TotalLength,
                        ALIGNMENT_32,
                        (VOID **)&Buffer
                        );
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto free_config_desc_entry;
    }
    //
    // Stall 1ms between two GetDescriptor command for specific device compatibility(0.1ms minimum)
    //
    UsbCore->Stall (1000);
    //
    // Then we get the total descriptors for this configuration
    //
    Status = UsbCore->UsbGetDescriptor (
                        UsbIo,
                        (UINT16) ((USB_DESC_TYPE_CONFIG << 8) | Index),
                        0,
                        (UINT16) TotalLength,
                        Buffer,
                        &Result
                        );
    if (EFI_ERROR (Status)) {
      DEBUG ((gUSBErrorLevel, "Get whole config descriptor error\n"));
      Status = EFI_DEVICE_ERROR;
      UsbCore->FreeBuffer (
                 TotalLength,
                 Buffer
                 );
      goto free_config_desc_entry;
    }

    InitializeListHead (&ConfigDescEntry->InterfaceDescListHead);

    //
    // Parse this whole configuration
    //
    Status = ParseThisConfig (ConfigDescEntry, Buffer, TotalLength);

    if (EFI_ERROR (Status)) {
      //
      // Ignore this configuration, parse next one
      //
      UsbCore->FreeBuffer (
                 sizeof (CONFIG_DESC_LIST_ENTRY),
                 ConfigDescEntry
                 );
      UsbCore->FreeBuffer (
                 TotalLength,
                 Buffer
                 );
      continue;
    }
    InsertTailList (&UsbIoDev->ConfigDescListHead, &ConfigDescEntry->Link);
    UsbCore->FreeBuffer (
               TotalLength,
               Buffer
               );
    //
    // Setup Smm address convert table for Smm security policy
    //
    UsbCore->InsertAddressConvertTable (
               ACT_INSTANCE_BODY,
               ConfigDescEntry,
               sizeof (CONFIG_DESC_LIST_ENTRY)
               );
    UsbCore->InsertAddressConvertTable (
               ACT_LINKING_LIST,
               &ConfigDescEntry->InterfaceDescListHead,
               1
               );
  }
  Status = EFI_SUCCESS;
  goto exit;
free_config_desc_entry:
  UsbCore->FreeBuffer (
             sizeof (CONFIG_DESC_LIST_ENTRY),
             ConfigDescEntry
             );
exit:
  return Status;
}

/**

  Set the device to a configuration value.
  
  @param  UsbIoDev              USB_IO_DEVICE to be set configuration
  @param  ConfigrationValue     The configuration value to be set to that device
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  
**/
EFI_STATUS
UsbSetConfiguration (
  IN USB_IO_DEVICE                      *UsbIoDev,
  IN UINTN                              ConfigurationValue
  )
{
  LIST_ENTRY              *NextEntry;
  CONFIG_DESC_LIST_ENTRY  *ConfigEntry;
  UINT32                  Result;
  EFI_STATUS              Status;
  EFI_USB_IO_PROTOCOL     *UsbIo;

  UsbIo     = &(UsbIoDev->UsbController[0]->UsbIo);
  NextEntry = UsbIoDev->ConfigDescListHead.ForwardLink;

  while (NextEntry != &UsbIoDev->ConfigDescListHead) {
    //
    // Get one entry
    //
    ConfigEntry = (CONFIG_DESC_LIST_ENTRY *) NextEntry;
    if (ConfigEntry->CongfigDescriptor.ConfigurationValue == ConfigurationValue) {
      //
      // Find one, set to the active configuration
      //
      UsbIoDev->ActiveConfig = ConfigEntry;
      break;
    }

    NextEntry = NextEntry->ForwardLink;
  }
  //
  // Next Entry should not be null
  //
  Status = mPrivate->UsbCore->UsbSetDeviceConfiguration (
                                UsbIo,
                                (UINT16) ConfigurationValue,
                                &Result
                                );
  return Status;
}

/*++

  Set the device to a default configuration value.
  
  @param  UsbIoDev              USB_IO_DEVICE to be set configuration
  
  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR
  
--*/
EFI_STATUS
UsbSetDefaultConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev
  )
{
  CONFIG_DESC_LIST_ENTRY  *ConfigEntry;
  UINT16                  ConfigValue;
  LIST_ENTRY              *NextEntry;

  if (IsListEmpty (&UsbIoDev->ConfigDescListHead)) {
    return EFI_DEVICE_ERROR;
  }

  NextEntry   = UsbIoDev->ConfigDescListHead.ForwardLink;

  ConfigEntry = (CONFIG_DESC_LIST_ENTRY *) NextEntry;
  ConfigValue = ConfigEntry->CongfigDescriptor.ConfigurationValue;

  return UsbSetConfiguration (UsbIoDev, ConfigValue);
}

/**

  Delete all configuration data when device is not used.
  
  @param  UsbIoDev              USB_IO_DEVICE to be set configuration

**/
VOID
UsbDestroyAllConfiguration (
  IN  USB_IO_DEVICE                     *UsbIoDev
  )
{
  CONFIG_DESC_LIST_ENTRY        *ConfigEntry;
  INTERFACE_DESC_LIST_ENTRY     *InterfaceEntry;
  ENDPOINT_DESC_LIST_ENTRY      *EndpointEntry;
  LIST_ENTRY                    *NextEntry;
  EFI_USB_CORE_PROTOCOL         *UsbCore;

  UsbCore = mPrivate->UsbCore;
  //
  // Delete all configuration descriptor data
  //
  ConfigEntry = (CONFIG_DESC_LIST_ENTRY *) UsbIoDev->ConfigDescListHead.ForwardLink;

  while (ConfigEntry && ConfigEntry != (CONFIG_DESC_LIST_ENTRY *) &UsbIoDev->ConfigDescListHead) {
    //
    // Delete all its interface descriptors
    //
    InterfaceEntry = (INTERFACE_DESC_LIST_ENTRY *) ConfigEntry->InterfaceDescListHead.ForwardLink;

    while (InterfaceEntry != (INTERFACE_DESC_LIST_ENTRY *) &ConfigEntry->InterfaceDescListHead) {
      //
      // Delete all its endpoint descriptors
      //
      EndpointEntry = (ENDPOINT_DESC_LIST_ENTRY *) InterfaceEntry->EndpointDescListHead.ForwardLink;
      while (EndpointEntry != (ENDPOINT_DESC_LIST_ENTRY *) &InterfaceEntry->EndpointDescListHead) {
        NextEntry = ((LIST_ENTRY *) EndpointEntry)->ForwardLink;
        RemoveEntryList ((LIST_ENTRY *) EndpointEntry);
        UsbCore->FreeBuffer (
                   sizeof (ENDPOINT_DESC_LIST_ENTRY),
                   EndpointEntry
                   );
        //
        // Remove Smm address convert table
        //
        UsbCore->RemoveAddressConvertTable (
                   ACT_INSTANCE_BODY,
                   EndpointEntry
                   );
        EndpointEntry = (ENDPOINT_DESC_LIST_ENTRY *) NextEntry;
      }

      NextEntry = ((LIST_ENTRY *) InterfaceEntry)->ForwardLink;
      RemoveEntryList ((LIST_ENTRY *) InterfaceEntry);
      UsbCore->FreeBuffer (
                 sizeof (INTERFACE_DESC_LIST_ENTRY),
                 InterfaceEntry
                 );
      //
      // Remove Smm address convert table
      //
      UsbCore->RemoveAddressConvertTable (
                 ACT_INSTANCE_BODY,
                 InterfaceEntry
                 );
      UsbCore->RemoveAddressConvertTable (
                 ACT_LINKING_LIST,
                 &InterfaceEntry->EndpointDescListHead
                 );
      InterfaceEntry = (INTERFACE_DESC_LIST_ENTRY *) NextEntry;
    }

    NextEntry = ((LIST_ENTRY *) ConfigEntry)->ForwardLink;
    RemoveEntryList ((LIST_ENTRY *) ConfigEntry);
    UsbCore->FreeBuffer (
               sizeof (CONFIG_DESC_LIST_ENTRY),
               ConfigEntry
               );
    //
    // Remove Smm address convert table
    //
    UsbCore->RemoveAddressConvertTable (
               ACT_INSTANCE_BODY,
               ConfigEntry
               );
    UsbCore->RemoveAddressConvertTable (
               ACT_LINKING_LIST,
               &ConfigEntry->InterfaceDescListHead
               );
    ConfigEntry = (CONFIG_DESC_LIST_ENTRY *) NextEntry;
  }
}

/**

  Tell if there is a device connected to that port according to
  the Port Status.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnect (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 0 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_CONNECTION) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if Port is enabled.

  @param  PortStatus            The status value of that port.

  @retval TRUE                  Port is enable
  @retval FALSE                 Port is disable

**/
BOOLEAN
IsPortEnable (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 1 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_ENABLE) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if the port is being reset.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortInReset (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 4 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_RESET) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is power applied to that port.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortPowerApplied (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 8 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_POWER) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if the connected device is a low device.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortLowSpeedDeviceAttached (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 9 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_LOW_SPEED) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if the port is suspend.

  @param  PortStatus            The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortSuspend (
  IN UINT16  PortStatus
  )
{
  //
  // return the bit 2 value of PortStatus
  //
  if ((PortStatus & USB_PORT_STAT_SUSPEND) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Connect Change status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortConnectChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 0 value of PortChangeStatus
  //
  if ((PortChangeStatus & USB_PORT_STAT_C_CONNECTION) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Enable/Disable change in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortEnableDisableChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 1 value of PortChangeStatus
  //
  if ((PortChangeStatus & USB_PORT_STAT_C_ENABLE) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Port Reset Change status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortResetChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 4 value of PortChangeStatus
  //
  if ((PortChangeStatus & (USB_PORT_STAT_C_RESET | USB_PORT_STAT_C_BH_RESET)) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Tell if there is a Suspend Change Status in that port.

  @param  PortChangeStatus      The status value of that port.

  @retval TRUE
  @retval FALSE

**/
BOOLEAN
IsPortSuspendChange (
  IN UINT16  PortChangeStatus
  )
{
  //
  // return the bit 2 value of PortChangeStatus
  //
  if ((PortChangeStatus & USB_PORT_STAT_C_SUSPEND) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**

  Reset if the device is disconencted or not

  @param  UsbIoController       Indicating the Usb Controller Device.
  @param  Disconnected          Indicate whether the device is disconencted or not

  @retval EFI_SUCCESS
  @retval EFI_DEVICE_ERROR

**/
EFI_STATUS
IsDeviceDisconnected (
  IN USB_IO_CONTROLLER_DEVICE    *UsbIoController,
  IN OUT BOOLEAN                 *Disconnected
  )
{
  USB_IO_DEVICE             *ParentIoDev;
  USB_IO_DEVICE             *UsbIoDev;
  USB_IO_CONTROLLER_DEVICE  *ParentController;
  UINT8                     HubPort;
  EFI_STATUS                Status;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  EFI_USB_PORT_STATUS       PortStatus;

  ParentController  = UsbIoController->Parent;
  ParentIoDev       = ParentController->UsbDevice;
  UsbIoDev          = UsbIoController->UsbDevice;
  HubPort           = UsbIoController->ParentPort;

  if (ParentIoDev->DeviceAddress == 1) {
    //
    // Connected to the root hub
    //
    ParentIoDev->BusController->Usb3HCInterface->GetRootHubPortStatus (
                                  ParentIoDev->BusController->Usb3HCInterface,
                                  HubPort,
                                  &PortStatus
                                  );

  } else {
    UsbIo = &UsbIoController->UsbIo;
    Status = HubGetPortStatus (
              &ParentController->UsbIo,
              HubPort + 1,
              (UINT32 *) &PortStatus
              );

    if (EFI_ERROR (Status)) {
      return IsDeviceDisconnected (ParentController, Disconnected);
    }
  }

  *Disconnected = FALSE;

  if (!IsPortConnect (PortStatus.PortStatus)) {
    *Disconnected = TRUE;
  }

  return EFI_SUCCESS;
}

/**

  Get the first AlternateSetting in the Interface ListEntry

  @param  This                  EFI_USB_IO_PROTOCOL

  @retval The first AlternateSetting value

**/
UINT8
GetFirstAlternateSetting (
  IN EFI_USB_IO_PROTOCOL                *This
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *UsbIoDev;
  LIST_ENTRY                *InterfaceListHead;
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  if (!UsbIoDev->IsConfigured) {
    return 0;
  }
  InterfaceListHead   = &UsbIoDev->ActiveConfig->InterfaceDescListHead;
  InterfaceListEntry  = (INTERFACE_DESC_LIST_ENTRY *) (InterfaceListHead->ForwardLink);
  //
  // Loop all interface descriptor to get match one.
  //
  while (InterfaceListEntry != (INTERFACE_DESC_LIST_ENTRY *) InterfaceListHead) {
    if (InterfaceListEntry->InterfaceDescriptor.InterfaceNumber == UsbIoController->InterfaceNumber) {
      return InterfaceListEntry->InterfaceDescriptor.AlternateSetting;
    }
    InterfaceListEntry = (INTERFACE_DESC_LIST_ENTRY *) InterfaceListEntry->Link.ForwardLink;
  }
  return 0;
}

/**

  Get the HID descriptor in the Interface ListEntry

  @param  This                  EFI_USB_IO_PROTOCOL

  @retval The HID descriptor

**/
EFI_USB_HID_DESCRIPTOR*
GetHidDescriptor (
  IN EFI_USB_IO_PROTOCOL                *This
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *UsbIoDev;
  LIST_ENTRY                *InterfaceListHead;
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;
  if (!UsbIoDev->IsConfigured) {
    return NULL;
  }
  InterfaceListHead   = &UsbIoDev->ActiveConfig->InterfaceDescListHead;
  InterfaceListEntry  = (INTERFACE_DESC_LIST_ENTRY *) (InterfaceListHead->ForwardLink);
  //
  // Loop all interface descriptor to get match one.
  //
  while (InterfaceListEntry != (INTERFACE_DESC_LIST_ENTRY *) InterfaceListHead) {
    if (InterfaceListEntry->InterfaceDescriptor.InterfaceNumber == UsbIoController->InterfaceNumber &&
        InterfaceListEntry->InterfaceDescriptor.AlternateSetting == UsbIoController->AlternateSetting) {
      return (InterfaceListEntry->HidDescriptor.Length != 0) ? &InterfaceListEntry->HidDescriptor : NULL;
    }
    InterfaceListEntry = (INTERFACE_DESC_LIST_ENTRY *) InterfaceListEntry->Link.ForwardLink;
  }
  return NULL;
}

INTERFACE_DESC_LIST_ENTRY*
FindInterfaceListEntry (
  IN EFI_USB_IO_PROTOCOL        *This
  )
{
  USB_IO_CONTROLLER_DEVICE  *UsbIoController;
  USB_IO_DEVICE             *UsbIoDev;
  LIST_ENTRY                *InterfaceListHead;
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;

  UsbIoController = USB_IO_CONTROLLER_DEVICE_FROM_USB_IO_THIS (This);
  UsbIoDev        = UsbIoController->UsbDevice;

  if (!UsbIoDev->IsConfigured) {
    return NULL;
  }

  InterfaceListHead   = &UsbIoDev->ActiveConfig->InterfaceDescListHead;
  InterfaceListEntry  = (INTERFACE_DESC_LIST_ENTRY *) (InterfaceListHead->ForwardLink);

  //
  // Loop all interface descriptor to get match one.
  //
  while (InterfaceListEntry != (INTERFACE_DESC_LIST_ENTRY *) InterfaceListHead) {
    if (InterfaceListEntry->InterfaceDescriptor.InterfaceNumber == UsbIoController->InterfaceNumber &&
        InterfaceListEntry->InterfaceDescriptor.AlternateSetting == UsbIoController->AlternateSetting) {
      return InterfaceListEntry;
    }

    InterfaceListEntry = (INTERFACE_DESC_LIST_ENTRY *) InterfaceListEntry->Link.ForwardLink;
  }

  return NULL;
}

/**

  Find EndPoint ListEntry.

  @param  This                  EFI_USB_IO_PROTOCOL   
  @param  EndPointAddress       Endpoint address.

  @retval ENDPOINT_DESC_LIST_ENTRY pointer

**/
ENDPOINT_DESC_LIST_ENTRY *
FindEndPointListEntry (
  IN EFI_USB_IO_PROTOCOL    *This,
  IN UINT8                  EndPointAddress
  )
{
  INTERFACE_DESC_LIST_ENTRY *InterfaceListEntry;
  LIST_ENTRY                *EndpointDescListHead;
  ENDPOINT_DESC_LIST_ENTRY  *EndPointListEntry;

  InterfaceListEntry = FindInterfaceListEntry (This);
  if (InterfaceListEntry != NULL) {
    EndpointDescListHead  = &InterfaceListEntry->EndpointDescListHead;
    EndPointListEntry     = (ENDPOINT_DESC_LIST_ENTRY *) (EndpointDescListHead->ForwardLink);

    //
    // Loop all interface descriptor to get match one.
    //
    while (EndPointListEntry != (ENDPOINT_DESC_LIST_ENTRY *) EndpointDescListHead) {
      if (EndPointListEntry->EndpointDescriptor.EndpointAddress == EndPointAddress) {
        return EndPointListEntry;
      }

      EndPointListEntry = (ENDPOINT_DESC_LIST_ENTRY *) EndPointListEntry->Link.ForwardLink;
    }
  }

  return NULL;
}

/**

  Set the datatoggle of a specified endpoint

  @param  UsbIo                 Given Usb Controller device.
  @param  EndpointAddr          Given Endpoint address.
  @param  DataToggle            The current data toggle of that endpoint to be set

**/
VOID
SetDataToggleBit (
  IN EFI_USB_IO_PROTOCOL    *UsbIo,
  IN UINT8                  EndpointAddr,
  IN UINT8                  DataToggle
  )
{

  ENDPOINT_DESC_LIST_ENTRY  *EndpointListEntry;

  EndpointListEntry = FindEndPointListEntry (UsbIo, EndpointAddr);
  if (EndpointListEntry == NULL) {
    return ;
  }

  EndpointListEntry->Toggle = DataToggle;
  return ;
}

/**

  Usb Set Device Address

  @param  UsbIo                 EFI_USB_IO_PROTOCOL
  @param  AddressValue          Device address 
  @param  Status                Transfer status

  @retval EFI_SUCCESS           Success
  @retval EFI_INVALID_PARAMETER Parameter is error
  @retval EFI_TIMEOUT           Device has no response 

**/
EFI_STATUS
UsbSetDeviceAddress (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo,
  IN  UINT16                  AddressValue,
  OUT UINT32                  *Status
  )
{
  EFI_USB_DEVICE_REQUEST  DevReq;

  if (UsbIo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&DevReq, sizeof (EFI_USB_DEVICE_REQUEST));

  DevReq.RequestType  = USB_DEV_SET_ADDRESS_REQ_TYPE;
  DevReq.Request      = USB_DEV_SET_ADDRESS;
  DevReq.Value        = AddressValue;

  return UsbIo->UsbControlTransfer (
                  UsbIo,
                  &DevReq,
                  EfiUsbNoData,
                  3 * 1000,
                  NULL,
                  0,
                  Status
                  );
}
