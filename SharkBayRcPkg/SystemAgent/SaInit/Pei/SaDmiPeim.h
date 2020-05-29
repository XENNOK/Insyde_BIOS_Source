/** @file

  Header file for the SA Dmi Init library.

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

#ifndef _SA_DMI_PEIM_H_
#define _SA_DMI_PEIM_H_

///===============================================
///  MdePkg/Include/
///===============================================
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelPch/LynxPoint/
///===============================================
#include <Ppi/PchInit.h>
#include <Ppi/PchDmiTcVcMap.h>
#include <Ppi/PchPlatformPolicy.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SystemAgent/
///===============================================
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Ppi/SaPlatformPolicy.h>


/**

  Map SA DMI TCs to VC

  @param[in] PchDmiTcVcMapPpi     - Instance of PCH_DMI_TC_VC_PPI
  @param[in] DmiBar               - DMIBAR address

  @retval EFI_SUCCESS

**/
EFI_STATUS
SaSetDmiTcVcMapping (
  IN    PCH_DMI_TC_VC_PPI  *PchDmiTcVcMapPpi,
  IN    UINT64             DmiBar
  )
;

/**

  Poll SA DMI negotiation completion

  @param[in] PchDmiTcVcMapPpi     - Instance of PCH_DMI_TC_VC_PPI
  @param[in] DmiBar               - DMIBAR address

  @retval EFI_SUCCESS

**/
EFI_STATUS
SaPollDmiVcStatus (
  IN    PCH_DMI_TC_VC_PPI  *PchDmiTcVcMapPpi,
  IN    UINT64             DmiBar
  )
;

#ifdef DMI_FLAG
/**

  DMI link training

  @param[in] DmiBar - DMIBAR address


**/
VOID
DmiLinkTrain (
  IN  UINT64 DmiBar
  )
;

/**

  Additional DMI Programming Steps at PEI

  @param[in] SaPlatformPolicyPpi - pointer to SA_PLATFORM_POLICY_PPI
  @param[in] MchBar              - MCHBAR address
  @param[in] DmiBar              - DMIBAR address

**/
VOID
AdditionalDmiProgramSteps (
  IN SA_PLATFORM_POLICY_PPI    *SaPlatformPolicyPpi,
  IN UINT32                    MchBar,
  IN UINT32                    DmiBar
  )
;
#endif // DMI_FLAG
#endif
