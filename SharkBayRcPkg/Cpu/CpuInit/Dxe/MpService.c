/** @file
  Code which support multi-processor

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
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

  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/

#include "MpService.h"
#include "PiMpService.h"
#include "CpuInitDxe.h"
#include "MachineCheck.h"
#include "Features.h"
#include <Uefi/UefiSpec.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/BootGuardLibrary.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/StartOfBdsDiagnostics.h>
#include <Protocol/CpuInfo.h>
#include <Guid/HtBistHob.h>



 
extern DXE_CPU_PLATFORM_POLICY_PROTOCOL *mPlatformCpu;
extern EFI_METRONOME_ARCH_PROTOCOL      *mMetronome;
extern MP_SYSTEM_DATA                   *mMPSystemData;
extern UINTN                            mCommonFeatures;
extern volatile UINTN                   mSwitchToLegacyRegionCount;
extern EFI_CPU_MICROCODE_HEADER         **mMicrocodePointerBuffer;
extern EFI_DATA_HUB_PROTOCOL            *mDataHub;

static EFI_HANDLE                       mHandle         = NULL;
static UINT32                           mFinishedCount  = 0;
extern UINT32                           mMcuLoadCount;
STATIC UINT64                           mCpuPerfCtrlValue;
FRAMEWORK_EFI_MP_SERVICES_PROTOCOL      mMpService = {
  GetGeneralMPInfo,
  GetProcessorContext,
  StartupAllAPs,
  StartupThisAP,
  SwitchBSP,
  SendIPI,
  EnableDisableAP,
  WhoAmI
};

EFI_PHYSICAL_ADDRESS  mOriginalBuffer;
EFI_PHYSICAL_ADDRESS  mBackupBuffer;
DXE_CPU_INFO_PROTOCOL mCpuInfo;

/**
  Initialize MP services by MP Service Protocol
**/
VOID
EFIAPI
InitializeMpServices (
  VOID
  )
{
  EFI_STATUS Status;
  EFI_EVENT  LegacyBootEvent;
  EFI_EVENT  ExitBootServicesEvent;
  VOID       *Registration;

  LegacyBootEvent       = NULL;
  ExitBootServicesEvent = NULL;

  ///
  /// Save Mtrr Registers in global data areas
  ///
  ReadMtrrRegisters ();

  ///
  /// Initialize and collect MP related data
  ///
  Status = InitializeMpSystemData ();
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  ///
  /// Register protocol notifacation function for ExitPmAuth protocol
  ///
  EfiCreateProtocolNotifyEvent (
    &gExitPmAuthProtocolGuid,
    TPL_NOTIFY,
    ReAllocateMemoryForAP,
    NULL,
    &Registration
    );
//[-start-120321-IB02700366-remove]// 
//  //
//  // Register for protocol notifactions to allocate memory in EBDA as early as possible
//  //
//  EfiCreateProtocolNotifyEvent (
//    &gEfiLegacyBiosProtocolGuid, TPL_NOTIFY,
//    ReAllocateEbdaMemoryForAP,
//    NULL,
//    &Registration
//    );
//[-end-120321-IB02700366-remove]// 
  Status = EfiCreateEventLegacyBootEx (
            TPL_CALLBACK,
            ResetAPs,
            mMPSystemData,
            &LegacyBootEvent
            );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES,
                  TPL_CALLBACK,
                  ResetAPs,
                  mMPSystemData,
                  &ExitBootServicesEvent
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  ///
  /// Create timer event to check AP state for non-blocking execution.
  ///
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  CheckAPsStatus,
                  NULL,
                  &mMPSystemData->CheckAPsEvent
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Now install the Frameowrk & PI MP services protocol.
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gFrameworkEfiMpServiceProtocolGuid,
                  &mMpService,
                  &gEfiMpServiceProtocolGuid,
                  &mPiMpService,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->SetTimer (
                  mMPSystemData->CheckAPsEvent,
                  TimerPeriodic,
                  10000 * MICROSECOND
                  );
  ASSERT_EFI_ERROR (Status);

  if (EFI_ERROR (Status)) {

Done:
    if (LegacyBootEvent != NULL) {
      gBS->CloseEvent (LegacyBootEvent);
    }

    if (ExitBootServicesEvent != NULL) {
      gBS->CloseEvent (ExitBootServicesEvent);
    }

    FreePool (mMPSystemData);
  }
}

/**
  Get general MP information

  @param[in] This                  - EFI_MP_SERVICE_PROTOCOL
  @param[in] NumberOfCPUs          - Number of processors
  @param[in] MaximumNumberOfCPUs   - Max supported number of processors
  @param[in] NumberOfEnabledCPUs   - Number of processors enabled
  @param[in] RendezvousIntNumber   - number of Rendezvous procedure
  @param[in] RendezvousProcLength  - length of Rendezvous procedure

  @retval EFI_SUCCESS - always return success
**/
EFI_STATUS
EFIAPI
GetGeneralMPInfo (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL     *This,
  OUT UINTN                   *NumberOfCPUs,
  OUT UINTN                   *MaximumNumberOfCPUs,
  OUT UINTN                   *NumberOfEnabledCPUs,
  OUT UINTN                   *RendezvousIntNumber,
  OUT UINTN                   *RendezvousProcLength
  )
{
  UINTN          Index;
  CPU_DATA_BLOCK *CpuData;

  if (NumberOfCPUs) {
    *NumberOfCPUs = mMPSystemData->NumberOfCpus;
  }

  if (MaximumNumberOfCPUs) {
    *MaximumNumberOfCPUs = mMPSystemData->MaximumCpusForThisSystem;
  }

  if (RendezvousProcLength) {
    *RendezvousProcLength = RENDEZVOUS_PROC_LENGTH;
  }

  if (RendezvousIntNumber) {
    *RendezvousIntNumber = 0;
  }

  if (NumberOfEnabledCPUs) {
    *NumberOfEnabledCPUs = 0;
    for (Index = 0; Index < mMPSystemData->NumberOfCpus; Index++) {
      CpuData = &mMPSystemData->CpuData[Index];
      if (mMPSystemData->EnableSecondaryCpu) {
        if (CpuData->State != CPU_STATE_DISABLED) {
          (*NumberOfEnabledCPUs)++;
        }
      } else {
        if (CpuData->State != CPU_STATE_DISABLED && !mMPSystemData->CpuData[Index].SecondaryCpu) {
          (*NumberOfEnabledCPUs)++;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Get processor context

  @param[in] This                    - EFI_MP_SERVICE_PROTOCOL
  @param[in] CpuNumber               - Cpu number
  @param[in] BufferLength            - buffer length
  @param[in] ProcessorContextBuffer  - pointer to the buffer that will be updated

  @retval EFI_INVALID_PARAMETER - buffer is NULL or CpuNumber our of range
  @retval EFI_BUFFER_TOO_SMALL - buffer too small
  @retval EFI_SUCCESS - got processor context successfully
**/
EFI_STATUS
EFIAPI
GetProcessorContext (
  IN       FRAMEWORK_EFI_MP_SERVICES_PROTOCOL     *This,
  IN UINTN                    CpuNumber,
  IN OUT UINTN                *BufferLength,
  IN OUT EFI_MP_PROC_CONTEXT  *ProcessorContextBuffer
  )
{
  EFI_MP_PROC_CONTEXT *ProcessorBuffer;
  CPU_DATA_BLOCK      *CpuData;
//[-start-111019-IB06150196-add]//
  UINT64              MsrValue;
//[-end-111019-IB06150196-add]//

  if (BufferLength == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*BufferLength < sizeof (EFI_MP_PROC_CONTEXT)) {
    *BufferLength = sizeof (EFI_MP_PROC_CONTEXT);
    return EFI_BUFFER_TOO_SMALL;
  }

  if ((mMPSystemData->NumberOfCpus <= CpuNumber) || (ProcessorContextBuffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData                   = &mMPSystemData->CpuData[CpuNumber];

  *BufferLength             = sizeof (EFI_MP_PROC_CONTEXT);
  ProcessorBuffer           = ProcessorContextBuffer;

  ProcessorBuffer->ApicID   = CpuData->ApicID;

  ProcessorBuffer->Enabled  = TRUE;
  if (!mMPSystemData->EnableSecondaryCpu) {
    if (CpuData->SecondaryCpu) {
      ProcessorBuffer->Enabled = FALSE;
    }
  }

  if (CpuData->State == CPU_STATE_DISABLED) {
    ProcessorBuffer->Enabled = FALSE;
  }

  if (CpuNumber == mMPSystemData->BSP) {
    ProcessorBuffer->Designation = EfiCpuBSP;
  } else {
    ProcessorBuffer->Designation = EfiCpuAP;
  }

  ProcessorBuffer->Health.Flags       = CpuData->Health;
  ProcessorBuffer->Health.TestStatus  = 0;

  ProcessorBuffer->PackageNumber      = CpuData->CpuDataforDatahub.Location.Package;
//[-start-111019-IB06150196-modify]//
  MsrValue = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  ProcessorBuffer->NumberOfCores      = (UINTN)((MsrValue >> 16) & 0xffff);//CpuData->CpuDataforDatahub.Location.Core;
  ProcessorBuffer->NumberOfThreads    = (UINTN)(MsrValue & 0xffff);//CpuData->CpuDataforDatahub.Location.Thread;
//[-end-111019-IB06150196-modify]//
  ProcessorBuffer->ProcessorTestMask  = 0;

  return EFI_SUCCESS;
}

/**
  MP Service to get specified application processor (AP)
  to execute a caller-provided code stream.

  @param[in] This                - Pointer to MP Service Protocol
  @param[in] Procedure           - The procedure to be assigned to AP.
  @param[in] CpuNumber           - Number of the specified processor.
  @param[in] WaitEvent           - If timeout, the event to be triggered after this AP finishes.
  @param[in] TimeoutInMicroSecs  - The timeout value in microsecond. Zero means infinity.
  @param[in] ProcArguments       - Argument for Procedure.

  @retval EFI_INVALID_PARAMETER - Procudure is NULL.
  @retval EFI_INVALID_PARAMETER - Number of CPU out of range, or it belongs to BSP.
  @retval EFI_INVALID_PARAMETER - Specified CPU is not idle.
  @retval EFI_SUCCESS           - The AP has finished.
  @retval EFI_TIMEOUT           - Time goes out before the AP has finished.
**/
EFI_STATUS
EFIAPI
StartupThisAP (
  IN       FRAMEWORK_EFI_MP_SERVICES_PROTOCOL                                 * This,
  IN EFI_AP_PROCEDURE         Procedure,
  IN UINTN                    CpuNumber,
  IN EFI_EVENT WaitEvent      OPTIONAL,
  IN UINTN TimeoutInMicroSecs OPTIONAL,
  IN OUT VOID *ProcArguments  OPTIONAL
  )
{
  EFI_STATUS     Status;
  CPU_DATA_BLOCK *CpuData;
  UINT64         ExpectedTime;

  ///
  /// Check for invalid CPU number
  ///
  if ((CpuNumber >= mMPSystemData->NumberOfCpus) || CpuNumber == mMPSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }

  if (Procedure == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData = &mMPSystemData->CpuData[CpuNumber];

  ///
  /// As a first step, check if processor is OK to start up code stream.
  ///
  if (CpuData->State != CPU_STATE_IDLE) {
    return EFI_INVALID_PARAMETER;
  }

  ExpectedTime                = CalculateTimeout (TimeoutInMicroSecs);

  mMPSystemData->StartCount   = 1;
  mMPSystemData->FinishCount  = 0;

  WakeUpAp (
          CpuData,
          Procedure,
          ProcArguments
          );

  while (TRUE) {
    AsmAcquireMPLock (&CpuData->StateLock);
    if (CpuData->State == CPU_STATE_FINISHED) {
      CpuData->State = CPU_STATE_IDLE;
      AsmReleaseMPLock (&CpuData->StateLock);
      break;
    }

    AsmReleaseMPLock (&CpuData->StateLock);

    if (CheckTimeout (ExpectedTime)) {
      ///
      /// Save data into private data structure, and create timer to poll AP state before exiting
      ///
      mMPSystemData->WaitEvent        = WaitEvent;
      Status = gBS->SetTimer (
                      CpuData->CheckThisAPEvent,
                      TimerPeriodic,
                      CPU_CHECK_AP_INTERVAL * MICROSECOND
                      );
      return EFI_TIMEOUT;
    }

    gBS->Stall (CPU_CHECK_AP_INTERVAL);
  }

  return EFI_SUCCESS;
}

/**
  MP Service to get all the available application processors (APs)
  to execute a caller-provided code stream.

  @param[in] This                - Pointer to MP Service Protocol
  @param[in] Procedure           - The procedure to be assigned to APs.
  @param[in] SingleThread        - If true, all APs execute in block mode.
                        Otherwise, all APs exceute in non-block mode.
  @param[in] WaitEvent           - If timeout, the event to be triggered after all APs finish.
  @param[in] TimeoutInMicroSecs  - The timeout value in microsecond. Zero means infinity.
  @param[in] ProcArguments       - Argument for Procedure.
  @param[in] FailedCPUList       - If not NULL, all APs that fail to start will be recorded in the list.

  @retval EFI_INVALID_PARAMETER - Procudure is NULL.
  @retval EFI_SUCCESS           - Only 1 logical processor exists.
  @retval EFI_SUCCESS           - All APs have finished.
  @retval EFI_TIMEOUT           - Time goes out before all APs have finished.
**/
EFI_STATUS
EFIAPI
StartupAllAPs (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL                                        * This,
  IN EFI_AP_PROCEDURE         Procedure,
  IN BOOLEAN                  SingleThread,
  IN EFI_EVENT WaitEvent      OPTIONAL,
  IN UINTN TimeoutInMicroSecs OPTIONAL,
  IN OUT VOID *ProcArguments  OPTIONAL,
  OUT UINTN *FailedCPUList    OPTIONAL
  )
{
  EFI_STATUS     Status;
  CPU_DATA_BLOCK *CpuData;
  CPU_DATA_BLOCK *NextCpuData;
  UINTN          ListIndex;
  UINTN          CpuNumber;
  UINTN          NextCpuNumber;
  UINT64         ExpectedTime;
  CPU_STATE      APInitialState;
  CPU_STATE      CpuState;

  ///
  /// Check for valid procedure for APs
  ///
  if (Procedure == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mMPSystemData->NumberOfCpus == 1) {
    return EFI_SUCCESS;
  }

  ExpectedTime                = CalculateTimeout (TimeoutInMicroSecs);

  ListIndex                   = 0;
  CpuData                     = NULL;

  mMPSystemData->FinishCount  = 0;
  mMPSystemData->StartCount   = 0;
  APInitialState              = CPU_STATE_READY;

  for (CpuNumber = 0; CpuNumber < mMPSystemData->NumberOfCpus; CpuNumber++) {
    CpuData = &mMPSystemData->CpuData[CpuNumber];

    ///
    /// Get APs prepared, and put failing APs into FailedCPUList
    /// If "SingleThread", one AP will be put to ready state.
    /// Once this AP finishes its task, the next AP is put to Ready state.
    /// This process continues until all APs are put into Ready State
    /// if not "SingleThread", all APs are put to ready state at the same time
    ///
    if (CpuNumber != mMPSystemData->BSP) {
      if (CpuData->State == CPU_STATE_IDLE) {
        mMPSystemData->StartCount++;

        AsmAcquireMPLock (&CpuData->StateLock);
        CpuData->State = APInitialState;
        AsmReleaseMPLock (&CpuData->StateLock);

        if (SingleThread) {
          APInitialState = CPU_STATE_BLOCKED;
        }

      } else if (FailedCPUList != NULL) {
        FailedCPUList[ListIndex] = CpuNumber;
        ListIndex++;
      }
    }
  }

  while (TRUE) {
    for (CpuNumber = 0; CpuNumber < mMPSystemData->NumberOfCpus; CpuNumber++) {
      CpuData = &mMPSystemData->CpuData[CpuNumber];
      if (CpuNumber == mMPSystemData->BSP) {
        continue;
      }
      CpuState = CpuData->State;
      switch (CpuState) {
        case CPU_STATE_READY:
          WakeUpAp (
                  CpuData,
                  Procedure,
                  ProcArguments
                  );
          break;

        case CPU_STATE_FINISHED:
          mMPSystemData->FinishCount++;
          if (SingleThread) {
            Status = GetNextBlockedCpuNumber (&NextCpuNumber);
            if (!EFI_ERROR (Status)) {
              NextCpuData = &mMPSystemData->CpuData[NextCpuNumber];
              AsmAcquireMPLock (&NextCpuData->StateLock);
              NextCpuData->State = CPU_STATE_READY;
              AsmReleaseMPLock (&NextCpuData->StateLock);
            }
          }
          AsmAcquireMPLock (&CpuData->StateLock);
          CpuData->State = CPU_STATE_IDLE;
          AsmReleaseMPLock (&CpuData->StateLock);
          break;

        default:
          break;
      }
    }

    if (mMPSystemData->FinishCount == mMPSystemData->StartCount) {
      return EFI_SUCCESS;
    }

    if (CheckTimeout (ExpectedTime)) {
      ///
      /// Save data into private data structure, and create timer to poll AP state before exiting
      ///
      mMPSystemData->Procedure      = Procedure;
      mMPSystemData->ProcArguments  = ProcArguments;
      mMPSystemData->SingleThread   = SingleThread;
      mMPSystemData->WaitEvent      = WaitEvent;

      Status = gBS->SetTimer (
                      mMPSystemData->CheckAllAPsEvent,
                      TimerPeriodic,
                      CPU_CHECK_AP_INTERVAL * MICROSECOND
                      );
      return EFI_TIMEOUT;
    }

    gBS->Stall (CPU_CHECK_AP_INTERVAL);
  }

  return EFI_SUCCESS;
}

/**
    MP Service to makes the current BSP into an AP and then switches the
    designated AP into the AP. This procedure is usually called after a CPU
    test that has found that BSP is not healthy to continue it's responsbilities.

    @param[in] This         - Pointer to MP Service Protocol.
    @param[in] CpuNumber    - The number of the specified AP.
    @param[in] EnableOldBSP - Whether to enable or disable the original BSP.

    @retval EFI_INVALID_PARAMETER - Number for Specified AP out of range.
    @retval EFI_INVALID_PARAMETER - Number of specified CPU belongs to BSP.
    @retval EFI_NOT_READY         - Specified AP is not idle.
    @retval EFI_SUCCESS           - BSP successfully switched.
**/
EFI_STATUS
EFIAPI
SwitchBSP (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL  *This,
  IN UINTN                    CpuNumber,
  IN BOOLEAN                  EnableOldBSP
  )
{
  EFI_STATUS            Status;
  EFI_CPU_ARCH_PROTOCOL *CpuArch;
  BOOLEAN               OldInterruptState;
  CPU_DATA_BLOCK        *CpuData;
  CPU_STATE             CpuState;

  ///
  /// Check if the specified CPU number is valid
  ///
  if (CpuNumber >= mMPSystemData->NumberOfCpus) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Check if the specified CPU is already BSP
  ///
  if (CpuNumber == mMPSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData = &mMPSystemData->CpuData[CpuNumber];
  if (CpuData->State != CPU_STATE_IDLE) {
    return EFI_NOT_READY;
  }
  ///
  /// Before send both BSP and AP to a procedure to exchange their roles,
  /// interrupt must be disabled. This is because during the exchange role
  /// process, 2 CPU may use 1 stack. If interrupt happens, the stack will
  /// be corrputed, since interrupt return address will be pushed to stack
  /// by hardware.
  ///
  CpuArch = mMPSystemData->CpuArch;
  (CpuArch->GetInterruptState)(CpuArch, &OldInterruptState);
  if (OldInterruptState) {
    Status = CpuArch->DisableInterrupt (CpuArch);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  ///
  /// Unprogram virtual wire mode for the old BSP
  ///
  ProgramXApic (FALSE);
  SetApicBSPBit (FALSE);

  mMPSystemData->BSPInfo.State  = CPU_SWITCH_STATE_IDLE;
  mMPSystemData->BSPInfo.Lock   = VacantFlag;
  mMPSystemData->APInfo.State   = CPU_SWITCH_STATE_IDLE;
  mMPSystemData->APInfo.Lock    = VacantFlag;

  ///
  /// Need to wakeUp AP (future BSP)
  ///
  WakeUpAp (
          CpuData,
          FutureBSPProc,
          mMPSystemData
          );

  AsmExchangeRole (&mMPSystemData->BSPInfo, &mMPSystemData->APInfo);

  ///
  /// The new BSP has come out. Since it carries the register value of the AP, need
  /// to pay attention to variable which are stored in registers (due to optimization)
  ///
  SetApicBSPBit (TRUE);
  ProgramXApic (TRUE);

  if (OldInterruptState) {
    Status = CpuArch->EnableInterrupt (CpuArch);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  CpuData = &mMPSystemData->CpuData[mMPSystemData->BSP];
  while (TRUE) {
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuState = CpuData->State;
    AsmReleaseMPLock (&CpuData->StateLock);

    if (CpuState == CPU_STATE_FINISHED) {
      break;
    }
  }

  Status              = ChangeCpuState (mMPSystemData->BSP, EnableOldBSP, CPU_CAUSE_NOT_DISABLED);
  mMPSystemData->BSP  = CpuNumber;

  return EFI_SUCCESS;
}

/**
  This procedure sends an IPI to the designated processor in
  the requested delivery mode with the requested vector.

  @param[in] This         - Pointer to MP Service Protocol.
  @param[in] CpuNumber    - The number of the specified AP.
  @param[in] VectorNumber - Vector number.
  @param[in] DeliveryMode - I/O APIC Interrupt Deliver Modes

  @retval EFI_INVALID_PARAMETER - Input paramters were not correct.
  @retval Other status          - Status returned by SendInterrupt ()
**/
EFI_STATUS
EFIAPI
SendIPI (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL  *This,
  IN UINTN                    CpuNumber,
  IN UINTN                    VectorNumber,
  IN UINTN                    DeliveryMode
  )
{
  UINT32               TriggerMode;
  EFI_STATUS           Status;
  CPU_DATA_BLOCK       *CpuData;
  MP_CPU_EXCHANGE_INFO *ExchangeInfo;
  MONITOR_MWAIT_DATA   *MonitorAddr;

  ///
  /// Check for valid input parameters.
  ///
  if (CpuNumber >= mMPSystemData->NumberOfCpus || CpuNumber == mMPSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }

  if (VectorNumber >= INTERRUPT_VECTOR_NUMBER) {
    return EFI_INVALID_PARAMETER;
  }

  if (DeliveryMode >= DELIVERY_MODE_MAX) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData     = &mMPSystemData->CpuData[CpuNumber];
  TriggerMode = TRIGGER_MODE_EDGE;

  ///
  /// Fix the vector number for special interrupts like SMI and INIT.
  ///
  if (DeliveryMode == DELIVERY_MODE_SMI || DeliveryMode == DELIVERY_MODE_INIT) {
    VectorNumber = 0x0;
  }

  ///
  /// If current waking manner is not HLT loop and some other DXE driver
  /// like as TXT needs to send INIT command per this MpService routine,
  /// then we need to set vector change flag for the specific AP to back
  /// to MWAIT or RUN loop procedure, so additional INIT-SIPI command
  /// will be sent in WakeUpAp routine for AP tasks to work later.
  ///
  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mAcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  if (ExchangeInfo->WakeUpApManner != WakeUpApPerHltLoop && DeliveryMode == DELIVERY_MODE_INIT) {
    MonitorAddr = (MONITOR_MWAIT_DATA *)
            (
                    (UINT8 *) ExchangeInfo->StackStart +
                    (ExchangeInfo->BistBuffer[CpuData->ApicID].Number + 1) *
                    ExchangeInfo->StackSize -
                    MONITOR_FILTER_SIZE
            );
    MonitorAddr->WakeUpApVectorChangeFlag = TRUE;
  }

  Status = SendInterrupt (
                  BROADCAST_MODE_SPECIFY_CPU,
                  CpuData->ApicID,
                  (UINT32) VectorNumber,
                  (UINT32) DeliveryMode,
                  TriggerMode,
                  TRUE
                  );

  return Status;
}

/**
  This procedure enables Or disables APs.

  @param[in] This         - Pointer to MP Service Protocol.
  @param[in] CpuNumber    - The number of the specified AP.
  @param[in] NewAPState   - Indicate new desired AP state
  @param[in] HealthState  - If not NULL, it points to the value that specifies
                            the new health status of the AP.  If it is NULL,
                            this parameter is ignored.

  @retval EFI_INVALID_PARAMETER - Input paramters were not correct.
  @retval EFI_SUCCESS           - Function completed successfully
**/
EFI_STATUS
EFIAPI
EnableDisableAP (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL  * This,
  IN UINTN                      CpuNumber,
  IN BOOLEAN                    NewAPState,
  IN EFI_MP_HEALTH *HealthState OPTIONAL
  )
{
  EFI_STATUS     Status;
  CPU_DATA_BLOCK *CpuData;

  ///
  /// Check for valid input parameters.
  ///
  if (CpuNumber >= mMPSystemData->NumberOfCpus || CpuNumber == mMPSystemData->BSP) {
    return EFI_INVALID_PARAMETER;
  }

  CpuData = &mMPSystemData->CpuData[CpuNumber];
  Status  = ChangeCpuState (CpuNumber, NewAPState, CPU_CAUSE_USER_SELECTION);

  if (HealthState != NULL) {
    CopyMem (&CpuData->Health, HealthState, sizeof (EFI_MP_HEALTH));
  }

  return EFI_SUCCESS;
}

/**
  This procedure returns the calling CPU handle.

  @param[in] This         - Pointer to MP Service Protocol.
  @param[in] CpuNumber    - The number of the specified AP.

  @retval EFI_SUCCESS           - Function completed successfully
**/
EFI_STATUS
EFIAPI
WhoAmI (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL  *This,
  OUT UINTN                               *CpuNumber
  )
{
  UINTN ApicID;
  UINTN NumOfCpus;
  UINTN Index;

  ApicID    = GetApicID (NULL, NULL);

  NumOfCpus = mMPSystemData->NumberOfCpus;

  for (Index = 0; Index < NumOfCpus; Index++) {
    if (ApicID == mMPSystemData->CpuData[Index].ApicID) {
      break;
    }
  }

  *CpuNumber = Index;
  return EFI_SUCCESS;
}

/**
  Searches the HOB list provided by the core to find
  if a MP guided HOB list exists or not. If it does, it copies it to the driver
  data area, else returns 0

  @param[in] MPSystemData - Pointer to an MP_SYSTEM_DATA structure

  @retval EFI_SUCCESS  - Success
  @retval EFI_NOT_FOUND - HOB not found or else
**/
EFI_STATUS
GetMpBistStatus (
  IN MP_SYSTEM_DATA *MPSystemData
  )
{
  VOID                 *HobList;
  VOID                 *DataInHob;
  EFI_PEI_HOB_POINTERS Hob;
  UINTN                DataSize;

  ///
  /// Check for MP Data Hob.
  ///
  HobList = GetFirstGuidHob (&gEfiHtBistHobGuid);

  if (HobList == NULL) {
    DEBUG ((EFI_D_ERROR, "No HOBs found\n"));
    return EFI_NOT_FOUND;
  }

  DataInHob   = (VOID *) ((UINTN) HobList + sizeof (EFI_HOB_GUID_TYPE));

  Hob.Header  = HobList;
  DataSize    = Hob.Header->HobLength - sizeof (EFI_HOB_GUID_TYPE);

  ///
  /// This is the MP HOB. So, copy all the data
  ///
  if (HobList != NULL) {
    if (NULL == MPSystemData->BistHobData) {
      (gBS->AllocatePool)(EfiReservedMemoryType, DataSize, (VOID **) &MPSystemData->BistHobData);
    }

    CopyMem (MPSystemData->BistHobData, DataInHob, DataSize);
    MPSystemData->BistHobSize = DataSize;
  }

  return EFI_SUCCESS;
}

/**
  Check if CPUID support large than 3

  @retval TRUE if CPUID support large than 3
  @retval FALSE if not
**/
BOOLEAN
IsCpuidSupportOver3 (
  VOID
  )
{
  BOOLEAN            RetVal;
  EFI_CPUID_REGISTER CpuidRegisters;
  RetVal = FALSE;
  AsmCpuid (
          CPUID_SIGNATURE,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  if (CpuidRegisters.RegEax > 3) {
    RetVal = TRUE;
  }

  return RetVal;
}

/**
  Allocate data pool for MP information and fill data in it.

  @param[in] WakeUpBuffer             - The address of wakeup buffer.
  @param[in] StackAddressStart        - The start address of APs's stacks.
  @param[in] MaximumCPUsForThisSystem - Maximum CPUs in this system.

  @retval EFI_SUCCESS              - Function successfully executed.
  @retval Other                    - Error occurred while allocating memory.
**/
EFI_STATUS
FillMPData (
  OUT EFI_PHYSICAL_ADDRESS WakeUpBuffer,
  OUT VOID                 *StackAddressStart,
  IN UINTN                 MaximumCPUsForThisSystem
  )
{
  EFI_STATUS Status;

  ///
  /// First check if the MP data structures and AP rendezvous routine have been
  /// supplied by the PEIMs that executed in early boot stage.
  ///
  ///
  /// Clear the data structure area first.
  ///
  ZeroMem (mMPSystemData, sizeof (MP_SYSTEM_DATA));

  Status                        = GetMpBistStatus (mMPSystemData);

  mAcpiCpuData->CpuPrivateData  = (EFI_PHYSICAL_ADDRESS) (UINTN) (&(mMPSystemData->S3DataPointer));
  mAcpiCpuData->APState         = mPlatformCpu->CpuConfig->HtState;
  mAcpiCpuData->WakeUpBuffer    = WakeUpBuffer;
  mAcpiCpuData->StackAddress    = (EFI_PHYSICAL_ADDRESS) StackAddressStart;

  Status = PrepareGdtIdtForAP (
                  (PSEUDO_DESCRIPTOR *) (UINTN) mAcpiCpuData->GdtrProfile,
                  (PSEUDO_DESCRIPTOR *) (UINTN) mAcpiCpuData->IdtrProfile
                  );

  ///
  /// First BSP fills and inits all known values, including it's own records.
  ///
  mMPSystemData->APSerializeLock    = VacantFlag;
  mMPSystemData->NumberOfCpus       = 1;
  mMPSystemData->EnableSecondaryCpu = mPlatformCpu->CpuConfig->HtState;

  ///
  /// Record these CPU configuration data (both for normal boot and for S3 use)
  ///
  if (IsCpuidSupportOver3 ()) {
    mMPSystemData->LimitCpuidMaximumValue = mPlatformCpu->CpuConfig->LimitCpuidMaximumValue;
  } else {
    mMPSystemData->LimitCpuidMaximumValue = FALSE;
  }

  mMPSystemData->ExecuteDisableBit                = mPlatformCpu->CpuConfig->ExecuteDisableBit;
  mMPSystemData->VmxEnable                        = mPlatformCpu->CpuConfig->VmxEnable;
  mMPSystemData->TxtEnable                        = mPlatformCpu->CpuConfig->SmxEnable;
  mMPSystemData->MonitorMwaitEnable               = mPlatformCpu->CpuConfig->MonitorMwaitEnable;
  mMPSystemData->MachineCheckEnable               = mPlatformCpu->CpuConfig->MachineCheckEnable;
  mMPSystemData->XapicEnable                      = mPlatformCpu->CpuConfig->XapicEnable;
  mMPSystemData->AesEnable                        = mPlatformCpu->CpuConfig->AesEnable;
  mMPSystemData->DebugInterfaceEnable             = 0;
  mMPSystemData->DebugInterfaceLockEnable         = 1;
  if (mPlatformCpu->Revision >= DXE_PLATFORM_CPU_POLICY_PROTOCOL_REVISION_2) {
    mMPSystemData->DebugInterfaceEnable             = mPlatformCpu->CpuConfig->DebugInterfaceEnable;
    mMPSystemData->DebugInterfaceLockEnable         = mPlatformCpu->CpuConfig->DebugInterfaceLockEnable;
  }

  mMPSystemData->S3DataPointer.S3BootScriptTable  = (UINT32) (UINTN) mMPSystemData->S3BootScriptTable;
  mMPSystemData->S3DataPointer.S3BspMtrrTable     = (UINT32) (UINTN) mMPSystemData->S3BspMtrrTable;

  mMPSystemData->CpuArch                          = NULL;
  gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &mMPSystemData->CpuArch);

  mMPSystemData->MaximumCpusForThisSystem = MaximumCPUsForThisSystem;

  mMPSystemData->BSP                      = 0;

  ///
  /// Save Mtrr Register for S3 resume
  ///
  SaveBspMtrrForS3 ();

  FillInProcessorInformation (mMPSystemData, TRUE, 0);

  return EFI_SUCCESS;
}

/**
  Wake up APs for the first time to count their number and collect BIST data.

  @param[in] WakeUpBuffer      - Address of the wakeup buffer.

  @retval EFI_SUCCESS       - Function successfully finishes.
**/
EFI_STATUS
CountApNumberAndCollectBist (
  IN EFI_PHYSICAL_ADDRESS WakeUpBuffer
  )
{
  MP_CPU_EXCHANGE_INFO *ExchangeInfo;
  UINTN                Index;
  UINT64               MsrValue;
  UINT64               ProcessorThreadCount;
  UINT32               ResponseProcessorCount;
  UINTN                TimeoutTime;

  ///
  /// Send INIT IPI - SIPI to all APs
  ///
  SendInterrupt (
          BROADCAST_MODE_ALL_EXCLUDING_SELF,
          0,
          0,
          DELIVERY_MODE_INIT,
          TRIGGER_MODE_EDGE,
          TRUE
          );
  gBS->Stall (10 * STALL_ONE_MILLI_SECOND);
  SendInterrupt (
          BROADCAST_MODE_ALL_EXCLUDING_SELF,
          0,
          (UINT32) RShiftU64 (WakeUpBuffer,
                              12),
          DELIVERY_MODE_SIPI,
          TRIGGER_MODE_EDGE,
          TRUE
          );
  gBS->Stall (200 * STALL_ONE_MICRO_SECOND);
  SendInterrupt (
          BROADCAST_MODE_ALL_EXCLUDING_SELF,
          0,
          (UINT32) RShiftU64 (WakeUpBuffer,
                              12),
          DELIVERY_MODE_SIPI,
          TRIGGER_MODE_EDGE,
          TRUE
          );
  gBS->Stall (200 * STALL_ONE_MICRO_SECOND);
  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);

  ///
  /// Get thread count
  ///
  MsrValue              = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
  ProcessorThreadCount  = MsrValue & 0xffff;

  ///
  /// Only support MAXIMUM_CPU_NUMBER threads so far
  ///
  ASSERT (ProcessorThreadCount <= MAXIMUM_CPU_NUMBER);
  if (ProcessorThreadCount > MAXIMUM_CPU_NUMBER) {
    ProcessorThreadCount = MAXIMUM_CPU_NUMBER;
  }

  for (TimeoutTime = 0; TimeoutTime <= CPU_WAIT_FOR_TASK_TO_BE_COMPLETED; TimeoutTime += CPU_CHECK_AP_INTERVAL) {
    ///
    /// Wait for task to complete and then exit.
    ///
    gBS->Stall (CPU_CHECK_AP_INTERVAL);
    for (Index = 1, ResponseProcessorCount = 1; Index < MAXIMUM_CPU_NUMBER; Index++) {
      if (ExchangeInfo->BistBuffer[Index].Number == 1) {
        ResponseProcessorCount++;
      }
    }

    if (ResponseProcessorCount == ProcessorThreadCount) {
      break;
    }
  }

  for (Index = 0; Index < MAXIMUM_CPU_NUMBER; Index++) {
    if (ExchangeInfo->BistBuffer[Index].Number == 1) {
      ExchangeInfo->BistBuffer[Index].Number = (UINT32) mMPSystemData->NumberOfCpus++;
    }
  }

  mAcpiCpuData->NumberOfCpus  = (UINT32) mMPSystemData->NumberOfCpus;

  ExchangeInfo->InitFlag      = 0;

  return EFI_SUCCESS;
}

/**
  Wake up APs for the second time to collect detailed information.

  @param[in] WakeUpBuffer      - Address of the wakeup buffer.

  @retval EFI_SUCCESS       - Function successfully finishes.
**/
EFI_STATUS
PollForInitialization (
  IN EFI_PHYSICAL_ADDRESS WakeUpBuffer
  )
{
  MP_CPU_EXCHANGE_INFO *ExchangeInfo;
  UINT32               FailedRevision;
  EFI_STATUS           Status;

  ExchangeInfo              = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);

  ExchangeInfo->ApFunction  = (VOID *) (UINTN) DetailedMpInitialization;

  CpuInitFloatPointUnit ();

  ///
  /// Update microcode for BSP
  ///
  mMcuLoadCount = 0;
  Status = InitializeMicrocode (mMicrocodePointerBuffer, &FailedRevision);
  if (Status == EFI_LOAD_ERROR) {
    DEBUG((EFI_D_INFO,"Failed to load microcode patch\n"));
  }

  ///
  /// Wait until all APs finish
  ///
  while (mFinishedCount < mAcpiCpuData->NumberOfCpus - 1) {
    CpuPause ();
  }

  McuFirstLoadDone ();
  return EFI_SUCCESS;
}

/**
  Callback function to initialize MP data hub

  @param[in] Event   - Event whose notification function is being invoked.
  @param[in] Context - Pointer to the notification functions context, which is implementation dependent.
**/
VOID
EFIAPI
MpDataHubCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  CPU_DATA_FOR_DATAHUB  *CpuDataforDatahub;
  UINTN                 Index;
  EFI_STATUS            Status;
//[-start-110726-IB03600415-add]//
  VOID                    *EfiStartOfBdsDiagnostics;

  //
  // Check whether this is real gEfiStartOfBdsDiagnosticsProtocolGuid notification
  //
  Status = gBS->LocateProtocol (&gEfiStartOfBdsDiagnosticsProtocolGuid, NULL, (VOID **)&EfiStartOfBdsDiagnostics);
  if (EFI_ERROR (Status)) {
    return ;
  }
//[-end-110726-IB03600415-add]//
  
  Status = InitializeDataHubPtr (&mDataHub);
  if (EFI_ERROR (Status)) {
    return;
  }

  for (Index = 0; Index < mMPSystemData->NumberOfCpus; Index++) {
    CpuDataforDatahub = &mMPSystemData->CpuData[Index].CpuDataforDatahub;
//[-start-110701-IB03600406-add]//
   if(Index == 0) {
     FillinDataforDataHub (Index, CpuDataforDatahub);
   }
//[-end-110701-IB03600406-add]//
    UpdateDataforDatahub (Index, CpuDataforDatahub);
  }

}

/**
  Initialize multiple processors and collect MP related data

  @retval EFI_SUCCESS   - Multiple processors get initialized and data collected successfully
  @retval Other         - The operation failed and appropriate error status will be returned
**/
EFI_STATUS
InitializeMpSystemData (
  VOID
  )
{
  EFI_STATUS Status;
  UINT32     MaxThreadsPerCore;
  UINT32     MaxCoresPerDie;
  UINT32     MaxDiesPerPackage;
  UINT32     MaxPackages;

  VOID                 *StackAddressStart;
  EFI_PHYSICAL_ADDRESS WakeUpBuffer;
  MP_CPU_EXCHANGE_INFO *ExchangeInfo;
  UINTN                Index;

  EFI_CPU_ARCH_PROTOCOL *CpuArch;
  BOOLEAN               mInterruptState;
  CPU_DATA_BLOCK        *CpuData;
  UINTN                 MaximumCPUsForThisSystem;
  VOID                  *Registration;
  EFI_HANDLE            Handle;

  Handle                = NULL;

  ///
  /// Program Local APIC registers
  ///
  ProgramXApic (TRUE);

  ///
  /// Get information on threads, cores, dies and package for the CPU(s) on this platform
  ///
  Status = mPlatformCpu->CpuConfig->GetMaxCount (
                  mPlatformCpu,
                  &MaxThreadsPerCore,
                  &MaxCoresPerDie,
                  &MaxDiesPerPackage,
                  &MaxPackages
                  );
  ///
  /// Get the total CPU count
  ///
  if (!EFI_ERROR (Status)) {
    MaximumCPUsForThisSystem = MaxThreadsPerCore * MaxCoresPerDie * MaxDiesPerPackage * MaxPackages;
  } else {
    MaximumCPUsForThisSystem = MAXIMUM_CPU_NUMBER;
  }

  ///
  /// Prepare Wakeup Buffer and Stack for APs
  ///
  Status = PrepareMemoryForAPs (
                  &WakeUpBuffer,
                  &StackAddressStart,
                  MaximumCPUsForThisSystem
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mOriginalBuffer = WakeUpBuffer;
  Status          = (gBS->AllocatePages)(AllocateAnyPages, EfiBootServicesData, 1, &mBackupBuffer);

  ///
  /// Fill MP Data
  ///
  FillMPData (
          WakeUpBuffer,
          StackAddressStart,
          MaximumCPUsForThisSystem
          );

  ///
  /// Prepare exchange information for APs
  ///
  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  PrepareExchangeInfo (
          ExchangeInfo,
          StackAddressStart,
          NULL,
          WakeUpBuffer
          );

  ReportStatusCode (
          EFI_PROGRESS_CODE,
          EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_PC_AP_INIT
          );

  CpuArch = mMPSystemData->CpuArch;
  (CpuArch->GetInterruptState)(CpuArch, &mInterruptState);
  CpuArch->DisableInterrupt (CpuArch);

  ///
  /// Get BSP Performance Control setting
  ///
  mCpuPerfCtrlValue = AsmReadMsr64 (MSR_IA32_PERF_CTRL);

#ifdef BOOT_GUARD_SUPPORT_FLAG
  //
  // Disable PBET before send IPI to APs
  //
  StopPbeTimer ();
#endif

  ///
  /// First INIT-SIPI-SIPI and reset AP waking counters
  ///
  CountApNumberAndCollectBist (WakeUpBuffer);
  ExchangeInfo->WakeUpApManner = WakeUpApCounterInit;

  ///
  /// Assign AP function to initialize FPU MCU MTRR and get detail info
  ///
  PollForInitialization (WakeUpBuffer);
  ///
  /// Assign WakeUpApManner (WakeUpApPerHltLoop/WakeUpApPerMwaitLoop/WakeUpApPerRunLoop)
  ///
  ExchangeInfo->WakeUpApManner = mPlatformCpu->CpuConfig->ApIdleManner;
  ///
  /// Assign AP function to ApProcWrapper for StartAllAps/StartThisAp calling
  ///
  ExchangeInfo->ApFunction = (VOID *) (UINTN) ApProcWrapper;

  if (mInterruptState) {
    CpuArch->EnableInterrupt (CpuArch);
  }

  for (Index = 1; Index < mMPSystemData->NumberOfCpus; Index++) {
    CpuData = &mMPSystemData->CpuData[Index];
    if (CpuData->Health.Uint32 != 0) {
      ReportStatusCode (
              EFI_ERROR_MAJOR | EFI_ERROR_CODE,
              EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_SELF_TEST
              );
    }

    Status = CheckMicrocodeUpdate (Index, CpuData->MicrocodeStatus, CpuData->FailedRevision);
    if (Status == EFI_LOAD_ERROR) {
      DEBUG((EFI_D_INFO,"Failed to load microcode patch\n"));
    }
  }

  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  CheckAllAPsStatus,
                  NULL,
                  &mMPSystemData->CheckAllAPsEvent
                  );
  for (Index = 0; Index < mMPSystemData->NumberOfCpus; Index++) {
    CpuData = &mMPSystemData->CpuData[Index];
    if (Index == mMPSystemData->BSP) {
      continue;
    }
    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    CheckThisAPStatus,
                    (VOID *) CpuData,
                    &CpuData->CheckThisAPEvent
                    );
  }
  ///
  /// Switch BSP to Lowest Feature Processor (LFP)
  ///
  if (mPlatformCpu->CpuConfig->BspSelection == 16) {
    Status = SwitchToLowestFeatureProcess (&mMpService);
  }

  ///
  /// Initialize feature control structure
  ///
  InitializeFeaturePerSetup (mMPSystemData);

  ///
  /// Detect and log all processor supported features
  ///
  CollectProcessorFeature (&mMpService);
  Status = mMpService.StartupAllAPs (
                  &mMpService,
                  CollectProcessorFeature,
                  FALSE,
                  NULL,
                  0,
                  (VOID *) &mMpService,
                  NULL
                  );

  ProgramProcessorFeature (&mMpService);
  Status = mMpService.StartupAllAPs (
                  &mMpService,
                  ProgramProcessorFeature,
                  FALSE,
                  NULL,
                  0,
                  (VOID *) &mMpService,
                  NULL
                  );

  ///
  /// Install CPU info protocol
  ///
  mCpuInfo.Revision           = DXE_CPU_INFO_REVISION_1;
  mCpuInfo.RcVersion          = CPU_RC_VERSION;
  mCpuInfo.CpuCommonFeatures  = mCommonFeatures;

  gBS->InstallMultipleProtocolInterfaces (
          &Handle,
          &gDxeCpuInfoProtocolGuid,
          &mCpuInfo,
          NULL
          );
  DEBUG ((EFI_D_INFO, "mCommonFeatures = %x\n", mCommonFeatures));

  EfiCreateProtocolNotifyEvent (
//[-start-110701-IB03600406-modify]//
    &gEfiStartOfBdsDiagnosticsProtocolGuid,
//[-end-110701-IB03600406-modify]//
    TPL_CALLBACK,
    MpDataHubCallback,
    NULL,
    &Registration
    );

  CopyMem ((VOID *) (UINTN) mBackupBuffer, (VOID *) (UINTN) mOriginalBuffer, EFI_PAGE_SIZE);

  return EFI_SUCCESS;
}

/**
  Wrapper function for all procedures assigned to AP via MP service protocol.
  It controls states of AP and invokes assigned precedure.
**/
VOID
ApProcWrapper (
  VOID
  )
{
  EFI_AP_PROCEDURE     Procedure;
  VOID                 *Parameter;
  UINTN                CpuNumber;
  CPU_DATA_BLOCK       *CpuData;
  MP_CPU_EXCHANGE_INFO *ExchangeInfo;
  MONITOR_MWAIT_DATA   *MonitorAddr;

  ///
  /// Initialize MCE for CR4.
  ///
  InitializeMce (mMPSystemData->MachineCheckEnable);

  WhoAmI (&mMpService, &CpuNumber);
  CpuData = &mMPSystemData->CpuData[CpuNumber];

  ///
  /// Now let us check it out.
  ///
  Procedure = CpuData->Procedure;
  Parameter = CpuData->Parameter;

  if (Procedure != NULL) {
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuData->State = CPU_STATE_BUSY;
    AsmReleaseMPLock (&CpuData->StateLock);
    Procedure (Parameter);

    ///
    /// if BSP is switched to AP, it continue execute from here, but it carries register state
    /// of the old AP, so need to reload CpuData (might be stored in a register after compiler
    /// optimization) to make sure it points to the right data
    ///
    WhoAmI (&mMpService, &CpuNumber);
    CpuData = &mMPSystemData->CpuData[CpuNumber];

    AsmAcquireMPLock (&CpuData->ProcedureLock);
    CpuData->Procedure = NULL;
    AsmReleaseMPLock (&CpuData->ProcedureLock);

    AsmAcquireMPLock (&CpuData->StateLock);
    CpuData->State = CPU_STATE_FINISHED;
    AsmReleaseMPLock (&CpuData->StateLock);

    ///
    /// Check AP wakeup manner, update signal and relating counter once finishing AP task
    ///
    ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mAcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
    MonitorAddr = (MONITOR_MWAIT_DATA *)
            (
                    (UINT8 *) ExchangeInfo->StackStart +
                    (ExchangeInfo->BistBuffer[CpuData->ApicID].Number + 1) *
                    ExchangeInfo->StackSize -
                    MONITOR_FILTER_SIZE
            );

    switch (ExchangeInfo->WakeUpApManner) {
      case WakeUpApPerHltLoop:
        MonitorAddr->HltLoopBreakCounter += 1;
        break;

      case WakeUpApPerMwaitLoop:
        MonitorAddr->MwaitLoopBreakCounter += 1;
        break;

      case WakeUpApPerRunLoop:
        MonitorAddr->RunLoopBreakCounter += 1;
        break;

      case WakeUpApPerMwaitLoop32:
        MonitorAddr->MwaitLoopBreakCounter32 += 1;
        break;

      case WakeUpApPerRunLoop32:
        MonitorAddr->RunLoopBreakCounter32 += 1;
        break;

      default:
        break;
    }

    MonitorAddr->BreakToRunApSignal = 0;
  }
}

/**
  Procedure for detailed initialization of APs. It will be assigned to all APs
  after first INIT-SIPI-SIPI finishing CPU number counting and BIST collection.
**/
VOID
DetailedMpInitialization (
  VOID
  )
{
  EFI_STATUS Status;
  UINT32     FailedRevision;

  ///
  /// Program APs CPU Performance Control setting
  ///
  AsmWriteMsr64 (MSR_IA32_PERF_CTRL, mCpuPerfCtrlValue);

  CpuInitFloatPointUnit ();

  Status = InitializeMicrocode (
                  (EFI_CPU_MICROCODE_HEADER **) (UINTN) mAcpiCpuData->MicrocodePointerBuffer,
                  &FailedRevision
                  );

  ///
  /// Init MLC Streamer Prefetcher and MLC Spatial Prefetcher
  ///
  InitializeProcessorsPrefetcher (
          mPlatformCpu->CpuConfig->MlcStreamerPrefetcher,
          mPlatformCpu->CpuConfig->MlcSpatialPrefetcher
//[-start-130906-IB08620308-add]//
          ,
          mPlatformCpu->CpuConfig->DcuStreamerPrefetcher,
          mPlatformCpu->CpuConfig->DcuIPPrefetcher
//[-end-130906-IB08620308-add]//
          );

  ///
  /// Init Cache after all MCU is loaded
  ///
  while (mMcuLoadCount < mAcpiCpuData->NumberOfCpus) {
    CpuPause ();
  }

  ///
  /// Save Mtrr Registers in global data areas
  ///
  MpMtrrSynchUp (NULL);
  ProgramXApic (FALSE);
  FillInProcessorInformation (mMPSystemData, FALSE, 0);
  InterlockedIncrement (&mFinishedCount);
}

/**
  Switch current BSP processor to AP

  @param[in] MPSystemData  - Pointer to the data structure containing MP related data
**/
VOID
FutureBSPProc (
  IN MP_SYSTEM_DATA *MPSystemData
  )
{
  AsmExchangeRole (&MPSystemData->APInfo, &MPSystemData->BSPInfo);
  return;
}

/**
  This function is called by all processors (both BSP and AP) once and collects MP related data

  @param[in] MPSystemData  - Pointer to the data structure containing MP related data
  @param[in] BSP           - TRUE if the CPU is BSP
  @param[in] BistParam     - BIST (build-in self test) data for the processor. This data
                             is only valid for processors that are waked up for the 1ast
                             time in this CPU DXE driver.

  @retval EFI_SUCCESS   - Data for the processor collected and filled in
**/
EFI_STATUS
FillInProcessorInformation (
  IN MP_SYSTEM_DATA *MPSystemData,
  IN BOOLEAN        BSP,
  IN UINT32         BistParam
  )
{
  UINT32               Health;
  UINT32               ApicID;
  CPU_DATA_BLOCK       *CpuData;
  UINT32               BIST;
  UINTN                CpuNumber;
  UINTN                Index;
  UINTN                Count;
  MP_CPU_EXCHANGE_INFO *ExchangeInfo;

  ApicID  = GetApicID (NULL, NULL);
  BIST    = 0;

  if (BSP) {
    CpuNumber = 0;
    BIST      = BistParam;
  } else {
    ExchangeInfo  = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mAcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
    CpuNumber     = ExchangeInfo->BistBuffer[ApicID].Number;
    BIST          = ExchangeInfo->BistBuffer[ApicID].BIST;
  }

  CpuData                 = &MPSystemData->CpuData[CpuNumber];
  CpuData->SecondaryCpu   = IsSecondaryThread ();
  CpuData->ApicID         = ApicID;
  CpuData->Procedure      = NULL;
  CpuData->Parameter      = NULL;
  CpuData->StateLock      = VacantFlag;
  CpuData->ProcedureLock  = VacantFlag;
  CpuData->State          = CPU_STATE_IDLE;

  Health                  = BIST;
  Count                   = MPSystemData->BistHobSize / sizeof (BIST_HOB_DATA);
  for (Index = 0; Index < Count; Index++) {
    if (ApicID == MPSystemData->BistHobData[Index].ApicId) {
      Health = MPSystemData->BistHobData[Index].Health.Uint32;
    }
  }

  if (Health > 0) {
    CpuData->State                        = CPU_STATE_DISABLED;
    MPSystemData->DisableCause[CpuNumber] = CPU_CAUSE_SELFTEST_FAILURE;

  } else {
    MPSystemData->DisableCause[CpuNumber] = CPU_CAUSE_NOT_DISABLED;
  }

//[-start-110701-IB03600406-remove]//
  FillinDataforDataHub (CpuNumber, &CpuData->CpuDataforDatahub);
//[-end-110701-IB03600406-remove]//
  CpuData->CpuDataforDatahub.Health.Uint32 = Health;

  CopyMem (&CpuData->PhysicalLocation, &CpuData->CpuDataforDatahub.Location, sizeof (PHYSICAL_LOCATION));
  return EFI_SUCCESS;
}

/**
  Set APIC BSP bit

  @param[in] Enable  - enable as BSP or not

  @retval EFI_SUCCESS - always return success
**/
EFI_STATUS
SetApicBSPBit (
  IN BOOLEAN Enable
  )
{
  UINT64 ApicBaseReg;

  ApicBaseReg = AsmReadMsr64 (MSR_IA32_APIC_BASE);

  if (Enable) {
    ApicBaseReg |= 0x100;
  } else {
    ApicBaseReg &= 0xfffffffffffffe00;
  }

  AsmWriteMsr64 (MSR_IA32_APIC_BASE, ApicBaseReg);

  return EFI_SUCCESS;
}

/**
  Change CPU state

  @param[in] CpuNumber - CPU number
  @param[in] NewState  - the new state that will be changed to
  @param[in] Cause     - Cause

  @retval EFI_SUCCESS - always return success
**/
EFI_STATUS
ChangeCpuState (
  IN UINTN                  CpuNumber,
  IN BOOLEAN                NewState,
  IN CPU_STATE_CHANGE_CAUSE Cause
  )
{
  CPU_DATA_BLOCK                             *CpuData;
  EFI_COMPUTING_UNIT_CPU_DISABLED_ERROR_DATA ErrorData;

  CpuData = &mMPSystemData->CpuData[CpuNumber];

  mMPSystemData->DisableCause[CpuNumber] = Cause;

  if (!NewState) {
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuData->State = CPU_STATE_DISABLED;
    AsmReleaseMPLock (&CpuData->StateLock);

    ErrorData.DataHeader.HeaderSize = sizeof (EFI_STATUS_CODE_DATA);
    ErrorData.DataHeader.Size       = sizeof (EFI_COMPUTING_UNIT_CPU_DISABLED_ERROR_DATA) - sizeof (EFI_STATUS_CODE_DATA);
    CopyMem (
            &ErrorData.DataHeader.Type,
            &gEfiStatusCodeSpecificDataGuid,
            sizeof (EFI_GUID)
            );
    ErrorData.Cause             = Cause;
    ErrorData.SoftwareDisabled  = TRUE;
    ReportStatusCode (
            EFI_ERROR_MINOR | EFI_ERROR_CODE,
            EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_EC_DISABLED
            );
  } else {
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuData->State = CPU_STATE_IDLE;
    AsmReleaseMPLock (&CpuData->StateLock);
  }

  return EFI_SUCCESS;
}

/**
  Check if this is non-core processor - HT AP thread

  @retval TRUE if this is HT AP thread
  @retval FALSE if this is core thread
**/
BOOLEAN
IsSecondaryThread (
  VOID
  )
{
  UINT32             ApicID;
  EFI_CPUID_REGISTER CpuidRegisters;
  UINT8              CpuCount;
  UINT8              CoreCount;
  UINT8              CpuPerCore;
  UINT32             Mask;

  ApicID = GetApicID (NULL, NULL);

  AsmCpuid (
          CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  if ((CpuidRegisters.RegEdx & 0x10000000) == 0) {
    return FALSE;
  }

  CpuCount = (UINT8) ((CpuidRegisters.RegEbx >> 16) & 0xff);
  if (CpuCount == 1) {
    return FALSE;
  }

  AsmCpuid (
          CPUID_SIGNATURE,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  if (CpuidRegisters.RegEax > 3) {

    CoreCount = GetCoreNumber ();
  } else {
    CoreCount = 1;
  }

  ///
  /// Assumes there is symmetry across core boundary, i.e. each core within a package has the same number of logical processors
  ///
  if (CpuCount == CoreCount) {
    return FALSE;
  }

  CpuPerCore = CpuCount / CoreCount;

  ///
  /// Assume 1 Core  has no more than 8 threads
  ///
  if (CpuPerCore == 2) {
    Mask = 0x1;
  } else if (CpuPerCore <= 4) {
    Mask = 0x3;
  } else {
    Mask = 0x7;
  }

  if ((ApicID & Mask) == 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  If timeout occurs in StartupAllAps(), a timer is set, which invokes this
  procedure periodically to check whether all APs have finished.

  @param[in] Event   - Event triggered.
  @param[in] Context - Parameter passed with the event.
**/
VOID
CheckAllAPsStatus (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  UINTN          CpuNumber;
  UINTN          NextCpuNumber;
  CPU_DATA_BLOCK *CpuData;
  CPU_DATA_BLOCK *NextCpuData;
  EFI_STATUS     Status;
  CPU_STATE      CpuState;

  for (CpuNumber = 0; CpuNumber < mMPSystemData->NumberOfCpus; CpuNumber++) {
    CpuData = &mMPSystemData->CpuData[CpuNumber];
    if (CpuNumber == mMPSystemData->BSP) {
      continue;
    }

    AsmAcquireMPLock (&CpuData->StateLock);
    CpuState = CpuData->State;
    AsmReleaseMPLock (&CpuData->StateLock);

    switch (CpuState) {
      case CPU_STATE_READY:
        WakeUpAp (
                CpuData,
                mMPSystemData->Procedure,
                mMPSystemData->ProcArguments
                );
        break;

      case CPU_STATE_FINISHED:
        if (mMPSystemData->SingleThread) {
          Status = GetNextBlockedCpuNumber (&NextCpuNumber);
          if (!EFI_ERROR (Status)) {
            NextCpuData = &mMPSystemData->CpuData[NextCpuNumber];

            AsmAcquireMPLock (&NextCpuData->StateLock);
            NextCpuData->State = CPU_STATE_READY;
            AsmReleaseMPLock (&NextCpuData->StateLock);

            WakeUpAp (
                    NextCpuData,
                    mMPSystemData->Procedure,
                    mMPSystemData->ProcArguments
                    );
          }
        }

        AsmAcquireMPLock (&CpuData->StateLock);
        CpuData->State = CPU_STATE_IDLE;
        AsmReleaseMPLock (&CpuData->StateLock);

        mMPSystemData->FinishCount++;
        break;

      default:
        break;
    }
  }

  if (mMPSystemData->FinishCount == mMPSystemData->StartCount) {
    gBS->SetTimer (
            mMPSystemData->CheckAllAPsEvent,
            TimerCancel,
            0
            );
    Status = gBS->SignalEvent (mMPSystemData->WaitEvent);
  }

  return;
}

/**
  Check if this AP has finished task

  @param[in] Event   - Event triggered.
  @param[in] Context - Parameter passed with the event.
**/
VOID
CheckThisAPStatus (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  CPU_DATA_BLOCK *CpuData;
  EFI_STATUS     Status;
  CPU_STATE      CpuState;

  CpuData = (CPU_DATA_BLOCK *) Context;

  AsmAcquireMPLock (&CpuData->StateLock);
  CpuState = CpuData->State;
  AsmReleaseMPLock (&CpuData->StateLock);

  if (CpuState == CPU_STATE_FINISHED) {
    gBS->SetTimer (
          CpuData->CheckThisAPEvent,
          TimerCancel,
          0
          );
    Status = gBS->SignalEvent (mMPSystemData->WaitEvent);
    AsmAcquireMPLock (&CpuData->StateLock);
    CpuData->State = CPU_STATE_IDLE;
    AsmReleaseMPLock (&CpuData->StateLock);
  }

  return;
}

/**
  Convert the timeout value to TSC value

  @param[in] TimeoutInMicroSecs  - how many microseconds the timeout is

  @retval expected TSC value for timeout
**/
UINT64
CalculateTimeout (
  IN UINTN TimeoutInMicroSecs
  )
{
  UINT64     CurrentTsc;
  UINT64     ExpectedTsc;
  UINT64     Frequency;
  EFI_STATUS Status;

  if (TimeoutInMicroSecs == 0) {
    return 0xffffffffffff;
  }

  CurrentTsc  = AsmReadTsc ();

  Status      = GetActualFrequency (mMetronome, &Frequency);

  ExpectedTsc = CurrentTsc + MultU64x32 (Frequency, (UINT32) TimeoutInMicroSecs);

  return ExpectedTsc;
}

/**
  Check if timeout happened

  @param[in] ExpectedTsc - the TSC value for timeout

  @retval TRUE if timeout happened
  @retval FALSE if timeout not yet happened
**/
BOOLEAN
CheckTimeout (
  IN UINT64 ExpectedTsc
  )
{
  UINT64 CurrentTsc;

  CurrentTsc = AsmReadTsc ();
  if (CurrentTsc >= ExpectedTsc) {
    return TRUE;
  }

  return FALSE;
}

/**
  Get the next blocked processor

  @param[in] NextCpuNumber - that will be updated for next blocked CPU number

  @retval EFI_SUCCESS - The next blocked CPU found
  @retval EFI_NOT_FOUND - cannot find blocked CPU
**/
EFI_STATUS
GetNextBlockedCpuNumber (
  OUT UINTN *NextCpuNumber
  )
{
  UINTN          CpuNumber;
  CPU_STATE      CpuState;
  CPU_DATA_BLOCK *CpuData;

  for (CpuNumber = 0; CpuNumber < mMPSystemData->NumberOfCpus; CpuNumber++) {
    if (CpuNumber == mMPSystemData->BSP) {
      continue;
    }

    CpuData = &mMPSystemData->CpuData[CpuNumber];

    AsmAcquireMPLock (&CpuData->StateLock);
    CpuState = CpuData->State;
    AsmReleaseMPLock (&CpuData->StateLock);

    if (CpuState == CPU_STATE_BLOCKED) {
      *NextCpuNumber = CpuNumber;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Update data hub for processor status

  @param[in] CpuNumber         - CPU number
  @param[in] CpuDataforDatahub - the data hub that will be updated
**/
VOID
UpdateDataforDatahub (
  IN UINTN                 CpuNumber,
  OUT CPU_DATA_FOR_DATAHUB *CpuDataforDatahub
  )
{
  CpuDataforDatahub->Status = GetProcessorStatus (CpuNumber);
  InitializeProcessorData (CpuNumber, CpuDataforDatahub);
  InitializeCacheData (CpuNumber, CpuDataforDatahub->CacheInformation);

  return;
}

/**
  Function to wake up a specified AP and assign procedure to it.

  @param[in] CpuData       - CPU data block for the specified AP.
  @param[in] Procedure     - Procedure to assign.
  @param[in] ProcArguments - Argument for Procedure.
**/
VOID
WakeUpAp (
  IN CPU_DATA_BLOCK   *CpuData,
  IN EFI_AP_PROCEDURE Procedure,
  IN VOID             *ProcArguments
  )
{
  MP_CPU_EXCHANGE_INFO *ExchangeInfo;
  MONITOR_MWAIT_DATA   *MonitorAddr;

  AsmAcquireMPLock (&CpuData->ProcedureLock);
  CpuData->Parameter  = ProcArguments;
  CpuData->Procedure  = Procedure;
  AsmReleaseMPLock (&CpuData->ProcedureLock);

  ExchangeInfo = (MP_CPU_EXCHANGE_INFO *) (UINTN) (mAcpiCpuData->WakeUpBuffer + MP_CPU_EXCHANGE_INFO_OFFSET);
  MonitorAddr = (MONITOR_MWAIT_DATA *)
          (
                  (UINT8 *) ExchangeInfo->StackStart +
                  (ExchangeInfo->BistBuffer[CpuData->ApicID].Number + 1) *
                  ExchangeInfo->StackSize -
                  MONITOR_FILTER_SIZE
          );

  if (MonitorAddr->WakeUpApVectorChangeFlag == TRUE || ExchangeInfo->WakeUpApManner == WakeUpApPerHltLoop) {
    SendInterrupt (
            BROADCAST_MODE_SPECIFY_CPU,
            CpuData->ApicID,
            0,
            DELIVERY_MODE_INIT,
            TRIGGER_MODE_EDGE,
            TRUE
            );
    SendInterrupt (
            BROADCAST_MODE_SPECIFY_CPU,
            CpuData->ApicID,
            (UINT32) RShiftU64 (mAcpiCpuData->WakeUpBuffer,
                                12),
            DELIVERY_MODE_SIPI,
            TRIGGER_MODE_EDGE,
            TRUE
            );
    MonitorAddr->WakeUpApVectorChangeFlag = FALSE;

    //
    // Clear StateLock to 0 to avoid AP locking it then entering SMM and getting INIT-SIPI here could cause dead-lock
    //
    CpuData->StateLock = 0;
  }

  MonitorAddr->BreakToRunApSignal = (UINTN) (BREAK_TO_RUN_AP_SIGNAL | CpuData->ApicID);
}

/**
  Check whether any AP is running for assigned task.

  @retval TRUE  - Some APs are running.
  @retval FALSE - No AP is running.
**/
BOOLEAN
ApRunning (
  VOID
  )
{
  CPU_DATA_BLOCK *CpuData;
  UINTN          CpuNumber;

  for (CpuNumber = 0; CpuNumber < mMPSystemData->NumberOfCpus; CpuNumber++) {
    CpuData = &mMPSystemData->CpuData[CpuNumber];

    if (CpuNumber != mMPSystemData->BSP) {
      if (CpuData->State == CPU_STATE_READY || CpuData->State == CPU_STATE_BUSY) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
  Re-load microcode patch before boot.

  @param[in] MpServices - point to EFI_MP_SERVICES_PROTOCOL

  @retval EFI_SUCCESS           - one processor re-loads microcode patch
**/
EFI_STATUS
ReLoadMicrocodePatch (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL     *MpServices
  )
{
  UINT32     FailedRevision;
  UINTN      CpuNumber;
  EFI_STATUS Status;

  Status = InitializeMicrocode (
                  mMicrocodePointerBuffer,
                  &FailedRevision
                  );
  WhoAmI (
          &mMpService,
          &CpuNumber
          );
  Status = CheckMicrocodeUpdate (
                  CpuNumber,
                  Status,
                  FailedRevision
                  );
  return Status;
}

/**
  Re-load microcode patch before boot.

  @retval EFI_SUCCESS           - Multiple processors re-load microcode patch
**/
EFI_STATUS
ReLoadMicrocodeBeforeBoot (
  VOID
  )
{
  EFI_STATUS Status;

  ///
  /// Added code to handle microcode patch reload here!!!
  ///
  mMcuLoadCount = 0;
  Status = ReLoadMicrocodePatch (&mMpService);
  if (!EFI_ERROR (Status)) {
    Status = mMpService.StartupAllAPs (
                    &mMpService,
                    ReLoadMicrocodePatch,
                    FALSE,
                    NULL,
                    0,
                    (VOID *) &mMpService,
                    NULL
                    );
  }
  return Status;
}
