/**
This file contains an 'Intel Peripheral Driver' and uniquely
identified as "Intel Reference Module" and is
licensed for Intel CPUs and chipsets under the terms of your
license agreement with Intel or your vendor.  This file may
be modified by the user, subject to additional terms of the
license agreement.

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MrcHswMcAddrDecode.h

@brief:
  File to support address decoding and encoding

**/
#ifndef _HSWULT_MC_ADDR_DECODE_H_
#define _HSWULT_MC_ADDR_DECODE_H_


#include "MrcOemAddrDecode.h"
#include "MrcOemMemory.h"

/**
@brief
  Address decode function
  Converts system address to DRAM address

  @param[in]      sys_addr - the 39-bit system address to convert
  @param[in, out] p_is_tcm - is the transaction to sys_addr "traffic class for the manageability engine"
  @param[in]      TOLUD - memory register
  @param[in]      REMAP_BASE - memory register
  @param[in]      REMAP_LIMIT - memory register
  @param[in]      CHANNEL_HASH - memory register
  @param[in]      MAD_ZR - memory register
  @param[in]      MAD_CHNL - memory register
  @param[in]      MAD_DIMM_ch0 - memory register
  @param[in]      MAD_DIMM_ch1 - memory register
  @param[in]      MAD_DIMM_ch2 - memory register
  @param[out]     p_chan - channel sys_addr decodes to
  @param[out]     p_dimm - DIMM sys_addr decodes to
  @param[out]     p_rank - rank sys_addr decodes to
  @param[out]     p_bank - bank sys_addr decodes to
  @param[out]     p_row - row sys_addr decodes to
  @param[out]     p_col - column sys_addr decodes to.

  @retval True if successful.

**/
BOOL
MrcHswDecode (
  IN     U64  sys_addr,
  IN OUT BOOL *p_is_tcm,
  IN     U32  TOLUD,
  IN     U64  REMAP_BASE,
  IN     U64  REMAP_LIMIT,
  IN     U32  CHANNEL_HASH,
  IN     U32  MAD_ZR,
  IN     U32  MAD_CHNL,
  IN     U32  MAD_DIMM_ch0,
  IN     U32  MAD_DIMM_ch1,
  IN     U32  MAD_DIMM_ch2,
  OUT    U16  *p_chan,
  OUT    U16  *p_dimm,
  OUT    U16  *p_rank,
  OUT    U16  *p_bank,
  OUT    U16  *p_row,
  OUT    U16  *p_col
  );

/**
@brief
  Address encode function (reverse address decode)
  DRAM address to system address conversion

  @param[in]     p_chan - channel sys_addr to encode
  @param[in]     p_dimm - DIMM sys_addr to encode
  @param[in]     p_rank - rank sys_addr to encode
  @param[in]     p_bank - bank sys_addr to encode
  @param[in]     p_row - row sys_addr to encode
  @param[in]     p_col - column sys_addr to encode. Note: The architecture is limited to
                         half-cache-line granularity for burst order. Therefore the last
                         two bits of the column are ignored.
  @param[in]      TOLUD - memory register
  @param[in]      REMAP_BASE - memory register
  @param[in]      REMAP_LIMIT - memory register
  @param[in]      CHANNEL_HASH - memory register
  @param[in]      MAD_ZR - memory register
  @param[in]      MAD_CHNL - memory register
  @param[in]      MAD_DIMM_ch0 - memory register
  @param[in]      MAD_DIMM_ch1 - memory register
  @param[in]      MAD_DIMM_ch2 - memory register
  @param[out]     sys_addr - the 39-bit system address convert to
  @param[in, out] p_is_tcm - is the transaction to sys_addr "traffic class for the manageability engine"

  @retval True if successful.

**/
BOOL
MrcHswEncode (
  IN  U16     chan,
  IN  U16     dimm,
  IN  U16     rank,
  IN  U16     bank,
  IN  U16     row,
  IN  U16     col,
  IN  U32     TOLUD,
  IN  U64     REMAP_BASE,
  IN  U64     REMAP_LIMIT,
  IN  U32     CHANNEL_HASH,
  IN  U32     MAD_ZR,
  IN  U32     MAD_CHNL,
  IN  U32     MAD_DIMM_ch0,
  IN  U32     MAD_DIMM_ch1,
  IN  U32     MAD_DIMM_ch2,
  OUT U64     *p_sys_addr,
  IN OUT BOOL *p_is_tcm
  );


#endif // _HSWULT_MC_ADDR_DECODE_H_
