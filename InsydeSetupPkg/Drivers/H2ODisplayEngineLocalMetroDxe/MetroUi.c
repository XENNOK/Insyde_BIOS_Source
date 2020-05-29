/** @file
  Entry point and initial functions for H2O local Metro display engine driver

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "H2ODisplayEngineLocalMetro.h"
#include "UiControls.h"
#include "UiCustomizedControls.h"
#include "H2OPanels.h"
#include "MetroUi.h"

extern H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA      *mMetroPrivate;
extern HWND                                       gLastFocus;


typedef struct _DISPLAY_ENGINE_FRAME DISPLAY_ENGINE_FRAME;

struct _DISPLAY_ENGINE_FRAME {
  UI_FRAME                      Frame;

  UI_CONTROL                    *H2OSetupMenuPanel;
  UI_CONTROL                    *H2OSetupPagePanel;
  UI_CONTROL                    *H2OFormTitlePanel;
  UI_CONTROL                    *H2OHelpTextPanel;
  UI_CONTROL                    *H2OHotkeyPanel;
  UI_CONTROL                    *H2OOwnerDrawPanel;
};

typedef struct _PANEL_VFCF_DATA {
  CHAR16                        *PanelName;
  UINT32                        PanelType;
} PANEL_VFCF_DATA;

PANEL_VFCF_DATA                 mPanelVfcfData[] = {
                                  {L"H2OOwnerDrawPanel", H2O_PANEL_TYPE_OWNER_DRAW},
                                  {L"H2OSetupMenuPanel", H2O_PANEL_TYPE_SETUP_MENU},
                                  {L"H2OFormTitlePanel", H2O_PANEL_TYPE_FORM_TITLE},
                                  {L"H2OSetupPagePanel", H2O_PANEL_TYPE_SETUP_PAGE},
                                  {L"H2OHelpTextPanel" , H2O_PANEL_TYPE_HELP_TEXT},
                                  {L"H2OHotkeyPanel"   , H2O_PANEL_TYPE_HOTKEY},
                                  {NULL, 0},
                                  };

typedef struct _DISPLAY_ENGINE_FRAME_CLASS {
  UI_FRAME_CLASS                ParentClass;
} DISPLAY_ENGINE_FRAME_CLASS;

STATIC DISPLAY_ENGINE_FRAME_CLASS    *mDisplayEngineFrameClass = NULL;
#define CURRENT_CLASS                mDisplayEngineFrameClass

DISPLAY_ENGINE_FRAME_CLASS *
EFIAPI
GetDisplayEngineFrameClass (
  VOID
  );

UI_CONTROL *
CreateDialogPanelChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *DialogPanel;


  DialogPanel = CreateControl (L"VerticalLayout", Root);
  CONTROL_CLASS(Root)->AddChild (Root, DialogPanel);
  UiApplyAttributeList (DialogPanel, L"bkcolor='0xFF000000' name='DialogPanel'");

  return DialogPanel;
}

UI_CONTROL *
CreateFrontPageChilds (
  UI_CONTROL *Root
  )
{
  UI_CONTROL                    *background;
  UI_CONTROL                    *H2OOwnerDrawPanel;
  UI_CONTROL                    *H2OSetupMenuPanel;
  UI_CONTROL                    *H2OFormTitlePanel;
  UI_CONTROL                    *H2OSetupPagePanel;
  UI_CONTROL                    *H2OHelpTextPanel;
  UI_CONTROL                    *H2OHotkeyPanel;
  UI_CONTROL                    *FormHalo;
  UI_CONTROL                    *overlay;


  background = CreateControl (L"Control", Root);
  CONTROL_CLASS(Root)->AddChild (Root, background);
  UiApplyAttributeList (background, L"bkcolor='0xFFE8E8E8' name='background'");

  H2OOwnerDrawPanel = CreateControl (L"H2OOwnerDrawPanel", background);
  CONTROL_CLASS(background)->AddChild (background, H2OOwnerDrawPanel);
  UiApplyAttributeList (H2OOwnerDrawPanel, L"RefreshInterval='400' name='H2OOwnerDrawPanel'");

  H2OSetupMenuPanel = CreateControl (L"H2OSetupMenuPanel", background);
  CONTROL_CLASS(background)->AddChild (background, H2OSetupMenuPanel);
  UiApplyAttributeList (H2OSetupMenuPanel, L"bkcolor='0xFF000000' name='H2OSetupMenuPanel'");

  H2OFormTitlePanel = CreateControl (L"H2OFormTitlePanel", background);
  CONTROL_CLASS(background)->AddChild (background, H2OFormTitlePanel);
  UiApplyAttributeList (H2OFormTitlePanel, L"bordercolor='0xff123456' name='H2OFormTitlePanel'");

  H2OSetupPagePanel = CreateControl (L"H2OSetupPagePanel", background);
  CONTROL_CLASS(background)->AddChild (background, H2OSetupPagePanel);
  UiApplyAttributeList (H2OSetupPagePanel, L"name='H2OSetupPagePanel'");

  H2OHelpTextPanel = CreateControl (L"H2OHelpTextPanel", background);
  CONTROL_CLASS(background)->AddChild (background, H2OHelpTextPanel);
  UiApplyAttributeList (H2OHelpTextPanel, L"bordercolor='0xff123456' name='H2OHelpTextPanel'");

  H2OHotkeyPanel = CreateControl (L"H2OHotkeyPanel", background);
  CONTROL_CLASS(background)->AddChild (background, H2OHotkeyPanel);
  UiApplyAttributeList (H2OHotkeyPanel, L"bordercolor='0xff123456' name='H2OHotkeyPanel'");

  FormHalo = CreateControl (L"Texture", background);
  CONTROL_CLASS(background)->AddChild (background, FormHalo);
  UiApplyAttributeList (FormHalo, L"bkimage='@FormHalo' float='true' name='FormHalo' scale9grid='23,26,22,31'");

  overlay = CreateControl (L"Texture", background);
  CONTROL_CLASS(background)->AddChild (background, overlay);
  UiApplyAttributeList (overlay, L"bkcolor='0x80C8C8C8' float='true' height='-1' name='overlay' visible='false' width='-1'");

  return background;
}


EFI_STATUS
GetInformationField (
  OUT RECT                                    *InfoField
  )
{
  EFI_STATUS                                  Status;
  RECT                                        SetupPagePanelField;
  RECT                                        HelpTextPanelField;

  Status = GetRectByName (gWnd, L"H2OSetupPagePanel", &SetupPagePanelField);
  if (EFI_ERROR (Status)) {
    SetRectEmpty (&SetupPagePanelField);
  }

  Status = GetRectByName (gWnd, L"H2OHelpTextPanel", &HelpTextPanelField);
  if (EFI_ERROR (Status)) {
    SetRectEmpty (&HelpTextPanelField);
  }

  if (SetupPagePanelField.top    == HelpTextPanelField.top &&
      SetupPagePanelField.bottom == HelpTextPanelField.bottom) {
    if (SetupPagePanelField.right == HelpTextPanelField.left ||
        SetupPagePanelField.left  == HelpTextPanelField.right) {
      UnionRect (InfoField, &SetupPagePanelField, &HelpTextPanelField);
      return EFI_SUCCESS;
    }
  } else if (SetupPagePanelField.left  == HelpTextPanelField.left &&
             SetupPagePanelField.right == HelpTextPanelField.right) {
    if (SetupPagePanelField.bottom == HelpTextPanelField.top ||
        SetupPagePanelField.top    == HelpTextPanelField.bottom) {
      UnionRect (InfoField, &SetupPagePanelField, &HelpTextPanelField);
      return EFI_SUCCESS;
    }
  }

  CopyRect (InfoField, &SetupPagePanelField);
  return EFI_SUCCESS;
}

EFI_STATUS
DisableHalo (
  IN DISPLAY_ENGINE_FRAME                    *This
  )
{
  UI_MANAGER                               *Manager;
  UI_CONTROL                               *Control;

  Manager = ((UI_CONTROL *) This)->Manager;
  Control = Manager->FindControlByName (Manager, L"FormHalo");
  if (Control == NULL) {
    return EFI_NOT_FOUND;
  }
  UiSetAttribute (Control, L"visible", L"false");

  return EFI_SUCCESS;
}

EFI_STATUS
SetPanelPosFromVfcf (
  IN DISPLAY_ENGINE_FRAME       *This
  )
{
  EFI_STATUS                    Status;
  H2O_LAYOUT_INFO               *LayoutInfo;
  RECT                          ScreenField;
  RECT                          PanelField;
  UI_MANAGER                    *Manager;
  UI_CONTROL                    *Control;
  CHAR16                        String[20];
  PANEL_VFCF_DATA               *PanelVfcfDataPtr;
  H2O_PANEL_INFO                *PanelInfo;
  BOOLEAN                       IsVisible;

  Manager = ((UI_CONTROL *) This)->Manager;

  if (NeedShowSetupMenu ()) {
    Status = GetLayoutById (PcdGet32 (PcdLayoutIdLgde), &LayoutInfo);
  } else if (IsFrontPage ()) {
    Status = GetLayoutById (PcdGet32 (PcdLayoutIdLgdeFrontPage), &LayoutInfo);
    DisableHalo (This);
  } else {
    Status = GetLayoutById (PcdGet32 (PcdLayoutIdLgdeCommonPage), &LayoutInfo);
  }
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ScreenField.left   = 0;
  ScreenField.top    = 0;
  ScreenField.right  = (INT32) PcdGet32 (PcdDisplayEngineLocalGraphicsHorizontalResolution);
  ScreenField.bottom = (INT32) PcdGet32 (PcdDisplayEngineLocalGraphicsVerticalResolution);

  PanelVfcfDataPtr = mPanelVfcfData;
  while (PanelVfcfDataPtr->PanelName != NULL) {
    IsVisible = FALSE;
    SetRectEmpty (&PanelField);

    PanelInfo = GetPannelInfo (LayoutInfo, PanelVfcfDataPtr->PanelType);
    if (PanelInfo != NULL) {
      GetPanelField (PanelInfo, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL, &ScreenField, &PanelField);
      IsVisible = IsVisibility (PanelInfo, H2O_IFR_STYLE_TYPE_PANEL, H2O_STYLE_PSEUDO_CLASS_NORMAL);
    }

    Control = Manager->FindControlByName (Manager, PanelVfcfDataPtr->PanelName);
    UnicodeSPrint (String, sizeof (String), L"%d,%d,%d,%d", PanelField.left, PanelField.top, PanelField.right, PanelField.bottom);
    UiSetAttribute (Control, L"pos", String);
    UiSetAttribute (Control, L"visible", IsVisible ? L"true" : L"false");

    PanelVfcfDataPtr++;
  }

  return Status;
}

VOID
LocalMetroSetComponent (
  DISPLAY_ENGINE_FRAME            *This
  )
{
  UI_MANAGER  *Manager;

  Manager = ((UI_CONTROL *) This)->Manager;

  This->H2OSetupMenuPanel = Manager->FindControlByName (Manager, L"H2OSetupMenuPanel");
  This->H2OSetupPagePanel = Manager->FindControlByName (Manager, L"H2OSetupPagePanel");
  This->H2OFormTitlePanel = Manager->FindControlByName (Manager, L"H2OFormTitlePanel");
  This->H2OHelpTextPanel  = Manager->FindControlByName (Manager, L"H2OHelpTextPanel");
  This->H2OHotkeyPanel    = Manager->FindControlByName (Manager, L"H2OHotkeyPanel");
  This->H2OOwnerDrawPanel = Manager->FindControlByName (Manager, L"H2OOwnerDrawPanel");
}

VOID
LocalMetroProcessUiNotify (
  DISPLAY_ENGINE_FRAME            *This,
  UINT                          Msg,
  UI_CONTROL                    *Sender
  )
{
  H2O_STATEMENT_ID              StatementId;
  H2O_FORM_BROWSER_S            *Statement;
  HOT_KEY_ACTION                HotkeyAction;
  EFI_STATUS                    Status;
  EFI_HII_VALUE                 HiiValue;

  switch (Msg) {

  case UI_NOTIFY_CLICK:
  case UI_NOTIFY_CARRIAGE_RETURN:
    if (StrnCmp (Sender->Name, L"Hotkey", StrLen (L"Hotkey")) == 0) {
      HotkeyAction = (HOT_KEY_ACTION) GetWindowLongPtr (Sender->Wnd, GWLP_USERDATA);
      HotKeyFunc (HotkeyAction);
      break;
    }

    if (StrCmp (Sender->Name, L"CheckBox") == 0) {
      StatementId = (H2O_PAGE_ID) GetWindowLongPtr (Sender->Wnd, GWLP_USERDATA);
      Status = gFB->GetSInfo (gFB, gFB->CurrentP->PageId, StatementId, &Statement);
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        break;
      }
      SendSelectQNotify (Statement->PageId, Statement->QuestionId, Statement->IfrOpCode);
      CopyMem (&HiiValue, &Statement->HiiValue, sizeof (EFI_HII_VALUE));
      HiiValue.Value.b = !HiiValue.Value.b;
      SendChangeQNotify (Statement->PageId, Statement->QuestionId, &HiiValue);
      break;
    }
    DEBUG ((EFI_D_INFO, "Unsuppoert item click: %s", Sender->Name));
    ASSERT (FALSE);
    break;
  }
}

STATIC
VOID
UpdateHaloColor (
  IN DISPLAY_ENGINE_FRAME            *This
  )
{
  UI_CONTROL                    *Control;
  HSV_VALUE                     MenuHsv;
  HSV_VALUE                     HaloHsv;
  INT16                         HueDiff;
  INT8                          SaturationDiff;
  INT8                          ValueDiff;
  CHAR16                        Str[20];

  ASSERT (This != NULL);
  GetCurrentHaloHsv (&HaloHsv);
  GetCurrentMenuHsv (&MenuHsv);
  if (MenuHsv.Hue == 0 && MenuHsv.Saturation == 0 && MenuHsv.Value == 0) {
    return;
  }
  HueDiff        = (INT16) (MenuHsv.Hue - HaloHsv.Hue);
  SaturationDiff = (INT8)  (MenuHsv.Saturation - HaloHsv.Saturation);
  ValueDiff      = (INT8)  (MenuHsv.Value - HaloHsv.Value);
  UnicodeSPrint (Str, sizeof (Str), L"%d,%d,%d",  HueDiff, SaturationDiff, ValueDiff);
  Control = UiFindChildByName (This, L"FormHalo");
  UiSetAttribute (Control, L"hsvadjust", Str);
}

EFI_STATUS
UpdateHaloPos (
  IN DISPLAY_ENGINE_FRAME                    *This
  )
{
  EFI_STATUS                               Status;
  RECT                                     Field;
  UI_MANAGER                               *Manager;
  UI_CONTROL                               *Control;
  CHAR16                                   String[20];

  Status = GetInformationField (&Field);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Manager = ((UI_CONTROL *) This)->Manager;

  Control = Manager->FindControlByName (Manager, L"FormHalo");
  if (Control == NULL) {
    return EFI_NOT_FOUND;
  }

  UnicodeSPrint (String, sizeof (String), L"%d,%d,%d,%d", Field.left, Field.top, Field.right, Field.bottom);
  UiSetAttribute (Control, L"pos", String);

  return EFI_SUCCESS;
}

VOID
LocalMetroRegisterHotkey (
  HWND                          Wnd
  )
{
  //
  // Register Hot key
  //
  UnregisterHotKey (Wnd, 1);
//    UnregisterHotKey (Wnd, 2TURN);
  UnregisterHotKey (Wnd, 3);
  UnregisterHotKey (Wnd, 6);
  UnregisterHotKey (Wnd, 7);

  MwRegisterHotKey (Wnd, 1,  0, VK_ESCAPE);
//    RegisterHotKey (Wnd, 2,  0, VK_RETURN);

  MwRegisterHotKey (Wnd, 3,  0, VK_F1);
  MwRegisterHotKey (Wnd, 6,  0, VK_F9);
  MwRegisterHotKey (Wnd, 7,  0, VK_F10);
}

LRESULT
CALLBACK
DisplayEngineProc (
  HWND Wnd,
  UINT Msg,
  WPARAM WParam,
  LPARAM LParam
  )
{
  DISPLAY_ENGINE_FRAME            *This;
  UI_MANAGER                    *Manager;
  UI_CONTROL                    *Control;

  This    = (DISPLAY_ENGINE_FRAME *) GetWindowLongPtr (Wnd, 0);
  if (This == NULL && Msg != WM_CREATE && Msg != WM_NCCALCSIZE) {
    ASSERT (FALSE);
    return 0;
  }
  Control = (UI_CONTROL *) This;

  switch ( Msg ) {

  case WM_CREATE:
    This = (DISPLAY_ENGINE_FRAME *) AllocateZeroPool (sizeof (DISPLAY_ENGINE_FRAME));
    if (This != NULL) {
      CONTROL_CLASS(This) = (UI_CONTROL_CLASS *) GetClassLongPtr (Wnd, 0);
      SetWindowLongPtr (Wnd, 0, (INTN)This);
      SendMessage (Wnd, UI_NOTIFY_CREATE, WParam, LParam);
    }
    break;

  case UI_NOTIFY_CREATE:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam,LParam);
    //
    // create UiContainer as root
    //
    Manager = Control->Manager;
    SetWindowLongPtr (Wnd, GWL_EXSTYLE, GetWindowLongPtr (Control->Wnd, GWL_EXSTYLE) & (~WS_EX_NOACTIVATE));
    if (gFB->CurrentP != NULL) {
      CreateFrontPageChilds (Control);
      LocalMetroSetComponent (This);
      SetPanelPosFromVfcf (This);
    } else {
      CreateDialogPanelChilds (Control);
    }
    break;

  case WM_SETFOCUS:
    if (gFB->CurrentP == NULL) {
      break;
    }
    if (gLastFocus != NULL) {
      if (gLastFocus == This->H2OSetupPagePanel->Wnd) {
        SetFocus (gLastFocus);
        break;
      }
    }
    if (IsWindowVisible(This->H2OSetupMenuPanel->Wnd)) {
      SetFocus (This->H2OSetupMenuPanel->Wnd);
    } else {
      SetFocus (This->H2OSetupPagePanel->Wnd);
    }
    break;

  case WM_HOTKEY:
    if (GetWindowLongPtr (Wnd, GWL_STYLE) & WS_DISABLED) {
      break;
    }

    switch (HIWORD(LParam)) {

    case VK_RETURN:
      HotKeyFunc (HotKeyEnter);
      break;

    case VK_ESCAPE:
      HotKeyFunc (HotKeyDiscardExit);
      break;

    case VK_F1:
      HotKeyFunc (HotkeyShowHelpMsg);
      break;

    case VK_F9:
      HotKeyFunc (HotKeyLoadDefault);
      break;

    case VK_F10:
      HotKeyFunc (HotKeySaveAndExit);
      break;
    }
    break;

  case FB_NOTIFY_SELECT_Q:
    if (gFB->CurrentP != NULL) {
      This->H2OSetupMenuPanel->Class->WndProc (This->H2OSetupMenuPanel->Wnd, Msg, WParam, LParam);
      This->H2OSetupPagePanel->Class->WndProc (This->H2OSetupPagePanel->Wnd, Msg, WParam, LParam);
      This->H2OFormTitlePanel->Class->WndProc (This->H2OFormTitlePanel->Wnd, Msg, WParam, LParam);
      This->H2OHelpTextPanel->Class->WndProc (This->H2OHelpTextPanel->Wnd, Msg, WParam, LParam);
      This->H2OHotkeyPanel->Class->WndProc (This->H2OHotkeyPanel->Wnd, Msg, WParam, LParam);
      This->H2OOwnerDrawPanel->Class->WndProc (This->H2OOwnerDrawPanel->Wnd, Msg, WParam, LParam);
    }
    break;

  case FB_NOTIFY_REPAINT:
    if (gFB->CurrentP != NULL) {
      This->H2OSetupMenuPanel->Class->WndProc (This->H2OSetupMenuPanel->Wnd, Msg, WParam, LParam);
      This->H2OSetupPagePanel->Class->WndProc (This->H2OSetupPagePanel->Wnd, Msg, WParam, LParam);
      This->H2OFormTitlePanel->Class->WndProc (This->H2OFormTitlePanel->Wnd, Msg, WParam, LParam);
      This->H2OHelpTextPanel->Class->WndProc (This->H2OHelpTextPanel->Wnd, Msg, WParam, LParam);
      This->H2OHotkeyPanel->Class->WndProc (This->H2OHotkeyPanel->Wnd, Msg, WParam, LParam);
      This->H2OOwnerDrawPanel->Class->WndProc (This->H2OOwnerDrawPanel->Wnd, Msg, WParam, LParam);
      UpdateHaloPos (This);
      UpdateHaloColor (This);
      LocalMetroRegisterHotkey (Wnd);

    }
    break;

  case UI_NOTIFY_CLICK:
  case UI_NOTIFY_CARRIAGE_RETURN:
    LocalMetroProcessUiNotify (This, Msg, (UI_CONTROL *)WParam);
    break;

  case WM_DESTROY:
    PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam,LParam);
    break;

  case WM_CLOSE:
    DestroyWindow (Wnd);
    break;

  default:
    return PARENT_CLASS_WNDPROC (CURRENT_CLASS, Wnd, Msg, WParam,LParam);
  }
  return 0;
}

DISPLAY_ENGINE_FRAME_CLASS *
GetDisplayEngineFrameClass (
  VOID
  )
{

  InitUiClass ((UI_CONTROL_CLASS **)&CURRENT_CLASS, sizeof (*CURRENT_CLASS), DISPLAY_ENGINE_CLASS_NAME, (UI_CONTROL_CLASS *)GetFrameClass());
  if (CURRENT_CLASS == NULL) {
    return NULL;
  }
  ((UI_CONTROL_CLASS *)CURRENT_CLASS)->WndProc = DisplayEngineProc;

  return CURRENT_CLASS;
}



UI_GET_CLASS mGetClassTable[] = {
               (UI_GET_CLASS) GetControlClass          ,
               (UI_GET_CLASS) GetTextureClass          ,

               (UI_GET_CLASS) GetLabelClass            ,
               (UI_GET_CLASS) GetButtonClass           ,
               (UI_GET_CLASS) GetSwitchClass           ,

               (UI_GET_CLASS) GetEditClass             ,
               (UI_GET_CLASS) GetScrollBarClass        ,
               (UI_GET_CLASS) GetVerticalLayoutClass   ,
               (UI_GET_CLASS) GetHorizontalLayoutClass ,

               (UI_GET_CLASS) GetListViewClass,
               (UI_GET_CLASS) GetNumberPickerClass     ,

               (UI_GET_CLASS) GetDialogClass           ,
               (UI_GET_CLASS) GetFrameClass            ,


               (UI_GET_CLASS) GetFrontPageItemClass    ,
               (UI_GET_CLASS) GetSetupPageItemClass    ,
               (UI_GET_CLASS) GetSetupMenuItemClass    ,
               (UI_GET_CLASS) GetDateItemClass         ,
               (UI_GET_CLASS) GetTimeItemClass         ,

               (UI_GET_CLASS) GetDisplayEngineFrameClass,

               (UI_GET_CLASS) GetH2OFormTitlePanelClass,
               (UI_GET_CLASS) GetH2OHotkeyPanelClass,
               (UI_GET_CLASS) GetH2OHelpTextPanelClass,
               (UI_GET_CLASS) GetH2OOwnerDrawPanelClass,
               (UI_GET_CLASS) GetH2OSetupMenuPanelClass,
               (UI_GET_CLASS) GetH2OSetupPagePanelClass,



               NULL,
            };


EFI_STATUS
InitializeGUI (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput
  )
{
  EFI_STATUS                    Status;

  Status = gBS->LocateProtocol (
                  &gSetupMouseProtocolGuid,
                  NULL,
                  (VOID **) &mSetupMouse
                  );
  ASSERT_EFI_ERROR (Status);

  AddHiiImagePackage ();

  GdAddGopDevice (GraphicsOutput);

  if (MwOpen() < 0) {
    return EFI_INVALID_PARAMETER;
  }

  RegisterClassTable (mGetClassTable);

  return EFI_SUCCESS;
}

EFI_STATUS
InitializeWindows (
  VOID
  )
{
  MSG                                      Msg;

  if (gWnd != NULL) {
    return EFI_ALREADY_STARTED;
  }

  //
  // CreateWindow
  //
  gWnd = CreateWindowEx (
           0,
           DISPLAY_ENGINE_CLASS_NAME,
           L"Insyde H2O",
           WS_OVERLAPPED | WS_VISIBLE,
           0, 0, GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN),
           NULL,
           NULL,
           NULL,
           NULL
           );
  //
  // Process Message
  //
  while (PeekMessage (&Msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage (&Msg);
    DispatchMessage (&Msg);
  }

  return EFI_SUCCESS;
}


