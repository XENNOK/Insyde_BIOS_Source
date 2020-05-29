/** @file
 Setup Mouse Cursor implementation

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
#include <Library/UefiHiiServicesLib.h>
extern EFI_HII_HANDLE                    mImageHiiHandle;

/**
 Initialize cursor data

 @param[in, out] Private       PRIVATE_MOUSE_DATA

 @retval EFI_SUCCESS           Initialize success
**/
EFI_STATUS
InitializeCursor (
  IN OUT PRIVATE_MOUSE_DATA             *Private
  )
{
  EFI_STATUS                            Status;
  EFI_IMAGE_INPUT                       Image;

  if (mImageHiiHandle == NULL) {
    return EFI_NOT_READY;
  }

  Status = gHiiImage->GetImage (
                        gHiiImage,
                        mImageHiiHandle,
                        IMAGE_TOKEN (IMAGE_CURSOR),
                        &Image
                        );
  if (EFI_ERROR (Status) || Image.Bitmap == NULL) {
    return Status;
  }
  if ((Image.Flags & H2O_IMAGE_ALPHA_CHANNEL) != H2O_IMAGE_ALPHA_CHANNEL) {
    ConvertToAlphaChannelImage (&Image);
  }

  SetRect (&Private->Cursor.ImageRc, 0, 0, Image.Width, Image.Height);
  Private->Cursor.Image = Image.Bitmap;

  return EFI_SUCCESS;
}

/**
 Destroy cursor data

 @param[in, out] Private       PRIVATE_MOUSE_DATA

 @retval EFI_SUCCESS           destroy success
**/
VOID
DestroyCursor (
  IN OUT PRIVATE_MOUSE_DATA                    *Private
  )
{
  DestroyImage (&Private->Cursor);
}

/**
 Move the cursor to a particular point indicated by the X, Y axis.

 @param [in]   X                indicates location to move to.
 @param [in]   Y                indicates location to move to.

 @retval None

**/
EFI_STATUS
EFIAPI
InternalSetCursorPos (
  IN  UINTN                                 X,
  IN  UINTN                                 Y
  )
{
  EFI_STATUS                  Status;
  PRIVATE_MOUSE_DATA          *Private;

  Private = mPrivate;

  ASSERT_LOCKED (&Private->SetupMouseLock);
  if (X < Private->MouseRange.StartX || Y < Private->MouseRange.StartY ||
      X > Private->MouseRange.EndX || Y > Private->MouseRange.EndY) {
    return EFI_SUCCESS;
  }

  Private->SaveCursorX = (UINT32)X;
  Private->SaveCursorY = (UINT32)Y;
  if ((INT32)X != Private->Cursor.ImageRc.left || (INT32)Y != Private->Cursor.ImageRc.top) {
    MoveImage (&Private->Cursor, X, Y);
  }

  Status = ProcessKeyboard ();
  if (!EFI_ERROR (Status)) {
    Private->HaveRawData = FALSE;
  }


  return Status;
}

/**
 According mouse / touch event to determinate cursor postion and button state

 @param [in]   Event            and Context  Not actually used in the function
 @param [in]   Context

 @retval EFI_SUCCESS            If receive mouse / touch event

**/
VOID
EFIAPI
ProcessMouse (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  EFI_STATUS                            Status;
  PRIVATE_MOUSE_DATA                    *Private;
  INTN                                  CurX, CurY;
  UINTN                                 Count;
  POINTER_PROTOCOL_INFO                 *PointerProtocolInfo;
  EFI_SIMPLE_POINTER_STATE              SimplePointerState;
  EFI_ABSOLUTE_POINTER_PROTOCOL         *AbsolutePointer;
  EFI_ABSOLUTE_POINTER_STATE            AbsolutePointerState;
  UINT64                                ScreenFactor;
  EFI_SIMPLE_POINTER_PROTOCOL           *SimplePointerPtr;
  STATIC UINTN                          TimeCount = 0;
  BOOLEAN                               ImageIsSame;
  RECT                                  InvalidateRc;
  RECT                                  Rc;
  GOP_ENTRY                             *GopEntry;
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  LIST_ENTRY                            *Node;

  Private = SETUP_MOUSE_DEV_FROM_THIS (Context);

  ASSERT (Private->IsStart);

  CurX = (INTN)Private->Cursor.ImageRc.left;
  CurY = (INTN)Private->Cursor.ImageRc.top;

  if (IsListEmpty (&Private->GopList)) {
    return ;
  }

  GopEntry = GOP_ENTRY_FROM_THIS (GetFirstNode (&Private->GopList));
  GraphicsOutput = GopEntry->GraphicsOutput;

  PointerProtocolInfo = Private->PointerProtocolInfo;
  for (Count = 0; Count < PointerProtocolInfo->Count; Count ++) {
    if ((PointerProtocolInfo->Data[Count].Attributes & ATTRIBUTE_BIT_SIMPLE_OR_ABSOLUTE) == ATTRIBUTE_VALUE_SIMPLE) {
      //
      // Use Simple Pointer protocol to update current mouse state
      //
      SimplePointerPtr = (EFI_SIMPLE_POINTER_PROTOCOL*) PointerProtocolInfo->Data[Count].PointerProtocol;
      Status = SimplePointerPtr->GetState(SimplePointerPtr, &SimplePointerState);
      if (EFI_ERROR(Status)) {
        continue;
      }
      AcquireSetupMouseLock (Private);
      ShowImage (&Private->Cursor);
      ReleaseSetupMouseLock (Private);


      CurX += (SimplePointerState.RelativeMovementX) / MOUSE_RESOLUTION;
      CurY += (SimplePointerState.RelativeMovementY) / MOUSE_RESOLUTION;
      Private->LButton = SimplePointerState.LeftButton;
      Private->RButton = SimplePointerState.RightButton;
    } else if ((PointerProtocolInfo->Data[Count].Attributes & ATTRIBUTE_BIT_SIMPLE_OR_ABSOLUTE) == ATTRIBUTE_VALUE_ABSOLUTE) {
      //
      // Use Absolute Pointer protocol to update current mouse state
      //
      AbsolutePointer = (EFI_ABSOLUTE_POINTER_PROTOCOL*) PointerProtocolInfo->Data[Count].PointerProtocol;
      Status = AbsolutePointer->GetState (AbsolutePointer, &AbsolutePointerState);
      if (EFI_ERROR (Status)) {
        continue;
      }

      if (Private->HideCursorWhenTouch) {
        AcquireSetupMouseLock (Private);
        HideImage (&Private->Cursor);
        ReleaseSetupMouseLock (Private);
      }

      ScreenFactor = DivU64x32 (LShiftU64 ((UINT64)(GraphicsOutput->Mode->Info->HorizontalResolution), 32), (UINT32)(AbsolutePointer->Mode->AbsoluteMaxX - AbsolutePointer->Mode->AbsoluteMinX));
      CurX = (UINTN)(RShiftU64 (MultU64x32 (ScreenFactor, (UINT32)(AbsolutePointerState.CurrentX - AbsolutePointer->Mode->AbsoluteMinX)), 32));
      ScreenFactor = DivU64x32 (LShiftU64 ((UINT64)(GraphicsOutput->Mode->Info->VerticalResolution), 32), (UINT32)(AbsolutePointer->Mode->AbsoluteMaxY - AbsolutePointer->Mode->AbsoluteMinY));
      CurY = (UINTN)(RShiftU64 (MultU64x32 (ScreenFactor, (UINT32)(AbsolutePointerState.CurrentY - AbsolutePointer->Mode->AbsoluteMinY)), 32));

      Private->LButton = (BOOLEAN) ((AbsolutePointerState.ActiveButtons & EFI_ABSP_TouchActive) == EFI_ABSP_TouchActive);
      Private->RButton = (BOOLEAN) ((AbsolutePointerState.ActiveButtons & EFI_ABS_AltActive) == EFI_ABS_AltActive);
    }
    Private->HaveRawData = TRUE;

    //
    // Bugbug: if event is touch, do we fix the position ?
    //
    if (CurX <= (INTN) Private->MouseRange.StartX) {
      CurX = (INTN) Private->MouseRange.StartX;
    } else if (CurX >= (INTN) Private->MouseRange.EndX) {
      CurX = (INTN)  Private->MouseRange.EndX;
    }

    if (CurY <= (INTN) Private->MouseRange.StartY) {
      CurY = (INTN) Private->MouseRange.StartY;
    } else if (CurY >= (INTN) mPrivate->MouseRange.EndY) {
      CurY = (INTN) Private->MouseRange.EndY;
    }

    SetCursorPos (CurX, CurY);
  }

  AcquireSetupMouseLock (Private);


  RenderImageForAllGop (Private);

  //
  // sync the keyboard state
  //
  UpdateKeyboardStateByBDA ();

  if (!Private->NeedSyncFrameBuffer) {
    ReleaseSetupMouseLock (Private);
    return ;
  }

  //
  // check screen per 30 ms
  //
  if (TimeCount++ < ((TICKS_PER_SECOND / 30) / MOUSE_TIMER)) {
    ReleaseSetupMouseLock (Private);
    return;
  }
  TimeCount = 0;

  ImageIsSame = FALSE;

  Node = GetFirstNode (&Private->GopList);
  while (!IsNull (&Private->GopList, Node)) {

    GopEntry = GOP_ENTRY_FROM_THIS (Node);

    //
    // sync (0,0) pixel with screen buffer for bitlocker
    //
    SetRect(&Rc, 0, 0, 1, 1);
    IntersectRect (&InvalidateRc, &Rc, &GopEntry->Screen.ImageRc);
    SyncScreenImage (Private, GopEntry, &InvalidateRc, &ImageIsSame);

    //
    // sync cursor pixel, for bitlocker read input box
    //
    IntersectRect (&InvalidateRc, &Private->Cursor.ImageRc, &GopEntry->Screen.ImageRc);
    SyncScreenImage (Private, GopEntry, &InvalidateRc, &ImageIsSame);
    if (!ImageIsSame) {
      UnionRect (&GopEntry->InvalidateRc, &GopEntry->InvalidateRc, &Private->Cursor.ImageRc);
    }

    if (Private->Keyboard.Visible) {
      IntersectRect (&InvalidateRc, &Private->Keyboard.ImageRc, &GopEntry->Screen.ImageRc);
      SyncScreenImage (Private, GopEntry, &InvalidateRc, &ImageIsSame);
      if (!ImageIsSame) {
        UnionRect (&GopEntry->InvalidateRc, &GopEntry->InvalidateRc, &Private->Keyboard.ImageRc);
      }
    }

    RenderImage (Private, GopEntry, TRUE);

    Node = GetNextNode (&Private->GopList, Node);
  }

  ReleaseSetupMouseLock (Private);
}

