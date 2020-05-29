/** @file
  Thunk driver for OEM_SERVICES_PPI

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
#include <Ppi/MemoryDiscovered.h>
#include <OemServices/Kernel.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>

typedef struct {
  UINT32                                Signature;
  EFI_PEI_PPI_DESCRIPTOR                PpiDescriptor;
  OEM_SERVICES_PPI                      OemServicesPpi;
} OEM_SERVIES_PEI_INSTANCE;

//
// function prototypes
//
EFI_STATUS
NullPeiOemSvcFunc (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

VOID
AssignNullPeiOemSvcFunc (
  IN OUT OEM_SERVICES_FUNCTION          *OemServicesFunc,
  IN UINT32                             NumOfEnum
  );

EFI_STATUS
RegisterPeiOemSvcFunc (
  IN  OEM_SERVICES_PPI                  *OemServicesPpi
  );

//
// callback function
//
EFI_STATUS
EFIAPI
ReRegisterOemServicesMemoryDiscoveredCallback (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN VOID                               *Ppi
  );

//
// Module variables
//
static EFI_PEI_NOTIFY_DESCRIPTOR mReRegisterOemServicesNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  ReRegisterOemServicesMemoryDiscoveredCallback
};

//
// helper functions implementation
//

/**
  An empty OEM_SERVICES_PPI function.

  @param  **PeiServices         PeiServices 
  @param  *This                 OEM_SERVICES_PPI
  @param  NumOfArgs             Number of variable arguments

  @retval EFI_UNSUPPORTED       Returns unsupported by default.
**/
EFI_STATUS
NullPeiOemSvcFunc (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Assign empty OEM_SERVICES_PPI functions by default.

  @param  *OemServicesFunc      Array of OEM_SERVICES_PPI functions.
  @param  NumOfEnum             Number of functions in array.

  @retval                       None
**/
VOID
AssignNullPeiOemSvcFunc (
  IN OUT OEM_SERVICES_FUNCTION          *OemServicesFunc,
  IN UINT32                             NumOfEnum
  )
{
  UINT32  Index;
  
  for (Index = 0; Index < NumOfEnum; Index++) {
    OemServicesFunc[Index] = NullPeiOemSvcFunc;
  }
}

/**
  Register OEM_SERVICES_PPI functions.

  @param  *OemServicesPpi       OEM_SERVICES_PPI

  @retval EFI_SUCCESS           Always return success.
**/
EFI_STATUS
RegisterPeiOemSvcFunc (
  IN  OEM_SERVICES_PPI                  *OemServicesPpi
  )
{
  OemServicesPpi->Services[COMMON_BOOT_MODE_CREATE_FV]             = OemSvcBootModeCreateFvThunk;
  OemServicesPpi->Services[COMMON_CHANGE_BOOT_MODE]                = OemSvcChangeBootModeThunk;
  OemServicesPpi->Services[COMMON_GET_SMART_BOOT_POLICY]           = OemSvcGetSmartBootPolicyThunk;
  OemServicesPpi->Services[COMMON_GET_PEI_CRISIS_PROTECT_TABLE]    = OemSvcGetProtectTableThunk;
  OemServicesPpi->Services[COMMON_SET_PLATFORM_HARDWARE_SWITCH]    = OemSvcSetPlatformHardwareSwitchThunk;
  OemServicesPpi->Services[COMMON_SET_DIMM_SMBUS_ADDR]             = OemSvcSetDimmSmbusAddrThunk;
  OemServicesPpi->Services[COMMON_MODIFY_CLOCKGEN_INFO]            = OemSvcChipsetModifyClockGenInfoThunk;
  OemServicesPpi->Services[COMMON_AFTER_MODIFY_CLOCKGEN]           = OemSvcChipsetAfterModifyClockGenThunk;
  OemServicesPpi->Services[COMMON_DETECT_RECOVERY_REQUEST]         = OemSvcDetectRecoveryRequestThunk;
  OemServicesPpi->Services[COMMON_SET_RECOVERY_REQUEST]            = OemSvcSetRecoveryRequestThunk;
  OemServicesPpi->Services[COMMON_HOOK_WHEN_RECOVERY_FAIL]         = OemSvcHookWhenRecoveryFailThunk;
  OemServicesPpi->Services[COMMON_INIT_PLATFORMSTAGE1]             = OemSvcInitPlatformStage1Thunk;
  OemServicesPpi->Services[COMMON_MODIFY_SETUPVAR_PLATFORMSTAGE2]  = OemSvcModifySetupVarPlatformStage2Thunk;
  OemServicesPpi->Services[COMMON_INIT_PLATFORMSTAGE2]             = OemSvcInitPlatformStage2Thunk;
  OemServicesPpi->Services[COMMON_IS_BOOT_WITH_NO_CHANGE]          = OemSvcIsBootWithNoChangeThunk;
  OemServicesPpi->Services[COMMON_INIT_MEMORY_DISCOVER_CALLBACK]   = OemSvcInitMemoryDiscoverCallbackThunk;
  OemServicesPpi->Services[COMMON_SIO_PEI_INIT]                    = OemSvcSioPeiInitThunk;
  OemServicesPpi->Services[COMMON_GET_VERB_TABLE]                  = OemSvcGetVerbTableThunk;

  return EFI_SUCCESS;
}

/**
  Entry point of the Thunk driver.

  @param  FileHandle            The file handle of the image.
  @param  **PeiServices         PeiServices 

  @retval EFI_SUCCESS           Register OEM_SERVICES_PPI functions successfully.
  @retval Other                 Failed. 
**/
EFI_STATUS
EFIAPI
PeiOemSvcThunkEntryPoint (
  IN EFI_PEI_FILE_HANDLE                FileHandle,
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                            Status;
  OEM_SERVIES_PEI_INSTANCE              *PrivateData;

  PrivateData = AllocateZeroPool (sizeof (OEM_SERVIES_PEI_INSTANCE));
  if (PrivateData != NULL) {
    PrivateData->PpiDescriptor.Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    PrivateData->PpiDescriptor.Guid  = &gOemServicesPpiGuid;
    PrivateData->PpiDescriptor.Ppi   = &PrivateData->OemServicesPpi;
    
    PrivateData->OemServicesPpi.Services = (OEM_SERVICES_FUNCTION *)AllocateZeroPool (sizeof (OEM_SERVICES_FUNCTION) * PEI_MAX_NUM_COMMON_SUPPORT); 
    if (PrivateData->OemServicesPpi.Services == NULL) {
     return EFI_OUT_OF_RESOURCES;
    }
    AssignNullPeiOemSvcFunc (PrivateData->OemServicesPpi.Services, PEI_MAX_NUM_COMMON_SUPPORT);
  
  }

  Status = RegisterPeiOemSvcFunc (&PrivateData->OemServicesPpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PeiServicesInstallPpi (&PrivateData->PpiDescriptor);

  //
  // NotifyPpi for MemoryDiscovery
  //
  if (!EFI_ERROR (Status)) {
    Status = PeiServicesNotifyPpi (&mReRegisterOemServicesNotify);
  }
      
  return Status;
}

/**
  ReRegister OemServices after memory be discovered.

  @param PeiServices            General purpose services available to every PEIM.
  @param NotifyDescriptor       Pointer to EFI_PEI_NOTIFY_DESCRIPTOR
  @param Ppi                    EFI_PEI_FV_FILE_LOADER_PPI

  @retval EFI_SUCCESS           ReRegister OemServices success.

**/
EFI_STATUS
EFIAPI
ReRegisterOemServicesMemoryDiscoveredCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                     Status;
  OEM_SERVICES_PPI               *OemServicesPpi;

  Status = PeiServicesLocatePpi (
             &gOemServicesPpiGuid,
             0,
             NULL,
             &OemServicesPpi
             );

  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  OemServicesPpi->Services = (OEM_SERVICES_FUNCTION *)AllocateZeroPool (sizeof (OEM_SERVICES_FUNCTION) * PEI_MAX_NUM_COMMON_SUPPORT);
  if (OemServicesPpi->Services == NULL) {
   return EFI_OUT_OF_RESOURCES;
  }
  AssignNullPeiOemSvcFunc (OemServicesPpi->Services, PEI_MAX_NUM_COMMON_SUPPORT);

  Status = RegisterPeiOemSvcFunc (OemServicesPpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}
