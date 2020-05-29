/** @file
  Execute TXT feature in pei phase.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>

#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/CmosLib.h>
#include <Library/IoLib.h>
//[-start-121212-IB10820191-remove]//
//#include <InsydeCpuRegs.h>
//[-end-121212-IB10820191-remove]//
#include <CpuAccess.h>
#include <PchAccess.h>
#include <ChipsetSetupConfig.h>
//[-start-121214-IB10820195-remove]//
//#include <BaseAddrDef.h>
//[-end-121214-IB10820195-remove]//
#include <ChipsetCmos.h>

//
// Function Definition
//
STATIC
EFI_STATUS
GetCsTxtCapabilities (
  IN     CONST EFI_PEI_SERVICES **PeiServices,
  IN OUT BOOLEAN                *IsCsTxtCapable
  );

STATIC
BOOLEAN
IsSmxSupported (
  VOID
  );

/**
 Get Chipset TXT capability.

 @param [in]        PeiServices         General purpose services available to every PEIM.
 @param [in]        SystemConfiguration A pointer to setup variables.
 @param [in]        IsVtSupport         VT support status
 @param [in, out]   CmosVmxSmxFlag      A temp data for VT and LT flag.

 @return            No Status Return.

*/
VOID
PeiTxtExecute (
  IN     CONST EFI_PEI_SERVICES **PeiServices,
  IN     CHIPSET_CONFIGURATION   *SystemConfiguration,
  IN     BOOLEAN                IsVtSupport,
  IN OUT UINT8                  *CmosVmxSmxFlag
  )
{
  EFI_STATUS                    Status;
  BOOLEAN                       ChipsetIsTxtCapable;
  BOOLEAN                       IsLtSupport;
  UINT8                         TempCmosVmxSmxFlag;
  UINT32                        IA32FeatureCntrl;
  UINT32                        Data32;
  UINT8                         TxtFlag;
  EFI_BOOT_MODE                 BootMode;

  //
  // Get Chipset TXT capability
  //
  GetCsTxtCapabilities (PeiServices, &ChipsetIsTxtCapable);

  if (SystemConfiguration->TXT) {
    *CmosVmxSmxFlag = *CmosVmxSmxFlag | B_SMX_SETUP_FLAG;
  }

  IsLtSupport = IsSmxSupported ();
  if (IsLtSupport) {
    *CmosVmxSmxFlag = *CmosVmxSmxFlag | B_SMX_CPU_FLAG;
  }

  Status = PeiServicesGetBootMode (&BootMode);

  if ((ChipsetIsTxtCapable == 0) && (BootMode != BOOT_ON_S3_RESUME) && (BootMode != BOOT_ON_S4_RESUME)) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "TxtExecute:: Warm boot or S3 !\n"));

    TxtFlag = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag);
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "TxtExecute:: Read TXT Flag By CMOS: %x\n", TxtFlag));

    if (TxtFlag & B_SMX_CHIPSET_FLAG){
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "TxtExecute:: ChipsetIsTxtCapable == TURE\n"));
      ChipsetIsTxtCapable = TRUE;
    }
  }
  
  if (ChipsetIsTxtCapable) {
    *CmosVmxSmxFlag = *CmosVmxSmxFlag | B_SMX_CHIPSET_FLAG;
  }

  TempCmosVmxSmxFlag = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag);
  if (*CmosVmxSmxFlag != TempCmosVmxSmxFlag) {
    WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag, *CmosVmxSmxFlag);
  }

  IA32FeatureCntrl = (UINT32)AsmReadMsr64 ( MSR_IA32_FEATURE_CONTROL );
  if (((SystemConfiguration->VTSupport) && (IsVtSupport)) && (!((IA32FeatureCntrl & B_MSR_IA32_FEATURE_CONTROL_EVT) >> 2)) \
      || ((!((SystemConfiguration->VTSupport) && (IsVtSupport))) & ((IA32FeatureCntrl & B_MSR_IA32_FEATURE_CONTROL_EVT)>> 2)) \
      || ((SystemConfiguration->TXT) && (IsLtSupport) && (ChipsetIsTxtCapable)) && ((IA32FeatureCntrl & B_MSR_IA32_FEATURE_CONTROL_ELT) != B_MSR_IA32_FEATURE_CONTROL_ELT) \
      || (!((SystemConfiguration->TXT) && (IsLtSupport) && (ChipsetIsTxtCapable))) && ((IA32FeatureCntrl & B_MSR_IA32_FEATURE_CONTROL_ELT) == B_MSR_IA32_FEATURE_CONTROL_ELT)) {
    if (((UINT32)AsmReadMsr64 ( MSR_IA32_FEATURE_CONTROL )) & BIT0) {
//[-start-120731-IB10820094-modify]//
      Data32 = IoRead32 (PcdGet16 (PcdPchGpioBaseAddress) + R_PCH_GP_RST_SEL);
//[-end-120731-IB10820094-modify]//
      Data32 |= BIT30;
      IoWrite32((UINT32)(PcdGet16 (PcdPchGpioBaseAddress) + R_PCH_GP_RST_SEL), Data32);
      
//[-start-120807-IB04770241-modify]//
      PchLpcPciCfg32Or (R_PCH_LPC_PMIR, B_PCH_LPC_PMIR_CF9GR);
//[-end-120807-IB04770241-modify]//
      IoWrite8(R_PCH_RST_CNT, V_PCH_RST_CNT_FULLRESET);
      
    } else {
      if ((SystemConfiguration->VTSupport) && (IsVtSupport)) {
        IA32FeatureCntrl |= B_MSR_IA32_FEATURE_CONTROL_EVT;
      } else {
        IA32FeatureCntrl &= ~B_MSR_IA32_FEATURE_CONTROL_EVT;
      }
      
      if ((SystemConfiguration->TXT) && (IsLtSupport) && (ChipsetIsTxtCapable)) {
        IA32FeatureCntrl |= B_MSR_IA32_FEATURE_CONTROL_ELT;
      } else {
        IA32FeatureCntrl &= ~B_MSR_IA32_FEATURE_CONTROL_ELT;
      }
      
      AsmWriteMsr64 ( MSR_IA32_FEATURE_CONTROL, IA32FeatureCntrl );
    }
  }

  return;
}

/**
 Updates Chipset TXT Capability.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in, out]    IsCsTxtCapable      Is Chipset TXT Capable

 @retval            EFI_SUCCESS         Procedure complete.
*/
STATIC
EFI_STATUS
GetCsTxtCapabilities (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN OUT       BOOLEAN               *IsCsTxtCapable
  )
{
  UINT32              Data32 = 0;
  
  //
  // Check PCH TXT capability
  //
  Data32 = MmioRead32(PcdGet32 (PcdTxtPublicBase) + 0x10);
  if (Data32 & BIT0) {
    *IsCsTxtCapable = TRUE;
  } else {
    *IsCsTxtCapable = FALSE;
  }

  return EFI_SUCCESS;
}

/**
 Check on the processor if LT is supported.

 @param             None

 @retval            TRUE                LT is supported
 @retval            FALSE               LT is not supported
*/
STATIC
BOOLEAN
IsSmxSupported (
  VOID
  )
{
  UINT32      RegEcx;
  BOOLEAN     Support;

  //
  // Default is not supported.
  //
  Support = FALSE;

  //
  // Get CPUID to check if the processor supports LaGrande Technology.
  //
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, &RegEcx, NULL);

  if ( ( RegEcx & B_CPUID_VERSION_INFO_ECX_SME ) != 0 ) {
    //
    // LT is supported.
    //
    Support = TRUE;
  }

  return Support;
}
