/** @file
 Provide True type decoder related interfaces

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "TrueTypeDecoderDxe.h"
#include <ft2build.h>

#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_SYNTHESIS_H
#include FT_CACHE_MANAGER_H

#define H2O_HII_FONT_STYLE_MONOSPACE    0x10000000


TRUE_TYPE_DECODER_PROTOCOL      mTruetypeDecoder = {
  SetDataBase,
  GetGlyph
};

#define  DEFAULT_FONT_SIZE 16

STATIC FT_Library               mFtLibrary  = NULL;
STATIC FT_Face                  mFtFace     = NULL;
//STATIC UINT16                   mFontSize   = 0;
//STATIC BOOLEAN                  mBold       = FALSE;

STATIC FTC_Manager              mFTCManager;
STATIC FTC_ImageCache           mFTCImageCache;

/**
 Init TrueType library.

 @retval EFI_SUCCESS            Init TrueType library successfully.
 @retval EFI_LOAD_ERROR         Init TrueType library failed.
**/
STATIC
EFI_STATUS
TrueTypeInit (
  VOID
  )
{
  FT_Error Error;

  if (mFtLibrary != NULL) {
    return EFI_SUCCESS;
  }

  Error = FT_Init_FreeType(&mFtLibrary);
  ASSERT (Error == FT_Err_Ok);
  if (Error) {
    return EFI_LOAD_ERROR;
  }

  return EFI_SUCCESS;
}

/**
 Initializes the TTF font driver

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS    The entry point is executed successfully.
 @retval Others         Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
InitTrueTypeDecoder (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  Status = TrueTypeInit ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Handle = NULL;
  return gBS->InstallProtocolInterface (
                  &Handle,
                  &gTrueTypeDecoderProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mTruetypeDecoder
                  );
}

STATIC
FT_Error
FaceRequester (
  FTC_FaceID                    FaceId,
  FT_Library                    Library,
  FT_Pointer                    RequestData,
  FT_Face                       *Aface
  )
{
  *Aface = mFtFace;

  return FT_Err_Ok;
}


/**
 Set TTF font database according to input buffer and buffer size.

 Note: Previous database will be replaced by input database if it exists.

 @param[in] This                A pointer to the TRUE_TYPE_DECODER_PROTOCOL instance.
 @param[in] Buffer              TTF file buffer
 @param[in] BufferSize          input buffer size

 @retval EFI_SUCCESS            Set TTF database successfully.
 @retval EFI_INVALID_PARAMETER  Input buffer is NULL or BufferSize is 0
 @retval EFI_NOT_READY          TrueType related code isn't ready.
**/
EFI_STATUS
EFIAPI
SetDataBase (
  IN  TRUE_TYPE_DECODER_PROTOCOL  *This,
  IN  UINT8                       *Buffer,
  IN  UINT32                      BufferSize
  )
{
  FT_Face     FtFace;
  FT_Error    Error;

  ASSERT (mFtLibrary != NULL);
  if (mFtLibrary == NULL) {
    return EFI_NOT_READY;
  }

  Error = FT_New_Memory_Face (mFtLibrary, Buffer, (UINT32)BufferSize, 0, &FtFace);
  ASSERT (Error == FT_Err_Ok);
  if (Error != FT_Err_Ok) {
    return EFI_NOT_READY;
  }
  mFtFace = FtFace;

  Error = FTC_Manager_New (mFtLibrary, 1, 20, 4 * 1024 * 1024, &FaceRequester, NULL, &mFTCManager);
  ASSERT (Error == FT_Err_Ok);
  if (Error != FT_Err_Ok) {
    return EFI_NOT_READY;
  }

  Error  = FTC_ImageCache_New (mFTCManager, &mFTCImageCache);
  ASSERT (Error == FT_Err_Ok);
  if (Error != FT_Err_Ok) {
    return EFI_NOT_READY;
  }

  return EFI_SUCCESS;
}

/**
 Function to get glyph information.

 @param[in]     This             A pointer to the TRUE_TYPE_DECODER_PROTOCOL instance.
 @param[in]     CharValue        Unicode character value.
 @param[out]    GlyphBuffer      Output the corresponding bitmap data. if GlyphBufferSize is NULL,
                                 GlyphBuffer will be ignored.
 @param[in,out] GlyphBufferSize  On input, the size in bytes of the return glyph buffer. ƒn
                                 On output the size of data returned in glyph buffer.
                                 if GlyphBufferSize is NULL, GlyphBuffer will be ignored.
 @param[out]    Cell             Output cell information of the encoded bitmap.
 @param[out]    Baseline         The distance from upper left corner of cell to the base of the Caps.

 @retval EFI_SUCCESS             Get glyph information successfully.
 @retval EFI_NOT_FOUND           Cannot find input character from TTF database.
 @retval EFI_BUFFER_TOO_SMALL    The GlyphBufferSize is too small for the result. GlyphBufferSize has been updated with the size
                                 needed to complete the request.
 @retval EFI_NOT_READY           TTF related code isn't ready.
**/
EFI_STATUS
EFIAPI
GetGlyph (
  IN     TRUE_TYPE_DECODER_PROTOCOL  *This,
  IN     CHAR16                      CharValue,
  IN     UINT16                      FontSize,
  IN     EFI_HII_FONT_STYLE          FontStyle,
  IN     UINT8                       *GlyphBuffer      OPTIONAL,
  IN OUT UINTN                       *GlyphBufferSize  OPTIONAL,
  OUT    EFI_HII_GLYPH_INFO          *Cell             OPTIONAL,
  OUT    UINTN                       *BaseLine         OPTIONAL
  )
{
  FT_Bitmap                     *FtBitmap;
  FT_Face                       FtFace;
  FT_Glyph                      FtGlyph;
  FT_BitmapGlyph                FtBitmapGlyph;
  FT_UInt                       CharIndex;
  FT_Error                      Error;
  UINTN                         BufferSize;
  EFI_TPL                       OriginalTPL;
  FTC_ImageTypeRec              FtcFontType;
  BOOLEAN                       Bold;
  FTC_ScalerRec                 FtScaler;
  FT_Pos                        NarrowAdvanceX;
  BOOLEAN                       WideGlyph;

  ASSERT (mFtLibrary != NULL);
  ASSERT (mFtFace != NULL);

  if (mFtLibrary == NULL || mFtFace == NULL) {
    return EFI_NOT_READY;
  }

  if (GlyphBuffer == NULL && (GlyphBufferSize != NULL && *GlyphBufferSize != 0)) {
    return EFI_INVALID_PARAMETER;
  }

  FtFace = mFtFace;
  CharIndex = FT_Get_Char_Index (FtFace, CharValue);
  if (CharIndex == 0) {
    return EFI_NOT_FOUND;
  }

  Bold = ((FontStyle &  EFI_HII_FONT_STYLE_BOLD) == EFI_HII_FONT_STYLE_BOLD) ? TRUE : FALSE;

  FtcFontType.face_id = 0;
  FtcFontType.width   = 0;
  FtcFontType.height  = FontSize;
  FtcFontType.flags   = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;

  OriginalTPL = gBS->RaiseTPL (TPL_NOTIFY);

  FtScaler.face_id = 0;
  FtScaler.width   = FontSize << 6;
  FtScaler.height  = FontSize << 6;
  FtScaler.pixel   = 0;
  FtScaler.x_res   = 0;
  FtScaler.y_res   = 0;

  Error = FTC_ImageCache_LookupScaler (
            mFTCImageCache,
            &FtScaler,
            FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL,
            CharIndex,
            &FtGlyph,
            NULL
            );
  if (Error != FT_Err_Ok) {
    gBS->RestoreTPL (OriginalTPL);
    return EFI_NOT_FOUND;
  }

  WideGlyph = FALSE;
  if ((FtGlyph->advance.x >> 16) == FontSize) {
    WideGlyph = TRUE;
  }
  
  if (((FontStyle & H2O_HII_FONT_STYLE_MONOSPACE) == H2O_HII_FONT_STYLE_MONOSPACE) &&
      !WideGlyph) {

    NarrowAdvanceX = (FontSize << 16) / 2;
    if ((CharValue <= 0xFF) && (FtGlyph->advance.x != NarrowAdvanceX)) {
      FtScaler.face_id = 0;
      FtScaler.width   = (64 * NarrowAdvanceX / FtGlyph->advance.x) * FontSize;
      FtScaler.height  = FontSize << 6;
      FtScaler.pixel   = 0;
      FtScaler.x_res   = 0;
      FtScaler.y_res   = 0;

      Error = FTC_ImageCache_LookupScaler (
                mFTCImageCache,
                &FtScaler,
                FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL,
                CharIndex,
                &FtGlyph,
                NULL
                );
      if (Error != FT_Err_Ok) {
        gBS->RestoreTPL (OriginalTPL);
        return EFI_NOT_FOUND;
      }
    }
  }

  ASSERT (FtGlyph->format == FT_GLYPH_FORMAT_BITMAP);
  if (FtGlyph->format != FT_GLYPH_FORMAT_BITMAP) {
    gBS->RestoreTPL (OriginalTPL);
    return EFI_NOT_FOUND;
  }

  if (Bold) {
    //
    // BUGBUG: It need copy glyph in FTC
    //
//  FT_GlyphSlot_Embolden (FtGlyph);
  }

  FtBitmapGlyph = (FT_BitmapGlyph) FtGlyph;
  FtBitmap      = &(FtBitmapGlyph->bitmap);

  if (FtBitmap->pitch < 0) {
    BufferSize = FtBitmap->pitch * (-1) * FtBitmap->rows;
  } else {
    BufferSize = FtBitmap->pitch * FtBitmap->rows;
  }

  if (GlyphBufferSize != NULL) {
    if (*GlyphBufferSize < BufferSize) {
      *GlyphBufferSize = BufferSize;
      gBS->RestoreTPL (OriginalTPL);
      return EFI_BUFFER_TOO_SMALL;
    }
    if (GlyphBuffer != NULL) {
      CopyMem (GlyphBuffer, FtBitmap->buffer, BufferSize);
    }
  }

  if (Cell != NULL) {
    Cell->Width    = (UINT16)FtBitmap->width;
    Cell->Height   = (UINT16)FtBitmap->rows;
    Cell->OffsetX  = (INT16) FtBitmapGlyph->left;
    Cell->OffsetY  = (INT16) (FtBitmapGlyph->top - FtBitmap->rows);
    Cell->AdvanceX = (INT16) (FtGlyph->advance.x >> 16);

    if (((FontStyle & H2O_HII_FONT_STYLE_MONOSPACE) == H2O_HII_FONT_STYLE_MONOSPACE) &&
        !WideGlyph) {
        Cell->AdvanceX = FontSize / 2;
    }
  }

  if (BaseLine != NULL) {
    *BaseLine = (FontSize * 75) / 100;
  }

  gBS->RestoreTPL (OriginalTPL);

  return EFI_SUCCESS;
}
