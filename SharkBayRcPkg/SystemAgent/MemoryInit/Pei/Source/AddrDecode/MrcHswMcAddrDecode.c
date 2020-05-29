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
  MrcHswMcAddrDecode.c

@brief:
  File to support address decoding and encoding

**/
/*

This file defines functions that perform address decoding, reverse address
decoding, configuration checking, and configuration printing:

    hswult_mc_addr_decode_config_check() - Checks the values in the registers used
        for DRAM address decoding for illegal or inconsistent programming.
        It would be wise to call this function once before using the other
        functions.  Once the configuration/register-programming passes the check,
        the decode and/or encode functions may be called multiple times.

    hswult_mc_addr_decode() - Decodes system addresses into DRAM addresses and is
        equivalent to the address decoding performed inside the memory
        controller.

    hswult_mc_addr_encode() - Performs the reverse of hswult_mc_addr_decode().
        Translates a DRAM address into a system address.

    hswult_mc_addr_decode_config_info() - Prints to a sring information about the
        configuration that can be determined from the registers involved in
        address decoding.

"System address" refers to the 39-bit address presented to the memory controller
at the memory controller's interface to the IMPH.

"DRAM address" refers to the physical memory location inside the DDR3 memory
hierarchy:
-Channel
    |
    +-DIMM
        |
        +-Rank
            |
            +-Bank
                |
                +-Row
                    |
                    +-Column

This code was authored with the intention that it comply with the C99 spec.
This file should be compiled with the -std=c99 command line option if GCC is
used to compile.
The checking, decoding, and encoding functions return the bool "false" if they
fail and return the bool "true" if they succeed.  They also print an error
message explaining the failure to a string for which memory must be allocated
before calling the function.

A reminder about the bit-widths of data types guaranteed by the C99 spec:
    - unsigned long long - must be at least 64-bits
    - unsigned long      - must be at least 32-bits
    - unsigned           - must be at least 16-bits, but is often 32.
                           The size will be the "natural" size for the platform
                           architecture.

A reminder about literals:
    literals are sometimes assumed to be the "natural" size for the platform
    architecture:

    unsigned long long x = (0x1 << 63);

    result is x = 0, not 0x8000000000000000.

    Solution:

    unsigned long long x = (((unsigned long long) 0x1) << 63);

    Also note that literals that are not the "natural size" must be typed
    with trailing letters. For example, 0x8000000000000000 must be specified as
    0x8000000000000000ULL (note the "ULL" at the end for "unsigned long long").

*/



#include "MrcHswMcAddrDecode.h"

// size of the hash mask field in the channel hash register
#define HSW_MC_ADDR_CHANNEL_HASH_MASK_SIZE 14

//
// Bit-Masks used to remove or test register fields
//
#define HSW_MC_ADDR_TOLUD_MASK             0xFFF00000
#define HSW_MC_ADDR_REMAP_MASK             0x0000007FFFF00000ULL
#define HSW_MC_ADDR_CHAN_HASH_ENABLE_MASK  0x00800000

// Mask used to add 1's to lower bits of REMAP_LIMIT register
#define HSW_MC_ADDR_REMAP_LIMIT_LOWER_BITS_MASK 0x00000000000FFFFFULL

// Useful number constants
#define HSW_MC_256MB_AS_CL  (1 << 22)
#define HSW_MC_512MB_AS_CL  (1 << 23)
#define HSW_MC_1GB_AS_CL    (1 << 24)
#define HSW_MC_2GB_AS_CL    (1 << 25)
#define HSW_MC_4GB_AS_CL    (1 << 26)
#define HSW_MC_8GB_AS_CL    (1 << 27)


// global variable to control debug messages printed to standard out.
BOOL g_hswult_mc_addr_debug_messages = FALSE;

//
// Functions to extract fields from the registers
//

static inline U64
get_onec_as_cl (
  U32 MAD_ZR
  )
{
  return MrcOemMemoryLeftShiftU64 ((((U64) MAD_ZR) & 0x000000FFULL), 22); // [ 7: 0]=8-bits
}

static inline U64
get_threec_as_cl (
  U32 MAD_ZR
  )
{
  return MrcOemMemoryLeftShiftU64 ((((U64) MAD_ZR) & 0x0000FF00ULL), 14); // [15: 8]=8-bits
}

static inline U64
get_twobandc_as_cl (
  U32 MAD_ZR
  )
{
  return MrcOemMemoryLeftShiftU64 ((((U64) MAD_ZR) & 0x00FF0000ULL), 6); // [23:16]=8-bits
}

static inline U64
get_bandc_as_cl (
  U32 MAD_ZR
  )
{
  return MrcOemMemoryRightShiftU64 ((((U64) MAD_ZR) & 0xFF000000ULL), 2); // [31:24]=8-bits
}

static inline U64
get_dimm_a_size_as_cl (
  U32 MAD_DIMM
  )
{
  return MrcOemMemoryLeftShiftU64 ((((U64) MAD_DIMM) & 0x00FFULL), 22); // MAD_DIMM[ 7:0]*256MB >>6
}

static inline U64
get_dimm_b_size_as_cl (
  U32 MAD_DIMM
  )
{
  return MrcOemMemoryLeftShiftU64 ((((U64) MAD_DIMM) & 0xFF00ULL), 14); // MAD_DIMM[15:8]*256MB >>6
}

static inline BOOL
get_dimm_a_number_of_ranks (
  U32 MAD_DIMM
  )
{
  return (BOOL) ((MAD_DIMM >> 17) & 1); // TRUE = 2-ranks, FALSE = 1-rank
}

static inline BOOL
get_dimm_b_number_of_ranks (
  U32 MAD_DIMM
  )
{
  return (BOOL) ((MAD_DIMM >> 18) & 1); // TRUE = 2-ranks, FALSE = 1-rank
}

static inline U64
get_dimm_a_rank_size_as_cl (
  U32 MAD_DIMM
  )
{
  return MrcOemMemoryRightShiftU64 (get_dimm_a_size_as_cl(MAD_DIMM), ((U8) get_dimm_a_number_of_ranks(MAD_DIMM)));
}

static inline U64
get_dimm_b_rank_size_as_cl (
  U32 MAD_DIMM
  )
{
  return MrcOemMemoryRightShiftU64 (get_dimm_b_size_as_cl(MAD_DIMM), ((U8) get_dimm_b_number_of_ranks(MAD_DIMM)));
}

static inline BOOL
get_dimm_and_rank_intlv (
  U32 MAD_DIMM
  )
{
  return (BOOL) ((MAD_DIMM >> 21) & 1); // MAD_DIMM[21:21]
}

static inline BOOL
get_high_order_intlv_mode (
  U32 MAD_DIMM
  )
{
  return (BOOL) ((MAD_DIMM >> 26) & 1); // MAD_DIMM[26:26]
}

static inline U16
get_hori_addr (
  U32 MAD_DIMM
  )
{
  return (U16) ((MAD_DIMM >> 27) & 7); // MAD_DIMM[29:27]
}

static inline BOOL
get_enhanced_intlv_mode (
  U32 MAD_DIMM
  )
{
  return (BOOL) ((MAD_DIMM >> 22) & 1); // MAD_DIMM[22:22]
}

static inline U16
get_dimm_a_select (
  U32 MAD_DIMM
  )
{
  return (U16) ((MAD_DIMM >> 16) & 1);
}

static inline BOOL
get_lpddr_mode (
  U32 MAD_CHNL
  )
{
  return (BOOL) (MAD_CHNL >> 10) & 1;
}

static inline U16
get_dimm_a_width (
  U32 MAD_CHNL,
  U32 MAD_DIMM
  )
{
  BOOL is_x16 = (BOOL) ((MAD_DIMM >> 19) & 1);
  return is_x16 ? 16 : (get_lpddr_mode(MAD_CHNL) ? 32 : 8);
}

static inline U16
get_dimm_b_width (
  U32 MAD_CHNL,
  U32 MAD_DIMM
  )
{
  BOOL is_x16 = (BOOL) ((MAD_DIMM >> 20) & 1);
  return is_x16 ? 16 : (get_lpddr_mode(MAD_CHNL) ? 32 : 8);
}

static inline U16
get_dimm_a_num_col_bits (
  U32 MAD_CHNL,
  U32 MAD_DIMM
  )
{
  U16 width;
  U64 dimm_rank_size_as_cl;

  if (!get_lpddr_mode(MAD_CHNL)) {
    return 10; // Supported DDR3 DRAM organizations all have 10 column bits
  }

  // If we got past the above line, we are LPDDR

  width                = get_dimm_a_width(MAD_CHNL, MAD_DIMM);
  dimm_rank_size_as_cl = get_dimm_a_rank_size_as_cl(MAD_DIMM);
  if (width == 16) {
    return (dimm_rank_size_as_cl == HSW_MC_1GB_AS_CL) ? 10 : 11; // LPDDR x16 2Gb device has 10 col bits,
                                                                 // the 4 and 8 Gb LPDDR x16s have 11 col bits.
  }
  // width == 32
  return (dimm_rank_size_as_cl == HSW_MC_512MB_AS_CL) ? 9 : 10; // LPDDR x32 2Gb device has 9 col bits,
                                                                // the 4 and 8 Gb LPDDR x32s have 10 col bits.
}

static inline U16
get_dimm_b_num_col_bits (
  U32 MAD_CHNL,
  U32 MAD_DIMM
  )
{
  U16 width;
  U64 dimm_rank_size_as_cl;

  if (!get_lpddr_mode(MAD_CHNL)) {
    return 10; // Supported DDR3 DRAM organizations all have 10 column bits
  }

  // If we got past the above line, we are LPDDR

  width                = get_dimm_b_width(MAD_CHNL, MAD_DIMM);
  dimm_rank_size_as_cl = get_dimm_b_rank_size_as_cl(MAD_DIMM);
  if (width == 16) {
    return (dimm_rank_size_as_cl == HSW_MC_1GB_AS_CL) ? 10 : 11; // LPDDR x16 2Gb device has 10 col bits,
                                                                 // the 4 and 8 Gb LPDDR x16s have 11 col bits.
  }
  // width == 32
  return (dimm_rank_size_as_cl == HSW_MC_512MB_AS_CL) ? 9 : 10; // LPDDR x32 2Gb device has 9 col bits,
                                                                // the 4 and 8 Gb LPDDR x32s have 10 col bits.
}

static inline U16
get_ch_hash_lsb_mask_bit (
  U32 CHANNEL_HASH
  )
{
  return (CHANNEL_HASH >> 21) & 3;
}

static inline U16
get_ch_hash_mask (
  U32 CHANNEL_HASH
  )
{
  return CHANNEL_HASH & 0x03FFF;
}

static inline BOOL
get_stacked_mode (
  U32 MAD_CHNL
  )
{
  return (BOOL) ((MAD_CHNL >> 6) & 1);
}

static inline U16
get_stacked_encoding (
  U32 MAD_CHNL
  )
{
  return (U16) (MAD_CHNL >> 7) & 7;
}

//
// Functions to aid in common tasks
//

// convert a cache-line address to a system address
static inline U64
cl_to_sys (
  U64 cache_line
  )
{
  return MrcOemMemoryLeftShiftU64 (cache_line, 6);
}

// Channel conversion functions. For logical channels: 0=A, 1=B.
static inline U16
logical_to_physical_chan (
  U32 MAD_CHNL,
  U16 logical_chan
  )
{
  return ((U16) ((MAD_CHNL >> (logical_chan << 1)) & 0x3));
}

static inline U16
physical_to_logical_chan (
  U32 MAD_CHNL,
  U16 physical_chan
  )
{
  return ((((U16) (MAD_CHNL & 3)) == physical_chan) ? 0 : 1); // 0=A, 1=B
}

// Function for decode of stacked channel debug feature
static inline U64
get_stacked_memory_size (
  U16 stacked_encoding
  )
{
  return MrcOemMemoryLeftShiftU64 (0x00400000ULL, (U8) stacked_encoding); // 1 << 28 + stacked_encoding - 6 for the cachline align
}


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
  )
{
  // used to hold the address after the remap zone has been applied to the system address
  U64  remap_addr; // full address
  U64  remap_line; // cache-line address
  // used to hold fields from MAD_ZR, adjusted to cache-line address
  U64  onec;     // 1 * C
  U64  threec;   // 3 * C       - top of Zone 0
  U64  twobandc; // (2 * B) + C - top of Zone 1
  U64  bandc;    // (B + C)
  // Make a MAD_DIMM array for easy access
  U32  MAD_DIMM[3];
  U32  chan_a_mad_dimm; // channel A's MAD_DIMM register
  U64  gap_limit; // MMIO Gap Limit
  U64  tom; // Top Of Memory (cache-line address)
  U16  lsb_mask_bit; // LsbMaskBit field from CHANNEL_HASH register
  U16  hash_mask; // channel hash mask from CHANNEL_HASH register
  U16  hash_line; // lower bits of remap_line with hash_mask applied
  U16  i; // loop counter
  U64  chan_line; // channel address space (cache-line)
  U16  chan_select; // 0 = Channel A, 1 = Channel B
  U32  selected_mad_dimm; // MAD_DIMM for the selected channel
  BOOL is_lpddr = get_lpddr_mode(MAD_CHNL); // LPDDR or DDR3
  U16  num_col_bits; // The number of column address bits the DIMM has.
  U64  dimm_a_size; // sizes of the DIMMs on the channel in cache-lines
  U64  dimm_b_size;
  BOOL dimm_and_rank_interleaving; // modes for the selected channel
  BOOL high_order_rank_interleave;
  BOOL enhanced_interleave_mode;
  U16  hori_addr; // bits to use from address for HORI
  U64  dimm_line; // DIMM address space (cache-line)
  U16  dimm_select; // 0 = DIMM A, 1 = DIMM B
  U32  dimm_size; // size of selected DIMM (cache-lines)
  BOOL dual_rank; // number of ranks on selected DIMM
  // channel stacking variables
  BOOL stacked_mode;
  U64  stacked_size;
  // temporary values for bit masking and shifting
  U64  row_mask;
  U16  rank_bit_shift;

  MAD_DIMM[0] = MAD_DIMM_ch0;
  MAD_DIMM[1] = MAD_DIMM_ch1;
  MAD_DIMM[2] = MAD_DIMM_ch2;

  // zero out unused register bits
  TOLUD       &= HSW_MC_ADDR_TOLUD_MASK;
  REMAP_BASE  &= HSW_MC_ADDR_REMAP_MASK;
  REMAP_LIMIT &= HSW_MC_ADDR_REMAP_MASK;

  // Assume lower bits of REMAP_LIMIT are all 1's.
  REMAP_LIMIT |= HSW_MC_ADDR_REMAP_LIMIT_LOWER_BITS_MASK;

  //
  // Register field values are in 256MB granularity.
  // They are stored here adjusted to cache-line granularity.
  //
  onec     = get_onec_as_cl (MAD_ZR);
  threec   = get_threec_as_cl (MAD_ZR);
  twobandc = get_twobandc_as_cl (MAD_ZR);
  bandc    = get_bandc_as_cl (MAD_ZR);

  //
  // TOM is not directly available in a register. It will be computed by
  // finding the channel index of channel A, then adding the two DIMM
  // capacities of that channel together. Technically, this is only needed to
  // check that the system address is not beyond the amount of memory
  // available.
  //

  // Overflow check MAD_DIMM
  if ((MAD_CHNL & 0x3) > 2) {
    return FALSE;
  }
  chan_a_mad_dimm = MAD_DIMM[MAD_CHNL & 0x3];
  tom  = get_dimm_a_size_as_cl(chan_a_mad_dimm);
  tom += get_dimm_b_size_as_cl(chan_a_mad_dimm);
  tom += bandc;

  // remap the address if it is not a TCM transaction and falls inside the remap range
  remap_addr = sys_addr;
  if (REMAP_LIMIT > REMAP_BASE) { // check for remap region being enabled
    gap_limit = ((U64) TOLUD) + REMAP_LIMIT - REMAP_BASE;
    //
    // check for address falling in remap region
    //
    if ((sys_addr >= REMAP_BASE) && (sys_addr <= REMAP_LIMIT)) { // REMAP_LIMIT is now inclusive
      if (*p_is_tcm) {
        if (sys_addr >= cl_to_sys(tom)) {
          // transaction to sys_addr should not have been TCM.
          *p_is_tcm = FALSE;
          return FALSE;
        }
      } else {
        //
        // The address hit the remap region, so remap the address from the remap
        // source region (REMAP_BASE to REMAP_LIMIT) to the remap target region
        // (TOLUD to size-of-gap).
        //
        remap_addr -= REMAP_BASE;
        remap_addr += ((U64) TOLUD);
      }
    } else if ((sys_addr >= ((U64) TOLUD)) &&
                 (sys_addr <= gap_limit                   )) { // check for address falling in MMIO gap created by remap region
      // transaction to sys_addr should not have been TCM.
      *p_is_tcm = FALSE;
      return FALSE;
    } else {
      //
      // transaction to sys_addr should not have been TCM, but the memory
      // controller will process the request anyway without any problems.
      //
      *p_is_tcm = FALSE;
    }
  } else {
    // transaction to sys_addr should not have been TCM, but the memory
    // controller will process the request anyway without any problems.
    //
    *p_is_tcm = FALSE;
  }


  // from now on we will work on cache-line addresses
  remap_line = MrcOemMemoryRightShiftU64 (remap_addr, 6); // shift off intra-cache-line bits


  stacked_mode = get_stacked_mode (MAD_CHNL);

  if (stacked_mode) {
    //
    // In stacked mode, check that remapped address is below TOM.
    //
    if (remap_line >= tom) {
      return FALSE;
    }

    stacked_size = get_stacked_memory_size (get_stacked_encoding(MAD_CHNL));

    chan_select = (remap_line < stacked_size) ? 0 : 1;

    chan_line = remap_line;


    chan_select = (remap_line < stacked_size) ? 0 : 1;
    chan_line = remap_line;

    // If this is channel 1 in stacked mode, then we need to subtract out the channel size (clear
    // the stacked mode bit)
    //
    if (chan_select == 1) {
      chan_line = chan_line - stacked_size;
    }
  } else if (remap_line < threec) { // Zone 0
    return FALSE;
  } else if (remap_line < twobandc) { // Zone 1
    // Determine if the channel hash feature is being used
    if (CHANNEL_HASH & HSW_MC_ADDR_CHAN_HASH_ENABLE_MASK) { // test enable bit
      lsb_mask_bit = get_ch_hash_lsb_mask_bit (CHANNEL_HASH);
      hash_mask = get_ch_hash_mask (CHANNEL_HASH);
      hash_mask = hash_mask | (1 << lsb_mask_bit); // force the selected lsb_mask_bit to be on

      hash_line = ((U16) remap_line) & hash_mask; // get the bits to XOR for the hash
      //
      // Produce chan_select by XORing together all of the bits of hash_line.
      //
      // I don't know of a single instruction to do this, so an unrollable
      // loop will be used.
      //
      chan_select = 0;
      for (i = 0 ; i < HSW_MC_ADDR_CHANNEL_HASH_MASK_SIZE ; i++) {
        chan_select = chan_select ^ (hash_line >> i);
      }
      chan_select = chan_select & 1;
      //
      // sys_addr 6 will be shifted off to produce the channel address, so it must
      // be preserved if it wasn't used in the hash.  This is done by moving it to
      // the position indicated by lsb_mask_bit.
      //
      remap_line = remap_line & (~MrcOemMemoryLeftShiftU64 (0x0000000000000001ULL, (U8) lsb_mask_bit)); // zero out lsb_mask_bit
      remap_line = remap_line | MrcOemMemoryLeftShiftU64 ((remap_line & 1), (U8) lsb_mask_bit); // OR in bit 6 to lsb_mask_bit position
    } else {
      chan_select = (U16) (remap_line & 1); // remap_addr[6:6]
    }
    chan_line = MrcOemMemoryRightShiftU64 ((remap_line - onec), 1); // right shift by 1 divides by 2
  } else if (remap_line < tom) { // Zone 2
    chan_select = 0; // Channel A
    chan_line = remap_line - bandc;
  } else { // address was above memory capacity
    return FALSE;
  }

  // obtain the physical channel index
  *p_chan = logical_to_physical_chan (MAD_CHNL, chan_select);

  // Overflow check *p_chan
  if (*p_chan > 2) {
    return FALSE;
  }

  // get the register for the channel we're using
  selected_mad_dimm = MAD_DIMM[*p_chan];

  // Find the DIMM sizes on our selected channel. adjust to cache-line granularity
  dimm_a_size = get_dimm_a_size_as_cl (selected_mad_dimm);
  dimm_b_size = get_dimm_b_size_as_cl (selected_mad_dimm);

  // determine if we are doing DIMM and Rank interleaving
  dimm_and_rank_interleaving = get_dimm_and_rank_intlv (selected_mad_dimm);

  // determine if we are doing high order rank interleave
  high_order_rank_interleave = get_high_order_intlv_mode (selected_mad_dimm);

  // determine if we are doing Enhanced Interleave Mode (EIM) (XOR rank & bank bits)
  enhanced_interleave_mode   = get_enhanced_intlv_mode (selected_mad_dimm);

  // DIMM address calculation

  // DIMMs are interleaved for both dimm_and_rank_interleaving and high_order_rank_interleave modes.
  if (dimm_and_rank_interleaving || high_order_rank_interleave) {
    if (chan_line < MrcOemMemoryLeftShiftU64 (dimm_b_size, 1)) { // Range 0 limit = 2 * dimm_b_size
      // 2-way DIMM interleave. Channel address [15:15] is used to select DIMM
      dimm_select = (U16) (MrcOemMemoryRightShiftU64 (chan_line, 9) & 1);

      // DIMM address is channel address with the interleave bit (15) removed
      dimm_line = (MrcOemMemoryRightShiftU64 (chan_line, 1) & (~((U64) 0x01FFULL))) |
                  (chan_line & ((U64) 0x01FFULL));
    } else if (chan_line < (dimm_a_size + dimm_b_size)) { // Range 1 limit
      // No DIMM interleave. DIMM is the largest DIMM: DIMM A.
      dimm_select = 0;

      // DIMM address is channel address with DIMM B's contribution removed
      dimm_line = chan_line - dimm_b_size;
    } else {
      return FALSE;
    }
  } else { // no DIMM and Rank interleaving
    dimm_line = chan_line;
    if (chan_line < dimm_a_size) { // Range 0 limit = dimm_a_size
      // No DIMM interleave. DIMM is the largest DIMM: DIMM A.
      dimm_select = 0;

      // DIMM address is channel address
    } else if (chan_line < (dimm_a_size + dimm_b_size)) { // Range 1 limit
      // No DIMM interleave. DIMM is the smallest DIMM: DIMM B.
      dimm_select = 1;

      // DIMM address is channel address with dimm_a_size removed.
      dimm_line -= dimm_a_size;
    } else {
      return FALSE;
    }
  }

  // get the physical DIMM index
  *p_dimm = dimm_select ^ get_dimm_a_select (selected_mad_dimm);

  // get DIMM info
  dimm_size = (U32) (dimm_select ? dimm_b_size : dimm_a_size);
  dual_rank =  dimm_select ? get_dimm_b_number_of_ranks (selected_mad_dimm):
                             get_dimm_a_number_of_ranks (selected_mad_dimm);

  num_col_bits = dimm_select ? get_dimm_b_num_col_bits (MAD_CHNL, selected_mad_dimm) :
                               get_dimm_a_num_col_bits (MAD_CHNL, selected_mad_dimm);


  // DRAM address calculation

  //
  // Grab the column first (because with HSW-ULT we will shift dimm_line up or
  // down by 1 based on column size).
  //
  // column is DimmAddress[12:3] when 10 column bits are present. [13:3] and
  // [11:3] for 11 and 9 column bits, respectively.
  //
  *p_col = (U16) MrcOemMemoryLeftShiftU64 (dimm_line, 3);

  // The low-order intra-cache-line bits must be added back in.
  *p_col = *p_col | ((U16) (MrcOemMemoryRightShiftU64 (sys_addr, 3) & 0x7ULL));

  // We picked up extra high-order bits from dimm_line.
  // Mask off the bits above the column range.
  //
  *p_col = *p_col & ((1 << num_col_bits) - 1);

  // Now compute Rank, Bank, and Row

  // The column address bits make up the bottom of the DIMM address space.
  // With the addition of LPDDR to HSW-ULT, the number column address bits may
  // change from the standard 10 with DDR3 to 9 or 11 with some of the LPDDR
  // organizations.  The entire DIMM address space can be shifted up or down
  // with this change, then the bank, rank, and row bits can be extracted as
  // with the standard 10 column bits.
  //
  if (num_col_bits == 9) {
    dimm_line = MrcOemMemoryLeftShiftU64 (dimm_line, 1); // Shift up as though there were 10.
  }

  if (num_col_bits == 11) {
    dimm_line = MrcOemMemoryRightShiftU64 (dimm_line, 1); // Shift down as though there were 10.
  }

  // high_order_rank_interleave is mutually exclusive with dimm_and_rank_interleaving
  if (dual_rank && high_order_rank_interleave) {
    //
    // Specify which address bit 20-27 to use as the rank interleave bit
    //  000 = bit 20, 001 = bit 21, ..., 111 = bit 27
    //
    hori_addr = get_hori_addr (selected_mad_dimm);

    // Rank is selected by the HORI address field, which chooses a bit from DimmAddress[27:20]
    *p_rank = (U16) (MrcOemMemoryRightShiftU64 (dimm_line, (U8) (hori_addr + 20 - 6)) & 1);

    // Bank in HORI mode is just like no-rank-interleave

    // bank = DimmAddress[15:13]
    *p_bank = (U16) (MrcOemMemoryRightShiftU64 (dimm_line, 7) & 7);

    if (enhanced_interleave_mode) {
      // bank = DimmAddress[15:13] ^ DimmAddress[18:16]
      *p_bank = *p_bank ^ ((U16) (MrcOemMemoryRightShiftU64 (dimm_line, 10) & 7));
    }

    // row[15:11] is always DimmAddress[32:28]
    // row[3:0] is always DimmAddress[19:16]
    // row[11:4] must make room for the rank bit, wherever hori_addr puts it.

    // Get all row bits plus the rank bit somewhere in there.
    *p_row = (U16) MrcOemMemoryRightShiftU64 (dimm_line, 10);

    // Create a mask with 1's in the position of the row bits below the rank bit
    row_mask = (1 << (hori_addr + 4)) - 1;

    // Shift down the upper bits by one to remove the rank bit and recombine with the lower bits
    *p_row = ((*p_row >> 1) & ((U16) (~row_mask))) | (*p_row & ((U16) row_mask));

    // Mask away any row bits too large for the size of DIMM (only the number of row bits changes with DIMM size).
    *p_row = *p_row & ((U16) ((dimm_size >> 10) - 1));
  } else if (dual_rank && dimm_and_rank_interleaving) {
    if (enhanced_interleave_mode) {
      //
      // rank = DimmAddress[15:15] XOR DimmAddress[19:19]
      // bank = {DimmAddress[16:16],DimmAddress[14:13]} XOR
      //        {DimmAddress[20:20],DimmAddress[18:17]}
      //
      // We can just modify the bank rank bits in the dimm_line address.
      // The rest of the bits will not be affected, neither will further
      // operations involving dimm_line.
      //
      dimm_line = dimm_line ^ (MrcOemMemoryRightShiftU64 (dimm_line, 4) & 0x780ULL);
    }

    // rank = DimmAddress[15:15]
    *p_rank = (U16) (MrcOemMemoryRightShiftU64 (dimm_line, 9) & 1);

    // bank = {DimmAddress[16:16],DimmAddress[14:13]}
    *p_bank = (U16) ((MrcOemMemoryRightShiftU64 (dimm_line, 8) & 4) | (MrcOemMemoryRightShiftU64 (dimm_line, 7) & 3));

    // row = DimmAddress[32..28:17] depending on DIMM size.
    *p_row = (U16) MrcOemMemoryRightShiftU64 (dimm_line, 11);

    // Mask away any row bits too large for the size of DIMM (only the number of row bits changes with DIMM size).
    *p_row = *p_row & ((U16) ((dimm_size >> 10) - 1));
  } else { // single rank or no rank interleaving
    // for single-rank DIMM, bits above row bits should all be zero
    row_mask = 0xFFFFFFFFFFFFFC00ULL;

    // rank = one of DimmAddress[32..28] depending on DIMM size, or 0 for single rank.
    *p_rank = 0;
    if (dual_rank) {
      //
      // When using 11 column bits the HSW ULT supports only 4Gb/8Gb x16 LPDDR devices.
      // Meaning rank size can be 2GB/4GB, hence since we are in dual rank DIMM, DIMM size is 4GB/8GB.
      // LPDDR only supports 14/15 row address bits, for 2GB/4GB ranks respectively.
      // But, DDR3 calculation (always 10 col bits) 2GB/4GB ranks (4GB/8GB DIMMs) uses 15/16 row address bits respectively.
      // So, we change dimm_size as if we calculate DDR3 to avoid getting 16 row bits and shifted rank position.
      //
      if (is_lpddr && (num_col_bits == 11)) {
        switch( dimm_size ) { // remember: dimm_size is in cache-lines
          case HSW_MC_4GB_AS_CL:
            dimm_size = HSW_MC_2GB_AS_CL;
            break;
          case HSW_MC_8GB_AS_CL:
            dimm_size = HSW_MC_4GB_AS_CL;
            break;
          default:
            return FALSE;
        }
      }
      //
      // When using 9column bits the HSW ULT supports only 2Gb x32 LPDDR devices.
      // Meaning rank size can be 512MGB, hence since we are in dual rank DIMM, DIMM size is 1GB.
      // LPDDR only supports 14 row address bits, for 512MB ranks.
      // But, DDR3 calculation (always 10 col bits) 512MB ranks (1GB DIMMs) uses 13 row address bits.
      // So, we change dimm_size as if we calculate DDR3 to avoid getting 13 row bits and shifted rank position.
      //
      if (is_lpddr && (num_col_bits == 9)) {
        switch( dimm_size ) { // remember: dimm_size is in cache-lines
          case HSW_MC_1GB_AS_CL:
            dimm_size = HSW_MC_2GB_AS_CL;
            break;
          default:
            return FALSE;
        }
      }

      switch( dimm_size ) // remember: dimm_size is in cache-lines
      {
          case HSW_MC_256MB_AS_CL:
            return FALSE;
          case HSW_MC_512MB_AS_CL:
            rank_bit_shift = 22;
            row_mask = 0x003FFC00ULL;
            break;
          case HSW_MC_1GB_AS_CL:
            rank_bit_shift = 23;
            row_mask = 0x007FFC00ULL;
            break;
          case HSW_MC_2GB_AS_CL:
            rank_bit_shift = 24;
            row_mask = 0x00FFFC00ULL;
            break;
          case HSW_MC_4GB_AS_CL:
            rank_bit_shift = 25;
            row_mask = 0x01FFFC00ULL;
            break;
          case HSW_MC_8GB_AS_CL:
            rank_bit_shift = 26;
            row_mask = 0x03FFFC00ULL;
            break;
          default:
            return FALSE;
      }
      *p_rank = (U16) MrcOemMemoryRightShiftU64 (dimm_line, (U8) rank_bit_shift) & 1;
    }

    // bank = DimmAddress[15:13]
    *p_bank = (U16) (MrcOemMemoryRightShiftU64 (dimm_line, 7) & 7);

    if( enhanced_interleave_mode ) {
      // bank = DimmAddress[15:13] ^ DimmAddress[18:16]
      *p_bank = *p_bank ^ ((U16) (MrcOemMemoryRightShiftU64 (dimm_line, 10) & 7));
    }

    // row = DimmAddress[31..27:16] depending on DIMM size. mask already prepared.
    row_mask = row_mask & dimm_line; // use row_mask to hold row because it is "U64"
    row_mask = MrcOemMemoryRightShiftU64 (row_mask, 10); // shift off bank and col bits
    *p_row = (U16) row_mask;
  }

  return TRUE;
}


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
  )
{
  U32  MAD_DIMM; // MAD_DIMM register for chosen channel
  U64  dimm_select; // DIMM A=0, B=1
  U64  dimm_size; // size of selected DIMM
  BOOL dual_rank; // number of ranks on selected DIMM
  U64  bit_above_row; // one-hot bit to mark the size of the row address
  BOOL dimm_and_rank_interleaving; // modes for the selected channel
  BOOL high_order_rank_interleave;
  BOOL enhanced_interleave_mode;
  U16  hori_addr; // bits to use from address for HORI
  U16  row_mask; // used to insert rank bit inbetween row bits in HORI mode
  U64  dimm_line; // DIMM address space (cache-line)
  U64  chan_line; // Channel address space (cache-line)
  U16  num_col_bits; // The number of column address bits the DIMM has.
  // sizes of the DIMMs on the channel (cache-line)
  U64  dimm_a_size;
  U64  dimm_b_size;
  U16  chan_select; // 0 = Channel A, 1 = Channel B
  // MAD_ZR register fields
  U64  bandc;    // (B + C)
  // address before reverse decoding the remap region
  U64  remap_line; // cache-line
  U64  remap_addr; // full address
  // stacked channel variables
  BOOL stacked_mode;
  U16  stacked_encoding;
  U64  stacked_size;
  U16  lsb_mask_bit; // LsbMaskBit field from CHANNEL_HASH register
  U16  hash_mask; // channel hash mask from CHANNEL_HASH register
  U16  hash_line; // lower bits of remap_line with hash_mask applied
  U16  hash_bit; // bit that gets destroyed in forward decode
  U16  i; // loop counter
  U64  top_of_remaped_mem; // used for reverse decode of remap region

  // perform some checks on the inputs

  // illegal channel check
  if (chan & ~((U16) 1)) {
    return FALSE;
  }

  // select our MAD_DIMM register. Ignore channel 2
  MAD_DIMM = chan ? MAD_DIMM_ch1 : MAD_DIMM_ch0;

  // check for too high of a DIMM index
  if (dimm & ~((U16) 1)) {
    return FALSE;
  }

  // is it DIMM A or B? A=0, B=1
  dimm_select = (U64) (dimm ^ get_dimm_a_select(MAD_DIMM));

  // get DIMM size
  dimm_size = dimm_select ? get_dimm_b_size_as_cl (MAD_DIMM) : get_dimm_a_size_as_cl (MAD_DIMM);

  // check if DIMM slot is populated
  if (dimm_size == 0) {
    return FALSE;
  }

  // check for too high of a rank index
  if (rank & ~((U16) 1)) {
    return FALSE;
  }

  // get number of ranks on DIMM
  dual_rank = dimm_select ? get_dimm_b_number_of_ranks (MAD_DIMM) : get_dimm_a_number_of_ranks (MAD_DIMM);

  // check that rank exists on DIMM
  if (rank && !dual_rank) {
    return FALSE;
  }

  // check for too high of a bank index
  if (bank & ~((U16) 0x7)) {
    return FALSE;
  }

  num_col_bits = dimm_select ? get_dimm_b_num_col_bits (MAD_CHNL, MAD_DIMM) :
                               get_dimm_a_num_col_bits (MAD_CHNL, MAD_DIMM);

  // Set a bit in a position that is one bit higher than the highest row bit
  // in the DIMM address space (cacheline address).
  //
  //  Most-Significant-Bits of Supported DRAM Chip Organizations (num bits - 1):
  //
  //  Type   Config  Device-Size  Row  Col  Bank  Rank-Size
  //  -----  ------  -----------  ---  ---  ----  ---------
  //  DDR3   x8      512 Mbit     12   9    2     512 MByte
  //  DDR3   x8      1 Gbit       13   9    2     1 GByte
  //  DDR3   x8      2 Gbit       14   9    2     2 GByte
  //  DDR3   x8      4 Gbit       15   9    2     4 GByte
  //  DDR3   x16     512 Mbit     11   9    2     256 MByte
  //  DDR3   x16     1 Gbit       12   9    2     512 MByte
  //  DDR3   x16     2 Gbit       13   9    2     1 GByte
  //  DDR3   x16     4 Gbit       14   9    2     2 GByte
  //  LPDDR  x16     2 Gbit       13   9    2     1 GByte
  //  LPDDR  x16     4 Gbit       13   10   2     2 GByte
  //  LPDDR  x16     8 Gbit       14   10   2     4 GByte
  //  LPDDR  x32     2 Gbit       13   8    2     512 MByte
  //  LPDDR  x32     4 Gbit       13   9    2     1 GByte
  //  LPDDR  x32     8 Gbit       14   9    2     2 GByte
  //
  // dimm size (GB) | dimm_size (cache-line)
  // ---------------+-----------------------
  //        8 GB    |        1<<27
  //        4 GB    |        1<<26
  //        2 GB    |        1<<25
  //        1 GB    |        1<<24
  //        0.5 GB  |        1<<23
  //        0.25 GB |        1<<22
  //
  bit_above_row = MrcOemMemoryRightShiftU64 (dimm_size, (U8) (num_col_bits + ((U16) dual_rank)));

  // Check for unexpected high-order row bits
  if (row & ~(((U32) bit_above_row) - 1)) {
    return FALSE;
  }

  // check for unexpected high-order column bits
  if (col & ~((((U16) 1) << num_col_bits) - 1)) {
    return FALSE;
  }


  //
  // Done with checking. Now reverse decode the address.
  //

  // determine if we are doing DIMM and Rank interleaving
  dimm_and_rank_interleaving = get_dimm_and_rank_intlv (MAD_DIMM);

  // determine if we are doing high order rank interleave
  high_order_rank_interleave = get_high_order_intlv_mode (MAD_DIMM);

  // determine if we are doing Enhanced Interleave Mode (EIM) (XOR rank & bank bits)
  enhanced_interleave_mode   = get_enhanced_intlv_mode (MAD_DIMM);

  // start building the DIMM Address Space (as a cache-line address)

  dimm_line = 0x0ULL;

  // build the rank, bank, and row parts of the DIMM space address

  if (dual_rank && high_order_rank_interleave) {
    hori_addr = get_hori_addr (MAD_DIMM);

    // Put the row part of the address into the dimm address

    // Create a mask with all 1's in the positions of the row bits below the rank bit
    row_mask = (1 << (hori_addr + 4)) - 1;

    // Split the row address at the rank bit and put it into the dimm address
    dimm_line = dimm_line | MrcOemMemoryLeftShiftU64 (((U64) (((row & ~row_mask) << 1) | (row & row_mask))), (16 - 6));

    // put the bank part of the address into the dimm address
    dimm_line = dimm_line | ((U64) (bank << (13 - 6)));

    // Rank bit goes into the spot specified by hori_addr
    dimm_line = dimm_line | ((U64) (rank << (20 - 6 + hori_addr)));

    // reverse the XOR operation for enhanced interleave mode
    //
    // bank = DimmAddress[15:13] XOR DimmAddress[18:16]
    //
    if (enhanced_interleave_mode) {
      dimm_line = dimm_line ^ MrcOemMemoryRightShiftU64 ((dimm_line & 0x1C00ULL), 3);
    }
  } else if (dual_rank && dimm_and_rank_interleaving) {
    // put in the row part of the address
    dimm_line = dimm_line | MrcOemMemoryLeftShiftU64 (((U64) row), 11);
    //
    // rank = DimmAddress[15:15]
    // bank = {DimmAddress[16:16],DimmAddress[14:13]}
    //
    dimm_line = dimm_line | MrcOemMemoryLeftShiftU64 (((U64) rank), 9);
    dimm_line = dimm_line | (MrcOemMemoryLeftShiftU64 (((U64) bank), 8) & 0x400ULL) |
                            (MrcOemMemoryLeftShiftU64 (((U64) bank), 7) & 0x180ULL);
    //
    // reverse the XOR operation for enhanced interleave mode
    //
    // rank = DimmAddress[15:15] XOR DimmAddress[19:19]
    // bank = {DimmAddress[16:16],DimmAddress[14:13]} XOR
    //        {DimmAddress[20:20],DimmAddress[18:17]}
    //
    if (enhanced_interleave_mode) {
      dimm_line = dimm_line ^ MrcOemMemoryRightShiftU64 ((dimm_line & 0x7800ULL), 4);
    }
  } else {
    // put in the row part of the address
    dimm_line = dimm_line | ((U64) (row << 10));

    // put in the bank part of the address
    dimm_line = dimm_line | ((U64) (bank << 7));

    // rank 1 will set the rank bit
    if (rank) {
      dimm_line = dimm_line | MrcOemMemoryLeftShiftU64 (bit_above_row, 10);
    }
    //
    // reverse the XOR operation for enhanced interleave mode
    //
    // bank = DimmAddress[15:13] XOR DimmAddress[18:16]
    //
    if (enhanced_interleave_mode) {
      dimm_line = dimm_line ^ MrcOemMemoryRightShiftU64 ((dimm_line & 0x1C00ULL), 3);
    }
  }

  // The column address bits make up the bottom of the DIMM address space.
  // With the addition of LPDDR to HSW-ULT, the number column address bits may
  // change from the standard 10 with DDR3 to 9 or 11 with some of the LPDDR
  // organizations.  The entire DIMM address space can be shifted up or down
  // with this change, then the column address can be inserted.
  //
  if (num_col_bits == 9) {
    // rank, bank, and row bits are in 10-col-bit locations in dimm_line, shift them down to 9-col-bit locations.
    dimm_line = MrcOemMemoryRightShiftU64 (dimm_line, 1);
  }

  if (num_col_bits == 11) {
    // rank, bank, and row bits are in 10-col-bit locations in dimm_line, shift them up to 11-col-bit locations.
    dimm_line = MrcOemMemoryLeftShiftU64 (dimm_line, 1);
  }

  // get the low order DIMM address space bits from the column
  dimm_line = dimm_line | col >> 3; // no need to mask col because of previous checks




  //
  // DIMM address to channel address
  //

  // Find the DIMM sizes on our selected channel. adjust to cache-line granularity
  dimm_a_size = get_dimm_a_size_as_cl (MAD_DIMM);
  dimm_b_size = get_dimm_b_size_as_cl (MAD_DIMM);

  //
  // Both dimm_and_rank_interleaving and high_order_rank_interleave cause DIMM interleaving.
  //
  if (dimm_and_rank_interleaving || high_order_rank_interleave) {
    if (dimm_line < dimm_b_size) { // Range 0 if DIMM address is less than DIMM B's size
      //
      // 2-way DIMM interleave. Channel address [15:15] is used to select DIMM.
      // Need to insert dimm_select bit there.
      //
      chan_line = (MrcOemMemoryLeftShiftU64 (dimm_line, 1) & 0xFFFFFFFFFFFFFC00ULL) |
                   MrcOemMemoryLeftShiftU64 (dimm_select, 9)                         |
                  (dimm_line & 0x01FFULL);
    } else { // not Range 0, must be Range 1
      // Channel address is DIMM A address with DIMM B's contribution from Range 0 added in
      chan_line = dimm_line + dimm_b_size;
    }
  } else { // no DIMM and Rank interleaving (nor HORI).
    chan_line = dimm_line;
    if (dimm_select) { // DIMM is B
      chan_line += dimm_a_size;
    }
  }


  //
  // Channel address to remaped system address
  //

  // map physical channel to A or B
  //
  chan_select = physical_to_logical_chan (MAD_CHNL, chan);


  //
  // MAD_ZR Register field values are in 256MB granularity.
  // They are stored here adjusted to cache-line granularity.
  bandc    = get_bandc_as_cl (MAD_ZR);

  // determine if we are in stacked mode; and if so, what the stacked size is.
  stacked_mode = get_stacked_mode (MAD_CHNL);

  if (stacked_mode) {
    stacked_encoding = get_stacked_encoding (MAD_CHNL);
    stacked_size = get_stacked_memory_size (stacked_encoding);

    remap_line = chan_line;
    //
    // In stacked mode, the channel is chosen based on the bit corresponding to the
    // size of the stacked register.  Bit-wise 'OR' in the channel selection bit into that
    // position.
    //
    remap_line |= chan_select << (22 + stacked_encoding);
  } else if (chan_line < bandc) { // Zone 1
    remap_line = MrcOemMemoryLeftShiftU64 (chan_line, 1);

    // Determine if the channel hash feature is being used
    if (CHANNEL_HASH & HSW_MC_ADDR_CHAN_HASH_ENABLE_MASK) { // test enable bit
      lsb_mask_bit = get_ch_hash_lsb_mask_bit (CHANNEL_HASH);
      hash_mask = get_ch_hash_mask (CHANNEL_HASH);

      // Don't need to force the selected lsb_mask_bit to be on because bit at lsb_mask_bit will be zero
      //hash_mask = hash_mask | (1 << lsb_mask_bit);

      // Reverse the swap of sys_addr bit 6 with bit pointed to by lsb_mask_bit
      remap_line = remap_line | (MrcOemMemoryRightShiftU64 (remap_line, (U8) lsb_mask_bit) & 0x0000000000000001ULL); // copy lsb_mask_bit to bit 6
      remap_line = remap_line & (~MrcOemMemoryLeftShiftU64 (0x0000000000000001ULL, (U8) lsb_mask_bit)); //  zero out lsb_mask_bit

      // Get the bits used to produce chan_select, sans the bit at lsb_mask_bit
      hash_line = ((U16) remap_line) & hash_mask;
      //
      // Recreate the value of the bit at lsb_mask_bit by doint the hash
      // XORs.
      //
      hash_bit = 0;
      for (i = 0 ; i < HSW_MC_ADDR_CHANNEL_HASH_MASK_SIZE ; i++) {
        hash_bit = hash_bit ^ (hash_line >> i);
      }
      hash_bit = hash_bit & 1;
      //
      // Recreate the missing bit by XORing the chan_select (the result of
      // the forward decode).
      // (If X = A ^ B, then A = X ^ B)
      //
      hash_bit = hash_bit ^ chan_select;

      // put the missing bit back into the address
      remap_line = remap_line | (hash_bit << lsb_mask_bit);
    } else {
      // Without the hash, sys_addr[6:6] determines the channel
      remap_line |= ((U64) chan_select);
    }
  } else { // Zone 2
    remap_line = chan_line + bandc; // This works if we consider C or not.
  }


  // zero out unused register bits
  TOLUD       &= HSW_MC_ADDR_TOLUD_MASK;
  REMAP_BASE  &= HSW_MC_ADDR_REMAP_MASK;
  REMAP_LIMIT &= HSW_MC_ADDR_REMAP_MASK;

  REMAP_LIMIT |= HSW_MC_ADDR_REMAP_LIMIT_LOWER_BITS_MASK;

  // work on full address instead of cache-line address;
  remap_addr = MrcOemMemoryLeftShiftU64 (remap_line, 6);

  //
  // Determine if the address is under the remap zone and therefore must be a
  // TCM.  remap_line can't be at or above TOM (Top Of Memory), so no need to
  // check that.  Simply check if the remap_line is between the base and
  // limit.
  //
  *p_is_tcm = (remap_addr <= REMAP_LIMIT) && (remap_addr >= REMAP_BASE); // b4194941 - REMAP_LIMIT is now inclusive


  //
  // reverse decode the remap region
  //

  *p_sys_addr = remap_addr; // if the remap doesn't apply system address is remap address

  if (!(*p_is_tcm) && (REMAP_LIMIT > REMAP_BASE)) { // remap doesn't apply if remap zone disabled
    top_of_remaped_mem = (U64) TOLUD;
    top_of_remaped_mem += REMAP_LIMIT;
    top_of_remaped_mem -= REMAP_BASE;
    if ((remap_addr <= top_of_remaped_mem) && (remap_addr >= ((U64) TOLUD))) {
      // remap applies. move the address to the remap zone
      *p_sys_addr -= ((U64) TOLUD);
      *p_sys_addr += REMAP_BASE;
    }
  }

  // restore cache-line chunk order
  *p_sys_addr = *p_sys_addr | (MrcOemMemoryLeftShiftU64 (((U64) col), 3) & 0x3FULL);

  // successful reverse address decode
  return TRUE;
}