/** @file
  Instance of rectangular library

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

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/RectLib.h>

/**
 Set rectangle data

 @param[in, out] Rect            Pointer to rectangle data which will be set
 @param[in]      Left            Left value of rectangle data
 @param[in]      Top             Top value of rectangle data
 @param[in]      Right           Right value of rectangle data
 @param[in]      Bottom          Bottom value of rectangle data

 @retval TRUE                    Rectangle data is set
**/
BOOLEAN
EFIAPI
SetRect(
  IN OUT RECT                                  *Rect,
  IN     INT32                                 Left,
  IN     INT32                                 Top,
  IN     INT32                                 Right,
  IN     INT32                                 Bottom
  )
{
  Rect->left   = Left;
  Rect->top    = Top;
  Rect->right  = Right;
  Rect->bottom = Bottom;

  return TRUE;
}

/**
 Set rectangle to empty

 @param[in] Rect                 Rectangle

 @retval TRUE                    Point is empty
**/
BOOLEAN
EFIAPI
SetRectEmpty (
  IN RECT                                      *Rect
  )
{
  ZeroMem (Rect, sizeof (RECT));

  return TRUE;
}

/**
 Copy rectangle data

 @param[out] DestRect            Pointer to destination rectangle data
 @param[in]  SrcRect             Pointer to source rectangle data

 @retval TRUE                    Rectangle data is copied
**/
BOOLEAN
EFIAPI
CopyRect (
  OUT      RECT                                *DestRect,
  IN CONST RECT                                *SrcRect
  )
{
  CopyMem (DestRect, SrcRect, sizeof (RECT));

  return TRUE;
}

/**
 Test rectangle is empty

 @param[in] Rect                Pointer to rectangle data

 @retval TRUE                   Rectangle is empty
 @retval FALSE                  Rectangle isn't empty
**/
BOOLEAN
EFIAPI
IsRectEmpty (
  IN CONST RECT                                *Rect
  )
{
  return (BOOLEAN) (Rect->right == Rect->left || Rect->bottom == Rect->top);
}

/**
 Inflate the rectangle data

 @param[in, out] Rect            Pointer to rectangle data which will be inflated
 @param[in]      DiffX           Inflated x-axis value
 @param[in]      DiffY           Inflated y-axis value

 @retval TRUE                    Rectangle data is inflated
**/
BOOLEAN
EFIAPI
InflateRect (
  IN OUT RECT                                  *Rect,
  IN INT32                                     DiffX,
  IN INT32                                     DiffY
  )
{
  Rect->left   -= DiffX;
  Rect->top    -= DiffY;
  Rect->right  += DiffX;
  Rect->bottom += DiffY;

  return TRUE;
}

/**
 Offset the rectangle data

 @param[in, out] Rect            Pointer to rectangle data which will be offset
 @param[in]      DiffX           Offset x-axis value
 @param[in]      DiffY           Offset y-axis value

 @retval TRUE                    Rectangle data is offset
**/
BOOLEAN
EFIAPI
OffsetRect(
  IN OUT RECT                                  *Rect,
  IN     INT32                                 DiffX,
  IN     INT32                                 DiffY
  )
{
  Rect->left   += DiffX;
  Rect->right  += DiffX;
  Rect->top    += DiffY;
  Rect->bottom += DiffY;

  return TRUE;
}

/**
 Point is in rectangle range

 @param[in] Rect                 Test rectangle
 @param[in] Pt                   Test point

 @retval TRUE                    Point is in rectangle range
 @retval FALS                    Point is not in rectangle range
**/
BOOLEAN
EFIAPI
PtInRect (
  IN RECT                                      *Rect,
  IN POINT                                     Pt
  )
{
  return (BOOLEAN) (Pt.x >= Rect->left && Pt.x < Rect->right &&
                    Pt.y >= Rect->top  && Pt.y < Rect->bottom);
}

/**
 Get intersetion region from two rectangle data

 @param[out] DestRect            Pointer to rectangle data which is intersetion of SrcRect1 and SrcRect2
 @param[in]  SrcRect1            Pointer to rectangle data one
 @param[in]  SrcRect2            Pointer to rectangle data two

 @retval TRUE                    Get intersetional rectangle data sucessfully
 @retval FALSE                   Get intersetional rectangle data fail
**/
BOOLEAN
EFIAPI
IntersectRect (
  IN OUT   RECT                                *DestRect,
  IN CONST RECT                                *SrcRect1,
  IN CONST RECT                                *SrcRect2
  )
{
  if (IsRectEmpty(SrcRect1) || IsRectEmpty(SrcRect2) ||
      (SrcRect1->left >= SrcRect2->right) || (SrcRect2->left >= SrcRect1->right) ||
      (SrcRect1->top >= SrcRect2->bottom) || (SrcRect2->top >= SrcRect1->bottom)) {
    SetRectEmpty(DestRect);
    return FALSE;
  }

  DestRect->left   = MAX(SrcRect1->left, SrcRect2->left);
  DestRect->right  = MIN(SrcRect1->right, SrcRect2->right);
  DestRect->top    = MAX(SrcRect1->top, SrcRect2->top);
  DestRect->bottom = MIN(SrcRect1->bottom, SrcRect2->bottom);

  return TRUE;
}

/**
 Get union region from two rectangle data

 @param[out] DestRect            Pointer to rectangle data which is union of SrcRect1 and SrcRect2
 @param[in]  SrcRect1            Pointer to rectangle data one
 @param[in]  SrcRect2            Pointer to rectangle data two

 @retval TRUE                    Get union rectangle data sucessfully
 @retval FALSE                   Get union rectangle data fail
**/
BOOLEAN
EFIAPI
UnionRect (
  IN OUT   RECT                                *DestRect,
  IN CONST RECT                                *SrcRect1,
  IN CONST RECT                                *SrcRect2
  )
{
  if (IsRectEmpty(SrcRect1)) {
    if (IsRectEmpty(SrcRect2)) {
      SetRectEmpty(DestRect);
      return FALSE;
    } else {
      *DestRect = *SrcRect2;
    }
  } else {
    if (IsRectEmpty(SrcRect2)) {
      *DestRect = *SrcRect1;
    } else {
      DestRect->left   = MIN( SrcRect1->left, SrcRect2->left );
      DestRect->right  = MAX( SrcRect1->right, SrcRect2->right );
      DestRect->top    = MIN( SrcRect1->top, SrcRect2->top );
      DestRect->bottom = MAX( SrcRect1->bottom, SrcRect2->bottom );
    }
  }
  return TRUE;
}

/**
 Subtract Src2Rect region from Src1Rect region

 @param[out] DestRect            Pointer to destination rectangle data
 @param[in]  Src1Rect            Pointer to source rectangle data one
 @param[in]  Src2Rect            Pointer to source rectangle data two

 @retval TRUE                    Rectangle data is subtracted
 @retval FALSE                   Source rectangle data one is empty
**/
BOOLEAN
EFIAPI
SubtractRect(
  OUT      RECT                                *DestRect,
  IN CONST RECT                                *Src1Rect,
  IN CONST RECT                                *Src2Rect
  )
{
  RECT                                         Tmp;

  if (IsRectEmpty (Src1Rect)) {
    SetRectEmpty (DestRect);
    return FALSE;
  }

  CopyRect (DestRect, Src1Rect);

  if (IntersectRect (&Tmp, Src1Rect, Src2Rect)) {
    if (EqualRect (&Tmp, DestRect)) {
      SetRectEmpty( DestRect );
      return FALSE;
    }

    if ((Tmp.top == DestRect->top) && (Tmp.bottom == DestRect->bottom)) {
      if (Tmp.left == DestRect->left) {
        DestRect->left = Tmp.right;
      } else if (Tmp.right == DestRect->right) {
        DestRect->right = Tmp.left;
      }
    } else if ((Tmp.left == DestRect->left) && (Tmp.right == DestRect->right)) {
      if (Tmp.top == DestRect->top) {
         DestRect->top = Tmp.bottom;
      } else if (Tmp.bottom == DestRect->bottom) {
        DestRect->bottom = Tmp.top;
      }
    }
  }

  return TRUE;
}

/**
 Copy rectangle data

 @param[in] Rect1                Pointer to rectangle data one
 @param[in] Rect2                Pointer to rectangle data two

 @retval TRUE                    Rectangle data is equal
 @retval FALSE                   Rectangle data is not equal
**/
BOOLEAN
EFIAPI
EqualRect (
  IN CONST RECT                                *Rect1,
  IN CONST RECT                                *Rect2
  )
{
  return ((Rect1->left == Rect2->left) && (Rect1->right == Rect2->right) &&
          (Rect1->top == Rect2->top) && (Rect1->bottom == Rect2->bottom));
}

