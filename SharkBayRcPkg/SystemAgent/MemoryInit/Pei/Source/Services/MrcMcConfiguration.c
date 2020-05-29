/** @file
  The functions in this file implement the memory controller registers that
  are not training specific. After these functions are executed, the
  memory controller will be ready to execute the timing training sequences.

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
#include <MrcMcConfiguration.h>

const U8 RxBiasTable[2][5][4] = {
  /// Vdd low
  ///    1067 Mhz,     1333 Mhz,     1600 MHz,     1867 MHz,     2133 Mhz
  { {4, 3, 3, 2}, {4, 4, 3, 2}, {5, 4, 3, 3}, {5, 4, 4, 3}, {5, 4, 4, 3} },
  /// Vdd hi
  ///    1067 Mhz,     1333 Mhz,     1600 MHz,     1867 MHz,     2133 Mhz
  { {4, 3, 3, 2}, {4, 4, 3, 2}, {5, 4, 3, 3}, {5, 4, 4, 3}, {4, 4, 3, 3} }
};

#ifdef ULT_FLAG
const U8 RxBiasTableUlt[2][3][4] = {
  /// Vdd low
  ///  1067 Mhz,     1333 Mhz,     1600 MHz
  { {5, 6, 6, 5}, {5, 6, 6, 5}, {4, 6, 6, 6} },
  /// Vdd hi
  ///  1067 Mhz,     1333 Mhz,     1600 MHz
  { {7, 6, 6, 5}, {7, 6, 6, 5}, {7, 6, 6, 6} }
};
#endif // ULT_FLAG

/**
@brief
  This function calculates the two numbers that get you closest to the slope.

  @param[in] Slope - targeted slope (multiplied by 100 for int match)

  @retval Returns the Slope Index to be programmed for VtSlope.
**/
U8
MrcCalcVtSlopeCode (
  IN const U16 Slope
  )
{
  const S16 Coding[] = {0, -125, -62, -31, 250, 125, 62, 31};
  S16       Error;
  S16       BestError;
  U8        BestI;
  U8        BestJ;
  U8        i;
  U8        j;

  BestError = 1000;
  BestI     = 0;
  BestJ     = 0;
  for (i = 0; i < (sizeof (Coding) / sizeof (Coding[0])); i++) {
    for (j = 0; j < (sizeof (Coding) / sizeof (Coding[0])); j++) {
      Error = Slope - (Coding[i] + Coding[j]);
      if (Error < 0) {
        Error = -Error;
      }

      if (BestError > Error) {
        BestError = Error;
        BestI     = i;
        BestJ     = j;
      }
    }
  }

  return (BestI << 3) + BestJ;
}

/**
@brief
  This function performs the memory controller configuration non training sequence.

  @param[in, out] MrcData - Include all MRC global data.

  @retval MrcStatus - mrcSuccess if successful or an error status
**/
MrcStatus
MrcMcConfiguration (
  IN OUT MrcParameters *const MrcData
  )
{
  const U8          StdCASLat[] = {7, 9, 11, 13, 14};             // 1067, 1333, 1600, 1867 & 2133 MHz
  const U8          ByteStagger[] = {0, 4, 1, 5, 2, 6, 3, 7, 8};
  const U8          StepSize[]    = {64, 96, 64, 64, 64};         // From Design
  U8                ReferenceR[]  = {25, 50, 20, 20, 25};         // Reference resistors on motherboard (+0 Ohm Rstray)
  const U8          MinCycleStageDelay[] = {46, 70, 70, 46};      // Avoid corner case
  const U8          TargetRcompConst[] = {33, 50, 20, 20, 29};    // Target values
  const U8          BufferStageDelayPSConst[] = {59, 53, 53, 53}; // Slew = 1V / (# Stages * StageDelayPS * Derating)
  const MrcDebug    *Debug;
  MrcStatus         Status;
  MrcInput          *Inputs;
  MrcOutput         *Outputs;
  MrcChannelIn      *ChannelIn;
  MrcChannelOut     *ChannelOut;
  MrcControllerOut  *ControllerOut;
  MrcDimmOut        *DimmOut;
  MrcCpuModel       CpuModel;
  MrcCpuStepping    CpuStepping;
  MrcProfile        Profile;
  MrcVddSelect      Vdd;
  BOOL              Cmd2N;
  BOOL              AutoSelfRefresh;
  U32               vrefup;
  U32               Offset;
  U32               Data32;
  U32               DisableOdtStatic;
  S16               CompVref;
  U16               VssHiSwingTarget;
  U16               vpanic;
  U16               SAFE;
  U16               NS;
  U16               VssHi;                                        // Target VssHi Voltage
  U16               Target;
  U16               Slope;
  U16               NumStages;
  U16               lndown;
  U16               Rdown;
  U16               vrefdown;
  U16               vsshiu;
  U16               vsshid;
  U16               lnup;
  U16               Rup;
  U16               RefiReduction;
  S8                RxFselect;
  U8                delta;
  U8                Controller;
  U8                Channel;
  U8                Dimm;
  U8                Rank;
  U8                Byte;
  U8                VddHi;
  U8                OverClock;
  U8                MinLatency;
  U8                Latency[MAX_CHANNEL];
  U8                ChannelLatency;
  U8                RxCBSelect;
  U8                RxB;
  U8                stagger;
  U8                Any2dpc;
  U8                TargetRcomp[sizeof (TargetRcompConst) / sizeof (TargetRcompConst[0])];
  U8                BufferStageDelayPS[sizeof (BufferStageDelayPSConst) / sizeof (BufferStageDelayPSConst[0])];
  U8                i;
  DDRDATA_CR_RXTRAINRANK0_STRUCT                RxTrainRank;
  DDRDATA_CR_TXTRAINRANK0_STRUCT                TxTrainRank;
  DDRDATA_CR_DDRCRDATACONTROL0_STRUCT           DdrCrDataControl0;
  DDRDATA_CR_DDRCRDATACONTROL1_STRUCT           DdrCrDataControl1;
  DDRDATA0CH0_CR_DDRCRDATACONTROL2_STRUCT       DdrCrDataControl2;
#ifdef ULT_FLAG
  BOOL                                          Lpddr;
  U32                                           Group;
  U32                                           Cke;
  U32                                           CkeRankMapping;
  DDRDATA_CR_DDRCRVSSHICONTROL_STRUCT           DdrCrVssHiControl;
  DDRDATA7CH1_CR_DDRCRVREFCONTROL_STRUCT        DdrCrVrefControl;
  PCU_CR_DDR_PTM_CTL_PCU_STRUCT                 DdrPtmCtl;
#endif // ULT_FLAG
#ifdef TRAD_FLAG
  DDRDATACH0_CR_DDRCRVSSHIORVREFCONTROL_STRUCT  DdrCrVssHiOrVrefControl;
#endif // TRAD_FLAG
  DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_STRUCT       DdrCrCompVssHiControl;
  DDRDATA_CR_DDRCRVREFADJUST1_STRUCT            DdrCrVrefAdjust;
  DDRCLKCH0_CR_DDRCRCLKCONTROLS_STRUCT          DdrCrClkControls;
  DDRCMDCH0_CR_DDRCRCMDCONTROLS_STRUCT          DdrCrCmdControls;
  DDRCKECH0_CR_DDRCRCTLCONTROLS_STRUCT          DdrCrCkeControls;
  DDRCTLCH0_CR_DDRCRCTLCONTROLS_STRUCT          DdrCrCtlControls;
  DDRCKECTLCH0_CR_DDRCRCTLPICODING_STRUCT       DdrCrCtlPiCoding;
  DDRCLKCH0_CR_DDRCRCLKRANKSUSED_STRUCT         DdrCrClkRanksUsed;
  DDRCTLCH0_CR_DDRCRCTLRANKSUSED_STRUCT         CtlDdrCrCtlRanksUsed;
  DDRCKECH0_CR_DDRCRCTLRANKSUSED_STRUCT         CkeDdrCrCtlRanksUsed;
  DDRCOMP_CR_DDRCRCOMPVSSHI_STRUCT              DdrCrCompVssHi;
  DDRSCRAM_CR_DDRMISCCONTROL0_STRUCT            DdrMiscControl;
  PCU_CR_M_COMP_PCU_STRUCT                      CrMCompPcu;
  DDRDATA_CR_RCOMPDATA1_STRUCT                  DataRCompData;
  DDRCMD_CR_DDRCRCMDCOMP_STRUCT                 CmdDdrCrCmdComp;
  DDRCKECTL_CR_DDRCRCTLCOMP_STRUCT              CkeCtlDdrCrCtlComp;
  DDRCLK_CR_DDRCRCLKCOMP_STRUCT                 ClkDdrCrClkComp;
  DDRCOMP_CR_DDRCRDATACOMP1_STRUCT              CompDdrCrDataComp;
  DDRCOMP_CR_DDRCRCMDCOMP_STRUCT                CompDdrCrCmdComp;
  DDRCOMP_CR_DDRCRCTLCOMP_STRUCT                CompDdrCrCtlComp;
  DDRCOMP_CR_DDRCRCLKCOMP_STRUCT                CompDdrCrClkComp;
  DDRCOMP_CR_DDRCRCOMPOVR_STRUCT                CompDdrCrCompOvr;
  DDRCOMP_CR_DDRCRCOMPCTL0_STRUCT               CompDdrCrCompCtl0;
  DDRCOMP_CR_DDRCRCOMPCTL1_STRUCT               CompDdrCrCompCtl1;

  Inputs            = &MrcData->SysIn.Inputs;
  Debug             = &Inputs->Debug;
  Outputs           = &MrcData->SysOut.Outputs;
  ControllerOut     = &Outputs->Controller[0];
  Profile           = Inputs->MemoryProfile;
  Status            = mrcSuccess;
  CpuModel          = Inputs->CpuModel;
  CpuStepping       = Inputs->CpuStepping;
  Vdd               = Outputs->VddVoltage[Inputs->MemoryProfile];
  VddHi             = 0;
  OverClock         = 0;
  MinLatency        = 24;
  SAFE              = 0;
  VssHiSwingTarget  = 950;  // VssHi target voltage in mV
  vpanic            = 24;   // Panic Treshold at 24 mV
  delta             = 15;   // VssHi change voltage during panic: 15mV
  RefiReduction     = 100;  // Init to 100% for no reduction.
  DisableOdtStatic  = DISABLE_ODT_STATIC;

  MrcOemMemorySet (Latency, 0, sizeof (Latency));
  MrcOemMemoryCpy (TargetRcomp, (U8 *) TargetRcompConst, sizeof (TargetRcomp) / sizeof (TargetRcomp[0]));
  MrcOemMemoryCpy (
    BufferStageDelayPS,
    (U8 *) BufferStageDelayPSConst,
    sizeof (BufferStageDelayPS) / sizeof (BufferStageDelayPS[0])
    );

  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerOut = &Outputs->Controller[Controller];
    ControllerOut->DeviceId   = (U16) (MrcOemPciRead32 (HOST_BRIDGE_BUS, HOST_BRIDGE_DEVICE, HOST_BRIDGE_FUNCTION, HOST_BRIDGE_DEVID) >> 16);
    ControllerOut->RevisionId = (U8)  (MrcOemPciRead32 (HOST_BRIDGE_BUS, HOST_BRIDGE_DEVICE, HOST_BRIDGE_FUNCTION, HOST_BRIDGE_REVID));
  }

#ifdef TRAD_FLAG
  if ((CpuModel == cmHSW) || (CpuModel == cmCRW)) {
    //
    // Make sure tCL-tCWL <= 4
    // This is needed to support ODT properly for 2DPC case
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        ChannelOut = &ControllerOut->Channel[Channel];
        if ((ChannelOut->Timing[Profile].tCL - ChannelOut->Timing[Profile].tCWL) > 4) {
          ChannelOut->Timing[Profile].tCWL = ChannelOut->Timing[Profile].tCL - 4;
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "(tCL-tCWL) > 4, CH %u - tCWL has been updated to %u\n",
            Channel,
            ChannelOut->Timing[Profile].tCWL
            );
        }
      }
    }
  }
#endif // TRAD_FLAG

  //
  // Set memory controller frequency
  //
  if (MrcOemCheckPoint (MrcData, OemFrequencySet, NULL) == mrcSuccess) {
    Status = McFrequencySet (MrcData);
    if (Status != mrcSuccess) {
      return Status;
    }
  }
#ifdef SSA_FLAG
  MrcOemCheckPoint (MrcData, OemFrequencySetDone, NULL);
#endif // SSA_FLAG
  Outputs->Qclkps = (U16) (Outputs->MemoryClock / (2 * 1000)); // QCLK period in pico seconds.

#ifdef ULT_FLAG
  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  if (CpuModel == cmHSW_ULT) {
    //
    // Select the interleaving mode of DQ/DQS pins
    // This must be the first DDR IO register to be programmed on ULT
    //
    DdrMiscControl.Data                   = 0;
    DdrMiscControl.Bits.DdrNoChInterleave = (Inputs->DqPinsInterleaved) ? 0 : 1;
    if (Lpddr) {
      DdrMiscControl.Bits.LPDDR_Mode = 1;

      //
      // Initialize the CKE-to-rank mapping for LPDDR
      //
      DdrMiscControl.Bits.CKEMappingCh0 = Inputs->CkeRankMapping & 0x0F;
      DdrMiscControl.Bits.CKEMappingCh1 = (Inputs->CkeRankMapping >> 4) & 0x0F;
    }
    MrcWriteCR (MrcData, DDRSCRAM_CR_DDRMISCCONTROL0_REG, DdrMiscControl.Data);
    DisableOdtStatic = 1;
  }
#endif // ULT_FLAG
  //
  // Save MRC Version into CR
  //
  MrcSetMrcVersion (MrcData);

  Any2dpc = 0;
  if (Vdd > VDD_1_35) {
    VddHi = 1;                                  // Set HiVdd bit if Vdd is over 1.35v
  }

  NS        = ~SAFE;

  //
  // RX BIAS calculations
  //
  GetRxFselect (MrcData, &RxFselect, &RxCBSelect);

#ifdef ULT_FLAG
  if (CpuModel == cmHSW_ULT) {
    RxFselect = MIN (RxFselect, RXF_SELECT_MAX_ULT);    // Maximum 1600 MHz
    RxB = RxBiasTableUlt[VddHi][RxFselect][RxCBSelect]; // Read setting from array lookup table
  } else
#endif // ULT_FLAG
  {
    RxB = RxBiasTable[VddHi][RxFselect][RxCBSelect]; // Read setting from array lookup table
  }

  //
  // Determine Overclocking
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut       = &ControllerOut->Channel[Channel];
      Latency[Channel] = (U8) ChannelOut->Timing[Profile].tCL;
      if (Latency[Channel] < MinLatency) {
        MinLatency = Latency[Channel];
      }
    }
  }

  if ((Outputs->Frequency > 2133) || (MinLatency < StdCASLat[RxFselect])) {
    OverClock = 1;
  }
  //
  // Initialize ValidChBitMask and ValidRankMask used during all training steps
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }
    ChannelOut = &ControllerOut->Channel[Channel];
    ChannelIn = &Inputs->Controller[0].Channel[Channel];

    if (ChannelOut->DimmCount == 2) {
      Any2dpc++;
    }

    Outputs->ValidChBitMask |= (1 << Channel);
    Outputs->ValidRankMask  |= ChannelOut->ValidRankBitMask;
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "C%uValidRankBitMask / ValidRankMask - 0x%x / 0x%x\n",
      Channel,
      ChannelOut->ValidRankBitMask,
      Outputs->ValidRankMask
      );

    //
    // Initialize RanksUsed in CLK fub
    //
    DdrCrClkRanksUsed.Data = 0;
    DdrCrClkRanksUsed.Bits.RankEn = ChannelOut->ValidRankBitMask;
#ifdef ULT_FLAG
    if (Lpddr) {
      //
      // On LPDDR the CLK RanksUsed goes by CLK group instead of by Rank
      //
      DdrCrClkRanksUsed.Bits.RankEn = 0;
      for (Group = 0; Group < 2; Group++) {
        if (ChannelIn->DQByteMap[MrcIterationClock][Group] != 0) {
          DdrCrClkRanksUsed.Bits.RankEn |= (1 << Group);
        }
      }
    }
#endif // ULT_FLAG
    Offset = DDRCLKCH0_CR_DDRCRCLKRANKSUSED_REG +
      ((DDRCLKCH1_CR_DDRCRCLKRANKSUSED_REG - DDRCLKCH0_CR_DDRCRCLKRANKSUSED_REG) * Channel);
    MrcWriteCR (MrcData, Offset, DdrCrClkRanksUsed.Data);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DDRCLKCH%d_CR_DDRCRCTLRANKSUSED = 0x%X\n", Channel, DdrCrClkRanksUsed.Data);

    //
    // Initialize RanksUsed in CTL fub - CS (and ODT for LPDDR)
    //
    CtlDdrCrCtlRanksUsed.Data = 0;
    CtlDdrCrCtlRanksUsed.Bits.RankEn = ChannelOut->ValidRankBitMask;
#ifdef ULT_FLAG
    if (CpuModel == cmHSW_ULT) {
      if (Lpddr && Inputs->LpddrDramOdt) {
        //
        // ODT is used on rank 0
        //
        CtlDdrCrCtlRanksUsed.Bits.OdtDisable = 2;
      } else {
        CtlDdrCrCtlRanksUsed.Bits.OdtDisable = 3;
      }
    }
#endif // ULT_FLAG
    Offset = DDRCTLCH0_CR_DDRCRCTLRANKSUSED_REG +
      ((DDRCTLCH1_CR_DDRCRCTLRANKSUSED_REG - DDRCTLCH0_CR_DDRCRCTLRANKSUSED_REG) * Channel);
    MrcWriteCR (MrcData, Offset, CtlDdrCrCtlRanksUsed.Data);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DDRCTLCH%d_CR_DDRCRCTLRANKSUSED = 0x%X\n", Channel, CtlDdrCrCtlRanksUsed.Data);

    //
    // Initialize RanksUsed in CKE fub
    //
    CkeDdrCrCtlRanksUsed.Data = 0;
    CkeDdrCrCtlRanksUsed.Bits.RankEn = ChannelOut->ValidRankBitMask;
#ifdef ULT_FLAG
    if (Lpddr) {
      CkeDdrCrCtlRanksUsed.Bits.RankEn = 0;
      //
      // Use CKE-to-Rank mapping: [3:0] - Channel 0, [7:4] - Channel 1
      //
      CkeRankMapping = (Inputs->CkeRankMapping >> (Channel * 4)) & 0x0F;
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        for (Cke = 0; Cke <= 3; Cke++) {
          if (((CkeRankMapping >> Cke) & 1) == Rank) {
            //
            // This CKE pin is connected to this Rank...
            //
            if (ChannelOut->ValidRankBitMask & (1 << Rank)) {
              //
              // ...and this rank is enabled
              //
              CkeDdrCrCtlRanksUsed.Bits.RankEn |= (1 << Cke);
            }
          }
        }
      }
    }
#endif // ULT_FLAG
    Offset = DDRCKECH0_CR_DDRCRCTLRANKSUSED_REG +
      ((DDRCKECH1_CR_DDRCRCTLRANKSUSED_REG - DDRCKECH0_CR_DDRCRCTLRANKSUSED_REG) * Channel);
    MrcWriteCR (MrcData, Offset, CkeDdrCrCtlRanksUsed.Data);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DDRCKECH%d_CR_DDRCRCTLRANKSUSED = 0x%X\n", Channel, CkeDdrCrCtlRanksUsed.Data);
    //
    // Save for future use in JEDEC Reset, etc.
    //
    ChannelOut->ValidCkeBitMask = (U8) CkeDdrCrCtlRanksUsed.Bits.RankEn;
  }  //  for Channel

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init Data CRs\n");


  //
  // Initialize Rx and Tx Data CRs
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if (((1 << Rank) & Outputs->ValidRankMask) == 0) {
      continue;
    }
    //
    // RxDqsN/P_Pi = 32, RcvEn = 64, RxEq = 1
    //
    RxTrainRank.Data           = 0;
    RxTrainRank.Bits.RxRcvEnPi = 64;
    RxTrainRank.Bits.RxDqsPPi  = 32;
    RxTrainRank.Bits.RxDqsNPi  = 32;
    RxTrainRank.Bits.RxEq      = 1;
    //
    // RxGroup - Broadcast all channels
    //
    Offset = DDRDATA_CR_RXTRAINRANK0_REG + ((DDRDATA_CR_RXTRAINRANK1_REG - DDRDATA_CR_RXTRAINRANK0_REG) * Rank);
    MrcWriteCrMulticast (MrcData, Offset, RxTrainRank.Data);

    //
    // Rx per bit offset - Middle value. Train later.
    //
    Offset = DDRDATA_CR_RXPERBITRANK0_REG + ((DDRDATA_CR_RXPERBITRANK1_REG - DDRDATA_CR_RXPERBITRANK0_REG) * Rank);
    Data32 = 0x88888888;
    MrcWriteCrMulticast (MrcData, Offset, Data32);

    //
    // Set TxEq to full strength, TxDqs = 0 and TxDq = 32,
    //
    TxTrainRank.Data                = 0;
    TxTrainRank.Bits.TxEqualization = TXEQFULLDRV | 0x0B;
    TxTrainRank.Bits.TxDqDelay      = 96;
    TxTrainRank.Bits.TxDqsDelay     = 64;
    Offset = DDRDATA_CR_TXTRAINRANK0_REG + ((DDRDATA_CR_TXTRAINRANK1_REG - DDRDATA_CR_TXTRAINRANK0_REG) * Rank);
    MrcWriteCrMulticast (MrcData, Offset, TxTrainRank.Data);
    //
    // Middle value. Train later.
    //
    Offset = DDRDATA_CR_TXPERBITRANK0_REG + ((DDRDATA_CR_TXPERBITRANK1_REG - DDRDATA_CR_TXPERBITRANK0_REG) * Rank);
    MrcWriteCrMulticast (MrcData, Offset, 0x88888888);

    //
    // Save in MrcData
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &ControllerOut->Channel[Channel];
      for (Byte = 0; Byte < MAX_SDRAM_IN_DIMM; Byte++) {
        ChannelOut->TxDq[Rank][Byte]    = (U16) (TxTrainRank.Bits.TxDqDelay);
        ChannelOut->TxDqs[Rank][Byte]   = (U16) (TxTrainRank.Bits.TxDqsDelay);
        ChannelOut->TxEq[Rank][Byte]    = (U8) (TxTrainRank.Bits.TxEqualization);

        ChannelOut->RcvEn[Rank][Byte]   = (U16) (RxTrainRank.Bits.RxRcvEnPi);
        ChannelOut->RxDqsP[Rank][Byte]  = (U8) (RxTrainRank.Bits.RxDqsPPi);
        ChannelOut->RxDqsN[Rank][Byte]  = (U8) (RxTrainRank.Bits.RxDqsNPi);
        ChannelOut->RxEq[Rank][Byte]    = (U8) (RxTrainRank.Bits.RxEq);
      }
    }
  }
  //
  // Initial value to corresponding 0.
  //
  MrcWriteCrMulticast (MrcData, DDRDATA_CR_TXXTALK_REG, 0x0);
  //
  // Amplifier voltage offset {0: Most negative offset,... 8: 0 offset, ... 15: Most postive offset}
  //
  MrcWriteCrMulticast (MrcData, DDRDATA_CR_RXOFFSETVDQ_REG, 0x88888888);
  MrcWriteCrMulticast (MrcData, DDRDATA_CR_DDRCRDATAOFFSETTRAIN_REG, 0x0);
  MrcWriteCrMulticast (MrcData, DDRDATA_CR_DDRCRDATAOFFSETCOMP_REG, 0x0);

  //
  // Disable ODT Static Leg, set Vdd
  //
  DdrCrDataControl0.Data                  = 0;
  DdrCrDataControl0.Bits.DataVccddqHi     = VddHi;
  DdrCrDataControl0.Bits.DisableOdtStatic = DisableOdtStatic;
#ifdef ULT_FLAG
  if (Lpddr) {
    DdrCrDataControl0.Bits.LPDDR_Mode       = 1;
    //
    // If C0 or greater, enable EarlyRleak.  Otherwise, no Read Conditioning.
    //
    if ((CpuModel == cmHSW_ULT) && (CpuStepping >= csHswUltC0)) {
      DdrCrDataControl0.Bits.EarlyRleakEn     = 1;  // Mutually exclusive with DdrCrDataControl0.EnReadPreamble
    }
    DdrCrDataControl0.Bits.OdtSampExtendEn  = 1;
  }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
  if ((CpuModel == cmHSW) || (CpuModel == cmCRW)) {
    DdrCrDataControl0.Bits.InternalClocksOn = 1;
  }
#endif // TRAD_FLAG
  MrcWriteCrMulticast (MrcData, DDRDATA_CR_DDRCRDATACONTROL0_REG, DdrCrDataControl0.Data);
  DdrCrDataControl1.Data = 0;
  if (Inputs->WeaklockEn) {
    DdrCrDataControl1.Bits.DllWeakLock = NS; // Enable DLL WeakLock
  }

  DdrCrDataControl1.Bits.DllMask          = 1;                                                 // 2 qclk DLL mask
  DdrCrDataControl1.Bits.OdtDelay         = 0xE;                                               // Signed value of (-2) has been converted to hex
  DdrCrDataControl1.Bits.SenseAmpDelay    = 0xE;                                               // Signed value of (-2) has been converted to hex
  DdrCrDataControl1.Bits.SenseAmpDuration = DDRDATA_CR_DDRCRDATACONTROL1_SenseAmpDuration_MAX; // Max Samp Duration.
  DdrCrDataControl1.Bits.OdtDuration      = DDRDATA_CR_DDRCRDATACONTROL1_OdtDuration_MAX;      // Max Odt Duration.
  DdrCrDataControl1.Bits.RxBiasCtl        = RxB;                                               // RxBias uses LUT.

#ifdef ULT_FLAG
#endif // ULT_FLAG

  MrcWriteCrMulticast (MrcData, DDRDATA_CR_DDRCRDATACONTROL1_REG, DdrCrDataControl1.Data);

  DdrCrDataControl2.Data = 0;       // Define DQControl2

#ifdef ULT_FLAG
  if (CpuModel == cmHSW_ULT) {
    DdrCrDataControl2.Bits.RxDqsAmpOffset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_RxDqsAmpOffset_DEF;
    DdrCrDataControl2.Bits.RxClkStgNum    = DDRDATA0CH0_CR_DDRCRDATACONTROL2_RxClkStgNum_MAX;
    if (Lpddr) {
      DdrCrDataControl2.Bits.LeakerComp = 3;
    }
  }
#endif // ULT_FLAG

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++){
    if (MrcChannelExist (Outputs, Channel)) {
      ChannelOut                   = &ControllerOut->Channel[Channel];

      ChannelOut->DqControl0.Data  = DdrCrDataControl0.Data;
      ChannelLatency               = 2 * Latency[Channel] - 6;
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        //
        // These CRs do a lot of RMW.
        //
        ChannelOut->DataOffsetTrain[Byte] = 0;    // Faster to store the value in host
        ChannelOut->DataCompOffset[Byte]  = 0;
        ChannelOut->DqControl1[Byte].Data = DdrCrDataControl1.Data;

        //
        // Stagger byte turnon to reduce dI/dT. In safe mode, turn off stagger feature
        //
        stagger = (SAFE) ? 0 : ((ChannelLatency * ByteStagger[Byte]) / (U8) Outputs->SdramCount) & 0x1F;
        Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
          ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
          ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
        DdrCrDataControl2.Bits.RxStaggerCtl = stagger;
        MrcWriteCR (MrcData, Offset, DdrCrDataControl2.Data);
        ChannelOut->DqControl2[Byte].Data = DdrCrDataControl2.Data;
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init Data VssHi CRs\n");
  //
  // Initialize VssHi CRs
  //
  // @todo: Need to verify as I don't have bit definitions for VssHi mode
  //
  VssHi = ((U16) Vdd - VssHiSwingTarget);   // VssHiSwingTarget = 950 mV, VddmV=1500 mV
  Target = (VssHi * 192) / (U16) Vdd - 20;  // Sets target for VssHi.

  DdrCrCompVssHiControl.Data = 0;
#ifdef ULT_FLAG
  if (CpuModel == cmHSW_ULT) {
    DdrCrVssHiControl.Data = (SAFE) ?                           // SAFE: Panic at 7*8=56mV, !SAFE: Panic at 24mV, GainBoost.
      (7 << 18) + (2 << 14) + (2 << 8) + (2 << 6) :             // Set BwError and *BWDivider to safe values
      (1 << 22) + (3 << 18) + (2 << 14) + (1 << 8) + (1 << 6);  // Normal values for BwError/*BWDivider
    DdrCrVssHiControl.Data += (1 << 16);                        // Enable Panic Driver
    DdrCrVssHiControl.Data += Target + (0 << 10);               // Set loop sample frequency at max
    MrcWriteCrMulticast (MrcData, DDRDATA_CR_DDRCRVSSHICONTROL_REG, DdrCrVssHiControl.Data); // Multicast to both channels
    //
    // Set COMP VssHi the same
    //
    DdrCrCompVssHiControl.Data = DdrCrVssHiControl.Data;
  }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
  if ((CpuModel == cmHSW) || (CpuModel == cmCRW)) {
    DdrCrVssHiOrVrefControl.Data = (SAFE) ?                     // SAFE: Panic at 7*8=56mV, !SAFE: Panic at 24mV, GainBoost.
      (7 << 18) + (2 << 14) + (2 << 8) + (2 << 6) :             // Set BwError and *BWDivider to safe values
      (1 << 22) + (3 << 18) + (2 << 14) + (1 << 8) + (1 << 6);  // Normal values for BwError/*BWDivider
    DdrCrVssHiOrVrefControl.Data += (1 << 16);                  // Enable Panic Driver
    DdrCrVssHiOrVrefControl.Data += Target + (0 << 10);         // Set loop sample frequency at max
    MrcWriteCrMulticast (MrcData, DDRDATACH0_CR_DDRCRVSSHIORVREFCONTROL_REG, DdrCrVssHiOrVrefControl.Data);

    //
    // Set COMP VssHi the same
    //
    DdrCrCompVssHiControl.Data = DdrCrVssHiOrVrefControl.Data;
  }
#endif // TRAD_FLAG

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init Comp VssHi CRs\n");
  MrcWriteCrMulticast (MrcData, DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_REG, DdrCrCompVssHiControl.Data);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init Dimm Vref CRs\n");
  //
  // Initialize Dimm Vref CRs - Use CH1 BYTE 7
  //
  // Ideal Slope EQN = (192/128*VccIo/Vdd-1)
  //
  Slope = (U16) ((1000 * 192 * Inputs->VccIomV) / (128 * (U16) Vdd) - 1000);
  Slope = MrcCalcVtSlopeCode (Slope);             // Translate ideal slope in CR value

#ifdef ULT_FLAG
  if (CpuModel == cmHSW_ULT) {
    //
    // No Offset.  Apply Slope adjustment VT Slope A = 4, VT Slope B = 0, Set SlowBWError = 1
    //
    DdrCrVrefControl.Data = (0 << 18) + (0x20 << 12) + (1 << 8);
    //
    // Enable HiBW mode, Set Loop Frequency
    //
    DdrCrVrefControl.Data += (1 << 10) + (1 << 4);
    //
    // Set LoBWDiv, HiBWDiv
    //
    DdrCrVrefControl.Data += (3 << 2) + (0 << 0);
    //
    // Program DimmVref Control Values
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DdrCrVrefControl: 0x%X\n", DdrCrVrefControl.Data);
    MrcWriteCR (MrcData, DDRDATA7CH1_CR_DDRCRVREFCONTROL_REG, DdrCrVrefControl.Data);
  }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
  if ((CpuModel == cmHSW) || (CpuModel == cmCRW)) {
    //
    // No Offset.  Apply Slope adjustment, Set SlowBWError = 1
    //
    DdrCrVssHiOrVrefControl.Data = (0 << 18) + (Slope << 12) + (1 << 8);
    //
    // Enable HiBW mode, Set Loop Frequency
    //
    DdrCrVssHiOrVrefControl.Data += (1 << 10) + (3 << 4);
    //
    // Set LoBWDiv, HiBWDiv
    //
    DdrCrVssHiOrVrefControl.Data += (3 << 2) + (3 << 0);
    //
    // Program DimmVref Control Values
    //
    MrcWriteCR (MrcData, DDRDATA7CH1_CR_DDRCRVSSHIORVREFCONTROL_REG, DdrCrVssHiOrVrefControl.Data);
  }
#endif // TRAD_FLAG

  //
  // Enable all DimmVref and VddHi based on VddVoltage
  //
  DdrCrVrefAdjust.Data               = 0;
  DdrCrVrefAdjust.Bits.EnDimmVrefCA  = 1;
  DdrCrVrefAdjust.Bits.EnDimmVrefCh0 = 1;
  DdrCrVrefAdjust.Bits.EnDimmVrefCh1 = 1;
  DdrCrVrefAdjust.Bits.VccddqHiQnnnH = VddHi;
  DdrCrVrefAdjust.Bits.HiZTimerCtrl  = DDRDATA_CR_DDRCRVREFADJUST1_HiZTimerCtrl_MAX;
  //
  // Enable DimmVref Drivers with Vref = 0
  //
  MrcWriteCrMulticast (MrcData, DDRDATA_CR_DDRCRVREFADJUST1_REG, DdrCrVrefAdjust.Data);
  Outputs->DimmVref = DdrCrVrefAdjust.Data;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {

      ChannelOut = &ControllerOut->Channel[Channel];

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init CLK CRs\n");
      //
      // Initialize Clock CRs
      //
      DdrCrClkControls.Data          = 0;
      DdrCrClkControls.Bits.DllMask  = 1;                 // Set 2 qclk DLL mask
      DdrCrClkControls.Bits.VccddqHi = VddHi;             // Set Vdd
#ifdef ULT_FLAG
      if (Lpddr) {
        DdrCrClkControls.Bits.LPDDR_Mode = 1;
      }
#endif // ULT_FLAG
      Offset = DDRCLKCH0_CR_DDRCRCLKCONTROLS_REG +
        ((DDRCLKCH1_CR_DDRCRCLKCONTROLS_REG - DDRCLKCH0_CR_DDRCRCLKCONTROLS_REG) * Channel);
      MrcWriteCR (MrcData, Offset, DdrCrClkControls.Data);

      DdrCrCmdControls.Data = DdrCrClkControls.Data;
      DdrCrCtlControls.Data = DdrCrClkControls.Data;
      // Determine if weaklock can or can not be enabled
      //
      if (Inputs->WeaklockEn) {
        DdrCrCmdControls.Bits.DllWeakLock = NS;
        DdrCrCtlControls.Bits.DllWeakLock = NS;
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init CMD N and P CRs\n");
      //
      // Initialize CmdN/CmdS CRx
      //
      DdrCrCmdControls.Bits.EarlyWeakDrive  = 3;
      DdrCrCmdControls.Bits.CmdTxEq         = NS & 1; // Enable Early Warning and Cmd DeEmphasis
      MrcWriteCR (MrcData, DDRCMDCH0_CR_DDRCRCMDCONTROLS_REG +
        ((DDRCMDCH1_CR_DDRCRCMDCONTROLS_REG - DDRCMDCH0_CR_DDRCRCMDCONTROLS_REG) * Channel), DdrCrCmdControls.Data);

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init CKE CRs\n");
      //
      // Initialize CKE CRs
      //
      // @todo: DONE Either CKE and CTL must be set the same or we have to be using the per channel defines and not a Multicast.
      //
      DdrCrCkeControls.Data           = DdrCrCmdControls.Data;
      DdrCrCkeControls.Bits.CtlSRDrv  = NS & 2;
      DdrCrCkeControls.Bits.CtlTxEq   = NS & 1; // Enable Weak CKE in SR and Cke DeEmphasis
      MrcWriteCR (
        MrcData,
        DDRCKECH0_CR_DDRCRCTLCONTROLS_REG +
        ((DDRCKECH1_CR_DDRCRCTLCONTROLS_REG - DDRCKECH0_CR_DDRCRCTLCONTROLS_REG) * Channel),
        DdrCrCkeControls.Data
        );

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init CTL CRs\n");
      //
      // Initialize CTL CRs
      //
      DdrCrCtlControls.Bits.CtlTxEq  = (NS & 1);  // Enable Weak CKE in SR and Cke DeEmphasis
      DdrCrCtlControls.Bits.CtlSRDrv = (NS & 2);  // Enable Weak CKE in SR and Cke DeEmphasis
      DdrCrCtlControls.Bits.LaDrvEnOvrd = 1;
      MrcWriteCR (
        MrcData,
        DDRCTLCH0_CR_DDRCRCTLCONTROLS_REG +
        ((DDRCTLCH1_CR_DDRCRCTLCONTROLS_REG - DDRCTLCH0_CR_DDRCRCTLCONTROLS_REG) * Channel),
        DdrCrCtlControls.Data
        );
      //
      // Initialize CRs shared between CKE/CTL/CMD/CLK
      //
      // PI setting must match value written for TxDQs above.
      // There are no shared registers for CLK, only CKE/CTL but only CTLPICODING and CTLCOMPOFFSET
      // Set CTL/CLK PI to 64, and CMD to 96 (64 + 1/2 QCLK), for ideal initial command centering.
      //
      DdrCrCtlPiCoding.Data            = 0;
      DdrCrCtlPiCoding.Bits.CtlPiCode0 =
      DdrCrCtlPiCoding.Bits.CtlPiCode1 =
      DdrCrCtlPiCoding.Bits.CtlPiCode2 =
      DdrCrCtlPiCoding.Bits.CtlPiCode3 = 96;

      Offset = DDRCMDCH0_CR_DDRCRCMDPICODING_REG +
        ((DDRCMDCH1_CR_DDRCRCMDPICODING_REG - DDRCMDCH0_CR_DDRCRCMDPICODING_REG) * Channel);
#ifdef ULT_FLAG
      if (CpuModel == cmHSW_ULT) {
        //
        // On ULT we have DdrCrCmdPiCoding.CmdPi0Code and CmdPi1Code
        //
        MrcWriteCR (MrcData, Offset, DdrCrCtlPiCoding.Data);
      }
#endif // ULT_FLAG
#ifdef TRAD_FLAG
      if ((CpuModel == cmHSW) || (CpuModel == cmCRW)) {
        MrcWriteCR8 (MrcData, Offset, (U8) DdrCrCtlPiCoding.Bits.CtlPiCode0);
      }
#endif // TRAD_FLAG

      Offset = DDRCKECH0_CR_DDRCRCMDPICODING_REG +
        ((DDRCKECH1_CR_DDRCRCMDPICODING_REG - DDRCKECH0_CR_DDRCRCMDPICODING_REG) * Channel);
      MrcWriteCR (MrcData, Offset, DdrCrCtlPiCoding.Data);

      DdrCrCtlPiCoding.Bits.CtlPiCode0 =
      DdrCrCtlPiCoding.Bits.CtlPiCode1 =
      DdrCrCtlPiCoding.Bits.CtlPiCode2 =
      DdrCrCtlPiCoding.Bits.CtlPiCode3 = 64;

      Offset = DDRCKECTLCH0_CR_DDRCRCTLPICODING_REG +
        ((DDRCKECTLCH1_CR_DDRCRCTLPICODING_REG - DDRCKECTLCH0_CR_DDRCRCTLPICODING_REG) * Channel);
      MrcWriteCR (MrcData, Offset, DdrCrCtlPiCoding.Data);

      Offset = DDRCLKCH0_CR_DDRCRCLKPICODE_REG +
        ((DDRCLKCH1_CR_DDRCRCLKPICODE_REG - DDRCLKCH0_CR_DDRCRCLKPICODE_REG) * Channel);
      MrcWriteCR (MrcData, Offset, DdrCrCtlPiCoding.Data);

      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        ChannelOut->ClkPiCode[Rank] = (U8) DdrCrCtlPiCoding.Bits.CtlPiCode0;
      }

      Offset = DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_REG +
        ((DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_REG - DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_REG) * Channel);
      MrcWriteCR (MrcData, Offset, 0x0);     // Zero for now. May offset comp in future

      Offset = DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_REG +
        ((DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_REG - DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_REG) * Channel);
      MrcWriteCR (MrcData, Offset, 0x0);  // Zero for now. May offset comp in future

      Offset = DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_REG +
        ((DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_REG - DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_REG) * Channel);
      MrcWriteCR (MrcData, Offset, 0x0);     // Zero for now. May offset comp in future
    }
  } // End of for Channel...

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init COMP CRs\n");
  //
  // Initialize COMP CRs
  //
  // 14:11 DqDrv  19:15 DqOdt  23:20 CmdDrv  27:24 CtlDrv  31:28 ClkDrv
  //
  Outputs->CompCtl0 = (DisableOdtStatic << 3); // Disable ODT Static Leg

  if ((Any2dpc) && (Inputs->BoardType == btCRBDT)) {
    TargetRcomp[1] = 60;
  }

#ifdef ULT_FLAG
  if (CpuModel == cmHSW_ULT) {
    //
    // RCOMP1 resistor is 120 Ohm on ULT boards
    // This is used for DQ/CLK Ron (drive strength)
    //
    ReferenceR[0]  = 40;
    TargetRcomp[0] = 40;

    ReferenceR[4]  = 40;
  }
#endif // ULT_FLAG

  for (i = 0; i < 5; i++) {
    CompVref = (StepSize[i] * (ReferenceR[i] - TargetRcomp[i])) / (2 * (ReferenceR[i] + TargetRcomp[i]));
    if (i == 1) {
      //
      // DqOdt is 5 bits
      //
      if (CompVref > 15) {
        CompVref = 15;
      } else if (CompVref < -16) {
        CompVref = -16;
      }

      Outputs->CompCtl0 |= (CompVref & 0x1F) << (15);
    } else {
      if (CompVref > 7) {
        CompVref = 7;
      } else if (CompVref < -8) {
        CompVref = -8;
      }

      if (i == 0) {
        Outputs->CompCtl0 |= (CompVref & 0xF) << (11);
      } else {
        Outputs->CompCtl0 |= (CompVref & 0xF) << (12 + 4 * i);
      }
    }
    //
    //    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CompVref[%d] = 0x%x\n", i, CompVref);
    //
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CompCtl0 = 0x%08X\n", Outputs->CompCtl0);
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPCTL0_REG, Outputs->CompCtl0);

  CompDdrCrCompCtl1.Data           = 0;
  CompDdrCrCompCtl1.Bits.VccddqHi  = VddHi;            // Set Vdd, 2 qclk DLL mask
  CompDdrCrCompCtl1.Bits.CompClkOn = SAFE;             // Override PwrDn in Safe Mode

  Cmd2N = FALSE;
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (Outputs->Controller[Controller].Channel[Channel].Timing[Profile].NMode == 2)
        Cmd2N = TRUE;
      if (Cmd2N) {
        break;
      }
    }
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      Outputs->Controller[Controller].Channel[Channel].Timing[Profile].NMode = (Cmd2N == TRUE) ? 2 : 1;
    }
  }

  //
  // Calculate how to program the slew rate targets
  // Buffer Type         DQ  CMD  CTL  CLK
  // Num Stages           3    5    5    3
  // Slew Rate          4.5  3.0  3.0  5.0
  // Derating            .8   .8   .8   .8
  //

/*
  U8  BufferStageDelayPS[4] = {92, 83, 83, 83}; // Slew = 1V(in mv) / (# Stages * StageDelayPS * Derating)
  U8  MinCycleStageDelay[4] = {46, 70, 70, 46}; // Avoid corner case
  U8  i;
  U16  NumStages;
*/
  if (Cmd2N == TRUE) {
    BufferStageDelayPS[1] = 89; // CMD Slew Rate  = 1.8  for 2N
  }

#ifdef ULT_FLAG
  if (Lpddr) {
    BufferStageDelayPS[1] = 63; // CMD Slew Rate  = 4 V/ns for double-pumped CMD bus
  }
#endif // ULT_FLAG

  for (i = 0; i < 4; i++) {
    //
    // Calculate DQ, CMD, CTL, CLK
    // Number of Stages in DLL, rounded to nearest integer
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "BufferStageDelayPS[%d] = %d\n", i, BufferStageDelayPS[i]);
    //
    NumStages = (Outputs->Qclkps + BufferStageDelayPS[i] / 2) / BufferStageDelayPS[i];
    if (NumStages < 5) {
      NumStages = 5;  // Minimum setting > 3
    }
    //
    // Lock DLL ....
    //
    Offset = i * (DDRCOMP_CR_DDRCRCOMPCTL1_DqScompCells_WID + DDRCOMP_CR_DDRCRCOMPCTL1_DqScompPC_WID);
    if ((NumStages > 16) || (BufferStageDelayPS[i] < MinCycleStageDelay[i])) {
      CompDdrCrCompCtl1.Data += ((NumStages / 2 - 1) << Offset); // ... to a phase
    } else {
      CompDdrCrCompCtl1.Data += (16 + NumStages - 1) << Offset;  // ... to a cycle
    }
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Qclkps = %d, NumStages = %d\n",Outputs->Qclkps, NumStages);
    //
  }

  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPCTL1_REG, CompDdrCrCompCtl1.Data);
  Outputs->CompCtl1 = CompDdrCrCompCtl1.Data;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CompCtl1 = 0x%x\n", CompDdrCrCompCtl1.Data);

  //
  // Calculate Target Values for VssHi Panic Driver
  //
  // Rtarget = Tperiod / Cdie / ln( VssHi / (VssHi - Delta) )
  //

/*
  U8  delta = 15;                               // VssHi change voltage during panic: 15mV
  U16 lndown;
  U16 Rdown;
  U16 vrefdown;
  U16 vsshiu, vsshid;
  U16 vpanic = 24;  // Panic Treshold at 24 mV
  U16 lnup;
  U16 Rup;
  U32 vrefup;
*/
  vsshid = VssHi + vpanic;
  //
  //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "vsshid = %d  VssHi = %d  vpanic = %d  \n", vsshid, VssHi, vpanic);
  // Calculate log to backsolve exp. RC decay
  // Input should be 100x.  Output is 100x
  //
  lndown = (U16) MrcNaturalLog ((100 * vsshid) / (vsshid - delta));
  Rdown = (Outputs->Qclkps * 2000) / (CDIEVSSHI * lndown); // Rdown is 10x.
  vrefdown = (128 * Rdown) / (Rdown + 10 * RCMDREF);  // Multiple RcmdRef by 10x to match Rdown
  //
  //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "vrefdown = %d  Rdown = %d  lndown = %d  \n", vrefdown, Rdown, lndown);
  //
  vsshiu = (Inputs->VccIomV - VssHi - vpanic);  // if VccIO == 1v then VccmV = 1000
  lnup = (U16) MrcNaturalLog ((100 * vsshiu) / (vsshiu - delta));
  Rup = (Outputs->Qclkps * 2000) / (CDIEVSSHI * lnup);
  vrefup = (128 * 10 * RCMDREF) / (10 * RCMDREF + Rup) - 64;
  //
  //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "vrefup = %d  Rup = %d  lnup = %d  vsshiu = %d\n", vrefup, Rup, lnup, vsshiu);
  //
  DdrCrCompVssHi.Data          = 0;
  DdrCrCompVssHi.Bits.VtSlopeA = 4;                   // Apply slope correction of 1.5 to VtComp
  DdrCrCompVssHi.Bits.VtOffset = (128 * 450 / Inputs->VccIomV / 2); // Apply offset correction to VtComp
  DdrCrCompVssHi.Bits.PanicDrvUpVref = vrefup;        // Apply Calculated Vref Values
  DdrCrCompVssHi.Bits.PanicDrvDnVref = vrefdown;
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPVSSHI_REG, DdrCrCompVssHi.Data);
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DdrCrCompVssHi = 0x%x\n", DdrCrCompVssHi.Data);
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init MISC CRs\n");
  //
  // Initialize MISC CRs
  //

  DdrMiscControl.Data                  = MrcReadCR (MrcData, DDRSCRAM_CR_DDRMISCCONTROL0_REG);
  DdrMiscControl.Bits.WeakLock_Latency = 12;
  DdrMiscControl.Bits.WL_SleepCycles   = 5;
  DdrMiscControl.Bits.WL_WakeCycles    = 2;
  Outputs->MiscControl0                = DdrMiscControl.Data;
  MrcWriteCR (MrcData, DDRSCRAM_CR_DDRMISCCONTROL0_REG, DdrMiscControl.Data);

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    Offset = DDRSCRAM_CR_DDRSCRAMBLECH0_REG +
      ((DDRSCRAM_CR_DDRSCRAMBLECH1_REG - DDRSCRAM_CR_DDRSCRAMBLECH0_REG) * Channel);
    MrcWriteCR (MrcData, Offset, 0);// Keep scrambling disabled for training
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Init KEY MC CRs\n");
  //
  // Initialize some key MC CRs
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut = &ControllerOut->Channel[Channel];

    //
    // set the valid rank - Either clear or set only populated so no check for vaid channel
    //
    Offset = MCHBAR_CH0_CR_MC_INIT_STATE_REG +
      ((MCHBAR_CH1_CR_MC_INIT_STATE_REG - MCHBAR_CH0_CR_MC_INIT_STATE_REG) * Channel);
    MrcWriteCR8 (MrcData, Offset, ChannelOut->ValidRankBitMask);
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Update LS COMP CRs\n");

  //
  // 1st Disable Perioid Comp and wait for 10us
  // Set periodic comp = (10uS * 2^COMP_INT)
  //
  CrMCompPcu.Data               = 0;
  CrMCompPcu.Bits.COMP_DISABLE  = 1;
  CrMCompPcu.Bits.COMP_FORCE    = 1;
  CrMCompPcu.Bits.COMP_INTERVAL = COMP_INT;  // Set COMP_INT to happen every 10mS
  MrcWriteCR (MrcData, PCU_CR_M_COMP_PCU_REG, CrMCompPcu.Data);
  MrcWait (MrcData, 10 * HPET_1US);

  //
  // Override LevelShifter Compensation to 0x4 (From Hua, 3 is not a valid value)
  //
  DataRCompData.Data                  = MrcReadCR (MrcData, DDRDATA_CR_RCOMPDATA1_REG);
  DataRCompData.Bits.LevelShifterComp = 2;
  MrcWriteCrMulticast (MrcData, DDRDATA_CR_RCOMPDATA1_REG, DataRCompData.Data);
  CmdDdrCrCmdComp.Data        = MrcReadCR (MrcData, DDRCMD_CR_DDRCRCMDCOMP_REG);
  CmdDdrCrCmdComp.Bits.LsComp = 2;
  MrcWriteCrMulticast (MrcData, DDRCMD_CR_DDRCRCMDCOMP_REG, CmdDdrCrCmdComp.Data);
  CkeCtlDdrCrCtlComp.Data        = MrcReadCR (MrcData, DDRCKECTL_CR_DDRCRCTLCOMP_REG);
  CkeCtlDdrCrCtlComp.Bits.LsComp = 2;
  MrcWriteCrMulticast (MrcData, DDRCKECTL_CR_DDRCRCTLCOMP_REG, CkeCtlDdrCrCtlComp.Data);
  ClkDdrCrClkComp.Data        = MrcReadCR (MrcData, DDRCLK_CR_DDRCRCLKCOMP_REG);
  ClkDdrCrClkComp.Bits.LsComp = 2;
  MrcWriteCrMulticast (MrcData, DDRCLK_CR_DDRCRCLKCOMP_REG, ClkDdrCrClkComp.Data);
  CompDdrCrDataComp.Data                  = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG);
  CompDdrCrDataComp.Bits.LevelShifterComp = 2;
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG, CompDdrCrDataComp.Data);
  CompDdrCrCmdComp.Data        = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRCMDCOMP_REG);
  CompDdrCrCmdComp.Bits.LsComp = 2;
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCMDCOMP_REG, CompDdrCrCmdComp.Data);
  CompDdrCrCtlComp.Data        = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRCTLCOMP_REG);
  CompDdrCrCtlComp.Bits.LsComp = 2;
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCTLCOMP_REG, CompDdrCrCtlComp.Data);
  CompDdrCrClkComp.Data        = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRCLKCOMP_REG);
  CompDdrCrClkComp.Bits.LsComp = 2;
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCLKCOMP_REG, CompDdrCrClkComp.Data);
  CompDdrCrCompOvr.Data        = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRCOMPOVR_REG);
  CompDdrCrCompOvr.Bits.LsComp = 2;
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPOVR_REG, CompDdrCrCompOvr.Data);

  //
  // Manually update the comp values
  //
  DdrMiscControl.Data                 = Outputs->MiscControl0;
  DdrMiscControl.Bits.ForceCompUpdate = 1;
  MrcWriteCR (MrcData, DDRSCRAM_CR_DDRMISCCONTROL0_REG, DdrMiscControl.Data);

  //
  // Fix Offset between ODT Up/Dn
  //
  CompDdrCrDataComp.Data              = MrcReadCR (MrcData, DDRCOMP_CR_DDRCRDATACOMP1_REG);
  CompDdrCrCompCtl0.Data              = Outputs->CompCtl0;
  //
  // Calculate (OdtDn - OdtUp) - Will be BITS 9:4
  //
  CompDdrCrCompCtl0.Bits.DqOdtUpDnOff = CompDdrCrDataComp.Bits.RcompOdtDown - CompDdrCrDataComp.Bits.RcompOdtUp;
  CompDdrCrCompCtl0.Bits.FixOdtD      = 1;  // Enable Fixed Offset between OdtUp/Dn - Will be BIT10
  Outputs->CompCtl0                   = CompDdrCrCompCtl0.Data;
  MrcWriteCR (MrcData, DDRCOMP_CR_DDRCRCOMPCTL0_REG, CompDdrCrCompCtl0.Data);

  //
  // 2X Refresh
  //
  if (
       (Inputs->RefreshRate2x == TRUE) &&
       (
         ((CpuModel == cmHSW) && (CpuStepping >= csHswC0)) ||
         ((CpuModel == cmCRW) && (CpuStepping >= csCrwC0)) ||
         ((CpuModel == cmHSW_ULT) && (CpuStepping >= csHswUltC0))
       )
     ) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "*** Enabling 2x Refresh ***\n");
    AutoSelfRefresh = Outputs->AutoSelfRefresh;

    if ((AutoSelfRefresh == FALSE)
#ifdef ULT_FLAG
        || (Lpddr == TRUE)
#endif
       ){
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Enabling Mailbox 2x Refresh\n");
      MrcOemEnable2xRefresh (MrcData);
    }

    //
    // Percentage reduction of tREFI needed for ASR and LPDDR cases (Mutually Exclusive).
    //
    if (AutoSelfRefresh == TRUE) {
      RefiReduction = 50;
    }
#ifdef ULT_FLAG
    if (Lpddr == TRUE) {
      DdrPtmCtl.Data                 = MrcReadCR (MrcData, PCU_CR_DDR_PTM_CTL_PCU_REG);
      DdrPtmCtl.Bits.DISABLE_DRAM_TS = 0;
      MrcWriteCR (MrcData, PCU_CR_DDR_PTM_CTL_PCU_REG, DdrPtmCtl.Data);
      RefiReduction                  = 97;
    }
#endif

    if ((Inputs->BootMode == bmCold) && ((AutoSelfRefresh == TRUE)
#ifdef ULT_FLAG
        || (Lpddr == TRUE)
#endif
       )) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "%s Detected, Reducing tREFI by %u percent.\n",
        (AutoSelfRefresh == TRUE) ? "Auto Self Refresh" : "LPDDR",
        RefiReduction
        );
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcChannelExist (Outputs, Channel)) {
          ChannelOut = &ControllerOut->Channel[Channel];
          ChannelOut->Timing[STD_PROFILE].tREFI = (ChannelOut->Timing[STD_PROFILE].tREFI * RefiReduction) / 100;
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "  C(%d).tREFI = 0x%x\n", Channel, ChannelOut->Timing[STD_PROFILE].tREFI);
          for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
            DimmOut = &ChannelOut->Dimm[Dimm];
            if (DimmOut->Status == DIMM_PRESENT) {
              DimmOut->Timing[STD_PROFILE].tREFI = (DimmOut->Timing[STD_PROFILE].tREFI * RefiReduction) / 100;
              MRC_DEBUG_MSG (
                Debug,
                MSG_LEVEL_NOTE,
                "  C(%d).D(%d).tREFI = 0x%x\n",
                Channel,
                Dimm,
                DimmOut->Timing[STD_PROFILE].tREFI
                );
            }
          }
        }
      }
    }
  }

  //
  // Set the DDR voltage in PCU
  //
  MrcSetPcuDdrVoltage (MrcData, Vdd);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Timing Config\n");
  MrcTimingConfiguration (MrcData);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Refresh Config\n");
  MrcRefreshConfiguration (MrcData);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Scheduler parameters\n");
  MrcSchedulerParametersConfig (MrcData);

  //
  // this function must be in the end.
  // if one of the function close channel the function execute this close.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Address Decoding Config\n");
  MrcAdConfiguration (MrcData);

  return Status;
}

/**
@brief
  This function init all the necessary registers for the training.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess
**/
MrcStatus
MrcPreTraining (
  IN MrcParameters *const MrcData
  )
{
  MrcOutput                             *Outputs;
  MrcControllerOut                      *ControllerOut;
  MrcChannelOut                         *ChannelOut;
  U32                                   Offset;
  U8                                    Channel;
  U8                                    Rank;
  U8                                    RankMod2;
  MCDECS_CR_MAD_DIMM_CH0_MCMAIN_STRUCT  CrMadDimmCh;
  const MrcDebug                        *Debug;

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }

    //
    // Dump the MR registers for DDR3
    // LPDDR Jedec Init is done after Early Command Training
    //
    if (Outputs->DdrType != MRC_DDR_TYPE_LPDDR3) {
      ChannelOut = &ControllerOut->Channel[Channel];

      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (!(MrcRankInChannelExist (MrcData, Rank, Channel))) {
          continue;
        }

        RankMod2 = Rank % 2;
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "MrcSetMR0  Channel %u Rank %u = 0x%X\n",
          Channel,
          Rank,
          ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR0]
          );

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "MrcSetMR1  Channel %u Rank %u = 0x%X\n",
          Channel,
          Rank,
          ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR1]
          );

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "MrcSetMR2  Channel %u Rank %u = 0x%X\n",
          Channel,
          Rank,
          ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR2]
          );

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "MrcSetMR3  Channel %u Rank %u = 0x%X\n",
          Channel,
          Rank,
          ChannelOut->Dimm[RANK_TO_DIMM_NUMBER (Rank)].Rank[RankMod2].MR[mrMR3]
          );
      }
    }

    if (Outputs->EccSupport == TRUE) {
      Offset = MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG +
        ((MCDECS_CR_MAD_DIMM_CH1_MCMAIN_REG - MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG) * Channel);
      CrMadDimmCh.Data = MrcReadCR (MrcData, Offset);
      //
      // set ECC IO ACTIVE ONLY - NOT IO
      //
      CrMadDimmCh.Bits.ECC = emEccIoActive;
      MrcWriteCR (MrcData, Offset, CrMadDimmCh.Data);
      //
      // Wait 4 usec after enabling the ECC IO, needed by HW
      //
      MrcWait (MrcData, 4 * HPET_1US);
    }
  } // for Channel

  //
  // Set up Write data Buffer before training steps
  //
  SetupWDB (MrcData);

  return mrcSuccess;
}

/**

@brief

  This function initializes all the necessary registers after main training steps but before LCT.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess

**/
MrcStatus
MrcPostTraining (
  IN MrcParameters *const MrcData
  )
{
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcProfile        Profile;
  U8                Channel;

  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Profile       = MrcData->SysIn.Inputs.MemoryProfile;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      //
      // Update CmdN timing, Round Trip Latency and tXP
      // OldN=3, NewN=2*Cmd2N
      //
      UpdateCmdNTiming (MrcData, Channel, 2 * 2, (ControllerOut->Channel[Channel].Timing[Profile].NMode == 2) ? 2 : 0);
    }
  }

  return mrcSuccess;
}

/**
@brief
  Program PCU_CR_DDR_VOLTAGE register.

  @param[in] MrcData    - Include all MRC global data.
  @param[in] VddVoltage - Current DDR voltage.

  @retval none
**/
void
MrcSetPcuDdrVoltage (
  IN OUT MrcParameters *MrcData,
  IN     MrcVddSelect  VddVoltage
  )
{
  MrcInput                      *Inputs;
  MrcOutput                     *Outputs;
  U8                            Data8;
  PCU_CR_DDR_VOLTAGE_PCU_STRUCT DdrVoltage;

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;

  switch (VddVoltage) {
    case VDD_1_35:
      Data8 = 1;
      break;

    case VDD_1_20:
      Data8 = 3;  // @todo For single CA bus set this to 2
      break;

    default:
      Data8 = 0;
  }

  MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "PCU_CR_DDR_VOLTAGE = 0x%02X\n", Data8);
  DdrVoltage.Data = 0;
  DdrVoltage.Bits.DDR_VOLTAGE = Data8;
  MrcWriteCR (MrcData, PCU_CR_DDR_VOLTAGE_PCU_REG, DdrVoltage.Data);
}
