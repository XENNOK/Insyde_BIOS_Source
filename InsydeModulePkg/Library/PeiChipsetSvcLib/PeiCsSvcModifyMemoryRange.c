/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcModifyMemoryRange().
 The function PeiCsSvcModifyMemoryRange() use chipset services to modify
 memory range setting.

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
 To modify memory range setting.

 @param[in, out]    OptionRomMask       A pointer to PEI_MEMORY_RANGE_OPTION_ROM
 @param[in, out]    SmramMask           A pointer to PEI_MEMORY_RANGE_SMRAM
 @param[in, out]    GraphicsMemoryMask  A pointer to PEI_MEMORY_RANGE_GRAPHICS_MEMORY
 @param[in, out]    PciMemoryMask       A pointer to PEI_MEMORY_RANGE_PCI_MEMORY
                   
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     If the function is not implemented.
*/
EFI_STATUS
EFIAPI
PeiCsSvcModifyMemoryRange (
  IN OUT  PEI_MEMORY_RANGE_OPTION_ROM           *OptionRomMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *SmramMask,
  IN OUT  PEI_MEMORY_RANGE_GRAPHICS_MEMORY      *GraphicsMemoryMask,
  IN OUT  PEI_MEMORY_RANGE_PCI_MEMORY           *PciMemoryMask
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
      ChipsetSvcPpi->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PPI, ModifyMemoryRange) + sizeof (VOID*)) || 
      ChipsetSvcPpi->ModifyMemoryRange == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O PEI Chipset Services is NULL or member ModifyMemoryRange() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return ChipsetSvcPpi->ModifyMemoryRange (OptionRomMask, SmramMask, GraphicsMemoryMask, PciMemoryMask);   
}  
