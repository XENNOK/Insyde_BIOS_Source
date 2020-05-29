/** @file

  This file all the MRC general API to the MRC wrapper.
  
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

#include <MrcGeneral.h>
#include <MrcDdr3.h>
const MrcVersion  cVersion = {
#include <MrcVersion.h>
};

#ifdef ULT_FLAG
//
// This table is used for LPDDR3 MR5 decoding
//
struct {
  U8    VendorId;
  char  *VendorName;
} DramVendorList [] = {
  { 1, "Samsung" },
  { 3, "Elpida"  },
  { 6, "Hynix"   }
};
#endif // ULT_FLAG

/**
@brief
  Thisfunction performs Software Memory testing

  @param[in] MrcData - Include all MRC global data.

  @retval Always returns mrcSuccess.
**/
MrcStatus
MrcHwMemTest (
  IN     MrcParameters *const MrcData
  )
{
  MrcStatus             Status;

  Status  = mrcSuccess;

  return Status;
}

/**
@brief
  This function changes the MC to normal mode, enables the ECC if needed, lock configuration and set PU_MRC_Done.
  If the ECC is enabled, this function should be called after memory is cleaned.

  @param[in, out] MrcData - Include all MRC global data.

  @retval Always returns mrcSuccess.
**/
MrcStatus
MrcMcActivate (
  IN     MrcParameters *const MrcData
  )
{
  const MrcInput                            *Inputs;
  const MrcDebug                            *Debug;
  MrcOutput                                 *Outputs;
  MrcControllerOut                          *ControllerOut;
  MrcChannelOut                             *ChannelOut;
  MrcCpuModel                               CpuModel;
  MrcCpuStepping                            CpuStepping;
  MCHBAR_CH0_CR_CMD_RATE_STRUCT             CmdRate;
  DDRSCRAM_CR_DDRSCRAMBLECH0_STRUCT         DdrScramble;
  MCDECS_CR_MAD_DIMM_CH0_MCMAIN_STRUCT      DimmCh0McMain;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT ReutChSeqCfg;
  PCU_CR_M_COMP_PCU_STRUCT                  MCompPcu;
  MCHBAR_CH0_CR_TC_BANK_RANK_A_STRUCT       TcBankRankA;
  U32                                       Offset;
  U32                                       GeneratedSeed;
  U8                                        Controller;
  U8                                        Channel;
  U8                                        Byte;
  U32                                       BurstEndOdtDelay;
#ifdef ULT_FLAG
  U8                                        Rank;
  U8                                        MaxRcvEn;
  U8                                        RcvEnDrift;
  U8                                        RcvEnTurnOff;
  S8                                        OdtTurnOff;
#endif // ULT_FLAG
  BOOL                                      Lpddr;

  Inputs        = &MrcData->SysIn.Inputs;
  Outputs       = &MrcData->SysOut.Outputs;
  Debug         = &Inputs->Debug;
  GeneratedSeed = 0;
  CpuModel      = Inputs->CpuModel;
  CpuStepping   = Inputs->CpuStepping;
  
  //
  // Oem hook before normal mode configuration starts
  //
  MrcOemCheckPoint (MrcData, OemBeforeNormalMode, NULL);

  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerOut = &Outputs->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(MrcChannelExist (Outputs, Channel))) {
        continue;
      }

      ChannelOut = &ControllerOut->Channel[Channel];

      //
      // Make sure tRDRD (sr, dr, dd) are above 6 for Scrambler W/A
      //
      if ((Inputs->ScramblerEnable == TRUE) &&
          ((CpuModel == cmHSW     && CpuStepping < csHswC0)    ||
           (CpuModel == cmHSW_ULT && CpuStepping < csHswUltC0) ||
           (CpuModel == cmCRW     && CpuStepping < csCrwC0)
          )
         ) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Increasing tRDRD(sr,dr,dd) by two:\n");

        Offset = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG +
          (MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * Channel;

        TcBankRankA.Data = MrcReadCR (MrcData, Offset);

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "Initial Value: Channel %d\n tRDRD    = 0x%X\n tRDRD_dr = 0x%X\n tRDRD_dd = 0x%X\n",
          Channel,
          TcBankRankA.Bits.tRDRD,
          TcBankRankA.Bits.tRDRD_dr,
          TcBankRankA.Bits.tRDRD_dd
          );

        TcBankRankA.Bits.tRDRD    = MAX (TcBankRankA.Bits.tRDRD, 6);
        TcBankRankA.Bits.tRDRD_dr = MAX (TcBankRankA.Bits.tRDRD_dr, 6);
        TcBankRankA.Bits.tRDRD_dd = MAX (TcBankRankA.Bits.tRDRD_dd, 6);

        MrcWriteCR (MrcData, Offset, TcBankRankA.Data);
        ChannelOut->MchbarBANKRANKA = TcBankRankA.Data;

        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "New value: Channel %d\n tRDRD    = 0x%X\n tRDRD_dr = 0x%X\n tRDRD_dd = 0x%X\n",
          Channel,
          TcBankRankA.Bits.tRDRD,
          TcBankRankA.Bits.tRDRD_dr,
          TcBankRankA.Bits.tRDRD_dd
          );
      }

      //
      // Enable Scrambling
      //
      if (Inputs->ScramblerEnable == TRUE) {
        GeneratedSeed = MrcGetRandomNumber ();
        //
        // Set Scramble key and enable bits
        //
        DdrScramble.Data          = 0;
        DdrScramble.Bits.ScramKey = GeneratedSeed;
        DdrScramble.Bits.ScramEn  = 1;
        MrcWriteCR (
          MrcData,
          DDRSCRAM_CR_DDRSCRAMBLECH0_REG + ((DDRSCRAM_CR_DDRSCRAMBLECH1_REG - DDRSCRAM_CR_DDRSCRAMBLECH0_REG) * Channel),
          DdrScramble.Data
          );
      }

      //
      // If we are in 1N mode, set Command Rate Limit to 3
      //
      if (ChannelOut->Timing[Inputs->MemoryProfile].NMode == 1) {
        Offset = MCHBAR_CH0_CR_CMD_RATE_REG + ((MCHBAR_CH1_CR_CMD_RATE_REG - MCHBAR_CH0_CR_CMD_RATE_REG) * Channel);
        CmdRate.Data = MrcReadCR (MrcData, Offset);
        CmdRate.Bits.enable_cmd_rate_limit  = 1;
        CmdRate.Bits.cmd_rate_limit         = 3;
        CmdRate.Bits.enable_cmd_rate_limit  = Inputs->EnCmdRate & 1;
        CmdRate.Bits.cmd_rate_limit         = Inputs->EnCmdRate >> 1;
        MrcWriteCR (MrcData, Offset, CmdRate.Data);
      }

      //
      // Enable the command tri state at the end of the training.
      //
      if (!Inputs->CmdTriStateDis) {
        TcBankRankA.Data            = ChannelOut->MchbarBANKRANKA;
        TcBankRankA.Bits.CMD_3st    = 0;
        ChannelOut->MchbarBANKRANKA = TcBankRankA.Data;
        MrcWriteCR (
          MrcData,
          MCHBAR_CH0_CR_TC_BANK_RANK_A_REG +
          ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * Channel),
          ChannelOut->MchbarBANKRANKA
          );
      }

      //
      // set MC to normal mode and clean the odt and cke.
      //
      ReutChSeqCfg.Data                     = 0;
      ReutChSeqCfg.Bits.Initialization_Mode = NOP_Mode;
      MrcWriteCR (
        MrcData,
        MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel),
        (U32) ReutChSeqCfg.Data
        );

      //
      // set again the rank occupancy
      //
      MrcWriteCR8 (
        MrcData,
        MCHBAR_CH0_CR_MC_INIT_STATE_REG + ((MCHBAR_CH1_CR_MC_INIT_STATE_REG - MCHBAR_CH0_CR_MC_INIT_STATE_REG) * Channel),
        ChannelOut->ValidRankBitMask
        );

      //
      // Set the MC to ECC mode for all channels if needed.
      //
      if (Outputs->EccSupport == TRUE) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ECC support\n");
        Offset = MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG + ((MCDECS_CR_MAD_DIMM_CH1_MCMAIN_REG - MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG) * Channel);
        DimmCh0McMain.Data      = MrcReadCR (MrcData, Offset);
        DimmCh0McMain.Bits.ECC  = emBothActive;
        MrcWriteCR (MrcData, Offset, DimmCh0McMain.Data);
      }
    }
  }

  //
  // Check if LPDDR3 memory is used
  //
  Lpddr = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  //
  // Update Odt timing, Samp timing and SlaveDLL to minimize power
  // @todo TAT step is skipped on LPDDR for now.
  //
  if ((Inputs->TrainingEnables.TAT == 0) || Lpddr) {
    UpdateSampOdtTiming (MrcData, 0);
  }
#ifdef TRAD_FLAG
  //
  // Update Internal clock setting
  //
  if ((CpuModel == cmHSW) || (CpuModel == cmCRW)) {
    UpdateInternalClksOn (MrcData);
  }
#endif // TRAD_FLAG

  UpdateSlaveDLLLength (MrcData);

  //
  // Program BurstEndODTDelay - it should be zero during training steps
  //
  BurstEndOdtDelay = ((14300 * 20) / 100 + Outputs->Qclkps / 2) / Outputs->Qclkps;
  if (BurstEndOdtDelay > 7) {
    BurstEndOdtDelay = 7;
  }
  if (BurstEndOdtDelay < 3) {
    BurstEndOdtDelay = 0;
  } else if (BurstEndOdtDelay < 4) {
    BurstEndOdtDelay = 4;
  }

  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerOut = &Outputs->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        ChannelOut = &ControllerOut->Channel[Channel];
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          if (CpuModel == cmHSW_ULT) {
            ChannelOut->DqControl1[Byte].Bits.BurstEndODTDelay = (Lpddr) ? 0 : BurstEndOdtDelay;  // Must be Disabled for LPDDR
          } else if ((CpuModel == cmHSW) && (CpuStepping == csHswA0)) {
            ChannelOut->DqControl1[Byte].Bits.BurstEndODTDelay = 0;
          } else {
            ChannelOut->DqControl1[Byte].Bits.BurstEndODTDelay = BurstEndOdtDelay;
          }
          Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG +
            ((DDRDATA1CH0_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Byte) +
            ((DDRDATA0CH1_CR_DDRCRDATACONTROL1_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL1_REG) * Channel);
          MrcWriteCR (MrcData, Offset, ChannelOut->DqControl1[Byte].Data);
        }
      }
    }
  }

#ifdef ULT_FLAG
  if (CpuModel == cmHSW_ULT) {
    //
    // Program RxClkStgNum
    //
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!MrcChannelExist (Outputs, Channel)) {
          continue;
        }
        ChannelOut = &ControllerOut->Channel[Channel];
        MaxRcvEn   = 0;
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          if (!MrcRankInChannelExist (MrcData, Rank, Channel)) {
            continue;
          }
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            MaxRcvEn = (U8) MAX (MaxRcvEn, ChannelOut->RcvEn[Rank][Byte] / 64);
          }
        }
        RcvEnDrift   = (Lpddr) ? (U8) ((tDQSCK_DRIFT + Outputs->Qclkps - 1) / Outputs->Qclkps) : 1;
        RcvEnTurnOff = MaxRcvEn + (5 - 6) + 1 + 7 + 3 + 3 + 2 + (2 * RcvEnDrift);

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          if (ChannelOut->DqControl1[Byte].Bits.LpDdrLongOdtEn) {
            RcvEnTurnOff ++;
          }

          OdtTurnOff = MrcSE ((U8) ChannelOut->DqControl1[Byte].Bits.OdtDelay, 4, 8) +
                              (U8) ChannelOut->DqControl1[Byte].Bits.OdtDuration + 14;

          ChannelOut->DqControl2[Byte].Bits.RxClkStgNum = (ChannelOut->DqControl0.Bits.OdtSampExtendEn) ?
            MAX (ChannelOut->DqControl2[Byte].Bits.RxClkStgNum, RcvEnTurnOff) : MAX (17, OdtTurnOff);
          Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG +
            ((DDRDATA0CH1_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Channel) +
            ((DDRDATA1CH0_CR_DDRCRDATACONTROL2_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL2_REG) * Byte);
          MrcWriteCR (MrcData, Offset, ChannelOut->DqControl2[Byte].Data);
        }
      }
    }
  
    //
    // Program DDRPL_CR_DDR_TX_DELAY if Memory Trace is enabled
    //
    MrcSetDdrplTxDelay (MrcData);
  }
#endif // ULT_FLAG

  //
  // Enable Periodic Comp with periodic internal = 10uS*2^COMP_INT
  //
  MCompPcu.Data               = 0;
  MCompPcu.Bits.COMP_INTERVAL = COMP_INT;
  MrcWriteCR (MrcData, PCU_CR_M_COMP_PCU_REG, MCompPcu.Data);

  //
  // Enable the power mode before PCU start working.
  //
  MrcPowerModesPostTraining (MrcData);

  //
  // Set Idle timer and Self Refresh enable bits
  //
  EnterSR (MrcData);

  //
  // Oem hook when normal mode configuration is done
  //
  MrcOemCheckPoint (MrcData, OemAfterNormalMode, (void *) &Inputs->McLock);

  if (Inputs->ThermalEnables.UserPowerWeightsEn == 0) {
    //
    // Apply power weight values
    //
    MrcPowerWeight (MrcData);
  }

  return mrcSuccess;
}

/**
@brief
  This function enables Normal Mode and configures the Power Down Modes
  for the boot flows other than Cold Boot.

  @param[in] MrcData - The MRC general data.

  @retval Always returns mrcSuccess.
**/
MrcStatus
MrcNormalMode (
  IN MrcParameters *const MrcData
  )
{
  MrcOutput *Outputs;
  U8  Channel;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT ReutChSeqCfg;
  PCU_CR_M_COMP_PCU_STRUCT                  MCompPcu;

  Outputs = &MrcData->SysOut.Outputs;

  //
  // Enable Periodic Comp with periodic internal = 10uS*2^COMP_INT
  //
  MCompPcu.Data               = 0;
  MCompPcu.Bits.COMP_INTERVAL = COMP_INT;
  MrcWriteCR (MrcData, PCU_CR_M_COMP_PCU_REG, MCompPcu.Data);
  //
  // Set Normal Operation Mode.
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

  //
  // Configure Power Down CR
  //
  MrcPowerDownConfig (MrcData);

  return mrcSuccess;
}

/**
@brief
  this function is the last funtion that call from the MRC core.
    the function set DISB and set the MRC_Done.

  @param[in] MrcData - include all the MRC general data.

  @retval Always returns mrcSuccess.
**/
MrcStatus
MrcDone (
  IN     MrcParameters *const MrcData
  )
{
  const MrcInput                          *Inputs;
  const MrcDebug                          *Debug;
  MCDECS_CR_MC_INIT_STATE_G_MCMAIN_STRUCT McInitStateG;
#ifdef ULT_FLAG
  MrcOutput                               *Outputs;
  U32                                     Channel;
  U32                                     Rank;
  U8                                      MrrResult[4];
  U32                                     MrAddr;
  U32                                     Device;
  U32                                     Index;
#endif //ULT_FLAG

  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;

#ifdef ULT_FLAG
  //
  // LPDDR: Read MR5 and MR8
  //
  Outputs = &MrcData->SysOut.Outputs;
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (!MrcRankInChannelExist (MrcData, (U8) Rank, (U8) Channel)) {
          continue;
        }

        //
        // MR5 - Manufacturer ID
        //
        MrAddr = 5;
        MrcIssueMrr (MrcData, Channel, Rank, MrAddr, MrrResult);
        for (Device = 0; Device < 4; Device++) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\tDevice[%d]= 0x%02X", Device, MrrResult[Device]);
          for (Index = 0; Index < sizeof (DramVendorList) / sizeof (DramVendorList[0]); Index++) {
            if (DramVendorList[Index].VendorId == MrrResult[Device]) {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " %s\n", DramVendorList[Index].VendorName);
            }
          }
        }

        //
        // MR8 - I/O Width, Density, Type
        //
        MrAddr = 8;
        MrcIssueMrr (MrcData, Channel, Rank, MrAddr, MrrResult);
        for (Device = 0; Device < 4; Device++) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\tDevice[%d]= 0x%02X - %s\n", Device, MrrResult[Device],
            (MRC_BIT6 & MrrResult[Device]) ? "x16" : "x32");
        }
      }
    }
  }
#endif //ULT_FLAG

  //
  // Set Idle timer and Self Refresh enable bits
  //  EnterSR (MrcData);
  //
  McInitStateG.Data = MrcReadCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG);
  //
  // Set refresh enable Bit
  //
  McInitStateG.Bits.refresh_enable = 1;

  //
  // used to know what is the state of the boot mode.
  //
  McInitStateG.Bits.pu_mrc_done = 1;

  //
  // set the MRC_Done bit.
  //
  McInitStateG.Bits.mrc_done = 1;

  MrcWriteCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG, McInitStateG.Data);

  //
  // lock the MC and memory map registers.
  //
    McRegistersLock (MrcData);

  //
  // Poll for to make sure MRC is complete
  //
  // wait for mc_init_done
  // @TODO: Possible infinite loop. Need to add a timeout counter/error handler.
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Waiting for mc_init_done Acknowledge\n");
  do {
    McInitStateG.Data = MrcReadCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG);
  } while (McInitStateG.Bits.mc_init_done_ack == 0);
  //
  // move the MRC data to the graphics driver.
  //
  MrcWmRegSet (MrcData);
  return mrcSuccess;
}

/**
@brief
  Print the MRC version to the MRC output device.

  @param[in] Debug   - Pointer to the MRC Debug structure.
  @param[in] Version - The MRC version.

  @retval Nothing.
**/
void
MrcVersionPrint (
  IN const MrcParameters *MrcData,
  IN const MrcVersion    *Version
  )
{
#ifdef MRC_DEBUG_PRINT
  const MrcDebug *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,  "*********************************************************************\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,  "** Copyright (c) 2011-2012 Intel Corporation. All rights reserved. **\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "** Haswell memory detection and initialization code.               **\n");
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "** Major version number is:   %2u                                   **\n", Version->Major);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "** Minor version number is:   %2u                                   **\n", Version->Minor);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "** Rev version number is:     %2u                                   **\n", Version->Rev);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "** Build number is:           %2u                                   **\n", Version->Build);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,  "*********************************************************************\n");
#endif
  return;
}

/**
@brief
  This function return the MRC version.

  @param[out] Version - Location to store the MRC version.

  @retval Nothing.
**/
void
MrcVersionGet (
     OUT MrcVersion *const Version
  )
{
  if (Version != NULL) {
    MrcOemMemoryCpy ((U8 *) Version, (U8 *) &cVersion, sizeof (MrcVersion));
  }
}

/**
@brief
  This function set the MRC vertion to MCDECS_SPARE register.
  The function need to be call by the wrapper after MrcStartMemoryConfiguration function where the MC CLK enable.
  The function write:
  Major number to bits 16-23
  Minor number to bits 8-15
  Build number to bits 0 - 7

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
MrcStatus
MrcSetMrcVersion (
  IN     MrcParameters *const MrcData
  )
{
  MrcVersion const                      *Version;
  MCDECS_CR_MRC_REVISION_MCMAIN_STRUCT  MrcRevision;

  Version = &MrcData->SysOut.Outputs.Version;
  MrcRevision.Data = (((U32) Version->Major) << 24) |
                     (((U32) Version->Minor) << 16) |
                     (((U32) Version->Rev)   << 8)  |
                     (((U32) Version->Build));

  MrcWriteCR (MrcData, MCDECS_CR_MRC_REVISION_MCMAIN_REG, MrcRevision.Data);
  return mrcSuccess;
}

/**
@brief
  This function locks the memory controller and memory map registers.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
void
McRegistersLock (
  IN     MrcParameters *const MrcData
  )
{
  const MrcInput                  *Inputs;
  MCDECS_CR_MC_LOCK_MCMAIN_STRUCT McLock;
  MRC_PCI_000_TOM_STRUCT          Tom;
  MRC_PCI_000_TOLUD_STRUCT        Tolud;
  MRC_PCI_000_TOUUD_STRUCT        Touud;
  MRC_PCI_000_REMAPBASE_STRUCT    RemapBase;
  MRC_PCI_000_REMAPLIMIT_STRUCT   RemapLimit;
//  MRC_PCI_000_TSEGMB_STRUCT       Tsegmb;
  MRC_PCI_000_BDSM_STRUCT         Bdsm;
  MRC_PCI_000_BGSM_STRUCT         Bgsm;
  MRC_PCI_000_MESEG_MASK_STRUCT   MeSegMask;
  MRC_PCI_000_GGC_STRUCT          Ggc;
  PCU_CR_DDR_PTM_CTL_PCU_STRUCT   DdrPtmCtl;
  U32                             Offset;

  Inputs = &MrcData->SysIn.Inputs;

  //
  // Lock the memory controller registers.
  //
  McLock.Data                   = 0;
  McLock.Bits.lock_addr_map     = 1;
  McLock.Bits.lock_mc_config    = 1;
  McLock.Bits.lock_iosav_init   = 1;
  McLock.Bits.lock_pwr_mngment  = 1;
  McLock.Bits.lock_mc_dft       = 1;
  MrcWriteCR (MrcData, MCDECS_CR_MC_LOCK_MCMAIN_REG, McLock.Data);

  MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "\nMemory controller config is locked\n");

  if (Inputs->McLock) {
    //
    // Lock the memory map registers.
    // Lock TOM.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TOM_REG);
    MrcOemMmioRead (Offset, &Tom.Data32.Low.Data, Inputs->PciEBaseAddress);
    Tom.Data32.Low.Bits.Lock = 1;
    MrcOemMmioWrite (Offset, Tom.Data32.Low.Data, Inputs->PciEBaseAddress);

    //
    // Lock TOLUD.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TOLUD_REG);
    MrcOemMmioRead (Offset, &Tolud.Data, Inputs->PciEBaseAddress);
    Tolud.Bits.Lock = 1;
    MrcOemMmioWrite (Offset, Tolud.Data, Inputs->PciEBaseAddress);

    //
    // Lock TOUUD.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TOUUD_REG);
    MrcOemMmioRead (Offset, &Touud.Data32.Low.Data, Inputs->PciEBaseAddress);
    Touud.Data32.Low.Bits.Lock = 1;
    MrcOemMmioWrite (Offset, Touud.Data32.Low.Data, Inputs->PciEBaseAddress);

    //
    // Lock REMAPBASE.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_REMAPBASE_REG);
    MrcOemMmioRead (Offset, &RemapBase.Data32.Low.Data, Inputs->PciEBaseAddress);
    RemapBase.Data32.Low.Bits.Lock = 1;
    MrcOemMmioWrite (Offset, RemapBase.Data32.Low.Data, Inputs->PciEBaseAddress);

    //
    // Lock REMAPLIMIT.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_REMAPLIMIT_REG);
    MrcOemMmioRead (Offset, &RemapLimit.Data32.Low.Data, Inputs->PciEBaseAddress);
    RemapLimit.Data32.Low.Bits.Lock = 1;
    MrcOemMmioWrite (Offset, RemapLimit.Data32.Low.Data, Inputs->PciEBaseAddress);

    //
    // @todo:  - Confirm if this has been fixed and are who is locking TSEGMB
    // Lock TSEGMB.
    // Rapid Start requires TSEG_BASE access so do not lock it here.
    //
    //  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TSEGMB_REG);
    //  MrcOemMmioRead (Offset, &Tsegmb.Data, Inputs->PciEBaseAddress);
    //  Tsegmb.Bits.Lock = 1;
    //  MrcOemMmioWrite (Offset, Tsegmb.Data, Inputs->PciEBaseAddress);

    //
    // Lock DPR register
    // Rapid Start requires DPR access so do not lock it here.
    // System Agent RC SaSecurityLock() will lock it during ExitPmAuth callback
    //

    //
    // Lock BDSM.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_BDSM_REG);
    MrcOemMmioRead (Offset, &Bdsm.Data, Inputs->PciEBaseAddress);
    Bdsm.Bits.Lock = 1;
    MrcOemMmioWrite (Offset, Bdsm.Data, Inputs->PciEBaseAddress);

    //
    // Lock BGSM.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_BGSM_REG);
    MrcOemMmioRead (Offset, &Bgsm.Data, Inputs->PciEBaseAddress);
    Bgsm.Bits.Lock = 1;
    MrcOemMmioWrite (Offset, Bgsm.Data, Inputs->PciEBaseAddress);

    //
    // Lock MESEG_MASK.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_MESEG_MASK_REG);
    MrcOemMmioRead (Offset, &MeSegMask.Data32.Low.Data, Inputs->PciEBaseAddress);
    MeSegMask.Data32.Low.Bits.Lock = 1;
    MrcOemMmioWrite (Offset, MeSegMask.Data32.Low.Data, Inputs->PciEBaseAddress);

    //
    // Lock GGC.
    //
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_GGC_REG);
    MrcOemMmioRead (Offset, &Ggc.Data, Inputs->PciEBaseAddress);
    Ggc.Bits.Ggclck = 1;
    MrcOemMmioWrite (Offset, Ggc.Data, Inputs->PciEBaseAddress);

    //
    // Lock POWER THERMAL MANAGEMENT CONTROL
    //
    DdrPtmCtl.Data                   = MrcReadCR (MrcData, PCU_CR_DDR_PTM_CTL_PCU_REG);
    DdrPtmCtl.Bits.LOCK_PTM_REGS_PCU = Inputs->ThermalEnables.LockPTMregs;
    MrcWriteCR (MrcData, PCU_CR_DDR_PTM_CTL_PCU_REG, DdrPtmCtl.Data);

    MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "\nMemory map registers are locked\n");
    }

  return;
}

/**
@brief
  This function returns the recommended MRC boot mode.

  @param[in] void - No arguments

  @retval bmWarm if we are in self refresh and the DISB bit is set, otherwise returns bmCold.
**/
MrcBootMode
MrcGetBootMode (
  void
  )
{
  MrcBootMode BootMode;
  U32         Register;
  U32         ioAddress;

  ioAddress = (U32) MrcOemGetPciDeviceAddress (
                      GENERAL_PM_CONFIGURATION_2_BUS_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2_DEVICE_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2_FUNCTION_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2
                      );

  MrcOemOutPort32 (MrcOemPciIndex (), ioAddress);
  //
  // We read 32 bits but we need only 8 bits of GENERAL_PM_CONFIGURATION_2 that start at offset 0xA2 and not 0xA0.
  //
  Register = (MrcOemInPort32 (MrcOemPciData ()) >> 16);

  if ((Register & GENERAL_PM_CONFIGURATION_2_MEM_SR_MASK) == GENERAL_PM_CONFIGURATION_2_MEM_SR_MASK &&
      (Register & GENERAL_PM_CONFIGURATION_2_DISB_MASK) == GENERAL_PM_CONFIGURATION_2_DISB_MASK
      ) {
    BootMode = bmWarm;
  } else {
    BootMode = bmCold;
  }

  return BootMode;
}
//
// @todo:  - Need to find out if we need it for PCH used in HSW timeframe
//
/**
@brief
  This function sets the DISB bit in General PM Configuration 2 B:D:F 0,31,0 offset 0xA2.

  @param[in] void - No arguments

  @retval Nothing.
**/
void
MrcSetDISB (
  void
  )
{
  U32 Register;
  U32 ioAddress;

  ioAddress = (U32) MrcOemGetPciDeviceAddress (
                      GENERAL_PM_CONFIGURATION_2_BUS_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2_DEVICE_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2_FUNCTION_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2
                      );

  MrcOemOutPort32 (MrcOemPciIndex (), ioAddress);
  Register = MrcOemInPort32 (MrcOemPciData ());

  //
  // GENERAL_PM_CONFIGURATION_2 start in A2 and not in A0.
  //
  Register |= (GENERAL_PM_CONFIGURATION_2_DISB_MASK << 16);

  MrcOemOutPort32 (MrcOemPciIndex (), ioAddress);
  MrcOemOutPort32 (MrcOemPciData (), Register);
}

/**
@brief
  This function resets the DISB bit in General PM Configuration 2 B:D:F 0,31,0 offset 0xA2.

  @param[in] void - No arguments

  @retval Nothing.
**/
void
MrcResetDISB (
  void
  )
{
  U32 Register;
  U32 ioAddress;

  ioAddress = (U32) MrcOemGetPciDeviceAddress (
                      GENERAL_PM_CONFIGURATION_2_BUS_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2_DEVICE_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2_FUNCTION_ADDRESS,
                      GENERAL_PM_CONFIGURATION_2
                      );

  MrcOemOutPort32 (MrcOemPciIndex (), ioAddress);
  Register = MrcOemInPort32 (MrcOemPciData ());

  //
  // GENERAL_PM_CONFIGURATION_2 address is A2 and not A0.
  //
  Register &= ((~(GENERAL_PM_CONFIGURATION_2_DISB_MASK)) << 16);

  MrcOemOutPort32 (MrcOemPciIndex (), ioAddress);
  MrcOemOutPort32 (MrcOemPciData (), Register);
}

/**
@brief
  This function reads the CAPID0 register and sets the memory controller's capability.

  @param[in, out] MrcData - All the MRC global data.

  @retval Returns mrcSuccess if the memory controller's capability has been determined, otherwise returns mrcFail.
**/
MrcStatus
MrcMcCapability (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput            *Inputs;
  const MrcDebug            *Debug;
  MrcSaveData               *Save;
  MrcOutput                 *Outputs;
  MrcControllerOut          *ControllerOut;
  MrcChannelOut             *ChannelOut;
  MrcDimmOut                *DimmOut;
  BOOL                      EccSupport;
  BOOL                      IgnoreNonEccDimm;
  MRC_PCI_000_CAPID0_STRUCT Capid0Reg;
  MRC_PCI_000_DEVEN_STRUCT  Deven;
  MrcProfile                Profile;
  U32                       ChannelCount;
  U32                       DimmCount;
  U32                       Max;
  U32                       Size;
  U32                       ChannelNum;
  U32                       DimmNum;
  U32                       ChDimmCount;
  U32                       Offset;
  U16                       NModeMinimum;
  U8                        Controller;
  U8                        Channel;
  U8                        Dimm;

  Inputs      = &MrcData->SysIn.Inputs;
  Outputs     = &MrcData->SysOut.Outputs;
  Save        = &MrcData->SysSave.Save.Data;
  Debug       = &Inputs->Debug;
  ChDimmCount = MAX_DIMMS_IN_CHANNEL;
  Profile     = Inputs->MemoryProfile;

  //
  // Obtain the capabilities of the memory controller.
  //
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_CAPID0_REG);
  MrcOemMmioRead (Offset, &Capid0Reg.Data32.A.Data, Inputs->PciEBaseAddress);
  MrcOemMmioRead (Offset + 4, &Capid0Reg.Data32.B.Data, Inputs->PciEBaseAddress);
  Save->McCapId.Data  = Capid0Reg.Data;

  Offset              = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_DEVEN_REG);
  MrcOemMmioRead (Offset, &Deven.Data, Inputs->PciEBaseAddress);

  //
  // Determine if the internal graphics engine is supported.
  //
  if ((Capid0Reg.Data32.A.Bits.IGD == 0) && (Deven.Bits.D2EN > 0)) {
    Outputs->GraphicsStolenSize = Inputs->GraphicsStolenSize;
    Outputs->GraphicsGttSize    = Inputs->GraphicsGttSize;
  } else {
    Outputs->GraphicsStolenSize = 0;
    Outputs->GraphicsGttSize    = 0;
  }

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Memory allocated for IGD = %uMB and for GTT = %uMB.\n",
    Outputs->GraphicsStolenSize,
    Outputs->GraphicsGttSize
    );

  //
  // Determine the maximum size of memory per channel, based on fuses.
  //
  switch (Capid0Reg.Data32.A.Bits.DDRSZ) {
  case tcs16GB:
    Outputs->MrcTotalChannelLimit = (16 * 1024);
    break;

  case tcs8GB:
    Outputs->MrcTotalChannelLimit = (8 * 1024);
    break;

  case tcs2GB:
    Outputs->MrcTotalChannelLimit = (2 * 1024);
    break;

  case tcs512MB:
  default:
    Outputs->MrcTotalChannelLimit = (512);
    break;
  }

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Maximum size of memory allowed on a channel = %uMB.\n",
    Outputs->MrcTotalChannelLimit
    );

  //
  // Determine how many channels are supported on this memory controller,
  // based on fuse and how many channels have DIMMs installed.
  //
  ChannelCount  = (Capid0Reg.Data32.A.Bits.PDCD == 0) ? MAX_CHANNEL : 1;
  DimmCount     = (Capid0Reg.Data32.A.Bits.DDPCD == 0) ? MAX_DIMMS_IN_CHANNEL : 1;

#ifdef ULT_FLAG
  if (Inputs->CpuModel == cmHSW_ULT) {
    //
    // Only 1DPC is supported on ULT platform
    //
    DimmCount = 1;
  }
#endif // ULT_FLAG

#ifdef EMBEDDED_FLAG
  if (Inputs->BoardType == btCRBEMB) {
    //
    // Only 1DPC is supported on EMBEDDED platform
    //
    DimmCount = 1;
  }
#endif

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "Number of channels supported = %u\nNumber of DIMMs per channel supported = %u\n",
    ChannelCount,
    DimmCount
    );

  //
  // Determine the minimum NMode supported on this memory controller.
  //
  NModeMinimum = (Capid0Reg.Data32.A.Bits.D1NM == 0) ? 1 : 2;

  //
  // Determine the ECC capability of the memory controller.
  //
  IgnoreNonEccDimm = (Capid0Reg.Data32.A.Bits.FDEE == 0) ? FALSE : TRUE;

  //
  // Set EccSupport flag to TRUE if we must NOT ignore ECC DIMMs
  //
  if (IgnoreNonEccDimm == TRUE) {
    Outputs->EccSupport = TRUE;
    EccSupport = TRUE; // FDEE has presedence over ECCDIS
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ALL DIMMs MUST be ECC capable\n");
  } else {
    EccSupport = ((Capid0Reg.Data32.A.Bits.ECCDIS > 0) || (Outputs->EccSupport == FALSE)) ? FALSE : TRUE;
  }
  //
  // Now copy ECC and NMode information to the channel and DIMM results.
  //
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerOut = &Outputs->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &ControllerOut->Channel[Channel];
      if (ChannelOut->Status == CHANNEL_PRESENT) {
        if ((NModeMinimum >= 2) ||
          ((Inputs->MemoryProfile == STD_PROFILE) &&
          ((Outputs->Frequency > f1867) || ((ChannelOut->DimmCount >= 2) && (Outputs->Frequency >= f1333))))) {
          ChannelOut->Timing[Profile].NMode = MAX (2, ChannelOut->Timing[Profile].NMode);
        }
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DimmOut->Status == DIMM_PRESENT) {
            DimmOut->Timing[Profile].NMode = ChannelOut->Timing[Profile].NMode;
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "  %s %u/%u/%u NMode = %u\n",
              CcdString,
              Controller,
              Channel,
              Dimm,
              DimmOut->Timing[Profile].NMode
            );
            if (EccSupport == TRUE) {
              if ((DimmOut->EccSupport == FALSE) && (IgnoreNonEccDimm == TRUE)) {
                DimmOut->Status = DIMM_DISABLED;
                MRC_DEBUG_MSG (
                  Debug,
                  MSG_LEVEL_NOTE,
                  "  %s %u/%u/%u Disabling non-ECC capable DIMM\n",
                  CcdString,
                  Controller,
                  Channel,
                  Dimm
                  );
              } else if (DimmOut->EccSupport == TRUE) {
                DimmOut->EccSupport = TRUE;
                DimmOut->SdramCount = MAX_SDRAM_IN_DIMM;
              } else {
                DimmOut->SdramCount = MAX_SDRAM_IN_DIMM - 1;
                Outputs->EccSupport = FALSE; // Final ECCSupport must be disabled if one DIMM is NOT capable
              }
            } else {
              DimmOut->EccSupport = FALSE;
              DimmOut->SdramCount = MAX_SDRAM_IN_DIMM - 1;
              Outputs->EccSupport = FALSE; // Final ECCSupport must be disabled if ECCDIS is set
            }
          }
        }
      }
    }
  }

  //
  // Update FInal SdramCount
  //
  Outputs->SdramCount = (Outputs->EccSupport == TRUE) ? MAX_SDRAM_IN_DIMM : (MAX_SDRAM_IN_DIMM - 1);

  //
  // Determine the size of memory in each channel.
  // Also determine the channel with the largest amount.
  //
  Max = ChannelNum = Outputs->MemoryMapData.TotalPhysicalMemorySize = 0;
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerOut = &Outputs->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut = &ControllerOut->Channel[Channel];
      Size        = 0;
      if (ChannelOut->Status == CHANNEL_PRESENT) {
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DimmOut->Status == DIMM_PRESENT) {
            Size += DimmOut->DimmCapacity;
          }
        }

        ChannelOut->Capacity = Size;
        if (Size > Max) {
          Max         = Size;
          ChannelNum  = Channel;
          ChDimmCount = ChannelOut->DimmCount;
        } else if ((Size == Max) && (DimmCount == 1)) {
          //
          // Choose channel with least amount of DIMMs if 2DPC is disabled
          //
          if (ChannelOut->DimmCount < ChDimmCount) {
            ChDimmCount = ChannelOut->DimmCount;
            ChannelNum  = Channel;
          }
        }
      }

      Outputs->MemoryMapData.TotalPhysicalMemorySize += ChannelOut->Capacity;
    }
  }

  if (ChannelCount == 1) {
    //
    // Determine which channels are supported on this memory controller.
    // If fused for one channel, we pick the channel with the most memory.
    //
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        if ((ChannelOut->Status == CHANNEL_PRESENT) && (Channel != ChannelNum)) {
          //
          // Disable Channel don't skip DIMM capacity
          //
          MrcChannelDisable (MrcData, (U8) Channel, 0);
        }
      }

      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "Controller configured to one channel, we've selected channel %u.\n",
        ChannelNum
        );
    }
  }

  if (DimmCount == 1) {
    //
    // Determine which DIMMs are supported on this memory controller.
    // If fused for one DIMM per channel, we pick the DIMM in a channel with the most memory.
    //
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelOut = &ControllerOut->Channel[Channel];
        Max                   = Size = DimmNum = 0;
        if (ChannelOut->Status == CHANNEL_PRESENT) {
          for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
            DimmOut = &ChannelOut->Dimm[Dimm];
            if (DimmOut->Status == DIMM_PRESENT) {
              Size = DimmOut->DimmCapacity;
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%uD%uDimmCapacity = 0x%x\n", Channel, Dimm, DimmOut->DimmCapacity);
              if (Size > Max) {
                Max     = Size;
                DimmNum = Dimm;
              }
            }
          }

          for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
            DimmOut = &ChannelOut->Dimm[Dimm];
            if ((DimmOut->Status == DIMM_PRESENT) && (Dimm != DimmNum)) {
              DimmOut->Status = DIMM_DISABLED;
            }
          }

          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "Controller configured to one DIMM per channel, we've selected channel %u, Dimm %u.\n",
            Channel,
            DimmNum
            );
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ValidRankBitMask = 0x%x\n", ChannelOut->ValidRankBitMask);
        }
      }
    }
  }

  //
  // Now that we know the enabled and disabled DIMM/Channel population,
  // determine if all enabled DIMMS support ASR.
  //
  // It is necessary to have all DIMMS in ASR or no DIMMS in ASR
  // when enabling 2x Refresh.
  //
  if (Inputs->RefreshRate2x == TRUE) {
    Outputs->AutoSelfRefresh = TRUE;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerOut = &Outputs->Controller[Controller];
      if (ControllerOut->Status == CONTROLLER_PRESENT) {
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          ChannelOut = &ControllerOut->Channel[Channel];
          if (ChannelOut->Status == CHANNEL_PRESENT) {
            for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
              DimmOut = &ChannelOut->Dimm[Dimm];
              if ((DimmOut->Status == DIMM_PRESENT) && (DimmOut->AutoSelfRefresh == FALSE)) {
                MRC_DEBUG_MSG (
                  Debug,
                  MSG_LEVEL_NOTE,
                  "Channel %d, Dimm %d does not support Auto Self Refresh.  Disabling ASR since 2x Refresh is enabled!\n",
                  Channel,
                  Dimm
                  );
                Outputs->AutoSelfRefresh = FALSE;
              }
            }
          }
        }
      }
    }
  }

  return mrcSuccess;
}

/**
@brief
  This function reads the CAPID0 register and sets the memory controller's capability.

  @param[in, out] MrcData - All the MRC global data.

  @retval Returns mrcSuccess if the memory controller's capability has been determined, otherwise returns mrcFail.
**/
MrcStatus
MrcMcCapabilityPreSpd (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcInput            *Inputs;
  const MrcDebug            *Debug;
  MrcOutput                 *Outputs;
  MrcFrequency              FreqMax;
  MrcFrequency              FreqMax100;
  MrcFrequency              FreqMax133;
  MrcRefClkSelect           RefClk;
  BOOL                      Capable;
  MRC_PCI_000_CAPID0_STRUCT Capid0Reg;
  U32                       Capable100;
  U32                       Capable133;
  U32                       Offset;

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  //
  // Obtain the capabilities of the memory controller.
  //
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_CAPID0_REG);
  MrcOemMmioRead (Offset, &Capid0Reg.Data32.A.Data, Inputs->PciEBaseAddress);
  MrcOemMmioRead (Offset + 4, &Capid0Reg.Data32.B.Data, Inputs->PciEBaseAddress);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "CAPID0 = %X_%Xh\n", Capid0Reg.Data32.B.Data, Capid0Reg.Data32.A.Data);

  //
  // Determine the maximum memory frequency supported and the memory reference clock.
  //
  Capable          = (Capid0Reg.Data32.A.Bits.DDR_OVERCLOCK > 0) ? TRUE : FALSE;
  Capable100       = Capid0Reg.Data32.B.Bits.PLL_REF100_CFG;
  Capable133       = Capid0Reg.Data32.B.Bits.DMFC;
  Outputs->RefClk  = Inputs->RefClk;
  Outputs->FreqMax = ((Inputs->FreqMax > fNoInit) && (Inputs->FreqMax < fUnSupport)) ? Inputs->FreqMax : f2667;

  if (Capable100 == 0) {
    Outputs->RefClk = MRC_REF_CLOCK_133;
  }

  RefClk = Outputs->RefClk;
  if (Capable) {
    Capable133 = 0;
    if (Capable100 > 0) {
      Capable100 = CAPID0_B_PLL_REF100_CFG_MAX;
      Outputs->Capable100 = TRUE;
    }
  }

  FreqMax100  = (Capable100 == 0) ? fNoInit : MrcRatioToFrequency (MrcData, (MrcClockRatio) Capable100 + 6, MRC_REF_CLOCK_100, BCLK_DEFAULT);
  FreqMax133  = MrcRatioToFrequency (MrcData, (MrcClockRatio) ((Capable133 == 0) ? 10 : 11 - Capable133), MRC_REF_CLOCK_133, BCLK_DEFAULT);
  //
  // If overclocking is supported, then there is no frequency limitation, otherwise check for limitation.
  // Note 1: If we are using standard memory profile, DIMMS should run at RefClk 133.
  // Note 2: If the 2 values are equal, then we want to pick RefClk 133.
  //

  if (Inputs->MemoryProfile == STD_PROFILE) {
    FreqMax = FreqMax133;
    RefClk  = MRC_REF_CLOCK_133;
  } else {    
    if (Capable) {
      FreqMax = (RefClk == MRC_REF_CLOCK_100) ? FreqMax100 : FreqMax133;
    } else if (FreqMax100 > FreqMax133) {
      FreqMax = FreqMax100;
      RefClk  = MRC_REF_CLOCK_100;
    } else {
      FreqMax = FreqMax133;
      RefClk  = MRC_REF_CLOCK_133;
    }
  }

  if (FreqMax < Outputs->FreqMax) {
    Outputs->FreqMax  = FreqMax;
    Outputs->RefClk   = RefClk;
  }

  Outputs->MemoryClockMax = ConvertFreq2Clock (MrcData, Outputs->FreqMax, NULL);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "The maximum memory frequency allowed is %u (%ufs)\n", Outputs->FreqMax, Outputs->MemoryClockMax);
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "%uMHz reference clock is selected\n",
    (Outputs->RefClk == MRC_REF_CLOCK_133) ? 133 : 100
    );

  return mrcSuccess;
}

/**
@brief
  This function sets the appropriate timing overrides in the output structure
  prior to Spd processing.

  @param[in, out] MrcData - All the MRC global data.

  @retval Returns mrcSuccess if the timing overrides have been conpleted.
**/
MrcStatus
MrcSetOverridesPreSpd (
  IN OUT MrcParameters *const MrcData
  )
{
  return mrcSuccess;
}

/**
@brief
  This function reads the input data structure and sets the appropriate timing overrides in the output structure.

  @param[in, out] MrcData - All the MRC global data.

  @retval Returns mrcSuccess if the timing overrides have been conpleted.
**/
MrcStatus
MrcSetOverrides (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcDebug                      *Debug;
  const MrcInput                      *Inputs;
  MrcOutput                           *Outputs;
  MPCOHTRK_CR_GDXC_MOT_REGION_STRUCT  GdxcMotRegion;
  MPCOHTRK_CR_GDXC_OCLA_REGION_STRUCT GdxcOclaRegion;

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  Outputs->EccSupport       = Inputs->EccSupport;
  Outputs->VddVoltageDone   = FALSE;

  Outputs->Gdxc.GdxcEnable  = Inputs->Gdxc.GdxcEnable;

  //
  // Read MOT register
  //
  MrcOemMmioRead (MPCOHTRK_CR_GDXC_MOT_REGION_REG, &GdxcMotRegion.Data, Inputs->GdxcBaseAddress);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "----- GDXC MOT LOW : 0x%x\n", GdxcMotRegion.Bits.START_ADDRESS);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "----- GDXC MOT UPP : 0x%x\n", GdxcMotRegion.Bits.END_ADDRESS);
  if (GdxcMotRegion.Bits.START_ADDRESS == 0  && GdxcMotRegion.Bits.END_ADDRESS > 1) {
    Outputs->Gdxc.GdxcMotSize = (U8) (GdxcMotRegion.Bits.END_ADDRESS);
  } else {
    Outputs->Gdxc.GdxcMotSize = Inputs->Gdxc.GdxcMotSize;
  }
  //
  // Read OCLA register
  //
  MrcOemMmioRead (MPCOHTRK_CR_GDXC_OCLA_REGION_REG, &GdxcOclaRegion.Data, Inputs->GdxcBaseAddress);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "----- GDXC IOT LOW : 0x%x\n", GdxcOclaRegion.Bits.START_ADDRESS);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "----- GDXC IOT UPP : 0x%x\n", GdxcOclaRegion.Bits.END_ADDRESS);

  if (GdxcOclaRegion.Bits.START_ADDRESS == 0  && GdxcOclaRegion.Bits.END_ADDRESS > 1) {
    Outputs->Gdxc.GdxcIotSize = (U8) (GdxcOclaRegion.Bits.END_ADDRESS);
  } else {
    Outputs->Gdxc.GdxcIotSize = Inputs->Gdxc.GdxcIotSize;
  }

  return mrcSuccess;
}

/**
@brief
  This function set the WM0-5 values. Those values are be using by the graphics driver.
  need to be call after PU_MRC_DONE bit is set to 1.

  @param[in] MrcData - include all the MRC data.

  @retval Nothing.

  **/
void
MrcWmRegSet (
  IN     MrcParameters *const MrcData
  )
{
  M_PCU_CR_SSKPD_PCU_STRUCT CrSskpdPcu;

  CrSskpdPcu.Data        = 0;
  CrSskpdPcu.Bits.NewWM0 = PCU_CR_SSKPD_PCU_NEW_WM0_DEF;
  CrSskpdPcu.Bits.WM4    = PCU_CR_SSKPD_PCU_WM4_DEF;
  CrSskpdPcu.Bits.WM3    = PCU_CR_SSKPD_PCU_WM3_DEF;
  CrSskpdPcu.Bits.WM2    = PCU_CR_SSKPD_PCU_WM2_DEF;
  CrSskpdPcu.Bits.WM1    = PCU_CR_SSKPD_PCU_WM1_DEF;
  CrSskpdPcu.Bits.OldWM0 = PCU_CR_SSKPD_PCU_OLD_WM0_DEF;
  MrcWriteCR64 (MrcData, PCU_CR_SSKPD_PCU_REG, CrSskpdPcu.Data);
  return;
}


#ifdef ULT_FLAG
/**
@brief
  Program DDRPL_CR_DDR_TX_DELAY if Memory Trace is enabled

  @param[in] MrcData - The MRC general data.

  @retval None
**/
void
MrcSetDdrplTxDelay (
  IN MrcParameters *const MrcData
  )
{
  MrcInput                              *Inputs;
  MrcOutput                             *Outputs;
  MrcChannelOut                         *ChannelOut;
  U32                                   Rank;
  U32                                   TxDelay;
  U32                                   Roundtrip;
  U32                                   tCL;
  U32                                   tWCL;
  U32                                   CmdDelay;
  U32                                   CmdStretch;
  U32                                   DecWrd;
  U32                                   AddWrDelay;
  U32                                   tWCL5_reduction;
  U32                                   StretchMode;
  DDRPL_CR_DDR_TX_DELAY_STRUCT          DdrTxDelay;
  MCHBAR_CH0_CR_SC_ROUNDT_LAT_STRUCT    ScRoundtLat;
  MCHBAR_CH0_CR_SC_WR_ADD_DELAY_STRUCT  ScWrAddDelay;
  MCHBAR_CH0_CR_TC_BANK_RANK_A_STRUCT   TcBankRankA;
  MCHBAR_CH0_CR_TC_BANK_RANK_B_STRUCT   TcBankRankB;
  MCHBAR_CH0_CR_TC_BANK_RANK_D_STRUCT   TcBankRankD;
  MCSCHEDS_CR_STM_CONFIG_STRUCT         StmConfig;
    
  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;

  if (!Inputs->MemoryTrace) {
    return;
  }

  //
  // TxDelay(rank) = Roundtrip(rank) - [2*RD_cmd2data_dclk_delay] + [2*Dec_WRD] - additional_wr_delay(rank) - [2*tCWL5_reduction] + [8*(STM - 1)] + Constant(5)
  //
  // RD_cmd2data_dclk_delay = tCL + tDQSCK + cmd_delay + cmd_stretch
  //                        tCL, tDQSCK      - according to JEDEC spec
  //                        cmd_delay        - MCSCHEDS_CR_TC_BANK_RANK_D. cmd_delay
  //                        cmd_stretch      - MCSCHEDS_CR_TC_BANK_RANK_A. cmd_stretch (0,1,2 for 1N,2N,3N respectively)
  //
  // tCWL5_reduction = (ddr_type==DDR3 && (tCWL + cmd_stretch - Dec_WRD == 5)) ? 1 : 0;
  //
  // STM = (STM_mode == SYSTEM ? STM_stf : 1)

  //
  // Assume we are tracing DDR channel 0 - taking all the timing parameters from Channel 0
  //
  
  ChannelOut = &Outputs->Controller[0].Channel[0];
  
  ScRoundtLat.Data  = MrcReadCR (MrcData, MCHBAR_CH0_CR_SC_ROUNDT_LAT_REG);
  ScWrAddDelay.Data = MrcReadCR (MrcData, MCHBAR_CH0_CR_SC_WR_ADD_DELAY_REG);
  StmConfig.Data    = MrcReadCR (MrcData, MCSCHEDS_CR_STM_CONFIG_REG);
  TcBankRankA.Data  = ChannelOut->MchbarBANKRANKA;
  TcBankRankB.Data  = ChannelOut->MchbarBANKRANKB;
  TcBankRankD.Data  = ChannelOut->MchbarBANKRANKD;
  tCL         = TcBankRankD.UltBits.tCL;
  tWCL        = TcBankRankD.UltBits.tWCL;
  CmdDelay    = TcBankRankD.UltBits.cmd_delay;
  CmdStretch  = TcBankRankA.Bits.CMD_stretch;
  DecWrd      = TcBankRankB.Bits.Dec_WRD;
  
  if ((Outputs->DdrType == MRC_DDR_TYPE_DDR3) && (tWCL + CmdStretch - DecWrd == 5)) {
    tWCL5_reduction = 1;
  } else {
    tWCL5_reduction = 0;
  }
  
  if (StmConfig.Bits.Stretch_mode == 2) {
    StretchMode = StmConfig.Bits.STF;
  } else {
    StretchMode = 1;
  }
  
  DdrTxDelay.Data = 0;
  MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "TX Delay values for Memory Trace:\n");
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    if (!MrcRankInChannelExist (MrcData, (U8) Rank, 0)) {
      continue;
    }
  
    Roundtrip  = (ScRoundtLat.Data >> (Rank * 8)) & MCHBAR_CH0_CR_SC_ROUNDT_LAT_Lat_R0D0_MSK;
    AddWrDelay = (ScWrAddDelay.Data >> (Rank * 2)) & MCHBAR_CH0_CR_SC_WR_ADD_DELAY_D0R0_MSK;
    
    TxDelay = Roundtrip - (2 * tCL + 1 + 2 * CmdDelay + 2 * CmdStretch) + 
      2 * DecWrd - 2 * AddWrDelay - 2 * tWCL5_reduction + 8 * (StretchMode - 1) + 5;
    
    DdrTxDelay.Data |= ((TxDelay & DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R0_MSK) << (Rank * DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R0_WID));
    
    MRC_DEBUG_MSG (
      &Inputs->Debug,
      MSG_LEVEL_NOTE,
      "Rank%u:\n RT = %u\n tCL = %u\n cmd_delay = %u\n CMD_stretch = %u\n Dec_WRD = %u\n AddWrDelay = %u\n tWCL5_reduction = %u\n STM = %u\n",
      Rank,
      Roundtrip,
      tCL,
      CmdDelay,
      CmdStretch,
      DecWrd,
      AddWrDelay,
      tWCL5_reduction,
      StretchMode
      );
  }

  MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "DDRPL_CR_DDR_TX_DELAY = 0x%08X\n", DdrTxDelay.Data);

  MrcOemMmioWrite (DDRPL_CR_DDR_TX_DELAY_REG, DdrTxDelay.Data, Inputs->GdxcBaseAddress);
}
#endif // ULT_FLAG
