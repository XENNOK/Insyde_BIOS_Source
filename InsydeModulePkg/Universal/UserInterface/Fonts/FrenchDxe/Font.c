/** @file
 Font database implementation

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

#include "Font.h"

extern EFI_NARROW_GLYPH mFontNarrowGlyphData[];
extern UINT16           mFontNarrowGlyphNumber;

#pragma pack(push, 1)
typedef struct {
  UINT8  Id[8];
  UINT32 EZH2OVersion;
  UINT32 FontNarrowGlyphDataOffset;
  UINT32 Reserved1;
  UINT32 FontNarrowGlyphNumberOffset;
  UINT32 Reserved2;
  UINT32 FontWideGlyphDataOffset;
  UINT32 Reserved3;
  UINT32 FontWideGlyphNumberOffset;
  UINT32 Reserved4;
  UINT32 LangCode;
  UINT32 Reserved5;
} EFI_FONT_PACKAGE_HEAD;
#pragma pack(pop)

EFI_FONT_PACKAGE_HEAD     FONT_PACKAGE_HEAD =
                           { {'$', 'F', 'O', 'N', 'T', 'P', 'K', 'G'},
                             0x02,
                             (UINT32)(UINTN)mFontNarrowGlyphData,    0,
                             (UINT32)(UINTN)&mFontNarrowGlyphNumber, 0,
                             0,                                      0,
                             0,                                      0,
                             (UINT32)(UINTN) "fr-FR",                0
                           };
//
// Globals
//
EFI_FONT_DATABASE_PROTOCOL     mFontDatabase = {
  mFontNarrowGlyphData,
  NULL,
  0,
  0,
  (UINT8 *)"fr-FR"
} ;


//
//Init Font Database
//
/**
 Initializes the Font Database Driver

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS    The entry point is executed successfully.
 @retval Others         Some error occurs when executing this entry point.
**/
EFI_STATUS
InitializeFontDatabase (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  UINT32      DummyVersion;

  //
  // Do not remove, it is for compiler optimization
  //
  DummyVersion = FONT_PACKAGE_HEAD.EZH2OVersion;

  //
  // Create a handle for the device
  //
  mFontDatabase.NumOfNarrowGlyph = mFontNarrowGlyphNumber;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiFontDatabaseProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mFontDatabase
                  );

  return Status;
}

