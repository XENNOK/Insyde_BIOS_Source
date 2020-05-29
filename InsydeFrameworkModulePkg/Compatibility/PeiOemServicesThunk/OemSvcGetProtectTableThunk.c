/** @file
  Provide OEM to define the protect regions when flash the recovery Bios by PeiCrisis method.

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
  This OemService provides OEM to define the protect regions when flash the recovery Bios by PeiCrisis method. 
  The detail of setting protect regions is described in document "PEI Crisis Feature Integration Guide ".  

  @param  *Count                The number of protect regions are defined.
  @param  *UseEcIdle            If UseEcIdle == TRUE, that EC will be idled, when flash the Bios.
  @param  *CrisisProtectTable   The table defines the protected regions when flash the bois.
  
  @retval EFI_SUCCESS           Get protected region success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetProtectTableThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINTN                                 *Count;
  BOOLEAN                               *UseEcIdle;
  FLASH_ENTRY                           **CrisisProtectTable;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  Count                                 = VA_ARG (Marker, UINTN *);
  UseEcIdle                             = VA_ARG (Marker, BOOLEAN *);
  CrisisProtectTable                    = VA_ARG (Marker, FLASH_ENTRY **);
  VA_END (Marker);

  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcGetProtectTable (
             Count,
             UseEcIdle,
             CrisisProtectTable
             );

  return Status;
}
