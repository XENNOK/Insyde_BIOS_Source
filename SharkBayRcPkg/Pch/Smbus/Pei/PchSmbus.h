/** @file
  PCH Smbus PPI

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

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
  PchSmbus.h

@brief
  PCH Smbus PPI

**/
#ifndef _PEI_PCH_SMBUS_H_
#define _PEI_PCH_SMBUS_H_

///
/// Driver Produced PPI Prototypes
///
#include <Ppi/Smbus.h>
///
/// Driver Consumed PPI Prototypes
///
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/SmbusPolicy.h>
#include <Ppi/MemoryDiscovered.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/PeiServicesLib.h>
#include <PchSmbusCommon.h>
///
/// Max number of SMBus devices
/// (7 bit address yields 128 combinations but 21 of those are reserved)
/// Due to limited resources, we only allow 8 in PEI.
///
#define MAX_SMBUS_DEVICES       8

#define MAX_SMBUS_NOTIFICATION  8

///
/// Private Data Structures
///
typedef struct _PEI_SMBUS_NOTIFY_FUNCTION_LIST_NODE {
  EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress;
  UINTN                         Data;
  EFI_PEI_SMBUS_NOTIFY_FUNCTION NotifyFunction;
} PEI_SMBUS_NOTIFY_FUNCTION_LIST_NODE;

///
/// Declare a local instance structure for this PEIM
///
typedef struct _SMBUS_INSTANCE {
  UINTN                               Signature;
  EFI_PEI_SERVICES                    **PeiServices;
  PEI_SMBUS_POLICY_PPI                *SmbusPolicy;
  UINTN                               SmbusIoBase;
  EFI_PEI_PPI_DESCRIPTOR              PpiDescriptor;
  EFI_PEI_SMBUS_PPI                   SmbusPpi;
  EFI_PEI_NOTIFY_DESCRIPTOR           NotifyDescriptor;
  UINT8                               DeviceMapEntries;
  EFI_SMBUS_DEVICE_MAP                DeviceMap[MAX_SMBUS_DEVICES];
  UINT8                               PlatformNumRsvd;
  UINT8                               *PlatformRsvdAddr;

  UINT8                               NotifyFunctionNum;
  PEI_SMBUS_NOTIFY_FUNCTION_LIST_NODE NotifyFunctionList[MAX_SMBUS_NOTIFICATION];
} SMBUS_INSTANCE;

#define SMBUS_PRIVATE_DATA_FROM_PPI_THIS(a) CR (a, SMBUS_INSTANCE, SmbusPpi, PCH_SMBUS_PRIVATE_DATA_SIGNATURE)

#define SMBUS_PRIVATE_DATA_FROM_DESCRIPTOR_THIS(a) \
  CR ( \
  a, \
  SMBUS_INSTANCE, \
  PpiDescriptor, \
  PCH_SMBUS_PRIVATE_DATA_SIGNATURE \
  )

#define SMBUS_PRIVATE_DATA_FROM_NOTIFY_THIS(a) \
  CR ( \
  a, \
  SMBUS_INSTANCE, \
  NotifyDescriptor, \
  PCH_SMBUS_PRIVATE_DATA_SIGNATURE \
  )

//
// Prototypes
//

/**
  This function provides a standard way to execute an SMBUS command
  PPI as defined in the SMBus Specification. The data can either be of
  the length byte, word, or a block of data (1 to 32 bytes long).
  The resulting transaction will be either the SMBus Slave Device accepts
  this transaction or this function returns with an error

  @param[in] PeiServices          PEI services table pointer
  @param[in] This                 PEI_SMBUS_PPI instance
  @param[in] SlaveAddress         Smbus Slave device address
  @param[in] Command              Command to be sent
  @param[in] Operation            Which SMBus PPI will be used
  @param[in] PecCheck             Defines if Packet Error Code Checking is to be used
  @param[in, out] Length          How many bytes to read/write. Must be 1 <= Length <= 32 depending on the Operation
  @param[in, out] Buffer          Data buffer

  @retval EFI_SUCCESS             Operation success.
                                  Length will contain the actual number of bytes read.
                                  Buffer will contain the data read.
  @retval Otherwise               Operation failed.
**/
EFI_STATUS
EFIAPI
SmbusExecute (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_SMBUS_PPI         *This,
  IN      EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress,
  IN      EFI_SMBUS_DEVICE_COMMAND  Command,
  IN      EFI_SMBUS_OPERATION       Operation,
  IN      BOOLEAN                   PecCheck,
  IN OUT  UINTN                     *Length,
  IN OUT  VOID                      *Buffer
  );

/**
  Initialize the Smbus PPI and program the Smbus BAR

  @param[in] FfsHeader            Not used.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The function completes successfully
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database
**/
EFI_STATUS
InitializePchSmbusPeim (
  IN EFI_PEI_FILE_HANDLE                   FileHandle,
  IN CONST EFI_PEI_SERVICES                **PeiServices
  );

/**
  Fix up pointers since they are located in real memory now.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  );

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
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      EFI_PEI_SMBUS_PPI             * This,
  IN      BOOLEAN                       ArpAll,
  IN      EFI_SMBUS_UDID                * SmbusUdid, OPTIONAL
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS      * SlaveAddress OPTIONAL
  );

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
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN      EFI_PEI_SMBUS_PPI             *This,
  IN OUT  UINTN                         *Length,
  IN OUT  EFI_SMBUS_DEVICE_MAP          **SmbusDeviceMap
  );

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
  );

/**
  This function initializes the SmBus driver in PEI.

  @param[in] PeiServices          Standard PEI services
  @param[in] Private              SMBUS private data structure

  @retval None.
**/
VOID
InitializePeiPrivate (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN      SMBUS_INSTANCE            *Private
  );

/**
  Issue a prepare ARP command to informs all devices that the ARP Master is starting the ARP process

  @param[in] Private              Pointer to the SMBUS_INSTANCE

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusPrepareToArp (
  IN      SMBUS_INSTANCE                *Private
  );

/**
  Issue a Get UDID (general) command to requests ARP-capable and/or Discoverable devices to
  return their slave address along with their UDID.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in, out] DeviceMap       Pointer to SMBUS device map table that slave device return

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusGetUdidGeneral (
  IN      SMBUS_INSTANCE                *Private,
  IN OUT  EFI_SMBUS_DEVICE_MAP          *DeviceMap
  );

/**
  Issue a Assign address command to assigns an address to a specific slave device.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in, out] DeviceMap       Pointer to SMBUS device map table that send to slave device

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusAssignAddress (
  IN      SMBUS_INSTANCE                *Private,
  IN OUT  EFI_SMBUS_DEVICE_MAP          *DeviceMap
  );

/**
  Do a fully (general) Arp procress to assign the slave address of all ARP-capable device.
  This function will issue issue the "Prepare to ARP", "Get UDID" and "Assign Address" commands.

  @param[in] Private              Pointer to the SMBUS_INSTANCE

  @retval EFI_OUT_OF_RESOURCES    No available address to assign
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SmbusFullArp (
  IN      SMBUS_INSTANCE                *Private
  );

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
  IN      SMBUS_INSTANCE                *Private,
  IN      EFI_SMBUS_UDID                *SmbusUdid,
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS      *SlaveAddress
  );

/**
  Find an available address to assign

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in, out] SlaveAddress    Buffer to store new Slave Address during directed ARP.

  @retval EFI_OUT_OF_RESOURCES    There is no available address to assign
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
GetNextAvailableAddress (
  IN      SMBUS_INSTANCE                *Private,
  IN OUT  EFI_SMBUS_DEVICE_ADDRESS      *SlaveAddress
  );

/**
  Check whether the address is assignable.

  @param[in] Private              Pointer to the SMBUS_INSTANCE
  @param[in] SlaveAddress         The Slave Address for checking

  @retval TRUE                    The address is assignable
  @retval FALSE                   The address is not assignable
**/
BOOLEAN
IsAddressAvailable (
  IN      SMBUS_INSTANCE                *Private,
  IN      EFI_SMBUS_DEVICE_ADDRESS      SlaveAddress
  );

/**
  This function gets called back at the end of PEI if any devices were ARPed
  during PEI. It will build a HOB to describe to DXE what devices were ARPed.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The EndOfPeiSignal PPI.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

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
  );

#endif
