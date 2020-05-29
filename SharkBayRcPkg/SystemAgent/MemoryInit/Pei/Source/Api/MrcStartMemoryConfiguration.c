/** @file
  Starting point for the core memory reference code.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved.
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
  license agreement

**/

//
// Include files
//
#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcCommandTraining.h>
#include <MrcCommon.h>
#include <MrcCrosser.h>
#include <MrcDdr3.h>
#include <MrcDebugHook.h>
#include <MrcGeneral.h>
#include <MrcGlobal.h>
#include <MrcBdat.h>
#include <MrcMcConfiguration.h>
#include <MrcMemoryMap.h>
#include <MrcMemoryScrub.h>
#include <MrcOem.h>
#include <MrcReadDqDqs.h>
#include <MrcReadReceiveEnable.h>
#include <MrcReset.h>
#include <MrcSaveRestore.h>
#include <MrcSpdProcessing.h>
#include <MrcStartMemoryConfiguration.h>
#include <MrcWriteDqDqs.h>
#include <MrcWriteLeveling.h>

/**
  Print the input parameters to the debug message output port.

  @param[in] MrcData - The MRC global data.

  @retval mrcSuccess
**/

//
// Functions:
//
const CallTableEntry  CallTable[] = {
  ///
  /// The functions are executed in the following order, as the policy flag dictates.
  /// Mrctask, post_code, OEM command, policy_flag, iteration, debug_string
  ///
  {MrcFastBootPermitted,       MRC_FAST_BOOT_PERMITTED,      OemFastBootPermitted,                                           MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Fast boot permitted")},
  {MrcRestoreNonTrainingValues,MRC_RESTORE_NON_TRAINING,     OemRestoreNonTraining,                MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Restore non-training values")},
#ifdef MRC_DEBUG_PRINT
  {MrcPrintInputParameters,    MRC_PRINT_INPUT_PARAMS,       OemPrintInputParameters,MRC_PF_COLD | MRC_PF_WARM             | MRC_PF_FAST | MRC_PF_FULL_MRC,                   MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Print input parameters")},
#endif // MRC_DEBUG_PRINT
  {MrcSetOverridesPreSpd,      MRC_SET_OVERRIDES_PSPD,       OemSetOverridePreSpd,   MRC_PF_COLD | MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Pre-SPD Timing overrides")},
  {MrcMcCapabilityPreSpd,      MRC_MC_CAPABILITY_PSPD,       OemMcCapabilityPreSpd,  MRC_PF_COLD | MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Pre-SPD MC Capabilities")},
  {MrcSpdProcessing,           MRC_SPD_PROCESSING,           OemSpdProcessingRun,    MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("SPD PROCESSING")},
  {MrcSetOverrides,            MRC_SET_OVERRIDES,            OemSetOverride,         MRC_PF_COLD | MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Timing overrides")},
  {MrcMcCapability,            MRC_MC_CAPABILITY,            OemMcCapability,        MRC_PF_COLD | MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MC Capabilities")},
  {MrcMcConfiguration,         MRC_MC_CONFIG,                OemMcInitRun,           MRC_PF_COLD | MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MC CONFIG")},
  {MrcSetMemoryMap,            MRC_MC_MEMORY_MAP,            OemMcMemoryMap,         MRC_PF_COLD | MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MC MEMORY MAP")},
  {MrcResetSequence,           MRC_RESET_SEQUENCE,           OemMcResetRun,          MRC_PF_COLD                           | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("JEDEC RESET")},
  {MrcPreTraining,             MRC_PRE_TRAINING,             OemPreTraining,         MRC_PF_COLD                           | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Pre-Training")},
  {MrcSenseAmpOffsetTraining,  MRC_SENSE_AMP_OFFSET,         OemSenseAmpTraining,    MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("SenseAmp Offset Training")},
  {MrcEarlyCommandTraining,    MRC_EARLY_COMMAND,            OemEarlyCommandTraining,MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Early command training")},
#ifdef ULT_FLAG
  {MrcJedecInitLpddr3,         MRC_JEDEC_INIT_LPDDR3,        OemJedecInitLpddr3,     MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("LPDDR3 JEDEC INIT")},
#endif // ULT_FLAG
  {MrcReadLevelingTraining,    MRC_RECEIVE_ENABLE,           OemReceiveEnable,       MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Read Leveling training")},
  {MrcReadMprTraining,         MRC_READ_MPR,                 OemReadMprTraining,     MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Read MPR training")},

  {MrcJedecWriteLevelingTraining,MRC_JEDEC_WRITE_LEVELING,   OemJedecWriteLeveling,  MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Jedec Write Leveling training")},

  {MrcWriteTimingCentering,    MRC_WRITE_TIMING_1D,          OemWriteDqDqs,          MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Write Timing Centering")},
  {MrcReadTimingCentering,     MRC_READ_TIMING_1D,           OemReadDqDqs,           MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Read Timing Centering")},

  {MrcPowerSavingMeter,        MRC_PWR_MTR,                  OemPowerSavingMeter,    MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("PowerSavingMeter Base Line Update")},
  {MrcDimmRonTraining,         MRC_DIMM_RON,                 OemDimmRonTraining,     MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("DIMM Ron Training")},
  {MrcDimmODTTraining,         MRC_DIMM_ODT,                 OemDimmODTTraining,     MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("DIMM ODT Training")},
  {MrcDimmODT1dTraining,       MRC_DIMM_ODT,                 OemDimmODT1dTraining,   MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("DIMM ODT 1d Training")},

  {MrcWriteDriveStrength,      MRC_WRITE_DS,                 OemWriteDriveStrength,  MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Write Drive Strength")},
  {MrcWriteEQTraining,         MRC_WRITE_EQ,                 OemWriteEQTraining,     MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Write Equalization Training")},
  {MrcReadODTTraining,         MRC_READ_ODT,                 OemReadODTTraining,     MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Read ODT Training")},

  {MrcWriteSlewRate,           MRC_WRITE_SR,                 OemWriteSlewRate,       MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Write Slew Rate")},
  {MrcReadAmplifierPower,      MRC_READ_AMP_POWER,           OemReadAmplifierPower,  MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Read Amplifier Power")},
  {MrcReadEQTraining,          MRC_READ_EQ,                  OemReadEQTraining,      MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Read Equalization Training")},
  {MrcOptimizeComp,            MRC_CMP_OPT,                  OemOptimizeComp,        MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Comp Optimization Training")},
//
/// @attention This sections of tests are left for future testing.  Determine later if we can remove.
//  {MrcTestGetMarginBitWrTBit,     MRC_ODT_STRETCH_START,      OemReadODTTraining,     MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Get the Margins per CHANNEL")},
//  {MrcTestGetBERMarginByteWrT,    MRC_READ_START,             OemReadEQTraining,      MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Get the Margins per BYTE for RcvEn")},
//  {MrcTestGetBERMarginByteRdT,    MRC_READ_START,             OemReadAmplifierPower,  MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Get the Margins per BYTE for RdT")},
//  {MrcTestGetBERMarginByteRcvEna, MRC_WRITE_START,            OemWriteDqDqs2D,        MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Get the Margins per BYTE for WrT")},
//  {MrcTestGetBERMarginByteWrDqsT, MRC_READ_START,             OemReadDqDqs2D,         MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Get the Margins per BYTE for WrdqsT")},
//  {MrcTestGetBERMarginByteWrLevel,MRC_WRITE_START,            OemWriteVoltCentering2D,MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Get the Margins per BYTE for WrLevel")},
//  {MrcTestGetBERMarginCh,         MRC_READ_START,             OemReadVoltCentering2D, MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Get the Margins per BIT for WrTBit")},
//  {MrcTestGetMarginBitRdTBit,     MRC_READ_START,             OemWriteXtalkCancel,    MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Get the Margins per BIT for RdTBit")},
//
  {MrcPostTraining,            MRC_POST_TRAINING,            OemPostTraining,        MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Post-training")},
  {MrcLateCommandTraining,     MRC_LATE_COMMAND,             OemLateCommandTraining, MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Late command training")},

  {MrcCmdVoltageCentering,     MRC_CMD_VREF,                 OemCmdVoltCentering,    MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Command Voltage Centering")},
  {MrcWriteVoltageCentering2D, MRC_WRITE_VREF_2D,            OemWriteVoltCentering2D,MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Write Voltage Centering 2D")},
  {MrcReadVoltageCentering2D,  MRC_READ_VREF_2D,             OemReadVoltCentering2D, MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Read Voltage Centering 2D")},

  {MrcWriteTimingCentering2D,  MRC_WRITE_TIMING_2D,          OemWriteDqDqs2D,        MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Write Timing Centering 2D")},
  {MrcReadTimingCentering2D,   MRC_READ_TIMING_2D,           OemReadDqDqs2D,         MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Read Timing Centering 2D")},

  {MrcRoundTripLatency,        MRC_ROUND_TRIP_LAT,           OemRoundTripLatency,    MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Round Trip Latency Training")},
  {MrcTurnAroundTiming,        MRC_TURN_AROUND,              OemTurnAroundTimes,     MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Turn Around Trip Training")},
#ifdef ULT_FLAG
  {MrcReceiveEnTimingCentering,MRC_RCVEN_TIMING_1D,          OemRcvEnCentering1D,    MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Receive Enable Timing Centering")},
#endif // ULT_FLAG
  {MrcRetrainMarginCheck,      MRC_RETRAIN_CHECK,            OemRetrainMarginCheck,  MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, 1, MRC_DEBUG_TEXT("Check Margin for Retrain")},
  {MrcRankMarginTool,          MRC_RMT_TOOL,                 OemRmt,                 MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Rank Margin Tool")},
  {MrcPowerSavingMeter,        MRC_PWR_MTR,                  OemPowerSavingMeter,    MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("PowerSavingMeter update")},
  {MrcMcActivate,              MRC_MC_ACTIVATE,              OemMrcActivate,         MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MRC activate")},
  {MrcSaveMCValues,            MRC_SAVE_MC_VALUES,           OemSaveMCValues,        MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Save MC Values")},
  {MrcRestoreTrainingValues,   MRC_RESTORE_TRAINING,         OemRestoreTraining,                   MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Restore Training Values")},
#ifdef ULT_FLAG
  {MrcJedecInitLpddr3,         MRC_JEDEC_INIT_LPDDR3,        OemJedecInitLpddr3,                                             MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("LPDDR3 JEDEC INIT")},
#endif // ULT_FLAG
  {MrcSelfRefreshExit,         MRC_SELF_REFRESH_EXIT,        OemSelfRefreshExit,                   MRC_PF_WARM | MRC_PF_S3               | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Perform Self Refresh Exit")},
  {MrcNormalMode,              MRC_NORMAL_MODE,              OemNormalMode,                        MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("Normal Operation For Non-Cold Boots")},
/// @attention: MrcAliasCheck must run before any test modifying the WDB entries to zero for memory scrubbing.
  {MrcAliasCheck,              MRC_ALIAS_CHECK,              OemAliasCheck,          MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MRC Memory alias check")},
  {MrcHwMemTest,               MRC_CPGC_MEMORY_TEST,         OemMemTest,             MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MRC HW Memory testing")},
  {MrcEccClean,                MRC_ECC_CLEAN_START,          OemHwMemInit,           MRC_PF_COLD                           | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MRC HW Memory Init")},
  {MrcDone,                    MRC_DONE,                     OemMrcDone,             MRC_PF_COLD | MRC_PF_WARM | MRC_PF_S3 | MRC_PF_FAST | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MRC done")},
#ifdef BDAT_SUPPORT
  {MrcFillRmtStructure,        MRC_FILL_RMT_STRUCTURE,       OemMrcFillRmt,          MRC_PF_COLD                                         | MRC_PF_FULL_MRC | MRC_PF_MINI_MRC, MRC_ITERATION_MAX, MRC_DEBUG_TEXT("MRC Fill RMT Structure")},
#endif
};

/**
  Initializes the memory controller and DIMMs.

  @param[in, out] MrcData - Include all MRC global data.

  @retval mrcSuccess if the initalization suceeded, otherwise an error status indicating the failure.
**/
MrcStatus
MrcStartMemoryConfiguration (
  IN OUT MrcParameters *const MrcData
  )
{
  const CallTableEntry  *ctptr;
  const MrcDebug        *Debug;
  const MrcInput        *Inputs;
  MrcOutput             *Outputs;
  U64                   start_time;
  U64                   finish_time;
  U32                   ElapsedTime;
  U32                   TotalTime;
  U16                   index;
  U8                    Run;
  MrcPostCode           post_code;
  MrcStatus             MrcStatus;

  //
  // Time to sequence thru the MRC tasks.
  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  Debug   = &Inputs->Debug;
  Outputs->BootMode = Inputs->BootMode;
  MrcVersionGet (&Outputs->Version);
  MrcStatus = mrcFail;
  post_code = MRC_INITIALIZATION_START;
  Run       = 1;
  TotalTime = 0;
  for (index = 0; Run && (index < (sizeof (CallTable) / sizeof (CallTableEntry))); index++, post_code++) {
    ctptr = &CallTable[index];
    //
    // Output post code to post code I/O port.
    //
    MrcOemDebugHook (MrcData, ((ctptr->post_code_ovr == POST_CODE_NO_OVR) ? post_code : ctptr->post_code_ovr));
    //
    // Decide if we need to execute the selected MRC task.
    if ((NULL != ctptr->mrc_task) && (Inputs->Iteration < ctptr->iteration)) {
      if  (((Inputs->MrcMode == MrcModeFull) && (ctptr->policy_flag & MRC_PF_FULL_MRC))
        || ((Inputs->MrcMode == MrcModeMini) && (ctptr->policy_flag & MRC_PF_MINI_MRC))) {
        if   (((Outputs->BootMode == bmS3)   && (ctptr->policy_flag & MRC_PF_S3))
          ||  ((Outputs->BootMode == bmFast) && (ctptr->policy_flag & MRC_PF_FAST))
          ||  ((Outputs->BootMode == bmWarm) && (ctptr->policy_flag & MRC_PF_WARM))
          ||  ((Outputs->BootMode == bmCold) && (ctptr->policy_flag & MRC_PF_COLD))) {
          if ((ctptr->oem_cmd < OemNumOfCommands) && (mrcSuccess != MrcOemCheckPoint (MrcData, ctptr->oem_cmd, NULL))) {
            continue;
          }
          //
          // Output debug string to serial output and execute the MRC task.
          //
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nMRC task -- %s -- Started.\n", ctptr->String);
          start_time  = MrcGetCpuTime ();
          MrcStatus   = ctptr->mrc_task (MrcData);
          finish_time = MrcGetCpuTime ();
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_ERROR,
            "MRC task %s -- %s, Status = %Xh.\n",
            ctptr->String,
            (mrcSuccess == MrcStatus) ? "SUCCEEDED" : "FAILED",
            MrcStatus
            );
          if (mrcSuccess != MrcStatus) {
            Run = 0;  //Stop task execution on failure.
          }

          ElapsedTime = (U32) (finish_time - start_time);
          TotalTime += ElapsedTime;
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_TIME, "MRC timer: Task %s took %u msec.\n", ctptr->String, ElapsedTime);
        }
      }
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_TIME, "MRC timer: Total time to execute tasks = %u msec.\n", TotalTime);

  return MrcStatus;
}
