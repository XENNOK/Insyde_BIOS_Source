/** @file
  PCH Smbus Driver, ARP functions not supported

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#include <PchSmbus.h>

/**
  Set Slave address for an Smbus device with a known UDID or perform a general
  ARP of all devices.

  @param[in] This                 Pointer to the instance of the EFI_SMBUS_HC_PROTOCOL.
  @param[in] ArpAll               If TRUE, do a full ARP. Otherwise, just ARP the specified UDID.
  @param[in] SmbusUdid            When doing a directed ARP, ARP the device with this UDID.
  @param[in, out] SlaveAddress    Buffer to store new Slave Address during directed ARP. On output,If
                                  ArpAlll == TRUE, this will contain the newly assigned Slave address.

  @exception EFI_UNSUPPORTED      This functionality is not supported
**/
EFI_STATUS
EFIAPI
SmbusArpDevice (
  IN CONST  EFI_SMBUS_HC_PROTOCOL         * This,
  IN        BOOLEAN                       ArpAll,
  IN        EFI_SMBUS_UDID                * SmbusUdid, OPTIONAL
  IN OUT    EFI_SMBUS_DEVICE_ADDRESS      * SlaveAddress OPTIONAL
  )
{
  ///
  /// ARP should be done in DXE SMBUS driver.
  /// Not needed here.
  ///
  return EFI_UNSUPPORTED;
}

/**
  Get a pointer to the assigned mappings of UDID's to Slave Addresses.

  @param[in] This                 Pointer to the instance of the EFI_SMBUS_HC_PROTOCOL.
  @param[in, out] Length          Buffer to contain the lenght of the Device Map, it will be updated to
                                  contain the number of pairs of UDID's mapped to Slave Addresses.
  @param[in, out] SmbusDeviceMap  Buffer to contian a pointer to the Device Map, it will be updated to
                                  point to the first pair in the Device Map

  @exception EFI_UNSUPPORTED      This functionality is not supported
**/
EFI_STATUS
EFIAPI
SmbusGetArpMap (
  IN CONST  EFI_SMBUS_HC_PROTOCOL         *This,
  IN OUT    UINTN                         *Length,
  IN OUT    EFI_SMBUS_DEVICE_MAP          **SmbusDeviceMap
  )
{
  ///
  /// ARP should be done in DXE SMBUS driver.
  /// Not needed here.
  ///
  return EFI_UNSUPPORTED;
}

/**
  Register a callback in the event of a Host Notify command being sent by a
  specified Slave Device.

  @param[in] This                 Pointer to the instance of the EFI_SMBUS_HC_PROTOCOL.
  @param[in] SlaveAddress         Address of the device whose Host Notify command we want to
                                  trap.
  @param[in] Data                 Data of the Host Notify command we want to trap.
  @param[in] NotifyFunction       Function to be called in the event the desired Host Notify
                                  command occurs.

  @exception EFI_UNSUPPORTED      This functionality is not supported
**/
EFI_STATUS
EFIAPI
SmbusNotify (
  IN CONST  EFI_SMBUS_HC_PROTOCOL         *This,
  IN        EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN        UINTN                         Data,
  IN        EFI_SMBUS_NOTIFY_FUNCTION     NotifyFunction
  )
{
  ///
  /// Not needed for SMM.
  ///
  return EFI_UNSUPPORTED;
}
