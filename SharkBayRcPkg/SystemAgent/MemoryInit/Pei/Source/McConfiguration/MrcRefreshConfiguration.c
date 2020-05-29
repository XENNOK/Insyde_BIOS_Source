/** @file

  This module sets the memory controller refresh parameters.
  
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

///
/// Include files
///
#include <MrcRefreshConfiguration.h>

/**
@brief
  This function returns the tXS offset.
  tXS-offset: tXS = tRFC+10ns. Setup of tXS-offset is # of cycles for 10 ns.

  @param[in] Frequency - The memory frequency.

  @retval tXS-offset value.
**/
static
U32
tXsOffset (
  IN const MrcFrequency Frequency
  )
{
  U32 tXSOffset;

  if (Frequency <= f800) {
    tXSOffset = Offset_10nsec_800;
  } else if (Frequency <= f1067) {
    tXSOffset = Offset_10nsec_1067;
  } else if (Frequency <= f1333) {
    tXSOffset = Offset_10nsec_1333;
  } else if (Frequency <= f1600) {
    tXSOffset = Offset_10nsec_1600;
  } else if (Frequency <= f1867) {
    tXSOffset = Offset_10nsec_1867;
  } else if (Frequency <= f2133) {
    tXSOffset = Offset_10nsec_2133;
  } else if (Frequency <= f2400) {
    tXSOffset = Offset_10nsec_2400;
  } else {
    tXSOffset = Offset_10nsec_2667;
  }

  return tXSOffset;
}

/**
@brief
  This function configures the TC-RFTP register and its fields 9tREFI, tRFC, tREFI.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] ChannelIndex    - Channel to work on.

  @retval Nothing.
**/
static
void
SetTcRftpReg (
  IN MrcParameters *const MrcData,
  IN const U32            ChannelIndex
  )
{
  MrcTiming                     *TimingOut;
  MCHBAR_CH0_CR_TC_RFTP_STRUCT  TcRftp;
  MrcProfile                    Profile;
  U32                           tRefix9;
  U32                           Offset;
  U16                           tREFI_MAX;

  Profile             = MrcData->SysIn.Inputs.MemoryProfile;
  TimingOut           = &MrcData->SysOut.Outputs.Controller[0].Channel[ChannelIndex].Timing[Profile];
  tRefix9             = (TimingOut->tREFI * 89) / (1024 * 10);
  TcRftp.Data         = 0;
  tREFI_MAX = MCHBAR_CH0_CR_TC_RFTP_tREFI_MAX;
  TcRftp.Bits.tREFI   = MIN (tREFI_MAX, TimingOut->tREFI);
  TcRftp.Bits.tRFC    = MIN (MCHBAR_CH0_CR_TC_RFTP_tRFC_MAX, TimingOut->tRFC);
  TcRftp.Bits.tREFIx9 = MIN (MCHBAR_CH0_CR_TC_RFTP_tREFIx9_MAX, tRefix9);
  Offset              = MCHBAR_CH0_CR_TC_RFTP_REG +
                        ((MCHBAR_CH1_CR_TC_RFTP_REG - MCHBAR_CH0_CR_TC_RFTP_REG) * ChannelIndex);
  MrcWriteCR (MrcData, Offset, TcRftp.Data);
  MRC_DEBUG_MSG (
    &MrcData->SysIn.Inputs.Debug,
    MSG_LEVEL_NOTE,
    "TC-RFTP Channel %u register value = %Xh\n",
    ChannelIndex,
    TcRftp.Data
    );
  return;
}

/**
@brief
  This function configures the TC-SRFTP register and its fields tZQOPER, tXS-offset, tXSDLL.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] ChannelIndex    - Channel to work on.

  @retval Nothing.
**/
static
void
SetTcSrftpReg (
  IN MrcParameters *const MrcData,
  IN const U32            ChannelIndex
  )
{
  const MrcInput                *Inputs;
  MrcFrequency                  Frequency;
  U32                           tZQOPER;
  U32                           tXS_offset;
  U32                           tMod;
  U32                           Offset;
  MCHBAR_CH0_CR_TC_SRFTP_STRUCT CrTcSrftp;

  Inputs                    = &MrcData->SysIn.Inputs;
  Frequency                 = MrcData->SysOut.Outputs.Frequency;
  tZQOPER                   = tZQOPERGet (MrcData, Frequency);
  tXS_offset                = tXsOffset (Frequency);
  tMod                      = tMODGet (Frequency) - 8;

  CrTcSrftp.Data            = 0;
  CrTcSrftp.Bits.tXSDLL     = MIN (MCHBAR_CH0_CR_TC_SRFTP_tXSDLL_MAX, tDLLK_VALUE);
  CrTcSrftp.Bits.tXS_offset = MIN (MCHBAR_CH0_CR_TC_SRFTP_tXS_offset_MAX, tXS_offset);
  CrTcSrftp.Bits.tZQOPER    = MIN (MCHBAR_CH0_CR_TC_SRFTP_tZQOPER_MAX, tZQOPER);
  CrTcSrftp.Bits.tMOD       = MIN (MCHBAR_CH0_CR_TC_SRFTP_tMOD_MAX, tMod);
  Offset                    = MCHBAR_CH0_CR_TC_SRFTP_REG +
                              ((MCHBAR_CH1_CR_TC_SRFTP_REG - MCHBAR_CH0_CR_TC_SRFTP_REG) * ChannelIndex);
  MrcWriteCR (MrcData, Offset, CrTcSrftp.Data);
  MRC_DEBUG_MSG (
    &Inputs->Debug,
    MSG_LEVEL_NOTE,
    "Ch%u: TC_SRFTP = %Xh\n tXSDLL = %u\n tXS_offset = %u\n tZQOPER = %u\n tMOD = %u\n",
    ChannelIndex,
    CrTcSrftp.Data,
    CrTcSrftp.Bits.tXSDLL,
    CrTcSrftp.Bits.tXS_offset,
    CrTcSrftp.Bits.tZQOPER,
    CrTcSrftp.Bits.tMOD
    );
  return;
}

/**
@brief
  This function returns the tZQOPER value.
  tZQOPER = Defines the period required for ZQCL after SR exit.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] Frequency       - The memory frequency.

  @retval The tZQOPER value.
**/
U32
tZQOPERGet (
  IN MrcParameters *const MrcData,
  IN const MrcFrequency   Frequency
  )
{
  U32             tZQOPER;
#ifdef ULT_FLAG
  MrcDdrType      DdrType;

  DdrType = MrcData->SysOut.Outputs.DdrType;
  if (DdrType == MRC_DDR_TYPE_LPDDR3) {
    tZQOPER = tZQCL_MIN / (MrcData->SysOut.Outputs.Qclkps * 2);
  } else
#endif // ULT_FLAG
  {
    if (Frequency <= f1600) {
      ///
      /// All frequencies below 1600 uses the same value
      ///
      tZQOPER = tZQOPER_1600;
    } else if (Frequency <= f1867) {
      tZQOPER = tZQOPER_1867;
    } else if (Frequency <= f2133) {
      tZQOPER = tZQOPER_2133;
    } else if (Frequency <= f2400) {
      tZQOPER = tZQOPER_2400;
    } else {
      tZQOPER = tZQOPER_2667;
    }
  }

  return tZQOPER;
}

/**
@brief
  This function returns the tMOD value.
  tMOD = max(12nCK, 15ns) nCK change by the frequency.

  @param[in] Frequency       - The memory frequency.

  @retval The tMOD value.
**/
U32
tMODGet (
  IN const MrcFrequency Frequency
  )
{
  U32 tMOD;

  if (Frequency <= f800) {
    tMOD = tMOD_800;
  } else if (Frequency <= f1067) {
    tMOD = tMOD_1067;
  } else if (Frequency <= f1333) {
    tMOD = tMOD_1333;
  } else if (Frequency <= f1600) {
    tMOD = tMOD_1600;
  } else if (Frequency <= f1867) {
    tMOD = tMOD_1867;
  } else if (Frequency <= f2133) {
    tMOD = tMOD_2133;
  } else if (Frequency <= f2400) {
    tMOD = tMOD_2400;
  } else {
    tMOD = tMOD_2667;
  }

  return tMOD;
}

/**
@brief
  This function configures the TC-ZQCAL register and its fields tZQCS and tZQCS_PERIOD.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] ChannelIndex    - Channel to work on.

  @retval Nothing.
**/
static
void
SetTcZqCalReg (
  IN MrcParameters *const MrcData,
  IN const U32            ChannelIndex
  )
{
  MCHBAR_CH0_CR_TC_ZQCAL_STRUCT ZqCal;
  U32                           ZQCS_period;
  U32                           tZQCS;
  U32                           Offset;
  MrcCpuModel                   CpuModel;
#ifdef ULT_FLAG
  MrcDdrType                    DdrType;
#endif // ULT_FLAG

  CpuModel = MrcData->SysIn.Inputs.CpuModel;

  Offset = MCHBAR_CH0_CR_TC_ZQCAL_REG + (MCHBAR_CH1_CR_TC_ZQCAL_REG - MCHBAR_CH0_CR_TC_ZQCAL_REG) * ChannelIndex;

  ZqCal.Data  = 0;
  tZQCS       = tZQCSGet (MrcData, MrcData->SysOut.Outputs.Frequency);
  ZQCS_period = ZQCS_PERIOD_DDR3;

#ifdef ULT_FLAG
  DdrType = MrcData->SysOut.Outputs.DdrType;
  if (CpuModel == cmHSW_ULT) {
    if (DdrType == MRC_DDR_TYPE_LPDDR3) {
      ZQCS_period = ZQCS_PERIOD_LPDDR3;
    }
    ZqCal.UltBits.ZQCS_period = MIN (MCHBAR_CH0_CR_TC_ZQCAL_ZQCS_period_Ult_MAX, ZQCS_period);
    ZqCal.UltBits.tZQCS       = MIN (MCHBAR_CH0_CR_TC_ZQCAL_tZQCS_Ult_MAX, tZQCS);
  } else
#endif // ULT_FLAG
  {
    ZqCal.Bits.ZQCS_period = MIN (MCHBAR_CH0_CR_TC_ZQCAL_ZQCS_period_MAX, ZQCS_period);
    ZqCal.Bits.tZQCS       = MIN (MCHBAR_CH0_CR_TC_ZQCAL_tZQCS_MAX, tZQCS);
  }
  MrcWriteCR (MrcData, Offset, ZqCal.Data);

#ifdef ULT_FLAG
  if (CpuModel == cmHSW_ULT) {
    ZQCS_period = ZqCal.UltBits.ZQCS_period;
    tZQCS       = ZqCal.UltBits.tZQCS;
  } else
#endif //ULT_FLAG
  {
    ZQCS_period = ZqCal.Bits.ZQCS_period;
    tZQCS       = ZqCal.Bits.tZQCS;
  }

  MRC_DEBUG_MSG (
    &MrcData->SysIn.Inputs.Debug,
    MSG_LEVEL_NOTE,
    "Ch%u: TC_ZQCAL = %Xh\n ZQCS_period = %u\n tZQCS = %u\n",
    ChannelIndex,
    ZqCal.Data,
    ZQCS_period,
    tZQCS
    );

  return;
}

/**
@brief
  This function returns the tZQCS value.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] Frequency       - The memory frequency.

  @retval The tZQCS value.
**/
U32
tZQCSGet (
  IN MrcParameters *const MrcData,
  IN const MrcFrequency   Frequency
  )
{
  U32         tZQCS;
#ifdef ULT_FLAG
  MrcDdrType  DdrType;

  DdrType = MrcData->SysOut.Outputs.DdrType;
  if (DdrType == MRC_DDR_TYPE_LPDDR3) {
    tZQCS = tZQCS_MIN / (MrcData->SysOut.Outputs.Qclkps * 2);
  } else
#endif // ULT_FLAG
  {
    if (Frequency <= f1600) {
      //
      // All frequencies below 1600 uses the same value
      //
      tZQCS = tZQCS_1600;
    } else if (Frequency <= f1867) {
      tZQCS = tZQCS_1867;
    } else if (Frequency <= f2133) {
      tZQCS = tZQCS_2133;
    } else if (Frequency <= f2400) {
      tZQCS = tZQCS_2400;
    } else {
      tZQCS = tZQCS_2667;
    }
  }
  return tZQCS;
}

/**
@brief
  This function configures the TC_MR2_SHADDOW register and its fields.

  @param[in] MrcData         - Include all MRC global data.
  @param[in] Channel         - Channel to work on.
  @param[in] Dimm            - Dimm to work on.
  @param[in] Mr2Value        - The value of MR2 to setup.

  @retval Nothing.
**/
void
SetTcMr2ShadowReg (
  IN MrcParameters *const MrcData,
  IN const U32            Channel,
  IN const U32            Dimm,
  IN U32                  Mr2Value
  )
{
  MrcDimmOut                          *DimmOut;
  MCHBAR_CH0_CR_TC_MR2_SHADDOW_STRUCT TcMr2Shaddow;
  U32                                 Offset;

  Offset = MCHBAR_CH0_CR_TC_MR2_SHADDOW_REG +
            ((MCHBAR_CH1_CR_TC_MR2_SHADDOW_REG - MCHBAR_CH0_CR_TC_MR2_SHADDOW_REG) * Channel);
  TcMr2Shaddow.Data             = MrcReadCR (MrcData, Offset);
  TcMr2Shaddow.Bits.MR2_sh_high = 0;
  TcMr2Shaddow.Bits.MR2_sh_low  = 0;

  DimmOut                       = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel].Dimm[Dimm];
  if ((DimmOut->SelfRefreshTemp) == TRUE) {
    TcMr2Shaddow.Bits.SRT_avail |= MRC_BIT0 << Dimm;
  }
  ((MCHBAR_CH0_CR_TC_MR2_SHADDOW_STRUCT *) (&Mr2Value))->Bits.SRT_avail = 0;
  TcMr2Shaddow.Data |= Mr2Value;

  //
  // Set address bit swizzle according to the DIMM number.
  //
  if (DimmOut->AddressMirrored == TRUE) {
    TcMr2Shaddow.Bits.Addr_bit_swizzle |= MRC_BIT0 << Dimm;
  }

  MrcWriteCR (MrcData, Offset, TcMr2Shaddow.Data);
  //
  // MRC_DEBUG_MSG (&MrcData->Inputs.Debug, MSG_LEVEL_NOTE, "TC-MR2_SHADOW Channel %u register value = %Xh\n", Channel, TcMr2Shaddow.Data);
  //
  return;
}

/**
@brief
  This function executes the refresh configuration process.

  @param[in] MrcData         - Include all MRC global data.

  @retval Nothing.
**/
void
MrcRefreshConfiguration (
  IN MrcParameters *const MrcData
  )
{
  MCMNTS_CR_TC_RFP_STRUCT TcRFP;
  U32                     Offset;  
  U8  Channel;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (&MrcData->SysOut.Outputs, Channel)) {
      SetTcRftpReg (MrcData, Channel);
      SetTcSrftpReg (MrcData, Channel);
      SetTcZqCalReg (MrcData, Channel);

      //
      // set LP0 WM and OREF_RI to support high memory BW traffic
      //
      Offset = MCHBAR_CH0_CR_TC_RFP_REG +
        ((MCHBAR_CH1_CR_TC_RFP_REG - MCHBAR_CH0_CR_TC_RFP_REG) * Channel);
      TcRFP.Data = MrcReadCR (MrcData, Offset);
      TcRFP.Bits.OREF_RI = 0xFF;
      MrcWriteCR (MrcData, Offset, TcRFP.Data);
      TcRFP.Data = MrcReadCR (MrcData, Offset);
      MRC_DEBUG_MSG (
        &MrcData->SysIn.Inputs.Debug,
        MSG_LEVEL_NOTE,
        "Ch%u: TC_RFP = %Xh\n OREF_RI = %u\n",
        Channel,
        TcRFP.Data,
        TcRFP.Bits.OREF_RI
        );
    }
  }

  return;
}
