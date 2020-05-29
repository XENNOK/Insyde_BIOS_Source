/** @file

  Digital Thermal Sensor (DTS) driver.
  This SMM driver configures and supports the Digital Thermal Sensor features for the platform.
  
;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#include <DigitalThermalSensorSmm.h>
#include <KscLib.h>
//
// Protocol GUID definition
//
DXE_CPU_PLATFORM_POLICY_PROTOCOL  *mPlatformCpu                     = NULL;
DTS_INIT_STATUS_PROTOCOL mDtsInitStatusProtocol;

//
// Global variables
//
EFI_SMM_BASE_PROTOCOL *mSmmBase;
EFI_SMM_SYSTEM_TABLE  *mSmst;

EFI_GLOBAL_NVS_AREA *mGlobalNvsAreaPtr;
UINT8               DTSSetupValue;

BOOLEAN mDtsEnabled;
UINT8   mDtsTjMax;
UINT16  mAcpiBaseAddr;
BOOLEAN mUpdateDtsInEverySmi;
UINT8   mNoOfThresholdRanges;
UINT8                             (*mDtsThresholdTable)[3];
UINT8 gIsPackageTempMsrAvailable;
///
/// The table is updated for the current CPU.
///
UINT8 mDigitalThermalSensorThresholdTable[DTS_NUMBER_THRESHOLD_RANGES][3] = {
  ///
  /// TJ_MAX = 110                ///< Current Temp.  Low Temp. High Temp.
  ///
  {TJ_MAX-80,100,75},     ///<    <= 30            10       35
  {TJ_MAX-70,85,65},      ///< 30  ~ 39            25       45
  {TJ_MAX-60,75,55},      ///< 40  ~ 49            35       55
  {TJ_MAX-50,65,45},      ///< 50  ~ 59            45       65
  {TJ_MAX-40,55,35},      ///< 60  ~ 69            55       75
  {TJ_MAX-30,45,25},      ///< 70  ~ 79            65       85
  {TJ_MAX-20,35,15},      ///< 80  ~ 89            75       95
  {TJ_MAX-10,25,05},      ///< 90  ~ 99            85       105
  {TJ_MAX-00,15,00}       ///< 100 ~ 109           95       110
};

//
// Function implementations
//
/**
  Read the temperature data per core/thread.
  This function must be AP safe.

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature

  @retval EFI_SUCCESS   Digital Thermal Sensor temperature has updated successfully.
**/
VOID
EFIAPI
DigitalThermalSensorUpdateTemperature (
  VOID *Buffer
  )
{
  MSR_REGISTER       MsrData;
  UINT8              Temperature;
  UINT8              DefApicId;
  EFI_CPUID_REGISTER CpuidRegisters;
  UINT8              *TempPointer;

  AsmCpuid (
    CPUID_VERSION_INFO,
          &CpuidRegisters.RegEax,
          &CpuidRegisters.RegEbx,
          &CpuidRegisters.RegEcx,
          &CpuidRegisters.RegEdx
          );
  ///
  /// Default APIC ID = CPUID Function 01, EBX[31:24]
  ///
  DefApicId = (UINT8) RShiftU64 (CpuidRegisters.RegEbx, 24);

  ///
  /// Read the temperature
  ///
  MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);

  ///
  /// Find the DTS temperature.
  ///
  Temperature = mDtsTjMax - (MsrData.Bytes.ThirdByte & OFFSET_MASK);
  ///
  /// We only update the temperature if it is above the current temperature.
  ///
  TempPointer = Buffer;
  if (Temperature > *((UINT8 *) (TempPointer + DefApicId))) {
    *((UINT8 *) (TempPointer + DefApicId)) = Temperature;
  }

  return;
}

/**
  Read the temperature and update the data for PTID support.

  @retval EFI_SUCCESS   Digital Thermal Sensor temperature has updated successfully.
**/
EFI_STATUS
DigitalThermalSensorUpdatePTID (
  VOID
  )
{
  ///
  /// ThreadTemperatureBuffer[0], [2], [4] and [6] are Thread0 and [1], [3], [5] and [7] are Thread1 for each Core.
  /// If thread or core not active, this thread/core temperature will be reported as 0
  ///
  UINT8 ThreadTemperatureBuffer[MAX_NUMBER_OF_THREADS_SUPPORTED];
  UINT8 Index;
  UINT8 CoreTemp[MAX_NUMBER_OF_THREADS_SUPPORTED / 2];

  ///
  /// Get DTS temperature for all cores/threads.
  ///
  for (Index = 0; Index < MAX_NUMBER_OF_THREADS_SUPPORTED; Index++) {
    ThreadTemperatureBuffer[Index] = 0;
  }

  RunOnAllLogicalProcessors (DigitalThermalSensorUpdateTemperature, ThreadTemperatureBuffer);
  ///
  /// Compare thread1 and thread2 temperature in each core and only report higher temperature as core temperature.
  ///
  for (Index = 0; Index < MAX_NUMBER_OF_THREADS_SUPPORTED; Index += 2) {
    CoreTemp[(UINT8) DivU64x32 (Index, 2)] =
            (ThreadTemperatureBuffer[Index] > ThreadTemperatureBuffer[Index + 1])
            ? ThreadTemperatureBuffer[Index] : ThreadTemperatureBuffer[Index + 1];
  }

  mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = CoreTemp[0];
  if ((MAX_NUMBER_OF_THREADS_SUPPORTED / 2) > 1) {
    mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature = CoreTemp[1];
  }

  if ((MAX_NUMBER_OF_THREADS_SUPPORTED / 2) > 2) {
    mGlobalNvsAreaPtr->Ap2DigitalThermalSensorTemperature = CoreTemp[2];
  }

  if ((MAX_NUMBER_OF_THREADS_SUPPORTED / 2) > 3) {
    mGlobalNvsAreaPtr->Ap3DigitalThermalSensorTemperature = CoreTemp[3];
  }

  return EFI_SUCCESS;
}

/**
  SMI handler to handle Digital Thermal Sensor CPU Local APIC SMI
  for thermal Out Of Spec interrupt

  @param[in] SmmImageHandle        Image handle returned by the SMM driver.
  @param[in] CommunicationBuffer   Pointer to the buffer that contains the communication Message
  @param[in] SourceSize            Size of the memory image to be used for handler.

  @retval EFI_SUCCESS           Callback Function Executed
**/
EFI_STATUS
EFIAPI
DtsOutOfSpecSmiCallback (
  IN EFI_HANDLE SmmImageHandle,
  IN OUT VOID   *CommunicationBuffer,
  IN OUT UINTN  *SourceSize
  )
{

  DTS_EVENT_TYPE EventType;

  ///
  /// If not enabled; return.  (The DTS will be disabled upon S3 entry
  /// and will remain disabled until after re-initialized upon wake.)
  ///
  if (!mDtsEnabled) {
    return EFI_SUCCESS;
  }

  EventType = DtsEventNone;

  if (gIsPackageTempMsrAvailable) {
    ///
    /// Get the Package DTS Event Type
    ///
    DigitalThermalSensorEventCheckPackageMsr (&EventType);
  } else {
    ///
    /// Get the DTS Event Type
    ///
    DigitalThermalSensorEventCheck (&EventType);
  }
  ///
  /// Check if this a DTS Out Of Spec SMI event
  ///
  if (EventType == DtsEventOutOfSpec) {
    ///
    /// Return Critical temperature value to _TMP and generate GPE event for critical shutdown.
    ///
    mGlobalNvsAreaPtr->EnableDigitalThermalSensor = DTS_OUT_OF_SPEC_OCCURRED;

    ///
    /// Generate SCI to shut down the system
    ///
    DigitalThermalSensorSetSwGpeSts ();
  }

  return EFI_SUCCESS;
}

/**
  Call from SMI handler to handle Package thermal temperature Digital Thermal Sensor CPU Local APIC SMI
  for thermal threshold interrupt

  @retval None
**/
VOID
PackageThermalDTS (
  VOID
  )
{
  DTS_EVENT_TYPE PkgEventType;

  PkgEventType = DtsEventNone;

  ///
  /// Check is this a Platform SMI event or the flag of update DTS temperature and threshold value in every SMI
  ///
  if (DigitalThermalSensorEventCheckPackageMsr (&PkgEventType) || mUpdateDtsInEverySmi) {
    ///
    /// Disable Local APIC SMI before programming the threshold
    ///
    RunOnAllLogicalProcessors (DigitalThermalSensorDisableSmi, NULL);

    do {
      ///
      /// Handle Package events
      ///
      if ((PkgEventType == DtsEventOutOfSpec) && (mGlobalNvsAreaPtr->OperatingSystem == 0)) {
        ///
        /// Handle critical event by shutting down via EC if ACPI
        /// is not enabled.
        ///
        PlatformEventOutOfSpec ();
      }
      ///
      /// Set the thermal trip toints as needed.
      ///
      mGlobalNvsAreaPtr->PackageDTSTemperature = 0;

      ///
      /// Set the Package thermal sensor thresholds
      ///
      PackageDigitalThermalSensorSetThreshold (&mGlobalNvsAreaPtr->PackageDTSTemperature);

      ///
      /// Set SWGPE Status to generate an SCI if we had any events
      ///
      if ((PkgEventType != DtsEventNone) || mUpdateDtsInEverySmi) {
        DigitalThermalSensorSetSwGpeSts ();
      }

    } while (DigitalThermalSensorEventCheckPackageMsr (&PkgEventType));
    ///
    /// Enable Local APIC SMI on all logical processors
    ///
    RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);
  }
}

/**
  SMI handler to handle Digital Thermal Sensor CPU Local APIC SMI
  for thermal threshold interrupt

  @param[in] SmmImageHandle        Image handle returned by the SMM driver.
  @param[in] CommunicationBuffer   Pointer to the buffer that contains the communication Message
  @param[in] SourceSize            Size of the memory image to be used for handler.

  @retval EFI_SUCCESS           Callback Function Executed
**/
EFI_STATUS
EFIAPI
DtsSmiCallback (
  IN EFI_HANDLE SmmImageHandle,
  IN OUT VOID   *CommunicationBuffer,
  IN OUT UINTN  *SourceSize
  )
{
  UINTN          Index;
  DTS_EVENT_TYPE EventType;
  ///
  /// If not enabled; return.  (The DTS will be disabled upon S3 entry
  /// and will remain disabled until after re-initialized upon wake.)
  ///
  if (!mDtsEnabled) {
    return EFI_SUCCESS;
  }
  ///
  /// Get the Package thermal temperature
  ///
  if (gIsPackageTempMsrAvailable) {
    RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);
    PackageThermalDTS ();
  } else {
    ///
    /// We enable the Thermal interrupt on the AP's prior to the event check
    /// for the case where the AP has gone through the INIT-SIPI-SIPI sequence
    /// and does not have the interrupt enabled.  (This allows the AP thermal
    /// interrupt to be re-enabled due to chipset-based SMIs without waiting
    /// to receive a DTS event on the BSP.)
    ///
    for (Index = 1; Index < mSmst->NumberOfCpus; Index++) {
      RunOnSpecificLogicalProcessor (DigitalThermalSensorEnableSmi, Index, NULL);
    }
    ///
    /// Check is this a DTS SMI event or the flag of update DTS temperature and threshold value in every SMI
    ///
    if (DigitalThermalSensorEventCheck (&EventType) || mUpdateDtsInEverySmi) {
      ///
      /// Disable Local APIC SMI before programming the threshold
      ///
      RunOnAllLogicalProcessors (DigitalThermalSensorDisableSmi, NULL);

      do {
        ///
        /// Handle BSP events
        ///
        if ((EventType == DtsEventOutOfSpec) && (mGlobalNvsAreaPtr->OperatingSystem == 0)) {
          ///
          /// Handle critical event by shutting down via EC if ACPI
          /// is not enabled.
          ///
          PlatformEventOutOfSpec ();
        }
        ///
        /// Update temperatures for PTID
        ///
        DigitalThermalSensorUpdatePTID ();

        ///
        /// Set the thermal trip toints as needed.
        /// Note:  We only save the highest temperature of each die in
        /// the NVS area when more than two logical processors are
        /// present as only the highest DTS reading is actually used by
        /// the current ASL solution.
        ///
        mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = 0;
        mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature  = 0;

        ///
        /// Set the BSP thermal sensor thresholds
        ///
        DigitalThermalSensorSetThreshold (&mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature);

        ///
        /// Set the AP thermal sensor thresholds and update temperatures
        ///
        for (Index = 1; Index < mSmst->NumberOfCpus / 2; Index++) {
          RunOnSpecificLogicalProcessor (
                  DigitalThermalSensorSetThreshold,
                  Index,
                  &mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature
                  );
        }

        for (Index = mSmst->NumberOfCpus / 2; Index < mSmst->NumberOfCpus; Index++) {
          RunOnSpecificLogicalProcessor (
                  DigitalThermalSensorSetThreshold,
                  Index,
                  &mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature
                  );
        }
        ///
        /// Set SWGPE Status to generate an SCI if we had any events
        ///
        if ((EventType != DtsEventNone) || mUpdateDtsInEverySmi) {
          DigitalThermalSensorSetSwGpeSts ();
        }

      } while (DigitalThermalSensorEventCheck (&EventType));
      ///
      /// Enable Local APIC SMI on all logical processors
      ///
      RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);
    }
  }

  return EFI_SUCCESS;
}

/**
  This catches IO trap SMI generated by the ASL code to enable the DTS AP function

  @param[in] DispatchHandle      Not used
  @param[in] CallbackContext     Not used
**/
VOID
EFIAPI
DtsIoTrapCallback (
  IN EFI_HANDLE                                DispatchHandle,
  IN EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT *CallbackContext
  )
{
  UINTN      Index;
  EFI_STATUS Status;
  UINT8      RetryIteration;

  ///
  /// Determine the function desired, passed in the global NVS area
  ///
  switch (mGlobalNvsAreaPtr->DigitalThermalSensorSmiFunction) {
    ///
    /// Enable AP Digital Thermal Sensor function after resume from S3
    ///
    case IO_TRAP_INIT_DTS_FUNCTION_AFTER_S3:
      ///
      /// Enable the Package DTS on the processors.
      ///
      if (gIsPackageTempMsrAvailable) {
        PackageDigitalThermalSensorEnable (NULL);
      } else {
        ///
        /// Enable the DTS on all logical processors.
        ///
        RunOnAllLogicalProcessors (DigitalThermalSensorEnable, NULL);
      }

      if (DTSSetupValue != DTS_OUT_OF_SPEC_ONLY) {
        ///
        /// Set the thermal trip toints on all logical processors.
        /// Note:  We only save the highest temperature of each die in the NVS area when
        /// more than two logical processors are present as only the highest DTS reading
        /// is actually used by the current ASL solution.
        ///
        mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = 0;
        mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature  = 0;
        mGlobalNvsAreaPtr->PackageDTSTemperature              = 0;

        if (gIsPackageTempMsrAvailable) {
          PackageDigitalThermalSensorSetThreshold (&mGlobalNvsAreaPtr->PackageDTSTemperature);
        } else {
          ///
          /// Update temperatures for PTID
          ///
          DigitalThermalSensorUpdatePTID ();
          mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = 0;
          mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature  = 0;
  DigitalThermalSensorSetThreshold (&mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature);

          for (Index = 1; Index < mSmst->NumberOfCpus / 2; Index++) {
            Status = EFI_NOT_READY;
            for (RetryIteration = 0;
                 (RetryIteration < DTS_AP_SAFE_RETRY_LIMIT) && (Status != EFI_SUCCESS);
                 RetryIteration++
                 ) {
  Status = mSmst->SmmStartupThisAp (
                              DigitalThermalSensorSetThreshold,
                              Index,
                              &mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature
                              );
              if (Status != EFI_SUCCESS) {
                ///
                /// SmmStartupThisAp might return failure if AP is busy executing some other code. Let's wait for sometime and try again.
                ///
                PchPmTimerStall (DTS_WAIT_PERIOD);
              }
            }
          }

          for (Index = mSmst->NumberOfCpus / 2; Index < mSmst->NumberOfCpus; Index++) {
            RunOnSpecificLogicalProcessor (
                    DigitalThermalSensorSetThreshold,
                    Index,
                    &mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature
                    );
          }
        }
        ///
        /// Re-enable the DTS.
        ///
        mGlobalNvsAreaPtr->EnableDigitalThermalSensor = CPU_FEATURE_ENABLE;
      } else {
        ///
        /// Enable Out Of Spec Interrupt
        ///
        if (gIsPackageTempMsrAvailable) {
          PackageDigitalThermalSensorSetOutOfSpecInterrupt (NULL);
        } else {
          RunOnAllLogicalProcessors (DigitalThermalSensorSetOutOfSpecInterrupt, NULL);
        }
        ///
        /// Re-enable the DTS which only handle Out-Of-Spec condition
        ///
        mGlobalNvsAreaPtr->EnableDigitalThermalSensor = DTS_OUT_OF_SPEC_ONLY;
      }
      ///
      /// Enable the Local APIC SMI on all logical processors
      ///
      RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);
      ///
      /// Set SWGPE Status
      ///
      DigitalThermalSensorSetSwGpeSts ();

      mUpdateDtsInEverySmi  = UPDATE_DTS_EVERY_SMI;
      mDtsEnabled           = TRUE;
      break;

    ///
    /// Disable update DTS temperature and threshold value in every SMI
    ///
    case IO_TRAP_DISABLE_UPDATE_DTS:
      mUpdateDtsInEverySmi = FALSE;
      break;

    default:
      break;
  }
  ///
  /// Store return value
  ///
  mGlobalNvsAreaPtr->DigitalThermalSensorSmiFunction = 0;
}

/**
  This function executes DTS procedures for preparing to enter S3.

  @param[in] Handle    Handle of the callback
  @param[in] Context   The dispatch context

  @retval EFI_SUCCESS        DTS disabled
**/
VOID
EFIAPI
DtsS3EntryCallBack (
  IN EFI_HANDLE                  Handle,
  IN EFI_SMM_SX_DISPATCH_CONTEXT *Context
  )
{
  ///
  /// Clear the Digital Thermal Sensor flag in ACPI NVS.
  ///
  mGlobalNvsAreaPtr->EnableDigitalThermalSensor = CPU_FEATURE_DISABLE;
  ///
  /// Clear the enable flag.
  ///
  mDtsEnabled = FALSE;

  return;
}

/**
  Performs initialization of the threshold table.

  @todo Update this function as necessary for the tables used by the implementation.

  @retval EFI_SUCCESS  Threshold tables initialized successfully.
**/
EFI_STATUS
ThresholdTableInit (
  VOID
  )
{
  UINTN i;
  UINT8 Delta;

  ///
  /// If the table must be updated, shift the thresholds by the difference between
  /// TJ_MAX=110 and DtsTjMax.
  ///
  if (mDtsTjMax != TJ_MAX) {
    Delta = TJ_MAX - mDtsTjMax;

    for (i = 0; i < mNoOfThresholdRanges; i++) {
      if (mDtsThresholdTable[i][1] <= mDtsTjMax) {
        mDtsThresholdTable[i][0] = mDtsThresholdTable[i][0] - Delta;
      } else {
        mDtsThresholdTable[i][0] = 0;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Perform first time initialization of the Digital Thermal Sensor

  @retval EFI_SUCCESS  Init Digital Thermal Sensor successfully
**/
EFI_STATUS
DigitalThermalSensorInit (
  VOID
  )
{
  UINTN Index;

  if (DTSSetupValue != DTS_OUT_OF_SPEC_ONLY) {
    ///
    /// Initialize the DTS threshold table.
    ///
    ThresholdTableInit ();

    ///
    /// Set the thermal trip points on all logical processors.
    /// Note:  We only save the highest temperature of each die in the NVS area when
    /// more than two logical processors are present as only the highest DTS reading
    /// is actually used by the current ASL solution.
    ///
    mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = 0;
    mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature  = 0;
    mGlobalNvsAreaPtr->PackageDTSTemperature              = 0;
    if (gIsPackageTempMsrAvailable) {
      PackageDigitalThermalSensorSetThreshold (&mGlobalNvsAreaPtr->PackageDTSTemperature);
    } else {
      ///
      /// Update temperatures for PTID
      ///
      DigitalThermalSensorUpdatePTID ();
      mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature = 0;
      mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature  = 0;
      DigitalThermalSensorSetThreshold (&mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature);

      for (Index = 1; Index < mSmst->NumberOfCpus / 2; Index++) {
        RunOnSpecificLogicalProcessor (
                DigitalThermalSensorSetThreshold,
                Index,
                &mGlobalNvsAreaPtr->BspDigitalThermalSensorTemperature
                );
      }

      for (Index = mSmst->NumberOfCpus / 2; Index < mSmst->NumberOfCpus; Index++) {
        RunOnSpecificLogicalProcessor (
                DigitalThermalSensorSetThreshold,
                Index,
                &mGlobalNvsAreaPtr->ApDigitalThermalSensorTemperature
                );
      }
    }

    mGlobalNvsAreaPtr->EnableDigitalThermalSensor = CPU_FEATURE_ENABLE;
  } else {
    ///
    /// Enable Out Of Spec Interrupt
    ///
    if (gIsPackageTempMsrAvailable) {
      PackageDigitalThermalSensorSetOutOfSpecInterrupt (NULL);
    } else {
      RunOnAllLogicalProcessors (DigitalThermalSensorSetOutOfSpecInterrupt, NULL);
    }

    mGlobalNvsAreaPtr->EnableDigitalThermalSensor = DTS_OUT_OF_SPEC_ONLY;
  }
  ///
  /// Enable the Local APIC SMI on all logical processors
  ///
  RunOnAllLogicalProcessors (DigitalThermalSensorEnableSmi, NULL);
  ///
  /// Set Digital Thermal Sensor flag in ACPI NVS
  ///
  mUpdateDtsInEverySmi  = UPDATE_DTS_EVERY_SMI;
  mDtsEnabled           = TRUE;

  return EFI_SUCCESS;
}

/**
  Initializes the Thermal Sensor Control MSR

  This function must be AP safe.

  @param[in] Buffer        Unused.

  @retval EFI_SUCCESS   The function completed successfully.
**/
VOID
EFIAPI
DigitalThermalSensorEnable (
  VOID *Buffer
  )
{
  MSR_REGISTER MsrData;

  ///
  /// First, clear our log bits
  ///
  MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
  if (DTSSetupValue != DTS_OUT_OF_SPEC_ONLY) {
    MsrData.Qword &= ~THERM_STATUS_LOG_MASK;
  } else {
    MsrData.Qword &= ~B_OUT_OF_SPEC_STATUS_LOG;
  }

  AsmWriteMsr64 (MSR_IA32_THERM_STATUS, MsrData.Qword);

  ///
  /// Second, configure the thermal sensor control
  ///
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_MISC_PWR_MGMT);

  ///
  /// Only lock interrupts if in CMP mode
  ///
  if (mSmst->NumberOfCpus > 1) {
    MsrData.Qword |= B_LOCK_THERMAL_INT;
  }

  AsmWriteMsr64 (EFI_MSR_MISC_PWR_MGMT, MsrData.Qword);

  return;
}

/**
  Initializes the Package Thermal Sensor Control MSR

  @param[in] Buffer        Unused.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
PackageDigitalThermalSensorEnable (
  VOID *Buffer
  )
{
  MSR_REGISTER MsrData;

  ///
  /// First, clear our log bits
  ///
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS);
  if (DTSSetupValue != DTS_OUT_OF_SPEC_ONLY) {
    MsrData.Qword &= ~THERM_STATUS_LOG_MASK;
  } else {
    MsrData.Qword &= ~B_OUT_OF_SPEC_STATUS_LOG;
  }

  AsmWriteMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS, MsrData.Qword);

  ///
  /// Second, configure the thermal sensor control
  ///
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_MISC_PWR_MGMT);

  ///
  /// Only lock interrupts if in CMP mode
  ///
  if (mSmst->NumberOfCpus > 1) {
    MsrData.Qword |= B_LOCK_THERMAL_INT;
  }

  AsmWriteMsr64 (EFI_MSR_MISC_PWR_MGMT, MsrData.Qword);

  return EFI_SUCCESS;
}

/**
  Generates a _GPE._L02 SCI to an ACPI OS.
**/
VOID
DigitalThermalSensorSetSwGpeSts (
  VOID
  )
{
  UINT8 GpeCntl;

  ///
  /// Check SCI enable
  ///
  if (((IoRead8 (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT)) & B_PCH_ACPI_PM1_CNT_SCI_EN) != 0) {
    ///
    /// Do platform specific things before generate SCI
    ///
    PlatformHookBeforeGenerateSCI ();

    ///
    /// Set SWGPE Status
    ///
    GpeCntl = IoRead8 (mAcpiBaseAddr + R_ACPI_GPE_CNTL);
    GpeCntl |= B_SWGPE_CTRL;
    IoWrite8 (mAcpiBaseAddr + R_ACPI_GPE_CNTL, GpeCntl);
  }
}

/**
  Checks for a Core Thermal Event on any processor

  @param[in] EventType - DTS_EVENT_TYPE to indicate which DTS event type has been detected.

  @retval TRUE means this is a DTS Thermal event
  @retval FALSE means this is not a DTS Thermal event.
**/
BOOLEAN
DigitalThermalSensorEventCheck (
  DTS_EVENT_TYPE *EventType
  )
{
  ///
  /// Clear event status
  ///
  *EventType = DtsEventNone;

  RunOnAllLogicalProcessors (DigitalThermalSensorEventCheckMsr, EventType);
  ///
  /// Return TRUE if any logical processor reported an event.
  ///
  if (*EventType != DtsEventNone) {
    return TRUE;
  }

  return FALSE;
}

/**
  Checks for a Package Thermal Event by reading MSR.

  @param[in] PkgEventType - DTS_EVENT_TYPE to indicate which DTS event type has been detected.

  @retval TRUE means this is a Package DTS Thermal event
  @retval FALSE means this is not a Package DTS Thermal event.
**/
BOOLEAN
DigitalThermalSensorEventCheckPackageMsr (
  DTS_EVENT_TYPE *PkgEventType
  )
{
  MSR_REGISTER MsrData;

  ///
  /// Clear event status
  ///
  *PkgEventType = DtsEventNone;

  ///
  /// If Processor has already been flagged as Out-Of-Spec,
  /// just return.
  ///
  if (*PkgEventType != DtsEventOutOfSpec) {
    ///
    /// Read thermal status
    ///
    MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS);

    ///
    /// Check for Out-Of-Spec status.
    ///
    if (MsrData.Qword & B_OUT_OF_SPEC_STATUS_LOG) {
      *PkgEventType = DtsEventOutOfSpec;

      ///
      /// Check thresholds.
      ///
    } else if ((DTSSetupValue != DTS_OUT_OF_SPEC_ONLY) &&
               (MsrData.Qword & (B_THERMAL_THRESHOLD_1_STATUS_LOG | B_THERMAL_THRESHOLD_2_STATUS_LOG))
               ) {
      *PkgEventType = DtsEventThreshold;
    }
  }
  ///
  /// Return TRUE if processor reported an event.
  ///
  if (*PkgEventType != DtsEventNone) {
    return TRUE;
  }

  return FALSE;

}

/**
  Checks for a Core Thermal Event by reading MSR.

  This function must be MP safe.

  @param[in] Buffer    Pointer to DTS_EVENT_TYPE
**/
VOID
EFIAPI
DigitalThermalSensorEventCheckMsr (
  IN VOID *Buffer
  )
{
  MSR_REGISTER   MsrData;
  DTS_EVENT_TYPE *EventType;

  ///
  /// Cast to enhance readability.
  ///
  EventType = (DTS_EVENT_TYPE *) Buffer;

  ///
  /// If any processor has already been flagged as Out-Of-Spec,
  /// just return.
  ///
  if (*EventType != DtsEventOutOfSpec) {
    ///
    /// Read thermal status
    ///
    MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);

    ///
    /// Check for Out-Of-Spec status.
    ///
    if (MsrData.Qword & B_OUT_OF_SPEC_STATUS_LOG) {
      *EventType = DtsEventOutOfSpec;

      ///
      /// Check thresholds.
      ///
    } else if ((DTSSetupValue != DTS_OUT_OF_SPEC_ONLY) &&
               (MsrData.Qword & (B_THERMAL_THRESHOLD_1_STATUS_LOG | B_THERMAL_THRESHOLD_2_STATUS_LOG))
               ) {
      *EventType = DtsEventThreshold;
    }
  }
}

/**
  Set the Out Of Spec Interrupt in all cores
  This function must be AP safe.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Out Of Spec Interrupt programmed successfully
**/
VOID
EFIAPI
DigitalThermalSensorSetOutOfSpecInterrupt (
  VOID *Buffer
  )
{
  MSR_REGISTER MsrData;

  ///
  /// Enable Out Of Spec interrupt
  ///
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_THERM_INTERRUPT);
  MsrData.Qword |= OVERHEAT_INTERRUPT_ENABLE;
  AsmWriteMsr64 (EFI_MSR_IA32_THERM_INTERRUPT, MsrData.Qword);

  return;

}

/**
  Set the Out Of Spec Interrupt on the package

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Out Of Spec Interrupt programmed successfully
**/
EFI_STATUS
PackageDigitalThermalSensorSetOutOfSpecInterrupt (
  VOID *Buffer
  )
{
  MSR_REGISTER MsrData;

  ///
  /// Enable Out Of Spec interrupt
  ///
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT);
  MsrData.Qword |= OVERHEAT_INTERRUPT_ENABLE;
  AsmWriteMsr64 (EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT, MsrData.Qword);

  return EFI_SUCCESS;

}

/**
  Read the temperature and reconfigure the thresholds.
  This function must be AP safe.

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature

  @retval EFI_SUCCESS   Digital Thermal Sensor threshold programmed successfully
**/
VOID
EFIAPI
DigitalThermalSensorSetThreshold (
  VOID *Buffer
  )
{
  INT8         ThresholdEntry;
  MSR_REGISTER MsrData;
  UINT8        Temperature;

  ///
  /// Read the temperature
  ///
  MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);

  ///
  /// If Out-Of-Spec, return the critical shutdown temperature.
  ///
  if (MsrData.Qword & B_OUT_OF_SPEC_STATUS) {
    *((UINT8 *) Buffer) = DTS_CRITICAL_TEMPERATURE;
    return;
  } else if (MsrData.Qword & B_READING_VALID) {
    ///
    /// Find the DTS temperature.
    ///
    Temperature = mDtsTjMax - (MsrData.Bytes.ThirdByte & OFFSET_MASK);
    ///
    /// We only update the temperature if it is above the current temperature.
    ///
    if (Temperature > *((UINT8 *) Buffer)) {
      *((UINT8 *) Buffer) = Temperature;
    }
    ///
    /// Compare the current temperature to the Digital Thermal Sensor Threshold Table until
    /// a matching Value is found.
    ///
    ThresholdEntry = 0;
    while ((Temperature > mDtsThresholdTable[ThresholdEntry][0]) && (ThresholdEntry < (mNoOfThresholdRanges - 1))) {
      ThresholdEntry++;
    }
    ///
    /// Update the threshold values
    ///
    MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_THERM_INTERRUPT);
    ///
    /// Low temp is threshold #2
    ///
    MsrData.Bytes.ThirdByte = mDtsThresholdTable[ThresholdEntry][1];
    ///
    /// High temp is threshold #1
    ///
    MsrData.Bytes.SecondByte = mDtsThresholdTable[ThresholdEntry][2];

    ///
    /// Enable interrupts
    ///
    MsrData.Qword |= TH1_ENABLE;
    MsrData.Qword |= TH2_ENABLE;

    ///
    /// If the high temp is at TjMax (offset == 0)
    /// We disable the int to avoid generating a large number of SMI because of TM1/TM2
    /// causing many threshold crossings
    ///
    if (MsrData.Bytes.SecondByte == 0x80) {
      MsrData.Qword &= ~TH1_ENABLE;
    }

    AsmWriteMsr64 (EFI_MSR_IA32_THERM_INTERRUPT, MsrData.Qword);
  }
  ///
  ///  Clear the threshold log bits
  ///
  MsrData.Qword = AsmReadMsr64 (MSR_IA32_THERM_STATUS);
  MsrData.Qword &= ~THERM_STATUS_THRESHOLD_LOG_MASK;
  AsmWriteMsr64 (MSR_IA32_THERM_STATUS, MsrData.Qword);

  return;
}

/**
  Read the temperature and reconfigure the thresholds on the package

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature

  @retval EFI_SUCCESS   Digital Thermal Sensor threshold programmed successfully
**/
EFI_STATUS
PackageDigitalThermalSensorSetThreshold (
  VOID *Buffer
  )
{
  INT8         ThresholdEntry;
  MSR_REGISTER MsrData;
  UINT8        Temperature;

  ///
  /// Read the temperature
  ///
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS);

  ///
  /// If Out-Of-Spec, return the critical shutdown temperature.
  ///
  if (MsrData.Qword & B_OUT_OF_SPEC_STATUS) {
    *((UINT8 *) Buffer) = DTS_CRITICAL_TEMPERATURE;
    return EFI_SUCCESS;
  } else if (MsrData.Qword & B_READING_VALID) {
    ///
    /// Update temperatures for PTID
    ///
    DigitalThermalSensorUpdatePTID ();

    ///
    /// Find the DTS temperature.
    ///
    Temperature = mDtsTjMax - (MsrData.Bytes.ThirdByte & OFFSET_MASK);
    ///
    /// We only update the temperature if it is above the current temperature.
    ///
    if (Temperature > *((UINT8 *) Buffer)) {
      *((UINT8 *) Buffer) = Temperature;
    }
    ///
    /// Compare the current temperature to the Digital Thermal Sensor Threshold Table until
    /// a matching Value is found.
    ///
    ThresholdEntry = 0;
    while ((Temperature > mDtsThresholdTable[ThresholdEntry][0]) && (ThresholdEntry < (mNoOfThresholdRanges - 1))) {
      ThresholdEntry++;
    }
    ///
    /// Update the threshold values
    ///
    MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT);
    ///
    /// Low temp is threshold #2
    ///
    MsrData.Bytes.ThirdByte = mDtsThresholdTable[ThresholdEntry][1];
    ///
    /// High temp is threshold #1
    ///
    MsrData.Bytes.SecondByte = mDtsThresholdTable[ThresholdEntry][2];

    ///
    /// Enable interrupts
    ///
    MsrData.Qword |= TH1_ENABLE;
    MsrData.Qword |= TH2_ENABLE;

    ///
    /// If the high temp is at TjMax (offset == 0)
    /// We disable the int to avoid generating a large number of SMI because of TM1/TM2
    /// causing many threshold crossings
    ///
    if (MsrData.Bytes.SecondByte == 0x80) {
      MsrData.Qword &= ~TH1_ENABLE;
    }

    AsmWriteMsr64 (EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT, MsrData.Qword);
  }
  ///
  ///  Clear the threshold log bits
  ///
  MsrData.Qword = AsmReadMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS);
  MsrData.Qword &= ~THERM_STATUS_THRESHOLD_LOG_MASK;
  AsmWriteMsr64 (EFI_MSR_IA32_PACKAGE_THERM_STATUS, MsrData.Qword);

  return EFI_SUCCESS;
}

/**
  Enables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Enable Local APIC to generate a SMI successfully
**/
VOID
EFIAPI
DigitalThermalSensorEnableSmi (
  VOID *Buffer
  )
{
  UINT32  ApicThermalValue;
  BOOLEAN x2ApicEnabled;

  x2ApicEnabled = (BOOLEAN) (((AsmReadMsr64 (EFI_MSR_XAPIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10);
  ///
  /// Configure the Local APIC to generate an SMI on Thermal events.  First,
  /// Clear BIT16, BIT10-BIT8, BIT7-BIT0.  Then, set BIT9 (delivery mode).
  /// Don't enable the interrupt if it's already enabled
  ///
  if (x2ApicEnabled) {
    ApicThermalValue = (UINT32) AsmReadMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM);
  } else {
    ApicThermalValue = *(UINT32 *) (UINTN) LOCAL_APIC_THERMAL_DEF;
  }

  if ((ApicThermalValue & (B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) != V_MODE_SMI) {
    ApicThermalValue = (ApicThermalValue &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | V_MODE_SMI;
    if (x2ApicEnabled) {
      AsmWriteMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM, ApicThermalValue);
    } else {
      *(UINT32 *) (UINTN) (LOCAL_APIC_THERMAL_DEF) = (UINT32) ApicThermalValue;
    }
  }

  return;
}

/**
  Disables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Disable Local APIC to generate a SMI successfully
**/
VOID
EFIAPI
DigitalThermalSensorDisableSmi (
  VOID *Buffer
  )
{
  UINT32  ApicThermalValue;
  BOOLEAN x2ApicEnabled;

  x2ApicEnabled = (BOOLEAN) (((AsmReadMsr64 (EFI_MSR_XAPIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10);
  ///
  /// Disable Local APIC thermal entry
  ///
  if (x2ApicEnabled) {
    ApicThermalValue = (UINT32) AsmReadMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM);
  } else {
    ApicThermalValue = *(UINT32 *) (UINTN) LOCAL_APIC_THERMAL_DEF;
  }
  ///
  /// Following descriptions were from SSE BIOS
  /// We set the interrupt mode at the same time as the interrupt is disabled to
  /// avoid the "Received Illegal Vector" being set in the Error Status Register.
  ///  and eax, 0FFFEF800h
  ///  or  eax, 000010200h     ; Clear Mask, Set Delivery
  ///
  ApicThermalValue = (ApicThermalValue &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | (B_INTERRUPT_MASK | V_MODE_SMI);
  if (x2ApicEnabled) {
    AsmWriteMsr64 (EFI_MSR_EXT_XAPIC_LVT_THERM, ApicThermalValue);
  } else {
    *(UINT32 *) (UINTN) (LOCAL_APIC_THERMAL_DEF) = (UINT32) ApicThermalValue;
  }

  return;
}

/**
  Runs the specified procedure on all logical processors, passing in the
  parameter buffer to the procedure.

  @param[in] Procedure     The function to be run.
  @param[in] Buffer        Pointer to a parameter buffer.

  @retval EFI_SUCCESS   Function executed successfully.
**/
STATIC
EFI_STATUS
RunOnAllLogicalProcessors (
  IN OUT EFI_AP_PROCEDURE Procedure,
  IN OUT VOID             *Buffer
  )
{
  UINTN      Index;
  EFI_STATUS Status;
  UINT8      RetryIteration;
  ///
  /// Run the procedure on all logical processors.
  ///
  for (Index = 1; Index < mSmst->NumberOfCpus; Index++) {
    Status = EFI_NOT_READY;
    for (RetryIteration = 0; (RetryIteration < DTS_AP_SAFE_RETRY_LIMIT) && (Status != EFI_SUCCESS); RetryIteration++) {
      Status = mSmst->SmmStartupThisAp (Procedure, Index, Buffer);
      if (Status != EFI_SUCCESS) {
        ///
        /// SmmStartupThisAp might return failure if AP is busy executing some other code. Let's wait for sometime and try again.
        ///
        PchPmTimerStall (DTS_WAIT_PERIOD);
      }
    }
  }
  PchPmTimerStall (DTS_WAIT_PERIOD);
  (*Procedure)(Buffer);
  return EFI_SUCCESS;
}

/**
  Runs the specified procedure on one specific logical processors, passing in the
  parameter buffer to the procedure.

  @param[in] Procedure     The function to be run.
  @param[in] Index         Indicate which logical processor should execute this procedure
  @param[in] Buffer        Pointer to a parameter buffer.

  @retval EFI_SUCCESS   Function executed successfully.
**/
STATIC
EFI_STATUS
RunOnSpecificLogicalProcessor (
  IN OUT EFI_AP_PROCEDURE Procedure,
  IN UINTN                Index,
  IN OUT VOID             *Buffer
  )
{
  EFI_STATUS Status;
  UINT8      RetryIteration;
  ///
  /// Run the procedure on one specific logical processor.
  ///
  Status = EFI_NOT_READY;
  for (RetryIteration = 0; (RetryIteration < DTS_AP_SAFE_RETRY_LIMIT) && (Status != EFI_SUCCESS); RetryIteration++) {
    Status = mSmst->SmmStartupThisAp (Procedure, Index, Buffer);
    if (Status != EFI_SUCCESS) {
      ///
      /// SmmStartupThisAp might return failure if AP is busy executing some other code. Let's wait for sometime and try again.
      ///
      PchPmTimerStall (DTS_WAIT_PERIOD);
    }
  }

  return EFI_SUCCESS;
}

/**
  Digital Thermal Sensor (DTS) SMM driver entry point function.

  @param[in] ImageHandle   Image handle for this driver image
  @param[in] SystemTable   Pointer to the EFI System Table

  @retval EFI_SUCCESS   Driver initialization completed successfully
  @retval EFI_OUT_OF_RESOURCES   Error when allocating required memory buffer.
**/
EFI_STATUS
EFIAPI
InstallDigitalThermalSensor (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   Handle;
  MSR_REGISTER                 MsrData;
  EFI_GLOBAL_NVS_AREA_PROTOCOL *GlobalNvsAreaProtocol;
  EFI_SMM_SX_DISPATCH_CONTEXT  SxDispatchContext;
  EFI_SMM_SX_DISPATCH_PROTOCOL *SxDispatchProtocol;
  EFI_HANDLE                   SxDispatchHandle;

  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL         *PchIoTrap;
  EFI_HANDLE                                PchIoTrapHandle;
  EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT PchIoTrapContext;
  EFI_CPUID_REGISTER                        Cpuid06;

  ///
  /// Install DTS_INIT_STATUS_PROTOCOL protocol
  ///
  Handle  = NULL;
  mDtsInitStatusProtocol.IsDtsInitComplete = FALSE;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gDtsInitStatusProtocolGuid,
                  &mDtsInitStatusProtocol,
                  NULL
                  );

  ///
  /// Locate DTS platform policy.
  ///
  Status = gBS->LocateProtocol (
                  &gDxeCpuPlatformPolicyProtocolGuid,
                  NULL,
                  (VOID **) &mPlatformCpu
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "No DTS Platform Policy Protocol available"));
  }

  ASSERT_EFI_ERROR (Status);

  ///
  /// Check if DTS disabled in setup.
  ///
  if (mPlatformCpu->CpuConfig->EnableDts == CPU_FEATURE_DISABLE) {
    DEBUG ((EFI_D_WARN, "DTS not enabled/supported, so driver not loaded into SMM\n"));
    return EFI_SUCCESS;
  }
  ///
  /// Find the SMM base protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **) &mSmmBase);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Initialize global variables.
  ///
  Status = mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  ASSERT_EFI_ERROR (Status);
  ///
  /// Verify the code supports the number of processors present.
  ///
  ASSERT (mSmst->NumberOfCpus <= MAX_NUMBER_OF_THREADS_SUPPORTED);

  ///
  /// Get the ACPI Base Address
  ///
  mAcpiBaseAddr = MmioRead16 (MmPciExpressAddress (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_ACPI_BASE)) &~BIT0;

  ///
  /// Initialize DTS setup value
  ///
  DTSSetupValue = mPlatformCpu->CpuConfig->EnableDts;

  ///
  /// Locate our shared data area
  ///
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **) &GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;
  ///
  /// CPU_ID 6, EAX bit 6 for the Package temperature MSR support
  ///
  ZeroMem (&Cpuid06, sizeof (Cpuid06));
  AsmCpuid (6, &Cpuid06.RegEax, &Cpuid06.RegEbx, &Cpuid06.RegEcx, &Cpuid06.RegEdx);

  gIsPackageTempMsrAvailable                    = (BOOLEAN) ((Cpuid06.RegEax >> 6) & 0x01);
  mGlobalNvsAreaPtr->IsPackageTempMSRAvailable  = gIsPackageTempMsrAvailable;
  ///
  /// Locate Platform specific data area, or prepare platform services
  ///
  InitializeDtsHookLib ();

  ///
  /// Initialize ASL manipulation library
  ///
  InitializeAslUpdateLib ();

  ///
  /// Locate the PCH Trap dispatch protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmIoTrapDispatchProtocolGuid, NULL, (VOID **) &PchIoTrap);
  ASSERT_EFI_ERROR (Status);

  PchIoTrapContext.Type         = ReadWriteTrap;
  PchIoTrapContext.Length       = 4;
  PchIoTrapContext.Address      = 0;
  PchIoTrapContext.Context      = NULL;
  PchIoTrapContext.MergeDisable = FALSE;
  Status = PchIoTrap->Register (
                  PchIoTrap,
                  DtsIoTrapCallback,
                  &PchIoTrapContext,
                  &PchIoTrapHandle
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Update two ASL items.
  /// 1: Operating Region for DTS IO Trap.
  /// 2: Resource Consumption in LPC Device.
  ///
  ASSERT (PchIoTrapContext.Length <= (UINT8) (-1));
  Status = UpdateAslCode (
             (SIGNATURE_32 ('I', 'O', '_', 'D')),
                  PchIoTrapContext.Address,
                  (UINT8) PchIoTrapContext.Length
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Register a callback function to handle Digital Thermal Sensor SMIs.
  ///
  if (mPlatformCpu->CpuConfig->EnableDts != DTS_OUT_OF_SPEC_ONLY) {
    Status = mSmmBase->RegisterCallback (mSmmBase, ImageHandle, DtsSmiCallback, FALSE, FALSE);
    ASSERT_EFI_ERROR (Status);
  } else {
    Status = mSmmBase->RegisterCallback (mSmmBase, ImageHandle, DtsOutOfSpecSmiCallback, FALSE, FALSE);
    ASSERT_EFI_ERROR (Status);
  }
  ///
  /// Locate the Sx Dispatch Protocol
  ///
  Status = gBS->LocateProtocol (
                  &gEfiSmmSxDispatchProtocolGuid,
                  NULL,
                  (VOID **) &SxDispatchProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Register the callback for S3 entry
  ///
  SxDispatchContext.Type  = SxS3;
  SxDispatchContext.Phase = SxEntry;
  Status = SxDispatchProtocol->Register (
                  SxDispatchProtocol,
                  DtsS3EntryCallBack,
                  &SxDispatchContext,
                  &SxDispatchHandle
                  );
  ASSERT_EFI_ERROR (Status);

  if (mPlatformCpu->CpuConfig->EnableDts != DTS_OUT_OF_SPEC_ONLY) {
    ///
    /// Get the TCC Activation Temperature and use it for TjMax.
    ///
    MsrData.Qword         = AsmReadMsr64 (EFI_MSR_IA32_TEMPERATURE_TARGET);

    mDtsTjMax             = (MsrData.Bytes.ThirdByte);
    mDtsThresholdTable    = mDigitalThermalSensorThresholdTable;
    mNoOfThresholdRanges  = DTS_NUMBER_THRESHOLD_RANGES;
  }

  if (gIsPackageTempMsrAvailable) {
    ///
    /// Enable the DTS on package.
    ///
    PackageDigitalThermalSensorEnable (NULL);
  } else {
    ///
    /// Enable the DTS on all logical processors.
    ///
    RunOnAllLogicalProcessors (DigitalThermalSensorEnable, NULL);
  }
  ///
  /// Initialize Digital Thermal Sensor Function in POST
  ///
  DigitalThermalSensorInit ();

  mDtsInitStatusProtocol.IsDtsInitComplete = TRUE;

  return EFI_SUCCESS;
}
