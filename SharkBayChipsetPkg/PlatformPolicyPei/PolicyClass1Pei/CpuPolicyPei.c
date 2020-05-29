/** @file
  
;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <CpuPolicyPei.h>


//static EFI_PEI_NOTIFY_DESCRIPTOR       mNotifyList[] = {
//  {
//    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
//    &gEfiPeiMemoryDiscoveredPpiGuid,
//    CpuPolicyCallback
//  }
//};

//[-start-121126-IB03780468-modify]//
EFI_STATUS
CpuPolicyCallBack (
  IN  CONST EFI_PEI_SERVICES         **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN  VOID                           *Ppi
  )
//[-end-121126-IB03780468-modify]//
{
  EFI_STATUS                         Status;
  CHIPSET_CONFIGURATION              SystemConfiguration;
//[-start-130710-IB05160465-add]//
  VOID                               *SetupVariable;
//[-end-130710-IB05160465-add]//
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *Variable;
  UINTN                              VariableSize;
  EFI_GUID                           SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_PPI_DESCRIPTOR             *CPUPlatformPolicyPpiDesc;
  PEI_CPU_PLATFORM_POLICY_PPI        *CPUPlatformPolicyPpi;
//[-start-120911-IB05300327-modify]//
//[-start-121207-IB10820186-remove]//
//  UINT8                              PlatformIDString[] = OEM_PFAT_PLATFORM_ID;
//[-end-121207-IB10820186-remove]//
//[-end-120911-IB05300327-modify]//
//[-start-120425-IB05300312-add]//
  VOID                               *Sha256Context;
  VOID                               *Data;
  UINTN                              DataSize;
  UINT8                              HashValue[SHA256_DIGEST_SIZE];
//[-start-120911-IB05300327-modify]//
//[-start-121207-IB10820186-remove]//
//  UINT8                              Pkey0[32] = OEM_PFAT_PUBLIC_KEY_SLOT_0;
//  UINT8                              Pkey1[32] = OEM_PFAT_PUBLIC_KEY_SLOT_1;
//  UINT8                              Pkey2[32] = OEM_PFAT_PUBLIC_KEY_SLOT_2;
//[-end-121207-IB10820186-remove]//
//[-end-120911-IB05300327-modify]//
//[-end-120425-IB05300312-add]//
//[-start-120824-IB05300324-add]//
  UINT8                              i;
  UINT32                             FlashBase;
  UINT32                             FlashAddr;
//[-end-120824-IB05300324-add]//
//[-start-121126-IB03780468-add]//
  WDT_PPI                            *WdtPpi;
  UINT8                              WdtStatus;
//[-end-121126-IB03780468-add]//

  Variable                 = NULL;
  VariableSize             = 0;
  CPUPlatformPolicyPpiDesc = NULL;
  CPUPlatformPolicyPpi     = NULL;

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nCPU Policy PEI Entry\n"));

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  ASSERT_EFI_ERROR (Status);

//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariable = AllocateZeroPool (VariableSize);

  Status = Variable->GetVariable (
             Variable,
             L"Setup",
             &SystemConfigurationGuid,
             NULL,
             &VariableSize,
             SetupVariable
             );
  ASSERT_EFI_ERROR (Status);

  CopyMem (&SystemConfiguration, SetupVariable, sizeof(CHIPSET_CONFIGURATION));
//[-end-130709-IB05160465-modify]//

  //
  // Allocate memory for CPU Policy PPI and Descriptor
  //
  CPUPlatformPolicyPpi = (PEI_CPU_PLATFORM_POLICY_PPI *)AllocateZeroPool (sizeof (PEI_CPU_PLATFORM_POLICY_PPI));
  ASSERT (CPUPlatformPolicyPpi != NULL);
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

//[-start-120627-IB04770237-modify]//
  CPUPlatformPolicyPpi->CpuConfig = (CPU_CONFIG_PPI *)AllocateZeroPool (sizeof (CPU_CONFIG_PPI));
  ASSERT (CPUPlatformPolicyPpi->CpuConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpi->CpuConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  CPUPlatformPolicyPpi->PowerMgmtConfig = (POWER_MGMT_CONFIG_PPI *)AllocateZeroPool (sizeof (POWER_MGMT_CONFIG_PPI));
  ASSERT (CPUPlatformPolicyPpi->PowerMgmtConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpi->PowerMgmtConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  CPUPlatformPolicyPpi->SecurityConfig = (SECURITY_CONFIG_PPI *)AllocateZeroPool (sizeof (SECURITY_CONFIG_PPI));
//[-end-120627-IB04770237-modify]//
  ASSERT (CPUPlatformPolicyPpi->SecurityConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpi->SecurityConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig = (PFAT_CONFIG *)AllocateZeroPool (sizeof (PFAT_CONFIG));
  ASSERT (CPUPlatformPolicyPpi->SecurityConfig->PfatConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpi->SecurityConfig->PfatConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  CPUPlatformPolicyPpi->SecurityConfig->TxtConfig = (TXT_CONFIG *)AllocateZeroPool (sizeof (TXT_CONFIG));
  ASSERT (CPUPlatformPolicyPpi->SecurityConfig->TxtConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpi->SecurityConfig->TxtConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  
//[-start-121113-IB06150258-add]//
  CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig = ( BOOT_GUARD_CONFIG * )AllocateZeroPool ( sizeof ( BOOT_GUARD_CONFIG ) );
  ASSERT ( CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig != NULL );
//[-end-121113-IB06150258-add]//
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

//[-start-120627-IB04770237-modify]//
  CPUPlatformPolicyPpi->OverclockingConfig = (OVERCLOCKING_CONFIG_PPI *)AllocateZeroPool (sizeof (OVERCLOCKING_CONFIG_PPI));
//[-end-120627-IB04770237-modify]//
  ASSERT (CPUPlatformPolicyPpi->OverclockingConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpi->OverclockingConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  CPUPlatformPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (CPUPlatformPolicyPpiDesc != NULL);
//[-start-130207-IB10870073-add]//
  if (CPUPlatformPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

//[-start-121211-IB06460477-modify]//
  //
  // Update REVISION number
  //
  CPUPlatformPolicyPpi->Revision                                             = PEI_CPU_PLATFORM_POLICY_PPI_REVISION_8;
//[-end-120403-IB05300305-modify]//
//[-end-120510-IB06460396-modify]//
//[-end-120627-IB04770237-modify]//

  //
  // Initialize CPU Configuration
  //
//[-start-120522-IB06460400-modify]//
  CPUPlatformPolicyPpi->CpuConfig->BistOnReset                               = SystemConfiguration.BistOnReset;
  CPUPlatformPolicyPpi->CpuConfig->HyperThreading                            = SystemConfiguration.HTSupport;
//[-start-121127-IB03780468-modify]// 
  CPUPlatformPolicyPpi->CpuConfig->CpuRatioOverride                          = SystemConfiguration.FlexRatioOverride;
//[-end-121127-IB03780468-modify]//
  CPUPlatformPolicyPpi->CpuConfig->VmxEnable                                 = SystemConfiguration.VTSupport;
//[-start-120413-IB05300312-modify]//
  if (FeaturePcdGet (PcdPfatSupport)) {
    CPUPlatformPolicyPpi->CpuConfig->Pfat                                      = SystemConfiguration.Pfat;
  } else {
    CPUPlatformPolicyPpi->CpuConfig->Pfat                                      = CPU_FEATURE_DISABLE;
  }
//[-end-120413-IB05300312-modify]//
  CPUPlatformPolicyPpi->CpuConfig->MlcStreamerPrefetcher                     = SystemConfiguration.HardwarePrefetcher;
  CPUPlatformPolicyPpi->CpuConfig->MlcSpatialPrefetcher                      = SystemConfiguration.HardwarePrefetcher;
//[-start-130906-IB08620308-add]//
  CPUPlatformPolicyPpi->CpuConfig->DcuStreamerPrefetcher                     = CPU_FEATURE_ENABLE;
  CPUPlatformPolicyPpi->CpuConfig->DcuIPPrefetcher                           = CPU_FEATURE_ENABLE;
//[-end-130906-IB08620308-add]//
  if (FeaturePcdGet (PcdTXTSupported)) {
    CPUPlatformPolicyPpi->CpuConfig->Txt                                     = SystemConfiguration.TXT;
  } else {
    CPUPlatformPolicyPpi->CpuConfig->Txt                                     = FeaturePcdGet (PcdTXTSupported);
  }
  CPUPlatformPolicyPpi->CpuConfig->ActiveCoreCount                           = SystemConfiguration.CoreActive;
//[-start-120405-IB03780430-modify]//
//[-start-121026-IB06460460-modify]//
//[-start-120705-IB05330352-modify]//
//  if ( FeaturePcdGet ( PcdUltFlag )) {
//    CPUPlatformPolicyPpi->CpuConfig->CpuRatio                                = PcdGet8 ( PcdCpuConfigCpuRatio );
//  } else {
    CPUPlatformPolicyPpi->CpuConfig->CpuRatio                                = SystemConfiguration.FlexRatio;
//  }
//[-end-120405-IB03780430-modify]//
//[-end-121026-IB06460460-modify]//
  CPUPlatformPolicyPpi->CpuConfig->CpuMaxNonTurboRatio                       = PcdGet8 (PcdCpuConfigCpuMaxNonTurboRatio);
  //
  // Initialize Power Management Configuration
  //
  CPUPlatformPolicyPpi->PowerMgmtConfig->BootInLfm                           = SystemConfiguration.BootInLfm;
  CPUPlatformPolicyPpi->PowerMgmtConfig->TccActivationOffset                 = PcdGet8 (PcdPowerMgmtConfigTccActivationOffset);
  CPUPlatformPolicyPpi->PowerMgmtConfig->VrCurrentLimit                      = PcdGet16 (PcdPowerMgmtConfigVrCurrentLimit);
  CPUPlatformPolicyPpi->PowerMgmtConfig->VrCurrentLimitLock                  = PcdGet8 (PcdPowerMgmtConfigVrCurrentLimitLock);
//[-start-121126-IB03780468-remove]//
//  CPUPlatformPolicyPpi->PowerMgmtConfig->Xe                                  = PcdGet8 (PcdPowerMgmtConfigXe);
//  CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[0]                       = PcdGet8 (PcdPowerMgmtConfigRatioLimit0);
//  CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[1]                       = PcdGet8 (PcdPowerMgmtConfigRatioLimit1);
//  CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[2]                       = PcdGet8 (PcdPowerMgmtConfigRatioLimit2);
//  CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[3]                       = PcdGet8 (PcdPowerMgmtConfigRatioLimit3);
//[-end-121126-IB03780468-remove]//
//[-start-120510-IB06460396-add]//
//[-start-120806-IB04770240-add]//
  ///
  /// VrMiscIoutSlope      = 0x200 default
  /// VrMiscIoutOffsetSign = 0 means it's positive offset. 1= negative offset
  /// VrMiscIoutOffset     = 0 means it's 0%, 625 means 6.25% (range is +6.25% ~ -6.25%)
  ///
  CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIoutSlope                                           = PcdGet16 (PcdPowerMgmtConfigVrMiscIoutSlope);
  CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIoutOffsetSign                                      = PcdGet8 (PcdPowerMgmtConfigVrMiscIoutOffsetSign);
  CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIoutOffset                                          = PcdGet16 (PcdPowerMgmtConfigVrMiscIoutOffset);
//[-end-120806-IB04770240-add]//

  CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscMinVid                        = V_MSR_VR_MISC_CONFIG_MIN_VID_DEFAULT;
  CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIdleExitRampRate              = PcdGet8 (PcdPowerMgmtConfigVrMiscIdleExitRampRate);
  CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIdleEntryRampRate             = PcdGet8 (PcdPowerMgmtConfigVrMiscIdleEntryRampRate);
  CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIdleEntryDecayEnable          = PcdGet8 (PcdPowerMgmtConfigVrMiscIdleEntryDecayEnable);
  if (GetCpuFamily() == EnumCpuHswUlt) {
    CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscSlowSlewRateConfig          = V_MSR_VR_MISC_CONFIG_SLOW_SLEW_RATE_CONFIG_DEFAULT;
    CPUPlatformPolicyPpi->PowerMgmtConfig->VrMisc2FastRampVoltage            = V_MSR_VR_MISC_CONFIG2_FAST_RAMP_VOLTAGE_DEFAULT;
    CPUPlatformPolicyPpi->PowerMgmtConfig->VrMisc2MinC8Voltage               = V_MSR_VR_MISC_CONFIG2_MIN_C8_VOLTAGE_DEFAULT;
//[-start-120904-IB03780459-add]//
    CPUPlatformPolicyPpi->PowerMgmtConfig->VrPSI4enable                      = PcdGet8 (PcdPowerMgmtConfigVrPSI4enable);
//[-end-120904-IB03780459-add]//
  }  
//[-end-120510-IB06460396-add]//

//[-start-120917-IB03780459-add]//
  CPUPlatformPolicyPpi->PowerMgmtConfig->Psi1Threshold                       = PSI1_THRESHOLD_DEFAULT;
  CPUPlatformPolicyPpi->PowerMgmtConfig->Psi2Threshold                       = PSI2_THRESHOLD_DEFAULT;
  CPUPlatformPolicyPpi->PowerMgmtConfig->Psi3Threshold                       = PSI3_THRESHOLD_DEFAULT;
//[-end-120917-IB03780459-add]//

//[-start-121022-IB03780465-add]//
  CPUPlatformPolicyPpi->PowerMgmtConfig->FivrSscEnable                       = PcdGet8 (PcdPowerMgmtConfigFivrSscEnable);
  CPUPlatformPolicyPpi->PowerMgmtConfig->FivrSscPercent                      = PcdGet8 (PcdPowerMgmtConfigFivrSscPercent);
//[-end-121022-IB03780465-add]//

//[-start-120403-IB05300305-modify]//
  //
  // Initialize PFAT Configuration
  //
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PpdtMajVer          = PPDT_MAJOR_VERSION;
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PpdtMinVer          = PPDT_MINOR_VERSION;
//[-start-121207-IB10820186-modify]//
  CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PlatId[0], (UINT8 *)PcdGetPtr (PcdOemPfatPlatformId), LibPcdGetSize (PcdToken (PcdOemPfatPlatformId)));
//[-end-121207-IB10820186-modify]//
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PfatModSvn          = PFAT_SVN;
//[-start-120917-IB03780459-modify]//
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.BiosSvn             = PcdGet32 (PcdSecurityConfigPfatConfigPpdtBiosSvn);
//[-end-120917-IB03780459-modify]//
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.ExecLim             = PcdGet32 (PcdSecurityConfigPfatConfigPpdtExecLim);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PlatAttr            = PcdGet32 (PcdSecurityConfigPfatConfigPpdtPlatAttr);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.LastSfam            = MIN_SFAM_COUNT - 1;
  if (CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.LastSfam > (MAX_SFAM_COUNT - 1)) {
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.LastSfam          = MAX_SFAM_COUNT - 1;
  }
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[0].FirstByte  = PcdGet32 (PcdSecurityConfigPfatConfigPpdtSfamData0FirstByte);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[0].LastByte   = PcdGet32 (PcdSecurityConfigPfatConfigPpdtSfamData0LastByte);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PpdtSize            = (sizeof (PPDT) - sizeof (CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData) + ((CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.LastSfam + 1) * sizeof (SFAM_DATA)));;
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[0]              = PcdGet64 (PcdSecurityConfigPfatConfigPpdtPpdtHash0);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[1]              = PcdGet64 (PcdSecurityConfigPfatConfigPpdtPpdtHash1);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[2]              = PcdGet64 (PcdSecurityConfigPfatConfigPpdtPpdtHash2);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[3]              = PcdGet64 (PcdSecurityConfigPfatConfigPpdtPpdtHash3);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.Version        = PUP_HDR_VERSION;
//[-start-121207-IB10820186-modify]//
  CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.PlatId[0], (UINT8 *)PcdGetPtr (PcdOemPfatPlatformId), LibPcdGetSize (PcdToken (PcdOemPfatPlatformId)));
//[-end-121207-IB10820186-modify]//
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.PkgAttributes  = PcdGet16 (PcdSecurityConfigPfatConfigPupHeaderPkgAttributes);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.PslMajorVer    = PSL_MAJOR_VERSION;
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.PslMinorVer    = PSL_MINOR_VERSION;
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.BiosSvn        = CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.BiosSvn;
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.EcSvn          = PcdGet32 (PcdSecurityConfigPfatConfigPupHeaderEcSvn);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.VendorSpecific = PcdGet32 (PcdSecurityConfigPfatConfigPupHeaderVendorSpecific);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatLog.Version          = PFAT_LOG_VERSION;
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatLog.LastPage         = PcdGet16 (PcdSecurityConfigPfatConfigPfatLogLastPage);
  if (CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatLog.LastPage > (MAX_PFAT_LOG_PAGE - 1)) {
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatLog.LastPage       = MAX_PFAT_LOG_PAGE - 1;
  }
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatLog.LoggingOptions   = PcdGet32 (PcdSecurityConfigPfatConfigPfatLoggingOptions);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->NumSpiComponents         = PcdGet8 (PcdSecurityConfigPfatConfigPfatNumSpiComponents);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->ComponentSize[0]         = PcdGet8 (PcdSecurityConfigPfatConfigPfatComponentSize0);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->ComponentSize[1]         = PcdGet8 (PcdSecurityConfigPfatConfigPfatComponentSize1);
  CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatMemSize              = PcdGet8 (PcdSecurityConfigPfatConfigPfatPfatMemSize);
//[-end-120403-IB05300305-modify]//
//[-end-120522-IB06460400-modify]//

//[-start-120827-IB05300324-modify]//
//[-start-120425-IB05300312-add]//
  if (SystemConfiguration.Pfat) {
    //
    // Select to Flash Map 0 Register to get the number of flash Component
    //
    PchMmRcrb32AndThenOr (
      R_PCH_SPI_FDOC,
      (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
      (UINT32) (V_PCH_SPI_FDOC_FDSS_FSDM | R_PCH_SPI_FDBAR_FLASH_MAP0)
    );
    //
    // Copy Zero based Number Of Components
    //
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->NumSpiComponents = (UINT8) (((PchMmRcrb16 (R_PCH_SPI_FDOD) & B_PCH_SPI_FDBAR_NC) >> N_PCH_SPI_FDBAR_NC) + 1);
    //
    // Select to Flash Components Register to get Components Density
    //
    PchMmRcrb32AndThenOr (
      R_PCH_SPI_FDOC,
      (UINT32) (~(B_PCH_SPI_FDOC_FDSS_MASK | B_PCH_SPI_FDOC_FDSI_MASK)),
      (UINT32) (V_PCH_SPI_FDOC_FDSS_COMP | R_PCH_SPI_FCBA_FLCOMP)
    );
    //
    // Copy Components Density
    //
    FlashAddr = (UINT8) PchMmRcrb32 (R_PCH_SPI_FDOD);
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->ComponentSize[0] = (UINT8) (FlashAddr & B_PCH_SPI_FLCOMP_COMP1_MASK);
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->ComponentSize[1] = (UINT8) ((FlashAddr & B_PCH_SPI_FLCOMP_COMP2_MASK) >> 4);
    FlashAddr = 0;
    for (i = 0; i < CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->NumSpiComponents; i++) {
      FlashAddr += (SPI_SIZE_BASE_512KB << CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->ComponentSize[i]);
    }
//[-start-121222-IB10820209-modify]//
//[-start-130322-IB05160422-modify]//
    FlashBase = FlashAddr - PcdGet32 (PcdFlashAreaSize);
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.LastSfam              = 2;
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[0].FirstByte = FlashBase + (PcdGet32 (PcdFlashFvMainBase));
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[0].LastByte  = FlashBase + (PcdGet32 (PcdFlashFvMainBase)) + PcdGet32 (PcdFlashFvMainSize) - 1;
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[1].FirstByte = FlashBase + (PcdGet32 (PcdFlashNvStorageVariableBase));
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[1].LastByte  = FlashBase + (PcdGet32 (PcdFlashNvStorageVariableBase)) + (PcdGet32 (PcdFlashNvStorageVariableSize)) - 1;
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[2].FirstByte = FlashBase + (PcdGet32 (PcdFlashFvRecoveryBase));
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[2].LastByte  = FlashBase + (PcdGet32 (PcdFlashFvRecoveryBase)) + PcdGet32 (PcdFlashFvRecoverySize) - 1;
//[-end-130322-IB05160422-modify]//
//[-end-121222-IB10820209-remove]//
//[-start-121207-IB10820186-modify]//
    CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PkeySlot0[0], (UINT8 *)PcdGetPtr (PcdOemPfatPublicKeySlot0), 32);
    CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PkeySlot1[0], (UINT8 *)PcdGetPtr (PcdOemPfatPublicKeySlot0), 32);
    CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PkeySlot2[0], (UINT8 *)PcdGetPtr (PcdOemPfatPublicKeySlot0), 32);
//[-end-121207-IB10820186-modify]//
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatLog.LastPage       = MAX_PFAT_LOG_PAGE - 1;
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatLog.LoggingOptions = PFAT_LOG_OPT_DEBUG | PFAT_LOG_OPT_FLASH_ERROR | PFAT_LOG_OPT_FLASH_ERASE | PFAT_LOG_OPT_FLASH_WRITE | PFAT_LOG_OPT_BRANCH_TRACE | PFAT_LOG_OPT_STEP_TRACE;

//[-start-121017-IB05300352-modify]//
    if (FeaturePcdGet (PcdPfatEcFlag)) {
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmd             = PcdGet32 (PcdSecurityConfigPfatConfigPpdtEcCmd);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcData            = PcdGet32 (PcdSecurityConfigPfatConfigPpdtEcData);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->EcCmdDiscovery         = PcdGet8 (PcdSecurityConfigPfatConfigEcCmdDiscovery);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->EcCmdProvisionEav      = PcdGet8 (PcdSecurityConfigPfatConfigEcCmdProvisionEav);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->EcCmdLock              = PcdGet8 (PcdSecurityConfigPfatConfigEcCmdLock);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmdGetSvn       = PcdGet32 (PcdSecurityConfigPfatConfigPpdtEcCmdGetSvn);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmdOpen         = PcdGet32 (PcdSecurityConfigPfatConfigPpdtEcCmdOpen);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmdClose        = PcdGet32 (PcdSecurityConfigPfatConfigPpdtEcCmdClose);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmdPortTest     = PcdGet32 (PcdSecurityConfigPfatConfigPpdtEcCmdPortTest);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PlatAttr         |= PcdGet32 (PcdSecurityConfigPfatConfigPpdtPlatAttr);
      CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.EcSvn        = PcdGet32 (PcdSecurityConfigPfatConfigPupHeaderEcSvn);
    }
//[-end-121017-IB05300352-modify]//
    CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PpdtSize          = (sizeof (PPDT) - sizeof (CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData) + ((CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.LastSfam + 1) * sizeof (SFAM_DATA)));;

    DataSize  = Sha256GetContextSize ();
    Status    = ((*PeiServices)->AllocatePool) (PeiServices, DataSize, &Sha256Context);
    ASSERT_EFI_ERROR (Status);
    DataSize = CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PpdtSize;
    Data = (VOID *) &CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt;
    Sha256Init (Sha256Context);
    Sha256Update (Sha256Context, Data, DataSize);
    Sha256Final (Sha256Context, HashValue);
    CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[0], &HashValue[0], 8);
    CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[1], &HashValue[8], 8);
    CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[2], &HashValue[16], 8);
    CopyMem (&CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[3], &HashValue[24], 8);
  }
//[-end-120425-IB05300312-add]//
//[-end-120827-IB05300324-modify]//
  //
  // Initialize TXT Configuration
  //
  if (FeaturePcdGet (PcdTXTSupported)) {
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->SinitMemorySize           = TXT_SINIT_MEMORY_SIZE;
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtHeapMemorySize         = TXT_HEAP_MEMORY_SIZE;
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtDprMemoryBase          = PcdGet64 (PcdSecurityConfigTxtConfigTxtDprMemoryBase);
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtDprMemorySize          = TXT_TOTAL_STOLEN_MEMORY_SIZE;
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->BiosAcmBase               = PcdGet64 (PcdSecurityConfigTxtConfigTxtBiosAcmBase);
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->BiosAcmSize               = PcdGet64 (PcdSecurityConfigTxtConfigTxtBiosAcmSize);
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->McuUpdateDataAddr         = PcdGet32 (PcdFlashNvStorageMicrocodeBase);
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TgaSize                   = TXT_TGA_MEMORY_SIZE;
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtLcpPdBase              = TXT_LCP_PD_BASE;
    CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtLcpPdSize              = TXT_LCP_PD_SIZE;
  }
//[-start-121113-IB06150258-add]//
  //
  // Initialize AnC data
  //
  CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->TpmType                   = TpmTypeMax;
  CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->BypassTpmInit             = FALSE;
  CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->MeasuredBoot              = FALSE;
//[-end-121113-IB06150258-add]//
//[-start-121211-IB06460477-add]//
  CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->BootGuardSupport          = PcdGetBool (PcdAncConfigAncSupport);
  CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->DisconnectAllTpms         = PcdGetBool (PcdAncConfigDisconnectAllTpms);
//[-end-121211-IB06460477-add]//

//[-start-121031-IB03780468-modify]//
  //
  // Use PCH WDT to avoid system hung always when incorrect settings are applied for Overclocking. 
  //
  WdtStatus = V_PCH_OC_WDT_CTL_STATUS_FAILURE;
  if (SystemConfiguration.Overclocking == 1) {
    Status = PeiServicesLocatePpi (
                               &gWdtPpiGuid,
                               0,
                               NULL,
                               (VOID **)&WdtPpi
                               );
    ASSERT_EFI_ERROR (Status);

    WdtStatus = WdtPpi->CheckStatus ();
  }

  //
  // Initialize Overclocking Configuration for Processor Ratio, IA Core, CLR, and SVID/FIVR.
  //
  if (WdtStatus == V_PCH_OC_WDT_CTL_STATUS_OK) {
    //
    // Apply Overclocking Configuration only when system boots correctly in previous POST.
    //
    CPUPlatformPolicyPpi->OverclockingConfig->OcSupport               = 1;

    //
    // Update Programmable Ratio Limits for Turbo Mode
    //
    if (SystemConfiguration.OcCapXe == 1 && SystemConfiguration.ExtremeEdition == 1) {
      CPUPlatformPolicyPpi->PowerMgmtConfig->Xe                       = SystemConfiguration.ExtremeEdition;
      CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[0]            = SystemConfiguration.RatioLimit1Core;
      CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[1]            = SystemConfiguration.RatioLimit2Core;
      CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[2]            = SystemConfiguration.RatioLimit3Core;
      CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[3]            = SystemConfiguration.RatioLimit4Core;
    }

    //
    // Update OC settings for IA Core.
    //
    if (SystemConfiguration.OcCapIaCore == 1) {
      CPUPlatformPolicyPpi->OverclockingConfig->CoreVoltageOffset     = (SystemConfiguration.CoreVoltageOffsetPrefix == 0) ? SystemConfiguration.CoreVoltageOffset : SystemConfiguration.CoreVoltageOffset | BIT15;
      CPUPlatformPolicyPpi->OverclockingConfig->CoreVoltageOverride   = SystemConfiguration.CoreVoltageOverride;
      CPUPlatformPolicyPpi->OverclockingConfig->CoreExtraTurboVoltage = SystemConfiguration.CoreExtraTurboVoltage;
      CPUPlatformPolicyPpi->OverclockingConfig->CoreMaxOcTurboRatio   = SystemConfiguration.CoreMaxOcRatio;
      CPUPlatformPolicyPpi->OverclockingConfig->CoreVoltageMode       = SystemConfiguration.CoreVoltageMode;
    }

    //
    // Update OC settings for CLR.
    //
    if (SystemConfiguration.OcCapIaCore == 1) {
      CPUPlatformPolicyPpi->OverclockingConfig->ClrVoltageOffset      = (SystemConfiguration.ClrVoltageOffsetPrefix == 0) ? SystemConfiguration.ClrVoltageOffset : SystemConfiguration.ClrVoltageOffset | BIT15;
      CPUPlatformPolicyPpi->OverclockingConfig->ClrVoltageOverride    = SystemConfiguration.ClrVoltageOverride;
      CPUPlatformPolicyPpi->OverclockingConfig->ClrExtraTurboVoltage  = SystemConfiguration.ClrExtraTurboVoltage;
      CPUPlatformPolicyPpi->OverclockingConfig->ClrMaxOcTurboRatio    = SystemConfiguration.ClrMaxOcRatio;
      CPUPlatformPolicyPpi->OverclockingConfig->ClrVoltageMode        = SystemConfiguration.ClrVoltageMode;
    }

    //
    // Update OC settings for SVID/FIVR.
    //
    CPUPlatformPolicyPpi->OverclockingConfig->SvidVoltageOverride     = SystemConfiguration.SvidVoltageOverride;
    CPUPlatformPolicyPpi->OverclockingConfig->SvidEnable              = SystemConfiguration.SvidSupport;
    CPUPlatformPolicyPpi->OverclockingConfig->FivrFaultsEnable        = SystemConfiguration.FivrFaults;
    CPUPlatformPolicyPpi->OverclockingConfig->FivrEfficiencyEnable    = SystemConfiguration.FivrEfficiencyManagement;
  }
//[-end-121031-IB03780468-modify]//
//[-start-121211-IB06460477-add]//
  CPUPlatformPolicyPpi->CpuPlatformPpiPtr           = (UINTN) CPUPlatformPolicyPpi;
//[-end-121211-IB06460477-add]//

  //
  // Update CPU Policy PPI Descriptor
  //
  CPUPlatformPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  CPUPlatformPolicyPpiDesc->Guid  = &gPeiCpuPlatformPolicyPpiGuid;
  CPUPlatformPolicyPpiDesc->Ppi   = CPUPlatformPolicyPpi;

//[-start-121226-IB10870063-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdatePeiCpuPlatformPolicy (CPUPlatformPolicyPpi, CPUPlatformPolicyPpiDesc);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcUpdatePeiCpuPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Installing CPU Policy ( PEI ).\n"));
    //
    // Install CPU PEI Platform Policy PPI
    //
    Status = PeiServicesInstallPpi (CPUPlatformPolicyPpiDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121226-IB10870063-modify]//

  //
  // Register NotifyList to produce ACPI S3 hob.
  //
//  Status = ( **PeiServices ).NotifyPpi ( PeiServices, &mNotifyList[0] );
  ASSERT_EFI_ERROR (Status);

  //
  // Dump policy
  //
  DumpCPUPolicyPEI (PeiServices, CPUPlatformPolicyPpi);

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "CPU Policy PEI Exit\n"));

  return Status;
}

VOID
DumpCPUPolicyPEI (
  IN      CONST EFI_PEI_SERVICES             **PeiServices,
  IN      PEI_CPU_PLATFORM_POLICY_PPI        *CPUPlatformPolicyPpi
  )
{
  UINTN        Count;

  Count = 0;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "CPUPlatformPolicyPpi ( Address : 0x%x )\n", CPUPlatformPolicyPpi));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Revision                         : %x\n", CPUPlatformPolicyPpi->Revision));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-CpuConfig ( Address : 0x%x )\n", CPUPlatformPolicyPpi->CpuConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-BistOnReset                   : %x\n", CPUPlatformPolicyPpi->CpuConfig->BistOnReset));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HyperThreading                : %x\n", CPUPlatformPolicyPpi->CpuConfig->HyperThreading));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CpuRatioOverride              : %x\n", CPUPlatformPolicyPpi->CpuConfig->CpuRatioOverride));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VmxEnable                     : %x\n", CPUPlatformPolicyPpi->CpuConfig->VmxEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Pfat                          : %x\n", CPUPlatformPolicyPpi->CpuConfig->Pfat));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MlcStreamerPrefetcher         : %x\n", CPUPlatformPolicyPpi->CpuConfig->MlcStreamerPrefetcher));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MlcSpatialPrefetcher          : %x\n", CPUPlatformPolicyPpi->CpuConfig->MlcSpatialPrefetcher));
//[-start-121213-IB08620144-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DcuStreamerPrefetcher         : %x\n", CPUPlatformPolicyPpi->CpuConfig->DcuStreamerPrefetcher ) );
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DcuIPPrefetcher               : %x\n", CPUPlatformPolicyPpi->CpuConfig->DcuIPPrefetcher ) );
//[-end-121213-IB08620144-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Txt                           : %x\n", CPUPlatformPolicyPpi->CpuConfig->Txt));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbyte                      : %x\n", CPUPlatformPolicyPpi->CpuConfig->Rsvdbyte));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ActiveCoreCount               : %x\n", CPUPlatformPolicyPpi->CpuConfig->ActiveCoreCount));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CpuRatio                      : %x\n", CPUPlatformPolicyPpi->CpuConfig->CpuRatio));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CpuMaxNonTurboRatio           : %x\n", CPUPlatformPolicyPpi->CpuConfig->CpuMaxNonTurboRatio));

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-PowerMgmtConfig ( Address : 0x%x )\n", CPUPlatformPolicyPpi->PowerMgmtConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-BootInLfm                     : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->BootInLfm));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-TccActivationOffset           : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->TccActivationOffset));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrCurrentLimit                : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrCurrentLimit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrCurrentLimitLock            : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrCurrentLimitLock));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Xe                            : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->Xe));
  for (Count = 0; Count < 4; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RatioLimit[%2x]                : %x\n", Count, CPUPlatformPolicyPpi->PowerMgmtConfig->RatioLimit[Count]));
  }
//[-start-120806-IB04770240-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMiscMinVid                  : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscMinVid));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMiscIdleExitRampRate        : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIdleExitRampRate));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMiscIdleEntryRampRate       : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIdleEntryRampRate));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMiscIdleEntryDecayEnable    : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIdleEntryDecayEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMiscSlowSlewRateConfig      : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscSlowSlewRateConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMisc2FastRampVoltage        : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMisc2FastRampVoltage));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMisc2MinC8Voltage           : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMisc2MinC8Voltage));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMiscIoutSlope               : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIoutSlope));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMiscIoutOffsetSign          : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIoutOffsetSign));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrMiscIoutOffset              : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrMiscIoutOffset));
//[-end-120806-IB04770240-add]//
//[-start-120917-IB03780459-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VrPSI4enable                  : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->VrPSI4enable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Psi1Threshold                 : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->Psi1Threshold));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Psi2Threshold                 : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->Psi2Threshold));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Psi3Threshold                 : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->Psi3Threshold));  
//[-end-120917-IB03780459-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FivrSscEnable                 : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->FivrSscEnable ) );
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FivrSscPercent                : %x\n", CPUPlatformPolicyPpi->PowerMgmtConfig->FivrSscPercent ) );

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-SecurityConfig ( Address : 0x%x )\n", CPUPlatformPolicyPpi->SecurityConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PfatConfig ( Address : 0x%x )\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PupHeader ( Address : 0x%x )\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Version                 : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.Version));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Reserved3[0]            : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.Reserved3[0]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Reserved3[1]            : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.Reserved3[1]));
  for (Count = 0; Count < 16; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PlatId[%2x]              : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.PlatId[Count]));
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PkgAttributes           : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.PkgAttributes));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Reserved4[0]            : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.Reserved4[0]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Reserved4[1]            : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.Reserved4[1]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PslMajorVer             : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.PslMajorVer));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PslMinorVer             : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.PslMinorVer));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-ScriptSectionSize       : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.ScriptSectionSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-DataSectionSize         : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.DataSectionSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-BiosSvn                 : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.BiosSvn));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-EcSvn                   : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.EcSvn));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-VendorSpecific          : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PupHeader.VendorSpecific));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Ppdt      ( Address : 0x%x )\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PpdtSize                : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PpdtSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PpdtMajVer              : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PpdtMajVer));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PpdtMinVer              : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PpdtMinVer));
  for (Count = 0; Count < 16; Count++) {  
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PlatId[%2x]              : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PlatId[Count]));
  }  
  for (Count = 0; Count < 32; Count++) {  
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PkeySlot0[%2x]           : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PkeySlot0[Count]));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PkeySlot1[%2x]           : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PkeySlot1[Count]));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PkeySlot2[%2x]           : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PkeySlot2[Count]));
  }  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PfatModSvn              : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PfatModSvn));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-BiosSvn                 : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.BiosSvn));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-ExecLim                 : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.ExecLim));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PlatAttr                : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.PlatAttr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-EcCmd                   : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmd));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-EcData                  : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcData));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-EcCmdGetSvn             : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmdGetSvn));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-EcCmdOpen               : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmdOpen));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-EcCmdClose              : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.EcCmdClose));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-LastSfam                : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.LastSfam));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Reserved2[0]             : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.Reserved2[0]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Reserved2[1]             : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.Reserved2[1]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Reserved2[2]             : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.Reserved2[2]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-SfamData[0] ( Address : 0x%x )\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |  |-FirstByte            : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[0].FirstByte));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |  |-LastByte             : %x\n", CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->Ppdt.SfamData[0].LastByte));
  for (Count = 0; Count < 4; Count++) {  
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PpdtHash[%2x]               : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PpdtHash[Count]));
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-NumSpiComponents           : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->NumSpiComponents));
  for (Count = 0; Count < MAX_SPI_COMPONENTS; Count++) {  
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ComponentSize[%2x]          : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->ComponentSize[Count]));
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PfatMemSize                : %x\n", Count, CPUPlatformPolicyPpi->SecurityConfig->PfatConfig->PfatMemSize));
  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-TxtConfig ( Address : 0x%x )\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig ) );
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SinitMemorySize            : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->SinitMemorySize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TxtHeapMemorySize          : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtHeapMemorySize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TxtDprMemoryBase           : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtDprMemoryBase));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TxtDprMemorySize           : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtDprMemorySize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-BiosAcmBase                : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->BiosAcmBase));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-BiosAcmSize                : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->BiosAcmSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-McuUpdateDataAddr          : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->McuUpdateDataAddr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TgaSize                    : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TgaSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TxtLcpPdBase               : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtLcpPdBase));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TxtLcpPdSize               : %x\n", CPUPlatformPolicyPpi->SecurityConfig->TxtConfig->TxtLcpPdSize));

//[-start-121113-IB06150258-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-BootGuardConfig ( Address : 0x%x )\n", CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-MeasuredBoot               : %x\n", CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->MeasuredBoot ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-BypassTpmInit              : %x\n", CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->BypassTpmInit ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-TpmType                    : %x\n", CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->TpmType ) );
//[-end-121113-IB06150258-add]//
//[-start-130201-IB05280045-modify]//
//[-start-121211-IB06460477-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-AncSupport                 : %x\n", CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->BootGuardSupport));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-DisconnectAllTpms          : %x\n", CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->DisconnectAllTpms));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ByPassTpmEventLog          : %x\n", CPUPlatformPolicyPpi->SecurityConfig->BootGuardConfig->ByPassTpmEventLog ) );
//[-end-121211-IB06460477-add]//
//[-end-130201-IB05280045-modify]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-OverclockingConfig ( Address : 0x%x )\n", CPUPlatformPolicyPpi->OverclockingConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CoreVoltageOffset             : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->CoreVoltageOffset));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CoreVoltageOverride           : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->CoreVoltageOverride));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CoreExtraTurboVoltage         : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->CoreExtraTurboVoltage));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CoreMaxOcTurboRatio           : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->CoreMaxOcTurboRatio));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ClrVoltageOffset              : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->ClrVoltageOffset));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ClrVoltageOverride            : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->ClrVoltageOverride));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ClrExtraTurboVoltage          : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->ClrExtraTurboVoltage));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ClrMaxOcTurboRatio            : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->ClrMaxOcTurboRatio));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SvidVoltageOverride           : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->SvidVoltageOverride));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SvidEnable                    : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->SvidEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FivrFaultsEnable              : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->FivrFaultsEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FivrEfficiencyEnable          : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->FivrEfficiencyEnable));
//[-start-120627-IB04770237-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CoreVoltageMode               : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->CoreVoltageMode));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ClrVoltageMode                : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->ClrVoltageMode));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-OcSupport                     : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->OcSupport));
//[-end-120627-IB04770237-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-BitReserved                   : %x\n", CPUPlatformPolicyPpi->OverclockingConfig->BitReserved));
//[-start-130201-IB05280045-modify]//
//[-start-121211-IB06460477-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-CpuPlatformPpiPtr                : %x\n", CPUPlatformPolicyPpi->CpuPlatformPpiPtr));
//[-end-121211-IB06460477-add]//
//[-end-130201-IB05280045-modify]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));

  return;
}

/**
 Provide a callback when the MemoryDiscovered PPI is installed.

 @param             PeiServices         The PEI core services table.
                    NotifyDescriptor    The descriptor for the notification event.
                    Ppi                 Pointer to the PPI in question.

 @retval            EFI_SUCCESS         The function is successfully processed.
*/
EFI_STATUS
CpuPolicyCallback (
  IN  CONST EFI_PEI_SERVICES          **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  )
{
  EFI_BOOT_MODE                       BootMode;
  EFI_STATUS                          Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *Variable;
  ACPI_VARIABLE_SET                   *AcpiVariableSet;
  UINT64                              AcpiVariableSet64;
  UINTN                               VarSize;

  //
  // Produce ACPI S3 hob when boot path is S3.
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    //
    // Initial PEI Variable service.
    //
    Status = PeiServicesLocatePpi (
               &gEfiPeiReadOnlyVariable2PpiGuid, 
               0,
               NULL,
               (VOID **)&Variable
               );
    ASSERT_EFI_ERROR (Status);
    
    AcpiVariableSet   = NULL;
    VarSize           = sizeof (AcpiVariableSet64);
    
    Status = Variable->GetVariable (
                         Variable,
                         ACPI_GLOBAL_VARIABLE,
                         &gEfiAcpiVariableGuid,
                         NULL,
                         &VarSize,
                         &AcpiVariableSet64
                         );
    ASSERT_EFI_ERROR (Status);

    //
    // Build ACPI_VARIABLE_SET hob here for S3Resume PEIM. MRC will not produce this hob after SA RC 0.5.1.
    //
    AcpiVariableSet = (ACPI_VARIABLE_SET *) (UINTN) AcpiVariableSet64;

    BuildGuidDataHob (
      &gEfiAcpiVariableGuid,
      AcpiVariableSet,
      sizeof (ACPI_VARIABLE_SET)
      );
  }
    
  return EFI_SUCCESS;
}

