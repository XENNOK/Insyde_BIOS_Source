/** @file
 Define function of value operation .
;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "H2ODisplayEngineLib.h"

/**
 Create value in UINT64 type.

 @param [in] TargetHiiValue     HII value which set type as UINT64
 @param [in] ValueUint64        Value for UINT64 type

**/
EFI_STATUS
CreateValueAsUint64 (
  IN EFI_HII_VALUE  *TargetHiiValue,
  IN UINT64         ValueUint64
  )
{
  if (TargetHiiValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TargetHiiValue->Type      = EFI_IFR_TYPE_NUM_SIZE_64;
  TargetHiiValue->Value.u64 = ValueUint64;

  return EFI_SUCCESS;
}

/**
 Create value in string type.

 @param [in] TargetHiiValue        HII value which set type as string
 @param [in] Buffer                String buffer

**/
EFI_STATUS
CreateValueAsString (
  IN EFI_HII_VALUE   *TargetHiiValue,
  IN UINT16          BufferLen,
  IN UINT8           *Buffer
  )
{
  if (TargetHiiValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TargetHiiValue->Type      = EFI_IFR_TYPE_STRING;
  TargetHiiValue->BufferLen = BufferLen;
  TargetHiiValue->Buffer    = Buffer;

  return EFI_SUCCESS;
}

