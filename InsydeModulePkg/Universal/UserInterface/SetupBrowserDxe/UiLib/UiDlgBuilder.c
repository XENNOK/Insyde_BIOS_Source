/** @file
  UI Dialog Builder

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
#include "Colors.h"
#include "UiRender.h"
#include "UiControls.h"
#include "UiManager.h"

#define TAG 1
#define CONTENT 2
#define QUOTE   3

#define MAX_ATTRIBS 64
#define MAX_PARENT  64

typedef struct {
  HWND       Parents[MAX_PARENT];
  UINTN      Count;
  UI_MANAGER *Manager;
  HINSTANCE  Instance;

} DIALOG_DATA;


typedef
VOID
(EFIAPI *CONTENT_CALLBACK) (
  VOID   *Data,
  CHAR16       *Str
);

typedef
VOID
(EFIAPI *START_CALLBACK) (
  VOID    *Data,
  CHAR16       *Element,
  CHAR16       **AttrNames,
  CHAR16       **AttrValues
);


typedef
VOID
(EFIAPI *END_CALLBACK) (
  VOID    *Data,
  CHAR16  *Element
);


STATIC
VOID
ParserContent (
  CHAR16 *Str,
  CONTENT_CALLBACK ContentCb,
  VOID   *Data
)
{
  while (*Str && isspace (*Str)) {
    Str++;
  }

  if (*Str == '\0') {
    return ;
  }

  if (ContentCb != NULL) {
    (*ContentCb)(Data, Str);
  }
}


STATIC
VOID
ParseElement (
  IN OUT CHAR16     *Str,
  IN START_CALLBACK StartCb,
  IN END_CALLBACK   EndCb,
  IN OUT VOID       *Data
)
{
  CHAR16       *AttrNames[MAX_ATTRIBS];
  CHAR16       *AttrValues[MAX_ATTRIBS];
  UINTN        AttrNum;
  CHAR16       *TagName;
  BOOLEAN      IsStart;
  BOOLEAN      IsEnd;
  CHAR16       QuoteChar;

  AttrNum = 0;
  TagName = NULL;
  IsStart = FALSE;
  IsEnd   = FALSE;

  //
  // trim white space after the '<'
  //
  while (*Str != '\0' && isspace (*Str)) {
    Str++;
  }

  //
  // start tag or end tag
  //
  if (*Str == L'/') {
    Str++;
    IsEnd = TRUE;
  } else {
    IsStart = TRUE;
  }

  //
  // commonts or preprocessor
  //
  if (*Str == '\0' || *Str == L'?' || *Str == '!') {
    return ;
  }


  TagName = Str;
  while (*Str != '\0' && !isspace (*Str)) {
    Str++;
  }
  if (*Str != '\0') {
    *Str++ = L'\0';
  }

  while (!IsEnd && *Str != '\0' && AttrNum < (MAX_ATTRIBS - 1)) {

    while (*Str && isspace (*Str)) {
      Str++;
    }

    if (*Str == '\0') {
      break;
    }

    if (*Str == '/') {
      IsEnd = TRUE;
      break;
    }

    AttrNames[AttrNum] = Str++;
    AttrValues[AttrNum] = NULL;
    while (*Str != '\0' && !isspace (*Str) && *Str != '=') {
      Str++;
    }

    if (*Str != '\0') {
      *Str++ = '\0';
    }

    //
    // beginning of the value
    //
    while (*Str != '\0' && *Str != '\"' && *Str != '\'') {
      Str++;
    }

    if (*Str == '\0') {
      break;
    }

    QuoteChar = *Str;
    Str++;
    AttrValues[AttrNum++] = Str;
    while (*Str != '\0' && *Str != QuoteChar) {
      if (*Str == '\\' && *(Str + 1) == QuoteChar) {
        Str += 2;
        continue;
      }
      Str++;
    }
    if (*Str != '\0') {
      *Str++ = '\0';
    }
  }
  AttrNames[AttrNum] = NULL;
  AttrValues[AttrNum++] = NULL;

  if (IsStart && StartCb) {
    (*StartCb)(Data, TagName, AttrNames, AttrValues);
  }
  if (IsEnd && EndCb) {
    (*EndCb)(Data, TagName);
  }
}

BOOLEAN
SaxParser (
  CHAR16           *XMLBuffer,
  START_CALLBACK   StartCb,
  CONTENT_CALLBACK ContentCb,
  END_CALLBACK     EndCb,
  VOID             *Data
)
{
  CHAR16 *Str;
  CHAR16 *Mark;
  UINTN  State;
  CHAR16 QuoteChar;

  Str   = XMLBuffer;
  Mark  = Str;
  State = CONTENT;
  QuoteChar = 0;

  while (*Str != '\0') {

    if (*Str == '<' && State == CONTENT) {
      *Str++ = '\0';
      ParserContent (Mark, ContentCb, Data);
      Mark = Str;
      State = TAG;
    } else if (*Str == '\\' && State == QUOTE) {
      if (*(Str + 1) == QuoteChar) {
        Str += 2;
        continue;
      }
      Str++;
    } else if (*Str == '\"' || *Str == '\'') {
      if (State == TAG) {
        State = QUOTE;
        QuoteChar = *Str;
      } else if (State == QUOTE && *Str == QuoteChar) {
        State = TAG;
      }
      Str++;
    } else if (*Str == '>' && State == TAG) {
      *Str++ = '\0';
      ParseElement (Mark, StartCb, EndCb, Data);
      Mark = Str;
      State = CONTENT;
    } else {
      Str++;
    }
  }
  return TRUE;
}


CHAR8 TmpStr[1024];

CHAR8 *
U2A (
  CHAR16 *Str
)
{

  CHAR16 *s = (CHAR16 *)Str;
  CHAR8  *d = TmpStr;

  while ((*d++ = (CHAR8)*s++) != '\0');

  return TmpStr;
}


VOID
UiDlgBuilderContentCb (
  VOID   *Data,
  CHAR16       *Str
)
{
  DEBUG ((EFI_D_ERROR, "content: %a\n", U2A(Str)));

}


VOID
UiDlgBuilderStartCb ( VOID    *Data,
                      CHAR16 *Element,
                      CHAR16  **AttrNames,
                      CHAR16  **AttrValues)
{
  HWND   Parent;
  HWND   Control;
  UI_CONTAINER *ContainerCtrl;
  UI_CONTROL   *ControlCtrl;
  DIALOG_DATA  *DialogData;
  BOOLEAN      IsContainer;

  Control     = NULL;
  DialogData  = Data;
  IsContainer = FALSE;

  Parent = DialogData->Parents[DialogData->Count];

  if ((StrCmp (Element, L"Container") == 0) ||
      (StrCmp (Element, L"VerticalLayout") == 0) ||
      (StrCmp (Element, L"HorizontalLayout") == 0) ||
      (StrCmp (Element, L"TileLayout") == 0)) {
    IsContainer = TRUE;
  }

  Control = CreateWindowEx (WS_EX_NOACTIVATE, Element, L"", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, Parent, 0, DialogData->Instance, DialogData->Manager);

  ASSERT (Control != NULL);
  if (Control == NULL) {
    return ;
  }

  ControlCtrl = (UI_CONTROL *) GetWindowLongPtr (Control, 0);

  //
  // parent maybe isn't ui_control
  //
  ContainerCtrl = (UI_CONTAINER *) GetWindowLongPtr (Parent, 0);
  ContainerCtrl->Add (ContainerCtrl, ControlCtrl);

  if (IsContainer) {
    DEBUG ((EFI_D_ERROR, "start: %a %d\n", U2A(Element), DialogData->Count));
    DialogData->Parents[++DialogData->Count] = Control;
  }

  while (*AttrNames != NULL) {
    ControlCtrl->SetAttribute (ControlCtrl, *AttrNames, *AttrValues);

    AttrNames++;
    AttrValues++;
  }

}

VOID
UiDlgBuilderEndCb (
  VOID    *Data,
  CHAR16  *Element
)
{

  DIALOG_DATA *DialogData;

  DialogData  = Data;


  ASSERT (DialogData->Count != 0);

  DEBUG ((EFI_D_ERROR, "end: %a %d\n", U2A(Element), DialogData->Count));

  if (DialogData->Count == 0) {
    return ;
  }

  if (StrCmp (Element, L"Container") == 0 ||
      (StrCmp (Element, L"VerticalLayout") == 0) ||
      (StrCmp (Element, L"HorizontalLayout") == 0) ||
      (StrCmp (Element, L"TileLayout") == 0)) {
    DialogData->Count--;
  }
}




UI_CONTROL *
UiDlgBuilderCreate (
  CHAR16     *XMLBuffer,
  UI_MANAGER *Manager,
  HWND       Parent
)
{
  DIALOG_DATA Data;
  CHAR16      *Buf;

  ASSERT (Parent != NULL);

  Data.Count       = 0;
  Data.Parents[0]  = Parent;
  Data.Manager     = Manager;
  Data.Instance    = Manager->Instance;

  Buf = AllocateCopyPool (StrSize(XMLBuffer), XMLBuffer);

  SaxParser (
    Buf,
    UiDlgBuilderStartCb,
    UiDlgBuilderContentCb,
    UiDlgBuilderEndCb,
    &Data
  );

  gBS->FreePool (Buf);

  return NULL;
}


BOOL CALLBACK UiDialogProc ( HWND Hwnd, UINT Msg, WPARAM WParam, LPARAM LParam )
{
  UI_DIALOG       *This;
  LRESULT         Ret;
  UI_MANAGER      *Manager;
  //RECT            Rc;
  UI_CONTROL      *Control;

  Manager = NULL;
  This    = (UI_DIALOG *) (UINTN) GetWindowLongPtr (Hwnd, 0);
  Control = (UI_CONTROL *) This;

  if (This != NULL) {
    Manager = Control->Manager;

    Ret     = 0;
    if (This->Proc != NULL) {
      Ret = This->Proc (Hwnd, Msg, WParam, LParam);
    }

    if (Ret) {
      return (BOOLEAN)(Ret != 0);
    }
  }

  switch (Msg) {

    case WM_CREATE:
      This = ((CREATESTRUCT *)(LParam))->lpCreateParams;
      SetWindowLongPtr (Hwnd, 0, (INTN)This);
      SendMessage (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
      break;

    case UI_NOTIFY_CREATE:
      UiFrameProc (Hwnd, UI_NOTIFY_CREATE, WParam, LParam);
      Manager = Control->Manager;
      UiDlgBuilderCreate (This->XmlBuffer, Manager, Manager->Root);
      break;

    default:
      return (BOOLEAN)UiFrameProc (Hwnd, Msg, WParam, LParam);
  }
  return 0;
}

BOOL WINAPI
UiEndDialog(HWND hDlg, int nResult)
{
  UI_DIALOG *This;
  //PMWDLGDATA params = DLG_PMWDLGDATA(hDlg);

  This = (UI_DIALOG *) GetWindowLongPtr (hDlg, 0);

  This->Running = FALSE;
  This->Ret     = nResult;

  return TRUE;
}

HWND
FindFirstFocusableControl (
  HWND Wnd
  )
{

  HWND  ChildList[100];
  HWND  Child;
  HWND  Ret;
  INTN  Count;
  INTN  Index;

  if (!(GetWindowLongPtr (Wnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
    return Wnd;
  }

  for (Count = 0, Child = Wnd->children; Child != NULL; Child = Child->siblings) {
    ChildList[Count++] = Child;
  }

  if (Count == 0) {
    return NULL;
  }

  for (Index = Count - 1; Index >= 0; Index--) {
    Ret = FindFirstFocusableControl (ChildList[Index]);
    if (Ret != NULL) {
      return Ret;
    }
  }

  return NULL;
}


INTN
UiDialogBoxParam (
  HINSTANCE  Instance,
  CHAR16     *XmlBuffer,
  HWND       ParentWnd,
  WNDPROC    DialogProc,
  LPARAM     Param,
  INT32      X,
  INT32      Y,
  INT32      Width,
  INT32      Height,
  BOOLEAN    CloseOnTouchOutside
  )
{

  HWND      Dlg;
  UI_DIALOG    *DialogData;
  MSG       Msg;
  DWORD     ltm;
  INTN      Ret;
  RECT      DlgRc;
  POINT     Pt;
  BOOLEAN   ProcessMsg;
  UI_MANAGER      *Manager;
  UI_CONTROL      *Control;
  HWND            FirstFocusableWnd;

  DialogData = AllocateZeroPool (sizeof (UI_DIALOG));
  DialogData->Instance   = Instance;
  DialogData->ParentWnd  = ParentWnd;
  DialogData->Proc       = DialogProc;
  DialogData->Param      = Param;
  DialogData->XmlBuffer  = XmlBuffer;
  DialogData->Running    = TRUE;

  Ret = 0;
  ltm = 0;

  Dlg = CreateWindowEx (
          WS_EX_NOACTIVATE, L"DIALOG", L"", WS_VISIBLE | WS_POPUP,
          X, Y, Width, Height, ParentWnd, NULL, Instance, DialogData
          );


  Control = (UI_CONTROL *) (UINTN) GetWindowLongPtr (Dlg, 0);
  Manager = Control->Manager;

  SendMessage (Manager->MainWnd, UI_NOTIFY_WINDOWINIT, (WPARAM)Manager->Root, 0);

  //
  // find first focus
  //
  FirstFocusableWnd = FindFirstFocusableControl (Dlg);
  if (FirstFocusableWnd != NULL) {
    SetFocus (FirstFocusableWnd);
  }

  while (DialogData->Running) {
    if (!PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
      continue;
    }

    if (Msg.hwnd == ParentWnd) {
      // continue;
    }

    if (Msg.message == WM_PAINT) {
//    Control = GetUiControl (Dlg);
//    if (Msg.hwnd != Control->Manager->Root) {
//      continue;
//    }
    }

    ProcessMsg = FALSE;

    if (Msg.message == WM_NCLBUTTONDOWN) {
      //
      // check outside click
      //
      GetWindowRect (Dlg, &DlgRc);
      POINTSTOPOINT(Pt, Msg.lParam);
      if (!PtInRect (&DlgRc, Pt)) {
        if (CloseOnTouchOutside) {
          SendMessage (Dlg, WM_CLOSE, 0, 0);
          ProcessMsg = TRUE;
        }
      }
    }

    //
    // check outside message
    //
    if ((Msg.message >= WM_LBUTTONDOWN && Msg.message <= WM_MBUTTONDBLCLK) ||
        (Msg.message >= WM_NCLBUTTONDOWN && Msg.message <= WM_NCRBUTTONDBLCLK)) {
      GetWindowRect (Dlg, &DlgRc);
      GetCursorPos (&Pt);
      if (!PtInRect (&DlgRc, Pt)) {
        if (!CloseOnTouchOutside) {
          continue;
        }
      }
    }

    if (Msg.message == WM_KEYDOWN) {
      if (Msg.wParam == VK_ESCAPE) {
        SendMessage (Dlg, WM_HOTKEY, 0, MAKELPARAM(0, Msg.wParam));
        ProcessMsg = TRUE;
      }
    }


    if (!ProcessMsg) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }

    if (Msg.message == WM_QUIT) {
      Ret = Msg.lParam;
      break;
    }
  }

  if (!DialogData->Running) {
    Ret = DialogData->Ret;
    DestroyWindow(Dlg);
  }

  return Ret;
}




UINT32
RegisterUiDialog (HINSTANCE hInstance)
{
  WNDCLASS wc;

  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_GLOBALCLASS;
  wc.lpfnWndProc   = (WNDPROC) UiDialogProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = sizeof (UI_DIALOG *);
  wc.hInstance     = hInstance;
  wc.hIcon         = NULL;
  wc.hCursor       = 0;
  wc.hbrBackground = GetStockObject(NULL_BRUSH);
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = L"DIALOG";

  return RegisterClass(&wc);
}

