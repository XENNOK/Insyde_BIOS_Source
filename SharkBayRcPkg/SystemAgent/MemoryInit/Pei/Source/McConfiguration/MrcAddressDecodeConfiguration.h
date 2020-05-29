/** @file
  This module configures the memory controller address decoder.

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
  license agreement
*/

#ifndef _MRC_ADDRESS_DECODE_CONFIGURATION_H_
#define _MRC_ADDRESS_DECODE_CONFIGURATION_H_

#include <MrcTypes.h>
#include <MrcGlobal.h>
#include <MrcApi.h>
#include <McAddress.h>
#include <MrcCommon.h>
#include <MrcOem.h>
#include <MrcCommon.h>
#include <MrcOemDebugPrint.h>

/**
@brief
  This function is the main address decoding configuration function.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.

**/
extern
void
MrcAdConfiguration (
  IN MrcParameters *const MrcData
  );

/**
@brief
  This function configures the zone configuration registers MAD-CR and MAD-ZR-CR.

  @param[in, out] MrcData - Include all MRC global data.

  @retval Nothing.
**/
extern
void
ZoneConfiguration (
  IN OUT MrcParameters *const MrcData
  );

/**
@brief
  This function configures the MAD_DIMM_CH0/1 register.

  @param[in] MrcData      - Include all MRC global data.
  @param[in] ChannelIndex - Channel index

  @retval Nothing.
**/
extern
void
ChannelAddressDecodeConfiguration (
  IN MrcParameters *const MrcData,
  IN const U32            ChannelIndex
  );

#endif
