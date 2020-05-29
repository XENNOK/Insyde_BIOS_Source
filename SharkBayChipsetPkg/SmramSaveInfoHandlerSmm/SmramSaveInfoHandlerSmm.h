/** @file
  Header file for the SMM SMRAM Save Info Handler Driver.

@copyright
  Copyright (c) 2004 - 2013 Intel Corporation. All rights reserved
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
**/
#ifndef _SMM_SMRAM_SAVE_INFO_HANDLER_H_
#define _SMM_SMRAM_SAVE_INFO_HANDLER_H_

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <PchRegs/PchRegsLpc.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Guid/SmramCpuDataVariable.h>
#include <Protocol/CpuPlatformPolicy.h>

#endif
