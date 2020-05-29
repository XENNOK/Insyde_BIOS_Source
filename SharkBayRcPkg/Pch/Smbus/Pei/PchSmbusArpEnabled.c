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
  PchSmbusArpEnabled.c

@brief
  PCH Smbus PEIM. This file is used when we want ARP support.

**/
#include "PchSmbus.h"

static EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEndOfPeiSignalPpiGuid,
  EndOfPeiCallback
};

EFI_GUID mEfiSmbusArpMapGuid = EFI_SMBUS_ARP_MAP_GUID;

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
  SMBUS_INSTANCE  *Private;
  EFI_STATUS      Status;
  UINT8           OldMapEntries;

  DEBUG ((EFI_D_INFO, "PEI SmbusArpDevice() Start\n"));

  Private       = SMBUS_PRIVATE_DATA_FROM_PPI_THIS (This);

  OldMapEntries = Private->DeviceMapEntries;

  if (ArpAll) {
    Status = SmbusFullArp (Private);
  } else {
    if ((SmbusUdid == NULL) || (SlaveAddress == NULL)) {
      return EFI_INVALID_PARAMETER;
    }

    Status = SmbusDirectedArp (Private, SmbusUdid, SlaveAddress);
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// If we just added the first entry in the device map, set up a callback so
  /// we can pass the map to DXE via a HOB at the end of PEI.
  ///
  if ((OldMapEntries == 0) && (Private->DeviceMapEntries > 0)) {
    Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyList);
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((EFI_D_INFO, "PEI SmbusArpDevice() End\n"));

  return EFI_SUCCESS;
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
  SMBUS_INSTANCE  *Private;

  Private         = SMBUS_PRIVATE_DATA_FROM_PPI_THIS (This);

  *Length         = Private->DeviceMapEntries * sizeof (EFI_SMBUS_DEVICE_MAP);
  *SmbusDeviceMap = Private->DeviceMap;
  return EFI_SUCCESS;
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
  SMBUS_INSTANCE  *Private;

  DEBUG ((EFI_D_INFO, "PEI SmbusNotify() Start\n"));

  Private = SMBUS_PRIVATE_DATA_FROM_PPI_THIS (This);

  if (NotifyFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// NOTE: Currently there is no periodic event in PEI.
  /// So we just check the Notification at the end of in each
  /// Smbus.Execute function.
  ///
  if (Private->NotifyFunctionNum >= MAX_SMBUS_NOTIFICATION) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->NotifyFunctionList[Private->NotifyFunctionNum].SlaveAddress.SmbusDeviceAddress = SlaveAddress.SmbusDeviceAddress;
  Private->NotifyFunctionList[Private->NotifyFunctionNum].Data                            = Data;
  Private->NotifyFunctionList[Private->NotifyFunctionNum].NotifyFunction                  = NotifyFunction;
  Private->NotifyFunctionNum++;

  ///
  /// Last step, check notification
  ///
  CheckNotification (Private);

  DEBUG ((EFI_D_INFO, "PEI SmbusNotify() End\n"));

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS              Status;
  EFI_PEI_PPI_DESCRIPTOR  *SmbusDescriptor;
  PEI_SMBUS_PPI           *SmbusPpi;
  SMBUS_INSTANCE          *Private;
  UINTN                   BufferSize;
  VOID                    *Hob;

  DEBUG ((EFI_D_INFO, "PEI EndOfPeiCallback() Start\n"));

  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiSmbusPpiGuid,  /// GUID
                            0,                  /// INSTANCE
                            &SmbusDescriptor,   /// PEI_PPI_DESCRIPTOR
                            (VOID **)&SmbusPpi           /// PPI
                            );
  ASSERT_EFI_ERROR (Status);

  Private     = SMBUS_PRIVATE_DATA_FROM_DESCRIPTOR_THIS (SmbusDescriptor);
  BufferSize  = sizeof (EFI_SMBUS_DEVICE_MAP) * Private->DeviceMapEntries;

  Hob = BuildGuidDataHob (
          &mEfiSmbusArpMapGuid,
          Private->DeviceMap,
          BufferSize
          );
  ASSERT (Hob != NULL);

  DEBUG ((EFI_D_INFO, "PEI EndOfPeiCallback() End\n"));

  return EFI_SUCCESS;
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
  EFI_SMBUS_DEVICE_ADDRESS  SlaveAddress;
  UINT8                     SstsReg;
  UINTN                     Data;
  UINTN                     Index;

  DEBUG ((EFI_D_INFO, "PEI CheckNotification() Start\n"));

  if (Private->NotifyFunctionNum == 0) {
    ///
    /// Since no one register it, not need to check.
    ///
    return;
  }

  SstsReg = SmbusIoRead (R_PCH_SMBUS_SSTS);
  if (!(SstsReg & B_PCH_SMBUS_HOST_NOTIFY_STS)) {
    ///
    /// Host Notify has not been received
    ///
    return;
  }
  ///
  /// There was a Host Notify, see if any one wants to know about it
  ///
  SlaveAddress.SmbusDeviceAddress = (SmbusIoRead (R_PCH_SMBUS_NDA)) >> 1;

  for (Index = 0; Index < Private->NotifyFunctionNum; Index++) {

    if (Private->NotifyFunctionList[Index].SlaveAddress.SmbusDeviceAddress == SlaveAddress.SmbusDeviceAddress) {
      Data = (SmbusIoRead (R_PCH_SMBUS_NDHB) << 8) + (SmbusIoRead (R_PCH_SMBUS_NDLB));
      if ((UINT16) Private->NotifyFunctionList[Index].Data == (UINT16) Data) {
        ///
        /// We have a match, notify the requested function
        ///
        Private->NotifyFunctionList[Index].NotifyFunction (
                                            Private->PeiServices,
                                            &Private->SmbusPpi,
                                            SlaveAddress,
                                            Data
                                            );
      }
    }
  }
  ///
  /// Clear the Notify Status bit and exit.
  ///
  SmbusIoWrite (R_PCH_SMBUS_SSTS, B_PCH_SMBUS_HOST_NOTIFY_STS);

  DEBUG ((EFI_D_INFO, "PEI CheckNotification() End\n"));
}
