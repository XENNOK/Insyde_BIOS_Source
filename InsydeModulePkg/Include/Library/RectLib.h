/** @file
 Structure and function definition for rectangular library

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _RECT_LIB_H_
#define _RECT_LIB_H_

#include <Uefi.h>

typedef struct _RECT {
  INT32                                        left;
  INT32                                        top;
  INT32                                        right;
  INT32                                        bottom;
} RECT;

typedef struct {
  INT32                                        x;
  INT32                                        y;
} POINT;

BOOLEAN
EFIAPI
SetRect(
  IN OUT RECT                                  *Rect,
  IN     INT32                                 Left,
  IN     INT32                                 Top,
  IN     INT32                                 Right,
  IN     INT32                                 Bottom
  );

BOOLEAN
EFIAPI
SetRectEmpty (
  IN RECT                                      *Rect
  );

BOOLEAN
EFIAPI
CopyRect (
  OUT      RECT                                *DestRect,
  IN CONST RECT                                *SrcRect
  );

BOOLEAN
EFIAPI
IsRectEmpty (
  IN CONST RECT                                *Rect
  );

BOOLEAN
EFIAPI
InflateRect (
  IN OUT RECT                                  *Rect,
  IN INT32                                     DiffX,
  IN INT32                                     DiffY
  );

BOOLEAN
EFIAPI
OffsetRect(
  IN OUT RECT                                  *Rect,
  IN     INT32                                 DiffX,
  IN     INT32                                 DiffY
  );

BOOLEAN
EFIAPI
PtInRect (
  IN RECT                                      *Rect,
  IN POINT                                     Pt
  );

BOOLEAN
EFIAPI
IntersectRect (
  IN OUT   RECT                                *DestRect,
  IN CONST RECT                                *SrcRect1,
  IN CONST RECT                                *SrcRect2
  );

BOOLEAN
EFIAPI
UnionRect (
  IN OUT   RECT                                *DestRect,
  IN CONST RECT                                *SrcRect1,
  IN CONST RECT                                *SrcRect2
  );

BOOLEAN
EFIAPI
SubtractRect(
  OUT      RECT                                *DestRect,
  IN CONST RECT                                *Src1Rect,
  IN CONST RECT                                *Src2Rect
  );

BOOLEAN
EFIAPI
EqualRect (
  IN CONST RECT                                *Rect1,
  IN CONST RECT                                *Rect2
  );

#endif

