/** @file
  Header file for set sensitive variable throuhg SMI related definitions

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


#ifndef __SENSITIVE_VARIABLE_FORMAT_H__
#define __SENSITIVE_VARIABLE_FORMAT_H__

#define SET_SENSITIVE_VARIABLE_SIGNATURE    SIGNATURE_32 ('S', 'S', 'V', 'S')
#define SET_SENSITIVE_VARIABLE_FUN_NUM      0x21

#pragma pack(push, 1)
typedef struct {
  UINT32                    HeaderSize;    ///< The size, in bytes, of SENSITIVE_VARIABLE_STORE_HEADER structure.
  UINT16                    VariableCount; ///< The number of sensitive variable in the sensitive variable store.
//  SENSITIVE_VARIABLE_HEADER Variable[];
} SENSITIVE_VARIABLE_STORE_HEADER;

typedef struct {
  UINT32    Attributes;  ///< The size, in bytes, of SENSITIVE_VARIABLE_STORE_HEADER structure.
  UINT32    DataSize;    ///< The size, in bytes, of SENSITIVE_VARIABLE_STORE_HEADER structure.
  EFI_GUID  VendorGuid;  ///< The size, in bytes, of SENSITIVE_VARIABLE_STORE_HEADER structure.
//  CHAR16    VariableName[];
//  UINT      Data[];
} SENSITIVE_VARIABLE_HEADER;

typedef struct {
  UINT32                             Signature;  ///< Particular sensitive variable signature. This signature must be 'S', 'S', 'V', 'S'
  UINT32                             DataSize;   ///< The size, in bytes, of whole input data.
  EFI_STATUS                         Status;     ///< Return Status code after setting sensitive variable.
//  SENSITIVE_VARIABLE_STORE_HEADER    VariableData;
} SENSITIVE_VARIABLE_AUTHENTICATION;
#pragma pack(pop)


#endif
