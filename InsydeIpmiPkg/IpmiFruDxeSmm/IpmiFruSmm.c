/** @file
 H2O IPMI FRU module implement code.

 This c file contains driver entry function for SMM phase.

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
#include "IpmiFruCommon.h"


/**
 Entrypoint of this module.

 This function is the entry point of this module. It installs H2O IPMI FRU Protocol
 in SMM phase.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_SUCCESS                    Install protocol success.
 @retval !EFI_SUCCESS                   Install protocol fail.
*/
EFI_STATUS
EFIAPI
IpmiFruSmmEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;

  //
  // Fill context
  //
  InitialIpmiFruContent ();

  //
  // Install protocol
  //
  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gH2OSmmIpmiFruProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mFruProtocol
                    );

  if (!EFI_ERROR (Status)) {
    //
    // Reserved for OEM to implement something additional requirements
    // after SMM IPMI FRU Protocol has been installed.
    //
    OemIpmiSmmAfterInstallFru ();
  }

  return Status;

}

