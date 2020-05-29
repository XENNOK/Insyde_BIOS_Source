/** @file

  The PEIM implements the SA PEI Initialization.

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

#include "SaInitPeim.h"

#ifdef RAPID_START_FLAG
#include <Ppi/RapidStart.h>
#endif

static EFI_PEI_PPI_DESCRIPTOR     mSaPeiInitPpi[] = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gSaPeiInitPpiGuid,
  NULL
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mSaResetCompleteNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gMemoryInitHobGuid,
  SaResetComplete
};

//EFI_GUID  gEfiPeiEndOfPeiPhasePpiGuid = EFI_PEI_END_OF_PEI_PHASE_PPI_GUID;
STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mSaS3ResumeNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  SaS3ResumeAtEndOfPei
};

#ifdef RAPID_START_FLAG
STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mSaOnRapidStartPpiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gRapidStartPpiGuid,
  SaCheckRapidStartMode
};
#endif

///
/// Functions
///
static
EFI_STATUS
EFIAPI
SaS3ResumeAtEndOfPei (
  IN       EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
  IN       VOID                        *Ppi
  )
/**
  This function handles SA S3 resume task

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
{
  SA_DATA_HOB                 *SaDataHob;
  CPU_STEPPING                CpuSteppingId;
  CPU_FAMILY                  CpuFamilyId;

  CpuSteppingId = GetCpuStepping();
  CpuFamilyId   = GetCpuFamily();
  ///
  /// Get SaPegDataHob HOB
  ///
  SaDataHob = NULL;
  SaDataHob = (SA_DATA_HOB *)GetFirstGuidHob (&gSaDataHobGuid);
  if (SaDataHob != NULL) {
     ///
     /// If there was no DXE mode supported, always enable SMM mode
     ///
#if SA_PCIE_ASPM_IN_DXE == 0
    SaDataHob->InitPcieAspmAfterOprom = 1;
#endif
     ///
     /// If there was no SMM mode supported, always enable DXE mode
     ///
#if SA_PCIE_ASPM_IN_SMM == 0
    SaDataHob->InitPcieAspmAfterOprom = 0;
#endif

    if ((SaDataHob->SaIotrapSmiAddress != 0) && (SaDataHob->InitPcieAspmAfterOprom == 1)) {
#if SA_PCIE_ASPM_IN_SMM == 1
      ///
      /// Always generate SA IO TRAP SMI when supported
      /// The SMI handler will directly return if not PCIe ASPM init after Oprom was enabled
      ///
      DEBUG ((EFI_D_INFO, "Generate SA IOTRAP SMI port=%X\n", SaDataHob->SaIotrapSmiAddress));
      IoWrite8 (SaDataHob->SaIotrapSmiAddress, 0);
#endif
    }
  }
#if SA_PCIE_ASPM_IN_DXE == 1
  if ((SaDataHob == NULL) || (SaDataHob->InitPcieAspmAfterOprom == 0)) {
    ///
    /// When SaDataHob not preset/corrupted or InitPcieAspmAfterOprom set to 0,
    /// try to do DXE mode S3 resume task.
    ///
    ///
    /// Lock processor/chipset BAR registers
    /// Other save/restore were done by S3 Save script table
    ///
    if (!(((CpuFamilyId == EnumCpuHsw)    && (CpuSteppingId <= EnumHswB0)   ) ||
          ((CpuFamilyId == EnumCpuHswUlt) && (CpuSteppingId <= EnumHswUltB0)) ||
          ((CpuFamilyId == EnumCpuCrw)    && (CpuSteppingId <= EnumCrwB0)   ) )) {
      AsmMsrOr64 (0x2e7, 1);
    }
  }
#endif
  return EFI_SUCCESS;
}

#ifdef RAPID_START_FLAG
static
EFI_STATUS
SaCheckRapidStartMode (
  IN       EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN       VOID                      *Ppi
  )
/**
  This function will check Rapid Start mode and install SaS3Resume callback notify if it was Rapid Start Resume

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
{
  RAPID_START_PPI     *RapidStartPpi;
  EFI_STATUS          Status;

  ///
  /// When Rapid Start implemented and in the Rapid Start Resume transition, this SA S3 resume Notify should happen early than Rapid Start End-Of-Pei callback
  /// This is because Rapid Start End-Of-Pei callback will generate ACPI_ENABLE SMI which should be after SA S3 resume completed.
  ///
  Status = PeiServicesLocatePpi (&gRapidStartPpiGuid, 0, NULL, (VOID **)&RapidStartPpi);
  ASSERT_EFI_ERROR (Status);
  if (RapidStartPpi->GetMode (RapidStartPpi) == RapidStartExit) {
    DEBUG ((EFI_D_INFO, "[SA] Install Notify callback for Rapid Start Resume\n"));
    Status = PeiServicesNotifyPpi (&mSaS3ResumeNotifyDesc);
    ASSERT_EFI_ERROR (Status);
  }
  return EFI_SUCCESS;
}
#endif

EFI_STATUS
EFIAPI
SaInitPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE         FileHandle,
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
/**
  SA PEI Initialization.

  @param[in] FfsHeader    - Pointer to Firmware File System file header.
  @param[in] PeiServices  - General purpose services available to every PEIM.

  @retval EFI_SUCCESS
**/
{
  EFI_STATUS              Status;
  SA_PLATFORM_POLICY_PPI  *SaPlatformPolicyPpi;
  EFI_BOOT_MODE           BootMode;
#ifdef RAPID_START_FLAG
  RAPID_START_PPI         *RapidStartPpi;
#endif
#if defined(DMI_FLAG) || defined(PEG_FLAG)
  CPU_FAMILY              CpuFamilyId;

  CpuFamilyId = GetCpuFamily();
#endif // DMI_FLAG || PEG_FLAG

  ///
  /// Get platform policy settings through the SaPlatformPolicy PPI
  ///
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gSaPlatformPolicyPpiGuid,
                            0,
                            NULL,
                            (VOID **) &SaPlatformPolicyPpi
                            );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Dump SA Platform Policy
  ///
  SaPeiPolicyDump (SaPlatformPolicyPpi);

  ///
  /// Program SA Bar Registers
  ///
  DEBUG ((EFI_D_INFO, "Programming SA Bars\n"));
  ProgramSaBars (SaPlatformPolicyPpi);

  ///
  /// Install SA HOBs
  ///
  InstallSaHob (PeiServices, SaPlatformPolicyPpi);

  ///
  /// Report SA PCIe code version
  ///
  DEBUG ((EFI_D_INFO, "Reporting SA PCIe code version\n"));
  ReportPcieVersion (SaPlatformPolicyPpi);

#ifdef DMI_FLAG
  if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuCrw)) {
    ///
    /// Initialize DMI
    ///
    DEBUG ((EFI_D_INFO, "Initializing DMI\n"));
    DmiInit (PeiServices, SaPlatformPolicyPpi);
  }
#endif // DMI_FLAG

//[-start-140220-IB10920078-modify]//
//[-start-120717-IB10040004-remove]//
//[-start-120627-IB05330350-modify]//
#ifdef SG_SUPPORT
//[-end-120627-IB05330350-modify]//
  ///
  /// Initialize SwitchableGraphics
  ///
  DEBUG ((DEBUG_INFO, "Initializing SwitchableGraphics\n"));
  SwitchableGraphicsInit (PeiServices, SaPlatformPolicyPpi);
#endif
//[-end-120717-IB10040004-remove]//
//[-end-140220-IB10920078-modify]//


#ifdef PEG_FLAG
  if ((CpuFamilyId == EnumCpuHsw) || (CpuFamilyId == EnumCpuCrw)) {
    ///
    /// Initialize SA PCIe
    ///
    DEBUG ((EFI_D_INFO, "Initializing SA PCIe\n"));
    PciExpressInit (PeiServices, SaPlatformPolicyPpi);
  }
#endif // PEG_FLAG

  ///
  /// Initialize Graphics (IGD/External)
  ///
  DEBUG ((EFI_D_INFO, "Initializing Graphics\n"));
  GraphicsInit (PeiServices, SaPlatformPolicyPpi);

  ///
  /// Initialize Overclocking
  ///
  DEBUG ((EFI_D_INFO, "Initializing System Agent Overclocking\n"));
  SaOcInit(PeiServices, SaPlatformPolicyPpi);

  ///
  /// Initialize DMI Tc/Vc mapping setting
  ///
  DEBUG ((EFI_D_INFO, "Initializing DMI Tc/Vc mapping\n"));
  SaDmiTcVcInit (PeiServices, SaPlatformPolicyPpi);

  ///
  /// Early BIOS POST Programming
  ///
  DEBUG ((EFI_D_INFO, "Early BIOS POST Programming\n"));
  EarlyBiosPostProgramming(SaPlatformPolicyPpi);

  ///
  /// Install Notify
  ///
  Status = PeiServicesNotifyPpi (&mSaResetCompleteNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Install SA S3 resume Notify only when booting from S3 resume
  ///
  Status = PeiServicesGetBootMode (&BootMode);
  DEBUG ((EFI_D_INFO, "[SA] BootMode = %X\n", BootMode));
  if ((Status == EFI_SUCCESS) && (BootMode == BOOT_ON_S3_RESUME)) {
    DEBUG ((EFI_D_INFO, "[SA] Install SA S3 Notify callback\n"));
    Status = PeiServicesNotifyPpi (&mSaS3ResumeNotifyDesc);
    ASSERT_EFI_ERROR (Status);
  }
#ifdef RAPID_START_FLAG
  else {
    ///
    /// When Rapid Start implemented and in the Rapid Start Resume transition, this SA S3 resume Notify should happen early than Rapid Start End-Of-Pei callback
    /// This is because Rapid Start End-Of-Pei callback will generate ACPI_ENABLE SMI which should be after SA S3 resume completed.
    ///
    Status = PeiServicesLocatePpi (&gRapidStartPpiGuid, 0, NULL, (VOID **)&RapidStartPpi);
    if (Status == EFI_SUCCESS) {
      DEBUG ((EFI_D_INFO, "[SA] Check Rapid Start transition mode\n"));
      if (RapidStartPpi->GetMode (RapidStartPpi) == RapidStartExit) {
        DEBUG ((EFI_D_INFO, "[SA] Install Notify callback for Rapid Start Resume\n"));
        Status = PeiServicesNotifyPpi (&mSaS3ResumeNotifyDesc);
        ASSERT_EFI_ERROR (Status);
      }
    } else {
      DEBUG ((EFI_D_INFO, "[SA] Postpone Rapid Start mode checking after RapidStartPpi installed\n"));
      Status = PeiServicesNotifyPpi (&mSaOnRapidStartPpiNotifyDesc);
    }
  }
#endif

//[-start-120717-IB10040004-remove]//
//[-start-120627-IB05330350-modify]//
//#ifdef SWITCHABLE_GRAPHICS_SUPPORT
//[-end-120627-IB05330350-modify]//
//  ///
//  /// Initialize SwitchableGraphics
//  ///
//  DEBUG ((DEBUG_INFO, "Initializing SwitchableGraphics\n"));
//  SwitchableGraphicsInit (PeiServices, SaPlatformPolicyPpi);
//#endif
//[-end-120717-IB10040004-remove]//

  ///
  /// Install Ppi with SaInitPeim complete
  ///
  Status = (**PeiServices).InstallPpi (PeiServices, mSaPeiInitPpi);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
SaResetComplete (
  IN       EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
  IN       VOID                        *Ppi
  )
/**
  BIOS_RESET_CPL bit is set for processor to activate the power and thermal management
  features on the platform.

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that
                  caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS
**/
{
  EFI_STATUS              Status;
  UINT64                  MchBar;
  UINT32                  Data32And;
  UINT32                  Data32Or;
  SA_PLATFORM_POLICY_PPI  *SaPlatformPolicyPpi;

  MchBar = McD0PciCfg64 (R_SA_MCHBAR) &~BIT0;

  ///
  /// Get platform policy settings through the SaPlatformPolicy PPI
  ///
  Status = PeiServicesLocatePpi (&gSaPlatformPolicyPpiGuid, 0, NULL, (VOID **) &SaPlatformPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Set MCHBAR Offset 5F00h [10:9] = 11b
  ///
  Data32And = (UINT32)~(BIT10 | BIT9);
  Data32Or  = 0x3 << 9;
  Mmio32AndThenOr (MchBar, R_SA_MCHBAR_SAPMCTL_OFFSET, Data32And, Data32Or);

  ///
  /// Set BIOS_RESET_CPL
  ///
  DEBUG ((EFI_D_INFO, "Set BIOS_RESET_CPL to indicate all configurations complete\n"));
  Mmio8Or ((UINTN) MchBar, R_SA_MCHBAR_BIOS_RESET_CPL_OFFSET, BIT0 | BIT1);

  return EFI_SUCCESS;
}

VOID
ProgramSaBars (
  IN    SA_PLATFORM_POLICY_PPI   *SaPlatformPolicyPpi
  )
/**
  Programs SA Bars

  @param[in] SaPlatformPolicyPpi - Instance of SA_PLATFORM_POLICY_PPI
**/
{
  CPU_FAMILY  CpuFamilyId;

  CpuFamilyId = GetCpuFamily();

  ///
  /// Program SA MchBar, DmiBar and EpBar
  ///
  McD0PciCfg64 (R_SA_MCHBAR)   = (UINT64) (SaPlatformPolicyPpi->PlatformData->MchBar | BIT0);
  McD0PciCfg64 (R_SA_DMIBAR)   = (UINT64) (SaPlatformPolicyPpi->PlatformData->DmiBar | BIT0);
  McD0PciCfg64 (R_SA_PXPEPBAR) = (UINT64) (SaPlatformPolicyPpi->PlatformData->EpBar  | BIT0);

  ///
  /// Program SA GdxcBar
  ///
  Mmio64 ((UINTN) (SaPlatformPolicyPpi->PlatformData->MchBar), R_SA_MCHBAR_GDXCBAR_OFFSET) = (UINT64)
    (SaPlatformPolicyPpi->PlatformData->GdxcBar | BIT0);

  if (CpuFamilyId == EnumCpuCrw) {
    ///
    /// Program SA EdramBar
    ///
    Mmio64 ((UINTN) (SaPlatformPolicyPpi->PlatformData->MchBar), R_SA_MCHBAR_EDRAMBAR_OFFSET) = (UINT64)
      (SaPlatformPolicyPpi->PlatformData->EdramBar | BIT0);
  }
}

VOID
EarlyBiosPostProgramming (
  IN    SA_PLATFORM_POLICY_PPI   *SaPlatformPolicyPpi
  )
/**
  Do Early BIOS POST Programming

  @param[in] SaPlatformPolicyPpi - Instance of SA_PLATFORM_POLICY_PPI
**/
{
  /// SA BS 11.1 Early BIOS POST Programming
  /// 1.  Enable System Agent Clock Gating by setting the MCHBAR offset 5F00h [0] = '1b'.
  MmioOr32 ((UINTN) (SaPlatformPolicyPpi->PlatformData->MchBar + 0x5F00), BIT0);
}

EFI_STATUS
InstallSaHob (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  )
/**
  Init and Install SA Hob

  @param[in] PeiServices  - General purpose services available to every PEIM.
  @param[in] SaPlatformPolicyPpi  - SaPlatformPolicyPpi to access the GtConfig related information

  @retval EFI_SUCCESS
**/
{
  EFI_STATUS                  Status;
  SA_DATA_HOB                 *SaDataHob;
  PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicy;

  ///
  /// Create HOB for SA Data
  ///
  Status = (**PeiServices).CreateHob (
                             PeiServices,
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             sizeof (SA_DATA_HOB),
                             (VOID **) &SaDataHob
                             );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initialize default HOB data
  ///
  SaDataHob->EfiHobGuidType.Name = gSaDataHobGuid;
  ZeroMem (&(SaDataHob->DprDirectory[EnumDprDirectoryTxt]), sizeof (DPR_DIRECTORY_ENTRY));
  ZeroMem (&(SaDataHob->DprDirectory[EnumDprDirectoryPfat]), sizeof (DPR_DIRECTORY_ENTRY));
  ZeroMem (&(SaDataHob->PegData), sizeof (SA_PEG_DATA));
  SaDataHob->PegDataValid             = FALSE;
  SaDataHob->PegPlatformResetRequired = FALSE;
  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_2) {
    SaDataHob->SaIotrapSmiAddress = SaPlatformPolicyPpi->PcieConfig->SaIotrapSmiAddress;
    SaDataHob->InitPcieAspmAfterOprom = SaPlatformPolicyPpi->PcieConfig->InitPcieAspmAfterOprom;
  } else {
    SaDataHob->SaIotrapSmiAddress = 0;
    SaDataHob->InitPcieAspmAfterOprom = 0;
  }

  ///
  /// Get platform policy settings through the SaPlatformPolicy PPI
  ///
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiCpuPlatformPolicyPpiGuid,
                            0,
                            NULL,
                            (VOID **) &CpuPlatformPolicy
                            );
  ASSERT_EFI_ERROR (Status);

  ///
  /// TXT DPR Directory Entry
  ///
#ifdef TXT_SUPPORT
  SaDataHob->DprDirectory[EnumDprDirectoryTxt].Type = DPR_DIRECTORY_TYPE_TXT;
  if (CpuPlatformPolicy->CpuConfig->Txt) {
    SaDataHob->DprDirectory[EnumDprDirectoryTxt].Size = (UINT8) RShiftU64 (CpuPlatformPolicy->SecurityConfig->TxtConfig->TxtDprMemorySize, 20);
  }
#endif

  ///
  /// PFAT Directory Entry
  ///
  SaDataHob->DprDirectory[EnumDprDirectoryPfat].Type = DPR_DIRECTORY_TYPE_PFAT;
  if (CpuPlatformPolicy->CpuConfig->Pfat) {
    SaDataHob->DprDirectory[EnumDprDirectoryPfat].Size = CpuPlatformPolicy->SecurityConfig->PfatConfig->PfatMemSize;
  }

  DEBUG ((EFI_D_INFO, "SA Data HOB installed\n"));

  return EFI_SUCCESS;
}

EFI_STATUS
ReportPcieVersion (
  IN    SA_PLATFORM_POLICY_PPI   *SaPlatformPolicyPpi
  )
/**
  Report the SA PCIe initialization code version.

  @param[in] SaPlatformPolicyPpi - Instance of SA_PLATFORM_POLICY_PPI

  @retval EFI_SUCCESS
**/
{
  UINT32            Version;
  CPU_FAMILY        CpuFamilyId;
  CPU_STEPPING      CpuSteppingId;
  const CodeVersion SaPcieCodeVersion = {
#include <SaPcieVersion.h>
  };

  DEBUG ((EFI_D_INFO, "***************** System Agent PCIe code version *****************\n"));
  DEBUG ((EFI_D_INFO, "** Major version number is: %3d                                 **\n", SaPcieCodeVersion.Major));
  DEBUG ((EFI_D_INFO, "** Minor version number is: %3d                                 **\n", SaPcieCodeVersion.Minor));
  DEBUG ((EFI_D_INFO, "** Rev version number is:   %3d                                 **\n", SaPcieCodeVersion.Rev));
  DEBUG ((EFI_D_INFO, "** Build number is:         %3d                                 **\n", SaPcieCodeVersion.Build));
  DEBUG ((EFI_D_INFO, "******************************************************************\n"));

  Version = (((UINT32) SaPcieCodeVersion.Major) << 24) | (((UINT32) SaPcieCodeVersion.Minor) << 16) | (((UINT32) SaPcieCodeVersion.Rev) << 8) | (((UINT32) SaPcieCodeVersion.Build));

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  ///
  /// Store SA Reference Code version and SA PCIe code version in scrachpad registers
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    Mmio32 ((UINTN) (SaPlatformPolicyPpi->PlatformData->MchBar), R_SA_MCHBAR_PCIE_CODE_VERSION_OFFSET_HSW) = Version;
  } else {
    Mmio32 ((UINTN) (SaPlatformPolicyPpi->PlatformData->DmiBar), R_SA_DMIBAR_SCRATCHPAD0_OFFSET) = SaPlatformPolicyPpi->Revision;
    Mmio32 ((UINTN) (SaPlatformPolicyPpi->PlatformData->DmiBar), R_SA_DMIBAR_SCRATCHPAD1_OFFSET) = Version;
  }
  return EFI_SUCCESS;
}

VOID
SaPeiPolicyDump (
  IN    SA_PLATFORM_POLICY_PPI   *SaPlatformPolicyPpi
  )
/**
  This function prints the PEI phase platform policy.

  @param[in] SaPlatformPolicyPpi - Instance of SA_PLATFORM_POLICY_PPI
**/
{
#ifdef EFI_DEBUG
  INTN   i;

  DEBUG ((EFI_D_INFO, "\n------------------------ SA Platform Policy (PEI) dump BEGIN -----------------\n"));
  DEBUG ((EFI_D_INFO, "Revision : %x\n", SaPlatformPolicyPpi->Revision));

  DEBUG ((EFI_D_INFO, "------------------------ SA_PLATFORM_DATA -----------------\n"));

  DEBUG ((EFI_D_INFO, " SpdAddressTable[%d] :", SA_MC_MAX_SOCKETS));
  for (i = 0; i < SA_MC_MAX_SOCKETS; i++) {
    DEBUG ((EFI_D_INFO, " %x", SaPlatformPolicyPpi->PlatformData->SpdAddressTable[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " MchBar : %x\n", SaPlatformPolicyPpi->PlatformData->MchBar));
  DEBUG ((EFI_D_INFO, " DmiBar : %x\n", SaPlatformPolicyPpi->PlatformData->DmiBar));
  DEBUG ((EFI_D_INFO, " EpBar : %x\n", SaPlatformPolicyPpi->PlatformData->EpBar));
  DEBUG ((EFI_D_INFO, " PciExpressBar : %x\n", SaPlatformPolicyPpi->PlatformData->PciExpressBar));
  DEBUG ((EFI_D_INFO, " SmbusBar : %x\n", SaPlatformPolicyPpi->PlatformData->SmbusBar));
  DEBUG ((EFI_D_INFO, " GdxcBar : %x\n", SaPlatformPolicyPpi->PlatformData->GdxcBar));
  DEBUG ((EFI_D_INFO, " TsegSize : %x\n", SaPlatformPolicyPpi->PlatformData->TsegSize));
  DEBUG ((EFI_D_INFO, " UserBd : %x\n", SaPlatformPolicyPpi->PlatformData->UserBd));
  DEBUG ((EFI_D_INFO, " FastBoot : %x\n", SaPlatformPolicyPpi->PlatformData->FastBoot));
  DEBUG ((EFI_D_INFO, " EdramBar : %x\n", SaPlatformPolicyPpi->PlatformData->EdramBar));

  DEBUG ((EFI_D_INFO, "------------------------ GT_CONFIGURATION -----------------\n"));
  DEBUG ((EFI_D_INFO, " MmioSize : %x MB\n", SaPlatformPolicyPpi->GtConfig->MmioSize));
  DEBUG ((EFI_D_INFO, " GttSize : %x MB\n", SaPlatformPolicyPpi->GtConfig->GttSize));
  DEBUG ((EFI_D_INFO, " IgdDvmt50PreAlloc : %x\n", SaPlatformPolicyPpi->GtConfig->IgdDvmt50PreAlloc));
  DEBUG ((EFI_D_INFO, " InternalGraphics : %x\n", SaPlatformPolicyPpi->GtConfig->InternalGraphics));
  DEBUG ((EFI_D_INFO, " PrimaryDisplay : %x\n", SaPlatformPolicyPpi->GtConfig->PrimaryDisplay));
  DEBUG ((EFI_D_INFO, " ApertureSize : %x\n", SaPlatformPolicyPpi->GtConfig->ApertureSize));
  DEBUG ((EFI_D_INFO, " GttMmAdr : %x\n", SaPlatformPolicyPpi->GtConfig->GttMmAdr));

  DEBUG ((EFI_D_INFO, "------------------------ PCIE_CONFIGURATION -----------------\n"));
  DEBUG ((EFI_D_INFO, " DmiVc1 : %x\n", SaPlatformPolicyPpi->PcieConfig->DmiVc1));
  DEBUG ((EFI_D_INFO, " DmiVcp : %x\n", SaPlatformPolicyPpi->PcieConfig->DmiVcp));
  DEBUG ((EFI_D_INFO, " DmiVcm : %x\n", SaPlatformPolicyPpi->PcieConfig->DmiVcm));
  DEBUG ((EFI_D_INFO, " DmiGen2 : %x\n", SaPlatformPolicyPpi->PcieConfig->DmiGen2));
  DEBUG ((EFI_D_INFO, " AlwaysEnablePeg : %x\n", SaPlatformPolicyPpi->PcieConfig->AlwaysEnablePeg));

  DEBUG ((EFI_D_INFO, " PegGenx[%d] :", SA_PEG_MAX_FUN));
  for (i = 0; i < SA_PEG_MAX_FUN; i++) {
    DEBUG ((EFI_D_INFO, " %x", SaPlatformPolicyPpi->PcieConfig->PegGenx[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " PegGen3Equalization : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3Equalization));

  DEBUG ((EFI_D_INFO, " Gen3RootPortPreset[%d] :", SA_PEG_MAX_LANE));
  for (i = 0; i < SA_PEG_MAX_LANE; i++) {
    DEBUG ((EFI_D_INFO, " %x", SaPlatformPolicyPpi->PcieConfig->Gen3RootPortPreset[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " Gen3EndPointPreset[%d] :", SA_PEG_MAX_LANE));
  for (i = 0; i < SA_PEG_MAX_LANE; i++) {
    DEBUG ((EFI_D_INFO, " %x", SaPlatformPolicyPpi->PcieConfig->Gen3EndPointPreset[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " Gen3EndPointHint[%d] :", SA_PEG_MAX_LANE));
  for (i = 0; i < SA_PEG_MAX_LANE; i++) {
    DEBUG ((EFI_D_INFO, " %x", SaPlatformPolicyPpi->PcieConfig->Gen3EndPointHint[i]));
  }
  DEBUG ((EFI_D_INFO, "\n"));

  DEBUG ((EFI_D_INFO, " PegSamplerCalibrate : %x\n", SaPlatformPolicyPpi->PcieConfig->PegSamplerCalibrate));
  DEBUG ((EFI_D_INFO, " PegGen3EqualizationPhase2 : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3EqualizationPhase2));
  DEBUG ((EFI_D_INFO, " PegGen3PresetSearch : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearch));
  DEBUG ((EFI_D_INFO, " PegGen3PresetSearchDwellTime : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchDwellTime));
  DEBUG ((EFI_D_INFO, " PegGen3PresetSearchMarginSteps : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchMarginSteps));
  DEBUG ((EFI_D_INFO, " PegGen3PresetSearchStartMargin : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchStartMargin));
  DEBUG ((EFI_D_INFO, " PegSwingControl : %x\n", SaPlatformPolicyPpi->PcieConfig->PegSwingControl));
  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_2) {
    DEBUG ((EFI_D_INFO, " PegGen3PresetSearchVoltageMarginSteps : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchVoltageMarginSteps));
    DEBUG ((EFI_D_INFO, " PegGen3PresetSearchVoltageStartMargin : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchVoltageStartMargin));
    DEBUG ((EFI_D_INFO, " PegGen3PresetSearchFavorTiming : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchFavorTiming));
    DEBUG ((EFI_D_INFO, " PegDataPtr : %p\n", SaPlatformPolicyPpi->PcieConfig->PegDataPtr));
    DEBUG ((EFI_D_INFO, " PegGen3ForcePresetSearch : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3ForcePresetSearch));
    DEBUG ((EFI_D_INFO, " InitPcieAspmAfterOprom : %x\n", SaPlatformPolicyPpi->PcieConfig->InitPcieAspmAfterOprom));
    DEBUG ((EFI_D_INFO, " SaIotrapSmiAddress : %x\n", SaPlatformPolicyPpi->PcieConfig->SaIotrapSmiAddress));
  }
  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_3) {
    DEBUG ((EFI_D_INFO, " PegGpioData : %p\n", SaPlatformPolicyPpi->PcieConfig->PegGpioData));
    if (SaPlatformPolicyPpi->PcieConfig->PegGpioData != NULL) {
      DEBUG ((EFI_D_INFO, " PegGpioData->GpioSupport : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGpioData->GpioSupport));
      if (SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset != NULL) {
        DEBUG ((EFI_D_INFO, " PegGpioData->SaPegReset->Value : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset->Value));
        DEBUG ((EFI_D_INFO, " PegGpioData->SaPegReset->Active : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGpioData->SaPegReset->Active));
      }
    }
    DEBUG ((EFI_D_INFO, " PegGen3PresetSearchErrorTarget : %x\n", SaPlatformPolicyPpi->PcieConfig->PegGen3PresetSearchErrorTarget));
    DEBUG ((EFI_D_INFO, " RxCEMLoopback : %x\n", SaPlatformPolicyPpi->PcieConfig->RxCEMLoopback));
    DEBUG ((EFI_D_INFO, " RxCEMLoopbackLane : %x\n", SaPlatformPolicyPpi->PcieConfig->RxCEMLoopbackLane));

    DEBUG ((EFI_D_INFO, " Gen3RxCtleP[%d] :", SA_PEG_MAX_BUNDLE));
    for (i = 0; i < SA_PEG_MAX_BUNDLE; i++) {
      DEBUG ((EFI_D_INFO, " %x", SaPlatformPolicyPpi->PcieConfig->Gen3RxCtleP[i]));
     }
    DEBUG ((EFI_D_INFO, "\n"));
  }

  DEBUG ((EFI_D_INFO, "------------------------ OVERCLOCKING_CONFIGURATION -----------------\n"));
  DEBUG ((EFI_D_INFO, " GtVoltageOffset : %x\n", SaPlatformPolicyPpi->OcConfig->GtVoltageOffset));
  DEBUG ((EFI_D_INFO, " GtVoltageOverride : %x\n", SaPlatformPolicyPpi->OcConfig->GtVoltageOverride));
  DEBUG ((EFI_D_INFO, " GtExtraTurboVoltage : %x\n", SaPlatformPolicyPpi->OcConfig->GtExtraTurboVoltage));
  DEBUG ((EFI_D_INFO, " GtMaxOcTurboRatio : %x\n", SaPlatformPolicyPpi->OcConfig->GtMaxOcTurboRatio));
  DEBUG ((EFI_D_INFO, " SaVoltageOffset : %x\n", SaPlatformPolicyPpi->OcConfig->SaVoltageOffset));
  DEBUG ((EFI_D_INFO, " GtVoltageMode : %x\n", SaPlatformPolicyPpi->OcConfig->GtVoltageMode));
  DEBUG ((EFI_D_INFO, " OcSupport : %x\n", SaPlatformPolicyPpi->OcConfig->OcSupport));
  DEBUG ((EFI_D_INFO, " IoaVoltageOffset : %x\n", SaPlatformPolicyPpi->OcConfig->IoaVoltageOffset));
  DEBUG ((EFI_D_INFO, " IodVoltageOffset : %x\n", SaPlatformPolicyPpi->OcConfig->IodVoltageOffset));

  DEBUG ((EFI_D_INFO, "------------------------ Misc -----------------\n"));
  DEBUG ((EFI_D_INFO, " S3DataPtr : %x\n", SaPlatformPolicyPpi->S3DataPtr));

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_3) {
    DEBUG ((EFI_D_INFO, "------------------------ SG_GPIO_DATA -----------------\n"));
    DEBUG ((EFI_D_INFO, " SgGpioData : %x\n", SaPlatformPolicyPpi->SgGpioData));
    if (SaPlatformPolicyPpi->SgGpioData != NULL) {
      DEBUG ((EFI_D_INFO, " SgGpioData->GpioSupport : %x\n", SaPlatformPolicyPpi->SgGpioData->GpioSupport));
    }
  }

  DEBUG ((EFI_D_INFO, "\n------------------------ SA Platform Policy (PEI) dump END -----------------\n"));
#endif
  return;
}
