/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for microcode update

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Microcode.c

**/

#include "Cpu.h"
#include "Microcode.h"

MICROCODE_INFO mMicrocodeInfo[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];

/**
  Verify the DWORD type checksum.
  
  This function verifies the DWORD type checksum.

  @param  ChecksumAddr   The handle number of specified processor.
  @param  ChecksumLen    The handle number of specified processor.

**/
EFI_STATUS
Checksum32Verify (
  IN UINT32 *ChecksumAddr,
  IN UINT32 ChecksumLen
  )
{
  UINT32   Checksum;
  UINT32   Index;
  
  Checksum = 0;
  
  for (Index = 0; Index < ChecksumLen; Index ++) {
    Checksum += ChecksumAddr[Index];
  }
  
  return (Checksum == 0) ? EFI_SUCCESS : EFI_CRC_ERROR;
}

/**
  Get microcode update signature of currently loaded microcode udpate.
**/
INT32
GetCurrentMicrocodeSignature (
  VOID
  )
{
  UINT64 Signature;

  AsmWriteMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);
  Signature = AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID);
  return (INT32) RShiftU64 (Signature, 32);
}

/**
  Detect whether specified processor can find matching mirocode patch.
  
  This function detects whether specified processor can find matching mirocode patch.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  FirstLoad         Indicates if it is first time load of the microcode update.

**/
VOID
MicrocodeDetect (
  IN UINTN   ProcessorNumber,
  IN BOOLEAN FirstLoad
  )
{
  UINT32                                  ExtendedTableLength;
  UINT32                                  ExtendedTableCount;
  EFI_CPU_MICROCODE_EXTENDED_TABLE        *ExtendedTable;
  EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *ExtendedTableHeader;
  EFI_CPU_MICROCODE_HEADER                *MicrocodeEntryPoint;
  UINTN                                   MicrocodeEnd;
  UINTN                                   Index;
  UINT8                                   PlatformId;
  UINT32                                  RegEax;
  UINT32                                  LatestRevision;
  UINTN                                   TotalSize;
  EFI_STATUS                              Status;
  BOOLEAN                                 CorrectMicrocode;
  INT32                                   CurrentSignature;
  MICROCODE_INFO                          *MicrocodeInfo;

  if (FirstLoad) {
    ExtendedTableLength = 0;
    //
    // Here data of CPUID leafs have not been collected into context buffer, so
    // GetProcessorCpuid() cannot be used here to retrieve CPUID data.
    //
    AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);

    //
    // The index of platform information resides in bits 50:52 of MSR IA32_PLATFORM_ID
    //
    PlatformId = (UINT8) (RShiftU64 (AsmReadMsr64 (EFI_MSR_IA32_PLATFORM_ID), 50) & 0x07);

    LatestRevision = 0;
    MicrocodeEnd = (UINTN) (PcdGet64 (PcdCpuMicrocodePatchAddress) + PcdGet64 (PcdCpuMicrocodePatchRegionSize));
    MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (UINTN) PcdGet64 (PcdCpuMicrocodePatchAddress);
    do {
      //
      // Check if the microcode is for the Cpu and the version is newer
      // and the update can be processed on the platform
      //
      CorrectMicrocode = FALSE;

      if (MicrocodeEntryPoint->HeaderVersion == 0x1) {
        if (MicrocodeEntryPoint->ProcessorId == RegEax &&
            MicrocodeEntryPoint->UpdateRevision > LatestRevision &&
            (MicrocodeEntryPoint->ProcessorFlags & (1 << PlatformId))
            ) {
          if (MicrocodeEntryPoint->DataSize == 0) {
            Status = Checksum32Verify ((UINT32 *)MicrocodeEntryPoint, 2048 / sizeof(UINT32));
          } else {
            Status = Checksum32Verify ((UINT32 *)MicrocodeEntryPoint, (MicrocodeEntryPoint->DataSize + sizeof(EFI_CPU_MICROCODE_HEADER)) / sizeof(UINT32));
          }
          if (!EFI_ERROR (Status)) {
            CorrectMicrocode = TRUE;
          }
        } else if ((MicrocodeEntryPoint->DataSize != 0) && 
                   (MicrocodeEntryPoint->UpdateRevision > LatestRevision)) {

          ExtendedTableLength = MicrocodeEntryPoint->TotalSize - (MicrocodeEntryPoint->DataSize + sizeof (EFI_CPU_MICROCODE_HEADER));
          if (ExtendedTableLength != 0) {
            //
            // Extended Table exist, check if the CPU in support list
            //
            ExtendedTableHeader = (EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER *)((UINT8 *)(MicrocodeEntryPoint) + MicrocodeEntryPoint->DataSize + sizeof (EFI_CPU_MICROCODE_HEADER));
            //
            // Calulate Extended Checksum
            //          
            if ((ExtendedTableLength % 4) == 0) {
              Status = Checksum32Verify ((UINT32 *)ExtendedTableHeader, ExtendedTableLength / sizeof(UINT32));
              if (!EFI_ERROR (Status)) {
                //
                // Checksum correct
                //
                ExtendedTableCount = ExtendedTableHeader->ExtendedSignatureCount;
                ExtendedTable      = (EFI_CPU_MICROCODE_EXTENDED_TABLE *)(ExtendedTableHeader + 1);
                for (Index = 0; Index < ExtendedTableCount; Index ++) {
                  Status = Checksum32Verify ((UINT32 *)ExtendedTable, sizeof(EFI_CPU_MICROCODE_EXTENDED_TABLE) / sizeof(UINT32)); 
                  if (!EFI_ERROR (Status)) {
                    // 
                    // Verify Header
                    //
                    if ((ExtendedTable->ProcessorSignature == RegEax) &&
                        (ExtendedTable->ProcessorFlag & (1 << PlatformId)) ) {
                      //
                      // Find one
                      //
                      CorrectMicrocode = TRUE; 
                      break;
                    }
                  }
                  ExtendedTable ++;
                }
              }
            }
          }

        }
      }

      //
      // Get the next patch.
      //
      if (MicrocodeEntryPoint->DataSize == 0) {
        TotalSize = 2048;
      } else {
        TotalSize = MicrocodeEntryPoint->TotalSize;
      }

       if (CorrectMicrocode) {
          LatestRevision = MicrocodeEntryPoint->UpdateRevision;
          mMicrocodeInfo[ProcessorNumber].MicrocodeData = (VOID *)((UINTN)MicrocodeEntryPoint + sizeof (EFI_CPU_MICROCODE_HEADER));
          mMicrocodeInfo[ProcessorNumber].MicrocodeSize = TotalSize;
          mMicrocodeInfo[ProcessorNumber].ProcessorId = RegEax;
       }

      MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *) (((UINTN) MicrocodeEntryPoint) + TotalSize);
    } while (((UINTN) MicrocodeEntryPoint < MicrocodeEnd) && (MicrocodeEntryPoint->TotalSize != 0xFFFFFFFF));

    if (LatestRevision > 0) {
      //
      // Get microcode update signature of currently loaded microcode update
      //
      CurrentSignature = GetCurrentMicrocodeSignature ();
      //
      // If no microcode update has been loaded, then trigger first time microcode load.
      //
      if (CurrentSignature == 0) {        
        AsmWriteMsr64 (
          EFI_MSR_IA32_BIOS_UPDT_TRIG, 
          (UINT64) (UINTN) mMicrocodeInfo[ProcessorNumber].MicrocodeData
          );
        mMicrocodeInfo[ProcessorNumber].Load = TRUE;
      } else {
        mMicrocodeInfo[ProcessorNumber].Load = FALSE;
      }

      //
      // If the patch is qualified, set corresponding capability, together with the microcode address
      // and the index of the patch.
      //
      SetProcessorFeatureCapability (ProcessorNumber, Microcode, &mMicrocodeInfo[ProcessorNumber]);
    }
  }else {
    //
    // second time microcode update load
    //
    CurrentSignature = GetCurrentMicrocodeSignature ();
    if (GetProcessorFeatureCapability (ProcessorNumber, Microcode, (VOID**) &MicrocodeInfo)) {
      MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *)((UINT8 *)(MicrocodeInfo->MicrocodeData) - sizeof (EFI_CPU_MICROCODE_HEADER));
      if (CurrentSignature < 0 || (CurrentSignature > 0 && (UINT32)CurrentSignature > MicrocodeEntryPoint->UpdateRevision)) {
          AsmWriteMsr64 (
            EFI_MSR_IA32_BIOS_UPDT_TRIG, 
            (UINT64) (UINTN) MicrocodeInfo->MicrocodeData
            );
        SetProcessorFeatureCapability (ProcessorNumber, Microcode2, MicrocodeInfo->MicrocodeData);
      }
    }
  }
}

/**
  Generate entry for microcode load in S3 register table.
  
  For first time microcode load, if needed, an entry is generated in the pre-SMM-init register table.
  For second time microcode load, if needed, an entry is generated in the post-SMM-init register table.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  FirstLoad         Indicates if it is first time load of the microcode update.

**/
VOID
MicrocodeReg (
  IN UINTN    ProcessorNumber,
  IN BOOLEAN  FirstLoad
  )
{
  MICROCODE_INFO  *MicrocodeInfo;
  VOID            *MicrocodeData;

  if (FirstLoad) {
    if (GetProcessorFeatureCapability (ProcessorNumber, Microcode, (VOID **)&MicrocodeInfo) && MicrocodeInfo->Load) {
      WritePreSmmInitRegisterTable (
        ProcessorNumber,
        Msr,
        EFI_MSR_IA32_BIOS_UPDT_TRIG,
        0,
        64,
        (UINT64)(UINTN)MicrocodeInfo->MicrocodeData
        );
    }
  } else {
    if (GetProcessorFeatureCapability (ProcessorNumber, Microcode2, (VOID **)&MicrocodeData)) {
      WriteRegisterTable (
        ProcessorNumber,
        Msr,
        EFI_MSR_IA32_BIOS_UPDT_TRIG,
        0,
        64,
        (UINT64)(UINTN)MicrocodeData
        );
    }
  }
}

/**
  Measure loaded CPU microcode.
  
**/
VOID
MeasureMicrocode (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_TCG_PROTOCOL          *TcgProtocol;
  UINTN                     ProcessorNumber;
  UINTN                     Index;
  MICROCODE_INFO            *MicrocodeInfo;
  TCG_PCR_EVENT             *TcgEvent;
  EFI_PHYSICAL_ADDRESS      EventLogLastEntry;
  UINT32                    EventNumber;
  CHAR8                     MicrocodeDescriptor[MAX_MICROCODE_DESCRIPTOR_LENGTH];
  EFI_CPU_MICROCODE_HEADER  *MicrocodeEntryPoint;
  UINTN                     DescriptorLen;

  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    //
    // If EFI TCG Protocol is not present, microcode won't be measured
    //
    Status = gBS->LocateProtocol (&gEfiTcgProtocolGuid, NULL, (VOID **)&TcgProtocol);
    if (EFI_ERROR (Status)) {
      return ;
    }

    for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
      //
      // Measure any microcode loaded by this driver or earlier by other modules
      //
      if (GetProcessorFeatureCapability (ProcessorNumber, Microcode, (VOID **)&MicrocodeInfo)) {
        for (Index = 0; Index < ProcessorNumber; Index++) {
          if (MicrocodeInfo->MicrocodeData == mMicrocodeInfo[Index].MicrocodeData) {
            break;
          }
        }
        //
        //Same microcode won't be measured multiple times
        //
        if (Index >= ProcessorNumber) {
          MicrocodeEntryPoint = (EFI_CPU_MICROCODE_HEADER *)MicrocodeInfo->MicrocodeData;
          DescriptorLen = AsciiSPrint (
                            MicrocodeDescriptor,
                            MAX_MICROCODE_DESCRIPTOR_LENGTH,
                            "Load microcode revision %8X for processor %8X",
                            MicrocodeEntryPoint->UpdateRevision,
                            MicrocodeInfo->ProcessorId
                            );

          TcgEvent = (TCG_PCR_EVENT *)AllocateZeroPool (sizeof (TCG_PCR_EVENT) + DescriptorLen);
          ASSERT (TcgEvent != NULL);

          //
          // Measure microcode into PCR[1] using event type EV_CPU_MICROCODE.
          // The digest field contains the SHA-1 hash of the microcode patch applied.
          // The event field contains a descriptor of the microcode patch.
          //
          TcgEvent->PCRIndex   = 1;
          TcgEvent->EventType  = EV_CPU_MICROCODE;
          TcgEvent->EventSize  = (UINT32)(DescriptorLen + 1);
          CopyMem (&TcgEvent->Event[0], MicrocodeDescriptor, DescriptorLen + 1);
          EventNumber = 1;
          Status = TcgProtocol->HashLogExtendEvent (
                     TcgProtocol,
                     (EFI_PHYSICAL_ADDRESS) (UINTN) (VOID *) MicrocodeEntryPoint,
                     (UINT64) MicrocodeInfo->MicrocodeSize,
                     TPM_ALG_SHA,
                     TcgEvent,
                     &EventNumber,
                     &EventLogLastEntry
                     );

          FreePool (TcgEvent);
        }
      }
    }
  }
}

