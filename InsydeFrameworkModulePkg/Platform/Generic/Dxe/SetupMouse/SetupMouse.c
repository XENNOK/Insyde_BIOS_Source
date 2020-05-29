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
//;   SetupMouse.c
//;
//; Abstract:
//;
//;   Setup Mouse Protocol implementation
//;
//;
#include "SetupMouse.h"
#include EFI_PROTOCOL_DEFINITION (LegacyBios)

EFI_DRIVER_ENTRY_POINT(InitializeSetupMouse)

PRIVATE_MOUSE_DATA          *mPrivate = NULL;

EFI_STATUS
EFIAPI
InitializeSetupMouse (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:
  Initialize Setup

Arguments:
  (Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT)

Returns:
  EFI_SUCCESS - Setup Mouse loaded.
  other       - Setup Mouse Error

--*/
{
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;

  //
  // Initialize the library and our protocol.
  //
  EfiInitializeDriverLib (ImageHandle, SystemTable);

  Private = EfiLibAllocateZeroPool (sizeof (PRIVATE_MOUSE_DATA));

  Private->Signature                        = SETUP_MOUSE_SIGNATURE;
  Private->SetupMouse.Start                 = SetupMouseStart;
  Private->SetupMouse.Close                 = SetupMouseClose;
  Private->SetupMouse.QueryState            = QueryState;
  Private->SetupMouse.StartKeyboard         = StartKeyboard;
  Private->SetupMouse.CloseKeyboard         = CloseKeyboard;
  Private->SetupMouse.SetMode               = SetupMouseSetMode;
  Private->SetupMouse.SetKeyboardAttributes = SetupMouseSetKeyboardAttributes;
  Private->SetupMouse.GetKeyboardAttributes = SetupMouseGetKeyboardAttributes;

  Private->IsStart                          = FALSE;
  Private->HideCursorWhenTouch              = TRUE;

  mPrivate = Private;

  Status = gBS->InstallProtocolInterface (
                 &(Private->Handle),
                 &gEfiSetupMouseProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &(Private->SetupMouse)
                 );

  return Status;
}

EFI_STATUS
SetupMouseSetMode (
  IN EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  IN EFI_SETUP_MOUSE_SCREEN_MODE       Mode
  )
/*++

Routine Description:
  SetupMouse set to graphics / text mode coordinate
  Can avoid user call SetupMouse->Start in graphics Mode,
  but he want use SetupMouse in text mode and vice versa.

Arguments:
  SetupMouse  - EFI_SETUP_MOUSE_PROTOCOL
  Mode        - Graphics / Text mode

Returns:
  EFI_SUCCESS    - Set mode success
  EFI_NOT_READY  - SetupMouse doesn't start

--*/
{
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;
  UINTN                                 Col;
  UINTN                                 Row;
  EFI_TPL                               OriginalTPL;

  ASSERT (Mode < EfiSetupMouseScreenMaxValue);

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  OriginalTPL = gBS->RaiseTPL(EFI_TPL_NOTIFY);
  if (!Private->IsStart) {
    gBS->RestoreTPL (OriginalTPL);
    return EFI_NOT_READY;
  }

  Private->ScreenMode = Mode;

  if (Private->GopList == NULL) {
    gBS->RestoreTPL (OriginalTPL);
    return EFI_SUCCESS;
  }

  if (Mode == EfiSetupMouseScreenText) {
    GetStartOffset (
      Private->GopList[0].Gop,
      &Private->MouseRange.StartX,
      &Private->MouseRange.StartY
      );

    Status = gST->ConOut->QueryMode (
                            gST->ConOut,
                            gST->ConOut->Mode->Mode,
                            &Col,
                            &Row
                            );

    Private->MouseRange.EndX = Private->MouseRange.StartX + (Col * GLYPH_WIDTH) - 1;
    Private->MouseRange.EndY = Private->MouseRange.StartY + (Row * GLYPH_HEIGHT) - 1;

  } else {
    Private->MouseRange.StartX = 0;
    Private->MouseRange.StartY = 0;
    Private->MouseRange.EndX   = Private->GopList[0].Gop->Mode->Info->HorizontalResolution - 1;
    Private->MouseRange.EndY   = Private->GopList[0].Gop->Mode->Info->VerticalResolution - 1;
  }

  gBS->RestoreTPL (OriginalTPL);
  return EFI_SUCCESS;

}

EFI_STATUS
SetupMouseStart (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse
  )
/*++

Routine Description:
  Start SetupMouse

Arguments:

  SetupMouse      - EFI_SETUP_MOUSE_PROTOCOL

Returns:
  EFI_SUCCESS     - Start success
  EFI_NOT_FOUND   - Can't find GOP protocol

--*/
{
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;
  EFI_SIMPLE_POINTER_PROTOCOL           *SimplePointer;
  EFI_ABSOLUTE_POINTER_PROTOCOL         *AbsolutePointer;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 Index;
  UINTN                                 Count;
  POINTER_PROTOCOL_INFO                 *PointerProtocolInfo;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_CONSOLE_CONTROL_PROTOCOL          *ConsoleControl;
  EFI_CONSOLE_CONTROL_SCREEN_MODE       ScreenMode;
  UINTN                                 ScreenBufferSize;
  GOP_ENTRY                             *GopEntry;
  EFI_SIMPLE_POINTER_STATE              SimplePointerState;
  EFI_TPL                               OriginalTPL;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;


  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);


  OriginalTPL = gBS->RaiseTPL(EFI_TPL_NOTIFY);
  if (Private->IsStart) {
    gBS->RestoreTPL (OriginalTPL);
    return EFI_SUCCESS;
  }
  gBS->RestoreTPL (OriginalTPL);


  GraphicsOutput = NULL;
  DevicePath     = NULL;
  Count          = 0;

  //
  // Initial Mouses. Use ConSplitter instead of SimplePointers
  //
  SimplePointer = NULL;
  Status = gBS->HandleProtocol(
                  gST->ConsoleInHandle,
                  &gEfiSimplePointerProtocolGuid,
                  (VOID**)&SimplePointer
                  );
  if (!EFI_ERROR (Status)) {
    Status = SimplePointer->Reset (SimplePointer, TRUE);
    if (!EFI_ERROR (Status)) {
      Count ++;
    }
  }

  //
  // Initial ConSplitter's Absolute Pointer
  //
  AbsolutePointer = NULL;
  Status = gBS->HandleProtocol(
                  gST->ConsoleInHandle,
                  &gEfiAbsolutePointerProtocolGuid,
                  (VOID**)&AbsolutePointer
                  );
  if (!EFI_ERROR (Status)) {
    Status = AbsolutePointer->Reset (AbsolutePointer, TRUE);
    if (!EFI_ERROR (Status)) {
      Count ++;
    }
  }
  if (!Count) {
    return EFI_NOT_FOUND;
  }
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  (sizeof (POINTER_PROTOCOL_INFO) + sizeof (POINTER_PROTOCOL_INFO_DATA) * Count),
                  &PointerProtocolInfo
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  PointerProtocolInfo->Count = 0;
  //
  // Fill in PointerProtocolInfo with ConSplitter's Simple Pointer protocol
  //
  if (SimplePointer) {
    //
    // PS2 mosue GetState() will reset mouse in first time, it will waste much time.
    // So we call GetState() early to avoid interrupt other process.
    //
    SimplePointer->GetState(SimplePointer, &SimplePointerState);
    PointerProtocolInfo->Data[PointerProtocolInfo->Count].Attributes = ATTRIBUTE_VALUE_SIMPLE;
    PointerProtocolInfo->Data[PointerProtocolInfo->Count].PointerProtocol = SimplePointer;
    PointerProtocolInfo->Count++;
  }
  //
  // Fill in PointerProtocolInfo with information about all the Absolute Pointer protocols
  //
  if (AbsolutePointer) {
    PointerProtocolInfo->Data[PointerProtocolInfo->Count].Attributes = ATTRIBUTE_VALUE_ABSOLUTE;
    PointerProtocolInfo->Data[PointerProtocolInfo->Count].PointerProtocol = AbsolutePointer;
    PointerProtocolInfo->Count++;
  }
  //
  // Update PointerProtocolInfo to private data
  //
  Private->PointerProtocolInfo = PointerProtocolInfo;

  //
  // Connect GraphicsOutput.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &Count,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto QUIT;
  }

  //
  // search all physical GOP protocols
  //
  Private->GopList  = EfiLibAllocateZeroPool (sizeof (GOP_ENTRY) * Count);
  Private->GopCount = 0;

  for (Index = 0; Index < Count; Index++) {
    Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiGraphicsOutputProtocolGuid, &GraphicsOutput);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiDevicePathProtocolGuid, &DevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }

    GopEntry = &(Private->GopList[Private->GopCount]);

    //
    // replace all GOP blt function
    //
    GopEntry->Gop              = GraphicsOutput;
    GopEntry->OriginalBlt      = GraphicsOutput->Blt;
    GraphicsOutput->Blt        = SetupMouseScreenBlt;

    ScreenBufferSize = sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) *
                         GraphicsOutput->Mode->Info->HorizontalResolution *
                         GraphicsOutput->Mode->Info->VerticalResolution;

    SetRect (
      &GopEntry->Screen.ImageRc,
      0,
      0,
      GraphicsOutput->Mode->Info->HorizontalResolution,
      GraphicsOutput->Mode->Info->VerticalResolution
      );
    EfiCopyMem (&GopEntry->InvalidateRc, &GopEntry->Screen.ImageRc, sizeof (RECT));

    GopEntry->Screen.Image = EfiLibAllocateZeroPool (ScreenBufferSize);
    GopEntry->BlendBuffer  = EfiLibAllocateZeroPool (ScreenBufferSize);
    GopEntry->OriginalBlt (
                GraphicsOutput,
                GopEntry->Screen.Image,
                EfiBltVideoToBltBuffer,
                0,
                0,
                0,
                0,
                GraphicsOutput->Mode->Info->HorizontalResolution,
                GraphicsOutput->Mode->Info->VerticalResolution,
                0
                );

    GopEntry->FillLine = EfiLibAllocateZeroPool (sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * GraphicsOutput->Mode->Info->HorizontalResolution);
    Private->GopCount++;
  }
  gBS->FreePool (HandleBuffer);

  if (Private->GopCount == 0) {
    Status = EFI_NOT_FOUND;
    goto QUIT;
  }

  ScreenBufferSize = sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) *
                       Private->GopList[0].Gop->Mode->Info->HorizontalResolution *
                       Private->GopList[0].Gop->Mode->Info->VerticalResolution;


  Private->CheckBuffer = EfiLibAllocateZeroPool (ScreenBufferSize);


  //
  // Set start flag, let SetupMouseSetMode work
  //
  Private->IsStart = TRUE;

  //
  // Retrieve screen mode, default is Text Mode
  //
  ScreenMode = EfiConsoleControlScreenText;
  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, &ConsoleControl);
  if (!EFI_ERROR (Status)) {
    Status = ConsoleControl->GetMode (ConsoleControl, &ScreenMode, NULL, NULL);
  }
  if (ScreenMode == EfiConsoleControlScreenText) {
    SetupMouseSetMode (&Private->SetupMouse, EfiSetupMouseScreenText);
  } else {
    SetupMouseSetMode (&Private->SetupMouse, EfiSetupMouseScreenGraphics);
  }

  InitializeCursor (Private);
  if (Private->SaveCursorX == 0 && Private->SaveCursorY == 0) {
    Private->SaveCursorX = (Private->MouseRange.StartX + Private->MouseRange.EndX) / 2;
    Private->SaveCursorY = (Private->MouseRange.StartY + Private->MouseRange.EndY) / 2;
  }
  CursorGotoXY (Private->SaveCursorX, Private->SaveCursorY);
  ShowImage (&Private->Cursor);
  RenderImageForAllGop (Private);

  //
  //Create Setup Mouse event
  //
  Status = gBS->CreateEvent (
                  EFI_EVENT_TIMER | EFI_EVENT_NOTIFY_SIGNAL,
                  EFI_TPL_NOTIFY,
                  ProcessMouse,
                  (VOID*)SetupMouse,
                  &SetupMouse->Event
                  );
  if (EFI_ERROR (Status)) {
    goto QUIT;
  }

  Status = gBS->SetTimer(SetupMouse->Event, TimerPeriodic, MOUSE_TIMER);
  if (EFI_ERROR (Status)) {
    gBS->CloseEvent(SetupMouse->Event);
    goto QUIT;
  }

  //
  // Check CSM enable
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  if (!EFI_ERROR (Status)) {
    Private->IsCsmEnabled = TRUE;
  } else {
    Private->IsCsmEnabled = FALSE;
  }

  return EFI_SUCCESS;

QUIT:
  Private->IsStart = FALSE;
  if (Private->PointerProtocolInfo != NULL) {
    gBS->FreePool (Private->PointerProtocolInfo);
    Private->PointerProtocolInfo = NULL;
  }
  return Status;
}

EFI_STATUS
SetupMouseClose (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse
  )

/*++

Routine Description:
  Stop SetupMouse

Arguments:

  SetupMouse      - EFI_SETUP_MOUSE_PROTOCOL

Returns:
  Start setupmosue

--*/
{
  PRIVATE_MOUSE_DATA                    *Private;
  UINT32                                 Index;
  EFI_TPL                                OriginalTPL;



  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);


  OriginalTPL = gBS->RaiseTPL(EFI_TPL_NOTIFY);

  if (!Private->IsStart) {
    gBS->RestoreTPL(OriginalTPL);
    return EFI_SUCCESS;
  }
  Private->IsStart = FALSE;
  gBS->CloseEvent (SetupMouse->Event);
  gBS->RestoreTPL(OriginalTPL);

  Private->SaveCursorX = (UINTN) Private->Cursor.ImageRc.left;
  Private->SaveCursorY = (UINTN) Private->Cursor.ImageRc.top;

  HideImage (&Private->Cursor);
  CloseKeyboard (SetupMouse);

  RenderImageForAllGop (Private);
  DestroyCursor (Private);

  if (Private->GopList != NULL) {
    for (Index = 0; Index < Private->GopCount; Index++) {

      Private->GopList[Index].Gop->Blt = Private->GopList[Index].OriginalBlt;
      DestroyImage (&Private->GopList[Index].Screen);

      if (Private->GopList[Index].BlendBuffer != NULL) {
        gBS->FreePool (Private->GopList[Index].BlendBuffer);
      }

      if (Private->GopList[Index].FillLine != NULL) {
        gBS->FreePool (Private->GopList[Index].FillLine);
      }
    }

    gBS->FreePool (Private->GopList);
    Private->GopList = NULL;
    Private->GopCount = 0;
  }

  if (Private->CheckBuffer != NULL) {
    gBS->FreePool (Private->CheckBuffer);
    Private->CheckBuffer = NULL;
  }

  if (Private->PointerProtocolInfo) {
    gBS->FreePool (Private->PointerProtocolInfo);
    Private->PointerProtocolInfo = NULL;
  }

  Private->LButton = FALSE;
  Private->RButton = FALSE;

  return EFI_SUCCESS;
}

EFI_STATUS
QueryState (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  OUT UINTN                             *X,
  OUT UINTN                             *Y,
  OUT BOOLEAN                           *LeftButton,
  OUT BOOLEAN                           *RightButton
  )
/*++

Routine Description:
  Query cursor coordinate and button state,
  if ScrrenMode is text, it will convert coordinate

Arguments:

  SetupMouse      - EFI_SETUP_MOUSE_PROTOCOL
  X               - X coordinate
  Y               - Y coordinate
  LeftButton      - Mouse left button is pressed or Touch panel is touched
  RightButton     - Right button is touched

Returns:
  EFI_NOT_READY   - There was no keystroke data availiable
  EFI_SUCCESS     - Query status success

--*/
{
  PRIVATE_MOUSE_DATA                    *Private;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  if (!Private->IsStart) {
    return EFI_NOT_READY;
  }

  if (!Private->HaveRawData) {
    return EFI_NOT_READY;
  }
  Private->HaveRawData = FALSE;

  if (Private->ScreenMode == EfiSetupMouseScreenText) {
    *X = (UINTN)((Private->Cursor.ImageRc.left - Private->MouseRange.StartX) / GLYPH_WIDTH);
    *Y = (UINTN)((Private->Cursor.ImageRc.top - Private->MouseRange.StartY) / GLYPH_HEIGHT);
  }  else {
    *X = (UINTN)Private->Cursor.ImageRc.left;
    *Y = (UINTN)Private->Cursor.ImageRc.top;
  }

  *LeftButton  = Private->LButton;
  *RightButton = Private->RButton;

  return EFI_SUCCESS;

}

