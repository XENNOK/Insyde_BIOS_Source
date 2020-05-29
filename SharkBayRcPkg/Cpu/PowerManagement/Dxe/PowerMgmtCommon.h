/** @file
  This header file contains power management definitions specific to
  Haswell processors.

  Acronyms:
    PPM - Processor Power Management
    TM    Thermal Monitor
    IST   Intel(R) Speedstep technology
    HT    Hyper-Threading Technology

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
#ifndef _POWER_MGMT_COMMON_H_
#define _POWER_MGMT_COMMON_H_

typedef UINT16                    STRING_REF;
#include <PowerMgmtDefinitions.h>
//[-start-130523-IB05160449-add]//
#include <Library/BaseLib/BaseLibInternals.h>
//[-end-130523-IB05160449-add]//
#include <Cpu.h>
#include <CpuAccess.h>
#include <PchAccess.h>
#include <SaAccess.h>
#include <EfiScriptLib.h>
#include <IndustryStandard/Acpi30.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/PchPlatformLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AslUpdateLib.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/BootScriptSave.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/CpuPlatformPolicy.h>
#include <Protocol/PowerMgmtInitDone.h>
#include <Protocol/PpmGlobalNvsArea.h>
#include <Guid/PowerMgmtAcpiTableStorage.h>

//
// Integer division with rounding to the nearest rather than truncating.
// For example 8/3=2 but IDIV_ROUND(8,3)=3. 1/3=0 and IDIV_ROUND(1,3)=0.
// A half is rounded up e.g., IDIV_ROUND(1,2)=1 but 1/2=0.
//
#define IDIV_ROUND(r, s)  ((r) / (s) + (((2 * ((r) % (s))) < (s)) ? 0 : 1))



#define HSW_ULT_PCH_POWER_LEVELS        0x3
#define EXTENDED_PCH_POWER_LEVELS       0x4
#define PM_CST_LVL2                     0x14

#define FADT_C3_LATENCY                 57
#define FADT_C3_LATENCY_DISABLED        1001

#define NATIVE_PSTATE_LATENCY           10
#define PSTATE_BM_LATENCY               10

#define MP_TIMEOUT_FOR_STARTUP_ALL_APS  0 ///< Set 0 for BSP always wait for APs

#define NO_CALIBRATE                       0
#define PCODE_CALIBRATE                    1
#define BIOS_CALIBRATE                     2
#define PCODE_BCLK_CALIBRATION_TIMEOUT     22

///
/// Limit the number of P-states to 16. Up to Windows 7, the OS allocates 1KB buffer for the PSS package.
/// So the maximum number of P-state OS can handle is 19. This is not an OS issue. Having too many P-states
/// is not good for the system performance.
///
#define FVID_MAX_STATES                 16
#define FVID_MIN_STEP_SIZE              1

///
/// Cpu Brandstring length
///
#define MAXIMUM_CPU_BRAND_STRING_LENGTH       48

///
/// Power Limit Level
///
#define PL12TimeWindowCovert            1
#define PL3TimeWindowConvert            3
//
// Global variables
//
///
/// CpuPlatformPolicy Revision
///
extern UINT8 mCpuPolicyRevision;

extern POWER_MGMT_CONFIG  *mCpuPmConfig;       ///< Power Managment policy configurations
extern EFI_CPUID_REGISTER mCpuid01;            // CPUID 01 values
//
// Values for FVID table calculate.
//
extern UINT16 mTurboBusRatio;
extern UINT16 mMaxBusRatio;
extern UINT16 mMinBusRatio;
extern UINT16 mProcessorFlavor;
extern UINT16 mBspBootRatio;
extern UINT16 mPackageTdp;
extern UINT16 mPackageTdpWatt;
extern UINT16 mCpuConfigTdpBootRatio;
extern UINT16 mCustomPowerUnit;
extern UINT16 mCpuCacheSize;
///
/// Fractional part of Processor Power Unit in Watts. (i.e. Unit is 1/mProcessorPowerUnit)
///
extern UINT8 mProcessorPowerUnit;
///
/// Fractional part of Processor Time Unit in seconds. (i.e Unit is 1/mProcessorTimeUnit)
///
extern UINT8 mProcessorTimeUnit;
///
/// Maximum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
/// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
///
extern UINT16 mPackageMaxPower;
///
/// Minimum allowed power limit value in TURBO_POWER_LIMIT_MSR and PRIMARY_PLANE_POWER_LIMIT_MSR
/// in units specified by PACKAGE_POWER_SKU_UNIT_MSR
///
extern UINT16 mPackageMinPower;
extern UINT8  mControllableTdpEnable;                        ///< Controllable TDP Enable/Disable
extern UINT8  mRatioLimitProgrammble;                        ///< Porgrammable Ratio Limit
extern UINT8  mTdpLimitProgrammble;                          ///< Porgrammable TDP Limit

extern PPM_GLOBAL_NVS_AREA_PROTOCOL *mPpmGlobalNvsAreaProtocol; ///< Ppm GlobalNvs Protocol
extern FRAMEWORK_EFI_MP_SERVICES_PROTOCOL     *mMpService;                ///< Mp Services Protocol

//
// Globals to support updating ACPI Tables
//
extern EFI_ACPI_SUPPORT_PROTOCOL   *mAcpiSupport;
extern EFI_ACPI_TABLE_PROTOCOL     *mAcpiTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpu0IstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mApIstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpu0CstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mApCstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpuPmTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCpu0TstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mApTstTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mLakeTinyTable;
extern EFI_ACPI_DESCRIPTION_HEADER *mCtdpTable;

//
// Structure Declarations
//

typedef struct _FVID_HEADER {
  UINT32 Stepping;    ///< Matches value returned by CPUID function 1
  UINT16 MaxBusRatio; ///< Matches BUS_RATIO_MAX field in PERF_STS_MSR
  UINT16 Gv3States;   ///< Number of states of FVID (N)
} FVID_HEADER;

typedef struct _FVID_STATE {
  UINT32 State;    ///< State Number (0 - N-1)
  UINT16 BusRatio; ///< BUS_RATIO_SEL value to be written to PERF_CTL
  UINT32 Power;    ///< Typical power consumed by CPU in this state
} FVID_STATE;

typedef union _FVID_TABLE {
  FVID_HEADER FvidHeader;
  FVID_STATE  FvidState;
  UINT64      FvidData;
} FVID_TABLE;

typedef struct _ENABLE_CSTATE_PARAMS {
  UINT16 C3IoAddress;
} ENABLE_CSTATE_PARAMS;

typedef struct _ENABLE_EMTTM_PARAMS {
  FVID_TABLE *FvidPointer;
} ENABLE_EMTTM_PARAMS;

typedef struct _PCODE_BCLK_CALIBRATION_MAILBOX {
  UINT32 PCalFactor;
  UINT32 TSC24_L1;
  UINT32 TSC24_L2;
  UINT32 TSC24_U1;
  UINT32 TSC24_U2;
  UINT32 TSC100_L1;
  UINT32 TSC100_L2;
  UINT32 TSC100_U1;
  UINT32 TSC100_U2;
} PCODE_BCLK_CALIBRATION_MAILBOX;

///
/// ASL SSDT structure layout
///
#pragma pack(1)
typedef struct {
  UINT8  NameOp;          ///< First opcode is a NameOp.
  UINT32 NameString;      ///< 'TDSS'   ; Name of object.
  UINT8  PackageOp;       ///< db 12h   ; Sixth OpCode is PackageOp.
  UINT16 PackageLen;      ///< dw 0h    ; Seventh/Eighth OpCode is PackageLength.
  UINT8  PackageEntryNum; ///< db 0Ch   ; Ninth OpCode is number of package entries.
  UINT8  StringPrefix1;   ///< 0Dh
  UINT64 Cpu0IstStr;      ///< 00h
  UINT8  StringNull1;     ///< 00h
  UINT8  DwordPrefix1a;   ///< 0Ch
  UINT32 Cpu0IstAddr;     ///< 00h
  UINT8  DwordPrefix1b;   ///< 0Ch
  UINT32 Cpu0IstLen;      ///< 00h
  UINT8  StringPrefix2;   ///< 0Dh
  UINT64 Cpu1IstStr;      ///< 00h
  UINT8  StringNull2;     ///< 00h
  UINT8  DwordPrefix2a;   ///< 0Ch
  UINT32 ApIstAddr;       ///< 00h
  UINT8  DwordPrefix2b;   ///< 0Ch
  UINT32 ApIstLen;        ///< 00h
  UINT8  StringPrefix3;   ///< 0Dh
  UINT64 Cpu0CstStr;      ///< 00h
  UINT8  StringNull3;     ///< 00h
  UINT8  DwordPrefix3a;   ///< 0Ch
  UINT32 Cpu0CstAddr;     ///< 00h
  UINT8  DwordPrefix3b;   ///< 0Ch
  UINT32 Cpu0CstLen;      ///< 00h
  UINT8  StringPrefix4;   ///< 0Dh
  UINT64 ApCstStr;        ///< 00h
  UINT8  StringNull4;     ///< 00h
  UINT8  DwordPrefix4a;   ///< 0Ch
  UINT32 ApCstAddr;       ///< 00h
  UINT8  DwordPrefix4b;   ///< 0Ch
  UINT32 ApCstLen;        ///< 00h
} SSDT_LAYOUT;
#pragma pack()

///
/// ASL PCTP structure layout
///
#pragma pack(1)
typedef struct {
  UINT8  RegDes;      ///<  Byte [0]=0x82:Register descriptor code
  UINT16 RegLen;      ///<  Byte [2:1]=0x0C:Register descriptor length
  UINT8  RegType;     ///<  Byte [3]=0x01:Register type (System IO)
  UINT8  RegWidth;    ///<  Byte [4]=0x10:Register width (16-bit)
  UINT8  RegBitOffst; ///<  Byte [5]=0x00:Register bit offset (0)
  UINT8  RegAccSize;  ///<  Byte [6]=0x00:Register access size (0)
  UINT64 RegAddress;  ///<  Byte [14:7]=Register address
  UINT16 EndTag;      ///<  Byte [16:15]=End tag
} PCTP_LAYOUT;
#pragma pack()

typedef struct {
  UINT16 SkuPackageTdp;
  UINTN  SkuIccMax;
  UINTN  MsrCtdpPowerLimit1;
  UINTN  MsrCtdpPowerLimit2;
  UINTN  CtdpUpPowerLimit1;
  UINTN  CtdpUpPowerLimit2;
  UINTN  CtdpNominalPowerLimit1;
  UINTN  CtdpNominalPowerLimit2;
  UINTN  CtdpDownPowerLimit1;
  UINTN  CtdpDownPowerLimit2;
  UINTN  Reserved;
} PPM_CTDP_OVERRIDE_TABLE;

///
/// PL1 Thermal Control structure layout
///
typedef union {
  struct {
   UINT8 Disable:1;      ///< [0] Disable
   UINT8 Reserved:7;     ///< [7:2] Reserved
   UINT8 FloorIA;        ///< [15:8] Percent Throttle for IA component 255-0; 255=0%, 0=100%
   UINT8 FloorGT;        ///< [23:16] Percent Throttle for GT component 255-0; 255=0%, 0=100%
   UINT8 FloorPCH;       ///< [31:24] Percent Throttle for PCH component 255-0; 255=0%, 0=100%
  } Bits;
  UINT32 Uint32;
 } PL1_THERMAL_CONTROL;

typedef struct _PPM_CTRL_TDP_SKU_TBL{
  UINT8  CpuFamily[MAXIMUM_CPU_BRAND_STRING_LENGTH];       ///< Cpu Family Brand String
  UINTN  NoOfCpus;            ///< Number of Cpus,FF mean all cpus matching CpuFamily string above
  UINT8  CpuModel[20][MAXIMUM_CPU_BRAND_STRING_LENGTH];    ///< Processor Model Number
} PPM_CTRL_TDP_SKU_TBL;

//
// FVID Table Information
// Default FVID table
// One header field plus states
//
extern UINT16     mNumberOfStates;
extern FVID_TABLE mEmptyFvidTable[FVID_MAX_STATES + 1];
extern FVID_TABLE *mFvidPointer;

//
// Function prototypes
//
/**
  Initializes P States and Turbo Power management features
**/
VOID
InitializePStates (
  VOID
  );

/**
  Initializes XE support in the processor.

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitTurboRatioLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Initializes required structures for P-State table creation and enables GV3
  support in the processor.

  @param[in] FvidPointer  Table to update, must be initialized.
  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitGv3 (
  IN OUT FVID_TABLE        *FvidPointer,
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Enables GV3 support in a logical processor.

  This function must be MP safe.

  @param[in] Buffer  Pointer to the function parameters passed in.

  @retval EFI_SUCCESS
**/
VOID
EFIAPI
ApSafeEnableGv3 (
  IN OUT VOID *Buffer
  );

/**
  This function updates the table provided with the FVID data for the processor.
  If CreateDefaultTable is TRUE, a minimam FVID table will be provided.
  The maximum number of states must be greater then or equal to two.
  The table should be initialized in such a way as for the caller to determine if the
  table was updated successfully.  This function should be deprecated in the future when
  Release 8 is integrated in favor of the EIST protocol calculating FVID information.

  @param[in] FvidPointer         Pointer to a table to be updated
  @param[in] MaxNumberOfStates   Number of entries in the table pointed to by FvidPointer
  @param[in] MinStepSize         Minimum step size for generating the FVID table
  @param[in] CreateDefaultTable  Create default FVID table rather then full state support
**/
VOID
InitFvidTable (
  IN OUT FVID_TABLE *FvidPointer,
  IN UINT16         MaxNumberOfStates,
  IN UINT16         MinStepSize,
  IN BOOLEAN        CreateDefaultTable
  );

/**
  Create an FVID table based on the algorithm provided by the BIOS writer's guide.

  @param[in] FvidPointer         Pointer to a table to be updated
  @param[in] MaxNumberOfStates   Number of entries in the table pointed to by FvidPointer

  @retval EFI_SUCCESS            FVID table created successfully.
  @retval EFI_INVALID_PARAMETER  The bus ratio range don't permit FVID table calculation;
                                 a default FVID table should be constructed.
**/
EFI_STATUS
CreateFvidTable (
  IN OUT FVID_TABLE *FvidPointer,
  IN UINT16         MaxNumberOfStates
  );

/**
  Create default FVID table with max and min states only.

  @param[in] FvidPointer  Pointer to a table to be updated
**/
VOID
CreateDefaultFvidTable (
  IN OUT FVID_TABLE *FvidPointer
  );

/**
  Initializes Energy efficient P-state feature.

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitEnergyEfficientPState (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );


/**
  Configures the Interrupt Redirection Mode Selection for Logical Interrupts.

  @param[in] CpuPmConfig  Pointer to PPM Policy structure.
**/
VOID
InitPpmIrmConfiguration (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Completes platform power management initialization
    (1) Initializes the TSC update variables.
    (2) Initializes the GV state for processors.
    (3) Saves MSR state for S3
    (4) Adds a callback (SMI) in S3 resume script to restore the MSR
**/
VOID
PpmPostInit (
  VOID
  );

/**
  Set processor P state to HFM or LFM
**/
VOID
SetBootPState (
  VOID
  );

/**
  Set processor P state to HFM or LFM.

  @param[in] Buffer    Unused

  @retval EFI_SUCCESS  Processor MSR setting is saved.
**/
VOID
EFIAPI
ApSafeSetBootPState (
  IN OUT VOID *Buffer
  );

/**
  Initializes C States Power management features
**/
VOID
InitializeCState (
  IN POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Disable/Enable the CState Pre-Wake Feature

  @param[in] CpuPmConfig  - Pointer to policy protocol instance
**/
VOID
InitCstatePreWake (
  IN POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  BootScript for PCode Mailbox function for mailbox write commands.
  This function will poll the mailbox interface for control, issue the command
  during s3 resume

  @param[IN]  MailboxCommand,
  @param[IN]  MailboxData,
**/
VOID
MailboxS3Write (
  IN  UINT32  MailboxCommand,
  IN  UINT32  MailboxData
  );

/**
  Calibrate 24MHz BCLK support to reduce the power consumption in idle states.

  @retval    EFI_UNSUPPORTED   Unrecognized 24MHz BCLK Calibration Type.
  @retval    EFI_SUCCESS       Processor C-State 24MHz BCLK support calibrated successfully.
**/
EFI_STATUS
CalibrateBclkForCStates (
  VOID
  );

/**
  Enables C-State support as specified by the input flags on all logical
  processors and sets associated timing requirements in the chipset.

  @param[in] This          Pointer to the protocol instance
  @param[in] C3IoAddress   IO address to generate C3 states (PM base + 014 usually)
**/
VOID
EnableCStates (
  IN UINT16 C3IoAddress
  );

/**
  Enable C-State support as specified by the input flags on a logical processor.
    Configure BIOS C1 Coordination (SMI coordination)
    Enable IO redirection coordination
    Choose proper coordination method
    Configure extended C-States

  This function must be MP safe.

  @param[in] Buffer        Pointer to a ENABLE_CSTATE_PARAMS containing the necessary
                           information to enable C-States

  @retval EFI_SUCCESS   Processor C-State support configured successfully.
**/
VOID
EFIAPI
ApSafeEnableCStates (
  IN OUT VOID *Buffer
  );

/**
  This will perform Miscellaneous Power Management related programming.

  @param[in] CtdpSupport   Status of InitializeConfigurableTdp funtion
**/
VOID
InitMiscFeatures (
  EFI_STATUS CtdpSupport
  );

/**
  CTDP BIOS settings Initialization(Msr)

  @param[in] CpuPmConfig  Pointer to policy protocol instance
  @param[in] FvidPointer  Table to update, must be initialized.

  @exception EFI_UNSUPPORTED Ctdp not supported
  @retval EFI_SUCCESS     Ctdp Settings Initialized successfully from MSRs
**/
EFI_STATUS
InitializeConfigurableTdp (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Configures PowerLimits and Config TDP values

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigureCtdp (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  This will perform general thermal initialization other then TM1, TM2, or
  PROCHOT# on all logical processors.

  @param[in] This         Pointer to the protocol instance
  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitThermal (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  This will perform enable thermal initialization. TM1, TM2 and adaptive thermal
  throttling are enabled/disabled together.

  This function must be MP safe.

  @param[in] Buffer    Pointer to the function parameters passed in.

  @retval EFI_SUCCESS  General thermal initialization completed successfully
**/
VOID
EFIAPI
ApSafeInitThermal (
  IN OUT VOID *Buffer
  );

/**
  Enables the bi-directional PROCHOT# signal.

  @retval EFI_SUCCESS  PROCHOT# configured successfully
**/
EFI_STATUS
EnableProcHot (
  VOID
  );


/**
  Locks down all settings.

  @param[in] CpuPmConfig  Pointer to PPM Policy structure.
**/
VOID
PpmLockDown (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Lock MSR_PMG_CST_CONFIG
  This function must be MP safe.

  @param[in] Buffer    Not used (needed for API compatibility)

  @retval EFI_SUCCESS  Processor C-State locked successfully.
**/
VOID
EFIAPI
ApSafeLockDown (
  IN OUT VOID *Buffer
  );

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
  );

/**
  Configures following fields of MSR 0x610 based on user configuration:
    Configures Long duration Turbo Mode (power limit 1) power level and time window
    Configures Short duration Turbo mode (power limit 2)

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigurePowerLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Configures following fields of MSR 0x610 based on user configuration:
    Configures Long duration Turbo Mode (power limit 1) power level and time window
    Configures Short duration Turbo mode (power limit 2)

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigurePowerLimitsNonConfigTdpSkus (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Configures following fields of PL3 MSR 0x615 based on user configuration:
    Configures PL 3 power level and time window   
  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigurePL3PowerLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  This will perform PowerLimit 1 algorithm will be used to control Thermal Throttling features

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/

/**
  This will perform PowerLimit 1 algorithm will be used to control Thermal Throttling features

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
InitPl1ThermalControl (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Configures following fields of MSR 0x618 based on user configuration:
    Configures Long duration Turbo Mode (power limit 1) power level and time window for DDR domain
    Configures Short duration Turbo mode (power limit 2)

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigureDdrPowerLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Verify and fix Custom Power Limit values

  @param[in] CustomPowerLimit  Custom Power Limit value

  @retval Verified Custom power limit value
**/
UINT16
VerifyAndFixCustomPowerLimit (
  IN UINT32 CustomPowerLimit,
  IN UINT16 CustomPlUnit
  );

/**
  Verify and fix Custom Ratio values
  Custom Ratio should be between MaxTurboFrequency and LFM

  @param[in] CustomPowerLimit  Custom Power Limit value
  @param[in] CustomPlUnit  Custom Power Limit Unit

  @retval Verified Custom Ratio value
**/
UINT8
VerifyAndFixCustomRatio (
  IN UINT8 CustomRatio
  );

/**
  CTDP BIOS settings Initialization(From Msrs)

  @param[in] CpuPmConfig      Pointer to policy protocol instance

  @exception EFI_UNSUPPORTED  Ctdp not supported
  @retval EFI_SUCCESS         Ctdp Settings Initialized successfully from MSRs
**/
EFI_STATUS
InitConfigurableTdpSettings (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Get Power Limit2 based on Power Limit1 on Config TDP

  @param[in] PowerLimit1  Power Limit 1 Value

  @retval Calculated Power Limit2 value
**/
UINT16
GetCtdpPowerLimit2 (
  IN UINT16 PowerLimit1
  );

/**
  Custom Configurable TDP Table BIOS Initialization

  @param[in] CpuPmConfig      Pointer to policy protocol instance

  @exception EFI_UNSUPPORTED  Custom Ctdp settings are not available
  @retval EFI_SUCCESS         Successfully Initialized Custom Ctdp Settings
**/
EFI_STATUS
InitCustomConfigurableTdp (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Configures following fields of MSR 0x610 based on user configuration:
     - Configures Long duration Turbo Mode (power limit 1) power level and time window
     - Configures Short duration turbo mode (power limit 2)

  @param[in] CpuPmConfig     Pointer to policy protocol instance
**/
VOID
InitPchPowerSharing (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

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
  );

/**
  Configures following fields of MSR 0x610
    Configures Long duration Turbo Mode (power limit 1) power level and time window
    Configures Short duration turbo mode (power limit 2)

  @param[in] CpuPmConfig  Pointer to policy protocol instance
**/
VOID
ConfigureCtdpPowerLimits (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );

/**
  Patch Fvid Table with Ctdp Tar ratio and Tar-1 Ratio

  @param[in] FvidPointer  Pointer to Fvid Table
**/
VOID
CtdpPatchFvidTable (
  IN OUT FVID_TABLE *FvidPointer
  );

/**
  Replace P state with given ratio

  @param[in] FvidPointer  Pointer to Fvid Table
  @param[in] PssIndex     FVID table index of P state to be replaced
  @param[in] Ratio        Target Ratio to put in
**/
VOID
CtdpReplaceFvidRatio (
  IN OUT FVID_TABLE *FvidPointer,
  UINTN             PssIndex,
  UINTN             Ratio
  );

/**
  Patch the native _PSS package with the GV3 values
  Uses ratio/VID values from the FVID table to fix up the control values in the _PSS.

  (1) Find _PSS package:
    (1.1) Find the _PR_CPU0 scope.
    (1.2) Save a pointer to the package length.
    (1.3) Find the _PSS AML name object.
  (2) Resize the _PSS package.
  (3) Fix up the _PSS package entries
    (3.1) Check Turbo mode support.
    (3.2) Check Dynamic FSB support.
  (4) Fix up the Processor block and \_PR_CPU0 Scope length.
  (5) Update SSDT Header with new length.

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND - If _PR_.CPU0 scope is not foud in the ACPI tables
**/
EFI_STATUS
AcpiPatchPss (
  VOID
  );

/**
  Configure the FACP for C state support
**/
VOID
ConfigureFadtCStates (
  VOID
  );

/**
  Locate the PPM ACPI tables data file and read ACPI SSDT tables.
  Publish the appropriate SSDT based on current configuration and capabilities.

  @param[in] This                Pointer to the protocol instance

  @retval EFI_SUCCESS - on success
  @retval Appropiate failure code on error
**/
EFI_STATUS
InitializePpmAcpiTable (
  VOID
  );

/**
  Check for Ctrl TDP enabled SKUs and enable Controllable TDP

  @param[in] CpuPmConfig      Pointer to policy protocol instance

  @exception EFI_UNSUPPORTED   Controllable TDP not Supported
  @retval EFI_SUCCESS         Controllable TDP Supported
**/
EFI_STATUS
InitControllableTdp (
  IN OUT POWER_MGMT_CONFIG *CpuPmConfig
  );
#endif
