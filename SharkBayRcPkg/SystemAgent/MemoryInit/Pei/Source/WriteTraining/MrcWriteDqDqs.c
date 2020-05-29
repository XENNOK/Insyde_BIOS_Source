/** @file
  The third stage of the write training is determining the PI setting for
  each byte strobe to make sure that data is sent at the optimal location.
  In order to do that a pattern of alternating zeros and ones is written to
  a block of the memory, and then read out.  By identifying the location
  where it is farthest away from where errors are shown the DQS will be
  aligned to the center of the eye.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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

//
// Include files
//
#include <MrcWriteDqDqs.h>

/**
@brief
  this function executes the write timing centering.
  Center Tx DQS-DQ using moderate pattern with 1D eye.

  @param[in] MrcData         - Include all MRC global data.

  @retval MrcStatus      - if it succeded returns mrcSuccess
**/
MrcStatus
MrcWriteTimingCentering (
  IN     MrcParameters *const MrcData
  )
{
  MrcOutput *Outputs;
  U8        ResetPerbit;
  U8        LoopCount;

  Outputs = &MrcData->SysOut.Outputs;
  ResetPerbit = 1;

  LoopCount   = 10;

  return DQTimeCentering1D (MrcData, Outputs->ValidChBitMask, WrT, ResetPerbit, LoopCount);
}

/**
  @brief
  this function executes the write timing centering in 2D.
    Final write timing centering using 2D algorithm and per bit optimization.

    @param[in] MrcData - Include all MRC global data.

    @retval MrcStatus -  if it succeded returns mrcSuccess

  **/
MrcStatus
MrcWriteTimingCentering2D (
  IN     MrcParameters *const MrcData
  )
{
  const MrcDebug  *Debug;
  MrcOutput       *Outputs;
  MrcStatus       Status;
  U8              EnPerBit;
  U8              EnRxDutyCycle;
  U8              ResetPerBit;
  U8              LoopCount;
  U8              En2D;

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  EnPerBit      = 1;
  EnRxDutyCycle = 0;
  ResetPerBit   = 1;
  LoopCount     = 15;
  En2D          = 0;

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "\n\nCalling with EnRxDutyCycle = %d, EnPerBit = %d, ResetPerBit = %d En2D = %d\n",
    EnRxDutyCycle,
    EnPerBit,
    ResetPerBit,
    En2D
    );

  Status = DataTimeCentering2D (
            MrcData,
            Outputs->MarginResult,
            Outputs->ValidChBitMask,
            WrT,
            EnPerBit,
            EnRxDutyCycle,
            ResetPerBit,
            LoopCount,
            En2D
            );

  if (mrcSuccess == Status) {
    EnPerBit    = 0;
    ResetPerBit = 0;
    En2D        = 1;
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "\n\nCalling with EnRxDutyCycle = %d, EnPerBit = %d, ResetPerBit = %d En2D = %d\n",
      EnRxDutyCycle,
      EnPerBit,
      ResetPerBit,
      En2D
      );

    Status = DataTimeCentering2D (
              MrcData,
              Outputs->MarginResult,
              Outputs->ValidChBitMask,
              WrT,
              EnPerBit,
              EnRxDutyCycle,
              ResetPerBit,
              LoopCount,
              En2D
              );
  }

  return Status;
}

/**
@brief
  Rank Margin Tool - Measure margins across various parameters

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus -  mrcSuccess if succeded
**/
MrcStatus
MrcRankMarginTool (
  IN OUT MrcParameters *const MrcData
  )
{
  const U16                           mode = 0;
  const MrcInput                      *Inputs;
  const MrcDebug                      *Debug;
  MrcOutput                           *Outputs;
  MrcControllerOut                    *ControllerOut;
  MrcChannelOut                       *ChannelOut;
  MrcCommandMargin                    *CommandOut;
  MrcRecvEnMargin                     *RecvEnOut;
  MrcWrLevelMargin                    *WrLevelOut;
  MrcStatus                           Status;
  U32                                 BERStats[4];
  U32                                 Offset;
  U8                                  Rank;
  U8                                  Param;
  U8                                  RankMask;
  U8                                  Controller;
  U8                                  Channel;
  U8                                  byte;
  U8                                  bit;
  U8                                  chBitMask;
  U8                                  MaxMargin;
  U8                                  DqLoopCount;
  U8                                  CmdLoopCount;
  S8                                  VrefOffsets[2];
  BOOL                                Lpddr;
  BOOL                                SkipVref;
  BOOL                                SkipPrint;
  MrcPower                            PwrChRank[MAX_CHANNEL][MAX_RANK_IN_CHANNEL];
  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;
  Outputs = &MrcData->SysOut.Outputs;
  Status  = mrcSuccess;
  MrcOemMemorySet ((U8 *) BERStats, 0, sizeof (BERStats));
  MrcOemMemorySet ((U8 *) VrefOffsets, 0, sizeof (VrefOffsets));
  MrcOemMemorySet ((U8 *) PwrChRank, 0, sizeof (PwrChRank));

  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);


  DqLoopCount = 17;
  CmdLoopCount = (Lpddr) ? 10 : 17;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Rank Margin Testing: DQ LC = %d, Cmd LC = %d\n\n", DqLoopCount, CmdLoopCount);
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Margin\nParams: RcvEna\tWrLevel\tRdT\tWrT\tRdV\tWrV\tCmdT\tCmdV\tDimmPwr\tCpuPwr\tTotPwr\n"
    );
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "\tLft Rgt Lft Rgt Lft Rgt Lft Rgt Low Hi  Low Hi  Lft Rgt Low Hi\t[mW]\t[mW]\t[mW]\n"
    );
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    //
    // Select rank for REUT test
    //
    RankMask  = 1 << Rank;
    chBitMask = 0;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      chBitMask |= SelectReutRanks (MrcData, Channel, RankMask, 0);
      if ((MRC_BIT0 << Channel) & chBitMask) {
        ChannelOut = &Outputs->Controller[0].Channel[Channel];
        //
        // Clear any old state in DataOffsetTrain
        //
        MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
      }
    }
    //
    // Continue with next rank if this rank is not present on any channel
    //
    if (!(chBitMask)) {
      continue;
    }
    //
    // Setup Test
    // SOE=1, EnCADB=0, EnCKE=0 SOE=1 sets bit12 of REUT_CH_ERR_CTL
    //
    SetupIOTestBasicVA (MrcData, chBitMask, DqLoopCount, NSOE, 0, 0, 8);
    for (Param = RcvEna; Param <= WrLevel; Param++) {
      if (Param == WrDqsT) {
        continue;
      }

      //
      // For Write/Read timing margining, we want to run traffic with Rd->Rd turnaround times of 4 and 5.
      // This statement depends on the order of MRC_MarginTypes.  If this enum's order changes, this
      // statement must change.
      //
      if (Param == RdT) {
        Outputs->DQPat = RdRdTA;
      } else if (Param == RdV) {
        Outputs->DQPat = BasicVA;
      }

      MaxMargin = ((Param == RdV) || (Param == WrV)) ? MAX_POSSIBLE_VREF : MAX_POSSIBLE_TIME;

      //
      // Run test for different Params
      //
      Status = MrcGetBERMarginCh (
                MrcData,
                Outputs->MarginResult,
                chBitMask,
                0xFF,
                Rank,
                Param,
                mode,
                1,
                MaxMargin,
                0,
                BERStats
                );
    }

    //
    // Use CADB test for Cmd to match Late Command Training
    //
    SetupIOTestCADB (MrcData, chBitMask, CmdLoopCount, NSOE, 1, 0);

    //
    // Run test for Cmd Timing
    //
    SkipVref  = TRUE;
    SkipPrint = TRUE;

#ifdef ULT_FLAG
    if (Lpddr) {
      CmdLinearFindEdgesLpddr (MrcData, MrcIterationClock, chBitMask, RankMask, !SkipPrint);
    } else
#endif // ULT_FLAG
    {
      CmdLinearFindEdges (
        MrcData,
        MrcIterationClock,
        chBitMask,
        0xFF,
        3,
        -64,
        64,
        1,
        VrefOffsets,
        SkipPrint,
        SkipVref
        );
    }

    //
    // Restore centered value
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationClock, 0xFF, 3, 0, 0);
    }

    Status = MrcResetSequence (MrcData);

    //
    // Run test for Cmd Voltage
    //
    Status = MrcGetBERMarginCh (
              MrcData,
              Outputs->MarginResult,
              chBitMask,
              0xFF,
              Rank,
              CmdV,
              mode,
              0,
              MAX_POSSIBLE_VREF,
              0,
              BERStats
              );
    Status = MrcResetSequence (MrcData);

    CalcSysPower(MrcData, PwrChRank);

#ifdef MRC_DEBUG_PRINT
    //
    // Print test results
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%dR%d:\t", Channel, Rank);
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%2d  %2d\t%2d  %2d\t%2d  %2d\t%2d  %2d\t%2d  %2d\t%2d  %2d\t%2d  %2d\t%2d  %2d\t%2d.%d\t%2d.%d\t%2d.%d\n",
          Outputs->MarginResult[LastRcvEna][Rank][Channel][0][0] / 10,
          Outputs->MarginResult[LastRcvEna][Rank][Channel][0][1] / 10,
          Outputs->MarginResult[LastWrLevel][Rank][Channel][0][0] / 10,
          Outputs->MarginResult[LastWrLevel][Rank][Channel][0][1] / 10,
          Outputs->MarginResult[LastRxT][Rank][Channel][0][0] / 10,
          Outputs->MarginResult[LastRxT][Rank][Channel][0][1] / 10,
          Outputs->MarginResult[LastTxT][Rank][Channel][0][0] / 10,
          Outputs->MarginResult[LastTxT][Rank][Channel][0][1] / 10,
          Outputs->MarginResult[LastRxV][Rank][Channel][0][0] / 10,
          Outputs->MarginResult[LastRxV][Rank][Channel][0][1] / 10,
          Outputs->MarginResult[LastTxV][Rank][Channel][0][0] / 10,
          Outputs->MarginResult[LastTxV][Rank][Channel][0][1] / 10,
          Outputs->MarginResult[LastCmdT][Rank][Channel][0][0] / 10,
          Outputs->MarginResult[LastCmdT][Rank][Channel][0][1] / 10,
          Outputs->MarginResult[LastCmdV][Rank][Channel][0][0] / 10,
          Outputs->MarginResult[LastCmdV][Rank][Channel][0][1] / 10,
          PwrChRank[Channel][Rank].DimmPwr / 10,
          PwrChRank[Channel][Rank].DimmPwr % 10,
          PwrChRank[Channel][Rank].CpuPower / 10,
          PwrChRank[Channel][Rank].CpuPower % 10,
          PwrChRank[Channel][Rank].TotPwr / 10,
          PwrChRank[Channel][Rank].TotPwr % 10
          );
      }
    }
#endif
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          ChannelOut        = &ControllerOut->Channel[Channel];
          CommandOut        = &ChannelOut->Command[Rank];
          CommandOut->Left  = (U8) (Outputs->MarginResult[LastCmdT][Rank][Channel][0][0] / 10);
          CommandOut->Right = (U8) (Outputs->MarginResult[LastCmdT][Rank][Channel][0][1] / 10);
          CommandOut->Low   = (U8) (Outputs->MarginResult[LastCmdV][Rank][Channel][0][0] / 10);
          CommandOut->High  = (U8) (Outputs->MarginResult[LastCmdV][Rank][Channel][0][1] / 10);
          RecvEnOut         = &ChannelOut->ReceiveEnable[Rank];
          RecvEnOut->Left   = (U8) (Outputs->MarginResult[LastRcvEna][Rank][Channel][0][0] / 10);
          RecvEnOut->Right  = (U8) (Outputs->MarginResult[LastRcvEna][Rank][Channel][0][1] / 10);
          WrLevelOut        = &ChannelOut->WriteLevel[Rank];
          WrLevelOut->Left  = (U8) (Outputs->MarginResult[LastWrLevel][Rank][Channel][0][0] / 10);
          WrLevelOut->Right = (U8) (Outputs->MarginResult[LastWrLevel][Rank][Channel][0][1] / 10);
          for (byte = 0; byte < Outputs->SdramCount; byte++) {
            for (bit = 0; bit < MAX_BITS; bit++) {
              ChannelOut->RxDqPb[Rank][byte][bit].Left      = (U8) (Outputs->MarginResult[LastRxT][Rank][Channel][0][0] / 10);
              ChannelOut->RxDqPb[Rank][byte][bit].Right     = (U8) (Outputs->MarginResult[LastRxT][Rank][Channel][0][1] / 10);
              ChannelOut->TxDqPb[Rank][byte][bit].Left      = (U8) (Outputs->MarginResult[LastTxT][Rank][Channel][0][0] / 10);
              ChannelOut->TxDqPb[Rank][byte][bit].Right     = (U8) (Outputs->MarginResult[LastTxT][Rank][Channel][0][1] / 10);
              ChannelOut->RxDqVrefPb[Rank][byte][bit].Low   = (U8) (Outputs->MarginResult[LastRxV][Rank][Channel][0][0] / 10);
              ChannelOut->RxDqVrefPb[Rank][byte][bit].High  = (U8) (Outputs->MarginResult[LastRxV][Rank][Channel][0][1] / 10);
              ChannelOut->TxDqVrefPb[Rank][byte][bit].Low   = (U8) (Outputs->MarginResult[LastTxV][Rank][Channel][0][0] / 10);
              ChannelOut->TxDqVrefPb[Rank][byte][bit].High  = (U8) (Outputs->MarginResult[LastTxV][Rank][Channel][0][1] / 10);
            }
          }
        }
      }
    }
  } // for Rank

  //
  // Disable CADB Deselects after RMT
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_REG) * Channel);
      MrcWriteCR8 (MrcData, Offset, 0);
    }
  }
  return Status;
}

/**
@brief
  Peform Read Voltage Centering in 2D.
  Note: This function currently only supports param = WrV

  @param[in, out] MrcData       - Include all MRC global data.

  @retval MrcStatus -    if t succeded return mrcSuccess
  @todo - Need option for loopcount
**/
MrcStatus
MrcWriteVoltageCentering2D (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcDebug  *Debug;
  const MrcInput  *Inputs;
  MrcOutput       *Outputs;
  MrcChannelOut   *ChannelOut;
  MrcStatus       Status;
  U32 (*marginch)[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES];

  U8  ResultType;
  U8  ResultTypeT;
  U8  loopcount;
  U8  param;
  U8  paramT;
  U8  Channel;
  U8  byte;
  U8  tim;
  U8  chBitMask;
  U8  MaxTscale;
  U8  SkipWait;
  S8  SumEH;
  S8  SumEHSign;
  S8  TimePoints[3];
  U8  EHWeights[sizeof (TimePoints)];
  U16 mode;
  S32 center;
  S32 height;
  U32 value0[MAX_CHANNEL];
  U32 BERStats[4];
  U32 TimScale[MAX_CHANNEL];
  S32 centersum[MAX_CHANNEL];

  Inputs    = &MrcData->SysIn.Inputs;
  Outputs   = &MrcData->SysOut.Outputs;
  Debug     = &Inputs->Debug;
  marginch  = &Outputs->MarginResult;
  Status    = mrcSuccess;
  loopcount = 17;
  MaxTscale = 12;
  SumEH     = 0;
  MrcOemMemorySet ((U8 *) BERStats, 0, sizeof (BERStats));
  MrcOemMemorySet ((U8 *) EHWeights, 1, sizeof (EHWeights));
  TimePoints[0] = -4;
  TimePoints[1] = 0;
  TimePoints[2] = 4;

  //
  // No input for param so set it to RdV
  //
  param = WrV;

  //
  // Assume rank0 is always popuplated
  //
  if (param == WrV) {
    paramT = WrT;
  } else {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Error Handler: Unknown Margin Parameter\n");
    Status = mrcFail;
    return Status;
  }

  ResultType  = GetMarginResultType (param);
  ResultTypeT = GetMarginResultType (paramT);

  //
  /// @todo: Need to check if we can enable it for A0 or not
  // Outputs->EnDumRd = 1;
  // SOE = 00b(No Stop on error), EnCADB=0, EnCKE=0
  //
  //
  /// @todo: Will enable the DQ tests instead of basic in the future
  //  SetupIOTestDQ (MrcData,Outputs->ValidChBitMask, loopcount, NSOE, 0, 0);
  //
  SetupIOTestBasicVA (MrcData, Outputs->ValidChBitMask, loopcount, NSOE, 0, 0, 8);

  //
  // Calculate SumEH for use in weighting equations
  //
  for (tim = 0; tim < sizeof (TimePoints); tim++) {
    SumEH += EHWeights[tim];
  }
  //
  // Select rank for REUT test
  //
  chBitMask = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }

    ChannelOut = &Outputs->Controller[0].Channel[Channel];
    chBitMask |= SelectReutRanks (MrcData, Channel, ChannelOut->ValidRankBitMask, 0);

    //
    // Clear any old state in DataTrain Offset
    //
    MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
  }
  //
  // ####################################################
  // #############  Initialize EW/EH variables   ########
  // ####################################################
  //
  Status = GetMarginCh (MrcData, Outputs->MarginResult, paramT, 0xF);

  //
  // Update TimScale with results
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    TimScale[Channel] = 0;
    if (!(chBitMask & (1 << Channel))) {
      continue;
    }

    TimScale[Channel] = ((*marginch)[ResultTypeT][0][Channel][0][0] + (*marginch)[ResultTypeT][0][Channel][0][1]) / 20;
    //
    // It is possible TimScale[Channel] is 0.
    //
    if (!TimScale[Channel] || (TimScale[Channel] > MaxTscale)) {
      TimScale[Channel] = MaxTscale;
    }
  }

  Status = GetMarginCh (MrcData, Outputs->MarginResult, param, 0xF);

  //
  // ####################################################
  // ######   Measure Eye Height at all Timing Points  #####
  // ####################################################
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TimScale{0] is %d, TimScale{1] is %d\n", TimScale[0], TimScale[1]);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel\t0\t\t\t\t1\n");

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "WrTime\tLow\tHigh\tHeight\tCenter\t");
  }
  //
  // Initialize parameters to 0
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    centersum[Channel] = 0;
    value0[Channel]    = 0;
  }
  //
  // Loop through all the Time Points to Test
  //
  for (tim = 0; tim < sizeof (TimePoints); tim++) {
    //
    // Setup Timing Offset for this point
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(chBitMask & (1 << Channel))) {
        continue;
      }

      value0[Channel] = (S32) (TimePoints[tim] * TimScale[Channel]) / MaxTscale;

      //
      // There is no multicast per channel...
      //
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        Status = ChangeMargin (MrcData, paramT, value0[Channel], 0, 0, Channel, 0, byte, 0, 1, 0, MrcRegFileStart);
      }
    }
    //
    // Run Margin Test
    //
    mode = 0;
    Status = MrcGetBERMarginCh (
              MrcData,
              Outputs->MarginResult,
              chBitMask,
              0xFF,
              0,
              param,
              mode,
              1,
              MAX_POSSIBLE_VREF,
              0,
              BERStats
              );

    //
    // Store Results
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(chBitMask & (1 << Channel))) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t");
        continue;
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", value0[Channel]);

      height  = ((*marginch)[ResultType][0][Channel][0][1] + (*marginch)[ResultType][0][Channel][0][0]) / 10;
      center  = (S32) ((*marginch)[ResultType][0][Channel][0][1] - (*marginch)[ResultType][0][Channel][0][0]);
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "%d\t%d\t%d\t%d\t",
        (*marginch)[ResultType][0][Channel][0][0] / 10,
        (*marginch)[ResultType][0][Channel][0][1] / 10,
        height,
        center / 20
        );
      if (tim == 0) {
        centersum[Channel] = 0;
      }
      //
      // Calculate weight for this point
      //
      centersum[Channel] += EHWeights[tim] * center;
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "+----->centersum[%d] = %d, \n", Channel, centersum[Channel]);
      //
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nWtdCntr\t");
  //
  // ####################################################
  // ###########   Center Results per Ch    #############
  // ####################################################
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(chBitMask & (1 << Channel))) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t");
      continue;
    }

    ChannelOut = &Outputs->Controller[0].Channel[Channel];

    //
    // Calculate CenterPoint.  Round to Nearest Int
    //
    SumEHSign           = (centersum[Channel] < 0) ? (-1) : 1;

    centersum[Channel]  = (centersum[Channel] + 10 * SumEH * SumEHSign) / (20 * SumEH);

    //
    // Apply new centerpoint
    // Only Byte 7 on Channel 1 is needed to update DIMM Vref
    // Skip if there are more channels
    //
    SkipWait = (chBitMask >> (Channel + 1));
    Status = ChangeMargin (MrcData, param, centersum[Channel], 0, 0, Channel, 0, 7, 0, 1, SkipWait, MrcRegFileCurrent);

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t\t\t\t", centersum[Channel]);

    //
    // Update MrcData for future tests
    //
    (*marginch)[ResultType][0][Channel][0][0] = (S32) ((*marginch)[ResultType][0][Channel][0][0]) + (10 * (centersum[Channel]));
    (*marginch)[ResultType][0][Channel][0][1] = (S32) ((*marginch)[ResultType][0][Channel][0][1]) - (10 * (centersum[Channel]));

    //
    // Clean up after test
    //
    MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  //
  // Clean up
  //
  Status            = ChangeMargin (MrcData, paramT, 0, 0, 1, 0, 0, 0, 0, 1, 0, MrcRegFileCurrent);

  Outputs->EnDumRd  = 0;
  return Status;
}
