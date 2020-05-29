/** @file
 Basic print functions for UI
 Basic Ascii AvSPrintf() function named VSPrint(). VSPrint() enables very
 simple implemenation of SPrint() and Print() to support debug.
 You can not Print more than EFI_DRIVER_LIB_MAX_PRINT_BUFFER characters at a
 time. This makes the implementation very simple.
 VSPrint, Print, SPrint format specification has the follwoing form
 %type
 type:
 'S','s' - argument is an Unicode string
 'c' - argument is an ascii character
 '%' - Print a %

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

#include "Setup.h"

/**
 The internal function prints to the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL protocol instance.
 Display string worker for: Print, PrintAt, IPrint, IPrintAt

 @param[in] Column          The position of the output string.
 @param[in] Row             The position of the output string.
 @param[in] Out             The EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL instance.
 @param[in] Fmt             The format string.
 @param[in] Args            The additional argument for the variables in the format string.

 @return Number of Unicode character printed.
**/
UINTN
PrintInternal (
  IN UINTN                            Column,
  IN UINTN                            Row,
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *Out,
  IN CHAR16                           *fmt,
  IN VA_LIST                          args
  )
{
  CHAR16  *Buffer;
  CHAR16  *BackupBuffer;
  UINTN   Index;
  UINTN   PreviousIndex;

  if (gGraphicsEnabled) {
    return 0;
  }

  //
  // For now, allocate an arbitrarily long buffer
  //
  Buffer        = AllocateZeroPool (0x10000);
  BackupBuffer  = AllocateZeroPool (0x10000);
  ASSERT (Buffer);
  ASSERT (BackupBuffer);
  if (Buffer == NULL || BackupBuffer == NULL) {
    return 0;
  }

  if (Column != (UINTN) -1) {
    Out->SetCursorPosition (Out, Column, Row);
  }

  UnicodeVSPrint  (Buffer, 0x10000, fmt, args);

  Out->Mode->Attribute = Out->Mode->Attribute & 0x7f;
  Out->SetAttribute (Out, Out->Mode->Attribute);

  Index         = 0;
  PreviousIndex = 0;

  do {
    for (; (Buffer[Index] != NARROW_CHAR) && (Buffer[Index] != WIDE_CHAR) && (Buffer[Index] != 0); Index++) {
      BackupBuffer[Index] = Buffer[Index];
    }

    if (Buffer[Index] == 0) {
      break;
    }
    //
    // Null-terminate the temporary string
    //
    BackupBuffer[Index] = 0;

    //
    // Print this out, we are about to switch widths
    //
    Out->OutputString (Out, &BackupBuffer[PreviousIndex]);

    //
    // Preserve the current index + 1, since this is where we will start printing from next
    //
    PreviousIndex = Index + 1;

    //
    // We are at a narrow or wide character directive.  Set attributes and strip it and print it
    //
    if (Buffer[Index] == NARROW_CHAR) {
      //
      // Preserve bits 0 - 6 and zero out the rest
      //
      Out->Mode->Attribute = Out->Mode->Attribute & 0x7f;
      Out->SetAttribute (Out, Out->Mode->Attribute);
    } else {
      //
      // Must be wide, set bit 7 ON
      //
      Out->Mode->Attribute = Out->Mode->Attribute | EFI_WIDE_ATTRIBUTE;
      Out->SetAttribute (Out, Out->Mode->Attribute);
    }

    Index++;
  } while (Buffer[Index] != 0);

  //
  // We hit the end of the string - print it
  //
  Out->OutputString (Out, &BackupBuffer[PreviousIndex]);

  gBS->FreePool (Buffer);
  gBS->FreePool (BackupBuffer);

  return EFI_SUCCESS;
}

/**
 Prints a formatted unicode string to the default console.

 @param[in] Fmt     Format string
 @param[in] ...     Variable argument list for format string.

 @return Length of string printed to the console.
**/
UINTN
ConsolePrint (
  IN CHAR16   *fmt,
  ...
  )
{
  VA_LIST args;

  VA_START (args, fmt);
  return PrintInternal ((UINTN) -1, (UINTN) -1, gST->ConOut, fmt, args);
}

/**
 Prints a unicode string to the default console,
 using L"%s" format.

 @param[in] String     String pointer.

 @return Length of string printed to the console
**/
UINTN
PrintString (
  IN CHAR16       *String
  )
{
  return ConsolePrint (L"%s", String);
}

/**
 Prints a chracter to the default console,
 using L"%c" format.

 @param[in] Character  Character to print.

 @return Length of string printed to the console.
**/
UINTN
PrintChar (
  IN CHAR16       Character
  )
{
  return ConsolePrint (L"%c", Character);
}

/**
 Prints a formatted unicode string to the default console, at
 the supplied cursor position.

 @param[in] Column     The cursor position to print the string at.
 @param[in] Row        The cursor position to print the string at.
 @param[in] Fmt        Format string.
 @param[in] ...        Variable argument list for format string.

 @return Length of string printed to the console
**/
UINTN
PrintAt (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    *fmt,
  ...
  )
{
  VA_LIST args;

  VA_START (args, fmt);
  return PrintInternal (Column, Row, gST->ConOut, fmt, args);
}

/**
 Prints a unicode string to the default console, at
 the supplied cursor position, using L"%s" format.

 @param[in] Column     The cursor position to print the string at.
 @param[in] Row        The cursor position to print the string at
 @param[in] String     String pointer.

 @return Length of string printed to the console
**/
UINTN
PrintStringAt (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    *String
  )
{
  return PrintAt (Column, Row, L"%s", String);
}

/**
 Prints a chracter to the default console, at
 the supplied cursor position, using L"%c" format.

 @param[in] Column     The cursor position to print the string at.
 @param[in] Row        The cursor position to print the string at.
 @param[in] Character  Character to print.

 @return Length of string printed to the console.
**/
UINTN
PrintCharAt (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    Character
  )
{
  return PrintAt (Column, Row, L"%c", Character);
}

