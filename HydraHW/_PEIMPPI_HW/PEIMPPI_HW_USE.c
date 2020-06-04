/** @file
  
  PEIMPPI HW USE.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/


#include "PEIMPPI_HW_USE.h"


EFI_STATUS
  HydraMain (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS              Status;
  EFI_PEI_PPI_DESCRIPTOR  *PpiDesc;
  HYDRA_PPI_FUNC          *HydraFun;
  UINT8                   *Data = NULL;
  UINTN                   Size  = 0;

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#################### Hydra PEIM ####################\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#              This is Hydra PEIM USE              #\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#################### Hydra PEIM ####################\n")); 
  
  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiHydraPpiGuid,
                             0,
                             &PpiDesc,
                             &HydraFun
                             );
  if (EFI_ERROR (Status)) {
    DEBUG (((EFI_D_ERROR | EFI_D_INFO), "\n#################### PEIMPPI_HW_USE PeiServicesLocatePpi ERROR : %r ####################\n", Status));
  }
  DEBUG (((EFI_D_ERROR | EFI_D_INFO), "\n#################### PEIMPPI_HW_USE PeiServicesLocatePpi : %r ####################\n", Status));

  Status = HydraFun->GetVari (
                       PeiServices,
                       Data,
                       &Size
                       );

  if (Status == EFI_NOT_FOUND) {
    DEBUG ((EFI_D_ERROR |EFI_D_ERROR, " PEIMPPI_HW_USE GetVar: %r\n",Status));
    return Status;
  }else if (Status == EFI_BUFFER_TOO_SMALL){
    PeiServicesAllocatePool (Size,&Data);
    DEBUG ((EFI_D_ERROR |EFI_D_ERROR, "PEIMPPI_HW_USE PeiServicesAllocatePool EFI_BUFFER_TOO_SMALL? : %r\n",Status));
    Status = HydraFun->GetVari(
                         PeiServices,
                         Data,
                         &Size
                         );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR |EFI_D_ERROR, "PEIMPPI_HW_USE EFI_BUFFER_TOO_SMALL GetVari : %r\n",Status));
    }
  } else {
    PeiServicesAllocatePool (Size,&Data);
    DEBUG ((EFI_D_ERROR |EFI_D_ERROR, " PEIMPPI_HW_USE PeiServicesAllocatePool : %r\n",Status));
    Status = HydraFun->GetVari(
                         PeiServices,
                         Data,
                         &Size
                         );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR |EFI_D_ERROR, "PEIMPPI_HW_USE PeiServicesAllocatePool GetVari : %r\n",Status));
    }
  }                                       
  
  Status = HydraFun->CreateHob(
                       PeiServices,
                       Data,
                       (UINT16)Size
                       );
  if (EFI_ERROR(Status)) {
    DEBUG ((EFI_D_ERROR | EFI_D_ERROR, " PEIM_HW_USE  CreateHob: %r\n",Status));
  }
  DEBUG ((EFI_D_ERROR | EFI_D_ERROR," CreateHob = %r \n",Status));
  DEBUG ((EFI_D_ERROR | EFI_D_ERROR," Size = %d\n",Size));
  // DEBUG ((EFI_D_ERROR | EFI_D_ERROR," Size = %d ,DATA = %x %x %x\n",Size,*Data,*(Data+1),*(Data+2)));

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#################### Hydra PEIM ####################\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#            This is Hydra PEIM USE End            #\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#################### Hydra PEIM ####################\n")); 
  return EFI_SUCCESS;
}