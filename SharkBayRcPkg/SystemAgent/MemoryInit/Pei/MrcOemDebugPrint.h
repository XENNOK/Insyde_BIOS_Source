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

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/


#ifndef _MRC_OEM_DEBUG_PRINT_H_
#define _MRC_OEM_DEBUG_PRINT_H_
#pragma pack (push, 1)

#include <MrcTypes.h>

typedef char  *MrcVaList;

#ifdef EFI_DEBUG
//[-start-140610-IB05400532-modify]//
#if !defined(__GNUC__)
#define MRC_DEBUG_PRINT (1)
#endif
//[-end-140610-IB05400532-modify]//
#endif // EFI_DEBUG
#define TRANSMIT_HOLDING        (0x00)

#define LINE_STATUS             (0x05)
#define TRANS_HOLDING_REG_EMPTY (0x20)
#define DATA_READY              (0x01)

///
/// com port options
///
#define MRC_COM1_BASE (0x3f8)
#define MRC_COM2_BASE (0x2f8)
#define MRC_COM3_BASE (0x3e8)
#define MRC_COM4_BASE (0x2e8)

///
/// select the platform com port address
///
#define GLOBALCOMPORT (MRC_COM1_BASE)                                   ///< 0x3F8-0x3FF
#ifndef MRC_LEFT_JUSTIFY
#define MRC_LEFT_JUSTIFY  (0x01)
#endif
#ifndef MRC_PREFIX_SIGN
#define MRC_PREFIX_SIGN (0x02)
#endif
#ifndef MRC_PREFIX_BLANK
#define MRC_PREFIX_BLANK  (0x04)
#endif
#ifndef MRC_COMMA_TYPE
#define MRC_COMMA_TYPE  (0x08)
#endif
#ifndef MRC_LONG_TYPE
#define MRC_LONG_TYPE (0x10)
#endif
#ifndef MRC_PREFIX_ZERO
#define MRC_PREFIX_ZERO (0x20)
#endif
#ifndef MRC_INT_SIGNED
#define MRC_INT_SIGNED  (0x40)
#endif

#define MRC_CHAR_LF         (0x0A)
#define MRC_CHAR_CR         (0x0D)
#define MRC_INT32_MAX       (0x7FFFFFFF)
#define MAX_STRING_LENGTH   160
#define MRC_PUTCC(_str, _c, _CharCount) { \
    _str[_CharCount] = _c; \
    if (_CharCount < (MAX_STRING_LENGTH - 1)) {_CharCount++;}; \
  }
#define MRC_ISDIGIT(_c) (((_c) >= '0') && ((_c) <= '9'))

#ifdef MRC_DEBUG_PRINT
#define MRC_DEBUG_MSG_OPEN(DEBUG, LEVEL, FILE, BUFFER, SIZE) MrcOemFopen (DEBUG, LEVEL, FILE, BUFFER, SIZE)
#define MRC_DEBUG_MSG(DEBUG, LEVEL, FORMAT, ...)             MrcOemPrintf (DEBUG, LEVEL, FORMAT, __VA_ARGS__)
#define MRC_DEBUG_TEXT(arg)                                  (arg)
#else
#define MRC_DEBUG_MSG_OPEN(DEBUG, LEVEL, FILE, BUFFER, SIZE)
#define MRC_DEBUG_MSG(DEBUG, LEVEL, FORMAT, ...)
#define MRC_DEBUG_TEXT(arg)
#endif

typedef enum {
  MSG_LEVEL_NEVER,
  MSG_LEVEL_ERROR,
  MSG_LEVEL_WARNING,
  MSG_LEVEL_NOTE,
  MSG_LEVEL_EVENT,
  MSG_LEVEL_TIME,
  MSG_LEVEL_ALL     = MRC_INT32_MAX
} MrcDebugMsgLevel;

typedef struct {
  U32 Stream;
  U32 Start;
  U32 End;
  U32 Current;
  int Level;
  U16 PostCode[2];
} MrcDebug;

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
extern
U32
OemUintnToStr (
  IN  const U32 Value,
  OUT char      *Str,
  IN  const U32 Width,
  IN  const U32 Flags,
  IN  const U32 Base
  );

/**
@brief
  Convert a string to a number.

  @param[in, out] String - String to convert.

  @retval Returns the string in number.
**/
extern
U32
OemStrToNumber (
  IN OUT char **String
  );

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

extern
int
StringFormatter (
  IN const char *const      Format,
  IN MrcVaList              Marker,
  IN U32                    BufferSize,
  IN OUT U8                 *Buffer
  );

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
extern
int
MrcOemPrintfVaList (
  IN const MrcDebug *const  Debug,
  IN const MrcDebugMsgLevel Level,
  IN const char *const      Format,
  IN MrcVaList              Marker
  );

/**
@brief
  put char in the uart device.

  @param[in] c   - char to put in the uart.

  @retval Returns the puted char.
**/
extern
int
MrcOemPrintf (
  IN const MrcDebug *const  Debug,
  IN const MrcDebugMsgLevel Level,
  IN const char *const      Format,
  ...
  );

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
extern
void
MrcOemFopen (
  OUT MrcDebug *const        Debug,
  IN  const MrcDebugMsgLevel Level,
  IN  U32                    Stream,
  IN  U32                    Buffer,
  IN  U32                    Size
  );

#pragma pack (pop)
#endif //_MrcOemDebugPrint_h_
