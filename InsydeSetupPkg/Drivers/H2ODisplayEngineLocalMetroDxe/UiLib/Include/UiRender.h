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

#ifndef _UI_RENDER_ENGINE_H
#define _UI_RENDER_ENGINE_H
#include <Protocol/H2OWindow.h>

#define INVALID_COLOR (0x00000000)

typedef struct _HSV_VALUE {
  UINT16       Hue;
  UINT8        Saturation;
  UINT8        Value;
} HSV_VALUE;

typedef struct _HSV_DIFF_VALUE {
  INT16       HueDiff;
  INT8        SaturationDiff;
  INT8        ValueDiff;
} HSV_DIFF_VALUE;

EFI_STATUS
EFIAPI
UiFastFillRect (
  IN HDC        Hdc,
  IN RECT       *Rc,
  IN COLORREF   Color
  );

EFI_STATUS
EFIAPI
UiPaintGradient (
  IN  HDC      Hdc,
  RECT         *Rc,
  COLORREF           Color1,
  COLORREF           Color2,
  BOOLEAN      Vertical,
  UINT32       Steps
  );

EFI_STATUS
EFIAPI
UiPaintText (
  IN  HDC         Hdc,
  RECT            *Rc,
  CHAR16            *String,
  COLORREF          TextColor,
  COLORREF          BackColor,
  UINT32            Style
  );

EFI_STATUS
EFIAPI
UiShowBitmap(
  HWND               hwnd,
  HDC                hdc,
  RECT               *Rc,
  RECT               *Scale9Grid,
  EFI_IMAGE_INPUT    *ImageIn,
  UINT32             Style,
  HSV_DIFF_VALUE     *HsvDiff
  );

#endif

