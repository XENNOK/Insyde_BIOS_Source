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

#include "UiManager.h"
#include <Library/BitBltLib.h>

#define INVALID_COLOR (0x00000000)

EFI_STATUS
UiFastFillRect (
  IN HDC        Hdc,
  IN RECT       *Rc,
  IN COLORREF   Color
  );

EFI_STATUS
UiPaintGradient (
  IN  HDC      Hdc,
  RECT         *Rc,
  COLORREF           Color1,
  COLORREF           Color2,
  BOOLEAN      Vertical,
  UINT32       Steps
  );

EFI_STATUS
UiPaintText (
  IN  HDC         Hdc,
  RECT            *Rc,
  CHAR16            *String,
  COLORREF          TextColor,
  COLORREF          BackColor,
  UINT32            Style
  );

VOID
UiShowBitmap(
  HWND    hwnd,
  HDC     hdc,
  RECT    *Rc,
  RECT    *Scale9Grid,
  UINT32  ImageId,
  UINT32  Style
  );

#endif

