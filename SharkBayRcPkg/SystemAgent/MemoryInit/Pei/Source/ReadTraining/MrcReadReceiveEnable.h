/** @file
  Read receive enable training definitions.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
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

#ifndef _MRC_READ_RECEIVE_ENABLE_H_
#define _MRC_READ_RECEIVE_ENABLE_H_

#include <MrcTypes.h>
#include <MrcCommon.h>
#include <MrcCrosser.h>
#include <MrcGlobal.h>

#define HW_ROUNDT_LAT_DEFAULT_VALUE_A0              (20)  ///< HSW HW specific default value
#define MRC_ROUND_TRIP_IO_COMPENSATION_2_CHANNEL_A0 (25)  // Roundtrip - IO compensation for 2 channel

#define HW_ROUNDT_LAT_DEFAULT_VALUE_B0              (16)  ///< HSW HW specific default value
#define MRC_ROUND_TRIP_IO_COMPENSATION_2_CHANNEL_B0 (21)  // Roundtrip - IO compensation for 2 channel

#define HW_ROUNDT_LAT_DEFAULT_VALUE_ULT_A0              (18)  // Roundtrip Latency
#define MRC_ROUND_TRIP_IO_COMPENSATION_2_CHANNEL_ULT_A0 (23)  // Roundtrip - IO compensation for 2 channel

///
/// ReadReceiveEnable (RRE) parameters
/// Command training will reduce this by 64, so plan for that now in the ideal value
///
#define RRE_PI_IDEAL       (256 + 64)
#define RRE_ALL_RANKS_MASK (0x0F)
#define RRE_PI_TO_RESERVE  (64)

/**
@brief
  Perform receive enable training.
  Optimize RcvEn timing with MPR pattern

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus -  if succeeded, return mrcSuccess
**/
extern
MrcStatus
MrcReadLevelingTraining (
  IN OUT MrcParameters *const MrcData
  );

/**
@brief
  Apply an signed offset to all selected bytes/ ranks in a channel to RcvEn timing
  Robustly handles any carries to/from the IO Latency vs. RcvEn FlyBy
  PiReserve will reserve a certain number of +/- PI ticks for margin purposes
  Routine also minimizes the difference in RcvEn settings across ranks

  @param[in,out] MrcData       - MRC Global Data
  @param[in]     Channel       - The channel to adjust
  @param[in]     RankMask      - Mask of Ranks to adjust
  @param[in]     ByteMask      - Mask of Bytes to adjust by the RcvEnOffset
  @param[in]     RcvEnOffset   - Amount to offset RcvEn
  @param[in]     PiReserve     - The number of PiTicks to reserve on each edge of RcvEn

  @retval MrcStatus - mrcSuccess if successfull
                      mrcWrongInputParameter if channel doesnt exist or a RankMask of 0 is provided
                      mrcReadLevelingError if we over/underflow RT_IOCOMP field.
**/
MrcStatus
MrcChangeRcvEnTiming (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Channel,
  IN     const U8             RankMask,
  IN     const U16            ByteMask,
  IN     const S16            RcvEnOffset,
  IN     const S16            PiReserve
  );

/**
@brief
  Once the DQS high phase has been found (for each DRAM) the next stage is to find out the round trip latency,
  by locating the preamble cycle. This is achieved by trying smaller and smaller roundtrip
  values until the strobe sampling is done on the preamble cycle.
  The following algorithm is used to find the preamble cycle:

  @param[in] MrcData         - all the global data

  @retval Nothing.
**/
extern
MrcStatus
MrcRoundTripLatency (
  IN     MrcParameters *const MrcData
  );

/**
  Perform Receive Enable Timing Centering.
  Center Receive Enable using moderate pattern with 1D eye

  @param[in] MrcData         - Include all MRC global data.

  @retval MrcStatus       - if it succeded returns mrcSuccess
**/
MrcStatus
MrcReceiveEnTimingCentering (
  IN MrcParameters *const MrcData
  );

#endif // _MrcReadReceiveEnable_h_
