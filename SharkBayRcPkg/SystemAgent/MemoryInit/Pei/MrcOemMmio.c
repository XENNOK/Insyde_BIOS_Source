/** @file
  This file contains the memory mapped I/O manipulation functions.

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
#include <MrcOemMmio.h>

/**
@brief
  Read 64 bits from the Memory Mapped I/O space.

  @param[in]  Offset      - Offset from the specified base address.
  @param[out] Value       - Where to store the read value.
  @param[in]  BaseAddress - MMIO space base address.

  @retval Nothing.
**/
void
MrcOemMmioRead64 (
  IN  U32 Offset,
  OUT U64 *Value,
  IN  U32 BaseAddress
  )
{
  U64 MmxSave;
  U64 *MmioOffset;

  MmioOffset = (U64 *) (Offset + BaseAddress);

#if defined __GNUC__  // GCC compiler
  __asm__ __volatile__ (
    "\n\t movq %%mm0, %0"
    "\n\t movq %2, %%mm0"
    "\n\t movq %%mm0, %1"
    "\n\t movq %3, %%mm0"
    "\n\t emms"
    : "=m" (MmxSave),
      "=m" (Value[0])
    : "m" (MmioOffset[0]),
      "m" (MmxSave)
    );
#else // MSFT compiler
  ASM {

    ; Save mm0
    movq  MmxSave, mm0

    mov   edi, MmioOffset

    movq  mm0, QWORD PTR DS:[edi]

    mov   edi, Value
    movq  QWORD PTR DS:[edi], mm0

    ; Restore mm0
    movq  mm0, MmxSave
    emms                     ; Exit mmx Instruction
  }
#endif

  MmioReadCount ();
  return;
}

/**
@brief
  Read 32 bits from the Memory Mapped I/O space.

  @param[in]  Offset      - Offset from the specified base address.
  @param[out] Value       - Where to store the read value.
  @param[in]  BaseAddress - MMIO space base address.

  @retval Nothing.
**/
void
MrcOemMmioRead (
  IN  U32 Offset,
  OUT U32 *Value,
  IN  U32 BaseAddress
  )
{
  *Value = (*((volatile U32 *) (Offset + BaseAddress)));
  MmioReadCount ();
  return;
}

/**
@brief
  Read 16 bits from the Memory Mapped I/O space.

  @param[in]  Offset      - Offset from the specified base address.
  @param[out] Value       - Where to store the read value.
  @param[in]  BaseAddress - MMIO space base address.

  @retval Nothing.
**/
void
MrcOemMmioRead16 (
  IN  U32 Offset,
  OUT U16 *Value,
  IN  U32 BaseAddress
  )
{
  *Value = (*((volatile U16 *) (Offset + BaseAddress)));
  MmioReadCount ();
  return;
}

/**
@brief
  Read 8 bits from the Memory Mapped I/O space.

  @param[in]  Offset      - Offset from the specified base address.
  @param[out] Value       - Where to store the read value.
  @param[in]  BaseAddress - MMIO space base address.

  @retval Nothing.
**/
void
MrcOemMmioRead8 (
  IN  U32 Offset,
  OUT U8  *Value,
  IN  U32 BaseAddress
  )
{
  *Value = (*((volatile U8 *) (Offset + BaseAddress)));
  MmioReadCount ();
  return;
}

/**
@brief
  Write 64 bits to the Memory Mapped I/O space.

  @param[in] Offset      - Offset from the specified base address.
  @param[in] Value       - The value to write.
  @param[in] BaseAddress - MMIO space base address.

  @retval Nothing.
**/
void
MrcOemMmioWrite64 (
  IN U32 Offset,
  IN U64 Value,
  IN U32 BaseAddress
  )
{
  U64 MmxSave;
  U64 *MmioOffset;

  MmioOffset = (U64 *) (Offset + BaseAddress);

#if defined __GNUC__  // GCC compiler
  __asm__ __volatile__ (
    "\n\t movq %%mm0, %0"
    "\n\t movq %2, %%mm0"
    "\n\t movq %%mm0, %1"
    "\n\t movq %3, %%mm0"
    "\n\t emms"
    : "=m" (MmxSave)
    : "m" (MmioOffset[0]),
      "m" (Value),
      "m" (MmxSave)
    );
#else //MSFT compiler
  ASM {

    ; Save mm0
    movq  MmxSave, mm0

    mov   edi, MmioOffset
    movq  mm0, Value

    movq  QWORD PTR DS:[edi], mm0

    ; Restore mm0
    movq  mm0, MmxSave
    emms                     ; Exit mmx Instruction

  }
#endif

  MmioWriteCount ();
  return;
}

/**
@brief
  Write 32 bits to the Memory Mapped I/O space.

  @param[in] Offset      - Offset from the specified base address.
  @param[in] Value       - The value to write.
  @param[in] BaseAddress - MMIO space base address.

  @retval Nothing.
**/
void
MrcOemMmioWrite (
  IN U32 Offset,
  IN U32 Value,
  IN U32 BaseAddress
  )
{
  (*((volatile U32 *) (Offset + BaseAddress))) = Value;
  MmioWriteCount ();
  return;
}

/**
@brief
  Write 16 bits to the Memory Mapped I/O space.

  @param[in] Offset      - Offset from the specified base address.
  @param[in] Value       - The value to write.
  @param[in] BaseAddress - MMIO space base address.

  @retval Nothing.
**/
void
MrcOemMmioWrite16 (
  IN U32 Offset,
  IN U16 Value,
  IN U32 BaseAddress
  )
{
  (*((volatile U16 *) (Offset + BaseAddress))) = Value;
  MmioWriteCount ();
  return;
}

/**
@brief
  Write 8 bits to the Memory Mapped I/O space.

  @param[in] Offset      - Offset from the specified base address.
  @param[in] Value       - The value to write.
  @param[in] BaseAddress - MMIO space base address.

  @retval Nothing.
**/
void
MrcOemMmioWrite8 (
  IN U32 Offset,
  IN U8  Value,
  IN U32 BaseAddress
  )
{
  (*((volatile U8 *) (Offset + BaseAddress))) = Value;
  MmioWriteCount ();
  return;
}
#ifndef MRC_MINIBIOS_BUILD
/*++

@brief
  This function count the number of access to writes MMIO registers.

  @param[in] Nothing.

  @retval Nothing.
**/
void
MmioWriteCount (
  void
  )
{
  return;
}

/**
@brief
  This function count the number of access to reads MMIO registers.

  @param[in] Nothing.

  @retval Nothing.
**/
void
MmioReadCount (
  void
  )
{
  return;
}
#endif // MRC_MINIBIOS_BUILD
