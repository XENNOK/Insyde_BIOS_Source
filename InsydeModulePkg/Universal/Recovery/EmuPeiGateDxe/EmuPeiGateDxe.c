/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Module Name:

    EmuPeiGate.c

Abstract:

  Fast recovery helper DXE driver. 
  Jump back to recovery only emulate PEI phase. Or do preparation for BDS's
  recovery process.


  **** Notes:
          The EDK1 code will use FlashMap to solve: 
            1. New / Old flash layout is different.
            2. It can get FlashMap HOB to get current running flash layout.
            3. It can use compile time flash layout by include flash out
               marco.
            4. Multiple PEI FV descriptions.
            
          I will modify EDK2 to not use FlashMap by these assumption:
            1. All PEI/DXE module could get correct FV values from Pcd flash
               layout values or PEI/DXE core or relative component.
            2. Pcd flash layout values always define the current running build's
               flash layout. Currently I assume they are same. It requires
               another facility to know running environment values.
            3. There is no multiple PEI FV. Only ONE.  
            
--*/

#include <Uefi.h>
#include <PiPei.h>
#include <Guid/FirmwareFileSystem.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/FirmwareFileSystem3.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <IndustryStandard/PeImage.h>
#include <FastRecoveryData.h>
#include <Guid/EmuSecFileName.h>
#include <Guid/EmuPeiMark.h>
#include <Guid/RecoveryFileAddress.h>
#include <Ppi/FirmwareAuthentication.h>

extern EFI_GUID EmuPeiMaskList[];
EFI_GUID *ChipsetEmuPeiMaskList = NULL;

EFI_STATUS 
EFIAPI
GoLegacyModePei (
  IN    FAST_RECOVERY_DXE_TO_PEI_DATA     *PhaseData,
  IN    UINTN                              RamSize
  );
    
EFI_STATUS 
UpdateHeaderChecksum (
  IN EFI_FFS_FILE_HEADER  *FileHeader
);

EFI_STATUS
GetBiosRegionFromCapsule (
  IN UINT8    *Capsule,
  IN UINTN    CapsuleSize,
  OUT UINTN   *BiosRegionOffset
);

/**

  Check specified file need to mask in recovery only PEI phase. 

  @param FileGuid                 Pointer to filename Guid.

  @retval TRUE                    This file need to mask and need not to dispatch.
  @retval FALSE                   This file should be dispatched as usual.

**/
BOOLEAN
IsThisFileNeedToMask (
  IN EFI_GUID             *FileGuid
)
{
  UINTN     Counter;
  BOOLEAN   FindItem = FALSE;
  EFI_GUID  EndGuid = {0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
  
  for (Counter = 0; !CompareGuid(EmuPeiMaskList + Counter, &EndGuid); ++Counter) {
    if (CompareGuid(EmuPeiMaskList + Counter, FileGuid)) {
      FindItem = TRUE;
      break;
    }
  }

  if (!FindItem && ChipsetEmuPeiMaskList) {
    for (Counter = 0; !CompareGuid(ChipsetEmuPeiMaskList + Counter, &EndGuid); ++Counter) {
      if (CompareGuid(ChipsetEmuPeiMaskList + Counter, FileGuid)) {
        FindItem = TRUE;
        break;
      }
    }
  }
  
  return FindItem; 
}

/**

  Update Ffs header's checksum. 

  @param FileHeader               Pointer to firmware file system entry header.

  @retval EFI_SUCCESS             Operation is successfully completed.

**/
EFI_STATUS 
UpdateHeaderChecksum (
  IN EFI_FFS_FILE_HEADER  *FileHeader
)
{
  UINT8   *ptr;
  UINTN   Index;
  UINT8   Sum;
  
  Sum = 0;
  ptr = (UINT8 *)FileHeader;
  FileHeader->IntegrityCheck.Checksum.Header = 0;

  for (Index = 0; 
       Index < (IS_FFS_FILE2 (FileHeader) ? sizeof(EFI_FFS_FILE_HEADER2) - 3 : sizeof(EFI_FFS_FILE_HEADER) - 3);
       Index += 4) {
    Sum = (UINT8)(Sum + ptr[Index]);
    Sum = (UINT8)(Sum + ptr[Index+1]);
    Sum = (UINT8)(Sum + ptr[Index+2]);
    Sum = (UINT8)(Sum + ptr[Index+3]);
  }

  for (; Index < (IS_FFS_FILE2 (FileHeader) ? sizeof(EFI_FFS_FILE_HEADER2) : sizeof(EFI_FFS_FILE_HEADER) ); Index++) {
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


/**

  Check if the input volume header is valid. 

  @param VolumeHeader              Pointer to firmware volume header.

  @retval EFI_VOLUME_CORRUPTED     Signature is incorrect.
  @retval EFI_UNSUPPORTED          Revision is not compatible to current version.
                                   File system is not supported.
  @retval EFI_SUCCESS              The files system is supported and valid.

**/
EFI_STATUS
ValidateFvHeader (
  IN EFI_FIRMWARE_VOLUME_HEADER *VolumeHeader
)
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
  if ((!CompareGuid (&VolumeHeader->FileSystemGuid, &gEfiFirmwareFileSystemGuid))
    &&(!CompareGuid (&VolumeHeader->FileSystemGuid, &gEfiFirmwareFileSystem2Guid))){
    DEBUG ((EFI_D_ERROR, "ValidateFvHeader(): Error : Bad GUID in capsule volume header\n"));
    return EFI_UNSUPPORTED;
  }
  
  return EFI_SUCCESS;  
}


/**

  Do execution file raw image rebase and return entry point to caller.

  @param File                     Pointer to the File header.
  @param ExecutionRawDataSize     Size of raw data
  @param RelocateFileBaseAddress  The file base address of this execution image.
                                  Actual Base address will adjust to execution image 
                                  code section.
  @param EntryPoint               Pointer to the entry point of this execution image 
                                  based on the RelicationFileBaseAddress.
                                  
  @retval EFI_SUCCESS             Rebase is complete and entry point address is in
                                  EntryPoint parameter.
  @retval EFI_ABORTED             Need not to do rebase.

**/
EFI_STATUS
RebasePeiExecutionFile (
  IN  VOID                    *File,
  IN  UINT32                  ExecutionRawDataSize,
  IN  EFI_PHYSICAL_ADDRESS    RelocateFileBaseAddress,
  OUT EFI_PHYSICAL_ADDRESS    *EntryPoint
)
{
  BOOLEAN                       IsTeImage;
  EFI_PHYSICAL_ADDRESS          ImagePointer;
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

  EFI_PHYSICAL_ADDRESS          EndOfSection;
  EFI_PHYSICAL_ADDRESS          CurrentAddress;
  EFI_COMMON_SECTION_HEADER     *Section;
  UINT32                        SectionSize;
  EFI_PHYSICAL_ADDRESS          ExecutionRawData;

  *EntryPoint   = 0;
  ImagePointer  = 0;
  ImageSize     = 0;
  IsTeImage     = FALSE;

  //
  // Loop through the FFS file sections within the FFS file
  //
  if (IS_FFS_FILE2 (File)) {
    EndOfSection = ExecutionRawData
                 = (EFI_PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) File + sizeof (EFI_FFS_FILE_HEADER2));
  } else {
    EndOfSection = ExecutionRawData
                 = (EFI_PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) File + sizeof (EFI_FFS_FILE_HEADER));
  }

  for (;;) {
    CurrentAddress = (EndOfSection + 3) & 0xfffffffffffffffcULL;
    Section = (EFI_COMMON_SECTION_HEADER*)(UINTN) CurrentAddress;

    if (IS_SECTION2 (Section)) {
      SectionSize  = SECTION2_SIZE (Section);
      if (SectionSize <= 0x00FFFFFF) {
        return EFI_NOT_FOUND;
      }
    } else {
      SectionSize  = SECTION_SIZE (Section);
      if (SectionSize < sizeof (EFI_COMMON_SECTION_HEADER)) {
        return EFI_NOT_FOUND;
      }
    }

    EndOfSection = CurrentAddress + SectionSize;
    if (EndOfSection > (ExecutionRawData + ExecutionRawDataSize)) {
      break;
    }

    //
    // Look for executable sections
    //
    if ((Section->Type == EFI_SECTION_TE)||(Section->Type == EFI_SECTION_PE32)) {
        IsTeImage     = (Section->Type == EFI_SECTION_TE) ? TRUE
                                                          : FALSE;
        ImagePointer  = IS_SECTION2 (Section) ? (PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER2))
                                              : (PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) Section + sizeof (EFI_COMMON_SECTION_HEADER));
        ImageSize     = IS_SECTION2 (Section) ? SectionSize - sizeof(EFI_COMMON_SECTION_HEADER2)
                                              : SectionSize - sizeof(EFI_COMMON_SECTION_HEADER);
    }

  }

  //
  // No TE or PE32 section is found. The file may contain a compressed section.
  // The PE/COFF loader will handle loading & relocation in run time.
  // We need not to relocate it now.
  //
  if ((ImagePointer == 0) || (ImageSize == 0)) {
    return EFI_ABORTED;
  }

  //
  // Start to Relocate
  //
  if (IsTeImage) {

    TEImageHeader = (EFI_TE_IMAGE_HEADER *)(UINTN)ImagePointer;

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
    RelocBase       = (EFI_IMAGE_BASE_RELOCATION *)((UINT8 *)(UINTN)ImagePointer 
                            + RelocDir->VirtualAddress 
                            - Pe32AdjustHeaderSize
                            );
    TEImageHeader->ImageBase = DestImageBase;

    //
    // TE image needs to reset relocation section flag to 0
    //
    SectionHeaders = (EFI_IMAGE_SECTION_HEADER *)((UINT8 *)(UINTN)ImagePointer + sizeof(EFI_TE_IMAGE_HEADER));
    SectionCount = TEImageHeader->NumberOfSections;
    for (SectionCounter = 0; SectionCounter < SectionCount; SectionCounter++) {
      if (SectionHeaders[SectionCounter].VirtualAddress == RelocDir->VirtualAddress) {
        SectionHeaders[SectionCounter].Characteristics = 0;
      }
    }

  } else {

    if (((EFI_IMAGE_DOS_HEADER *)(UINTN)ImagePointer)->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
      //
      // DOS image header is present, so read the PE header after the DOS image header
      //
      PeHeader = (EFI_IMAGE_NT_HEADERS32 *)((UINTN) ImagePointer + (UINTN) ((((EFI_IMAGE_DOS_HEADER *)(UINTN)ImagePointer)->e_lfanew) & 0x0ffff));
    } else {
      //
      // DOS image header is not present, so PE header is at the image base
      //
      PeHeader = (EFI_IMAGE_NT_HEADERS32 *)(UINTN)ImagePointer;
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
    RelocBase       = (EFI_IMAGE_BASE_RELOCATION *)((UINT8 *)(UINTN)ImagePointer 
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


/**

  Rebase PEIMs in EmuPeiFv and find PeiCore and fake SEC entry points. 

  @param EmuPeiData             Data that required for EmuSec/EmuPei.
  
                                  Input fields:
                                      EmuPeiFv
                                      EmuPeiFvBase
                                      EmuPeiFvSize
                                      
                                  Output fields:
                                      EmuPeiEntry
                                      EmuSecEntry
                                      EmuSecFfsAddress
  
  @param NoNeedFindEntry  The operation does not need to find entry point.

  @retval EFI_NOT_FOUND         Can not find PeiCore or fake SEC entry point.
  @retval EFI_VOLUME_CORRUPTED  Signature is incorrect.
  @retval EFI_UNSUPPORTED       Revision is not compatible to current version.
                                File system is not supported.
  @retval EFI_SUCCESS           Rebase successful complete and Pei and fake Sec
                                entry points are found

**/
EFI_STATUS
EmuPeiFvRebaseFindEntry (
  IN FAST_RECOVERY_DXE_TO_PEI_DATA *EmuPeiData,
  IN EFI_PHYSICAL_ADDRESS           EmuPeiFv
)
{
  EFI_FIRMWARE_VOLUME_HEADER    *VolumeHeader;
  EFI_STATUS                    Status;
  UINT32                        FfsFileSize;
  EFI_PHYSICAL_ADDRESS          ModuleEntryPoint;
  BOOLEAN                       InversePolarity;
  EFI_PHYSICAL_ADDRESS          CurrentAddress;
  EFI_PHYSICAL_ADDRESS          EndOfFirmwareVolume;
  EFI_PHYSICAL_ADDRESS          EndOfFile;
  EFI_FFS_FILE_HEADER           *File;

  //
  // Check Fv file header.
  //
  VolumeHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)EmuPeiFv);
  InversePolarity = ((VolumeHeader->Attributes & EFI_FVB2_ERASE_POLARITY) != 0) ? TRUE : FALSE; 
  Status = ValidateFvHeader(VolumeHeader);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  CurrentAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)VolumeHeader;
  EndOfFirmwareVolume = CurrentAddress + VolumeHeader->FvLength;

  //
  // Loop through the FFS files in the Emu Firmware Volume
  //
  for (EndOfFile = CurrentAddress + VolumeHeader->HeaderLength; ; ) {
    CurrentAddress = (EndOfFile + 7) & 0xfffffffffffffff8ULL;
    if (CurrentAddress >= EndOfFirmwareVolume) {
      break;
    }

    File = (EFI_FFS_FILE_HEADER*)(UINTN) CurrentAddress;
    if (IS_FFS_FILE2 (File)) {
      FfsFileSize = FFS_FILE2_SIZE (File);
      if (FfsFileSize <= 0x00FFFFFF) {
        return EFI_NOT_FOUND;
      }
    } else {
      FfsFileSize = FFS_FILE_SIZE (File);
      if (FfsFileSize < sizeof (EFI_FFS_FILE_HEADER)) {
        return EFI_NOT_FOUND;
      }
    }

    EndOfFile = CurrentAddress + FfsFileSize;

    //
    // Loop each file entry to rebase or delete.
    //
    if (IsThisFileNeedToMask(&(File->Name))) {
      //
      // Mark this file as deleted
      //
      File->State =  EFI_FILE_DELETED | 
                     EFI_FILE_MARKED_FOR_UPDATE | 
                     EFI_FILE_DATA_VALID | 
                     EFI_FILE_HEADER_VALID | 
                     EFI_FILE_HEADER_CONSTRUCTION;

      if (InversePolarity) {
        File->State = ~File->State; 
      }
      UpdateHeaderChecksum (File);
      
    } else if ((File->Type == EFI_FV_FILETYPE_PEI_CORE) || (File->Type == EFI_FV_FILETYPE_PEIM)) {
      //
      // Rebase this file
      //
      Status = RebasePeiExecutionFile (
                    (VOID *)File,
                    IS_FFS_FILE2 (File) ? FfsFileSize - sizeof(EFI_FFS_FILE_HEADER2)   : FfsFileSize - sizeof(EFI_FFS_FILE_HEADER),
                    IS_FFS_FILE2 (File) ? CurrentAddress + sizeof(EFI_FFS_FILE_HEADER2): CurrentAddress + sizeof(EFI_FFS_FILE_HEADER),
                    &ModuleEntryPoint
                    );
      
      if (!EFI_ERROR(Status)) {

        //
        // Update file check sum
        //
        if ((File->Attributes & FFS_ATTRIB_CHECKSUM) != 0) {
          File->IntegrityCheck.Checksum.File   = 0;
          File->IntegrityCheck.Checksum.Header = 0;
        } else {
          File->IntegrityCheck.Checksum.File   = FFS_FIXED_CHECKSUM;
          File->IntegrityCheck.Checksum.Header = FFS_FIXED_CHECKSUM;
        }
        UpdateHeaderChecksum (File);

        //
        // Update Ouptut fields
        //
        if ((File->Type == EFI_FV_FILETYPE_PEI_CORE) && (ModuleEntryPoint != 0)) {
          EmuPeiData->EmuPeiEntry      = ModuleEntryPoint;
        } else if (CompareGuid (&(File->Name), &gEmuSecFileNameGuid)) {
          EmuPeiData->EmuSecEntry      = ModuleEntryPoint;
          EmuPeiData->EmuSecFfsAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)File; 
        }
      }
    }
  }

  return EFI_SUCCESS;
}


/**
  
    Checking recovery status and transfer into EmuPei phase if we are just
    into recovery boot mode.
  
    @param ImageHandle      Handle for the image of this driver
    @param SystemTable      Pointer to the EFI System Table

    @retval EFI_SUCCESS     Nothing need to do by this module.
    @retval Never return    Transfer control to EmuSec.
    
**/
EFI_STATUS
EFIAPI
EmuPeiGateEntryPoint (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{

  EFI_STATUS                      Status;
  EFI_BOOT_MODE                   BootMode;
  VOID                            *HobList;
  UINTN                           *EmuGuidMark;
  EFI_PEI_HOB_POINTERS            RecoveryHob;
  EFI_FIRMWARE_VOLUME_HEADER      *EmuPeiVolumeHeader;
  FAST_RECOVERY_DXE_TO_PEI_DATA   *PeiData;
  EFI_PHYSICAL_ADDRESS            PeiDataAddress;
  EFI_PHYSICAL_ADDRESS            EmuPeiFvAddress;
  UINTN                           NumberOfPage;
  PEI_FV_DEFINITION               RecoveryRegion[5];
  EFI_PHYSICAL_ADDRESS            PeiFvBegin;
  EFI_PHYSICAL_ADDRESS            PeiFvEnd;
  UINTN                           PeiFvSize;
  EFI_PHYSICAL_ADDRESS            FlashItemBeginOffset;
  UINTN                           Index;
  UINTN                           BiosRegionOffset;
  EFI_FIRMWARE_VOLUME_HEADER      *VolumeHeader;
  EFI_PHYSICAL_ADDRESS            CurrentAddress;
  EFI_PHYSICAL_ADDRESS            CurrentFileAddress;
  EFI_PHYSICAL_ADDRESS            EndOfCapsule;
  EFI_PHYSICAL_ADDRESS            EndOfFirmwareVolume;
  UINT32                          FfsFileSize;
  EFI_FFS_FILE_HEADER             *File;
  UINT8                           MultiFvIndex;


  ZeroMem(RecoveryRegion, (sizeof (PEI_FV_DEFINITION) * 5));
  BiosRegionOffset = 0;
  MultiFvIndex = 0;

  //
  // Skip execution futher process if we are 
  //   1. Not boot into recovery mode. (By checking boot mode)
  //   2. DXE phase after EmuPei phase. (By locating EmuPeiBootMark GUID Hob) 
  //
  BootMode = GetBootModeHob();
  if (BootMode != BOOT_IN_RECOVERY_MODE) {
    return EFI_SUCCESS;
  }
  
  HobList = GetHobList();
  EmuGuidMark = GetNextGuidHob (&gEmuPeiMarkGuid, HobList);
  if (EmuGuidMark != NULL) {
    return EFI_SUCCESS;
  }
  
  //
  // Find Recovery image pointer by locate recovery file HOB.
  //
  RecoveryHob.Raw  = HobList;
  RecoveryHob.Raw  = GetNextHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, RecoveryHob.Raw);
  while ((RecoveryHob.Raw != NULL) &&
         (RecoveryHob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION) &&
         (!CompareGuid (
                &RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.Name,
                &gEfiRecoveryFileAddressGuid))) {

    RecoveryHob.Raw  = GET_NEXT_HOB (RecoveryHob);
    RecoveryHob.Raw  = GetNextHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, RecoveryHob.Raw);
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
// Go through whole capsule and find out total multi-PEI FV size.
//
  CurrentAddress = PeiFvEnd
                 = RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress + BiosRegionOffset;
  EndOfCapsule = PeiFvBegin
               = RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress
                 + RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryLength;
  while ( CurrentAddress < EndOfCapsule) {
    VolumeHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)CurrentAddress);
    Status = ValidateFvHeader(VolumeHeader);
    if (EFI_ERROR(Status)) {
      CurrentAddress += 0x10;
      continue;
    }

    EndOfFirmwareVolume = CurrentAddress + VolumeHeader->FvLength;
    CurrentFileAddress  = CurrentAddress + VolumeHeader->HeaderLength;

    //
    // Loop through the FFS files in this FV, and find out the EFI_FV_FILETYPE_PEIM.
    //
    while (1) {
      CurrentFileAddress = (CurrentFileAddress + 7) & 0xfffffffffffffff8ULL;
      if (CurrentFileAddress >= EndOfFirmwareVolume) {
        break;
      }

      File = (EFI_FFS_FILE_HEADER*)(UINTN) CurrentFileAddress;
      if (IS_FFS_FILE2 (File)) {
        FfsFileSize = FFS_FILE2_SIZE (File);
        if (FfsFileSize <= 0x00FFFFFF) {
          return EFI_NOT_FOUND;
        }
      } else {
        FfsFileSize = FFS_FILE_SIZE (File);
        if (FfsFileSize < sizeof (EFI_FFS_FILE_HEADER)) {
          return EFI_NOT_FOUND;
        }
      }

      if (File->Type != EFI_FV_FILETYPE_PEIM) {
        CurrentFileAddress += FfsFileSize;
        continue;
      } else {
        //
        // This FV is used in PEI phase. Record FV address info for re-base mechanism.
        //
        RecoveryRegion [MultiFvIndex].BaseAddress = CurrentAddress;
        RecoveryRegion [MultiFvIndex++].Length      = (UINT32)(EndOfFirmwareVolume - CurrentAddress);

        PeiFvBegin = (CurrentAddress < PeiFvBegin) ? CurrentAddress : PeiFvBegin;
        PeiFvEnd = (EndOfFirmwareVolume > PeiFvEnd) ? EndOfFirmwareVolume : PeiFvEnd; 
        break;
      }
    }
    
    CurrentAddress = EndOfFirmwareVolume;
  }
    
  PeiFvSize = (UINTN)(PeiFvEnd - PeiFvBegin); 

  //
  // Allocate memory for Recovery only PEI FV
  //
  Status = gBS->AllocatePages (AllocateAnyPages, EfiBootServicesData, EFI_SIZE_TO_PAGES(PeiFvSize), &EmuPeiFvAddress);
  ASSERT_EFI_ERROR (Status);
  CopyMem (
          (VOID *)(UINTN)EmuPeiFvAddress,
          (VOID *)(UINTN)PeiFvBegin, 
          PeiFvSize
          );

  //
  // Allocate Pei Phase Data
  //
  NumberOfPage = EFI_SIZE_TO_PAGES(sizeof(FAST_RECOVERY_DXE_TO_PEI_DATA) + PcdGet32(PcdTemporaryRamSizeWhenRecovery)) + 1;
  Status = gBS->AllocatePages(AllocateAnyPages, EfiBootServicesData, NumberOfPage, &PeiDataAddress);
  ASSERT_EFI_ERROR (Status);
  
  PeiData = (FAST_RECOVERY_DXE_TO_PEI_DATA *)(UINTN)PeiDataAddress;
  ZeroMem (PeiData, NumberOfPage * EFI_PAGE_SIZE);
  PeiData->StackData = PeiDataAddress + sizeof(FAST_RECOVERY_DXE_TO_PEI_DATA);
  PeiData->StackData = (PeiData->StackData + 0x1F) & ~((EFI_PHYSICAL_ADDRESS)0x1F);
  
  for (Index = 0; Index < MultiFvIndex; ++Index) {
    FlashItemBeginOffset = RecoveryRegion[Index].BaseAddress - PeiFvBegin;

    //
    // Check the recovery image is valid by the following rules:
    //    1. The start address of possible EmuPei recovery Fv should be a valid
    //       Firmware Volume block.
    //    2. Other checking rule... Depends on the platform design. (TBD)  
    //
    EmuPeiVolumeHeader = (EFI_FIRMWARE_VOLUME_HEADER *)((UINTN)(EmuPeiFvAddress + FlashItemBeginOffset));
    Status = ValidateFvHeader(EmuPeiVolumeHeader);
    ASSERT_EFI_ERROR (Status);

    //
    // Rebase PEIMs (Boot FV) and find entry points
    //
    Status = EmuPeiFvRebaseFindEntry(PeiData, (EFI_PHYSICAL_ADDRESS)(((UINTN)EmuPeiFvAddress) + FlashItemBeginOffset));
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) {
      return EFI_SUCCESS;
    }

    if (PeiData->EmuPeiFv == 0 && PeiData->EmuPeiEntry != 0) {
      PeiData->EmuPeiFv = PeiData->EmuPeiFvBase
                        = (EFI_PHYSICAL_ADDRESS)(((UINTN)EmuPeiFvAddress) + FlashItemBeginOffset);      
      PeiData->EmuPeiFvSize = RecoveryRegion[Index].Length;
    }
  }

  ASSERT ((PeiData->EmuPeiEntry != 0) && (PeiData->EmuSecEntry != 0));
  if ((PeiData->EmuPeiEntry == 0) || (PeiData->EmuSecEntry == 0)) {
    return EFI_SUCCESS;
  }
  
  //
  // Transfer to EmuSec phase to startup EmuPei .
  //
  PeiData->RecoveryImageAddress = RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryBaseAddress;
  PeiData->RecoveryImageSize = RecoveryHob.MemoryAllocationModule->MemoryAllocationHeader.MemoryLength;
  PeiData->OriginalHobList = (EFI_PHYSICAL_ADDRESS)(UINTN)HobList;
  PeiData->RomToRamDifference =  PcdGet32(PcdFlashFvRecoveryBase) - PeiData->EmuPeiFv; 
  Status = GoLegacyModePei(PeiData, (UINTN) PcdGet32(PcdTemporaryRamSizeWhenRecovery));
  ASSERT_EFI_ERROR (Status);

  //
  // Should not go here
  //
  CpuDeadLoop();
  
  return EFI_SUCCESS;
}

/**

  Find out the BIOS region offset from different capsule format.
    1. BIOS Region.
    2. BIOS Image (Whole SPI Image).
    3. Binary signed by Insyde sign tool.

  @param Capsule                A pointer point to capsule buffer.
  @param CapsuleSize            Capsule buffer size.
  @param BiosRegionOffset       The BIOS region offset from capusle buffer.

  @retval EFI_NOT_FOUND         Capsule image is incorrect. 
  @retval EFI_SUCCESS           Find out the offset value. 

**/EFI_STATUS
GetBiosRegionFromCapsule (
  IN UINT8    *Capsule,
  IN UINTN    CapsuleSize,
  OUT UINTN   *BiosRegionOffset
)
{
  UINTN                       Index;
  ISFLASH_DATA_REGION_HEADER  *DataRegion;
  UINTN                       BiosImageSize;
  UINTN                       BiosImageOffset;

  Index      = 0;
  DataRegion = NULL;
  BiosImageSize = 0;  
  BiosImageOffset = 0;

  if (FeaturePcdGet(PcdSecureFlashSupported) && 
     FeaturePcdGet(PcdRecoveryHobReportsEntireCapsule) && 
     ((Capsule[0] == (UINT8)'M') && (Capsule[1] == (UINT8)'Z'))) {
    //
    // Capsule is signed by Insyde sign tool.
    //
    // ---------------------------------------------
    //| 'M','Z'                                     |
    //|                                             |
    // ---------------------------------------------|
    //|ISFLASH_BIOS_IMAGE_TAG                       | 
    //|---------------------------------------------| 
    //|UINT32 AllocatedSize  |  UINT32 DataSize     |
    //|---------------------- ----------------------|
    //|start of BIOS image                          |
    //|                                             |
    //|                                             |
    //|                                             |
    //|                                             | 
    //|                                             | 
    //|                                             |
    //|---------------------------------------------|
    //|BIOS Region                                  |
    //|                                             |
    //|                                             |
    //|---------------------------------------------| 
    //|ISFLASH_IMG_SIGNATURE_TAG                    |
    //|---------------------------------------------|
    //|UINT32 AllocatedSize  |  UINT32 DataSize     |
    //|---------------------- ----------------------|
    //|Certification                                |
    // ---------------------------------------------
    //
    // Search for BIOS image
    //
    for (Index = 0; Index < CapsuleSize - ISFLASH_TAG_SIZE; Index++) {
      if (CompareMem(Capsule + Index, ISFLASH_BIOS_IMAGE_TAG, ISFLASH_TAG_SIZE) == 0) {
        break;
      }
    }
    if (Index == CapsuleSize - ISFLASH_TAG_SIZE) {
      return EFI_NOT_FOUND;
    }

    //
    // Get BIOS Region offset
    //
    DataRegion = (ISFLASH_DATA_REGION_HEADER *)(Capsule + Index);
    BiosImageOffset = Index + sizeof(ISFLASH_DATA_REGION_HEADER);
    BiosImageSize = (UINTN)DataRegion->DataSize;
    if (BiosImageOffset + BiosImageSize > CapsuleSize) {
      return EFI_NOT_FOUND;
    }

    *BiosRegionOffset = BiosImageOffset + BiosImageSize - (UINTN)PcdGet32 (PcdFlashAreaSize);
  } else {
    // 
    // Capsule is a BIOS image.
    //
    // ---------------------------------------------
    //|start of BIOS image                          |
    //|                                             |
    //|                                             |
    //|                                             |
    //|                                             |
    //|                                             |
    //|                                             |
    //|---------------------------------------------|
    //|BIOS Region                                  |
    //|                                             |
    //|                                             |
    // --------------------------------------------- 
    *BiosRegionOffset = CapsuleSize - (UINTN)PcdGet32 (PcdFlashAreaSize);
  }

  return EFI_SUCCESS;
}

