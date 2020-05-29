/** @file
  Output debug messages to the debug port.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement.

**/

#include <MrcTypes.h>
#include <MrcDebugHook.h>
#include <MrcGlobal.h>
#include <MrcOemDebugPrint.h>
#include <MrcOemMemory.h>
#include <Library/DebugLib.h>

#ifdef MRC_DEBUG_PRINT
#define ASCII_ETX (3)

/**
@brief
  Convert an unsigned integer to a string.

  @param[in]  Value           - Value to work on.
  @param[out] Str             - The return string to print.
  @param[in]  Width           - The width of string to print
  @param[in]  Flags           - The flag type to print out, like '-' or '+'.
  @param[in]  Base            - Number base to work on, as in 10, or 16.

  @retval Number of characters in the resulting string.
**/
U32
OemUintnToStr (
  IN  const U32 Value,
  OUT char      *Str,
  IN  const U32 Width,
  IN  const U32 Flags,
  IN  const U32 Base
  )
{
  char  *Ptr;
  U32   Negative;
  U32   Int;
  U32   i;
  char  Prefix;
  char  c;
  const char Hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

  Ptr = Str;
  if ((Value > MRC_INT32_MAX) && (Flags & MRC_INT_SIGNED)) {
    Int       = ~Value + 1; /* -Value */
    Negative  = 1;
  } else {
    Int       = Value;
    Negative  = 0;
  }

  i = 0;
  do {           /* Generate digits in reverse order */
    i++;
    *Ptr++ = Hex[Int % Base];
    if (Flags & MRC_COMMA_TYPE) {
      if (Base == 16) {
        if (i % 4 == 0) {
          *Ptr++ = ',';
        }
      } else if (Base == 10) {
        if ((i % 3) == 0) {
          *Ptr++ = ',';
        }
      }
    }
  } while ((Int /= Base) > 0);
  if (*(Ptr - 1) == ',') {
    Ptr--;
  }

  if (Negative) {
    *Ptr++ = '-';
    i++;
  } else if (Flags & MRC_PREFIX_SIGN) {
    *Ptr++ = '+';
    i++;
  }

  if (Flags & MRC_PREFIX_ZERO) {
    Prefix = '0';
  } else if (!(Flags & MRC_LEFT_JUSTIFY)) {
    Prefix = ' ';
  } else {
    Prefix = 0x00;
  }

  if (Prefix != 0x00) {
    for (i = (int) (Ptr - Str); i < Width; i++) {
      *Ptr++ = Prefix;
    }
  }

  *Ptr = '\0';

  /* Reverse string */
  while (Str < --Ptr) {
    c       = *Str;
    *Str++  = *Ptr;
    *Ptr    = c;
  }

  return i;
}

/**
@brief
  Convert a string to a number.

  @param[in, out] String - String to convert.

  @retval Returns the string in number.
**/
U32
OemStrToNumber (
  IN OUT char **String
  )
{
  U32   Sum;
  char  *Str;

  Str = *String;
  if (*Str == '0') {
    Str++;
  }

  Sum = 0;
  while (MRC_ISDIGIT (*Str)) {
    Sum = Sum * 10 + (*Str++ -'0');
  }

  *String = Str;
  return Sum;
}

/*++
@brief
  Format string using specified format specifier. Limited support for sizes other than
  unsigned 32-bit to save code space. Type overrides like {h | I | I64 | L}
  are not supported.

  @param[in] Format     - String containing characters to print and formatting data.
    %[flags][width]type
    [flags] '-' left align
    [flags] '+' prefix with sign (+ or -)
    [flags] '0' zero pad numbers
    [flags] ' ' prefix black in front of postive numbers
    [width] non negative decimal integer that specifies the width to print a value.
    [width] '*' get the width from a int argument on the stack.
    type    'd'|'i' signed decimal integer
    type    'u' unsigned integer
    type    'x'|'X' hexidecimal using "ABCDEF"
    type    'c' print character
    type    'p' print a pointer to void
    type    's' print a null terminated string
  @param[in] Marker     - Variable argument marker.
  @param[in] BufferSize - Size of the buffer, in bytes.
  @param[in] Buffer     - The buffer.

  @retval Number of characters printed.
**/
int
StringFormatter (
  IN const char *const      Format,
  IN MrcVaList              Marker,
  IN U32                    BufferSize,
  IN OUT U8                 *Buffer
  )
{
  char     *p;
  char     *String;
  U32      Width;
  U32      Flags;
  U32      CharCount;

  CharCount = 0;
  if (Format != NULL) {
    for (p = (char *) Format; *p && (CharCount < BufferSize); p++) {
      if (*p != '%') {
        if (*p == MRC_CHAR_LF) {
          //
          // Make LF into CR LF
          //
          MRC_PUTCC (Buffer, MRC_CHAR_CR, CharCount);
        }

        MRC_PUTCC (Buffer, *p, CharCount);
      } else {
        p++;
        //
        // Check for flags
        //
        Flags = 0;
        if (*p == '-') {
          Flags |= MRC_LEFT_JUSTIFY;
        } else if (*p == '+') {
          Flags |= MRC_PREFIX_SIGN;
        } else if (*p == ' ') {
          Flags |= MRC_PREFIX_BLANK;
        }

        if (Flags != 0) {
          p++;
        }
        //
        // Check for width
        //
        if (MRC_ISDIGIT (*p)) {
          if (*p == '0') {
            Flags |= MRC_PREFIX_ZERO;
          }

          Width = OemStrToNumber (&p);
        } else if (*p == '*') {
          Width = VA_ARG (Marker, int);
          p++;
        } else {
          Width = 0;
        }

        if (*p == ',') {
          Flags |= MRC_COMMA_TYPE;
          p++;
        }
        //
        // Get type
        //
        switch (*p) {
          case 'd':
          case 'i':
            //
            // Always print as UINTN. Will need extra code to print different widths.
            //
            CharCount = CharCount + OemUintnToStr (
              (U32) VA_ARG (Marker, U32 *),
              (char *) &Buffer[CharCount],
              Width,
              Flags | MRC_INT_SIGNED,
              10
              );
            break;

          case 'u':
            //
            // Always print as UINTN. Will need extra code to print different widths.
            //
            CharCount = CharCount + OemUintnToStr ((U32) VA_ARG (Marker, U32 *), (char *) &Buffer[CharCount], Width, Flags, 10);
            break;

          case 'x':
          case 'X':
          case 'p':
            //
            // Always print as UINTN. Will need extra code to print different widths.
            //
            CharCount = CharCount + OemUintnToStr ((U32) VA_ARG (Marker, U32 *), (char *) &Buffer[CharCount], Width, Flags, 16);
            break;

          case 'c':
            MRC_PUTCC (Buffer, VA_ARG (Marker, char), CharCount);
            Buffer[CharCount] = '\0';
            break;

          case 's':
            String = (char *) VA_ARG (Marker, char *);
            while (*String != '\0') {
              MRC_PUTCC (Buffer, *String++, CharCount);
            }
            break;
        }
      }
    }

    if (CharCount < BufferSize) {
      MRC_PUTCC (Buffer, '\0', CharCount);
    } else {
      Buffer[BufferSize - 1] = '\0';
    }
  }

  return CharCount;
}

/*++

@brief
  Print to output stream/device. Limited support for sizes other than
  unsigned 32-bit to save code space. Type overrides like {h | I | I64 | L}
  are not supported.

  @param[in] Debug  - Location to store debug message print information for future use.
  @param[in] Level  - The debug level.
  @param[in] Format - String containing characters to print and formatting data.
    %[flags][width]type
    [flags] '-' left align
    [flags] '+' prefix with sign (+ or -)
    [flags] '0' zero pad numbers
    [flags] ' ' prefix black in front of postive numbers
    [width] non negative decimal integer that specifies the width to print a value.
    [width] '*' get the width from a int argument on the stack.
    type    'd'|'i' signed decimal integer
    type    'u' unsigned integer
    type    'x'|'X' hexidecimal using "ABCDEF"
    type    'c' print character
    type    'p' print a pointer to void
    type    's' print a null terminated string
  @param[in] Marker - Variable argument marker

  @retval Number of characters printed.
**/
int
MrcOemPrintfVaList (
  IN const MrcDebug *const  Debug,
  IN const MrcDebugMsgLevel Level,
  IN const char *const      Format,
  IN MrcVaList              Marker
  )
{
  MrcDebug *Dbg;
  U8       *String;
  U32      CharCount;
  U8       Buffer[MAX_STRING_LENGTH];

  CharCount = 0;
  if ((Format != NULL) && (Level != MSG_LEVEL_NEVER)) {
    if (((Debug->Level == MSG_LEVEL_TIME) && (Level == MSG_LEVEL_TIME)) || ((Debug->Level != MSG_LEVEL_TIME) && (Level <= Debug->Level))) {
      CharCount = StringFormatter (Format, Marker, sizeof (Buffer), Buffer);

      //
      // Write the string to the serial log buffer.
      //
      if (Debug->Current > 0) {
        Dbg    = (MrcDebug *) Debug;
        String = Buffer;
        while (*String != '\0') {
          if (Dbg->Current >= Dbg->End) {
            Dbg->Current = Dbg->Start;
          }
          *((U8 *) (Dbg->Current)) = *String++;
          Dbg->Current++;
        }
        //
        // Write a "end of text" marker to the buffer but don't increment the current pointer.
        //
        if (Dbg->Current >= Dbg->End) {
          Dbg->Current = Dbg->Start;
        }
        *((U8 *) (Dbg->Current)) = ASCII_ETX;
      }

      if (Debug->Stream > 0) {
#ifdef MRC_MINIBIOS_BUILD
        puts ((char *) Buffer);
#else
        DEBUG ((EFI_D_ERROR, Buffer));
#endif
      }
    }
  }
  return CharCount;
}

/**
@brief
  Print to output stream/device.  Gets the start Marker for the variable arguments
  and calls MrcOemPrintfVaList().

  @param[in] Debug  - Location to store debug message print information for future use.
  @param[in] Level  - The debug level.
  @param[in] Format - String containing characters to print and formatting data.
           %[flags][width]type
           [flags] '-' left align
           [flags] '+' prefix with sign (+ or -)
           [flags] '0' zero pad numbers
           [flags] ' ' prefix black in front of postive numbers
           [width] non negative decimal integer that specifies the width to print a value.
           [width] '*' get the width from a int argument on the stack.
           type    'd'|'i' signed decimal integer
           type    'u' unsigned integer
           type    'x'|'X' hexidecimal using "ABCDEF"
           type    'c' print character
           type    'p' print a pointer to void
           type    's' print a null terminated string
  @param[in] ...    - Variable number of arguments to print

  @retval Number of characters printed.
**/
int
MrcOemPrintf (
  IN const MrcDebug *const  Debug,
  IN const MrcDebugMsgLevel Level,
  IN const char *const      Format,
  ...
  )
{
  MrcVaList Marker;

  VA_START (Marker, Format);

  return MrcOemPrintfVaList (Debug, Level, Format, Marker);
}

/**
@brief
  Save the output stream/device. This function must be called prior to debug
  string output.

  @param[out] Debug  - Location to store debug message print information for future use.
  @param[in]  Level  - The debug level to set. Messages at or above this level are printed.
  @param[in]  Stream - Pointer to the stream/device to use.
  @param[in]  Buffer - Pointer to the buffer that will be used to 'tee' the data to.
  @param[in]  Size   - Size of the 'tee' buffer.

  @retval Nothing.
**/
void
MrcOemFopen (
  OUT MrcDebug *const        Debug,
  IN  const MrcDebugMsgLevel Level,
  IN  U32                    Stream,
  IN  U32                    Buffer,
  IN  U32                    Size
  )
{
  const U8 StringBegin[] = "ISV>";
  const U8 StringEnd[]   = "<ISV";

  Debug->Level        = Level;
  Debug->Stream       = Stream;
  Debug->PostCode[0]  = 0;
  if ((Buffer > 0) && (Size > (sizeof (StringBegin) + sizeof (StringEnd)))) {
    Debug->Start   = Buffer + (sizeof (StringBegin) - 1);
    Debug->End     = Buffer + Size - (sizeof (StringEnd) - 1);
    Debug->Current = Debug->Start;
    MrcOemMemorySet ((U8 *) Debug->Start, ' ', Size - (sizeof (StringBegin) + sizeof (StringEnd) - 2));
    MrcOemMemoryCpy ((U8 *) Buffer, (U8 *) StringBegin, sizeof (StringBegin) - 1);
    MrcOemMemoryCpy ((U8 *) Debug->End, (U8 *) StringEnd, sizeof (StringEnd) - 1);
  } else {
    Debug->Start   = 0;
    Debug->End     = 0;
    Debug->Current = 0;
  }

  return;
}

#endif // MRC_DEBUG_PRINT
