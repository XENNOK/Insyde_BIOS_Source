/** @file
  Hot plug support for H2O display engine

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

#include "DEHotPlug.h"

EFI_HANDLE                                 *mDelayDetachDevList;
UINT32                                     mDelayDetachDevCount;

EFI_SIMPLE_TEXT_OUTPUT_MODE                mFakeSimpleTextOutputMode;
EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL            mFakeSimpleTextOutput = {
  FakeTextOutputReset,
  FakeTextOutputOutputString,
  FakeTextOutputTestString,
  FakeTextOutputQueryMode,
  FakeTextOutputSetMode,
  FakeTextOutputSetAttribute,
  FakeTextOutputClearScreen,
  FakeTextOutputSetCursorPosition,
  FakeTextOutputEnableCursor,
  &mFakeSimpleTextOutputMode
  };

EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE          mFakeGraphicsOutMode;
EFI_GRAPHICS_OUTPUT_PROTOCOL               mFakeGraphicsOut = {
  FakeGraphicsOutputQueryMode,
  FakeGraphicsOutputSetMode,
  FakeGraphicsOutputBlt,
  &mFakeGraphicsOutMode
  };

EFI_STATUS
EFIAPI
FakeTextOutputReset (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN BOOLEAN                               ExtendedVerification
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FakeTextOutputOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN CHAR16                                *WString
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FakeTextOutputTestString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN CHAR16                                *WString
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FakeTextOutputQueryMode (
  IN  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL      *This,
  IN  UINTN                                ModeNumber,
  OUT UINTN                                *Columns,
  OUT UINTN                                *Rows
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeTextOutputSetMode (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN UINTN                                 ModeNumber
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeTextOutputSetAttribute (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN UINTN                                 Attribute
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FakeTextOutputClearScreen (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FakeTextOutputSetCursorPosition (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN UINTN                                 Column,
  IN UINTN                                 Row
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FakeTextOutputEnableCursor (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *This,
  IN BOOLEAN                               Visible
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FakeGraphicsOutputQueryMode (
  IN  EFI_GRAPHICS_OUTPUT_PROTOCOL         *This,
  IN  UINT32                               ModeNumber,
  OUT UINTN                                *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION **Info
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeGraphicsOutputSetMode (
  IN EFI_GRAPHICS_OUTPUT_PROTOCOL          *This,
  IN UINT32                                ModeNumber
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
FakeGraphicsOutputBlt (
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
  return EFI_SUCCESS;
}

/**
  Add console device into target console device list

  @param[in]      ConsoleHandle       Console device handle
  @param[in, out] ConsoleDevList      Pointer to output console device list
  @param[in, out] ConsoleDevCount     Pointer to the number of output console device list

  @retval EFI_SUCCESS                 Add console device successfully
  @retval EFI_INVALID_PARAMETER       ConsoleDevList or ConsoleDevCount is NULL
  @retval EFI_OUT_OF_RESOURCES        Allocate pool fail

**/
EFI_STATUS
DEAddConsoleIntoList (
  IN     EFI_HANDLE                        ConsoleHandle,
  IN OUT EFI_HANDLE                        **ConsoleDevList,
  IN OUT UINT32                            *ConsoleDevCount
  )
{
  EFI_HANDLE                               *NewList;

  if (ConsoleDevList == NULL || ConsoleDevCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NewList = AllocatePool ((*ConsoleDevCount + 1) * sizeof (EFI_HANDLE));
  if (NewList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (*ConsoleDevList != NULL) {
    CopyMem (NewList, *ConsoleDevList, *ConsoleDevCount * sizeof (EFI_HANDLE));
    FreePool (*ConsoleDevList);
  }
  NewList[*ConsoleDevCount] = ConsoleHandle;

  *ConsoleDevList = NewList;
  *ConsoleDevCount += 1;

  return EFI_SUCCESS;
}


/**
  Replace fake protocol instance in the device data of ControllerHandle

  @param[in] Private            Pointer to display engine private data
  @param[in] ControllerHandle   Handle of device to be replaced

  @retval EFI_SUCCESS           Replace fake protocol instance in the device data successfully
  @retval EFI_NOT_FOUND         Can not find the corresponding device data for ControllerHandle

**/
EFI_STATUS
DEReplaceFakeConOutDev (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA       *Private,
  IN EFI_HANDLE                            ControllerHandle
  )
{
  LIST_ENTRY                               *Link;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE        *ConDevNode;

  if (IsListEmpty (&Private->ConsoleDevListHead)) {
    return EFI_NOT_FOUND;
  }

  Link = Private->ConsoleDevListHead.ForwardLink;
  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);
    if (ConDevNode->Handle == ControllerHandle) {
      if (ConDevNode->SimpleTextOut != NULL) {
        CopyMem (&mFakeSimpleTextOutputMode, ConDevNode->SimpleTextOut->Mode, sizeof (EFI_SIMPLE_TEXT_OUTPUT_MODE));
        ConDevNode->SimpleTextOut = &mFakeSimpleTextOutput;
      }

      if (ConDevNode->GraphicsOut != NULL) {
        CopyMem (&mFakeGraphicsOutMode, ConDevNode->GraphicsOut->Mode, sizeof (EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));
        ConDevNode->GraphicsOut = &mFakeGraphicsOut;
      }

      DEAddConsoleIntoList (ControllerHandle, &mDelayDetachDevList, &mDelayDetachDevCount);

      return EFI_SUCCESS;
    }

    if (IsNodeAtEnd (&Private->ConsoleDevListHead, Link)) {
      break;
    }

    Link = Link->ForwardLink;
  }

  return EFI_NOT_FOUND;
}

/**
  Remove all device data which is replaced with fake output protocol

  @param[in] Private            Pointer to display engine private data

  @retval EFI_SUCCESS           Remove all device data which is replaced with fake output protocol successfully

**/
EFI_STATUS
DERemoveFakeConOutDev (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA       *Private
  )
{
  H2O_FORM_BROWSER_CONSOLE_DEV             ConDev;
  UINT32                                   Index;

  if (mDelayDetachDevList == NULL || mDelayDetachDevCount == 0) {
    return EFI_NOT_FOUND;
  }

  ZeroMem (&ConDev, sizeof (H2O_FORM_BROWSER_CONSOLE_DEV));

  for (Index = 0; Index < mDelayDetachDevCount; Index++) {
    ConDev.Handle = mDelayDetachDevList[Index];
    Private->DisplayEngine.DetachConsole (&Private->DisplayEngine, &ConDev);
  }

  mDelayDetachDevCount = 0;
  FreePool (mDelayDetachDevList);
  mDelayDetachDevList = NULL;

  return EFI_SUCCESS;
}
