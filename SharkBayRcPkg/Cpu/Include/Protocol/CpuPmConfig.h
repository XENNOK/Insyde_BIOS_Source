/** @file
  Describes the defintions / functions visible to the rest of the PPM.

@copyright
  Copyright (c) 2011 - 2013 Intel Corporation. All rights reserved
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
#ifndef _CPU_PM_CONFIG_H_
#define _CPU_PM_CONFIG_H_

///
/// Define maximum number of custom ratio states supported
///
#define MAX_CUSTOM_RATIO_TABLE_ENTRIES  16

///
/// Define maximum number of custom ConfigTdp entries supported
///
#define MAX_CUSTOM_CTDP_ENTRIES 3

///
/// This structure is used to describe the custom processor ratio table desired by the platform
///
typedef struct {
  ///
  /// The number of custom ratio state entries, it must be a value from 2 to 16 for a valid custom ratio table.
  ///
  UINT8  NumberOfEntries;
  UINT32 Cpuid;            ///< The CPU ID for which this custom ratio table applies.
  UINT16 MaxRatio;         ///< The maximum ratio of the custom ratio table.
  UINT16 StateRatio[MAX_CUSTOM_RATIO_TABLE_ENTRIES]; ///< The processor ratios in the custom ratio table.
} PPM_CUSTOM_RATIO_TABLE;

///
/// This structure is used to describe which of the PPM functions will be enabled by PPM implementation.
///
typedef struct {
  UINT32 Eist                  : 1; ///< Enable or Disable Intel SpeedStep Technology.
  UINT32 Cx                    : 1; ///< Enable or Disable CPU power states (C-states).
  UINT32 C1e                   : 1; ///< Enable or Disable Enhanced C-states.
  UINT32 C3                    : 1; ///< Enable or Disable C3.
  UINT32 C6                    : 1; ///< Enable or Disable C6.
  UINT32 DeepCState            : 3; ///< Enable or Disable C7/C7S.
  UINT32 LongLatencyC6         : 1; ///< Enable or Disable C6 Long Latency.
  UINT32 LongLatencyC7         : 1; ///< Enable or Disable C7 Long Latency.
  UINT32 C8                    : 1; ///< Enable or Disable C8.
  UINT32 C9                    : 1; ///< Enable or Disable C9.
  UINT32 C10                   : 1; ///< Enable or Disable C10.
  UINT32 C1AutoDemotion        : 1; ///< Enable or Disable C1 Auto Demotion.
  UINT32 C3AutoDemotion        : 1; ///< Enable or Disable C3 Auto Demotion.
  UINT32 TurboMode             : 1; ///< Enable or Disable long duration Turbo Mode.
  UINT32 PowerLimit2           : 1; ///< Enable or Disable short duration Turbo Mode.
  UINT32 EnergyEfficientPState : 1; ///< Enable or Disable Energy Efficient P-state will be applied in Turbo mode.
  UINT32 BiProcHot             : 1; ///< @deprecated in Powermgmt policy and moved to THERM_FUNCTION_ENABLES.
  UINT32 TStates               : 1; ///< @deprecated in Powermgmt policy and moved to THERM_FUNCTION_ENABLES.
  UINT32 Xe                    : 1; ///< Enable or Disable Intel Extreme Edition support.
  UINT32 C1UnDemotion          : 1; ///< Enable or Disable C1UnDemotion.
  UINT32 C3UnDemotion          : 1; ///< Enable or Disable C3UnDemotion.
  UINT32 PkgCStateDemotion     : 1; ///< Enable or Disable Package Cstate Demotion.
  UINT32 PkgCStateUnDemotion   : 1; ///< Enable or Disable Package Cstate UnDemotion.
  UINT32 DisableProcHotOut     : 1; ///< @deprecated in Powermgmt policy and moved to THERM_FUNCTION_ENABLES.
  UINT32 DisableVRThermalAlert : 1; ///< @deprecated in Powermgmt policy and moved to THERM_FUNCTION_ENABLES.
  UINT32 ProcHotResponce       : 1; ///< @deprecated in Powermgmt policy and moved to THERM_FUNCTION_ENABLES.
  UINT32 AutoThermalReporting  : 1; ///< @deprecated in Powermgmt policy and moved to THERM_FUNCTION_ENABLES.
  UINT32 CStatePreWake         : 1; ///< Enable or Disable CState-Pre wake.
  UINT32 ThermalMonitor        : 1; ///< @deprecated in Powermgmt policy and moved to THERM_FUNCTION_ENABLES.
  UINT32 LakeTiny              : 1; ///< Enable or Disable LakeTiny Support.
  UINT32 TimedMwait            : 1; ///< Enable or Disable TimedMwait Support.
  UINT32 Reserved              : 2; ///< Bits reserved for future use.
} PPM_FUNCTION_ENABLES;

///
/// This structure is used to describe various PPM turbo settings
///
typedef struct _PPM_TURBO_SETTINGS {
  UINT16 PowerLimit1;     ///< Package Long duration turbo mode power limit in 125mw or watts.
  UINT32 PowerLimit1Time; ///< Package Long duration turbo mode time window in seconds.
  UINT16 PowerLimit2;     ///< Package Short duration turbo mode power limit in 125mw or watts.
  ///
  /// Describes whether TURBO_POWER_LIMIT[63] should be set. Setting this bit will lock all Turbo settings.
  ///
  UINT8  TurboPowerLimitLock;
  UINT16 DdrPowerLimit1;     ///< @deprecated in Powermgmt policy
  UINT32 DdrPowerLimit1Time; ///< @deprecated in Powermgmt policy
  UINT16 DdrPowerLimit2;     ///< @deprecated in Powermgmt policy
  UINT8  DdrPowerLimitLock;  ///< @deprecated in Powermgmt policy
  ///
  /// Configuration for boot TDP selection, value 0 describes TDP Nominal, value 1 describes TDP Down and
  /// value 2 describes TDP Up.
  ///
  UINT8  ConfigTdpLevel;
  ///
  /// Configurable TDP Mode Lock can be sets the to Lock ConfigTdp mode settings from runtime change.
  ///
  UINT8  ConfigTdpLock;
  UINT8  ConfigTdpBios; ///< Configuration whether load Configurable TDP SSDT.
  UINT8  EnergyPolicy;  ///< Describes the Energy efficiency policy to be set in MSR 0x1B0.

  // 
  // PL3 configuration
  //
  UINT16 PowerLimit3;           ///< Package PL3 power limit in 125mw or watts.
  UINT32 PowerLimit3Time;       ///< Package PL3 time window in seconds.
  UINT8  PowerLimit3DutyCycle;  ///< Package PL3 Duty Cycle.
  UINT8  PowerLimit3Lock;       ///< Package PL3 MSR 615h lock.

} PPM_TURBO_SETTINGS;

///
/// PPM Custom ConfigTdp Settings
///
typedef struct _PPM_CUSTOM_CTDP_TABLE {
  UINT16 CustomPowerLimit1;          ///< Short term Power Limit value for custom cTDP level in 125mw or watts.
  UINT16 CustomPowerLimit2;          ///< Long term Power Limit value for custom cTDP level in 125mw or watts.
  UINT8  CustomPowerLimit1Time;      ///< Short term Power Limit time window value for custom cTDP level.
  UINT8  CustomTurboActivationRatio; ///< Turbo Activation Ratio for custom cTDP level.
  UINT8  CustomConfigTdpControl;     ///< Config Tdp Control (0/1/2) value for custom cTDP level.
} PPM_CUSTOM_CTDP_TABLE;

///
/// This structure is used to configure custom ConfigTdp level settings.
///
typedef struct _PPM_CUSTOM_CTDP {
  UINT8                 ConfigTdpCustom;     ///< Describes whether or not Custom Config Tdp should be enabled.
  UINT8                 CustomTdpCount;      ///< Describes the number of Custom Config Tdp levels required (1/2/3).
  UINT8                 CustomBootModeIndex; ///< Describes the Boot mode index selection from Custom Tdp levels.Index to CustomConfigTdpTable. valid values are 0,1,2.
  ///
  /// Describes the details of each Custom Config Tdp levels. This supports up to  MAX_CUSTOM_CTDP_ENTRIES number
  /// of Custom ConfigTdp levels.
  ///
  PPM_CUSTOM_CTDP_TABLE CustomConfigTdpTable[MAX_CUSTOM_CTDP_ENTRIES];
} PPM_CUSTOM_CTDP;

///
/// This structure is used to control enabled / disabled various PPM MSR lock settings
///
typedef struct _PPM_LOCK_ENABLES {
  UINT32 PmgCstCfgCtrlLock : 1;   ///< Setting this to 1 will set MSR 0xE2[15]
  UINT32 OverclockingLock  : 1;   ///< Setting this to 1 will set MSR 0x194[20]
  UINT32 ProcHotLock       : 1;   ///< Setting this to 1 will set MSR 0x1FC[23]
  UINT32 Reserved          : 29;  ///< Bits reserved for future use.
} PPM_LOCK_ENABLES;
///
/// PM Deep C State Limit
///
typedef enum {
  Disabled                = 0,
  DeepC7,
  DeepC7S
} DEEP_C_STATE;
///
/// PPM Package C State Limit
///
typedef enum {
  PkgC0C1                 = 0,
  PkgC2,
  PkgC3,
  PkgC6,
  PkgC7,
  PkgC7s,
  PkgC8,
  PkgC9,
  PkgC10,
  PkgCMax,
  PkgCpuDefault = 254,
  PkgAuto = 255
} MAX_PKG_C_STATE;
///
/// PPM Package C State Time Limit
///
typedef enum {
  TimeUnit1ns             = 0,
  TimeUnit32ns,
  TimeUnit1024ns,
  TimeUnit32768ns,
  TimeUnit1048576ns,
  TimeUnit33554432ns
} C_STATE_TIME_UNIT;
///
/// Custom Power Uints.User can choose to enter in MilliWatts or Watts
///
typedef enum {
  PowerUnitWatts = 0,       ///< in Watts
  PowerUnit125MilliWatts,   ///< in 125 Milli Watts. Example 11.250 W Value to use for Power limts 90
  PowerUnitMax
} CUSTOM_POWER_UNIT;

typedef enum {
  Percent5  = 242,
  Percent10 = 229,
  Percent15 = 217,
  Percent20 = 204,
  Percent25 = 191,
  Percent30 = 178,
  Percent35 = 166,
  Percent40 = 153,
  Percent45 = 140,
  Percent50 = 127,
  Percent55 = 115,
  Percent60 = 102,
  Percent65 = 89,
  Percent70 = 76,
  Percent75 = 64,
  Percent80 = 51,
  Percent85 = 38,
  Percent90 = 25,
  Percent95 = 13,
  Percent100 = 0
} PL1_THERMAL_THROTTLE_FLOOR_UNIT;

typedef struct {
    PL1_THERMAL_THROTTLE_FLOOR_UNIT FloorIA;  /// < FLOOR_IA, Default: 0 (Percent100)
    PL1_THERMAL_THROTTLE_FLOOR_UNIT FloorGT;  /// < FLOOR_GT, Default: 0 (Percent100)
    PL1_THERMAL_THROTTLE_FLOOR_UNIT FloorPCH; /// < FLOOR_PCH, Default: 0 (Percent100)
} PL1_THERMAL_CONTROL_FLOOR;

///
/// This structure is used to describe which of the Thermal functions will be enabled by Thermal implementation.
///
typedef struct {
  UINT16 BiProcHot             : 1; ///< Enable or Disable Bi-Directional PROCHOT#.
  UINT16 TStates               : 1; ///< Enable or Disable T states.
  UINT16 DisableProcHotOut     : 1; ///< Enable or Disable PROCHOT# signal being driven externally.
  UINT16 DisableVRThermalAlert : 1; ///< Enable or Disable VR Thermal Alert.
  UINT16 ProcHotResponce       : 1; ///< Enable or Disable PROCHOT# Responce.
  UINT16 AutoThermalReporting  : 1; ///< Enable or Disable Thermal Reporting through ACPI tables.
  UINT16 ThermalMonitor        : 1; ///< Enable or Disable Thermal Monitor.
  UINT16 Pl1ThermalControl     : 2; ///< Disable(0), Enable/Manual(1), Auto(2) PL1 thermal throttling features
  UINT16 ThermReserved         : 7; ///< Reserved
  PL1_THERMAL_CONTROL_FLOOR Pl1ThermalControlFloor;  ///< PL1 Floor Throttle Values
} THERM_FUNCTION_ENABLES;

///
/// Power management Configuration for all processor Power Management features enabling definitions are in this field.
/// Platform code can enable/disable features thru this field.
///
typedef struct {
  ///
  /// This structure is used to describe which of the PPM functions should be enabled. For details of this structure,
  /// please see Related Definitions.
  ///
  PPM_FUNCTION_ENABLES   *pFunctionEnables;
  ///
  /// This structure is used to describe the custom CPU Frequency Table that should be used. For details of this
  /// structure, please see Related Definitions.
  ///
  PPM_CUSTOM_RATIO_TABLE *pCustomRatioTable;
  ///
  /// This structure is used to describe long duration and short duration turbo settings. For details of this
  /// structure, please see Related Definitions.
  ///
  PPM_TURBO_SETTINGS     *pTurboSettings;
  UINT8                  S3RestoreMsrSwSmiNumber; ///< SW SMI number to restore the power Mgmt MSRs during S3 resume.
  UINT8                  *pRatioLimit;         ///< This field is a pointer to Ratio Limit.
  PPM_LOCK_ENABLES       *pPpmLockEnables;     ///< This field is a pointer to PPM_LOCK_ENABLES structure.
  PPM_CUSTOM_CTDP        *pCustomCtdpSettings; ///< This structure is used to describe the custom config TDP settings.
  ///
  /// This field is used to set the Max Pkg Cstate. Default set to Auto which limits the Max Pkg Cstate to deep C-state.
  ///
  MAX_PKG_C_STATE        PkgCStateLimit;
  C_STATE_TIME_UNIT      CstateLatencyControl0TimeUnit; ///< TimeUnit for Latency Control0 MSR 0x60A[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl1TimeUnit; ///< TimeUnit for Latency Control1 MSR 0x60B[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl2TimeUnit; ///< TimeUnit for Latency Control2 MSR 0x60C[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl3TimeUnit; ///< TimeUnit for Latency Control3 MSR 0x633[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl4TimeUnit; ///< TimeUnit for Latency Control4 MSR 0x634[12:10].
  C_STATE_TIME_UNIT      CstateLatencyControl5TimeUnit; ///< TimeUnit for Latency Control5 MSR 0x635[12:10].
  UINT16                 CstateLatencyControl0Irtl; ///< Interrupt Response Time Limit of LatencyContol0 MSR 0x60A[9:0].
  UINT16                 CstateLatencyControl1Irtl; ///< Interrupt Response Time Limit of LatencyContol1 MSR 0x60B[9:0].
  UINT16                 CstateLatencyControl2Irtl; ///< Interrupt Response Time Limit of LatencyContol2 MSR 0x60C[9:0].
  UINT16                 CstateLatencyControl3Irtl; ///< Interrupt Response Time Limit of LatencyContol3 MSR 0x633[9:0].
  UINT16                 CstateLatencyControl4Irtl; ///< Interrupt Response Time Limit of LatencyContol4 MSR 0x634[9:0].
  UINT16                 CstateLatencyControl5Irtl; ///< Interrupt Response Time Limit of LatencyContol5 MSR 0x635[9:0].
  BOOLEAN                RfiFreqTunningOffsetIsNegative; ///< Specify RfiFreqTunningOffset is Positive or Negative.
  UINT8                  RfiFreqTunningOffset; ///< specify the Target FIVR Frequency offset.
  ///
  /// Calibrate 24MHz BCLK support; 0: NO_CALIBRATE, 1: PCODE_CALIBRATE, 2: BIOS_CALIBRATE (Default :1)
  ///
  UINT8                  PcodeCalibration;
  BOOLEAN                EnableRerunPcodeCalibration; ///< Calibrate C state 24MHz BCLK support.
  ///
  /// This structure is used to describe which of the Thermal functions should be enabled. For details of this
  /// structure, please see Related Definitions.
  ///
  THERM_FUNCTION_ENABLES *ThermalFuncEnables;
  CUSTOM_POWER_UNIT   CustomPowerUnit;      ///< Power Management Custom Power Limit Unit.

} POWER_MGMT_CONFIG;

#endif
