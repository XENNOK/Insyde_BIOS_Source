/** @file
  This Library will install CrConfigDefaultLib for reference.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include <CrSetupConfig.h>

#include <Library/CrVfrConfigLib.h>
#include <Library/VariableLib.h>
#include <Library/CrBdsLib.h>

extern UINT8  AdvanceVfrCrConfigDefault0000[];

/**

  Extract default CrConfig data from specific VFR forms.

  @param NvData          A pointer to the CrConfig data buffer
  @param VfrDefault      The VFR default data of a single VFR form

  @retval EFI_SUCCESS    Extract CrConfig data successfully

**/
EFI_STATUS
CrExtractVfrDefault (
  UINT8 *NvData,
  UINT8 *VfrDefault
  )
{
   UINTN   VfrBufSize;
   UINTN   DataSize;
   UINTN   VfrIndex;
   UINTN   NvDataIndex;

   VfrBufSize = (UINTN)( *(UINT32 *)VfrDefault );
   VfrIndex = sizeof (UINT32);
   do {
     NvDataIndex  = *(UINT16 *)(VfrDefault + VfrIndex);
     VfrIndex    += 2;
     DataSize     = *(UINT16 *)(VfrDefault + VfrIndex);
     VfrIndex    += 2;
     CopyMem(NvData+NvDataIndex, VfrDefault+VfrIndex, DataSize);
     VfrIndex += DataSize;
   } while (VfrIndex < VfrBufSize);

   return EFI_SUCCESS;
}


/**

  Extract default CrConfig data.

  @param CrConfigDefault     A pointer to the CrConfig data buffer

  @retval EFI_SUCCESS        Extract CrConfig data successfully

**/
EFI_STATUS
ExtractCrConfigDefault (
  UINT8 *CrConfigDefault
  )
{
  CrExtractVfrDefault (
    CrConfigDefault,
    AdvanceVfrCrConfigDefault0000
  );
  
  return EFI_SUCCESS;
}


/**
  Initial CrConfig variable data

  @retval EFI_SUCCESS        Initial CrConfig variable data successfully

**/
EFI_STATUS
CrConfigVarInit (void)
{
  EFI_STATUS             Status;
  CR_CONFIGURATION       *CrConfig;
  CR_CONFIGURATION       *CrConfigDefault;
  
  CrConfigDefault = CommonGetVariableData (L"CrConfigDefault", &gCrConfigurationGuid);
  if (CrConfigDefault == NULL) {
    CrConfigDefault = (CR_CONFIGURATION*)AllocatePool (sizeof (CR_CONFIGURATION));
    ExtractCrConfigDefault ((UINT8*)CrConfigDefault);
    
    Status = CommonSetVariable (
               L"CrConfigDefault",
               &gCrConfigurationGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               sizeof (CR_CONFIGURATION),
               CrConfigDefault
               );
    
  }

  CrConfig = CommonGetVariableData (L"CrConfig", &gCrConfigurationGuid);
  if (CrConfig == NULL) {
    Status = CommonSetVariable (
               L"CrConfig",
               &gCrConfigurationGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               sizeof (CR_CONFIGURATION),
               CrConfigDefault
               );    
  }

  if (CrConfigDefault != NULL) {
    FreePool (CrConfigDefault);
  }
  
  if (CrConfig != NULL) {
    FreePool (CrConfig);
  }

  return EFI_SUCCESS;
}


