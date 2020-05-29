/** @file

  This module configures the memory controller address decoder.  
  
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


#include <MrcAddressDecodeConfiguration.h>

#if (MAX_CHANNEL > 2)
#error This module only supports a maximum of 2 channels.
#endif
#if (MAX_DIMMS_IN_CHANNEL > 2)
#error This module only supports a maximum of 2 DIMMs per channel.
#endif

/**
@brief
  This function configures the zone configuration registers MAD-CR and MAD-ZR-CR.

  @param[in, out] MrcData - Include all MRC global data.

  @retval Nothing.
**/
void
ZoneConfiguration (
  IN OUT MrcParameters *const MrcData
)
{
  MrcInput                              *Inputs;
  MrcOutput                             *Outputs;
  MrcDebug                              *Debug;
  MrcControllerOut                      *ControllerOut;
  MrcChannelOut                         *ChannelOut;
  MrcChannelOut                         *ChannelOut0;
  MrcChannelOut                         *ChannelOut1;
  MCDECS_CR_MAD_CHNL_MCMAIN_STRUCT      MadChnl;
  MCDECS_CR_MAD_ZR_MCMAIN_STRUCT        MadZr;
  MCDECS_CR_CHANNEL_HASH_MCMAIN_STRUCT  ChannelHash;
  U32                                   ChannelSizeMin;
  U32                                   ChannelSizeBC;
  U32                                   ChannelSize2BC;
  U32                                   ChannelSize[MAX_CHANNEL];
  U8                                    Channel;
  U8                                    Dimm;

  MadChnl.Data      = 0;
  MadZr.Data        = 0;
  ChannelHash.Data  = 0;
  Inputs            = &MrcData->SysIn.Inputs;
  Debug             = &Inputs->Debug;
  Outputs           = &MrcData->SysOut.Outputs;
  ControllerOut     = &MrcData->SysOut.Outputs.Controller[0];

  //
  // Add up the amount of memory in each channel.
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    ChannelOut            = &ControllerOut->Channel[Channel];
    ChannelSize[Channel]  = 0;
    if (ChannelOut->Status == CHANNEL_PRESENT) {
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        if(ChannelOut->Dimm[Dimm].Status == DIMM_PRESENT) {
        ChannelSize[Channel] += ChannelOut->Dimm[Dimm].DimmCapacity;
      }
    }
  }
  }
  //
  // Define MAD_ZR register:
  // MAD-ZR-CR [29:24] = channel C size: ch_c_size
  // MAD-ZR-CR [23:16] = (channel C size) * 3: ch_3c_size
  // MAD-ZR-CR [15:8]  = (channel B size) * 2 + (channel C size): ch_b_2c_size
  // MAD-ZR-CR [7:0]   = (channel B size) + (channel C size): ch_b_c_size
  //
  ChannelOut0 = &ControllerOut->Channel[cCHANNEL0];
  ChannelOut1 = &ControllerOut->Channel[cCHANNEL1];
  if (ChannelSize[cCHANNEL1] <= ChannelSize[cCHANNEL0]) {
    MadChnl.Bits.CH_A = 0;
    MadChnl.Bits.CH_B = 1;

    //
    // Set the virtual channel type according to the address decoding decision.
    //
    ChannelOut0->VirtualChannel = vcA;
    ChannelOut1->VirtualChannel = vcB;

    ChannelSizeMin              = ChannelSize[cCHANNEL1];
  } else {
    //
    //  ChannelSize0 < ChannelSize1
    //
    MadChnl.Bits.CH_A = 1;
    MadChnl.Bits.CH_B = 0;

    //
    // Set the virtual channel type according to the address decoding decision.
    //
    ChannelOut0->VirtualChannel = vcB;
    ChannelOut1->VirtualChannel = vcA;

    ChannelSizeMin              = ChannelSize[cCHANNEL0];
  }
  //
  // Divided by 256 because the channel size is in 256 MB units.
  //
  ChannelSizeBC       = ChannelSizeMin / 256;
  ChannelSize2BC      = ChannelSizeBC << 1;
  MadZr.Bits.BandC    = MIN (ChannelSizeBC, MCDECS_CR_MAD_ZR_MCMAIN_BandC_MAX);
  MadZr.Bits.TwoBandC = MIN (ChannelSize2BC, MCDECS_CR_MAD_ZR_MCMAIN_TwoBandC_MAX);
  MadChnl.Bits.CH_C   = 2;

#ifdef ULT_FLAG
  MadChnl.Bits.LPDDR = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) ? 1 : 0;
#endif
    //
    // Interleaved mode
    // Check for any Channel hash support
    //
    if (Inputs->ChHashEnable) {
      ChannelHash.Bits.Mask         = MIN (Inputs->ChHashMask, MCDECS_CR_CHANNEL_HASH_MCMAIN_Mask_MAX);
      ChannelHash.Bits.LSB_mask_bit = MIN (Inputs->ChHashInterleaveBit, MCDECS_CR_CHANNEL_HASH_MCMAIN_LSB_mask_bit_MAX);
      ChannelHash.Bits.Enable       = 1;
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel HASH Enabled\n");
    }
  
  if (Inputs->MemoryTrace) {
    if (ChannelSize[cCHANNEL0] == ChannelSize[cCHANNEL1]) {
      //
      // Enable the Stacked Mode for memory tracing
      //
      MadChnl.Bits.STKD_MODE = 1;
      MadChnl.Bits.STKD_MODE_CH_BITS = MrcLog2 (ChannelSizeMin) - 9;
      ChannelHash.Bits.Enable = 1;
      MRC_DEBUG_MSG (
        Debug, 
        MSG_LEVEL_NOTE, 
        "Enabling Stacked Mode for Memory Trace, Stacked Mode Ch bit = %u (%u MB per channel)\n",
        MadChnl.Bits.STKD_MODE_CH_BITS + 28,
        ChannelSizeMin
        );
    } else {
      Inputs->MemoryTrace = 0;
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Channels are not equal in size, cannot enable Memory Trace !\n");
    }
  }
  
  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "CHANNEL_HASH = 0x%08X\nMAD_CHNL = 0x%08X\nMAD_ZR = 0x%08X\n",
    ChannelHash.Data,
    MadChnl.Data,
    MadZr.Data
    );
  MrcWriteCR (MrcData, MCDECS_CR_CHANNEL_HASH_MCMAIN_REG, ChannelHash.Data);
  MrcWriteCR (MrcData, MCDECS_CR_MAD_CHNL_MCMAIN_REG, MadChnl.Data);
  MrcWriteCR (MrcData, MCDECS_CR_MAD_ZR_MCMAIN_REG, MadZr.Data);
  return;
}

/**
@brief
  This function configures the MAD_DIMM_CH0/1 register.

  @param[in] MrcData      - Include all MRC global data.
  @param[in] ChannelIndex - Channel index

  @retval Nothing.
**/
void
ChannelAddressDecodeConfiguration (
  IN MrcParameters *const MrcData,
  IN const U32            ChannelIndex
)
{
  const MrcInput                        *Inputs;
  MrcOutput                             *Outputs;
  MrcChannelOut                         *ChannelOut;
  MrcDimmOut                            *DimmA;
  MrcDimmOut                            *DimmB;
  MCDECS_CR_MAD_DIMM_CH0_MCMAIN_STRUCT  MadDimm;
  U32                                   DimmCapacity;
  U32                                   Dimm0Capacity;
  U32                                   Dimm1Capacity;
  U32                                   Scratch;
#ifdef ULT_FLAG
  MCHBAR_CH0_CR_LPDDR_MR_PARAMS_STRUCT  LpddrMrParams;
#endif //ULT_FLAG

  Inputs        = &MrcData->SysIn.Inputs;
  Outputs       = &MrcData->SysOut.Outputs;
  ChannelOut    = &Outputs->Controller[0].Channel[ChannelIndex];
  MadDimm.Data  = 0;
  if (ChannelOut->Dimm[dDIMM0].Status == DIMM_PRESENT) {
  Dimm0Capacity = ChannelOut->Dimm[dDIMM0].DimmCapacity;
  } else {
    Dimm0Capacity = 0;
  }

  if (ChannelOut->Dimm[dDIMM1].Status == DIMM_PRESENT) {
  Dimm1Capacity = (MAX_DIMMS_IN_CHANNEL > 1) ? ChannelOut->Dimm[dDIMM1].DimmCapacity : 0;
  } else {
    Dimm1Capacity = 0;
  }

  //
  // larger dimm will be located to Dimm A and small dimm will be located to dimm B
  //
  if (Dimm1Capacity <= Dimm0Capacity) {
    DimmA = &ChannelOut->Dimm[dDIMM0];
    DimmB = &ChannelOut->Dimm[dDIMM1];
    //
    // larger DIMM in capacity 0 - DIMM 0 or 1 - DIMM 1
    //
    MadDimm.Bits.DAS = 0;
  } else {
    DimmA = &ChannelOut->Dimm[dDIMM1];
    DimmB = &ChannelOut->Dimm[dDIMM0];
    //
    // larger DIMM in capacity 0 - DIMM 0 or 1 - DIMM 1
    //
    MadDimm.Bits.DAS = 1;
  }
  //
  // Dimm A
  //
  if ((0 < DimmA->RankInDIMM) && (DimmA->Status == DIMM_PRESENT)) {
    DimmCapacity              = DimmA->DimmCapacity / 256;
    MadDimm.Bits.DIMM_A_Size  = MIN (DimmCapacity, MCDECS_CR_MAD_DIMM_CH0_MCMAIN_DIMM_A_Size_MAX);
    //
    // RankInDIMM must be 1 or 2, we test the case that the value is 0
    //
    Scratch             = DimmA->RankInDIMM - 1;
    MadDimm.Bits.DANOR  = MIN (Scratch, MCDECS_CR_MAD_DIMM_CH0_MCMAIN_DANOR_MAX);
    //
    // SDRAM width x8 or x32 set to 0, x16 set to 1
    //
    MadDimm.Bits.DAW = (DimmA->SdramWidth == 16) ? 1 : 0;
  }
  //
  // Dimm B
  //
  if ((MAX_DIMMS_IN_CHANNEL > 1) && (0 < DimmB->RankInDIMM) && (DimmB->Status == DIMM_PRESENT)) {
    DimmCapacity              = DimmB->DimmCapacity / 256;
    MadDimm.Bits.DIMM_B_Size  = MIN (DimmCapacity, MCDECS_CR_MAD_DIMM_CH0_MCMAIN_DIMM_B_Size_MAX);
    //
    // RankInDIMM must be 1 or 2, we test the case that this value is 0.
    //
    Scratch             = DimmB->RankInDIMM - 1;
    MadDimm.Bits.DBNOR  = MIN (Scratch, MCDECS_CR_MAD_DIMM_CH0_MCMAIN_DBNOR_MAX);

    //
    // SDRAM width x8 or x32 set to 0, x16 set to 1
    //
    MadDimm.Bits.DBW = (DimmB->SdramWidth == 16) ? 1 : 0;
  }

#ifdef ULT_FLAG
  if (Inputs->CpuModel == cmHSW_ULT) {
    //
    // On HSW-ULT only 1DPC is supported, and DBW should have the same value as DAW
    //
    MadDimm.Bits.DBW = MadDimm.Bits.DAW;
  }
#endif

  if (Inputs->RankInterleave) {
    MadDimm.Bits.RI = MRC_DIMM_RANK_INTERLEAVE;
  }
  if (Inputs->EnhancedInterleave) {
    MadDimm.Bits.Enh_Interleave = MRC_ENHANCED_INTERLEAVE_MODE;
  }

  MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "MAD_DIMM_CH%u = 0x%08X\n", ChannelIndex, MadDimm.Data);
  MrcWriteCR (
    MrcData,
    MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG +
    (ChannelIndex * (MCDECS_CR_MAD_DIMM_CH1_MCMAIN_REG - MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG)),
    MadDimm.Data
    );

#ifdef ULT_FLAG
  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    LpddrMrParams.Data            = 0;
    LpddrMrParams.Bits.MR4_PERIOD = 0x200D;

    if (DimmA->SdramWidth == 32) {
      LpddrMrParams.Bits.Rank_0_x32 = 1;
      LpddrMrParams.Bits.Rank_1_x32 = 1;
    }

    MrcWriteCR (
      MrcData,
      MCHBAR_CH0_CR_LPDDR_MR_PARAMS_REG +
      (ChannelIndex * (MCHBAR_CH1_CR_LPDDR_MR_PARAMS_REG - MCHBAR_CH0_CR_LPDDR_MR_PARAMS_REG)),
      LpddrMrParams.Data
      );
  }
#endif // ULT_FLAG
  return;
}

/**
@brief
  This function is the main address decoding configuration function.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing.
**/
void
MrcAdConfiguration (
  IN MrcParameters *const MrcData
)
{
  MrcOutput         *Outputs;
  MrcChannelOut     *ChannelOut;
  MrcControllerOut  *ControllerOut;
  MrcDimmOut        *DimmOut;
  U8                Controller;
  U8                Channel;
  U8                Dimm;

  ZoneConfiguration (MrcData);

  Outputs = &MrcData->SysOut.Outputs;
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerOut = &Outputs->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!MrcChannelExist (Outputs, Channel)) {
        continue;
      }
      ChannelOut = &ControllerOut->Channel[Channel];

      ChannelAddressDecodeConfiguration (MrcData, Channel);

      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; ++Dimm) {
        DimmOut = &ChannelOut->Dimm[Dimm];
        if (DimmOut->Status == DIMM_PRESENT) {
          MRC_DEBUG_MSG (
            &MrcData->SysIn.Inputs.Debug,
            MSG_LEVEL_NOTE,
            "Channel: %u, Dimm %d Rank in DIMM is: %u\n",
            Channel,
            Dimm,
            DimmOut->RankInDIMM
            );
        }
      } // for Dimm
    } // for Channel
  } // for Controller

  return;
}
