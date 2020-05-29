/** @file
  The functions in this file implement the DDR3 reset sequence.

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

**/

//
// Include files
//
#include <MrcReset.h>

/**
@brief
  Perform full JEDEC reset and init sequence.

  @param[in] MrcData - Include all MRC global data.

  @retval MrcStatus - mrcSuccess or reason for failure.
**/
MrcStatus
MrcResetSequence (
  IN MrcParameters *const MrcData
  )
{
  MrcStatus Status;
  MrcOutput *Outputs;
#ifdef ULT_FLAG
  BOOL      Lpddr;
#endif //ULT_FLAG

  Outputs = &MrcData->SysOut.Outputs;

  //
  // Start with an IO reset
  //
  Status = IoReset (MrcData);
  if (mrcSuccess == Status) {
    //
    // Check if rcomp is done and the ddr ready to use
    //
    Status = CheckFirstRcompDone (MrcData);
    if (mrcSuccess == Status) {
      //
      // Perform jedec reset.
      //
      // If RTT_NOM is to be enabled in MR1, the ODT input signal must be statically held LOW
      // in our system RTT_NOM is always enable.
      // Force ODT low
      //
      MrcWriteCR (MrcData, MCSCHEDS_CR_REUT_CH_MISC_ODT_CTRL_REG, MCSCHEDS_CR_REUT_CH_MISC_ODT_CTRL_ODT_Override_MAX);

#ifdef ULT_FLAG
      //
      // Check if LPDDR3 memory is used
      //
      Lpddr = (MrcData->SysOut.Outputs.DdrType == MRC_DDR_TYPE_LPDDR3);

      if (Lpddr) {
        Status = MrcJedecResetLpddr3 (MrcData);
        //
        // The rest of JEDEC init will be done in a separate step after Early Command Training,
        // and ECT will set the LpddrEctDone flag.
        //
        if (Outputs->LpddrEctDone) {
          Status = MrcJedecInitLpddr3 (MrcData);
        }
        return Status;
      }
#endif // ULT_FLAG
      MrcJedecReset (MrcData);

      //
      // Initialize the DIMM MRS registers.
      //
      //
      // Step 6 - Set the MR2 for each rank
      //
      Status = MrcSetMR2 (MrcData, 0);
      if (Status == mrcSuccess) {
        //
        // Step 7 - Set the MR3 for each rank
        //
        Status = MrcSetMR3 (MrcData, 0, 0);
        if (Status == mrcSuccess) {
          //
          // Step 8 - Set the MR1 for each rank
          //
          Status = MrcSetMR1 (MrcData, 0, DIMMRON, 0, 0, 0, 0);
          if (Status == mrcSuccess) {
            //
            // Step 9 - Set the MR0 for each rank
            //
            Status = MrcSetMR0 (MrcData, 0, 0, 0, 1);
            if (Status == mrcSuccess) {
              //
              // Step 10 - Issue ZQCL command to start ZQ calibration
              //
              Status = MrcIssueZQ (MrcData, 0x3, MRC_ZQ_INIT);
              if (Status == mrcSuccess) {
                //
                // If RTT_NOM is to be enabled in MR1, the ODT input signal must be statically held LOW
                // in our system RTT_NOM is always enable.
                // Force ODT low
                //
                MrcWriteCR (MrcData, MCSCHEDS_CR_REUT_CH_MISC_ODT_CTRL_REG, 0);
              }
            }
          }
        }
      }
    }
  }
  //
  // Set flag to restore from host structure instead from look-up table
  //
  Outputs->RestoreMRs = TRUE;

  return Status;
}

/**
@brief
  Perform JEDEC DIMM reset sequence.

  @param[in] MrcData - Include all MRC global data.

  @retval - none
**/
void
MrcJedecReset (
  IN MrcParameters *const MrcData
  )
{
  MrcInput                                *Inputs;
  MrcOutput                               *Outputs;
  MrcControllerOut                        *ControllerOut;
  U32                                     Offset;
  U32                                     VddSettleWaitTime;
  U8                                      Channel;
  MCDECS_CR_MC_INIT_STATE_G_MCMAIN_STRUCT McInitStateG;
#ifdef ULT_FLAG
  U32                                     Rcba;
  U32                                     PmCfg2;
#endif // ULT_FLAG

  Inputs            = &MrcData->SysIn.Inputs;
  Outputs           = &MrcData->SysOut.Outputs;
  ControllerOut     = &Outputs->Controller[0];
  VddSettleWaitTime = 200; // 200us is the minimum w/o the delay needed to allow for DDR3L Change

#ifdef ULT_FLAG
  //
  // Get the PCH RCBA from 0:1F:0:F0, and clear the Enable bit
  //
  Offset = MrcOemGetPcieDeviceAddress (0, 0x1F, 0, 0xF0);
  MrcOemMmioRead (Offset, &Rcba, Inputs->PciEBaseAddress);
  Rcba &= (~1);

  if (Inputs->CpuModel == cmHSW_ULT) {
    //
    // Deassert DRAM RESET# via PCH regsiter on ULT
    //
    MrcOemMmioRead (R_PCH_RCRB_PM_CFG2, &PmCfg2, Rcba);
    PmCfg2 |= B_PCH_RCRB_PM_CFG2_DRAM_RESET_CTL;
    MrcOemMmioWrite (R_PCH_RCRB_PM_CFG2, PmCfg2, Rcba);
  }
#endif // ULT_FLAG

  McInitStateG.Data                     = 0;
  McInitStateG.Bits.pu_mrc_done         = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_pu_mrc_done_DEF;
  McInitStateG.Bits.ddr_reset           = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_ddr_reset_DEF;
  McInitStateG.Bits.refresh_enable      = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_refresh_enable_DEF;
  McInitStateG.Bits.mc_init_done_ack    = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_mc_init_done_ack_DEF;
  McInitStateG.Bits.mrc_done            = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_mrc_done_DEF;
  McInitStateG.Bits.safe_sr             = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_safe_sr_DEF;
  McInitStateG.Bits.HVM_Gate_DDR_Reset  = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_HVM_Gate_DDR_Reset_DEF;
  McInitStateG.Bits.dclk_enable         = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_dclk_enable_DEF;
  McInitStateG.Bits.reset_io            = MCDECS_CR_MC_INIT_STATE_G_MCMAIN_reset_io_DEF;

  MrcWriteCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG, McInitStateG.Data);

  //
  // Force CKE low
  //
  MrcWriteCR (MrcData, MCSCHEDS_CR_REUT_CH_MISC_CKE_CTRL_REG, MCSCHEDS_CR_REUT_CH_MISC_CKE_CTRL_CKE_Override_MAX);

  //
  // Assert DIMM reset signal - step 1
  //
#ifdef ULT_FLAG
  if (Inputs->CpuModel == cmHSW_ULT) {
    PmCfg2 &= ~(B_PCH_RCRB_PM_CFG2_DRAM_RESET_CTL);
    MrcOemMmioWrite (R_PCH_RCRB_PM_CFG2, PmCfg2, Rcba);
  } else
#endif // ULT_FLAG
  {
    McInitStateG.Data = MrcReadCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG);  // Read 'MC_Init_State_G' register.
    McInitStateG.Bits.ddr_reset = 0;
    MrcWriteCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG, McInitStateG.Data);  // Assert DIMM reset
  }

  //
  // Check and Switch DDR3 voltage
  //
  if ((Outputs->VddVoltage[Inputs->MemoryProfile] != VDD_INVALID) && (Outputs->VddVoltageDone == FALSE)) {
    MrcOemVDDVoltageCheckAndSwitch (MrcData, Outputs->VddVoltage[Inputs->MemoryProfile], &VddSettleWaitTime);
  }
  //
  // delay 200 micro sec as jedec ask
  //
  MrcWait (MrcData, VddSettleWaitTime * HPET_1US);

  //
  // De-asserted DIMM reset signal - step 2
  //
#ifdef ULT_FLAG
  if (Inputs->CpuModel == cmHSW_ULT) {
    PmCfg2 |= B_PCH_RCRB_PM_CFG2_DRAM_RESET_CTL;
    MrcOemMmioWrite (R_PCH_RCRB_PM_CFG2, PmCfg2, Rcba);
  } else
#endif // ULT_FLAG
  {
    McInitStateG.Bits.ddr_reset = 1;
    MrcWriteCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG, McInitStateG.Data);  // De-assert DIMM reset
  }

  //
  // delay 500 micro sec as jedec ask
  //
  MrcWait (MrcData, 500 * HPET_1US);

  //
  // Enable the DCLK - step 3
  //
  McInitStateG.Bits.dclk_enable = 1;
  MrcWriteCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG, McInitStateG.Data);

  //
  // wait the 20 nano sec tCKSRX .
  //
  MrcWait (MrcData, 1 * HPET_MIN);  // Minimum is 69.84 ns

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {

      //
      // Set the Valid CKE - step 4
      //
      Offset = 2 + MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_MISC_CKE_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG) * Channel);
      MrcWriteCR8 (MrcData, Offset, ControllerOut->Channel[Channel].ValidRankBitMask);
    }
  }
  //
  // wait minimum of Reset CKE Exit time, tXPR - Step 5
  //
  // Spec says max (tXS, 5 tCK). 5 tCK is 10 nsec and minimum using HPET is 69.64ns
  //
  MrcWait (MrcData, 1 * HPET_MIN); // Minimum is 69.84 ns

  return;
}

#ifdef ULT_FLAG
/**
@brief
  Perform JEDEC LPDDR3 DIMM reset sequence.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess if succeeded
**/
MrcStatus
MrcJedecResetLpddr3 (
  IN MrcParameters *const MrcData
  )
{
  MrcStatus                               Status;
  MrcOutput                               *Outputs;
  MrcControllerOut                        *ControllerOut;
  U32                                     Offset;
  U8                                      Channel;
  U8                                      Rank;
  MCDECS_CR_MC_INIT_STATE_G_MCMAIN_STRUCT McInitStateG;

  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];

  //
  // Force CKE low
  //
  MrcWriteCR (MrcData, MCSCHEDS_CR_REUT_CH_MISC_CKE_CTRL_REG, MCSCHEDS_CR_REUT_CH_MISC_CKE_CTRL_CKE_Override_MAX);

  //
  // Wait till voltages are stable
  //
  // @todo
  //  if ((Outputs->VddVoltage[Inputs->MemoryProfile] != VDD_INVALID) && (Outputs->VddVoltageDone == FALSE)) {
  //    MrcOemVDDVoltageCheckAndSwitch (MrcData, Outputs->VddVoltage[Inputs->MemoryProfile], VddSettleWaitTime);
  //  }
  //
  // Enable the DCLK
  //
  McInitStateG.Data             = MrcReadCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG);
  McInitStateG.Bits.dclk_enable = 1;
  MrcWriteCR (MrcData, MCDECS_CR_MC_INIT_STATE_G_MCMAIN_REG, McInitStateG.Data);

  //
  // Wait 20ns before CKE goes high
  //
  MrcWait (MrcData, 1 * HPET_MIN);  // Minimum is 69.84 ns

  //
  // Force CKE high on populated ranks
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {

      //
      // Set the Valid CKE
      //
      Offset = 2 + MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_MISC_CKE_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG) * Channel);
      MrcWriteCR8 (MrcData, Offset, ControllerOut->Channel[Channel].ValidCkeBitMask);
    }
  }
  //
  // Delay 200 micro sec per JEDEC requirement
  // tINIT3 - minimum idle time after first CKE assertion
  //
  MrcWait (MrcData, 200 * HPET_1US);

  //
  // Send the RESET MRW command to populated ranks
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          if (Outputs->LpddrEctDone) {
            //
            // Issue a PRECHARGE ALL command to put all banks to idle state.
            // MRW can only be issued when all banks are idle.
            //
            MrcIssuePrechargeAll (MrcData, Channel, 1 << Rank);
            MrcWait (MrcData, 1 * HPET_1US);
          }

          Status = MrcIssueMrw (
                    MrcData,
                    Channel,
                    Rank,
                    0x3F, // Address = 63
                    0xFC, // Data is selected so that High and Low phases of CA[9:0] are equal
                    TRUE, // InitMrw
                    TRUE  // ChipSelect2N
                    );
          if (Status != mrcSuccess) {
            return Status;
          }
        }
      }
    }
  }
  //
  // tINIT5 - Maximum duration of device auto initialization = 10 us
  //
  MrcWait (MrcData, 10 * HPET_1US);

  return mrcSuccess;
}

typedef struct _MRC_LPDDR_MR_DATA {
  U8 Address;
  U8 Data;
} MRC_LPDDR_MR_DATA;

/**
@brief
  Perform JEDEC LPDDR3 DIMM init sequence.
  1. ZQ Calibration
  2. Program MR2, MR1, MR3, MR11

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess if succeeded
**/
MrcStatus
MrcJedecInitLpddr3 (
  IN MrcParameters *const MrcData
  )
{
  MrcStatus         Status;
  MrcInput          *Inputs;
  MrcOutput         *Outputs;
  MrcDebug          *Debug;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  U8                Channel;
  U8                Rank;
  U32               MrIndex;
  U8                MrData;
  U32               Index;
  MRC_LPDDR_MR_DATA MrTable[] = {
    {  2, 0x40 }, // MR2:  nWRE = 1, RL & WL depend on frequency
    {  1, 0x43 }, // MR1:  BL = BL8, nWR = 12
    {  3, 0x01 }, // MR3:  DS = 34.3 Ohm
    { 11, 0x00 }  // MR11: ODT
  };

  Outputs       = &MrcData->SysOut.Outputs;
  Inputs        = &MrcData->SysIn.Inputs;
  Debug         = &Inputs->Debug;
  ControllerOut = &Outputs->Controller[0];

  //
  // Issue ZQ Init calibration on all channels / ranks
  //
  Status = MrcIssueZQ (MrcData, 0x3, MRC_ZQ_INIT);
  if (Status != mrcSuccess) {
    return Status;
  }

  if (Outputs->Frequency <= f800) {
      MrData = 0x14;
  } else if (Outputs->Frequency <= f1067) {
      MrData = 0x16;
  } else if (Outputs->Frequency <= f1200) {
      MrData = 0x17;
  } else if (Outputs->Frequency <= f1333) {
      MrData = 0x18;
  } else if (Outputs->Frequency <= f1600) {
      MrData = 0x1A;
  } else {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "MrcJedecInitLpddr3: Invalid LPDDR frequency!\n");
      return mrcFrequencyError;
  }

  MrTable[0].Data = MrData;  // MR2

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!MrcChannelExist (Outputs, Channel)) {
      continue;
    }

    ChannelOut = &ControllerOut->Channel[Channel];
    for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
      if (MrcRankInChannelExist (MrcData, Rank, Channel) == 0) {
        continue;
      }

      if ((Rank == 0) && Inputs->LpddrDramOdt) {
        //
        // Enable PD Control on Rank 0 only if we have 2 ranks and ODT is used
        //
        if (MrcRankInChannelExist (MrcData, 1, Channel)) {
          MrTable[3].Data = 0x06;  // MR11, ODT Enabled, PD Control = 1
        } else {
          MrTable[3].Data = 0x02;  // MR11, ODT Enabled, PD Control = 0
        }
      } else {
        MrTable[3].Data = 0;       // MR11, ODT Disabled
      }

      //
      // Send out all the MR commands from the table
      //
      for (Index = 0; Index < sizeof (MrTable) / sizeof (MrTable[0]); ++Index) {
        MrIndex = MrTable[Index].Address;

        if (Outputs->RestoreMRs) {
          if (MrIndex < MAX_MR_IN_DIMM) {
            MrData = (U8) ChannelOut->Dimm[0].Rank[Rank % 2].MR[MrIndex];
          } else {
            MrData = (U8) ChannelOut->Dimm[0].Rank[Rank % 2].MR11;
          }
        } else {
          //
          // Save the MR value in the global struct
          //
          MrData  = MrTable[Index].Data;

          if (MrIndex < MAX_MR_IN_DIMM) {
            ChannelOut->Dimm[0].Rank[Rank % 2].MR[MrIndex] = MrData;
          } else if (MrIndex == mrMR11) {
            ChannelOut->Dimm[0].Rank[Rank % 2].MR11 = MrData;
          }
        }

        Status = MrcIssueMrw (
                  MrcData,
                  Channel,
                  Rank,
                  MrIndex,
                  MrData,
                  FALSE,  // InitMrw
                  FALSE   // ChipSelect2N
                  );
        if (Status != mrcSuccess) {
          return Status;
        }
      }
      //
      // @todo: Read MR8 (compare to SPD values) - optional step
      //
    }  // for Rank
  } // for Channel

  Outputs->LpddrJedecInitDone = TRUE;
  Outputs->RestoreMRs         = TRUE;
  return Status;
}

#endif // ULT_FLAG

/**
@brief
  Wait in a loop until the first RCOMP has been completed.
  MRC should wait until this bit is set before executing any DDR command.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcDeviceBusy - On Rcomp completion timeout.
  @retval mrcSuccess    - On Rcomp completion.
**/
MrcStatus
CheckFirstRcompDone (
  IN MrcParameters *const MrcData
  )
{
  MCDECS_CR_RCOMP_TIMER_MCMAIN_STRUCT CrRcompTimer;
  BOOL                                Busy;
  U32                                 Timeout;

  Timeout = (U32) MrcGetCpuTime () + 10000; // 10 seconds timeout

  do {
    CrRcompTimer.Data = MrcReadCR (MrcData, MCDECS_CR_RCOMP_TIMER_MCMAIN_REG);
    Busy              = (0 == CrRcompTimer.Bits.First_Rcomp_done) ? TRUE : FALSE;
  } while (Busy && ((U32) MrcGetCpuTime () < Timeout));

  return ((Busy) ? mrcDeviceBusy : mrcSuccess);
}

/**
@brief
  Perform the required steps to exit self refresh in S3/Warm reset:
    Download the Read Reg File for all populated ranks.
    Assert CKE for all the ranks present to pull DIMMs out of Self-Refresh.
    Issue long ZQ Calibration for all the ranks present in the channel.
    Set REUT to normal mode for all channels.
    Set the Power Down Config Register.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess
**/
MrcStatus
MrcSelfRefreshExit (
  IN MrcParameters *const MrcData
  )
{
  const MrcOutput *Outputs;
  U32             Offset;
  U8              Channel;
  U8              Rank;
  DDRDATACH0_CR_DDRCRDATACONTROL0_STRUCT DdrCrDataControl0;

  Outputs = &MrcData->SysOut.Outputs;
  //
  // Download Read Reg File for all populated ranks per channel
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Offset = DDRDATA0CH0_CR_DDRCRDATACONTROL0_REG +
        ((DDRDATA0CH1_CR_DDRCRDATACONTROL0_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      DdrCrDataControl0.Data = MrcReadCR (MrcData, Offset);

      Offset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
        ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
      DdrCrDataControl0.Bits.ReadRFRd = 1;
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
          DdrCrDataControl0.Bits.ReadRFRank = Rank;
          MrcWriteCrMulticast (MrcData, Offset, DdrCrDataControl0.Data);
        }
      }
    }
  }

  //
  // Pull the DIMMs out of self refresh by asserting CKE high.
  // The time needed to stabilize the DCLK (~6uS) should be covered
  // by the last 43 MC CR restores after restoring MC_INIT_STATE
  // in MrcRestoreTrainingValues().
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++){
    if (MrcChannelExist (&MrcData->SysOut.Outputs, Channel)) {
      Offset = 2 + MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_MISC_CKE_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG) * Channel);
      MrcWriteCR8 (MrcData, Offset, Outputs->Controller[0].Channel[Channel].ValidRankBitMask);
    }
  }

  //
  // Issue ZQ Long on both channels / all ranks
  //
  if (MrcIssueZQ (MrcData, 0x3, MRC_ZQ_LONG) != mrcSuccess) {
    MRC_DEBUG_MSG (&MrcData->SysIn.Inputs.Debug, MSG_LEVEL_WARNING, "\nZQ Long failed during S3/warm reset\n");
  }

  return mrcSuccess;
}
