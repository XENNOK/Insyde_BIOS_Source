/** @file

  AT authetication module for using AT DXE driver.
  This library is utilized by the AT and related drivers to implement Theft Deterrence Technology AM module.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _ATAM_HELPER_H_
#define _ATAM_HELPER_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

#define PEM_LINE_SIZE         64
#define PEM_INPUT_LINE_SIZE   48
#define PEM_UNITS_PER_LINE    (PEM_INPUT_LINE_SIZE / 3)
#define PEM_DECODED_LINE_SIZE 48
#define PEM_INVALID_CHAR      255
#define PEM_PAD_CHAR          64

/**
  Convert Hex values into Base32 values

  @param[in] encodedStr           Array of encoded BASE values.
  @param[in] encodedLen           Length of encoded values.
  @param[out] rawData             Array of Hex strings that needs to be encoded into BASE32.
  @param[out] rawDataLen          Length of Hex values.

  @retval EFI_BUFFER_TOO_SMALL   Buffer to store encoded string too small.
  @retval EFI_SUCCESS            Initialization complete.
**/
EFI_STATUS
Base32Encode (
  IN UINT8                        *encodedStr,
  IN UINTN                        *encodedLen,
  OUT UINT8                       *rawData,
  OUT UINTN                       rawDataLen
  );

/**
  Convert Base32 values into Hex values.

  @param[in] encodedStr           Array of Decimal numbers.
  @param[out] decodedData         Converted Hex values.

  @retval   Length of the Hex Strings that expected.
**/
INTN
Base32Decode (
  IN UINT8                        *encodedStr,
  IN UINT8                        *decodedData
  );

/**
  Decimal large (BASE10) into hex value.

  @param[in][out] decStr          Array of Decimal numbers.
  @param[out] u8Hex               Converted Hex values.
  @param[in] hexIndex             Length of the Hex Strings that expected

  @retval None
**/
VOID
DecimalToHexString (
  IN OUT CHAR8                    *decStr,
  OUT UINT8                       *u8Hex,
  IN UINTN                        hexIndex
  );

/**
  Convert the CHAR8 ASCII into CHAR16 Unicode strings.

  @param[in] AsciiString          Ascii String.
  @param[out] UnicodeString Buffer for converted Unicode string.

  @retval None
**/
VOID
Uint8ToUnicode (
  IN  CHAR8                       *AsciiString,
  OUT CHAR16                      *UnicodeString
);

/**
  This routine displays the debug message in ASCII

  @param[in] Message              Message to be displayed
  @param[in] Length               Length of the message

  @retval None
**/
VOID
ShowBuffer (
  IN UINT8                        *Message,
  IN UINT32                       Length
  );

#endif /// _ATAM_HELPER_H_
