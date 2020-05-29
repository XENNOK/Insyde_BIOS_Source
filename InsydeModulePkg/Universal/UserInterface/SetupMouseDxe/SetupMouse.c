/** @file
 Setup Mouse Protocol implementation

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

#include "SetupMouse.h"
#include <Protocol/LegacyBios.h>
#include <Protocol/DriverBinding.h>
#include <Guid/ReturnFromImage.h>
#include <Library/UefiLib.h>

PRIVATE_MOUSE_DATA              *mPrivate = NULL;
EFI_HII_HANDLE                  mImageHiiHandle;
//
// BugBug:
//   For detect device is disconnected, so we hook protocol's driver binding protocol stop function.
//   Besides we only except below function to been hooked
//
//     ConSplitter using EFI_OPEN_PROTOCOL_BY_DRIVER for SimplePointer / AbsolutePointer protocol;
//     ConSplitterSimplePointerDriverBindingStop and ConSplitterAbsolutePointerDriverBindingStop
//
//     GraphicsConsole using EFI_OPEN_PROTOCOL_BY_DRIVER for GraphicsOutput protocol
//     GraphicsConsoleControllerDriverStop
//
#define SETUP_MOUSE_DRIVER_BINDING_HOOK_LIST_COUNT 3

struct {
  EFI_GUID                      *ProtocolGuid;
  EFI_DRIVER_BINDING_PROTOCOL   *DriverBinding;
  EFI_DRIVER_BINDING_STOP       StopFunction;
} mSetupMouseDriverBindingHookList[SETUP_MOUSE_DRIVER_BINDING_HOOK_LIST_COUNT] = {
  {&gEfiSimplePointerProtocolGuid,   NULL, NULL},
  {&gEfiAbsolutePointerProtocolGuid, NULL, NULL},
  {&gEfiGraphicsOutputProtocolGuid,  NULL, NULL}
};

EFI_STATUS
HookDeviceStopFunction (
  IN EFI_GUID                   *ProtocolGuid
  );

/**
 Get number of physical device according to assigned protocol guid.


 @param [in]   ProtocolGuid     Search physical device protocol guid.


 @retval                        Count of physical device according to protocol guid.

**/
STATIC
UINT32
GetNumberOfDevices (
  IN EFI_GUID                   *ProtocolGuid
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         NumberOfHandles;
  EFI_HANDLE                    *HandleBuffer;
  UINT32                        Count;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return 0;
  }

  //
  // make sure it is physical device
  //
  Count = 0;
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &DevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Count++;
  }

  FreePool (HandleBuffer);

  return Count;
}

STATIC
VOID
FreeGopEntry (
  IN OUT GOP_ENTRY              *GopEntry,
  IN BOOLEAN                    RestoreHook
  )
{
  ASSERT (GopEntry != NULL);

  //
  // Restore GOP
  //
  if (RestoreHook) {
    if (GopEntry->GraphicsOutput != NULL && GopEntry->OriginalBlt != NULL) {
      GopEntry->GraphicsOutput->Blt = GopEntry->OriginalBlt;
//    GopEntry->GraphicsOutput->SetMode = GopEntry->OriginalSetMode;
    }
  }

  if (GopEntry->BlendBuffer != NULL) {
    FreePool (GopEntry->BlendBuffer);
  }
  if (GopEntry->FillLine != NULL) {
    FreePool (GopEntry->FillLine);
  }

  if (GopEntry->CheckBuffer != NULL) {
    FreePool (GopEntry->CheckBuffer);
  }

  if (GopEntry->Screen.Image != NULL) {
    FreePool (GopEntry->Screen.Image);
  }

  FreePool (GopEntry);
}


STATIC
EFI_STATUS
RetrieveGraphicsOutputInfo (
  OUT PRIVATE_MOUSE_DATA        *Private
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  UINTN                                 Count;
  EFI_HANDLE                            *HandleBuffer;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          **GraphicsOutputList;
  UINTN                                 GopCount;

  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  GOP_ENTRY                             *GopEntry;
  UINTN                                 ScreenBufferSize;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *ModeInfo;
  LIST_ENTRY                            *Node;

  ASSERT_LOCKED (&Private->SetupMouseLock);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &Count,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Retrieve GOP list
  //
  GraphicsOutputList = AllocateZeroPool (Count * sizeof (EFI_GRAPHICS_OUTPUT_PROTOCOL*));
  if (GraphicsOutputList == NULL) {
    FreePool (HandleBuffer);
    return EFI_OUT_OF_RESOURCES;
  }

  GopCount = 0;
  for (Index = 0; Index < Count; Index++) {

    Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiGraphicsOutputProtocolGuid, (VOID **)&GraphicsOutput);
    if (EFI_ERROR (Status)) {
      continue;
    }

    GraphicsOutputList[GopCount++] = GraphicsOutput;
  }
  FreePool (HandleBuffer);

  //
  // check the gop list valid
  //
  Node = GetFirstNode (&Private->GopList);
  while (!IsNull (&Private->GopList, Node)) {
    GopEntry = GOP_ENTRY_FROM_THIS (Node);
    for (Index = 0; Index < GopCount; Index++) {
      if (GraphicsOutputList[Index] == GopEntry->GraphicsOutput) {
        if (CompareMem (&GopEntry->ModeInfo, GraphicsOutputList[Index]->Mode->Info, sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION) == 0)) {
           ASSERT (FALSE);
        }
        GraphicsOutputList[Index] = NULL;
        break;
      }
    }

    //
    // remove invalid GopEntry
    //
    if (Index == GopCount) {
      Node = RemoveEntryList (&GopEntry->Link);
      FreeGopEntry (GopEntry, FALSE);
      continue;
    }

    Node = GetNextNode (&Private->GopList, Node);
  }



  //
  // add new GOP
  //
  for (Index = 0; Index < GopCount; Index++) {

    GraphicsOutput = GraphicsOutputList[Index];
    if (GraphicsOutput == NULL) {
      continue;
    }

    GopEntry = AllocateZeroPool (sizeof (GOP_ENTRY));
    if (GopEntry == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    GopEntry->Signature = GOP_ENTRY_SIGNATURE;
    InsertTailList (&Private->GopList, &GopEntry->Link);


    ModeInfo = GraphicsOutput->Mode->Info;
    CopyMem (&GopEntry->ModeInfo, ModeInfo, sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));

//  GopEntry->Handle            = HandleBuffer[Index];
    GopEntry->BytesPerScanLine  = sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * ModeInfo->HorizontalResolution;
    GopEntry->FillLine          = AllocateZeroPool (GopEntry->BytesPerScanLine);
    if (GopEntry->FillLine == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    SetRect (&GopEntry->Screen.ImageRc, 0, 0, ModeInfo->HorizontalResolution, ModeInfo->VerticalResolution);
    SetRect (&GopEntry->InvalidateRc,   0, 0, ModeInfo->HorizontalResolution, ModeInfo->VerticalResolution);
    ScreenBufferSize = ModeInfo->HorizontalResolution * ModeInfo->VerticalResolution * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    GopEntry->Screen.Image = AllocateZeroPool (ScreenBufferSize);
    if (GopEntry->Screen.Image == NULL) {
      Status =  EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    GopEntry->BlendBuffer  = AllocateZeroPool (ScreenBufferSize);
    if (GopEntry->BlendBuffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    GopEntry->CheckBuffer = AllocateZeroPool (ScreenBufferSize);
    if (GopEntry->CheckBuffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    Status = GraphicsOutput->Blt (
                               GraphicsOutput,
                               GopEntry->Screen.Image,
                               EfiBltVideoToBltBuffer,
                               0,
                               0,
                               0,
                               0,
                               ModeInfo->HorizontalResolution,
                               ModeInfo->VerticalResolution,
                               GopEntry->BytesPerScanLine
                               );
    ASSERT_EFI_ERROR (Status);

    //
    // invalidate screen region, for update cursor / virtual keybaord image to screen.
    //
    SetRect (&GopEntry->InvalidateRc, 0, 0, ModeInfo->HorizontalResolution, ModeInfo->VerticalResolution);

    //
    // hook gop blt function
    //
    GopEntry->GraphicsOutput = GraphicsOutput;
    GopEntry->OriginalBlt    = GraphicsOutput->Blt;
    GraphicsOutput->Blt      = SetupMouseScreenBlt;
  }

  Status = EFI_SUCCESS;

Done:
  ASSERT_EFI_ERROR (Status);
  return Status;
}

EFI_STATUS
EFIAPI
SetCursorPos (
  IN  UINTN                     X,
  IN  UINTN                     Y
  )
{
  PRIVATE_MOUSE_DATA            *Private;
  EFI_STATUS                    Status;

  Private = mPrivate;
  AcquireSetupMouseLock (Private);
  Status = InternalSetCursorPos (X, Y);
  ReleaseSetupMouseLock (Private);

  return Status;
}



/**
 Determine mouse or touch devices are existence.

 @param[in]  Event              The event that be siganlled when devices number change.
 @param[in]  Context            None.

**/
VOID
EFIAPI
DetermineDevicesExistCallback (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  PRIVATE_MOUSE_DATA            *Private;
  UINT32                        MouseCount;
  UINT32                        TouchCount;
  GOP_ENTRY                     *GopEntry;

  Private = mPrivate;

  MouseCount = GetNumberOfDevices (&gEfiSimplePointerProtocolGuid);
  TouchCount = GetNumberOfDevices (&gEfiAbsolutePointerProtocolGuid);

  AcquireSetupMouseLock (Private);

  RetrieveGraphicsOutputInfo (Private);

  //
  // Gop == 0                                : reset cursor
  // SaveCursorX == -1 and SaveCursorY == -1 : set cursor in center of screen
  //
  if (IsListEmpty (&Private->GopList)) {
    Private->MouseRange.StartX = 0;
    Private->MouseRange.StartY = 0;
    Private->MouseRange.EndX   = 0;
    Private->MouseRange.EndY   = 0;
    Private->SaveCursorX = (UINT32) -1;
    Private->SaveCursorY = (UINT32) -1;

    ReleaseSetupMouseLock (Private);
    return ;
  }

  GopEntry = GOP_ENTRY_FROM_THIS (GetFirstNode (&Private->GopList));
  Private->MouseRange.StartX = 0;
  Private->MouseRange.StartY = 0;
  Private->MouseRange.EndX   = GopEntry->ModeInfo.HorizontalResolution;
  Private->MouseRange.EndY   = GopEntry->ModeInfo.VerticalResolution;

  if ((Private->SaveCursorX == (UINT32) -1) && (Private->SaveCursorY == (UINT32) -1)) {
    Private->SaveCursorX = GopEntry->ModeInfo.HorizontalResolution / 2;
    Private->SaveCursorY = GopEntry->ModeInfo.VerticalResolution / 2;
  }

  InternalSetCursorPos (Private->SaveCursorX, Private->SaveCursorY);

  if (MouseCount == 0) {
    HideImage (&Private->Cursor);
  } else if (!Private->HideCursor) {
    ShowImage (&Private->Cursor);
  }

  ReleaseSetupMouseLock (Private);
}

/**
 Callback function for devices plugs in, it will hook device's DriverBinding stop function.

 @param [in] Event              Event for SimplePointer / AbsolutePointer.
 @param [in] Context            Context of event.

**/
VOID
EFIAPI
DevicePluginCallback (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  PRIVATE_MOUSE_DATA            *Private;

  Private = mPrivate;

  HookDeviceStopFunction (&gEfiSimplePointerProtocolGuid);
  HookDeviceStopFunction (&gEfiAbsolutePointerProtocolGuid);
  HookDeviceStopFunction (&gEfiGraphicsOutputProtocolGuid);

  gBS->SignalEvent (Private->CheckDevicesEvent);
}

/**
 According to ProtocolGuid to search which DriverBinding protocol OpenProtocol by EFI_OPEN_PROTOCOL_BY_DRIVER,
 and then hook the DriverBinding protocol stop function

 @param [in] ControllerHandle   To hook driver binding stop function from ControllerHandle.
 @param [in] ProtocolGuid       ProtocolGuid which OpenProtocol by EFI_OPEN_PROTOCOL_BY_DRIVER.

 @retval EFI_SUCCESS            Hook DriverBinding protocol stop function success.
 @retval EFI_NOT_FOUND          Can't found the assigned DriverBinding protocol.
 @retval Others                 Fail to get return status data.

**/
EFI_STATUS
EFIAPI
SetupMouseDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL           *This,
  IN EFI_HANDLE                            ControllerHandle,
  IN UINTN                                 NumberOfChildren,
  IN EFI_HANDLE                            *ChildHandleBuffer
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_DRIVER_BINDING_STOP       StopFunction;
  EFI_GUID                      *ProtocolGuid;
  PRIVATE_MOUSE_DATA            *Private;

  Private = mPrivate;

  StopFunction = NULL;
  ProtocolGuid = NULL;
  for (Index = 0; Index < SETUP_MOUSE_DRIVER_BINDING_HOOK_LIST_COUNT; Index++) {
    if (This == mSetupMouseDriverBindingHookList[Index].DriverBinding) {
      ProtocolGuid = mSetupMouseDriverBindingHookList[Index].ProtocolGuid;
      StopFunction = mSetupMouseDriverBindingHookList[Index].StopFunction;
      break;
    }
  }

  ASSERT (StopFunction != NULL);
  ASSERT (ProtocolGuid != NULL);
  if (StopFunction == NULL || ProtocolGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = StopFunction (This, ControllerHandle, NumberOfChildren, ChildHandleBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gBS->SignalEvent (Private->CheckDevicesEvent);

  return Status;
}

/**
 According to ProtocolGuid to search which DriverBinding protocol OpenProtocol by EFI_OPEN_PROTOCOL_BY_DRIVER,
 and then hook the DriverBinding protocol stop function

 @param [in] ControllerHandle   To hook driver binding stop function from ControllerHandle.
 @param [in] ProtocolGuid       ProtocolGuid which OpenProtocol by EFI_OPEN_PROTOCOL_BY_DRIVER.

 @retval EFI_SUCCESS            Hook DriverBinding protocol stop function success.
 @retval EFI_NOT_FOUND          Can't found the assigned DriverBinding protocol.
 @retval Others                 Fail to get return status data.

**/
EFI_STATUS
HookSingleDeviceStopFunction (
  IN EFI_HANDLE                 ControllerHandle,
  IN EFI_GUID                   *ProtocolGuid
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            AgentHandle;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY   *OpenInfoBuffer;
  UINTN                                 EntryCount;
  UINTN                                 Index;
  EFI_DRIVER_BINDING_PROTOCOL           *DriverBinding;
  PRIVATE_MOUSE_DATA                    *Private;
  EFI_TPL                               OriginalTPL;

  ASSERT (ProtocolGuid != NULL);

  Private = mPrivate;

  Status = gBS->OpenProtocolInformation (
                  ControllerHandle,
                  (EFI_GUID *) ProtocolGuid,
                  &OpenInfoBuffer,
                  &EntryCount
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AgentHandle = NULL;
  for (Index = 0; Index < EntryCount; Index++) {
    if ((OpenInfoBuffer[Index].ControllerHandle == ControllerHandle) &&
        (OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_DRIVER) != 0) {
      AgentHandle = OpenInfoBuffer[Index].AgentHandle;
      break;
    }
  }

  FreePool (OpenInfoBuffer);
  if (AgentHandle == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // locate driver binding protocol
  //
  Status = gBS->HandleProtocol (
                  AgentHandle,
                  &gEfiDriverBindingProtocolGuid,
                  (VOID **) &DriverBinding
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // hook driver binding stop function
  //
  for (Index = 0; Index < SETUP_MOUSE_DRIVER_BINDING_HOOK_LIST_COUNT; Index++) {
    if (CompareGuid (mSetupMouseDriverBindingHookList[Index].ProtocolGuid, ProtocolGuid)) {
      if (DriverBinding->Stop != SetupMouseDriverBindingStop) {
        OriginalTPL = gBS->RaiseTPL(TPL_NOTIFY);
        mSetupMouseDriverBindingHookList[Index].DriverBinding = DriverBinding;
        mSetupMouseDriverBindingHookList[Index].StopFunction  = DriverBinding->Stop;
        DriverBinding->Stop = SetupMouseDriverBindingStop;
        gBS->RestoreTPL (OriginalTPL);
      }
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


/**
 Search physical device protocol which OpenProtocol by OPEN_PROTOCOL_BY_DRIVER and then
 hook the DriverBinding protocol stop function for monitor the protocol.

 @param [in]   ProtocolGuid     Searched physical device protocol


 @retval EFI_SUCCESS            Hook DriverBinding protocol stop function success.
 @retval Others                 Fail to get return status data.

**/
EFI_STATUS
HookDeviceStopFunction (
  IN EFI_GUID                   *ProtocolGuid
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         NumberOfHandles;
  EFI_HANDLE                    *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  ProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // make sure it is physical device
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &DevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    HookSingleDeviceStopFunction (HandleBuffer[Index], ProtocolGuid);
  }

  FreePool (HandleBuffer);

  return EFI_SUCCESS;
}


/**
  This function uses to enable sync frame buffer

  @param Event      Event whose notification function is being invoked.
  @param Context    Pointer to the notification function's context.

**/
VOID
EFIAPI
EnableSyncFrameBufferCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  PRIVATE_MOUSE_DATA            *Private;

  Private = SETUP_MOUSE_DEV_FROM_THIS (Context);

  Private->NeedSyncFrameBuffer  = TRUE;
}

/**
  This function uses to disable sync frame buffer

  @param Event      Event whose notification function is being invoked.
  @param Context    Pointer to the notification function's context.

**/
VOID
EFIAPI
DisableSyncFrameBufferCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  PRIVATE_MOUSE_DATA            *Private;

  Private = SETUP_MOUSE_DEV_FROM_THIS (Context);

  Private->NeedSyncFrameBuffer  = FALSE;
}



/**
 Initialize Setup

 @param [in]   ImageHandle
 @param [in]   SystemTable

 @retval EFI_SUCCESS            Setup Mouse loaded.
 @return other                  Setup Mouse Error

**/
EFI_STATUS
EFIAPI
InitializeSetupMouse (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS                    Status;
  PRIVATE_MOUSE_DATA            *Private;
  EFI_EVENT                     Event;
  VOID                          *Registration;

  Private = AllocateZeroPool (sizeof (PRIVATE_MOUSE_DATA));
  if (Private == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->Signature                        = SETUP_MOUSE_SIGNATURE;
  Private->SetupMouse.Start                 = SetupMouseStart;
  Private->SetupMouse.Close                 = SetupMouseClose;
  Private->SetupMouse.QueryState            = QueryState;
  Private->SetupMouse.StartKeyboard         = StartKeyboard;
  Private->SetupMouse.CloseKeyboard         = CloseKeyboard;
  Private->SetupMouse.SetMode               = SetupMouseSetMode;
  Private->SetupMouse.SetKeyboardAttributes = SetupMouseSetKeyboardAttributes;
  Private->SetupMouse.GetKeyboardAttributes = SetupMouseGetKeyboardAttributes;
  Private->SaveCursorX                      = (UINT32) -1;
  Private->SaveCursorY                      = (UINT32) -1;

  Private->IsStart                          = FALSE;
  Private->HideCursor                       = FALSE;
  Private->HideCursorWhenTouch              = TRUE;
  Private->NeedSyncFrameBuffer              = FALSE;

  InitializeListHead (&Private->GopList);
  EfiInitializeLock (&Private->SetupMouseLock, TPL_NOTIFY);

  mPrivate = Private;

  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             EnableSyncFrameBufferCallback,
             (VOID *) &Private->SetupMouse,
             &Event
             );
  ASSERT_EFI_ERROR (Status);


  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  DisableSyncFrameBufferCallback,
                  (VOID *) &Private->SetupMouse,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gReturnFromImageGuid,
                    Event,
                    &Registration
                    );
    ASSERT_EFI_ERROR (Status);
  }

  mImageHiiHandle = HiiAddPackages (&gEfiCallerIdGuid, NULL, SetupMouseDxeImages, NULL);
  Status = gBS->InstallProtocolInterface (
                  &(Private->Handle),
                  &gSetupMouseProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &(Private->SetupMouse)
                  );

  return Status;
}

/**
 SetupMouse set to graphics / text mode coordinate
 Can avoid user call SetupMouse->Start in graphics Mode,
 but he want use SetupMouse in text mode and vice versa.

 @param [in]   SetupMouse       EFI_SETUP_MOUSE_PROTOCOL
 @param [in]   Mode             Graphics / Text mode

 @retval EFI_SUCCESS            Set mode success
 @retval EFI_NOT_READY          SetupMouse doesn't start

**/
EFI_STATUS
EFIAPI
SetupMouseSetMode (
  IN EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  IN EFI_SETUP_MOUSE_SCREEN_MODE       Mode
  )
{
  return EFI_UNSUPPORTED;
}

STATIC
EFI_STATUS
RetrievePointerProtocolInfo (
  PRIVATE_MOUSE_DATA            *Private
  )
{
  UINTN                         Count;
  EFI_STATUS                    Status;
  EFI_SIMPLE_POINTER_PROTOCOL   *SimplePointer;
  EFI_ABSOLUTE_POINTER_PROTOCOL *AbsolutePointer;
  POINTER_PROTOCOL_INFO         *PointerProtocolInfo;
  EFI_SIMPLE_POINTER_STATE      SimplePointerState;


  Count = 0;
  //
  // Initial ConSplitter's Simple Pointer
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

  PointerProtocolInfo = (POINTER_PROTOCOL_INFO *) AllocateZeroPool (
                                                    sizeof (POINTER_PROTOCOL_INFO) + sizeof (POINTER_PROTOCOL_INFO_DATA) * Count);
  if (PointerProtocolInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
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

  return EFI_SUCCESS;

}

VOID
FreeGopEntryList (
  PRIVATE_MOUSE_DATA            *Private
  )
{
  GOP_ENTRY                     *GopEntry;

  while (!IsListEmpty (&Private->GopList)) {
    GopEntry = GOP_ENTRY_FROM_THIS (GetFirstNode (&Private->GopList));
    RemoveEntryList (&GopEntry->Link);

    if (GopEntry->BlendBuffer != NULL) {
      FreePool (GopEntry->BlendBuffer);
    }
    if (GopEntry->FillLine != NULL) {
      FreePool (GopEntry->FillLine);
    }
    if (GopEntry->Screen.Image != NULL) {
      FreePool (GopEntry->Screen.Image);
    }

    //
    // Restore GOP
    //
    if (GopEntry->GraphicsOutput != NULL && GopEntry->OriginalBlt != NULL) {
      GopEntry->GraphicsOutput->Blt = GopEntry->OriginalBlt;
    }

    FreePool (GopEntry);
  }

}

/**
 Start SetupMouse

 @param [in]   SetupMouse       EFI_SETUP_MOUSE_PROTOCOL

 @retval EFI_SUCCESS            Start success
 @retval EFI_NOT_FOUND          Can't find GOP protocol

**/
EFI_STATUS
EFIAPI
SetupMouseStart (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse
  )
{
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;
  EFI_HANDLE                            *HandleBuffer;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;
  VOID                                  *Registration;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  if (Private->IsStart) {
    return EFI_SUCCESS;
  }

  HandleBuffer = NULL;
  AcquireSetupMouseLock (Private);

  Status = RetrievePointerProtocolInfo (Private);
  if (EFI_ERROR (Status)) {
    goto QUIT;
  }

  Status = InitializeCursor (Private);
  if (EFI_ERROR (Status)) {
    goto QUIT;
  }

  Private->MouseRange.StartX = 0;
  Private->MouseRange.StartY = 0;
  Private->MouseRange.EndX = 0;
  Private->MouseRange.EndY = 0;


  //
  // Using CSM enable to make sure BDA existence.
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


//if (Private->SaveCursorX == (UINT32)(-1) && Private->SaveCursorY == (UINT32)(-1) && IsRectEmpty (&Private->MouseRange)) {
//  Private->SaveCursorX = (Private->MouseRange.StartX + Private->MouseRange.EndX) / 2;
//  Private->SaveCursorY = (Private->MouseRange.StartY + Private->MouseRange.EndY) / 2;
//}

//InternalSetCursorPos (Private->SaveCursorX, Private->SaveCursorY);
//ShowImage (&Private->Cursor);

//Status = RetrieveGraphicsOutputInfo (Private);
//if (EFI_ERROR (Status) && Status != EFI_NOT_FOUND) {
//  goto QUIT;
//}

  //
  // Set start flag, let SetupMouseSetMode work
  //
  Private->IsStart = TRUE;
//RenderImageForAllGop (Private);

  //
  //Create Setup Mouse event
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  ProcessMouse,
                  (VOID*)SetupMouse,
                  &SetupMouse->Event
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->SetTimer (SetupMouse->Event, TimerPeriodic, MOUSE_TIMER);
  if (EFI_ERROR (Status)) {
    goto QUIT;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  DetermineDevicesExistCallback,
                  NULL,
                  &Private->CheckDevicesEvent
                  );
  ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR (Status)) {
    Status = gBS->SignalEvent (Private->CheckDevicesEvent);
  }

  //
  // Register SimplePointer/AbsolutePointer/GraphicsOutput Protocol notify
  //
  Private->MouseHotplugEvent = EfiCreateProtocolNotifyEvent (
                                 &gEfiSimplePointerProtocolGuid,
                                 TPL_NOTIFY,
                                 DevicePluginCallback,
                                 NULL,
                                 &Registration
                                 );

  Private->TouchHotplugEvent = EfiCreateProtocolNotifyEvent (
                                 &gEfiAbsolutePointerProtocolGuid,
                                 TPL_NOTIFY,
                                 DevicePluginCallback,
                                 NULL,
                                 &Registration
                                 );
  Private->GopHotplugEvent = EfiCreateProtocolNotifyEvent (
                                 &gEfiGraphicsOutputProtocolGuid,
                                 TPL_NOTIFY,
                                 DevicePluginCallback,
                                 NULL,
                                 &Registration
                                 );


  ReleaseSetupMouseLock (Private);

  return EFI_SUCCESS;

QUIT:

  ASSERT_EFI_ERROR (Status);
  Private->IsStart = FALSE;

  if (Private->PointerProtocolInfo != NULL) {
    FreePool (Private->PointerProtocolInfo);
    Private->PointerProtocolInfo = NULL;
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  ReleaseSetupMouseLock (Private);

  return Status;
}

EFI_STATUS
EFIAPI
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
  PRIVATE_MOUSE_DATA            *Private;
  GOP_ENTRY                     *GopEntry;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  AcquireSetupMouseLock (Private);
  if (!Private->IsStart) {
    ReleaseSetupMouseLock (Private);
    return EFI_SUCCESS;
  }
  Private->IsStart = FALSE;
  gBS->CloseEvent (SetupMouse->Event);
  gBS->CloseEvent (Private->MouseHotplugEvent);
  gBS->CloseEvent (Private->TouchHotplugEvent);
  gBS->CloseEvent (Private->GopHotplugEvent);
  gBS->CloseEvent (Private->CheckDevicesEvent);

//Private->SaveCursorX = (UINTN) Private->Cursor.ImageRc.left;
//Private->SaveCursorY = (UINTN) Private->Cursor.ImageRc.top;

  HideImage (&Private->Cursor);
  InternalCloseKeyboard (SetupMouse);

  RenderImageForAllGop (Private);
  DestroyCursor (Private);

  //
  // Free GOP list
  //
  while (!IsListEmpty (&Private->GopList)) {
    GopEntry = GOP_ENTRY_FROM_THIS (GetFirstNode (&Private->GopList));
    RemoveEntryList (&GopEntry->Link);
    FreeGopEntry (GopEntry, TRUE);
  }

  if (Private->PointerProtocolInfo) {
    FreePool (Private->PointerProtocolInfo);
    Private->PointerProtocolInfo = NULL;
  }

  Private->LButton = FALSE;
  Private->RButton = FALSE;

  ReleaseSetupMouseLock (Private);

  return EFI_SUCCESS;
}

/**
 Query cursor coordinate and button state,
 if ScrrenMode is text, it will convert coordinate

 @param [in]   SetupMouse       EFI_SETUP_MOUSE_PROTOCOL
 @param [out]  X                X coordinate
 @param [out]  Y                Y coordinate
 @param [out]  LeftButton       Mouse left button is pressed or Touch panel is touched
 @param [out]  RightButton      Right button is touched

 @retval EFI_NOT_READY          There was no keystroke data availiable
 @retval EFI_SUCCESS            Query status success

**/
EFI_STATUS
EFIAPI
QueryState (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  OUT UINTN                             *X,
  OUT UINTN                             *Y,
  OUT BOOLEAN                           *LeftButton,
  OUT BOOLEAN                           *RightButton
  )
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

  *X = (UINTN)Private->Cursor.ImageRc.left;
  *Y = (UINTN)Private->Cursor.ImageRc.top;

  *LeftButton  = Private->LButton;
  *RightButton = Private->RButton;

  return EFI_SUCCESS;

}

//
// SetupMouse Virtual Keyboard
//

EFI_STATUS
EFIAPI
StartKeyboard (
  IN  EFI_SETUP_MOUSE_PROTOCOL  *SetupMouse,
  IN  UINTN                     X,
  IN  UINTN                     Y
  )
{
  EFI_STATUS                    Status;
  PRIVATE_MOUSE_DATA            *Private;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  SetupMouse->Start (SetupMouse);
  AcquireSetupMouseLock (Private);
  Status = InternalStartKeyboard (SetupMouse, X, Y);
  ReleaseSetupMouseLock (Private);

  return Status;
}


/**
 KeyboardData function restores data from blt buffer to the screen.

 @param [in]   SetupMouse

 @retval EFI_SUCCESS            Screen has been restored from blt buffer successfully

**/
EFI_STATUS
EFIAPI
CloseKeyboard (
  IN  EFI_SETUP_MOUSE_PROTOCOL  *SetupMouse
  )
{
  EFI_STATUS                    Status;
  PRIVATE_MOUSE_DATA            *Private;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  AcquireSetupMouseLock (Private);
  Status = InternalCloseKeyboard (SetupMouse);
  ReleaseSetupMouseLock (Private);

  return Status;
}


EFI_STATUS
EFIAPI
SetupMouseSetKeyboardAttributes (
  IN  EFI_SETUP_MOUSE_PROTOCOL         *SetupMouse,
  IN  KEYBOARD_ATTRIBUTES              *KeyboardAttributes
  )
{
  EFI_STATUS                     Status;
  PRIVATE_MOUSE_DATA            *Private;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  AcquireSetupMouseLock (Private);
  Status = InternalSetupMouseSetKeyboardAttributes (SetupMouse, KeyboardAttributes);
  ReleaseSetupMouseLock (Private);

  return Status;
}

EFI_STATUS
EFIAPI
SetupMouseGetKeyboardAttributes (
  IN  EFI_SETUP_MOUSE_PROTOCOL         *SetupMouse,
  IN  KEYBOARD_ATTRIBUTES              *KeyboardAttributes
  )
{
  EFI_STATUS                     Status;
  PRIVATE_MOUSE_DATA            *Private;

  Private = SETUP_MOUSE_DEV_FROM_THIS (SetupMouse);

  AcquireSetupMouseLock (Private);
  Status = InternalSetupMouseGetKeyboardAttributes (SetupMouse, KeyboardAttributes);
  ReleaseSetupMouseLock (Private);

  return Status;
}


