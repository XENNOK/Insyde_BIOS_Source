/** @file

  Definitions for HECI SMM driver

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SpsHeciSmm.h"
#include "SpsHeciHpet.h"

#include <SpsHeciRegs.h>
#include <SpsChipset.h>

///
/// Extern for shared HECI data and protocols
///
extern HECI_INSTANCE_SMM     *mSmmHeciContext;
volatile UINT32              mSaveHpetConfigReg;

/**
  Enable Hpet function.

  @param[in] None.

  @retval None
**/
VOID
EnableHpet (
  VOID
  )
{
  volatile UINT32 *HpetConfigReg;

  HpetConfigReg = NULL;
  ///
  /// Get the High Precision Event Timer base address and enable the memory range
  ///
  HpetConfigReg = (UINT32 *) (UINTN) (SPS_RCRB_BASE + R_SPS_RCRB_HPTC);
  switch (*HpetConfigReg & B_SPS_RCRB_HPTC_AS) {

  case 0:
    mSmmHeciContext->HpetTimer = (VOID *) (UINTN) (SPS_HPET_ADDRESS_0);
    break;

  case 1:
    mSmmHeciContext->HpetTimer = (VOID *) (UINTN) (SPS_HPET_ADDRESS_1);
    break;

  case 2:
    mSmmHeciContext->HpetTimer = (VOID *) (UINTN) (SPS_HPET_ADDRESS_2);
    break;

  case 3:
    mSmmHeciContext->HpetTimer = (VOID *) (UINTN) (SPS_HPET_ADDRESS_3);
    break;

  default:
    mSmmHeciContext->HpetTimer = NULL;
    break;
  }
  ///
  /// Read this back to force the write-back.
  ///
  *HpetConfigReg = *HpetConfigReg | B_SPS_RCRB_HPTC_AE;

  ///
  /// Start the timer so it is up and running
  ///
  mSmmHeciContext->HpetTimer[SPS_HPET_GEN_CONFIG_LOW]  = SPS_HPET_START;
  mSmmHeciContext->HpetTimer[SPS_HPET_GEN_CONFIG_LOW]  = SPS_HPET_START;

  return ;
}

/**
  Store the value of High Performance Timer

  @param[in] None

  @retval None
**/
VOID
SaveHpet (
  VOID
  )
{
  mSaveHpetConfigReg = MmioRead32 (SPS_RCRB_BASE + R_SPS_RCRB_HPTC);
}

/**
  Restore the value of High Performance Timer

  @param[in] None

  @retval None
**/
VOID
RestoreHpet (
  VOID
  )
{
  MmioWrite32 (SPS_RCRB_BASE + R_SPS_RCRB_HPTC, mSaveHpetConfigReg);
}

/**
  Used for calculating timeouts

  @param[out] Start               Snapshot of the HPET timer
  @param[out] End                 Calculated time when timeout period will be done
  @param[in] Time                 Timeout period in microseconds

  @retval None
**/
VOID
StartTimer (
  OUT UINT32                      *Start,
  OUT UINT32                      *End,
  IN  UINT32                      Time
  )
{
  UINT32  Ticks;

  ///
  /// Make sure that HPET is enabled and running
  ///
  EnableHpet ();

  ///
  /// Read current timer value into start time from HPET
  ///
  *Start = mSmmHeciContext->HpetTimer[SPS_HPET_MAIN_COUNTER_LOW];

  ///
  /// Convert microseconds into 70ns timer ticks
  ///
  Ticks = Time * SPS_HPET_TICKS_PER_MICRO;

  ///
  /// Compute end time
  ///
  *End = *Start + Ticks;

  return ;
}

/**
  Used to determine if a timeout has occured.

  @param[in] Start                Snapshot of the HPET timer when the timeout period started.
  @param[in] End                  Calculated time when timeout period will be done.

  @retval EFI_TIMEOUT             Timeout occured.
  @retval EFI_SUCCESS             Not yet timed out
**/
EFI_STATUS
Timeout (
  IN UINT32                            Start,
  IN UINT32                            End
  )
{
  UINT32  Current;

  ///
  /// Read HPET and assign the value as the current time.
  ///
  Current = mSmmHeciContext->HpetTimer[SPS_HPET_MAIN_COUNTER_LOW];

  ///
  /// Test basic case (no overflow)
  ///
  if ((Start < End) && (End <= Current)) {
    return EFI_TIMEOUT;
  }
  ///
  /// Test basic start/end conditions with overflowed timer
  ///
  if ((Start < End) && (Current < Start)) {
    return EFI_TIMEOUT;
  }
  ///
  /// Test for overflowed start/end condition
  ///
  if ((Start > End) && ((Current < Start) && (Current > End))) {
    return EFI_TIMEOUT;
  }
  ///
  /// Catch corner case of broken arguments
  ///
  if (Start == End) {
    return EFI_TIMEOUT;
  }
  ///
  /// Else, we have not yet timed out
  ///
  return EFI_SUCCESS;
}

/**
  Delay for at least the request number of microseconds

  @param[in] delayTime            Number of microseconds to delay.

  @retval None
**/
VOID
IoDelay (
  IN UINT32                       delayTime
  )
{
  MicroSecondDelay ((UINTN)delayTime);
}
