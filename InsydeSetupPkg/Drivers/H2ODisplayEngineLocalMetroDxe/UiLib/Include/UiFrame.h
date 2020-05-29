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

#ifndef _UI_FRAME_H
#define _UI_FRAME_H

typedef struct _UI_FRAME UI_FRAME;
typedef struct _UI_FRAME_CLASS UI_FRAME_CLASS;

UI_FRAME_CLASS *
EFIAPI
GetFrameClass (
  VOID
  );

struct _UI_FRAME {
  UI_CONTROL                    Control;
  BOOLEAN                       DoOffScreen;
};


struct _UI_FRAME_CLASS {
  UI_CONTROL_CLASS              ParentClass;
};

#endif
