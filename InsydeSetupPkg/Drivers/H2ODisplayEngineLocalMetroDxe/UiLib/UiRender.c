/** @file
  UI Render

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
#include <Protocol/H2OWindow.h>
#include "UiRender.h"
#include "UiControls.h"
#include "UiManager.h"
#include "H2ODisplayEngineLocalMetro.h"
#include "MetroUi.h"

#include <Protocol/SimpleFileSystem.h>
#include <Library/UefiHiiServicesLib.h>

extern EFI_PERFORMANCE_MEASURE_PROTOCOL  *mPerf;

EFI_STATUS
EFIAPI
UiFastFillRect (
  IN HDC        Hdc,
  IN RECT       *Rc,
  IN COLORREF   Color
  )
{

  MwFastFillRect (Hdc, Rc, Color);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UiPaintGradient (
  IN  HDC      Hdc,
  RECT         *Rc,
  COLORREF           Color1,
  COLORREF           Color2,
  BOOLEAN      Vertical,
  UINT32       Steps
  )
{
  UINT32 Shift;
  UINT32 Lines;
  UINT8  R, G, B;
  UINTN  i;

  HBRUSH hBrush;
  HBRUSH hOldBrush;
  RECT   Rc2;


  Shift = 1;
  // Determine how many shades
  if( Steps >= 64 ) Shift = 6;
  else if( Steps >= 32 ) Shift = 5;
  else if( Steps >= 16 ) Shift = 4;
  else if( Steps >= 8 ) Shift = 3;
  else if( Steps >= 4 ) Shift = 2;

  Lines = 1 << Shift;

  hBrush    = GetStockObject (DC_BRUSH);
  hOldBrush = SelectObject (Hdc, hBrush);

  for(i = 0; i < Lines; i++ ) {
     // Do a little alpha blending
     R = (UINT8)((GetRValue (Color1) * (Lines - i) + GetRValue (Color2) * i) >> Shift);
     G = (UINT8)((GetGValue (Color1) * (Lines - i) + GetGValue (Color2) * i) >> Shift);
     B = (UINT8)((GetBValue (Color1) * (Lines - i) + GetBValue (Color2) * i) >> Shift);
     // ... then paint with the resulting color
     SetDCBrushColor (Hdc, RGB (R, G, B));
     Rc2 = *Rc;

     if( Vertical ) {
       Rc2.bottom = Rc->bottom - (((UINT32)i * (Rc->bottom - Rc->top)) >> Shift);
       Rc2.top    = Rc->bottom - (((UINT32)(i + 1) * (Rc->bottom - Rc->top)) >> Shift);
       if((Rc2.bottom - Rc2.top) > 0) {
         FillRect (Hdc, &Rc2, hBrush);
       }
     }
     else {
        Rc2.left  = Rc->right - (((UINT32)(i + 1) * (Rc->right - Rc->left)) >> Shift);
        Rc2.right = Rc->right - (((UINT32)i * (Rc->right - Rc->left)) >> Shift);
        if((Rc2.right - Rc2.left) > 0) {
          FillRect (Hdc, &Rc2, hBrush);
        }
     }
  }

  SelectObject (Hdc, hOldBrush);


  return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
UiPaintText (
  IN  HDC         Hdc,
  RECT            *Rc,
  CHAR16            *String,
  COLORREF          TextColor,
  COLORREF          BackColor,
  UINT32            Style
  )
{
  BOOLEAN bDraw;
  POINT   pt;
  UINT32  cyline;

  UINT8    ColorIndex;
  COLORREF ColorArray[10];
  COLORREF Color;

  UINT8    FontSizeIndex;
  UINT32   FontSizeArray[10];
  UINT32   FontSize;

  ColorIndex = 0;
  FontSizeIndex = 0;

  ASSERT (Hdc != NULL);
  if (!Hdc) return 0;

  if(IsRectEmpty(Rc)) return 0;

  if ((Style & DT_CALCRECT) == 0) {
//  GetClipBox (Hdc, &ClipRc);
//  OldRgn = CreateRectRgnIndirect (&ClipRc);
//  NewRgn = CreateRectRgnIndirect (&TmpRc);
//  ExtSelectClipRgn (Hdc, NewRgn, RGN_AND);
  }

  bDraw = ((Style & DT_CALCRECT) == 0);
  // If the drawstyle includes an alignment, we'll need to first determine the text-size so
  // we can draw it at the correct position...
  if( (Style & DT_SINGLELINE) != 0 && (Style & DT_VCENTER) != 0 && (Style & DT_CALCRECT) == 0 ) {
    RECT rcText = { 0, 0, 9999, 100 };
    UiPaintText (Hdc, &rcText, String, TextColor, BackColor, Style | DT_CALCRECT);
    Rc->top    = Rc->top + ((Rc->bottom - Rc->top) / 2) - ((rcText.bottom - rcText.top) / 2);
    Rc->bottom = Rc->top + (rcText.bottom - rcText.top);
  }
  if( (Style & DT_SINGLELINE) != 0 && (Style & DT_CENTER) != 0 && (Style & DT_CALCRECT) == 0 ) {
    RECT rcText = { 0, 0, 9999, 100 };
    UiPaintText (Hdc, &rcText, String, TextColor, BackColor, Style | DT_CALCRECT);
    OffsetRect (Rc, (Rc->right - Rc->left) / 2 - (rcText.right - rcText.left) / 2, 0);
  }
  if( (Style & DT_SINGLELINE) != 0 && (Style & DT_RIGHT) != 0 && (Style & DT_CALCRECT) == 0 ) {
    RECT rcText = { 0, 0, 9999, 100 };
    UiPaintText (Hdc, &rcText, String, TextColor, BackColor, Style | DT_CALCRECT);
    Rc->left = Rc->right - (rcText.right - rcText.left);
  }

  if( (Style & DT_SINGLELINE) != 0 && (Style & DT_RIGHT) != 0 && (Style & DT_CALCRECT) == 0 ) {
    RECT rcText = { 0, 0, 9999, 100 };
    UiPaintText (Hdc, &rcText, String, TextColor, BackColor, Style | DT_CALCRECT);
    Rc->left = Rc->right - (rcText.right - rcText.left);
  }

  if( (Style & DT_SINGLELINE) != 0 && (Style & DT_BOTTOM) != 0 && (Style & DT_CALCRECT) == 0 ) {
    RECT rcText;
    SetRect (&rcText, 0, 0, Rc->right, 9999);
    UiPaintText (Hdc, &rcText, String, TextColor, BackColor, Style | DT_CALCRECT);
    Rc->top  = Rc->bottom - (rcText.bottom - rcText.top);
  }

  if ((Style & DT_CALCRECT) == 0) {
    SetBkMode(Hdc, TRANSPARENT);
    SetTextColor (Hdc, TextColor);
    if (BackColor != INVALID_COLOR) {
      UiFastFillRect (Hdc, Rc, BackColor);
    }
  }

  pt.x = Rc->left;
  pt.y = Rc->top;
  cyline = Hdc->font->FontSize;

  while (*String != '\0') {

    if (pt.x >= Rc->right || *String == '\n') {
      //
      // \n or word wrap
      //
      if ((Style & DT_SINGLELINE) != 0) break;
      if (*String == '\n') {
        String++;
      }
      pt.x = Rc->left;
      pt.y += cyline;
      if (pt.x >= Rc->right) {
        break;
      }

      cyline = Hdc->font->FontSize;

      while (*String == ' ') String++;

    } else if ((*String == '<') &&
               (*(String + 1) >= 'a' && *(String + 1) <= 'z') &&
               (*(String + 2) == ' ' || *(String + 2) == '>')) {

      String++;

      switch (*String) {

      case 'f':
        String++;
        FontSize = 19;
        while (*String > '\0' && *String <= ' ') String++;
        if (isdigit(*String)) {
          FontSize = wcstol (String, &String, 10);
        }
        while (*String > '\0' && *String <= ' ') String++;

        ASSERT (FontSizeIndex < (sizeof (FontSizeArray) / sizeof (FontSizeArray[0])));
        FontSizeArray[FontSizeIndex++] = Hdc->font->FontSize;

        Hdc->font->FontSize = FontSize;

        cyline = MAX (cyline, FontSize);
        break;

      case 'c':
        String++;
        while (*String > '\0' && *String <= ' ') String++;
        Color = wcstoul (String, &String, 16);

        ASSERT (ColorIndex < (sizeof (ColorArray) / sizeof (ColorArray[0])));
        ColorArray[ColorIndex++] = GetTextColor (Hdc);
        SetTextColor (Hdc, Color);
        break;
      }

      while (*String != '\0' && *String != '>') {
        String++;
      }
      if (*String == '>') {
        String++;
      }
    } else if((*String == '<') && (*(String + 1) == '/')) {

      String++;
      String++;
      switch(*String) {

      case 'f':
        ASSERT (FontSizeIndex > 0);
        if (FontSizeIndex == 0) {
          return EFI_INVALID_PARAMETER;
        }
        FontSize = FontSizeArray[--FontSizeIndex];
        Hdc->font->FontSize = FontSize;
        cyline = MAX (cyline, Hdc->font->FontSize);
        break;

      case 'c':
        ASSERT (ColorIndex > 0);
        if (ColorIndex == 0) {
          return EFI_INVALID_PARAMETER;
        }
        SetTextColor (Hdc, ColorArray[--ColorIndex]);
        break;

      }

      while (*String != '\0' && *String != '>') {
        String++;
      }
      if (*String == '>') {
        String++;
      }
      /*
    } else if (*String == ' ') {

      GetTextExtentPoint32 (Hdc, String, 1, &Size);
      if (bDraw) {
        TextOut (Hdc, pt.x, pt.y, L" ", 1);
      }
      pt.x += Size.cx;
      String++;
      */
    } else {

      POINT   ptPos = pt;
      int     cchChars = 0;
      int     cchLastGoodWord = 0;
      CHAR16  *p = (CHAR16 *)String;
      SIZE    szText = { 0 };
      SIZE    Size;
      INT32   CharMaxWidth;
      INT32   LineWidth;


      if(*p == '<') {
        p++, cchChars++;
      }

      CharMaxWidth = cyline;
      while (*p != '\0' && *p != '\n' && *p != '<') {
        cchChars++;

        szText.cx = cchChars * CharMaxWidth;
        if (pt.x + szText.cx >= Rc->right) {
          GetTextExtentPoint32 (Hdc, String, cchChars, &Size);
          szText.cx = Size.cx;
        }

        if(pt.x + szText.cx >= Rc->right ) {
           if( (Style & DT_WORDBREAK) != 0) {
              if (cchLastGoodWord > 0 ) {
                cchChars = cchLastGoodWord;
                pt.x = Rc->right;
              } else {
                cchChars--;
                p--;
                pt.x = Rc->right;
              }
           }
           break;
        }
        if( *p == ' ' ) cchLastGoodWord = cchChars;
          p++;
      }

      if (cchChars > 0) {
        GetTextExtentPoint32 (Hdc, String, cchChars, &Size);
        if (bDraw) {
          if (((Style & DT_CENTER) != 0) && ((Style & DT_SINGLELINE) == 0)) {
            LineWidth = Rc->right - Rc->left;
            TextOut (Hdc, ptPos.x + (LineWidth - Size.cx) / 2, ptPos.y, String, cchChars);
          } else {
            TextOut (Hdc, ptPos.x, ptPos.y, String, cchChars);
          }
        }
        pt.x += Size.cx;
        String += cchChars;
      }
    }
  }

  if((Style & DT_CALCRECT) != 0) {
    Rc->bottom = pt.y + cyline;
    if (Rc->right >= 9999) {
      if (StrLen ((CHAR16 *)String) > 0) pt.x += 3;
      Rc->right = pt.x;
    }
    Rc->right = pt.x;
  }

  if((Style & DT_CALCRECT) == 0) {
//  SelectClipRgn(Hdc, OldRgn);
//  DeleteObject(OldRgn);
//  DeleteObject(NewRgn);
  }

  return (pt.y == 0) ? pt.x : Rc->right;

}
/*
EFI_STATUS
LoadDataFromFile (
  CHAR16  *FileName,
  UINT8   **Data,
  UINTN   *Length
  )
{

  EFI_STATUS                       Status;
  EFI_HANDLE                       *HandleBuffer;
  UINTN                            NumberOfHandles;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *SimpleFile;
  UINTN                            Index;
  EFI_FILE                         *Root, *File;
  UINT64                           Pos;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );

  *Data = NULL;

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID **) &SimpleFile
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = SimpleFile->OpenVolume (SimpleFile, &Root);

    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Root->Open (Root, &File, FileName, EFI_FILE_MODE_READ, 0);

    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = File->SetPosition (File, (UINT64)-1);
    if (EFI_ERROR (Status)) {
      continue;
    }

    File->GetPosition (File, &Pos);
    if (EFI_ERROR (Status)) {
      continue;
    }

    File->SetPosition (File, 0);
    if (EFI_ERROR (Status)) {
      continue;
    }

    *Length = (UINTN) Pos;

    *Data = AllocatePool (*Length);

    File->Read (File, Length, *Data);
    File->Close (File);
    Root->Close (Root);
  }

  return EFI_SUCCESS;
}*/

VOID
UiDrawImage (
  IN PSD         psd,
  IN RECT        *Rc,
  IN RECT        *Scale9Grid,
  IN PSD         pmd
  )
/*++

Routine Description:
  Scale image by Scale9Grid algorithm

Arguments:
  This       - Ui render
  Hwnd       - window handle
  Hdc        - window device content
  Rc         - window rect
  Scale9Grid - scale 9 grid
  Image      - image
  ImageDC    - image dc

Returns:
  None

--*/
{

  RECT DstHoldRc;
  RECT SrcHoldRc;

  DstHoldRc.left    = Rc->left   + Scale9Grid->left;
  DstHoldRc.top     = Rc->top    + Scale9Grid->top;
  DstHoldRc.right   = Rc->right  - Scale9Grid->right;
  DstHoldRc.bottom  = Rc->bottom - Scale9Grid->bottom;

  SrcHoldRc.left    = Scale9Grid->left;
  SrcHoldRc.top     = Scale9Grid->top;
  SrcHoldRc.right   = pmd->xvirtres - Scale9Grid->right;
  SrcHoldRc.bottom  = pmd->yvirtres - Scale9Grid->bottom;

  //
  // hold
  //
  if (SrcHoldRc.right != SrcHoldRc.left && SrcHoldRc.bottom != SrcHoldRc.top) {
    GdDrawImagePartToFit (
      psd,
      DstHoldRc.left, DstHoldRc.top, DstHoldRc.right - DstHoldRc.left, DstHoldRc.bottom - DstHoldRc.top,
      SrcHoldRc.left, SrcHoldRc.top, SrcHoldRc.right - SrcHoldRc.left, SrcHoldRc.bottom - SrcHoldRc.top,
      pmd
      );
  }

  //
  // left - top
  //
  if (Scale9Grid->left != 0 && Scale9Grid->top != 0) {
    GdDrawImagePartToFit (
      psd,
      Rc->left, Rc->top, Scale9Grid->left, Scale9Grid->top,
      0, 0, Scale9Grid->left, Scale9Grid->top,
      pmd
      );
  }

  //
  // right - top
  //
  if (Scale9Grid->right != 0 && Scale9Grid->top != 0) {
    GdDrawImagePartToFit (
      psd,
      DstHoldRc.right, Rc->top, Scale9Grid->right, Scale9Grid->top,
      SrcHoldRc.right,       0, Scale9Grid->right, Scale9Grid->top,
      pmd
      );
  }

  //
  // left - bottom
  //
  if (Scale9Grid->left != 0 && Scale9Grid->bottom != 0) {
    GdDrawImagePartToFit (
      psd,
      Rc->left, DstHoldRc.bottom, Scale9Grid->left, Scale9Grid->bottom,
      0, SrcHoldRc.bottom, Scale9Grid->left, Scale9Grid->bottom,
      pmd
      );
  }

  //
  // right - bottom
  //
  if (Scale9Grid->right != 0 && Scale9Grid->bottom != 0) {
    GdDrawImagePartToFit (
      psd,
      DstHoldRc.right, DstHoldRc.bottom, Scale9Grid->right, Scale9Grid->bottom,
      SrcHoldRc.right, SrcHoldRc.bottom, Scale9Grid->right, Scale9Grid->bottom,
      pmd
      );
  }

  //
  // top
  //
  if (SrcHoldRc.right != SrcHoldRc.left && Scale9Grid->top != 0) {
    GdDrawImagePartToFit (
      psd,
      DstHoldRc.left, Rc->top, DstHoldRc.right - DstHoldRc.left, Scale9Grid->top,
      SrcHoldRc.left,       0, SrcHoldRc.right - SrcHoldRc.left, Scale9Grid->top,
      pmd
      );
  }

  //
  // left
  //
  if (SrcHoldRc.left != 0 && SrcHoldRc.bottom != SrcHoldRc.top) {
    GdDrawImagePartToFit (
      psd,
      Rc->left, DstHoldRc.top, Scale9Grid->left, DstHoldRc.bottom - DstHoldRc.top,
      0, SrcHoldRc.top, Scale9Grid->left, SrcHoldRc.bottom - SrcHoldRc.top,
      pmd
      );
  }

  //
  // right
  //
  if (Scale9Grid->right != 0 && SrcHoldRc.bottom != SrcHoldRc.top) {
    GdDrawImagePartToFit (
      psd,
      DstHoldRc.right, DstHoldRc.top, Scale9Grid->right, DstHoldRc.bottom - DstHoldRc.top,
      SrcHoldRc.right, SrcHoldRc.top, Scale9Grid->right, SrcHoldRc.bottom - SrcHoldRc.top,
      pmd
      );
  }

  //
  // bottom
  //
  if (SrcHoldRc.right != SrcHoldRc.left && Scale9Grid->bottom != 0) {
    GdDrawImagePartToFit (
      psd,
      DstHoldRc.left, DstHoldRc.bottom, DstHoldRc.right - DstHoldRc.left, Scale9Grid->bottom,
      SrcHoldRc.left, SrcHoldRc.bottom, SrcHoldRc.right - SrcHoldRc.left, Scale9Grid->bottom,
      pmd
      );
  }

}

EFI_IMAGE_INPUT *
CreateHsvAdjustImage (
  IN CONST  EFI_IMAGE_INPUT     *ImageIn,
  IN CONST  HSV_DIFF_VALUE      *HsvDiff
  )
{
  EFI_IMAGE_INPUT               *Image;
  UINTN                         BufferLen;
  INTN                          Index;
  HSV_VALUE                     Hsv;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BitmapIn;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BitmapOut;



  ASSERT (ImageIn != NULL && HsvDiff != NULL);
  if (ImageIn == NULL || HsvDiff == NULL) {
    return NULL;
  }
  if (HsvDiff->HueDiff == 0 && HsvDiff->SaturationDiff == 0 && HsvDiff->ValueDiff == 0) {
    return NULL;
  }
  Image = AllocateCopyPool (sizeof (EFI_IMAGE_INPUT), ImageIn);
  ASSERT (Image != NULL);
  if (Image == NULL) {
    return NULL;
  }

  BufferLen = ImageIn->Width * ImageIn->Height;
  Image->Bitmap = AllocatePool (BufferLen * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  ASSERT (Image->Bitmap != NULL);
  if (Image->Bitmap == NULL) {
    FreePool (Image);
    return NULL;
  }

  Index = BufferLen;
  BitmapIn  = ImageIn->Bitmap;
  BitmapOut = Image->Bitmap;

  while (--Index >= 0) {
    if (BitmapIn->Reserved != 0) {
      RGB2HSV (BitmapIn, &Hsv);
      if (Hsv.Hue + HsvDiff->HueDiff < 0) {
        Hsv.Hue = (UINT16) Hsv.Hue + HsvDiff->HueDiff + 360;
      } else {
        Hsv.Hue = (UINT16) (Hsv.Hue + HsvDiff->HueDiff);
        Hsv.Hue = Hsv.Hue > 360 ? Hsv.Hue - 360 : Hsv.Hue;
      }
      Hsv.Saturation = (UINT8)  (Hsv.Saturation + HsvDiff->SaturationDiff);
      Hsv.Saturation = Hsv.Saturation > 100 ? 100 : Hsv.Saturation;
      Hsv.Value      = (UINT8)  (Hsv.Value + HsvDiff->ValueDiff);
      Hsv.Value      = Hsv.Value > 100 ? 100 : Hsv.Value;
      HSV2RGB (&Hsv, BitmapOut);
      BitmapOut->Reserved = BitmapIn->Reserved;
    } else {
      *(UINT32 *)BitmapOut = 0;
    }
    ++BitmapIn;
    ++BitmapOut;
  }

  return Image;
}

EFI_IMAGE_INPUT *
CreateStyleAdjustImage (
  IN CONST  EFI_IMAGE_INPUT       *ImageIn,
  IN CONST  UINT32                Style
  )
{
  EFI_IMAGE_INPUT               *Image;
  UINTN                         BufferLen;
  INTN                          Index;
  UINT32                        *BitmapIn;
  UINT32                        *BitmapOut;

  ASSERT (ImageIn != NULL);
  ASSERT ((Style & (DT_GRAY | DT_LIGHT)) != (DT_GRAY | DT_LIGHT));
  if ((Style & (DT_GRAY | DT_LIGHT)) == 0) {
    return NULL;
  }

  Image = AllocateCopyPool (sizeof (EFI_IMAGE_INPUT), ImageIn);
  ASSERT (Image != NULL);
  if (Image == NULL) {
    return NULL;
  }
  BufferLen = ImageIn->Width * ImageIn->Height;
  Image->Bitmap = AllocatePool (BufferLen * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  ASSERT (Image->Bitmap != NULL);
  if (Image->Bitmap == NULL) {
    FreePool (Image);
    return NULL;
  }


  Index = BufferLen;
  BitmapIn  = (UINT32 *)ImageIn->Bitmap;
  BitmapOut = (UINT32 *)Image->Bitmap;


  if (Style & DT_GRAY) {
    while (--Index >= 0) {
      *(BitmapOut++) = 0x00D6D6D6 + (*(BitmapIn++) & 0xFF000000);
    }
  } else {
    while (--Index >= 0) {
      *(BitmapOut++) = 0x00FFFFFF + (*(BitmapIn++) & 0xFF000000);
    }
  }

  return Image;
}

EFI_STATUS
EFIAPI
UiShowBitmap (
  HWND                          hwnd,
  HDC                           hdc,
  RECT                          *Rc,
  RECT                          *Scale9Grid,
  EFI_IMAGE_INPUT               *ImageIn,
  UINT32                        Style,
  HSV_DIFF_VALUE                *HsvDiff
  )
{
  INT32                         Width;
  INT32                         Height;
  PSD                           pmd;
  MWCOORD                       ImageOffsetX;
  MWCOORD                       ImageOffsetY;
  INT32                         DstWidth;
  INT32                         DstHeight;
  EFI_IMAGE_INPUT               *TempImageIn;

  ASSERT (ImageIn != NULL);
  if (ImageIn == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TempImageIn = NULL;
  if (HsvDiff != NULL && (HsvDiff->HueDiff != 0 || HsvDiff->SaturationDiff != 0 || HsvDiff->ValueDiff != 0)) {
    TempImageIn = CreateHsvAdjustImage (ImageIn, HsvDiff);
    ASSERT (TempImageIn != NULL);
    pmd = GdLoadImageFromBuffer (TempImageIn);
  } else if (Style & (DT_GRAY | DT_LIGHT)) {
    TempImageIn = CreateStyleAdjustImage (ImageIn, Style);
    ASSERT (TempImageIn != NULL);
    pmd = GdLoadImageFromBuffer (TempImageIn);
  } else {
    pmd = GdLoadImageFromBuffer (ImageIn);
  }
  if (pmd == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Width  = MIN ((Rc->right - Rc->left), pmd->xvirtres);
  Height = MIN ((Rc->bottom - Rc->top), pmd->yvirtres);
  DstWidth  = Width;
  DstHeight = Height;

  //
  // center image
  //
  ImageOffsetX = 0;
  ImageOffsetY = 0;

  if (Style & DT_STRETCH) {
    ASSERT ((Style & (DT_CENTER | DT_VCENTER)) == 0);
    ASSERT (Scale9Grid->left == 0 && Scale9Grid->right == 0 && Scale9Grid->top == 0 && Scale9Grid->bottom == 0);

    Width        = pmd->xvirtres;
    Height       = pmd->yvirtres;
    DstWidth     = Rc->right - Rc->left;
    DstHeight    = Rc->bottom - Rc->top;
  } else {

    if (Style & DT_CENTER) {
      if ((Rc->right - Rc->left) > pmd->xvirtres) {
        Rc->left = (Rc->right + Rc->left - pmd->xvirtres)  / 2;
      }
      if (Width <= pmd->xvirtres) {
        ImageOffsetX = (pmd->xvirtres - Width) / 2;
      }
    } else if (Style & DT_VCENTER) {
      if ((Rc->bottom - Rc->top) > pmd->yvirtres) {
        Rc->top = (Rc->bottom  + Rc->top - pmd->yvirtres) / 2;
      }
      if (Height < pmd->yvirtres) {
        ImageOffsetY = (pmd->yvirtres - Height) / 2;
      }
    }
  }

  if (Scale9Grid->left == 0 && Scale9Grid->right == 0 && Scale9Grid->top == 0 && Scale9Grid->bottom == 0) {
    GdDrawImagePartToFit (hdc->psd, Rc->left, Rc->top, DstWidth, DstHeight, ImageOffsetX, ImageOffsetY, Width, Height, pmd);
  } else {
    UiDrawImage (hdc->psd, Rc, Scale9Grid, pmd);
  }

  pmd->FreeMemGC(pmd);
  if (TempImageIn != NULL) {
    FreePool (TempImageIn->Bitmap);
    FreePool (TempImageIn);
  }

  return EFI_SUCCESS;
}


