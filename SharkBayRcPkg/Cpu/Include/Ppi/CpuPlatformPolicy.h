/** @file
  CPU policy PPI produced by a platform driver specifying various
  expected CPU settings. This PPI is consumed by CPU PEI modules.

@copyright
  Copyright (c) 2009 - 2013 Intel Corporation. All rights reserved
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
#ifndef CPU_PLATFORM_POLICY_H_
#define CPU_PLATFORM_POLICY_H_

#include "PfatDefinitions.h"

//
// CPU policy provided by platform for PEI phase
//
//
// EDK and EDKII have different GUID formats
//
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define PEI_CPU_PLATFORM_POLICY_PPI_GUID \
  { \
    0xf8d5438e, 0x26e1, 0x481d, 0xb6, 0x3c, 0x30, 0xd6, 0xef, 0xf4, 0xa4, 0x20 \
  }
#else
#define PEI_CPU_PLATFORM_POLICY_PPI_GUID \
  { \
    0xf8d5438e, 0x26e1, 0x481d, \
    { \
      0xb6, 0x3c, 0x30, 0xd6, 0xef, 0xf4, 0xa4, 0x20 \
    } \
  }
#endif

extern EFI_GUID gPeiCpuPlatformPolicyPpiGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PEI_CPU_PLATFORM_POLICY_PPI PEI_CPU_PLATFORM_POLICY_PPI;

#define CPU_FEATURE_ENABLE  1
#define CPU_FEATURE_DISABLE 0

///
/// PPI revision number
/// Any backwards compatible changes to this PPI will result in an update in the revision number
/// Major changes will require publication of a new PPI
///

///
///Revision 1: Original version
///
#define PEI_CPU_PLATFORM_POLICY_PPI_REVISION_1  1
///
///Revision 2: Added policies for PFAT-BIOS-EC Interface to PFAT_CONFIG
///            Added PfatLog to PFAT_CONFIG
#define PEI_CPU_PLATFORM_POLICY_PPI_REVISION_2  2
///
///Revision 3: Added VR_MISC MSR configuration policies
///              POWER_MGMT_CONFIG.VrMiscMinVid;
///              POWER_MGMT_CONFIG.VrMiscIdleExitRampRate;
///              POWER_MGMT_CONFIG.VrMiscIdleEntryRampRate;
///              POWER_MGMT_CONFIG.VrMiscIdleEntryDecayEnable;
///              POWER_MGMT_CONFIG.VrMiscSlowSlewRateConfig;
///            Added VR_MISC2 MSR configuration policies
///              POWER_MGMT_CONFIG.VrMisc2FastRampVoltage;
///              POWER_MGMT_CONFIG.VrMisc2MinC8Voltage;
///            Extended POWER_MGMT_CONFIG.BootInLfm settings from 0(HFM)/ 1(LFM) to 0(HFM)/ 1(LFM)/ 2(TURBO)
///
#define PEI_CPU_PLATFORM_POLICY_PPI_REVISION_3  3
///
///Revision 4: Added VR_MISC MSR configuration policies
///              POWER_MGMT_CONFIG.VrMiscIoutSlope;
///              POWER_MGMT_CONFIG.VrMiscIoutOffsetSign;
///              POWER_MGMT_CONFIG.VrMiscIoutOffset;
///
#define PEI_CPU_PLATFORM_POLICY_PPI_REVISION_4  4
///
///Revision 5: Added POWER_MGMT_CONFIG_PPI policies
///              POWER_MGMT_CONFIG_PPI.VrPSI4enable;
///              POWER_MGMT_CONFIG_PPI.Psi1Threshold;
///              POWER_MGMT_CONFIG_PPI.Psi2Threshold;
///              POWER_MGMT_CONFIG_PPI.Psi3Threshold;
///
#define PEI_CPU_PLATFORM_POLICY_PPI_REVISION_5  5
///
///Revision 6: Added POWER_MGMT_CONFIG_PPI Fivr polcies
///             POWER_MGMT_CONFIG_PPI.FivrSscEnable
///             POWER_MGMT_CONFIG_PPI.FivrSscPercent
///
#define PEI_CPU_PLATFORM_POLICY_PPI_REVISION_6  6
///
///Revision 7: Added CpuPlatformPpiPtr
///             BOOT_GUARD_CONFIG.BootGuardSupport
///             BOOT_GUARD_CONFIG.DisconnectAllTpms
///
#define PEI_CPU_PLATFORM_POLICY_PPI_REVISION_7  7
///
///Revision 8: Added BOOT_GUARD_CONFIG configuration policies
///             BOOT_GUARD_CONFIG.ByPassTpmEventLog
///
#define PEI_CPU_PLATFORM_POLICY_PPI_REVISION_8  8

///
/// General CPU features configration for all processor features enabling / configuration bit definitions are in
/// this field. Platform code can enable/disable/configure features thru this field.
///
typedef struct {
  //
  // Byte 0, bit definition for functionality enable/disable
  //
  UINT8 BistOnReset           : 1; ///< Enable or Disable BIST on Reset.
  UINT8 HyperThreading        : 1; ///< Enable or Disable Hyper Threading.
  UINT8 CpuRatioOverride      : 1; ///< Enable or Disable CPU Radio Override.
  UINT8 VmxEnable             : 1; ///< Enable or Disable VMX.
  UINT8 Pfat                  : 1; ///< Enable or Disable PFAT.
  UINT8 MlcStreamerPrefetcher : 1; ///< Enable or Disable MLC Streamer Prefetcher.
  UINT8 MlcSpatialPrefetcher  : 1; ///< Enable or Disable MLC Spatial Prefetcher.
  UINT8 Txt                   : 1; ///< Enable or Disable TXT.

  //
  // Byte 1, bit definition for functionality enable/disable
  //
  UINT8 Rsvdbyte : 8; ///< A place holder for additonal functionalites expected in futture platforms.
  //
  // Generic CPU feature configuration
  //
  UINT8 ActiveCoreCount;     ///< Number of active cores.
  UINT8 CpuRatio;            ///< CPU ratio value
  UINT8 CpuMaxNonTurboRatio; ///< CPU Maximum Non-Turbo ratio value

//[-start-130906-IB08620308-add]//
  //
  // Byte n, bit definition for functionality enable/disable
  //
  UINT8 DcuStreamerPrefetcher   : 1; ///< Enable or Disable Dcu Streamer Prefetcher.
  UINT8 DcuIPPrefetcher         : 1; ///< Enable or Disable Dcu IP Prefetcher.
//[-end-130906-IB08620308-add]//

} CPU_CONFIG_PPI;

///
/// Power management Configuration for all Processor Power Management Features
/// Configs are in this field. Platform code can enable/disable features thru this field.
///
typedef struct {
  UINT8  BootInLfm; ///< Boot in Low-Frequency-Mode.
  ///
  /// TCC Activation Offset. Offset from factory set TCC activation temperature at which the Thermal Control Circuit
  /// must be activated.  TCC will be activated at TCC Activation Temp V TCC Activation Offset.
  ///
  UINT8  TccActivationOffset;
  ///
  /// MSR 0x601[12:0], Primary (IA) Current Limit, in 1/8 A units, 0=Auto. Maximum instantaneous current allowed at
  /// any given time. Setting a value too far below the Iccmax specification for this platform/CPU might result
  /// in System hang / CATERR / BSOD depending on the platform configuration.
  ///
  UINT16 VrCurrentLimit;
  UINT8  VrCurrentLimitLock; ///< MSR 0x601[31]: when set, locks bits 31:0 of this MSR.
  UINT8  Xe; ///< Enable or Disable Intel Extreme Edition support.
  ///
  /// 1-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 1-Core Ratio Limit + OC Bins.
  /// This 1-Core Ratio Limit Must be greater than or equal to 2-Core Ratio Limit, 3-Core Ratio Limit, 4-Core
  /// Ratio Limit.
  /// 2-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 2-Core Ratio Limit + OC Bins.
  /// This 2-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
  /// 3-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 3-Core Ratio Limit + OC Bins.
  /// This 3-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
  /// 4-Core Ratio Limit: For XE part: LFM to 255, For overclocking part: LFM to Fused 4-Core Ratio Limit + OC Bins.
  /// This 4-Core Ratio Limit Must be Less than or equal to 1-Core Ratio Limit.
  ///
  UINT8  RatioLimit[4];
  UINT8  VrMiscMinVid; ///< MSR 0x603[31:24]: Used to constrain the CPU's minimum voltage during package C-states.
  UINT8  VrMiscIdleExitRampRate; ///< MSR 0x603[50]: Controls the VR ramp rate on package C-state wake events.
  UINT8  VrMiscIdleEntryRampRate; ///< MSR 0x603[51]: Controls the VR ramp rate on package C-state entry events.
  UINT8  VrMiscIdleEntryDecayEnable; ///< MSR 0x603[52]: Enables decay mode on package C-state entry.
  UINT8  VrMiscSlowSlewRateConfig; ///< MSR 0x53[54:53]: Slow Slew rate configuration.
  UINT8  VrMisc2FastRampVoltage; ///< MSR 0x636[7:0]: Update FAST_RAMP_VOLTAGE; must be 0 to 0xFF, resolution 10mV.
  UINT8  VrMisc2MinC8Voltage; ///< MSR 0x636[15:8]: Update MIN_C8_VOLTAGE; must be 0 to 0xFF, resolution 10mV.
  UINT16 VrMiscIoutSlope; ///< MSR 0x603[49-40]: IOUT SLOPE. Default is 200.
  UINT8  VrMiscIoutOffsetSign; ///< IOUT OFFSET Sign. 0 = positive offset. 1= negative offset.
  UINT16 VrMiscIoutOffset; ///< MSR 0x603[39-32]: IOUT_OFFSET. 0 = 0%, 625 = 6.25% (Range is +6.25% ~ -6.25%)
  UINT8  VrPSI4enable; ///< MSR 0x601[62]: PSI4 Enabled/Disabled bit in VR_CURRENT_CONFIG, it's only applied to HSW ULT.
  UINT8  Psi1Threshold; ///< MSR 0x601[41:32]: Current Threshold for PSI1_CODE specified in Amps.
  UINT8  Psi2Threshold; ///< MSR 0x601[51:42]: Current Threshold for PSI2_CODE specified in Amps.
  UINT8  Psi3Threshold; ///< MSR 0x601[61:52]: Current Threshold for PSI3_CODE specified in Amps.
  UINT8  FivrSscEnable; ///< Enable/Disable FIVR Spread Spectrum support
  UINT8  FivrSscPercent; ///< PCODE MMIO Mailbox: FIVR spread spectrum percentage value.
} POWER_MGMT_CONFIG_PPI;

///
/// PFAT Configuration for all processor security features configuration structures
/// are in this field. Platform code can pass relevant configuration data thru this field.
///
typedef struct {
  ///
  /// PFAT update package header that will be packaged along with PFAT script and update data.
  ///
  PUP_HEADER PupHeader;
  ///
  /// PFAT Platform Data Table contains all the platform data that will be parsed by PFAT module.
  ///
  PPDT       Ppdt;
  UINT64     PpdtHash[4];       ///< Hash of the PPDT that will be programmed to PLAT_FRMW_PROT_HASH_0/1/2/3 MSR.
  UINT8      NumSpiComponents;  ///< Number of SPI flash components on platform.
  UINT8      ComponentSize[MAX_SPI_COMPONENTS]; ///< Size of each flash component on platform.
  UINT8      PfatMemSize;       ///< PFAT memory size.
  UINT8      EcCmdDiscovery;    ///< EC Command discovery.
  UINT8      EcCmdProvisionEav; ///< EC Command Provision Eav.
  UINT8      EcCmdLock;         ///< EC Command Lock.
  PFAT_LOG   PfatLog;           ///< PFAT log.
} PFAT_CONFIG;

//
// TxT Configuraion Policy
//
#define TXT_SINIT_MEMORY_SIZE         0x20000
#define TXT_HEAP_MEMORY_SIZE          0xE0000
#define TXT_DPR_MEMORY_SIZE           0x400000
#define TXT_DEVICE_MEMORY_SIZE        (TXT_SINIT_MEMORY_SIZE + TXT_HEAP_MEMORY_SIZE)

#define TXT_LCP_PD_BASE               0x0     ///< Platform default LCP
#define TXT_LCP_PD_SIZE               0x0     ///< Platform default LCP
#define TXT_TGA_MEMORY_SIZE           0x0     ///< Maximum 512K of TGA memory (aperture)
#define TXT_TGTT_MEMORY_SIZE          0x80000 ///< 512K of TGTT memory
#define TXT_TGR_MEMORY_SIZE           0x80000 ///< 512K of TGR memory (Registers)
#define TXT_TOTAL_STOLEN_MEMORY_SIZE  (TXT_DPR_MEMORY_SIZE)

typedef struct {
  UINT64               SinitMemorySize;   ///< Size of SINIT module if installed in flash part. Zero otherwise.
  UINT64               TxtHeapMemorySize; ///< Size of memory reserved for TXT Heap. This memory is used by MLE.
  EFI_PHYSICAL_ADDRESS TxtDprMemoryBase;  ///< Base address of DPR protected memory reserved for Intel TXT component.
  UINT64               TxtDprMemorySize;  ///< Size of DPR protected memory reserved for Intel TXT component.
  ///
  /// Base address of BIOS ACM in flash part. It can be passed through platform code for customization; Intel TXT
  /// reference code would skip searching the BIOS ACM in PEI firmware volume if the field is not zero.
  ///
  EFI_PHYSICAL_ADDRESS BiosAcmBase;
  UINT64               BiosAcmSize; ///< Size of ACM Binary.
  ///
  /// Base address of CPU micro code patch loaded into BSP. It can be passed through platform code for customization;
  /// Intel TXT reference code would skip searching the micro code path in PEI firmware volume if the field is not zero.
  ///
  EFI_PHYSICAL_ADDRESS McuUpdateDataAddr;
  ///
  /// Size of Trusted Graphics Aperture if supported by chipset. For Cantiga must be 0.
  ///
  UINT64               TgaSize;
  ///
  /// Base address of Platform Default Launch Control Policy data if installed in flash part. Zero otherwise.
  ///
  EFI_PHYSICAL_ADDRESS TxtLcpPdBase;
  ///
  /// Size of Platform Default Launch Control Policy data if installed in flash part. Zero otherwise.
  ///
  UINT64               TxtLcpPdSize;
} TXT_CONFIG;

//
// Boot Guard Configuration
//
typedef enum {
  TpmNone = 0,
  dTpm12,
  dTpm20,
  Ptt,
  TpmTypeMax
} TPM_TYPE;

typedef struct {
  BOOLEAN               MeasuredBoot;
  BOOLEAN               BypassTpmInit;
  TPM_TYPE              TpmType;
  BOOLEAN               BootGuardSupport;
  BOOLEAN               DisconnectAllTpms;
  BOOLEAN               ByPassTpmEventLog;
} BOOT_GUARD_CONFIG;

///
/// Security Configuration
///
typedef struct {
  PFAT_CONFIG        *PfatConfig;       ///< PFAT Configuration settings.
  TXT_CONFIG         *TxtConfig;        ///< TxT Configuration settings.
  BOOT_GUARD_CONFIG  *BootGuardConfig;  ///< Boot Guard Configuration settings.
} SECURITY_CONFIG_PPI;

///
/// Overclocking Configuration controls which use the CPU overclocking mailbox interface are defined in this structure.
/// Platform code can pass in data to the mailbox through this structure.
///
typedef struct {
  INT16  CoreVoltageOffset;    ///< The voltage offset applied to the core while operating in turbo mode.
  ///
  /// The core voltage override which is applied to the entire range of cpu core frequencies.
  ///
  UINT16 CoreVoltageOverride;
  ///
  /// Extra Turbo voltage applied to the cpu core when the cpu is operating in turbo mode.
  ///
  UINT16 CoreExtraTurboVoltage;
  ///
  /// Maximum core turbo ratio override allows to increase CPU core frequency beyond the fused max turbo ratio limit.
  ///
  UINT16 CoreMaxOcTurboRatio;
  INT16  ClrVoltageOffset;     ///< The voltage offset applied to CLR while operating in turbo mode.
  UINT16 ClrVoltageOverride;   ///< The clr voltage override which is applied to the entire range of cpu frequencies.
  UINT16 ClrExtraTurboVoltage; ///< Extra Turbo voltage applied to clr.
  ///
  /// Maximum clr turbo ratio override allows to increase CPU clr frequency beyond the fused max turbo ratio limit.
  ///
  UINT16 ClrMaxOcTurboRatio;
  UINT16 SvidVoltageOverride;  ///< Overrides the VR voltage through SVID bus to apply a new input voltage to the cpu.
  UINT8  SvidEnable           : 1; ///< Enable or Disable SVID communication from FIVR to the external VR.
  UINT8  FivrFaultsEnable     : 1; ///< Enable or Disable FIVR Fault management.
  UINT8  FivrEfficiencyEnable : 1; ///< Enable or Disable FIVR efficiency.
  UINT8  CoreVoltageMode      : 1; ///< 0: Adaptive, 1: Override
  UINT8  ClrVoltageMode       : 1; ///< 0: Adaptive, 1:Override
  UINT8  OcSupport            : 1; ///< 0: Disable, 1: Enable
  UINT8  BitReserved          : 2; ///< Bits reserved for future use
} OVERCLOCKING_CONFIG_PPI;

///
/// CPU Platform Policy PPI definition, this PPI provides interface to configure
/// the processor. Platform code needs to configure the policies for processor
/// ref code to finish the initialization.
///
struct _PEI_CPU_PLATFORM_POLICY_PPI {
  ///
  /// Revision for the PPI structure.
  ///
  UINT8                   Revision;
  ///
  /// Processor Features Configuration.
  ///
  CPU_CONFIG_PPI          *CpuConfig;
  ///
  /// Processor Power ManagementFeatures Configuration.
  ///
  POWER_MGMT_CONFIG_PPI   *PowerMgmtConfig;
  ///
  /// Processor Security features Configuration.
  ///
  SECURITY_CONFIG_PPI     *SecurityConfig;
  ///
  /// Processor OverClocking features Configuration.
  ///
  OVERCLOCKING_CONFIG_PPI *OverclockingConfig;
  ///
  /// PEI_CPU_PLATFORM_POLICY_PPI Ptr.
  ///
  UINTN                  CpuPlatformPpiPtr;
};

#endif
