/** @file
 Platform Overclocking Initialization Driver.

***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Platform.h>
#include <Ppi/Wdt.h>
#include <Guid/XTUInfoHob.h>
#include <Library/OverclockingLib.h>
#include <Library/HobLib.h>
#include <Cpu.h>
#include <PchRegs/PchRegsLpc.h>

EFI_STATUS
PlatformOcInit (
  IN OUT CHIPSET_CONFIGURATION          *SystemConfiguration
)
{
  EFI_STATUS                   Status;
  VOID                         *HobList;
  XTU_INFO_DATA                *XtuInfoHobData;
  WDT_PROTOCOL                 *WdtProtocol;
  UINT8                        WdtStatus;
  OC_CAPABILITIES_ITEM         OcCaps;
  UINT32                       LibStatus;
  UINT8                        DomainId;

  HobList = GetHobList ();

  XtuInfoHobData = GetNextGuidHob (&gXTUInfoHobGuid, HobList);

//[-start-140210-IB05400512-add]//
  ASSERT (XtuInfoHobData != NULL);
  if (XtuInfoHobData == NULL) {
    return EFI_NOT_FOUND;
  }
  XtuInfoHobData = (XTU_INFO_DATA*)GET_GUID_HOB_DATA (XtuInfoHobData);
//[-end-140210-IB05400512-add]//

  //
  // Initialize OC settings to default when it is invalid.   
  //
  if (SystemConfiguration->RatioLimit1Core == 0 ||
      SystemConfiguration->RatioLimit2Core == 0 ||
      SystemConfiguration->RatioLimit3Core == 0 ||
      SystemConfiguration->RatioLimit4Core == 0) {
    SystemConfiguration->RatioLimit1Core = XtuInfoHobData->TurboRatioLimitDefault.LimitCore1;
    SystemConfiguration->RatioLimit2Core = XtuInfoHobData->TurboRatioLimitDefault.LimitCore2;
    SystemConfiguration->RatioLimit3Core = XtuInfoHobData->TurboRatioLimitDefault.LimitCore3;
    SystemConfiguration->RatioLimit4Core = XtuInfoHobData->TurboRatioLimitDefault.LimitCore4;
  }

  if (SystemConfiguration->FlexRatio == 0) {
    SystemConfiguration->FlexRatio       = XtuInfoHobData->CpuNonTurboRatio.MaxRatio;
  }

  //
  // Initialize Overclocking Capabilities
  //
  SystemConfiguration->OcCapXe     = 0;
  SystemConfiguration->OcCapIaCore = 0;
  SystemConfiguration->OcCapGt     = 0;
  SystemConfiguration->OcCapClr    = 0;
  SystemConfiguration->OcCapUncore = 0;
  SystemConfiguration->OcCapIoa    = 0;
  SystemConfiguration->OcCapIod    = 0;

  //
  // Detect XE capability(include unlocked processors) :
  //   1. MSR:CEh[28] == 1     (Programmable Ratio Limits for Turbo Mode)
  //   2. MSR:194h[19:17] > 0  (Number of overclocking bins supported)
  //
  if ((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & BIT28) &&
      (AsmReadMsr64 (EFI_IA32_CLOCK_FLEX_MAX) & (BIT17|BIT18|BIT19)) != 0) {
    SystemConfiguration->OcCapXe = 1; 
  }
  
  //
  // Detect capabilities for IA Core, GT, CLR, Uncore, IOA, and IOD.
  //
  for (DomainId = OC_LIB_DOMAIN_ID_IA_CORE; DomainId <= OC_LIB_DOMAIN_ID_IOD; DomainId++) {
    ZeroMem (&OcCaps, sizeof (OcCaps));
    OcCaps.DomainId = DomainId;
    Status = GetOcCapabilities (&OcCaps, &LibStatus);
    if ((Status == EFI_SUCCESS && LibStatus == OC_LIB_COMPLETION_CODE_SUCCESS) &&
        (OcCaps.RatioOcSupported || OcCaps.VoltageOverridesSupported || OcCaps.VoltageOffsetSupported)) {
      switch (DomainId) {
        case OC_LIB_DOMAIN_ID_IA_CORE:
          SystemConfiguration->OcCapIaCore = 1;
          break;
          
        case OC_LIB_DOMAIN_ID_GT:
          SystemConfiguration->OcCapGt = 1;
          break;
          
        case OC_LIB_DOMAIN_ID_CLR:
          SystemConfiguration->OcCapClr = 1;
          break;
          
        case OC_LIB_DOMAIN_ID_UNCORE:
          SystemConfiguration->OcCapUncore = 1;
          break;
          
        case OC_LIB_DOMAIN_ID_IOA:
          SystemConfiguration->OcCapIoa = 1;
          break;
          
        case OC_LIB_DOMAIN_ID_IOD:
          SystemConfiguration->OcCapIod = 1;
          break;
          
        default:
          break;
      }
    }
  }

  //
  // Disable Overclocking Configuration when system hung in previous POST.
  //
  Status = gBS->LocateProtocol (&gWdtProtocolGuid, NULL, (VOID **)&WdtProtocol);
  ASSERT_EFI_ERROR (Status);

  WdtStatus = WdtProtocol->CheckStatus ();
  if (WdtStatus != V_PCH_OC_WDT_CTL_STATUS_OK) {
    SystemConfiguration->Overclocking = 0;
  }

  return EFI_SUCCESS;
}

