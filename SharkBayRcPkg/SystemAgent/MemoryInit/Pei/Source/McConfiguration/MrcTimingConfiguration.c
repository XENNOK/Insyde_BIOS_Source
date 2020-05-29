/** @file

  This module configures the memory controller timing parameters.
  
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

#include <MrcTimingConfiguration.h>

/**
@brief
  This function returns the tCKE value for the specified frequency.

  @param[in] DdrType    - DDR type
  @param[in] Frequency  - The memory frequency.

  @retval The tCKE value for the specified frequency.
**/
static
U32
tCKEValue (
  IN MrcDdrType         DdrType,
  IN const MrcFrequency Frequency
  )
{
  U32 tCKE;

#if (SUPPORT_LPDDR3 == SUPPORT)
  if (DdrType == MRC_DDR_TYPE_LPDDR3) {
    if (Frequency <= f1067) {
      tCKE = tCKE_LPDDR_1067;
    } else if (Frequency <= f1333) {
      tCKE = tCKE_LPDDR_1333;
    } else {
      tCKE = tCKE_LPDDR_1600;
    }
    return tCKE;
  }
#endif // SUPPORT_LPDDR3

  if (Frequency <= f800) {
    tCKE = TCKE_800;
  } else if (Frequency <= f1067) {
    tCKE = TCKE_1067;
  } else if (Frequency <= f1333) {
    tCKE = TCKE_1333;
  } else if (Frequency <= f1600) {
    tCKE = TCKE_1600;
  } else if (Frequency <= f1867) {
    tCKE = TCKE_1867;
  } else if (Frequency <= f2133) {
    tCKE = TCKE_2133;
  } else if (Frequency <= f2400) {
    tCKE = TCKE_2400;
  } else {
    tCKE = TCKE_2667;
  }

  return tCKE;
}

/**
@brief
  This function returns the tXPDLL value for the specified frequency.

  @param[in] Frequency - The memory frequency.

  @retval The tXPDLL value for the specified frequency.
**/
static
U32
tXPDLLValue (
  IN const MrcFrequency Frequency
  )
{
  U32 tXPDLL;

  if (Frequency <= f800) {
    tXPDLL = TXPDLL_800;
  } else if (Frequency <= f1067) {
    tXPDLL = TXPDLL_1067;
  } else if (Frequency <= f1333) {
    tXPDLL = TXPDLL_1333;
  } else if (Frequency <= f1600) {
    tXPDLL = TXPDLL_1600;
  } else if (Frequency <= f1867) {
    tXPDLL = TXPDLL_1867;
  } else if (Frequency <= f2133) {
    tXPDLL = TXPDLL_2133;
  } else if (Frequency <= f2400) {
    tXPDLL = TXPDLL_2400;
  } else {
    tXPDLL = TXPDLL_2667;
  }

  return tXPDLL;
}

/**
@brief
  This function returns the tXP value for the specified frequency.

  @param[in] DdrType   - DDR type
  @param[in] Frequency - The memory frequency.
  @param[in] NMode     - Command mode to lookup.

  @retval The tXP value for the specified frequency.
**/
U32
tXPValue (
  IN MrcDdrType         DdrType,
  IN const MrcFrequency Frequency,
  IN U8                 NMode
  )
{
  U32 tXP;

#if (SUPPORT_LPDDR3 == SUPPORT)
  if (DdrType == MRC_DDR_TYPE_LPDDR3) {
    if (Frequency <= f1333) {
      tXP = tXP_LPDDR_1333;
    } else {
      tXP = tXP_LPDDR_1600;
    }
  } else
#endif // SUPPORT_LPDDR3
  {
    if (Frequency <= f1600) {
      tXP = ((MC_tXP_1600_1N - 1) + NMode);
    } else if (Frequency <= f1867) {
      tXP = ((NMode <= 2) ? MC_tXP_1867_2N : MC_tXP_1867_3N);
    } else if (Frequency <= f2133) {
      tXP = (MC_tXP_2133_1N);
    } else {
      tXP = MC_tXP_MAX;
    }
  }
  return (tXP);
}

/**
@brief
  This function returns the tAONPD value for the specified frequency.

  @param[in] Frequency - The memory frequency.

  @retval The tAONPD value for the specified frequency.
**/
static
U32
tAONPDValue (
  IN const MrcFrequency Frequency
  )
{
  U32 tAONPD;

  if (Frequency <= f800) {
    tAONPD = TAONPD_800;
  } else if (Frequency <= f1067) {
    tAONPD = TAONPD_1067;
  } else if (Frequency <= f1333) {
    tAONPD = TAONPD_1333;
  } else if (Frequency <= f1600) {
    tAONPD = TAONPD_1600;
  } else if (Frequency <= f1867) {
    tAONPD = TAONPD_1867;
  } else if (Frequency <= f2133) {
    tAONPD = TAONPD_2133;
  } else if (Frequency <= f2400) {
    tAONPD = TAONPD_2400;
  } else {
    tAONPD = TAONPD_2667;
  }

  return tAONPD;
}

/**
@brief
  This function sets up the TC-BANK register,
  which includes the tRCD, tRP, tRAS, tRDPRE, tRTP, tWRPRE, and tRRD values.

  @param[in, out] MrcData - Include all MRC global data.
  @param[in]      Channel - Channel select.

  @retval Nothing.
**/
static
void
SetTcBankReg (
  IN OUT MrcParameters *const MrcData,
  IN     const U32            Channel
  )
{
  const MrcInput                *Inputs;
  const MrcDebug                *Debug;
  MrcOutput                     *Outputs;
  MrcChannelOut                 *ChannelOut;
  MrcTiming                     *Timing;
  MCHBAR_CH0_CR_TC_BANK_STRUCT  CrTcBank;
  U32                           tWRPRE;

  Inputs      = &MrcData->SysIn.Inputs;
  Debug       = &Inputs->Debug;
  Outputs     = &MrcData->SysOut.Outputs;
  ChannelOut  = &Outputs->Controller[0].Channel[Channel];
  Timing      = &ChannelOut->Timing[Inputs->MemoryProfile];

  //
  // tWRPRE is = 4tCK + tWR + tWL = 4DCLK + tWR + tWL
  //
  tWRPRE                = 4 + Timing->tCWL + Timing->tWR;

  CrTcBank.Data         = 0;
  CrTcBank.Bits.tRCD    = MIN (MCHBAR_CH0_CR_TC_BANK_tRCD_MAX, Timing->tRCD);
  CrTcBank.Bits.tRP     = MIN (MCHBAR_CH0_CR_TC_BANK_tRP_MAX, Timing->tRP);
  CrTcBank.Bits.tRAS    = MIN (MCHBAR_CH0_CR_TC_BANK_tRAS_MAX, Timing->tRAS);
  CrTcBank.Bits.tRDPRE  = MIN (MCHBAR_CH0_CR_TC_BANK_tRDPRE_MAX, Timing->tRTP);
  CrTcBank.Bits.tWRPRE  = MIN (MCHBAR_CH0_CR_TC_BANK_tWRPRE_MAX, tWRPRE);
  CrTcBank.Bits.tRRD    = MIN (MCHBAR_CH0_CR_TC_BANK_tRRD_MAX, Timing->tRRD);
#ifdef ULT_FLAG
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    CrTcBank.Bits.tRPab_ext = MIN (MCHBAR_CH0_CR_TC_BANK_tRPab_ext_MAX, Timing->tRPab - Timing->tRP);
  }
#endif
  MrcWriteCR (
    MrcData,
    MCHBAR_CH0_CR_TC_BANK_REG + ((MCHBAR_CH1_CR_TC_BANK_REG - MCHBAR_CH0_CR_TC_BANK_REG) * Channel),
    CrTcBank.Data
    );

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %u:\n tRCD = %u\n", Channel, CrTcBank.Bits.tRCD);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRP = %u\n", CrTcBank.Bits.tRP);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRAS = %u\n", CrTcBank.Bits.tRAS);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRDPRE = %u\n", CrTcBank.Bits.tRDPRE);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tWRPRE = %u\n", CrTcBank.Bits.tWRPRE);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRRD = %u\n", CrTcBank.Bits.tRRD);
#ifdef ULT_FLAG
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRPab_ext = %u\n", CrTcBank.Bits.tRPab_ext);
  }
#endif
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " TC_BANK = %Xh\n", CrTcBank.Data);

  //
  // Save in MrcData structure
  //
  ChannelOut->MchbarBANK = CrTcBank.Data;

  return;
}

/**
@brief
  This function sets up the TC_BANK_RANK_D register,
  which includes the tCL and tWCL values.

  @param[in, out] MrcData - Include all MRC global data.
  @param[in]      Channel - Channel select.

  @retval Nothing.
**/
static
void
SetTcBankRankDReg (
  IN OUT MrcParameters *const MrcData,
  IN     const U32            Channel
  )
{
  const MrcInput                      *Inputs;
  const MrcDebug                      *Debug;
  MrcOutput                           *Outputs;
  MrcChannelOut                       *ChannelOut;
  MrcTiming                           *Timing;
  MCHBAR_CH0_CR_TC_BANK_RANK_D_STRUCT TcBankRankD;
  U32                                 OdtReadDelay;
#ifdef ULT_FLAG
  U32                                 OdtWriteDelay;
  U32                                 OdtWriteDuration;
  U32                                 DclkPs;
#endif // ULT_FLAG

  Inputs      = &MrcData->SysIn.Inputs;
  Outputs     = &MrcData->SysOut.Outputs;
  Debug       = &Inputs->Debug;
  ChannelOut  = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  Timing      = &ChannelOut->Timing[Inputs->MemoryProfile];

  //
  // @todo: Need to make sure this value, after power on, follows the restrictions in the XML description.
  //
  OdtReadDelay                      = Timing->tCL - Timing->tCWL;

  TcBankRankD.Data                = 0;
  TcBankRankD.Bits.tWCL           = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_D_tWCL_MAX, Timing->tCWL);
  TcBankRankD.Bits.tCL            = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_D_tCL_MAX, Timing->tCL);
  TcBankRankD.Bits.tCPDED         = MCHBAR_CH0_CR_TC_BANK_RANK_D_tCPDED_DEF;
  TcBankRankD.Bits.tPRPDEN        = MCHBAR_CH0_CR_TC_BANK_RANK_D_tPRPDEN_DEF;
  TcBankRankD.Bits.Odt_Read_Delay = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_D_Odt_Read_Delay_MAX, OdtReadDelay);

#ifdef ULT_FLAG
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    if (Inputs->LpddrDramOdt) {
      //
      // Only ODT[0] is used in ULT, need to use it for both ranks
      //
      TcBankRankD.UltBits.Odt_Always_Rank0 = 1;
    }

    //
    // Timing->tCWL has 1 extra clock because of tDQSS - subtract it here.
    //
    TcBankRankD.Bits.tWCL--;

    //
    // JEDEC Spec requires tCPDED should be 2 clocks for all LPDDR3 frequencies
    //
    TcBankRankD.Bits.tCPDED = 2;

    DclkPs = Outputs->Qclkps * 2;

    //
    // Odt_Write_Delay = WL - 1 - RU(tODTon(max))
    //
    OdtWriteDelay = Timing->tCWL - 1 - (tODT_ON_MAX + DclkPs - 1) / DclkPs;

    //
    // Odt_Write_Duration = 6 + RU(tODTon(max)-tODToff(min)) - 6 + 1
    //
    OdtWriteDuration = 1 + (tODT_ON_MAX - tODT_OFF_MIN + DclkPs - 1) / DclkPs;

    TcBankRankD.UltBits.Odt_Write_Delay    = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_D_Odt_Write_Delay_Ult_MAX, OdtWriteDelay);
    TcBankRankD.UltBits.Odt_Write_Duration = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_D_Odt_Write_Duration_Ult_MAX, OdtWriteDuration);
  }
#endif // ULT_FLAG

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %u:\n tCL = %u\n", Channel, TcBankRankD.Bits.tCL);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tWCL = %u\n", TcBankRankD.Bits.tWCL);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tCPDED = %u\n", TcBankRankD.Bits.tCPDED);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tPRPDEN = %u\n", TcBankRankD.Bits.tPRPDEN);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Odt_Read_Delay = %u\n", TcBankRankD.Bits.Odt_Read_Delay);
#ifdef ULT_FLAG
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Odt_Write_Delay = %u\n", TcBankRankD.UltBits.Odt_Write_Delay);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Odt_Write_Duration = %u\n", TcBankRankD.UltBits.Odt_Write_Duration);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Odt_Always_Rank0 = %u\n", TcBankRankD.UltBits.Odt_Always_Rank0);
  }
#endif // ULT_FLAG

  MrcWriteCR (
    MrcData,
    MCHBAR_CH0_CR_TC_BANK_RANK_D_REG + ((MCHBAR_CH1_CR_TC_BANK_RANK_D_REG - MCHBAR_CH0_CR_TC_BANK_RANK_D_REG) * Channel),
    TcBankRankD.Data
    );
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " TC_BANK_RANK_D = %Xh\n", TcBankRankD.Data);

  //
  // Save in MrcData structure
  //
  ChannelOut->MchbarBANKRANKD = TcBankRankD.Data;

  return;
}

/**
@brief
  This function sets up the TC_BANK_RANK_A register.
  which includes the tCKE, tFAW, tRDRD, tRDRD_dr, tRDRD_dd, tRDPDEN, and command rate mode.

  @param[in, out] MrcData - Include all MRC global data.
  @param[in]      Channel - Channel select.

  @retval Nothing.
**/
static
void
SetTcBankRankAReg (
  IN OUT MrcParameters *const MrcData,
  IN     const U32            Channel
  )
{
  const MrcInput                      *Inputs;
  const MrcDebug                      *Debug;
  MrcOutput                           *Outputs;
  MrcTiming                           *Timing;
  MrcDdrType                          DdrType;
  MCHBAR_CH0_CR_TC_BANK_RANK_A_STRUCT TcBankRankA;
  U32                                 CRValue;

  Inputs            = &MrcData->SysIn.Inputs;
  Debug             = &Inputs->Debug;
  Outputs           = &MrcData->SysOut.Outputs;
  Timing            = &Outputs->Controller[0].Channel[Channel].Timing[Inputs->MemoryProfile];
  DdrType           = Outputs->DdrType;
  TcBankRankA.Data  = 0;

  //
  // Get the tCKE value.
  //
  CRValue               = tCKEValue (DdrType, Outputs->Frequency);
  TcBankRankA.Bits.tCKE = MIN (MCHBAR_CH1_CR_TC_BANK_RANK_A_tCKE_MAX, CRValue);

  //
  // Get the command rate mode value.
  // Use 3N mode during training steps
  //
#ifdef ULT_FLAG
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    //
    // Need to use 1N mode for LPDDR
    //
    TcBankRankA.Bits.CMD_stretch = 0;
  } else
#endif // ULT_FLAG
  {
    TcBankRankA.Bits.CMD_stretch = MIN (MCHBAR_CH1_CR_TC_BANK_RANK_A_CMD_stretch_MAX, 3);
  }
  //
  // Program tFAW value
  //
  TcBankRankA.Bits.tFAW = MIN (MCHBAR_CH1_CR_TC_BANK_RANK_A_tFAW_MAX, Timing->tFAW);

  //
  // Calculate tRDRD
  //
  TcBankRankA.Bits.tRDRD = MIN (MCHBAR_CH1_CR_TC_BANK_RANK_A_tRDRD_MAX, TCCD_ALL_FREQ);

  //
  // Calculate tRDRD_dr = BL/2 + max(tRTR, ODT(R,R,DR)) + tRPRE
  //
  CRValue = 4 + 1 + TRPRE_ALL_FREQ;

#ifdef ULT_FLAG
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    //
    // Increase tRDRD_dr from 6 to 7 DCLKs on LPDDR3
    //
    CRValue++;
  }
#endif // ULT_FLAG

  TcBankRankA.Bits.tRDRD_dr = MIN (MCHBAR_CH1_CR_TC_BANK_RANK_A_tRDRD_dr_MAX, CRValue);

  //
  // Calculate tRDRD_dd = BL/2 + max(tRTR, ODT(R,R,DD)) + tRPRE
  //
  TcBankRankA.Bits.tRDRD_dd = MIN (MCHBAR_CH1_CR_TC_BANK_RANK_A_tRDRD_dd_MAX, CRValue);

  //
  // Calculate tRDPDEN = tCL + BL/2 +1
  //
  CRValue                   = Timing->tCL + 5;
  TcBankRankA.Bits.tRDPDEN  = MIN (MCHBAR_CH1_CR_TC_BANK_RANK_A_tRDPDEN_MAX, CRValue);

  //
  // Disable command tri state before training.
  //
  TcBankRankA.Bits.CMD_3st = 1;

  MrcWriteCR (
    MrcData,
    MCHBAR_CH0_CR_TC_BANK_RANK_A_REG + ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * Channel),
    TcBankRankA.Data
    );

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %u:\n tCKE = %u\n", Channel, TcBankRankA.Bits.tCKE);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " NMode = %u\n", TcBankRankA.Bits.CMD_stretch);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tFAW = %u\n", TcBankRankA.Bits.tFAW);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRDRD = %u\n", TcBankRankA.Bits.tRDRD);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRDRD_dr = %u\n", TcBankRankA.Bits.tRDRD_dr);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRDRD_dd = %u\n", TcBankRankA.Bits.tRDRD_dd);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRDPDEN = %u\n", TcBankRankA.Bits.tRDPDEN);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " CMD_3st = %u\n", TcBankRankA.Bits.CMD_3st);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " TC_BANK_RANK_A = %Xh\n", TcBankRankA.Data);

  //
  // Save in MrcData structure
  //
  Outputs->Controller[0].Channel[Channel].MchbarBANKRANKA = TcBankRankA.Data;

  return;
}

/**
@brief
  This function sets up the TC_BANK_RANK_B register, which includes
  Dec_WRD, tWRPDEN, tWRWR_dd, tWRWR_dr, tWRWR, tWRRD_dd, tWRRD_dr and tWRRD.

  @param[in, out] MrcData - Include all MRC global data.
  @param[in]      Channel - Channel select.

  @retval Nothing.
**/
static
void
SetTcBankRankBReg (
  IN OUT MrcParameters *const MrcData,
  IN     const U32            Channel
  )
{
  const MrcInput                        *Inputs;
  const MrcDebug                        *Debug;
  MrcOutput                             *Outputs;
  MrcTiming                             *Timing;
  MCHBAR_CH0_CR_TC_BANK_RANK_B_STRUCT   TcBankRankB;
  MCHBAR_CH0_CR_TC_BANK_RANK_D_STRUCT   TcBankRankD;
  MCHBAR_CH0_CR_SC_WR_ADD_DELAY_STRUCT  ScWrAddDelay;
  U32                                   CRValue;
  U32                                   tWRRD_dr;
  U32                                   tWRWR_dr;
  U32                                   Offset;

  Inputs            = &MrcData->SysIn.Inputs;
  Debug             = &Inputs->Debug;
  Outputs           = &MrcData->SysOut.Outputs;
  Timing            = &Outputs->Controller[0].Channel[Channel].Timing[Inputs->MemoryProfile];
  TcBankRankB.Data  = 0;

  //
  // Calculate tWRRD = tCCD + tCWL + tWTR + max(tWrCAS2RdCAS_sr,ODT(W,R,SR)).
  //
  CRValue                 = TCCD_ALL_FREQ + Timing->tCWL + Timing->tWTR + 2;
  TcBankRankB.Bits.tWRRD  = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRRD_MAX, CRValue);

  //
  // Calculate tWRRD_dr = tCWL-tCL + BL/2 + max(tWRDRDD,ODT(W,R,DR)) + tRPRE
  //
  tWRRD_dr = Timing->tCWL - Timing->tCL + 4 + 2 + TRPRE_ALL_FREQ;

#ifdef ULT_FLAG
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    //
    // tWRRD_dr is 8 for all LPDDR bins
    //
    tWRRD_dr = 8;
  }
#endif // ULT_FLAG

  TcBankRankB.Bits.tWRRD_dr = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRRD_dr_MAX, tWRRD_dr);

  //
  // Calculate tWRRD_dd = tCWL-tCL + BL/2 + max(tWRDRDD,ODT(W,R,DR)) + tRPRE
  //
  TcBankRankB.Bits.tWRRD_dd = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRRD_dd_MAX, tWRRD_dr);

  //
  // Calculate tWRWR
  //
  TcBankRankB.Bits.tWRWR = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRWR_MAX, TCCD_ALL_FREQ);

  //
  // Calculate tWRWR_dr = BL/2 + max(tWWDR,ODT(W,W,DR)) + tWPRE
  //
  tWRWR_dr                  = 4 + 2 + TWPRE_ALL_FREQ;
  TcBankRankB.Bits.tWRWR_dr = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRWR_dr_MAX, tWRWR_dr);

  //
  // Calculate tWRWR_dd = BL/2 + max(tWWDD,ODT(W,W,DR)) + tWPRE
  //
  TcBankRankB.Bits.tWRWR_dd = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRWR_dd_MAX, tWRWR_dr);

  //
  // Calculate tWRPDEN = tWR+tWL+BL/2
  //
  CRValue                   = Timing->tWR + Timing->tCWL + 4;
  TcBankRankB.Bits.tWRPDEN  = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_B_tWRPDEN_MAX, CRValue);

  //
  // Set Dec_WRD.
  // Can be set to 1 only if tWCL is 6 or more.
  //
  Offset = MCHBAR_CH0_CR_TC_BANK_RANK_D_REG +
    (MCHBAR_CH1_CR_TC_BANK_RANK_D_REG - MCHBAR_CH0_CR_TC_BANK_RANK_D_REG) * Channel;
  TcBankRankD.Data = MrcReadCR (MrcData, Offset);
  if (TcBankRankD.Bits.tWCL >= 6) {
    TcBankRankB.Bits.Dec_WRD = 1;
  } else {
    TcBankRankB.Bits.Dec_WRD = 0;
  }

  MrcWriteCR (
    MrcData,
    MCHBAR_CH0_CR_TC_BANK_RANK_B_REG + ((MCHBAR_CH1_CR_TC_BANK_RANK_B_REG - MCHBAR_CH0_CR_TC_BANK_RANK_B_REG) * Channel),
    TcBankRankB.Data
    );

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %u:\n tWRRD = %u\n", Channel, TcBankRankB.Bits.tWRRD);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tWRRD_dr = %u\n", TcBankRankB.Bits.tWRRD_dr);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tWRRD_dd = %u\n", TcBankRankB.Bits.tWRRD_dd);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tWRWR = %u\n", TcBankRankB.Bits.tWRWR);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tWRWR_dr = %u\n", TcBankRankB.Bits.tWRWR_dr);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tWRWR_dd = %u\n", TcBankRankB.Bits.tWRWR_dd);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tWRPDEN = %u\n", TcBankRankB.Bits.tWRPDEN);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Dec_WRD = %u\n", TcBankRankB.Bits.Dec_WRD);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " TC_BANK_RANK_B = %Xh\n", TcBankRankB.Data);

  //
  // Save in MrcData structure
  //
  Outputs->Controller[0].Channel[Channel].MchbarBANKRANKB = TcBankRankB.Data;

  //
  // Set sc_wr_add_delay accordingly = 1 + Dec_WRD
  //
  CRValue                 = TcBankRankB.Bits.Dec_WRD + 1;
  ScWrAddDelay.Data       = 0;
  ScWrAddDelay.Bits.D1R1  = MIN (MCHBAR_CH0_CR_SC_WR_ADD_DELAY_D1R1_MAX, CRValue);
  ScWrAddDelay.Bits.D1R0  = MIN (MCHBAR_CH0_CR_SC_WR_ADD_DELAY_D1R0_MAX, CRValue);
  ScWrAddDelay.Bits.D0R1  = MIN (MCHBAR_CH0_CR_SC_WR_ADD_DELAY_D0R1_MAX, CRValue);
  ScWrAddDelay.Bits.D0R0  = MIN (MCHBAR_CH0_CR_SC_WR_ADD_DELAY_D0R0_MAX, CRValue);
  MrcWriteCR8 (
    MrcData,
    MCHBAR_CH0_CR_SC_WR_ADD_DELAY_REG +
    ((MCHBAR_CH1_CR_SC_WR_ADD_DELAY_REG - MCHBAR_CH0_CR_SC_WR_ADD_DELAY_REG) * Channel),
    (U8) ScWrAddDelay.Data
    );
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " SC_WR_ADD_Delay = %Xh\n", ScWrAddDelay.Data);

  return;
}

/**
@brief
  This function sets up the TC_BANK_RANK_C register, which includes
  TAONPD, tXP, tXPDLL, tRDWR, tRDWR_dr, and tRDWR_dd.

  @param[in, out] MrcData - Include all MRC global data.
  @param[in]      Channel - Channel select.

  @retval Nothing.
**/
static
void
SetTcBankRankCReg (
  IN OUT MrcParameters *const MrcData,
  IN     const U32            Channel
  )
{
  const MrcInput                      *Inputs;
  const MrcDebug                      *Debug;
  MrcOutput                           *Outputs;
  MrcTiming                           *Timing;
  MrcDdrType                          DdrType;
  MCHBAR_CH0_CR_TC_BANK_RANK_C_STRUCT TcBankRankC;
  U32                                 Value;
  U32                                 DclkPs;

  Inputs            = &MrcData->SysIn.Inputs;
  Debug             = &Inputs->Debug;
  Outputs           = &MrcData->SysOut.Outputs;
  Timing            = &Outputs->Controller[0].Channel[Channel].Timing[Inputs->MemoryProfile];
  DdrType           = Outputs->DdrType;
  TcBankRankC.Data  = 0;

  Value                   = tXPDLLValue (Outputs->Frequency);
  TcBankRankC.Bits.tXPDLL = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_C_tXPDLL_MAX, Value);

  Value                   = tXPValue (DdrType, Outputs->Frequency, 3);
  TcBankRankC.Bits.tXP    = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_C_tXP_MAX, Value);

  Value                   = tAONPDValue (Outputs->Frequency);
  TcBankRankC.Bits.TAONPD = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_C_TAONPD_MAX, Value);

  //
  // Calculate tRDWR = tCL-tCWL+tCCD+tWPRE+max(tRWSR,ODT(R,W,SR))
  // tWPRE - Write Preamble
  //
  Value = Timing->tCL - Timing->tCWL + TCCD_ALL_FREQ + TWPRE_ALL_FREQ + 2;
  //
  // Add 1 for frequencies above 1333.
  //
  if (Outputs->Frequency > f1333) {
    Value++;
  }

  DclkPs = Outputs->Qclkps * 2;

#ifdef ULT_FLAG
  if (DdrType == MRC_DDR_TYPE_LPDDR3) {
    //
    // tRDWR = tCL - tCWL + tDQSCK_max + tCCD + tWPRE + ElectricalTurnaround
    //
    Value = Timing->tCL - Timing->tCWL + (tDQSCK_MAX + DclkPs - 1) / DclkPs +
      TCCD_ALL_FREQ + TWPRE_ALL_FREQ + 1;
  }
#endif // ULT_FLAG

  TcBankRankC.Bits.tRDWR = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_C_tRDWR_MAX, Value);

  //
  // Calculate tRDWR_dr = tCL-tCWL+tCCD+tWPRE+max(tRWSR,ODT(R,W,SR))
  //
  TcBankRankC.Bits.tRDWR_dr = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_C_tRDWR_dr_MAX, Value);

  //
  // Calculate tRDWR_dd = tCL-tCWL+tCCD+tWPRE+max(tRWSR,ODT(R,W,SR))
  //
  TcBankRankC.Bits.tRDWR_dd = MIN (MCHBAR_CH0_CR_TC_BANK_RANK_C_tRDWR_dd_MAX, Value);

  MrcWriteCR (
    MrcData,
    MCHBAR_CH0_CR_TC_BANK_RANK_C_REG + ((MCHBAR_CH1_CR_TC_BANK_RANK_C_REG - MCHBAR_CH0_CR_TC_BANK_RANK_C_REG) * Channel),
    TcBankRankC.Data
    );

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %u:\n tXPDLL = %u\n", Channel, TcBankRankC.Bits.tXPDLL);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tXP = %u\n", TcBankRankC.Bits.tXP);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tAONPD = %u\n", TcBankRankC.Bits.TAONPD);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRDWR = %u\n", TcBankRankC.Bits.tRDWR);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRDWR_dr = %u\n", TcBankRankC.Bits.tRDWR_dr);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " tRDWR_dd = %u\n", TcBankRankC.Bits.tRDWR_dd);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " TC_BANK_RANK_C = %Xh\n", TcBankRankC.Data);

  //
  // Save in MrcData structure
  //
  Outputs->Controller[0].Channel[Channel].MchbarBANKRANKC = TcBankRankC.Data;

  return;
}

/**
@brief
  This function configures the memory controller timings.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
void
MrcTimingConfiguration (
  IN MrcParameters *const MrcData
  )
{
  U8 Channel;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (&MrcData->SysOut.Outputs, Channel)) {
      //
      // setup TC-BANK register
      //
      SetTcBankReg (MrcData, Channel);

      //
      // setup TC-BANK_RANK_D register
      //
      SetTcBankRankDReg (MrcData, Channel);

      //
      // setup TC-BANK_RANK_A register
      //
      SetTcBankRankAReg (MrcData, Channel);

      //
      // setup TC-BANK_RANK_B register
      //
      SetTcBankRankBReg (MrcData, Channel);

      //
      // setup TC-BANK_RANK_C register
      //
      SetTcBankRankCReg (MrcData, Channel);
    }
  }

  //
  // Check RawCard Types and adjust Read ODT if needed
  //
  RdOdtStretch (MrcData);

  return;
}

/**
@brief
  This function sets up the Read ODTD values based on RawCard types and adjusts the tDRRD2RD, tDDRD2RD, tDRRD2WR and tDDRD2WR

  @param[in, out] MrcData - Include all MRC global data.

  @retval Nothing.
**/
void
RdOdtStretch (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcDebug        *Debug;
  const MrcInput        *Inputs;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  U8                    Channel;
  U8                    ChBitMask;
  U8                    RankMaskCh;
  S8                    OdtStretch;
#if SUPPORT_SODIMM == SUPPORT
  MrcDimmOut            *DimmOut;
  BOOL                  SoDimm;
  U8                    Value;
  U8                    Dimm;
  U8                    DimmRawCardType[MAX_DIMMS_IN_CHANNEL];
  MCHBAR_CH0_CR_TC_BANK_RANK_A_STRUCT     TcBankRankA;
  MCHBAR_CH0_CR_TC_BANK_RANK_C_STRUCT     TcBankRankC;
#endif //SUPPORT_SODIMM == SUPPORT

  Inputs          = &MrcData->SysIn.Inputs;
  Debug           = &Inputs->Debug;
  ControllerOut   = &MrcData->SysOut.Outputs.Controller[0];

  ChBitMask = 0;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut  = &ControllerOut->Channel[Channel];
    if (ChannelOut->ValidRankBitMask && (ChannelOut->DimmCount == 2)) {
      ChBitMask |= (MRC_BIT0 << Channel);
    }
  }

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!((MRC_BIT0 << Channel) & ChBitMask)) {
      //
      // Skip any channels that do not have 2 DIMMs populated
      //
      continue;
    }

    ChannelOut  = &ControllerOut->Channel[Channel];
    RankMaskCh  = ChannelOut->ValidRankBitMask;
    //
    // Start with the most aggressive setting
    //
    OdtStretch = 6;

#if SUPPORT_SODIMM == SUPPORT
    SoDimm = FALSE;
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
      DimmOut = &ChannelOut->Dimm[Dimm];
      if (((DimmOut->ModuleType == MRC_MODULE_TYPE_SODIMM) || (DimmOut->ModuleType == MRC_MODULE_72B_SO_UDIMM))
        && (SoDimm == FALSE)) {
        SoDimm = TRUE;
      }
      if (SoDimm) {
        DimmRawCardType[Dimm] = DimmOut->ReferenceRawCard;
      }
    }

    if (SoDimm) {
      if ((DimmRawCardType[0] == rcF || DimmRawCardType[1] == rcF)
          && (DimmRawCardType[0] != DimmRawCardType[1])) {
        OdtStretch = 7;
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"Rd Odt Stretch F\n");
      }
    }
#endif //SUPPORT_SODIMM == SUPPORT
    //
    // Program Rdodtd value
    //
    UpdateTAParamOffset (MrcData, Channel, 0, rdodtd, OdtStretch, 1, 1, RankMaskCh);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Selected RdOdtD Offset for channel %d is = %d\n", Channel, OdtStretch);

#if SUPPORT_SODIMM == SUPPORT
    if (OdtStretch > 6) {
      TcBankRankA.Data = ChannelOut->MchbarBANKRANKA;
      Value = (U8)(TcBankRankA.Bits.tRDRD_dr);
      Value += OdtStretch - 6;
      //
      // Program Different Rank RD 2 RD value
      //
      UpdateTAParamOffset (MrcData, Channel, 0, drrd2rd, Value, 1, 1, RankMaskCh);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Selected DR RD2RD Offset for channel %d is = %d\n", Channel, Value);

      Value = (U8)(TcBankRankA.Bits.tRDRD_dd);
      Value += OdtStretch - 6;
      //
      // Program Different DIMM RD 2 RD value
      //
      UpdateTAParamOffset (MrcData, Channel, 0, ddrd2rd, Value, 1, 1, RankMaskCh);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Selected DD RD2RD Offset for channel %d is = %d\n", Channel, Value);

      TcBankRankC.Data = ChannelOut->MchbarBANKRANKC;
      Value = (U8)(TcBankRankC.Bits.tRDWR_dr);
      Value += OdtStretch - 6;
      //
      // Program Different Rank RD 2 WR value
      //
      UpdateTAParamOffset (MrcData, Channel, 0, drrd2wr, Value, 1, 1, RankMaskCh);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Selected DR RD2WR Offset for channel %d is = %d\n", Channel, Value);

      Value = (U8)(TcBankRankC.Bits.tRDWR_dd);
      Value += OdtStretch - 6;
      //
      // Program Different DIMM RD 2 WR value
      //
      UpdateTAParamOffset (MrcData, Channel, 0, ddrd2wr, Value, 1, 1, RankMaskCh);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Selected DD RD2WR Offset for channel %d is = %d\n", Channel, Value);

      Value = (U8)(TcBankRankC.Bits.tRDWR);
      Value += OdtStretch - 6;
      //
      // Program Same Rank RD 2 WR value
      //
      UpdateTAParamOffset (MrcData, Channel, 0, srrd2wr, Value, 1, 1, RankMaskCh);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Selected SR RD2WR Offset for channel %d is = %d\n", Channel, Value);
    }
#endif //SUPPORT_SODIMM == SUPPORT
  }

}
