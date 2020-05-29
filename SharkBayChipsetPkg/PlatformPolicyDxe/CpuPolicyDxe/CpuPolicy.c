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

#include <CpuPolicy.h>
//[-start-120628-IB04770237-add]//
#include <Library/CpuPlatformLib.h>
//[-end-120628-IB04770237-add]//

EFI_EXP_BASE10_DATA               mCoreFrequencyList[] = {
  {
    0,
    0
  },  /// 0 Means "Auto", also, the first is the default.
  {
    -1,
    0
  }   /// End marker
};

EFI_EXP_BASE10_DATA               mFsbFrequencyList[] = {
  {
    0,
    0
  },  /// 0 Means "Auto", also, the first is the default.
  {
    -1,
    0
  }   /// End marker
};

/**
 Platform function to get MAX CPU count

 @param[in]         This                platform policy protocol
 @param[out]        MaxThreadsPerCore   variable that will store MaxThreadsPerCore
 @param[out]        MaxCoresPerDie      variable that will store MaxCoresPerDie
 @param[out]        MaxDiesPerPackage   variable that will store MaxDiesPerPackage
 @param[out]        MaxPackages         variable that will store MaxPackages

 @retval            EFI_SUCCESS         Always return success
*/
EFI_STATUS
EFIAPI
PlatformCpuGetMaxCount (
  IN  DXE_CPU_PLATFORM_POLICY_PROTOCOL       *This,
  OUT UINT32                                 *MaxThreadsPerCore,
  OUT UINT32                                 *MaxCoresPerDie,
  OUT UINT32                                 *MaxDiesPerPackage,
  OUT UINT32                                 *MaxPackages
  )
{
  EFI_CPUID_REGISTER        CpuidRegister;

  CpuidRegister.RegEax = 0;
  CpuidRegister.RegEbx = 0;
  CpuidRegister.RegEcx = 0;
  CpuidRegister.RegEdx = 0;

  AsmCpuidEx (CPUID_PROCESSOR_TOPOLOGY, 0, &CpuidRegister.RegEax, &CpuidRegister.RegEbx, &CpuidRegister.RegEcx, &CpuidRegister.RegEdx);
  *MaxThreadsPerCore = CpuidRegister.RegEbx & 0xFFFF;

  AsmCpuidEx (CPUID_PROCESSOR_TOPOLOGY, 1, &CpuidRegister.RegEax, &CpuidRegister.RegEbx, &CpuidRegister.RegEcx, &CpuidRegister.RegEdx);
  *MaxCoresPerDie = (CpuidRegister.RegEbx & 0xFFFF) / *MaxThreadsPerCore;

  *MaxDiesPerPackage = 1;
  *MaxPackages = 1;

  return EFI_SUCCESS;
}

/**
  Get CPU information

  @param[in] This                - platform policy protocol
  @param[in] Location            - structure that describe CPU location information
  @param[in,out] PlatformCpuInfo     - structure that will be updated for platform CPU information

  @retval EFI_INVALID_PARAMETER  - PlatformCpuInfo is NULL
  @retval EFI_SUCCESS            - platform CPU info structure has been updated
**/
EFI_STATUS
EFIAPI
PlatformCpuGetCpuInfo (
  IN DXE_CPU_PLATFORM_POLICY_PROTOCOL *This,
  IN CPU_PHYSICAL_LOCATION            *Location,
  IN OUT PLATFORM_CPU_INFORMATION     *PlatformCpuInfo
  )
{
  EFI_STATUS                     Status;
  EFI_HII_STRING_PROTOCOL        *HiiString;
  CHAR8                          Language[] = "en-US";
  STRING_REF                     SocketNameToken;
  STRING_REF                     FillByOemToken;
//[-start-121022-IB03780465-add]//
  UINT64                         MsrValue;
//[-end-121022-IB03780465-add]//
  UINT8      CpuSku;
  
  SocketNameToken = 0;
  FillByOemToken  = 0;

  ///
  /// For Processor SocketName definition.
  ///
  if (PlatformCpuInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (
             &gEfiHiiStringProtocolGuid,
             NULL,
             (VOID **)&HiiString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = HiiString->NewString (
             HiiString,
             PlatformCpuInfo->StringHandle,
             &SocketNameToken,
             Language,
             NULL,
             L"U3E1",
             NULL
             );
  ASSERT_EFI_ERROR (Status);

  Status = HiiString->NewString (
             HiiString,
             PlatformCpuInfo->StringHandle,
             &FillByOemToken,
             Language,
             NULL,
             L"To Be Filled By O.E.M.",
             NULL
             );
  ASSERT_EFI_ERROR (Status);

  PlatformCpuInfo->ApicID                    = Location->Thread;
  PlatformCpuInfo->ReferenceString           = 0;
  CpuSku = GetCpuSku ();
  switch (CpuSku) {
    case EnumCpuTrad:
      PlatformCpuInfo->SocketType = 0x2d; // @todo EfiProcessorSocketLGA1150, pending updated SMBIOS spec release
      break;

    case EnumCpuUlt:
      PlatformCpuInfo->SocketType = 0x2e; // @todo EfiProcessorSocketBGA1168, pending updated SMBIOS spec release
      break;

    default:
      PlatformCpuInfo->SocketType = EfiProcessorSocketOther;
      break;
  }
  PlatformCpuInfo->SocketName                = SocketNameToken;

//[-start-121022-IB03780465-modify]//
  MsrValue = AsmReadMsr64 (MSR_PLATFORM_INFO);
  PlatformCpuInfo->MaxCoreFrequency.Value    = (100 * (((UINT32) MsrValue >> N_PLATFORM_INFO_MAX_RATIO) & B_PLATFORM_INFO_RATIO_MASK));
//[-end-121022-IB03780465-modify]//
  PlatformCpuInfo->MaxCoreFrequency.Exponent = PcdGet16 (PcdMaxCoreFrequencyExponent);

  PlatformCpuInfo->MaxFsbFrequency.Value     = PcdGet16 (PcdMaxFsbFrequencyValue);
  PlatformCpuInfo->MaxFsbFrequency.Exponent  = PcdGet16 (PcdMaxFsbFrequencyExponent);

  PlatformCpuInfo->PlatformCoreFrequencyList = mCoreFrequencyList;
  PlatformCpuInfo->PlatformFsbFrequencyList  = mFsbFrequencyList;

  PlatformCpuInfo->AssetTag                  = FillByOemToken;
  PlatformCpuInfo->SerialNumber              = FillByOemToken;
  PlatformCpuInfo->PartNumber                = FillByOemToken;

  return EFI_SUCCESS;
}

/**
 Get the microcode patch.

 @param[in]         This                Driver context.
 @param[out]        MicrocodeData       Retrieved image of the microcode.

 @retval            EFI_SUCCESS         Image found.
 @retval            EFI_NOT_FOUND       Image not found.
*/
EFI_STATUS
EFIAPI
PlatformCpuRetrieveMicrocode (
  IN  DXE_CPU_PLATFORM_POLICY_PROTOCOL      *This,
  OUT UINT8                                 **MicrocodeData
  )
{
  EFI_CPU_MICROCODE_HEADER  *Microcode;
  UINTN                     MicrocodeStart;
  UINTN                     MicrocodeEnd;
  UINTN                     TotalSize;

  MicrocodeStart = PcdGet32 (PcdFlashNvStorageMicrocodeBase);
  MicrocodeEnd   = PcdGet32 (PcdFlashNvStorageMicrocodeBase) + PcdGet32 (PcdFlashNvStorageMicrocodeSize);

  if (*MicrocodeData == NULL) {
    *MicrocodeData = (UINT8 *) (UINTN) MicrocodeStart;
  } else {
    if (*MicrocodeData < (UINT8 *) (UINTN) MicrocodeStart) {
      return EFI_NOT_FOUND;
    }

    TotalSize = (UINTN) (((EFI_CPU_MICROCODE_HEADER *) *MicrocodeData)->TotalSize);
    if (TotalSize == 0) {
      TotalSize = 2048;
    }
    //
    // Add alignment check - begin
    //
    if ((TotalSize & 0x7FF) != 0) {
      TotalSize = (TotalSize & 0xFFFFF800) + 0x800;
    }
    //
    // Add alignment check - end
    //

    *MicrocodeData += TotalSize;

    Microcode = (EFI_CPU_MICROCODE_HEADER *) *MicrocodeData;
    if (*MicrocodeData >= (UINT8 *) (UINTN) (MicrocodeEnd) || Microcode->TotalSize == (UINT32) -1) {
      return EFI_NOT_FOUND;
    }

  }

  return EFI_SUCCESS;
}

/**
 Initilize Intel Cpu DXE Platform Policy

 @param[in]         ImageHandle                   Image handle of this driver.
 @param[in]         SystemTable                   Global system service table.

 @retval            EFI_SUCCESS                   Initialization complete.
 @retval            EFI_UNSUPPORTED               The chipset is unsupported by this driver.
 @retval            EFI_OUT_OF_RESOURCES          Do not have enough resources to initialize the driver.
 @retval            EFI_DEVICE_ERROR              Device error, driver exits abnormally.
*/
EFI_STATUS
EFIAPI
CPUPolicyEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                              Status;
//[-start-120627-IB04770237-add]//
  CPU_FAMILY                              CpuFamilyId;
//[-end-120627-IB04770237-add]//
  EFI_HANDLE                              Handle;
  VOID                                    *Registration;
  DXE_CPU_PLATFORM_POLICY_PROTOCOL        *CPUPlatformPolicy;
  EFI_SETUP_UTILITY_PROTOCOL              *SetupUtility;
  CHIPSET_CONFIGURATION                    *SetupVariable;
  UINTN                                   CpuCount;
  UINTN                                   CpuCountForCorePowerLimit;
  UINT16                                  StateRatio[MAX_CUSTOM_RATIO_TABLE_ENTRIES];
  UINT8                                   Index;
//[-start-120418-IB07360189-add]//
  UINT16                                  PmCon2;
  UINT16                                  PmCon3;
//[-end-120418-IB07360189-add]//
  UINTN                                   Size;
  UINT8                                   BootType;
  Handle                    = NULL;
  CPUPlatformPolicy         = NULL;
  SetupUtility              = NULL;
  SetupVariable             = NULL;
  CpuCount                  = 0;
  CpuCountForCorePowerLimit = 0;
  Index                     = 0;

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\nCPU Policy Entry\n" ) );

  Status = gBS->LocateProtocol ( &gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility );
  ASSERT_EFI_ERROR ( Status );
  SetupVariable = ( CHIPSET_CONFIGURATION * )( SetupUtility->SetupNvData );
  BootType = DUAL_BOOT_TYPE;
  Size = sizeof (UINT8);
  Status = gRT->GetVariable (
                  L"BootType",
                  &gSystemConfigurationGuid,
                  NULL,
                  &Size,
                  &BootType
                  );
  //
  // Allocate and set CPU Policy structure to known value
  //
  CPUPlatformPolicy = NULL;
  CPUPlatformPolicy = AllocateZeroPool (sizeof (DXE_CPU_PLATFORM_POLICY_PROTOCOL));
  ASSERT (CPUPlatformPolicy != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->CpuConfig = NULL;
  CPUPlatformPolicy->CpuConfig = AllocateZeroPool (sizeof (CPU_CONFIG));
  ASSERT (CPUPlatformPolicy->CpuConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->CpuConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->PowerMgmtConfig = NULL;
  CPUPlatformPolicy->PowerMgmtConfig = AllocateZeroPool (sizeof (POWER_MGMT_CONFIG));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->PowerMgmtConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables 		= NULL;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables 		= AllocateZeroPool (sizeof (PPM_FUNCTION_ENABLES));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables 		= NULL;
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables   	= AllocateZeroPool (sizeof (THERM_FUNCTION_ENABLES));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables != NULL);
  if (CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable = NULL;
  CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable = AllocateZeroPool (sizeof (PPM_CUSTOM_RATIO_TABLE));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings = NULL;
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings = AllocateZeroPool (sizeof (PPM_TURBO_SETTINGS));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CpuCount                  = 8;
  CpuCountForCorePowerLimit = (CpuCount < 4) ? 4 : CpuCount;
  CPUPlatformPolicy->PowerMgmtConfig->pRatioLimit = NULL;
  CPUPlatformPolicy->PowerMgmtConfig->pRatioLimit = AllocateZeroPool ((sizeof ( UINT8 ) * CpuCountForCorePowerLimit));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig->pRatioLimit != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->PowerMgmtConfig->pRatioLimit == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables = NULL;
  CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables = AllocateZeroPool (sizeof (PPM_LOCK_ENABLES));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings = NULL;
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings = AllocateZeroPool (sizeof (PPM_CUSTOM_CTDP));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

//[-start-121120-IB05300359-add]//
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables = NULL;
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables = AllocateZeroPool (sizeof (THERM_FUNCTION_ENABLES));
  ASSERT (CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables != NULL);
//[-end-121120-IB05300359-add]//
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->SecurityConfig = NULL;
  CPUPlatformPolicy->SecurityConfig = AllocateZeroPool (sizeof (SECURITY_CONFIG));
  ASSERT (CPUPlatformPolicy->SecurityConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->SecurityConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig = NULL;
  CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig = AllocateZeroPool (sizeof (TXT_FUNCTION_CONFIG));
  ASSERT (CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

//[-start-130104-IB10870064-remove]//
////[-start-121120-IB05300359-modify]//
//  if (
//    ( CPUPlatformPolicy                                       == NULL ) ||
//    ( CPUPlatformPolicy->CpuConfig                            == NULL ) ||
//    ( CPUPlatformPolicy->PowerMgmtConfig                      == NULL ) ||
//    ( CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables    == NULL ) ||
//    ( CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable   == NULL ) ||
//    ( CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings      == NULL ) ||
//    ( CPUPlatformPolicy->PowerMgmtConfig->pRatioLimit         == NULL ) ||
//    ( CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables     == NULL ) ||
//    ( CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings == NULL ) ||
//    (CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables  == NULL) ||
//    ( CPUPlatformPolicy->SecurityConfig                       == NULL ) ||
//    ( CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig    == NULL )
//    ) {
//    return EFI_OUT_OF_RESOURCES;
//  }
//[-end-130104-IB10870064-remove]//
  if (EFI_ERROR (Status)) {
    return Status;
  }
//[-end-121120-IB05300359-modify]//

  //
  // CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->StateRatio
  //
  for (Index = 0; Index < MAX_CUSTOM_RATIO_TABLE_ENTRIES; Index = Index + 1) {
    StateRatio[Index] = 0;
  }

//[-start-120627-IB04770237-add]//
  CpuFamilyId = GetCpuFamily ();
//[-end-120627-IB04770237-add]//

  //
  // Update REVISION number
  //
//[-start-121212-IB06460477-modify]//
//[-start-120904-IB03780459-modify]//
//[-start-121113-IB06150258-modify]//
  CPUPlatformPolicy->Revision                                                 = DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_7;
//[-end-121113-IB06150258-modify]//
//[-end-120904-IB03780459-modify]//
//[-end-121212-IB06460477-modify]//

  //
  // Initialzie CPU Configuration
  //
//[-start-120522-IB06460400-modify]//
  CPUPlatformPolicy->CpuConfig->HtState                                       = SetupVariable->HTSupport;
  CPUPlatformPolicy->CpuConfig->LimitCpuidMaximumValue                        = SetupVariable->LimitCpuidMaximumValue;
//[-start-120806-IB04770240-remove]//
//  CPUPlatformPolicy->CpuConfig->DcaState                                      = CPU_FEATURE_DISABLE;
//[-end-120806-IB04770240-remove]//
  CPUPlatformPolicy->CpuConfig->ExecuteDisableBit                             = SetupVariable->ExecuteDisableBit;
  CPUPlatformPolicy->CpuConfig->VmxEnable                                     = SetupVariable->VTSupport;
  CPUPlatformPolicy->CpuConfig->SmxEnable                                     = SetupVariable->TXT;
//[-start-120806-IB04770240-remove]//
//  CPUPlatformPolicy->CpuConfig->FastString                                    = CPU_FEATURE_ENABLE;
//[-end-120806-IB04770240-remove]//
//[-start-120427-IB07360192-modify]//
//
// Disable MachineCheck for BSOD 0x9C Issue. It's a mistake that BWG says that this policy should be enable.
// Intel will remove it from the next rev of the HSW BWG 0.6.0.
// MSR 409h should be marked as reserved and shouldn't be read or written.
//
  CPUPlatformPolicy->CpuConfig->MachineCheckEnable                            = PcdGet8 (PcdDxeCpuConfigMachineCheckEnable);
//[-end-120427-IB07360192-modify]//
  CPUPlatformPolicy->CpuConfig->MonitorMwaitEnable                            = PcdGet8 (PcdDxeCpuConfigMonitorMwaitEnable);
  CPUPlatformPolicy->CpuConfig->XapicEnable                                   = SetupVariable->xAPICMode;
  CPUPlatformPolicy->CpuConfig->AesEnable                                     = SetupVariable->AESEnable;
//[-start-120627-IB04770237-add]//
  CPUPlatformPolicy->CpuConfig->DebugInterfaceEnable                          = PcdGet8 (PcdDxeCpuConfigDebugInterfaceEnable);
  CPUPlatformPolicy->CpuConfig->DebugInterfaceLockEnable                      = PcdGet8 (PcdDxeCpuConfigDebugInterfaceLockEnable);
//[-end-120627-IB04770237-add]//
//[-start-121022-IB03780465-remove]//
////[-start-120904-IB03780459-add]//
//  CPUPlatformPolicy->CpuConfig->MwaitHandoff                                  = PcdGet8 ( PcdDxeCpuConfigMwaitHandoff );
////[-end-120904-IB03780459-add]//
//[-end-121022-IB03780465-remove]//
  CPUPlatformPolicy->CpuConfig->MachineCheckStatusClean                       = PcdGet8 (PcdDxeCpuConfigMachineCheckStatusClean);
  CPUPlatformPolicy->CpuConfig->IsColdReset                                   = PcdGet8 (PcdDxeCpuConfigIsColdReset);
  CPUPlatformPolicy->CpuConfig->MlcStreamerPrefetcher                         = SetupVariable->HardwarePrefetcher;
  CPUPlatformPolicy->CpuConfig->MlcSpatialPrefetcher                          = SetupVariable->HardwarePrefetcher;
//[-start-130906-IB08620308-add]//
  CPUPlatformPolicy->CpuConfig->DcuStreamerPrefetcher                         = CPU_FEATURE_ENABLE;
  CPUPlatformPolicy->CpuConfig->DcuIPPrefetcher                               = CPU_FEATURE_ENABLE;
//[-end-130906-IB08620308-add]//
  CPUPlatformPolicy->CpuConfig->EnableDts                                     = SetupVariable->DtsEnable;
  CPUPlatformPolicy->CpuConfig->FviReport                                     = PcdGet8 (PcdDxeCpuConfigFviReport);
  CPUPlatformPolicy->CpuConfig->BspSelection                                  = PcdGet8 (PcdDxeCpuConfigBspSelection);
//[-start-121026-IB03780465-add]//
  CPUPlatformPolicy->CpuConfig->ApIdleManner                                  = PcdGet8 (PcdDxeCpuConfigApIdleManner);
  CPUPlatformPolicy->CpuConfig->ApHandoffManner                               = PcdGet8 (PcdDxeCpuConfigApHandoffManner);
//[-end-121026-IB03780465-add]//
//[-start-120806-IB04770240-remove]//
//   CPUPlatformPolicy->CpuConfig->DcaPrefetchDelayValue                         = 4;
//   CPUPlatformPolicy->CpuConfig->VirtualWireMode                               = 0;    // Virtual Wire A
//[-end-120806-IB04770240-remove]//
  CPUPlatformPolicy->CpuConfig->SmmbaseSwSmiNumber                            = SW_SMI_FROM_SMMBASE;
  CPUPlatformPolicy->CpuConfig->FviSmbiosType                                 = PcdGet8 (PcdMeMiscConfigFviSmbiosType);    // Default SMBIOS Type 221
  CPUPlatformPolicy->CpuConfig->RetrieveMicrocode                             = PlatformCpuRetrieveMicrocode;
  CPUPlatformPolicy->CpuConfig->GetMaxCount                                   = PlatformCpuGetMaxCount;
  CPUPlatformPolicy->CpuConfig->GetCpuInfo                                    = PlatformCpuGetCpuInfo;
//[-start-120418-IB07360189-add]//
  //
  // Get below 3 bits from CPT GEN_PMCON 2 & 3 for system warm boot checking
  // (1)GEN_PMCON_2 (0:31:0 offset 0A2) bit[5] RO - Memory placed in Self-Refresh (MEM_SR)
  // (2)GEN_PMCON_2 (0:31:0 offset 0A2) bit[1] RWCiV SYS_PWROK Failure (SYSPWR_FLR)
  // (3)GEN_PMCON_3 (0:31:0 offset 0A4) bit[1] RWC-(PWR_FLR)
  //
  PmCon2  = PchLpcPciCfg16 (R_PCH_LPC_GEN_PMCON_2);
  PmCon3  = PchLpcPciCfg16 (R_PCH_LPC_GEN_PMCON_3);

  //
  // Check GEN_PMCON_2 (0:31:0 offset 0A2) bit[5] - Memory placed in Self-Refresh (MEM_SR)
  // Bit was set if system reset w/o power cycle (warm reset) or S3 entery & exit successfully
  //
  if (PmCon2 & B_PCH_LPC_GEN_PMCON_MEM_SR) {
    CPUPlatformPolicy->CpuConfig->IsColdReset = FALSE;
  } else {
    CPUPlatformPolicy->CpuConfig->IsColdReset = TRUE;
  }

  //
  // Check GEN_PMCON_3 (0:31:0 offset 0A4) bit[1] - PWR FLR
  //
  if (PmCon3 & B_PCH_LPC_GEN_PMCON_PWR_FLR) {
    MmioWrite16 (
      MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GEN_PMCON_3),
      PmCon3
      );
    CPUPlatformPolicy->CpuConfig->IsColdReset = TRUE;
  }

  //
  // When the CPUPWR_FLR bit is set, it indicates VRMPWRGD signal from the CPU VRM went low.
  // Software must clear this bit if set.
  //
  if (PmCon2 & B_PCH_LPC_GEN_PMCON_SYSPWR_FLR) {
    //
    // BIOS clears this bit by writing a '0' to it.
    //
    PmCon2 &= ~(B_PCH_LPC_GEN_PMCON_SYSPWR_FLR);
    MmioWrite16 (
      MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GEN_PMCON_2),
      PmCon2
      );
    CPUPlatformPolicy->CpuConfig->IsColdReset = TRUE;
  }
//[-end-120418-IB07360189-add]//

  //
  // Initialzie Power Management Configuration
  //
  //
  // PPM Functional Enables
  //
//[-start-120320-IB06460377-modify]//
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->Eist                  = SetupVariable->ISTConfig;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->Cx                    = SetupVariable->CStates;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C1e                   = SetupVariable->EnCStates;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C3                    = SetupVariable->EnableC3;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C6                    = SetupVariable->EnableC6;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->DeepCState            = SetupVariable->EnableC7;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->LongLatencyC6         = (SetupVariable->EnableC6) ? SetupVariable->C6Latency : PPM_DISABLE;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->LongLatencyC7         = (SetupVariable->EnableC7) ? SetupVariable->C7Latency : PPM_DISABLE;
//[-start-120727-IB06460249-modify]//
//[-start-120627-IB04770237-add]//
  if (CpuFamilyId == EnumCpuHswUlt) {
    CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C8                    = SetupVariable->EnableC8;
    CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C9                    = SetupVariable->EnableC9;
    CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C10                   = SetupVariable->EnableC10;
  } else {
    CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C8                    = PPM_DISABLE;
    CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C9                    = PPM_DISABLE;
    CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C10                   = PPM_DISABLE;
  }
//[-end-120627-IB04770237-add]//
//[-end-120727-IB06460249-modify]//
//[-start-120917-IB03780459-remove]//
//  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->ThermalMonitor        = PcdGet32 ( PcdpFunctionEnablesThermalMonitor );
//[-end-120917-IB03780459-remove]//
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->LakeTiny              = PcdGet32 (PcdpFunctionEnablesLakeTiny);
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->TimedMwait            = PcdGet32 (PcdpFunctionEnablesTimedMwait);
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C1AutoDemotion        = (SetupVariable->CStateAutoDemotion & B_C1_AUTO_DEMOTION) ? PPM_ENABLE : PPM_DISABLE;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C3AutoDemotion        = (SetupVariable->CStateAutoDemotion & B_C3_AUTO_DEMOTION) ? PPM_ENABLE : PPM_DISABLE;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->TurboMode             = SetupVariable->EnableTurboMode;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->PowerLimit2           = SetupVariable->ShortTermPowerLimitOverride;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->EnergyEfficientPState = SetupVariable->EnergyEfficientPState;
//[-start-120917-IB03780459-remove]//
//  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->BiProcHot             = SetupVariable->EnableProcHot;
//  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->TStates               = SetupVariable->TStatesEnable;
//[-end-120917-IB03780459-remove]//
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->Xe                    = SetupVariable->ExtremeEdition;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C1UnDemotion          = (SetupVariable->CStateAutoUnDemotion & B_C1_UNDEMOTION) ? PPM_ENABLE : PPM_DISABLE;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C3UnDemotion          = (SetupVariable->CStateAutoUnDemotion & B_C3_UNDEMOTION) ? PPM_ENABLE : PPM_DISABLE;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->PkgCStateDemotion     = SetupVariable->PkgCStateDemotion;
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->PkgCStateUnDemotion   = SetupVariable->PkgCStateUnDemotion;
//[-start-120917-IB03780459-remove]//
//  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->DisableProcHotOut     = PcdGet32 ( PcdpFunctionEnablesDisableProcHotOut );
//  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->DisableVRThermalAlert = PcdGet32 ( PcdpFunctionEnablesDisableVRThermalAlert );
//  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->ProcHotResponce       = PcdGet32 ( PcdpFunctionEnablesProcHotResponce );
////[-start-120426-IB07360191-modify]//
//  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->AutoThermalReporting  = SetupVariable->AutoThermalReporting;
////[-end-120426-IB07360191-modify]//
//[-end-120917-IB03780459-remove]//
  CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->CStatePreWake         = SetupVariable->CStatePreWake;
//[-end-120320-IB06460377-modify]//

  //
  // Custom Ratio Table
  //
  CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->NumberOfEntries      = PcdGet8 (PcdpCustomRatioTableNumberOfEntries);
  CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->Cpuid                = PcdGet32 (PcdpCustomRatioTableCpuid);
  CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->MaxRatio             = PcdGet16 (PcdpCustomRatioTableMaxRatio);
  CopyMem (CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->StateRatio, StateRatio, sizeof (UINT16) * MAX_CUSTOM_RATIO_TABLE_ENTRIES);

  //
  // PPM Turbo Settings
  //
  CPUPlatformPolicy->PowerMgmtConfig->CustomPowerUnit                         = PowerUnit125MilliWatts;  
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit1             = (SetupVariable->LongTermPowerLimitOverride) ? SetupVariable->LongDurationPowerLimit : AUTO;
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit1Time         = (SetupVariable->LongTermPowerLimitOverride) ? SetupVariable->LongDurationTimeWindow : 0;
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit2             = (CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->PowerLimit2) ? SetupVariable->ShortDurationPowerLimit : AUTO;
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->TurboPowerLimitLock     = SetupVariable->TurboPowerLimitMsrLock;
//  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->TurboPowerLimitLock     = PPM_DISABLE;
//[-start-120917-IB03780459-remove]//
//  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->DdrPowerLimit1          = SetupVariable->DdrLongTermPowerLimit;
//  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->DdrPowerLimit1Time      = SetupVariable->DdrLongTermTimeWindow;
//  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->DdrPowerLimit2          = SetupVariable->DdrShortTermPowerLimit;
//  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->DdrPowerLimitLock       = SetupVariable->DdrPowerLimitLock;
//[-end-120917-IB03780459-remove]//
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->EnergyPolicy            = SetupVariable->EnergyPolicy;
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->ConfigTdpLevel          = SetupVariable->ConfigTDPLevel;
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->ConfigTdpLock           = SetupVariable->ConfigTDPLock;

//[-start-121211-IB06460477-add]//
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit3             = PcdGet8 (PcdPowerLimit3);
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit3Time         = PcdGet8 (PcdPowerLimit3Time);
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit3DutyCycle    = PcdGet8 (PcdPowerLimit3DutyCycle);
  CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit3Lock         = PcdGetBool (PcdPowerLimit3Lock);
//[-end-121211-IB06460477-add]//

  CPUPlatformPolicy->PowerMgmtConfig->S3RestoreMsrSwSmiNumber                 = S3_RESTORE_MSR_SW_SMI;

//[-start-120320-IB06460377-modify]//
  CPUPlatformPolicy->PowerMgmtConfig->PkgCStateLimit                          = SetupVariable->PkgCStateLimit;
  CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl0TimeUnit           = SetupVariable->CstateLatencyControl0TimeUnit;
  CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl1TimeUnit           = SetupVariable->CstateLatencyControl1TimeUnit;
  CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl2TimeUnit           = SetupVariable->CstateLatencyControl2TimeUnit;
  CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl0Irtl               = SetupVariable->CstateLatencyControl0Irtl;
  CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl1Irtl               = SetupVariable->CstateLatencyControl1Irtl;
  CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl2Irtl               = SetupVariable->CstateLatencyControl2Irtl;
//[-end-120320-IB06460377-modify]//
//[-start-120727-IB06460249-modify]//
//[-start-120627-IB04770237-add]//
  if (CpuFamilyId == EnumCpuHswUlt) {
    CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl3TimeUnit         = SetupVariable->CstateLatencyControl3TimeUnit;
    CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl4TimeUnit         = SetupVariable->CstateLatencyControl4TimeUnit;
    CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl5TimeUnit         = SetupVariable->CstateLatencyControl5TimeUnit;
    CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl3Irtl             = SetupVariable->CstateLatencyControl3Irtl;
    CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl4Irtl             = SetupVariable->CstateLatencyControl4Irtl;
    CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl5Irtl             = SetupVariable->CstateLatencyControl5Irtl;
  }
//[-end-120627-IB04770237-add]//
//[-end-120727-IB06460249-modify]//
  CPUPlatformPolicy->PowerMgmtConfig->RfiFreqTunningOffsetIsNegative          = PcdGetBool (PcdPowerMgmtConfigRfiFreqTunningOffsetIsNegative);
  CPUPlatformPolicy->PowerMgmtConfig->RfiFreqTunningOffset                    = PcdGet8 (PcdPowerMgmtConfigRfiFreqTunningOffset);
  //
  // PPM Lock Enables
  //
  CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables->PmgCstCfgCtrlLock      = PcdGet32 (PcdpPpmLockEnablesPmgCstCfgCtrlLock);
  CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables->OverclockingLock       = PcdGet32 (PcdpPpmLockEnablesOverclockingLock);
  CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables->ProcHotLock            = PcdGet32 (PcdpPpmLockEnablesProcHotLock);

  //
  // PPM Custom ConfigTDP Settings
  //
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->ConfigTdpCustom    = SetupVariable->ConfigTdpCustom;

  if (SetupVariable->ConfigTDPLock) {
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomTdpCount                                       = 1;
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomBootModeIndex                                  = 0;
  } else {
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomTdpCount                                       = SetupVariable->CustomTdpCount;
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomBootModeIndex                                  = SetupVariable->CustomBootModeIndex;
  }
  //
  // Custom Setting 1
  //
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[0].CustomPowerLimit1           = SetupVariable->CustomPowerLimit10;
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[0].CustomPowerLimit2           = SetupVariable->CustomPowerLimit20; 
  if (SetupVariable->CustomPowerLimit1Time0 == 0) {
    //
    // If the value is 0, default values will be programmed (28 sec for Mobile and 1 sec from Desktop). Indicates the time window over which TDP value should be maintained.
    //
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[0].CustomPowerLimit1Time     = 28;
  } else {
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[0].CustomPowerLimit1Time     = SetupVariable->CustomPowerLimit1Time0;
  }
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[0].CustomTurboActivationRatio  = SetupVariable->CustomTurboActivationRatio0;
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[0].CustomConfigTdpControl      = SetupVariable->CustomConfigTdpControl0;
  //
  // Custom Setting 2
  //
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[1].CustomPowerLimit1           = SetupVariable->CustomPowerLimit11;
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[1].CustomPowerLimit2           = SetupVariable->CustomPowerLimit21;
  if (SetupVariable->CustomPowerLimit1Time1 == 0) {
    //
    // If the value is 0, default values will be programmed (28 sec for Mobile and 1 sec from Desktop). Indicates the time window over which TDP value should be maintained.
    //
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[1].CustomPowerLimit1Time     = 28;
  } else {
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[1].CustomPowerLimit1Time     = SetupVariable->CustomPowerLimit1Time1;
  }
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[1].CustomTurboActivationRatio  = SetupVariable->CustomTurboActivationRatio1;
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[1].CustomConfigTdpControl      = SetupVariable->CustomConfigTdpControl1;
  //
  // Custom Setting 3
  //
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[2].CustomPowerLimit1           = SetupVariable->CustomPowerLimit12;
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[2].CustomPowerLimit2           = SetupVariable->CustomPowerLimit22; 
  if (SetupVariable->CustomPowerLimit1Time2 == 0) {
    //
    // If the value is 0, default values will be programmed (28 sec for Mobile and 1 sec from Desktop). Indicates the time window over which TDP value should be maintained.
    //
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[2].CustomPowerLimit1Time     = 28;
  } else {
    CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[2].CustomPowerLimit1Time     = SetupVariable->CustomPowerLimit1Time2;
  }
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[2].CustomTurboActivationRatio  = SetupVariable->CustomTurboActivationRatio2;
  CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[2].CustomConfigTdpControl      = SetupVariable->CustomConfigTdpControl2;

//[-start-121109-IB05300357-modify]//
//[-start-120917-IB03780459-add]//
  //
  // THERM_FUNCTION_ENABLES
  //
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->BiProcHot             = SetupVariable->EnableProcHot;
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->TStates               = SetupVariable->TStatesEnable;
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->DisableProcHotOut     = PcdGet8 (PcdpFunctionEnablesDisableProcHotOut);
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->DisableVRThermalAlert = PcdGet8 (PcdpFunctionEnablesDisableVRThermalAlert);
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->ProcHotResponce       = PcdGet8 (PcdpFunctionEnablesProcHotResponce);
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->AutoThermalReporting  = SetupVariable->AutoThermalReporting;
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->ThermalMonitor        = PcdGet8 (PcdpFunctionEnablesThermalMonitor);
//[-end-120917-IB03780459-add]//
//[-end-121109-IB05300357-modify]//

  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->Pl1ThermalControl    = 2; ///< AUTO
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorIA   = Percent100;
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorGT   = Percent100;
  CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorPCH  = Percent100;

  if (FeaturePcdGet (PcdTXTSupported)) {
    CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig->ResetAux              = PcdGet8 (PcdTxtFunctionConfigResetAux);
  }

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Install CPU Policy Protocol\n"));
//[-start-121220-IB10820206-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateDxeCpuPlatformPolicy (CPUPlatformPolicy);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateDxeCpuPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    //
    // Install CPU Policy Protocol
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
               &Handle,
               &gDxeCpuPlatformPolicyProtocolGuid,
               EFI_NATIVE_INTERFACE,
               CPUPlatformPolicy
               );
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121220-IB10820206-modify]//
  //
  // Dump policy
  //
  DumpCPUPolicy (CPUPlatformPolicy);

//[-start-120924-IB03610461-add]//
  InvalidateSmramCpuData ();
//[-end-120924-IB03610461-add]//

  //
  // Initialize 0-640K main memory for WakeUpBuffer in CPU RC when the memory are untested
  //
  Status = InitializeWakeUpBuffer ();
  ASSERT_EFI_ERROR (Status);

//[-start-120816-IB05300312-modify]//
  //
  // Initial ACPI_VARIABLE_SET variable before AcpiS3Save driver
  //
  Status = InitialAcpiS3Variable ();
  ASSERT_EFI_ERROR ( Status );

  //
  // 1. Create event to update ACPI_VARIABLE_SET after CPU RC establish SMRAM_CPU_DATA_VARIABLE
  // 2. Wake AP to the new WakeUpBuffer after CRU RC reallocate event
  //
  // Ps. Notify the same GUID with CPU RC and lower the priority (This event should run after CPU RC reallocate event)
  //
  EfiCreateProtocolNotifyEvent (
    &gExitPmAuthProtocolGuid,
    TPL_CALLBACK,
    MpCpuReAllocateEventCallback,
    NULL,
    &Registration
    );
//[-start-120323-IB03600473-add]//
  if (BootType == EFI_BOOT_TYPE) {
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gNonCSMSupportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Install gNonCSMSupportProtocolGuid : %r\n",Status));

  }
//[-end-120323-IB03600473-add]//
//[-end-120522-IB06460400-modify]//

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "CPU Policy Exit\n"));

  return Status;
}

VOID
EFIAPI
DumpCPUPolicy (
  IN      DXE_CPU_PLATFORM_POLICY_PROTOCOL        *CPUPlatformPolicy
  )
{
  UINTN        Count;

  Count = 0;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "CPUPlatformPolicy ( Address : 0x%x )\n", CPUPlatformPolicy));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Revision                         : %x\n", CPUPlatformPolicy->Revision));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-CpuConfig ( Address : 0x%x )\n", CPUPlatformPolicy->CpuConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-HtState                       : %x\n", CPUPlatformPolicy->CpuConfig->HtState));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-LimitCpuidMaximumValue        : %x\n", CPUPlatformPolicy->CpuConfig->LimitCpuidMaximumValue));
//[-start-120806-IB04770240-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DcaState                      : %x\n", CPUPlatformPolicy->CpuConfig->DcaState ) );
//[-end-120806-IB04770240-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ExecuteDisableBit             : %x\n", CPUPlatformPolicy->CpuConfig->ExecuteDisableBit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-VmxEnable                     : %x\n", CPUPlatformPolicy->CpuConfig->VmxEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SmxEnable                     : %x\n", CPUPlatformPolicy->CpuConfig->SmxEnable));
//[-start-120806-IB04770240-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-FastString                    : %x\n", CPUPlatformPolicy->CpuConfig->FastString ) );
//[-end-120806-IB04770240-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MachineCheckEnable            : %x\n", CPUPlatformPolicy->CpuConfig->MachineCheckEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MonitorMwaitEnable            : %x\n", CPUPlatformPolicy->CpuConfig->MonitorMwaitEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-XapicEnable                   : %x\n", CPUPlatformPolicy->CpuConfig->XapicEnable));
//[-start-120627-IB04770237-modify]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MachineCheckStatusClean       : %x\n", CPUPlatformPolicy->CpuConfig->MachineCheckStatusClean));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-IsColdReset                   : %x\n", CPUPlatformPolicy->CpuConfig->IsColdReset));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MlcStreamerPrefetcher         : %x\n", CPUPlatformPolicy->CpuConfig->MlcStreamerPrefetcher));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-MlcSpatialPrefetcher          : %x\n", CPUPlatformPolicy->CpuConfig->MlcSpatialPrefetcher));
//[-start-130906-IB08620308-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DcuStreamerPrefetcher         : %x\n", CPUPlatformPolicy->CpuConfig->DcuStreamerPrefetcher ));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DcuIPPrefetcher               : %x\n", CPUPlatformPolicy->CpuConfig->DcuIPPrefetcher ));
//[-end-130906-IB08620308-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-EnableDts                     : %x\n", CPUPlatformPolicy->CpuConfig->EnableDts));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FviReport                     : %x\n", CPUPlatformPolicy->CpuConfig->FviReport));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-AesEnable                     : %x\n", CPUPlatformPolicy->CpuConfig->AesEnable));
//[-end-120627-IB04770237-modify]//
//[-start-120627-IB04770237-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DebugInterfaceEnable          : %x\n", CPUPlatformPolicy->CpuConfig->DebugInterfaceEnable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-DebugInterfaceLockEnable      : %x\n", CPUPlatformPolicy->CpuConfig->DebugInterfaceLockEnable));
//[-end-120627-IB04770237-add]//
//[-start-121026-IB03780465-remove]//
////[-start-120904-IB03780459-add]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-MwaitHandoff                  : %x\n", CPUPlatformPolicy->CpuConfig->MwaitHandoff ) );
////[-end-120904-IB03780459-add]//
//[-end-121026-IB03780465-remove]//
//[-start-121030-IB03780467-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                      : %x\n", CPUPlatformPolicy->CpuConfig->Rsvdbits ) );
//[-end-121030-IB03780467-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-BspSelection                  : %x\n", CPUPlatformPolicy->CpuConfig->BspSelection));
//[-start-121022-IB03780465-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ApIdleManner                  : %x\n", CPUPlatformPolicy->CpuConfig->ApIdleManner));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ApHandoffManner               : %x\n", CPUPlatformPolicy->CpuConfig->ApHandoffManner));
//[-end-121022-IB03780465-add]//
//[-start-120806-IB04770240-remove]//
//   DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DcaPrefetchDelayValue         : %x\n", CPUPlatformPolicy->CpuConfig->DcaPrefetchDelayValue ) );
//   DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-VirtualWireMode               : %x\n", CPUPlatformPolicy->CpuConfig->VirtualWireMode ) );
//[-end-120806-IB04770240-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-SmmbaseSwSmiNumber            : %x\n", CPUPlatformPolicy->CpuConfig->SmmbaseSwSmiNumber));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-FviSmbiosType                 : %x\n", CPUPlatformPolicy->CpuConfig->FviSmbiosType));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RetrieveMicrocode ( Address : 0x%x )\n", CPUPlatformPolicy->CpuConfig->RetrieveMicrocode));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GetMaxCount ( Address : 0x%x )\n", CPUPlatformPolicy->CpuConfig->GetMaxCount));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-GetCpuInfo ( Address : 0x%x )\n", CPUPlatformPolicy->CpuConfig->GetCpuInfo));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-PowerMgmtConfig ( Address : 0x%x )\n", CPUPlatformPolicy->PowerMgmtConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-pFunctionEnables ( Address : 0x%x )\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Eist                       : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->Eist));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Cx                         : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->Cx));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C1e                        : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C1e));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C3                         : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C3));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C6                         : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C6));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-DeepCState                 : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->DeepCState));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-LongLatencyC6              : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->LongLatencyC6));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-LongLatencyC7              : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->LongLatencyC7));
//[-start-120627-IB04770237-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C8                         : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C8));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C9                         : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C9));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C10                        : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C10));
//[-end-120627-IB04770237-add]//
//[-start-120917-IB03780459-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-ThermalMonitor             : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->ThermalMonitor ) );  
//[-end-120917-IB03780459-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-LakeTiny                   : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->LakeTiny));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C1AutoDemotion             : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C1AutoDemotion));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C3AutoDemotion             : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C3AutoDemotion));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TurboMode                  : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->TurboMode));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PowerLimit2                : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->PowerLimit2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-EnergyEfficientPState      : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->EnergyEfficientPState));
//[-start-120917-IB03780459-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-BiProcHot                  : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->BiProcHot ) ); 
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-TStates                    : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->TStates ) ); 
//[-end-120917-IB03780459-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Xe                         : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->Xe));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C1UnDemotion               : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C1UnDemotion));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-C3UnDemotion               : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->C3UnDemotion));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PkgCStateDemotion          : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->PkgCStateDemotion));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PkgCStateUnDemotion        : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->PkgCStateUnDemotion));
//[-start-120917-IB03780459-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DisableProcHotOut          : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->DisableProcHotOut ) ); 
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DisableVRThermalAlert      : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->DisableVRThermalAlert ) ); 
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-ProcHotResponce            : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->ProcHotResponce ) ); 
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-AutoThermalReporting       : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->AutoThermalReporting ) ); 
//[-end-120917-IB03780459-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-CStatePreWake              : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->CStatePreWake));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Reserved                   : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pFunctionEnables->Reserved));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-pCustomRatioTable ( Address : 0x%x )\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-NumberOfEntries            : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->NumberOfEntries));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Cpuid                      : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->Cpuid));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-MaxRatio                   : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->MaxRatio));
  for (Count = 0; Count < MAX_CUSTOM_RATIO_TABLE_ENTRIES; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-StateRatio[%2x]             : %x\n", Count, CPUPlatformPolicy->PowerMgmtConfig->pCustomRatioTable->StateRatio[Count]));
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CustomPowerUnit               : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CustomPowerUnit ) );  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-pTurboSettings ( Address : 0x%x )\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PowerLimit1                : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit1));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PowerLimit1Time            : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit1Time));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PowerLimit2                : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit2));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TurboPowerLimitLock        : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->TurboPowerLimitLock));
//[-start-120917-IB03780459-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DdrPowerLimit1             : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->DdrPowerLimit1 ) ); 
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DdrPowerLimit1Time         : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->DdrPowerLimit1Time ) ); 
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DdrPowerLimit2             : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->DdrPowerLimit2 ) ); 
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DdrPowerLimitLock          : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->DdrPowerLimitLock ) ); 
//[-end-120917-IB03780459-remove]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-EnergyPolicy               : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->EnergyPolicy));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ConfigTdpLevel             : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->ConfigTdpLevel));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ConfigTdpLock              : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->ConfigTdpLock));
//[-start-121211-IB06460477-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PowerLimit3                : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit3 ) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PowerLimit3Time            : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit3Time ) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PowerLimit3DutyCycle       : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit3DutyCycle ) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PowerLimit3Lock            : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pTurboSettings->PowerLimit3Lock ) );   
//[-end-121211-IB06460477-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-S3RestoreMsrSwSmiNumber       : %x\n", CPUPlatformPolicy->PowerMgmtConfig->S3RestoreMsrSwSmiNumber));

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-PkgCStateLimit                : %x\n", CPUPlatformPolicy->PowerMgmtConfig->PkgCStateLimit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl0TimeUnit : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl0TimeUnit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl1TimeUnit : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl1TimeUnit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl2TimeUnit : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl2TimeUnit));
//[-start-120627-IB04770237-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl3TimeUnit : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl3TimeUnit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl4TimeUnit : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl4TimeUnit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl5TimeUnit : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl5TimeUnit));
//[-end-120627-IB04770237-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl0Irtl     : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl0Irtl));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl1Irtl     : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl1Irtl));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl2Irtl     : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl2Irtl));
//[-start-120627-IB04770237-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl3Irtl     : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl3Irtl));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl4Irtl     : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl4Irtl));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-CstateLatencyControl5Irtl     : %x\n", CPUPlatformPolicy->PowerMgmtConfig->CstateLatencyControl5Irtl));
//[-end-120627-IB04770237-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-RfiFreqTunningOffsetIsNegative: %x\n", CPUPlatformPolicy->PowerMgmtConfig->RfiFreqTunningOffsetIsNegative));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-S3RestoreMsrSwSmiNumber       : %x\n", CPUPlatformPolicy->PowerMgmtConfig->RfiFreqTunningOffset));
  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-pRatioLimit                   : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pRatioLimit));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-pPpmLockEnables ( Address : 0x%x )\n", CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-PmgCstCfgCtrlLock          : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables->PmgCstCfgCtrlLock));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-OverclockingLock           : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables->OverclockingLock));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ProcHotLock                : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables->ProcHotLock));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Reserved                   : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pPpmLockEnables->Reserved));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-pCustomCtdpSettings ( Address : 0x%x )\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ConfigTdpCustom            : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->ConfigTdpCustom));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-CustomTdpCount             : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomTdpCount));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-CustomBootModeIndex        : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomBootModeIndex));
  for (Count = 0; Count < MAX_CUSTOM_CTDP_ENTRIES; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-CustomConfigTdpTable[%2x]   : %x\n", Count) );  
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-CustomPowerLimit1       : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[Count].CustomPowerLimit1));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-CustomPowerLimit2       : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[Count].CustomPowerLimit2));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-CustomPowerLimit1Time   : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[Count].CustomPowerLimit1Time));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-CustomTurboActivationRatio : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[Count].CustomTurboActivationRatio));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-CustomConfigTdpControl  : %x\n", CPUPlatformPolicy->PowerMgmtConfig->pCustomCtdpSettings->CustomConfigTdpTable[Count].CustomConfigTdpControl));
  }
//[-start-120917-IB03780459-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-ThermalFuncEnables ( Address : 0x%x )\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-BiProcHot                  : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->BiProcHot));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-TStates                    : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->TStates));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-DisableProcHotOut          : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->DisableProcHotOut));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-DisableVRThermalAlert      : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->DisableVRThermalAlert));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ProcHotResponce            : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->ProcHotResponce));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-AutoThermalReporting       : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->AutoThermalReporting));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ThermalMonitor             : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->ThermalMonitor));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Pl1ThermalControl          : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->Pl1ThermalControl));
//[-end-120917-IB03780459-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Pl1ThermalControlFloor     \n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-FloorIA                 : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorIA));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-FloorGT                 : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorGT));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-FloorPCH                : %x\n", CPUPlatformPolicy->PowerMgmtConfig->ThermalFuncEnables->Pl1ThermalControlFloor.FloorPCH));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-SecurityConfig ( Address : 0x%x )\n", CPUPlatformPolicy->SecurityConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |-TxtFunctionConfig ( Address : 0x%x )\n", CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ResetAux                   : %x\n", CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig->ResetAux));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-Reserved                   : %x\n", CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig->Reserved));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |  |  |-ByetReserved               : %x\n", CPUPlatformPolicy->SecurityConfig->TxtFunctionConfig->ByetReserved[0]));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n" ) );

  return;
}

//[-start-120924-IB03610461-add]//
/**

 Invalidate SMRAM CPU Data that should not be generated yet.

**/
VOID
InvalidateSmramCpuData (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_ACCESS_PROTOCOL        *SmmAccess;
  UINTN                          Size;
  EFI_SMRAM_DESCRIPTOR           *SmramRangesPtr;
  EFI_SMRAM_DESCRIPTOR           *SmramRanges;
  UINTN                          SmramRangeCount;
  UINTN                          Index;
  UINT32                         Address;
  SMRAM_CPU_DATA                 *SmramCpuData;

  SmmAccess       = NULL;
  Size            = 0;
  SmramRangesPtr  = NULL;
  SmramRanges     = NULL;
  SmramRangeCount = 0;
  Index           = 0;
  Address         = 0;
  SmramCpuData    = NULL;

  //
  // Locate SMM Access Protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmmAccessProtocolGuid, NULL, (VOID **)&SmmAccess);
  ASSERT_EFI_ERROR (Status);

  //
  // Get SMRAM information
  //
  Size   = 0;
  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);

  SmramRangesPtr = NULL;
  SmramRangesPtr = AllocateZeroPool (Size);
  SmramRanges    = SmramRangesPtr;
  ASSERT (SmramRanges != NULL);

  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, SmramRanges);
  ASSERT_EFI_ERROR (Status);

  //
  // Open all SMRAM ranges
  //
  SmramRangeCount = Size / sizeof (*SmramRanges);
  for (Index = 0; Index < SmramRangeCount; Index = Index + 1) {
    Status = SmmAccess->Open (SmmAccess, Index);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // BUGBUG : We assume T-SEG is the last range of SMRAM in SmramRanges
  //
  SmramRanges = SmramRanges + (SmramRangeCount - 1);

  //
  // Search SMRAM on page alignment for the SMMNVS signature
  //
  for (Address = ( UINT32 )( SmramRanges->CpuStart + SmramRanges->PhysicalSize - EFI_PAGE_SIZE ); Address >= ( UINT32 )SmramRanges->CpuStart; Address = Address - EFI_PAGE_SIZE) {
    SmramCpuData = ( SMRAM_CPU_DATA * )( UINTN )Address;
    if (CompareGuid (&SmramCpuData->HeaderGuid, &gSmramCpuDataHeaderGuid)) {
      //
      // Invalidate it
      //
      ZeroMem (&SmramCpuData->HeaderGuid, sizeof (SmramCpuData->HeaderGuid));
    }
  }

  FreePool (SmramRangesPtr);

  return;
}
//[-end-120924-IB03610461-add]//


/**
 The function is to initial the untested main memory below 640K.
 Not all MRC initial and report 0-640K as tested main memory.
 If the memory ranges are untested, it cannot be allocated and used.
 CPU RC remove the tested function but it still allocate a temp buffer below 640K.

 @param             N/A
 
 @retval            EFI_SUCCESS:        The function completed successfully.
*/
EFI_STATUS
InitializeWakeUpBuffer (
  )
{
  EFI_GENERIC_MEMORY_TEST_PROTOCOL   *GenMemoryTest;
  EFI_PHYSICAL_ADDRESS               StartAddress;
  EFI_STATUS                         Status;

  //
  // Locate GenericMemoryTest Protocol for untested main memory
  //
  Status = gBS->LocateProtocol (
                  &gEfiGenericMemTestProtocolGuid,
                  NULL,
                  (VOID **)&GenMemoryTest
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  for (StartAddress = 0; StartAddress < 0xA0000; StartAddress += 0x1000) {
    //
    // Test 0-640K main memory
    //
    Status = GenMemoryTest->CompatibleRangeTest (
      GenMemoryTest,
      StartAddress,
      0x1000
      );
    
    if (EFI_ERROR (Status)) {
      continue;
    }
  }

  return EFI_SUCCESS;
}

/**
  The function is used to establish ACPI_VARIABLE_SET for S3 usage.
  CPU RC remove the parts to establish ACPI_VARIABLE_SET now, and it need to be
  established before AcpiS3Save driver in first boot.
**/
EFI_STATUS
InitialAcpiS3Variable (
  )
{
  ACPI_VARIABLE_SET           *AcpiVariableSet;
//[-start-121003-IB10820132-remove]//
//  EFI_GUID                    gEfiAcpiVariableGuid = EFI_ACPI_VARIABLE_GUID;
//[-end-121003-IB10820132-remove]//
  UINTN                       Pages;
  EFI_PHYSICAL_ADDRESS        Address;
  EFI_STATUS                  Status;

  //
  // Allocate reserved memory to save ACPI_VARIABLE_SET for S3 usage.
  //
  Pages = EFI_SIZE_TO_PAGES (sizeof (ACPI_VARIABLE_SET));
  Address = 0xFFFFFFFF;
  Status  = (gBS->AllocatePages) (
                    AllocateMaxAddress,
                    EfiReservedMemoryType,
                    Pages,
                    &Address
                    );

  if (!EFI_ERROR (Status)) {
    //
    // Initial ACPI_VARIABLE_SET for MRC and S3resume PEIMs in S3 path.
    //
    AcpiVariableSet = (ACPI_VARIABLE_SET *) Address;
    ZeroMem (AcpiVariableSet, sizeof (ACPI_VARIABLE_SET));

    Status = gRT->SetVariable (
                    ACPI_GLOBAL_VARIABLE,
//[-start-121003-IB10820132-modify]//
                    &gAcpiVariableSetGuid,
//[-end-121003-IB10820132-modify]//
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof (UINTN),
                    &AcpiVariableSet
                    );
  }

  return Status;
}

/**
 The function is used to :
 1. Update CPU_ACPI_DATA in the ACPI_VARIABLE_SET variable for S3 usage.
    CPU RC remove the parts to update ACPI_VARIABLE_SET now.
    CPU_ACPI_DATA is from CPU RC driver and needs to be updated during POST for MRC and S3resume PEIMs in S3 path.
 2. Initial AP to a known state after new buffer prepared.
    CPU RC only reallocate and config new setting in new WakeUpBuffer.
    AP still hlt in old WakeUpBuffer with old setting.

 For example:
  CPU RC reallocate event config WakeUpApManner to WakeUpApPerHltLoop in new buffer.
  But AP still keep previous setting(WakeUpApPerMwaitLoop) because AP hlt in old buffer.
  This setting must be changed to WakeUpApPerHltLoop before booting to OS.

 Note: Change in this function
  AP hlt will transit from 0x58000 to 0x9F000 (memory position depends on CPU RC programming).
  AP WakeUpApManner will transit from WakeUpApPerMwaitLoop to WakeUpApPerHltLoop (WakeUpApManner depends on CPU RC programming, too).
 @param[in]         Event               Event whose notification function is being invoked.
 @param[in]         Context             Pointer to the notification function's context.
*/
VOID
EFIAPI
MpCpuReAllocateEventCallback (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
{
  SMRAM_CPU_DATA_ADDRESS      SmramCpuDataAddr;
  SMRAM_CPU_DATA              *SmramCpuDataTemplate;
  ACPI_VARIABLE_SET           *AcpiVariableSet;
//[-start-121003-IB10820132-remove]//
//  EFI_GUID                    gEfiAcpiVariableGuid = EFI_ACPI_VARIABLE_GUID;
//[-end-121003-IB10820132-remove]//
  UINTN                       VarSize;
  UINT32                      VarAttrib;
  EFI_STATUS                  Status;
  VOID                        *ExitPmAuth;
  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL    *MpService;
  MP_CPU_EXCHANGE_INFO        *ExchangeInfo;

  //
  // Check whether this is real ExitPmAuth notification
  //
  Status = gBS->LocateProtocol (&gExitPmAuthProtocolGuid, NULL, (VOID **)&ExitPmAuth);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Locate MP Service Protocol for AP init
  //
  Status = gBS->LocateProtocol (&gFrameworkEfiMpServiceProtocolGuid, NULL, (VOID **)&MpService);
  ASSERT_EFI_ERROR (Status);

  //
  // Get ACPI_VARIABLE_SET variable and update it in this function.
  //
  VarAttrib = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  VarSize   = sizeof (UINTN);
  Status    = gRT->GetVariable (
                     ACPI_GLOBAL_VARIABLE,
//[-start-121003-IB10820132-modify]//
                     &gAcpiVariableSetGuid,
//[-end-121003-IB10820132-modify]//
                     &VarAttrib,
                     &VarSize,
                     &AcpiVariableSet
                     );
  ASSERT_EFI_ERROR (Status);

  //
  // Get ACPI_CPU_DATA from SMRAM_CPU_DATA_VARIABLE variable established by CPU RC.
  //
  VarSize = sizeof (SmramCpuDataAddr);
  Status = gRT->GetVariable (
                  SMRAM_CPU_DATA_VARIABLE,
                  &gSmramCpuDataVariableGuid,
                  NULL,
                  &VarSize,
                  &SmramCpuDataAddr
                  );
  ASSERT_EFI_ERROR (Status);
  
  SmramCpuDataTemplate = (SMRAM_CPU_DATA *) SmramCpuDataAddr.SmramCpuData;

  //
  // Only update CPU_ACPI_DATA here, other parts will be updated by AcpiS3Save driver.
  //
  AcpiVariableSet->AcpiCpuData.APState                = SmramCpuDataTemplate->AcpiCpuData.APState;
  AcpiVariableSet->AcpiCpuData.S3BootPath             = SmramCpuDataTemplate->AcpiCpuData.S3BootPath;
  AcpiVariableSet->AcpiCpuData.WakeUpBuffer           = SmramCpuDataTemplate->AcpiCpuData.WakeUpBuffer;
  AcpiVariableSet->AcpiCpuData.GdtrProfile            = SmramCpuDataTemplate->AcpiCpuData.GdtrProfile;
  AcpiVariableSet->AcpiCpuData.IdtrProfile            = SmramCpuDataTemplate->AcpiCpuData.IdtrProfile;
  AcpiVariableSet->AcpiCpuData.CpuPrivateData         = SmramCpuDataTemplate->AcpiCpuData.CpuPrivateData;
  AcpiVariableSet->AcpiCpuData.StackAddress           = SmramCpuDataTemplate->AcpiCpuData.StackAddress;
  AcpiVariableSet->AcpiCpuData.MicrocodePointerBuffer = SmramCpuDataTemplate->AcpiCpuData.MicrocodePointerBuffer;
  AcpiVariableSet->AcpiCpuData.SmramBase              = SmramCpuDataTemplate->AcpiCpuData.SmramBase;
  AcpiVariableSet->AcpiCpuData.SmmStartImageBase      = SmramCpuDataTemplate->AcpiCpuData.SmmStartImageBase;
  AcpiVariableSet->AcpiCpuData.SmmStartImageSize      = SmramCpuDataTemplate->AcpiCpuData.SmmStartImageSize;
  AcpiVariableSet->AcpiCpuData.NumberOfCpus           = SmramCpuDataTemplate->AcpiCpuData.NumberOfCpus;

  //
  // Override WakeUpApManner for CPU RC 0.5.1 update. Temp to fix S4 issue.
  //
  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (AcpiVariableSet->AcpiCpuData.WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  ExchangeInfo->WakeUpApManner = WakeUpApPerHltLoop;

  //
  // Wake up AP to the new WakeUpBuffer after CPU RC reallocate AP WakeUpBuffer
  //
  Status = MpService->StartupAllAPs (
                        MpService,
                        (EFI_AP_PROCEDURE) NullFunctionForApInit,
                        FALSE,
                        NULL,
                        0,
                        NULL,
                        NULL
                        );
  ASSERT_EFI_ERROR (Status);
  
  return;
}

/*

 @param             N/A

 @retval            EFI_SUCCESS
*/
EFI_STATUS
NullFunctionForApInit (
  )
{
  return EFI_SUCCESS;
}

