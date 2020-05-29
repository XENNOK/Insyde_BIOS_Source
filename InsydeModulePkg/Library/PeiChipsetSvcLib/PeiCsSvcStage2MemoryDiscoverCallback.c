/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcStage2MemoryDiscoverCallback().
 The function PeiCsSvcStage2MemoryDiscoverCallback() use chipset services to install
 Firmware Volume Hob's once there is main memory.

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


#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/H2OPeiChipsetServices.h>

/**
 Install Firmware Volume Hob's once there is main memory.

 @param [in]        BootMode            A pointer to boot mode status
                   
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcStage2MemoryDiscoverCallback (
  IN EFI_BOOT_MODE       *BootMode
  )
{
  EFI_STATUS                Status;
  H2O_CHIPSET_SERVICES_PPI *ChipsetSvcPpi;

  ChipsetSvcPpi = NULL;

  Status = PeiServicesLocatePpi (
             &gH2OChipsetServicesPpiGuid,
             0,
             NULL,
             (VOID **)&ChipsetSvcPpi
             );

  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR, "H2O PEI Chipset Services can not be found!\n"));
    return EFI_UNSUPPORTED;
  }
  
  //
  // Verify that the PPI interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (ChipsetSvcPpi == NULL ||
      ChipsetSvcPpi->Size < (OFFSET_OF(H2O_CHIPSET_SERVICES_PPI, Stage2MemoryDiscoverCallback) + sizeof(VOID*)) || 
      ChipsetSvcPpi->Stage2MemoryDiscoverCallback == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O PEI Chipset Services is NULL or member Stage2MemoryDiscoverCallback() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return ChipsetSvcPpi->Stage2MemoryDiscoverCallback (BootMode);   
}  
