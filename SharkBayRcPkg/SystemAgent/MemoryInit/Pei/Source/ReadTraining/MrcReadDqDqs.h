/** @file
  Read DQ/DQS training definitions.

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

#ifndef _MrcReadDqDqs_h_
#define _MrcReadDqDqs_h_

#include <McAddress.h>
#include <MrcTypes.h>
#include <MrcCommon.h>
#include <MrcGlobal.h>

#define RMPR_DQS_START (-32)
#define RMPR_DQS_STOP  (32)

#define RMPR_DQS_STEP  (1)

#define RMPR_MIN_WIDTH (12)

/**
@brief
  Perform Read MPR Training.
  Center read DQ-DQS with MPR pattern.

  @param[in, out] MrcData   - Include all MRC global data.

  @retval MrcStatus - mrcSuccess or reason for failure.
**/
extern
MrcStatus
MrcReadMprTraining (
  IN OUT MrcParameters *const MrcData
  );

/**
@brief
  Peform Read Timing Centering.
  Center Rx DQS-DQ using moderate pattern with 1D eye

  @param[in] MrcData         - Include all MRC global data.

  @retval MrcStatus       - if it succeded returns mrcSuccess
**/
extern
MrcStatus
MrcReadTimingCentering (
  IN     MrcParameters *const MrcData
  );

/**
@brief
  Peform Read Timing Centering in 2D.
  Final read timing centering using 2D algorithm and per bit optimization

  @param[in] MrcData         - Include all MRC global data.

  @retval MrcStatus       - if it succeded returns mrcSuccess
**/
extern
MrcStatus
MrcReadTimingCentering2D (
  IN     MrcParameters *const MrcData
  );

/**
@brief
  Peform Read Voltage Centering in 2D.
  Note: This function currently only supports param = RdV

  @param[in] MrcData       - Include all MRC global data.

  @retval MrcStatus       - if it succeded return mrcSuccess
**/
extern
MrcStatus
MrcReadVoltageCentering2D (
  IN     MrcParameters *const MrcData
  );

#endif // _MrcReadDqDqs_h_
