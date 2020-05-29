/** @file
  HII Form Representation

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _INTERNAL_FORM_REPRESENTATION_H_
#define _INTERNAL_FORM_REPRESENTATION_H_

#include <Uefi/UefiInternalFormRepresentation.h>

#define IMAGE_TOKEN(t)     t
#define ANIMATION_TOKEN(t) t


#define H2O_HII_IIBT_IMAGE_32BIT       0x80
#define H2O_IMAGE_ALPHA_CHANNEL        0x80000000

#pragma pack(1)

typedef struct _H2O_HII_RGBA_PIXEL {
  UINT8                        b;
  UINT8                        g;
  UINT8                        r;
  UINT8                        a;
} H2O_HII_RGBA_PIXEL;

typedef struct _H2O_HII_IIBT_IMAGE_32BIT_BLOCK {
  EFI_HII_IMAGE_BLOCK              Header;
  UINT8                            BlockType2;
  UINT32                           Length;
  UINT16                           Width;
  UINT16                           Height;
  H2O_HII_RGBA_PIXEL               Bitmap[1];
} H2O_HII_IIBT_IMAGE_32BIT_BLOCK;

#pragma pack()


#endif

