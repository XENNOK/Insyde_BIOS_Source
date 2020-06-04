/** @file
  

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

#include "PEIMPPI_HW.h"

HYDRA_PPI_FUNC mHydraPpiFunction = {GetVari, CreateHob};

EFI_PEI_PPI_DESCRIPTOR EfiHydraPpiDescriptor = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiHydraPpiGuid,
  &mHydraPpiFunction
};

EFI_STATUS
GetVari (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Data,
  IN OUT UINTN                   *Size
  ) 
{
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVari;

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiPeiReadOnlyVariable2PpiGuid,
                             0,
                             NULL,
                             (VOID**) &ReadVari
                             );
   if (Status != EFI_SUCCESS) {
    //
    // Should never happen since we have a dependency expression based on variable services
    //
    DEBUG (((EFI_D_ERROR | EFI_D_INFO), "PEIMPPI_HW GetVari LocatePpi ERROR = %r\n", Status));
    return EFI_NOT_FOUND;
  } else {
    DEBUG (((EFI_D_ERROR | EFI_D_INFO), "PEIMPPI_HW GetVari LocatePpi = %r\n", Status));
    Status = ReadVari->GetVariable (
                         ReadVari,
                         L"HydraVari",
                         &gEfiHydraVariGuid,                                        
                         NULL,
                         Size,
                         Data
                         );
    if (EFI_ERROR (Status)) {
      DEBUG (((EFI_D_ERROR | EFI_D_INFO), "PEIMPPI_HW GetVari GetVariable ERROR = %r\n", Status));
      return Status;
    }
  }
  return Status;
}

EFI_STATUS
CreateHob (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Data,
  IN OUT UINT16                  DataSize
  )
{ 
  EFI_STATUS                Status;
  HOB_LIST                  *Hob;

  Status = PeiServicesCreateHob (
             EFI_HOB_TYPE_GUID_EXTENSION,
             sizeof(HOB_LIST) + DataSize,
             (VOID **)&Hob
             );
 
  if (EFI_ERROR(Status)) {
    DEBUG (((EFI_D_ERROR | EFI_D_INFO), "Variable = %r\n", Status));
    return Status;
  }
  DEBUG (((EFI_D_ERROR | EFI_D_INFO), "Variable = %r\n", Status));
  Hob->Guid = gEfiHydraVariGuid;
  // Hob->Data = Data ;
  Hob->Size = DataSize ;
  Hob->Data = CopyMem(sizeof(HOB_LIST)+Hob,Data,DataSize);
  DEBUG (((EFI_D_ERROR | EFI_D_INFO), "Hob.guid = %g,\nHob.Size= %ld\n", Hob->Guid,Hob->Size));
  return Status;
}

EFI_STATUS
  HydraMain (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;
  

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#################### Hydra PEIM ####################\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#                This is Hydra PEIM                #\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#################### Hydra PEIM ####################\n")); 
  
  Status = PeiServicesInstallPpi(&EfiHydraPpiDescriptor);

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#################### Hydra PEIM ####################\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#              This is Hydra PEIM End              #\n"));
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\r\n#################### Hydra PEIM ####################\n")); 
  return EFI_SUCCESS;
}






