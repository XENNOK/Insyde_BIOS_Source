/** @file

 Command training definitions.
  
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

///
/// Include files
///

#ifndef _MRC_COMMAND_TRAINING_H_
#define _MRC_COMMAND_TRAINING_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcCommon.h>
#include <MrcDdr3.h>
#include <MrcGlobal.h>
#include <MrcIoControl.h>
#include <MrcOem.h>
#include <MrcReadReceiveEnable.h>
#include <MrcReset.h>
#include <MrcTimingConfiguration.h>

#define ECT_CLK_START (0)
#define ECT_CLK_STOP  (128)

#define ECT_CLK_STEP  (2)

#define ECT_CLK_LOOPS (ECT_CLK_STOP / ECT_CLK_STEP)

#define ECT_DQS_START (-32)
#define ECT_DQS_STOP  (32)
#define ECT_DQS_STEP  (8)
#define ECT_MIN_WIDTH (16)

/**

@brief

  This function performs early command training.
  Center CTL-CLK timing to allow subsequent steps to work

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus -  mrcSuccess if it succeeded

**/
extern
MrcStatus
MrcEarlyCommandTraining (
  IN MrcParameters *const MrcData
  );

/**

@brief

  This function performs Late command training.
  Center CMD/CTL-CLK timing using complex patterns.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it's a success return mrcSuccess

**/
extern
MrcStatus
MrcLateCommandTraining (
  IN MrcParameters *const MrcData
  );

/**

@brief

  Perform Command Voltage Centering.

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus - if it succeded returns mrcSuccess

**/
extern
MrcStatus
MrcCmdVoltageCentering (
  IN OUT MrcParameters *const MrcData
  );

/**

@brief

  Centers Command Timing around a MidPoint

  @param[in] MrcData         - Include all MRC global data.
  @param[in] Iteration       - Determines which PI to shift
  @param[in] Ranks           - Valid Rank bit mask
  @param[in] GroupMask       - which LPDDR groups to work on for CMD/CLK/CKE; not used for DDR3
  @param[in] MidPoint        - The MidPoint to center around (per channel)

  @retval Nothing
**/
extern
void
CmdTimingCentering (
  IN MrcParameters *const MrcData,
  IN U8                   Iteration,
  IN U8                   Ranks,
  IN U8                   GroupMask,
  IN U8                   MidPoint[MAX_CHANNEL]
  );

/**
@brief

  Use a linear search to find the edges between Low and High
  if WrapAround = 0: Look for largest passing region between low and high
  if WrapAround = 1: Look for largest passing region, including wrapping from high to low

  @param[in, out] MrcData     - Include all MRC global data.
  @param[in]      Iteration   - Determines which PI to shift
  @param[in]      chBitMask   - Valid Channel bit mask
  @param[in]      Ranks       - Valid Rank bit mask
  @param[in]      GroupMask   - which LPDDR groups to work on for CMD/CLK/CKE; not used for DDR3
  @param[in]      Low         - Low limit
  @param[in]      High        - High limit
  @param[in]      WrapAllowed - Determines the search region
  @param[in]      VrefOffsets - Array of Vref offsets
  @param[in]      SkipPrint   - Switch to enable or disable debug printing
  @param[in]      SkipVref    - Skip changing CMD Vref offsets, only run test once at the current Vref.

  @retval MrcStatus -  If it succeeds return mrcSuccess

**/
extern
void
CmdLinearFindEdges (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   Iteration,
  IN     U8                   chBitMask,
  IN     U8                   Ranks,
  IN     U8                   GroupMask,
  IN     S8                   Low,
  IN     U8                   High,
  IN     U8                   WrapAllowed,
  IN     S8                   *VrefOffsets,
  IN     BOOL                 SkipPrint,
  IN     BOOL                 SkipVref
  );

/**

@brief

  Use a binary search to find the edge between Low and High
  High and Low track passing points
  if CountUp: Low is a passing point and need to count up to find a failing point
  if CountDn: High is a passing point and need to count dn to find a failing point

  @param[in]      MrcData     - Include all MRC global data.
  @param[in]      Iteration   - Determines which PI to shift
  @param[in]      ChBitMask   - Valid Channel bit mask
  @param[in]      Ranks       - Valid Rank bit mask
  @param[in]      GroupMask   - which LPDDR groups to work on for CMD/CLK/CKE; not used for DDR3
  @param[in, out] Low         - Low limit
  @param[in, out] High        - High limit
  @param[in]      CountUp     - The direction to search
  @param[in]      VrefOffsets - Array of Vref offsets

  @retval Nothing

**/
extern
void
CmdBinaryFindEdge (
  IN     MrcParameters *const MrcData,
  IN     U8                   Iteration,
  IN     U8                   ChBitMask,
  IN     U8                   Ranks,
  IN     U8                   GroupMask,
  IN OUT U8                   *Low,
  IN OUT U8                   *High,
  IN     U8                   CountUp,
  IN     S8                   *VrefOffsets
  );

/**

@brief

  Shift the CLK/CTL Timing
  Shift the CMD Timing

  @param[in] MrcData    - Include all MRC global data.
  @param[in] Channel    - Channel to shift
  @param[in] Offset     - Offset to shift by
  @param[in] UpdateHost - Switch to update the host structure

  @retval Nothing

**/
extern
void
ShiftChannelTiming (
  IN MrcParameters *const MrcData,
  IN U8                   Channel,
  IN S32                  Offset,
  IN U8                   UpdateHost
  );

/**

@brief

  This function updtes Command Mode register, tXP and Round trip latency

  @param[in, out] MrcData     - Include all MRC global data.
  @param[in]      Channel     - Channel to perform update to
  @param[in]      OldN        - Old N Mode value
  @param[in]      NewN        - New N mode value

  @retval Nothing

**/
extern
void
UpdateCmdNTiming (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   Channel,
  IN     U8                   OldN,
  IN     U8                   NewN
  );

#ifdef ULT_FLAG

typedef struct _CADB_LINE {
  U32 CaHigh;
  U32 CaLow;
  U32 ChipSelect;
} CADB_LINE;

typedef enum {
  CaTrainingMode41,     ///< Enter CA training mode using MRW41
  CaTrainingMode48,     ///< Enter CA training mode using MRW48
  CaTrainingMode42      ///< Exit CA training mode using MRW42
} MrcCaTrainingMode;

/**
@brief

  Early CA / CS training for LPDDR.
  Main flow:
  1. Put DRAMs in CA training mode using MRW41.
  2. Run CS vs. CLK training.
  3. Map DQ pins according to the board swizzling.
  4. Run CA vs. CLK training.
  5. Select optimal CA timings for each CA bus per rank

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess if succeeded

**/
extern
MrcStatus
EarlyCommandTrainingLpddr (
  IN MrcParameters *const MrcData
  );

/**
@brief

  Early CS / CLK training for LPDDR.
  Main flow:
  1. Setup CADB pattern for CS Training.
  2. Run CS vs. CLK training.
  3. Select optimal CS and CLK timings

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess if succeeded

**/
extern
MrcStatus
EarlyChipSelectTraining (
  IN MrcParameters *const MrcData
  );

/**
@brief

  Sweep right and left from the current point to find the margins.

  @param[in, out] MrcData     - Include all MRC global data.
  @param[in]      Iteration   - Determines which PI to shift
  @param[in]      ChannelMask - Valid Channel bit mask
  @param[in]      RankMask    - Valid Rank bit mask
  @param[in]      DebugPrint  - Print debug messages or not

  @retval MrcStatus -  If it succeeds return mrcSuccess

**/
void
CmdLinearFindEdgesLpddr (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   Iteration,
  IN     U8                   ChannelMask,
  IN     U8                   RankMask,
  IN     BOOL                 DebugPrint
  );

#endif // ULT_FLAG
#endif // _MrcCommandTraining_h_
