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

#ifndef _UI_FUNCS_H
#define _UI_FUNCS_H

#include <Protocol/H2OWindow.h>

extern UINT32                   mDbgNo;
extern UINT32                   mSetPositionLevel;
extern BOOLEAN                  mShowSetPositionDbg;
extern BOOLEAN                  mShowUpdateRegionDbg;


typedef struct _UI_CONTROL_CLASS UI_CONTROL_CLASS;

typedef
UI_CONTROL_CLASS *
(EFIAPI *UI_GET_CLASS) (
  VOID
  );

VOID
EFIAPI
RegisterClassTable (
  UI_GET_CLASS                  *GetClassTable
  );

EFI_IMAGE_INPUT *
GetImageByString (
  CHAR16  *Name
  );

COLORREF
EFIAPI
GetColorValue (
  CHAR16 *Value
  );

BOOLEAN
EFIAPI
IsHexDigit (
  OUT UINT8      *Digit,
  IN  CHAR16      Char
  );

CHAR16 *
StrDuplicate (
  IN CONST CHAR16  *Src
  );

BOOLEAN
EFIAPI
UiSetAttribute (
  VOID       *This,
  CHAR16     *Name,
  CHAR16     *Value
  );

UI_CONTROL *
UiFindChildByName (
  VOID                          *This,
  CHAR16                        *Name
  );

VOID
ShowSetPositionDbgInfo (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  );

VOID
ShowUpdateRegionDbgInfo (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  );


VOID
UiNeedUpdatePos (
  VOID                          *This
  );


#endif
