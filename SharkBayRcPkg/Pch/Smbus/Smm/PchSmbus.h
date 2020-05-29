/** @file
  PCH Smbus Protocol

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

#ifndef _SMM_PCH_SMBUS_H
#define _SMM_PCH_SMBUS_H

///
/// Driver Produced Protocol Prototypes
///
#include <Protocol/SmbusHc.h>
#include <Protocol/SmmSmbus.h>

///
/// Driver Consumed Protcol Prototypes
///
#include <PchSmbusCommon.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
///
/// Max number of SMBus devices (7 bit address yields 128 combinations but 21 of those are reserved)
///
#define MAX_SMBUS_DEVICES 107
#define MICROSECOND 10
#define MILLISECOND (1000 * MICROSECOND)
#define ONESECOND   (1000 * MILLISECOND)

///
/// Declare a local instance structure for this driver
///
typedef struct _SMBUS_INSTANCE {
  UINTN                 Signature;
  EFI_HANDLE            Handle;

  UINT32                SmbusIoBase;
  SMBUS_IO_READ         SmbusIoRead;
  SMBUS_IO_WRITE        SmbusIoWrite;
  SMBUS_IO_DONE         IoDone;

  ///
  /// Published interface
  ///
  EFI_SMBUS_HC_PROTOCOL SmbusController;

} SMBUS_INSTANCE;

SMBUS_INSTANCE  *mSmbusContext;

//
// Prototypes
//

/**
  Execute an SMBUS operation

  @param[in] This                 The protocol instance
  @param[in] SlaveAddress         The address of the SMBUS slave device
  @param[in] Command              The SMBUS command
  @param[in] Operation            Which SMBus protocol will be issued
  @param[in] PecCheck             If Packet Error Code Checking is to be used
  @param[in, out] Length          Length of data
  @param[in, out] Buffer          Data buffer

  @retval EFI_SUCCESS             The SMBUS operation is successful
  @retval Other Values            Something error occurred
**/
EFI_STATUS
EFIAPI
SmbusExecute (
  IN CONST  EFI_SMBUS_HC_PROTOCOL         *This,
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND      Command,
  IN      EFI_SMBUS_OPERATION           Operation,
  IN      BOOLEAN                       PecCheck,
  IN OUT  UINTN                         *Length,
  IN OUT  VOID                          *Buffer
  );

/**
  Smbus driver entry point

  @param[in] ImageHandle          ImageHandle of this module
  @param[in] SystemTable          EFI System Table

  @retval EFI_SUCCESS             Driver initializes successfully
  @retval Other values            Some error occurred
**/
EFI_STATUS
EFIAPI
InitializePchSmbusSmm (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  );

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
  IN      BOOLEAN                       ArpAll,
  IN      EFI_SMBUS_UDID                * SmbusUdid, OPTIONAL
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS      * SlaveAddress OPTIONAL
  );

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
  IN OUT  UINTN                         *Length,
  IN OUT  EFI_SMBUS_DEVICE_MAP          **SmbusDeviceMap
  );

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
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress,
  IN      UINTN                         Data,
  IN      EFI_SMBUS_NOTIFY_FUNCTION     NotifyFunction
  );

#endif
