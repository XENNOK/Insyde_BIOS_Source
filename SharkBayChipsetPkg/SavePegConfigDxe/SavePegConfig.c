/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
//[-start-121113-IB10820161-modify]//
#include <Guid/SaDataHob.h>
#include <Library/HobLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/PegDataVariable.h>
//[-end-121113-IB10820161-modify]//

EFI_STATUS
EFIAPI
SavePegConfigEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  SA_DATA_HOB               *SaDataHob;
  SA_PEG_DATA               VarPegData;
  VOID                      *PegDataPtr;
  UINTN                     PegDataSize;
  EFI_STATUS                Status;
  UINTN                     VarSize;
  UINT32                    VarAttrib;
  BOOLEAN                   VariableUpdate;

  Status                 = EFI_SUCCESS;
  SaDataHob              = NULL;
  SaDataHob              = (SA_DATA_HOB *)GetFirstGuidHob (&gSaDataHobGuid);
  PegDataSize            = sizeof(SA_PEG_DATA);
  PegDataPtr             = (VOID *) &(SaDataHob->PegData);
  VarAttrib              = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  VarSize                = sizeof(SA_PEG_DATA);
  VariableUpdate         = FALSE;

  Status = gRT->GetVariable (
                  PEG_DATA_VARIABLE_NAME,
                  &gPegDataVariableGuid,
                  &VarAttrib,
                  &VarSize,
                  &VarPegData
                  );
  if (EFI_ERROR (Status)) {
    VariableUpdate = TRUE;
  } else {
    if (CompareMem (&VarPegData, PegDataPtr, PegDataSize) != 0) {
      //
      // PegDataVariable does not match with current data, system needs to update variable.
      //
      VariableUpdate = TRUE;
    }
  }
  
  if (VariableUpdate) {
    Status = gRT->SetVariable (
                    PEG_DATA_VARIABLE_NAME,
                    &gPegDataVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    PegDataSize,
                    PegDataPtr
                    );
  }
  return Status;
}
