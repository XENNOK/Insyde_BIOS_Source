/** @file
  Protocol used for specifying platform related CPU information and policy setting.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
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

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _CPU_PLATFORM_POLICY_H_
#define _CPU_PLATFORM_POLICY_H_

#include "CpuPmConfig.h"
//#include <Protocol/MpService.h>
#include <Protocol/FrameworkMpService.h>
#include <Protocol/HiiDatabase.h>
#include <Guid/StatusCodeDataTypeId.h>

#include <Guid/DataHubRecords.h>

#include <OemDataHubSubClassProcessor.h>

#define DXE_CPU_PLATFORM_POLICY_PROTOCOL_GUID \
  { 0x1279e288, 0x24cd, 0x47e9, { 0x96, 0xba, 0xd7, 0xa3, 0x8c, 0x17, 0xbd, 0x64 } }

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///

///
/// Revision 1: Initial version
///
#define DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_1 1
///
/// Revision 2: Added CPU_CONFIG.DebugInterfaceEnable
///             Added CPU_CONFIG.DebugInterfaceLockEnable
///
#define DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_2 2
///
/// Revision 3: Added CPU_CONFIG.ApIdleManner
///             Added CPU_CONFIG.ApHandoffManner
///
#define DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_3 3

///
/// Revision 4: Added POWER_MGMT_CONFIG.THERM_FUNCTION_ENABLES.Pl1ThermalControl
///
#define DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_4 4
///
/// Revision 5: Added POWER_MGMT_CONFIG.PPM_TURBO_SETTINGS.PowerLimit3
///                   POWER_MGMT_CONFIG.PPM_TURBO_SETTINGS.PowerLimit3Time
///                   POWER_MGMT_CONFIG.PPM_TURBO_SETTINGS.PowerLimit3DutyCycle
///                   POWER_MGMT_CONFIG.PPM_TURBO_SETTINGS.PowerLimit3Lock
///
#define DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_5 5

///
/// Revision 6: Added POWER_MGMT_CONFIG.CustomPowerUnit .Support for user to configuration custom Power Limts in  milli watts
/// Default Power Unit for follwing Items changes to MilliWatts
/// POWER_MGMT_CONFIG.PPM_TURBO_SETTINGS
///                 PowerLimit1
///                 PowerLimit2
///                 PowerLimit3
/// POWER_MGMT_CONFIG.PPM_CUSTOM_CTDP
///                 CustomPowerLimit1
///                 CustomPowerLimit2
///
///

#define DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_6 6

///
/// Revision 7:  Added POWER_MGMT_CONFIG.THERM_FUNCTION_ENABLES.Pl1ThermalControlFloor. Changed definition of
/// POWER_MGMT_CONFIG.THERM_FUNCTION_ENABLES.Pl1ThermalControl from 1 (enable) to 1 (enable/manual)
/// POWER_MGMT_CONFIG.THERM_FUNCTION_ENABLES.Pl1ThermalControlFloor
/// 

#define DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_7 7


extern EFI_GUID gDxeCpuPlatformPolicyProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _DXE_CPU_PLATFORM_POLICY_PROTOCOL DXE_CPU_PLATFORM_POLICY_PROTOCOL;

//
// Prototypes for the Platform CPU Protocol
//

///
/// This function is for platform code to provide Microcode location since different BIOS has different flash layout.
/// Platform code need to provide a function for CPU code to call to get the Microcode location in flash or memory.
///
typedef
EFI_STATUS
(EFIAPI *PLATFORM_CPU_RETRIEVE_MICROCODE)(
  IN DXE_CPU_PLATFORM_POLICY_PROTOCOL *This,
  OUT UINT8                           **MicrocodeData
  );

typedef struct {
  UINT32 Package;
  UINT32 Die;
  UINT32 Core;
  UINT32 Thread;
} CPU_PHYSICAL_LOCATION;

///
/// The platform category, Server, Desktop and Mobile are defined.
///
typedef enum {
  CpuPlatformUnknown= 0,
  CpuPlatformDesktop,
  CpuPlatformMobile,
  CpuPlatformServer,
  CpuPlatformMax
} CPU_PLATFORM_CATEGORY;

///
/// The reason for changing the state of the processor Only applies to Disabling processors.
/// In future, we can add add/remove support
///
#define CPU_CAUSE_NOT_DISABLED      0x0000
#define CPU_CAUSE_INTERNAL_ERROR    0x0001
#define CPU_CAUSE_THERMAL_ERROR     0x0002
#define CPU_CAUSE_SELFTEST_FAILURE  0x0004
#define CPU_CAUSE_PREBOOT_TIMEOUT   0x0008
#define CPU_CAUSE_FAILED_TO_START   0x0010
#define CPU_CAUSE_CONFIG_ERROR      0x0020
#define CPU_CAUSE_USER_SELECTION    0x0080
#define CPU_CAUSE_BY_ASSOCIATION    0x0100
#define CPU_CAUSE_UNSPECIFIED       0x8000

typedef UINT32 CPU_STATE_CHANGE_CAUSE;

typedef struct {
  EFI_MP_PROC_CONTEXT                   *Context;
  EFI_EXP_BASE10_DATA                   CoreFreq;
  EFI_EXP_BASE10_DATA                   BusFreq;
  EFI_EXP_BASE2_DATA                    CacheSize[EFI_CACHE_LMAX];
  EFI_PROCESSOR_ID_DATA                 CpuId;
  EFI_PROCESSOR_MICROCODE_REVISION_DATA MuData;
  EFI_PROCESSOR_STATUS_DATA             Status;
} EFI_DETAILED_CPU_INFO;

///
/// Platform Specific Processor Information
///
typedef struct {
  UINT64                                 ApicID;                     ///< APIC ID
  STRING_REF                             ReferenceString;            ///< Reference String
//[-start-110825-IB03610346-modify]//
  EFI_PROCESSOR_SOCKET_TYPE_DATA_PLUS     SocketType;
//[-end-110825-IB03610346-modify]//
  EFI_PROCESSOR_SOCKET_NAME_DATA         SocketName;                 ///< Socket Name
  EFI_PROCESSOR_MAX_CORE_FREQUENCY_DATA  MaxCoreFrequency;           ///< Maximum Core Frequency
  EFI_PROCESSOR_MAX_FSB_FREQUENCY_DATA   MaxFsbFrequency;            ///< Maximum FSB Frequency
  EFI_PROCESSOR_CORE_FREQUENCY_LIST_DATA PlatformCoreFrequencyList;  ///< Platform Core Frequency List
  EFI_PROCESSOR_FSB_FREQUENCY_LIST_DATA  PlatformFsbFrequencyList;   ///< Platform FSB Frequency List
  STRING_REF                             AssetTag;                   ///< Asset Tag
  EFI_HII_HANDLE                         StringHandle;               ///< String Handle
  STRING_REF                             SerialNumber;               ///< Serial Number
  STRING_REF                             PartNumber;                 ///< Part Number
} PLATFORM_CPU_INFORMATION;

///
/// This interface is for platform to provide processor support layout, such as how many packages we want
/// processor code to support. If return EFI_UNSUPPORTED, processor code will assume MAXIMUM_CPU_NUMBER and
/// allocate MAX memory for all APs.
///
typedef
EFI_STATUS
(EFIAPI *PLATFORM_CPU_GET_MAX_COUNT)(
  IN DXE_CPU_PLATFORM_POLICY_PROTOCOL *This,
  OUT UINT32                          *MaxThreadsPerCore,
  OUT UINT32                          *MaxCoresPerDie,
  OUT UINT32                          *MaxDiesPerPackage,
  OUT UINT32                          *MaxPackages
  );

///
/// Platform code can provide platform specific processor information, such as processor socket Name on board,
/// processor Socket Type, and so on for SMBIOS table creation.
///
typedef
EFI_STATUS
(EFIAPI *PLATFORM_CPU_GET_CPU_INFO)(
  IN DXE_CPU_PLATFORM_POLICY_PROTOCOL *This,
  IN CPU_PHYSICAL_LOCATION            *Location,
  IN OUT PLATFORM_CPU_INFORMATION     *PlatformCpuInfo
  );

//
// Generic definitions for device enabling/disabling used by CPU code
//
#define CPU_FEATURE_ENABLE  1
#define CPU_FEATURE_DISABLE 0

//
// Generic definitions for DTS
//
#define DTS_OUT_OF_SPEC_ONLY      2
#define DTS_OUT_OF_SPEC_OCCURRED  3

///
/// General CPU feature Configuration for all processor features enabling bit definitions are in this field.
/// Platform code can enable/disable features thru this field.
///
typedef struct {
  //
  // Byte 0, bit definition for functionality enable/disable
  //
  UINT8 HtState                : 1; ///< Enable or Disable Hyper Threading State; 0: Disable; 1: Enable
  UINT8 LimitCpuidMaximumValue : 1; ///< Enable or Disable Limit Cpuid Maximum Value; 0: Disable; 1: Enable
  UINT8 DcaState               : 1; ///< @deprecated Deprecated for Client (Server specific)
  UINT8 ExecuteDisableBit      : 1; ///< Enable or Disable Execute Disable Bit; 0: Disable; 1: Enable
  UINT8 VmxEnable              : 1; ///< Enable or Disable VMX; 0: Disable; 1: Enable
  UINT8 SmxEnable              : 1; ///< Enable or Disable SMX; 0: Disable; 1: Enable
  UINT8 FastString             : 1; ///< @deprecated
  UINT8 MachineCheckEnable     : 1; ///< Enable or Disable Machine Check; 0: Disable; 1: Enable
  //
  // Byte 1, bit definition for functionality enable/disable
  //
  UINT8 MonitorMwaitEnable      : 1; ///< Enable or Disable Monitor Mwait mode; 0: Disable; 1: Enable
  UINT8 XapicEnable             : 1; ///< Enable or Disable Xapic mode; 0: Disable; 1: Enable
  UINT8 MachineCheckStatusClean : 1; ///< @deprecated
  UINT8 IsColdReset             : 1; ///< Check if is Cold Reset; 0: Not Cold Reset; 1: Cold Reset
  UINT8 MlcStreamerPrefetcher   : 1; ///< Enable or Disable MlcStreamerPrefetcher; 0: Disable; 1: Enable
  UINT8 MlcSpatialPrefetcher    : 1; ///< Enable or Disable MlcSpatialPrefetcher; 0: Disable; 1: Enable
  UINT8 EnableDts               : 2; ///< Enable or Disable DTS feature; 0=Disable; 1=Enable; 2=OUT_OF_SPEC;
  //
  // Byte 2, byte definition for addiional functionalities expected later
  //
  UINT8 FviReport                : 1; ///< Enable or Disable FVI report; 0: Disable; 1: Enable
  UINT8 AesEnable                : 1; ///< Enable or Disable AES feature; 0: Disable; 1: Enable
  UINT8 DebugInterfaceEnable     : 1; ///< Enable or Disable Debug Interface; This policy must be disabled for production BIOS. <b>0: Disable</b>; 1: Enable
  UINT8 DebugInterfaceLockEnable : 1; ///< Enable or Disable Debug Interface Lock; This policy must be enabled on production platforms. 0: Disable; <b>1: Enable</b>
  UINT8 ApIdleManner             : 2; ///< Settings for AP Threads Idle; 1: HALT 2:MWAIT 3:RUN
  UINT8 ApHandoffManner          : 2; ///< Settings for AP Handoff to OS; 1: HALT 2:MWAIT32
  //
  // CPU feature configuration
  //
  UINT8  BspSelection;                ///< Select BSP
  UINT32 DcaPrefetchDelayValue;       ///< @deprecated Deprecated for Client (Server specific)
  UINT8  VirtualWireMode;             ///< @deprecated
  UINT8  SmmbaseSwSmiNumber;          ///< SW SMI Number from Smbase.
  //
  // CPU Misc Config
  //
  UINT8 FviSmbiosType;                ///< Create SMBIOS Table Type for FVI
  //
  // Functions provided by platform code
  //
  ///
  /// Platform code can provide microcode location thru this function.
  ///
  PLATFORM_CPU_RETRIEVE_MICROCODE RetrieveMicrocode;
  ///
  /// Platform to provide the processor detail about Max Thread per Core, Max Cores per Die, Max Dies per
  /// Package and Max packages.
  ///
  PLATFORM_CPU_GET_MAX_COUNT      GetMaxCount;
  ///
  /// Platform code to provide platform specific processor information
  ///
  PLATFORM_CPU_GET_CPU_INFO       GetCpuInfo;

//[-start-130906-IB08620308-add]//
  //
  // Byte n, bit definition for functionality enable/disable
  //
  UINT8 DcuStreamerPrefetcher   : 1; ///< Enable or Disable DcuStreamerPrefetcher; 0: Disable; 1: Enable
  UINT8 DcuIPPrefetcher         : 1; ///< Enable or Disable DcuIPPrefetcher; 0: Disable; 1: Enable
//[-end-130906-IB08620308-add]//
} CPU_CONFIG;

///
/// TxT Platform Configuration
///
typedef struct {
  UINT8 ResetAux : 1;    ///< Reset Auxiliary content when it is set "TRUE"
  UINT8 Reserved : 7;    ///< Reserved for future use
  UINT8 ByetReserved[1]; ///< Reserved for future use
} TXT_FUNCTION_CONFIG;


///
/// All processor security features enabling definitions are in this field.
/// Platform code can enable/disable features thru this field.
///
typedef struct {
  TXT_FUNCTION_CONFIG *TxtFunctionConfig;
} SECURITY_CONFIG;

///
/// The CPU platform policy protocol allows the platform code to publish a set of configuration information that the
/// CPU drivers will use to configure the processor. Platform code needs to provide the information for processor
/// drivers to finish the initialization.
///
typedef struct _DXE_CPU_PLATFORM_POLICY_PROTOCOL {
  ///
  /// This member specifies the revision of the CPU Policy protocol. This field is used to indicate backwards
  /// compatible changes to the protocol. Platform code that produces this protocol must fill with the correct revision
  /// value for the PCH reference code to correctly interpret the content of the protocol fields.
  ///
  UINT8             Revision;
  ///
  /// Processor standard features configuration.
  ///
  CPU_CONFIG        *CpuConfig;
  ///
  /// Processor power management features configuration.
  ///
  POWER_MGMT_CONFIG *PowerMgmtConfig;
  ///
  /// Processor security features configuration.
  ///
  SECURITY_CONFIG   *SecurityConfig;
} DXE_CPU_PLATFORM_POLICY_PROTOCOL;

#endif
