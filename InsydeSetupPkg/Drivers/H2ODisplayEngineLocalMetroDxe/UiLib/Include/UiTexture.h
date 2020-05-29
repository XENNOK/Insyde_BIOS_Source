/** @file

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

#ifndef _UI_TEXTUTE_H
#define _UI_TEXTUTE_H

typedef struct _UI_TEXTURE UI_TEXTURE;
typedef struct _UI_TEXTURE_CLASS UI_TEXTURE_CLASS;

UI_TEXTURE_CLASS *
EFIAPI
GetTextureClass (
  VOID
  );

struct _UI_TEXTURE {
  UI_CONTROL                    Control;
};

struct _UI_TEXTURE_CLASS {
  UI_CONTROL_CLASS              ParentClass;
};

#endif
