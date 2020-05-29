/** @file

  Definitions for HECI driver

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

#ifndef _SPS_HECI_HPET_SMM_H
#define _SPS_HECI_HPET_SMM_H

/**
  Store the value of High Performance Timer

  @param[in] None

  @retval None
**/
VOID
SaveHpet (
  VOID
  );

/**
  Restore the value of High Performance Timer

  @param[in] None

  @retval None
**/
VOID
RestoreHpet (
  VOID
  );

/**
  Used for calculating timeouts

  @param[out] Start               Snapshot of the HPET timer
  @param[out] End                 Calculated time when timeout period will be done
  @param[in] Time                 Timeout period in microseconds

  @retval None
**/
VOID
StartTimer (
  OUT UINT32                           *Start,
  OUT UINT32                           *End,
  IN  UINT32                           Time
  );

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
  );

/**
  Delay for at least the request number of microseconds

  @param[in] delayTime            Number of microseconds to delay.

  @retval None
**/
VOID
IoDelay (
  IN UINT32                       delayTime
  );

#endif /// _HECI_HPET_H
