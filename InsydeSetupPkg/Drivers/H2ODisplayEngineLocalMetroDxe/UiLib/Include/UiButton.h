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

#ifndef _UI_BUTTON_H
#define _UI_BUTTON_H

typedef struct _UI_BUTTON UI_BUTTON;
typedef struct _UI_BUTTON_CLASS UI_BUTTON_CLASS;

UI_BUTTON_CLASS *
EFIAPI
GetButtonClass (
  VOID
  );

struct _UI_BUTTON {
  UI_LABEL                      Label;

  CHAR16                        *NormalImage;
  CHAR16                        *HoverImage;
  CHAR16                        *PushImage;
};

struct _UI_BUTTON_CLASS {
  UI_LABEL_CLASS                ParentClass;
};

#endif
