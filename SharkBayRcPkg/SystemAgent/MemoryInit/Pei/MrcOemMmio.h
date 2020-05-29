/** @file

  This file contains the memory mapped I/O manipulation definitions.
  
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

#ifndef _MrcOemMmio_h_
#define _MrcOemMmio_h_

#include <MrcTypes.h>

/**
@brief
  Read 64 bits from the Memory Mapped I/O space.

  @param[in]  Offset      - Offset from the specified base address.
  @param[out] Value       - Where to store the read value.
  @param[in]  BaseAddress - MMIO space base address.

  @retval Nothing.
**/
extern
void
MrcOemMmioRead64 (
  IN  U32 Offset,
  OUT U64 *Value,
  IN  U32 BaseAddress
  );

/**
@brief
  Read 32 bits from the Memory Mapped I/O space.

  @param[in]  Offset      - Offset from the specified base address.
  @param[out] Value       - Where to store the read value.
  @param[in]  BaseAddress - MMIO space base address.

  @retval Nothing.
**/
extern
void
MrcOemMmioRead (
  IN  U32 Offset,
  OUT U32 *Value,
  IN  U32 BaseAddress
  );

/**
@brief
  Read 16 bits from the Memory Mapped I/O space.

  @param[in]  Offset      - Offset from the specified base address.
  @param[out] Value       - Where to store the read value.
  @param[in]  BaseAddress - MMIO space base address.

  @retval Nothing.
**/
extern
void
MrcOemMmioRead16 (
  IN  U32 Offset,
  OUT U16 *Value,
  IN  U32 BaseAddress
  );

/**
@brief
  Read 8 bits from the Memory Mapped I/O space.

  @param[in]  Offset      - Offset from the specified base address.
  @param[out] Value       - Where to store the read value.
  @param[in]  BaseAddress - MMIO space base address.

  @retval Nothing.
**/
extern
void
MrcOemMmioRead8 (
  IN  U32 Offset,
  OUT U8  *Value,
  IN  U32 BaseAddress
  );

/**
@brief
  Write 64 bits to the Memory Mapped I/O space.

  @param[in] Offset      - Offset from the specified base address.
  @param[in] Value       - The value to write.
  @param[in] BaseAddress - MMIO space base address.

  @retval Nothing
**/
extern
void
MrcOemMmioWrite64 (
  IN U32 Offset,
  IN U64 Value,
  IN U32 BaseAddress
  );

/**
@brief
  Write 32 bits to the Memory Mapped I/O space.

  @param[in] Offset      - Offset from the specified base address.
  @param[in] Value       - The value to write.
  @param[in] BaseAddress - MMIO space base address.

  @retval Nothing.
**/
extern
void
MrcOemMmioWrite (
  IN U32 Offset,
  IN U32 Value,
  IN U32 BaseAddress
  );

/**
@brief
  Write 16 bits to the Memory Mapped I/O space.

  @param[in] Offset      - Offset from the specified base address.
  @param[in] Value       - The value to write.
  @param[in] BaseAddress - MMIO space base address.

  @retval Nothing.
**/
extern
void
MrcOemMmioWrite16 (
  IN U32 Offset,
  IN U16 Value,
  IN U32 BaseAddress
  );

/**
@brief
  Write 8 bits to the Memory Mapped I/O space.

  @param[in] Offset      - Offset from the specified base address.
  @param[in] Value       - The value to write.
  @param[in] BaseAddress - MMIO space base address.

  @retval Nothing.
**/
extern
void
MrcOemMmioWrite8 (
  IN U32 Offset,
  IN U8  Value,
  IN U32 BaseAddress
  );

extern
void
MmioReadCount (
  void
  );

/**
@brief
  This function count the number of access to writes MMIO registers.

  @param[in] Nothing

  @retval Nothing
**/
extern
void
MmioWriteCount (
  void
  );

#endif // _MrcOemMmio_h_
