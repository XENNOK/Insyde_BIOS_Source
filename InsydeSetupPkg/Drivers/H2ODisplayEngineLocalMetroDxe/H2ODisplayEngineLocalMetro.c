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

#include <H2ODisplayEngineLocalMetro.h>
#include <Library/HiiLib.h>
#include <Library/Win32Lib.h>
#include <Library/TimerLib.h>
#include "MetroUi.h"
#include "UiControls.h"

EFI_PERFORMANCE_MEASURE_PROTOCOL  *mPerf = NULL;

BOOLEAN                                    mIsInNotifyProcess;
H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA      *mMetroPrivate = NULL;
H2O_FORM_BROWSER_PROTOCOL                  *gFB = NULL;
extern      H2O_WINDOW_PROTOCOL            *gH2OWindow;
EFI_ABSOLUTE_POINTER_STATE                 mPreviousAbsPtrState;

HWND                            gWnd = NULL;
HWND                            gLastFocus = NULL;
EFI_GRAPHICS_OUTPUT_PROTOCOL    *mGraphicsOutput;

#define                         SHOW_NONE               0x00000000
#define                         SHOW_REFRESH_DE_FPS     0x00000001
#define                         SHOW_GOP_BLT_FPS        0x00000002
#define                         SHOW_TIMER_FPS          0x00000004

UINT32                          mShowFps = SHOW_NONE;


/**
  Add new console to the console list.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleHandle      A pointer to the input console handle instance.

  @retval EFI_SUCCESS            Add new console to console list successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.

**/
EFI_STATUS
EFIAPI
LocalMetroInitConsole (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  )
{
  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Attach a specific console to this display engine.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleDev         A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Attach new console successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.
  @retval EFI_UNSUPPORTED        This display engine doesn't support input console device.

**/

EFI_STATUS
EFIAPI
LocalMetroAttachConsole (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  )
{
  H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA    *Private;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE        *ConDevNode;
  EFI_STATUS                               Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL             *GraphicsOut;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL          *SimpleTextOut;
  UINT32                                   Mode;
  UINTN                                    SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION     *Info;
  STATIC BOOLEAN                           FirstIn = TRUE;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  ConsoleDev->Handle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOut
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mGraphicsOutput = GraphicsOut;

  Status = gBS->HandleProtocol (
                  ConsoleDev->Handle,
                  &gEfiSimpleTextOutProtocolGuid,
                  (VOID **) &SimpleTextOut
                  );
  if (EFI_ERROR (Status)) {
    SimpleTextOut = NULL;
  }

  Private = H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA_FROM_PROTOCOL (This);

  ConDevNode = (H2O_FORM_BROWSER_CONSOLE_DEV_NODE*) AllocatePool (sizeof (H2O_FORM_BROWSER_CONSOLE_DEV_NODE));
  if (ConDevNode == NULL) {
    return EFI_UNSUPPORTED;
  }

  ConDevNode->Signature     = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_SIGNATURE;
  ConDevNode->ConsoleDev    = AllocateCopyPool (sizeof (H2O_FORM_BROWSER_CONSOLE_DEV), ConsoleDev);
  ConDevNode->GraphicsOut   = GraphicsOut;
  ConDevNode->SimpleTextOut = SimpleTextOut;

  InsertTailList (&Private->ConsoleDevListHead, &ConDevNode->Link);

  for (Mode = 0; Mode < GraphicsOut->Mode->MaxMode; Mode++) {
    Status = GraphicsOut->QueryMode (GraphicsOut, Mode, &SizeOfInfo, &Info);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if ((Info->HorizontalResolution == (UINTN) PcdGet32 (PcdDisplayEngineLocalGraphicsHorizontalResolution)) &&
        (Info->VerticalResolution   == (UINTN) PcdGet32 (PcdDisplayEngineLocalGraphicsVerticalResolution))) {
      if (GraphicsOut->Mode->Mode != Mode) {
        GraphicsOut->SetMode (GraphicsOut, Mode);
      }
      FreePool (Info);
      break;
    }
    FreePool (Info);
  }

  //
  // Initialize GUI
  //
  if (FirstIn) {
    Status = InitializeGUI (GraphicsOut);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  return EFI_SUCCESS;
}

/**
  Detach a specific console from this display engine.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleDev         A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Detach a console device from the device engine successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.
  @retval EFI_NOT_FOUND          The input device console USN't attached to the display engine.

**/
EFI_STATUS
EFIAPI
LocalMetroDetachConsole (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  )
{
  EFI_STATUS                               Status;
  H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA    *Private;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE        *ConDevNode;
  LIST_ENTRY                               *Link;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA_FROM_PROTOCOL (This);

  if (IsListEmpty (&Private->ConsoleDevListHead)) {
    return EFI_NOT_FOUND;
  }

  //
  // If call this function during processing notify function, replace fake protocol on this console device.
  // It can make sure that interrupted notify function can be executed successfully.
  // After interrupted notify function is finished, remove the console device.
  //
  if (mIsInNotifyProcess) {
//  Status = DEReplaceFakeConOutDev (Private, ConsoleDev->Handle);
//  return Status;
  }

  Status = EFI_NOT_FOUND;
  Link = Private->ConsoleDevListHead.ForwardLink;
  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);
    if (ConsoleDev->ConsoleId == ConDevNode->ConsoleDev->ConsoleId) {

      //
      // Remove Console Device Node
      //
      RemoveEntryList (&ConDevNode->Link);
      FreePool (ConDevNode->ConsoleDev);
      FreePool (ConDevNode);
      Status = EFI_SUCCESS;
      break;
    }
    //
    // Get Next Console
    //
    if (IsNodeAtEnd (&Private->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  }

  if (IsListEmpty (&Private->ConsoleDevListHead)) {
    ASSERT (gWnd != NULL);
    if (gWnd != NULL) {
      DestroyWindow (gWnd);
      gWnd = NULL;
    }
  }

  return Status;
}

VOID
ShowFps (
  HWND                          Wnd
  )
{
  STATIC HDC                    MemHdc = NULL;
  STATIC HDC                    Hdc = NULL;
  HDC                           DefaultHdc;
  STATIC UINT64                 Frames = 1;
  STATIC UINT64                 LastTime = 0;
  STATIC UINT64                 Fps = 0;
  STATIC EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Pixel = NULL;
  STATIC EFI_GRAPHICS_OUTPUT_BLT_PIXEL *Pixel2 = NULL;
  UINT64                        OneSecond;
  UINT64                        NanoSecond;
  CHAR16                        Str[200];
  RECT                          Rc;
  UINT32                        Y;

  if (!mShowFps) {
    return ;
  }

  OneSecond = 1000000000; // nanosecond
  if (MemHdc == NULL) {
    Pixel  = AllocateZeroPool (1024 * 768 * 4);
    Pixel2 = AllocateZeroPool (1024 * 768 * 4);
    DefaultHdc = GetDC(NULL);
    if (DefaultHdc == NULL) {
      return;
    }
    MemHdc = CreateCompatibleDC(DefaultHdc);
    ASSERT (MemHdc != NULL);
    if (MemHdc == NULL) {
      ReleaseDC (NULL, DefaultHdc);
      return;
    }
    SelectObject(MemHdc, CreateCompatibleBitmap(MemHdc, 300, 20));
    SetRect (&Rc, 0, 0, GetDeviceCaps (MemHdc, HORZRES), GetDeviceCaps (MemHdc, VERTRES));
    MwFastFillRect (MemHdc, &Rc, RGB(255,255,255));
    ReleaseDC (NULL, DefaultHdc);
  }

  NanoSecond = GetTimeInNanoSecond(GetPerformanceCounter());
  Frames++;

  if ((NanoSecond - LastTime) > OneSecond) {
    Fps = Frames;
    Frames = 0;
    LastTime = NanoSecond;
    DEBUG ((EFI_D_INFO, "%020ld, %04ld fps\n", NanoSecond, Fps));
    if (mPerf) {
      mPerf->IsPrintEnable = TRUE;
      mPerf->End (2, TRUE, L"FPS", L"\n");
      mPerf->Start (2, TRUE);
      mPerf->IsPrintEnable = FALSE;
    }
    MemHdc->font->FontSize = 19;

    UnicodeSPrint (Str, sizeof (Str), L"%020ld, %04ld fps", NanoSecond, Fps);
    SetRect (&Rc, 0, 0, GetDeviceCaps (MemHdc, HORZRES), GetDeviceCaps (MemHdc, VERTRES));
    MwFastFillRect (MemHdc, &Rc, RGB(255,255,255));
    TextOut (MemHdc, 0, 0, Str, -1);

    if (mShowFps & SHOW_GOP_BLT_FPS) {
      for (Y = 0; Y < (UINT32)GetDeviceCaps (MemHdc, VERTRES); Y++) {
        CopyMem (Pixel + Y * 1024, MemHdc->psd->addr + Y * GetDeviceCaps (MemHdc, HORZRES) * 4, GetDeviceCaps (MemHdc, HORZRES) * 4);
      }
    }

    if (mShowFps == SHOW_TIMER_FPS) {
      Hdc = GetDC(Wnd);
      ASSERT (Hdc != NULL);
      if (Hdc == NULL) {
        return;
      }
      BitBlt(Hdc, 0, 0, GetDeviceCaps (MemHdc, HORZRES), GetDeviceCaps (MemHdc, VERTRES), MemHdc, 0, 0, MWROP_SRC);
      ReleaseDC (Wnd, Hdc);
      return ;
    }
  }

  switch (mShowFps) {

  case SHOW_GOP_BLT_FPS:
    CopyMem (Pixel2, Pixel, 1024 * 768 * 4);
    mGraphicsOutput->Blt (
                       mGraphicsOutput,
                       Pixel2,
                       EfiBltBufferToVideo,
                       0,
                       0,
                       0,
                       0,
                       1024,
                       768,
                       0
                       );
    break;

  case SHOW_REFRESH_DE_FPS:
    CONTROL_CLASS_INVALIDATE (GetUiControl (Wnd));
    Hdc = GetDC(Wnd);
    if (Hdc != NULL) {
      BitBlt(Hdc, 0, 0, GetDeviceCaps (MemHdc, HORZRES), GetDeviceCaps (MemHdc, VERTRES), MemHdc, 0, 0, MWROP_SRC);
      ReleaseDC (Wnd, Hdc);
    }
    break;

  case SHOW_TIMER_FPS:
    // do nothing
    break;

  default:
    ASSERT (FALSE);
  }
}



/**
  Report which console devices are supported by this display engine.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleDev         A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Report supported console device type successful.
  @retval EFI_UNSUPPORTED        This display engine doesn't support input console device.
  @retval EFI_INVALID_PARAMETER  This is NULL or ConsoleDev is NULL.

**/
EFI_STATUS
EFIAPI
LocalMetroSupportConsole (
  IN     H2O_DISPLAY_ENGINE_PROTOCOL       *This,
  IN     H2O_FORM_BROWSER_CONSOLE_DEV      *ConsoleDev
  )
{
  EFI_STATUS                               Status;
  EFI_DEVICE_PATH_PROTOCOL                 *DevPath;
  EFI_GRAPHICS_OUTPUT_PROTOCOL             *ConOut;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevPath = ConsoleDev->DevicePath;
  if (DevPath->Type != 0) {
    //
    // Check Device Path is matched Local Graphics
    //
    Status = gBS->HandleProtocol (
                ConsoleDev->Handle,
                &gEfiGraphicsOutputProtocolGuid,
                (VOID **) &ConOut
                );
    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS
SelectNextSetupMenu (
  BOOLEAN                   NextSetupMEnu
  )
{
  H2O_PAGE_ID               CurrentPageId;
  H2O_PAGE_ID               NextPageId;
  EFI_STATUS                Status;
  H2O_FORM_BROWSER_SM       *SetupMenuData;
  UINT32                    Index;
  UINT32                    HiiIndex;

  if (gFB == NULL || gFB->CurrentP == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CurrentPageId = gFB->CurrentP->PageId;
  HiiIndex = CurrentPageId >> 16;

  Status = gFB->GetSMInfo (gFB, &SetupMenuData);
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < SetupMenuData->NumberOfSetupMenus; Index++) {
    if ((SetupMenuData->SetupMenuInfoList[Index].PageId >> 16) == HiiIndex) {
      break;
    }
  }
  if (NextSetupMEnu) {
    if (Index >= (SetupMenuData->NumberOfSetupMenus - 1)) {
      NextPageId = SetupMenuData->SetupMenuInfoList[0].PageId;
    } else {
      NextPageId = SetupMenuData->SetupMenuInfoList[Index + 1].PageId;
    }
  } else {
    if (Index == 0) {
      NextPageId = SetupMenuData->SetupMenuInfoList[SetupMenuData->NumberOfSetupMenus - 1].PageId;
    } else {
      NextPageId = SetupMenuData->SetupMenuInfoList[Index - 1].PageId;
    }
  }

  FreePool (SetupMenuData->CoreVersionString);
  FreePool (SetupMenuData->SetupMenuInfoList);
  FreePool (SetupMenuData);

  SendSelectPNotify (NextPageId);

  return EFI_SUCCESS;
}

extern BOOLEAN                         mShowSetPositionDbg;

/**
  Add the notification to the notification queue and signal the Notification event.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] Notify             A pointer to the H2O_DISPLAY_ENGINE_EVT instance.

  @retval EFI_SUCCESS            Register notify successful.
  @retval EFI_INVALID_PARAMETER  This is NULL or Notify is NULL.

**/
EFI_STATUS
EFIAPI
LocalMetroNotify (
  IN       H2O_DISPLAY_ENGINE_PROTOCOL     *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT          *Notify
  )
{
  EFI_STATUS                                  Status;
  H2O_DISPLAY_ENGINE_EVT_KEYPRESS             *KeyPress;
  H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE         *AbsPtr;
  MSG                                         Msg;
  RECT                                        Rc;
  UI_CONTROL                                  *Control;

  mIsInNotifyProcess = TRUE;
  Status = EFI_SUCCESS;

  switch (Notify->Type) {

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_L:
    InitializeWindows ();
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_P:
    SendMessage (gWnd, FB_NOTIFY_REPAINT, 0, 0);
    RegisterTimerEvent (H2O_METRO_DE_TIMER_ID_PERIODIC_TIMER, H2O_METRO_DE_TIMER_PERIODIC_TIME);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SELECT_Q:
    SendMessage (gWnd, FB_NOTIFY_SELECT_Q, 0, 0);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_REL_PTR_MOVE:
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_ABS_PTR_MOVE:
    AbsPtr = (H2O_DISPLAY_ENGINE_EVT_ABS_PTR_MOVE *) Notify;
    GdAddPointerData (NULL, &AbsPtr->AbsPtrState);
    CopyMem (&mPreviousAbsPtrState, &AbsPtr->AbsPtrState, sizeof (EFI_ABSOLUTE_POINTER_STATE));
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS:
    KeyPress = (H2O_DISPLAY_ENGINE_EVT_KEYPRESS *) Notify;
    Control = GetUiControl (gWnd);

    switch (KeyPress->KeyData.Key.UnicodeChar) {

    case '1':
      SetRect (&Rc, 0, 0, 1024, 768);
      CONTROL_CLASS (Control)->SetPosition (Control, &Rc);
      break;

    case '2':
      SetRect (&Rc, 0, 0, 1366, 768);
      CONTROL_CLASS (Control)->SetPosition (Control, &Rc);
      break;

    case '3':
      mShowFps &= SHOW_REFRESH_DE_FPS;
      mShowFps ^= SHOW_REFRESH_DE_FPS;
      CONTROL_CLASS_INVALIDATE (Control);
      break;

    case '4':
      mShowFps &= SHOW_GOP_BLT_FPS;
      mShowFps ^= SHOW_GOP_BLT_FPS;
      CONTROL_CLASS_INVALIDATE (Control);
      break;

    case '5':
      mShowFps &= SHOW_TIMER_FPS;
      mShowFps ^= SHOW_TIMER_FPS;
      CONTROL_CLASS_INVALIDATE (Control);
      break;


    case '0':
      mShowSetPositionDbg = !mShowSetPositionDbg;
      break;

    case 'p':
    case 'P':
      SelectNextSetupMenu (FALSE);
      break;

    case 'n':
    case 'N':
      SelectNextSetupMenu (TRUE);
      break;

    default:
      GdAddEfiKeyData (&KeyPress->KeyData);
      break;

    }
    break;

  //
  // Open Dialog
  //
  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_D:
    InitializeWindows ();
    ReleaseCapture ();
    This->Notify = LocalMetroDialogNotify;
    This->Notify (This, Notify);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_TIMER:
    if (((H2O_DISPLAY_ENGINE_EVT_TIMER *) Notify)->TimerId == H2O_METRO_DE_TIMER_ID_PERIODIC_TIMER) {
      RegisterTimerEvent (H2O_METRO_DE_TIMER_ID_PERIODIC_TIMER, H2O_METRO_DE_TIMER_PERIODIC_TIME);
      ShowFps (gWnd);
    }
    break;
  }

  while (PeekMessage (&Msg, NULL, 0, 0, PM_REMOVE)) {
    if (Msg.message == WM_RBUTTONUP) {
      PostMessage (gWnd, WM_HOTKEY, 0, MAKELPARAM(0, VK_ESCAPE));
    }
    TranslateMessage (&Msg);
    DispatchMessage (&Msg);
  }

  mIsInNotifyProcess = FALSE;

//DERemoveFakeConOutDev (mDEPrivate);

  return Status;
}

/**
  Initialize private data for local graphics display engine and install display engine protocol

  @param [in] ImageHandle        The image handle
  @param [in] SystemTable        The system table

  @retval EFI_SUCCESS            Success to initialize private data and install display engine protocol
  @retval Other                  Fail to initialize display engine private data

**/
EFI_STATUS
EFIAPI
H2ODisplayEngineLocalMetroEntryPoint (
  IN     EFI_HANDLE              ImageHandle,
  IN     EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                                 Status;

  mMetroPrivate = AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_METRO_PRIVATE_DATA));
  if (mMetroPrivate == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mMetroPrivate->Signature   = H2O_DISPLAY_ENGINE_SIGNATURE;
  mMetroPrivate->ImageHandle = ImageHandle;

  mMetroPrivate->DisplayEngine.Size           = (UINT32) sizeof (H2O_DISPLAY_ENGINE_PROTOCOL);
  CopyGuid (&mMetroPrivate->DisplayEngine.Id, &gH2ODisplayEngineLocalMetroGuid);
  mMetroPrivate->DisplayEngine.InitConsole    = LocalMetroInitConsole;
  mMetroPrivate->DisplayEngine.AttachConsole  = LocalMetroAttachConsole;
  mMetroPrivate->DisplayEngine.DetachConsole  = LocalMetroDetachConsole;
  mMetroPrivate->DisplayEngine.SupportConsole = LocalMetroSupportConsole;
  mMetroPrivate->DisplayEngine.Notify         = LocalMetroNotify;

  InitializeListHead (&mMetroPrivate->ConsoleDevListHead);
  ZeroMem (&mPreviousAbsPtrState, sizeof (EFI_ABSOLUTE_POINTER_STATE));

  //
  // Locate FBProtocol
  //
  Status = gBS->LocateProtocol (
                  &gH2OFormBrowserProtocolGuid,
                  NULL,
                  (VOID **) &mMetroPrivate->FBProtocol
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gFB = mMetroPrivate->FBProtocol;


  Status = gBS->LocateProtocol (
                  &gH2OWindowProtocolGuid,
                  NULL,
                  (VOID **) &gH2OWindow
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                &gPerformanceMeasureProtocolGuid,
                NULL,
                (VOID **) &mPerf
                );
//ASSERT_EFI_ERROR (Status);
  if (mPerf) {
    mPerf->IsPrintEnable = FALSE;
  }


  //
  // Initial Display Engine Status
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gH2ODisplayEngineProtocolGuid,
                  &mMetroPrivate->DisplayEngine,
                  NULL
                  );

  return Status;
}
