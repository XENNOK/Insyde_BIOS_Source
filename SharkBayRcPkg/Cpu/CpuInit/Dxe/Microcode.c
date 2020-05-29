/** @file
  CPU Microcode update driver

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

#include "CpuInitDxe.h"
#include "ProcessorData.h"
#include "MpCommon.h"
#include "MpService.h"
#include <Library/BaseLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>

extern DXE_CPU_PLATFORM_POLICY_PROTOCOL *mPlatformCpu;

///
/// static type so this variable only can be accessed in this file
///
static UINT32 mMcuFirstLoadDone = FALSE;
UINT32        mMcuLoadCount;

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
  To indicate the first microcode load is done
**/
VOID
McuFirstLoadDone (
  VOID
  )
{
  mMcuFirstLoadDone = TRUE;
}

/**
  Wait till all primary threads done the microcode load
**/
VOID
WaitForPrimaryThreadMcuUpdate (
  VOID
  )
{
  UINTN CoreNumber;
  CoreNumber = RShiftU64 (AsmReadMsr64 (MSR_CORE_THREAD_COUNT), 16) & 0xffff;
  if (IsSecondaryThread ()) {
    while (mMcuLoadCount < CoreNumber) {
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
  IN UINT32                   *Revision
  )
{
  EFI_STATUS Status;
  Status = EFI_ABORTED;

  if (mMcuFirstLoadDone) {
    if (MicrocodePointer->UpdateRevision & 0x80000000 ||
        (MicrocodePointer->UpdateRevision > 0 && MicrocodePointer->UpdateRevision > *Revision)
        ) {
      Status = EFI_SUCCESS;
    }
  } else {
    if (*Revision == 0) {
      Status = EFI_SUCCESS;
    }
  }

  return Status;
}

/**
  This will locate a processor microcode and if it finds a newer revision, it will
  load it to the processor.

  @param[in] MicrocodePointerBuffer - The Array of pointers which each points to 1 microcode update binary (in memory)
  @param[in] FailedRevision         - The microcode revision that fails to be loaded

  @retval EFI_SUCCESS           - A new microcode update is loaded
  @retval Other                 - Due to some reason, no new microcode update is loaded
**/
EFI_STATUS
InitializeMicrocode (
  IN EFI_CPU_MICROCODE_HEADER **MicrocodePointerBuffer,
  OUT UINT32                  *FailedRevision
  )
{
  EFI_STATUS         Status;
  EFI_CPUID_REGISTER Cpuid;
  UINT32             UcodeRevision;

  AsmCpuid (
          CPUID_VERSION_INFO,
          &Cpuid.RegEax,
          &Cpuid.RegEbx,
          &Cpuid.RegEcx,
          &Cpuid.RegEdx
          );

  WaitForPrimaryThreadMcuUpdate ();
  UcodeRevision = GetCpuUcodeRevision ();
  Status = FindLoadMicrocode (
                  Cpuid.RegEax,
                  MicrocodePointerBuffer,
                  &UcodeRevision
                  );
  *FailedRevision = UcodeRevision;

  InterlockedIncrement (&mMcuLoadCount);

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

  @param[in] ChecksumAddr  - The start address to be checksumed
  @param[in] ChecksumLen  - The length of data that will be checksumed

  @retval EFI_SUCCESS           - Checksum correct
  @retval EFI_CRC_ERROR         - Checksum incorrect
**/
EFI_STATUS
Checksum32Verify (
  IN UINT32 *ChecksumAddr,
  IN UINT32 ChecksumLen
  )
{
  UINT32 Checksum;
  UINT32 Index;

  Checksum = 0;

  for (Index = 0; Index < ChecksumLen; Index++) {
    Checksum += ChecksumAddr[Index];
  }

  return (Checksum == 0) ? EFI_SUCCESS : EFI_CRC_ERROR;
}

/**
  Check if this microcode is correct one for processor

  @param[in] Cpuid               - processor CPUID
  @param[in] MicrocodeEntryPoint - entry point of microcode
  @param[in] Revision            - revision of microcode

  @retval CorrectMicrocode if this microcode is correct
**/
BOOLEAN
CheckMicrocode (
  IN UINTN                    Cpuid,
  IN EFI_CPU_MICROCODE_HEADER *MicrocodeEntryPoint,
  IN UINT32                   *Revision
  )
{
  EFI_STATUS                              Status;
  UINT8                                   ExtendedIndex;
  UINT8                                   MsrPlatform;
  UINT32                                  ExtendedTableLength;
  UINT32                                  ExtendedTableCount;
  BOOLEAN                                 CorrectMicrocode;
  EFI_CPU_MICROCODE_EXTENDED_TABLE        *ExtendedTable;
  EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *ExtendedTableHeader;

  Status              = EFI_NOT_FOUND;
  ExtendedTableLength = 0;
  CorrectMicrocode    = FALSE;

  ///
  /// The index of platform information resides in bits 50:52 of MSR IA32_PLATFORM_ID
  ///
  MsrPlatform = (UINT8) (RShiftU64 (AsmReadMsr64 (MSR_IA32_PLATFORM_ID), 50) & 0x07);

  ///
  /// Check if the microcode is for the Cpu and the version is newer
  /// and the update can be processed on the platform
  ///
  if ((MicrocodeEntryPoint->HeaderVersion == 0x00000001) &&
      !EFI_ERROR (CheckMcuRevision (MicrocodeEntryPoint, Revision))
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

  return CorrectMicrocode;
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
  EFI_STATUS               Status;
  EFI_CPU_MICROCODE_HEADER *MicrocodeEntryPoint;
  UINT8                    Index;
  BOOLEAN                  CorrectMicrocode;

  Status              = EFI_NOT_FOUND;
  MicrocodeEntryPoint = NULL;
  CorrectMicrocode    = FALSE;

  Index               = 0;
  while (MicrocodePointerBuffer[Index] != NULL) {
    MicrocodeEntryPoint = MicrocodePointerBuffer[Index];
    CorrectMicrocode    = CheckMicrocode (Cpuid, MicrocodeEntryPoint, Revision);

    if (CorrectMicrocode) {
      break;
    }

    Index++;
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

  @retval EFI_SUCCESS               - A new microcode update is loaded
  @retval Other                     - Due to some reason, no new microcode update is loaded
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

/**
  Load all microcode updates to memory. Since in S3 resume boot path, CPUs should be
  patched again, these microcode updates are copied to OS reserved memory.

  @retval EFI_SUCCESS           - All microcode updates are loaded to memory successfully
  @retval EFI_OUT_OF_RESOURCES  - Not enough memory to accomodate all the microcode updates
**/
EFI_STATUS
LoadAllMicrocodeUpdates (
  VOID
  )
{
  EFI_STATUS               Status;
  EFI_CPU_MICROCODE_HEADER *MicrocodeEntryPoint;
  EFI_CPU_MICROCODE_HEADER *MicrocodeBuffer;
  UINTN                    MicrocodeNumber;
  UINTN                    Index;
  UINTN                    TotalSize[NUMBER_OF_MICROCODE_UPDATE + 1];
  EFI_CPUID_REGISTER       Cpuid;
  UINT32                   UcodeRevision;

  AsmCpuid (
          CPUID_VERSION_INFO,
          &Cpuid.RegEax,
          &Cpuid.RegEbx,
          &Cpuid.RegEcx,
          &Cpuid.RegEdx
          );

  UcodeRevision   = 0;
  MicrocodeNumber = 0;
  Status = (gBS->AllocatePool)
          (
                  EfiReservedMemoryType, sizeof (EFI_CPU_MICROCODE_HEADER *) * (NUMBER_OF_MICROCODE_UPDATE + 1), (VOID *)
                  (&mMicrocodePointerBuffer)
          );
  ASSERT_EFI_ERROR (Status);

  ZeroMem (mMicrocodePointerBuffer, sizeof (EFI_CPU_MICROCODE_HEADER *) * (NUMBER_OF_MICROCODE_UPDATE + 1));

  MicrocodeEntryPoint = NULL;
  while (TRUE) {
    if (MicrocodeNumber > NUMBER_OF_MICROCODE_UPDATE) {
      DEBUG ((EFI_D_INFO, "CPU Too Many Microcode available > %d\n", (UINTN) NUMBER_OF_MICROCODE_UPDATE));
      Status = EFI_SUCCESS;
      break;
    }
    ///
    /// Initialize it to 0
    ///
    TotalSize[MicrocodeNumber] = 0;
    ///
    /// Continue to try to find patch
    ///
    Status = mPlatformCpu->CpuConfig->RetrieveMicrocode (mPlatformCpu, (VOID *) &MicrocodeEntryPoint);

    if (EFI_ERROR (Status)) {
      if (Status == EFI_NOT_FOUND) {
        Status = EFI_SUCCESS;
      }
      break;

    } else {
      if (!CheckMicrocode (Cpuid.RegEax, MicrocodeEntryPoint, &UcodeRevision)) {
        continue;
      }

      if (MicrocodeEntryPoint->DataSize == 0) {
        TotalSize[MicrocodeNumber] = 2048;
      } else {
        TotalSize[MicrocodeNumber] = MicrocodeEntryPoint->TotalSize;
      }

      Status = AllocateReservedMemoryBelow4G (
                      TotalSize[MicrocodeNumber],
                      &MicrocodeBuffer
                      );
      if (EFI_ERROR (Status)) {
        break;
      }

      CopyMem (MicrocodeBuffer, MicrocodeEntryPoint, TotalSize[MicrocodeNumber]);
      mMicrocodePointerBuffer[MicrocodeNumber] = MicrocodeBuffer;
      MicrocodeNumber++;
    }
  }

  if (EFI_ERROR (Status)) {
    Index = 0;
    while ((Index <= NUMBER_OF_MICROCODE_UPDATE) && (mMicrocodePointerBuffer[Index] != NULL)) {
      (gBS->FreePages)((EFI_PHYSICAL_ADDRESS) (UINTN) mMicrocodePointerBuffer[Index], EFI_SIZE_TO_PAGES (TotalSize[Index]));
      mMicrocodePointerBuffer[Index] = NULL;
      Index++;
    }
  }

  return Status;
}

/**
  Check if loading microcode update fails, if so, report proper status code

  @param[in] CpuNumber       - The CPU number
  @param[in] Status          - The return value of InitializeMicrocode()
  @param[in] FailedRevision  - The revision of the microcode update that failed to be loaded

  @retval EFI_SUCCESS - The status is check and proper status code is reported
**/
EFI_STATUS
CheckMicrocodeUpdate (
  IN UINTN      CpuNumber,
  IN EFI_STATUS Status,
  IN UINT32     FailedRevision
  )
{
  EFI_STATUS_CODE_VALUE StatusCode;

  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  if (Status == EFI_LOAD_ERROR) {
    StatusCode = EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_MICROCODE_UPDATE;
  } else if (Status == EFI_NOT_FOUND) {
    if (GetCpuUcodeRevision () != 0) {
      ///
      /// Some other driver (for example, SEC) already updated CPU microcode
      ///
      return EFI_SUCCESS;
    }
    StatusCode = EFI_COMPUTING_UNIT_HOST_PROCESSOR | EFI_CU_HP_EC_NO_MICROCODE_UPDATE;
  } else {
    return Status;
  }
  ///
  /// ReportStatusCode UEFI service can't be called by AP currently, so call by BSP only
  ///
  if (CpuNumber == mMPSystemData->BSP) {
    return ReportStatusCode (
                   EFI_ERROR_MINOR | EFI_ERROR_CODE,
                   StatusCode
                   );
  } else {
    return Status;
  }
}
