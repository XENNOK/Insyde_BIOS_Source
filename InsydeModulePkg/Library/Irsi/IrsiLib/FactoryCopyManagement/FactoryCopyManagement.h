/** @file
  Definitions for IRSI FactoryCopyMangement function

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

#ifndef _FACTORY_COPY_MANAGEMENT_H_
#define _FACTORY_COPY_MANAGEMENT_H_

#define FACTORY_COPY_HEADER_SIZE            8
#define FACTORY_COPY_SIGNATURE              SIGNATURE_32 ('_','F','D','C')
#define EFI_GLOBAL_VARIABLE_GUID \
  { \
    0x8BE4DF61, 0x93CA, 0x11d2, {0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C} \
  }

///
/// ECP Variable Store region header.
///
#pragma pack(1)
typedef struct {
  UINT32  Signature;
  UINT32  Size;
  UINT8   Format;
  UINT8   State;
  UINT16  Reserved;
  UINT32  Reserved1;
} ECP_VARIABLE_STORE_HEADER;
#pragma pack()

typedef struct {
  CHAR16    VariableName[32];
  EFI_GUID  VendorGuid;
} VAR_NAME_GUID;

#endif