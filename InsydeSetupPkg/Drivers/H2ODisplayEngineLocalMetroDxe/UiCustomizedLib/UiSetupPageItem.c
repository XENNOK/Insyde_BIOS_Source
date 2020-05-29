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

STATIC UI_SETUP_PAGE_ITEM_CLASS    *mSetupPageItemClass = NULL;
#define CURRENT_CLASS              mSetupPageItemClass

UI_CONTROL *
CreateSubtitleChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *OptionPrompt;


  OptionPrompt = CreateControl (L"Label", Root);
  CONTROL_CLASS(Root)->AddChild (Root, OptionPrompt);
  UiApplyAttributeList (OptionPrompt, L"align='singleline' fontsize='21' height='35' padding='0,0,0,20' name='OptionPrompt' textcolor='0xFF4D4D4D' ");

  return OptionPrompt;
}

UI_CONTROL *
CreateH2OCheckBoxOpChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DummyControl1;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *DummyTexture1;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *OptionPrompt;
  UI_CONTROL                    *DummyHorizontalLayout3;
  UI_CONTROL                    *DummyControl2;
  UI_CONTROL                    *DummyHorizontalLayout4;
  UI_CONTROL                    *CheckBox;
  UI_CONTROL                    *DummyControl3;


  DummyControl1 = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyControl1);
  UiApplyAttributeList (DummyControl1, L"height='70' width='-1' name='DummyControl1'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"float='true' height='70' width='-1' name='DummyHorizontalLayout1'");

  DummyTexture1 = CreateControl (L"Texture", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyTexture1);
  UiApplyAttributeList (DummyTexture1, L"bkcolor='0xFF7D7D7D' bkimage='@OptionBkg' height='70' scale9grid='0,1,0,1' name='DummyTexture1'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"height='70' width='-1' name='DummyHorizontalLayout2'");

  OptionPrompt = CreateControl (L"Label", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, OptionPrompt);
  UiApplyAttributeList (OptionPrompt, L"bkcolor='0x0' fontsize='20' height='35' name='OptionPrompt' padding='7,150,7,56' textcolor='0xFF4D4D4D' width='300'");

  DummyHorizontalLayout3 = CreateControl (L"HorizontalLayout", DummyControl1);
  CONTROL_CLASS(DummyControl1)->AddChild (DummyControl1, DummyHorizontalLayout3);
  UiApplyAttributeList (DummyHorizontalLayout3, L"float='true' height='70' width='-1' name='DummyHorizontalLayout3'");

  DummyControl2 = CreateControl (L"Label", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyControl2);
  UiApplyAttributeList (DummyControl2, L"height='70' name='DummyControl2'");

  DummyHorizontalLayout4 = CreateControl (L"HorizontalLayout", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyHorizontalLayout4);
  UiApplyAttributeList (DummyHorizontalLayout4, L"height='70' padding='17,0,17,0' width='120' name='DummyHorizontalLayout4'");

  CheckBox = CreateControl (L"Switch", DummyHorizontalLayout4);
  CONTROL_CLASS(DummyHorizontalLayout4)->AddChild (DummyHorizontalLayout4, CheckBox);
  UiApplyAttributeList (CheckBox, L"name='CheckBox' switchcolor='@menucolor'");

  DummyControl3 = CreateControl (L"Label", DummyHorizontalLayout3);
  CONTROL_CLASS(DummyHorizontalLayout3)->AddChild (DummyHorizontalLayout3, DummyControl3);
  UiApplyAttributeList (DummyControl3, L"height='70' width='30' name='DummyControl3'");

  return DummyControl1;
}

UI_CONTROL *
CreateStatementChilds (
  UI_CONTROL *Root
  )
{

  UI_CONTROL                    *OptionBkg;
  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *OptionImagePadding;
  UI_CONTROL                    *OptionImage;
  UI_CONTROL                    *OptionLayout;
  UI_CONTROL                    *OptionPrompt;
  UI_CONTROL                    *OptionValue;
  UI_CONTROL                    *OptionEnd;


  OptionBkg = CreateControl (L"Texture", Root);
  CONTROL_CLASS(Root)->AddChild (Root, OptionBkg);
  UiApplyAttributeList (OptionBkg, L"bkimage='@OptionBkg' bkimagestyle='stretch' float='true' name='OptionBkg'");

  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"name='DummyHorizontalLayout1'");

  OptionImagePadding = CreateControl (L"Control", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, OptionImagePadding);
  UiApplyAttributeList (OptionImagePadding, L"name='OptionImagePadding' width='60'");

  OptionImage = CreateControl (L"Texture", OptionImagePadding);
  CONTROL_CLASS(OptionImagePadding)->AddChild (OptionImagePadding, OptionImage);
  UiApplyAttributeList (OptionImage, L"bkimagestyle='stretch' name='OptionImage'");

  OptionLayout = CreateControl (L"HorizontalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, OptionLayout);
  UiApplyAttributeList (OptionLayout, L"name='OptionLayout'");

  OptionPrompt = CreateControl (L"Label", OptionLayout);
  CONTROL_CLASS(OptionLayout)->AddChild (OptionLayout, OptionPrompt);
  UiApplyAttributeList (OptionPrompt, L"align='singleline' fontsize='21' name='OptionPrompt' textcolor='0xFF666666' ");

  OptionValue = CreateControl (L"Label", OptionLayout);
  CONTROL_CLASS(OptionLayout)->AddChild (OptionLayout, OptionValue);
  UiApplyAttributeList (OptionValue, L"align='singleline' fontsize='21' name='OptionValue' textcolor='0xFF666666' width='110'");

  OptionEnd = CreateControl (L"Label", OptionLayout);
  CONTROL_CLASS(OptionLayout)->AddChild (OptionLayout, OptionEnd);
  UiApplyAttributeList (OptionEnd, L"align='singleline' fontsize='21' name='OptionEnd' text='>' textcolor='0xFF666666' width='60'");

  return OptionBkg;
}

VOID
CreateH2OTextOpChilds (
  UI_CONTROL *Root
  )
{

  UI_CONTROL                    *DummyHorizontalLayout1;
  UI_CONTROL                    *DummyHorizontalLayout2;
  UI_CONTROL                    *DummyTexture1;
  UI_CONTROL                    *DummyTexture2;
  UI_CONTROL                    *OptionPrompt;
  UI_CONTROL                    *OptionValue;
  UI_CONTROL                    *DummyTexture3;


  DummyHorizontalLayout1 = CreateControl (L"HorizontalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DummyHorizontalLayout1);
  UiApplyAttributeList (DummyHorizontalLayout1, L"childpadding='2' height='wrap_content' minheight='35' name='DummyHorizontalLayout1' width='match_parent'");

  DummyHorizontalLayout2 = CreateControl (L"HorizontalLayout", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyHorizontalLayout2);
  UiApplyAttributeList (DummyHorizontalLayout2, L"childpadding='2' float='true' height='match_parent' name='DummyHorizontalLayout2' width='match_parent'");

  DummyTexture1 = CreateControl (L"Texture", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyTexture1);
  UiApplyAttributeList (DummyTexture1, L"bkimage='@SetupMenuTextOpBkg' height='match_parent' name='OptionPromptBackground' scale9grid='1,1,1,1' width='258'");

  DummyTexture2 = CreateControl (L"Texture", DummyHorizontalLayout2);
  CONTROL_CLASS(DummyHorizontalLayout2)->AddChild (DummyHorizontalLayout2, DummyTexture2);
  UiApplyAttributeList (DummyTexture2, L"bkimage='@SetupMenuTextOpBkg' height='match_parent' name='OptionValueBackground' scale9grid='1,1,1,1'");

  OptionPrompt = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, OptionPrompt);
  UiApplyAttributeList (OptionPrompt, L"fontsize='20' height='wrap_content' minheight='35' name='OptionPrompt' padding='7,0,7,56' textcolor='0xFF4D4D4D' width='258'");

  OptionValue = CreateControl (L"Label", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, OptionValue);
  UiApplyAttributeList (OptionValue, L"align='center' fontsize='20' height='wrap_content' minheight='35' name='OptionValue' padding='7,0,7,0' textcolor='@menucolor' width='258'");

  DummyTexture3 = CreateControl (L"Texture", DummyHorizontalLayout1);
  CONTROL_CLASS(DummyHorizontalLayout1)->AddChild (DummyHorizontalLayout1, DummyTexture3);
  UiApplyAttributeList (DummyTexture3, L"bkcolor='@menucolor' float='true' pos='30,11,38,19' name='DummyTexture3'");

}


#define  OPTION_FONT_SIZE         21
#define  OPTION_IMAGE_SIZE        25

#define  OPTION_LEFT_PADDING      60
#define  OPTION_MIN_PROMPT_WIDTH  200
#define  OPTION_VALUE_WIDTH       105
#define  OPTION_RIGHT_PADDING     30


STATIC
COLORREF
GetImageColor (
  VOID
  )
{
  EFI_IMAGE_INPUT                 *ImageIn;
  UINTN                           Index;
  UINTN                           BufferLen;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Pixel;

  ImageIn = GetCurrentFormSetImage ();
  if (ImageIn == NULL) {
    return 0;
  }

  BufferLen = ImageIn->Width * ImageIn->Height;
  for (Index = 0; Index < BufferLen; Index++) {
    if (ImageIn->Bitmap[Index].Reserved != 0) {
      Pixel = &ImageIn->Bitmap[Index];
      return (0xFF000000 | (Pixel->Red << 16) | (Pixel->Green << 8) | Pixel->Blue);
    }
  }
  return 0;
}


VOID
UpdateSetupPageItem (
  UI_CONTROL                    *Control
  )
{
  UI_SETUP_PAGE_ITEM          *This;
  H2O_FORM_BROWSER_S        *Statement;
  CHAR16                    Str[20];
  UI_CONTROL                *Child;
  UINT32                    Index;
  INTN                      Result;
  EFI_STATUS                Status;
  INT32                     Height;
  UI_CONTROL                *CheckBox;

  Height = 70;
  This      = (UI_SETUP_PAGE_ITEM *) Control;
  if (This->Statement == NULL) {
    return ;
  }

  Statement = This->Statement;
  ASSERT (Statement != NULL);
  if (Statement == NULL) {
    return;
  }

  //
  // Add Statement image if need
  //
  if (Statement->Image != NULL) {
    Child = UiFindChildByName (This, L"OptionImage");
    UnicodeSPrint (Str, sizeof (Str), L"0x%x", Statement->Image);
    UiSetAttribute (Child, L"bkimage", Str);
    UnicodeSPrint (
      Str,
      sizeof (Str),
      L"%d,%d,%d,%d",
      (Height - OPTION_IMAGE_SIZE) / 2 + (Height - OPTION_IMAGE_SIZE) % 2,
      17,
      (Height - OPTION_IMAGE_SIZE) / 2,
      18
      );
    Child = UiFindChildByName (This, L"OptionImagePadding");
    UiSetAttribute (Child, L"padding", Str);
  }

  //
  // Adjust text and text color
  //
  if (Statement->Prompt != NULL) {
    Child = UiFindChildByName (This, L"OptionPrompt");
    UiSetAttribute (Child, L"text", Statement->Prompt);
  }

  if (Statement->Operand == EFI_IFR_CHECKBOX_OP) {
    CheckBox = UiFindChildByName (This, L"CheckBox");
    if (Statement->HiiValue.Value.b) {
      UiSetAttribute (CheckBox, L"checkboxvalue", L"true");
    } else {
      UiSetAttribute (CheckBox, L"checkboxvalue", L"false");
    }
  } else if (Statement->Operand == EFI_IFR_TEXT_OP) {
    if (Statement->TextTwo != NULL && Statement->TextTwo[0] != '\0') {
      Child = UiFindChildByName (This, L"OptionValue");
      UiSetAttribute (Child, L"text", Statement->TextTwo);
    } else {
      Child = UiFindChildByName (This, L"OptionPrompt");
      UiSetAttribute (Child, L"width", L"0");

      Child = UiFindChildByName (This, L"OptionValue");
      UiSetAttribute (Child, L"visible", L"false");

      Child = UiFindChildByName (This, L"OptionPromptBackground");
      UiSetAttribute (Child, L"width", L"0");

      Child = UiFindChildByName (This, L"OptionValueBackground");
      UiSetAttribute (Child, L"visible", L"false");
    }
  } else if (Statement->NumberOfOptions != 0) {
    UnicodeSPrint (Str, sizeof (Str), L"0x%x", GetImageColor ());
    for (Index = 0; Index < Statement->NumberOfOptions; Index++) {
      Status = CompareHiiValue (&Statement->Options[Index].HiiValue, &Statement->HiiValue, &Result);
      if (!EFI_ERROR (Status) && Result == 0) {
        Child = UiFindChildByName (This, L"OptionValue");
        UiSetAttribute (Child, L"text", Statement->Options[Index].Text);
        if (Statement->Selectable) {
          UiSetAttribute (Child, L"textcolor", Str);
          Child = UiFindChildByName (This, L"OptionEnd");
          UiSetAttribute (Child, L"textcolor", Str);
        }
        break;
      }
    }
  } else if (Statement->Operand == EFI_IFR_NUMERIC_OP) {
    Child = UiFindChildByName (This, L"OptionValue");
    UnicodeSPrint (Str, sizeof (Str), L"%d", Statement->HiiValue.Value.u64);
    UiSetAttribute (Child, L"text", Str);

    if (Statement->Selectable) {
      UnicodeSPrint (Str, sizeof (Str), L"0x%x", GetImageColor ());
      UiSetAttribute (Child, L"textcolor", Str);
      Child = UiFindChildByName (This, L"OptionEnd");
      UiSetAttribute (Child, L"textcolor", Str);
    }
  } else {
    Child = UiFindChildByName (This, L"OptionValue");
    if (Child != NULL) {
      UiSetAttribute (Child, L"visible", L"false");
      Child = UiFindChildByName (This, L"OptionEnd");
      UiSetAttribute (Child, L"visible", L"false");
    }
  }
}

BOOLEAN
EFIAPI
UiSetupPageItemSetAttribute (
  UI_CONTROL *Control,
  CHAR16     *Name,
  CHAR16     *Value
  )
{
  UI_SETUP_PAGE_ITEM    *This;
  EFI_STATUS          Status;
  UI_CONTROL          *Switch;

  This = (UI_SETUP_PAGE_ITEM *) Control;

  if (StrCmp (Name, L"statement") == 0) {
    This->Statement = (H2O_FORM_BROWSER_S *)(UINTN) StrToUInt (Value, 16, &Status);
    if (This->Statement->Operand == EFI_IFR_SUBTITLE_OP) {
      CONTROL_CLASS_SET_STATE(Control, UISTATE_DISABLED, 0);
      CreateSubtitleChilds(Control);
    } else if (This->Statement->Operand == EFI_IFR_CHECKBOX_OP) {
      CreateH2OCheckBoxOpChilds (Control);
      Switch = UiFindChildByName (This, L"CheckBox");
      SetWindowLongPtr (Switch->Wnd, GWLP_USERDATA, This->Statement->StatementId);
    } else if (This->Statement->Operand == EFI_IFR_TEXT_OP) {
      CreateH2OTextOpChilds (Control);
    } else {
      switch (This->Statement->Operand) {

      case EFI_IFR_REF_OP:
      case EFI_IFR_ACTION_OP:
      case EFI_IFR_PASSWORD_OP:
      case EFI_IFR_NUMERIC_OP:
      case EFI_IFR_ONE_OF_OP:
      case EFI_IFR_TIME_OP:
      case EFI_IFR_DATE_OP:
      case EFI_IFR_ORDERED_LIST_OP:
      case EFI_IFR_RESET_BUTTON_OP:
      case EFI_IFR_STRING_OP:
        CreateStatementChilds (Control);
        UiApplyAttributeList (Control, L"minheight='70'");
        break;

      default:
        DEBUG ((EFI_D_ERROR, "Unsupported opcode to create UiOneOfOption: %d", This->Statement->Operand));
        This->Statement = NULL;
        ASSERT (FALSE);
        break;
      }
    }
    UpdateSetupPageItem (Control);
    CONTROL_CLASS_INVALIDATE (This);
    return TRUE;
  }

  return PARENT_CLASS_SET_ATTRIBUTE (CURRENT_CLASS, Control, Name, Value);

}


LRESULT
EFIAPI
UiSetupPageItemProc (
  HWND   Hwnd,
  UINT32 Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  UI_SETUP_PAGE_ITEM      *This;
  UI_CONTROL            *Control;

  This = (UI_SETUP_PAGE_ITEM *) GetWindowLongPtr (Hwnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *)This;

  switch (Msg) {

  case WM_CREATE:
    This = (UI_SETUP_PAGE_ITEM *) AllocateZeroPool (sizeof (UI_SETUP_PAGE_ITEM));
    if (This != NULL) {
      CONTROL_CLASS (This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Hwnd, 0);
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
    Control->FixedSize.cy = -2;
    Control->MinSize.cy   = 35;
    break;

  case UI_NOTIFY_PAINT:
    if (WParam == PAINT_BKCOLOR || WParam == PAINT_STATUSIMAGE) {
      break;
    }
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);

    if (WParam == PAINT_ALL &&
        (CONTROL_CLASS_GET_STATE (This) & (UISTATE_SELECTED | UISTATE_FOCUSED)) == (UISTATE_SELECTED | UISTATE_FOCUSED)) {
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

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Hwnd, Msg, WParam, LParam);
  }

  return 0;
}

UI_SETUP_PAGE_ITEM_CLASS *
EFIAPI
GetSetupPageItemClass (
  VOID
  )
{
  if (CURRENT_CLASS != NULL) {
    return CURRENT_CLASS;
  }

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), L"SetupPageItem", (UI_CONTROL_CLASS *) GetControlClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc      = UiSetupPageItemProc;
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->SetAttribute = UiSetupPageItemSetAttribute;

  return CURRENT_CLASS;
}


