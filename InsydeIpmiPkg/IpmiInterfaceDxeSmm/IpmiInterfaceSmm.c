/** @file
 H2O IPMI Interface SMM module entry.

 This file contains implementation of SMM module entry point.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <Library/SmmServicesTableLib.h>
#include <Library/SmmOemIpmiPackageLib.h>
#include <IpmiInterfaceCommon.h>

  
/**
 Entrypoint of this module.

 This function is the entry point of this module. It installs H2O IPMI Interface Protocol
 in SMM phase.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_SUCCESS                    Install protocol success.
 @retval !EFI_SUCCESS                   Install protocol fail.
*/
EFI_STATUS
EFIAPI
IpmiInterfaceSmmEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_PRIVATE_DATA                 *IpmiData;
  H2O_IPMI_HOB                          *IpmiHob;


  //
  // Get Hob from IPMI PEIM
  //
  Status = IpmiLibGetIpmiHob (&IpmiHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Store address/offset information as static data
  //
  CopyMem (
    mAllBaseAddressTable,
    IpmiHob->InterfaceAddress,
    H2O_IPMI_MAX_BASE_ADDRESS_NUM * sizeof (H2O_IPMI_INTERFACE_ADDRESS)
    );

  //
  // Allocate private data memory
  //
  IpmiData = (H2O_IPMI_PRIVATE_DATA*)AllocateZeroPool (sizeof (H2O_IPMI_PRIVATE_DATA));
  if (IpmiData == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Fill SMM context
  //
  InitialIpmiInterfaceContent (IpmiHob, TRUE, IpmiData);

  //
  // SMM Only
  //
  IpmiData->TotalTimeTicks = FixedPcdGet8 (PcdIpmiSmmExecuteTime) * 1000000 * TICK_OF_TEN_MS / 10;
  IpmiData->RetryAfterIdle = FixedPcdGet8 (PcdIpmiSmmRetryAfterIdle);

  //
  // Install the Ipmi interface
  //
  Status = gSmst->SmmInstallProtocolInterface (
                    &IpmiData->ProtocolHandle,
                    &gH2OSmmIpmiInterfaceProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &IpmiData->H2OIpmiProtocol
                    );

  if (!EFI_ERROR (Status)) {
    //
    // Reserved for OEM to implement something additional requirements
    // after SMM IPMI Interface Protocol has been installed.
    //
    OemIpmiSmmAfterInstallInterface (&IpmiData->H2OIpmiProtocol);
  }

  return Status;

}

