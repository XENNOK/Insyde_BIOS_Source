/** @file
    Provide support functions for sensitive variable.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SensitiveVariable.h"


/**
  This fucnitons uses to set sensitive variable in SMM.

  @retval EFI_SUCCESS     Set sensitive variable successfully.
  @return Other           Set sensitive variable failed.

**/
EFI_STATUS
SmmSetSensitiveVariable (
  VOID
  )
{
  UINT32                              BufferSize;
  SENSITIVE_VARIABLE_AUTHENTICATION   *VariableBuffer;
  VARIABLE_POINTER_TRACK              Variable;
  UINTN                               VariableCount;
  SENSITIVE_VARIABLE_STORE_HEADER     *SensitiveStoreHeader;
  SENSITIVE_VARIABLE_HEADER           *SensitiveVariable;
  UINT16                              VariableIndex;
  EFI_STATUS                          Status;


  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  VariableBuffer = NULL;
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);

  if (VariableBuffer->Signature != SET_SENSITIVE_VARIABLE_SIGNATURE || VariableBuffer->DataSize != BufferSize) {
    return EFI_UNSUPPORTED;
  }

  VariableBuffer->Status = EFI_SUCCESS;
  SensitiveStoreHeader   = (SENSITIVE_VARIABLE_STORE_HEADER *)(VariableBuffer + 1);
  VariableIndex          = 0;
  SensitiveVariable      = GetNextSensitiveVariable (SensitiveStoreHeader, NULL, &VariableIndex);
  while (SensitiveVariable != NULL) {
    if ((SensitiveVariable->Attributes & (EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) != 0) {
      VariableBuffer->Status = EFI_ACCESS_DENIED;
      break;
    }
    Status = FindVariableByLifetime (
               (CHAR16 *) (SensitiveVariable + 1),
               &SensitiveVariable->VendorGuid,
               &Variable,
               &VariableCount,
               &mVariableModuleGlobal->VariableBase
               );
    Status = UpdateVariable (
               (CHAR16 *) (SensitiveVariable + 1),
               &SensitiveVariable->VendorGuid,
               GetSensitiveVariableDataPtr (SensitiveVariable),
               SensitiveVariable->DataSize,
               SensitiveVariable->Attributes,
               0,
               0,
               &Variable,
               NULL,
               &mVariableModuleGlobal->VariableBase
               );
    if (EFI_ERROR (Status)) {
      VariableBuffer->Status = Status;
      ASSERT (FALSE);
      break;
    }
    SensitiveVariable = GetNextSensitiveVariable (SensitiveStoreHeader, SensitiveVariable, &VariableIndex);
  }

  return VariableBuffer->Status;
}

/**
  Dummy function to set sensitive variable in SMM.

  @return EFI_UNSUPPORTED  Always return EFI_UNSUPPORTED.
**/
EFI_STATUS
DummySmmSetSensitiveVariable (
  VOID
  )
{
  UINT32                              BufferSize;
  SENSITIVE_VARIABLE_AUTHENTICATION   *VariableBuffer;

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  VariableBuffer = NULL;
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);

  if (VariableBuffer->Signature != SET_SENSITIVE_VARIABLE_SIGNATURE || VariableBuffer->DataSize != BufferSize) {
    return EFI_UNSUPPORTED;
  }

  VariableBuffer->Status = EFI_UNSUPPORTED;
  return EFI_UNSUPPORTED;

}

/**
  According to input parameter to enable or disable set sensitive variable capability.

  @param[in]  Enable    TRUE : Enable set sensitive variable capability.
                        FALSE: Disable set sensitive variable capability.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
STATIC
EFI_STATUS
EnableSetSensitiveVariable (
  IN BOOLEAN       Enable
  )
{
  UINTN                Index;

  for (Index = 0; mNonSecureBootFunctionsTable[Index].SmiSubFunction != NULL ; Index++) {
    if (mNonSecureBootFunctionsTable[Index].FunNum == SET_SENSITIVE_VARIABLE_FUN_NUM) {
      mNonSecureBootFunctionsTable[Index].SmiSubFunction = Enable ? SmmSetSensitiveVariable : DummySmmSetSensitiveVariable;
      break;
    }
  }

  for (Index = 0; mSecureBootFunctionsTable[Index].SmiSubFunction != NULL ; Index++) {
    if (mSecureBootFunctionsTable[Index].FunNum == SET_SENSITIVE_VARIABLE_FUN_NUM) {
      mSecureBootFunctionsTable[Index].SmiSubFunction = Enable ? SmmSetSensitiveVariable : DummySmmSetSensitiveVariable;
      break;
    }
  }

  return EFI_SUCCESS;
}


/**
  Function to enable/disable set sensitive variable capability.

  @retval EFI_SUCCESS     Enable or disable set sensitive variable capability successfully.
  @return Other           Any error occurred while enabling/disabling set sensitive variable capability.
**/
EFI_STATUS
SmmControlSensitiveCapability (
  VOID
  )
{
  UINT32         BufferSize;
  UINT8          *VariableBuffer;
  UINT32         Signature;

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  VariableBuffer = NULL;
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);

  if (BufferSize != sizeof (UINT32)) {
    return EFI_UNSUPPORTED;
  }

  Signature = DISABLE_SET_SENSITIVE_SIGNATURE;
  if (CompareMem (VariableBuffer, &Signature, sizeof (UINT32))== 0) {
    return EnableSetSensitiveVariable (FALSE);
  }
  Signature = ENABLE_SET_SENSITIVE_SIGNATURE;
  if (CompareMem (VariableBuffer, &Signature, sizeof (UINT32))== 0) {
    return EnableSetSensitiveVariable (TRUE);
  }
 return EFI_UNSUPPORTED;


}

