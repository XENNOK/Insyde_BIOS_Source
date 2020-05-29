/** @file
  This module includes the memory controller scheduler parameters.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*****************************************************************************

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

#ifndef _MRC_SCHEDULER_PARAMETERS_H_
#define _MRC_SCHEDULER_PARAMETERS_H_

#include <McAddress.h>
#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcCommon.h>
#include <MrcGlobal.h>

#define SCHED_CBIT_DEFAULT    (0x00100030)
#define SCHED_CBIT_DEFAULT_B0 (0x00100000)

#define SC_WDBWM_DEFAULT      (0x553C3038)

/**
@brief
  This function configures the memory controller scheduler.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
void
MrcSchedulerParametersConfig (
  IN MrcParameters *const MrcData
  );

#endif // _MrcSchedulerParameters_h_
