/** @file
  This file contains Processor Power Management ACPI related functions for
  Haswell processors.

  Acronyms:
    PPM - Processor Power Management
    TM  - Thermal Monitor
    IST - Intel(R) Speedstep technology
    HT  - Hyper-Threading Technology

@copyright
  Copyright (c) 2012-2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include "PowerMgmtCommon.h"

///
/// Table to convert PL1 / Pl2 Seconds into equivalent MSR values
/// This table is used for TDP Time Window programming
///
UINT8 mSecondsToMsrValueMapTable[][2] = {
  ///
  ///  Seconds,    MSR Value
  ///
  {        1,         0x0A     },
  {        2,         0x0B     },
  {        3,         0x4B     },
  {        4,         0x0C     },
  {        5,         0x2C     },
  {        6,         0x4C     },
  {        7,         0x6C     },
  {        8,         0x0D     },
  {       10,         0x2D     },
  {       12,         0x4D     },
  {       14,         0x6D     },
  {       16,         0x0E     },
  {       20,         0x2E     },
  {       24,         0x4E     },
  {       28,         0x6E     },
  {       32,         0x0F     },
  {       40,         0x2F     },
  {       48,         0x4F     },
  {       56,         0x6F     },
  {       64,         0x10     },
  {       80,         0x30     },
  {       96,         0x50     },
  {      112,         0x70     },
  {      128,         0x11     },
  { END_OF_TABLE, END_OF_TABLE }
};

///
/// Table to convert PL3 Milli Seconds into equivalent MSR values
/// This table is used for TDP Time Window programming
///
UINT8 mMilliSecondsToMsrValueMapTable[][2] = {
  ///
  ///  MilliSeconds,  MSR Value
  ///
  {        3,         0x41     },
  {        4,         0x02     },
  {        5,         0x22     },
  {        6,         0x42     },
  {        7,         0x62     },
  {        8,         0x03     },
  {       10,         0x23     },
  {       12,         0x43     },
  {       14,         0x63     },
  {       16,         0x04     },
  {       20,         0x24     },
  {       24,         0x44     },
  {       28,         0x64     },
  {       32,         0x05     },
  {       40,         0x25     },
  {       48,         0x45     },
  {       56,         0x65     },
  {       64,         0x06     },
  { END_OF_TABLE, END_OF_TABLE }
};

/**
  This will perform Miscellaneous Power Management related programming.

  @param[in] CtdpSupport  Status of InitializeConfigurableTdp funtion
**/
VOID
InitMiscFeatures (
  EFI_STATUS CtdpSupport
  )
{
  InitPchPowerSharing(mCpuPmConfig);
  ///
  /// Configure Package Turbo Power Limits
  ///
  if (CtdpSupport == EFI_SUCCESS) {
    ConfigureCtdp (mCpuPmConfig);
  } else {
    ConfigurePowerLimitsNonConfigTdpSkus (mCpuPmConfig);
  }

  ///
  /// This will perform PowerLimit 1 algorithm will be used to control Thermal Throttling features
  ///
  InitPl1ThermalControl (mCpuPmConfig);

  ///
  /// Configure PL3
  ///
  ConfigurePL3PowerLimits(mCpuPmConfig);

  ///
  /// Configure DDR RAPL PowerLimits
  ///
  ConfigureDdrPowerLimits(mCpuPmConfig);
}

/**
  Private helper function to convert various Turbo Power Limit Time from Seconds to CPU units

  @param[in] TimeInSeconds    Time in seconds
  @param[in] PowerLimitLevel  Power Limit Level  

  @retval UINT8 Converted time in CPU units
**/
UINT8
GetConvertedTime (
  IN UINT32 TimeInSeconds,
  IN UINT8  PowerLimitLevel
  )
{
  UINT8 ConvertedPowerLimitTime;
  UINT8 Index;

  ///
  /// Convert seconds to MSR value. Since not all values are programmable, we'll select
  /// the entry from mapping table which is either equal to the user selected value. OR to a value in the mapping table
  /// which is closest (but less than) to the user-selected value.
  ///
  ConvertedPowerLimitTime = 0;  
  switch(PowerLimitLevel) {
    case PL12TimeWindowCovert:
      ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[0][1];
      for (Index = 0; mSecondsToMsrValueMapTable[Index][0] != END_OF_TABLE; Index++) {
        if (TimeInSeconds == mSecondsToMsrValueMapTable[Index][0]) {
          ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[Index][1];
          break;
        }
        if (TimeInSeconds > mSecondsToMsrValueMapTable[Index][0]) {
          ConvertedPowerLimitTime = mSecondsToMsrValueMapTable[Index][1];
        } else {
          break;
        }
      }
      break;
    case PL3TimeWindowConvert:
      ConvertedPowerLimitTime = mMilliSecondsToMsrValueMapTable[0][1];
      for (Index = 0; mMilliSecondsToMsrValueMapTable[Index][0] != END_OF_TABLE; Index++) {
        if (TimeInSeconds == mMilliSecondsToMsrValueMapTable[Index][0]) {
          ConvertedPowerLimitTime = mMilliSecondsToMsrValueMapTable[Index][1];
          break;
        }
        if (TimeInSeconds > mMilliSecondsToMsrValueMapTable[Index][0]) {
          ConvertedPowerLimitTime = mMilliSecondsToMsrValueMapTable[Index][1];
        } else {
          break;
        }
      }
      break;
    default:
      break;
    }
  
  return ConvertedPowerLimitTime;
}

/**
   Configure PMSYNC_TPR_CFG and PMSYNC_TPR_CFG2 using values returned by CPU BIOS Mail box

  @param[in] CpuPmConfig     Pointer to policy protocol instance
**/
VOID
InitPchPowerSharing (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  UINT32     PcodeMailBoxPchPowerLevels;
  UINT32     MailBoxStatus;
  UINT32     Rcba;
  UINT32     Index;
  UINT8      PchPowerLevel;
  UINT32     Data32And;
  UINT32     Data32Or;
  UINT16     LpcDeviceId;
  UINT8      PchRevId;
  UINT8      IsLptLp;
  CPU_STEPPING CpuSteppingId;
  CPU_FAMILY   CpuFamilyId;

  CpuFamilyId   = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_FAMILY_MODEL;
  CpuSteppingId = mPpmGlobalNvsAreaProtocol->Area->Cpuid & CPUID_FULL_STEPPING;

  ///
  /// PCH Power sharing supported only on HSW ULT.
  ///
  if (CpuFamilyId != EnumCpuHswUlt) {
    return;
  }

  ///
  /// Read PCH Power Limit from PCODE Mail Box.
  ///
  MailboxRead (MAILBOX_TYPE_PCODE,READ_PCH_POWER_LEVELS_CMD,&PcodeMailBoxPchPowerLevels,&MailBoxStatus);

  DEBUG ((EFI_D_ERROR, "Read PCH Power Limit from PCODE Mail Box : %x \n",PcodeMailBoxPchPowerLevels));
    Rcba = MmioRead32 (
                    MmPciAddress (
                            0,
                            DEFAULT_PCI_BUS_NUMBER_PCH,
                            PCI_DEVICE_NUMBER_PCH_LPC,
                            PCI_FUNCTION_NUMBER_PCH_LPC,
                            R_PCH_LPC_RCBA
                            )
                    );
    Rcba &= (UINT32) (~BIT0);
  if (MailBoxStatus == PCODE_MAILBOX_CC_SUCCESS) {
    ///
    /// Program RCBA+PMSYNC_TPR_CONFIG PCH power limit values.
    /// READ_PCH_POWER_LEVELS_CMD MailBox[0:5],MailBox[6:11],MailBox[12:17] to PCHReg [0:4],[8:12],[16:20]
    ///
    Data32And =0x0;
    Data32Or =0x0;

    for (Index = 0; Index < HSW_ULT_PCH_POWER_LEVELS; Index++) {
      PchPowerLevel = PcodeMailBoxPchPowerLevels & 0x3F;
      PcodeMailBoxPchPowerLevels = PcodeMailBoxPchPowerLevels >> 6;
      Data32And |= 0x1F << (Index * 8);
      Data32Or |= (PchPowerLevel & 0x1F) << (Index * 8);
    }
    Data32And = ~Data32And;
    MmioAndThenOr32(Rcba+PMSYNC_TPR_CONFIG,Data32And,Data32Or);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINT64) (Rcba + PMSYNC_TPR_CONFIG),
      1,
      (VOID *) (UINTN) (Rcba + PMSYNC_TPR_CONFIG)
      ); 
  } else {
    DEBUG ((EFI_D_ERROR, "Failure - Read PCH Power Limit from PCODE Mail Box\n"));
  }
  ///
  /// Extended PCH power sharing supported on HSW ULT C0 & LPT-LP B0 and later
  ///
  ///
  PchRevId = MmioRead8 (
              MmPciAddress (0,
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_LPC,
                PCI_FUNCTION_NUMBER_PCH_LPC,
                R_PCH_LPC_RID)
              );

  LpcDeviceId = MmioRead16 (
                 MmPciAddress (0,
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_DEVICE_ID)
                );
  IsLptLp = IS_PCH_LPTLP_LPC_DEVICE_ID(LpcDeviceId);

  if(IsLptLp && (PchRevId < V_PCH_LPT_LPC_RID_2) && (CpuSteppingId < EnumHswUltC0)) {
    return;
  }
  ///
  /// Program RCBA+PMSYNC_TPR_CONFIG Extnded PCH power limit values.
  /// READ_PCH_POWER_LEVELS_CMD-MailBox[23:18],READ_EXT_PCH_POWER_LEVELS_CMD- MailBox[6:11],MailBox[12:17],MailBox[18:22] to PCHReg [0:4],[8:12],[16:20],[24:28]
  ///
  Data32And = 0x1F;
  Data32Or = (PcodeMailBoxPchPowerLevels & 0x1F);
  ///
  /// Read Extended PCH Power Limit from PCODE Mail Box.
  ///
  MailboxRead (MAILBOX_TYPE_PCODE,READ_EXT_PCH_POWER_LEVELS_CMD,&PcodeMailBoxPchPowerLevels,&MailBoxStatus);
  DEBUG ((EFI_D_ERROR, "Read Extended PCH Power Limit from PCODE Mail Box : %x \n",PcodeMailBoxPchPowerLevels));
  if (MailBoxStatus == PCODE_MAILBOX_CC_SUCCESS) {
    for (Index = 1; Index < EXTENDED_PCH_POWER_LEVELS; Index++) {
      PchPowerLevel = PcodeMailBoxPchPowerLevels & 0x3F;
      PcodeMailBoxPchPowerLevels = PcodeMailBoxPchPowerLevels >> 6;
      Data32And |= 0x1F << (Index * 8);
      Data32Or |= (PchPowerLevel & 0x1F) << (Index * 8);
    }
    Data32And = ~Data32And;
    MmioAndThenOr32(Rcba+PMSYNC_TPR_CONFIG2,Data32And,Data32Or);
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINT64) (Rcba + PMSYNC_TPR_CONFIG2),
      1,
      (VOID *) (UINTN) (Rcba + PMSYNC_TPR_CONFIG2)
      ); 		
  } else {
     DEBUG ((EFI_D_ERROR, "Failure -Extended Read PCH Power Limit from PCODE Mail Box\n"));
  }
}

/**
  Locks down all settings.

  @param[in] CpuPmConfig  Pointer to PPM Policy structure.
**/
VOID
PpmLockDown (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER TempMsr;
  ///
  /// Program PMG_CST_CONFIG MSR [15] (CFG lock bit)
  ///
  RunOnAllLogicalProcessors (ApSafeLockDown, CpuPmConfig);
  ///
  /// Lock Package power limit MSR
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  TempMsr.Dwords.High &= ~(B_POWER_LIMIT_LOCK);
  if (CpuPmConfig->pTurboSettings->TurboPowerLimitLock) {
    TempMsr.Dwords.High |= B_POWER_LIMIT_LOCK;
  }
  AsmWriteMsr64 (MSR_PACKAGE_POWER_LIMIT, TempMsr.Qword);
  ///
  /// Program the OverClocking Lock Bit.
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_FLEX_RATIO);
  TempMsr.Dwords.Low &= ~(B_OVERCLOCKING_LOCK);
  if (CpuPmConfig->pPpmLockEnables->OverclockingLock) {
    TempMsr.Dwords.Low |= B_OVERCLOCKING_LOCK;
  }
  AsmWriteMsr64 (MSR_FLEX_RATIO, TempMsr.Qword);
  ///
  /// Program the PROCHOT_Lock
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_POWER_CTL);
  TempMsr.Dwords.Low &= ~(B_MSR_POWER_CTL_PROC_HOT_LOCK);
  if (CpuPmConfig->pPpmLockEnables->ProcHotLock) {
    TempMsr.Dwords.Low |= B_MSR_POWER_CTL_PROC_HOT_LOCK;
  }
  AsmWriteMsr64 (MSR_POWER_CTL, TempMsr.Qword);
  ///
  /// Program Ddr RAPL LIMIT Lock
  ///
  TempMsr.Qword = AsmReadMsr64 (MSR_DDR_RAPL_LIMIT);
  TempMsr.Dwords.High &= ~(B_POWER_LIMIT_LOCK);
//[-start-130424-IB10930028-modify]//
   if (CpuPmConfig->pTurboSettings->DdrPowerLimitLock) {
//[-end-130424-IB10930028-modify]//
    TempMsr.Dwords.High |= B_POWER_LIMIT_LOCK;
  }
  AsmWriteMsr64 (MSR_DDR_RAPL_LIMIT, TempMsr.Qword);

  return;
}

/**
  Lock MSR_PMG_CST_CONFIG.
  This function must be MP safe.

  @param[in] Buffer    Not used (needed for API compatibility)

  @retval EFI_SUCCESS  Processor C-State locked successfully.
**/
VOID
EFIAPI
ApSafeLockDown (
  IN OUT VOID *Buffer
  )
{
  MSR_REGISTER      PmCfgCtrl;
  POWER_MGMT_CONFIG *CpuPmConfig;
  UINT8             CfgLock;

  CpuPmConfig = (POWER_MGMT_CONFIG *) Buffer;
  if (CpuPmConfig == NULL) {
    CfgLock = PPM_ENABLE;
  } else {
    CfgLock = (UINT8) CpuPmConfig->pPpmLockEnables->PmgCstCfgCtrlLock;
  }
  PmCfgCtrl.Qword = AsmReadMsr64 (MSR_PMG_CST_CONFIG);
  PmCfgCtrl.Dwords.Low &= ~B_CST_CONTROL_LOCK;
  if (CfgLock == PPM_ENABLE) {
    PmCfgCtrl.Dwords.Low |= B_CST_CONTROL_LOCK;
  }
  AsmWriteMsr64 (MSR_PMG_CST_CONFIG, PmCfgCtrl.Qword);

  return;
}

/**
  Runs the specified procedure on all logical processors, passing in the
  parameter buffer to the procedure.

  @param[in] Procedure  The function to be run.
  @param[in] Buffer     Pointer to a parameter buffer.

  @retval EFI_SUCCESS
**/
EFI_STATUS
RunOnAllLogicalProcessors (
  IN OUT EFI_AP_PROCEDURE Procedure,
  IN OUT VOID             *Buffer
  )
{
  EFI_STATUS Status;

  ///
  /// Run the procedure on all logical processors.
  ///
  (*Procedure)(Buffer);
  Status = mMpService->StartupAllAPs (
                  mMpService,
                  (EFI_AP_PROCEDURE) Procedure,
                  TRUE,
                  NULL,
                  MP_TIMEOUT_FOR_STARTUP_ALL_APS,
                  Buffer,
                  NULL
                  );

  return Status;
}

/**
  Configures the RFI Tunning MSR (0xE3) for FIVR switching freq.

  @param[in] CpuPmConfig           Pointer to PPM Policy structure.
**/
VOID
InitFivrSwitchingFreq (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  )
{
  MSR_REGISTER RfiTuningValue;
  UINT16       FreqTuningOffsetValue;
  UINT32       Remainder;

  ///
  /// Check if we have to change the RFI Freq Tunning offset.
  /// Check PLATFORM_INFO MSR[25] == 1 before accessing the MSR_RFI_TUNNING
  ///
  if ((CpuPmConfig->RfiFreqTunningOffset != AUTO) &&
      ((AsmReadMsr64 (MSR_PLATFORM_INFO)) & B_FIVR_RFI_TUNING_AVAIL)
      ) {
    ///
    /// Convert the Policy Freq Tunning offset.
    /// Target frequency encoding = int(value*2^16+0.5) for positive offsets and inv(int(value*2^16+0.5))+1 for negative offsets
    ///
    FreqTuningOffsetValue = (UINT16) DivU64x32Remainder (
                    (UINT64) (CpuPmConfig->RfiFreqTunningOffset * (1 << 16)),
                    1000,
                    &Remainder
                    );
    if (Remainder >= 500) {
      FreqTuningOffsetValue += 1;
    }
    ///
    /// Check if Freq Tunning offset value is -ve
    ///
    if (CpuPmConfig->RfiFreqTunningOffsetIsNegative == 1) {
      FreqTuningOffsetValue = (UINT16) (~FreqTuningOffsetValue + 1);
    }
    ///
    /// Write to the RFI_TUNING_MSR. System BIOS must set the desired frequency offset in bits 15:0 of this MSR.
    ///
    RfiTuningValue.Qword = AsmReadMsr64 (MSR_RFI_TUNNING);

    ///
    /// Set the Tuning Frequency
    ///
    RfiTuningValue.Qword = ((RfiTuningValue.Qword & V_FREQ_TUNNING_MASK) | FreqTuningOffsetValue);
    AsmWriteMsr64 (MSR_RFI_TUNNING, RfiTuningValue.Qword);
  }
}

/**
  Update the SSDT table pointers and config DWORD CFGD with the PpmFlags current configuration value
**/
VOID
PatchCpuPmTable (
  VOID
  )
{
  UINT8       *CurrPtr;
  UINT32      *Signature;
  SSDT_LAYOUT *SsdtPackage;

  ///
  /// Locate the SSDT package
  ///
  SsdtPackage = NULL;
  CurrPtr     = (UINT8 *) mCpuPmTable;
  for (CurrPtr; CurrPtr <= ((UINT8 *) mCpuPmTable + mCpuPmTable->Length); CurrPtr++) {
    Signature = (UINT32 *) (CurrPtr + 1);
    if ((*CurrPtr == AML_NAME_OP) && *Signature == SIGNATURE_32 ('S', 'S', 'D', 'T')) {
      ///
      /// Update the SSDT table pointers for dynamically loaded tables
      ///
      SsdtPackage = (SSDT_LAYOUT *) CurrPtr;
      ///
      /// Set the P-State SSDT table information
      ///
      SsdtPackage->Cpu0IstAddr  = (UINT32) (UINTN) mCpu0IstTable;
      SsdtPackage->Cpu0IstLen   = mCpu0IstTable->Length;
      SsdtPackage->ApIstAddr    = (UINT32) (UINTN) mApIstTable;
      SsdtPackage->ApIstLen     = mApIstTable->Length;
      ///
      /// Set the C-State SSDT table information
      ///
      SsdtPackage->Cpu0CstAddr  = (UINT32) (UINTN) mCpu0CstTable;
      SsdtPackage->Cpu0CstLen   = mCpu0CstTable->Length;
      SsdtPackage->ApCstAddr    = (UINT32) (UINTN) mApCstTable;
      SsdtPackage->ApCstLen     = mApCstTable->Length;
    }
    ///
    /// Update the PPM GlobalNvs area
    ///
    if ((*CurrPtr == AML_OPREGION_OP) && *Signature == SIGNATURE_32 ('P', 'P', 'M', 'T')) {
      ASSERT_EFI_ERROR (*(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) == 0xFFFF0000);
      ASSERT_EFI_ERROR (*(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA55);
      ///
      /// PPM Global NVS Area address
      ///
      *(UINT32 *) (CurrPtr + 1 + sizeof (*Signature) + 2) = (UINT32) (UINTN) mPpmGlobalNvsAreaProtocol->Area;
      ///
      /// PPM Global NVS Area size
      ///
      *(UINT16 *) (CurrPtr + 1 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) = sizeof (PPM_GLOBAL_NVS_AREA);
      break;
    }
  }
  //
  // Assert if we didn't update the PM table
  //
  ASSERT (SsdtPackage != NULL);

  return;
}

/**
  Locate the PPM ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

  @retval EFI_SUCCESS - On success
  @retval EFI_NOT_FOUND - Required firmware volume not found
  @retval - Appropiate failure code on error
**/
EFI_STATUS
InitializePpmAcpiTable (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   *HandleBuffer;
  UINTN                        NumberOfHandles;
  EFI_FV_FILETYPE              FileType;
  UINT32                       FvStatus;
  EFI_FV_FILE_ATTRIBUTES       Attributes;
  UINTN                        Size;
  UINTN                        i;
  EFI_FIRMWARE_VOLUME_PROTOCOL *FwVol;
  INTN                         Instance;
  EFI_ACPI_TABLE_VERSION       Version;
  EFI_ACPI_COMMON_HEADER       *CurrentTable;
  EFI_ACPI_DESCRIPTION_HEADER  *TempTable;
  UINTN                        AcpiTableHandle;

  ///
  /// Locate Firmware volume protocol.
  /// There is little chance we can't find an FV protocol
  ///
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Look for FV with ACPI storage file
  ///
  FwVol = NULL;
  for (i = 0; i < NumberOfHandles; i++) {
    ///
    /// Get the protocol on this handle
    /// This should not fail because of LocateHandleBuffer
    ///
    Status = gBS->HandleProtocol (
                    HandleBuffer[i],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **) &FwVol
                    );
    ASSERT_EFI_ERROR (Status);
    ///
    /// See if it has the ACPI storage file
    ///
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                    FwVol,
                    &gPowerMgmtAcpiTableStorageGuid,
                    NULL,
                    &Size,
                    &FileType,
                    &Attributes,
                    &FvStatus
                    );
    ///
    /// If we found it, then we are done
    ///
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  ///
  /// Our exit status is determined by the success of the previous operations
  /// If the protocol was found, Instance already points to it.
  /// Free any allocated buffers
  ///
  FreePool (HandleBuffer);
  ///
  /// Sanity check that we found our data file
  ///
  ASSERT (FwVol != NULL);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }
  ///
  /// By default, a table belongs in all ACPI table versions published.
  ///
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
  ///
  /// Read tables from the storage file.
  ///
  Instance      = 0;
  CurrentTable  = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gPowerMgmtAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **) &CurrentTable,
                      &Size,
                      &FvStatus
                      );
    if (!EFI_ERROR (Status)) {
      ///
      /// Check the table ID to modify the table
      ///
      switch (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId) {
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'I', 's', 't', 0)):
          mCpu0IstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EIST) {
            ///
            /// Patch the native _PSS package with the GV3 values
            ///
            Status = AcpiPatchPss ();
            if (EFI_ERROR (Status)) {
              return Status;
            }
          }
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'C', 's', 't', 0)):
          mCpu0CstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', '0', 'T', 's', 't', 0)):
          mCpu0TstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'I', 's', 't', 0, 0, 0)):
          mApIstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'C', 's', 't', 0, 0, 0)):
          mApCstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('A', 'p', 'T', 's', 't', 0, 0, 0)):
          mApTstTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 'p', 'u', 'P', 'm', 0, 0, 0)):
          mCpuPmTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('L', 'a', 'k', 'e', 'T','i', 'n', 'y')):
          mLakeTinyTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        case (SIGNATURE_64 ('C', 't', 'd', 'p', 'B', 0, 0, 0)):
          mCtdpTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
          break;
        default:
          break;
      }
      Instance++; // Increment the instance
      CurrentTable = NULL;
    }
  }
  ///
  /// Statically load IST SSDT if EIST is enabled
  ///
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_EIST) {
    AcpiTableHandle = 0;
    Status = mAcpiTable->InstallAcpiTable (
                    mAcpiTable,
                    mCpu0IstTable,
                    mCpu0IstTable->Length,
                    &AcpiTableHandle
                    );
    //
    // Free this table as it has been copied into ACPI tables
    //
    FreePool (mCpu0IstTable);
  }
  ///
  /// If we are CMP, then the PPM tables are dynamically loaded:
  ///   We need to publish the CpuPm table to the ACPI tables, and move the CST
  ///   tables that are dynamically loaded to a separate location so that we can fix the
  ///   addresses in the CpuPm table.
  /// Otherwise (non-CMP):
  ///   We need to publish CPU 0 tables only, and CST tables only if CST is enabled
  ///
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_CMP) {
    //
    // Copy tables to our own location and checksum them
    //
    Status = (gBS->AllocatePool) (EfiReservedMemoryType, mApIstTable->Length, (VOID **) &TempTable);
    ASSERT_EFI_ERROR (Status);
    CopyMem (TempTable, mApIstTable, mApIstTable->Length);
    FreePool (mApIstTable);
    mApIstTable = TempTable;
    AcpiChecksum (mApIstTable, mApIstTable->Length, OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    Status = (gBS->AllocatePool) (EfiReservedMemoryType, mCpu0CstTable->Length, (VOID **) &TempTable);
    ASSERT_EFI_ERROR (Status);
    CopyMem (TempTable, mCpu0CstTable, mCpu0CstTable->Length);
    FreePool (mCpu0CstTable);
    mCpu0CstTable = TempTable;
    AcpiChecksum (mCpu0CstTable, mCpu0CstTable->Length, OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
    Status = (gBS->AllocatePool) (EfiReservedMemoryType, mApCstTable->Length, (VOID **) &TempTable);
    ASSERT_EFI_ERROR (Status);
    CopyMem (TempTable, mApCstTable, mApCstTable->Length);
    FreePool (mApCstTable);
    mApCstTable = TempTable;
    AcpiChecksum (mApCstTable, mApCstTable->Length, OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum));
  } else {
    //
    // CMP disabled, so statically load the tables
    //
    // Add CST SSDT if C states are enabled
    //
    if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_C_STATES) {
      AcpiTableHandle = 0;
      Status = mAcpiTable->InstallAcpiTable (
                      mAcpiTable,
                      mCpu0CstTable,
                      mCpu0CstTable->Length,
                      &AcpiTableHandle
                      );
    }
    ///
    /// Since we are UP, there is no need for the CPU 1 tables
    ///
    ///
    /// Free all tables, since they have been copied into ACPI tables by ACPI support protocol
    ///
    FreePool (mCpu0CstTable);
    FreePool (mApIstTable);
    FreePool (mApCstTable);
  }
  ///
  /// Update the CpuPm SSDT table in the ACPI tables.
  ///
  PatchCpuPmTable ();
  AcpiTableHandle = 0;
  Status = mAcpiTable->InstallAcpiTable (
                  mAcpiTable,
                  mCpuPmTable,
                  mCpuPmTable->Length,
                  &AcpiTableHandle
                  );
  FreePool (mCpuPmTable);
  if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_TSTATES) {
    ///
    /// Load the Cpu0Tst SSDT table in the ACPI tables
    ///
    AcpiTableHandle = 0;
    Status = mAcpiTable->InstallAcpiTable (
                    mAcpiTable,
                    mCpu0TstTable,
                    mCpu0TstTable->Length,
                    &AcpiTableHandle
                    );
    FreePool (mCpu0TstTable);
    ///
    /// If the CMP is enabled then load the ApTst SSDT table in the ACPI tables
    ///
    if (mPpmGlobalNvsAreaProtocol->Area->PpmFlags & PPM_CMP) {
      AcpiTableHandle = 0;
      Status = mAcpiTable->InstallAcpiTable (
                      mAcpiTable,
                      mApTstTable,
                      mApTstTable->Length,
                      &AcpiTableHandle
                      );
    }
  }
  FreePool (mApTstTable);
  ///
  /// Load LakeTiny SSDT only when it is enabled in policy and laketiny SSDT is included.
  ///
  if ((mCpuPmConfig->pFunctionEnables->LakeTiny) && (mLakeTinyTable != NULL)) {
    AcpiTableHandle = 0;
    Status = mAcpiTable->InstallAcpiTable (
                    mAcpiTable,
                    mLakeTinyTable,
                    mLakeTinyTable->Length,
                    &AcpiTableHandle
                    );
    FreePool (mLakeTinyTable);
  }
  ///
  /// Load Ctdp SSDT
  ///
  if (mCpuPmConfig->pTurboSettings->ConfigTdpBios == 1) {
    AcpiTableHandle = 0;
    Status = mAcpiTable->InstallAcpiTable (
                    mAcpiTable,
                    mCtdpTable,
                    mCtdpTable->Length,
                    &AcpiTableHandle
                    );
    FreePool (mCtdpTable);
  }

  return Status;
}