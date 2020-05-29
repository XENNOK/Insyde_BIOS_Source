/** @file
  This module is for Switchable Graphics Feature PEI pre-initialize.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SwitchableGraphicsPei.h>

//
// Function Prototypes
//
STATIC
EFI_STATUS
SwitchableGraphicsPeiInitialize (
  IN OUT SA_PLATFORM_POLICY_PPI               **SaPlatformPolicyPpi,
  IN OUT CHIPSET_CONFIGURATION                 *SystemConfiguration
  );

STATIC
VOID
SetSgRelatedSaPlatformPolicy (
  IN OUT SA_PLATFORM_POLICY_PPI               *SaPlatformPolicyPpi,
  IN CHIPSET_CONFIGURATION                     SystemConfiguration
  );

STATIC
EFI_STATUS
CreateSgInfoDataHob (
  IN OUT SG_INFORMATION_DATA_HOB              **SgInfoDataHob,
  IN CHIPSET_CONFIGURATION                     SystemConfiguration
  );

STATIC
SG_MODE_SETTING
SetSgModeValue (
  IN CHIPSET_CONFIGURATION                     SystemConfiguration
  );

STATIC
VOID
MxmDgpuPowerEnableSequence (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              SgPpi,
  IN OUT SG_INFORMATION_DATA_HOB              *SgInfoDataHob
  );

//
// PEI Switchable Graphics PPI Function Prototypes
//
BOOLEAN
EFIAPI
GpioRead (
  IN UINT8                                    Value,
  IN BOOLEAN                                  IsUlt
  );

VOID
EFIAPI
GpioWrite (
  IN UINT8                                    Value,
  IN BOOLEAN                                  Level,
  IN BOOLEAN                                  IsUlt
  );

EFI_STATUS
EFIAPI
Stall (
  IN UINTN                                    Microseconds
  );

//
// Module globals
//
STATIC H2O_SWITCHABLE_GRAPHICS_PPI            mH2OSwitchableGraphicsPpi = {
  GpioRead,
  GpioWrite,
  Stall
};

STATIC EFI_PEI_PPI_DESCRIPTOR                 mPpiListSwitchableGraphics = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gH2OSwitchableGraphicsPpiGuid,
  &mH2OSwitchableGraphicsPpi
};

/**

  Switchable Graphics feature PEIM entry point.
  This module will modify SG related SA MCH policy, set SG mode and power enable the discrete GPU.

  @param[in] FileHandle     Handle of the file being invoked.
  @param[in] PeiServices    It's a general purpose services available to every PEIM.

  @retval EFI_SUCCESS       Switchable Graphics feature PEI initialized successfully.
  @retval EFI_UNSUPPORTED   Switchable Graphics feature doesn't be supported.

**/
EFI_STATUS
SwitchableGraphicsPeiInitEntry (
  IN EFI_PEI_FILE_HANDLE                      FileHandle,
  IN CONST EFI_PEI_SERVICES                   **PeiServices
  )
{
//[-start-130710-IB05160465-modify]//
  EFI_STATUS                                  Status;
  SA_PLATFORM_POLICY_PPI                      *SaPlatformPolicyPpi;
  SG_INFORMATION_DATA_HOB                     *SgInfoDataHob;
  VOID                                        *SystemConfiguration;

  //
  // Install Switchable Graphics PPI for module dependency,
  // Switchable Graphics PEI module must be dispatched before SA initialize PEI module,
  // so must add dependency in SA initialize PEI module.
  //
  PeiServicesInstallPpi (&mPpiListSwitchableGraphics);

  if (FeaturePcdGet (PcdSwitchableGraphicsSupported)) {

    SystemConfiguration = AllocateZeroPool (PcdGet32 (PcdSetupConfigSize));
    
    //
    // Get Setup Variable and SA platform policy PPI.
    //
    Status = SwitchableGraphicsPeiInitialize (&SaPlatformPolicyPpi, SystemConfiguration);
    if (EFI_ERROR (Status)) {
     	return Status;
    }

    //
    // Base on SG related System Configuration variables to change SA MCH policy.
    //
    SetSgRelatedSaPlatformPolicy (SaPlatformPolicyPpi, *(CHIPSET_CONFIGURATION *)SystemConfiguration);

    //
    // Create Switchable Graphics information data HOB 
    //
    CreateSgInfoDataHob (&SgInfoDataHob, *(CHIPSET_CONFIGURATION *)SystemConfiguration);

    //
    // Base on SG mode to power enable the discrete GPU or not.
    //
    MxmDgpuPowerEnableSequence (mH2OSwitchableGraphicsPpi, SgInfoDataHob);
//[-end-130710-IB05160465-modify]//

    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}

/**

  Switchable Graphics feature PEIM initialize function.
  Prepare variables and PPI services this module needed.

  @param[in, out] SaPlatformPolicyPpi   A double pointer of SA platform policy PPI to access the GTC
                                        (Graphics Translation Configuration) related information.
  @param[in, out] SystemConfiguration   It's a pointer to setup variables (system configuration).

  @retval EFI_SUCCESS    Get system configuration and related PPI successfully.
  @retval !EFI_SUCCESS   Get variable or PPI service failed.

**/
STATIC
EFI_STATUS
SwitchableGraphicsPeiInitialize (
  IN OUT SA_PLATFORM_POLICY_PPI               **SaPlatformPolicyPpi,
  IN OUT CHIPSET_CONFIGURATION                 *SystemConfiguration
  )
{
  EFI_PEI_READ_ONLY_VARIABLE2_PPI             *VariableServices;
  EFI_STATUS                                  Status;
  UINTN                                       VariableSize;

  //
  // Locate PEI Read Only Variable PPI
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&VariableServices
             );
  if (EFI_ERROR (Status)) {
   	return Status;
  }

  //
  // Get Setup Variable
  //
//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  Status = VariableServices->GetVariable (
                                VariableServices,
                                L"Setup",
                                &gSystemConfigurationGuid,
                                NULL,
                                &VariableSize,
                                SystemConfiguration
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Locate SA platform Policy PPI
  //
  Status = PeiServicesLocatePpi (
             &gSaPlatformPolicyPpiGuid,
             0,
             NULL,
             SaPlatformPolicyPpi
             );
  if (EFI_ERROR (Status)) {
   	return Status;
  }

  return EFI_SUCCESS;
}

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

  Base on PCD variables to set SG related information data, then through HOB pass to DXE driver.

  @param[in, out] SgInfoDataHob   A double pointer of SG information data HOB.
  @param[in] SystemConfiguration   It's the setup variables (system configuration).

  @retval EFI_SUCCESS    Created SG information data HOB and initialized successfully.
  @retval !EFI_SUCCESS   Created SG information data HOB failed.

**/
STATIC
EFI_STATUS
CreateSgInfoDataHob (
  IN OUT SG_INFORMATION_DATA_HOB              **SgInfoDataHob,
  IN CHIPSET_CONFIGURATION                     SystemConfiguration
  )
{
  EFI_STATUS                                  Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI             *VariableServices;
  UINTN                                       VariableSize;
  UINT8                                       BootType;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&VariableServices
             );
  if (EFI_ERROR (Status)) {
   	return Status;
  }

  Status = PeiServicesCreateHob (
             EFI_HOB_TYPE_GUID_EXTENSION,
             sizeof (SG_INFORMATION_DATA_HOB),
             SgInfoDataHob
             );
  if (EFI_ERROR (Status)) {
   	return Status;
  }

  (**SgInfoDataHob).EfiHobGuidType.Name = gH2OSgInformationDataHobGuid;
  (**SgInfoDataHob).SgGpioSupport   = PcdGetBool (PcdSgGpioSupport);
  (**SgInfoDataHob).SgDgpuPwrOk     = PcdGet8 (PcdSgDgpuPwrOkGpioPinValue);
  (**SgInfoDataHob).SgDgpuHoldRst   = PcdGet8 (PcdSgDgpuHoldRstGpioPinValue);
  (**SgInfoDataHob).SgDgpuEdidSel   = PcdGet8 (PcdSgDgpuEdidSelGpioPinValue);
  (**SgInfoDataHob).SgDgpuSel       = PcdGet8 (PcdSgDgpuSelGpioPinValue);
  (**SgInfoDataHob).SgDgpuPwmSel    = PcdGet8 (PcdSgDgpuPwmSelGpioPinValue);
  (**SgInfoDataHob).SgDgpuPwrEnable = PcdGet8 (PcdSgDgpuPwrEnableGpioPinValue);
  (**SgInfoDataHob).SgDgpuPrsnt     = PcdGet8 (PcdSgDgpuPrsntGpioPinValue);
  (**SgInfoDataHob).SgDgpuPwrOk     |= (PcdGetBool (PcdSgDgpuPwrOkGpioPinActive) << 7);
  (**SgInfoDataHob).SgDgpuHoldRst   |= (PcdGetBool (PcdSgDgpuHoldRstGpioPinActive) << 7);
  (**SgInfoDataHob).SgDgpuEdidSel   |= (PcdGetBool (PcdSgDgpuEdidSelGpioPinActive) << 7);
  (**SgInfoDataHob).SgDgpuSel       |= (PcdGetBool (PcdSgDgpuSelGpioPinActive) << 7);
  (**SgInfoDataHob).SgDgpuPwmSel    |= (PcdGetBool (PcdSgDgpuPwmSelGpioPinActive) << 7);
  (**SgInfoDataHob).SgDgpuPwrEnable |= (PcdGetBool (PcdSgDgpuPwrEnableGpioPinActive) << 7);
  (**SgInfoDataHob).SgDgpuPrsnt     |= (PcdGetBool (PcdSgDgpuPrsntGpioPinActive) << 7);
  (**SgInfoDataHob).ActiveDgpuPwrEnableDelay = PcdGet16 (PcdSgActiveDgpuPwrEnableDelay);
  (**SgInfoDataHob).ActiveDgpuHoldRstDelay   = PcdGet16 (PcdSgActiveDgpuHoldRstDelay);
  (**SgInfoDataHob).InactiveDgpuHoldRstDelay = PcdGet16 (PcdSgInactiveDgpuHoldRstDelay);
  (**SgInfoDataHob).SgDgpuEdidSelExist = PcdGetBool (PcdSgDgpuEdidSelGpioPinExist);
  (**SgInfoDataHob).SgDgpuSelExist     = PcdGetBool (PcdSgDgpuSelGpioPinExist);
  (**SgInfoDataHob).SgDgpuPwmSelExist  = PcdGetBool (PcdSgDgpuPwmSelGpioPinExist);
  (**SgInfoDataHob).SgDgpuPrsntExist   = PcdGetBool (PcdSgDgpuPrsntGpioPinExist);
  (**SgInfoDataHob).SlaveMxmGpuSupport = PcdGetBool (PcdSgSlaveMxmGpuSupport);
  (**SgInfoDataHob).SgSlaveDgpuPwrOk     = PcdGet8 (PcdSgSlaveDgpuPwrOkGpioPinValue);
  (**SgInfoDataHob).SgSlaveDgpuHoldRst   = PcdGet8 (PcdSgSlaveDgpuHoldRstGpioPinValue);
  (**SgInfoDataHob).SgSlaveDgpuPwrEnable = PcdGet8 (PcdSgSlaveDgpuPwrEnableGpioPinValue);
  (**SgInfoDataHob).SgSlaveDgpuPrsnt     = PcdGet8 (PcdSgSlaveDgpuPrsntGpioPinValue);
  (**SgInfoDataHob).SgSlaveDgpuPwrOk     |= (PcdGetBool (PcdSgSlaveDgpuPwrOkGpioPinActive) << 7);
  (**SgInfoDataHob).SgSlaveDgpuHoldRst   |= (PcdGetBool (PcdSgSlaveDgpuHoldRstGpioPinActive) << 7);
  (**SgInfoDataHob).SgSlaveDgpuPwrEnable |= (PcdGetBool (PcdSgSlaveDgpuPwrEnableGpioPinActive) << 7);
  (**SgInfoDataHob).SgSlaveDgpuPrsnt     |= (PcdGetBool (PcdSgSlaveDgpuPrsntGpioPinActive) << 7);
  (**SgInfoDataHob).SgSlaveDgpuPrsntExist = PcdGetBool (PcdSgSlaveDgpuPrsntGpioPinExist);
  (**SgInfoDataHob).DgpuBridgeBus           = PcdGet8 (PcdSgPegBridgeBus);
  (**SgInfoDataHob).DgpuBridgeDevice        = PcdGet8 (PcdSgPegBridgeDevice);
  (**SgInfoDataHob).DgpuBridgeFunction      = PcdGet8 (PcdSgPegBridgeFunction);
  (**SgInfoDataHob).SlaveDgpuBridgeBus      = PcdGet8 (PcdSgSlaveDgpuBridgeBus);
  (**SgInfoDataHob).SlaveDgpuBridgeDevice   = PcdGet8 (PcdSgSlaveDgpuBridgeDevice);
  (**SgInfoDataHob).SlaveDgpuBridgeFunction = PcdGet8 (PcdSgSlaveDgpuBridgeFunction);
  (**SgInfoDataHob).AmdSecondaryGrcphicsCommandRegister    = PcdGet8 (PcdAmdSecondaryGrcphicsCommandRegister);
  (**SgInfoDataHob).NvidiaSecondaryGrcphicsCommandRegister = PcdGet8 (PcdNvidiaSecondaryGrcphicsCommandRegister);
  (**SgInfoDataHob).OptimusDgpuHotPlugSupport      = PcdGetBool (PcdSgNvidiaOptimusDgpuHotPlugSupport);
  (**SgInfoDataHob).OptimusDgpuPowerControlSupport = PcdGetBool (PcdSgNvidiaOptimusDgpuPowerControlSupport);
  (**SgInfoDataHob).GpsFeatureSupport              = PcdGetBool (PcdSgNvidiaGpsFeatureSupport);
  (**SgInfoDataHob).VenturaFeatureSupport          = PcdGetBool (PcdSgNvidiaVenturaFeatureSupport);
  (**SgInfoDataHob).OptimusGc6FeatureSupport       = PcdGetBool (PcdSgNvidiaOptimusGc6FeatureSupport);
  CopyMem (&((**SgInfoDataHob).SgModeMxmBinaryGuid), PcdGetPtr (PcdSgModeMxmBinaryGuid), sizeof (EFI_GUID));
  CopyMem (&((**SgInfoDataHob).PegModeMasterMxmBinaryGuid), PcdGetPtr (PcdPegModeMasterMxmBinaryGuid), sizeof (EFI_GUID));
  CopyMem (&((**SgInfoDataHob).PegModeSlaveMxmBinaryGuid), PcdGetPtr (PcdPegModeSlaveMxmBinaryGuid), sizeof (EFI_GUID));
  CopyMem (&((**SgInfoDataHob).AmdDiscreteSsdtGuid), PcdGetPtr (PcdAmdDiscreteSsdtGuid), sizeof (EFI_GUID));
  CopyMem (&((**SgInfoDataHob).AmdPowerXpressSsdtGuid), PcdGetPtr (PcdAmdPowerXpressSsdtGuid), sizeof (EFI_GUID));
  CopyMem (&((**SgInfoDataHob).AmdUltPowerXpressSsdtGuid), PcdGetPtr (PcdAmdUltPowerXpressSsdtGuid), sizeof (EFI_GUID));
  CopyMem (&((**SgInfoDataHob).NvidiaDiscreteSsdtGuid), PcdGetPtr (PcdNvidiaDiscreteSsdtGuid), sizeof (EFI_GUID));
  CopyMem (&((**SgInfoDataHob).NvidiaOptimusSsdtGuid), PcdGetPtr (PcdNvidiaOptimusSsdtGuid), sizeof (EFI_GUID));
  CopyMem (&((**SgInfoDataHob).NvidiaUltOptimusSsdtGuid), PcdGetPtr (PcdNvidiaUltOptimusSsdtGuid), sizeof (EFI_GUID));


  //
  // System Configuration variables to set SG mode default value,
  // 0 = SgModeDisabled, 1 = SgModeMuxed, 2 = SgModeMuxless, 3 = SgModeDgpu.
  //
  VariableSize = sizeof (UINT8);
  Status = VariableServices->GetVariable (
                                VariableServices,
                                L"BootType",
                                &gSystemConfigurationGuid,
                                NULL,
                                &VariableSize,
                                &BootType
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  (**SgInfoDataHob).SgMode = SetSgModeValue (SystemConfiguration);
  (**SgInfoDataHob).BootType = BootType;
  (**SgInfoDataHob).PannelScaling = SystemConfiguration.PannelScaling;
  (**SgInfoDataHob).PowerXpressSetting = FullDgpuPowerOffDynamic;
  if (SystemConfiguration.PowerXpress != 0) {
    (**SgInfoDataHob).PowerXpressSetting = SystemConfiguration.PowerXpress;
  }
  (**SgInfoDataHob).IsUltBoard = FALSE;
  if (GetCpuFamily () == EnumCpuHswUlt) {
    (**SgInfoDataHob).IsUltBoard = TRUE;
    (**SgInfoDataHob).DgpuBridgeBus      = PcdGet8 (PcdSgPcieBridgeBus);
    (**SgInfoDataHob).DgpuBridgeDevice   = PcdGet8 (PcdSgPcieBridgeDevice);
    (**SgInfoDataHob).DgpuBridgeFunction = PcdGet8 (PcdSgPcieBridgeFunction);
  }

  return EFI_SUCCESS;
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

/**

  Power enable MXM (Mobile PCI Express Module) interface discrete GPU in this function,
  and reserved a hook point for OEM to customization.

  @param[in, out] SgInfoDataHob   A pointer of SG information data HOB.
  @param[in]      SgPpi           A pointer of PEI Switchable Graphics PPI Function
                                  for SG GPIO read/write and stall.

  @retval None.

**/
STATIC
VOID
MxmDgpuPowerEnableSequence (
  IN H2O_SWITCHABLE_GRAPHICS_PPI              SgPpi,
  IN OUT SG_INFORMATION_DATA_HOB              *SgInfoDataHob
  )
{
  BOOLEAN                                     DgpuMode;
  BOOLEAN                                     IsUlt;
  BOOLEAN                                     SlaveGpuExist;
  EFI_STATUS                                  Status;

  DgpuMode = Inactive;
  SlaveGpuExist = FALSE;
  IsUlt = SgInfoDataHob->IsUltBoard;

  //
  // OEM service hook for OEM customization,
  // base on project own discrete GPU power enable sequence to enable the GPU,
  // and OEM can base on SG mode variable to do different sequence,
  // or base on SKU ID to change SG mode or any SG information data HOB too.
  //
//[-start-130524-IB05160451-modify]//
  Status = OemSvcMxmDgpuPowerSequence ();
//[-end-130524-IB05160451-modify]//
  if (!EFI_ERROR (Status)) {
    //
    // If return EFI_SUCCESS, it means MXM GPU power sequence already done and success,
    // doesn't need do anythings else in this function, so just return.
    //
    return;
  }
  //
  // If return EFI_UNSUPPORTED, it means the OEM service doesn't do anything then returned go through here.
  // If return EFI_MEDIA_CHANGED, it means the OEM service change the SG information data HOB,
  // but still need MXM GPU power enable sequence in this function.
  //
  if (SgInfoDataHob->SgGpioSupport == SgGpioNotSupported) {
    return;
  }
  //
  // Detect slave MXM GPU status.
  //
  if (SgInfoDataHob->SlaveMxmGpuSupport) {
    if (SgInfoDataHob->SgSlaveDgpuPrsntExist) {
      if (SgPpi.GpioRead (SgInfoDataHob->SgSlaveDgpuPrsnt, IsUlt) == Active) {
        SlaveGpuExist = TRUE;
      }
    } else {
      SlaveGpuExist = TRUE;
    }
  }
  //
  // If this platform used GPIO to control MXM interface discrete GPU power,
  // through default sequence to power enable the GPU for CRB.
  //
  if (((SgInfoDataHob->SgDgpuPrsntExist) && (SgPpi.GpioRead (SgInfoDataHob->SgDgpuPrsnt, IsUlt) == Inactive)) ||
      (SgInfoDataHob->SgMode == SgModeDisabled)) {
    //
    // If can not detect GPU on MXM slot or SG mode disable (internal only SKU),
    // make sure the GPU power down, and select internal GPU to control output.
    //
    if (SgPpi.GpioRead (SgInfoDataHob->SgDgpuPwrEnable, IsUlt) == Active) {
      //
      // If GPIO default set discrete GPU power enable,
      // through the power down sequence to disable the discrete GPU.
      // Power down sequence: (1) Active DgpuHoldRst 
      //                      (2) Inactive DgpuPwrEnable
      //
      SgPpi.GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Active, IsUlt);
      SgPpi.GpioWrite (SgInfoDataHob->SgDgpuPwrEnable, Inactive, IsUlt);
    } else {
      SgPpi.GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Inactive, IsUlt);
    }
    if (SlaveGpuExist) {
      if (SgPpi.GpioRead (SgInfoDataHob->SgSlaveDgpuPwrEnable, IsUlt) == Active) {
        SgPpi.GpioWrite (SgInfoDataHob->SgSlaveDgpuHoldRst, Active, IsUlt);
        SgPpi.GpioWrite (SgInfoDataHob->SgSlaveDgpuPwrEnable, Inactive, IsUlt);
      } else {
        SgPpi.GpioWrite (SgInfoDataHob->SgSlaveDgpuHoldRst, Inactive, IsUlt);
      }
    }
    SgInfoDataHob->SgMode = SgModeDisabled;
  } else {
    //
    // Power enable the discrete GPU when dual VGA mode (SG) or discrete GPU only mode.
    // Power enable sequence: (1) Active DgpuHoldRst => Delay 100ms
    //                        (2) Active DgpuPwrEnable => Delay 300ms
    //                        (3) Inactive DgpuHoldRst => Delay 100ms
    // The delay time after GPIO active or inactive that can tune by project for optimization.
    //
    if ((SgPpi.GpioRead (SgInfoDataHob->SgDgpuPwrEnable, IsUlt) == Inactive) ||
        (SlaveGpuExist && (SgPpi.GpioRead (SgInfoDataHob->SgDgpuPwrEnable, IsUlt) == Inactive))) {
      SgPpi.GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Active, IsUlt);
      if (SlaveGpuExist) {
        SgPpi.GpioWrite (SgInfoDataHob->SgSlaveDgpuHoldRst, Active, IsUlt);
      }
      SgPpi.Stall (SgInfoDataHob->ActiveDgpuHoldRstDelay * 1000);
      SgPpi.GpioWrite (SgInfoDataHob->SgDgpuPwrEnable, Active, IsUlt);
      if (SlaveGpuExist) {
        SgPpi.GpioWrite (SgInfoDataHob->SgSlaveDgpuPwrEnable, Active, IsUlt);
      }
      SgPpi.Stall (SgInfoDataHob->ActiveDgpuPwrEnableDelay * 1000);
      SgPpi.GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Inactive, IsUlt);
      if (SlaveGpuExist) {
        SgPpi.GpioWrite (SgInfoDataHob->SgSlaveDgpuHoldRst, Inactive, IsUlt);
      }
      SgPpi.Stall (SgInfoDataHob->InactiveDgpuHoldRstDelay * 1000);
    } else {
      SgPpi.GpioWrite (SgInfoDataHob->SgDgpuHoldRst, Inactive, IsUlt);
      if (SlaveGpuExist) {
        SgPpi.GpioWrite (SgInfoDataHob->SgSlaveDgpuHoldRst, Inactive, IsUlt);
      }
    }
    //
    // PEG bridge device (0/1/0 or 0/1/1 or 0/1/2) offset 0xBA (SLOTSTS - Slot Status)
    // Bit 6 (Presence Detect State (PDS)) 0: Slot Empty, 1: Card present in slot.
    // After MXM GPU power enable sequence, this bit should be set to high.
    //
  }
  //
  // Select the discrete GPU to control output when discrete GPU only platform,
  // or select the internal GPU to control output when internal GPU only or dual VGA platform.
  //
  if (SgInfoDataHob->SgMode == SgModeDgpu) {
    DgpuMode = Active;
  }
  if (SgInfoDataHob->SgDgpuEdidSelExist) {
    SgPpi.GpioWrite (SgInfoDataHob->SgDgpuEdidSel, DgpuMode, IsUlt);
  }
  if (SgInfoDataHob->SgDgpuSelExist) {
    SgPpi.GpioWrite (SgInfoDataHob->SgDgpuSel, DgpuMode, IsUlt);
  }
  if (SgInfoDataHob->SgDgpuPwmSelExist) {
    SgPpi.GpioWrite (SgInfoDataHob->SgDgpuPwmSel, DgpuMode, IsUlt);
  }
}

/**

  SG own related GPIO pin read function.

  @param[in] Value   Bit 0 to 6 decide which pin will be read,
                     bit 7 decide this pin high active or low active.
  @param[in] IsUlt   FALSE: Code will base on Two Chip GPIO design to read/write.
                     TRUE:  Code will base on ULT GPIO design to read/write.

  @retval TRUE    This GPIO pin is active this moment.
  @retval FALSE   This GPIO pin is inactive this moment.

**/
BOOLEAN
EFIAPI
GpioRead (
  IN UINT8                                    Value,
  IN BOOLEAN                                  IsUlt
  )
{
  BOOLEAN                                     Active;
  UINT16                                      BitOffset;
  UINT16                                      GpioAddress;
  UINT32                                      Data;

  GpioAddress = PcdGet16 (PcdPchGpioBaseAddress);

  Active = (BOOLEAN) (Value >> 7);
  Value &= 0x7F;

  if (IsUlt) {
    Data = IoRead32 (GpioAddress + R_PCH_GP_N_CONFIG0 + Value * 8 + 0x0);
    if ((Data >> 2) & 1) {
      Data = (Data >> 30) & 1; // GPI_LVL
    } else {
      Data = (Data >> 31) & 1; // GPO_LVL
    }
  } else {
    if (Value < 0x20) {
      GpioAddress += R_PCH_GPIO_LVL;
      BitOffset = Value;
    } else if (Value < 0x40) {
      GpioAddress += R_PCH_GPIO_LVL2;
      BitOffset = Value - 0x20;
    } else {
      GpioAddress += R_PCH_GPIO_LVL3;
      BitOffset = Value - 0x40;
    }
    Data = IoRead32 (GpioAddress);
    Data >>= BitOffset;
  }

  if (Active == 0) {
     Data = ~Data;
  }

  return (BOOLEAN)(Data & 0x1);
}

/**

  SG own related GPIO pin write function.

  @param[in] Value   Bit 0 to 6 decide which pin will be write,
                     bit 7 decide this pin high active or low active.
  @param[in] Level   TRUE  Base on GPIO value information to let this GPIO pin "active".
                     FALSE Base on GPIO value information to let this GPIO pin "inactive".
  @param[in] IsUlt   FALSE: Code will base on Two Chip GPIO design to read/write.
                     TRUE:  Code will base on ULT GPIO design to read/write.

  @retval None.

**/
VOID
EFIAPI
GpioWrite (
  IN UINT8                                    Value,
  IN BOOLEAN                                  Level,
  IN BOOLEAN                                  IsUlt
  )
{
  BOOLEAN                                     Active;
  UINT16                                      BitOffset;
  UINT16                                      GpioAddress;
  UINT32                                      Data;

  Active = (BOOLEAN) (Value >> 7);
  Value &= 0x7F;

  if (Active == 0) {
     Level = (~Level) & 0x1;
  }

  GpioAddress = PcdGet16 (PcdPchGpioBaseAddress);

  if (IsUlt) {
    Data = IoRead32 (GpioAddress + R_PCH_GP_N_CONFIG0 + Value * 8 + 0x0);
    Data &= ~(0x1 << 31);
    Data |= (Level << 31);
    IoWrite32 (GpioAddress + R_PCH_GP_N_CONFIG0 + Value * 8 + 0x0, Data);
  } else {
    if (Value < 0x20) {
      GpioAddress += R_PCH_GPIO_LVL;
      BitOffset = Value;
    } else if (Value < 0x40) {
      GpioAddress += R_PCH_GPIO_LVL2;
      BitOffset = Value - 0x20;
    } else {
      GpioAddress += R_PCH_GPIO_LVL3;
      BitOffset = Value - 0x40;
    }
    Data = IoRead32 (GpioAddress);
    Data &= ~(0x1 << BitOffset);
    Data |= (Level << BitOffset);
    IoWrite32 (GpioAddress, Data);
  }

  return;
}

/**

  Waits for at least the given number of microseconds.

  @param[in]  Microseconds   - Desired length of time to wait
  
  @retval   EFI_SUCCESS    - If the desired amount of time passed.
  @retval   !EFI_SUCCESS   - If error occurs while locating CpuIoPpi.

**/
EFI_STATUS
EFIAPI
Stall (
  IN UINTN                                    Microseconds
  )
{
  UINT32                                      CurrentTick;
  UINT32                                      OriginalTick;
  UINT32                                      RemainingTick;
  UINTN                                       Counts;
  UINTN                                       Ticks;

  if (Microseconds == 0) {
    return EFI_SUCCESS;
  }

  OriginalTick  = IoRead32 (ACPI_TIMER_ADDR) & 0x00FFFFFF;

  CurrentTick   = OriginalTick;

  //
  // The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;
  //
  // The loops needed by timer overflow
  //
  Counts = Ticks / ACPI_TIMER_MAX_VALUE;
  //
  // remaining clocks within one loop
  //
  RemainingTick = Ticks % ACPI_TIMER_MAX_VALUE;
  //
  // not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  // one I/O operation, and maybe generate SMI
  //
  while (Counts != 0) {
    CurrentTick = IoRead32 (ACPI_TIMER_ADDR) & 0x00FFFFFF;

    if (CurrentTick <= OriginalTick) {
      Counts--;
    }

    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick)) {
    OriginalTick  = CurrentTick;
    CurrentTick   = (IoRead32 (ACPI_TIMER_ADDR) & 0x00FFFFFF);
  }

  return EFI_SUCCESS;
}
