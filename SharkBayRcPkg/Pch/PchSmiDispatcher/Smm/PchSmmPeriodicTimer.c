/** @file
  File to contain all the hardware specific stuff for the Periodical Timer dispatch protocol.

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
  PchSmmPeriodicTimer.c

@brief
  File to contain all the hardware specific stuff for the Periodical Timer dispatch protocol.

**/
#include <PchSmmHelpers.h>

typedef enum {
  PERIODIC_TIMER= 0,
  SWSMI_TIMER,
  NUM_TIMERS
} SUPPORTED_TIMER;

typedef struct _TIMER_INTERVAL {
  UINT64  Interval;
  UINT8   AssociatedTimer;
} TIMER_INTERVAL;

#define NUM_INTERVALS 8

//
// Time constants, in 100 nano-second units
//
#define TIME_64s    640000000 ///< 64   s
#define TIME_32s    320000000 ///< 32   s
#define TIME_16s    160000000 ///< 16   s
#define TIME_8s     80000000  ///<  8   s
#define TIME_64ms   640000    ///< 64   ms
#define TIME_32ms   320000    ///< 32   ms
#define TIME_16ms   160000    ///< 16   ms
#define TIME_1_5ms  15000     ///< 1.5  ms

typedef enum {
  INDEX_TIME_64s  = 0,
  INDEX_TIME_32s,
  INDEX_TIME_16s,
  INDEX_TIME_8s,
  INDEX_TIME_64ms,
  INDEX_TIME_32ms,
  INDEX_TIME_16ms,
  INDEX_TIME_1_5ms,
  INDEX_TIME_MAX
} TIMER_INTERVAL_INDEX;

static TIMER_INTERVAL mSmmPeriodicTimerIntervals[NUM_INTERVALS] = {
  {
    TIME_64s,
    PERIODIC_TIMER
  },
  {
    TIME_32s,
    PERIODIC_TIMER
  },
  {
    TIME_16s,
    PERIODIC_TIMER
  },
  {
    TIME_8s,
    PERIODIC_TIMER
  },
  {
    TIME_64ms,
    SWSMI_TIMER
  },
  {
    TIME_32ms,
    SWSMI_TIMER
  },
  {
    TIME_16ms,
    SWSMI_TIMER
  },
  {
    TIME_1_5ms,
    SWSMI_TIMER
  },
};

typedef struct _TIMER_INFO {
  UINTN   NumChildren;        ///< number of children using this timer
  UINT64  MinReqInterval;     ///< minimum interval required by children
  UINTN   CurrentSetting;     ///< interval this timer is set at right now (index into interval table)
} TIMER_INFO;

TIMER_INFO          mTimers[NUM_TIMERS];

PCH_SMM_SOURCE_DESC mTIMER_SOURCE_DESCS[NUM_TIMERS] = {
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_PERIODIC
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_PERIODIC
      }
    }
  },
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_SWSMI_TMR
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_SWSMI_TMR
      }
    }
  }
};

VOID
PchSmmPeriodicTimerProgramTimers (
  VOID
  );

/**
  Convert the dispatch context to the timer interval, this function will assert if then either:
  (1) The context contains an invalid interval
  (2) The timer interval table is corrupt

  @param[in] DispatchContext      The pointer to the Dispatch Context

  @retval TIMER_INTERVAL          The timer interval of input dispatch context
**/
TIMER_INTERVAL *
ContextToTimerInterval (
  IN  PCH_SMM_CONTEXT     *DispatchContext
  )
{
  UINTN loopvar;

  ///
  /// Determine which timer this child is using
  ///
  for (loopvar = 0; loopvar < NUM_INTERVALS; loopvar++) {
    if (((DispatchContext->PeriodicTimer.SmiTickInterval == 0) &&
        (DispatchContext->PeriodicTimer.Period >= mSmmPeriodicTimerIntervals[loopvar].Interval)) ||
        (DispatchContext->PeriodicTimer.SmiTickInterval == mSmmPeriodicTimerIntervals[loopvar].Interval)) {
      return &mSmmPeriodicTimerIntervals[loopvar];
    }
  }
  ///
  /// If this assertion fires, then either:
  ///    (1) the context contains an invalid interval
  ///    (2) the timer interval table is corrupt
  ///
  ASSERT (FALSE);

  return NULL;
}

/**
  Figure out which timer the child is requesting and
  send back the source description

  @param[in] DispatchContext      The pointer to the Dispatch Context instances
  @param[out] SrcDesc             The pointer to the source description

  @retval None
**/
VOID
MapPeriodicTimerToSrcDesc (
  IN  PCH_SMM_CONTEXT             *DispatchContext,
  OUT PCH_SMM_SOURCE_DESC         *SrcDesc
  )
{
  TIMER_INTERVAL  *TimerInterval;

  ///
  /// Figure out which timer the child is requesting and
  /// send back the source description
  ///
  TimerInterval = ContextToTimerInterval (DispatchContext);
  if (TimerInterval == NULL) {
    return;
  }

  CopyMem (
    (VOID *) SrcDesc,
    (VOID *) (&mTIMER_SOURCE_DESCS[TimerInterval->AssociatedTimer]),
    sizeof (PCH_SMM_SOURCE_DESC)
    );

  ///
  /// Program the value of the interval into hardware
  ///
  PchSmmPeriodicTimerProgramTimers ();
}

/**
  Update the elapsed time from the Interval data of DATABASE_RECORD

  @param[in] Record               The pointer to the DATABASE_RECORD.
  @param[out] HwContext           The Context to be updated.

  @retval None
**/
VOID
EFIAPI
PeriodicTimerGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT PCH_SMM_CONTEXT    *HwContext
  )
{
  TIMER_INTERVAL  *TimerInterval;

  ASSERT (Record->ProtocolType == PeriodicTimerType);

  TimerInterval = ContextToTimerInterval (&Record->ChildContext);
  if (TimerInterval == NULL) {
    return;
  }
  ///
  /// Ignore the hardware context. It's not required for this protocol.
  /// Instead, just increment the child's context.
  /// Update the elapsed time w/ the data from our tables
  ///
  Record->ChildContext.PeriodicTimer.ElapsedTime += TimerInterval->Interval;
  *HwContext = Record->ChildContext;
}

/**
  Check whether Periodic Timer of two contexts match

  @param[in] Context1             Context 1 that includes Periodic Timer  1
  @param[in] Context2             Context 2 that includes Periodic Timer  2

  @retval FALSE                   Periodic Timer match
  @retval TRUE                    Periodic Timer don't match
**/
BOOLEAN
EFIAPI
PeriodicTimerCmpContext (
  IN PCH_SMM_CONTEXT     *HwContext,
  IN PCH_SMM_CONTEXT     *ChildContext
  )
{

  if (ChildContext->PeriodicTimer.ElapsedTime >= ChildContext->PeriodicTimer.Period) {
    ///
    /// This child should be dispatched
    /// Need reset ElapsedTime, or SMI handler will be invoked during SmiTickInterval instead of Period.
    ///
    ChildContext->PeriodicTimer.ElapsedTime = 0;
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Program Smm Periodic Timer

  @param[in] None

  @retval None
**/
VOID
PchSmmPeriodicTimerProgramTimers (
  VOID
  )
{
  UINT16          GenPmCon1;
  UINT8           GenPmCon3;
  SUPPORTED_TIMER Timer;
  DATABASE_RECORD *RecordInDb;
  LIST_ENTRY      *LinkInDb;
  UINTN           PciD31F0RegBase;
  TIMER_INTERVAL  *TimerInterval;

  PciD31F0RegBase = MmPciAddress (
                      0,
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      0
                      );
  ///
  /// Find the minimum required interval for each timer
  ///
  for (Timer = 0; Timer < NUM_TIMERS; Timer++) {
    mTimers[Timer].MinReqInterval = ~ (UINT64) 0x0;
    mTimers[Timer].NumChildren    = 0;
  }

  LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);
  while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
    RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);
    if (RecordInDb->ProtocolType == PeriodicTimerType) {
      ///
      /// This child is registerd with the PeriodicTimer protocol
      ///
      TimerInterval = ContextToTimerInterval (&RecordInDb->ChildContext);
      if (TimerInterval == NULL) {
        return;
      }

      Timer = TimerInterval->AssociatedTimer;
      if (Timer < 0 || Timer >= NUM_TIMERS) {
        ASSERT (FALSE);
        CpuDeadLoop ();
      }

      if (mTimers[Timer].MinReqInterval > RecordInDb->ChildContext.PeriodicTimer.SmiTickInterval) {
        mTimers[Timer].MinReqInterval = RecordInDb->ChildContext.PeriodicTimer.SmiTickInterval;
      }

      mTimers[Timer].NumChildren++;
    }

    LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);
  }
  ///
  /// Program the hardware
  ///
  if (mTimers[PERIODIC_TIMER].NumChildren > 0) {
    GenPmCon1 = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1);

    GenPmCon1 &= ~B_PCH_LPC_GEN_PMCON_PER_SMI_SEL;
    switch (mTimers[PERIODIC_TIMER].MinReqInterval) {
    case TIME_64s:
      GenPmCon1 |= V_PCH_LPC_GEN_PMCON_PER_SMI_64S;
      mTimers[PERIODIC_TIMER].CurrentSetting = INDEX_TIME_64s;
      break;

    case TIME_32s:
      GenPmCon1 |= V_PCH_LPC_GEN_PMCON_PER_SMI_32S;
      mTimers[PERIODIC_TIMER].CurrentSetting = INDEX_TIME_32s;
      break;

    case TIME_16s:
      GenPmCon1 |= V_PCH_LPC_GEN_PMCON_PER_SMI_16S;
      mTimers[PERIODIC_TIMER].CurrentSetting = INDEX_TIME_16s;
      break;

    case TIME_8s:
      GenPmCon1 |= V_PCH_LPC_GEN_PMCON_PER_SMI_8S;
      mTimers[PERIODIC_TIMER].CurrentSetting = INDEX_TIME_8s;
      break;

    default:
      ASSERT (FALSE);
      break;
    }

    MmioWrite16 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_1, GenPmCon1);

    ///
    /// Restart the timer here, just need to clear the SMI
    ///
    PchSmmClearSource (&mTIMER_SOURCE_DESCS[PERIODIC_TIMER]);
  } else {
    PchSmmDisableSource (&mTIMER_SOURCE_DESCS[PERIODIC_TIMER]);
  }

  if (mTimers[SWSMI_TIMER].NumChildren > 0) {
    ///
    /// ICH9, ICH10 and PCH share the same bit positions for SW SMI Rate settings
    ///
    GenPmCon3 = MmioRead8 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3);
    GenPmCon3 &= ~B_PCH_LPC_GEN_PMCON_SWSMI_RTSL;
    switch (mTimers[SWSMI_TIMER].MinReqInterval) {
    case TIME_64ms:
      GenPmCon3 |= V_PCH_LPC_GEN_PMCON_SWSMI_RTSL_64MS;
      mTimers[SWSMI_TIMER].CurrentSetting = INDEX_TIME_64ms;
      break;

    case TIME_32ms:
      GenPmCon3 |= V_PCH_LPC_GEN_PMCON_SWSMI_RTSL_32MS;
      mTimers[SWSMI_TIMER].CurrentSetting = INDEX_TIME_32ms;
      break;

    case TIME_16ms:
      GenPmCon3 |= V_PCH_LPC_GEN_PMCON_SWSMI_RTSL_16MS;
      mTimers[SWSMI_TIMER].CurrentSetting = INDEX_TIME_16ms;
      break;

    case TIME_1_5ms:
      GenPmCon3 |= V_PCH_LPC_GEN_PMCON_SWSMI_RTSL_1_5MS;
      mTimers[SWSMI_TIMER].CurrentSetting = INDEX_TIME_1_5ms;
      break;

    default:
      ASSERT (FALSE);
      break;
    }
    ///
    /// SWSMI_RATE_SEL BIT (D31:F0:A4h[7:6]) bits are in RTC well
    ///
    MmioWrite8 (PciD31F0RegBase + R_PCH_LPC_GEN_PMCON_3, GenPmCon3);

    ///
    /// Restart the timer here, need to disable, clear, then enable to restart this timer
    ///
    PchSmmDisableSource (&mTIMER_SOURCE_DESCS[SWSMI_TIMER]);
    PchSmmClearSource (&mTIMER_SOURCE_DESCS[SWSMI_TIMER]);
    PchSmmEnableSource (&mTIMER_SOURCE_DESCS[SWSMI_TIMER]);
  } else {
    PchSmmDisableSource (&mTIMER_SOURCE_DESCS[SWSMI_TIMER]);
  }
}

/**
  This services returns the next SMI tick period that is supported by the chipset.
  The order returned is from longest to shortest interval period.

  @param[in] This                 Pointer to the EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL instance.
  @param[in, out] SmiTickInterval Pointer to pointer of the next shorter SMI interval period that is supported by the child.

  @retval EFI_SUCCESS             The service returned successfully.
  @retval EFI_INVALID_PARAMETER   The parameter SmiTickInterval is invalid.
**/
EFI_STATUS
PchSmmPeriodicTimerDispatchGetNextShorterInterval (
  IN EFI_SMM_PERIODIC_TIMER_DISPATCH_PROTOCOL  *This,
  IN OUT UINT64                                **SmiTickInterval
  )
{
  TIMER_INTERVAL  *IntervalPointer;

  ASSERT (SmiTickInterval != NULL);

  IntervalPointer = (TIMER_INTERVAL *) *SmiTickInterval;

  if (IntervalPointer == NULL) {
    ///
    /// The first time child requesting an interval
    ///
    IntervalPointer = &mSmmPeriodicTimerIntervals[0];
  } else if (IntervalPointer == &mSmmPeriodicTimerIntervals[NUM_INTERVALS - 1]) {
    ///
    /// At end of the list
    ///
    IntervalPointer = NULL;
  } else {
    if ((IntervalPointer >= &mSmmPeriodicTimerIntervals[0]) &&
        (IntervalPointer < &mSmmPeriodicTimerIntervals[NUM_INTERVALS - 1])
        ) {
      ///
      /// Get the next interval in the list
      ///
      IntervalPointer++;
    } else {
      ///
      /// Input is out of range
      ///
      return EFI_INVALID_PARAMETER;
    }
  }

  if (IntervalPointer != NULL) {
    *SmiTickInterval = &IntervalPointer->Interval;
  } else {
    *SmiTickInterval = NULL;
  }

  return EFI_SUCCESS;
}

/**
  This function is responsible for calculating and enabling any timers that are required
  to dispatch messages to children. The SrcDesc argument isn't acutally used.

  @param[in] SrcDesc              Pointer to the PCH_SMM_SOURCE_DESC instance.

  @retval None.
**/
VOID
EFIAPI
PchSmmPeriodicTimerClearSource (
  IN PCH_SMM_SOURCE_DESC     *SrcDesc
  )
{
  PchSmmPeriodicTimerProgramTimers ();
}
