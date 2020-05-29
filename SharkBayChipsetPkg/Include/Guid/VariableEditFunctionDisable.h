/** @file
  VariableEditFunctionDisable.h

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

#ifndef _VARIABLE_EDIT_FUNCTION_DISABLE_GUID_H_
#define _VARIABLE_EDIT_FUNCTION_DISABLE_GUID_H_

// {EF0849B6-FAD0-40e9-9107-974AEB8787A2}
#define VARIABLE_EDIT_FUNCTION_DISABLE_GUID  \
  { \
    0xef0849b6, 0xfad0, 0x40e9, { 0x91, 0x7, 0x97, 0x4a, 0xeb, 0x87, 0x87, 0xa2 } \
  }

#define VARIABLE_EDIT_FUNCTION_DISABLE_VARIABLE_NAME    L"VarEditFunDis"

#pragma pack(push, 1)
typedef struct {
  UINT32             VariableEditFunDis   :  1;
  UINT32             Reserve              : 31;
} VARIABLE_EDIT_FUNCTION_DISABLE_SETTING;
#pragma pack(pop)

extern EFI_GUID                               gVarableEditFunctionDisableGuid;

#endif
