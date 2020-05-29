/** @file
  This file contains the memory manipulation functions.

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

#include <Library/PeiMemoryLib/MemLibInternals.h>
#include <MrcTypes.h>
#include <MrcOemMemory.h>

/**
@brief
  Copy the specified number of memory bytes, a byte at a time, from the
  specified source to the specified destination.

  @param[in, out] Dest            - Destination pointer.
  @param[in]      Src             - Source pointer.
  @param[in]      NumBytes        - The number of bytes to copy.

  @retval Nothing.
**/
void
MrcOemMemoryCpy (
  IN OUT U8 *Dest,
  IN U8     *Src,
  IN U32    NumBytes
  )
{
  CopyMem (Dest, Src, NumBytes);
  return;
}

/**
@brief
  Sets the specified number of memory bytes, a byte at a time, at the
  specified destination.

  @param[in, out] Dest            - Destination pointer.
  @param[in]      Value           - The value to set.
  @param[in]      NumBytes        - The number of bytes to set.

  @retval Nothing.
**/
void
MrcOemMemorySet (
  IN OUT U8 *Dest,
  IN U32    Value,
  IN U32    NumBytes
  )
{
  SetMem ((U8 *) Dest, NumBytes, (U8) Value);
  return;
}

/**
@brief
  Sets the specified number of memory dwords, a dword at a time, at the
  specified destination.

  @param[in, out] Dest            - Destination pointer.
  @param[in]      Value           - The value to set.
  @param[in]      NumWords        - The number of dwords to set.

  @retval Nothing.
**/
void
MrcOemMemorySetWord (
  IN OUT U16    *Dest,
  IN const U16  Value,
  IN U32        NumWords
  )
{
  while (0 != NumWords--) {
    *Dest++ = Value;
  }

  return;
}

/**
@brief
  Sets the specified number of memory dwords, a dword at a time, at the
  specified destination.

  @param[in, out] Dest            - Destination pointer.
  @param[in]      Value           - The value to set.
  @param[in]      NumDwords       - The number of dwords to set.

  @retval Nothing.
**/
void
MrcOemMemorySetDword (
  IN OUT U32    *Dest,
  IN const U32  Value,
  IN U32        NumDwords
  )
{
  while (0 != NumDwords--) {
    *Dest++ = Value;
  }

  return;
}

/**
@brief
  Shift the specified data value left by the specified count.

  @param[in] Data            - 64 bit number to shift left.
  @param[in] Count           - Number of bits to shift (0..63)

  @retval The number of bits shifted left.
**/
U64
MrcOemMemoryLeftShiftU64 (
  IN const U64 Data,
  IN const U8  Count
  )
{
  return LShiftU64 (Data, Count);
}

/**
@brief
  Shift the specified data value Right by the specified count..

  @param[in] Data            - U64 number to shift
  @param[in] Count           - number of bits to shift (0..63)

  @retval Returns the shifted U64 value.
**/
U64
MrcOemMemoryRightShiftU64 (
  IN const U64 Data,
  IN const U8  Count
  )
{
  return RShiftU64 (Data, Count);
}

/**
@brief
  this function Multiply U64 with a U32 number. Result is <= 64 bits
  need to be port for OEM platform requirements.

  @param[in] Multiplicand  - U64 number to be multiplied with
  @param[in] Multiplier - U32 number to multiply

  @retval N/A
**/
U64
MrcOemMemoryMultiplyU64ByU32 (
  IN const U64 Multiplicand,
  IN const U32 Multiplier
  )
{
  return MultU64x32 (Multiplicand, Multiplier);
}

/**
@brief
  Divide U64 with a U64 number. Result is <= 32 bits

  @param[in] Dividend  - U64 number to be multiplied with
  @param[in] Divisor - U32 number to multiply

  @retval Returns the quotient result of U32 value.
**/
U64
MrcOemMemoryDivideU64ByU64 (
  IN const U64 Dividend,
  IN const U64 Divisor
  )
{
  return (DivU64x64Remainder (Dividend, Divisor, NULL));
}
