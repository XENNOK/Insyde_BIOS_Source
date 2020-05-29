/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcSetPlatformHardwareSwitch().
 The function PeiCsSvcSetPlatformHardwareSwitch() use chipset services to feedback 
 its proprietary settings of the hardware switches.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
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
 To feedback its proprietary settings of the hardware switches.

 @param[in]         PlatformHardwareSwitch     Pointer to PLATFORM_HARDWARE_SWITCH.

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcSetPlatformHardwareSwitch (
  OUT PLATFORM_HARDWARE_SWITCH          **PlatformHardwareSwitchDptr
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
      ChipsetSvcPpi->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, SetPlatformHardwareSwitch) + sizeof (VOID*)) || 
      ChipsetSvcPpi->SetPlatformHardwareSwitch == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O PEI Chipset Services is NULL or member SetPlatformHardwareSwitch() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return ChipsetSvcPpi->SetPlatformHardwareSwitch (PlatformHardwareSwitchDptr);   
}  
