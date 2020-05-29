/** @file

  Memory controller IO configuration definition header.
  
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

#ifndef _MRC_IO_CONTROL_H_
#define _MRC_IO_CONTROL_H_

#include <MrcTypes.h>
#include <McAddress.h>
#include <MrcCommon.h>
#include <MrcGlobal.h>

/**
  Reset the MC IO module. The MC hardware will handle creating the 20 dclk pulse 
  after the bit is set and will also clear the bit.

  @param[in] MrcData - Include all MRC global data.
  
  @retval mrcSuccess    - IO Reset was done successfully
  @retval mrcDeviceBusy - Timed out waiting for the IO to clear the bit
*/
MrcStatus
IoReset (
  IN MrcParameters *const MrcData
  );

#endif // _MrcIoControl_h_
