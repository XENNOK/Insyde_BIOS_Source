/** @file
  When PEI begins to initialize clock generator, 
  this OemService sends the clock generator information to ProgClkGen driver. 
  According to the OEM specification, add the clock generator information to this service.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PeiOemServicesThunk.h"
#include <Library/PeiOemSvcKernelLib.h>

/**
  Process After Modify ClockGen.
  
  @param  Base on OEM design

  @retval EFI_SUCCESS           If ChipsetModifyClockGenInfo run success.
**/
EFI_STATUS
OemSvcChipsetAfterModifyClockGenThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}
