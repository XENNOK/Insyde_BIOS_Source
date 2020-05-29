/** @file
  This file is SampleCode for Intel SA PEI Platform Policy initialzation.

;******************************************************************************
;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

**/
#include <SaPolicyPei.h>

//[-start-140225-IB10920078-add]//
#include <SwitchableGraphicsDefine.h>
//[-end-140225-IB10920078-add]//

//[-start-120404-IB05300309-add]//
///
/// This IO Trap address can be overridden by defining it
/// in compiling environment variable
/// It must not conflict with other IO address in platform
///
#ifndef SA_IOTRAP_SMI_ADDRESS
#define SA_IOTRAP_SMI_ADDRESS 0x2000
#endif
//[-end-120404-IB05300309-add]//

//[-start-121128-IB03780468-add]//
//
// OcInitNotifyPpi
//
static EFI_PEI_NOTIFY_DESCRIPTOR    mOcInitNotifyDesc[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMemoryDiscoveredPpiGuid,
    OcInitCallback
  }
};
//[-end-121128-IB03780468-add]//

//[-start-140225-IB10920078-add]//
/**

  Graphics Translation Configuration of Primary Display setting didn't have SG mode,
  it just had IGD, PEG or PCI mode in PEI graphics Initialize code, so feature code needed base on
  System Configuration settings to change the Graphics Translation Configuration settings,
  SG mode output display is go through IGD, so the primary display should set to IGD.

  @param[in, out] SaPlatformPolicyPpi   A pointer of SA platform policy PPI to access the GTC
                                        (Graphics Translation Configuration) related information.
  @param[in]      SystemConfiguration   It's the setup variables (system configuration).

  @retval None.

**/
STATIC
VOID
SetSgRelatedSaPlatformPolicy (
  IN OUT SA_PLATFORM_POLICY_PPI               *SaPlatformPolicyPpi,
  IN CHIPSET_CONFIGURATION                     SystemConfiguration
  )
{
  if (SystemConfiguration.EnSGFunction == EnableSgFunction) {
    //
    // It can not add SG mode in reference code,
    // so need use setup configuration setting to make up the same action whit SG.
    //
    if (SystemConfiguration.PrimaryDisplay == DisplayModeSg) {
      if (SystemConfiguration.IGDControl == IgdDisable) {
        //
        // Set to PEG mode when SG mode to disable IGD
        //
        SaPlatformPolicyPpi->GtConfig->PrimaryDisplay = DisplayModeDgpu;
      } else {
        //
        // Set to IGD mode when SG mode to enable IGD
        //
        SaPlatformPolicyPpi->GtConfig->PrimaryDisplay = DisplayModeIgpu;
      }
    }
    //
    // If project's discrete GPU after power enable sequence still can't be powered on successfully,
    // "always enable PEG of PCIE configuration" policy need be set to always enable in SCU.
    //
  }
}

/**

  Base on system configuration to set Switchable Graphics own SG mode variable.

  @param[in] SystemConfiguration   It's the setup variables (system configuration).

  @retval SgModeDisabled   SG mode should be set to disabled on internal graphics only platform,
                           and should not power enable the discrete GPU.
  @retval SgModeMuxless    SG mode should be set to Muxless on dual graphics platform,
                           dual graphics platform ever used multiplexer to choose VGA output
                           that called Muxed, Muxless indicate the platform doesn't have multiplexer
                           and every output just go through internal or discrete GPU.
  @retval SgModeDgpu       SG mode should be set to discrete GPU on discrete graphics only platform,
                           if the discrete GPU used MXM interface, discrete GPU should be powered on.

**/
STATIC
SG_MODE_SETTING
SetSgModeValue (
  IN CHIPSET_CONFIGURATION                     SystemConfiguration
  )
{
  SG_MODE_SETTING                             SgMode;

  //
  // Set SG mode default
  //
  SgMode = SgModeDisabled;

  //
  // Base on System Configuration variable data to change SG mode
  //
  if (SystemConfiguration.PrimaryDisplay == DisplayModeSg) {
    SgMode = SgModeMuxless;
  }

  if ((SystemConfiguration.PrimaryDisplay == DisplayModeDgpu) ||
      (SystemConfiguration.PrimaryDisplay == DisplayModeAuto) ||
      (SystemConfiguration.PrimaryDisplay == DisplayModePci)  ||
      (SystemConfiguration.IGDControl == IgdDisable)) {
    SgMode = SgModeDgpu;
  }

  if ((SystemConfiguration.EnSGFunction == DisableSgFunction) ||
      (SystemConfiguration.PrimaryDisplay == DisplayModeIgpu)) {
    SgMode = SgModeDisabled;
  }

  return SgMode;
}


//[-end-140225-IB10920078-add]//
//[-start-121120-IB03780473-modify]//
EFI_STATUS
SaPolicyCallBack (
  IN  CONST EFI_PEI_SERVICES        **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN  VOID                          *Ppi
  )
//[-end-121120-IB03780473-modify]//
{
  EFI_STATUS                        Status;
  CHIPSET_CONFIGURATION              SystemConfiguration;
//[-start-130710-IB05160465-add]//
  VOID                              *SetupVariable;
//[-end-130710-IB05160465-add]//
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *Variable;
  UINTN                             VariableSize;
  EFI_GUID                          SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_PPI_DESCRIPTOR            *SaPlatformPolicyPpiDesc;
  SA_PLATFORM_POLICY_PPI            *SaPlatformPolicyPpi;
  UINT8                             SpdAddressTable[SA_MC_MAX_SOCKETS];
  UINT8                             *DimmSmbusAddrArray;
  EFI_PEI_HOB_POINTERS              Hob;
  VOID                              *MrcS3RestoreDataPtr;
  UINT8                             Index;
//[-start-120627-IB05330350-add]//
  PEG_GPIO_DATA                     *PegGpio;
//[-end-120627-IB05330350-add]//
//[-start-130421-IB05400398-remove]//
////[-start-120712-IB05330357-add]//
//  UINT8                              BoardID1;
//  UINT8                              BoardID2;
////[-end-120712-IB05330357-add]//
//[-end-130421-IB05400398-remove]//
//[-start-120816-IB06460430-add]//
  VOID                              *PegDataHob;
//[-end-120816-IB06460430-add]//
//[-start-121126-IB03780468-add]//
  WDT_PPI                            *WdtPpi;
  UINT8                              WdtStatus;
//[-end-121126-IB03780468-add]//
//[-start-121205-IB06460473-add]//
  EFI_BOOT_MODE                      SysBootMode;
//[-end-121205-IB06460473-add]//
  CPU_FAMILY                        CpuFamilyId;
//[-start-130709-IB05400426-add]//
  UINT32                            BestTolud;
//[-end-130709-IB05400426-add]//

  Variable                   = NULL;
  VariableSize               = 0;
  SaPlatformPolicyPpiDesc    = NULL;
  SaPlatformPolicyPpi        = NULL;
  DimmSmbusAddrArray         = NULL;
  MrcS3RestoreDataPtr        = NULL;
  Index                      = 0;
//[-start-120627-IB05330350-add]//
  PegGpio                    = NULL;
//[-end-120627-IB05330350-add]//
//[-start-120816-IB06460430-add]//
  PegDataHob                 = NULL;
//[-end-120816-IB06460430-add]//
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nSA Policy PEI Entry\n"));

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
  // Allocate memory for SA Policy PPI and Descriptor
  //
  SaPlatformPolicyPpi = (SA_PLATFORM_POLICY_PPI *)AllocateZeroPool (sizeof (SA_PLATFORM_POLICY_PPI));
  ASSERT (SaPlatformPolicyPpi != NULL);
//[-start-130207-IB10870073-add]//
  if (SaPlatformPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  SaPlatformPolicyPpi->PlatformData = (SA_PLATFORM_DATA *)AllocateZeroPool (sizeof (SA_PLATFORM_DATA));
  ASSERT (SaPlatformPolicyPpi->PlatformData != NULL);
//[-start-130207-IB10870073-add]//
  if (SaPlatformPolicyPpi->PlatformData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  SaPlatformPolicyPpi->GtConfig = (GT_CONFIGURATION *)AllocateZeroPool (sizeof (GT_CONFIGURATION));
  ASSERT (SaPlatformPolicyPpi->GtConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (SaPlatformPolicyPpi->GtConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  SaPlatformPolicyPpi->MemConfig = (MEMORY_CONFIGURATION *)AllocateZeroPool (sizeof (MEMORY_CONFIGURATION));
  ASSERT (SaPlatformPolicyPpi->MemConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (SaPlatformPolicyPpi->MemConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  SaPlatformPolicyPpi->PcieConfig = (PCIE_CONFIGURATION *)AllocateZeroPool (sizeof (PCIE_CONFIGURATION));
  ASSERT (SaPlatformPolicyPpi->PcieConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (SaPlatformPolicyPpi->PcieConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  SaPlatformPolicyPpi->OcConfig= (OVERCLOCKING_CONFIGURATION *)AllocateZeroPool (sizeof (OVERCLOCKING_CONFIGURATION));
  ASSERT (SaPlatformPolicyPpi->OcConfig != NULL);
//[-start-130207-IB10870073-add]//
  if (SaPlatformPolicyPpi->OcConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
//[-start-120627-IB05330350-add]//
  PegGpio = (PEG_GPIO_DATA *)AllocateZeroPool (sizeof (PEG_GPIO_DATA));
  ASSERT (PegGpio != NULL);
//[-start-130207-IB10870073-add]//
  if (PegGpio == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

  PegGpio->SaPegReset = (SA_GPIO_INFO *)AllocatePool (sizeof (SA_GPIO_INFO));
  ASSERT (PegGpio->SaPegReset != NULL);

  if (PegGpio->SaPegReset == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }  
  SaPlatformPolicyPpi->PcieConfig->PegGpioData = PegGpio;
//[-start-140219-IB10920078-modify]//
//[-start-130218-IB07250301-remove]//
  if (FeaturePcdGet (PcdIntelSwitchableGraphicsSupported)) { 
    SaPlatformPolicyPpi->SgGpioData = (SG_GPIO_DATA *)AllocateZeroPool (sizeof (SG_GPIO_DATA));
    ASSERT (SaPlatformPolicyPpi->SgGpioData != NULL);
  }
//[-end-130218-IB07250301-remove]//
//[-end-120627-IB05330350-add]//
//[-end-140219-IB10920078-modify]//
  SaPlatformPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (SaPlatformPolicyPpiDesc != NULL);
//[-start-130207-IB10870073-add]//
  if (SaPlatformPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//

//[-start-121023-IB03780466-modify]//
  ///
  /// Update the REVISION number
  ///
//[-start-121113-IB06150257-modify]//
  SaPlatformPolicyPpi->Revision = SA_PLATFORM_POLICY_PPI_REVISION_15;
//[-end-121113-IB06150257-modify]//
//[-end-121023-IB03780466-modify]//
//[-end-121211-IB06460476-modify]//

  CpuFamilyId = GetCpuFamily();

  //
  // SaPlatformPolicyPpi->PlatformData->SpdAddressTable
  //
  Status = (*PeiServices)->GetHobList (PeiServices, (VOID **)&Hob.Raw);
//[-start-130207-IB10870073-modify]//
  ASSERT_EFI_ERROR (Status);
  if (!EFI_ERROR (Status)) {
    while (!END_OF_HOB_LIST (Hob)) {
      if ((Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) && (CompareGuid (&Hob.Guid->Name, &gDimmSmbusAddrHobGuid))) {
        DimmSmbusAddrArray = (UINT8 *)(Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
        break;
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }
  } else {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-modify]//
  for (Index = 0; Index < SA_MC_MAX_SOCKETS; Index = Index + 1) {
    SpdAddressTable[Index] = DimmSmbusAddrArray[Index];
  }

  //
  // Initialize the Platform Configuration
  //
  CopyMem (SaPlatformPolicyPpi->PlatformData->SpdAddressTable, SpdAddressTable, sizeof(UINT8) * SA_MC_MAX_SOCKETS);
  SaPlatformPolicyPpi->PlatformData->MchBar                               = MCH_BASE_ADDRESS;
  SaPlatformPolicyPpi->PlatformData->DmiBar                               = PcdGet32 (PcdDmiBaseAddress);
  SaPlatformPolicyPpi->PlatformData->EpBar                                = PcdGet32 (PcdEpBaseAddress);
//[-start-120404-IB05300309-add]//
  SaPlatformPolicyPpi->PlatformData->EdramBar                             = PcdGet32 (PcdSaEdRamBaseAddess);
//[-end-120404-IB05300309-add]//
//[-start-121214-IB10820194-modify]//
  SaPlatformPolicyPpi->PlatformData->PciExpressBar                        = (UINT32)PcdGet64 (PcdPciExpressBaseAddress);
//[-end-121214-IB10820194-modify]//
  SaPlatformPolicyPpi->PlatformData->SmbusBar                             = PcdGet16 (PcdSmBusBaseAddress);
  SaPlatformPolicyPpi->PlatformData->TsegSize                             = TSEG_SIZE_8192K;
  SaPlatformPolicyPpi->PlatformData->IedSize                              = 0x400000;
  SaPlatformPolicyPpi->PlatformData->FastBoot                             = PcdGet8 (PcdPlatformDataFastBoot);
//[-start-120320-IB06460377-modify]//
  SaPlatformPolicyPpi->PlatformData->GdxcBar                              = PcdGet32 (PcdSaGdxcBaseAddress);
//[-end-120320-IB06460377-modify]//
//[-start-130421-IB05400398-modify]//
//[-start-120629-IB05330352-modify]//
//  if (FeaturePcdGet (PcdUltFlag)) {
////[-start-120711-IB05330357-modify]//
////[-start-121114-IB052800010-modify]//
//    GetUltBoardId (&BoardID1, &BoardID2);
////[-end-121114-IB052800010-modify]//
//    SaPlatformPolicyPpi->PlatformData->BoardId                            = BoardID2;
////[-end-120711-IB05330357-modify]//
//  } else {
////[-start-120627-IB05330350-add]//
//    SaPlatformPolicyPpi->PlatformData->BoardId                            = PcdGet16 (PcdPlatformDataBoardId);
////[-end-120627-IB05330350-add]//
//  }
  SaPlatformPolicyPpi->PlatformData->BoardId                              = PcdGet8 (PcdCurrentBoardId);
//[-end-130421-IB05400398-modify]//
  SaPlatformPolicyPpi->PlatformData->UserBd                               = PcdGet8 (PcdPlatformDataUserBd); 
//[-end-120629-IB05330352-modify]//

  //
  // Initialize the Graphics configuration
  //
//[-start-120220-IB03600472-modify]//
  SaPlatformPolicyPpi->GtConfig->MmioSize                                 = SystemConfiguration.MaxTOLUD ;
//[-end-120220-IB03600472-modify]//
  SaPlatformPolicyPpi->GtConfig->GttSize                                  = SystemConfiguration.GttSize;
  SaPlatformPolicyPpi->GtConfig->IgdDvmt50PreAlloc                        = SystemConfiguration.DvmtPreAllocated;
  SaPlatformPolicyPpi->GtConfig->InternalGraphics                         = SystemConfiguration.IGDControl;
  SaPlatformPolicyPpi->GtConfig->PrimaryDisplay                           = SystemConfiguration.PrimaryDisplay;
  SaPlatformPolicyPpi->GtConfig->ApertureSize                             = SystemConfiguration.ApertureSize;
  SaPlatformPolicyPpi->GtConfig->PanelPowerEnable                         = 1;
//[-start-121023-IB03780466-add]//
  SaPlatformPolicyPpi->GtConfig->GttMmAdr                                 = PcdGet32 (PcdSaGttTempBaseAddress);
//[-end-121023-IB03780466-add]//

//[-start-130709-IB05400426-add]//
  VariableSize = sizeof (UINT32);
  Status = Variable->GetVariable (
                       Variable,
                       L"BestTolud",
                       &gEfiGenericVariableGuid,
                       NULL,
                       &VariableSize,
                       &BestTolud
                       );
  if (!EFI_ERROR (Status)) {
    //
    // Be careful !! Althouth "BestTolud" variable exist, use it only when
    // Max_TOLUD is set to dynamic, otherwise system will hang at allocating 
    // MMIO resource !!
    //
    if (SystemConfiguration.MaxTOLUD == 0) {
      SaPlatformPolicyPpi->GtConfig->MmioSize = (UINT16)((0x0FFFFFFFF - BestTolud + 1) >> 20);
    }
    PcdSetBool (PcdBestToludExist, TRUE);
  } else {
    PcdSetBool (PcdBestToludExist, FALSE);
  }
//[-end-130709-IB05400426-add]//

  //
  // Initialize the Memory Configuration
  //
  SaPlatformPolicyPpi->MemConfig->SerialDebug                             = PcdGet8 (PcdMemConfigSerialDebug);
  SaPlatformPolicyPpi->MemConfig->EccSupport                              = PcdGet8 (PcdMemConfigEccSupport);
//[-start-121204-IB05300366-modify]//
  SaPlatformPolicyPpi->MemConfig->DdrFreqLimit                            = (UINT16)(UINTN) CalculateFrequencyLimit (SystemConfiguration);
//[-end-121204-IB05300366-modify]//
//[-start-130709-IB05400426-modify]//
//[-start-120220-IB03600472-modify]//
  SaPlatformPolicyPpi->MemConfig->MaxTolud                                = PcdGet8 (PcdMemConfigMaxTolud);
  if (SystemConfiguration.MaxTOLUD != 0) {
    SaPlatformPolicyPpi->MemConfig->MaxTolud                                = MAX_TOULD_MANUAL;
    PcdSetBool (PcdDynamicTolud, FALSE);
  } else {
    SaPlatformPolicyPpi->MemConfig->MaxTolud                                = MAX_TOULD_AUTO;
    PcdSetBool (PcdDynamicTolud, TRUE);
  }
//[-end-120220-IB03600472-modify]//
//[-end-130709-IB05400426-modify]//
//[-start-121022-IB05330384-remove]//
//  SaPlatformPolicyPpi->MemConfig->SpdProfileSelected                      = PcdGet8 ( PcdMemConfigSpdProfileSelected );
//[-end-121022-IB05330384-remove]//
//[-start-121127-IB03780468-remove]//
//  SaPlatformPolicyPpi->MemConfig->NModeSupport                            = SystemConfiguration.NModeSupport;
//[-end-121127-IB03780468-remove]//
//[-start-121026-IB06460460-modify]//
//[-start-120629-IB05330352-modify]//
//  if ( FeaturePcdGet ( PcdUltFlag )) {
//    SaPlatformPolicyPpi->MemConfig->ScramblerSupport                        = 0;
//  } else {
    SaPlatformPolicyPpi->MemConfig->ScramblerSupport                        = SystemConfiguration.ScramblerEnable;
//  }
//[-end-120629-IB05330352-modify]//
//[-end-121026-IB06460460-modify]//
//[-start-121009-IB03780464-add]//
  SaPlatformPolicyPpi->MemConfig->PowerDownMode                           = PcdGet8 (PcdMemConfigPowerDownMode);
  SaPlatformPolicyPpi->MemConfig->PwdwnIdleCounter                        = PcdGet8 (PcdMemConfigPwdwnIdleCounter);
  SaPlatformPolicyPpi->MemConfig->RankInterleave                          = PcdGetBool (PcdMemConfigRankInterleave);
  SaPlatformPolicyPpi->MemConfig->EnhancedInterleave                      = PcdGetBool (PcdMemConfigEnhancedInterleave);
  SaPlatformPolicyPpi->MemConfig->WeaklockEn                              = PcdGetBool (PcdMemConfigWeaklockEn);
  SaPlatformPolicyPpi->MemConfig->EnCmdRate                               = PcdGet8 (PcdMemConfigEnCmdRate);
  SaPlatformPolicyPpi->MemConfig->CmdTriStateDis                          = PcdGetBool (PcdMemConfigCmdTriStateDis);
//[-end-121009-IB03780464-add]//
  SaPlatformPolicyPpi->MemConfig->RefreshRate2x                           = FALSE;
  SaPlatformPolicyPpi->MemConfig->AutoSelfRefreshSupport                  = TRUE;
  SaPlatformPolicyPpi->MemConfig->ExtTemperatureSupport                   = TRUE;

//[-start-121127-IB03780468-remove]//
//  SaPlatformPolicyPpi->MemConfig->tCL                                     = PcdGet16 (PcdMemConfigtCL);
//  SaPlatformPolicyPpi->MemConfig->tRCD                                    = PcdGet16 (PcdMemConfigtRCD);
//  SaPlatformPolicyPpi->MemConfig->tRP                                     = PcdGet16 (PcdMemConfigtRP);
//  SaPlatformPolicyPpi->MemConfig->tRAS                                    = PcdGet16 (PcdMemConfigtRAS);
//  SaPlatformPolicyPpi->MemConfig->tWR                                     = PcdGet16 (PcdMemConfigtWR);
//  SaPlatformPolicyPpi->MemConfig->tRFC                                    = PcdGet16 (PcdMemConfigtRFC);
//  SaPlatformPolicyPpi->MemConfig->tRRD                                    = PcdGet16 (PcdMemConfigtRRD);
//  SaPlatformPolicyPpi->MemConfig->tWTR                                    = PcdGet16 (PcdMemConfigtWTR);
//  SaPlatformPolicyPpi->MemConfig->tRTP                                    = PcdGet16 (PcdMemConfigtRTP);
//  SaPlatformPolicyPpi->MemConfig->tRC                                     = PcdGet16 (PcdMemConfigtRC);
//  SaPlatformPolicyPpi->MemConfig->tFAW                                    = PcdGet16 (PcdMemConfigtFAW);
//  SaPlatformPolicyPpi->MemConfig->tCWL                                    = PcdGet16 (PcdMemConfigtCWL);
//  SaPlatformPolicyPpi->MemConfig->tREFI                                   = PcdGet16 (PcdMemConfigtREFI);
//[-end-121127-IB03780468-remove]//
  //
  // Thermal Management
  //
  SaPlatformPolicyPpi->MemConfig->ThermalManagement                       = PcdGet8 (PcdMemConfigThermalManagement);
  SaPlatformPolicyPpi->MemConfig->PeciInjectedTemp                        = PcdGet8 (PcdMemConfigPeciInjectedTemp);
  SaPlatformPolicyPpi->MemConfig->ExttsViaTsOnBoard                       = PcdGet8 (PcdMemConfigExttsViaTsOnBoard); 
  SaPlatformPolicyPpi->MemConfig->ExttsViaTsOnDimm                        = PcdGet8 (PcdMemConfigExttsViaTsOnDimm);
  SaPlatformPolicyPpi->MemConfig->VirtualTempSensor                       = PcdGet8 (PcdMemConfigVirtualTempSensor);
//[-start-120404-IB05300309-modify]//
  //
  // Force ColdReset
  //
  SaPlatformPolicyPpi->MemConfig->ForceColdReset                          = PcdGetBool (PcdMemConfigForceColdReset);
//[-end-120404-IB05300309-modify]//
  //
  // Channel DIMM Disable
  //
  SaPlatformPolicyPpi->MemConfig->DisableDimmChannel[0]                   = PcdGet8 (PcdMemConfigDisableDimmChannel0);
  SaPlatformPolicyPpi->MemConfig->DisableDimmChannel[1]                   = PcdGet8 (PcdMemConfigDisableDimmChannel0);
  SaPlatformPolicyPpi->MemConfig->ChHashEnable                            = TRUE;
  SaPlatformPolicyPpi->MemConfig->ChHashMask                              = 0x30CE;
  SaPlatformPolicyPpi->MemConfig->ChHashInterleaveBit                     = 1;
  ///
  /// Options for Thermal settings
  ///
//[-start-130403-IB05330413-modify]//
  SaPlatformPolicyPpi->MemConfig->EnableExtts                             = SystemConfiguration.ExternThermalStatus;
  SaPlatformPolicyPpi->MemConfig->EnableCltm                              = SystemConfiguration.ClosedLoopThermalManage;
  SaPlatformPolicyPpi->MemConfig->EnableOltm                              = SystemConfiguration.OpenLoopThermalManage;
  SaPlatformPolicyPpi->MemConfig->EnablePwrDn                             = SystemConfiguration.EnablePwrDn;
  if (CpuFamilyId == EnumCpuHswUlt) {
    SaPlatformPolicyPpi->MemConfig->EnablePwrDnLpddr                      = SystemConfiguration.EnablePwrDnLpddr;
  }
  SaPlatformPolicyPpi->MemConfig->Refresh2X                               = SystemConfiguration.Refresh2X;
  SaPlatformPolicyPpi->MemConfig->LpddrThermalSensor                      = SystemConfiguration.LpddrThermalSensor;
  SaPlatformPolicyPpi->MemConfig->LockPTMregs                             = SystemConfiguration.LockThermalManageRegs;
  SaPlatformPolicyPpi->MemConfig->UserPowerWeightsEn                      = SystemConfiguration.UserPowerWeightsEn;

  SaPlatformPolicyPpi->MemConfig->EnergyScaleFact                         = SystemConfiguration.EnergyScaleFact;
  SaPlatformPolicyPpi->MemConfig->RaplPwrFlCh1                            = SystemConfiguration.RaplPwrFlCh1;
  SaPlatformPolicyPpi->MemConfig->RaplPwrFlCh0                            = SystemConfiguration.RaplPwrFlCh0;

  SaPlatformPolicyPpi->MemConfig->RaplLim2Lock                            = SystemConfiguration.RaplPLLock;
  SaPlatformPolicyPpi->MemConfig->RaplLim2WindX                           = SystemConfiguration.RaplPL2WindowX;
  SaPlatformPolicyPpi->MemConfig->RaplLim2WindY                           = SystemConfiguration.RaplPL2WindowY;
  SaPlatformPolicyPpi->MemConfig->RaplLim2Ena                             = SystemConfiguration.RaplPL2Enable;
  SaPlatformPolicyPpi->MemConfig->RaplLim2Pwr                             = SystemConfiguration.RaplPL2Power;
  SaPlatformPolicyPpi->MemConfig->RaplLim1WindX                           = SystemConfiguration.RaplPL1WindowX;
  SaPlatformPolicyPpi->MemConfig->RaplLim1WindY                           = SystemConfiguration.RaplPL1WindowY;
  SaPlatformPolicyPpi->MemConfig->RaplLim1Ena                             = SystemConfiguration.RaplPL1Enable;
  SaPlatformPolicyPpi->MemConfig->RaplLim1Pwr                             = SystemConfiguration.RaplPL1Power;

  SaPlatformPolicyPpi->MemConfig->WarmThresholdCh0Dimm0                   = SystemConfiguration.WarmThresholdCh0Dimm0;
  SaPlatformPolicyPpi->MemConfig->WarmThresholdCh0Dimm1                   = SystemConfiguration.WarmThresholdCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->WarmThresholdCh1Dimm0                   = SystemConfiguration.WarmThresholdCh1Dimm0;
  SaPlatformPolicyPpi->MemConfig->WarmThresholdCh1Dimm1                   = SystemConfiguration.WarmThresholdCh1Dimm1;
  SaPlatformPolicyPpi->MemConfig->HotThresholdCh0Dimm0                    = SystemConfiguration.HotThresholdCh0Dimm0;
  SaPlatformPolicyPpi->MemConfig->HotThresholdCh0Dimm1                    = SystemConfiguration.HotThresholdCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->HotThresholdCh1Dimm0                    = SystemConfiguration.HotThresholdCh1Dimm0;
  SaPlatformPolicyPpi->MemConfig->HotThresholdCh1Dimm1                    = SystemConfiguration.HotThresholdCh1Dimm1;
  SaPlatformPolicyPpi->MemConfig->WarmBudgetCh0Dimm0                      = SystemConfiguration.WarmBudgetCh0Dimm0;
  SaPlatformPolicyPpi->MemConfig->WarmBudgetCh0Dimm1                      = SystemConfiguration.WarmBudgetCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->WarmBudgetCh1Dimm0                      = SystemConfiguration.WarmBudgetCh1Dimm0;
  SaPlatformPolicyPpi->MemConfig->WarmBudgetCh1Dimm1                      = SystemConfiguration.WarmBudgetCh1Dimm1;
  SaPlatformPolicyPpi->MemConfig->HotBudgetCh0Dimm0                       = SystemConfiguration.HotBudgetCh0Dimm0;
  SaPlatformPolicyPpi->MemConfig->HotBudgetCh0Dimm1                       = SystemConfiguration.HotBudgetCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->HotBudgetCh1Dimm0                       = SystemConfiguration.HotBudgetCh1Dimm0;
  SaPlatformPolicyPpi->MemConfig->HotBudgetCh1Dimm1                       = SystemConfiguration.HotBudgetCh1Dimm1;

  SaPlatformPolicyPpi->MemConfig->IdleEnergyCh0Dimm1                      = SystemConfiguration.IdleEnergyCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->IdleEnergyCh0Dimm0                      = SystemConfiguration.IdleEnergyCh0Dimm0;
  SaPlatformPolicyPpi->MemConfig->PdEnergyCh0Dimm1                        = SystemConfiguration.PdEnergyCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->PdEnergyCh0Dimm0                        = SystemConfiguration.PdEnergyCh0Dimm0;
  SaPlatformPolicyPpi->MemConfig->ActEnergyCh0Dimm1                       = SystemConfiguration.ActEnergyCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->ActEnergyCh0Dimm0                       = SystemConfiguration.ActEnergyCh0Dimm0;
  SaPlatformPolicyPpi->MemConfig->RdEnergyCh0Dimm1                        = SystemConfiguration.RdEnergyCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->RdEnergyCh0Dimm0                        = SystemConfiguration.RdEnergyCh0Dimm0;
  SaPlatformPolicyPpi->MemConfig->WrEnergyCh0Dimm1                        = SystemConfiguration.WrEnergyCh0Dimm1;
  SaPlatformPolicyPpi->MemConfig->WrEnergyCh0Dimm0                        = SystemConfiguration.WrEnergyCh0Dimm0;

  SaPlatformPolicyPpi->MemConfig->IdleEnergyCh1Dimm1                      = SystemConfiguration.IdleEnergyCh1Dimm1;
  SaPlatformPolicyPpi->MemConfig->IdleEnergyCh1Dimm0                      = SystemConfiguration.IdleEnergyCh1Dimm0;
  SaPlatformPolicyPpi->MemConfig->PdEnergyCh1Dimm1                        = SystemConfiguration.PdEnergyCh1Dimm1;
  SaPlatformPolicyPpi->MemConfig->PdEnergyCh1Dimm0                        = SystemConfiguration.PdEnergyCh1Dimm0;
  SaPlatformPolicyPpi->MemConfig->ActEnergyCh1Dimm1                       = SystemConfiguration.ActEnergyCh1Dimm1;
  SaPlatformPolicyPpi->MemConfig->ActEnergyCh1Dimm0                       = SystemConfiguration.ActEnergyCh1Dimm0;
  SaPlatformPolicyPpi->MemConfig->RdEnergyCh1Dimm1                        = SystemConfiguration.RdEnergyCh1Dimm1;
  SaPlatformPolicyPpi->MemConfig->RdEnergyCh1Dimm0                        = SystemConfiguration.RdEnergyCh1Dimm0;
  SaPlatformPolicyPpi->MemConfig->WrEnergyCh1Dimm1                        = SystemConfiguration.WrEnergyCh1Dimm1;
  SaPlatformPolicyPpi->MemConfig->WrEnergyCh1Dimm0                        = SystemConfiguration.WrEnergyCh1Dimm0;

  SaPlatformPolicyPpi->MemConfig->SrefCfgEna                              = SystemConfiguration.SrefCfgEna;
  SaPlatformPolicyPpi->MemConfig->SrefCfgIdleTmr                          = SystemConfiguration.SrefCfgIdleTmr;
  SaPlatformPolicyPpi->MemConfig->ThrtCkeMinDefeat                        = SystemConfiguration.ThrtCkeMinDefeat;
  SaPlatformPolicyPpi->MemConfig->ThrtCkeMinTmr                           = SystemConfiguration.ThrtCkeMinTmr;
  if (CpuFamilyId == EnumCpuHswUlt) {
    SaPlatformPolicyPpi->MemConfig->ThrtCkeMinDefeatLpddr                  = SystemConfiguration.ThrtCkeMinDefeatLpddr;
    SaPlatformPolicyPpi->MemConfig->ThrtCkeMinTmrLpddr                     = SystemConfiguration.ThrtCkeMinTmrLpddr;
  }
//[-end-130403-IB05330413-modify]//

  SaPlatformPolicyPpi->MemConfig->McLock                                  = PcdGet8 (PcdMemConfigMcLock);
  //
  // SA : RestrictedEnd
  //
  SaPlatformPolicyPpi->MemConfig->GdxcEnable                              = PcdGet8 (PcdMemConfigGdxcEnable);
  SaPlatformPolicyPpi->MemConfig->GdxcIotSize                             = PcdGet8 (PcdMemConfigGdxcIotSize);
  SaPlatformPolicyPpi->MemConfig->GdxcMotSize                             = PcdGet8 (PcdMemConfigGdxcMotSize);
  SaPlatformPolicyPpi->MemConfig->MemoryTrace                             = 0;
  //
  // Training Algorithms
  //
  SaPlatformPolicyPpi->MemConfig->ECT                                     = PcdGet8 (PcdMemConfigECT);
  SaPlatformPolicyPpi->MemConfig->SOT                                     = PcdGet8 (PcdMemConfigSOT);
  SaPlatformPolicyPpi->MemConfig->RDMPRT                                  = PcdGet8 (PcdMemConfigRDMPRT);
  SaPlatformPolicyPpi->MemConfig->RCVET                                   = PcdGet8 (PcdMemConfigRCVET);
  SaPlatformPolicyPpi->MemConfig->JWRL                                    = PcdGet8 (PcdMemConfigJWRL);
  SaPlatformPolicyPpi->MemConfig->FWRL                                    = PcdGet8 (PcdMemConfigFWRL);
  SaPlatformPolicyPpi->MemConfig->WRTC1D                                  = PcdGet8 (PcdMemConfigWRTC1D);
  SaPlatformPolicyPpi->MemConfig->RDTC1D                                  = PcdGet8 (PcdMemConfigRDTC1D);
  SaPlatformPolicyPpi->MemConfig->DIMMODTT                                = PcdGet8 (PcdMemConfigDIMMODTT);
  SaPlatformPolicyPpi->MemConfig->WRDST                                   = PcdGet8 (PcdMemConfigWRDST);
  SaPlatformPolicyPpi->MemConfig->WREQT                                   = PcdGet8 (PcdMemConfigWREQT);
//[-start-121023-IB03780466-remove]//
//  SaPlatformPolicyPpi->MemConfig->WRXTCT                                  = PcdGet8 ( PcdMemConfigWRXTCT );
//[-end-121023-IB03780466-remove]//
  SaPlatformPolicyPpi->MemConfig->RDODTT                                  = PcdGet8 (PcdMemConfigRDODTT);
  SaPlatformPolicyPpi->MemConfig->RDEQT                                   = PcdGet8 (PcdMemConfigRDEQT);
  SaPlatformPolicyPpi->MemConfig->RDAPT                                   = PcdGet8 (PcdMemConfigRDAPT);
//[-start-120627-IB05330350-modify]//
  SaPlatformPolicyPpi->MemConfig->WRTC2D                                  = PcdGet8 (PcdMemConfigWRTC2D);
  SaPlatformPolicyPpi->MemConfig->RDTC2D                                  = PcdGet8 (PcdMemConfigRDTC2D);
//[-start-120917-IB06150247-add]//
  SaPlatformPolicyPpi->MemConfig->CMDVC                                   = PcdGet8 (PcdMemConfigCMDVC);
//[-end-120917-IB06150247-add]//
  SaPlatformPolicyPpi->MemConfig->WRVC2D                                  = PcdGet8 (PcdMemConfigWRVC2D);
  SaPlatformPolicyPpi->MemConfig->RDVC2D                                  = PcdGet8 (PcdMemConfigRDVC2D);
//[-end-120627-IB05330350-modify]//
  SaPlatformPolicyPpi->MemConfig->B2BXTT                                  = PcdGet8 (PcdMemConfigB2BXTT);
  SaPlatformPolicyPpi->MemConfig->C2CXTT                                  = PcdGet8 (PcdMemConfigC2CXTT);
  SaPlatformPolicyPpi->MemConfig->LCT                                     = PcdGet8 (PcdMemConfigLCT);
  SaPlatformPolicyPpi->MemConfig->RTL                                     = PcdGet8 (PcdMemConfigRTL);
  SaPlatformPolicyPpi->MemConfig->TAT                                     = PcdGet8 (PcdMemConfigTAT);
  SaPlatformPolicyPpi->MemConfig->RMT                                     = PcdGet8 (PcdMemConfigRMT);
  SaPlatformPolicyPpi->MemConfig->MEMTST                                  = PcdGet8 (PcdMemConfigMEMTST);
//[-start-120404-IB05300309-add]//
  SaPlatformPolicyPpi->MemConfig->DIMMODTT1D                              = PcdGet8 (PcdMemConfigDIMMODTT1D);
  SaPlatformPolicyPpi->MemConfig->WRSRT                                   = PcdGet8 (PcdMemConfigWRSRT);
  SaPlatformPolicyPpi->MemConfig->DIMMRONT                                = PcdGet8 (PcdMemConfigDIMMRONT);
//[-end-120404-IB05300309-add]//
//[-start-121023-IB03780466-add]//
  SaPlatformPolicyPpi->MemConfig->tRPab                                   = PcdGet16 (PcdMemConfigtRPab);
  SaPlatformPolicyPpi->MemConfig->ALIASCHK                                = PcdGet8 (PcdMemConfigALIASCHK);
//[-end-121023-IB03780466-add]//
  SaPlatformPolicyPpi->MemConfig->RCVENC1D                                = PcdGet8 (PcdMemConfigRCVENC1D);
  SaPlatformPolicyPpi->MemConfig->RMC                                     = PcdGet8 (PcdMemConfigRMC);
  SaPlatformPolicyPpi->MemConfig->RemapEnable                             = PcdGetBool(PcdMemConfigRemapEnable);
  SaPlatformPolicyPpi->MemConfig->RmtBdatEnable                           = PcdGetBool(PcdMemConfigRmtBdatEnable);
  SaPlatformPolicyPpi->MemConfig->MrcTimeMeasure                          = PcdGet8 (PcdMemConfigMrcTimeMeasure);
  SaPlatformPolicyPpi->MemConfig->MrcFastBoot                             = PcdGet8 (PcdMemConfigMrcFastBoot);

#ifdef MRC_RMT
  SaPlatformPolicyPpi->MemConfig->MrcFastBoot                             = FALSE;
#endif
  
//[-start-120319-IB06460376-modify]//
//[-start-120529-IB05330343-modify]//
//[-start-120628-IB05330350-modify]//
//[-start-121019-IB05330384-add]//
//
//  Setting SpdProfileSelected value to UserDefined, then DDR3 voltage is available.
//
  SaPlatformPolicyPpi->MemConfig->SpdProfileSelected                      = PcdGet8 (PcdMemConfigSpdProfileSelected);
//[-end-121019-IB05330384-add]//
  SaPlatformPolicyPpi->MemConfig->DDR3Voltage                             = PcdGet16 (PcdMemConfigDDR3Voltage);
//[-end-120628-IB05330350-modify]//
//[-start-120628-IB05330350-remove]//
//  if (SystemConfiguration.DDR3Voltage == 1) {
//    SaPlatformPolicyPpi->MemConfig->DDR3Voltage                             = 1500;
//  }
//  if (SystemConfiguration.DDR3Voltage == 2) {
//    SaPlatformPolicyPpi->MemConfig->DDR3Voltage                             = 1350;
//  }
//[-end-120628-IB05330350-remove]//
//[-end-120529-IB05330343-modify]//
//[-end-120319-IB06460376-modify]//
  SaPlatformPolicyPpi->MemConfig->DDR3VoltageWaitTime                     = PcdGet32 (PcdMemConfigDDR3VoltageWaitTime);
  SaPlatformPolicyPpi->MemConfig->RefClk                                  = PcdGet8 (PcdMemConfigRefClk);
  SaPlatformPolicyPpi->MemConfig->Ratio                                   = PcdGet8 (PcdMemConfigRatio);
//[-start-121009-IB03780464-add]//
  SaPlatformPolicyPpi->MemConfig->BClkFrequency                           = PcdGet32 (PcdMemConfigBClkFrequency);
//[-end-121009-IB03780464-add]//
  SaPlatformPolicyPpi->MemConfig->MaxRttWr                                = 0;
  

//[-start-120511-IB04770230-remove]//
// //[-start-120404-IB05300309-add]//
// #ifdef ULT_SUPPORT
//   ///
//   /// Interleaving mode of DQ/DQS pins - depends on board routing
//   ///
//   SaPlatformPolicyPpi->MemConfig->DqPinsInterleaved                       = FALSE;
// #endif // ULT_SUPPORT
// //[-end-120404-IB05300309-add]//
//[-end-120511-IB04770230-remove]//
//[-start-120511-IB04770230-add]//
//[-start-120627-IB05330350-add]//
  ///
  /// MrcUltPoSafeConfig
  /// 1 to enable, 0 to disable
  ///
//[-start-121023-IB03780466-remove]//
//  SaPlatformPolicyPpi->MemConfig->MrcUltPoSafeConfig =  PcdGetBool( PcdMemConfigMrcUltPoSafeConfig );
//[-end-121023-IB03780466-remove]//
  if (GetCpuFamily () == EnumCpuHswUlt) {
    ///
    /// Interleaving mode of DQ/DQS pins - depends on board routing
    ///
    SaPlatformPolicyPpi->MemConfig->DqPinsInterleaved                     = FALSE;
//[-start-121023-IB03780466-remove]//
//    SaPlatformPolicyPpi->MemConfig->MrcUltPoSafeConfig                    = 1;
//[-end-121023-IB03780466-remove]//
//[-start-120808-IB05330369-add]//
//[-start-121130-IB06150264-remove]//
//    SaPlatformPolicyPpi->MemConfig->MrcFastBoot                           = FALSE;
//[-end-121130-IB06150264-remove]//
//[-end-120808-IB05330369-add]//
  }
//[-end-120627-IB05330350-add]//
//[-end-120511-IB04770230-add]//
//[-start-121113-IB06150257-add]//
  SaPlatformPolicyPpi->MemConfig->MemoryTrace                             = PcdGet8 (PcdMemConfigMemoryTrace);
//[-end-121113-IB06150257-add]//
  //
  // Initialize the PciExpress Configuration
  //
  SaPlatformPolicyPpi->PcieConfig->DmiVc1                                 = SystemConfiguration.DmiVc1;
  SaPlatformPolicyPpi->PcieConfig->DmiVcp                                 = SystemConfiguration.DmiVcp;
  SaPlatformPolicyPpi->PcieConfig->DmiVcm                                 = SystemConfiguration.DmiVcm;
  SaPlatformPolicyPpi->PcieConfig->DmiGen2                                = SystemConfiguration.DmiGen2;
//[-start-120606-IB04770231-modify]//
  SaPlatformPolicyPpi->PcieConfig->AlwaysEnablePeg                        = SystemConfiguration.AlwaysEnablePeg;    
//[-end-120606-IB04770231-modify]//
  SaPlatformPolicyPpi->PcieConfig->PegGenx[0]                             = SystemConfiguration.PegGenx0;
  SaPlatformPolicyPpi->PcieConfig->PegGenx[1]                             = SystemConfiguration.PegGenx1;
  SaPlatformPolicyPpi->PcieConfig->PegGenx[2]                             = SystemConfiguration.PegGenx2;

  SaPlatformPolicyPpi->PcieConfig->PegGen3Equalization                    = PcdGet8 (PcdPcieConfigPegGen3Equalization);
//[-start-110930-IB05300224-add]//
  SaPlatformPolicyPpi->PcieConfig->PcieResetDelay                         = SystemConfiguration.PcieResetDelay;
//[-end-110930-IB05300224-add]//
  SaPlatformPolicyPpi->PcieConfig->PegSamplerCalibrate                    = PcdGet8 (PcdPcieConfigPegSamplerCalibrate);
//[-start-121023-IB03780466-remove]//
//  SaPlatformPolicyPpi->PcieConfig->PegGen3EqualizationPhase2              = PcdGet8 ( PcdPcieConfigPegGen3EqualizationPhase2 );
//[-end-121023-IB03780466-remove]//
  SaPlatformPolicyPpi->PcieConfig->PegSwingControl                        = PcdGet8 (PcdPcieConfigPegSwingControl);
  SaPlatformPolicyPpi->PcieConfig->PegComplianceTestingMode  							= 0;	

  ///
  /// PEG Gen3 Preset Search: 0 = Disabled, 1 = Enabled (default)
  /// PEG Gen3 Force Preset Search (always re-search): 0 = Disabled (default), 1 = Enabled
  /// PEG Gen3 Preset Search Dwell Time: 100 ms
  /// PEG Gen3 Preset Search Timing Margin Steps: 2
  /// PEG Gen3 Preset Search Timing Start Margin: 15
  /// PEG Gen3 Preset Search Voltage Margin Steps: 2
  /// PEG Gen3 Preset Search Voltage Start Margin: 20
  /// PEG Gen3 Preset Search Favor Timing: 0 = Timing + Voltage (default), 1 = Timing only
  ///
  SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch                    = PcdGet8 (PcdPcieConfigPegGen3PresetSearch);
//[-start-120404-IB05300309-add]//
  SaPlatformPolicyPpi->PcieConfig->PegGen3ForcePresetSearch               = PcdGetBool (PcdPcieConfigPegGen3ForcePresetSearch);
//[-end-120404-IB05300309-add]//
//[-start-121009-IB03780464-modify]//
//[-start-120627-IB05330350-modify]//
  SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchDwellTime           = STALL_ONE_MILLI_SECOND;
//[-end-120627-IB05330350-modify]//
//[-end-121009-IB03780464-modify]//
//[-start-121023-IB03780466-remove]//
//  SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchMarginSteps         = PcdGet8 ( PcdPcieConfigPegGen3PresetSearchMarginSteps );
//[-end-121023-IB03780466-remove]//
  SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchStartMargin         = PcdGet8 (PcdPcieConfigPegGen3PresetSearchStartMargin);
//[-start-120404-IB05300309-add]//
//[-start-121023-IB03780466-remove]//
//  SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchVoltageMarginSteps  = PcdGet8 ( PcdPcieConfigPegGen3PresetSearchVoltageMarginSteps );
//[-end-121023-IB03780466-remove]//
  SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchVoltageStartMargin  = PcdGet8 (PcdPcieConfigPegGen3PresetSearchVoltageStartMargin);
//[-start-121023-IB03780466-remove]//
//  SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchFavorTiming         = PcdGet8 ( PcdPcieConfigPegGen3PresetSearchFavorTiming );
//[-end-121023-IB03780466-remove]//
//[-end-120404-IB05300309-add]//
//[-start-120627-IB05330350-add]//
  SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchErrorTarget         = PcdGet16 (PcdPcieConfigPegGen3PresetSearchErrorTarget);
  ///
  /// Parameters for PCIe Gen3 device reset
  ///
//[-start-120630-IB05330352-modify]//
 
  PegGpio->GpioSupport = PcdGetBool (PcdPegGpioDataGpioSupport);

  if (PegGpio->GpioSupport) {
    ///
    /// PEG Reset: GPIO 50, Active Low (Mobile PDG)
    ///
    PegGpio->SaPegReset->Value  = PcdGet8 (PcdPegGpioDataSaPegResetValue);
    PegGpio->SaPegReset->Active = PcdGetBool (PcdPegGpioDataSaPegResetActive);
  }
//[-end-120627-IB05330350-add]//
//[-start-120404-IB05300309-modify]//
  for (Index = 0; Index < SA_PEG_MAX_LANE; Index = Index + 1) {
    SaPlatformPolicyPpi->PcieConfig->Gen3RootPortPreset[Index]            = PcdGet8 (PcdPcieConfigGen3RootPortPreset);
    SaPlatformPolicyPpi->PcieConfig->Gen3EndPointPreset[Index]            = PcdGet8 (PcdPcieConfigGen3EndPointPreset);
    SaPlatformPolicyPpi->PcieConfig->Gen3EndPointHint[Index]              = PcdGet8 (PcdPcieConfigGen3EndPointHint);
  }
//[-end-120404-IB05300309-modify]//
//[-start-120627-IB05330350-add]//
  ///
  /// Enable/Disable RxCEM Loop back
  /// 1=Enable, 0=Disable (default)
  /// When enabled, Lane for loop back should be selected (0 ~ 15 and default is Lane 0)
  ///
  SaPlatformPolicyPpi->PcieConfig->RxCEMLoopback     = PcdGetBool (PcdPcieConfigRxCEMLoopback);
  SaPlatformPolicyPpi->PcieConfig->RxCEMLoopbackLane = PcdGet8 (PcdPcieConfigRxCEMLoopbackLane);
  ///
  /// Gen3 RxCTLE peaking default is 8
  ///
  for (Index = 0; Index < SA_PEG_MAX_BUNDLE; Index++) {
    SaPlatformPolicyPpi->PcieConfig->Gen3RxCtleP[Index] = PcdGet8 (PcdPcieConfigGen3RxCtleP);
  }
//[-end-120627-IB05330350-add]//
//[-start-121211-IB06460476-add]//
  for (Index = 0; Index < SA_PEG_MAX_FUN; Index++) {
    SaPlatformPolicyPpi->PcieConfig->PowerDownUnusedBundles[Index] = 0xFF;
  }
//[-end-121211-IB06460476-add]//
//[-start-120404-IB05300309-add]//
  ///
  /// Parameters for PCIe ASPM flow control
  ///   InitPcieAspmAfterOprom:
  ///     0 (default) - PCIe ASPM will be initialized Before Oprom
  ///     1           - PCIe ASPM will be initialized After Oprom (required IOTRAP SMI handler)
  ///   Note: This setting should match supported mode!
  ///
  ///   SaIotrapSmiAddress:
  ///     IOTRAP SMI address for SA SMI callback handler. This should be given if platform supports InitPcieAspmAfterOprom = 1 scenario (SaLateInitSmm driver was compiled)
  ///
//[-start-120807-IB05330369-modify]//
  SaPlatformPolicyPpi->PcieConfig->InitPcieAspmAfterOprom                 = PcdGetBool (PcdPcieConfigInitPcieAspmAfterOprom);
//[-end-120807-IB05330369-modify]//
  SaPlatformPolicyPpi->PcieConfig->SaIotrapSmiAddress                     = SA_IOTRAP_SMI_ADDRESS;

  ///
  /// Initialize the SA PEG Data pointer for saved preset search results
  ///
  SaPlatformPolicyPpi->PcieConfig->PegDataPtr                             = NULL;
//[-start-120816-IB06460430-add]//
  VariableSize        = 0 ;    // Set VariableSize as 0 at first time calling GetVariable to get the correct variable data size
  Status = Variable->GetVariable (
                       Variable,
                       PEG_DATA_VARIABLE_NAME,
                       &gPegDataVariableGuid,
                       NULL,
                       &VariableSize,
                       PegDataHob
                       );
  if ((Status == EFI_BUFFER_TOO_SMALL) && (VariableSize != 0)) {
    PegDataHob = (VOID *)AllocateZeroPool (VariableSize);
    ASSERT (PegDataHob != NULL);

    Status = Variable->GetVariable (
                         Variable,
                         PEG_DATA_VARIABLE_NAME,
                         &gPegDataVariableGuid,
                         NULL,
                         &VariableSize,
                         PegDataHob
                        );
    ASSERT_EFI_ERROR (Status);
  }
  if (PegDataHob == NULL) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PegDataHob does not be found!!\n"));
  }
  SaPlatformPolicyPpi->PcieConfig->PegDataPtr                             = PegDataHob;
//[-end-120816-IB06460430-add]//
//[-end-120404-IB05300309-add]//

//[-start-121101-IB03780468-add]//
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

    Status = PeiServicesNotifyPpi (mOcInitNotifyDesc);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Initialize Overclocking Configuration for GT, Uncore, IOA, IOD, and Memory.
  //
  if (WdtStatus == V_PCH_OC_WDT_CTL_STATUS_OK) {
    //
    // Apply Overclocking Configuration only when system boots correctly in previous POST.
    //
    SaPlatformPolicyPpi->OcConfig->OcSupport             = 1;

    //
    // Update OC settings for GT.
    //
    if (SystemConfiguration.OcCapGt == 1) {
      SaPlatformPolicyPpi->OcConfig->GtVoltageOffset     = (SystemConfiguration.GtVoltageOffsetPrefix == 0) ? SystemConfiguration.GtVoltageOffset : SystemConfiguration.GtVoltageOffset | BIT15;
      SaPlatformPolicyPpi->OcConfig->GtVoltageOverride   = SystemConfiguration.GtVoltageOverride;
      SaPlatformPolicyPpi->OcConfig->GtExtraTurboVoltage = SystemConfiguration.GtExtraTurboVoltage;
      SaPlatformPolicyPpi->OcConfig->GtMaxOcTurboRatio   = SystemConfiguration.GtOverclockFreq;
      SaPlatformPolicyPpi->OcConfig->GtVoltageMode       = SystemConfiguration.GtVoltageMode;
    }

    //
    // Update OC settings for Uncore.
    //
    if (SystemConfiguration.OcCapUncore == 1) {
      SaPlatformPolicyPpi->OcConfig->SaVoltageOffset     = (SystemConfiguration.UncoreVoltageOffsetPrefix == 0) ? SystemConfiguration.UncoreVoltageOffset : SystemConfiguration.UncoreVoltageOffset | BIT15;
    }

    //
    // Update OC settings for IOA.
    //
    if (SystemConfiguration.OcCapIoa == 1) {
      SaPlatformPolicyPpi->OcConfig->IoaVoltageOffset    = (SystemConfiguration.IoaVoltageOffsetPrefix == 0) ? SystemConfiguration.IoaVoltageOffset : SystemConfiguration.IoaVoltageOffset | BIT15;
    }

    //
    // Update OC settings for IOD.
    //
    if (SystemConfiguration.OcCapIod == 1) {
      SaPlatformPolicyPpi->OcConfig->IodVoltageOffset    = (SystemConfiguration.IodVoltageOffsetPrefix == 0) ? SystemConfiguration.IodVoltageOffset : SystemConfiguration.IodVoltageOffset | BIT15;
    }

    //
    // Apply Overclocking Configuration only when system boots correctly in previous POST.
    //
    SaPlatformPolicyPpi->MemConfig->SpdProfileSelected   = SystemConfiguration.XmpProfileSetting;

    //
    // Update custom OC settings for Memory when SPD profile is selected to user defined.
    //
    if (SystemConfiguration.XmpProfileSetting == UserDefined) {
      SaPlatformPolicyPpi->MemConfig->RefClk             = SystemConfiguration.MemoryRefClk;
      SaPlatformPolicyPpi->MemConfig->Ratio              = SystemConfiguration.MemoryRatio;
      SaPlatformPolicyPpi->MemConfig->tCL                = SystemConfiguration.MemoryTcl;
      SaPlatformPolicyPpi->MemConfig->tRCD               = SystemConfiguration.MemoryTrcd;
      SaPlatformPolicyPpi->MemConfig->tRP                = SystemConfiguration.MemoryTrp;
      SaPlatformPolicyPpi->MemConfig->tRAS               = SystemConfiguration.MemoryTras;
      SaPlatformPolicyPpi->MemConfig->tWR                = SystemConfiguration.MemoryTwr;
      SaPlatformPolicyPpi->MemConfig->tRFC               = SystemConfiguration.MemoryTrfc;
      SaPlatformPolicyPpi->MemConfig->tRRD               = SystemConfiguration.MemoryTrrd;
      SaPlatformPolicyPpi->MemConfig->tWTR               = SystemConfiguration.MemoryTwtr;
      SaPlatformPolicyPpi->MemConfig->tRTP               = SystemConfiguration.MemoryTrtp;
      SaPlatformPolicyPpi->MemConfig->tRC                = SystemConfiguration.MemoryTrc;
      SaPlatformPolicyPpi->MemConfig->tFAW               = SystemConfiguration.MemoryTfaw;
      SaPlatformPolicyPpi->MemConfig->tCWL               = SystemConfiguration.MemoryTcwl;
      SaPlatformPolicyPpi->MemConfig->tREFI              = SystemConfiguration.MemoryTrefi;
      SaPlatformPolicyPpi->MemConfig->tRPab              = SystemConfiguration.MemoryTrpab;
      SaPlatformPolicyPpi->MemConfig->NModeSupport       = SystemConfiguration.NModeSupport;
    }
  }
//[-end-121101-IB03780468-add]//
//[-start-121128-IB03780468-remove]//
//  ///
//  /// Initialize the Overclocking Configuration
//  ///
////[-start-120627-IB05330350-modify]//
//  SaPlatformPolicyPpi->OcConfig->GtVoltageOffset     = PcdGet16 (PcdOcConfigGtVoltageOffset);
//  SaPlatformPolicyPpi->OcConfig->GtVoltageOverride   = PcdGet16 (PcdOcConfigGtVoltageOverride);
//  SaPlatformPolicyPpi->OcConfig->GtExtraTurboVoltage = PcdGet16 (PcdOcConfigGtExtraTurboVoltage);
//  SaPlatformPolicyPpi->OcConfig->GtMaxOcTurboRatio   = PcdGet16 (PcdOcConfigGtMaxOcTurboRatio);
//  SaPlatformPolicyPpi->OcConfig->SaVoltageOffset     = PcdGet16 (PcdOcConfigSaVoltageOffset);
////[-end-120627-IB05330350-modify]//
//[-end-121128-IB03780468-remove]//
//[-start-120627-IB05330350-add]//
//[-start-121128-IB03780468-remove]//
//  SaPlatformPolicyPpi->OcConfig->GtVoltageMode       = PcdGet8 (PcdOcConfigGtVoltageMode);
//  SaPlatformPolicyPpi->OcConfig->OcSupport           = PcdGet8 (PcdOcConfigOcSupport);
////[-start-121009-IB03780464-add]//
//  SaPlatformPolicyPpi->OcConfig->IoaVoltageOffset    = PcdGet16 (PcdOcConfigIoaVoltageOffset);
//  SaPlatformPolicyPpi->OcConfig->IodVoltageOffset    = PcdGet16 (PcdOcConfigIodVoltageOffset);
////[-end-121009-IB03780464-add]//
//[-end-121128-IB03780468-remove]//
//[-start-140219-IB10920078-modify]//
//[-start-130218-IB07250301-remove]//
  if (FeaturePcdGet (PcdIntelSwitchableGraphicsSupported)) { 
    ///
    /// Initialize the Switchable Graphics Configuration
    ///
    ///
    /// Switchable Graphics mode set as MUXLESS (By default)
    ///
    SaPlatformPolicyPpi->PlatformData->SgMode = PcdGet8 (PcdPlatformDataSgMode);

    //
    // System Configuration variables to set SG mode default value, 
    // 0 = SgModeDisabled, 1 = SgModeMuxed, 2 = SgModeMuxless, 3 = SgModeDgpu.
    //
    SaPlatformPolicyPpi->PlatformData->SgMode = (UINT8)SetSgModeValue (SystemConfiguration);

    //
    // Base on SG related System Configuration variables to change SA MCH policy.
    //
    SetSgRelatedSaPlatformPolicy (SaPlatformPolicyPpi, SystemConfiguration);
      
    SaPlatformPolicyPpi->PlatformData->SgSubSystemId = PcdGet16 (PcdPlatformDataSgSubSystemId);

    ///
    /// Configure below based on the OEM platfrom design
    /// Switchable Graphics GPIO support - 1=Supported 0=Not Supported
    ///
    SaPlatformPolicyPpi->SgGpioData->GpioSupport = PcdGetBool (PcdSgGpioDataGpioSupport);

    if (SaPlatformPolicyPpi->SgGpioData->GpioSupport) {
      ///
      /// Initialzie the GPIO Configuration
      ///
      ///
      /// dGPU PWROK GPIO assigned
      ///
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrOK = (SA_GPIO_INFO *) AllocatePool (sizeof (SA_GPIO_INFO));
      ASSERT (SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrOK != NULL);
      if (SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrOK == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrOK->Value = PcdGet8 (PcdSgGpioDataSgDgpuPwrOKValue);
      ///
      /// dGPU PWROK Active High
      ///
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrOK->Active = PcdGetBool (PcdSgGpioDataSgDgpuPwrOKActive);

      ///
      /// dGPU HLD RST GPIO assigned
      ///
      SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst = (SA_GPIO_INFO *) AllocatePool (sizeof (SA_GPIO_INFO));
      ASSERT (SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst != NULL);
      if (SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst->Value = PcdGet8 (PcdSgGpioDataSgDgpuHoldRstValue);
      ///
      /// dGPU HLD RST Active Low
      ///
      SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst->Active = PcdGetBool (PcdSgGpioDataSgDgpuHoldRstActive);

      if (CpuFamilyId == EnumCpuHswUlt) {
        SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst->Value = 48;
     }

      ///
      /// dGPU PWR Enable GPIO assigned
      ///
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable = (SA_GPIO_INFO *) AllocatePool (sizeof (SA_GPIO_INFO));
      ASSERT (SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable != NULL);
      if (SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable->Value = PcdGet8 (PcdSgGpioDataSgDgpuPwrEnableValue);
      ///
      /// dGPU PWR Enable Active Low
      ///
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable->Active = PcdGetBool (PcdSgGpioDataSgDgpuPwrEnableActive);

      if (CpuFamilyId == EnumCpuHswUlt) {
        SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable->Value = 84;
      }

      ///
      /// dGPU_PRSNT# GPIO assigned
      ///
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPrsnt = (SA_GPIO_INFO *) AllocatePool (sizeof (SA_GPIO_INFO));
      ASSERT (SaPlatformPolicyPpi->SgGpioData->SgDgpuPrsnt != NULL);
      if (SaPlatformPolicyPpi->SgGpioData->SgDgpuPrsnt == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPrsnt->Value = PcdGet8 (PcdSgGpioDataSgDgpuPrsntValue);
      ///
      /// dGPU_PRSNT# Active Low
      ///
      SaPlatformPolicyPpi->SgGpioData->SgDgpuPrsnt->Active = PcdGetBool (PcdSgGpioDataSgDgpuPrsntActive);

      SaPlatformPolicyPpi->SgGpioData = SaPlatformPolicyPpi->SgGpioData;
    }
  }
//[-end-130218-IB07250301-remove]//
//[-end-140219-IB10920078-modify]//
//[-end-120627-IB05330350-add]//
//[-start-121205-IB06460473-modify]//
  Status = PeiServicesGetBootMode (&SysBootMode);
  ASSERT_EFI_ERROR (Status);

  if (SysBootMode != BOOT_IN_RECOVERY_MODE) {
    //
    // Initialize the DataPtr for S3 resume
    //
    MrcS3RestoreDataPtr = NULL;
    VariableSize        = 0;    // Set VariableSize as 0 at first time calling GetVariable to get the correct variable data size
    Status = Variable->GetVariable (
                         Variable,
                         L"MrcS3RestoreVariable",
                         &gMrcS3RestoreVariableGuid,
                         NULL,
                         &VariableSize,
                         MrcS3RestoreDataPtr
                         );
    if ((Status == EFI_BUFFER_TOO_SMALL) && (VariableSize != 0)) {
      MrcS3RestoreDataPtr = (VOID *)AllocateZeroPool (VariableSize);
      ASSERT (MrcS3RestoreDataPtr != NULL);

      Status = Variable->GetVariable (
                           Variable,
                           L"MrcS3RestoreVariable",
                           &gMrcS3RestoreVariableGuid,
                           NULL,
                           &VariableSize,
                           MrcS3RestoreDataPtr
                           );
      ASSERT_EFI_ERROR (Status);
    }
    if (MrcS3RestoreDataPtr == NULL) {
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "MrcS3RestoreVariable does not be found!!\n"));
    }
    SaPlatformPolicyPpi->S3DataPtr                                          = MrcS3RestoreDataPtr;

    //
    // To force MrcBootMode be bmCold.
    //
    if (FeaturePcdGet (PcdMrcRmt)) {
      SaPlatformPolicyPpi->S3DataPtr                                          = NULL;
    }
  } else {
    SaPlatformPolicyPpi->S3DataPtr                                          = NULL;
  }
//[-end-121205-IB06460473-modify]//
//[-start-120404-IB05300309-remove]//
//  //
//  // Initialize 16bit CMOS location for scrambling seed storage when RapidStart is enabled
//  //
//  SaPlatformPolicyPpi->ScramblerSeedCmosLocation                          = ScramblerSeedOffset;
//[-end-120404-IB05300309-remove]//

  //
  // Update SA Policy PPI Descriptor
  //
  SaPlatformPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  SaPlatformPolicyPpiDesc->Guid  = &gSaPlatformPolicyPpiGuid;
  SaPlatformPolicyPpiDesc->Ppi   = SaPlatformPolicyPpi;

//[-start-121226-IB10870063-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateSaPlatformPolicy (SaPlatformPolicyPpi, SaPlatformPolicyPpiDesc);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcUpdateSaPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Installing SA Policy (PEI).\n"));
    //
    // Install SA PEI Platform Policy PPI
    //
    Status = PeiServicesInstallPpi (SaPlatformPolicyPpiDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121226-IB10870063-modify]//

  //
  // Dump policy
  //
  DumpSAPolicyPEI (PeiServices, SaPlatformPolicyPpi);

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SA Policy PEI Exit\n"));

  return Status;
}

VOID
DumpSAPolicyPEI (
  IN      CONST EFI_PEI_SERVICES        **PeiServices,
  IN      SA_PLATFORM_POLICY_PPI        *SaPlatformPolicyPpi
 )
{
  UINTN        Count;

  Count = 0;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "SaPlatformPolicyPpi (Address : 0x%x)\n", SaPlatformPolicyPpi));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Revision                          : %x\n", SaPlatformPolicyPpi->Revision));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-PlatformData (Address : 0x%x)\n", SaPlatformPolicyPpi->PlatformData));
//[-start-120404-IB05300309-modify]//
  for (Count = 0 ; Count < SA_MC_MAX_SOCKETS ; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SpdAddressTable[%2x]            : %x\n", Count, SaPlatformPolicyPpi->PlatformData->SpdAddressTable[Count]));
  }
//[-end-120404-IB05300309-modify]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MchBar                         : %x\n", SaPlatformPolicyPpi->PlatformData->MchBar));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiBar                         : %x\n", SaPlatformPolicyPpi->PlatformData->DmiBar));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EpBar                          : %x\n", SaPlatformPolicyPpi->PlatformData->EpBar));
//[-start-120404-IB05300309-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EdramBar                       : %x\n", SaPlatformPolicyPpi->PlatformData->EdramBar));
//[-end-120404-IB05300309-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PciExpressBar                  : %x\n", SaPlatformPolicyPpi->PlatformData->PciExpressBar));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SmbusBar                       : %x\n", SaPlatformPolicyPpi->PlatformData->SmbusBar));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GdxcBar                        : %x\n", SaPlatformPolicyPpi->PlatformData->GdxcBar));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-TsegSize                       : %x\n", SaPlatformPolicyPpi->PlatformData->TsegSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IedSize                        : %x\n", SaPlatformPolicyPpi->PlatformData->IedSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-UserBd                         : %x\n", SaPlatformPolicyPpi->PlatformData->UserBd));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FastBoot                       : %x\n", SaPlatformPolicyPpi->PlatformData->FastBoot));
//[-start-120627-IB05330350-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-BoardId                        : %x\n", SaPlatformPolicyPpi->PlatformData->BoardId));  
//[-start-130218-IB07250301-remove]//
//#ifdef SWITCHABLE_GRAPHICS_SUPPORT
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SgMode                       : %x\n", SaPlatformPolicyPpi->PlatformData->SgMode));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SgModeDisabled                       : %x\n", SaPlatformPolicyPpi->PlatformData->SgModeDisabled));
//#endif
//[-end-130218-IB07250301-remove]//
//[-end-120627-IB05330350-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-GtConfig (Address : 0x%x)\n", SaPlatformPolicyPpi->GtConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MmioSize                       : %x\n", SaPlatformPolicyPpi->GtConfig->MmioSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GttSize                        : %x\n", SaPlatformPolicyPpi->GtConfig->GttSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IgdDvmt50PreAlloc              : %x\n", SaPlatformPolicyPpi->GtConfig->IgdDvmt50PreAlloc));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-InternalGraphics               : %x\n", SaPlatformPolicyPpi->GtConfig->InternalGraphics));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PrimaryDisplay                 : %x\n", SaPlatformPolicyPpi->GtConfig->PrimaryDisplay));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ApertureSize                   : %x\n", SaPlatformPolicyPpi->GtConfig->ApertureSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PanelPowerEnable               : %x\n", SaPlatformPolicyPpi->GtConfig->PanelPowerEnable));
//[-start-121023-IB03780466-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GttMmAdr                       : %x\n", SaPlatformPolicyPpi->GtConfig->GttMmAdr));
//[-end-121023-IB03780466-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-MemConfig (Address : 0x%x)\n", SaPlatformPolicyPpi->MemConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EccSupport                     : %x\n", SaPlatformPolicyPpi->MemConfig->EccSupport));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DdrFreqLimit                   : %x\n", SaPlatformPolicyPpi->MemConfig->DdrFreqLimit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MaxTolud                       : %x\n", SaPlatformPolicyPpi->MemConfig->MaxTolud));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SpdProfileSelected             : %x\n", SaPlatformPolicyPpi->MemConfig->SpdProfileSelected));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tCL                            : %x\n", SaPlatformPolicyPpi->MemConfig->tCL));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tRCD                           : %x\n", SaPlatformPolicyPpi->MemConfig->tRCD));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tRP                            : %x\n", SaPlatformPolicyPpi->MemConfig->tRP));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tRAS                           : %x\n", SaPlatformPolicyPpi->MemConfig->tRAS));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-NModeSupport                   : %x\n", SaPlatformPolicyPpi->MemConfig->NModeSupport));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ScramblerSupport               : %x\n", SaPlatformPolicyPpi->MemConfig->ScramblerSupport));
//[-start-121009-IB03780464-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PowerDownMode                  : %x\n", SaPlatformPolicyPpi->MemConfig->PowerDownMode));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PwdwnIdleCounter               : %x\n", SaPlatformPolicyPpi->MemConfig->PwdwnIdleCounter));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RankInterleave                 : %x\n", SaPlatformPolicyPpi->MemConfig->RankInterleave));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnhancedInterleave             : %x\n", SaPlatformPolicyPpi->MemConfig->EnhancedInterleave));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WeaklockEn                     : %x\n", SaPlatformPolicyPpi->MemConfig->WeaklockEn));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnCmdRate                      : %x\n", SaPlatformPolicyPpi->MemConfig->EnCmdRate));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CmdTriStateDis                 : %x\n", SaPlatformPolicyPpi->MemConfig->CmdTriStateDis));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RefreshRate2x                  : %x\n", SaPlatformPolicyPpi->MemConfig->RefreshRate2x));
//[-end-121009-IB03780464-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tWR                            : %x\n", SaPlatformPolicyPpi->MemConfig->tWR));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tRFC                           : %x\n", SaPlatformPolicyPpi->MemConfig->tRFC));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tRRD                           : %x\n", SaPlatformPolicyPpi->MemConfig->tRRD));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tWTR                           : %x\n", SaPlatformPolicyPpi->MemConfig->tWTR));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tRTP                           : %x\n", SaPlatformPolicyPpi->MemConfig->tRTP));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tRC                            : %x\n", SaPlatformPolicyPpi->MemConfig->tRC));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tFAW                           : %x\n", SaPlatformPolicyPpi->MemConfig->tFAW));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tCWL                           : %x\n", SaPlatformPolicyPpi->MemConfig->tCWL));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tREFI                          : %x\n", SaPlatformPolicyPpi->MemConfig->tREFI));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ThermalManagement              : %x\n", SaPlatformPolicyPpi->MemConfig->ThermalManagement)); 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PeciInjectedTemp               : %x\n", SaPlatformPolicyPpi->MemConfig->PeciInjectedTemp)); 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ExttsViaTsOnBoard              : %x\n", SaPlatformPolicyPpi->MemConfig->ExttsViaTsOnBoard)); 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ExttsViaTsOnDimm               : %x\n", SaPlatformPolicyPpi->MemConfig->ExttsViaTsOnDimm)); 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VirtualTempSensor              : %x\n", SaPlatformPolicyPpi->MemConfig->VirtualTempSensor)); 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ForceColdReset                 : %x\n", SaPlatformPolicyPpi->MemConfig->ForceColdReset)); 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DisableDimmChannel[0]          : %x\n", SaPlatformPolicyPpi->MemConfig->DisableDimmChannel[0]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DisableDimmChannel[1]          : %x\n", SaPlatformPolicyPpi->MemConfig->DisableDimmChannel[1]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ChHashEnable                   : %x\n", SaPlatformPolicyPpi->MemConfig->ChHashEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ChHashMask                     : %x\n", SaPlatformPolicyPpi->MemConfig->ChHashMask));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ChHashInterleaveBit            : %x\n", SaPlatformPolicyPpi->MemConfig->ChHashInterleaveBit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnableExtts                    : %x\n", SaPlatformPolicyPpi->MemConfig->EnableExtts));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnableCltm                     : %x\n", SaPlatformPolicyPpi->MemConfig->EnableCltm));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnableOltm                     : %x\n", SaPlatformPolicyPpi->MemConfig->EnableOltm));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnablePwrDn                    : %x\n", SaPlatformPolicyPpi->MemConfig->EnablePwrDn));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnablePwrDnLpddr               : %x\n", SaPlatformPolicyPpi->MemConfig->EnablePwrDnLpddr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Refresh2X                      : %x\n", SaPlatformPolicyPpi->MemConfig->Refresh2X));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-LpddrThermalSensor             : %x\n", SaPlatformPolicyPpi->MemConfig->LpddrThermalSensor));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-LockPTMregs                    : %x\n", SaPlatformPolicyPpi->MemConfig->LockPTMregs));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-UserPowerWeightsEn             : %x\n", SaPlatformPolicyPpi->MemConfig->UserPowerWeightsEn));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnergyScaleFact                : %x\n", SaPlatformPolicyPpi->MemConfig->EnergyScaleFact));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplPwrFlCh1                   : %x\n", SaPlatformPolicyPpi->MemConfig->RaplPwrFlCh1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplPwrFlCh0                   : %x\n", SaPlatformPolicyPpi->MemConfig->RaplPwrFlCh0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim2Lock                   : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim2Lock));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim2WindX                  : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim2WindX));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim2WindY                  : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim2WindY));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim2Ena                    : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim2Ena));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim2Pwr                    : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim2Pwr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim1WindX                  : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim1WindX));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim1WindY                  : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim1WindY));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim1Ena                    : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim1Ena));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RaplLim1Pwr                    : %x\n", SaPlatformPolicyPpi->MemConfig->RaplLim1Pwr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WarmThresholdCh0Dimm0          : %x\n", SaPlatformPolicyPpi->MemConfig->WarmThresholdCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WarmThresholdCh0Dimm1          : %x\n", SaPlatformPolicyPpi->MemConfig->WarmThresholdCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WarmThresholdCh1Dimm0          : %x\n", SaPlatformPolicyPpi->MemConfig->WarmThresholdCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WarmThresholdCh1Dimm1          : %x\n", SaPlatformPolicyPpi->MemConfig->WarmThresholdCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HotThresholdCh0Dimm0           : %x\n", SaPlatformPolicyPpi->MemConfig->HotThresholdCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HotThresholdCh0Dimm1           : %x\n", SaPlatformPolicyPpi->MemConfig->HotThresholdCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HotThresholdCh1Dimm0           : %x\n", SaPlatformPolicyPpi->MemConfig->HotThresholdCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HotThresholdCh1Dimm1           : %x\n", SaPlatformPolicyPpi->MemConfig->HotThresholdCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WarmBudgetCh0Dimm0             : %x\n", SaPlatformPolicyPpi->MemConfig->WarmBudgetCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WarmBudgetCh0Dimm1             : %x\n", SaPlatformPolicyPpi->MemConfig->WarmBudgetCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WarmBudgetCh1Dimm0             : %x\n", SaPlatformPolicyPpi->MemConfig->WarmBudgetCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WarmBudgetCh1Dimm1             : %x\n", SaPlatformPolicyPpi->MemConfig->WarmBudgetCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HotBudgetCh0Dimm0              : %x\n", SaPlatformPolicyPpi->MemConfig->HotBudgetCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HotBudgetCh0Dimm1              : %x\n", SaPlatformPolicyPpi->MemConfig->HotBudgetCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HotBudgetCh1Dimm0              : %x\n", SaPlatformPolicyPpi->MemConfig->HotBudgetCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HotBudgetCh1Dimm1              : %x\n", SaPlatformPolicyPpi->MemConfig->HotBudgetCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IdleEnergyCh0Dimm1             : %x\n", SaPlatformPolicyPpi->MemConfig->IdleEnergyCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IdleEnergyCh0Dimm0             : %x\n", SaPlatformPolicyPpi->MemConfig->IdleEnergyCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PdEnergyCh0Dimm1               : %x\n", SaPlatformPolicyPpi->MemConfig->PdEnergyCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PdEnergyCh0Dimm0               : %x\n", SaPlatformPolicyPpi->MemConfig->PdEnergyCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ActEnergyCh0Dimm1              : %x\n", SaPlatformPolicyPpi->MemConfig->ActEnergyCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ActEnergyCh0Dimm0              : %x\n", SaPlatformPolicyPpi->MemConfig->ActEnergyCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RdEnergyCh0Dimm1               : %x\n", SaPlatformPolicyPpi->MemConfig->RdEnergyCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RdEnergyCh0Dimm0               : %x\n", SaPlatformPolicyPpi->MemConfig->RdEnergyCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WrEnergyCh0Dimm1               : %x\n", SaPlatformPolicyPpi->MemConfig->WrEnergyCh0Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WrEnergyCh0Dimm0               : %x\n", SaPlatformPolicyPpi->MemConfig->WrEnergyCh0Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IdleEnergyCh1Dimm1             : %x\n", SaPlatformPolicyPpi->MemConfig->IdleEnergyCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IdleEnergyCh1Dimm0             : %x\n", SaPlatformPolicyPpi->MemConfig->IdleEnergyCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PdEnergyCh1Dimm1               : %x\n", SaPlatformPolicyPpi->MemConfig->PdEnergyCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PdEnergyCh1Dimm0               : %x\n", SaPlatformPolicyPpi->MemConfig->PdEnergyCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ActEnergyCh1Dimm1              : %x\n", SaPlatformPolicyPpi->MemConfig->ActEnergyCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ActEnergyCh1Dimm0              : %x\n", SaPlatformPolicyPpi->MemConfig->ActEnergyCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RdEnergyCh1Dimm1               : %x\n", SaPlatformPolicyPpi->MemConfig->RdEnergyCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RdEnergyCh1Dimm0               : %x\n", SaPlatformPolicyPpi->MemConfig->RdEnergyCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WrEnergyCh1Dimm1               : %x\n", SaPlatformPolicyPpi->MemConfig->WrEnergyCh1Dimm1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WrEnergyCh1Dimm0               : %x\n", SaPlatformPolicyPpi->MemConfig->WrEnergyCh1Dimm0));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SrefCfgEna                     : %x\n", SaPlatformPolicyPpi->MemConfig->SrefCfgEna));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SrefCfgIdleTmr                 : %x\n", SaPlatformPolicyPpi->MemConfig->SrefCfgIdleTmr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ThrtCkeMinDefeat               : %x\n", SaPlatformPolicyPpi->MemConfig->ThrtCkeMinDefeat));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ThrtCkeMinTmr                  : %x\n", SaPlatformPolicyPpi->MemConfig->ThrtCkeMinTmr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ThrtCkeMinDefeatLpddr          : %x\n", SaPlatformPolicyPpi->MemConfig->ThrtCkeMinDefeatLpddr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ThrtCkeMinTmrLpddr             : %x\n", SaPlatformPolicyPpi->MemConfig->ThrtCkeMinTmrLpddr));    
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-AutoSelfRefreshSupport         : %x\n", SaPlatformPolicyPpi->MemConfig->AutoSelfRefreshSupport));    
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ExtTemperatureSupport          : %x\n", SaPlatformPolicyPpi->MemConfig->ExtTemperatureSupport));    
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MaxRttWr                       : %x\n", SaPlatformPolicyPpi->MemConfig->MaxRttWr));    
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RCVENC1D                       : %x\n", SaPlatformPolicyPpi->MemConfig->RCVENC1D));    
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RMC                            : %x\n", SaPlatformPolicyPpi->MemConfig->RMC));    
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RemapEnable                    : %x\n", SaPlatformPolicyPpi->MemConfig->RemapEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SerialDebug                    : %x\n", SaPlatformPolicyPpi->MemConfig->SerialDebug));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-McLock                         : %x\n", SaPlatformPolicyPpi->MemConfig->McLock));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GdxcEnable                     : %x\n", SaPlatformPolicyPpi->MemConfig->GdxcEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GdxcIotSize                    : %x\n", SaPlatformPolicyPpi->MemConfig->GdxcIotSize));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GdxcMotSize                    : %x\n", SaPlatformPolicyPpi->MemConfig->GdxcMotSize));    
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ECT                            : %x\n", SaPlatformPolicyPpi->MemConfig->ECT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SOT                            : %x\n", SaPlatformPolicyPpi->MemConfig->SOT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RDMPRT                         : %x\n", SaPlatformPolicyPpi->MemConfig->RDMPRT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RCVET                          : %x\n", SaPlatformPolicyPpi->MemConfig->RCVET));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-JWRL                           : %x\n", SaPlatformPolicyPpi->MemConfig->JWRL));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FWRL                           : %x\n", SaPlatformPolicyPpi->MemConfig->FWRL));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WRTC1D                         : %x\n", SaPlatformPolicyPpi->MemConfig->WRTC1D));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RDTC1D                         : %x\n", SaPlatformPolicyPpi->MemConfig->RDTC1D));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DIMMODTT                       : %x\n", SaPlatformPolicyPpi->MemConfig->DIMMODTT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WRDST                          : %x\n", SaPlatformPolicyPpi->MemConfig->WRDST));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WREQT                          : %x\n", SaPlatformPolicyPpi->MemConfig->WREQT));
//[-start-121023-IB03780466-remove]//
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WRXTCT                         : %x\n", SaPlatformPolicyPpi->MemConfig->WRXTCT));
//[-end-121023-IB03780466-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RDODTT                         : %x\n", SaPlatformPolicyPpi->MemConfig->RDODTT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RDEQT                          : %x\n", SaPlatformPolicyPpi->MemConfig->RDEQT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RDAPT                          : %x\n", SaPlatformPolicyPpi->MemConfig->RDAPT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WRTC2D                         : %x\n", SaPlatformPolicyPpi->MemConfig->WRTC2D));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RDTC2D                         : %x\n", SaPlatformPolicyPpi->MemConfig->RDTC2D));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WRVC2D                         : %x\n", SaPlatformPolicyPpi->MemConfig->WRVC2D));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RDVC2D                         : %x\n", SaPlatformPolicyPpi->MemConfig->RDVC2D));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-B2BXTT                         : %x\n", SaPlatformPolicyPpi->MemConfig->B2BXTT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-C2CXTT                         : %x\n", SaPlatformPolicyPpi->MemConfig->C2CXTT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-LCT                            : %x\n", SaPlatformPolicyPpi->MemConfig->LCT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RTL                            : %x\n", SaPlatformPolicyPpi->MemConfig->RTL));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-TAT                            : %x\n", SaPlatformPolicyPpi->MemConfig->TAT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RMT                            : %x\n", SaPlatformPolicyPpi->MemConfig->RMT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MEMTST                         : %x\n", SaPlatformPolicyPpi->MemConfig->MEMTST));
//[-start-120404-IB05300309-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DIMMODTT1D                     : %x\n", SaPlatformPolicyPpi->MemConfig->DIMMODTT1D));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-WRSRT                          : %x\n", SaPlatformPolicyPpi->MemConfig->WRSRT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DIMMRONT                       : %x\n", SaPlatformPolicyPpi->MemConfig->DIMMRONT));
//[-end-120404-IB05300309-add]//
//[-start-121023-IB03780466-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-tRPab                          : %x\n", SaPlatformPolicyPpi->MemConfig->tRPab));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ALIASCHK                       : %x\n", SaPlatformPolicyPpi->MemConfig->ALIASCHK));
//[-end-121023-IB03780466-add]//
//[-start-121113-IB06150257-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MemoryTrace                    : %x\n", SaPlatformPolicyPpi->MemConfig->MemoryTrace));
//[-end-121113-IB06150257-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DDR3Voltage                    : %x\n", SaPlatformPolicyPpi->MemConfig->DDR3Voltage));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DDR3VoltageWaitTime            : %x\n", SaPlatformPolicyPpi->MemConfig->DDR3VoltageWaitTime));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RmtBdatEnable                  : %x\n", SaPlatformPolicyPpi->MemConfig->RmtBdatEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RefClk                         : %x\n", SaPlatformPolicyPpi->MemConfig->RefClk));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Ratio                          : %x\n", SaPlatformPolicyPpi->MemConfig->Ratio));
//[-start-121009-IB03780464-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-BClkFrequency                  : %x\n", SaPlatformPolicyPpi->MemConfig->BClkFrequency));
//[-end-121009-IB03780464-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MrcTimeMeasure                 : %x\n", SaPlatformPolicyPpi->MemConfig->MrcTimeMeasure));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MrcFastBoot                    : %x\n", SaPlatformPolicyPpi->MemConfig->MrcFastBoot));  
//[-start-120627-IB05330350-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DqPinsInterleaved              : %x\n", SaPlatformPolicyPpi->MemConfig->DqPinsInterleaved));  
//[-start-121023-IB03780466-remove]//
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MrcUltPoSafeConfig             : %x\n", SaPlatformPolicyPpi->MemConfig->MrcUltPoSafeConfig));  
//[-end-121023-IB03780466-remove]//
//[-end-120627-IB05330350-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-PcieConfig (Address : 0x%x)\n", SaPlatformPolicyPpi->PcieConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiVc1                         : %x\n", SaPlatformPolicyPpi->PcieConfig->DmiVc1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiVcp                         : %x\n", SaPlatformPolicyPpi->PcieConfig->DmiVcp));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiVcm                         : %x\n", SaPlatformPolicyPpi->PcieConfig->DmiVcm));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DmiGen2                        : %x\n", SaPlatformPolicyPpi->PcieConfig->DmiGen2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-AlwaysEnablePeg                : %x\n", SaPlatformPolicyPpi->PcieConfig->AlwaysEnablePeg));
  for (Count = 0; Count < SA_PEG_MAX_FUN; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGenx[%2x]                    : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->PegGenx[Count]));
  }  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3Equalization            : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3Equalization));
  for (Count = 0; Count < SA_PEG_MAX_LANE; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Gen3RootPortPreset[%2x]         : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->Gen3RootPortPreset[Count]));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Gen3EndPointPreset[%2x]         : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->Gen3EndPointPreset[Count]));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Gen3EndPointHint  [%2x]         : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->Gen3EndPointHint[Count]));
  }
//[-start-120627-IB05330350-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR,   " |  |-RxCEMLoopback                   : %x\n", SaPlatformPolicyPpi->PcieConfig->RxCEMLoopback));  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR,   " |  |-RxCEMLoopbackLane               : %x\n", SaPlatformPolicyPpi->PcieConfig->RxCEMLoopbackLane));    
  for (Count = 0; Count < SA_PEG_MAX_BUNDLE; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-Gen3RxCtleP[%2x]         : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->Gen3RxCtleP[Count]));
  }
//[-start-130201-IB05280045-modify]//
//[-start-121211-IB06460476-add]//
  for (Count = 0; Count < SA_PEG_MAX_FUN; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PowerDownUnusedBundles[%2x]     : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->PowerDownUnusedBundles[Count]));
  }
//[-end-121211-IB06460476-add]//
//[-end-130201-IB05280045-modify]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGpioData (Address : 0x%x)\n", Count, SaPlatformPolicyPpi->PcieConfig->PegGpioData));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-GpioSupport                    : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->PegGpioData->GpioSupport));  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SaPegReset->Active             : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset->Active));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SaPegReset->Value              : %x\n", Count, SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset->Value));  
//[-end-120627-IB05330350-add]//
//[-start-110930-IB05300224-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PcieResetDelay                        : %d0 ms\n", SaPlatformPolicyPpi->PcieConfig->PcieResetDelay) );
//[-end-110930-IB05300224-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegSamplerCalibrate            : %x\n", SaPlatformPolicyPpi->PcieConfig->PegSamplerCalibrate));
//[-start-121023-IB03780466-remove]//
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3EqualizationPhase2      : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3EqualizationPhase2));
//[-end-121023-IB03780466-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3PresetSearch            : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch));
//[-start-120404-IB05300309-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3ForcePresetSearch       : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3ForcePresetSearch));
//[-end-120404-IB05300309-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3PresetSearchDwellTime   : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchDwellTime));
//[-start-121023-IB03780466-remove]//
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3PresetSearchMarginSteps : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchMarginSteps));
//[-end-121023-IB03780466-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3PresetSearchStartMargin : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchStartMargin));
//[-start-120404-IB05300309-add]//
//[-start-121023-IB03780466-remove]//
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3PresetSearchVoltageMarginSteps : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchVoltageMarginSteps));
//[-end-121023-IB03780466-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3PresetSearchVoltageStartMargin : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchVoltageStartMargin));
//[-start-121023-IB03780466-remove]//
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3PresetSearchFavorTiming : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchFavorTiming));
//[-end-121023-IB03780466-remove]//
//[-end-120404-IB05300309-add]//
//[-start-120627-IB05330350-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegGen3PresetSearchErrorTarget   : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchErrorTarget));
//[-end-120627-IB05330350-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PegSwingControl                : %x\n", SaPlatformPolicyPpi->PcieConfig->PegSwingControl));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GtVoltageOffset                : %x\n", SaPlatformPolicyPpi->OcConfig->GtVoltageOffset));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GtVoltageOverride              : %x\n", SaPlatformPolicyPpi->OcConfig->GtVoltageOverride));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GtExtraTurboVoltage            : %x\n", SaPlatformPolicyPpi->OcConfig->GtExtraTurboVoltage));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GtMaxOcTurboRatio              : %x\n", SaPlatformPolicyPpi->OcConfig->GtMaxOcTurboRatio));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SaVoltageOffset                : %x\n", SaPlatformPolicyPpi->OcConfig->SaVoltageOffset));
//[-start-120627-IB05330350-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-OcSupport                      : %x\n", SaPlatformPolicyPpi->OcConfig->OcSupport));  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GtVoltageMode                  : %x\n", SaPlatformPolicyPpi->OcConfig->GtVoltageMode));  
//[-end-120627-IB05330350-add]//
//[-start-121009-IB03780464-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IoaVoltageOffset               : %x\n", SaPlatformPolicyPpi->OcConfig->IoaVoltageOffset));  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IodVoltageOffset               : %x\n", SaPlatformPolicyPpi->OcConfig->IodVoltageOffset));  
//[-end-121009-IB03780464-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-S3DataPtr (Address : 0x%x)\n", SaPlatformPolicyPpi->S3DataPtr));
//[-start-120627-IB05330350-add]//
//[-start-130218-IB07250301-remove]//
//#ifdef SWITCHABLE_GRAPHICS_SUPPORT
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SgGpioData (Address : 0x%x)\n", Count, SaPlatformPolicyPpi->SgGpioData));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-GpioSupport                 : %x\n", Count, SaPlatformPolicyPpi->SgGpioData->GpioSupport));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SgDgpuHoldRst               : %x\n", Count, SaPlatformPolicyPpi->SgGpioData->SgDgpuHoldRst));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SgDgpuPrsnt                 : %x\n", Count, SaPlatformPolicyPpi->SgGpioData->SgDgpuPrsnt));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SgDgpuPwrEnable             : %x\n", Count, SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrEnable));
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-SgDgpuPwrOK                 : %x\n", Count, SaPlatformPolicyPpi->SgGpioData->SgDgpuPwrOK));
//#endif
//[-end-130218-IB07250301-remove]//
//[-end-120627-IB05330350-add]//
//[-start-120404-IB05300309-remove]//
//  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-ScramblerSeedCmosLocation         : %x\n", SaPlatformPolicyPpi->ScramblerSeedCmosLocation));
//[-end-120404-IB05300309-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));

  return;
}

//[-start-121204-IB05300366-modify]//
MrcFrequency
CalculateFrequencyLimit (
  CHIPSET_CONFIGURATION              SystemConfiguration
 )
{
  MrcFrequency                      FrequencyLimit;

  FrequencyLimit = 0;

  //
  // SystemConfiguration->MemoryFrequency :
  //
  //   0 - Auto
  //   1 - 800
  //   2 - 1000
  //   3 - 1067
  //   4 - 1200
  //   5 - 1333
  //   6 - 1400
  //   7 - 1600
  //   8 - 1800
  //   9 - 1867
  //  10 - 2000
  //  11 - 2133
  //  12 - 2200
  //  13 - 2400
  //  14 - 2600
  //  15 - 2667
  //
  switch (SystemConfiguration.MemoryFrequency) {    
  case 1:
    FrequencyLimit = f800;
    break;

  case 2:
    FrequencyLimit = f1000;
    break;

  case 3:
    FrequencyLimit = f1067;
    break;

  case 4:
    FrequencyLimit = f1200;
    break;

  case 5:
    FrequencyLimit = f1333;
    break;

  case 6:
    FrequencyLimit = f1400;
    break;

  case 7:
    FrequencyLimit = f1600;
    break;

  case 8:
    FrequencyLimit = f1800;
    break;

  case 9:
    FrequencyLimit = f1867;
    break;

  case 10:
    FrequencyLimit = f2000;
    break;

  case 11:
    FrequencyLimit = f2133;
    break;

  case 12:
    FrequencyLimit = f2200;
    break;

  case 13:
    FrequencyLimit = f2400;
    break;

  case 14:
    FrequencyLimit = f2600;
    break;

  case 0:
  case 15:
  default:
    FrequencyLimit = f2667;
    break;
  }

  return FrequencyLimit;
}
//[-end-121204-IB05300366-modify]//

//[-start-121128-IB03780468-add]//
EFI_STATUS
OcInitCallback (
  IN  EFI_PEI_SERVICES        **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN  VOID                          *Ppi
  )
{
  EFI_STATUS                        Status;
  WDT_PPI                           *WdtPpi;

  //
  // Disable WDT when CPU/SA RC complete OC initialization.
  //
  Status = PeiServicesLocatePpi (
                             &gWdtPpiGuid,
                             0,
                             NULL,
                             (VOID **)&WdtPpi
                             );
  ASSERT_EFI_ERROR (Status);

  WdtPpi->Disable ();

  return EFI_SUCCESS;
}
//[-end-121128-IB03780468-add]//

