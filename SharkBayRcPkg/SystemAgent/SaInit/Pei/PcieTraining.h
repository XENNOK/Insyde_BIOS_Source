/** @file

  Header file for PcieTraining Initialization Driver.

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

#ifndef _PCIETRAINING_H_
#define _PCIETRAINING_H_

#include "PciExpressInit.h"
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SystemAgent/
///===============================================
#include <SaAccess.h>

///
/// Data structures
///

typedef struct {
  PEG_PORT PegPort;
  UINT32   EndPointVendorIdDeviceId;
  BOOLEAN  LinkIsGen3Capable;
  UINT8    LaneList[SA_PEG_MAX_LANE];
  UINT8    LaneListLength;
  BOOLEAN  EnableMargin;
  BOOLEAN  SkipMargin;
  BOOLEAN  FoundUsablePreset;
} PORT_INFO;

typedef struct {
  INT8  Depth;
  UINT8 Step;
  UINT8 ReportedMargin;
  UINT8 DoubleMargin;
} JITTER_SETTING;

typedef struct {
  UINT8  Lane;
  UINT32 InitialDs0Dac;
  UINT32 InitialDs0Value;
  INT32  Ds0MarginOffset;
  UINT32 InitialDs1Dac;
  UINT32 InitialDs1Value;
  INT32  Ds1MarginOffset;
  INT32  MaxUpMargin;
  INT32  MaxDownMargin;
} VOC_STATE;

typedef enum {
  LaneLevelRxJitter,
  VocUp,
  VocDown
} MARGIN_TEST_TYPE;

#define JITTER_LENGTH                         25
#define JITTER_MARGIN_INITIAL_OFFSET          1
#define MARGIN_CONVERGANCE_ALLOWED_DELTA      100
#define MARGIN_CONVERGANCE_MIN_MATCHES        2
#define MARGIN_CONVERGANCE_MAX_REPEATS        30

#define SA_PEI_MONITOR_OFFSET                 0xFED85000

///
/// Register Definitions
///
#define B_SA_PEG_LTSSMC_WIDTH_MASK            0xFFFFFFE0

#define R_SA_PEG_REUT_PH_CTR_OFFSET           0x444
#define B_SA_PEG_REUT_PH_CTR_PHYRESET_MASK    0x1
#define B_SA_PEG_REUT_PH_CTR_RESETMOD_MASK    0x2
#define B_SA_PEG_REUT_PH_CTR_AUTOCOMP_MASK    0x2000

#define R_SA_PEG_REUT_PH1_PIS_OFFSET          0x464
#define B_SA_PEG_REUT_PH1_PIS_ST_MASK         0x3F
#define B_SA_PEG_REUT_PH1_PIS_ST_STEP         0x8

#define B_SA_PEG_BCTRL_SRESET_MASK            BIT6

#define V_SA_VCU_OPCODE_SET_TXJITTER_MUX      0x3002
#define V_SA_VCU_SEQID_SET_TXJITTER_MUX       0x00030003

#define R_SA_VCU_REUT_PH_CTR_ADDRESS_REV1     0x04448808
#define R_SA_VCU_REUT_PH_CTR_ADDRESS_REV2     0x04448080

#define R_SA_VCU_REUT_PH1_PIS_ADDRESS_REV1    0x04648808
#define R_SA_VCU_REUT_PH1_PIS_ADDRESS_REV2    0x04648080

///
/// Function Prototypes
///



UINT16
GetErrorTarget (
  IN  SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi
  );

VOID GetPortInfo (
  OUT PORT_INFO *PortInfoList,
  OUT UINT8     *PortInfoListLength,
  OUT BOOLEAN   *SkipBundle0
  );

EFI_STATUS
RunMarginTest (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN  SA_PLATFORM_POLICY_PPI  *SaPlatformPolicyPpi,
  IN  SA_DATA_HOB             *SaDataHob,
  IN       EFI_PEI_STALL_PPI  *StallPpi,
  IN  UINT32                  MonitorPort,
  IN  PORT_INFO               *PortInfoList,
  IN  UINT8                   PortInfoListLength,
  IN  MARGIN_TEST_TYPE        MarginTest,
  OUT INT32                   *Margins
  );

EFI_STATUS
LaneLevelJitterTest (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN  SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi,
  IN  SA_DATA_HOB            *SaDataHob,
  IN       EFI_PEI_STALL_PPI *StallPpi,
  IN  UINT32                 MonitorPort,
  IN  UINT8                  *LaneList,
  IN  UINT8                  LaneListLength,
  IN  PEG_PORT               *PegPort,
  IN  UINT8                  OriginalLinkSpeed,
  IN  UINT8                  OriginalLinkWidth,
  IN  BOOLEAN                TxJitterTest,
  OUT INT32                  *Margins
  );

UINT32
SaPciePointTest (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi,
  IN      EFI_PEI_STALL_PPI *StallPpi,
  IN UINT32                 MonitorPort,
  IN PEG_PORT               *PegPort,
  IN UINT32                 InitialRecoveryCount
  );

INT32
CalculateMarginDifference (
  IN INT32                   Margin1,
  IN INT32                   Margin2
  );

INT32
InterpolateMargin (
  IN UINT32 ErrorTarget,
  IN UINT32 CurrentErrorCount,
  IN UINT32 PreviousErrorCount,
  IN INT32  FailingPoint
  );


UINT32
NaturalLog (
  IN UINT32 Input
  );


VOID
PegGen3PresetSearch (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       SA_DATA_HOB                 *SaDataHob
  );

BOOLEAN
SaPolicyEnablesGen3 (
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi
  );


EFI_STATUS
EnsureLinkIsHealthy (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN SA_DATA_HOB            *SaDataHob,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort,
  IN       UINT8                       OriginalLinkSpeed,
  IN       UINT8                       OriginalLinkWidth
  );

EFI_STATUS
WaitForL0 (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort,
  IN       BOOLEAN                     UseVcu
  );

EFI_STATUS
TogglePegSlotReset (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  );

EFI_STATUS
AssertPegSlotReset (
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi
  );

EFI_STATUS
DeassertPegSlotReset (
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi
  );

EFI_STATUS
RecoverLinkFailure (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN SA_DATA_HOB            *SaDataHob,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN PEG_PORT               *PegPort,
  IN UINT8                  OriginalLinkSpeed,
  IN UINT8                  OriginalLinkWidth
  );

BOOLEAN
LinkIsDowngraded (
  IN PEG_PORT *PegPort,
  IN UINT8    OriginalLinkSpeed,
  IN UINT8    OriginalLinkWidth
  );

EFI_STATUS
SecondaryBusReset (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort
  );

EFI_STATUS
ResetPhyLayer (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort
  );

EFI_STATUS
RetrainLink (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                   *PegPort
  );

UINT8
GetNegotiatedWidth (
  IN PEG_PORT *PegPort
  );

EFI_STATUS
RecoverLinkWidth (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort,
  IN       UINT8                       OriginalLinkWidth
  );

UINT8
GetLinkSpeed (
  IN PEG_PORT *PegPort
  );

EFI_STATUS
RecoverLinkSpeed (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi,
  IN       PEG_PORT                    *PegPort,
  IN       UINT8                       OriginalLinkSpeed
  );

VOID
PcieTrainingWarmReset (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  );


EFI_STATUS
SetJitterTolerance (
  IN UINT8   *LaneList,
  IN UINT8   LaneListLength,
  IN UINT8   ReportedMargin
  );

EFI_STATUS
SetRawJitterTolerance (
  IN UINT8   *LaneList,
  IN UINT8   LaneListLength,
  IN UINT8   Step,
  IN UINT8   Depth,
  IN UINT8   DoubleMargin,
  IN BOOLEAN EnableJitter
  );

VOID
EnableTxJitterInjection (
  IN UINT8    Lane,
  IN BOOLEAN  EnableTxJitter
  );

VOID
ConfigureTxJitterMux (
  IN UINT8    Lane,
  IN UINT32   MchBar
  );

EFI_STATUS
GetBundleList (
  IN  UINT8 *LaneList,
  IN  UINT8 LaneListLength,
  OUT UINT8 *BundleList,
  OUT UINT8 *BundleListLength
  );


UINT32
OpenMonitor (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi,
  IN       EFI_PEI_STALL_PPI           *StallPpi
  );

VOID
CloseMonitor (
  IN SA_PLATFORM_POLICY_PPI *SaPlatformPolicyPpi,
  IN UINT32                  MonitorPort
  );

UINT32
SaPcieGetErrorCount (
  IN UINT32 MonitorPort,
  IN UINT8  PcieController
  );

VOID
SaPcieClearErrorCount (
  IN       UINT32                      MonitorPort,
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi
  );

VOID
InitMonitor (
  IN UINT32 MchBar,
  IN UINT32 GdxcBar
  );

VOID
TearDownMonitor (
  IN UINT32 MchBar,
  IN UINT32 GdxcBar
  );

UINT32
EnableMonitor (
  VOID
  );

VOID
DisableMonitor (
  VOID
  );

VOID
FullMonitorReset (
  IN UINT32 MonitorPort
  );

VOID
ProgramMonitor (
  IN       UINT32                      MonitorPort,
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_STALL_PPI           *StallPpi
  );


VOID
GetLinkPartnerFullSwing (
  IN  UINT8 Lane,
  OUT UINT8 *FullSwing
  );

VOID
GetCoefficientsFromPreset (
  IN  UINT8 Preset,
  IN  UINT8 FullSwing,
  OUT UINT8 *PreCursor,
  OUT UINT8 *Cursor,
  OUT UINT8 *PostCursor
  );

VOID
SetPartnerTxCoefficients (
  IN  UINT8 Lane,
  IN  UINT8 *PreCursor,
  IN  UINT8 *Cursor,
  IN  UINT8 *PostCursor
  );

#endif
