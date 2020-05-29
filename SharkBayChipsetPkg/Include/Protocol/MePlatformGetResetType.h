/** @file

  Interface definition Me Platform Get Reset Type.

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


#ifndef _ME_PLATFORM_GET_RESET_TYPE_H_
#define _ME_PLATFORM_GET_RESET_TYPE_H_

#define ME_PLATFORM_GET_RESET_TYPE_GUID \
  { \
    0xb8cdced7, 0xbdc4, 0x4464, 0x9a, 0x1a, 0xff, 0x3f, 0xbd, 0xf7, 0x48, 0x69 \
  }

#define ME_PLATFORM_GET_RESET_TYPE_PROTOCOL_REVISION  1
extern EFI_GUID gMePlatformGetResetTypeGuid;

///
/// ME_SPEICAL_RESET_TYPES must be aligned with PCH_EXTENDED_RESET_TYPES
///
typedef enum {
  PowerCycleResetReq  = 3,
  GlobalResetReq,
  GlobalResetWithEcReq,
  MaxRestReq
} ME_SPEICAL_RESET_TYPES;

/**
  Get Platform requested reset type

  @param[in] Type                 UEFI defined reset type

  @retval ME_SPEICAL_RESET_TYPES  ME reset type aligned with PCH_EXTENDED_RESET_TYPES
**/
typedef
ME_SPEICAL_RESET_TYPES
(EFIAPI *GET_RESET_TYPE) (
  IN EFI_RESET_TYPE               Type
  );

typedef struct _ME_PLATFORM_GET_RESET_TYPE_PROTOCOL {
  GET_RESET_TYPE                  GetResetType;
} ME_PLATFORM_GET_RESET_TYPE_PROTOCOL;

#endif
