/** @file
  PEIM for Debug Mask

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DebugMaskPei.h"

//
// module variables
//
EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnDebugMaskPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  DebugMaskPpiNotifyCallback 
};

/**
  Entry point of Debug Mask PEIM.

  This function is the entry point of this DXE Status Code Driver.
  It registers notify event for EFI_RSC_HANDLER_PROTOCOL installed.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
DebugMaskPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                    Status;

  Status = (*PeiServices)->NotifyPpi (PeiServices, &mNotifyOnDebugMaskPpiList);
  return Status;
}

/**
  Notification function for ReportStatusCode handler Protocol

  This routine is the notification function for EFI_RSC_HANDLER_PROTOCOL

  @param EFI_EVENT              Event of the notification
  @param Context                not used in this function

  @retval none

**/
EFI_STATUS
EFIAPI
DebugMaskPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Variable;
  UINTN                           Size;
  UINT64                          GlobalErrorLevel;
  UINT64                          HobErrorLevel;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
    if (!EFI_ERROR (Status)) {
      Size = sizeof (GlobalErrorLevel);
      Status = Variable->GetVariable ( 
                           Variable, 
                           DEBUG_MASK_VARIABLE_NAME,
                           &gEfiGenericVariableGuid,
                           NULL,
                           &Size,
                           &GlobalErrorLevel
                           );
      if (!EFI_ERROR (Status)) {
        //
        // Build the GUID'ed HOB for DXE
        //
        HobErrorLevel = GlobalErrorLevel;
        BuildGuidDataHob (
          &gEfiGenericVariableGuid,
          &HobErrorLevel,
          sizeof (HobErrorLevel)
          );
      }
     } 
  
  return Status;
}

