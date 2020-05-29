/** @file

  This file contains functions that read the SPD data for each DIMM slot over
  the SMBus interface.

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

#ifndef _MRC_SPD_DRIVER_H_
#define _MRC_SPD_DRIVER_H_

#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcGlobal.h>
#include <MrcOemDebugPrint.h>
#include <MrcOemSmbus.h>

/**
  Read the SPD data over the SMBus, for all DIMM slots and copy the data to the MrcData structure. 
  The SPD data locations read is controlled by the current boot mode.

  @param[in] BootMode - Mrc Boot Mode
  @param[in] Inputs - Mrc Inputs structure

  @retval mrcSuccess if the read is successful, otherwise mrcDimmNotExist, which
  @retval indicates that no slots are populated.
*/
extern
MrcStatus
MrcGetSpdData (
  IN     const MrcBootMode BootMode,
  IN OUT MrcInput *const   Inputs
  );

#endif
