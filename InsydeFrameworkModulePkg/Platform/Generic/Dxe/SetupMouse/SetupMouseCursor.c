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
//;   SetupMouseCursor.c
//;
//; Abstract:
//;
//;   Setup Mouse Cursor implementation
//;
//;
#include "SetupMouse.h"
#include "SetupMouseAniDefs.h"
#include EFI_PROTOCOL_DEFINITION (GifDecoder)

extern unsigned char                 SetupMouseAnimations[];

EFI_STATUS
InitializeCursor (
  IN PRIVATE_MOUSE_DATA                    *Private
  )
/*++

Routine Description:

  Initialize cursor data

Arguments:

  Private     - PRIVATE_MOUSE_DATA

Returns:

  EFI_SUCCESS - Initialize success

--*/
{
  UINTN                                 Width;
  UINTN                                 Height;
  EFI_HII_AIBT_EXT4_BLOCK               *Ext4Block;
  ANIMATION                             *Animation;
  EFI_STATUS                            Status;
  EFI_GIF_DECODER_PROTOCOL              *GifDecoder;
  EFI_HII_ANIMATION_PACKAGE_HDR         *AnimationPackage;

  AnimationPackage    = (EFI_HII_ANIMATION_PACKAGE_HDR *)(SetupMouseAnimations + 4);

  Status = gBS->LocateProtocol (&gEfiGifDecoderProtocolGuid, NULL, (VOID**)&GifDecoder);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Ext4Block = (EFI_HII_AIBT_EXT4_BLOCK *) SetupMouseGetAnimation (
    (UINT8 *)AnimationPackage + AnimationPackage->AnimationInfoOffset,
    (EFI_ANIMATION_ID) ANIMATION_TOKEN (ANI_CURSOR)
    );
  if (Ext4Block == NULL) {
    return EFI_NOT_FOUND;
  }
  if (*(UINT8 *)Ext4Block != EFI_HII_AIBT_EXT4) {
    return EFI_NOT_FOUND;
  }
  Status = GifDecoder->CreateAnimationFromMem (
                          GifDecoder,
                          (UINT8 *)(Ext4Block + 1),
                          Ext4Block->Length,
                          (VOID *) NULL,
                          &Animation
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Width  = Animation->Frames->Width;
  Height = Animation->Frames->Height;
  SetRect (
    &Private->Cursor.ImageRc,
    (INT32)Private->MouseRange.StartX,
    (INT32)Private->MouseRange.StartY,
    (INT32)(Private->MouseRange.StartX + Width),
    (INT32)(Private->MouseRange.StartY + Height)
    );

  Private->Cursor.Image = EfiLibAllocateZeroPool (sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height);
  EfiCopyMem (Private->Cursor.Image, Animation->Frames->Bitmap, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * Width * Height);

  GifDecoder->DestroyAnimation (GifDecoder, Animation);

  return EFI_SUCCESS;
}

VOID
DestroyCursor (
  PRIVATE_MOUSE_DATA                    *Private
  )
/*++

Routine Description:

  Destroy cursor data

Arguments:

  Private     - PRIVATE_MOUSE_DATA

Returns:

  EFI_SUCCESS - destroy success

--*/
{
  DestroyImage (&Private->Cursor);
}

EFI_STATUS
CursorGotoXY (
  IN  UINTN                                 X,
  IN  UINTN                                 Y
  )
/*++

Routine Description:

  Move the cursor to a particular point indicated by the X, Y axis.

Arguments:

  X  - indicates location to move to.
  Y  - indicates location to move to.

Returns:

 None

--*/
{
  EFI_STATUS                  Status;
  PRIVATE_MOUSE_DATA          *Private;

  Private = mPrivate;

  if (X < Private->MouseRange.StartX || Y < Private->MouseRange.StartY ||
      X > Private->MouseRange.EndX || Y > Private->MouseRange.EndY) {
    return EFI_SUCCESS;
  }

  if ((INT32)X != Private->Cursor.ImageRc.left || (INT32)Y != Private->Cursor.ImageRc.top) {
    MoveImage (&Private->Cursor, X, Y);
  }

  Status = ProcessKeyboard ();
  if (!EFI_ERROR (Status)) {
    Private->HaveRawData = FALSE;
  }

  return Status;
}

EFI_STATUS
ProcessMouse (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
/*++

Routine Description:

  According mouse / touch event to determinate cursor postion and button state

Arguments:

  Event and Context - Not actually used in the function

Returns:

 EFI_SUCCESS - If receive mouse / touch event

--*/
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
  UINTN                                 Index;
  BOOLEAN                               ImageIsSame;
  RECT                                  InvalidateRc;

  Private = SETUP_MOUSE_DEV_FROM_THIS (Context);

  ASSERT (Private->IsStart);

  CurX = (INTN)Private->Cursor.ImageRc.left;
  CurY = (INTN)Private->Cursor.ImageRc.top;

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

      ShowImage (&Private->Cursor);

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
        HideImage (&Private->Cursor);
      }

      ScreenFactor = DivU64x32 (LShiftU64 ((UINT64)(mPrivate->GopList[0].Gop->Mode->Info->HorizontalResolution), 32), (UINTN)(AbsolutePointer->Mode->AbsoluteMaxX - AbsolutePointer->Mode->AbsoluteMinX), NULL);
      CurX = (UINTN)(RShiftU64 (MultU64x32 (ScreenFactor, (UINTN)(AbsolutePointerState.CurrentX - AbsolutePointer->Mode->AbsoluteMinX)), 32));
      ScreenFactor = DivU64x32 (LShiftU64 ((UINT64)(mPrivate->GopList[0].Gop->Mode->Info->VerticalResolution), 32), (UINTN)(AbsolutePointer->Mode->AbsoluteMaxY - AbsolutePointer->Mode->AbsoluteMinY), NULL);
      CurY = (UINTN)(RShiftU64 (MultU64x32 (ScreenFactor, (UINTN)(AbsolutePointerState.CurrentY - AbsolutePointer->Mode->AbsoluteMinY)), 32));

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

    CursorGotoXY (CurX, CurY);
    RenderImageForAllGop (Private);
    return EFI_SUCCESS;
  }

  //
  // sync the keyboard state
  //
  UpdateKeyboardStateByBDA ();

  //
  // check screen per 30 ms
  //
  if (TimeCount++ < ((TICKS_PER_SECOND / 30) / MOUSE_TIMER)) {
    return EFI_NOT_READY;
  }
  TimeCount = 0;

  ImageIsSame = FALSE;
  for (Index = 0; Index < Private->GopCount; Index++) {
    if (Private->Cursor.Visible) {
      IntersectRect (&InvalidateRc, &Private->Cursor.ImageRc, &Private->GopList[Index].Screen.ImageRc);
      SyncScreenImage (Private, &Private->GopList[Index], &InvalidateRc, &ImageIsSame);
      if (!ImageIsSame) {
        UnionRect (&Private->GopList[Index].InvalidateRc, &Private->GopList[Index].InvalidateRc, &Private->Cursor.ImageRc);
      }
    }

    if (Private->Keyboard.Visible) {
      IntersectRect (&InvalidateRc, &Private->Keyboard.ImageRc, &Private->GopList[Index].Screen.ImageRc);
      SyncScreenImage (Private, &Private->GopList[Index], &InvalidateRc, &ImageIsSame);
      if (!ImageIsSame) {
        UnionRect (&Private->GopList[Index].InvalidateRc, &Private->GopList[Index].InvalidateRc, &Private->Keyboard.ImageRc);
      }
    }
    RenderImage (Private, &Private->GopList[Index], TRUE);
  }
  return EFI_NOT_READY;
}

