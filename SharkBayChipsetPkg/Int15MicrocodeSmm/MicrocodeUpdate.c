/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** 
  MCU SMI handler

Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include <PiSmm.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SmmFwBlockService.h>

#include "MicrocodeUpdate.h"

#pragma pack (push, 1)

//
// Definition for IA32 microcode format
//
typedef struct {
  UINT32  HeaderVersion;
  UINT32  UpdateRevision;
  UINT32  Date;
  UINT32  ProcessorId;
  UINT32  Checksum;
  UINT32  LoaderRevision;
  UINT32  ProcessorFlags;
  UINT32  DataSize;
  UINT32  TotalSize;
  UINT8   Reserved[12];
} EFI_CPU_MICROCODE_HEADER;

typedef struct {
  UINT32  ExtendedSignatureCount;
  UINT32  ExtendedTableChecksum;
  UINT8   Reserved[12];
} EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER;

typedef struct {
  UINT32  ProcessorSignature;
  UINT32  ProcessorFlag;
  UINT32  ProcessorChecksum;
} EFI_CPU_MICROCODE_EXTENDED_TABLE;

//
// MU type declaration.
//
typedef struct {
  EFI_CPU_MICROCODE_HEADER  MuHeader;  // The Microcode file header.
  UINT8                     Data[1];   // The Microcode bytes. Variable length.
} EFI_CPU_MICROCODE;

#pragma pack (pop)

#define HEADER_VERSION      0x00000001
#define LOADER_REVISION     0x00000001

#define SIG_PART_ONE        0x0494e5445 // 'INTE'
#define SIG_PART_TWO        0x04c504550 // 'LPEP'
// Supported functions
//
#define PRESCENCE_TEST      0x00
#define WRITE_UPDATE_DATA   0x01
#define UPDATE_CONTROL      0x02
#define READ_UPDATE_DATA    0x03

//
// ah return codes
//
#define INT15_SUCCESS       0x00000
#define NOT_IMPLEMENTED     0x08600
#define ERASE_FAILURE       0x09000
#define WRITE_FAILURE       0x09100
#define READ_FAILURE        0x09200
#define STORAGE_FULL        0x09300
#define CPU_NOT_PRESENT     0x09400
#define INVALID_HEADER      0x09500
#define INVALID_HEADER_CS   0x09600
#define SECURITY_FAILURE    0x09700
#define INVALID_REVISION    0x09800
#define UPDATE_NUM_INVALID  0x09900
#define NOT_EMPTY           0x09A00

#define INT15_ENABLE        0x001

//
// Old microcode size is 2048 bytes, DataSize = 0
// And then microcode became 4096 bytes, DataSize = 0x1000
// Now, we have 5120 bytes of microcode, Datasize= 0x1400
// NOTE: Need to be careful that not all Microcode is aligned to block
// 
#define SIZE_OF_UC_BLOCK    0x800   //original insyde code is 400
#define BLOCK_UNUSED        0x0FF
#define BLOCK_USED          0x000
#define BLOCK_CONTINUOUS    0x001

#define DEFAULT_MICROCODE_SIZE 2048

#define MICROCODE_SIZE_ALIGNED(Size)   ((Size) & ~(SIZE_OF_UC_BLOCK - 1))
#define MICROCODE_SIZE_OCCUPIED(Size)   (((Size) + SIZE_OF_UC_BLOCK - 1) & ~(SIZE_OF_UC_BLOCK - 1))
#define MICROCODE_SIZE_TO_BLOCK(Size)  (((Size) + SIZE_OF_UC_BLOCK - 1) / SIZE_OF_UC_BLOCK)
#define MICROCODE_BLOCK_TO_SIZE(Block)  ((Block) * SIZE_OF_UC_BLOCK)
#define MICROCODE_SIZE_TO_BLOCK_COUNT(Size)  ((Size) / SIZE_OF_UC_BLOCK)


typedef struct {
  BOOLEAN                   PlatformUCValid;
  EFI_CPU_MICROCODE_HEADER  PlatformUC;
  UINT16                    PlatformUCCount;
  UINT32                    PlatformUCFreeSpace;
  UINT32                    PlatformProcessorFlag;
  UINT32                    PlatformProcessorID;
  UINT32                    PlatformUCOffset;
  UINT32                    PlatformPossibleUCAvaibleSpace;
} PLATFORM_MICROCODE_INFO;

PLATFORM_MICROCODE_INFO                 mPlatformUCInfo = { FALSE };
EFI_SMM_CPU_PROTOCOL                    *mSmmCpu;
EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *mSmmFwb;


/**
  Returns the Microcode size.

  @param  MicrocodeHeader Microcode header address

  @return Microcode size
**/
UINT32
GetMicrocodeSize (
  IN EFI_CPU_MICROCODE_HEADER  *MicrocodeHeader
  )
{
  if (MicrocodeHeader->DataSize == 0) {
    return DEFAULT_MICROCODE_SIZE;
  } else {
    return MicrocodeHeader->TotalSize;
  }
}

/**
  Returns the processor microcode revision of the processor installed in the system.

  @return Processor Microcode Revision
**/
UINT32
GetCpuUcodeRevision (
  VOID 
  )
{
  AsmWriteMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);
  return (UINT32) RShiftU64 (AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID), 32);
}

/**
  This will load the microcode to a single processor.
  This function must be MP safe.

  @param MicrocodeEntryPoint - The microcode update pointer
  @param Revision            - The current (before load this microcode update) microcode revision

  @retval EFI_SUCCESS           - Microcode loaded
  @retval EFI_LOAD_ERROR        - Microcode not loaded
**/
EFI_STATUS
LoadMicrocode (
  IN  EFI_CPU_MICROCODE_HEADER  *MicrocodeEntryPoint
  )
{
  UINT32  Revision;

  //
  // Load the Processor Microcode
  //
  Revision = MicrocodeEntryPoint->UpdateRevision;

  AsmWriteMsr64 (
    EFI_MSR_IA32_BIOS_UPDT_TRIG,
    (UINT64) ((UINTN) MicrocodeEntryPoint + sizeof (EFI_CPU_MICROCODE_HEADER))
    );

  //
  // Verify that the microcode has been loaded
  //
  if (GetCpuUcodeRevision () != Revision) {
    return EFI_LOAD_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Appends a MU into a buffer.

  @param MuBuf         - Pointer to buffer to which append the MU.
  @param MuBufSize     - Size of the buffer pointed by MuBuf
  @param Microcode     - Pointer to the MU to be appended. 
                         Must not point within the Buf. Oterwise the result is undefined.
  @param MicrocodeSize - Size of the patch pointed by Microcode.

  @retval EFI_SUCCESS          - Successfully appended the MU.
  @retval EFI_VOLUME_FULL      - No space in the vloume.
  @retval EFI_LOAD_ERROR       - Processor reject the Microcode
**/
EFI_STATUS
MicrocodeAppend (
  IN UINT8                      *MuBuf,
  IN UINTN                      MuBufSize,
  IN UINT8                      *Microcode,
  IN UINTN                      MicrocodeSize
  )
{
  EFI_STATUS  Status;
  UINT8       *NewStartAddr;
  UINT16      MicrocodeIndex;
  UINT32      OldMuLen;
  UINT32      OldMuUsedBlock;
  UINT8       *OldMuAddr;
  UINTN       RequestBlock;
  UINTN       Index;

  Index = 0;
  Status = EFI_SUCCESS;
  RequestBlock = MICROCODE_SIZE_TO_BLOCK(MicrocodeSize);

  if (mPlatformUCInfo.PlatformUCOffset == 0xFFFFFFFF) {
    //
    // No platform microcode found, try append it at end
    //
    if (mPlatformUCInfo.PlatformUCFreeSpace >= RequestBlock) {
      //
      // Not enough space for requesting block then append it
      // Update the microcode first before append it
      //
      DEBUG ((EFI_D_ERROR, "mPlatformUCInfo.PlatformUCFreeSpace >= RequestBlock\n"));

      //
      // Update microcode to each processor
      // APs first, then BSP
      //
      for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
        gSmst->SmmStartupThisAp (LoadMicrocode, Index, (EFI_CPU_MICROCODE_HEADER *) Microcode);
      }

      Status = LoadMicrocode ((EFI_CPU_MICROCODE_HEADER *) Microcode);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      //
      // Append the microcode
      //
      CopyMem (
        (UINT8 *)(MuBuf + MuBufSize - MICROCODE_BLOCK_TO_SIZE(mPlatformUCInfo.PlatformUCFreeSpace)),
        Microcode,
        MicrocodeSize
        );
      if (MICROCODE_SIZE_OCCUPIED(MicrocodeSize) != MicrocodeSize) {
        SetMem (
          (UINT8 *)(MuBuf + MuBufSize - MICROCODE_BLOCK_TO_SIZE(mPlatformUCInfo.PlatformUCFreeSpace) + MicrocodeSize),
          MICROCODE_SIZE_OCCUPIED(MicrocodeSize) - MicrocodeSize,
          0xFF
          );
      }

    } else {
      return EFI_VOLUME_FULL;
    }
  } else {
    //
    // Overwrite able?
    //
    if (mPlatformUCInfo.PlatformUC.TotalSize == MicrocodeSize) {
      DEBUG ((EFI_D_ERROR, "mPlatformUCInfo.PlatformUCTotalSize == MicrocodeSize\n"));
      //
      // Update the microcode for each processor then
      // Overrite it
      //

      //
      // Update microcode to each processor
      // APs first, then BSP
      //
      for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
        gSmst->SmmStartupThisAp (LoadMicrocode, Index, (EFI_CPU_MICROCODE_HEADER *) Microcode);
      }

      Status = LoadMicrocode ((EFI_CPU_MICROCODE_HEADER *) Microcode);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      //
      // Overrite it
      //
      CopyMem (
        (UINT8 *) (MuBuf + mPlatformUCInfo.PlatformUCOffset),
        Microcode,
        MicrocodeSize
        );
      if (MICROCODE_SIZE_OCCUPIED(MicrocodeSize) != MicrocodeSize) {
        SetMem (
          (UINT8 *)(MuBuf + mPlatformUCInfo.PlatformUCOffset + MicrocodeSize),
          MICROCODE_SIZE_OCCUPIED(MicrocodeSize) - MicrocodeSize,
          0xFF
          );
      }
    } else {
      if (mPlatformUCInfo.PlatformPossibleUCAvaibleSpace >= RequestBlock) {
        DEBUG ((EFI_D_ERROR, "mPlatformUCInfo.PlatformPossibleUCAvaibleSpace >= RequestBlock\n"));
        //
        // No available space for requesting block
        // Remove all the overwritted microcode
        //

        //
        // Update microcode to each processor
        // APs first, then BSP
        //
        for (Index = 1; Index < gSmst->NumberOfCpus; Index++) {
          gSmst->SmmStartupThisAp (LoadMicrocode, Index, (EFI_CPU_MICROCODE_HEADER *) Microcode);
        }

        Status = LoadMicrocode ((EFI_CPU_MICROCODE_HEADER *) Microcode);
        if (EFI_ERROR (Status)) {
          return Status;
        }

        OldMuAddr     = MuBuf;
        NewStartAddr  = MuBuf;
        for (MicrocodeIndex = 0; MicrocodeIndex < mPlatformUCInfo.PlatformUCCount; MicrocodeIndex++) {
          OldMuLen = GetMicrocodeSize((EFI_CPU_MICROCODE_HEADER *) OldMuAddr);
          OldMuUsedBlock = MICROCODE_SIZE_TO_BLOCK(OldMuLen);
          
          if ((((EFI_CPU_MICROCODE_HEADER *) OldMuAddr)->ProcessorId != mPlatformUCInfo.PlatformProcessorID) ||
              ((((EFI_CPU_MICROCODE_HEADER *) OldMuAddr)->ProcessorFlags & (1 << mPlatformUCInfo.PlatformProcessorFlag)) == 0)) {
            CopyMem(NewStartAddr, (UINT8 *) ((UINTN) OldMuAddr), OldMuLen);
            if (MICROCODE_SIZE_OCCUPIED(OldMuLen) != OldMuLen) {
              SetMem (
                (UINT8 *)(NewStartAddr + OldMuLen),
                MICROCODE_SIZE_OCCUPIED(OldMuLen) - OldMuLen,
                0xFF
                );
            }

            NewStartAddr += MICROCODE_BLOCK_TO_SIZE(OldMuUsedBlock);
          }

          OldMuAddr += MICROCODE_BLOCK_TO_SIZE(OldMuUsedBlock);
        }

        CopyMem(NewStartAddr, Microcode, MicrocodeSize);
        if (MICROCODE_SIZE_OCCUPIED(MicrocodeSize) != MicrocodeSize) {
          SetMem (
            (UINT8 *)(NewStartAddr + MicrocodeSize),
            MICROCODE_SIZE_OCCUPIED(MicrocodeSize) - MicrocodeSize,
            0xFF
            );
        }
        //
        // Set the unused block to 0xFF
        //
        NewStartAddr += MICROCODE_BLOCK_TO_SIZE(RequestBlock);
        SetMem (
          NewStartAddr,
          (MuBuf + MuBufSize - NewStartAddr),
          0xFF
          ); 
      } else {
        return EFI_VOLUME_FULL;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Finds a Microcode patch in the buffer passed given the index.
  The Index is 0 based.

  @param CountNum      - 0 based index of the MU to retrieve.
  @param Microcode     - Pointer to pointer to buffer holding MU. 
                         Caller must not dispose or modifiy the buffer, only examine.
                         It has to point to NULL for the first MU to be retrieved.
                         May be NULL.
  @param MicrocodeSize - Length of the retrieved MU. May be NULL.

  @retval EFI_SUCCESS   - Successfully read the FV.
  @retval EFI_ABORTED   - The block is a continued block 
**/
EFI_STATUS
GetMicrocode (
  IN     UINTN                      CountNum,
  OUT    UINT8                      **Microcode,   
  OUT    UINTN                      *MicrocodeSize 
  )
{
  UINT8 *Buffer;
  UINTN SpanBlock;
  UINTN Size;
  UINTN Index;
  UINT8 *UCBlockStart;
  UINTN NumberOfUCBlock;
  UINT8 *PlatformUCBlockInfo;
  UINTN MicrocodeFvBase;
  UINTN MicrocodeFvSize;
  UINTN MicrocodeFvOffset;
	
  MicrocodeFvBase = PcdGet32 (PcdFlashNvStorageMicrocodeBase);
  MicrocodeFvSize = PcdGet32 (PcdFlashNvStorageMicrocodeSize);
  MicrocodeFvOffset = 0;
	
  NumberOfUCBlock = (UINTN)MICROCODE_SIZE_TO_BLOCK_COUNT(MicrocodeFvSize - MicrocodeFvOffset);
  PlatformUCBlockInfo = AllocatePool (NumberOfUCBlock);
//[-start-140625-IB05080432-modify]//
  if (PlatformUCBlockInfo == NULL) {
    ASSERT (PlatformUCBlockInfo != NULL);
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-modify]//

  Buffer  = NULL;
  Size    = 0;

  for (Index = 0; Index < NumberOfUCBlock; ) {
    //
    // Check Block Status
    //
    UCBlockStart = (UINT8 *) (MicrocodeFvBase + MicrocodeFvOffset + MICROCODE_BLOCK_TO_SIZE(Index));
    if (((EFI_CPU_MICROCODE_HEADER *) UCBlockStart)->HeaderVersion == HEADER_VERSION) {
      PlatformUCBlockInfo[Index] = BLOCK_USED;
      //
      // Check the block size and set the continuous attribute if needed
      //
      Size = GetMicrocodeSize((EFI_CPU_MICROCODE_HEADER *) UCBlockStart);

      SpanBlock = MICROCODE_SIZE_TO_BLOCK(Size);
      SetMem (
        &PlatformUCBlockInfo[Index + 1],
        SpanBlock - 1,
        BLOCK_CONTINUOUS
        );
      Index += SpanBlock;
    } else {
      PlatformUCBlockInfo[Index] = BLOCK_UNUSED;
      Index++;
    }
  }

  if (PlatformUCBlockInfo[CountNum] == BLOCK_USED) {
    Buffer = (UINT8 *) (MicrocodeFvBase + MicrocodeFvOffset + MICROCODE_BLOCK_TO_SIZE(CountNum));
    Size = GetMicrocodeSize((EFI_CPU_MICROCODE_HEADER *) Buffer);
  } else if (PlatformUCBlockInfo[CountNum] == BLOCK_UNUSED) {
    Buffer = (UINT8 *) (MicrocodeFvBase + MicrocodeFvOffset + MICROCODE_BLOCK_TO_SIZE(CountNum));
    Size = MICROCODE_BLOCK_TO_SIZE(1);
  } else {
    return EFI_ABORTED;
  }
  //
  // Set up returned value
  //
  if (Microcode != NULL) {
    *Microcode = Buffer;
  }

  if (MicrocodeSize != NULL) {
    *MicrocodeSize = Size;
  }

  FreePool (PlatformUCBlockInfo);

  return EFI_SUCCESS;
}

/**
  Appends a MU into a FV. The result is commited to the FV, if there is
  not enough space, it will remove one or more lowest CPUID/Rev MU(s)
  to make the space. If the MU is too big to fit in the reserved MU 
  space, it will return error without modifying the FV.

  @param Microcode     - Pointer to the MU.
  @param MicrocodeSize - Length of the MU.
  @param ScratchPad    - Pointer to the scatch pad memory pass in from caller

  @retval EFI_SUCCESS         - Successfully appended the FV.
  @retval EFI_VOLUME_FULL     - The MU is too big to fit into the FV. 
  @retval EFI_LOAD_ERROR      - Processor reject the Microcode
  @retval others              - An operation accessing the NV failed.
                                There is possibility the FV became corrupted.
**/
EFI_STATUS
MicrocodeAppendCommit (
  IN OUT UINT8               *Microcode,
  IN     UINTN               MicrocodeSize,
  IN     UINT8               *ScratchPad
  )
{
  EFI_STATUS      Status;
  UINT8           *UpdateBuffer;
  UINT8           *MuBuf;
  UINTN           MuBufSize;
  UINTN           MicrocodeFvBase;
  UINTN           MicrocodeFvSize;
  UINTN           MicrocodeFvOffset;
//EFI_FFS_FILE_HEADER   *FfsHeader;
//UINT32                FfsSize;
 
  MicrocodeFvBase = PcdGet32 (PcdFlashNvStorageMicrocodeBase);
  MicrocodeFvSize = PcdGet32 (PcdFlashNvStorageMicrocodeSize);	
  MicrocodeFvOffset = 0;

  //
  // we use the memory from caller
  //
  UpdateBuffer = ScratchPad;
  
  CopyMem (
    UpdateBuffer,
    (VOID *)MicrocodeFvBase,
    MicrocodeFvSize
    );
  
  MuBuf = (UINT8 *)(UINTN)(UpdateBuffer + MicrocodeFvOffset);
  MuBufSize = MICROCODE_SIZE_ALIGNED(MicrocodeFvSize - MicrocodeFvOffset);
  
  //
  // Try to append this MU.
  // If there is no space in the MU buffer, remove the oldest MUs.
  //
  Status = MicrocodeAppend (MuBuf, MuBufSize, Microcode, MicrocodeSize);
  DEBUG ((EFI_D_ERROR, "MicrocodeAppend - %r\n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Need update FFS header size
  //
//
//  FfsSize = (UINT32)(sizeof(EFI_FFS_FILE_HEADER) + MuBufSize);
//  FfsHeader = (EFI_FFS_FILE_HEADER *)(UpdateBuffer + sizeof(EFI_FV_BLOCK_MAP_ENTRY));
//  CopyMem (FfsHeader->Size, &FfsSize, sizeof(FfsHeader->Size));
//  if ((FfsHeader->Attributes & FFS_ATTRIB_CHECKSUM) != 0) {
//    FfsHeader->IntegrityCheck.Checksum.File = 0;
//    FfsHeader->IntegrityCheck.Checksum.File = CalculateCheckSum8 ((UINT8 *) FfsHeader + sizeof(EFI_FFS_FILE_HEADER), FfsSize - sizeof(EFI_FFS_FILE_HEADER));
//  }

  //
  // Update flash part
  //
  Status = mSmmFwb->EraseBlocks (mSmmFwb, MicrocodeFvBase, &MicrocodeFvSize);   
  if (!EFI_ERROR (Status)) {
		Status = mSmmFwb->Write (mSmmFwb, MicrocodeFvBase, &MicrocodeFvSize, UpdateBuffer);
  }

  DEBUG ((EFI_D_ERROR, "WriteMicrocodeSpace - %r\n", Status));

  return Status;
}

/**
  Computes 8-bit checksum.

  @param Adr - Addres to start the computation.
  @param Len - Len in bytes over which to compute the checksum.

  @return 8-bit checksum.
**/
UINT32
Checksum32 (
  IN VOID              *Adr,
  IN UINTN              Len
  )
{
  UINT32  Sum;

  Sum = 0;
  while (TRUE) {
    if (Len-- == 0) {
     return Sum;
    }

    Sum = (UINT32)(Sum + *(UINT32*)Adr);
    Adr = (UINT32*)Adr + 1;
  }
}

/**
  Get the Microcode counts in NVS storage.

  @param MicrocodeSegment:       The start address of the Microcode in NVS storage.
  @param MicrocodeSegmentLength: The lenght of the Microcode segment in NVS storage.
**/
VOID
InitializeMicrocodeUpdate (
  UINT8        *MicrocodeSegment,
  UINT32       MicrocodeSegmentLength
  )
{
  EFI_CPU_MICROCODE_HEADER  *MicrocodeHeader;
  UINT16                    MicrocodeCount;
  UINT32                    MicrocodeSize;
  UINT32                    UsedBlock;
  UINT32                    TotalUsedBlock;
  UINT32                    RegEax;
  UINT64                    PlatformID;

  MicrocodeSegmentLength = MICROCODE_SIZE_ALIGNED(MicrocodeSegmentLength);

  MicrocodeCount  = 0;
  TotalUsedBlock  = 0;
  MicrocodeHeader = (EFI_CPU_MICROCODE_HEADER *) MicrocodeSegment;
  if (mPlatformUCInfo.PlatformUCValid == TRUE) {
    return ;
  }

  ZeroMem ((UINT8 *) (&mPlatformUCInfo), sizeof (mPlatformUCInfo));
  mPlatformUCInfo.PlatformUCValid = TRUE;

  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);
  PlatformID                             = AsmReadMsr64 (EFI_MSR_IA32_PLATFORM_ID);
  mPlatformUCInfo.PlatformProcessorFlag  = (UINT32)(RShiftU64 (PlatformID, 50)) & 0x7;
  mPlatformUCInfo.PlatformProcessorID    = RegEax & 0x0FFF3FFF;
  mPlatformUCInfo.PlatformUCOffset       = 0xFFFFFFFF;
  mPlatformUCInfo.PlatformPossibleUCAvaibleSpace = 0;
  while ((MicrocodeHeader->HeaderVersion != 0xFFFFFFFF) && (MICROCODE_BLOCK_TO_SIZE(TotalUsedBlock) < MicrocodeSegmentLength)) {
    //
    // We assume the microcode is valid, more check is optional
    //
    MicrocodeSize = GetMicrocodeSize ((EFI_CPU_MICROCODE_HEADER *)MicrocodeHeader);
    UsedBlock = MICROCODE_SIZE_TO_BLOCK(MicrocodeSize);
    
    //
    // Record the platform micro code address
    //
    if (MicrocodeHeader->ProcessorFlags & (1 << mPlatformUCInfo.PlatformProcessorFlag) &&
        MicrocodeHeader->ProcessorId == mPlatformUCInfo.PlatformProcessorID
        ) {
      if (MicrocodeHeader->UpdateRevision > mPlatformUCInfo.PlatformUC.UpdateRevision) {

        mPlatformUCInfo.PlatformUC.UpdateRevision  = MicrocodeHeader->UpdateRevision;
        mPlatformUCInfo.PlatformUCOffset           = MICROCODE_BLOCK_TO_SIZE(TotalUsedBlock);
        mPlatformUCInfo.PlatformUC.TotalSize       = MicrocodeSize;
        mPlatformUCInfo.PlatformPossibleUCAvaibleSpace += UsedBlock;
      }
    }

    TotalUsedBlock += UsedBlock;
    MicrocodeCount++;
    MicrocodeHeader = (EFI_CPU_MICROCODE_HEADER *) (MicrocodeSegment + MICROCODE_BLOCK_TO_SIZE(TotalUsedBlock));
  }

  mPlatformUCInfo.PlatformUCCount      = MicrocodeCount;
  mPlatformUCInfo.PlatformUCFreeSpace  = MICROCODE_SIZE_TO_BLOCK_COUNT(MicrocodeSegmentLength) - TotalUsedBlock;
  mPlatformUCInfo.PlatformPossibleUCAvaibleSpace += MICROCODE_SIZE_TO_BLOCK_COUNT(MicrocodeSegmentLength) - TotalUsedBlock;
}

UINT32
ReadRegister32 (
  EFI_SMM_SAVE_STATE_REGISTER  Register,
  UINTN                        CpuIndex
  )
{
  EFI_STATUS                   Status;
  UINT32                       Uint32;

  Status = mSmmCpu->ReadSaveState (
                      mSmmCpu,
                      sizeof (Uint32),
                      Register,
                      CpuIndex,
                      &Uint32
                      );
  ASSERT_EFI_ERROR (Status);
  return Uint32;
}

/**
  This notification function is called when an SMM Mode is invoked through SMI.
  This may happen during RT, so it must be RT safe.

  @param DispatchHandle  - EFI Handle
  @param DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

  @retval EFI_SUCCESS - If this function did handle its specific event.
  @retval other       - This function did not need to handle its events.

--*/
EFI_STATUS
EFIAPI
MicrocodeUpdateCallback (
  IN EFI_HANDLE                        DispatchHandle,
  IN CONST EFI_SMM_SW_REGISTER_CONTEXT *Context,
  IN OUT EFI_SMM_SW_CONTEXT            *SwContext,
  IN OUT UINTN                         *CommBufferSize
  )
{
  EFI_STATUS                Status;
  EFI_DWORD_REGS            *RegBuf;
  UINTN                     Index;
  UINT8                     *Microcode;
  UINTN                     MicrocodeSize;
  UINTN                     MicrocodeFvBase;
  UINTN                     MicrocodeFvSize;
  UINTN                     MicrocodeFvOffset;

  MicrocodeFvBase = PcdGet32 (PcdFlashNvStorageMicrocodeBase);
  MicrocodeFvSize = PcdGet32 (PcdFlashNvStorageMicrocodeSize);
  MicrocodeFvOffset = 0;  // Insyde code microcode don't have header
  //
  // Get the pointer to saved CPU regs.
  //
  RegBuf = (EFI_DWORD_REGS *) (UINTN) ReadRegister32 (EFI_SMM_SAVE_STATE_REGISTER_RSI, SwContext->SwSmiCpuIndex);

  //
  // Check parameter condition
  //
  if (((UINTN)RegBuf == 0xffffffff) || ((UINTN)RegBuf == 0)) {
    return EFI_SUCCESS;
  }

  //
  // Test to make sure the agent causing INT15 had in mind the Microcode Update.
  //
  if ((RegBuf->EAX & 0x0000FFFF) != 0x0000D042) {
    return EFI_SUCCESS;
  }

  //
  // Get the Microcode count
  //
  InitializeMicrocodeUpdate (
    (UINT8 *) (UINTN) MicrocodeFvBase + MicrocodeFvOffset,
    (UINT32) (MicrocodeFvSize - MicrocodeFvOffset)
    );

  //
  // Process requests now. Assume success.
  //
  RegBuf->EFlags.CF = 0;
  //
  // Same as INT15_SUCCESS
  //
  RegBuf->EAX &= 0xffff0000;

  switch (RegBuf->EBX & 0xFF) {

  case PRESCENCE_TEST:
    //
    // Input:
    //   AX - D042h
    //   BL - 00h i.e., PRESCENCE_TEST
    //
    // output:
    //   CF  NC - All return values are valid
    //       CY - Failure, AH contains status.
    //
    //   AH  Return code
    //   AL  Additional OEM information
    //   EBX Part one of the signature 'INTE'.
    //   ECX Part two of the signature 'LPEP'.
    //   EDX Version number of the BIOS update loader
    //   SI  Number of update blocks system can record in NVRAM (1 based).
    //
    DEBUG ((EFI_D_ERROR, "PRESCENCE_TEST\n"));
    RegBuf->EBX = SIG_PART_ONE;
    RegBuf->ECX = SIG_PART_TWO;
    RegBuf->EDX = LOADER_REVISION;
    RegBuf->ESI &= 0xffff0000;
    RegBuf->ESI |= MICROCODE_SIZE_TO_BLOCK_COUNT(MicrocodeFvSize - MicrocodeFvOffset);
    break;

  case UPDATE_CONTROL:
    //
    // Input:
    //  AX - D042h
    //  BL - 02h i.e., UPDATE_CONTROL
    //  BH - Task
    //       1 - Enable the update loading at initialization time.
    //       2 - Determine the current state of the update control without changing its status.
    //
    // output:
    //  AH  Return code
    //  AL  Additional OEM information
    //  BL  Update status Disable or Enable.
    //
    DEBUG ((EFI_D_ERROR, "UPDATE_CONTROL\n"));
    if ((RegBuf->EBX & 0x0000FF00) == 0 || (RegBuf->EBX & 0x0000FF00) > 0x200) {
      //
      // Indicate we cannot determinate the Enable/Disable status via CMOS
      //
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX       = (RegBuf->EAX & 0xffffff00) | READ_FAILURE;  
    }

    //
    // Always enabled.
    //
    RegBuf->EBX = (RegBuf->EBX & 0xffffff00) | INT15_ENABLE; 
    break;

  case READ_UPDATE_DATA:
    //
    // Determine the current state of the update control without changing its status.
    //
    // Input:
    //  AX  - D042h
    //  BL  - 03h i.e., READ_UPDATE_DATA
    //  ES:DI - Real Mode Pointer to the Intel Update structure.
    //  SS:SP - Stack pointer (32K minimum)
    //  SI    - Update number, the index number of the update block to be read.
    //          This number is zero based and must be less than the update
    //          count returned from the prescence test function.
    //
    // output:
    //  AH  Return code
    //  AL  Additional OEM information
    //  BL  Update status Disable or Enable.
    //
    DEBUG ((EFI_D_ERROR, "READ_UPDATE_DATA\n"));
    Index = RegBuf->ESI & 0xFFFF;

    //
    // Zero Based
    //
    if (Index >= MICROCODE_SIZE_TO_BLOCK_COUNT(MicrocodeFvSize - MicrocodeFvOffset)) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= UPDATE_NUM_INVALID;
      break;
    }

    Status = GetMicrocode (Index, &Microcode, &MicrocodeSize);
    if (Status == EFI_NOT_FOUND) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= READ_FAILURE;
      break;
    } else if (Status == EFI_ABORTED) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= NOT_EMPTY;
      break;
    }

    CopyMem (
            (VOID*) (UINTN) ((RegBuf->ES << 4) + (RegBuf->EDI & 0xFFFF)),
            Microcode,
            MicrocodeSize
            );

    //
    // Always enabled.
    //
    RegBuf->EBX = (RegBuf->EBX & 0xffffff00) | INT15_ENABLE;
    break;

  case WRITE_UPDATE_DATA:
    //
    // Input:
    //  AX    - D042h
    //  BL    - 01h i.e., WRITE_UPDATE_DATA
    //  ES:DI - Real Mode Pointer to the Intel Update structure.
    //  CX    - Scratch Pad1 (Real Mode Scratch segment 64K in length)
    //  DX    - Scratch Pad2 (Real Mode Scratch segment 64K in length)
    //  SI    - Scratch Pad3 (Real Mode Scratch segment 64K in length)
    //  SS:SP - Stack pointer (32K minimum)
    //
    // output:
    //  CF  NC - All return values are valid
    //      CY - Failure, AH contains status.
    //
    //  AH  Return code
    //  AL  Additional OEM information
    //
    // Append the new MU and commit it to the FV.
    // If not enough space, remove some old ones.
    //
    DEBUG ((EFI_D_ERROR, "WRITE_UPDATE_DATA\n"));
    //
    // Get the address to the buffer.
    //
    Microcode = (VOID *) (UINTN) ((RegBuf->ES << 4) + (RegBuf->EDI & 0xFFFF));
      
    //
    // Compute Size of the update.
    //
    MicrocodeSize = GetMicrocodeSize ((EFI_CPU_MICROCODE_HEADER *)Microcode);
    
    //
    // Check if the Microcode valid
    //
    if (((EFI_CPU_MICROCODE_HEADER *) Microcode)->LoaderRevision != 1 ||
        ((EFI_CPU_MICROCODE_HEADER *) Microcode)->HeaderVersion  != 1) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= INVALID_HEADER;
      DEBUG ((EFI_D_ERROR, "INVALID_HEADER\n"));
      break;
    }

    if (Checksum32 (Microcode, MicrocodeSize/4) != 0) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= INVALID_HEADER_CS;
      DEBUG ((EFI_D_ERROR, "INVALID_HEADER_CS\n"));
      break;
    }

    if (((EFI_CPU_MICROCODE_HEADER *) Microcode)->ProcessorId != mPlatformUCInfo.PlatformProcessorID) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= CPU_NOT_PRESENT;
      DEBUG ((EFI_D_ERROR, "CPU_NOT_PRESENT\n"));
      break;
    }

    if ((((EFI_CPU_MICROCODE_HEADER *) Microcode)->ProcessorFlags & (1 << mPlatformUCInfo.PlatformProcessorFlag)) == 0) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= CPU_NOT_PRESENT;
      DEBUG ((EFI_D_ERROR, "CPU_NOT_PRESENT\n"));
      break;
    }

    if ((mPlatformUCInfo.PlatformUC.UpdateRevision >= ((EFI_CPU_MICROCODE_HEADER *) Microcode)->UpdateRevision) &&
        mPlatformUCInfo.PlatformUCOffset != 0xFFFFFFFF
        ) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= INVALID_REVISION;
      DEBUG ((EFI_D_ERROR, "INVALID_REVISION\n"));
      break;
    }

    if (MICROCODE_SIZE_TO_BLOCK(MicrocodeSize) > MICROCODE_SIZE_TO_BLOCK_COUNT(MicrocodeFvSize - MicrocodeFvOffset)) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= STORAGE_FULL;
      DEBUG ((EFI_D_ERROR, "STORAGE_FULL\n"));
      break;
    }
    
    Status = MicrocodeAppendCommit (Microcode, MicrocodeSize, (UINT8 *) (UINTN) ((RegBuf->ECX & 0xFFFF) << 4));
    DEBUG ((EFI_D_ERROR, "MicrocodeAppendCommit - %r\n", Status));
    if (Status == EFI_VOLUME_FULL) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= STORAGE_FULL;
      DEBUG ((EFI_D_ERROR, "STORAGE_FULL\n"));
    } else if (Status == EFI_LOAD_ERROR) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= SECURITY_FAILURE;
      DEBUG ((EFI_D_ERROR, "SECURITY_FAILURE\n"));
    } else if (EFI_ERROR (Status)) {
      RegBuf->EFlags.CF = 1;
      RegBuf->EAX |= WRITE_FAILURE;
      DEBUG ((EFI_D_ERROR, "WRITE_FAILURE\n"));
      break;
    } else {
      mPlatformUCInfo.PlatformUCValid = FALSE;
    }
    break;

  default:
    RegBuf->EFlags.CF = 1;
    RegBuf->EAX |= NOT_IMPLEMENTED;
  }
  //
  // The event was handled.
  //
  return EFI_SUCCESS;
}

/**
  Initializes the SMM Platfrom Driver
  
  @param ImageHandle   - Pointer to the loaded image protocol for this driver
  @param SystemTable   - Pointer to the EFI System Table

  @retval Status        - EFI_SUCCESS
  @retval Assert, otherwise.

--*/
EFI_STATUS
EFIAPI
MicrocodeUpdateEntrypoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;  
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT               SwContext;
  EFI_HANDLE                                SwHandle;


	Status = gSmst->SmmLocateProtocol (&gEfiSmmFwBlockServiceProtocolGuid, NULL, (VOID **)&mSmmFwb);
  ASSERT_EFI_ERROR (Status);

  //
  // Get SMM CPU protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid, 
                    NULL, 
                    (VOID **)&mSmmCpu
                    );
  ASSERT_EFI_ERROR (Status);

  //
  //  Get the Sw dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID**)&SwDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register microcode update handler (INT15 D042 call SMI function)
  //
  SwContext.SwSmiInputValue = INT15_0D042H;
  Status = SwDispatch->Register (SwDispatch, MicrocodeUpdateCallback, &SwContext, &SwHandle);
  ASSERT_EFI_ERROR (Status);
  

  return EFI_SUCCESS;
}

