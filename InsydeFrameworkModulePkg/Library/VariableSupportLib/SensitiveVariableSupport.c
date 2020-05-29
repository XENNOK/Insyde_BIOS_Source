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
//;   SensitiveVariable.c
//;
//; Abstract:
//;
//;   Common Sensitive Variable supported functions
//;
//;

#include "VariableSupportLib.h"
#include "EfiCommonLib.h"

UINTN
GetSensitiveVariableSize (
  IN      CONST SENSITIVE_VARIABLE_HEADER           *VariableHeader
  )
/*++

Routine Description:

  Get whole single sensitive variable size. This size includes header size, name size and data size.

Arguments:

  VariableHeader  - Pointer to SENSITIVE_VARIABLE_HEADER instance.

Returns:

  Size of sensitive variable data size in bytes, or 0 if the VariableHeader is invalid.

--*/
{
  if (VariableHeader == NULL) {
    return 0;
  }
  return (UINTN) (GetSensitiveVariableDataPtr (VariableHeader) + VariableHeader->DataSize - (UINT8 *) VariableHeader);
}

SENSITIVE_VARIABLE_HEADER *
GetNextSensitiveVariable (
  IN      CONST SENSITIVE_VARIABLE_STORE_HEADER     *StoreHeader,
  IN      CONST SENSITIVE_VARIABLE_HEADER           *VariableHeader,
  IN OUT        UINT16                              *VariableNum
  )
/*++

Routine Description:

  Function uses to get next sensitive variable and return the current variable number in sensitive variable.

Arguments:

  StoreHeader                  - Pointer to SENSITIVE_VARIABLE_STORE_HEADER instance.
  VariableHeader               - Pointer to SENSITIVE_VARIABLE_HEADER instance.
  VariableNum                  - [in]:  Supplies the last variable number returned by GetNextSensitiveVariable().
                                 [out]: Returns the variable number of the current variable.

Returns:

  Pointer to SENSITIVE_VARIABLE_HEADER instance, or NULL if cannot find next sensitive variable.

--*/
{
  SENSITIVE_VARIABLE_HEADER        *NextVariableHeader;

  if (StoreHeader == NULL || VariableNum == NULL) {
    return NULL;
  }

  if (VariableHeader == NULL && *VariableNum != 0) {
    return NULL;
  }

  if (StoreHeader->VariableCount <= *VariableNum) {
    return NULL;
  }

  if (*VariableNum == 0) {
    NextVariableHeader = (SENSITIVE_VARIABLE_HEADER *) (StoreHeader + 1);
  } else {
    NextVariableHeader = (SENSITIVE_VARIABLE_HEADER *) (GetSensitiveVariableDataPtr (VariableHeader) + VariableHeader->DataSize);
  }
  *VariableNum += 1;

  return NextVariableHeader;
}


UINT8 *
GetSensitiveVariableDataPtr (
  IN CONST SENSITIVE_VARIABLE_HEADER   *VariableHeader
  )
/*++

Routine Description:

  This code gets the pointer to the sensitive variable data.

Arguments:

  Variable  - Name of the variable to be found.

Returns:

  Pointer to sensitive variable data, or NULL if VariableHeader is invalid.

--*/
{
   if (VariableHeader == NULL) {
     return NULL;
   }
   return ((UINT8 *) VariableHeader) + sizeof (SENSITIVE_VARIABLE_HEADER) + EfiStrSize ((CHAR16 *) (VariableHeader + 1));
}