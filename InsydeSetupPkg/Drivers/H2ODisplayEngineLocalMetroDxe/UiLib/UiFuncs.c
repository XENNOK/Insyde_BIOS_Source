/** @file
  UI Common Controls

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
#include "UiControls.h"
#include "UiManager.h"
#include "UiRender.h"
#include "H2ODisplayEngineLocalMetro.h"
#include "MetroUi.h"

UINT32                          mDbgNo               = 0;
UINT32                          mSetPositionLevel    = 0;
BOOLEAN                         mShowSetPositionDbg  = FALSE;
BOOLEAN                         mShowUpdateRegionDbg = FALSE;
STATIC CHAR16                   mPaddingChar[201] = {0};


VOID
SetUnicodeMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN CHAR16 Value
  )
/*++

Routine Description:

  Set Buffer to Value for Size bytes.

Arguments:

  Buffer  - Memory to set.

  Size    - Number of bytes to set

  Value   - Value of the set operation.

Returns:

  None

--*/
{
  CHAR16  *Ptr;

  Ptr = Buffer;
  while (Size--) {
    *(Ptr++) = Value;
  }
}


VOID
ShowSetPositionDbgInfo (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  )
{
  if (!mShowSetPositionDbg) {
    return ;
  }

  if (mSetPositionLevel == 1) {
    SetUnicodeMem (mPaddingChar, 200, '-');
  }

  if (mSetPositionLevel > 50) {
    ASSERT (FALSE);
  }

  mPaddingChar[(mSetPositionLevel << 1) + 1] = '\0';
  DEBUG ((
    EFI_D_INFO,
    "%06d:SetPosition %4d %4d %4d %4d:%s(%s):%s:%d\n",
    mDbgNo++,
    Pos->left,
    Pos->top,
    Pos->right - Pos->left,
    Pos->bottom - Pos->top,
    mPaddingChar,
    Control->Class->ClassName,
    ((Control->Name[0] != '\0') ? Control->Name : L""),
    mSetPositionLevel
    ));
  mPaddingChar[(mSetPositionLevel << 1) + 1] = '-';
}

VOID
ShowUpdateRegionDbgInfo (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  )
{
  if (!mShowUpdateRegionDbg) {
    return;
  }

  DEBUG ((
    EFI_D_INFO, "%06d:UpdateRegion %4d %4d %4d %4d:(%s)\n",
    mDbgNo++,
    Pos->left,
    Pos->top,
    Pos->right - Pos->left,
    Pos->bottom - Pos->top,
    Control->Class->ClassName
    ));
}



/**
 Determines if a Unicode character is a hexadecimal digit.
 The test is case insensitive.

 @param[out] Digit              Pointer to byte that receives the value of the hex character.
 @param[in]  Char               Unicode character to test.

 @retval TRUE                   If the character is a hexadecimal digit.
 @retval FALSE                  Otherwise.
**/
BOOLEAN
EFIAPI
IsHexDigit (
  OUT UINT8      *Digit,
  IN  CHAR16      Char
  )
{
  if ((Char >= L'0') && (Char <= L'9')) {
    *Digit = (UINT8) (Char - L'0');
    return TRUE;
  }

  if ((Char >= L'A') && (Char <= L'F')) {
    *Digit = (UINT8) (Char - L'A' + 0x0A);
    return TRUE;
  }

  if ((Char >= L'a') && (Char <= L'f')) {
    *Digit = (UINT8) (Char - L'a' + 0x0A);
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
EFIAPI
UiSetAttribute (
  VOID                          *This,
  CHAR16                        *Name,
  CHAR16                        *Value
  )
{
  return CONTROL_CLASS(This)->SetAttribute ((UI_CONTROL *)This, Name, Value);
}

UI_CONTROL *
UiFindChildByName (
  VOID                          *This,
  CHAR16                        *Name
  )
{
  return CONTROL_CLASS(This)->FindChildByName ((UI_CONTROL *)This, Name);
}

VOID
EFIAPI
UiApplyAttributeList (
  UI_CONTROL *Control,
  CHAR16     *StrList
  )
{
  CHAR16                        Name[256];
  CHAR16                        Value[256];
  UINTN                         Index;
  CHAR16                        QuoteChar;

  while(*StrList != L'\0') {
    Index = 0;

    //
    // skip space or tab
    //
    while ((*StrList == L' ') || (*StrList == L'\t')) {
      StrList++;
    }

    while( *StrList != L'\0' && *StrList != L'=') {
      Name[Index++] = *StrList++;
    }
    Name[Index] = '\0';

    ASSERT(*StrList == L'=');
    if(*StrList++ != L'=') {
      return ;
    }

    ASSERT(*StrList == L'\"' || *StrList == L'\'');
    if (*StrList != L'\"' && *StrList != L'\'') {
      return ;
    }

    QuoteChar = *StrList;
    StrList++;
    Index = 0;
    while(*StrList != L'\0' && *StrList != QuoteChar) {
      if (*StrList == '\\' && *(StrList + 1) == QuoteChar) {
        Value[Index++] = *StrList++;
      }
      Value[Index++] = *StrList++;
    }
    Value[Index] = '\0';

    ASSERT(*StrList == L'\"' || *StrList == L'\'');
    if (*StrList != L'\"' && *StrList != L'\'') {
      return ;
    }

    UiSetAttribute (Control, Name, Value);

    StrList++;
    if(*StrList != L' ' && *StrList != '\t') {
      return ;
    }
    StrList++;
  }
}

UI_CONTROL *
EFIAPI
GetUiControl (
  HWND Hwnd
  )
{

  return (UI_CONTROL *) GetWindowLongPtr (Hwnd, 0);
}

UI_CONTROL *
EFIAPI
CreateControl (
  CHAR16     *ControlName,
  UI_CONTROL *Parent
  )
{
  HWND                          Wnd;
  UI_MANAGER                    *Manager;
  
  Manager = NULL;
  if (Parent != NULL) {
    Manager = Parent->Manager;
    ASSERT (Manager != NULL);
  }

  Wnd = CreateWindowEx (
          WS_EX_NOACTIVATE,
          ControlName,
          NULL,
          ((Parent != NULL) ? WS_CHILD : 0) | WS_VISIBLE,
          0, 0, 0, 0,
          (Parent != NULL) ? Parent->Wnd : NULL,
          NULL,
          NULL,
          Manager
          );

  ASSERT (Wnd != NULL);

  if (Wnd == NULL) {
    return NULL;
  }

  return GetUiControl (Wnd);
}

STATIC
COLORREF
GetMenuColor (
  VOID
  )
{
  HSV_VALUE                        Hsv;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    Pixel;

  GetCurrentMenuHsv (&Hsv);
  if (Hsv.Hue == 0 && Hsv.Saturation == 0 && Hsv.Value == 0) {
    //
    // Use default color if current menu image doesn't exist.
    //
    return 0xFF834EAD;
  }
  HSV2RGB (&Hsv, &Pixel);

  return (0xFF000000 | (Pixel.Red << 16) | (Pixel.Green << 8) | Pixel.Blue);
}

STATIC
COLORREF
GetMenuLightColor (
  VOID
  )
{
  HSV_VALUE                        Hsv;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    Pixel;

  GetCurrentMenuHsv (&Hsv);
  if (Hsv.Hue == 0 && Hsv.Saturation == 0 && Hsv.Value == 0) {
    //
    // Use default light color if current menu image doesn't exist.
    //
    return 0xFF9D72C0;
  }
  Hsv.Hue = Hsv.Hue <= 5 ? Hsv.Hue + 360 - 5 : Hsv.Hue - 5;
  Hsv.Saturation = Hsv.Saturation <= 3 ? 0 : Hsv.Saturation - 3;  HSV2RGB (&Hsv, &Pixel);
  Hsv.Value = Hsv.Value >= 73 ? 100 : Hsv.Value + 27;
  HSV2RGB (&Hsv, &Pixel);
  return (0xFF000000 | (Pixel.Red << 16) | (Pixel.Green << 8) | Pixel.Blue);
}

COLORREF
EFIAPI
GetColorValue (
  CHAR16 *Value
  )
{
  EFI_STATUS Status;
  COLORREF   Color;

  while (*Value > L'\0' && *Value <= L' ') Value++;
  if (*Value == '#') Value++;
  if (*Value == L'@') {
    Value++;
    if (StrCmp (Value, L"menucolor") == 0) {
      return GetMenuColor ();
    } else if (StrCmp (Value, L"menulightcolor") == 0) {
      return GetMenuLightColor ();
    }
    ASSERT (FALSE);
  }

  Color = (COLORREF) StrToUInt (Value, 16, &Status);

  return Color;
}

UI_CONTROL *
EFIAPI
ContainerCreateControl (
  UI_CONTROL *Container,
  CHAR16     *ControlName
  )
{
  UI_CONTROL *Control;

  Control = CreateControl (ControlName,  (UI_CONTROL *) Container);
  CONTROL_CLASS(Container)->AddChild (Container, Control);

  return Control;
}

VOID
EFIAPI
RegisterClassTable (
  UI_GET_CLASS                  *GetClassTable
  )
{
  UINT8                         Buf[sizeof (WNDCLASS) + sizeof (UI_CONTROL_CLASS *)];
  WNDCLASS                      *WndClass;
  UI_CONTROL_CLASS              *ControlClass;
  UI_GET_CLASS                  *GetClass;

  WndClass                      = (WNDCLASS *) Buf;
  WndClass->style               = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  WndClass->lpfnWndProc         = (WNDPROC) NULL;
  WndClass->cbClsExtra          = sizeof (UI_CONTROL_CLASS *);
  WndClass->cbWndExtra          = sizeof (UI_CONTROL *);
  WndClass->hInstance           = NULL;
  WndClass->hIcon               = NULL;
  WndClass->hCursor             = 0;
  WndClass->hbrBackground       = GetStockObject(NULL_BRUSH);
  WndClass->lpszMenuName        = NULL;
  WndClass->lpszClassName       = NULL;


  GetClass = GetClassTable;

  while ((*GetClass) != NULL) {
    ControlClass = (*GetClass)();
    ASSERT (ControlClass != NULL);
    if (ControlClass == NULL) {
      continue;
    }

    WndClass->lpfnWndProc       = ControlClass->WndProc;
    WndClass->lpszClassName     = ControlClass->ClassName;

    *(UI_CONTROL_CLASS **)(Buf + sizeof (WNDCLASS))   = ControlClass;
    RegisterClass(WndClass);

    GetClass++;
  }
}

CHAR16 *
StrDuplicate (
  IN CONST CHAR16  *Src
  )
{
  return AllocateCopyPool (StrSize (Src), Src);
}

EFI_STATUS
EFIAPI
InitUiClass (
  UI_CONTROL_CLASS              **Class,
  UINT32                        ClassSize,
  CHAR16                        *ClassName,
  UI_CONTROL_CLASS              *ParentClass
  )
{
  ASSERT (ParentClass != NULL);
  if (ParentClass == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Class = AllocateZeroPool (ClassSize);
  ASSERT (*Class != NULL);
  if (*Class == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (*Class, ParentClass, ParentClass->ClassSize);

  (*Class)->Parent     = ParentClass;
  (*Class)->ClassSize  = ClassSize;
  (*Class)->ClassName  = StrDuplicate (ClassName);

  return EFI_SUCCESS;
}

VOID
UiNeedUpdatePos (
  VOID                          *This
  )
{
  UI_CONTROL                    *Control;
  UI_CONTROL                    *Parent;

  Control = (UI_CONTROL *)This;

  Control->NeedUpdateChildPos = TRUE;
  CONTROL_CLASS (Control)->Invalidate (Control);

  Parent = CONTROL_CLASS(Control)->GetParent(Control);
  if (Parent != NULL) {
    CONTROL_CLASS (Control)->Invalidate (Control);
  }
  while (Parent != NULL) {
    Parent->NeedUpdateChildPos = TRUE;
    Parent = CONTROL_CLASS (Parent)->GetParent(Parent);
  }
}


