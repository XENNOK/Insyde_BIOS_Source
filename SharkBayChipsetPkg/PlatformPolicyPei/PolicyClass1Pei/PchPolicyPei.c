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

#include <PchPolicyPei.h>
//[-start-130812-IB06720232-add]//
#include <Library/BaseOemSvcChipsetLib.h>
//[-end-130812-IB06720232-add]//

EFI_STATUS
PchPolicyPeiEntry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                    Status;
  EFI_PEI_PPI_DESCRIPTOR        *PchPlatformPolicyPpiDesc;
  PCH_PLATFORM_POLICY_PPI       *PchPlatformPolicyPpi;
  PCH_GBE_CONFIG                *GbeConfig;
  PCH_THERMAL_MANAGEMENT        *ThermalMgmt;
  PCH_MEMORY_THROTTLING         *MemoryThrottling;
  PCH_HPET_CONFIG               *HpetConfig;
  PCH_SATA_CONTROL              *SataConfig;
  PCH_SATA_TRACE_CONFIG         *SataTraceConfig;
  PCH_PCIE_CONFIG               *PcieConfig;
  PCH_IOAPIC_CONFIG             *IoApicConfig;
  PCH_PLATFORM_DATA             *PlatformData;
#ifdef USB_PRECONDITION_ENABLE_FLAG
  EFI_BOOT_MODE                 BootMode;
#endif  // USB_PRECONDITION_ENABLE_FLAG  
//[-start-120807-IB04770241-add]//
  PCH_USB_CONFIG                *UsbConfig;
//[-end-120807-IB04770241-add]//
  CHIPSET_CONFIGURATION          SystemConfiguration;
//[-start-130710-IB05160465-add]//
  VOID                           *SetupVariable;
//[-end-130710-IB05160465-add]//
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *Variable;
  UINTN                         VariableSize;
  EFI_GUID                      SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
//[-start-130809-IB06720232-add]//
  EFI_STATUS                    SetDswModeStatus;
//[-end-130809-IB06720232-add]//
  UINT8                         PortIndex;

  PchPlatformPolicyPpiDesc = NULL;
  PchPlatformPolicyPpi     = NULL;
  GbeConfig                = NULL;
  ThermalMgmt              = NULL;
  MemoryThrottling         = NULL;
  HpetConfig               = NULL;
  SataConfig               = NULL;
  SataTraceConfig          = NULL;
  PcieConfig               = NULL;
  IoApicConfig             = NULL;
  PlatformData             = NULL;
  Variable                 = NULL;
  VariableSize             = 0;
//[-start-130809-IB06720232-add]//
  SetDswModeStatus         = EFI_UNSUPPORTED;
//[-end-130809-IB06720232-add]//

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
  
  SystemConfiguration.HPETSupport           = 1;

  //
  // Allocate memory for the PCH Policy PPI and Descriptor
  //
  PchPlatformPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *) AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (PchPlatformPolicyPpiDesc != NULL);
  if (PchPlatformPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PchPlatformPolicyPpi = (PCH_PLATFORM_POLICY_PPI *) AllocateZeroPool (sizeof (PCH_PLATFORM_POLICY_PPI));
  ASSERT (PchPlatformPolicyPpi != NULL);
  if (PchPlatformPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  GbeConfig = (PCH_GBE_CONFIG *) AllocateZeroPool (sizeof (PCH_GBE_CONFIG));
  ASSERT (GbeConfig != NULL);
  if (GbeConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ThermalMgmt = (PCH_THERMAL_MANAGEMENT *) AllocateZeroPool (sizeof (PCH_THERMAL_MANAGEMENT));
  ASSERT (ThermalMgmt != NULL);
  if (ThermalMgmt == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  
  MemoryThrottling = (PCH_MEMORY_THROTTLING *) AllocateZeroPool (sizeof (PCH_MEMORY_THROTTLING));
  ASSERT (MemoryThrottling != NULL);
  if (MemoryThrottling == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HpetConfig = (PCH_HPET_CONFIG *) AllocateZeroPool (sizeof (PCH_HPET_CONFIG));
  ASSERT (HpetConfig != NULL);
  if (HpetConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SataConfig = (PCH_SATA_CONTROL *) AllocateZeroPool (sizeof (PCH_SATA_CONTROL));
  ASSERT (SataConfig != NULL);
  if (SataConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SataTraceConfig = (PCH_SATA_TRACE_CONFIG *) AllocateZeroPool (sizeof (PCH_SATA_TRACE_CONFIG));
  ASSERT (SataTraceConfig != NULL);
  if (SataTraceConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PcieConfig = (PCH_PCIE_CONFIG *) AllocateZeroPool (sizeof (PCH_PCIE_CONFIG));
  ASSERT (PcieConfig != NULL);
  if (PcieConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  IoApicConfig = (PCH_IOAPIC_CONFIG *) AllocateZeroPool (sizeof (PCH_IOAPIC_CONFIG));
  ASSERT (IoApicConfig != NULL);
  if (IoApicConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  PlatformData = (PCH_PLATFORM_DATA *) AllocateZeroPool (sizeof (PCH_PLATFORM_DATA));
  ASSERT (PlatformData != NULL);
  if (PlatformData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-start-120807-IB04770241-add]//
  UsbConfig = (PCH_USB_CONFIG *) AllocateZeroPool (sizeof (PCH_USB_CONFIG));
  ASSERT (UsbConfig != NULL);
  if (UsbConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-120807-IB04770241-add]//

//[-start-120807-IB04770241-modify]//
  PchPlatformPolicyPpi->Revision                                = PCH_PLATFORM_POLICY_PPI_REVISION_4;
//[-end-120807-IB04770241-modify]//
  PchPlatformPolicyPpi->BusNumber                               = 0;
  PchPlatformPolicyPpi->Rcba                                    = PcdGet32 (PcdRcbaBaseAddress);
//[-start-121109-IB05280008-modify]//
//[-start-121008-IB11410008-modify]//
  PchPlatformPolicyPpi->PmBase                                  = PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress);
//[-end-121008-IB11410008-modify]//
//[-end-121109-IB05280008-modify]//
  PchPlatformPolicyPpi->GpioBase                                = PcdGet16 (PcdPchGpioBaseAddress);

  PchPlatformPolicyPpi->GbeConfig                               = GbeConfig;
//[-start-120320-IB06460377-modify]//
  GbeConfig->EnableGbe                                          = SystemConfiguration.Lan;
//[-end-120320-IB06460377-modify]//
//  GbeConfig->GbeMemBaseAddr                                     = PCH_LAN_MBARB_BASE_ADDRESS;
  
  PchPlatformPolicyPpi->ThermalMgmt                             = ThermalMgmt;
//  ThermalMgmt->ThermalBaseB                                     = PCH_THERMAL_TBARB_BASE_ADDRESS;
  ThermalMgmt->MemoryThrottling                                 = MemoryThrottling;
  MemoryThrottling->Enable                                      = PcdGet8 (PcdPchMemoryThrottlingEnable);
  MemoryThrottling->TsGpioPinSetting[TsGpioC].PmsyncEnable      = PcdGet8 (PcdPchMemoryThrottlingTsGpioPinSettingTsGpioCPmsyncEnable);
  MemoryThrottling->TsGpioPinSetting[TsGpioD].PmsyncEnable      = PcdGet8 (PcdPchMemoryThrottlingTsGpioPinSettingTsGpioDPmsyncEnable);
  MemoryThrottling->TsGpioPinSetting[TsGpioC].C0TransmitEnable  = PcdGet8 (PcdPchMemoryThrottlingTsGpioPinSettingTsGpioCC0TransmitEnable);
  MemoryThrottling->TsGpioPinSetting[TsGpioD].C0TransmitEnable  = PcdGet8 (PcdPchMemoryThrottlingTsGpioPinSettingTsGpioDC0TransmitEnable);
  MemoryThrottling->TsGpioPinSetting[TsGpioC].PinSelection      = PcdGet8 (PcdPchMemoryThrottlingTsGpioPinSettingTsGpioCPinSelection);
  MemoryThrottling->TsGpioPinSetting[TsGpioD].PinSelection      = PcdGet8 (PcdPchMemoryThrottlingTsGpioPinSettingTsGpioDPinSelection);

  PchPlatformPolicyPpi->HpetConfig                              = HpetConfig;
  HpetConfig->Enable                                            = SystemConfiguration.HPETSupport;
  HpetConfig->Base                                              = PcdGet32 (PcdHpetBaseAddress);            //HpetConfig->Base;

  PchPlatformPolicyPpi->Port80Route                             = SystemConfiguration.Port80Route;
  
  PchPlatformPolicyPpi->SataConfig                              = SataConfig;
  SataConfig->SataMode                                          = SystemConfiguration.SataCnfigure;
  SataConfig->SataTraceConfig                                   = SataTraceConfig;
  SataTraceConfig->TestMode                                     = PcdGet8 (PcdPchSataTraceConfigTestMode);
  SataTraceConfig->RsvdBits                                     = 0; 

  for( PortIndex = 0; PortIndex < 6; PortIndex++ ) {
    SataTraceConfig->PortRxEq[PortIndex].GenSpeed[0].Enable = PcdGet8 (PcdPchSataPortRxEqGenSpeedEnable);
    SataTraceConfig->PortRxEq[PortIndex].GenSpeed[1].Enable = PcdGet8 (PcdPchSataPortRxEqGenSpeedEnable);
    SataTraceConfig->PortRxEq[PortIndex].GenSpeed[2].Enable = PcdGet8 (PcdPchSataPortRxEqGenSpeedEnable);
    SataTraceConfig->PortRxEq[PortIndex].GenSpeed[0].RxEq = 0x0;
    SataTraceConfig->PortRxEq[PortIndex].GenSpeed[1].RxEq = 0x0;
    SataTraceConfig->PortRxEq[PortIndex].GenSpeed[2].RxEq = 0x0;
  }


  PchPlatformPolicyPpi->PcieConfig                              = PcieConfig;
  PcieConfig->PcieSpeed[0]                                      = SystemConfiguration.PcieSpeed0;
  PcieConfig->PcieSpeed[1]                                      = SystemConfiguration.PcieSpeed1;
  PcieConfig->PcieSpeed[2]                                      = SystemConfiguration.PcieSpeed2;
  PcieConfig->PcieSpeed[3]                                      = SystemConfiguration.PcieSpeed3;
  PcieConfig->PcieSpeed[4]                                      = SystemConfiguration.PcieSpeed4;
  PcieConfig->PcieSpeed[5]                                      = SystemConfiguration.PcieSpeed5;
  PcieConfig->PcieSpeed[6]                                      = SystemConfiguration.PcieSpeed6;
  PcieConfig->PcieSpeed[7]                                      = SystemConfiguration.PcieSpeed7;
  
  PchPlatformPolicyPpi->IoApicConfig                            = IoApicConfig;
  IoApicConfig->IoApicId                                        = PcdGet8 (PcdPchIoApicConfigIoApicId);                  //IoApicConfig->IoApicId;
  IoApicConfig->ApicRangeSelect                                 = PcdGet8 (PcdPchIoApicConfigApicRangeSelect);            //IoApicConfig->ApicRangeSelect;  
//[-start-120628-IB06460410-add]//
  IoApicConfig->IoApicEntry24_39                                = PcdGet8 (PcdPchIoApicConfigIoApicEntry24_39);
//[-end-120628-IB06460410-add]//
  PchPlatformPolicyPpi->PlatformData                            = PlatformData;
//[-start-120807-IB04770241-add]//
  PchPlatformPolicyPpi->UsbConfig                               = UsbConfig;

  ///
  /// UsbConfig should be initialized based on platform configuration if UsbPrecondition feature is
  /// enabled. Otherwise, the remaining data of UsbConfig can stay in zero.
  ///
  UsbConfig->UsbPrecondition                                    = PcdGetBool (PcdPchUsbConfigUsbPrecondition);
//[-end-120807-IB04770241-add]//

  PlatformData->EcPresent                                       = PcdGet8 (PcdPchPchPlatformDataEcPresent);
//[-start-120827-IB05300323-modify]//
  if (FeaturePcdGet (PcdPfatSupport)) {
//[-start-130305-IB05160418-modify]//
    PlatformData->SmmBwp                                        = 0; // = SystemConfiguration.Pfat;
//[-end-130305-IB05160418-modify]//
  } else {
    PlatformData->SmmBwp                                          = PcdGet8 (PcdPchPchPlatformDataSmmBwp);
  }
//[-end-120827-IB05300323-modify]//
  ///
  /// Temporary Memory Base Address for PCI devices to be used to initialize MMIO registers.
  /// Minimum size is 64KB bytes.
  ///
  PlatformData->TempMemBaseAddr                                 = PCH_TEMP_MEM_BASE_ADDRESS;
//[-start-120807-IB04770241-remove]//
// //[-start-120628-IB06460410-add]//
// #ifdef PCH_A0PWRON_SAFEMODE
//   //
//   // PwrOnSafeModeBits - bit definition:
//   //
//   // Enable safe modes by setting all bits
//   // LPT-LP definition:
//   //  BIT0  = Misc Safe Mode
//   //  BIT1  = Functional PM Safe Mode
//   //  BIT2  = Low Power PM Disable
//   //  BIT3  = CPPM Disable
//   //  BIT4  = Clock Gate Disable
//   //  BIT5  = All ASPM Disable
//   //  BIT6  = Lock register bypass
//   //  BIT7  = Thermal Disable
//   //  BIT8  = Gbe Disable
//   //  BIT9  = Chap Disable
//   //
// //[-start-120705-IB05330352-modify]//
//   PchPlatformPolicyPpi->PwrOnSafeModeBits                        = 0x12;  
// //[-end-120705-IB05330352-modify]//
// #endif
// //[-end-120628-IB06460410-add]//
//[-end-120807-IB04770241-remove]//
  
//[-start-130809-IB06720232-modify]//
  OemSvcEcSetDswMode (&SetDswModeStatus, SystemConfiguration.DeepSlpSx);
//[-end-130809-IB06720232-modify]//
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Set DSW Mode: %x to EC: %r.\n",SystemConfiguration.DeepSlpSx, SetDswModeStatus));

  //
  // Update the PCH Policy Ppi Descriptor
  //
  PchPlatformPolicyPpiDesc->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PchPlatformPolicyPpiDesc->Guid  = &gPchPlatformPolicyPpiGuid;
  PchPlatformPolicyPpiDesc->Ppi   = PchPlatformPolicyPpi;

//[-start-121224-IB10870061-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdatePchPlatformPolicy (PchPlatformPolicyPpi, PchPlatformPolicyPpiDesc);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcUpdatePchPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Installing PCH Policy (PEI).\n"));

#ifdef USB_PRECONDITION_ENABLE_FLAG
  ///
  /// Update Precondition option for S4 resume. 
  /// Skip Precondition for S4 resume in case this boot may not connect BIOS USB driver.
  /// If BIOS USB driver will be connected always for S4, then disable below update.
  /// To keep consistency during boot, must enabled or disabled below function in both PEI and DXE
  /// PlatformPolicyInit driver.
  ///
  if (UsbConfig->UsbPrecondition == TRUE) {
    (*PeiServices)->GetBootMode (PeiServices, &BootMode);
    if (BootMode == BOOT_ON_S4_RESUME) {
      UsbConfig->UsbPrecondition = FALSE;
      DEBUG ((EFI_D_INFO, "BootMode is BOOT_ON_S4_RESUME, disable Precondition\n"));
    }
  }
#endif  // USB_PRECONDITION_ENABLE_FLAG

    //
    // Install PCH Platform Policy PPI
    //
    Status = PeiServicesInstallPpi (PchPlatformPolicyPpiDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121224-IB10870061-add]//

  //
  // Dump policy
  //
  DumpPCHPolicyPEI (PeiServices, PchPlatformPolicyPpi);

  return Status;
}

VOID
DumpPCHPolicyPEI (
  IN      CONST EFI_PEI_SERVICES         **PeiServices,
  IN      PCH_PLATFORM_POLICY_PPI        *PchPlatformPolicyPpi
  )
{
  UINTN        Count;

  Count = 0;

  DEBUG ((EFI_D_INFO, "PchPlatformPolicyPpi ( Address : 0x%x )\n", PchPlatformPolicyPpi));
  DEBUG ((EFI_D_INFO, " |\n"));
  DEBUG ((EFI_D_INFO, " |-Revision                       : %x\n", PchPlatformPolicyPpi->Revision));
  DEBUG ((EFI_D_INFO, " |-BusNumber                      : %x\n", PchPlatformPolicyPpi->BusNumber));
  DEBUG ((EFI_D_INFO, " |-Rcba                           : %x\n", PchPlatformPolicyPpi->Rcba));
  DEBUG ((EFI_D_INFO, " |-PmBase                         : %x\n", PchPlatformPolicyPpi->PmBase));
  DEBUG ((EFI_D_INFO, " |-GpioBase                       : %x\n", PchPlatformPolicyPpi->GpioBase));
  DEBUG ((EFI_D_INFO, " |-GbeConfig ( Address : 0x%x )\n", PchPlatformPolicyPpi->GbeConfig));  
  DEBUG ((EFI_D_INFO, " | |-EnableGbe                    : %x\n", PchPlatformPolicyPpi->GbeConfig->EnableGbe)); 
//  DEBUG ( ( EFI_D_INFO, " | |-GbeMemBaseAddr               : %x\n", PchPlatformPolicyPpi->GbeConfig->GbeMemBaseAddr));  
  DEBUG ((EFI_D_INFO, " |-ThermalMgmt                    :\n", PchPlatformPolicyPpi->ThermalMgmt));  
//  DEBUG ( ( EFI_D_INFO, " | |-ThermalBaseB                 : %x\n", PchPlatformPolicyPpi->ThermalMgmt->ThermalBaseB));  
  DEBUG ((EFI_D_INFO, " | |-MemoryThrottling ( Address : 0x%x )\n",PchPlatformPolicyPpi->ThermalMgmt->MemoryThrottling));
  DEBUG ((EFI_D_INFO, " | | |-Enable                     : %x\n", PchPlatformPolicyPpi->ThermalMgmt->MemoryThrottling->Enable));   
  DEBUG ((EFI_D_INFO, " | | |-Rsvdbits                   : %x\n", PchPlatformPolicyPpi->ThermalMgmt->MemoryThrottling->Rsvdbits));  
  for (Count = 0; Count < MaxTsGpioPin; Count++) {  
    DEBUG ((EFI_D_INFO, " | | |-[%2x].PmsyncEnable          : %x\n", Count, PchPlatformPolicyPpi->ThermalMgmt->MemoryThrottling->TsGpioPinSetting[Count].PmsyncEnable));    
    DEBUG ((EFI_D_INFO, " | | |-[%2x].C0TransmitEnable      : %x\n", Count, PchPlatformPolicyPpi->ThermalMgmt->MemoryThrottling->TsGpioPinSetting[Count].C0TransmitEnable));    
    DEBUG ((EFI_D_INFO, " | | |-[%2x].PinSelection          : %x\n", Count, PchPlatformPolicyPpi->ThermalMgmt->MemoryThrottling->TsGpioPinSetting[Count].PinSelection));  
    DEBUG ((EFI_D_INFO, " | | |-[%2x].Rsvdbits              : %x\n", Count, PchPlatformPolicyPpi->ThermalMgmt->MemoryThrottling->TsGpioPinSetting[Count].Rsvdbits));     
  }
  DEBUG ((EFI_D_INFO, " |-HpetConfig ( Address : 0x%x )\n", PchPlatformPolicyPpi->HpetConfig));
  DEBUG ((EFI_D_INFO, " | |-Enable                       : %x\n", PchPlatformPolicyPpi->HpetConfig->Enable));  
  DEBUG ((EFI_D_INFO, " | |-Base                         : %x\n", PchPlatformPolicyPpi->HpetConfig->Base));    
  DEBUG ((EFI_D_INFO, " |-Port80Route                    : %x\n", PchPlatformPolicyPpi->Port80Route));  
  DEBUG ((EFI_D_INFO, " |-SataConfig                     :\n"));
  DEBUG ((EFI_D_INFO, " | |-SataMode                     : %x\n", PchPlatformPolicyPpi->SataConfig->SataMode));
  DEBUG ((EFI_D_INFO, " | |-SataTraceConfig ( Address : 0x%x ) \n", PchPlatformPolicyPpi->SataConfig->SataTraceConfig));
  DEBUG ((EFI_D_INFO, " | | |-PortLength[0]              : %x\n", PchPlatformPolicyPpi->SataConfig->SataTraceConfig->PortLength[0]));  
  DEBUG ((EFI_D_INFO, " | | |-PortLength[1]              : %x\n", PchPlatformPolicyPpi->SataConfig->SataTraceConfig->PortLength[1]));
  DEBUG ((EFI_D_INFO, " | | |-TestMode                   : %x\n", PchPlatformPolicyPpi->SataConfig->SataTraceConfig->TestMode));
  DEBUG ((EFI_D_INFO, " | | |-RsvdBits                   : %x\n", PchPlatformPolicyPpi->SataConfig->SataTraceConfig->RsvdBits));
  DEBUG ((EFI_D_INFO, " | | |-PortTopology[0]            : %x\n", PchPlatformPolicyPpi->SataConfig->SataTraceConfig->PortTopology[0]));  
  DEBUG ((EFI_D_INFO, " | | |-PortTopology[1]            : %x\n", PchPlatformPolicyPpi->SataConfig->SataTraceConfig->PortTopology[1]));  
  DEBUG ((EFI_D_INFO, " |-PcieConfig ( Address : 0x%x )\n", PchPlatformPolicyPpi->PcieConfig));
//[-start-120628-IB06460410-modify]//
  for (Count = 0; Count < LPTH_PCIE_MAX_ROOT_PORTS; Count++) {
//[-end-120628-IB06460410-modify]//
    DEBUG ((EFI_D_INFO, " | |-PcieSpeed[%2x]                : %x\n", Count, PchPlatformPolicyPpi->PcieConfig->PcieSpeed[Count]));    
  }
  DEBUG ((EFI_D_INFO, " |-IoApicConfig                   : %x\n", PchPlatformPolicyPpi->IoApicConfig));
  DEBUG ((EFI_D_INFO, " | |-IoApicId                     : %x\n", PchPlatformPolicyPpi->IoApicConfig->IoApicId));
  DEBUG ((EFI_D_INFO, " | |-ApicRangeSelect              : %x\n", PchPlatformPolicyPpi->IoApicConfig->ApicRangeSelect));    
//[-start-120628-IB06460410-add]//
  DEBUG ((EFI_D_INFO, " | |-IoApicEntry24_39              : %x\n", PchPlatformPolicyPpi->IoApicConfig->IoApicEntry24_39));    
//[-end-120628-IB06460410-add]//
  DEBUG ((EFI_D_INFO, " |-PlatformData                   : %x\n", PchPlatformPolicyPpi->PlatformData));
  DEBUG ((EFI_D_INFO, " | |-EcPresent                    : %x\n", PchPlatformPolicyPpi->PlatformData->EcPresent));
  DEBUG ((EFI_D_INFO, " | |-SmmBwp                       : %x\n", PchPlatformPolicyPpi->PlatformData->SmmBwp));
  DEBUG ((EFI_D_INFO, " | |-Rsvdbits                     : %x\n", PchPlatformPolicyPpi->PlatformData->Rsvdbits));
  DEBUG ((EFI_D_INFO, " | |-TempMemBaseAddr              : %x\n", PchPlatformPolicyPpi->PlatformData->TempMemBaseAddr));  
//[-start-120807-IB04770241-remove]//
// //[-start-120628-IB06460410-add]//
// #ifdef PCH_A0PWRON_SAFEMODE
//   DEBUG ( ( EFI_D_INFO, " |-PwrOnSafeModeBits                       : %x\n", PchPlatformPolicyPpi->PwrOnSafeModeBits));
// #endif // PCH_A0PWRON_SAFEMODE
// //[-end-120628-IB06460410-add]//
//[-end-120807-IB04770241-remove]//
  DEBUG ((EFI_D_INFO, " |\n"));

  return;
}
