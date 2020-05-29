/** @file

  Describes the functions visible to the rest of the OcInit.

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

#ifndef _SA_INIT_H_
#define _SA_INIT_H_

///===============================================
///  MdePkg/Include/
///===============================================
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Cpu/Haswell/
///===============================================
#include <Library/OverclockingLib.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SystemAgent/
///===============================================
#include <Ppi/SaPlatformPolicy.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelPch/LynxPoint/
///===============================================
#include <Ppi/Wdt.h>

#ifdef USE_WDT_IN_DEBUG_BIOS
///
/// MRC takes a lot of time to execute in debug mode
///
#define WDT_TIMEOUT_BETWEEN_PEI_DXE 120
#else
#define WDT_TIMEOUT_BETWEEN_PEI_DXE 60
#endif

///
/// Function Prototypes
///
/**
  Initializes Overclocking settings in the processor.

  @param [in] PeiServices         General purpose services available to every PEIM.
  @param [in] OverclockingtConfig Pointer to Policy protocol instance

  @retval EFI_SUCCESS

**/
EFI_STATUS
SaOcInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  );

#endif
