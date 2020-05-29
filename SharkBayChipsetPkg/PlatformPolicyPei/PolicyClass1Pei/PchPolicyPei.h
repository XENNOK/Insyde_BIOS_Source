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

#ifndef _PCH_POLICY_PEI_H_
#define _PCH_POLICY_PEI_H_
//[-start-130809-IB06720232-remove]//
//#include <Library/EcLib.h>
//[-end-130809-IB06720232-remove]//
#include <Library/DebugLib.h>
//[-start-130710-IB05160465-modify]//
#include <Library/BaseMemoryLib.h>
//[-end-130710-IB05160465-modify]//
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
//[-start-121224-IB10870061-add]//
#include <Library/PeiOemSvcChipsetLib.h>
//[-end-121224-IB10870061-add]//
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/PchPlatformPolicy.h>
#include <PchAccess.h>
#include <ChipsetSetupConfig.h>
//[-start-121214-IB10820195-remove]//
//#include <BaseAddrDef.h>

//#define PCH_LPC_RCBA_BASE_ADDRESS         0xFED1C000
//[-end-121214-IB10820195-remove]//
//[-start-120628-IB06460410-remove]//
//#define PCH_LPC_ACPI_BASE_ADDRESS         0x400
//#define PCH_LPC_GPIO_BASE_ADDRESS         0x500
//[-end-120628-IB06460410-remove]//
//[-start-120807-IB04770241-modify]//
//#define PCH_TEMP_MEM_BASE_ADDRESS         0xFED08000
#define PCH_TEMP_MEM_BASE_ADDRESS         0xDFFF0000
//[-end-120807-IB04770241-modify]//
//[-start-121214-IB10820195-remove]//
//#define PCH_HPET_BASE_ADDRESS             0xFED00000
//[-end-121214-IB10820195-remove]//

VOID
DumpPCHPolicyPEI (
  IN      CONST EFI_PEI_SERVICES         **PeiServices,
  IN      PCH_PLATFORM_POLICY_PPI        *SAPlatformPolicyPpi
  );

#endif
