/** @file
  Header file for form browser console splitter

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _FB_CON_SPLITTER_H_
#define _FB_CON_SPLITTER_H_

#include "InternalH2OFormBrowser.h"

#include <Protocol/SimplePointer.h>
#include <Protocol/AbsolutePointer.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/UgaDraw.h>

#define HOT_PLUG_ALLOC_UNIT          32

#define CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_SIGNATURE SIGNATURE_32 ('C', 'I', 'H', 'P')
typedef struct {
  UINT32                             Signature;

  EFI_SIMPLE_TEXT_INPUT_PROTOCOL     TextIn;
  UINT32                             CurrentNumOfTextIn;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL     **TextInList;
  UINT32                             TextInListCount;

  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  TextInEx;
  UINT32                             CurrentNumOfTextInEx;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  **TextInExList;
  UINT32                             TextInExListCount;
  LIST_ENTRY                         NotifyList;
  EFI_KEY_TOGGLE_STATE               KeyToggleState;

  EFI_SIMPLE_POINTER_PROTOCOL        SimplePointer;
  EFI_SIMPLE_POINTER_MODE            SimplePointerMode;
  UINT32                             CurrentNumOfSimplePointer;
  EFI_SIMPLE_POINTER_PROTOCOL        **SimplePointerList;
  UINT32                             SimplePointerListCount;

  EFI_ABSOLUTE_POINTER_PROTOCOL      AbsolutePointer;
  EFI_ABSOLUTE_POINTER_MODE          AbsolutePointerMode;
  UINT32                             CurrentNumOfAbsolutePointer;
  EFI_ABSOLUTE_POINTER_PROTOCOL      **AbsolutePointerList;
  UINT32                             AbsolutePointerListCount;
  BOOLEAN                            AbsoluteInputEventSignalState;

  BOOLEAN                            KeyEventSignalState;
  BOOLEAN                            InputEventSignalState;
} CONSOLE_IN_HOT_PLUG_PRIVATE_DATA;

#define CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_TEXT_IN(a)  \
  CR ((a),                                                \
      CONSOLE_IN_HOT_PLUG_PRIVATE_DATA,                   \
      TextIn,                                             \
      CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_SIGNATURE       \
      )
#define CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_TEXT_IN_EX(a) \
  CR (a,                                                    \
      CONSOLE_IN_HOT_PLUG_PRIVATE_DATA,                     \
      TextInEx,                                             \
      CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_SIGNATURE            \
      )
#define CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_SIMPLE_POINTER(a) \
  CR ((a),                                                      \
      CONSOLE_IN_HOT_PLUG_PRIVATE_DATA,                         \
      SimplePointer,                                            \
      CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_SIGNATURE                \
      )
#define CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_FROM_ABSOLUTE_POINTER(a) \
  CR (a,                                                          \
      CONSOLE_IN_HOT_PLUG_PRIVATE_DATA,                           \
      AbsolutePointer,                                            \
      CONSOLE_IN_HOT_PLUG_PRIVATE_DATA_SIGNATURE                  \
      )

typedef struct {
  UINTN   Columns;
  UINTN   Rows;
} TEXT_OUT_SPLITTER_QUERY_DATA;

typedef struct {
  EFI_GRAPHICS_OUTPUT_PROTOCOL     *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL            *UgaDraw;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *TextOut;
} TEXT_OUT_AND_GOP_DATA;

#define CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('C', 'O', 'H', 'P')

typedef struct {
  UINT32                                Signature;

  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       TextOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE           TextOutMode;

  EFI_UGA_DRAW_PROTOCOL                 UgaDraw;
  UINT32                                UgaHorizontalResolution;
  UINT32                                UgaVerticalResolution;
  UINT32                                UgaColorDepth;
  UINT32                                UgaRefreshRate;

  EFI_GRAPHICS_OUTPUT_PROTOCOL          GraphicsOutput;
  UINT32                                CurrentNumberOfGraphicsOutput;
  UINT32                                CurrentNumberOfUgaDraw;

  UINT32                                CurrentNumberOfConsoles;
  TEXT_OUT_AND_GOP_DATA                 *TextOutList;
  UINT32                                TextOutListCount;
} CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA;

#define HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_TEXT_OUT(a) \
  CR ((a),                                                    \
      CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA,                      \
      TextOut,                                                \
      CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA_SIGNATURE             \
      )
#define HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_GRAPHIC_OUT(a) \
  CR ((a),                                                       \
      CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA,                         \
      GraphicsOutput,                                            \
      CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA_SIGNATURE                \
      )
#define HOT_PLUG_CONSOLE_OUTPUT_PRIVATE_DATA_FROM_UGA_DRAW(a) \
  CR ((a),                                                    \
      CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA,                      \
      UgaDraw,                                                \
      CONSOLE_OUT_HOT_PLUG_PRIVATE_DATA_SIGNATURE             \
      )

//
// Simple text input protocol
//
EFI_STATUS
EFIAPI
FBTextInReset (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL  *This,
  IN  BOOLEAN                         ExtendedVerification
  );

EFI_STATUS
EFIAPI
FBTextInPrivateReadKeyStroke (
  IN  CONSOLE_IN_HOT_PLUG_PRIVATE_DATA      *Private,
  OUT EFI_INPUT_KEY                         *Key
  );

EFI_STATUS
EFIAPI
FBTextInReadKeyStroke (
  IN  EFI_SIMPLE_TEXT_INPUT_PROTOCOL       *This,
  OUT EFI_INPUT_KEY                        *Key
  );

VOID
EFIAPI
FBTextInWaitForKey (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  );

//
// Simple text input ex protocol
//
EFI_STATUS
EFIAPI
FBTextInExReset (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN BOOLEAN                               ExtendedVerification
  );

EFI_STATUS
EFIAPI
FBTextInExReadKeyStrokeEx (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
  OUT EFI_KEY_DATA                         *KeyData
  );

EFI_STATUS
EFIAPI
FBTextInExSetState (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN EFI_KEY_TOGGLE_STATE                  *KeyToggleState
  );

EFI_STATUS
EFIAPI
FBTextInExRegisterKeyNotify (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *This,
  IN  EFI_KEY_DATA                         *KeyData,
  IN  EFI_KEY_NOTIFY_FUNCTION              KeyNotificationFunction,
  OUT EFI_HANDLE                           *NotifyHandle
  );

EFI_STATUS
EFIAPI
FBTextInExUnregisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *This,
  IN EFI_HANDLE                            NotificationHandle
  );

//
// Simple pointer protocol
//
EFI_STATUS
EFIAPI
FBSimplePointerReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL           *This,
  IN BOOLEAN                               ExtendedVerification
  );

EFI_STATUS
EFIAPI
FBSimplePointerGetState (
  IN     EFI_SIMPLE_POINTER_PROTOCOL       *This,
  IN OUT EFI_SIMPLE_POINTER_STATE          *State
  );

//
// Absolute pointer protocol
//
EFI_STATUS
EFIAPI
FBAbsolutePointerReset (
  IN EFI_ABSOLUTE_POINTER_PROTOCOL         *This,
  IN BOOLEAN                               ExtendedVerification
  );

EFI_STATUS
EFIAPI
FBAbsolutePointerGetState (
  IN     EFI_ABSOLUTE_POINTER_PROTOCOL     *This,
  IN OUT EFI_ABSOLUTE_POINTER_STATE        *State
  );

//
// Simple text output protocol
//
EFI_STATUS
EFIAPI
FBTextOutReset (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  BOOLEAN                            ExtendedVerification
  );

EFI_STATUS
EFIAPI
FBTextOutOutputString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  CHAR16                             *WString
  );

EFI_STATUS
EFIAPI
FBTextOutTestString (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  CHAR16                             *WString
  );

EFI_STATUS
EFIAPI
FBTextOutQueryMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  UINTN                              ModeNumber,
  OUT UINTN                              *Columns,
  OUT UINTN                              *Rows
  );

EFI_STATUS
EFIAPI
FBTextOutSetMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  UINTN                              ModeNumber
  );

EFI_STATUS
EFIAPI
FBTextOutSetAttribute (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  UINTN                              Attribute
  );

EFI_STATUS
EFIAPI
FBTextOutClearScreen (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This
  );

EFI_STATUS
EFIAPI
FBTextOutSetCursorPosition (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  UINTN                              Column,
  IN  UINTN                              Row
  );

EFI_STATUS
EFIAPI
FBTextOutEnableCursor (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *This,
  IN  BOOLEAN                            Visible
  );

//
// Uga draw protocol
//
EFI_STATUS
EFIAPI
FBUgaDrawGetMode (
  IN  EFI_UGA_DRAW_PROTOCOL                *This,
  OUT UINT32                               *HorizontalResolution,
  OUT UINT32                               *VerticalResolution,
  OUT UINT32                               *ColorDepth,
  OUT UINT32                               *RefreshRate
  );

EFI_STATUS
EFIAPI
FBUgaDrawSetMode (
  IN EFI_UGA_DRAW_PROTOCOL            *This,
  IN UINT32                           HorizontalResolution,
  IN UINT32                           VerticalResolution,
  IN UINT32                           ColorDepth,
  IN UINT32                           RefreshRate
  );

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
  );

//
// Graphic output protocol
//
EFI_STATUS
EFIAPI
FBGraphicsOutputQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL         *This,
  IN  UINT32                               ModeNumber,
  OUT UINTN                                *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info
  );

EFI_STATUS
EFIAPI
FBGraphicsOutputSetMode (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN UINT32                                ModeNumber
  );

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
  );

//
// For FB consplitter
//
EFI_STATUS
FBConsplitterUpdateModeData (
  VOID
  );

EFI_STATUS
FBConsplitterAddDevice (
  IN EFI_HANDLE                            ControllerHandle
  );

EFI_STATUS
FBConsplitterDeleteDevice (
  IN EFI_HANDLE                            ControllerHandle
  );

EFI_STATUS
FBConsplitterInit (
  VOID
  );

EFI_STATUS
FBConsplitterStart (
  VOID
  );

EFI_STATUS
FBConsplitterShutdown (
  VOID
  );

#endif
