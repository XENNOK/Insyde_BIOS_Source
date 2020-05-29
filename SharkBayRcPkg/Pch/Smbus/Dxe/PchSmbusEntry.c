/** @file
  PCH Smbus Driver

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
  Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSmbusEntry.c

@brief
  PCH Smbus Driver

**/
#include <PchSmbus.h>
//[-start-120308-IB10820018-add]//
#define PLATFORM_NUM_SMBUS_RSVD_ADDRESSES 4

#define SMBUS_ADDR_CH_A_1 0xA0
#define SMBUS_ADDR_CH_A_2 0xA2
#define SMBUS_ADDR_CH_B_1 0xA4
#define SMBUS_ADDR_CH_B_2 0xA6
//[-end-120308-IB10820018-add]//
//EFI_GUID  mEfiSmbusArpMapGuid = EFI_SMBUS_ARP_MAP_GUID;
//[-start-120308-IB10820018-add]//
static UINT8 mSmbusRsvdAddresses[PLATFORM_NUM_SMBUS_RSVD_ADDRESSES] = {
  SMBUS_ADDR_CH_A_1,
  SMBUS_ADDR_CH_A_2,
  SMBUS_ADDR_CH_B_1,
  SMBUS_ADDR_CH_B_2
};
//[-end-120308-IB10820018-add]//

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
  )
{
  DEBUG ((EFI_D_INFO, "SmbusExecute() Start, SmbusDeviceAddress=%x, Command=%x, Operation=%x\n", (SlaveAddress.SmbusDeviceAddress << 1), Command, Operation));
  InitializeSmbusRegisters ();

  return SmbusExec (
          SlaveAddress,
          Command,
          Operation,
          PecCheck,
          Length,
          Buffer
          );
}

/**
  Smbus driver entry point

  @param[in] ImageHandle          ImageHandle of this module
  @param[in] SystemTable          EFI System Table

  @retval EFI_SUCCESS             Driver initializes successfully
  @retval Other values            Some error occurred
**/
EFI_STATUS
EFIAPI
InitializePchSmbus (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable
  )
{
//[-start-120308-IB10820018-remove]//
//  DXE_PCH_PLATFORM_POLICY_PROTOCOL  *PchPlatformPolicy;
//[-end-120308-IB10820018-remove]//
  EFI_STATUS                        Status;
  //UINTN                             DataSize;
  //VOID                              *Data;
  EFI_PEI_HOB_POINTERS              HobList;

  DEBUG ((EFI_D_INFO, "InitializePchSmbus() Start\n"));
//[-start-120308-IB10820018-remove]//
//  Status = gBS->LocateProtocol (
//                  &gDxePchPlatformPolicyProtocolGuid,
//                  NULL,
//                  &PchPlatformPolicy
//                  );
//  ASSERT_EFI_ERROR (Status);
//[-end-120308-IB10820018-remove]//
  mSmbusContext = AllocateZeroPool (sizeof (SMBUS_INSTANCE));
  if (mSmbusContext == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  mSmbusContext->Signature                  = PCH_SMBUS_PRIVATE_DATA_SIGNATURE;
  mSmbusContext->IoDone                     = IoDone;
  mSmbusContext->SmbusIoRead                = SmbusIoRead;
  mSmbusContext->SmbusIoWrite               = SmbusIoWrite;
  mSmbusContext->SmbusController.Execute    = SmbusExecute;
  mSmbusContext->SmbusController.ArpDevice  = SmbusArpDevice;
  mSmbusContext->SmbusController.GetArpMap  = SmbusGetArpMap;
  mSmbusContext->SmbusController.Notify     = SmbusNotify;
//[-start-120308-IB10820018-modify]//
//  mSmbusContext->PlatformNumRsvd            = PchPlatformPolicy->SmbusConfig->NumRsvdSmbusAddresses;
//  mSmbusContext->PlatformRsvdAddr           = PchPlatformPolicy->SmbusConfig->RsvdSmbusAddressTable;
  mSmbusContext->PlatformNumRsvd            = PLATFORM_NUM_SMBUS_RSVD_ADDRESSES; //PchPlatformPolicy->SmbusConfig->NumRsvdSmbusAddresses;
  mSmbusContext->PlatformRsvdAddr           = mSmbusRsvdAddresses; //PchPlatformPolicy->SmbusConfig->RsvdSmbusAddressTable;
//[-end-120308-IB10820018-modify]//
  //
  // See if PEI already ARPed any devices, and if so, update our device map.
  //
  // Get Hob list
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **) &HobList.Raw);
  ASSERT_EFI_ERROR (Status);

  //HobList.Raw = GetNextGuidHob (&mEfiSmbusArpMapGuid, HobList.Raw);
  /////
  ///// If we found the right hob, store the information. Otherwise, continue.
  /////
  //if (HobList.Raw != NULL) {
  //  Data      = (VOID *) ((UINT8 *) (&HobList.Guid->Name) + sizeof (EFI_GUID));
  //  DataSize  = HobList.Header->HobLength - sizeof (EFI_HOB_GUID_TYPE);
  //  CopyMem (mSmbusContext->DeviceMap, Data, DataSize);
  //  mSmbusContext->DeviceMapEntries = (UINT8) (DataSize / sizeof (EFI_SMBUS_DEVICE_MAP));
  //}
  ///
  /// Initialize the NotifyFunctionList
  ///
  InitializeListHead (&mSmbusContext->NotifyFunctionList);

  ///
  /// Install the SMBUS interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmbusContext->Handle,
                  &gEfiSmbusHcProtocolGuid,
                  &mSmbusContext->SmbusController,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "InitializePchSmbus() End\n"));

  return EFI_SUCCESS;
}
