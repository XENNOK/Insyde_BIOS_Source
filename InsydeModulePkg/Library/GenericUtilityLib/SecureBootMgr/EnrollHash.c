/** @file
  GenericUtilityLib

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

#include "EnrollHash.h"


#define TWO_BYTE_ENCODE                   0x82
#define SHORT_FORM_MASK                   0x80

ENROLL_MENU_OPTION         mFsOptionMenu = {
  EM_MENU_ENTRY_SIGNATURE,
  NULL,
  0
};
ENROLL_MENU_OPTION         mDirectoryMenu =  {
  EM_MENU_ENTRY_SIGNATURE,
  NULL,
  0
};
EFI_HII_UPDATE_DATA        mUpdateData;
EFI_TEXT_STRING            mOriginalOutputString;


UINT8 mHashOidValue[] = {
  0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x02, 0x05,         // OBJ_md5
  0x2B, 0x0E, 0x03, 0x02, 0x1A,                           // OBJ_sha1
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x04,   // OBJ_sha224
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01,   // OBJ_sha256
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02,   // OBJ_sha384
  0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03,   // OBJ_sha512
  };

HASH_TABLE mHash[] = {
  { &gEfiHashAlgorithmMD5Guid,    L"MD5",    16, &mHashOidValue[0],  8},
  { &gEfiHashAlgorithmSha1Guid,   L"SHA1",   20, &mHashOidValue[8],  5},
  { &gEfiHashAlgorithmSha224Guid, L"SHA224", 28, &mHashOidValue[13], 9},
  { &gEfiHashAlgorithmSha256Guid, L"SHA256", 32, &mHashOidValue[22], 9},
  { &gEfiHashAlgorithmSha384Guid, L"SHA384", 48, &mHashOidValue[31], 9},
  { &gEfiHashAlgorithmSha512Guid, L"SHA512", 64, &mHashOidValue[40], 9}
};

ENROLL_MENU_ENTRY *
EmCreateFileMenuEntry (
  VOID
  );
;
VOID
EmSafeFreePool (
  IN VOID    **Buffer
  )
;

VOID
EmDestroyMenuEntry (
  ENROLL_MENU_ENTRY         *MenuEntry
  )
;

EFI_STATUS
EnrollHashImage (
  EM_FILE_CONTEXT     *FileContext
  );


/**
  Wrap original FreePool gBS call
  in order to decrease code length
  (without setting back Buffer to NULL).

**/
VOID
EmSafeFreePool (
  IN VOID    **Buffer
  )
{
  if (*Buffer != NULL) {
    gBS->FreePool (*Buffer);
    *Buffer = NULL;
  }
}


/**
  Destroy the menu entry passed in

  @param  MenuEntry         The menu entry need to be destroyed

**/
VOID
EmDestroyMenuEntry (
  ENROLL_MENU_ENTRY         *MenuEntry
  )
{
  EM_FILE_CONTEXT           *FileContext;

  FileContext = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;

  if (!FileContext->IsRoot) {
    EmSafeFreePool ((VOID **) &FileContext->DevicePath);
  } else {
    if (FileContext->FHandle != NULL) {
      FileContext->FHandle->Close (FileContext->FHandle);
    }
  }

  EmSafeFreePool ((VOID **) &FileContext->FileName);
  EmSafeFreePool ((VOID **) &FileContext->Info);
  EmSafeFreePool ((VOID **) &FileContext);
  EmSafeFreePool ((VOID **) &MenuEntry->DisplayString);
  EmSafeFreePool ((VOID **) &MenuEntry->HelpString);
  EmSafeFreePool ((VOID **) &MenuEntry);
}


/**
  Check this ROM is a  EFI format image or not

  @param  Buffer        Pointer to the image buffer
  @param  BufferSize    The image size
  @param  IsSigned      TRUE: It is a signed EFI image
                        FALSE: It isn't a signed EFI image.

  @retval TRUE          This image is EFI format image.
  @retval FALSE         This image isn't EFI format image.

**/
BOOLEAN
IsEfiImage (
  IN  UINT8      *Buffer,
  IN  UINTN      BufferSize,
  OUT BOOLEAN    *IsSigned
  )
{
  EFI_IMAGE_DOS_HEADER                     *DosHdr;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION      NtHeader;
  EFI_IMAGE_DATA_DIRECTORY                 *SecDataDir;
  UINTN                                    PeCoffHeaderOffset;
  UINT16                                   Magic;
  BOOLEAN                                  IsSignedEfi;
  BOOLEAN                                  IsEfi;

  IsSignedEfi = FALSE;
  IsEfi       = FALSE;
  SecDataDir  = NULL;

  DosHdr = (EFI_IMAGE_DOS_HEADER *) Buffer;
  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    //
    // DOS image header is present,
    // so read the PE header after the DOS image header.
    //
    PeCoffHeaderOffset = DosHdr->e_lfanew;
  } else {
    PeCoffHeaderOffset = 0;
  }
  //
  // Check PE/COFF image.
  //
  NtHeader.Pe32 = (EFI_IMAGE_NT_HEADERS32 *) (Buffer + PeCoffHeaderOffset);
  if (NtHeader.Pe32->Signature == EFI_IMAGE_NT_SIGNATURE) {
    Magic = NtHeader.Pe32->OptionalHeader.Magic;
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset.
      //
      SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &NtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
      IsEfi = TRUE;
    }

    if (IsEfi && SecDataDir->Size != 0) {
      //
      // This image is signed.
      //
      IsSignedEfi = TRUE;
    }
  }
  NtHeader.Pe32Plus = (EFI_IMAGE_NT_HEADERS64 *) (Buffer + PeCoffHeaderOffset);
  if (NtHeader.Pe32Plus->Signature == EFI_IMAGE_NT_SIGNATURE) {
    Magic = NtHeader.Pe32Plus->OptionalHeader.Magic;
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      //
      // Use PE32 offset.
      //
      SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &NtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
      IsEfi = TRUE;
    }

    if (IsEfi && SecDataDir->Size != 0) {
      //
      // This image is signed.
      //
      IsSignedEfi = TRUE;
    }
  }
  *IsSigned = IsSignedEfi;

  return IsEfi;
}


/**
  Check this ROM is a  EFI format image or not

  @param  HashAlg         Hash algorithm type
  @param  ImageDigest     Pointer to EFI_HASH_OUTPUT instance.

  @retval EFI_SUCCESS             add hash image to link list successful.
  @retval EFI_INVALID_PARAMETER   Any input parameter is invalid.
  @retval EFI_UNSUPPORTED         The input hash type is unsupported.

**/
EFI_STATUS
AddHashDataToList (
  IN  UINT32              HashAlg,
  IN  EFI_HASH_OUTPUT     *ImageDigest
)
{
  HASH_LINK_LIST        *HashLink;

  if (ImageDigest == NULL || ImageDigest->Sha256Hash == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (HashAlg != HASHALG_SHA1 && HashAlg != HASHALG_SHA256) {
    return EFI_UNSUPPORTED;
  }

  HashLink            = AllocateZeroPool (sizeof (HASH_LINK_LIST));
  ASSERT (HashLink != NULL);
  if (HashLink == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  HashLink->Signature = HASH_LINK_LIST_SIGNATURE;
  HashLink->ShaType   = HashAlg;
  HashLink->HashSize  = (HashAlg == HASHALG_SHA1) ? sizeof (EFI_SHA1_HASH) : sizeof (EFI_SHA256_HASH);
  CopyMem (HashLink->Hash, ImageDigest->Sha256Hash, HashLink->HashSize);
  InsertTailList (&mHashLinkList, &HashLink->Link);
  return EFI_SUCCESS;

}


/**
  Caculate hash of Pe/Coff image based on the authenticode image hashing in
  PE/COFF Specification 8.0 Appendix A

  @param[in]    HashAlg   Hash algorithm type.

  @retval TRUE            Successfully hash image.
  @retval FALSE           Fail in hash image.

**/
BOOLEAN
HashPeImage (
  IN  UINT8               *Buffer,
  IN  UINTN               BufferSize,
  IN  UINT32              HashAlg
  )
{
  EFI_STATUS                              Status;
  UINT16                                  Magic;
  EFI_IMAGE_SECTION_HEADER                *Section;
  EFI_IMAGE_SECTION_HEADER                *NextSection;
  UINT8                                   *HashBase;
  UINTN                                   HashSize;
  UINTN                                   SumOfBytesHashed;
  EFI_IMAGE_SECTION_HEADER                *SectionHeader;
  UINTN                                   Index;
  UINTN                                   Pos;
  EFI_HASH_PROTOCOL                       *Hash;
  EFI_GUID                                CertType;
  UINTN                                   ImageDigestSize;
  EFI_HASH_OUTPUT                         *ImageDigest;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION     NtHeader;
  EFI_IMAGE_DOS_HEADER                    *DosHdr;
  UINTN                                   PeCoffHeaderOffset;
  UINT32                                  CertSize;
  UINT32                                  NumberOfRvaAndSizes;

  SectionHeader = NULL;
  ImageDigest   = NULL;

  if ((HashAlg != HASHALG_SHA1) && (HashAlg != HASHALG_SHA256)) {
    return FALSE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHashProtocolGuid,
                  NULL,
                  (VOID **) &Hash
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Initialize context of hash.
  //
  if (HashAlg == HASHALG_SHA1) {
   CertType         = gEfiCertSha1Guid;
  } else if (HashAlg == HASHALG_SHA256) {
   CertType         = gEfiCertSha256Guid;
  } else {
    return FALSE;
  }
  Status  = Hash->GetHashSize (
                    Hash,
                    mHash[HashAlg].Index,
                    &ImageDigestSize
                    );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  ImageDigest = AllocateZeroPool (sizeof (EFI_HASH_OUTPUT));
  if (ImageDigest == NULL) {
    return FALSE;
  }
  ImageDigest->Sha256Hash = AllocateZeroPool (sizeof (EFI_SHA256_HASH));
  if (ImageDigest->Sha256Hash == NULL) {
    EmSafeFreePool ((VOID **) &ImageDigest);
    return FALSE;
  }

  DosHdr = (EFI_IMAGE_DOS_HEADER *) Buffer;
  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    //
    // DOS image header is present,
    // so read the PE header after the DOS image header.
    //
    PeCoffHeaderOffset = DosHdr->e_lfanew;
  } else {
    PeCoffHeaderOffset = 0;
  }
  //
  // Check PE/COFF image.
  //
  NtHeader.Pe32 = (EFI_IMAGE_NT_HEADERS32 *) (Buffer + PeCoffHeaderOffset);
  //
  // Measuring PE/COFF Image Header;
  // But CheckSum field and SECURITY data directory (certificate) are excluded
  //
  if (NtHeader.Pe32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64 && NtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // NOTE: Some versions of Linux ELILO for Itanium have an incorrect magic value
    //       in the PE/COFF Header. If the MachineType is Itanium(IA64) and the
    //       Magic value in the OptionalHeader is EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC
    //       then override the magic value to EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC
    //
    Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  } else {
    //
    // Get the magic value from the PE/COFF Optional Header
    //
    Magic =  NtHeader.Pe32->OptionalHeader.Magic;
  }
  //
  // 3.  Calculate the distance from the base of the image header to the image checksum address.
  // 4.  Hash the image header from its base to beginning of the image checksum.
  //
  HashBase = Buffer;
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    HashSize = (UINTN) ((UINT8 *) (&NtHeader.Pe32->OptionalHeader.CheckSum) - HashBase);
    NumberOfRvaAndSizes = NtHeader.Pe32->OptionalHeader.NumberOfRvaAndSizes;
  } else {
    //
    // Use PE32+ offset.
    //
    HashSize = (UINTN) ((UINT8 *) (&NtHeader.Pe32Plus->OptionalHeader.CheckSum) - HashBase);
    NumberOfRvaAndSizes = NtHeader.Pe32Plus->OptionalHeader.NumberOfRvaAndSizes;
  }

  Status = Hash->Hash (
                   Hash,
                   mHash[HashAlg].Index,
                   FALSE,
                   HashBase,
                   HashSize,
                   (EFI_HASH_OUTPUT *) ImageDigest
                   );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // 5.  Skip over the image checksum (it occupies a single ULONG).
  //
  if (NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_SECURITY) {
    //
    // 6.  Since there is no Cert Directory in optional header, hash everything
    //     from the end of the checksum to the end of image header.
    //
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset.
      //
      HashBase = (UINT8 *) &NtHeader.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
      HashSize = NtHeader.Pe32->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - Buffer);
    } else {
      //
      // Use PE32+ offset.
      //
      HashBase = (UINT8 *) &NtHeader.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
      HashSize = NtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - Buffer);
    }

    if (HashSize != 0) {
      Status = Hash->Hash (
                       Hash,
                       mHash[HashAlg].Index,
                       TRUE,
                       HashBase,
                       HashSize,
                       (EFI_HASH_OUTPUT *) ImageDigest
                       );
      if (EFI_ERROR (Status)) {
        goto Done;
      }
    }
  } else {
    //
    // 7.  Hash everything from the end of the checksum to the start of the Cert Directory.
    //
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset.
      //
      HashBase = (UINT8 *) &NtHeader.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
      HashSize = (UINTN) ((UINT8 *) (&NtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
    } else {
      //
      // Use PE32+ offset.
      //
      HashBase = (UINT8 *) &NtHeader.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
      HashSize = (UINTN) ((UINT8 *) (&NtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
    }
    if (HashSize != 0) {
      Status = Hash->Hash (
                       Hash,
                       mHash[HashAlg].Index,
                       TRUE,
                       HashBase,
                       HashSize,
                       (EFI_HASH_OUTPUT *) ImageDigest
                       );


      if (EFI_ERROR (Status)) {
        goto Done;
      }
    }
    //
    // 8.  Skip over the Cert Directory. (It is sizeof(IMAGE_DATA_DIRECTORY) bytes.)
    // 9.  Hash everything from the end of the Cert Directory to the end of image header.
    //
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset
      //
      HashBase = (UINT8 *) &NtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
      HashSize = NtHeader.Pe32->OptionalHeader.SizeOfHeaders - (UINTN) ((UINT8 *) (&NtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - Buffer);
    } else {
      //
      // Use PE32+ offset.
      //
      HashBase = (UINT8 *) &NtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
      HashSize = NtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders - (UINTN) ((UINT8 *) (&NtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1]) - Buffer);
    }
    if (HashSize != 0) {
      Status = Hash->Hash (
                       Hash,
                       mHash[HashAlg].Index,
                       TRUE,
                       HashBase,
                       HashSize,
                       (EFI_HASH_OUTPUT *) ImageDigest
                       );

      if (EFI_ERROR (Status)) {
        goto Done;
      }
    }
  }

  //
  // 10. Set the SUM_OF_BYTES_HASHED to the size of the header.
  //
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    SumOfBytesHashed = NtHeader.Pe32->OptionalHeader.SizeOfHeaders;
  } else {
    //
    // Use PE32+ offset
    //
    SumOfBytesHashed = NtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders;
  }

  //
  // 11. Build a temporary table of pointers to all the IMAGE_SECTION_HEADER
  //     structures in the image. The 'NumberOfSections' field of the image
  //     header indicates how big the table should be. Do not include any
  //     IMAGE_SECTION_HEADERs in the table whose 'SizeOfRawData' field is zero.
  //
  SectionHeader = (EFI_IMAGE_SECTION_HEADER *) AllocateZeroPool (sizeof (EFI_IMAGE_SECTION_HEADER) * NtHeader.Pe32->FileHeader.NumberOfSections);
  ASSERT (SectionHeader != NULL);
  if (SectionHeader == NULL) {
    EmSafeFreePool ((VOID **) &ImageDigest);
    EmSafeFreePool ((VOID **) &ImageDigest->Sha256Hash);
    return FALSE;
  }
  //
  // 12.  Using the 'PointerToRawData' in the referenced section headers as
  //      a key, arrange the elements in the table in ascending order. In other
  //      words, sort the section headers according to the disk-file offset of
  //      the section.
  //
  Section = (EFI_IMAGE_SECTION_HEADER *) (
               Buffer +
               PeCoffHeaderOffset +
               sizeof (UINT32) +
               sizeof (EFI_IMAGE_FILE_HEADER) +
               NtHeader.Pe32->FileHeader.SizeOfOptionalHeader
               );
  for (Index = 0; Index < NtHeader.Pe32->FileHeader.NumberOfSections; Index++) {
    Pos = Index;
    while ((Pos > 0) && (Section->PointerToRawData < SectionHeader[Pos - 1].PointerToRawData)) {
      CopyMem (&SectionHeader[Pos], &SectionHeader[Pos - 1], sizeof (EFI_IMAGE_SECTION_HEADER));
      Pos--;
    }
    CopyMem (&SectionHeader[Pos], Section, sizeof (EFI_IMAGE_SECTION_HEADER));
    Section += 1;
  }

  //
  // 13.  Walk through the sorted table, bring the corresponding section
  //      into memory, and hash the entire section (If two section data isn't
  //      continuous, the 'SizeOfRawData' field in the section header is incorrect.
  //      Therefore, We use the whole data between two sections.).
  // 14.  Add the section's hash size to SUM_OF_BYTES_HASHED .
  // 15.  Repeat steps 13 and 14 for all the sections in the sorted table.
  //
  for (Index = 0; Index < NtHeader.Pe32->FileHeader.NumberOfSections; Index++) {
    Section = &SectionHeader[Index];
    if (Section->SizeOfRawData == 0) {
      continue;
    }
    HashBase  = Buffer + Section->PointerToRawData;
    if (Index != (UINTN) (NtHeader.Pe32->FileHeader.NumberOfSections - 1)) {
      NextSection = &SectionHeader[Index + 1];
      HashSize  = (UINTN) NextSection->PointerToRawData - (UINTN) Section->PointerToRawData;
    } else {
      HashSize  = (UINTN) Section->SizeOfRawData;
    }
    Status = Hash->Hash (
                     Hash,
                     mHash[HashAlg].Index,
                     TRUE,
                     HashBase,
                     HashSize,
                     (EFI_HASH_OUTPUT *) ImageDigest
                     );

  if (EFI_ERROR (Status)) {
    goto Done;
  }

    SumOfBytesHashed += HashSize;
  }

  //
  // 16.  If the file size is greater than SUM_OF_BYTES_HASHED, there is extra
  //      data in the file that needs to be added to the hash. This data begins
  //      at file offset SUM_OF_BYTES_HASHED and its length is:
  //             FileSize  -  (CertDirectory->Size)
  //
  if (BufferSize > SumOfBytesHashed) {
    HashBase = Buffer + SumOfBytesHashed;
    if (NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_SECURITY) {
      CertSize = 0;
    } else {
      if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        //
        // Use PE32 offset.
        //
        CertSize = NtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
      } else {
        //
        // Use PE32+ offset.
        //
        CertSize = NtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
      }
      if (BufferSize > CertSize + SumOfBytesHashed) {
        HashSize = (UINTN) (BufferSize - CertSize - SumOfBytesHashed);
        Status = Hash->Hash (
                         Hash,
                         mHash[HashAlg].Index,
                         TRUE,
                         HashBase,
                         HashSize,
                         (EFI_HASH_OUTPUT *) ImageDigest
                         );
        if (EFI_ERROR (Status)) {
          goto Done;
        }
      } else if (BufferSize < CertSize + SumOfBytesHashed) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
    }
  }

  Status = AddHashDataToList (HashAlg, ImageDigest);


Done:

  EmSafeFreePool ((VOID **) &SectionHeader);
  EmSafeFreePool ((VOID **) &ImageDigest->Sha256Hash);
  EmSafeFreePool ((VOID **) &ImageDigest);
  return EFI_ERROR (Status) ? FALSE : TRUE;
}


/**
  Recognize the Hash algorithm in PE/COFF Authenticode and caculate hash of
  Pe/Coff image based on the authenticode image hashing in PE/COFF Specification
  8.0 Appendix A

  @param  Buffer            Pointer to the image buffer.
  @param  BufferSize        The image size.
  @param  AuthData          Pointer to the Authenticode Signature retrieved from signed image.
  @param  AuthDataSize      Size of the Authenticode Signature in bytes.

  @retval EFI_SUCCESS       Hash successfully.
  @retval EFI_UNSUPPORTED   Hash algorithm is not supported.
  @retval EFI_UNSUPPORTED   AuthData is NULL or AuthDataSize is 0.
**/
EFI_STATUS
HashPeImageByType (
  IN  UINT8           *Buffer,
  IN  UINTN           BufferSize,
  IN UINT8            *AuthData,
  IN UINTN            AuthDataSize

  )
{
  UINT8                     Index;

  if (AuthData == NULL || AuthDataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < HASHALG_MAX; Index++) {
    //
    // Check the Hash algorithm in PE/COFF Authenticode.
    //    According to PKCS#7 Definition:
    //        SignedData ::= SEQUENCE {
    //            version Version,
    //            digestAlgorithms DigestAlgorithmIdentifiers,
    //            contentInfo ContentInfo,
    //            .... }
    //    The DigestAlgorithmIdentifiers can be used to determine the hash algorithm in PE/COFF hashing
    //    This field has the fixed offset (+32) in final Authenticode ASN.1 data.
    //
    if (AuthDataSize < 32 + mHash[Index].OidLength) {
      return EFI_UNSUPPORTED;
    }

    if ((*(AuthData + 1) & TWO_BYTE_ENCODE) != TWO_BYTE_ENCODE && (*(AuthData + 1) & SHORT_FORM_MASK) != 0x00) {
      //
      // Only support two bytes of Long Form of Length Encoding and short form Encoding.
      //
      continue;
    }

    if (CompareMem (AuthData + 32, mHash[Index].OidValue, mHash[Index].OidLength) == 0) {
      break;
    }
  }

  if (Index == HASHALG_MAX) {
    return EFI_UNSUPPORTED;
  }

  //
  // HASH PE Image based on Hash algorithm in PE/COFF Authenticode.
  //
  if (!HashPeImage(Buffer, BufferSize, Index)) {
    //
    // If specific alogrithm doesn't support, try to use SHA256
    //
    if (!HashPeImage(Buffer, BufferSize, HASHALG_SHA256)) {
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}


/**
  Check this ROM is a  EFI format image or not

  @param  Buffer        Pointer to the image buffer
  @param  BufferSize    The image size

  @retval EFI_SUCCESS   This image is EFI format image.

**/
EFI_STATUS
EnrollSignedEfiHahsImage (
  IN  UINT8      *Buffer,
  IN  UINTN      BufferSize
  )
{
  EFI_IMAGE_DOS_HEADER                     *DosHdr;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION      NtHeader;
  EFI_IMAGE_DATA_DIRECTORY                 *SecDataDir;
  UINTN                                    PeCoffHeaderOffset;
  UINT16                                   Magic;
  BOOLEAN                                  IsSignedEfi;
  BOOLEAN                                  IsEfi;
  WIN_CERTIFICATE                          *WinCertificate;
  WIN_CERTIFICATE_UEFI_GUID                *WinCertUefiGuid;
  WIN_CERTIFICATE_EFI_PKCS                 *PkcsCertData;
  BOOLEAN                                  SupportedSig;
  UINT8                                    *AuthData;
  UINTN                                    AuthDataSize;

  IsSignedEfi = FALSE;
  IsEfi       = FALSE;
  SecDataDir  = NULL;

  DosHdr = (EFI_IMAGE_DOS_HEADER *) Buffer;
  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    //
    // DOS image header is present,
    // so read the PE header after the DOS image header.
    //
    PeCoffHeaderOffset = DosHdr->e_lfanew;
  } else {
    PeCoffHeaderOffset = 0;
  }
  //
  // Check PE/COFF image.
  //
  NtHeader.Pe32 = (EFI_IMAGE_NT_HEADERS32 *) (Buffer + PeCoffHeaderOffset);
  if (NtHeader.Pe32->Signature == EFI_IMAGE_NT_SIGNATURE) {
    Magic = NtHeader.Pe32->OptionalHeader.Magic;
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset.
      //
      SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &NtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
      IsEfi = TRUE;
    }

    if (IsEfi && SecDataDir->Size != 0) {
      //
      // This image is signed.
      //
      IsSignedEfi = TRUE;
    }
  }
  NtHeader.Pe32Plus = (EFI_IMAGE_NT_HEADERS64 *) (Buffer + PeCoffHeaderOffset);
  if (NtHeader.Pe32Plus->Signature == EFI_IMAGE_NT_SIGNATURE) {
    Magic = NtHeader.Pe32Plus->OptionalHeader.Magic;
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      //
      // Use PE32 offset.
      //
      SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &NtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
      IsEfi = TRUE;
    }

    if (IsEfi && SecDataDir->Size != 0) {
      //
      // This image is signed.
      //
      IsSignedEfi = TRUE;
    }
  }
  if (!IsSignedEfi) {
    return EFI_UNSUPPORTED;
  }

  AuthData       = NULL;
  AuthDataSize   = 0;
  SupportedSig   = FALSE;
  WinCertificate = (WIN_CERTIFICATE *) (Buffer + SecDataDir->VirtualAddress);
  //
  // Check is whether supported signature
  //
  if (WinCertificate->wCertificateType == WIN_CERT_TYPE_PKCS_SIGNED_DATA) {
    PkcsCertData = (WIN_CERTIFICATE_EFI_PKCS *) WinCertificate;
    if (PkcsCertData->Hdr.dwLength >= sizeof(PkcsCertData->Hdr)) {
      AuthData     = PkcsCertData->CertData;
      AuthDataSize = PkcsCertData->Hdr.dwLength - sizeof(PkcsCertData->Hdr);
      SupportedSig = TRUE;
    }
  } else if (WinCertificate->wCertificateType == WIN_CERT_TYPE_EFI_GUID) {
    WinCertUefiGuid = (WIN_CERTIFICATE_UEFI_GUID *) WinCertificate;
    if (CompareGuid(&WinCertUefiGuid->CertType, &gEfiCertPkcs7Guid) &&
        WinCertUefiGuid->Hdr.dwLength >= OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData)) {
      AuthData = WinCertUefiGuid->CertData;
      AuthDataSize = WinCertUefiGuid->Hdr.dwLength - OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData);
      SupportedSig = TRUE;
    }
  }

  if (!SupportedSig) {
    return EFI_UNSUPPORTED;
  }

  return HashPeImageByType (Buffer, BufferSize, AuthData, AuthDataSize);
}

/**
  Dummy function uses to hook original OutputString ().

  @param[in] This       The protocol instance pointer.
  @param[in] String     The NULL-terminated string to be displayed on the output
                        device(s). All output devices must also support the Unicode
                        drawing character codes defined in this file.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
DummyOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL        *This,
  IN CHAR16                                 *String
  )
{
  return EFI_SUCCESS;
}

/**
  Fuction to hook original Outputstring function to disable all of
  output strings.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
STATIC
EFI_STATUS
HookOutputString (
  VOID
  )
{
  mOriginalOutputString = gST->ConOut->OutputString;
  gST->ConOut->OutputString = DummyOutputString;

  return EFI_SUCCESS;
}

/**
  Fuction to restore original Outputstring function to ConOut in
  system table.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
STATIC
EFI_STATUS
RestoreOutputString (
  VOID
  )
{
  gST->ConOut->OutputString = mOriginalOutputString;

  return EFI_SUCCESS;
}

EFI_STATUS
EnrollHashImage (
  EM_FILE_CONTEXT     *FileContext
  )
{
  EFI_FILE_HANDLE         Dir;
  EFI_FILE_HANDLE         NewDir;
  EFI_FILE_INFO           *DirInfo;
  EFI_STATUS              Status;
  UINTN                   BufferSize;
  UINT8                   *Buffer;
  BOOLEAN                 IsSignedEfi;
  EFI_HASH_PROTOCOL       *Hash;
  CHAR16                  *StringPtr;
  EFI_INPUT_KEY           Key;
  EFI_LOAD_FILE_PROTOCOL  *LoadFile;

  if (FileContext->IsLoadFile && !FileContext->IsBootLegacy) {
    if (FileContext->Handle == NULL) {
      return EFI_UNSUPPORTED;
    }
    Status = gBS->HandleProtocol (FileContext->Handle, &gEfiLoadFileProtocolGuid, (VOID**)&LoadFile);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    HookOutputString ();
    //
    // Call LoadFile with the correct buffer size
    //
    BufferSize = 0;
    Buffer     = NULL;
    Status = LoadFile->LoadFile (
                         LoadFile,
                         FileContext->DevicePath,
                         TRUE,
                         &BufferSize,
                         Buffer
                         );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Buffer = AllocatePool (BufferSize);
      if (Buffer == NULL) {
        RestoreOutputString ();
        return EFI_OUT_OF_RESOURCES;
      }
      Status = LoadFile->LoadFile (
                           LoadFile,
                           FileContext->DevicePath,
                           TRUE,
                           &BufferSize,
                           Buffer
                           );
    }
    RestoreOutputString ();
    if (EFI_ERROR (Status)) {
      StringPtr = HiiGetString (
                    mSecureBootPrivate.HiiHandle,
                    STRING_TOKEN (STR_FILE_NOT_FOUND_MESSAGE),
                    NULL
                    );
      mSecureBootPrivate.H2ODialog->ConfirmDialog (
                                           DlgOk,
                                           FALSE,
                                           0,
                                           NULL,
                                           &Key,
                                           StringPtr
                                           );
      gBS->FreePool (StringPtr);
      return EFI_UNSUPPORTED;
    }
  } else {
    Dir = FileContext->FHandle;
    Status = Dir->Open (
                    Dir,
                    &NewDir,
                    FileContext->FileName,
                    EFI_FILE_READ_ONLY,
                    0
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    DirInfo = EfiLibFileInfo (NewDir);
    if (!DirInfo) {
      NewDir->Close (NewDir);
      return EFI_NOT_FOUND;
    }

    BufferSize = (UINTN) DirInfo->FileSize;
    Buffer     = AllocateZeroPool (BufferSize);
    ASSERT (Buffer != NULL);
    if (Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    EmSafeFreePool ((VOID **) &DirInfo);

    Status = NewDir->Read (NewDir, &BufferSize, Buffer);
    NewDir->Close (NewDir);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (Buffer == NULL || !IsEfiImage (Buffer, BufferSize, &IsSignedEfi)) {
    StringPtr = HiiGetString (
                  mSecureBootPrivate.HiiHandle,
                  STRING_TOKEN (STR_FORMATE_INCORRECT_MESSAGE),
                  NULL
                  );
    mSecureBootPrivate.H2ODialog->ConfirmDialog (
                                         DlgOk,
                                         FALSE,
                                         0,
                                         NULL,
                                         &Key,
                                         StringPtr
                                         );
    //
    // pop message for user, and return directly
    //
    gBS->FreePool (StringPtr);
    return EFI_UNSUPPORTED;
  }
  StringPtr = HiiGetString (
                mSecureBootPrivate.HiiHandle,
                STRING_TOKEN (STR_ADD_HASH_MESSAGE),
                NULL
                );
  mSecureBootPrivate.H2ODialog->ConfirmDialog (
                                       DlgYesNo,
                                       FALSE,
                                       0,
                                       NULL,
                                       &Key,
                                       StringPtr
                                       );
  gBS->FreePool (StringPtr);
  if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
    Status = gBS->LocateProtocol (
                    &gEfiHashProtocolGuid,
                    NULL,
                    (VOID **) &Hash
                    );
    ASSERT_EFI_ERROR (Status);


    if (IsSignedEfi) {
      Status = EnrollSignedEfiHahsImage (Buffer, BufferSize);
    } else {
      Status = HashPeImage (Buffer, BufferSize, HASHALG_SHA256) ? EFI_SUCCESS : EFI_UNSUPPORTED;
    }
  }

  return Status;
}


/**
  Append file name to existing file name.

  @param  Str1         existing file name
  @param  Str2         file name to be appended

  @return Allocate a new string to hold the appended result.
          Caller is responsible to free the returned string.

**/
CHAR16 *
EmAppendFileName (
  IN  CHAR16  *Str1,
  IN  CHAR16  *Str2
  )
{
  UINTN   Size1;
  UINTN   Size2;
  CHAR16  *Str;
  CHAR16  *Ptr;
  CHAR16  *LastSlash;

  Size1 = StrSize (Str1);
  Size2 = StrSize (Str2);
  Str   = AllocateZeroPool (Size1 + Size2 + sizeof (CHAR16));
  ASSERT (Str != NULL);
  if (Str == NULL) {
    return NULL;
  }

  StrCat (Str, Str1);
  if (!((*Str == '\\') && (*(Str + 1) == 0))) {
    StrCat (Str, L"\\");
  }

  StrCat (Str, Str2);

  Ptr       = Str;
  LastSlash = Str;
  while (*Ptr != 0) {
    if (*Ptr == '\\' && *(Ptr + 1) == '.' && *(Ptr + 2) == '.' && *(Ptr + 3) != 0) {
      //
      // /* Convert \Name\..\ to \               */
      // DO NOT convert the .. if it is at the end of the string. This will
      // break the .. behavior in changing directories.
      //
      StrCpy (LastSlash, Ptr + 3);
      Ptr = LastSlash;
    } else if (*Ptr == '\\' && *(Ptr + 1) == '.' && *(Ptr + 2) == '\\') {
      //
      // /* Convert a \.\ to a \                 */
      //
      StrCpy (Ptr, Ptr + 2);
      Ptr = LastSlash;
    } else if (*Ptr == '\\') {
      LastSlash = Ptr;
    }

    Ptr++;
  }

  return Str;
}


/**
  Fetch a usable string node from the string depository and return the string token.

  @param  StringDepository         Pointer of the string depository.

  @return String token.

**/
EFI_STRING_ID
EmGetStringTokenFromDepository (
  IN   SECURE_BOOT_MANAGER_CALLBACK_DATA     *CallbackData,
  IN   STRING_DEPOSITORY                     *StringDepository
  )
{
  STRING_LIST_NODE  *CurrentListNode;
  STRING_LIST_NODE  *NextListNode;

  CurrentListNode = StringDepository->CurrentNode;

  if ((NULL != CurrentListNode) && (NULL != CurrentListNode->Next)) {
    //
    // Fetch one reclaimed node from the list.
    //
    NextListNode = StringDepository->CurrentNode->Next;
  } else {
    //
    // If there is no usable node in the list, update the list.
    //
    NextListNode = AllocateZeroPool (sizeof (STRING_LIST_NODE));
    if (NextListNode == NULL) {
      return 0;
    }
    NextListNode->StringToken = HiiSetString (
                                  CallbackData->HiiHandle,
                                  NextListNode->StringToken,
                                  L" ",
                                  NULL
                                  );
    ASSERT (NextListNode->StringToken != 0);

    StringDepository->TotalNodeNumber++;

    if (NULL == CurrentListNode) {
      StringDepository->ListHead = NextListNode;
    } else {
      CurrentListNode->Next = NextListNode;
    }
  }

  StringDepository->CurrentNode = NextListNode;

  return StringDepository->CurrentNode->StringToken;
}


/**
  Check whether current FileName point to a valid
  Efi Image File.

  @param  FileName      File need to be checked.

  @retval TRUE          Is Efi Image
  @retval FALSE         Not a valid Efi Image

**/
BOOLEAN
EmIsEfiImageName (
  IN UINT16  *FileName
  )
{
  //
  // Search for ".efi" extension
  //
  while (*FileName) {
    if (FileName[0] == '.') {
      if (FileName[1] == 'e' || FileName[1] == 'E') {
        if (FileName[2] == 'f' || FileName[2] == 'F') {
          if (FileName[3] == 'i' || FileName[3] == 'I') {
            return TRUE;
          } else if (FileName[3] == 0x0000) {
            return FALSE;
          }
        } else if (FileName[2] == 0x0000) {
          return FALSE;
        }
      } else if (FileName[1] == 0x0000) {
        return FALSE;
      }
    }

    FileName += 1;
  }

  return FALSE;
}


/**
  Create Menu Entry for future use, make all types together
  in order to reduce code size

  @param  MenuType      Use this parameter to identify current
                        Menu type

  @return A valid pointer pointing to the allocated memory pool for current menu entry
          or NULL if it cannot allocate memory for current menu entry

**/
ENROLL_MENU_ENTRY *
EmCreateFileMenuEntry (
  VOID
  )
{
  ENROLL_MENU_ENTRY *MenuEntry;
  UINTN             ContextSize;


  ContextSize = sizeof (BM_FILE_CONTEXT);

  MenuEntry = AllocateZeroPool (sizeof (ENROLL_MENU_ENTRY));
  if (NULL == MenuEntry) {
    return MenuEntry;
  }

  MenuEntry->VariableContext = AllocateZeroPool (ContextSize);
  if (NULL == MenuEntry->VariableContext) {
    EmSafeFreePool ((VOID**) &MenuEntry);
    MenuEntry = NULL;
    return MenuEntry;
  }

  MenuEntry->Signature        = EM_MENU_ENTRY_SIGNATURE;
  MenuEntry->ContextSelection = EM_FILE_CONTEXT_SELECT;
  return MenuEntry;
}


/**
  Find files under current directory
  All files and sub-directories in current directory
  will be stored in DirectoryMenu for future use.

  @param  FileOption    Pointer for Dir to explore

  @retval TRUE          Get files from current dir successfully
  @retval FALSE         Can't get files from current dir

**/
EFI_STATUS
EmFindFiles (
  IN SECURE_BOOT_MANAGER_CALLBACK_DATA      *CallbackData,
  IN ENROLL_MENU_ENTRY                      *MenuEntry
  )
{
  EFI_FILE_HANDLE     NewDir;
  EFI_FILE_HANDLE     Dir;
  EFI_FILE_INFO       *DirInfo;
  UINTN               BufferSize;
  UINTN               DirBufferSize;
  ENROLL_MENU_ENTRY   *NewMenuEntry;
  EM_FILE_CONTEXT     *FileContext;
  EM_FILE_CONTEXT     *NewFileContext;
  UINTN               Pass;
  EFI_STATUS          Status;
  UINTN               OptionNumber;

  FileContext   = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;
  Dir           = FileContext->FHandle;
  OptionNumber  = 0;
  //
  // Open current directory to get files from it
  //
  Status = Dir->Open (
                  Dir,
                  &NewDir,
                  FileContext->FileName,
                  EFI_FILE_READ_ONLY,
                  0
                  );
  if (!FileContext->IsRoot) {
    Dir->Close (Dir);
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  DirInfo = EfiLibFileInfo (NewDir);
  if (!DirInfo) {
    return EFI_NOT_FOUND;
  }

  if (!(DirInfo->Attribute & EFI_FILE_DIRECTORY)) {
    return EFI_INVALID_PARAMETER;
  }

  FileContext->DevicePath = FileDevicePath (
                              FileContext->Handle,
                              FileContext->FileName
                              );

  DirBufferSize = sizeof (EFI_FILE_INFO) + 1024;
  DirInfo       = AllocateZeroPool (DirBufferSize);
  if (!DirInfo) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get all files in current directory
  // Pass 1 to get Directories
  // Pass 2 to get files that are EFI images
  //
  for (Pass = 1; Pass <= 2; Pass++) {
    NewDir->SetPosition (NewDir, 0);
    for (;;) {
      BufferSize  = DirBufferSize;
      Status      = NewDir->Read (NewDir, &BufferSize, DirInfo);
      if (EFI_ERROR (Status) || BufferSize == 0) {
        break;
      }

      if ((DirInfo->Attribute & EFI_FILE_DIRECTORY && Pass == 2) ||
          (!(DirInfo->Attribute & EFI_FILE_DIRECTORY) && Pass == 1)
          ) {
        //
        // Pass 1 is for Directories
        // Pass 2 is for file names
        //
        continue;
      }

      if (!(EmIsEfiImageName (DirInfo->FileName) || DirInfo->Attribute & EFI_FILE_DIRECTORY)) {
        //
        // Slip file unless it is a directory entry or a .EFI file
        //
        continue;
      }

      NewMenuEntry = EmCreateFileMenuEntry ();
      if (NULL == NewMenuEntry) {
        return EFI_OUT_OF_RESOURCES;
      }

      NewFileContext          = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;
      NewFileContext->Handle  = FileContext->Handle;
      NewFileContext->FileName = EmAppendFileName (
                                  FileContext->FileName,
                                  DirInfo->FileName
                                  );
      if (NewFileContext->FileName == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      NewFileContext->FHandle = NewDir;
      NewFileContext->DevicePath = FileDevicePath (
                                    NewFileContext->Handle,
                                    NewFileContext->FileName
                                    );
      NewMenuEntry->HelpString = NULL;

      MenuEntry->DisplayStringToken = EmGetStringTokenFromDepository (
                                        CallbackData,
                                        mFileOptionStrDepository
                                        );

      NewFileContext->IsDir = (BOOLEAN) ((DirInfo->Attribute & EFI_FILE_DIRECTORY) == EFI_FILE_DIRECTORY);

      if (NewFileContext->IsDir) {
        BufferSize                  = StrLen (DirInfo->FileName) * 2 + 6;
        NewMenuEntry->DisplayString = AllocateZeroPool (BufferSize);

        UnicodeSPrint (
          NewMenuEntry->DisplayString,
          BufferSize,
          L"<%s>",
          DirInfo->FileName
          );

      } else {
        NewMenuEntry->DisplayString = AllocateCopyPool (StrSize (DirInfo->FileName), DirInfo->FileName);
      }

      NewFileContext->IsRoot            = FALSE;
      NewFileContext->IsLoadFile        = FALSE;
      NewFileContext->IsRemovableMedia  = FALSE;

      NewMenuEntry->OptionNumber        = OptionNumber;
      OptionNumber++;
      InsertTailList (&mDirectoryMenu.Head, &NewMenuEntry->Link);
    }
  }

  mDirectoryMenu.MenuNumber = OptionNumber;
  EmSafeFreePool ((VOID**) &DirInfo);
  return EFI_SUCCESS;
}


/**
  Use this routine to get one particular menu entry in specified menu

  @param  MenuOption         The menu that we will search
  @param  MenuNumber         The menunubmer that we want

  @return The desired menu entry

**/
ENROLL_MENU_ENTRY *
EmGetMenuEntry (
  ENROLL_MENU_OPTION     *MenuOption,
  UINTN                  MenuNumber
  )
{
  ENROLL_MENU_ENTRY   *NewMenuEntry;
  UINTN               Index;
  LIST_ENTRY          *List;

  if (MenuNumber >= MenuOption->MenuNumber) {
    return NULL;
  }

  List = MenuOption->Head.ForwardLink;
  for (Index = 0; Index < MenuNumber; Index++) {
    List = List->ForwardLink;
  }

  NewMenuEntry = CR (List, ENROLL_MENU_ENTRY, Link, EM_MENU_ENTRY_SIGNATURE);

  return NewMenuEntry;
}


/**
  Update the Enroll Menu page.

  @param  CallbackData         Pointer to SECURE_BOOT_MANAGER_CALLBACK_DATA instance
  @param  MenuOption           Pointer to menu options to display.

**/
VOID
UpdateEnrollMenuPage (
  IN SECURE_BOOT_MANAGER_CALLBACK_DATA     *CallbackData,
  IN ENROLL_MENU_OPTION                    *MenuOption
  )
{
  UINTN                       Index;
  ENROLL_MENU_ENTRY           *NewMenuEntry;
  EM_FILE_CONTEXT             *NewFileContext;
  EFI_FORM_ID                 FormId;
  VOID                        *StartOpCodeHandle;
  VOID                        *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL          *StartLabel;
  EFI_IFR_GUID_LABEL          *EndLabel;

  NewMenuEntry    = NULL;
  NewFileContext  = NULL;
  FormId          = 0;

  //
  // Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);
  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = FORM_ENROLL_FILE_ID;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = FORM_ENROLL_FILE_END_ID;

  for (Index = 0; Index < MenuOption->MenuNumber; Index++) {
    NewMenuEntry    = EmGetMenuEntry (MenuOption, Index);
    NewFileContext  = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;

    if (NewFileContext->IsBootLegacy) {
      continue;
    }
    HiiCreateActionOpCode (
      StartOpCodeHandle,
      (UINT16) (ENROLL_FILE_OPTION_OFFSET + Index),
      NewMenuEntry->DisplayStringToken,
      STRING_TOKEN (STR_SB_NULL_STRING),
      EFI_IFR_FLAG_CALLBACK,
      0
      );
  }
  HiiUpdateForm (
    CallbackData->HiiHandle,
    &mSecureBootGuid,
    FORM_ENROLL_HASH_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
    );
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

}


/**
  Create string tokens for a menu from its help strings and display strings

  @param  HiiHandle         Hii Handle of the package to be updated.
  @param  MenuOption        The Menu whose string tokens need to be created

  @retval EFI_SUCCESS   string tokens created successfully
  @retval Other         contain some errors

**/
EFI_STATUS
EmCreateMenuStringToken (
  IN EFI_HII_HANDLE                   HiiHandle,
  IN ENROLL_MENU_OPTION               *MenuOption
  )
{
  ENROLL_MENU_ENTRY *NewMenuEntry;
  UINTN             Index;

  for (Index = 0; Index < MenuOption->MenuNumber; Index++) {
    NewMenuEntry = EmGetMenuEntry (MenuOption, Index);
    NewMenuEntry->DisplayStringToken = HiiSetString (
                                         HiiHandle,
                                         NewMenuEntry->DisplayStringToken,
                                         NewMenuEntry->DisplayString,
                                         NULL
                                         );
    if (NULL == NewMenuEntry->HelpString) {
      NewMenuEntry->HelpStringToken = NewMenuEntry->DisplayStringToken;
    } else {
      NewMenuEntry->HelpStringToken = HiiSetString (
                                        HiiHandle,
                                        NewMenuEntry->HelpStringToken,
                                        NewMenuEntry->HelpString,
                                        NULL
                                        );
    }
  }

  return EFI_SUCCESS;
}


/**
  Destroy the menu entry passed in

  @param  MenuEntry     The menu entry need to be destroyed

**/
VOID
EmDestroyFileMenuEntry (
  ENROLL_MENU_ENTRY         *MenuEntry
  )
{
  EM_FILE_CONTEXT           *FileContext;

  FileContext = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;

  if (!FileContext->IsRoot) {
    EmSafeFreePool ((VOID**) &FileContext->DevicePath);
  } else {
    if (FileContext->FHandle != NULL) {
      FileContext->FHandle->Close (FileContext->FHandle);
    }
  }

  EmSafeFreePool ((VOID**) &FileContext->FileName);
  EmSafeFreePool ((VOID**) &FileContext->Info);
  EmSafeFreePool ((VOID**) &FileContext);

  EmSafeFreePool ((VOID**) &MenuEntry->DisplayString);
  EmSafeFreePool ((VOID**) &MenuEntry->HelpString);

  EmSafeFreePool ((VOID**) &MenuEntry);
}


/**
  Free resources allocated in Allocate Rountine

  @param  FreeMenu      Menu to be freed

**/
VOID
EmFreeMenu (
  ENROLL_MENU_OPTION       *FreeMenu
  )
{
  ENROLL_MENU_ENTRY    *MenuEntry;

  while (!IsListEmpty (&FreeMenu->Head)) {
    MenuEntry = CR (
                  FreeMenu->Head.ForwardLink,
                  ENROLL_MENU_ENTRY,
                  Link,
                  EM_MENU_ENTRY_SIGNATURE
                  );
    RemoveEntryList (&MenuEntry->Link);
    EmDestroyFileMenuEntry (MenuEntry);
  }
}


/**
  Find file systems for current Extensible Firmware
  Including Handles that support Simple File System
  protocol, Load File protocol.

  Building up the FileSystem Menu for user selection
  All file system will be stored in FsOptionMenu
  for future use.

  @param  CallbackData  BMM context data

  @retval EFI_SUCCESS               Success find the file system
  @retval EFI_OUT_OF_RESOURCES      Can not create menu entry

**/
EFI_STATUS
EmFindFileSystem (
  VOID
  )
{
  UINTN                     NoBlkIoHandles;
  UINTN                     NoSimpleFsHandles;
  UINTN                     NoLoadFileHandles;
  EFI_HANDLE                *BlkIoHandle;
  EFI_HANDLE                *SimpleFsHandle;
  EFI_HANDLE                *LoadFileHandle;
  UINT16                    *VolumeLabel;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  UINTN                     Index;
  EFI_STATUS                Status;
  ENROLL_MENU_ENTRY          *MenuEntry;
  EM_FILE_CONTEXT           *FileContext;
  UINT16                    *TempStr;
  UINTN                     OptionNumber;
  VOID                      *Buffer;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  UINT16                    DeviceType;
  BBS_BBS_DEVICE_PATH       BbsDevicePathNode;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   RemovableMedia;



  NoSimpleFsHandles = 0;
  NoLoadFileHandles = 0;
  OptionNumber      = 0;
  InitializeListHead (&mFsOptionMenu.Head);

  //
  // Locate Handles that support BlockIo protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &NoBlkIoHandles,
                  &BlkIoHandle
                  );
  if (!EFI_ERROR (Status)) {

    for (Index = 0; Index < NoBlkIoHandles; Index++) {
      Status = gBS->HandleProtocol (
                      BlkIoHandle[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );

      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // Issue a dummy read to trigger reinstall of BlockIo protocol for removable media
      //
      if (BlkIo->Media->RemovableMedia) {
        Buffer = AllocateZeroPool (BlkIo->Media->BlockSize);
        if (NULL == Buffer) {
          EmSafeFreePool ((VOID**) &BlkIoHandle);
          return EFI_OUT_OF_RESOURCES;
        }

        BlkIo->ReadBlocks (
                 BlkIo,
                 BlkIo->Media->MediaId,
                 0,
                 BlkIo->Media->BlockSize,
                 Buffer
                 );
        EmSafeFreePool ((VOID**) &Buffer);
      }
    }
    EmSafeFreePool ((VOID**) &BlkIoHandle);
  }

  //
  // Locate Handles that support Simple File System protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NoSimpleFsHandles,
                  &SimpleFsHandle
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Find all the instances of the File System prototocol
    //
    for (Index = 0; Index < NoSimpleFsHandles; Index++) {
      Status = gBS->HandleProtocol (
                      SimpleFsHandle[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );
      if (EFI_ERROR (Status)) {
        //
        // If no block IO exists assume it's NOT a removable media
        //
        RemovableMedia = FALSE;
      } else {
        //
        // If block IO exists check to see if it's remobable media
        //
        RemovableMedia = BlkIo->Media->RemovableMedia;
      }

      //
      // Allocate pool for this load option
      //
      MenuEntry = EmCreateFileMenuEntry ();
      if (NULL == MenuEntry) {
        EmSafeFreePool ((VOID**) &SimpleFsHandle);
        return EFI_OUT_OF_RESOURCES;
      }

      FileContext = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;

      FileContext->Handle     = SimpleFsHandle[Index];
      MenuEntry->OptionNumber = Index;
      FileContext->FHandle    = EfiLibOpenRoot (FileContext->Handle);
      if (!FileContext->FHandle) {
        EmDestroyFileMenuEntry (MenuEntry);
        continue;
      }

      MenuEntry->HelpString = DevicePathToStr (DevicePathFromHandle (FileContext->Handle));
      FileContext->Info = EfiLibFileSystemVolumeLabelInfo (FileContext->FHandle);
      FileContext->FileName = EfiStrDuplicate (L"\\");
      if (FileContext->FileName == NULL) {
        EmDestroyFileMenuEntry (MenuEntry);
        EmSafeFreePool ((VOID**) &SimpleFsHandle);
        return EFI_OUT_OF_RESOURCES;
      }
      FileContext->DevicePath = FileDevicePath (
                                  FileContext->Handle,
                                  FileContext->FileName
                                  );
      FileContext->IsDir            = TRUE;
      FileContext->IsRoot           = TRUE;
      FileContext->IsRemovableMedia = FALSE;
      FileContext->IsLoadFile       = FALSE;

      //
      // Get current file system's Volume Label
      //
      if (FileContext->Info == NULL) {
        VolumeLabel = L"NO FILE SYSTEM INFO";
      } else {
        if (FileContext->Info->VolumeLabel == NULL) {
          VolumeLabel = L"NULL VOLUME LABEL";
        } else {
          VolumeLabel = FileContext->Info->VolumeLabel;
          if (*VolumeLabel == 0x0000) {
            VolumeLabel = L"NO VOLUME LABEL";
          }
        }
      }

      TempStr                   = MenuEntry->HelpString;
      MenuEntry->DisplayString  = AllocateZeroPool (MAX_CHAR);
      ASSERT (MenuEntry->DisplayString != NULL);
      UnicodeSPrint (
        MenuEntry->DisplayString,
        MAX_CHAR,
        L"%s, [%s]",
        VolumeLabel,
        TempStr
        );
      OptionNumber++;
      InsertTailList (&mFsOptionMenu.Head, &MenuEntry->Link);
    }
  }

  if (NoSimpleFsHandles != 0) {
    EmSafeFreePool ((VOID**) &SimpleFsHandle);
  }
  //
  // Searching for handles that support Load File protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  &NoLoadFileHandles,
                  &LoadFileHandle
                  );

  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NoLoadFileHandles; Index++) {
      MenuEntry = EmCreateFileMenuEntry ();
      if (NULL == MenuEntry) {
        EmSafeFreePool ((VOID**) &LoadFileHandle);
        return EFI_OUT_OF_RESOURCES;
      }

      FileContext                   = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;
      FileContext->IsRemovableMedia = FALSE;
      FileContext->IsLoadFile       = TRUE;
      FileContext->Handle           = LoadFileHandle[Index];
      FileContext->IsRoot           = TRUE;

      FileContext->DevicePath = DevicePathFromHandle (FileContext->Handle);

      MenuEntry->HelpString     = DevicePathToStr (FileContext->DevicePath);

      TempStr                   = MenuEntry->HelpString;
      MenuEntry->DisplayString  = AllocateZeroPool (MAX_CHAR);
      ASSERT (MenuEntry->DisplayString != NULL);
      UnicodeSPrint (
        MenuEntry->DisplayString,
        MAX_CHAR,
        L"Load File [%s]",
        TempStr
        );

      MenuEntry->OptionNumber = OptionNumber;
      OptionNumber++;
      InsertTailList (&mFsOptionMenu.Head, &MenuEntry->Link);
    }
  }

  if (NoLoadFileHandles != 0) {
    EmSafeFreePool ((VOID**) &LoadFileHandle);
  }

  //
  // Add Legacy Boot Option Support Here
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **) &LegacyBios
                  );
  if (!EFI_ERROR (Status)) {

    for (Index = BBS_TYPE_FLOPPY; Index <= BBS_TYPE_EMBEDDED_NETWORK; Index++) {
      MenuEntry = EmCreateFileMenuEntry ();
      if (NULL == MenuEntry) {
        return EFI_OUT_OF_RESOURCES;
      }

      FileContext                       = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;

      FileContext->IsRemovableMedia     = FALSE;
      FileContext->IsLoadFile           = TRUE;
      FileContext->IsBootLegacy         = TRUE;
      DeviceType                        = (UINT16) Index;
      BbsDevicePathNode.Header.Type     = BBS_DEVICE_PATH;
      BbsDevicePathNode.Header.SubType  = BBS_BBS_DP;
      SetDevicePathNodeLength (
        &BbsDevicePathNode.Header,
        sizeof (BBS_BBS_DEVICE_PATH)
        );
      BbsDevicePathNode.DeviceType  = DeviceType;
      BbsDevicePathNode.StatusFlag  = 0;
      BbsDevicePathNode.String[0]   = 0;
      DevicePath = AppendDevicePathNode (
                    EndDevicePath,
                    (EFI_DEVICE_PATH_PROTOCOL *) &BbsDevicePathNode
                    );

      FileContext->DevicePath   = DevicePath;
      MenuEntry->HelpString     = DevicePathToStr (FileContext->DevicePath);

      TempStr                   = MenuEntry->HelpString;
      MenuEntry->DisplayString  = AllocateZeroPool (MAX_CHAR);
      ASSERT (MenuEntry->DisplayString != NULL);
      UnicodeSPrint (
        MenuEntry->DisplayString,
        MAX_CHAR,
        L"Boot Legacy [%s]",
        TempStr
        );
      MenuEntry->OptionNumber = OptionNumber;
      OptionNumber++;
      InsertTailList (&mFsOptionMenu.Head, &MenuEntry->Link);
    }
  }
  //
  // Remember how many file system options are here
  //
  mFsOptionMenu.MenuNumber = OptionNumber;
  return EFI_SUCCESS;
}


/**
  Update the file explower page with the refershed file system.

  @param  CallbackData      BSECURE_BOOT_MANAGER_CALLBACK_DATA instance
  @param  KeyValue          Key value to identify the type of data to expect.

  @retval TRUE              Inform the caller to create a callback packet to exit file explorer.
  @retval FALSE             Indicate that there is no need to exit file explorer.

**/
BOOLEAN
UpdateEmFileExplorer (
  IN SECURE_BOOT_MANAGER_CALLBACK_DATA      *CallbackData,
  IN UINT16                                 KeyValue
  )
{
  UINT16              FileOptionMask;
  ENROLL_MENU_ENTRY   *NewMenuEntry;
  EM_FILE_CONTEXT     *NewFileContext;
  EFI_STATUS          Status;


  FileOptionMask    = (UINT16) (ENROLL_FILE_OPTION_MASK & KeyValue);
  NewMenuEntry      = NULL;

  if (EM_UNKNOWN_CONTEXT == CallbackData->EmDisplayContext) {
    EmFindFileSystem ();
    EmCreateMenuStringToken (CallbackData->HiiHandle, &mFsOptionMenu);
    UpdateEnrollMenuPage (CallbackData, &mFsOptionMenu);
    CallbackData->EmDisplayContext = EM_FILE_SYSTEM;
  } else {
    if (EM_FILE_SYSTEM == CallbackData->EmDisplayContext) {
      NewMenuEntry = EmGetMenuEntry (&mFsOptionMenu, FileOptionMask);
    } else if (EM_DIRECTORY == CallbackData->EmDisplayContext) {
      NewMenuEntry = EmGetMenuEntry (&mDirectoryMenu, FileOptionMask);
    }
    if (NewMenuEntry == NULL) {
      return FALSE;
    }
    NewFileContext = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;

    if (NewFileContext->IsDir ) {
      CallbackData->EmDisplayContext = EM_DIRECTORY;
      RemoveEntryList (&NewMenuEntry->Link);
      EmFreeMenu (&mDirectoryMenu);
      Status = EmFindFiles (CallbackData, NewMenuEntry);
       if (EFI_ERROR (Status)) {
         return TRUE;
       }

      EmCreateMenuStringToken (CallbackData->HiiHandle, &mDirectoryMenu);
      EmDestroyMenuEntry (NewMenuEntry);
      UpdateEnrollMenuPage (CallbackData, &mDirectoryMenu);
    } else {
      //
      // Ask user whether want to add this EFI image to allowed database (db)
      //
      EnrollHashImage (NewFileContext);
    }
  }

  return FALSE;


}


/**
  Initialize file system for Enroll menu

  @param  SecureBootData      Pointer to SECURE_BOOT_MANAGER_CALLBACK_DATA instance

  @retval EFI_SUCCESS   Initialize HII information successful
  @retval Other

**/
BOOLEAN
InitializeEnrollMenu (
  IN  SECURE_BOOT_MANAGER_CALLBACK_DATA     *CallbackData
  )
{
  CallbackData->EmDisplayContext = EM_UNKNOWN_CONTEXT;
  return UpdateEmFileExplorer (CallbackData, 0);
}
