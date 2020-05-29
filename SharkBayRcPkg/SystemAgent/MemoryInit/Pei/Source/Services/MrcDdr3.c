/** @file
  This file includes all the DDR3 specific characteristic functions.

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

*/

//
// Include files
//
#include <MrcDdr3.h>

#ifdef ULT_FLAG
///
/// Only 1DPC is supported on HSW-ULT
///
const TOdtValue MbUltOdtTable[MAX_DIMMS_IN_CHANNEL][2] = {
/// 1DPC 1R,  1DPC 2R
  {{120,0},  {120,0}},
};

const TOdtValue User1UltOdtTable[MAX_DIMMS_IN_CHANNEL][2] = {
/// 1DPC 1R,  1DPC 2R
  {{120,0},  {120,0}},
};

#endif //ULT_FLAG

#ifdef TRAD_FLAG
const TOdtValue MbTradOdtTable[MAX_DIMMS_IN_CHANNEL][oiNotValid] = {
/// 1DPC 1R,  1DPC 2R,  2DPC 1R 1R,  2DPC 1R 2R,  2DPC 2R 1R,  2DPC 2R 2R
  {{00,40},  {00,40},  {60,30},    {60,30},    {60,30},    {60,30}},
  {{00,40},  {00,40},  {60,30},    {60,30},    {60,30},    {60,30}}
};

const TOdtValue DtTradOdtTable[MAX_DIMMS_IN_CHANNEL][oiNotValid] = {
/// 1DPC 1R,  1DPC 2R,  2DPC 1R 1R,  2DPC 1R 2R,  2DPC 2R 1R,  2DPC 2R 2R
  {{60,00},  {60,00},  {60,30},    {60,30},    {60,30},    {60,30}},
  {{60,00},  {60,00},  {60,30},    {60,30},    {60,30},    {60,30}}
};

const TOdtValue User1TradOdtTable[MAX_DIMMS_IN_CHANNEL][oiNotValid] = {
/// 1DPC 1R,  1DPC 2R,  2DPC 1R 1R,  2DPC 1R 2R,  2DPC 2R 1R,  2DPC 2R 2R
  {{60,60},  {60,60},  {60,30},    {60,30},    {60,30},    {60,30}},
  {{60,60},  {60,60},  {60,30},    {60,30},    {60,30},    {60,30}}
};

const TOdtValue User2TradOdtTable[MAX_DIMMS_IN_CHANNEL][oiNotValid] = {
/// 1DPC 1R,  1DPC 2R,  2DPC 1R 1R,  2DPC 1R 2R,  2DPC 2R 1R,  2DPC 2R 2R
  {{60,60},  {60,60},  {60,40},    {60,40},    {60,40},    {60,40}},
  {{60,60},  {60,60},  {60,40},    {60,40},    {60,40},    {60,40}}
};
#endif // TRAD_FLAG

//
// Module external functions
//
/**
@brief
  this function reverses MA and BA bits for Rank1

  @param[in] BA              - MRS command to be sent
  @param[in] MA              - Value to be sent

  @retval Proper MA and BA BITS.
**/
U32
MrcMirror (
  IN U8              BA,
  IN U16             MA
  )
{
  U16 ma357;
  U16 ma468;

  //
  // UDIMM/SODIMM reverses the following bits on Rank1
  // A3 - 4, A5 - 6, A7 - 8
  // BA0 - 1
  //
  ma357 = 0xA8 & MA;
  ma468 = 0x150 & MA;
  MA    = (0xFE07 & MA) | (ma357 << 1) | (ma468 >> 1);
  BA    = (0x4 & BA) | ((0x2 & BA) >> 1) | ((0x1 & BA) << 1);

  return (BA << 24) + MA;
}

/**
@brief
  this function writes to CADB

  @param[in] MrcData         - include all the MRC data
  @param[in] Channel         - Channel to send command to
  @param[in] RankMask        - Rank mask for which command will be sent to.
  @param[in] CMD             - 0: MRS, 1: REF, 2: PRE, 3: ACT, 4: WR, 5: RD, 6: ZQ, 7: NOP
  @param[in] BA              - MRS command to be sent
  @param[in] MA              - Value to be sent
  @param[in] Delay           - Delay in Dclocks

  @retval MrcStatus
**/
MrcStatus
MrcWriteCADBCmd (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             RankMask,
  IN const U8             CMD,
  IN const U8             BA,
  IN const U16 *const     MA,
  IN const U8             Delay
  )
{
  const MrcInput                              *Inputs;
  MrcOutput                                   *Outputs;
  MrcChannelOut                               *ChannelOut;
  MrcStatus                                   Status;
  U32                                         Offset;
  U8                                          Stop;
  U8                                          Dimm;
  U8                                          Rank;
  U8                                          AddressMirrored; // bitMask
  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_STRUCT  ReutChPatCadbProg;
  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MRS_STRUCT   ReutChPatCadbMrs;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT   ReutChSeqCfg;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT   ReutChSeqCfgSave;
  MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_STRUCT   ReutChSeqCtl;
  MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_STRUCT     ReutGlobalErr;
  MrcDebug                                    *Debug;

  Debug           = &MrcData->SysIn.Inputs.Debug;
  Inputs          = &MrcData->SysIn.Inputs;
  Outputs         = &MrcData->SysOut.Outputs;
  ChannelOut      = &Outputs->Controller[0].Channel[Channel];
  Status          = mrcSuccess;
  Stop            = 0;
  AddressMirrored = 0;

  //
  // Clear DDR qualifier during reset sequence
  //
  MrcWriteCR8 (MrcData, MCSCHEDS_CR_DFT_MISC_REG + 1, 0);

  //
  // Check for AddressMirrored on each DIMM present
  //
  for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
    if (ChannelOut->Dimm[Dimm].AddressMirrored == TRUE) {
      AddressMirrored |= (MRC_BIT0 << Dimm);
    }
  }
  //
  // Pointer will be dynamically incremented after a write to CADB_PROG register
  //
  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG) * Channel);
  MrcWriteCR8 (MrcData, Offset, 0);

  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if (MrcRankInChannelExist (MrcData, Rank, Channel) & RankMask) {

      ReutChPatCadbProg.Data                   = 0;
      ReutChPatCadbProg.Bits.CADB_Data_Bank    = BA;
      ReutChPatCadbProg.Bits.CADB_Data_Address = MA[RANK_TO_DIMM_NUMBER (Rank)];

      //
      // Check if Rank 1 and if DIMM requires AddressMirrored
      //
      if ((Rank % 2) && (AddressMirrored & ((Rank / 2) + 1))) {
        //
        // Remainder is 1 only for Rank1 of each DIMM
        //
        ReutChPatCadbProg.Data = MrcMirror (BA, MA[RANK_TO_DIMM_NUMBER (Rank)]);
      }

      ReutChPatCadbProg.Bits.CADB_Data_CKE     = 0xF;
      ReutChPatCadbProg.Bits.CADB_Data_Control = CMD;
      ReutChPatCadbProg.Bits.CADB_Data_CS      = ~(MRC_BIT0 << Rank);

      Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_PROG_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_REG) * Channel);
      MrcWriteCR64 (MrcData, Offset, ReutChPatCadbProg.Data);

      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "CH%d Rank %d ReutChPatCadbProg: 0x%08X%08X\n", Channel, Rank, ReutChPatCadbProg.Data32[1], ReutChPatCadbProg.Data32[0]);

      Stop += 1;
    }
  }

  if (Stop == 0) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_ERROR,
      "MrcWriteCADBCmd: Channel %d Ranks %d ValidRankBitMask 0x%X\n",
      Channel,
      RankMask,
      ChannelOut->ValidRankBitMask
      );
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "MrcWriteCADBCmd: Not a valid Rank in RankBitMask\n");
    Status = mrcFail;
    return Status;
  }
  //
  // Execute MRS Mode
  //
  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MRS_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MRS_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MRS_REG) * Channel);
  ReutChPatCadbMrs.Data                      = 0;
  ReutChPatCadbMrs.Bits.MRS_Gap              = (Delay == 0) ? 3 : Delay;
  ReutChPatCadbMrs.Bits.CADB_MRS_End_Pointer = Stop - 1;
  MrcWriteCR (MrcData, Offset, ReutChPatCadbMrs.Data);

  //
  // Save before MR command
  //
  Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
    ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel);
  ReutChSeqCfgSave.Data = MrcReadCR (MrcData, Offset);

  //
  // Prepare for MRS command
  //
  ReutChSeqCfg.Data                     = ReutChSeqCfgSave.Data;
  ReutChSeqCfg.Bits.Global_Control      = 0;
  ReutChSeqCfg.Bits.Initialization_Mode = MRS_Mode;
  MrcWriteCR (MrcData, Offset, (U32) ReutChSeqCfg.Data); // Set MRS Mode w/o Global control

  //
  // Start test and clear errors
  //
  Offset = MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_REG +
    ((MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_REG) * Channel);
  ReutChSeqCtl.Data                    = 0;
  ReutChSeqCtl.Bits.Local_Clear_Errors = 1;
  ReutChSeqCtl.Bits.Local_Start_Test   = 1;
  MrcWriteCR8 (MrcData, Offset, (U8) ReutChSeqCtl.Data);

  //
  // Wait for Channel_Test_Done_Status for the channel.
  //
  // @todo: Infinite loop possible, need timer.
  //
  do {
    ReutGlobalErr.Data = MrcReadCR (MrcData, MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_REG);
    if (1 == ((Channel == 0) ? ReutGlobalErr.Bits.Channel_Error_Status_0 : ReutGlobalErr.Bits.Channel_Error_Status_1)) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR IN MrcWriteCADBCmd: REUT_GLOBAL_ERR 0x%X\n", ReutGlobalErr.Data);
      return mrcFail;
    }
  } while (0 == ((Channel == 0) ? ReutGlobalErr.Bits.Channel_Test_Done_Status_0 : ReutGlobalErr.Bits.Channel_Test_Done_Status_1));

  //
  // Restore after MR command
  //
  Offset = MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_REG +
    ((MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_REG) * Channel);
  ReutChSeqCtl.Data                    = 0;
  ReutChSeqCtl.Bits.Local_Clear_Errors = 1;
  MrcWriteCR8 (MrcData, Offset, (U8) ReutChSeqCtl.Data);

  Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
    ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel);
  MrcWriteCR (MrcData, Offset, (U32) ReutChSeqCfgSave.Data);
  return Status;
}

/**
@brief
  This function sends the proper MRS command for specific ranks as indicated by RankMask

  @param[in] MrcData         - include all the MRC data
  @param[in] Channel         - Channel to send command to
  @param[in] RankMask        - Rank mask for which command will be sent to
  @param[in] MR              - MRS command to be sent
  @param[in] DimmValue       - Dimm Values to be sent

  @retval MrcStatus
**/
MrcStatus
MrcWriteMRSAll (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             RankMask,
  IN const U8             MR,
  IN const U16 *const     DimmValue
  )
{
  //
  // CMD = [0: MRS, 1: REF, 2: PRE, 3: ACT, 4: WR, 5: RD, 6: ZQ, 7: NOP]
  //
  return MrcWriteCADBCmd (MrcData, Channel, RankMask, MRS_CMD, MR, DimmValue, 0);
}

/**
@brief
  This function sends the proper MRS command for specific ranks as indicated by RankMask

  @param[in] MrcData         - Include all the MRC data
  @param[in] Channel         - Channel to send command to
  @param[in] RankMask        - Rank mask for which command will be sent to
  @param[in] MR              - MRS command to be sent
  @param[in] Value           - Value to be sent

  @retval MrcStatus
**/
MrcStatus
MrcWriteMRS (
  IN MrcParameters  *const MrcData,
  IN const U8              Channel,
  IN const U8              RankMask,
  IN const U8              MR,
  IN const U16             Value
  )
{
  MrcStatus Status;
  U16       DimmValue[MAX_DIMMS_IN_CHANNEL];

  //
  // Update proper Dimm Values based on Ranks (rank bit mask)
  //
  if (RankMask <= 3) {
    //
    // For DIMM 0
    //
    DimmValue[0] = Value;
#if MAX_DIMMS_IN_CHANNEL > 1
    DimmValue[1] = 0;
#endif
  } else {
    //
    // DIMM 1
    //
    DimmValue[0] = 0;
#if MAX_DIMMS_IN_CHANNEL > 1
    DimmValue[1] = Value;
#endif
  }
  //
  // CMD = [0: MRS, 1: REF, 2: PRE, 3: ACT, 4: WR, 5: RD, 6: ZQ, 7: NOP]
  //
  Status = MrcWriteCADBCmd (MrcData, Channel, RankMask, MRS_CMD, MR, DimmValue, 0);

  return Status;
}

/**
@brief
  Issue ZQ calibration command on all ranks.
  When done, wait appropriate delay depending on the ZQ type.

  @param[in] MrcData         - include all the MRC data
  @param[in] chBitMask       - Channel bit mask to be sent to.
  @param[in] ZqType          - Type of ZQ Calibration: see MrcZqType enum

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
MrcStatus
MrcIssueZQ (
  IN MrcParameters *const MrcData,
  IN const U8             chBitMask,
  IN const MrcZqType      ZqType
  )
{
  MrcOutput *Outputs;
  MrcStatus Status;
  MrcDebug  *Debug;
  U8        Channel;
  U8        Dimm;
  U8        Delay;
  U16       MaValue;
  U16       DimmValue[MAX_DIMMS_IN_CHANNEL];
  U32       OpCode;
#ifdef MRC_DEBUG_PRINT
  char      *StrZqType;
#endif // MRC_DEBUG_PRINT
#ifdef ULT_FLAG
  U8        Rank;
  BOOL      Lpddr;
#endif // ULT_FLAG
  Debug   = &MrcData->SysIn.Inputs.Debug;
  Outputs = &MrcData->SysOut.Outputs;
  Status  = mrcSuccess;
  Delay   = 1;

#ifdef ULT_FLAG
  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);
#endif //ULT_FLAG
  MaValue = 0;

  switch (ZqType) {
    case MRC_ZQ_INIT:
#ifdef MRC_DEBUG_PRINT
      StrZqType = "INIT";
#endif
      MaValue = MRC_BIT10;
      OpCode  = 0xFF;
      break;

    case MRC_ZQ_LONG:
#ifdef MRC_DEBUG_PRINT
      StrZqType = "LONG";
#endif
      MaValue = MRC_BIT10;
      OpCode  = 0xAB;
      break;

    case MRC_ZQ_SHORT:
#ifdef MRC_DEBUG_PRINT
      StrZqType = "SHORT";
#endif
      OpCode  = 0x56;
      break;

    case MRC_ZQ_RESET:
#ifdef MRC_DEBUG_PRINT
      StrZqType = "RESET";
#endif
      OpCode  = 0xC3;
      break;

    default:
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Wrong ZQ type: %d\n", ZqType);
      return mrcWrongInputParameter;
  }

  //
  // Program MA value for all DIMMs
  //
  for (Dimm = 0; Dimm < (sizeof (DimmValue) / sizeof (DimmValue[0])); Dimm++) {
    DimmValue[Dimm] = MaValue;
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      if ((MRC_BIT0 << Channel) & chBitMask) {
        if (!(MRC_BIT0 << (Channel + 1) & chBitMask) && (ZqType == MRC_ZQ_SHORT)) {
          Delay = 7;
        }
        //
        // Issue ZQ calibration command on all ranks of this channel
        //
#ifdef ULT_FLAG
        if (Lpddr) {
          for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
            if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
              //
              // MR10, ZQ calibration
              //
              if (!Outputs->LpddrJedecInitDone) {
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "LPDDR: Issue ZQ %s on ch %d rank %d\n", StrZqType, Channel, Rank);
              }
              Status = MrcIssueMrw (MrcData, Channel, Rank, 10, OpCode, FALSE, FALSE);
            }
          }
        } else
#endif // ULT_FLAG
        {
          //
          //          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DDR3: Issue ZQ %s on ch %d\n", StrZqType, Channel);
          //
          Status = MrcWriteCADBCmd (MrcData, Channel, 0x0F, ZQ_CMD, 0, DimmValue, Delay);
        }
      }
    }
  }

  if ((ZqType == MRC_ZQ_INIT) || (ZqType == MRC_ZQ_LONG)) {
    MrcWait (MrcData, 1 * HPET_1US);
  }

  return Status;
}

/**
@brief
  This function writes the MR2 register for all the ranks and channels

  @param[in, out] MrcData    - general data
  @param[in]      Pasr       - Partial array self refresh bit A0-A2

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
MrcStatus
MrcSetMR2 (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Pasr
  )
{
  const MrcInput    *Inputs;
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  TOdtValue         *OdtTableIndex;
  MrcProfile        Profile;
  U8                Channel;
  U8                Rank;
  U8                RankMod2;
  U8                AutoSelfRefresh;
  U8                SelfRefreshTemp;
  U16               DimmValue[MAX_DIMMS_IN_CHANNEL];
  MrcStatus         Status;
  DDR3_MODE_REGISTER_2_STRUCT Ddr3ModeRegister;

  Inputs        = &MrcData->SysIn.Inputs;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Debug         = &Inputs->Debug;
  Status        = mrcSuccess;
  Profile       = Inputs->MemoryProfile;

  for (Channel = 0; (Channel < MAX_CHANNEL) && (Status == mrcSuccess); Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut                            = &ControllerOut->Channel[Channel];
      Ddr3ModeRegister.Data                 = 0;
      Ddr3ModeRegister.Bits.PartialArraySR  = Pasr;
      //
      // Subtract 5 because of jedec mr2 CWL table 0 = 5 1=6 2=7 ...
      //
      Ddr3ModeRegister.Bits.CasWriteLatency = ChannelOut->Timing[Profile].tCWL - 5;

      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank += MAX_RANK_IN_DIMM) {
        //
        // loop only for each DIMM
        //
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          if (Outputs->RestoreMRs) {
            RankMod2              = Rank % 2;
            Ddr3ModeRegister.Data = ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR2];
          } else {
            //
            // ASR: Set if both bits 0 and 2 of byte 31 in SPD are set.
            //
            // @todo: Need to check and see if we need to set DDR3_MODE_REGISTER_2_STR_OFF based on EXTENDED_TEMP support
            //           If ASR need BIT6 set, else if EXTENDED_TEMP set BIT7.
            //           Need to understand the policy here or should we follow NHM's approach for LFD/CFD.
            //
            if (
                (ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].AutoSelfRefresh == TRUE) &&
                ((Inputs->RefreshRate2x == FALSE) || (Outputs->AutoSelfRefresh == TRUE))
               ) {
              AutoSelfRefresh = 1;
              SelfRefreshTemp = 0;
            } else if (ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].SelfRefreshTemp == TRUE) {
              AutoSelfRefresh = 0;
              SelfRefreshTemp = 1;
            } else {
              AutoSelfRefresh = 0;
              SelfRefreshTemp = 0;
            }

            Ddr3ModeRegister.Bits.AutoSelfRefresh = AutoSelfRefresh;
            Ddr3ModeRegister.Bits.SelfRefreshTemp = SelfRefreshTemp;

            OdtTableIndex = GetOdtTableIndex (MrcData, Channel, RANK_TO_DIMM_NUMBER (Rank));
            if (OdtTableIndex == NULL) {
              return mrcFail;
            }

            Ddr3ModeRegister = UpdateRttWrValue (MrcData, OdtTableIndex->RttWr, Ddr3ModeRegister);

            //
            // *** must be before the MRC command because of address swizzling bits in SODIMM/UDIMM
            //
            SetTcMr2ShadowReg (MrcData, Channel, RANK_TO_DIMM_NUMBER (Rank), Ddr3ModeRegister.Data);

            //
            // save MR2 for later validation usage
            //
            RankMod2 = Rank % 2;
            ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR2]     = Ddr3ModeRegister.Data;
            ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2 + 1].MR[mrMR2] = Ddr3ModeRegister.Data;
            //
            // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,  "MrcSetMR2  Channel %u Dimm %u Rank %u = 0x%X\n",
            //                Channel, RANK_TO_DIMM_NUMBER (Rank), Rank, Ddr3ModeRegister.Data);
            //
          }
          //
          // Update proper DIMM value
          //
          DimmValue[RANK_TO_DIMM_NUMBER (Rank)] = Ddr3ModeRegister.Data;
        }
      }
      //
      // 3rd parameter is really a rank mask of which ranks to send command to.
      // If need it for all possible and present ranks, it should be 0xF.
      //
      Status = MrcWriteMRSAll (MrcData, Channel, 0x0F, mrMR2, DimmValue);
    }
  }

  return Status;
}

/**
@brief
  This function writes the MR3 register for all the ranks and channels

  @param[in] MrcData         - include all the MRC data
  @param[in] MPRLoc          - MPR Location bit A0-A1
  @param[in] Mpr             - MPR bit A2

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
MrcStatus
MrcSetMR3 (
  IN MrcParameters *const MrcData,
  IN const U8             MPRLoc,
  IN const U8             Mpr
  )
{
  const MrcInput              *Inputs;
  const MrcDebug              *Debug;
  MrcOutput                   *Outputs;
  U16                         DimmValue[MAX_DIMMS_IN_CHANNEL];
  U8                          Channel;
  U8                          Dimm;
  MrcStatus                   Status;
  DDR3_MODE_REGISTER_3_STRUCT Ddr3ModeRegister;

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;
  Status  = mrcSuccess;

  //
  // Independent channel data
  //
  Ddr3ModeRegister.Data              = 0;
  Ddr3ModeRegister.Bits.MprLocation  = MPRLoc;
  Ddr3ModeRegister.Bits.MprOperation = Mpr;

  for (Channel = 0; (Channel < MAX_CHANNEL) && (Status == mrcSuccess); Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      for (Dimm = 0; Dimm < (sizeof (DimmValue) / sizeof (DimmValue[0])); Dimm++) {
        DimmValue[Dimm] = Ddr3ModeRegister.Data;
      }
      //
      // 3rd parameter is really a rank mask of which ranks to send command to.
      // If need it for all possible and present ranks, it should be 0xF.
      //
      Status = MrcWriteMRSAll (MrcData, Channel, 0x0F, mrMR3, DimmValue);
    }
  }

  return Status;
}

/**
@brief
  This function writes the MR1 register for all the ranks and channels

  @param[in, out] MrcData         - include all the MRC data
  @param[in]      DLLEnable       - DLL enable bit A0
  @param[in]      Odic            - Output driver impedance control A5, A1
  @param[in]      AdditiveLatency - Additive latency bit A3-A4
  @param[in]      WlEnable        - Write leveling enable bit A7
  @param[in]      Tdqs            - TDQS enable bit A11
  @param[in]      Qoff            - Qoff bit A12

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
MrcStatus
MrcSetMR1 (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             DLLEnable,
  IN     const U8             Odic,
  IN     const U8             AdditiveLatency,
  IN     const U8             WlEnable,
  IN     const U8             Tdqs,
  IN     const U8             Qoff
  )
{
  const MrcInput              *Inputs;
  const MrcDebug              *Debug;
  MrcOutput                   *Outputs;
  MrcControllerOut            *ControllerOut;
  MrcChannelOut               *ChannelOut;
  TOdtValue                   *OdtTableIndex;
  U16                         DimmValue[MAX_DIMMS_IN_CHANNEL];
  U8                          Channel;
  U8                          Rank;
  U8                          RankMod2;
  U8                          RttNom;
  MrcStatus                   Status;
  DDR3_MODE_REGISTER_1_STRUCT Ddr3ModeRegister;

  Inputs        = &MrcData->SysIn.Inputs;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Debug         = &Inputs->Debug;
  Status        = mrcSuccess;
  OdtTableIndex = NULL;

  for (Channel = 0; (Channel < MAX_CHANNEL) && (Status == mrcSuccess); Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {

      ChannelOut = &ControllerOut->Channel[Channel];
      //
      // independent channel data
      //
      Ddr3ModeRegister.Data                 = 0;
      Ddr3ModeRegister.Bits.DllEnable       = DLLEnable;
      Ddr3ModeRegister.Bits.ODImpedanceLow  = Odic & 1;
      Ddr3ModeRegister.Bits.ODImpedanceHigh = (Odic & 2) >> 1;
      Ddr3ModeRegister.Bits.AdditiveLatency = AdditiveLatency;
      Ddr3ModeRegister.Bits.WriteLeveling   = WlEnable;
      Ddr3ModeRegister.Bits.Tdqs            = Tdqs; // @todo: We used to set Tdqs if the DIMM is X8.
      Ddr3ModeRegister.Bits.Qoff            = Qoff;

      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank += 2) {
        //
        // loop only for each DIMM
        //
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          if (Outputs->RestoreMRs) {
            RankMod2              = Rank % 2;
            Ddr3ModeRegister.Data = ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR1];
          } else {
            //
            // Get the ODT table index.
            //
            OdtTableIndex = GetOdtTableIndex (MrcData, Channel, RANK_TO_DIMM_NUMBER (Rank));
            if (OdtTableIndex == NULL) {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: Un support board type\n");
              return mrcFail;
            }
            //
            // Set the RttNom value
            //
#ifdef ULT_FLAG
            if (Inputs->CpuModel == cmHSW_ULT && Outputs->DdrType == MRC_DDR_TYPE_DDR3) {
              RttNom = 0;   // ODT disabled on DDR3 ULT
            } else
#endif // ULT_FLAG
            {
              RttNom = OdtTableIndex->RttNom;
            }

            Ddr3ModeRegister = UpdateRttNomValue (MrcData, RttNom, Ddr3ModeRegister);

            //
            // save MR1 for later validation usage
            //
            RankMod2 = Rank % 2;
            ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR1]     = Ddr3ModeRegister.Data;
            ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2 + 1].MR[mrMR1] = Ddr3ModeRegister.Data;
            //
            // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MrcSetMR1  Channel %u Dimm %u Rank %u = 0x%X\n",
            //                Channel, RANK_TO_DIMM_NUMBER (Rank), Rank, Ddr3ModeRegister.Data);
            //
          }
          //
          // Update proper DIMM value
          //
          DimmValue[RANK_TO_DIMM_NUMBER (Rank)] = Ddr3ModeRegister.Data;
        }
      }
      //
      // 3rd parameter is really a rank mask of which ranks to send command to.
      // If need it for all possible and present ranks, it should be 0xF.
      //
      Status = MrcWriteMRSAll (MrcData, Channel, 0x0F, mrMR1, DimmValue);
    }
  }

  return Status;
}

/**
@brief
  This function writes the MR0 register for all the ranks

  @param[in, out] MrcData         - include all the MRC data
  @param[in]      CommandControl  - include the command control params
  @param[in]      BurstLength     - Burst length bit A0-A1
  @param[in]      ReadBurstType   - Read burst type bit A3
  @param[in]      TestMode        - Test mode type bit A7
  @param[in]      DllReset        - DLL reset bit A8

  @retval MrcStatus - mrcSuccess if passes, otherwise an error status
**/
MrcStatus
MrcSetMR0 (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             BurstLength,
  IN     const U8             ReadBurstType,
  IN     const U8             TestMode,
  IN     const U8             DllReset
  )
{
  /*
  CAS Latency
  A6 A5 A4 A2 CAS Latency
  0  0  0  0  Reserved
  0  0  1  0  5
  0  1  0  0  6
  0  1  1  0  7
  1  0  0  0  8
  1  0  1  0  9
  1  1  0  0  10
  1  1  1  0  11(Optional for DDR3-1600)
  CAS = (CAS - 4) <<1

  Write recovery
  A11 A10 A9 WR(cycles)
  0   0   0  16*2
  0   0   1   5*2
  0   1   0   6*2
  0   1   1   7*2
  1   0   0   8*2
  1   0   1   10*2
  1   1   0   12*2
  1   1   1   14*2
  Wr = Wr - 5
*/
  const U8                    WrTable[] = {1, 2, 3, 4, 0, 5, 0, 6, 0, 7, 0, 0};
  const MrcInput              *Inputs;
  const MrcDebug              *Debug;
  MrcOutput                   *Outputs;
  MrcControllerOut            *ControllerOut;
  MrcChannelOut               *ChannelOut;
  MrcProfile                  Profile;
  U16                         DimmValue[MAX_DIMMS_IN_CHANNEL];
  U16                         Cas;
  U16                         Wr;
  U16                         Offset;
  U16                         Cl_A2;
  U8                          Channel;
  U8                          Rank;
  U8                          RankMod2;
  MrcStatus                   Status;
  DDR3_MODE_REGISTER_0_STRUCT Ddr3ModeRegister;

  Inputs        = &MrcData->SysIn.Inputs;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Debug         = &Inputs->Debug;
  Status        = mrcSuccess;
  Profile       = Inputs->MemoryProfile;

  //
  // independent channel data
  //
  Ddr3ModeRegister.Data               = 0;
  Ddr3ModeRegister.Bits.BurstLength   = BurstLength;
  Ddr3ModeRegister.Bits.ReadBurstType = ReadBurstType;
  Ddr3ModeRegister.Bits.TestMode      = TestMode;
  Ddr3ModeRegister.Bits.DllReset      = DllReset;

  for (Channel = 0; (Channel < MAX_CHANNEL) && (Status == mrcSuccess); Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {

      ChannelOut  = &ControllerOut->Channel[Channel];

      Cas         = ChannelOut->Timing[Profile].tCL;
      Wr          = ChannelOut->Timing[Profile].tWR;

      //
      // find the new cas value from the CAS table
      //
      if (Cas < 5 || Cas > 16) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: CAS value %d is not valid \n", Cas);
        Status = mrcCasError;
      }

      if ((Wr < 5) || (Wr > 8 && Wr != 10 && Wr != 12 && Wr != 14 && Wr != 16)) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: Write recovery Wr value %d is not valid \n", Wr);
        return mrcWrError;
      }
      //
      // convert CAS to jedec ddr3 values
      //
      if (Cas <= 11) {
        Offset  = 4;
        Cl_A2   = 0;
      } else {
        Offset  = 12;
        Cl_A2   = 1;
      }

      Ddr3ModeRegister.Bits.CasLatencyLow   = Cl_A2;
      Ddr3ModeRegister.Bits.CasLatencyHigh  = Cas - Offset;

      //
      // convert Wr to jedec ddr3 values
      //
      Ddr3ModeRegister.Bits.WriteRecovery = WrTable[Wr - 5];

      //
      // calculate the Ppd
      // DLL control for PPD: slow (0) for mobile, fast (1) for desktop, open for external input
      // Note - PM_PDWN_CONFIG_C# should be aligned with this. For slow exit use DLL_off. Otherwise use all others.
      //
      Ddr3ModeRegister.Bits.PrechargePdDll =
        (
          Inputs->PowerDownMode == pdmNoPowerDown ||
          Inputs->PowerDownMode == pdmAPD
        ) ? 1 : 0;
      if ((Inputs->PowerDownMode != pdmNoPowerDown && Inputs->PowerDownMode != pdmAPD) &&
          (Inputs->PowerDownMode != pdmPPDDLLOFF)
          ) {
        Ddr3ModeRegister.Bits.PrechargePdDll = 0;
      }
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank += 2) {
        //
        // loop only for each DIMM
        //
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          if (Outputs->RestoreMRs) {
            RankMod2              = Rank % 2;
            Ddr3ModeRegister.Data = ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR0];
          } else {
            //
            // save MR0 for later validation usage
            //
            RankMod2  = Rank % 2;
            ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR0]     = Ddr3ModeRegister.Data;
            ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2 + 1].MR[mrMR0] = Ddr3ModeRegister.Data;
            //
            // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,  "MrcSetMR0  Channel %u Dimm %u Rank %u = 0x%X\n",
            //               Channel, RANK_TO_DIMM_NUMBER (Rank), Rank, Ddr3ModeRegister.Data);
            //
          }
          //
          // Update proper DIMM value
          //
          DimmValue[RANK_TO_DIMM_NUMBER (Rank)] = Ddr3ModeRegister.Data;
        }
      }
      //
      // 3rd parameter is really a rank mask of which ranks to send command to.
      // If need it for all possible and present ranks, it should be 0xF.
      //
      Status = MrcWriteMRSAll (MrcData, Channel, 0x0F, mrMR0, DimmValue);
    }
  }

  return Status;
}

/**
@brief
  This function return tWLO time. this time is Write leveling output delay.

  @param[in] Frequency       - MC frequency.

  @retval tWLO timein nCK.
**/
U32
GetTwloTime (
  IN const MrcFrequency  Frequency
  )
{
  U32 tWLO;

  switch (Frequency) {
    case f2133:
    case f1867:
      tWLO = 8;
      break;

    case f1600:
    case f1333:
      tWLO = 6;
      break;

    case f1067:
      tWLO = 5;
      break;

    case f800:
      tWLO = 4;
      break;

    default:
      tWLO = 0;
  }

  return tWLO;
}

/**
@brief
  This funtion returns the odt table index for the given Dimm/Channel.

  @param[in] MrcData         - Include all the mrc global data.
  @param[in] Channel         - Channel to work on.
  @param[in] Dimm            - Rank to work on.

  @retval OdtValue - iThe pointer to the relevant Odt values.
**/
TOdtValue *
GetOdtTableIndex (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             Dimm
  )
{
  MrcDebug      *Debug;
  MrcChannelOut *ChannelOut;
  MrcDimmOut    *DimmOut;
  TOdtIndex     OdtIndex;

  Debug       = &MrcData->SysIn.Inputs.Debug;
  ChannelOut  = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  DimmOut     = &ChannelOut->Dimm[dDIMM0];
  OdtIndex    = oiNotValid;

  switch (ChannelOut->DimmCount) {
#ifdef TRAD_FLAG
    case 2:
      //
      // Two dimms per channel.
      //
      if ((DimmOut[dDIMM0].RankInDIMM == 1) && (DimmOut[dDIMM1].RankInDIMM == 1)) {
        OdtIndex = oi2DPC1R1R;
      } else if ((DimmOut[dDIMM0].RankInDIMM == 1) && (DimmOut[dDIMM1].RankInDIMM == 2)) {
        OdtIndex = oi2DPC1R2R;
      } else if ((DimmOut[dDIMM0].RankInDIMM == 2) && (DimmOut[dDIMM1].RankInDIMM == 1)) {
        OdtIndex = oi2DPC2R1R;
      } else if ((DimmOut[dDIMM0].RankInDIMM == 2) && (DimmOut[dDIMM1].RankInDIMM == 2)) {
        OdtIndex = oi2DPC2R2R;
      } else {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: Invalid 2DPC rank mode\n");
      }
      break;
#endif // TRAD_FLAG

    case 1:
      //
      // One dimm per channel.
      //
      if ((DimmOut[dDIMM0].RankInDIMM == 1) ||
          ((DimmOut[dDIMM1].RankInDIMM == 1) && (MAX_DIMMS_IN_CHANNEL > 1))
         ) {
          OdtIndex = oi1DPC1R;
      } else if ((DimmOut[dDIMM0].RankInDIMM == 2) ||
                 ((DimmOut[dDIMM1].RankInDIMM == 2) && (MAX_DIMMS_IN_CHANNEL > 1))
                ) {
          OdtIndex = oi1DPC2R;
      } else {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: Invalid 1DPC rank mode\n");
      }
      break;

    default:
      break;
  }

  return (OdtIndex == oiNotValid) ? NULL : SelectTable (MrcData, Dimm, OdtIndex);
}

/**
@brief
  This funtion takes the MR1 register value and updates the odt value
 inside the register.

  @param[in] MrcData          - Include all the MRC general data.
  @param[in] OdtValue         - Selected odt index.
  @param[in] Ddr3ModeRegister - Register to update.

  @retval Ddr3ModeRegister  - Updated register
**/
DDR3_MODE_REGISTER_1_STRUCT
UpdateRttNomValue (
  IN MrcParameters *const        MrcData,
  IN const U8                    OdtValue,
  IN DDR3_MODE_REGISTER_1_STRUCT Ddr3ModeRegister
  )
{
  const MrcDebug  *Debug;
  U8              A2Value;
  U8              A6Value;
  U8              A9Value;

  Debug = &MrcData->SysIn.Inputs.Debug;
  if (OdtValue == 0) {
    //
    // rtt_nom is disable
    //
    A2Value = 0;
    A6Value = 0;
    A9Value = 0;
  } else if (OdtValue == 60) {
    //
    // RZQ/4
    //
    A2Value = 1;
    A6Value = 0;
    A9Value = 0;
  } else if (OdtValue == 120) {
    //
    // RZQ/2
    //
    A2Value = 0;
    A6Value = 1;
    A9Value = 0;
  } else if (OdtValue == 40) {
    //
    // RZQ/6
    //
    A2Value = 1;
    A6Value = 1;
    A9Value = 0;
  } else if (OdtValue == 20) {
    //
    // RZQ/12
    //
    A2Value = 0;
    A6Value = 0;
    A9Value = 1;
  } else if (OdtValue == 30) {
    //
    // RZQ/8
    //
    A2Value = 1;
    A6Value = 0;
    A9Value = 1;
  } else {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: unsupported odt RttNom value\n");
    A2Value = 1;
    A6Value = 1;
    A9Value = 1;
  }

  Ddr3ModeRegister.Bits.OdtRttValueLow  = A2Value;
  Ddr3ModeRegister.Bits.OdtRttValueMid  = A6Value;
  Ddr3ModeRegister.Bits.OdtRttValueHigh = A9Value;
  return Ddr3ModeRegister;
}

/**
@brief
  This function updates the Rtt value in the MR2 value passed in.

  @param[in] MrcData          - Include all the MRC general data.
  @param[in] OdtValue         - Selected odt index.
  @param[in] Ddr3ModeRegister - Register to update.

  @retval Ddr3ModeRegister - Updated MR2 register
**/
DDR3_MODE_REGISTER_2_STRUCT
UpdateRttWrValue (
  MrcParameters *const        MrcData,
  const U8                    OdtValue,
  DDR3_MODE_REGISTER_2_STRUCT Ddr3ModeRegister
  )
{
  U8  RttValue;

  if ((OdtValue > 120) || ((OdtValue % 60) != 0)) {
    MRC_DEBUG_MSG (
      &MrcData->SysIn.Inputs.Debug,
      MSG_LEVEL_ERROR,
      "ERROR: unsupported odt RttWr value of %u\n",
      OdtValue
      );
    RttValue = 0;
  } else {
    RttValue = OdtValue / 60;
  }

  Ddr3ModeRegister.Bits.DynamicOdt = RttValue;
  return Ddr3ModeRegister;
}

/**
@brief
  this funtion select the ODT table according OEM/USER decision.
  In the MRC have 4 table type Mb,Dt,User1,User2.
  User1,User2 use as internal usage.

  @param[in] MrcData         - Include all the MRC general data.
  @param[in] Dimm            - selected DIMM.
  @param[in] OdtIndex        - selected odt index.

  @retval TOdtValue * - Pointer to the relevant table.
                        The return value is NULL if the table could
                        not be found
**/
TOdtValue *
SelectTable (
  IN MrcParameters *const MrcData,
  IN const U8             Dimm,
  IN const TOdtIndex      OdtIndex
  )
{
  const MrcInput *Inputs;
  TOdtValue      *OdtTable;
  const MrcDebug          *Debug;

  Inputs = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  OdtTable = NULL;
  switch (Inputs->BoardType) {
  case btCRBMB:
  case btCRBEMB:
#ifdef ULT_FLAG
    if (Inputs->CpuModel == cmHSW_ULT) {
      if (OdtIndex >= 2) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Error: MbUltOdtTable array out of bounds!\n");
        return NULL;
      }
      OdtTable = (TOdtValue *) &MbUltOdtTable[Dimm][OdtIndex];
    }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
    if ((Inputs->CpuModel == cmHSW) || (Inputs->CpuModel == cmCRW)) {
      OdtTable = (TOdtValue *) &MbTradOdtTable[Dimm][OdtIndex];
    }
#endif //TRAD_FLAG
    break;

  case btCRBDT:
#ifdef TRAD_FLAG
    if ((Inputs->CpuModel == cmHSW) || (Inputs->CpuModel == cmCRW)) {
      OdtTable = (TOdtValue *) &DtTradOdtTable[Dimm][OdtIndex];
    }
#endif //TRAD_FLAG
    break;

  case btUser1:
#ifdef ULT_FLAG
    if (Inputs->CpuModel == cmHSW_ULT) {
      if (OdtIndex >= 2) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Error: User1UltOdtTable array out of bounds!\n");
        return NULL;
      }
      OdtTable = (TOdtValue *) &User1UltOdtTable[Dimm][OdtIndex];
    }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
    if ((Inputs->CpuModel == cmHSW) || (Inputs->CpuModel == cmCRW)) {
      OdtTable = (TOdtValue *) &User1TradOdtTable[Dimm][OdtIndex];
    }
#endif //TRAD_FLAG
    break;

  case btUser2:
#ifdef TRAD_FLAG
    if ((Inputs->CpuModel == cmHSW) || (Inputs->CpuModel == cmCRW)) {
      OdtTable = (TOdtValue *) &User2TradOdtTable[Dimm][OdtIndex];
    }
#endif //TRAD_FLAG
    break;

  case btUser4:
    //
    // @todo: Need to Port ODT table for Ult
    //
#ifdef ULT_FLAG
    if (Inputs->CpuModel == cmHSW_ULT) {
      if (OdtIndex >= 2) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Error: MbUltOdtTable array out of bounds!\n");
        return NULL;
      }
      OdtTable = (TOdtValue *) &MbUltOdtTable[Dimm][OdtIndex];
    }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
    if ((Inputs->CpuModel == cmHSW) || (Inputs->CpuModel == cmCRW)) {
      OdtTable = (TOdtValue *) &MbTradOdtTable[Dimm][OdtIndex];
    }
#endif //TRAD_FLAG
    break;

  default:
    OdtTable = NULL;
    break;
  }

  return OdtTable;
}

#ifdef ULT_FLAG

/**
@brief
  Issue LPDDR MRW (Mode Register Write) command using MRH (Mode Register Handler).

  @param[in] MrcData - Include all MRC global data.
  @param[in] Channel - the channel to work on
  @param[in] Rank    - the rank to work on
  @param[in] Address - MRW address
  @param[in] Data    - MRW Data
  @param[in] InitMrw - when TRUE, command is stretched (used before CA training is done)
  @param[in] ChipSelect2N - when TRUE, use 2N stretch mode for CS (used before CA training is done)

  @retval mrcSuccess    - MRW was sent successfully
  @retval mrcDeviceBusy - timed out waiting for MRH
**/
MrcStatus
MrcIssueMrw (
  IN MrcParameters *const MrcData,
  IN U32                  Channel,
  IN U32                  Rank,
  IN U32                  Address,
  IN U32                  Data,
  IN BOOL                 InitMrw,
  IN BOOL                 ChipSelect2N
  )
{
  const MrcDebug                        *Debug;
  MrcOutput                             *Outputs;
  U32                                   OffsetMrCommand;
  U32                                   OffsetCmdRate;
  MCHBAR_CH0_CR_LPDDR_MR_COMMAND_STRUCT MrCommand;
  MCHBAR_CH0_CR_CMD_RATE_STRUCT         CmdRate;
  BOOL                                  Busy;
  U32                                   Timeout;

  Timeout = (U32) MrcGetCpuTime () + 10000;   // 10 seconds timeout

  Debug   = &MrcData->SysIn.Inputs.Debug;
  Outputs = &MrcData->SysOut.Outputs;

  OffsetMrCommand = MCHBAR_CH0_CR_LPDDR_MR_COMMAND_REG +
    ((MCHBAR_CH1_CR_LPDDR_MR_COMMAND_REG - MCHBAR_CH0_CR_LPDDR_MR_COMMAND_REG) * Channel);

  //
  // Make sure MRH is not busy
  //
  do {
    MrCommand.Data = MrcReadCR (MrcData, OffsetMrCommand);
    Busy           = (MrCommand.Bits.Busy == 1);
  } while (Busy && ((U32) MrcGetCpuTime () < Timeout));
  if (Busy) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Timed out waiting for previous MRH command to finish!\n");
    return mrcDeviceBusy;
  }

  OffsetCmdRate = MCHBAR_CH0_CR_CMD_RATE_REG + ((MCHBAR_CH1_CR_CMD_RATE_REG - MCHBAR_CH0_CR_CMD_RATE_REG) * Channel);
  if (ChipSelect2N) {
    //
    // Enable 2N stretch mode for CS
    //
    CmdRate.Data                = MrcReadCR(MrcData, OffsetCmdRate);
    CmdRate.Bits.init_mrw_2n_cs = 1;
    MrcWriteCR (MrcData, OffsetCmdRate, CmdRate.Data);
  }
  //
  // Send the MRW
  //
  MrCommand.Bits.Address  = Address;
  MrCommand.Bits.Data     = Data;
  MrCommand.Bits.Write    = 1;
  MrCommand.Bits.Init_MRW = InitMrw;
  MrCommand.Bits.Rank     = Rank;
  MrCommand.Bits.Busy     = 1;

  if (!Outputs->LpddrJedecInitDone) {
    MRC_DEBUG_MSG (
      Debug, MSG_LEVEL_NOTE,
      "MrcIssueMrw on ch %d rank %d: MR%d, Opcode=0x%02X, InitMrw=%d, 2N_CS=%d\n",
      Channel, Rank, Address, Data, InitMrw, ChipSelect2N
      );
  }
  MrcWriteCR (MrcData, OffsetMrCommand, MrCommand.Data);

  //
  // Wait till MRH is done sending the command
  //
  Timeout = (U32) MrcGetCpuTime () + 10000;   // 10 seconds timeout
  do {
    MrCommand.Data = MrcReadCR (MrcData, OffsetMrCommand);
    Busy           = (MrCommand.Bits.Busy == 1);
  } while (Busy && ((U32) MrcGetCpuTime () < Timeout));

  if (Busy) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Timed out sending MRH command!\n");
    return mrcDeviceBusy;
  }

  if (ChipSelect2N) {
    //
    // Disable 2N stretch mode for CS
    //
    CmdRate.Bits.init_mrw_2n_cs = 0;
    MrcWriteCR (MrcData, OffsetCmdRate, CmdRate.Data);
  }

  return mrcSuccess;
}

/**
@brief
  Issue LPDDR MRR (Mode Register Read) command using MRH (Mode Register Handler).
  Use DQ mapping array to deswizzle the MR data.

  @param[in]  MrcData - Include all MRC global data.
  @param[in]  Channel - the channel to work on
  @param[in]  Rank    - the rank to work on
  @param[in]  Address - MRR address
  @param[out] Data    - MRR Data array per SDRAM device

  @retval mrcSuccess    - MRR was executed successfully
  @retval mrcDeviceBusy - timed out waiting for MRH
**/
MrcStatus
MrcIssueMrr (
  IN MrcParameters *const MrcData,
  IN U32                  Channel,
  IN U32                  Rank,
  IN U32                  Address,
  OUT U8                  Data[4]
  )
{
  MrcInput          *Inputs;
  MrcDebug          *Debug;
  MrcOutput         *Outputs;
  MrcControllerIn   *ControllerIn;
  MrcControllerOut  *ControllerOut;
  MrcChannelIn      *ChannelIn;
  MrcChannelOut     *ChannelOut;
  U32               OffsetMrCommand;
  U32               OffsetMrrResult;
  BOOL              Busy;
  U32               CurrCpuBit;
  U32               CurrCpuByte;
  U32               CpuByteCnt;
  U32               DeviceCnt;
  U32               CurrDramBit;
  U32               BitVal;
  MCHBAR_CH0_CR_LPDDR_MR_RESULT_STRUCT  MrResult;
  MCHBAR_CH0_CR_LPDDR_MR_COMMAND_STRUCT MrCommand;
  U32               Timeout;
  Timeout = (U32) MrcGetCpuTime () + 10000;   // 10 seconds timeout

  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerIn  = &Inputs->Controller[0];
  ControllerOut = &Outputs->Controller[0];
  ChannelOut    = &ControllerOut->Channel[Channel];
  ChannelIn     = &ControllerIn->Channel[Channel];
  CurrCpuByte   = 0;
  MrcOemMemorySet (Data, 0, 4 * sizeof (Data[0]));

  OffsetMrCommand = MCHBAR_CH0_CR_LPDDR_MR_COMMAND_REG +
    ((MCHBAR_CH1_CR_LPDDR_MR_COMMAND_REG - MCHBAR_CH0_CR_LPDDR_MR_COMMAND_REG) * Channel);

  OffsetMrrResult= MCHBAR_CH0_CR_LPDDR_MR_RESULT_REG +
    ((MCHBAR_CH1_CR_LPDDR_MR_RESULT_REG - MCHBAR_CH0_CR_LPDDR_MR_RESULT_REG) * Channel);

  //
  // Make sure MRH is not busy
  //
  do {
    MrCommand.Data = MrcReadCR (MrcData, OffsetMrCommand);
    Busy           = (MrCommand.Bits.Busy == 1);
  } while (Busy && ((U32) MrcGetCpuTime () < Timeout));
  if (Busy) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Timed out waiting for previous MRH command to finish!\n");
    return mrcDeviceBusy;
  }

  //
  // Send the MRR
  //
  MrCommand.Bits.Address  = Address;
  MrCommand.Bits.Data     = 0; // Reading from DRAM
  MrCommand.Bits.Write    = 0; // MRR
  MrCommand.Bits.Init_MRW = 0; // MRR doesn't support Init_MRW
  MrCommand.Bits.Rank     = Rank;
  MrCommand.Bits.Busy     = 1;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MrcIssueMrr on ch %d rank %d: MR%d\n", Channel, Rank, Address);
  MrcWriteCR (MrcData, OffsetMrCommand, MrCommand.Data);

  //
  // Wait till MRH is done sending the command and getting the result
  //
  Timeout = (U32) MrcGetCpuTime () + 10000;   // 10 seconds timeout
  do {
    MrCommand.Data = MrcReadCR (MrcData, OffsetMrCommand);
    Busy           = (MrCommand.Bits.Busy == 1);
  } while (Busy && ((U32) MrcGetCpuTime () < Timeout));

  if (Busy) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Timed out sending MRH command!\n");
    return mrcDeviceBusy;
  }

  MrResult.Data = MrcReadCR (MrcData, OffsetMrrResult);

  for (DeviceCnt = 0; DeviceCnt < 4; DeviceCnt++) {
    if ((ChannelOut->Dimm[dDIMM0].SdramWidth == 32) && (1 == (DeviceCnt & 1))) {
      //
      // We only know DQ mapping for the lower 16 bits of the x32 devices
      // So we'll copy their MRR feedback to the upper bytes' place
      // Hence, we skip the odd dies for x32
      //
      Data[DeviceCnt] = Data[DeviceCnt - 1];
      continue;
    }

    //
    // Find which CPU byte is mapped to the relevant DRAM byte
    //
    for (CpuByteCnt = 0; CpuByteCnt < Outputs->SdramCount; CpuByteCnt++) {
      if ((DeviceCnt * 2) == ChannelIn->DqsMapCpu2Dram[CpuByteCnt]) {
        CurrCpuByte = CpuByteCnt;
        break;
      }
    }

    for (CurrCpuBit = 0; CurrCpuBit < MAX_BITS; CurrCpuBit++) {
      //
      // The actual DRAM bit that is connected to the current CPU DQ pin
      //
      CurrDramBit = ChannelIn->DqMapCpu2Dram[CurrCpuByte][CurrCpuBit] - 8 * (DeviceCnt * 2); // Subtract 8xDramByte

      BitVal = (MrResult.Data8[DeviceCnt] >> CurrCpuBit) & 1; // The 0/1 value that came from the DRAM bit
      Data[DeviceCnt] |= (BitVal << CurrDramBit);             // Putting the value in the correct place
    }
  }  // for DeviceCnt

  return mrcSuccess;
}

/**
@brief
  Issue LPDDR PRECHARGE ALL command using CADB.

  @param[in]  MrcData   - Include all MRC global data.
  @param[in]  Channel   - The channel to work on
  @param[in]  RankMask  - The rank(s) to work on

  @retval none
**/
void
MrcIssuePrechargeAll (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             RankMask
  )
{
  U32 CaHigh;
  U32 CaLow;
  U32 CMD;
  U32 BA;
  U32 MA;

  CaHigh = 0x1B;
  CaLow  = 0;

  MrcConvertLpddr2Ddr (CaHigh, CaLow, &MA, &BA, &CMD);

//  MRC_DEBUG_MSG (&MrcData->SysIn.Inputs.Debug, MSG_LEVEL_ERROR, "MA: 0x%X, BA: 0x%X, CMD: 0x%X\n", MA, BA, CMD);

  MrcWriteCADBCmd (MrcData, Channel, RankMask, (U8) CMD, (U8) BA, (U16 *) &MA, 0);
}

#endif // ULT_FLAG
