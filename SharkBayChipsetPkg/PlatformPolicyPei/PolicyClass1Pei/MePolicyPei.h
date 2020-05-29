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

#ifndef _ME_POLICY_PEI_H_
#define _ME_POLICY_PEI_H_

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/MePlatformPolicyPei.h>
//[-start-121004-IB10820133-add]//
#include <Ppi/PlatformMeHook.h>
//[-end-121004-IB10820133-add]//
#include <Library/PeiOemSvcChipsetLib.h>
//[-start-121123-IB11410024-add]//
#include <ChipsetSetupConfig.h>
#include <Ppi/ReadOnlyVariable2.h>
//[-end-121123-IB11410024-add]//

VOID
DumpMEPolicyPEI (
  IN      CONST EFI_PEI_SERVICES                 **PeiServices,
  IN      PEI_ME_PLATFORM_POLICY_PPI        *MEPlatformPolicyPpi
  );
//[-start-121004-IB10820133-add]//
EFI_STATUS
PreGlobalResetPeiHook (
  IN      CONST EFI_PEI_SERVICES       **PeiServices,
  IN      PLATFORM_ME_HOOK_PPI         * This
  );
//[-end-121004-IB10820133-add]//
#endif
