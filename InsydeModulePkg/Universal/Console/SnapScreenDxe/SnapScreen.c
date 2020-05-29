/** @file
  Serial driver for standard UARTS on an ISA bus.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "SnapScreen.h"
#include "SnapDialog.h"
#include "SnapBmp.h"
#include "SnapConOut.h"
#include "SnapTextOutHook.h"

//#include "EfiPrintLib.h"
#include <Library/PrintLib.h>

//#include EFI_PROTOCOL_CONSUMER (LoadedImage)
#include <Protocol/LoadedImage.h>
//#include EFI_PROTOCOL_CONSUMER (SimpleTextInputEx)

#ifndef SNAP_HOTKEY
#define SNAP_HOTKEY         SCAN_F8
#endif

#define MAX_HOT_KEY_HANDLE     10

// {2226205D-03FE-4395-B29D-C0F1EB11D50D}
#define SNAP_SCREEN_DRV_GUID   { 0x2226205d, 0x3fe, 0x4395, { 0xb2, 0x9d, 0xc0, 0xf1, 0xeb, 0x11, 0xd5, 0xd } }

typedef struct {
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *StiProtocol;
  EFI_HANDLE                          NotifyHandle;
} HOTKEY_ENTRY;

typedef VOID (*CONSOLE_GET_TEXT) (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

typedef VOID (*CONSOLE_PUT_TEXT) (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

CONSOLE_GET_TEXT    GetText;
CONSOLE_PUT_TEXT    PutText;

EFI_GUID    gSnapScreenDriverGuid = SNAP_SCREEN_DRV_GUID;

HOTKEY_ENTRY      gHotkeyEntry[MAX_HOT_KEY_HANDLE];
UINT8             gNotifyHandleCount = 0;
BOOLEAN           gInService = FALSE;
VOID              *gStiRegistration;
EFI_EVENT         gSimpleInputExEvent;
EFI_EVENT         gHotkeyEvtNotify;

VOID
HotkeyNotificationFunction (
  IN  EFI_EVENT          Event,
  IN  VOID               *Context
  )
{
  EFI_STATUS              Status;
  CHAR16                  FileName[BMP_TIME_FILE_NAME_SIZE];
  FS_DIALOG               *FsDialog;
  EFI_HANDLE              FsHandle;
  BMP_FILE                *BmpFile;
  MSG_DIALOG              *MsgDialog;
  INT32                   OldAttribute;
  BOOLEAN                 CursorVisible;
  CHAR16                  Buffer[256];
  INT32                   CursorColumn;
  INT32                   CursorRow;

  gInService = TRUE;
 
  Status = InitializeSnapConOut ();
  if (EFI_ERROR(Status))
    return;

  ScoGetModeInfo (&CursorColumn, &CursorRow, &CursorVisible, &OldAttribute);

  ScoEnableCursor (FALSE);

  //2 Pop-up a window for select a storage device
  Status = TFsDialogCreate (
                            (UINT16)WIN_SCR_CENTER, 
                            (UINT16)WIN_SCR_CENTER, 
                            L"SnapScreen -- Device Selection", 
                            NULL, 
                            &FsDialog
                           ); 
  if (EFI_ERROR(Status)) {
    if (Status == EFI_NOT_FOUND) {
      PopUpMessage (L"No Recordable device !");
    }
    goto EXIT;
  }
  
  Status = TFsDialogExec(FsDialog, &FsHandle);
  TFsDialogDestroy (FsDialog);

  if (EFI_ERROR(Status)) {
    goto EXIT;
  }

  //2 Save image to BMP file
  
  // Obtain a file name that make up by date&time
  Status = ObtainBmpFileNameByTime ( FileName, BMP_TIME_FILE_NAME_SIZE);
  if (EFI_ERROR(Status)) {
    goto EXIT;
  }

  // create a BMP file object
  Status = BmpCreate (FsHandle, FileName, &BmpFile);
  if (EFI_ERROR(Status)) {
    PopUpMessage (L"Device is removed!");
    goto EXIT;
  }                      

  // Capture screen to bmp image
  Status = BmpScreenToImage (BmpFile, 0, 0, MAX_VGA_MODE_WIDTH, MAX_VGA_MODE_HEIGHT);
  if (EFI_ERROR(Status)) {
    goto EXIT;
  }                      

  //Pop-up Message Box
  UnicodeSPrint( Buffer, 254, L"Save to : %s ....", FileName);
  TMsgDialogCreate (Buffer, &MsgDialog);
  TMsgDialogShow (MsgDialog);

  // save bmp image to a file
  Status = BmpSaveToFile (BmpFile);

  TMsgDialogDestroy(MsgDialog);
  
  if (EFI_ERROR(Status)) {
    PopUpMessage (L"ERROR : Cann't save to file.");
    goto EXIT;
  }  
  
  UnicodeSPrint ( Buffer, 254, L"Save to : %s OK!", FileName);
  TMsgDialogCreate ( Buffer, &MsgDialog);
  TMsgDialogRun ( MsgDialog);
  TMsgDialogDestroy( MsgDialog);

  // release BMP object
  BmpDestroy(BmpFile);
    
  Status = EFI_SUCCESS;

EXIT:

  ScoSetCursorPosition (CursorColumn, CursorRow);
  ScoEnableCursor (CursorVisible);
  ScoSetAttribute (OldAttribute);
  gInService = FALSE;
  gBS->CloseEvent (gHotkeyEvtNotify);
  
  return;
}

EFI_STATUS
HotkeyEventCallback (
  IN  EFI_KEY_DATA        *KeyData
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  // set in service flag for prevent re-enterance
  if (gInService == TRUE)
    return Status;

  // Create another event for termianlConin fail issue.
  // Reduce event TPL level for terminalConOut fail issue. 
  Status = gBS->CreateEvent (
               EVT_TIMER | EVT_NOTIFY_SIGNAL, 
               TPL_CALLBACK - 1,
               HotkeyNotificationFunction, 
               NULL, 
               &gHotkeyEvtNotify
               );

  Status = gBS->SetTimer (gHotkeyEvtNotify, TimerRelative, 10 * TICKS_PER_MS);
    
  return Status;
  
}

EFI_STATUS
RegisterSnapHotKey (
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *SimpleTextInputEx,
  UINT16                                ScanCode
  )
{
  EFI_STATUS        Status = EFI_OUT_OF_RESOURCES;
  EFI_KEY_DATA      KeyData;
  
  KeyData.Key.ScanCode = ScanCode;
  KeyData.Key.UnicodeChar = 0;
  KeyData.KeyState.KeyShiftState = 0;
  KeyData.KeyState.KeyToggleState = 0;

  if (gNotifyHandleCount < MAX_HOT_KEY_HANDLE) {
  
    gHotkeyEntry[gNotifyHandleCount].StiProtocol = SimpleTextInputEx;
    
    // Register SnapScreen Hotkey
    Status = SimpleTextInputEx->RegisterKeyNotify (
                                    SimpleTextInputEx, 
                                    &KeyData, 
                                    HotkeyEventCallback, 
                                    &gHotkeyEntry[gNotifyHandleCount].NotifyHandle
                                    );
  }

  if (Status == EFI_SUCCESS)
    gNotifyHandleCount++;

  return Status;
}


VOID
EFIAPI
SimpleInputExCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS                            Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *SimpleTextInputEx;
  UINTN                                 HandleSize;
  EFI_HANDLE                            Handle;

  while (TRUE) {
    HandleSize = sizeof (EFI_HANDLE);
    Status = gBS->LocateHandle (
                    ByRegisterNotify,
                    NULL,
                    gStiRegistration,
                    &HandleSize,
                    &Handle
                    );

    if (Status == EFI_NOT_FOUND) {
      return;
    }
    
    ASSERT_EFI_ERROR (Status);
    
    Status = gBS->HandleProtocol ( Handle, &gEfiSimpleTextInputExProtocolGuid, (VOID **)&SimpleTextInputEx);

    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Snap Screen : Can't locate SimpleTextInputExProtocol !, Status = %r\n", Status));
      continue;
    }
    
    RegisterSnapHotKey(SimpleTextInputEx, SNAP_HOTKEY);
  }
}

EFI_STATUS
UnloadSnapDrv (
  IN EFI_HANDLE     ImageHandle
  )
{
  UINTN       Index;

  // un-register hotkey
  for (Index = 0; Index < gNotifyHandleCount; Index++) {
    gHotkeyEntry[Index].StiProtocol->UnregisterKeyNotify (
                            gHotkeyEntry[Index].StiProtocol,
                            gHotkeyEntry[Index].NotifyHandle
                            );
  }

  // release SimpleTextOut hook
  FinalizeTextOutHook();

  // close events
  gBS->CloseEvent (gSimpleInputExEvent);

  // uninstall SnapScreen ID protocol
  gBS->UninstallProtocolInterface (
          ImageHandle,
          &gSnapScreenDriverGuid,
          NULL
          );
          
  return EFI_SUCCESS;
}

//
// Define driver entry point
//
//EFI_DRIVER_ENTRY_POINT (SnapScreenEntryPoint);

EFI_STATUS
EFIAPI
SnapScreenEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS             Status;
  UINTN                  Index;
  UINTN                  HandleCount;
  EFI_HANDLE             *HandleBuffer;
  
  EFI_LOADED_IMAGE_PROTOCOL           *LoadedImage;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL   *StiProtocol;

  // Check SnapScreen driver is loaded.
  Status = LocateDriverIndex (&gSnapScreenDriverGuid);

  if (Status == EFI_SUCCESS)
    return EFI_ALREADY_STARTED;
  
  // Register unload function
  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage);
  if (Status == EFI_SUCCESS)
    LoadedImage->Unload = UnloadSnapDrv;

  //2 Register Hot Key on EXIST simple_text_input_protocol
  Status = gBS->LocateHandleBuffer (
                        ByProtocol,
                        &gEfiSimpleTextInputExProtocolGuid,
                        NULL,
                        &HandleCount,
                        &HandleBuffer
                        );

  if (Status == EFI_SUCCESS) {
    for (Index=0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                        HandleBuffer[Index], 
                        &gEfiSimpleTextInputExProtocolGuid,
                        (VOID **)&StiProtocol
                        );
      if (EFI_ERROR(Status))
        continue;

      RegisterSnapHotKey (StiProtocol, SNAP_HOTKEY);
    }

    gBS->FreePool (HandleBuffer);
  }

  //2 Register SimpleTextInEx protocol notify for "Active Hotkey"
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SimpleInputExCallback,
                  NULL,
                  &gSimpleInputExEvent
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSimpleTextInputExProtocolGuid,
                  gSimpleInputExEvent,
                  &gStiRegistration
                  );

  //2 Install a Identify protocol
  gBS->InstallProtocolInterface (
                &ImageHandle,
                &gSnapScreenDriverGuid,
                EFI_NATIVE_INTERFACE,
                NULL
                );

  //
  // Initialize module
  //
  InitializeTextOutHook();
  
  return Status;
}

