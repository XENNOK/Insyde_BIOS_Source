/**

Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  AtPolicy.h

@brief
  TdtDxePolicyInit to check and set Tdt Platform Policy.

**/
/**
 This file contains an 'Intel Peripheral Driver' and is
 licensed for Intel CPUs and chipsets under the terms of your
 license agreement with Intel or your vendor.  This file may
 be modified by the user, subject to additional terms of the
 license agreement
**/

#ifndef _TDT_PLATFORM_POLICY_H_
#define _TDT_PLATFORM_POLICY_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
//[-start-121220-IB10820206-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121220-IB10820206-add]//
#include <Protocol/AtPlatformPolicy.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/MeBiosPayloadData.h>
#include <ChipsetSetupConfig.h>

#endif
