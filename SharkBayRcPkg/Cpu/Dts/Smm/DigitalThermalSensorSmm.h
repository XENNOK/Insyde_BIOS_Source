/** @file

  Defines and prototypes for the Digital Thermal Sensor SMM driver                                         

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
  
#ifndef _DIGITAL_THERMAL_SENSOR_SMM_H_
#define _DIGITAL_THERMAL_SENSOR_SMM_H_

typedef UINT16  STRING_REF;

//
// Include files
//
#include <Library/UefiLib/UefiLibInternal.h>
#include <Cpu.h>
#include <CpuFuncs.h>
#include <CpuRegs.h>
#include <PchAccess.h>

//
// Include dependent protocols
//
//#include <Protocol/HiiDatabase.h>
#include <Protocol/CpuPlatformPolicy.h>
#include <Protocol/DigitalThermalSensorInitStatus.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/SmmBase.h>
#include <Protocol/SmmIchnDispatch.h>
#include <Protocol/SmmIoTrapDispatch.h>
#include <Protocol/SmmSxDispatch.h>

#include <Library/SmmIoLibSmmCpuIo2/SmmCpuIoLibInternal.h>
#include <Library/AslUpdateLib.h>
#include <Library/DigitalThermalSensorLib.h>
#include <Library/PchPlatformLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/PciLib.h>

///
/// Stall period in microseconds
///
#define DTS_WAIT_PERIOD         1
#define DTS_AP_SAFE_RETRY_LIMIT 3
///
/// Define module definitions
///
#define TJ_MAX                                110
#define DTS_CRITICAL_TEMPERATURE              255

#define DTS_SAMPLE_RATE                       0x10
#define EFI_MSR_XAPIC_BASE                    0x1B
#define EFI_MSR_IA32_THERM_INTERRUPT          0x19B
#define TH1_VALUE                             8
#define TH1_ENABLE                            (1 << 15)
#define TH2_VALUE                             16
#define TH2_ENABLE                            (1 << 23)
#define OFFSET_MASK                           (0x7F)
#define OVERHEAT_INTERRUPT_ENABLE             (1 << 4)

#define B_OUT_OF_SPEC_STATUS                  (1 << 4)
#define B_OUT_OF_SPEC_STATUS_LOG              (1 << 5)
#define B_THERMAL_THRESHOLD_1_STATUS          (1 << 6)
#define B_THERMAL_THRESHOLD_1_STATUS_LOG      (1 << 7)
#define B_THERMAL_THRESHOLD_2_STATUS          (1 << 8)
#define B_THERMAL_THRESHOLD_2_STATUS_LOG      (1 << 9)
#define B_READING_VALID                       (1 << 31)

#define EFI_MSR_IA32_TEMPERATURE_TARGET       0x1A2
#define EFI_MSR_EXT_XAPIC_LVT_THERM           0x833
#define EFI_MSR_MISC_PWR_MGMT                 0x1AA
#define B_LOCK_THERMAL_INT                    (1 << 22)

#define THERM_STATUS_LOG_MASK                 (B_THERMAL_THRESHOLD_2_STATUS_LOG | B_THERMAL_THRESHOLD_1_STATUS_LOG | B_OUT_OF_SPEC_STATUS_LOG)
#define THERM_STATUS_THRESHOLD_LOG_MASK       (B_THERMAL_THRESHOLD_2_STATUS_LOG | B_THERMAL_THRESHOLD_1_STATUS_LOG)

#define EFI_MSR_IA32_PACKAGE_THERM_STATUS     0x1B1
#define EFI_MSR_IA32_PACKAGE_THERM_INTERRUPT  0x1B2

#define B_DTS_IO_TRAP                         (1 << 2)
#define R_ACPI_GPE_CNTL                       0x42                                        ///< ACPI PM IO register 42h
#define R_ACPI_SMI_EN                         0x30                                        ///< ACPI PM IO register 30h
#define B_SWGPE_CTRL                          (1 << 1)
#define DTS_IO_TRAP_REGISTER_LOW_DWORD        (0x00040001 + ICH_DTS_IO_TRAP_BASE_ADDRESS) ///< DigitalThermalSensor IO Trap High DWord value
#define DTS_IO_TRAP_REGISTER_HIGH_DWORD       0x000200F0                                  ///< DigitalThermalSensor IO Trap High DWord value
#define LOCAL_APIC_THERMAL_DEF                0xFEE00330
#define B_INTERRUPT_MASK                      (1 << 16)
#define B_DELIVERY_MODE                       (0x07 << 8)
#define V_MODE_SMI                            (0x02 << 8)
#define B_VECTOR                              (0xFF << 0)

#define DTS_NUMBER_THRESHOLD_RANGES           9     ///< How many ranges are in the threshold table
#define IO_TRAP_INIT_AP_DTS_FUNCTION          0x0A  ///< Enable AP DigitalThermalSensor function
#define IO_TRAP_INIT_DTS_FUNCTION_AFTER_S3    0x14  ///< Enable Digital Thermal Sensor function after resume from S3
#define IO_TRAP_DISABLE_UPDATE_DTS            0x1E  ///< Disable update DTS temperature and threshold value in every SMI
#define INIT_DTS_SCF_MIN                      0x10  ///< SCF Minimum value.
#define INIT_DTS_SCF_UNITY                    0x20  ///< SCF Unity Value.
#define INIT_DTS_SCF_MAX                      0x30  ///< SCF Maximum value.
#define UPDATE_DTS_EVERY_SMI                  TRUE  ///< Update DTS temperature and threshold value in every SMI
#define R_PCH_ACPI_PM1_CNT                    0x04
#define B_PCH_ACPI_PM1_CNT_SCI_EN             0x00000001
#define R_PCH_LPC_ACPI_BASE                   0x40
#define PCI_DEVICE_NUMBER_PCH_LPC             31
#define MAX_NUMBER_OF_THREADS_SUPPORTED       8     ///< Max number of threads supported by processor.
#ifndef BIT63
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000
#define BIT32 0x100000000
#define BIT33 0x200000000
#define BIT34 0x400000000
#define BIT35 0x800000000
#define BIT36 0x1000000000
#define BIT37 0x2000000000
#define BIT38 0x4000000000
#define BIT39 0x8000000000
#define BIT40 0x10000000000
#define BIT41 0x20000000000
#define BIT42 0x40000000000
#define BIT43 0x80000000000
#define BIT44 0x100000000000
#define BIT45 0x200000000000
#define BIT46 0x400000000000
#define BIT47 0x800000000000
#define BIT48 0x1000000000000
#define BIT49 0x2000000000000
#define BIT50 0x4000000000000
#define BIT51 0x8000000000000
#define BIT52 0x10000000000000
#define BIT53 0x20000000000000
#define BIT54 0x40000000000000
#define BIT55 0x80000000000000
#define BIT56 0x100000000000000
#define BIT57 0x200000000000000
#define BIT58 0x400000000000000
#define BIT59 0x800000000000000
#define BIT60 0x1000000000000000
#define BIT61 0x2000000000000000
#define BIT62 0x4000000000000000
#define BIT63 0x8000000000000000
#endif
///
/// Enumerate a DTS event type
///
typedef enum {
  DtsEventNone,
  DtsEventThreshold,
  DtsEventOutOfSpec,
  DtsEventMax
} DTS_EVENT_TYPE;

///
/// Memory Mapped PCI Access macro
///
#define MmPciExpressAddress(Bus, Device, Function, Register) \
    ( \
      (UINTN) (PciRead32 (PCI_LIB_ADDRESS (0,0,0,0x60)) & 0xFC000000) + \
      (UINTN) (Bus << 20) + \
      (UINTN) (Device << 15) + \
      (UINTN) (Function << 12) + \
      (UINTN) (Register) \
    )

//
// Function declarations
//
/**
  SMI handler to handle Digital Thermal Sensor CPU Local APIC SMI
  for thermal threshold interrupt

  @param[in] SmmImageHandle        Image handle returned by the SMM driver.
  @param[in] CommunicationBuffer   Pointer to the buffer that contains the communication Message
  @param[in] SourceSize            Size of the memory image to be used for handler.

  @retval EFI_SUCCESS              Callback Function Executed
**/
EFI_STATUS
EFIAPI
DtsSmiCallback (
  IN EFI_HANDLE SmmImageHandle,
  IN OUT VOID   *CommunicationBuffer,
  IN OUT UINTN  *SourceSize
  );

/**
  Call from SMI handler to handle Package thermal temperature Digital Thermal Sensor CPU Local APIC SMI
  for thermal threshold interrupt

  @retval None
**/
VOID
PackageThermalDTS (
  VOID
  );

/**
  Perform first time initialization of the Digital Thermal Sensor

  @retval EFI_SUCCESS  Init Digital Thermal Sensor successfully
**/
EFI_STATUS
DigitalThermalSensorInit (
  VOID
  );

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
  );

/**
  Initializes the Package Thermal Sensor Control MSR

  @param[in] Buffer        Unused.

  @retval EFI_SUCCESS   The function completed successfully.
**/
EFI_STATUS
PackageDigitalThermalSensorEnable (
  VOID *Buffer
  );

/**
  Generates a _GPE._L02 SCI to an ACPI OS.
**/
VOID
DigitalThermalSensorSetSwGpeSts (
  VOID
  );

/**
  Checks for a Core Thermal Event by reading MSR.

  This function must be MP safe.

  @param[in] Buffer    Pointer to DTS_EVENT_TYPE
**/
VOID
EFIAPI
DigitalThermalSensorEventCheckMsr (
  IN VOID *Buffer
  );

/**
  Checks for a Package Thermal Event by reading MSR.

  @param[in] PkgEventType - DTS_EVENT_TYPE to indicate which DTS event type has been detected.

  @retval TRUE means this is a Package DTS Thermal event
  @retval FALSE means this is not a Package DTS Thermal event.
**/
BOOLEAN
DigitalThermalSensorEventCheckPackageMsr (
  DTS_EVENT_TYPE *PkgEventType
  );

/**
  Checks for a Core Thermal Event on any processor

  @param[in] EventType - DTS_EVENT_TYPE to indicate which DTS event type has been detected.

  @retval TRUE means this is a DTS Thermal event
  @retval FALSE means this is not a DTS Thermal event.
**/
BOOLEAN
DigitalThermalSensorEventCheck (
  DTS_EVENT_TYPE *EventType
  );

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
  );

/**
  Read the temperature and reconfigure the thresholds on the package

  @param[in] Buffer        Pointer to UINT8 to update with the current temperature

  @retval EFI_SUCCESS   Digital Thermal Sensor threshold programmed successfully
**/
EFI_STATUS
PackageDigitalThermalSensorSetThreshold (
  VOID *Buffer
  );

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
  );

/**
  Set the Out Of Spec Interrupt on the package

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Out Of Spec Interrupt programmed successfully
**/
EFI_STATUS
PackageDigitalThermalSensorSetOutOfSpecInterrupt (
  VOID *Buffer
  );

/**
  Enables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Enable Local APIC to generate a SMI successfully
**/
VOID
EFIAPI
DigitalThermalSensorEnableSmi (
  VOID *Buffer
  );

/**
  Disables the Thermal Interrupt in the core Local APIC.

  @param[in] Buffer        Unused

  @retval EFI_SUCCESS   Disable Local APIC to generate a SMI successfully
**/
VOID
EFIAPI
DigitalThermalSensorDisableSmi (
  VOID *Buffer
  );

/**
  Digital Thermal Sensor (DTS) SMM driver entry point function.

  @param[in] ImageHandle   Image handle for this driver image
  @param[in] SystemTable   Pointer to the EFI System Table

  @retval EFI_SUCCESS   Driver initialization completed successfully
**/
EFI_STATUS
EFIAPI
InstallDigitalThermalSensor (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

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
  );

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
  );

/**
  Performs initialization of the threshold table.

  @todo Update this function as necessary for the tables used by the implementation.

  @retval EFI_SUCCESS  Threshold tables initialized successfully.
**/
EFI_STATUS
ThresholdTableInit (
  VOID
  );

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
  );

#endif
