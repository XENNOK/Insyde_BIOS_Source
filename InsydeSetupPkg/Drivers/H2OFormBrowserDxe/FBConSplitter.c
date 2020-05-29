/** @file
  Form browser console splitter for H2O form browser

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

#include "FBConSplitter.h"

EFI_SIMPLE_TEXT_INPUT_PROTOCOL             *mOriginalConIn;
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL          *mOriginalTextInEx;
EFI_SIMPLE_POINTER_PROTOCOL                *mOriginalSimplePointer;
EFI_ABSOLUTE_POINTER_PROTOCOL              *mOriginalAbsolutePointer;

CONSOLE_IN_HOT_PLUG_PRIVATE_DATA           mFBConIn = {
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_SIGNATURE,
  {
    FBTextInReset,
    FBTextInReadKeyStroke,
    (EFI_EVENT) NULL
  },
  0,
  NULL,
  0,
  {
    FBTextInExReset,
    FBTextInExReadKeyStrokeEx,
    (EFI_EVENT) NULL,
    FBTextInExSetState,
    FBTextInExRegisterKeyNotify,
    FBTextInExUnregisterKeyNotify
  },
  0,
  NULL,
  0,
  {
   (LIST_ENTRY *) NULL,
   (LIST_ENTRY *) NULL
  },
  0,
  {
    FBSimplePointerReset,
    FBSimplePointerGetState,
    (EFI_EVENT) NULL,
    (EFI_SIMPLE_POINTER_MODE *) NULL
  },
  {
    0x10000,
    0x10000,
    0x10000,
    TRUE,
    TRUE
  },
  0,
  (EFI_SIMPLE_POINTER_PROTOCOL **) NULL,
  0,
  {
    FBAbsolutePointerReset,
    FBAbsolutePointerGetState,
    (EFI_EVENT) NULL,
    (EFI_ABSOLUTE_POINTER_MODE *) NULL
  },
  {
    0,
    0,
    0,
    0x10000,
    0x10000,
    0x10000,
    0
  },
  0,
  (EFI_ABSOLUTE_POINTER_PROTOCOL **) NULL,
  0,
  FALSE,
  FALSE,
  FALSE
  };

EFI_UGA_DRAW_PROTOCOL                      *mOriginalUgaDraw;
EFI_GRAPHICS_OUTPUT_PROTOCOL               *mOriginalGraphicsOutput;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL            *mOriginalConOut;
CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA          mFBConOut = {
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA_SIGNATURE,
  {
    FBTextOutReset,
    FBTextOutOutputString,
    FBTextOutTestString,
    FBTextOutQueryMode,
    FBTextOutSetMode,
    FBTextOutSetAttribute,
    FBTextOutClearScreen,
    FBTextOutSetCursorPosition,
    FBTextOutEnableCursor,
    (EFI_SIMPLE_TEXT_OUTPUT_MODE *) NULL
  },
  {
    1,
    0,
    0,
    0,
    0,
    FALSE,
  },
  {
    FBUgaDrawGetMode,
    FBUgaDrawSetMode,
    FBUgaDrawBlt
  },
  0,
  0,
  0,
  0,
  {
    FBGraphicsOutputQueryMode,
    FBGraphicsOutputSetMode,
    FBGraphicsOutputBlt,
    NULL
  },
  0,
  0,
  0,
  (TEXT_OUT_AND_GOP_DATA *) NULL,
  0
  };


EFI_STATUS
FBGrowBuffer (
  IN     UINT32                            ElementSize,
  IN OUT UINT32                            *Count,
  IN OUT VOID                              **Buffer
  )
{
  VOID                                     *Ptr;

  //
  // Grow the buffer to new buffer size,
  // copy the old buffer's content to the new-size buffer,
  // then free the old buffer.
  //
  Ptr = ReallocatePool (
          (UINTN) (ElementSize * (*Count)),
          (UINTN) (ElementSize * ((*Count) + HOT_PLUG_ALLOC_UNIT)),
          *Buffer
          );
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  *Count += HOT_PLUG_ALLOC_UNIT;
  *Buffer = Ptr;
  return EFI_SUCCESS;
}

VOID
EFIAPI
FBTextInWaitForKey (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  )
{
  EFI_STATUS                               Status;
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  UINT32                                   Index;

  Private = (CONSOLE_IN_HOT_PLUG_PRIVATE_DATA *) Context;

  if (Private->KeyEventSignalState) {
    //
    // If KeyEventSignalState is flagged before, and not cleared by Reset() or ReadKeyStroke()
    //
    gBS->SignalEvent (Event);
    return ;
  }

  //
  // If any physical console input device has key input, signal the event.
  //
  Status = EFI_NOT_READY;
  for (Index = 0; Index < Private->CurrentNumOfTextIn; Index++) {
    Status = gBS->CheckEvent (Private->TextInList[Index]->WaitForKey);
    if (!EFI_ERROR (Status)) {
      gBS->SignalEvent (Event);
      Private->KeyEventSignalState = TRUE;
    }
  }

  Status = gBS->CheckEvent (mOriginalConIn->WaitForKey);
  if (!EFI_ERROR (Status)) {
    gBS->SignalEvent (Event);
    Private->KeyEventSignalState = TRUE;
  }
}

EFI_STATUS
EFIAPI
FBTextInReset (
  IN EFI_SIMPLE_TEXT_INPUT_PROTOCOL        *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  EFI_STATUS                               Status;
  EFI_STATUS                               ReturnStatus;
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  UINT32                                   Index;

  Private                       = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_TEXT_IN (This);
  Private->KeyEventSignalState  = FALSE;

  //
  // return the worst status met
  //
  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumOfTextIn; Index++) {
    Status = Private->TextInList[Index]->Reset (
                                           Private->TextInList[Index],
                                           ExtendedVerification
                                           );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Status = mOriginalConIn->Reset (mOriginalConIn, ExtendedVerification);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBTextInReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL       *This,
  OUT EFI_INPUT_KEY                        *Key
  )
{
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  EFI_STATUS                               Status;
  UINT32                                   Index;
  EFI_INPUT_KEY                            CurrentKey;

  Private                      = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_TEXT_IN (This);
  Private->KeyEventSignalState = FALSE;

  Key->UnicodeChar = 0;
  Key->ScanCode    = SCAN_NULL;

  //
  // if no physical console input device exists, return EFI_NOT_READY;
  // if any physical console input device has key input,
  // return the key and EFI_SUCCESS.
  //
  for (Index = 0; Index < Private->CurrentNumOfTextIn; Index++) {
    Status = Private->TextInList[Index]->ReadKeyStroke (
                                           Private->TextInList[Index],
                                           &CurrentKey
                                           );
    if (!EFI_ERROR (Status)) {
      CopyMem (Key, &CurrentKey, sizeof (EFI_INPUT_KEY));
      return Status;
    }
  }

  Status = mOriginalConIn->ReadKeyStroke (mOriginalConIn, &CurrentKey);
  if (!EFI_ERROR (Status)) {
    CopyMem (Key, &CurrentKey, sizeof (EFI_INPUT_KEY));
    return Status;
  }

  return EFI_NOT_READY;
}

EFI_STATUS
EFIAPI
FBTextInExReset (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  EFI_STATUS                               Status;
  EFI_STATUS                               ReturnStatus;
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  UINT32                                   Index;

  Private                       = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_TEXT_IN_EX (This);
  Private->KeyEventSignalState  = FALSE;

  //
  // return the worst status met
  //
  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumOfTextInEx; Index++) {
    Status = Private->TextInExList[Index]->Reset (
                                             Private->TextInExList[Index],
                                             ExtendedVerification
                                             );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Status = mOriginalTextInEx->Reset (mOriginalTextInEx, ExtendedVerification);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBTextInExReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
  OUT EFI_KEY_DATA                         *KeyData
  )
{
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  EFI_STATUS                               Status;
  UINT32                                   Index;
  EFI_KEY_DATA                             CurrentKeyData;


  if (KeyData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private                      = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_TEXT_IN_EX (This);
  Private->KeyEventSignalState = FALSE;

  KeyData->Key.UnicodeChar  = 0;
  KeyData->Key.ScanCode     = SCAN_NULL;

  //
  // if no physical console input device exists, return EFI_NOT_READY;
  // if any physical console input device has key input,
  // return the key and EFI_SUCCESS.
  //
  for (Index = 0; Index < Private->CurrentNumOfTextInEx; Index++) {
    Status = Private->TextInExList[Index]->ReadKeyStrokeEx (
                                             Private->TextInExList[Index],
                                             &CurrentKeyData
                                             );
    if (!EFI_ERROR (Status)) {
      CopyMem (KeyData, &CurrentKeyData, sizeof (CurrentKeyData));
      return Status;
    }
  }

  Status = mOriginalTextInEx->ReadKeyStrokeEx (mOriginalTextInEx, &CurrentKeyData);
  if (!EFI_ERROR (Status)) {
    CopyMem (KeyData, &CurrentKeyData, sizeof (CurrentKeyData));
    return Status;
  }

  return EFI_NOT_READY;
}

EFI_STATUS
EFIAPI
FBTextInExSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN EFI_KEY_TOGGLE_STATE                  *KeyToggleState
  )
{
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  EFI_STATUS                               Status;
  UINT32                                   Index;

  if (KeyToggleState == NULL || (UINTN) This == (UINTN) KeyToggleState) {
    return EFI_INVALID_PARAMETER;
  }

  Private = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_TEXT_IN_EX (This);

  //
  // if no physical console input device exists, return EFI_SUCCESS;
  // otherwise return the status of setting state of physical console input device
  //
  for (Index = 0; Index < Private->CurrentNumOfTextInEx; Index++) {
    Status = Private->TextInExList[Index]->SetState (
                                             Private->TextInExList[Index],
                                             KeyToggleState
                                             );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if ((*KeyToggleState & EFI_TOGGLE_STATE_VALID) == EFI_TOGGLE_STATE_VALID) {
    Private->KeyToggleState = *KeyToggleState;
  }

  Status = mOriginalTextInEx->SetState (mOriginalTextInEx, KeyToggleState);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FBTextInExRegisterKeyNotify (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
  IN  EFI_KEY_DATA                         *KeyData,
  IN  EFI_KEY_NOTIFY_FUNCTION              KeyNotificationFunction,
  OUT EFI_HANDLE                           *NotifyHandle
  )
{
  //
  // BUGBUG:
  // It can register key notify, but it can not unregister key notify.
  // Because some key notify are registered in Consplitter and FB doesn't have the NotifyHandle.
  // So, temporarily unspport RegisterKeyNotify/nregisterKeyNotify funcstions.
  //
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FBTextInExUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN EFI_HANDLE                            NotificationHandle
  )
{
  //
  // BUGBUG:
  // It can not unregister key notify.
  // Because some key notify are registered in Consplitter and FB doesn't have the NotifyHandle.
  // So, temporarily unspport RegisterKeyNotify/nregisterKeyNotify funcstions.
  //
  return EFI_UNSUPPORTED;
}

VOID
EFIAPI
FBSimplePointerWaitForInput (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  )
{
  EFI_STATUS                               Status;
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  UINT32                                   Index;

  Private = (CONSOLE_IN_HOT_PLUG_PRIVATE_DATA *) Context;

  //
  // if InputEventSignalState is flagged before, and not cleared by Reset() or ReadKeyStroke()
  //
  if (Private->InputEventSignalState) {
    gBS->SignalEvent (Event);
    return ;
  }

  //
  // if any physical console input device has key input, signal the event.
  //
  for (Index = 0; Index < Private->CurrentNumOfSimplePointer; Index++) {
    Status = gBS->CheckEvent (Private->SimplePointerList[Index]->WaitForInput);
    if (!EFI_ERROR (Status)) {
      gBS->SignalEvent (Event);
      Private->InputEventSignalState = TRUE;
    }
  }

  Status = gBS->CheckEvent (mOriginalSimplePointer->WaitForInput);
  if (!EFI_ERROR (Status)) {
    gBS->SignalEvent (Event);
    Private->KeyEventSignalState = TRUE;
  }
}

EFI_STATUS
EFIAPI
FBSimplePointerReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL           *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  EFI_STATUS                               Status;
  EFI_STATUS                               ReturnStatus;
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  UINT32                                   Index;

  Private                         = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_SIMPLE_POINTER (This);
  Private->InputEventSignalState  = FALSE;

  if (Private->CurrentNumOfSimplePointer == 0) {
    return EFI_SUCCESS;
  }
  //
  // return the worst status met
  //
  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumOfSimplePointer; Index++) {
    Status = Private->SimplePointerList[Index]->Reset (
                                            Private->SimplePointerList[Index],
                                            ExtendedVerification
                                            );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Status = mOriginalSimplePointer->Reset (mOriginalSimplePointer, ExtendedVerification);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBSimplePointerGetState (
  IN     EFI_SIMPLE_POINTER_PROTOCOL       *This,
  IN OUT EFI_SIMPLE_POINTER_STATE          *State
  )
{
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  EFI_STATUS                               Status;
  EFI_STATUS                               ReturnStatus;
  UINT32                                   Index;
  EFI_SIMPLE_POINTER_STATE                 CurrentState;

  Private                        = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_SIMPLE_POINTER (This);
  Private->InputEventSignalState = FALSE;

  State->RelativeMovementX  = 0;
  State->RelativeMovementY  = 0;
  State->RelativeMovementZ  = 0;
  State->LeftButton         = FALSE;
  State->RightButton        = FALSE;

  //
  // if no physical console input device exists, return EFI_NOT_READY;
  // if any physical console input device has key input,
  // return the key and EFI_SUCCESS.
  //
  ReturnStatus = EFI_NOT_READY;
  for (Index = 0; Index < Private->CurrentNumOfSimplePointer; Index++) {
    Status = Private->SimplePointerList[Index]->GetState (
                                            Private->SimplePointerList[Index],
                                            &CurrentState
                                            );
    if (!EFI_ERROR (Status)) {
      if (ReturnStatus == EFI_NOT_READY) {
        ReturnStatus = EFI_SUCCESS;
      }

      if (CurrentState.LeftButton) {
        State->LeftButton = TRUE;
      }

      if (CurrentState.RightButton) {
        State->RightButton = TRUE;
      }

      if (CurrentState.RelativeMovementX != 0 && Private->SimplePointerList[Index]->Mode->ResolutionX != 0) {
        State->RelativeMovementX += (CurrentState.RelativeMovementX * (INT32) Private->SimplePointerMode.ResolutionX) / (INT32) Private->SimplePointerList[Index]->Mode->ResolutionX;
      }

      if (CurrentState.RelativeMovementY != 0 && Private->SimplePointerList[Index]->Mode->ResolutionY != 0) {
        State->RelativeMovementY += (CurrentState.RelativeMovementY * (INT32) Private->SimplePointerMode.ResolutionY) / (INT32) Private->SimplePointerList[Index]->Mode->ResolutionY;
      }

      if (CurrentState.RelativeMovementZ != 0 && Private->SimplePointerList[Index]->Mode->ResolutionZ != 0) {
        State->RelativeMovementZ += (CurrentState.RelativeMovementZ * (INT32) Private->SimplePointerMode.ResolutionZ) / (INT32) Private->SimplePointerList[Index]->Mode->ResolutionZ;
      }
    } else if (Status == EFI_DEVICE_ERROR) {
      ReturnStatus = EFI_DEVICE_ERROR;
    }
  }

  Status = mOriginalSimplePointer->GetState (mOriginalSimplePointer, State);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

VOID
EFIAPI
FBAbsolutePointerWaitForInput (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  )
{
  EFI_STATUS                               Status;
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  UINT32                                   Index;

  Private = (CONSOLE_IN_HOT_PLUG_PRIVATE_DATA *) Context;

  //
  // if AbsoluteInputEventSignalState is flagged before,
  // and not cleared by Reset() or GetState(), signal it
  //
  if (Private->AbsoluteInputEventSignalState) {
    gBS->SignalEvent (Event);
    return ;
  }
  //
  // if any physical console input device has key input, signal the event.
  //
  for (Index = 0; Index < Private->CurrentNumOfAbsolutePointer; Index++) {
    Status = gBS->CheckEvent (Private->AbsolutePointerList[Index]->WaitForInput);
    if (!EFI_ERROR (Status)) {
      gBS->SignalEvent (Event);
      Private->AbsoluteInputEventSignalState = TRUE;
    }
  }

  Status = gBS->CheckEvent (mOriginalAbsolutePointer->WaitForInput);
  if (!EFI_ERROR (Status)) {
    gBS->SignalEvent (Event);
    Private->AbsoluteInputEventSignalState = TRUE;
  }
}

EFI_STATUS
EFIAPI
FBAbsolutePointerReset (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL         *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  EFI_STATUS                               Status;
  EFI_STATUS                               ReturnStatus;
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  UINT32                                   Index;

  Private                                = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_ABSOLUTE_POINTER (This);
  Private->AbsoluteInputEventSignalState = FALSE;

  if (Private->CurrentNumOfAbsolutePointer == 0) {
    return EFI_SUCCESS;
  }
  //
  // return the worst status met
  //
  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumOfAbsolutePointer; Index++) {
    Status = Private->AbsolutePointerList[Index]->Reset (
                                                    Private->AbsolutePointerList[Index],
                                                    ExtendedVerification
                                                    );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Status = mOriginalAbsolutePointer->Reset (mOriginalAbsolutePointer, ExtendedVerification);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBAbsolutePointerGetState (
  IN     EFI_ABSOLUTE_POINTER_PROTOCOL     *This,
  IN OUT EFI_ABSOLUTE_POINTER_STATE        *State
  )
{
  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA         *Private;
  EFI_STATUS                               Status;
  EFI_STATUS                               ReturnStatus;
  UINT32                                   Index;
  EFI_ABSOLUTE_POINTER_STATE               CurrentState;
  UINT64                                   Factor;


  Private                                = CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_ABSOLUTE_POINTER (This);
  Private->AbsoluteInputEventSignalState = FALSE;

  State->CurrentX                        = 0;
  State->CurrentY                        = 0;
  State->CurrentZ                        = 0;
  State->ActiveButtons                   = 0;

  //
  // if no physical pointer device exists, return EFI_NOT_READY;
  // if any physical pointer device has changed state,
  // return the state and EFI_SUCCESS.
  //
  ReturnStatus = EFI_NOT_READY;
  for (Index = 0; Index < Private->CurrentNumOfAbsolutePointer; Index++) {
    Status = Private->AbsolutePointerList[Index]->GetState (
                                                    Private->AbsolutePointerList[Index],
                                                    &CurrentState
                                                    );
    if (!EFI_ERROR (Status)) {
      if (ReturnStatus == EFI_NOT_READY) {
        ReturnStatus = EFI_SUCCESS;
      }

      State->ActiveButtons = CurrentState.ActiveButtons;

      if (!(Private->AbsolutePointerMode.AbsoluteMinX == 0 && Private->AbsolutePointerMode.AbsoluteMaxX == 0)) {
        if (!(Private->AbsolutePointerList[Index]->Mode->AbsoluteMinX == 0 && Private->AbsolutePointerList[Index]->Mode->AbsoluteMaxX == 0)) {
          Factor = DivU64x32 (LShiftU64 ((UINT64)Private->AbsolutePointerMode.AbsoluteMaxX, 32), (UINT32)(Private->AbsolutePointerList[Index]->Mode->AbsoluteMaxX - Private->AbsolutePointerList[Index]->Mode->AbsoluteMinX));
          State->CurrentX = (UINTN)(RShiftU64 (MultU64x32 (Factor, (UINT32)(CurrentState.CurrentX - Private->AbsolutePointerList[Index]->Mode->AbsoluteMinX)), 32));
        }
      }
      if (!(Private->AbsolutePointerMode.AbsoluteMinY == 0 && Private->AbsolutePointerMode.AbsoluteMaxY == 0)) {
        if (!(Private->AbsolutePointerList[Index]->Mode->AbsoluteMinY == 0 && Private->AbsolutePointerList[Index]->Mode->AbsoluteMaxY == 0)) {
          Factor = DivU64x32 (LShiftU64 ((UINT64)Private->AbsolutePointerMode.AbsoluteMaxY, 32), (UINT32)(Private->AbsolutePointerList[Index]->Mode->AbsoluteMaxY - Private->AbsolutePointerList[Index]->Mode->AbsoluteMinY));
          State->CurrentY = (UINTN)(RShiftU64 (MultU64x32 (Factor, (UINT32)(CurrentState.CurrentY - Private->AbsolutePointerList[Index]->Mode->AbsoluteMinY)), 32));
        }
      }
      if (!(Private->AbsolutePointerMode.AbsoluteMinZ == 0 && Private->AbsolutePointerMode.AbsoluteMaxZ == 0)) {
        if (!(Private->AbsolutePointerList[Index]->Mode->AbsoluteMinZ == 0 && Private->AbsolutePointerList[Index]->Mode->AbsoluteMaxZ == 0)) {
          Factor = DivU64x32 (LShiftU64 ((UINT64)Private->AbsolutePointerMode.AbsoluteMaxZ, 32), (UINT32)(Private->AbsolutePointerList[Index]->Mode->AbsoluteMaxZ - Private->AbsolutePointerList[Index]->Mode->AbsoluteMinZ));
          State->CurrentZ = (UINTN)(RShiftU64 (MultU64x32 (Factor, (UINT32)(CurrentState.CurrentZ - Private->AbsolutePointerList[Index]->Mode->AbsoluteMinZ)), 32));
        }
      }
    } else if (Status == EFI_DEVICE_ERROR) {
      ReturnStatus = EFI_DEVICE_ERROR;
    }
  }

  Status = mOriginalAbsolutePointer->GetState (mOriginalAbsolutePointer, State);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
FBAddDeviceForTextIn (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL           *TextIn;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiSimpleTextInProtocolGuid, (VOID **) &TextIn);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (mFBConIn.CurrentNumOfTextIn >= mFBConIn.TextInListCount) {
    Status = FBGrowBuffer (
               sizeof (EFI_SIMPLE_TEXT_INPUT_PROTOCOL *),
               &mFBConIn.TextInListCount,
               (VOID **) &mFBConIn.TextInList
               );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  mFBConIn.TextInList[mFBConIn.CurrentNumOfTextIn] = TextIn;
  mFBConIn.CurrentNumOfTextIn++;

  //
  // Extra CheckEvent added to reduce the double CheckEvent().
  //
  gBS->CheckEvent (TextIn->WaitForKey);

  return EFI_SUCCESS;
}

EFI_STATUS
FBAddDeviceForTextInEx (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL        *TextInEx;
  EFI_KEY_TOGGLE_STATE                     KeyToggleState;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID **) &TextInEx);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (mFBConIn.CurrentNumOfTextInEx >= mFBConIn.TextInExListCount) {
    Status = FBGrowBuffer (
               sizeof (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *),
               &mFBConIn.TextInExListCount,
               (VOID **) &mFBConIn.TextInExList
               );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  mFBConIn.TextInExList[mFBConIn.CurrentNumOfTextInEx] = TextInEx;
  mFBConIn.CurrentNumOfTextInEx++;

  //
  // Synchronize toggle state for current input device.
  //
  if (mFBConIn.KeyToggleState) {
    KeyToggleState = mFBConIn.KeyToggleState;
    TextInEx->SetState (
                TextInEx,
                &KeyToggleState
                );
  }

  //
  // Extra CheckEvent added to reduce the double CheckEvent().
  //
  gBS->CheckEvent (TextInEx->WaitForKeyEx);

  return EFI_SUCCESS;
}

EFI_STATUS
FBAddDeviceForSimplePointer (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  EFI_SIMPLE_POINTER_PROTOCOL              *SimplePointer;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiSimplePointerProtocolGuid, (VOID **) &SimplePointer);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (mFBConIn.CurrentNumOfSimplePointer >= mFBConIn.SimplePointerListCount) {
    Status = FBGrowBuffer (
              sizeof (EFI_SIMPLE_POINTER_PROTOCOL *),
              &mFBConIn.SimplePointerListCount,
              (VOID **) &mFBConIn.SimplePointerList
              );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  mFBConIn.SimplePointerList[mFBConIn.CurrentNumOfSimplePointer] = SimplePointer;
  mFBConIn.CurrentNumOfSimplePointer++;

  return EFI_SUCCESS;
}

EFI_STATUS
FBAddDeviceForAbsolutePointer (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  EFI_ABSOLUTE_POINTER_PROTOCOL            *AbsolutePointer;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiAbsolutePointerProtocolGuid, (VOID **) &AbsolutePointer);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (mFBConIn.CurrentNumOfAbsolutePointer >= mFBConIn.AbsolutePointerListCount) {
    Status = FBGrowBuffer (
               sizeof (EFI_ABSOLUTE_POINTER_PROTOCOL *),
               &mFBConIn.AbsolutePointerListCount,
               (VOID **) &mFBConIn.AbsolutePointerList
               );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  mFBConIn.AbsolutePointerList[mFBConIn.CurrentNumOfAbsolutePointer] = AbsolutePointer;
  mFBConIn.CurrentNumOfAbsolutePointer++;

  return EFI_SUCCESS;
}

EFI_STATUS
FBAddDeviceForConIn (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  FBAddDeviceForTextIn (ControllerHandle);
  FBAddDeviceForTextInEx (ControllerHandle);
  FBAddDeviceForSimplePointer (ControllerHandle);
  FBAddDeviceForAbsolutePointer (ControllerHandle);

  return EFI_SUCCESS;
}

EFI_STATUS
FBDeleteDeviceForTextIn (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL           *TextIn;
  UINT32                                   Index;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiSimpleTextInProtocolGuid, (VOID **) &TextIn);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Remove the specified text-in device data structure from the Text In List,
  // and rearrange the remaining data structures in the Text In List.
  //
  for (Index = 0; Index < mFBConIn.CurrentNumOfTextIn; Index++) {
    if (mFBConIn.TextInList[Index] == TextIn) {
      for (; Index < mFBConIn.CurrentNumOfTextIn - 1; Index++) {
        mFBConIn.TextInList[Index] = mFBConIn.TextInList[Index + 1];
      }

      mFBConIn.CurrentNumOfTextIn--;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
FBDeleteDeviceForTextInEx (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL        *TextInEx;
  UINT32                                   Index;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID **) &TextInEx);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Remove the specified text-in device data structure from the Text Input Ex List,
  // and rearrange the remaining data structures in the Text In List.
  //
  for (Index = 0; Index < mFBConIn.CurrentNumOfTextInEx; Index++) {
    if (mFBConIn.TextInExList[Index] == TextInEx) {
      for (; Index < mFBConIn.CurrentNumOfTextInEx - 1; Index++) {
        mFBConIn.TextInExList[Index] = mFBConIn.TextInExList[Index + 1];
      }

      mFBConIn.CurrentNumOfTextInEx--;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
FBDeleteDeviceForSimplePointer (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  UINT32                                   Index;
  EFI_SIMPLE_POINTER_PROTOCOL              *SimplePointer;
  EFI_SIMPLE_POINTER_MODE                  *SimplePointerMode;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiSimplePointerProtocolGuid, (VOID **) &SimplePointer);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Remove the specified text-in device data structure from the Simple Pointer List,
  // and rearrange the remaining data structures in the Text In List.
  //
  for (Index = 0; Index < mFBConIn.CurrentNumOfSimplePointer; Index++) {
    if (mFBConIn.SimplePointerList[Index] == SimplePointer) {
      for (; Index < mFBConIn.CurrentNumOfSimplePointer - 1; Index++) {
        mFBConIn.SimplePointerList[Index] = mFBConIn.SimplePointerList[Index + 1];
      }

      mFBConIn.CurrentNumOfSimplePointer--;

      if (mFBConIn.CurrentNumOfSimplePointer == 0) {
        SimplePointerMode = mOriginalSimplePointer->Mode;
      } else {
        SimplePointerMode = mFBConIn.SimplePointerList[0]->Mode;
      }
      CopyMem (mFBConIn.SimplePointer.Mode, SimplePointerMode, sizeof(EFI_SIMPLE_POINTER_MODE));

      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
FBDeleteDeviceForAbsolutePointer (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  UINT32                                   Index;
  EFI_ABSOLUTE_POINTER_PROTOCOL            *AbsolutePointer;
  EFI_ABSOLUTE_POINTER_MODE                *AbsoluteMode;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiAbsolutePointerProtocolGuid, (VOID **) &AbsolutePointer);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Remove the specified text-in device data structure from the Absolute Pointer List,
  // and rearrange the remaining data structures from the Absolute Pointer List.
  //
  for (Index = 0; Index < mFBConIn.CurrentNumOfAbsolutePointer; Index++) {
    if (mFBConIn.AbsolutePointerList[Index] == AbsolutePointer) {
      for (; Index < mFBConIn.CurrentNumOfAbsolutePointer - 1; Index++) {
        mFBConIn.AbsolutePointerList[Index] = mFBConIn.AbsolutePointerList[Index + 1];
      }

      mFBConIn.CurrentNumOfAbsolutePointer--;

      if (mFBConIn.CurrentNumOfAbsolutePointer == 0) {
        AbsoluteMode = mOriginalAbsolutePointer->Mode;
      } else {
        AbsoluteMode = mFBConIn.AbsolutePointerList[0]->Mode;
      }
      CopyMem (mFBConIn.AbsolutePointer.Mode, AbsoluteMode, sizeof(EFI_ABSOLUTE_POINTER_MODE));

      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
FBDeleteDeviceForConIn (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  FBDeleteDeviceForTextIn (ControllerHandle);
  FBDeleteDeviceForTextInEx (ControllerHandle);
  FBDeleteDeviceForSimplePointer (ControllerHandle);
  FBDeleteDeviceForAbsolutePointer (ControllerHandle);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FBTextOutReset (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FBTextOutOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN CHAR16                                *WString
  )
{
  EFI_STATUS                               Status;
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA        *Private;
  UINT32                                   Index;
  EFI_STATUS                               ReturnStatus;
  UINTN                                    MaxColumn;
  UINTN                                    MaxRow;

  This->SetAttribute (This, This->Mode->Attribute);

  Private = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_TEXT_OUT (This);

  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumberOfConsoles; Index++) {
    Status = Private->TextOutList[Index].TextOut->OutputString (
                                                    Private->TextOutList[Index].TextOut,
                                                    WString
                                                    );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Status = mOriginalConOut->OutputString (mOriginalConOut, WString);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  if (Private->CurrentNumberOfConsoles > 0) {
    Private->TextOutMode.CursorColumn = Private->TextOutList[0].TextOut->Mode->CursorColumn;
    Private->TextOutMode.CursorRow    = Private->TextOutList[0].TextOut->Mode->CursorRow;
  } else {
    //
    // When there is no real console devices in system,
    // update cursor position for the virtual device in consplitter.
    //
    Private->TextOut.QueryMode (
                       &Private->TextOut,
                       Private->TextOutMode.Mode,
                       &MaxColumn,
                       &MaxRow
                       );
    for (; *WString != CHAR_NULL; WString++) {
      switch (*WString) {
      case CHAR_BACKSPACE:
        if (Private->TextOutMode.CursorColumn == 0 && Private->TextOutMode.CursorRow > 0) {
          Private->TextOutMode.CursorRow--;
          Private->TextOutMode.CursorColumn = (INT32) (MaxColumn - 1);
        } else if (Private->TextOutMode.CursorColumn > 0) {
          Private->TextOutMode.CursorColumn--;
        }
        break;

      case CHAR_LINEFEED:
        if (Private->TextOutMode.CursorRow < (INT32) (MaxRow - 1)) {
          Private->TextOutMode.CursorRow++;
        }
        break;

      case CHAR_CARRIAGE_RETURN:
        Private->TextOutMode.CursorColumn = 0;
        break;

      default:
        if (Private->TextOutMode.CursorColumn < (INT32) (MaxColumn - 1)) {
          Private->TextOutMode.CursorColumn++;
        } else {
          Private->TextOutMode.CursorColumn = 0;
          if (Private->TextOutMode.CursorRow < (INT32) (MaxRow - 1)) {
            Private->TextOutMode.CursorRow++;
          }
        }
        break;
      }
    }
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBTextOutTestString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN CHAR16                                *WString
  )
{
  EFI_STATUS                               Status;
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA        *Private;
  UINT32                                   Index;
  EFI_STATUS                               ReturnStatus;

  Private = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_TEXT_OUT (This);

  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumberOfConsoles; Index++) {
    Status = Private->TextOutList[Index].TextOut->TestString (
                                                    Private->TextOutList[Index].TextOut,
                                                    WString
                                                    );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Status = mOriginalConOut->TestString (mOriginalConOut, WString);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBTextOutQueryMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL      *This,
  IN  UINTN                                ModeNumber,
  OUT UINTN                                *Columns,
  OUT UINTN                                *Rows
  )
{
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA        *Private;

  Private = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_TEXT_OUT (This);

  //
  // Check whether param ModeNumber is valid.
  // ModeNumber should be within range 0 ~ MaxMode - 1.
  //
  if ((ModeNumber > (UINTN) (((UINT32)-1)>>1)) ) {
    return EFI_UNSUPPORTED;
  }

  if ((INT32) ModeNumber >= This->Mode->MaxMode) {
    return EFI_UNSUPPORTED;
  }

  if (Private->CurrentNumberOfConsoles == 0) {
    mOriginalConOut->QueryMode (
                       mOriginalConOut,
                       ModeNumber,
                       Columns,
                       Rows
                       );
  } else {
    Private->TextOutList[0].TextOut->QueryMode (
                                       Private->TextOutList[0].TextOut,
                                       ModeNumber,
                                       Columns,
                                       Rows
                                       );
  }

  if (*Columns <= 0 && *Rows <= 0) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FBTextOutSetMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  UINTN                              ModeNumber
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FBTextOutSetAttribute (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  UINTN                              Attribute
  )
{
  EFI_STATUS                             Status;
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA      *Private;
  UINT32                                 Index;
  EFI_STATUS                             ReturnStatus;

  Private = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_TEXT_OUT (This);

  if ( (Attribute > (UINTN)(((UINT32)-1)>>1)) ) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumberOfConsoles; Index++) {
    Status = Private->TextOutList[Index].TextOut->SetAttribute (
                                                    Private->TextOutList[Index].TextOut,
                                                    Attribute
                                                    );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Private->TextOutMode.Attribute = (INT32) Attribute;

  Status = mOriginalConOut->SetAttribute (mOriginalConOut, Attribute);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBTextOutClearScreen (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This
  )
{
  EFI_STATUS                             Status;
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA      *Private;
  UINT32                                 Index;
  EFI_STATUS                             ReturnStatus;

  Private = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_TEXT_OUT (This);

  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumberOfConsoles; Index++) {
    Status = Private->TextOutList[Index].TextOut->ClearScreen (Private->TextOutList[Index].TextOut);
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Private->TextOutMode.CursorColumn  = 0;
  Private->TextOutMode.CursorRow     = 0;
  Private->TextOutMode.CursorVisible = TRUE;

  Status = mOriginalConOut->ClearScreen (mOriginalConOut);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBTextOutSetCursorPosition (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN UINTN                                 Column,
  IN UINTN                                 Row
  )
{
  EFI_STATUS                               Status;
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA        *Private;
  UINT32                                   Index;
  EFI_STATUS                               ReturnStatus;
  UINTN                                    MaxColumn;
  UINTN                                    MaxRow;
  INT32                                    ModeNumber;

  Private    = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_TEXT_OUT (This);
  ModeNumber = Private->TextOutMode.Mode;

  //
  // Get current MaxColumn and MaxRow from intersection map
  //
  Status = FBTextOutQueryMode (This, ModeNumber, &MaxColumn, &MaxRow);
  if (EFI_ERROR (Status)) {
   return Status;
  }

  if (Column >= MaxColumn || Row >= MaxRow) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumberOfConsoles; Index++) {
    Status = Private->TextOutList[Index].TextOut->SetCursorPosition (
                                                    Private->TextOutList[Index].TextOut,
                                                    Column,
                                                    Row
                                                    );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Private->TextOutMode.CursorColumn = (INT32) Column;
  Private->TextOutMode.CursorRow    = (INT32) Row;

  Status = mOriginalConOut->SetCursorPosition (mOriginalConOut, Column, Row);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBTextOutEnableCursor (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  BOOLEAN                            Visible
  )
{
  EFI_STATUS                             Status;
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA      *Private;
  UINT32                                 Index;
  EFI_STATUS                             ReturnStatus;

  Private = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_TEXT_OUT (This);

  //
  // return the worst status met
  //
  for (Index = 0, ReturnStatus = EFI_SUCCESS; Index < Private->CurrentNumberOfConsoles; Index++) {
    Status = Private->TextOutList[Index].TextOut->EnableCursor (
                                                    Private->TextOutList[Index].TextOut,
                                                    Visible
                                                    );
    if (EFI_ERROR (Status)) {
      ReturnStatus = Status;
    }
  }

  Private->TextOutMode.CursorVisible = Visible;

  Status = mOriginalConOut->EnableCursor (mOriginalConOut, Visible);
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBUgaDrawGetMode (
  IN  EFI_UGA_DRAW_PROTOCOL                *This,
  OUT UINT32                               *HorizontalResolution,
  OUT UINT32                               *VerticalResolution,
  OUT UINT32                               *ColorDepth,
  OUT UINT32                               *RefreshRate
  )
{
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA        *Private;
  EFI_STATUS                               Status;

  if ((HorizontalResolution == NULL) ||
      (VerticalResolution   == NULL) ||
      (RefreshRate          == NULL) ||
      (ColorDepth           == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  if (mFBConOut.CurrentNumberOfUgaDraw != 0) {
    Private               = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_UGA_DRAW (This);
    *HorizontalResolution = Private->UgaHorizontalResolution;
    *VerticalResolution   = Private->UgaVerticalResolution;
    *ColorDepth           = Private->UgaColorDepth;
    *RefreshRate          = Private->UgaRefreshRate;
  } else {
    Status = mOriginalUgaDraw->GetMode (
                                 mOriginalUgaDraw,
                                 HorizontalResolution,
                                 VerticalResolution,
                                 ColorDepth,
                                 RefreshRate
                                 );
  }

  return Status;
}

EFI_STATUS
EFIAPI
FBUgaDrawSetMode (
  IN EFI_UGA_DRAW_PROTOCOL            *This,
  IN UINT32                           HorizontalResolution,
  IN UINT32                           VerticalResolution,
  IN UINT32                           ColorDepth,
  IN UINT32                           RefreshRate
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FBUgaDrawBlt (
  IN  EFI_UGA_DRAW_PROTOCOL                *This,
  IN  EFI_UGA_PIXEL                        *BltBuffer   OPTIONAL,
  IN  EFI_UGA_BLT_OPERATION                BltOperation,
  IN  UINTN                                SourceX,
  IN  UINTN                                SourceY,
  IN  UINTN                                DestinationX,
  IN  UINTN                                DestinationY,
  IN  UINTN                                Width,
  IN  UINTN                                Height,
  IN  UINTN                                Delta        OPTIONAL
  )
{
  EFI_STATUS                               Status;
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA        *Private;
  UINT32                                   Index;
  EFI_STATUS                               ReturnStatus;
  EFI_GRAPHICS_OUTPUT_PROTOCOL             *GraphicsOutput;

  Private = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_UGA_DRAW (This);

  ReturnStatus = EFI_SUCCESS;
  //
  // return the worst status met
  //
  for (Index = 0; Index < Private->CurrentNumberOfConsoles; Index++) {
    GraphicsOutput = Private->TextOutList[Index].GraphicsOutput;
    if (GraphicsOutput != NULL) {
      Status = GraphicsOutput->Blt (
                                 GraphicsOutput,
                                 (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) BltBuffer,
                                 (EFI_GRAPHICS_OUTPUT_BLT_OPERATION) BltOperation,
                                 SourceX,
                                 SourceY,
                                 DestinationX,
                                 DestinationY,
                                 Width,
                                 Height,
                                 Delta
                                 );
      if (EFI_ERROR (Status)) {
        ReturnStatus = Status;
      } else if (BltOperation == EfiUgaVideoToBltBuffer) {
        //
        // Only need to read the data into buffer one time
        //
        return EFI_SUCCESS;
      }
    }

    if (Private->TextOutList[Index].UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
      Status = Private->TextOutList[Index].UgaDraw->Blt (
                                                      Private->TextOutList[Index].UgaDraw,
                                                      BltBuffer,
                                                      BltOperation,
                                                      SourceX,
                                                      SourceY,
                                                      DestinationX,
                                                      DestinationY,
                                                      Width,
                                                      Height,
                                                      Delta
                                                      );
      if (EFI_ERROR (Status)) {
        ReturnStatus = Status;
      } else if (BltOperation == EfiUgaVideoToBltBuffer) {
        //
        // Only need to read the data into buffer one time
        //
        return EFI_SUCCESS;
      }
    }
  }

  Status = mOriginalUgaDraw->Blt (
                               mOriginalUgaDraw,
                               BltBuffer,
                               BltOperation,
                               SourceX,
                               SourceY,
                               DestinationX,
                               DestinationY,
                               Width,
                               Height,
                               Delta
                               );
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
EFIAPI
FBGraphicsOutputQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL         *This,
  IN  UINT32                               ModeNumber,
  OUT UINTN                                *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info
  )
{
  UINT32                                   Index;
  EFI_GRAPHICS_OUTPUT_PROTOCOL             *GraphicsOutput;

  if (This == NULL || Info == NULL || SizeOfInfo == NULL || ModeNumber >= This->Mode->MaxMode) {
    return EFI_INVALID_PARAMETER;
  }

  GraphicsOutput = mOriginalGraphicsOutput;
  if (mFBConOut.CurrentNumberOfGraphicsOutput != 0) {
    for (Index = 0; Index < mFBConOut.CurrentNumberOfConsoles; Index++) {
      if (mFBConOut.TextOutList[Index].GraphicsOutput != NULL) {
        GraphicsOutput = mFBConOut.TextOutList[Index].GraphicsOutput;
        break;;
      }
    }
  }

  return GraphicsOutput->QueryMode (
                           GraphicsOutput,
                           ModeNumber,
                           SizeOfInfo,
                           Info
                           );
}

EFI_STATUS
EFIAPI
FBGraphicsOutputSetMode (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN UINT32                                ModeNumber
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FBGraphicsOutputBlt (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *BltBuffer OPTIONAL,
  IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION     BltOperation,
  IN UINTN                                 SourceX,
  IN UINTN                                 SourceY,
  IN UINTN                                 DestinationX,
  IN UINTN                                 DestinationY,
  IN UINTN                                 Width,
  IN UINTN                                 Height,
  IN UINTN                                 Delta      OPTIONAL
  )
{
  EFI_STATUS                               Status;
  EFI_STATUS                               ReturnStatus;
  CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA        *Private;
  UINT32                                   Index;
  EFI_GRAPHICS_OUTPUT_PROTOCOL             *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL                    *UgaDraw;

  if (This == NULL || ((UINTN) BltOperation) >= EfiGraphicsOutputBltOperationMax) {
    return EFI_INVALID_PARAMETER;
  }

  Private = HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_GRAPHIC_OUT (This);

  ReturnStatus = EFI_SUCCESS;

  //
  // return the worst status met
  //
  for (Index = 0; Index < Private->CurrentNumberOfConsoles; Index++) {
    GraphicsOutput = Private->TextOutList[Index].GraphicsOutput;
    if (GraphicsOutput != NULL) {
      Status = GraphicsOutput->Blt (
                                 GraphicsOutput,
                                 BltBuffer,
                                 BltOperation,
                                 SourceX,
                                 SourceY,
                                 DestinationX,
                                 DestinationY,
                                 Width,
                                 Height,
                                 Delta
                                 );
      if (EFI_ERROR (Status)) {
        ReturnStatus = Status;
      } else if (BltOperation == EfiBltVideoToBltBuffer) {
        //
        // Only need to read the data into buffer one time
        //
        return EFI_SUCCESS;
      }
    }

    UgaDraw = Private->TextOutList[Index].UgaDraw;
    if (UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
      Status = UgaDraw->Blt (
                          UgaDraw,
                          (EFI_UGA_PIXEL *) BltBuffer,
                          (EFI_UGA_BLT_OPERATION) BltOperation,
                          SourceX,
                          SourceY,
                          DestinationX,
                          DestinationY,
                          Width,
                          Height,
                          Delta
                          );
      if (EFI_ERROR (Status)) {
        ReturnStatus = Status;
      } else if (BltOperation == EfiBltVideoToBltBuffer) {
        //
        // Only need to read the data into buffer one time
        //
        return EFI_SUCCESS;
      }
    }
  }

  Status = mOriginalGraphicsOutput->Blt (
                                      mOriginalGraphicsOutput,
                                      BltBuffer,
                                      BltOperation,
                                      SourceX,
                                      SourceY,
                                      DestinationX,
                                      DestinationY,
                                      Width,
                                      Height,
                                      Delta
                                      );
  if (EFI_ERROR (Status)) {
    ReturnStatus = Status;
  }

  return ReturnStatus;
}

EFI_STATUS
FBUpdateModeDataForTextOut (
  VOID
  )
{
  EFI_SIMPLE_TEXT_OUTPUT_MODE              *Mode;

  if (mOriginalConOut == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (mFBConOut.CurrentNumberOfConsoles == 0) {
    Mode = mOriginalConOut->Mode;
  } else {
    Mode = mFBConOut.TextOutList[0].TextOut->Mode;
  }
  CopyMem (mFBConOut.TextOut.Mode, Mode, sizeof(EFI_SIMPLE_TEXT_OUTPUT_MODE));

  return EFI_SUCCESS;
}

EFI_STATUS
FBUpdateModeDataForUga (
  VOID
  )
{
  UINT32                                   Index;
  EFI_UGA_DRAW_PROTOCOL                    *UgaDraw;

  if (mOriginalUgaDraw == NULL) {
    return EFI_UNSUPPORTED;
  }

  UgaDraw = mOriginalUgaDraw;
  if (mFBConOut.CurrentNumberOfUgaDraw != 0) {
    for (Index = 0; Index < mFBConOut.CurrentNumberOfConsoles; Index++) {
      if (mFBConOut.TextOutList[Index].UgaDraw != NULL) {
        UgaDraw = mFBConOut.TextOutList[Index].UgaDraw;
        break;
      }
    }
  }

  UgaDraw->GetMode (
             UgaDraw,
             &mFBConOut.UgaHorizontalResolution,
             &mFBConOut.UgaVerticalResolution,
             &mFBConOut.UgaColorDepth,
             &mFBConOut.UgaRefreshRate
             );

  return EFI_SUCCESS;
}

EFI_STATUS
FBUpdateModeDataForGop (
  VOID
  )
{
  UINT32                                   Index;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE        *Mode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION     *Info;

  if (mOriginalGraphicsOutput == NULL) {
    return EFI_UNSUPPORTED;
  }

  Mode = mOriginalGraphicsOutput->Mode;
  if (mFBConOut.CurrentNumberOfGraphicsOutput != 0) {
    for (Index = 0; Index < mFBConOut.CurrentNumberOfConsoles; Index++) {
      if (mFBConOut.TextOutList[Index].GraphicsOutput != NULL) {
        Mode = mFBConOut.TextOutList[Index].GraphicsOutput->Mode;
        break;
      }
    }
  }

  Info = mFBConOut.GraphicsOutput.Mode->Info;
  CopyMem (mFBConOut.GraphicsOutput.Mode, Mode      , sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));
  CopyMem (Info                         , Mode->Info, sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
  mFBConOut.GraphicsOutput.Mode->Info = Info;

  return EFI_SUCCESS;
}

EFI_STATUS
FBAddDeviceForConOut (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  EFI_STATUS                               Status;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL          *TextOut;
  EFI_GRAPHICS_OUTPUT_PROTOCOL             *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL                    *UgaDraw;
  UINT32                                   CurrentNumOfConsoles;
  UINT32                                   CurrentNumOfGraphicsOutput;
  UINT32                                   CurrentNumOfUgaDraw;
  TEXT_OUT_AND_GOP_DATA                    *TextAndGop;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiSimpleTextOutProtocolGuid, (VOID **) &TextOut);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  GraphicsOutput = NULL;
  UgaDraw        = NULL;
  Status = gBS->HandleProtocol (ControllerHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status) && FeaturePcdGet (PcdUgaConsumeSupport)) {
    gBS->HandleProtocol (ControllerHandle, &gEfiUgaDrawProtocolGuid, (VOID **) &UgaDraw);
  }

  CurrentNumOfGraphicsOutput = mFBConOut.CurrentNumberOfGraphicsOutput;
  CurrentNumOfUgaDraw        = mFBConOut.CurrentNumberOfUgaDraw;
  CurrentNumOfConsoles       = mFBConOut.CurrentNumberOfConsoles;

  while (CurrentNumOfConsoles >= mFBConOut.TextOutListCount) {
    Status = FBGrowBuffer (
               sizeof (TEXT_OUT_AND_GOP_DATA),
               &mFBConOut.TextOutListCount,
               (VOID **) &mFBConOut.TextOutList
               );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  TextAndGop                 = &mFBConOut.TextOutList[CurrentNumOfConsoles];
  TextAndGop->TextOut        = TextOut;
  TextAndGop->GraphicsOutput = GraphicsOutput;
  TextAndGop->UgaDraw        = UgaDraw;

  mFBConOut.CurrentNumberOfConsoles++;
  if (CurrentNumOfConsoles == 0) {
    FBUpdateModeDataForTextOut ();
  }

  if (GraphicsOutput != NULL) {
    mFBConOut.CurrentNumberOfGraphicsOutput++;
    if (CurrentNumOfGraphicsOutput == 0) {
      FBUpdateModeDataForGop ();
    }
  }

  if (UgaDraw != NULL) {
    mFBConOut.CurrentNumberOfUgaDraw++;
    if (CurrentNumOfUgaDraw == 0) {
      FBUpdateModeDataForUga ();
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBDeleteDeviceForConOut (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  INT32                                    Index;
  UINT32                                   CurrentNumOfConsoles;
  TEXT_OUT_AND_GOP_DATA                    *TextOutList;
  EFI_STATUS                               Status;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL          *TextOut;

  Status = gBS->HandleProtocol (ControllerHandle, &gEfiSimpleTextOutProtocolGuid, (VOID **) &TextOut);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Remove the specified text-out device data structure from the Text out List,
  // and rearrange the remaining data structures in the Text out List.
  //
  CurrentNumOfConsoles  = mFBConOut.CurrentNumberOfConsoles;
  Index                 = (INT32) CurrentNumOfConsoles - 1;
  TextOutList           = mFBConOut.TextOutList;
  while (Index >= 0) {
    if (TextOutList->TextOut == TextOut) {
      if (TextOutList->UgaDraw != NULL && FeaturePcdGet (PcdUgaConsumeSupport)) {
        mFBConOut.CurrentNumberOfUgaDraw--;
      }
      if (TextOutList->GraphicsOutput != NULL) {
        mFBConOut.CurrentNumberOfGraphicsOutput--;
      }
      CopyMem (TextOutList, TextOutList + 1, sizeof (TEXT_OUT_AND_GOP_DATA) * Index);
      mFBConOut.CurrentNumberOfConsoles--;
      break;
    }

    Index--;
    TextOutList++;
  }

  if (Index < 0) {
    return EFI_NOT_FOUND;
  }

  FBUpdateModeDataForTextOut ();
  FBUpdateModeDataForUga ();
  FBUpdateModeDataForGop ();

  return EFI_SUCCESS;
}

EFI_STATUS
FBConInConstructor (
  VOID
  )
{
  EFI_STATUS                               Status;
  UINT32                                   Size;

  //
  // Simple text input protocol
  //
  Size   = sizeof (EFI_SIMPLE_TEXT_INPUT_PROTOCOL *);
  Status = FBGrowBuffer (Size, &mFBConIn.TextInListCount, (VOID **) &mFBConIn.TextInList);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = gBS->CreateEvent (EVT_NOTIFY_WAIT, TPL_NOTIFY, FBTextInWaitForKey, &mFBConIn, &mFBConIn.TextIn.WaitForKey);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mOriginalConIn = gST->ConIn;

  //
  // Simple text input ex protocol
  //
  Size   = sizeof (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *);
  Status = FBGrowBuffer (Size, &mFBConIn.TextInExListCount, (VOID **) &mFBConIn.TextInExList);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = gBS->CreateEvent (EVT_NOTIFY_WAIT, TPL_NOTIFY, FBTextInWaitForKey, &mFBConIn, &mFBConIn.TextInEx.WaitForKeyEx);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->HandleProtocol (gST->ConsoleInHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID **) &mOriginalTextInEx);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Simple pointer protocol
  //
  Size   = sizeof (EFI_SIMPLE_POINTER_PROTOCOL *);
  Status = FBGrowBuffer (Size, &mFBConIn.SimplePointerListCount, (VOID **) &mFBConIn.SimplePointerList);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = gBS->CreateEvent (EVT_NOTIFY_WAIT, TPL_NOTIFY, FBSimplePointerWaitForInput, &mFBConIn, &mFBConIn.SimplePointer.WaitForInput);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mFBConIn.SimplePointer.Mode = &mFBConIn.SimplePointerMode;
  Status = gBS->HandleProtocol (gST->ConsoleInHandle, &gEfiSimplePointerProtocolGuid, (VOID **) &mOriginalSimplePointer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Absolute pointer protocol
  //
  Size   = sizeof (EFI_ABSOLUTE_POINTER_PROTOCOL *);
  Status = FBGrowBuffer (Size, &mFBConIn.AbsolutePointerListCount, (VOID **) &mFBConIn.AbsolutePointerList);
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = gBS->CreateEvent (EVT_NOTIFY_WAIT, TPL_NOTIFY, FBAbsolutePointerWaitForInput, &mFBConIn, &mFBConIn.AbsolutePointer.WaitForInput);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mFBConIn.AbsolutePointer.Mode = &mFBConIn.AbsolutePointerMode;
  Status = gBS->HandleProtocol (gST->ConsoleInHandle, &gEfiAbsolutePointerProtocolGuid, (VOID **) &mOriginalAbsolutePointer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBConInDeconstructor (
  VOID
  )
{
  mFBConIn.TextInListCount          = 0;
  mFBConIn.TextInExListCount        = 0;
  mFBConIn.SimplePointerListCount   = 0;
  mFBConIn.AbsolutePointerListCount = 0;

  FBFreePool ((VOID **) &mFBConIn.TextInList);
  if (mFBConIn.TextIn.WaitForKey != NULL) {
    gBS->CloseEvent (mFBConIn.TextIn.WaitForKey);
    mFBConIn.TextIn.WaitForKey = NULL;
  }

  FBFreePool ((VOID **) &mFBConIn.TextInExList);
  if (mFBConIn.TextInEx.WaitForKeyEx != NULL) {
    gBS->CloseEvent (mFBConIn.TextInEx.WaitForKeyEx);
    mFBConIn.TextInEx.WaitForKeyEx = NULL;
  }

  FBFreePool ((VOID **) &mFBConIn.SimplePointerList);
  if (mFBConIn.SimplePointer.WaitForInput != NULL) {
    gBS->CloseEvent (mFBConIn.SimplePointer.WaitForInput);
    mFBConIn.SimplePointer.WaitForInput = NULL;
  }

  FBFreePool ((VOID **) &mFBConIn.AbsolutePointerList);
  if (mFBConIn.AbsolutePointer.WaitForInput != NULL) {
    gBS->CloseEvent (mFBConIn.AbsolutePointer.WaitForInput);
    mFBConIn.AbsolutePointer.WaitForInput = NULL;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBConInStart (
  VOID
  )
{
  EFI_STATUS                               Status;
  EFI_HANDLE                               ConInHandle;
  EFI_GUID                                 *Guid;
#if 0
  EFI_SIMPLE_POINTER_MODE                  *SimplePointerMode;
  EFI_ABSOLUTE_POINTER_MODE                *AbsoluteMode;
#endif

  ConInHandle = gST->ConsoleInHandle;

  //
  // Simple text input protocol
  //
  gST->ConIn = &mFBConIn.TextIn;

  //
  // Simple text input ex protocol
  //
  Guid   = &gEfiSimpleTextInputExProtocolGuid;
  Status = gBS->ReinstallProtocolInterface (ConInHandle, Guid, mOriginalTextInEx, &mFBConIn.TextInEx);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // SetupMosue will process SimplePointer and AbsolutePointer
  // so comment below reinstall produce
  //
#if 0
  //
  // Simple pointer protocol
  //
  if (mFBConIn.CurrentNumOfSimplePointer == 0) {
    SimplePointerMode = mOriginalSimplePointer->Mode;
  } else {
    SimplePointerMode = mFBConIn.SimplePointerList[0]->Mode;
  }
  CopyMem (mFBConIn.SimplePointer.Mode, SimplePointerMode, sizeof(EFI_SIMPLE_POINTER_MODE));
  Guid   = &gEfiSimplePointerProtocolGuid;
  Status = gBS->ReinstallProtocolInterface (ConInHandle, Guid, mOriginalSimplePointer, &mFBConIn.SimplePointer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Absolute pointer protocol
  //
  if (mFBConIn.CurrentNumOfAbsolutePointer == 0) {
    AbsoluteMode = mOriginalAbsolutePointer->Mode;
  } else {
    AbsoluteMode = mFBConIn.AbsolutePointerList[0]->Mode;
  }
  CopyMem (mFBConIn.AbsolutePointer.Mode, AbsoluteMode, sizeof(EFI_ABSOLUTE_POINTER_MODE));
  Guid   = &gEfiAbsolutePointerProtocolGuid;
  Status = gBS->ReinstallProtocolInterface (ConInHandle, Guid, mOriginalAbsolutePointer, &mFBConIn.AbsolutePointer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif

  return EFI_SUCCESS;
}

EFI_STATUS
FBConInStop (
  VOID
  )
{
  EFI_STATUS                               Status;
  EFI_HANDLE                               ConInHandle;
  EFI_GUID                                 *Guid;

  ConInHandle = gST->ConsoleInHandle;

  //
  // SetupMosue will process SimplePointer and AbsolutePointer
  // so comment below reinstall produce
  //
#if 0
  if (mOriginalAbsolutePointer != NULL) {
    Guid   = &gEfiAbsolutePointerProtocolGuid;
    Status = gBS->ReinstallProtocolInterface (ConInHandle, Guid, &mFBConIn.AbsolutePointer, mOriginalAbsolutePointer);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mOriginalAbsolutePointer = NULL;
  }

  if (mOriginalSimplePointer != NULL) {
    Guid   = &gEfiSimplePointerProtocolGuid;
    Status = gBS->ReinstallProtocolInterface (ConInHandle, Guid, &mFBConIn.SimplePointer, mOriginalSimplePointer);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mOriginalSimplePointer = NULL;
  }
#endif

  if (mOriginalTextInEx != NULL) {
    Guid   = &gEfiSimpleTextInputExProtocolGuid;
    Status = gBS->ReinstallProtocolInterface (ConInHandle, Guid, &mFBConIn.TextInEx, mOriginalTextInEx);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mOriginalTextInEx = NULL;
  }


  if (mOriginalConIn != NULL) {
    gST->ConIn     = mOriginalConIn;
    mOriginalConIn = NULL;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBConOutConstructor (
  VOID
  )
{
  EFI_STATUS                               Status;

  mFBConOut.TextOut.Mode = &mFBConOut.TextOutMode;

  Status = FBGrowBuffer (
             sizeof (TEXT_OUT_AND_GOP_DATA),
             &mFBConOut.TextOutListCount,
             (VOID **) &mFBConOut.TextOutList
             );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  mFBConOut.GraphicsOutput.Mode = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));
  if (mFBConOut.GraphicsOutput.Mode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  mFBConOut.GraphicsOutput.Mode->Info = AllocatePool (sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
  if (mFBConOut.GraphicsOutput.Mode->Info == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  mOriginalConOut = gST->ConOut;

  mOriginalUgaDraw = NULL;
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiUgaDrawProtocolGuid, (VOID **) &mOriginalUgaDraw);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mOriginalGraphicsOutput = NULL;
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &mOriginalGraphicsOutput);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBConOutDeconstructor (
  VOID
  )
{
  mFBConOut.TextOutListCount = 0;
  FBFreePool ((VOID **) &mFBConOut.TextOutList);

  return EFI_SUCCESS;
}

EFI_STATUS
FBConOutStartForTextOut (
  VOID
  )
{
  FBUpdateModeDataForTextOut ();

  gST->ConOut = &mFBConOut.TextOut;

  return EFI_SUCCESS;
}

EFI_STATUS
FBConOutStartForUga (
  VOID
  )
{
  EFI_STATUS                               Status;

  FBUpdateModeDataForUga ();

  Status = gBS->ReinstallProtocolInterface (
                  gST->ConsoleOutHandle,
                  &gEfiUgaDrawProtocolGuid,
                  mOriginalUgaDraw,
                  &mFBConOut.UgaDraw
                  );
  if (EFI_ERROR (Status)) {
    if (mOriginalUgaDraw != NULL) {
      Status = gBS->InstallProtocolInterface (
                      &gST->ConsoleOutHandle,
                      &gEfiUgaDrawProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mFBConOut.UgaDraw
                      );
    }
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBConOutStartForGop (
  VOID
  )
{
  EFI_STATUS                               Status;

  FBUpdateModeDataForGop ();

  Status = gBS->ReinstallProtocolInterface (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  mOriginalGraphicsOutput,
                  &mFBConOut.GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    if (mOriginalGraphicsOutput != NULL) {
      Status = gBS->InstallProtocolInterface (
                      &gST->ConsoleOutHandle,
                      &gEfiGraphicsOutputProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mFBConOut.GraphicsOutput
                      );
    }
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBConOutStart (
  VOID
  )
{
  if (FeaturePcdGet (PcdConOutUgaSupport)) {
    FBConOutStartForUga ();
  }
  if (FeaturePcdGet (PcdConOutGopSupport)) {
    FBConOutStartForGop ();
  }

  FBConOutStartForTextOut ();

  return EFI_SUCCESS;
}

EFI_STATUS
FBConOutStop (
  VOID
  )
{
  EFI_STATUS                               Status;
  EFI_HANDLE                               ConsoleOutHandle;
  EFI_GUID                                 *Guid;

  ConsoleOutHandle = gST->ConsoleOutHandle;

  if (mOriginalConOut != NULL) {
    gST->ConOut = mOriginalConOut;
    gST->ConOut->SetAttribute (gST->ConOut, (EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK));
    gST->ConOut->ClearScreen (gST->ConOut);
    mOriginalConOut = NULL;
  }

  if (mOriginalUgaDraw != NULL) {
    Guid   = &gEfiUgaDrawProtocolGuid;
    Status = gBS->ReinstallProtocolInterface (ConsoleOutHandle, Guid, &mFBConOut.UgaDraw, mOriginalUgaDraw);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mOriginalUgaDraw = NULL;
  }

  if (mOriginalGraphicsOutput != NULL) {
    FBFreePool ((VOID **) &mFBConOut.GraphicsOutput.Mode->Info);
    FBFreePool ((VOID **) &mFBConOut.GraphicsOutput.Mode);

    Guid   = &gEfiGraphicsOutputProtocolGuid;
    Status = gBS->ReinstallProtocolInterface (ConsoleOutHandle, Guid, &mFBConOut.GraphicsOutput, mOriginalGraphicsOutput);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mOriginalGraphicsOutput = NULL;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBConsplitterUpdateModeData (
  VOID
  )
{
  FBUpdateModeDataForTextOut ();
  FBUpdateModeDataForUga ();
  FBUpdateModeDataForGop ();

  return EFI_SUCCESS;
}

EFI_STATUS
FBConsplitterAddDevice (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  FBAddDeviceForConIn (ControllerHandle);
  FBAddDeviceForConOut (ControllerHandle);

  return EFI_SUCCESS;
}

EFI_STATUS
FBConsplitterDeleteDevice (
  IN EFI_HANDLE                            ControllerHandle
  )
{
  FBDeleteDeviceForConIn (ControllerHandle);
  FBDeleteDeviceForConOut (ControllerHandle);

  return EFI_SUCCESS;
}

EFI_STATUS
FBConsplitterInit (
  VOID
  )
{
  EFI_STATUS                               Status;

  Status = FBConInConstructor ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = FBConOutConstructor ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
FBConsplitterStart (
  VOID
  )
{
  FBConInStart ();
  FBConOutStart ();

  gST->Hdr.CRC32 = 0;
  gBS->CalculateCrc32 (
         (UINT8 *) &gST->Hdr,
         gST->Hdr.HeaderSize,
         &gST->Hdr.CRC32
         );

  return EFI_SUCCESS;
}

EFI_STATUS
FBConsplitterShutdown (
  VOID
  )
{
  FBConInStop ();
  FBConOutStop ();
  FBConInDeconstructor ();
  FBConOutDeconstructor ();

  gST->Hdr.CRC32 = 0;
  gBS->CalculateCrc32 (
         (UINT8 *) &gST->Hdr,
         gST->Hdr.HeaderSize,
         &gST->Hdr.CRC32
         );

  return EFI_SUCCESS;
}

