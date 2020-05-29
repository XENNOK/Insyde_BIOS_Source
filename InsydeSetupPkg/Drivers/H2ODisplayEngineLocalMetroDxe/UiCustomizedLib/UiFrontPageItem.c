/** @file
  UI Common Controls

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UiCustomizedControls.h"
#include "H2ODisplayEngineLocalMetro.h"
#include "MetroUi.h"

STATIC UI_FRONT_PAGE_ITEM_CLASS    *mFrontPageItemClass = NULL;
#define CURRENT_CLASS              mFrontPageItemClass

UI_CONTROL *
CreateFrontPageItemChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *ItemLayout;
  UI_CONTROL                    *ImageLayout;
  UI_CONTROL                    *ImageLayoutLeft;
  UI_CONTROL                    *FrontPageImage;
  UI_CONTROL                    *menu_prompt;
  UI_CONTROL                    *DummyLabel1;
  UI_CONTROL                    *separator;
  UI_CONTROL                    *DummyLabel2;
  UI_CONTROL                    *menu_help;


  ItemLayout = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, ItemLayout);
  UiApplyAttributeList (ItemLayout, L"name='ItemLayout' padding='15,20,15,20'");

  ImageLayout = CreateControl (L"HorizontalLayout", ItemLayout);
  CONTROL_CLASS(ItemLayout)->AddChild (ItemLayout, ImageLayout);
  UiApplyAttributeList (ImageLayout, L"name='ImageLayout'");

  ImageLayoutLeft = CreateControl (L"Label", ImageLayout);
  CONTROL_CLASS(ImageLayout)->AddChild (ImageLayout, ImageLayoutLeft);
  UiApplyAttributeList (ImageLayoutLeft, L"name='ImageLayoutLeft'");

  FrontPageImage = CreateControl (L"Texture", ImageLayout);
  CONTROL_CLASS(ImageLayout)->AddChild (ImageLayout, FrontPageImage);
  UiApplyAttributeList (FrontPageImage, L"bkimagestyle='stretch|light' name='FrontPageImage'");

  menu_prompt = CreateControl (L"Label", ItemLayout);
  CONTROL_CLASS(ItemLayout)->AddChild (ItemLayout, menu_prompt);
  UiApplyAttributeList (menu_prompt, L"align='center' fontsize='15' height='23' name='menu_prompt' padding='8,0,0,0' textcolor='0xFFFFFFFF'");

  DummyLabel1 = CreateControl (L"Label", ItemLayout);
  CONTROL_CLASS(ItemLayout)->AddChild (ItemLayout, DummyLabel1);
  UiApplyAttributeList (DummyLabel1, L"height='7' name='DummyLabel1'");

  separator = CreateControl (L"Texture", ItemLayout);
  CONTROL_CLASS(ItemLayout)->AddChild (ItemLayout, separator);
  UiApplyAttributeList (separator, L"bkcolor='0xFFF4F4F4' height='1' name='separator'");

  DummyLabel2 = CreateControl (L"Label", ItemLayout);
  CONTROL_CLASS(ItemLayout)->AddChild (ItemLayout, DummyLabel2);
  UiApplyAttributeList (DummyLabel2, L"height='8' name='DummyLabel2'");

  menu_help = CreateControl (L"Label", ItemLayout);
  CONTROL_CLASS(ItemLayout)->AddChild (ItemLayout, menu_help);
  UiApplyAttributeList (menu_help, L"align='center' fontsize='13' height='26' name='menu_help' textcolor='0xFFFFFFFF'");

  return ItemLayout;
}

#define PROMPT_HEIGHT       23
#define SEPARATOR_HEIGHT    16
#define HELP_HEIGHT         26


VOID
UpdateFrontPageItem (
  UI_CONTROL                    *Control,
  CONST RECT                    *Rc
  )
{
  UI_FRONT_PAGE_ITEM        *This;
  H2O_FORM_BROWSER_S        *Statement;
  CHAR16                    Str[20];
  UI_CONTROL                *Child;
  INT32                     Width;
  INT32                     Height;
  INT32                     ImageSize;

  Width  = Rc->right   -  Rc->left;
  Height = Rc->bottom -  Rc->top;
  ASSERT (Width > HELP_HEIGHT + SEPARATOR_HEIGHT + PROMPT_HEIGHT + 10 + 40);
  ASSERT (Height > HELP_HEIGHT + SEPARATOR_HEIGHT + PROMPT_HEIGHT + 10 + 30);
  Width -= 40;
  Height -= 30;

  ImageSize = Height - HELP_HEIGHT - SEPARATOR_HEIGHT - PROMPT_HEIGHT;
  ImageSize = MIN (Width / 2, ImageSize);

  This      = (UI_FRONT_PAGE_ITEM *) Control;
  Statement = This->Statement;
  ASSERT (Statement != NULL);
  if (Statement == NULL) {
    return;
  }

  if (Statement->Image != NULL) {
    Child = UiFindChildByName (This, L"FrontPageImage");
    UnicodeSPrint (Str, sizeof (Str), L"0x%x", Statement->Image);
    UiSetAttribute (Child, L"bkimage", Str);
    UnicodeSPrint (Str, sizeof (Str), L"%d", ImageSize);
    UiSetAttribute (Child, L"width", Str);
    Child = UiFindChildByName (This, L"ImageLayout");
    UiSetAttribute (Child, L"height", Str);
    Child = UiFindChildByName (This, L"ImageLayoutLeft");
    UnicodeSPrint (Str, sizeof (Str), L"%d", (Width - ImageSize) / 2);
    UiSetAttribute (Child, L"width", Str);
  }

  if (Statement->Prompt != NULL) {
    Child = UiFindChildByName (This, L"menu_prompt");
    UiSetAttribute (Child, L"text", Statement->Prompt);
  }
  if (Statement->Help != NULL) {
    Child = UiFindChildByName (This, L"menu_help");
    UiSetAttribute (Child, L"text", Statement->Help);
  }
}

STATIC
COLORREF
GetImageColor (
  IN  EFI_IMAGE_INPUT                *ImageIn
  )
{
  UINTN                           Index;
  UINTN                           BufferLen;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Pixel;

  ASSERT (ImageIn != NULL && ImageIn->Bitmap != NULL);

  BufferLen = ImageIn->Width * ImageIn->Height;

  for (Index = 0; Index < BufferLen; Index++) {
    if (ImageIn->Bitmap[Index].Reserved != 0) {
      Pixel = &ImageIn->Bitmap[Index];
      return (0xFF000000 | (Pixel->Red << 16) | (Pixel->Green << 8) | Pixel->Blue);
    }
  }
  return 0;
}

STATIC
VOID
UpdateFrontPageItemColor (
  IN UI_FRONT_PAGE_ITEM    *FrontPageItem,
  IN BOOLEAN               MenuColor
  )
{
  CHAR16                  TextColor[20];
  H2O_FORM_BROWSER_S      *Statement;
  COLORREF                ImageColor;
  UI_CONTROL              *Child;

  Child = UiFindChildByName (FrontPageItem, L"ItemLayout");
  ImageColor = 0;
  Statement  = FrontPageItem->Statement;
  if (Statement->Image != NULL) {
    ImageColor = GetImageColor (Statement->Image);
  }
  if (MenuColor) {
    UnicodeSPrint (TextColor, sizeof (TextColor), L"0x%x", ImageColor);
    UiSetAttribute (Child, L"bkcolor", TextColor);
  } else {
    UiSetAttribute (Child, L"bkcolor", L"0x0");
  }
}

VOID
UiFrontPageItemOnSetState (
  UI_CONTROL                    *Control,
  UI_STATE                      SetState,
  UI_STATE                      ClearState
  )
{
  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetState (Control, SetState, ClearState);

  if (!((SetState & UISTATE_HOVER) || (ClearState & UISTATE_HOVER))) {
    return ;
  }

  UpdateFrontPageItemColor ((UI_FRONT_PAGE_ITEM *)Control, (SetState & UISTATE_HOVER) ? TRUE : FALSE);
}



VOID
EFIAPI
UiFrontPageItemSetPosition (
  UI_CONTROL                    *Control,
  CONST RECT                    *Pos
  )
{
  UpdateFrontPageItem (Control, Pos);
  PARENT_CONTROL_CLASS(CURRENT_CLASS)->SetPosition (Control,Pos);
}


BOOLEAN
EFIAPI
UiFrontPageItemSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_FRONT_PAGE_ITEM  *This;
  EFI_STATUS          Status;

  This = (UI_FRONT_PAGE_ITEM *) Control;

  if (StrCmp (Name, L"statement") == 0) {
    This->Statement = (H2O_FORM_BROWSER_S *)(UINTN) StrToUInt (Value, 16, &Status);
  } else {
    return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);
  }

  CONTROL_CLASS_INVALIDATE (This);

  return TRUE;
}


LRESULT
EFIAPI
UiFrontPageItemProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_FRONT_PAGE_ITEM    *This;
  UI_CONTROL            *Control;
  UI_MANAGER            *Manager;

  Manager = NULL;

  This = (UI_FRONT_PAGE_ITEM *) GetWindowLongPtr (Hwnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_FRONT_PAGE_ITEM *) AllocateZeroPool (sizeof (UI_FRONT_PAGE_ITEM));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    Manager = Control->Manager;
    CreateFrontPageItemChilds (Control);
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_BKCOLOR || WParam == PAINT_STATUSIMAGE) {
      break;
    }
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

    if (WParam == PAINT_ALL &&
        (CONTROL_CLASS_GET_STATE (This) & UISTATE_FOCUSED)) {
      COLORREF                  Color;
      UI_MANAGER                *Manager;
      RECT                      Rc;
      HDC                       Hdc;

      Color = 0xFFFF0000;
      Manager = Control->Manager;
      Hdc     = Manager->PaintDC;

      Manager->GetControlRect (Manager, Control, &Rc);

      SelectObject (Hdc, GetStockObject (PS_NULL));
      SelectObject (Hdc, GetStockObject (DC_PEN));
      SetDCPenColor (Hdc, Color);
      Rectangle (Hdc, Rc.left, Rc.top, Rc.right, Rc.bottom);
    }
    break;

  case WM_NCHITTEST:
    return HTTRANSPARENT;
    break;

  case WM_DESTROY:
    KillTimer (Hwnd, 0);
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_FRONT_PAGE_ITEM_CLASS *
EFIAPI
GetFrontPageItemClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"FrontPageItem", (UI_CONTROL_CLASS *) GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiFrontPageItemProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiFrontPageItemSetAttribute;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetPosition  = UiFrontPageItemSetPosition;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetState     = UiFrontPageItemOnSetState;

  return CURRENT_CLASS;
}


