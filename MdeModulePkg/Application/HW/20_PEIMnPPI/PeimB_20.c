/** @file
  PeimB_20 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PeimA_20.h"

EFI_STATUS
EFIAPI
PeimMainB (
  IN EFI_PEI_FILE_HANDLE FileHandle, 
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  EFI_STATUS      Status;
  EFI_PEIM_PPI    *PeimPpi;
  
  DEBUG((EFI_D_ERROR | EFI_D_INFO, "\n\n##########start##########\n\n"));
  DEBUG((EFI_D_ERROR | EFI_D_INFO, "        PeimMainB           "));
  DEBUG((EFI_D_ERROR | EFI_D_INFO, "\n\n###########end###########\n\n"));

  Status = (*gPeiServices)->LocatePpi(
                                   gPeiServices,
                                   &gPeimPpiGuid,                     // Guid
                                   0,                                 // Instance
                                   NULL,                              // PpiDescriptor
                                   &PeimPpi                           // Ppi interface
                                   );
  DEBUG ( (EFI_D_ERROR | EFI_D_INFO, "Locate PPI: %r\n", Status) );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  PeimPpi->PeimDo();

  return Status; 
}