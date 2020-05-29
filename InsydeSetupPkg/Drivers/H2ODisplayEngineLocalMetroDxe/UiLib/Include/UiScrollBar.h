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

#ifndef _UI_SCROLLBAR_H
#define _UI_SCROLLBAR_H

typedef struct _UI_SCROLLBAR UI_SCROLLBAR;
typedef struct _UI_SCROLLBAR_CLASS UI_SCROLLBAR_CLASS;

#define SCROLLBAR_CLASS(Control)  ((UI_SCROLLBAR_CLASS *)(((UI_CONTROL *)(Control))->Class))

UI_SCROLLBAR_CLASS *
EFIAPI
GetScrollBarClass (
  VOID
  );

typedef
VOID
(EFIAPI *UI_SCROLLBAR_SET_POS) (
  IN UI_SCROLLBAR               *This,
  IN INT32                      Pos
  );

struct _UI_SCROLLBAR {
  UI_CONTROL                    Control;
  UI_CONTROL                    *Owner;

  BOOLEAN                       Horizontal;
  INT32      Range;
  INT32      ScrollPos;
  INT32      LineSize;
  POINT      LastMouse;
  INT32      LastScrollPos;
  INT32      LastScrollOffset;
  INTN       ScrollRepeatDelay;

  UINTN      Button1State;
  UINTN      Button2State;
  UINTN      ThumbState;


  BOOLEAN    ShowButton1;
  BOOLEAN    ShowButton2;
  BOOLEAN    ShowThumb;


  RECT       Button1;
  RECT       Button2;
  RECT       Thumb;
};

typedef struct _UI_SCROLLBAR_CLASS {
  UI_CONTROL_CLASS              ParentClass;
  UI_SCROLLBAR_SET_POS          SetScrollPos;
} UI_SCROLLBAR_CLASS;

#endif
