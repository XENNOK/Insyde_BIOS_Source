/** @file
 Library Instance for PeiKernelConfigLib Library Class.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//
#include <ChipsetSetupConfig.h>
#include <Guid/SetupDefaultHob.h>

/**
 Retrieve kernel setup configuration data

 @param[out] KernelConfig       Pointer to the structure of KERNEL_CONFIGURATION,
                                this pointer must be allocated with sizeof(KERNEL_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The kernel configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input KernelConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
GetKernelConfiguration (
  OUT KERNEL_CONFIGURATION         *KernelConfig
  )
{
  EFI_STATUS                       Status;
//[-start-130709-IB0516-modify]//
  VOID                             *SystemConfiguration;
//[-end-130709-IB0516-modify]//
  UINTN                            Size;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi;

  if (KernelConfig == NULL) {
    ASSERT_EFI_ERROR (KernelConfig != NULL);
    return EFI_INVALID_PARAMETER;
  }

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariablePpi
           );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
//[-start-130709-IB05160465-modify]//
  Size = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocatePool(Size);
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          SETUP_VARIABLE_NAME,
                          &gSystemConfigurationGuid,
                          NULL,
                          &Size,
                          SystemConfiguration
                          );
  if (Status == EFI_SUCCESS) {
    CopyMem (KernelConfig, SystemConfiguration, sizeof(KERNEL_CONFIGURATION));
  }
  FreePool (SystemConfiguration);
//[-end-130709-IB05160465-modify]//
  return Status;
}

/**
 Retrieve the size of Setup variable

 @return                        Size of the setup variable data
**/
UINTN
EFIAPI
GetSetupVariableSize (
  VOID
  )
{
//[-start-130709-IB05160465-modify]//
  return PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
}

VOID
InvalidateSetupData (
  IN        CONST GUID        *CallBackGuid, OPTIONAL
  IN        UINTN             CallBackToken,
  IN  OUT   VOID              *TokenData,
  IN        UINTN             TokenDataSize
  )
{
  EFI_PEI_HOB_POINTERS                      GuidHob;
  CHIPSET_CONFIGURATION                      *SetupData;

  LibPcdCancelCallback (NULL, PcdToken(PcdSetupDataInvalid), InvalidateSetupData);

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gSetupDefaultHobGuid, GuidHob.Raw);
  if (GuidHob.Raw == NULL) {
    ASSERT (GuidHob.Raw != NULL);
    return;
  }

  SetupData = (CHIPSET_CONFIGURATION *)GET_GUID_HOB_DATA (GuidHob.Guid);
  SetupData->SetupVariableInvalid = 1;

}

EFI_STATUS
EFIAPI
KernelConfigLibInit (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  LibPcdCallbackOnSet (NULL, PcdToken(PcdSetupDataInvalid), InvalidateSetupData);
  return EFI_SUCCESS;
}
