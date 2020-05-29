/** @file
  This file include all the common tolls for the mrc algo

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
#include <MrcCommon.h>

#ifdef MRC_DEBUG_PRINT
const char  CcdString[] = "Controller, Channel, Dimm";
const char  RcvEnDelayString[] = "RcvEnDelay";
const char  DqsDelayString[] = "DqsDelay";

#endif

/**
  Return the rank mask in channel if rank exist exist.

  @param[in] MrcData - Pointer to MRC global data.
  @param[in] Channel - Channel to work on.
  @param[in] Rank    - Rank to check.

  @retval Bit mask of Rank requested if the Rank exists in the system.
**/
U8
MrcRankInChannelExist (
  IN MrcParameters *const MrcData,
  IN const U8             Rank,
  IN const U8             Channel
  )
{
  return (MRC_BIT0 << Rank) & MrcData->SysOut.Outputs.Controller[0].Channel[Channel].ValidRankBitMask;
}

/**
  Return the number of ranks in specific dimm.

  @param[in] MrcData - Pointer to MRC global data.
  @param[in] Channel - Channel to work on.
  @param[in] Dimm    - Dimm in channel to return.

  @retval The number of ranks in the dimm.
**/
U8
MrcGetRankInDimm (
  IN MrcParameters *const MrcData,
  IN const U8             Dimm,
  IN const U8             Channel
  )
{
  return MrcData->SysOut.Outputs.Controller[0].Channel[Channel].Dimm[Dimm].RankInDIMM;
}

/**
  Returns whether Channel is or is not present.

  @param[in] Outputs - Pointer to MRC global Output data.
  @param[in] Channel - Channel to test.

  @retval TRUE  - if there is at least one enabled DIMM in the channel.
  @retval FALSE - if there are no enabled DIMMs in the channel.
**/
BOOL
MrcChannelExist (
  IN const MrcOutput *const Outputs,
  IN const U8               Channel
  )
{

  return (Outputs->Controller[0].Channel[Channel].Status == CHANNEL_PRESENT) ? TRUE : FALSE;
}

/**
  This function disable channel parameters.
  After this function the MRC don't use with the channel.

  @param[in] MrcData           - Include all MRC global data.
  @param[in] ChannelToDisable  - Channel to disable.
  @param[in] SkipDimmCapacity  - Switch to skip setting the DimmCapacity to 0 for the dimms in the channel disabled.

  @retval Nothing
**/
void
MrcChannelDisable (
  IN MrcParameters *const MrcData,
  IN const U8             ChannelToDisable,
  IN const U8             SkipDimmCapacity
  )
{
  MrcChannelOut *ChannelOut;
  MrcDimmOut    *DimmOut;
  U32           Dimm;

  ChannelOut = &MrcData->SysOut.Outputs.Controller[0].Channel[ChannelToDisable];
  if (ChannelOut->Status == CHANNEL_PRESENT) {
    ChannelOut->Status            = CHANNEL_DISABLED;
    ChannelOut->RankInChannel     = 0;
    ChannelOut->ValidRankBitMask  = 0;
    for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
      DimmOut = &ChannelOut->Dimm[Dimm];
      if (DimmOut->Status == DIMM_PRESENT) {
        DimmOut->Status     = DIMM_DISABLED;
        DimmOut->RankInDIMM = 0;
        if (!SkipDimmCapacity) {
          DimmOut->DimmCapacity = 0;
        }
      }
    }
  }
}

/**
  Convert the given frequency and reference clock to a clock ratio.

  @param[in] MrcData   - Pointer to MRC global data.
  @param[in] Frequency - The memory frequency.
  @param[in] RefClk    - The memory reference clock.
  @param[in] BClk      - The base system reference clock.

  @retval Returns the memory clock ratio.
**/
MrcClockRatio
MrcFrequencyToRatio (
  IN MrcParameters *const  MrcData,
  IN const MrcFrequency    Frequency,
  IN const MrcRefClkSelect RefClk,
  IN const MrcBClkRef      BClk
  )
{
  U64 Value;
  U64 FreqValue;
  U32 RefClkValue;
  U32 BClkValue;

  BClkValue   = (BClk == 0) ? (BCLK_DEFAULT / 100000) : (BClk / 100000);
  RefClkValue = (RefClk == MRC_REF_CLOCK_100) ? 200000 : 266667;
  FreqValue   = MrcOemMemoryMultiplyU64ByU32 (Frequency, 1000000000ULL);
  Value       = MrcOemMemoryDivideU64ByU64 (FreqValue, (RefClkValue * BClkValue));
  Value       = ((U32) Value + 500) / 1000;
  return ((MrcClockRatio) Value);
}

/**
  @brief
  Convert the given ratio and reference clocks to a memory frequency.

  @param[in] MrcData - Pointer to MRC global data.
  @param[in] Ratio   - The memory ratio.
  @param[in] RefClk  - The memory reference clock.
  @param[in] BClk    - The base system reference clock.

  @retval Returns the memory frequency.
**/
MrcFrequency
MrcRatioToFrequency (
  IN MrcParameters *const  MrcData,
  IN const MrcClockRatio   Ratio,
  IN const MrcRefClkSelect RefClk,
  IN const MrcBClkRef      BClk
  )
{
  U64 Value;
  U32 BClkValue;
  U32 RefClkValue;

  BClkValue   = (BClk == 0) ? BCLK_DEFAULT : BClk;
  RefClkValue = (RefClk == MRC_REF_CLOCK_100) ? 200000000 : 266666667;
  Value       = MrcOemMemoryMultiplyU64ByU32 (RefClkValue, Ratio * BClkValue);
  Value      += 50000000000000ULL;
  Value       = MrcOemMemoryDivideU64ByU64 (Value, 100000000000000ULL);
  return ((MrcFrequency) Value);
}

/**
  Convert the given ratio and reference clocks to a memory clock. 

  @param[in] Ratio  - The memory ratio.
  @param[in] RefClk - The memory reference clock.
  @param[in] BClk   - The base system reference clock.

  @retval Returns the memory clock in femtoseconds.
**/
U32
MrcRatioToClock (
  IN const MrcClockRatio   Ratio,
  IN const MrcRefClkSelect RefClk,
  IN const MrcBClkRef      BClk
  )
{
  U32 BClkValue;
  U32 RefClkValue;
  U32 Factor;
  U64 Value;

  BClkValue   = (BClk == 0) ? 100000000UL : BClk;
  Factor      = BClkValue / 100000UL;
  RefClkValue = (RefClk == MRC_REF_CLOCK_100) ? 1000000000UL : 1333333333UL;
  Value       = MrcOemMemoryMultiplyU64ByU32 (Factor, RefClkValue);
  Value       = MrcOemMemoryMultiplyU64ByU32 (Value, Ratio);
  return ((Value == 0) ? 0 : (U32) MrcOemMemoryDivideU64ByU64 (10000000000000000000ULL, Value));
}

/**
  This function return the DIMM number according to the rank number.

  @param[in] Rank - The requested rank.

  @retval The DIMM number.
**/
U8
MrcGetDimmFromRank (
  IN const U8 Rank
  )
{
  U8  Dimm;

  if (Rank == rRank0 || Rank == rRank1) {
    Dimm = dDIMM0;
  } else {
    Dimm = dDIMM1;
  }

  return Dimm;
}

/**
  This function sets the memory frequency.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess on success, mrcFrequencyError on error.
**/
MrcStatus
McFrequencySet (
  IN MrcParameters *const MrcData
  )
{
  const MrcDebug                *Debug;
  const MrcInput                *Inputs;
  MrcOutput                     *Outputs;
  MrcFrequency                  NewFrequency;
  MrcClockRatio                 Ratio;
  MrcRefClkSelect               RefClk;
  PCU_CR_MC_BIOS_REQ_PCU_STRUCT McBiosReq;
  U32                           MemoryClock;
#ifdef MRC_DEBUG_PRINT
  U8                            Channel;
#endif // MRC_DEBUG_PRINT
  U32                           Time;

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;

  NewFrequency = MrcGetCurrentMemoryFrequency (MrcData, &MemoryClock, &Ratio, &RefClk);
  if (NewFrequency != fNoInit) {
    Outputs->Frequency   = NewFrequency;
    Outputs->MemoryClock = MemoryClock;
    Outputs->RefClk      = RefClk;
    Outputs->Ratio       = Ratio;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "WARNING: Memory frequency is already initialized to %u\n", Outputs->Frequency);
    return mrcSuccess;
  }
  //
  // Set the reference clock, ratio and run_busy bit.
  if (Outputs->BootMode == bmCold) {
    if ((Inputs->MemoryProfile == USER_PROFILE) && (Inputs->Ratio > 0)) {
      Outputs->Ratio = Inputs->Ratio;
    } else {
      Outputs->Ratio = MrcFrequencyToRatio (MrcData, Outputs->Frequency, Outputs->RefClk, Inputs->BClkFrequency);
    }
  }
  if ((MEMORY_RATIO_MIN > Outputs->Ratio) || (MEMORY_RATIO_MAX < Outputs->Ratio)) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_ERROR,
      "Invalid DDR ratio of %u specified, range %u - %u\n",
      Outputs->Ratio,
      MEMORY_RATIO_MIN,
      MEMORY_RATIO_MAX
      );
  } else {
    McBiosReq.Data          = 0;
    McBiosReq.Bits.REQ_DATA = Outputs->Ratio;
    McBiosReq.Bits.REQ_TYPE = (Outputs->RefClk == MRC_REF_CLOCK_133) ? 0 : 1;
    McBiosReq.Bits.RUN_BUSY = 1;
    MrcWriteCR (MrcData, PCU_CR_MC_BIOS_REQ_PCU_REG, McBiosReq.Data);

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Attempting value = 0x%x - Pll busy wait ", McBiosReq.Data);
    Time = 1000 * (U32) MrcGetCpuTime ();
    while (McBiosReq.Bits.RUN_BUSY && (MrcGetCpuTime () < Time))
    {
      McBiosReq.Data = MrcReadCR (MrcData, PCU_CR_MC_BIOS_REQ_PCU_REG);
    }

    if (McBiosReq.Bits.RUN_BUSY) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "- NOT DONE. DDR frequency Update FAILED!\n");
      return mrcFrequencyError;
    } else
    {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "- done\n");
    }
    //
    // Wait on RCOMP Done.  Needed to ensure Rcomp completes on warm reset/S3 before restoring dclk_enable.
    //
    if (CheckFirstRcompDone (MrcData) != mrcSuccess) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "RComp did not complete before the timeout in McFrequencySet\n");
      return mrcDeviceBusy;
    }

#ifdef MRC_DEBUG_PRINT
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "Channel %u post PLL RCOMP REG = %Xh\n",
        Channel,
        MrcReadCR (
          MrcData,
          DDRCKECH0_CR_DDRCRCMDCOMP_REG + ((DDRCKECH1_CR_DDRCRCMDCOMP_REG - DDRCKECH0_CR_DDRCRCMDCOMP_REG) * Channel))
          );
    }
#endif
    Outputs->Frequency = MrcGetCurrentMemoryFrequency (MrcData, &MemoryClock, &Ratio, &RefClk);
    MRC_DEBUG_MSG (
      Debug,
      (Ratio == Outputs->Ratio) ? MSG_LEVEL_NOTE : MSG_LEVEL_ERROR,
      "Requested/actual ratio %u/%u, frequency is %u, BClk %uHz RefClk %uMHz, memory clock %u\n",
      Outputs->Ratio,
      Ratio,
      Outputs->Frequency,
      Inputs->BClkFrequency,
      (RefClk == MRC_REF_CLOCK_133) ? 133 : 100,
      MemoryClock);
    if (Ratio == Outputs->Ratio) {
      return mrcSuccess;
    }
  }
  return mrcFrequencyError;
}

/**
  Returns the extrapolated margin to a fixed # of errors (logT)
  vrefpass is 10x the first passing margin (with no errors) (10x used for int math)
  Errors at vrefpass/10+1 = log1
  Errors at vrefpass/10+2 = logT

  @param[in]      vrefpass      - 10x the first pass margin (w/no errors) (10x used for int match)
  @param[in]      errLog_1      - Errors at vrefpass/10+1
  @param[in]      errLog_2      - Errors at vrefpass/10+2
  @param[in]      errLog_Target - Error target determines extrapolation vs interpolation
  @param[in, out] *berStats     - Used to track interpolation vs extrapolation or if the slope is non-monotonic.
                                  NOTE: target would be Interpolation only

  @retval Interpolated/Extrapolated vref with the scale increased by 10.
**/
U32
interpolateVref (
  IN     U32  vrefpass,
  IN     U32  errLog_1,
  IN     U32  errLog_2,
  IN     U32  errLog_Target,
  IN OUT U32  *berStats
  )
{
  U32 vref;
  U32 slope;
  U32 ErrLogDiff;

  ErrLogDiff = errLog_2 - errLog_1;
  if (errLog_2 <= errLog_1) {
    berStats[3] += 1;                 // non-monotonic case
    return (vrefpass * 10 + 10);
  } else if (errLog_2 < errLog_Target) {
    berStats[2] += 1;                 // error target above errLog_2 -> extrapolation
  } else if (errLog_1 <= errLog_Target) {
    berStats[1] += 1;                 // error target between errLog_1 and errLog_2 -> interpolation
  } else {
    berStats[0] += 1;                 // error target below errLog_1 -> extrapolation
  }

  //
  //extrapolate above errLog_2, max extrapolation is 1 tick (10)
  //
  if (errLog_2 < errLog_Target) {
    vref =  vrefpass * 10 + 20 + MIN (10, (10 * (errLog_Target - errLog_2)) / (ErrLogDiff));
  } else if ( (errLog_1 <= errLog_Target) && (errLog_Target <= errLog_2) && (ErrLogDiff != 0)) {
    vref =  vrefpass * 10 + 10 + (10 * (errLog_Target - errLog_1)) / (ErrLogDiff);  //interpolate
  } else {
    //
    //extrapolate below errLog_1
    //
    slope = (ErrLogDiff) > errLog_1 ? (ErrLogDiff) : errLog_1;
    if (slope != 0) {
      vref = vrefpass * 10 + (10 * errLog_Target) / slope;
    } else {
      vref = 0;
    }
  }

  return vref;  //returns a (vref * 10) interpolation/extrapolation
};

/**
  This function swaps a subfield, within a 32 bit integer value with the specified value.

  @param[in] CurrentValue - 32 bit input value.
  @param[in] NewValue     - 32 bit New value.
  @param[in] Start        - Subfield start bit.
  @param[in] Length       - Subfield length in bits/

  @retval The updated 32 bit value.
**/
U32
MrcBitSwap (
  IN U32       CurrentValue,
  IN const U32 NewValue,
  IN const U8  Start,
  IN const U8  Length
  )
{
  U32 mask;

  //
  // Do bitwise replacement:
  //
  mask = (MRC_BIT0 << Length) - 1;
  CurrentValue &= ~(mask << Start);
  CurrentValue |= ((NewValue & mask) << Start);

  return CurrentValue;
}

/**
  This function returns the maximim Rx margin for a given Channel, Rank(s), and byte.

  @param[in] MrcData   - Pointer to MRC global data.
  @param[in] Channel   - Channel to calculate max Rx margin.
  @param[in] RankRx    - Rank index.  0xFF causes all ranks to be considered.
  @param[in] byte      - Byte to check.
  @param[in] sign      - Sign of the margins (0 - negative/min, 1 - positive/max).
  @param[in] MaxMargin - Current max margin value.

  @retval The max Rx margin, either MaxMargin or value from stored margins.
**/
U8
MrcCalcMaxRxMargin (
  IN MrcParameters  *const MrcData,
  IN const U8              Channel,
  IN const U8              RankRx,
  IN const U8              byte,
  IN const U8              sign,
  IN U8                    MaxMargin
  )
{
  MrcChannelOut *ChannelOut;
  U8            RxDqsP;
  U8            RxDqsN;
  U8            Start;
  U8            Stop;
  U8            rank;

  //
  // Check for saturation on Rx Timing
  //
  if (RankRx == 0xFF) {
    //
    // If desired for all ranks
    //
    Start = 0;              // Start in rank 0
    Stop = 4;               // Up to 4 ranks
  } else {
    Start = RankRx;
    Stop  = RankRx + 1;
  }

  ChannelOut = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  for (rank = Start; rank < Stop; rank++) {
    if (MrcRankInChannelExist (MrcData, rank, Channel)) {
      RxDqsP = ChannelOut->RxDqsP[rank][byte];
      RxDqsN = ChannelOut->RxDqsN[rank][byte];

      if (sign == 0) {
        if (MaxMargin > RxDqsP) {
          MaxMargin = RxDqsP;
        }

        if (MaxMargin > RxDqsN) {
          MaxMargin = RxDqsN;
        }
      } else {
        if (MaxMargin > 63 - RxDqsP) {
          MaxMargin = 63 - RxDqsP;
        }

        if (MaxMargin > 63 - RxDqsN) {
          MaxMargin = 63 - RxDqsN;
        }
      }
    }
  }

  return MaxMargin;
}

/**
  This function determines if a bit lane[0-7] has seen a pass and a fail in each byte for all channels populated.

  @param[in] MrcData   - Pointer to MRC global data.
  @param[in] chBitmask - Bit mask of channels to consider.
  @param[in] OnePass   - Array of Bit masks marking DQ lanes has had a passing value.
  @param[in] OneFail   - Array of Bit masks marking DQ lanes has had a failing value.

  @retval The AND result of each Channel/byte for OnePass and OneFail.
**/
U8
MrcAndBytes (
  IN MrcParameters *const MrcData,
  IN const U8             chBitmask,
  IN U8                   OnePass[MAX_CHANNEL][MAX_SDRAM_IN_DIMM],
  IN U8                   OneFail[MAX_CHANNEL][MAX_SDRAM_IN_DIMM]
  )
{
  U8  Res;
  U8  Channel;
  U8  byte;

  Res = 0xFF;
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (((MRC_BIT0 << Channel) & chBitmask) == 0) {
      continue;
    }

    for (byte = 0; byte < MrcData->SysOut.Outputs.SdramCount; byte++) {
      Res &= OnePass[Channel][byte];
      Res &= OneFail[Channel][byte];
    }
  }

  return Res;
}

/**
  This function Finds the margin for all channels/all bits. The margin sweep is a parameterized
  Assume REUT test has already been fully setup to run
  This will unscale the results such that future tests start at the correct point
  Uses ChangeMargin function to handle a variety cases (Timing, Voltage, Fan, etc.)

  @param[in]     MrcData    - Include all MRC global data.
  @param[in]     chBitMask  - Channel BIT mask for Channel(s) to work on
  @param[in]     Rank       - Rank to work on
  @param[in,out] marginbit  - used as the return data ( real margin measurement, no 10x)
                              marginbit[ch,byte,bit,sign] = abs(Margin)
                              Note: If param == RdTBit/RdVBit/WrVBit, marginbit is also the starting point
  @param[in,out] marginbyte - provides the starting point on a per byte basis (still 10x)
  @param[in]     param      - defines the margin type
  @param[in]     mode       - allows for different types of modes for margining
                              {Bit0: PhLock (keep all bytes within in ch in phase),
                               Bit1: Ch2Ch Data out of phase (LFSR seed)
                               Bits 15:2: Reserved}
  @param[in]     MaxMargin  - Default Maximum margin

  @retval mrcSuccess if successful, otherwise it returns an error status.
**/
MrcStatus
MrcGetMarginBit (
  IN     MrcParameters *const MrcData,
  IN     U8                   chBitMask,
  IN     U8                   Rank,
  IN OUT U32                  marginbit[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS][MAX_EDGES],
  IN OUT U32                  marginbyte[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN     U8                   param,
  IN     U16                  mode,
  IN     U8                   MaxMargin
  )
{
  const MrcDebug  *Debug;
  MrcOutput       *Outputs;
  MrcStatus       Status;
  U8              Channel;
  U8              Byte;
  U8              bit;
  U8              sign;
  S8              realSign;
  U8              pbyte;
  BOOL            PerCh;
  U8              PerBit;
  U8              SeqLC[4];
  U8              Points2D;
  U8              DoneMask;
  U8              ByteMax;
  U8              SkipWait;
  U8              chPass;
  U8              chFail;
  U8              NoECC;
  U8              AllFail;
  // Set to 1 after ch/byte/bit has a passing point
  U8              OnePass[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  // Set to 1 after ch/byte/bit has a failing point
  U8              OneFail[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U8              ErrByte;
  U8              ErrECC;
  U32             ErrLower;
  U32             ErrUpper;
  U8              MinMargin;
  U32             value0;
  U32             value1;
  U32             v0;
  U32             CMargin[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS];    // Current Margin Point Testing
  U32             ABMargin[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];             // Average Byte Margin
  U32             MinTested[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS];  // Min Margin Point Tested
  U8              PrintPetByte;
  S8              RdTAdjust;
  U32             Offset;
  U32             BitTimePerBit;
  U8              BitMask;
  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT ReutGlobalCtl;

  Status    = mrcSuccess;
  SkipWait  = 0;
  NoECC     = 0;
  Debug     = &MrcData->SysIn.Inputs.Debug;
  Outputs   = &MrcData->SysOut.Outputs;

  //
  // Define Constants
  //
  ByteMax = MaxMargin;

  //
  // Define the correct loopcount for test
  //
  if (Outputs->DQPat == SegmentWDB) {
    SeqLC[0] = Outputs->DQPatLC;
    SeqLC[1] = Outputs->DQPatLC;
    SeqLC[2] = Outputs->DQPatLC + 4;
    SeqLC[3] = Outputs->DQPatLC + 2;
  } else {
    SeqLC[0] = 1;
    SeqLC[1] = 1;
    SeqLC[2] = 1;
    SeqLC[3] = 1;
  }
  //
  // How many points to test
  //
  Points2D = 1 + (param / 16);

  //
  // Define PerByte param for PerBit cases
  //
  if (param == RdTBit) {
    pbyte   = RdT;
    PerBit  = 1;
  } else if (param == WrTBit) {
    pbyte   = WrT;
    PerBit  = 1;
  } else if (param == RdVBit) {
    pbyte   = RdV;
    PerBit  = 1;
  } else {
    pbyte   = 0;
    PerBit  = 0;
  }
  //
  // Print results PerBit or PerByte
  //
  PrintPetByte = (param == RdT || param == WrT || param == RdV);
  //
  // Created for debug purpose
  // Are we using DIMM Vref?  If so, need to use the same Vref across all bytes
  //
  PerCh = ((param == WrFan2) || (param == WrFan3) || (param == WrV) || (mode & 0x1)) && (PerBit == 0);

  //
  // Get Average Byte back to real margin numbers
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (chBitMask & (MRC_BIT0 << Channel)) {
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        ABMargin[Channel][Byte] = (marginbyte[Channel][Byte][0] + marginbyte[Channel][Byte][1]) / 20;
      }
    }
  }
  //
  // Find Left and Right Edges
  //
  for (sign = 0; sign < 2; sign++) {
    realSign = (S8) ((2 * sign) - 1);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n+--MrcGetMarginBit, rsign = %d\n", realSign);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, (PrintPetByte) ? "\nMargin\tBits\n" : "");

    //
    // Initialize variables
    //
    DoneMask = 0xFF;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!(chBitMask & (MRC_BIT0 << Channel))) {
        continue; // This channel is not populated
      }

      MinMargin = 0x7F; // Start with a huge unsigned number
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        //
        // Init arrays to 0
        //
        OnePass[Channel][Byte] = OneFail[Channel][Byte] = 0;

        //
        // Find MaxMargin for this byte
        //
        ByteMax = MaxMargin;
        if (param == RdT) {
          ByteMax = MrcCalcMaxRxMargin (MrcData, Channel, Rank, Byte, sign, MaxMargin);
        }

        CMargin[Channel][Byte][0] = ABMargin[Channel][Byte] - 2; //start from a definite pass for all bytes/bits

        if ((param == RdTBit) || (param == WrTBit)) {
          // Special case for PerBit Timing
          v0 = realSign * (CMargin[Channel][Byte][0] + 0); // Push into failing region
          Status = ChangeMargin (MrcData, pbyte, v0, 0, 0, Channel, Rank, Byte, 0, 0, 0, MrcRegFileStart);
        } else if (param == RdVBit) {
          // Special case for PerBit Voltage
          v0 = realSign * (CMargin[Channel][Byte][0] + 4); // Push into failing region
          Status = ChangeMargin (MrcData, pbyte, v0, 0, 0, Channel, Rank, Byte, 0, 0, 0, MrcRegFileStart);
        }
        //
        // Update the variables for PerBit tracking
        //
        if (PerBit) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            CMargin[Channel][Byte][bit] = marginbit[Channel][Byte][bit][sign];
            //
            // Double check saturation limits
            //
            if (CMargin[Channel][Byte][bit] > MaxMargin) {
              CMargin[Channel][Byte][bit] = MaxMargin;
            }
          }
        }
        //
        // Find MinMargin to start and set marginbyte for the PerCh case
        //
        if (PerCh) {
          if (CMargin[Channel][Byte][0] < MinMargin) {
            MinMargin = (U8) CMargin[Channel][Byte][0];
          }

          CMargin[Channel][Byte][0] = MinMargin;
        }

        for (bit = 0; bit < MAX_BITS; bit++) {
          MinTested[Channel][Byte][bit]       = CMargin[Channel][Byte][bit * PerBit];
          marginbit[Channel][Byte][bit][sign] = CMargin[Channel][Byte][bit * PerBit];
        }
      }
    } // END OF CHANNEL LOOP

    //##########################################################
    // Search algorithm:
    // Walk up until everybody fails.  Then Walk down until everybody passes.
    //##########################################################
    while (MrcAndBytes (MrcData, chBitMask, OnePass, OneFail) != DoneMask) {
      //
      // Walk through all 2D points
      ReutGlobalCtl.Data = 0;
      ReutGlobalCtl.Bits.Global_Clear_Errors = 1;
      MrcWriteCR8 (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, (U8) ReutGlobalCtl.Data); // Clear errors
      for (value1 = 0; value1 < Points2D; value1++) {
        //
        // Set Margin level
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (!(chBitMask & (MRC_BIT0 << Channel))) {
            continue;
          }

          SkipWait = (chBitMask >> (Channel + 1)); // Skip if there are more channels
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            if (PerBit) {
              value0 = 0;
              for (bit = 0; bit < MAX_BITS; bit++) {
                //
                // Per Bit Deskew.  Note: When (sign==1), then CMargin is off by 1.
                // Suppose RdTBit and Right/Left Edge Last Pass @ CMargin = 12, 9
                //    Real Right Edge = (15-12)=3,  Right Edge Moved By (8-3)=5
                //    Real Left Edge = 9,           Left Edge Moved By (9-8) =1
                // Center Movement = (5-1)/2 = +2
                // To get correct answer, need to add +1 to CMargin for Right Edge
                //   ie: Center Moverment = (12+1-9)/2 = +2
                // This error will be corrected at the edge of the function
                // For RdTBit we shift data not strobe.Since we shift the opposite signal, sign is inverted
                //
                if ((param == RdTBit && sign) || ((param != RdTBit) && (sign == 0))) {
                  v0 = (MaxMargin - CMargin[Channel][Byte][bit]);
                } else {
                  v0 = CMargin[Channel][Byte][bit];
                }

                if (v0 > MaxMargin) {
                  v0 = MaxMargin;
                }
                value0 |= (v0 << (4 * bit));
              }
            } else {
              value0 = realSign * CMargin[Channel][Byte][0];
            }
            //
            // EnMultiCast=0, ch,rank,byte,0, UpdateHost=0, SkipWait
            //
            Status = ChangeMargin (
                      MrcData,
                      param,
                      value0,
                      value1,
                      0,
                      Channel,
                      Rank,
                      Byte,
                      0,
                      0,
                      SkipWait,
                      MrcRegFileStart
                      );
          }
        }
        //
        // Run Test
        //
        RunIOTest (MrcData, chBitMask, Outputs->DQPat, SeqLC, (value1 == 0), mode);

        //
        // Check if we have already failed and can stop running
        //
        if (value1 < (U32) (Points2D - 1)) {
          AllFail = 1;
          NoECC   = (Outputs->EccSupport == FALSE);
          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            if (!(chBitMask & (MRC_BIT0 << Channel))) {
              continue;
            }
            Offset = MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG +
              (Channel * (MCHBAR_CH1_CR_REUT_CH_ERR_DATA_STATUS_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG));
            AllFail &= (MrcReadCR (MrcData, Offset) == 0xFFFFFFFF);
            AllFail &= (MrcReadCR (MrcData, Offset + 4) == 0xFFFFFFFF);
            Offset = MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG +
              (
                Channel *
                  (
                    MCHBAR_CH1_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG -
                    MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG
                  )
              );
            AllFail &= (NoECC || ((U8) MrcReadCR (MrcData, Offset) == 0xFF));
          }

          if (AllFail) {
            break;  // break if any error
          }
        }
      }
      //
      // Collect results and Update variables for next point to test
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(chBitMask & (MRC_BIT0 << Channel))) {
          continue;
        }

        // Read Error Results (Assume all reads are 32 bit access
        Offset = MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG +
          (Channel * (MCHBAR_CH1_CR_REUT_CH_ERR_DATA_STATUS_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG));
        ErrLower = MrcReadCR (MrcData, Offset); // Lower 32 bits
        ErrUpper = MrcReadCR (MrcData, Offset + 4); // Upper 32 bits
        Offset = MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG +
          (
            Channel *
              (
                MCHBAR_CH1_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG -
                MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG
              )
          );
        ErrECC = (U8) MrcReadCR (MrcData, Offset);

        chPass = 0xFF;
        chFail = 0xFF;

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          //
          // Extract Errors
          //
          if (Byte < 4) {
            ErrByte = (U8) (ErrLower >> (8 * Byte));
          } else if (Byte < 8) {
            ErrByte = (U8) (ErrUpper >> (8 * (Byte - 4)));
          } else {
            ErrByte = ErrECC;
          }

          ErrByte &= DoneMask;
#ifdef MRC_DEBUG_PRINT
          if (param == WrTBit) {
            Offset = DDRDATA0CH0_CR_TXPERBITRANK0_REG +
              ((DDRDATA0CH1_CR_TXPERBITRANK0_REG - DDRDATA0CH0_CR_TXPERBITRANK0_REG) * Channel) +
              ((DDRDATA0CH0_CR_TXPERBITRANK1_REG - DDRDATA0CH0_CR_TXPERBITRANK0_REG) * Rank)+
              ((DDRDATA1CH0_CR_TXPERBITRANK0_REG - DDRDATA0CH0_CR_TXPERBITRANK0_REG) * Byte);
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "0x%08X", MrcReadCR (MrcData, Offset));
          } else if (param == RdTBit) {
            Offset = DDRDATA0CH0_CR_RXPERBITRANK0_REG +
              ((DDRDATA0CH1_CR_RXPERBITRANK0_REG - DDRDATA0CH0_CR_RXPERBITRANK0_REG) * Channel) +
              ((DDRDATA0CH0_CR_RXPERBITRANK1_REG - DDRDATA0CH0_CR_RXPERBITRANK0_REG) * Rank)+
              ((DDRDATA1CH0_CR_RXPERBITRANK0_REG - DDRDATA0CH0_CR_RXPERBITRANK0_REG) * Byte);
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "0x%08X", MrcReadCR (MrcData, Offset));
          } else if (param == RdVBit) {
            Offset = DDRDATA0CH0_CR_RXOFFSETVDQ_REG +
              ((DDRDATA0CH1_CR_RXOFFSETVDQ_REG - DDRDATA0CH0_CR_RXOFFSETVDQ_REG) * Channel) +
              ((DDRDATA1CH0_CR_RXOFFSETVDQ_REG - DDRDATA0CH0_CR_RXOFFSETVDQ_REG) * Byte);
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "0x%08X", MrcReadCR (MrcData, Offset));
          } else if (param == WrT || param == RdT || param == RdV) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "% 2d", CMargin[Channel][Byte][0]);
          }
#endif // MRC_DEBUG_PRINT

          for (bit = 0; bit < MAX_BITS; bit++) {
            BitMask       = MRC_BIT0 << bit;
            BitTimePerBit = bit * PerBit;
            //
            // Skip if this Bit Group is done
            //
            if (OnePass[Channel][Byte] & OneFail[Channel][Byte] & (BitMask)) {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "   $");
              continue;
            }
            //
            // Update variables for fail
            //
            if (ErrByte & (BitMask)) {
              OneFail[Channel][Byte] |= (BitMask);

              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "   #");

              //
              // Handle Speckles
              //
              if (marginbit[Channel][Byte][bit][sign] >= CMargin[Channel][Byte][BitTimePerBit]) {
                marginbit[Channel][Byte][bit][sign] = CMargin[Channel][Byte][BitTimePerBit] - 1;
                OnePass[Channel][Byte] &= ~(BitMask);
              }
              //
              // Update variables for pass
              //
            } else {
              OnePass[Channel][Byte] |= (BitMask);

              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "   .");

              if (marginbit[Channel][Byte][bit][sign] < CMargin[Channel][Byte][BitTimePerBit]) {
                marginbit[Channel][Byte][bit][sign] = CMargin[Channel][Byte][BitTimePerBit];
              }
            }
          }
          //
          // FIND MAX Saturation limit
          //
          ByteMax = MaxMargin;
          if (param == RdT) {
            ByteMax = MrcCalcMaxRxMargin (MrcData, Channel, Rank, Byte, sign, MaxMargin);

          }
          //
          // HANDLE Saturation
          //
          if (PerBit) {
            for (bit = 0; bit < MAX_BITS; bit++) {
              BitMask = MRC_BIT0 << bit;
              if (CMargin[Channel][Byte][bit] >= ByteMax) {
                OneFail[Channel][Byte] |= (BitMask);
              }

              if (CMargin[Channel][Byte][bit] == 0) {
                OnePass[Channel][Byte] |= (BitMask);
              }
            }
          } else {
            if (CMargin[Channel][Byte][0] >= ByteMax) {
              OneFail[Channel][Byte] = DoneMask;
            }

            if (CMargin[Channel][Byte][0] == 0) {
              OnePass[Channel][Byte] = DoneMask;
            }
          }
          //
          // DECIDE WHAT TO TEST NEXT
          // If PerByte, Do this within the for byte loop
          //
          chPass &= OnePass[Channel][Byte];
          chFail &= OneFail[Channel][Byte];

          if (PerCh == FALSE) {
            if (PerBit) {
              for (bit = 0; bit < MAX_BITS; bit++) {
                BitMask = MRC_BIT0 << bit;
                //
                // Skip if this Bit Group is done
                //
                if (OnePass[Channel][Byte] & OneFail[Channel][Byte] & (BitMask)) {
                  continue;
                }

                if ((OneFail[Channel][Byte] & BitMask) == 0) {
                  CMargin[Channel][Byte][bit] += 1;
                } else if ((OnePass[Channel][Byte] & BitMask) == 0) {
                  MinTested[Channel][Byte][bit] -= 1;
                  CMargin[Channel][Byte][bit] = MinTested[Channel][Byte][bit];
                } else {
                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "WARNING! Can't have both: OnePass and OneFail Not Done\n");
                }
              }
            } else {
              //
              // PerCh
              // Skip if this Byte Group is done
              //
              if ((OnePass[Channel][Byte] & OneFail[Channel][Byte]) == DoneMask) {
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " ");
                continue;
              }

              if (OneFail[Channel][Byte] != DoneMask) {
                CMargin[Channel][Byte][0] += 1;
              } else if (OnePass[Channel][Byte] != DoneMask) {
                MinTested[Channel][Byte][0] -= 1;
                CMargin[Channel][Byte][0] = MinTested[Channel][Byte][0];
              }
            }
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " ");
        }
        //
        // END OF BYTE LOOP
        // DECIDE WHAT TO TEST NEXT
        // If PerCh, Do this within the for ch loop
        //
        if (PerCh == TRUE) {
          if ((chPass & chFail) == DoneMask) {
            continue;
          }

          if (chFail != DoneMask) {
            CMargin[Channel][0][0] += 1;
          } else {
            MinTested[Channel][0][0] -= 1;
            CMargin[Channel][0][0] = MinTested[Channel][0][0];
          }
          //
          // All bytes must use the same margin point
          //
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            CMargin[Channel][Byte][0] = CMargin[Channel][0][0];
          }
        }
      }
      //
      // END OF CHANNEL LOOP
      //
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
    }
    //
    // END OF WHILE LOOP
    // Update MarginByte to have the correct result
    //
    if (PerBit == 0) {
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (chBitMask & (MRC_BIT0 << Channel)) {

          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            MinMargin = 0x7F; // Start with a huge unsigned number
            for (bit = 0; bit < MAX_BITS; bit++) {
              if (MinMargin > marginbit[Channel][Byte][bit][sign]) {
                MinMargin = (U8) marginbit[Channel][Byte][bit][sign];
              }
            }

            marginbyte[Channel][Byte][sign] = MinMargin * 10;
            //
            // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+--marginbyte = MinMargin*10 = %d\n", MinMargin*10);
            //
          }
        }
      }
    } else {
      //
      // Bit Margins
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(chBitMask & (1 << Channel))) {
          continue;
        }

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            if ((param == RdTBit && sign) || ((param != RdTBit) && (sign == 0))) {
              marginbit[Channel][Byte][bit][sign] = MaxMargin - marginbit[Channel][Byte][bit][sign];
            }
          }
        }
      }
      //
      // Cleanup after test
      //
      Status = ChangeMargin (MrcData, pbyte, 0, 0, 1, 0, Rank, 0, 0, 0, 0, MrcRegFileCurrent);
    }
  }
  //
  // END OF SIGN LOOP
  // Clean up after step
  // @todo Restore perBit to last saved value
  //
  value0 = (PerBit == 1 ? 0x88888888 : 0);
  Status = ChangeMargin (MrcData, param, value0, 0, 1, 0, Rank, 0, 0, 0, 0, MrcRegFileCurrent);

  //
  // Correct for 1 tick error in Per Bit Deskew right edge
  //
  RdTAdjust = 1;
#ifdef MRC_DEBUG_PRINT
  if (PerBit == 1) {
    if (param == RdTBit) {
      RdTAdjust = -1;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nGains     ");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (chBitMask & (MRC_BIT0 << Channel)) {

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              " %d %d",
              ((RdTAdjust) * (8 - marginbit[Channel][Byte][bit][0])),
              ((RdTAdjust) * (marginbit[Channel][Byte][bit][1] - 8))
              );
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "           ");
        }
      }
    }
  } else {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nCt");

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if ((chBitMask & (MRC_BIT0 << Channel))) {
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              "%4d",
              (S8) (marginbit[Channel][Byte][bit][1] - marginbit[Channel][Byte][bit][0]) / 2
              );
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "   ");
        }
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
#endif // MRC_DEBUG_PRINT

  return Status;
}

/**
  Assume REUT test has already been fully setup to run
  Finds the margin for all channels/all bytes
  The margin sweep is parameterized
  Uses ChangeMargin function to handle a variety of cases (Timing, Voltage, Fan, etc.)
  mode allows for different types of modes for margining:
    mode is {Bit0: PhLock (keep all bytes within in ch in phase),
             Bit1: Ch2Ch Data out of phase (LFSR seed), Bit 15:2: Reserved}
  marginByte is used as the starting point for the search (10x the actual margin)
  marginch returns the results (10x the actual margin)
  Interior: Search inside marginch limits, enabling multiple calls with different setups
  To reduce repeatibility noise, the returned margins is actually a BER extrapolation

  @param[in]     MrcData    - The global MrcData
  @param[in,out] marginByte - Data structure with the latest margin results
  @param[in]     chBitmask  - Bit mask of present channels
  @param[in]     Rank       - Rank to change margins for
  @param[in]     RankRx     - Ranks for Rx margin
  @param[in]     param      - parameter to get margins for
  @param[in]     mode       - allows for different types of modes for margining:
  @param[in]     BMap       - Byte mapping to configure error counter control register
  @param[in]     EnBER      - Enable BER extrapolation calculations
  @param[in]     MaxMargin  - Max Margin allowed for the parameter
  @param[in]     Interior   - Search inside marginch limits, enabling multiple calls with different setups
  @param[in,out] BERStats   - Bit Error Rate Statistics.

  @retval mrcSuccess if successful, otherwise returns an error status.
**/
MrcStatus
MrcGetBERMarginByte (
  IN     MrcParameters * const MrcData,
  IN OUT U32             marginByte[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN     U8              chBitmask,
  IN     U8              Rank,
  IN     U8              RankRx,
  IN     U8              param,
  IN     U16             mode,
  IN     U8              *BMap,
  IN     U8              EnBER,
  IN     U8              MaxMargin,
  IN     U8              Interior,
  IN OUT U32             *BERStats
  )
{
  const MrcDebug   *Debug;
  MrcOutput        *Outputs;
  MrcChannelOut    *ChannelOut;
  MrcControllerOut *ControllerOut;
  U32              *MarginByteTemp;
  MrcStatus        Status;
  U8               ResultType;
  U8               sign;
  S8               rSign;
  U8               SeqLC[4];
  U8               Points2D;
  U8               Channel;
  U8               byte;
  U8               chByte;
  U8               SkipWait;
  U8               byteMax;
  U8               Margin;
  U16              DoneMask;
  // Set to 1 after ch has 2 passing points
  U16              TwoPass[MAX_CHANNEL];
  // Set to 1 after ch has 2 failing points
  U16              TwoFail[MAX_CHANNEL];
  U16              res;
  U16              BitMask;
  S8               Delta;
  BOOL             Done;
  BOOL             allFail;
  BOOL             PerCh;
  U32              value0;
  U32              value1;
  U32              tmp;
  U32              ErrCount;
  U8               LastPassVref[MAX_CHANNEL][MAX_SDRAM_IN_DIMM]; // Last passing Vref
  U32              InitValue[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];    // Initial value from margin byte
  U8               MaxTested[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];    // Highest Vref Point Tested
  U8               MinTested[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];    // Lowest Vref Point Tested
  // Log8(Error count) at different Vref Points. 32 bit number that is broken in 4 bytes
  // [LastPass+2, LastPass+1, LastPass, LastPass-1]
  U32              Errors[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U32              Offset;

  MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_STRUCT                  ReutChErrCounterCtl;
  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT                         ReutGlobalCtl;
  MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_STRUCT               ReutChErrCounterStatus;
  MCHBAR_CH0_CR_SC_IO_LATENCY_STRUCT                              ScIoLatency;


  Status        = mrcSuccess;
  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  chByte        = 0;
  Points2D      = (param / RdFan2) + 1;
  ResultType    = GetMarginResultType (param);

  //
  // Are we using DIMM Vref?
  //
  PerCh = (param == WrFan2 || param == WrFan3 || param == WrV || ((mode & 1) == 1));   // WrFan not defined

  //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+--->MrcGetBERMarginByte, Points2D: %d, PerCh: %d --\n", Points2D,PerCh);

  DoneMask = (MRC_BIT0 << Outputs->SdramCount) - 1; // 0x1FF or 0xFF

  if (Outputs->DQPat == SegmentWDB) {
    SeqLC[0] = Outputs->DQPatLC;
    SeqLC[1] = Outputs->DQPatLC;
    SeqLC[2] = Outputs->DQPatLC + 4;
    SeqLC[3] = Outputs->DQPatLC + 2;
  } else {
    SeqLC[0] = 1;
    SeqLC[1] = 1;
    SeqLC[2] = 1;
    SeqLC[3] = 1;
  }
  //
  // Run through margin test
  //
  for (sign = 0; sign < 2; sign++) {
    rSign = (S8) ((2 * sign) - 1);
    //
    // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+--MrcGetBERMarginByte, rsign = %d\n", rSign);
    //
    MrcOemMemorySet ((U8 *) TwoPass, 0, sizeof (TwoPass));
    MrcOemMemorySet ((U8 *) TwoFail, 0, sizeof (TwoFail));

    //
    // Initialize variables
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!((MRC_BIT0 << Channel) & chBitmask)) {
        TwoPass[Channel] = DoneMask;
        TwoFail[Channel] = DoneMask;
        continue;
      }

      MinTested[Channel][0] = 0x7F;
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        LastPassVref[Channel][byte] = 0x7F; // Start with a huge unsigned numer - 128
        Errors[Channel][byte] = 0;

        //
        // Find MaxMargin for this byte
        //
        byteMax = MaxMargin;
        if (param == RdT) {
          byteMax = MrcCalcMaxRxMargin (MrcData, Channel, RankRx, byte, sign, MaxMargin);
        }
        //
        // Scale MarginResult back to real margin numbers.  Set Max/MinTested
        //
        MarginByteTemp  = &marginByte[ResultType][Rank][Channel][byte][sign];
        *MarginByteTemp = *MarginByteTemp / 10;
        if (*MarginByteTemp > byteMax) {
          *MarginByteTemp = byteMax;
        }

        InitValue[Channel][byte] = *MarginByteTemp;

        //
        // if Per Ch, find MinMargin to start.  Else set margin for that Byte
        //
        if (PerCh == TRUE) {
          if (*MarginByteTemp < MinTested[Channel][0]) {
            MaxTested[Channel][0] = (U8) *MarginByteTemp;
            MinTested[Channel][0] = (U8) *MarginByteTemp;
          }
        } else {
          MaxTested[Channel][byte] = (U8) *MarginByteTemp;
          MinTested[Channel][byte] = (U8) *MarginByteTemp;
        }
        //
        // Setup REUT Error Counters to count errors per byte lane
        // Count Errors on a particular Byte Group BITS 8:7 = 10b
        //
        ReutChErrCounterCtl.Data = 0;
        ReutChErrCounterCtl.Bits.Counter_Pointer = BMap[byte];
        ReutChErrCounterCtl.Bits.Counter_Control = 2;
        Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG +
          ((MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_CTL_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG) * Channel) +
          ((MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_1_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG) * byte);
        MrcWriteCR (MrcData, Offset, ReutChErrCounterCtl.Data);
      }
      //
      // Set MarginResult for the PerCh case
      //
      if (PerCh == TRUE) {
        for (byte = 0; byte < Outputs->SdramCount; byte++) {
          marginByte[ResultType][Rank][Channel][byte][sign] = MinTested[Channel][0];
        }
      }
    }
    //
    // Search algorithm:
    // If start with a passing point, walk to hit 2 failing points
    //    Return as needed to hit a second passing point
    // If start with a failing point, walk to hit 2 passing points
    //    Return as needed to hit a second failing point
    // Keep testing until all ch/bytes find 2 passes and 2 fails
    //
    Done = FALSE;
    do {
      //
      // Walk through all 2D points
      //
      ReutGlobalCtl.Data = 0;
      ReutGlobalCtl.Bits.Global_Clear_Errors = 1;
      MrcWriteCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, ReutGlobalCtl.Data); // Clear errors
      for (value1 = 0; value1 < Points2D; value1++) {
        //
        // Set Vref level
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (!((MRC_BIT0 << Channel) & chBitmask)) {
            continue;
          }

          SkipWait = (chBitmask >> (Channel + 1)); // Skip if there are more channels
          for (byte = 0; byte < Outputs->SdramCount; byte++) {
            value0 = rSign * marginByte[ResultType][Rank][Channel][byte][sign];
            //
            // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+----->Value0 is %d, Value1 is %d\n", (S32)value0, value1);
            //
            Status = ChangeMargin (
                      MrcData,
                      param,
                      value0,
                      value1,
                      0,
                      Channel,
                      Rank,
                      byte,
                      0,
                      0,
                      SkipWait,
                      MrcRegFileStart
                      );
            if ((PerCh) && ((mode & 1) == 0)) {
              //
              // Only Byte 7 on Channel 1 is needed to update Wr DIMM Vref - Taken care of inside ChangeMargin routine
              //
              break;
            }
          }
        }
        //
        // Run Test
        //
        RunIOTest (MrcData, chBitmask, Outputs->DQPat, SeqLC, (value1 == 0), mode);

        //
        // What is the idea behind this? What if all byte groups failed?
        //
        if (EnBER == 0 && value1 < (U32) (Points2D - 1)) {
          allFail = TRUE;
          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            if (!((MRC_BIT0 << Channel) & chBitmask)) {
              continue;
            }
            //
            // Read out per byte error results
            //
            Offset = 4 + MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG +
              ((MCHBAR_CH1_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG -
              MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG) * Channel);
            res = (U16) MrcReadCR (MrcData, Offset);
            if ((res & DoneMask) != DoneMask) {
              allFail = FALSE;
            }
          }

          if (allFail == TRUE) {
            break;
          }
        }
      }
      //
      // Collect results and Update variables for next point to test
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!((MRC_BIT0 << Channel) & chBitmask)) {
          continue;
        }

        for (byte = 0; byte < Outputs->SdramCount; byte++) {
          BitMask = MRC_BIT0 << byte;
          //
          // Skip if this Byte Group is done
          //
          if ((TwoPass[Channel] & TwoFail[Channel] & (BitMask)) != 0) {
            continue;
          }
          //
          // Handle PerCh vs. PerByte variable differences
          //
          chByte = (PerCh == TRUE ? 0 : byte);

          //
          // Read Error Count
          //
          Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG +
            ((MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG) * Channel) +
            ((MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_1_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG) * byte);
          ReutChErrCounterStatus.Data = MrcReadCR (MrcData, Offset);
          ErrCount = ReutChErrCounterStatus.Bits.Counter_Status;
          Margin = (U8) marginByte[ResultType][Rank][Channel][byte][sign];
          Delta = (Margin - LastPassVref[Channel][byte]);
          //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+----->channel: %d, Error count:%x.\n", Channel, ErrCount);
          //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+----->Margin:%d, LastPassVref:%d, delta:%d. sign:%d\n", Margin, LastPassVref[Channel][byte], (S8) Delta, sign);

          // Update Pass/Fail Variables:
          //
          if (ErrCount == 0 && Margin == MaxTested[Channel][chByte]) {
            //
            // Passing while walking up
            //
            if (Delta < 0) {
              //
              // First passing point
              // @todo: Next line should be changed to return failure. Code should never hang.
              //
              MRC_ASSERT (
                MinTested[Channel][chByte] == MaxTested[Channel][chByte],
                Debug,
                "Error: MaxTested < LastPass after first point"
                );
              LastPassVref[Channel][byte] = Margin;
            } else if (Delta == 1) {
              //
              // Normal, walk to fail
              //
              Errors[Channel][byte]       = MrcBitShift (Errors[Channel][byte], -8 * Delta) & BER_ERROR_MASK;
              LastPassVref[Channel][byte] = Margin;
              TwoPass[Channel] |= (BitMask);
            } else if (Delta == 2) {
              //
              // Speckling in response, Consider point as error
              //
              Errors[Channel][byte] = MrcBitSwap (Errors[Channel][byte], MrcLog8 (ErrCount), 24, 8);
              TwoFail[Channel] |= (BitMask);
            } else {
              //
              // @todo: Next line should be changed to return failure. Code should never hang.
              //
              MRC_ASSERT (
                FALSE,
                Debug,
                "Error: Tested point twice or Tested >2 above LastPass (Passing while walking up)"
                );
            }
          } else if (ErrCount == 0 && Margin == MinTested[Channel][chByte]) {
            //
            // Skip if this byte is already done
            //
            if ((TwoPass[Channel] & (BitMask)) != 0) {
              continue;
            }

            if (Delta == -1) {
              //
              // Finding 2nd pass
              //
              Errors[Channel][byte] = MrcBitSwap (Errors[Channel][byte], 0, 0, 8);
              TwoPass[Channel] |= (BitMask);
            } else {
              //
              // 1st passing point
              // Don't shift Errors.  Fail points already assumed correct LastPass
              //
              LastPassVref[Channel][byte] = Margin;
              TwoPass[Channel] &= ~(BitMask);
            }
          } else if (ErrCount > 0 && Margin == MaxTested[Channel][chByte]) {
            //
            // Failing while walking up
            // @todo: Next line should be changed to return failure. Code should never hang.
            //
            MRC_ASSERT (Delta <= 2, Debug, "Error: Tested >2 above LastPass (Failing while walking up)");
            if (Delta < 2) {
              //
              // first failing point
              //
              Errors[Channel][byte] = MrcBitSwap (Errors[Channel][byte], MrcLog8 (ErrCount), 16, 8);
              TwoFail[Channel] &= ~(BitMask);
            } else if (Delta == 2) {
              //
              // 2nd failing point
              //
              Errors[Channel][byte] = MrcBitSwap (Errors[Channel][byte], MrcLog8 (ErrCount), 24, 8);
              TwoFail[Channel] |= (BitMask);
            }
          } else if (ErrCount > 0 && Margin == MinTested[Channel][chByte]) {
            //
            // Failing while walking down
            //
            if (LastPassVref[Channel][byte] < 0xFF && Delta <= 0) {
              //
              // Adjust LastPassVref and Error count to be below this failure point.
              //
              Errors[Channel][byte]       = MrcBitSwap (Errors[Channel][byte], MrcLog8 (ErrCount), 8 * (Delta + 1), 8);
              Errors[Channel][byte]       = MrcBitShift (Errors[Channel][byte], 8 * (1 - Delta));
              LastPassVref[Channel][byte] = Margin - 1;
            } else {
              tmp                   = ((Errors[Channel][byte] & 0xFF0000) << 8) + MrcLog8 (ErrCount);
              Errors[Channel][byte] = MrcBitSwap (Errors[Channel][byte], tmp, 16, 16);
              //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"Unexpected case for channel: %d, delta: %d.\n", Channel, Delta);
            }

            if (MinTested[Channel][chByte] < MaxTested[Channel][chByte]) {
              TwoFail[Channel] |= (BitMask);
            }

            if (Delta <= 0) {
              TwoPass[Channel] &= ~(BitMask);
            }
          } else {
            //
            // @todo: Next line should be changed to return failure. Code should never hang.
            //
            MRC_ASSERT (FALSE, Debug, "Error: Testing points other than Max/MinTested");
          }
          //
          // FIND MAX Saturation limit
          //
          byteMax = MaxMargin;
          if (param == RdT) {
            byteMax = MrcCalcMaxRxMargin (MrcData, Channel, RankRx, byte, sign, MaxMargin);
          }

          if (Interior && InitValue[Channel][byte] == Margin) {
            byteMax = Margin;
          }
          //
          // HANDLE MAX Saturation
          //
          if (Margin == byteMax) {
            TwoFail[Channel] |= (BitMask);
          }

          if (ErrCount == 0 && byteMax == LastPassVref[Channel][byte] && (TwoPass[Channel] & (BitMask)) != 0) {
            Errors[Channel][byte] = MrcBitSwap (Errors[Channel][byte], 0xFFFE, 16, 16);
          }
          //
          // HANDLE MIN Saturation
          //
          if (Margin == 0) {
            TwoPass[Channel] |= (BitMask);
            if (ErrCount > 0) {
              TwoFail[Channel] |= (BitMask);
              LastPassVref[Channel][byte] = 0;
              Errors[Channel][byte] = MrcBitSwap (
                                        Errors[Channel][byte],
                                        (BER_LOG_TARGET << 8) + BER_LOG_TARGET,
                                        16,
                                        16
                                        );
            }
          }
          //
          // DECIDE WHAT TO TEST NEXT
          // If In PerByte, Do this within the for byte loop
          //
          if (PerCh == FALSE) {
            //
            // Skip if this Byte Group is done
            //
            if ((TwoPass[Channel] & TwoFail[Channel] & (BitMask)) != 0) {
              continue;
            }

            if (ErrCount == 0) {
              if ((TwoFail[Channel] & (BitMask)) == 0) {
                //
                // Count up to find 2 fails
                //
                marginByte[ResultType][Rank][Channel][byte][sign] = ++MaxTested[Channel][chByte];
              } else {
                //
                // Count down to find 2 passes
                //
                marginByte[ResultType][Rank][Channel][byte][sign] = --MinTested[Channel][chByte];
              }
            } else {
              if ((TwoPass[Channel] & (BitMask)) == 0) {
                marginByte[ResultType][Rank][Channel][byte][sign] = --MinTested[Channel][chByte];
              } else {
                marginByte[ResultType][Rank][Channel][byte][sign] = ++MaxTested[Channel][chByte];
              }
            }
          }
        }
        //
        // DECIDE WHAT TO TEST NEXT
        // If In PerCh, Do this within the for ch loop
        //
        if (PerCh == TRUE) {
          if ((TwoPass[Channel] & TwoFail[Channel]) == DoneMask) {
            continue;
          }

          if (TwoPass[Channel] != DoneMask) {
            marginByte[ResultType][Rank][Channel][0][sign] = --MinTested[Channel][chByte];
          } else {
            marginByte[ResultType][Rank][Channel][0][sign] = ++MaxTested[Channel][chByte];
          }
          //
          // All bytes must use the same margin point
          //
          for (byte = 0; byte < Outputs->SdramCount; byte++) {
            marginByte[ResultType][Rank][Channel][byte][sign] = marginByte[ResultType][Rank][Channel][0][sign];
          }
        }
      }
      //
      // check if we are done
      //
      Done = TRUE;
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if ((TwoPass[Channel] & DoneMask) != DoneMask || (TwoFail[Channel] & DoneMask) != DoneMask) {
          Done = FALSE;
          break;
        }
      }
    } while (Done == FALSE);

    //
    // Calculate the effective margin
    // Update MarginResult with extroploated BER Margin
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (((MRC_BIT0 << Channel) & chBitmask) == 0) {
        continue;
      }

      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+----->marginByte[Ch %d, Byte%d, Sign %d] is: %d\n", Channel, byte, sign,  marginByte[ResultType][Rank][Channel][byte][sign]);
        if (EnBER) {
          marginByte[ResultType][Rank][Channel][byte][sign] = interpolateVref (
                                                                LastPassVref[Channel][byte],
                                                                (Errors[Channel][byte] >> 16) & 0xFF,
                                                                (Errors[Channel][byte] >> 24) & 0xFF,
                                                                BER_LOG_TARGET,
                                                                BERStats
                                                                );
          // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+----->BERmarginByte[Ch %d, Byte%d, Sign %d] is: %d\n", Channel, byte, sign,  marginByte[ResultType][Rank][Channel][byte][sign]);
        } else {
          marginByte[ResultType][Rank][Channel][byte][sign] = 10 * LastPassVref[Channel][byte];
          //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+----->marginByte[Ch %d, Byte%d, Sign %d] is: %d\n", Channel, byte, sign,  marginByte[ResultType][Rank][Channel][byte][sign]);
        }
      }
    }
  }
  //
  // Clean up after step
  //
  if (param == RcvEnaX) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (((MRC_BIT0 << Channel) & chBitmask) == 0) {
        continue;
      }
      ChannelOut  = &ControllerOut->Channel[Channel];
      for (byte = 0; byte < Outputs->SdramCount; byte++) {
        UpdateRxT (MrcData, Channel, Rank, byte, 0xff, 0);
        Offset = MCHBAR_CH0_CR_SC_IO_LATENCY_REG +
          ((MCHBAR_CH1_CR_SC_IO_LATENCY_REG - MCHBAR_CH0_CR_SC_IO_LATENCY_REG) * Channel);
        ScIoLatency.Data           = MrcReadCR (MrcData, Offset);
        ScIoLatency.Bits.RT_IOCOMP = MCHBAR_CH0_CR_SC_IO_LATENCY_RT_IOCOMP_MAX & ChannelOut->RTIoComp;
        MrcWriteCR (MrcData, Offset, ScIoLatency.Data);
      }
    }
  }
  Status = ChangeMargin (MrcData, param, 0, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileCurrent);
  for (byte = 0; byte < Outputs->SdramCount; byte++) {
    Offset = MCSCHEDS_CR_REUT_CH_ERR_COUNTER_CTL_0_REG +
      ((MCSCHEDS_CR_REUT_CH_ERR_COUNTER_CTL_1_REG - MCSCHEDS_CR_REUT_CH_ERR_COUNTER_CTL_0_REG) * byte);
    MrcWriteCrMulticast (MrcData, Offset, 0);
  }

  return Status;
}

/**
  Assume REUT test has already been fully setup to run
  Finds the margin for all channels/all bytes
  The margin sweep is parameterized
  Uses ChangeMargin function to handle a variety of cases (Timing, Voltage, Fan, etc.)
  mode allows for different types of modes for margining:
  mode is {Bit0: PhLock (keep all bytes within in ch in phase),
  Bit1: Ch2Ch Data out of phase (LFSR seed), Bit 15:2: Reserved}
  marginCh is used as the starting point for the search (10x the actual margin)
  marginch returns the results (10x the actual margin)
  Interior: Search inside marginch limits, enabling multiple calls with different setups
  To reduce repeatibility noise, the returned margins is actually a BER extrapolation

  @param[in]     MrcData   - The global MrcData
  @param[in,out] marginCh  - Data structure with the latest margin results
  @param[in]     chBitmask - Bit mask of present channels
  @param[in]     RankRx    - Ranks for Rx margin
  @param[in]     Rank      - Rank to change margins for
  @param[in]     param     - parameter to get margins for
  @param[in]     mode      - allows for different types of modes for margining:
  @param[in]     EnBER     - Enable BER extrapolation calculations
  @param[in]     MaxMargin - Max Margin allowed for the parameter
  @param[in]     Interior  - Search inside marginch limits, enabling multiple calls with different setups
  @param[in,out] BERStats  - Bit Error Rate Statistics.

  @retval mrcSuccess if successful, otherwise returns an error status.
**/
MrcStatus
MrcGetBERMarginCh (
  IN     MrcParameters   *MrcData,
  IN     U32             marginCh[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN OUT U8              chBitmask,
  IN     U8              RankRx,
  IN     U8              Rank,
  IN     U8              param,
  IN     U16             mode,
  IN     U8              EnBER,
  IN     U8              MaxMargin,
  IN     U8              Interior,
  IN OUT U32             *BERStats
  )
{
  const MrcDebug  *Debug;
  MrcOutput       *Outputs;
  MrcStatus       Status;
  U8              ResultType;
  U8              sign;
  S8              rSign;
  U8              SeqLC[4];
  U8              Points2D;
  U8              Channel;
  U8              byte;
  U8              SkipWait;
  U8              byteMax[MAX_CHANNEL];
  U8              Margin;
  // Set to 1 after ch has 2 passing points
  U16             TwoPass[MAX_CHANNEL];
  // Set to 1 after ch has 2 failing points
  U8              TwoFail[MAX_CHANNEL];
  S8              Delta;
  BOOL            Done;
  BOOL            DimmVrefParam;
  U32             value0;
  U32             value1;
  U32             tmp;
  U32             chError;
  U32             ErrCount;
  U8              LastPassVref[MAX_CHANNEL];  // Last passing Vref
  U8              MaxTested[MAX_CHANNEL];     // Highest Vref Point Tested
  U8              MinTested[MAX_CHANNEL];     // Lowest Vref Point Tested
  // Log8(Error count) at different Vref Points. 32 bit number that is broken in 4 bytes
  // [LastPass+2, LastPass+1, LastPass, LastPass-1]
  U32             Errors[MAX_CHANNEL];
  U32             Offset;
  BOOL            PerMc;
  U8              McChannel;

  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT           ReutGlobalCtl;
  MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_STRUCT ReutChErrCounterStatus;

  Debug      = &MrcData->SysIn.Inputs.Debug;
  Outputs    = &MrcData->SysOut.Outputs;
  Status     = mrcSuccess;
  ResultType = GetMarginResultType(param);
  Points2D   = (param / 16) + 1;        // 2 for Fan2 and 3 for Fan3
  McChannel  = 0;

  if (Outputs->DQPat == SegmentWDB) {
    SeqLC[0] = Outputs->DQPatLC;
    SeqLC[1] = Outputs->DQPatLC;
    SeqLC[2] = Outputs->DQPatLC + 4;
    SeqLC[3] = Outputs->DQPatLC + 2;
  } else {
    SeqLC[0] = 1;
    SeqLC[1] = 1;
    SeqLC[2] = 1;
    SeqLC[3] = 1;
  }
  //
  // Make sure we cover all DIMM Vref cases
  //
  DimmVrefParam = (param == WrFan2 || param == WrFan3 || param == WrV );   // WrFan not defined
  PerMc         = (param == CmdV) && (MrcCountBitsEqOne (chBitmask) >= 2);

  //
  // Run through margin test
  //
  for (sign = 0; sign < 2; sign++) {
    rSign = (S8) ((2 * sign) - 1);
    //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+----->rsign: %d \n", rSign);

    MrcOemMemorySet ((U8 *) TwoPass, 0, sizeof (TwoPass));
    MrcOemMemorySet ((U8 *) TwoFail, 0, sizeof (TwoFail));

    //
    // Initialize variables
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      //
      // Set default of all variables
      //
      byteMax[Channel]      = MaxMargin;
      LastPassVref[Channel] = 0x7F;  // Start with a huge unsigned numer - 128
      Errors[Channel]       = 0;
      MinTested[Channel]    = 0;
      MaxTested[Channel]    = 0;

      if (((MRC_BIT0 << Channel) & chBitmask) == 0) {
        TwoPass[Channel] = 1;
        TwoFail[Channel] = 1;
        continue;
      }
      //
      // Find MaxMargin for this channel
      //
      if (param == RdT) {
        for (byte = 0; byte < Outputs->SdramCount; byte++) {
          byteMax[Channel] = MrcCalcMaxRxMargin (MrcData, Channel, RankRx, byte, sign, byteMax[Channel]);
        }
      }
      //
      // Scale back variables to normal margins and check saturation
      //
      marginCh[ResultType][Rank][Channel][0][sign] = marginCh[ResultType][Rank][Channel][0][sign] / 10;
      if (marginCh[ResultType][Rank][Channel][0][sign] > byteMax[Channel]) {
        marginCh[ResultType][Rank][Channel][0][sign] = byteMax[Channel];
      }
      //
      // If PerMC, all channels should start with the lowest margin across all the channel
      //
      if (PerMc) {
        if (marginCh[ResultType][Rank][McChannel][0][sign] > marginCh[ResultType][Rank][Channel][0][sign]) {
          marginCh[ResultType][Rank][McChannel][0][sign] = marginCh[ResultType][Rank][Channel][0][sign];
        }
      }

      MinTested[Channel]  = (U8) marginCh[ResultType][Rank][Channel][0][sign];
      MaxTested[Channel]  = MinTested[Channel];

      //
      // Setup REUT Error Counters to count errors per channel
      //
      Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG +
        ((MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_CTL_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG) * Channel);
      MrcWriteCR (MrcData, Offset, 0);
    }
    //
    // If PerMC, set all channels to use margin associated with mcChannel = 0
    //
    if (PerMc) {
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (((MRC_BIT0 << Channel) & chBitmask) == 0) {
          continue;
        }
        marginCh[ResultType][Rank][Channel][0][sign] = marginCh[ResultType][Rank][McChannel][0][sign];
        MinTested[Channel] = (U8) marginCh[ResultType][Rank][McChannel][0][sign];
        MaxTested[Channel] = MinTested[Channel];
      }
    }
    //
    // Search algorithm:
    // If start with a passing point, walk to hit 2 failing points
    //    Return as needed to hit a second passing point
    // If start with a failing point, walk to hit 2 passing points
    //    Return as needed to hit a second failing point
    // Keep testing until all ch/bytes find 2 passes and 2 fails
    //
    Done = FALSE;
    do {
      //
      // Walk through all 2D points
      //
      ReutGlobalCtl.Data = 0;
      ReutGlobalCtl.Bits.Global_Clear_Errors = 1;
      MrcWriteCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, ReutGlobalCtl.Data);  // Clear errors
      chError = 0;

      for (value1 = 0; value1 < Points2D; value1++) {
        //
        // Set Vref level
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (((MRC_BIT0 << Channel) & chBitmask) == 0) {
            continue;
          }

          SkipWait = (chBitmask >> (Channel + 1)); // Skip if there are more channels
          value0 = rSign * marginCh[ResultType][Rank][Channel][0][sign];
          //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "+----->Value0 is %d, Value1 is %d\n", (S32) value0, value1);

          if (param == CmdV) {
            UpdateVrefWaitTilStable (MrcData, 2, 0, value0, 0);
            MrcResetSequence (MrcData);
            //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "+----->Value0 is %d, Value1 is %d\n", (S32) value0, value1);
            break; // Just update for one channel
          } else {
            for (byte = 0; byte < Outputs->SdramCount; byte++) {
              Status = ChangeMargin (
                        MrcData,
                        param,
                        value0,
                        value1,
                        0,
                        Channel,
                        RankRx,
                        byte,
                        0,
                        0,
                        SkipWait,
                        MrcRegFileStart
                        );
              if (DimmVrefParam) {
                //
                // Only Byte 7 on Channel 1 is needed to update Wr DIMM Vref - Taken care of inside ChangeMargin routine
                //
                break;
              }
            }
          }
        }
        //
        // Run Test
        //
        chError |= RunIOTest (MrcData, chBitmask, Outputs->DQPat, SeqLC, (value1 == 0), mode);

        //
        // check if we have already failed and can stop running
        //
        if (EnBER == 0 && value1 < (U32) (Points2D - 1) && chError == chBitmask) {
          break;
        }
        //
        // Collect results and Update variables for next point to test
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if ((TwoPass[Channel] == 1 && TwoFail[Channel] == 1) || ((MRC_BIT0 << Channel) & chBitmask) == 0) {
            continue;
          }

          McChannel = (PerMc) ? 0 : Channel;

          //
          // Read Error Count
          //
          Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG +
            ((MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG) * Channel);
          ReutChErrCounterStatus.Data = MrcReadCR (MrcData, Offset);
          ErrCount                    = ReutChErrCounterStatus.Bits.Counter_Status;
          Margin                      = (U8) marginCh[ResultType][Rank][Channel][0][sign];
          Delta                       = (Margin - LastPassVref[Channel]);

          //
          // Update Pass/Fail Variables:
          //
          if (ErrCount == 0 && Margin == MaxTested[McChannel]) {
            //
            // Passing while walking up
            //
            if (Delta < 0) {
              //
              // First passing point
              // @todo: Next line should be changed to return failure. Code should never hang.
              //
              MRC_ASSERT (
                MinTested[McChannel] == MaxTested[McChannel],
                Debug,
                "Error: MaxTested < LastPass after first point"
                );
              LastPassVref[Channel] = Margin;
            } else if (Delta == 1) {
              //
              // Normal, walk to fail
              //
              Errors[Channel]       = MrcBitShift (Errors[Channel], -8 * Delta) & BER_ERROR_MASK;
              LastPassVref[Channel] = Margin;
              TwoPass[Channel]      = 1;
            } else if (Delta == 2) {
              //
              // Speckling in response, Consider point as error
              //
              Errors[Channel]   = MrcBitSwap (Errors[Channel], MrcLog8 (ErrCount), 24, 8);
              TwoFail[Channel]  = 1;
            } else {
              //
              // @todo: Next line should be changed to return failure. Code should never hang.
              //
              MRC_ASSERT (FALSE, Debug, "Error: Tested point twice or Tested >2 above LastPass");
            }
          } else if (ErrCount == 0 && Margin == MinTested[McChannel]) {
            if (TwoPass[Channel] == 1) {
              continue; // Skip if this channel is already done
            }
            //
            // Passing while walking down
            //
            if (Delta == -1) {
              Errors[Channel]  = MrcBitSwap (Errors[Channel], 0, 0, 8);
              TwoPass[Channel] = 1;     // found second pass
            } else {
              //
              // 1st passing point
              // Don't shift errors.  Fail points already assumed correct
              //
              LastPassVref[Channel] = Margin;
              TwoPass[Channel]      = 0;
            }
          } else if (ErrCount > 0 && Margin == MaxTested[McChannel]) {
            //
            // Failing while walking up
            // @todo: Next line should be changed to return failure. Code should never hang.
            //
            MRC_ASSERT (Delta <= 2, Debug, "Error: Tested >2 above LastPass");
            if (Delta < 2) {
              //
              // first failing point
              //
              Errors[Channel]   = MrcBitSwap (Errors[Channel], MrcLog8 (ErrCount), 16, 8);
              TwoFail[Channel]  = 0;
            } else if (Delta == 2) {
              //
              // 2nd failing point
              //
              Errors[Channel]   = MrcBitSwap (Errors[Channel], MrcLog8 (ErrCount), 24, 8);
              TwoFail[Channel]  = 1;
            }
          } else if (ErrCount > 0 && Margin == MinTested[McChannel]) {
            //
            // Failing while walking down
            //
            if (LastPassVref[Channel] < 0xFF && Delta <= 0) {
              //
              // Adjust LastPassVref and Error count to be below this failure point
              //
              Errors[Channel]       = MrcBitSwap (Errors[Channel], MrcLog8 (ErrCount), 8 * (Delta + 1), 8);
              Errors[Channel]       = MrcBitShift (Errors[Channel], 8 * (1 - Delta));
              LastPassVref[Channel] = Margin - 1;
            } else {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Unexpected case for channel: %d, delta: %d.\n", Channel, Delta);
              tmp             = ((Errors[Channel] & 0xFF0000) >> 8) + MrcLog8 (ErrCount);
              Errors[Channel] = MrcBitSwap (Errors[Channel], tmp, 16, 16);
            }

            if (MinTested[McChannel] < MaxTested[McChannel]) {
              TwoFail[Channel] = 1;
            }

            if (Delta <= 0) {
              TwoPass[Channel] = 0;
            }
          } else {
            //
            // @todo: Next line should be changed to return failure. Code should never hang.
            //
            MRC_ASSERT (FALSE, Debug, "Error: Testing points other than Max/MinTested");
          }
          //
          // Find Max Saturation limit
          //
          if (Interior && MaxTested[Channel] == Margin) {
            byteMax[Channel] = Margin;
          }
          //
          // Handle Max Saturation
          //
          if (Margin == byteMax[Channel]) {
            TwoFail[Channel] = 1;
          }

          if (ErrCount == 0 && byteMax[Channel] == LastPassVref[Channel] && TwoPass[Channel] == 1) {
            Errors[Channel] = MrcBitSwap (Errors[Channel], 0xFFFE, 16, 16);
          }
          //
          // Handle Min Saturation
          //
          if (Margin == 0) {
            TwoPass[Channel] = 1;
            if (ErrCount > 0) {
              TwoFail[Channel]      = 1;
              LastPassVref[Channel] = 0;
              Errors[Channel]       = MrcBitSwap (Errors[Channel], (BER_LOG_TARGET << 8) + BER_LOG_TARGET, 16, 16);
            }
          }
          //
          // Decide what to test next for PerMC == FALSE
          //
          if (!PerMc) {
            if (TwoPass[Channel] == 1) {
              if (TwoFail[Channel] == 1) {
                continue;
              }
              //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"++---->MaxTested[Ch] before ++:%d\n", MaxTested[Channel]);////////
              marginCh[ResultType][Rank][Channel][0][sign] = ++MaxTested[Channel];
            } else {
              //            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"++---->MinTested[Ch] before --:%d\n", MinTested[Channel]);////////
              marginCh[ResultType][Rank][Channel][0][sign] = --MinTested[Channel];
            }
          }
        }
        //
        // Decide what to test next for PerMC == TRUE
        //
        if (PerMc) {
          if ((TwoPass[0] == 1) && (TwoPass[1] == 1)) {
            //
            // All Channels have 2 passes
            //
            if ((TwoFail[0] == 1) && (TwoFail[1] == 1)) {
              //
              // All Channels have 2 fails
              //
              continue;
            }
            //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"++---->MaxTested[Ch] before ++:%d\n", MaxTested[Channel]);////////
            marginCh[ResultType][Rank][McChannel][0][sign] = ++MaxTested[McChannel];
          } else {
            //            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"++---->MinTested[Ch] before --:%d\n", MinTested[Channel]);////////
            marginCh[ResultType][Rank][McChannel][0][sign] = --MinTested[McChannel];
          }

          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            if (((MRC_BIT0 << Channel) & chBitmask) == 0) {
              continue;
            }

            marginCh[ResultType][Rank][Channel][0][sign] = marginCh[ResultType][Rank][McChannel][0][sign];
            MinTested[Channel] = MinTested[McChannel];
            MaxTested[Channel] = MaxTested[McChannel];
          }
        }
      }
      //
      // check if we are done
      //
      Done = TRUE;
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (TwoPass[Channel] == 0 || TwoFail[Channel] == 0) {
          Done = FALSE;
          break;
        }
      }
    } while (Done == FALSE);

    //
    // Calculate the effective margin
    // Update marginch with extroploated BER Margin
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (((MRC_BIT0 << Channel) & chBitmask) == 0) {
        continue;
      }
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE,"+----->marginCh[%d,%d] is: %d\n", Channel, sign,  marginCh[ResultType][Rank][Channel][0][sign]);
      if (EnBER) {
        marginCh[ResultType][Rank][Channel][0][sign] = interpolateVref (
                                                        LastPassVref[Channel],
                                                        (Errors[Channel] >> 16) & 0xFF,
                                                        (Errors[Channel] >> 24) & 0xFF,
                                                        BER_LOG_TARGET,
                                                        BERStats
                                                        );
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "+----->BERmarginCh[%d,%d] is: %d, Errors = 0x%x\n", Channel, sign,  marginCh[ResultType][Rank][Channel][0][sign], Errors[Channel]);
      } else {
        marginCh[ResultType][Rank][Channel][0][sign] = 10 * LastPassVref[Channel];
        //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "+----->marginCh[%d,%d] is: %d\n", Channel, sign,  marginCh[ResultType][Rank][Channel][0][sign]);
      }
    }
  }
  //
  // Clean up after step
  //
  if (param == CmdV) {
    UpdateVrefWaitTilStable (MrcData, 2, 0, 0, 0);
  } else {
    Status = ChangeMargin (MrcData, param, 0, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileCurrent);
  }

  MrcWriteCrMulticast (MrcData, MCSCHEDS_CR_REUT_CH_ERR_COUNTER_CTL_0_REG, 0);

  return Status;
}

/**
  This function shifts a 32 bit int either positive or negative

  @param[in] Value       - Input value to be shifted
  @param[in] ShiftAmount - Number of bits places to be shifted.

  @retval 0 if ShiftAmount exceeds +/- 31.  Otherwise the updated 32 bit value.
**/
U32
MrcBitShift (
  IN const U32 Value,
  IN const S8  ShiftAmount
  )
{
  if ((ShiftAmount > 31) || (ShiftAmount < -31)) {
    return 0;
  }

  if (ShiftAmount > 0) {
    return Value << ShiftAmount;
  } else {
    return Value >> (-1 * ShiftAmount);
  }
}

/**
  This function Sign extends OldMSB to NewMSB Bits (Eg: Bit 6 to Bit 7)

  @param[in] CurrentValue - Input value to be shifted
  @param[in] OldMSB       - The original most significant Bit
  @param[in] NewMSB       - The new most significant bit.

  @retval The updated 8 bit value.
**/
U8
MrcSE (
  IN U8        CurrentValue,
  IN const U8  OldMSB,
  IN const U8  NewMSB
  )
{
  U8 Scratch;

  Scratch = ((MRC_BIT0 << (NewMSB - OldMSB)) - 1) << OldMSB;
  if (CurrentValue >> (OldMSB - 1)) {
    CurrentValue |= Scratch;
  } else {
    CurrentValue &= (~Scratch);
  }

  return CurrentValue;
}

/**
  This function calculates the Log base 2 of the value to a maximum of Bits

  @param[in] Value - Input value

  @retval Returns the log base 2 of input value
**/
U8
MrcLog2 (
  IN const U32 Value
  )
{
  U8  Log;
  U8  Bit;

  //
  // Return 0 if value is negative
  //
  Log = 0;
  if ((Value + 1) != 0) {
    for (Bit = 0; Bit < 32; Bit++) {
      if (Value & (MRC_BIT0 << Bit)) {
        Log = (Bit + 1);
      }
    }
  }

  return Log;
}

/**
  ***** Has Buffer Overflow for 68-71, 544-575, 4352-4607, ... ****
  This function calculates the Log base 8 of the Input parameter using integers

  @param[in] Value - Input value

  @retval Returns 10x the log base 8 of input Value
**/
U32
MrcLog8 (
  IN U32 Value
  )
{
  const U8  Loglook[17] = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 7, 8, 8, 9, 9, 9, 10, 10 };
  U32       Loga;
  U32       Rema;

  Loga  = 0;
  Rema  = 2 * Value;
  while (Value > 8) {
    Rema  = Value >> 2;
    Value = Value >> 3;
    Loga += 10;
  };

  return (Loga + Loglook[Rema]); //returns an integer approximation of "log8(a) * 10"
}

/**
  This function Sorts Arr from largest to smallest

  @param[in,out] Arr     - Array to be sorted
  @param[in]     Channel - Channel to sort.
  @param[in]     lenArr  - Length of the array

  @retval Nothing
**/
void
MrcBsortPerChannel (
  IN OUT U32      Arr[MAX_CHANNEL][4],
  IN     const U8 Channel,
  IN     const U8 lenArr
  )
{
  U8  i;
  U8  j;
  U32 temp;

  for (i = 0; i < lenArr - 1; i++) {
    for (j = 0; j < lenArr - (1 + i); j++) {
      if (Arr[Channel][j] < Arr[Channel][j + 1]) {
        temp                = Arr[Channel][j];
        Arr[Channel][j]     = Arr[Channel][j + 1];
        Arr[Channel][j + 1] = temp;
      }
    }
  }

  return;
}

/**
  This function Sorts Arr from largest to smallest

  @param[in,out] Arr    - Array to be sort
  @param[in]     lenArr - Lenght of the array

  @retval Nothing
**/
void
MrcBsort (
  IN OUT U32 *const Arr,
  IN     const U8   lenArr
  )
{
  U8  i;
  U8  j;
  U32 temp;

  for (i = 0; i < lenArr - 1; i++) {
    for (j = 0; j < lenArr - (1 + i); j++) {
      if (Arr[j] < Arr[j + 1]) {
        temp        = Arr[j];
        Arr[j]      = Arr[j + 1];
        Arr[j + 1]  = temp;
      }
    }
  }

  return;
}

/**
  This function calculates the Natural Log of the Input parameter using integers

  @param[in] Input - 100 times a number to get the Natural log from.
                      Max Input Number is 40,000 (without 100x)

  @retval 100 times the actual result. Accurate within +/- 2
**/
U32
MrcNaturalLog (
  U32 Input
  )
{
  U32 Output;

  Output = 0;
  while (Input > 271) {
    Input = (Input * 1000) / 2718;
    Output += 100;
  }

  Output += ((-16 * Input * Input + 11578 * Input - 978860) / 10000);

  return Output;
}

/**
  This function calculates the number of bits set to 1 in a 32-bit value.

  @param[in] Input - The value to work on.

  @retval The number of bits set to 1 in Input.
**/
U8
MrcCountBitsEqOne (
  IN U32 Input
  )
{
  U8  NumOnes;

  NumOnes = 0;
  while (Input > 0) {
    NumOnes++;
    Input &= (Input - 1);
  }

  return NumOnes;
}

/**
  This function calculates e to the power of of the Input parameter using integers.

  @param[in] Input - 100 times a number to elevate e to.

  @retval 100 times the actual result. Accurate within +/- 2.
**/
U32
Mrceexp (
  IN U32 Input
  )
{
  U32 Extra100;
  U32 Output;

  Extra100  = 0;
  Output    = 1;
  while (Input > 100) {
    Input -= 100;
    Output = (Output * 2718) / 10;
    if (Extra100) {
      Output /= 100;
    }

    Extra100 = 1;
  }

  Output = ((Output * (8 * Input * Input + 900 * Input + 101000)) / 1000);

  if (Extra100) {
    Output /= 100;
  }

  return Output;
}

/**
  This function writes a 32 bit register.

  @param[in] MrcData - Include all MRC global data.
  @param[in] Offset  - Offset of register from MchBar Base Address.
  @param[in] Value   - Value to write.

  @retval Nothing
**/
void
MrcWriteCrMulticast (
  IN MrcParameters *const MrcData,
  IN const U32            Offset,
  IN const U32            Value
  )
{
  MrcOemMmioWrite (Offset, Value, MrcData->SysIn.Inputs.MchBarBaseAddress);
#ifdef MRC_DEBUG_PRINT
  if (MrcData->SysIn.Inputs.Debug.PostCode[0] == MrcData->SysIn.Inputs.Debug.PostCode[1]) {
    MRC_DEBUG_MSG (
      &MrcData->SysIn.Inputs.Debug,
      MSG_LEVEL_NOTE,
      "%08Xh > %08Xh\n",
      MrcData->SysIn.Inputs.MchBarBaseAddress + Offset,
      Value
      );
  }
#endif // MRC_DEBUG_PRINT
  return;
}

/**
  This function writes a 64 bit register.

  @param[in] MrcData - Include all MRC global data.
  @param[in] Offset  - Offset of register from MchBar Base Address.
  @param[in] Value   - Value to write.

  @retval Nothing
**/
void
MrcWriteCR64 (
  IN MrcParameters *const MrcData,
  IN const U32            Offset,
  IN const U64            Value
  )
{
  MrcOemMmioWrite64 (Offset, Value, MrcData->SysIn.Inputs.MchBarBaseAddress);
#ifdef MRC_DEBUG_PRINT
  if (MrcData->SysIn.Inputs.Debug.PostCode[0] == MrcData->SysIn.Inputs.Debug.PostCode[1]) {
    MRC_DEBUG_MSG (
      &MrcData->SysIn.Inputs.Debug,
      MSG_LEVEL_NOTE,
      "%08Xh > %016Xh\n",
      MrcData->SysIn.Inputs.MchBarBaseAddress + Offset,
      Value
      );
  }
#endif // MRC_DEBUG_PRINT
  return;
}

/**
  This function writes a 32 bit register.

  @param[in] MrcData - Include all MRC global data.
  @param[in] Offset  - Offset of register from MchBar Base Address.
  @param[in] Value   - Value to write.

  @retval Nothing
**/
void
MrcWriteCR (
  IN MrcParameters *const MrcData,
  IN const U32            Offset,
  IN const U32            Value
  )
{
  MrcOemMmioWrite (Offset, Value, MrcData->SysIn.Inputs.MchBarBaseAddress);
#ifdef MRC_DEBUG_PRINT
  if (MrcData->SysIn.Inputs.Debug.PostCode[0] == MrcData->SysIn.Inputs.Debug.PostCode[1]) {
    MRC_DEBUG_MSG (
      &MrcData->SysIn.Inputs.Debug,
      MSG_LEVEL_NOTE,
      "%08Xh > %08Xh\n",
      MrcData->SysIn.Inputs.MchBarBaseAddress + Offset,
      Value
      );
  }
#endif // MRC_DEBUG_PRINT
  return;
}

/**
  This function writes a 8 bit register.

  @param[in] MrcData - Include all MRC global data.
  @param[in] Offset  - Offset of register from MchBar Base Address.
  @param[in] Value   - The value to write.

  @retval Nothing
**/
void
MrcWriteCR8 (
  IN MrcParameters*const MrcData,
  IN const U32            Offset,
  IN const U8             Value
  )
{
  MrcOemMmioWrite8 (Offset, Value, MrcData->SysIn.Inputs.MchBarBaseAddress);
#ifdef MRC_DEBUG_PRINT
  if (MrcData->SysIn.Inputs.Debug.PostCode[0] == MrcData->SysIn.Inputs.Debug.PostCode[1]) {
    MRC_DEBUG_MSG (
      &MrcData->SysIn.Inputs.Debug,
      MSG_LEVEL_NOTE,
      "%08Xh > %02Xh\n",
      MrcData->SysIn.Inputs.MchBarBaseAddress + Offset,
      Value
      );
  }
#endif // MRC_DEBUG_PRINT
  return;
}

/**
  This function reads a 64 bit register.

  @param[in] MrcData - Include all MRC global data.
  @param[in] Offset  - Offset of register from MchBar Base Address.

  @retval Value read from the register.
**/
U64
MrcReadCR64 (
  IN MrcParameters *const MrcData,
  IN const U32            Offset
  )
{
  U64 Value;

  MrcOemMmioRead64 (Offset, &Value, MrcData->SysIn.Inputs.MchBarBaseAddress);
  return Value;
}

/**
  This function reads a 32 bit register.

  @param[in] MrcData - Include all MRC global data.
  @param[in] Offset  - Offset of register from MchBar Base Address.

  @retval Value read from the register
**/
U32
MrcReadCR (
  IN MrcParameters *const MrcData,
  IN const U32            Offset
  )
{
  U32 Value;

  MrcOemMmioRead (Offset, &Value, MrcData->SysIn.Inputs.MchBarBaseAddress);
  return Value;
}

/**
  This function blocks the CPU for the duration specified in HPET Delay time.

  @param[in] MrcData   - Include all MRC global data.
  @param[in] DelayHPET - time to wait in 69.841279ns

  @retval Nothing
**/
void
MrcWait (
  IN MrcParameters *const MrcData,
  IN U32                  DelayHPET
  )
{
  const MrcInput  *Inputs;
  BOOL            Done;
  U32             Start;
  volatile U32    Finish;
  U32             Now;

Inputs = &MrcData->SysIn.Inputs;
Done   = FALSE;


  if (DelayHPET >= (5 * HPET_1US)) {
    MrcOemMmioRead (0xF0, &Start, Inputs->HpetBaseAddress);
    Finish = Start + DelayHPET;

    do {
      MrcOemMmioRead (0xF0, &Now, Inputs->HpetBaseAddress);
      if (Finish > Start) {
        if (Now >= Finish) {
          Done = TRUE;
        }
      } else {
        if ((Now < Start) && (Now >= Finish)) {
          Done = TRUE;
        }
      }
    } while (Done == FALSE);
  } else {
    for (Start = 0; Start < ((DelayHPET + HPET_MIN) / (2 * HPET_MIN)); Start++) {
      //
      // Just perform Dummy reads to CPU CR
      //
      Finish = MrcReadCR (MrcData, MCSCHEDS_CR_REUT_CH_ERR_DATA_STATUS_REG);
    }
  }
  return;
}

/**
  This function forces an RCOMP.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing
**/
void
ForceRcomp (
  IN MrcParameters *const MrcData
  )
{
  MrcWriteCR8 (MrcData, PCU_CR_M_COMP_PCU_REG + 1, MRC_BIT0);

  //
  // 10 - 20 us wait.
  //
  MrcWait (MrcData, 10 * HPET_1US);
  return;
}

/**
  This function sets the self refresh idle timer and enables it.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing
**/
void
EnterSR (
  IN MrcParameters *const MrcData
  )
{
  MCDECS_CR_PM_SREF_CONFIG_MCMAIN_STRUCT  PmSrefConfig;

  PmSrefConfig.Data             = 0;
  PmSrefConfig.Bits.SR_Enable   = 1;
  PmSrefConfig.Bits.Idle_timer  = SELF_REFRESH_IDLE_COUNT;
  MrcWriteCR (MrcData, MCDECS_CR_PM_SREF_CONFIG_MCMAIN_REG, PmSrefConfig.Data);
  MrcWait (MrcData, HPET_1US);
  return;
}

/**
  This function sets the self refresh idle timer and disables it.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing
**/
void
ExitSR (
  IN MrcParameters *const MrcData
  )
{
  MCDECS_CR_PM_SREF_CONFIG_MCMAIN_STRUCT  PmSrefConfig;

  PmSrefConfig.Data             = 0;
  PmSrefConfig.Bits.Idle_timer  = SELF_REFRESH_IDLE_COUNT;
  MrcWriteCR (MrcData, MCDECS_CR_PM_SREF_CONFIG_MCMAIN_REG, PmSrefConfig.Data);
  MrcWait (MrcData, HPET_1US);
  return;
}

/**
  This function programs the WDB.

  @param[in] MrcData - Include all MRC global data.

  @retval Nothing
**/
void
SetupWDB (
  IN MrcParameters *const MrcData
  )
{
  U8        a;
  const U32 vmask    =  0x41041041;
  const U32 amask[9] = {0x86186186, 0x18618618, 0x30C30C30, 0xA28A28A2, 0x8A28A28A,
                        0x14514514, 0x28A28A28, 0x92492492, 0x24924924};
  const U32 seeds[MRC_WDB_NUM_MUX_SEEDS] = {0xA10CA1, 0xEF0D08, 0xAD0A1E};
  U8        Channel;
  U32       Offset;
  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_LMN_STRUCT ReutPatWdbClMuxLmn;

  //
  // Fill first 8 entries as simple 2 LFSR VA pattern
  // VicRot=8, Start=0
  //
  WriteWDBVAPattern (MrcData, 0, BASIC_VA_PATTERN_SPRED_8, 8, 0);

  //
  // Fill next 54 entries as 3 LFSR VA pattern
  //
  for (a = 0; a < 9; a++) {
    //
    // VicRot=6, Start=8+a*6
    //
    WriteWDBVAPattern (MrcData, amask[a], vmask, 6, 8 + a * 6);
  }
  //
  // Write the LFSR seeds
  //
  MrcProgramLFSR (MrcData, seeds);
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (&MrcData->SysOut.Outputs, Channel)) {
      ReutPatWdbClMuxLmn.Data                         = 0;
      ReutPatWdbClMuxLmn.Bits.N_counter               = 10;
      ReutPatWdbClMuxLmn.Bits.M_counter               = 1;
      ReutPatWdbClMuxLmn.Bits.L_counter               = 1;
      ReutPatWdbClMuxLmn.Bits.Enable_Sweep_Frequency  = 1;
      Offset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_LMN_REG +
        ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_LMN_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_LMN_REG) * Channel);
      MrcWriteCR (MrcData, Offset, ReutPatWdbClMuxLmn.Data);
    }
  }

  return;
}

/**
  This function will program all present channels with the 3 seeds passed in.

  @param[in] MrcData - Global MRC data structure
  @param[in] seeds   - Array of 3 seeds programmed into PAT_WDB_CL_MUX_PB_RD/WR

  @retval - Nothing

**/
void
MrcProgramLFSR (
  IN MrcParameters *const MrcData,
  IN U32 const            seeds[MRC_WDB_NUM_MUX_SEEDS]
  )
{
  U32 CrOffset;
  U8  Channel;
  U8  s;

  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (&MrcData->SysOut.Outputs, Channel)) {
      for (s = 0; s < MRC_WDB_NUM_MUX_SEEDS; s++) {
        CrOffset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_PB_RD_0_REG +
          ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_PB_RD_0_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_PB_RD_0_REG) * Channel) +
          ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_PB_RD_1_REG - MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_PB_RD_0_REG) * s);
        MrcWriteCR (MrcData, CrOffset, seeds[s]);
        CrOffset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_PB_WR_0_REG +
          ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_PB_WR_0_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_PB_WR_0_REG) * Channel) +
          ((MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_PB_WR_1_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_PB_WR_0_REG) * s);
        MrcWriteCR (MrcData, CrOffset, seeds[s]);
      }
    }
  }
}

/**
  This function Write 1 cacheline worth of data to the WDB

  @param[in] MrcData  - Include all MRC global data.
  @param[in] Patterns - Array of bytes.  Each bytes represents 8 chunks of the cachelines for 1 lane.
                         Each entry in Patterns represents a different cacheline for a different lane.
  @param[in] PMask    - Array of len Spread uint8.  Maps the patterns to the actual DQ lanes.
                         DQ[0] = Patterns[PMask[0]], ... DQ[Spread-1] = Patterns[PMask[Spread-1]]
                         DQ[Spread] = DQ[0], ... DQ[2*Spread-1] = DQ[Spread-1]
  @param[in] Start    - Starting entry in the WDB.

  @retval Nothing
**/
void
WriteWDBFixedPattern (
  IN MrcParameters *const MrcData,
  IN U8 *const            Patterns,
  IN U8 *const            PMask,
  IN const U8             Spread,
  IN const U16            Start
  )
{
  MrcOutput                           *Outputs;
  U8                                  Channel;
  U8                                  up32;
  U8                                  chunk;
  U8                                  b;
  U8                                  beff;
  U8                                  burst;
  U32                                 data;
  U32                                 pointer;
  U32                                 Offset;
  MCHBAR_CH0_CR_QCLK_LDAT_PDAT_STRUCT QclkLdatPdat;

  Outputs = &MrcData->SysOut.Outputs;
  for (chunk = 0; chunk < 8; chunk++) {
    //
    // Program LDAT_DATAIN_*
    //
    for (up32 = 0; up32 < 2; up32++) {
      data = 0;
      for (b = 0; b < 32; b++) {
        beff  = (b + 32 * up32) % Spread;
        burst = Patterns[PMask[beff]];
        if (burst & (MRC_BIT0 << chunk)) {
          data |= (MRC_BIT0 << b);
        }
      }

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcChannelExist (Outputs, Channel)) {
          Offset = MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_0_REG +
            ((MCHBAR_CH1_CR_QCLK_LDAT_DATAIN_0_REG - MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_0_REG) * Channel) +
            ((MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_1_REG - MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_0_REG) * up32);
          MrcWriteCR (MrcData, Offset, data);
        }
      }
    } // up32

    pointer = MRC_BIT16 + chunk;

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        //
        // Set rep = 0 don't want to replicate the data
        // Set banksel field to the value of the chunk you want to write the 64 bits to.
        // Set arraysel = 0 ( indicating it is the MC WDB) and mode = 'b01 in the SDAT register
        //
        Offset = MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG +
          ((MCHBAR_CH1_CR_QCLK_LDAT_SDAT_REG - MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG) * Channel);
        MrcWriteCR (MrcData, Offset, pointer);

        //
        // Finally, write the PDAT register indicating which cacheline of the WDB you want to write to
        // by setting fastaddr field to one of the 64 cache lines. Also set cmdb in the pdat register to 4'b1000,
        // indicating that this is a LDAT write
        //
        Offset = MCHBAR_CH0_CR_QCLK_LDAT_PDAT_REG +
          ((MCHBAR_CH1_CR_QCLK_LDAT_PDAT_REG - MCHBAR_CH0_CR_QCLK_LDAT_PDAT_REG) * Channel);
        QclkLdatPdat.Data = 0;
        QclkLdatPdat.Bits.CMDB = 8;
        QclkLdatPdat.Bits.FASTADDR = MIN (Start, MCHBAR_CH0_CR_QCLK_LDAT_PDAT_FASTADDR_MAX);
        MrcWriteCR (MrcData, Offset, QclkLdatPdat.Data);
      }
    }
  } // chunk
  //
  // Turn off LDAT mode after writing to WDB is complete
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Offset = MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG +
        ((MCHBAR_CH1_CR_QCLK_LDAT_SDAT_REG - MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG) * Channel);
      MrcWriteCR (MrcData, Offset, 0);
    }
  }

  return;
}

/**
  This rotine performs the following steps:
    Step 0: Iterate through all VicRots
    Step 1: Create a compressed vector for a given 32 byte cacheline
            Each byte has a value of LFSR0=AA/LFSR1=CC/LFSR2=F0
    Step 2: Expand compressed vector into chunks and 32 bit segments

  @param[in] MrcData - Include all MRC global data.
  @param[in] vmask   - 32 bit victim mask.  1 indicates this bit should use LFSR0
  @param[in] amask   - 32 bit aggressor mask. 0/1 indicates this bit should use LFSR1/2
  @param[in] VicRot  - Number of times to circular rotate vmask/amask
  @param[in] Start   - Starting entry in the WDB

  @retval Nothing
**/
void
WriteWDBVAPattern (
  IN MrcParameters *const MrcData,
  IN U32                  amask,
  IN U32                  vmask,
  IN const U8             VicRot,
  IN const U16            Start
  )
{
  const U8        VAMask2Compressed[4] = {0xAA, 0xC0, 0xCC, 0xF0};
  MrcOutput       *Outputs;
  U8              b;
  U8              chunk;
  U8              Channel;
  U8              cmask;
  U16             v;
  U32             Vic;
  U32             Agg2;
  U32             data;
  U32             pointer;
  U32             msb;
  U8              Compressed[32];
  U32             BitMask;
  U8              Index;
  U16             Scratch;
  U32             Offset;
  MCHBAR_CH0_CR_QCLK_LDAT_PDAT_STRUCT QclkLdatPdat;

  Outputs = &MrcData->SysOut.Outputs;
  for (v = 0; v < VicRot; v++) {
    //
    // Iterate through all 32 bits and create a compressed version of cacheline
    // AA = Victim (LFSR0), CC = Agg1(LFSR1), F0 = Agg2 (LFSR2)
    //
    for (b = 0; b < 32; b++) {
      BitMask = MRC_BIT0 << b;
      Vic     = (vmask & BitMask);
      Agg2    = (amask & BitMask);

      //
      // Program compressed vector
      //
      if (Vic && Agg2) {
        Index = 1;
      } else if (Vic && !Agg2) {
        Index = 0;
      } else if (!Vic && !Agg2) {
        Index = 2;
      } else {
        Index = 3;
      }

      Compressed[b] = VAMask2Compressed[Index];
    }

    for (chunk = 0; chunk < 8; chunk++) {
      data  = 0;
      cmask = (MRC_BIT0 << chunk);
      for (b = 0; b < 32; b++) {
        if (Compressed[b] & cmask) {
          data |= (MRC_BIT0 << b);
        }
      }
      //
      // Set rep = 0 don't want to replicate the data
      // Set banksel field to the value of the chunk you want to write the 64 bits to.
      // Set arraysel = 0 ( indicating it is the MC WDB) and mode = 'b01 in the SDAT register
      //
      pointer = MRC_BIT16 + chunk;
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (MrcChannelExist (Outputs, Channel)) {
          Offset = MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_0_REG +
            ((MCHBAR_CH1_CR_QCLK_LDAT_DATAIN_0_REG - MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_0_REG) * Channel);
          MrcWriteCR (MrcData, Offset, data);
          Offset = MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_1_REG +
            ((MCHBAR_CH1_CR_QCLK_LDAT_DATAIN_1_REG - MCHBAR_CH0_CR_QCLK_LDAT_DATAIN_1_REG) * Channel);
          MrcWriteCR (MrcData, Offset, data);
          Offset = MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG +
            ((MCHBAR_CH1_CR_QCLK_LDAT_SDAT_REG - MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG) * Channel);
          MrcWriteCR (MrcData, Offset, pointer);

          //
          // Finally, write the PDAT register indicating which cacheline of the WDB you want to write to
          // by setting fastaddr field to one of the 64 cache lines. Also set cmdb in the pdat register to 4'b1000,
          // indicating that this is a LDAT write
          //
          Scratch = Start + v;
          Offset = MCHBAR_CH0_CR_QCLK_LDAT_PDAT_REG +
            ((MCHBAR_CH1_CR_QCLK_LDAT_PDAT_REG - MCHBAR_CH0_CR_QCLK_LDAT_PDAT_REG) * Channel);
          QclkLdatPdat.Data           = 0;
          QclkLdatPdat.Bits.CMDB      = 8;
          QclkLdatPdat.Bits.FASTADDR  = MIN (Scratch, MCHBAR_CH0_CR_QCLK_LDAT_PDAT_FASTADDR_MAX);
          MrcWriteCR (MrcData, Offset, QclkLdatPdat.Data);
        }
      }
    }
    //
    // Circular Rotate Vic/Agg Masks
    //
    msb   = (vmask >> 31) & 0x1;
    vmask = (vmask << 1) | msb;
    msb   = (amask >> 31) & 0x1;
    amask = (amask << 1) | msb;
  }
  //
  // Clear LDAT mode
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Offset = MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG +
        ((MCHBAR_CH1_CR_QCLK_LDAT_SDAT_REG - MCHBAR_CH0_CR_QCLK_LDAT_SDAT_REG) * Channel);
      MrcWriteCR (MrcData, Offset, 0);
    }
  }

  return;
}

/**
  Write VA pattern in CADB
  Use basic VA pattern for CADB with 2 LFSRs. Rotation is manual
  Bit Order is  [CKE[3:0], ODT[3:0], CMD[2:0], CS[3:0], BA[2:0], MA[15:0]]
  [59:56]  [51:48]  [42:40] [35:32]  [26:24]  [15:0]

  NOTE: CKE, ODT and CS are not used in functional mode and are ignored

  @param[in] MrcData   - Include all MRC global data.
  @param[in] Channel   - Channel to setup.
  @param[in] VicSpread - Separation of the Victim Bit.
  @param[in] VicBit    - The Bit to be the Victim.
  @param[in] LMNEn     - To enable LMN counter

  @retval Nothing
**/
void
SetupCADB (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             VicSpread,
  IN const U8             VicBit,
  IN const U8             LMNEn
  )
{
  const U16 seeds[3] = {0xEA1, 0xBEEF, 0xDEAD};
  U8        Row;
  U8        bit;
  U8        lfsr0;
  U8        lfsr1;
  U8        bremap;
  U8        Fine;
  U32       Offset;
  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_STRUCT        ReutChPatCadbProg;
  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_CTRL_STRUCT    ReutChPatCadbMuxCtrl;
  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CL_MUX_LMN_STRUCT  ReutCadbClMuxLmn;

  //
  // Currently, always start writing at CADB row0.  Could add Start point in future.
  //
  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_WRITE_POINTER_REG) * Channel);
  MrcWriteCR8 (MrcData, Offset, 0);

  //
  // Plan to use VicSpread of 7 bits
  // Walk through CADB rows, assigning bit for 1 VA pattern
  //
  for (Row = 0; Row < MRC_NUM_CADB_ENTRIES; Row++) {

    lfsr0 = (Row & 0x1);         // 0, 1, 0, 1 0, 1, 0, 1 for r = 0,1, ..., 7
    lfsr1 = ((Row >> 1) & 0x1);  // 0, 0, 1, 1 0, 0, 1, 1 for r = 0,1, ..., 7
    //
    // Assign Victim/Aggressor Bits
    //
    ReutChPatCadbProg.Data = 0;
    for (bit = 0; bit < 22; bit++) {
      //
      // b in range(22)
      //
      Fine = bit % VicSpread;
      if (bit >= 19) {
        bremap = bit + 21; // b = [40-42]
      } else if (bit >= 16) {
        bremap = bit + 8; // b = [24-26]
      } else {
        bremap = bit; // b = [0-15]
      }

      if (Fine == VicBit) {
        ReutChPatCadbProg.Data |= MrcOemMemoryLeftShiftU64 ((U64) lfsr0, bremap);
      } else {
        ReutChPatCadbProg.Data |= MrcOemMemoryLeftShiftU64 ((U64) lfsr1, bremap);
      }
    }
    //
    // Write Row. CADB is auto incremented after every write
    //
    Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_REG +
      ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_PROG_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_PROG_REG) * Channel);
    MrcWriteCR64 (MrcData, Offset, ReutChPatCadbProg.Data);
  }
  //
  // Setup CADB in terms of LFSR selects, LFSR seeds, LMN constants and overall control
  //
  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_CTRL_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MUX_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_CTRL_REG) * Channel);
  ReutChPatCadbMuxCtrl.Data               = 0;
  ReutChPatCadbMuxCtrl.Bits.Mux0_Control  = LMNEn ? 0 : 2;
  ReutChPatCadbMuxCtrl.Bits.Mux1_Control  = 2;
  ReutChPatCadbMuxCtrl.Bits.Mux2_Control  = 2;
  MrcWriteCR (MrcData, Offset, ReutChPatCadbMuxCtrl.Data);

  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CL_MUX_LMN_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_CL_MUX_LMN_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CL_MUX_LMN_REG) * Channel);
  ReutCadbClMuxLmn.Data                         = 0;
  ReutCadbClMuxLmn.Bits.Enable_Sweep_Frequency  = 1;
  ReutCadbClMuxLmn.Bits.L_counter               = 1;
  ReutCadbClMuxLmn.Bits.M_counter               = 1;
  ReutCadbClMuxLmn.Bits.N_counter               = 6;
  MrcWriteCR (MrcData, Offset, ReutCadbClMuxLmn.Data);

  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_0_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MUX_PB_0_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_0_REG) * Channel);
  MrcWriteCR (MrcData, Offset, seeds[0]);

  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_1_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MUX_PB_1_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_1_REG) * Channel);
  MrcWriteCR (MrcData, Offset, seeds[1]);

  Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_2_REG +
    ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_MUX_PB_2_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_MUX_PB_2_REG) * Channel);
  MrcWriteCR (MrcData, Offset, seeds[2]);

  return;
}

/**
  Program the subsequence type field in a given MCDFXS_CR_REUT_CHx_SUBSEQ_CTL_MCMAIN_x_STRUCT register

  @param[in]      MrcData    - MRC global data
  @param[in, out] SubSeqCtl  - Address of the MCDFXS_CR_REUT_CHx_SUBSEQ_CTL_MCMAIN_x_STRUCT register
  @param[in]      Type       - The subsequence type to program

  @retval Nothing.
**/
void
SetSubsequenceType (
  IN     MrcParameters *const MrcData,
  IN OUT U32                  *SubSeqCtl,
  IN     U32                  Type
  )
{
  const MrcInput  *Inputs;

  Inputs = &MrcData->SysIn.Inputs;

  if ((Inputs->CpuModel == cmHSW) && (Inputs->CpuStepping == csHswA0)) {
    ((MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_STRUCT_HSW_A0 *) (SubSeqCtl))->Bits.Subsequence_Type = Type;
  } else {
    ((MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_STRUCT *) (SubSeqCtl))->Bits.Subsequence_Type = Type;
  }
}

/**
  This function handles writing to the REUT Addressing sequence for IO Tests.
  To not write a certain parameter, pass a NULL pointer to the function.

  @param[in] MrcData       - MRC global data structure.
  @param[in] Channel       - Specifies the channel to program.
  @param[in] StartAddr     - Start value for Rank/Bank/Row/Col.
  @param[in] StopAddr      - Stop value for Rank/Bank/Row/Col.
  @param[in] FieldOrder    - Relative order for carry propagates of Rank/Bank/Row/Col.
  @param[in] IncRate       - The number of writes to Rank/Bank/Row/Col before updating the address.
                              Note: The function will handle linear vs exponential and a value of 0 specifies a rate of 1.
  @param[in] IncValue      - The amount to increase Rank/Bank/Row/Col address.
  @param[in] WrapTriggerEn - Enables wrap trigger for Rank/Bank/Row/Col to enable stopping on subsequence and sequence.
  @param[in] WrapCarryEn   - Enables carry propagation on wrap to the next higest order field
  @param[in] AddrInvertEn  - Enables inverting the Rank/Bank/Row/Col addresses based on AddrInvertRate.
  @param[in] AddrIvertRate - Exponential rate of address inversion.  Only updated if AddrInvertEn != NULL.
  @param[in] EnableDebug   - Enables/Disables debug printing.

  @retval Nothing
**/
void
MrcProgramSequenceAddress (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U16            StartAddr[MrcReutFieldMax],
  IN const U16            StopAddr[MrcReutFieldMax],
  IN const U8             FieldOrder[MrcReutFieldMax],
  IN const U32            IncRate[MrcReutFieldMax],
  IN const U16            IncValue[MrcReutFieldMax],
  IN const U8             WrapTriggerEn[MrcReutFieldMax],
  IN const U8             WrapCarryEn[MrcReutFieldMax],
  IN const U8             AddrInvertEn[MrcReutFieldMax],
  IN const U8             AddrInvertRate,
  IN const BOOL           EnableDebug
  )
{
  MrcInput  *Inputs;
  U64       RowMask;
  U32       ColumnMask;
  U32       CrOffset;
  U32       IncRateScratch;
  U16       ColAddrIncMax;
  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_STRUCT                   ReutChSeqBaseAddrStart;
  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_STRUCT                    ReutChSeqBaseAddrWrap;
  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_STRUCT                 ReutChSeqBaseAddrIncCtl;
  MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_STRUCT  ReutChSeqBaseAddrOrderCarryInvertCtl;
#ifdef MRC_DEBUG_PRINT
  MrcDebug  *Debug;
  Debug = &MrcData->SysIn.Inputs.Debug;
#endif

  Inputs = &MrcData->SysIn.Inputs;

  //
  // @todo: Review next stepping
  //
  RowMask = (U64) MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Row_Address_MSK;
  switch (Inputs->CpuModel) {
    case cmHSW:
      if (Inputs->CpuStepping == csHswA0) {
        ColumnMask = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Column_Address_MSK_A0;
        ColAddrIncMax = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Column_Base_Address_Increment_MAX_A0;
      } else {
        ColumnMask = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Column_Address_MSK;
        ColAddrIncMax = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Column_Base_Address_Increment_MAX;
      }
      break;

    case cmHSW_ULT:
    case cmCRW:
      ColumnMask = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Column_Address_MSK;
      ColAddrIncMax = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Column_Base_Address_Increment_MAX;
      break;

    default:
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "Invalid CPU Type in MrcProgramSequenceAddress.  Defaulting to Hsw last stepping: %x.\n",
        csHswLast
        );
      ColumnMask = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Column_Address_MSK;
      ColAddrIncMax = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_Column_Base_Address_Increment_MAX;
  }

#ifdef MRC_DEBUG_PRINT
  if (EnableDebug) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "Ch.%d Masks: Col - 0x%x\t Row - 0x%08x%08x\tColAddrIncMax: 0x%x\n",
      Channel,
      ColumnMask,
      (U32) MrcOemMemoryRightShiftU64 (RowMask, 32),
      (U32) RowMask,
      ColAddrIncMax
      );
  }
#endif

  if (StartAddr != NULL) {
    ReutChSeqBaseAddrStart.Data = MrcOemMemoryLeftShiftU64 (
                                    (U64) ((StartAddr[MrcReutFieldRank] << (56 - 32)) + (StartAddr[MrcReutFieldBank] << (48 - 32))),
                                    32
                                    );
    ReutChSeqBaseAddrStart.Data |= MrcOemMemoryLeftShiftU64 ((U64) StartAddr[MrcReutFieldRow], 24) & RowMask;
    ReutChSeqBaseAddrStart.Data |= StartAddr[MrcReutFieldCol] & ColumnMask;

    CrOffset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG +
      (
        (MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG)
        * Channel
      );
    MrcWriteCR64 (MrcData, CrOffset, ReutChSeqBaseAddrStart.Data);

#ifdef MRC_DEBUG_PRINT
    if (EnableDebug) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Start:\n\tField\tInput\t\tStruct\n");
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tCol:\t0x%x\t\t0x%x\n",
        StartAddr[MrcReutFieldCol],
        ReutChSeqBaseAddrStart.Data & ColumnMask
        );
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tRow:\t0x%x\t\t0x%x\n",
        StartAddr[MrcReutFieldRow],
        MrcOemMemoryRightShiftU64 (ReutChSeqBaseAddrStart.Data, 24) & MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Row_Address_MAX
        );
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tBank:\t0x%x\t\t0x%x\n",
        StartAddr[MrcReutFieldBank],
        MrcOemMemoryRightShiftU64 (ReutChSeqBaseAddrStart.Data, 48) & MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Bank_Address_MAX
        );
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tRank:\t0x%x\t\t0x%x\n",
        StartAddr[MrcReutFieldRank],
        MrcOemMemoryRightShiftU64 (ReutChSeqBaseAddrStart.Data, 56) & MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Rank_Address_MAX
        );
    }
#endif
  }

  if (StopAddr != NULL) {
    ReutChSeqBaseAddrWrap.Data = MrcOemMemoryLeftShiftU64 (
                                  (U64) ((StopAddr[MrcReutFieldRank] << (56 - 32)) + (StopAddr[MrcReutFieldBank] << (48 - 32))),
                                  32
                                  );
    ReutChSeqBaseAddrWrap.Data |= MrcOemMemoryLeftShiftU64 ((U64) StopAddr[MrcReutFieldRow], 24) & RowMask;
    ReutChSeqBaseAddrWrap.Data |= StopAddr[MrcReutFieldCol] & ColumnMask;

    CrOffset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG +
      (
        (MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG)
        * Channel
      );
    MrcWriteCR64 (MrcData, CrOffset, ReutChSeqBaseAddrWrap.Data);

#ifdef MRC_DEBUG_PRINT
    if (EnableDebug) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Stop:\n\tField\tInput\t\tStruct\n");
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tCol:\t0x%x\t\t0x%x\n",
        StopAddr[MrcReutFieldCol],
        ReutChSeqBaseAddrWrap.Data & ColumnMask
        );
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tRow:\t0x%x\t\t0x%x\n",
        StopAddr[MrcReutFieldRow],
        MrcOemMemoryRightShiftU64 (ReutChSeqBaseAddrWrap.Data, 24) & MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Row_Address_MAX
        );
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tBank:\t0x%x\t\t0x%x\n",
        StopAddr[MrcReutFieldBank],
        MrcOemMemoryRightShiftU64 (ReutChSeqBaseAddrWrap.Data, 48) & MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Bank_Address_MAX
        );
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE,
        "\tRank:\t0x%x\t\t0x%x\n",
        StopAddr[MrcReutFieldRank],
        MrcOemMemoryRightShiftU64 (ReutChSeqBaseAddrWrap.Data, 56) & MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_Rank_Address_MAX
        );
    }
#endif
  }

  if (FieldOrder != NULL || WrapTriggerEn != NULL || WrapCarryEn != NULL || AddrInvertEn != NULL) {
    ReutChSeqBaseAddrOrderCarryInvertCtl.Data = 0;

    CrOffset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_REG +
      (
        (
          MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_1_REG -
          MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_ORDER_CARRY_INVERT_CTL_MCMAIN_0_REG
        ) * Channel
      );

    if (FieldOrder == NULL || WrapTriggerEn == NULL || WrapCarryEn == NULL || AddrInvertEn == NULL) {
      ReutChSeqBaseAddrOrderCarryInvertCtl.Data = MrcReadCR (MrcData, CrOffset);
    }

    if (FieldOrder != NULL) {
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Column_Address_Order = FieldOrder[MrcReutFieldCol];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Row_Address_Order    = FieldOrder[MrcReutFieldRow];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Bank_Address_Order   = FieldOrder[MrcReutFieldBank];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Rank_Address_Order   = FieldOrder[MrcReutFieldRank];

#ifdef MRC_DEBUG_PRINT
      if (EnableDebug) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Order:\n\tField\tInput\t\tStruct\t\t\n");
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tCol:\t0x%x\t\t0x%x\t\t\n",
          FieldOrder[MrcReutFieldCol],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Column_Address_Order
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRow:\t0x%x\t\t0x%x\t\t\n",
          FieldOrder[MrcReutFieldRow],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Row_Address_Order
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tBank:\t0x%x\t\t0x%x\t\t\n",
          FieldOrder[MrcReutFieldBank],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Bank_Address_Order
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRank:\t0x%x\t\t0x%x\t\t\n",
          FieldOrder[MrcReutFieldRank],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Rank_Address_Order
          );
      }
#endif
    }

    if (WrapTriggerEn != NULL) {
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Column_Base_Wrap_Trigger_Enable = WrapTriggerEn[MrcReutFieldCol];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Row_Base_Wrap_Trigger_Enable    = WrapTriggerEn[MrcReutFieldRow];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Bank_Base_Wrap_Trigger_Enable   = WrapTriggerEn[MrcReutFieldBank];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Rank_Base_Wrap_Trigger_Enable   = WrapTriggerEn[MrcReutFieldRank];
#ifdef MRC_DEBUG_PRINT
      if (EnableDebug) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "WrapT:\n\tField\tInput\t\tStruct\t\t\n");
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tCol:\t0x%x\t\t0x%x\t\t\n",
          WrapTriggerEn[MrcReutFieldCol],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Column_Base_Wrap_Trigger_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRow:\t0x%x\t\t0x%x\t\t\n",
          WrapTriggerEn[MrcReutFieldRow],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Row_Base_Wrap_Trigger_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tBank:\t0x%x\t\t0x%x\t\t\n",
          WrapTriggerEn[MrcReutFieldBank],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Bank_Base_Wrap_Trigger_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRank:\t0x%x\t\t0x%x\t\t\n",
          WrapTriggerEn[MrcReutFieldRank],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Rank_Base_Wrap_Trigger_Enable
          );
      }
#endif
    }

    if (WrapCarryEn != NULL) {
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Column_Base_Wrap_Carry_Enable = WrapCarryEn[MrcReutFieldCol];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Row_Base_Wrap_Carry_Enable    = WrapCarryEn[MrcReutFieldRow];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Bank_Base_Wrap_Carry_Enable   = WrapCarryEn[MrcReutFieldBank];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Rank_Base_Wrap_Carry_Enable   = WrapCarryEn[MrcReutFieldRank];

#ifdef MRC_DEBUG_PRINT
      if (EnableDebug) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "WrapC:\n\tField\tInput\t\tStruct\t\t\n");
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tCol:\t0x%x\t\t0x%x\t\t\n",
          WrapCarryEn[MrcReutFieldCol],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Column_Base_Wrap_Carry_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRow:\t0x%x\t\t0x%x\t\t\n",
          WrapCarryEn[MrcReutFieldRow],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Row_Base_Wrap_Carry_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tBank:\t0x%x\t\t0x%x\t\t\n",
          WrapCarryEn[MrcReutFieldBank],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Bank_Base_Wrap_Carry_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRank:\t0x%x\t\t0x%x\t\t\n",
          WrapCarryEn[MrcReutFieldRank],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Rank_Base_Wrap_Carry_Enable
          );
      }
#endif
    }

    if (AddrInvertEn != NULL) {
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Address_Invert_Rate          = AddrInvertRate;
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Column_Base_Address_Invert_Enable = AddrInvertEn[MrcReutFieldCol];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Row_Base_Address_Invert_Enable    = AddrInvertEn[MrcReutFieldRow];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Bank_Base_Address_Invert_Enable   = AddrInvertEn[MrcReutFieldBank];
      ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Rank_Base_Address_Invert_Enable   = AddrInvertEn[MrcReutFieldRank];

#ifdef MRC_DEBUG_PRINT
      if (EnableDebug) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "InvtEn:\n\tField\tInput\t\tStruct\t\t\n");
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tCol:\t0x%x\t\t0x%x\t\t\n",
          AddrInvertEn[MrcReutFieldCol],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Column_Base_Address_Invert_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRow:\t0x%x\t\t0x%x\t\t\n",
          AddrInvertEn[MrcReutFieldRow],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Row_Base_Address_Invert_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tBank:\t0x%x\t\t0x%x\t\t\n",
          AddrInvertEn[MrcReutFieldBank],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Bank_Base_Address_Invert_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRank:\t0x%x\t\t0x%x\t\t\n",
          AddrInvertEn[MrcReutFieldRank],
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Rank_Base_Address_Invert_Enable
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRate:\t0x%x\t\t0x%x\t\t\n",
          AddrInvertRate,
          ReutChSeqBaseAddrOrderCarryInvertCtl.Bits.Base_Address_Invert_Rate
          );
      }
#endif
    }

    MrcWriteCR (MrcData, CrOffset, ReutChSeqBaseAddrOrderCarryInvertCtl.Data);
  }

  if (IncRate != 0 || IncValue != 0) {
    ReutChSeqBaseAddrIncCtl.Data = 0;

    CrOffset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG +
      (
        (MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG)
        * Channel
      );

    if (IncRate == 0 || IncValue == 0) {
      ReutChSeqBaseAddrIncCtl.Data = MrcReadCR64 (MrcData, CrOffset);
    }

    if (IncRate != 0) {
      //
      // RANK
      //
      IncRateScratch = (IncRate[MrcReutFieldRank] > 31) ? (MrcLog2 (IncRate[MrcReutFieldRank] - 1)) :
                                                          (128 + IncRate[MrcReutFieldRank]);
      ReutChSeqBaseAddrIncCtl.Bits.Rank_Base_Address_Update_Rate  = IncRateScratch;
      ReutChSeqBaseAddrIncCtl.Bits.Rank_Base_Address_Update_Scale = IncRateScratch >> 7;
      //
      // BANK
      //
      IncRateScratch = (IncRate[MrcReutFieldBank] > 31) ? (MrcLog2 (IncRate[MrcReutFieldBank] - 1)) :
                                                          (128 + IncRate[MrcReutFieldBank]);
      ReutChSeqBaseAddrIncCtl.Bits.Bank_Base_Address_Update_Rate  = IncRateScratch;
      ReutChSeqBaseAddrIncCtl.Bits.Bank_Base_Address_Update_Scale = IncRateScratch >> 7;
      //
      // ROW
      //
      IncRateScratch = (IncRate[MrcReutFieldRow] > 15) ? (MrcLog2 (IncRate[MrcReutFieldRow] - 1)) :
                                                         (32 + IncRate[MrcReutFieldRow]);
      ReutChSeqBaseAddrIncCtl.Bits.Row_Base_Address_Update_Rate   = IncRateScratch;
      ReutChSeqBaseAddrIncCtl.Bits.Row_Base_Address_Update_Scale  = IncRateScratch >> 5;
      //
      // COL
      //
      IncRateScratch = (IncRate[MrcReutFieldCol] > 31) ? (MrcLog2 (IncRate[MrcReutFieldCol] - 1)) :
                                                         (128 + IncRate[MrcReutFieldCol]);
      ReutChSeqBaseAddrIncCtl.Bits.Column_Base_Address_Update_Rate  = IncRateScratch;
      ReutChSeqBaseAddrIncCtl.Bits.Column_Base_Address_Update_Scale = IncRateScratch >> 7;

#ifdef MRC_DEBUG_PRINT
      if (EnableDebug) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "IncRate:\n\tField\tInput\t\tStruct\t\tScale\n");
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tCol:\t0x%x\t\t0x%x\t\t0x%x\n",
          IncRate[MrcReutFieldCol],
          ReutChSeqBaseAddrIncCtl.Bits.Column_Base_Address_Update_Rate,
          ReutChSeqBaseAddrIncCtl.Bits.Column_Base_Address_Update_Scale
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRow:\t0x%x\t\t0x%x\t\t0x%x\n",
          IncRate[MrcReutFieldRow],
          ReutChSeqBaseAddrIncCtl.Bits.Row_Base_Address_Update_Rate,
          ReutChSeqBaseAddrIncCtl.Bits.Row_Base_Address_Update_Scale
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tBank:\t0x%x\t\t0x%x\t\t0x%x\n",
          IncRate[MrcReutFieldBank],
          ReutChSeqBaseAddrIncCtl.Bits.Bank_Base_Address_Update_Rate,
          ReutChSeqBaseAddrIncCtl.Bits.Bank_Base_Address_Update_Scale
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRank:\t0x%x\t\t0x%x\t\t0x%x\n",
          IncRate[MrcReutFieldRank],
          ReutChSeqBaseAddrIncCtl.Bits.Rank_Base_Address_Update_Rate,
          ReutChSeqBaseAddrIncCtl.Bits.Rank_Base_Address_Update_Scale
          );
      }
#endif
    }

    if (IncValue != 0) {
      ReutChSeqBaseAddrIncCtl.Bits.Rank_Base_Address_Increment    = IncValue[MrcReutFieldRank];
      ReutChSeqBaseAddrIncCtl.Bits.Bank_Base_Address_Increment    = IncValue[MrcReutFieldBank];
      ReutChSeqBaseAddrIncCtl.Bits.Row_Base_Address_Increment     = IncValue[MrcReutFieldRow];
      ReutChSeqBaseAddrIncCtl.Bits.Column_Base_Address_Increment  = IncValue[MrcReutFieldCol] & ColAddrIncMax;

#ifdef MRC_DEBUG_PRINT
      if (EnableDebug) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "IncVal:\n\tField\tInput\t\tStruct\t\t\n");
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tCol:\t0x%x\t\t0x%x\t\t\n",
          IncValue[MrcReutFieldCol],
          ReutChSeqBaseAddrIncCtl.Bits.Column_Base_Address_Increment
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRow:\t0x%x\t\t0x%x\t\t\n",
          IncValue[MrcReutFieldRow],
          ReutChSeqBaseAddrIncCtl.Bits.Row_Base_Address_Increment
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tBank:\t0x%x\t\t0x%x\t\t\n",
          IncValue[MrcReutFieldBank],
          ReutChSeqBaseAddrIncCtl.Bits.Bank_Base_Address_Increment
          );
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "\tRank:\t0x%x\t\t0x%x\t\t\n",
          IncValue[MrcReutFieldRank],
          ReutChSeqBaseAddrIncCtl.Bits.Rank_Base_Address_Increment
          );
      }
#endif
    }

    MrcWriteCR64 (MrcData, CrOffset, ReutChSeqBaseAddrIncCtl.Data);
  }
}

/**
  Programs all the key registers to define a CPCG test

  @param[in] MrcData     - Include all MRC global data.
  @param[in] WDBPattern  - Structure that stores start, Stop, IncRate and Dqpat for pattern.
  @param[in] ChbitMask   - Channel Bit mak for which test should be setup for.
  @param[in] CmdPat      - [0: PatWrRd (Standard Write/Read Loopback),
                            1: PatWr (Write Only),
                            2: PatRd (Read Only),
                            3: PatRdWrTA (ReadWrite Turnarounds),
                            4: PatWrRdTA (WriteRead Turnarounds),
                            5: PatODTTA (ODT Turnaround]
  @param[in] NumCL       - Number of Cache lines
  @param[in] LC          - Loop Count exponent
  @param[in] REUTAddress - Structure that stores start, stop and increment details for address
  @param[in] SOE         - [0: Never Stop, 1: Stop on Any Lane, 2: Stop on All Byte, 3: Stop on All Lane]
  @param[in] WDBPattern  - Structure that stores start, Stop, IncRate and Dqpat for pattern.
  @param[in] EnCADB      - Enable test to write random deselect packages on bus to create xtalk/isi
  @param[in] EnCKE       - Enable CKE power down by adding 64
  @param[in] SubSeqWait  - # of Dclks to stall at the end of a sub-sequence

  @retval Nothing
**/
void
SetupIOTest(
  IN MrcParameters *const         MrcData,
  IN const U8                     ChbitMask,
  IN const U8                     CmdPat,
  IN const U16                    NumCL,
  IN const U8                     LC,
  IN const MRC_REUTAddress *const REUTAddress,
  IN const U8                     SOE,
  IN MRC_WDBPattern *const        WDBPattern,
  IN const U8                     EnCADB,
  IN const U8                     EnCKE,
  IN U16                          SubSeqWait
  )
{
  const MrcDebug  *Debug;
  const MrcInput  *Inputs;
  const U8        WrapCarryEn[MrcReutFieldMax]   = {0, 0, 0, 0}; // Not used in training tests
  const U8        WrapTriggerEn[MrcReutFieldMax] = {0, 0, 0, 0}; // Not used in training tests
  const U8        AddrInvertEn[MrcReutFieldMax]  = {0, 0, 0, 0}; // Not used in training tests
  MrcOutput       *Outputs;
  U8              Channel;
  S8              LCeff;
  U32             LoopCountLinear;
  U8              Mux0;
  U8              Reload;
  U8              Save;
  U8              NumCLCR;
  U8              NumCL2CR;
  U16             Wait;
  U16             NumCL2;
  U32             LMNFreq[2];
  U32             Offset;
  U8              SubSeqStart;
  U8              SubSeqEnd;
  U8              Index;
  MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_STRUCT              ReutChPatCadbCtrl;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT               ReutChSeqCfg;
  MCSCHEDS_CR_PM_PDWN_CONFIG_STRUCT                       PmPdwnConfig;
  MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_STRUCT_HSW_A0    ReutSubSeqCtl0HswA0;
  MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_STRUCT           ReutSubSeqCtl0;
  U32                                                     ReutSubSeqCtl0Data;
  MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_STRUCT           ReutSubSeqCtl1;
  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_STRUCT            ReutChPatWdbCl;
  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_STRUCT         ReutChPatWdbClMuxCfg;
  MCHBAR_CH0_CR_REUT_CH_ERR_CTL_STRUCT                    ReutChErrCtrl;
  MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_STRUCT     ReutChSeqDummyReadCtl;
  struct LocalSubSeqCtl {
    U8  ValidMask;
    MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_STRUCT Ctl[8];
  } SubSeq;

  Outputs = &MrcData->SysOut.Outputs;
  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;

  //
  // Prepare variables needed for both channels
  //
  // Check for the cases where this MUST be 1: When we manually walk through SUBSEQ ODT and TAWR
  //
  LCeff = LC - MrcLog2 (NumCL - 1) + 1;
  if ((LCeff < 1) || (CmdPat == PatWrRdTA) || (CmdPat == PatODTTA)) {
    LCeff = 1;
  }

  LoopCountLinear = 1 << (LCeff - 1);

  if (NumCL > 127) {
    NumCLCR = MrcLog2 (NumCL - 1);       // Assume Exponential number
  } else {
    NumCLCR = (U8) NumCL + (MRC_BIT0 << 7);  // Set Number of Cache Lines as linear number
  }

  NumCL2 = 2 * NumCL;
  if (NumCL2 > 127) {
    NumCL2CR = MrcLog2 (NumCL2 - 1);    // Assume Exponential number
  } else {
    NumCL2CR = (U8) NumCL2 + (MRC_BIT0 << 7); // Set Number of Cache Lines as linear number
  }

  Reload = MrcLog2 (WDBPattern->IncRate - 1);
  //
  // @todo: 'Save' is initialized but never used.
  //
  Save = Reload + MrcLog2 ((WDBPattern->Stop - WDBPattern->Start - 1) & 0xFF);

  if (WDBPattern->IncRate > 31) {
    WDBPattern->IncRate = Reload;
  } else {
    WDBPattern->IncRate += 32;
  }

  if (EnCKE) {
    //
    // @todo: Need to check that PDWN is programmed already.
    //
    PmPdwnConfig.Data = MrcReadCR (MrcData, MCSCHEDS_CR_PM_PDWN_CONFIG_REG);
    Wait = (U16) (PmPdwnConfig.Bits.PDWN_idle_counter + 16); // Adding extra DCKs, 16,  to make sure we make it to power down.
    if (Wait > SubSeqWait) {
      SubSeqWait = Wait;
    }
  }

  if (SubSeqWait > 0xFF) {
    SubSeqWait = 0xFF;
  }
  //
  // Per channel settings
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (!((MRC_BIT0 << Channel) & ChbitMask)) {
      Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel);
      MrcWriteCR (MrcData, Offset, 0);   // Clear global control
      continue;
    }

    //###########################################################
    //
    // Program CADB
    //
    //###########################################################
    Offset = MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_REG +
      ((MCHBAR_CH1_CR_REUT_CH_PAT_CADB_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_CADB_CTRL_REG) * Channel);
    ReutChPatCadbCtrl.Data = 0;
    ReutChPatCadbCtrl.Bits.Enable_CADB_on_Deselect = EnCADB;
    MrcWriteCR8 (MrcData, Offset, (U8) ReutChPatCadbCtrl.Data);
    if (EnCADB) {
      SetupCADB (MrcData, Channel, 7, 8, 0);   // LMNEn=0
    }

    //###########################################################
    //
    // Program Sequence
    //
    //###########################################################
    SubSeqStart = SubSeqEnd = 0;
    switch (CmdPat) {
      case PatWrRd:
        SubSeqEnd = 1;
        break;

      case PatWr:
        break;

      case PatRd:
        SubSeqStart = SubSeqEnd = 1;
        break;

      case PatRdWrTA:
        break;

      case PatWrRdTA:
        SubSeqEnd = 7;
        break;

      case PatODTTA:
        SubSeqEnd = 3;
        break;

      default:
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "SetupIOTest: Unknown value for Pattern\n");
        break;
    }
    ReutChSeqCfg.Data                             = 0;
    ReutChSeqCfg.Bits.Subsequence_Start_Pointer   = SubSeqStart;
    ReutChSeqCfg.Bits.Subsequence_End_Pointer     = SubSeqEnd;
    ReutChSeqCfg.Bits.Initialization_Mode         = REUT_Testing_Mode;
    ReutChSeqCfg.Bits.Global_Control              = 1;
    ReutChSeqCfg.Bits.Enable_Dummy_Reads          = MIN (
                                                      Outputs->EnDumRd,
                                                      MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Enable_Dummy_Reads_MAX
                                                      );
    if (CmdPat == DimmTest) { // Inc address based on LC
      ReutChSeqCfg.Bits.Address_Update_Rate_Mode  = 1;
    }
    ReutChSeqCfg.Bits.Start_Test_Delay            = 2;

    if (
        (Inputs->CpuModel == cmHSW && Inputs->CpuStepping < csHswC0) ||
        (Inputs->CpuModel == cmCRW && Inputs->CpuStepping < csCrwC0) ||
        (Inputs->CpuModel == cmHSW_ULT && Inputs->CpuStepping < csHswUltC0)
       ) {
      ReutChSeqCfg.Bits.Loopcount = MIN (LCeff, MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_Loopcount_MAX);
    } else {
      Offset = MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG +
        (
          (MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG) *
          Channel
        );
      MrcWriteCR (MrcData, Offset, LoopCountLinear);
    }

    Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
      ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel);
    MrcWriteCR64 (MrcData, Offset, ReutChSeqCfg.Data);
    //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "SetupIOTest: C%d REUT_CH_SEQ_CFG_0 = 0x%X %X\n", Channel, ReutChSeqCfg.Data32[1], ReutChSeqCfg.Data32[0]);

    Offset = MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_REG +
      ((MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_REG) * Channel);
    MrcWriteCR8 (MrcData, Offset, MCDFXS_CR_REUT_CH_SEQ_CTL_MCMAIN_0_Local_Clear_Errors_MSK);

    //###########################################################
    //
    // Program Sub Sequences
    //
    //###########################################################
    if ((Inputs->CpuModel == cmHSW) && (Inputs->CpuStepping == csHswA0)) {
      ReutSubSeqCtl0HswA0.Data                                     = 0;
      ReutSubSeqCtl0HswA0.Bits.Number_of_Cachelines                = NumCLCR;
      ReutSubSeqCtl0HswA0.Bits.Number_of_Cachelines_Scale          = NumCLCR >> 7;
      ReutSubSeqCtl0HswA0.Bits.Reset_Current_Base_Address_To_Start = 1;
      ReutSubSeqCtl0HswA0.Bits.Subsequence_Wait                    = SubSeqWait;
      ReutSubSeqCtl0Data = ReutSubSeqCtl0HswA0.Data;
    } else {
      ReutSubSeqCtl0.Data                                     = 0;
      ReutSubSeqCtl0.Bits.Number_of_Cachelines                = NumCLCR;
      ReutSubSeqCtl0.Bits.Number_of_Cachelines_Scale          = NumCLCR >> 7;
      ReutSubSeqCtl0.Bits.Reset_Current_Base_Address_To_Start = 1;
      ReutSubSeqCtl0.Bits.Subsequence_Wait                    = SubSeqWait;
      ReutSubSeqCtl0Data = ReutSubSeqCtl0.Data;
    }

    ReutSubSeqCtl1.Data                            = ReutSubSeqCtl0Data;
    ReutSubSeqCtl1.Bits.Number_of_Cachelines       = NumCL2CR;
    ReutSubSeqCtl1.Bits.Number_of_Cachelines_Scale = NumCL2CR >> 7;

    switch (CmdPat) {
      case PatWrRdTA:
        SubSeq.Ctl[0].Data                        = ReutSubSeqCtl0Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[0].Data, BWr);            // Write CMD
        for (Index = 1; Index <= 6; Index++) {
          SubSeq.Ctl[Index].Data                  = ReutSubSeqCtl1.Data;
          SetSubsequenceType (MrcData, &SubSeq.Ctl[Index].Data, BRdWr);    // Read-Write CMD
        }
        SubSeq.Ctl[7].Data                        = ReutSubSeqCtl0Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[7].Data, BRd);            // Read CMD
        SubSeq.ValidMask                          = 0xFF;
        break;

      case PatRdWrTA:
        SubSeq.Ctl[0].Data                        = ReutSubSeqCtl1.Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[0].Data, BWrRd);          // Write-Read CMD
        SubSeq.ValidMask                          = 0x01;
        break;

      case PatODTTA:
        SubSeq.Ctl[0].Data                        = ReutSubSeqCtl0Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[0].Data, BWr);            // Write CMD

        SubSeq.Ctl[1].Data                        = ReutSubSeqCtl1.Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[1].Data, BRdWr);          // Read-Write CMD

        SubSeq.Ctl[2].Data                        = ReutSubSeqCtl0Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[2].Data, BRd);            // Read CMD

        SubSeq.Ctl[3].Data                        = ReutSubSeqCtl1.Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[3].Data, BWrRd);          // Write-Read CMD

        SubSeq.ValidMask                          = 0x0F;
        break;

      default:
        SubSeq.Ctl[0].Data                        = ReutSubSeqCtl0Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[0].Data, BWr);            // Write CMD

        SubSeq.Ctl[1].Data                        = ReutSubSeqCtl0Data;
        SetSubsequenceType (MrcData, &SubSeq.Ctl[1].Data, BRd);            // Read CMD

        SubSeq.ValidMask                          = 0x03;
        break;
    }
    Offset = MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG +
      ((MCDFXS_CR_REUT_CH1_SUBSEQ_CTL_MCMAIN_0_REG - MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG) * Channel);
    for (Index = 0; Index < 8; Index++) {
      if (SubSeq.ValidMask & (MRC_BIT0 << Index)) {
        MrcWriteCR (MrcData, Offset, SubSeq.Ctl[Index].Data);
        Offset += MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH0_SUBSEQ_CTL_MCMAIN_0_REG;
      } else {
        break;
      }
    }

    //###########################################################
    //
    // Program Sequence Address
    //
    //###########################################################
    MrcProgramSequenceAddress (
      MrcData,
      Channel,
      REUTAddress->Start,
      REUTAddress->Stop,
      REUTAddress->Order,
      REUTAddress->IncRate,
      REUTAddress->IncVal,
      WrapCarryEn,
      WrapTriggerEn,
      AddrInvertEn,
      0,
      FALSE
      );

    //###########################################################
    //
    // Program Write Data Buffer Related Entries
    //
    //###########################################################
    ReutChPatWdbCl.Data                     = 0;
    ReutChPatWdbCl.Bits.WDB_End_Pointer     = WDBPattern->Stop;
    ReutChPatWdbCl.Bits.WDB_Start_Pointer   = WDBPattern->Start;
    ReutChPatWdbCl.Bits.WDB_Increment_Rate  = WDBPattern->IncRate;
    ReutChPatWdbCl.Bits.WDB_Increment_Scale = WDBPattern->IncRate >> MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_Increment_Rate_WID;
    Offset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG +
      ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG) * Channel);
    MrcWriteCR (MrcData, Offset, ReutChPatWdbCl.Data);

    ReutChPatWdbClMuxCfg.Data = 0;

    //
    // Enable LMN in either LMN mode or CADB -to create lots of supply noise
    //
    Mux0  = ((WDBPattern->DQPat == LMNVa) || (WDBPattern->DQPat == CADB)) ? LMNMode : LFSRMode;

    ReutChPatWdbClMuxCfg.Bits.ECC_Data_Source_Sel = 1;
    ReutChPatWdbClMuxCfg.Bits.Mux2_Control        = LFSRMode;
    ReutChPatWdbClMuxCfg.Bits.Mux1_Control        = LFSRMode;
    ReutChPatWdbClMuxCfg.Bits.Mux0_Control        = Mux0;      // ECC, Select LFSR
    //
    // Program LFSR Save/Restore.  Too complex unless everything is power of 2
    //
    if ((CmdPat == PatODTTA) || (CmdPat == PatWrRdTA)) {
      ReutChPatWdbClMuxCfg.Bits.Reload_LFSR_Seed_Rate = MrcLog2 (NumCL - 1) + 1;
      ReutChPatWdbClMuxCfg.Bits.Save_LFSR_Seed_Rate   = 1;
    }

    Offset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG +
      ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_MUX_CFG_REG) * Channel);
    MrcWriteCR (MrcData, Offset, ReutChPatWdbClMuxCfg.Data);

    //
    // Currently, not planning to use the Inversion Mask
    //
    Offset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_INV_REG + ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_INV_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_INV_REG) * Channel);
    MrcWriteCR (MrcData, Offset, 0);

    //###########################################################
    //
    // Program Error Checking
    //
    //###########################################################

    //
    // Enable selective_error_enable_chunk and selective_error_enable_cacheline, mask later
    // the bits we don't want to check.
    //
    Offset = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG +
      ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * Channel);
    ReutChErrCtrl.Data                                  = 0;
    ReutChErrCtrl.Bits.Stop_on_Nth_Error                = 1;
    ReutChErrCtrl.Bits.Stop_On_Error_Control            = SOE;
    ReutChErrCtrl.Bits.Selective_Error_Enable_Chunk     = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Chunk_MAX;
    ReutChErrCtrl.Bits.Selective_Error_Enable_Cacheline = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Cacheline_MAX;
    MrcWriteCR (MrcData, Offset, ReutChErrCtrl.Data);

    Offset = MCHBAR_CH0_CR_REUT_CH_ERR_DATA_MASK_REG +
      ((MCHBAR_CH1_CR_REUT_CH_ERR_DATA_MASK_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_MASK_REG) * Channel);
    MrcWriteCR64 (MrcData, Offset, 0);

    Offset = MCHBAR_CH0_CR_REUT_CH_ERR_ECC_MASK_REG +
      ((MCHBAR_CH1_CR_REUT_CH_ERR_ECC_MASK_REG - MCHBAR_CH0_CR_REUT_CH_ERR_ECC_MASK_REG) * Channel);
    MrcWriteCR8 (MrcData, Offset, 0);

    //###########################################################
    //
    // Program Dummy Read
    //
    //###########################################################
    if (Outputs->EnDumRd) {
      //
      // REUT traffic only uses BA[1:0] - Mask BANK that will not be used
      //
      Offset = MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_MASK_MCMAIN_0_REG +
        (
          (MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_MASK_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_MASK_MCMAIN_0_REG) *
          Channel
        );
      MrcWriteCR8 (MrcData, Offset, 0xFC);

      //
      // Rotated from 40nS to 200nS
      //
      if (Outputs->Qclkps > 0) {
        LMNFreq[0] = (40000 / Outputs->Qclkps);
        LMNFreq[1] = (200000 / Outputs->Qclkps);
      } else {
        LMNFreq[0] = LMNFreq[1] = 0xFF;
      }

      ReutChSeqDummyReadCtl.Data                        = 0;
      ReutChSeqDummyReadCtl.Bits.L_counter              = LMNFreq[0];
      ReutChSeqDummyReadCtl.Bits.M_counter              = LMNFreq[0];
      ReutChSeqDummyReadCtl.Bits.N_Counter              = LMNFreq[1];
      ReutChSeqDummyReadCtl.Bits.Enable_Sweep_Frequency = 1;
      //
      // Chirp Freq from 5 to 25 MHz
      //
      Offset = MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_REG + (
          (MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_DUMMYREAD_CTL_MCMAIN_0_REG) * Channel
        );
      MrcWriteCR (MrcData, Offset, ReutChSeqDummyReadCtl.Data);
    }
  }
  //
  // Always do a ZQ Short before the beginning of a test
  //
  MrcIssueZQ (MrcData, ChbitMask, MRC_ZQ_SHORT);

  return;
}

/**
  This function sets up a test with CADB for the given channel mask.

  @param[in,out] MrcData   - Pointer to MRC global data.
  @param[in]     ChbitMask - Bit masks of channels to enable for the test.
  @param[in]     LC        - Exponential umber of loops to run the test.
  @param[in]     SOE       - Error handling switch for test.
  @param[in]     EnCADB    - Switch to enable CADB
  @param[in]     EnCKE     - Switch to enable CKE.

  @retval Nothing
**/
void
SetupIOTestCADB (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             ChbitMask,
  IN     const U8             LC,
  IN     const U8             SOE,
  IN     const U8             EnCADB,
  IN     const U8             EnCKE
  )
{
  const MRC_REUTAddress REUTAddress = {
    // Rank, Bank, Row, Col
    {    0,  0,    0,    0 },  // Start
    {    0,  7, 2047, 1023 },  // Stop
    {    0,  0,    0,    0 },  // Order
    {   32,  3,    3,    0 },  // IncRate
    {    1,  1,   73,   53 }   // IncValue
  };
  MRC_WDBPattern  WDBPattern;
  MrcOutput       *Outputs;
  U16             NumCL;

  Outputs            = &MrcData->SysOut.Outputs;
  WDBPattern.IncRate = 4;
  WDBPattern.Start   = 0;
  WDBPattern.Stop    = 9;
  WDBPattern.DQPat   = CADB;

  NumCL = 128;

  SetupIOTest (MrcData, ChbitMask, PatWrRd, NumCL, LC, &REUTAddress, SOE, &WDBPattern, EnCADB, EnCKE, 0);

  Outputs->DQPatLC = LC - 2 - 3 + 1;
  if (Outputs->DQPatLC < 1) {
    Outputs->DQPatLC = 1;
  }

  Outputs->DQPat = CADB;
  return;
}

/**
  This function sets up a basic victim-aggressor test for the given channel mask.

  @param[in,out] MrcData   - Pointer to MRC global data.
  @param[in]     ChbitMask - Bit masks of channels to enable for the test.
  @param[in]     LC        - Exponential umber of loops to run the test.
  @param[in]     SOE       - Error handling switch for test.
  @param[in]     EnCADB    - Switch to enable CADB
  @param[in]     EnCKE     - Switch to enable CKE.
  @param[in]     Spread    - Stopping point of the pattern.

  @retval Nothing
**/
void
SetupIOTestBasicVA (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             ChbitMask,
  IN     const U8             LC,
  IN     const U8             SOE,
  IN     const U8             EnCADB,
  IN     const U8             EnCKE,
  IN     const U32            Spread
  )
{
  const MRC_REUTAddress REUTAddress = {
    // Rank, Bank, Row, Col
    {    0,  0,    0,    0 },  // Start
    {    0,  0,    0, 1023 },  // Stop
    {    0,  0,    0,    0 },  // Order
    {   32,  0,    0,    0 },  // IncRate
    {    1,  0,    0,    1 }   // IncValue
  };
  
  MRC_WDBPattern        WDBPattern;
  MrcOutput             *Outputs;
  U16                   NumCL;

  Outputs            = &MrcData->SysOut.Outputs;
  WDBPattern.IncRate = 4;
  WDBPattern.Start   = 0;
  WDBPattern.Stop    = Spread - 1;
  WDBPattern.DQPat   = BasicVA;

  NumCL = 128;

  SetupIOTest (MrcData, ChbitMask, PatWrRd, NumCL, LC, &REUTAddress, SOE, &WDBPattern, EnCADB, EnCKE, 0);

  Outputs->DQPatLC = LC - 8 + 1;
  if (Outputs->DQPatLC < 1) {
    Outputs->DQPatLC = 1;
  }

  Outputs->DQPat = BasicVA;
  return;
}

/**
  This function sets up a DQ test for the given channel mask.

  @param[in,out] MrcData   - Pointer to MRC global data.
  @param[in]     ChbitMask - Bit masks of channels to enable for the test.
  @param[in]     LC        - Exponential umber of loops to run the test.
  @param[in]     SOE       - Error handling switch for test.
  @param[in]     EnCADB    - Switch to enable CADB
  @param[in]     EnCKE     - Switch to enable CKE.

  @retval Nothing
**/
void
SetupIOTestDQ (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             ChbitMask,
  IN     const U8             LC,
  IN     const U8             SOE,
  IN     const U8             EnCADB,
  IN     const U8             EnCKE
  )
{
  const MRC_REUTAddress REUTAddress = {{0, 0, 0, 0},        // Start
  {0, 1, 512, 1023},   // Stop
  {0, 0, 0, 0},        // Order
  {2047, 255, 255, 0}, // IncRate
  {1, 1, 512, 1}};     // IncValue
  MRC_WDBPattern        WDBPattern;
  MrcOutput             *Outputs;
  U16                   NumCL;

  Outputs            = &MrcData->SysOut.Outputs;
  WDBPattern.IncRate = 32;
  WDBPattern.Start   = 0;
  WDBPattern.Stop    = 63;
  WDBPattern.DQPat   = SegmentWDB;

  NumCL = 256;

  SetupIOTest (MrcData, ChbitMask, PatWrRd, NumCL, LC, &REUTAddress, SOE, &WDBPattern, EnCADB, EnCKE, 0);

  Outputs->DQPatLC = LC - 8 - 3 + 1;
  if (Outputs->DQPatLC < 1) {
    Outputs->DQPatLC = 1;
  }

  Outputs->DQPat = SegmentWDB;
  return;
}

/**
  This function sets up a test with CADB for the given channel mask.

  @param[in,out] MrcData   - Pointer to MRC global data.
  @param[in]     ChbitMask - Bit masks of channels to enable for the test.
  @param[in]     LC        - Exponential umber of loops to run the test.
  @param[in]     SOE       - Error handling switch for test.
  @param[in]     EnCADB    - Switch to enable CADB
  @param[in]     EnCKE     - Switch to enable CKE.

  @retval Nothing
**/
void
SetupIOTestC2C (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             ChbitMask,
  IN     const U8             LC,
  IN     const U8             SOE,
  IN     const U8             EnCADB,
  IN     const U8             EnCKE
  )
{
  const MRC_REUTAddress REUTAddress = {{0, 0, 0, 0},    // Start
  {0, 0, 0, 1023}, // Stop
  {0, 0, 0, 0},    // Order
  {2047, 0, 0, 0}, // IncRate
  {1, 0, 0, 1}};   // IncValue
  MRC_WDBPattern        WDBPattern;
  MrcOutput             *Outputs;

  Outputs            = &MrcData->SysOut.Outputs;
  WDBPattern.IncRate = 32;
  WDBPattern.Start   = 0;
  WDBPattern.Stop    = 63;
  WDBPattern.DQPat   = SegmentWDB;

  SetupIOTest (MrcData, ChbitMask, PatWrRd, 32, LC, &REUTAddress, SOE, &WDBPattern, EnCADB, EnCKE, 0);

  Outputs->DQPatLC = LC - 5 + 1;
  if (Outputs->DQPatLC < 1) {
    Outputs->DQPatLC = 1;
  }

  Outputs->DQPat = SegmentWDB;
  return;
}

/**
  This function sets up a MPR test for the given channel mask.

  @param[in,out] MrcData   - Pointer to MRC global data.
  @param[in]     ChbitMask - Bit masks of channels to enable for the test.
  @param[in]     LC        - Exponential umber of loops to run the test.
  @param[in]     SOE       - Error handling switch for test.
  @param[in]     EnCADB    - Switch to enable CADB
  @param[in]     EnCKE     - Switch to enable CKE.

  @retval Nothing
**/
void
SetupIOTestMPR (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             ChbitMask,
  IN     const U8             LC,
  IN     const U8             SOE,
  IN     const U8             EnCADB,
  IN     const U8             EnCKE
  )
{
  const MRC_REUTAddress REUTAddress_ddr = {
    { 0,  0, 0, 0    }, // Start
    { 0,  0, 0, 1023 }, // Stop
    { 0,  0, 0, 0    }, // Order
    { 32, 0, 0, 0    }, // IncRate
    { 1,  0, 0, 1    }  // IncValue
  };
  const MRC_REUTAddress REUTAddress_lpddr = {
    { 0, 4, 0, 0 },     // Start
    { 0, 4, 0, 0 },     // Stop
    { 0, 0, 0, 0 },     // Order
    { 0, 0, 0, 0 },     // IncRate
    { 0, 0, 0, 0 }      // IncValue
  };
  const MRC_REUTAddress *ReutAddress;
  MRC_WDBPattern        WDBPattern;
  MrcOutput             *Outputs;
  U16                   NumCL;

  Outputs            = &MrcData->SysOut.Outputs;
  WDBPattern.IncRate = 4;
  WDBPattern.Start   = 0;
  WDBPattern.Stop    = 9;
  WDBPattern.DQPat   = BasicVA;

  NumCL = 128;

  if (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3) {
    ReutAddress = &REUTAddress_lpddr;
  } else {
    ReutAddress = &REUTAddress_ddr;
  }

  SetupIOTest (MrcData, ChbitMask, PatRd, NumCL, LC, ReutAddress, SOE, &WDBPattern, EnCADB, EnCKE, 0);

  Outputs->DQPatLC = 1;
  Outputs->DQPat   = BasicVA;
  return;
}

/**
  Runs one or more REUT tests (based on TestType)

  @param[in] MrcData     - Include all MRC global data.
  @param[in] ChbitMask   - Channel Bit mask for which test should be setup for.
  @param[in] DQPat       - [0: BasicVA
                            1: SegmentWDB
                            2: CADB
                            3: TurnAround
                            4: LMNVa
                            5: TurnAroundWR
                            6: TurnAroundODT
                            7: RdRdTA]
  @param[in] SeqLCs      - An array of one or more loopcounts.
  @param[in] ClearErrors - Decision to clear or not errors.
  @param[in] Mode        - Allows for different types of modes for margining
                           {Bit0: PhLock (keep all bytes within in ch in phase),
                            Bit1: Ch2Ch Data out of phase (LFSR seed)
                            Bits 15:2: Reserved}

  @retval Returns ch errors
**/
U8
RunIOTest (
  IN MrcParameters *const MrcData,
  IN const U8             ChbitMask,
  IN const U8             DQPat,
  IN const U8 *const      SeqLCs,
  IN const U8             ClearErrors,
  IN const U16            Mode
  )
{
  const MrcDebug    *Debug;
  MrcInput          *Inputs;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  U8                ch;
  U8                Reload;
  U8                NumTests;
  U8                t;
  U8                IncRate;
  U8                TestSOE;
  U8                TestDoneStatus;
  U8                ErrorStatus;
  U32               CRValue;
  U32               TestRand;
  U32               Offset;
  U32               LoopCountLinear;
  U8                tRDRD_dr_Min[MAX_CHANNEL];
  U8                TurnAroundOffset;
  // When we segment the WDB, we run a normal 2 LFSR VA pattern on the first 10 entries
  // The last 54 entries are used for a more complex 3 LFSR pattern
  // In this mode:
  // SeqLC is usually [0: host.DQPatLC, 1: host.DQPatLC, 2: host.DQPatLC+4, 3: host.DQPatLC+2]
  //
  // Anotherwords:
  // The first 10 entries of the LFSR are run for twice, each for 2^DQPatLC
  //   and the WDB is incremented every 25 cachelines
  //
  // 25 was chosen since 10 Entry * 25 cachelines = 250.
  // This is pretty close to 256, a power of 2, which should be roughly uniform coverage across all entries
  //
  // The second 54 entries of the LFSR are run twice
  //            Once with 2^(DQPatLC+4) and the WDB is incremented every 19 cachelines
  //            Once with 2^(DQPatLC+2) and the WDB is incremented every 10 cachelines
  //            Again, 19*54 = 1026 and 10*54 = 540 and both of these numbers are close
  //             to power of 2 and will provide roughly uniform coverage
  //
  // Each entry in the first 10 entries is hit 2 ^ (DQPatLC + NumCachelines + 1) / 10
  //           or 2 ^ (DQPatLC + NumCachelines -2.32)
  //
  // Each entry in the second 54 entries is hit 2 ^ (DQPatLC + NumCachelines + 4.32) / 54
  //           or ~2 ^ (DQPatLC + NumCachelines -1.43)
  //           or ~2x more than the first 10 entries

  U8 WDBIncRates[8];
  U8 WDBStart[8];
  U8 WDBStop[8];

  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT                 ReutGlobalCtl;
  MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_STRUCT                 ReutGlobalErr;
  MCHBAR_CH0_CR_REUT_CH_ERR_CTL_STRUCT                    ReutChErrCtl;
  MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_STRUCT            ReutChPatWdbCl;
  MCHBAR_CH0_CR_TC_BANK_RANK_A_STRUCT                     TcBankRankA[MAX_CHANNEL];

  TestSOE       = 0;
  Inputs        = &MrcData->SysIn.Inputs;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  Debug         = &MrcData->SysIn.Inputs.Debug;
  MrcOemMemorySet (WDBIncRates, 1, sizeof (WDBIncRates));
  MrcOemMemorySet (WDBStart, 0, sizeof (WDBStart));
  MrcOemMemorySet (WDBStop, 9, sizeof (WDBStop));
  MrcOemMemorySetDword ((U32 *) TcBankRankA, 0, sizeof (TcBankRankA) / sizeof (TcBankRankA[0]));
  ReutGlobalErr.Data  = 0;
  ErrorStatus         = 0;

  TestRand     = 0xBAD00451;
  NumTests     = 1;
  if (DQPat == SegmentWDB) {
    NumTests = 4;
    WDBIncRates[3] = 10;
    WDBIncRates[2] = 19;
    WDBIncRates[1] = 25;
    WDBIncRates[0] = 25;

    WDBStart[3]    = 10;
    WDBStart[2]    = 10;
    WDBStop[3]     = 63;
    WDBStop[2]     = 63;
  } else if (DQPat == CADB) {
    NumTests = 7;
  } else if (DQPat == TurnAroundWR) {
    NumTests = 8;
  } else if (DQPat == TurnAroundODT) {
    NumTests = 4;
  } else if (DQPat == RdRdTA) {
    NumTests = 2;
    for (ch = 0; ch < MAX_CHANNEL; ch++) {
      if (!((MRC_BIT0 << ch) & ChbitMask)) {
        continue;
      }

      TcBankRankA[ch].Data = ControllerOut->Channel[ch].MchbarBANKRANKA;
    }
  } else if (DQPat == RdRdTA_All) {
    NumTests = 8;
    for (ch = 0; ch < MAX_CHANNEL; ch++) {
      if (((1 << ch) & ChbitMask) == 0) {
        continue;
      }

      TcBankRankA[ch].Data = ControllerOut->Channel[ch].MchbarBANKRANKA;
      tRDRD_dr_Min[ch] = (U8) TcBankRankA[ch].Bits.tRDRD_dr; // save the min value allowed 
    }
  }

  for (t = 0; t < NumTests; t++) {
    //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RunIOTest: t = %d\n",t);
    Reload = MrcLog2 (WDBIncRates[t] - 1);
    if (WDBIncRates[t] > 31) {
      WDBIncRates[t] = Reload;
    } else {
      WDBIncRates[t] += 32;
    }

    for (ch = 0; ch < MAX_CHANNEL; ch++) {
      if (!((MRC_BIT0 << ch) & ChbitMask)) {
        continue;
      }
      //
      // Check for SOE == NTHSOE, ALSOE
      // @todo: I still feel we need to exit if we get  errors on any test
      //
      TestSOE = 0;
      Offset = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG +
        ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * ch);
      ReutChErrCtl.Data = MrcReadCR (MrcData, Offset);
      CRValue = ReutChErrCtl.Bits.Stop_On_Error_Control;
      if ((CRValue == NTHSOE) || (CRValue == ALSOE)) {
        TestSOE = 1; // SOE bits are set
      }

      if (DQPat == SegmentWDB) {
        Offset = MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG +
          ((MCHBAR_CH1_CR_REUT_CH_PAT_WDB_CL_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_REG) * ch);
        ReutChPatWdbCl.Data                     = 0;
        ReutChPatWdbCl.Bits.WDB_Start_Pointer   = WDBStart[t];
        ReutChPatWdbCl.Bits.WDB_End_Pointer     = WDBStop[t];
        ReutChPatWdbCl.Bits.WDB_Increment_Rate  = WDBIncRates[t];
        ReutChPatWdbCl.Bits.WDB_Increment_Scale = WDBIncRates[t] >> MCHBAR_CH0_CR_REUT_CH_PAT_WDB_CL_CTRL_WDB_Increment_Rate_WID;
        MrcWriteCR (MrcData, Offset, ReutChPatWdbCl.Data);

        //
        // Skip programming LFSR Save/Restore.  Too complex unless power of 2
        //
        if (
            (Inputs->CpuModel == cmHSW && Inputs->CpuStepping < csHswC0) ||
            (Inputs->CpuModel == cmCRW && Inputs->CpuStepping < csCrwC0) ||
            (Inputs->CpuModel == cmHSW_ULT && Inputs->CpuStepping < csHswUltC0)
           ) {
          //
          // Program desired loopcount
          //
          Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG + 2 +
            ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * ch);
          MrcWriteCR8 (MrcData, Offset, (SeqLCs[t] + 1));
        } else {
          LoopCountLinear = 1 << SeqLCs[t];
          Offset = MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG +
            (
              (MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_LOOPCOUNT_LIMIT_MCMAIN_0_REG) *
              ch
            );
          MrcWriteCR (MrcData, Offset, LoopCountLinear);
        }

      } else if (DQPat == CADB) {
        SetupCADB (MrcData, ch, NumTests, t, 0);   // LMNEn=0
      } else if ( (DQPat == TurnAroundWR) || (DQPat == TurnAroundODT) ) {
        //
        // Program which subseq to run
        //
        Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG + 3 +
          ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * ch);
        MrcWriteCR8 (MrcData, Offset, (t << 4) + t);

        //
        // Program RankInc Rate
        //
        IncRate =
          (
            ((DQPat == TurnAroundWR) && ((t == 0) || (t == 7))) ||
            ((DQPat == TurnAroundODT) && ((t == 0) || (t == 2)))
          ) ? 0 : 1;

        Offset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG +
        ((
          MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG
         ) * ch
        );
        MrcWriteCR8 (MrcData, Offset + 7, 128 + IncRate); // 0x80+IncRate
        CRValue = MrcReadCR (MrcData, Offset);
        //
        // Program bit 19, 16:12 to IncRate (assume linear mode)
        //
        CRValue = MrcBitSwap (CRValue, (128 + IncRate), 12, 8);
        MrcWriteCR (MrcData, Offset, CRValue);
      } else if (DQPat == RdRdTA) {
        //
        // Program tRDRD parameter
        //
        Offset = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG +
          ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * ch);
        TcBankRankA[ch].Bits.tRDRD = (t == 0) ? 4 : 5;
        MrcWriteCR (MrcData, Offset, TcBankRankA[ch].Data);
      } else if (DQPat == RdRdTA_All) {
        //
        // Program tRDRD for SR and DR
        //  Run 8 tests, Covering tRDRD_sr = 4,5,6,7 and tRDRD_dr = Min,+1,+2,+3 
        //
        TurnAroundOffset = (t % 4);
        Offset = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG +
          ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * ch);
        TcBankRankA[ch].Bits.tRDRD = 4 + TurnAroundOffset;
        TcBankRankA[ch].Bits.tRDRD_dr = tRDRD_dr_Min[ch] + TurnAroundOffset;

        MrcWriteCR (MrcData, Offset, TcBankRankA[ch].Data);
        //
        // Program RankInc Rate
        //
        IncRate = (t > 3)? 0 : 31; // this field + 1 

        Offset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG +
          (
            (
              MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_INC_CTL_MCMAIN_0_REG
            ) * ch
          );
        MrcWriteCR8 (MrcData, Offset + 7, IncRate | MRC_BIT7); // Linear Rank Address Update Rate 
      }
    }

    //###########################################################
    //
    // Start Test and Poll on completion
    //
    //###########################################################
    //
    // IO Reset neded before starting test.
    //
    IoReset (MrcData);

    ReutGlobalCtl.Data                    = 0;
    ReutGlobalCtl.Bits.Global_Start_Test  = 1;
    if (ClearErrors && (t == 0)) {
      ReutGlobalCtl.Bits.Global_Clear_Errors = 1;
    }

    MrcWriteCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, ReutGlobalCtl.Data);

    //
    // Wait until Channel test done status matches ChbitMask
    //
    do {
      ReutGlobalErr.Data = MrcReadCR (MrcData, MCDFXS_CR_REUT_GLOBAL_ERR_MCMAIN_REG);
      TestDoneStatus = (U8) ((ReutGlobalErr.Bits.Channel_Test_Done_Status_1 << 1) | ReutGlobalErr.Bits.Channel_Test_Done_Status_0);
    } while ((TestDoneStatus & ChbitMask) != ChbitMask);

    //
    // Exit if SOE and Channel_Test_Done_Status bits matches ChbitMask
    //
    ErrorStatus = (U8) ((ReutGlobalErr.Bits.Channel_Error_Status_1 << 1) | ReutGlobalErr.Bits.Channel_Error_Status_0);
    if ((ErrorStatus & ChbitMask) && TestSOE) {
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ERROR IN RunIOTest: REUT_GLOBAL_CTRL = %Xh, REUT_GLOBAL_ERR %Xh\n", ReutGlobalErr.Data, ErrorStatus);
      return (ReutGlobalErr.Data & ChbitMask);
    }
  }

  if ((DQPat == RdRdTA) || (DQPat == RdRdTA_All)) {
    //
    // Restore original tRDRD value
    //
    for (ch = 0; ch < MAX_CHANNEL; ch++) {
      if (!((MRC_BIT0 << ch) & ChbitMask)) {
        continue;
      }

      Offset = MCHBAR_CH0_CR_TC_BANK_RANK_A_REG +
        ((MCHBAR_CH1_CR_TC_BANK_RANK_A_REG - MCHBAR_CH0_CR_TC_BANK_RANK_A_REG) * ch);
      MrcWriteCR (MrcData, Offset, ControllerOut->Channel[ch].MchbarBANKRANKA);
    }
  }

  return (ReutGlobalErr.Data & ChbitMask);
}

/**
  Programs REUT to run on the selected physical ranks.

  @param[in] MrcData           - Pointer to MRC global data.
  @param[in] ch                - Channel to enable.
  @param[in] RankBitMask       - Bit mask of ranks to enable.
  @param[in] RankFeatureEnable - RankFeatureEnable is a bit mask that can enable CKE, Refresh or ZQ
                                 RankFeatureEnable[0] enables Refresh on all non-selected ranks
                                 RankFeatureEnable[1] enables Refresh on all ranks
                                 RankFeatureEnable[2] enables ZQ on all non-selected ranks
                                 RankFeatureEnable[3] enables ZQ on all ranks
                                 RankFeatureEnable[4] enables CKE on all non-selected ranks
                                 RankFeatureEnable[5] enables CKE on all ranks

  @retval Bit mask of channel enabled if rank in the channel exists.
**/
U8
SelectReutRanks (
  IN MrcParameters *const MrcData,
  IN const U8             ch,
  IN U8                   RankBitMask,
  IN const U8             RankFeatureEnable
  )
{
  U32 Offset;
  U8  En;
  U8  rank;
  U8  RankCount;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT       ReutChSeqCfg;
  MCHBAR_CH0_CR_REUT_CH_MISC_REFRESH_CTRL_STRUCT  ReutChMiscRefreshCtrl;
  MCHBAR_CH0_CR_REUT_CH_MISC_ZQ_CTRL_STRUCT       ReutChMiscZqCtrl;
  MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_STRUCT      ReutChMiscCkeCtrl;
  MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_STRUCT  ReutChSeqRankL2PMapping;

  //
  // Make sure valid rank bit mask for this channel
  //
  RankBitMask &= MrcData->SysOut.Outputs.Controller[0].Channel[ch].ValidRankBitMask;

  //
  // Check if nothing is selected
  //
  if ((RankBitMask & 0xF) == 0) {
    Offset = MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG +
      (
        (
          MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_1_REG -
          MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG
        ) * ch
      );
    MrcWriteCR (MrcData, Offset, 0);

    //
    // Disable Channel by clearing global start bit in change config
    //
    Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
      ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * ch);
    ReutChSeqCfg.Data                 = MrcReadCR (MrcData, Offset);
    ReutChSeqCfg.Bits.Global_Control  = 0;
    MrcWriteCR (MrcData, Offset, (U32) ReutChSeqCfg.Data);

    return 0;

  } else {
    //
    // Normal case
    // Setup REUT Test to iteration through appropriate ranks during test
    //
    ReutChSeqRankL2PMapping.Data  = 0;
    RankCount                     = 0;

    //
    // Prepare Rank Mapping and Max Rank
    //
    for (rank = 0; rank < MAX_RANK_IN_CHANNEL; rank++) {
      //
      // rank in range(4):
      //
      if ((MRC_BIT0 << rank) & RankBitMask) {
        ReutChSeqRankL2PMapping.Data |= (rank << (4 * RankCount));
        RankCount += 1;
      }
    }
    //
    // Write New Rank Mapping and Max Rank
    //
    Offset = MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG +
      (
        (
          MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_1_REG -
          MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG
        ) * ch
      );
    MrcWriteCR (MrcData, Offset, ReutChSeqRankL2PMapping.Data);
    Offset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG + 7 +
      ((MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_WRAP_MCMAIN_0_REG) * ch);
    MrcWriteCR8 (MrcData, Offset, RankCount - 1);

    //
    // Make sure channel is enabled
    //
    Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG + ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * ch);
    ReutChSeqCfg.Data                 = MrcReadCR (MrcData, Offset);
    ReutChSeqCfg.Bits.Global_Control  = 1;
    MrcWriteCR (MrcData, Offset, (U32) ReutChSeqCfg.Data);
  }
  //
  // Need to convert RankFeatureEnable as an input parameter so we don't pass it all the time
  //
  if (RankFeatureEnable != 0) {
    //
    // Enable Refresh and ZQ - 0's to the the desired ranks
    //
    En = RankFeatureEnable & 0x3; // Refresh
    ReutChMiscRefreshCtrl.Data                    = 0;
    ReutChMiscRefreshCtrl.Bits.Refresh_Rank_Mask  = MCHBAR_CH0_CR_REUT_CH_MISC_REFRESH_CTRL_Refresh_Rank_Mask_MAX;
    ReutChMiscRefreshCtrl.Bits.Panic_Refresh_Only = 1;

    if (En == 1) {
      ReutChMiscRefreshCtrl.Bits.Refresh_Rank_Mask = ~RankBitMask; // Enable all non-selected ranks
    } else if (En > 1) {
      ReutChMiscRefreshCtrl.Bits.Refresh_Rank_Mask = 0;           // Enable all ranks
    }
    Offset = MCHBAR_CH0_CR_REUT_CH_MISC_REFRESH_CTRL_REG +
      ((MCHBAR_CH1_CR_REUT_CH_MISC_REFRESH_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_REFRESH_CTRL_REG) * ch);
    MrcWriteCR (MrcData, Offset, ReutChMiscRefreshCtrl.Data);

    En = (RankFeatureEnable >> 2) & 0x3; // ZQ
    ReutChMiscZqCtrl.Data              = 0;
    ReutChMiscZqCtrl.Bits.ZQ_Rank_Mask = MCHBAR_CH0_CR_REUT_CH_MISC_ZQ_CTRL_ZQ_Rank_Mask_MAX;
    ReutChMiscZqCtrl.Bits.Always_Do_ZQ = 1;
    if (En == 1) {
      ReutChMiscZqCtrl.Bits.ZQ_Rank_Mask = ~RankBitMask;
    } else if (En > 1) {
      ReutChMiscZqCtrl.Bits.ZQ_Rank_Mask = 0; // Enable all ranks
    }
    Offset =  MCHBAR_CH0_CR_REUT_CH_MISC_ZQ_CTRL_REG +
      ((MCHBAR_CH1_CR_REUT_CH_MISC_ZQ_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_ZQ_CTRL_REG) * ch);
    MrcWriteCR (MrcData, Offset, ReutChMiscZqCtrl.Data);

    //
    // Enable CKE ranks - 1's to enable desired ranks
    //
    En = (RankFeatureEnable >> 4) & 0x3;
    ReutChMiscCkeCtrl.Data        = 0;
    ReutChMiscCkeCtrl.Bits.CKE_On = MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_CKE_On_MAX;
    if (En == 1) {
      ReutChMiscCkeCtrl.Bits.CKE_On       = ~RankBitMask;
      ReutChMiscCkeCtrl.Bits.CKE_Override = ~RankBitMask; // Enable all non-selected ranks
    } else if (En > 1) {
      ReutChMiscCkeCtrl.Bits.CKE_On       = MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_CKE_On_MAX;
      ReutChMiscCkeCtrl.Bits.CKE_Override = MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_CKE_Override_MAX; // Enable all ranks.
    }
    Offset = MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG +
      ((MCHBAR_CH1_CR_REUT_CH_MISC_CKE_CTRL_REG - MCHBAR_CH0_CR_REUT_CH_MISC_CKE_CTRL_REG) * ch);
    MrcWriteCR (MrcData, Offset, ReutChMiscCkeCtrl.Data);
  }

  return (U8) (MRC_BIT0 << ch);
}

/**
  This routine updates RXTRAINRANK register's specific fields defined by the subfield
  subfield values:
    0 - Update RcvEn   - leave other parameter the same
    1 - Update RxDqsP  - leave other parameter the same
    2 - Update RxEq    - leave other parameter the same
    3 - Update RxDqsN  - leave other parameter the same
    4 - Update RxVref  - leave other parameter the same
    5 - Update RxDqsP & RxDqsN - leave other parameter the same
    FF - leave all parameter the same

  @param[in] MrcData  - Include all MRC global data.
  @param[in] Channel  - Defines channel to update
  @param[in] Rank     - Defines rank to update
  @param[in] Byte     - Defines byte to update
  @param[in] Subfield - Defines the register's field or fields to update
  @param[in] Value    - value to be writen into register fields

  @retval Nothing
**/
void
UpdateRxT (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             Rank,
  IN const U8             Byte,
  IN const U8             Subfield,
  IN const U16            Value
  )
{
  MrcChannelOut                       *ChannelOut;
  U32                                 Offset;
  DDRDATA0CH0_CR_RXTRAINRANK0_STRUCT  CrRxTrainRank;

  ChannelOut                   = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  CrRxTrainRank.Data           = 0;
  CrRxTrainRank.Bits.RxRcvEnPi = (Subfield == 0) ? Value : ChannelOut->RcvEn[Rank][Byte];
  CrRxTrainRank.Bits.RxDqsPPi  = ((Subfield == 1) || (Subfield == 5)) ? Value : ChannelOut->RxDqsP[Rank][Byte];
  CrRxTrainRank.Bits.RxEq      = (Subfield == 2) ? Value : ChannelOut->RxEq[Rank][Byte];
  CrRxTrainRank.Bits.RxDqsNPi  = ((Subfield == 3) || (Subfield == 5)) ? Value : ChannelOut->RxDqsN[Rank][Byte];
  CrRxTrainRank.Bits.RxVref    = (Subfield == 4) ? Value : ChannelOut->RxVref[Byte];

  Offset = DDRDATA0CH0_CR_RXTRAINRANK0_REG +
    ((DDRDATA0CH1_CR_RXTRAINRANK0_REG - DDRDATA0CH0_CR_RXTRAINRANK0_REG) * Channel) +
    ((DDRDATA0CH0_CR_RXTRAINRANK1_REG - DDRDATA0CH0_CR_RXTRAINRANK0_REG) * Rank) +
    ((DDRDATA1CH0_CR_RXTRAINRANK0_REG - DDRDATA0CH0_CR_RXTRAINRANK0_REG) * Byte);
  MrcWriteCR (MrcData, Offset, CrRxTrainRank.Data);
  //
  // Download new settings from the RegFile to the Pads
  //
  MrcDownloadRegFile (MrcData, Channel, 0, Rank, MrcRegFileRank, Byte, 1, 0);
  return;
}

/**
  This routine updates TXTRAINRANK register's specific fields defined by the subfield
  subfield values:
    0 - Update TxDq    - leave other parameter the same
    1 - Update TxDqs   - leave other parameter the same
    2 - Update TxEq    - leave other parameter the same
    3 - Update ALL from input value (non from Mrcdata structure)

  @param[in] MrcData  - Include all MRC global data.
  @param[in] Channel  - Defines channel to update
  @param[in] Rank     - Defines rank to update
  @param[in] Byte     - Defines byte to update
  @param[in] Subfield - Defines the register's field or fields to update
  @param[in] Value    - value to be writen into register fields

  @retval Nothing
**/
void
UpdateTxT (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const U8             Rank,
  IN const U8             Byte,
  IN const U8             Subfield,
  IN const U32            Value
  )
{
  MrcChannelOut                       *ChannelOut;
  U32                                 Offset;
  DDRDATA0CH0_CR_TXTRAINRANK0_STRUCT  CrTxTrainRank;

  ChannelOut = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  if (Subfield == 3) {
    CrTxTrainRank.Data = Value;
  } else {
    CrTxTrainRank.Data                = 0;
    CrTxTrainRank.Bits.TxDqDelay      = (Subfield == 0) ? Value : ChannelOut->TxDq[Rank][Byte];
    CrTxTrainRank.Bits.TxDqsDelay     = (Subfield == 1) ? Value : ChannelOut->TxDqs[Rank][Byte];
    CrTxTrainRank.Bits.TxEqualization = (Subfield == 2) ? Value : ChannelOut->TxEq[Rank][Byte];
  }

  Offset = DDRDATA0CH0_CR_TXTRAINRANK0_REG +
    ((DDRDATA0CH1_CR_TXTRAINRANK0_REG - DDRDATA0CH0_CR_TXTRAINRANK0_REG) * Channel) +
    ((DDRDATA0CH0_CR_TXTRAINRANK1_REG - DDRDATA0CH0_CR_TXTRAINRANK0_REG) * Rank) +
    ((DDRDATA1CH0_CR_TXTRAINRANK0_REG - DDRDATA0CH0_CR_TXTRAINRANK0_REG) * Byte);
  MrcWriteCR (MrcData, Offset, CrTxTrainRank.Data);
  //
  // Download new settings from the RegFile to the Pads
  //
  MrcDownloadRegFile (MrcData, Channel, 0, Rank, MrcRegFileRank, Byte, 0, 1);
  return;
}

/**
  Returns the index into the array MarginResult in the MrcOutput structure.

  @param[in] ParamV - Margin parameter

  @retval One of the following values: LastRxV(0), LastRxT (1), LastTxV(2), LastTxT (3), LastRcvEna (4),
                                       LastWrLevel (5), LastCmdT (6), LastCmdV (7)
**/
U8
GetMarginResultType (
  IN const U8  ParamV
  )
{
  switch (ParamV) {
    case WrV:
    case WrFan2:
    case WrFan3:
      return LastTxV;

    case WrT:
      return LastTxT;

    case RdV:
    case RdFan2:
    case RdFan3:
      return LastRxV;

    case RdT:
      return LastRxT;

    case RcvEna:
    case RcvEnaX:
      return LastRcvEna;

    case WrLevel:
      return LastWrLevel;

    case CmdT:
      return LastCmdT;

    case CmdV:
      return LastCmdV;

    default:
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "GetMarginByte: Unknown Margin Parameter\n");
      break;
  }

  return 0; // Return LastRxV to point to the beginning of the array
}

/*
1D Margin Types:
RcvEn:   Shifts just RcvEn.  Only side effect is it may eat into read dq-dqs for first bit of burst
RdT:     Shifts read DQS timing, changing where DQ is sampled
WrT:     Shifts write DQ timing, margining DQ-DQS timing
WrDqsT:  Shifts write DQS timing, margining both DQ-DQS and DQS-CLK timing
RdV:     Shifts read Vref voltage for DQ only
WrV:     Shifts write Vref voltage for DQ only
WrLevel: Shifts write DQ and DQS timing, margining only DQS-CLK timing
WrTBit:  Shifts write DQ per bit timing.
RdTBit:  Shifts read DQ per bit timing.
RdVBit:  Shifts read DQ per bit voltage.

2D Margin Types (Voltage, Time)
RdFan2:  Margins both RdV and RdT at { (off, -2/3*off),  (off, 2/3*off) }
WrFan2:  Margins both WrV and WrT at { (off, -2/3*off),  (off, 2/3*off) }
RdFan3:  Margins both RdV and RdT at { (off, -2/3*off),  (5/4*off, 0),  (off, 2/3*off)  }
WrFan3:  Margins both WrV and WrT at { (off, -2/3*off),  (5/4*off, 0),  (off, 2/3*off)  }
*/
/**
  This function Reads MrcData structure and finds the minimum last recorded margin for param
  Searches across all bytes and ranks in RankMask

  @param[in]     MrcData         - Include all MRC global data.
  @param[in,out] MarginResult    - Data structure with the latest margin results.
  @param[in]     Param           - Defines the margin type
  @param[in]     Ranks           - Condenses down the results from multiple ranks

  @retval mrcWrongInputParameter if a bad Param is passed in, otherwise mrcSuccess.
**/
MrcStatus
GetMarginCh (
  IN     MrcParameters *const MrcData,
  IN OUT U32                  MarginResult[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN     const U8             Param,
  IN     const U8             Ranks
  )
{
  MrcOutput *Outputs;
  U32       *Margin1;
  U32       *Margin2;
  U8        ResultType;
  U8        Channel;
  U8        Rank;
  U8        Byte;
  U8        Edge;
  U8        Scale;

  Outputs = &MrcData->SysOut.Outputs;
  switch (Param) {
    case WrV:
    case WrT:
    case RdV:
    case RdT:
      Scale = 10;
      break;

    case WrFan2:
    case WrFan3:
    case RdFan2:
    case RdFan3:
      Scale = 21 / 3;
      break;

    default:
      MRC_DEBUG_MSG (&MrcData->SysIn.Inputs.Debug, MSG_LEVEL_ERROR, "GetMarginCh: Unknown Margin Parameter\n");
      return mrcWrongInputParameter;
  }

  ResultType = GetMarginResultType (Param);
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      Margin2 = &MarginResult[ResultType][0][Channel][0][0];
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel) & Ranks) {
            Margin1 = &MarginResult[ResultType][Rank][Channel][Byte][0];
            for (Edge = 0; Edge < MAX_EDGES; Edge++, Margin1++) {
              if (Margin2[Edge] > *Margin1) {
                Margin2[Edge] = *Margin1;
              }
            }
          }
        }
      }
      //
      // Scale results as needed
      //
      for (Edge = 0; Edge < MAX_EDGES; Edge++, Margin2++) {
        *Margin2 = (*Margin2 * Scale) / 10;
      }
    }
  }

  return mrcSuccess;
}

/**
  Use this function to retrieve the last margin results from MrcData

  @param[in]     MrcData         - Include all MRC global data.
  @param[in,out] MarginResult    - Data structure with the latest margin results.
  @param[in]     Param           - Defines the margin type
  @param[in]     RankIn          - Which rank of the host structure you want the result returned on
  @param[in]     Ranks           - Condenses down the results from multiple ranks

  @retval MarginResult structure has been updated if MrcStatus returns mrcSuccess.
  @retval Otherwise, mrcWrongInputParameter is returned if an incorrect Param is passed in.
**/
MrcStatus
GetMarginByte (
  IN     MrcParameters *const MrcData,
  IN OUT U32                  MarginResult[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN     const U8             Param,
  IN     const U8             RankIn,
  IN     const U8             Ranks
  )
{
  MrcOutput *Outputs;
  U32       *Margin1;
  U32       *Margin2;
  U8        ResultType;
  U8        Channel;
  U8        Rank;
  U8        Byte;
  U8        Edge;
  U8        Scale;

  Outputs = &MrcData->SysOut.Outputs;
  switch (Param) {
    case WrV:
    case WrT:
    case RdV:
    case RdT:
    case RcvEna:
    case RcvEnaX:
      Scale = 10;
      break;

    case WrFan2:
    case WrFan3:
    case RdFan2:
    case RdFan3:
      Scale = 21 / 3;
      break;

    default:
      MRC_DEBUG_MSG (&MrcData->SysIn.Inputs.Debug, MSG_LEVEL_ERROR, "GetMarginByte: Unknown Margin Parameter\n");
      return mrcWrongInputParameter;
  }

  ResultType = GetMarginResultType (Param);
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (MrcChannelExist (Outputs, Channel)) {
      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          if (MrcRankInChannelExist (MrcData, Rank, Channel) & Ranks) {
            Margin1 = &MarginResult[ResultType][RankIn][Channel][Byte][0];
            Margin2 = &MarginResult[ResultType][Rank][Channel][Byte][0];
            for (Edge = 0; Edge < MAX_EDGES; Edge++, Margin1++, Margin2++) {
              if (*Margin1 > *Margin2) {
                *Margin1 = *Margin2;
              }
            }
          }
        }
        //
        // Scale results as needed
        //
        Margin1 = &MarginResult[ResultType][RankIn][Channel][Byte][0];
        for (Edge = 0; Edge < MAX_EDGES; Edge++, Margin1++) {
          *Margin1 = (*Margin1 * Scale) / 10;
        }
      }
    }
  }

  return mrcSuccess;
}

/**
  This function is use to "unscale" the MrcData last margin point
  GetMarginByte will scale the results for FAN margin
  This will unscale the results such that future tests start at the correct point

  @param[in]     MrcData      - Include all MRC global data.
  @param[in,out] MarginResult - Input array to be unscaled.
  @param[in]     Param        - Defines the margin type for proper scale selection.
  @param[in]     Rank         - Which rank of the host structure to work on

  @retval mrcSuccess
**/
MrcStatus
ScaleMarginByte (
  IN     MrcParameters *const MrcData,
  IN OUT U32                  MarginResult[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN     const U8             Param,
  IN     const U8             Rank
  )
{
  MrcOutput *Outputs;
  U32       *Margin;
  U8        ResultType;
  U8        Channel;
  U8        Byte;
  U8        Edge;

  //
  // Calculate scale parameter based on param
  // Leave room for expansion in case other params needed to be scaled
  //
  Outputs = &MrcData->SysOut.Outputs;
  if ((Param == RdFan2) || (Param == RdFan3) || (Param == WrFan2) || (Param == WrFan3)) {
    ResultType = GetMarginResultType (Param);
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          Margin = &MarginResult[ResultType][Rank][Channel][Byte][0];
          for (Edge = 0; Edge < MAX_EDGES; Edge++, Margin++) {
            *Margin = (*Margin * 15) / 10;
          }
        }
      }
    }
  }

  return mrcSuccess;
}

/**
  This function is used by most margin search functions to change te underlying margin parameter.
  This function allows single search function to be used for different types of margins with minimal impact.
  It provides multiple different parameters, including 2D parameters like Read or Write FAN.
  It can work in either MultiCast or single register mode.

  @param[in,out] MrcData       - Include all MRC global data.
  @param[in]     param         - Includes parameter(s) to change including two dimentional.
  @param[in]     value0        - Selected value to program margin param to
  @param[in]     value1        - Selected value to program margin param to in 2D mode (FAN mode)
  @param[in]     EnMultiCast   - To enable Multicast (broadcast) or single register mode
  @param[in]     channel       - Desired Channel
  @param[in]     rankIn        - Desired Rank - only used for the RxTBit and TxTBit settings and to propagate RdVref
  @param[in]     byte          - Desired byte offset register
  @param[in]     bitIn         - Desired bit offset Mrc data strucure if UpdateMrcData is 1
  @param[in]     UpdateMrcData - Used to decide if Mrc host must be updated
  @param[in]     SkipWait      - Used to skip wait until all channel are done
  @param[in]     RegFileParam  - Used to determine which Rank to download.  Passed to MrcDownloadRegFile.

  @retval MrcStatus -  if succeeded, return mrcSuccess
**/
MrcStatus
ChangeMargin (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             param,
  IN     const S32            value0,
  IN     const S32            value1,
  IN     const U8             EnMultiCast,
  IN     const U8             channel,
  IN     const U8             rankIn,
  IN     const U8             byte,
  IN     const U8             bitIn,
  IN     const U8             UpdateMrcData,
  IN     const U8             SkipWait,
  IN     const MrcRegFile     RegFileParam
  )
{
  //
  // Programs margin param to the selected value0
  // If param is a 2D margin parameter (ex: FAN), then it uses both value0 and value1
  //    For an N point 2D parameter, value1 can be an interger from 0 to (N-1)
  //    For per bit timing parameter, value1 is the sign of the shift
  // param = {0:RcvEna, 1:RdT, 2:WrT, 3: WrDqsT, 4:RdV, 5:WrV, 6:WrLevel,
  //               7:WrTBox, 8:WrTBit, 9:RdTBit, 10:RdVBit,
  //              16:RdFan2, 17:WrFan2, 32:RdFan3, 33:WrFan3}
  // Note: For Write Vref, the trained value and margin register are the same
  // Note: rank is only used for the RxTBit and TxTBit settings and to propagate RdVref
  // Note: PerBit Settings (WrTBit, RdTBit, RdVBit) provide all 8 offsets in value0

  const MrcDebug    *Debug;
  const MrcInput    *Inputs;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcChannelOut     *CurrentChannelOut;
  MrcStatus         Status;
  U8                CurrentCh;
  U8                CurrentByte;
  U8                Max0;
  U8                MaxT;
  U8                MaxV;
  U8                maskT;
  U8                rank;
  U8                bit;
  U8                ReadRFRd;
  U8                ReadRFWr;
  S32               sign;
  S32               v0;
  S32               v1;
  U32               Offset;
  BOOL              UpdateDataOffset;
  DDRDATA0CH0_CR_DDRCRDATAOFFSETTRAIN_STRUCT CRValue;
  MCHBAR_CH0_CR_SC_IO_LATENCY_STRUCT         ScIoLatency;

  Status            = mrcSuccess;
  UpdateDataOffset  = FALSE;
  ReadRFRd          = 0;
  ReadRFWr          = 0;
  Inputs            = &MrcData->SysIn.Inputs;
  Debug             = &Inputs->Debug;
  Outputs           = &MrcData->SysOut.Outputs;
  ControllerOut     = &Outputs->Controller[0];

  //
  // Pre-Process the margin numbers
  //
  MaxT   = MAX_POSSIBLE_TIME;   // Maximum value for Time
  MaxV   = MAX_POSSIBLE_VREF;   // Maximum value for Vref
  maskT  = 0x3F; // 6 bits (2's complement)

  if ((param < RdV) || (param == WrLevel)) {
    Max0 = MaxT;
  } else if ((param == WrTBit) || (param == RdTBit) || (param == RdVBit)) {
    Max0 = 0xFF;
  } else {
    Max0 = MaxV; // Vref for RdV, WrV, and FAN modes
  }
  //
  // Pre-Process the margin numbers.  Calculate 2D points based on FAN slopes
  //
  v0    = value0;
  sign  = (2 * value1 - 1);

  //
  // For Fan3, optimize point orders to minimize Vref changes and # of tests required
  //
  if (param >= RdFan3) {
    sign = ((3 * value1 - 5) * value1) / 2;   // Translates to {0:0, 1:-1, 2:+1}
    if (value1 == 0) {
      v0 = (5 * value0) / 4;
    }
  }

  v1 = (sign * value0) / 3;
  if (v0 > Max0) {
    v0 = Max0;
  } else if (v0 < (-1 * Max0)) {
    v0 = (-1 * Max0);
  }

  if (v1 > MaxT) {
    v1 = MaxT;
  } else if (v1 < (0 - MaxT)) {
    v1 = (0 - MaxT);
  }
  //
  // Rank = -1 sometimes if used to indicate all ranks
  // Does not make sense here, hence set to 0)
  //
  rank          = (rankIn == 0xFF) ? 0 : rankIn;

  ChannelOut    = &ControllerOut->Channel[channel];
  CRValue.Data  = ChannelOut->DataOffsetTrain[byte];
  switch (param) {
    case RcvEna:
      CRValue.Bits.RcvEnOffset = (U32) v0;
      UpdateDataOffset         = TRUE;
      break;

    case RdT:
      CRValue.Bits.RxDqsOffset = (U32) v0;
      UpdateDataOffset         = TRUE;
      break;

    case WrT:
      CRValue.Bits.TxDqOffset = (U32) v0;
      UpdateDataOffset        = TRUE;
      break;

    case WrDqsT:
      CRValue.Bits.TxDqsOffset = (U32) v0;
      UpdateDataOffset         = TRUE;
      break;

    case RdV:
      CRValue.Bits.VrefOffset = (U32) v0;
      UpdateDataOffset        = TRUE;
      break;

    case RcvEnaX:
      //
      // Calculate new IOComp Latency to include over/underflow
      //
      Offset = MCHBAR_CH0_CR_SC_IO_LATENCY_REG +
        ((MCHBAR_CH1_CR_SC_IO_LATENCY_REG - MCHBAR_CH0_CR_SC_IO_LATENCY_REG) * channel);
      ScIoLatency.Data = MrcReadCR (MrcData, Offset);
      if (v0 > 0) {
        v0 = v0 * 2 - 16;
        ScIoLatency.Bits.RT_IOCOMP = (MCHBAR_CH0_CR_SC_IO_LATENCY_RT_IOCOMP_MAX & (ChannelOut->RTIoComp - 1));
      } else if (v0 < 0) {
        v0 = v0 * 2 + 16;
        ScIoLatency.Bits.RT_IOCOMP = (MCHBAR_CH0_CR_SC_IO_LATENCY_RT_IOCOMP_MAX & (ChannelOut->RTIoComp + 1));
      } else {
        ScIoLatency.Bits.RT_IOCOMP = (MCHBAR_CH0_CR_SC_IO_LATENCY_RT_IOCOMP_MAX & ChannelOut->RTIoComp);
      }

      v0 +=  ChannelOut->RcvEn[rank][byte];// the assumption is that we are @ 1 Qclk before edge
      //
      // Limit RcvEna 0-511 to prevent under/overflow.
      //
      if (v0 < 0) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "WARNING: RcvEn PI wrapped below zero!\n");
        v0 = 0;
      } else if (v0 > DDRDATA_CR_RXTRAINRANK0_RxRcvEnPi_MAX) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "WARNING: RcvEn PI wrapped above 9 bits!\n");
        v0 = DDRDATA_CR_RXTRAINRANK0_RxRcvEnPi_MAX;
      }
      UpdateRxT (MrcData, channel, rank, byte, 0,(U16) v0);
      MrcWriteCR (MrcData, Offset, ScIoLatency.Data);
      break;

    case WrV:
    case WrFan2:
    case WrFan3:
      for (CurrentCh = 0; CurrentCh < MAX_CHANNEL; CurrentCh++) {
        if (MrcChannelExist (Outputs, CurrentCh)) {
          if ((EnMultiCast == 1) || (CurrentCh == channel)) {
            UpdateVrefWaitTilStable (MrcData, CurrentCh, UpdateMrcData, v0, SkipWait);
          }
        }
      }

      if ((param == WrFan2) || (param == WrFan3)) {
        CRValue.Data            = ChannelOut->DataOffsetTrain[byte];
        CRValue.Bits.TxDqOffset = v1; // Update TxDqOffset
        UpdateDataOffset = TRUE;
      }
      break;

    case RdFan2:  // Read margin in FAN modes.
    case RdFan3:
      CRValue.Data             = ChannelOut->DataOffsetTrain[byte];
      CRValue.Bits.VrefOffset  = v0; // Update VrefOffset
      CRValue.Bits.RxDqsOffset = v1; // Update RxDqsOffset
      UpdateDataOffset = TRUE;
      break;

    case WrLevel: // Write DQ and DQS timing, margining only DQS-CLK timing
      CRValue.Data             = ChannelOut->DataOffsetTrain[byte];
      CRValue.Bits.TxDqOffset  = v0; // Update TxDqOffset
      CRValue.Bits.TxDqsOffset = v0; // Update TxDqsOffset
      UpdateDataOffset = TRUE;
      break;

    case WrTBit:  // Write DQ per BIT timing
      ReadRFWr = 1;
      if (EnMultiCast) {
        Offset = DDRDATA_CR_TXPERBITRANK0_REG +
          ((DDRDATA_CR_TXPERBITRANK1_REG - DDRDATA_CR_TXPERBITRANK0_REG) * rank);
        MrcWriteCrMulticast (MrcData, Offset, value0);
        for (CurrentCh = 0; CurrentCh < MAX_CHANNEL; CurrentCh++) {
          CurrentChannelOut = &ControllerOut->Channel[CurrentCh];
          //
          // Download new settings from the RegFile to the Pads
          //
          MrcDownloadRegFile (MrcData, CurrentCh, 1, rank, RegFileParam, 0, ReadRFRd, ReadRFWr);
          if (UpdateMrcData) {
            for (CurrentByte = 0; CurrentByte < Outputs->SdramCount; CurrentByte++) {
              for (bit = 0; bit < MAX_BITS; bit++) {
                CurrentChannelOut->TxDqPb[rank][CurrentByte][bit].Center = (value0 >> (4 * bit)) & 0xF;
              }
            }
          }
        }
      } else {
        Offset = DDRDATA0CH0_CR_TXPERBITRANK0_REG +
          ((DDRDATA0CH0_CR_TXPERBITRANK1_REG - DDRDATA0CH0_CR_TXPERBITRANK0_REG) * rank) +
          ((DDRDATA1CH0_CR_TXPERBITRANK0_REG - DDRDATA0CH0_CR_TXPERBITRANK0_REG) * byte) +
          ((DDRDATA0CH1_CR_TXPERBITRANK0_REG - DDRDATA0CH0_CR_TXPERBITRANK0_REG) * channel);
        MrcWriteCR (MrcData, Offset, value0);
        //
        // Download new settings from the RegFile to the Pads
        //
        MrcDownloadRegFile (MrcData, channel, 0, rank, RegFileParam, byte, ReadRFRd, ReadRFWr);
        if (UpdateMrcData) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            ChannelOut->TxDqPb[rank][byte][bit].Center = (value0 >> (4 * bit)) & 0xF;
          }
        }
      }
      break;

    case RdTBit:  // Read DQ per BIT timing
      ReadRFRd = 1;
      if (EnMultiCast) {
        Offset = DDRDATA_CR_RXPERBITRANK0_REG +
          ((DDRDATA_CR_RXPERBITRANK1_REG - DDRDATA_CR_RXPERBITRANK0_REG) * rank);
        MrcWriteCrMulticast (MrcData, Offset, value0);
        for (CurrentCh = 0; CurrentCh < MAX_CHANNEL; CurrentCh++) {
          CurrentChannelOut = &ControllerOut->Channel[CurrentCh];
          //
          // Download new settings from the RegFile to the Pads
          //
          MrcDownloadRegFile (MrcData, CurrentCh, 1, rank, RegFileParam, 0, ReadRFRd, ReadRFWr);
          if (UpdateMrcData) {
            for (CurrentByte = 0; CurrentByte < Outputs->SdramCount; CurrentByte++) {
              for (bit = 0; bit < MAX_BITS; bit++) {
                CurrentChannelOut->RxDqPb[rank][CurrentByte][bit].Center = (value0 >> (4 * bit)) & 0xF;
              }
            }
          }
        }
      } else {
        Offset = DDRDATA0CH0_CR_RXPERBITRANK0_REG +
          ((DDRDATA0CH0_CR_RXPERBITRANK1_REG - DDRDATA0CH0_CR_RXPERBITRANK0_REG) * rank) +
          ((DDRDATA1CH0_CR_RXPERBITRANK0_REG - DDRDATA0CH0_CR_RXPERBITRANK0_REG) * byte) +
          ((DDRDATA0CH1_CR_RXPERBITRANK0_REG - DDRDATA0CH0_CR_RXPERBITRANK0_REG) * channel);
        MrcWriteCR (MrcData, Offset, value0);
        //
        // Download new settings from the RegFile to the Pads
        //
        MrcDownloadRegFile (MrcData, channel, 0, rank, RegFileParam, byte, ReadRFRd, ReadRFWr);
        if (UpdateMrcData) {
          for (bit = 0; bit < MAX_BITS; bit++) {
            ChannelOut->RxDqPb[rank][byte][bit].Center = (value0 >> (4 * bit)) & 0xF;
          }
        }
      }
      break;

    case RdVBit:  // Read DQ per BIT Voltage
      ReadRFRd = 1;
      if (EnMultiCast) {
        MrcWriteCrMulticast (MrcData, DDRDATA_CR_RXOFFSETVDQ_REG, value0);
        for (CurrentCh = 0; CurrentCh < MAX_CHANNEL; CurrentCh++) {
          CurrentChannelOut = &ControllerOut->Channel[CurrentCh];
          //
          // Download new settings from the RegFile to the Pads
          //
          MrcDownloadRegFile (MrcData, CurrentCh, 1, rank, RegFileParam, 0, ReadRFRd, ReadRFWr);
          if (UpdateMrcData) {
            for (CurrentByte = 0; CurrentByte < Outputs->SdramCount; CurrentByte++) {
              for (bit = 0; bit < MAX_BITS; bit++) {
                CurrentChannelOut->RxDqVrefPb[rank][CurrentByte][bit].Center = (value0 >> (4 * bit)) & 0xF;
              }
            }
          }
        }
      } else {
        Offset = DDRDATA0CH0_CR_RXOFFSETVDQ_REG +
          ((DDRDATA1CH0_CR_RXOFFSETVDQ_REG - DDRDATA0CH0_CR_RXOFFSETVDQ_REG) * byte) +
          ((DDRDATA0CH1_CR_RXOFFSETVDQ_REG - DDRDATA0CH0_CR_RXOFFSETVDQ_REG) * channel);
        MrcWriteCR (MrcData, Offset, value0);
        //
        // Download new settings from the RegFile to the Pads
        //
        MrcDownloadRegFile (MrcData, channel, 0, rank, RegFileParam, byte, ReadRFRd, ReadRFWr);
        if (UpdateMrcData) {
          ChannelOut = &ControllerOut->Channel[channel];
          for (bit = 0; bit < MAX_BITS; bit++) {
            ChannelOut->RxDqVrefPb[rank][byte][bit].Center = (value0 >> (4 * bit)) & 0xF;
          }
        }
      }

      break;

    default:
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Function ChangeMargin, Invalid parameter %d\n", param);
      return mrcWrongInputParameter;
  } // end switch (param)

  if (UpdateDataOffset) {
    //
    // Download new settings from the RegFile to the Pads
    //
    if ((param == RcvEnaX) ||(param == RcvEna) || (param == RdT) || (param == RdV) || (param == RdFan2) || (param == RdFan3)) {
      ReadRFRd = 1;
    } else if ((param == WrT) || (param == WrDqsT) || (param == WrLevel) || (param == WrFan2) || (param == WrFan3)) {
      ReadRFWr = 1;
    }
    //
    // Write CR
    //
    if (EnMultiCast) {
      MrcWriteCrMulticast (MrcData, DDRDATA_CR_DDRCRDATAOFFSETTRAIN_REG, CRValue.Data);
      for (CurrentCh = 0; CurrentCh < MAX_CHANNEL; CurrentCh++) {
        if (MrcChannelExist (Outputs, CurrentCh)) {
          CurrentChannelOut = &ControllerOut->Channel[CurrentCh];
          //
          // Download new settings from the RegFile to the Pads
          //
          MrcDownloadRegFile (MrcData, CurrentCh, 1, rank, RegFileParam, 0, ReadRFRd, ReadRFWr);
          for (CurrentByte = 0; CurrentByte < Outputs->SdramCount; CurrentByte++) {
            if (UpdateMrcData) {
              CurrentChannelOut->DataOffsetTrain[CurrentByte] = CRValue.Data;
            }
          }
        }
      }
    } else {
      Offset = DDRDATA0CH0_CR_DDRCRDATAOFFSETTRAIN_REG +
        ((DDRDATA0CH1_CR_DDRCRDATAOFFSETTRAIN_REG - DDRDATA0CH0_CR_DDRCRDATAOFFSETTRAIN_REG) * channel) +
        ((DDRDATA1CH0_CR_DDRCRDATAOFFSETTRAIN_REG - DDRDATA0CH0_CR_DDRCRDATAOFFSETTRAIN_REG) * byte);
      MrcWriteCR (MrcData, Offset, CRValue.Data);
      //
      // Download new settings from the RegFile to the Pads
      //
      MrcDownloadRegFile (MrcData, channel, 0, rank, RegFileParam, byte, ReadRFRd, ReadRFWr);
      if (UpdateMrcData) {
        ChannelOut->DataOffsetTrain[byte] = CRValue.Data;
      }
    }
  }

  return Status;
}

/**
  This function triggers the hardware to download the specified RegFile.
  The setting of ReadRfRd and ReadRfWr must be mutually exclusive.
  Only 1 (start download) and 0 (do nothing) are valid values for ReadRfXx.

  @param[in] MrcData       - Global MRC Data
  @param[in] Channel       - The Channel to download target.
  @param[in] ByteMulticast - Enable Multicasting all bytes on that Channel.
  @param[in] Rank          - The Rank download target.
  @param[in] RegFileParam  - Used to determine which Rank to download.
                              MrcRegFileRank - Uses the Rank Parameter.
                              MrcRegFileStart    - Uses the Rank in REUT_CH_SEQ_BASE_ADDR_START after decoding logical to physical.
                              MrcRegFileCurrent  - Uses the Rank in REUT_CH_SEQ_BASE_ADDR_CURRENT after decoding logical to physical.
  @param[in] Byte          - The Byte download target.
  @param[in] ReadRfRd      - Download the read RegFile. 1 enables, 0 otherwise
  @param[in] ReadRfWr      - Download the write RegFile. 1 enables, 0 otherwise

  @retval MrcStatus - If both ReadRfRd and ReadRfWr are set, the functions returns mrcWrongInputParameters.
                         Otherwise, mrcSuccess.
**/
void
MrcDownloadRegFile (
  IN MrcParameters *const MrcData,
  IN const U8             Channel,
  IN const BOOL           ByteMulticast,
  IN  U8                  Rank,
  IN const MrcRegFile     RegFileParam,
  IN const U8             Byte,
  IN const BOOL           ReadRfRd,
  IN const BOOL           ReadRfWr
  )
{
  DDRDATA0CH0_CR_DDRCRDATACONTROL0_STRUCT                                 DdrCrDataControl0;
  MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_STRUCT  ReutChSeqRankL2PMapping;
  U64                                                                     ReutChSeqBaseAddr;
  MrcChannelOut                                                           *ChannelOut;
  U32                                                                     CrOffset;
  U8                                                                      LogicalRank;

  ChannelOut = &MrcData->SysOut.Outputs.Controller[0].Channel[Channel];
  //
  // Determine the rank to download the Reg File
  //
  switch (RegFileParam) {
    case MrcRegFileStart:
      CrOffset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG +
      (
        (MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_START_MCMAIN_0_REG) *
        Channel
      );
      break;

    case MrcRegFileCurrent:
      CrOffset = MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_REG +
        (
          (MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_REG) *
          Channel
        );
      break;

    case MrcRegFileRank:
    default:
      CrOffset = 0;
      break;
  }

  if (CrOffset != 0) {
    ReutChSeqBaseAddr  = MrcReadCR64 (MrcData, CrOffset);
    ReutChSeqBaseAddr &= MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_Rank_Address_MSK;
    LogicalRank        = (U8) MrcOemMemoryRightShiftU64 (
                               ReutChSeqBaseAddr,
                               MCDFXS_CR_REUT_CH_SEQ_BASE_ADDR_CURRENT_MCMAIN_0_Rank_Address_OFF
                               );

    CrOffset = MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG +
      (
        (
          MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_1_REG -
          MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_REG
        ) * Channel
      );
    ReutChSeqRankL2PMapping.Data = MrcReadCR (MrcData, CrOffset);
    Rank = (U8)
      (
        (ReutChSeqRankL2PMapping.Data >> (LogicalRank * 4)) &
        MCDFXS_CR_REUT_CH_SEQ_RANK_LOGICAL_TO_PHYSICAL_MAPPING_MCMAIN_0_Logical_to_Physical_Rank0_Mapping_MSK
      );
  }

  if (ByteMulticast) {
    //
    // Multicast settings on the channel
    //
    CrOffset = DDRDATACH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATACH1_CR_DDRCRDATACONTROL0_REG - DDRDATACH0_CR_DDRCRDATACONTROL0_REG) * Channel);
  } else {
    CrOffset = DDRDATA0CH0_CR_DDRCRDATACONTROL0_REG +
      ((DDRDATA0CH1_CR_DDRCRDATACONTROL0_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL0_REG) * Channel) +
      ((DDRDATA1CH0_CR_DDRCRDATACONTROL0_REG - DDRDATA0CH0_CR_DDRCRDATACONTROL0_REG) * Byte);
  }

  DdrCrDataControl0.Data            = MrcReadCR (MrcData, CrOffset);
  DdrCrDataControl0.Bits.ReadRFRd   = ReadRfRd;
  DdrCrDataControl0.Bits.ReadRFWr   = ReadRfWr;
  DdrCrDataControl0.Bits.ReadRFRank = Rank;
  MrcWriteCR (MrcData, CrOffset, DdrCrDataControl0.Data);
}

/**
  This procedure is meant to handle basic timing centering, places strobe in the middle of the data eye,
  for both read and write DQ/DQS using a very robust, linear search algorthim.

  @param[in,out] MrcData     - Include all MRC global data.
  @param[in]     chBitMaskIn - Channel bit mask.
  @param[in]     param       - {0:RcvEn, 1:RdT, 2:WrT, 3: WrDqsT, 4:RdV, 5:WrV, 6:WrLevel,
                                8:WrTBit, 9:RdTBit, 10:RdVBit,
                                16:RdFan2, 17:WrFan2, 32:RdFan3, 32:WrFan3}
                                ONLY RdT and WrT are allowed in this function
  @param[in]     ResetPerBit - Option to Reset PerBit Deskew to middle value before byte training
  @param[in]     loopcount   - loop count

  @retval MrcStatus -  If succeeded, return mrcSuccess
**/
MrcStatus
DQTimeCentering1D (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             chBitMaskIn,
  IN     const U8             param,
  IN     const U8             ResetPerBit,
  IN     const U8             loopcount
  )
{
  const MrcDebug    *Debug;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  S32               *CurrentPS;
  S32               *CurrentPE;
  S32               *LargestPS;
  S32               *LargestPE;
  U32               *Margin;
  MrcStatus         Status;
  BOOL              Pass;
  U8                Channel;
  U8                Rank;
  U8                Byte;
  U8                Step;
  U8                MinWidth;
  U8                chBitMask;
  U8                DumArr[7];
  U16               Result;
  S32               CurrentPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               CurrentPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               LargestPassingStart[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               LargestPassingEnd[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               cWidth;
  S32               lWidth;
  S32               Center;
  S32               DqsDelay;
  U32               Start;
  U32               End;
  U32               Offset;
  MCHBAR_CH0_CR_SC_IO_LATENCY_STRUCT ScIoLatency;
#ifdef MRC_DEBUG_PRINT
  U64               BitLaneFailures[MAX_CHANNEL][(MAX_POSSIBLE_TIME * 2) + 1];
  U8                BitCount;
  const char       *DelayString;
#endif

  Debug         = &MrcData->SysIn.Inputs.Debug;
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  chBitMask     = chBitMaskIn;
  Status        = mrcSuccess;
  Center        = 0;
  MinWidth      = 8;
  MrcOemMemorySet (DumArr, 1, sizeof (DumArr));

  if ((param != RdT) && (param != WrT) && (param != RcvEnaX)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "DataTimeCentering1D: Unknown Margin Parameter\n");
    return mrcWrongInputParameter;
  }

  Step     = 1;
  if (param == RcvEnaX) {
    SetupIOTestBasicVA (MrcData, chBitMask, loopcount - 3, 0, 0, 0, 8);
    Outputs->DQPat = RdRdTA_All;
  } else {
    SetupIOTestBasicVA (MrcData, chBitMask, loopcount, NSOE, 0, 0, 8);
  }

#ifdef MRC_DEBUG_PRINT
  DelayString = (param == RcvEnaX) ? RcvEnDelayString : DqsDelayString;
#endif
  //
  // Reset PerBit Deskew to middle value before byte training
  // Write timing offset for bit[x] of the DQ byte. Linear from 0-15, each step size is tQCLK/64
  // Read timing offset for bit[x] of the DQ byte. Linear from 0-15, each step size is tQCLK/64
  //
  if (ResetPerBit == 1) {
    //
    // EnMultiCast, UpdateMrcData
    //
    Status = ChangeMargin (
              MrcData,
              (param == RdT) ? RdTBit : WrTBit,
              0x88888888,
              0,
              1,
              0,
              0,
              0,
              0,
              1,
              0,
              MrcRegFileStart
              );
  }
  //
  // Center all Ranks
  //
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {

#ifdef MRC_DEBUG_PRINT
    if (Outputs->ValidRankMask & (MRC_BIT0 << Rank)) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Rank = %d\n", Rank);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel 0                1\nByte\t");
      MRC_DEBUG_MSG (
        Debug,
        MSG_LEVEL_NOTE, (
        Outputs->SdramCount == MAX_SDRAM_IN_DIMM
        ) ? "0 1 2 3 4 5 6 7 8 0 1 2 3 4 5 6 7 8 Error Count" : "0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 Error Count"
        );
    }
#endif  // MRC_DEBUG_PRINT

    chBitMask = 0;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      chBitMask |= SelectReutRanks (MrcData, Channel, (MRC_BIT0 << Rank), 0);
      ChannelOut = &ControllerOut->Channel[Channel];
      if ((MRC_BIT0 << Channel) & chBitMask) {
        //
        // Clear out anything left over in DataOffsetTrain
        // Update rank timing to middle value
        //
        for (Byte = 0; (Byte < Outputs->SdramCount) && (param != RcvEnaX); Byte++) {
          if (param == RdT) {
            //
            // Read Dq/Dqs
            //
            ChannelOut->RxDqsP[Rank][Byte] = 32;
            ChannelOut->RxDqsN[Rank][Byte] = 32;
            UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
          } else if (param == WrT) {
            //
            // Write Dq/Dqs
            //
            ChannelOut->TxDq[Rank][Byte] = ChannelOut->TxDqs[Rank][Byte] + 32;
            UpdateTxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
          }
        }
        //
        // Clear any old state in DataTrain Offset
        //
        MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);

        //
        // Setup REUT Error Counters to count errors per channel
        //
        Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG +
          ((MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_CTL_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_CTL_0_REG) * Channel);
        MrcWriteCR (MrcData, Offset, 0);
      }
    }
    //
    // Continue if not valid rank on any channel
    //
    if (chBitMask == 0) {
      continue; // This rank does not exist on any of the channels
    }
    //
    // Sweep through values
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n%s", DelayString);
    for (DqsDelay = -MAX_POSSIBLE_TIME; DqsDelay <= MAX_POSSIBLE_TIME; DqsDelay += Step) {
      //
      // Program DQS Delays
      //
      if (param == RcvEnaX){
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if ((MRC_BIT0 << Channel) & chBitMask) {
            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              Status = ChangeMargin (MrcData, param, DqsDelay, 0, 0, Channel, Rank, Byte, 0, 0, 0, MrcRegFileStart);
            }
          }
        }
      } else {
        Status = ChangeMargin (MrcData, param, DqsDelay, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileStart);
      }

      //
      // Clear Errors and Run Test
      //
      RunIOTest (MrcData, chBitMask, Outputs->DQPat, DumArr, 1, 0);

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n% 5d  \t", DqsDelay);

      //
      // Update results for all Channel/bytes
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (!(chBitMask & (MRC_BIT0 << Channel))) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            (Channel != 0) ? "" : ((Outputs->SdramCount == MAX_SDRAM_IN_DIMM) ? "                  " : "                ")
            );
          continue;
        }

        //
        // Read out per byte error results and update limit
        //
        Offset = 4 + MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG +
          (
            (
              MCHBAR_CH1_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG -
              MCHBAR_CH0_CR_REUT_CH_ERR_ECC_CHUNK_RANK_BYTE_NTH_STATUS_REG
            ) * Channel
          );
        Result = (U16) MrcReadCR (MrcData, Offset);

#ifdef MRC_DEBUG_PRINT
        Offset = MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG +
          ((MCHBAR_CH1_CR_REUT_CH_ERR_DATA_STATUS_REG - MCHBAR_CH0_CR_REUT_CH_ERR_DATA_STATUS_REG) * Channel);

        BitLaneFailures[Channel][DqsDelay + MAX_POSSIBLE_TIME] = MrcReadCR64 (MrcData, Offset);
#endif
        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          //
          // Check for Byte group error status
          //
          Pass = ((Result & (MRC_BIT0 << Byte)) == 0);

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, Pass ? ". " : "# ");

          CurrentPS = &CurrentPassingStart[Channel][Byte];
          CurrentPE = &CurrentPassingEnd[Channel][Byte];
          LargestPS = &LargestPassingStart[Channel][Byte];
          LargestPE = &LargestPassingEnd[Channel][Byte];
          if (DqsDelay == -31) {
            if (Pass) {
              //
              // No error on this Byte group
              //
              *CurrentPS = *CurrentPE = *LargestPS = *LargestPE = DqsDelay;
            } else {
              //
              // Selected Byte group has accumulated an error during loop back pattern
              //
              *CurrentPS = *CurrentPE = *LargestPS = *LargestPE = -33;
            }
          } else {
            if (Pass) {
              //
              // No error on this Byte group
              //
              if (*CurrentPE != (DqsDelay - Step)) {
                *CurrentPS = DqsDelay;
              }
              *CurrentPE = DqsDelay;

              //
              // Update Largest variables
              //
              cWidth = *CurrentPE - *CurrentPS;
              lWidth = *LargestPE - *LargestPS;
              if (cWidth > lWidth) {
                *LargestPS = *CurrentPS;
                *LargestPE = *CurrentPE;
              }
            }
          }
        } // for Byte
      } // for Channel
#ifdef MRC_DEBUG_PRINT
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (chBitMask & (MRC_BIT0 << Channel)) {
          Offset = MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG +
            ((MCHBAR_CH1_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG - MCHBAR_CH0_CR_REUT_CH_ERR_COUNTER_STATUS_0_REG) * Channel);
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " 0x%x\t", MrcReadCR (MrcData, Offset));
        }
      }
#endif  // MRC_DEBUG_PRINT
    } // for DqsDelay
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "\n");  // End last line of Byte table.

#ifdef MRC_DEBUG_PRINT
    //
    // Print out the bit lane failure information
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Bit Lane Information\n");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (MrcChannelExist (Outputs, Channel)) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %d\nBitLane ", Channel);
        for (BitCount = 0; BitCount < 7; BitCount++) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%u         ", BitCount);
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n        ");  // End tens number and align ones number
        for (BitCount = 0; BitCount < 64; BitCount++) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%u", BitCount % 10);
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n%s", DelayString);

        for (DqsDelay = -MAX_POSSIBLE_TIME; DqsDelay <= MAX_POSSIBLE_TIME; DqsDelay += Step) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n% 5d\t", DqsDelay); // Begin with a new line and print the DqsDelay value
          for (BitCount = 0; BitCount < 64; BitCount++) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_NOTE,
              (BitLaneFailures[Channel][DqsDelay + MAX_POSSIBLE_TIME] & MrcOemMemoryLeftShiftU64 (1, BitCount)) ? "#" : "."
              );
          }
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n"); // Gap after Channel
      }
    }
#endif

    //
    // Clean Up for next Rank
    //
    Status = ChangeMargin (MrcData, param, 0, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileCurrent);

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (chBitMask & (MRC_BIT0 << Channel)) {
        ChannelOut = &ControllerOut->Channel[Channel];

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "C%d.R%d:  Left\tRight\tWidth\tCenter\n", Channel, Rank);

        for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
          LargestPS = &LargestPassingStart[Channel][Byte];
          LargestPE = &LargestPassingEnd[Channel][Byte];
          lWidth    = *LargestPE - *LargestPS;
          if (lWidth < MinWidth) {
            MRC_DEBUG_MSG (
              Debug,
              MSG_LEVEL_ERROR,
              "ERROR!! DataTimeCentering1D Eye Too Small Channel: %u, Rank: %u, Byte: %u\n",
              Channel,
              Rank,
              Byte
              );
            Status = mrcDataTimeCentering1DErr;
          } else {
            Center = *LargestPS + (lWidth / 2);
          }

          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_NOTE,
            "  B%d:   %d\t%d\t%d\t%d\n",
            Byte,
            *LargestPS,
            *LargestPE,
            lWidth,
            Center
            );

          Start = ABS (10 **LargestPS);
          End   = ABS (10 **LargestPE);
          if (param == RdT) {
            //
            // read Dq./Dqs
            //
            Margin                          = &Outputs->MarginResult[LastRxT][Rank][Channel][Byte][0];
            *Margin                         = Start;
            Margin[1]                       = End;
            ChannelOut->RxDqsP[Rank][Byte]  = (U8) ((S32) ChannelOut->RxDqsP[Rank][Byte] + Center);
            ChannelOut->RxDqsN[Rank][Byte]  = (U8) ((S32) ChannelOut->RxDqsN[Rank][Byte] + Center);
            UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
          } else if (param == WrT){
            //
            // Write Dq/Dqs
            //
            Margin                        = &Outputs->MarginResult[LastTxT][Rank][Channel][Byte][0];
            *Margin                       = Start;
            Margin[1]                     = End;
            ChannelOut->TxDq[Rank][Byte]  = (U16) ((S32) ChannelOut->TxDq[Rank][Byte] + Center);
            UpdateTxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
          } else if (param == RcvEnaX){
            //
            // Receive Enable
            //
            Margin                          = &Outputs->MarginResult[LastRcvEna][Rank][Channel][Byte][0];
            *Margin                         = Start;
            Margin[1]                       = End;
            ChannelOut->RcvEn[Rank][Byte]   = (ChannelOut->RcvEn[Rank][Byte] + (U16) (2 * Center));
            UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
          }
        }
        if (param == RcvEnaX){
          //
          // clean up
          //
          Offset = MCHBAR_CH0_CR_SC_IO_LATENCY_REG +
            ((MCHBAR_CH1_CR_SC_IO_LATENCY_REG - MCHBAR_CH0_CR_SC_IO_LATENCY_REG) * Channel);
          ScIoLatency.Data = MrcReadCR (MrcData, Offset);
          ScIoLatency.Bits.RT_IOCOMP = MCHBAR_CH0_CR_SC_IO_LATENCY_RT_IOCOMP_MAX & ChannelOut->RTIoComp;
          MrcWriteCR (MrcData, Offset, ScIoLatency.Data);
        }
      }
    }
  }

  if (param == RcvEnaX) {
    IoReset (MrcData);
  }

  return Status;
}

/**
  This procedure is meant to handle much more complex centering that will use a 2D algorithm to optimize asymetical
  eyes for both timing and voltage margin.

  @param[in,out] MrcData         - Include all MRC global data.
  @param[in,out] MarginResult    - Margin data from centering
  @param[in]     ChBitMaskIn     - Channel bit mask.
  @param[in]     param           - {0:RcvEn, 1:RdT, 2:WrT, 3: WrDqsT, 4:RdV, 5:WrV, 6:WrLevel,
                                    8:WrTBit, 9:RdTBit, 10:RdVBit,
                                    16:RdFan2, 17:WrFan2, 32:RdFan3, 32:WrFan3}
                                    ONLY RdT and WrT are allowed in this function
  @param[in]     EnPerBit        - Option to enable per bit margining
  @param[in]     EnRxDutyCycleIn - Phase to center.
  @param[in]     ResetPerBit     - Option to Reset PerBit Deskew to middle value before byte training
  @param[in]     LoopCount       - loop count
  @param[in]     En2D            - Option to only run center at nominal Vref point

  @retval MrcStatus -  If succeeded, return mrcSuccess
**/
MrcStatus
DataTimeCentering2D (
  IN OUT MrcParameters *const MrcData,
  IN OUT U32                  MarginResult[MAX_RESULT_TYPE][MAX_RANK_IN_CHANNEL][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES],
  IN     const U8             ChBitMaskIn,
  IN     const U8             Param,
  IN     const U8             EnPerBit,
  IN     const U8             EnRxDutyCycleIn,
  IN     const U8             ResetPerBit,
  IN     const U8             LoopCount,
  IN     const U8             En2D
  )
{
  const U32         EHWeights[]       = {6, 2, 1, 0, 2, 1, 0};
  const U32         EWWeights[]       = {0, 1, 2, 3, 1, 2, 3};
  const S32         VrefPointsConst[] = {0, -6, -12, -18, 6, 12, 18};
  const MrcDebug    *Debug;
  const MrcInput    *Inputs;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  U8                *RxDqPbCenter;
  U8                *TxDqPbCenter;
  U16               centerTiming;
  U32               *Margin;
  U32               *Eye;
  S32               *CenterBit;
  S32               *CSum;
  MrcStatus         Status;
  U8                ResultType;
  U8                Channel;
  U8                Rank;
  U8                Byte;
  U8                Bit;
  U8                ParamV;
  U8                ParamB;
  U8                MaxVScale;
  U8                EnPerBitEH;
  U8                Strobe;
  U8                Strobes;
  U8                Vref;
  U8                SaveLC;
  U8                LCloop;
  U8                i;
  U8                SkipWait;
  U8                ChBitMask;
  U8                EnRxDutyCycle;
  U8                Edge;
  U8                BMap[9];
  U8                LoopEnd;
  U16               Mode;
  U32               MarginBit[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS][MAX_EDGES];
  S32               Center;
  U32               Weight;
  S32               VrefPoints[sizeof (VrefPointsConst) / sizeof (VrefPointsConst[0])];
  U32               SumEH;
  U32               SumEW;
  U32               BERStats[4];
  U32               VrefScale[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U32               EH[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U32               EW[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  U32               EyeShape[7][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_EDGES]; // Store all eye edges for Per Bit
  U32               StrobeMargin[7][MAX_CHANNEL][MAX_SDRAM_IN_DIMM][2][MAX_EDGES];//Save Edges per Strobe to pass Min (Stobe1, Strobe2)
  S32               CenterSum[MAX_CHANNEL][MAX_SDRAM_IN_DIMM];
  S32               DivBy;
  S8                DivBySign;
  S32               Value0;
  U32               Offset;
  S32               CenterSumBit[MAX_CHANNEL][MAX_SDRAM_IN_DIMM][MAX_BITS];
  S32               Calc;
  MCHBAR_CH0_CR_REUT_CH_ERR_CTL_STRUCT  ReutChErrCtl;
  DDRDATA0CH0_CR_RXPERBITRANK0_STRUCT   CrPerBitRank;

  Inputs = &MrcData->SysIn.Inputs;
  Debug  = &Inputs->Debug;

  //
  // 2D Margin Types (Voltage, Time)
  // RdFan2:  Margins both RdV and RdT at { (off, -2/3*off),  (off, 2/3*off) }
  // WrFan2:  Margins both WrV and WrT at { (off, -2/3*off),  (off, 2/3*off) }
  // RdFan3:  Margins both RdV and RdT at { (off, -2/3*off),  (5/4*off, 0),  (off, 2/3*off)  }
  // WrFan3:  Margins both WrV and WrT at { (off, -2/3*off),  (5/4*off, 0),  (off, 2/3*off)  }
  //
  if ((Param != RdT) && (Param != WrT)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "DataTimeCentering2D: Incorrect Margin Parameter %d\n", Param);
    return mrcWrongInputParameter;
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Parameter = %d (%sT)\n", Param, (Param == RdT) ? "Rd" : "Wr");
  Outputs       = &MrcData->SysOut.Outputs;
  ControllerOut = &Outputs->Controller[0];
  ChBitMask     = ChBitMaskIn;
  EnRxDutyCycle = EnRxDutyCycleIn;
  Status        = mrcSuccess;
  MaxVScale     = 24;
  Strobes       = 2;
  Center        = 0;
  Value0        = 0;
  MrcOemMemorySet ((U8 *) BERStats, 0, sizeof (BERStats));
  for (i = 0; i < (sizeof (BMap) / sizeof (BMap[0])); i++) {
    BMap[i] = i;
  }

  ResultType = GetMarginResultType (Param);

  EnPerBitEH = 1; // Repeat EH Measurement after byte training, before bit training
  //
  // SOE = 10b ( Stop on All Byte Groups Error )
  //
  SetupIOTestBasicVA (MrcData, ChBitMask, LoopCount - 1, NSOE, 0, 0, 8);
  Outputs->DQPat = RdRdTA;
  //
  // Duty cycle should be ONLY for Rx
  //
  if (Param != RdT) {
    EnRxDutyCycle = 0;
  }

  Strobes = 1 + EnRxDutyCycle;

  //
  // Option to only run center at nominal Vref point
  //
  if (En2D == 0) {
    MrcOemMemorySet ((U8 *) &VrefPoints[0], 0, sizeof (VrefPoints));
  } else {
    MrcOemMemoryCpy ((U8 *) &VrefPoints[0], (U8 *) &VrefPointsConst[0], sizeof (VrefPoints));
  }
  //
  // Calculate SumEH / SumEW for use in weighting equations
  //
  SumEH = SumEW = 0;
  for (Vref = 0; Vref < sizeof (VrefPoints) / sizeof (VrefPoints[0]); Vref++) {
    SumEH += EHWeights[Vref];
    SumEW += EWWeights[Vref];

    //
    // Loop once at nominal Vref point
    //
    if (En2D == 0) {
      Vref = 6;
    }
  }

  if (Param == RdT) {
    ParamV = RdV;
    ParamB = RdTBit;
  } else {
    ParamV = WrV;
    ParamB = WrTBit;
  }
  //
  // Optimize timing per rank
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Optimization is per rank\n");
  for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
    ChBitMask = 0;
    //
    // Select rank for REUT test
    //
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if ((MRC_BIT0 << Channel) & ChBitMaskIn) {
        ChBitMask |= SelectReutRanks (MrcData, Channel, (MRC_BIT0 << Rank), 0);
        if ((MRC_BIT0 << Channel) & ChBitMask) {
          ChannelOut = &ControllerOut->Channel[Channel];
          //
          // Clear any old state in DataTrain Offset
          //
          MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
        }
      }
    }
    //
    // Continue if not valid rank on any channel
    //
    if (ChBitMask == 0) {
      continue;
      //
      // This rank does not exist on any of the channels
      //
    }
    //
    // Reset PerBit Deskew to middle value before byte training
    // Write timing offset for bit[x] of the DQ byte. Linear from 0-15, each step size is tQCLK/64
    // Read timing offset for bit[x] of the DQ byte. Linear from 0-15, each step size is tQCLK/64
    //
    if (ResetPerBit == 1) {
      Status = ChangeMargin (MrcData, ParamB, 0x88888888, 0, 1, 0, Rank, 0, 0, 1, 0, MrcRegFileRank);
    }

    //####################################################
    //######   Get EH to scale vref sample point by  #####
    //####################################################
    //
    // Pass the host last edges by reference
    // Get EH/VrefScale for the use in timing centering
    //
    if (En2D > 0) {
      // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nCalling DQTimeCenterEH\n");
      Status = DQTimeCenterEH (
                MrcData,
                ChBitMask,
                Rank,
                ParamV,
                MaxVScale,
                BMap,
                EH,
                VrefScale,
                BERStats
                );
      if (Status != mrcSuccess) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "\nDQTimeCenterEH FAILED - Using VrefScale = %d\n", MaxVScale);
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          MrcOemMemorySetDword (&VrefScale[Channel][0], MaxVScale, Outputs->SdramCount);
        }
      }
    } else {
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        MrcOemMemorySetDword (&EH[Channel][0], 1, Outputs->SdramCount);
        MrcOemMemorySetDword (&VrefScale[Channel][0], 1, Outputs->SdramCount);
      }
    }

    Status = GetMarginByte (MrcData, MarginResult, Param, Rank, (MRC_BIT0 << Rank));

#if 0
#ifdef MRC_DEBUG_PRINT
    //
    // Read the margins
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nLstSavd Margins ");
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      if (!((1 << Channel) & ChBitMask)) {
        continue;
      }

      for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE,
          "%d %d ",
          MarginResult[ResultType][Rank][Channel][Byte][0],
          MarginResult[ResultType][Rank][Channel][Byte][1]
          );
      }
    }
#endif // MRC_DEBUG_PRINT
#endif

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "### Rank = %d ###\n", Rank);
    for (Strobe = 0; Strobe < Strobes; Strobe++) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\n### Strobe = %d ###\n", Strobe);
      if (Outputs->ValidRankMask & (MRC_BIT0 << Rank)) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nChannel\t\t0\t\t\t\t\t\t\t\t\t 1\nByte\t\t");
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_NOTE, (
          Outputs->SdramCount == MAX_SDRAM_IN_DIMM
          ) ? "0\t1\t2\t3\t4\t\t5\t6\t7\t8\t0\t1\t2\t3\t4\t5\t6\t7\t8\nEdges L/R" :
          "0\t1\t2\t3\t4\t5\t6\t7\t0\t1\t2\t3\t4\t5\t6\t7\nEdges L/R"
          );
      }
      //####################################################
      //######   Measure Eye Width at all Vref Points  #####
      //####################################################
      //
      // Program Selective error checking for RX. if strobe = 0 then Check even else Check odd
      //
      if (EnRxDutyCycle) {
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (ChBitMask & (MRC_BIT0 << Channel)) {
            Offset = 2 + MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG +
              ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * Channel);
            MrcWriteCR8 (MrcData, Offset, (0x55 << Strobe));
          }
        }
      }
      //
      // Loop through all the Vref Points to Test
      //
      for (Vref = 0; Vref < sizeof (VrefPoints) / sizeof (VrefPoints[0]); Vref++) {
        //
        // Setup Vref Voltage for this point
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (ChBitMask & (MRC_BIT0 << Channel)) {
            SkipWait = (ChBitMask >> (Channel + 1)); // Skip if there are more channels

            LoopEnd = (U8) ((ParamV == RdV) ? Outputs->SdramCount : 1);
            for (Byte = 0; Byte < LoopEnd; Byte++) {
              Value0 = (S32) (VrefPoints[Vref] * VrefScale[Channel][Byte]) / MaxVScale;
              Status = ChangeMargin (
                        MrcData,
                        ParamV,
                        Value0,
                        0,
                        0,
                        Channel,
                        Rank,
                        Byte,
                        0,
                        0,
                        SkipWait,
                        MrcRegFileRank
                        );
            }
          }
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nVref = %d:\t", Value0);

        //
        // Run Margin Test
        //
        Mode = 0;
        Status = MrcGetBERMarginByte (
                  MrcData,
                  MarginResult,
                  ChBitMask,
                  Rank,
                  Rank,
                  Param,
                  Mode,
                  BMap,
                  1,
                  31,
                  0,
                  BERStats
                  );
        //
        // Store Results; Setup Vref Voltage for this point
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if ((ChBitMask & (MRC_BIT0 << Channel))) {

            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              Margin = &MarginResult[ResultType][Rank][Channel][Byte][0];
              MRC_DEBUG_MSG (
                Debug,
                MSG_LEVEL_NOTE,
                "%d %d ",
                MarginResult[ResultType][Rank][Channel][Byte][0],
                MarginResult[ResultType][Rank][Channel][Byte][1]
                );

              Center = (S32) (Margin[1] -*Margin);
              if (Vref == 0) {
                EW[Channel][Byte]         = (Margin[1] +*Margin) / 10;
                CenterSum[Channel][Byte]  = 0;
              }
              //
              // Calculate weight for this point
              //
              Weight = EHWeights[Vref] * EH[Channel][Byte] + EWWeights[Vref] * EW[Channel][Byte];
              CenterSum[Channel][Byte] += Weight * Center;
              //
              // Store Edges for Per Bit deskew
              //
              Eye = &EyeShape[Vref][Channel][Byte][0];
              for (Edge = 0; Edge < MAX_EDGES; Edge++) {
                Eye[Edge] = Margin[Edge];
              }
              //
              // RunTime Improvement.  Set margin back to Vref = 0 point when the sign  of the VrefPoint changes
              //
              if ((VrefPoints[Vref] < 0) &&
                  (Vref < (sizeof (VrefPoints) / sizeof (VrefPoints[0]) - 1)) &&
                  (VrefPoints[Vref + 1] > 0)
                  ) {
                Eye = &EyeShape[0][Channel][Byte][0];
                for (Edge = 0; Edge < MAX_EDGES; Edge++) {
                  Margin[Edge] = Eye[Edge];
                }
              }
            }
          }
        }
        //
        // Loop once at nominal Vref point
        //
        if (En2D == 0) {
          Vref = sizeof (VrefPoints) / sizeof (VrefPoints[0]);
        }
      }

      //####################################################
      //############    Center Results per Byte  ###########
      //####################################################
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\nWeighted Center\t");

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {
          ChannelOut = &ControllerOut->Channel[Channel];

          //
          // Calculate and apply CenterPoint.  Round to Nearest Int
          //
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            DivBy = (SumEH * EH[Channel][Byte] + SumEW * EW[Channel][Byte]);
            if (DivBy == 0) {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "DataTimeCentering2D: Divide by zero\n");
              return mrcFail;
            }

            CSum      = &CenterSum[Channel][Byte];
            DivBySign = (*CSum < 0) ? (-1) : 1;

            *CSum     = (*CSum + 10 * (DivBySign * DivBy)) / (20 * DivBy);
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", *CSum);

            //
            // Apply new centerpoint
            //
            if (Param == RdT) {
              if (Strobe == 0) {
                ChannelOut->RxDqsP[Rank][Byte] = (U8) ((S32) ChannelOut->RxDqsP[Rank][Byte] +*CSum);
              }

              if ((!EnRxDutyCycle) || (Strobe == 1)) {
                ChannelOut->RxDqsN[Rank][Byte] = (U8) ((S32) ChannelOut->RxDqsN[Rank][Byte] +*CSum);
                UpdateRxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
              }
            } else {
              ChannelOut->TxDq[Rank][Byte] = (U16) ((S32) ChannelOut->TxDq[Rank][Byte] +*CSum);
              UpdateTxT (MrcData, Channel, Rank, Byte, 0xFF, 0);
            }
            //
            // Update the Eye Edges
            //
            for (Vref = 0; Vref < sizeof (VrefPoints) / sizeof (VrefPoints[0]); Vref++) {
              Calc  = 10 **CSum;
              Eye   = &EyeShape[Vref][Channel][Byte][0];
              *Eye += Calc;
              Eye[1] -= Calc;

              //
              // Save Per Strobe Edges
              //
              for (Edge = 0; Edge < MAX_EDGES; Edge++) {
                StrobeMargin[Vref][Channel][Byte][Strobe][Edge] = EyeShape[Vref][Channel][Byte][Edge];
              }
              //
              // Loop once at nominal Vref point
              //
              if (En2D == 0) {
                Vref = sizeof (VrefPoints) / sizeof (VrefPoints[0]);
              }
            }
            //
            // Update MrcData for future tests (MarginResult points back to MrcData)
            // EyeShape for Vref 0 is assumed to have the best shape for future tests.
            //
            for (Edge = 0; Edge < MAX_EDGES; Edge++) {
              MarginResult[ResultType][Rank][Channel][Byte][Edge] = EyeShape[0][Channel][Byte][Edge];
            }
          }
          //
          // Clean up after test
          //
          MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
        }
      }

      centerTiming = 0;
#ifdef MRC_DEBUG_PRINT
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nFinal Center\t");
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {
          ChannelOut = &ControllerOut->Channel[Channel];

          //
          // Calculate final center point relative to "zero" as in the 1D case
          //
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            if (Param == RdT) {
              if (Strobe == 0) {
                centerTiming = (U8) (ChannelOut->RxDqsP[Rank][Byte] - 32);
              }

              if ((!EnRxDutyCycle) || (Strobe == 1)) {
                centerTiming = (U8) (ChannelOut->RxDqsN[Rank][Byte] - 32);
              }
            } else {
              centerTiming = ChannelOut->TxDq[Rank][Byte] - (ChannelOut->TxDqs[Rank][Byte] + 32);
            }

            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t", (S8) centerTiming);
          }
        }
      }
#endif // MRC_DEBUG_PRINT
    } // End of Byte Centering

    //######################################################
    //############     Measure Eye Width Per BIT  ##########
    //######################################################

    if (EnPerBit) {
#if 0
#ifdef MRC_DEBUG_PRINT
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\nEdges we pass on to GetMarginBit are\n");
      for (Vref = 0; Vref < sizeof (VrefPoints) / sizeof (VrefPoints[0]); Vref++) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t");
        //
        // Setup Vref Voltage for this point
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (ChBitMask & (MRC_BIT0 << Channel)) {
            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              for (Edge = 0; Edge < MAX_EDGES; Edge++) {
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d ", EyeShape[Vref][Channel][Byte][Edge]);
              }
            }
          }
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
        if (En2D == 0) {
          Vref = sizeof (VrefPoints) / sizeof (VrefPoints[0]);
        }
      }
#endif // MRC_DEBUG_PRINT
#endif

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\n### Measure Eye Width Per BIT\n");
      //
      // Recalculate the EH after the Byte Centering
      //
      if (EnPerBitEH && (En2D > 0)) {
        Status = DQTimeCenterEH (
                  MrcData,
                  ChBitMask,
                  Rank,
                  ParamV,
                  MaxVScale,
                  BMap,
                  EH,
                  VrefScale,
                  BERStats
                  );
        if (Status != mrcSuccess) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "\nDQTimeCenterEH FAILED - Using VrefScale = %d\n", MaxVScale);
          for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
            MrcOemMemorySetDword (&VrefScale[Channel][0], MaxVScale, Outputs->SdramCount);
          }
        }
      }
      //
      // No stop on error or selective error cheking
      // Stop on all lane fail
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {
#ifdef MRC_DEBUG_PRINT
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel      %d", Channel);
          if (Channel == 0) {
            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\t\t\t\t\t\t");
            }
          }
#endif // MRC_DEBUG_PRINT
          //
          // SOE = 11b ( Stop on All Lanes Error )
          //
          Offset = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG +
            ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * Channel);
          ReutChErrCtl.Data                                   = 0;
          ReutChErrCtl.Bits.Selective_Error_Enable_Cacheline  = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Cacheline_MAX;
          ReutChErrCtl.Bits.Selective_Error_Enable_Chunk      = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Chunk_MAX;
          ReutChErrCtl.Bits.Stop_On_Error_Control             = ALSOE;
          ReutChErrCtl.Bits.Stop_on_Nth_Error                 = 1;
          MrcWriteCR (MrcData, Offset, ReutChErrCtl.Data);
        }
      }

#ifdef MRC_DEBUG_PRINT
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "             Byte % 24d ", Byte);
          }
        }
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
#endif // MRC_DEBUG_PRINT
      //
      // Loop through all the Vref Points to Test
      //
      SaveLC = Outputs->DQPatLC;
      for (Vref = 0; Vref < sizeof (VrefPoints) / sizeof (VrefPoints[0]); Vref++) {
        //
        // Setup Vref Voltage for this point
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if ((ChBitMask & (MRC_BIT0 << Channel))) {

            SkipWait = (ChBitMask >> (Channel + 1)); // Skip if there are more channels
            //
            // Change Vref margin
            //
            LoopEnd = (U8) ((ParamV == RdV) ? Outputs->SdramCount : 1);
            for (Byte = 0; Byte < LoopEnd; Byte++) {
              Value0 = (S32) (VrefPoints[Vref] * VrefScale[Channel][Byte]) / MaxVScale;
              Status = ChangeMargin (
                        MrcData,
                        ParamV,
                        Value0,
                        0,
                        0,
                        Channel,
                        Rank,
                        Byte,
                        0,
                        0,
                        SkipWait,
                        MrcRegFileRank
                        );
            }
          }

          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %d Vref = %d\t", Channel, Value0);
          MrcOemMemorySetDword (&MarginBit[Channel][0][0][0], 8, MAX_SDRAM_IN_DIMM * MAX_BITS * MAX_EDGES);
        }
        //
        // Run Margin Test; Loop through 2 times. Once at low loop count and Once at high loopcount. Improves runtime
        // @todo: Need loop count of 2 when not using BASICVA
        //
        for (LCloop = 0; LCloop < 1; LCloop++) {
          Outputs->DQPatLC  = (LCloop == 0) ? 1 : SaveLC;

          Mode = 0;
          Status = MrcGetMarginBit (MrcData, ChBitMask, Rank, MarginBit, EyeShape[Vref], ParamB, Mode, 15);
        }
        //
        // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nCSum      ");
        // Store Results
        // Setup Vref Voltage for this point
        //
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (ChBitMask & (MRC_BIT0 << Channel)) {

            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              //
              // Calculate weight for this point
              //
              Weight = EHWeights[Vref] * EH[Channel][Byte] + EWWeights[Vref] * EW[Channel][Byte];
              for (Bit = 0; Bit < MAX_BITS; Bit++) {
                Margin  = &MarginBit[Channel][Byte][Bit][0];
                CSum    = &CenterSumBit[Channel][Byte][Bit];

                Center = ((Margin[1] - 8) - (8 - *Margin));
                if (Vref == 0) {
                  *CSum = 0;
                }

                *CSum += Weight * Center;
                //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "% 4d", *CSum);
              }
              //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " Weight %d  ", Weight);
            }
          }
        }
        //
        // Loop once at nominal Vref point
        //
        if (En2D == 0) {
          Vref = sizeof (VrefPoints) / sizeof (VrefPoints[0]);
        }
      }

      //######################################################
      //#############     Center Result Per BIT  #############
      //######################################################
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nWtd Ctr\t  ");
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {
          ChannelOut = &ControllerOut->Channel[Channel];

          //
          // Cleanup after test - go back to the per byte setup
          //
          ReutChErrCtl.Data                                  = 0;
          ReutChErrCtl.Bits.Stop_on_Nth_Error                = 1;
          ReutChErrCtl.Bits.Stop_On_Error_Control            = NSOE;
          ReutChErrCtl.Bits.Selective_Error_Enable_Chunk     = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Chunk_MAX;
          ReutChErrCtl.Bits.Selective_Error_Enable_Cacheline = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Cacheline_MAX;
          Offset = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG +
            ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * Channel);
          MrcWriteCR (MrcData, Offset, ReutChErrCtl.Data);

          //
          // Calculate and apply CenterPoint.  Round to Nearest Int
          //
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            DivBy = (SumEH * EH[Channel][Byte] + SumEW * EW[Channel][Byte]);

            //
            // Make sure DivBy is never 0
            //
            if (DivBy == 0) {
              DivBy = 1;
            }

            CrPerBitRank.Data = 0;
            for (Bit = 0; Bit < MAX_BITS; Bit++) {
              CenterBit    = &CenterSumBit[Channel][Byte][Bit];
              RxDqPbCenter = &ChannelOut->RxDqPb[Rank][Byte][Bit].Center;
              TxDqPbCenter = &ChannelOut->TxDqPb[Rank][Byte][Bit].Center;

              DivBySign  = (*CenterBit < 0) ? (-1) : 1;
              *CenterBit = (*CenterBit + (DivBySign * DivBy)) / (2 * DivBy);

              //
              // Centerpoint needs to be added to starting DqPb value
              //
              *CenterBit += (Param == RdT) ? (S32) *RxDqPbCenter : (S32) *TxDqPbCenter;

              //
              // Check for saturation
              //
              if (*CenterBit > DDRDATA0CH0_CR_RXPERBITRANK0_Lane0_MAX) {
                *CenterBit = DDRDATA0CH0_CR_RXPERBITRANK0_Lane0_MAX;
              } else if (*CenterBit < 0) {
                *CenterBit = 0;
              }

              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "% 4x", *CenterBit);

              //
              // Update MrcData
              //
              if (Param == RdT) {
                *RxDqPbCenter = (U8) *CenterBit;
              } else {
                *TxDqPbCenter = (U8) *CenterBit;
              }

              CrPerBitRank.Data |= (*CenterBit << (4 * Bit));
            }
            //
            // Apply new centerpoint
            // ParamB already has the proper per bit parameter based on Param
            //
            Status = ChangeMargin (
                      MrcData,
                      ParamB,
                      CrPerBitRank.Data,
                      0,
                      0,
                      Channel,
                      Rank,
                      Byte,
                      0,
                      0,
                      0,
                      MrcRegFileRank
                      );

            //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " DivBy %d   ", DivBy);
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "           ");
          }
          //
          // Clear any old state in DataTrain Offset
          //
          MrcOemMemorySetDword (&ChannelOut->DataOffsetTrain[0], 0, Outputs->SdramCount);
        }
      }
      //
      // No stop on error or selective error cheking
      // Stop on all lane fail
      //
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {
          //
          // SOE = 11b ( Stop on All Lanes Error )
          //
          ReutChErrCtl.Data                                  = 0;
          ReutChErrCtl.Bits.Stop_on_Nth_Error                = 1;
          ReutChErrCtl.Bits.Stop_On_Error_Control            = ALSOE;
          ReutChErrCtl.Bits.Selective_Error_Enable_Chunk     = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Chunk_MAX;
          ReutChErrCtl.Bits.Selective_Error_Enable_Cacheline = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Cacheline_MAX;
          Offset = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG +
            ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * Channel);
          MrcWriteCR (MrcData, Offset, ReutChErrCtl.Data);
        }
      }

#if 0  // This code is for debug purposes ONLY if we want to know the perbyte margins after calling the perbit centering
#ifdef MRC_DEBUG_PRINT
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n\nEdges\t");
      for (Vref = 0; Vref < (sizeof (VrefPoints) / sizeof (VrefPoints[0])); Vref++) {
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (ChBitMask & (MRC_BIT0 << Channel)) {
            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              for (Edge = 0; Edge < MAX_EDGES; Edge++) {
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d ", EyeShape[Vref][Channel][Byte][Edge]);
              }
            }
          }
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
        if (En2D == 0) {
          Vref = sizeof (VrefPoints) / sizeof (VrefPoints[0]);
        }
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nCalling GetMarginBit with per Byte Timing\nByte\n");
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (1 << Channel)) {
          for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%d\t\t\t\t\t", Byte);
          }
        }
      }

      for (Vref = 0; Vref < (sizeof (VrefPoints) / sizeof (VrefPoints[0])); Vref++) {
        Mode    = 0;
        Status  = MrcGetMarginBit (MrcData, ChBitMask, Rank, MarginBit, EyeShape[Vref], Param, Mode, 31);
        //
        // Loop once at nominal Vref point
        //
        if (En2D == 0) {
          Vref = sizeof (VrefPoints) / sizeof (VrefPoints[0]);
        }
      }

      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nPerByte Margins after Bit Centering\nLeft\tRight\tCenter\n");
      for (Vref = 0; Vref < (sizeof (VrefPoints) / sizeof (VrefPoints[0])); Vref++) {
        for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
          if (ChBitMask & (MRC_BIT0 << Channel)) {
            MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Channel %d\n", Channel);
            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              MRC_DEBUG_MSG (
                Debug,
                MSG_LEVEL_NOTE,
                "%d\t%d\t%d\n",
                EyeShape[Vref][Channel][Byte][0],
                EyeShape[Vref][Channel][Byte][1],
                (((S32) EyeShape[Vref][Channel][Byte][1] - (S32) EyeShape[Vref][Channel][Byte][0]) / (2 * 10))
                );
            }
          }
        }

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
        if (En2D == 0) {
          Vref = sizeof (VrefPoints) / sizeof (VrefPoints[0]);
        }
      }
#endif // MRC_DEBUG_PRINT
#endif

      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        if (ChBitMask & (MRC_BIT0 << Channel)) {
          //
          // Cleanup after test
          //
          ReutChErrCtl.Data                                  = 0;
          ReutChErrCtl.Bits.Stop_on_Nth_Error                = 1;
          ReutChErrCtl.Bits.Stop_On_Error_Control            = NSOE;
          ReutChErrCtl.Bits.Selective_Error_Enable_Chunk     = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Chunk_MAX;
          ReutChErrCtl.Bits.Selective_Error_Enable_Cacheline = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_Selective_Error_Enable_Cacheline_MAX;
          Offset = MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG +
            ((MCHBAR_CH1_CR_REUT_CH_ERR_CTL_REG - MCHBAR_CH0_CR_REUT_CH_ERR_CTL_REG) * Channel);
          MrcWriteCR (MrcData, Offset, ReutChErrCtl.Data);
        }
      }
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
  } //End of Rank
  //
  // Clean Up after test
  //
  Outputs->EnDumRd  = 0;
  Status            = ChangeMargin (MrcData, ParamV, 0, 0, 1, 0, 0, 0, 0, 0, 0, MrcRegFileCurrent);
  return Status;
}

/**
  Subfunction of 2D Timing Centering
  Measures paramV margin across ch/bytes and updates the EH/VrefScale variables

  @param[in]     MrcData   - Include all MRC global data.
  @param[in]     ChBitMask - Channel Bit mak for which test should be setup for.
  @param[in]     rank      - Defines rank to used for MrcData
  @param[in]     ParamV    - Margin parameter
  @param[in]     MaxVScale - Maximum Voltage Scale to use
  @param[in]     BMap      - Byte mapping to configure error counter control register
  @param[in,out] EH        - Structure that stores start, stop and increment details for address
  @param[in,out] VrefScale - Parameter to be updated
  @param[in,out] BERStats  - Bit Error Rate Statistics.

  @retval mrcSuccess if successful, otherwise the function returns an error status.
**/
MrcStatus
DQTimeCenterEH (
  IN     MrcParameters * const MrcData,
  IN     const U8              ChBitMask,
  IN     const U8              rank,
  IN     const U8              ParamV,
  IN     const U8              MaxVScale,
  IN     U8 * const            BMap,
  IN OUT U32                   EH[MAX_CHANNEL][MAX_SDRAM_IN_DIMM],
  IN OUT U32                   VrefScale[MAX_CHANNEL][MAX_SDRAM_IN_DIMM],
  IN OUT U32 * const           BERStats
  )
{
  const MrcDebug  *Debug;
  MrcOutput       *Outputs;
  U32             *MarginResult;
  U32             *VrefS;
  MrcStatus       Status;
  U8              ResultType;
  U8              Channel;
  U8              Byte;
  U32             MinVrefScale;
  U16             Mode;

  Debug   = &MrcData->SysIn.Inputs.Debug;
  Outputs = &MrcData->SysOut.Outputs;
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "DQTimeCenterEH:\n");
  // Run Margin Test
  //
  Mode    = 0;
  Status  = GetMarginByte (MrcData, Outputs->MarginResult, ParamV, rank, (MRC_BIT0 << rank));
  if (mrcSuccess == Status) {
    Status = MrcGetBERMarginByte (
              MrcData,
              Outputs->MarginResult,
              ChBitMask,
              rank,
              rank,
              ParamV,
              Mode,
              BMap,
              1,
              MAX_POSSIBLE_VREF,
              0,
              BERStats
              );
    if (mrcSuccess == Status) {
      Status = ScaleMarginByte (MrcData, Outputs->MarginResult, ParamV, rank);
      if (mrcSuccess == Status) {
        ResultType = GetMarginResultType (ParamV);

        //
        // Update VrefScale with results
        //
        for (Channel = 0; (Channel < MAX_CHANNEL) && (mrcSuccess == Status); Channel++) {
          if (ChBitMask & (MRC_BIT0 << Channel)) {
            //
            // Calculate EH and VrefScale
            //
            MinVrefScale = MaxVScale;
            for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
              MarginResult      = &Outputs->MarginResult[ResultType][rank][Channel][Byte][0];
              VrefS             = &VrefScale[Channel][Byte];
              EH[Channel][Byte] = (*MarginResult + *(MarginResult + 1)) / 10;
              *VrefS            = EH[Channel][Byte] / 2;

              if (*VrefS > MaxVScale) {
                *VrefS = MaxVScale;
              }

              if (MinVrefScale > *VrefS) {
                MinVrefScale = *VrefS;
              }
              //
              // Scale host back to correct values
              //
              Status = ScaleMarginByte (MrcData, Outputs->MarginResult, ParamV, rank);
              if (mrcSuccess != Status) {
                break;
              }
              //
              // For Tx, use the same Vref limit for all bytes. Store result in byte0
              //
              if (ParamV == WrV) {
                MrcOemMemorySetDword (&VrefScale[Channel][0], MinVrefScale, Outputs->SdramCount);
              }
            }
          }
        }
      }
    }
  }
  //
  // Updates EH and VrefScale
  //
  return Status;
}

/**
  Update the Vref value
  if VrefType = 0     Updates Ch0 Vref_Dq
  if VrefType = 1     Updates Ch1 Vref_Dq
  if VrefType = 2     Updates Vref_CA

  @param[in,out] MrcData       - Include all MRC global data.
  @param[in]     VrefType      - Determines the Vref to change
  @param[in]     UpdateMrcData - Used to decide if Mrc host must be updated
  @param[in]     Offset        - Vref value
  @param[in]     SkipWait      - Determines if we will wait for vref to settle after writing to register

  @retval Nothing
**/
void
UpdateVrefWaitTilStable (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             VrefType,
  IN     const U8             UpdateMrcData,
  IN     S32                  Offset,
  IN     U8                   SkipWait
  )
{
  const MrcDebug                          *Debug;
  MrcInput                                *Inputs;
  MrcOutput                               *Outputs;
  U32                                     CheckMask;
  U8                                      OffH;
  U8                                      Count;
  DDRDATA7CH1_CR_DDRCRVREFADJUST1_STRUCT  DdrCrVrefAdjust;

  Inputs  = &MrcData->SysIn.Inputs;
  Debug   = &Inputs->Debug;
  Outputs = &MrcData->SysOut.Outputs;

  //
  // Calculate and write the new Vref offset value.
  //
  switch (VrefType) {
    case 0:
      OffH = (U8) (((DDRDATA7CH1_CR_DDRCRVREFADJUST1_STRUCT *) (&Outputs->DimmVref))->Bits.Ch0VrefCtl);
      break;

    case 1:
      OffH = (U8) (((DDRDATA7CH1_CR_DDRCRVREFADJUST1_STRUCT *) (&Outputs->DimmVref))->Bits.Ch1VrefCtl);
      break;

    case 2:
      OffH = (U8) (((DDRDATA7CH1_CR_DDRCRVREFADJUST1_STRUCT *) (&Outputs->DimmVref))->Bits.CAVrefCtl);
      break;

    default:
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "UpdateVrefWaitTilStable called with an incorrect value!\n");
      return;
  }

  Offset = Offset + (S8) MrcSE (OffH, 7, 8); // Get offset from host. SE = Sign Extend number 7->8 bits
  if (Offset > MAX_POSSIBLE_VREF) {
    Offset = MAX_POSSIBLE_VREF;
  } else if (Offset < (-1 * MAX_POSSIBLE_VREF)) {
    Offset = -1 * MAX_POSSIBLE_VREF;
  }

  if (UpdateMrcData) {
    switch (VrefType) {
      case 0:
        (((DDRDATA7CH1_CR_DDRCRVREFADJUST1_STRUCT *) (&Outputs->DimmVref))->Bits.Ch0VrefCtl) = Offset;
        break;

      case 1:
        (((DDRDATA7CH1_CR_DDRCRVREFADJUST1_STRUCT *) (&Outputs->DimmVref))->Bits.Ch1VrefCtl) = Offset;
        break;

      case 2:
        (((DDRDATA7CH1_CR_DDRCRVREFADJUST1_STRUCT *) (&Outputs->DimmVref))->Bits.CAVrefCtl) = Offset;
        break;

      default:
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "UpdateVrefWaitTilStable called with an incorrect value!\n");
        return;
    }
  }

  DdrCrVrefAdjust.Data = MrcReadCR (MrcData, DDRDATA7CH1_CR_DDRCRVREFADJUST1_REG);
  switch (VrefType) {
    case 0:
      DdrCrVrefAdjust.Bits.Ch0VrefCtl = Offset;
      CheckMask                       = DDRDATA7CH1_CR_DDRCRVREFADJUST1_ch0SlowBW_MSK;
      break;

    case 1:
      DdrCrVrefAdjust.Bits.Ch1VrefCtl = Offset;
      CheckMask                       = DDRDATA7CH1_CR_DDRCRVREFADJUST1_ch1SlowBW_MSK;
      break;

    case 2:
      DdrCrVrefAdjust.Bits.CAVrefCtl = Offset;
      CheckMask                      = DDRDATA7CH1_CR_DDRCRVREFADJUST1_caSlowBW_MSK;
      break;

    default:
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "UpdateVrefWaitTilStable called with an incorrect value!\n");
      return;
  }

  MrcWriteCrMulticast (MrcData, DDRDATA7CH1_CR_DDRCRVREFADJUST1_REG, DdrCrVrefAdjust.Data);

  //
  // Wait for Vref to settle.  Note VrefCA needs longer to settle.
  //
  if (!SkipWait) {
    Count = 0;
    while (Count < 50) {
      //
      // Don't wait more than 50uS
      //
      if ((MrcReadCR (MrcData, DDRDATA7CH1_CR_DDRCRVREFADJUST1_REG) & CheckMask) == CheckMask) {
        break;
      }

      MrcWait (MrcData, 1 * HPET_1US);
      Count += 1;
    }

    if (Count >= 50) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "CAVref circuit failed to converge, \n");
    }

    MrcWait (MrcData, 5 * HPET_1US); // Add 5us to make sure everything is done
  }
}

/**
  This function is used to move CMD/CTL/CLK/CKE PIs during training

  @param[in,out] MrcData    - Include all MRC global data.
  @param[in]     Channel    - Channel to shift PI for
  @param[in]     Iteration  - Determines which PI to shift:
                              MrcIterationClock = 0
                              MrcIterationCmdN  = 1
                              MrcIterationCmdS  = 2
                              MrcIterationCke   = 3
                              MrcIterationCtl   = 4
                              MrcIterationCmdV  = 5
  @param[in]     RankMask   - Ranks to work on
  @param[in]     GroupMask  - which LPDDR groups to work on for CMD/CLK/CKE; not used for DDR3
  @param[in]     NewValue   - value to shift in case of CLK Iteration, New value for all other cases
  @param[in]     UpdateHost - Determines if MrcData structure is updated

  @retval Nothing
**/
void
ShiftPIforCmdTraining (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Channel,
  IN     const U8             Iteration,
  IN     const U8             RankMask,
  IN     const U8             GroupMask,
  IN     S32                  NewValue,
  IN     const U8             UpdateHost
  )
{
  const MrcInput                        *Inputs;
  const MrcDebug                        *Debug;
  const MrcChannelIn                    *ChannelIn;
  MrcChannelOut                         *ChannelOut;
  MrcOutput                             *Outputs;
  U32                                   Offset;
  U32                                   ByteMask;
  U32                                   BitOffset;
  U8                                    Rank;
#ifdef ULT_FLAG
  U8                                    Group;
  U32                                   Cke;
  U32                                   CkeRankMapping;
#endif // ULT_FLAG
  S8                                    Shift;
  BOOL                                  Lpddr;
  DDRCLKCH0_CR_DDRCRCLKPICODE_STRUCT    ClkPiCode;
  DDRCKECH0_CR_DDRCRCMDPICODING_STRUCT  CkeCmdPiCoding;
  DDRCMDSCH0_CR_DDRCRCMDPICODING_STRUCT CmdSPiCoding;
  DDRCMDNCH0_CR_DDRCRCMDPICODING_STRUCT CmdNPiCoding;
  DDRCTLCH0_CR_DDRCRCTLPICODING_STRUCT  CtlPiCoding;

  Inputs     = &MrcData->SysIn.Inputs;
  Outputs    = &MrcData->SysOut.Outputs;
  Debug      = &MrcData->SysIn.Inputs.Debug;
  ChannelIn  = &Inputs->Controller[0].Channel[Channel];
  ChannelOut = &Outputs->Controller[0].Channel[Channel];
  Lpddr      = (Outputs->DdrType == MRC_DDR_TYPE_LPDDR3);

  if (Iteration != MrcIterationClock) {
    if (NewValue < 0) {
      NewValue = 0;
    } else if (NewValue > 127) {
      NewValue = 127;
    }
  }

  //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nShiftPIforCmdTraining: Iteration: %d, Channel: %d, RankMask: %d, GroupMask: %d, NewValue = 0x%x\n", Iteration, Channel, RankMask, GroupMask, NewValue);

  switch (Iteration) {
    case MrcIterationClock: // SHIFT CLOCK
      ClkPiCode.Data = 0;
      ByteMask  = 0x1FF;    // Shift DQ PI on all 9 bytes by default on DDR3

#ifdef ULT_FLAG
      if (Lpddr) {
        //
        // In LPDDR clocks are per group, not per rank
        //
        for (Group = 0; Group < 2; Group++) {
          BitOffset = DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_WID * Group;
          if (GroupMask & (1 << Group)) {
            Shift = (ChannelOut->ClkPiCode[Group] + NewValue) % 128;
            if (Shift < 0) {
              Shift = (128 - ABS (Shift));
            }

            Shift &= DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_MSK;
            if (UpdateHost) {
              ChannelOut->ClkPiCode[Group] = Shift;
            }

            ClkPiCode.Data += (Shift << BitOffset);
            //
            // Each clock spans all ranks, so need to shift DQ PIs on all ranks, for bytes in this group only
            //
            ByteMask = ChannelIn->DQByteMap[MrcIterationClock][Group];
            for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
              if (MrcRankInChannelExist (MrcData, Rank, Channel)) {
                ShiftDQPIs (MrcData, Channel, Rank, ByteMask, (S8) NewValue, UpdateHost, 0);
              }
            }
          } else {
            ClkPiCode.Data += (ChannelOut->ClkPiCode[Group] << BitOffset);
          }
        } // for Group
      } else
#endif // ULT_FLAG
      {
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          BitOffset = DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_WID * Rank;
          if (RankMask & (1 << Rank)) {
            Shift = (ChannelOut->ClkPiCode[Rank] + NewValue) % 128;
            if (Shift < 0) {
              Shift = (128 - ABS (Shift));
            }

            Shift &= DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_MSK;
            if (UpdateHost) {
              ChannelOut->ClkPiCode[Rank] = Shift;
            }

            ClkPiCode.Data += (Shift << BitOffset);
            ShiftDQPIs (MrcData, Channel, Rank, ByteMask, (S8) NewValue, UpdateHost, 0);
          } else {
            ClkPiCode.Data += (ChannelOut->ClkPiCode[Rank] << BitOffset);
          }
        }
      }

      Offset = DDRCLKCH0_CR_DDRCRCLKPICODE_REG + ((DDRCLKCH1_CR_DDRCRCLKPICODE_REG - DDRCLKCH0_CR_DDRCRCLKPICODE_REG) * Channel);
      MrcWriteCR (MrcData, Offset, ClkPiCode.Data);
      //MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "**** ShiftPIforCmdTraining, Iteration = %d, CRValue = 0x%x ****\n", Iteration,CRValue);
      break;

    case MrcIterationCmdN: // SHIFT COMMAND NORTH
      CmdNPiCoding.Data = 0;
      NewValue = MIN (NewValue, DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi0Code_MAX);
      //
      // HSW ULT LPDDR3: CMDN.CmdPi1Code controls CAB
      // HSW ULT DDR3L:  Both CmdPi0Code and CmdPi1Code should have the same value
      // HSW TRAD DDR3L: No harm setting CmdPi1Code same as CmdPi0Code
      //
      CmdNPiCoding.Bits.CmdPi0Code = NewValue;
      CmdNPiCoding.Bits.CmdPi1Code = NewValue;
      Offset = DDRCMDNCH0_CR_DDRCRCMDPICODING_REG +
        ((DDRCMDNCH1_CR_DDRCRCMDPICODING_REG - DDRCMDNCH0_CR_DDRCRCMDPICODING_REG) * Channel);
      MrcWriteCR (MrcData, Offset, CmdNPiCoding.Data);
      if (UpdateHost) {
        ChannelOut->CmdnCmdPiCode[0] = NewValue;
        ChannelOut->CmdnCmdPiCode[1] = NewValue;
      }
      break;

    case MrcIterationCmdS: // SHIFT COMMAND SOUTH
      CmdSPiCoding.Data = 0;
      NewValue = MIN (NewValue, DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi0Code_MAX);
      //
      // HSW ULT LPDDR3: CMDS.CmdPi0Code controls CAA, CMDS.CmdPi1Code controls CAB
      // HSW ULT DDR3L:  Both CmdPi0Code and CmdPi1Code should have the same value, also program CKE fub
      // HSW TRAD DDR3L: No harm setting CmdPi1Code same as CmdPi0Code, also program CKE fub
      //
#ifdef ULT_FLAG
      if (Lpddr) {
        CmdSPiCoding.Bits.CmdPi0Code = (GroupMask & 1) ? (U32) NewValue : ChannelOut->CmdsCmdPiCode[0]; // CAA
        CmdSPiCoding.Bits.CmdPi1Code = (GroupMask & 2) ? (U32) NewValue : ChannelOut->CmdsCmdPiCode[1]; // CAB
      } else
#endif // ULT_FLAG
      {
        CmdSPiCoding.Bits.CmdPi0Code = NewValue;
        CmdSPiCoding.Bits.CmdPi1Code = NewValue;

        CkeCmdPiCoding.Data             = 0;
        NewValue                        = MIN (NewValue, DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi0Code_MAX);
        CkeCmdPiCoding.Bits.CmdPi0Code  = NewValue;
        CkeCmdPiCoding.Bits.CmdPi1Code  = NewValue;
        Offset = DDRCKECH0_CR_DDRCRCMDPICODING_REG +
          ((DDRCKECH1_CR_DDRCRCMDPICODING_REG - DDRCKECH0_CR_DDRCRCMDPICODING_REG) * Channel);
        MrcWriteCR (MrcData, Offset, CkeCmdPiCoding.Data);
        if (UpdateHost) {
          ChannelOut->CkeCmdPiCode[0] = NewValue;
          ChannelOut->CkeCmdPiCode[1] = NewValue;
        }
      }

      if (UpdateHost) {
        ChannelOut->CmdsCmdPiCode[0] = CmdSPiCoding.Bits.CmdPi0Code;
        ChannelOut->CmdsCmdPiCode[1] = CmdSPiCoding.Bits.CmdPi1Code;
      }

      Offset = DDRCMDSCH0_CR_DDRCRCMDPICODING_REG +
        ((DDRCMDSCH1_CR_DDRCRCMDPICODING_REG - DDRCMDSCH0_CR_DDRCRCMDPICODING_REG) * Channel);
      MrcWriteCR (MrcData, Offset, CmdSPiCoding.Data);
      break;

    case MrcIterationCke: // Shift CKE command
      CkeCmdPiCoding.Data = 0;
      NewValue            = MIN (NewValue, DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi0Code_MAX);
      //
      // HSW ULT LPDDR3: CKE.CmdPi0Code controls CAA
      // HSW ULT DDR3L:  Both CmdPi0Code and CmdPi1Code should have the same value
      // HSW TRAD DDR3L: No harm setting CmdPi1Code same as CmdPi0Code
      //
      CkeCmdPiCoding.Bits.CmdPi0Code = NewValue;
      CkeCmdPiCoding.Bits.CmdPi1Code = NewValue;
      Offset = DDRCKECH0_CR_DDRCRCMDPICODING_REG +
        ((DDRCKECH1_CR_DDRCRCMDPICODING_REG - DDRCKECH0_CR_DDRCRCMDPICODING_REG) * Channel);
      MrcWriteCR (MrcData, Offset, CkeCmdPiCoding.Data);
      if (UpdateHost) {
        ChannelOut->CkeCmdPiCode[0] = NewValue;
        ChannelOut->CkeCmdPiCode[1] = NewValue;
      }
      break;

    case MrcIterationCtl: // Shift CS/ODT and CKE.Control
      CtlPiCoding.Data = 0;
      NewValue         = MIN (NewValue, DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_MAX);
      for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
        if (RankMask & (1 << Rank)) {
          CtlPiCoding.Data += (NewValue << (DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_WID * Rank));
          if (UpdateHost) {
            ChannelOut->CtlPiCode[Rank] = (U8) NewValue;
            ChannelOut->CkePiCode[Rank] = (U8) NewValue;
          }
        } else {
          CtlPiCoding.Data += (ChannelOut->CtlPiCode[Rank] << (DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_WID * Rank));
        }
      }

#ifdef ULT_FLAG
      if (Lpddr && Inputs->LpddrDramOdt) {
        //
        // ODT[0] (equal to CS[0] PI setting) goes in to CTL.CtlPiCode2
        //
        CtlPiCoding.Bits.CtlPiCode2 = ChannelOut->CtlPiCode[0];
      }
#endif // ULT_FLAG
      Offset = DDRCTLCH0_CR_DDRCRCTLPICODING_REG +
        ((DDRCTLCH1_CR_DDRCRCTLPICODING_REG - DDRCTLCH0_CR_DDRCRCTLPICODING_REG) * Channel);
      MrcWriteCR (MrcData, Offset, CtlPiCoding.Data);

#ifdef ULT_FLAG
      if (Lpddr) {
        CtlPiCoding.Data = 0;
        //
        // Use CKE-to-Rank mapping: [3:0] - Channel 0, [7:4] - Channel 1
        //
        CkeRankMapping = (Inputs->CkeRankMapping >> (Channel * 4)) & 0x0F;
        for (Rank = 0; Rank < MAX_RANK_IN_CHANNEL; Rank++) {
          for (Cke = 0; Cke <= 3; Cke++) {
            if (((CkeRankMapping >> Cke) & 1) == Rank) {
              //
              // This CKE pin is connected to this Rank
              //
              CtlPiCoding.Data += (ChannelOut->CkePiCode[Rank] << (DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode0_WID * Cke));
            }
          }
        }
        //
        // Put average of CKE2 and CKE3 into CKE2 PI setting.
        //
        CtlPiCoding.Bits.CtlPiCode2 = (CtlPiCoding.Bits.CtlPiCode2 + CtlPiCoding.Bits.CtlPiCode3) / 2;
      }
#endif // ULT_FLAG
      Offset = DDRCKECH0_CR_DDRCRCTLPICODING_REG +
        ((DDRCKECH1_CR_DDRCRCTLPICODING_REG - DDRCKECH0_CR_DDRCRCTLPICODING_REG) * Channel);
      CtlPiCoding.Bits.CtlPiCode3 = 0;  // Do not write PiCode3
      MrcWriteCR (MrcData, Offset, CtlPiCoding.Data);
      break;

    default:
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "WARNING...Unknown parameter to shift\n");
      break;
  }

  return;
}

/**
  Shifts RcvEn, WriteLevel and WriteDQS timing for all bytes
  Usually used when moving the clocks on a channel

  @param[in,out] MrcData    - Include all MRC global data.
  @param[in]     Channel    - Channel to update
  @param[in]     Rank       - Rank to update
  @param[in]     ByteMask   - Bytes to update
  @param[in]     Offset     - value to shift
  @param[in]     UpdateHost - Determines if MrcData structure is updated
  @param[in]     SkipTx     - Determines if TX update should be skipped

  @retval Nothing
**/
void
ShiftDQPIs (
  IN OUT MrcParameters *const MrcData,
  IN     const U8             Channel,
  IN     const U8             Rank,
  IN     const U32            ByteMask,
  IN     const S8             Offset,
  IN     const U8             UpdateHost,
  IN     const U8             SkipTx
  )
{
  MrcOutput                           *Outputs;
  MrcChannelOut                       *ChannelOut;
  U8                                  Byte;
  S8                                  OffTx;
  U16                                 RcvEnValue;
  DDRDATA0CH0_CR_TXTRAINRANK0_STRUCT  CrTxTrainRank;

  const MrcDebug                      *Debug;

  Debug       = &MrcData->SysIn.Inputs.Debug;
  Outputs     = &MrcData->SysOut.Outputs;
  ChannelOut  = &Outputs->Controller[0].Channel[Channel];
  OffTx       = SkipTx ? 0 : Offset;

  for (Byte = 0; Byte < Outputs->SdramCount; Byte++) {
    if (((1 << Byte) & ByteMask) == 0) {
      continue;
    }

    RcvEnValue = ChannelOut->RcvEn[Rank][Byte] + Offset;
    if ((S16) RcvEnValue < 0) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "WARNING: RcvEn PI wrapped below zero!\n");
      RcvEnValue = 0;   // Don't go below zero
    } else if (RcvEnValue > DDRDATA_CR_RXTRAINRANK0_RxRcvEnPi_MAX) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "WARNING: RcvEn PI wrapped above 9 bits!\n");
      RcvEnValue = DDRDATA_CR_RXTRAINRANK0_RxRcvEnPi_MAX;   // Don't go above max
    }

    UpdateRxT (MrcData, Channel, Rank, Byte, 0, RcvEnValue);

    CrTxTrainRank.Data                = 0;
    CrTxTrainRank.Bits.TxEqualization = ChannelOut->TxEq[Rank][Byte];
    CrTxTrainRank.Bits.TxDqsDelay     = ChannelOut->TxDqs[Rank][Byte] + OffTx;
    CrTxTrainRank.Bits.TxDqDelay      = ChannelOut->TxDq[Rank][Byte] + OffTx;
    UpdateTxT (MrcData, Channel, Rank, Byte, 3, CrTxTrainRank.Data);

    if (UpdateHost) {
      ChannelOut->RcvEn[Rank][Byte]  = RcvEnValue;
      ChannelOut->TxDqs[Rank][Byte] += OffTx;
      ChannelOut->TxDq[Rank][Byte]  += OffTx;
    }
  }

  return;
}

/**
  Retrieve the current memory frequency and clock from the memory controller.

  @param[in]      MrcData     - Include all MRC global data.
  @param[in, out] MemoryClock - The current memory clock.
  @param[in, out] Ratio       - The current memory ratio setting.
  @param[in, out] RefClk      - The current memory reference clock.

  @retval: The current memory frequency.
**/
MrcFrequency
MrcGetCurrentMemoryFrequency (
  MrcParameters * const   MrcData,
  U32 * const             MemoryClock,
  MrcClockRatio * const   Ratio,
  MrcRefClkSelect * const RefClk
  )
{
  const MrcInput                 *Inputs;
  const MrcOutput                *Outputs;
  PCU_CR_MC_BIOS_DATA_PCU_STRUCT McBiosData;
  PCU_CR_MC_BIOS_REQ_PCU_STRUCT  McBiosReqPcu;

  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  McBiosReqPcu.Data = MrcReadCR (MrcData, PCU_CR_MC_BIOS_REQ_PCU_REG);
  McBiosData.Data   = MrcReadCR (MrcData, PCU_CR_MC_BIOS_DATA_PCU_REG);
  if (MemoryClock != NULL) {
    *MemoryClock = MrcRatioToClock ((MrcClockRatio) McBiosData.Bits.MC_FREQ, McBiosReqPcu.Bits.REQ_TYPE, Inputs->BClkFrequency);
  }
  if (Ratio != NULL) {
    *Ratio = (MrcClockRatio) McBiosData.Bits.MC_FREQ;
  }
  if (RefClk != NULL) {
    *RefClk = (MrcRefClkSelect) McBiosReqPcu.Bits.REQ_TYPE;
  }
  return MrcRatioToFrequency (
          MrcData,
          (MrcClockRatio) McBiosData.Bits.MC_FREQ,
          McBiosReqPcu.Bits.REQ_TYPE,
          Inputs->BClkFrequency
          );
}

#ifdef ULT_FLAG

/**
  Translate LPDDR command from CA[9:0] high and low phase to DDR3 MA/BA/CMD.
  This is needed to program CADB.

  @param[in]  CaHigh - CA[9:0] value on the rising clock
  @param[in]  CaLow  - CA[9:0] value on the falling clock
  @param[out] MA     - Translated value of MA[15:0]
  @param[out] BA     - Translated value of BA[2:0]
  @param[out] CMD    - Translated value of CMD[2:0] = [RASb, CASb, WEb]

  @retval none
**/
void
MrcConvertLpddr2Ddr (
  IN  U32 const CaHigh,
  IN  U32 const CaLow,
  OUT U32       *MA,
  OUT U32       *BA,
  OUT U32       *CMD
  )
{
  *MA  = MRC_BIT15;   // MA[15] should be 1
  *BA  = 0;
  *CMD = MRC_BIT2;    // RASb should be 1

  //
  // Translation table
  //
  // Command  High phase Low phase
  //-----------------------------
  // CA[0]    CASb       MA[0]
  // CA[1]    WEb        MA[1]
  // CA[2]    MA[8]      MA[2]
  // CA[3]    MA[9]      MA[3]
  // CA[4]    MA[10]     MA[4]
  // CA[5]    MA[11]     MA[5]
  // CA[6]    MA[12]     MA[6]
  // CA[7]    BA[0]      MA[7]
  // CA[8]    BA[1]      MA[13]
  // CA[9]    BA[2]      MA[14]

  *MA  |= (CaLow & 0xFF);          // MA[7:0]
  *MA  |= ((CaHigh & 0x7C) << 6);  // MA[12:8]
  *MA  |= ((CaLow & 0x300) << 5);  // MA[14:13]

  *BA  |= ((CaHigh & 0x380) >> 7); // BA[2:0]

  *CMD |= ((CaHigh & 0x02) >> 1);  // CMD[0] = WEb
  *CMD |= ((CaHigh & 0x01) << 1);  // CMD[1] = CASb
}

/**
  Run a short CADB sequence on selected channels

  @param[in] MrcData   - The MRC global data.
  @param[in] ChBitMask - channels to work on.

  @retval none
**/
void
ShortRunCADB (
  IN MrcParameters *const  MrcData,
  IN U8                    ChBitMask
  )
{
  U32   Channel;
  U32   Offset;
  MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_STRUCT   ReutGlobalCtl;
  MCSCHEDS_CR_REUT_CH_PAT_CADB_CTRL_STRUCT  ReutChPatCadbCtrl;
  MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_STRUCT ReutChSeqCfg;

  //
  // Enable REUT mode and Global Control on selected channels
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
      ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel);
    ReutChSeqCfg.Data = MrcReadCR64 (MrcData, Offset);
    if (((1 << Channel) & ChBitMask) != 0) {
      ReutChSeqCfg.Bits.Initialization_Mode = REUT_Testing_Mode;
      ReutChSeqCfg.Bits.Global_Control      = 1;
    } else {
      ReutChSeqCfg.Bits.Global_Control = 0;
    }

    MrcWriteCR64 (MrcData, Offset, ReutChSeqCfg.Data);
  }
  //
  // Enable CADB Always On mode
  //
  ReutChPatCadbCtrl.Data                        = 0;
  ReutChPatCadbCtrl.Bits.Enable_CADB_Always_On  = 1;
  MrcWriteCR (MrcData, MCSCHEDS_CR_REUT_CH_PAT_CADB_CTRL_REG, ReutChPatCadbCtrl.Data);

  //
  // Start CADB
  //
  ReutGlobalCtl.Data                    = 0;
  ReutGlobalCtl.Bits.Global_Start_Test  = 1;
  MrcWriteCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, ReutGlobalCtl.Data);

  //
  // It's enough to read from this register, no need for an extra delay
  //
  ReutGlobalCtl.Data = MrcReadCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG);
  //
  // Disable CADB Always On mode
  //
  ReutChPatCadbCtrl.Bits.Enable_CADB_Always_On = 0;
  MrcWriteCR (MrcData, MCSCHEDS_CR_REUT_CH_PAT_CADB_CTRL_REG, ReutChPatCadbCtrl.Data);

  //
  // Stop CADB
  //
  ReutGlobalCtl.Bits.Global_Start_Test  = 0;
  ReutGlobalCtl.Bits.Global_Stop_Test   = 1;
  MrcWriteCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG, ReutGlobalCtl.Data);

  //
  // Read back
  //
  ReutGlobalCtl.Data = MrcReadCR (MrcData, MCDFXS_CR_REUT_GLOBAL_CTL_MCMAIN_REG);

  //
  // Disable Global Control on selected channels
  //
  for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
    if (((1 << Channel) & ChBitMask) != 0) {
      Offset = MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG +
        ((MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_1_REG - MCDFXS_CR_REUT_CH_SEQ_CFG_MCMAIN_0_REG) * Channel);
      ReutChSeqCfg.Data                 = MrcReadCR64 (MrcData, Offset);
      ReutChSeqCfg.Bits.Global_Control  = 0;
      MrcWriteCR64 (MrcData, Offset, ReutChSeqCfg.Data);
    }
  }
}

#endif // ULT_FLAG

/**
  Get the Rx Bias values

  @param[in, out] MrcData    - Include all MRC global data.
  @param[in, out] RxFselect  - Location to save RxFselect.
  @param[in, out] RxCBSelect - Location to save RxCBSelect.

  @retval none
**/
void
GetRxFselect (
  IN MrcParameters *const MrcData,
  IN OUT S8            *RxFselect,
  IN OUT U8            *RxCBSelect
  )
{
  MrcOutput                    *Outputs;
  DDRCLK_CR_DDRCBSTATUS_STRUCT DdrCbStatus;

  Outputs          = &MrcData->SysOut.Outputs;
  DdrCbStatus.Data = MrcReadCR (MrcData, DDRCLK_CR_DDRCBSTATUS_REG);
  *RxCBSelect      = (U8) DdrCbStatus.Bits.DllCB;
  *RxFselect       = (Outputs->Ratio - ((Outputs->RefClk == MRC_REF_CLOCK_133) ? RXF_SELECT_RC_133 : RXF_SELECT_RC_100));

  //
  // Limit ratios for 1067, 1333, 1600, 1867 & 2133 MHz
  //
  *RxFselect = MIN (*RxFselect, RXF_SELECT_MAX);  // Maximum 2133 MHz
  *RxFselect = MAX (*RxFselect, RXF_SELECT_MIN);  // Minimum 1067 MHz
  return;
}
