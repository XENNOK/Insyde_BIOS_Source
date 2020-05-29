//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   Print.c
//;
#include "Tiano.h"
#include "EfiDriverLib.h"
#include "TianoCommon.h"
#include "EfiCommonLib.h"
#include "PrintWidth.h"
#include "EfiPrintLib.h"
#include "Print.h"

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
#include EFI_PROTOCOL_DEFINITION (HiiFont)
#else
#include EFI_PROTOCOL_DEFINITION (Hii)
#endif

STATIC
CHAR_W                *
GetFlagsAndWidth (
  IN  CHAR_W      *Format,
  OUT UINTN       *Flags,
  OUT UINTN       *Width,
  IN OUT  VA_LIST *Marker
  );

STATIC
UINTN
GuidToString (
  IN  EFI_GUID  *Guid,
  IN OUT CHAR_W *Buffer,
  IN  UINTN     BufferSize
  );

STATIC
UINTN
TimeToString (
  IN  EFI_TIME  *Time,
  IN OUT CHAR_W *Buffer,
  IN  UINTN     BufferSize
  );

STATIC
UINTN
EfiStatusToString (
  IN EFI_STATUS   Status,
  OUT CHAR_W      *Buffer,
  IN  UINTN       BufferSize
  );

STATIC
UINTN
Atoi (
  CHAR_W  *String
  );


static EFI_GRAPHICS_OUTPUT_BLT_PIXEL  mEfiColors[16] = {
  0x00, 0x00, 0x00, 0x00,
  0x98, 0x00, 0x00, 0x00,
  0x00, 0x98, 0x00, 0x00,
  0x98, 0x98, 0x00, 0x00,
  0x00, 0x00, 0x98, 0x00,
  0x98, 0x00, 0x98, 0x00,
  0x00, 0x98, 0x98, 0x00,
  0x98, 0x98, 0x98, 0x00,
  0x10, 0x10, 0x10, 0x00,
  0xff, 0x10, 0x10, 0x00,
  0x10, 0xff, 0x10, 0x00,
  0xff, 0xff, 0x10, 0x00,
  0x10, 0x10, 0xff, 0x00,
  0xf0, 0x10, 0xff, 0x00,
  0x10, 0xff, 0xff, 0x00,
  0xff, 0xff, 0xff, 0x00,
};

UINTN
_IPrint (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL     *GraphicsOutput,
  IN EFI_UGA_DRAW_PROTOCOL            *UgaDraw,
  IN EFI_SIMPLE_TEXT_OUT_PROTOCOL     *Sto,
  IN UINTN                            X,
  IN UINTN                            Y,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *Foreground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *Background,
  IN CHAR16                           *fmt,
  IN VA_LIST                          args
  )
/*++

Routine Description:

  Display string worker for: Print, PrintAt, IPrint, IPrintAt

Arguments:

  GraphicsOutput  - Graphics output protocol interface

  UgaDraw         - UGA draw protocol interface

  Sto             - Simple text out protocol interface

  X               - X coordinate to start printing

  Y               - Y coordinate to start printing

  Foreground      - Foreground color

  Background      - Background color

  fmt             - Format string

  args            - Print arguments

Returns:

  EFI_SUCCESS             -  success
  EFI_OUT_OF_RESOURCES    -  out of resources

--*/
{
  VOID                           *Buffer;
  EFI_STATUS                     Status;
  UINTN                          Index;
  CHAR16                         *UnicodeWeight;
#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
  EFI_HII_FONT_PROTOCOL          *HiiFont;
  EFI_IMAGE_OUTPUT               *Blt;
  EFI_FONT_DISPLAY_INFO          *FontInfo;
#else
  EFI_HII_PROTOCOL               *Hii;
  UINT16                         GlyphWidth;
  UINT32                         GlyphStatus;
  UINT16                         StringIndex;
  EFI_NARROW_GLYPH               *Glyph;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *LineBuffer;
  UINTN                          ImageCharLen;
  UINTN                          ImageCharIndex;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  ForeColor;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  BackColor;
  CHAR16                         Unicode[2];
  BOOLEAN                        WideCharFlag;
  EFI_NARROW_GLYPH               WideGlyph;
#endif
  UINT32                         HorizontalResolution;
  UINT32                         VerticalResolution;
  UINT32                         ColorDepth;
  UINT32                         RefreshRate;
  UINTN                          BufferLen;
  UINTN                          LineBufferLen;

  //
  // For now, allocate an arbitrarily long buffer
  //
  Buffer = EfiLibAllocateZeroPool (0x10000);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (GraphicsOutput != NULL) {
    HorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
    VerticalResolution = GraphicsOutput->Mode->Info->VerticalResolution;
  } else {
    UgaDraw->GetMode (UgaDraw, &HorizontalResolution, &VerticalResolution, &ColorDepth, &RefreshRate);
  }
  ASSERT ((HorizontalResolution != 0) && (VerticalResolution !=0));

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
  Blt      = NULL;
  FontInfo = NULL;
  Status = gBS->LocateProtocol (&gEfiHiiFontProtocolGuid, NULL, (VOID **) &HiiFont);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
#else
  LineBufferLen = sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * HorizontalResolution * GLYPH_HEIGHT;
  LineBuffer = EfiLibAllocatePool (LineBufferLen);
  if (LineBuffer == NULL) {
    gBS->FreePool (Buffer);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gBS->LocateProtocol (&gEfiHiiProtocolGuid, NULL, &Hii);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  Status = gBS->LocateProtocol (&gEfiHiiProtocolGuid, NULL, &Hii);
  if (EFI_ERROR (Status)) {
    goto Error;
  }
#endif

  VSPrint (Buffer, 0x10000, fmt, args);

#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
  UnicodeWeight = (CHAR16 *) Buffer;

  for (Index = 0; UnicodeWeight[Index] != 0; Index++) {
    if (UnicodeWeight[Index] == CHAR_BACKSPACE ||
        UnicodeWeight[Index] == CHAR_LINEFEED  ||
        UnicodeWeight[Index] == CHAR_CARRIAGE_RETURN) {
      UnicodeWeight[Index] = 0;
    }
  }

  BufferLen = EfiStrLen (Buffer);

  LineBufferLen = sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * HorizontalResolution * EFI_GLYPH_HEIGHT;
  if (EFI_GLYPH_WIDTH * EFI_GLYPH_HEIGHT * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * BufferLen > LineBufferLen) {
     Status = EFI_INVALID_PARAMETER;
     goto Error;
  }

  Blt = (EFI_IMAGE_OUTPUT *) EfiLibAllocateZeroPool (sizeof (EFI_IMAGE_OUTPUT));
  if (Blt == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  Blt->Width        = (UINT16) (HorizontalResolution);
  Blt->Height       = (UINT16) (VerticalResolution);
  Blt->Image.Screen = GraphicsOutput;

  FontInfo = (EFI_FONT_DISPLAY_INFO *) EfiLibAllocateZeroPool (sizeof (EFI_FONT_DISPLAY_INFO));
  if (FontInfo == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }
  if (Foreground != NULL) {
    EfiCopyMem (&FontInfo->ForegroundColor, Foreground, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  } else {
    EfiCopyMem (
      &FontInfo->ForegroundColor,
      &mEfiColors[Sto->Mode->Attribute & 0x0f],
      sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
      );
  }
  if (Background != NULL) {
    EfiCopyMem (&FontInfo->BackgroundColor, Background, sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
  } else {
    EfiCopyMem (
      &FontInfo->BackgroundColor,
      &mEfiColors[Sto->Mode->Attribute >> 4],
      sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
      );
  }

  Status = HiiFont->StringToImage (
                       HiiFont,
                       EFI_HII_IGNORE_IF_NO_GLYPH | EFI_HII_DIRECT_TO_SCREEN,
                       Buffer,
                       FontInfo,
                       &Blt,
                       X,
                       Y,
                       NULL,
                       NULL,
                       NULL
                       );

#else
  GlyphStatus = 0;

  UnicodeWeight = (CHAR16 *) Buffer;

  for (Index = 0, BufferLen = 0, ImageCharLen = 0; UnicodeWeight[Index] != 0; Index++) {
    BufferLen ++;
    if (UnicodeWeight[Index] < NARROW_CHAR) {
      ImageCharLen ++;
      if (UnicodeWeight[Index] > 0xff) {
        ImageCharLen ++;
      }
    }
  }
  if (GLYPH_WIDTH * GLYPH_HEIGHT * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * ImageCharLen > LineBufferLen) {
    Status = EFI_INVALID_PARAMETER;
    goto Error;
  }
  if (Foreground == NULL || Background == NULL) {
    ForeColor = mEfiColors[Sto->Mode->Attribute & 0x0f];
    BackColor = mEfiColors[Sto->Mode->Attribute >> 4];
  } else {
    ForeColor = *Foreground;
    BackColor = *Background;
  }
  for (Index = 0, ImageCharIndex = 0, WideCharFlag = FALSE; Index < BufferLen; Index++) {
    if (UnicodeWeight[Index] == CHAR_BACKSPACE ||
        UnicodeWeight[Index] == CHAR_LINEFEED  ||
        UnicodeWeight[Index] == CHAR_CARRIAGE_RETURN) {
           Unicode[0] = 0;
    } else if (UnicodeWeight[Index] == NARROW_CHAR ||
               UnicodeWeight[Index] == WIDE_CHAR) {
      continue;
    } else if (UnicodeWeight[Index] > 0xff) {
      Unicode[0] = WIDE_CHAR;
      Unicode[1] = UnicodeWeight[Index];
      WideCharFlag = TRUE;
    } else if (WideCharFlag) {
      Unicode[0] = NARROW_CHAR;
      Unicode[1] = UnicodeWeight[Index];
      WideCharFlag = FALSE;
    } else {
      Unicode[0] = UnicodeWeight[Index];
    }
    StringIndex = 0;
    Status      = Hii->GetGlyph (Hii, Unicode, &StringIndex, (UINT8 **) &Glyph, &GlyphWidth, &GlyphStatus);
    if (EFI_ERROR (Status)) {
      goto Error;
    }
GlyphToBlt:
    Status = Hii->GlyphToBlt (
                    Hii,
                    (UINT8 *) Glyph,
                    ForeColor,
                    BackColor,
                    ImageCharLen,
                    GLYPH_WIDTH,
                    GLYPH_HEIGHT,
                    &LineBuffer[ImageCharIndex * GLYPH_WIDTH]
                    );
    ImageCharIndex ++;
    if (GlyphWidth == 0x10) {
      EfiCopyMem (&WideGlyph, Glyph, 4);
      EfiCopyMem (WideGlyph.GlyphCol1, ((EFI_WIDE_GLYPH*)Glyph)->GlyphCol2, sizeof (Glyph->GlyphCol1));
      Glyph = &WideGlyph;
      GlyphWidth = 0;
      goto GlyphToBlt;
    }
  }
  BufferLen = ImageCharLen;

  //
  // Blt a character to the screen
  //
  if (GraphicsOutput != NULL) {
    Status = GraphicsOutput->Blt (
                        GraphicsOutput,
                        LineBuffer,
                        EfiBltBufferToVideo,
                        0,
                        0,
                        X,
                        Y,
                        GLYPH_WIDTH * BufferLen,
                        GLYPH_HEIGHT,
                        GLYPH_WIDTH * BufferLen * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                        );
  } else {
    Status = UgaDraw->Blt (
                        UgaDraw,
                        (EFI_UGA_PIXEL *) LineBuffer,
                        EfiUgaBltBufferToVideo,
                        0,
                        0,
                        X,
                        Y,
                        GLYPH_WIDTH * BufferLen,
                        GLYPH_HEIGHT,
                        GLYPH_WIDTH * BufferLen * sizeof (EFI_UGA_PIXEL)
                        );
  }
#endif

Error:
#if (EFI_SPECIFICATION_VERSION >= 0x0002000A)
  if (Blt != NULL) {
    gBS->FreePool (Blt);
  }
  if (FontInfo != NULL) {
    gBS->FreePool (FontInfo);
  }
#else
  gBS->FreePool (LineBuffer);
#endif
  gBS->FreePool (Buffer);

  return Status;
}

UINTN
PrintXY (
  IN UINTN                            X,
  IN UINTN                            Y,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *ForeGround, OPTIONAL
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *BackGround, OPTIONAL
  IN CHAR_W                           *Fmt,
  ...
  )
/*++

Routine Description:

    Prints a formatted unicode string to the default console

Arguments:

    X           - X coordinate to start printing

    Y           - Y coordinate to start printing

    ForeGround  - Foreground color

    BackGround  - Background color

    Fmt         - Format string

    ...         - Print arguments

Returns:

    Length of string printed to the console

--*/
{
  EFI_HANDLE                    Handle;
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL         *UgaDraw;
  EFI_SIMPLE_TEXT_OUT_PROTOCOL  *Sto;
  EFI_STATUS                    Status;
  VA_LIST                       Args;

  VA_START (Args, Fmt);

  Handle = gST->ConsoleOutHandle;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiGraphicsOutputProtocolGuid,
                  &GraphicsOutput
                  );

  UgaDraw = NULL;
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;

    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiUgaDrawProtocolGuid,
                    &UgaDraw
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiSimpleTextOutProtocolGuid,
                  &Sto
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return _IPrint (GraphicsOutput, UgaDraw, Sto, X, Y, ForeGround, BackGround, Fmt, Args);
}


