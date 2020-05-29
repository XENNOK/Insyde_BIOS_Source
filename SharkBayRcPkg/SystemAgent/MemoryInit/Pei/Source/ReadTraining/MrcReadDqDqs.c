/** @file
  Once DQS is aligned against the clock in the receive enable training flow,
  the second stage of the read training is the DQ/DQS training, aligning each
  strobe with it's byte of data. The DQ/DQS training is once again using the
  DDR read synchronization mode, in this mode a predetermined pattern is read
  out of the DDR. The following algorithm is used to align the data sampling
  to the best sampling point.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
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

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement.

*/

//
// Include files
//
#include <MrcReadDqDqs.h>

/**
  Perform Read MPR Training.
  Center read DQ-DQS with MPR pattern.

  @param[in, out] MrcData   - Include all MRC global data.

  @retval MrcStatus - mrcSuccess or reason for failure.
**/
MrcStatus
MrcReadMprTraining (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput                              *Inputs;
  const MrcDebug                              *Debug;
  MrcOutput                                   *Outputs;
  MrcChannelOut                               *ChannelOut;
  MrcStatus                                   Status;
  U8                                          Channel;
  U8                                          Rank;
  U8                                          Byte;
  U8                                          ChBitMask;
  U8                                          RankMask;  // RankBitMask for both channels
  U8                                          LoopCount;
  S8                                          MPRCorrectionFactor;
  S8                                          DqsDelay;
  S32                                         CurrentPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                                         CurrentPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                                         LargestPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                                         LargestPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                                         cWidth;
  S32                                         lWidth;
  S32                                         Center;
  S32                                         Center2;
  BOOL                                        Pass;
  BOOL                                        Lpddr;
  U32                                         Offset;
  U32                                         OdtSampExtendDelay;
  MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_STRUCT  ReutChMiscOdtCtrl;
  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT     ReutGlobalCtl;
  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT      DdrCrDataControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT     DdrCrDataControl2;

  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;
  Outputs = &MrcData->SysOut.Outputs;
  Status  = mrcSuccess;
  Lpddr   = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  LoopCount = 10;
  OdtSampExtendDelay  = 1 * HPET_MIN;

  //
  // Use basic addressing mode (open a page on a rank and keep writing/reading to it)
  // Rotate through all 8 logical ranks
  // LFSR and LMN disabled.
  //
  ChBitMask = Outputs->ValidChBitMask;
  RankMask  = Outputs->ValidRankMask;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &Outputs->Controller[0].Channel[Channel];

      //
      // Set DQS Delay to 32
      //
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
            //
            // Update RxDqsP & RxDqsN - leave other parameter the same; can we update in the next loop or do it per channel
            //
            UpdateRxT (MrcData, Channel, Rank, Byte, 5, 32);
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
  //
  SetupIOTestMPR (MrcData, ChBitMask, LoopCount, NSOE, 0, 0);

  //
  /// @todo: Start with 0 for now.
  //
  MPRCorrectionFactor = 0;

  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if (!((MRC_BIT0 << Rank) & RankMask)) {
      continue; // Skip if both channels empty
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Rank = %u\n", Rank);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel\t0                1\nByte\t");
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE, (
      Outputs->SdramCount == (MAX_SDRAM_IN_DIMM)
      ) ? "0 1 2 3 4 5 6 7 8 0 1 2 3 4 5 6 7 8" : "0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7"
      );

    //
    // Program MR3 and Mask RAS/WE to prevent scheduler for issuing non-Read commands
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      SelectReutRanks (MrcData, Channel, MRC_BIT0 << Rank, 0);
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
        if (!Lpddr) {
          Status = MrcWriteMRS (MrcData, Channel, (MRC_BIT0 << Rank), mrMR3, 0x4);
        }

        Offset  = MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG +
          ((MCHBAR_CH1_CR_REUT_CH_MISC_ODT_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG) * Channel);
        ReutChMiscOdtCtrl.Data                  = MrcReadCR (MrcData, Offset);
        ReutChMiscOdtCtrl.Bits.MPR_Train_DDR_On = 1;
        MrcWriteCR (MrcData, Offset, ReutChMiscOdtCtrl.Data);
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nDqsDelay");
    for (DqsDelay = RMPR_DQS_START; DqsDelay < RMPR_DQS_STOP; DqsDelay += RMPR_DQS_STEP) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n% 5d\t", DqsDelay);

      //
      // Program DQS Delays and download the Reg File for the current rank.
      //
      Status = ChangeMargin (MrcData, RdT, DqsDelay, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileStart);

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            (Channel != 0) ? "" : ((Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "                  " : "                ")
            );
        } else {
          ChannelOut = &Outputs->Controller[0].Channel[Channel];

          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            //
            // Force on SenseAmp
            //
            Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
              ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
              ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
            DdrCrDataControl2.Data              = ChannelOut->DqControl2[Byte].Data;
            DdrCrDataControl2.Bits.ForceBiasOn  = 1;
            DdrCrDataControl2.Bits.ForceRxOn    = 1;
            DdrCrDataControl2.Bits.LeakerComp   = 0;
            MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
          }
          //
          // Enable RX Training mode. Turn on the ODT.
          //
          Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
            ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
          DdrCrDataControl0.Data = ChannelOut->DqControl0.Data;
          if (!Lpddr) {
            DdrCrDataControl0.Bits.ForceOdtOn = 1;
          }
          DdrCrDataControl0.Bits.RxTrainingMode = 1;

          //
          // Need to disable EnReadPreamble
          //
          DdrCrDataControl0.Bits.EnReadPreamble   = 0;
          MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
        }
      }

      Status = IoReset (MrcData);


      //
      // Start REUT and run for 1uS
      //
      ReutGlobalCtl.Data                    = 0;
      ReutGlobalCtl.Bits.Global_Start_Test  = 1;
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
      // Update results for all ch/bytes
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          continue;
        }

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
          Pass = (MrcReadCR (MrcData, Offset) == 1);

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, Pass ? ". " : "# ");

          if (DqsDelay == RMPR_DQS_START) {
            if (Pass) {
              CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte] = DqsDelay;
              LargestPassingStart[Channel][Byte] = LargestPassingEnd[Channel][Byte] = DqsDelay;
            } else {
              CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte] = -33;
              LargestPassingStart[Channel][Byte] = LargestPassingEnd[Channel][Byte] = -33;
            }
          } else {
            if (Pass) {
              if (CurrentPassingEnd[Channel][Byte] == (DqsDelay - RMPR_DQS_STEP)) {
                CurrentPassingEnd[Channel][Byte] = DqsDelay;
              } else {
                CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte] = DqsDelay;
              }
              //
              // Update Largest variables
              //
              cWidth  = CurrentPassingEnd[Channel][Byte] - CurrentPassingStart[Channel][Byte];
              lWidth  = LargestPassingEnd[Channel][Byte] - LargestPassingStart[Channel][Byte];
              if (cWidth > lWidth) {
                LargestPassingStart[Channel][Byte]  = CurrentPassingStart[Channel][Byte];
                LargestPassingEnd[Channel][Byte]    = CurrentPassingEnd[Channel][Byte];
              }
            }
          }
        }
      }

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
          continue;
        }
        ChannelOut = &Outputs->Controller[0].Channel[Channel];

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          //
          // Restore orginal value
          //
          Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
            ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
            ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
          MrcWriteCR (MrcData, Offset, ChannelOut->DqControl2[Byte].Data);
        }

        //
        // Clear RxTrainingMode
        //
        Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
          ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);

#ifdef ULT_FLAG
        if (Lpddr) {
          //
          // For LPDDR need to disable OdtSampExtendEn before disabling RxTrainingMode,
          // then re-enable OdtSampExtendEn (from the host struct)
          //
          DdrCrDataControl0.Bits.OdtSampExtendEn = 0;
          MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
          MrcWait (MrcData, OdtSampExtendDelay);

          DdrCrDataControl0.Bits.RxTrainingMode = 0;
          MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
          MrcWait (MrcData, OdtSampExtendDelay);
        }
#endif // ULT_FLAG

        MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0.Data);
      } // for Channel

      Status = IoReset (MrcData);
    } // for DqsDelay

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

    //
    // Clean Up registers.
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
        ChannelOut = &Outputs->Controller[0].Channel[Channel];

        //
        // MPR_TRAIN_DDR_ON bit will force a special command so clear it before MRS command
        //
        Offset = MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG +
          ((MCHBAR_CH1_CR_REUT_CH_MISC_ODT_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG) * Channel);
        ReutChMiscOdtCtrl.Data                  = MrcReadCR (MrcData, Offset);
        ReutChMiscOdtCtrl.Bits.MPR_Train_DDR_On = 0;
        MrcWriteCR (MrcData, Offset, ReutChMiscOdtCtrl.Data);

        if (!Lpddr) {
          Status = MrcWriteMRS (MrcData, Channel, (MRC_BIT0 << Rank), mrMR3, 0x0);
        }
        //
        // Clear up after test
        //
        MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%u.R%u: \tLeft\tRight\tWidth\tCenter\tRxDqsPN\n", Channel, Rank);
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          lWidth = LargestPassingEnd[Channel][Byte] - LargestPassingStart[Channel][Byte];

          //
          // Error Handler if eye not found for all bytes
          //
          if (lWidth == 0) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_ERROR,
              "\nERROR!! NO EYE found for Channel: %u Rank: %u Byte: %u \n",
              Channel,
              Rank,
              Byte
              );
            return mrcReadMPRErr;
          }

          if (lWidth > RMPR_MIN_WIDTH) {
            Center = LargestPassingStart[Channel][Byte] + lWidth / 2;
          } else {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_WARNING,
              "\nWARNING!! lWidth <= %u for Channel: %u Rank: %u Byte: %u \n",
              RMPR_MIN_WIDTH,
              Channel,
              Rank,
              Byte
              );
            Center = 0;
          }
          //
          // Based on previous data, the MPR center is not very good; Adjust it with a magical number
          //
          Center += MPRCorrectionFactor;
          Center2                         = 32 + Center;
          ChannelOut->RxDqsP[Rank][Byte]  = (U8) Center2;
          ChannelOut->RxDqsN[Rank][Byte]  = (U8) Center2;
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "  B%u: \t%d\t%d\t%d\t%d\t%d\n",
            Byte,
            LargestPassingStart[Channel][Byte],
            LargestPassingEnd[Channel][Byte],
            lWidth,
            Center,
            ChannelOut->RxDqsP[Rank][Byte]
            );
          //
          //        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Center = %d, RxDqsPN[%d][%d] = %d\n", Center, Channel, Byte, ChannelOut->RxDqsP[Rank][Byte]);
          //
        } // for Byte
      }
    } // for Channel
  } // for Rank

  //
  // Now program the DQS center values on populated ranks, data is taken from the host struct.
  // Need to do it after all ranks are trained, because we need to keep the same DQS value on all ranks
  // during the training.
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
        }
      }
    }
  }

  //
  // Clean up after Test.  Download the Reg file of the last rank used.
  //
  Status  = ChangeMargin (MrcData, RdT, 0, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileCurrent);
  Status  = IoReset (MrcData);
  return Status;
}

/**
  Peform Read Timing Centering.
  Center Rx DQS-DQ using moderate pattern with 1D eye

  @param[in] MrcData         - Include all MRC global data.

  @retval MrcStatus       - if it succeded returns mrcSuccess
**/
MrcStatus
MrcReadTimingCentering (
  IN     MrcParameters *const MrcData
  )
{
  MrcOutput *Outputs;
  U8        ResetPerBit;
  U8        LoopCount;

  Outputs = &MrcData->SysOut.Outputs;
  ResetPerBit = 1;

  LoopCount   = 10;

  return DQTimeCentering1D (MrcData, Outputs->ValidChBitMask, RdT, ResetPerBit, LoopCount);
}

/**
  Peform Read Timing Centering in 2D.
  Final read timing centering using 2D algorithm and per bit optimization

  @param[in] MrcData         - Include all MRC global data.

  @retval MrcStatus       - if it succeded returns mrcSuccess
**/
MrcStatus
MrcReadTimingCentering2D (
  IN     MrcParameters *const MrcData
  )
{
  const MrcInput  *Inputs;
  const MrcDebug  *Debug;
  MrcOutput       *Outputs;
  MrcStatus       Status;
  U8              EnPerBit;
  U8              EnRxDutyCycle;
  U8              ResetPerBit;
  U8              LoopCount;
  U8              En2D;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
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
            RdT,
            EnPerBit,
            EnRxDutyCycle,
            ResetPerBit,
            LoopCount,
            En2D
            );
  if (mrcSuccess == Status) {
    EnPerBit      = 0;
    EnRxDutyCycle = 1;
    ResetPerBit   = 0;
    En2D          = 1;
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
              RdT,
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
  )
{
  const S8                                TimePoints[] = { 0, -8, 8 };
  const U8                                EHWeights[sizeof (TimePoints)] = { 1, 1, 1 };
  const MrcInput                          *Inputs;
  const MrcDebug                          *Debug;
  MrcOutput                               *Outputs;
  MrcChannelOut                           *ChannelOut;
  MrcStatus                               Status;
  U8                                      ResultType;
  U8                                      Channel;
  U8                                      Byte;
  U8                                      Rank;
  U8                                      bit;
  U8                                      lcloop;
  U8                                      tim;
  U8                                      paramB;
  U8                                      paramT;
  U8                                      BMap[MAX_SDRAM_IN_DIMM];
  S8                                      SumEH;
  S8                                      SumEHSign;
  U8                                      MaxTscale;
  U8                                      SaveLC;
  U16                                     mode;
  S32                                     center[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                                     value0[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U32                                     BERStats[4];
  U32                                     TimScale[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                                     CenterSumByte[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                                     CenterSumBit[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS];
  U32                                     marginbit[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS][MAX_EDGES];
  U32                                     EyeShape[3][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES];
  U32                                     Offset;
  MCHBAR_CH0_CR_REUT_CH_ERR_CTL_STRUCT    ReutChErrCtl;
  DDRDATA0CH0_CR_RXOFFSETVDQ_STRUCT       RxOffsetVdq;

  Inputs    = &MrcData->SysIn.Inputs;
  Debug     = &Inputs->Debug;
  Outputs   = &MrcData->SysOut.Outputs;
  Status    = mrcSuccess;
  SumEH     = 0;
  MaxTscale = 12;
  MrcOemMemorySet ((U8 *) BERStats, 0, sizeof (BERStats));
  for (lcloop = 0; lcloop < (sizeof (BMap) / sizeof (BMap[0])); lcloop++) {
    BMap[lcloop] = lcloop;
  }
  //
  // Assume rank0 is always popuplated
  //
  if (Param == RdV) {
    paramB = RdVBit;
    paramT = RdT;
  } else {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Error Handler: Unknown Margin Parameter\n");
    Status = mrcReadVoltage2DError;
    return Status;
  }
  //
  /// @todo: Need to check if we can enable it for A0 or not
  // Outputs->EnDumRd = 1;
  // SOE = 10b ( Stop on All Byte Groups Error )
  //
  //
  /// @todo: Will enable the DQ tests instead of basic in the future
  //  SetupIOTestDQ (MrcData, ChBitMask, LoopCount, ABGSOE, 0, 0);
  //
  SetupIOTestBasicVA (MrcData, ChBitMask, LoopCount, ABGSOE, 0, 0, 8);
  //
  // Calculate SumEH for use in weighting equations
  //
  for (tim = 0; tim < sizeof (TimePoints); tim++) {
    SumEH += EHWeights[tim];

    //
    // Loop once at nominal Vref point
    //
    if (En2D == 0) {
      tim = sizeof (TimePoints);
    }
  }
  //
  // SumEH is used as divisor, make sure is never 0
  //
  if (SumEH == 0) {
    SumEH = 1;
  }
  //
  // Reset PerBit Deskew to middle value before Byte training
  // Amplifier voltage offset for bit[x] of the DQ Byte.
  // {0: Most negative offset,... 8: 0 offset, ... 15: Most postive offset}
  //
  if (ResetPerBit == 1) {
    //
    // EnMultiCast=1, 0,0,0,0, UpdateHost=1, SkipWait=0
    //
    Status = ChangeMargin (MrcData, paramB, 0x88888888, 0, 1, 0, 0, 0, 0, 1, 0, MrcRegFileStart);
  }
  //
  // Select rank for REUT test
  //
  ChBitMask = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut = &Outputs->Controller[0].Channel[Channel];
    ChBitMask |= SelectReutRanks (MrcData, Channel, ChannelOut->ValidRankBitMask, 0);
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    //
    // Clear any old state in DataTrain Offset
    //
    MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
  }
  //
  // ####################################################
  // ################  Initialize EW/EH variables  ######
  // ####################################################
  //
  Status      = GetMarginByte (MrcData, Outputs->MarginResult, paramT, 0, 0xF);
  ResultType  = GetMarginResultType (paramT);

#ifdef MRC_DEBUG_PRINT
  if (En2D) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n### Measure Eye Height, per BYTE, at ALL (2D) Timing Points\n");
  } else {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n### Measure Eye Height, per BYTE, at NOMINAL Timing\n");
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(ChBitMask & (MRC_BIT0 << Channel))) {
      continue;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %d", Channel);
    if (Channel == 0) {
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "        ");
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nByte    ");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(ChBitMask & (MRC_BIT0 << Channel))) {
      continue;
    }

    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d       ", Byte);
    }
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nTScale\t");
#endif // MRC_DEBUG_PRINT
  //
  // Update TimScale to have the appropriate eye width (read from last saved parameters)
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(ChBitMask & (MRC_BIT0 << Channel))) {
      continue;
    }

    ChannelOut = &Outputs->Controller[0].Channel[Channel];

    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      if (En2D > 0) {
        TimScale[Channel][Byte] =
          (
            MarginByte[ResultType][0][Channel][Byte][0] +
            MarginByte[ResultType][0][Channel][Byte][1]
          ) /
          20;
      } else {
        TimScale[Channel][Byte] = 1;
      }
      //
      // It is possible sumT is 0.
      //
      if (!(TimScale[Channel][Byte]) || (TimScale[Channel][Byte] > MaxTscale)) {
        TimScale[Channel][Byte] = MaxTscale;
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", TimScale[Channel][Byte]);
    }
  }

  Status      = GetMarginByte (MrcData, Outputs->MarginResult, Param, 0, 0xF);
  ResultType  = GetMarginResultType (Param);

  //
  // ####################################################
  // ######   Measure Eye Height at all Timing Points  #####
  // ####################################################
  //
  //
  // Loop through all the Time Points to Test
  //
  for (tim = 0; tim < sizeof (TimePoints); tim++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\nRdTime\t");

    //
    // Setup Timing Offset for this point
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(ChBitMask & (MRC_BIT0 << Channel))) {
        continue;
      }

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        value0[Channel][Byte] = (S32) (TimePoints[tim] * TimScale[Channel][Byte]) / MaxTscale;
        Status = ChangeMargin (MrcData, paramT, value0[Channel][Byte], 0, 0, Channel, 0, Byte, 0, 1, 0, MrcRegFileCurrent);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", value0[Channel][Byte]);
      }
    }
    //
    // Run Margin Test
    //
    mode = 0;
    Status = MrcGetBERMarginByte (
              MrcData,
              Outputs->MarginResult,
              ChBitMask,
              0,
              0xFF,
              Param,
              mode,
              BMap,
              1,
              MAX_POSSIBLE_VREF,
              0,
              BERStats
              );

#ifdef MRC_DEBUG_PRINT
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nLo-Hi\t");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(ChBitMask & (MRC_BIT0 << Channel))) {
        continue;
      }

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d %d\t",
          MarginByte[ResultType][0][Channel][Byte][0],
          MarginByte[ResultType][0][Channel][Byte][1]
          );
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nCenter\t");
#endif // MRC_DEBUG_PRINT
    //
    // Store Results
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (ChBitMask & (MRC_BIT0 << Channel)) {

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          center[Channel][Byte] = (S32)(MarginByte[ResultType][0][Channel][Byte][1] -
                                        MarginByte[ResultType][0][Channel][Byte][0]);
          if (tim == 0) {
            CenterSumByte[Channel][Byte] = 0;
          }
          //
          // Calculate weight for this point
          //
          CenterSumByte[Channel][Byte] += EHWeights[tim] * center[Channel][Byte];

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", center[Channel][Byte] / 20);

          //
          // Record edges for use in per bit margining
          //
          EyeShape[tim][Channel][Byte][0] = MarginByte[ResultType][0][Channel][Byte][0];
          EyeShape[tim][Channel][Byte][1] = MarginByte[ResultType][0][Channel][Byte][1];
        }
      }
    }
    //
    // Loop once at nominal Vref point
    //
    if (En2D == 0) {
      tim = sizeof (TimePoints);
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\nWtdCntr\t");
  //
  // ####################################################
  // ###########   Center Results per Byte   ############
  // ####################################################
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(ChBitMask & (MRC_BIT0 << Channel))) {
      continue;
    }

    ChannelOut = &Outputs->Controller[0].Channel[Channel];

    //
    // Calculate CenterPoint.  Round to Nearest Int
    //
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      SumEHSign                     = (CenterSumByte[Channel][Byte] < 0) ? (-1) : 1;

      CenterSumByte[Channel][Byte]  = (CenterSumByte[Channel][Byte] + 10 * (SumEHSign * SumEH)) / (20 * SumEH);
      MRC_DEBUG_MSG(Debug, MSG_LEVEL_NOTE,"%d\t", CenterSumByte[Channel][Byte] / 2);

      //
      // Apply new centerpoint
      // step size for RxVref is about 7.8mv AND for RxVrefOffset is about 3.9mv
      //
      ChannelOut->RxVref[Byte] = (U8) ((S32) ChannelOut->RxVref[Byte] + (CenterSumByte[Channel][Byte] / 2));

      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
        }
      }
      //
      // Update the Eye Edges
      //
      for (tim = 0; tim < sizeof (TimePoints); tim++) {
        EyeShape[tim][Channel][Byte][0] = (S32) EyeShape[tim][Channel][Byte][0] + (10 * CenterSumByte[Channel][Byte]);
        EyeShape[tim][Channel][Byte][1] = (S32) EyeShape[tim][Channel][Byte][1] - (10 * CenterSumByte[Channel][Byte]);

        //
        // Loop once at nominal Vref point
        //
        if (En2D == 0) {
          tim = sizeof (TimePoints);
        }
      }
      //
      // Update MrcData for future tests (MarginResult points back to MrcData)
      // EyeShape for Vref 0 is assumed to have the best shape for future tests.
      //
      MarginByte[ResultType][0][Channel][Byte][0] = EyeShape[0][Channel][Byte][0];
      MarginByte[ResultType][0][Channel][Byte][1] = EyeShape[0][Channel][Byte][1];
    }

    //
    // Clear up after test
    //
    MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);

    //
    // Propagate new settings from the RegFile to the Pads
    //
    MrcDownloadRegFile (MrcData, Channel, 1, 0, MrcRegFileCurrent, 0, 1, 0);
  }

#ifdef MRC_DEBUG_PRINT
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRdVref\t");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(ChBitMask & (MRC_BIT0 << Channel))) {
      continue;
    }
    ChannelOut = &Outputs->Controller[0].Channel[Channel];
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", (S8) ChannelOut->RxVref[Byte]);
    }
  }
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nPerByte Margins after per BYTE Centering\nLo-Hi   ");
  //
#endif // MRC_DEBUG_PRINT
  //
  // ####################################################
  // ############    Measure Eye Height Per BIT   ########
  // ####################################################
  //
  if (EnPerBit) {

    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE, (
      En2D
      ) ? "\n### Measure Eye Height, per BIT, at ALL (2D) Timing Points\n" :
      "\n### Measure Eye Height, per BIT, at NOMINAL Timing\n"
      );

    //
    // Stop on all lane fail
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (ChBitMask & (MRC_BIT0 << Channel))
      {
#ifdef MRC_DEBUG_PRINT
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel      %d", Channel);
        if (Channel == 0) {
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t");
          }
        }
#endif // MRC_DEBUG_PRINT
        //
        // SOE = 11b ( Stop on All Lanes Error )
        //
        Offset = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG + ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * Channel);
        ReutChErrCtl.Data = 0;

        ReutChErrCtl.Bits.Selective_Error_Enable_Chunk = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Chunk_MAX;
        ReutChErrCtl.Bits.Selective_Error_Enable_Cacheline = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Cacheline_MAX;
        ReutChErrCtl.Bits.Stop_On_Error_Control = ALSOE;
        ReutChErrCtl.Bits.Stop_on_Nth_Error     = 1;
        MrcWriteCR (MrcData, Offset, ReutChErrCtl.Data);
      }
    }

#ifdef MRC_DEBUG_PRINT
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (ChBitMask & (MRC_BIT0 << Channel)) {
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "             Byte % 24d ", Byte);
        }
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
#endif // MRC_DEBUG_PRINT
    //
    // Loop through all the Vref Points to Test
    //
    SaveLC = Outputs->DQPatLC;
    for (tim = 0; tim < sizeof (TimePoints); tim++) {
      //
      // Setup Timing Offset for this point
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {

          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            value0[Channel][Byte] = (S32) (TimePoints[tim] * TimScale[Channel][Byte]) / MaxTscale;
            Status = ChangeMargin (MrcData, paramT, value0[Channel][Byte], 0, 0, Channel, 0, Byte, 0, 1, 0, MrcRegFileStart);

            //
            // Amplifier voltage offset for bit[x] of the DQ Byte.
            // {0: Most negative offset,... 8: 0 offset, ... 15: Most postive offset}
            //
            for (bit = 0; bit < MAX_BITS; bit++) {
              marginbit[Channel][Byte][bit][0] = marginbit[Channel][Byte][bit][1] = 8;
            }
          }
        }
      }
      //
      // Run Margin Test
      // Loop through 2 times.  Once at low loop count and Once at high loopcount
      // Improves runtime
      // @todo: Need 2 loops below if not using BASICVA
      //
      for (lcloop = 0; lcloop < 1; lcloop++) {
        Outputs->DQPatLC  = (lcloop == 0) ? 1 : SaveLC;
        mode              = 0;
        Status            = MrcGetMarginBit (MrcData, ChBitMask, 0, marginbit, EyeShape[tim], paramB, mode, 15);
      }
      //
      // Store Results
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {

          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            for (bit = 0; bit < MAX_BITS; bit++) {
              center[Channel][Byte] = ((marginbit[Channel][Byte][bit][1] - 8) - (8 - marginbit[Channel][Byte][bit][0]));
              if (tim == 0) {
                CenterSumBit[Channel][Byte][bit] = 0;
              }
              //
              // Calculate weight for this point
              //
              CenterSumBit[Channel][Byte][bit] += EHWeights[tim] * center[Channel][Byte];
            }
          }
        }
      }
      //
      // Loop once at nominal Vref point
      //
      if (En2D == 0) {
        tim = sizeof (TimePoints);
      }
    } // END OF TIM LOOP
    //
    // ####################################################
    // ############   Center Result Per BIT  ##############
    // ####################################################
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nWgted Center ");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(ChBitMask & (MRC_BIT0 << Channel))) {
        continue;
      }

      ChannelOut = &Outputs->Controller[0].Channel[Channel];

      //
      // Calculate and apply CenterPoint.  Round to Nearest Int
      //
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        RxOffsetVdq.Data = 0;

        for (bit = 0; bit < MAX_BITS; bit++) {
          SumEHSign                         = (CenterSumBit[Channel][Byte][bit] < 0) ? (-1) : 1;

          CenterSumBit[Channel][Byte][bit]  = (CenterSumBit[Channel][Byte][bit] + (SumEHSign * SumEH)) / (2 * SumEH);

          //
          // Centerpoint needs to be added to starting DqPb value
          //
          CenterSumBit[Channel][Byte][bit] += (S32) ChannelOut->RxDqVrefPb[0][Byte][bit].Center;

          //
          // Check for saturation
          //
          if (CenterSumBit[Channel][Byte][bit] > 15) {
            CenterSumBit[Channel][Byte][bit] = 15;
          } else if (CenterSumBit[Channel][Byte][bit] < 0) {
            CenterSumBit[Channel][Byte][bit] = 0;
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%x   ", CenterSumBit[Channel][Byte][bit]);

          //
          // Update MrcData
          //
          ChannelOut->RxDqVrefPb[0][Byte][bit].Center = (U8) CenterSumBit[Channel][Byte][bit];

          RxOffsetVdq.Data |= (CenterSumBit[Channel][Byte][bit] << (DDRDATA0CH0_CR_RXOFFSETVDQ_Lane0_WID * bit));
        }
        //
        // Apply and propagate new centerpoint
        //
        Status = ChangeMargin (MrcData, RdVBit, RxOffsetVdq.Data, 0, 0, Channel, 0, Byte, 0, 0, 0, MrcRegFileCurrent);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "           ");
      }
      //
      // Clear up after test
      //
      MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
    }
  } // END PERBIT LOOP

/// @attention - This is used to determine if the PerBit routines are correct.  Left for sanity.
/*
#ifdef MRC_DEBUG_PRINT
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (ChBitMask & (MRC_BIT0 << Channel)) {

        ChannelOut = &Outputs->Channel[Channel];

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          Offset = DDRDATA0CH0_CR_RXOFFSETVDQ_REG +
            ((DDRDATA0CH1_CR_RXOFFSETVDQ_REG - DDRDATA0CH0_CR_RXOFFSETVDQ_REG) * Channel) +
            ((DDRDATA1CH0_CR_RXOFFSETVDQ_REG - DDRDATA0CH0_CR_RXOFFSETVDQ_REG) * Byte);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRdVref = % 3d   RdVBit = 0x%08X", (S8) ChannelOut->RxVref[Byte],
                         MrcReadCR (MrcData, Offset));
        }
      }
    }
#endif // MRC_DEBUG_PRINT
*/
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  //
  // Clean up after test
  //
  Outputs->EnDumRd  = 0;
  Status            = ChangeMargin (MrcData, paramT, 0, 0, 1, 0, 0, 0, 0, 1, 0, MrcRegFileCurrent);

  return Status;
}

/**

@brief

  Peform Read Voltage Centering in 2D.
  Note: This function currently only supports param = RdV

  @param[in] MrcData       - Include all MRC global data.

  @retval MrcStatus       - if it succeded return mrcSuccess

**/
MrcStatus
MrcReadVoltageCentering2D (
  IN     MrcParameters *const MrcData
  )
{
  MrcOutput *Outputs;
  MrcStatus Status;
  U8        EnPerBit;
  U8        ResetPerBit;
  U8        LoopCount;
  U8        En2D;

  Outputs   = &MrcData->SysOut.Outputs;

  EnPerBit    = 1;
  ResetPerBit = 1;
  LoopCount   = 15;
  En2D        = 0;
  Status = ReadVoltageCentering2D (
            MrcData,
            Outputs->MarginResult,
            Outputs->ValidChBitMask,
            RdV,
            EnPerBit,
            ResetPerBit,
            LoopCount,
            En2D
            );

  if (mrcSuccess == Status) {
    //
    // EnPerBit = 0; ResetPerbit = 0; loopcount = 10; En2D=1
    //
    EnPerBit    = 0;
    ResetPerBit = 0;
    LoopCount   = 15;
    En2D        = 1;
    Status = ReadVoltageCentering2D (
              MrcData,
              Outputs->MarginResult,
              Outputs->ValidChBitMask,
              RdV,
              EnPerBit,
              ResetPerBit,
              LoopCount,
              En2D
              );
  }

  return Status;
}
