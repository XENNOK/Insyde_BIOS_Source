/** @file
 H2O IPMI SEL Info module implement code.

 This c file contains driver entry function for DXE phase.

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


#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeOemIpmiPackageLib.h>
#include <IpmiSelInfoCommon.h>


/**
 This is the entry point to the H2O IPMI SelInfo Protocol DXE Driver.

 @param[in]         ImageHandle         A handle for this module.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @return Install procotol status.
*/
EFI_STATUS
EFIAPI
IpmiSelInfoDxeEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;

  InitialIpmiSelInfoContent ();

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gH2OIpmiSelInfoProtocolGuid,
                  &mSelInfoProtocol,
                  NULL
                  );

  if (!EFI_ERROR (Status)) {
    //
    // Reserved for OEM to implement something additional requirements
    // after DXE IPMI SEL Info Protocol has been installed.
    //
    OemIpmiDxeAfterInstallSelInfo ();
  }

  return Status ;

}

