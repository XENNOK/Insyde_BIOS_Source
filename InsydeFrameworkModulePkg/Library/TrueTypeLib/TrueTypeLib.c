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
//;   TrueTypeLib
//; 
//; Abstract:
//; 
//;   Provide functions to access TTF file
//;
//;
#include "Tiano.h"
#include "EfiHii.h"
#include "TrueTypeLib.h"
#include "EfiDriverLib.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

#define  DEFAULT_FONT_SIZE 16

STATIC FT_Library mFtLibrary  = NULL; 
STATIC FT_Face    mFtFace     = NULL;
STATIC UINT16     mFontSize   = 0; 

EFI_STATUS 
TrueTypeInit (
  VOID
  )
/*++

  Routine Description:
    Init TrueType library 

  Arguments:
    None
    
  Returns:
    EFI_SUCCESS     - Init success 
    EFI_LOAD_ERROR  - Init TrueType library fail 
    
--*/  
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

EFI_STATUS 
TrueTypeLoadMemory (
  IN  UINT8      *Buffer, 
  IN  UINT32     BufferSize
  )
/*++

  Routine Description:
    Load TTF file from memory, and get fontid to access 

  Arguments:
    Buffer          - TTF file buffer
    BufferLength    - TTF buffer size         
    
  Returns:
    EFI_SUCCESS     - Load TTF file success 
    EFI_LOAD_ERROR  - Load TTF file fail 
    EFI_NOT_READY   - TrueType doesn't init 
    
--*/  
{

  FT_Face     FtFace; 
  FT_Error    Error; 
  EFI_STATUS  Status; 

  ASSERT (mFtLibrary != NULL); 
    
  if (mFtLibrary == NULL) {
    return EFI_NOT_READY; 
  }

  Error = FT_New_Memory_Face (mFtLibrary, Buffer, (UINT32)BufferSize, 0, &FtFace); 
  ASSERT (Error == FT_Err_Ok); 
  if (Error != FT_Err_Ok) {
    return EFI_LOAD_ERROR; 
  }
  mFtFace = FtFace; 

  //
  // set font size
  // 
  Status = TrueTypeSetFontSize (DEFAULT_FONT_SIZE); 
  ASSERT_EFI_ERROR (Status); 
  if (EFI_ERROR (Status)) {
    return Status; 
  }

  return EFI_SUCCESS; 
}


EFI_STATUS 
TrueTypeSetFontSize (
  IN  UINT16    FontSize
  )
/*++

  Routine Description:
    Set font size before load glyph 

  Arguments:
    FontSize        - Font Height 
    
  Returns:
    EFI_SUCCESS     - Set font size success 

--*/    
{
  FT_Error                 Error; 

  ASSERT (mFtLibrary != NULL); 
  ASSERT (mFtFace != NULL);
  ASSERT (FontSize != 0); 
  
  if (FontSize == 0) {
    return EFI_INVALID_PARAMETER;
  }
  
  if (FontSize != mFontSize) { 
    Error = FT_Set_Pixel_Sizes (mFtFace, 0, FontSize); 
    ASSERT (Error == FT_Err_Ok); 
    if (Error != FT_Err_Ok) {
      return EFI_LOAD_ERROR; 
    }
    mFontSize = FontSize; 
  }
  
  return EFI_SUCCESS; 
}

EFI_STATUS 
TrueTypeGetGlyph (
  IN     CHAR16             CharValue, 
  IN     UINT8              *GlyphBuffer,     OPTIONAL
  IN OUT UINTN              *GlyphBufferSize, OPTIONAL
  OUT    EFI_HII_GLYPH_INFO *Cell             OPTIONAL
  )
/*++

  Routine Description:
    load glyph data  

  Arguments:
    CharValue              - Cahr value by unicode 
    GlyphBuffer            - if *GlyphBuffer isn't NULL, copy bitmap 
    GlyphBufferSize        - GlyphBuffer Size 
    Cell                   - Cell information 
    
    
  Returns:
    EFI_SUCCESS            - Set font size success 
    EFI_NOT_READY          - TrueType doesn't init 
    EFI_NOT_FOUND          - Can't find Glyph 
    EFI_UNSUPPORTED        - Font size unsupport 

--*/      
{
  FT_Bitmap                *FtBitmap; 
  FT_Face                  FtFace; 
  FT_GlyphSlot             FtGlyphSlot; 
  FT_UInt                  GlyphIndex; 
  FT_Error                 Error; 
  UINTN                    BufferSize; 

  ASSERT (mFtLibrary != NULL); 
  ASSERT (mFtFace != NULL);
  ASSERT (mFontSize != 0);
  
  if (mFtLibrary == NULL) {
    return EFI_NOT_READY; 
  }

  if (mFtFace == NULL) {
    return EFI_NOT_READY;
  }

  FtFace = mFtFace; 

  GlyphIndex = FT_Get_Char_Index(FtFace, CharValue);
  if (GlyphIndex == 0) {
    return EFI_NOT_FOUND; 
  }

  Error = FT_Load_Glyph (FtFace, GlyphIndex, FT_LOAD_DEFAULT); 
  ASSERT (Error == FT_Err_Ok); 
  if (Error != FT_Err_Ok) {
    return EFI_NOT_FOUND; 
  }

  Error = FT_Render_Glyph (FtFace->glyph, FT_RENDER_MODE_NORMAL); 
  ASSERT (Error == FT_Err_Ok); 
  if (Error != FT_Err_Ok) {
    return EFI_NOT_FOUND; 
  }    
  
  FtGlyphSlot = FtFace->glyph; 
  FtBitmap    = &FtGlyphSlot->bitmap; 

  if (FtBitmap->pitch < 0) {
    BufferSize = FtBitmap->pitch * (-1) * FtBitmap->rows;  
  } else {
    BufferSize = FtBitmap->pitch * FtBitmap->rows;  
  }

  if (GlyphBufferSize != NULL) {
    if (*GlyphBufferSize < BufferSize) {      
      *GlyphBufferSize = BufferSize; 
      return EFI_BUFFER_TOO_SMALL; 
    } 
  }  

  if (GlyphBuffer != NULL) {
    ASSERT (GlyphBufferSize != NULL);
    EfiCopyMem (GlyphBuffer, FtBitmap->buffer, BufferSize); 
  }

  if (Cell != NULL) {
    Cell->Width    = (UINT16)FtBitmap->width; 
    Cell->Height   = (UINT16)FtBitmap->rows; 
    Cell->OffsetX  = (INT16) FtGlyphSlot->bitmap_left; 
    Cell->OffsetY  = (INT16) ((FtFace->size->metrics.ascender >> 6) - FtGlyphSlot->bitmap_top); 
    Cell->AdvanceX = (INT16) (FtGlyphSlot->advance.x >> 6); 
  }

  return EFI_SUCCESS;    
}
  

