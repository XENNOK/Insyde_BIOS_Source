//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TRUETYPE_LIB_H
#define _TRUETYPE_LIB_H

#include "Tiano.h"
#include "EfiHii.h"

EFI_STATUS 
TrueTypeInit (
  VOID
  )
/*++

  Routine Description:
    Init freetype2 library 

  Arguments:
    None
    
  Returns:
    EFI_SUCCESS     - Init success 
    EFI_LOAD_ERROR  - Init TrueType library fail 
    
--*/  
;

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
;

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
;

EFI_STATUS 
TrueTypeGetGlyph (
  IN     CHAR16             CharValue, 
  IN     UINT8              *GlyphBuffer,   
  IN OUT UINTN              *GlyphBufferSize,
  OUT    EFI_HII_GLYPH_INFO *Cell  OPTIONAL
  )
/*++

  Routine Description:
    load glyph data  

  Arguments:
    CharValue       - Cahr value by unicode 
    GlyphBuffer     - GlyphBuffer must have value, if *GlyphBuffer is NULL, 
                      then auto allcoate memory for glyph
    GlyphBufferSize - If *GlyphBuffer Size, 
    Cell            - Cell information 
    
    
  Returns:
    EFI_SUCCESS     - Set font size success 
    EFI_NOT_READY   - TrueType doesn't init 


--*/      
;
  
#endif 
