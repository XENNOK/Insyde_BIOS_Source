/** @file
  Functions for H2O display engine driver.

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

#include <H2ODisplayEngineLib.h>

BOOLEAN
IsSupportGraphicOut (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL              *SimpleTextOut
  )
{
  BOOLEAN                                         Result;
  EFI_STATUS                                      Status;
  EFI_HANDLE                                      *HandleBuffer;
  UINTN                                           NumberOfHandles;
  UINTN                                           Index;
  UINT8                                           *Instance;

  Result = FALSE;

  if (SimpleTextOut == NULL) {
    return Result;
  }

  NumberOfHandles = 0;
  HandleBuffer    = NULL;
  Status = gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiSimpleTextOutProtocolGuid,
                   NULL,
                   &NumberOfHandles,
                   &HandleBuffer
                   );
  if (EFI_ERROR (Status) || NumberOfHandles == 0 || HandleBuffer == NULL) {
    return Result;
  }

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiSimpleTextOutProtocolGuid,
                    (VOID**) &Instance
                    );
    if (EFI_ERROR (Status) || ((UINTN) Instance != (UINTN) SimpleTextOut)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiGraphicsOutputProtocolGuid,
                    (VOID**) &Instance
                    );
    if (!EFI_ERROR (Status)) {
      Result = TRUE;
      break;
    }
  }

  FreePool (HandleBuffer);

  return Result;
}

EFI_STATUS
DEConOutRest (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN BOOLEAN                                ExtendedVerification
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->Reset (ConDevNode->SimpleTextOut, ExtendedVerification);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutOutputString (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN CHAR16                                 *String
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->OutputString (ConDevNode->SimpleTextOut, String);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutTestString (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN CHAR16                                 *String
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->OutputString (ConDevNode->SimpleTextOut, String);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutQueryModeWithoutModeNumer (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  //IN UINTN                                  ModeNumber,
  OUT UINT32                                *Columns,
  OUT UINT32                                *Rows
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;
  UINTN                                     ColumnValue;
  UINTN                                     RowValue;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  ColumnValue = 0;
  RowValue    = 0;

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->QueryMode (
                                          ConDevNode->SimpleTextOut,
                                          ConDevNode->SimpleTextOut->Mode->Mode,
                                          &ColumnValue,
                                          &RowValue
                                          );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  }

  *Columns = (UINT32) ColumnValue;
  *Rows    = (UINT32) RowValue;

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutSetMode (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN UINTN                                  ModeNumber
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->SetMode (ConDevNode->SimpleTextOut, ModeNumber);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutSetAttribute (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN UINTN                                  Attribute
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->SetAttribute (ConDevNode->SimpleTextOut, Attribute);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutSetNarrowAttribute (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    ConDevNode->SimpleTextOut->Mode->Attribute = ConDevNode->SimpleTextOut->Mode->Attribute & NARROW_ATTRIBUTE;
    Status = ConDevNode->SimpleTextOut->SetAttribute (ConDevNode->SimpleTextOut, ConDevNode->SimpleTextOut->Mode->Attribute);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutSetWideAttribute (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    ConDevNode->SimpleTextOut->Mode->Attribute = ConDevNode->SimpleTextOut->Mode->Attribute | WIDE_ATTRIBUTE;
    Status = ConDevNode->SimpleTextOut->SetAttribute (ConDevNode->SimpleTextOut, ConDevNode->SimpleTextOut->Mode->Attribute);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutClearScreen (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->ClearScreen (ConDevNode->SimpleTextOut);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutSetCursorPosition (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN UINTN                                  Column,
  IN UINTN                                  Row
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->SetCursorPosition (ConDevNode->SimpleTextOut, Column, Row);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutEnableCursor (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA        *DEPrivateData,
  IN BOOLEAN                                Visible
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->SimpleTextOut->EnableCursor (ConDevNode->SimpleTextOut, Visible);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEConOutGetAttribute (
  IN    H2O_DISPLAY_ENGINE_PRIVATE_DATA     *DEPrivateData,
  OUT   INT32                               *Attribute
  )
{
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    *Attribute = ConDevNode->SimpleTextOut->Mode->Attribute;

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEGraphicsOutQueryMode (
  IN  H2O_DISPLAY_ENGINE_PRIVATE_DATA       *DEPrivateData,
  IN  UINT32                                ModeNumber,
  OUT UINTN                                 *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->GraphicsOut->QueryMode (ConDevNode->GraphicsOut, ModeNumber, SizeOfInfo, Info);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEGraphicsOutQueryModeWithoutModeNumer (
  IN  H2O_DISPLAY_ENGINE_PRIVATE_DATA       *DEPrivateData,
  //IN  UINT32                                ModeNumber,
  OUT UINT32                                *SizeOfInfo,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  **Info
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;
  UINTN                                     InfoSize;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->GraphicsOut->QueryMode (ConDevNode->GraphicsOut, ConDevNode->GraphicsOut->Mode->Mode, &InfoSize, Info);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    *SizeOfInfo = (UINT32) InfoSize;

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEGraphicsOutSetMode (
  IN  H2O_DISPLAY_ENGINE_PRIVATE_DATA       *DEPrivateData,
  IN  UINT32                                ModeNumber
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->GraphicsOut->SetMode (ConDevNode->GraphicsOut, ModeNumber);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

EFI_STATUS
DEGraphicsOutBlt (
  IN  H2O_DISPLAY_ENGINE_PRIVATE_DATA       *DEPrivateData,
  IN  EFI_GRAPHICS_OUTPUT_BLT_PIXEL         *BltBuffer   OPTIONAL,
  IN  EFI_GRAPHICS_OUTPUT_BLT_OPERATION     BltOperation,
  IN  UINTN                                 SourceX,
  IN  UINTN                                 SourceY,
  IN  UINTN                                 DestinationX,
  IN  UINTN                                 DestinationY,
  IN  UINTN                                 Width,
  IN  UINTN                                 Height,
  IN  UINTN                                 Delta         OPTIONAL
  )
{
  EFI_STATUS                                Status;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE         *ConDevNode;
  LIST_ENTRY                                *Link;


  Link = DEPrivateData->ConsoleDevListHead.ForwardLink;
  if (IsNull (&DEPrivateData->ConsoleDevListHead, Link)) {
    return EFI_NOT_FOUND;
  }

  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

    Status = ConDevNode->GraphicsOut->Blt (
                                        ConDevNode->GraphicsOut,
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
      return Status;
    }

    if (IsNodeAtEnd (&DEPrivateData->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  };

  return EFI_SUCCESS;
}

/**
  Get device path string

  @param[in] DevicePath          Target device path

  @return The device path string or NULL if input device path is NULL or fail to locate device path to text protocol

**/
CHAR16 *
GetDevicePathStr (
  IN EFI_DEVICE_PATH_PROTOCOL                *DevicePath
  )
{
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL           *DevicePathToText;
  EFI_STATUS                                 Status;

  if (DevicePath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID**) &DevicePathToText
                  );
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  return DevicePathToText->ConvertDevicePathToText (DevicePath, TRUE, TRUE);
}

