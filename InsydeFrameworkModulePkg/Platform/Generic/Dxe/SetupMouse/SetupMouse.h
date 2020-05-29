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
//;   SetupMouse.h
//;
//; Abstract:
//;
//;   Common Utility Header for SetupMouse
//;
//;
#ifndef _SETUP_MOUSE_H
#define _SETUP_MOUSE_H

#include "Tiano.h"
#include "TianoHii.h"
#include "EfiDriverLib.h"

#include EFI_PROTOCOL_DEFINITION (SimplePointer)
#include EFI_PROTOCOL_DEFINITION (AbsolutePointer)
#include EFI_PROTOCOL_DEFINITION (GraphicsOutput)
#include EFI_PROTOCOL_DEFINITION (SetupMouse)

#include EFI_PROTOCOL_DEFINITION (ConsoleControl)
#include EFI_PROTOCOL_DEFINITION (DevicePath)


#define MOUSE_RESOLUTION        0x2800 //0x2000 to fixed USB mouse don't smooth issue on Setup

#define TICKS_PER_MS            10000U
#define TICKS_PER_SECOND        10000000U
#define MOUSE_TIMER             (10 * TICKS_PER_MS)  //100000  fixed USB mouse don't smooth at SCU


#define SETUP_MOUSE_SIGNATURE   EFI_SIGNATURE_32('S','t','p','m')

typedef struct _POINTER_PROTOCOL_INFO_DATA {
  UINTN   Attributes;
  VOID    *PointerProtocol;
} POINTER_PROTOCOL_INFO_DATA;

#define ATTRIBUTE_BIT_SIMPLE_OR_ABSOLUTE  (1 << 0)  // Bit 0
#define ATTRIBUTE_VALUE_SIMPLE            (0 << 0)  // Bit 0 clear
#define ATTRIBUTE_VALUE_ABSOLUTE          (1 << 0)  // Bit 0 set

typedef struct _POINTER_PROTOCOL_INFO {
  UINTN                       Count;
  POINTER_PROTOCOL_INFO_DATA  Data[1];
} POINTER_PROTOCOL_INFO;

typedef struct {
  INT32    left;
  INT32    top;
  INT32    right;
  INT32    bottom;
} RECT;

typedef struct {
  INT32    x;
  INT32    y;
} POINT;


typedef struct {
  BOOLEAN                               Visible;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *Image;
  RECT                                  ImageRc;
} IMAGE_INFO;

typedef struct {
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *Gop;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT OrgBlt;
  EFI_GRAPHICS_OUTPUT_PROTOCOL_BLT OriginalBlt;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *BlendBuffer;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL    *FillLine;
  IMAGE_INFO                       Screen;
  RECT                             InvalidateRc;
} GOP_ENTRY;


typedef struct _SETUP_MOUSE_KEYBOARD SETUP_MOUSE_KEYBOARD;

typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            Handle;
  EFI_SETUP_MOUSE_PROTOCOL              SetupMouse;
  POINTER_PROTOCOL_INFO                 *PointerProtocolInfo;
  GOP_ENTRY                             *GopList;
  UINTN                                 GopCount;

  //
  // SetupMouse data
  //
  EFI_SETUP_MOUSE_SCREEN_MODE           ScreenMode;
  BOOLEAN                               IsStart;
  BOOLEAN                               HaveRawData;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *CheckBuffer;
  BOOLEAN                               IsCsmEnabled;

  //
  // Cursor data
  //
  MOUSE_CURSOR_RANGE                    MouseRange;
  BOOLEAN                               LButton, RButton;
  IMAGE_INFO                            Cursor;
  BOOLEAN                               HideCursorWhenTouch;
  UINTN                                 SaveCursorX;
  UINTN                                 SaveCursorY;

  //
  // Keyboard data
  //
  SETUP_MOUSE_KEYBOARD                  *KeyboardData;
  IMAGE_INFO                             Keyboard;
  KEYBOARD_ATTRIBUTES                   SaveKeyboardAttributes;

} PRIVATE_MOUSE_DATA;

extern PRIVATE_MOUSE_DATA                *mPrivate;

#define SETUP_MOUSE_DEV_FROM_THIS(a) \
  CR(a, PRIVATE_MOUSE_DATA, SetupMouse, SETUP_MOUSE_SIGNATURE)

//
// Keyboard State in BDA
//
#define BDA_QUALIFIER_STAT      0x417

typedef struct {
  UINT8 RightShift : 1;
  UINT8 LeftShift : 1;
  UINT8 Ctrl : 1;
  UINT8 Alt : 1;
  UINT8 ScrollLock : 1;
  UINT8 NumLock : 1;
  UINT8 CapsLock : 1;
  UINT8 Insert : 1;
} QUALIFIER_STAT;

//
// Setup Mouse services
//
EFI_STATUS
SetupMouseStart (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse
  );

EFI_STATUS
SetupMouseClose (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse
  );

EFI_STATUS
QueryState (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  OUT UINTN                             *X,
  OUT UINTN                             *Y,
  OUT BOOLEAN                           *LeftButton,
  OUT BOOLEAN                           *RightButton
  );

EFI_STATUS
StartKeyboard (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  IN  UINTN                             X,
  IN  UINTN                             Y
  );

EFI_STATUS
CloseKeyboard (
  IN  EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse
  );

EFI_STATUS
SetupMouseSetMode (
  IN EFI_SETUP_MOUSE_PROTOCOL          *SetupMouse,
  IN EFI_SETUP_MOUSE_SCREEN_MODE       Mode
  );


EFI_STATUS
SetupMouseGetKeyboardAttributes (
  IN  EFI_SETUP_MOUSE_PROTOCOL         *SetupMouse,
  IN  KEYBOARD_ATTRIBUTES              *KeyboardState
  );

EFI_STATUS
SetupMouseSetKeyboardAttributes (
  IN  EFI_SETUP_MOUSE_PROTOCOL         *SetupMouse,
  IN  KEYBOARD_ATTRIBUTES              *KeyboardState
  );


//
// Setup Mouse event function
//
EFI_STATUS
ProcessMouse (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  );

EFI_STATUS
UpdateKeyboardStateByBDA (
  VOID
  );

EFI_STATUS
SetupMouseScreenBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION       BltOperation,
  IN  UINTN                                   SourceX,
  IN  UINTN                                   SourceY,
  IN  UINTN                                   DestinationX,
  IN  UINTN                                   DestinationY,
  IN  UINTN                                   Width,
  IN  UINTN                                   Height,
  IN  UINTN                                   Delta         OPTIONAL
  );


//
// SetupMouse Misc function
//
EFI_STATUS
GetStartOffset (
  IN      EFI_GRAPHICS_OUTPUT_PROTOCOL  *GraphicsOutput,
  OUT     UINTN                         *OffsetX,
  OUT     UINTN                         *OffsetY
  );

BOOLEAN
IsRectEmpty(
  CONST RECT *lprc
  );

VOID
SetRectEmpty(
  RECT *lprc
  );

BOOLEAN
IntersectRect(
  RECT *dest,
  const RECT *src1,
  const RECT *src2
  );

VOID
SetRect(
  RECT *lprc,
  int  xLeft,
  int  yTop,
  int  xRight,
  int  yBottom
  );

BOOLEAN
OffsetRect(
  RECT *lprc,
  INT32 dx,
  INT32 dy
  );


EFI_STATUS
InitializeCursor (
  IN PRIVATE_MOUSE_DATA                    *Private
  );

VOID
DestroyCursor (
  IN PRIVATE_MOUSE_DATA                    *Private
  );

BOOLEAN
PtInRect (
  IN RECT *lprc,
  IN POINT pt
  );

BOOLEAN
UnionRect(
  OUT RECT       *dest,
  IN  CONST RECT *src1,
  IN  CONST RECT *src2
  );

EFI_STATUS
ProcessKeyboard (
  );

EFI_STATUS
RenderImage (
  IN PRIVATE_MOUSE_DATA   *Private,
  IN GOP_ENTRY            *GopEntry,
  IN BOOLEAN              NeedSyncScreen
  );

VOID
HideImage (
  IN  IMAGE_INFO                           *ImageInfo
  );

VOID
MoveImage (
  IN  IMAGE_INFO                           *ImageInfo,
  IN  UINTN                                X,
  IN  UINTN                                Y
  );

VOID
RenderImageForAllGop (
  IN PRIVATE_MOUSE_DATA   *Private
  );

IMAGE_INFO *
CreateImage (
  IN UINTN Width,
  IN UINTN Height
  );

VOID
DestroyImage (
  IN IMAGE_INFO *ImageInfo
  );

UINT8 *
SetupMouseGetAnimation (
  IN UINT8             *AnimationBlock,
  IN EFI_ANIMATION_ID  AnimationId
  );

UINTN
GetStringWidth (
  IN CHAR16            *String
  );


EFI_STATUS
SetupMouseDrawText (
  IN IMAGE_INFO                    *ImageInfo,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *TextColor,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL *BackgroundColor,
  IN EFI_STRING                    String,
  IN RECT                          *TextRc
  );

VOID
SyncScreenImage (
  IN  PRIVATE_MOUSE_DATA  *Private,
  IN  GOP_ENTRY           *GopEntry,
  IN  RECT                *Rc,
  OUT BOOLEAN             *ImageIsSame
  );

VOID
InvalidateImage (
  IN PRIVATE_MOUSE_DATA                    *Private,
  IN IMAGE_INFO                           *ImageInfo
  );

VOID
ShowImage (
  IN  IMAGE_INFO                           *ImageInfo
  );


EFI_STATUS
CursorGotoXY (
  IN  UINTN                                 X,
  IN  UINTN                                 Y
  );


EFI_STATUS
SetupMouseShowBitmap(
  IN RECT                          *DstRc,
  IN RECT                          *Scale9Grid,
  IN UINT16                        ImageId
  );


#endif
