/** @file
  IRSI Validation Library Class definitions

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

#ifndef _IRSI_VALIDATION_LIB_H_
#define _IRSI_VALIDATION_LIB_H_

#include <Uefi.h>

/**
  IRSI function validation

  This routine provides Irsi validation process during Registration

  @param FunctionName           Name of the Irsi function
  @param Guid                   Irsi function GUID
  @param Data                   The Irsi Header pointer

  @retval EFI_SUCCESS           IRSI function validated successfully
  @retval EFI_INVALID_PARAMETER Failed to validate IRSI function

**/
EFI_STATUS
EFIAPI
IrsiRegistrationValidate (
  IN  CHAR16                       *FunctionName,
  IN  EFI_GUID                     *Guid
  );
  
/**
  IRSI function validation

  This routine provides Irsi Header validation process during Runtime

  @param FunctionName           Name of the Irsi function
  @param Guid                   Irsi function GUID
  @param Data                   The Irsi Header pointer

  @retval EFI_SUCCESS           IRSI function validated successfully
  @retval EFI_INVALID_PARAMETER Failed to validate IRSI function

**/
EFI_STATUS
IrsiRuntimeValidate (
  IN  CHAR16        *FunctionName,
  IN  EFI_GUID      *Guid,
  IN  VOID          *Data
  );
  
#endif

