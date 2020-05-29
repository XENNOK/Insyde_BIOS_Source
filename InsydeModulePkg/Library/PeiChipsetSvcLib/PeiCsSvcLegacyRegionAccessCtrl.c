/** @file
 PEI Chipset Services Library.
 
 This file contains only one function that is PeiCsSvcLegacyRegionAccessCtrl().
 The function PeiCsSvcLegacyRegionAccessCtrl() use chipset services to control
 legacy region can be access or not.

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
 Legacy Region Access Control.
 
 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                LEGACY_REGION_ACCESS_LOCK or LEGACY_REGION_ACCESS_UNLOCK
                                        If LEGACY_REGION_ACCESS_LOCK, then LegacyRegionAccessCtrl()
                                        lock the specific legacy region.
                    
 @retval            EFI_SUCCESS           Function returns successfully
 @retval            EFI_UNSUPPORTED       If the function is not implemented.
 @retval            EFI_INVALID_PARAMETER 1. The parameter Start is small then 0xC0000.
                                          2. The parameter Length is to long. 
                                             The Start + Length - 1 should small then 0xF0000.
                                          3. The Mode parameter is neither 
                                             LEGACY_REGION_ACCESS_LOCK nor LEGACY_REGION_ACCESS_UNLOCK

*/
EFI_STATUS
PeiCsSvcLegacyRegionAccessCtrl (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  UINT32                        Mode  
  )
{
  EFI_STATUS               Status;
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
      ChipsetSvcPpi->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, LegacyRegionAccessCtrl) + sizeof (VOID*)) || 
      ChipsetSvcPpi->LegacyRegionAccessCtrl == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O PEI Chipset Services is NULL or member LegacyRegionAccessCtrl() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return ChipsetSvcPpi->LegacyRegionAccessCtrl (Start, Length, Mode);   
}  
