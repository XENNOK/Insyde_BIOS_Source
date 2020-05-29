/** @file
 

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _STRING_TRANSFORM_H_
#define _STRING_TRANSFORM_H_


#define STRING_TRANSFORM_PROTOCOL_GUID \
  {0x404DAB9C, 0xEE13, 0x483c, {0xBD, 0x3A, 0x33, 0x5B, 0xC0, 0x91, 0x7C, 0x6D}}

typedef struct _STRING_TRANSFORM_PROTOCOL STRING_TRANSFORM_PROTOCOL;


typedef
EFI_STATUS
(EFIAPI *SYSTEM_INFO_TRANSFORM) (
  IN       STRING_TRANSFORM_PROTOCOL                *This,
  IN CONST CHAR16                                   *String,
  OUT      CHAR16                                   **OutputString
  );

//
// String Transform Protocol
//
typedef struct _STRING_TRANSFORM_PROTOCOL {
  SYSTEM_INFO_TRANSFORM        Transform;
};


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gStringTransformProtocolGuid;


#endif

