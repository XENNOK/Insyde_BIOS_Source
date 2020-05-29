/** @file

  The DDR3 reset sequence definitions.
    
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

#ifndef _MRC_RESET_H_
#define _MRC_RESET_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <McAddress.h>
#include <MrcCommon.h>
#include <MrcDdr3.h>
#include <MrcOemPlatform.h>

#include <PchRegs/PchRegsRcrb.h>

/**

@brief

  Perform full JEDEC reset and init sequence.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - mrcSuccess or reason for failure.

**/
extern
MrcStatus
MrcResetSequence (
  IN MrcParameters *const MrcData
  );

/**

@brief

  Perform JEDEC DIMM reset sequence.

  @param[in] MrcData - Include all MRC global data.

  @retval - none

**/
extern
void
MrcJedecReset (
  IN MrcParameters *const MrcData
  );

#ifdef ULT_FLAG

/**

@brief

  Perform JEDEC LPDDR3 DIMM reset sequence.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess if succeeded

**/
extern
MrcStatus
MrcJedecResetLpddr3 (
  IN MrcParameters *const MrcData
  );

/**

@brief

  Perform JEDEC LPDDR3 DIMM init sequence.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess if succeeded

**/
MrcStatus
MrcJedecInitLpddr3 (
  IN MrcParameters *const MrcData
  );

#endif // ULT_FLAG

/**

@brief

  Wait in a loop until the first RCOMP has been completed.
  MRC should wait until this bit is set before executing any DDR command.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcDeviceBusy - On Rcomp completion timeout.
  @retval mrcSuccess    - On Rcomp completion.

**/
extern
MrcStatus
CheckFirstRcompDone (
  IN MrcParameters *const MrcData
  );

/**

@brief

  Perform the required steps to exit self refresh in S3/Warm reset:
    Download the Read Reg File for all populated ranks.
    Assert CKE for all the ranks present to pull Dimms out of Self-Refresh.
    Issue long ZQ Calibration for all the ranks present in the channel.
    Set REUT to normal mode for all channels.
    Set the Power Down Config Register.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess

**/
MrcStatus
MrcSelfRefreshExit (
  IN MrcParameters *const MrcData
  );

#endif // _MrcReset_h_
