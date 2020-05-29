/** @file
  This file is used as a driver to all memory controller IO registers.
  It includes all the functions that the MRC needs to configure the IO and do the training.

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

//
// Include files
//
#include <MrcIoControl.h>

/**
@brief

  Reset the MC IO module. The MC hardware will handle creating the 20 dclk pulse 
  after the bit is set and will also clear the bit.

  @param[in] MrcData - Include all MRC global data.
  
  @retval mrcSuccess    - IO Reset was done successfully
  @retval mrcDeviceBusy - Timed out waiting for the IO to clear the bit

**/
MrcStatus
IoReset (
  IN MrcParameters *const MrcData
  )
{
  MCDECS_CR_MC_INIT_STATE_G_MCMAIN_STRUCT McInitStateG;
  BOOL                                    Busy;
  U32                                     Timeout;
  
  Timeout = (U32) MrcGetCpuTime () + 10000;   // 10 seconds timeout

  McInitStateG.Data          = MrcReadCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG);
  McInitStateG.Bits.reset_io = 1;
  MrcWriteCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG, McInitStateG.Data);

  //
  // Wait until the bit is cleared by hardware
  //
  do {
    McInitStateG.Data = MrcReadCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG);
    Busy              = (McInitStateG.Bits.reset_io == 1);
  } while (Busy && ((U32) MrcGetCpuTime () < Timeout));

  return (Busy ? mrcDeviceBusy : mrcSuccess);
}
