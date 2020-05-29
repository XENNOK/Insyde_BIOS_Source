/** @file
  UI Manager

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*/
#include "UiRender.h"
#include "UiControls.h"
#include "UiManager.h"

EFI_SETUP_MOUSE_PROTOCOL        *mSetupMouse = NULL;

UI_CONTROL *
UiManagerFindControlByName (
  IN UI_MANAGER *This,
  IN CHAR16     *Name
  )
{
  return (UI_CONTROL *)StringMapFind (&This->NameList, Name);
}

VOID
UiManagerGetControlRect (
  IN UI_MANAGER *This,
  IN UI_CONTROL *Control,
  OUT RECT      *Rect
  )
{

  RECT RootRc;

  GetWindowRect (This->Root, &RootRc);
  GetWindowRect (Control->Wnd, Rect);

  OffsetRect (Rect, -RootRc.left, -RootRc.top);
}

VOID
UiManagerSetControlRect (
  IN UI_MANAGER *This,
  IN UI_CONTROL *Control,
  IN RECT       *Rect
  )
{

  RECT RootRc;

  GetWindowRect (This->Root, &RootRc);
  OffsetRect (Rect, RootRc.left, RootRc.top);

  ScreenToClient (Control->Wnd->parent, (POINT *)Rect);
  ScreenToClient (Control->Wnd->parent, (POINT *)Rect + 1);

  MoveWindow (Control->Wnd, Rect->left, Rect->top,
    Rect->right - Rect->left + 1, Rect->bottom - Rect->top + 1, TRUE);
}

#define DEFAULT_FONT_SIZE 19

UINT32
UiManagerGetDefaultFontSize (
  IN UI_MANAGER *This
  )
{

  return This->DefaultFontSize;
}

UINTN
UiManagerSetDefaultFontSize (
  IN UI_MANAGER *This,
  IN UINT32      FontSize
  )
{

  UINTN OldFontSize;

  OldFontSize = This->DefaultFontSize;
  This->DefaultFontSize = FontSize;

  return OldFontSize;
}

VOID
InitUiManager (
  IN UI_MANAGER  *This
  )
{
  ZeroMem (This, sizeof (UI_MANAGER));

  This->Root = NULL;
  This->Instance = NULL;
  This->NeedUpdatePos = TRUE;
  This->DefaultFontSize = DEFAULT_FONT_SIZE;
  This->DoOffScreen     = TRUE;

  InitializeListHead (&This->OptionGroup.Link);
  InitializeListHead (&This->NameList.Link);

  This->FindControlByName     = UiManagerFindControlByName;
  This->GetControlRect        = UiManagerGetControlRect;
  This->SetControlRect        = UiManagerSetControlRect;
  This->SetDefaultFontSize    = UiManagerSetDefaultFontSize;
  This->GetDefaultFontSize    = UiManagerGetDefaultFontSize;
}

VOID
DestroyUiManager (
  IN UI_MANAGER  *This
  )
{
  StringMapRemoveAll  (&This->OptionGroup);
  StringMapRemoveAll  (&This->NameList);
  FreePool (This);
}

VOID
StrTrim (
  IN OUT CHAR16   *str,
  IN     CHAR16   c
  )
/*++

Routine Description:

  Removes (trims) specified leading and trailing characters from a string.

Arguments:

  str     - Pointer to the null-terminated string to be trimmed. On return,
      str will hold the trimmed string.
  c       - Character will be trimmed from str.

Returns:

--*/
{
  CHAR16  *p1;

  CHAR16  *p2;

  ASSERT (str != NULL);

  if (*str == 0) {
    return ;
  }
  //
  // Trim off the leading characters c
  //
  for (p1 = str; *p1 && *p1 == c; p1++) {
    ;
  }

  p2 = str;
  if (p2 == p1) {
    while (*p1) {
      p2++;
      p1++;
    }
  } else {
    while (*p1) {
      *p2 = *p1;
      p1++;
      p2++;
    }

    *p2 = 0;
  }

  for (p1 = str + StrLen (str) - 1; p1 >= str && *p1 == c; p1--) {
    ;
  }

  if (p1 != str + StrLen (str) - 1) {
    *(p1 + 1) = 0;
  }
}


UINT64
StrToUInt (
  IN     CHAR16       *Str,
  IN     UINTN  Base,
  OUT    EFI_STATUS   *Status
  )
/*++

Routine Description:
  Transfer a string to an unsigned integer

Arguments:
  Str        Source string
  Base       Base
  Status     The status

Returns:

  The value about the string

--*/
{
  UINT64  Value;
  UINT64  NewValue;
  CHAR16  *StrTail;
  CHAR16  c;
  UINTN   Len;

  Value   = 0;
  *Status = EFI_ABORTED;

  if (!*Str) {
    return 0;
  }

  StrTrim (Str, L' ');

  Len     = StrLen (Str);
  StrTail = Str + Len - 1;
  //
  // Check hex prefix '0x'
  //
  if (Len >= 2 && *Str == '0' && (*(Str + 1) == 'x' || *(Str + 1) == 'X')) {
    Str += 2;
    Len -= 2;
    if (Base != 16) {
      *Status = EFI_ABORTED;
      return 0;
    }
  }

  if (!Len) {
    return 0;
  }
  //
  // Convert the string to value
  //
  for (; Str <= StrTail; Str++) {

    c = *Str;

    if (Base == 16) {
      if (RShiftU64 (Value, 60)) {
  //
  // Overflow here x16
  //
  return 0;
      }

      NewValue = LShiftU64 (Value, 4);
    } else {
      if (RShiftU64 (Value, 61)) {
  //
  // Overflow here x8
  //
  return 0;
      }

      NewValue  = LShiftU64 (Value, 3);
      Value     = LShiftU64 (Value, 1);
      NewValue += Value;
      if (NewValue < Value) {
  //
  // Overflow here
  //
  return 0;
      }
    }

    Value = NewValue;

    if (Base == 16 && c >= 'a' && c <= 'f') {
      c = (CHAR16) (c - 'a' + 'A');
    }

    if (Base == 16 && c >= 'A' && c <= 'F') {
      Value += (c - 'A') + 10;
    } else if (c >= '0' && c <= '9') {
      Value += (c - '0');
    } else {
      //
      // Unexpected char is encountered
      //
      return 0;
    }
  }

  *Status = EFI_SUCCESS;
  return Value;
}

STATIC
STRING_MAP *
StringMapFindItem (
  STRING_MAP *Head,
  CHAR16     *Key
  )
{
  LIST_ENTRY *Link;
  STRING_MAP     *Item;

  Link = GetFirstNode ((LIST_ENTRY *)Head);

  while (!IsNull ((LIST_ENTRY *)Head, Link)) {
    Item = (STRING_MAP *)Link;
    if (StrCmp (Item->Key, Key) == 0) {
      return Item;
    }
    Link = GetNextNode ((LIST_ENTRY *)Head, Link);
  }

  return NULL;
}


BOOLEAN
StringMapAdd (
  STRING_MAP *Head,
  CHAR16 *Key,
  VOID *Data
  )
{
  STRING_MAP     *Item;

  ASSERT (Key != NULL);

  if (StringMapFindItem (Head, Key) != NULL) {
    return FALSE;
  }

  Item = AllocatePool (sizeof (STRING_MAP));
  InsertTailList((LIST_ENTRY *)Head, (LIST_ENTRY *)Item);

  Item->Key  = AllocateCopyPool (StrSize (Key), Key);
  Item->Data = Data;

  return TRUE;
}

VOID *
StringMapFind (
  STRING_MAP *Head,
  CHAR16     *Key
  )
{
  STRING_MAP *Item;

  Item = StringMapFindItem (Head, Key);
  if (Item != NULL) {
    return Item->Data;
  }
  return NULL;
}

VOID *
StringMapSet (
  STRING_MAP *Head,
  CHAR16     *Key,
  VOID       *Data
  )
{
  STRING_MAP *Item;
  VOID       *OldData;

  Item = StringMapFind (Head, Key);
  if (Item != NULL) {
    OldData = Item->Data;
    Item->Data = Data;
    return OldData;
  }

  StringMapAdd (Head, Key, Data);
  return NULL;
}


BOOLEAN
StringMapRemove (
  STRING_MAP *Head,
  CHAR16     *Key
  )
{
  STRING_MAP *Item;

  Item = StringMapFindItem (Head, Key);
  if (Item != NULL) {
    RemoveEntryList((LIST_ENTRY *)Item);
    ASSERT (Item->Key);
    gBS->FreePool (Item->Key);
    gBS->FreePool (Item);
    return TRUE;
  }
  return FALSE;
}

VOID
StringMapRemoveAll (
  STRING_MAP *Head
  )
{
  STRING_MAP     *Item;

  while (!IsListEmpty ((LIST_ENTRY *)Head)) {
    Item = (STRING_MAP *)GetFirstNode ((LIST_ENTRY *)Head);
    RemoveEntryList((LIST_ENTRY *)Item);
    ASSERT (Item->Key);
    gBS->FreePool (Item->Key);
    gBS->FreePool (Item);

  }
}


STRING_MAP *
StringMapGetAtItem (
  STRING_MAP *Head,
  UINTN      Index
  )
{
  UINTN    Pos;
  LIST_ENTRY *Link;
  STRING_MAP     *Item;

  Pos  = 0;
  Link = GetFirstNode ((LIST_ENTRY *)Head);

  while (!IsNull ((LIST_ENTRY *)Head, Link)) {
    Item = (STRING_MAP *)Link;
    if (Pos++ == Index) {
      return Item;
    }

    Link = GetNextNode ((LIST_ENTRY *)Head, Link);
  }

  return NULL;
}

UINTN
StringMapGetSize (
  STRING_MAP *Head
  )
{
  UINTN    Count;
  LIST_ENTRY *Link;

  Count = 0;

  Link = GetFirstNode ((LIST_ENTRY *)Head);

  while (!IsNull ((LIST_ENTRY *)Head, Link)) {
    Count++;
    Link = GetNextNode ((LIST_ENTRY *)Head, Link);
  }

  return Count;
}

