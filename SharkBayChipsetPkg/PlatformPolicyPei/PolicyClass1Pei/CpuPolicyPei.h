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

#ifndef _CPU_POLICY_PEI_H_
#define _CPU_POLICY_PEI_H_

#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
//[-start-121226-IB10870063-add]//
#include <Library/PeiOemSvcChipsetLib.h>
//[-end-121226-IB10870063-add]//
#include <Ppi/CpuPlatformPolicy.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/AcpiVariable.h>
#include <ChipsetSetupConfig.h>
//[-start-120925-IB10820124-add]//
//[-start-120425-IB05300312-add]//
#include <Library/BaseCryptLib.h>
//[-end-120425-IB05300312-add]//
//[-start-120824-IB05300324-add]//
#include <PchAccess.h>
//[-end-120824-IB05300324-add]//
//[-start-121222-IB10820209-remove]//
//#include <Compatibility/FlashMap.h>
//[-end-121222-IB10820209-remove]//
#include <Library/IoLib.h>
//[-end-120925-IB10820124-add]//
//[-start-120911-IB05300327-add]//
//[-start-130129-IB10820225-remove]//
//#include <OemPfatConfig.h>
//[-end-130129-IB10820225-remove]//
//[-end-120911-IB05300327-add]//
//[-start-120918-IB03780459-add]//
#include <PowerMgmtDefinitions.h>
//[-end-120918-IB03780459-add]//
//[-start-130118-IB11410043-add]//
#include <Ppi/Wdt.h>
//[-end-130118-IB11410043-add]//

VOID
DumpCPUPolicyPEI (
  IN      CONST EFI_PEI_SERVICES             **PeiServices,
  IN      PEI_CPU_PLATFORM_POLICY_PPI        *CPUPlatformPolicyPpi
  );

EFI_STATUS
CpuPolicyCallback (
  IN      CONST EFI_PEI_SERVICES             **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR          *NotifyDescriptor,
  IN      VOID                               *Ppi
  );

#endif
