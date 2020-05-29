/** @file
  Implementation of PI MP Services Protocol.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "PiMpService.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_MP_SERVICES_PROTOCOL mPiMpService = {
  GetNumberOfProcessors,
  GetProcessorInfo,
  PiStartupAllAPs,
  PiStartupThisAP,
  PiSwitchBSP,
  PiEnableDisableAP,
  PiWhoAmI
};

BOOLEAN mStopCheckAPsStatus = FALSE;

/**
  Abort any task on the AP and reset the AP to be in idle state.

  @param[in] ProcessorNumber - Processor index of an AP.
**/
VOID
ResetProcessorToIdleState (
  UINTN ProcessorNumber
  )
{
  CPU_DATA_BLOCK *CpuData;

  CpuData = &mMPSystemData->CpuData[ProcessorNumber];

  SendIPI (
          &mMpService,
          ProcessorNumber,
          0,
          DELIVERY_MODE_INIT
          );

  AsmAcquireMPLock (&CpuData->StateLock);
  CpuData->State = CPU_STATE_IDLE;
  AsmReleaseMPLock (&CpuData->StateLock);
}

/**
  Searches for the next waiting AP.

  Search for the next AP that is put in waiting state by single-threaded StartupAllAPs().

  @param[in] NextProcessorNumber  - Pointer to the processor number of the next waiting AP.

  @retval EFI_SUCCESS          - The next waiting AP has been found.
  @retval EFI_NOT_FOUND        - No waiting AP exists.
**/
EFI_STATUS
GetNextWaitingProcessorNumber (
  OUT UINTN *NextProcessorNumber
  )
{
  UINTN ProcessorNumber;

  for (ProcessorNumber = 0; ProcessorNumber < mMPSystemData->NumberOfCpus; ProcessorNumber++) {

    if (mMPSystemData->CpuList[ProcessorNumber]) {
      *NextProcessorNumber = ProcessorNumber;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Checks status of all APs.

  This function checks whether all APs have finished task assigned by StartupAllAPs(),
  and whether timeout expires.

  @retval EFI_SUCCESS    - All APs have finished task assigned by StartupAllAPs().
  @retval EFI_TIMEOUT    - The timeout expires.
  @retval EFI_NOT_READY  - APs have not finished task and timeout has not expired.
**/
EFI_STATUS
CheckAllAPs (
  VOID
  )
{
  UINTN          ProcessorNumber;
  UINTN          NextProcessorNumber;
  UINTN          ListIndex;
  EFI_STATUS     Status;
  CPU_STATE      CpuState;
  CPU_DATA_BLOCK *CpuData;

  NextProcessorNumber = 0;

  ///
  /// Go through all APs that are responsible for the StartupAllAPs().
  ///
  for (ProcessorNumber = 0; ProcessorNumber < mMPSystemData->NumberOfCpus; ProcessorNumber++) {
    if (!mMPSystemData->CpuList[ProcessorNumber]) {
      continue;
    }

    CpuData = &mMPSystemData->CpuData[ProcessorNumber];

    ///
    ///  Check the CPU state of AP. If it is CPU_STATE_FINISHED, then the AP has finished its task.
    ///  Only BSP and corresponding AP access this unit of CPU Data. This means the AP will not modify the
    ///  value of state after setting the it to CPU_STATE_FINISHED, so BSP can safely make use of its value.
    ///
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuState = CpuData->State;
    AsmReleaseMPLock (&CpuData->StateLock);

    if (CpuState == CPU_STATE_FINISHED) {
      mMPSystemData->FinishCount++;
      mMPSystemData->CpuList[ProcessorNumber] = FALSE;

      AsmAcquireMPLock (&CpuData->StateLock);
      CpuData->State = CPU_STATE_IDLE;
      AsmReleaseMPLock (&CpuData->StateLock);

      ///
      /// If in Single Thread mode, then search for the next waiting AP for execution.
      ///
      if (mMPSystemData->SingleThread) {
        Status = GetNextWaitingProcessorNumber (&NextProcessorNumber);

        if (!EFI_ERROR (Status)) {
          WakeUpAp (
                  &mMPSystemData->CpuData[NextProcessorNumber],
                  mMPSystemData->Procedure,
                  mMPSystemData->ProcArguments
                  );
        }
      }
    }
  }
  ///
  /// If all APs finish, return EFI_SUCCESS.
  ///
  if (mMPSystemData->FinishCount == mMPSystemData->StartCount) {
    return EFI_SUCCESS;
  }
  ///
  /// If timeout expires, report timeout.
  ///
  if (CheckTimeout (mMPSystemData->ExpectedTime)) {
    ///
    /// If FailedCpuList is not NULL, record all failed APs in it.
    ///
    if (mMPSystemData->FailedCpuList != NULL) {
      Status = (gBS->AllocatePool)
              (
                      EfiBootServicesData,
                      ((mMPSystemData->StartCount - mMPSystemData->FinishCount + 1) * sizeof (UINTN)),
                      (VOID **) mMPSystemData->FailedCpuList
              );
      ASSERT_EFI_ERROR (Status);
    }

    ListIndex = 0;

    for (ProcessorNumber = 0; ProcessorNumber < mMPSystemData->NumberOfCpus; ProcessorNumber++) {
      ///
      /// Check whether this processor is responsible for StartupAllAPs().
      ///
      if (mMPSystemData->CpuList[ProcessorNumber]) {
        ///
        /// Reset failed APs to idle state
        ///
        ResetProcessorToIdleState (ProcessorNumber);
        mMPSystemData->CpuList[ProcessorNumber] = FALSE;
        if (mMPSystemData->FailedCpuList != NULL) {
          (*mMPSystemData->FailedCpuList)[ListIndex++] = ProcessorNumber;
        }
      }
    }

    if (mMPSystemData->FailedCpuList != NULL) {
      (*mMPSystemData->FailedCpuList)[ListIndex] = END_OF_CPU_LIST;
    }

    return EFI_TIMEOUT;
  }

  return EFI_NOT_READY;
}

/**
  Checks status of specified AP.

  This function checks whether specified AP has finished task assigned by StartupThisAP(),
  and whether timeout expires.

  @param[in] ProcessorNumber - The handle number of processor.

  @retval EFI_SUCCESS     - Specified AP has finished task assigned by StartupThisAPs().
  @retval EFI_TIMEOUT     - The timeout expires.
  @retval EFI_NOT_READY   - Specified AP has not finished task and timeout has not expired.
**/
EFI_STATUS
CheckThisAP (
  UINTN ProcessorNumber
  )
{
  CPU_DATA_BLOCK *CpuData;
  CPU_STATE      CpuState;

  CpuData = &mMPSystemData->CpuData[ProcessorNumber];

  ///
  ///  Check the CPU state of AP. If it is CPU_STATE_FINISHED, then the AP has finished its task.
  ///  Only BSP and corresponding AP access this unit of CPU Data. This means the AP will not modify the
  ///  value of state after setting the it to CPU_STATE_FINISHED, so BSP can safely make use of its value.
  ///
  AsmAcquireMPLock (&CpuData->StateLock);
  CpuState = CpuData->State;
  AsmReleaseMPLock (&CpuData->StateLock);

  ///
  /// If the APs finishes for StartupThisAP(), return EFI_SUCCESS.
  ///
  if (CpuState == CPU_STATE_FINISHED) {

    AsmAcquireMPLock (&CpuData->StateLock);
    CpuData->State = CPU_STATE_IDLE;
    AsmReleaseMPLock (&CpuData->StateLock);

    if (CpuData->Finished != NULL) {
      *(CpuData->Finished) = TRUE;
    }

    return EFI_SUCCESS;
  } else {
    ///
    /// If timeout expires for StartupThisAP(), report timeout.
    ///
    if (CheckTimeout (CpuData->ExpectedTime)) {

      if (CpuData->Finished != NULL) {
        *(CpuData->Finished) = FALSE;
      }
      ///
      /// Reset failed AP to idle state
      ///
      ResetProcessorToIdleState (ProcessorNumber);

      return EFI_TIMEOUT;
    }
  }

  return EFI_NOT_READY;
}

/**
  Checks APs' status periodically.

  This function is triggerred by timer perodically to check the
  state of APs for StartupAllAPs() and StartupThisAP() executed
  in non-blocking mode.

  @param[in] Event   - Event triggered.
  @param[in] Context - Parameter passed with the event.
**/
VOID
EFIAPI
CheckAPsStatus (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  UINTN          ProcessorNumber;
  CPU_DATA_BLOCK *CpuData;
  EFI_STATUS     Status;

  ///
  /// If CheckAPsStatus() is stopped, then return immediately.
  ///
  if (mStopCheckAPsStatus) {
    return;
  }
  ///
  /// First, check whether pending StartupAllAPs() exists.
  ///
  if (mMPSystemData->WaitEvent != NULL) {

    Status = CheckAllAPs ();
    ///
    /// If all APs finish for StartupAllAPs(), signal the WaitEvent for it..
    ///
    if (Status != EFI_NOT_READY) {
      Status                    = gBS->SignalEvent (mMPSystemData->WaitEvent);
      mMPSystemData->WaitEvent  = NULL;
    }
  }
  ///
  /// Second, check whether pending StartupThisAPs() callings exist.
  ///
  for (ProcessorNumber = 0; ProcessorNumber < mMPSystemData->NumberOfCpus; ProcessorNumber++) {

    CpuData = &mMPSystemData->CpuData[ProcessorNumber];

    if (CpuData->WaitEvent == NULL) {
      continue;
    }

    Status = CheckThisAP (ProcessorNumber);

    if (Status != EFI_NOT_READY) {
      gBS->SignalEvent (CpuData->WaitEvent);
      CpuData->WaitEvent = NULL;
    }
  }

  return;
}

/**
  Implementation of GetNumberOfProcessors() service of MP Services Protocol.

  This service retrieves the number of logical processor in the platform
  and the number of those logical processors that are enabled on this boot.
  This service may only be called from the BSP.

  @param[in] This                      - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] NumberOfProcessors        - Pointer to the total number of logical processors in the system,
                              including the BSP and disabled APs.
  @param[in] NumberOfEnabledProcessors - Pointer to the number of enabled logical processors that exist
                              in system, including the BSP.

  @retval EFI_SUCCESS            - Number of logical processors and enabled logical processors retrieved..
  @retval EFI_DEVICE_ERROR       - Caller processor is AP.
  @retval EFI_INVALID_PARAMETER  - NumberOfProcessors is NULL.
  @retval EFI_INVALID_PARAMETER  - NumberOfEnabledProcessors is NULL.
**/
EFI_STATUS
EFIAPI
GetNumberOfProcessors (
  IN  EFI_MP_SERVICES_PROTOCOL     *This,
  OUT UINTN                      *NumberOfProcessors,
  OUT UINTN                      *NumberOfEnabledProcessors
  )
{
  EFI_STATUS Status;
  UINTN      CallerNumber;

  ///
  /// Check whether caller processor is BSP
  ///
  WhoAmI (&mMpService, &CallerNumber);
  if (CallerNumber != mMPSystemData->BSP) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Check parameter NumberOfProcessors and NumberOfEnabledProcessors
  ///
  if (NumberOfProcessors == NULL || NumberOfEnabledProcessors == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetGeneralMPInfo (
                  &mMpService,
                  NumberOfProcessors,
                  NULL,
                  NumberOfEnabledProcessors,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Implementation of GetNumberOfProcessors() service of MP Services Protocol.

  Gets detailed MP-related information on the requested processor at the
  instant this call is made. This service may only be called from the BSP.

  @param[in] This                - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber     -  The handle number of processor.
  @param[in] ProcessorInfoBuffer - A pointer to the buffer where information for the requested processor is deposited.

  @retval EFI_SUCCESS           - Processor information successfully returned.
  @retval EFI_DEVICE_ERROR      - Caller processor is AP.
  @retval EFI_INVALID_PARAMETER - ProcessorInfoBuffer is NULL
  @retval EFI_NOT_FOUND         - Processor with the handle specified by ProcessorNumber does not exist.
**/
EFI_STATUS
EFIAPI
GetProcessorInfo (
  IN       EFI_MP_SERVICES_PROTOCOL     *This,
  IN UINTN                       ProcessorNumber,
  OUT EFI_PROCESSOR_INFORMATION  *ProcessorInfoBuffer
  )
{
  EFI_STATUS          Status;
  UINTN               CallerNumber;
  UINTN               BufferSize;
  EFI_MP_PROC_CONTEXT ProcessorContextBuffer;
  CPU_DATA_BLOCK      *CpuData;

  ///
  /// Check whether caller processor is BSP
  ///
  WhoAmI (&mMpService, &CallerNumber);
  if (CallerNumber != mMPSystemData->BSP) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Check parameter ProcessorInfoBuffer
  ///
  if (ProcessorInfoBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Check whether processor with the handle specified by ProcessorNumber exists
  ///
  if (ProcessorNumber >= mMPSystemData->NumberOfCpus) {
    return EFI_NOT_FOUND;
  }

  BufferSize = sizeof (EFI_MP_PROC_CONTEXT);
  Status = GetProcessorContext (
                  &mMpService,
                  ProcessorNumber,
                  &BufferSize,
                  &ProcessorContextBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  ProcessorInfoBuffer->ProcessorId = (UINT64) ProcessorContextBuffer.ApicID;

  ///
  /// Get Status Flag of specified processor
  ///
  ProcessorInfoBuffer->StatusFlag = 0;

  if (ProcessorContextBuffer.Enabled) {
    ProcessorInfoBuffer->StatusFlag |= PROCESSOR_ENABLED_BIT;
  }

  if (ProcessorContextBuffer.Designation == EfiCpuBSP) {
    ProcessorInfoBuffer->StatusFlag |= PROCESSOR_AS_BSP_BIT;
  }

  if (ProcessorContextBuffer.Health.Flags.Uint32 == 0) {
    ProcessorInfoBuffer->StatusFlag |= PROCESSOR_HEALTH_STATUS_BIT;
  }

  CpuData                               = &mMPSystemData->CpuData[ProcessorNumber];
  ProcessorInfoBuffer->Location.Package = (UINT32) CpuData->CpuDataforDatahub.Location.Package;
  ProcessorInfoBuffer->Location.Core    = (UINT32) CpuData->CpuDataforDatahub.Location.Core;
  ProcessorInfoBuffer->Location.Thread  = (UINT32) CpuData->CpuDataforDatahub.Location.Thread;

  return EFI_SUCCESS;
}

/**
  Implementation of StartupAllAPs() service of MP Services Protocol.

  This service lets the caller get all enabled APs to execute a caller-provided function.
  This service may only be called from the BSP.

  @param[in] This                  - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] Procedure             - A pointer to the function to be run on enabled APs of the system.
  @param[in] SingleThread          - Indicates whether to execute the function simultaneously or one by one..
  @param[in] WaitEvent             - The event created by the caller.
                                     If it is NULL, then execute in blocking mode.
                                     If it is not NULL, then execute in non-blocking mode.
  @param[in] TimeoutInMicroSeconds - The time limit in microseconds for this AP to finish the function.
                                     Zero means infinity.
  @param[in] ProcedureArgument     - Pointer to the optional parameter of the assigned function.
  @param[in] FailedCpuList         - The list of processor numbers that fail to finish the function before
                                     TimeoutInMicrosecsond expires.

  @retval EFI_SUCCESS           - In blocking mode, all APs have finished before the timeout expired.
  @retval EFI_SUCCESS           - In non-blocking mode, function has been dispatched to all enabled APs.
  @retval EFI_DEVICE_ERROR      - Caller processor is AP.
  @retval EFI_NOT_STARTED       - No enabled AP exists in the system.
  @retval EFI_NOT_READY         - Any enabled AP is busy.
  @retval EFI_TIMEOUT           - In blocking mode, The timeout expired before all enabled APs have finished.
  @retval EFI_INVALID_PARAMETER - Procedure is NULL.
**/
EFI_STATUS
EFIAPI
PiStartupAllAPs (
  IN  EFI_MP_SERVICES_PROTOCOL                           * This,
  IN EFI_AP_PROCEDURE            Procedure,
  IN BOOLEAN                     SingleThread,
  IN EFI_EVENT WaitEvent         OPTIONAL,
  IN UINTN                       TimeoutInMicroSeconds,
  IN VOID *ProcedureArgument     OPTIONAL,
  OUT UINTN **FailedCpuList      OPTIONAL
  )
{
  EFI_STATUS     Status;
  UINTN          ProcessorNumber;
  CPU_DATA_BLOCK *CpuData;
  BOOLEAN        Blocking;

  if (FailedCpuList != NULL) {
    *FailedCpuList = NULL;
  }
  ///
  /// Check whether caller processor is BSP
  ///
  WhoAmI (&mMpService, &ProcessorNumber);
  if (ProcessorNumber != mMPSystemData->BSP) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Check parameter Procedure
  ///
  if (Procedure == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Temporarily suppress CheckAPsStatus()
  ///
  mStopCheckAPsStatus = TRUE;

  ///
  /// Check whether all enabled APs are idle.
  /// If any enabled AP is not idle, return EFI_NOT_READY.
  ///
  for (ProcessorNumber = 0; ProcessorNumber < mMPSystemData->NumberOfCpus; ProcessorNumber++) {

    CpuData = &mMPSystemData->CpuData[ProcessorNumber];

    mMPSystemData->CpuList[ProcessorNumber] = FALSE;
    ///
    /// Skip BSP and disabled APs.
    ///
    if (ProcessorNumber == mMPSystemData->BSP || CpuData->State == CPU_STATE_DISABLED) {
      continue;
    }
    ///
    /// If any enabled APs are busy, return EFI_NOT_FOUND.
    ///
    if (CpuData->State != CPU_STATE_IDLE) {
      mStopCheckAPsStatus = FALSE;
      return EFI_NOT_READY;
    } else {
      ///
      /// Mark this processor as responsible for current calling.
      ///
      mMPSystemData->CpuList[ProcessorNumber] = TRUE;
    }
  }
  ///
  /// All enabled APs are idle, we can safely initiate a new session
  ///
  mMPSystemData->FinishCount  = 0;
  mMPSystemData->StartCount   = 0;
  Blocking                    = FALSE;
  ///
  /// Go through all enabled APs to wakeup them for Procedure.
  /// If in Single Thread mode, then only one AP is woken up, and others are waiting.
  ///
  for (ProcessorNumber = 0; ProcessorNumber < mMPSystemData->NumberOfCpus; ProcessorNumber++) {

    CpuData = &mMPSystemData->CpuData[ProcessorNumber];
    ///
    /// Check whether this processor is responsible for current calling.
    ///
    if (mMPSystemData->CpuList[ProcessorNumber]) {

      mMPSystemData->StartCount++;

      AsmAcquireMPLock (&CpuData->StateLock);
      CpuData->State = CPU_STATE_READY;
      AsmReleaseMPLock (&CpuData->StateLock);

      if (!Blocking) {
        WakeUpAp (
                CpuData,
                Procedure,
                ProcedureArgument
                );
      }

      if (SingleThread) {
        Blocking = TRUE;
      }
    }
  }
  ///
  /// If no enabled AP exists, return EFI_NOT_STARTED.
  ///
  if (mMPSystemData->StartCount == 0) {
    mStopCheckAPsStatus = FALSE;
    return EFI_NOT_STARTED;
  }
  ///
  /// If WaitEvent is not NULL, execute in non-blocking mode.
  /// BSP saves data for CheckAPsStatus(), and returns EFI_SUCCESS.
  /// CheckAPsStatus() will check completion and timeout periodically.
  ///
  mMPSystemData->Procedure      = Procedure;
  mMPSystemData->ProcArguments  = ProcedureArgument;
  mMPSystemData->SingleThread   = SingleThread;
  mMPSystemData->FailedCpuList  = FailedCpuList;
  mMPSystemData->ExpectedTime   = CalculateTimeout (TimeoutInMicroSeconds);
  mMPSystemData->WaitEvent      = WaitEvent;

  ///
  /// Allow CheckAPsStatus()
  ///
  mStopCheckAPsStatus = FALSE;

  if (WaitEvent != NULL) {
    return EFI_SUCCESS;
  }
  ///
  /// If WaitEvent is NULL, execute in blocking mode.
  /// BSP checks APs'state until all APs finish or TimeoutInMicrosecsond expires.
  ///
  do {
    Status = CheckAllAPs ();
  } while (Status == EFI_NOT_READY);

  return Status;
}

/**
  Implementation of StartupThisAP() service of MP Services Protocol.

  This service lets the caller get one enabled AP to execute a caller-provided function.
  This service may only be called from the BSP.

  @param[in] This                  - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] Procedure             - A pointer to the function to be run on the designated AP.
  @param[in] ProcessorNumber       - The handle number of AP..
  @param[in] WaitEvent             - The event created by the caller.
                                     If it is NULL, then execute in blocking mode.
                                     If it is not NULL, then execute in non-blocking mode.
  @param[in] TimeoutInMicroseconds - The time limit in microseconds for this AP to finish the function.
                                     Zero means infinity.
  @param[in] ProcedureArgument     - Pointer to the optional parameter of the assigned function.
  @param[in] Finished              - Indicates whether AP has finished assigned function.
                                     In blocking mode, it is ignored.

  @retval EFI_SUCCESS           - In blocking mode, specified AP has finished before the timeout expires.
  @retval EFI_SUCCESS           - In non-blocking mode, function has been dispatched to specified AP.
  @retval EFI_DEVICE_ERROR      - Caller processor is AP.
  @retval EFI_TIMEOUT           - In blocking mode, the timeout expires before specified AP has finished.
  @retval EFI_NOT_READY         - Specified AP is busy.
  @retval EFI_NOT_FOUND         - Processor with the handle specified by ProcessorNumber does not exist.
  @retval EFI_INVALID_PARAMETER - ProcessorNumber specifies the BSP or disabled AP.
  @retval EFI_INVALID_PARAMETER - Procedure is NULL.
**/
EFI_STATUS
EFIAPI
PiStartupThisAP (
  IN  EFI_MP_SERVICES_PROTOCOL               * This,
  IN EFI_AP_PROCEDURE            Procedure,
  IN UINTN                       ProcessorNumber,
  IN EFI_EVENT WaitEvent         OPTIONAL,
  IN UINTN                       TimeoutInMicroseconds,
  IN VOID *ProcedureArgument     OPTIONAL,
  OUT BOOLEAN *Finished          OPTIONAL
  )
{
  CPU_DATA_BLOCK *CpuData;
  UINTN          CallerNumber;
  EFI_STATUS     Status;

  if (Finished != NULL) {
    *Finished = TRUE;
  }
  ///
  /// Check whether caller processor is BSP
  ///
  WhoAmI (&mMpService, &CallerNumber);
  if (CallerNumber != mMPSystemData->BSP) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Check whether processor with the handle specified by ProcessorNumber exists
  ///
  if (ProcessorNumber >= mMPSystemData->NumberOfCpus) {
    return EFI_NOT_FOUND;
  }
  ///
  /// Check whether specified processor is BSP
  ///
  if (ProcessorNumber == mMPSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Check parameter Procedure
  ///
  if (Procedure == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData = &mMPSystemData->CpuData[ProcessorNumber];

  ///
  /// Temporarily suppress CheckAPsStatus()
  ///
  mStopCheckAPsStatus = TRUE;

  ///
  /// Check whether specified AP is disabled
  ///
  if (CpuData->State == CPU_STATE_DISABLED) {
    mStopCheckAPsStatus = FALSE;
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Check whether specified AP is busy
  ///
  if (CpuData->State != CPU_STATE_IDLE) {
    mStopCheckAPsStatus = FALSE;
    return EFI_NOT_READY;
  }
  ///
  /// Wakeup specified AP for Procedure.
  ///
  AsmAcquireMPLock (&CpuData->StateLock);
  CpuData->State = CPU_STATE_READY;
  AsmReleaseMPLock (&CpuData->StateLock);

  WakeUpAp (
          CpuData,
          Procedure,
          ProcedureArgument
          );

  ///
  /// If WaitEvent is not NULL, execute in non-blocking mode.
  /// BSP saves data for CheckAPsStatus(), and returns EFI_SUCCESS.
  /// CheckAPsStatus() will check completion and timeout periodically.
  ///
  CpuData->WaitEvent    = WaitEvent;
  CpuData->Finished     = Finished;
  CpuData->ExpectedTime = CalculateTimeout (TimeoutInMicroseconds);

  ///
  /// Allow CheckAPsStatus()
  ///
  mStopCheckAPsStatus = FALSE;

  if (WaitEvent != NULL) {
    return EFI_SUCCESS;
  }
  ///
  /// If WaitEvent is NULL, execute in blocking mode.
  /// BSP checks AP's state until it finishes or TimeoutInMicrosecsond expires.
  ///
  do {
    Status = CheckThisAP (ProcessorNumber);
  } while (Status == EFI_NOT_READY);

  return Status;
}

/**
  Implementation of SwitchBSP() service of MP Services Protocol.

  This service switches the requested AP to be the BSP from that point onward.
  This service may only be called from the current BSP.

  @param[in] This                 - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber      - The handle number of processor.
  @param[in] EnableOldBSP         - Whether to enable or disable the original BSP.

  @retval EFI_SUCCESS           - BSP successfully switched.
  @retval EFI_DEVICE_ERROR      - Caller processor is AP.
  @retval EFI_NOT_FOUND         - Processor with the handle specified by ProcessorNumber does not exist.
  @retval EFI_INVALID_PARAMETER - ProcessorNumber specifies the BSP or disabled AP.
  @retval EFI_NOT_READY         - Specified AP is busy.
**/
EFI_STATUS
EFIAPI
PiSwitchBSP (
  IN  EFI_MP_SERVICES_PROTOCOL     *This,
  IN UINTN                       ProcessorNumber,
  IN BOOLEAN                     EnableOldBSP
  )
{
  EFI_STATUS     Status;
  CPU_DATA_BLOCK *CpuData;
  UINTN          CallerNumber;

  ///
  /// Check whether caller processor is BSP
  ///
  WhoAmI (&mMpService, &CallerNumber);
  if (CallerNumber != mMPSystemData->BSP) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Check whether processor with the handle specified by ProcessorNumber exists
  ///
  if (ProcessorNumber >= mMPSystemData->NumberOfCpus) {
    return EFI_NOT_FOUND;
  }
  ///
  /// Check whether specified processor is BSP
  ///
  if (ProcessorNumber == mMPSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData = &mMPSystemData->CpuData[ProcessorNumber];

  ///
  /// Check whether specified AP is disabled
  ///
  if (CpuData->State == CPU_STATE_DISABLED) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Check whether specified AP is busy
  ///
  if (CpuData->State != CPU_STATE_IDLE) {
    return EFI_NOT_READY;
  }

  Status = SwitchBSP (
                  &mMpService,
                  ProcessorNumber,
                  EnableOldBSP
                  );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Implementation of EnableDisableAP() service of MP Services Protocol.

  This service lets the caller enable or disable an AP.
  This service may only be called from the BSP.

  @param[in] This                  - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber       - The handle number of processor.
  @param[in] EnableAP              - Indicates whether the newstate of the AP is enabled or disabled.
  @param[in] HealthFlag            - Indicates new health state of the AP..

  @retval EFI_SUCCESS            - AP successfully enabled or disabled.
  @retval EFI_DEVICE_ERROR       - Caller processor is AP.
  @retval EFI_NOT_FOUND          - Processor with the handle specified by ProcessorNumber does not exist.
  @retval EFI_INVALID_PARAMETERS - ProcessorNumber specifies the BSP.
**/
EFI_STATUS
EFIAPI
PiEnableDisableAP (
  IN  EFI_MP_SERVICES_PROTOCOL     * This,
  IN UINTN                       ProcessorNumber,
  IN BOOLEAN                     EnableAP,
  IN UINT32 *HealthFlag          OPTIONAL
  )
{
  EFI_STATUS    Status;
  UINTN         CallerNumber;
  EFI_MP_HEALTH HealthState;
  EFI_MP_HEALTH *HealthStatePointer;

  ///
  /// Check whether caller processor is BSP
  ///
  WhoAmI (&mMpService, &CallerNumber);
  if (CallerNumber != mMPSystemData->BSP) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Check whether processor with the handle specified by ProcessorNumber exists
  ///
  if (ProcessorNumber >= mMPSystemData->NumberOfCpus) {
    return EFI_NOT_FOUND;
  }
  ///
  /// Check whether specified processor is BSP
  ///
  if (ProcessorNumber == mMPSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }

  if (HealthFlag == NULL) {
    HealthStatePointer = NULL;
  } else {
    if ((*HealthFlag & PROCESSOR_HEALTH_STATUS_BIT) == 0) {
      HealthState.Flags.Uint32 = 1;
    } else {
      HealthState.Flags.Uint32 = 0;
    }

    HealthState.TestStatus  = 0;

    HealthStatePointer      = &HealthState;
  }

  Status = EnableDisableAP (
                  &mMpService,
                  ProcessorNumber,
                  EnableAP,
                  HealthStatePointer
                  );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Implementation of WhoAmI() service of MP Services Protocol.

  This service lets the caller processor get its handle number.
  This service may be called from the BSP and APs.

  @param[in] This                 - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber      - Pointer to the handle number of AP.

  @retval EFI_SUCCESS           - Processor number successfully returned.
  @retval EFI_INVALID_PARAMETER - ProcessorNumber is NULL
**/
EFI_STATUS
EFIAPI
PiWhoAmI (
  IN EFI_MP_SERVICES_PROTOCOL     *This,
  OUT UINTN                      *ProcessorNumber
  )
{
  EFI_STATUS Status;

  if (ProcessorNumber == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = WhoAmI (&mMpService, ProcessorNumber);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
