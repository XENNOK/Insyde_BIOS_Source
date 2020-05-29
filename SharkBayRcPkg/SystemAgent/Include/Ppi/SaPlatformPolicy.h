/** @file
  Interface definition details between MRC and platform drivers during PEI phase.

;******************************************************************************
;* Copyright (c) 1983-2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2010 - 2013 Intel Corporation. All rights reserved
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
#ifndef _SA_PLATFORM_POLICY_PPI_H_
#define _SA_PLATFORM_POLICY_PPI_H_

#include "MrcApi.h"
#include "SaAccess.h"

///
/// SA Platform Policy for PEI phase {573EAF99-F445-46b5-A5D5-BC4A933598F3}
///
#define SA_PLATFORM_POLICY_PPI_GUID \
  { \
    0x573eaf99, 0xf445, 0x46b5, 0xa5, 0xd5, 0xbc, 0x4a, 0x93, 0x35, 0x98, 0xf3 \
  }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gSaPlatformPolicyPpiGuid;

///
/// PPI revision number
/// Any backwards compatible changes to this PPI will result in an update in the revision number
/// Major changes will require publication of a new PPI
///
/// Revision 1: Initial version.
///             Included in SA RC 0.5.0.0
///
#define SA_PLATFORM_POLICY_PPI_REVISION   1
///
/// Revision 2: Added PCIE_CONFIGURATION.PegGen3PresetSearchVoltageMarginSteps
///                   PCIE_CONFIGURATION.PegGen3PresetSearchVoltageStartMargin
///                   PCIE_CONFIGURATION.PegGen3PresetSearchFavorTiming
///                   PCIE_CONFIGURATION.PegDataPtr
///                   PCIE_CONFIGURATION.PegGen3ForcePresetSearch
///                   PCIE_CONFIGURATION.InitPcieAspmAfterOprom
///                   PCIE_CONFIGURATION.SaIotrapSmiAddress
///             Added MEMORY_CONFIGURATION.DqPinsInterleaved
///                   MEMORY_CONFIGURATION.DIMMODTT1D
///                   MEMORY_CONFIGURATION.WRSRT
///                   MEMORY_CONFIGURATION.DIMMRONT
///
#define SA_PLATFORM_POLICY_PPI_REVISION_2 2
///
/// Revision 3: Added SA_PLATFORM_DATA.SgMode
///                   SA_PLATFORM_DATA.SgSubSystemId
///             Added SA_PLATFORM_POLICY_PPI.SgGpioData
///                   PCIE_CONFIGURATION.PegGpioData
///                   PCIE_CONFIGURATION.PegGen3PresetSearchErrorTarget
///                   PCIE_CONFIGURATION.RxCEMLoopback
///                   PCIE_CONFIGURATION.RxCEMLoopbackLane
///                   PCIE_CONFIGURATION.Gen3RxCtleP[SA_PEG_MAX_LANE >> 1]
///             Obsoleted PCIE_CONFIGURATION.PegGen3PresetSearchMarginSteps
///                       PCIE_CONFIGURATION.PegGen3PresetSearchVoltageMarginSteps
///                       PCIE_CONFIGURATION.PegGen3PresetSearchFavorTiming
///                       MEMORY_CONFIGURATION.ForceColdReset
///             Added OVERCLOCKING_CONFIGURATION.GtVoltageMode
///                   OVERCLOCKING_CONFIGURATION.OcSupport
///                   MEMORY_CONFIGURATION.MrcUltPoSafeConfig
///
#define SA_PLATFORM_POLICY_PPI_REVISION_3 3
///
/// Revision 4: Added MEMORY_CONFIGURATION.CMDVC
///                   MEMORY_CONFIGURATION.PowerDownMode
///                   MEMORY_CONFIGURATION.PwdwnIdleCounter
///                   MEMORY_CONFIGURATION.RankInterleave
///                   MEMORY_CONFIGURATION.EnhancedInterleave
///                   MEMORY_CONFIGURATION.WeaklockEn;
///                   MEMORY_CONFIGURATION.EnCmdRate
///                   MEMORY_CONFIGURATION.CmdTriStateDis
///
#define SA_PLATFORM_POLICY_PPI_REVISION_4 4
///
/// Revision 5: Added MEMORY_CONFIGURATION.BClkFrequency
///                   OVERCLOCKING_CONFIGURATION.IoaVoltageOffset
///                   OVERCLOCKING_CONFIGURATION.IodVoltageOffset
///
#define SA_PLATFORM_POLICY_PPI_REVISION_5 5
///
/// Revision 6: Added MEMORY_CONFIGURATION.tRPab
///                   MEMORY_CONFIGURATION.ALIASCHK
///             Obsoleted MEMORY_CONFIGURATION.WRXTCT
///                       PCIE_CONFIGURATION.PegGen3EqualizationPhase2
///             Added GT_CONFIGURATION.GttMmAdr
///
#define SA_PLATFORM_POLICY_PPI_REVISION_6 6
///
/// Revision 7: Obsoleted MEMORY_CONFIGURATION.B2BXTT
///                       MEMORY_CONFIGURATION.C2CXTT
///             Added     MEMORY_CONFIGURATION.MemoryTrace
///
#define SA_PLATFORM_POLICY_PPI_REVISION_7 7
///
/// Revision 8: Added PCIE_CONFIGURATION.PowerDownUnusedBundles[SA_PEG_MAX_FUN]
///
#define SA_PLATFORM_POLICY_PPI_REVISION_8 8
///
/// Revision 9: Added SA_PLATFORM_DATA.IedSize
///                   MEMORY_CONFIGURATION.RefreshRate2x
///                   MEMORY_CONFIGURATION.ChHashEnable
///                   MEMORY_CONFIGURATION.ChHashMask
///                   MEMORY_CONFIGURATION.ChHashInterleaveBit
///
#define SA_PLATFORM_POLICY_PPI_REVISION_9 9
///
/// Revision 10: Added MEMORY_CONFIGURATION.EnableExtts
///                    MEMORY_CONFIGURATION.EnableCltm
///                    MEMORY_CONFIGURATION.EnableOltm
///                    MEMORY_CONFIGURATION.EnablePwrDn
///                    MEMORY_CONFIGURATION.EnablePwrDnLpddr
///                    MEMORY_CONFIGURATION.Refresh2X
///                    MEMORY_CONFIGURATION.LpddrThermalSensor
///                    MEMORY_CONFIGURATION.LockPTMregs
///                    MEMORY_CONFIGURATION.UserPowerWeightsEn
///                    MEMORY_CONFIGURATION.EnergyScaleFact
///                    MEMORY_CONFIGURATION.RaplPwrFlCh1
///                    MEMORY_CONFIGURATION.RaplPwrFlCh0
///                    MEMORY_CONFIGURATION.RaplLim2Lock
///                    MEMORY_CONFIGURATION.RaplLim2WindX
///                    MEMORY_CONFIGURATION.RaplLim2WindY
///                    MEMORY_CONFIGURATION.RaplLim2Ena
///                    MEMORY_CONFIGURATION.RaplLim2Pwr
///                    MEMORY_CONFIGURATION.RaplLim1WindX
///                    MEMORY_CONFIGURATION.RaplLim1WindY
///                    MEMORY_CONFIGURATION.RaplLim1Ena
///                    MEMORY_CONFIGURATION.RaplLim1Pwr
///                    MEMORY_CONFIGURATION.WarmThresholdCh0Dimm0
///                    MEMORY_CONFIGURATION.WarmThresholdCh0Dimm1
///                    MEMORY_CONFIGURATION.WarmThresholdCh1Dimm0
///                    MEMORY_CONFIGURATION.WarmThresholdCh1Dimm1
///                    MEMORY_CONFIGURATION.HotThresholdCh0Dimm0
///                    MEMORY_CONFIGURATION.HotThresholdCh0Dimm1
///                    MEMORY_CONFIGURATION.HotThresholdCh1Dimm0
///                    MEMORY_CONFIGURATION.HotThresholdCh1Dimm1
///                    MEMORY_CONFIGURATION.WarmBudgetCh0Dimm0
///                    MEMORY_CONFIGURATION.WarmBudgetCh0Dimm1
///                    MEMORY_CONFIGURATION.WarmBudgetCh1Dimm0
///                    MEMORY_CONFIGURATION.WarmBudgetCh1Dimm1
///                    MEMORY_CONFIGURATION.HotBudgetCh0Dimm0
///                    MEMORY_CONFIGURATION.HotBudgetCh0Dimm1
///                    MEMORY_CONFIGURATION.HotBudgetCh1Dimm0
///                    MEMORY_CONFIGURATION.HotBudgetCh1Dimm1
///                    MEMORY_CONFIGURATION.IdleEnergyCh0Dimm1
///                    MEMORY_CONFIGURATION.IdleEnergyCh0Dimm0
///                    MEMORY_CONFIGURATION.PdEnergyCh0Dimm1
///                    MEMORY_CONFIGURATION.PdEnergyCh0Dimm0
///                    MEMORY_CONFIGURATION.ActEnergyCh0Dimm1
///                    MEMORY_CONFIGURATION.ActEnergyCh0Dimm0
///                    MEMORY_CONFIGURATION.RdEnergyCh0Dimm1
///                    MEMORY_CONFIGURATION.RdEnergyCh0Dimm0
///                    MEMORY_CONFIGURATION.WrEnergyCh0Dimm1
///                    MEMORY_CONFIGURATION.WrEnergyCh0Dimm0
///                    MEMORY_CONFIGURATION.IdleEnergyCh1Dimm1
///                    MEMORY_CONFIGURATION.IdleEnergyCh1Dimm0
///                    MEMORY_CONFIGURATION.PdEnergyCh1Dimm1
///                    MEMORY_CONFIGURATION.PdEnergyCh1Dimm0
///                    MEMORY_CONFIGURATION.ActEnergyCh1Dimm1
///                    MEMORY_CONFIGURATION.ActEnergyCh1Dimm0
///                    MEMORY_CONFIGURATION.RdEnergyCh1Dimm1
///                    MEMORY_CONFIGURATION.RdEnergyCh1Dimm0
///                    MEMORY_CONFIGURATION.WrEnergyCh1Dimm1
///                    MEMORY_CONFIGURATION.WrEnergyCh1Dimm0
///                    MEMORY_CONFIGURATION.SrefCfgEna
///                    MEMORY_CONFIGURATION.SrefCfgIdleTmr
///                    MEMORY_CONFIGURATION.ThrtCkeMinDefeat
///                    MEMORY_CONFIGURATION.ThrtCkeMinTmr
///                    MEMORY_CONFIGURATION.ThrtCkeMinDefeatLpddr
///                    MEMORY_CONFIGURATION.ThrtCkeMinTmrLpddr
#define SA_PLATFORM_POLICY_PPI_REVISION_10 10
///
/// Revision 11: Added MEMORY_CONFIGURATION.CAVrefCtlOffset
///                    MEMORY_CONFIGURATION.Ch0VrefCtlOffset
///                    MEMORY_CONFIGURATION.Ch1VrefCtlOffset
///                    MEMORY_CONFIGURATION.Ch0ClkPiCodeOffset
///                    MEMORY_CONFIGURATION.Ch1ClkPiCodeOffset
///                    MEMORY_CONFIGURATION.Ch0RcvEnOffset
///                    MEMORY_CONFIGURATION.Ch0RxDqsOffset
///                    MEMORY_CONFIGURATION.Ch0TxDqOffset
///                    MEMORY_CONFIGURATION.Ch0TxDqsOffset
///                    MEMORY_CONFIGURATION.Ch0VrefOffset
///                    MEMORY_CONFIGURATION.Ch1RcvEnOffset
///                    MEMORY_CONFIGURATION.Ch1RxDqsOffset
///                    MEMORY_CONFIGURATION.Ch1TxDqOffset
///                    MEMORY_CONFIGURATION.Ch1TxDqsOffset
///                    MEMORY_CONFIGURATION.Ch1VrefOffset
///                    PCIE_CONFIGURATION.PegComplianceTestingMode
///                    MEMORY_CONFIGURATION.MaxRttWr
#define SA_PLATFORM_POLICY_PPI_REVISION_11 11
///
/// Revision 12: Added MEMORY_CONFIGURATION.RCVENC1D
///
#define SA_PLATFORM_POLICY_PPI_REVISION_12 12
///
/// Revision 13: Added MEMORY_CONFIGURATION.MCREGOFFSET
///
#define SA_PLATFORM_POLICY_PPI_REVISION_13 13
///
/// Revision 14: Added GT_CONFIGURATION.PanelPowerEnable
///          Obsoleted MEMORY_CONFIGURATION.CAVrefCtlOffset
///                    MEMORY_CONFIGURATION.Ch0VrefCtlOffset
///                    MEMORY_CONFIGURATION.Ch1VrefCtlOffset
///                    MEMORY_CONFIGURATION.Ch0ClkPiCodeOffset
///                    MEMORY_CONFIGURATION.Ch1ClkPiCodeOffset
///                    MEMORY_CONFIGURATION.Ch0RcvEnOffset
///                    MEMORY_CONFIGURATION.Ch0RxDqsOffset
///                    MEMORY_CONFIGURATION.Ch0TxDqOffset
///                    MEMORY_CONFIGURATION.Ch0TxDqsOffset
///                    MEMORY_CONFIGURATION.Ch0VrefOffset
///                    MEMORY_CONFIGURATION.Ch1RcvEnOffset
///                    MEMORY_CONFIGURATION.Ch1RxDqsOffset
///                    MEMORY_CONFIGURATION.Ch1TxDqOffset
///                    MEMORY_CONFIGURATION.Ch1TxDqsOffset
///                    MEMORY_CONFIGURATION.Ch1VrefOffset
///                    MEMORY_CONFIGURATION.MCREGOFFSET
///
#define SA_PLATFORM_POLICY_PPI_REVISION_14 14
///
/// Revision 15: Added: MEMORY_CONFIGURATION.RMC
///
#define SA_PLATFORM_POLICY_PPI_REVISION_15 15

#ifndef SA_MC_MAX_SOCKETS
#define SA_MC_MAX_SOCKETS 4
#endif

#define S3_TIMING_DATA_LEN          9
#define S3_READ_TRAINING_DATA_LEN   16
#define S3_WRITE_TRAINING_DATA_LEN  12
#define SAVE_PRESET_SEARCH_DATA_LEN 3

#ifndef S3_RESTORE_DATA_LEN
#define S3_RESTORE_DATA_LEN (S3_TIMING_DATA_LEN + S3_READ_TRAINING_DATA_LEN + S3_WRITE_TRAINING_DATA_LEN)
#endif // S3_RESTORE_DATA_LEN

#pragma pack(1)
///
/// SA Platform Data Structure
///
typedef struct {
  UINT8   SpdAddressTable[SA_MC_MAX_SOCKETS];///< Memory DIMMs' SPD address for reading SPD data
  UINT32  MchBar;                            ///< Address of System Agent MCHBAR
  UINT32  DmiBar;                            ///< Address of System Agent DMIBAR
  UINT32  EpBar;                             ///< Address of System Agent EPBAR
  UINT32  PciExpressBar;                     ///< Address of System Agent PCI EXPRESS BAR
  UINT32  SmbusBar;                          ///< Address of System Agent SMBUS BAR
  UINT32  GdxcBar;                           ///< Address of System Agent GDXCBAR
  UINT32  TsegSize;                          ///< Size of TSEG in bytes
  UINT8   UserBd;                            ///< Mobile - 0; Desktop/UpServer - 1
  UINT8   FastBoot;                          ///< FastBoot option: enable or disable MRC FastBoot feature
  UINT32  EdramBar;                          ///< Address of System Agent EDRAMBAR. Default of 0xfed80000
  UINT16  BoardId;                           ///< Platform Board ID
  UINT8   SgMode;                            ///< SgMode (0=Disabled, 1=SG Muxed, 2=SG Muxless, 3=PEG)
  UINT16  SgSubSystemId;                     ///< Switchable Graphics Subsystem ID
  UINT32  IedSize;                           ///< Size of IED region in bytes
} SA_PLATFORM_DATA;

///
/// GT Configuration
///
typedef struct {
  UINT16  MmioSize;           ///< Reserved MMIO space for Graphics
  UINT16  GttSize;            ///< Gtt Memory size of IGD
  UINT8   IgdDvmt50PreAlloc;  ///< Pre-allocated memory for IGD
  UINT8   InternalGraphics;   ///< Control for enabling/disabling iGfx device
  UINT8   PrimaryDisplay;     ///< Selection of the primary display device (iGFX, External PCIe or PCI Graphics)
  UINT8   ApertureSize;       ///< Graphics aperture size
  UINT32  GttMmAdr;           ///< Address of System Agent GTTMMADR
  UINT8   PanelPowerEnable;   ///< Control for enabling/disabling VDD force bit (Required only for early enabling of eDP panel)
} GT_CONFIGURATION;

///
/// SA_SPD
///
typedef enum {
  Default,        ///< 0, Default SPD
  UserDefined,    ///< 1, User Defined profile
  XMPProfile1,    ///< 2, XMP Profile 1
  XMPProfile2     ///< 3, XMP Profile 2
} SA_SPD;

///
/// Memory Configuration
///
typedef struct {
  UINT8   EccSupport;             ///< Ecc Support option - for Desktop only
  UINT16  DdrFreqLimit;           ///< Memory Frequency setting
  UINT8   MaxTolud;               ///< Maximum top of memory size below 4G
  SA_SPD  SpdProfileSelected;     ///< SPD XMP profile selection - for XMP supported DIMM
  UINT16  tCL;                    ///< User defined Memory Timing tCL value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tRCD;                   ///< User defined Memory Timing tRCD value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tRP;                    ///< User defined Memory Timing tRP value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tRAS;                   ///< User defined Memory Timing tRAS value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT8   NModeSupport;           ///< Memory N Mode Support - Enable user to select Auto, 1N or 2N
  UINT8   ScramblerSupport;       ///< Memory scrambler support
  UINT16  tWR;                    ///< User defined Memory Timing tWR value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tRFC;                   ///< User defined Memory Timing tRFC value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tRRD;                   ///< User defined Memory Timing tRRD value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tWTR;                   ///< User defined Memory Timing tWTR value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tRTP;                   ///< User defined Memory Timing tRP value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tRC;                    ///< User defined Memory Timing tRC value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tFAW;                   ///< User defined Memory Timing tFAW value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tCWL;                   ///< User defined Memory Timing tCWL value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT16  tREFI;                  ///< User defined Memory Timing tREFI value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  //
  // Thermal Management
  //
  UINT8   ThermalManagement;      ///< Memory Thermal Management Support - Enable/Disable
  UINT8   PeciInjectedTemp;       ///< Enable/Disable memory temperatures to be injected to the processor via PECI
  UINT8   ExttsViaTsOnBoard;      ///< Enable/Disable routing TS-on-Board's ALERT# and THERM# to EXTTS# pins on the PCH
  UINT8   ExttsViaTsOnDimm;       ///< Enable/Disable routing TS-on-DIMM's ALERT# to EXTTS# pin on the PCH
  UINT8   VirtualTempSensor;      ///< Enable/Disable Virtual Temperature Sensor (VTS)
  BOOLEAN ForceColdReset;         ///< @deprecated since revision 3
  ///
  /// Disables a DIMM slot in the channel even if a DIMM is present. Array index represents
  /// the channel number (0 = channel 0, 1 = channel 1). 0x0 = DIMM 0 and DIMM 1 enabled.
  ///
  ///   0x1 = DIMM 0 disabled, DIMM 1 enabled.
  ///   0x2 = DIMM 0 enabled, DIMM 1 disabled.
  ///   0x3 = DIMM 0 and DIMM 1 disabled (will disable the whole channel)
  ///
  UINT8   DisableDimmChannel[SA_MC_MAX_CHANNELS];
  BOOLEAN RemapEnable;             ///< This option is used to control whether to enable/disable memory remap above 4GB
  ///
  /// Sets the serial debug message level.
  ///   0x00 = Disabled
  ///   0x01 = Errors only.
  ///   0x02 = Errors and Warnings
  ///   0x03 = Errors, Warnings, and Info
  ///   0x04 = Errors, Warnings, Info, and Events
  ///   0x05 = Displays Memory Init Execution Time Summary only.
  ///
  UINT8   SerialDebug;

  UINT8   McLock;                    ///< Enable/Disable memory configuration register locking.
  UINT8   GdxcEnable;                ///< Enable/Disable GDXC debug support feature.
  UINT8   GdxcIotSize;               ///< GDXC IOT Size in 8MB granularity
  UINT8   GdxcMotSize;               ///< GDXC MOT Size in 8MB granularity
  //
  // Training Algorithms
  //
  UINT8   ECT;                       ///< Enable/Disable Early Command Training. Note it is not recommended to change this setting from the default value
  UINT8   SOT;                       ///< Enable/Disable Sense Amp Offset Training. Note it is not recommended to change this setting from the default value
  UINT8   RDMPRT;                    ///< Enable/Disable Read MPR Training. Note it is not recommended to change this setting from the default value
  UINT8   RCVET;                     ///< Enable/Disable Receive Enable Training. Note it is not recommended to change this setting from the default value
  UINT8   JWRL;                      ///< Enable/Disable JEDEC Write Leveling Training. Note it is not recommended to change this setting from the default value
  UINT8   FWRL;                      ///< Enable/Disable Functional Write Leveling Training. Note it is not recommended to change this setting from the default value
  UINT8   WRTC1D;                    ///< Enable/Disable 1D Write Timing Centering Training. Note it is not recommended to change this setting from the default value
  UINT8   RDTC1D;                    ///< Enable/Disable 1D Read Timing Centering Training. Note it is not recommended to change this setting from the default value
  UINT8   DIMMODTT;                  ///< Enable/Disable DIMM ODT Training. Note it is not recommended to change this setting from the default value
  UINT8   WRDST;                     ///< Enable/Disable Write Drive Strength Training. Note it is not recommended to change this setting from the default value
  UINT8   WREQT;                     ///< Enable/Disable Write Equalization Training. Note it is not recommended to change this setting from the default value
  UINT8   WRXTCT;                    ///< @deprecated since revision 6
  UINT8   RDODTT;                    ///< Enable/Disable Read ODT Training. Note it is not recommended to change this setting from the default value
  UINT8   RDEQT;                     ///< Enable/Disable Read Equalization Training. Note it is not recommended to change this setting from the default value
  UINT8   RDAPT;                     ///< Enable/Disable Read Amplifier Power Training. Note it is not recommended to change this setting from the default value
  UINT8   WRTC2D;                    ///< Enable/Disable 2D Write Timing Centering Training. Note it is not recommended to change this setting from the default value
  UINT8   RDTC2D;                    ///< Enable/Disable 2D Read Timing Centering Training. Note it is not recommended to change this setting from the default value
  UINT8   WRVC2D;                    ///< Enable/Disable 2D Write Voltage Centering Training. Note it is not recommended to change this setting from the default value
  UINT8   RDVC2D;                    ///< Enable/Disable 2D Read Voltage Centering Training. Note it is not recommended to change this setting from the default value
  UINT8   B2BXTT;                    ///< @deprecated since revision 7
  UINT8   C2CXTT;                    ///< @deprecated since revision 7
  UINT8   LCT;                       ///< Enable/Disable Late Command Training. Note it is not recommended to change this setting from the default value
  UINT8   RTL;                       ///< Enable/Disable Round Trip Latency function. Note it is not recommended to change this setting from the default value
  UINT8   TAT;                       ///< Enable/Disable Turn Around Time function. Note it is not recommended to change this setting from the default value
  UINT8   RMT;                       ///< Enable/Disable Rank Margin Tool function
  UINT8   MEMTST;                    ///< Enable/Disable Memory Test function

  UINT16  DDR3Voltage;               ///< Indicates if platform supports DDR3L DIMMs
  UINT32  DDR3VoltageWaitTime;       ///< Amount of time in microseconds to wait for the DDR3 voltage to change from 1.5V to 1.35V

  BOOLEAN RmtBdatEnable;             ///< Enable/Disable creating the BIOS DATA ACPI tables and populating with the RMT data
  ///
  /// Selects the DDR base reference clock.
  ///   0x00 = Auto
  ///   0x01 = 133MHz
  ///   0x02 = 100MHz
  ///
  UINT8   RefClk;
  ///
  /// Selects the ratio to multiply the reference clock by for the DDR frequency.
  ///   0x00 = Auto
  ///   0x03 through 0x0A are valid values, all others are invalid.
  ///
  UINT8   Ratio;
  UINT8   MrcTimeMeasure;        ///< Enables serial debug level to display the MRC execution times only
  UINT8   MrcFastBoot;           ///< Enables the MRC fast boot path for faster cold boot execution
  BOOLEAN DqPinsInterleaved;     ///< Interleaving mode of DQ/DQS pins for HSW_ULT which depends on board routing
  UINT8   DIMMODTT1D;            ///< DIMM ODT 1D training switch
  UINT8   WRSRT;                 ///< Write Slew Rate traning switch
  UINT8   DIMMRONT;              ///< DIMM RON training switch
  BOOLEAN MrcUltPoSafeConfig;    ///< MRC ULT PO Safe Mode
  UINT8   CMDVC;                 ///< Command Vref Centering
  UINT8   PowerDownMode;         ///< REVISION_4 - Power Down Mode
  UINT8   PwdwnIdleCounter;      ///< REVISION_4 - Power Down Mode Idle Counter
  UINT8   RankInterleave;        ///< REVISION_4 - Rank Interleave Mode
  UINT8   EnhancedInterleave;    ///< REVISION_4 - Enhanced Interleave Mode
  UINT8   WeaklockEn;            ///< REVISION_4 - Weak Lock Enable
  UINT8   EnCmdRate;             ///< REVISION_4 - CMD Rate Enable
  UINT8   CmdTriStateDis;        ///< REVISION_4 - CMD Tri-State Disable
  UINT32  BClkFrequency;         ///< REVISION_5 - Base reference clock value, in Hertz.
  UINT16  tRPab;                 ///< REVISION_6 - User defined Memory Timing tRPab value, it's valid when SpdProfileSelected is CUSTOM_PROFILE
  UINT8   ALIASCHK;              ///< REVISION_6 - DIMM SPD Alias Check Switch
  UINT8   MemoryTrace;           ///< REVISION_7 - Memory Trace to second DDR channel using Stacked Mode
  BOOLEAN RefreshRate2x;         ///< REVISION_9 - 2x Refresh Rate Enable
  UINT8   ChHashEnable;          ///< REVISION_9 - Channel Hash Enable
  UINT16  ChHashMask;            ///< REVISION_9 - Channel Hash Mask
  UINT8   ChHashInterleaveBit;   ///< REVISION_9 - Channel Hash Enable
  UINT8   EnableExtts;           ///< REVISION_10 - Enable Extts
  UINT8   EnableCltm;            ///< REVISION_10 - Enable Closed Loop Thermal Management
  UINT8   EnableOltm;            ///< REVISION_10 - Enable Open Loop Thermal Management
  UINT8   EnablePwrDn;           ///< REVISION_10 - Enable Power Down
  UINT8   EnablePwrDnLpddr;      ///< REVISION_10 - Enable Power Down for LPDDR
  UINT8   Refresh2X;             ///< REVISION_10 - Refresh 2x
  UINT8   LpddrThermalSensor;    ///< REVISION_10 - Lpddr Thermal Sensore
  UINT8   LockPTMregs;           ///< REVISION_10 - Lock PCU Thermal Management registers
  UINT8   UserPowerWeightsEn;    ///< REVISION_10 - Allows user to explicitly set power weight, scale factor, and channel power floor values
  UINT8   EnergyScaleFact;       ///< REVISION_10 - Energy Scale Factor
  UINT8   RaplPwrFlCh1;          ///< REVISION_10 - Power Channel 1 Floor value
  UINT8   RaplPwrFlCh0;          ///< REVISION_10 - Power Channel 0 Floor value
  UINT8   RaplLim2Lock;          ///< REVISION_10 - Lock DDR_RAPL_LIMIT register
  UINT8   RaplLim2WindX;         ///< REVISION_10 - Power Limit 2 Time Window X value
  UINT8   RaplLim2WindY;         ///< REVISION_10 - Power Limit 2 Time Window Y value
  UINT8   RaplLim2Ena;           ///< REVISION_10 - Enable Power Limit 2
  UINT16  RaplLim2Pwr;           ///< REVISION_10 - Power Limit 2
  UINT8   RaplLim1WindX;         ///< REVISION_10 - Power Limit 1 Time Window X value
  UINT8   RaplLim1WindY;         ///< REVISION_10 - Power Limit 1 Time Window Y value
  UINT8   RaplLim1Ena;           ///< REVISION_10 - Enable Power Limit 1
  UINT16  RaplLim1Pwr;           ///< REVISION_10 - Power Limit 1
  UINT8   WarmThresholdCh0Dimm0; ///< REVISION_10 - Warm Threshold (Channel 0, Dimm 0)
  UINT8   WarmThresholdCh0Dimm1; ///< REVISION_10 - Warm Threshold (Channel 0, Dimm 1)
  UINT8   WarmThresholdCh1Dimm0; ///< REVISION_10 - Warm Threshold (Channel 1, Dimm 0)
  UINT8   WarmThresholdCh1Dimm1; ///< REVISION_10 - Warm Threshold (Channel 1, Dimm 1)
  UINT8   HotThresholdCh0Dimm0;  ///< REVISION_10 - Hot Threshold (Channel 0, Dimm 0)
  UINT8   HotThresholdCh0Dimm1;  ///< REVISION_10 - Hot Threshold (Channel 0, Dimm 1)
  UINT8   HotThresholdCh1Dimm0;  ///< REVISION_10 - Hot Threshold (Channel 1, Dimm 0)
  UINT8   HotThresholdCh1Dimm1;  ///< REVISION_10 - Hot Threshold (Channel 1, Dimm 1)
  UINT8   WarmBudgetCh0Dimm0;    ///< REVISION_10 - Warm Budget (Channel 0, Dimm 0)
  UINT8   WarmBudgetCh0Dimm1;    ///< REVISION_10 - Warm Budget (Channel 0, Dimm 1)
  UINT8   WarmBudgetCh1Dimm0;    ///< REVISION_10 - Warm Budget (Channel 1, Dimm 0)
  UINT8   WarmBudgetCh1Dimm1;    ///< REVISION_10 - Warm Budget (Channel 1, Dimm 1)
  UINT8   HotBudgetCh0Dimm0;     ///< REVISION_10 - Hot Budget (Channel 0, Dimm 0)
  UINT8   HotBudgetCh0Dimm1;     ///< REVISION_10 - Hot Budget (Channel 0, Dimm 1)
  UINT8   HotBudgetCh1Dimm0;     ///< REVISION_10 - Hot Budget (Channel 1, Dimm 0)
  UINT8   HotBudgetCh1Dimm1;     ///< REVISION_10 - Hot Budget (Channel 1, Dimm 1)
  UINT8   IdleEnergyCh0Dimm1;    ///< REVISION_10 - Idle Energy (Channel 0, Dimm 1)
  UINT8   IdleEnergyCh0Dimm0;    ///< REVISION_10 - Idle Energy (Channel 0, Dimm 0)
  UINT8   PdEnergyCh0Dimm1;      ///< REVISION_10 - Power Down Energy (Channel 0, Dimm 1)
  UINT8   PdEnergyCh0Dimm0;      ///< REVISION_10 - Power Down Energy (Channel 0, Dimm 0)
  UINT8   ActEnergyCh0Dimm1;     ///< REVISION_10 - Activation Energy (Channel 0, Dimm 1)
  UINT8   ActEnergyCh0Dimm0;     ///< REVISION_10 - Activation Energy (Channel 0, Dimm 0)
  UINT8   RdEnergyCh0Dimm1;      ///< REVISION_10 - Read Energy (Channel 0, Dimm 1)
  UINT8   RdEnergyCh0Dimm0;      ///< REVISION_10 - Read Energy (Channel 0, Dimm 0)
  UINT8   WrEnergyCh0Dimm1;      ///< REVISION_10 - Write Energy (Channel 0, Dimm 1)
  UINT8   WrEnergyCh0Dimm0;      ///< REVISION_10 - Write Energy (Channel 0, Dimm 0)
  UINT8   IdleEnergyCh1Dimm1;    ///< REVISION_10 - Idle Energy (Channel 1, Dimm 1)
  UINT8   IdleEnergyCh1Dimm0;    ///< REVISION_10 - Idle Energy (Channel 1, Dimm 0)
  UINT8   PdEnergyCh1Dimm1;      ///< REVISION_10 - Power Down Energy (Channel 1, Dimm 1)
  UINT8   PdEnergyCh1Dimm0;      ///< REVISION_10 - Power Down Energy (Channel 1, Dimm 0)
  UINT8   ActEnergyCh1Dimm1;     ///< REVISION_10 - Activation Energy (Channel 1, Dimm 1)
  UINT8   ActEnergyCh1Dimm0;     ///< REVISION_10 - Activation Energy (Channel 1, Dimm 0)
  UINT8   RdEnergyCh1Dimm1;      ///< REVISION_10 - Read Energy (Channel 1, Dimm 1)
  UINT8   RdEnergyCh1Dimm0;      ///< REVISION_10 - Read Energy (Channel 1, Dimm 0)
  UINT8   WrEnergyCh1Dimm1;      ///< REVISION_10 - Write Energy (Channel 1, Dimm 1)
  UINT8   WrEnergyCh1Dimm0;      ///< REVISION_10 - Write Energy (Channel 1, Dimm 0)
  UINT8   SrefCfgEna;            ///< REVISION_10 - Enable Self Refresh
  UINT16  SrefCfgIdleTmr;        ///< REVISION_10 - Self Refresh idle timer
  UINT8   ThrtCkeMinDefeat;      ///< REVISION_10 - Throttler CKE min defeature
  UINT8   ThrtCkeMinTmr;         ///< REVISION_10 - Throttler CKE min timer
  UINT8   ThrtCkeMinDefeatLpddr; ///< REVISION_10 - Throttler CKE min defeature for LPDDR
  UINT8   ThrtCkeMinTmrLpddr;    ///< REVISION_10 - Throttler CKE min timer for LPDDR
  UINT8   CAVrefCtlOffset;       ///< @deprecated since revision 14
  UINT8   Ch0VrefCtlOffset;      ///< @deprecated since revision 14
  UINT8   Ch1VrefCtlOffset;      ///< @deprecated since revision 14
  UINT8   Ch0ClkPiCodeOffset;    ///< @deprecated since revision 14
  UINT8   Ch1ClkPiCodeOffset;    ///< @deprecated since revision 14
  UINT8   Ch0RcvEnOffset;        ///< @deprecated since revision 14
  UINT8   Ch0RxDqsOffset;        ///< @deprecated since revision 14
  UINT8   Ch0TxDqOffset;         ///< @deprecated since revision 14
  UINT8   Ch0TxDqsOffset;        ///< @deprecated since revision 14
  UINT8   Ch0VrefOffset;         ///< @deprecated since revision 14
  UINT8   Ch1RcvEnOffset;        ///< @deprecated since revision 14
  UINT8   Ch1RxDqsOffset;        ///< @deprecated since revision 14
  UINT8   Ch1TxDqOffset;         ///< @deprecated since revision 14
  UINT8   Ch1TxDqsOffset;        ///< @deprecated since revision 14
  UINT8   Ch1VrefOffset;         ///< @deprecated since revision 14
  BOOLEAN AutoSelfRefreshSupport;///< REVISION_11 - FALSE = No auto self refresh support, TRUE = auto self refresh support.
  BOOLEAN ExtTemperatureSupport; ///< REVISION_11 - FALSE = No extended temperature support, TRUE = extended temperature support.
  UINT8   MaxRttWr;              ///< REVISION_11 - Maximum DIMM RTT_WR to use in power training 0 = Off, 1 = 120 ohms
  UINT8   RCVENC1D;              ///< REVISION_12 - Receive Enable Centering Training.  LPDDR Only.
  UINT8   MCREGOFFSET;           ///< @deprecated since revision 14
  UINT8   RMC;                   ///< REVISION_15 - Retrain Margin Check Enable/Disable
} MEMORY_CONFIGURATION;

typedef struct {
  UINTN   CpuId;
  UINTN   CapIda;
  UINTN   CapIdb;
  UINT16  Peg0VenId;
  UINT16  Peg0DevId;
  UINT16  Peg1VenId;
  UINT16  Peg1DevId;
  UINT16  Peg2VenId;
  UINT16  Peg2DevId;
} PRESET_SEARCH_SIGNATURE;

///
/// SA GPIO Data Structure
///
typedef struct {
  UINT8   Value;  ///< GPIO Value
  BOOLEAN Active; ///< 0=Active Low; 1=Active High
} SA_GPIO_INFO;

///
/// SA Board PEG GPIO Info
///
typedef struct {
  BOOLEAN       GpioSupport;  ///< 1=Supported; 0=Not Supported
  SA_GPIO_INFO  *SaPegReset;  ///< PEG PERST# GPIO assigned
} PEG_GPIO_DATA;

///
/// Information for PCI Express controller configuration and DMI VC enable/disable control
///
typedef struct {
  UINT8             DmiVc1;                                ///< DMI Virtual channel 1 control (enable or disable)
  UINT8             DmiVcp;                                ///< DMI Virtual channel P control (enable or disable)
  UINT8             DmiVcm;                                ///< DMI Virtual channel M control (enable or disable)
  UINT8             DmiGen2;                               ///< DMI Gen2 support control (enable or disable)
  UINT8             AlwaysEnablePeg;                       ///< Force PEG controllers to always be enabled
  UINT8             PegGenx[SA_PEG_MAX_FUN];               ///< PEG Port PCIE GenX control (Gen1, Gen2 or Gen3)
  UINT8             PegGen3Equalization;                   ///< Enable PEG Gen3 static Presets programming. 0=Disable and 1=Enable (Default)
  UINT8             Gen3RootPortPreset[SA_PEG_MAX_LANE];   ///< Used for programming PEG Gen3 preset values per lane. Range: 0 to 9, 8 is default for each lane
  UINT8             Gen3EndPointPreset[SA_PEG_MAX_LANE];   ///< Used for programming PEG Gen3 preset values per lane. Range: 0 to 9, 7 is default for each lane
  UINT8             Gen3EndPointHint[SA_PEG_MAX_LANE];     ///< Hint value per lane for GEN3 end point device. Range: 0 to 6, 2 is default for each lane
//[-start-110930-IB05300224-add]//
  UINT8             PcieResetDelay;
//[-end-110930-IB05300224-add]//
  UINT8             PegSamplerCalibrate;                   ///< Enable/Disable PEG Sampler Calibration. 0=Disable (Default) and 1=Enable
  UINT8             PegGen3EqualizationPhase2;             ///< @deprecated since revision 6
  ///
  /// Note: An attack on the PresetSearch policies could result in an apparent hang,
  ///   but the system will eventually boot.  These variables should be protected.
  ///
  UINT8             PegGen3PresetSearch;                   ///< Enable/Disable PEG Gen3 Preset Search algorithm which improves GEN3 link quality. 0=Disable and 1=Enable (Default)
  UINT16            PegGen3PresetSearchDwellTime;          ///< Used for PEG Gen3 Preset Search algorithm. Range: 0 to 65535, default is 1000
  UINT8             PegGen3PresetSearchMarginSteps;        ///< @deprecated since revision 3
  UINT8             PegGen3PresetSearchStartMargin;        ///< Used for PEG Gen3 Preset Search algorithm. Range: 4 to 255, default is 15
  UINT8             PegSwingControl;                       ///< Used for PEG Swing Control in PCIe Recipe steps. 1=Half and 2=Full (Default)
  UINT8             PegGen3PresetSearchVoltageMarginSteps; ///< @deprecated since revision 3
  UINT8             PegGen3PresetSearchVoltageStartMargin; ///< The starting value for the Gen3 preset search voltage backward margin search. Range: 4 to 255, default is 20
  UINT8             PegGen3PresetSearchFavorTiming;        ///< @deprecated since revision 3
  ///
  /// This is a memory data pointer for saved preset search results. The reference code will store
  ///   the Gen3 Preset Search results in the SaDataHob's PegData structure. In order to skip the Gen3
  ///   preset search on boots where the PEG card configuration has not changed since the previous boot,
  ///   platform code can save the contents of the SaDataHob's PegData structure in DXE and provide a
  ///   pointer to a restored copy of that data. Default value is NULL, which results in a full
  ///   preset search every boot.
  ///
  /// Note: An attack on this policy could prevent the PEG display from working until a boot when
  ///   PegDataPtr is NULL or PegGen3ForcePresetSearch is enabled.  The variable used to save the
  ///   preset search results should be protected in a way that it can only be modified by the
  ///   platform manufacturer.
  ///
  VOID              *PegDataPtr;
  ///
  /// When enabled, Gen3 preset search will be executed each boot. Otherwise, it will be skipped
  ///   and the previous Preset value will be re-used in the following boot. 0=Disable (Default)
  ///   and 1=Enable
  ///
  BOOLEAN           PegGen3ForcePresetSearch;
  ///
  /// Set to 1 to do the PCIe ASPM programming after Oprom. This will require one SMM IO Trap handler
  ///   implemented so ensure the build flag "SA_PCIE_ASPM_IN_SMM=1" is defined in compiling parameter.
  ///   Set to 0 to do the PCIe ASPM programming before Oprom. ("SA_PCIE_ASPM_IN_DXE=1" should be
  ///   defined in compiling parameter)
  ///
  /// Note:  Platform designer may include both SMM and DXE implementation (both build switch defined
  ///   as 1) and provide a setup option to support different scenarios by this policy. It is required
  ///   to have at least one implementation (either DXE or SMM implementation) included and working
  ///   on platform or it may have security concern.
  ///
  BOOLEAN           InitPcieAspmAfterOprom;
  ///
  /// This IO Trap address is required to support PCIe ASPM programming after Oprom. When build switch
  ///   "SA_PCIE_ASPM_IN_SMM=1" defined in compiling parameter this IO Trap address must be provided
  ///   and should not conflict with any other IO address used by platform
  ///
  UINT16            SaIotrapSmiAddress;
  ///
  /// This is a memory data pointer to PCIe PERST# GPIO information.  This structure is required
  ///   for PCIe Gen3 operation. The reference code will use the information in this structure in
  ///   order to reset PCIe Gen3 devices during equalization, if necessary.  Refer to the Platform
  ///   Developer's Guide (PDG) for additional details.  The default value of NULL is only valid
  ///   for systems without PCIe Gen3 devices connected to the PEG controllers.
  ///
  PEG_GPIO_DATA     *PegGpioData;
  UINT16            PegGen3PresetSearchErrorTarget;        ///< Used for PEG Gen3 Preset Search algorithm. Range: 0 to 65535, default is 4
  BOOLEAN           RxCEMLoopback;                         ///< Enable/Disable RxCEMLoopback Test Mode. 0=Disable (default) and 1=Enable
  UINT8             RxCEMLoopbackLane;                     ///< When RxCEMLoopback Test Mode enabled, a lane (0 ~ 15) has to be specified by this policy. Default is lane0 (0)
  ///
  /// PCIe Gen3 RxCTLEp per-Bundle control support. The range of the setting is (0~15). This setting
  ///   has to be specified based upon platform design and must follow the guideline. Default is 12.
  ///
  UINT8             Gen3RxCtleP[SA_PEG_MAX_BUNDLE];
  ///
  /// PCIe power down unused bundles support. It has 3 policies.
  ///   Disabled (0x0)     : No power saving.
  ///   Auto (0xFF)        : Bios will decide unused bundles to power down [if the controller max-linkwidth is greater than the endpoint max-linkwidth
  ///                        then bios will calculate the unused bundles to power down].
  ///   1-n bundles (0x1-n): Number of bundles to power down via user selection [n depends on HW configuration].
  ///
  UINT8             PowerDownUnusedBundles[SA_PEG_MAX_FUN];
  ///
  /// PCIe compliance testing mode. Set this bit to when running a PCIe compliance test. Leave disabled during normal operation.
  ///  - Disabled (0x0)     : Normal Operation          - Disable PCIe compliance testing
  ///  - Enabled  (0x1)     : PCIe Compliance Test Mode - PEG controller is in compliance testing mode, should only be set when doing PCIe compliance testing
  /// @since Added in revision 11
  UINT8             PegComplianceTestingMode;
} PCIE_CONFIGURATION;

///
/// Defines the overclocking configuration parameters for System Agent.
///
typedef struct {
  INT16   GtVoltageOffset;      ///< The voltage offset applied to GT. Valid range from -1000mv to 1000mv
  UINT16  GtVoltageOverride;    ///< The GT voltage override which is applied to the entire range of GT frequencies
  UINT16  GtExtraTurboVoltage;  ///< The adaptive voltage applied during turbo frequencies. Valid range from 0 to 2000mV
  UINT16  GtMaxOcTurboRatio;    ///< Maximum GT turbo ratio override
  INT16   SaVoltageOffset;      ///< The voltage offset applied to the SA. Valid range from -1000mv to 1000mv
  UINT8   GtVoltageMode;        ///< Specifies whether GT voltage is operating in Adaptive or Override mode - 0: Adaptive, 1: Override
  UINT8   OcSupport;            ///< Enable disable of SA overclocking mailbox commands
  INT16   IoaVoltageOffset;     ///< The voltage offset applied to the IOA domain. Valid Range -1000mv to 1000mv
  INT16   IodVoltageOffset;     ///< The voltage offset applied to the IOD domain. Valid Range -1000mv to 1000mv
} OVERCLOCKING_CONFIGURATION;

///
/// Defines the Switchable Graphics configuration parameters for System Agent.
///
typedef struct {
  BOOLEAN       GpioSupport;        ///< 1=Supported; 0=Not Supported
  SA_GPIO_INFO  *SgDgpuPwrOK;       ///< This field contain dGPU PWROK GPIO value and Level information
  SA_GPIO_INFO  *SgDgpuHoldRst;     ///< This field contain dGPU HLD RESET GPIO value and level information
  SA_GPIO_INFO  *SgDgpuPwrEnable;   ///< This field contain dGPU_PWR Enable GPIO value and level information
  SA_GPIO_INFO  *SgDgpuPrsnt;       ///< This field contain dGPU_PRSNT# GPIO value and level information
} SG_GPIO_DATA;

///
/// SA Platform Policy PPI
///
/// Note: User must initialize the policy "UserBd" based on the customer platform
/// (Mobile - 0; Desktop/UpServer - 1) in customer BIOS before executing MRC. This policy
/// isn't initialized in the Sample Code.
///
typedef struct _SA_PLATFORM_POLICY_PPI {
  ///
  /// This field specifies the revision of the PPI.  The PPI is expected to change in a
  /// backwards compatible manner as the chipset configuration options are added or removed.
  /// Major changes will result in new PPI definitions/GUID.  The PPI producer must update this
  /// field at build time.
  ///
  /// Please ensure to use SA_PLATFORM_POLICY_PPI_REVISION macro to define the protocol revision
  /// as input for this version
  ///
  UINT8                       Revision;
  SA_PLATFORM_DATA            *PlatformData;             ///< Platform specific data
  GT_CONFIGURATION            *GtConfig;                 ///< Contains the information for Graphic configurations
  MEMORY_CONFIGURATION        *MemConfig;                ///< Contains the information for Memory configurations
  PCIE_CONFIGURATION          *PcieConfig;               ///< Contains the information for PCI Express controller configurations
  OVERCLOCKING_CONFIGURATION  *OcConfig;                 ///< Contains the information for Overclocking configurations
  VOID                        *S3DataPtr;                ///< Memory data save pointer for S3 resume. The memory space should be allocated and filled with proper S3 resume data on a resume path
  UINT8                       ScramblerSeedCmosLocation; ///< @deprecated since revision 2
  SG_GPIO_DATA                *SgGpioData;               ///< Switchable Graphics GPIO data (REVISION_3)
} SA_PLATFORM_POLICY_PPI;

#pragma pack()

#endif // _SA_PLATFORM_POLICY_PPI_H_
