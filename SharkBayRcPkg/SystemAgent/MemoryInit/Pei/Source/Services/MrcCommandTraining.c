/** @file
  Implementation of the command training algorithm.
  The algorithm finds the N mode for the current board and also the correct
  CLK CMD CTL pi setting.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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
#include "MrcCommandTraining.h"

#define MRC_CADB_PB_LENGTH 16

/**
@brief
  This function performs early command training.
  Center CTL-CLK timing to allow subsequent steps to work

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus -  mrcSuccess if it succeeded
**/
MrcStatus
MrcEarlyCommandTraining (
  IN MrcParameters *const MrcData
  )
{
  const MrcInput                              *Inputs;
  const MrcDebug                              *Debug;
  MrcOutput                                   *Outputs;
  MrcControllerOut                            *ControllerOut;
  MrcChannelOut                               *ChannelOut;
  S32                                         *IPStart;
  S32                                         *IPEnd;
  S32                                         *CPStart;
  S32                                         *CPEnd;
  S32                                         *LPStart;
  S32                                         *LPEnd;
  MrcStatus                                   Status;
  BOOL                                        Pass;
  BOOL                                        Done;
  DDRCLK_CR_DDRCRCLKPICODE_STRUCT             DdrCrClkPiCode;
  MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_STRUCT  ReutChMiscOdtCtrl;
  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT     ReutGlobalCtl;
  DDRDATA0CH0_CR_DDRCRDATACONTROL0_STRUCT     DdrCrDataControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT     DdrCrDataControl2;
  U32                                         CRValue;
  S32                                         cWidth;
  S32                                         lWidth;
  S32                                         InitialPassingStart[MAX_CHANNEL][MAX_RANK_IN_CHANNEL];
  S32                                         InitialPassingEnd[MAX_CHANNEL][MAX_RANK_IN_CHANNEL];
  S32                                         CurrentPassingStart[MAX_CHANNEL][MAX_RANK_IN_CHANNEL];
  S32                                         CurrentPassingEnd[MAX_CHANNEL][MAX_RANK_IN_CHANNEL];
  S32                                         LargestPassingStart[MAX_CHANNEL][MAX_RANK_IN_CHANNEL];
  S32                                         LargestPassingEnd[MAX_CHANNEL][MAX_RANK_IN_CHANNEL];
  U32                                         DqsDoneMask;
  U32                                         bytePass[MAX_CHANNEL];
  U32                                         byteFail[MAX_CHANNEL];
  U32                                         Offset;
  U8                                          Channel;
  U8                                          Rank;
  U8                                          byte;
  U8                                          chBitMask;
  U8                                          RankMask;
  U8                                          ValidRankMask;
  U8                                          clkDelay;
  U8                                          clkDelayArray;
  U8                                          PiCode;
  S8                                          DqsDelay;
  S8                                          LastDqsRan[ECT_CLK_LOOPS][MAX_CHANNEL][MAX_RANK_IN_CHANNEL];

  MrcOemMemorySet ((U8 *) LastDqsRan, ECT_DQS_STOP, sizeof (LastDqsRan));

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  //
  // RankBitMask for both channels
  //
  ValidRankMask = Outputs->ValidRankMask;
  //
  // Channel bit mask
  //
  chBitMask = Outputs->ValidChBitMask;

#ifdef ULT_FLAG
  //
  // Check if LPDDR3 memory is used
  //
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    return EarlyCommandTrainingLpddr (MrcData);
  }
#endif // ULT_FLAG

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];

      //
      // Set DQS Delay to 32
      //
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          if ((MrcRankInChannelExist (MrcData, Rank, Channel)) && (Inputs->SetRxDqs32 == TRUE)) {
            //
            // Update RxDqsP & RxDqsN - leave other parameter the same; can we update in the next loop or do it per channel
            //
            UpdateRxT (MrcData, Channel, Rank, byte, 5, 32);
          }
        }
      }
      //
      // Clear any old state in DataTrain Offset
      //
      MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
    }
  }
  //
  // Setup REUT Engine
  // LC = 10, SOE = 0 (NSOE), EnCADB = 0, EnCKE = 0
  //
  SetupIOTestMPR (MrcData, chBitMask, 10, NSOE, 0, 0);

#ifdef MRC_DEBUG_PRINT
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nChannel\t");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%u\t\t\t\t", Channel);
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRank\t");

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%u", Rank);
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nClock");
#endif // MRC_DEBUG_PRINT
  for (clkDelay = ECT_CLK_START; clkDelay < ECT_CLK_STOP; clkDelay += ECT_CLK_STEP) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n% 5u\t", clkDelay);
    clkDelayArray = clkDelay / ECT_CLK_STEP;
    //
    // Program Clock Delays
    //
    DdrCrClkPiCode.Data = 0;
    DdrCrClkPiCode.Bits.PiSettingRank0 =
    DdrCrClkPiCode.Bits.PiSettingRank1 =
    DdrCrClkPiCode.Bits.PiSettingRank2 =
    DdrCrClkPiCode.Bits.PiSettingRank3 = clkDelay;
    MrcWriteCrMulticast (MrcData, DDRCLK_CR_DDRCRCLKPICODE_REG, DdrCrClkPiCode.Data);

    //
    // Reset FIFOs and Reset all DIMM/all channels after changing PI codes
    //
    Status = MrcResetSequence (MrcData);

    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      RankMask = MRC_BIT0 << Rank;
      //
      // Skip ranks that are not populated
      //
      if ((ValidRankMask & RankMask) == 0) {
        continue;
      }
      //
      // Program MR3 and Mask RAS/WE to prevent scheduler from issuing non-Read commands
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        SelectReutRanks (MrcData, Channel, RankMask, 0);
        bytePass[Channel] = 0;
        byteFail[Channel] = 0;
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          Status = MrcWriteMRS (MrcData, Channel, RankMask, mrMR3, 4);

          Offset = MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG +
            ((MCHBAR_CH1_CR_REUT_CH_MISC_ODT_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG) * Channel);
          ReutChMiscOdtCtrl.Data                  = MrcReadCR (MrcData, Offset);
          ReutChMiscOdtCtrl.Bits.MPR_Train_DDR_On = 1;
          MrcWriteCR (MrcData, Offset, ReutChMiscOdtCtrl.Data);
        }
      }
      //
      // Run ReadDQS Test
      //
      DqsDoneMask = (MRC_BIT0 << Outputs->SdramCount) - 1;
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRank %u DqsDelay for clkDelay = %u", Rank, clkDelay);
      //
      for (DqsDelay = ECT_DQS_START; DqsDelay < ECT_DQS_STOP; DqsDelay += ECT_DQS_STEP) {
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n %d\t", DqsDelay);
        //
        // Write DqsDelay
        //
        Status = ChangeMargin (MrcData, RdT, DqsDelay, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileStart);

        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
            /*MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              (Channel != 0) ? "" : ((Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ?
              "                  " : "                ")
              );*/
          } else {
            ChannelOut = &ControllerOut->Channel[Channel];

            for (byte = 0; byte < Outputs->SdramCount; byte++) {
              //
              // Force on SenseAmp
              //
              Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
                ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
                ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * byte);
              DdrCrDataControl2.Data             = ChannelOut->DqControl2[byte].Data;
              DdrCrDataControl2.Bits.ForceBiasOn = DDRDATA0CH0_CR_DDRCRDATACONTROL2_ForceBiasOn_MAX;
              DdrCrDataControl2.Bits.ForceRxOn   = DDRDATA0CH0_CR_DDRCRDATACONTROL2_ForceRxOn_MAX;
              MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl2.Data);
            }
            //
            // Enable RX Training mode.  Turn on Odt
            //
            Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
              ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
            DdrCrDataControl0.Data                = ChannelOut->DqControl0.Data;
            DdrCrDataControl0.Bits.ForceOdtOn     = DDRDATACH0_CR_DDRCRDATACONTROL0_ForceOdtOn_MAX;
            DdrCrDataControl0.Bits.RxTrainingMode = DDRDATACH0_CR_DDRCRDATACONTROL0_RxTrainingMode_MAX;
            MrcWriteCR (MrcData, Offset, DdrCrDataControl0.Data);
          }
        }
        //
        // Clear Results for Prior Test and wait to obtain results
        //
        Status = IoReset (MrcData);

        //
        // Start REUT and run for 1uS
        //
        ReutGlobalCtl.Data                     = 0;
        ReutGlobalCtl.Bits.Global_Clear_Errors = 1;
        ReutGlobalCtl.Bits.Global_Start_Test   = 1;
        MrcWriteCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, ReutGlobalCtl.Data);

        //
        // Wait for test to start clearing errors.
        //
        MrcWait (MrcData, START_TEST_DELAY);

        //
        // Clear Results for Prior Test and wait to obtain results
        //
        Status = IoReset (MrcData);
        MrcWait (MrcData, IO_RESET_DELAY);

        //
        // Stop REUT
        //
        ReutGlobalCtl.Data                  = 0;
        ReutGlobalCtl.Bits.Global_Stop_Test = 1;
        MrcWriteCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, ReutGlobalCtl.Data);

        //
        // Get Results for all ch/bytes
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
            for (byte = 0; byte < Outputs->SdramCount; byte++) {
              Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
                ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
                ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * byte);
              Pass = (MrcReadCR (MrcData, Offset) == 1);
              //
              // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, Pass ? ". " : "# ");
              //
              CRValue = (MRC_BIT0 << byte);
              if (Pass) {
                bytePass[Channel] |= CRValue;
              } else {
                byteFail[Channel] |= CRValue;
              }
            }
            //
            // Save DqsDelay where all bytes passed
            //
            if ((bytePass[Channel] == DqsDoneMask) && (LastDqsRan[clkDelayArray][Channel][Rank] > DqsDelay)) {
              LastDqsRan[clkDelayArray][Channel][Rank] = DqsDelay;
            }
          }
        }

        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
            ChannelOut = &ControllerOut->Channel[Channel];
            for (byte = 0; byte < Outputs->SdramCount; byte++) {
              //
              // Restore orginal value
              //
              Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
                ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
                ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * byte);
              MrcWriteCR (MrcData, Offset, ChannelOut->DqControl2[byte].Data);
            }

            Status = IoReset (MrcData);

            //
            // Clear RX Mode
            //
            Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
              ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
            MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0.Data);
          }
        }
        //
        // Are We done yet?
        //
        Done = TRUE;
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
            if ((bytePass[Channel] != DqsDoneMask) || (byteFail[Channel] != DqsDoneMask)) {
              Done = FALSE;
              break;
            }
          }
        }
        //
        // If we re done, we passed or failed for all bytes
        //
        if (Done == TRUE) {
          break;
        }
      }
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
      //
      // Update results for all channel at this rank
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          Pass = ((bytePass[Channel] == DqsDoneMask) && (byteFail[Channel] == DqsDoneMask));

          //
          // Check if we have a valid pass
          //
          if (Pass &&
              (clkDelay != ECT_CLK_START) &&
              ((LastDqsRan[clkDelayArray][Channel][Rank] - LastDqsRan[clkDelayArray - 1][Channel][Rank]) > 16)
              ) {
            Pass = FALSE;
          }

          IPStart = &InitialPassingStart[Channel][Rank];
          IPEnd   = &InitialPassingEnd[Channel][Rank];
          CPStart = &CurrentPassingStart[Channel][Rank];
          CPEnd   = &CurrentPassingEnd[Channel][Rank];
          LPStart = &LargestPassingStart[Channel][Rank];
          LPEnd   = &LargestPassingEnd[Channel][Rank];
          if (clkDelay == ECT_CLK_START) {
            if (Pass) {
              *IPStart = clkDelay;
              *IPEnd   = clkDelay;
              *CPStart = clkDelay;
              *CPEnd   = clkDelay;
              *LPStart = clkDelay;
              *LPEnd   = clkDelay;
            } else {
              *IPStart = -ECT_CLK_STEP;
              *IPEnd   = -ECT_CLK_STEP;
              *CPStart = -ECT_CLK_STEP;
              *CPEnd   = -ECT_CLK_STEP;
              *LPStart = -ECT_CLK_STEP;
              *LPEnd   = -ECT_CLK_STEP;
            }
          } else {
            if (Pass) {
              //
              // Update Initial variables
              //
              if (*IPEnd == clkDelay - ECT_CLK_STEP) {
                *IPEnd = clkDelay;          // In passing region
              }
              //
              // Update Current variables
              //
              if (*CPEnd == clkDelay - ECT_CLK_STEP) {
                *CPEnd = clkDelay;          // In passing region
              } else {
                *CPStart = clkDelay;        // New region
                *CPEnd = clkDelay;
              }
              //
              // Special case for last step: Append Initial Passing Region
              // clkDelay should be considered a continuous range that wraps around 0
              //
              if (clkDelay == 128 - ECT_CLK_STEP && *IPStart == ECT_CLK_START && *IPEnd != clkDelay) {
                *CPEnd += ECT_CLK_STEP + (*IPEnd -*IPStart);
              }
              //
              // Update Largest variables
              //
              cWidth = *CPEnd - *CPStart;
              lWidth = *LPEnd - *LPStart;
              if (cWidth > lWidth) {
                *LPStart  = *CPStart;
                *LPEnd    = *CPEnd;
              }
            }
          }
        }
      }
      //
      // Clean up registers.  No need to clear MR3 since DIMM will be reset
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          Offset = MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG +
            ((MCHBAR_CH1_CR_REUT_CH_MISC_ODT_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG) * Channel);
          ReutChMiscOdtCtrl.Data                  = MrcReadCR (MrcData, Offset);
          ReutChMiscOdtCtrl.Bits.MPR_Train_DDR_On = 0;
          MrcWriteCR (MrcData, Offset, ReutChMiscOdtCtrl.Data);
        }
      }
    }

#ifdef MRC_DEBUG_PRINT
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
            CPEnd = &CurrentPassingEnd[Channel][Rank];
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              (LastDqsRan[clkDelayArray][Channel][Rank] < ECT_DQS_STOP) ? ". " : "# "
              );
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "(% 3d)", LastDqsRan[clkDelayArray][Channel][Rank]);
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
        }
      }
    }
#endif // MRC_DEBUG_PRINT
  }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\tLeft\tRight\tWidth\tClkDelay\n");
  //
  // Find largest passing region and Update PICodes
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut          = &ControllerOut->Channel[Channel];
      DdrCrClkPiCode.Data = 0;
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          LPStart = &LargestPassingStart[Channel][Rank];
          LPEnd   = &LargestPassingEnd[Channel][Rank];
          lWidth  = *LPEnd - *LPStart;

          //
          // Error Handler if eye not found for all bytes
          //
          if (lWidth == 0) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "\nERROR!! NO EYE found for Channel:%u Rank:%u\n", Channel, Rank);
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "Channel %u Rank %u:\t%d\t%d\t%d\t%d\n",
              Channel,
              Rank,
              *LPStart,
              *LPEnd,
              lWidth,
              0
              );
            return mrcReadMPRErr;
          }

          if (lWidth > ECT_MIN_WIDTH) {
            PiCode = (U8) (*LPStart + lWidth / 2);
          } else {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_WARNING,
              "\nWARNING!! lWidth <= %u for Channel %u Rank %u \n",
              ECT_MIN_WIDTH,
              Channel,
              Rank
              );
            PiCode = 64;
          }
          //
          // Update Host Structure with new PiCode
          //
          switch (Rank) {
            case 0:
              DdrCrClkPiCode.Bits.PiSettingRank0 = PiCode;
              break;

            case 1:
              DdrCrClkPiCode.Bits.PiSettingRank1 = PiCode;
              break;

            case 2:
              DdrCrClkPiCode.Bits.PiSettingRank2 = PiCode;
              break;

            case 3:
              DdrCrClkPiCode.Bits.PiSettingRank3 = PiCode;
              break;

            default:
              break;
          }

          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "C%u.R%u:\t%d\t%d\t%d\t%d\n",
            Channel,
            Rank,
            *LPStart,
            *LPEnd,
            lWidth,
            PiCode
            );
        }
      }

      Offset = DDRCLKCH0_CR_DDRCRCLKPICODE_REG +
        ((DDRCLKCH1_CR_DDRCRCLKPICODE_REG - DDRCLKCH0_CR_DDRCRCLKPICODE_REG) * Channel);
      MrcWriteCR (MrcData, Offset, DdrCrClkPiCode.Data);
      MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
    }
  }

  Status = ChangeMargin (MrcData, RdT, 0, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileCurrent);
  Status = MrcResetSequence (MrcData);

  return Status;
}

/**
@brief
  This function performs Late command training.
  Center CMD/CTL-CLK timing using complex patterns.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it's a success return mrcSuccess
**/
MrcStatus
MrcLateCommandTraining (
  MrcParameters *const MrcData
  )
{
  const MrcDebug                      *Debug;
  MrcOutput                           *Outputs;
  MrcControllerOut                    *ControllerOut;
  MrcChannelOut                       *ChannelOut;
  MrcStatus                           Status;
  MrcProfile                          Profile;
  U32                                 MinCode;
  U32                                 Offset;
  U8                                  Cmd2N;
  U8                                  Channel;
  U8                                  ChBitMask;
  U8                                  RankMask;
  U8                                  Rank;
  U8                                  Ranks;
  U8                                  CmdPiCode[MAX_CHANNEL];
  U8                                  CtlPiCode[MAX_CHANNEL];
#ifdef ULT_FLAG
  BOOL                                Lpddr;
  U8                                  MidPointCke[MAX_CHANNEL];
  U8                                  MidPointCmdN[MAX_CHANNEL];
  U8                                  MidPointCmdS[MAX_CHANNEL];
  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_STRUCT  ReutChSeqBaseAddrOrderCarryInvertCtl;
  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_STRUCT ReutChSeqBaseAddrIncCtl;
  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_STRUCT              ReutChPatCadbCtrl;
  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_STRUCT    ReutChSeqBaseAddrWrap;
#endif //ULT_FLAG

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Profile       = MrcData->SysIn.Inputs.MemoryProfile;

#ifdef ULT_FLAG
  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);
#endif // ULT_FLAG

  ChBitMask     = Outputs->ValidChBitMask;
  RankMask      = Outputs->ValidRankMask;

#ifdef ULT_FLAG
  if (Lpddr) {
    SetupIOTestCADB (MrcData, ChBitMask, 10, NTHSOE, 1, 0); // LC = 10
  } else
#endif //ULT_FLAG
  {
    Cmd2N = FALSE;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        Cmd2N = (ControllerOut->Channel[Channel].Timing[Profile].NMode == 2) ? TRUE : FALSE; // All channels have same NMode
        break;
      }
    }
    CmdPiCode[0] = CmdPiCode[1] = (Cmd2N == TRUE) ? 85 : 64;
    CtlPiCode[0] = CtlPiCode[1] = 64;

    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      " Cmd2N %d, CmdPiCode %d, ChBitMask = 0x%x\n",
      Cmd2N,
      CmdPiCode[0],
      ChBitMask
      );

    //
    // Setup REUT
    // LC= 10, SOE  = 1 (NTHSOE), EnCADB = 1, EnCKE = 0
    //
    SetupIOTestCADB (MrcData, ChBitMask, 10, NTHSOE, 1, 0);
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcChannelExist (Outputs, Channel))) {
        continue;
      }

      ChannelOut = &ControllerOut->Channel[Channel];

      //
      // Shift everything to the right.  To get DQ timing right, program Clk to 0
      //
      ShiftPIforCmdTraining (
        MrcData,
        Channel,
        MrcIterationClock,
        ChannelOut->ValidRankBitMask,
        1,
        0 - ChannelOut->ClkPiCode[0],
        1
        );
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, ChannelOut->ValidRankBitMask, 1, CmdPiCode[Channel], 1);
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdN, ChannelOut->ValidRankBitMask, 1, CmdPiCode[Channel], 1);
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCtl,  ChannelOut->ValidRankBitMask, 1, CtlPiCode[Channel], 1);
    }
  }

#ifdef ULT_FLAG

  if (Lpddr) {
    //
    // Center Command Timing
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** ECT results\n");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &ControllerOut->Channel[Channel];
      MidPointCke[Channel]  = (U8) ChannelOut->CkeCmdPiCode[0];
      MidPointCmdS[Channel] = (U8) ChannelOut->CmdsCmdPiCode[0];
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "C%u: CAA: CKE fub: %d, CmdS fub: %d\n",
        Channel,
        MidPointCke[Channel],
        MidPointCmdS[Channel]
        );
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** Center CAA[5,6,7,8,9] Timing using CKE fub\n");
    CmdTimingCentering (MrcData, MrcIterationCke, RankMask, 1, MidPointCke);

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** Center CAA[0,1,2,3,4] Timing using CmdS fub\n");
    CmdTimingCentering (MrcData, MrcIterationCmdS, RankMask, 1, MidPointCmdS);

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** ECT results\n");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &ControllerOut->Channel[Channel];
      MidPointCmdS[Channel] = (U8)ChannelOut->CmdsCmdPiCode[1];
      MidPointCmdN[Channel] = (U8)ChannelOut->CmdnCmdPiCode[1];
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "C%u: CAB: CmdS fub: %d, CmdN fub: %d\n",
        Channel,
        MidPointCmdS[Channel],
        MidPointCmdN[Channel]
        );
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** Center CAB[5,8] Timing using CmdS fub\n");
    CmdTimingCentering (MrcData, MrcIterationCmdS, RankMask, 2, MidPointCmdS);

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** Center CAB[0,1,2,3,4,6,7,9] Timing using CmdN fub\n");
    CmdTimingCentering (MrcData, MrcIterationCmdN, RankMask, 2, MidPointCmdN);

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** Center Control Timing\n");

    //
    // @todo Reinitialize registers to CAS-centric training (no CADB) ?
    //

    //
    // Modify the differences
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcChannelExist (Outputs, Channel)) {
        continue;
      }
      Offset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_REG) * Channel);
      ReutChSeqBaseAddrOrderCarryInvertCtl.Data = 0;
      MrcWriteCR (MrcData, Offset, ReutChSeqBaseAddrOrderCarryInvertCtl.Data);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d ReutChSeqBaseAddrOrderCarryInvertCtl: 0x%08X\n", Channel, ReutChSeqBaseAddrOrderCarryInvertCtl.Data);

      Offset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG) * Channel);
      ReutChSeqBaseAddrIncCtl.Data = 0;
      ReutChSeqBaseAddrIncCtl.Bits.Rank_Base_Address_Update_Scale   = 1;
      ReutChSeqBaseAddrIncCtl.Bits.Bank_Base_Address_Update_Scale   = 1;
      ReutChSeqBaseAddrIncCtl.Bits.Row_Base_Address_Update_Scale    = 1;
      ReutChSeqBaseAddrIncCtl.Bits.Column_Base_Address_Update_Scale = 1;
      ReutChSeqBaseAddrIncCtl.Bits.Column_Base_Address_Increment    = 1;
      MrcWriteCR64 (MrcData, Offset, ReutChSeqBaseAddrIncCtl.Data);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d ReutChSeqBaseAddrIncCtl: 0x%08X%08X\n", Channel, ReutChSeqBaseAddrIncCtl.Data32[1],
        ReutChSeqBaseAddrIncCtl.Data32[0]);

      Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_REG) * Channel);
      ReutChPatCadbCtrl.Data = 0;
      ReutChPatCadbCtrl.Bits.Lane_Deselect_Enable = 0xB; // All, except CMD
      ReutChPatCadbCtrl.Bits.CMD_Deselect_Start = 2;     // Start on RD
      MrcWriteCR (MrcData, Offset, ReutChPatCadbCtrl.Data);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d ReutChPatCadbCtrl: 0x%08X\n", Channel, ReutChPatCadbCtrl.Data);

      Offset = MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_MASK_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_MASK_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_MASK_MCMAIN_0_REG) * Channel);
      MrcWriteCR8 (MrcData, Offset, 0xFF);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d ReutChSeqDummyreadMask Offset:0x%X Value:0x%X\n", Channel, Offset, 0xFF);

      //
      // Start from logical Rank 0
      //
      Offset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG) * Channel);
      MrcWriteCR64 (MrcData, Offset, 0);

      //
      // Wrap at column 127
      // Logical Rank Wrap address will be updated in SelectReutRanks() later on.
      //
      Offset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG) * Channel);
      ReutChSeqBaseAddrWrap.Data = 0;
      ReutChSeqBaseAddrWrap.Bits.Column_Address = 0x7F;
      MrcWriteCR64 (MrcData, Offset, ReutChSeqBaseAddrWrap.Data);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d ReutChSeqBaseAddrWrap: 0x%08X%08X\n", Channel, ReutChSeqBaseAddrWrap.Data32[1],
        ReutChSeqBaseAddrWrap.Data32[0]);
    } // for Channel

    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      Ranks = (1 << Rank);
      if ((Ranks & RankMask) == 0) {
        continue;
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** ECT results\n");
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          CtlPiCode[Channel] = ControllerOut->Channel[Channel].CtlPiCode[Rank];
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%uR%u: CTL: %u\n", Channel, Rank, CtlPiCode[Channel]);
        } else {
          CtlPiCode[Channel] = 0;
        }
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nCentering CTL on Rank %d\n", Rank);
      CmdTimingCentering (MrcData, MrcIterationCtl, Ranks, Ranks, CtlPiCode);
    }
  } else // not Lpddr
#endif //ULT_FLAG
  {
    //
    // Center Clock Timing in the global eye
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** Center Clock Timing in the Global eye\n");

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " ALL Ranks - RankBitMask = %d\n", RankMask);
    CmdTimingCentering (MrcData, MrcIterationClock, RankMask, 1, NULL);

    //
    // Center Command Timing
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\n*** Center Command S Timing\n");
    CmdTimingCentering (MrcData, MrcIterationCmdS, RankMask, 1, CmdPiCode);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\n*** Center Command N Timing\n");
    CmdTimingCentering (MrcData, MrcIterationCmdN, RankMask, 1, CmdPiCode);

    //
    // Center Control Timing. For control pins, CKE PI is shared between Rank 2 and 3
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\n*** Center Control Timing. CKE PI is shared between Rank 2 and 3");
    for (Rank = 0; Rank < (MAX_RANK_IN_CHANNEL - 1); Rank++) {
      Ranks = (1 << Rank);

      if (Rank == 2) {
        Ranks = 0xC;
      }

      Ranks = Ranks & RankMask;

      if (Ranks) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\n Rank %d\n", Rank);
        CmdTimingCentering (MrcData, MrcIterationCtl, Ranks, 1, CtlPiCode);
      }
    }
  }

  //
  // Normalize timing back to 0 to improve performance
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n*** Normalize timing back to 0\n");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];

    //
    // Find the minimum PI Code across all relevant CMD and CTL fubs
    //
    MinCode = ChannelOut->CkeCmdPiCode[0];
    MinCode = MIN (MinCode, ChannelOut->CmdsCmdPiCode[0]);

#ifdef ULT_FLAG
    if (Lpddr) {
      MinCode = MIN (MinCode, ChannelOut->CmdsCmdPiCode[1]);
      MinCode = MIN (MinCode, ChannelOut->CmdnCmdPiCode[1]);
    } else
#endif //ULT_FLAG
    {
      MinCode = MIN (MinCode, ChannelOut->CmdnCmdPiCode[0]);
    }

    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
        MinCode = MIN (MinCode, ChannelOut->CkePiCode[Rank]);
        MinCode = MIN (MinCode, ChannelOut->CtlPiCode[Rank]);
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d: shifting all PI settings by Min PI Code = %d\n", Channel, MinCode);
    ShiftChannelTiming (MrcData, Channel, (-1) * MinCode, 1);
  } // for Channel

  //
  // Disable CADB Deselects after Command Training
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_REG) * Channel);
      MrcWriteCR (MrcData, Offset, 0);
    }
  }

  //
  // Finish Training with JEDEC Reset / Init
  //
  Status = MrcResetSequence (MrcData);
  return Status;
}

/**
@brief
  Perform Command Voltage Centering.

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus -  if it succeded returns mrcSuccess
**/
MrcStatus
MrcCmdVoltageCentering (
  IN OUT MrcParameters *const MrcData
  )
{
  const U16      mode = 0;
  const MrcInput *Inputs;
  const MrcDebug *Debug;
  MrcOutput      *Outputs;
  MrcChannelOut  *ChannelOut;
  MrcStatus      Status;
  U32            BERStats[4];
  U32            Offset;
  U8             LoopCount;
  U8             ValidRankMask;
  U8             Channel;
  U8             Rank;
  U8             chBitMask;
  U8             RankMask;
  U32            MinChLow;
  U32            MinChHigh;
  BOOL           Lpddr;
#ifdef MRC_DEBUG_PRINT
  U32            Low;
  U32            High;
  U32            Height;
#endif
  S32            Center;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ValidRankMask = Outputs->ValidRankMask;
  Status    = mrcSuccess;
  MinChLow  = 0xFFFFFFFF;
  MinChHigh = 0xFFFFFFFF;
  MrcOemMemorySet ((U8 *) BERStats, 0, sizeof (BERStats));

  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  LoopCount = (Lpddr) ? 6 : 10;

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Cmd Vref Training with LC = %d\n\nMargin\nParams: CmdV\n\tLow\tHigh\tHeight\tCenter\n",
    LoopCount
    );

  //
  // Use CADB test for Cmd to match Late Command Training
  //
  SetupIOTestCADB (MrcData, Outputs->ValidChBitMask, LoopCount, NSOE, 1, 0);

  //
  // Select rank for REUT test
  //
  chBitMask = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut = &Outputs->Controller[0].Channel[Channel];
    chBitMask |= SelectReutRanks (MrcData, Channel, ChannelOut->ValidRankBitMask, 0);
    if ((MRC_BIT0 << Channel) & chBitMask) {
      //
      // Clear any old state in DataTrain Offset
      //
      MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
    }
  }

  //
  // Run test for Cmd Voltage
  //
  Status = MrcGetBERMarginCh (
            MrcData,
            Outputs->MarginResult,
            chBitMask,
            0xFF,
            0,
            CmdV,
            mode,
            0,
            MAX_POSSIBLE_VREF,
            0,
            BERStats
            );
  Status = MrcResetSequence (MrcData);

  //
  // Find center value and update Vref.
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      MinChLow  = MIN (MinChLow, Outputs->MarginResult[LastCmdV][0][Channel][0][0]);
      MinChHigh = MIN (MinChHigh, Outputs->MarginResult[LastCmdV][0][Channel][0][1]);
    }
  }
  Center = ((S32) (MinChHigh - MinChLow)) / 2;
  UpdateVrefWaitTilStable (MrcData, 2, 1, Center / 10, 0);
  Status = MrcResetSequence (MrcData);

#ifdef MRC_DEBUG_PRINT
  //
  // Print test results
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Low  = Outputs->MarginResult[LastCmdV][0][Channel][0][0] / 10;
      High = Outputs->MarginResult[LastCmdV][0][Channel][0][1] / 10;
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "C%u:\t%u\t%u\t%u\t%d\n",
        Channel,
        Low,
        High,
        Low + High,
        ((S32) (High - Low)) / 2
        );
    }
  }
  Height = MinChHigh + MinChLow;
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Sys:\t%u\t%u\t%u\t%d\n",
    MinChLow / 10,
    MinChHigh / 10,
    Height / 10,
    Center / 10
    );
#endif

  //
  // Update MrcData for future tests
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    RankMask = MRC_BIT0 << Rank;
    if (!(RankMask & ValidRankMask)) {
      //
      // Skip if all channels empty
      //
      continue;
    }
    Outputs->MarginResult[LastCmdV][Rank][0][0][0] = MinChLow + Center;
    Outputs->MarginResult[LastCmdV][Rank][0][0][1] = MinChHigh - Center;
  }
  //
  // Disable CADB Deselects
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
  Centers Command Timing around a MidPoint

  @param[in] MrcData         - Include all MRC global data.
  @param[in] Iteration       - Determines which PI to shift
  @param[in] Ranks           - Valid Rank bit mask
  @param[in] GroupMask       - which LPDDR groups to work on for CMD/CLK/CKE; not used for DDR3
  @param[in] MidPoint        - The MidPoint to center around (per channel)

  @retval Nothing
**/
void
CmdTimingCentering (
  IN MrcParameters *const MrcData,
  IN U8                   Iteration,
  IN U8                   Ranks,
  IN U8                   GroupMask,
  IN U8                   MidPoint[MAX_CHANNEL]
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  U8                Ledge[MAX_CHANNEL];
  U8                Redge[MAX_CHANNEL];
  U8                Mid[MAX_CHANNEL];
  U8                Low[MAX_CHANNEL];
  U8                High[MAX_CHANNEL];
  U8                MidValue;
  S8                VrefOffsets[2];
  U8                Center;
  U8                ChBitMask;
  U8                RankMask;
  U8                Channel;
  U8                MinWidth;
  U8                lWidth;
  BOOL              SkipVref;

  Debug          = &MrcData->SysIn.Inputs.Debug;
  Outputs        = &MrcData->SysOut.Outputs;
  ControllerOut  = &Outputs->Controller[0];
  VrefOffsets[0] = -8;
  VrefOffsets[1] = 8;
  MinWidth       = 18;
  MrcOemMemorySet (Ledge, 0, sizeof (Ledge));
  MrcOemMemorySet (Redge, 0, sizeof (Redge));

   if ((Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) && (Iteration != MrcIterationClock)) {
    //
    // Limit the binary search to +/- 32 PI ticks from the ECT midpoint, for LPDDR3 Command/Control
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      MidValue = MidPoint[Channel];
      Low[Channel]  = (MidValue > 32) ? (MidValue - 32) : 0;
      High[Channel] = (MidValue < 127 - 32) ? (MidValue + 32) : 127;
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Ch%d: search range is [%d..%d]\n", Channel, Low[Channel], High[Channel]);
    }
  } else {
    //
    // Binary search will use the full PI range of [0..127] 
    //
    MrcOemMemorySet (Low,   0, sizeof (Low));
    MrcOemMemorySet (High,  127, sizeof (High));
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel\t\t\t0 1\n");

  //
  // Setup REUT Test to iteration through appropriate ranks during test
  //
  ChBitMask = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChBitMask |= SelectReutRanks (MrcData, Channel, Ranks, 0);
    if (MidPoint != NULL) {
      Mid[Channel] = MidPoint[Channel];
    }
  }
  //
  // MRC_DEBUG_MSG (&MrcData->Inputs.Debug, MSG_LEVEL_NOTE, "**** CmdTimingCentering, Iteration = %d, ChBitMask = 0x%x\n", Iteration,ChBitMask);
  //
  if (Iteration == MrcIterationClock) {
    //
    // Use a linear search to center clock and Update Clock Delay/Host
    // Allow wrap around since this is clock
    // CmdLinearFindEdges also programs the new values
    //
    SkipVref = FALSE;
    CmdLinearFindEdges (MrcData, Iteration, ChBitMask, Ranks, GroupMask, Low[0], High[0], 1, VrefOffsets, FALSE, SkipVref);
  } else {
    CmdBinaryFindEdge (MrcData, Iteration, ChBitMask, Ranks, GroupMask, Low, Mid, 0, VrefOffsets);
    Ledge[0] = Mid[0];
    Ledge[1] = Mid[1];  // CountUp is 0 so return High.
    if (MidPoint == NULL) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Midpoint[] is NULL and MrcIterationClock not selected!\n");
    } else {
      Mid[0]   = MidPoint[0];
      Mid[1]   = MidPoint[1]; //Mid Modified by CmdBinaryFindEdge
    }
    CmdBinaryFindEdge (MrcData, Iteration, ChBitMask, Ranks, GroupMask, Mid, High, 1, VrefOffsets);
    Redge[0] = Mid[0];
    Redge[1] = Mid[1];  // CountUp is 1 so return Low.
    //
    // Update Variables:
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nCH\tLeft\tRight\tWidth\tCenter");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (((1 << Channel) & ChBitMask) == 0) {
        continue;
      }
      RankMask  = Ranks & ControllerOut->Channel[Channel].ValidRankBitMask;
      lWidth    = Redge[Channel] - Ledge[Channel];

      if ((Redge[Channel] == 127) && (Ledge[Channel] == 0)) {
        //
        // No errors found
        //
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "\nNo Errors Found for C%u!\n", Channel);
        Center = MidPoint[Channel];
      } else {
        Center = (Ledge[Channel] + Redge[Channel] + 1) / 2;
        if (lWidth < MinWidth) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "\nEye < %u for C%u!\n", MinWidth, Channel);
        }
      }

      ShiftPIforCmdTraining (MrcData, Channel, Iteration, RankMask, GroupMask, Center, 1);

      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\n %d\t%d\t%d\t%d\t%d",
        Channel,
        Ledge[Channel],
        Redge[Channel],
        lWidth,
        Center
        );
    } // for Channel
  }

  return;
}

/**
@brief
  Use a linear search to find the edges between Low and High
  if WrapAround = 0: Look for largest passing region between low and high
  if WrapAround = 1: Look for largest passing region, including wrapping from high to low

  @param[in, out] MrcData     - Include all MRC global data.
  @param[in]      Iteration   - Determines which PI to shift
  @param[in]      chBitMask   - Valid Channel bit mask
  @param[in]      Ranks       - Valid Rank bit mask
  @param[in]      GroupMask   - which LPDDR groups to work on for CMD/CLK/CKE; not used for DDR3
  @param[in]      Low         - Low limit
  @param[in]      High        - High limit
  @param[in]      WrapAllowed - Determines the search region
  @param[in]      VrefOffsets - Array of Vref offsets
  @param[in]      SkipPrint   - Switch to enable or disable debug printing
  @param[in]      SkipVref    - Skip changing CMD Vref offsets, only run test once at the current Vref.

  @retval MrcStatus -  If it succeeds return mrcSuccess
**/
void
CmdLinearFindEdges (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   Iteration,
  IN     U8                   chBitMask,
  IN     U8                   Ranks,
  IN     U8                   GroupMask,
  IN     S8                   Low,
  IN     U8                   High,
  IN     U8                   WrapAllowed,
  IN     S8                   *VrefOffsets,
  IN     BOOL                 SkipPrint,
  IN     BOOL                 SkipVref
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  S32               *IPStart;
  S32               *IPEnd;
  S32               *CPStart;
  S32               *CPEnd;
  S32               *LPStart;
  S32               *LPEnd;
  MrcStatus         Status;
  BOOL              Pass;
  BOOL              Lpddr;
  S32               InitialPassingStart[MAX_CHANNEL];
  S32               InitialPassingEnd[MAX_CHANNEL];
  S32               CurrentPassingStart[MAX_CHANNEL];
  S32               CurrentPassingEnd[MAX_CHANNEL];
  S32               LargestPassingStart[MAX_CHANNEL];
  S32               LargestPassingEnd[MAX_CHANNEL];
  S32               lWidth;
  S32               iWidth;
  S32               cWidth;
  S32               Center;
  S16               LCTDelay;
  U8                Channel;
  U8                ChannelMask;
  U8                RankMask;
  U8                Rank;
  U8                LCTStep;
  U8                LastStep;
  U8                Vloop;
  U8                ChError;
  U8                DumArr[7];
  S8                Vref;

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  lWidth        = 0;
  iWidth        = 0;
  cWidth        = 0;
  MrcOemMemorySet (DumArr, 1, sizeof (DumArr));

  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  LCTStep = (Lpddr) ? 2 : 6;

  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "**** CmdLinearFindEdges, Iteration = %d, Low = %d, High = %d\n", Iteration, Low, High);
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (!SkipPrint) ? "CLkDlay" : "");

  for (LCTDelay = Low; LCTDelay <= High; LCTDelay += LCTStep) {
    //
    // Update Timing
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if ((1 << Channel) & chBitMask) {
        RankMask = Ranks & ControllerOut->Channel[Channel].ValidRankBitMask;
        ShiftPIforCmdTraining (MrcData, Channel, Iteration, RankMask, GroupMask, LCTDelay, 0);
      }
    }
    //
    // Reset DDR
    //
    Status = MrcResetSequence (MrcData);

    //
    // Run REUT until both channels fail or we finish all Vref points
    //
    if (SkipVref) {
      ChError = RunIOTest (MrcData, chBitMask, Outputs->DQPat, DumArr, 1, 0);
    } else {
      ChError = 0;
      for (Vloop = 0; Vloop < 2; Vloop++) {
        Vref = VrefOffsets[Vloop];
        UpdateVrefWaitTilStable (MrcData, 2, 0, Vref, 0);

        ChError |= RunIOTest (MrcData, chBitMask, Outputs->DQPat, DumArr, 1, 0);

        if (ChError == chBitMask) {
          break;
        }

      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (!SkipPrint) ? "\n %d\t\t\t" : "", LCTDelay);

    //
    // Update Passing Variables
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelMask = MRC_BIT0 << Channel;
      if (!(ChannelMask & chBitMask)) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (!SkipPrint && (Channel == 0)) ? "  " : "");
        continue;
      }

      Pass = !(ChError & ChannelMask);

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (!SkipPrint) ? (Pass ? ". " : "# ") : "");

      IPStart = &InitialPassingStart[Channel];
      IPEnd   = &InitialPassingEnd[Channel];
      CPStart = &CurrentPassingStart[Channel];
      CPEnd   = &CurrentPassingEnd[Channel];
      LPStart = &LargestPassingStart[Channel];
      LPEnd   = &LargestPassingEnd[Channel];

      if (LCTDelay == (S16) Low) {
        if (Pass) {
          *IPStart = *IPEnd = *CPStart = *CPEnd = *LPStart = *LPEnd = Low;
        } else {
          *IPStart = *IPEnd = *CPStart = *CPEnd = *LPStart = *LPEnd = Low - LCTStep;
        }
      } else {
        if (Pass) {
          //
          // Update Initial variables
          //
          if (*IPEnd == (LCTDelay - LCTStep)) {
            *IPEnd = LCTDelay; // In passing region
          }
          //
          // Update Current variables
          //
          if (*CPEnd == (LCTDelay - LCTStep)) {
            *CPEnd = LCTDelay; // In passing region
          } else {
            *CPStart = *CPEnd = LCTDelay;
          }
          //
          // Special case for last step: Append Initial Passing Region
          // LCTDelay should be considered a continuous range that wraps around 0
          //
          LastStep = High - LCTStep;
          if ((LCTDelay >= LastStep) && (*IPStart == Low) && WrapAllowed) {
            iWidth = *IPEnd -*IPStart;
            *CPEnd += (LCTStep + iWidth);
          }
          //
          // Update Largest variables
          //
          cWidth = *CPEnd - *CPStart;
          lWidth = *LPEnd - *LPStart;
          if (cWidth > lWidth) {
            *LPStart = *CPStart;
            *LPEnd   = *CPEnd;
          }
        }
      }
    } // for Channel


  } // for LCTDelay

  if (!SkipPrint) {
    //
    // Update Variables:
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nCH\tLeft\tRight\tWidth\tCenter\n");
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if ((MRC_BIT0 << Channel) & chBitMask) {
      LPStart = &LargestPassingStart[Channel];
      LPEnd   = &LargestPassingEnd[Channel];
      //
      // Handle any corner cases
      //
      lWidth  = *LPEnd - *LPStart;
      if ((lWidth < (3 * LCTStep)) || (lWidth >= (High - Low))) {
        //
        // @todo: Pass a default center parameter instead of line below.
        //
        Center = (Low + High) / 2;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "\nError Handler! Found Bad command Eye\n");
      } else {
        Center = (*LPEnd + *LPStart) / 2;
      }
      RankMask = Ranks & ControllerOut->Channel[Channel].ValidRankBitMask;
      if (!SkipPrint) {
        //
        // Shift Timing
        //
        ShiftPIforCmdTraining (MrcData, Channel, Iteration, RankMask, GroupMask, Center, 1);

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          " %d\t%d\t%d\t%d\t%d\n",
          Channel,
          *LPStart,
          *LPEnd,
          lWidth,
          Center
          );
      }
      //
      // Determine in which rank to save the margins...
      //
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if ((RankMask >> Rank) & MRC_BIT0) {
          Outputs->MarginResult[LastCmdT][Rank][Channel][0][0] = 10 * ABS (*LPStart);
          Outputs->MarginResult[LastCmdT][Rank][Channel][0][1] = 10 * ABS (*LPEnd);
        }
      }
    }
  }
  //
  // Clean Up
  //
  if (!SkipVref) {
    UpdateVrefWaitTilStable (MrcData, 2, 0, 0, 0);
  }

  Status = MrcResetSequence (MrcData);
  return;
}

/**
@brief
  Use a binary search to find the edge between Low and High
  High and Low track passing points
  if CountUp: Low is a passing point and need to count up to find a failing point
  if CountDn: High is a passing point and need to count dn to find a failing point

  @param[in]      MrcData     - Include all MRC global data.
  @param[in]      Iteration   - Determines which PI to shift
  @param[in]      ChBitMask   - Valid Channel bit mask
  @param[in]      Ranks       - Valid Rank bit mask
  @param[in]      GroupMask   - which LPDDR groups to work on for CMD/CLK/CKE; not used for DDR3
  @param[in, out] Low         - Low limit
  @param[in, out] High        - High limit
  @param[in]      CountUp     - The direction to search
  @param[in]      VrefOffsets - Array of Vref offsets

  @retval Nothing
**/
void
CmdBinaryFindEdge (
  IN     MrcParameters *const MrcData,
  IN     U8                   Iteration,
  IN     U8                   ChBitMask,
  IN     U8                   Ranks,
  IN     U8                   GroupMask,
  IN OUT U8                   *Low,
  IN OUT U8                   *High,
  IN     U8                   CountUp,
  IN     S8                   *VrefOffsets
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcStatus         Status;
  U8                Target[MAX_CHANNEL];
  U8                Done;
  U8                ChError;
  U8                DumArr[7];
  S8                Vref;
  U8                Channel;
  U8                ChannelMask;
  U8                RankMask;
  U8                Group;
  U8                Fail;
  U8                Vloop;

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Done          = 0;
  ChError       = 0;
  MrcOemMemorySet (Target, 0, sizeof (Target));
  MrcOemMemorySet (DumArr, 1, sizeof (DumArr));

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CmdTgt\nCh0G0\tCh0G1\tCh1G0\tCh1G1\n");

  while (!Done) {
    //
    // Update Timing
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (((1 << Channel) & ChBitMask) == 0) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
      } else {
        Target[Channel] = (High[Channel] + Low[Channel] + CountUp) / 2;    // CountUp gets rounding correct
        RankMask = Ranks & ControllerOut->Channel[Channel].ValidRankBitMask;
        for (Group = 0; Group < 2; Group++) {
          if (((1 << Group) & GroupMask) == 0) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
          } else {
            ShiftPIforCmdTraining (MrcData, Channel, Iteration, RankMask, 1 << Group, Target[Channel], 0);
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", Target[Channel]);
          }
        }
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");

    //
    // Reset DDR
    //
    Status = MrcResetSequence (MrcData);

    //
    // Run REUT until both channels fail or we finish all Vref points
    //
    ChError = 0;
    for (Vloop = 0; Vloop < 2; Vloop++) {
      Vref = VrefOffsets[Vloop];
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "**** Run REUT until both channels fail or we finish all Vref points, Vref = %d\n", Vref);
      //
      UpdateVrefWaitTilStable (MrcData, 2, 0, Vref, 0);

      ChError |= RunIOTest (MrcData, ChBitMask, Outputs->DQPat, DumArr, 1, 0);
      if (ChError == ChBitMask) {
        break;
      }

    }

    //
    // Update High/Low
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelMask = 1 << Channel;
      if (!(ChannelMask & ChBitMask)) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (Channel == 0) ? "  " : "");
      } else {
        Fail = (ChError & ChannelMask);

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, Fail ? "# " : ". ");

        //
        // Skip if this channel is done
        //
        if (High[Channel] > Low[Channel]) {
          if (CountUp) {
            if (Fail) {
              High[Channel] = Target[Channel] - 1;
            } else {
              Low[Channel] = Target[Channel];
            }
          } else {
            if (Fail) {
              Low[Channel] = Target[Channel] + 1;
            } else {
              High[Channel] = Target[Channel];
            }
          }
        }
      }
    }
    //
    // Update Done
    //
    Done = 1;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if ((1 << Channel) & ChBitMask) {
        if (High[Channel] > Low[Channel]) {
          Done = 0;
        }
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  }
  //
  // Clean Up
  //
  UpdateVrefWaitTilStable (MrcData, 2, 0, 0, 0);
  MrcResetSequence (MrcData);
  return;
}

/**
@brief
  Shift the CLK/CMD/CTL Timing by the given PI setting value

  @param[in] MrcData    - Include all MRC global data.
  @param[in] Channel    - Channel to shift
  @param[in] Offset     - Offset to shift by
  @param[in] UpdateHost - Switch to update the host structure

  @retval Nothing
**/
void
ShiftChannelTiming (
  IN MrcParameters *const MrcData,
  IN U8                   Channel,
  IN S32                  Offset,
  IN U8                   UpdateHost
  )
{
  const MrcDebug      *Debug;
  const MrcInput      *Inputs;
  const MrcChannelIn  *ChannelIn;
  MrcChannelOut       *ChannelOut;
  S32                 NewCode;
  U8                  Rank;
  U8                  RankBit;
#ifdef ULT_FLAG
  U8                  Group;
  BOOL                Lpddr;
#endif // ULT_FLAG

  Inputs      = &MrcData->SysIn.Inputs;
  Debug       = &Inputs->Debug;
  ChannelIn   = &Inputs->Controller[0].Channel[Channel];
  ChannelOut  = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %d:\n", Channel);

#ifdef ULT_FLAG
  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (MrcData->SysOut.Outputs.DdrType == MRC_DDR_TYPE_LPDDR3);
#endif // ULT_FLAG

  //
  // Shift the CLK/CTL Timing
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
      RankBit = 1 << Rank;
      NewCode = ChannelOut->ClkPiCode[Rank] + Offset;
#ifdef ULT_FLAG
      if (!Lpddr)
#endif // ULT_FLAG
      {
        //
        // CLK is per Rank in DDR3
        //
        ShiftPIforCmdTraining (MrcData, Channel, MrcIterationClock, RankBit, RankBit, Offset, UpdateHost);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " R%d New CLK value = %d\n", Rank, NewCode);
      }

      NewCode = ChannelOut->CtlPiCode[Rank] + Offset;
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCtl, RankBit, 1, NewCode, UpdateHost);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " R%d New CTL value = %d\n", Rank, NewCode);
    }
  }

  //
  // Shift the CMD Timing
  //
  NewCode = ChannelOut->CmdsCmdPiCode[0] + Offset;
  ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, ChannelOut->ValidRankBitMask, 1, NewCode, UpdateHost);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " New CMDS[0] value = %d\n", NewCode);

#ifdef ULT_FLAG
  if (Lpddr) {
    //
    // CLK is per Group in LPDDR3
    //
    for (Group = 0; Group < 2; Group++) {
      if (ChannelIn->DQByteMap[MrcIterationClock][Group] != 0) {
        ShiftPIforCmdTraining (MrcData, Channel, MrcIterationClock, 0, 1 << Group, Offset, UpdateHost);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " New CLK%d value = %d\n", Group, ChannelOut->ClkPiCode[Group]);
      }
    }

    NewCode = ChannelOut->CkeCmdPiCode[0] + Offset;
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCke, ChannelOut->ValidRankBitMask, 1, NewCode, UpdateHost);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " New CKE[0] value = %d\n", NewCode);

    NewCode = ChannelOut->CmdsCmdPiCode[1] + Offset;
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, ChannelOut->ValidRankBitMask, 2, NewCode, UpdateHost);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " New CMDS[1] value = %d\n", NewCode);

    NewCode = ChannelOut->CmdnCmdPiCode[1] + Offset;
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdN, ChannelOut->ValidRankBitMask, 2, NewCode, UpdateHost);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " New CMDN[1] value = %d\n", NewCode);
  } else
#endif // ULT_FLAG
  {
    NewCode = ChannelOut->CmdnCmdPiCode[0] + Offset;
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdN, ChannelOut->ValidRankBitMask, 1, NewCode, UpdateHost);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " New CMDN[0] value = %d\n", NewCode);
  }
  return;
}

/**
@brief
  This function updtes Command Mode register, tXP and Round trip latency

  @param[in, out] MrcData     - Include all MRC global data.
  @param[in]      Channel     - Channel to perform update to
  @param[in]      OldN        - Old N Mode value
  @param[in]      NewN        - New N mode value

  @retval Nothing
**/
void
UpdateCmdNTiming (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   Channel,
  IN     U8                   OldN,
  IN     U8                   NewN
  )
{
  const U8 CmdStretch[1 << MCHBAR_CH0_CR_TC_BANK_RANK_A_CMD_stretch_WID] = {
    0,
    2,
    3,
    MCHBAR_CH0_CR_TC_BANK_RANK_A_CMD_stretch_DEF
  };

  const MrcDebug                      *Debug;
  MrcOutput                           *Outputs;
  MrcChannelOut                       *ChannelOut;
  U8                                  *RtLatency;
  MCHBAR_CH0_CR_TC_BANK_RANK_A_STRUCT TcBankRankA;
  MCHBAR_CH0_CR_TC_BANK_RANK_C_STRUCT TcBankRankC;
  MCHBAR_CH0_CR_SC_ROUNDT_LAT_STRUCT  ScRoundtLat;
  MrcProfile                          Profile;
  U32                                 Offset;
  U32                                 Scratch;
  U8                                  Rank;
  S8                                  Diff;

  Debug      = &MrcData->SysIn.Inputs.Debug;
  Outputs    = &MrcData->SysOut.Outputs;
  ChannelOut = &Outputs->Controller[0].Channel[Channel];
  Profile    = MrcData->SysIn.Inputs.MemoryProfile;

  //
  // Update CmdN timing
  //
  Offset = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG +
    ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * Channel);
  TcBankRankA.Data              = MrcReadCR (MrcData, Offset);
  TcBankRankA.Bits.CMD_stretch  = CmdStretch[ChannelOut->Timing[Profile].NMode - 1];
  MrcWriteCR (MrcData, Offset, TcBankRankA.Data);
  ChannelOut->MchbarBANKRANKA = TcBankRankA.Data;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d_TC_BANK_RANK_A = 0x%x\n", Channel, TcBankRankA.Data);

  //
  // Adjust tXP value
  //
  Offset = MCHBAR_CH0_CR_TC_BANK_RANK_C_REG +
    ((MCHBAR_CH1_CR_TC_BANK_RANK_C_REG - MCHBAR_CH0_CR_TC_BANK_RANK_C_REG) * Channel);
  TcBankRankC.Data      = MrcReadCR (MrcData, Offset);
  Scratch = tXPValue (Outputs->DdrType, Outputs->Frequency, (U8) ChannelOut->Timing[Profile].NMode);
  TcBankRankC.Bits.tXP  = MIN (Scratch, MCHBAR_CH0_CR_TC_BANK_RANK_C_tXP_MAX);
  MrcWriteCR (MrcData, Offset, TcBankRankC.Data);
  ChannelOut->MchbarBANKRANKC = TcBankRankC.Data;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d_TC_BANK_RANK_C = 0x%x\n", Channel, TcBankRankC.Data);

  //
  // Adjust RT values to compensate.
  //
  Diff              = (NewN - OldN);
  ScRoundtLat.Data  = 0;
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
      RtLatency  = &ChannelOut->RTLatency[Rank];
      *RtLatency = (U8) (*RtLatency + Diff);
        switch (Rank) {
          case 0:
            ScRoundtLat.Bits.Lat_R0D0 = *RtLatency;
            break;

          case 1:
            ScRoundtLat.Bits.Lat_R1D0 = *RtLatency;
            break;

          case 2:
            ScRoundtLat.Bits.Lat_R0D1 = *RtLatency;
            break;

          case 3:
            ScRoundtLat.Bits.Lat_R1D1 = *RtLatency;
            break;

          default:
            break;
        }
    }
  }

  Offset = MCHBAR_CH0_CR_SC_ROUNDT_LAT_REG +
    ((MCHBAR_CH1_CR_SC_ROUNDT_LAT_REG - MCHBAR_CH0_CR_SC_ROUNDT_LAT_REG) * Channel);
  MrcWriteCR (MrcData, Offset, ScRoundtLat.Data);
  return;
}

#ifdef ULT_FLAG

/**
@brief
  Enter / exit LPDDR CA training modes.
  Main flow:
  1. Force CKE high.
  2. Send MRW 41, 48 or 42.
  3. Force CKE low for MRW 41 or 48

  @param[in] MrcData      - The MRC global data.
  @param[in] ChBitMask    - channels to work on.
  @param[in] RankBitMask  - ranks to work on.
  @param[in] Mode         - CA training mode.

  @retval mrcSuccess if succeeded
**/
MrcStatus
LpddrCommandTrainingMode (
  IN MrcParameters * const MrcData,
  IN U8                    ChBitMask,
  IN U8                    RankBitMask,
  IN MrcCaTrainingMode     Mode
  )
{
  MrcStatus         Status;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  U8                Channel;
  U8                Rank;
  U32               Offset;
  U32               Address;
  U32               Data;
  BOOL              InitMrw;
  BOOL              ChipSelect2N;
  MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_STRUCT MiscCkeCtrl;

  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];

  InitMrw = TRUE;
  ChipSelect2N = FALSE;

  switch (Mode) {
    case CaTrainingMode41:
      Address = 0x29;
      Data    = 0xA4;   // Data is selected so that High and Low phases of CA[9:0] are equal
      break;

    case CaTrainingMode48:
      Address = 0x30;
      Data    = 0xC0;
      break;

    case CaTrainingMode42:
      Address = 0x2A;
      Data    = 0xA8;
      ChipSelect2N = FALSE;
      break;

    default:
      return mrcFail;
  }

  //
  // Send the MRW41 command to populated ranks
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (((1 << Channel) & ChBitMask) == 0) {
      continue;
    }

    //
    // Force CKE high
    //
    MiscCkeCtrl.Data = 0;
    MiscCkeCtrl.Bits.CKE_Override = 0x0F;
    MiscCkeCtrl.Bits.CKE_On       = ControllerOut->Channel[Channel].ValidCkeBitMask;
    Offset = MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG +
      (MCHBAR_CH1_CR_REUT_CH_MISC_CKE_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG) * Channel;
    MrcWriteCR (MrcData, Offset, MiscCkeCtrl.Data);

    //
    // Wait for CKE to become effective
    //
    MrcWait (MrcData, 1 * HPET_MIN);

    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank ++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      if (((1 << Rank) & RankBitMask) != 0) {
        Status = MrcIssueMrw (MrcData, Channel, Rank, Address, Data, InitMrw, ChipSelect2N);
        if (Status != mrcSuccess) {
          return Status;
        }
      }
    }

    //
    // Force CKE Low for MRW 41 or 48
    //
    if (Mode != CaTrainingMode42) {
      //
      // Wait tCACKEL = 10 tCK
      //
      MrcWait (MrcData, 1 * HPET_MIN);

      //
      // Force CKE low, tCACKEL after MRW41 issued
      //
      MiscCkeCtrl.Bits.CKE_On = 0;
      MrcWriteCR (MrcData, Offset, MiscCkeCtrl.Data);
    }
  }  // for Channel
  return mrcSuccess;
}

/**
  Program CADB Pattern Buffers with given values

  @param[in] MrcData    - The MRC global data.
  @param[in] Channel    - channel to work on.
  @param[in] PatBuf0    - Pattern Buffer 0 value
  @param[in] PatBuf1    - Pattern Buffer 1 value
  @param[in] PatBuf2    - Pattern Buffer 2 value

  @retval none
**/
void
SetCadbPatternBuffers (
  IN MrcParameters * const  MrcData,
  IN U8                     Channel,
  IN U32                    PatBuf0,
  IN U32                    PatBuf1,
  IN U32                    PatBuf2
  )
{
  U32 Offset;

  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_0_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MUX_PB_0_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_0_REG) * Channel);
  MrcWriteCR (MrcData, Offset, PatBuf0);

  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_1_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MUX_PB_1_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_1_REG) * Channel);
  MrcWriteCR (MrcData, Offset, PatBuf1);

  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_2_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MUX_PB_2_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_2_REG) * Channel);
  MrcWriteCR (MrcData, Offset, PatBuf2);
}

//
//  Sets up 3 CADB lines that will be used to send out a CS pattern.
//
//  -----------------------
//  CADB  Phase  Phase  CS
//  Line  High   Low
//  -----------------------
//   0    0x000  0x000  Off
//   1    0x3FF  0x3FF  Off
//   2    0x2AA  0x2AA  On
//   3    0x155  0x155  On
//
//  The CS pattern uses Pattern Buffer and hence has 16 lines, with CS active for one line only.
//  This will send a command every 16 DCLKs.
//
//  Pattern Buffer details:
//  The line order is:    0, 0, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//  or different command: 0, 0, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//
//  000
//  000
//  000
//  000
//
//  000
//  000
//  000
//  000
//
//  000
//  000
//  000
//  000
//
//  001
//  001
//  010 or 011
//  000
//  ----
//  000  --> PB[0] = 0x3000 or 0x7000
//  000      PB[1] = 0x4000
//  000      PB[2] = 0x0000
//  043 or 047
//
CADB_LINE CadbLinesCs[] = {
  { 0x000, 0x000, 0 },
  { 0x3FF, 0x3FF, 0 },
  { 0x2AA, 0x2AA, 1 },
  { 0x155, 0x155, 1 }
};

/**
  Setup the CADB for CS or CA training.

  @param[in] MrcData    - The MRC global data.
  @param[in] Channel    - channel to work on
  @param[in] Rank       - rank to work on
  @param[in] CadbLines  - CADB lines to program
  @param[in] CadbCount  - Number of CADB lines to program
  @param[in] PatBuf0    - Pattern Buffer 0 value
  @param[in] PatBuf1    - Pattern Buffer 1 value
  @param[in] PatBuf2    - Pattern Buffer 2 value
  @retval none
**/
void
SetupCaTrainingCadb (
  IN MrcParameters  * const MrcData,
  IN U8             Channel,
  IN U8             Rank,
  IN CADB_LINE      *CadbLines,
  IN U32            CadbCount,
  IN U32            PatBuf0,
  IN U32            PatBuf1,
  IN U32            PatBuf2
)
{
  U32 Offset;
  U32 MA;
  U32 BA;
  U32 CMD;
  U32 i;

  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_CTRL_STRUCT  ReutChPatCadbMuxCtrl;
  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_STRUCT      ReutChPatCadbProg;

  //
  // Set Mux0/1/2 to Pattern Buffer mode
  //
  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_CTRL_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MUX_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_CTRL_REG) * Channel);
  ReutChPatCadbMuxCtrl.Data = MrcReadCR (MrcData, Offset);
  ReutChPatCadbMuxCtrl.Bits.Mux0_Control = 1;
  ReutChPatCadbMuxCtrl.Bits.Mux1_Control = 1;
  ReutChPatCadbMuxCtrl.Bits.Mux2_Control = 1;
  MrcWriteCR (MrcData, Offset, ReutChPatCadbMuxCtrl.Data);

  //
  // Program Pattern Buffers for a specific progression over CADB,
  // according to the given Pattern Buffer values
  //
  SetCadbPatternBuffers (MrcData, Channel, PatBuf0, PatBuf1, PatBuf2);

  //
  // Start writing at CADB row 0
  //
  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG) * Channel);
  MrcWriteCR (MrcData, Offset, 0);

  ReutChPatCadbProg.Data = 0;
  ReutChPatCadbProg.Bits.CADB_Data_ODT = (0 << Rank);
  ReutChPatCadbProg.Bits.CADB_Data_CKE = (0 << Rank);

  //
  // Program the CADB lines
  //
  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_PROG_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_REG) * Channel);
  for (i = 0; i < CadbCount ; i++) {
    MrcConvertLpddr2Ddr (CadbLines[i].CaHigh, CadbLines[i].CaLow, &MA, &BA, &CMD);
    ReutChPatCadbProg.Bits.CADB_Data_Address  = MA;
    ReutChPatCadbProg.Bits.CADB_Data_Bank     = BA;
    ReutChPatCadbProg.Bits.CADB_Data_Control  = CMD;
    ReutChPatCadbProg.Bits.CADB_Data_CS       = 0x0F & ~(CadbLines[i].ChipSelect << Rank);

    //
    // Write CADB line. It is auto incremented after every write
    //
    MrcWriteCR64 (MrcData, Offset, ReutChPatCadbProg.Data);
  }
}

/**
  Program DESWIZZLE_HIGH/LOW registers for MR4 decoding

  @param[in] MrcData - The MRC global data.

  @retval none
**/
void
ProgramDeswizzleRegisters (
  IN MrcParameters * const  MrcData
  )
{
  MrcInput          *Inputs;
  MrcDebug          *Debug;
  MrcOutput         *Outputs;
  MrcChannelIn      *ChannelIn;
  MrcChannelOut     *ChannelOut;
  MrcControllerOut  *ControllerOut;
  U8                Channel;
  U32               Byte;
  U8                Bit;
  U32               Offset;
  MCHBAR_CH0_CR_DESWIZZLE_LOW_STRUCT  DeswizzleLow;
  MCHBAR_CH0_CR_DESWIZZLE_HIGH_STRUCT DeswizzleHigh;

  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;
  Outputs = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelIn = &Inputs->Controller[0].Channel[Channel];
    ChannelOut = &ControllerOut->Channel[Channel];

    DeswizzleLow.Data   = 0;
    DeswizzleHigh.Data  = 0;

    for (Byte = 0; Byte <= 7; Byte++) {
      //
      // DqsMapCpu2Dram maps CPU bytes to DRAM, we need to find the reverse mapping here
      //
      switch (ChannelIn->DqsMapCpu2Dram[Byte]) {
        case 0:
          DeswizzleLow.Bits.Byte_0  = Byte;
          break;
        case 2:
          DeswizzleLow.Bits.Byte_2  = Byte;
          break;
        case 4:
          DeswizzleHigh.Bits.Byte_4 = Byte;
          break;
        case 6:
          DeswizzleHigh.Bits.Byte_6 = Byte;
          break;
      }
    }

    for (Bit = 0; Bit <= 7; Bit++) {
      //
      // DqMapCpu2Dram maps CPU DQ pins to DRAM, we need to find the reverse mapping here
      //
      Byte = DeswizzleLow.Bits.Byte_0;
      switch (ChannelIn->DqMapCpu2Dram[Byte][Bit]) {
        case 0:
          DeswizzleLow.Bits.Bit_0 = Bit;
          break;
        case 1:
          DeswizzleLow.Bits.Bit_1 = Bit;
          break;
        case 2:
          DeswizzleLow.Bits.Bit_2 = Bit;
          break;
      }

      Byte = DeswizzleLow.Bits.Byte_2;
      switch (ChannelIn->DqMapCpu2Dram[Byte][Bit]) {
        case 16:
          DeswizzleLow.Bits.Bit_16 = Bit;
          break;
        case 17:
          DeswizzleLow.Bits.Bit_17 = Bit;
          break;
        case 18:
          DeswizzleLow.Bits.Bit_18 = Bit;
          break;
      }

      Byte = DeswizzleHigh.Bits.Byte_4;
      switch (ChannelIn->DqMapCpu2Dram[Byte][Bit]) {
        case 32:
          DeswizzleHigh.Bits.Bit_32 = Bit;
          break;
        case 33:
          DeswizzleHigh.Bits.Bit_33 = Bit;
          break;
        case 34:
          DeswizzleHigh.Bits.Bit_34 = Bit;
          break;
      }

      Byte = DeswizzleHigh.Bits.Byte_6;
      switch (ChannelIn->DqMapCpu2Dram[Byte][Bit]) {
        case 48:
          DeswizzleHigh.Bits.Bit_48 = Bit;
          break;
        case 49:
          DeswizzleHigh.Bits.Bit_49 = Bit;
          break;
        case 50:
          DeswizzleHigh.Bits.Bit_50 = Bit;
          break;
      }
    } // for Bit

    if (ChannelOut->Dimm[dDIMM0].SdramWidth == 32) {
      //
      // Bytes 2 and 6 (and their bits) are irrelevant for x32 devices - copy from Bytes 0 and 4 instead
      //
      DeswizzleLow.Bits.Byte_2 = DeswizzleLow.Bits.Byte_0;
      DeswizzleLow.Bits.Bit_16 = DeswizzleLow.Bits.Bit_0;
      DeswizzleLow.Bits.Bit_17 = DeswizzleLow.Bits.Bit_1;
      DeswizzleLow.Bits.Bit_18 = DeswizzleLow.Bits.Bit_2;

      DeswizzleHigh.Bits.Byte_6 = DeswizzleHigh.Bits.Byte_4;
      DeswizzleHigh.Bits.Bit_48 = DeswizzleHigh.Bits.Bit_32;
      DeswizzleHigh.Bits.Bit_49 = DeswizzleHigh.Bits.Bit_33;
      DeswizzleHigh.Bits.Bit_50 = DeswizzleHigh.Bits.Bit_34;
    }

    Offset = MCHBAR_CH0_CR_DESWIZZLE_LOW_REG +
      (MCHBAR_CH1_CR_DESWIZZLE_LOW_REG - MCHBAR_CH0_CR_DESWIZZLE_LOW_REG) * Channel;
    MrcWriteCR (MrcData, Offset, DeswizzleLow.Data);

    Offset = MCHBAR_CH0_CR_DESWIZZLE_HIGH_REG +
      (MCHBAR_CH1_CR_DESWIZZLE_HIGH_REG - MCHBAR_CH0_CR_DESWIZZLE_HIGH_REG) * Channel;
    MrcWriteCR (MrcData, Offset, DeswizzleHigh.Data);

    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "Ch%d DESWIZZLE_HIGH=%08X, DESWIZZLE_LOW=%08X\n",
      Channel,
      DeswizzleHigh.Data,
      DeswizzleLow.Data
      );
  } // for Channel
}

/**
  Sweep both CS and CMD PI and print the feedback.

  @param[in] MrcData - The MRC global data.

  @retval none
**/
MrcStatus
Ca2DMargins (
  IN MrcParameters * const  MrcData,
  U8                        Rank
  )
{
  MrcInput          *Inputs;
  MrcDebug          *Debug;
  MrcOutput         *Outputs;
  MrcStatus         Status;
  U8                Channel;
  U8                ChannelMask;
  U8                Byte;
  U8                CaStart = 32;
  U8                CaStop = 127;
  S8                CaStep = 6;
  U8                CsStart = 0;
  U8                CsStop = 127;
  S8                CsStep = 8;
  U8                CaPiCode;
  U8                CsPiCode;
  U32               DelayCadb;
  U32               Offset;
  char              PassFail;
  DDRDATA0CH0_CR_DATATRAINFEEDBACK_STRUCT DataTrainFeedback;

  Status        = mrcSuccess;
  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;

  DelayCadb = 1 * HPET_1US;

  ChannelMask   = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
      ChannelMask |= (1 << Channel);
    }
  }

  for (CsPiCode = CsStart; CsPiCode < CsStop; CsPiCode += CsStep) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      //
      // Shift the CS PI on Rank.
      //
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCtl, 1 << Rank, 1, CsPiCode, 0);
    }

    for (CaPiCode = CaStart; CaPiCode < CaStop; CaPiCode += CaStep) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t%d\t", CsPiCode, CaPiCode);
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
          continue;
        }
        //
        // Shift the Command PI on both CAA and CAB groups
        //
        ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdN, 1 << Rank, 3, CaPiCode, 0);
        ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, 1 << Rank, 3, CaPiCode, 0);
        ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCke,  1 << Rank, 3, CaPiCode, 0);
      }

      //
      // Perform Jedec Reset ONLY
      //
      MrcJedecResetLpddr3 (MrcData);

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
          continue;
        }
        //
        // Put the current Rank in CA training mode using MRW41.
        //
        Status = LpddrCommandTrainingMode (MrcData, 1 << Channel, 1 << Rank, CaTrainingMode41);
        if (Status != mrcSuccess) {
          return Status;
        }
      }

      //
      // Run CADB pattern on selected channels at the same time
      //
      ShortRunCADB (MrcData, ChannelMask);
      MrcWait (MrcData, DelayCadb);

      //
      // Read and process the results
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Byte Feedback\n");
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
          DataTrainFeedback.Data = (MrcReadCR (MrcData, Offset) & 0xFF);  // Get only DQ bits, not DQS

          //
          // If we don't see 4 ones in the byte, then the command was not aligned properly
          //
          if (MrcCountBitsEqOne (DataTrainFeedback.Data) != 4) {
            PassFail = '#';
          } else {
            PassFail = '.';
          }
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%c%02X\t", PassFail, DataTrainFeedback.Data);
        }  // for Byte
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
      }  // for Channel
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
    }
  }  // while not done

  return Status;
}

/**
  Sweep CMD PI up or down and find edges for all bytes.
  Main flow:
  1.
  2.
  3.

  @param[in] MrcData - The MRC global data.

  @retval none
**/
void
EarlyCaFindEdge (
  IN MrcParameters * const  MrcData,
  U8                        Rank,
  U8                        Start,
  U8                        Stop,
  S8                        Step,
  U8                        Limit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM]
  )
{
  MrcInput          *Inputs;
  MrcDebug          *Debug;
  MrcOutput         *Outputs;
  MrcControllerIn   *ControllerIn;
  MrcControllerOut  *ControllerOut;
  MrcChannelIn      *ChannelIn;
  MrcChannelOut     *ChannelOut;
  U8                Channel;
  U8                ChannelMask;
  U8                Byte;
  U8                ByteMask;
  U8                DramByte;
  U8                ByteDoneMask[MAX_CHANNEL];
  U8                PiCode;
  U32               DelayCadb;
  U32               Offset;
  BOOL              Done;
  char              *BytesHeader;
  char              PassFail;
  DDRDATA0CH0_CR_DATATRAINFEEDBACK_STRUCT DataTrainFeedback;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerIn  = &Inputs->Controller[0];
  ControllerOut = &Outputs->Controller[0];

  DelayCadb = 1 * HPET_1US;

  MrcOemMemorySet (ByteDoneMask, 0, sizeof (ByteDoneMask));

  PiCode = Start;
  Done = FALSE;

  ChannelMask   = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
      ChannelMask |= (1 << Channel);
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t Ch0\t\t\t\t   Ch1\n");
  BytesHeader = "0   1   2   3   4   5   6   7     ";
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CMD PI\t %s%s\n", BytesHeader, BytesHeader);

  while (!Done) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d: \t", PiCode);
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      //
      // Shift the Command PI on both CAA and CAB groups
      //
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdN, 1 << Rank, 3, PiCode, 0);
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, 1 << Rank, 3, PiCode, 0);
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCke,  1 << Rank, 3, PiCode, 0);
    }

    //
    // Run CADB pattern on selected channels at the same time
    //
    ShortRunCADB (MrcData, ChannelMask);
    MrcWait (MrcData, DelayCadb);

    //
    // Read and process the results
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        ByteDoneMask[Channel] = 0xFF;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "                                  ");
        continue;
      }
      if (ByteDoneMask[Channel] == 0xFF) { // All bytes failed on this channel, no need to sweep anymore
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "                                  ");
        continue;
      }
      ChannelIn = &ControllerIn->Channel[Channel];
      ChannelOut = &ControllerOut->Channel[Channel];
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        ByteMask = (1 << Byte);
        DramByte = ChannelIn->DqsMapCpu2Dram[Byte];
        if (ChannelOut->Dimm[dDIMM0].SdramWidth == 32) {
          if ((DramByte & 0x02) != 0) {
            //
            // Ignore upper 16 bits on x32 devices in MRW41 feedback - DRAM bytes 2, 3, 6 and 7
            //
            ByteDoneMask[Channel] |= ByteMask;
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    ");
            continue;
          }
        }
        Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
          ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
          ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
        DataTrainFeedback.Data = (MrcReadCR (MrcData, Offset) & 0xFF);  // Get only DQ bits, not DQS

        PassFail = '#';
        if ((ByteDoneMask[Channel] & ByteMask) == 0) {
          //
          // If we don't see 4 ones in the byte, then the command was not aligned properly
          //
          if (MrcCountBitsEqOne (DataTrainFeedback.Data) != 4) {
            Limit[Channel][Rank][Byte] = PiCode;
            ByteDoneMask[Channel] |= ByteMask;
          } else {
            PassFail = '.';
          }
        }
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%c%02X ", PassFail, DataTrainFeedback.Data);
      }  // for Byte
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  ");
    }  // for Channel
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
    if ((ByteDoneMask[0] == 0xFF) && (ByteDoneMask[1] == 0xFF)) {
      // Found the limit on all bytes on both channels - no need to sweep Pi any longer
      break;
    }

    PiCode += Step;
    if (Step > 0) {
      // Sweep up
      Done = (PiCode > Stop);
    } else {
      // Sweep down
      Done = (((S8) PiCode) < Stop);
    }
  }  // while not done
}

/**
  Process the results of the early LPDDR3 CMD training and find the best PI settings for CmdS/CmdN/Cke.
  Flow:
   1. Find the worst case Right and Left limits for each channel
   2. Find the Center for each channel

  @param[in]  MrcData     - The MRC global data.
  @param[in]  LeftLimit   - array of left edge values per channel, rank and CPU byte
  @param[in]  RightLimit  - array of right edge values per channel, rank and CPU byte
  @param[out] BestCs      - array of best CMD PI settings, per channel

  @retval mrcSuccess if succeeded
**/
MrcStatus
FindBestCmdPi (
  IN  MrcParameters * const MrcData,
  IN  U8                    LeftLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM],
  IN  U8                    RightLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM],
  OUT U8                    BestCmd[MAX_CHANNEL][2]    // per Channel and per group (CAA and CAB)
  )
{
  MrcInput          *Inputs;
  MrcDebug          *Debug;
  MrcOutput         *Outputs;
  MrcStatus         Status;
  MrcChannelIn      *ChannelIn;
  MrcControllerIn   *ControllerIn;
  U8                Channel;
  U8                Rank;
  U8                Byte;
  U8                CaGroup;
  U8                CmdLeftLimit[MAX_CHANNEL][2];  // Per ch and group
  U8                CmdRightLimit[MAX_CHANNEL][2]; // Per ch and group

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerIn  = &Inputs->Controller[0];

  Status = mrcSuccess;

  MrcOemMemorySet ((U8 *) CmdRightLimit, 127, sizeof (CmdRightLimit));
  MrcOemMemorySet ((U8 *) CmdLeftLimit, 0, sizeof (CmdLeftLimit));

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Finding best CMD PIs:\n");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelIn = &ControllerIn->Channel[Channel];
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel: %d\t\tLeft\tRight\tCenter\n", Channel);
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      //
      // Find the worst case Right and Left limits for all ranks, for bytes from the particular CA group
      //
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        //
        // CmdS CAA goes to Bytes[3:0], CmdS CAB goes to Byte[7:4]
        //
        if ((1 << Byte) & ChannelIn->DQByteMap[MrcIterationCmdS][0]) {
          CaGroup = 0;
        } else {
          CaGroup = 1;
        }
        CmdRightLimit[Channel][CaGroup] = MIN (CmdRightLimit[Channel][CaGroup], RightLimit[Channel][Rank][Byte]);
        CmdLeftLimit[Channel][CaGroup]  = MAX (CmdLeftLimit[Channel][CaGroup], LeftLimit[Channel][Rank][Byte]);
      }
    } // for Rank

    //
    // Find the Center for each group, worst case of all ranks
    //
    BestCmd[Channel][0] = (CmdRightLimit[Channel][0] + CmdLeftLimit[Channel][0]) / 2;
    BestCmd[Channel][1] = (CmdRightLimit[Channel][1] + CmdLeftLimit[Channel][1]) / 2;

    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "CA%c\t\t\t%d\t%d\t%d\n",
      'A',
      CmdLeftLimit[Channel][0],
      CmdRightLimit[Channel][0],
      BestCmd[Channel][0]
      );

    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "CA%c\t\t\t%d\t%d\t%d\n",
      'B',
      CmdLeftLimit[Channel][1],
      CmdRightLimit[Channel][1],
      BestCmd[Channel][1]
      );
  } // for Channel

  return Status;
}

/**
  Update DqMapCpu2Dram array

  @param[in] MrcData  - The MRC global data.
  @param[in] Channel  - the channel to work on
  @param[in] Feedback - array of DATATRAINFEEDBACK values for all 8 bytes
  @param[in] Bit      - The DQ bit the should be set in each DRAM byte

  @retval none
**/
void
FillCA2DQMapResult (
  IN OUT MrcParameters * const  MrcData,
  IN const U8                   Channel,
  IN const U8                   Feedback[8],
  IN const U8                   Bit
  )
{
  MrcInput          *Inputs;
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcChannelOut     *ChannelOut;
  MrcControllerOut  *ControllerOut;
  MrcControllerIn   *ControllerIn;
  MrcChannelIn      *ChannelIn;
  U8                Byte;
  U8                Temp;
  U8                CpuBit;
  S8                BitNumber;
  BOOL              BitFound;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  ControllerIn  = &Inputs->Controller[0];

  ChannelOut = &ControllerOut->Channel[Channel];
  ChannelIn  = &ControllerIn->Channel[Channel];

  BitNumber = -1;

  //
  // Loop on CPU bytes
  //
  for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
    if (Feedback[Byte] == 0) {
      continue;
    }
    Temp = Feedback[Byte];
    BitNumber = 0;
    CpuBit    = 0;
    BitFound  = FALSE;
      while (Temp > 0) {
        if (Temp & 1) {
          if (!BitFound) {
              CpuBit = BitNumber;
              BitFound = TRUE;
          } else {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "Ch%d: ERROR: More than one DQ pin toggled while looking for DQ%d in Byte%d, Feedback=0x%X\n",
              Channel,
              Bit,
              Byte,
              Feedback[Byte]
              );
            break;
          }
        }
        Temp >>= 1;
        BitNumber++;
      }
      ChannelIn->DqMapCpu2Dram[Byte][CpuBit] = ChannelIn->DqsMapCpu2Dram[Byte] * 8 + Bit;
  } // for Byte
}

/**
  Rotate a given number left by a specified number of bits.

  @param[in] Value      - The input value
  @param[in] BitLength  - How many bits to rotate in the input value.
  @param[in] RotateBy   - Number of bits to rotate by.

  @retval The rotated number
**/
U32
RotateLeft (
  IN const U32 Value,
  IN const U8  BitLength, // should be >1 and <32, tested for 16
  IN const U8  RotateBy
  )
{
  U32 Mask;
  U32 Lsb;
  U32 Result;
  U8  i;

  Result = Value;
  Mask = (1 << BitLength) - 1;

  for (i = 0; i < RotateBy; i++) {
    Lsb = 1 & (((Result) & (1 << (BitLength - 1))) >> (BitLength-1)); // The MSB value needs to move to LSB
    Result = (Mask & (Result << 1)) | Lsb;                            // Shift Left once and add the new LSB
  }

  return Result;
}

/**
  Calculate 3 Pattern Buffers values for the given CADB sequence.

  @param[in] MrcData             - The MRC global data.
  @param[in] CadbSequence        - CADB line numbers in the order of transmission.
                                   Example: 0,1,0,0,...0 for DQ mapping, 0,0,2,1,1,0,0,...0 for CS training
  @param[out] CadbPatternBuffers - Array of 3 Pattern Buffer values

  @retval none
**/
void
CalculateCadbPB (
  IN MrcParameters * const  MrcData,
  IN const U8               CadbSequence[MRC_CADB_PB_LENGTH],
  OUT U32                   CadbPatternBuffers[3]
  )
{
  MrcInput  *Inputs;
  MrcDebug  *Debug;
  U8        i;
  U8        j;

  Inputs = &MrcData->SysIn.Inputs;
  Debug  = &Inputs->Debug;

  MrcOemMemorySet ((U8 *) CadbPatternBuffers, 0, 3 * sizeof (CadbPatternBuffers[0]));

  for (i = 0; i < MRC_CADB_PB_LENGTH; i++) {
    for (j = 0; j < 3; j++) {
      CadbPatternBuffers[j] = RotateLeft (CadbPatternBuffers[j], MRC_CADB_PB_LENGTH, 1) |
                                ((CadbSequence[i] & (1 << j)) >> j);
    }
  }

  for (j = 0; j < 3; j++) {
    CadbPatternBuffers[j] = RotateLeft (CadbPatternBuffers[j], MRC_CADB_PB_LENGTH, 1);
  }
}

/**
  Map CA to DQ Pins for CA training and MR4 bit swizzling settings for LPDDR.
  Main flow:
  Repeat for each of the 8 bits per DQ byte (total 8 iterations for both channels, for rank0 only):
    Transmit single CA phase expected to appear on a known DQ pin
     One CA phase per byte, 2 different CA phases for Even and Odd bytes in parallel
    Locate the single DQ in each byte based on DATATRAINFEEDBACK
     Report error if more than one DQ pin toggles
     Report error if no active DQ pin found
     Ignore Byte2 and Byte3 for x32 devices if they don't return feedback (only DQ[15:0] must return feedback per JEDEC)
    Update the DQ mapping data structure.

  Assumption: runs on stable and correct CLK, CS and CA PI settings (either guaranteed by design or pre-trained)

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess if succeeded
**/
MrcStatus
MapCA2DQPins (
  IN MrcParameters * const MrcData
)
{
  MrcInput                                    *Inputs;
  MrcDebug                                    *Debug;
  MrcOutput                                   *Outputs;
  MrcControllerIn                             *ControllerIn;
  MrcControllerOut                            *ControllerOut;
  MrcChannelIn                                *ChannelIn;
  MrcChannelOut                               *ChannelOut;
  MrcStatus                                   Status;
  U8                                          Channel;
  U8                                          Rank;
  U8                                          Byte;
  U8                                          DramByte;
  U8                                          Bit;
  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT      DdrCrDataControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT     DdrCrDataControl2;
  U32                                         Offset;
  U32                                         CaPattern;
  U8                                          Feedback[8];
  CADB_LINE CadbLinesDqMapping[] = {
    { 0x000, 0x000, 0 },
    { 0x001, 0x000, 1 }
  };
  U32     CadbPatternBuffers[3];
  U8      CadbSequence[MRC_CADB_PB_LENGTH];
#ifdef MRC_DEBUG_PRINT
  U8                                          DramBit;
  U8                                          i;
#endif

  Status = mrcSuccess;
  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerIn  = &Inputs->Controller[0];
  ControllerOut = &Outputs->Controller[0];
  Rank = 0;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MapCA2DQPins started\n");
  if (ControllerOut->Channel[0].Dimm[dDIMM0].SdramWidth == 32) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "x32 DRAM devices - skipping Bytes 2,3 in each DRAM!\n");
  }

  MrcOemMemorySet (CadbSequence, 0, sizeof (CadbSequence));
  CadbSequence[1] = 1; // The 2nd PB entry is the 2nd CADB line with active CS. The rest are 0.

  //
  // Calculate the Pattern Buffers values for the given CADB sequence
  //
  CalculateCadbPB (MrcData, CadbSequence, CadbPatternBuffers);

  //
  // Enable the following bits because we will use DATATRAINFEEDBACK to read back CA values on DQ pins:
  //  DataControl0.SenseampTrainingMode and ForceOdtOn
  //  DataControl2.ForceRxOn and ForceBiasOn
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcRankInChannelExist (MrcData, 0, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];
    MrcOemMemorySet (
      (U8 *) (ControllerIn->Channel[Channel].DqMapCpu2Dram),
      0xFF,
      sizeof (ControllerIn->Channel[0].DqMapCpu2Dram)
      );

    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      DdrCrDataControl2.Data = ChannelOut->DqControl2[Byte].Data;
      DdrCrDataControl2.Bits.ForceBiasOn = 1;
      DdrCrDataControl2.Bits.ForceRxOn   = 1;
      Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
        ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
        ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
      MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
    }
    DdrCrDataControl0.Data = ChannelOut->DqControl0.Data;
    DdrCrDataControl0.Bits.ForceOdtOn           = 1;
    DdrCrDataControl0.Bits.SenseampTrainingMode = 1;
    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
    MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);

    //
    // Put Rank 0 in CA training mode using MRW41.
    //
    Status = LpddrCommandTrainingMode (MrcData, 1 << Channel, 1 << Rank, CaTrainingMode41);
    if (Status != mrcSuccess) {
      return Status;
    }
  }

  for (Bit = 0; Bit < MAX_BITS; Bit++) {
    //
    // Create CA patterns for high and low phases,
    // such that only one DQ bit should toggle on each phase, per DRAM byte
    //
    CaPattern = (1 << (Bit / 2)) | (1 << (Bit / 2 + 5));
    CadbLinesDqMapping[1].CaHigh = ((Bit % 2) == 0) ? CaPattern : 0;
    CadbLinesDqMapping[1].CaLow  = ((Bit % 2) == 1) ? CaPattern : 0;

#ifdef MRC_DEBUG_PRINT
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRunning Bit %d\n", Bit);
    for (i = 0; i < sizeof (CadbLinesDqMapping) / sizeof (CadbLinesDqMapping[0]); i++) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tCADB[%d] CaHigh=0x%03X\tCaLow=0x%03X\tCS=0x%03X\n",
        i,
        CadbLinesDqMapping[i].CaHigh,
        CadbLinesDqMapping[i].CaLow,
        CadbLinesDqMapping[i].ChipSelect
        );
    }
#endif //MRC_DEBUG_PRINT

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, 0, Channel)) {
        continue;
      }

      SetupCaTrainingCadb (
        MrcData,
        Channel,
        Rank,
        CadbLinesDqMapping,
        sizeof (CadbLinesDqMapping) / sizeof (CadbLinesDqMapping[0]),
        CadbPatternBuffers[0],
        CadbPatternBuffers[1],
        CadbPatternBuffers[2]
        );
    }

    //
    // Run CADB pattern on both channels at the same time
    //
    ShortRunCADB (MrcData, 0x3);

    //
    // Get Results for all ch/bytes
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, 0, Channel)) {
        continue;
      }
      ChannelOut = &ControllerOut->Channel[Channel];
      ChannelIn  = &ControllerIn->Channel[Channel];

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        DramByte = ChannelIn->DqsMapCpu2Dram[Byte];
        if (ChannelOut->Dimm[dDIMM0].SdramWidth == 32) {
          if ((DramByte & 0x02) != 0) {
            //
            // Ignore upper 16 bits (in DRAM terms) on x32 devices in MRW41 feedback - DRAM bytes 2, 3, 6 and 7
            //
            Feedback[Byte] = 0;
            continue;
          }
        }
        Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
          ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
          ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
        Feedback[Byte] = (U8) (MrcReadCR (MrcData, Offset) & 0xFF);
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\t   Channel %d CPU Byte %d DRAM Byte %d => Feedback = %02X - %s feedback\n",
          Channel,
          Byte,
          DramByte,
          Feedback[Byte],
          (MrcCountBitsEqOne (Feedback[Byte]) == 1) ? "Good" : "Bad"
          );
      } // for Byte
      //
      // Store results in ChannelIn->DqMapCpu2Dram
      //
      FillCA2DQMapResult (MrcData, Channel, Feedback, Bit);
    } // for Channel
  }  // for Bit

  //
  // Exit CA training mode on rank 0 on both channels
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcRankInChannelExist (MrcData, 0, Channel)) {
      continue;
    }
    Status = LpddrCommandTrainingMode (MrcData, 1 << Channel, 1 << Rank, CaTrainingMode42);
    if (Status != mrcSuccess) {
      return Status;
    }
  }

#ifdef MRC_DEBUG_PRINT
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcRankInChannelExist (MrcData, 0, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];
    ChannelIn  = &ControllerIn->Channel[Channel];
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "\nMapCA2DQPins Results for Ch%d (\"-1\" for skipped Bytes, DRAM DQ pins offsets):\n%s",
      Channel,
      "CPU Bit: \t[0]\t[1]\t[2]\t[3]\t[4]\t[5]\t[6]\t[7]\n"
      );
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CPU Byte%d:", Byte);
      for (Bit = 0; Bit <  MAX_BITS; Bit++) {
        DramBit = ChannelIn->DqMapCpu2Dram[Byte][Bit];
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t%d", (DramBit == 255) ? -1: DramBit);
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
    }
  }
#endif //MRC_DEBUG_PRINT

  return Status;
}

/**
  Sweep the given PI up or down and find the edge.

  @param[in]  MrcData     - The MRC global data.
  @param[in]  Iteration   - Determines which PI to shift
  @param[in]  ChannelMask - Valid Channel bit mask
  @param[in]  RankMask    - Valid Rank bit mask
  @param[in]  Stop        - End of the PI range
  @param[in]  Step        - PI step for the sweep
  @param[out] Limit       - array of edge values (per channel), filled by this function
  @param[in]  DebugPrint  - Print debug messages or not

  @retval none
**/
void
CaFindEdge (
  IN  MrcParameters * const  MrcData,
  IN  U8                     Iteration,
  IN  U8                     ChannelMask,
  IN  U8                     RankMask,
  IN  S16                    Stop,
  IN  S16                    Step,
  OUT U8                     Limit[MAX_CHANNEL],
  IN  BOOL                   DebugPrint
  )
{
  MrcInput          *Inputs;
  MrcDebug          *Debug;
  MrcOutput         *Outputs;
  U8                Channel;
  U8                ChannelBit;
  U8                ChError;
  U8                DumArr[7];
  S16               PiOffset;
  BOOL              Pass;
  BOOL              Done;
  BOOL              ChannelDone[MAX_CHANNEL];

  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;
  Outputs = &MrcData->SysOut.Outputs;

  PiOffset = 0;
  Done = FALSE;
  ChannelDone[0] = ChannelDone[1] = FALSE;
  MrcOemMemorySet (DumArr, 1, sizeof (DumArr));

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (DebugPrint) ? "\t0 1\n" : "");

  while (!Done) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (DebugPrint) ? "%d:\t" : "", PiOffset);
    //
    // Update Timing
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if ((1 << Channel) & ChannelMask) {
        if (!ChannelDone[Channel]) {
          ShiftPIforCmdTraining (MrcData, Channel, Iteration, RankMask, 3, PiOffset, 0);
        }
      }
    }
    //
    // Reset DDR after changing the CLK PI
    //
    MrcResetSequence (MrcData);

    //
    // Run CPGC test on both channels
    //
    ChError = RunIOTest (MrcData, ChannelMask, Outputs->DQPat, DumArr, 1, 0);

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelBit = (1 << Channel);
      if (((ChannelBit & ChannelMask) == 0) || (ChannelDone[Channel])) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (DebugPrint && (Channel == 0)) ? "  " : "");
        continue;
      }

      Pass = !(ChError & ChannelBit);

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (DebugPrint) ? (Pass ? ". " : "# ") : "");

      if (Pass) {
        Limit[Channel] = (U8) (ABS (PiOffset));
      } else {
        ChannelDone[Channel] = TRUE;
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (DebugPrint) ? "\n" : "");

    PiOffset += Step;
    if (Step > 0) {
      // Sweep up
      Done = (PiOffset > Stop);
    } else {
      // Sweep down
      Done = (PiOffset < Stop);
    }

    if (ChannelDone[0] && ChannelDone[1]) {
      // Found the limit on both channels - no need to sweep PI any longer
      Done = TRUE;
    }
  }  // while not done
}

/**
@brief
  Sweep right and left from the current point to find the margins.

  @param[in, out] MrcData     - Include all MRC global data.
  @param[in]      Iteration   - Determines which PI to shift
  @param[in]      ChannelMask - Valid Channel bit mask
  @param[in]      RankMask    - Valid Rank bit mask
  @param[in]      DebugPrint  - Print debug messages or not

  @retval MrcStatus -  If it succeeds return mrcSuccess
**/
void
CmdLinearFindEdgesLpddr (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   Iteration,
  IN     U8                   ChannelMask,
  IN     U8                   RankMask,
  IN     BOOL                 DebugPrint
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  U8                Channel;
  U8                Rank;
  S16               PiLow;
  S16               PiHigh;
  S16               PiStep;
  U8                RightLimit[MAX_CHANNEL];
  U8                LeftLimit[MAX_CHANNEL];

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;

  //
  // We are going to sweep clock 32 PI ticks to the left and to the right
  //
  PiLow   = -32;
  PiHigh  = 32;

  PiStep = 1;

  //
  // Initialize to zero margin
  //
  MrcOemMemorySet ((U8 *) RightLimit, 0, sizeof (RightLimit));
  MrcOemMemorySet ((U8 *) LeftLimit, 0, sizeof (LeftLimit));

  //
  // Find right and left margins
  //
  CaFindEdge (MrcData, Iteration, ChannelMask, RankMask, PiHigh, PiStep, RightLimit, DebugPrint);
  CaFindEdge (MrcData, Iteration, ChannelMask, RankMask, PiLow, -PiStep, LeftLimit, DebugPrint);

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if ((1 << Channel) & ChannelMask) {
      //
      // Save margins for RMT
      //
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if ((1 << Rank) & RankMask) {
          Outputs->MarginResult[LastCmdT][Rank][Channel][0][0] = 10 * LeftLimit[Channel];
          Outputs->MarginResult[LastCmdT][Rank][Channel][0][1] = 10 * RightLimit[Channel];
        }
      }
    }
  }
}

/**
  Early CMD / CLK training for LPDDR.

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess if succeeded
**/
MrcStatus
EarlyCaTraining (
  IN MrcParameters * const MrcData
  )
{
  MrcInput                                    *Inputs;
  MrcDebug                                    *Debug;
  MrcOutput                                   *Outputs;
  MrcControllerIn                             *ControllerIn;
  MrcControllerOut                            *ControllerOut;
  MrcChannelIn                                *ChannelIn;
  MrcChannelOut                               *ChannelOut;
  MrcStatus                                   Status;
  U8                                          Channel;
  U8                                          Rank;
  U8                                          RankBit;
  U8                                          Byte;
  U8                                          DramByte;
  U8                                          RankMask;
  U32                                         Offset;
  U8                                          PiLow;
  U8                                          PiHigh;
  U8                                          PiMiddle;
  U8                                          PiStep;
  U8                                          RightLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];
  U8                                          LeftLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];
  U8                                          BestCmd[MAX_CHANNEL][2]; // per Channel and per group (CAA and CAB)
  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT      DdrCrDataControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT     DdrCrDataControl2;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerIn  = &Inputs->Controller[0];
  ControllerOut = &Outputs->Controller[0];

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "EarlyCaTraining started\n");

  Status = mrcSuccess;
  RankMask  = Outputs->ValidRankMask;

  PiLow     = 0;
  PiHigh    = 127;
  PiMiddle  = 96;
  PiStep = 2;

  MrcOemMemorySet ((U8 *) RightLimit, PiHigh, sizeof (RightLimit));
  MrcOemMemorySet ((U8 *) LeftLimit, PiLow, sizeof (LeftLimit));

  //
  // Enable the following bits because we will use DATATRAINFEEDBACK to read back CA values on DQ pins:
  //  DataControl0.SenseampTrainingMode and ForceOdtOn
  //  DataControl2.ForceRxOn and ForceBiasOn
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      DdrCrDataControl2.Data = ChannelOut->DqControl2[Byte].Data;
      DdrCrDataControl2.Bits.ForceBiasOn = 1;
      DdrCrDataControl2.Bits.ForceRxOn   = 1;
      Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
        ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
        ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
      MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
    }
    DdrCrDataControl0.Data = ChannelOut->DqControl0.Data;
    DdrCrDataControl0.Bits.ForceOdtOn           = 1;
    DdrCrDataControl0.Bits.SenseampTrainingMode = 1;
    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
    MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
  }

  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {  // @todo Do we have to do this per rank, or rank 0 is enough ?
    RankBit = 1 << Rank;
    if ((RankBit & RankMask) == 0) {
      continue;
    }

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }

      //
      // Put the current Rank in CA training mode using MRW41.
      //
      Status = LpddrCommandTrainingMode (MrcData, 1 << Channel, RankBit, CaTrainingMode41);
      if (Status != mrcSuccess) {
        return Status;
      }

      SetupCaTrainingCadb (
        MrcData,
        Channel,
        Rank,
        CadbLinesCs,
        sizeof (CadbLinesCs) / sizeof (CadbLinesCs[0]),
        0x3000,
        0x4000,
        0x0000
        );
    }  // for Channel

    //
    // Sweep CMD PI up and down from the middle, on both channels at the same time
    //
//    Ca2DMargins (MrcData, Rank);  // This is used for test
    EarlyCaFindEdge (MrcData, Rank, PiMiddle, PiHigh, PiStep, RightLimit);
    EarlyCaFindEdge (MrcData, Rank, PiMiddle, PiLow, -PiStep, LeftLimit);

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      ChannelIn  = &ControllerIn->Channel[Channel];
      ChannelOut = &ControllerOut->Channel[Channel];
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CA training data Ch%d Rank%d\nCPU Byte\tLeft\tRight\n", Channel, Rank);
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        DramByte = ChannelIn->DqsMapCpu2Dram[Byte];
        if (ChannelOut->Dimm[dDIMM0].SdramWidth == 32) {
          if ((DramByte & 0x02) != 0) {
            //
            // Ignore upper 16 bits on x32 devices in MRW41 feedback - DRAM bytes 2, 3, 6 and 7
            //
            continue;
          }
        }
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t\t%d\t%d\n",
          Byte, LeftLimit[Channel][Rank][Byte], RightLimit[Channel][Rank][Byte]);
      }

      //
      // Put the CMD PI back to middle for MRW42 command
      //
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, RankBit,        3, PiMiddle, 1);
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCke,  RankBit, (1 << 0), PiMiddle, 1);
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdN, RankBit, (1 << 1), PiMiddle, 1);

      //
      // Exit CA training mode on the current rank
      //
      Status = LpddrCommandTrainingMode (MrcData, 1 << Channel, RankBit, CaTrainingMode42);
      if (Status != mrcSuccess) {
        return Status;
      }
    } // for Channel
  }  // for Rank

  //
  // Restore original DataControl0 and DataControl2 values
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];
    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
    MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0.Data);

    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
        ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
        ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
      MrcWriteCR (MrcData, Offset, ChannelOut->DqControl2[Byte].Data);
    }
  }

  //
  // Select optimal CMD timings for both channels
  //
  FindBestCmdPi (MrcData, LeftLimit, RightLimit, BestCmd);

  //
  // Apply the new CmdN, CmdS and CKE command PI settings
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];

    //
    // CAA is controlled by CMDS.CmdPi0Code and CKE.CmdPi0Code
    //
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, RankMask, (1 << 0), BestCmd[Channel][0], 1);
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCke,  RankMask, (1 << 0), BestCmd[Channel][0], 1);

    //
    // CAB is controlled by CMDS.CmdPi1Code and CMDN.CmdPi1Code
    //
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, RankMask, (1 << 1), BestCmd[Channel][1], 1);
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdN, RankMask, (1 << 1), BestCmd[Channel][1], 1);
  } // for Channel

  return Status;
}

/**
  Early CA / CS training for LPDDR.
  Main flow:
  1. Put DRAMs in CA training mode using MRW41.
  2. Run CS vs. CLK training.
  3. Map DQ pins according to the board swizzling.
  4. Run CA vs. CLK training.
  5. Select optimal CA timings for each CA bus per rank

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess if succeeded
**/
MrcStatus
EarlyCommandTrainingLpddr (
  IN MrcParameters * const MrcData
)
{
  const MrcInput                              *Inputs;
  const MrcDebug                              *Debug;
  MrcOutput                                   *Outputs;
  MrcControllerOut                            *ControllerOut;
  MrcStatus                                   Status;
  U8                                          ChannelBitMask;
  U8                                          ValidRankMask;
//  DDRCMD_CR_DDRCRCMDPICODING_STRUCT           DdrCrCmdPiCoding;


  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];

  ValidRankMask   = Outputs->ValidRankMask;
  ChannelBitMask  = Outputs->ValidChBitMask;
  Status = mrcSuccess;

//  //
//  // Set initial Pi settings for CLK / CMD / CTL - done in MrcMcConfiguration
//  //
//  DdrCrCmdPiCoding.Data = 0;
//  DdrCrCmdPiCoding.Bits.CmdPi0Code = 96;
//  DdrCrCmdPiCoding.Bits.CmdPi1Code = 96;
//  MrcWriteCR (MrcData, DDRCMD_CR_DDRCRCMDPICODING_REG, DdrCrCmdPiCoding.Data);


  //
  // Run CPU-to-DRAM DQ Mapping - Run after 2D CS/CA and/or before CS training
  //
  Status = MapCA2DQPins (MrcData);
  if (Status != mrcSuccess) {
    return Status;
  }

  //
  // Run CS vs. CLK training
  //
  Status = EarlyChipSelectTraining (MrcData);
  if (Status != mrcSuccess) {
    return Status;
  }

  //
  // Run CA vs. CLK training
  //
  Status = EarlyCaTraining (MrcData);
  if (Status != mrcSuccess) {
    return Status;
  }

  //
  // Program DESWIZZLE_HIGH/LOW registers
  //
  ProgramDeswizzleRegisters (MrcData);

  //
  // Set this flag so that MrcResetSequence() will include MrcJedecInitLpddr3() as well.
  //
  Outputs->LpddrEctDone = TRUE;

  return Status;
}

/**
  Sweep CS Pi up or down and find edges for all bytes.
  Main flow:
  1.
  2.
  3.

  @param[in]  MrcData - The MRC global data.
  @param[out] Limit   - array of edge PI values per channel, rank and CPU byte

  @retval none
**/
void
ChipSelectFindEdge (
  IN MrcParameters * const  MrcData,
  U8                        Rank,
  U8                        Start,
  U8                        Stop,
  S8                        Step,
  OUT U8                    Limit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM]
  )
{
  MrcInput          *Inputs;
  MrcDebug          *Debug;
  MrcOutput         *Outputs;
  MrcControllerIn   *ControllerIn;
  MrcControllerOut  *ControllerOut;
  MrcChannelIn      *ChannelIn;
  MrcChannelOut     *ChannelOut;
  U8                Channel;
  U8                ChannelMask;
  U8                Byte;
  U8                ByteMask;
  U8                DramByte;
  U8                ByteDoneMask[MAX_CHANNEL];
  U8                PiCode;
  U8                Feedback[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U32               Pattern;
  U32               DelayChipSelectCadb;
  U32               Offset;
  BOOL              Done;
  BOOL              Failed;
  char              *BytesHeader;
  char              PassFail;
  DDRDATA0CH0_CR_DATATRAINFEEDBACK_STRUCT DataTrainFeedback;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerIn  = &Inputs->Controller[0];
  ControllerOut = &Outputs->Controller[0];
  ChannelMask   = 0;

  DelayChipSelectCadb = 1 * HPET_1US;

  MrcOemMemorySet (ByteDoneMask, 0, sizeof (ByteDoneMask));

  PiCode = Start;
  Done = FALSE;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t Ch0 pattern 1\t\t\t   Ch1 pattern 1\t\t     Ch 0 pattern 2\t\t       Ch 1 pattern 2\n");
  BytesHeader = "0   1   2   3   4   5   6   7     ";
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CTL PI\t %s%s%s%s\n", BytesHeader, BytesHeader, BytesHeader, BytesHeader);

  while (!Done) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d:\t", PiCode);
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCtl, 1 << Rank, 1, PiCode, 0);
    }

    //
    // Try two different paterns (0x2AA or 0x155), to see if the command is still decoded correctly
    //
    for (Pattern = 0; Pattern <= 1; Pattern++) {
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
          continue;
        }
        ChannelMask |= (1 << Channel);
        SetCadbPatternBuffers (MrcData, Channel, (Pattern == 0) ? 0x3000 : 0x7000, 0x4000, 0x0000);
      }

      //
      // Run CADB pattern on selected channels at the same time
      //
      ShortRunCADB (MrcData, ChannelMask);
      MrcWait (MrcData, DelayChipSelectCadb);

      //
      // Read and process the results
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
          ByteDoneMask[Channel] = 0xFF;
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "                                  ");
          continue;
        }
        if (ByteDoneMask[Channel] == 0xFF) { // All bytes failed on this channel, no need to sweep anymore
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "                                  ");
          continue;
        }
        ChannelIn  = &ControllerIn->Channel[Channel];
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          ByteMask = (1 << Byte);
          DramByte = ChannelIn->DqsMapCpu2Dram[Byte];
          if (ChannelOut->Dimm[dDIMM0].SdramWidth == 32) {
            if ((DramByte & 0x02) != 0) {
              //
              // Ignore upper 16 bits on x32 devices in MRW41 feedback - DRAM bytes 2, 3, 6 and 7
              //
              ByteDoneMask[Channel] |= ByteMask;
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    ");
              continue;
            }
          }
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
          DataTrainFeedback.Data = (MrcReadCR (MrcData, Offset) & 0xFF);  // Get only DQ bits, not DQS
          PassFail = '#';
          if ((ByteDoneMask[Channel] & ByteMask) == 0) {
            if (Pattern == 0) {
              //
              // First pattern
              //
              Feedback[Channel][Byte] = (U8) DataTrainFeedback.Data;
              PassFail = ' ';
            } else {
              //
              // Second Pattern
              // If still read the same data, then DRAM was not able to decode the new command
              //
              Failed = FALSE;
              if (Feedback[Channel][Byte] == (U8) DataTrainFeedback.Data) {
                Failed = TRUE;
              }
              if (MrcCountBitsEqOne (DataTrainFeedback.Data) != 4) {
                Failed = TRUE;
              }
              if (Failed) {
                Limit[Channel][Rank][Byte] = PiCode;
                ByteDoneMask[Channel] |= ByteMask;
              } else {
                PassFail = '.';
              }
            }
          }
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%c%02X ", PassFail, DataTrainFeedback.Data);
        }  // for Byte
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  ");
      }  // for Channel
    } // for Pattern
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

    if ((ByteDoneMask[0] == 0xFF) && (ByteDoneMask[1] == 0xFF)) {
      // Found the limit on all bytes on both channels - no need to sweep Pi any longer
      break;
    }

    PiCode += Step;
    if (Step > 0) {
      // Sweep up
      Done = (PiCode > Stop);
    } else {
      // Sweep down
      Done = (((S8) PiCode) < Stop);
    }
  }  // while not done
}

/**
  Process the results of the early LPDDR3 CS training and find the best PI settings for CS and CLK.
  Flow:
   1. Find the worst case Right and Left limits for each group
   2. Find the Center for each group

  @param[in]  MrcData     - The MRC global data.
  @param[in]  LeftLimit   - array of left edge values per channel, rank and CPU byte
  @param[in]  RightLimit  - array of right edge values per channel, rank and CPU byte
  @param[out] BestCs      - array of best CS PI settings, per channel and group
  @param[out] BestClk     - array of best CLK PI settings, per channel and group

  @retval mrcSuccess if succeeded
**/
MrcStatus
FindBestCsClkPi (
  IN  MrcParameters * const MrcData,
  IN  U8                    LeftLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM],
  IN  U8                    RightLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM],
  OUT U8                    BestCs[MAX_CHANNEL][2],
  OUT U8                    BestClk[MAX_CHANNEL][2]
  )
{
  MrcInput          *Inputs;
  MrcDebug          *Debug;
  MrcOutput         *Outputs;
  MrcChannelIn      *ChannelIn;
  MrcStatus         Status;
  U8                Channel;
  U8                Rank;
  U8                Byte;
  U8                Group;
  U8                Count;
  U8                GroupLeftLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][2];  // Per ch, rank and group
  U8                GroupRightLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][2]; // Per ch, rank and group
  U8                GroupCenter[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][2];     // Per ch, rank and group
  S8                ClkDelta[MAX_RANK_IN_CHANNEL];
  S8                MeanClkDelta;
  S8                CsDelta[2];  // Per group
  S8                MeanCsDelta;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;

  Status = mrcSuccess;

  MrcOemMemorySet ((U8 *) GroupRightLimit, 127, sizeof (GroupRightLimit));
  MrcOemMemorySet ((U8 *) GroupLeftLimit, 0, sizeof (GroupLeftLimit));
  MrcOemMemorySet ((U8 *) GroupCenter, 0, sizeof (GroupCenter));

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Finding best CS/CLK PIs:\n");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel: %d\tLeft\tRight\tCenter\n", Channel);
    ChannelIn = &Inputs->Controller[0].Channel[Channel];
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      for (Group = 0; Group < 2; Group++) {
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          //
          // Find the worst case Right and Left limits for each group
          //
          if (ChannelIn->DQByteMap[MrcIterationClock][Group] & (1 << Byte)) {
            if (GroupRightLimit[Channel][Rank][Group] > RightLimit[Channel][Rank][Byte]) {
              GroupRightLimit[Channel][Rank][Group] = RightLimit[Channel][Rank][Byte];
            }

            if (GroupLeftLimit[Channel][Rank][Group] < LeftLimit[Channel][Rank][Byte]) {
              GroupLeftLimit[Channel][Rank][Group] = LeftLimit[Channel][Rank][Byte];
            }
          }
        } // for Byte

        //
        // Find the Center for each group
        //
        GroupCenter[Channel][Rank][Group] =
          (GroupRightLimit[Channel][Rank][Group] + GroupLeftLimit[Channel][Rank][Group]) / 2;

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "Rank%d CLK%d\t%d\t%d\t%d\n",
          Rank,
          Group,
          GroupLeftLimit[Channel][Rank][Group],
          GroupRightLimit[Channel][Rank][Group],
          GroupCenter[Channel][Rank][Group]
          );
      } // for Group
    } // for Rank

    //
    // Find the CS delta between ranks for each clock group, and then group average
    //
    for (Count = 0, Group = 0; Group < 2; Group++) {
      if (MrcRankInChannelExist (MrcData, 1, Channel)) {
        CsDelta[Group] = (GroupCenter[Channel][1][Group] - GroupCenter[Channel][0][Group]);
        Count++;
      } else {
        CsDelta[Group] = 0; // Single rank 0 case
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CLK%d delta (Rank0-Rank1) = %d  \n", Group, CsDelta[Group]);
    }

    MeanCsDelta = (Count != 0) ? (CsDelta[0] + CsDelta[1]) / Count : 0;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Mean CS delta = %d\n", MeanCsDelta);

    //
    // Find the Clock delta for each rank, and then average between ranks
    // @todo Add case of single CLK group
    //
    for (Count = 0, Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
        ClkDelta[Rank] = (GroupCenter[Channel][Rank][1] - GroupCenter[Channel][Rank][0]);
        Count++;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Rank%d delta (CLK1-CLK0) = %d  \n", Rank, ClkDelta[Rank]);
      } else {
        ClkDelta[Rank] = 0; // No such rank
      }
    }
    MeanClkDelta = (Count != 0) ? (ClkDelta[0] + ClkDelta[1]) / Count : 0;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Mean Clock delta = %d\n", MeanClkDelta);

    BestClk[Channel][0] = (U8) (64 - MeanClkDelta / 2);               // CLK0
    BestClk[Channel][1] = (U8) (BestClk[Channel][0] + MeanClkDelta);  // CLK1

    BestCs[Channel][0] = (GroupCenter[Channel][0][0] + GroupCenter[Channel][0][1]) / 2; // CS0
    BestCs[Channel][1] = (GroupCenter[Channel][1][0] + GroupCenter[Channel][1][1]) / 2; // CS1

    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "Best PI CLK0=%d, CLK1=%d, CS0=%d, CS1=%d\n",
      BestClk[Channel][0],
      BestClk[Channel][1],
      BestCs[Channel][0],
      BestCs[Channel][1]
      );
  } // for Channel

  return Status;
}

/**
  Early CS / CLK training for LPDDR.
  Main flow:
  1. Setup CADB pattern for CS Training.
  2. Run CS vs. CLK training.
  3. Select optimal CS and CLK timings

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess if succeeded
**/
MrcStatus
EarlyChipSelectTraining (
  IN MrcParameters * const MrcData
  )
{
  MrcInput                                    *Inputs;
  MrcDebug                                    *Debug;
  MrcOutput                                   *Outputs;
  MrcControllerIn                             *ControllerIn;
  MrcControllerOut                            *ControllerOut;
  MrcChannelIn                                *ChannelIn;
  MrcChannelOut                               *ChannelOut;
  MrcStatus                                   Status;
  BOOL                                        ClockPiChanged;
  U8                                          Channel;
  U8                                          Rank;
  U8                                          RankBit;
  U8                                          Byte;
  U8                                          DramByte;
  U8                                          Group;
  U8                                          RankMask;
  U32                                         Offset;
  S32                                         ClkDelta;
  U8                                          PiLow;
  U8                                          PiHigh;
  U8                                          PiMiddle;
  U8                                          PiStep;
  U8                                          RightLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];
  U8                                          LeftLimit[MAX_CHANNEL][MAX_RANK_IN_CHANNEL][MAX_SDRAM_IN_DIMM];
  U8                                          BestCs[MAX_CHANNEL][2];
  U8                                          BestClk[MAX_CHANNEL][2];
  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT      DdrCrDataControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT     DdrCrDataControl2;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerIn  = &Inputs->Controller[0];
  ControllerOut = &Outputs->Controller[0];

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "EarlyChipSelectTraining started\n");

  Status = mrcSuccess;
  RankMask  = Outputs->ValidRankMask;

  PiLow     = 0;
  PiHigh    = 127;
  PiMiddle  = 64;
  PiStep = 2;

  MrcOemMemorySet ((U8 *) RightLimit, PiHigh, sizeof (RightLimit));
  MrcOemMemorySet ((U8 *) LeftLimit, PiLow, sizeof (LeftLimit));

  //
  // Enable the following bits because we will use DATATRAINFEEDBACK to read back CA values on DQ pins:
  //  DataControl0.SenseampTrainingMode and ForceOdtOn
  //  DataControl2.ForceRxOn and ForceBiasOn
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      DdrCrDataControl2.Data = ChannelOut->DqControl2[Byte].Data;
      DdrCrDataControl2.Bits.ForceBiasOn = 1;
      DdrCrDataControl2.Bits.ForceRxOn   = 1;
      Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
        ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
        ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
      MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
    }
    DdrCrDataControl0.Data = ChannelOut->DqControl0.Data;
    DdrCrDataControl0.Bits.ForceOdtOn           = 1;
    DdrCrDataControl0.Bits.SenseampTrainingMode = 1;
    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
    MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
  }

  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    RankBit = 1 << Rank;
    if ((RankBit & RankMask) == 0) {
      continue;
    }

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }

      //
      // Put the current Rank in CA training mode using MRW41.
      //
      Status = LpddrCommandTrainingMode (MrcData, 1 << Channel, RankBit, CaTrainingMode41);
      if (Status != mrcSuccess) {
        return Status;
      }

      SetupCaTrainingCadb (
        MrcData,
        Channel,
        Rank,
        CadbLinesCs,
        sizeof (CadbLinesCs) / sizeof (CadbLinesCs[0]),
        0x3000,
        0x4000,
        0x0000
        );
    }  // for Channel

    //
    // Sweep CS Pi up and down from the middle, on both channels at the same time
    //
    ChipSelectFindEdge (MrcData, Rank, PiMiddle, PiHigh, PiStep, RightLimit);
    ChipSelectFindEdge (MrcData, Rank, PiMiddle, PiLow, -PiStep, LeftLimit);

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      ChannelIn  = &ControllerIn->Channel[Channel];
      ChannelOut = &ControllerOut->Channel[Channel];
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CS training data Ch%d Rank%d\nCPU Byte\tLeft\tRight\n", Channel, Rank);
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        DramByte = ChannelIn->DqsMapCpu2Dram[Byte];
        if (ChannelOut->Dimm[dDIMM0].SdramWidth == 32) {
          if ((DramByte & 0x02) != 0) {
            //
            // Ignore upper 16 bits on x32 devices in MRW41 feedback - DRAM bytes 2, 3, 6 and 7
            //
            continue;
          }
        }
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t\t%d\t%d\n",
          Byte, LeftLimit[Channel][Rank][Byte], RightLimit[Channel][Rank][Byte]);
      }

      //
      // Put the CTL PI back to middle for MRW42 command
      //
      ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCtl, 3, 1, PiMiddle, 1);

      //
      // Exit CA training mode on the current rank
      //
      Status = LpddrCommandTrainingMode (MrcData, 1 << Channel, RankBit, CaTrainingMode42);

      if (Status != mrcSuccess) {
        return Status;
      }
    } // for Channel
  }  // for Rank

  //
  // Restore original DataControl0 and DataControl2 values
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];
    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
    MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0.Data);

    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
        ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
        ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
      MrcWriteCR (MrcData, Offset, ChannelOut->DqControl2[Byte].Data);
    }
  }

  //
  // Select optimal CS and CLK timings for both channels
  //
  FindBestCsClkPi (MrcData, LeftLimit, RightLimit, BestCs, BestClk);

  //
  // Apply the new CTL and CLK PI settings
  //
  ClockPiChanged = FALSE;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelIn  = &ControllerIn->Channel[Channel];
    ChannelOut = &ControllerOut->Channel[Channel];
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }
      //
      // Shift CS per rank
      //
      ShiftPIforCmdTraining (
        MrcData,
        Channel,
        MrcIterationCtl,
        1 << Rank,
        1,
        BestCs[Channel][Rank],
        1            // UpdateHost
        );
    }
    for (Group = 0; Group < 2; Group++) {
      //
      // Shift CLK per group, if needed, and update host struct
      //
      ClkDelta = (S32) BestClk[Channel][Group] - (S32) ChannelOut->ClkPiCode[Group];
      if (ClkDelta != 0) {
        ClockPiChanged = TRUE;
        ShiftPIforCmdTraining (MrcData, Channel, MrcIterationClock, RankMask, 1 << Group, ClkDelta, 1);

        //
        // Shift the corresponding CMD PI by the same amount as the CLK
        //
        if (ChannelIn->DQByteMap[MrcIterationClock][Group] == ChannelIn->DQByteMap[MrcIterationCmdS][0]) {
          //
          // CAA is controlled by CMDS.CmdPi0Code and CKE.CmdPi0Code
          //
          ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, RankMask, (1 << 0),
            ChannelOut->CmdsCmdPiCode[0] + ClkDelta, 1);
          ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCke,  RankMask, (1 << 0),
            ChannelOut->CkeCmdPiCode[0] + ClkDelta, 1);
        }
        if (ChannelIn->DQByteMap[MrcIterationClock][Group] == ChannelIn->DQByteMap[MrcIterationCmdS][1]) {
          //
          // CAB is controlled by CMDS.CmdPi1Code and CMDN.CmdPi1Code
          //
          ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdS, RankMask, (1 << 1),
            ChannelOut->CmdsCmdPiCode[1] + ClkDelta, 1);
          ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCmdN, RankMask, (1 << 1),
            ChannelOut->CmdnCmdPiCode[1] + ClkDelta, 1);
        }
      }
    }
  } // for Channel

  //
  // Perform IO reset and JEDEC reset if clock PI was changed.
  //
  if (ClockPiChanged) {
    MrcResetSequence (MrcData);
  }

  return Status;
}

////
//// CA to DQ mapping during MRW41
//// First index is rising edge (0) / falling edge (1)
//// Second index is CA pin
////
//U8 CA2DQMapping[2][9] = {
//  { 0x00,0x02,0x04,0x06,0x10,0x08,0x0A,0x0C,0x0E },
//  { 0x01,0x03,0x05,0x07,0x11,0x09,0x0B,0x0D,0x0F }
//};
//
///**
//
//  Map byte to group for the given Iteration
//
//  @param[in] MrcData - The MRC global data.
//  @param[in] Iteration
//  @param[in] Byte
//
//  @retval Group
//
//**/
//
//U8
//MapByte2Group (
//  MrcParameters * const MrcData,
//  U8                    Iteration,
//  U8                    Channel,
//  U8                    Byte
//  )
//{
//  MrcInput          *Inputs;
//  MrcControllerIn   *ControllerIn;
//  MrcChannelIn      *ChannelIn;
//  U32               ByteMask;
//  U32               SafeIteration;
//  U8                Group;
//  U8                SafeGroup;
//  U8                TargetGroup;
//
//  Inputs        = &MrcData->SysIn.Inputs;
//  ControllerIn  = &Inputs->Controller[0];
//  ChannelIn     = &ControllerIn->Channel[Channel];
//
//  SafeIteration = 2;            // CmdS has a mapping for all bytes (uses CAA & CAB)
//  SafeGroup     = 0;            // This is not true for all iterations
//  TargetGroup   = 0;            // ex: CmdN and CKE only use one of the CAA/CAB buses
//  ByteMask      = 0;            // ex: Clk may or may not go to all bytes
//
//  for (Group = 0; Group < 2; Group++) {
//    ByteMask |= ChannelIn->DQByteMap[Iteration][Group];
//    if (ChannelIn->DQByteMap[Iteration][Group] & (1 << Byte)) {
//      TargetGroup = Group;
//    }
//    if (ChannelIn->DQByteMap[SafeIteration][Group] & (1 << Byte)) {
//      SafeGroup = Group;
//    }
//  }
//
//  if (ByteMask == 0xFF) {
//    return TargetGroup;
//  } else {
//    return SafeGroup;
//  }
//}
//
///**
//
//  Update the DRAM to CPU DQ mapping for a given byte based on CA training results.
//  DQMapping is from CPU to DRAM
//  Victim is the CA victim bit
//  ByteFB is an 8 bit value that we received back for a Walking 0 VA pattern
//  Diff is an 8 bit value with ones in the victim bit positions
//  Return 0 if everything made sense, otherwise returns (1<<Group) to indicate an error
//  In the case of an error, DQMapping is not updated
//
//  @param[in] MrcData - The MRC global data.
//
//  @retval 0            - if a valid mapping was found
//  @retval (1 << Group) - in case of error
//
//**/
//U8
//UpdateDQMapping (
//  IN OUT U8 DQMapping[64],
//  IN U8 Byte,
//  IN U8 ByteFeedback,
//  IN U8 Diff,
//  IN U8 Victim,
//  IN U8 Group
//  )
//{
//  U32 BitsHigh;
//  U8  DramDQHigh;
//  U8  DramDQLow;
//  U8  CpuDQHigh;
//  U8  CpuDQLow;
//  U8  Bit;
//  U8  BitValue;
//
//  if (Diff == 0) {
//    return 0;   // No error to look at
//  }
//
//  //
//  // Should never see an error with these victim lanes since their data returns with MRW 48
//  // This function is only used with data from MRW 41
//  if ((Victim == 4) || (Victim == 9)) {
//    return (1 << Group);
//  }
//
//  //
//  // Walk through the bits with errors and figure out the mapping
//  //
//  BitsHigh    = 0;
//  DramDQHigh  = 0xFF;
//  DramDQLow   = 0xFF;
//  CpuDQHigh   = 0xFF;
//  CpuDQLow    = 0xFF;
//
//  for (Bit = 0; Bit < 8; Bit++) {
//    BitValue = 1 << Bit;
//    if ((Diff & BitValue) != 0) {
//      BitsHigh++;                           // Count number of mismatches.  Should be exactly 2
//      if ((ByteFeedback & BitValue) != 0) { // 1 on Victim bit during WalkZero indicates low phase
//        CpuDQLow  = Bit;
//        DramDQLow = CA2DQMapping[1][Victim] & 0xF;
//      } else {                              // 0 on Victim bit during WalkZero indicates high phase
//        CpuDQHigh  = Bit;
//        DramDQHigh = CA2DQMapping[0][Victim] & 0xF;
//      }
//    }
//  }
//
//  if ((BitsHigh == 2) && (CpuDQLow != 0xFF) && (CpuDQHigh != 0xFF)) {
//    //
//    // This is a valid feedback that makes sense
//    //
//    DQMapping[8 * Byte + CpuDQLow]  = (Group << 4) + DramDQLow;
//    DQMapping[8 * Byte + CpuDQHigh] = (Group << 4) + DramDQHigh;
//    return 0;
//  }
//
//  //
//  // This is not a valid feedback
//  //
//  return (1 << Group);
//}
//
///**
//
//  Find the DRAM to CPU DQ mapping based on CA training results.
//  DQMapping is from CPU to DRAM
//  ByteFB is an 8 bit value that we received back for a Walking 0 VA pattern
//
//  @param[in] MrcData    - The MRC global data.
//  @param[in] ByteFB     - Array of Ch X Byte for WalkZero results (WalkOne will just be the inverse)
//  @param[in] DQMapping  - Array of Ch x 64 bits for mapping X64 CPU to X16 DRAM
//  @param[in] X16Count   - Array of Ch x Group that indicates how many X16 words are in a given ch/group
//                          Usually this is either 0 or 2 (ie: 0 bits or 32 bits)
//  @param[in] ChError    - Arrray of Ch with a bit mask indicating if this Ch/Group had an error
//
//  @retval none
//
//**/
//
//void
//FindECTCpu2DramMapping (
//  MrcParameters * const MrcData,
//  U8                    ChBitMask,
//  U8                    Iteration,
//  U8                    ByteFB[MAX_CHANNEL][8][11],
//  IN OUT U8             DQMapping[MAX_CHANNEL][64],
//  IN U8                 X16Count[MAX_CHANNEL][2],
//  OUT U8                ChError[MAX_CHANNEL],
//  BOOL                  UpdateDqMapping
//  )
//{
//  MrcOutput         *Outputs;
//  MrcChannelOut     *ChannelOut;
//  MrcControllerOut  *ControllerOut;
//  U8                Channel;
//  U8                Byte;
//  U8                Bit;
//  U8                Group;
//  U8                Victim;
//  U8                RefFB;
//  U8                Diff;
//  U8                DramDQ;
//  U8                CountMappings[2][16];
//
//  Outputs       = &MrcData->SysOut.Outputs;
//  ControllerOut = &Outputs->Controller[0];
//
//  MrcOemMemorySet ((U8 *) CountMappings, 0, sizeof (CountMappings));
//
//  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
//    if (!MrcChannelExist (Outputs, Channel)) {
//      continue;
//    }
//    if (((1 << Channel) & ChBitMask) == 0) {
//      continue;
//    }
//    ChannelOut = &ControllerOut->Channel[Channel];
//
//    //
//    // Figure out mappings for all lanes by comparing different walking zero results.
//    // XOR the Victim=10 vs Victim=N results
//    // During Victim=10, all CA lanes drive 0101
//    // During Victim<10, all CA lanes drive 0101 except the victim, which drives 1010
//    // Use this and the mapping of CA2DQ lanes (JEDEC spec) to figure out the mapping.
//    //
//    for (Byte = 0; Byte < 8; ++Byte) {
//      RefFB = ByteFB[Channel][Byte][10];                // All lanes drive WalkZero Agg = 0101
//      Group = MapByte2Group (MrcData, Iteration, Channel, Byte); // Find Byte-to-Group mapping
//
//      for (Victim = 0; Victim < 10; ++Victim) {
//        Diff = RefFB ^ ByteFB[Channel][Byte][Victim];   // XOR should produce either (0,2) diffs
//        ChError[Channel] |= UpdateDQMapping (DQMapping[Channel], Byte, ByteFB[Channel][Byte][Victim], Diff, Victim, Group);
//      }
//    }
//
//    //
//    // Check results by making sure everybody is mapped exactly the right number of times.
//    // DQMapping defines the mapping from 64 CPU DQ lanes to 16 DRAM DQ lanes.
//    // As a result, we should see each DRAM   DQ lane is listed exactly X16Count times.
//    //
//    for (Byte = 0; Byte < 8; ++Byte) {
//      for (Bit = 0; Bit < 8; ++Bit) {
//        DramDQ =  DQMapping[Channel][8 * Byte + Bit] & 0xF;
//        Group  = (DQMapping[Channel][8 * Byte + Bit] >> 4) & 1;
//        CountMappings[Group][DramDQ]++;
//      }
//    }
//
//    for (Group = 0; Group < 2; ++Group) {
//      for (Bit = 0; Bit < 16; ++Bit) {
//        if (CountMappings[Group][Bit] != X16Count[Channel][Group]) {
//          ChError[Channel] |= (1 << Group);
//        }
//      }
//    }
//
//    //
//    // Update the DQ Mapping in the host structure if needed
//    //
//    if (UpdateDqMapping && (ChError[Channel] == 0)) {
//      for (Byte = 0; Byte < 8; ++Byte) {
//        for (Bit = 0; Bit < 8; ++Bit) {
//          ChannelOut->DqMapCpu2Dram[Byte][Bit] = DQMapping[Channel][8 * Byte + Bit] & 0xF;
//        }
//      }
//    }
//  } // for Channel
//}
//
///**
//
//  Check if the CA training test is done (everybody has already failed) or we need to keep running
//
//  @param[in] MrcData   - The MRC global data.
//  @param[in] ChBitMask - channels to work on
//  @param[in] Iteration -
//  @param[in] ChError   - Array of error values per channel, each element is a bitmask per group, '1' means error.
//
//  @retval TRUE if the test is done, FALSE otherwise
//
//**/
//BOOL
//CADBLPDDR3TestIsDone (
//  IN MrcParameters  * const MrcData,
//  IN U8                     ChBitMask,
//  IN U8                     Iteration,
//  IN U8                     ChError[MAX_CHANNEL]
//  )
//{
//  MrcInput          *Inputs;
//  MrcControllerIn   *ControllerIn;
//  MrcChannelIn      *ChannelIn;
//  U8                Channel;
//  U8                Group;
//
//  Inputs        = &MrcData->SysIn.Inputs;
//  ControllerIn  = &Inputs->Controller[0];
//
//  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
//    if (((1 << Channel) & ChBitMask) == 0) {
//      continue;
//    }
//    ChannelIn  = &ControllerIn->Channel[Channel];
//    for (Group = 0; Group < 2; ++Group) {
//      if ((ChannelIn->DQByteMap[Iteration][Group] > 0) && ((ChError[Channel] & (1 << Group)) == 0)) {
//        //
//        // Not done if at least 1 valid ch/group is still passing
//        //
//        return FALSE;
//      }
//    }
//  }
//
//  //
//  // If we reach this point, we are done
//  //
//  return TRUE;
//}
//
///**
//
//  Programs the CADB to output either a WalkingOne or WalkingZero pattern on VictimBit.
//  Each CA[9:0] lane will toggle with a 1010 pattern but the Victim lane will be inverted.
//  For a WalkingOne pattern, CAHi[Vic] = 1 and CALo[Vic] = 0.
//  For a WalkingZero pattern, CAHi[Vic] = 0 and CALo[Vic] = 1.
//  (VictimBit == 10) is a special case where all CAHi/CALo lanes drive ~WalkOne/WalkOne
//
//  Sets up 3 CADB lines that will be used to send out a CA pattern.
//
//  Example for VictimBit = 0, WalkOne = 0:
//
//  -----------------------
//  CADB  Phase  Phase  CS
//  Line  High   Low
//  -----------------------
//   0    0x000  0x000  Off  // For delay between CA patterns
//   1    0x3FE  0x001  On   // CA pattern for rank 0
//   2    0x3FE  0x001  On   // CA pattern for rank 1
//
//  The CS pattern uses Pattern Buffer and hence has 16 lines, with CS active for one line only.
//  This will send command every 16 DCLKs.
//
//  Pattern Buffer details:
//  The line order is:
//   for rank 0:    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//   for rank 1:    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//
//  001 or 010
//  000
//  000
//  000
//
//  000
//  000
//  000
//  000
//
//  000
//  000
//  000
//  000
//
//  000
//  000
//  000
//  000
//  ----
//  001 or 010  --> PB[0] = 0x0001 or 0x0000
//  000             PB[1] = 0x0000 or 0x0001
//  000             PB[2] = 0x0000
//  000
//
//  @param[in] MrcData   - The MRC global data.
//  @param[in] ChBitMask - channels to work on
//  @param[in] VictimBit - The Victim bit
//  @param[in] WalkOne   - '1': Use WalkinOne pattern, '0': use WalkingZero pattern.
//
//  @retval TRUE if the test is done, FALSE otherwise
//
//**/
//void
//SetupCADBLPDDR3VaPattern (
//  IN MrcParameters  * const MrcData,
//  IN U8                     ChBitMask,
//  IN U8                     VictimBit,
//  IN U8                     WalkOne
//  )
//{
//  /*
//  CADB_LINE CadbLinesCs[] = {
//    { 0x3FF, 0x3FF, 0 },
//    { 0x000, 0x000, 0 },
//    { 0x2AA, 0x2AA, 1 },
//    { 0x155, 0x155, 1 }
//  };
//
//  SetupCaTrainingCadb (
//        MrcData,
//        Channel,
//        Rank,
//        CadbLinesCs,
//        sizeof (CadbLinesCs) / sizeof (CadbLinesCs[0]),
//        0x9FFE,
//        0x4000,
//        0x0000
//        );
//  */
//}
//
///**
//
//  Runs through a VA test on all CH/Ranks for the current CMD PI timing.
//  Setups the test, walks though a walking one/zero pattern with each lane as a victim .
//  If UpdateDqMapping, it will also update host.ch[ch].DQMappingCpu2Dram
//  Returns a pass/fail based on Ch/Group in ChError
//
//  Main flow:
//  1.
//  2.
//  3.
//
//  @param[in]      MrcData - The MRC global data.
//  @param[in, out] ChError - Array of error values per channel, each element is a bitmask per group, '1' means error.
//
//  @retval mrcSuccess if succeeded
//
//**/
//MrcStatus
//RunCADBLPDDR3VATest (
//  IN MrcParameters  * const MrcData,
//  IN U8                     ChBitMask,
//  IN U8                     Ranks,
//  IN U8                     Iteration,
//  IN OUT U8                 ChError[MAX_CHANNEL],
//  IN BOOL                   UpdateDqMapping
//  )
//{
//  MrcInput          *Inputs;
//  MrcDebug          *Debug;
//  MrcStatus         Status;
//  MrcOutput         *Outputs;
//  MrcControllerIn   *ControllerIn;
//  MrcControllerOut  *ControllerOut;
//  MrcChannelIn      *ChannelIn;
//  MrcChannelOut     *ChannelOut;
//  U32               Offset;
//  U8                Channel;
//  U8                Rank;
//  U8                Group;
//  U8                BytesPerGroup;
//  U8                WalkOne;
//  U8                VictimBit;
//  U8                ByteFB[MAX_CHANNEL][8][11];
//  U8                X16Count[MAX_CHANNEL][2];   // How many X16 words for each ch/group
//  U8                DQMapping[MAX_CHANNEL][64]; // Lower nibble is X64 CPU lane to X16 DRAM lanes
//                                                // (this does not unswizzle byte/words)
//                                                // Upper nibble is which group this X64 CPU lane belongs to (0 or 1)
//  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT      DdrCrDataControl0;
//
//  Inputs        = &MrcData->SysIn.Inputs;
//  Debug         = &Inputs->Debug;
//  Outputs       = &MrcData->SysOut.Outputs;
//  ControllerOut = &Outputs->Controller[0];
//  ControllerIn  = &Inputs->Controller[0];
//
//  MrcOemMemorySet ((U8 *) ChError, 0, sizeof (ChError));
//  MrcOemMemorySet ((U8 *) ByteFB, 0, sizeof (ByteFB));
//  MrcOemMemorySet ((U8 *) X16Count, 0, sizeof (X16Count));
//  MrcOemMemorySet ((U8 *) DQMapping, 0, sizeof (DQMapping));
//
//  //
//  // Count number of ranks that are being tested
//  //
//  if (MrcCountBitsEqOne (Ranks) == 0) {
//    return mrcSuccess;
//  }
//
//  //
//  // Count how many X16 devices are associated with each group
//  // This will be used to determine how many Ones/Zeros are set in each results
//  //
//  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
//    if (((1 << Channel) & ChBitMask) == 0) {
//      continue;
//    }
//    ChannelIn  = &ControllerIn->Channel[Channel];
//    ChannelOut = &ControllerOut->Channel[Channel];
//    for (Group = 0; Group < 2; Group++) {
//      if (ChannelIn->DQByteMap[Iteration][Group] > 0) {
//        BytesPerGroup = MrcCountBitsEqOne(ChannelIn->DQByteMap[Iteration][Group]);
//        if ((BytesPerGroup & 1) != 0) {
//          MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR - Cannot have odd number of bytes per group\n");
//          return mrcFail;
//        }
//        X16Count[Channel][Group] = BytesPerGroup / 2;
//      }
//    }
//
//    //
//    // Enable DataControl0.SenseampTrainingMode because we will use DATATRAINFEEDBACK to read back CA values on DQ pins.
//    // Also enable DataControl0.ForceOdtOn
//    //
//    DdrCrDataControl0.Data = ChannelOut->DqControl0;
//    DdrCrDataControl0.Bits.SenseampTrainingMode = 1;
//    DdrCrDataControl0.Bits.ForceOdtOn           = 1;
//    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
//      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
//    MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
//  }
//
//  //
//  // Put DRAM into MRW 41 CA Training mode
//  //
//  Status = LpddrCommandTrainingMode (MrcData, ChBitMask, Ranks, CaTrainingMode41);
//  if (Status != mrcSuccess) {
//    return Status;
//  }
//
//  //
//  // Run associated VA tests for MRW 41 (16 out of 20 CA bits)
//  //
//  for (WalkOne = 0; WalkOne < 2; WalkOne++) {
//    for (VictimBit = 0; VictimBit < 11; VictimBit++) {
//      //
//      // Are we done ?
//      //
//      if (CADBLPDDR3TestIsDone (MrcData, ChBitMask, Iteration, ChError)) {
//        break;
//      }
//
//      //
//      // Program VA pattern in the CADB
//      //
//      SetupCADBLPDDR3VaPattern (MrcData, ChBitMask, VictimBit, WalkOne);
//
//      //
//      // Run test on 1 rank at a time and read out the results
//      //
//      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
//        if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
//          continue;
//        }
//        if ((Ranks & (1 << Rank)) == 0) {
//          continue;
//        }
////@todo:        IssueCADBLPDDR3CmdTest (MrcData, ChBitMask, 1 << Rank, 3); // Operation = 3
//
///* @todo:        ReadECTLPDDR3Results (
//          MrcData,
//          ChBitMask,
//          Iteration,
//          WalkOne,
//          ByteFB,
//          VictimBit,
//          DQMapping,
//          TRUE,       // Using MRW41
//          X16Count,
//          ChError
//          ); */
//      }
//    }
//  }
//
//  //
//  // Figure out DQ Lane Mapping for later steps and to ensure this sampled good data
//  // If results do not match expectations, updates ChError appropriately
//  //
//  FindECTCpu2DramMapping (MrcData, ChBitMask, Iteration, ByteFB, DQMapping, X16Count, ChError, UpdateDqMapping);
//
//  //
//  // Put DRAM into MRW 48 CA Training Mode
//  //
//  Status = LpddrCommandTrainingMode (MrcData, ChBitMask, Ranks, CaTrainingMode48);
//  if (Status != mrcSuccess) {
//    return Status;
//  }
//
//  //
//  // Run associated VA tests for MRW 48 (4 out of 20 CA bits)
//  //
//  for (WalkOne = 0; WalkOne < 2; WalkOne++) {
//    for (VictimBit = 0; VictimBit < 11; VictimBit++) {
//      //
//      // Are we done ?
//      //
//      if (CADBLPDDR3TestIsDone (MrcData, ChBitMask, Iteration, ChError)) {
//        break;
//      }
//
//      //
//      // Program VA pattern in the CADB
//      //
//      SetupCADBLPDDR3VaPattern (MrcData, ChBitMask, VictimBit, WalkOne);
//
//      //
//      // Run test on 1 rank at a time and read out the results
//      //
//      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
//        if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
//          continue;
//        }
//        if ((Ranks & (1 << Rank)) == 0) {
//          continue;
//        }
////@todo:        IssueCADBLPDDR3CmdTest (MrcData, ChBitMask, 1 << Rank, 3); // Operation = 3
//
///* @todo:        ReadECTLPDDR3Results (
//          MrcData,
//          ChBitMask,
//          Iteration,
//          WalkOne,
//          ByteFB,
//          VictimBit,
//          DQMapping,
//          FALSE,       // Using MRW48
//          X16Count,
//          ChError
//          ); */
//      }
//    }
//  }
//
//  //
//  // Exit CA training mode using MRW42
//  //
//  Status = LpddrCommandTrainingMode (MrcData, ChBitMask, Ranks, CaTrainingMode42);
//  if (Status != mrcSuccess) {
//    return Status;
//  }
//
//  //
//  // Restore DataControl0
//  //
//  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
//    if (((1 << Channel) & ChBitMask) == 0) {
//      continue;
//    }
//    ChannelOut = &ControllerOut->Channel[Channel];
//    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
//      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
//    MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0);
//  }
//  return Status;
//}

#endif // ULT_FLAG

