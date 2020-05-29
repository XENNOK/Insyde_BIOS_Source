/** @file
  These functions implement the crosser training algorithm.

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

**/

//
// Include files
//
#include <Library/BaseMemoryLib.h>
#include <MrcCrosser.h>
#if SUPPORT_SODIMM == SUPPORT
#include <MrcSpdProcessing.h>
#endif //SUPPORT_SODIMM == SUPPORT

///
/// Module Definitions
///
#define BIT_TX_XTALK_SWEEP_START  (-3)
#define BIT_TX_XTALK_SWEEP_STOP   (3)
#define BIT_TX_XTALK_SWEEP_LEN    (BIT_TX_XTALK_SWEEP_STOP - BIT_TX_XTALK_SWEEP_START + 1)
#define BIT_TX_XTALK_RANGE        (16)

#define DIMM_ODT_DIMM_MASK_SHIFT  (4)

///
/// Power optimizations Global Parameters
///
#define OPT_PARAM_LOOP_COUNT (15)
#define OPT_PARAM_1D_LC      (15)

///
/// UPM/PWR increment value if margins are at or below the retrain limit.
///
#define MRC_UPM_PWR_INC_VAL (40)

//
// Module Globals
//
const MrcUpmPwrRetrainLimits  InitialLimits[MRC_NUMBER_UPM_PWR_RETRAIN_MARGINS] = {
  //
  //           UPM,     PWR      Retrain
  //
  {RdT,       {240,     320,     90 }},
  {WrT,       {220,     280,     90 }},
  {RdV,       {400,     520,     160}},
  // For ULT DDR3L rcF the values are increased by 20 ticks, see UpmPwrLimitValue()
  {WrV,       {400,     520,     160}},
  {RdFan2,    {240,     480,     0  }},
  {WrFan2,    {240,     480,     0  }},
  {RdFan3,    {240*4/5, 480*4/5, 0  }},
  {WrFan3,    {240*4/5, 480*4/5, 0  }},
  // {650ps,750ps} * 64 pi ticks * 2 (for width) = 134 PI Ticks.  ~1.3nsec for UPM,~1.5nsec for PWR
  // We must subtract out the built in margin of 96 when shifting IO Lat.
  // Margin function works in steps of 2, so we divide the margin by 2.
  // Margin numbers are scaled by 10.
  {RcvEnaX,  {(((134 - 96) / 2) * 10), (((154 - 96) / 2) * 10), 0}}
};

const U8    ActualDimmOdt[6] = { 0, 120, 60, 40, 30, 20 };

#ifdef MRC_DEBUG_PRINT
const char  *TOptParamOffsetString[] = {
  "OptWrDS",
  "OptRdOdt",
  "OptSComp",
  "OptTComp",
  "OptTxEq",
  "OptRxEq",
  "OptRxBias",
  "OptDimmOdt",
  "OptDimmOdtWr",
  "OptDimmRon",
  "OptDefault"
};

const char  *MarginTypesString[] = {
  "RcvEna",
  "RdT",
  "WrT",
  "WrDqsT",
  "RdV",
  "WrV",
  "WrLevel",
  "WrTBox",
  "WrTBit",
  "RdTBit",
  "RdVBit",
  "RcvEnaX",
  "CmdT",
  "CmdV"
};

///
/// These strings match the OptResultPerByteDbgStr enum for indexing
/// the switch PrintCalcResultTableCh and PrintODTResultTable.
///
const char *OptResultDbgStrings[] = {
  "Best",
  "GrdBnd",
  "OffSel",
  "Scale",
  "Signal",
  "Noise",
  "Ratio",
  "MaxPost",
  "MinPost",
  "Ticks",
  "SNRTot."
};

#endif // MRC_DEBUG_PRINT

/**
  This function implements Sense Amp Offset training.
  SenseAmp/ODT offset cancellation
  Find the best "average" point for Vref Control
  Test Vref point with SampOffset=-7 and Test Vref Point with SampOffset=+7
  Find Vref on per ch/byte basis where -7 samples all 1 and +7 samples all 0

  @param[in,out] MrcData - Include all MRC global data.

  @retval MrcStatus - if it succeded return mrcSuccess
**/
MrcStatus
MrcSenseAmpOffsetTraining (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput                          *Inputs;
  const MrcDebug                          *Debug;
  MrcOutput                               *Outputs;
  MrcControllerOut                        *ControllerOut;
  MrcChannelOut                           *ChannelOut;
  MrcStatus                               Status;
  U32                                     Offset;
  S8                                      sumBits[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S8                                      FirstBestPoint[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S8                                      LastBestPoint[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  //
  // additional bit for DQS per each byte
  //
  S8                                      firstZero[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS_FOR_OFFSET_TRAINING];
  //
  // additional bit for DQS per each byte
  //
  S8                                      lastOne[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS_FOR_OFFSET_TRAINING];
  S8                                      sampOffset;
  S8                                      vref;
  S8                                      VrefWidth;
  U8                                      HighMask[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U8                                      LowMask[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U8                                      Channel;
  U8                                      Rank;
  U8                                      byte;
  U8                                      bit;
  U8                                      MaxBits;
  BOOL                                    Lpddr;
  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT  DdrCrDataControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT DdrCrDataControl2;
  DDRDATA0CH0_CR_DATATRAINFEEDBACK_STRUCT DataTrainFeedback;
  DDRDATA_CR_RXOFFSETVDQ_STRUCT           RxOffsetVdq;

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Status        = mrcSuccess;
  Lpddr         = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  //
  // Init LastBestPoint to 0, FirstBestPoint to -8, LowMask to 0xff and HighMask to 0
  //
  MrcOemMemorySet ((U8 *) LastBestPoint, 0, sizeof (LastBestPoint));
  MrcOemMemorySet ((U8 *) FirstBestPoint, (U32) (-8), sizeof (FirstBestPoint));
  MrcOemMemorySet ((U8 *) LowMask, (U32) (-1), sizeof (LowMask));
  MrcOemMemorySet ((U8 *) HighMask, 0, sizeof (LowMask));

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Stage 1: Vref Offset Training\nPlot Of SumOfBits across Vref settings\nChannel\t0                1\nByte\t"
    );
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    (Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "0 1 2 3 4 5 6 7 8 0 1 2 3 4 5 6 7 8" : "0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7"
    );

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];
      //
      // Force RXAmp and Bias on -MUST use Per byte as preious DqControl2 values depended on byte number
      //
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        DdrCrDataControl2.Data              = ChannelOut->DqControl2[byte].Data;
        DdrCrDataControl2.Bits.ForceBiasOn  = 1;
        DdrCrDataControl2.Bits.ForceRxOn    = 1;
        Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
          ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
            ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * byte);
        MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
      }
    }
  }
  //
  // Sweep through vref settings and find point SampOffset of +/- 7 passes
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n1/2 Vref");
  for (vref = -8; vref <= 8; vref++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n% 5d\t", vref);

    //
    // Run Test and Record Results.
    //
    Status = ChangeMargin (MrcData, RdV, vref, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileRank);

    //
    // Program settings for Vref and SampOffset = 7
    //
    MrcWriteCrMulticast (MrcData, DDRDATA_CR_RXOFFSETVDQ_REG, 0xFFFFFFFF); // (8+7)
    //
    // To run test, enable Offset Cancel mode and Enable ODT
    // Check Results and Update variables.  Ideal result is all 0
    // Clear Offset Cancel mode at end test to enable writing RX_OffsetV
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        ChannelOut = &ControllerOut->Channel[Channel];

        //
        // Propagate delay values (without a read command)
        //
        DdrCrDataControl0.Data                      = ChannelOut->DqControl0.Data;
        DdrCrDataControl0.Bits.ReadRFRd             = 1;
        DdrCrDataControl0.Bits.ReadRFWr             = 0;
        DdrCrDataControl0.Bits.ReadRFRank           = 0;
        DdrCrDataControl0.Bits.ForceOdtOn           = 1;
        DdrCrDataControl0.Bits.SenseampTrainingMode = 1;
        Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
          ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
        MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);

        MrcWait (MrcData, HPET_1US);

        for (byte = 0; byte < Outputs->SdramCount; byte++) {
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * byte);
          DataTrainFeedback.Data  = (U8) MrcReadCR (MrcData, Offset);
          sumBits[Channel][byte]  = -(MrcCountBitsEqOne (DataTrainFeedback.Bits.DataTrainFeedback));
          LowMask[Channel][byte] &= (U8) DataTrainFeedback.Bits.DataTrainFeedback;
        }

        DdrCrDataControl0.Bits.SenseampTrainingMode = 0;
        Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
          ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
        MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
      }
    }
    //
    // Program settings for SampOffset = -7
    //
    MrcWriteCrMulticast (MrcData, DDRDATA_CR_RXOFFSETVDQ_REG, 0x11111111); // (8-7)
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcChannelExist (Outputs, Channel))) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          (Channel != 0) ? "" : ((Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "                  " : "                ")
          );
      } else {
        ChannelOut = &ControllerOut->Channel[Channel];

        //
        // Propagate delay values (without a read command)
        //
        DdrCrDataControl0.Data                      = ChannelOut->DqControl0.Data;
        DdrCrDataControl0.Bits.ReadRFRd             = 1;
        DdrCrDataControl0.Bits.ReadRFWr             = 0;
        DdrCrDataControl0.Bits.ReadRFRank           = 0;
        DdrCrDataControl0.Bits.ForceOdtOn           = 1;
        DdrCrDataControl0.Bits.SenseampTrainingMode = 1;
        Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
          ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
        MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);

        MrcWait (MrcData, HPET_1US);
        for (byte = 0; byte < Outputs->SdramCount; byte++) {
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * byte);
          DataTrainFeedback.Data = (U8) MrcReadCR (MrcData, Offset);
          sumBits[Channel][byte] += MrcCountBitsEqOne (DataTrainFeedback.Bits.DataTrainFeedback);
          HighMask[Channel][byte] |= DataTrainFeedback.Bits.DataTrainFeedback;

          //
          // Check if this point is better
          //
          if (sumBits[Channel][byte] > FirstBestPoint[Channel][byte]) {
            FirstBestPoint[Channel][byte] = sumBits[Channel][byte];
            LastBestPoint[Channel][byte]  = vref;
            ChannelOut->RxVref[byte]      = vref;
          } else if (sumBits[Channel][byte] == FirstBestPoint[Channel][byte]) {
            LastBestPoint[Channel][byte] = vref;
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d ", sumBits[Channel][byte]);
        }

        DdrCrDataControl0.Bits.SenseampTrainingMode = 0;
        Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
          ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
        MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
      }
    }
  }

#ifdef MRC_DEBUG_PRINT
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "\nHi-Lo\t");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "0x%x 0x%x ", HighMask[Channel][byte], LowMask[Channel][byte]);
        //
        // Exit with error if any bit did not change
        //
        if ((HighMask[Channel][byte] ^ LowMask[Channel][byte]) != 0xFF) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_ERROR,
            "\nERROR! At least one bit with unexpected results for Channel %u Byte %u\n",
            Channel,
            byte
            );
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_ERROR,
            "A '0' in the following BitMask value represents the failing Bit(s) 0x%x\n",
            (HighMask[Channel][byte] ^ LowMask[Channel][byte])
            );
          return mrcSenseAmpErr;
        }
      }
    }
  }

  //
  // Display the selected Read Vref per byte
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRdVref\t");
#endif // MRC_DEBUG_PRINT
  //
  // Clear RdV offset
  //
  Status = ChangeMargin (MrcData, RdV, 0, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileRank);

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];
      //
      // Upate RxVref delay center
      //
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        VrefWidth = (S8) (LastBestPoint[Channel][byte] - ChannelOut->RxVref[byte]);
        vref      = (S8) (ChannelOut->RxVref[byte] + (VrefWidth / 2));

        //
        // Add 1 to Round Up and find the center
        //
        if (vref < 0) {
          vref--;
        } else {
          vref++;
        }
        //
        // step size for RxVref is about 7.8mv AND for RxVrefOffset is about 3.9mv
        //
        ChannelOut->RxVref[byte] = vref / 2;

        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
            UpdateRxT (MrcData, Channel, Rank, byte, 0xFF, 0);
          }
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d ", (S8) ChannelOut->RxVref[byte]);
      }
    }
  }
  //
  // init firstZero and lastOne to 0
  //
  MrcOemMemorySet ((U8 *) firstZero, 0, sizeof (firstZero));
  MrcOemMemorySet ((U8 *) lastOne, 0, sizeof (lastOne));

  MaxBits = MAX_BITS_FOR_OFFSET_TRAINING - 1;
#ifdef ULT_FLAG
  if (Lpddr) {
    MaxBits++;  // for ULT offset training done for 8 bits + DQS bit
  }
#endif //ULT_FLAG

#ifdef MRC_DEBUG_PRINT
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\nStage 2: SampOffset Training\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel 0\t\t\t\t\t\t\t\t\t %s1\nByte    ", Lpddr ? "\t" : "");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    for (byte = 0; byte < Outputs->SdramCount; byte++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d        %s", byte, Lpddr ? " " : "");
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nBits    ");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    for (byte = 0; byte < Outputs->SdramCount; byte++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "01234567%s ", Lpddr ? "S" : "");
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n SAmp");
#endif // MRC_DEBUG_PRINT

  for (sampOffset = 1; sampOffset <= 15; sampOffset++) {
    //
    // Display per Byte Feedback from REUT Registers
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n% 5d\t", sampOffset);

    //
    // Program Offset and Propagate new value from RF
    //
    RxOffsetVdq.Data = 0;
    for (bit = 0; bit < MAX_BITS; bit++) {
      RxOffsetVdq.Data += (sampOffset & DDRDATA_CR_RXOFFSETVDQ_Lane0_MSK) << (DDRDATA_CR_RXOFFSETVDQ_Lane0_WID * bit);
    }

    MrcWriteCrMulticast (MrcData, DDRDATA_CR_RXOFFSETVDQ_REG, RxOffsetVdq.Data);

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcChannelExist (Outputs, Channel)) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t\t%s", Lpddr ? "\t" : "");
        continue;
      }

      ChannelOut = &ControllerOut->Channel[Channel];
#ifdef ULT_FLAG
      if (Lpddr) {
        //
        // write DQS offset to control2 reg sampOffset
        //
        for (byte = 0; byte < Outputs->SdramCount; byte++) {
          DdrCrDataControl2.Data                = ChannelOut->DqControl2[byte].Data;

          DdrCrDataControl2.Bits.RxDqsAmpOffset = sampOffset;
          DdrCrDataControl2.Bits.ForceBiasOn    = 1;
          DdrCrDataControl2.Bits.ForceRxOn      = 1;
          DdrCrDataControl2.Bits.LeakerComp     = 0;

          Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
            ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
              ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * byte);
          MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
        }
      }
#endif //ULT_FLAG
      //
      // Propagate delay values (without a read command)
      //
      DdrCrDataControl0.Data                      = ChannelOut->DqControl0.Data;
      DdrCrDataControl0.Bits.ReadRFRd             = 1;
      DdrCrDataControl0.Bits.ReadRFWr             = 0;
      DdrCrDataControl0.Bits.ReadRFRank           = 0;
      DdrCrDataControl0.Bits.ForceOdtOn           = 1;
      DdrCrDataControl0.Bits.SenseampTrainingMode = 1;
      Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
        ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);

      MrcWait (MrcData, HPET_1US);

      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
          ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
          ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * byte);
        DataTrainFeedback.Data = MrcReadCR (MrcData, Offset);
        //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DataTrainFeedback = 0x%x, sampOffset = %d\n", DataTrainFeedback.Data, sampOffset);

        for (bit = 0; bit < MaxBits; bit++) {
          if (DataTrainFeedback.Bits.DataTrainFeedback & (MRC_BIT0 << bit)) {
            lastOne[Channel][byte][bit] = sampOffset;
          } else {
            if (firstZero[Channel][byte][bit] == 0) {
              firstZero[Channel][byte][bit] = sampOffset;
            }
          }
          //
          // Display in bits
          //
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            ((MRC_BIT0 << bit) & DataTrainFeedback.Bits.DataTrainFeedback) ? "1" : "0"
            );
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " ");
      }

      DdrCrDataControl0.Bits.SenseampTrainingMode = 0;
      Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
        ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
    } // for Channel
  } // for sampOffset

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nBitSAmp ");

  //
  // Calculate and Program Offsets and display per bit SenseAmp Offset
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        RxOffsetVdq.Data = 0;
        for (bit = 0; bit < MaxBits; bit++) {
          //
          // Find vref center, add 1 for Round Up
          //
          vref = (firstZero[Channel][byte][bit] + lastOne[Channel][byte][bit]) / 2;

          //
          // Check for saturation conditions
          // to make sure we are as close as possible to vdd/2 (750mv)
          //
          if (firstZero[Channel][byte][bit] == 0) {
            vref = 15;
          }

          if (lastOne[Channel][byte][bit] == 0) {
            vref = 0;
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%X", vref);
#ifdef ULT_FLAG
          if (Lpddr) {
            if (bit == 8) {
              //
              // Write the DQS sense amp offset value to the host struct.
              // It will be written to the HW at the end of this routine.
              //
              // Add 8 to the center value, to better suppress DQS reflections before the read preamble.
              //
              ChannelOut->DqControl2[byte].Bits.RxDqsAmpOffset = MIN (vref + 8, DDRDATA_CR_DDRCRDATACONTROL2_RxDqsAmpOffset_MAX);
              break;
            }
          }
#endif   // ULT_FLAG

          RxOffsetVdq.Data += (vref & DDRDATA0CH0_CR_RXOFFSETVDQ_Lane0_MSK) << (DDRDATA0CH0_CR_RXOFFSETVDQ_Lane0_WID * bit);
          ChannelOut->RxDqVrefPb[0][byte][bit].Center = vref;
        }

        Offset = DDRDATA0CH0_CR_RXOFFSETVDQ_REG +
          ((DDRDATA0CH1_CR_RXOFFSETVDQ_REG - DDRDATA0CH0_CR_RXOFFSETVDQ_REG) * Channel) +
          ((DDRDATA1CH0_CR_RXOFFSETVDQ_REG - DDRDATA0CH0_CR_RXOFFSETVDQ_REG) * byte);
        MrcWriteCR (MrcData, Offset, RxOffsetVdq.Data);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " ");

        //
        // Propagate delay values (without a read command)
        //
        MrcDownloadRegFile (MrcData, Channel, 1, 0, MrcRegFileRank, 0, 1, 0);
      }
    }
#ifdef MRC_DEBUG_PRINT
    else {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t\t%s", Lpddr ? "\t" : ""); // Line up Channel 1
    }
#endif
  }
  //
  // Clean up after test
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];

      //
      // Restore DataControl2
      //
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
          ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
          ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * byte);
        MrcWriteCR (MrcData, Offset, ChannelOut->DqControl2[byte].Data);
      }
      //
      // Restore DataControl0
      //
      Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
        ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0.Data);
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

  Status = IoReset (MrcData);

  return Status;
}

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
  )
{
  MrcDebug const          *Debug;
  MrcOutput               *Outputs;
  MrcControllerOut        *ControllerOut;
  MrcUpmPwrRetrainLimits  *UpmPwrRetrainLimits;
  MRC_MarginTypes         MarginParam;
  MrcMarginResult         LastResultParam;
  MrcStatus               Status;
  MRC_MARGIN_LIMIT_TYPE   MarginLimitType;
  U32                     (*LastMargins)[MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES];
  U32                     BERStats[4];
  U16                     MinEdgeMargin[MAX_EDGES];
  U16                     RetrainMarginLimit;
  U16                     CurrentMargin;
  U8                      Channel;
  U8                      ChannelMask;
  U8                      Rank;
  U8                      RankMask;
  U8                      Edge;
  U8                      Loopcount;
  U8                      MaxMargin;
  BOOL                    RdWrMarginFail[2];

  Debug               = &MrcData->SysIn.Inputs.Debug;
  Outputs             = &MrcData->SysOut.Outputs;
  ControllerOut       = &Outputs->Controller[0];
  LastMargins         = Outputs->MarginResult;
  UpmPwrRetrainLimits = Outputs->UpmPwrRetrainLimits.Pointer;
  Status              = mrcSuccess;
  Loopcount           = 17;
  MrcOemMemorySet ((U8 *) BERStats, 0, sizeof (BERStats));
  RdWrMarginFail[0] = FALSE;
  RdWrMarginFail[1] = FALSE;


  //
  // Loop is dependent on the order of MRC_MarginTypes.  If this changes, pleas ensure functionality
  // stays the same.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Loopcount: %d\n", Loopcount);
  SetupIOTestBasicVA (MrcData, Outputs->ValidChBitMask, Loopcount, NSOE, 0, 0, 8);
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    RankMask    = 1 << Rank;
    ChannelMask = 0;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelMask |= SelectReutRanks (MrcData, Channel, RankMask, 0);
      if ((1 << Channel) & ChannelMask) {
        MrcOemMemorySetDword (&ControllerOut->Channel[Channel].DataOffsetTrain[0], 0, Outputs->SdramCount);
      }
    }

    if (ChannelMask == 0) {
      continue;
    }

    for (MarginParam = RdT; MarginParam <= WrV; MarginParam++) {
      if (MarginParam == WrDqsT) {
        continue;
      }

      if (MarginParam == RdT) {
        Outputs->DQPat = RdRdTA;
      } else if (MarginParam == RdV) {
        Outputs->DQPat = BasicVA;
      }

      MaxMargin = ((MarginParam == RdV) || (MarginParam == WrV)) ? MAX_POSSIBLE_VREF : MAX_POSSIBLE_TIME;

      Status = MrcGetBERMarginCh (
        MrcData,
        LastMargins,
        ChannelMask,
        0xFF,
        Rank,
        MarginParam,
        0,
        1,
        MaxMargin,
        0,
        BERStats
        );
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Margins\nParams: RdT\tWrT\tRdV\tWrV\n\tLft Rgt Lft Rgt Low Hi  Low Hi");
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
        continue;
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nC%dR%d\t", Channel, Rank);
      for (MarginParam = RdT; MarginParam <= WrV; MarginParam++) {
        if (MarginParam == WrDqsT) {
          continue;
        }

        LastResultParam     = GetMarginResultType (MarginParam);
        RetrainMarginLimit  = UpmPwrLimitValue (MrcData, MarginParam, RetrainLimit) / 10;
        MrcOemMemorySetWord (MinEdgeMargin, (U16) (~0), MAX_EDGES);


        for (Edge = 0; Edge < MAX_EDGES; Edge++) {
          CurrentMargin       = (U16) LastMargins[LastResultParam][Rank][Channel][0][Edge] / 10;
          MinEdgeMargin[Edge] = MIN (MinEdgeMargin[Edge], CurrentMargin);
          if ((CurrentMargin <= RetrainMarginLimit)) {
            Status =  mrcRetrain;
            if ((MarginParam == RdT) || (MarginParam == RdV)) {
              RdWrMarginFail[0] = TRUE;
            } else if ((MarginParam == WrT) || (MarginParam == WrV)) {
              RdWrMarginFail[1] = TRUE;
            }
          }
        }
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%2d  %2d\t", MinEdgeMargin[0], MinEdgeMargin[1]);
        if ((RdWrMarginFail[0] == TRUE) && (RdWrMarginFail[1] == TRUE)) {
          Rank    = MAX_RANK_IN_CHANNEL;
          Channel = MAX_CHANNEL;
          break;
        }
      }
    }
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");  // End of table

  if (Status == mrcRetrain) {
    //
    // Loop is dependent on the order of MRC_MarginTypes.  If this changes, please ensure functionality
    // stays the same.
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*** Margin Limit Check Failed! ***\nNew Limits:\nParam\tUPM\tPWR");
    for (MarginParam = RdT; MarginParam <= WrV; MarginParam++) {
      if (((RdWrMarginFail[0] == FALSE) && ((MarginParam == RdT) || (MarginParam == RdV))) ||
          ((RdWrMarginFail[1] == FALSE) && ((MarginParam == WrT) || (MarginParam == WrV))) ||
          (MarginParam == WrDqsT)) {
        continue;
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n%s", MarginTypesString[MarginParam]);
      for (MarginLimitType = UpmLimit; MarginLimitType < RetrainLimit; MarginLimitType++) {
        RetrainMarginLimit = MrcUpdateUpmPwrLimits (MrcData, MarginParam, MarginLimitType, MRC_UPM_PWR_INC_VAL);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t%d", RetrainMarginLimit);
      }
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");  // End of table.
  }

  return Status;
}


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
MrcStatus
MrcDimmODTTraining (
  IN MrcParameters *const MrcData
  )
{
  const MrcDebug    *Debug;
  const MrcInput    *Inputs;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcStatus         Status;
  U8                Channel;
  U8                byte;
  U32               NumBytes;
  U8                NumCh;
  U8                RankMask;
  U8                LocalRanks[MAX_CHANNEL];
  U8                ChMask;
  U8                RttNomPoints;
  U8                RdOdtPoints;
  S8                GRdOdt;
  U8                RttWr0;
  U8                Dimm;
  U8                RttWr[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8                RttNom[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  S8                RttNom0;
  S8                RttNom1;
  U8                BestRttNom[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  U8                BestRttWr[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL];
  S8                BestRdOdt[MAX_CHANNEL];
  U8                offset;
  U8                test;
  U8                *TestList;
  U8                TestListSize;
  S32               RdOdtCodes[2]; // Store Comp Codes associated with each RdOdt
  S8                RttNom1Off;
  BOOL              any2DPC;
  BOOL              any1DPC;
  BOOL              Lpddr;
  S8                BestGRdOdt;
  U8                RttOffset;
  U8                OffsetPoints;
  U8                loopcount; // for centering
  S8                GRdOdtStep;
  BOOL              IncEnds;
  BOOL              SubPwrLimits;
  BOOL              skipSubOpt;
  BOOL              skipOptPrint;
  BOOL              ReCenterPoints;
  U8                TestListTradRd[] = { OptRxBias };
  U8                TestListWr[]  = { OptWrDS, OptTxEq };
  U8                TestListRdWr[] = { OptRxBias, OptWrDS, OptTxEq };
  U8                ScaleTest[] = { 1, 1, 1, 1, 1 }; // must specify scale=0 to unpopulated slots !!
  U8                ScaleTest1DPC[] = { 1, 1, 1, 0, 0 }; // must specify scale=0 to unpopulated slots !!
  U8                *Scale;
  U16               PwrLimits[] = { 3000, 3000, 0, 0, 0 };
  S16               Best;
  DimmOptPoint      DimmOptPoints[MaxOptOff];
  U16               Points2calc[5][MaxOptOff];
  U8                PWRTrendSlope2D;
  U8                NumTests;
  U8                ArrayLength;
  U8                localChMask;
  U8                OdtTrainingDimmMask;
  S32               OdtOff;
  BOOL              ForceCenter;
  S8                GRdOdtOff;
  U32               BestGRdOdtCode;
  S8                Average;
  OptResultsPerByte BestOff;
#ifdef ULT_FLAG
  U8                TestListUltRd[]  = { OptRxBias, OptRxEq };
  BOOL              UltCpu;
#endif

  Inputs              = &MrcData->SysIn.Inputs;
  Debug               = &Inputs->Debug;
  Outputs             = &MrcData->SysOut.Outputs;
  ControllerOut       = &Outputs->Controller[0];
  Status              = mrcSuccess;
  loopcount           = 15;
  GRdOdtStep          = 16;
  IncEnds             = 1;
  SubPwrLimits        = 0;
  skipSubOpt          = 0;
  skipOptPrint        = 1;
  ReCenterPoints      = 0;
  NumTests            = 5;
  OdtTrainingDimmMask = 0;
  PWRTrendSlope2D     = 65;
  NumBytes            = Outputs->SdramCount;
  ArrayLength         = sizeof (Points2calc) / sizeof (U16) / NumTests;
#ifdef ULT_FLAG
  UltCpu             = (Inputs->CpuModel == cmHSW_ULT);
#endif

  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  MrcOemMemorySet ((U8 *) &BestOff, 0, sizeof (BestOff));
  MrcOemMemorySet ((U8 *) DimmOptPoints, 0, sizeof (DimmOptPoints));
  MrcOemMemorySet ((U8 *) Points2calc, 0, sizeof (Points2calc));
  MrcOemMemorySet ((U8 *) LocalRanks, 0, sizeof (LocalRanks));
  MrcOemMemorySet ((U8 *) RdOdtCodes, 0, sizeof (RdOdtCodes));

  TestList      = TestListRdWr;
  TestListSize  = sizeof (TestListRdWr);
  Scale         = ScaleTest;

  //
  // GOdt : [150,110, 84, 64, 50]
  // 1 dpc: Search [Off, 120, 60]
  // 2 dpc: Search [120, 60, 40]
  //      Dimm0/1 = [40/40, 40/30, 30/40, 30/30, 30/20, 20/30, 20/20]
  //
  ChMask    = 0x3;
  RankMask  = 0xf;

  //
  // Possible RttNom values to pick
  //
#ifdef ULT_FLAG
  if (UltCpu) {
    RttOffset = 0; //In ULT no Rtt nom by definition
  } else
#endif
  {
    RttOffset   = 1;
  }
  RttNomPoints  = 2;  //[120, 60]
  RdOdtPoints   = 2;  //[150,110]
  ChMask &= Outputs->ValidChBitMask;
  RankMask &= Outputs->ValidRankMask;

  any2DPC = FALSE;
  any1DPC = FALSE;
  //
  // set channel and rank population
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!((MRC_BIT0 << Channel) & ChMask)) {
      continue;
    }

    ChannelOut = &ControllerOut->Channel[Channel];
    //
    // Setup Dimm Masks for CalcPowerTrend so we don't access a Dimm that isn't present.
    //
    if (ChannelOut->DimmCount == 2) {
      any2DPC |= 1;
      OdtTrainingDimmMask |= 0x3 << (DIMM_ODT_DIMM_MASK_SHIFT * Channel);
    } else {
      any1DPC |= 1;
      OdtTrainingDimmMask |= 0x1 << (DIMM_ODT_DIMM_MASK_SHIFT * Channel);
    }

    LocalRanks[Channel] = RankMask & ChannelOut->ValidRankBitMask;
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
      //
      // start with 60 Ohm by default
      //
#ifdef ULT_FLAG
      if (MrcData->SysOut.Outputs.DdrType == MRC_DDR_TYPE_LPDDR3) {
        RttWr[Channel][Dimm] = 0;
      } else
#endif
      {
        RttWr[Channel][Dimm] = 2;
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "apply 2DPC optimization to ChMask %d\n", ChMask);

  //
  // *** Read flow ***
  // if both ch 1DPC RttNomPoints=1 and RttNom1Off=0 i.e. only GRdOdt loop
  //
  if (any2DPC == 0) {
    //
    // if no 2DPC ch
    //
    RttNomPoints  = 1;
#ifdef ULT_FLAG
    if (UltCpu && Inputs->TrainingEnables.RDEQT) {
      TestList      = TestListUltRd;
      TestListSize  = sizeof (TestListUltRd);
    } else
#endif
    {
      TestList      = TestListTradRd;
      TestListSize  = sizeof (TestListTradRd);
    }
    Scale         = ScaleTest1DPC;
  }

  OffsetPoints = 0;
  //
  // Walk Through RttNOM Settings - going from negative to positive
  //
  for (GRdOdt = 0; GRdOdt < RdOdtPoints; GRdOdt++) {
    GRdOdtOff = -16 + GRdOdt * GRdOdtStep;
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CalcRdOdt = %d, GRdOdt = %d GRdOdtOff=%d\n",CalcRdOdt(GRdOdt),GRdOdt,GRdOdtOff);
    //
    for (RttNom0 = RttOffset; RttNom0 < (RttNomPoints + RttOffset); RttNom0++) {
      //
      // Dimm0 RttNom Value
      //
      for (RttNom1Off = -1; RttNom1Off < 2; RttNom1Off++) {
        //
        // Dimm1 RttNom Value
        // Calculate RttNom1 and check if out of range
        //
        RttNom1 = RttNom0 + RttNom1Off;
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RttNom0 = %d, RttNom1 = %d, RttNom1Off = %d\n",RttNom0,RttNom1,RttNom1Off);
        //
        if ((RttNom1 == (RttNomPoints + RttOffset)) || (RttNom1 < RttOffset)) {
          continue;
        }
        //
        // if RttNom == 120ohm run recenter timing
        //
        if (((RttNom0 == 1) || (RttNom1 == 1)) && (any2DPC)) {
          ForceCenter = 1;
        } else {
          ForceCenter = 0;
        }

        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          //
          // set default opt params offset
          //
          if (!((MRC_BIT0 << Channel) & ChMask)) {
            continue; // check if the ch exist
          }

          RttNom[Channel][0]  = RttNom0;
          RttNom[Channel][1]  = RttNom1;
          for (byte = 0; byte < NumBytes; byte++) {
            UpdateOptParamOffset (MrcData, Channel, 0, byte, OptWrDS, 0, 1);
            UpdateOptParamOffset (
              MrcData,
              Channel,
              LocalRanks[Channel],
              byte,
              OptTxEq,
              (S16) (3 * (TXEQFULLDRV >> 4) + 3),
              1
              );
          }
        }
        //
        // Apply new settings and optimize various parameters
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TrainDimmOdtSetting:  GRdOdt = %d, RdOdtOff = %d, RttNom0= %d, RttNom1= %d \n",GRdOdt,RdOdtOffsets[GRdOdt], RttNom0, RttNom1);
        //
        if (OffsetPoints < MaxOptOff) {
          TrainDimmOdtSetting (
            MrcData,
            &DimmOptPoints[OffsetPoints],
            ChMask,
            RankMask,
            0,
            RttNom,
            RttWr,
            GRdOdtOff,
            TestList,
            TestListSize,
            SubPwrLimits,
            skipSubOpt,
            skipOptPrint,
            ReCenterPoints | ForceCenter,
            ReCenterPoints,
            Points2calc,
            ArrayLength,
            OffsetPoints
            );
          OffsetPoints++;
        } else {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "Error: DimmOptPoints array out of bounds! %d > %d\n",
            OffsetPoints,
            MaxOptOff - 1
            );
        }
      }
    }
  }
  //
  // for each channel apply Power Trend and find best point
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    BestRdOdt[Channel] = 0;
    if (!((MRC_BIT0 << Channel) & ChMask)) {
      continue; // check if the ch exist in this ch
    }

    for (offset = 0; offset < OffsetPoints; offset++) {
      //
      // copy point for the FindOptTradeOff routing
      //
      for (test = 0; test < (DimmOptPoints->NumTests); test++) {
        Points2calc[test][offset] = DimmOptPoints[offset].Points2Trade[test][Channel];
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "read channel=%d Points2calc[test=%d][offset=%d]=%d\n",Channel,test,offset,Points2calc[test][offset]);
        //
      }
    }
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "FindOptimalTradeOff read\n");
    // LenMargin,TestList,TestListSize,noPwrCalc
    //
    CalcPowerTrend (
      MrcData,
      Channel,
      (OdtTrainingDimmMask >> (DIMM_ODT_DIMM_MASK_SHIFT * Channel)),
      DimmOptPoints,
      Points2calc,
      MaxOptOff,
      OffsetPoints,
      DimmOptPoints->TestList,
      Scale,
      DimmOptPoints->NumTests,
      0,
      PWRTrendSlope2D
      );
    //
    // senSq=0,AveN=1,caleM=1,powerOpHigh=0
    //
    FindOptimalTradeOff (
      MrcData,
      &BestOff,
      Points2calc,
      ArrayLength,
      OffsetPoints,
      Scale,
      0,
      1,
      IncEnds,
      1,
      PwrLimits,
      0,
      0           // GuardBand
      );
    Best = BestOff.Best + BestOff.GuardBand;
    UpdateOdtsValues (
      MrcData,
      MRC_BIT0 << Channel,
      &DimmOptPoints[Best],
      0,
      0,
      skipSubOpt,
      1
      );
    //
    // MrcWait (MrcData, 10 * HPET_1MS);
    // MrcResetSequence (MrcData);
    //
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
      BestRttNom[Channel][Dimm]  = DimmOptPoints[Best].ODTSet.RttNom[Channel][Dimm];
    }

    BestRdOdt[Channel]  = DimmOptPoints[Best].ODTSet.GRdOdt;
    RdOdtCodes[Channel] = DimmOptPoints[Best].ODTSet.GRdOdtCode;

#ifdef MRC_DEBUG_PRINT
    //
    // printing the results
    //
    PrintODTResultTable (
      MrcData,
      &BestOff,
      DimmOptPoints,
      OffsetPoints,
      0,
      1,
      OptDimmOdt,
      Channel,
      LocalRanks[Channel],
      1,
      0,
      1
      );
    //
    // PrintODTResultTable(*MrcData,calcResultSummary,*TestList,NumTest,NumOffsets,MidPoint,IncEnds,OptParam,Channel,Ranks,TrendLine,Nibble,perCh);
    //
#endif // MRC_DEBUG_PRINT
  } // end ch loop
  //
  // Find Best "Average" value for Global RdOdt Offset
  //
  NumCh   = 0;
  Average = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!((MRC_BIT0 << Channel) & ChMask)) {
      continue;
    }

    NumCh += 1;
    Average += BestRdOdt[Channel];
    //
    // RdOdtChOffset[Channel] = RdOdtCodes[BestRdOdt[Channel]]; //set comp code associated comp offset per ch
    //
  }

  BestGRdOdt      = (NumCh != 0) ? (Average / NumCh) : Average;
  //
  // update average rdOdt offset (both ch)
  //
  BestGRdOdtCode  = UpdateCompGlobalOffset (MrcData, RdOdt, (U8) BestGRdOdt, 1);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Calculated Average (both ch) GRdOdt is %d\n", BestGRdOdt);

  if (NumCh > 1) {
    //
    // adjust RdOdt to per channel/byte
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!((MRC_BIT0 << Channel) & ChMask)) {
        continue;
      }

      RdOdtCodes[Channel] -= BestGRdOdtCode;
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        //
        // Apply Best RdOdt in case we didnt run Godt.
        //
        OdtOff = RdOdtCodes[Channel] + ((Outputs->Controller[0].Channel[Channel].DataCompOffset[byte] >> 12) & 0x1f);
        UpdateOptParamOffset (MrcData, Channel, 0xF, byte, OptRdOdt, (S16) OdtOff, 1);
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Read Voltage\n");
  Status = ReadVoltageCentering2D (
             MrcData,
             Outputs->MarginResult,
             ChMask,
             RdV,
             0,
             0,
             loopcount,
             0
             );

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Read Timing\n");
  Status = DataTimeCentering2D (
             MrcData,
             Outputs->MarginResult, // prev. margin results
             ChMask,
             RdT,
             0, // EnPerBit,
             0, // EnRxDutyCycle
             0, // ResetPerBit
             loopcount,
             0  // En2D
             );
  
  if (!Lpddr) {
    //
    // DIMM ODT is disabled by default on LPDDR, so skip this section
    //
    
    //
    // *** Write flow ***//
    //
    RttOffset = (Inputs->MaxRttWr < 0x2) ? (Inputs->MaxRttWr) : (0x1); // Get user input for MaxRttWr, 0 = off, 1 = 120 ohms
    if (any2DPC) {
      //
      // At least one 2DPC ch
      //
      if (!any1DPC) {
        RttOffset = 1; // Start with 120ohm
      }
    }

    TestList      = TestListWr;
    TestListSize  = sizeof (TestListWr);
    Scale         = ScaleTest1DPC;

    //
    // option 1: Keep RttWr the same for both DIMMs (ie: train per Ch for both RttWr & WrDrv)
    // option 2: Allow different RttWr for each DIMM and break WrDrv out of this optimization (ie: do it later).
    //
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
      //
      // check if dimm exist in any channel
      //
      OdtTrainingDimmMask = (0x3 << (Dimm * 2));
      if (!(Outputs->ValidRankMask & OdtTrainingDimmMask)) {
        continue;
      }
  
      localChMask = 0;
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (OdtTrainingDimmMask & LocalRanks[Channel]) {
          localChMask |= MRC_BIT0 << Channel; // can run 1 or 2 ch
        }
      }
  
      localChMask &= ChMask;
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "write odt train localChMask=%x\n", localChMask);
  
      OffsetPoints = 0;
      for (RttWr0 = RttOffset; RttWr0 < 3; RttWr0++) {
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          RttWr[Channel][Dimm] = RttWr0; // start with 60 Ohm by default
          if (!((MRC_BIT0 << Channel) & localChMask)) {
            continue; // skip if the ch doesn't exist
          }
        }
  
        if ((RttWr0 == 0) && (any1DPC)) {
          ForceCenter = 1;
        } else {
          ForceCenter = 0;
        }
  
        TrainDimmOdtSetting (
          MrcData,
          &DimmOptPoints[OffsetPoints],
          localChMask,
          OdtTrainingDimmMask,
          1,
          BestRttNom,
          RttWr,
          BestGRdOdt,
          TestList,
          TestListSize,
          SubPwrLimits,
          skipSubOpt,
          skipOptPrint,
          ReCenterPoints,
          ReCenterPoints | ForceCenter,
          Points2calc,
          ArrayLength,
          OffsetPoints
          );
        OffsetPoints++;
      }
      //
      // for each channel apply Power Trend and find best point
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!((MRC_BIT0 << Channel) & localChMask)) {
          continue; // check if the ch exist
        }
  
        for (offset = 0; offset < OffsetPoints; offset++) {
          //
          // copy point for the FindOptTradeOff routing
          //
          for (test = 0; test < (DimmOptPoints->NumTests); test++) {
            Points2calc[test][offset] = DimmOptPoints[offset].Points2Trade[test][Channel];
            //
            // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Write channel=%d Points2calc[test=%d][offset=%d]=%d\n",Channel,test,offset,Points2calc[test][offset]);
            //
          }
        }
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "FindOptimalTradeOff write\n");
        // LenMargin,TestList,TestListSize,noPwrCalc
        //
        CalcPowerTrend (
          MrcData,
          Channel,
          MRC_BIT0 << Dimm,
          DimmOptPoints,
          Points2calc,
          MaxOptOff,
          OffsetPoints,
          DimmOptPoints->TestList,
          Scale,
          DimmOptPoints->NumTests,
          0,
          PWRTrendSlope2D
          );
        //
        // senSq=0,AveN=1,caleM=1,powerOpHigh=0
        //
        FindOptimalTradeOff (
          MrcData,
          &BestOff,
          Points2calc,
          ArrayLength,
          OffsetPoints,
          Scale,
          0,
          1,
          IncEnds,
          1,
          PwrLimits,
          0,
          0               // GuardBand
          );
        Best = BestOff.Best + BestOff.GuardBand;
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "===============> BestOff=%d\n",BestOff.Best);
        // skipGRdOdt=1 SkipDimmOdts=0, SkipBestOffsets,updateHost
        //
        UpdateOdtsValues (MrcData, MRC_BIT0 << Channel, &DimmOptPoints[Best], 1, 0, skipSubOpt, 1);
        RttWr[Channel][Dimm]      = DimmOptPoints[Best].ODTSet.RttWr[Channel][Dimm];
        BestRttWr[Channel][Dimm]  = DimmOptPoints[Best].ODTSet.RttWr[Channel][Dimm]; // delete?
  #ifdef MRC_DEBUG_PRINT // printing the results
        PrintODTResultTable (
          MrcData,
          &BestOff,
          DimmOptPoints,
          OffsetPoints,
          0,
          1,
          OptDimmOdtWr,
          Channel,
          OdtTrainingDimmMask,
          1,
          0,
          1
          );
        //
        // PrintODTResultTable(*MrcData,calcResultSummary,*TestList,NumTest,NumOffsets,MidPoint,IncEnds,OptParam,Channel,Ranks,TrendLine,Nibble,perCh);
        //
  #endif // MRC_DEBUG_PRINT
      } // end of channel loop
    } // end dimm loop
    //
    //set equal TxEq for all bytes  before DS
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!((MRC_BIT0 << Channel) & Outputs->ValidChBitMask)) {
        continue; // skip if the ch doesn't exist
      }
  
      for (byte = 0; byte < NumBytes; byte++) {
        UpdateOptParamOffset (
          MrcData,
          Channel,
          LocalRanks[Channel],
          byte,
          OptTxEq,
          (S16) (3 * (TXEQFULLDRV >> 4) + 3),
          1
          );
      }
    }
  } // if (!Lpddr)
  
  //
  // run WriteDS
  //
  Status = TrainWriteDriveStrength (MrcData, Outputs->ValidChBitMask, 0, OPT_PARAM_1D_LC, 0);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Write Vref\n");
  Status = MrcWriteVoltageCentering2D (MrcData);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Write Timing\n");
  Status = DataTimeCentering2D (
             MrcData,
             Outputs->MarginResult, // prev. margin results
             Outputs->ValidChBitMask,
             WrT,
             0, // EnPerBit,
             0, // EnRxDutyCycle
             0, // ResetPerBit
             loopcount,
             0  // En2D
             );

  return Status;
}

/**
  This function implements Read Equalization training.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - if it succeeds return mrcSuccess
**/
MrcStatus
MrcReadEQTraining (
  IN MrcParameters *const MrcData
  )
{
#ifdef ULT_FLAG
  U8              RankMask;
  U8              TestList[]  = { RdV, RdT };
  U8              Scale[]     = { 1, 2, 1, 0, 0 };
  U16             PwrLimits[] = { 1280, 1280, 0, 0, 0 };
  U16             GlobalPwrLimit;
  OptOffsetChByte BestOff;

  if (MrcData->SysIn.Inputs.CpuModel == cmHSW_ULT) {
    GlobalPwrLimit  = UpmPwrLimitValue (MrcData, TestList[0], PowerLimit);
    PwrLimits[0]    = MAX (PwrLimits[0], GlobalPwrLimit);
    GlobalPwrLimit  = UpmPwrLimitValue (MrcData, TestList[1], PowerLimit);
    PwrLimits[1]    = MAX (PwrLimits[1], GlobalPwrLimit);

    //
    // Function Call for RxEQ Training
    //
    for (RankMask = 1; RankMask < (MRC_BIT0 << MAX_RANK_IN_CHANNEL); RankMask <<= 1) {
      if (RankMask & MrcData->SysOut.Outputs.ValidRankMask) {
        TrainDDROptParam (
          MrcData,
          &BestOff,
          0x3,
          RankMask,
          OptRxEq,
          TestList,
          sizeof (TestList),
          Scale,
          PwrLimits,
          0,  // Start
          7,  // Stop
          OPT_PARAM_1D_LC,
          1,  // Repeats
          0,  // NoPrint
          0,  // SkipOptUpdate
          0,  // RdRd2Test
          0   // GuardBand
          );
      }
    }
  }
#endif

  return mrcSuccess;
}

/**
  This function implements Write (Transmitter) Equalization training.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - if it succeeds return mrcSuccess
**/
MrcStatus
MrcWriteEQTraining (
  IN MrcParameters *const MrcData
  )
{
  MrcStatus       Status;
  U8              Rank;
  U8              RankMask;
  U8              TestList[]  = { WrV, WrT };
  U8              Scale[]     = { 1, 2, 1, 0, 0 }; // must specify scale=0 to unpopulate slots !!
  U16             PwrLimits[5];
  OptOffsetChByte BestOff;

  MrcOemMemorySetWord (PwrLimits, 0, sizeof (PwrLimits) / sizeof (PwrLimits[0]));
  PwrLimits[0] = UpmPwrLimitValue (MrcData, TestList[0], PowerLimit);
  PwrLimits[1] = UpmPwrLimitValue (MrcData, TestList[1], PowerLimit);

  //
  // Function Call for RxEQ Training
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    RankMask = MRC_BIT0 << Rank;
    if (RankMask & MrcData->SysOut.Outputs.ValidRankMask) {
      TrainDDROptParam (
        MrcData,
        &BestOff,
        0x3,
        RankMask,
        OptTxEq,
        TestList,
        sizeof (TestList),
        Scale,
        PwrLimits,
        0,              // Start
        11,             // Stop
        OPT_PARAM_1D_LC,
        1,              // Repeats
        0,              // NoPrint
        0,              // SkipOdtUpdate
        0,              // RdRd2Test
        2               // GuardBand
        );
    }
  }

  DataTimeCentering2D (
    MrcData,
    MrcData->SysOut.Outputs.MarginResult, // prev. margin results
    0x3,
    WrT,
    0, // EnPerBit,
    0, // EnRxDutyCycle
    0, // ResetPerBit
    OPT_PARAM_1D_LC,
    0  // En2D
    );

  Status = mrcSuccess;
  return Status;
}

/**
  This function implements Read Amplifier Power training.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeeds return mrcSuccess
**/
MrcStatus
MrcReadAmplifierPower (
  IN MrcParameters *const MrcData
  )
{
  MrcStatus       Status;
  MrcOutput       *Outputs;
  const MrcDebug  *Debug;
  U8              TestList[]  = { RdV, RdT };
  U8              Scale[]     = { 1, 2, 1, 0, 0 }; // must specify scale=0 to unpopulate slots !!
  U16             PwrLimits[5];
  OptOffsetChByte BestOff;
  BOOL            RdCenter;
  U8              RecenterLC;

  RdCenter    = 1;
  RecenterLC  = 15;
  Status      = mrcSuccess;
  Outputs     = &MrcData->SysOut.Outputs;
  Debug       = &MrcData->SysIn.Inputs.Debug;

  MrcOemMemorySetWord (PwrLimits, 0, sizeof (PwrLimits) / sizeof (PwrLimits[0]));
  PwrLimits[0] = UpmPwrLimitValue (MrcData, TestList[0], PowerLimit);
  PwrLimits[1] = UpmPwrLimitValue (MrcData, TestList[1], PowerLimit);

  //
  // Function Call for RxBias
  //
  TrainDDROptParam (
    MrcData,
    &BestOff,
    0x3,
    0xF,
    OptRxBias,
    TestList,
    sizeof (TestList),
    Scale,
    PwrLimits,
    0,              // Start
    7,              // Stop
    OPT_PARAM_1D_LC,
    1,              // Repeats
    0,              // NoPrint
    0,              // SkipOdtUpdate
    0,              // RdRd2Test
    0               // GuardBand
    );

  if (RdCenter) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Read Timing\n");
    Status = DataTimeCentering2D (
              MrcData,
              Outputs->MarginResult, // prev. margin results
              0x3,
              RdT,
              0, // EnPerBit,
              0, // EnRxDutyCycle
              0, // ResetPerBit
              RecenterLC,
              0  // En2D
              );
  }

  return Status;
}

/**
  This function implements Dimm Ron training.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus -  if it succeeds return mrcSuccess
**/
MrcStatus
MrcDimmRonTraining (
  IN MrcParameters *const MrcData
  )
{
#ifdef ULT_FLAG
  OptOffsetChByte BestOff;
  U8              TestList[]  = { RdV, RdT };
  U8              Scale[]     = { 1, 2, 1, 0, 0 }; // must specify scale=0 to unpopulate slots !!
  U16             PwrLimits[5];

  MrcOemMemorySetWord (PwrLimits, 0, sizeof (PwrLimits) / sizeof (PwrLimits[0]));
  PwrLimits[0] = UpmPwrLimitValue (MrcData, TestList[0], PowerLimit);
  PwrLimits[1] = UpmPwrLimitValue (MrcData, TestList[1], PowerLimit);

  //
  // Check if LPDDR3 memory is used
  //
  if (MrcData->SysOut.Outputs.DdrType == MRC_DDR_TYPE_LPDDR3) {
    //
    // Function Call for RxBias
    //
    TrainDDROptParam (
      MrcData,
      &BestOff,
      0x3,         // Channels
      0xF,         // Ranks
      OptDimmRon,
      TestList,
      sizeof (TestList),
      Scale,
      PwrLimits,
      0,               // Start
      2,               // Stop
      OPT_PARAM_1D_LC, // Loopcount
      1,               // Repeats
      0,               // NoPrint
      0,               // SkipOdtUpdate
      0,               // RdRd2Test
      0                // GuardBand
      );
  }
#endif // ULT_FLAG

  return mrcSuccess;

}

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
  @param[in]     GuardBand       - Signed offset to apply to the Opt param best value.

  @retval Nothing
**/
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
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcChannelOut     *ChannelOut;
  MrcControllerOut  *ControllerOut;
  MrcStatus         Status;
  U32               (*MarginByte)[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES];
  //
  // TestParam X 24 Points X Ch X Byte X Hi/Lo
  //
  U32                                 BERStats[4];
  U16                                 SaveMargin[4][24][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES];
  U16                                 Test;
  U16                                 MinEdge;
  U16                                 Margins[5][24]; // TestParam X 24 Comp Points
  U16                                 Mode;
  S16                                 Best;
  U8                                  ResultType;
  U8                                  AveN;
  U8                                  ChBitMask;
  U8                                  Channel;
  U8                                  Byte;
  U8                                  Rank;
  U8                                  Edge;
  U8                                  FirstRank;
  U8                                  OdtValue;
  U8                                  NumBytes;
  U8                                  BMap[9]; // Need by GetBERMarginByte
  U8                                  Param;
  U8                                  MaxMargin;
  U8                                  localR[MAX_CHANNEL];
  U8                                  Rep;
  void                                *NullPtr;
  S8                                  CurrentComp;
  S8                                  ReservedComp;
  S8                                  MaxComp;
  U16                                 OptPower[24];
  U8                                  PWRTrendSlope1D;
  S8                                  Delta;
  S8                                  Index;
  S8                                  Off;
  S8                                  LenMargin;
  S8                                  Shift;
  U16                                 NoiseTicks;
  BOOL                                NoSignal;
  BOOL                                IncEnds;
  BOOL                                IncEndsForPrint;
  BOOL                                CPUComp;
  BOOL                                printPerCh;
  DDRCOMP_CR_DDRCRDATACOMP0_STRUCT    DdrCrDataComp0;
  DDRCOMP_CR_DDRCRDATACOMP1_STRUCT    DdrCrDataComp1;
  DDRSCRAM_CR_DDRMISCCONTROL0_STRUCT  DdrMiscControl0;
  DDRCOMP_CR_DDRCRCOMPCTL0_STRUCT     DdrCrCompCtl0;
  DDR3_MODE_REGISTER_1_STRUCT         Ddr3ModeRegister1;
  //
  // result print summary: 5 columns per byte
  //
  OptResultsPerByte                   calcResultSummary[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];

  MaxComp             = 63;
  PWRTrendSlope1D     = 65;
  ResultType          = 0;
  NullPtr             = 0;
  CurrentComp         = 0;
  IncEnds             = 0;
  IncEndsForPrint     = 0;
  printPerCh          = 0;
  Debug               = &MrcData->SysIn.Inputs.Debug;
  Outputs             = &MrcData->SysOut.Outputs;
  ControllerOut       = &Outputs->Controller[0];
  DdrCrCompCtl0.Data  = Outputs->CompCtl0;
  MarginByte          = &Outputs->MarginResult;
  ChannelMask        &= Outputs->ValidChBitMask;
  RankMask           &= Outputs->ValidRankMask;
  MrcOemMemorySet ((U8 *) calcResultSummary, 0, sizeof (calcResultSummary));
  MrcOemMemorySet ((U8 *) BestOff, 0xffff, sizeof (OptOffsetChByte));
  MrcOemMemorySet ((U8 *) Margins, 0, sizeof (Margins));
  MrcOemMemorySet ((U8 *) OptPower, 0, sizeof (OptPower));
  MrcOemMemorySet ((U8 *) localR, 0, sizeof (localR));
  MrcOemMemorySet ((U8 *) BERStats, 0, sizeof (BERStats));
  for (Byte = 0; Byte < (sizeof (BMap) / sizeof (BMap[0])); Byte++) {
    BMap[Byte] = Byte;
  }
  Outputs->EnDumRd = 0;

  if (RdRd2Test == RdRdTA) {
    LoopCount -= 1;  // 2 TA tests, so cut the loop count in half
  } else if (RdRd2Test == RdRdTA_All) {
    LoopCount -= 3;  // 8 TA tests, so divide the loop count by 8
  }

  SetupIOTestBasicVA (MrcData, ChannelMask, LoopCount, 0, 0, 0, 8); // set test to all channels

  if (RdRd2Test != 0) {
    Outputs->DQPat = RdRd2Test;
  }
  //
  // Select All Ranks for REUT test
  //
  ChBitMask = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!((MRC_BIT0 << Channel) & ChannelMask)) {
      continue;
    }

    ChannelOut      = &ControllerOut->Channel[Channel];
    localR[Channel] = ChannelOut->ValidRankBitMask & RankMask;
    //
    // use ChBitMask from here down - if ch is set that mean at least 1 rank for testing, also remove ch w/o active ranks
    //
    ChBitMask |= SelectReutRanks (MrcData, Channel, localR[Channel], 0);
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "reut ranks ChBitMask %x Local ranks=%x\n", ChBitMask,localR[Channel]);
    // Clear any old state in DataTrain Offset
    //
    MrcOemMemorySet ((U8 *) &ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
  }

  if (ChBitMask == 0) {
    return ;
  }
  //
  // Find the first selected rank
  //
  FirstRank = 0;
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if ((MRC_BIT0 << Rank) & RankMask) {
      FirstRank = Rank; // could be in any channel
      break;
    }
  }
  //
  // Store margin results for
  //
  NumBytes = (U8) Outputs->SdramCount;

  if ((OptParam == OptDimmOdt) || (OptParam == OptDimmOdtWr) || (OptParam == OptDimmRon)) {
    NumBytes = 1;
  }
  //
  // Calculate Start/Stop Point for Comp Optimization
  //
  CPUComp = ((OptParam == OptWrDS) || (OptParam == OptRdOdt) || (OptParam == OptTComp) || (OptParam == OptSComp));
  if (CPUComp) {
    DdrCrDataComp0.Data = DdrCrDataComp1.Data = 0;
    if (OptParam == OptRdOdt) {
      DdrCrDataComp1.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG);
    } else {
      DdrCrDataComp0.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP0_REG);
    }

    switch (OptParam) {
    case OptWrDS:
      CurrentComp = (S8) DdrCrDataComp0.Bits.RcompDrvUp;
      break;

    case OptRdOdt:
      CurrentComp = (S8) DdrCrDataComp1.Bits.RcompOdtUp;
      break;

    case OptSComp:
      CurrentComp = (S8) DdrCrDataComp0.Bits.SlewRateComp;
      //
      // For SCOMP we have a 5-bit register with the max value of 31.
      // All other COMPs have 6-bit registers with the max value of 63
      //
      MaxComp = 31;
      break;

    case OptTComp:
      CurrentComp = (S8) DdrCrDataComp0.Bits.TcoComp;
      break;

    default:
      CurrentComp = 0;
      break;
    }

    ReservedComp = 3; // Reserve 3 comp codes for adjustment range
    Delta = CurrentComp - ReservedComp + Start;
    if (Delta < 0) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "------------> warning offset range is clipped by %d\n", Delta);
      Start -= Delta;
    }

    Delta = MaxComp - CurrentComp - ReservedComp - Stop;
    if (Delta < 0) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "------------> warning offset range is clipped by %d\n", Delta);
      Stop += Delta;
    }

    if (Stop < Start) {
      Stop = Start;
    }
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CurrentComp = %d, Start = %d, Stop = %d, Delta = %d\n", CurrentComp, Start, Stop, Delta);
    //
  }
  //
  // Loop through all Test Params and Measure Margin
  //
  for (Test = 0; Test < NumTests; Test++) {
    Param = TestList[Test]; // tl[0]=4 tl[1]=1
    ResultType = GetMarginResultType (Param); // rxv=0 rxt=1
    //
    // Assign to last pass margin results by reference
    // get lowest margin from all ch/rankS/byte save in FirstRank
    //
    Status = GetMarginByte (MrcData, Outputs->MarginResult, Param, FirstRank, RankMask);
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n--- FirstRank = %d ResultType=%d Param=%d ranks=0x%x\n", FirstRank,ResultType,Param,RankMask);
    // Calculate the MaxMargin for this test
    //
    MaxMargin = MAX_POSSIBLE_TIME;
    if ((Param == RdV) ||
        (Param == RdFan2) ||
        (Param == RdFan3) ||
        (Param == WrV) ||
        (Param == WrFan2) ||
        (Param == WrFan3)
        ) {
      MaxMargin = MAX_POSSIBLE_VREF;
    }
    //
    // No need to search too far
    //
    if (MaxMargin > (PwrLimitsABC[Test] / 20)) {
      MaxMargin = (U8) (PwrLimitsABC[Test] / 20);
    }
    //
    // Loop Through all Comp Codes
    //
    for (Off = Start; Off < Stop + 1; Off++) {
      Index = Off - Start;
      //
      // Apply Code
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!((MRC_BIT0 << Channel) & ChBitMask)) {
          continue;
        }

        ChannelOut = &ControllerOut->Channel[Channel];
        //
        // For DIMM ODT 2dpc, Start sweeping with RttNom = 40 and RttWr fixed at 60
        // For DIMM ODT 1dpc, Start sweeping with RttNom = Off and RttWr fixed at Off
        //
        if ((OptParam == OptDimmOdt) && (ChannelOut->DimmCount == 2)) {
          Shift = 0x20;
        } else {
          Shift = 0;
        }

        for (Byte = 0; Byte < NumBytes; Byte++) {
          if (!SkipOptUpdate) {
            //
            // change OpParam offset for all ch/byte/LocalR
            //
            UpdateOptParamOffset (MrcData, Channel, localR[Channel], Byte, OptParam, (S16) (Off + Shift), 0);
          }
          //
          // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n--Channel=%d, localR[Channel]=%x Byte=%d OffsetComp=%d Off=%d\n",Channel,localR[Channel],Byte,OffsetComp,Off);
          //
        } // some are limited in range inside e.g: RdOdt +15:-16
      }

      for (Rep = 0; Rep < Repeats; Rep++) {
        //
        // Run Margin Test - margin_1d with chosen param
        // run on all ranks but change param only for firstRank??
        //
        Mode = 0;
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " start MrcGetBERMarginByte \n");
        //
        Status = MrcGetBERMarginByte (
                  MrcData,
                  Outputs->MarginResult,
                  ChBitMask,
                  FirstRank,
                  FirstRank,
                  Param,
                  Mode,
                  BMap,
                  1,
                  MaxMargin,
                  0,
                  BERStats
                  );
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " finish MrcGetBERMarginByte \n");
        // Record Results
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (!((MRC_BIT0 << Channel) & ChBitMask)) {
            continue;
          }

          for (Edge = 0; Edge < MAX_EDGES; Edge++) {
            MinEdge = 0xFFFF;
            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              if (Rep == 0) {
                SaveMargin[Test][Index][Channel][Byte][Edge] = 0;
              }

              SaveMargin[Test][Index][Channel][Byte][Edge] +=
                (U16) (*MarginByte)[ResultType][FirstRank][Channel][Byte][Edge];
              if (MinEdge > SaveMargin[Test][Index][Channel][Byte][Edge]) {
                MinEdge = SaveMargin[Test][Index][Channel][Byte][Edge];
              }
            }

            if (NumBytes == 1) {
              SaveMargin[Test][Index][Channel][0][Edge] = MinEdge; // Todo:change Byte->0
            }
          }
        }
      }
    } // end of offset
#ifdef MRC_DEBUG_PRINT
    PrintResultTableByte4by24 (
      MrcData,
      ChBitMask,
      SaveMargin,
      Test,
      Stop - Start + 1,
      -Start,
      2,
      OptParam,
      TestList[Test],
      PwrLimitsABC,
      NoPrint
      );
#endif // MRC_DEBUG_PRINT
  } // end of test list
  //
  // Calculate the best value for every byte
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n start calculate the the best margin \n");
  //
  LenMargin = (Stop - Start) + 1;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!((MRC_BIT0 << Channel) & ChBitMask)) {
      continue;
    }

    ChannelOut = &ControllerOut->Channel[Channel];
    for (Byte = 0; Byte < NumBytes; Byte++) {
      //
      // Populate Margins array and asymetric penalty
      //
      for (Test = 0; Test < NumTests; Test++) {
        for (Off = Start; Off < Stop + 1; Off++) {
          Index = Off - Start; // 0:..
          //
          // for now just get EW
          //
          if ((TestList[Test] == RdV) ||
              (TestList[Test] == WrV) ||
              (TestList[Test] == WrFan3) ||
              (TestList[Test] == RdFan3)
              ) {
            Margins[Test][Index] = EffectiveMargin (
                                    Scale[Test] * SaveMargin[Test][Index][Channel][Byte][0],
                                    Scale[Test] * SaveMargin[Test][Index][Channel][Byte][1]
                                    );
          } else {
            Margins[Test][Index] = Scale[Test] *
              (SaveMargin[Test][Index][Channel][Byte][0] + SaveMargin[Test][Index][Channel][Byte][1]);
          }
        }
      }
      //
      // Special Cases for Running Average Filter
      //
      if ((OptParam == OptDimmOdt) || (OptParam == OptDimmOdtWr) || (OptParam == OptDimmRon)) {
        AveN = 1;
      } else if (OptParam == OptRxBias) {
        AveN = 3;
      } else if (OptParam == OptRxEq) {
        //
        // Use special, 2D running average for RxEq
        //
        AveN = 1;
        RunningAverage2D (Margins, 0, Stop - Start + 1, 5, 2, 1);
        RunningAverage2D (Margins, 1, Stop - Start + 1, 5, 2, 1); // try Cscale=1 first.
      } else {
        AveN = 7;
        if (LenMargin < AveN) {
          AveN = LenMargin - 1;
        }
      }
      //
      // Use one of the Margin Arrays for fine grain power tradeoffs. This is only used if Scale[NumTests] is not 0
      //
      for (Off = 0; Off < LenMargin; Off++) {
        OptPower[Off]          = (U16) CalcOptPower (MrcData, Channel, 0, Byte, OptParam, Off + Start, CurrentComp, 0);
        Margins[NumTests][Off] = OptPower[Off];
        if ((OptParam == OptDimmRon) || (OptParam == OptWrDS) || (OptParam == OptRdOdt)) {
          //
          // convert from Ohm to mW to pass for T-line calc : = (Vdd/2)^2/R ~ 562 / R
          //
          Margins[NumTests][Off] = 562 / Margins[NumTests][Off];
        }

        if ((OptParam == OptTxEq) && (ChannelOut->DimmCount == 1)) {
          //
          // find first rank: 0 or 2
          //
          for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
            if ((MRC_BIT0 << Rank) & ChannelOut->ValidRankBitMask) {
              FirstRank = Rank; // could be in any channel
              break;
            }
          }

          if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
            OdtValue = 0;
          } else {
            Ddr3ModeRegister1.Data = ChannelOut->Dimm[FirstRank / 2].Rank[FirstRank % 2].MR[mrMR1];
            OdtValue = (U8)
              (
                (Ddr3ModeRegister1.Bits.OdtRttValueHigh << 2) |
                (Ddr3ModeRegister1.Bits.OdtRttValueMid << 1) |
                Ddr3ModeRegister1.Bits.OdtRttValueLow
              );
          }

          if (OdtValue == 0) {
            Margins[NumTests][Off] = 1;//no power consideration
          }
        }
      }
      //
      // need to provide set of power numbers depending on the OffsetComp codes (per byte)for trend line .
      //
      CalcPowerTrend (
        MrcData,
        Channel,
        localR[Channel],
        NullPtr,
        Margins,
        24,
        LenMargin,
        TestList,
        Scale,
        NumTests,
        1,
        PWRTrendSlope1D
        );
      //
      // Use that value to create Margin Results based on power.
      // Creates a smooth, linear function that goes from MaxSum to N/(N-1)*MaxSum
      // RatioNum = FinePwrRatio[OptParam] * LenMargin; //e.g FinePwrRatio[RdOdt]=5
      // Find the Best Overall Setting
      // senSq=0,caleM=1,powerOpHigh=0
      //
      FindOptimalTradeOff (
        MrcData,
        &calcResultSummary[Channel][Byte],
        Margins,
        24,
        LenMargin,
        Scale,
        0,
        AveN,
        IncEnds,
        1,
        PwrLimitsABC,
        0,
        GuardBand
        );
      //
      // Get the best index considering the GuardBand
      //
      Best       = calcResultSummary[Channel][Byte].Best + calcResultSummary[Channel][Byte].GuardBand;
      NoiseTicks = 3;
      NoSignal   = FALSE;
      for (Test = 0; Test < NumTests; Test++) {
        if ((calcResultSummary[Channel][Byte].Ticks[Test] / 10) > NoiseTicks) {
          NoSignal = FALSE;
          //
          // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n NumTests = %d Best =%d ch=%d byte=%d calcResultSummary[Channel][Byte].Ticks[Test]=%d NoiseTicks=%d\n",NumTests,Best,Channel,Byte,calcResultSummary[Channel][Byte].Ticks[Test],NoiseTicks);
          //
          break;
        }
      }

      if (NoSignal) {
        Best = 0;//set to min
        calcResultSummary[Channel][Byte].Best = 0;
      }
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n Best =%d ch=%d byte=%d \n",Best,Channel,Byte);
      // Update CR
      //
      if ((OptParam == OptDimmOdt) && (ChannelOut->DimmCount == 2)) {
        Shift = 0x20;
      } else {
        Shift = 0;
      }
      //
      // Best += (Shift - Start);
      //
      Best -= (Shift - Start); // update take offset
      if (!SkipOptUpdate) {
        UpdateOptParamOffset (MrcData, Channel, localR[Channel], Byte, OptParam, Best, 1);
      }
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " localR[Channel]=%x Best =%d ch=%d byte=%d \n",localR[Channel],Best,Channel,Byte);
      //
      BestOff->Offset[Channel][Byte] = Best;
    } // end byte
  } // End of Calculating best value (ch)
#ifdef MRC_DEBUG_PRINT
  //
  // printing the results
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if ((MrcChannelExist (Outputs, Channel))) {
      if (!((MRC_BIT0 << Channel) & ChBitMask)) {
        continue;
      }

      ChannelOut = &ControllerOut->Channel[Channel];
      if (!(ChannelOut->ValidRankBitMask & localR[Channel])) {
        continue;
      }

      IncEndsForPrint =
        (
          OptParam == OptDimmOdt ||
          OptParam == OptDimmOdtWr ||
          OptParam == OptDimmRon ||
          OptParam == OptRxEq ||
          IncEnds
        );
      printPerCh = (OptParam == OptDimmOdt || OptParam == OptDimmOdtWr || OptParam == OptDimmRon);
      //
      // lower bytes
      //
      PrintCalcResultTableCh (
        MrcData,
        calcResultSummary,
        TestList,
        NumTests,
        Stop - Start + 1,
        -Start,
        IncEndsForPrint,
        OptParam,
        OptPower,
        Channel,
        localR[Channel],
        Scale[NumTests],
        0,
        printPerCh,
        NoPrint
        );
      //
      // higher bytes
      //
      if (!printPerCh) {
        PrintCalcResultTableCh (
          MrcData,
          calcResultSummary,
          TestList,
          NumTests,
          Stop - Start + 1,
          -Start,
          IncEndsForPrint,
          OptParam,
          OptPower,
          Channel,
          localR[Channel],
          Scale[NumTests],
          1,
          printPerCh,
          NoPrint
          );
      }
    }
  }
#endif // MRC_DEBUG_PRINT
  //
  // Propgate new CR setting
  //
  // @todo: redundant :there is one inside updateComps
  //
  if (CPUComp) {
    DdrMiscControl0.Data                  = Outputs->MiscControl0;
    DdrMiscControl0.Bits.ForceCompUpdate  = 1;
    MrcWriteCR (MrcData, DDRSCRAM_CR_DDRMISCCONTROL0_REG, DdrMiscControl0.Data);
  }
  //
  // Update the LastPass points in host
  //
  for (Test = 0; Test < NumTests; Test++) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!((MRC_BIT0 << Channel) & ChBitMask)) {
        continue;
      }

      ResultType = GetMarginResultType (TestList[Test]);
      for (Byte = 0; Byte < NumBytes; Byte++) {
        //
        // save the margins in best offset point for each byte/ch in  rank 0/1
        //
        (*MarginByte)[ResultType][0][Channel][Byte][0] =
          SaveMargin[Test][BestOff->Offset[Channel][Byte] - Start][Channel][Byte][0];
        (*MarginByte)[ResultType][0][Channel][Byte][1] =
          SaveMargin[Test][BestOff->Offset[Channel][Byte] - Start][Channel][Byte][1];
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "best offset= %d ;byte=%d ;(*MarginByte)[ResultType][0][Channel][Byte][0] -%d (*MarginByte)[ResultType][0][Channel][Byte][1] -%d add=%d\n",BestOff->Offset[Channel][Byte],Byte,(U16) (*MarginByte)[ResultType][0][Channel][Byte][0] , (*MarginByte)[ResultType][0][Channel][Byte][1],((U16) (*MarginByte)[ResultType][0][Channel][Byte][0] + (U16)(*MarginByte)[ResultType][0][Channel][Byte][1]));
        //
      }
    }

    Status = ScaleMarginByte (MrcData, Outputs->MarginResult, TestList[Test], 0);
  }

  BestOff->NumTests = sizeof (TestList);
  for (Test = 0; Test < NumTests; Test++) {
    ResultType = GetMarginResultType (TestList[Test]);
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!((MRC_BIT0 << Channel) & ChBitMask)) {
        continue;
      }
      //
      // track minimum eye width per ch
      //
      for (Byte = 0; Byte < NumBytes; Byte++) {
        if (Byte == 0) {
          BestOff->Margins[Test][Channel] = (U16) ((*MarginByte)[ResultType][0][Channel][0][0] +
            (*MarginByte)[ResultType][0][Channel][0][1]);
        } else if (BestOff->Margins[Test][Channel] >
                   ((*MarginByte)[ResultType][0][Channel][Byte][0] + (*MarginByte)[ResultType][0][Channel][Byte][1])
                  ) {
          BestOff->Margins[Test][Channel] = (U16) ((*MarginByte)[ResultType][0][Channel][Byte][0] +
            (*MarginByte)[ResultType][0][Channel][Byte][1]);
        }
      }

      BestOff->TestList[Test][Channel] = TestList[Test];

      //
      // Normalize margins
      // BestOff->Margins[Test][Channel] *= Scale[Test]; //Scale??
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "after scale - BestOff->Margins[ch=%d][%s]= %d \n",Channel,MarginTypesString[TestList[Test]],BestOff->Margins[Test][Channel]);
      //
    }
    //
    // if (BestOff->Margins[Test][Channel]<=20) {//set OptParam to max in case off no eye
    //   for (Byte = 0; Byte < NumBytes; Byte++) {
    //     if(OptParam == OptRxBias) UpdateOptParamOffset (MrcData, Channel, 0, Byte, OptRxBias, 15, 1);
    //     if(OptParam == OptWrDS) UpdateOptParamOffset (MrcData, Channel, 0, Byte, OptWrDS, 7, 1);
    //   }
    // }
    //
  }
  //
  // Clean up
  //
  Outputs->EnDumRd = 0;

  return;

}

/**
  This function implements Read ODT training.
  Optimize Read ODT strength for performance & power

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeded return mrcSuccess
**/
MrcStatus
MrcReadODTTraining (
  IN MrcParameters *const MrcData
  )
{
  MrcStatus                         Status;
  const MrcInput                   *Inputs;
  const MrcDebug                   *Debug;
  MrcOutput                         *Outputs;
  MrcCpuModel                       CpuModel;
  U8                                *TestList;
  U8                                TestListDdr3[]  = { RdV, RdT };
  U8                                TestListSize;
  U8                                ScaleDdr3[]     = { 1, 2, 1, 0, 0 };
  U8                                *Scale;
  U16                               PwrLimits[5]; // Eye width/height
  S8                                Start;
  S8                                Stop;
  S16                               OffLimit;
  S16                               OffLimitDn;
  U16                               OdtLimit;
  U8                                OdtLimitDn;
  U16                               Rleg;
  S8                                StatLegs;
  U8                                OdtLegsDis;
  S8                                CurrentVref;
  S8                                CurrentComp;
  OptOffsetChByte                   BestOff;
  BOOL                              RdCenter;
  U8                                RecenterLC;
  DDRCOMP_CR_DDRCRCOMPCTL0_STRUCT   DdrCrCompCtl0;
  DDRCOMP_CR_DDRCRDATACOMP1_STRUCT  DdrCrDataComp1;
  U8                                RdTATestType;
  U8                                Index;
#ifdef ULT_FLAG
  U8                                TestListLpddr[] = { RdV, RdT, RcvEnaX };
  U8                                ScaleLpddr[]    = { 1, 2, 2, 1, 0 };
  BOOL                              Lpddr;
#endif

  Inputs      = &MrcData->SysIn.Inputs;
  Outputs     = &MrcData->SysOut.Outputs;
  Debug       = &Inputs->Debug;
  CpuModel    = Inputs->CpuModel;
  Status      = mrcSuccess;
  RdCenter    = 1;
  RecenterLC  = 17;
  Start       = -16;
  MrcOemMemorySetWord (PwrLimits, 0, sizeof (PwrLimits) / sizeof (PwrLimits[0]));

  //
  // find a start offset where we below 180ohm to protect against OS/US
  //
  DdrCrDataComp1.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG);
  CurrentComp         = (S8) DdrCrDataComp1.Bits.RcompOdtUp;
  OdtLimitDn          = 30; //ohm
  TestList            = TestListDdr3;
  TestListSize        = sizeof (TestListDdr3);
  Scale               = ScaleDdr3;
  RdTATestType        = RdRdTA;

#ifdef ULT_FLAG
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);
  if (CpuModel == cmHSW_ULT) {
    OdtLimit = 230;
    if (Lpddr) {
      RdTATestType = RdRdTA_All;
      OdtLimitDn   = 80; //ohm
      TestList     = TestListLpddr;
      Scale        = ScaleLpddr;
      TestListSize = sizeof (TestListLpddr);
    }
  } else
#endif //ULT_FLAG
  {
    OdtLimit = 180;
  }

  for (Index = 0; Index < TestListSize; Index++) {
    PwrLimits[Index] = UpmPwrLimitValue (MrcData, TestList[Index], PowerLimit);
  }

  DdrCrCompCtl0.Data  = Outputs->CompCtl0;
  OdtLegsDis          = (U8) DdrCrCompCtl0.Bits.DisableOdtStatic;
  CurrentVref         = (S8) DdrCrCompCtl0.Bits.DqOdtVref;
  StatLegs            = 4 * 4; // we enable only 1/3 segment for odt
  if (CurrentVref & 0x10) {
    CurrentVref -= 0x20; // 2's complement
  }

  Rleg      = CalcRdOdt (MrcData, CurrentVref) * (StatLegs * (!OdtLegsDis) + CurrentComp);
  OffLimit  = (Rleg / OdtLimit) - StatLegs * (!OdtLegsDis) - CurrentComp;

  //
  // Find max ODT offset 
  //
  OffLimitDn  = (Rleg / OdtLimitDn) - StatLegs * (!OdtLegsDis) - CurrentComp;
  Stop        = (U8) OffLimitDn;

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_ERROR,
    " current code = %d OdtLegsDis = %d Rleg = %d CurrentVref = %d OffLimit = %d Start = %d stop = %d\n",
    CurrentComp,
    OdtLegsDis,
    Rleg,
    CurrentVref,
    OffLimit,
    Start,
    Stop
    );

  if (OffLimit > Start) {
    Start = (S8) OffLimit;
  }

  if (Stop > (23 + Start)) {
    Stop = (S8) (23 + Start); // Only 24 offsets in the margin array.
  }
  if (Stop > 15) {
    Stop = 15;
  }

  //
  // Function Call for RdODT
  //
  TrainDDROptParam (
    MrcData,
    &BestOff,
    0x3,
    0xF,
    OptRdOdt,
    TestList,
    TestListSize,
    Scale,
    PwrLimits,
    Start,
    Stop,           // Stop
    17,             // Loopcount increased from 15 to better match RMT margins
    1,              // Repeats
    0,              // NoPrint
    0,              // SkipOptUpdate
    RdTATestType,   // RdRd2Test
    0               // GuardBand
    );

  if (RdCenter) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Read Timing\n");
    Status = DataTimeCentering2D (
              MrcData,
              Outputs->MarginResult, // prev. margin results
              0x3,
              RdT,
              0, // EnPerBit,
              0, // EnRxDutyCycle
              0, // ResetPerBit
              RecenterLC,
              0  // En2D
              );
  }

  return Status;
}

/**
  This function implements Dimm Odt training.
  Optimize Dimm Odt value for performance/power

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeded return mrcSuccess
**/
MrcStatus
MrcDimmODT1dTraining (
  IN MrcParameters *const MrcData
  )
{
  MrcStatus       Status;
  MrcOutput       *Outputs;
  U8              TestList[] = { RdV, RdT, WrV, WrT }; // ie 4,1
  U8              TestListWr[] = { WrV, WrT }; // ie 4,1
  U8              Scale[]     = { 1, 2, 1, 2, 0 };
  U8              ScaleWr[]   = { 1, 2, 0, 0, 0 };
  U16             PwrLimits[] = { 2480, 2240, 2480, 2240, 0 }; // just margin consideration
  U8              dimm;
  U8              Channel;
  U8              ChannelMask;
  OptOffsetChByte BestOff;

  Outputs = &MrcData->SysOut.Outputs;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if ((MrcChannelExist (Outputs, Channel))) {
      ChannelMask = MRC_BIT0 << Channel;
      if (Outputs->Controller[0].Channel[Channel].DimmCount == 2) {
        //
        // DimmODT Rtt Nom - 120,60,40,30
        // run Rtt nom with the Rtt write 0x20=60 ohm
        //
        TrainDDROptParam (
          MrcData,
          &BestOff,
          ChannelMask,
          0xF,
          OptDimmOdt,
          TestList,
          sizeof (TestList),
          Scale,
          PwrLimits,
          1,  // Start
          4,  // Stop
          OPT_PARAM_1D_LC,
          1,  // Repeats
          0,  // NoPrint
          0,  // SkipOdtUpdate
          0,  // RdRd2Test
          0   // GuardBand
          );
        for (dimm = 0; dimm < 2; dimm++) {
          //
          // Function Call for DimmODT Write - 120,60
          //
          TrainDDROptParam (
            MrcData,
            &BestOff,
            ChannelMask,
            0x3 << (dimm * 2),
            OptDimmOdtWr,
            TestListWr,
            sizeof (TestListWr),
            ScaleWr,
            PwrLimits,
            1,  // Start
            2,  // Stop
            OPT_PARAM_1D_LC,
            1,  // Repeats
            0,  // NoPrint
            0,  // SkipOdtUpdate
            0,  // RdRd2Test
            0   // GuardBand
            );
        }
      } else {
        //
        // 1DPC (only write) - off,120,60
        //
        TrainDDROptParam (
          MrcData,
          &BestOff,
          ChannelMask,
          0xF,
          OptDimmOdt,
          TestListWr,
          sizeof (TestListWr),
          ScaleWr,
          PwrLimits,
          0,  // Start
          2,  // Stop
          OPT_PARAM_1D_LC,
          1,  // Repeats
          0,  // NoPrint
          0,  // SkipOdtUpdate
          0,  // RdRd2Test
          0   // GuardBand
          );
      }
    }
  }

  Status = mrcSuccess;

  return Status;
}

/**
  This function is the Write Drive Strength training entry point.
  This step will optimize write drive strength for performance & power.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeeds return mrcSuccess
**/
MrcStatus
MrcWriteDriveStrength (
  IN MrcParameters *const MrcData
  )
{
  MrcStatus Status;
  U8        OptParamLC;
  U8        RecenterLC;
  BOOL      Recenter;

  Status      = mrcSuccess;
  OptParamLC  = OPT_PARAM_LOOP_COUNT;
  RecenterLC  = OPT_PARAM_1D_LC;
  Recenter    = 1;

  Status      = TrainWriteDriveStrength (MrcData, 0x3, RecenterLC, OptParamLC, Recenter);

  return Status;
}

/**
  This function implements the Write Drive Strength optimization for performance and power.

  @param[in] MrcData    - Include all MRC global data.
  @param[in] ChBitMask  - Channel mask to perform training on the Opt Param test list.
  @param[in] RecenterLC - The loopcount for Write Time recentering.
  @param[in] OptParamLC - The loopcount for training the Opt Param test list.
  @param[in] Recenter   - Switch which determines if the step recenters Write Timing.

  @retval If it succeeds return mrcSuccess
**/
MrcStatus
TrainWriteDriveStrength (
  IN MrcParameters *const MrcData,
  IN const U8             ChBitMask,
  IN const U8             RecenterLC,
  IN const U8             OptParamLC,
  IN const BOOL           Recenter
  )
{
  MrcStatus       Status;
  MrcOutput       *Outputs;
  const MrcDebug  *Debug;
  U8              TestList[]  = { WrV, WrT };
  U8              Scale[]     = { 1, 2, 1, 0, 0 };
  U16             PwrLimits[5];
  OptOffsetChByte BestOff;

  Status  = mrcSuccess;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &MrcData->SysIn.Inputs.Debug;

  MrcOemMemorySetWord (PwrLimits, 0, sizeof (PwrLimits) / sizeof (PwrLimits[0]));
  PwrLimits[0] = UpmPwrLimitValue (MrcData, TestList[0], PowerLimit);
  PwrLimits[1] = UpmPwrLimitValue (MrcData, TestList[1], PowerLimit);

  TrainDDROptParam (
    MrcData,
    &BestOff,
    ChBitMask,
    0xf,
    OptWrDS,
    TestList,
    sizeof (TestList),
    Scale,
    PwrLimits,
    -13,        // Start
    10,         // Stop
    OptParamLC,
    1,          // Repeats
    0,          // NoPrint
    0,          // SkipOdtUpdate
    0,          // RdRd2Test
    1           // GuardBand
    );

  if (Recenter) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Write Timing\n");
    Status = DataTimeCentering2D (
              MrcData,
              Outputs->MarginResult,  // prev. margin results
              0x3,
              WrT,
              0,                      // EnPerBit,
              0,                      // EnRxDutyCycle
              0,                      // ResetPerBit
              RecenterLC,
              0                       // En2D
              );
  }

  return Status;
}

/**
  This function implements Write Slew Rate training.
  Optimize Write Slew Rate for performance & power

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess
**/
MrcStatus
MrcWriteSlewRate (
  IN MrcParameters *const MrcData
  )
{
#ifdef ULT_FLAG
  U8              TestList[]  = { WrV, WrT };
  U8              Scale[]     = { 1, 2, 0, 0, 0 };
  U16             PwrLimits[] = { 2480, 2240, 0, 0, 0 }; // no power consideration
  U16             GlobalPwrLimit;
  OptOffsetChByte BestOff;

  if (MrcData->SysIn.Inputs.CpuModel == cmHSW_ULT) {
    GlobalPwrLimit  = UpmPwrLimitValue (MrcData, TestList[0], PowerLimit);
    PwrLimits[0]    = MAX (PwrLimits[0], GlobalPwrLimit);
    GlobalPwrLimit  = UpmPwrLimitValue (MrcData, TestList[1], PowerLimit);
    PwrLimits[1]    = MAX (PwrLimits[1], GlobalPwrLimit);

    TrainDDROptParam (
      MrcData,
      &BestOff,
      0x3,
      0xf,
      OptSComp,
      TestList,
      sizeof (TestList),
      Scale,
      PwrLimits,
      -15,  // Start
      8,    // Stop
      OPT_PARAM_1D_LC,
      1,    // Repeats
      0,    // NoPrint
      0,    // SkipOdtUpdate
      0,    // RdRd2Test
      0     // GuardBand
      );
  }
#endif

  return mrcSuccess;
}

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
void
UpdateOptParamOffset (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Channel,
  IN     const U8             Ranks,
  IN     const U8             Byte,
  IN     const U8             OptParam,
  IN     S16                  Off,
  IN     const U8             UpdateHost
  )
{
  const U16       RttNomMRSEncodingConst[] = {0x00, 0x10, 0x01, 0x11, 0x81, 0x80}; // RttNom Off,120,60,40,30,20 Ohms
  const U16       RttWrMRSEncodingConst[]  = {0x00, 0x02, 0x01};                // RttWr  RttNom,120,60 Ohms
  const U16       RttDimmRonEncodingConst[]  = {0x00, 0x02};                // Dimm Ron  240/6,240/7 Oms
  const MrcDebug  *Debug;
#ifdef ULT_FLAG
  const U8        LpddrRonEnc[] = {0x1,0x2,0x3}; //{34,40,48};
  const U8        LpddrOdtEnc[] = {0x0,0x2,0x3}; //{0,120,240};
  BOOL            Lpddr;
#endif // ULT_FLAG
  MrcOutput       *Outputs;
  MrcChannelOut   *ChannelOut;
  U16             *MrReg;
  MrcStatus       Status;
  BOOL            Type;
  U8              Rank;
  U8              RankMask;
  U8              Value;
  U8              Index;
  U16             MRValue;
  U16             RttNomMRSEncoding[sizeof (RttNomMRSEncodingConst) / sizeof (RttNomMRSEncodingConst[0])];
  U16             RttWrMRSEncoding[sizeof (RttWrMRSEncodingConst) / sizeof (RttWrMRSEncodingConst[0])];
  U16             RttWr, RttNom, RttNomMask;
  U16             DimmRon;
  U16             RttWrMask;
  U16             DimmRonMask;
  U32             Offset;
  S16             OffCode;
  S16             OffMin;
  S16             OffMax;
  DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_STRUCT DdrCrDataOffsetComp;
  DDRSCRAM_CR_DDRMISCCONTROL0_STRUCT        DdrMiscControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL1_STRUCT   DdrCrDataControl1;

  Debug       = &MrcData->SysIn.Inputs.Debug;
  Outputs     = &MrcData->SysOut.Outputs;
  ChannelOut  = &Outputs->Controller[0].Channel[Channel];
  MrcOemMemoryCpy ((U8 *) RttNomMRSEncoding, (U8 *) RttNomMRSEncodingConst, sizeof (RttNomMRSEncoding));
  MrcOemMemoryCpy ((U8 *) RttWrMRSEncoding, (U8 *) RttWrMRSEncodingConst, sizeof (RttWrMRSEncoding));
#ifdef ULT_FLAG
  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);
#endif // ULT_FLAG

  //
  // Compensation Offsets
  //
  Type =
    (
      (OptParam == OptWrDS) ||
      (OptParam == OptRdOdt) ||
      (OptParam == OptTComp) ||
      (OptParam == OptSComp) ||
      (OptParam == OptDefault)
    );
  if (Type) {
    if (OptParam == OptWrDS) {
      OffMin = -32;
      OffMax = 31;
    } else {
      OffMin = -16;
      OffMax = 15;
    }

    if (Off > OffMax) {
      Off = OffMax;
    } else if (Off < OffMin) {
      Off = OffMin;
    }

    DdrCrDataOffsetComp.Data = ChannelOut->DataCompOffset[Byte];

    if (OptParam == OptWrDS) {
      DdrCrDataOffsetComp.Bits.DqDrvUpCompOffset    = Off;
      DdrCrDataOffsetComp.Bits.DqDrvDownCompOffset  = Off;
    } else if (OptParam == OptRdOdt) {
      DdrCrDataOffsetComp.Bits.DqOdtUpCompOffset    = Off;
      DdrCrDataOffsetComp.Bits.DqOdtDownCompOffset  = Off;
    } else if (OptParam == OptTComp) {
      DdrCrDataOffsetComp.Bits.DqTcoCompOffset = Off;
    } else if (OptParam == OptSComp) {
      DdrCrDataOffsetComp.Bits.DqSlewRateCompOffset = Off;
    }

    Offset = DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_REG +
      ((DDRDATA1CH0_CR_DDRCRDATAOFFSETCOMP_REG - DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_REG) * Byte) +
      ((DDRDATA0CH1_CR_DDRCRDATAOFFSETCOMP_REG - DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_REG) * Channel);
    MrcWriteCR (MrcData, Offset, DdrCrDataOffsetComp.Data);
    if (UpdateHost) {
      ChannelOut->DataCompOffset[Byte] = DdrCrDataOffsetComp.Data;
    }
    //
    // Propagate new value and force comp update
    //
    DdrMiscControl0.Data                  = Outputs->MiscControl0;
    DdrMiscControl0.Bits.ForceCompUpdate  = 1;
    MrcWriteCR (MrcData, DDRSCRAM_CR_DDRMISCCONTROL0_REG, DdrMiscControl0.Data);
  }
  //
  // Equalization Settings
  //
  Type = ((OptParam == OptTxEq) || (OptParam == OptRxEq) || (OptParam == OptDefault));
  if (Type) {
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if ((MrcRankInChannelExist (MrcData, Rank, Channel)) && ((Ranks & (MRC_BIT0 << Rank)))) {
        //
        // TxEq[5:4] = Emphasize   = [3, 6, 9, 12] legs
        // TxEq[3:0] = Deemphasize = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 4*Rsvd] legs
        //
        if (OptParam == OptTxEq) {
          if (Off > 11) {
            Off = 11;
          }

          if (Off < 0) {
            Off = 0;
          }

          OffCode = Off | TXEQFULLDRV; // Use 12 Emphasize legs (not trained)
          //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel=%d,Rank= %d update to %x \n",Channel,Rank,OffCode);
          UpdateTxT (MrcData, Channel, Rank, Byte, 2, OffCode);
          if (UpdateHost) {
            ChannelOut->TxEq[Rank][Byte] = (S8) OffCode;
          }
        }
        //
        // RxEQ[4:0] CR Decoding (pF/kOhm)
        //            [2:0]
        //  [4:3]     0        1        2        3        4        5-7
        //     0      0.5/.02  0.5/1.0  0.5/.50  0.5/.25  0.5/.12  rsvd
        //     1      1.0/.02  1.0/1.0  1.0/.50  1.0/.25  1.0/.12  rsvd
        //     2      1.5/.02  1.5/1.0  1.5/.50  1.5/.25  1.5/.12  rsvd
        //     3      2.0/.02  2.0/1.0  2.0/.50  2.0/.25  2.0/.12  rsvd
        // Sweep = 0-19        [4:3] = (Sweep/5)  [2:0] = (Sweep%5)
        //
        if (OptParam == OptRxEq) {
          if (Off > 19) {
            Off = 19;
          }

          if (Off < 0) {
            Off = 0;
          }

          Value = (U8) (((Off / 5) << 3) + (Off % 5));
          UpdateRxT (MrcData, Channel, Rank, Byte, 2, Value);
          if (UpdateHost) {
            ChannelOut->RxEq[Rank][Byte] = Value;
          }
        }

        if (OptParam == OptDefault) {
          UpdateTxT (MrcData, Channel, Rank, Byte, 0xff, 0);
          UpdateRxT (MrcData, Channel, Rank, Byte, 0xff, 0);
        }
      }
    }
  }
  //
  // RX Amplifier BIAS
  //
  if ((OptParam == OptRxBias) || (OptParam == OptDefault)) {
    if (Off > 7) {
      Off = 7;
    }

    if (Off < 0) {
      Off = 0;
    }
    //
    // Mapping: [0: 0.44, 1: 0.66, 2: 0.88, 3: 1.00, 4: 1.33, 5: 1.66, 6: 2.00, 7: 2.33]
    //
    DdrCrDataControl1.Data = ChannelOut->DqControl1[Byte].Data;
    if (OptParam == OptRxBias) {
      DdrCrDataControl1.Bits.RxBiasCtl = Off;
    }

    Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG +
      ((DDRDATA1CH0_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Byte) +
      ((DDRDATA0CH1_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Channel);
    MrcWriteCR (MrcData, Offset, DdrCrDataControl1.Data);
    if (UpdateHost) {
      ChannelOut->DqControl1[Byte].Data = DdrCrDataControl1.Data;
    }
  }
  //
  // Update Dimm Ron value
  //
  if ((OptParam == OptDimmRon)) {
#ifdef ULT_FLAG
    //
    // Check if LPDDR3 memory is used
    //
    if (Lpddr) {
      DimmRonMask = (U16)~(MRC_BIT3 | MRC_BIT2 | MRC_BIT1 | MRC_BIT0);
      Index       = (U8) Off;
      Index       = MIN (Index, sizeof (LpddrRonEnc) / sizeof (LpddrRonEnc[0]) - 1);
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        RankMask = MRC_BIT0 << Rank;
        if ((MrcRankInChannelExist (MrcData, Rank, Channel)) && (Ranks & RankMask)) {
          MrReg = &ChannelOut->Dimm[Rank / 2].Rank[(Rank % 2)].MR[mrMR0];
          //
          // Program Dimm Ron
          //
          DimmRon = LpddrRonEnc[Index];
          MRValue = (MrReg[mrMR3] & DimmRonMask) | DimmRon;
          Status = MrcIssueMrw (
                    MrcData,
                    Channel,
                    Rank,
                    mrMR3,
                    MRValue,
                    FALSE,  // InitMrw
                    FALSE   // ChipSelect2N
                    );
          if (UpdateHost) {
            MrReg[mrMR3] = MRValue;
          }
        }
      }
    } else
#endif // ULT_FLAG
    {
      //
      // DIMM Ron Encoding   RttNom[A5,A1]
      //
      DimmRonMask = (U16)~(MRC_BIT5 | MRC_BIT1);
      Index       = (U8) Off;
      Index       = MIN (Index, 1);
      //
      // can be 0 or 1
      //
      DimmRon = RttDimmRonEncodingConst[Index];
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        RankMask = MRC_BIT0 << Rank;
        if ((MrcRankInChannelExist (MrcData, Rank, Channel)) && (Ranks & RankMask)) {
          MrReg = &ChannelOut->Dimm[Rank / 2].Rank[(Rank % 2)].MR[mrMR0];
          //
          // Program Dimm Ron
          //
          MRValue = (MrReg[mrMR1] & DimmRonMask) | DimmRon;
          Status  = MrcWriteMRS (MrcData, Channel, RankMask, mrMR1, MRValue);
          if (UpdateHost) {
            MrReg[mrMR1] = MRValue;
          }
        }
      }
    }
  }
  //
  // DIMM ODT Values
  //
  if ((OptParam == OptDimmOdt) || (OptParam == OptDimmOdtWr)) {
#ifdef ULT_FLAG
    if (Lpddr) {
      //
      // We have only Odt write
      //
      RttWrMask = (U16)~(MRC_BIT1 | MRC_BIT0);
      Index     = (U8) Off;
      Index     = MIN (Index, sizeof (LpddrOdtEnc) / sizeof (LpddrOdtEnc[0]) - 1);
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        RankMask = MRC_BIT0 << Rank;
        if ((MrcRankInChannelExist (MrcData, Rank, Channel)) && (Ranks & RankMask)) {
          MrReg   = &ChannelOut->Dimm[Rank / 2].Rank[Rank % 2].MR11;
          MRValue = *MrReg;
          //
          // Program Dimm DS
          //
          RttWr   = LpddrOdtEnc[Index];
          MRValue = (MRValue & RttWrMask) | RttWr;
          Status = MrcIssueMrw (
                    MrcData,
                    Channel,
                    Rank,
                    mrMR11,
                    MRValue,
                    FALSE,  // InitMrw
                    FALSE   // ChipSelect2N
                    );
          if (UpdateHost) {
            *MrReg = MRValue;
          }
        }
      }

      return;
    }
#endif
    //
    // DIMM ODT Encoding   RttNom[A9,A6,A2]   RttWr[A10, A9]
    //
    RttNomMask  = (U16)~(MRC_BIT9 | MRC_BIT6 | MRC_BIT2);
    RttWrMask   = (U16)~(MRC_BIT10 | MRC_BIT9);

    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      RankMask = MRC_BIT0 << Rank;
      if ((MrcRankInChannelExist (MrcData, Rank, Channel)) && (Ranks & RankMask)) {
        MrReg = &ChannelOut->Dimm[Rank / 2].Rank[(Rank % 2)].MR[mrMR0];

        //
        // Program RTT WR
        //
        Index   = (U8) ((OptParam == OptDimmOdt) ? (Off >> 4) : Off);
        Index   = MIN (Index, 2);
        RttWr   = RttWrMRSEncoding[Index] << 9;
        MRValue = (MrReg[mrMR2] & RttWrMask) | RttWr;
        Status  = MrcWriteMRS (MrcData, Channel, RankMask, mrMR2, MRValue);
        if (UpdateHost) {
          MrReg[mrMR2] = MRValue;
        }
        //
        // Program RTT NOM
        //
        if (OptParam == OptDimmOdtWr) {
          continue;
        }

        Index = ((U8) Off & 0xF);
        if (Index > 5) {
          Index = 5;
        }

        RttNom  = RttNomMRSEncoding[Index] << 2;
        MRValue = (MrReg[mrMR1] & RttNomMask) | RttNom;
        Status  = MrcWriteMRS (MrcData, Channel, RankMask, mrMR1, MRValue);
        if (UpdateHost) {
          MrReg[mrMR1] = MRValue;
        }
      }
    }
  }

  return;
}

/**
  Slightly penalize any Asymmetry in margin

  @param[in] NegEdge - Negative edge of the margin
  @param[in] PosEdge - Positive edge of the margin

  @retval p2p - Width/Height reduced by the asymmetric difference in margin.
**/
U16
EffectiveMargin (
  IN const U16 NegEdge,
  IN const U16 PosEdge
  )
{
  S16 p2p;
  U16 p2pDiff;

  p2p     = 2 * (PosEdge + NegEdge);
  p2pDiff = PosEdge - NegEdge;

  if (PosEdge > NegEdge) {
    p2p -= p2pDiff;
  } else {
    p2p += p2pDiff;
  }

  return p2p / 2;
}

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
void
RunningAverage2D (
  IN OUT U16       Margins[2][24],
  IN     const U8  Test,
  IN     const U8  MLen,
  IN     const U8  XDim,
  IN     const U8  XMin,
  IN     const U8  CScale
)

{
  U8  XMax;
  U8  YMax;
  U16 TMargins[24];
  U8  i;
  U8  x;
  U8  y;
  U8  xo;
  U8  yo;
  U8  XOff;
  S8  YOff;

  XMax  = XDim - 1;
  YMax  = ((MLen + XDim - 1) / XDim) - 1; // Ceiling to int in case the matrix is not fully populated

  for (i = 0; i < MLen; i++) {
    x = (i % XDim);
    y = (i / XDim);

    //
    // Center Point
    //
    TMargins[i] = Margins[Test][i] * (CScale - 1); // Also add margin at the centerpoint below
    //
    // Sum up surrounding results
    //
    for (xo = 0; xo < 3; xo++) {
      XOff = x + xo - 1;
      //
      // Avoid negative numbers on XOff
      //
      if ((x == 0) && (xo == 0)) {
        XOff = 0;
      }
      //
      // (x < XMin) allows averaging across points (1;0) and (2;0)
      //
      if ((XOff < XMin) && (x < XMin)) {
        XOff = x; // RxEq special case.  Skip averaging on Col0/Col1
      }

      if (XOff > XMax) {
        XOff = XMax;
      }

      for (yo = 0; yo < 3; yo++) {
        YOff = y + yo - 1;
        if (YOff < 0) {
          YOff = 0;
        }

        if (YOff > YMax) {
          YOff = YMax;
        }
        //
        // Avoid averaging with unpopulated matrix elements when dealing with partially populated matrices
        //
        if ((XDim * YOff + XOff) > (MLen - 1)) {
          YOff = YOff - 1;
        }

        TMargins[i] += Margins[Test][XDim * YOff + XOff];
      }
    }
  }
  //
  // Copy TempMargins back over to real margins
  //
  for (i = 0; i < MLen; i++) {
    Margins[Test][i] = TMargins[i] / (8 + CScale); // Added div to maintain margin scaling
  }

  return;
}

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
  )

{
  const MrcDebug    *Debug;
  U8                NumArr;  // Arrays to keep track of results
  U32               PostMar[5][MaxOptOff];  // Margin array after scaling & averaging
  U32               MaxPost[5];  // Variables for Results
  U32               SMaxPost[5];
  U32               MinPost[5];
  U32               Signal[5];
  U32               Noise[5];
  U32               Ratio[5];
  U16               PwrLimitPost[5];
  U32               ScaleMin;
  U8                Nby2;
  U8                EqOrder;
  U8                xArr;
  U8                yArr;
  U8                x;
  U8                i;
  U8                Off;
  S8                xEff;
  S32               n;
  U8                NumBits;
  U32               localY;
  U8                Shift;
  U8                Adder;
  U8                Start;
  U8                Stop;
  U64               Result;
  U64               rlocal;
  U64               MaxR;
  U64               MinR;
  U64               SNRTotal;
  U64               MarginLimit;
  U8                BestX;
  U8                PowerX;
  U8                FoundPwrOpt;
  U8                NumCalcArr;
  S8                StepSize;
  U8                MarginDropPercent;
  U32               MinPost1;
  BOOL              GoodPower;
  U16               *Margins;
  OptResultsPerByte *calcResults;

  MarginDropPercent = 10;  // 10% loss of margin is a bad guardband offset.
  NumArr            = 5;
  Result            = 0;
  rlocal            = 0;
  MaxR              = 0;
  MinR              = 0;
  SNRTotal          = 0;
  Debug             = &MrcData->SysIn.Inputs.Debug;
  MrcOemMemorySetDword (MaxPost, 1, sizeof (MaxPost) / sizeof (U32));
  MrcOemMemorySetDword (SMaxPost, 1, sizeof (SMaxPost) / sizeof (U32));
  MrcOemMemorySetDword (MinPost, 0xFFFFFFFF, sizeof (MinPost) / sizeof (U32));
  MrcOemMemorySetDword (Signal, 0, sizeof (Signal) / sizeof (U32));
  MrcOemMemorySetDword (Noise, 0, sizeof (Noise) / sizeof (U32));
  MrcOemMemorySetDword (Ratio, 0, sizeof (Ratio) / sizeof (U32));
  MrcOemMemorySetWord (PwrLimitPost, 0, sizeof (PwrLimitPost) / sizeof (U16));

  //
  // Initialize PostMar with zeroes
  //
  MrcOemMemorySet ((U8 *) PostMar, 0, sizeof (PostMar));

  calcResults = OptResByte;
  Margins     = (U16 *) inputMargins;
  MrcOemMemorySet ((U8 *) calcResults, 0, sizeof (OptResultsPerByte));
  //
  // Avoid division by zero.
  //
  if (AveN == 0) {
    AveN = 1;
  }
  Nby2    = (AveN >> 1);
  EqOrder = 0; // Is the optimization equation: X^1, X^2, X^5

  //
  // Process Raw Margins Results
  //
  for (xArr = 0; xArr < NumArr; xArr++) {
    //
    // Scale PwrLimit to match PostMar results
    //
    PwrLimitPost[xArr] = PwrLimit[xArr] * (AveN + ScaleM - 1) * Scale[xArr];

    for (x = 0; x < LenMargin; x++) {
      //
      // Calculate the Running Average Filter
      //
      if (Scale[xArr] == 0) {
        //
        // not in the game
        //
        MinPost[xArr] = PostMar[xArr][x] = 1;
      } else {
        if (x == 0) {
          //
          // update EqOrder once for each xArr value with a non-zero scale factor e.g.:so for {RdT,RdV,0,0} it will be =2
          //
          EqOrder += 1;
        }

        for (Off = 0; Off < AveN; Off++) {
          xEff = x + Off - Nby2;
          if (xEff < 0) {
            PostMar[xArr][x] += *(Margins + xArr * MarginsLength + 0);  // Margins[xArr][0];
          } else if (xEff >= LenMargin) {
            PostMar[xArr][x] += *(Margins + xArr * MarginsLength + LenMargin - 1);
          } else if (x == xEff) {
            PostMar[xArr][x] += ScaleM * *(Margins + xArr * MarginsLength + xEff);
          } else {
            PostMar[xArr][x] += *(Margins + xArr * MarginsLength + xEff);
          }
        }

        if (MaxPost[xArr] < PostMar[xArr][x]) {
          MaxPost[xArr] = PostMar[xArr][x];
        }

        if (MinPost[xArr] > PostMar[xArr][x]) {
          MinPost[xArr] = PostMar[xArr][x];
        }
        //
        // signal delta pre/post average filter
        //
        n = (PostMar[xArr][x] -*(Margins + xArr * MarginsLength + x) * (AveN + ScaleM - 1));
        Noise[xArr] += (n * n);
      }

      calcResults->Margins[xArr][x].EW = PostMar[xArr][x] / (AveN + ScaleM - 1);
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Margins[%d][%d] =%d\n",xArr,x,calcResults->Margins[xArr][x].EW);
      //
    }

    if (Scale[xArr] == 0) {
      continue;
    }
    //
    // Calculate SNR for this margin result
    // For stdev, need sqrt function.  Use log domain to change exponential to mult
    // Make both Signal and Noise a % of (Max+Min)/2
    // *100 for signal&noise = not go to zero
    //
    Signal[xArr] = ((MaxPost[xArr] - MinPost[xArr]) * 200 * 100) / (MaxPost[xArr] + MinPost[xArr]);
    Noise[xArr] /= LenMargin;
    if (Noise[xArr] != 0) {
      Noise[xArr] = Mrceexp (MrcNaturalLog (100 * Noise[xArr]) / 2);  // result is 100x
    }

    Noise[xArr] = (Noise[xArr] * 2 * 100) / (MaxPost[xArr] + MinPost[xArr]);

    if (Noise[xArr] == 0) {
      Ratio[xArr] = (Signal[xArr] * 1000);
    } else {
      Ratio[xArr] = (Signal[xArr] * 1000) / Noise[xArr];
    }

    SMaxPost[xArr] = MaxPost[xArr];

    //
    // update global results
    //
    calcResults->Scale[xArr]    = Scale[xArr];
    calcResults->Signal[xArr]   = Signal[xArr];
    calcResults->Noise[xArr]    = Noise[xArr];
    calcResults->Ratio[xArr]    = Ratio[xArr];
    calcResults->MaxPost[xArr]  = MaxPost[xArr] / (AveN + ScaleM - 1);
    calcResults->MinPost[xArr]  = MinPost[xArr] / (AveN + ScaleM - 1);
    //
    // 10x the tick diff.
    //
    calcResults->Ticks[xArr] = (U16) (MaxPost[xArr] - MinPost[xArr]) / (AveN + ScaleM - 1) / (Scale[xArr]);
  }
  //
  // Sort Array
  //
  MrcBsort (SMaxPost, NumArr);

  //
  // Calculate Number of Bits Required to represent this number. Make sure to handle care of EnSq
  //
  NumBits = 0;

  for (xArr = 0; xArr < NumArr; xArr++) {
    if (xArr < (NumArr - 1)) {
      //
      // if EnSq we do Max^2 so the num get twice the bits...
      //
      localY = SMaxPost[xArr];
      if (EnSq) {
        localY = (localY * localY);
      }

      NumBits += MrcLog2 ((U32) localY);
    } else {
      NumBits += MrcLog2 ((U32) SMaxPost[xArr]);
    }
  }

  NumBits += 11; // reserved another 10 bits for division in order to format for printing ; 3 for adding - up to 8
  //
  // EqOrder for square terms
  //
  if (EnSq) {
    EqOrder = (EqOrder + (EqOrder - 1));
  }
  //
  // Handle Potential Saturation
  //
  if (NumBits > 64) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Warning number of bits exceeds 64 bit : %d \n", NumBits);
    //
    // Shift all numbers to reduce final result to be less than 32 bits.  Round Up
    //
    Shift = (NumBits - 64 + EqOrder - 1) / EqOrder;
    //
    // RoundUp Adder
    //
    Adder = (1 << (Shift - 1));
    //
    // Divide by (1<<Shift) and Round Up
    //
    for (xArr = 0; xArr < NumArr; xArr++) {
      MaxPost[xArr]       = (MaxPost[xArr] + Adder) >> Shift;
      PwrLimitPost[xArr]  = (PwrLimitPost[xArr] + Adder) >> Shift;
      for (x = 0; x < LenMargin; x++) {
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,  "PostMar[%d][%d] before Shift : %d Adder : %d Shift : %d\n",xArr,x,PostMar[xArr][x],Shift,Adder);
        //
        PostMar[xArr][x] = (PostMar[xArr][x] + Adder) >> Shift;
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,  "after: %d\n",PostMar[xArr][x]);
        //
      }
    }
  }
  //
  // Calculate Square terms:
  //
  if (EnSq) {
    for (xArr = 0; xArr < NumArr; xArr++) {
      MaxPost[xArr] = MaxPost[xArr] * MaxPost[xArr];
    }
  }
  //
  // Set Limits for Search
  //
  Start = 0;
  Stop  = LenMargin;
  if ((IncEnds == 0) && (LenMargin > AveN)) {
    //
    // most commonly
    //
    if (Nby2 > 0) {
      Start++;
      Stop--;
    }
  }
  //
  // Find the Best Point to Use
  //
  Result      = 0;
  MaxR        = 0;
  MinR        = ~(0ULL);
  BestX       = 0;
  PowerX      = 0;
  FoundPwrOpt = 0;

  for (x = Start; x < Stop; x++) {
    //
    // Find Optimal Point from Margin Point of View
    // Combine the points using the formula:
    //   Max0*Max1*Max2*Post3 + Max1*Max2*Max3*Post0 + Max2*Max3*Max0*Post1 +
    //   Max3*Max0*Max1*Post2 + Scale*min(Post0,Post1,Post2,Post3)^EqOrder
    //   Scale = 1 + (10*(SMaxPost[0]-SMaxPost[1]))/SMaxPost[NumArr-1]
    //
    Result    = 0;
    MinPost1  = 0xFFFFFFFF;
    GoodPower = 1;
    for (xArr = 0; xArr < NumArr; xArr++) {
      if (Scale[xArr] == 0) {
        continue; // not need to calculate those
      }
      //
      // Find Min of all PostMar at offset x
      // Does this point meet the min power Margin requirements?
      //
      if (Scale[xArr] > 0) {
        if (MinPost1 > PostMar[xArr][x]) {
          MinPost1 = PostMar[xArr][x];
        }

        if (PostMar[xArr][x] < PwrLimitPost[xArr]) {
          GoodPower = 0; // not ! //@todo: delete this power limit for this routing
        }
      }
      //
      // Calculate this portion of result
      //
      rlocal = 1;
      for (yArr = 0; yArr < NumArr; yArr++) {
        if (Scale[yArr] == 0) {
          continue; // not need to calculate those
        }

        if (xArr == yArr) {
          continue;
        } else {
          rlocal = MrcOemMemoryMultiplyU64ByU32 (rlocal, MaxPost[yArr]);
        }
      }

      Result += MrcOemMemoryMultiplyU64ByU32 (rlocal, PostMar[xArr][x]);
    }

    NumCalcArr = 0;
    for (xArr = 0; xArr < NumArr; xArr++) {
      //
      // required for following step.
      //
      if (Scale[xArr] != 0) {
        NumCalcArr++;
      }
    }
    //
    // Add in (MinPost ^ EqOrder)
    // If NumCalcArr is 0, set it to 1 so that it still in the range of array size.
    //
    if (NumCalcArr == 0) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "Error: wrong input parameter caused NumCalcArr = 0 when calling FindOptimalTradeOff()\n"
        );
      NumCalcArr = 1;
    }

    ScaleMin = 1 + (10 * (SMaxPost[0] - SMaxPost[1])) / SMaxPost[NumCalcArr - 1];
    if (ScaleMin > 5) {
      ScaleMin = 5;
    }

    ScaleMin  = 1;
    rlocal    = ScaleMin;
    for (i = 0; i < EqOrder; i++) {
      rlocal = MrcOemMemoryMultiplyU64ByU32 (rlocal, MinPost1);
    }

    Result += rlocal;

    if (Result < MinR) {
      MinR = Result;
    }

    if (Result > MaxR) {
      MaxR  = Result;
      BestX = x; // save first highest function result offset
    }

    calcResults->Result[x] = Result;
    //
    // Find Optimal Point from Power Point of View
    //
    if (GoodPower) {
      //
      // are all the point meet margins requirements for all Tests ?
      //
      if (FoundPwrOpt == 0) {
        FoundPwrOpt = 1; // power optimization is possible
        PowerX = x; // first point passing to power limits
      } else {
        if ((PwrOptHigh == 1) && (x > PowerX)) {
          PowerX = x; // we take the less power save point
        }

        if ((PwrOptHigh == 0) && (x < PowerX)) {
          PowerX = x; // @todo: how can it be ? x is alwaye increasing
        }
      }
    }
  } // end shmoo offsets
  if ((MaxR + MinR) == 0) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "warninig : MaxR+MinR are Zero !!!\n");
  }
  //
  // Record for debug purposes.
  // more simple: 1000*(max-min)/((max+min)/2)
  //
  SNRTotal = MrcOemMemoryDivideU64ByU64 (MrcOemMemoryMultiplyU64ByU32 ((MaxR - MinR), 2000), (MaxR + MinR + 1));
  //
  // Are we optimizing for Power or Margin?
  //
  if (FoundPwrOpt) {
    if ((PwrOptHigh == 1) && (BestX < PowerX)) {
      BestX = PowerX;
    }
    //
    // if ((PwrOptHigh==0) && (BestX>PowerX)) BestX = PowerX;//give the more power saving offset that meet power limits
    //
  }

  calcResults->Best     = BestX;
  calcResults->SNRTotal = SNRTotal;
  calcResults->MaxR     = MaxR;
  calcResults->MinR     = MinR;
  //
  // Apply a guard band to the best setting, clamped at edges of the search.
  //
  if (GuardBand != 0) {
    //
    // Determine step direction and limit to the search edge.
    //
    if (GuardBand < 0) {
      StepSize = 1;
      Off = ((BestX + GuardBand) < Start) ? Start : (BestX + GuardBand);
    } else {
      StepSize = -1;
      Off = ((BestX + GuardBand) >= Stop) ? (Stop - 1) : (BestX + GuardBand);
    }
    //
    // Check each test for margin drop of MarginDropPercent.
    // If any test fails, we step towards the original selection.
    //
    MarginLimit = MrcOemMemoryMultiplyU64ByU32 (calcResults->Result[BestX], (100 - MarginDropPercent));
    MarginLimit = MrcOemMemoryDivideU64ByU64 (MarginLimit, 100);
    for(; (Off != BestX); Off += StepSize) {
      if (calcResults->Result[Off] > MarginLimit) {
        break;
      }
    }

    calcResults->GuardBand = Off - (S8) BestX;
  }

  return;
}

/**
  This function implements Turn Around Timing training.
  Optimize TA ODT Delay and Duration

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If it succeeds return mrcSuccess.
**/
MrcStatus
MrcTurnAroundTiming (
  IN MrcParameters *const MrcData
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcInput          *Inputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcStatus         Status;
  U8                Channel;
  U8                RankMaskCh;
  U8                RankMask;
  BOOL              RunDD;
  BOOL              RunDR;
  U8                ParamList[4]; // List of parameters to margin
  U8                TestListRd[2];
  U8                TestListWr[2];
  U8                GuardBand;
  U8                NomWR2RD;
  U8                Update;
  U8                LoopCount;
  S8                ClkShifts[2];
  U32               Offset;

  Status        = mrcSuccess;
  RankMaskCh    = 0;
  Update        = 1;
  LoopCount     = 12;
  RunDD         = FALSE;
  RunDR         = FALSE;
  NomWR2RD      = 0;
  RankMask      = 0xF;
  ParamList[0]  = RdV;
  ParamList[1]  = RdT;
  ParamList[2]  = WrV;
  ParamList[3]  = WrT;
  TestListRd[0] = RdV;
  TestListRd[1] = RdT;
  TestListWr[0] = WrV;
  TestListWr[1] = WrT;
  ClkShifts[0]  = -7;
  ClkShifts[1]  = 7;

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Inputs        = &MrcData->SysIn.Inputs;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut = &ControllerOut->Channel[Channel];
    if (!ChannelOut->ValidRankBitMask) {
      continue;
    }

    RankMaskCh  = ChannelOut->ValidRankBitMask;
    RunDD       = RunDD || (ChannelOut->DimmCount == 2);
    RunDR       = RunDR || ((RankMaskCh & 0xC) == 0xC) || ((RankMaskCh & 0x3) == 0x3);
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "Channel %d: RunDR = 0x%x, RunDD = 0x%x, RankMaskCh = 0x%x\n",
      Channel,
      RunDR,
      RunDD,
      RankMaskCh
      );

    //
    // Use nominal values (previuosly programmed) +1 an -1 to test.
    //
    NomWR2RD = (U8)
      (
        (ChannelOut->MchbarBANKRANKB & MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRRD_dr_MSK) >>
        MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRRD_dr_OFF
      );
  }
  //
  // Program SAFE values for ODT and SAmp
  //
  GuardBand = 1;
  UpdateSampOdtTiming (MrcData, GuardBand);

  //
  // Sweep ODT values but do not apply optimized value yet (Data Collection Only)
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n ##### Running mcodts\n");
  Status = TrainDDROptParamCliff (
            MrcData,
            mcodts,
            TestListRd,
            sizeof (TestListRd),
            0,
            2 + GuardBand,
            LoopCount,
            Update,
            Outputs->MarginResult,
            ClkShifts,
            sizeof (ClkShifts),
            0,
            RankMask,
            GuardBand
            );
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n ##### Running mcodtd\n");
  Status = TrainDDROptParamCliff (
            MrcData,
            mcodtd,
            TestListRd,
            sizeof (TestListRd),
            (-1 - GuardBand),
            0,
            LoopCount,
            Update,
            Outputs->MarginResult,
            ClkShifts,
            sizeof (ClkShifts),
            0,
            RankMask,
            GuardBand
            );

  //
  // Restore SAFE values when ONLY collecting data
  //
  if (Update == 0) {
    UpdateSampOdtTiming (MrcData, GuardBand);

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &ControllerOut->Channel[Channel];
      if (!ChannelOut->ValidRankBitMask) {
        continue;
      }
      Offset = MCHBAR_CH0_CR_TC_BANK_RANK_D_REG +
        ((MCHBAR_CH1_CR_TC_BANK_RANK_D_REG - MCHBAR_CH0_CR_TC_BANK_RANK_D_REG) * Channel);
      MrcWriteCR (MrcData, Offset, ChannelOut->MchbarBANKRANKD);
    }
  }
  //
  // Sweep DD Timing but do not apply optimized value yet (Data Collection Only)
  //
  if (RunDD) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n ##### Running DDRD2RD\n");
    Status = TrainDDROptParamCliff (
              MrcData,
              ddrd2rd,
              TestListRd,
              sizeof (TestListRd),
              6,
              7,
              LoopCount,
              Update,
              Outputs->MarginResult,
              ClkShifts,
              sizeof (ClkShifts),
              0,
              RankMask,
              0
              );
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n ##### Running DDWR2WR\n");
    Status = TrainDDROptParamCliff (
              MrcData,
              ddwr2wr,
              TestListWr,
              sizeof (TestListWr),
              7,
              8,
              LoopCount,
              Update,
              Outputs->MarginResult,
              ClkShifts,
              sizeof (ClkShifts),
              0,
              RankMask,
              0
              );
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n ##### Running DDWR2RD\n");
    Status = TrainDDROptParamCliff (
              MrcData,
              ddwr2rd,
              ParamList,
              sizeof (ParamList),
              NomWR2RD - 1,
              NomWR2RD + 1,
              LoopCount,
              Update,
              Outputs->MarginResult,
              ClkShifts,
              sizeof (ClkShifts),
              0,
              RankMask,
              0
              );
  }
  //
  // Sweep DR Timing but do not apply optimized value yet (Data Collection Only)
  //
  if (RunDR) {

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n ##### Running DRRD2RD\n");
    Status = TrainDDROptParamCliff (
              MrcData,
              drrd2rd,
              TestListRd,
              sizeof (TestListRd),
              6,
              7,
              LoopCount,
              Update,
              Outputs->MarginResult,
              ClkShifts,
              sizeof (ClkShifts),
              0,
              RankMask,
              0
              );
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n ##### Running DRWR2WR\n");
    Status = TrainDDROptParamCliff (
              MrcData,
              drwr2wr,
              TestListWr,
              sizeof (TestListWr),
              7,
              8,
              LoopCount,
              Update,
              Outputs->MarginResult,
              ClkShifts,
              sizeof (ClkShifts),
              0,
              RankMask,
              0
              );
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n ##### Running DRWR2RD\n");
    Status = TrainDDROptParamCliff (
              MrcData,
              drwr2rd,
              ParamList,
              sizeof (ParamList),
              NomWR2RD - 1,
              NomWR2RD + 1,
              LoopCount,
              Update,
              Outputs->MarginResult,
              ClkShifts,
              sizeof (ClkShifts),
              0,
              RankMask,
              0
              );
  }
  //
  // Restore SAFE values when ONLY collecting data
  //
  if (Update == 0) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &ControllerOut->Channel[Channel];
      if (ChannelOut->ValidRankBitMask) {
        Offset = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG +
          ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * Channel);
        MrcWriteCR (MrcData, Offset, ChannelOut->MchbarBANKRANKA);

        Offset = MCHBAR_CH0_CR_TC_BANK_RANK_B_REG +
          ((MCHBAR_CH1_CR_TC_BANK_RANK_B_REG - MCHBAR_CH0_CR_TC_BANK_RANK_B_REG) * Channel);
        MrcWriteCR (MrcData, Offset, ChannelOut->MchbarBANKRANKB);

      }
    }
  }

  return Status;
}

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
  )
{
  const MRC_REUTAddress REUTAddressConst = {
    {0, 0, 0, 0},       // Start
    {7, 0, 0, 1023},    // Stop
    {0, 0, 0, 0},       // Order
    {0, 0, 0, 0},       // IncRate
    {1, 0, 0, 1}};      // IncValue
  const U8          OptParamDDType[13] = {1, 2, 1, 2, 1, 2, 1, 2, 3, 3, 3, 3, 0}; // Does this test run dr, dd or both?
  const U8          RankMapping[16] = {15, 15, 15, 4, 15, 3, 15, 1, 15, 15, 15, 15, 5, 2, 15, 0};
    // Order of rank turnarounds for dr & dd.
  const U32         RankOrder[2][6] = {{0x32320101, 0x20101010, 0x23232320, 0x20, 0x10, 0x23}, // RankOrder[0]: drsd - same DIMM
                                       {0x21303120, 0x2120, 0x3020, 0x20, 0, 0}};              // RankOrder[1]: drdd - diff DIMM
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcStatus         Status;
  MRC_REUTAddress   REUTAddress;
  MRC_WDBPattern    WDBPattern;                   // For 8 bit VA, this walks through each WDB pointer ~ 2X
  BOOL              IsDual;
  BOOL              ODT;
  BOOL              PerByte;
  BOOL              NotRankTraining;
  BOOL              Lpddr;
  BOOL              FindFirstPass;
  U32               BERStats[4];                                  // Track BER results
  U32               RankList;
  U32               Offset;
  U32               CRValue;
  U16               Margins[4][2][2][MAX_CHANNEL][MAX_SDRAM_IN_DIMM];  // Tests X DR/DD x ClkPhases x Ch X Byte
  U16               NumCL;                                        // Number of cachelines per SubSeq
  U16               m;
  U16               SeqLC;
  U16               MinMarginLimit;
  U8                ShiftValue;
  U8                Channel;
  U8                ChannelMask;
  U8                Byte;
  U16               ByteMask;
  U8                Rank;
  U8                ChBitMask;
  U8                RankCount;
  U8                ChBitMaskdd;
  U8                RankMaskCh;
  U8                drddPresent[2];                               // [0]: ChBitMask for dr, [1]: ChBitMask for dd
  U8                CmdPat;
  U8                BMap[9];                                      // Needed for GetBERMarginByte function
  U8                MarginLimit;                                  // Need to change it to 20%of eye heigth
  U8                ResetDDR;
  U8                SelfRefresh;
  U8                RankInc;                                      // Increment every cacheline (HW adds +1 automatically)
  U16               ByteFailMask[MAX_CHANNEL];                    // Per ch mask indicating which bytes have failed
  U8                offs[MAX_CHANNEL];
  U8                Param;
  U8                iparam;
  U16               ByteDone;
  U8                dd;
  U8                test0;
  U8                v0;
  U8                Mode;
  U8                RankOrderIndex;
  U8                UpdateHostMargin;
  U8                Done;
  U8                MaxMargin;
  U8                ResultType;
  U8                WDBIncRate;                                   // Number of cachelines between incrementing WDB.
  U8                LoopEnd;
  S8                Inc;
  S8                Off;
  S8                Index;
  S8                LastPass[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];     // Lass Pass Value for off
  S8                Begin;
  S8                End;
  S8                ChLastPass;
  S8                ActualGuardBand;
#ifdef MRC_DEBUG_PRINT
  S8                ChLastPass1[MAX_CHANNEL];
#endif // MRC_DEBUG_PRINT
  MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_STRUCT  ReutChSeqRankL2PMapping;

  Status          = mrcSuccess;
  Done            = 0;
  Rank            = 0;
  drddPresent[0]  = 0;
  drddPresent[1]  = 0;
  MarginLimit     = (rtl == OptParam) ? 10 : 20;  // Drop of X% in margin means failure
  ResetDDR        = 1;
  SelfRefresh     = 0;
  WDBIncRate      = 13;
  NumCL           = 128;
  //
  // For {8,5,4,3,2} ranks, this covers each rank ~ {3,5,6,8,12}X
  // For 8 bit VA, this walks through each WDB pointer ~ 2X
  //
  WDBPattern.IncRate  = 0;
  WDBPattern.Start    = 0;
  WDBPattern.Stop     = 7;
  WDBPattern.DQPat    = 0;
  MrcOemMemorySetWord (ByteFailMask, 0, sizeof (ByteFailMask) / sizeof(ByteFailMask[0]));
  MrcOemMemorySet (offs, 0, sizeof (offs));
  MrcOemMemorySet ((U8 *) BERStats, 0, sizeof (BERStats));
  MrcOemMemoryCpy ((U8 *) &REUTAddress, (U8 *) &REUTAddressConst, sizeof (REUTAddress));
  for (Byte = 0; Byte < (sizeof (BMap) / sizeof (BMap[0])); Byte++) {
    BMap[Byte] = Byte;
  }

  Debug           = &MrcData->SysIn.Inputs.Debug;
  Outputs         = &MrcData->SysOut.Outputs;
  ControllerOut   = &Outputs->Controller[0];

  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  NotRankTraining = (OptParam == rtl);
  FindFirstPass   = (OptParam == rtl);  // FindFirstPass logic only works for RTL!
  ODT             = (OptParam == rdodtd) || (OptParam == wrodtd) || (OptParam == mcodtd) || (OptParam == mcodts);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "\nNotRankTraining = %u, ODT = %d\n", NotRankTraining, ODT);

  //
  // Decide which channels need to be run and program NumCachelines
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut = &ControllerOut->Channel[Channel];
    if (ChannelOut->ValidRankBitMask) {
      ChannelMask = MRC_BIT0 << Channel;
      RankMaskCh  = ChannelOut->ValidRankBitMask;
      IsDual      = ((RankMaskCh & 0xC) == 0xC) || ((RankMaskCh & 0x3) == 0x3);

      //
      // Continue if no ranks in this channel
      //
      if ((RankMaskCh & RankMask) == 0) {
        continue;
      }

      if ((OptParamDDType[OptParam] & 0x2) && (ChannelOut->DimmCount == 2)) {
        drddPresent[1] |= ChannelMask; // dd parameter and channel has 2 DIMMs
      }

      if (((OptParamDDType[OptParam] & 0x1) && IsDual) || NotRankTraining) {
        drddPresent[0] |= ChannelMask; // dr parameter and channel has a dual rank
      }

      if (ODT && ((drddPresent[0] & (1 << Channel)) == 0)) {
        //
        // ODT matters when Single rank
        // dr parameter and channel has a dual rank
        //
        drddPresent[0] |= ChannelMask;
      }
    }
  }

  ChBitMask = drddPresent[1] | drddPresent[0]; // Chanel is present if it has either a dr or dd
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "drddPresent[0] = 0x%x, drddPresent[1] = 0x%x, ChBitMask = 0x%x\n",
    drddPresent[0],
    drddPresent[1],
    ChBitMask
    );

  //
  // There is nothing to optimize for this parameter
  //
  if ((ChBitMask == 0) || (Stop <= Start)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ChBitMask = %d, Start = 0x%x, Stop = 0x%x\n", ChBitMask, Start, Stop);

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "No need to optimized TA, OptParam = %d\n", OptParam);
    return mrcFail;
  }
  //
  // Setup the REUT Test
  //
  SeqLC           = LoopCount;
  RankInc         = 0;
  Outputs->DQPat  = TurnAround;
  if ((OptParam == ddwr2rd) || (OptParam == drwr2rd)) {
    CmdPat          = PatWrRdTA;
    Outputs->DQPat  = TurnAroundWR;
    RankInc         = 1;
  } else if (ODT) {
    CmdPat          = PatODTTA;
    Outputs->DQPat  = TurnAroundODT;
    RankInc         = 1;
  } else if (OptParam == rtl) {
    CmdPat = PatWrRd;
    //
    // Less optimistic values since we are updating values and RMT fails
    //
    WDBIncRate  = 16;
    NumCL       = 4;
  } else {
    CmdPat = PatWrRd;
  }

  WDBPattern.DQPat        = Outputs->DQPat;
  WDBPattern.IncRate      = WDBIncRate;
  REUTAddress.IncRate[0]  = RankInc;
  REUTAddress.IncRate[3]  = RankInc;

  //
  // SOE=0, EnCADB=0, EnCKE=0, SubSeqWait=0
  //
  SetupIOTest (MrcData, ChBitMask, CmdPat, NumCL, (U8) SeqLC, &REUTAddress, NSOE, &WDBPattern, 0, 0, 0);

  Outputs->DQPatLC = MRC_BIT0 << (LoopCount - MrcLog2 ((U32) (NumCL - 1)));
  if (Outputs->DQPatLC < 1) {
    Outputs->DQPatLC = 1;
  }
  //
  // Optimize parameter per byte.  Everything else is per channel
  //
  PerByte = (OptParam == mcodts) || (OptParam == mcodtd);

  //
  // Keep track of which bytes have failed and are we done yet
  //
  ByteDone = (1 << Outputs->SdramCount) - 1;

  //
  // ###########################################################
  // ####  Loop through OptParam X DD X ClkPhases X Params and measure margin #####
  // ###########################################################
  //
  if (OptParam == mcodts) {
    //
    // In the case of mcodts, higher values are actually worst.
    //
    Begin = Start;
    End   = Stop;
    Inc   = 1;
  } else {
    Begin = Stop;
    End   = Start;
    Inc   = -1;
  }

  ActualGuardBand = (Inc * GuardBand);

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Start = %d, Stop = %d, Begin = %d, End = %d, Inc = %d\n",
    Start,
    Stop,
    Begin,
    End,
    Inc
    );
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (OptParam == rtl) ? "Rank = %d\n" : "", rank);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel 0\t\t\t\t\t\t\t\t1\nByte\t");
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE, (
    Outputs->SdramCount == MAX_SDRAM_IN_DIMM
    ) ? "0\t1\t2\t3\t4\t\t5\t6\t7\t8\t0\t1\t2\t3\t4\t5\t6\t7\t8\n" :
    "0\t1\t2\t3\t4\t5\t6\t7\t0\t1\t2\t3\t4\t5\t6\t7\n"
    );

  //
  // Init Variables
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        LastPass[Channel][Byte] = Begin - ActualGuardBand;
        for (iparam = 0; iparam < NumTests; iparam++) {
          for (dd = 0; dd < 2; dd++) {
            for (test0 = 0; test0 < NumR2RPhases; test0++) {
              Margins[iparam][dd][test0][Channel][Byte] = 1280;
            }
          }
        }
      }
    }
  }
  //
  // Walk through different OptParam values
  //
  for (Off = (S8) Begin; Off != (S8) (End + Inc); Off += Inc) {
    if (Done) {
      break;
    }
    Index = (Off - Begin) * Inc; // Index = 0, 1, 2..
    //
    // Inc can only take a value of +/- 1.
    //
    if ((Index == 1) && (TRUE == FindFirstPass)) {
      Inc  *= -1;
      Off   = End;
      End   = Begin - Inc;  // One Inc less since we have already done Index 0.
      Begin = Off - Inc;    // One Inc less to get us starting at Index 1
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "Find First Pass - Walking backwards.\n Off = %d, Begin = %d, End = %d, Inc = %d, Index = %d\n",
        Off,
        Begin,
        End,
        Inc,
        (Off - Begin) * Inc
        );
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Param^ Offset-> %d\n   Actl\t", Off);

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut  = &ControllerOut->Channel[Channel];
      RankMaskCh  = ChannelOut->ValidRankBitMask & RankMask;
      //
      // if nothing for this channel OR No Ranks in this channel
      //
      if (!((MRC_BIT0 << Channel) & ChBitMask) || (RankMaskCh == 0)) {
#ifdef MRC_DEBUG_PRINT
        if (Channel == 0) {
          if (Outputs->SdramCount == (MAX_SDRAM_IN_DIMM - 1)) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t");
          } else {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t\t");
          }
        }
#endif // MRC_DEBUG_PRINT
        continue;
      }
      //
      // For debug purposes program Row start stop to OptParam + Offset value
      // OptParam in upper BYTE
      //
      Offset = 4 + MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG +
        (
          (MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG) *
          Channel
        );
      CRValue = MrcReadCR (MrcData, Offset);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "upper SEQ_BASE_ADDR_START BEFORE = 0x%x ", CRValue);
      CRValue = MrcBitSwap (CRValue, OptParam, 0, 8);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "After = 0x%x\n", CRValue);
      MrcWriteCR (MrcData, Offset, CRValue);
      //
      // Offset in Lower BYTE
      //
      Offset -= 4;
      CRValue = MrcReadCR (MrcData, Offset);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "lower SEQ_BASE_ADDR_START BEFORE = 0x%x ", CRValue);
      CRValue = MrcBitSwap (CRValue, Off, 24, 8);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "After = 0x%x\n", CRValue);
      MrcWriteCR (MrcData, Offset, CRValue);

      //
      // OptParam in upper BYTE
      //
      Offset = 4 + MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG +
        (
          (MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG) *
          Channel
        );
      CRValue = MrcReadCR (MrcData, Offset);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "upper SEQ_BASE_ADDR_WRAP BEFORE = 0x%x ", CRValue);
      CRValue = MrcBitSwap (CRValue, OptParam, 0, 8);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "After = 0x%x\n", CRValue);
      MrcWriteCR (MrcData, Offset, CRValue);
      //
      // Offset in Lower BYTE
      //
      Offset -= 4;
      CRValue = MrcReadCR (MrcData, Offset);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "lower SEQ_BASE_ADDR_WRAP BEFORE = 0x%x ", CRValue);
      CRValue = MrcBitSwap (CRValue, Off, 24, 8);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "After = 0x%x\n", CRValue);
      MrcWriteCR (MrcData, Offset, CRValue);

      //
      // No need to update MrcData host during this step even if not collecting data
      //
      LoopEnd = (U8) ((PerByte) ? Outputs->SdramCount : 1);
      for (Byte = 0; Byte < LoopEnd; Byte++) {
        UpdateTAParamOffset (MrcData, Channel, Byte, OptParam, Off, 0, 0, RankMaskCh);
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

    //
    // Test both: different dr and dd as required
    //
    for (dd = 0; dd < 2; dd++) {
      if (Done) {
        break;
      }
      //
      // Check if this test type should be run
      //
      ChBitMaskdd = drddPresent[dd];
      if (ChBitMaskdd == 0) {
        continue;
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (dd == 0) ? "Dual Rank\n" : "Dual Dimm\n");
      //
      // Select Ranks in the correct order based on the test type
      // Need to re-order the ranks based on the value of ddw2r
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(ChBitMaskdd & (MRC_BIT0 << Channel))) {
          continue;
        }

        ChannelOut  = &ControllerOut->Channel[Channel];
        RankMaskCh  = ChannelOut->ValidRankBitMask & RankMask;
        if (RankMaskCh == 0) {
          continue; // No Ranks in this channel
        }
        //
        // Initialize variables and read out ordered rank list
        //
        ReutChSeqRankL2PMapping.Data  = 0;
        RankCount                     = 0;

        if (NotRankTraining) {
          RankList = 0x00003210;
        } else {
          RankOrderIndex = RankMapping[RankMaskCh];
          if (RankOrderIndex == 15) {
            RankList = 0x00003210;
          } else {
            RankList = RankOrder[dd][RankOrderIndex];
          }
        }

        while (RankList > 0) {
          Rank = (RankList & 0xF); // Nibble by Nibble
          RankList = (RankList >> 4);
          if (!(RankMaskCh & (MRC_BIT0 << Rank))) {
            continue;
          }

          ShiftValue = RankCount *
            MRC_BIT0 <<
            MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_Logical_to_Physical_Rank0_Mapping_WID;
          ReutChSeqRankL2PMapping.Data |= (Rank << ShiftValue);
          RankCount++;
        }

        Offset = MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG +
          (
            (
              MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_1_REG -
              MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG
            ) * Channel
          );
        MrcWriteCR (MrcData, Offset, ReutChSeqRankL2PMapping.Data);
        Offset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG +
          (
            (MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG) *
            Channel
          ) + 7;
        MrcWriteCR8 (MrcData, Offset, RankCount - 1);
      }
      //
      // ###################################################
      // ### Walk through different sets of rank2rank timings  ###
      // ###################################################
      //
      for (test0 = 0; test0 < NumR2RPhases; test0++) {
        if (Done) {
          break;
        }

        v0 = ClkShifts[test0];

        //
        // Program rank offsets differently for dd vs. dr
        //
        if (NotRankTraining) {
          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            if (!(ChBitMaskdd & (MRC_BIT0 << Channel))) {
              offs[Channel] = 0;
            } else {
              //
              // Shift all signals in the channel(Clk/Ctl/Cmd/Dq) by v0
              //
              offs[Channel] = v0;
            }
          }
          //
          // UpdateHost=0, SkipTx=0
          //
          ShiftCh2Ch (MrcData, RankMask, offs, ResetDDR, SelfRefresh, 0, 0);
        } else if (dd == 1) {
          //
          // For DD
          // Shift Clk/DQ on one DIMM by v0 and Clk/DQ on other DIMM by -v0
          // @todo: CTL picode should be optionally shifted to improve margins
          //
          SetCmdMargin (MrcData, ChBitMaskdd, 0x3, WrT, v0, 0, ResetDDR, SelfRefresh);
          SetCmdMargin (MrcData, ChBitMaskdd, 0xC, WrT, -v0, 0, ResetDDR, SelfRefresh);
        } else {
          //
          // For DR
          // Shift Clk/DQ on front side by v0 and Clk/DQ on backside by -v0
          // @todo: CTL picode should be optionally shifted to improve margins
          //
          SetCmdMargin (MrcData, ChBitMaskdd, 0x5, WrT, v0, 0, ResetDDR, SelfRefresh);
          SetCmdMargin (MrcData, ChBitMaskdd, 0xA, WrT, -v0, 0, ResetDDR, SelfRefresh);
        }
        //
        // Test different margin param
        //
        for (iparam = 0; iparam < NumTests; iparam++) {
          Param = TestList[iparam];
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s ", MarginTypesString[Param]);
          if (Param == 0) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, " WARNING! UNNECESSARY LOOPS. Param = %d \n", Param);
            return mrcFail;
          }

          ResultType = GetMarginResultType (Param);

          //
          // Get the width/height limit for the parameter
          //
          MinMarginLimit = UpmPwrLimitValue (MrcData, Param, UpmLimit);
          //
          // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d ", MinMarginLimit);
          // Calculate MaxMargin and Starting Point for margin search
          //
          MaxMargin = MAX_POSSIBLE_TIME;
          if ((Param == RdV) ||
              (Param == RdFan2) ||
              (Param == RdFan3) ||
              (Param == WrV) ||
              (Param == WrFan2) ||
              (Param == WrFan3)
              ) {
            MaxMargin = MAX_POSSIBLE_VREF;
          }
          //
          // Are we done yet or should we keep testing?
          //
          Done = 1;
          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            if (!(ChBitMaskdd & (MRC_BIT0 << Channel))) {
              continue;
            }

            ChannelOut  = &ControllerOut->Channel[Channel];
            RankMaskCh  = ChannelOut->ValidRankBitMask & RankMask;
            if (RankMaskCh == 0) {
              continue; // No Ranks in this channel
            }

            //
            // When FindFirstPass is used, all Bytes have to have passed before we stop.
            // We uses ByteFailMask[] to track the passing bytes in this case.
            //
            if (PerByte || FindFirstPass) {
              if (ByteFailMask[Channel] != ByteDone) {
                Done = 0;
              }
            } else {
              if (ByteFailMask[Channel] == 0) {
                Done = 0;
              }
            }
          }

          if (Done) {
            break;
          }

          Status = GetMarginByte (MrcData, Outputs->MarginResult, Param, 0, 0xF);
          if (Status != mrcSuccess) {
            return Status;
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "% 3d\t", (S8) v0);

          Mode = 0;
          Status = MrcGetBERMarginByte (
                    MrcData,
                    Outputs->MarginResult,
                    ChBitMaskdd,
                    rank,
                    0xFF,
                    Param,
                    Mode,
                    BMap,
                    1,
                    MaxMargin,
                    0,
                    BERStats
                    );
          if (Status != mrcSuccess) {
            return Status;
          }
          //
          // Record Results
          //
          UpdateHostMargin = 1;

          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            ChannelOut  = &ControllerOut->Channel[Channel];
            RankMaskCh  = ChannelOut->ValidRankBitMask & RankMask;
            //
            // if nothing for this channel OR No Ranks in this channel
            //
            if (!((MRC_BIT0 << Channel) & ChBitMaskdd) || (RankMaskCh == 0)) {
#ifdef MRC_DEBUG_PRINT
              if (Channel == 0) {
                if (Outputs->SdramCount == (MAX_SDRAM_IN_DIMM - 1)) {
                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t");
                } else {
                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t\t");
                }
              }
#endif // MRC_DEBUG_PRINT
              continue;
            }

            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              //
              // For this optimization, it makes more sense to look at the full sum
              //
              ByteMask = MRC_BIT0 << Byte;
              m = EffectiveMargin (
                    (U16) MarginByte[ResultType][rank][Channel][Byte][0],
                    (U16) MarginByte[ResultType][rank][Channel][Byte][1]
                    );
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d", m);

              //
              // If previously failed, this is also a failure unless we are looking for
              // the first passing offset.
              //
              if ((ByteFailMask[Channel] & ByteMask) && (FALSE == FindFirstPass)) {
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "#\t");
                continue;
              }
              //
              // Byte fails if margin is below MinMarginLimit at any time
              //
              if (m < MinMarginLimit) {
                //
                // If we are looking for pass, continue and do not update LastPass
                //
                if (TRUE == FindFirstPass) {
                  if (Index == 0) {
                    //
                    // When training from the most aggressive setting to the conservative setting,
                    // if we fail the first setting we stop.
                    //
                    ByteFailMask[Channel] = ByteDone;
                  }
                  UpdateHostMargin = 0;
                } else {
                  ByteFailMask[Channel] |= ByteMask;
                  LastPass[Channel][Byte] = Off - Inc - ActualGuardBand;
                }
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "#\t");
                continue;
              }

              if (Index == 0) {
                //
                // Get the smallest marging at Index 0
                //
                if (Margins[iparam][dd][test0][Channel][Byte] > m) {
                  Margins[iparam][dd][test0][Channel][Byte] = m;
                }
              } else {
                //
                // Check if we dropped more than the percent allowed
                //
                if (m < ((Margins[iparam][dd][test0][Channel][Byte] * (100 - MarginLimit)) / 100)) {
                  if (FALSE == FindFirstPass) {
                    ByteFailMask[Channel] |= ByteMask;
                    LastPass[Channel][Byte] = Off - Inc - ActualGuardBand;
                  }
                  UpdateHostMargin = 0;
                  MRC_DEBUG_MSG (
                    Debug,
                    MSG_LEVEL_NOTE,
                    "#-%d\t",
                    (ABS (m - Margins[iparam][dd][test0][Channel][Byte]) * 100) / Margins[iparam][dd][test0][Channel][Byte]
                    );
                  continue;
                } else {
                  if (TRUE == FindFirstPass) {
                    if ((ByteFailMask[Channel] & ByteMask) != ByteMask) {
                      LastPass[Channel][Byte] = Off - ActualGuardBand;
                      ByteFailMask[Channel] |= ByteMask;
                    }
                  } else {
                    LastPass[Channel][Byte] = Off - ActualGuardBand;
                  }
                }
              }

              MRC_DEBUG_MSG (
                Debug,
                MSG_LEVEL_NOTE,
                ".%c%d\t",
                (m > Margins[iparam][dd][test0][Channel][Byte]) ? '+' : '-',
                (ABS(m - Margins[iparam][dd][test0][Channel][Byte]) * 100) / Margins[iparam][dd][test0][Channel][Byte]
                );
            }
          }

          if (UpdateHostMargin) {
            Status = ScaleMarginByte (MrcData, Outputs->MarginResult, Param, rank);
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

        //
        // Clean up
        //
        if (NotRankTraining) {
          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            offs[Channel] = 0;
          }
          //
          // UpdateHost=0, SkipTx=0
          //
          ShiftCh2Ch (MrcData, RankMask, offs, ResetDDR, SelfRefresh, 0, 0);
        } else {
          SetCmdMargin (MrcData, ChBitMaskdd, RankMask, WrT, 0, 0, ResetDDR, SelfRefresh);
        }
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
    }
  }

  //
  // If we are sweeping agressive settings to conservative settings, we
  // need to restore original Inc, Begin, and End values to select the 
  // proper offset if bytes have varying offsets values for a parameter
  // that is NOT specified per Byte.
  //
  if (TRUE == FindFirstPass) {
    Off   = End;         // Temp storage for swap
    End   = Begin + Inc;
    Begin = Off + Inc;
    Inc  *= -1;
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "Find First Pass - Reverting Inc, Begin, and End\n Begin = %d, End = %d, Inc = %d,\n",
      Begin,
      End,
      Inc
      );
  }

#ifdef MRC_DEBUG_PRINT
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Optimal offset per Byte\n\t");
  //
  // Print OPTIMAL value
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut            = &ControllerOut->Channel[Channel];
    RankMaskCh            = ChannelOut->ValidRankBitMask & RankMask;
    ChLastPass1[Channel]  = End;
    //
    // if nothing for this channel OR No Ranks in this channel
    //
    if (!((MRC_BIT0 << Channel) & ChBitMask) || (RankMaskCh == 0)) {
      if (Channel == 0) {
        if (Outputs->SdramCount == (MAX_SDRAM_IN_DIMM - 1)) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t");
        } else {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t\t");
        }
      }

      continue;
    }

    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", LastPass[Channel][Byte]);
      if ((Inc == 1) && (ChLastPass1[Channel] > LastPass[Channel][Byte])) {
        ChLastPass1[Channel] = LastPass[Channel][Byte];
      }

      if ((Inc == -1) && (ChLastPass1[Channel] < LastPass[Channel][Byte])) {
        ChLastPass1[Channel] = LastPass[Channel][Byte];
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut  = &ControllerOut->Channel[Channel];
    RankMaskCh  = ChannelOut->ValidRankBitMask & RankMask;
    //
    // if nothing for this channel OR No Ranks in this channel
    //
    if (!((MRC_BIT0 << Channel) & ChBitMask) || (RankMaskCh == 0)) {
      continue;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Optimal offset Channel %d = %d\n", Channel, ChLastPass1[Channel]);
  }
#endif // MRC_DEBUG_PRINT
  //
  // Program new value
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!((MRC_BIT0 << Channel) & ChBitMask)) {
      continue;
    }

    ChannelOut  = &ControllerOut->Channel[Channel];
    RankMaskCh  = ChannelOut->ValidRankBitMask & RankMask;
    if (RankMaskCh == 0) {
      continue; // No Ranks in this channel
    }
    //
    // Start with the most aggressive setting
    //
    ChLastPass = End;
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      if (Update == 0) {
        LastPass[Channel][Byte] = Begin;
      }

      if ((Inc == 1) && (ChLastPass > LastPass[Channel][Byte])) {
        ChLastPass = LastPass[Channel][Byte];
      }

      if ((Inc == -1) && (ChLastPass < LastPass[Channel][Byte])) {
        ChLastPass = LastPass[Channel][Byte];
      }

      if (PerByte) {
        UpdateTAParamOffset (MrcData, Channel, Byte, OptParam, LastPass[Channel][Byte], Update, 1, RankMaskCh);
      }
    }

    if (PerByte == 0) {
      UpdateTAParamOffset (MrcData, Channel, 0, OptParam, ChLastPass, Update, 1, RankMaskCh);
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Selected Offset for channel %d is = %d\n", Channel, ChLastPass);
  }

  return Status;
}

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
  )
{
  MrcControllerOut  *ControllerOut;
  U8                Channel;
  U8                RankMaskCh;
  U8                Offset;

  ControllerOut = &MrcData->SysOut.Outputs.Controller[0];
  Offset        = 0;
  if (SelfRefresh && ResetDDR) {
    MRC_DEBUG_MSG (
      &MrcData->SysIn.Inputs.Debug,
      MSG_LEVEL_ERROR,
      "WARNING SelfRefresh OR ResetDDR can be set at once...performing SelfRefresh\n"
      );
    ResetDDR = 0;
  }

  if (SelfRefresh) {
    EnterSR (MrcData);
  }
  //
  // Change Clock Timing
  //
  if ((Param == WrT) || (Param == WrTBox)) {
    //
    // Walk though all chs and ranks
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (ChBitMask & (MRC_BIT0 << Channel)) {
        //
        // determine which ranks from parameter "Ranks" exist in this channel
        //
        RankMaskCh = Ranks & ControllerOut->Channel[Channel].ValidRankBitMask;
        if (RankMaskCh == 0) {
          continue; // No Ranks in this channel
        }

        ShiftPIforCmdTraining (MrcData, Channel, MrcIterationClock, RankMaskCh, 3, Value0, 0);
      }
    }
  }

  if ((Param == WrV) || (Param == (WrTBox))) {
    if (Param == WrV) {
      Offset = Value0;
    } else {
      if (Param == WrTBox) {
        Offset = ((2 * Value1) - 1) * 8;
      }
    }

    UpdateVrefWaitTilStable (MrcData, 2, 0, Offset, 0);
  }

  if (ResetDDR) {
    MrcResetSequence (MrcData);
  } else if (SelfRefresh) {
    ExitSR (MrcData);
  }

  return;
}

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
  )
{
  const MrcDebug                          *Debug;
  MrcOutput                               *Outputs;
  MrcChannelOut                           *ChannelOut;
  U8                                      Rank;
  U8                                      IOLat;
  S8                                      New;
  U32                                     Offset1;
  U32                                     Offset2;
  MCHBAR_CH0_CR_TC_BANK_RANK_A_STRUCT     TcBankRankA;
  MCHBAR_CH0_CR_TC_BANK_RANK_B_STRUCT     TcBankRankB;
  MCHBAR_CH0_CR_TC_BANK_RANK_C_STRUCT     TcBankRankC;
  MCHBAR_CH0_CR_TC_BANK_RANK_D_STRUCT     TcBankRankD;
  DDRDATA0CH0_CR_DDRCRDATACONTROL1_STRUCT DdrDataControl1;
  MCHBAR_CH0_CR_SC_IO_LATENCY_STRUCT      ScIoLatency;
  MCHBAR_CH0_CR_SC_ROUNDT_LAT_STRUCT      ScRoundtLat;

  Debug       = &MrcData->SysIn.Inputs.Debug;
  Outputs     = &MrcData->SysOut.Outputs;
  ChannelOut  = &Outputs->Controller[0].Channel[Channel];

  switch (OptParam) {
  case drrd2rd:
    //
    // dr RD 2 RD Turn Around offsets
    //
    TcBankRankA.Data          = ChannelOut->MchbarBANKRANKA;
    TcBankRankA.Bits.tRDRD_dr = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG + ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankA.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKA = TcBankRankA.Data;
    }
    break;

  case ddrd2rd:
    //
    // dd RD 2 RD Turn Around offsets
    //
    TcBankRankA.Data          = ChannelOut->MchbarBANKRANKA;
    TcBankRankA.Bits.tRDRD_dd = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankA.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKA = TcBankRankA.Data;
    }
    break;

  case drwr2wr:
    //
    // dr WR 2 WR Turn Around offsets
    //
    TcBankRankB.Data          = ChannelOut->MchbarBANKRANKB;
    TcBankRankB.Bits.tWRWR_dr = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_B_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_B_REG - MCHBAR_CH0_CR_TC_BANK_RANK_B_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankB.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKB = TcBankRankB.Data;
    }
    break;

  case ddwr2wr:
    //
    // dd WR 2 WR Turn Around offsets
    //
    TcBankRankB.Data          = ChannelOut->MchbarBANKRANKB;
    TcBankRankB.Bits.tWRWR_dd = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_B_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_B_REG - MCHBAR_CH0_CR_TC_BANK_RANK_B_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankB.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKB = TcBankRankB.Data;
    }
    break;

  case drrd2wr:
    //
    // dr RD 2 WR Turn Around offsets
    //
    TcBankRankC.Data          = ChannelOut->MchbarBANKRANKC;
    TcBankRankC.Bits.tRDWR_dr = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_C_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_C_REG - MCHBAR_CH0_CR_TC_BANK_RANK_C_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankC.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKC = TcBankRankC.Data;
    }
    break;

  case ddrd2wr:
    //
    // dd RD 2 WR Turn Around offsets
    //
    TcBankRankC.Data          = ChannelOut->MchbarBANKRANKC;
    TcBankRankC.Bits.tRDWR_dd = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_C_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_C_REG - MCHBAR_CH0_CR_TC_BANK_RANK_C_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankC.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKC = TcBankRankC.Data;
    }
    break;

  case drwr2rd:
    //
    // dr WR 2 RD Turn Around offsets
    //
    TcBankRankB.Data          = ChannelOut->MchbarBANKRANKB;
    TcBankRankB.Bits.tWRRD_dr = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_B_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_B_REG - MCHBAR_CH0_CR_TC_BANK_RANK_B_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankB.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKB = TcBankRankB.Data;
    }
    break;

  case ddwr2rd:
    //
    // dd WR 2 RD Turn Around offsets
    //
    TcBankRankB.Data          = ChannelOut->MchbarBANKRANKB;
    TcBankRankB.Bits.tWRRD_dd = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_B_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_B_REG - MCHBAR_CH0_CR_TC_BANK_RANK_B_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankB.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKB = TcBankRankB.Data;
    }
    break;

  case rdodtd:
    TcBankRankD.Data                    = ChannelOut->MchbarBANKRANKD;
    TcBankRankD.Bits.Odt_Read_Duration  = Off - 6; // Convert into Register values. 2'b00 = BL/2 + 2 (6 DCLKs
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_D_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_D_REG - MCHBAR_CH0_CR_TC_BANK_RANK_D_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankD.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKD = TcBankRankD.Data;
    }
    break;

  case wrodtd:
    TcBankRankD.Data = ChannelOut->MchbarBANKRANKD;
#ifdef ULT_FLAG
    if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
      TcBankRankD.UltBits.Odt_Write_Duration = Off - 6; // Convert into Register values. 2'b00 = BL/2 + 2 (6 DCLKs
    } else
#endif // ULT_FLAG
    {
      TcBankRankD.Bits.Odt_Write_Duration = Off - 6; // Convert into Register values. 2'b00 = BL/2 + 2 (6 DCLKs
    }
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_D_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_D_REG - MCHBAR_CH0_CR_TC_BANK_RANK_D_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankD.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKD = TcBankRankD.Data;
    }
    break;

  case mcodts:
    //
    // MC ODT delay
    //
    DdrDataControl1.Data  = ChannelOut->DqControl1[Byte].Data;
    New                   = MrcSE ((U8) DdrDataControl1.Bits.OdtDelay, 4, 8) + Off; // SignExtend
    if (New < -4) {
      New = -4; // RcvEnPi[8:6] - 5 qclk Min
    } else if (New > 6) {
      New = 6; // RcvEnPi[8:6] + 5 qclk Max
    }

    DdrDataControl1.Bits.OdtDelay       = New;
    DdrDataControl1.Bits.SenseAmpDelay  = New;
    Offset1 = DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG +
      ((DDRDATA0CH1_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Channel) +
      ((DDRDATA1CH0_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Byte);
    MrcWriteCR (MrcData, Offset1, DdrDataControl1.Data);
    if (UpdateHost) {
      ChannelOut->DqControl1[Byte].Data = DdrDataControl1.Data;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (SkipPrint) ? "" : "%d\t", New);
    break;

  case mcodtd:
    //
    // Duration
    //
    DdrDataControl1.Data  = ChannelOut->DqControl1[Byte].Data;
    New                   = (U8) DdrDataControl1.Bits.OdtDuration + Off;
    if (New < 0) {
      New = 0; // 11 tQCK Min
    } else if (New > DDRDATA0CH0_CR_DDRCRDATACONTROL1_OdtDuration_MAX) {
      New = DDRDATA0CH0_CR_DDRCRDATACONTROL1_OdtDuration_MAX; // 18 tQCK Max
    }

    DdrDataControl1.Bits.OdtDuration      = New;
    DdrDataControl1.Bits.SenseAmpDuration = New;
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "mcodtd CRValue = 0x%x\n", DdrDataControl1.Bits.OdtDuration);
    //
    Offset1 = DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG +
      ((DDRDATA0CH1_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Channel) +
      ((DDRDATA1CH0_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Byte);
    MrcWriteCR (MrcData, Offset1, DdrDataControl1.Data);
    if (UpdateHost) {
      ChannelOut->DqControl1[Byte].Data = DdrDataControl1.Data;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (SkipPrint) ? "" : "%d\t", DdrDataControl1.Bits.OdtDuration);
    break;

  case rtl:
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if (RankMask & (MRC_BIT0 << Rank)) {
        //
        // Update IO Latency & RoundTrip
        //
        IOLat = ChannelOut->IoLatency[Rank] - (ChannelOut->RTLatency[Rank] - Off);
        if ((S8) IOLat < 0) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "IOLatency reached the Saturation point \n");
        } else {
          Offset1 = MCHBAR_CH0_CR_SC_IO_LATENCY_REG +
            ((MCHBAR_CH1_CR_SC_IO_LATENCY_REG - MCHBAR_CH0_CR_SC_IO_LATENCY_REG) * Channel);
          ScIoLatency.Data = MrcReadCR (MrcData, Offset1);
          Offset2 = MCHBAR_CH0_CR_SC_ROUNDT_LAT_REG +
            ((MCHBAR_CH1_CR_SC_ROUNDT_LAT_REG - MCHBAR_CH0_CR_SC_ROUNDT_LAT_REG) * Channel);
          ScRoundtLat.Data = MrcReadCR (MrcData, Offset2);
          switch (Rank) {
          case 0:
            ScIoLatency.Bits.IOLAT_R0D0 = IOLat;
            ScRoundtLat.Bits.Lat_R0D0   = Off;
            break;

          case 1:
            ScIoLatency.Bits.IOLAT_R1D0 = IOLat;
            ScRoundtLat.Bits.Lat_R1D0   = Off;
            break;

          case 2:
            ScIoLatency.Bits.IOLAT_R0D1 = IOLat;
            ScRoundtLat.Bits.Lat_R0D1   = Off;
            break;

          case 3:
            ScIoLatency.Bits.IOLAT_R1D1 = IOLat;
            ScRoundtLat.Bits.Lat_R1D1   = Off;
            break;

          default:
            break;
          }

          MrcWriteCR (MrcData, Offset1, ScIoLatency.Data);
          MrcWriteCR (MrcData, Offset2, ScRoundtLat.Data);

          //
          // Update host
          //
          if (UpdateHost) {
            ChannelOut->RTLatency[Rank] = Off;
            ChannelOut->IoLatency[Rank] = IOLat;
          }
        }
      }
    }
    break;

  case srrd2rd:
    //
    // sr RD 2 RD Turn Around offsets
    //
    TcBankRankA.Data          = ChannelOut->MchbarBANKRANKA;
    TcBankRankA.Bits.tRDRD    = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG + ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankA.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKA = TcBankRankA.Data;
    }
    break;

  case srrd2wr:
    //
    // sr RD 2 WR Turn Around offsets
    //
    TcBankRankC.Data          = ChannelOut->MchbarBANKRANKC;
    TcBankRankC.Bits.tRDWR    = Off;
    Offset1 = MCHBAR_CH0_CR_TC_BANK_RANK_C_REG +
      ((MCHBAR_CH1_CR_TC_BANK_RANK_C_REG - MCHBAR_CH0_CR_TC_BANK_RANK_C_REG) * Channel);
    MrcWriteCR (MrcData, Offset1, TcBankRankC.Data);
    if (UpdateHost) {
      ChannelOut->MchbarBANKRANKC = TcBankRankC.Data;
    }
    break;

  default:
    break;
  }

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    ((OptParam != mcodtd) && (OptParam != mcodts) && (!SkipPrint)) ? "%d\t" : "",
    Off
    );

  return;
}

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
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  DimmOptPoint      *PointResults;
  OptOffsetChByte   BestOffArr[SizeOfTCompOffset][MAX_RANK_IN_CHANNEL];
  TCompOffset       OffsetType;
  U8                dimm;
  U8                rank;
  U8                ValidRankMask;
  U8                LocalRanks[MAX_CHANNEL];
  U8                ChBitMask;
  U8                Channel;
  U8                ParamList[] = { RdV, RdT, WrV, WrT }; // List of parameters to margin
  U8                TestListRd[]  = { RdV, RdT };
  U8                TestListWr[]  = { WrV, WrT };
  U8                *TestList;
  U8                TestListSize;
  U8                TScale[]      = { 1, 2, 1, 0, 0 };
  U8                GScale[]      = { 1, 2, 0, 0, 0 };
  U16               GPwrLimits[]  = { 520, 280, 0, 0, 0 };
  U16               noPwrLimits[] = { 2480, 2240, 0, 0, 0 };
  U8                *Scale;
  U16               *PwrLimits;
  S8                start;
  S8                stop;
  U8                i;
  U8                t;
  U8                ResultType;
  U8                RecenterLC;
  U8                OptParamLC;
  BOOL              clipPowerLmt;
  U16               *BestMargin;
  U8                TestResultType[4] = { 0, 0, 0, 0 };

  TestListSize                        = 0;
  RecenterLC                          = 15;
  OptParamLC                          = OPT_PARAM_LOOP_COUNT;
  clipPowerLmt                        = 1;
  Outputs       = &MrcData->SysOut.Outputs;
  Debug         = &MrcData->SysIn.Inputs.Debug;
  ControllerOut = &Outputs->Controller[0];

  PointResults  = DimmOptPoints;
  BestMargin    = (U16 *) inputBestMargin;
  MrcOemMemorySet ((U8 *) BestOffArr, 0xffff, sizeof (BestOffArr));
  MrcOemMemorySet ((U8 *) PointResults, 0xffff, sizeof (DimmOptPoint));
  OffsetType = 0;

  if (SubPwrLimits) {
    //
    // Use power limits and Trendline
    //
    Scale     = TScale;
    PwrLimits = GPwrLimits;
  } else {
    //
    //  No power limits and no TrendLine
    //
    Scale     = GScale;
    PwrLimits = noPwrLimits;
  }
  //
  // TrainDDROptParam already check the valid against host chRankBit mask
  // Walk through channels, check if this point is redundant, set RttNom
  //
  ChMask &= Outputs->ValidChBitMask;
  RankMask &= Outputs->ValidRankMask;
  ValidRankMask = 0;
  ChBitMask     = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    LocalRanks[Channel] = 0;
    if (((MRC_BIT0 << Channel) & ChMask)) {
      LocalRanks[Channel] = RankMask & ControllerOut->Channel[Channel].ValidRankBitMask;
      if (LocalRanks[Channel]) {
        ChBitMask |= MRC_BIT0 << Channel; // remove ch with no "active" ranks
      }
    }
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    for (dimm = 0; dimm < MAX_DIMMS_IN_CHANNEL; dimm++) {
      PointResults->ODTSet.RttNom[Channel][dimm]  = RttNom[Channel][dimm];
      PointResults->ODTSet.RttWr[Channel][dimm]   = RttWr[Channel][dimm];
    }
  }

  PointResults->ODTSet.GRdOdt = GRdOdt;
  UpdateOdtsValues (MrcData, ChBitMask, PointResults, skipGRdOdt, 0, 1, 1);
  //
  // update only DimmOdt and GROdt if not skipped.
  // Recenter Timing
  //
  if (RdCenter) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Read Vref\n");
    ReadVoltageCentering2D (
      MrcData,
      Outputs->MarginResult,
      ChBitMask,
      RdV,
      0,
      0,
      RecenterLC,
      0
      );
    //
    // We can add if status fail go to next point
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Read Timing ChBitMask=%x\n", ChBitMask);
    DataTimeCentering2D (
      MrcData,
      Outputs->MarginResult, // prev. margin results
      ChBitMask,
      RdT,
      0, // EnPerBit,
      0, // EnRxDutyCycle
      0, // ResetPerBit
      RecenterLC,
      0  // En2D
      );
  }

  if (WrCenter) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Write Vref\n");
    MrcWriteVoltageCentering2D (MrcData);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Re-center Write Timing ChBitMask=%x\n", ChBitMask);
    DataTimeCentering2D (
      MrcData,
      Outputs->MarginResult, // prev. margin results
      ChBitMask,
      WrT,
      0, // EnPerBit,
      0, // EnRxDutyCycle
      0, // ResetPerBit
      RecenterLC,
      0  // En2D
      );
  }
  //
  // @todo: we could check here if we have some reasonable amount of margin to play with
  //
  TestList  = ParamList;
  PointResults->OptParamTestListSize = OptParamTestListSize;
  for (t = 0; t < OptParamTestListSize; t++) {
    //
    // also apply the best offset to hw and host and inside also best offset related margin is saved in host struct
    //
    PointResults->OptParamTestList[t] = OptParamTestList[t];
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "OptParamTestList[%d]=%d , %s\n",t,OptParamTestList[t],TOptParamOffsetString[OptParamTestList[t]]);
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "OptParamTestList[%d]=%d OptParamTestListSize=%d\n",t,OptParamTestList[t],OptParamTestListSize);
    switch (OptParamTestList[t]) {
    case (OptWrDS):
      start = -11;
      stop  = 12;
      TrainDDROptParam (
        MrcData,
        &BestOffArr[WrDSOfft][0],
        ChBitMask,
        RankMask,
        OptWrDS,
        TestListWr,
        sizeof (TestListWr),
        Scale,
        PwrLimits,
        start,
        stop,
        OptParamLC,
        1,  // Repeats
        skipOptPrint,
        skipOptTests,
        0,  // RdRd2Test
        1   // GuardBand
        );
      TestList = TestListWr;
      TestListSize = sizeof (TestListWr);
      OffsetType = WrDSOfft;
      PointResults->BestOptOff[WrDSOfft][0] = BestOffArr[WrDSOfft][0];
      break;

    case (OptRdOdt):
      start = -10;
      stop  = 6;
      TrainDDROptParam (
        MrcData,
        &BestOffArr[RdOdtOfft][0],
        ChBitMask,
        RankMask,
        OptRdOdt,
        TestListRd,
        sizeof (TestListRd),
        Scale,
        PwrLimits,
        start,
        stop,
        OptParamLC,
        1,            // Repeats
        skipOptPrint,
        skipOptTests,
        RdRdTA,       // RdRd2Test
        0             // GuardBand
        );
      TestList      = TestListRd;
      TestListSize  = sizeof (TestListRd);
      OffsetType    = RdOdtOfft;
      PointResults->BestOptOff[RdOdtOfft][0] = BestOffArr[RdOdtOfft][0];
      break;

    case (OptSComp):
    case (OptTComp):
      break;

    case (OptTxEq):
      start = 0;
      stop  = 11;
      for (rank = 0; rank < MAX_RANK_IN_CHANNEL; rank++) {
        if (!((MRC_BIT0 << rank) & RankMask)) {
          continue; // check if rank at least on one channel
        }

        TrainDDROptParam (
          MrcData,
          &BestOffArr[TxEqOfft][rank],
          ChBitMask,
          (MRC_BIT0 << rank),
          OptTxEq,
          TestListWr,
          sizeof (TestListWr),
          Scale,
          PwrLimits,
          start,
          stop,
          OptParamLC,
          1,            // Repeats
          skipOptPrint,
          skipOptTests,
          0,            // RdRd2Test
          2             // GuardBand
          );
        PointResults->BestOptOff[TxEqOfft][rank] = BestOffArr[TxEqOfft][rank];
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (!((MRC_BIT0 << rank) & LocalRanks[Channel])) {
            continue;
            //
            // check if the rank exist in this ch
            //
          }

          for (i = 0; i < sizeof (TestListWr); i++) {
            //
            // track min margin per ch
            //
            if (BestOffArr[TxEqOfft][rank].Margins[i][Channel] < BestOffArr[TxEqOfft][0].Margins[i][Channel]) {
              BestOffArr[TxEqOfft][0].Margins[i][Channel] = BestOffArr[TxEqOfft][rank].Margins[i][Channel];
            }
          }
        }
      }

      TestList      = TestListWr;
      TestListSize  = sizeof (TestListWr);
      OffsetType    = TxEqOfft;
      break;

    case (OptRxEq):
      start = 0;
      stop  = 14;
      for (rank = 0; rank < MAX_RANK_IN_CHANNEL; rank++) {
        if (!((MRC_BIT0 << rank) & RankMask)) {
          continue; // check if rank at least on one channel
        }

        TrainDDROptParam (
          MrcData,
          &BestOffArr[RxEqOfft][rank],
          ChBitMask,
          (MRC_BIT0 << rank),
          OptRxEq,
          TestListRd,
          sizeof (TestListRd),
          Scale,
          noPwrLimits,
          start,
          stop,
          OptParamLC,
          1,       // Repeats
          skipOptPrint,
          skipOptTests,
          RdRdTA,  // RdRd2Test
          0        // GuardBand
          );
        PointResults->BestOptOff[RxEqOfft][rank] = BestOffArr[RxEqOfft][rank];
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (!((1 << rank) & LocalRanks[Channel])) {
            continue; // check if the rank exist in this ch
          }

          for (i = 0; i < sizeof (TestListRd); i++) {
            //
            // track min margin per ch and asign to rank0
            //
            if (BestOffArr[RxEqOfft][rank].Margins[i][Channel] < BestOffArr[RxEqOfft][0].Margins[i][Channel]) {
              BestOffArr[RxEqOfft][0].Margins[i][Channel] = BestOffArr[RxEqOfft][rank].Margins[i][Channel];
            }
          }
        }
      }

      TestList      = TestListRd;
      TestListSize  = sizeof (TestListRd);
      OffsetType    = RxEqOfft;
      break;

    case (OptRxBias):
      start = 0;
      stop  = 7;
      TrainDDROptParam (
        MrcData,
        &BestOffArr[RdSAmpOfft][0],
        ChBitMask,
        RankMask,
        OptRxBias,
        TestListRd,
        sizeof (TestListRd),
        Scale,
        PwrLimits,
        start,
        stop,
        OptParamLC,
        1,       // Repeats
        skipOptPrint,
        skipOptTests,
        RdRdTA,  // RdRd2Test
        0        // GuardBand
        );
      TestList      = TestListRd;
      TestListSize  = sizeof (TestListRd);
      OffsetType    = RdSAmpOfft;
      PointResults->BestOptOff[RdSAmpOfft][0] = BestOffArr[RdSAmpOfft][0];
      break;

    case (OptDimmOdt):
      break;

    case (OptDimmOdtWr):
      break;

    default:
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "OptParam Test not valid\n");

    }

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(LocalRanks[Channel])) {
        continue; // check if the active rank run this ch
      }
      //
      // run through all BestOff[optParam][0] and track min[RdV,RdT,WrV,WrT]
      //
      for (i = 0; i < TestListSize; i++) {
        ResultType                  = GetMarginResultType (TestList[i]);
        TestResultType[ResultType]  = TestList[i]; // indicate which test we run and create the reverse dic
        //
        //we need to update only last results
        //
        PointResults->Test[ResultType][Channel] = BestOffArr[OffsetType][0].Margins[i][Channel];
      }
    }
  } // end for OptParamTest
  //
  // assign the point for passing to the FindOptimalTradeOff function
  //
  i                       = 0;
  PointResults->NumTests  = 0;
  for (t = 0; t < 4; t++) {
    //
    // ResultType=GetMarginResultType(TestList[i]);
    //
    if (TestResultType[t] == 0) {
      continue; // can only be 1,2,4,5
    } else {
      PointResults->TestList[i] = TestResultType[t];
      PointResults->NumTests++;
      //
      // *(BestMargin+i*MarginsLength+OffsetPoint)=PointResults->Test[t][Channel];
      // sorting test for TradeOff
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(LocalRanks[Channel])) {
          continue; // check if the active rank run this ch
        }

        if (clipPowerLmt) {
          if (PointResults->Test[t][Channel] > UpmPwrLimitValue (MrcData, TestResultType[t], PowerLimit)) {
            PointResults->Points2Trade[i][Channel] = UpmPwrLimitValue (MrcData, TestResultType[t], PowerLimit);
          } else {
            PointResults->Points2Trade[i][Channel] = PointResults->Test[t][Channel];
          }
        } else {
          PointResults->Points2Trade[i][Channel] = PointResults->Test[t][Channel];
        }
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "PointResults->TestList[%d]=%d PointResults->Test[test index=%d][channel=%d] =%d\n",i,PointResults->TestList[i],t,Channel,PointResults->Test[t][Channel]);
        //
      }

      i++;
    }
  }
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "PointResults->NumTests =%d\n",PointResults->NumTests);
  //
  return;
}

/**
  This function applies an offset to the global compensation logic.
  Reruns Compensation and returns the new comp value

  @param[in,out] MrcData         - Include all MRC global data.
  @param[in]     param           - Parameter defining the desired global compensation logic
  @param[in]     offset          - Value to apply
  @param[in]     UpdateHost      - Desides if MrcData has to be updated

  @retval Returns the new comp value.
**/
U32
UpdateCompGlobalOffset (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             param,
  IN     const U32            offset,
  IN     const U8             UpdateHost
  )
{
  const MrcDebug                          *Debug;
  MrcOutput                               *Outputs;
  MrcControllerOut                        *ControllerOut;
  MrcChannelOut                           *ChannelOut;
  DDRCOMP_CR_DDRCRCOMPCTL0_STRUCT         DdrCrCompCtl0;
  DDRCOMP_CR_DDRCRCOMPCTL1_STRUCT         DdrCrCompCtl1;
  PCU_CR_M_COMP_PCU_STRUCT                PcuCrMComp;
  DDRCOMP_CR_DDRCRDATACOMP0_STRUCT        DdrCrDataComp0;
  DDRCOMP_CR_DDRCRDATACOMP1_STRUCT        DdrCrDataComp1;
  DDRCOMP_CR_DDRCRCMDCOMP_STRUCT          DdrCrCmdComp;
  DDRCOMP_CR_DDRCRCTLCOMP_STRUCT          DdrCrCtlComp;
  DDRCOMP_CR_DDRCRCLKCOMP_STRUCT          DdrCrClkComp;
  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT  DdrCrDataControl0;
  DDRCOMP_CR_DDRCRCOMPCTL0_STRUCT         DdrCrCompCtl0_Temp;
  U32                                     RegOffset;
  U8                                      Channel;

  Outputs                 = &MrcData->SysOut.Outputs;
  ControllerOut           = &Outputs->Controller[0];
  Debug                   = &MrcData->SysIn.Inputs.Debug;
  DdrCrCompCtl0.Data      = Outputs->CompCtl0;
  DdrCrCompCtl1.Data      = Outputs->CompCtl1;
  DdrCrDataControl0.Data  = 0;

  //
  // Update offset in local CR variable
  //
  switch (param) {
  case RdOdt:
    //
    // Disable FixOdt feature before changing this param
    //
    DdrCrCompCtl0.Bits.FixOdtD = 0;
    //
    // Apply Comp Offset to RdOdt
    //
    DdrCrCompCtl0.Bits.DqOdtVref = offset;
    break;

  case WrDS:
    //
    // Apply Comp Offset to WrDS-DQ
    //
    DdrCrCompCtl0.Bits.DqDrvVref = offset;
    break;

  case WrDSCmd:
    //
    // Apply Comp Offset to WrDS-CMD
    //
    DdrCrCompCtl0.Bits.CmdDrvVref = offset;
    break;

  case WrDSCtl:
    //
    // Apply Comp Offset to WrDS-CTL
    //
    DdrCrCompCtl0.Bits.CtlDrvVref = offset;
    break;

  case WrDSClk:
    //
    // Apply Comp Offset to WrDS-CLK
    //
    DdrCrCompCtl0.Bits.ClkDrvVref = offset;
    break;

  case SCompDq:
    //
    // Apply Comp Offset to Scomp-DQ
    //
    DdrCrCompCtl1.Bits.DqScompCells = offset;
    DdrCrCompCtl1.Bits.DqScompPC    = offset >> 4;
    break;

  case SCompCmd:
    //
    // Apply Comp Offset to Scomp-CMD
    //
    DdrCrCompCtl1.Bits.CmdScompCells  = offset;
    DdrCrCompCtl1.Bits.CmdScompPC     = offset >> 4;
    break;

  case SCompCtl:
    //
    // Apply Comp Offset to Scomp-CTL
    //
    DdrCrCompCtl1.Bits.CtlScompCells  = offset;
    DdrCrCompCtl1.Bits.CtlScompPC     = offset >> 4;
    break;

  case SCompClk:
    //
    // Apply Comp Offset to Scomp-CLK
    //
    DdrCrCompCtl1.Bits.ClkScompCells  = offset;
    DdrCrCompCtl1.Bits.ClkScompPC     = offset >> 4;
    break;

  case DisOdtStatic:
    //
    // disable static read Otd legs
    //
    DdrCrCompCtl0.Bits.DisableOdtStatic = offset;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &ControllerOut->Channel[Channel];
      DdrCrDataControl0.Data = ChannelOut->DqControl0.Data;
      DdrCrDataControl0.Bits.DisableOdtStatic = offset; // apply to bytes fubs
      RegOffset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
        ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      MrcWriteCrMulticast (MrcData, RegOffset, DdrCrDataControl0.Data);
      if (UpdateHost) {
        ChannelOut->DqControl0.Data = DdrCrDataControl0.Data;
      }
    }
    break;

  default:
    break;
  }
  //
  // Update the Comp Offsets and Host Structure
  //
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPCTL0_REG, DdrCrCompCtl0.Data);
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPCTL1_REG, DdrCrCompCtl1.Data);
  if (UpdateHost) {
    Outputs->CompCtl0 = DdrCrCompCtl0.Data;
    Outputs->CompCtl1 = DdrCrCompCtl1.Data;
  }
  //
  // Run Compensation
  // Start Comp Engine
  //
  PcuCrMComp.Data               = 0;
  PcuCrMComp.Bits.COMP_FORCE    = 1;
  PcuCrMComp.Bits.COMP_INTERVAL = MIN (COMP_INT, PCU_CR_M_COMP_PCU_COMP_INTERVAL_MAX);
  PcuCrMComp.Bits.COMP_DISABLE  = 1;
  MrcWriteCR (MrcData, PCU_CR_M_COMP_PCU_REG, PcuCrMComp.Data);
  MrcWait (MrcData, 8 * HPET_1US); // Wait for Comp to Complete
  if (param == RdOdt) {
    //
    // we check if we close to saturation and try dis/en the static legs
    //
    DdrCrDataComp1.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG);
    if ((DdrCrDataComp1.Bits.RcompOdtUp < 16) || (DdrCrDataComp1.Bits.RcompOdtUp > 48)) {
      //
      // disable/enable  static read Otd legs
      //
      if (DdrCrDataComp1.Bits.RcompOdtUp < 16) {
        DdrCrCompCtl0.Bits.DisableOdtStatic = 1;
      } else {
        DdrCrCompCtl0.Bits.DisableOdtStatic = 0;
      }
      //
      // Update the Comp Offsets and Host Structure
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut  = &ControllerOut->Channel[Channel];
        ChannelOut->DqControl0.Bits.DisableOdtStatic = DdrCrCompCtl0.Bits.DisableOdtStatic; // apply to bytes fubs
        RegOffset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
          ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "DdrCrDataControl0.Bits.DisableOdtStatic=%d\n",DdrCrDataControl0.Bits.DisableOdtStatic);
        //
        MrcWriteCrMulticast (MrcData, RegOffset, ChannelOut->DqControl0.Data);
      }

      MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPCTL0_REG, DdrCrCompCtl0.Data);
      //
      // host need to always be updated with static state
      //
      DdrCrCompCtl0_Temp.Data                  = Outputs->CompCtl0;
      DdrCrCompCtl0_Temp.Bits.DisableOdtStatic = DdrCrCompCtl0.Bits.DisableOdtStatic;
      Outputs->CompCtl0                        = DdrCrCompCtl0_Temp.Data;
      //
      // Run Compensation
      // Start Comp Engine
      //
      MrcWriteCR (MrcData, PCU_CR_M_COMP_PCU_REG, PcuCrMComp.Data);
      MrcWait (MrcData, 8 * HPET_1US); // Wait for Comp to Complete
    }

  }
  //
  // Return the new comp code
  //
  switch (param) {
  case DisOdtStatic:
  case RdOdt:
    DdrCrDataComp1.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG);
    //
    // re-Enable FixOdt feature after changing this param
    //
    DdrCrCompCtl0.Bits.DqOdtUpDnOff = DdrCrDataComp1.Bits.RcompOdtDown - DdrCrDataComp1.Bits.RcompOdtUp;
    DdrCrCompCtl0.Bits.FixOdtD      = 1;
    MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPCTL0_REG, DdrCrCompCtl0.Data);
    if (UpdateHost) {
      Outputs->CompCtl0 = DdrCrCompCtl0.Data;
    }
    return DdrCrDataComp1.Bits.RcompOdtUp;

  case WrDS:
  case SCompDq:
    DdrCrDataComp0.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP0_REG);
    return (param == WrDS) ? DdrCrDataComp0.Bits.RcompDrvUp : DdrCrDataComp0.Bits.SlewRateComp;

  case WrDSCmd:
  case SCompCmd:
    DdrCrCmdComp.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRCMDCOMP_REG);
    return (param == WrDSCmd) ? DdrCrCmdComp.Bits.RcompDrvUp : DdrCrCmdComp.Bits.Scomp;

  case WrDSCtl:
  case SCompCtl:
    DdrCrCtlComp.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRCTLCOMP_REG);
    return (param == WrDSCtl) ? DdrCrCtlComp.Bits.RcompDrvUp : DdrCrCtlComp.Bits.Scomp;

  case WrDSClk:
  case SCompClk:
    DdrCrClkComp.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRCLKCOMP_REG);
    return (param == WrDSClk) ? DdrCrClkComp.Bits.RcompDrvUp : DdrCrClkComp.Bits.Scomp;

  default:
    break;
  }

  return 0;
}

/**
  Programs Delay/Duration for the SenseAmp and MCODT based on RcvEn timing
  Provide GuardBand > 0 if needed to be more conservative in timing
  Main goal is to optimize power

  @param[in,out] MrcData   - Include all MRC global data.
  @param[in]     GuardBand - Input parameter with more conservative value

  @retval Nothing
**/
void
UpdateSampOdtTiming (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             GuardBand
  )

{
  MrcOutput                               *Outputs;
  MrcDebug                                *Debug;
  MrcControllerOut                        *ControllerOut;
  MrcChannelOut                           *ChannelOut;
  U16                                     *CurRcvEn;
  U8                                      Channel;
  U8                                      Byte;
  U8                                      rank;
  U16                                     MaxRcvEn;
  U16                                     MinRcvEn;
  U32                                     Offset;
  U32                                     SWakeUp;
  U32                                     SAWakeUppS;  // Round up to nearest Qclk
  S8                                      SOn;         // SenseAmpDelay
  S8                                      OOn;         // OdtDelay
  S32                                     SOff;        // SenseAmpDuration
  S32                                     OOff;        // OdtDuration
  DDRDATA0CH0_CR_DDRCRDATACONTROL1_STRUCT *DqControl1;

  SAWakeUppS    = 1250;
  Outputs       = &MrcData->SysOut.Outputs;
  Debug         = &MrcData->SysIn.Inputs.Debug;
  ControllerOut = &Outputs->Controller[0];

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "UpdateSampOdtTiming: GuardBand = %d\n", GuardBand);
  
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Ch %d\tOdtOn\tOdtOff\tSAmpOn\tSAmpOff\n", Channel);

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        MaxRcvEn  = 0;
        MinRcvEn  = 512;

        for (rank = 0; rank < MAX_RANK_IN_CHANNEL; rank++) {
          if (MrcRankInChannelExist (MrcData, rank, Channel)) {
            CurRcvEn = &ChannelOut->RcvEn[rank][Byte];
            if (MaxRcvEn < *CurRcvEn) {
              MaxRcvEn = *CurRcvEn;
            }

            if (MinRcvEn > *CurRcvEn) {
              MinRcvEn = *CurRcvEn;
            }
          }
        }
        //
        // Round Max to nearest cycle
        //
        MaxRcvEn = (MaxRcvEn >> 6) + 1;

        //
        // SENSE AMP CAN ONLY BE ON WHEN ODT IS ON FOR EOS REASONS.
        // SWakeUp = (U32)( (SAWakeUppS + Outputs->Qclkps - 1) / Outputs->Qclkps );
        // SOn = MinRcvEn - SWakeUp - GuardBand;
        // OOn = MinRcvEn - 2 - GuardBand;
        //
        SWakeUp = (U32) ((64 * SAWakeUppS) / Outputs->Qclkps); // Convert to PI codes
        //
        // Turn On ODT & Samp at least 2 Qclks before earlier RcvEn Rise
        //
        if (SWakeUp < 128) {
          SWakeUp = 128; // at least 2-Qclks
        }

        OOn = SOn = (S8) ((MinRcvEn - SWakeUp) >> 6) - GuardBand;
        //
        // SenseAmp Delay
        //
        if (SOn < -4) {
          SOn = -4; // RcvEnPi[8:6] - 5 qclk
        } else if (SOn > 6) {
          SOn = 6; // RcvEnPi[8:6] + 5 qclk
        }
        //
        // OdtDelay
        //
        if (OOn < -4) {
          OOn = -4; // RcvEnPi[8:6] - 5 qclk
        } else if (OOn > 6) {
          OOn = 6; // RcvEnPi[8:6] + 5 qclk
        }
        //
        // Turn Off Samp 1 qclk after postamble
        // Turn Off ODT 1 qclk after postamble
        // Program the duration to leave Odt/Samp On
        //      OnBeforeRcvEn   BL+Post  AfterPost   CR Encoding
        //
        SOff  = (MaxRcvEn - SOn) + (8 + 1) + 1 + GuardBand - 11;
        OOff  = (MaxRcvEn - OOn) + (8 + 1) + 1 + GuardBand - 11;

        if (SOff < 0) {
          SOff = 0; // 11 tQCK Min
        } else if (SOff > 7) {
          SOff = 7; // 18 tQCK Max
        }

        if (OOff < 0) {
          OOff = 0;  // 11 tQCK Min
        } else if (OOff > 7) {
          OOff = 7;  // 18 tQCK mAx
        }

        DqControl1 = &ChannelOut->DqControl1[Byte];
        DqControl1->Bits.OdtDelay         = OOn;
        DqControl1->Bits.OdtDuration      = OOff;
        DqControl1->Bits.SenseAmpDelay    = SOn;
        DqControl1->Bits.SenseAmpDuration = SOff;
        Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG +
          ((DDRDATA1CH0_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Byte) +
          ((DDRDATA0CH1_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Channel);
        MrcWriteCR (MrcData, Offset, DqControl1->Data);
        
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d:\t%d\t%d\t%d\t%d\n",
          Byte,
          DqControl1->Bits.OdtDelay,
          DqControl1->Bits.OdtDuration,
          DqControl1->Bits.SenseAmpDelay,
          DqControl1->Bits.SenseAmpDuration
        );
      }
    }
  }

  return;
}

/**
  Turns off unused portions of the slave DLL to save power

  @param[in,out] MrcData - Include all MRC global data.

  @retval Nothing
**/
void
UpdateSlaveDLLLength (
  IN OUT MrcParameters *const MrcData
  )
{
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  U8                *CurRxDqs;
  U32               Offset;
  U8                Channel;
  U8                byte;
  U8                rank;
  U8                MaxPi;

  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        MaxPi = 0;
        for (rank = 0; rank < MAX_RANK_IN_CHANNEL; rank++) {
          if (MrcRankInChannelExist (MrcData, rank, Channel)) {
            CurRxDqs = &ChannelOut->RxDqsP[rank][byte];
            if (MaxPi < *CurRxDqs) {
              MaxPi = *CurRxDqs;
            }

            CurRxDqs = &ChannelOut->RxDqsN[rank][byte];
            if (MaxPi < *CurRxDqs) {
              MaxPi = *CurRxDqs;
            }
          }
        }
        //
        // Update SlaveDLL Length for power Savings
        // Calculate which segments to turn off:
        // NEW (OFF: 0, PI<48: 0x2, PI<32: 0x4, PI<16: 0x6)
        // results are:   0, 2 , 4 or 6
        //
        ChannelOut->DqControl1[byte].Bits.SdllSegmentDisable = ((7 - (MaxPi >> 3)) &~MRC_BIT0);
        Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG +
          ((DDRDATA1CH0_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * byte) +
          ((DDRDATA0CH1_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Channel);
        MrcWriteCR (MrcData, Offset, ChannelOut->DqControl1[byte].Data);
      }
    }
  }

  return;
}

#ifdef TRAD_FLAG
/**
  Update Internal clocks on setting if needed.

  @param[in,out] MrcData - Include all MRC global data.

  @retval Nothing
**/
void
UpdateInternalClksOn (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  U32               Offset;
  U8                Channel;
  U8                Byte;
  S8                SOn;  // SenseAmpDelay
  S8                OOn;  // OdtDelay
  S32               SOff; // SenseAmpDuration
  S32               OOff; // OdtDuration
  U8                InternalClkOn;

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      InternalClkOn = 0;
      ChannelOut    = &ControllerOut->Channel[Channel];
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        OOn   = MrcSE ((U8) ChannelOut->DqControl1[Byte].Bits.OdtDelay, 4, 8);
        OOff  = ChannelOut->DqControl1[Byte].Bits.OdtDuration;
        SOn   = MrcSE ((U8) ChannelOut->DqControl1[Byte].Bits.SenseAmpDelay, 4, 8);
        SOff  = ChannelOut->DqControl1[Byte].Bits.SenseAmpDuration;

        //
        // Check if OdtDelay + OdtDuration >= 7 or if SADelay + SADuration >= 7
        //
        if (((OOn + OOff) >= 7) || ((SOn + SOff) >= 7)) {
          InternalClkOn = 1;
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ODTOn = %d, ODTOff = %d\n", OOn, OOff);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "SAOn  = %d, SAOff  = %d\n", SOn, SOff);
          break;
        }
      }

      ChannelOut->DqControl0.Bits.InternalClocksOn = InternalClkOn;
      Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
        ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0.Data);
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "C%dDdrCrDataControl0.Data  = 0x%x\n",
        Channel,
        ChannelOut->DqControl0.Data
        );
    }
  }

  return;
}
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
MrcStatus
ShiftCh2Ch (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Ranks,
  IN     const U8 *const      offset,
  IN     U8                   ResetDDR,
  IN     const U8             SelfRefresh,
  IN     const U8             UpdateHost,
  IN     const U8             SkipTx
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcStatus         Status;
  U8                Channel;
  U8                Rank;
  U8                RankMaskCh;
  S32               NewValue;
  S32               Offset;
  BOOL              Lpddr;

  Status        = mrcSuccess;
  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Lpddr         = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  if (SelfRefresh && ResetDDR) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_ERROR,
      "WARNING SelfRefresh OR ResetDDR can be set at once...performing SelfRefresh\n"
      );
    ResetDDR = 0;
  }

  if (SelfRefresh) {
    EnterSR (MrcData);
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }

    ChannelOut = &ControllerOut->Channel[Channel];
    RankMaskCh = Ranks & ChannelOut->ValidRankBitMask;

    if (RankMaskCh == 0) {
      continue;
    }

    Offset = offset[Channel];

    //
    // Shift CLK (this will shift DQ PIs as well)
    //
    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationClock, RankMaskCh, 3, Offset, UpdateHost);

    //
    // Shift CTL
    //
    NewValue = 0;
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if (RankMaskCh & (1 << Rank)) {
        NewValue = ChannelOut->CtlPiCode[Rank] + Offset;
        break;
      }
    }

    ShiftPIforCmdTraining (MrcData, Channel, MrcIterationCtl, RankMaskCh, 1, NewValue, UpdateHost);

    //
    // Shift CmdS
    //
    ShiftPIforCmdTraining (
      MrcData,
      Channel,
      MrcIterationCmdS,
      RankMaskCh,
      1,
      ChannelOut->CmdsCmdPiCode[0] + Offset,
      UpdateHost
      );

    //
    // Shift CmdN
    //
    ShiftPIforCmdTraining (
      MrcData,
      Channel,
      MrcIterationCmdN,
      RankMaskCh,
      1,
      ChannelOut->CmdnCmdPiCode[0] + Offset,
      UpdateHost
      );

#ifdef ULT_FLAG
    if (Lpddr) {
      //
      // For LPDDR need to shift CmdS PiCode[1] separately.
      // Host struct is not updated, so update PiCode[0] manually, and then restore back.
      //
      ChannelOut->CmdsCmdPiCode[0] = ChannelOut->CmdsCmdPiCode[0] + Offset;
      ShiftPIforCmdTraining (
        MrcData,
        Channel,
        MrcIterationCmdS,
        RankMaskCh,
        2,
        ChannelOut->CmdsCmdPiCode[1] + Offset,
        UpdateHost
        );
      ChannelOut->CmdsCmdPiCode[0] = ChannelOut->CmdsCmdPiCode[0] - Offset;
    }
#endif // ULT_FLAG
    //
    // Shift CKE
    //
    ShiftPIforCmdTraining (
      MrcData,
      Channel,
      MrcIterationCke,
      RankMaskCh,
      1,
      ChannelOut->CkeCmdPiCode[0] + Offset,
      UpdateHost
      );
  } // for Channel
  //
  // Reset DDR is required
  //
  if (ResetDDR) {
    Status = MrcResetSequence (MrcData);
  } else if (SelfRefresh) {
    ExitSR (MrcData);
  }

  return Status;
}

/**
  Returns the index into the array OptResult in the MrcOutput structure.

  @param[in] OptParam - Margin parameter

  @retval One of the following values: RdSAmpOfft(0), WrDSOfft (1), RxEqOfft(2), TxEqOfft (3), RdOdtOfft(4)
**/
U8
GetOptResultType (
  IN U8 OptParam
  )
{
  switch (OptParam) {
  case OptRxBias:
    return RdSAmpOfft;

  case OptWrDS:
    return WrDSOfft;

  case OptRxEq:
    return RxEqOfft;

  case OptTxEq:
    return TxEqOfft;

  case OptRdOdt:
    return RdOdtOfft;

  default:
    return 0; // Return RdSAmpOfft to point to the beginning of the array
  }
}

/**
  Program DimmOptPoint values on CPU and DIMM sides, such as DIMM ODT, CPU ODT, Ron, Slew Rate, Equalization.

  @param[in,out] MrcData          - Include all MRC global data.
  @param[in]     ChMask           - Channel to work on.
  @param[in,out] BestDimmOptPoint - Best DIMM Opt settings used to update hardware
  @param[in]     SkipGRdOdt       - Switch to skip updating CPU ODT
  @param[in]     SkipDimmOdts     - Switch to skip updating DIMM ODT
  @param[in]     SkipBestOffsets  - Switch to skip updating Opt settings
  @param[in]     UpdateHost       - Switch to skip updating MRC host structure

  @retval Nothing
**/
void
UpdateOdtsValues (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   ChMask,
  IN OUT DimmOptPoint         *BestDimmOptPoint,
  IN     BOOL                 SkipGRdOdt,
  IN     BOOL                 SkipDimmOdts,
  IN     BOOL                 SkipBestOffsets,
  IN     BOOL                 UpdateHost
  )
{
  MrcOutput       *Outputs;
  U8              byte;
  U8              rank;
  U8              Channel;
  U8              offset;
  U8              Dimm;
  U8              test;
  U8              TestArray[5];
  BOOL            DebugPrint;
  U8              OptParam;
  U8              NumTests;
  const MrcDebug  *Debug;

  Outputs     = &MrcData->SysOut.Outputs;
  Debug       = &MrcData->SysIn.Inputs.Debug;
  NumTests    = BestDimmOptPoint->OptParamTestListSize;
  DebugPrint  = 0;

  MrcOemMemorySet (TestArray, 0, sizeof (TestArray));
  if (SkipBestOffsets) {
    NumTests = 0;
  }
  //
  // build tests array to update RdSAmpOfft(0), WrDSOfft (1), RxEqOfft(2), TxEqOfft (3), RdOdtOfft(4)
  //
  for (test = 0; test < NumTests; test++) {
    OptParam  = BestDimmOptPoint->OptParamTestList[test];
    TestArray[GetOptResultType (OptParam)] = 1;
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "Test - %s : %d ,UpdateHost: %d\n",
      TOptParamOffsetString[OptParam],
      test,
      UpdateHost
      );
  }

  if (!SkipGRdOdt) {
    //
    // update GRdOdt
    //
    BestDimmOptPoint->ODTSet.GRdOdtCode = UpdateCompGlobalOffset (
                                            MrcData,
                                            RdOdt,
                                            (U8) BestDimmOptPoint->ODTSet.GRdOdt,
                                            UpdateHost
                                            );
    if (DebugPrint) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "best GRdODT aplly is : %d \n",
        CalcRdOdt (MrcData, BestDimmOptPoint->ODTSet.GRdOdt)
        );
    }
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(MrcChannelExist (Outputs, Channel))) {
      continue; // Not valid channel
    }

    if (!((MRC_BIT0 << Channel) & ChMask)) {
      continue;
    }

    offset = 1;
    if ((Outputs->Controller[0].Channel[Channel].DimmCount == 1)) {
      offset = 0; // disable dynamic odt
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        //
        // set RttNom=write and RttWr=0
        //
        BestDimmOptPoint->ODTSet.RttNom[Channel][Dimm] = BestDimmOptPoint->ODTSet.RttWr[Channel][Dimm];
      }
    }
#ifdef ULT_FLAG
    if (MrcData->SysIn.Inputs.CpuModel == cmHSW_ULT) {
      //
      // On ULT (1DPC) DIMM ODT is connected to Vdd, so RttNom must be disabled
      //
      offset = 1;
      BestDimmOptPoint->ODTSet.RttNom[Channel][0] = 0;
      BestDimmOptPoint->ODTSet.RttNom[Channel][1] = 0;
      BestDimmOptPoint->ODTSet.RttWr[Channel][1] = 0;
    }
#endif //ULT_FLAG

    //
    // Apply Best RTT Points
    //
    if (!SkipDimmOdts) {
      UpdateOptParamOffset (
        MrcData,
        Channel,
        0x3,
        0,
        OptDimmOdt,
        (S16)
        (
        ((offset * BestDimmOptPoint->ODTSet.RttWr[Channel][0]) << 4) +
        (BestDimmOptPoint->ODTSet.RttNom[Channel][0])
        ),
        UpdateHost
        );
      UpdateOptParamOffset (
        MrcData,
        Channel,
        0xC,
        0,
        OptDimmOdt,
        (S16)
        (
        ((offset * BestDimmOptPoint->ODTSet.RttWr[Channel][1]) << 4) +
        (BestDimmOptPoint->ODTSet.RttNom[Channel][1])
        ),
        UpdateHost
        );
      if (DebugPrint) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "best RttNom0 aplly is : %d\t\n",
          ActualDimmOdt[BestDimmOptPoint->ODTSet.RttNom[Channel][0]]
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "best RttNom1 aplly is : %d\t\n",
          ActualDimmOdt[BestDimmOptPoint->ODTSet.RttNom[Channel][1]]
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "best RttWr0 aplly is : %d\t\n",
          ActualDimmOdt[BestDimmOptPoint->ODTSet.RttWr[Channel][0]]
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "best RttWr1 aplly is : %d\t\n",
          ActualDimmOdt[BestDimmOptPoint->ODTSet.RttWr[Channel][1]]
          );
      }
    }

    if (NumTests) {
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        //
        // Apply Best RdOdt and WrDS
        // OdtOff = Off[RdOdtOfft][0][Channel][byte] + RdOdtChOffset[Channel];
        //
        if (TestArray[RdSAmpOfft]) {
          //
          // OptRdOdt->OptRxBias
          //
          UpdateOptParamOffset (
            MrcData,
            Channel,
            0xF,
            byte,
            OptRxBias,
            BestDimmOptPoint->BestOptOff[RdSAmpOfft][0].Offset[Channel][byte],
            UpdateHost
            );
        }

        if (TestArray[WrDSOfft]) {
          UpdateOptParamOffset (
            MrcData,
            Channel,
            0xF,
            byte,
            OptWrDS,
            BestDimmOptPoint->BestOptOff[WrDSOfft][0].Offset[Channel][byte],
            UpdateHost
            );
        }

        if (DebugPrint) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "enable=%d best WrDSOfft byte %d is : %d\t\n",
            TestArray[WrDSOfft],
            byte,
            BestDimmOptPoint->BestOptOff[WrDSOfft][0].Offset[Channel][byte]
            );
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "enable=%d best RdSAmpOfft byte %d is : %d\t\n",
            TestArray[RdSAmpOfft],
            byte,
            BestDimmOptPoint->BestOptOff[RdSAmpOfft][0].Offset[Channel][byte]
            );
        }

        for (rank = 0; rank < MAX_RANK_IN_CHANNEL; rank++) {
          if (!MrcRankInChannelExist (MrcData, rank, Channel)) {
            continue;
          }
          //
          // Apply Best Tx/Rx EQ Codes
          //
          if (TestArray[RxEqOfft]) {
            UpdateOptParamOffset (
              MrcData,
              Channel,
              (MRC_BIT0 << rank),
              byte,
              OptRxEq,
              BestDimmOptPoint->BestOptOff[RxEqOfft][rank].Offset[Channel][byte],
              UpdateHost
              );
          }

          if (TestArray[TxEqOfft]) {
            UpdateOptParamOffset (
              MrcData,
              Channel,
              (MRC_BIT0 << rank),
              byte,
              OptTxEq,
              BestDimmOptPoint->BestOptOff[TxEqOfft][rank].Offset[Channel][byte],
              UpdateHost
              );
          }

          if (DebugPrint) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "enable=%d best OptRxEq rank%d byte %d is : %d\t\n",
              TestArray[RxEqOfft],
              rank,
              byte,
              BestDimmOptPoint->BestOptOff[RxEqOfft][rank].Offset[Channel][byte]
              );
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "enable=%d best OptTxEq rank%d byte %d is : %d\t\n",
              TestArray[TxEqOfft],
              rank,
              byte,
              BestDimmOptPoint->BestOptOff[TxEqOfft][rank].Offset[Channel][byte]
              );
          }
        }
      }
    }
  }
}

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
void
CalcPower (
  IN  MrcParameters *MrcData,
  OUT MrcPower      *Results,
  IN  U16           RonCpu,
  IN  U8            RonDimm,
  IN  U16           Rodtcpu,
  IN  U16           Rodtdram,
  IN  U16           Wodtdram
  )
{
  const MrcInput  *Inputs;
  const MrcDebug  *Debug;
  MrcOutput       *Outputs;
  U16             CapTotal;
  U32             CapOdt;
  U32             Rodt;
  U32             Vx;
  U32             Vy;
  U32             Ix;
  U32             Iy;
  U32             DrvPwr;
  U32             ACPowerRd;
  U32             ACPowerWr;
  //
  // Power Results;
  //
  U16             ScaleCpuPwr;
  U16             ScaleDramPwr;
  U16             LineLength; // cm
  U16             Cpad; // pF
  U32             Derating;
  U32             ACPower;
  U32             Vswing;
  U16             CapPerLength; // pF/cm
  U16             Freq; // Ghz
  U16             FreqEff;
  U16             Pi; // 3.14;
  U16             Vdd; // 1.5; mV
  U16             SRDimm; // 15ohm serial resistance
  U16             NormFactor;

  Vx            = 0;
  Vy            = 0;
  Ix            = 0;
  Iy            = 0;
  ScaleCpuPwr   = 1;
  ScaleDramPwr  = 1;
  LineLength    = 10;
  Cpad          = 4;
  CapPerLength  = 2;
  Pi            = 3;
  SRDimm        = 15;
  NormFactor    = 100; // if 1000 we get mW
  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  Vdd           = Outputs->VddVoltage[Inputs->MemoryProfile];
  Freq = (U16) (Outputs->Frequency);
  Freq /= 100; // in 10xGhz
  //
  // capacitance for AC power
  // Cut real cap in half and add 10pF offset to better match curves - results x100 pf
  // Fixed frequency at 500 MHz(~Data Rate/4 assuming random 1100 type data) - resutls is 100x Ghz
  // In general, most of the simulations show fairly flat AC power vs. frequency
  //
  CapTotal = (Cpad + LineLength * CapPerLength) * 45;
  FreqEff  = 50;
  CapOdt   = 10000000 / (2 * Pi * CapTotal * FreqEff);  // Scale Up by 2.5x to better match curves
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RonDimm=%d Rodtcpu=%d Wodtdram=%d  Rodtdram=%d RonCpu=%d CapOdt=%d\n",RonDimm,Rodtcpu,Wodtdram,Rodtdram,RonCpu,CapOdt);
  // for read
  //
  Rodt = (Rodtcpu * (Rodtdram + SRDimm)) / (Rodtcpu + (Rodtdram + SRDimm));
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "(Rodtdram+SRDimm)=%d Rodtcpu=%d  Rodt=%d \n",(Rodtdram+SRDimm),Rodtcpu,Rodt);
  Derating = 1000 * Rodt / (Rodt + CapOdt); // Derate ACPower based on ratio of Real ODT vs. "0DT" due to cap
  Ix = Vdd / 2 / (RonDimm + SRDimm + Rodt); // mA
  Vx = Vdd - Ix * (RonDimm + SRDimm); // voltage after dimm driver+15ohm
  DrvPwr = Ix * Ix * (RonDimm + SRDimm) / NormFactor; // dimm Ron static power
  Vswing = 2 * Vx - Vdd; // for ACpower= Vh-Vl
  //
  // Calculate power associated with swing that cap - mV/1000*pf/100*Ghz/100000
  //
  ACPower = Vswing * Vswing / 1000 * CapTotal * FreqEff / 100 / 100 / NormFactor;
  ACPowerRd = ACPower * Derating / 1000; // mW
  //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "read ACPowerRd=%d ACPower=%d Derating=%d  Vswing=%d Rodt=%d Ix=%d,Vx=%d,PwrDrv=%d,FreqEff=%d CapOdt=%d Rodt=%d ,DrvPwr=%d\n",ACPowerRd,ACPower,Derating,Vswing,Rodt,Ix,Vx,DrvPwr,FreqEff,CapOdt,Rodt,DrvPwr);
  Results->CpuPwrRd = ((Vdd - Vx) * (Vdd - Vx) + Vx * Vx) / (2 * Rodtcpu * NormFactor); // mW  @todo:add RxBias?
  Iy = (Vx - Vdd / 2) / (Rodtdram + SRDimm); // current in to the NT dimm
  Vy = Vx - Iy * (SRDimm); // voltage after 15 ohm inside the dimm
  Results->DimmPwrRd = Iy *
    Iy *
    SRDimm /
    NormFactor +
    ((Vdd - Vy) * (Vdd - Vy) + Vy * Vy) /
    (2 * Rodtdram * NormFactor) +
    DrvPwr;  // mW
  //
  // for write
  //
  Rodt      = (Wodtdram + SRDimm) * (Rodtdram + SRDimm) / ((Wodtdram + SRDimm) + (Rodtdram + SRDimm));
  Derating  = 1000 * Rodt / (Rodt + CapOdt); // De-rate ACPower based on ratio of Real ODT vs. "0DT" due to cap
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Rodt=%d Derating=%d ",Rodt,Derating);
  Ix = Vdd / 2 / (RonCpu + Rodt); // mA
  Vx = Vdd - Ix * (RonCpu);  // voltage after cpu driver
  DrvPwr = Ix * Ix * (RonCpu) / NormFactor; // cpu Ron static power
  Vswing = 2 * Vx - Vdd;  // for ACpower
  //
  // Calculate power associated with swing that cap
  //
  ACPower = Vswing * Vswing / 1000 * CapTotal * FreqEff / 100 / 100 / NormFactor;
  ACPowerWr = ACPower * Derating / 1000; // mW
  //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "write ACPowerWr=%d ACPower=%d Derating=%d  Vswing=%d Rodt=%d Ix=%d,Vx=%d,PwrDrv=%d,FreqEff=%d CapOdt=%d Rodt=%d ,DrvPwr=%d\n",ACPowerWr,ACPower,Derating,Vswing,Rodt,Ix,Vx,DrvPwr,FreqEff,CapOdt,Rodt,DrvPwr);
  Results->CpuPwrWr = DrvPwr; // mW
  Iy = (Vx - Vdd / 2) / (Wodtdram + SRDimm); // current in to the T dimm
  Vy = Vx - Iy * (SRDimm); // voltage after 15 ohm inside the dimm
  Results->DimmPwrWrT = Iy * Iy * SRDimm / NormFactor + ((Vdd - Vy) * (Vdd - Vy) + Vy * Vy) / (2 * Wodtdram * NormFactor); // mW
  Iy = (Vx - Vdd / 2) / (Rodtdram + SRDimm);  // current in to the NT dimm
  Vy = Vx - Iy * (SRDimm);  // voltage after 15 ohm inside the dimm
  Results->DimmPwrWrNT = Iy * Iy * SRDimm / NormFactor + ((Vdd - Vy) * (Vdd - Vy) + Vy * Vy) / (2 * Rodtdram * NormFactor);  // mW

  //
  // ScaleCpuPwr and ScaleDramPwr allows one to tradeoff CPU vs. DRAM power
  //
  Results->ACPowerRd  = ACPowerRd;
  Results->ACPowerWr  = ACPowerWr;
  Results->TotPwr = (U16)
    (
      60 * (Results->CpuPwrRd * ScaleCpuPwr + Results->DimmPwrRd * ScaleDramPwr + ACPowerRd * ScaleDramPwr) + 40 *
      (
        (Results->DimmPwrWrT + Results->DimmPwrWrNT) *
        ScaleDramPwr +
        Results->CpuPwrWr *
        ScaleCpuPwr +
        ACPowerWr *
        ScaleCpuPwr
      )
    ) / 100;
  Results->ACPower = (60 * ACPowerRd + 40 * ACPowerWr) / 100;
  Results->CpuPower = (U16)
    (
      60 * (Results->CpuPwrRd * ScaleCpuPwr ) + 40 *
      (
        Results->CpuPwrWr *
        ScaleCpuPwr +
        ACPowerWr *
        ScaleCpuPwr
      )
    ) / 100;

  Results->DimmPwr = (U16)
    (
      60 * (Results->DimmPwrRd * ScaleDramPwr + ACPowerRd * ScaleDramPwr) + 40 *
      (
        (Results->DimmPwrWrT + Results->DimmPwrWrNT) *
        ScaleDramPwr
      )
    ) / 100;
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " PwrTotal=%d DimmPwrR=%d DimmPwrWrT=%d DimmPwrWrNT=%d PwrDrvR=%d Rodt=%d  PwrDrvW=%d\n",Results->TotPwr,Results->DimmPwrRd,Results->DimmPwrWrT,Results->DimmPwrWrNT,ACPowerRd,Rodt,ACPowerWr);
  return;
}

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
void
CalcPowerTrend (
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
  )
{
  const MrcDebug  *Debug;
  MrcChannelOut   *ChannelOut;
  U16             MaxPoints[4];
  U16             MinPoints[4];
  U16             MaxPwr;
  U16             MinPwr;
  U8              off;
  U8              test;
  U8              dimm;
  U8              TestParam;
  MrcPower        PwrRes;
  U16             AveOfMax;
  U16             X;
  S16             MinRatio;
  S16             Ratio;
  U16             Slope;
  U16             SlopeOver100;
  U16             Rodtcpu;
  U8              RonDimm;
  U8              RonCpu;
  U16             Rodtdram;
  U16             Wodtdram;
  U16             *Points;
  U16             *PointsElement;
  DimmOptPoint    *DimmPoints;
  U8              dimmcount;
  BOOL            is1DPC;
  U16             AvgROdt;

  ChannelOut  = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  Debug       = &MrcData->SysIn.Inputs.Debug;
  Points      = (U16 *) Points2calc;
  is1DPC      = (ChannelOut->DimmCount == 1);
  MaxPwr      = 0;
  MinPwr      = 0xffff;
  Wodtdram    = 0;

  MrcOemMemorySet ((U8 *) &PwrRes, 0, sizeof (PwrRes));
  MrcOemMemorySetWord (MaxPoints, 0, sizeof (MaxPoints) / sizeof (U16));
  MrcOemMemorySetWord (MinPoints, 0xFFFF, sizeof (MinPoints) / sizeof (U16));

  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TestListSize=%d\n",TestListSize);
  //
  for (off = 0; off < LenMargin; off++) {
    //
    // sorting the min max power points
    //
    for (test = 0; test < TestListSize; test++) {
      //
      // sorting the min max margin points for each test
      //
      PointsElement = (Points + ArrayLength * test + off);
      if (MaxPoints[test] < *PointsElement) {
        MaxPoints[test] = *PointsElement;
      }

      if (MinPoints[test] > *PointsElement) {
        MinPoints[test] = *PointsElement;
      }
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*(Points+ArrayLength*test+off)=%d (Points+ArrayLength*test+off)=%x\n",*(Points+ArrayLength*test+off),(Points+ArrayLength*test+off));
      //
    }

    if (!PwrCalc1d) {
      DimmPoints  = (DimmOptPoint *) DimmOptPoints;
      RonDimm     = 0;
      RonCpu      = 30;
      Rodtcpu     = CalcRdOdt (MrcData, (DimmPoints + off)->ODTSet.GRdOdt);
      dimmcount   = 0;
      AvgROdt     = 0;
      for (dimm = 0; dimm < MAX_DIMMS_IN_CHANNEL; dimm++) {
        if (!((MRC_BIT0 << dimm) & DimmMask)) {
          continue;
        }
        //
        // read from MR1 the DimmRon
        //
        RonDimm  += (U8) CalcOptPower (MrcData, Channel, 2 * dimm, 0, OptDimmRon, 0, 0, 1);
        Rodtdram  = ActualDimmOdt[(DimmPoints + off)->ODTSet.RttNom[Channel][dimm]];
        Wodtdram  = ActualDimmOdt[(DimmPoints + off)->ODTSet.RttWr[Channel][dimm]];
        if (is1DPC) {           // in 1DPC channel always use only one of the terminations
          if (Wodtdram == 0) {
            Wodtdram  = Rodtdram;
          }
          Rodtdram  = 0x3fff; // put 8k ohm as infinity
        }

        if (Rodtdram == 0) {
          Rodtdram = 0x3fff;
        }

        if (Wodtdram == 0) {
          Wodtdram = Rodtdram; // in 2DPC with RttW=0
        }

        AvgROdt += Rodtdram;
        dimmcount++;
      }

      AvgROdt = (dimmcount != 0) ? AvgROdt / dimmcount : AvgROdt;
      RonDimm = (dimmcount != 0) ? RonDimm / dimmcount : RonDimm;
      if ((120 < AvgROdt) && (AvgROdt < 0x3fff)) {
        Rodtdram = 240; // the mix case of one open and one not
      } else {
        Rodtdram = AvgROdt; // for write average not needed because its by dimm
      }
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "input for power calc Wodtdram=%d Rodtdram=%d RonDimm=%d \n",Wodtdram,Rodtdram,RonDimm);
      //
      PointsElement = (Points + ArrayLength * TestListSize + off);
      CalcPower (MrcData, &PwrRes, RonCpu, RonDimm, Rodtcpu, Rodtdram, Wodtdram);
      *PointsElement                = PwrRes.TotPwr;
      (DimmPoints + off)->PowerCalc = PwrRes;

      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Rodtcpu=%d  Wodtdram=%d EqRodtdram=%d Calcpower=%d\n",Rodtcpu,Wodtdram,Rodtdram,*(Points+ArrayLength*test+off));
      //
    } else {
      PointsElement = (Points + ArrayLength * TestListSize + off);
    }

    if (MaxPwr < *PointsElement) {
      MaxPwr = *PointsElement;
    }

    if (MinPwr > *PointsElement) {
      MinPwr = *PointsElement;
    }

    if (LenMargin == 1) {
      MaxPwr  = *PointsElement;
      MinPwr  = 0;
    }
  }
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MaxPwr=%d MinPwr=%d\n",MaxPwr,MinPwr);
  //
  AveOfMax  = 0;
  MinRatio  = 0x7fff;
  for (test = 0; test < TestListSize; test++) {
    AveOfMax += MaxPoints[test];
    //
    // map Test to TestParam
    //
    TestParam = TestList[test];
    Ratio = (100 * (MaxPoints[test] / Scale[test] - UpmPwrLimitValue (MrcData, TestParam, UpmLimit))) /
            (UpmPwrLimitValue (MrcData, TestParam, PowerLimit) - UpmPwrLimitValue (MrcData, TestParam, UpmLimit));
    if (MinRatio > Ratio) {
      MinRatio = Ratio;
    }
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "AveOfMax=%d MinRatio=%d MaxPoints[%d]=%d Scale[%d]=%d \n",AveOfMax,MinRatio,test,MaxPoints[test],test,Scale[test]);
    //
  }

  AveOfMax = AveOfMax / TestListSize;
  //
  // if MaxPoint > UPM Limit:          PwrTrend should be flat
  // if MaxPoints == PwrLimit:         PwrTrend should have slope going from AveOfMax to (1-PWRTrendSlope/100)*AveOfMax
  // PwrTrend will be a linear slope going from (MinPwr, (1- PWRTrendSlope/100)*AveOfMax) to (MaxPwr, AveOfMax)
  //
  Slope         = (PWRTrendSlope * MinRatio) / 100;
  SlopeOver100  = 0;
  if (Slope > 100) {
    //
    // could only happen if no power limits
    //
    SlopeOver100  = Slope - 100;
    Slope         = 100;
  }

  for (off = 0; off < LenMargin; off++) {
    PointsElement = (Points + ArrayLength * TestListSize + off);
    if (MinRatio < 0) {
      *PointsElement = 1;
    } else {
      if (MaxPwr == MinPwr) {
        X = 0; // no power consideration and not divide by zero
      } else {
        //
        // % of where you are between Min and Max Pwr. X=0 should be MaxPwr and 100 should be MinPwr
        //
        X = 100 - 100 * (*PointsElement - MinPwr) / (MaxPwr);
      }
      //
      // Create a linear line based on Power from (1 - PWRTrendSlope / 100) * AveOfMax to AveOfMax
      // Adding a specicial case for TX XTalk: If PWRTrendSlope = 0 and ArrayLength = BIT_TX_XTALK_RANGE
      // just multiply power numbers by AveOfMax.
      //
      if ((PWRTrendSlope == 0) && (ArrayLength == BIT_TX_XTALK_RANGE)) {
        *PointsElement = *PointsElement * AveOfMax / 100;
      } else {
        *PointsElement = AveOfMax * (100 - Slope + (((Slope + SlopeOver100) * X) / 100)) / 100;
      }
      //
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "power trend Points[%d][%d]=%d\n",TestListSize,off,*(Points+ArrayLength*TestListSize+off));
      //
    }
  }
}

#ifdef MRC_DEBUG_PRINT

#if 0 // This function is not used right now
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
void
ReadOptParamOffsetSum (
  IN MrcParameters *const MrcData,
  IN U8                   ChMask,
  IN U8                   RankMask,
  IN const U8             OptParam,
  IN OptOffsetChByte      *OptOff,
  IN BOOL                 OptResult
  )
{
  const MrcDebug  *Debug;
  MrcOutput       *Outputs;
  U8              Channel;
  U8              Rank;
  U8              Byte;
  U8              Param;
  U8              NumBytes;
  U8              ChannelMask;
  S16             OffArr[2];
  S16             Best;
  BOOL            PerRank;
  BOOL            SkipByte;

  Outputs     = &MrcData->SysOut.Outputs;
  Debug       = &MrcData->SysIn.Inputs.Debug;
  ChannelMask = Outputs->ValidChBitMask & ChMask;
  NumBytes    = (U8) Outputs->SdramCount;
  MrcOemMemorySetWord ((U16 *) OffArr, (U16) 0, sizeof (OffArr) / sizeof (OffArr[0]));

  for (Param = OptWrDS; Param < OptDefault; Param++) {
    if (OptParam == Param || OptParam == OptDefault) {

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nOffsets for Optimization Parameter %s\n", TOptParamOffsetString[Param]);
      PerRank =
        (
          Param == OptTxEq ||
          Param == OptRxEq ||
          Param == OptDimmOdt ||
          Param == OptDimmOdtWr ||
          Param == OptDimmRon
        );
      SkipByte = (Param == OptDimmRon || Param == OptDimmOdt || Param == OptDimmOdtWr);

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!((MRC_BIT0 << Channel) & ChannelMask)) {
          continue;
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %d\n", Channel);
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          if ((MRC_BIT0 << Rank) & RankMask) {
            if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
              continue;
            }

            if (PerRank) {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Rank %d\n", Rank);
            } else if (Rank > 0) {
              continue;
            }

            if (!SkipByte) {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Byte\t");
              for (Byte = 0; Byte < NumBytes; Byte++) {
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", Byte);
              }

              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
            }

            if (OptResult) {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Opt/CR/Host\t");
            } else {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CR/Host\t");
            }

            if (!SkipByte) {
              for (Byte = 0; Byte < NumBytes; Byte++) {
                ReadOptParamOffset (MrcData, &OffArr[0], Channel, Rank, Byte, Param);

                if (OptResult) {
                  Best = OptOff->Offset[Channel][Byte];
                  if (Best != OffArr[0] || Best != OffArr[1]) {
                    MRC_DEBUG_MSG (
                      Debug,
                      MSG_LEVEL_NOTE,
                      "\nError: Mismatch in Param %s in Channel %d Rank %d Byte %d is found: Best=%d CR=%d Host=%d\n",
                      TOptParamOffsetString[Param],
                      Channel,
                      Rank,
                      Byte,
                      Best,
                      OffArr[0],
                      OffArr[1]
                      );
                  }

                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d/%d/%d\t", Best, OffArr[0], OffArr[1]);
                } else {
                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d/%d\t", OffArr[0], OffArr[1]);
                }
              }
            } else {
              ReadOptParamOffset (MrcData, &OffArr[0], Channel, Rank, 0, Param);

              if (Param == OptDimmRon || Param == OptDimmOdtWr) {
                if (OptResult) {
                  Best = OptOff->Offset[Channel][0];
                  if (Best != OffArr[1]) {
                    MRC_DEBUG_MSG (
                      Debug,
                      MSG_LEVEL_NOTE,
                      "\nError: Mismatch in Param %s in Channel %d Rank %d is found: Best=%d Host=%d\n",
                      TOptParamOffsetString[Param],
                      Channel,
                      Rank,
                      Best,
                      OffArr[1]
                      );
                  }

                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d/NA/%d", Best, OffArr[1]);
                } else {
                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "NA/%d", OffArr[1]);
                }
              } else if (Param == OptDimmOdt) {
                if (OptResult) {
                  Best = OptOff->Offset[Channel][0];
                  if (Best != OffArr[0]) {
                    MRC_DEBUG_MSG (
                      Debug,
                      MSG_LEVEL_NOTE,
                      "\nError: Mismatch in Param %s in Channel %d Rank %d is found: Best=%d Host=%d\n",
                      TOptParamOffsetString[Param],
                      Channel,
                      Rank,
                      Best,
                      OffArr[0]
                      );
                  }

                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d/NA/%d", Best, OffArr[0]);
                } else {
                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "NA/%d", OffArr[0]);
                }
              }
            }

            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
          }
        }
      }
    }
  }
}

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
void
ReadOptParamOffset (
  IN  MrcParameters *const MrcData,
  OUT S16                  *FinalVal,
  IN  const U8             Channel,
  IN  const U8             Rank,
  IN  const U8             Byte,
  IN  const U8             OptParam
  )
{
  const U16       RttNomMRSEncodingConst[] = {0x00, 0x10, 0x01, 0x11, 0x81, 0x80}; // RttNom Off,120,60,40,30,20 Ohms
  const U16       RttWrMRSEncodingConst[]  = {0x00, 0x02, 0x01};                // RttWr  RttNom,120,60 Ohms
  const MrcDebug  *Debug;
#ifdef ULT_FLAG
  const U8        LpddrRonEnc[] = {0x1,0x2,0x3}; //{34,40,48};
  const U8        LpddrOdtEnc[] = {0x0,0x2,0x3}; //{0,120,240};
  BOOL            Lpddr;
  U16             DimmRonMask;
#endif // ULT_FLAG
  MrcOutput       *Outputs;
  MrcChannelOut   *ChannelOut;
  U16             *MrReg;
  BOOL            Type;
  U8              Index;
  U16             MRValue;
  U16             RttNomMRSEncoding[sizeof (RttNomMRSEncodingConst) / sizeof (RttNomMRSEncodingConst[0])];
  U16             RttWrMRSEncoding[sizeof (RttWrMRSEncodingConst) / sizeof (RttWrMRSEncodingConst[0])];
  U16             RttWr;
  U16             RttNom;
  U16             RttNomMask;
  U16             RttWrMask;
  U32             Offset;
  S16             UpOff;
  S16             DnOff;
  DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_STRUCT DdrCrDataOffsetCompCr;
  DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_STRUCT DdrCrDataOffsetCompHost;
  DDRDATA0CH0_CR_TXTRAINRANK0_STRUCT        CrTxTrainRank;
  DDRDATA0CH0_CR_RXTRAINRANK0_STRUCT        CrRxTrainRank;
  DDRDATA0CH0_CR_DDRCRDATACONTROL1_STRUCT   DdrCrDataControl1Cr;
  DDRDATA0CH0_CR_DDRCRDATACONTROL1_STRUCT   DdrCrDataControl1Host;

  Debug       = &MrcData->SysIn.Inputs.Debug;
  Outputs     = &MrcData->SysOut.Outputs;
  ChannelOut  = &Outputs->Controller[0].Channel[Channel];
  MrcOemMemoryCpy ((U8 *) RttNomMRSEncoding, (U8 *) RttNomMRSEncodingConst, sizeof (RttNomMRSEncoding));
  MrcOemMemoryCpy ((U8 *) RttWrMRSEncoding, (U8 *) RttWrMRSEncodingConst, sizeof (RttWrMRSEncoding));

#ifdef ULT_FLAG
  Lpddr = Outputs->DdrType == MRC_DDR_TYPE_LPDDR3;
#endif
  //
  // Compensation Offsets
  //
  Type = ((OptParam == OptWrDS) || (OptParam == OptRdOdt) || (OptParam == OptTComp) || (OptParam == OptSComp));
  if (Type) {

    Offset = DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_REG +
      ((DDRDATA1CH0_CR_DDRCRDATAOFFSETCOMP_REG - DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_REG) * Byte) +
      ((DDRDATA0CH1_CR_DDRCRDATAOFFSETCOMP_REG - DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_REG) * Channel);
    DdrCrDataOffsetCompCr.Data   = MrcReadCR (MrcData, Offset);
    DdrCrDataOffsetCompHost.Data = ChannelOut->DataCompOffset[Byte];

    if (OptParam == OptWrDS) {
      UpOff = (S16) (S32) DdrCrDataOffsetCompCr.Bits.DqDrvUpCompOffset;
      DnOff = (S16) (S32) DdrCrDataOffsetCompCr.Bits.DqDrvDownCompOffset;
      if (UpOff != DnOff) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "DqDrvUpCompOffset %d is not equal to DqDrvDownCompOffset for Channel=%d, Byte=%d\n",
          UpOff,
          DnOff,
          Channel,
          Byte
          );
      }

      FinalVal[0] = UpOff;
      UpOff = (S16) (S32) DdrCrDataOffsetCompHost.Bits.DqDrvUpCompOffset;
      DnOff = (S16) (S32) DdrCrDataOffsetCompHost.Bits.DqDrvDownCompOffset;
      if (UpOff != DnOff) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "DqDrvUpCompOffset %d is not equal to DqDrvDownCompOffset for Channel=%d, Byte=%d in Host Structure\n",
          UpOff,
          DnOff,
          Channel,
          Byte
          );
      }

      FinalVal[1] = UpOff;

      if (FinalVal[0] & 0x20) {     // 6-bit 2's complement
        FinalVal[0] -= 0x40;
      }
      if (FinalVal[1] & 0x20) {     // 6-bit 2's complement
        FinalVal[1] -= 0x40;
      }
    } else if (OptParam == OptRdOdt) {
      UpOff = (S16) (S32) DdrCrDataOffsetCompCr.Bits.DqOdtUpCompOffset;
      DnOff = (S16) (S32) DdrCrDataOffsetCompCr.Bits.DqOdtDownCompOffset;
      if (UpOff != DnOff) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "DqOdtUpCompOffset %d is not equal to DqOdtDownCompOffset for Channel=%d, Byte=%d\n",
          UpOff,
          DnOff,
          Channel,
          Byte
          );
      }

      FinalVal[0] = UpOff;
      UpOff = (S16) (S32) DdrCrDataOffsetCompHost.Bits.DqOdtUpCompOffset;
      DnOff = (S16) (S32) DdrCrDataOffsetCompHost.Bits.DqOdtDownCompOffset;
      if (UpOff != DnOff) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "DqOdtUpCompOffset %d is not equal to DqOdtDownCompOffset for Channel=%d, Byte=%d in Host Structure\n",
          UpOff,
          DnOff,
          Channel,
          Byte
          );
      }

      FinalVal[1] = UpOff;

      if (FinalVal[0] & 0x10) {     // 5-bit 2's complement
        FinalVal[0] -= 0x20;
      }
      if (FinalVal[1] & 0x10) {     // 5-bit 2's complement
        FinalVal[1] -= 0x20;
      }
    } else if (OptParam == OptTComp) {
      FinalVal[0] = (S16) (S32) DdrCrDataOffsetCompCr.Bits.DqTcoCompOffset;
      FinalVal[1] = (S16) (S32) DdrCrDataOffsetCompHost.Bits.DqTcoCompOffset;

      if (FinalVal[0] & 0x10) {     // 5-bit 2's complement
        FinalVal[0] -= 0x20;
      }
      if (FinalVal[1] & 0x10) {     // 5-bit 2's complement
        FinalVal[1] -= 0x20;
      }
    } else if (OptParam == OptSComp) {
      FinalVal[0] = (S16) (S32) DdrCrDataOffsetCompCr.Bits.DqSlewRateCompOffset;
      FinalVal[1] = (S16) (S32) DdrCrDataOffsetCompHost.Bits.DqSlewRateCompOffset;

      if (FinalVal[0] & 0x10) {     // 5-bit 2's complement
        FinalVal[0] -= 0x20;
      }
      if (FinalVal[1] & 0x10) {     // 5-bit 2's complement
        FinalVal[1] -= 0x20;
      }
    }

    if (FinalVal[0] != FinalVal[1]) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "Error: Param %s: CR value %d doesn't match Host value %d for Channel=%d, Byte=%d\n",
        TOptParamOffsetString[OptParam],
        FinalVal[0],
        FinalVal[1],
        Channel,
        Byte
        );
    }
  }
  //
  // Equalization Settings
  //
  Type = ((OptParam == OptTxEq) || (OptParam == OptRxEq));
  if (Type) {
    //
    // TxEq[5:4] = Emphasize   = [3, 6, 9, 12] legs
    // TxEq[3:0] = Deemphasize = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 4*Rsvd] legs
    //
    if (OptParam == OptTxEq) {

     Offset = DDRDATA0CH0_CR_TXTRAINRANK0_REG +
      ((DDRDATA0CH1_CR_TXTRAINRANK0_REG - DDRDATA0CH0_CR_TXTRAINRANK0_REG) * Channel) +
      ((DDRDATA0CH0_CR_TXTRAINRANK1_REG - DDRDATA0CH0_CR_TXTRAINRANK0_REG) * Rank) +
      ((DDRDATA1CH0_CR_TXTRAINRANK0_REG - DDRDATA0CH0_CR_TXTRAINRANK0_REG) * Byte);

      CrTxTrainRank.Data  = MrcReadCR (MrcData, Offset);
      FinalVal[0]         = (S16) (S32) CrTxTrainRank.Bits.TxEqualization;
      FinalVal[1]         = (S16) (S32) ChannelOut->TxEq[Rank][Byte];
      FinalVal[0]        &= 0xF;  // Read Deemphasize portion only
      FinalVal[1]        &= 0xF;  // Read Deemphasize portion only
    }
    //
    // RxEQ[4:0] CR Decoding (pF/kOhm)
    //            [2:0]
    //  [4:3]     0        1        2        3        4        5-7
    //     0      0.5/.02  0.5/1.0  0.5/.50  0.5/.25  0.5/.12  rsvd
    //     1      1.0/.02  1.0/1.0  1.0/.50  1.0/.25  1.0/.12  rsvd
    //     2      1.5/.02  1.5/1.0  1.5/.50  1.5/.25  1.5/.12  rsvd
    //     3      2.0/.02  2.0/1.0  2.0/.50  2.0/.25  2.0/.12  rsvd
    // Sweep = 0-19        [4:3] = (Sweep/5)  [2:0] = (Sweep%5)
    //
    if (OptParam == OptRxEq) {
      Offset = DDRDATA0CH0_CR_RXTRAINRANK0_REG +
        ((DDRDATA0CH1_CR_RXTRAINRANK0_REG - DDRDATA0CH0_CR_RXTRAINRANK0_REG) * Channel) +
        ((DDRDATA0CH0_CR_RXTRAINRANK1_REG - DDRDATA0CH0_CR_RXTRAINRANK0_REG) * Rank) +
        ((DDRDATA1CH0_CR_RXTRAINRANK0_REG - DDRDATA0CH0_CR_RXTRAINRANK0_REG) * Byte);

      CrRxTrainRank.Data = MrcReadCR (MrcData, Offset);
      FinalVal[0] = (S16) (S32) CrRxTrainRank.Bits.RxEq;
      FinalVal[1] = (S16) (S32) ChannelOut->RxEq[Rank][Byte];
      FinalVal[0] = ((FinalVal[0] >> 3) * 5) + (FinalVal[0] & 0x7);  // Multiply Cap portion by 5 and add Res portion
      FinalVal[1] = ((FinalVal[1] >> 3) * 5) + (FinalVal[1] & 0x7);  // Multiply Cap portion by 5 and add Res portion
    }

    if (FinalVal[0] != FinalVal[1]) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "Error: Param %s: CR value %d doesn't match Host value %d for Channel=%d, Rank=%d, Byte=%d\n",
        TOptParamOffsetString[OptParam],
        FinalVal[0],
        FinalVal[1],
        Channel,
        Rank,
        Byte
        );
    }
  }
  //
  // RX Amplifier BIAS
  //
  if ((OptParam == OptRxBias)) {
    //
    // Mapping: [0: 0.44, 1: 0.66, 2: 0.88, 3: 1.00, 4: 1.33, 5: 1.66, 6: 2.00, 7: 2.33]
    //
    Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG +
      ((DDRDATA1CH0_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Byte) +
      ((DDRDATA0CH1_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Channel);

    DdrCrDataControl1Cr.Data    = MrcReadCR (MrcData, Offset);
    DdrCrDataControl1Host.Data  = ChannelOut->DqControl1[Byte].Data;
    FinalVal[0]                 = (S16) (S32) DdrCrDataControl1Cr.Bits.RxBiasCtl;
    FinalVal[1]                 = (S16) (S32) DdrCrDataControl1Host.Bits.RxBiasCtl;

    if (FinalVal[0] != FinalVal[1]) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "Error: Param %s: CR value %d doesn't match Host value %d for Channel=%d, Byte=%d\n",
        TOptParamOffsetString[OptParam],
        FinalVal[0],
        FinalVal[1],
        Channel,
        Byte
        );
    }
  }
  //
  // Dimm Ron value
  //
  if ((OptParam == OptDimmRon)) {
    //
    // DIMM Ron Encoding   DriverImpCtrl[A5,A1]
    //
    if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
      MrReg = &ChannelOut->Dimm[Rank / 2].Rank[(Rank % 2)].MR[mrMR0];
#ifdef ULT_FLAG
      if (Lpddr) {
        DimmRonMask = (MRC_BIT3 | MRC_BIT2 | MRC_BIT1 | MRC_BIT0);
        MRValue     = (MrReg[mrMR3] & DimmRonMask);

        for (Index = 0; Index < (sizeof (LpddrRonEnc) / sizeof (LpddrRonEnc[0])); Index++) {
          if (MRValue == LpddrRonEnc[Index]) {
            FinalVal[1] = (S16) (S8) Index;
          }
        }
      } else
#endif
      {
        MRValue     = MrReg[mrMR1];
        FinalVal[1] = (S16) ((MRValue >> 1) & 0x1);
      }
    }
  }
  //
  // DIMM ODT Values
  //
  if ((OptParam == OptDimmOdt) || (OptParam == OptDimmOdtWr)) {
    //
    // DIMM ODT Encoding   RttNom[A9,A6,A2]   RttWr[A10, A9] LPDDR - No RttNom
    //
    if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
#ifdef ULT_FLAG
      if (Lpddr) {
        RttWrMask = (MRC_BIT1 | MRC_BIT0);
        MRValue   = (ChannelOut->Dimm[Rank / 2].Rank[Rank % 2].MR11 & RttWrMask);

        for (Index = 0; Index < (sizeof (LpddrOdtEnc) / sizeof (LpddrOdtEnc[0])); Index++) {
          if (MRValue == LpddrOdtEnc[Index]) {
            FinalVal[1] = (S16) (S8) Index;
          }
        }

        FinalVal[0] = 0;
      } else
#endif
      {
        MrReg       = &ChannelOut->Dimm[Rank / 2].Rank[Rank % 2].MR[mrMR0];
        RttNomMask  = (MRC_BIT9 + MRC_BIT6 + MRC_BIT2);
        RttWrMask   = (MRC_BIT10 + MRC_BIT9);
        RttWr       = (MrReg[mrMR2] & RttWrMask) >> 9;
        RttNom      = (MrReg[mrMR1] & RttNomMask) >> 2;

        for (Index = 0; Index < sizeof (RttNomMRSEncodingConst) / sizeof (RttNomMRSEncodingConst[0]); Index++) {
          if (RttNom == RttNomMRSEncoding[Index]) {
            FinalVal[0] = (S16) (S8) Index;
          }
        }

        for (Index = 0; Index < sizeof (RttWrMRSEncodingConst) / sizeof (RttWrMRSEncodingConst[0]); Index++) {
          if (RttWr == RttWrMRSEncoding[Index]) {
            FinalVal[1] = (S16) (S8) Index;
          }
        }
      }
    }
  }
}

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
  )
{
  MrcDebug const  *Debug;
  MrcOutput       *Outputs;
  char            *EdgeString;
  MrcMarginResult LastResultParam;
  U32             (*LastMargins)[MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES];
  U8              Channel;
  U8              Rank;
  U8              Byte;
  U8              Edge;

  LastResultParam = GetMarginResultType (Param);
  Debug           = &MrcData->SysIn.Inputs.Debug;
  Outputs         = &MrcData->SysOut.Outputs;
  LastMargins     = Outputs->MarginResult;

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "%s Last Margins:\n",
    MarginTypesString[Param]
    );

  EdgeString = ((Param == RdV) || (Param == WrV)) ? "H" : "R";

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Byte\t");
  for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
    if ((1 << Byte) & ByteMask) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "% 10d", Byte);
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nEdge\t");
  for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
    if ((1 << Byte) & ByteMask) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "    L    %s", EdgeString);
    }
  }

  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if ((1 << Rank) & RankMask) {
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if ((1 << Channel) & ChannelMask) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nR%d.C%d\t", Rank, Channel);
          for(Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            if ((1 << Byte) & ByteMask) {
              for (Edge = 0; Edge < MAX_EDGES; Edge++) {
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "% 5d", LastMargins[LastResultParam][Rank][Channel][Byte][Edge]);
              }
            }
          }
        }
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");  // End of table
}
#endif // #if 0

/**
  This function implements switch to print the correct format and data for the 
  OptResultsPerByte struct members.

  @param[in] Debug     - Debug pointer for printing.
  @param[in] Data      - Pointer to OptResultsPerByte struct.
  @param[in] TypeIndex - Member of OptResultsPerByte to print.
  @param[in] TestIndex - Some parameters store multiple test results to be printed.
  @param[in] MidPoint  - Used to convert from zero-based indexing to the selected value

  @retval Nothing.
**/
void
MrcOptResultsPerBytePrint (
  IN const MrcDebug *const Debug,
  IN OptResultsPerByte      *Data,
  IN U8                     TypeIndex,
  IN U8                     TestIndex,
  IN S8                     MidPoint
  )
{
  switch (TypeIndex) {
    case (MrcOptResultBest):
      (TestIndex == 0) ? MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "=== %d ===", Data->Best - MidPoint) :
                         MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
      break;

    case (MrcOptResultGrdBnd):
      (TestIndex == 0) ? MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*** %d ***", Data->GuardBand) :
                         MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
      break;

    case(MrcOptResultOffSel):
      (TestIndex == 0) ? MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "--> %d <--", Data->Best - MidPoint + Data->GuardBand) :
                         MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
      break;

    case (MrcOptResultScale):
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", Data->Scale[TestIndex]);
      break;

    case (MrcOptResultSignal):
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d.%d%s\t", Data->Signal[TestIndex] / 100, Data->Signal[TestIndex] % 100 / 10, "%");
      break;

    case (MrcOptResultNoise):
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d.%d%s\t", Data->Noise[TestIndex] / 100, Data->Noise[TestIndex] % 100 / 10, "%");
      break;

    case (MrcOptResultRatio):
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d.%d\t", Data->Ratio[TestIndex] / 1000, Data->Ratio[TestIndex] % 1000 / 100);
      break;

    case (MrcOptResultMaxPost):
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", Data->MaxPost[TestIndex]);
      break;

    case (MrcOptResultMinPost):
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", Data->MinPost[TestIndex]);
      break;

    case (MrcOptResultTicks):
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d.%d\t", Data->Ticks[TestIndex] / 10, Data->Ticks[TestIndex] % 10);
      break;

    case (MrcOptResultSnrTot):
      (TestIndex == 0) ? MRC_DEBUG_MSG (
                  Debug,
                  MSG_LEVEL_NOTE,
                  " %d.%d%s\t",
                  (U32) Data->SNRTotal / 100,
                  (U32) Data->SNRTotal % 100 / 10,
                  "%"
                  ) : MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
      break; // assuming we dont exceed 32 bits

    default:
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "OptResultPerByteDbgStr Switch exceeded number of cases defined\n");
  }
}


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
void
PrintCalcResultTableCh (
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
  )
{
  const MrcDebug    *Debug;
  OptResultsPerByte *data;
  S8                Off;
  S8                Start;
  S8                Stop;
  U8                i;
  U8                j;
  U8                b;
  U8                FirstByte;
  U8                NumBytes;
  U8                NumTestPlus;
  U32               Result;
  BOOL              Format64Results;
  U8                Param;

  Format64Results = 1;
  //
  // Display result in %/Delta , 0-displat raw 64bit result in HEX
  //
  Debug = &MrcData->SysIn.Inputs.Debug;
  Start = (!IncEnds);
  Stop  = NumOffsets - (!IncEnds);
  if (noPrint) {
    return ;

  }

  FirstByte = (Nibble) ? 4 : 0;
  NumBytes  = FirstByte + 4 + Nibble * MrcData->SysOut.Outputs.SdramCount % 8;
  if (perCh) {
    NumBytes = 1;
  }

  NumTestPlus = (TrendLine) ? NumTest + 1 : NumTest;

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "\n<======== optimize %s ========>Plot results ",
    TOptParamOffsetString[OptParam]
    );
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "<Channel=%d><rank/s=0x%x><Nibble=%s> across settings :(Start=%d,Stop=%d)\n",
    Channel,
    Ranks,
    (Nibble) ? "High" : "Low",
    Start - MidPoint,
    Stop - MidPoint - 1
    );
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Bytes\t");
  for (b = FirstByte; b < NumBytes; b++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", b);
    for (i = 0; i < NumTestPlus + 1; i++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t"); // tab insertion
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n"); // row end here !
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Offset\t"); // row starts here !
  if (OptPower[Stop - 1] != 0) {//WA: need to add param to enable this print
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t", 3 + TOptParamOffsetString[OptParam]);
  }

  for (b = FirstByte; b < NumBytes; b++) {
    for (i = 0; i < NumTest; i++) {
      //
      // Test types header
      //
      Param = TestList[i];
      if (Param > CmdV) {
        Param = (Param % 16) + 4;
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t", MarginTypesString[Param]);
    }

    if (TrendLine) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t", "T.line");
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Opt.func\t"); // more header..
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\n"); // row end here !
  for (Off = Start; Off < Stop; Off++) {
    //
    // row starts here !
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", Off - MidPoint);
    if (OptPower[Stop - 1] != 0) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", OptPower[Off]);
    }

    for (b = FirstByte; b < NumBytes; b++) {
      if (b < MAX_SDRAM_IN_DIMM) {
        data = &calcResultSummary[Channel][b];
      } else {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "Error: calcResultSummary array out of bounds! %d > %d \n",
          b,
          MAX_SDRAM_IN_DIMM - 1
          );
        return;
      }

      for (i = 0; i < NumTestPlus; i++) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", data->Margins[i][Off].EW);
      }

      if (Format64Results) {
        Result = (U32) (MrcOemMemoryDivideU64ByU64 (MrcOemMemoryMultiplyU64ByU32 (data->Result[Off], 200), data->MaxR));
        Result /= 2;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t\t", Result);
      }

      if (!Format64Results) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%08x-%08x\t\t",
          (U32) MrcOemMemoryRightShiftU64 (data->Result[Off],
                32),
          (U32) (data->Result[Off])
          );
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n"); // row end here !
  }

  for (i = 0; i < (sizeof (OptResultDbgStrings) / sizeof (*OptResultDbgStrings)); i++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t", OptResultDbgStrings[i]);
    for (b = FirstByte; b < NumBytes; b++) {
      if (b < MAX_SDRAM_IN_DIMM) {
        data = &calcResultSummary[Channel][b];
      } else {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "Error: calcResultSummary array out of bounds! %d > %d \n",
          b,
          MAX_SDRAM_IN_DIMM - 1
          );
        return;
      }

      for (j = 0; j < NumTestPlus; j++) {
        MrcOptResultsPerBytePrint (Debug, data, i, j, MidPoint);
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t"); // tab insertion
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  } // row end here !
  return;
}

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
void
PrintODTResultTable (
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
  )
{
  const char        *OdtStrings[] = {
    "RttNom0",
    "RttNom1",
    "RttWr0",
    "RttWr1",
    "RdOdt",
    "Pwr[mW]",
    "Cpu Rd",
    "Dim Rd",
    "Cpu Wr",
    "DimW-T",
    "DimW-NT",
    "ACPower"
  };
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  OptResultsPerByte *data;
  S8                Off;
  S8                Start;
  S8                Stop;
  U8                i;
  U8                j;
  U8                b;
  U8                r;
  U8                FirstByte;
  U8                NumBytes;
  U8                NumTestPlus;
  U8                *TestList;
  U8                Param;
  U32               Result;
  U8                OptResultType;
  BOOL              Format64Results; // Display result in %/MaxR , 0-display raw 64bit result in HEX
  BOOL              printOptSetting;
  U64               delta;

  Format64Results = 1;
  printOptSetting = 1;
  Debug           = &MrcData->SysIn.Inputs.Debug;
  Outputs         = &MrcData->SysOut.Outputs;
  Start           = (!IncEnds);
  Stop            = NumOffsets - (!IncEnds);
  TestList        = DimmOptPoints[0].TestList;
  FirstByte       = (Nibble) ? 4 : 0;
  NumBytes        = FirstByte + 4 + Nibble * MrcData->SysOut.Outputs.SdramCount % 8;
  Ranks          &= Outputs->Controller[0].Channel[Channel].ValidRankBitMask;

  if (perCh) {
    NumBytes = 1;
  }

  NumTestPlus = (TrendLine) ? DimmOptPoints[0].NumTests + 1 : DimmOptPoints[0].NumTests;
  //
  // RttNomOffset = (MrcData->Outputs.Channel[Channel].DimmCount == 1) ? 0 : RttOffset; // if 2DPC - RttNom 40,30,20 Ohms
  //
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "\n<======== optimize %s ========>Plot results ",
    TOptParamOffsetString[OptParam]
    );
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "<Channel=%d><rank/s=0x%x> across settings :(Start=%d,Stop=%d)\n",
    Channel,
    Ranks,
    Start - MidPoint,
    Stop - MidPoint - 1
    );
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ODT\t");
  // for (b = 0; b < (sizeof(OdtStrings)/sizeof(*OdtStrings)); b++) {
  //  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t",OdtStrings[b] );
  //  for (i = 0; i < NumTestPlus+1; i++) MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");//tab insertion
  // }
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");//row end here!
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Offset\t"); // row starts here!
  for (b = 0; b < (sizeof (OdtStrings) / sizeof (*OdtStrings)); b++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t", OdtStrings[b]);
  }

  for (i = 0; i < DimmOptPoints[0].NumTests; i++) {
    //
    // Test types header
    //
    Param = TestList[i];
    if (Param > CmdV) {
      Param = (Param % 16) + 4;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t", MarginTypesString[Param]);
  }

  if (TrendLine) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t", "T.line");
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Opt.func\t"); // more header
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\n");

  for (Off = Start; Off < Stop; Off++) {
    //
    // row starts here !
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", Off - MidPoint);
    for (b = 0; b < (sizeof (OdtStrings) / sizeof (*OdtStrings)); b++) {
      if (b == 0) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", ActualDimmOdt[DimmOptPoints[Off].ODTSet.RttNom[Channel][0]]);
      }

      if (b == 1) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", ActualDimmOdt[DimmOptPoints[Off].ODTSet.RttNom[Channel][1]]);
      }

      if (b == 2) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", ActualDimmOdt[DimmOptPoints[Off].ODTSet.RttWr[Channel][0]]);
      }

      if (b == 3) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", ActualDimmOdt[DimmOptPoints[Off].ODTSet.RttWr[Channel][1]]);
      }

      if (b == 4) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", CalcRdOdt (MrcData, DimmOptPoints[Off].ODTSet.GRdOdt));
      }

      if (b == 5) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d.%d\t",
          DimmOptPoints[Off].PowerCalc.TotPwr / 10,
          DimmOptPoints[Off].PowerCalc.TotPwr % 10
          );
      }

      if (b == 6) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d.%d\t",
          DimmOptPoints[Off].PowerCalc.CpuPwrRd / 10,
          DimmOptPoints[Off].PowerCalc.CpuPwrRd % 10
          );
      }

      if (b == 7) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d.%d\t",
          DimmOptPoints[Off].PowerCalc.DimmPwrRd / 10,
          DimmOptPoints[Off].PowerCalc.DimmPwrRd % 10
          );
      }

      if (b == 8) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d.%d\t",
          DimmOptPoints[Off].PowerCalc.CpuPwrWr / 10,
          DimmOptPoints[Off].PowerCalc.CpuPwrWr % 10
          );
      }

      if (b == 9) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d.%d\t",
          DimmOptPoints[Off].PowerCalc.DimmPwrWrT / 10,
          DimmOptPoints[Off].PowerCalc.DimmPwrWrT % 10
          );
      }

      if (b == 10) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d.%d\t",
          DimmOptPoints[Off].PowerCalc.DimmPwrWrNT / 10,
          DimmOptPoints[Off].PowerCalc.DimmPwrWrNT % 10
          );
      }

      if (b == 11) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d.%d\t",
          DimmOptPoints[Off].PowerCalc.ACPower / 10,
          DimmOptPoints[Off].PowerCalc.ACPower % 10
          );
      }
    }

    data = calcResultSummary;
    for (i = 0; i < NumTestPlus; i++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", data->Margins[i][Off].EW);
    }

    delta = data->MaxR - data->MinR + 1; // +1 to not divide by 0
    if (Format64Results) {
      Result = (U32) (MrcOemMemoryDivideU64ByU64 (MrcOemMemoryMultiplyU64ByU32 (data->Result[Off], 200), data->MaxR));
      Result /= 2;
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t\t", Result);
    } else {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "%08x-%08x\t\t",
        (U32) MrcOemMemoryRightShiftU64 (data->Result[Off],
              32),
        (U32) (data->Result[Off])
        );
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n"); // row end here !
    if (printOptSetting) {
      for (i = 0; i < DimmOptPoints[0].OptParamTestListSize; i++) {
        OptResultType = GetOptResultType (DimmOptPoints[0].OptParamTestList[i]);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s  \t", TOptParamOffsetString[DimmOptPoints[0].OptParamTestList[i]]);
        if ((OptResultType == RxEqOfft) || (OptResultType == TxEqOfft)) {
          //
          // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
          //
          for (r = 0; r < MAX_RANK_IN_CHANNEL; r++) {
            if (!(Ranks & (0x1 << r))) {
              continue;
            }

            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "<Rank %d>|", r);
            for (b = 0; b < Outputs->SdramCount; b++) {
              MRC_DEBUG_MSG (
                Debug,
                MSG_LEVEL_NOTE,
                "%02d|",
                DimmOptPoints[Off].BestOptOff[OptResultType][r].Offset[Channel][b]
                );
            }

            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t");
          }
        } else {
          for (b = 0; b < Outputs->SdramCount; b++) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "%d|",
              DimmOptPoints[Off].BestOptOff[OptResultType][0].Offset[Channel][b]
              );
          }
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n"); // row end here !
      }
    }

  }

  for (i = 0; i < (sizeof (OptResultDbgStrings) / sizeof (*OptResultDbgStrings)); i++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\t", OptResultDbgStrings[i]);
    for (b = FirstByte; b < NumBytes; b++) {
      data = calcResultSummary;
      for (j = 0; j < NumTestPlus; j++) {
        MrcOptResultsPerBytePrint (Debug, data, i, j, MidPoint);
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t");  // tab insertion
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  } // row end here !
}

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
void
PrintResultTableByte4by24 (
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
  )
{
  const MrcDebug  *Debug;
  MrcOutput       *Outputs;
  U8              Channel;
  U8              Byte;
  S8              Off;
  S8              Start;
  S8              Stop;

  Debug   = &MrcData->SysIn.Inputs.Debug;
  Outputs = &MrcData->SysOut.Outputs;
  Start   = -MidPoint;
  Stop    = OffsetsNum - MidPoint - 1;
  if (Param > CmdV) {
    Param = (Param % 16) + 4;
  }

  if (noPrint) {
    return;

  }

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "\nTest number : %d - %s ,Plot results across OptParam=%s settings:(Start=%d,Stop=%d) w/ power limits(width): %d \nChannel\t0                1\nByte\t",
    TestNum,
    MarginTypesString[Param],
    TOptParamOffsetString[OptParam],
    Start,
    Stop,
    PowerLimits[TestNum]
    );
  if (Outputs->SdramCount == 8) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "0\t1\t2\t3\t4\t5\t6\t7\t0\t1\t2\t3\t4\t5\t6\t7");
  } else if (Outputs->SdramCount == 9) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "0\t1\t2\t3\t4\t5\t6\t7\t0\t1\t2\t3\t4\t5\t6\t7\t8");
  }
  //
  // Sweep through OpParam settings
  //
  for (Off = Start; Off < Stop + 1; Off++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n  %d:\t", Off);
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      //
      // spaces for non populated channel
      //
      if (!((0x1 << Channel) & ChMask)) {
        if (Channel == 0) {
          if (Outputs->SdramCount == 8) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t\t\t");
          } else {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "                  ");
          }
        }

        continue;
      }

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        if (Edges > 1) {
          if (Byte < MAX_SDRAM_IN_DIMM) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "%d-%d\t",
              ResultArray[TestNum][Off - Start][Channel][Byte][0],
              ResultArray[TestNum][Off - Start][Channel][Byte][1]
              );
          }
        } else {
          if (Byte < MAX_SDRAM_IN_DIMM) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", ResultArray[TestNum][Off - Start][Channel][Byte][0]);
          }
        }
      }
    }
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");  // New line after the end of the table

  return;
}
#endif // MRC_DEBUG_PRINT

/**
  This function returns the UPM or PWR limit value for the specified parameter

  @param[in] MrcData   - Pointer to MRC global data.
  @param[in] Param     - Margin type
  @param[in] LimitType - Type of limit: UpmLimit or PowerLimit

  @retval Returns the UPM or PWR limit
**/
U16
UpmPwrLimitValue (
  IN MrcParameters *const MrcData,
  IN U8                   Param,
  IN U8                   LimitType
  )
{
  MrcOutput              *Outputs;
  MrcUpmPwrRetrainLimits *MrcLimits;
  U32                    Index;
  U16                    Limit;
#ifdef ULT_FLAG
  U8                     Channel;
#endif // ULT_FLAG

  Limit     = 0;
  Outputs   = &MrcData->SysOut.Outputs;
  MrcLimits = Outputs->UpmPwrRetrainLimits.Pointer;

  for (Index = 0; Index < MRC_NUMBER_UPM_PWR_RETRAIN_MARGINS; Index++) {
    if (Param == MrcLimits[Index].Param) {
      Limit = MrcLimits[Index].ParamLimit[LimitType];
      break;
    }
  }

#ifdef ULT_FLAG
  if ((MrcData->SysIn.Inputs.CpuModel == cmHSW_ULT) && 
      (Outputs->DdrType == MRC_DDR_TYPE_DDR3) &&
      (Param == WrV) &&
      (LimitType != RetrainLimit)
      ) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        if (Outputs->Controller[0].Channel[Channel].Dimm[0].ReferenceRawCard == rcF) {
          Limit += 200;  // Add 20 ticks for WrV on HSW ULT with DDR3L and raw card F
          break;
        }
      }
    }
  }
#endif // ULT_FLAG

  return Limit;
}

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
  )
{
  MrcUpmPwrRetrainLimits  *MrcLimits;
  U32                     Index;
  S32                     UpdatedValue;

  MrcLimits     = MrcData->SysOut.Outputs.UpmPwrRetrainLimits.Pointer;
  UpdatedValue  = 0;

  for (Index = 0; Index < MRC_NUMBER_UPM_PWR_RETRAIN_MARGINS; Index++) {
    if (Param == MrcLimits[Index].Param) {
      UpdatedValue = MrcLimits[Index].ParamLimit[LimitType];
      break;
    }
  }

  UpdatedValue += Offset;
  UpdatedValue = MAX (UpdatedValue, 0);
  UpdatedValue = MIN (UpdatedValue, 0xFFFF);

  MrcLimits[Index].ParamLimit[LimitType] = (U16) UpdatedValue;

  return (U16) UpdatedValue;
}

/**
  This function returns the Actual Cpu Driver Impedance (1 segment) in ohm.

  @param[in] MrcData - Pointer to MRC global data.
  @param[in] Offset  - Vref Offset (+-8).

  @retval Returns the CPU driver impedance value (for 1 segment)
**/
U16
CalcDrvImp (
  IN MrcParameters *const MrcData,
  IN S8              Offset
  )
{
  U16 Result;
  U8  Rext;

  Rext = 75;

#ifdef ULT_FLAG
  if (MrcData->SysIn.Inputs.CpuModel == cmHSW_ULT) {
    Rext = 120;  // RCOMP1 resistor is 120 Ohm on HSW-ULT boards
  }
#endif // ULT_FLAG

  //
  // If Offset == -32, return 0;
  //
  if (Offset == -32) {
    Result = 0;
  } else {
    Result = Rext * (32 - Offset) / (32 + Offset);
  }

  return Result;
}

/**
  This function returns the Actual Cpu Odt termination in ohm.

  @param[in] MrcData - Pointer to MRC global data.
  @param[in] Offset  - Vref Offset (+-16).

  @retval Returns the Odt termination value.
**/
U16
CalcRdOdt (
  IN MrcParameters *const MrcData,
  IN S8                   Offset
  )
{
  U16 Result;
  U8  Rext;

#ifdef ULT_FLAG
  if (MrcData->SysIn.Inputs.CpuModel == cmHSW_ULT) {
    Rext = 100; // 200 / 2
  } else
#endif //ULT_FLAG
  {
    Rext = 50; // 100 / 2
  }

  Result = (Rext * 96 / (Offset + 48) - Rext);

  return Result;
}

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
U32
CalcOptPower (
  IN MrcParameters   *MrcData,
  IN U8              Channel,
  IN U8              Rank,
  IN U8              Byte,
  IN U8              OptParam,
  IN S8              Offset,
  IN S8              CurrentComp,
  IN BOOL            ReadHost
  )
{
  U32                                       Power;
  U16                                       Rleg;
  S8                                        StatLegs;
  U8                                        OdtLegsDis;
  S8                                        CurrentVref;
  U8                                        RxVselect;
  U8                                        RxCBSelect;
  S8                                        RxFselect;
  U8                                        RxDefault;
  extern const U8                           RxBiasTable[2][5][4];
  U8                                        RxPowerScale[] = { 33, 66, 88, 100, 133, 166, 200, 233 };
  U32                                       Vcc;
  U32                                       CPURXPower;
  MrcVddSelect                              Vdd;
  MrcOutput                                 *Outputs;
  MrcChannelOut                             *ChannelOut;
  const MrcInput                            *Inputs;
  const MrcDebug                            *Debug;
  DDRCOMP_CR_DDRCRCOMPCTL0_STRUCT           DdrCrCompCtl0;
  DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_STRUCT DdrCrDataOffsetComp;
  DDR3_MODE_REGISTER_1_STRUCT               Ddr3ModeRegister1;
  DDRCOMP_CR_DDRCRDATACOMP0_STRUCT          DdrCrDataComp0;
  DDRCOMP_CR_DDRCRDATACOMP1_STRUCT          DdrCrDataComp1;
#ifdef ULT_FLAG
  BOOL                                      Lpddr;
  U16                                       DimmRon;
  extern const U8                           RxBiasTableUlt[2][3][4];

  Lpddr = (MrcData->SysOut.Outputs.DdrType == MRC_DDR_TYPE_LPDDR3);
#endif //ULT_FLAG

  Inputs                   = &MrcData->SysIn.Inputs;
  Debug                    = &Inputs->Debug;
  Outputs                  = &MrcData->SysOut.Outputs;
  ChannelOut               = &Outputs->Controller[0].Channel[Channel];
  DdrCrDataComp0.Data      = 0;
  DdrCrDataComp1.Data      = 0;
  DdrCrDataOffsetComp.Data = 0;
  Power                    = 0;
  Vdd                      = Outputs->VddVoltage[Inputs->MemoryProfile];

  if ((OptParam == OptWrDS) || (OptParam == OptRdOdt) || (OptParam == OptSComp)) {
    DdrCrDataOffsetComp.Data = ChannelOut->DataCompOffset[Byte];
    if (OptParam == OptRdOdt) {
      DdrCrDataComp1.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG);
    } else {
      DdrCrDataComp0.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP0_REG);
    }
  }

  if (OptParam == OptWrDS) {
    DdrCrCompCtl0.Data = Outputs->CompCtl0;
    //
    // Added Driver RCOMP Vref for driver impedance calculation
    //
    CurrentVref = (S8) DdrCrCompCtl0.Bits.DqDrvVref;
    if (CurrentVref & 0x8) {
      CurrentVref -= 0x10;  // 2's complement
    }

    if (ReadHost) {
      CurrentComp = (S8) DdrCrDataComp0.Bits.RcompDrvUp;
      Offset      = (S8) DdrCrDataOffsetComp.Bits.DqDrvUpCompOffset;

      if (Offset & 0x20) {
        Offset-= 0x40; // 2's complement
      }
    }

    StatLegs = 3 * 4 * 4; // seg*legs*4 - for calc set to 48
    Rleg  = CalcDrvImp (MrcData, CurrentVref) / 3 * (StatLegs + 3 * CurrentComp); // RCOMP Vref added to Rleg calculation
    Power = Rleg / (StatLegs + 3 * (CurrentComp + (Offset))); // in ohm
  }

  if (OptParam == OptRdOdt) {
    DdrCrCompCtl0.Data  = Outputs->CompCtl0;
    OdtLegsDis          = (U8) DdrCrCompCtl0.Bits.DisableOdtStatic;
    CurrentVref         = (S8) DdrCrCompCtl0.Bits.DqOdtVref;
    StatLegs            = 4 * 4;  // we enable only 1/3 segment for odt 4 legs time 4

    if (CurrentVref & 0x10) {
      CurrentVref -= 0x20;  // 2's complement
    }

    if (ReadHost) {
      CurrentComp = (S8) DdrCrDataComp1.Bits.RcompOdtUp;
      Offset      = (S8) DdrCrDataOffsetComp.Bits.DqOdtUpCompOffset;
      if (Offset & 0x10) {
        Offset-= 0x20; // 2's complement
      }
    }
    //
    // Avoid division by zero.
    //
    if (CurrentComp == 0) {
      CurrentComp = 1;
    }
    Rleg  = CalcRdOdt (MrcData, CurrentVref) * (StatLegs * (!OdtLegsDis) + CurrentComp);
    Power = Rleg / (StatLegs * (!OdtLegsDis) + (CurrentComp + (Offset)));  // in ohm
  }

  if (OptParam == OptSComp) {
    if (ReadHost) {
      Offset = (S8) DdrCrDataOffsetComp.Bits.DqSlewRateCompOffset;
      if (Offset & 0x10) {
        Offset -= 0x20; // 2's complement
      }
    }
    Power = 50 + Offset; // simple linear T-line
  }

  if (OptParam == OptTxEq) {
    Power = Offset; // simple linear T-line
  }

  if (OptParam == OptRxEq) {
    Power = 100 + (5 * (Offset / 5)); // modulo 5 T-line
  }

  if (OptParam == OptDimmRon) {
    //
    // calc the DimmRon [ohm]
    //
    if (ReadHost) {
      if (Rank < MAX_RANK_IN_CHANNEL) {
#ifdef ULT_FLAG
        if (Lpddr) {
          DimmRon = ChannelOut->Dimm[Rank / 2].Rank[Rank % 2].MR[mrMR3];
          Offset  = (U8) 0xF & (DimmRon - 1); //{0x1,0x2,0x3}; //{34,40,48};
        } else
#endif //ULT_FLAG
        {
          Ddr3ModeRegister1.Data  = ChannelOut->Dimm[Rank / 2].Rank[Rank % 2].MR[mrMR1];
          Offset                  = (U8) Ddr3ModeRegister1.Bits.ODImpedanceLow;
        }
      } else {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "Error: ChannelOut->Dimm array out of bounds! %d > %d\n",
          Rank / 2,
          MAX_DIMMS_IN_CHANNEL - 1
          );
        return 0;
      }
    }

#ifdef ULT_FLAG
    if (Lpddr) {
      if (Offset > 6) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "Offset %d causes negative unsigned number or divide by 0.  Dividing by 1.\n",
          Offset
          );
        Offset = 6;
      }

      Power = 240 / (7 - Offset);
    } else
#endif //ULT_FLAG
    {
      if (Offset < -5) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "Offset %d causes negative unsigned number or divide by 0.  Dividing by 1.\n",
          Offset
          );
        Offset = -5;
      }

      Power = 240 / (6 + Offset);
    }
  }

  if (OptParam == OptRxBias) {
    //
    // RX BIAS calculations
    //
    Vcc       = 1050;
    RxVselect = 0;
    if (Vdd > VDD_1_35) {
      RxVselect = 1;  // Set HiVdd bit if Vdd is over 1.35v
    }
    //
    // RX BIAS calculations
    //
    GetRxFselect (MrcData, &RxFselect, &RxCBSelect);

#ifdef ULT_FLAG
    if (MrcData->SysIn.Inputs.CpuModel == cmHSW_ULT) {
      RxFselect = MIN (RxFselect, RXF_SELECT_MAX_ULT);              // Maximum 1600 MHz
      RxDefault = RxBiasTableUlt[RxVselect][RxFselect][RxCBSelect]; // Read setting from array lookup table
    } else
#endif // ULT_FLAG
    {
      RxDefault = RxBiasTable[RxVselect][RxFselect][RxCBSelect];  // Read setting from array lookup table
    }

    CPURXPower = Vdd * 1200 / 1000 + Vcc * 1250 / 1000; // mW
    CPURXPower /= 1000;
    if (ReadHost) {
      Offset = (U8) ChannelOut->DqControl1[Byte].Bits.RxBiasCtl;
    }

    CPURXPower  = (RxPowerScale[Offset] * CPURXPower) / RxPowerScale[RxDefault];
    Power       = (U16) CPURXPower;
  }

  return Power;
}

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
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcChannelOut     *ChannelOut;
  MrcControllerOut  *ControllerOut;
  MrcStatus         Status;
  U32               (*MarginByte)[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES];
  U32               BERStats[4];
  U16               SaveMargin[63][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES]; //40 Points X Ch X Byte X Hi/Lo
  BOOL              Eye[63][108];
  BOOL              Lines[108];
  U8                MaxH=108;
  U8                MaxW=63;
  U8                i,j;
  U16               MinEdge;
  U16               Mode;
  U8                ResultTypeV = 0;
  U8                ChBitMask;
  U8                Byte;
  U8                Rank;
  U8                Edge;
  U8                FirstRank;
  U8                NumBytes;
  U8                BMap[9];  // Need by GetBERMarginByte
  U8                MaxMarginV;
  U8                localR[MAX_CHANNEL];
  U8                Rep;
  S8                Index;
  U8                IndexOff;
  S8                Off;
  U8                byteMax[MAX_CHANNEL];
  U32               Offset;
  U64               CrValue64;

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  MarginByte    = &Outputs->MarginResult;
  Ranks        &= Outputs->ValidRankMask;
  ControllerOut = &Outputs->Controller[0];
  IndexOff      = 0;
  CrValue64     = 0x0ULL; //64 bit Data bit mask

  MrcOemMemorySet ((U8 *) localR, 0, sizeof(localR));
  MrcOemMemorySet ((U8 *) Eye, 0, sizeof(Eye));
  MrcOemMemorySet ((U8 *) Lines, 0, sizeof(Lines));
  MrcOemMemorySet ((U8 *) SaveMargin, 0, sizeof(SaveMargin));
  MrcOemMemorySetDword (BERStats, 0, sizeof(BERStats) / sizeof (U32));
  for (Byte = 0; Byte < sizeof (BMap) / sizeof (BMap[0]); Byte++) {
    BMap[Byte] = Byte;
  }

  Outputs->EnDumRd = 0;
  SetupIOTestBasicVA(MrcData, 1<<Channel, LoopCount, 0, 0, 0,8);  //set test to all channels
  //
  // Select All Ranks for REUT test
  //
  ChannelOut      = &ControllerOut->Channel[Channel];
  localR[Channel] = ChannelOut->ValidRankBitMask & Ranks;
  //
  // use ChBitMask from here down - if ch is set that mean at least 1 rank for testing, also remove ch w/o active ranks
  //
  ChBitMask = SelectReutRanks (MrcData, Channel, localR[Channel], 0);

  if (ChBitMask == 0) {
    return ;
  }

  for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
    ChannelOut->DataOffsetTrain[Byte] = 0;
  }
  //
  // Find the first selected rank
  //
  FirstRank = 0;
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if ((1 << Rank) & localR[Channel]) {
      FirstRank = Rank;  // could be in any channel
      break;
    }
  }
  //
  // Store margin results for
  //
  NumBytes = (U8) Outputs->SdramCount;

  //
  // Loop through all Test Params and Measure Margin
  // Find MaxMargin for this channel
  //
  byteMax[Channel] = Stop;
  if (ParamT == RdT) {
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      byteMax[Channel] = MrcCalcMaxRxMargin (MrcData, Channel, Ranks, Byte, 0, byteMax[Channel]);
    }
  }

  Offset = MCHBAR_CH0_CR_REUT_CH_ERR_DATA_MASK_REG +
    ((MCHBAR_CH1_CR_REUT_CH_ERR_DATA_MASK_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_MASK_REG) * Channel);

  MrcWriteCR64 (MrcData, Offset, CrValue64);
  MaxMarginV = MAX_POSSIBLE_VREF;
  if (MAX_POSSIBLE_TIME < Stop) {
    Stop = MAX_POSSIBLE_TIME;
  }

  if (-MAX_POSSIBLE_TIME > Start) {
    Start = -MAX_POSSIBLE_TIME;
  }

  IndexOff = MaxW / 2 + Start;
  //
  // No need to search too far
  //
  if (MaxMarginV > SearchLimits) {
    MaxMarginV = (U8) (SearchLimits);
  }

  for (Off = Start; Off < Stop + 1; Off++) {
    Index = Off - Start;
    //
    // change margin ParamT
    //
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      Status = ChangeMargin (MrcData, ParamT, Off, 0, 0, Channel, localR[Channel], Byte, 0, 1, 0, MrcRegFileStart);
    }
    ResultTypeV = GetMarginResultType (ParamV); // rxv=0 rxt=1
    //
    // Assign to last pass margin results by reference
    // get lowest margin from all ch/rankS/byte save in FirstRank
    //
    Status = GetMarginByte (
              MrcData,
              Outputs->MarginResult,
              ParamV,
              FirstRank,
              Ranks
              );
    for (Rep = 0; Rep < Repeats; Rep++) {
      //
      // Run Margin Test - margin_1d with chosen param
      // run on all ranks but change param only for firstRank??
      //
      Mode = 0;
      Status = MrcGetBERMarginByte (
                MrcData,
                Outputs->MarginResult,
                ChBitMask,
                FirstRank,
                FirstRank,
                ParamV,
                Mode,
                BMap,
                1,
                MaxMarginV,
                0,
                BERStats
                );
      //
      // Record Results
      //
      for (Edge = 0; Edge < MAX_EDGES; Edge++) {
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          if ((Index > 62) || (Index < 0)) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Error: SaveMargin array out of bounds! %d", Index);
            return;
          }

          if (Rep == 0) {
            SaveMargin[Index][Channel][Byte][Edge] = 0;
          }

          SaveMargin[Index][Channel][Byte][Edge] += (U16) (*MarginByte)[ResultTypeV][FirstRank][Channel][Byte][Edge];
        }
      }
    }

    for (Edge = 0; Edge < MAX_EDGES; Edge++) {
      MinEdge = 0xFFFF;
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        if ((Index > 62) || (Index < 0)) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Error: SaveMargin array out of bounds! %d", Index);
          return;
        }

        SaveMargin[Index][Channel][Byte][Edge] /= Repeats;
        if (MinEdge > SaveMargin[Index][Channel][Byte][Edge]) {
          MinEdge = SaveMargin[Index][Channel][Byte][Edge];
        }
      }

      if (((Index + IndexOff) > 62) ||
          ((Index + IndexOff) < 0) ||
          ((MaxH / 2 - (MinEdge - 1) / 10) > 107) ||
          ((MaxH / 2 - (MinEdge - 1) / 10) < 0) ||
          ((MaxH / 2 + (MinEdge - 1) / 10) > 107) ||
          ((MaxH / 2 + (MinEdge - 1) / 10) < 0)
          ) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Error: Eye or Lines array out of bounds!\n");
        return;
      }

      if (Edge) {
        Eye[Index + IndexOff][MaxH / 2 - (MinEdge - 1) / 10]  = 1;
        Lines[MaxH / 2 - (MinEdge - 1) / 10]                  = 1;
      } else {
        Eye[Index + IndexOff][MaxH / 2 + (MinEdge - 1) / 10]  = 1;
        Lines[MaxH / 2 + (MinEdge - 1) / 10]                  = 1;
      }
    }
  }//end of offset
  //
  // Print the box
  //
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Plot Eye across ParamT = %s ParamV = %s settings:(Start=%d,Stop=%d) LC = %d  Channel = %d Ranks = 0x%x\n",
    MarginTypesString[ParamT],
    MarginTypesString[ParamV],
    Start,
    Stop,
    LoopCount,
    Channel,
    Ranks
    );
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t------------------------------- +++++++++++++++++++++++++++++++\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t332222222222111111111100000000000000000001111111111222222222233\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Vref\t109876543210987654321098765432101234567890123456789012345678901\n");
  for (i = 0; i < MaxH; i++) {
    if (Lines[i]) {
      //
      // print only fail lines
      //
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%3d:\t", MaxH / 2 - i); // per ch
      for (j = 0; j < MaxW; j++) {
        if (Eye[j][i]) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s", "#"); // per ch
        } else {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s", ((j == (MaxW) / 2) || (i == (MaxH) / 2)) ? "+" : " "); // per ch
        }
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");//per ch
    }
  }
  //
  // Clean up after test
  //
  ChannelOut = &ControllerOut->Channel[Channel];
  MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
  Status = ChangeMargin (MrcData, ParamT, 0, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileCurrent);

}

/**
  This function fill the input array (e.g array[ch][rank]) with the power calculation
  per rank/ch for current sys. setting.

  @param[in]     MrcData   - MRC data struct;
  @param[in,out] PwrChRank - Array to fill;

  @retval Nothing
**/
void
CalcSysPower (
  IN     MrcParameters *const MrcData,
  IN OUT MrcPower             PwrChRank[MAX_CHANNEL][MAX_RANK_IN_CHANNEL]
  )
{
  const MrcDebug              *Debug;
  MrcChannelOut               *ChannelOut;
  MrcOutput                   *Outputs;
  MrcOdtPowerSaving           *OdtPowerSaving;
  U8                          Rank;
  U8                          Byte;
  U8                          Channel;
  BOOL                        is1DPC;
  BOOL                        ChCalcDone;
  U16                         ROdtCpu;
  U8                          RonDimm;
  U16                         RonCpu;
  U16                         Rodtdram;
  U16                         Wodtdram;
  U16                         RxBiasPwr;
  U8                          TotalRankCount;
  U32                         PwrAvgRd;
  U32                         PwrAvgWr;
  const U8                    RttNomDic[6] = {0,60,120,40,20,30}; //accordingly to DDR3 spec
  const U8                    RttWrDic[3]  = {0,60,120};          //accordingly to DDR3 spec
  DDR3_MODE_REGISTER_1_STRUCT Ddr3ModeRegister1;
  DDR3_MODE_REGISTER_2_STRUCT Ddr3ModeRegister2;
#ifdef ULT_FLAG
  U16                         LpddrMr3; //dimm DS
  U16                         LpddrMr11; //dimm ODT
  const U8                    LpddrRonDic[4] = {0,34,40,48};
  const U8                    LpddrOdtDic[4] = {0,0,120,240};
#endif // ULT_FLAG

  Outputs         = &MrcData->SysOut.Outputs;
  OdtPowerSaving  = &Outputs->OdtPowerSavingData;
  Debug           = &MrcData->SysIn.Inputs.Debug;
  RxBiasPwr       = 0;
  TotalRankCount  = 0;
  PwrAvgRd        = 0;
  PwrAvgWr        = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChCalcDone  = 0;
    RonCpu      = 0;
    ROdtCpu     = 0;
    RxBiasPwr   = 0;
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      ChannelOut  = &Outputs->Controller[0].Channel[Channel];
      is1DPC      = (ChannelOut->DimmCount == 1);
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
#ifdef ULT_FLAG
        if (MrcData->SysOut.Outputs.DdrType == MRC_DDR_TYPE_LPDDR3) {
          if (Rank >= MAX_RANK_IN_DIMM) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Error: ChannelOut array out of bounds!\n");
            return ;
          }

          LpddrMr3  = ChannelOut->Dimm[0].Rank[Rank].MR[mrMR3];
          LpddrMr11 = ChannelOut->Dimm[0].Rank[Rank].MR11;
          RonDimm   = LpddrRonDic[0x3 & LpddrMr3];
          Wodtdram  = LpddrOdtDic[0x3 & LpddrMr11];
          Rodtdram  = 0x3FFF; // put 8k ohm as infinity - in lpddr there is no nomOdt
        } else
#endif // ULT_FLAG
        {
          Ddr3ModeRegister1.Data  = ChannelOut->Dimm[Rank / 2].Rank[Rank % 2].MR[mrMR1];
          Ddr3ModeRegister2.Data  = ChannelOut->Dimm[Rank / 2].Rank[Rank % 2].MR[mrMR2];
          RonDimm                 = 240 / (6 + (U8) Ddr3ModeRegister1.Bits.ODImpedanceLow);
          Rodtdram = RttNomDic[(Ddr3ModeRegister1.Bits.OdtRttValueHigh << 2) |
                               (Ddr3ModeRegister1.Bits.OdtRttValueMid << 1) |
                                Ddr3ModeRegister1.Bits.OdtRttValueLow];
          Wodtdram = RttWrDic[Ddr3ModeRegister2.Bits.DynamicOdt];
        }
        if (!ChCalcDone){
           //
           //Ron CPU - take average all bytes only per ch
           //
           for (Byte = 0; Byte < Outputs->SdramCount; Byte++){
             RonCpu    += (U16) CalcOptPower (MrcData, Channel, 0, Byte, OptWrDS, 0, 0, 1);//read from host
             ROdtCpu   += (U16) CalcOptPower (MrcData, Channel, 0, Byte, OptRdOdt, 0, 0, 1);//read from host
             RxBiasPwr += (U16) CalcOptPower (MrcData, Channel, 0, Byte, OptRxBias, 0, 0, 1);//read from host
           }
           RonCpu    /= (U16) Outputs->SdramCount;
           ROdtCpu   /= (U16) Outputs->SdramCount;
           RxBiasPwr /= (U16) Outputs->SdramCount;
           ChCalcDone = 1;
        }

        if (is1DPC) {
          //
          // in 1DPC channel always use only one of the terminations
          //
          if (Wodtdram == 0) {
            Wodtdram  = Rodtdram;
            Rodtdram  = 0x3FFF; // put 8k ohm as infinity
          } else {
            Rodtdram  = 0x3FFF; // put 8k ohm as infinity
          }
        }

        if (Rodtdram == 0) {
          Rodtdram = 0x3FFF;
        }

        if (Wodtdram == 0) {
          Wodtdram = Rodtdram; // in 2DPC where RttW=0
        }

        CalcPower (MrcData, &PwrChRank[Channel][Rank], RonCpu, RonDimm, ROdtCpu, Rodtdram, Wodtdram);
        //
        // add RxBias to CPU and Total
        //
        PwrChRank[Channel][Rank].CpuPower += RxBiasPwr;
        PwrChRank[Channel][Rank].TotPwr   += RxBiasPwr;
        PwrAvgRd += PwrChRank[Channel][Rank].CpuPwrRd + PwrChRank[Channel][Rank].DimmPwrRd +
                    PwrChRank[Channel][Rank].ACPowerRd;
        PwrAvgWr += PwrChRank[Channel][Rank].CpuPwrWr + PwrChRank[Channel][Rank].DimmPwrWrT +
                    PwrChRank[Channel][Rank].DimmPwrWrNT + PwrChRank[Channel][Rank].ACPowerWr;
        TotalRankCount++;
      }
    }
  }

  if (TotalRankCount == 0) {
    TotalRankCount = 1;  // Prevent divide by 0
  }

  PwrAvgRd /= TotalRankCount;
  PwrAvgRd += RxBiasPwr;
  PwrAvgWr /= TotalRankCount;
  //
  // update Mrc struct with Base line numbers
  //
  if (OdtPowerSaving->BaseFlag == FALSE) {
    OdtPowerSaving->BaseSavingRd  = (U16) PwrAvgRd;
    OdtPowerSaving->BaseSavingWr  = (U16) PwrAvgWr;
    OdtPowerSaving->BaseSavingCmd = 0; // currently no power train for CMD
  } else {
    OdtPowerSaving->MrcSavingRd  = (U16) PwrAvgRd;
    OdtPowerSaving->MrcSavingWr  = (U16) PwrAvgWr;
    OdtPowerSaving->MrcSavingCmd = 0; // currently no power train for CMD
  }

  return;
}

/**
  This function optimize the digital offsets by reducing the digital
  offset and apply the difference to the global one.

  @param[in] MrcData    - Include all MRC global data.
  @param[in] Param      - Parameter defining the desired digital compensation offset.
  @param[in] UpdateHost - Decides if MrcData is to be updated.

  @retval The new comp value.
**/
U32
OptimizeCompOffset (
  IN MrcParameters *const MrcData,
  IN const U8             Param,
  IN const U8             UpdateHost
  )
{
  const MrcDebug                            *Debug;
  MrcOutput                                 *Outputs;
  MrcControllerOut                          *ControllerOut;
  MrcChannelOut                             *ChannelOut;
  DDRCOMP_CR_DDRCRDATACOMP0_STRUCT          DdrCrDataComp0;
  DDRCOMP_CR_DDRCRDATACOMP1_STRUCT          DdrCrDataComp1;
  DDRCOMP_CR_DDRCRCOMPCTL0_STRUCT           DdrCrCompCtl0;
  DDRCOMP_CR_DDRCRCOMPCTL1_STRUCT           DdrCrCompCtl1;
  DDRDATA0CH0_CR_DDRCRDATAOFFSETCOMP_STRUCT DdrCrDataOffsetComp;
  U8                                        GlobalParam;
  U8                                        CurrCompVref;
  S8                                        NewCompVref;
  U8                                        CurrentComp;
  U8                                        NewComp;
  S8                                        Sign;
  U8                                        Done;
  S16                                       AvgOffset;
  U8                                        Offset;
  U8                                        StartDelta;
  U8                                        CurrDelta;
  U8                                        MinDelta;
  U8                                        Off;
  U8                                        BestVrefOff;
  U8                                        SignBit;
  U8                                        Byte;
  U8                                        Channel;
  U8                                        NumCh;
  U8                                        ReservedCodes;
  S8                                        MaxCompVref;
  S8                                        MinCompVref;
  U8                                        DqSCompPC;
  U8                                        CurrDqSCompPC;
  U8                                        CompCodes[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];

  Outputs                   = &MrcData->SysOut.Outputs;
  ControllerOut             = &Outputs->Controller[0];
  Debug                     = &MrcData->SysIn.Inputs.Debug;
  DdrCrCompCtl0.Data        = Outputs->CompCtl0;
  DdrCrCompCtl1.Data        = Outputs->CompCtl1;

  DdrCrDataOffsetComp.Data  = 0;
  ReservedCodes             = 3;
  NewComp                   = 0;
  Offset                    = 0;
  SignBit                   = 0;
  DqSCompPC                 = 0;
  CurrDqSCompPC             = 0;

  switch (Param) {
  case OptWrDS:
    DdrCrDataComp0.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP0_REG);
    CurrentComp         = (U8) DdrCrDataComp0.Bits.RcompDrvUp;
    CurrCompVref        = MrcSE ((U8) DdrCrCompCtl0.Bits.DqDrvVref, 4, 8);
    MaxCompVref         = (1 << (DDRCOMP_CR_DDRCRCOMPCTL0_DqDrvVref_WID - 1)) - 1;
    MinCompVref         = (-1) * 1 << (DDRCOMP_CR_DDRCRCOMPCTL0_DqDrvVref_WID - 1);
    GlobalParam         = WrDS;
    break;

  case OptRdOdt:
    DdrCrDataComp1.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG);
    CurrentComp         = (U8) DdrCrDataComp1.Bits.RcompOdtUp;
    CurrCompVref        = MrcSE ((U8) DdrCrCompCtl0.Bits.DqOdtVref, 5, 8);
    MaxCompVref         = (1 << (DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtVref_WID - 1)) - 1;
    MinCompVref         = (-1) * 1 << (DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtVref_WID - 1);
    GlobalParam         = RdOdt;
    break;

  case OptSComp:
    DdrCrDataComp0.Data = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP0_REG);
    CurrentComp         = (U8) DdrCrDataComp0.Bits.SlewRateComp;
    CurrDqSCompPC       = (U8) DdrCrCompCtl1.Bits.DqScompPC;
    CurrCompVref        = (U8) DdrCrCompCtl1.Bits.DqScompCells;
    MaxCompVref         = (1 << (DDRCOMP_CR_DDRCRCOMPCTL1_DqScompCells_WID)) - 1;
    MinCompVref         = 4;
    GlobalParam         = SCompDq;
    break;

  default:
    CurrentComp   = 0;
    CurrCompVref  = 0;
    MaxCompVref   = 0;
    MinCompVref   = 0;
    GlobalParam   = 0;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Invalid Param : %d", Param);
    break;
  }

  AvgOffset = 0;
  NumCh     = 0;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if ((MrcChannelExist (Outputs, Channel))) {
      ChannelOut = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        DdrCrDataOffsetComp.Data = ChannelOut->DataCompOffset[Byte];
        if (Param == OptWrDS) {
          Offset  = (U8) DdrCrDataOffsetComp.Bits.DqDrvUpCompOffset;
          SignBit = 6;
        }

        if (Param == OptRdOdt) {
          Offset  = (U8) DdrCrDataOffsetComp.Bits.DqOdtUpCompOffset;
          SignBit = 5;
        }

        if (Param == OptSComp) {
          Offset  = (U8) DdrCrDataOffsetComp.Bits.DqSlewRateCompOffset;
          SignBit = 5;
        }

        AvgOffset += (S8) MrcSE (Offset, SignBit, 8);
        CompCodes[Channel][Byte] = CurrentComp + MrcSE (Offset, SignBit, 8);
      }

      NumCh++;
    }
  }

  Sign = (AvgOffset < 0) ? -1 : 1;
  //
  // Calculate the average offset and round to the nearest integer.
  //
  AvgOffset = (AvgOffset + Sign * NumCh * ((U8) Outputs->SdramCount) / 2) / (NumCh * ((U8) Outputs->SdramCount));

  if (AvgOffset == 0) {
    return CurrentComp;
  }
  //
  // Find the CompVref minimum of the delta between (CurrentComp + AvgOffset) to NewComp.
  // Take care of keeping 3 code reserved.
  // Exit if no vref range left.
  //
  Done        = 0;
  Off         = 1;
  BestVrefOff = CurrCompVref;
  NewComp     = CurrentComp;
  DqSCompPC   = CurrDqSCompPC;
  StartDelta  = ABS ((S8) AvgOffset);
  MinDelta    = StartDelta;
  if (Param == OptSComp) {
    Sign *= -1;
  }

  while (!Done) {
    NewCompVref = CurrCompVref + (Sign * Off);
    if ((MinCompVref > NewCompVref) || (NewCompVref > MaxCompVref)) {
      Done = 1;
    }
    //
    // Reserve 3 comp codes
    //
    if ((ReservedCodes > NewComp) || (NewComp > (63 - ReservedCodes))) {
      Done = 1;
    }

    if (Param == OptSComp) {
      if ((NewCompVref + 1) > 16) {
        DqSCompPC = 0;
      }

      NewCompVref = (DqSCompPC << 4) + NewCompVref;
    }

    if (!Done) {
      NewComp   = (U8) UpdateCompGlobalOffset (MrcData, GlobalParam, NewCompVref, 0);
      CurrDelta = ABS (CurrentComp + (S8) AvgOffset - NewComp);
      if (CurrDelta < StartDelta) {
        if (CurrDelta < MinDelta) {
          MinDelta    = CurrDelta;
          BestVrefOff = NewCompVref;
          if (MinDelta == 0) {
            Done = 1;
          }
        }
      } else {
        Done = 1;
      }
    }

    Off++;
  }
  //
  // update new compVref setting
  //
  if (BestVrefOff != CurrCompVref) {
    NewComp = (U8) UpdateCompGlobalOffset (MrcData, GlobalParam, BestVrefOff, UpdateHost);
    //
    // Update all bytes with new offset: Offset + code - newcode = +newoffset
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
      if ((MrcChannelExist (Outputs, Channel))) {
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          UpdateOptParamOffset (MrcData, Channel, 0, Byte, Param, CompCodes[Channel][Byte] - NewComp, UpdateHost);
        }
      }
    }
  } else {
    //
    // Restore CompVref
    //
    if (Param == OptSComp) {
      NewCompVref = (CurrDqSCompPC << 4) + CurrCompVref;
    }

    NewComp = (U8) UpdateCompGlobalOffset (MrcData, GlobalParam, CurrCompVref, UpdateHost);
  }

  return NewComp;
}

/**
  This step performs Comp Offset optimization on the param list defined in this function.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - mrcSuccess
**/
MrcStatus
MrcOptimizeComp (
  IN MrcParameters *const MrcData
  )
{
  const U8  ParamList[] = { OptWrDS, OptRdOdt, OptSComp };
  U8        Param;

  for (Param = 0; Param < sizeof (ParamList); Param++) {
    OptimizeCompOffset (MrcData, ParamList[Param], 1);
  }

  return mrcSuccess;
}

/**
  This function calculates the percent of power saving from the power optimization steps and
  updates the proper registers in the PCU.  To get the correct base line for this calculation,
  this routing needs to run first time early in the training in order to update the MrcStruct
  with the base line.  After the power training steps, it will run again to get the actual
  percent of power saving.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - mrcSuccess

**/
MrcStatus
MrcPowerSavingMeter (
  IN MrcParameters *const MrcData
  )
{
  MrcDebug const                          *Debug;
  MrcPower                                PwrChRank[MAX_CHANNEL][MAX_RANK_IN_CHANNEL];
  MrcOdtPowerSaving                       *OdtPowerSaving;
  U8                                      PercentRd;
  U8                                      PercentWr;
  U8                                      PercentCmd;
  PCU_CR_MRC_ODT_POWER_SAVING_PCU_STRUCT  CrMrcOdtPowerSavingPcu;

  Debug           = &MrcData->SysIn.Inputs.Debug;
  OdtPowerSaving  = &MrcData->SysOut.Outputs.OdtPowerSavingData;
  CalcSysPower (MrcData, PwrChRank);
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "\tBaseLine\tMrcSaving\nAvgRd\t%d\t\t%d\nAvgWr\t%d\t\t%d\n",
    OdtPowerSaving->BaseSavingRd,
    OdtPowerSaving->MrcSavingRd,
    OdtPowerSaving->BaseSavingWr,
    OdtPowerSaving->MrcSavingWr
    );

  if (OdtPowerSaving->BaseFlag) {
    //
    // Calculate power saving and update PCU regs
    //
    if (OdtPowerSaving->BaseSavingRd > OdtPowerSaving->MrcSavingRd) {
      PercentRd = (U8) (((U32) (OdtPowerSaving->BaseSavingRd - OdtPowerSaving->MrcSavingRd) * 256) / OdtPowerSaving->BaseSavingRd);
    } else {
      PercentRd = 0;
    }

    if (OdtPowerSaving->BaseSavingWr > OdtPowerSaving->MrcSavingWr) {
      PercentWr = (U8) (((U32) (OdtPowerSaving->BaseSavingWr - OdtPowerSaving->MrcSavingWr) * 256) / OdtPowerSaving->BaseSavingWr);
    } else {
      PercentWr = 0;
    }

    if (OdtPowerSaving->BaseSavingCmd > OdtPowerSaving->MrcSavingCmd) {
      PercentCmd = (U8) (((U32) (OdtPowerSaving->BaseSavingCmd - OdtPowerSaving->MrcSavingCmd) * 256) / OdtPowerSaving->BaseSavingCmd);
    } else {
      PercentCmd = 0;
    }

    CrMrcOdtPowerSavingPcu.Bits.MRC_Saving_Rd   = PercentRd;
    CrMrcOdtPowerSavingPcu.Bits.MRC_Saving_Wt   = PercentWr;
    CrMrcOdtPowerSavingPcu.Bits.MRC_Saving_Cmd  = PercentCmd;

    MrcWriteCR (MrcData, PCU_CR_MRC_ODT_POWER_SAVING_PCU_REG, CrMrcOdtPowerSavingPcu.Data);
  } else {
    OdtPowerSaving->BaseFlag = TRUE;
  }

  return mrcSuccess;
}
