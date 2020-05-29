/** @file
 H2O IPMI SEL Data module implement code.

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
#include <IpmiSelDataCommon.h>


/**
 This is the entry point to the H2O IPMI SelData Protocol DXE Driver.

 @param[in]         ImageHandle         A handle for this module.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @return Install procotol status.
*/
EFI_STATUS
EFIAPI
IpmiSelDataDxeEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;

  InitialIpmiSelDataContent ();

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces(
                  &Handle,
                  &gH2OIpmiSelDataProtocolGuid,
                  &mSelDataProtocol,
                  NULL
                  );

  if (!EFI_ERROR (Status)) {
    //
    // Reserved for OEM to implement something additional requirements
    // after DXE IPMI SEL Data Protocol has been installed.
    //
    OemIpmiDxeAfterInstallSelData ();
  }

  return Status ;

}

