/** @file

  PciExpressInit header file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PCIEXPRESS_INIT_H_
#define _PCIEXPRESS_INIT_H_

///===============================================
///  MdePkg/Include/
///===============================================
#include <IndustryStandard/Pci.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PostCodeLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/HobLib.h>
#include <Ppi/Stall.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SystemAgent/
///===============================================
#include <SaAccess.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Guid/SaDataHob.h>
#include <Ppi/SaPlatformPolicy.h>


#define PEG_AUTO    0
#define PEG_GEN1    1
#define PEG_GEN2    2
#define PEG_GEN3    3

#define DMI_GEN1    1
#define DMI_GEN2    2

#define LANE_STEP   0x10
#define BUNDLE_STEP 0x20

typedef struct {
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  UINT8   Index;
  BOOLEAN PresenceDetect;
  UINT8   MaxLinkWidth;
  UINT8   EndpointMaxLinkSpeed;
} PEG_PORT;

///
/// Data structure used in Sampler Calibration
///
#pragma pack(1)
typedef struct _SAMPLE {
  UINT8   Data;
  UINT16  Count;
} DATA_SAMPLE;
#pragma pack()

#define MAX_CODES 10

///
/// Data structure used in Preset Search
///
typedef struct _PRESET_DATA {
  UINT8 Preset;
  UINTN TimingMargin[SA_PEG_MAX_BUNDLE];
  UINTN VoltageUpMargin[SA_PEG_MAX_BUNDLE];
  UINTN VoltageDownMargin[SA_PEG_MAX_BUNDLE];
} PRESET_DATA;

#define MAX_PRESETS 3

#define SA_PEG_SAMPLER_ITERATIONS  500
///
/// Common register access macros - use either DMIBAR or PEG10
///
#define SaMmio32AndThenOr(BaseAddr, Device, Register, AndData, OrData) \
  if (BaseAddr != 0) { \
    Mmio32AndThenOr (BaseAddr, Register, AndData, OrData); \
  } else { \
    MmPci32AndThenOr (0, 0, Device, 0, Register, AndData, OrData); \
  }

#ifdef PEG_FLAG
/**
  Bubble sort from DATA_SAMPLE

  @param [in, out]                Array[]: array of DATA_SAMPLE

  @retval None

**/
VOID
BubbleSort (
  IN OUT DATA_SAMPLE                   Array[]
  );

/**
  Get Middle Value from DATA_SAMPLE

  @param [in, out]                Array[]: array of DATA_SAMPLE

  @retval UINT32 : Middle Value of DATA_SAMPLE

**/
UINT32
GetMiddleValue (
  IN OUT DATA_SAMPLE                   Array[]
  );

/**
  Set Load Bus

  @param [in] DmiBar              DMIBAR address
  @param [in] Dev                 Device Number
  @param [in] Lane                Number of Lane
  @param [in] LoadSel             Load selection value
  @param [in] LoadData            Load Data
  @param [in] CpuSteppingId       CPUID.1.EAX[3:0], CPU stepping ID

  @retval None

**/
VOID
SetLoadBus (
  IN UINT32                            DmiBar,
  IN UINTN                             Dev,
  IN UINTN                             Lane,
  IN UINT32                            LoadSel,
  IN UINT32                            LoadData,
  IN UINT8                             CpuSteppingId
  );

/**
  Get monitor bus from the lane selected

  @param [in] DmiBar              DMIBAR address
  @param [in] Dev                 Device number
  @param [in] Lane                Number of Lane
  @param [in] LoadSel             Load selection value
  @param [in] LoadData            Load selecttion data
  @param [in] CpuSteppingId       CPUID.1.EAX[3:0], CPU stepping ID

  @retval UINT32 - Load bus address

**/
UINT32
GetMonBus (
  IN UINT32                            DmiBar,
  IN UINTN                             Dev,
  IN UINTN                             Lane,
  IN UINT32                            LoadSel,
  IN UINT8                             CpuSteppingId
  );

/**
  Dump Sampler Values

  @param [in] DmiBar              DMIBAR address
  @param [in] CpuSteppingId       CPUID.1.EAX[3:0], CPU stepping ID
  @param [in] Dev                 Device number
  @param [in] LanesCount          Value of Lanes

  @retval None

**/VOID
DumpSamplerValues (
  IN UINT32                            DmiBar,
  IN UINT8                             CpuSteppingId,
  IN UINTN                             Dev,
  IN UINTN                             LanesCount
  );
#endif /// PEG_FLAG

#if defined(DMI_FLAG) || defined(PEG_FLAG)
/**
  Perform PEG/DMI PCIe Recipe steps

  @param [in] SaPlatformPolicyPpi Pointer to SA_PLATFORM_POLICY_PPI
  @param [in] MchBar              MCHBAR or zero if called for PEG
  @param [in] DmiBar              DMIBAR or zero if called for PEG
  @param [in] Dev                 PEG device number: 1 for PEG10, 0 if called for DMI.
  @param [in] SwingControl        0 = Reduced, 1 = Half, 2 = Full

  @retval None

**/
VOID
PegDmiRecipe (
  IN SA_PLATFORM_POLICY_PPI            *SaPlatformPolicyPpi,
  IN UINT32                            MchBar,
  IN UINT32                            DmiBar,
  IN UINTN                             Dev,
  IN UINT8                             SwingControl
  );
#endif /// DMI_FLAG || PEG_FLAG

#ifdef PEG_FLAG


/**
  Configure PEG GenX mode

  @param [in] PeiServices         Pointer to the PEI services table
  @param [in] StallPpi            Pointer to PEI_STALL_PPI
  @param [in] SaPlatformPolicyPpi Pointer to SA_PLATFORM_POLICY_PPI
  @param [in] PegPortTable        Pointer to PEG_PORT array
  @param [in] TableIndex          Index in PEG_PORT array
  @param [in] CpuSteppingId       CPU stepping
  @param [in] Gen3Capable         Selected PEG_PORT is Gen3 capable

  @retval None

**/
VOID
ConfigurePegGenX (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN       PEG_PORT                    *PegPortTable,
  IN       UINTN                       TableIndex,
  IN       UINT8                       CpuSteppingId,
  IN       UINT8                       Gen3Capable
  );

/**
  Additional PEG Programming Steps at PEI

  @param [in] SaPlatformPolicyPpi pointer to SA_PLATFORM_POLICY_PPI
  @param [in] PegBus              Pci Bus Number
  @param [in] PegDev              Pci Device Number
  @param [in] PegFunc             Pci Func Number

  @retval None

**/
VOID
AdditionalPegProgramSteps (
  IN SA_PLATFORM_POLICY_PPI            *SaPlatformPolicyPpi,
  IN UINT8                             PegBus,
  IN UINT8                             PegDev,
  IN UINT8                             PegFunc
  );

/**
  Maximize the dedicated credits for the PEG controllers

**/
VOID
MaximizeSharedCredits (
  VOID
  );

/**
  Rebalance Credits when PEG controllers so that no starvation occurs

  @param [in] DisablePeg10        Peg10 disable/enable status
  @param [in] DisablePeg11        Peg11 disable/enable status
  @param [in] DisablePeg12        Peg12 disable/enable status

  @retval None

**/
VOID
RebalancePegPerformanceCredits (
  IN BOOLEAN                           DisablePeg10,
  IN BOOLEAN                           DisablePeg11,
  IN BOOLEAN                           DisablePeg12
  );



/**
  Additional PEG Programming Steps before PEG detection at PEI

  @param [in] PegBus              Pci Bus Number
  @param [in] PegDev              Pci Device Number
  @param [in] PegFunc             Pci Func Number
  @param [in] SaPlatformPolicyPpi Pointer to SA_PLATFORM_POLICY_PPI

**/
VOID
PegPreDetectionSteps (
  IN UINT8                             PegBus,
  IN UINT8                             PegDev,
  IN UINT8                             PegFunc,
  IN SA_PLATFORM_POLICY_PPI            *SaPlatformPolicyPpi
  );
#endif /// PEG_FLAG

#if defined(DMI_FLAG) || defined(PEG_FLAG)
/**
  Send one sequence to VCU MailBox

  @param [in] MchBar              MCHBAR value
  @param [in] Address             Target address
  @param [in] OpCode              OpCode number
  @param [in] WriteData           Data value (only used if OpCode is a write)

**/
UINT32
SendVcuApiSequence (
  IN UINT32                            MchBar,
  IN UINT32                            Address,
  IN UINT16                            OpCode,
  IN UINT32                            WriteData
  );

/**
  Send one command to VCU MailBox

  @param [in] MchBar              MCHBAR value
  @param [in] Interface           Interface number
  @param [in] Data                Data value

**/
VOID
SendVcuApiCmd (
  IN UINT32                            MchBar,
  IN UINT32                            Interface,
  IN UINT32                            Data
  );

/**
 This function reports a PEG controller's link status

  @param [in] PegBus              Peg Bus
  @param [in] PegDev              Peg Device
  @param [in] PegFunc             Peg Function

  @retval None

**/
VOID
ReportPcieLinkStatus (
  IN UINT8                             PegBus,
  IN UINT8                             PegDev,
  IN UINT8                             PegFunc
);

VOID
WaitForVc0Negotiation (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,

  IN UINT8             PegBus,
  IN UINT8             PegDev,
  IN UINT8             PegFunc
)
/**

  This function prints the time required for VC0 Negotiation Pending to be cleared. Quits after 100 msec.

  @param[in] PeiServices  - Pointer to the PEI services table
  @param[in] StallPpi     - Pointer to PEI_STALL_PPI
  @param[in] PegBus       - Peg Bus
  @param[in] PegDev       - Peg Device
  @param[in] PegFunc      - Peg Function

  @retval None

**/
;
/**
  Find the Offset to a given Capabilities ID
  CAPID list:
    0x01 = PCI Power Management Interface
    0x04 = Slot Identification
    0x05 = MSI Capability
    0x10 = PCI Express Capability

  @param [in] Bus                 Pci Bus Number
  @param [in] Device              Pci Device Number
  @param [in] PegFunc             Pci Function Number
  @param [in] CapId               CAPID to search for

  @retval 0       - CAPID not found
  @retval Other   - CAPID found, Offset of desired CAPID

**/
UINT32
PcieFindCapId (
  IN UINT8                             Bus,
  IN UINT8                             Device,
  IN UINT8                             PegFunc,
  IN UINT8                             CapId
  );

/**
  Program PEG Gen3 preset value

  @param [in] Port                0 = Root Port, 1 = End Point
  @param [in] PresetValue         Preset value to program
  @param [in] PegFunc             Peg function number to be configured
  @param [in] BundleIndex         Bundle to be configured

  @retval None
**/
VOID
ProgramPreset (
  IN UINT8  Direction,
  IN UINT8  PresetValue,
  IN UINT8  PegFunc,
  IN UINT8  Lane
  );

#endif /// DMI_FLAG || PEG_FLAG

#ifdef PEG_FLAG
UINT8
GetMaxBundles (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN UINT8                     PegFunc,
  IN UINT8                     HwStrap
  )
/**
  GetMaxBundles: Get the maximum bundle numbers for the corresponding PEG

  @param[in] PeiServices          - Pointer to the PEI services table
  @param[in] PegFunc              - Points to PEG0/PEG1/PEG2/...
  @param[in] HwStrap              - Points to PEG configuration information [x16_x0_x0/x8_x8_x0/x8_x4_x4/...]

  @retval                         - MaxBndlPwrdnCount [Maximun number of bundles for this HW configuration]
**/
;

VOID
PowerDownUnusedBundles (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN UINT8                     PegFunc,
  IN UINT8                     HwStrap,
  IN UINT8                     BndlPwrdnCount
  )
/**
  PowerDownUnusedBundles: Program the PEG BundleSpare registers for power on sequence [PowerOff unused bundles for PEGs]

  @param[in] PeiServices          - Pointer to the PEI services table
  @param[in] PegFunc              - Points to PEG0/PEG1/PEG2/...
  @param[in] HwStrap              - Points to PEG configuration information [x16_x0_x0/x8_x8_x0/x8_x4_x4/...]
  @param[in] BndlPwrdnCount       - Points to how many bundles are unused and should be powered down

  @retval                         - None
**/
;
#endif // PEG_FLAG

#endif
