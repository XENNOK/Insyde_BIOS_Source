/** @file
  Header file for true type decoder interfaces

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

#ifndef _TRUE_TYPE_DECODER_DXE_H_
#define _TRUE_TYPE_DECODER_DXE_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/TrueTypeDecoder.h>


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
  );

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
  );

#endif

