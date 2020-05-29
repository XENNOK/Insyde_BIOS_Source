/** @file
  This file includes all the data structures that the MRC considers "global data".

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved.
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

#ifndef _MRC_GLOBAL_H_
#define _MRC_GLOBAL_H_

#pragma pack (push, 1)
#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcOemDebugPrint.h>
#include <MrcRmtData.h>
#include <MrcSpdData.h>
#include <McAddress.h>

///
///***************************************************
/// Structures common to all "global data" elements.
///***************************************************
///
typedef U8 MrcIteration;
#define MRC_ITERATION_MAX ((1 << ((sizeof (MrcIteration) * 8) - 1)) + ((1 << ((sizeof (MrcIteration) * 8) - 1)) - 1))

#define MRC_NUMBER_UPM_PWR_RETRAIN_MARGINS (9)

typedef struct {
  ///
  ///< Thermal Options
  ///
  U8  EnableExtts;
  U8  EnableCltm;
  U8  EnableOltm;
  U8  EnablePwrDn;
#ifdef ULT_FLAG
  U8  EnablePwrDnLpddr;
#endif // ULT_FLAG
  U8  Refresh2X;
  U8  LpddrThermalSensor;
  U8  LockPTMregs;
  U8  UserPowerWeightsEn;

  U8  EnergyScaleFact;
  U8  RaplPwrFl[MAX_CHANNEL];

  U8  RaplLim2Lock;
  U8  RaplLim2WindX;
  U8  RaplLim2WindY;
  U8  RaplLim2Ena;
  U16 RaplLim2Pwr;
  U8  RaplLim1WindX;
  U8  RaplLim1WindY;
  U8  RaplLim1Ena;
  U16 RaplLim1Pwr;

  U8  WarmThreshold[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8  HotThreshold[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8  WarmBudget[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8  HotBudget[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];

  U8  IdleEnergy[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8  PdEnergy[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8  ActEnergy[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8  RdEnergy[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8  WrEnergy[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8  SrefCfgEna;
  U16 SrefCfgIdleTmr;
  U8  ThrtCkeMinDefeat;
  U8  ThrtCkeMinTmr;
#ifdef ULT_FLAG
  U8  ThrtCkeMinDefeatLpddr;
  U8  ThrtCkeMinTmrLpddr;
#endif // ULT_FLAG
} ThermalMngmtEn;


typedef struct {
  MrcBool GdxcEnable;   ///< GDXC  MOT enable
  U8      GdxcIotSize;  ///< IOT size in multiples of 8MEG
  U8      GdxcMotSize;  ///< MOT size in multiples of 8MEG
} MrcGdxc;

typedef struct {
  U32 ECT : 1;        ///< BIT0 - Early Command Training
  U32 SOT : 1;        ///< BIT1 - Sense Amp Offset Training
  U32 RDMPRT : 1;     ///< BIT2 - Read MPR Training
  U32 RCVET : 1;      ///< BIT3 - Read Leveling Training (RcvEn)
  U32 JWRL : 1;       ///< BIT4 - Jedec Write Leveling
  U32 FWRL : 1;       ///< BIT5 - Functional Write Leveling
  U32 WRTC1D : 1;     ///< BIT6 - Write Timing Centerin 1D
  U32 RDTC1D : 1;     ///< BIT7 - Read Timing Centering 1D
  U32 DIMMODTT : 1;   ///< BIT8 - Dimm ODT Training
  U32 WRDST : 1;      ///< BIT9 - Write Drive Strength Training
  U32 WREQT : 1;      ///< BIT10 - Write Equalization Training
  U32 RCVENC1D: 1;    ///< BIT11 - Receive Enable Centering 1D
  U32 RDODTT : 1;     ///< BIT12 - Read ODT Training
  U32 RDEQT : 1;      ///< BIT13 - Read Equalization Training
  U32 RDAPT : 1;      ///< BIT14 - Read Amplifier Power Training
  U32 WRTC2D : 1;     ///< BIT15 - Write Timing Centerin 2D
  U32 RDTC2D : 1;     ///< BIT16 - Read Timing Centering 2D
  U32 CMDVC : 1;      ///< BIT17 - Command Voltage Centering
  U32 WRVC2D : 1;     ///< BIT18 - Write Voltage Centering 2D
  U32 RDVC2D : 1;     ///< BIT19 - Read Voltage Centering 2D
  U32 RMC : 1;        ///< BIT20 - Retrain Margin Check
  U32 : 1;            ///< BIT21 -
  U32 LCT : 1;        ///< BIT22 - Late Command Training
  U32 RTL : 1;        ///< BIT23 - Round Trip latency
  U32 TAT : 1;        ///< BIT24 - Turn Around Timing
  U32 RMT : 1;        ///< BIT25 - RMT Tool
  U32 MEMTST : 1;     ///< BIT26 - Memory Test
  U32 DIMMODTT1D : 1; ///< BIT27 - DIMMODTT1d
  U32 WRSRT : 1;      ///< BIT28 - Write Slew Rate Training
  U32 DIMMRONT : 1;   ///< BIT29 - Dimm Ron Training
  U32 ALIASCHK: 1;    ///< BIT30 - SPD Alias Check
} TrainingStepsEn;

typedef enum {
  MrcModeFull,
  MrcModeMini
} MrcMode;

typedef enum {
  LastRxV,
  LastRxT,
  LastTxV,
  LastTxT,
  LastRcvEna,
  LastWrLevel,
  LastCmdT,
  LastCmdV,
  MAX_RESULT_TYPE
} MrcMarginResult;

///
/// Define the MRC recommended boot modes.
///
typedef enum {
  bmCold, ///< Cold boot
  bmWarm, ///< Warm boot
  bmS3,   ///< S3 resume
  bmFast, ///< Fast boot
} MrcBootMode;

///
/// DIMM SPD Security Status
///
typedef enum {
  MrcSpdStatusGood,      ///< Memory is in a secure state.
  MrcSpdStatusAliased,   ///< Memory is aliased.

  MrcSpdStatusLast       ///< Must be last in the list
} MrcSpdStatus;

///
/// Define the virtual channel.
///
typedef enum {
  vcA,  ///< Virtual channel A
  vcB,  ///< Virtual channel B
} MrcVirtualChannel;

///
/// Define the board types.
///
typedef enum {
  btCRBMB,    ///< 0 - CRB Mobile
  btCRBDT,    ///< 1 - CRB Desktop
  btUser1,    ///< 2 - SV mobile
  btUser2,    ///< 3 - SV desktop
  btUser3,    ///< 4 - SV server?
  btUser4,    ///< 5 - Ult
  btCRBEMB,   ///< 6 - CRB Embedded
  btUnknown,  ///< 7 - Unknown
} MrcBoardType;

///
/// Define the CPU family/model.
///
typedef enum {
  cmHSW     = 0x306C0,                  ///< Haswell
  cmHSW_ULT = 0x40650,                  ///< Haswell-ULT
  cmCRW     = 0x40660,                  ///< Crystalwell
  cmBDW     = 0x306D0                   ///< Broadwell
} MrcCpuModel;

///
/// Define the CPU stepping number.
///
typedef enum {
  ///
  /// Haswell
  ///
  csHswA0       = 1,
  csHswB0       = 2,
  csHswC0       = 3,
  csHswLast     = csHswC0,

  ///
  /// Crystalwell
  ///
  csCrwB0       = 0,
  csCrwC0       = 1,
  csCrwLast     = csCrwC0,

  ///
  /// Haswell-ULT
  ///
  csHswUltB0    = 0,
  csHswUltC0    = 1,
  csHswUltLast  = csHswUltC0,

  ///
  /// Broadwell
  ///
  csBdwA0       = 0,
  csBdwLast     = csBdwA0
} MrcCpuStepping;

typedef enum {
  CONTROLLER_NOT_PRESENT, ///< There is no controller present in the system.
  CONTROLLER_DISABLED,    ///< There is a controller present but it is disabled.
  CONTROLLER_PRESENT      ///< There is a controller present and it is enabled.
} MrcControllerSts;

typedef enum {
  CHANNEL_NOT_PRESENT,    ///< There is no channel present on the controller.
  CHANNEL_DISABLED,       ///< There is a channel present but it is disabled.
  CHANNEL_PRESENT         ///< There is a channel present and it is enabled.
} MrcChannelSts;

typedef enum {
  DIMM_ENABLED,           ///< DIMM/rank Pair is enabled, presence TBD
  DIMM_DISABLED,          ///< DIMM/rank Pair is disabled, regardless of presence.
  DIMM_PRESENT,           ///< There is a DIMM present in the slot/rank pair and it will be used.
  DIMM_NOT_PRESENT        ///< There is no DIMM present in the slot/rank pair.
} MrcDimmSts;

typedef enum {
  STD_PROFILE,            ///< Standard DIMM profile select.
  USER_PROFILE,           ///< User specifies various override values.
  XMP_PROFILE1,           ///< XMP enthusiast settings select (XMP profile #1).
  XMP_PROFILE2,           ///< XMP extreme settings select (XMP profile #2).
  MAX_PROFILE             ///< Delimiter
} MrcProfile;

typedef enum {
  MRC_REF_CLOCK_133,
  MRC_REF_CLOCK_100,
  MRC_REF_CLOCK_MAXIMUM   ///< Delimiter
} MrcRefClkSelect;        ///< This value times the MrcClockRatio determines the MrcFrequency.

typedef U32 MrcBClkRef;

typedef enum {
  MRC_DDR_TYPE_UNKNOWN,
  MRC_DDR_TYPE_DDR3,
  MRC_DDR_TYPE_LPDDR3
} MrcDdrType;

typedef enum {
  MRC_MODULE_TYPE_UNKNOWN,
  MRC_MODULE_TYPE_RDIMM,
  MRC_MODULE_TYPE_UDIMM,
  MRC_MODULE_TYPE_SODIMM,
  MRC_MODULE_MICRO_DIMM,
  MRC_MODULE_MINI_RDIMM,
  MRC_MODULE_MINI_UDIMM,
  MRC_MODULE_MINI_CDIMM,
  MRC_MODULE_72B_SO_UDIMM,
  MRC_MODULE_72B_SO_RDIMM,
  MRC_MODULE_72B_SO_CDIMM,
  MRC_MODULE_LRDIMM,
  MRC_MODULE_16B_SO_DIMM,
  MRC_MODULE_32B_SO_DIMM
} MrcModuleType;

typedef enum {
  MrcIterationClock = 0,
  MrcIterationCmdN  = 1,
  MrcIterationCmdS  = 2,
  MrcIterationCke   = 3,
  MrcIterationCtl   = 4,
  MrcIterationCmdV  = 5,
  MrcIterationMax
} MrcIterationType;

typedef enum {
  UpmLimit,
  PowerLimit,
  RetrainLimit,
  MarginLimitMax
} MRC_MARGIN_LIMIT_TYPE;

typedef U8  MrcClockRatio;  ///< This value times the MrcRefClkSelect determines the MrcFrequency.
typedef U32 MrcGfxDataSize; ///< The size of the stolen graphics data memory, in MBytes.
typedef U32 MrcGfxGttSize;  ///< The size of the graphics translation table, in MBytes.

///
/// UPM PWR and Retrain Limits
///
typedef struct {
  U8  Param;
  U16 ParamLimit[MarginLimitMax];
} MrcUpmPwrRetrainLimits;

typedef union {
  MrcUpmPwrRetrainLimits  *Pointer;
  U64                     Data;
} MrcUPRLimitPtr;

typedef union {
  U64 Data;
  U32 Data32[2];
} MrcCapId;                 ///< The memory controller capabilities.

///
/// MRC version description.
///
typedef struct {
  U8  Major;  ///< Major version number
  U8  Minor;  ///< Minor version number
  U8  Rev;    ///< Revision number
  U8  Build;  ///< Build number
} MrcVersion;

///
/// Memory map configuration information.
///
typedef struct {
  U32     TomMinusMe;
  U32     ToludBase;
  U32     BdsmBase;
  U32     GttBase;
  U32     GraphicsControlRegister;
  U32     TsegBase;
  MrcBool ReclaimEnable;
  U32     RemapBase;
  U32     RemapLimit;
  U32     TouudBase;
  U32     TotalPhysicalMemorySize;
  U32     MeStolenBase;
  U32     MeStolenSize;
  U32     GdxcMotBase;
  U32     GdxcMotSize;
  U32     GdxcIotBase;
  U32     GdxcIotSize;
  U32     DprSize;
  U32     FtpmStolenBase;
} MrcMemoryMap;

///
/// Real time clock information.
///
typedef struct {
  U8  Seconds;
  U8  Minutes;
  U8  Hours;
  U8  DayOfMonth;
  U8  Month;
  U16 Year;
} MrcBaseTime;

///
/// DIMM timings
///
typedef struct {
  U32 tCK;    ///< Memory cycle time, in femtoseconds.
  U16 NMode;  ///< Number of tCK cycles for the channel DIMM's command rate mode.
  U16 tCL;    ///< Number of tCK cycles for the channel DIMM's CAS latency.
  U16 tCWL;   ///< Number of tCK cycles for the channel DIMM's minimum CAS write latency time.
  U16 tFAW;   ///< Number of tCK cycles for the channel DIMM's minimum four activate window delay time.
  U16 tRAS;   ///< Number of tCK cycles for the channel DIMM's minimum active to precharge delay time.
  U16 tRC;    ///< Number of tCK cycles for the channel DIMM's minimum active to active/refresh delay time.
  U16 tRCD;   ///< Number of tCK cycles for the channel DIMM's minimum RAS# to CAS# delay time.
  U16 tREFI;  ///< Number of tCK cycles for the channel DIMM's minimum Average Periodic Refresh Interval.
  U16 tRFC;   ///< Number of tCK cycles for the channel DIMM's minimum refresh recovery delay time.
  U16 tRFC2;  ///< Number of tCK cycles for the channel DIMM's minimum refresh recovery delay time.
  U16 tRFC4;  ///< Number of tCK cycles for the channel DIMM's minimum refresh recovery delay time.
  U16 tRP;    ///< Number of tCK cycles for the channel DIMM's minimum row precharge delay time.
  U16 tRPab;  ///< Number of tCK cycles for the channel DIMM's minimum row precharge delay time for all banks.
  U16 tRRD;   ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time.
  U16 tRRD_L; ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time for same bank groups.
  U16 tRRD_S; ///< Number of tCK cycles for the channel DIMM's minimum row active to row active delay time for different bank groups.
  U16 tRTP;   ///< Number of tCK cycles for the channel DIMM's minimum internal read to precharge command delay time.
  U16 tWR;    ///< Number of tCK cycles for the channel DIMM's minimum write recovery time.
  U16 tWTR;   ///< Number of tCK cycles for the channel DIMM's minimum internal write to read command delay time.
} MrcTiming;

typedef struct {
  S32 Mtb;    ///< Medium time base.
  S32 Ftb;    ///< Fine time base.
} MrcTimeBase;

typedef struct {
  U8  Left;   ///< The left side of the timing eye.
  U8  Center; ///< The center of the timing eye.
  U8  Right;  ///< The right side of the timing eye.
} MrcDqTimeMargin;

typedef struct {
  U8  High;   ///< The high side of the Vref eye.
  U8  Center; ///< The center of the Vref eye.
  U8  Low;    ///< The low side of the Vref eye.
} MrcDqVrefMargin;

typedef struct {
  U8  Left;   ///< The left side of the command eye.
  U8  Right;  ///< The right side of the command eye.
  U8  High;   ///< The high side of the command eye.
  U8  Low;    ///< The low side of the command eye.
} MrcCommandMargin;

typedef struct {
  U8  Left;   ///< The left side of the receive enable eye.
  U8  Right;  ///< The right side of the receive enableeye.
} MrcRecvEnMargin;

typedef struct {
  U8  Left;   ///< The left side of the write leveling eye.
  U8  Right;  ///< The right side of the write leveling eye.
} MrcWrLevelMargin;

#ifdef SSA_FLAG
typedef struct {
  U8   *BufBase;
  U32  BufLimit;
  union {
    struct {
      U8  Occupied : 1;
      U8  HeapEnd  : 1;
    } Bits;
    U8 Data;
  } BufFlags;
} HeapBufHeader;
#endif // SSA_FLAG

#if ((defined SSA_FLAG) || (defined MRC_DEBUG_PRINT))
typedef union {
    struct {
      U8  Init : 1; ///< 1 is SSA heap initialized.
      U8       : 7;
    } Bits;
    U8 Data;
  } SsaHeapFlagType;
#endif

///
///*****************************************
/// Output related "global data" structures.
///*****************************************
///
/// This data structure contains all the "global data" values that are considered output by the MRC.
/// The following are SDRAM level definitions. All ranks on a rank are set to these values.
///
/* Commented out until needed, in order to save space.
typedef struct {
} MrcSdramOut;
*/

///
/// This data structure contains all the "global data" values that are considered output by the MRC.
/// The following are rank level definitions. All ranks on a DIMM are set to these values.
///
typedef struct {
//MrcSdramOut     Sdram[MAX_SDRAM_IN_DIMM];
  U16             MR[MAX_MR_IN_DIMM]; ///< DRAM mode register value.
#ifdef ULT_FLAG
  U16             MR11; ///< LPDDR3 ODT MR
#endif
} MrcRankOut;

///
/// This data structure contains all the "global data" values that are considered output by the MRC.
/// The following are DIMM level definitions. All ranks on a DIMM are set to these values.
///
typedef struct {
  MrcDimmSts    Status;                 ///< See MrcDimmSts for the definition of this field.
  MrcTiming     Timing[MAX_PROFILE];    ///< The DIMMs timing values.
  MrcVddSelect  VddVoltage[MAX_PROFILE];///< The voltage (VDD) setting for this DIMM, per profile.
  MrcBool       EccSupport;             ///< TRUE if ECC is enabled and supported on this DIMM.
  MrcBool       IgnoreNonEccDimm;       ///< TRUE if a DIMM without ECC capability should be ignored.
  MrcBool       AddressMirrored;        ///< TRUE if the DIMM is address mirrored.
  MrcBool       SelfRefreshTemp;        ///< TRUE if the DIMM supports self refresh extended operating temperature range (SRT).
  MrcBool       AutoSelfRefresh;        ///< TRUE if the DIMM supports automatic self refresh (ASR).
  MrcBool       PartialSelfRefresh;     ///< TRUE if the DIMM supports Partial Array Self Refresh (PASR).
  MrcBool       OnDieThermalSensor;     ///< TRUE if the DIMM supports On-die Thermal Sensor (ODTS) Readout.
  MrcBool       ExtendedTemperRange;    ///< TRUE if the DIMM supports Extended Temperature Range (ETR).
  MrcBool       ExtendedTemperRefresh;  ///< TRUE if the DIMM supports 1x Extended Temperature Refresh rate, FALSE = 2x.
  MrcDdrType    DdrType;                ///< DDR type: DDR3 or LPDDR3
  MrcModuleType ModuleType;             ///< Module type: UDIMM, SO-DIMM, etc.
  U32           SdramCount;             ///< The number of SDRAM components on a DIMM.
  U32           DimmCapacity;           ///< DIMM size in MBytes.
  U32           RowSize;                ///< The DIMMs row address size.
  U16           ColumnSize;             ///< The DIMMs column address size.
  U16           Crc;                    ///< Calculated CRC16 of the DIMM's provided SPD. Can be used to detect DIMM change.
  U8            RankInDIMM;             ///< The number of ranks in this DIMM.
  U8            Banks;                  ///< Number of banks the DIMM contains.
  U8            BankGroups;             ///< Number of bank groups the DIMM contains.
  U8            PrimaryBusWidth;        ///< DIMM primary bus width.
  U8            SdramWidth;             ///< DIMM SDRAM width.
  U8            SdramWidthIndex;        ///< DIMM SDRAM width index (0 = x4, 1 = x8, 2 = x16, 3 = x32).
  U8            DensityIndex;           ///< Total SDRAM capacity index (0 = 256Mb, 1 = 512Mb, 2 = 1Gb, etc).
  U8            ReferenceRawCard;       ///< Indicates which JEDEC reference design raw card was used as the basis for the module assembly.
  U8            XmpSupport;             ///< Indicates if XMP profiles are supported. 0 = None, 1 = XMP1 only, 2 = XMP2 only, 3 = All.
  U8            XmpRevision;            ///< Indicates the XMP revision of this DIMM. 0 = None, 12h = 1.2, 13h = 1.3.
  MrcRankOut    Rank[MAX_RANK_IN_DIMM];
} MrcDimmOut;

///
/// This data structure contains all the "global data" values that are considered output by the MRC.
/// The following are channel level definitions. All DIMMs on a memory channel are set to these values.
///
typedef struct {
  MrcChannelSts     Status;                            ///< Indicates whether this channel should be used.
  MrcVirtualChannel VirtualChannel;                    ///< define the virtual channel type A or B.
  MrcTiming         Timing[MAX_PROFILE];               ///< The channel timing values.
  MrcTimeBase       TimeBase[MAX_DIMMS_IN_CHANNEL][MAX_PROFILE];
  MrcBool           EccSupport;                        ///< TRUE if ECC is enabled and supported on this channel.
  U32               Capacity;                          ///< Amount of memory in this channel, in MBytes.
  U32               DimmCount;                         ///< Number of valid DIMMs that exist in the channel.
  U32               DataOffsetTrain[MAX_SDRAM_IN_DIMM];///< DataOffsetTrain CR
  U32               DataCompOffset[MAX_SDRAM_IN_DIMM]; ///< DataCompOffset CR
  DDRDATA0CH0_CR_DDRCRDATACONTROL0_STRUCT DqControl0;                    ///< DqControl0 CR
  DDRDATA0CH0_CR_DDRCRDATACONTROL1_STRUCT DqControl1[MAX_SDRAM_IN_DIMM]; ///< DqControl1 CR
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT DqControl2[MAX_SDRAM_IN_DIMM]; ///< DqControl2 CR
  U32               CkeCmdPiCode[2];                   ///< CKE  CmdPiCode CR, per group
  U32               CmdsCmdPiCode[2];                  ///< CmdS CmdPiCode CR, per group
  U32               CmdnCmdPiCode[2];                  ///< CmdN CmdPiCode CR, per group
  U32               MchbarBANK;                        ///< tRCD tRP tRAS tRDPRE (tRTP) tWRPRE and tRRD values.
  U32               MchbarBANKRANKA;                   ///< Mchbar TC Read to Read Turnaround CR
  U32               MchbarBANKRANKB;                   ///< Mchbar TC Write to x Turnaround CR
  U32               MchbarBANKRANKC;                   ///< Mchbar TC Read to Write Turnaround CR
  U32               MchbarBANKRANKD;                   ///< Mchbar TC Write /Read Duration
  U32               TxXtalk[MAX_SDRAM_IN_DIMM];        ///< TxXtalk Setting
  U16               TxDqs[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM]; ///< TxDQS PI Code
  U16               TxDq[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];  ///< TxDQ Pi Code
  U16               RcvEn[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM]; ///< RcvEn PI Code
  U16               WlDelay[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];
  U8                ClkPiCode[MAX_RANK_IN_CHANNEL];    ///< CLK ClkPiCode
  U8                CtlPiCode[MAX_RANK_IN_CHANNEL];    ///< CTL CtlPiCode
  U8                CkePiCode[MAX_RANK_IN_CHANNEL];    ///< CKE CtlPiCode
  U8                TxEq[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];  ///< TxEq Setting
  MrcCommandMargin  Command[MAX_RANK_IN_CHANNEL];
  MrcDqTimeMargin   RxDqPb[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS]; ///< Rx PerBit Pi Code
  MrcDqTimeMargin   TxDqPb[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS]; ///< Tx PerBit Pi Code
  MrcDqVrefMargin   RxDqVrefPb[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS]; ///< Rx PerBit Vref
  MrcDqVrefMargin   TxDqVrefPb[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS]; ///< Rx PerBit Vref
  MrcRecvEnMargin   ReceiveEnable[MAX_RANK_IN_CHANNEL]; ///< Receive enable per rank
  MrcWrLevelMargin  WriteLevel[MAX_RANK_IN_CHANNEL];   ///< Write leveling per rank
  U8                IoLatency[MAX_RANK_IN_CHANNEL];    ///< IOLatency
  U8                RTLatency[MAX_RANK_IN_CHANNEL];    ///< RoundTripLatency
  U32               RTIoComp;                          ///< RoundTrip IO Compensation of the Channel
  U8                RxVref[MAX_SDRAM_IN_DIMM];         ///< RX Vref in steps of 7.9 mv
  U8                RxEq[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];  ///< RxEQ Setting
  U8                RxDqsP[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];/// RxDQSP PI Code
  U8                RxDqsN[MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];/// RxDQSN PI Code
  U8                RankInChannel;                     ///< Number of valid ranks that exist in the channel.
  U8                ValidRankBitMask;                  ///< Bit map of the populated ranks per channel
  U8                ValidCkeBitMask;                   ///< Bit map of the used CKE pins per channel
  MrcDimmOut        Dimm[MAX_DIMMS_IN_CHANNEL];
} MrcChannelOut;

///
/// This data structure contains all the "global data" values that are considered output by the MRC.
/// The following are memory controller level definitions. All channels on a controller are set to these values.
///
typedef struct {
  MrcControllerSts  Status;               ///< Indicates whether this controller should be used.
  U16               DeviceId;             ///< The PCI device id of this memory controller.
  U8                RevisionId;           ///< The PCI revision id of this memory controller.
  U8                ChannelCount;         ///< Number of valid channels that exist on the controller.
  MrcChannelOut     Channel[MAX_CHANNEL]; ///< The following are channel level definitions.
} MrcControllerOut;

///
/// This data structure contains all the "DDR power saving data" values that are considered output by the MRC.
/// The following are memory controller level definitions. All channels on a controller are set to these values.
///
typedef struct {
  BOOL              BaseFlag;         ///< Indicates if the base line of power was already calculated.
  U16               BaseSavingRd;     ///< Indicates the base line of power consume by the ddr on read.
  U16               BaseSavingWr;     ///< Indicates the base line of power consume by the ddr on write.
  U16               BaseSavingCmd;    ///< Indicates the base line of power consume by the ddr on command.
  U16               MrcSavingRd;      ///< Indicates the power consume by the ddr on read at the end of MRC.
  U16               MrcSavingWr;      ///< Indicates the power consume by the ddr on write at the end of MRC.
  U16               MrcSavingCmd;     ///< Indicates the power consume by the ddr on command at the end of MRC.
} MrcOdtPowerSaving;

///
/// This data structure contains all the "global data" values that are considered output by the MRC.
/// The following are system level definitions. All memory controllers in the system are set to these values.
///
typedef struct {
  MrcVersion              Version;                  ///< The memory reference code version.
  MrcFrequency            FreqMax;                  ///< The requested maximum valid frequency.
  MrcFrequency            Frequency;                ///< The system's common memory controller frequency.
  U32                     MemoryClockMax;           ///< The system's common memory controller maximum clock, in femtoseconds.
  U32                     MemoryClock;              ///< The system's common memory controller clock, in femtoseconds.
  MrcRefClkSelect         RefClk;                   ///< The memory controller is going to use this reference clock.
  MrcClockRatio           Ratio;                    ///< Request for this memory controller to use this clock ratio.
  MrcBootMode             BootMode;                 ///< The system's common memory controller boot mode.
  MrcMemoryMap            MemoryMapData;            ///< The system's memory map data.
  MrcGfxDataSize          GraphicsStolenSize;       ///< Graphics Data Stolen Memory size in MB
  MrcGfxGttSize           GraphicsGttSize;          ///< GTT graphics stolen memory size in MB
  MrcVddSelect            VddVoltage[MAX_PROFILE];  ///< The currently running voltage (VDD) setting for all DIMMs in the system, per profile.
  MrcGdxc                 Gdxc;                     ///< GDXC enable and size.
  MrcBool                 VddVoltageDone;           ///< To determine if VddVoltageDone update has been done already
  MrcBool                 EccSupport;               ///< TRUE if ECC is enabled and supported on this controller.
  MrcBool                 EnDumRd;                  ///< Enable/Disable Logic Analizer
  MrcBool                 RestoreMRs;               ///< Enable/Disable restoring
  MrcBool                 AsyncOdtDis;              ///< Enable Asyncronous ODT
  MrcBool                 LpddrEctDone;             ///< Set to TRUE once Early Command Training on LPDDR is done, and we can run JEDEC Init
  MrcBool                 LpddrJedecInitDone;       ///< Set to TRUE once JEDEC Init on LPDDR is done
  MrcBool                 XmpProfileEnable;         ///< XMP capable DIMMs detected in system (0 = no, 1 = yes).
  MrcBool                 Capable100;               ///< The MC is capable of 100 reference clock (0 = no, 1 = yes).
  MrcBool                 AutoSelfRefresh;          ///< Indicates ASR is supported for all the DIMMS for 2xRefresh
  MrcDdrType              DdrType;                  ///< Current memory type: DDR3 or LPDDR3
  MrcSpdStatus            SpdSecurityStatus;        ///< Status variable to inform BIOS that memory contains an alias.
  U32                     MrcTotalChannelLimit;     ///< The maximum allowed memory size per channel, in MBytes.
  U8                      SdramCount;               ///< The number of SDRAM components on a DIMM.
  U32                     CompCtl0;                 ///< CompCtl0 CR
  U32                     CompCtl1;                 ///< CompCtl1 CR
  U32                     DimmVref;                 ///< DimmVref CR
  U32                     MiscControl0;             ///< MiscCOntrol0 CR
  U16                     Qclkps;                   ///< Qclk period in pS
  U8                      DQPat;                    ///< Global Variables storing the current DQPat REUT Test
  S8                      DQPatLC;                  ///< Global Variables storing the current DQPat Loopcount
  U8                      ValidRankMask;            ///< Rank bit map - includes both channels
  U8                      ValidChBitMask;           ///< Channel bit map of the populated channels
  MrcUPRLimitPtr          UpmPwrRetrainLimits;      ///< Pointer to Global UPM/PWR/RETRAIN Limits on the stack the size of MRC_NUMBER_UPM_PWR_RETRAIN_MARGINS
  U32                     MarginResult[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES];  ///< Stores last margin measurement.
  MrcControllerOut        Controller[MAX_CONTROLLERS];
  MrcOdtPowerSaving       OdtPowerSavingData;
} MrcOutput;

///
///****************************************
/// Input related "global data" structures.
///****************************************
///
/// This data structure contains all the "global data" values that are considered input by the MRC.
/// The following are SDRAM level definitions. All ranks on a rank are set to these values.
///
/* Commented out until needed, in order to save space.
typedef struct {
  U8  Placeholder;  ///< TODO: Is there anything that needs to go in here?
} MrcSdramIn;
*/

///
/// This data structure contains all the "global data" values that are considered input by the MRC.
/// The following are rank level definitions. All ranks on a DIMM are set to these values.
///
/* Commented out until needed, in order to save space.
typedef struct {
  MrcSdramIn  Sdram[MAX_SDRAM_IN_DIMM];
} MrcRankIn;
*/

///
/// This data structure contains all the "global data" values that are considered input by the MRC.
/// The following are DIMM level definitions. All ranks on a DIMM are set to these values.
///
typedef struct {
  MrcDimmSts  Status; ///< Indicates whether this DIMM should be used.
  U8          SpdValid[sizeof (MrcSpd) / (CHAR_BITS * sizeof (U8))]; ///< Each valid bit maps to SPD byte.
  MrcSpd      Spd;        ///< The SPD data for each DIMM. SPDGeneral field = 0 when absent.
  MrcTiming   Timing;     ///< The DIMMs requested timing overrides.
  U8          SpdAddress; ///< The SMBus address for the DIMM's SPD data.
//MrcRankIn   Rank[MAX_RANK_IN_DIMM];
} MrcDimmIn;

///
/// This data structure contains all the "global data" values that are considered input by the MRC.
/// The following are channel level definitions. All DIMMs on a memory channel are set to these values.
///
typedef struct {
  MrcChannelSts Status;     ///< Indicates whether this channel should be used.
  U32           DimmCount;  ///< The maximum number of DIMMs on this channel.
  MrcDimmIn     Dimm[MAX_DIMMS_IN_CHANNEL];
#ifdef ULT_FLAG
  U8            DqsMapCpu2Dram[8];            ///< Mapping from CPU DQS pins to SDRAM DQS pins
  U8            DqMapCpu2Dram[8][MAX_BITS];   ///< Mapping from CPU DQ pins to SDRAM DQ pins
  U8            DQByteMap[MrcIterationMax][2];  ///< Maps which PI clocks are used by what LPDDR DQ Bytes (from CPU side), per group
                            ///< DQByteMap[0] - ClkDQByteMap:
                            ///<   If clock is per rank, program to [0xFF, 0xFF]
                            ///<   If clock is shared by 2 ranks, program to [0xFF, 0] or [0, 0xFF]
                            ///<   If clock is shared by 2 ranks but does not go to all bytes,
                            ///<           Entry[i] defines which DQ bytes Group i services
                            ///< DQByteMap[1] - CmdNDQByteMap: Entry[0] is CmdN/CAA and Entry[1] is CmdN/CAB
                            ///< DQByteMap[2] - CmdSDQByteMap: Entry[0] is CmdS/CAA and Entry[1] is CmdS/CAB
                            ///< DQByteMap[3] - CkeDQByteMap : Entry[0] is CKE /CAA and Entry[1] is CKE /CAB
                            ///<                For DDR, DQByteMap[3:1] = [0xFF, 0]
                            ///< DQByteMap[4] - CtlDQByteMap : Always program to [0xFF, 0] since we have 1 CTL / rank
                            ///<                               Variable only exists to make the code easier to use
                            ///< DQByteMap[5] - CmdVDQByteMap: Always program to [0xFF, 0] since we have 1 CA Vref
                            ///<                               Variable only exists to make the code easier to use
#endif // ULT_FLAG
} MrcChannelIn;

///
/// This data structure contains all the "global data" values that are considered input by the MRC.
/// The following are memory controller level definitions. All channels on a controller are set to these values.
///
typedef struct {
  MrcControllerSts  Status;               ///< Indicates whether this controller should be used.
  U8                ChannelCount;         ///< Number of valid channels that are requested on the controller.
  MrcChannelIn      Channel[MAX_CHANNEL]; ///< The following are channel level definitions.
} MrcControllerIn;

///
/// This data structure contains all the "global data" values that are considered input by the MRC.
/// The following are system level definitions. All memory controllers in the system are set to these values.
///
typedef struct {
  MrcDebug        Debug;
  MrcFrequency    FreqMax;                    ///< The requested maximum valid frequency.
  MrcClockRatio   Ratio;                      ///< Request for this memory controller to use this clock ratio.
  MrcRefClkSelect RefClk;                     ///< Request for this memory controller to use this reference clock.
  MrcBClkRef      BClkFrequency;              ///< Base reference clock value, in Hertz.
  MrcBoardType    BoardType;                  ///< define the board type (CRBMB,CRBDT,User1,User2). the OEM can add more boards.
  MrcCpuStepping  CpuStepping;                ///< define the CPU stepping.
  MrcCpuModel     CpuModel;                   ///< define the CPU model.
  MrcGfxDataSize  GraphicsStolenSize;         ///< Graphics Data Stolen Memory size in MB
  MrcGfxGttSize   GraphicsGttSize;            ///< GTT graphics stolen memory size in MB
  MrcBaseTime     BaseTime;                   ///< RTC base time.
  MrcIteration    Iteration;                  ///
  MrcMode         MrcMode;                    ///< The control for full or MiniBIOS MRC.
  MrcVddSelect    VddVoltage;                 ///< The requested voltage (VDD) setting.
  MrcProfile      MemoryProfile;              ///< The memory profile requested to be used.
  MrcBootMode     BootMode;                   ///< The requested memory controller boot mode.
  MrcBool         TxtFlag;                    ///
  MrcBool         MobilePlatform;             ///< define Mobile or Desktop platform. true is mobile.
  MrcBool         EccSupport;                 ///< Tell to the MRC if ECC supporting or not. if false the ecc will not be support even if the DIMM will support in ECC.
  MrcBool         SetRxDqs32;                 ///
  MrcBool         GfxIsVersatileAcceleration; ///< iGFX engines are in Versatile Acceleration
  MrcBool         ScramblerEnable;            ///< Enable/Disable scrambling
  MrcBool         McLock;                     ///
  MrcBool         RemapEnable;                ///
  MrcBool         AutoSelfRefreshSupport;     ///< FALSE = No auto self refresh support, TRUE = auto self refresh support.
  MrcBool         ExtTemperatureSupport;      ///< FALSE = No extended temperature support, TRUE = extended temperature support.
  U32             SaMemCfgAddress;
  U32             SaMemCfgSize;
  U32             PciEBaseAddress;            ///< define the PciE base address.
  U32             MchBarBaseAddress;          ///< define the MCH bar base address.
  U32             SmbusBaseAddress;           ///< This field defines the smbus base address.
  U32             GdxcBaseAddress;            ///< This field defines the GDXC base address.
  U32             HpetBaseAddress;            ///< This field defines the hpet base address.
  U32             MeStolenSize;               ///< define the size that the ME need in MB.
  U32             MmioSize;                   ///< define the MMIO size in MB.
  U32             TsegSize;                   ///< TSEG size that require by the system in MB.
  U32             IedSize;                    ///< IED size that require by the system in MB.
  U32             DprSize;                    ///< DPR size required by system in MB.
  U32             VddSettleWaitTime;          ///< The minimum time in nanoseconds to wait for VDD to settle after being changed.
  U16             VccIomV;                    ///< VccIO logic voltage in mV.
  U8              PowerDownMode;              ///< Option to select No PD, APD or PPD-DLLoff
  U8              PwdwnIdleCounter;           ///< Option to select the power down Idle counter.
  MrcBool         RankInterleave;             ///< Option to Enable Rank Interleave.
  MrcBool         EnhancedInterleave;         ///< Option to Enable Enhanced Interleave.
  MrcBool         WeaklockEn;                 ///< Option to Enable Weaklock for CMD, CTL and CKE
  U8              EnCmdRate;                  ///< Option to Enable and select the number of CMDs for 1.5NMode
  MrcBool         CmdTriStateDis;             ///< Option to Disable cmd tri-state
  MrcBool         RefreshRate2x;              ///< Tells the MRC to enable 2x Refresh.
  MrcBool         ChHashEnable;               ///< Option to Enable Channel Hash.
  U16             ChHashMask;                 ///< Option to select Address bits[19:6] to include in Channel XOR function.
  U8              ChHashInterleaveBit;        ///< Option to select interleave Address bit. Valid values are 0 - 3 for BITS 6 - 9.
  ThermalMngmtEn  ThermalEnables;             ///< Options to Enable Thermal management settings
  MrcControllerIn Controller[MAX_CONTROLLERS];
#ifdef SSA_FLAG
  U32             SsaCallbackPpi;
#endif // SSA_FLAG
#if ((defined SSA_FLAG) || (defined MRC_DEBUG_PRINT))
  U32             SsaHeapBase;      ///< Starting address of the SSA services heap space.
  U32             SsaHeapSize;      ///< Size of the SSA services heap space, in bytes.
  SsaHeapFlagType SsaHeapFlag;      ///< Bit 0 = 1 is SSA heap initialized, all other bits reserved.
#endif
  MrcGdxc         Gdxc;             ///< GDXC enable and size.
  MrcBool         MemoryTrace;      ///< Option to Enable Memory Trace to second DDR channel using Stacked Mode
  TrainingStepsEn TrainingEnables;  ///< Options to Enable individual training steps
  MrcBool         OemCleanMemory;   ///< TRUE to request a memory clean
  MrcBool         RmtBdatEnable;    ///< Option to enable output of training results into BDAT.
#ifdef ULT_FLAG
  MrcBool         DqPinsInterleaved;  ///< Interleaving mode of DQ/DQS pins - depends on board routing
  MrcBool         LpddrDramOdt;       ///< TRUE if LPDDR DRAM ODT is used - depends on board design
  U8              CkeRankMapping;     ///< [3:0] - Channel 0, [7:4] - Channel 1.
                                      ///< Bit [i] specifies which rank CKE[i] goes to.
#endif
  U8              MaxRttWr;           ///< Maximum DIMM RTT_WR to use in power training 0 = Off, 1 = 120 ohms
} MrcInput;

///
///********************************************
/// Saved data related "global data" structures.
///********************************************
///

///
/// This data structure contains all the "global data" values that are considered to be needed
/// by the MRC between power state transitions (S0->S3->S0) and also fast and warm boot modes.
/// The following are channel level definitions.
///
typedef struct {
  MrcChannelSts Status;                               ///< Indicates whether this channel should be used.
  MrcBool       EccSupport;                           ///< TRUE if ECC is enabled and supported on this DIMM.
  U32           DimmCount;                            ///< Number of valid DIMMs that exist in the channel.
  U8            ValidRankBitMask;                     ///< Bit map of the populated ranks per channel
  MrcTiming     Timing[MAX_PROFILE];                  ///< The channel timing values.
  MrcDimmOut    Dimm[MAX_DIMMS_IN_CHANNEL];
  MrcSpdSave    SpdSave[MAX_DIMMS_IN_CHANNEL];        ///< Save SPD information needed for SMBIOS structure creation.
} MrcChannelSave;

///
/// This data structure contains all the "global data" values that are considered to be needed
/// by the MRC between power state transitions (S0->S3->S0) and also fast and warm boot modes.
/// The following are controller level definitions.
///
typedef struct {
  MrcControllerSts  Status;                           ///< Indicates whether this controller should be used.
  U8                ChannelCount;                     ///< Number of valid channels that exist on the controller.
  MrcChannelSave    Channel[MAX_CHANNEL];
} MrcContSave;

///
/// This data structure contains all the "global data" values that are considered to be needed
/// by the MRC between power state transitions (S0->S3->S0) and also fast and warm boot modes.
/// The following are system level definitions.
///
typedef struct {
  U32 Crc;                  ///< The CRC-32 of the data in this structure.
} MrcSaveHeader;

//
// ------- IMPORTANT NOTE --------
// MRC_MC_REGISTER_COUNT in Global.h should match the table in MrcSaveRestore.c.
// Update this define whenever you add/remove registers from this table.
//
 #define MRC_MC_REGISTER_COUNT (1 + (2496 / sizeof (U32))) ///< The number of MC registers that need to be saved.

typedef struct {
  MrcCapId        McCapId;                            ///< The memory controller's capabilities.
  U32             MeStolenSize;
  U32             McRegister[MRC_MC_REGISTER_COUNT];  ///< The memory controllers registers.
  MrcCpuStepping  CpuStepping;                        ///< The last cold boot happended with this CPU stepping.
  MrcCpuModel     CpuModel;                           ///< The last cold boot happended with this CPU model.
  MrcVersion      Version;                            ///< The last cold boot happended with this MRC version.
  U32             SaMemCfgCrc;                        ///< The CRC32 of the system agent memory configuration structure.
  MrcContSave     Controller[MAX_CONTROLLERS];
  MrcFrequency    Frequency;                          ///< The system's common memory controller frequency.
  U32             MemoryClock;                        ///< The system's common memory controller clock, in femtoseconds.
  MrcRefClkSelect RefClk;                             ///< The memory controller is going to use this reference clock.
  MrcClockRatio   Ratio;                              ///< Request for this memory controller to use this clock ratio.
  MrcVddSelect    VddVoltage[MAX_PROFILE];            ///< The voltage (VDD) setting for all DIMMs in the system, per profile.
  MrcBool         EccSupport;                         ///< TRUE if ECC is enabled and supported on this controller.
  MrcDdrType      DdrType;                            ///< DDR type: DDR3 or LPDDR3
  MrcBool         XmpProfileEnable;                   ///< XMP capable DIMMs detected in system (0 = no, 1 = yes).
} MrcSaveData;

typedef struct {
  MrcSaveHeader Header;                               ///< The header portion of the MRC saved data.
  MrcSaveData   Data;                                 ///< The data portion of the MRC saved data.
} MrcSave;

typedef struct {
  MrcInput  Inputs;
} SysInput;

typedef struct {
  MrcOutput Outputs;
} SysOutput;

typedef struct {
  MrcSave Save;
} SysSave;

///
/// This data structure contains all of the MRC "global data" values.
///
typedef struct {
  U32       SaveSize;
  SysSave   SysSave;
  SysInput  SysIn;
  SysOutput SysOut;
  U64       Oem;
#ifdef BDAT_SUPPORT
  RmtData   Rmt;
#endif
} MrcParameters;

#pragma pack (pop)
#endif

