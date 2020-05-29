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

#ifndef _UI_SWITCH_H
#define _UI_SWITCH_H

typedef struct _UI_SWITCH UI_SWITCH;
typedef struct _UI_SWITCH_CLASS UI_SWITCH_CLASS;

UI_SWITCH_CLASS *
EFIAPI
GetSwitchClass (
  VOID
  );

struct _UI_SWITCH {
  UI_BUTTON                     Button;
  BOOLEAN                       CheckBoxValue;
  RECT                          ThumbRc;
  INT32                         ThumbPos;
  INT32                         ThumbRange;
  BOOLEAN                       InitThumb;
  BOOLEAN                       IsHitThumb;
  BOOLEAN                       MoveThumb;
};

struct _UI_SWITCH_CLASS {
  UI_BUTTON_CLASS               ParentClass;
};


#endif
