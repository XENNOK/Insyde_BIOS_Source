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
//; Abstract:
//;
//;   Header file for set sensitive variable throuhg SMI related definitions
//;

#ifndef __SENSITIVE_VARIABLE_FORMAT_H__
#define __SENSITIVE_VARIABLE_FORMAT_H__

#include "Tiano.h"

#define SET_SENSITIVE_VARIABLE_SIGNATURE    EFI_SIGNATURE_32 ('S', 'S', 'V', 'S')
#define SET_SENSITIVE_VARIABLE_FUN_NUM      0x21

#pragma pack(push, 1)
typedef struct {
  UINT32                    HeaderSize;
  UINT16                    VariableCount;
//  SENSITIVE_VARIABLE_HEADER Variable[];
} SENSITIVE_VARIABLE_STORE_HEADER;

typedef struct {
  UINT32    Attributes;
  UINT32    DataSize;
  EFI_GUID  VendorGuid;
//  CHAR16    VariableName[];
//  UINT      Data[];
} SENSITIVE_VARIABLE_HEADER;

typedef struct {
  UINT32                             Signature;  ///< Particular sensitive variable signature. This signature must be 'S', 'S', 'V', 'S'
  UINT32                             DataSize;   ///< The size, in bytes, of whole input data.
  EFI_STATUS                         Status;     ///< The return status of setting sensitive variable.
//  SENSITIVE_VARIABLE_STORE_HEADER    VariableData;
} SENSITIVE_VARIABLE_AUTHENTICATION;
#pragma pack(pop)

#endif
