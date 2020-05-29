/** @file
  Protocol for describing Font Database.

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

#ifndef _EFI_FONT_DATABASE_PROTOCOL_H_
#define _EFI_FONT_DATABASE_PROTOCOL_H_

#define EFI_FONT_DATABASE_PROTOCOL_GUID  \
  { 0x2f432bab, 0xabe0, 0x4f0b, 0x94, 0x49, 0x9f, 0xba, 0x68, 0x7e, 0x6, 0xac };


//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_FONT_DATABASE_PROTOCOL  EFI_FONT_DATABASE_PROTOCOL;

struct _EFI_FONT_DATABASE_PROTOCOL {
  EFI_NARROW_GLYPH                      *FontNarrowGlyph;
  EFI_WIDE_GLYPH                        *FontsWideGlyph;
  UINT16                                NumOfNarrowGlyph;
  UINT16                                NumOfWideGlyph;
  UINT8                                 *SupportedLanguages;
};

extern EFI_GUID gEfiFontDatabaseProtocolGuid;

#endif
