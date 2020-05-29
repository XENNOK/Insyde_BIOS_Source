/** @file
 H2O IPMI SDR module implement code.

 This c file contains driver entry function for SDR phase.

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
#include <IpmiSdrCommon.h>


/**
 This is the entry point to the H2O IPMI SDR SMM Driver.

 @param[in]         ImageHandle         A handle for this module.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @return Install procotol status.
*/
EFI_STATUS
EFIAPI
IpmiSdrSmmEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS        Status;
  EFI_HANDLE        SdrHandle;


  //
  // Install Ipmi SDR Protocol Interface
  //
  InitialIpmiSdrContent ();

  //
  // Install protocol
  //
  SdrHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &SdrHandle,
                    &gH2OSmmIpmiSdrProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSdrProtocol
                    );

  if (!EFI_ERROR (Status)) {
    //
    // Reserved for OEM to implement something additional requirements
    // after SMM IPMI SDR Protocol has been installed.
    //
    OemIpmiSmmAfterInstallSdr ();
  }

  return Status;

}

