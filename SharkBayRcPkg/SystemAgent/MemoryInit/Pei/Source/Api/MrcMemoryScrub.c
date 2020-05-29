/** @file

  This file include all the logic and API for cleaning the memory when LT is enable.
  
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

//
// Include files
//
#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcMemoryScrub.h>

/**
@brief
  This function sets all the memory to a known value when ECC is enabled and
  either we are not in warm boot or we are in warm boot and TXT is set.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess if the clean succeeded, otherwise an error status.
**/
MrcStatus
MrcEccClean (
  IN     MrcParameters *const MrcData
  )
{
  const MrcInput        *Inputs;
  const MrcDebug        *Debug;
  const U8              WrapCarryEn[MrcReutFieldMax] = {0, 0, 0, 0};
  const U8              WrapTriggerEn[MrcReutFieldMax] = {0, 0, 1, 0};  // Trigger Stop on Bank Wrap
  const U8              AddrInvertEn[MrcReutFieldMax] = {0, 0, 0, 0};
  MrcControllerOut      *ControllerOut;
  MrcDimmOut            *DimmOut;
  MrcOutput             *Outputs;
  MrcStatus             Status;
  U32                   ReutSubSeqCtl0Data;
  U8                    Pattern;
  U8                    PMask;
  U8                    Rank;
  U8                    Bank;
  U8                    Channel;
  U8                    ActiveChBitMask;
  U8                    RankToDimm;
  MRC_REUTAddress       ReutAddress;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT     ReutChSeqCfg;
  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_STRUCT  ReutChPatWdbCl;

  Inputs        = &MrcData->SysIn.Inputs;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Debug         = &Inputs->Debug;
  Status        = mrcSuccess;
  Pattern       = 0;
  PMask         = 0;
  MrcOemMemorySet ((U8 *) &ReutAddress, 0, sizeof (ReutAddress));

  if ((Outputs->EccSupport == TRUE) || (Inputs->OemCleanMemory == TRUE)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Scrubbing Memory\n");
    ReutAddress.IncVal[MrcReutFieldCol] = 1;  // Each write is 1 cache line which is 8 column addresses worth of data.
    ReutAddress.IncVal[MrcReutFieldRow] = 1;  // Walk through rows 1 at a time.

    //
    // Setup the first cache line to zeros.
    //
    WriteWDBFixedPattern (MrcData, &Pattern, &PMask, 1, 0);

    //
    // Setup Reut for both channels.
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcChannelExist (Outputs, Channel)) {
        continue;
      }

      //
      // Write initial Reut Address Values.
      //
      MrcProgramSequenceAddress (
       MrcData,
       Channel,
       ReutAddress.Start,
       NULL,                // Stop
       ReutAddress.Order,
       ReutAddress.IncRate,
       ReutAddress.IncVal,
       WrapTriggerEn,
       WrapCarryEn,
       AddrInvertEn,
       0,                 // AddrInvertRate
       FALSE
       );

      //
      // Set Reut to Write
      //
      ReutChSeqCfg.Data                                    = 0;
      ReutChSeqCfg.Bits.Initialization_Mode                = REUT_Testing_Mode;
      ReutChSeqCfg.Bits.Global_Control                     = 1;
      ReutChSeqCfg.Bits.Start_Test_Delay                   = 2;
      ReutChSeqCfg.Bits.Address_Update_Rate_Mode           = 1;
      ReutChSeqCfg.Bits.Stop_Base_Sequence_On_Wrap_Trigger = 1;
      MrcWriteCR64 (
        MrcData,
        MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel),
        ReutChSeqCfg.Data
        );

      //
      // Program new loopcount registers based on stepping.
      //
      if (
          (Inputs->CpuModel == cmHSW && Inputs->CpuStepping >= csHswC0) ||
          (Inputs->CpuModel == cmCRW && Inputs->CpuStepping >= csCrwC0) ||
          (Inputs->CpuModel == cmHSW_ULT && Inputs->CpuStepping >= csHswUltC0) ||
          (Inputs->CpuModel == cmBDW)
         ) {
        MrcWriteCR (
          MrcData,
          MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG +
            ((MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG)
            * Channel),
          0
          );
      }

      //
      // Set up the Subsequence control.
      //
      ReutSubSeqCtl0Data = 0;
      SetSubsequenceType (MrcData, &ReutSubSeqCtl0Data, BWr);
      MrcWriteCR (
        MrcData,
        MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH1_SUBSEQ_CTL_MCMAIN_0_REG - MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG) * Channel),
        ReutSubSeqCtl0Data
        );

      //
      // Program Write Data Buffer Control.  Since we are using 1 cache line, we only need
      // to set the increment scale to linear.
      //
      ReutChPatWdbCl.Data = 0;
      ReutChPatWdbCl.Bits.WDB_Increment_Scale = 1;
      MrcWriteCR (
        MrcData,
        MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG) * Channel),
        ReutChPatWdbCl.Data
        );
    }

    //
    // Run Per Rank
    //
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if ((MRC_BIT0 << Rank) & Outputs->ValidRankMask) {
        //
        // Determine the Active Channels
        //
        ActiveChBitMask = 0;
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          ActiveChBitMask |= SelectReutRanks (MrcData, Channel, MRC_BIT0 << Rank, 0);
        }

        //
        // Counter registers are not large enough to walk through 1 Rank for LPDDR3 support due to 11 column bits.
        // Must walk through memory on a bank loop.
        //
        for (Bank = 0; Bank < 8; Bank++) {
          ReutAddress.Start[MrcReutFieldBank] = Bank;
          ReutAddress.Stop[MrcReutFieldBank]  = Bank;

          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            if (ActiveChBitMask & (MRC_BIT0 << Channel)) {
              //
              // Update Bank start/stop
              //
              RankToDimm                           = RANK_TO_DIMM_NUMBER (Rank);
              DimmOut                              = &ControllerOut->Channel[Channel].Dimm[RankToDimm];
              ReutAddress.Stop[MrcReutFieldRow]    = (U16) DimmOut->RowSize - 1;
              ReutAddress.Stop[MrcReutFieldCol]    = DimmOut->ColumnSize - WDB_CACHE_LINE_SIZE;
              ReutAddress.IncRate[MrcReutFieldRow] = DimmOut->ColumnSize / WDB_CACHE_LINE_SIZE;
              MrcProgramSequenceAddress (
                MrcData,
                Channel,
                ReutAddress.Start,
                ReutAddress.Stop,
                NULL,                 // Order
                ReutAddress.IncRate,
                NULL,                 // IncVal
                NULL,                 // WrapTriggerEn
                NULL,                 // WrapCarryEn
                NULL,                 // AddrInvertEn
                0,                    // AddrInvertRate
                FALSE
                );
            }
          }

          //
          // Run the test
          //
          Status = MrcRunMemoryScrub (MrcData, ActiveChBitMask);
          if (Status != mrcSuccess) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Rank %d error!\n", Rank);
            break;
          }
        }
      }

      if (Status != mrcSuccess) {
        break;
      }
    }

    //
    // Return to normal operation mode
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        ReutChSeqCfg.Data                     = 0;
        ReutChSeqCfg.Bits.Initialization_Mode = NOP_Mode;
        MrcWriteCR (
          MrcData,
          MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
          ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel),
          (U32) ReutChSeqCfg.Data
          );
      }
    }

    if (Status != mrcSuccess) {
      MrcOemDebugHook (MrcData, MRC_ECC_CLEAN_ERROR);
    }
  }

  return Status;
}

/**
@brief
  This function performs a memory alias check.

  @param[in] MrcData - The global host structure

  @retval mrcSuccess or error value.
**/
MrcStatus
MrcAliasCheck (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcDebug    *Debug;
  const MrcInput    *Inputs;
  const U8          WrapCarryEn[MrcReutFieldMax]   = {0, 0, 0, 0};
  const U8          WrapTriggerEn[MrcReutFieldMax] = {0, 0, 0, 0};  // Trigger Stop on Bank Wrap
  const U8          AddrInvertEn[MrcReutFieldMax] = {0, 0, 0, 0};
  const U16         SdramCapacityTable[] = {256, 512, 1024, 2048, 4096, 8192, 16384, 32768}; // Mb
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcDimmOut        *DimmOut;
  MrcStatus         Status;
  MrcDdrType        DdrType;
  BOOL              InvalidSpdAddressingCapacity;
  U32               SdramAddressingCapacity;
  U32               CrOffset;
  U16               SdramCapacity;
  U16               WritesPerPage;
  U16               ColumnIncValUnaligned;
  U8                Rank;
  U8                RankToDimm;
  U8                Channel;
  U8                ActiveChBitMask;
  MRC_REUTAddress   ReutAddress;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT       ReutChSeqCfg;
  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_STRUCT    ReutChPatWdbCl;
  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_STRUCT ReutChPatWdbClMuxCfg;
  MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_STRUCT   ReutSubSeqCtl;
  MCDECS_CR_MAD_DIMM_CH0_MCMAIN_STRUCT            MadDimmCh[MAX_CHANNEL];
  MCDECS_CR_MAD_DIMM_CH0_MCMAIN_STRUCT            MadDimm;

  Outputs                       = &MrcData->SysOut.Outputs;
  Inputs                        = &MrcData->SysIn.Inputs;
  Debug                         = &Inputs->Debug;
  ControllerOut                 = &Outputs->Controller[0];
  Status                        = mrcSuccess;
  InvalidSpdAddressingCapacity  = FALSE;
  DdrType                       = Outputs->DdrType;
  //
  // Check to see if the SDRAM Addressing * Primary Bus Width == SDRAM capacity.
  // If not, report an alias and exit.
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank += MAX_RANK_IN_DIMM) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
        RankToDimm              = RANK_TO_DIMM_NUMBER (Rank);
        DimmOut                 = &ControllerOut->Channel[Channel].Dimm[RankToDimm];
        SdramAddressingCapacity = (DimmOut->ColumnSize * DimmOut->RowSize);
        //
        // Since the minimum number of row and coulmn bits are 12 and 9 respectivly,
        // we can shift by 20 to get the result in Mb before multiplying by the bus width.
        //
        SdramAddressingCapacity  = SdramAddressingCapacity >> 20;
        SdramAddressingCapacity *= DimmOut->Banks;
        SdramAddressingCapacity *= (DimmOut->BankGroups > 0) ? DimmOut->BankGroups : 1;
        SdramAddressingCapacity *= DimmOut->SdramWidth;
        SdramCapacity            = SdramCapacityTable[DimmOut->DensityIndex];
        if (SdramCapacity != SdramAddressingCapacity) {
          InvalidSpdAddressingCapacity = TRUE;
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_ERROR,
            "ERROR: Channel %d Dimm %d SPD SDRAM Adressing Capacity(0x%xMb) does not match SDRAM Capacity(0x%xMb)\nPlease verify:\n",
            Channel,
            RankToDimm,
            SdramAddressingCapacity,
            SdramCapacity
            );
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_ERROR,
            " Capacity: 0x%x\n RowSize: 0x%x\n ColumnSize: 0x%x\n Banks: 0x%x\n Bank Groups: 0x%x\n Device Width: 0x%x\n",
            SdramCapacity,
            DimmOut->RowSize,
            DimmOut->ColumnSize,
            DimmOut->Banks,
            DimmOut->BankGroups,
            DimmOut->SdramWidth
            );
          break;
        }
      }
    }
  }
  //
  // Since we will not hang the system, signal that an Alias could exist and return mrcSuccess.
  //
  if (TRUE == InvalidSpdAddressingCapacity) {
    Outputs->SpdSecurityStatus = MrcSpdStatusAliased;
    return Status;
  }

  if ((Inputs->CpuModel == cmHSW && Inputs->CpuStepping >= csHswB0) || (Inputs->CpuModel != cmHSW)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Performing Alias Test\n");
    MrcOemMemorySet ((U8 *) &ReutAddress, 0, sizeof (ReutAddress));

    //
    // Determine if we are ECC enabled.  If so, disable ECC since the ECC scrub has yet to occur.
    //
    if (Outputs->EccSupport == TRUE) {
      MRC_DEBUG_MSG(Debug, MSG_LEVEL_NOTE, "ECC enabled.  Disabling ECC for the test.  Must scrub after this!!!\n");
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcChannelExist (Outputs, Channel)) {
          CrOffset = MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG +
            ((MCDECS_CR_MAD_DIMM_CH1_MCMAIN_REG - MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG) * Channel);

          MadDimmCh[Channel].Data = MrcReadCR (MrcData, CrOffset);
          MadDimm.Data            = MadDimmCh[Channel].Data;
          MadDimm.Bits.ECC        = emNoEcc;
          MrcWriteCR (MrcData, CrOffset, MadDimm.Data);
        }
      }
    }

    //
    // Test Initialization
    //
    //
    // Start with IncRate = 3 so we have 4 column writes per page.  This will change with Column Size.
    // Must have 4 (reg + 1) writes to move to the next LFSR code for unique values.
    //
    ReutAddress.IncRate[MrcReutFieldRow]  = 3;
    //
    // IncVal[Col] is chosen to be 1/4 of the minimum column supported to get 4 writes per page.
    // Each write is 1 cache line (8 column addresses worth of data).
    // IncVal is on a cache line basis when programmed.  Account for this here ( >> 3).
    //
    ColumnIncValUnaligned = MRC_BIT10 >> 2; // divide by 4
    ReutAddress.IncVal[MrcReutFieldCol]   = ColumnIncValUnaligned >> 3; // cache line shift
    //
    // Smallest Row address size is 2^12, but Row_Base_Address_Increment is a 12-bit signed field [0-11].
    // Thus we have to increment by 2^10.
    //
    ReutAddress.IncVal[MrcReutFieldRow]   = MRC_BIT10;
    ReutAddress.Stop[MrcReutFieldCol]     = 24; // 4 ([0-3] << 3) column writes before wrapping
    ReutAddress.Start[MrcReutFieldBank]   = 1;
    ReutAddress.Stop[MrcReutFieldBank]    = 1;

    //
    // Setup Reut all present channels.
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcChannelExist (Outputs, Channel)) {
        continue;
      }

      //
      // Write initial Reut Address Values.
      //
      MrcProgramSequenceAddress (
       MrcData,
       Channel,
       ReutAddress.Start,
       NULL,              // Stop
       ReutAddress.Order,
       ReutAddress.IncRate,
       ReutAddress.IncVal,
       WrapTriggerEn,
       WrapCarryEn,
       AddrInvertEn,
       0,
       FALSE
       );

      //
      // Set Reut to Write
      //
      ReutChSeqCfg.Data                                     = 0;
      ReutChSeqCfg.Bits.Initialization_Mode                 = REUT_Testing_Mode;
      ReutChSeqCfg.Bits.Global_Control                      = 1;
      ReutChSeqCfg.Bits.Start_Test_Delay                    = 2;
      ReutChSeqCfg.Bits.Subsequence_End_Pointer             = 1;

     if (
          (Inputs->CpuModel == cmHSW && Inputs->CpuStepping < csHswC0) ||
          (Inputs->CpuModel == cmCRW && Inputs->CpuStepping < csCrwC0) ||
          (Inputs->CpuModel == cmHSW_ULT && Inputs->CpuStepping < csHswUltC0) ||
          (Inputs->CpuModel == cmBDW)
         ) {
        ReutChSeqCfg.Bits.Loopcount = 1;
      } else {
        CrOffset = MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG +
          ((MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG) *
          Channel);
        MrcWriteCR (MrcData, CrOffset, 1);
      }

      CrOffset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel);
      MrcWriteCR64 (
        MrcData,
        CrOffset,
        ReutChSeqCfg.Data
        );

      //
      // Program Write Data Buffer Control.
      //
      ReutChPatWdbCl.Data = 0;
      ReutChPatWdbCl.Bits.WDB_Increment_Scale = 1;
      MrcWriteCR (
        MrcData,
        MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG) * Channel),
        ReutChPatWdbCl.Data
        );

      ReutChPatWdbClMuxCfg.Bits.ECC_Data_Source_Sel = 1;
      ReutChPatWdbClMuxCfg.Bits.Mux2_Control        = LFSRMode;
      ReutChPatWdbClMuxCfg.Bits.Mux1_Control        = LFSRMode;
      ReutChPatWdbClMuxCfg.Bits.Mux0_Control        = LFSRMode;
      CrOffset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG) * Channel);
      MrcWriteCR (MrcData, CrOffset, ReutChPatWdbClMuxCfg.Data);
    }

    //
    // Run test Per Dimm
    //
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank += MAX_RANK_IN_DIMM){
      if ((MRC_BIT0 << Rank) & Outputs->ValidRankMask) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Testing Dimm %d\n", Rank / 2);
        //
        // Determine Active Channels
        //
        ActiveChBitMask = 0;
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          ActiveChBitMask |= SelectReutRanks (MrcData, Channel, MRC_BIT0 << Rank, 0);
          //
          // Update Rank stop address based on DIMM SPD if Active.
          //
          if (ActiveChBitMask & (MRC_BIT0 << Channel)) {
            RankToDimm  = RANK_TO_DIMM_NUMBER (Rank);
            DimmOut     = &ControllerOut->Channel[Channel].Dimm[RankToDimm];
            //
            // Since we're counting cache lines and won't wrap the row address,
            // program Row Stop to RowSize - 1 to avoid resetting the current address.
            // Column must wrap.  The wrap occurs on the increment which is after writing,
            // to that address.  Thus, we set wrap to be the last accessed column.
            //
            WritesPerPage = DimmOut->ColumnSize / ColumnIncValUnaligned;  // Should be >= 4
            ReutAddress.Stop[MrcReutFieldRow]     = (U16) DimmOut->RowSize - 1;
            ReutAddress.Stop[MrcReutFieldCol]     = DimmOut->ColumnSize - ColumnIncValUnaligned;
            ReutAddress.IncRate[MrcReutFieldRow]  = WritesPerPage - 1; // IncRate is +1 the programmed value

            MrcProgramSequenceAddress (
              MrcData,
              Channel,
              NULL,
              ReutAddress.Stop,
              NULL,
              ReutAddress.IncRate,
              NULL,
              NULL,
              NULL,
              NULL,
              0,
              FALSE
              );
            //
            // Set up the Subsequence control.
            //
            CrOffset = MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG +
              ((MCDFXS_CR_REUT_CH1_SUBSEQ_CTL_MCMAIN_0_REG - MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG) * Channel);
            //
            // @todo: Review that the settings programmed here are common between the steppings.
            //
            ReutSubSeqCtl.Data = 0;
            ReutSubSeqCtl.Bits.Subsequence_Type = BWr;
            //
            // Instead of matching wrap addresses, we will stop on 1 less cache line write from the top.
            // This works because when aliasing occurs, the physical addressing size must double for row/col.
            //
            ReutSubSeqCtl.Bits.Number_of_Cachelines = MrcLog2 (((DimmOut->RowSize / MRC_BIT10) * WritesPerPage) - 1);
            MrcWriteCR (
              MrcData,
              CrOffset,
              ReutSubSeqCtl.Data
              );

            CrOffset += MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG;
            ReutSubSeqCtl.Bits.Reset_Current_Base_Address_To_Start  = 1;
            ReutSubSeqCtl.Bits.Subsequence_Type = BRd;
            MrcWriteCR (
              MrcData,
              CrOffset,
              ReutSubSeqCtl.Data
              );
          }
        }

        //
        // Run the test
        //
        Status = MrcRunMemoryScrub (MrcData, ActiveChBitMask);
        if (Status != mrcSuccess) {
          break;
        }
      }
    }

    if (Outputs->EccSupport == TRUE) {
      MRC_DEBUG_MSG(Debug, MSG_LEVEL_NOTE, "ReEnabling ECC Logic.  Must scrub after this!\n");
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcChannelExist (Outputs, Channel)) {
          CrOffset = MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG +
            ((MCDECS_CR_MAD_DIMM_CH1_MCMAIN_REG - MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG) * Channel);

          MrcWriteCR (MrcData, CrOffset, MadDimmCh[Channel].Data);
        }
      }
    }
    //
    // Wait 4 usec after enabling the ECC IO, needed by HW
    //
    MrcWait (MrcData, 4 * HPET_1US);

    //
    // Return to normal operation mode
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        ReutChSeqCfg.Data                     = 0;
        ReutChSeqCfg.Bits.Initialization_Mode = NOP_Mode;
        MrcWriteCR (
          MrcData,
          MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
          ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel),
          (U32) ReutChSeqCfg.Data
          );
      }
    }
  }

  if (mrcSuccess != Status) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*** Alias Detected!  See REUT Error above. ***\n");
    Outputs->SpdSecurityStatus = MrcSpdStatusAliased;
    Status = mrcSuccess;
  }

  return Status;
}

/**
@brief
  This function runs the srcubbing test reporting any timeouts/errors.

  @param[in] MrcData - The global host structure
  @param[in] ChBitMask - Bitmask of channels the test is run on.

  @retval mrcSuccess or error value.
**/
MrcStatus
MrcRunMemoryScrub (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   ChBitMask
  )
{
  const MrcDebug  *Debug;
  MrcStatus       Status;
  U8              ErrorStatus;
  U8              TestDoneStatus;
  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT       ReutGlobalCtl;
  MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_STRUCT       ReutGlobalErr;
  U32  Timer;

  Status = mrcSuccess;
  Debug  = &MrcData->SysIn.Inputs.Debug;

  //
  // Setup Timer and run the test
  //
  Timer = (U32) MrcGetCpuTime() + 10000; // 10 Second timeout
  ReutGlobalCtl.Data                                = 0;
  ReutGlobalCtl.Bits.Global_Start_Test              = 1;
  ReutGlobalCtl.Bits.Global_Clear_Errors            = 1;
  ReutGlobalCtl.Bits.Global_Stop_Test_On_Any_Error  = NSOE;
  MrcWriteCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, ReutGlobalCtl.Data);

  //
  // Wait until Channel test done status matches ChbitMask or TimeoutCounter value reaches 0;
  //
  do {
    ReutGlobalErr.Data = MrcReadCR (MrcData, MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_REG);
    TestDoneStatus     = (U8) ((ReutGlobalErr.Bits.Channel_Test_Done_Status_1 << 1) |
                                ReutGlobalErr.Bits.Channel_Test_Done_Status_0);
  } while (((TestDoneStatus & ChBitMask) != ChBitMask) && ((U32) MrcGetCpuTime () < Timer));

  if ((TestDoneStatus & ChBitMask) != ChBitMask) {
    Status = mrcDeviceBusy;
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_ERROR,
      "Timeout occured while running the test: ReutGlobalErr: 0x%X.\n",
      ReutGlobalErr.Data
      );
  }

  ErrorStatus = (U8) ((ReutGlobalErr.Bits.Channel_Error_Status_1 << 1) | ReutGlobalErr.Bits.Channel_Error_Status_0);
  if (ErrorStatus & ChBitMask) {
    Status = mrcReutSequenceError;
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_ERROR,
      "REUT Error: Channel(s):%s%s\n",
      (ReutGlobalErr.Bits.Channel_Error_Status_0 == 1) ? " 0" : "",
      (ReutGlobalErr.Bits.Channel_Error_Status_1 == 1) ? " 1" : ""
      );
  }

  return Status;
}

