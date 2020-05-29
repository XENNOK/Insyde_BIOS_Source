/** @file
    This file contains all the crosser training algorithm definitions.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@Copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved.
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
  license agreement.
**/

#ifndef _MRC_CROSSER_H_
#define _MRC_CROSSER_H_

//
// Include files
//
#include <MrcTypes.h>
#include <MrcApi.h>
#include <McAddress.h>
#include <MrcCommon.h>
#include <MrcDdr3.h>
#include <MrcIoControl.h>
#include <MrcOem.h>
#include <MrcReadDqDqs.h>
#include <MrcWriteDqDqs.h>
#include <MrcRefreshConfiguration.h>
#include <MrcTimingConfiguration.h>
#include <MrcReset.h>

#define MAX_BITS_FOR_OFFSET_TRAINING    (MAX_BITS + 1)    ///< for ULT offset training done for 8 bits + DQS bit

///
/// Module Defines
///
#define CROSSER_DISABLE_SQUARED_FUNCTION (0)
#define CROSSER_ENABLE_SQUARED_FUNCTION  (1)
#define CROSSER_MIDDLE_SCALING_1         (1)
#define CROSSER_OPTIMIZE_LOW_POWER       (0)
#define CROSSER_OPTIMIZE_HIGH_POWER      (1)
#define CROSSER_EXCLUDE_END_POINTS       (0)
#define CROSSER_INCLUDE_END_POINTS       (1)

///
/// Enumerations and Structs
///

typedef enum {
  RdOdt,
  WrDS,
  WrDSCmd,
  WrDSCtl,
  WrDSClk,
  SCompDq,
  SCompCmd,
  SCompCtl,
  SCompClk,
  DisOdtStatic
} TGlobalCompOffset;

typedef enum {
  RdSAmpOfft,
  WrDSOfft,
  RxEqOfft,
  TxEqOfft,
  RdOdtOfft,
  SizeOfTCompOffset
} TCompOffset;

typedef enum {
  rd2rdXtalk,
  rd2wrXtalk,
  wr2wrXtalk,
  wr2rdXtalk,
  AllXtalk
} CrossTalkModes;

///
/// These enums index MoreResultsStrings in PrintCalcResultTableCh()
///
typedef enum {
  MrcOptResultBest,
  MrcOptResultGrdBnd,
  MrcOptResultOffSel,
  MrcOptResultScale,
  MrcOptResultSignal,
  MrcOptResultNoise,
  MrcOptResultRatio,
  MrcOptResultMaxPost,
  MrcOptResultMinPost,
  MrcOptResultTicks,
  MrcOptResultSnrTot,

  MrcOptResultMax
} MrcOptResultString;


#define  MaxOptOff       (35)

#pragma pack (push, 1)
typedef struct {
  U32              EW;
} OptResult;

typedef struct {
  U16              Best;
  S8               GuardBand;
  U8               Scale[5];
  U32              Signal[5];
  U32              Noise[5];
  U32              Ratio[5];
  U32              MaxPost[5];
  U32              MinPost[5];
  U16              Ticks[5];
  U64              SNRTotal;
  U64              MaxR;
  U64              MinR;
  U64              Result[MaxOptOff];
  OptResult        Margins[5][MaxOptOff];
} OptResultsPerByte;
#pragma pack (pop)

typedef struct {
  S16              Offset[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U16              Margins[4][MAX_CHANNEL];
  U8               TestList[4][MAX_CHANNEL];
  U8               NumTests;
  U8               Best;
} OptOffsetChByte;

typedef struct {
  U8        RttNom[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8        RttWr[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  S8        GRdOdt;
  U32       GRdOdtCode;
} DimmOffset;

typedef struct {
  U32        CpuPwrRd;
  U32        DimmPwrRd;
  U32        CpuPwrWr;
  U32        DimmPwrWrNT;
  U32        DimmPwrWrT;
  U32        ACPowerRd;
  U32        ACPowerWr;
  U32        ACPower;
  U16        CpuPower;
  U16        DimmPwr;
  U16        TotPwr;
} MrcPower;

typedef struct {
  DimmOffset        ODTSet;
  MrcPower          PowerCalc;
  U16               Test[5][MAX_CHANNEL];
  OptOffsetChByte   BestOptOff[SizeOfTCompOffset][MAX_RANK_IN_CHANNEL];
  U8                NumTests;
  U8                TestList[4];
  U8                OptParamTestList[5];
  U8                OptParamTestListSize;
  U16               Points2Trade[5][MAX_CHANNEL];
} DimmOptPoint;

/**
  This function implements Sense Amp Offset training.
  SenseAmp/ODT offset cancellation
  Find the best "average" point for Vref Control
  Test Vref point with SampOffset=-7 and Test Vref Point with SampOffset=+7
  Find Vref on per ch/byte basis where -7 samples all 1 and +7 samples all 0

  @param[in,out] MrcData - Include all MRC global data.

  @retval MrcStatus - if it succeded return mrcSuccess
**/
extern
MrcStatus
MrcSenseAmpOffsetTraining (
  IN OUT MrcParameters *const MrcData
  );

/**
  This function looks at the margin values stored in the global data structure and checks
  WrT, WrV, RdT, and RdV to see if they are above the minimum margin required.

  @param[in, out] MrcData - MRC global data.

  @retval mrcSuccess if margins are acceptable.
  @retval Otherwise, mrcRetrain.
**/
MrcStatus
MrcRetrainMarginCheck (
  IN OUT MrcParameters *const MrcData
  );

/**
  This function implements Dimm Ron training.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus -  if it succeeds return mrcSuccess
**/
extern
MrcStatus
MrcDimmRonTraining (
  IN MrcParameters *const MrcData
  );

/**
  This function implements DIMM ODT training.
  Adjust DIMM RTT_NOM/RTT_WR value to maximize read/write voltage/timing

  RdOdtPriority Needs to be an input parameter
  option to prioritize the ReadODT setting and attempt to optimize that value first,
  reducing CPU TDP power (as opposed to system power for the DRAM).
  For this case, the base value for ReadODT is changed at the compensation block
  by looking at the following values:
     RdOdt Global: (50, 64, 84, 110)

  In the case of 2 dpc, the flow will first optimizing RttNom, while keeping RttWr fixed
  at 60 Ohms (60 Ohms usually gives the best results).  It will then try to reduce RttWr
  to 120 Ohms if possible.

  In the case of 1 dpc, only RttNom is used and only a single pass is required.
  However, it is important to note that the two channels are completely independent
  and can have different numbers of dimms populated.

  @param[in] MrcData         - Include all MRC global data.

  @retval MrcStatus      - if it succeed return mrcSuccess
**/
extern
MrcStatus
MrcDimmODTTraining (
  IN MrcParameters *const MrcData
  );

/**
  This function implements Dimm Odt training.
  Optimize Dimm Odt value for performance/power

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeded return mrcSuccess
**/
extern
MrcStatus
MrcDimmODT1dTraining (
  IN MrcParameters *const MrcData
  );

/**
  This function implements Read Equalization training.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - if it succeeds return mrcSuccess
**/
extern
MrcStatus
MrcReadEQTraining (
  IN MrcParameters *const MrcData
  );

/**
  This function implements Write (Transmitter) Equalization training.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - if it succeeds return mrcSuccess
**/
extern
MrcStatus
MrcWriteEQTraining (
  IN MrcParameters *const MrcData
  );

/**
  This function implements Read Amplifier Power training.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeeds return mrcSuccess
**/
extern
MrcStatus
MrcReadAmplifierPower (
  IN MrcParameters *const MrcData
  );

/**
  Returns the index into the array OptResult in the MrcOutput structure.

  @param[in] OptParam - Margin parameter

  @retval One of the following values: RdSAmpOfft(0), WrDSOfft (1), RxEqOfft(2), TxEqOfft (3), RdOdtOfft(4)
**/
extern
U8
GetOptResultType(
  IN U8 OptParam
  );

/**
  This function implements Read ODT training and Write DS.
  Optimize Read ODT strength for performance & power.

  @param[in,out] MrcData         - Include all MRC global data.
  @param[in,out] BestOff         - Structure containg the best offest and margins for th Opt param.
  @param[in]     ChannelMask     - Channels to train
  @param[in]     RankMask        - Condenses down the results from multiple ranks
  @param[in]     OptParam        - Defines the OptParam Offsets.
                               Supported OptParam = [0: WrDS, 1: RdODT, 2: SComp, 3: TComp, 3: TxEq, 4: RxEq,
                                                     5: RxBias, 6: DimmOdt, 7: DimmOdtWr]
  @param[in]     TestList        - List of margin params that will be tested (up to 4)
  @param[in]     NumTests        - The length of TestList
  @param[in]     Scale           - List of the relative importance between the 4 tests
  @param[in]     PwrLimitsABC    - List of the values for each test margin, above which margin is "adequate"
  @param[in]     Start           - Start point of sweeping the Comp values
  @param[in]     Stop            - Stop point of sweeping the Comp values
  @param[in]     LoopCount       - The number of loops to run in IO tests.
  @param[in]     Repeats         - Number of times to repeat the test to average out any noise
  @param[in]     NoPrint         - Switch to disable printing.
  @param[in]     SkipOptUpdate   - Switch to train but not update Opt settings.
  @param[in]     RdRd2Test       - Switch to run with different TA times: possible values are [0, RdRdTA, RdRdTA_All]
  @param[in]     GuardBand       - Signed offset to apply to the Opt params best value.

  @retval Nothing
**/
extern
void
TrainDDROptParam (
  IN OUT MrcParameters *const MrcData,
  IN OUT OptOffsetChByte      *BestOff,
  IN     U8                   ChannelMask,
  IN     U8                   RankMask,
  IN     U8                   OptParam,
  IN     U8                   *TestList,
  IN     U8                   NumTests,
  IN     U8                   *Scale,
  IN     U16                  *PwrLimitsABC,
  IN     S8                   Start,
  IN     S8                   Stop,
  IN     U8                   LoopCount,
  IN     U8                   Repeats,
  IN     BOOL                 NoPrint,
  IN     BOOL                 SkipOptUpdate,
  IN     U8                   RdRd2Test,
  IN     S8                   GuardBand
  );

/**
  This function implements Read ODT training.
  Optimize Read ODT strength for performance & power

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeded return mrcSuccess
**/
extern
MrcStatus
MrcReadODTTraining (
  IN MrcParameters *const MrcData
  );

/**
  This function is the Write Drive Strength training entry point.
  This step will optimize write drive strength for performance & power.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeeds return mrcSuccess
**/
extern
MrcStatus
MrcWriteDriveStrength (
  IN MrcParameters *const MrcData
  );

/**
  Updates a given ch/Rank/byte combination with a new value for OptParam
  OptParam can be: WrDS, RdOdt, TComp, SComp, RxEq, TxEq, RxBias or DimmOdt
  OptParam == OptDefault restore values from Host except Dimms Odt's
  @param[in,out] MrcData         - Include all MRC global data.
  @param[in]     Channel         - Channel index to work on.
  @param[in]     Ranks           - Condenses down the results from multiple ranks
  @param[in]     Byte            - Byte index to work on.
  @param[in]     OptParam        - Defines the OptParam Offsets.
                                   Supported OptParam = [0: WrDS, 1: RdODT, 2: SComp, 3: TComp, 4: TxEq,
                                                         5: RxEq, 6: RxBias, 7: DimmOdt, 8: DimmOdtWr]
  @param[in]     Off             - Offset
  @param[in]     UpdateHost      - Desides if MrcData has to be updated

  @retval Nothing
**/
extern
void
UpdateOptParamOffset (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Channel,
  IN     const U8             Ranks,
  IN     const U8             Byte,
  IN     const U8             OptParam,
  IN     S16                  Off,
  IN     const U8             UpdateHost
  );

/**
  Slightly penalize any Asymmetry in margin

  @param[in] NegEdge - Negative edge of the margin
  @param[in] PosEdge - Positive edge of the margin

  @retval p2p - Width/Height reduced by the asymmetric difference in margin.
**/
extern
U16
EffectiveMargin (
  IN const U16 NegEdge,
  IN const U16 PosEdge
  );

/**
  This function does a running average on Margins in two dimentional fashion.

  @param[in,out] Margins - Margins to average
  @param[in]     Test    - Selects the Margins to average
  @param[in]     MLen    - Determines the Y-Dimension lengths
  @param[in]     XDim    - Determines the X-Dimension lengths
  @param[in]     XMin    - Used to skip the first elements in the Margin when averaging.
  @param[in]     CScale  - Used to place more weight on the center point.

  @retval Nothing
**/
extern
void
RunningAverage2D (
  IN OUT U16       Margins[2][24],
  IN     const U8  Test,
  IN     const U8  MLen,
  IN     const U8  XDim,
  IN     const U8  XMin,
  IN     const U8  CScale
  );

/**
    Updates a given ch/Rank/byte combination with a new value for OptParam
    OptParam can be: WrDS, RdOdt, TComp, SComp, RxEq, TxEq, RxBias or DimmOdt

    # Margins: Upto 4 arrays that contain lenMargin elements
    # Index to the array represents some arbitrary parameter value that we are optimizing
    # Scale is 4 element array that scales the relative importance on Margins[0] vs. [1] ...
    #    ex: To make Margins[0] twice as important, set Scale = [1, 2, 2, 2]
    #    Since the search optimizes the lowest margin, increasing 1/2/3 makes 0 more important
    #    This function can be used to optimize only Margin[0] by setting Scale = [1, 0, 0, 0]
    # EnSq = 1 uses a squared function to make the tradeoff between 0/1/2/3 steeper
    # If AveN > 0, pre-processes the results with a N point running average filter
    # IncEnds: By setting to 1, the running average will also include the end points
    # ScaleM:  Allows the middle point of the running average to be scaled up
    #
    # In addition to optimizing for margin, this function can also optimize for power
    # PwrLimit is a 4 element array that sets level where pwr is more important than margin
    # Find any points where ((Margin[0]>PwrLimit[0]) & (Margin[1]>PwrLimit[1]) & ... )
    # If such points exists and PwrOptHigh = 1, returns point with the highest X value
    # If such points exists and PwrOptHigh = 0, returns point with the lowest X value
    # If you don't want to optimize for power, set PwrLimitA and PwrLimitB to large number
    # Power Optimize still uses the running average filter
    #
    # To avoid overflow, this function will automatic scale margins to fit in uint32

  @param[in]     MrcData       - The global MRC data structure.
  @param[in,out] OptResByte    - Structure containing the optimized results.
  @param[in]     inputMargins  - Margins we are optimizing
  @param[in]     MarginsLength - The length of inputMargins
  @param[in]     LenMargin     - The length of inputMargins we are optimizing (0 - LenMargin -1).
  @param[in]     Scale         - Controls the scaling of the input margin: 1-1, 1-2, ... and so on.
  @param[in]     EnSq          - Enables the square root term in the optimization functions.
  @param[in]     AveN          - The number of points used for the averaging filter.
  @param[in]     IncEnds       - Controls if the endpoints are to be included.
  @param[in]     ScaleM        - Controls the scaling of the middle point in 1-D average filter.
  @param[in]     PwrLimit      - The power limit above which we only trade-off for power and not margin.
  @param[in]     PwrOptHigh    - Controls returning the highest or lowest optimization point.
  @param[in]     GuardBand     - Signed offest to check if margin drop is acceptable.  Save good guardband
                                 in OptResByte.

  @retval Nothing.
**/
extern
void
FindOptimalTradeOff (
  IN     MrcParameters      *const MrcData,
  IN OUT OptResultsPerByte         *OptResByte,
  IN     void                      *inputMargins,
  IN     U8                        MarginsLength,
  IN     S8                        LenMargin,
  IN     U8                        *Scale,
  IN     U8                        EnSq,
  IN     U8                        AveN,
  IN     U8                        IncEnds,
  IN     U8                        ScaleM,
  IN     U16                       *PwrLimit,
  IN     U8                        PwrOptHigh,
  IN     S8                        GuardBand
  );

/**
  This function implements Turn Around Timing training.
  Optimize TA ODT Delay and Duration

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeeds return mrcSuccess.
**/
extern
MrcStatus
MrcTurnAroundTiming (
  IN MrcParameters * const MrcData
  );

/**
  General purpose function to optimize an abritray value, OptParam (see list above)
    OptParam is generally some timing number that impacts performance or power
    Expects that as OptParam gets smaller*, margins are flat until we hit a cliff
    This procedure defines a cliff as a reducution of 4 ticks in eye height/width
    * In the case of mcodts, higher values are actually worst
    To stress out the timing, xxDDR_CLK is shifted by +/- 15 PI ticks

  @param[in] MrcData         - Include all MRC global data.
  @param[in] OptParam        - Supports Turnaround Timings and ODT Start / Duration
  @param[in] TestList        - List of margin param to check to make sure timing are okay.
  @param[in] NumTests        - The size of TestList
  @param[in] Start           - Start point for this turn around time setting.
  @param[in] Stop            - Stop point for this turnaround time setting.
                                 Note that the Start/Stop values are the real values, not the encoded value
  @param[in] LoopCount       - Length of a given test
  @param[in] Update          - Update the CRs and host structure with ideal values
  @param[in] ClkShifts       - Array of Pi clocks to be shifted
  @param[in] MarginByte      - Byte level margins
  @param[in] NumR2RPhases    - Number of PI clock phases
  @param[in] rank            - rank to work on
  @param[in] RankMask        - RankMask to be optimized
  @param[in] GuardBand       - GuardBand to be added to last pass value (to be a bit conservative).

  @retval MrcStatus      - If it succeeds return mrcSuccess
**/
extern
MrcStatus
TrainDDROptParamCliff (
  IN MrcParameters *const MrcData,
  IN U8                   OptParam,
  IN U8                   TestList[],
  IN U8                   NumTests,
  IN S8                   Start,
  IN S8                   Stop,
  IN U8                   LoopCount,
  IN U8                   Update,
  IN U32                  MarginByte[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN S8                   *ClkShifts,
  IN U8                   NumR2RPhases,
  IN U8                   rank,
  IN U8                   RankMask,
  IN U8                   GuardBand
  );

/**
  Sets commnad margins when moving WrT, WrTBox, or WrV
  NOTE: ONLY one, ResetDDR or SelfRefresh can be set inside this function

  @param[in] MrcData         - Include all MRC global data.
  @param[in] ChBitMask       - Bit mask of populated channels
  @param[in] Ranks           - Bit Mask of populated ranks
  @param[in] Param           - Input parameter to update
  @param[in] Value0          - value to be added
  @param[in] Value1          - value to be added
  @param[in] ResetDDR        - Do we reset DDR?
  @param[in] SelfRefresh     - Do we perform Self refresh?

  @retval MrcStatus      - If it succeeds return mrcSuccess
**/
extern
void
SetCmdMargin (
  IN MrcParameters *const MrcData,
  IN const U8             ChBitMask,
  IN const U8             Ranks,
  IN const U8             Param,
  IN const U8             Value0,
  IN const U8             Value1,
  IN U8                   ResetDDR,
  IN const U8             SelfRefresh
  );

/**
  Updates the value for following OptParamCliff variables:
  drrd2rd=0, ddrd2rd=1, drwr2wr=2, ddwr2wr=3, drrd2wr=4, ddrd2wr=5, drwr2rd=6, ddwr2rd=7,
  rdodtd=8, wrodtd=9, mcodts=10, mcodtd=11, rtl=12}

  @param[in,out] MrcData    - Include all MRC global data.
  @param[in]     Channel    - Channel to update the specificed parameter.
  @param[in]     Byte       - Byte to update the specified parameter.
  @param[in]     OptParam   - Parameter to update.
  @param[in]     Off        - Value to offset the current setting.
  @param[in]     UpdateHost - Switch to update the host structure with the new value.
  @param[in]     SkipPrint  - Switch to skip debug prints.
  @param[in]     RankMask   - Bit mask of Ranks to update.

  @retval Nothing
**/
extern
void
UpdateTAParamOffset (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Channel,
  IN     const U8             Byte,
  IN     const U8             OptParam,
  IN     const U8             Off,
  IN     const U8             UpdateHost,
  IN     const U8             SkipPrint,
  IN     const U8             RankMask
  );

/**
  This function applies the new DRAM ODT settings
  Walks through various optimizations to get the best result with new ODT values
  This includes WrDS, RdODT, Eq, etc.
  Updates Best* variables if this point if better than the prior points
  chDone is both an input and output.  Reports which channels have a good enough value
  if SkipRd is high, it will skip the read related functions (RdODT, RdEq, RdTiming)

  @param[in]     MrcData              - Include all MRC global data.
  @param[in,out] DimmOptPoints        - Structure of all the DIMM ODT optimal settings.
  @param[in]     ChMask               - Channel to work on.
  @param[in]     RankMask             - Rank to work on.
  @param[in]     skipGRdOdt           - Used to skip RdODT.
  @param[in]     RttNom               - Rtt_Nom value for each DIMM.
  @param[in]     RttWr                - Rtt_Wr value for each DIMM.
  @param[in]     GRdOdt               - CPU Global Read ODT.
  @param[in]     OptParamTestList     - List of Opt test(Drive Strength, RxBias, TxEq, RxEq) to run.
  @param[in]     OptParamTestListSize - Size of OptParamTestList.
  @param[in]     SubPwrLimits         - Switch to apply power limits to the suboptimization.
  @param[in]     skipOptTests         - Skips the suboptimization.
  @param[in]     skipOptPrint         - Skip printing of the suboptimization steps.
  @param[in]     RdCenter             - Switch to recenter read.
  @param[in]     WrCenter             - Switch to recenter write.
  @param[in]     inputBestMargin      - Array of the best margin for each test.
  @param[in]     MarginsLength        - Length of inputBestMargin.
  @param[in]     OffsetPoint          - Index inside inputBestMargin to start.

  @retval Nothing.
**/
extern
void
TrainDimmOdtSetting (
  IN     MrcParameters *const MrcData,
  IN OUT DimmOptPoint         *DimmOptPoints,
  IN     U8                   ChMask,
  IN     U8                   RankMask,
  IN     U8                   skipGRdOdt,
  IN     U8                   RttNom[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL],
  IN     U8                   RttWr[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL],
  IN     S8                   GRdOdt,
  IN     U8                   *OptParamTestList,
  IN     U8                   OptParamTestListSize,
  IN     BOOL                 SubPwrLimits,
  IN     BOOL                 skipOptTests,
  IN     BOOL                 skipOptPrint,
  IN     BOOL                 RdCenter,
  IN     BOOL                 WrCenter,
  IN     void                *inputBestMargin,
  IN     U8                   MarginsLength,
  IN     U8                   OffsetPoint
  );

/**
  This function applies an offset to the global compensation logic.
  Reruns Compensation and returns the new comp value

  @param[in,out] MrcData         - Include all MRC global data.
  @param[in]     param           - Parameter defining the desired global compensation logic
  @param[in]     offset          - Value to apply
  @param[in]     UpdateHost      - Desides if MrcData has to be updated

  @retval Returns the new comp value.
**/
extern
U32
UpdateCompGlobalOffset (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             param,
  IN     const U32            offset,
  IN     const U8             UpdateHost
  );

/**
  Programs Delay/Duration for the SenseAmp and MCODT based on RcvEn timing
  Provide GuardBand > 0 if needed to be more conservative in timing
  Main goal is to optimize power

  @param[in,out] MrcData   - Include all MRC global data.
  @param[in]     GuardBand - Input parameter with more conservative value

  @retval Nothing
**/
extern
void
UpdateSampOdtTiming(
  IN OUT MrcParameters *const MrcData,
  IN     const U8             GuardBand
  );

/**
  Turns off unused portions of the slave DLL to save power

  @param[in,out] MrcData - Include all MRC global data.

  @retval Nothing
**/
extern
void
UpdateSlaveDLLLength(
  IN OUT MrcParameters * const MrcData
  );

#ifdef TRAD_FLAG
/**
  Update Internal clocks on setting if needed.

  @param[in,out] MrcData - Include all MRC global data.

  @retval Nothing
**/
extern
void
UpdateInternalClksOn (
  IN OUT MrcParameters *const MrcData
  );
#endif // TRAD_FLAG

/**
  This function Shifts the CMD timing.
  NOTE: ONLY one, ResetDDR or SelfRefresh can be set inside this function

  @param[in,out] MrcData     - Include all MRC global data.
  @param[in]     Ranks       - Parameter defining the desired global compensation logic
  @param[in]     offset      - per channel Value to shift picode for
  @param[in]     ResetDDR    - Do we reset DDR?
  @param[in]     SelfRefresh - Do we perform Self refresh?
  @param[in]     UpdateHost  - Determines if MrcData has to be updated
  @param[in]     SkipTx      - Determines if TX update should be skipped
                               @todo: SkipTx is NOT USED at this time and we don't skip it anyway

  @retval MrcStatus       - If it succeeds return mrcSuccess
**/
extern
MrcStatus
ShiftCh2Ch (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Ranks,
  IN     const U8 *const      offset,
  IN     U8                   ResetDDR,
  IN     const U8             SelfRefresh,
  IN     const U8             UpdateHost,
  IN     const U8             SkipTx
  );

/**
  Programs Delay/Duration for the SenseAmp and MCODT based on RcvEn timing
  Provide GuardBand > 0 if needed to be more conservative in timing
  Main goal is to optimize power

  @param[in,out] MrcData          - Include all MRC global data.
  @param[in]     ChMask           - Channel to work on.
  @param[in,out] BestDimmOptPoint - Best DIMM Opt settings used to update hardware
  @param[in]     SkipGRdOdt       - Switch to skip updating CPU ODT
  @param[in]     SkipDimmOdts     - Switch to skip updating DIMM ODT
  @param[in]     SkipBestOffsets  - Switch to skip updating Opt settings
  @param[in]     UpdateHost       - Switch to skip updating MRC host structure

  @retval Nothing
**/
extern
void
UpdateOdtsValues(
  IN OUT MrcParameters *const MrcData,
  IN     U8                   ChMask,
  IN OUT DimmOptPoint         *BestDimmOptPoint,
  IN     BOOL                 SkipGRdOdt,
  IN     BOOL                 SkipDimmOdts,
  IN     BOOL                 SkipBestOffsets,
  IN     BOOL                 UpdateHost
  );

/**
  Peform Read Voltage Centering in 2D.
  Note: This function currently only supports param = RdV

  @param[in,out] MrcData     - Include all MRC global data.
  @param[in,out] MarginByte  - Pointer to Marging Results data structure
  @param[in]     ChBitMask   - Channel bit mask.
  @param[in]     Param       - {0:RcvEn, 1:RdT, 2:WrT, 3: WrDqsT, 4:RdV, 5:WrV, 6:WrLevel,
                                8:WrTBit, 9:RdTBit, 10:RdVBit,
                                16:RdFan2, 17:WrFan2, 32:RdFan3, 32:WrFan3}
                                ONLY RdV is allowed in this function
  @param[in]     EnPerBit    - Option to enable per bit margining
  @param[in]     ResetPerBit - Option to Reset PerBit Deskew to middle value before byte training
  @param[in]     LoopCount   - Loop count
  @param[in]     En2D        - Option to only run center at nominal Vref point

  @retval MrcStatus - If it succeded return mrcSuccess
**/
extern
MrcStatus
ReadVoltageCentering2D (
  IN OUT MrcParameters *const MrcData,
  IN OUT U32                  MarginByte[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN     U8                   ChBitMask,
  IN     U8                   Param,
  IN     U8                   EnPerBit,
  IN     U8                   ResetPerBit,
  IN     U8                   LoopCount,
  IN     U8                   En2D
  );

#ifdef MRC_DEBUG_PRINT
/**
  Prints OptParam values from CRs and Host structure for all ch/Rank/byte as well as
  the Best optimization value (if requested)
  OptWrDS = 0
  OptRdOd = 1
  OptSCom = 2
  OptTComp = 3
  OptTxEq = 4
  OptRxEq = 5
  OptRxBias = 6
  OptDimmOdt = 7
  OptDimmOdtWr = 8
  OptDimmRon = 9
  OptDefault = 10

  @param[in] MrcData   - Include all MRC global data.
  @param[in] ChMask    - Channel Mask to print the summary for
  @param[in] RankMask  - Rank Mask to print the summary for (in case Rank is not applicable set RankMask = 0xF)
  @param[in] OptParam  - Defines the OptParam Offsets. OptDefault reports all parameters
  @param[in] OptOff    - Structure containg the best offest and margins for the OptParam.
                         If OptOffsetChByte is not available, NullPtr needs to be passed (void  *NullPtr)
  @param[in] OptResult - True/False: Whether to print the Best optimization value

  @retval Nothing
**/
extern
void
ReadOptParamOffsetSum (
  IN MrcParameters *const MrcData,
  IN U8                   ChMask,
  IN U8                   RankMask,
  IN const U8             OptParam,
  IN OptOffsetChByte      *OptOff,
  IN BOOL                 OptResult
  );

/**
  Reads OptParam value from CRs and Host structure for a given ch/Rank/byte combination
  OptParam can be: WrDS, RdOdt, TComp, SComp, RxEq, TxEq, RxBias, DIMM Ron, DIMM RttNom or DIMM RttWr

  @param[in]  MrcData  - Include all MRC global data.
  @param[out] FinalVal - Pointer to the array consisting of CR value and Host value for a particular
                         OptParam and given ch/Rank/byte combination.
  @param[in]  Channel  - Channel index to work on.
  @param[in]  Rank     - Rank index to work on (valid only for TxEq and RxEq, for others is ignored)
  @param[in]  Byte     - Byte index to work on.
  @param[in]  OptParam - Defines the OptParam Offsets. Supported OptParam =
                         [0: WrDS, 1: RdODT, 2: SComp, 3: TComp, 3: TxEq,
                          4: RxEq, 5: RxBias, 6: DimmOdt, 7: DimmOdtWr]

  @retval Nothing
**/
extern
void
ReadOptParamOffset (
  IN  MrcParameters *const MrcData,
  OUT S16                  *FinalVal,
  IN  const U8             Channel,
  IN  const U8             Rank,
  IN  const U8             Byte,
  IN  const U8             OptParam
  );

/**
  This function prints the Optimize margin result table
  e.g: MarginResult[Test][Offset][Channel][Byte][sign]

  @param[in] MrcData     - MRC data structure
  @param[in] ChMask      - Channels to print
  @param[in] ResultArray - Array with saved margin results
  @param[in] TestNum     - Test index
  @param[in] OffsetsNum  - number of offsets
  @param[in] MidPoint    - Zero point
  @param[in] Edges       - 1 edge or 2 edge
  @param[in] OptParam    - Used to convert to the Opt param string for printing
  @param[in] Param       - Margin type to be printed.
  @param[in] PowerLimits - Power limits to print.
  @param[in] noPrint     - Used to skip printing.

  @retval Nothing
**/
extern
void
PrintResultTableByte4by24(
  IN MrcParameters   *MrcData,
  IN U8              ChMask,
  IN U16             ResultArray[4][24][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN U16             TestNum,
  IN U8              OffsetsNum,
  IN U8              MidPoint,
  IN U8              Edges,
  IN U8              OptParam,
  IN U8              Param,
  IN U16             *PowerLimits,
  IN BOOL            noPrint
  );

/**
  This function prints the Optimize margin result table
  e.g: calcResultSummary[MAX_CHANNEL][MAX_SDRAM_IN_DIMM]

  @param[in] MrcData           - MRC data structure
  @param[in] calcResultSummary - The data array [MAX_CHANNEL][MAX_SDRAM_IN_DIMM]
  @param[in] TestList          - Test list
  @param[in] NumTest           - Number of test
  @param[in] NumOffsets        - Number of offsets
  @param[in] MidPoint          - Middle point
  @param[in] IncEnds           - Print ends points
  @param[in] OptParam          - Used to convert to the Opt param string for printing
  @param[in] OptPower          - Opt Power values to be printed
  @param[in] Channel           - Channel to print
  @param[in] Ranks             - Ranks to print
  @param[in] TrendLine         - Switch to print the trend line
  @param[in] Nibble            - take low/high bytes
  @param[in] perCh             - Switch to only print 1 Byte of data
  @param[in] noPrint           - Boolean used to disable printing of results

  @retval Nothing
**/
extern
void
PrintCalcResultTableCh(
  IN MrcParameters *const MrcData,
  IN OptResultsPerByte    calcResultSummary[MAX_CHANNEL][MAX_SDRAM_IN_DIMM],
  IN U8                   *TestList,
  IN U8                   NumTest,
  IN U8                   NumOffsets,
  IN S8                   MidPoint,
  IN BOOL                 IncEnds,
  IN U8                   OptParam,
  IN U16                  *OptPower,
  IN U8                   Channel,
  IN U8                   Ranks,
  IN BOOL                 TrendLine,
  IN U8                   Nibble,
  IN BOOL                 perCh,
  IN BOOL                 noPrint
  );

/**
  This function prints the Optimize margin result table
  e.g: calcResultSummary[MAX_CHANNEL][MAX_SDRAM_IN_DIMM]

  @param[in] MrcData           - MRC data structure
  @param[in] calcResultSummary - The data array [MAX_CHANNEL][MAX_SDRAM_IN_DIMM]
  @param[in] DimmOptPoints     - add argument and description to function comment
  @param[in] TestList          - Test list
  @param[in] NumTest           - Number of test
  @param[in] NumOffsets        - Number of offsets
  @param[in] MidPoint          - Middle point
  @param[in] IncEnds           - Print ends points
  @param[in] OptParam          - Used to convert to the Opt param string for printing
  @param[in] Channel           - Channel to print
  @param[in] Ranks             - Ranks to print
  @param[in] TrendLine         - Switch to print the trend line
  @param[in] Nibble            - take low/high bytes
  @param[in] perCh             - Switch to only print 1 Byte of data

  @retval Nothing
**/
extern
void
PrintODTResultTable(
  IN MrcParameters *const MrcData,
  IN OptResultsPerByte    *calcResultSummary,
  IN DimmOptPoint         *DimmOptPoints,
  IN U8                   NumOffsets,
  IN S8                   MidPoint,
  IN BOOL                 IncEnds,
  IN U8                   OptParam,
  IN U8                   Channel,
  IN U8                   Ranks,
  IN BOOL                 TrendLine,
  IN U8                   Nibble,
  IN BOOL                 perCh
  );

/**
  This function will print out the last margin data collected of the Param passed in.
  It will print both edges of all the requested bytes, Ranks and Channels.
  NOTE: The function will not check to see if the Rank/Channel exists.  It will print out the
  values stored in the margin array regardless of population status.

  @param[in]  MrcData     - Global MRC data.
  @param[in]  Param       - Parameter of MRC_MarginTypes of which to print the margin.
  @param[in]  ChannelMask - Bit mask of channels to print.
  @param[in]  RankMask    - Bit mask of ranks to print.
  @param[in]  ByteMask    - Bit mask of bytes to print.

  @retval Nothing.
**/
void
MrcPrintLastMargins (
  IN MrcParameters *const MrcData,
  IN const U8             Param,
  IN const U8             ChannelMask,
  IN const U8             RankMask,
  IN const U16            ByteMask
  );
#endif // MRC_DEBUG_PRINT

/**
  This function returns the UPM or PWR limit value for the specified parameter

  @param[in] MrcData   - Pointer to MRC global data.
  @param[in] Param     - Margin type
  @param[in] LimitType - Type of limit: UpmLimit or PowerLimit

  @retval Returns the UPM or PWR limit
**/
extern
U16
UpmPwrLimitValue (
  IN MrcParameters *const MrcData,
  IN U8                   Param,
  IN U8                   LimitType
  );

/**
  This function will adjust the requested Limit Type of the margin parameter by the signed offset passed in.

  @param[in]  MrcData   - MRC global data.
  @param[in]  Param     - Margin parameter type to adjust.
  @param[in]  LimitType - MRC_MARGIN_LIMIT_TYPE to adjust.
  @param[in]  Offset    - The adjustment value.

  @retval U16 - The new value of Param[MRC_MARGIN_LIMIT_TYPE]
**/
U16
MrcUpdateUpmPwrLimits (
  IN OUT MrcParameters * const  MrcData,
  IN U8                         Param,
  IN U8                         LimitType,
  IN S8                         Offset
  );

/**
  Calculate Power based on Ron and Rodt
  Includes both static power from Ron/Rodt and dynamic power from Cpad/Cline
  The power results here are not absolutely correct but give a reasonable estimate (ie: within 2x) with the proper trends
  Getting absolutely correct power numbers with simple calculations is fairly difficult given the transmission line nature of the system
  Driver power is calculated as the amount of power drawn from the CPU pin (do we want this to be thermal power instead?) based on the Ron and ODTeff
              ODTeff is calculated as both the real, resistive ODT on the bus in parallel with the effective impendence of the cap on the line
              This effective impedance is how AC power is included in the measurements
              This better models the real system behavior where the power consumed due to dynamic power reduces as termination strength increases
  ODT power is calculated as a purely DC term based on Ron and Rodt
  The final power reported back is a scaled version of the CPU and DRAM power
  This allows one to weight the CPU vs. DRAM power differently in the optimization function based on what is more important
              CPU power is generally more important since it can be translated into additional performance

  @param[in]  MrcData  - Include all MRC global data.
  @param[out] Results  - Results of the Power power calculations
  @param[in]  RonCpu   - RON CPU value (ohm)
  @param[in]  RonDimm  - RON DIMM value (ohm)
  @param[in]  Rodtcpu  - RODT CPU value
  @param[in]  Rodtdram - RODT DRAM value
  @param[in]  Wodtdram - WODT DRAM value

  @retval Nothing
**/
extern
void
CalcPower(
  IN  MrcParameters *MrcData,
  OUT MrcPower      *Results,
  IN  U16           RonCpu,
  IN  U8            RonDimm,
  IN  U16           Rodtcpu,
  IN  U16           Rodtdram,
  IN  U16           Wodtdram
  );

/**
  This function fill the input array (e.g array[ch][rank]) with the power calculation
  per rank/ch for current sys. setting.

  @param[in]     MrcData   - MRC data struct;
  @param[in,out] PwrChRank - Array to fill;

  @retval Nothing
**/
extern
void
CalcSysPower (
  IN     MrcParameters *const MrcData,
  IN OUT MrcPower             PwrChRank[MAX_CHANNEL][MAX_RANK_IN_CHANNEL]
  );

/**
  Calculate Power Trend line based on Cpu and Dimms Ron and Odt's

  @param[in]     MrcData       - Include all MRC global data.
  @param[in]     Channel       - Channel to work on.
  @param[in]     DimmMask      - DIMMs to work on.
  @param[in,out] DimmOptPoints - Structure of all the DIMM ODT optimal settings.
  @param[in]     Points2calc   - Data to build the trendline on.
  @param[in]     ArrayLength   - Array length of Points2calc.
  @param[in]     LenMargin     - The length of inputMargins we are optimizing (0 - LenMargin -1).
  @param[in]     TestList      - TestList index in Points2cal: WrVref, RdVref, WrT, RdT
  @param[in]     Scale         - Scale to apply per test to Points2calc
  @param[in]     TestListSize  - Size of TestList/Scale
  @param[in]     PwrCalc1d     - Determines if the power test is 1-D or 2-D.
  @param[in]     PWRTrendSlope - Determines how aggressive the T-line will be.(0%-100%)

  @retval Nothing
**/
extern
void
CalcPowerTrend(
  IN     MrcParameters   *MrcData,
  IN     U8              Channel,
  IN     U8              DimmMask,
  IN OUT void            *DimmOptPoints,
  IN     void            *Points2calc,
  IN     U8              ArrayLength,
  IN     U8              LenMargin,
  IN     U8              *TestList,
  IN     U8              *Scale,
  IN     U8              TestListSize,
  IN     BOOL            PwrCalc1d,
  IN     U8              PWRTrendSlope
  );

/**
  This function returns the Actual Cpu Driver Impedance (1 segment) in ohm.

  @param[in] MrcData - Pointer to MRC global data.
  @param[in] Offset  - Vref Offset (+-8).

  @retval Returns the CPU driver impedance value (for 1 segment)
**/
extern
U16
CalcDrvImp (
  IN MrcParameters *const MrcData,
  IN S8                   Offset
  );

/**
  This function returns the Actual Cpu Odt termination in ohm.

  @param[in] MrcData - Pointer to MRC global data.
  @param[in] Offset  - Vref Offset (+-16).

  @retval Returns the Odt termination value.
**/
extern
U16
CalcRdOdt (
  IN MrcParameters *const MrcData,
  IN S8                   Offset
  );

/**
  Calculate Power for the selected Opt param based on

  @param[in] MrcData     - Include all MRC global data.
  @param[in] Channel     - Channel to work on
  @param[in] Rank        - Rank to work on
  @param[in] Byte        - Byte to work on
  @param[in] OptParam    - The Opt Parameter to work on
  @param[in] Offset      - The Offset to work on
  @param[in] CurrentComp - The current Comp code for OptParam
  @param[in] ReadHost    - Switch to read current offset and CompCode from Host structure.

  @retval Calc power in mW
**/
extern
U32
CalcOptPower(
  IN MrcParameters   *MrcData,
  IN U8              Channel,
  IN U8              Rank,
  IN U8              Byte,
  IN U8              OptParam,
  IN S8              Offset,
  IN S8              CurrentComp,
  IN BOOL            ReadHost
  );

/**
  This function implements Write Slew Rate training.
  Optimize Write Slew Rate for performance & power

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess
**/
extern
MrcStatus
MrcWriteSlewRate (
  IN MrcParameters *const MrcData
  );

/**
  This function prints out the Margin eye diagram for ParamT/ParamV.


  @param[in] MrcData - Include all MRC global data.
  @param[in] Channel - Channel to margin.
  @param[in] Ranks   - Bit mask of Ranks to margin.
  @param[in] ParamT  - Time parameter to margin.
  @param[in] ParamV  - Voltage parameter to margin.
  @param[in] Start   - Starting point for margining.
  @param[in] Stop    - Stopping point for margining.
  @param[in] Repeats - Number of times to repeat the test to average out any noise.
  @param[in] NoPrint - Switch to skip printing.

  @retval Nothing
**/
extern
void
EyeMargin (
  IN MrcParameters *const MrcData,
  IN U8                   Channel,
  IN U8                   Ranks,
  IN U8                   ParamT,
  IN U8                   ParamV,
  IN S8                   Start,
  IN S8                   Stop,
  IN U16                  SearchLimits,
  IN U8                   LoopCount,
  IN U8                   Repeats,
  IN BOOL                 NoPrint
  );

/**
  This function optimize the digital offsets by reducing the digital
  offset and apply the difference to the global one.

  @param[in] MrcData    - Include all MRC global data.
  @param[in] Param      - Parameter defining the desired digital compensation offset.
  @param[in] UpdateHost - Decides if MrcData is to be updated.

  @retval The new comp value.
**/
extern
U32
OptimizeCompOffset (
  IN MrcParameters *const MrcData,
  IN const U8             Param,
  IN const U8             UpdateHost
  );

/**
  This function implements the Write Drive Strength optimization for performance and power.

  @param[in] MrcData    - Include all MRC global data.
  @param[in] ChBitMask  - Channel mask to perform training on the Opt Param test list.
  @param[in] RecenterLC - The loopcount for Write Time recentering.
  @param[in] OptParamLC - The loopcount for training the Opt Param test list.
  @param[in] Recenter   - Switch which determines if the step recenters Write Timing.

  @retval If it succeeds return mrcSuccess
**/
extern
MrcStatus
TrainWriteDriveStrength (
  IN MrcParameters *const MrcData,
  IN const U8             ChBitMask,
  IN const U8             RecenterLC,
  IN const U8             OptParamLC,
  IN const BOOL           Recenter
  );

/**
  This step performs Comp Offset optimization on the param list defined in this function.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - mrcSuccess
**/
extern
MrcStatus
MrcOptimizeComp (
  IN MrcParameters *const MrcData
  );

/**
  This function calculates the percent of power saving from the power optimization steps and
  updates the proper registers in the PCU.  To get the correct base line for this calculation,
  this routing needs to run first time early in the training in order to update the MrcStruct
  with the base line.  After the power training steps, it will run again to get the actual
  percent of power saving.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - mrcSuccess

**/
extern
MrcStatus
MrcPowerSavingMeter (
  IN MrcParameters *const MrcData
  );

#endif // _MrcCrosser_h_
