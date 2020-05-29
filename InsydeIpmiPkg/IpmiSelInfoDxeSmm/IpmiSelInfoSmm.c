/** @file
 H2O IPMI SEL Info module implement code.

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
#include <IpmiSelInfoCommon.h>


/**
 This is the entry point to the H2O IPMI SelInfo Protocol SMM Driver.

 @param[in]         ImageHandle         A handle for this module.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @return Install procotol status.
*/
EFI_STATUS
EFIAPI
IpmiSelInfoSmmEntry (
  IN      EFI_HANDLE                    ImageHandle,
  IN      EFI_SYSTEM_TABLE              *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;

  InitialIpmiSelInfoContent ();

  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gH2OSmmIpmiSelInfoProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSelInfoProtocol
                    );

  if (!EFI_ERROR (Status)) {
    //
    // Reserved for OEM to implement something additional requirements
    // after SMM IPMI SEL Info Protocol has been installed.
    //
    OemIpmiSmmAfterInstallSelInfo ();
  }

  return Status;

}

