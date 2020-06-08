/** @file
  PeimA_20 C Source File

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
PeimMainA (
  IN EFI_PEI_FILE_HANDLE FileHandle, 
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  EFI_STATUS            Status;
  
  DEBUG((EFI_D_ERROR | EFI_D_INFO, "\n\n##########start##########\n\n"));
  DEBUG((EFI_D_ERROR | EFI_D_INFO, "        PeimMainA           "));
  DEBUG((EFI_D_ERROR | EFI_D_INFO, "\n\n###########end###########\n\n"));

  gPeimPpi.PeimDo = PeimDo;

  Status = (*gPeiServices)->InstallPpi(
                                    gPeiServices,
                                    &EfiPeimDescriptorPpi
                                    );
  DEBUG ( (EFI_D_ERROR | EFI_D_INFO, "Install PPI: %r\n", Status) );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = EFI_SUCCESS;

  return Status; 
}

EFI_STATUS
PeimDo (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *ReadOnlyVariable2;

  CHAR16                             *VariableName = L"abc";
  EFI_GUID                           VariableGUID = VARIABLE_GUID;
  UINT64                             DataSize = 2;
  CHAR8                              *Data;

  EFI_HOB_GUID_TYPE                  *Hob;
  EFI_GUID                           HobGuid = HOB_GUID;
  VOID                               *DataBeginAddress; 

  Status = (*gPeiServices)->AllocatePool(
                                      gPeiServices,
                                      DataSize,
                                      &Data
                                      );
  DEBUG ( (EFI_D_ERROR | EFI_D_INFO, "AllocatePool: %r\n", Status) );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = (*gPeiServices)->LocatePpi(
                                   gPeiServices,
                                   &gEfiPeiReadOnlyVariable2PpiGuid,  // Guid
                                   0,                                 // Instance
                                   NULL,                              // PpiDescriptor
                                   &ReadOnlyVariable2                 // Ppi interface
                                   );
  DEBUG ( (EFI_D_ERROR | EFI_D_INFO, "Locate PPI: %r\n", Status) );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = ReadOnlyVariable2->GetVariable (
                                       ReadOnlyVariable2,
                                       VariableName,
                                       &VariableGUID,
                                       NULL,
                                       &DataSize,
                                       Data
                                       );
  DEBUG ( (EFI_D_ERROR | EFI_D_INFO, "Get Variable: %r\n", Status) );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = (*gPeiServices)->CreateHob(
                                   gPeiServices,
                                   EFI_HOB_TYPE_GUID_EXTENSION,
                                   (UINT16) (sizeof (EFI_HOB_GUID_TYPE) + DataSize),
                                   &Hob
                                   );
  DEBUG ( (EFI_D_ERROR | EFI_D_INFO, "Create HOB: %r\n", Status) );                                   
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Hob->Name = HobGuid;

  DataBeginAddress = Hob + 1;
  (*gPeiServices)->CopyMem(
                        DataBeginAddress,
                        Data,
                        DataSize
                        );

  return Status;
}