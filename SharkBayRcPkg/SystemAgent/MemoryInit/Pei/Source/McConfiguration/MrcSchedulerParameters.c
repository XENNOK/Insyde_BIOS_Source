/** @file
  This module configures the memory controller scheduler.

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
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved.

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

//
// Include files
//
#include "MrcSchedulerParameters.h"

/**
@brief
  This function configures the memory controller scheduler.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
void
MrcSchedulerParametersConfig (
  IN MrcParameters *const MrcData
  )
{
  const MrcInput                       *Inputs;
  MrcOutput                            *Outputs;
  MCSCHEDS_CR_SCHED_CBIT_STRUCT        SchedCbit;
  MCSCHEDS_CR_SCHED_SECOND_CBIT_STRUCT SchedSecondCbit;
  U8                                   Channel;
  U32                                  Offset;
#ifdef ULT_FLAG
  MCHBAR_CH0_CR_CMD_RATE_STRUCT        CmdRate;
#endif // ULT_FLAG

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Channel = 0;
  Offset  = 0;
  if ((Inputs->CpuModel == cmHSW) && (Inputs->CpuStepping == csHswA0)) {
    SchedCbit.Data = SCHED_CBIT_DEFAULT;
  } else {
    SchedCbit.Data = SCHED_CBIT_DEFAULT_B0;
  }
#ifdef ULT_FLAG
  if (Inputs->CpuModel == cmHSW_ULT) {
    SchedCbit.Bits.dis_odt = 1;
    if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
      //
      // LPDDR3 DDP and QDP (multi-die) packages share the same ZQ pin,
      // so we need to serialize the ZQ calibration for different ranks.
      //
      SchedCbit.Bits.Serialize_ZQ = 1;

      if (Inputs->LpddrDramOdt) {
        //
        // DRAM ODT is used
        //
        SchedCbit.Bits.dis_odt = 0;
      }
    }
  }
#endif

  MrcWriteCrMulticast (MrcData, MCSCHEDS_CR_SCHED_CBIT_REG, SchedCbit.Data);

  MrcWriteCrMulticast (MrcData, MCMNTS_CR_SC_WDBWM_REG, SC_WDBWM_DEFAULT);

  if (Outputs->AsyncOdtDis) {
    SchedSecondCbit.Data                = 0;
    SchedSecondCbit.Bits.dis_async_odt  = 1;
    MrcWriteCR8 (MrcData, MCSCHEDS_CR_SCHED_SECOND_CBIT_REG + 1, SchedSecondCbit.Data8[1]);
  }

#ifdef ULT_FLAG
  //
  // For LPDDR3, set Command Rate Limit to 3
  //
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        Offset = MCHBAR_CH0_CR_CMD_RATE_REG + ((MCHBAR_CH1_CR_CMD_RATE_REG - MCHBAR_CH0_CR_CMD_RATE_REG) * Channel);
        CmdRate.Data = MrcReadCR (MrcData, Offset);
        CmdRate.Bits.enable_cmd_rate_limit  = 1;
        CmdRate.Bits.cmd_rate_limit         = 3;
        MrcWriteCR (MrcData, Offset, CmdRate.Data);
      }
    }
  }
#endif


  return;
}
