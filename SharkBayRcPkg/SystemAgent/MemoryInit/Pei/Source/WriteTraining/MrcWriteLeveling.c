/** @file

  The write leveling flow is the first part of the write training.
  In this stage the memory controller needs to synchronize its DQS sending
  with the clock for each DRAM.  The DRAM can be put in a mode where for a
  write command it responds by sampling the clock using DQS and sending it
  back as the data.  The IO can receive this and tune the DQS alignment so
  it will appear in sync with the clock at the DRAM side.
  The following algorithm is used for the write leveling flow:

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/


#include <MrcWriteLeveling.h>

/**

@brief

  this function execute the Jedec write leveling Cleanup.
  Center TxDQS-CLK timing

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus -  if it succeded return mrcSuccess

**/
MrcStatus
MrcJedecWriteLevelingCleanUp (
  IN OUT MrcParameters *const MrcData
  )
{
  const MRC_REUTAddress REUTAddress = {{0, 0, 0, 0},    // Start
                                       {0, 0, 0, 1023}, // Stop
                                       {0, 0, 0, 0},    // Order
                                       {0, 0, 0, 0},    // IncRate
                                       {0, 0, 0, 1}};   // IncValue
  const U8          DumArr[7] = {1, 1, 1, 1, 1, 1, 1};
  const U8          DqOffsetMax = 7;
  const S8          DqOffsets[7] = {0, -10, 10, -5, 5, -15, 15};
  const S8          Offsets[5] = {0, 1, -1, 2, 3};
  const U8          PMaskConst[8] = {0, 0, 1, 1, 1, 1, 0, 0};
  const U32         CleanUpSeeds[MRC_WDB_NUM_MUX_SEEDS] = {0xAAAAAA, 0xCCCCCC, 0xF0F0F0};
  const U32         NormalSeeds[MRC_WDB_NUM_MUX_SEEDS] = {0xA10CA1, 0xEF0D08, 0xAD0A1E};
  const MrcInput    *Inputs;
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcStatus         Status;
  U8                Channel;
  U8                Rank;
  U8                Byte;
  U8                offset;
  U8                ChBitMask;
  U8                RankMask;  // RankBitMask for both channels
  U8                ValidRankMask;
  U8                Pattern[4][2];
  U8                PMask[sizeof (PMaskConst)];
  U8                AllGood;
  U8                AllGoodLoops;
  U8                DqOffset;
  U8                RankDouble;
  U8                RankHalf;
  U8                RankMod2;
  U8                Start;
  S8                ByteOff[MAX_CHANNEL][MAX_SDRAM_IN_DIMM]; // Passing offset for each ch/byte.
  S8                ByteSum[MAX_CHANNEL]; // Sum of passing offsets for a ch
  S8                TargetOffset;
  U16               ByteMask;
  U16               ValidByteMask;
  U16               Result;
  U16               SkipMe;
  U16               BytePass[MAX_CHANNEL]; // Bit mask indicating which ch/byte has passed
  S16               GlobalByteOff;
  U32               CRValue;
  U32               Offset;
  U32               CRAddDelay[MAX_CHANNEL];
  S32               LocalOffset;
  BOOL              Done;
#ifdef ULT_FLAG
  BOOL              Lpddr;
#endif //ULT_FLAG
  MRC_WDBPattern    WDBPattern;

  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_STRUCT                 ReutChPatWdbClMuxCfg;
  DDRDATA0CH0_CR_TXTRAINRANK0_STRUCT                              CrTxTrainRank;
#ifdef MRC_DEBUG_PRINT
  U32               ErrLower[MAX_CHANNEL];
  U32               ErrUpper[MAX_CHANNEL];
#endif // MRC_DEBUG_PRINT

  //
  // Setup REUT Pattern
  // Use 0x00FFC33C pattern to keep DQ-DQS simple but detect any failures
  // Same Pattern as NHM/WSM
  //
  Inputs              = &MrcData->SysIn.Inputs;
  Outputs             = &MrcData->SysOut.Outputs;
  ControllerOut       = &Outputs->Controller[0];
  Debug               = &Inputs->Debug;
  Status              = mrcSuccess;
  LocalOffset         = 0;
  Done                = TRUE;
  Pattern[0][0]       = 0x00;
  Pattern[0][1]       = 0xFF;
  Pattern[1][0]       = 0xFF;
  Pattern[1][1]       = 0x00;
  Pattern[2][0]       = 0xC3;
  Pattern[2][1]       = 0x3C;
  Pattern[3][0]       = 0x3C;
  Pattern[3][1]       = 0xC3;
  WDBPattern.IncRate  = 1;
  WDBPattern.Start    = 0;
  WDBPattern.Stop     = 3;
  WDBPattern.DQPat    = BasicVA;
  MrcOemMemoryCpy (PMask, (U8 *) PMaskConst, sizeof (PMask));
  MrcOemMemorySet ((U8 *) CRAddDelay, 0, sizeof (CRAddDelay));
#ifdef MRC_DEBUG_PRINT
  MrcOemMemorySet ((U8 *) ErrLower, 0, sizeof (ErrLower));
  MrcOemMemorySet ((U8 *) ErrUpper, 0, sizeof (ErrUpper));
#endif // MRC_DEBUG_PRINT

#ifdef ULT_FLAG
  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);
#endif // ULT_FLAG
  //
  // Spread = 8, Start = 0, 1, 2 and 3
  //
  for (Start = 0; Start < (sizeof (Pattern) / sizeof (Pattern[0])); Start++) {
    WriteWDBFixedPattern (MrcData, Pattern[Start], PMask, 8, Start);
  }

  //
  // Set LSFR Seed to be sequential
  //
  MrcProgramLFSR (MrcData, CleanUpSeeds);

  //
  // Set Channel and Rank bit masks
  //
  ChBitMask     = Outputs->ValidChBitMask;
  ValidRankMask = Outputs->ValidRankMask;
  ValidByteMask = (MRC_BIT0 << Outputs->SdramCount) - 1; // 0x1FF or 0xFF
  //
  // Setip IO test CmdPat=PatWrRd, NumCL=4, LC=4, REUTAddress, SOE=3,
  // WDBPattern, EnCADB=0, EnCKE=0, SubSeqWait=0 )
  //
  SetupIOTest (MrcData, ChBitMask, PatWrRd, 2, 4, &REUTAddress, NSOE, &WDBPattern, 0, 0, 0);

  //
  // Progam BITBUFFER for JWLT
  //
  ReutChPatWdbClMuxCfg.Data                     = 0;
  ReutChPatWdbClMuxCfg.Bits.Mux0_Control        = BTBUFFER;
  ReutChPatWdbClMuxCfg.Bits.Mux1_Control        = BTBUFFER;
  ReutChPatWdbClMuxCfg.Bits.Mux2_Control        = BTBUFFER;
  ReutChPatWdbClMuxCfg.Bits.ECC_Data_Source_Sel = 1;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Offset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG) * Channel);
      MrcWriteCR (MrcData, Offset, ReutChPatWdbClMuxCfg.Data);
    }
  }

  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    //
    // Select Rank for REUT test
    //
    ChBitMask   = 0;
    RankMask    = MRC_BIT0 << Rank;
    RankDouble  = Rank * 2;
    RankHalf    = Rank / 2;
    RankMod2    = Rank % 2;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChBitMask |= SelectReutRanks (MrcData, Channel, (RankMask), 0);
      BytePass[Channel] = ByteSum[Channel] = 0;
    }
    //
    // Skip if both channels empty
    //
    if (!(RankMask & ValidRankMask)) {
      continue;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRank %d\n", Rank);

    //
    // *************************************************
    // Sweep through the cycle offsets until we find a value that passes
    // *************************************************
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Sweep through the cycle offsets until we find a value that passes\n");

    if (RankMask & ValidRankMask) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel                 0                1\nDelay  DqOffset   Byte \t");
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE, (
        Outputs->SdramCount == MAX_SDRAM_IN_DIMM
        ) ? "0 1 2 3 4 5 6 7 8 0 1 2 3 4 5 6 7 8" : "0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7"
        );
    }

    for (offset = 0; offset < sizeof (Offsets); offset++) {
      //
      // Program new delay offsets to DQ/DQS timing:
      //
      /// @todo - Address debuging switches or remove.
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Program new delay offsets to DQ/DQS timing %d\n", Offsets[offset]);
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          continue;
        }

        ChannelOut = &ControllerOut->Channel[Channel];

        //
        // Calculate offsets
        //
        GlobalByteOff = 0;
        if (Offsets[offset] > MAX_ADD_DELAY) {
          CRAddDelay[Channel] = MrcBitSwap (CRAddDelay[Channel], MAX_ADD_DELAY, RankDouble, 2);
          GlobalByteOff       = 128 * (Offsets[offset] - MAX_ADD_DELAY);
        } else if (Offsets[offset] < 0) {
          CRAddDelay[Channel] = MrcBitSwap (CRAddDelay[Channel], 0, RankDouble, 2);
          GlobalByteOff       = 128 * Offsets[offset];
        } else {
          CRAddDelay[Channel] = MrcBitSwap (CRAddDelay[Channel], Offsets[offset], RankDouble, 2);
        }
        //
        // Write Tx DQ/DQS Flyby delays
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Add GlobalByteOff = %d to TxDQS Flyby delay: Ch %d \n", GlobalByteOff, Channel);
        //
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          CRValue                       = ChannelOut->TxDqs[Rank][Byte] + GlobalByteOff;
          CrTxTrainRank.Data            = 0;
          CrTxTrainRank.Bits.TxDqDelay  = CRValue + 32;
          CrTxTrainRank.Bits.TxDqsDelay = CRValue;
          CrTxTrainRank.Bits.TxEqualization = ChannelOut->TxEq[Rank][Byte];
          UpdateTxT (MrcData, Channel, Rank, Byte, 0x3, CrTxTrainRank.Data);
          //
          /// @todo - Address debuging switches or remove.
          // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CRValue = 0x%x \n", CrTxTrainRank.Data);
          //
        }
        //
        // Write Wr ADD Delays
        //
        Offset = MCHBAR_CH0_CR_SC_WR_ADD_DELAY_REG + ((MCHBAR_CH1_CR_SC_WR_ADD_DELAY_REG - MCHBAR_CH0_CR_SC_WR_ADD_DELAY_REG) * Channel);
        MrcWriteCR (MrcData, Offset, CRAddDelay[Channel]);
        //
        /// @todo - Address debuging switches or remove.
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CRAddDelay[%d] = 0x%x \n", Channel, CRAddDelay[Channel]);
        //
      }

#ifdef ULT_FLAG
      if (!Lpddr) {
#endif // ULT_FLAG
        //
        // Reset FIFOs & Reset DRAM DLL (Micron WorkAround). Wait 1uS for test to complete
        //
        Status = IoReset (MrcData);

        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
            ChannelOut = &ControllerOut->Channel[Channel];
            Status = MrcWriteMRS (
                      MrcData,
                      Channel,
                      RankMask,
                      mrMR0,
                      ChannelOut->Dimm[RankHalf].Rank[RankMod2].MR[mrMR0] | (MRC_BIT0 << 8)
                      );
          }
        }

        MrcWait (MrcData, (1 * HPET_1US));
#ifdef ULT_FLAG
      }
#endif // ULT_FLAG

      //
      // Run Test across all DqOffsets points
      //
      for (DqOffset = 0; DqOffset < DqOffsetMax; DqOffset++) {
        //
        // Update Offset
        //
        ChangeMargin (MrcData, WrT, DqOffsets[DqOffset], 0, 1, 0, Rank, 0, 0, 0, 0, MrcRegFileRank);

        //
        // Run Test
        // DQPat = BasicVA, DumArr, ClearErrors = 1, mode = 0
        //
        RunIOTest (MrcData, ChBitMask, BasicVA, DumArr, 1, 0);

        //
        // Update results for all ch/bytes
        //
        Done = TRUE;

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n% 3d\t% 3d\t       \t", Offsets[offset], DqOffsets[DqOffset]);

        //
        // Update results for all ch/bytes
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
            if (Channel == 0) {
              MRC_DEBUG_MSG (
                Debug,
                MSG_LEVEL_NOTE,
                (Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "                 " : "                "
                );
            }

            continue;
          }
          //
          // Read out per byte error results and check for any byte error
          //
          Offset = 4 + MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG +
            (
              (
                MCHBAR_CH1_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG -
                MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG
              ) * Channel
            );
          Result = (U16) MrcReadCR (MrcData, Offset);
          SkipMe = (Result & ValidByteMask) | BytePass[Channel];

#ifdef MRC_DEBUG_PRINT
          Offset = MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG + ((MCHBAR_CH1_CR_REUT_CH_ERR_DATA_STATUS_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG) * Channel);
          ErrLower[Channel] = MrcReadCR (MrcData, Offset);
          //
          // Lower 32 bits
          //
          ErrUpper[Channel] = MrcReadCR (MrcData, Offset + 4);
          //
          // Upper 32 bits
          //
#endif // MRC_DEBUG_PRINT
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            ByteMask = MRC_BIT0 << Byte;
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              ((Result & ValidByteMask) & ByteMask) ?
                "# " : //Fail
                ". " // Pass
              );
            //
            // If this byte has failed or previously passed, nothing to do
            //
            if (SkipMe & ByteMask) {
              continue;
            }

            BytePass[Channel] |= ByteMask;
            ByteOff[Channel][Byte] = Offsets[offset];
            ByteSum[Channel] += Offsets[offset];
          }
          //
          /// @todo - Address debuging switches or remove.
          // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "For DqOffsets %d: BytePass[%d] = 0x%X, Result = 0x%x, SkipMe = 0x%x\n", DqOffsets[DqOffset], Channel, BytePass[Channel], Result, SkipMe);
          //
          if (BytePass[Channel] != ValidByteMask) {
            Done = FALSE;
          }
        }

#ifdef MRC_DEBUG_PRINT
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "0x%08x%08x ",
              ErrUpper[Channel],
              ErrLower[Channel]
              );
          }
        }
#endif // MRC_DEBUG_PRINT
        //
        // Jump out of the for DqOffset loop if everybody is passing
        //
        if (Done == TRUE) {
          break;
        }
      }
      //
      // Jump out of the for offset loop if everybody is passing
      //
      if (Done == TRUE) {
        break;
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "\n");

    //
    // Walk through and find the correct value for each ch/byte
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
        continue;
      }

      ChannelOut = &ControllerOut->Channel[Channel];

      if (Done == FALSE) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_ERROR,
          "Error! Write Leveling CleanUp - Couldn't find a passing value for all bytes on Channel %u Rank %u:\nBytes - ",
          Channel,
          Rank
          );
#ifdef MRC_DEBUG_PRINT
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, ((BytePass[Channel] ^ ValidByteMask) & (1 << Byte)) ? "%d " : "", Byte);
        }
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "\n");
#endif
        return mrcWriteLevelingError;
      }
      //
      // Calculate the average offset, rounding up
      // Apply that offset to the global MC CRAddDelay register
      //
      TargetOffset = (ByteSum[Channel] + (S8) (Outputs->SdramCount / 2)) / (S8) Outputs->SdramCount;
      //
      /// @todo - Address debuging switches or remove.
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TargetOffset = 0x%x, ByteSum[%d] = 0x%x \n", TargetOffset, Channel, ByteSum[Channel]);
      //
      AllGood       = 0;
      AllGoodLoops  = 0;
      GlobalByteOff = 0;
      while (AllGood == 0) {
        //
        // Update CRAddDelay and GlobalByteOff
        //
        GlobalByteOff = 0;
        if (TargetOffset > MAX_ADD_DELAY) {
          CRAddDelay[Channel] = MrcBitSwap (CRAddDelay[Channel], MAX_ADD_DELAY, RankDouble, 2);
          GlobalByteOff       = 128 * (TargetOffset - MAX_ADD_DELAY);
        } else if (TargetOffset < 0) {
          CRAddDelay[Channel] = MrcBitSwap (CRAddDelay[Channel], 0, RankDouble, 2);
          GlobalByteOff       = 128 * TargetOffset;
        } else {
          CRAddDelay[Channel] = MrcBitSwap (CRAddDelay[Channel], TargetOffset, RankDouble, 2);
        }
        //
        /// @todo - Address debuging switches or remove.
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GlobalByteOff = 0x%x, CRAddDelay[%d] = 0x%x \n", GlobalByteOff, Channel, CRAddDelay[Channel]);
        // Refine TargetOffset to make sure it works for all byte lanes
        //
        AllGood = 1;
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          LocalOffset = GlobalByteOff + 128 * (ByteOff[Channel][Byte] - TargetOffset);
          if ((ChannelOut->TxDq[Rank][Byte] + LocalOffset) > (511 - 64)) {
            AllGood = 0;
            AllGoodLoops += 1;
            TargetOffset += 1;
            break;
          }

          if ((ChannelOut->TxDqs[Rank][Byte] + LocalOffset) < 96) {
            AllGood = 0;
            AllGoodLoops += 1;
            TargetOffset -= 1;
            break;
          }
          //
          /// @todo - Address debuging switches or remove.
          // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TargetOffset = 0x%x, ByteOff[%d][%d] = 0x%x \n", TargetOffset, Channel, Byte, ByteOff[Channel][Byte]);
          //
        }
        //
        // Avoid an infinite loop
        //
        if (AllGoodLoops > 3) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Error Handler1: JWL CleanUp - TargetOffset refining failed \n");
          return mrcFail;
        }
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d.R%d:  Offset\tFinalEdge\n", Channel, Rank);
      //
      // Program the final settings to the DQ bytes and update MrcData
      //
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        LocalOffset = GlobalByteOff + 128 * (ByteOff[Channel][Byte] - TargetOffset);
        ChannelOut->TxDq[Rank][Byte]  += (S16) LocalOffset;
        ChannelOut->TxDqs[Rank][Byte] += (S16) LocalOffset;
        UpdateTxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  B%d:   %d\t%d\n", Byte, LocalOffset, ChannelOut->TxDqs[Rank][Byte]);
      }
      //
      /// @todo - Address debuging switches or remove.
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "LocalOffset = 0x%x \n", LocalOffset);
      // Update MC Delay
      //
      Offset = MCHBAR_CH0_CR_SC_WR_ADD_DELAY_REG + ((MCHBAR_CH1_CR_SC_WR_ADD_DELAY_REG - MCHBAR_CH0_CR_SC_WR_ADD_DELAY_REG) * Channel);
      MrcWriteCR (MrcData, Offset, CRAddDelay[Channel]);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CRAddDelay[%d] = 0x%x \n", Channel, CRAddDelay[Channel]);

#ifdef ULT_FLAG
      if (!Lpddr) {
#endif // ULT_FLAG
        //
        // Clean up after Test
        //
        Status = MrcWriteMRS (
                  MrcData,
                  Channel,
                  RankMask,
                  mrMR0,
                  ChannelOut->Dimm[RankHalf].Rank[RankMod2].MR[mrMR0] | (MRC_BIT0 << 8)
                  );
        MrcWait (MrcData, (1 * HPET_1US));
#ifdef ULT_FLAG
      }
#endif // ULT_FLAG
    }
  }
  //
  // Clean up after Test
  // Restore WDB - VicRot=8, Start=0 and restore default seed
  //
  WriteWDBVAPattern (MrcData, 0, BASIC_VA_PATTERN_SPRED_8, 8, 0);
  MrcProgramLFSR (MrcData, NormalSeeds);

  MrcWriteCrMulticast (MrcData, DDRDATA_CR_DDRCRDATAOFFSETTRAIN_REG, 0);
  Status = IoReset (MrcData);

  return Status;
}

/**
@brief
  this function execute the Jedec write leveling training.
  Center TxDQS-CLK timing

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus -  if it succedes return mrcSuccess
**/
MrcStatus
MrcJedecWriteLevelingTraining (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput    *Inputs;
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  TOdtValue         *OdtTableIndex;
  MrcStatus         Status;
  U8                Channel;
  U8                Rank;
  U8                RankDouble;
  U8                RankHalf;
  U8                RankMod2;
  U8                Byte;
  U8                refbyte;
  U8                ChBitMask;
  U8                RankMask;  // RankBitMask for both channels
  U8                ValidRankMask;
  U8                OtherDimm;
  U8                OdtMatrix;
  U16               WLStart;
  U16               WLStop;
  U16               WLDelay;
  U8                WLStep;
  U32               WaitTime;
  U32               CRValue;
  U32               Offset;
  U32               DqsToggleTime;
  S32               InitialPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               InitialPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               CurrentPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               CurrentPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               LargestPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               LargestPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               iWidth;
  S32               cWidth;
  S32               lWidth;
  S32               ByteWidth;
  BOOL              Pass;
  BOOL              RankIsx16;
  BOOL              SavedRestoreMRS;
#ifdef ULT_FLAG
  BOOL              Lpddr;
#endif //ULT_FLAG

  DDR3_MODE_REGISTER_1_STRUCT                 Ddr3ModeRegister1;
  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT      DdrCrDataControl0;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT     DdrCrDataControl2;
  MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_STRUCT  ReutChMiscOdtCtrl;
  DDRDATA0CH0_CR_DDRCRDATACONTROL0_STRUCT     DdrCrData0Control0;
  DDRDATA0CH0_CR_DATATRAINFEEDBACK_STRUCT     DataTrainFeedback;

  Inputs        = &MrcData->SysIn.Inputs;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Debug         = &Inputs->Debug;
  Status        = mrcSuccess;
  OdtTableIndex = NULL;
  CRValue       = 0;
  ChBitMask     = Outputs->ValidChBitMask;
  ValidRankMask = Outputs->ValidRankMask;

  // Save the flag and force MRS recalculation
  SavedRestoreMRS = Outputs->RestoreMRs;
  Outputs->RestoreMRs = FALSE;

  DqsToggleTime = 1024;
#ifdef ULT_FLAG
  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  if (Lpddr) {
    DqsToggleTime = 2048;
  }
#endif // ULT_FLAG

  //
  // Enabling WLmode causes DQS to toggle for 1024 qclk.  Wait for this to stop
  // Round up to nearest uS
  //

  WaitTime = (Outputs->Qclkps * DqsToggleTime + 500000) / 1000000;
  //
  // Propagate delay values (without a write command) and Set Qoff on all ranks.
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut = &ControllerOut->Channel[Channel];

      //
      // Propagate delay values from rank 0 to prevent assertion failures in RTL
      //
      DdrCrDataControl0.Data            = ChannelOut->DqControl0.Data;
      DdrCrDataControl0.Bits.ReadRFRd   = 0;
      DdrCrDataControl0.Bits.ReadRFWr   = 1;
      DdrCrDataControl0.Bits.ReadRFRank = 0;
      Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG + ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);

      //
      // Set ForceBiasOn and make sure ForceRxAmpOn is cleared
      //
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        DdrCrDataControl2.Data              = ChannelOut->DqControl2[Byte].Data;
        DdrCrDataControl2.Bits.ForceBiasOn  = 1;
        DdrCrDataControl2.Bits.ForceRxOn    = 0;
#ifdef ULT_FLAG
        if (Lpddr) {
          DdrCrDataControl2.Bits.WlLongDelEn  = 1;
        }
#endif // ULT_FLAG
        Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
          ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
          ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
        MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
      }
    }
  }

  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    RankMask = MRC_BIT0 << Rank;
    if (!(RankMask & ValidRankMask)) {
      //
      // Skip if all channels empty
      //
      continue;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nRank %d\n", Rank);
    RankDouble  = Rank * 2;
    RankHalf    = Rank / 2;
    RankMod2    = Rank % 2;
    //
    // Program MR1: Set A7 to enter Write Leveling mode
    // Write MaskRasWe to prevent scheduler from issuing non-Read commands
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {

        ChannelOut = &ControllerOut->Channel[Channel];

#ifdef ULT_FLAG
        //
        // Enable WL mode in MR2[7]
        //
        if (Lpddr) {
          CRValue = (ChannelOut->Dimm[RankHalf].Rank[RankMod2].MR[mrMR2]);
          Status  = MrcIssueMrw (MrcData, Channel, Rank, mrMR2, (CRValue | MRC_BIT7), FALSE, FALSE);
        } else
#endif // ULT_FLAG
        {
          Ddr3ModeRegister1.Data = ChannelOut->Dimm[RankHalf].Rank[RankMod2].MR[mrMR1];
          Ddr3ModeRegister1.Bits.WriteLeveling = 1;

          OdtTableIndex = GetOdtTableIndex (MrcData, Channel, RANK_TO_DIMM_NUMBER (Rank));
          if (OdtTableIndex == NULL) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Error: OdtTableIndex is NULL!\n");
            return mrcFail;
          }
#ifdef ULT_FLAG
          if (Inputs->CpuModel == cmHSW_ULT) {  // DDR3L case
            Ddr3ModeRegister1 = UpdateRttNomValue (MrcData, OdtTableIndex->RttWr, Ddr3ModeRegister1);
          }
#endif // ULT_FLAG

          //
          // In write leveling mode Rtt_Nom = Rtt_Wr ONLY for 2DPC
          //
          if (ChannelOut->DimmCount == 2) {
            Ddr3ModeRegister1 = UpdateRttNomValue (MrcData, OdtTableIndex->RttWr, Ddr3ModeRegister1);
          }
          Status = MrcWriteMRS (MrcData, Channel, RankMask, mrMR1, (U16) Ddr3ModeRegister1.Data);
        }

        //
        // Assert ODT for myself
        //
        OdtMatrix = RankMask;
        if (ChannelOut->DimmCount == 2) {
          //
          // Calculate non-target DIMM
          //
          OtherDimm = ((Rank + 2) / 2) & MRC_BIT0;
          //
          // Assert ODT for non-target DIMM
          //
          OdtMatrix |= 1 << (2 * OtherDimm);
        }

        ReutChMiscOdtCtrl.Data = 0;
#ifdef ULT_FLAG
        if (Lpddr) {
          //
          // Only one ODT pin for ULT
          //
          ReutChMiscOdtCtrl.Bits.ODT_On       = 1;
          ReutChMiscOdtCtrl.Bits.ODT_Override = 1;
        }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
        if ((Inputs->CpuModel == cmHSW) || (Inputs->CpuModel == cmCRW)) {
          ReutChMiscOdtCtrl.Bits.ODT_On       = OdtMatrix;
          ReutChMiscOdtCtrl.Bits.ODT_Override = MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_ODT_Override_MAX;
        }
#endif // TRAD_FLAG
        Offset = MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG +
          ((MCHBAR_CH1_CR_REUT_CH_MISC_ODT_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG) * Channel);
        MrcWriteCR (MrcData, Offset, ReutChMiscOdtCtrl.Data);
      }
    }  // for Channel

    //
    // ******************************************
    // STEP 1 and 2: Find middle of high region
    // ******************************************
    //
    WLStart = 192;
    WLStop  = 320;
    WLStep  = 2;

    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "\n\tCh0\t\t\t\t\t\t\t\t%sCh1\n",
      (Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "\t" : ""
      );

    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "WLDelay%s%s",
      (Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "\t0\t1\t2\t3\t4\t5\t6\t7\t8" : "\t0\t1\t2\t3\t4\t5\t6\t7",
      (Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "\t0\t1\t2\t3\t4\t5\t6\t7\t8" : "\t0\t1\t2\t3\t4\t5\t6\t7"
      );

    for (WLDelay = WLStart; WLDelay < WLStop; WLDelay += WLStep) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n %d:", WLDelay);
      //
      // Program WL DQS Delays:
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {

          ChannelOut = &ControllerOut->Channel[Channel];

          //
          // Enable Write Level Mode in DDR and Propagate delay values (without a write command).
          // Stay in WLMode.
          //
          DdrCrData0Control0.Data                 = ChannelOut->DqControl0.Data;
          DdrCrData0Control0.Bits.WLTrainingMode  = 1;
          DdrCrData0Control0.Bits.TxPiOn          = 1;
          DdrCrData0Control0.Bits.ReadRFRd        = 0;
          DdrCrData0Control0.Bits.ReadRFWr        = 1;
          DdrCrData0Control0.Bits.ReadRFRank      = Rank;
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            UpdateTxT (MrcData, Channel, Rank, Byte, 1, WLDelay);

            Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL0_REG +
              ((DDRDATA0CH1_CR_DDRCRDATACONTROL0_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL0_REG) * Channel) +
              ((DDRDATA1CH0_CR_DDRCRDATACONTROL0_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL0_REG) * Byte);
            MrcWriteCR (MrcData, Offset, DdrCrData0Control0.Data);
          }
        }
      }

      if (WLDelay == WLStart) {
        MrcWait (MrcData, (WaitTime * HPET_1US));  // Wait for the first burst to finish
      }

      Status = IoReset (MrcData);

      MrcWait (MrcData, (WaitTime * HPET_1US));

      //
      // Update results for all Channels/Bytes
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "\t\t\t\t\t\t\t\t%s",
            (Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "\t" : ""
            );
          continue;
        }
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          Offset = DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG +
            ((DDRDATA0CH1_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Channel) +
            ((DDRDATA1CH0_CR_DATATRAINFEEDBACK_REG - DDRDATA0CH0_CR_DATATRAINFEEDBACK_REG) * Byte);
          DataTrainFeedback.Data  = MrcReadCR (MrcData, Offset);
          Pass                    = (DataTrainFeedback.Bits.DataTrainFeedback >= 16);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t%c%d", Pass ? '.' : '#', DataTrainFeedback.Data);
          if (WLDelay == WLStart) {
            if (Pass) {
              InitialPassingStart[Channel][Byte] = InitialPassingEnd[Channel][Byte]  = WLStart;
              CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte]  = WLStart;
              LargestPassingStart[Channel][Byte] = LargestPassingEnd[Channel][Byte]  = WLStart;
            } else {
              InitialPassingStart[Channel][Byte] = InitialPassingEnd[Channel][Byte]  = -WLStep;
              CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte]  = -WLStep;
              LargestPassingStart[Channel][Byte] = LargestPassingEnd[Channel][Byte]  = -WLStep;
            }
          } else {
            if (Pass) {
              if (InitialPassingEnd[Channel][Byte] == (WLDelay - WLStep)) {
                InitialPassingEnd[Channel][Byte] = WLDelay;
              }

              if (CurrentPassingEnd[Channel][Byte] == (WLDelay - WLStep)) {
                CurrentPassingEnd[Channel][Byte] = WLDelay;
              } else {
                CurrentPassingStart[Channel][Byte] = CurrentPassingEnd[Channel][Byte] = WLDelay;
              }
              //
              // Special case for last step: Append Initial Passing Region
              // WLDelay should be considered a continuous range that wraps around 0
              //
              if ((WLDelay >= (WLStop - WLStep)) && (InitialPassingStart[Channel][Byte] == WLStart)) {
                iWidth = (InitialPassingEnd[Channel][Byte] - InitialPassingStart[Channel][Byte]);
                CurrentPassingEnd[Channel][Byte] += (WLStep + iWidth);
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
        } // for Byte
      } // for Channel
    } // for WLDelay

#ifdef MRC_DEBUG_PRINT
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\tInitPassSt\tInitPassEn\tCurrPassSt\tCurrPassEn\tLargPassSt\tLargPassEn\n");

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d\n", Channel);
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "   B%d:\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",
            Byte,
            InitialPassingStart[Channel][Byte],
            InitialPassingEnd[Channel][Byte],
            CurrentPassingStart[Channel][Byte],
            CurrentPassingEnd[Channel][Byte],
            LargestPassingStart[Channel][Byte],
            LargestPassingEnd[Channel][Byte]
            );
      }
    }
  }
#endif // MRC_DEBUG_PRINT

    //
    // Clean up after step
    // Very coarsely adjust for any cycle errors
    // Program values for TxDQS
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {

        ChannelOut = &ControllerOut->Channel[Channel];

        //
        // Check if rank is a x16
        //
        RankIsx16 = (ChannelOut->Dimm[RankHalf].SdramWidth == 16 ? TRUE : FALSE);

        //
        // Clear ODT before MRS (JEDEC Spec)
        //
        Offset = MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG +
          ((MCHBAR_CH1_CR_REUT_CH_MISC_ODT_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_ODT_CTRL_REG) * Channel);
        MrcWriteCR (MrcData, Offset, 0);

#ifdef ULT_FLAG
        //
        // Restore MR2 values
        //
        if (Lpddr) {
          CRValue = (ChannelOut->Dimm[RankHalf].Rank[RankMod2].MR[mrMR2]);
          Status  = MrcIssueMrw (MrcData, Channel, Rank, mrMR2, CRValue, FALSE, FALSE);
        } else
#endif // ULT_FLAG
        {
          //
          // Restore Write Leveling mode and Rtt_Nom for this rank
          //
          CRValue = (ChannelOut->Dimm[RankHalf].Rank[RankMod2].MR[mrMR1]);
          Status  = MrcWriteMRS (MrcData, Channel, RankMask, mrMR1, (U16) CRValue);
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d.R%d:  LftEdge Width\n", Channel, Rank);
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          ByteWidth = LargestPassingEnd[Channel][Byte] - LargestPassingStart[Channel][Byte];
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "  B%d:   %d\t%d\n",
            Byte,
            LargestPassingStart[Channel][Byte],
            ByteWidth
            );

          //
          // Check if width is valid
          //
          if ((ByteWidth <= 32) || (ByteWidth >= 96)) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_ERROR,
              "\nERROR! Width region outside expected limits for Channel: %u Rank %u Byte: %u\n",
              Channel,
              Rank,
              Byte
              );
            return mrcWriteLevelingError;
          }
          //
          // Align byte pairs if DIMM is x16
          //
          if (RankIsx16 && (Byte & MRC_BIT0)) {
            //
            // If odd byte number (1, 3, 5 or 7)
            //
            refbyte = Byte - 1;
            if (LargestPassingStart[Channel][Byte] > (LargestPassingStart[Channel][refbyte] + 64)) {
              LargestPassingStart[Channel][Byte] -= 128;
            }

            if (LargestPassingStart[Channel][Byte] < (LargestPassingStart[Channel][refbyte] - 64)) {
              LargestPassingStart[Channel][Byte] += 128;
            }
          }

#ifdef ULT_FLAG
          if (Inputs->CpuModel == cmHSW_ULT) {
            //
            // Fix for b4618067 - need to add 1 QCLK to DqsPi
            //
            LargestPassingStart[Channel][Byte] += 64;
          }
#endif // ULT_FLAG

          ChannelOut->TxDqs[Rank][Byte] = (U16) LargestPassingStart[Channel][Byte];
          ChannelOut->TxDq[Rank][Byte]  = (U16) (LargestPassingStart[Channel][Byte] + 32);
          UpdateTxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
        }
      }
    }
  }
  //
  // Clean up after Test
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {

      ChannelOut = &ControllerOut->Channel[Channel];

      Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
        ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      MrcWriteCrMulticast (MrcData, Offset, ChannelOut->DqControl0.Data);

      //
      // Restore DqControl2 values.
      //
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
          ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
          ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
        MrcWriteCR (MrcData, Offset, ChannelOut->DqControl2[Byte].Data);
      }
    }
  }

#ifdef ULT_FLAG
  if (!Lpddr)
#endif // ULT_FLAG
  {
    //
    // DLLEnable=0, Dic=0, Al=0, Level=0, Tdqs=0, Qoff=0
    //
    Status = MrcSetMR1 (MrcData, 0, DIMMRON, 0, 0, 0, 0);
    if (Status != mrcSuccess) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: RESET FAIL - WLT\n");
      return Status;
    }
  }

  // Restore flag
  Outputs->RestoreMRs = SavedRestoreMRS;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nJedec Write Leveling CLEANUP\n");
  Status = MrcJedecWriteLevelingCleanUp (MrcData);

  return Status;
}
