//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
//;   ConsoleLib.h
//;
//; Abstract:
//;
//;   Interface definition for ConsoleLib.
//;
//;

#ifndef _EFI_CONSOLE_LIBRARY_H
#define _EFI_CONSOLE_LIBRARY_H

//
// Statements that include other header files
//
#include "Tiano.h"
#include "TianoHii.h"

//
// Console Library API
//
EFI_STATUS
GetGlyphWidth (
  IN  CHAR16                              UnicodeChar,
  OUT UINT32                              *GlyphWidth
  );

UINTN
GetStringWidth (
  CHAR16                                  *String
  );

EFI_STATUS
UnicodeStrDisplayLen (
  IN  CHAR16                              *UnicodeStr,
  OUT UINT32                              *DisplayLength
  );

#endif
