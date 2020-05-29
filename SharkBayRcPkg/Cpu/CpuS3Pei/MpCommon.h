/** @file
  some definitions for MP and HT driver.

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
#ifndef _MP_COMMON_
#define _MP_COMMON_

#include <CpuAccess.h>
#include <Library/CpuPlatformLib.h>
///
/// Protocol produced by this driver
///
/// #include EFI_PROTOCOL_PRODUCER (MpService)
///
/// Protocol consumed by this driver
///
#include <Ppi/Stall.h>
#include <Guid/SmramCpuDataHeader.h>
#include <FrameworkDxe.h>
#include <Ppi/CpuPlatformPolicy.h>

#define VacantFlag          0x00
#define NotVacantFlag       0xff

#define MICROSECOND         10

#define MAXIMUM_CPU_NUMBER  0x40
#define STACK_SIZE_PER_PROC 0x8000

#define MAXIMUM_CPU_S3_TABLE_SIZE     0x1000

#define IO_APIC_INDEX_REGISTER        0xFEC00000
#define IO_APIC_DATA_REGISTER         0xFEC00010
extern UINTN FixedMtrrNumber;
extern UINTN MtrrDefTypeNumber;
extern UINTN VariableMtrrNumber;

typedef struct {
  UINT16 Index;
  UINT64 Value;
} EFI_MTRR_VALUES;

typedef struct {
  UINT32 ApicId;
  UINT32 MsrIndex;
  UINT64 MsrValue;
} MP_CPU_S3_SCRIPT_DATA;

typedef struct {
  UINT32 S3BootScriptTable;
  UINT32 S3BspMtrrTable;
  UINT32 VirtualWireMode;
} MP_CPU_S3_DATA_POINTER;

typedef struct {
  UINT32                Lock;
  UINT32                StackStart;
  UINT32                StackSize;
  UINT32                ApFunction;
  PSEUDO_DESCRIPTOR     GdtrProfile;
  PSEUDO_DESCRIPTOR     IdtrProfile;
  UINT32                BufferStart;
  UINT32                PmodeOffset;
  UINT32                AcpiCpuDataAddress;
  UINT32                MtrrValuesAddress;
  UINT32                FinishedCount;
  UINT32                WakeupCount;
  UINT32                SerializeLock;
  UINT32                MicrocodePointer;
  MP_CPU_S3_SCRIPT_DATA *S3BootScriptTable;
  UINT32                StartState;
  UINT32                VirtualWireMode;
  VOID (*SemaphoreCheck)(
          UINT32 *SemaphoreAddress
          );
  UINT32                      McuLoadCount;
  EFI_PEI_SERVICES            **PeiServices;
  EFI_PEI_STALL_PPI           *PeiStall;
  UINT64                      CpuPerfCtrlValue;
  PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi;
} MP_CPU_EXCHANGE_INFO;

/**
  Get protected mode code offset

  @retval Offset of protected mode code
**/
VOID *
AsmGetPmodeOffset (
  VOID
  );

/**
  Get code offset of SemaphoreCheck

  @retval Offset of SemaphoreCheck
**/
UINT32
AsmGetSemaphoreCheckOffset (
  VOID
  );

/**
  Read MTRR settings

  @param[in] PeiServices - General purpose services available to every PEIM.
  @param[in] MtrrValues  - buffer to store MTRR settings
**/
VOID
ReadMtrrRegisters (
  IN EFI_PEI_SERVICES **PeiServices,
  UINT64              *MtrrValues
  );

/**
  Syncup MTRR settings between all processors

  @param[in] MtrrValues  - buffer to store MTRR settings
**/
VOID
MpMtrrSynchUp (
  UINT64 *MtrrValues
  );

/**
  Set MTRR registers

  @param[in] PeiServices - General purpose services available to every PEIM.
  @param[in] MtrrArray   - buffer with MTRR settings
**/
VOID
SetMtrrRegisters (
  IN EFI_PEI_SERVICES **PeiServices,
  IN EFI_MTRR_VALUES  *MtrrArray
  );

#ifdef EFI_DEBUG
/**
  Print MTRR settings in debug build BIOS

  @param[in] PeiServices - General purpose services available to every PEIM.
  @param[in] MtrrArray   - buffer with MTRR settings
**/
VOID
ShowMtrrRegisters (
  IN EFI_PEI_SERVICES **PeiServices,
  IN EFI_MTRR_VALUES  *MtrrArray
  );
#endif

/**
  This will locate a processor microcode and if it finds a newer revision, it will
  load it to the processor.

  @param[in] ExchangeInfo           - Pointer to the exchange info buffer for output.
  @param[in] MicrocodePointerBuffer - The Array of pointers which each points to 1 microcode update binary (in memory)
  @param[in] FailedRevision         - The microcode revision that fails to be loaded

  @retval EFI_SUCCESS           - A new microcode update is loaded
  @retval Other                 - Due to some reason, no new microcode update is loaded
**/
EFI_STATUS
InitializeMicrocode (
  IN MP_CPU_EXCHANGE_INFO     *ExchangeInfo,
  IN EFI_CPU_MICROCODE_HEADER **MicrocodePointerBuffer,
  OUT UINT32                  *FailedRevision
  );

///
/// Functions shared in MP/HT drivers
///
/**
  Send interrupt to CPU

  @param[in] BroadcastMode - interrupt broadcast mode
  @param[in] ApicID        - APIC ID for sending interrupt
  @param[in] VectorNumber  - Vector number
  @param[in] DeliveryMode  - Interrupt delivery mode
  @param[in] TriggerMode   - Interrupt trigger mode
  @param[in] Assert        - Interrupt pin polarity
  @param[in] PeiServices   - Indirect reference to the PEI Services Table
  @param[in] PeiStall      - EFI_PEI_STALL_PPI to stall for some interval

  @retval EFI_INVALID_PARAMETER - input parameter not correct
  @retval EFI_NOT_READY         - there was a pending interrupt
  @retval EFI_SUCCESS           - interrupt sent successfully
**/
EFI_STATUS
SendInterrupt (
  IN UINT32           BroadcastMode,
  IN UINT32           ApicID,
  IN UINT32           VectorNumber,
  IN UINT32           DeliveryMode,
  IN UINT32           TriggerMode,
  IN BOOLEAN          Assert,
  IN EFI_PEI_SERVICES **PeiServices,
  IN EFI_PEI_STALL_PPI                     *PeiStall
  );

/**
  Get APIC ID of processor

  @retval APIC ID of processor
**/
UINT32
GetApicID (
  VOID
  );

/**
  Programs Local APIC registers for virtual wire mode.

  @param[in] BSP             - Is this BSP?
**/
VOID
ProgramXApic (
  BOOLEAN BSP
  );

/**
  Lock APs

  @param[in] Lock  - Lock state
**/
VOID
AsmAcquireMPLock (
  IN UINT8 *Lock
  );

/**
  Release APs

  @param[in] Lock  - Lock state
**/
VOID
AsmReleaseMPLock (
  IN UINT8 *Lock
  );

/**
  Get address map of RendezvousFunnelProc.

  @retval AddressMap  - Output buffer for address map information
**/
VOID *
AsmGetAddressMap (
  VOID
  );

/**
  Do CLI and HALT processor
**/
VOID
AsmCliHltLoop (
  VOID
  );

#endif
