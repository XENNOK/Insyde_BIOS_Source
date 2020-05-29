/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Support for Graphics output spliter.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.


**/

#include "ConSplitter.h"
#include <Protocol/LegacyBios.h>
#include <Library/OemGraphicsLib.h>

#define EFI_CURRENT_VGA_MODE_ADDRESS  0x4A3
#define CURRENT_VGA_MODE_ADDRESS      0x449
#define NARROW_CHAR         0xFFF0
#define WIDE_CHAR           0xFFF1

CHAR16 mCrLfString[3] = { CHAR_CARRIAGE_RETURN, CHAR_LINEFEED, CHAR_NULL };

/**
  Returns information for an available graphics mode that the graphics device
  and the set of active video output devices supports.

  @param  This                  The EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  @param  ModeNumber            The mode number to return information on.
  @param  SizeOfInfo            A pointer to the size, in bytes, of the Info buffer.
  @param  Info                  A pointer to callee allocated buffer that returns information about ModeNumber.

  @retval EFI_SUCCESS           Mode information returned.
  @retval EFI_BUFFER_TOO_SMALL  The Info buffer was too small.
  @retval EFI_DEVICE_ERROR      A hardware error occurred trying to retrieve the video mode.
  @retval EFI_INVALID_PARAMETER One of the input args was NULL.
  @retval EFI_OUT_OF_RESOURCES  No resource available.

**/
EFI_STATUS
EFIAPI
ConSplitterGraphicsOutputQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN  UINT32                                ModeNumber,
  OUT UINTN                                 *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  )
{
  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private;

  if (This == NULL || Info == NULL || SizeOfInfo == NULL || ModeNumber >= This->Mode->MaxMode) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // retrieve private data
  //
  Private = GRAPHICS_OUTPUT_SPLITTER_PRIVATE_DATA_FROM_THIS (This);

  *Info = AllocatePool (sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
  if (*Info == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *SizeOfInfo = sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);

  CopyMem (*Info, &Private->GraphicsOutputModeBuffer[ModeNumber], *SizeOfInfo);

  return EFI_SUCCESS;
}


/**
  Set the video device into the specified mode and clears the visible portions of
  the output display to black.

  @param  This                  The EFI_GRAPHICS_OUTPUT_PROTOCOL instance.
  @param  ModeNumber            Abstraction that defines the current video mode.

  @retval EFI_SUCCESS           The graphics mode specified by ModeNumber was selected.
  @retval EFI_DEVICE_ERROR      The device had an error and could not complete the request.
  @retval EFI_UNSUPPORTED       ModeNumber is not supported by this device.
  @retval EFI_OUT_OF_RESOURCES  No resource available.

**/
EFI_STATUS
EFIAPI
ConSplitterGraphicsOutputSetMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL * This,
  IN  UINT32                       ModeNumber
  )
{
  EFI_STATUS                             Status;
  TEXT_OUT_SPLITTER_PRIVATE_DATA         *Private;
  UINTN                                  Index;
  EFI_STATUS                             ReturnStatus;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Mode;
  EFI_GRAPHICS_OUTPUT_PROTOCOL           *GraphicsOutput;
  UINTN                                  NumberIndex;
  UINTN                                  SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
  EFI_UGA_DRAW_PROTOCOL                  *UgaDraw;
  EFI_LEGACY_BIOS_PROTOCOL               *LegacyBios;
  UINT8                                  *EFIBdaVGAMode;
  UINT8                                  *BdaVGAMode;
  UINT32                                 ClearFrameBuffer = 0x00;

  EFIBdaVGAMode = (UINT8 *)(UINTN)EFI_CURRENT_VGA_MODE_ADDRESS;
  BdaVGAMode    = (UINT8 *)(UINTN)CURRENT_VGA_MODE_ADDRESS;

  if (ModeNumber >= This->Mode->MaxMode) {
    return EFI_UNSUPPORTED;
  }

  Private = GRAPHICS_OUTPUT_SPLITTER_PRIVATE_DATA_FROM_THIS (This);
  Mode = &Private->GraphicsOutputModeBuffer[ModeNumber];

  ReturnStatus = EFI_SUCCESS;

  if (ModeNumber == This->Mode->Mode && IsModeSync ()) {

    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
    if (EFI_ERROR (Status) || (*EFIBdaVGAMode != 0 && *EFIBdaVGAMode == *BdaVGAMode)) {
      //
      // Assign zero blt buffer for clearing screen.
      //
      ConSplitterGraphicsOutputBlt (
        This,
        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)&ClearFrameBuffer,
        EfiBltVideoFill,
        0,
        0,
        0,
        0,
        This->Mode->Info->HorizontalResolution,
        This->Mode->Info->VerticalResolution,
        0
        );
      return EFI_SUCCESS;
    }
  }


  //
  // return the worst status met
  //
  for (Index = 0; Index < Private->CurrentNumberOfConsoles; Index++) {
    GraphicsOutput = Private->TextOutList[Index].GraphicsOutput;
    if (GraphicsOutput != NULL) {
      //
      // Find corresponding ModeNumber of this GraphicsOutput instance
      //
      for (NumberIndex = 0; NumberIndex < GraphicsOutput->Mode->MaxMode; NumberIndex ++) {
        Status = GraphicsOutput->QueryMode (GraphicsOutput, (UINT32) NumberIndex, &SizeOfInfo, &Info);
        if (EFI_ERROR (Status)) {
          return Status;
        }
        if ((Info->HorizontalResolution == Mode->HorizontalResolution) && (Info->VerticalResolution == Mode->VerticalResolution)) {
          FreePool (Info);
          break;
        }
        FreePool (Info);
      }

      if ((GraphicsOutput->Mode->Info->HorizontalResolution == Mode->HorizontalResolution) &&
          (GraphicsOutput->Mode->Info->VerticalResolution == Mode->VerticalResolution)) {
          //
          // In the case that the mode to be set is the same as current gop mode, skip set mode.
          // This will help improve consplitter performace.
          //
        Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
        if (EFI_ERROR (Status) || (*EFIBdaVGAMode != 0 && *EFIBdaVGAMode == *BdaVGAMode)) {
          continue;
        }
      }
      
      Status = GraphicsOutput->SetMode (GraphicsOutput, (UINT32) NumberIndex);
      if (EFI_ERROR (Status)) {
        ReturnStatus = Status;
      }
    } else if (FeaturePcdGet (PcdUgaConsumeSupport)) {
      UgaDraw = Private->TextOutList[Index].UgaDraw;
      if (UgaDraw != NULL) {
        Status = UgaDraw->SetMode (
                            UgaDraw,
                            Mode->HorizontalResolution,
                            Mode->VerticalResolution,
                            32,
                            60
                            );
        if (EFI_ERROR (Status)) {
          ReturnStatus = Status;
        }
      }
    }
  }

  This->Mode->Mode = ModeNumber;

  CopyMem (This->Mode->Info, &Private->GraphicsOutputModeBuffer[ModeNumber], This->Mode->SizeOfInfo);

  //
  // Update mode information only when there is one GOP
  //
  if (Private->CurrentNumberOfConsoles == 1 &&
      Private->TextOutList[0].GraphicsOutput != NULL) {
    GraphicsOutput = Private->TextOutList[0].GraphicsOutput;
    Private->GraphicsOutput.Mode->FrameBufferBase = GraphicsOutput->Mode->FrameBufferBase;
    Private->GraphicsOutput.Mode->FrameBufferSize = GraphicsOutput->Mode->FrameBufferSize;
  }

  return ReturnStatus;
}



/**
  The following table defines actions for BltOperations.

  EfiBltVideoFill - Write data from the  BltBuffer pixel (SourceX, SourceY)
  directly to every pixel of the video display rectangle
  (DestinationX, DestinationY)
  (DestinationX + Width, DestinationY + Height).
  Only one pixel will be used from the BltBuffer. Delta is NOT used.
  EfiBltVideoToBltBuffer - Read data from the video display rectangle
  (SourceX, SourceY) (SourceX + Width, SourceY + Height) and place it in
  the BltBuffer rectangle (DestinationX, DestinationY )
  (DestinationX + Width, DestinationY + Height). If DestinationX or
  DestinationY is not zero then Delta must be set to the length in bytes
  of a row in the BltBuffer.
  EfiBltBufferToVideo - Write data from the  BltBuffer rectangle
  (SourceX, SourceY) (SourceX + Width, SourceY + Height) directly to the
  video display rectangle (DestinationX, DestinationY)
  (DestinationX + Width, DestinationY + Height). If SourceX or SourceY is
  not zero then Delta must be set to the length in bytes of a row in the
  BltBuffer.
  EfiBltVideoToVideo - Copy from the video display rectangle
  (SourceX, SourceY) (SourceX + Width, SourceY + Height) .
  to the video display rectangle (DestinationX, DestinationY)
  (DestinationX + Width, DestinationY + Height).
  The BltBuffer and Delta  are not used in this mode.

  @param  This                    Protocol instance pointer.
  @param  BltBuffer               Buffer containing data to blit into video buffer.
                                  This buffer has a size of
                                  Width*Height*sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
  @param  BltOperation            Operation to perform on BlitBuffer and video
                                  memory
  @param  SourceX                 X coordinate of source for the BltBuffer.
  @param  SourceY                 Y coordinate of source for the BltBuffer.
  @param  DestinationX            X coordinate of destination for the BltBuffer.
  @param  DestinationY            Y coordinate of destination for the BltBuffer.
  @param  Width                   Width of rectangle in BltBuffer in pixels.
  @param  Height                  Hight of rectangle in BltBuffer in pixels.
  @param  Delta                   OPTIONAL.

  @retval EFI_SUCCESS             The Blt operation completed.
  @retval EFI_INVALID_PARAMETER   BltOperation is not valid.
  @retval EFI_DEVICE_ERROR        A hardware error occured writting to the video
                                  buffer.

**/
EFI_STATUS
EFIAPI
ConSplitterGraphicsOutputBlt (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL                  *This,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL                 *BltBuffer, OPTIONAL
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION             BltOperation,
  IN  UINTN                                         SourceX,
  IN  UINTN                                         SourceY,
  IN  UINTN                                         DestinationX,
  IN  UINTN                                         DestinationY,
  IN  UINTN                                         Width,
  IN  UINTN                                         Height,
  IN  UINTN                                         Delta         OPTIONAL
  )
{
  EFI_STATUS                      Status;
  EFI_STATUS                      ReturnStatus;
  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private;
  UINTN                           Index;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL           *UgaDraw;

  if (This == NULL || ((UINTN) BltOperation) >= EfiGraphicsOutputBltOperationMax) {
    return EFI_INVALID_PARAMETER;
  }

  Private = GRAPHICS_OUTPUT_SPLITTER_PRIVATE_DATA_FROM_THIS (This);

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

  return ReturnStatus;
}

/**
  Return the current video mode information.

  @param  This                  The EFI_UGA_DRAW_PROTOCOL instance.
  @param  HorizontalResolution  The size of video screen in pixels in the X dimension.
  @param  VerticalResolution    The size of video screen in pixels in the Y dimension.
  @param  ColorDepth            Number of bits per pixel, currently defined to be 32.
  @param  RefreshRate           The refresh rate of the monitor in Hertz.

  @retval EFI_SUCCESS           Mode information returned.
  @retval EFI_NOT_STARTED       Video display is not initialized. Call SetMode ()
  @retval EFI_INVALID_PARAMETER One of the input args was NULL.

**/
EFI_STATUS
EFIAPI
ConSplitterUgaDrawGetMode (
  IN  EFI_UGA_DRAW_PROTOCOL           *This,
  OUT UINT32                          *HorizontalResolution,
  OUT UINT32                          *VerticalResolution,
  OUT UINT32                          *ColorDepth,
  OUT UINT32                          *RefreshRate
  )
{
  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private;

  if ((HorizontalResolution == NULL) ||
      (VerticalResolution   == NULL) ||
      (RefreshRate          == NULL) ||
      (ColorDepth           == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // retrieve private data
  //
  Private               = UGA_DRAW_SPLITTER_PRIVATE_DATA_FROM_THIS (This);

  *HorizontalResolution = Private->UgaHorizontalResolution;
  *VerticalResolution   = Private->UgaVerticalResolution;
  *ColorDepth           = Private->UgaColorDepth;
  *RefreshRate          = Private->UgaRefreshRate;

  return EFI_SUCCESS;
}


/**
  Set the current video mode information.

  @param  This                 The EFI_UGA_DRAW_PROTOCOL instance.
  @param  HorizontalResolution The size of video screen in pixels in the X dimension.
  @param  VerticalResolution   The size of video screen in pixels in the Y dimension.
  @param  ColorDepth           Number of bits per pixel, currently defined to be 32.
  @param  RefreshRate          The refresh rate of the monitor in Hertz.

  @retval EFI_SUCCESS          Mode information returned.
  @retval EFI_NOT_STARTED      Video display is not initialized. Call SetMode ()
  @retval EFI_OUT_OF_RESOURCES Out of resources.

**/
EFI_STATUS
EFIAPI
ConSplitterUgaDrawSetMode (
  IN  EFI_UGA_DRAW_PROTOCOL           *This,
  IN UINT32                           HorizontalResolution,
  IN UINT32                           VerticalResolution,
  IN UINT32                           ColorDepth,
  IN UINT32                           RefreshRate
  )
{
  EFI_STATUS                             Status;
  TEXT_OUT_SPLITTER_PRIVATE_DATA         *Private;
  UINTN                                  Index;
  EFI_STATUS                             ReturnStatus;
  EFI_GRAPHICS_OUTPUT_PROTOCOL           *GraphicsOutput;
  UINTN                                  NumberIndex;
  UINTN                                  SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   *Info;
  EFI_UGA_DRAW_PROTOCOL                  *UgaDraw;

  Private = UGA_DRAW_SPLITTER_PRIVATE_DATA_FROM_THIS (This);

  ReturnStatus = EFI_SUCCESS;

  //
  // Update the Mode data
  //
  Private->UgaHorizontalResolution  = HorizontalResolution;
  Private->UgaVerticalResolution    = VerticalResolution;
  Private->UgaColorDepth            = ColorDepth;
  Private->UgaRefreshRate           = RefreshRate;

  //
  // return the worst status met
  //
  for (Index = 0; Index < Private->CurrentNumberOfConsoles; Index++) {

    GraphicsOutput = Private->TextOutList[Index].GraphicsOutput;
    if (GraphicsOutput != NULL) {
      //
      // Find corresponding ModeNumber of this GraphicsOutput instance
      //
      for (NumberIndex = 0; NumberIndex < GraphicsOutput->Mode->MaxMode; NumberIndex ++) {
        Status = GraphicsOutput->QueryMode (GraphicsOutput, (UINT32) NumberIndex, &SizeOfInfo, &Info);
        if (EFI_ERROR (Status)) {
          return Status;
        }
        if ((Info->HorizontalResolution == HorizontalResolution) && (Info->VerticalResolution == VerticalResolution)) {
          FreePool (Info);
          break;
        }
        FreePool (Info);
      }

      Status = GraphicsOutput->SetMode (GraphicsOutput, (UINT32) NumberIndex);
      if (EFI_ERROR (Status)) {
        ReturnStatus = Status;
      }
    } else if (FeaturePcdGet (PcdUgaConsumeSupport)){
      UgaDraw = Private->TextOutList[Index].UgaDraw;
      if (UgaDraw != NULL) {
        Status = UgaDraw->SetMode (
                          UgaDraw,
                          HorizontalResolution,
                          VerticalResolution,
                          ColorDepth,
                          RefreshRate
                          );
        if (EFI_ERROR (Status)) {
          ReturnStatus = Status;
        }
      }
    }
  }

  return ReturnStatus;
}


/**
  Blt a rectangle of pixels on the graphics screen.

  The following table defines actions for BltOperations.

  EfiUgaVideoFill:
    Write data from the  BltBuffer pixel (SourceX, SourceY)
    directly to every pixel of the video display rectangle
    (DestinationX, DestinationY)
    (DestinationX + Width, DestinationY + Height).
    Only one pixel will be used from the BltBuffer. Delta is NOT used.
  EfiUgaVideoToBltBuffer:
    Read data from the video display rectangle
    (SourceX, SourceY) (SourceX + Width, SourceY + Height) and place it in
    the BltBuffer rectangle (DestinationX, DestinationY )
    (DestinationX + Width, DestinationY + Height). If DestinationX or
    DestinationY is not zero then Delta must be set to the length in bytes
    of a row in the BltBuffer.
  EfiUgaBltBufferToVideo:
    Write data from the  BltBuffer rectangle
    (SourceX, SourceY) (SourceX + Width, SourceY + Height) directly to the
    video display rectangle (DestinationX, DestinationY)
    (DestinationX + Width, DestinationY + Height). If SourceX or SourceY is
    not zero then Delta must be set to the length in bytes of a row in the
    BltBuffer.
  EfiUgaVideoToVideo:
    Copy from the video display rectangle
    (SourceX, SourceY) (SourceX + Width, SourceY + Height) .
    to the video display rectangle (DestinationX, DestinationY)
    (DestinationX + Width, DestinationY + Height).
    The BltBuffer and Delta  are not used in this mode.

  @param  This           Protocol instance pointer.
  @param  BltBuffer      Buffer containing data to blit into video buffer. This
                         buffer has a size of Width*Height*sizeof(EFI_UGA_PIXEL)
  @param  BltOperation   Operation to perform on BlitBuffer and video memory
  @param  SourceX        X coordinate of source for the BltBuffer.
  @param  SourceY        Y coordinate of source for the BltBuffer.
  @param  DestinationX   X coordinate of destination for the BltBuffer.
  @param  DestinationY   Y coordinate of destination for the BltBuffer.
  @param  Width          Width of rectangle in BltBuffer in pixels.
  @param  Height         Hight of rectangle in BltBuffer in pixels.
  @param  Delta          OPTIONAL

  @retval EFI_SUCCESS            The Blt operation completed.
  @retval EFI_INVALID_PARAMETER  BltOperation is not valid.
  @retval EFI_DEVICE_ERROR       A hardware error occured writting to the video buffer.

**/
EFI_STATUS
EFIAPI
ConSplitterUgaDrawBlt (
  IN  EFI_UGA_DRAW_PROTOCOL                         *This,
  IN  EFI_UGA_PIXEL                                 *BltBuffer, OPTIONAL
  IN  EFI_UGA_BLT_OPERATION                         BltOperation,
  IN  UINTN                                         SourceX,
  IN  UINTN                                         SourceY,
  IN  UINTN                                         DestinationX,
  IN  UINTN                                         DestinationY,
  IN  UINTN                                         Width,
  IN  UINTN                                         Height,
  IN  UINTN                                         Delta         OPTIONAL
  )
{
  EFI_STATUS                      Status;
  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private;
  UINTN                           Index;
  EFI_STATUS                      ReturnStatus;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;

  Private = UGA_DRAW_SPLITTER_PRIVATE_DATA_FROM_THIS (This);

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

  return ReturnStatus;
}

/**
  Sets the output device(s) to a specified mode.

  @param  Private                 Text Out Splitter pointer.
  @param  ModeNumber              The mode number to set.

**/
VOID
TextOutSetMode (
  IN  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private,
  IN  UINTN                           ModeNumber
  )
{
  //
  // No need to do extra check here as whether (Column, Row) is valid has
  // been checked in ConSplitterTextOutSetCursorPosition. And (0, 0) should
  // always be supported.
  //
  Private->TextOutMode.Mode          = (INT32) ModeNumber;
  Private->TextOutMode.CursorColumn  = 0;
  Private->TextOutMode.CursorRow     = 0;
  Private->TextOutMode.CursorVisible = TRUE;

  return;
}

EFI_STATUS
DevNullTextOutOutputString (
  IN  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private,
  IN  CHAR16                          *WString
  )
/*++

  Routine Description:
    Write a Unicode string to the output device.

  Arguments:
    Private - Pointer to the console output splitter's private data. It
              indicates the calling context.
    WString - The NULL-terminated Unicode string to be displayed on the output
              device(s). All output devices must also support the Unicode
              drawing defined in this file.

  Returns:
    EFI_SUCCESS            - The string was output to the device.
    EFI_DEVICE_ERROR       - The device reported an error while attempting to
                              output the text.
    EFI_UNSUPPORTED        - The output device's mode is not currently in a
                              defined text mode.
    EFI_WARN_UNKNOWN_GLYPH - This warning code indicates that some of the
                              characters in the Unicode string could not be
                              rendered and were skipped.

--*/
{
  UINTN                       SizeScreen;
  UINTN                       SizeAttribute;
  UINTN                       Index;
  EFI_SIMPLE_TEXT_OUTPUT_MODE *Mode;
  CHAR16                      *Screen;
  CHAR16                      *NullScreen;
  CHAR16                      InsertChar;
  CHAR16                      TempChar;
  CHAR16                      *PStr;
  INT32                       *Attribute;
  INT32                       *NullAttributes;
  INT32                       CurrentWidth;
  UINTN                       LastRow;
  UINTN                       MaxColumn;

  Mode            = &Private->TextOutMode;
  NullScreen      = Private->DevNullScreen;
  NullAttributes  = Private->DevNullAttributes;
  LastRow         = Private->DevNullRows - 1;
  MaxColumn       = Private->DevNullColumns;

  if (Mode->Attribute & EFI_WIDE_ATTRIBUTE) {
    CurrentWidth = 2;
  } else {
    CurrentWidth = 1;
  }

  Private->IsDevNullBlankScreen = FALSE;

  while (*WString) {

    if (*WString == CHAR_BACKSPACE) {
      //
      // If the cursor is at the left edge of the display, then move the cursor
      // one row up.
      //
      if (Mode->CursorColumn == 0 && Mode->CursorRow > 0) {
        Mode->CursorRow--;
        Mode->CursorColumn = (INT32) MaxColumn;
      }

      //
      // If the cursor is not at the left edge of the display,
      // then move the cursor left one column.
      //
      if (Mode->CursorColumn > 0) {
        Mode->CursorColumn--;
        if (Mode->CursorColumn > 0 &&
            NullAttributes[Mode->CursorRow * MaxColumn + Mode->CursorColumn - 1] & EFI_WIDE_ATTRIBUTE
            ) {
          Mode->CursorColumn--;

          //
          // Insert an extra backspace
          //
          InsertChar  = CHAR_BACKSPACE;
          PStr        = WString + 1;
          while (*PStr) {
            TempChar    = *PStr;
            *PStr       = InsertChar;
            InsertChar  = TempChar;
            PStr++;
          }

          *PStr     = InsertChar;
          *(++PStr) = 0;

          WString++;
        }
      }

      WString++;

    } else if (*WString == CHAR_LINEFEED) {
      //
      // If the cursor is at the bottom of the display,
      // then scroll the display one row, and do not update
      // the cursor position. Otherwise, move the cursor down one row.
      //
      if (Mode->CursorRow == (INT32) (LastRow)) {
        //
        // Scroll Screen Up One Row
        //
        SizeAttribute = LastRow * MaxColumn;
        CopyMem (
          NullAttributes,
          NullAttributes + MaxColumn,
          SizeAttribute * sizeof (INT32)
          );

        //
        // Each row has an ending CHAR_NULL. So one more character each line
        // for DevNullScreen than DevNullAttributes
        //
        SizeScreen = SizeAttribute + LastRow;
        CopyMem (
          NullScreen,
          NullScreen + (MaxColumn + 1),
          SizeScreen * sizeof (CHAR16)
          );

        //
        // Print Blank Line at last line
        //
        Screen    = NullScreen + SizeScreen;
        Attribute = NullAttributes + SizeAttribute;

        for (Index = 0; Index < MaxColumn; Index++, Screen++, Attribute++) {
          *Screen     = ' ';
          *Attribute  = Mode->Attribute;
        }
      } else {
        Mode->CursorRow++;
      }

      WString++;
    } else if (*WString == CHAR_CARRIAGE_RETURN) {
      //
      // Move the cursor to the beginning of the current row.
      //
      Mode->CursorColumn = 0;
      WString++;
    } else {
      //
      // Print the character at the current cursor position and
      // move the cursor right one column. If this moves the cursor
      // past the right edge of the display, then the line should wrap to
      // the beginning of the next line. This is equivalent to inserting
      // a CR and an LF. Note that if the cursor is at the bottom of the
      // display, and the line wraps, then the display will be scrolled
      // one line.
      //
      Index = Mode->CursorRow * MaxColumn + Mode->CursorColumn;

      while (Mode->CursorColumn < (INT32) MaxColumn) {
        if (*WString == CHAR_NULL) {
          break;
        }

        if (*WString == CHAR_BACKSPACE) {
          break;
        }

        if (*WString == CHAR_LINEFEED) {
          break;
        }

        if (*WString == CHAR_CARRIAGE_RETURN) {
          break;
        }

        if (*WString == WIDE_CHAR || *WString == NARROW_CHAR) {
          CurrentWidth = (*WString == WIDE_CHAR) ? 2 : 1;
          WString++;
          continue;
        }

        if (Mode->CursorColumn + CurrentWidth > (INT32) MaxColumn) {
          //
          // If a wide char is at the rightmost column, then move the char
          // to the beginning of the next row
          //
          NullScreen[Index + Mode->CursorRow] = L' ';
          NullAttributes[Index]               = Mode->Attribute | (UINT32) EFI_WIDE_ATTRIBUTE;
          Index++;
          Mode->CursorColumn++;
        } else {
          NullScreen[Index + Mode->CursorRow] = *WString;
          NullAttributes[Index]               = Mode->Attribute;
          if (CurrentWidth == 1) {
            NullAttributes[Index] &= (~ (UINT32) EFI_WIDE_ATTRIBUTE);
          } else {
            NullAttributes[Index] |= (UINT32) EFI_WIDE_ATTRIBUTE;
            NullAttributes[Index + 1] &= (~ (UINT32) EFI_WIDE_ATTRIBUTE);
          }

          Index += CurrentWidth;
          WString++;
          Mode->CursorColumn += CurrentWidth;
        }
      }
      //
      // At the end of line, output carriage return and line feed
      //
      if (Mode->CursorColumn >= (INT32) MaxColumn) {
        DevNullTextOutOutputString (Private, mCrLfString);
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
DevNullTextOutSetMode (
  IN  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private,
  IN  UINTN                           ModeNumber
  )
/*++

  Routine Description:
    Sets the output device(s) to a specified mode.

  Arguments:
    Private    - Private data structure pointer.
    ModeNumber - The mode number to set.

  Returns:
    EFI_SUCCESS      - The requested text mode was set.
    EFI_DEVICE_ERROR - The device had an error and
                       could not complete the request.
    EFI_UNSUPPORTED - The mode number was not valid.
    EFI_OUT_OF_RESOURCES - Out of resources.

--*/
{
  UINTN                         Size;
  INT32                         CurrentMode;
  UINTN                         Row;
  UINTN                         Column;
  TEXT_OUT_SPLITTER_QUERY_DATA  *Mode;

  //
  // No extra check for ModeNumber here, as it has been checked in
  // ConSplitterTextOutSetMode. And mode 0 should always be supported.
  // Row and Column should be fetched from intersection map.
  //
  if (Private->TextOutModeMap != NULL) {
    CurrentMode = *(Private->TextOutModeMap + Private->TextOutListCount * ModeNumber);
  } else {
    CurrentMode = (INT32)(ModeNumber);
  }
  Mode    = &(Private->TextOutQueryData[CurrentMode]);
  Row     = Mode->Rows;
  Column  = Mode->Columns;

  if (Row <= 0 && Column <= 0) {
    return EFI_UNSUPPORTED;
  }

  if (Private->TextOutMode.Mode != (INT32) ModeNumber) {

    Private->TextOutMode.Mode = (INT32) ModeNumber;
    Private->DevNullColumns   = Column;
    Private->DevNullRows      = Row;

    gBS->FreePool (Private->DevNullScreen);
    Size                    = (Row * (Column + 1)) * sizeof (CHAR16);
    Private->DevNullScreen  = AllocateZeroPool (Size);
    if (Private->DevNullScreen == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    if (Private->IsConOutPrivateData) {
      PcdSet64(PcdDevNullScreenPtr, (UINT64)(UINTN)Private->DevNullScreen);
    }

    gBS->FreePool (Private->DevNullAttributes);
    Size                        = Row * Column * sizeof (INT32);
    Private->DevNullAttributes  = AllocateZeroPool (Size);
    if (Private->DevNullAttributes == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    if (Private->IsConOutPrivateData) {
      PcdSet64(PcdDevNullAttributes, (UINT64)(UINTN)Private->DevNullAttributes);
    }
    
  }

  DevNullTextOutClearScreen (Private);

  return EFI_SUCCESS;
}

EFI_STATUS
DevNullTextOutClearScreen (
  IN  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private
  )
/*++

  Routine Description:
    Clears the output device(s) display to the currently selected background
    color.

  Arguments:
    Private     - Protocol instance pointer.

  Returns:
    EFI_SUCCESS      - The operation completed successfully.
    EFI_DEVICE_ERROR - The device had an error and
                       could not complete the request.
    EFI_UNSUPPORTED - The output device is not in a valid text mode.

--*/
{
  UINTN   Row;
  UINTN   Column;
  CHAR16  *Screen;
  INT32   *Attributes;
  INT32   CurrentAttribute;

  //
  // Clear the DevNull Text Out Buffers.
  // The screen is filled with spaces.
  // The attributes are all synced with the current Simple Text Out Attribute
  //
  Screen            = Private->DevNullScreen;
  Attributes        = Private->DevNullAttributes;
  CurrentAttribute  = Private->TextOutMode.Attribute;

  for (Row = 0; Row < Private->DevNullRows; Row++) {
    for (Column = 0; Column < Private->DevNullColumns; Column++, Screen++, Attributes++) {
      *Screen     = ' ';
      *Attributes = CurrentAttribute;
    }
    //
    // Each line of the screen has a NULL on the end so we must skip over it
    //
    Screen++;
  }

  DevNullTextOutSetCursorPosition (Private, 0, 0);
  Private->IsDevNullBlankScreen = TRUE;

  return DevNullTextOutEnableCursor (Private, TRUE);
}

EFI_STATUS
DevNullTextOutSetCursorPosition (
  IN  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private,
  IN  UINTN                           Column,
  IN  UINTN                           Row
  )
/*++

  Routine Description:
    Sets the current coordinates of the cursor position

  Arguments:
    Private       - Protocol instance pointer.
    Column, Row - the position to set the cursor to. Must be greater than or
                  equal to zero and less than the number of columns and rows
                  by QueryMode ().

  Returns:
    EFI_SUCCESS      - The operation completed successfully.
    EFI_DEVICE_ERROR - The device had an error and
                       could not complete the request.
    EFI_UNSUPPORTED - The output device is not in a valid text mode, or the
                       cursor position is invalid for the current mode.

--*/
{
  //
  // No need to do extra check here as whether (Column, Row) is valid has
  // been checked in ConSplitterTextOutSetCursorPosition. And (0, 0) should
  // always be supported.
  //
  Private->TextOutMode.CursorColumn = (INT32) Column;
  Private->TextOutMode.CursorRow    = (INT32) Row;

  return EFI_SUCCESS;
}

EFI_STATUS
DevNullTextOutEnableCursor (
  IN  TEXT_OUT_SPLITTER_PRIVATE_DATA  *Private,
  IN  BOOLEAN                         Visible
  )
/*++
  Routine Description:
  
    Implements SIMPLE_TEXT_OUTPUT.EnableCursor().
    In this driver, the cursor cannot be hidden.
  
  Arguments:
  
    Private - Indicates the calling context.
        
    Visible - If TRUE, the cursor is set to be visible, If FALSE, the cursor
              is set to be invisible.

  Returns:

    EFI_SUCCESS - The request is valid.
       
               
--*/
{
  Private->TextOutMode.CursorVisible = Visible;

  return EFI_SUCCESS;
}

