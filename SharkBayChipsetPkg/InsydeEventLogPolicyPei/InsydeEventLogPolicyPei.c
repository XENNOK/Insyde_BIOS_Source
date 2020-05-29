/** @file
  Implementation of Insyde Event Log Policy PEI module.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <InsydeEventLogPolicyPei.h>


EFI_PEI_NOTIFY_DESCRIPTOR mReadOnlyVariable2PpiNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  ReadOnlyVariable2Callback
};

/**
  Callback function for the notification of ReadOnlyVariable2 PPI.

  @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.

  @retval EFI_SUCCESS                    Update Policy success.
  @return EFI_ERROR                      Locate gEfiPeiReadOnlyVariable2PpiGuid or execute GetVariable error.
*/
EFI_STATUS
ReadOnlyVariable2Callback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                            Status;

  Status = UpdateInsydeEventLogPolicy (PeiServices);

  return Status;
}

/**
 Update Insyde Event Log Policy according to the "Setup" Variable.

 @param[in]         PeiServices         A pointer to EFI_PEI_SERVICES struct pointer.

 @retval EFI_SUCCESS                    Update Policy success.
 @return EFI_ERROR                      Locate gEfiPeiReadOnlyVariable2PpiGuid or execute GetVariable error.
*/
EFI_STATUS
UpdateInsydeEventLogPolicy (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI       *VariablePpi;
  UINTN                                 Size;
  SYSTEM_CONFIGURATION                  SystemConfiguration;
  EFI_GUID                              SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VariablePpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Size = sizeof(SYSTEM_CONFIGURATION);
  Status = VariablePpi->GetVariable (
                                 VariablePpi,
                                 L"Setup",
                                 &SystemConfigurationGuid,
                                 NULL,
                                 &Size,
                                 &SystemConfiguration
                                 );
  if (!EFI_ERROR (Status)) {
    
    PcdSet8 (PcdLogEventTo,                 SystemConfiguration.LogEventTo);
    PcdSet8 (PcdEventLogFullOption,         SystemConfiguration.EventLogFullOption);
    PcdSetBool (PcdPostMessageEn,           SystemConfiguration.PostMessageEn);
    PcdSetBool (PcdPostMessageProgressCode, SystemConfiguration.ProgressCode);
    PcdSetBool (PcdPostMessageErrorCode,    SystemConfiguration.ErrorCode);
    PcdSetBool (PcdPostMessageDebugCode,    SystemConfiguration.DebugCode);
    PcdSetBool (PcdPostMessageLogPostMsg,   SystemConfiguration.LogPostMsg);
    PcdSetBool (PcdPostMessageShowPostMsg,  SystemConfiguration.ShowPostMsg);
    PcdSetBool (PcdPostMessageBeepPostMsg,  SystemConfiguration.BeepPostMsg);

    
    DEBUG((EFI_D_ERROR, "PcdLogEventTo: %x\n",              PcdGet8(PcdLogEventTo)));    
    DEBUG((EFI_D_ERROR, "PcdEventLogFullOption: %x\n",      PcdGet8(PcdEventLogFullOption)));
    DEBUG((EFI_D_ERROR, "PcdPostMessageEn: %x\n",           PcdGetBool(PcdPostMessageEn)));    
    DEBUG((EFI_D_ERROR, "PcdPostMessageProgressCode: %x\n", PcdGetBool(PcdPostMessageProgressCode)));    
    DEBUG((EFI_D_ERROR, "PcdPostMessageErrorCode: %x\n",    PcdGetBool(PcdPostMessageErrorCode)));    
    DEBUG((EFI_D_ERROR, "PcdPostMessageDebugCode: %x\n",    PcdGetBool(PcdPostMessageDebugCode)));    
    DEBUG((EFI_D_ERROR, "PcdPostMessageLogPostMsg: %x\n",   PcdGetBool(PcdPostMessageLogPostMsg)));    
    DEBUG((EFI_D_ERROR, "PcdPostMessageShowPostMsg: %x\n",  PcdGetBool(PcdPostMessageShowPostMsg)));    
    DEBUG((EFI_D_ERROR, "PcdPostMessageBeepPostMsg: %x\n",  PcdGetBool(PcdPostMessageBeepPostMsg))); 
  }
  
  return Status;

}


/**
  Perform Insyde Event Log Policy Init.

  @param [in] FileHandle           Handle of the file being invoked. 
  @param [in] PeiServices          General purpose services available to every PEIM.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
InsydeEventLogPolicyPeiEntry (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                            Status;

  //
  // Update Base address for Bios Event Storage.
  //
  if (PcdGet32 (PcdFlashNvStorageGPNVHANDLE0Base)) {
    PcdSet32(PcdBiosEventStorageBase, PcdGet32 (PcdFlashNvStorageGPNVHANDLE0Base));
  }
  if (PcdGet32 (PcdFlashNvStorageGPNVHANDLE0Size)) {
    PcdSet32(PcdBiosEventStorageSize, PcdGet32 (PcdFlashNvStorageGPNVHANDLE0Size));
  }

  //
  // Update policy by "Setup" variable.
  //
  Status = UpdateInsydeEventLogPolicy (PeiServices);
  if (EFI_ERROR (Status)) {
    //
    // Register callback function for Read Only Variable2 PPI.
    //
    Status = (**PeiServices).NotifyPpi (PeiServices, &mReadOnlyVariable2PpiNotify); 
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } 
  
  return Status;
}

