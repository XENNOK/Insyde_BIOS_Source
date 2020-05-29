//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   SensitiveVariableLib.h
//;
//; Abstract:
//;
//;   Header file for Sensitive Variable Library
//;

#ifndef _SENSITIVE_VARIABLE_LIB_H_
#define _SENSITIVE_VARIABLE_LIB_H_

#include "Tiano.h"
#include "SensitiveVariableFormat.h"

EFI_STATUS
SetVariableToSensitiveVariable (
  IN   CHAR16                           *VariableName,
  IN   EFI_GUID                         *VendorGuid,
  IN   UINT32                           Attributes,
  IN   UINTN                            DataSize,
  IN   VOID                             *Data
  )
/*++

Routine Description:

  Set changed sensitive setup data to sensitive setup variable.

Arguments:

  VariableName          - A pointer to a null-terminated string that is the variable's name.
  VendorGuid            - A pointer to an EFI_GUID that is the variable's GUID. The combination of
                          VendorGuid and VariableName must be unique.
  Attributes            - Attributes bitmask to set for the variable.
  DataSize              - The data size by byte of variable
  Data                  - Pointer to the start address of data.

Returns:

  EFI_SUCCESS           - Set data to sensitive variable successfully.
  EFI_INVALID_PARAMETER - Any input parameter is incorrect.
  EFI_NOT_FOUND         - Cannot set specific variable in sensitive setup variable.

--*/
;



#endif


