/** @file
  This OemService is part of setting Verb Table. The function is created for setting verb table 
  to support Multi-Sku and return the table to common code to program.

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
  This OemService is part of setting Verb Table. The function is created for setting verb table 
  to support Multi-Sku and return the table to common code to program.

  @param  *VerbTableHeaderDataAddress    A pointer to VerbTable data/header

  @retval EFI_SUCCESS                    Get verb table data/header success.
  @retval Others                         Base on OEM design.
**/
EFI_STATUS
OemSvcGetVerbTableThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  COMMON_CHIPSET_AZALIA_VERB_TABLE      **VerbTableHeaderDataAddress;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  VerbTableHeaderDataAddress = VA_ARG (Marker, COMMON_CHIPSET_AZALIA_VERB_TABLE **);
  VA_END (Marker);

  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcGetVerbTable (
             VerbTableHeaderDataAddress
             );

  return Status;
}
