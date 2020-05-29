/** @file
  CPU microcode update PEIM

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

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#include "MpCommon.h"
#include <Library/SynchronizationLib.h>
#include <CpuLib.h>

#define SKIP_MICROCODE_CHECKSUM_CHECK 1
///
/// Array of pointers which each points to 1 microcode update binary (in memory)
///
EFI_CPU_MICROCODE_HEADER **mMicrocodePointerBuffer;

///
/// Function declarations
///
EFI_STATUS
FindLoadMicrocode (
  IN UINT32                   Cpuid,
  IN EFI_CPU_MICROCODE_HEADER **MicrocodePointerBuffer,
  IN OUT UINT32               *Revision
  );

/**
  Check number of cores in the package.

  @retval Number of cores in the package.
**/
UINT8
GetCoreNumber (
  VOID
  )
{
  EFI_CPUID_REGISTER Cpuid;
  AsmCpuidEx (
          4,
          0,
          &Cpuid.RegEax,
          NULL,
          NULL,
          NULL
          );
  return (UINT8) ((RShiftU64 (Cpuid.RegEax, 26) & 0x3f) + 1);
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

  ApicID = GetApicID ();

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
  Returns the processor microcode revision of the processor installed in the system.

  @retval Processor Microcode Revision
**/
UINT32
GetCpuUcodeRevision (
  VOID
  )
{
  AsmWriteMsr64 (MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);
  return (UINT32) RShiftU64 (AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID), 32);
}

/**
  Wait till all primary threads done the microcode load

  @param[in] ExchangeInfo      - Pointer to the exchange info buffer for output.
**/
VOID
WaitForPrimaryThreadMcuUpdate (
  IN MP_CPU_EXCHANGE_INFO *ExchangeInfo
  )
{
  UINTN CoreNumber;
  CoreNumber = (UINTN) ((RShiftU64 (AsmReadMsr64 (MSR_CORE_THREAD_COUNT), 16)) & 0xffff);
  if (IsSecondaryThread ()) {
    while (ExchangeInfo->McuLoadCount < CoreNumber) {
      CpuPause ();
    }
  }
}

/**
  This function checks the MCU revision to decide if BIOS needs to load
  microcode.

  @param[in] MicrocodePointer - Microcode in memory
  @param[in] Revision         - Current CPU microcode revision

  @retval EFI_SUCCESS - BIOS needs to load microcode
  @retval EFI_ABORTED - Don't need to update microcode
**/
EFI_STATUS
CheckMcuRevision (
  IN EFI_CPU_MICROCODE_HEADER *MicrocodePointer,
  IN UINT32                   Revision
  )
{
  EFI_STATUS Status;
  Status = EFI_ABORTED;

  if (Revision == 0) {
    Status = EFI_SUCCESS;
  }

  return Status;
}

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
  )
{
  EFI_STATUS         Status;
  EFI_CPUID_REGISTER Cpuid;
  UINT32             UcodeRevision;
  ACPI_CPU_DATA      *mAcpiCpuData;

  mAcpiCpuData = (ACPI_CPU_DATA *) (ExchangeInfo->AcpiCpuDataAddress);

  AsmCpuid (
          CPUID_VERSION_INFO,
          &Cpuid.RegEax,
          &Cpuid.RegEbx,
          &Cpuid.RegEcx,
          &Cpuid.RegEdx
          );

  WaitForPrimaryThreadMcuUpdate (ExchangeInfo);
  UcodeRevision = GetCpuUcodeRevision ();
  Status = FindLoadMicrocode (
                  Cpuid.RegEax,
                  MicrocodePointerBuffer,
                  &UcodeRevision
                  );
  *FailedRevision = UcodeRevision;

  InterlockedIncrement (&(ExchangeInfo->McuLoadCount));
  return Status;
}

/**
  This will load the microcode to the processors.

  @param[in] MicrocodeEntryPoint - The microcode update pointer
  @param[in] Revision            - The current (before load this microcode update) microcode revision

  @retval EFI_SUCCESS           - Microcode loaded
  @retval EFI_LOAD_ERROR        - Microcode not loaded
**/
EFI_STATUS
LoadMicrocode (
  IN EFI_CPU_MICROCODE_HEADER *MicrocodeEntryPoint,
  IN UINT32                   *Revision
  )
{
  ///
  /// Load the Processor Microcode
  ///
  AsmWriteMsr64 (
          MSR_IA32_BIOS_UPDT_TRIG,
          (UINT64) ((UINTN) MicrocodeEntryPoint + sizeof (EFI_CPU_MICROCODE_HEADER))
          );

  ///
  /// Verify that the microcode has been loaded
  ///
  if (GetCpuUcodeRevision () == *Revision) {
    return EFI_LOAD_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Verify the DWORD type checksum

  @param[in] ChecksumAddr  - The start address to be checkumed
  @param[in] ChecksumLen   - The length of data to be checksumed

  @retval EFI_SUCCESS           - Checksum correct
  @retval EFI_CRC_ERROR         - Checksum incorrect
**/
EFI_STATUS
Checksum32Verify (
  IN UINT32 *ChecksumAddr,
  IN UINT32 ChecksumLen
  )
{
#if SKIP_MICROCODE_CHECKSUM_CHECK
  return EFI_SUCCESS;
#else
  UINT32 Checksum;
  UINT32 Index;

  Checksum = 0;

  for (Index = 0; Index < ChecksumLen; Index++) {
    Checksum += ChecksumAddr[Index];
  }

  return (Checksum == 0) ? EFI_SUCCESS : EFI_CRC_ERROR;
#endif
}

/**
  Find microcode data

  @param[in] Cpuid                   - processor CPUID
  @param[in] MicrocodePointerBuffer  - the pointer for microcode buffer
  @param[in] Revision                - revision of microcode

  @retval The pointer of microcode header
**/
EFI_CPU_MICROCODE_HEADER *
FindMicrocode (
  IN UINTN                    Cpuid,
  IN EFI_CPU_MICROCODE_HEADER **MicrocodePointerBuffer,
  IN UINT32                   *Revision
  )
{
  EFI_STATUS                              Status;
  EFI_CPU_MICROCODE_HEADER                *MicrocodeEntryPoint;
  UINT8                                   Index;
  UINT8                                   ExtendedIndex;
  UINT8                                   MsrPlatform;
  UINT32                                  ExtendedTableLength;
  UINT32                                  ExtendedTableCount;
  BOOLEAN                                 CorrectMicrocode;
  EFI_CPU_MICROCODE_EXTENDED_TABLE        *ExtendedTable;
  EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *ExtendedTableHeader;

  Status              = EFI_NOT_FOUND;
  ExtendedTableLength = 0;
  MicrocodeEntryPoint = NULL;
  CorrectMicrocode    = FALSE;

  ///
  /// The index of platform information resides in bits 50:52 of MSR IA32_PLATFORM_ID
  ///
  MsrPlatform = (UINT8) (RShiftU64 (AsmReadMsr64 (MSR_IA32_PLATFORM_ID), 50) & 0x07);

  Index       = 0;
  while (MicrocodePointerBuffer[Index] != NULL) {
    MicrocodeEntryPoint = MicrocodePointerBuffer[Index];
    CorrectMicrocode    = FALSE;
    ///
    /// Check if the microcode is for the Cpu and the version is newer
    /// and the update can be processed on the platform
    ///
    if ((MicrocodeEntryPoint->HeaderVersion == 0x00000001) &&
        !EFI_ERROR (CheckMcuRevision (MicrocodeEntryPoint, *Revision))
        ) {
      if ((MicrocodeEntryPoint->ProcessorId == Cpuid) && (MicrocodeEntryPoint->ProcessorFlags & (1 << MsrPlatform))) {
        if (MicrocodeEntryPoint->DataSize == 0) {
          Status = Checksum32Verify ((UINT32 *) MicrocodeEntryPoint, 2048 / sizeof (UINT32));
        } else {
          Status = Checksum32Verify (
                          (UINT32 *) MicrocodeEntryPoint,
                          (MicrocodeEntryPoint->DataSize + sizeof (EFI_CPU_MICROCODE_HEADER)) / sizeof (UINT32)
                          );
        }

        if (!EFI_ERROR (Status)) {
          CorrectMicrocode = TRUE;
        }
      } else if ((MicrocodeEntryPoint->DataSize != 0)) {
        ///
        /// Check the  Extended Signature if the entended signature exist
        /// Only the data size != 0 the extended signature may exist
        ///
        ExtendedTableLength = MicrocodeEntryPoint->TotalSize - (MicrocodeEntryPoint->DataSize + sizeof (EFI_CPU_MICROCODE_HEADER));
        if (ExtendedTableLength != 0) {
          ///
          /// Extended Table exist, check if the CPU in support list
          ///
          ExtendedTableHeader = (EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *) ((UINT8 *) (MicrocodeEntryPoint) + MicrocodeEntryPoint->DataSize + 48);
          ///
          /// Calulate Extended Checksum
          ///
          if ((ExtendedTableLength % 4) == 0) {
            Status = Checksum32Verify ((UINT32 *) ExtendedTableHeader, ExtendedTableLength / sizeof (UINT32));
            if (!EFI_ERROR (Status)) {
              ///
              /// Checksum correct
              ///
              ExtendedTableCount  = ExtendedTableHeader->ExtendedSignatureCount;
              ExtendedTable       = (EFI_CPU_MICROCODE_EXTENDED_TABLE *) (ExtendedTableHeader + 1);
              for (ExtendedIndex = 0; ExtendedIndex < ExtendedTableCount; ExtendedIndex++) {
                ///
                /// Verify Header
                ///
                if ((ExtendedTable->ProcessorSignature == Cpuid) && (ExtendedTable->ProcessorFlag & (1 << MsrPlatform))) {
                  Status = Checksum32Verify (
                                  (UINT32 *) ExtendedTable,
                                  sizeof (EFI_CPU_MICROCODE_EXTENDED_TABLE) / sizeof (UINT32)
                                  );
                  if (!EFI_ERROR (Status)) {
                    ///
                    /// Find one
                    ///
                    CorrectMicrocode = TRUE;
                    break;
                  }
                }

  ExtendedTable++;
              }
            }
          }
        }
      }
    }

    if (CorrectMicrocode) {
      break;
    }

    Index += 2;
  }

  if (!CorrectMicrocode) {
    MicrocodeEntryPoint = NULL;
  }

  return MicrocodeEntryPoint;
}

/**
  This will locate a processor microcode and if it finds a newer revision, it will
  load it to the processor.

  @param[in] Cpuid                  - Data returned by cpuid instruction
  @param[in] MicrocodePointerBuffer - The Array of pointers which each points to 1 microcode update binary (in memory)
  @param[in] Revision               - As input parameter, the current microcode revision;
                           as output parameter, the microcode revision after microcode update is loaded

  @retval EFI_SUCCESS           - A new microcode update is loaded
  @retval Other                 - Due to some reason, no new microcode update is loaded
**/
EFI_STATUS
FindLoadMicrocode (
  IN UINT32                   Cpuid,
  IN EFI_CPU_MICROCODE_HEADER **MicrocodePointerBuffer,
  IN OUT UINT32               *Revision
  )
{
  EFI_STATUS               Status;
  EFI_CPU_MICROCODE_HEADER *MicrocodeEntryPoint;

  Status = EFI_NOT_FOUND;

  MicrocodeEntryPoint = FindMicrocode (
                  Cpuid,
                  MicrocodePointerBuffer,
                  Revision
                  );

  if (MicrocodeEntryPoint != NULL) {
    Status    = LoadMicrocode (MicrocodeEntryPoint, Revision);
    *Revision = MicrocodeEntryPoint->UpdateRevision;
  }

  return Status;
}
