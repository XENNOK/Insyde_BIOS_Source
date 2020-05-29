/** @file

  Definitions for HECI driver

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "HeciDxe.h"
#include "HeciHpet.h"

//
// Extern for shared HECI data and protocols
//
extern HECI_INSTANCE  *mHeciContext;
volatile UINT32       mSaveHpetConfigReg;

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
  mSaveHpetConfigReg = MmioRead32 (PCH_RCRB_BASE + R_PCH_RCRB_HPTC);
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
  MmioWrite32 (PCH_RCRB_BASE + R_PCH_RCRB_HPTC, mSaveHpetConfigReg);
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
  *Start = mHeciContext->HpetTimer[HPET_MAIN_COUNTER_LOW];

  ///
  /// Convert microseconds into 70ns timer ticks
  ///
  Ticks = Time * HPET_TICKS_PER_MICRO;

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
  IN  UINT32                      Start,
  IN  UINT32                      End
  )
{
  UINT32  Current;

  ///
  /// Read HPET and assign the value as the current time.
  ///
  Current = mHeciContext->HpetTimer[HPET_MAIN_COUNTER_LOW];

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
  gBS->Stall (delayTime);
}
