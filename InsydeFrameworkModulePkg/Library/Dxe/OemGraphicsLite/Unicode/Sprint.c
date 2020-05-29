//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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
//;   Sprint.h
//;

#include "TianoCommon.h"
#include "PrintWidth.h"
#include "EfiPrintLib.h"
#include "Print.h"


UINTN
ASPrint (
  OUT CHAR8         *Buffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR8   *Format,
  ...
  )
/*++

Routine Description:

  Process format and place the results in Buffer for narrow chars.

Arguments:

  Buffer      - Narrow char buffer to print the results of the parsing of Format into.
  BufferSize  - Maximum number of characters to put into buffer.
  Format      - Format string
  ...         - Vararg list consumed by processing Format.

Returns:

  Number of characters printed.

--*/
{
  UINTN   Return;
  VA_LIST Marker;

  VA_START (Marker, Format);
  Return = AvSPrint (Buffer, BufferSize, Format, Marker);
  VA_END (Marker);

  return Return;
}


UINTN
AvSPrint (
  OUT CHAR8         *Buffer,
  IN  UINTN         BufferSize,
  IN  CONST CHAR8   *FormatString,
  IN  VA_LIST       Marker
  )
/*++

Routine Description:

  Internal implementation of ASPrint.
  Process format and place the results in Buffer for narrow chars.

Arguments:

  Buffer        - Narrow char buffer to print the results of the parsing of Format into.
  BufferSize    - Maximum number of characters to put into buffer.
  FormatString  - Format string
  Marker        - Vararg list consumed by processing Format.

Returns:

  Number of characters printed.

--*/
{
  UINTN   Index;
  CHAR16  UnicodeFormat[EFI_DRIVER_LIB_MAX_PRINT_BUFFER];
  CHAR16  UnicodeResult[EFI_DRIVER_LIB_MAX_PRINT_BUFFER];

  for (Index = 0; Index < EFI_DRIVER_LIB_MAX_PRINT_BUFFER && FormatString[Index] != '\0'; Index++) {
    UnicodeFormat[Index] = (CHAR16) FormatString[Index];
  }

  UnicodeFormat[Index]  = '\0';

  Index                 = VSPrint (UnicodeResult, EFI_DRIVER_LIB_MAX_PRINT_BUFFER, UnicodeFormat, Marker);

  for (Index = 0; (Index < (BufferSize - 1)) && UnicodeResult[Index] != '\0'; Index++) {
    Buffer[Index] = (CHAR8) UnicodeResult[Index];
  }

  Buffer[Index] = '\0';

  return Index++;
}
