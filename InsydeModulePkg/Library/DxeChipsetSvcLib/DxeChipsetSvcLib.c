/** @file
 DXE Chipset Services Library.
 	
 This file is a constructor for DXE chipset services library.
	
***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/H2ODxeChipsetServices.h>

#include <Library/DxeChipsetSvcLib.h>

//
// Global Variables (This Source File Only)
//

H2O_CHIPSET_SERVICES_PROTOCOL *mChipsetSvc = NULL;

/**
  Constructor which locates the H2O Chipset Services protocol. Since this 
  protocol is also listed in the dependency expression for this library,
  this should succeed.
  
  @param[in]        ImageHandle         The firmware allocated handle for the EFI image.
  @param[in]        SystemTable         A pointer to the EFI System Table.
  
  @retval           EFI_SUCCESS         This function alway return EFI_SUCCESS.
*/

EFI_STATUS
EFIAPI
DxeChipsetSvcLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  
  Status = gBS->LocateProtocol (
                  &gH2OChipsetServicesProtocolGuid,
                  NULL,
                  (VOID **)&mChipsetSvc
                  );
  if (EFI_ERROR (Status)) {
    //
    // If we get to this point (with the depex and everything) and there is no
    // H2O Chipset Services, we will return EFI_SUCCESS. In this case, the 
    // function calls below will catch this and return EFI_UNSUPPORTED.
    //
    mChipsetSvc = NULL;
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found!\n"));
  }

  return EFI_SUCCESS;             
}

