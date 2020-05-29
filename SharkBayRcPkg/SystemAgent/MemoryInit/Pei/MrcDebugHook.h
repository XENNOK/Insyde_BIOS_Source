/** @file
  This file defines all the MRC debug hooks.

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
#ifndef _MrcDebugHook_h_
#define _MrcDebugHook_h_

#define MRC_FAILURE_INDICATION          (0x0080)  /// This value is or'ed with below "*_ERROR" codes
#define MRC_INITIALIZATION_START        (0xDD00)

#define MRC_FAST_BOOT_PERMITTED         (0xDD1B)
#define MRC_FAST_BOOT_PERMITTED_ERROR   (MRC_FAST_BOOT_PERMITTED | MRC_FAILURE_INDICATION)

#define MRC_RESTORE_NON_TRAINING        (0xDD1C)
#define MRC_RESTORE_NON_TRAINING_ERROR  (MRC_RESTORE_NON_TRAINING | MRC_FAILURE_INDICATION)

#define MRC_PRINT_INPUT_PARAMS          (0xDD1D)
#define MRC_PRINT_INPUT_PARAMS_ERROR    (MRC_PRINT_INPUT_PARAMS | MRC_FAILURE_INDICATION)

#define MRC_SET_OVERRIDES_PSPD          (0xDD1E)
#define MRC_SET_OVERRIDES_PSPD_ERROR    (MRC_SET_OVERRIDES_PSPD | MRC_FAILURE_INDICATION)

#define MRC_MC_CAPABILITY_PSPD          (0xDD1F)
#define MRC_MC_CAPABILITY_PSPD_ERROR    (MRC_MC_CAPABILITY_PSPD | MRC_FAILURE_INDICATION)

#define MRC_SPD_PROCESSING              (0xDD20)
#define MRC_SPD_PROCESSING_ERROR        (MRC_SPD_PROCESSING | MRC_FAILURE_INDICATION)

#define MRC_SET_OVERRIDES               (0xDD21)
#define MRC_SET_OVERRIDES_ERROR         (MRC_SET_OVERRIDES | MRC_FAILURE_INDICATION)

#define MRC_MC_CAPABILITY               (0xDD22)
#define MRC_MC_CAPABILITY_ERROR         (MRC_MC_CAPABILITY | MRC_FAILURE_INDICATION)

#define MRC_MC_CONFIG                   (0xDD23)
#define MRC_MC_CONFIG_ERROR             (MRC_MC_CONFIG | MRC_FAILURE_INDICATION)

#define MRC_MC_MEMORY_MAP               (0xDD24)
#define MRC_MC_MEMORY_MAP_ERROR         (MRC_MC_MEMORY_MAP | MRC_FAILURE_INDICATION)

#define MRC_JEDEC_INIT_LPDDR3           (0xDD25)
#define MRC_JEDEC_INIT_LPDDR3_ERROR     (MRC_JEDEC_INIT_LPDDR3 | MRC_FAILURE_INDICATION)

#define MRC_RESET_SEQUENCE              (0xDD26)
#define MRC_RESET_ERROR                 (MRC_RESET_SEQUENCE | MRC_FAILURE_INDICATION)

#define MRC_PRE_TRAINING                (0xDD27)
#define MRC_PRE_TRAINING_ERROR          (MRC_PRE_TRAINING | MRC_FAILURE_INDICATION)

///
/// TRAINING STEPS START
///
#define MRC_EARLY_COMMAND               (0xDD28)
#define MRC_EARLY_COMMAND_ERROR         (MRC_EARLY_COMMAND | MRC_FAILURE_INDICATION)

#define MRC_SENSE_AMP_OFFSET            (0xDD29)
#define MRC_SENSE_AMP_OFFSET_ERROR      (MRC_SENSE_AMP_OFFSET | MRC_FAILURE_INDICATION)

#define MRC_READ_MPR                    (0xDD2A)
#define MRC_READ_MPR_ERROR              (MRC_READ_MPR | MRC_FAILURE_INDICATION)

#define MRC_RECEIVE_ENABLE              (0xDD2B)
#define MRC_RECEIVE_ENABLE_ERROR        (MRC_RECEIVE_ENABLE | MRC_FAILURE_INDICATION)

#define MRC_JEDEC_WRITE_LEVELING        (0xDD2C)
#define MRC_JEDEC_WRITE_LEVELING_ERROR  (MRC_JEDEC_WRITE_LEVELING | MRC_FAILURE_INDICATION)

#define MRC_WRITE_TIMING_1D             (0xDD2E)
#define MRC_WRITE_TIMING_1D_ERROR       (MRC_WRITE_TIMING_1D | MRC_FAILURE_INDICATION)

#define MRC_READ_TIMING_1D              (0xDD2F)
#define MRC_READ_TIMING_1D_ERROR        (MRC_READ_TIMING_1D | MRC_FAILURE_INDICATION)

#define MRC_DIMM_ODT                    (0xDD30)
#define MRC_DIMM_ODT_ERROR              (MRC_DIMM_ODT | MRC_FAILURE_INDICATION)

#define MRC_WRITE_DS                    (0xDD32)
#define MRC_WRITE_DS_ERROR              (MRC_WRITE_DS | MRC_FAILURE_INDICATION)

#define MRC_WRITE_EQ                    (0xDD33)
#define MRC_WRITE_EQ_ERROR              (MRC_WRITE_EQ | MRC_FAILURE_INDICATION)

#define MRC_READ_ODT                    (0xDD35)
#define MRC_READ_ODT_ERROR              (MRC_READ_ODT | MRC_FAILURE_INDICATION)

#define MRC_READ_EQ                     (0xDD36)
#define MRC_READ_EQ_ERROR               (MRC_READ_EQ | MRC_FAILURE_INDICATION)

#define MRC_READ_AMP_POWER              (0xDD37)
#define MRC_READ_AMP_POWER_ERROR        (MRC_READ_AMP_POWER | MRC_FAILURE_INDICATION)

#define MRC_WRITE_TIMING_2D             (0xDD38)
#define MRC_WRITE_TIMING_2D_ERROR       (MRC_WRITE_TIMING_2D | MRC_FAILURE_INDICATION)

#define MRC_READ_TIMING_2D              (0xDD39)
#define MRC_READ_TIMING_2D_ERROR        (MRC_READ_TIMING_2D | MRC_FAILURE_INDICATION)

#define MRC_CMD_VREF                    (0xDD3A)
#define MRC_CMD_VREF_ERROR              (MRC_CMD_VREF_2D | MRC_FAILURE_INDICATION)

#define MRC_WRITE_VREF_2D               (0xDD3B)
#define MRC_WRITE_VREF_2D_ERROR         (MRC_WRITE_VREF_2D | MRC_FAILURE_INDICATION)

#define MRC_READ_VREF_2D                (0xDD3C)
#define MRC_READ_VREF_2D_ERROR          (MRC_READ_VREF_2D | MRC_FAILURE_INDICATION)

#define MRC_POST_TRAINING               (0xDD3D)
#define MRC_POST_TRAINING_ERROR         (MRC_POST_TRAINING | MRC_FAILURE_INDICATION)

#define MRC_LATE_COMMAND                (0xDD3E)
#define MRC_LATE_COMMAND_ERROR          (MRC_LATE_COMMAND | MRC_FAILURE_INDICATION)

#define MRC_ROUND_TRIP_LAT              (0xDD3F)
#define MRC_ROUND_TRIP_LAT_ERROR        (MRC_ROUND_TRIP_LAT | MRC_FAILURE_INDICATION)

#define MRC_TURN_AROUND                 (0xDD40)
#define MRC_TURN_AROUND_ERROR           (MRC_TURN_AROUND | MRC_FAILURE_INDICATION)

#define MRC_CMP_OPT                     (0xDD41)
#define MRC_CMP_OPT_ERROR               (MRC_CMP_OPT | MRC_FAILURE_INDICATION)

#define MRC_SAVE_MC_VALUES              (0xDD42)
#define MRC_SAVE_MC_VALUES_ERROR        (MRC_SAVE_MC_VALUES | MRC_FAILURE_INDICATION)

#define MRC_RESTORE_TRAINING            (0xDD43)
#define MRC_RESTORE_TRAINING_ERROR      (MRC_RESTORE_TRAINING | MRC_FAILURE_INDICATION)

#define MRC_RMT_TOOL                    (0xDD44)
#define MRC_RMT_TOOL_ERROR              (MRC_RMT_TOOL | MRC_FAILURE_INDICATION)

#define MRC_WRITE_SR                    (0xDD45)
#define MRC_WRITE_SR_ERROR              (MRC_WRITE_SR | MRC_FAILURE_INDICATION)

#define MRC_DIMM_RON                    (0xDD46)
#define MRC_DIMM_RON_ERROR              (MRC_DIMM_RON | MRC_FAILURE_INDICATION)

#define MRC_RCVEN_TIMING_1D             (0xDD47)
#define MRC_RCVEN_TIMING_1D_ERROR       (MRC_RCVEN_TIMING_1D | MRC_FAILURE_INDICATION)

#define MRC_PWR_MTR                     (0xDD49)
#define MRC_PWR_MTR_ERROR               (MRC_PWR_MTR | MRC_FAILURE_INDICATION)

#define MRC_MC_ACTIVATE                 (0xDD50)
#define MRC_MC_ACTIVATE_ERROR           (MRC_MC_ACTIVATE | MRC_FAILURE_INDICATION)

#define MRC_GET_MRC_DATA                (0xDD52)
#define MRC_GET_MRC_DATA_ERROR          (MRC_GET_MRC_DATA | MRC_FAILURE_INDICATION)


///
/// To have distinct post codes for debuggin purposes, do not define a training step
/// value to have the same lower byte value as MRC_MEM_INIT_DONE.  This value is
/// specific to signal the completion of the module.  This holds higher signifiance
/// on systems that do not use the high byte.
///
/// #define MRC_DO_NOT_USE                (0x__55)
///

#define MRC_RETRAIN_CHECK               (0xDD58)
#define MRC_RETRAIN_CHECK_ERROR         (MRC_RETRAIN_CHECK | MRC_FAILURE_INDICATION)

#define MRC_INIT_IO_DEFAULT             (0xDD59)
#define MRC_INIT_IO_DEFAULT_ERROR       (MRC_INIT_IO_DEFAULT | MRC_FAILURE_INDICATION)

#define MRC_ALIAS_CHECK                 (0xDD5B)
#define MRC_ALIAS_CHECK_ERROR           (MRC_ALIAS_CHECK | MRC_FAILURE_INDICATION)

#define MRC_ECC_CLEAN_START             (0xDD5C)
#define MRC_ECC_CLEAN_ERROR             (MRC_ECC_CLEAN_START | MRC_FAILURE_INDICATION)

#define MRC_DONE                        (0xDD5D)
#define MRC_DONE_WITH_ERROR             (MRC_DONE | MRC_FAILURE_INDICATION)

#define MRC_CPGC_MEMORY_TEST            (0xDD5F)
#define MRC_CPGC_MEMORY_TEST_ERROR      (MRC_CPGC_MEMORY_TEST | MRC_FAILURE_INDICATION)

#define MRC_MEMORY_TEST                 (0xDD68)
#define MRC_MEMORY_TEST_ERROR           (MRC_MEMORY_TEST | MRC_FAILURE_INDICATION)

#define MRC_FILL_RMT_STRUCTURE          (0xDD69)
#define MRC_FILL_RMT_STRUCTURE_ERROR    (MRC_FILL_RMT_STRUCTURE | MRC_FAILURE_INDICATION)

#define MRC_SELF_REFRESH_EXIT           (0xDD70)
#define MRC_SELF_REFRESH_EXIT_ERROR     (MRC_SELF_REFRESH_EXIT | MRC_FAILURE_INDICATION)

#define MRC_NORMAL_MODE                 (0xDD71)
#define MRC_MRC_NORMAL_MODE_ERROR       (MRC_NORMAL_MODE | MRC_FAILURE_INDICATION)

#define MRC_NO_MEMORY_DETECTED          (0xDD7E | MRC_FAILURE_INDICATION)

#define MRC_MEM_INIT_DONE               (0x0055)
#define MRC_MEM_INIT_DONE_WITH_ERRORS   (MRC_MEM_INIT_DONE | MRC_FAILURE_INDICATION)

#endif
