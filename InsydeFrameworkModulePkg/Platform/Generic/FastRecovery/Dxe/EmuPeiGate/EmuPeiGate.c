//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

    EmuPeiGate.c

Abstract:

  Fast recovery helper DXE driver. 
  Jump back to recovery only emulate PEI phase. Or do preparation for BDS's
  recovery process.

--*/
#include "Tiano.h"
#include "EfiCommon.h"
#include "EfiDriverLib.h"
#include "EfiFlashMap.h"
#include "FlashMap.h"
#include "EfiHobLib.h"
#include "EfiImage.h"
#include "EfiPeOptionalHeader.h"
#include "FastRecoveryData.h"
#include "EmuPeiGate.h"
#include "FlashLayout.h"

#include EFI_GUID_DEFINITION (Hob)
#include EFI_GUID_DEFINITION (FirmwareFileSystem)
#include EFI_GUID_DEFINITION (RecoveryFileAddress)
#include EFI_GUID_DEFINITION (EmuPeiMark)
#include EFI_GUID_DEFINITION (EmuSecFileName)
#include EFI_GUID_DEFINITION (FlashMapHob)
#include EFI_GUID_DEFINITION (SystemNvDataGuid)
#include EFI_PPI_DEFINITION (FirmwareAuthentication)

#define MAX_BIOS_IMAGE_SIZE  0x2000000        // 32MB

static EFI_FLASH_AREA_DATA   mFlashAreaData[] = { EFI_FLASH_AREA_DATA_DEFINITION };

#define NUM_FLASH_AREA_DATA (sizeof (mFlashAreaData) / sizeof (mFlashAreaData[0]))

extern EFI_GUID EmuPeiMaskList[];

EFI_STATUS
GetBiosRegionFromCapsule (
  IN UINT8    *Capsule,
  IN UINTN    CapsuleSize,
  OUT UINTN   *BiosRegionOffset
);

BOOLEAN
IsThisFileNeedToMask (
  IN EFI_GUID             *FileGuid
)
/*++

Routine Description:

  Check specified file need to mask in recovery only PEI phase. 

Arguments:

  FileGuid                - Pointer to filename Guid.

Returns:

  TRUE                    - This file need to mask and need not to dispatch.
  FALSE                   - This file should be dispatched as usual.

--*/
{
  UINTN     Counter;
  BOOLEAN   FindItem;
  EFI_GUID  EndGuid = {0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
  
  for (Counter = 0, FindItem = FALSE; 
       (!EfiCompareGuid(EmuPeiMaskList + Counter, &EndGuid)) && (!FindItem);
       Counter ++ ) {
       
    FindItem = EfiCompareGuid(EmuPeiMaskList + Counter, FileGuid);
  }
  
  return FindItem; 
  
}

EFI_STATUS 
UpdateHeaderChecksum (
  IN EFI_FFS_FILE_HEADER  *FileHeader
)
/*++

Routine Description:

  Update Ffs header's checksum. 

Arguments:

  FileHeader              - Pointer to firmware file system entry header.

Returns:

  EFI_SUCCESS             - Operation is successfully completed.

--*/
{
  UINT8   *ptr;
  UINTN   Index;
  UINT8   Sum;
  
  Sum = 0;
  ptr = (UINT8 *)FileHeader;
  FileHeader->IntegrityCheck.Checksum.Header = 0;

  for (Index = 0; Index < sizeof(EFI_FFS_FILE_HEADER) - 3; Index += 4) {
    Sum = (UINT8)(Sum + ptr[Index]);
    Sum = (UINT8)(Sum + ptr[Index+1]);
    Sum = (UINT8)(Sum + ptr[Index+2]);
    Sum = (UINT8)(Sum + ptr[Index+3]);
  }

  for (; Index < sizeof(EFI_FFS_FILE_HEADER); Index++) {
    Sum = (UINT8)(Sum + ptr[Index]);
  }
  
  //
  // State field (since this indicates the different state of file). 
  //
  Sum = (UINT8)(Sum - FileHeader->State);

  //
  // Checksum field of the file is not part of the header checksum.
  //
  Sum = (UINT8)(Sum - FileHeader->IntegrityCheck.Checksum.File);

  //
  // Fill final checksum
  //
  FileHeader->IntegrityCheck.Checksum.Header = (~Sum) + 1; 

  return EFI_SUCCESS;
}


EFI_STATUS
ValidateFvHeader (
  IN EFI_FIRMWARE_VOLUME_HEADER *VolumeHeader
)
/*++

Routine Description:

  Check if the input volume header is valid. 

Arguments:

  VolumeHeader            - Pointer to firmware volume header.

Returns:

  EFI_VOLUME_CORRUPTED    - Signature is incorrect.
  EFI_UNSUPPORTED         - Revision is not compatible to current version.
                            File system is not supported.
  EFI_SUCCESS             - The files system is supported and valid.

--*/
{
  
  if (VolumeHeader->Signature != EFI_FVH_SIGNATURE) {
    DEBUG ((EFI_D_ERROR, "ValidateFvHeader(): Error : Bad signature in capsule volume header\n"));
    return EFI_VOLUME_CORRUPTED;
  }
  
  //
  // Make sure the revision is ok
  //
  if (VolumeHeader->Revision != EFI_FVH_REVISION) {
    DEBUG ((EFI_D_ERROR, "ValidateFvHeader(): Error : Bad revision in capsule volume header\n"));
    return EFI_UNSUPPORTED;
  }
  
  //
  // Make sure the file system guid is what we expect.
  //
  if (!EfiCompareGuid (&VolumeHeader->FileSystemGuid, &gEfiFirmwareFileSystemGuid)) {
    DEBUG ((EFI_D_ERROR, "ValidateFvHeader(): Error : Bad GUID in capsule volume header\n"));
    return EFI_UNSUPPORTED;
  }
  
  return EFI_SUCCESS;  
}


EFI_STATUS
RebasePeiExecutionFile (
  IN  VOID                    *ExecutionRawData,
  IN  UINT32                  ExecutionRawDataSize,
  IN  EFI_PHYSICAL_ADDRESS    RelocateFileBaseAddress,
  OUT EFI_PHYSICAL_ADDRESS    *EntryPoint
)
/*++

Routine Description:

  Do execution file raw image rebase and return entry point to caller.

Arguments:

  ExecutionRawData        - Pointer to the execution raw data
  ExecutionRawDataSize    - Size of raw data
  RelocateFileBaseAddress - The file base address of this execution image.
                            Actual Base address will adjust to execution image 
                            code section.
  EntryPoint              - Pointer to the entry point of this execution image 
                            based on the RelicationFileBaseAddress.

Returns:

  EFI_SUCCESS             - Rebase is complete and entry point address is in
                            EntryPoint parameter.
  EFI_ABORTED             - Need not to do rebase.

--*/
{
  EFI_COMMON_SECTION_HEADER     *SectionHeader;
  UINT32                        FileSize;
  UINT32                        SectionOffset;
  BOOLEAN                       IsTeImage;
  VOID*                         ImagePointer;
  UINT32                        ImageSize;
  EFI_TE_IMAGE_HEADER           *TEImageHeader;
  EFI_IMAGE_NT_HEADERS32        *PeHeader;
  EFI_IMAGE_BASE_RELOCATION     *RelocBase;
  EFI_IMAGE_BASE_RELOCATION     *RelocBaseEnd;
  EFI_IMAGE_DATA_DIRECTORY      *RelocDir;
  UINT64                        Adjust;
  UINT64                        SrcImageBase;
  UINT64                        DestImageBase;
  CHAR8                         *Fixup;
  CHAR8                         *FixupBase;
  UINTN                         Pe32AdjustHeaderSize;
  UINT16                        *F16;
  UINT32                        *F32;  
  UINT64                        *F64;
  UINT16                        *Reloc;
  UINT16                        *RelocEnd;
  UINTN                         SectionCount;
  UINTN                         SectionCounter;
  EFI_IMAGE_SECTION_HEADER      *SectionHeaders;

  *EntryPoint   = 0;
  ImagePointer  = NULL;
  ImageSize     = 0;
  IsTeImage     = FALSE;

  //
  // Find TE section or PE32 section.
  //
  for (SectionOffset = 0; SectionOffset < ExecutionRawDataSize; SectionOffset += ((FileSize + 3) / 4 * 4)) {

    SectionHeader = (EFI_COMMON_SECTION_HEADER *)((UINT8 *)ExecutionRawData + SectionOffset);
    FileSize = (*(UINT32 *)(SectionHeader->Size)) & 0x00FFFFFF;

    if (SectionHeader->Type == EFI_SECTION_TE) {
        IsTeImage     = TRUE;
        ImagePointer  = (VOID *)(SectionHeader + 1);
        ImageSize     = FileSize - sizeof(EFI_COMMON_SECTION_HEADER);
    } else if (SectionHeader->Type == EFI_SECTION_PE32) {
        IsTeImage     = FALSE;
        ImagePointer  = (VOID *)(SectionHeader + 1);
        ImageSize     = FileSize - sizeof(EFI_COMMON_SECTION_HEADER);
    }

  }

  //
  // No TE or PE32 section is found. The file may contain a compressed section.
  // The PE/COFF loader will handle loading & relocation in run time.
  // We need not to relocate it now.
  //
  if ((ImagePointer == NULL) || (ImageSize == 0)) {
    return EFI_ABORTED;
  }


  //
  // Start to Relocate
  //
  if (IsTeImage) {

    TEImageHeader = (EFI_TE_IMAGE_HEADER *)ImagePointer;

    ASSERT (TEImageHeader->Machine == EFI_IMAGE_MACHINE_IA32);
    ASSERT (TEImageHeader->StrippedSize >= sizeof (EFI_TE_IMAGE_HEADER));
    
    Pe32AdjustHeaderSize = TEImageHeader->StrippedSize - sizeof (EFI_TE_IMAGE_HEADER);
    SrcImageBase    = TEImageHeader->ImageBase;
    
    //
    // Kernel tool GenTEImage will fill different ImageBase for different version.
    // So, we need to use different code to correspond them.
    //   3.5B 's Image Base => Original PE Image Base address shift to TE image
    //                         Base.
    //   3.7  's Image Base => Original PE Image Base address. 
    //
    
    //
    // The following code is for Kernel 3.5B .
    //
    
    //DestImageBase   = RelocateFileBaseAddress 
    //                        + (UINT64)ImagePointer
    //                        - (UINT64)ExecutionRawData;
    
    //
    // The following code is for Kernel 3.7 .
    //
    DestImageBase   = RelocateFileBaseAddress 
                            + (UINT64)ImagePointer
                            - (UINT64)ExecutionRawData
                            - (UINT64)Pe32AdjustHeaderSize;
    *EntryPoint     = RelocateFileBaseAddress 
                            + (UINT64)ImagePointer
                            - (UINT64)ExecutionRawData 
                            + sizeof (EFI_TE_IMAGE_HEADER)
                            + TEImageHeader->AddressOfEntryPoint 
                            - TEImageHeader->StrippedSize;
    RelocDir        = TEImageHeader->DataDirectory + 0;
    RelocBase       = (EFI_IMAGE_BASE_RELOCATION *)((UINT8 *)ImagePointer 
                            + RelocDir->VirtualAddress 
                            - Pe32AdjustHeaderSize
                            );
    TEImageHeader->ImageBase = DestImageBase;

    //
    // TE image needs to reset relocation section flag to 0
    //
    SectionHeaders = (EFI_IMAGE_SECTION_HEADER *)((UINT8 *)ImagePointer + sizeof(EFI_TE_IMAGE_HEADER));
    SectionCount = TEImageHeader->NumberOfSections;
    for (SectionCounter = 0; SectionCounter < SectionCount; SectionCounter++) {
      if (SectionHeaders[SectionCounter].VirtualAddress == RelocDir->VirtualAddress) {
        SectionHeaders[SectionCounter].Characteristics = 0;
      }
    }

  } else {

    if (((EFI_IMAGE_DOS_HEADER *) ImagePointer)->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
      //
      // DOS image header is present, so read the PE header after the DOS image header
      //
      PeHeader = (EFI_IMAGE_NT_HEADERS32 *) ((UINTN) ImagePointer + (UINTN) ((((EFI_IMAGE_DOS_HEADER *) ImagePointer)->e_lfanew) & 0x0ffff));
    } else {
      //
      // DOS image header is not present, so PE header is at the image base
      //
      PeHeader = (EFI_IMAGE_NT_HEADERS32 *) ImagePointer;
    }

    ASSERT (PeHeader->FileHeader.Machine == EFI_IMAGE_MACHINE_IA32);

    Pe32AdjustHeaderSize = 0;
    SrcImageBase    = PeHeader->OptionalHeader.ImageBase;
    DestImageBase   = RelocateFileBaseAddress 
                            + (UINT64)ImagePointer
                            - (UINT64)ExecutionRawData;
    *EntryPoint     = RelocateFileBaseAddress 
                            + (UINT64)ImagePointer
                            - (UINT64)ExecutionRawData 
                            + (EFI_PHYSICAL_ADDRESS)(PeHeader->OptionalHeader.AddressOfEntryPoint & 0x0ffffffff);
    RelocDir        = PeHeader->OptionalHeader.DataDirectory + EFI_IMAGE_DIRECTORY_ENTRY_BASERELOC;
    RelocBase       = (EFI_IMAGE_BASE_RELOCATION *)((UINT8 *)ImagePointer 
                            + RelocDir->VirtualAddress 
                            - Pe32AdjustHeaderSize
                            );

    PeHeader->OptionalHeader.ImageBase = (UINT32)DestImageBase;

  }
  RelocBaseEnd  = (EFI_IMAGE_BASE_RELOCATION *)((UINT8 *)RelocBase + RelocDir->Size - 1);
  Adjust        = DestImageBase - SrcImageBase;

  //
  // Run the relocation information and apply the fixups
  //
  while (RelocBase < RelocBaseEnd) {

    //
    // Base Image address for curent directory entry based on original compiler assume.
    //
    FixupBase = (CHAR8 *)((UINTN)ImagePointer + RelocBase->VirtualAddress - Pe32AdjustHeaderSize );

    //
    // Run this relocation directory
    //
    for ( Reloc     = (UINT16 *)((CHAR8 *)RelocBase + sizeof(EFI_IMAGE_BASE_RELOCATION)),
          RelocEnd  = (UINT16 *)((CHAR8 *)RelocBase + RelocBase->SizeOfBlock);
          Reloc < RelocEnd;
          Reloc++ ) {

      //
      // Fixed data's address of current memory
      //
      Fixup = FixupBase + (*Reloc & 0xFFF);

      //
      // Fixup data should not overlapped relocation directory.
      // If this appears, we will skip to fixup.
      //
      if (((UINTN)(Fixup + 8) >= (UINTN)Reloc) &&
          ((UINTN)Fixup <= (UINTN)RelocEnd)
          ) {
        continue;
      }


      switch ((*Reloc) >> 12) {

      case EFI_IMAGE_REL_BASED_ABSOLUTE:
        break;

      case EFI_IMAGE_REL_BASED_HIGH:
        F16   = (UINT16 *) Fixup;
        *F16  = (UINT16) (*F16 + (UINT16)(((UINT32)Adjust) >> 16));
        break;

      case EFI_IMAGE_REL_BASED_LOW:
        F16   = (UINT16 *) Fixup;
        *F16  = (UINT16) (*F16 + (UINT16) Adjust);
        break;

      case EFI_IMAGE_REL_BASED_HIGHLOW:
        F32   = (UINT32 *) Fixup;
        *F32  = *F32 + (UINT32) Adjust;
        break;

      case EFI_IMAGE_REL_BASED_DIR64:
        //
        // For X64 and IPF
        //
        F64 = (UINT64 *) Fixup;
        *F64 = *F64 + (UINT64) Adjust;
        break;

      case EFI_IMAGE_REL_BASED_HIGHADJ:
        //
        // Out of support . Should not go here.
        //
        ASSERT (FALSE);
        break;

      default:
        //
        // Out of support . Should not go here.
        //
        ASSERT (FALSE);
        break;
      }

    }

    //
    // Next reloc block
    //
    RelocBase = (EFI_IMAGE_BASE_RELOCATION *)RelocEnd;
  }

  
  
  return EFI_SUCCESS;
  
}


EFI_STATUS
EmuPeiFvRebaseFindEntry (
  IN FAST_RECOVERY_DXE_TO_PEI_DATA *EmuPeiData,
  IN BOOLEAN                       NoNeedFindEntry
)
/*++

Routine Description:

  Rebase PEIMs in EmuPeiFv and find PeiCore and fake SEC entry points. 

Arguments:

  EmuPeiData              - Data that required for EmuSec/EmuPei.
  
                            Input fields:
                                EmuPeiFv
                                EmuPeiFvBase
                                EmuPeiFvSize
                                
                            Output fields:
                                EmuPeiEntry
                                EmuSecEntry
                                EmuSecFfsAddress
  
  NoNeedFindEntry         - The operation does not need to find entry point.
   
Returns:

  EFI_NOT_FOUND           - Can not find PeiCore or fake SEC entry point.
  EFI_VOLUME_CORRUPTED    - Signature is incorrect.
  EFI_UNSUPPORTED         - Revision is not compatible to current version.
                            File system is not supported.
  EFI_SUCCESS             - Rebase successful complete and Pei and fake Sec
                            entry points are found

--*/
{
  EFI_FIRMWARE_VOLUME_HEADER    *VolumeHeader;
  EFI_STATUS                    Status;
  EFI_FFS_FILE_HEADER           *FfsFileHeader;
  EFI_FFS_FILE_HEADER           FfsEndHeader;
  UINTN                         CurrentOffset;
  UINT32                        FfsFileSize;
  EFI_PHYSICAL_ADDRESS          ModuleEntryPoint;
  UINTN                         TailSize;
  UINTN                         FfsOccupiedSize;
  EFI_FFS_FILE_TAIL             TailValue;
  BOOLEAN                       InversePolarity;


  EmuPeiData->EmuPeiEntry = 0;
  EmuPeiData->EmuSecEntry = 0;
  EmuPeiData->EmuSecFfsAddress = 0;

  //
  // Check Fv file header.
  //
  VolumeHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)(EmuPeiData->EmuPeiFv));
  Status = ValidateFvHeader(VolumeHeader);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  //
  // Loop each file entry to rebase or delete.
  //
  InversePolarity = ((VolumeHeader->Attributes & EFI_FVB_ERASE_POLARITY) != 0) ? TRUE : FALSE; 
  for (FfsFileHeader = (EFI_FFS_FILE_HEADER *)((UINT8 *)(UINTN)EmuPeiData->EmuPeiFv + VolumeHeader->HeaderLength), CurrentOffset = VolumeHeader->HeaderLength;
       (CurrentOffset < EmuPeiData->EmuPeiFvSize) && (EfiCompareMem(FfsFileHeader, &FfsEndHeader, sizeof(EFI_FFS_FILE_HEADER)) != 0);
       FfsFileHeader = (EFI_FFS_FILE_HEADER *)(((UINT8*)(UINTN)EmuPeiData->EmuPeiFv) + CurrentOffset)) {

    FfsFileSize = (*((UINT32 *)FfsFileHeader->Size)) & 0xffffff; 
    if (FfsFileSize < sizeof(EFI_FFS_FILE_HEADER)) {
      break;
    }

    FfsOccupiedSize = ((FfsFileSize + 7) / 8 * 8);
    
    if (IsThisFileNeedToMask(&(FfsFileHeader->Name))) {
      //
      // Mark this file as deleted
      //
      FfsFileHeader->State =  EFI_FILE_DELETED | 
                              EFI_FILE_MARKED_FOR_UPDATE | 
                              EFI_FILE_DATA_VALID | 
                              EFI_FILE_HEADER_VALID | 
                              EFI_FILE_HEADER_CONSTRUCTION;

      if (InversePolarity) {
        FfsFileHeader->State = ~FfsFileHeader->State; 
      }
      
    } else if ((FfsFileHeader->Type == EFI_FV_FILETYPE_PEI_CORE) || (FfsFileHeader->Type == EFI_FV_FILETYPE_PEIM)) {
      //
      // Rebase this file
      //
      Status = RebasePeiExecutionFile (
                    (VOID *)(FfsFileHeader + 1),
                    FfsFileSize - sizeof(EFI_FFS_FILE_HEADER),
                    EmuPeiData->EmuPeiFvBase + CurrentOffset + sizeof(EFI_FFS_FILE_HEADER),
                    &ModuleEntryPoint
                    );
      if (!EFI_ERROR(Status)) {

        TailSize = ((FfsFileHeader->Attributes & FFS_ATTRIB_TAIL_PRESENT) != 0) ?
                          sizeof (EFI_FFS_FILE_TAIL) :
                          0;
        //
        // Update file check sum
        //
        if ((FfsFileHeader->Attributes & FFS_ATTRIB_CHECKSUM) != 0) {
          FfsFileHeader->IntegrityCheck.Checksum.File   = 0;
          FfsFileHeader->IntegrityCheck.Checksum.Header = 0;
        } else {
          FfsFileHeader->IntegrityCheck.Checksum.File   = FFS_FIXED_CHECKSUM;
          FfsFileHeader->IntegrityCheck.Checksum.Header = FFS_FIXED_CHECKSUM;
        }
        UpdateHeaderChecksum (FfsFileHeader);

        //
        // Update tail if present
        //
        if (FfsFileHeader->Attributes & FFS_ATTRIB_TAIL_PRESENT) {
          TailValue = (EFI_FFS_FILE_TAIL) (~(FfsFileHeader->IntegrityCheck.TailReference));
          *(EFI_FFS_FILE_TAIL *) (((UINTN) FfsFileHeader + FfsOccupiedSize - sizeof (EFI_FFS_FILE_TAIL))) = TailValue;
        }

        //
        // Update Ouptut fields
        //
        if ((FfsFileHeader->Type == EFI_FV_FILETYPE_PEI_CORE) && (ModuleEntryPoint != 0)) {
          EmuPeiData->EmuPeiEntry      = ModuleEntryPoint;
        } else if (EfiCompareGuid (&(FfsFileHeader->Name), &gEmuSecFileNameGuid)) {
          EmuPeiData->EmuSecEntry      = ModuleEntryPoint;
          EmuPeiData->EmuSecFfsAddress = (EFI_PHYSICAL_ADDRESS)FfsFileHeader; 
        }

      }
    }

    CurrentOffset = CurrentOffset + FfsOccupiedSize; 
  }       

  return ((NoNeedFindEntry || 
           ((EmuPeiData->EmuPeiEntry != 0) && (EmuPeiData->EmuSecEntry != 0))) 
          ? EFI_SUCCESS : EFI_NOT_FOUND); 
}


EFI_DRIVER_ENTRY_POINT (EmuPeiGate);

EFI_STATUS
EmuPeiGate (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
/*++
  Routine Description:
  
    Checking recovery status and transfer into EmuPei phase if we are just
    into recovery boot mode.

  Arguments:
  
    ImageHandle     - Handle for the image of this driver
    SystemTable     - Pointer to the EFI System Table

  Returns:
  
    EFI_SUCCESS     - Nothing need to do by this module.
        OR
    Never return    - Transfer control to EmuSec.
    
--*/
{

  EFI_STATUS                      Status;
  EFI_BOOT_MODE                   BootMode;
  VOID                            *HobList;
  UINTN                           *EmuGuidMark;
  UINTN                           EmuGuidMarkSize;
  VOID                            *EmuGuidMarkHob;
  EFI_PEI_HOB_POINTERS            RecoveryHob;
  EFI_FIRMWARE_VOLUME_HEADER      *EmuPeiVolumeHeader;
  FAST_RECOVERY_DXE_TO_PEI_DATA   *PeiData;
  EFI_PHYSICAL_ADDRESS            PeiDataAddress;
  EFI_PHYSICAL_ADDRESS            EmuPeiFvAddress;
  UINTN                           PeiFvSize;
  UINTN                           BiosRegionOffset;
  UINTN                           PeiFvBeginOffset;
  UINTN                           PeiFvEndOffset;
  UINTN                           RecoveryFvOffsetToPeiFv;
  VOID                            *FlashmapHob;
  EFI_FLASH_MAP_ENTRY_DATA        *FlashMapEntryData;
  EFI_PHYSICAL_ADDRESS            FlashItemBeginOffset;
  UINTN                           Index;

  BiosRegionOffset  = 0;

  EfiInitializeDriverLib (ImageHandle, SystemTable);
  EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  
  
  //
  // Skip execution futher process if we are 
  //   1. Not boot into recovery mode. (By checking boot mode)
  //   2. DXE phase after EmuPei phase. (By locating EmuPeiBootMark GUID Hob) 
  //
  Status = GetHobBootMode (HobList, &BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode != BOOT_IN_RECOVERY_MODE) {
    return EFI_SUCCESS;
  }
  
  EmuGuidMarkHob = HobList;
  Status = GetNextGuidHob (
      &EmuGuidMarkHob,
      &gEmuPeiMarkGuid,
      (VOID **)&EmuGuidMark,
      &EmuGuidMarkSize
      );
  if (Status == EFI_SUCCESS) {
    
    //
    // We just pass EmuPei phase. So, the Recovery PEI FV's base address is 
    // point to EmuPei FV in RAM. I change it back to ROM address for
    // BDS Recovery process. 
    //
    EFI_PHYSICAL_ADDRESS      RamToRomDifference;
    
    ASSERT (EmuGuidMarkSize >= sizeof(EFI_PHYSICAL_ADDRESS));    
    RamToRomDifference = *((EFI_PHYSICAL_ADDRESS *)EmuGuidMark);

    //
    // Change RAM address to ROM address
    //    
    FlashmapHob = HobList; 
    while (!EFI_ERROR(GetNextGuidHob (
                              &FlashmapHob, 
                              &gEfiFlashMapHobGuid, 
                              &FlashMapEntryData, 
                              NULL
                              ))) {
  
      if (FlashMapEntryData->AreaType == EFI_FLASH_AREA_RECOVERY_BIOS) {
        FlashMapEntryData->Entries[0].Base += RamToRomDifference;
      }
    }    
    return EFI_SUCCESS;
  }
  
  //
  // Find smallest and largest offset (in flash image) of PEI FVs by check
  // each FlashMap HOB items.
  //
  PeiFvBeginOffset = FLASH_REGION_FV_RECOVERY_OFFSET;
  PeiFvEndOffset = FLASH_REGION_FV_RECOVERY_OFFSET + FLASH_REGION_FV_RECOVERY_SIZE; 
  for (Index = 0; Index < NUM_FLASH_AREA_DATA; Index++) {
     if (mFlashAreaData[Index].AreaType != EFI_FLASH_AREA_RECOVERY_BIOS) {
       continue;
     }

     if ((mFlashAreaData[Index].Base - FLASH_BASE) < PeiFvBeginOffset) {
       PeiFvBeginOffset = (mFlashAreaData[Index].Base - FLASH_BASE);
     }

     if ((mFlashAreaData[Index].Base - FLASH_BASE + mFlashAreaData[Index].Length) > PeiFvEndOffset) {
       PeiFvEndOffset = (mFlashAreaData[Index].Base - FLASH_BASE + mFlashAreaData[Index].Length);
     }
  }

  ASSERT ((PeiFvBeginOffset != -1) && (PeiFvEndOffset != 0) && (PeiFvBeginOffset < PeiFvEndOffset));    
  PeiFvSize = PeiFvEndOffset - PeiFvBeginOffset; 
  RecoveryFvOffsetToPeiFv = FLASH_REGION_FV_RECOVERY_OFFSET - PeiFvBeginOffset;

  
  //
  // Find Recovery image pointer by locate recovery file HOB.
  //
  RecoveryHob.Raw  = HobList;
  RecoveryHob.Raw  = GetHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, RecoveryHob.Raw);
  while ((RecoveryHob.Raw != NULL) &&
         (RecoveryHob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION) &&
         (!EfiCompareGuid (
                &RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.Name,
                &gEfiRecoveryFileAddressGuid))) {

    RecoveryHob.Raw  = GET_NEXT_HOB (RecoveryHob);
    RecoveryHob.Raw  = GetHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, RecoveryHob.Raw);

  }
  
  if (RecoveryHob.Raw == NULL) 
  {
    ASSERT (RecoveryHob.Raw == NULL);
    return EFI_SUCCESS;
  }

  Status = GetBiosRegionFromCapsule (
          (VOID *)(UINTN)RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress,
          (UINTN) RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryLength,
          &BiosRegionOffset
          );

  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Allocate memory for Recovery only PEI FV
  //
  Status = gBS->AllocatePages(
                    AllocateAnyPages,
                    EfiBootServicesData,
                    EFI_SIZE_TO_PAGES(PeiFvSize),
                    &EmuPeiFvAddress
                    );
  ASSERT_EFI_ERROR (Status);
  EfiCommonLibCopyMem (
          (VOID *)(UINTN)EmuPeiFvAddress,
          ((UINT8 *)(UINTN)RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress) + BiosRegionOffset + PeiFvBeginOffset,
          PeiFvSize
          );

  //
  // Check the recovery image is valid by the following rules:
  //    1. The start address of possible EmuPei recovery Fv should be a valid
  //       Firmware Volume block.
  //    2. Other checking rule... Depends on the platform design. (TBD)  
  //
  EmuPeiVolumeHeader = (EFI_FIRMWARE_VOLUME_HEADER *)(((UINTN)EmuPeiFvAddress) + RecoveryFvOffsetToPeiFv);  
  Status = ValidateFvHeader(EmuPeiVolumeHeader);
  ASSERT_EFI_ERROR (Status);
  
  //
  // Allocate Pei Phase Data
  //
  Status = gBS->AllocatePages(
                    AllocateAnyPages,
                    EfiBootServicesData,
                    EFI_SIZE_TO_PAGES(sizeof(FAST_RECOVERY_DXE_TO_PEI_DATA) + EMU_CAR_STACK_SIZE),
                    &PeiDataAddress
                    );
  ASSERT_EFI_ERROR (Status);
  PeiData = (FAST_RECOVERY_DXE_TO_PEI_DATA *)(((UINTN)PeiDataAddress + EMU_CAR_STACK_SIZE - 1)/EMU_CAR_STACK_SIZE * EMU_CAR_STACK_SIZE);
  EfiZeroMem(PeiData,sizeof(FAST_RECOVERY_DXE_TO_PEI_DATA));
  
  //
  // Rebase PEIMs (except boot PEI FV)
  //
   for (Index = 0; Index < NUM_FLASH_AREA_DATA; Index++) {
     if (mFlashAreaData[Index].AreaType != EFI_FLASH_AREA_RECOVERY_BIOS) {
       continue;
     }

     if (mFlashAreaData[Index].Base != FLASH_REGION_FV_RECOVERY_BASE) {
       FlashItemBeginOffset = mFlashAreaData[Index].Base - FLASH_BASE;
       PeiData->EmuPeiFv = PeiData->EmuPeiFvBase 
                         = (EFI_PHYSICAL_ADDRESS)(((UINTN)EmuPeiFvAddress) 
                           + FlashItemBeginOffset 
                           - PeiFvBeginOffset);      
       PeiData->EmuPeiFvSize = mFlashAreaData[Index].Length;
  
        //
        // Rebase the PEIMs in FV
        // 
        Status = EmuPeiFvRebaseFindEntry(PeiData, TRUE);
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR(Status)) {
          return EFI_SUCCESS;
        }
     }
  }

  //
  // Rebase PEIMs (Boot FV) and find entry points
  //
  PeiData->EmuPeiFv = PeiData->EmuPeiFvBase 
                    = (EFI_PHYSICAL_ADDRESS)(((UINTN)EmuPeiFvAddress) + RecoveryFvOffsetToPeiFv); 
  PeiData->EmuPeiFvSize = FLASH_REGION_FV_RECOVERY_SIZE; 
  Status = EmuPeiFvRebaseFindEntry(PeiData, FALSE);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return EFI_SUCCESS;
  }
  
  //
  // Transfer to EmuSec phase to startup EmuPei .
  //
  PeiData->RecoveryImageAddress = RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress;
  PeiData->RecoveryImageSize = RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryLength;
  PeiData->OriginalHobList = (EFI_PHYSICAL_ADDRESS)HobList;
  PeiData->RomToRamDifference = FLASH_REGION_FV_RECOVERY_BASE - PeiData->EmuPeiFv; 
  Status = GoLegacyModePei(PeiData);
  ASSERT_EFI_ERROR (Status);

  //
  // Should not go here
  //
  EFI_DEADLOOP();
  
  return EFI_SUCCESS;
}

EFI_STATUS
GetBiosRegionFromCapsule (
  IN UINT8    *Capsule,
  IN UINTN    CapsuleSize,
  OUT UINTN   *BiosRegionOffset
)
/*++

Routine Description:

  Find out the BIOS region offset from capsule. 

Arguments:

  Capsule              - A pointer point to capsule buffer.
  CapsuleSize          - Capsule buffer size.
  BiosRegionOffset     - BIOS region offset from capsule buffer.
   
Returns:
  EFI_SUCCESS         
  
--*/
{
  // ---------------------------------------------
  //|start of BIOS image                          |
  //|                                             |
  //|                                             |
  //|                                             |
  //|                                             |
  //|                                             |
  //|                                             |
  //|---------------------------------------------|-
  //|BIOS Region                                  |
  //|                                             | FLASH_SIZE
  //|                                             |
  // ---------------------------------------------|- 
  *BiosRegionOffset = CapsuleSize - (UINTN)FLASH_SIZE;
  return EFI_SUCCESS;
}




