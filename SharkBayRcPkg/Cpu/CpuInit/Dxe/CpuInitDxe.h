/** @file
  Private data structures

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

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/

#ifndef _CPU_INIT_DXE_H
#define _CPU_INIT_DXE_H

#include "CpuAccess.h"
#include "MemoryAttribute.h"
#include <FrameworkDxe.h>
#include <Protocol/Cpu.h>
#include <Protocol/CpuPlatformPolicy.h>
#include <Protocol/Metronome.h>
#include <Protocol/DataHub.h>

#include <Uefi/UefiBaseType.h>


#define INTERRUPT_VECTOR_NUMBER     256
#define INTERRUPT_GATE_ATTRIBUTE    0x8e00
#define NUMBER_OF_MICROCODE_UPDATE  10

extern UINT8 mSmmbaseSwSmiNumber;

#define CPU_FVI_STRING          "Reference Code - CPU"
#define CPU_FVI_SMBIOS_TYPE     0xDD  ///< Default value
#define CPU_FVI_SMBIOS_INSTANCE 0x02
#define UCODE_FVI_STRING        "uCode Version"
#define UCODE_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }
#define TXT_FVI_STRING          "TXT ACM version"
#define TXT_ACM_MAJOR_VERSION   0x1
#define TXT_ACM_MINOR_VERSION   0x3
#define TXT_ACM_REVERSION       0x0
#define TXT_ACM_BUILD_NUMBER    0x1
#define TXT_VERSION \
  { \
    TXT_ACM_MAJOR_VERSION, TXT_ACM_MINOR_VERSION, TXT_ACM_REVERSION, TXT_ACM_BUILD_NUMBER \
  }
enum {
  CPU_RC_VER= 0,
  UCODE_VER,
  TXT_VER
} CPU_FVI_INDEX;

//[-start-121005-IB10300008-remove]//
//extern FVI_ELEMENT_AND_FUNCTION       mCpuFviElementsData[];
//extern FVI_DATA_HUB_CALLBACK_CONTEXT  mCpuFviVersionData;
//extern UINTN                          mCpuFviElements;
//[-end-121005-IB10300008-remove]//

/**
  Adjust length to a paragraph boundry

  @param[in] MemoryLength  -  Input memory length.

  @retval Returned Maximum length.
**/
UINT64
Power2MaxMemory (
  IN UINT64 MemoryLength
  );

/**
  Disable cache and its mtrr

  @param[in] OldMtrr - To return the Old MTRR value
**/
VOID
EfiDisableCacheMtrr (
  IN UINT64 *OldMtrr
  );

/**
  Recover cache MTRR

  @param[in] EnableMtrr - Whether to enable the MTRR
  @param[in] OldMtrr    - The saved old MTRR value to restore when not to
                          enable the MTRR
**/
VOID
EfiRecoverCacheMtrr (
  IN BOOLEAN EnableMtrr,
  IN UINT64  OldMtrr
  );

typedef struct _ALIGNED_DWORD {
  UINT32 High;
  UINT32 Low;
} ALIGNED_DWORD;

typedef union _ALIGNED {
  UINT64        AlignedQword;
  ALIGNED_DWORD AlignedDword;
} ALIGNED;

/**
  Initialize the state information for the CPU Architectural Protocol

  @param[in] ImageHandle - Image handle of the loaded driver
  @param[in] SystemTable - Pointer to the System Table

  @retval EFI_SUCCESS           - thread can be successfully created
  @retval EFI_OUT_OF_RESOURCES  - cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      - cannot create the thread
**/
EFI_STATUS
EFIAPI
InitializeCpu (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  );

/**
  Prepare memory for essential system tables.

  @retval EFI_SUCCESS  - Memory successfully prepared.
**/
EFI_STATUS
PrepareMemory (
  VOID
  );

/**
  Flush CPU data cache. If the instruction cache is fully coherent
  with all DMA operations then function can just return EFI_SUCCESS.

  @param[in] This             - Protocol instance structure
  @param[in] Start            - Physical address to start flushing from.
  @param[in] Length           - Number of bytes to flush. Round up to chipset granularity.
  @param[in] FlushType        - Specifies the type of flush operation to perform.

  @retval EFI_SUCCESS         - If cache was flushed
  @exception EFI_UNSUPPORTED  - If flush type is not supported.
  @retval EFI_DEVICE_ERROR    - If requested range could not be flushed.
**/
EFI_STATUS
EFIAPI
FlushCpuDataCache (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  IN EFI_PHYSICAL_ADDRESS  Start,
  IN UINT64                Length,
  IN EFI_CPU_FLUSH_TYPE    FlushType
  );

/**
  Enables CPU interrupts.

  @param[in] This            - Protocol instance structure

  @retval EFI_SUCCESS        - If interrupts were enabled in the CPU
  @retval EFI_DEVICE_ERROR   - If interrupts could not be enabled on the CPU.
**/
EFI_STATUS
EFIAPI
EnableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL *This
  );

/**
  Disables CPU interrupts.

  @param[in] This           - Protocol instance structure

  @retval EFI_SUCCESS       - If interrupts were disabled in the CPU.
  @retval EFI_DEVICE_ERROR  - If interrupts could not be disabled on the CPU.
**/
EFI_STATUS
EFIAPI
DisableInterrupt (
  IN EFI_CPU_ARCH_PROTOCOL *This
  );

/**
  Return the state of interrupts.

  @param[in] This                - Protocol instance structure
  @param[in] State               - Pointer to the CPU's current interrupt state

  @retval EFI_SUCCESS            - If interrupts were disabled in the CPU.
  @retval EFI_INVALID_PARAMETER  - State is NULL.
**/
EFI_STATUS
EFIAPI
CpuGetInterruptState (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  OUT BOOLEAN              *State
  );

/**
  Generates an INIT to the CPU

  @param[in] This             - Protocol instance structure
  @param[in] InitType         - Type of CPU INIT to perform

  @retval EFI_SUCCESS         - If CPU INIT occurred. This value should never be seen.
  @retval EFI_DEVICE_ERROR    - If CPU INIT failed.
  @exception EFI_UNSUPPORTED  - Requested type of CPU INIT not supported.
**/
EFI_STATUS
EFIAPI
Init (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  IN EFI_CPU_INIT_TYPE     InitType
  );

/**
  Registers a function to be called from the CPU interrupt handler.

  @param[in] This                - Protocol instance structure
  @param[in] InterruptType       - Defines which interrupt to hook.
                                   IA-32 valid range is 0x00 through 0xFF
  @param[in] InterruptHandler    - A pointer to a function of type EFI_CPU_INTERRUPT_HANDLER
                                   that is called when a processor interrupt occurs.
                                   A null pointer is an error condition.

  @retval EFI_SUCCESS            - If handler installed or uninstalled.
  @retval EFI_ALREADY_STARTED    - InterruptHandler is not NULL, and a handler for
                                   InterruptType was previously installed
  @retval EFI_INVALID_PARAMETER  - InterruptHandler is NULL, and a handler for
                                   InterruptType was not previously installed.
  @exception EFI_UNSUPPORTED     - The interrupt specified by InterruptType is not supported.
**/
EFI_STATUS
EFIAPI
RegisterInterruptHandler (
  IN EFI_CPU_ARCH_PROTOCOL     *This,
  IN EFI_EXCEPTION_TYPE        InterruptType,
  IN EFI_CPU_INTERRUPT_HANDLER InterruptHandler
  );

/**
  Returns a timer value from one of the CPU's internal timers. There is no
  inherent time interval between ticks but is a function of the CPU frequency.

  @param[in] This                - Protocol instance structure.
  @param[in] TimerIndex          - Specifies which CPU timer is requested.
  @param[in] TimerValue          - Pointer to the returned timer value.
  @param[in] TimerPeriod         - A pointer to the amount of time that passes in femtoseconds (10-15) for each
                                   increment of TimerValue. If TimerValue does not increment at a predictable
                                   rate, then 0 is returned. The amount of time that has passed between two calls to
                                   GetTimerValue() can be calculated with the formula
                                   (TimerValue2 - TimerValue1) * TimerPeriod. This parameter is optional and may be NULL.

  @retval EFI_SUCCESS            - If the CPU timer count was returned.
  @exception EFI_UNSUPPORTED     - If the CPU does not have any readable timers.
  @retval EFI_DEVICE_ERROR       - If an error occurred while reading the timer.
  @retval EFI_INVALID_PARAMETER  - TimerIndex is not valid or TimerValue is NULL.
**/
EFI_STATUS
EFIAPI
GetTimerValue (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  IN UINT32                TimerIndex,
  OUT UINT64               *TimerValue,
  OUT UINT64 *TimerPeriod  OPTIONAL
  );

/**
  Set memory cacheability attributes for given range of memeory

  @param[in] This                - Protocol instance structure
  @param[in] BaseAddress         - Specifies the start address of the memory range
  @param[in] Length              - Specifies the length of the memory range
  @param[in] Attributes          - The memory cacheability for the memory range

  @retval EFI_SUCCESS            - If the cacheability of that memory range is set successfully
  @exception EFI_UNSUPPORTED     - If the desired operation cannot be done
  @retval EFI_INVALID_PARAMETER  - The input parameter is not correct, such as Length = 0
**/
EFI_STATUS
EFIAPI
SetMemoryAttributes (
  IN EFI_CPU_ARCH_PROTOCOL *This,
  IN EFI_PHYSICAL_ADDRESS  BaseAddress,
  IN UINT64                Length,
  IN UINT64                Attributes
  );

/**
  Init Global Descriptor table
**/
VOID
InitializeSelectors (
  VOID
  );

typedef struct {
  VOID  *Start;
  UINTN Size;
  UINTN FixOffset;
} INTERRUPT_HANDLER_TEMPLATE_MAP;

typedef union {
//[-start-110825-IB03610346-modify]//
  EFI_CPU_DATA_RECORD_PLUS        *DataRecord;
//[-end-110825-IB03610346-modify]//
  UINT8               *Raw;
} EFI_CPU_DATA_RECORD_BUFFER;

///
/// This constant defines the maximum length of the CPU brand string. According to the
/// IA manual, the brand string is in EAX through EDX (thus 16 bytes) after executing
/// the CPUID instructions with EAX as 80000002, 80000003, 80000004.
///
#define MAXIMUM_CPU_BRAND_STRING_LENGTH 48

typedef struct {
  BOOLEAN                    StringValid;
  CHAR16                     BrandString[MAXIMUM_CPU_BRAND_STRING_LENGTH + 1];
  EFI_PROCESSOR_VERSION_DATA StringRef;
} PROCESSOR_VERSION_INFORMATION;

///
/// The constant defines how many times the Cpuid instruction should be executed
/// in order to get all the cache information. For Pentium 4 processor, 1 is enough
///
#define CPU_CPUID_EXECTION_COUNT  2

typedef struct {
  UINT64                                 IntendCoreFrequency;
  UINT64                                 IntendFsbFrequency;
  PROCESSOR_VERSION_INFORMATION          Version;
  EFI_PROCESSOR_MANUFACTURER_DATA        Manufacturer;
  EFI_PROCESSOR_ID_DATA                  CpuidData;
//[-start-110825-IB03610346-modify]//
  EFI_PROCESSOR_FAMILY_DATA_PLUS          Family;
//[-end-110825-IB03610346-modify]//
  INT16                                  Voltage;
  EFI_PROCESSOR_APIC_BASE_ADDRESS_DATA   ApicBase;
  EFI_PROCESSOR_APIC_ID_DATA             ApicID;
  EFI_PROCESSOR_APIC_VERSION_NUMBER_DATA ApicVersion;
  UINT32                                 MicrocodeRevision;
  EFI_PROCESSOR_STATUS_DATA              Status; ///< Need to update this field before report
  CPU_PHYSICAL_LOCATION                  Location;
  EFI_MP_HEALTH_FLAGS                    Health;
  EFI_CPUID_REGISTER                     CacheInformation[CPU_CPUID_EXECTION_COUNT];
} CPU_DATA_FOR_DATAHUB;

///
/// Type, FamilyId and Model values for the different processors
/// [0:7]   - Model
/// [8:23]  - FamilyId
/// [24:31] - Type
///
#define RESERVED  0x00

/**
  This will locate a processor microcode and if it finds a newer revision, it will
  load it to the processor.

  @param[in] MicrocodePointerBuffer  - The Array of pointers which each points to 1 microcode update binary (in memory)
  @param[in] FailedRevision          - The microcode revision that fails to be loaded

  @retval EFI_SUCCESS                - A new microcode update is loaded
  @retval Other                      - Due to some reason, no new microcode update is loaded
**/
EFI_STATUS
InitializeMicrocode (
  IN EFI_CPU_MICROCODE_HEADER **MicrocodePointerBuffer,
  OUT UINT32                  *FailedRevision
  );

/**
  To indicate the first microcode load is done
**/
VOID
McuFirstLoadDone (
  VOID
  );

/**
  Initializes MP support in the system.

  @retval EFI_SUCCESS           - Multiple processors are initialized successfully.
  @retval EFI_NOT_FOUND         - The ACPI variable is not found in S3 boot path.
  @retval EFI_OUT_OF_RESOURCES  - No enough resoruces (such as out of memory).
**/
EFI_STATUS
InitializeMpSupport (
  VOID
  );

/**
  Save the MTRR registers to global variables
**/
VOID
ReadMtrrRegisters (
  VOID
  );

/**
  Synch up the MTRR values for all processors
**/
VOID
EFIAPI
MpMtrrSynchUp (
  IN VOID *Buffer
  );

/**
  Copy Global MTRR data to S3
**/
VOID
SaveBspMtrrForS3 (
  VOID
  );

/**
  Load all microcode updates to memory. Since in S3 resume boot path, CPUs should be
  patched again, these microcode updates are copied to OS reserved memory.

  @retval EFI_SUCCESS           - All microcode updates are loaded to memory successfully
  @retval EFI_OUT_OF_RESOURCES  - Not enough memory to accomodate all the microcode updates
**/
EFI_STATUS
LoadAllMicrocodeUpdates (
  VOID
  );

/**
  Returns the actual CPU core frequency in MHz.

  @param[in] Metronome           - Metronome protocol
  @param[in] Frequency           - Pointer to the CPU core frequency

  @retval EFI_SUCCESS            - If the frequency is returned successfully
  @retval EFI_INVALID_PARAMETER  - If the input parameter is wrong
**/
EFI_STATUS
GetActualFrequency (
  IN EFI_METRONOME_ARCH_PROTOCOL *Metronome,
  OUT UINT64                     *Frequency
  );

/**
  Dump RC CPU and PPM platform policies
**/
VOID
CpuDxePolicyDump (
  VOID
  );

/**
  Initialize the global DataHub pointer

  @param[in] DataHub   - Pointer to the DataHub protocol as output

  @retval EFI_SUCCESS  - If the DataHub pointer is initialized successfully
**/
EFI_STATUS
InitializeDataHubPtr (
  OUT EFI_DATA_HUB_PROTOCOL **DataHub
  );

/**
  Check if loading microcode update fails, if so, report proper status code

  @param[in] CpuNumber       - The CPU number
  @param[in] Status          - The return value of InitializeMicrocode()
  @param[in] FailedRevision  - The revision of the microcode update that failed to be loaded

  @retval EFI_SUCCESS        - The status is check and proper status code is reported
**/
EFI_STATUS
CheckMicrocodeUpdate (
  IN UINTN      CpuNumber,
  IN EFI_STATUS Status,
  IN UINT32     FailedRevision
  );

/**
  Enable Cpu Interrupt
**/
VOID
CpuEnableInterrupt (
  VOID
  );

/**
  Disable Cpu Interrupt
**/
VOID
CpuDisableInterrupt (
  VOID
  );

/**
  Get code segment

  @retval Code segmnet value
**/
UINT16
GetCodeSegment (
  VOID
  );

/**
  Initialize Cpu float point unit
**/
VOID
CpuInitFloatPointUnit (
  VOID
  );

/**
  Drop into SMM to register IOTRAP handler for pfat tools interface

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
InitializePfatToolsIntCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

/**
  Register callback function for Boot Guard revocation flow.

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.
**/
VOID
EFIAPI
BootGuardRevocationCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

#endif
