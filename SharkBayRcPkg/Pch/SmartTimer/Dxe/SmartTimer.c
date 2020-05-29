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
  SmartTimer.c

@brief
  Timer Architectural Protocol as defined in the DXE CIS

**/
#include "SmartTimer.h"

///
/// The handle onto which the Timer Architectural Protocol will be installed
///
EFI_HANDLE                mTimerHandle = NULL;

///
/// The Timer Architectural Protocol that this driver produces
///
EFI_TIMER_ARCH_PROTOCOL   mTimer = {
  TimerDriverRegisterHandler,
  TimerDriverSetTimerPeriod,
  TimerDriverGetTimerPeriod,
  TimerDriverGenerateSoftInterrupt
};

///
/// Pointer to the CPU Architectural Protocol instance
///
EFI_CPU_ARCH_PROTOCOL     *mCpu;

///
/// Pointer to the Legacy 8259 Protocol instance
///
EFI_LEGACY_8259_PROTOCOL  *mLegacy8259;

///
/// The notification function to call on every timer interrupt.
/// A bug in the compiler prevents us from initializing this here.
///
volatile EFI_TIMER_NOTIFY mTimerNotifyFunction;

///
/// The current period of the timer interrupt
///
volatile UINT64           mTimerPeriod = 0;

///
/// The time of twice timer interrupt duration
///
volatile UINTN            mPreAcpiTick = 0;

///
/// PMIO BAR Registers
///
UINT16                    mPchPmioBase;

//
// Worker Functions
//

/**
  Sets the counter value for Timer #0 in a legacy 8254 timer.

  @param[in] Count                The 16-bit counter value to program into Timer #0 of the legacy 8254 timer.

  @retval None
**/
VOID
SetPitCount (
  IN UINT16  Count
  )
{
  UINT8 Data;
  ///
  /// 0x36 = Read/Write counter LSB then MSB, Mode3 square wave output from this timer.
  /// Check register Counter Access Ports Register(0x40/41/42 for counter0/1/2) in PCH B0D31F0
  /// check Counter Operating Mode 0~5 at 8254 Timer function description in LPC in EDS.
  ///
  Data = 0x36;
  IoWrite8 (TIMER_CONTROL_PORT, Data);
  IoWrite8 (TIMER0_COUNT_PORT, (UINT8) Count);
  IoWrite8 (TIMER0_COUNT_PORT, (UINT8) (Count >> 8));
}

/**
  Get the current ACPI counter's value

  @param[in] None

  @retval UINT32                  The value of the counter
**/
UINT32
GetAcpiTick (
  VOID
  )
{
  UINT32  Tick;

  Tick = IoRead32 ((UINTN) (mPchPmioBase + R_PCH_ACPI_PM1_TMR));
  return Tick;

}

/**
  Measure the 8254 timer interrupt use the ACPI time counter

  @param[in] TimePeriod           The current period of the timer interrupt

  @retval UINT64                  The real system time pass between the sequence 8254 timer
                                  interrupt
**/
UINT64
MeasureTimeLost (
  IN UINT64             TimePeriod
  )
{
  UINT32  CurrentTick;
  UINT32  EndTick;
  UINT64  LostTime;

  CurrentTick = GetAcpiTick ();
  EndTick     = CurrentTick;

  if (CurrentTick < mPreAcpiTick) {
    EndTick = CurrentTick + 0x1000000;
  }
  ///
  /// The calculation of the lost system time should be very accurate, we use
  /// the shift calcu to make sure the value's accurate:
  /// the origenal formula is:
  ///                      (EndTick - mPreAcpiTick) * 10,000,000
  ///      LostTime = -----------------------------------------------
  ///                   (3,579,545 Hz / 1,193,182 Hz) * 1,193,182 Hz
  ///
  /// Note: the 3,579,545 Hz is the ACPI timer's clock;
  ///       the 1,193,182 Hz is the 8254 timer's clock;
  ///
  LostTime = RShiftU64 (
              MultU64x32 ((UINT64) (EndTick - mPreAcpiTick),
              46869689) + 0x00FFFFFF,
              24
              );

  if (LostTime != 0) {
    mPreAcpiTick = CurrentTick;
  }

  return LostTime;
}

/**
  8254 Timer #0 Interrupt Handler

  @param[in] InterruptType        The type of interrupt that occured
  @param[in] SystemContext        A pointer to the system context when the interrupt occured

  @retval None
**/
VOID
EFIAPI
TimerInterruptHandler (
  IN EFI_EXCEPTION_TYPE   InterruptType,
  IN EFI_SYSTEM_CONTEXT   SystemContext
  )
{
  EFI_TPL OriginalTPL;

  OriginalTPL = gBS->RaiseTPL (TPL_HIGH_LEVEL);

  mLegacy8259->EndOfInterrupt (mLegacy8259, Efi8259Irq0);

  if (mTimerNotifyFunction) {
    ///
    /// If we have the timer interrupt miss, then we use
    /// the platform ACPI time counter to retrieve the time lost
    ///
    mTimerNotifyFunction (MeasureTimeLost (mTimerPeriod));
  }

  gBS->RestoreTPL (OriginalTPL);
}

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
  )
{
  ///
  /// If an attempt is made to unregister a handler when a handler is not registered,
  /// then EFI_INVALID_PARAMETER is returned.
  ///
  if (mTimerNotifyFunction == NULL && NotifyFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// If an attempt is made to register a handler
  /// when a handler is already registered, then EFI_ALREADY_STARTED is returned.
  ///
  if (mTimerNotifyFunction != NULL && NotifyFunction != NULL) {
    return EFI_ALREADY_STARTED;
  }
  ///
  /// If the CPU does not support registering a timer interrupt handler, then EFI_UNSUPPORTED is returned.
  ///
  if (mCpu == NULL || mLegacy8259 == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (NotifyFunction == NULL) {
    ///
    /// If NotifyFunction is NULL, then the handler is unregistered.
    ///
    mTimerNotifyFunction = NULL;
  } else {
    mTimerNotifyFunction = NotifyFunction;
  }

  return EFI_SUCCESS;
}

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
  )
{
  UINT64  TimerCount;

  ///
  ///  The basic clock is 1.19318 MHz or 0.119318 ticks per 100 ns.
  ///  TimerPeriod * 0.119318 = 8254 timer divisor. Using integer arithmetic
  ///  TimerCount = (TimerPeriod * 119318)/1000000.
  ///
  ///  Round up to next highest integer. This guarantees that the timer is
  ///  equal to or slightly longer than the requested time.
  ///  TimerCount = ((TimerPeriod * 119318) + 500000)/1000000
  ///
  /// Note that a TimerCount of 0 is equivalent to a count of 65,536
  ///
  /// Since TimerCount is limited to 16 bits for IA32, TimerPeriod is limited
  /// to 20 bits.
  ///
  if (TimerPeriod == 0) {
    ///
    /// Disable timer interrupt for a TimerPeriod of 0
    ///
    mLegacy8259->DisableIrq (mLegacy8259, Efi8259Irq0);
  } else {
    ///
    /// Convert TimerPeriod into 8254 counts
    ///
    TimerCount = DivU64x32Remainder (MultU64x32 (119318, (UINT32) TimerPeriod) + 500000, 1000000, 0);

    ///
    /// Check for overflow
    ///
    if (TimerCount >= 65536) {
      TimerCount = 0;
      if (TimerPeriod >= DEFAULT_TIMER_TICK_DURATION) {
        TimerPeriod = DEFAULT_TIMER_TICK_DURATION;
      }
    }
    ///
    /// Program the 8254 timer with the new count value
    ///
    SetPitCount ((UINT16) TimerCount);

    ///
    /// Enable timer interrupt
    ///
    mLegacy8259->EnableIrq (mLegacy8259, Efi8259Irq0, FALSE);
  }
  ///
  /// Save the new timer period
  ///
  mTimerPeriod = TimerPeriod;

  return EFI_SUCCESS;
}

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
  )
{
  if (TimerPeriod == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *TimerPeriod = mTimerPeriod;

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS  Status;
  UINT16      IRQMask;
  EFI_TPL     OriginalTPL;

  DEBUG ((EFI_D_INFO, "TimerDriverGenerateSoftInterrupt() Start\n"));

  ///
  /// If the timer interrupt is enabled, then the registered handler will be invoked.
  ///
  Status = mLegacy8259->GetMask (mLegacy8259, NULL, NULL, &IRQMask, NULL);
  ASSERT_EFI_ERROR (Status);
  if ((IRQMask & 0x1) == 0) {
    ///
    /// Invoke the registered handler
    ///
    OriginalTPL = gBS->RaiseTPL (TPL_HIGH_LEVEL);

    if (mTimerNotifyFunction) {
      ///
      /// We use the platform ACPI time counter to determine
      /// the amount of time that has passed
      ///
      mTimerNotifyFunction (MeasureTimeLost (mTimerPeriod));
    }

    gBS->RestoreTPL (OriginalTPL);
  }

  DEBUG ((EFI_D_INFO, "TimerDriverGenerateSoftInterrupt() End\n"));

  return EFI_SUCCESS;
}

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
{
  EFI_STATUS  Status;
  UINT32      TimerVector;

  DEBUG ((EFI_D_INFO, "TimerDriverInitialize() Start\n"));

  ///
  /// Initialize the pointer to our notify function.
  ///
  mTimerNotifyFunction  = NULL;
  mCpu                  = NULL;
  mLegacy8259           = NULL;

  ///
  /// Make sure the Timer Architectural Protocol is not already installed in the system
  ///
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiTimerArchProtocolGuid);

  ///
  /// Find the CPU architectural protocol.  ASSERT if not found.
  ///
  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &mCpu);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Find the Legacy8259 protocol.  ASSERT if not found.
  ///
  Status = gBS->LocateProtocol (&gEfiLegacy8259ProtocolGuid, NULL, (VOID **) &mLegacy8259);
  ASSERT_EFI_ERROR (Status);

  mPchPmioBase = MmioRead16 (
                  MmPciAddress (0,
                  DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_ACPI_BASE)
                  ) & B_PCH_LPC_ACPI_BASE_BAR;
  ASSERT (mPchPmioBase != 0);
  ///
  /// Force the timer to be disabled
  ///
  Status = TimerDriverSetTimerPeriod (&mTimer, 0);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Get the interrupt vector number corresponding to IRQ0 from the 8259 driver
  ///
  TimerVector = 0;
  Status      = mLegacy8259->GetVector (mLegacy8259, Efi8259Irq0, (UINT8 *) &TimerVector);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Install interrupt handler for 8254 Timer #0 (ISA IRQ0)
  ///
  Status = mCpu->RegisterInterruptHandler (mCpu, TimerVector, TimerInterruptHandler);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Force the timer to be enabled at its default period
  ///
  Status = TimerDriverSetTimerPeriod (&mTimer, DEFAULT_TIMER_TICK_DURATION);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Begin the ACPI timer counter
  ///
  mPreAcpiTick = GetAcpiTick ();

  ///
  /// Install the Timer Architectural Protocol onto a new handle
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mTimerHandle,
                  &gEfiTimerArchProtocolGuid,
                  &mTimer,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "TimerDriverInitialize() End\n"));

  return Status;
}