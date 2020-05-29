/** @file

  The third stage of the write training is determining the PI setting for each 
  byte strobe to make sure that data is sent at the optimal location. 
  In order to do that a pattern of alternating zeros and ones is written to a block of the memory, and then read out. 
  By identifying the location where it is farthest away from where errors are shown the DQS will be aligned to the 
  center of the eye. 

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

#ifndef _MRC_WRITE_DQ_DQS_H_
#define _MRC_WRITE_DQ_DQS_H_

#include <MrcTypes.h>
#include <McAddress.h>
#include <MrcApi.h>
#include <MrcCommandTraining.h>
#include <MrcCommon.h>
#include <MrcGlobal.h>
#include <MrcReset.h>

/**

  @brief
    this function executes the write timing centering in 2D.
    Final write timing centering using 2D algorithm and per bit optimization.

    @param[in] MrcData - Include all MRC global data.

    @retval MrcStatus -  if it succeded returns mrcSuccess

  **/
extern
MrcStatus
MrcWriteTimingCentering (
  IN     MrcParameters *const MrcData
  );

/**

  @brief
    this function executes the write timing centering in 2D.
    Final write timing centering using 2D algorithm and per bit optimization.

    @param[in] MrcData - Include all MRC global data.

    @retval MrcStatus -  if it succeded returns mrcSuccess

  **/
extern
MrcStatus
MrcWriteTimingCentering2D (
  IN     MrcParameters *const MrcData
  );

/**

@brief

  Rank Margin Tool - Measure margins across various parameters

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus -  mrcSuccess if succeded

**/
extern
MrcStatus
MrcRankMarginTool (
  IN OUT MrcParameters *const MrcData
  );

/**

@brief

  this function execute the write timing centering in 2D.
  Final write timing centering using 2D algorithm and per bit optimization

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus -  if it succeded returns mrcSuccess

**/
/**

@brief

  Peform Read Voltage Centering in 2D.
  Note: This function currently only supports param = WrV

  @param[in, out] MrcData       - Include all MRC global data.

  @retval MrcStatus -    if it succeded return mrcSuccess
  @todo - Need option for loopcount

**/
extern
MrcStatus
MrcWriteVoltageCentering2D (
  IN OUT MrcParameters *const MrcData
  );

#endif // _MrcWriteDqDqs_h_
