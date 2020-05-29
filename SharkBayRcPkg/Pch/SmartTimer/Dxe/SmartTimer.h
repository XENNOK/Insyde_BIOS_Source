/** @file
  Private data structures

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

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SmartTimer.h

@brief
  Private data structures

**/
#ifndef _SMARTTIMER_H_
#define _SMARTTIMER_H_

#include <PchAccess.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
///
/// Consumed Protocols
///
#include <Protocol/Cpu.h>
#include <Protocol/Legacy8259.h>
///
/// Produced Protocols
///
#include <Protocol/Timer.h>
///
/// The PCAT 8253/8254 has an input clock at 1.193182 MHz and Timer 0 is
/// initialized as a 16 bit free running counter that generates an interrupt(IRQ0)
/// each time the counter rolls over.
///
///   65536 counts
/// ---------------- * 1,000,000 uS/S = 54925.4 uS = 549254 * 100 ns
///   1,193,182 Hz
///
#define DEFAULT_TIMER_TICK_DURATION 549254
#define TIMER_CONTROL_PORT          0x43
#define TIMER0_COUNT_PORT           0x40

//
// Function Prototypes
//

/**
  Initialize the Timer Architectural Protocol driver

  @param[in] ImageHandle          ImageHandle of the loaded driver
  @param[in] SystemTable          Pointer to the System Table

  @retval EFI_SUCCESS             Timer Architectural Protocol created
  @retval Other                   Failed
**/
EFI_STATUS
EFIAPI
TimerDriverInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
;

/**
  This function registers the handler NotifyFunction so it is called every time
  the timer interrupt fires.  It also passes the amount of time since the last
  handler call to the NotifyFunction.  If NotifyFunction is NULL, then the
  handler is unregistered.  If the handler is registered, then EFI_SUCCESS is
  returned.  If the CPU does not support registering a timer interrupt handler,
  then EFI_UNSUPPORTED is returned.  If an attempt is made to register a handler
  when a handler is already registered, then EFI_ALREADY_STARTED is returned.
  If an attempt is made to unregister a handler when a handler is not registered,
  then EFI_INVALID_PARAMETER is returned.  If an error occurs attempting to
  register the NotifyFunction with the timer interrupt, then EFI_DEVICE_ERROR
  is returned.

  @param[in] This                 The EFI_TIMER_ARCH_PROTOCOL instance.
  @param[in] NotifyFunction       The function to call when a timer interrupt fires.  This
                                  function executes at TPL_HIGH_LEVEL.  The DXE Core will
                                  register a handler for the timer interrupt, so it can know
                                  how much time has passed.  This information is used to
                                  signal timer based events.  NULL will unregister the handler.

  @retval EFI_SUCCESS             The timer handler was registered.
  @exception EFI_UNSUPPORTED      The CPU does not support registering a timer interrupt handler
  @retval EFI_ALREADY_STARTED     NotifyFunction is not NULL, and a handler is already registered.
  @retval EFI_INVALID_PARAMETER   NotifyFunction is NULL, and a handler was not previously registered.
**/
EFI_STATUS
EFIAPI
TimerDriverRegisterHandler (
  IN EFI_TIMER_ARCH_PROTOCOL  *This,
  IN EFI_TIMER_NOTIFY         NotifyFunction
  );

/**
  This function adjusts the period of timer interrupts to the value specified
  by TimerPeriod.  If the timer period is updated, then the selected timer
  period is stored in EFI_TIMER.TimerPeriod, and EFI_SUCCESS is returned.  If
  the timer hardware is not programmable, then EFI_UNSUPPORTED is returned.
  If an error occurs while attempting to update the timer period, then the
  timer hardware will be put back in its state prior to this call, and
  EFI_DEVICE_ERROR is returned.  If TimerPeriod is 0, then the timer interrupt
  is disabled.  This is not the same as disabling the CPU's interrupts.
  Instead, it must either turn off the timer hardware, or it must adjust the
  interrupt controller so that a CPU interrupt is not generated when the timer
  interrupt fires.

  @param[in] This                 The EFI_TIMER_ARCH_PROTOCOL instance.
  @param[in] TimerPeriod          The rate to program the timer interrupt in 100 nS units.  If
                                  the timer hardware is not programmable, then EFI_UNSUPPORTED is
                                  returned. If the timer is programmable, then the timer period
                                  will be rounded up to the nearest timer period that is supported
                                  by the timer hardware.  If TimerPeriod is set to 0, then the
                                  timer interrupts will be disabled.

  @retval EFI_SUCCESS             The timer period was changed.
**/
EFI_STATUS
EFIAPI
TimerDriverSetTimerPeriod (
  IN EFI_TIMER_ARCH_PROTOCOL  *This,
  IN UINT64                   TimerPeriod
  );

/**
  This function retrieves the period of timer interrupts in 100 ns units,
  returns that value in TimerPeriod, and returns EFI_SUCCESS.  If TimerPeriod
  is NULL, then EFI_INVALID_PARAMETER is returned.  If a TimerPeriod of 0 is
  returned, then the timer is currently disabled.

  @param[in] This                 The EFI_TIMER_ARCH_PROTOCOL instance.
  @param[out] TimerPeriod         A pointer to the timer period to retrieve in 100 ns units.
                                  If 0 is returned, then the timer is currently disabled.

  @retval EFI_SUCCESS             The timer period was returned in TimerPeriod.
  @retval EFI_INVALID_PARAMETER   TimerPeriod is NULL.
**/
EFI_STATUS
EFIAPI
TimerDriverGetTimerPeriod (
  IN EFI_TIMER_ARCH_PROTOCOL   *This,
  OUT UINT64                   *TimerPeriod
  );

/**
  This function generates a soft timer interrupt. If the platform does not support soft
  timer interrupts, then EFI_UNSUPPORTED is returned. Otherwise, EFI_SUCCESS is returned.
  If a handler has been registered through the EFI_TIMER_ARCH_PROTOCOL.RegisterHandler()
  service, then a soft timer interrupt will be generated. If the timer interrupt is
  enabled when this service is called, then the registered handler will be invoked. The
  registered handler should not be able to distinguish a hardware-generated timer
  interrupt from a software-generated timer interrupt.

  @param[in] This                 The EFI_TIMER_ARCH_PROTOCOL instance.

  @retval EFI_SUCCESS             The soft timer interrupt was generated.
**/
EFI_STATUS
EFIAPI
TimerDriverGenerateSoftInterrupt (
  IN EFI_TIMER_ARCH_PROTOCOL  *This
  );

#endif
