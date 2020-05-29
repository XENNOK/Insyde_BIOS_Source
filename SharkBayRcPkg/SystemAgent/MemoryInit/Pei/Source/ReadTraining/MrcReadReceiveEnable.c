/** @file
  Implementation of the receive enable algorithm.
  Receive enable training is made out of two stages, the first is finding the
  DQS rising edge for each DRAM device, and the second is determining the
  roundtrip latency by locating the preamble cycle.

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
#include <MrcReadReceiveEnable.h>
///
/// Local defines
///

#define RCV_EN_CENTER_LC (17)

/**
@brief
  Perform receive enable training.
  Optimize RcvEn timing with MPR pattern

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus -  if succeeded, return mrcSuccess
**/
MrcStatus
MrcReadLevelingTraining (
  IN OUT MrcParameters *const MrcData
  )
{
  const MRC_REUTAddress REUTAddress = {{0, 0, 0, 0},    // Start
                                       {0, 0, 0, 1023}, // Stop
                                       {0, 0, 0, 0},    // Order
                                       {0, 0, 0, 0},    // IncRate
                                       {0, 0, 0, 1}};   // IncValue
  const U8              RLStep0 = 8;
  const MrcInput        *Inputs;
  const MrcDebug        *Debug;
  MrcOutput             *Outputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcStatus             Status;
  MrcProfile            Profile;
  U8                    NumSamples;
  U8                    FineStep;
  U8                    DumArr[7];
  U8                    Channel;
  U8                    Rank;
  U8                    Byte;
  U8                    ByteN;     // ByteNumber
  U8                    ByteNTimes2;
  U8                    ChBitMask;
  U8                    RankMask;  // RankBitMask for both channels
  U8                    Done;
  U8                    Inc;
  U16                   RLStart;
  U16                   RLStop;
  U16                   RLDelay;
  U16                   ChResult[MAX_CHANNEL];
  U16                   ChMask;
  U32                   CRValue;
  U32                   Offset;
  U32                   RtIoComp;
  U32                   RtLatency;
  S32                   InitialPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                   InitialPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                   CurrentPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                   CurrentPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                   LargestPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                   LargestPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                   IncPreAmble[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32                   iWidth;
  S32                   cWidth;
  S32                   lWidth;
  S32                   Center;
  S32                   Width;
  BOOL                  Pass;
  MRC_WDBPattern        WDBPattern;
  U8                    Temp;
  U16                   TDqsCkDrift;
#ifdef ULT_FLAG
  U32                   DclkPs;
#endif // ULT_FLAG

  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT  DdrCrDataControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT DdrCrDataControl2;
  MCHBAR_CH0_CR_SC_IO_LATENCY_STRUCT      ScIoLatency;
  DDRDATA0CH0_CR_DATATRAINFEEDBACK_STRUCT DataTrainFeedback;

  WDBPattern.IncRate  = 32;
  WDBPattern.Start    = 0;
  WDBPattern.Stop     = 9;
  WDBPattern.DQPat    = BasicVA;
  Status              = mrcSuccess;
  Done                = 0;
  Inputs              = &MrcData->SysIn.Inputs;
  Debug               = &Inputs->Debug;
  Outputs             = &MrcData->SysOut.Outputs;
  ControllerOut       = &Outputs->Controller[0];
  ChBitMask           = Outputs->ValidChBitMask;
  RankMask            = Outputs->ValidRankMask;
  Profile             = Inputs->MemoryProfile;
  TDqsCkDrift         = tDQSCK_DRIFT;  // Pull in RcvEna by 1 QClk for Traditional.

#ifdef ULT_FLAG
#endif

  MrcOemMemorySet (DumArr, 1, sizeof (DumArr));

  NumSamples = 6;
  FineStep   = 1;

  RtIoComp  = 0;
  RtLatency = 0;

  switch (Inputs->CpuModel) {
    case cmCRW:
      RtIoComp  = MRC_ROUND_TRIP_IO_COMPENSATION_2_CHANNEL_A0;
      RtLatency = HW_ROUNDT_LAT_DEFAULT_VALUE_A0;
      break;

    case cmHSW_ULT:
      RtIoComp  = MRC_ROUND_TRIP_IO_COMPENSATION_2_CHANNEL_ULT_A0;
      RtLatency = HW_ROUNDT_LAT_DEFAULT_VALUE_ULT_A0;
      break;

    case cmHSW:
    default:
      RtIoComp = (Inputs->CpuStepping > csHswA0) ? MRC_ROUND_TRIP_IO_COMPENSATION_2_CHANNEL_B0 :
        MRC_ROUND_TRIP_IO_COMPENSATION_2_CHANNEL_A0;
      RtLatency = (Inputs->CpuStepping > csHswA0) ? HW_ROUNDT_LAT_DEFAULT_VALUE_B0 :
        HW_ROUNDT_LAT_DEFAULT_VALUE_A0;
      break;
  }

  //
  // CmdPat=PatRd, NumCL=2, LC=7, REUTAddress, SOE=0,
  // WDBPattern, EnCADB=0, EnCKE=0, SubSeqWait=8
  //
  SetupIOTest (MrcData, ChBitMask, PatRd, 2, NumSamples + 1, &REUTAddress, 0, &WDBPattern, 0, 0, 8);

  //
  // Prepare Channel and Rank bit mask & Enable RLMode, force Odt and SAmp.
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(MrcChannelExist (Outputs, Channel))) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];

    //
    // Enable ReadLeveling Mode and Force On ODT and SenseAmp
    //
    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
      Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
        ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
        ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
      DdrCrDataControl2.Data            = ChannelOut->DqControl2[Byte].Data;
      DdrCrDataControl2.Bits.ForceRxOn  = 1;
      MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
    }
    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
    DdrCrDataControl0.Data = ChannelOut->DqControl0.Data;

#ifdef ULT_FLAG
    if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
      //
      // W/A for b4618574 - @todo: remove for HSW ULT C0
      // Can't have ForceOdtOn together with Leaker, disable LPDDR mode during this training step
      // LPDDR_Mode is restored at the end of this function from the host structure.
      //
      DdrCrDataControl0.Bits.LPDDR_Mode = 0;
      MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
    }
#endif // ULT_FLAG

    DdrCrDataControl0.Bits.ForceOdtOn     = 1;
    DdrCrDataControl0.Bits.RLTrainingMode = 1;
    MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);

    //
    // Set initial IO Latency and IO_COMP
    //
    Offset = MCHBAR_CH0_CR_SC_IO_LATENCY_REG +
      ((MCHBAR_CH1_CR_SC_IO_LATENCY_REG - MCHBAR_CH0_CR_SC_IO_LATENCY_REG) * Channel);
    ScIoLatency.Data = 0;
    ScIoLatency.Bits.RT_IOCOMP = RtIoComp;
    MrcWriteCR (MrcData, Offset, ScIoLatency.Data);
  }

  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if (!((MRC_BIT0 << Rank) & RankMask)) {
      //
      // Skip if both channels empty
      //
      continue;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRank %d\n", Rank);

    ChBitMask = 0;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChBitMask |= SelectReutRanks (MrcData, Channel, MRC_BIT0 << Rank, 0);
      if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
        continue;
      }

      ChannelOut = &ControllerOut->Channel[Channel];

      ChannelOut->IoLatency[Rank] = 0;
      //
      // Set initial Roundtrip Latency values -4 QCLK assumed for worst board layout
      //
      // Default ROUNDT_LAT = HW_ROUNDT_LAT_DEFAULT_VALUE + nMode value * 2 + (2 * tCL) + 4QCLK + PI_CLK
      // LPDDR3 formula:      HW_ROUNDT_LAT_DEFAULT_VALUE + (2 * tCL) + 4QCLK + PI_CLK + tDQSCK_max
      // NMode = 3 during training mode
      //
      Temp = (Outputs->Ratio >= 2) ? Outputs->Ratio : 0;

#ifdef ULT_FLAG
      if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
        DclkPs = Outputs->Qclkps * 2;
        CRValue = RtLatency + (2 * ChannelOut->Timing[Profile].tCL) + MAX (Temp, 4) + 1 + (tDQSCK_MAX + DclkPs - 1) / DclkPs;
      } else
#endif // ULT_FLAG
      {
        CRValue = RtLatency + (2 * 2) + (2 * ChannelOut->Timing[Profile].tCL) + MAX (Temp, 4) + 1;
      }
      CRValue = MIN (MCHBAR_CH0_CR_SC_ROUNDT_LAT_Lat_R0D0_MAX, CRValue);
      Offset = MCHBAR_CH0_CR_SC_ROUNDT_LAT_REG +
        ((MCHBAR_CH1_CR_SC_ROUNDT_LAT_REG - MCHBAR_CH0_CR_SC_ROUNDT_LAT_REG) * Channel) + Rank;
      MrcWriteCR8 (MrcData, Offset, (U8) CRValue);
      ChannelOut->RTLatency[Rank] = (U8) CRValue;
    }
    //
    // ******************************************
    // STEP 1 and 2: Find middle of high region
    // ******************************************
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Step 1 and 2 Find Middle of high region\n");
    RLStart = 256 + 24;
    RLStop  = 384 + 24;

    for (RLDelay = RLStart; RLDelay < RLStop; RLDelay += RLStep0) {
      //
      // Program RL Delays:
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          continue;
        }

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          UpdateRxT (MrcData, Channel, Rank, Byte, 0, RLDelay);
        }
      }
      //
      // Run Test, Reset FIFOs will be done before running test
      //
      RunIOTest (MrcData, ChBitMask, BasicVA, DumArr, 1, 0);

      //
      // Update results for all Channels/Bytes
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          continue;
        }

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
          Pass = (MrcReadCR (MrcData, Offset) >= (U8) (MRC_BIT0 << (NumSamples - 1)));
          if (RLDelay == RLStart) {
            if (Pass) {
              InitialPassingStart[Channel][Byte] = InitialPassingEnd[Channel][Byte]  = RLStart;
              CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte]  = RLStart;
              LargestPassingStart[Channel][Byte] = LargestPassingEnd[Channel][Byte]  = RLStart;
            } else {
              InitialPassingStart[Channel][Byte] = InitialPassingEnd[Channel][Byte]  = -RLStep0;
              CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte]  = -RLStep0;
              LargestPassingStart[Channel][Byte] = LargestPassingEnd[Channel][Byte]  = -RLStep0;
            }
          } else {
            if (Pass) {
              if (InitialPassingEnd[Channel][Byte] == (RLDelay - RLStep0)) {
                InitialPassingEnd[Channel][Byte] = RLDelay;
              }

              if (CurrentPassingEnd[Channel][Byte] == (RLDelay - RLStep0)) {
                CurrentPassingEnd[Channel][Byte] = RLDelay;
              } else {
                CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte] = RLDelay;
              }
              //
              // Special case for last step: Append Initial Passing Region
              // RLDelay should be considered a continuous range that wraps around 0
              //
              if ((RLDelay >= (RLStop - RLStep0)) &&
                  (InitialPassingStart[Channel][Byte] == RLStart) &&
                  (InitialPassingEnd[Channel][Byte] != RLDelay)
                  ) {

                iWidth = (CurrentPassingEnd[Channel][Byte] - CurrentPassingStart[Channel][Byte]);
                InitialPassingStart[Channel][Byte] -= (RLStep0 + iWidth);

                LargestPassingStart[Channel][Byte]  = InitialPassingStart[Channel][Byte];
                LargestPassingEnd[Channel][Byte]    = InitialPassingEnd[Channel][Byte];
                continue;
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
    }
    //
    // Update RcvEn timing to be in the center of the high region.
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
        continue;
      }

      ChannelOut = &ControllerOut->Channel[Channel];

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d.R%d:  Left\tRight\tWidth\tCenter\n", Channel, Rank);
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        Center  = (LargestPassingEnd[Channel][Byte] + LargestPassingStart[Channel][Byte]) / 2;
        Width   = LargestPassingEnd[Channel][Byte] - LargestPassingStart[Channel][Byte];

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "  B%d:   %d     %d     %d     %d\n",
          Byte,
          LargestPassingStart[Channel][Byte],
          LargestPassingEnd[Channel][Byte],
          Width,
          Center
          );

        //
        // Check if center of High was found
        //
        if (Center > RLStop) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_ERROR,
            "\nERROR! Center of High Higher than expected for Channel: %u Byte: %u\n",
            Channel,
            Byte
            );
          return mrcReadLevelingError;
        }
        //
        // Check if width is valid
        //
        if ((Width <= 32) || (Width >= 96)) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_ERROR,
            "\nERROR! Width region (%d) outside expected limits for Channel: %u Byte: %u\n",
            Width,
            Channel,
            Byte
            );
          return mrcReadLevelingError;
        }

        ChannelOut->RcvEn[Rank][Byte] = (U16) Center;
        UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
      }
    }
    //
    // ******************************************************************************
    // STEP 3: Walk Backwards
    // ******************************************************************************
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nStep 3: Quarter Preamble - Walk Backwards\n");

    if (Outputs->ValidRankMask & (MRC_BIT0 << Rank)) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel     0                 1\nByte        ");
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE, (
        Outputs->SdramCount == MAX_SDRAM_IN_DIMM
        ) ? "0 1 2 3 4 5 6 7 8      0 1 2 3 4 5 6 7 8" : "0 1 2 3 4 5 6 7      0 1 2 3 4 5 6 7"
        );
    }

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChResult[Channel] = 0x1FF;
    }

    //
    // 0x1FF or 0xFF
    //
    ChMask = (MRC_BIT0 << Outputs->SdramCount) - 1;
    while ((ChResult[0] != 0) || (ChResult[1] != 0)) {
      //
      // Run Test
      //
      RunIOTest (MrcData, ChBitMask, BasicVA, DumArr, 1, 0);

      //
      // Update results for all Channel/Bytes
      //
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nIOLAT =");

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChResult[Channel] = 0;

        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          if (Channel == 0) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              (Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "                       " : "                     "
              );
          }

          continue;
        }

        ChannelOut = &ControllerOut->Channel[Channel];

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  %u  ", ChannelOut->IoLatency[Rank]);

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
          DataTrainFeedback.Data  = MrcReadCR (MrcData, Offset);
          Pass                    = (DataTrainFeedback.Bits.DataTrainFeedback >= (U16) (MRC_BIT0 << (NumSamples - 1)));
          if (Pass) {
            ChResult[Channel] |= (MRC_BIT0 << Byte);
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, Pass ? "H " : "L ");
        }
        //
        // Adjust Timing
        //
        if ((ChResult[Channel] == ChMask) && (ChannelOut->IoLatency[Rank] < 14)) {
          //
          // Adjust Timing globally for all Bytes - Number in Qclks
          //
          ChannelOut->IoLatency[Rank] = ((ChannelOut->IoLatency[Rank] + 2) & MCHBAR_CH0_CR_SC_IO_LATENCY_IOLAT_R0D0_MSK);

          //
          // @todo: Add an error check if we reach IOLAT > 10
          // Update Value
          //
          ByteN       = RANK_TO_DIMM_NUMBER (Rank);
          ByteNTimes2 = ByteN * 2;
          CRValue     = (ChannelOut->IoLatency[ByteNTimes2]);
          CRValue += (ChannelOut->IoLatency[ByteNTimes2 + 1] << MCHBAR_CH0_CR_SC_IO_LATENCY_IOLAT_R0D0_WID);
          Offset = MCHBAR_CH0_CR_SC_IO_LATENCY_REG +
            ((MCHBAR_CH1_CR_SC_IO_LATENCY_REG - MCHBAR_CH0_CR_SC_IO_LATENCY_REG) * Channel) + (ByteN);
          MrcWriteCR8 (MrcData, Offset, (U8) CRValue);
        } else {
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            if (ChResult[Channel] & (MRC_BIT0 << Byte)) {
              if (ChannelOut->RcvEn[Rank][Byte] > 127) {
                ChannelOut->RcvEn[Rank][Byte] -= 128;
              } else {
                //
                // Error Handler
                //
                MRC_DEBUG_MSG (
                  Debug,
                  MSG_LEVEL_ERROR,
                  "\nERROR! Channel: %u Rank: %uByte: %u - RcvEn %u/IoLat %u while walking backwards\n",
                  Channel,
                  Rank,
                  Byte,
                  ChannelOut->RcvEn[Rank][Byte],
                  ChannelOut->IoLatency[Rank]
                  );
                return mrcReadLevelingError;
              }

              UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
            }
          }
        }
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
        continue;
      }
      ChannelOut = &ControllerOut->Channel[Channel];
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%u:  Preamble\n", Channel);

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " B%u: %u\n", Byte, ChannelOut->RcvEn[Rank][Byte]);
      }
    }
    //
    // ******************************************
    // STEP 4: Add 1 qclk
    // ******************************************
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
        continue;
      }

      ChannelOut = &Outputs->Controller[0].Channel[Channel];

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        ChannelOut->RcvEn[Rank][Byte] += 64;
        UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
      }
    }
    //
    // ******************************************
    // STEP 5: Walk forward
    // ******************************************
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Step 5 Walk forward\n");
    //
    // Find Rising Edge
    //
    ChResult[0] = 0;
    ChResult[1] = 0;

    for (Inc = 0; Inc < 64; Inc += FineStep) {
      //
      // Run Test
      //
      RunIOTest (MrcData, ChBitMask, BasicVA, DumArr, 1, 0);

      //
      // Update results for all Channel/bytes
      //
      Done = 1;
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          continue;
        }
        ChannelOut = &ControllerOut->Channel[Channel];

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          //
          // Skip Bytes that are already done
          //
          if (ChResult[Channel] & (MRC_BIT0 << Byte)) {
            continue;
          }
          //
          // Check if this byte is done
          //
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
          Pass = (MrcReadCR (MrcData, Offset) >= (U8) (MRC_BIT0 << (NumSamples - 1)));
          if (Pass) {
            ChResult[Channel] |= (MRC_BIT0 << Byte);
          } else {
            ChannelOut->RcvEn[Rank][Byte] += FineStep;
            UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
            IncPreAmble[Channel][Byte] = Inc;
          }
        }

        if (ChResult[Channel] != ChMask) {
          Done = 0;
        }
      }
      //
      // Skip additional testing if all Channel/bytes done
      //
      if (Done) {
        break;
      }
    }
    //
    // Check if Edge was found for all Bytes in the channels
    //
    if (!Done) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_ERROR,
        "Error! Pre-amble edge not found for all Bytes with following final RcvEn results\n"
        );

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          continue;
        }

        ChannelOut = &ControllerOut->Channel[Channel];

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %u Rank %u:  Preamble\n", Channel, Rank);
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            " Byte %u: %u %s\n",
            Byte,
            ChannelOut->RcvEn[Rank][Byte],
            ((ChResult[Channel] ^ ChMask) & (1 << Byte)) ? "" : "*** ERROR! Check This Byte ***"
            );
        }
      }

      return mrcReadLevelingError;
    }
    //
    // ******************************************
    // STEP 6: Sub 1 qclk and Clean Up Rank
    // ******************************************
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Step 6: Mid Preamble\n");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
        continue;
      }
      ChannelOut = &ControllerOut->Channel[Channel];

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%u:  Preamble Increment\n", Channel);

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        //
        // For Traditional, pull in RcvEn by 64
        // For ULT, Take the DQS drift into account to the specified guardband: tDQSCK_DRIFT.
        //
        ChannelOut->RcvEn[Rank][Byte] -= TDqsCkDrift;

        UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          " B%u: %u      %u\n",
          Byte,
          ChannelOut->RcvEn[Rank][Byte],
          IncPreAmble[Channel][Byte]
          );
      }
    }
  } // END OF RANK LOOP
  //
  // Clean up after Test
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!(MrcChannelExist (Outputs, Channel))) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];

    Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
    DdrCrDataControl0.Data = ChannelOut->DqControl0.Data;
#ifdef ULT_FLAG
    if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
      //
      // W/A for b4618574 - @todo: remove for HSW ULT C0
      // Can't have ForceOdtOn together with Leaker, disable LPDDR mode during this training step
      // This write will disable ForceOdtOn while still keeping LPDDR_Mode disabled.
      // Second write will restore LPDDR_Mode.
      //
      DdrCrDataControl0.Bits.LPDDR_Mode = 0;
      MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
    }
#endif // ULT_FLAG
    MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0.Data);

    for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
    Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
      ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
      ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
      MrcWriteCR (MrcData, Offset, ChannelOut->DqControl2[Byte].Data);
    }
  }

  Status = IoReset (MrcData);

  //
  // Step 7: Try to get IO Lat the same across all ranks per channel
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Step 7: Sync IO Lat Across Ranks\n");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Status = MrcChangeRcvEnTiming (
        MrcData,
        Channel,
        RRE_ALL_RANKS_MASK,
        0,  // ByteMask
        0,  // Offset
        RRE_PI_TO_RESERVE
        );
    }
  }

  //
  // Print IO Latency/RcvEn
  //
#ifdef MRC_DEBUG_PRINT
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Adjusted Receive Enable and IO Lat Values\n");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];
      Offset = MCHBAR_CH0_CR_SC_IO_LATENCY_REG +
        ((MCHBAR_CH1_CR_SC_IO_LATENCY_REG - MCHBAR_CH0_CR_SC_IO_LATENCY_REG) * Channel);
      ScIoLatency.Data  = MrcReadCR (MrcData, Offset);
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if ((MrcRankInChannelExist (MrcData, Rank, Channel))) {
          MRC_DEBUG_MSG (Debug,
            MSG_LEVEL_NOTE,
            "  C%d.R%d: IOLAT = %u  RT_IOCOMP = %d\n",
            Channel,
            Rank,
            ChannelOut->IoLatency[Rank],
            ScIoLatency.Bits.RT_IOCOMP
            );
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "   B%d:   %u\n", Byte, ChannelOut->RcvEn[Rank][Byte]);
          }
        }
      }
    }
  }
#endif

  return Status;
}

/**
@brief
  Apply an signed offset to all selected bytes/ ranks in a channel to RcvEn timing
  Robustly handles any carries to/from the IO Latency vs. RcvEn FlyBy
  PiReserve will reserve a certain number of +/- PI ticks for margin purposes
  Routine also minimizes the difference in RcvEn settings across ranks

  @param[in,out] MrcData       - MRC Global Data
  @param[in]     Channel       - The channel to adjust
  @param[in]     RankMask      - Mask of Ranks to adjust
  @param[in]     ByteMask      - Mask of Bytes to adjust by the RcvEnOffset
  @param[in]     RcvEnOffset   - Amount to offset RcvEn
  @param[in]     PiReserve     - The number of PiTicks to reserve on each edge of RcvEn

  @retval MrcStatus - mrcSuccess if successfull
                      mrcWrongInputParameter if channel doesnt exist or a RankMask of 0 is provided
                      mrcReadLevelingError if we over/underflow RT_IOCOMP field.
**/
MrcStatus
MrcChangeRcvEnTiming (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Channel,
  IN     const U8             RankMask,
  IN     const U16            ByteMask,
  IN     const S16            RcvEnOffset,
  IN     const S16            PiReserve
  )
{
  MrcStatus                          Status;
  MrcOutput                          *Outputs;
  MrcDebug                           *Debug;
  MrcChannelOut                      *ChannelOut;
  MCHBAR_CH0_CR_SC_IO_LATENCY_STRUCT ScIoLatency;
  U8                                 Rank;
  U8                                 Byte;
  S8                                 CycleOffset;
  S8                                 IoGlobalOffset;
  S8                                 IoLatRank[MAX_RANK_IN_CHANNEL];
  S8                                 IoLatTarget;
  S8                                 MaxRankLat;
  S8                                 MinRankLat;
  S16                                NewRcvEn;
  S16                                MaxRcvEn;
  S16                                MinRcvEn;
  S16                                MaxRcvEnRank[MAX_RANK_IN_CHANNEL];
  S16                                MinRcvEnRank[MAX_RANK_IN_CHANNEL];
  U32                                CrOffset;

  //
  // Init variables with min and max values
  //
  Status           = mrcSuccess;
  Outputs          = &MrcData->SysOut.Outputs;
  Debug            = &MrcData->SysIn.Inputs.Debug;
  ChannelOut       = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  MaxRankLat       = 0;
  MinRankLat       = 15;
  MaxRcvEn         = -4096;
  MinRcvEn         = 4096;
  IoGlobalOffset   = 0;
  MrcOemMemorySetWord ((U16*) MaxRcvEnRank, (U16)-4096, MAX_RANK_IN_CHANNEL);
  MrcOemMemorySetWord ((U16*) MinRcvEnRank, 4096, MAX_RANK_IN_CHANNEL);
  MrcOemMemorySet ((U8*) IoLatRank, 0, MAX_RANK_IN_CHANNEL);

  //
  // Quick error check on parameters
  //
  if ((!(MrcChannelExist (Outputs, Channel))) || (RankMask == 0)) {
    return mrcWrongInputParameter;
  }

  //
  // Walk through all the ranks/bytes to find Max/Min RcvEn values
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if ((((MRC_BIT0 << Rank) & RankMask) != 0) && ((MrcRankInChannelExist (MrcData, Rank, Channel)))) {
      //
      // Find Max/Min for RcvEn across bytes.  RcvEn is the total (RcvEnPi - 64 * IOLat)
      //
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        NewRcvEn = (S16) ChannelOut->RcvEn[Rank][Byte] - (64 * (S16) ChannelOut->IoLatency[Rank]);
        if (ByteMask & (MRC_BIT0 << Byte)) {
          //
          // Apply an offset for this byte
          //
          NewRcvEn += RcvEnOffset;
        }

        if (MaxRcvEnRank[Rank] < NewRcvEn) {
          MaxRcvEnRank[Rank] = NewRcvEn;
        }

        if (MinRcvEnRank[Rank] > NewRcvEn) {
          MinRcvEnRank[Rank] = NewRcvEn;
        }
      }
      //
      // Find Max/Min for RcvEn across ranks
      //
      if (MaxRcvEn < MaxRcvEnRank[Rank]) {
        MaxRcvEn = MaxRcvEnRank[Rank];
      }

      if (MinRcvEn > MinRcvEnRank[Rank]) {
        MinRcvEn = MinRcvEnRank[Rank];
      }
    }
  }

  //
  // Determine how far we are from the ideal center point for RcvEn timing.
  // (PiIdeal - AveRcvEn)/64 is the ideal number of cycles we should have for IOLatency
  // Command training will reduce this by 64, so plan for that now in the ideal value
  //
  IoLatTarget = (S8) ((RRE_PI_IDEAL - ((MaxRcvEn + MinRcvEn) / 2) + 32) / 64); // Rnd to closest int

  //
  // Walk through all the ranks and calculate new values of IOLat
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if ((((MRC_BIT0 << Rank) & RankMask) != 0) && ((MrcRankInChannelExist (MrcData, Rank, Channel)))) {
      IoLatRank[Rank] = IoLatTarget;

      //
      // Check for RcvEn underflow
      //
      NewRcvEn = 64 * IoLatRank[Rank] + MinRcvEnRank[Rank];
      if (NewRcvEn < PiReserve) {
        IoLatRank[Rank] += (U8) ((PiReserve - NewRcvEn + 63) / 64); // Ceiling
      }

      //
      // Check for RcvEn overflow
      //
      NewRcvEn = 64 * IoLatRank[Rank] + MaxRcvEnRank[Rank];
      if (NewRcvEn > (511 - PiReserve)) {
        IoLatRank[Rank] -= (U8) ((NewRcvEn - (511 - PiReserve) + 63) / 64); // Ceiling
      }
      //
      // Check for IO Latency over/underflow
      //
      if ((IoLatRank[Rank] - IoGlobalOffset) > 14) {
        IoGlobalOffset = IoLatRank[Rank] - 14;
      }
      if ((IoLatRank[Rank] - IoGlobalOffset) < 1) {
        IoGlobalOffset = IoLatRank[Rank] - 1;
      }
    //
    // Update Byte level results
    //
      CycleOffset = IoLatRank[Rank] - (S8) ChannelOut->IoLatency[Rank];
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        ChannelOut->RcvEn[Rank][Byte] += 64 * (U16) CycleOffset;
        if (ByteMask & (MRC_BIT0 << Byte)) {
          ChannelOut->RcvEn[Rank][Byte] += (U16) RcvEnOffset;
        }
        UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
      }
    }
  }
  //
  // Calculate new IOComp Latency to include over/underflow
  //
  CrOffset = MCHBAR_CH0_CR_SC_IO_LATENCY_REG +
    ((MCHBAR_CH1_CR_SC_IO_LATENCY_REG - MCHBAR_CH0_CR_SC_IO_LATENCY_REG) * Channel);
  ScIoLatency.Data = MrcReadCR (MrcData, CrOffset);

  //
  // Check to see if were under/overflowing this field
  //
  if ((IoGlobalOffset < 0) && (ScIoLatency.Bits.RT_IOCOMP < (U8) -IoGlobalOffset)) {
    Status = mrcReadLevelingError;
  } else if (
               (IoGlobalOffset > 0) &&
               ((U8)IoGlobalOffset > (MCSCHEDS_CR_SC_IO_LATENCY_RT_IOCOMP_MAX - ScIoLatency.Bits.RT_IOCOMP))
             ) {
    Status = mrcReadLevelingError;
  }

  if(Status == mrcReadLevelingError) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_ERROR,
      "MrcChangeRcvEnTiming(): RT_IOCOMP %s\n IoGlobalOffset: %d\n RT_IOCOMP(6'b): %d\n",
      (IoGlobalOffset < 0) ? "underflowed" : "overflowed",
      IoGlobalOffset,
      ScIoLatency.Bits.RT_IOCOMP
      );
  }
  ScIoLatency.Bits.RT_IOCOMP += IoGlobalOffset;
  ChannelOut->RTIoComp        = ScIoLatency.Bits.RT_IOCOMP;

  //
  // Walk through all ranks to program new IO Latency values
  //
  ScIoLatency.Data &= ~(
                        MCHBAR_CH0_CR_SC_IO_LATENCY_IOLAT_R0D0_MSK +
                        MCHBAR_CH0_CR_SC_IO_LATENCY_IOLAT_R1D0_MSK +
                        MCHBAR_CH0_CR_SC_IO_LATENCY_IOLAT_R0D1_MSK +
                        MCHBAR_CH0_CR_SC_IO_LATENCY_IOLAT_R1D1_MSK
                        );
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if ((((MRC_BIT0 << Rank) & RankMask) != 0) && ((MrcRankInChannelExist (MrcData, Rank, Channel)))) {
      ChannelOut->IoLatency[Rank] = IoLatRank[Rank] - IoGlobalOffset;
    }
    ScIoLatency.Data |= (
        (MCHBAR_CH0_CR_SC_IO_LATENCY_IOLAT_R0D0_MSK & (ChannelOut->IoLatency[Rank])) <<
        (Rank * MCHBAR_CH0_CR_SC_IO_LATENCY_IOLAT_R0D0_WID)
      );
  }

  //
  // Program new IO Latency
  //
  MrcWriteCR (MrcData, CrOffset, ScIoLatency.Data);

  return Status;
}

/**
@brief
  Once the DQS high phase has been found (for each DRAM) the next stage is to find out the round trip latency,
  by locating the preamble cycle. This is achieved by trying smaller and smaller roundtrip
  values until the strobe sampling is done on the preamble cycle.
  The following algorithm is used to find the preamble cycle:

  @param[in] MrcData         - all the global data

  @retval Nothing.
**/
MrcStatus
MrcRoundTripLatency (
  IN     MrcParameters *const MrcData
  )
{
  MrcStatus         Status;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcDebug          *Debug;
  U8                Channel;
  U8                Rank;
  U8                OptParam;
  U8                RankMask;
  U8                TestList[1];
  S8                ClkShifts[1];
  U8                Start;
  U8                Stop;
  U8                LoopCount;
  U8                Update;
  U8                MinIoLat;
  U8                MaxRankRtl;
  S8                DeltaLimitRtl;
  U8                DeltaRtl;
  MCHBAR_CH0_CR_TC_BANK_RANK_A_STRUCT TcBankRankA;

  Status        = mrcSuccess;
  TestList[0]   = RdT;        // Test based on read eye width
  ClkShifts[0]  = 25;         // Delay by 25 pi ticks to guardband for delay drift/jitter
  LoopCount     = 10;
  Update        = 1;          // Apply the optimal settings
  MaxRankRtl    = 0;
  Outputs       = &MrcData->SysOut.Outputs;
  Debug         = &MrcData->SysIn.Inputs.Debug;
  ControllerOut = &Outputs->Controller[0];
  OptParam      = rtl;        // Which parameter to optimize for

  //
  // Train timing separately for each rank
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    RankMask = (MRC_BIT0 << Rank);
    if (!(RankMask & Outputs->ValidRankMask)) {
      continue;
    }
    //
    // Pick starting and stopping points
    //
    Stop      = 0;
    Start     = 0;
    MinIoLat  = 15;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
        continue;
      }
      ChannelOut = &ControllerOut->Channel[Channel];

      if (Stop < ChannelOut->RTLatency[Rank]) {
        Stop = ChannelOut->RTLatency[Rank];
      }

      if (MinIoLat > ChannelOut->IoLatency[Rank]) {
        MinIoLat = ChannelOut->IoLatency[Rank];
      }

      Start = Stop - MinIoLat;
    }

    if ((S8) Start < 0) {
      Start = 0;
    }
    //
    // Find optimal answer
    //
    Status = TrainDDROptParamCliff (
              MrcData,
              OptParam,
              TestList,
              sizeof (TestList),
              Start,
              Stop,
              LoopCount,
              Update,
              Outputs->MarginResult,
              ClkShifts,
              sizeof (ClkShifts),
              Rank,
              RankMask,
              0
              );
    if (Status == mrcFail) {
      return mrcRoundTripLatencyError;
    }
  }

  //
  // Limit the RTL delta across the ranks present.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nLimit the delta between Rank's RTL value.\n");
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %d\n", Channel);
      ChannelOut = &ControllerOut->Channel[Channel];
      TcBankRankA.Data = ChannelOut->MchbarBANKRANKA;
      DeltaLimitRtl = MAX ((S8) TcBankRankA.Bits.tRDRD_dr, (S8) TcBankRankA.Bits.tRDRD_dd);
      //
      // TA Times are in dclks.  Must convert to qclks and subtract the burst length.
      // Ensure we do not underflow the variable.
      //
      DeltaLimitRtl = ((2 * DeltaLimitRtl) - 8);
      DeltaLimitRtl = MAX (DeltaLimitRtl, 0);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RTL Delta Limit: %d\n", DeltaLimitRtl);

      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          MaxRankRtl = MAX (MaxRankRtl, ChannelOut->RTLatency[Rank]);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Rank %u RTL: %u\n", Rank, ChannelOut->RTLatency[Rank]);
        }
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MaxRankRtl: %u\n", MaxRankRtl);

      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          DeltaRtl = MaxRankRtl - ChannelOut->RTLatency[Rank];
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "Rank %d: DeltaRtl: %u\tDeltaLimitRtl: %u%s",
            Rank,
            DeltaRtl,
            DeltaLimitRtl,
            (DeltaRtl > DeltaLimitRtl) ? "\tNew RTL: " : ""
            );
          if (DeltaRtl > DeltaLimitRtl) {
            UpdateTAParamOffset (MrcData, Channel, 0, OptParam, MaxRankRtl - DeltaLimitRtl, 1, 0, 1 << Rank);
          }
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
        }
      }
    }
  }

  return Status;
}

/**
  Perform Receive Enable Timing Centering.
  Center Receive Enable using moderate pattern with 1D eye.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - If successful, returns mrcSuccess.
**/
MrcStatus
MrcReceiveEnTimingCentering (
  IN MrcParameters *const MrcData
  )
{
  MrcOutput *Outputs;

  Outputs = &MrcData->SysOut.Outputs;

  return DQTimeCentering1D (MrcData, Outputs->ValidChBitMask, RcvEnaX, 0, RCV_EN_CENTER_LC);
}