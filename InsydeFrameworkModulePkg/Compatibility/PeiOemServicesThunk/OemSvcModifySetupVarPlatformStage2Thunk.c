/** @file
  Initial Setup Variable setting depends on project characteristic.

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
#include <Library/PeiOemSvcKernelLib.h>

/**
  Initial Setup Variable setting depends on project characteristic.

  @param  *SystemConfiguration  A pointer to SYSTEM_CONFIGURATION.
  @param  SetupVariableExist    Setup variable be found in variable storage or not.

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcModifySetupVarPlatformStage2Thunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST               Marker;
  VOID                  *Buffer;
  BOOLEAN               SetupVariableExist;
  EFI_STATUS            Status;

  VA_START (Marker, NumOfArgs);
  Buffer = VA_ARG (Marker, VOID *);
  SetupVariableExist = VA_ARG (Marker, BOOLEAN);
  VA_END (Marker);

  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcModifySetupVarPlatformStage2 (
             Buffer,
             SetupVariableExist
             );

  return Status;
}

