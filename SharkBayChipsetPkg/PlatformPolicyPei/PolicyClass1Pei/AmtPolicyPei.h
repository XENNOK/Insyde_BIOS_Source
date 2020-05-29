/** @file
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _AMT_POLICY_PEI_H_
#define _AMT_POLICY_PEI_H_

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
//[-start-121224-IB10870061-add]//
#include <Library/PeiOemSvcChipsetLib.h>
//[-end-121224-IB10870061-add]//
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/AmtPlatformPolicyPei.h>
#include <ChipsetSetupConfig.h>

VOID
DumpAMTPolicyPEI (
  IN      CONST EFI_PEI_SERVICES             **PeiServices,
  IN      PEI_AMT_PLATFORM_POLICY_PPI        *AMTPlatformPolicyPpi
  );

#endif
