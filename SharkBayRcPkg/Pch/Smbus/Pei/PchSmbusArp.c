/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSmbusArp.c

@brief
  PCH Smbus PEIM. This file is used when we want ARP support.

**/
#include "PchSmbus.h"

///
/// These addresses are reserved by the SMBus 2.0 specification
///
static UINT8  mReservedAddress[SMBUS_NUM_RESERVED] = {
  0x00,
  0x02,
  0x04,
  0x06,
  0x08,
  0x0A,
  0x0C,
  0x0E,
  0x10,
  0x18,
  0x50,
  0x6E,
  0xC2,
  0xF0,
  0xF2,
  0xF4,
  0xF6,
  0xF8,
  0xFA,
  0xFC,
  0xFE,
  0x12,
  0x14,
  0x16,
  0x58,
  0x5A,
  0x80,
  0x82,
  0x84,
  0x86,
  0x88,
  0x90,
  0x92,
  0x94,
  0x96,
  0x1A,
  0x1C,
  0x1E
};

/**
  Issue a prepare ARP command to informs all devices that the ARP Master is starting the ARP process

  @param[in] Private              Pointer to the SMBUS_INSTANCE

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusPrepareToArp (
  IN  SMBUS_INSTANCE        *Private
  )
{
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  EFI_STATUS                Status;
  UINTN                     Length;
  UINT8                     Buffer;

  DEBUG ((EFI_D_INFO, "PEI SmbusPrepareToArp() Start\n"));

  SlaveAddress.SmbusDeviceAddress = SMBUS_ADDRESS_ARP;
  Length                          = 1;
  Buffer                          = SMBUS_DATA_PREPARE_TO_ARP;

  Status = SmbusExec (
            SlaveAddress,
            0,
            EfiSmbusSendByte,
            TRUE,
            &Length,
            &Buffer
            );

  DEBUG ((EFI_D_INFO, "PEI SmbusPrepareToArp() End\n"));

  return Status;
}

/**
  Issue a Get UDID (general) command to requests ARP-capable and/or Discoverable devices to 
  return their slave address along with their UDID.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in, out] DeviceMap       Pointer to SMBUS device map table that slave device return

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusGetUdidGeneral (
  IN      SMBUS_INSTANCE        *Private,
  IN OUT  EFI_SMBUS_DEVICE_MAP  *DeviceMap
  )
{
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  EFI_STATUS                Status;
  UINTN                     Length;
  UINT8                     Buffer[SMBUS_GET_UDID_LENGTH];

  SlaveAddress.SmbusDeviceAddress = SMBUS_ADDRESS_ARP;
  Length                          = SMBUS_GET_UDID_LENGTH;

  DEBUG ((EFI_D_INFO, "PEI SmbusGetUdidGeneral() Start\n"));

  Status = SmbusExec (
            SlaveAddress,
            SMBUS_DATA_GET_UDID_GENERAL,
            EfiSmbusReadBlock,
            TRUE,
            &Length,
            Buffer
            );

  if (!EFI_ERROR (Status)) {
    if (Length == SMBUS_GET_UDID_LENGTH) {
      DeviceMap->SmbusDeviceUdid.DeviceCapabilities = Buffer[0];
      DeviceMap->SmbusDeviceUdid.VendorRevision     = Buffer[1];
      DeviceMap->SmbusDeviceUdid.VendorId           = (UINT16) ((Buffer[2] << 8) + Buffer[3]);
      DeviceMap->SmbusDeviceUdid.DeviceId           = (UINT16) ((Buffer[4] << 8) + Buffer[5]);
      DeviceMap->SmbusDeviceUdid.Interface          = (UINT16) ((Buffer[6] << 8) + Buffer[7]);
      DeviceMap->SmbusDeviceUdid.SubsystemVendorId  = (UINT16) ((Buffer[8] << 8) + Buffer[9]);
      DeviceMap->SmbusDeviceUdid.SubsystemDeviceId  = (UINT16) ((Buffer[10] << 8) + Buffer[11]);
      DeviceMap->SmbusDeviceUdid.VendorSpecificId = (UINT32) ((Buffer[12] << 24) + (Buffer[13] << 16) + (Buffer[14] << 8) + Buffer[15]);
      DeviceMap->SmbusDeviceAddress.SmbusDeviceAddress = (UINT8) (Buffer[16] >> 1);
    } else {
      Status = EFI_DEVICE_ERROR;
    }
  }

  DEBUG ((EFI_D_INFO, "PEI SmbusGetUdidGeneral() End\n"));

  return Status;
}

/**
  Issue a Assign address command to assigns an address to a specific slave device.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in, out] DeviceMap       Pointer to SMBUS device map table that send to slave device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusAssignAddress (
  IN      SMBUS_INSTANCE        *Private,
  IN OUT  EFI_SMBUS_DEVICE_MAP  *DeviceMap
  )
{
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  EFI_STATUS                Status;
  UINTN                     Length;
  UINT8                     Buffer[SMBUS_GET_UDID_LENGTH];

  DEBUG ((EFI_D_INFO, "PEI SmbusAssignAddress() Start\n"));

  Buffer[0]                       = DeviceMap->SmbusDeviceUdid.DeviceCapabilities;
  Buffer[1]                       = DeviceMap->SmbusDeviceUdid.VendorRevision;
  Buffer[2]                       = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorId >> 8);
  Buffer[3]                       = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorId);
  Buffer[4]                       = (UINT8) (DeviceMap->SmbusDeviceUdid.DeviceId >> 8);
  Buffer[5]                       = (UINT8) (DeviceMap->SmbusDeviceUdid.DeviceId);
  Buffer[6]                       = (UINT8) (DeviceMap->SmbusDeviceUdid.Interface >> 8);
  Buffer[7]                       = (UINT8) (DeviceMap->SmbusDeviceUdid.Interface);
  Buffer[8]                       = (UINT8) (DeviceMap->SmbusDeviceUdid.SubsystemVendorId >> 8);
  Buffer[9]                       = (UINT8) (DeviceMap->SmbusDeviceUdid.SubsystemVendorId);
  Buffer[10]                      = (UINT8) (DeviceMap->SmbusDeviceUdid.SubsystemDeviceId >> 8);
  Buffer[11]                      = (UINT8) (DeviceMap->SmbusDeviceUdid.SubsystemDeviceId);
  Buffer[12]                      = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorSpecificId >> 24);
  Buffer[13]                      = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorSpecificId >> 16);
  Buffer[14]                      = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorSpecificId >> 8);
  Buffer[15]                      = (UINT8) (DeviceMap->SmbusDeviceUdid.VendorSpecificId);
  Buffer[16]                      = (UINT8) (DeviceMap->SmbusDeviceAddress.SmbusDeviceAddress << 1);

  SlaveAddress.SmbusDeviceAddress = SMBUS_ADDRESS_ARP;
  Length                          = SMBUS_GET_UDID_LENGTH;

  Status = SmbusExec (
            SlaveAddress,
            SMBUS_DATA_ASSIGN_ADDRESS,
            EfiSmbusWriteBlock,
            TRUE,
            &Length,
            Buffer
            );

  DEBUG ((EFI_D_INFO, "PEI SmbusAssignAddress() End\n"));

  return Status;
}

/**
  Do a fully (general) Arp procress to assign the slave address of all ARP-capable device.
  This function will issue issue the "Prepare to ARP", "Get UDID" and "Assign Address" commands.

  @param[in] Private              Pointer to the SMBUS_INSTANCE

  @retval EFI_OUT_OF_RESOURCES    No available address to assign
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusFullArp (
  IN      SMBUS_INSTANCE  *Private
  )
{
  EFI_STATUS            Status;
  EFI_SMBUS_DEVICE_MAP  *CurrentDeviceMap;

  DEBUG ((EFI_D_INFO, "PEI SmbusFullArp() Start\n"));

  Status = SmbusPrepareToArp (Private);
  if (EFI_ERROR (Status)) {
    if (Status == EFI_DEVICE_ERROR) {
      ///
      ///  ARP is complete
      ///
      return EFI_SUCCESS;
    } else {
      return Status;
    }
  }
  ///
  ///  Main loop to ARP all ARP-capable devices
  ///
  do {
    CurrentDeviceMap  = &Private->DeviceMap[Private->DeviceMapEntries];
    Status            = SmbusGetUdidGeneral (Private, CurrentDeviceMap);
    if (EFI_ERROR (Status)) {
      break;
    }

    if (CurrentDeviceMap->SmbusDeviceAddress.SmbusDeviceAddress == (0xFF >> 1)) {
      ///
      /// If address is unassigned, assign it
      ///
      Status = GetNextAvailableAddress (
                Private,
                &CurrentDeviceMap->SmbusDeviceAddress
                );
      if (EFI_ERROR (Status)) {
        return EFI_OUT_OF_RESOURCES;
      }
    } else if (((CurrentDeviceMap->SmbusDeviceUdid.DeviceCapabilities) & 0xC0) != 0) {
      ///
      /// if address is not fixed, check if the current address is available
      ///
      if (!IsAddressAvailable (
            Private,
            CurrentDeviceMap->SmbusDeviceAddress
            )) {
        ///
        /// if currently assigned address is already used, get a new one
        ///
        Status = GetNextAvailableAddress (
                  Private,
                  &CurrentDeviceMap->SmbusDeviceAddress
                  );
        if (EFI_ERROR (Status)) {
          return EFI_OUT_OF_RESOURCES;
        }
      }
    }

    Status = SmbusAssignAddress (Private, CurrentDeviceMap);
    if (EFI_ERROR (Status)) {
      ///
      /// If there was a device error, just continue on and try again.
      /// Other errors should be reported.
      ///
      if (Status != EFI_DEVICE_ERROR) {
        return Status;
      }
    } else {
      ///
      /// If there was no error, the address was assigned and we must update our
      /// records.
      ///
      Private->DeviceMapEntries++;
    }

  } while (Private->DeviceMapEntries < MAX_SMBUS_DEVICES);

  DEBUG ((EFI_D_INFO, "PEI SmbusFullArp() End\n"));

  return EFI_SUCCESS;
}

/**
  Do a directed Arp procress to assign the slave address of a single ARP-capable device.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in] SmbusUdid            When doing a directed ARP, ARP the device with this UDID.
  @param[in, out] SlaveAddress    Buffer to store new Slave Address during directed ARP.

  @retval EFI_OUT_OF_RESOURCES    DeviceMapEntries is more than Max number of SMBus devices.
                                  Or there is no available address to assign
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusDirectedArp (
  IN      SMBUS_INSTANCE            *Private,
  IN      EFI_SMBUS_UDID            *SmbusUdid,
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS  *SlaveAddress
  )
{
  EFI_STATUS            Status;
  EFI_SMBUS_DEVICE_MAP  *CurrentDeviceMap;

  DEBUG ((EFI_D_INFO, "PEI SmbusDirectedArp() Start\n"));

  if (Private->DeviceMapEntries >= MAX_SMBUS_DEVICES) {
    return EFI_OUT_OF_RESOURCES;
  }

  CurrentDeviceMap = &Private->DeviceMap[Private->DeviceMapEntries];

  ///
  /// Find an available address to assign
  ///
  Status = GetNextAvailableAddress (
            Private,
            &CurrentDeviceMap->SmbusDeviceAddress
            );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  CurrentDeviceMap->SmbusDeviceUdid.DeviceCapabilities  = SmbusUdid->DeviceCapabilities;
  CurrentDeviceMap->SmbusDeviceUdid.DeviceId            = SmbusUdid->DeviceId;
  CurrentDeviceMap->SmbusDeviceUdid.Interface           = SmbusUdid->Interface;
  CurrentDeviceMap->SmbusDeviceUdid.SubsystemDeviceId   = SmbusUdid->SubsystemDeviceId;
  CurrentDeviceMap->SmbusDeviceUdid.SubsystemVendorId   = SmbusUdid->SubsystemVendorId;
  CurrentDeviceMap->SmbusDeviceUdid.VendorId            = SmbusUdid->VendorId;
  CurrentDeviceMap->SmbusDeviceUdid.VendorRevision      = SmbusUdid->VendorRevision;
  CurrentDeviceMap->SmbusDeviceUdid.VendorSpecificId    = SmbusUdid->VendorSpecificId;

  Status = SmbusAssignAddress (Private, CurrentDeviceMap);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Private->DeviceMapEntries++;
  SlaveAddress->SmbusDeviceAddress = CurrentDeviceMap->SmbusDeviceAddress.SmbusDeviceAddress;

  DEBUG ((EFI_D_INFO, "PEI SmbusDirectedArp() End\n"));

  return EFI_SUCCESS;
}

/**
  Find an available address to assign

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in] SlaveAddress         Buffer to store new Slave Address during directed ARP.

  @retval EFI_OUT_OF_RESOURCES    There is no available address to assign
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
GetNextAvailableAddress (
  IN      SMBUS_INSTANCE            *Private,
  IN      EFI_SMBUS_DEVICE_ADDRESS  *SlaveAddress
  )
{
  for (SlaveAddress->SmbusDeviceAddress = 0x03;
       SlaveAddress->SmbusDeviceAddress < 0x7F;
       SlaveAddress->SmbusDeviceAddress++
      ) {
    if (IsAddressAvailable (Private, *SlaveAddress)) {
      return EFI_SUCCESS;
    }
  }

  return EFI_OUT_OF_RESOURCES;
}

/**
  Check whether the address is assignable.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in] SlaveAddress         The Slave Address for checking

  @retval TRUE                    The address is assignable
  @retval FALSE                   The address is not assignable
**/
BOOLEAN
IsAddressAvailable (
  IN      SMBUS_INSTANCE            *Private,
  IN      EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress
  )
{
  UINT8 Index;

  ///
  /// See if we have already assigned this address to a device
  ///
  for (Index = 0; Index < Private->DeviceMapEntries; Index++) {
    if (SlaveAddress.SmbusDeviceAddress == Private->DeviceMap[Index].SmbusDeviceAddress.SmbusDeviceAddress) {
      return FALSE;
    }
  }
  ///
  /// See if this address is claimed by a platform non-ARP-capable device
  ///
  for (Index = 0; Index < Private->PlatformNumRsvd; Index++) {
    if ((SlaveAddress.SmbusDeviceAddress << 1) == Private->PlatformRsvdAddr[Index]) {
      return FALSE;
    }
  }
  ///
  /// See if this is a reserved address
  ///
  for (Index = 0; Index < SMBUS_NUM_RESERVED; Index++) {
    if ((SlaveAddress.SmbusDeviceAddress << 1) == (UINTN) mReservedAddress[Index]) {
      return FALSE;
    }
  }

  return TRUE;
}
