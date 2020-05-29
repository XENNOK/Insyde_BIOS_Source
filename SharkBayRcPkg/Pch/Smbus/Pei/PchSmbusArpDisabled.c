/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSmbusArpDisabled.c

@brief
  PCH Smbus PEIM. This file is used when we do not want ARP support.

**/
#include "PchSmbus.h"

/**
  Set Slave address for an Smbus device with a known UDID or perform a general
  ARP of all devices.

  @param[in] PeiServices          Pointer to the PEI Services table.
  @param[in] This                 Pointer to the instance of the PEI_SMBUS_PPI.
  @param[in] ArpAll               If TRUE, do a full ARP. Otherwise, just ARP the specified UDID.
  @param[in] SmbusUdid            When doing a directed ARP, ARP the device with this UDID.
  @param[in, out] SlaveAddress    Buffer to store new Slave Address during directed ARP.

  @exception EFI_UNSUPPORTED      This functionality is not supported
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
SmbusArpDevice (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_SMBUS_PPI         * This,
  IN      BOOLEAN                   ArpAll,
  IN      EFI_SMBUS_UDID            * SmbusUdid OPTIONAL,
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS  * SlaveAddress OPTIONAL
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Get a pointer to the assigned mappings of UDID's to Slave Addresses.

  @param[in] PeiServices          Pointer to the PEI Services table.
  @param[in] This                 Pointer to the instance of the PEI_SMBUS_PPI.
  @param[in, out] Length          Buffer to contain the lenght of the Device Map.
  @param[in, out] SmbusDeviceMap  Buffer to contian a pointer to the Device Map.

  @exception EFI_UNSUPPORTED      This functionality is not supported
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
SmbusGetArpMap (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_SMBUS_PPI         *This,
  IN OUT  UINTN                     *Length,
  IN OUT  EFI_SMBUS_DEVICE_MAP      **SmbusDeviceMap
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Register a callback in the event of a Host Notify command being sent by a
  specified Slave Device.

  @param[in] PeiServices          The general PEI Services
  @param[in] This                 The PPI instance
  @param[in] SlaveAddress         Address of the device whose Host Notify command we want to trap.
  @param[in] Data                 Data of the Host Notify command we want to trap.
  @param[in] NotifyFunction       Function to be called in the event the desired Host Notify command occurs.

  @exception EFI_UNSUPPORTED      This functionality is not supported
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
SmbusNotify (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      EFI_PEI_SMBUS_PPI             *This,
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      UINTN                         Data,
  IN      EFI_PEI_SMBUS_NOTIFY_FUNCTION NotifyFunction
  )
{
  ///
  /// Requires a periodic event, not supported in PEI
  ///
  return EFI_UNSUPPORTED;
}

/**
  Function to be called when SMBus.Execute happens. This will check if
  the SMBus Host Controller has received a Host Notify command. If so, it will
  see if a notification has been reqested on that event and make any callbacks
  that may be necessary.

  @param[in] Private              Pointer to the SMBUS_INSTANCE

  @retval None
**/
VOID
CheckNotification (
  IN      SMBUS_INSTANCE            *Private
  )
{
  return;
}