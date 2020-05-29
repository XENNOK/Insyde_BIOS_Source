/** @file
  This protocol provides the memory information data, such as
  total physical memory size, memory frequency, memory size
  of each dimm and rank.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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
  license agreement
**/
#ifndef _MEM_INFO_PROTOCOL_H_
#define _MEM_INFO_PROTOCOL_H_

///
/// Define the  protocol GUID
///
#define MEM_INFO_PROTOCOL_GUID \
  { \
    0x6f20f7c8, 0xe5ef, 0x4f21, 0x8d, 0x19, 0xed, 0xc5, 0xf0, 0xc4, 0x96, 0xae \
  }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID                   gMemInfoProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _MEM_INFO_PROTOCOL MEM_INFO_PROTOCOL;

//
// Protocol definitions
//
#define NODE_NUM  1
#define CH_NUM    2
#define DIMM_NUM  2
#define RANK_NUM  2
#define PROFILE_NUM 4 // number of memory profiles supported

#pragma pack(1)
typedef struct {
  UINT32 tCK;    ///< Memory cycle time, in femtoseconds.
  UINT16 NMode;  ///< Number of tCK cycles for the channel DIMM's command rate mode.
  UINT16 tCL;    ///< Number of tCK cycles for the channel DIMM's CAS latency.
  UINT16 tCWL;   ///< Number of tCK cycles for the channel DIMM's minimum CAS write latency time.
  UINT16 tFAW;   ///< Number of tCK cycles for the channel DIMM's minimum four activate window delay time.
  UINT16 tRAS;   ///< Number of tCK cycles for the channel DIMM's minimum active to precharge delay time.
  UINT16 tRC;    ///< Number of tCK cycles for the channel DIMM's minimum active to active/refresh delay time.
  UINT16 tRCD;   ///< Number of tCK cycles for the channel DIMM's minimum RAS# to CAS# delay time.
  UINT16 tREFI;  ///< Number of tCK cycles for the channel DIMM's minimum Average Periodic Refresh Interval.
  UINT16 tRFC;   ///< Number of tCK cycles for the channel DIMM's minimum refresh recovery delay time.
  UINT16 tRP;    ///< Number of tCK cycles for the channel DIMM's minimum row precharge delay time.
  UINT16 tRPab;  ///< Number of tCK cycles for the channel DIMM's minimum row precharge delay time for all banks.
  UINT16 tRRD;   ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time.
  UINT16 tRTP;   ///< Number of tCK cycles for the channel DIMM's minimum internal read to precharge command delay time.
  UINT16 tWR;    ///< Number of tCK cycles for the channel DIMM's minimum write recovery time.
  UINT16 tWTR;   ///< Number of tCK cycles for the channel DIMM's minimum internal write to read command delay time.
} MEMORY_TIMING;

typedef struct {
  UINT32  memSize;                                  ///< Total physical memory size
  UINT16  ddrFreq;                                  ///< DDR Frequency
  UINT16  ddrFreqMax;
  UINT8   RefClk;
  UINT8   Ratio;
  BOOLEAN EccSupport;                               ///< ECC Support
  UINT16  dimmSize[NODE_NUM * CH_NUM * DIMM_NUM];   ///< Dimm Size
  BOOLEAN DimmExist[NODE_NUM * CH_NUM * DIMM_NUM];  ///< Dimm Present or not
  UINT8   RankInDimm[NODE_NUM * CH_NUM * DIMM_NUM]; ///< No. of ranks in a dimm
  UINT8   Reserved[24];
  UINT8   *DimmsSpdData[NODE_NUM * CH_NUM * DIMM_NUM];
  UINT16  VddVoltage[PROFILE_NUM];
  MEMORY_TIMING Timing[PROFILE_NUM];
  UINT8   Profile;                                  ///< Currently running memory profile
  UINT8   XmpProfileEnable;                         ///< 0 = no XMP DIMMs in system
} MEMORY_INFO_DATA;
#pragma pack()

///
/// Protocol definition
///
struct _MEM_INFO_PROTOCOL {
  MEMORY_INFO_DATA  MemInfoData;
};

#endif
