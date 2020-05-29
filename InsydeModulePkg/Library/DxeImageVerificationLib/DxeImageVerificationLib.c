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
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/**

  Implement image verification services for secure boot
  service in UEFI2.2.

Copyright (c) 2009 - 2010, Intel Corporation. All rights reserved. <BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include "DxeImageVerificationLib.h"
#include "UnsignedFvRegion.h"

#include <Protocol/CryptoServices.h>
#include <Protocol/Hash.h>
#include <SecureFlash.h>
#include <Protocol/Bds.h>


#define EFI_FIRMWARE_VOLUME_PROTOCOL_GUID \
  { \
    0x389F751F, 0x1838, 0x4388, {0x83, 0x90, 0xCD, 0x81, 0x54, 0xBD, 0x27, 0xF8 } \
  }
EFI_GUID gEfiFirmwareVolumeProtocolGuid = EFI_FIRMWARE_VOLUME_PROTOCOL_GUID;
UINT8    mTrustedPubKey[256];

EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION mNtHeader;
UINTN                               mImageSize;
UINT32                              mPeCoffHeaderOffset;
EFI_HASH_OUTPUT                     *mImageDigest     = NULL;
UINTN                               mImageDigestSize;
UINT8                               *mImageBase       = NULL;
UINT8                               mTrustedCertFingerPrint[MAX_DIGEST_SIZE];
EFI_GUID                            mCertType;
VOID                                *mCertBuffer = NULL;
EFI_BDS_ENTRY                       mOriginalBdsEntry = NULL;
BOOLEAN                             mEnterBdsPhase = FALSE;

//
// Notify string for authorization UI.
//
CHAR16  mNotifyString1[MAX_NOTIFY_STRING_LEN] = L"Image verification pass but not found in authorized database!";
CHAR16  mNotifyString2[MAX_NOTIFY_STRING_LEN] = L"Launch this image anyway? (Yes/Defer/No)";
//
// Public Exponent of RSA Key.
//
CONST UINT8 mRsaE[] = { 0x01, 0x00, 0x01 };
//
// OID ASN.1 Value for Hash Algorithms
//
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


/**
  BdsEntry hook function. This function uses to set mEnterBdsPhase to TRUE
  and then enter original BdsEntry ().

  @param[in]  This             The EFI_BDS_ARCH_PROTOCOL instance.
**/
VOID
EFIAPI
BdsEntry (
  IN EFI_BDS_ARCH_PROTOCOL  *This
  )
{
  mEnterBdsPhase = TRUE;
  mOriginalBdsEntry (This);
}

/**
  This function uses to hook original BdsEntry ().

  @param Event    Event whose notification function is being invoked.
  @param Context  Pointer to the notification function's context.

**/
VOID
EFIAPI
BdsCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                        Status;
  EFI_BDS_ARCH_PROTOCOL             *Bds;

  Status = gBS->LocateProtocol (
                  &gEfiBdsArchProtocolGuid,
                  NULL,
                  (VOID **)&Bds
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  mOriginalBdsEntry = Bds->Entry;
  Bds->Entry = BdsEntry;
  return;
}

/**
  This function uses to initialize code to check system is weather enter BDS phase.

  @param[in]  SystemTable   Pointer to the EFI System Table.
**/
VOID
InitializeBdsPhaseCheck (
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_BOOT_SERVICES                 *BootServices;
  EFI_STATUS                        Status;
  EFI_BDS_ARCH_PROTOCOL             *Bds;
  EFI_EVENT                         Event;
  VOID                              *Registration;

  BootServices = SystemTable->BootServices;
  Status = BootServices->LocateProtocol (
                           &gEfiBdsArchProtocolGuid,
                           NULL,
                           (VOID **)&Bds
                           );
  if (!EFI_ERROR (Status)) {
    mOriginalBdsEntry = Bds->Entry;
    Bds->Entry = BdsEntry;
  } else {
    Status = BootServices->CreateEvent (
                             EVT_NOTIFY_SIGNAL,
                             TPL_NOTIFY,
                             BdsCallback,
                             NULL,
                             &Event
                             );
    ASSERT_EFI_ERROR (Status);

    //
    // Register for protocol notifications on this event
    //
    Status = BootServices->RegisterProtocolNotify (
                             &gEfiBdsArchProtocolGuid,
                             Event,
                             &Registration
                             );
    ASSERT_EFI_ERROR (Status);
  }
  return;
}

/**
  Get the image type.

  @param[in]    File       This is a pointer to the device path of the file that is
                           being dispatched.

  @return UINT32           Image Type

**/
UINT32
GetImageType (
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL          *TempDevicePath;
  EFI_BLOCK_IO_PROTOCOL             *BlockIo;

  if (File == NULL) {
    return IMAGE_UNKNOWN;
  }
  //
  // First check to see if File is from a Firmware Volume
  //
  DeviceHandle      = NULL;
  TempDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)File;
    Status = gBS->LocateDevicePath (
                  &gEfiFirmwareVolumeProtocolGuid,
                  &TempDevicePath,
                  &DeviceHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->OpenProtocol (
                    DeviceHandle,
                    &gEfiFirmwareVolumeProtocolGuid,
                    NULL,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                    );
    if (!EFI_ERROR (Status)) {
      return IMAGE_FROM_FV;
    }
  }
  Status = gBS->LocateDevicePath (
                  &gEfiFirmwareVolume2ProtocolGuid,
                  &TempDevicePath,
                  &DeviceHandle
                  );
  if (!EFI_ERROR (Status)) {
    Status = gBS->OpenProtocol (
                    DeviceHandle,
                    &gEfiFirmwareVolume2ProtocolGuid,
                    NULL,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                    );
    if (!EFI_ERROR (Status)) {
      return IMAGE_FROM_FV;
    }
  }

  //
  // Next check to see if File is from a Block I/O device
  //
  DeviceHandle   = NULL;
  TempDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)File;
  Status = gBS->LocateDevicePath (
                  &gEfiBlockIoProtocolGuid,
                  &TempDevicePath,
                  &DeviceHandle
                  );
  if (!EFI_ERROR (Status)) {
    BlockIo = NULL;
    Status = gBS->OpenProtocol (
                    DeviceHandle,
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlockIo,
                    NULL,
                    NULL,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (!EFI_ERROR (Status) && BlockIo != NULL) {
      if (BlockIo->Media != NULL) {
        if (BlockIo->Media->RemovableMedia) {
          //
          // Block I/O is present and specifies the media is removable
          //
          return IMAGE_FROM_REMOVABLE_MEDIA;
        } else {
          //
          // Block I/O is present and specifies the media is not removable
          //
          return IMAGE_FROM_FIXED_MEDIA;
        }
      }
    }
  }

  //
  // File is not in a Firmware Volume or on a Block I/O device, so check to see if
  // the device path supports the Simple File System Protocol.
  //
  DeviceHandle   = NULL;
  TempDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)File;
  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &TempDevicePath,
                  &DeviceHandle
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Simple File System is present without Block I/O, so assume media is fixed.
    //
    return IMAGE_FROM_FIXED_MEDIA;
  }

  //
  // File is not from an FV, Block I/O or Simple File System, so the only options
  // left are a PCI Option ROM and a Load File Protocol such as a PXE Boot from a NIC.
  //
  TempDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)File;
  while (!IsDevicePathEndType (TempDevicePath)) {
    switch (DevicePathType (TempDevicePath)) {

    case MEDIA_DEVICE_PATH:
      if (DevicePathSubType (TempDevicePath) == MEDIA_RELATIVE_OFFSET_RANGE_DP) {
        return IMAGE_FROM_OPTION_ROM;
      }
      break;

    case MESSAGING_DEVICE_PATH:
      if (DevicePathSubType(TempDevicePath) == MSG_MAC_ADDR_DP) {
        return IMAGE_FROM_REMOVABLE_MEDIA;
      }
      break;

    default:
      break;
    }
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }
  return IMAGE_UNKNOWN;
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
  IN  UINT32              HashAlg
  )
{
  EFI_STATUS                Status;
  UINT16                    Magic;
  EFI_IMAGE_SECTION_HEADER  *Section;
  EFI_IMAGE_SECTION_HEADER  *NextSection;
  UINT8                     *HashBase;
  UINTN                     HashSize;
  UINTN                     SumOfBytesHashed;
  EFI_IMAGE_SECTION_HEADER  *SectionHeader;
  UINTN                     Index;
  UINTN                     Pos;
  EFI_HASH_PROTOCOL         *Hash;
  UINT32                    CertSize;
  UINT32                    NumberOfRvaAndSizes;

  SectionHeader = NULL;


  if ((HashAlg != HASHALG_SHA1) && (HashAlg != HASHALG_SHA256)) {
    return FALSE;
  }

  Status = gBS->LocateProtocol (
                  &gEfiHashProtocolGuid,
                  NULL,
                  (VOID **)&Hash
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Initialize context of hash.
  //
  ZeroMem (mImageDigest->Sha256Hash, MAX_DIGEST_SIZE);
  if (HashAlg == HASHALG_SHA1) {
    mCertType         = gEfiCertSha1Guid;
  } else if (HashAlg == HASHALG_SHA256) {
    mCertType         = gEfiCertSha256Guid;
  } else {
    return FALSE;
  }
  Status  = Hash->GetHashSize (
                    Hash,
                    mHash[HashAlg].Index,
                    &mImageDigestSize
                    );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Measuring PE/COFF Image Header;
  // But CheckSum field and SECURITY data directory (certificate) are excluded
  //
  if (mNtHeader.Pe32->FileHeader.Machine == IMAGE_FILE_MACHINE_IA64 && mNtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
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
    Magic =  mNtHeader.Pe32->OptionalHeader.Magic;
  }

  //
  // 3.  Calculate the distance from the base of the image header to the image checksum address.
  // 4.  Hash the image header from its base to beginning of the image checksum.
  //
  HashBase = mImageBase;
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    HashSize = (UINTN) ((UINT8 *) (&mNtHeader.Pe32->OptionalHeader.CheckSum) - HashBase);
    NumberOfRvaAndSizes = mNtHeader.Pe32->OptionalHeader.NumberOfRvaAndSizes;
  } else {
    //
    // Use PE32+ offset.
    //
    HashSize = (UINTN) ((UINT8 *) (&mNtHeader.Pe32Plus->OptionalHeader.CheckSum) - HashBase);
    NumberOfRvaAndSizes = mNtHeader.Pe32Plus->OptionalHeader.NumberOfRvaAndSizes;
  }

  Status = Hash->Hash (
                   Hash,
                   mHash[HashAlg].Index,
                   FALSE,
                   HashBase,
                   HashSize,
                   (EFI_HASH_OUTPUT *) mImageDigest
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
      HashBase = (UINT8 *) &mNtHeader.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
      HashSize = mNtHeader.Pe32->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - mImageBase);
    } else {
      //
      // Use PE32+ offset.
      //
      HashBase = (UINT8 *) &mNtHeader.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
      HashSize = mNtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - mImageBase);
    }

    if (HashSize != 0) {
      Status = Hash->Hash (
                       Hash,
                       mHash[HashAlg].Index,
                       TRUE,
                       HashBase,
                       HashSize,
                       (EFI_HASH_OUTPUT *) mImageDigest
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
      HashBase = (UINT8 *) &mNtHeader.Pe32->OptionalHeader.CheckSum + sizeof (UINT32);
      HashSize = (UINTN) ((UINT8 *) (&mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
    } else {
      //
      // Use PE32+ offset.
      //
      HashBase = (UINT8 *) &mNtHeader.Pe32Plus->OptionalHeader.CheckSum + sizeof (UINT32);
      HashSize = (UINTN) ((UINT8 *) (&mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY]) - HashBase);
    }
    if (HashSize != 0) {
      Status = Hash->Hash (
                       Hash,
                       mHash[HashAlg].Index,
                       TRUE,
                       HashBase,
                       HashSize,
                       (EFI_HASH_OUTPUT *) mImageDigest
                       );


      if (EFI_ERROR (Status)) {
        goto Done;
      }
    }

    //
    // 7.  Hash everything from the end of the checksum to the start of the Cert Directory.
    //
    if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      //
      // Use PE32 offset
      //
      HashBase = (UINT8 *) &mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
      HashSize = mNtHeader.Pe32->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - mImageBase);
    } else {
      //
      // Use PE32+ offset.
      //
      HashBase = (UINT8 *) &mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY + 1];
      HashSize = mNtHeader.Pe32->OptionalHeader.SizeOfHeaders - (UINTN) (HashBase - mImageBase);
    }
    if (HashSize != 0) {
      Status = Hash->Hash (
                       Hash,
                       mHash[HashAlg].Index,
                       TRUE,
                       HashBase,
                       HashSize,
                       (EFI_HASH_OUTPUT *) mImageDigest
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
    SumOfBytesHashed = mNtHeader.Pe32->OptionalHeader.SizeOfHeaders;
  } else {
    //
    // Use PE32+ offset
    //
    SumOfBytesHashed = mNtHeader.Pe32Plus->OptionalHeader.SizeOfHeaders;
  }

  //
  // 11. Build a temporary table of pointers to all the IMAGE_SECTION_HEADER
  //     structures in the image. The 'NumberOfSections' field of the image
  //     header indicates how big the table should be. Do not include any
  //     IMAGE_SECTION_HEADERs in the table whose 'SizeOfRawData' field is zero.
  //
  SectionHeader = (EFI_IMAGE_SECTION_HEADER *) AllocateZeroPool (sizeof (EFI_IMAGE_SECTION_HEADER) * mNtHeader.Pe32->FileHeader.NumberOfSections);
  ASSERT (SectionHeader != NULL);
  if (SectionHeader == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  //
  // 12.  Using the 'PointerToRawData' in the referenced section headers as
  //      a key, arrange the elements in the table in ascending order. In other
  //      words, sort the section headers according to the disk-file offset of
  //      the section.
  //
  Section = (EFI_IMAGE_SECTION_HEADER *) (
               mImageBase +
               mPeCoffHeaderOffset +
               sizeof (UINT32) +
               sizeof (EFI_IMAGE_FILE_HEADER) +
               mNtHeader.Pe32->FileHeader.SizeOfOptionalHeader
               );
  for (Index = 0; Index < mNtHeader.Pe32->FileHeader.NumberOfSections; Index++) {
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
  for (Index = 0; Index < mNtHeader.Pe32->FileHeader.NumberOfSections; Index++) {
    Section = &SectionHeader[Index];
    if (Section->SizeOfRawData == 0) {
      continue;
    }
    HashBase  = mImageBase + Section->PointerToRawData;
    if (Index != (UINTN) (mNtHeader.Pe32->FileHeader.NumberOfSections - 1)) {
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
                     (EFI_HASH_OUTPUT *) mImageDigest
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
  if (mImageSize > SumOfBytesHashed) {
    HashBase = mImageBase + SumOfBytesHashed;
    if (NumberOfRvaAndSizes <= EFI_IMAGE_DIRECTORY_ENTRY_SECURITY) {
      CertSize = 0;
    } else {
      if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
       //
        // Use PE32 offset.
        //
        CertSize = mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
      } else {
        //
        // Use PE32+ offset.
        //
        CertSize = mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY].Size;
      }
      if (mImageSize > CertSize + SumOfBytesHashed) {
        HashSize = (UINTN) (mImageSize - CertSize - SumOfBytesHashed);
        Status = Hash->Hash (
                         Hash,
                         mHash[HashAlg].Index,
                         TRUE,
                         HashBase,
                         HashSize,
                         (EFI_HASH_OUTPUT *) mImageDigest
                         );
        if (EFI_ERROR (Status)) {
          goto Done;
        }
      } else if (mImageSize < CertSize + SumOfBytesHashed) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
    }
  }

Done:
  if (SectionHeader != NULL) {
    FreePool (SectionHeader);
  }
  return EFI_ERROR (Status) ? FALSE : TRUE;
}

/**
  Recognize the Hash algorithm in PE/COFF Authenticode and caculate hash of
  Pe/Coff image based on the authenticode image hashing in PE/COFF Specification
  8.0 Appendix A

  @param[in]  AuthData           Pointer to the Authenticode Signature retrieved from signed image.
  @param[in]  AuthDataSize       Size of the Authenticode Signature in bytes.

  @retval EFI_SUCCESS            Hash successfully.
  @retval EFI_UNSUPPORTED        Hash algorithm is not supported.
  @retval EFI_INVALID_PARAMETER  AuthData is NULL or AuthDataSize is 0.

**/
EFI_STATUS
HashPeImageByType (
  IN UINT8              *AuthData,
  IN UINTN              AuthDataSize
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
  if (!HashPeImage(Index)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}



/**
  Returns the size of a given image execution info table in bytes.

  This function returns the size, in bytes, of the image execution info table specified by
  ImageExeInfoTable. If ImageExeInfoTable is NULL, then 0 is returned.

  @param  ImageExeInfoTable          A pointer to a image execution info table structure.

  @retval 0       If ImageExeInfoTable is NULL.
  @retval Others  The size of a image execution info table in bytes.

**/
STATIC
UINTN
GetImageExeInfoTableSize (
  EFI_IMAGE_EXECUTION_INFO_TABLE        *ImageExeInfoTable
  )
{
  UINTN                     Index;
  EFI_IMAGE_EXECUTION_INFO  *ImageExeInfoItem;
  UINTN                     TotalSize;

  if (ImageExeInfoTable == NULL) {
    return 0;
  }

  ImageExeInfoItem  = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoTable + sizeof (EFI_IMAGE_EXECUTION_INFO_TABLE));
  TotalSize         = sizeof (EFI_IMAGE_EXECUTION_INFO_TABLE);
  for (Index = 0; Index < ImageExeInfoTable->NumberOfImages; Index++) {
    TotalSize += ReadUnaligned32 ((UINT32 *) &ImageExeInfoItem->InfoSize);
    ImageExeInfoItem = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoItem + ReadUnaligned32 ((UINT32 *) &ImageExeInfoItem->InfoSize));
  }

  return TotalSize;
}

/**
  According to input information to create execution information entry. Caller has responsibility to free
  allocated memory for execution information entry if doesn't need use this information any more.

  @param[in]  Action             Describes the action taken by the firmware regarding this image.
  @param[in]  Name               Input a null-terminated, user-friendly name.
  @param[in]  DevicePath         Input device path pointer.
  @param[in]  Signature          Input signature info in EFI_SIGNATURE_LIST data structure.
  @param[in]  SignatureSize      Size of signature.
  @param[out] ImageInfo          pointer to start address of EFI_IMAGE_EXECUTION_INFO_TABLE instance.
  @param[out] ImageInfoSize      pointer to start address of EFI_IMAGE_EXECUTION_INFO_TABLE instance.

  @retval EFI_SUCCESS            Create new image information entry successful.
  @retval EFI_INVALID_PARAMETER  Any input parameter is incorrect.
  @retval EFI_OUT_RESOURCES      Allocate memory failed.
**/
STATIC
EFI_STATUS
CreateNewImageInfo (
  IN       EFI_IMAGE_EXECUTION_ACTION       Action,
  IN       CHAR16                           *Name OPTIONAL,
  IN CONST EFI_DEVICE_PATH_PROTOCOL         *DevicePath,
  IN       EFI_SIGNATURE_LIST               *Signature OPTIONAL,
  IN       UINTN                            SignatureSize,
  OUT      EFI_IMAGE_EXECUTION_INFO         **ImageInfo,
  OUT      UINTN                            *ImageInfoSize
  )
{
  UINTN                          NewImageSize;
  EFI_IMAGE_EXECUTION_INFO       *NewImageExeInfo;
  UINTN                          NameStringLen;
  UINTN                          DevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL       EndOfDevicePath;

  if (ImageInfo == NULL || ImageInfoSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NameStringLen   = Name == NULL ? sizeof (CHAR16) : StrSize (Name);
  DevicePathSize  = DevicePath == NULL ? sizeof (EFI_DEVICE_PATH_PROTOCOL) : GetDevicePathSize (DevicePath);
  NewImageSize    = sizeof (EFI_IMAGE_EXECUTION_INFO) - sizeof (EFI_SIGNATURE_LIST) + NameStringLen + DevicePathSize + SignatureSize;
  NewImageExeInfo = AllocateZeroPool (NewImageSize);
  if (NewImageExeInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  WriteUnaligned32 ((UINT32 *) &NewImageExeInfo->Action, Action);
  WriteUnaligned32 ((UINT32 *) &NewImageExeInfo->InfoSize, (UINT32) NewImageSize);

  if (Name != NULL) {
    CopyMem ((UINT8 *) &NewImageExeInfo->InfoSize + sizeof (UINT32), Name, NameStringLen);
  } else {
    ZeroMem ((UINT8 *) &NewImageExeInfo->InfoSize + sizeof (UINT32), NameStringLen);
  }

  SetDevicePathEndNode (&EndOfDevicePath);
  CopyMem (
    (UINT8 *) &NewImageExeInfo->InfoSize + sizeof (UINT32) +  NameStringLen,
    DevicePath == NULL ? &EndOfDevicePath : DevicePath,
    DevicePathSize
    );

  if (Signature != NULL) {
    CopyMem (
      (UINT8 *) &NewImageExeInfo->InfoSize + sizeof (UINT32) + NameStringLen + DevicePathSize,
      Signature,
      SignatureSize
      );
  }

  *ImageInfo     = NewImageExeInfo;
  *ImageInfoSize = NewImageSize;
  return EFI_SUCCESS;
}

/**
  According to input information to check this image information is whether saved in execution information table.

  @param[in] Action             Describes the action taken by the firmware regarding this image.
  @param[in] Name               Input a null-terminated, user-friendly name.
  @param[in] DevicePath         Input device path pointer.
  @param[in] Signature          Input signature info in EFI_SIGNATURE_LIST data structure.
  @param[in] SignatureSize      Size of signature.
  @param[in] ImageExeInfoTable  pointer to start address of EFI_IMAGE_EXECUTION_INFO_TABLE instance.

  @retval TRUE                  The image information exists in execution information table.
  @retval FALSE                 The image information doesn't exist in execution information table.
**/
STATIC
BOOLEAN
IsImageInfoExistence (
  IN  EFI_IMAGE_EXECUTION_INFO         *NewImageInfoEntry,
  IN  UINTN                            NewImageInfoSize,
  IN  EFI_IMAGE_EXECUTION_INFO_TABLE   *ImageExeInfoTable
  )
{
  UINTN                      NumberOfImages;
  EFI_IMAGE_EXECUTION_INFO   *ImageExeInfoItem;
  UINTN                      Index;

  ASSERT (NewImageInfoEntry != NULL && NewImageInfoSize != 0);
  if (ImageExeInfoTable == NULL) {
    return FALSE;
  }
  NumberOfImages = ImageExeInfoTable->NumberOfImages;
  ImageExeInfoItem  = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoTable + sizeof (EFI_IMAGE_EXECUTION_INFO_TABLE));

  for (Index = 0; Index < NumberOfImages; Index++) {
   if (NewImageInfoSize == (UINTN) ReadUnaligned32 ((UINT32 *) &ImageExeInfoItem->InfoSize) &&
       CompareMem (NewImageInfoEntry, ImageExeInfoItem, NewImageInfoSize) == 0) {
     return TRUE;
   }
   ImageExeInfoItem = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoItem + ReadUnaligned32 ((UINT32 *) &ImageExeInfoItem->InfoSize));
  }

  return FALSE;
}

/**
  Create an Image Execution Information Table entry and add it to system configuration table.

  @param[in] Action            Describes the action taken by the firmware regarding this image.
  @param[in] Name              Input a null-terminated, user-friendly name.
  @param[in] DevicePath        Input device path pointer.
  @param[in] Signature         Input signature info in EFI_SIGNATURE_LIST data structure.
  @param[in] SignatureSize     Size of signature.

  @retval EFI_SUCCESS          Add new image execution information to table successful
  @retval EFI_ALREADY_STARTED  The image execution information already exists.
  @return Other                Create new image execution information failed.
**/
EFI_STATUS
AddImageExeInfo (
  IN       EFI_IMAGE_EXECUTION_ACTION       Action,
  IN       CHAR16                           *Name OPTIONAL,
  IN CONST EFI_DEVICE_PATH_PROTOCOL         *DevicePath,
  IN       EFI_SIGNATURE_LIST               *Signature OPTIONAL,
  IN       UINTN                            SignatureSize
  )
{
  EFI_STATUS                      Status;
  EFI_IMAGE_EXECUTION_INFO_TABLE  *ImageExeInfoTable;
  EFI_IMAGE_EXECUTION_INFO_TABLE  *NewImageExeInfoTable;
  EFI_IMAGE_EXECUTION_INFO        *ImageExeInfoEntry;
  EFI_IMAGE_EXECUTION_INFO        *NewImageExeInfoEntry;
  UINTN                           ImageExeInfoTableSize;
  UINTN                           NewImageExeInfoEntrySize;

  NewImageExeInfoEntry = NULL;
  NewImageExeInfoEntrySize = 0;

  Status = CreateNewImageInfo (Action, Name, DevicePath, Signature, SignatureSize, &NewImageExeInfoEntry, &NewImageExeInfoEntrySize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  EfiGetSystemConfigurationTable (&gEfiImageSecurityDatabaseGuid, (VOID**) &ImageExeInfoTable);
  if (ImageExeInfoTable != NULL) {
    //
    // Add this image information to execution information table only if this image information isn't saved in this table.
    //
    if (IsImageInfoExistence (NewImageExeInfoEntry, NewImageExeInfoEntrySize, ImageExeInfoTable)) {
      FreePool (NewImageExeInfoEntry);
      return EFI_ALREADY_STARTED;
    }
    //
    // The table has been found!
    // We must enlarge the table to accmodate the new exe info entry.
    //
    ImageExeInfoTableSize = GetImageExeInfoTableSize (ImageExeInfoTable);
  } else {
    //
    // Not Found!
    // We should create a new table to append to the configuration table.
    //
    ImageExeInfoTableSize = sizeof (EFI_IMAGE_EXECUTION_INFO_TABLE);
  }

  NewImageExeInfoTable = (EFI_IMAGE_EXECUTION_INFO_TABLE *) AllocateRuntimePool (ImageExeInfoTableSize + NewImageExeInfoEntrySize);
  ASSERT (NewImageExeInfoTable != NULL);
  if (NewImageExeInfoTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (ImageExeInfoTable != NULL) {
    CopyMem (NewImageExeInfoTable, ImageExeInfoTable, ImageExeInfoTableSize);
  } else {
    NewImageExeInfoTable->NumberOfImages = 0;
  }
  NewImageExeInfoTable->NumberOfImages++;
  ImageExeInfoEntry = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) NewImageExeInfoTable + ImageExeInfoTableSize);
  CopyMem (ImageExeInfoEntry, NewImageExeInfoEntry, NewImageExeInfoEntrySize);
  //
  // Update/replace the image execution table.
  //
  Status = gBS->InstallConfigurationTable (&gEfiImageSecurityDatabaseGuid, (VOID *) NewImageExeInfoTable);
  ASSERT_EFI_ERROR (Status);

  //
  // Free Old table data!
  //
  if (ImageExeInfoTable != NULL) {
    FreePool (ImageExeInfoTable);
  }
  FreePool (NewImageExeInfoEntry);

  return EFI_SUCCESS;

}

/**
  Check whether signature is in specified database.

  @param[in]  VariableName        Name of database variable that is searched in.
  @param[in]  Signature           Pointer to signature that is searched for.
  @param[in]  CertType            Pointer to hash algrithom.
  @param[in]  SignatureSize       Size of Signature.

  @return TRUE
  @return FALSE

**/
BOOLEAN
IsSignatureFoundInDatabase (
  IN CHAR16             *VariableName,
  IN UINT8              *Signature,
  IN EFI_GUID           *CertType,
  IN UINTN              SignatureSize
  )
{
  EFI_STATUS          Status;
  EFI_SIGNATURE_LIST  *CertList;
  EFI_SIGNATURE_DATA  *Cert;
  UINTN               DataSize;
  UINT8               *Data;
  UINTN               Index;
  UINTN               CertCount;
  BOOLEAN             IsFound;
  //
  // Read signature database variable.
  //
  IsFound   = FALSE;
  Data      = NULL;
  Status = CommonGetVariableDataAndSize (
             VariableName,
             &gEfiImageSecurityDatabaseGuid,
             &DataSize,
             (VOID**) &Data
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  //
  // Enumerate all signature data in SigDB to check if executable's signature exists.
  //
  CertList = (EFI_SIGNATURE_LIST *) Data;
  while ((DataSize > 0) && (DataSize >= CertList->SignatureListSize)) {
    //
    // check the size of singature list size first to make sure singature data integrity
    //
   if (CertList->SignatureListSize  < sizeof (EFI_SIGNATURE_LIST)) {
     break;
   }
    CertCount = (CertList->SignatureListSize - CertList->SignatureHeaderSize) / CertList->SignatureSize;
    Cert      = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
    if (CertList->SignatureSize == sizeof(EFI_SIGNATURE_DATA) - 1 + SignatureSize) {
      for (Index = 0; Index < CertCount; Index++) {
        if (CompareMem (Cert->SignatureData, Signature, SignatureSize) == 0) {
          //
          // Find the signature in database.
          //
          IsFound = TRUE;
          break;
        }
        Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) Cert + CertList->SignatureSize);
      }

      if (IsFound) {
        break;
      }
    }

    DataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

Done:
  if (Data != NULL) {
    FreePool (Data);
  }

  return IsFound;
}

/**
  Verify PKCS#7 SignedData using certificate found in Variable which formatted
  as EFI_SIGNATURE_LIST. The Variable may be PK, KEK, DB or DBX.

  @param[in] AuthData      Pointer to the Authenticode Signature retrieved from signed image.
  @param[in] AuthDataSize  Size of the Authenticode Signature in bytes.
  @param[in] VariableName  Name of Variable to search for Certificate.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE             Image pass verification.
  @retval FALSE            Image fail verification.
**/

BOOLEAN
IsPkcsSignedDataVerifiedBySignatureList (
  IN UINT8              *AuthData,
  IN UINTN              AuthDataSize,
  IN CHAR16             *VariableName,
  IN EFI_GUID           *VendorGuid
  )
{
  EFI_STATUS                      Status;
  BOOLEAN                         VerifyStatus;
  EFI_SIGNATURE_LIST              *CertList;
  EFI_SIGNATURE_DATA              *Cert;
  UINTN                           DataSize;
  UINT8                           *Data;
  UINT8                           *RootCert;
  UINTN                           RootCertSize;
  UINTN                           Index;
  UINTN                           CertCount;
  CRYPTO_SERVICES_PROTOCOL        *CryptoServices;
  EFI_TREE_MEASURE_OS_LOADER_AUTHORITY_PROTOCOL   *MeasureOsLoaderAuthorityProtocol;

  Data         = NULL;
  CertList     = NULL;
  Cert         = NULL;
  RootCert     = NULL;
  RootCertSize = 0;
  VerifyStatus = FALSE;

  Status = gBS->LocateProtocol (
                &gCryptoServicesProtocolGuid,
                NULL,
                (VOID **)&CryptoServices
                );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = CommonGetVariableDataAndSize (
             VariableName,
             VendorGuid,
             &DataSize,
             (VOID**) &Data
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  //
  // Find X509 certificate in Signature List to verify the signature in pkcs7 signed data.
  //
  CertList = (EFI_SIGNATURE_LIST *) Data;
  while ((DataSize > 0) && (DataSize >= CertList->SignatureListSize)) {
    if (CertList->SignatureListSize < sizeof (EFI_SIGNATURE_LIST)) {
      break;
    }
    if (CompareGuid (&CertList->SignatureType, &gEfiCertX509Guid)) {
      Cert          = (EFI_SIGNATURE_DATA *) ((UINT8 *) CertList + sizeof (EFI_SIGNATURE_LIST) + CertList->SignatureHeaderSize);
      CertCount     = (CertList->SignatureListSize - sizeof (EFI_SIGNATURE_LIST) - CertList->SignatureHeaderSize) / CertList->SignatureSize;
      for (Index = 0; Index < CertCount; Index++) {
        //
        // Iterate each Signature Data Node within this CertList for verify.
        //
        RootCert      = Cert->SignatureData;
        RootCertSize  = CertList->SignatureSize - sizeof(EFI_GUID);

        //
        // Call AuthenticodeVerify library to Verify Authenticode struct.
        //
        VerifyStatus = CryptoServices->AuthenticodeVerify (
                                         AuthData,
                                         AuthDataSize,
                                         RootCert,
                                         RootCertSize,
                                         (UINT8*) mImageDigest->Sha256Hash,
                                         mImageDigestSize
                                         );
        if (VerifyStatus) {
          Status = gBS->LocateProtocol (
                          &gEfiTrEEMeasureOsLoaderAuthorityProtocolGuid,
                          NULL,
                          (VOID**)&MeasureOsLoaderAuthorityProtocol
                          );
          if (!EFI_ERROR (Status) && MeasureOsLoaderAuthorityProtocol->OsLoader) {
            Status = MeasureOsLoaderAuthorityProtocol->MeasureOsLoaderAuthority (
                       MeasureOsLoaderAuthorityProtocol,
                       Cert,
                       CertList->SignatureSize
                       );
          }
          goto Done;
        }
        Cert = (EFI_SIGNATURE_DATA *) ((UINT8 *) Cert + CertList->SignatureSize);
      }
    }
    DataSize -= CertList->SignatureListSize;
    CertList = (EFI_SIGNATURE_LIST *) ((UINT8 *) CertList + CertList->SignatureListSize);
  }

Done:
  if (Data != NULL) {
    FreePool (Data);
  }

  return VerifyStatus;
}

/**
  Verify certificate in WIN_CERT_TYPE_PKCS_SIGNED_DATA format for secure flash .

  @param[in] AuthData             Pointer to the Authenticode Signature retrieved from signed image.
  @param[in] AuthDataSize         Size of the Authenticode Signature in bytes.

  @retval EFI_SUCCESS             Image pass verification.
  @retval EFI_SECURITY_VIOLATION  Image fail verification.
**/

EFI_STATUS
VerifyAuthenCodeSecureFlash (
  IN UINT8              *AuthData,
  IN UINTN              AuthDataSize
  )
{
  UINT8                           *SecureFlashSetupMode;
  EFI_STATUS                      Status;
  EFI_SIGNATURE_LIST              *CertList;
  UINT8                           *RootCert;
  UINTN                           RootCertSize;
  CRYPTO_SERVICES_PROTOCOL        *CryptoServices;


  Status = gBS->LocateProtocol (
                  &gCryptoServicesProtocolGuid,
                  NULL,
                  (VOID **)&CryptoServices
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EFI_SECURITY_VIOLATION;
  CertList = NULL;
  SecureFlashSetupMode = NULL;
  SecureFlashSetupMode = CommonGetVariableData (L"SecureFlashSetupMode", &gSecureFlashInfoGuid);
  if (SecureFlashSetupMode != NULL) {
    CertList = CommonGetVariableData (L"SecureFlashCertData", &gSecureFlashInfoGuid);
    if (CertList != NULL) {
      RootCert = (UINT8 *)CertList + sizeof (EFI_SIGNATURE_LIST) + sizeof(EFI_GUID);
      RootCertSize = CertList->SignatureSize - sizeof(EFI_GUID);
      Status = CryptoServices->AuthenticodeVerify (
                                 AuthData,
                                 AuthDataSize,
                                 RootCert,
                                 RootCertSize,
                                 (UINT8*)mImageDigest->Sha256Hash,
                                 mImageDigestSize
                                 )
               ? EFI_SUCCESS : EFI_SECURITY_VIOLATION;
    }
  }

  if (CertList != NULL) {
    FreePool (CertList);
  }
  if (SecureFlashSetupMode != NULL) {
    FreePool (SecureFlashSetupMode);
  }
  return Status;
}


/**
  Verify certificate in WIN_CERT_TYPE_PKCS_SIGNED_DATA format .

  @param[in]  AuthData            Pointer to the Authenticode Signature retrieved from signed image.
  @param[in]  AuthDataSize        Size of the Authenticode Signature in bytes.
  @param[out] Action              Output execution action after verify signed data base.

  @retval EFI_SUCCESS             Image pass verification.
  @retval EFI_SECURITY_VIOLATION  Image fail verification.
  @retval other error value
**/

EFI_STATUS
VerifyCertPkcsSignedData (
  IN  UINT8                        *AuthData,
  IN  UINTN                        AuthDataSize,
  OUT EFI_IMAGE_EXECUTION_ACTION   *Action
  )
{
  EFI_STATUS          Status;
  //
  // 1: Find certificate from DBX forbidden database for revoked certificate.
  //
  if (IsPkcsSignedDataVerifiedBySignatureList (AuthData, AuthDataSize, EFI_IMAGE_SECURITY_DATABASE1, &gEfiImageSecurityDatabaseGuid)) {
    //
    // DBX is forbidden database, if Authenticode verification pass with
    // one of the certificate in DBX, this image should be rejected.
    //
    *Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND;
    return EFI_SECURITY_VIOLATION;
  }

  //
  // 2: Find certificate from DB database and try to verify authenticode struct.
  //
  *Action = EFI_IMAGE_EXECUTION_AUTH_SIG_PASSED;
  if (IsPkcsSignedDataVerifiedBySignatureList (AuthData, AuthDataSize, EFI_IMAGE_SECURITY_DATABASE, &gEfiImageSecurityDatabaseGuid)) {
    return EFI_SUCCESS;
  }

  //
  // If iamge can not find or verifry through allowed database (db) and forbidden database (dbx), we should
  // check secure flash here.
  //
  Status = VerifyAuthenCodeSecureFlash (AuthData, AuthDataSize);
  if (Status != EFI_SUCCESS) {
    *Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FAILED;
  }
  return Status;
}

/**
  Retrieves the magic value from the PE/COFF header.

  @param[in] Pe32Header                      The buffer in which to return the PE32, PE32+, or TE header.

  @return EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC  Image is PE32
  @return EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC  Image is PE32+

**/
STATIC
UINT16
PeCoffLoaderGetPeHeaderMagicValue (
  IN  EFI_IMAGE_OPTIONAL_HEADER_UNION   *Pe32Header
  )
{
  //
  // NOTE: Some versions of Linux ELILO for Itanium have an incorrect magic value
  //       in the PE/COFF Header.  If the MachineType is Itanium(IA64) and the
  //       Magic value in the OptionalHeader is  EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC
  //       then override the returned value to EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC
  //
  if (Pe32Header->Pe32.FileHeader.Machine == IMAGE_FILE_MACHINE_IA64 && Pe32Header->Pe32.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    return EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  }
  //
  // Return the magic value from the PC/COFF Optional Header
  //
  return Pe32Header->Pe32.OptionalHeader.Magic;
}


/**
  Get sub-system type offset from input image.

  @param[in] SourceBuffer        Pointer to the memory location containing copy
                                 of the image to be loaded.
  @param[in] Offset              Output sub-system type offset.

  @retval EFI_SUCCESS            Get sub-system type offset successful.
  @retval EFI_INVALID_PARAMETER  SourceBuffer is NULL or Offset is NULL.
  @retval EFI_UNSUPPORTED        Function doesn't support input image format.
**/
STATIC
EFI_STATUS
GetSubSystemTypeOffSet (
  IN  VOID                *SourceBuffer,
  OUT UINTN               *Offset
  )
{
  EFI_STATUS                      Status;
  EFI_IMAGE_DOS_HEADER            *DosHeader;
  UINTN                           PeCoffHeaderOffset;
  UINTN                           SubSystemTypeOffset;
  EFI_IMAGE_OPTIONAL_HEADER_UNION *ImageHeader;
  UINT16                          MagicNumber;

  if (SourceBuffer == NULL || Offset == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PeCoffHeaderOffset = 0;

  //
  // Chech PE32 is DOS image
  //
  DosHeader          = (EFI_IMAGE_DOS_HEADER *) SourceBuffer;
  if (DosHeader->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    PeCoffHeaderOffset = (UINTN) DosHeader->e_lfanew;
  }

  Status              = EFI_UNSUPPORTED;
  SubSystemTypeOffset = 0;
  ImageHeader         = (EFI_IMAGE_OPTIONAL_HEADER_UNION *) ((UINTN) SourceBuffer + PeCoffHeaderOffset);
  if (ImageHeader->Te.Signature == EFI_TE_IMAGE_HEADER_SIGNATURE) {
    SubSystemTypeOffset = PeCoffHeaderOffset +(UINTN) &ImageHeader->Te.Subsystem - (UINTN) ImageHeader;
    Status    = EFI_SUCCESS;
  } else if (ImageHeader->Pe32.Signature == EFI_IMAGE_NT_SIGNATURE) {
    MagicNumber = PeCoffLoaderGetPeHeaderMagicValue (ImageHeader);
    if (MagicNumber == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
      SubSystemTypeOffset = PeCoffHeaderOffset +(UINTN) &ImageHeader->Pe32.OptionalHeader.Subsystem - (UINTN) ImageHeader;
      Status    = EFI_SUCCESS;

    } else if (MagicNumber == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
      SubSystemTypeOffset = PeCoffHeaderOffset +(UINTN) &ImageHeader->Pe32Plus.OptionalHeader.Subsystem - (UINTN) ImageHeader;
      Status    = EFI_SUCCESS;
    }
  }

  if (!EFI_ERROR (Status)) {
    *Offset = SubSystemTypeOffset;
  }

  return Status;
}

/**
  Compare to image buffer is whether the same.

  It will skip to comapre sub-system byte, since we will overriding driver to different type.

  @param[in] DestinationBuffer  The pointer to the destination buffer to compare.
  @param[in] DestinationSize    The size of destination buffer by bytes.
  @param[in] SourceBuffer       The pointer to the source buffer to compare.
  @param[in] SourceSize         The size of source buffer by bytes.

  @retval TRUE                  These two buffers are the same
  @retval FALSE                 These two buffers aren't the same.
**/
STATIC
BOOLEAN
IsSourceBufferValid (
  IN  VOID           *DestinationBuffer,
  IN  UINTN          DestinationSize,
  IN  VOID           *SourceBuffer,
  IN  UINTN          SourceSize
  )
{
  UINTN         Offset;
  EFI_STATUS    Status;

  if (DestinationBuffer == NULL || SourceBuffer == NULL || DestinationSize != SourceSize) {
    return FALSE;
  }

  Status = GetSubSystemTypeOffSet (SourceBuffer, &Offset);
  if (EFI_ERROR (Status) || Offset > SourceSize) {
    return FALSE;
  }

  //
  // Since some internal drivers changes sub-system type (ex: DXE driver to Runtime driver) to relocate itself, skip checking
  // Sub-system type to prevent from  loading image failed in this situation.
  //
  if (CompareMem (SourceBuffer, DestinationBuffer, Offset) != 0 ||
      CompareMem ((UINT8 *) SourceBuffer + Offset + 1, (UINT8 *) DestinationBuffer + Offset + 1, SourceSize - Offset - 1) != 0) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check the data which saved in input device path is whether same as
  input buffer.

  @param[in] FilePath      Specific file path saved image file.
  @param[in] SourceBuffer  Input image buffer.
  @param[in] SourceSize    size of input buffer by bytes.

  @retval TRUE             The input file path is valid
  @retval FALSE            The input file path is invalid
**/
STATIC
BOOLEAN
IsFilePahtValid (
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *FilePath,
  IN        VOID                       *SourceBuffer,
  IN        UINTN                      SourceSize
  )
{
  VOID            *FileBuffer;
  UINTN           FileSize;
  UINT32          AuthenticationStatus;

  if (FilePath == NULL || SourceBuffer == NULL || SourceSize == 0) {
    return FALSE;
  }

  //
  // Try to get image by FALSE boot policy for the exact boot file path.
  //
  FileBuffer = GetFileBufferByFilePath (FALSE, FilePath, &FileSize, &AuthenticationStatus);
  if (FileBuffer == NULL) {
    //
    // Try to get image by TRUE boot policy for the inexact boot file path.
    //
    FileBuffer = GetFileBufferByFilePath (TRUE, FilePath, &FileSize, &AuthenticationStatus);
  }

  if (FileBuffer == NULL) {
    return FALSE;
  }

  return IsSourceBufferValid (FileBuffer, FileSize, SourceBuffer, SourceSize);
}

/**
  Provide verification service for signed images, which include both signature validation
  and platform policy control. For signature types, both UEFI WIN_CERTIFICATE_UEFI_GUID and
  MSFT Authenticode type signatures are supported.

  In this implementation, only verify external executables when in USER MODE.
  Executables from FV is bypass, so pass in AuthenticationStatus is ignored.

  @param[in]    AuthenticationStatus
                           This is the authentication status returned from the security
                           measurement services for the input file.
  @param[in]    File       This is a pointer to the device path of the file that is
                           being dispatched. This will optionally be used for logging.
  @param[in]    FileBuffer File buffer matches the input file device path.
  @param[in]    FileSize   Size of File buffer matches the input file device path.
  @param[in]    BootPolicy A boot policy that was used to call LoadImage() UEFI service.

  @retval EFI_SUCCESS            The file specified by DevicePath and non-NULL
                                 FileBuffer did authenticate, and the platform policy dictates
                                 that the DXE Foundation may use the file.
  @retval EFI_SUCCESS            The device path specified by NULL device path DevicePath
                                 and non-NULL FileBuffer did authenticate, and the platform
                                 policy dictates that the DXE Foundation may execute the image in
                                 FileBuffer.
  @retval EFI_OUT_RESOURCE       Fail to allocate memory.
  @retval EFI_SECURITY_VIOLATION The file specified by File did not authenticate, and
                                 the platform policy dictates that File should be placed
                                 in the untrusted state. The image has been added to the file
                                 execution table.
  @retval EFI_ACCESS_DENIED      The file specified by File and FileBuffer did not
                                 authenticate, and the platform policy dictates that the DXE
                                 Foundation many not use File.
**/
EFI_STATUS
EFIAPI
DxeImageVerificationHandler (
  IN  UINT32                           AuthenticationStatus,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL   *File,
  IN  VOID                             *FileBuffer,
  IN  UINTN                            FileSize,
  IN  BOOLEAN                          BootPolicy
  )
{
  EFI_STATUS                  Status;
  UINT16                      Magic;
  EFI_IMAGE_DOS_HEADER        *DosHdr;
  EFI_STATUS                  VerifyStatus;
  UINT8                       *SetupMode;
  EFI_SIGNATURE_LIST          *SignatureList;
  UINTN                       SignatureListSize;
  EFI_SIGNATURE_DATA          *Signature;
  EFI_IMAGE_EXECUTION_ACTION  Action;
  WIN_CERTIFICATE             *WinCertificate;
  UINT32                      Policy;
  UINT8                       SecureBootEnforce;
  UINT8                       Sha256Digest[SHA256_DIGEST_SIZE];
  UINTN                       DataSize;
  UINT32                      NumberOfRvaAndSizes;
  WIN_CERTIFICATE_EFI_PKCS    *PkcsCertData;
  WIN_CERTIFICATE_UEFI_GUID   *WinCertUefiGuid;
  UINT8                       *AuthData;
  UINTN                       AuthDataSize;
  EFI_IMAGE_DATA_DIRECTORY    *SecDataDir;
  BOOLEAN                     SupportedSig;
  BOOLEAN                     SignatureValid;
  UINT8                       *SkipRegionDataBuffer;
  UINT8                       *SkipRegionAddress;
  UINTN                       SkipRegionSize;
  UINTN                       SkipRegionOffset;

  SkipRegionDataBuffer = NULL;
  SkipRegionAddress    = NULL;
  SkipRegionOffset     = 0;
  SkipRegionSize       = 0;

  SignatureList     = NULL;
  SignatureListSize = 0;
  WinCertificate    = NULL;
  SecDataDir        = NULL;
  PkcsCertData      = NULL;
  Action            = EFI_IMAGE_EXECUTION_AUTH_UNTESTED;
  //
  // Check the image type and get policy setting.
  //
  switch (GetImageType (File)) {

  case IMAGE_FROM_FV:
    Policy = ALWAYS_EXECUTE;
    if (mEnterBdsPhase && !IsFilePahtValid (File, FileBuffer, FileSize)) {
      Policy = DENY_EXECUTE_ON_SECURITY_VIOLATION;
    }
    break;

  case IMAGE_FROM_OPTION_ROM:
    Policy = PcdGet32 (PcdOptionRomImageVerificationPolicy);
    break;

  case IMAGE_FROM_REMOVABLE_MEDIA:
    Policy = PcdGet32 (PcdRemovableMediaImageVerificationPolicy);
    break;

  case IMAGE_FROM_FIXED_MEDIA:
    Policy = PcdGet32 (PcdFixedMediaImageVerificationPolicy);
    break;

  default:
    Policy = mEnterBdsPhase ? DENY_EXECUTE_ON_SECURITY_VIOLATION : ALWAYS_EXECUTE;
    break;
  }
  //
  // If policy is always/never execute, return directly.
  //
  if (Policy == ALWAYS_EXECUTE) {
    return EFI_SUCCESS;
  } else if (Policy == NEVER_EXECUTE) {
    return EFI_ACCESS_DENIED;
  }

  SetupMode = GetEfiGlobalVariable (EFI_SETUP_MODE_NAME);

  //
  // check SecureFlashSetupMode if SetupMode variable doesn't exit (sysmte doesn't support secure boot)
  // or system is in setup mode.
  //
  if ((SetupMode == NULL) || (*SetupMode == SETUP_MODE)) {
    if (SetupMode != NULL) {
      FreePool (SetupMode);
    }

    SetupMode = CommonGetVariableData (L"SecureFlashSetupMode", &gSecureFlashInfoGuid);
    if (SetupMode == NULL) {
      return EFI_SUCCESS;
    }

  }
  //
  // Return success direclty if system is in setup mode
  //
  if (*SetupMode == SETUP_MODE) {
    FreePool (SetupMode);
    return EFI_SUCCESS;
  }

  //
  // Check SecureBootEnforce variable to determine system wants to disable secure boot or not
  // in user mode.
  //
  DataSize = 1;
  Status = CommonGetVariable (L"SecureBootEnforce", &gEfiGenericVariableGuid, &DataSize, &SecureBootEnforce);
  if (!EFI_ERROR (Status) && SecureBootEnforce == 0) {
    FreePool (SetupMode);
    //
    // If system is in secure flash mode, we must do image verification.
    //
    SetupMode = CommonGetVariableData (L"SecureFlashSetupMode", &gSecureFlashInfoGuid);
    if ((SetupMode == NULL) || (*SetupMode == SETUP_MODE)) {
      if (SetupMode != NULL) {
        FreePool (SetupMode);
      }
      return EFI_SUCCESS;
    }
  }

  //
  // Read the Dos header.
  //
  ASSERT (FileBuffer != NULL);

  //
  // If there is a unsigned FV region, backup the region data and fill with 0xFF.
  //
  BackupSkipRegion ((UINT8 *)FileBuffer, FileSize, &SkipRegionOffset, &SkipRegionSize, &SkipRegionDataBuffer);

  mImageBase  = (UINT8 *) FileBuffer;
  mImageSize  = FileSize;
  DosHdr      = (EFI_IMAGE_DOS_HEADER *) (mImageBase);
  Status = EFI_ACCESS_DENIED;
  SignatureValid = FALSE;
  if (DosHdr->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    //
    // DOS image header is present,
    // so read the PE header after the DOS image header.
    //
    mPeCoffHeaderOffset = DosHdr->e_lfanew;
  } else {
    mPeCoffHeaderOffset = 0;
  }
  //
  // Check PE/COFF image.
  //
  mNtHeader.Pe32 = (EFI_IMAGE_NT_HEADERS32 *) (mImageBase + mPeCoffHeaderOffset);
  if (mNtHeader.Pe32->Signature != EFI_IMAGE_NT_SIGNATURE) {
    //
    // It is not a valid Pe/Coff file.
    //
    goto Done;
  }

  if (mNtHeader.Pe32->FileHeader.Machine == IMAGE_FILE_MACHINE_IA64 && mNtHeader.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
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
    Magic = mNtHeader.Pe32->OptionalHeader.Magic;
  }

  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    NumberOfRvaAndSizes = mNtHeader.Pe32->OptionalHeader.NumberOfRvaAndSizes;
    if (NumberOfRvaAndSizes > EFI_IMAGE_DIRECTORY_ENTRY_SECURITY) {
      SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &mNtHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
    }
  } else {
    //
    // Use PE32+ offset.
    //
    NumberOfRvaAndSizes = mNtHeader.Pe32Plus->OptionalHeader.NumberOfRvaAndSizes;
    if (NumberOfRvaAndSizes > EFI_IMAGE_DIRECTORY_ENTRY_SECURITY) {
      SecDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &mNtHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
    }
  }


  if (mImageDigest == NULL) {
    mImageDigest = AllocateRuntimePool (sizeof (EFI_HASH_OUTPUT));
    if (mImageDigest == NULL) {
      goto Done;
    }
    mImageDigest->Sha256Hash = AllocateRuntimePool (MAX_DIGEST_SIZE);
    if (mImageDigest->Sha256Hash == NULL) {
      goto Done;
    }
  }
  //
  // No matter image is signed or not, we should check the hash of this image is whether in forbidden database(dbx) first
  //
  if (!HashPeImage (HASHALG_SHA256)) {
    goto Done;
  }
  SignatureValid = TRUE;
  CopyMem (Sha256Digest, mImageDigest->Sha256Hash, SHA256_DIGEST_SIZE);
  if (IsSignatureFoundInDatabase (EFI_IMAGE_SECURITY_DATABASE1, Sha256Digest, &gEfiHashAlgorithmSha256Guid, SHA256_DIGEST_SIZE)) {
    //
    // Image Hash is in forbidden database (dbx).
    //
    Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND;
    Status = EFI_ACCESS_DENIED;
    goto Done;
  }

  if ((SecDataDir == NULL) || ((SecDataDir != NULL) && (SecDataDir->Size == 0))) {
    //
    // An unsigned image and its hash doesn't exist in forbidden database, we should check this image is in allowed database (db)
    //
    if (IsSignatureFoundInDatabase (EFI_IMAGE_SECURITY_DATABASE, Sha256Digest, &gEfiHashAlgorithmSha256Guid, SHA256_DIGEST_SIZE)) {
      Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND;
      Status = EFI_SUCCESS;
    } else {
      //
      // Image Hash is not found in both forbidden and allowed database.
      //
      Action = EFI_IMAGE_EXECUTION_AUTH_SIG_NOT_FOUND;
      Status = EFI_ACCESS_DENIED;
    }
    goto Done;
  }

  AuthData       = NULL;
  AuthDataSize   = 0;
  WinCertificate = (WIN_CERTIFICATE *) (mImageBase + SecDataDir->VirtualAddress);
  SupportedSig   = FALSE;
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
        WinCertUefiGuid->Hdr.dwLength >= OFFSET_OF(WIN_CERTIFICATE_UEFI_GUID, CertData)) {
      AuthData = WinCertUefiGuid->CertData;
      AuthDataSize = WinCertUefiGuid->Hdr.dwLength - OFFSET_OF(WIN_CERTIFICATE_UEFI_GUID, CertData);
      SupportedSig = TRUE;
    }
  }

  //
  // Only check allowed Sha256 if this is an unsupported signature.
  //
  if (!SupportedSig) {
    if (IsSignatureFoundInDatabase (EFI_IMAGE_SECURITY_DATABASE, Sha256Digest, &gEfiHashAlgorithmSha256Guid, SHA256_DIGEST_SIZE)) {
      Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND;
      Status = EFI_SUCCESS;
    } else {
      Action = EFI_IMAGE_EXECUTION_AUTH_SIG_NOT_FOUND;
      Status = EFI_ACCESS_DENIED;
    }
    goto Done;
  }

  //
  // If signature hash algorithm is unsupported, Only check allowed Sha256.
  //
  if (HashPeImageByType (AuthData, AuthDataSize) != EFI_SUCCESS) {
    if (IsSignatureFoundInDatabase (EFI_IMAGE_SECURITY_DATABASE, Sha256Digest, &gEfiHashAlgorithmSha256Guid, SHA256_DIGEST_SIZE)) {
      Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND;
      Status = EFI_SUCCESS;
    }
    goto Done;
  }

  //
  // 1. check hash from forbidden data base first
  //
  if (!CompareGuid (&mCertType, &gEfiHashAlgorithmSha256Guid) &&
      IsSignatureFoundInDatabase (EFI_IMAGE_SECURITY_DATABASE1, (UINT8*) mImageDigest->Sha256Hash, &mCertType, mImageDigestSize)) {
    Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND;
    Status = EFI_ACCESS_DENIED;
    goto Done;
  }

  //
  // 2. Verify image from forbidden and allowed database
  //
  VerifyStatus = VerifyCertPkcsSignedData (AuthData, AuthDataSize, &Action);
  if (VerifyStatus == EFI_SUCCESS) {
    Status = EFI_SUCCESS;
  } else if (Action == EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND) {
    //
    // if the verify failed is caused by verifing imaged from forbidden, should not check allowed database.
    //
    Status = EFI_ACCESS_DENIED;

  //
  // 3. Finally, check hash from allowed database
  //
  } else if (IsSignatureFoundInDatabase (EFI_IMAGE_SECURITY_DATABASE, Sha256Digest, &gEfiHashAlgorithmSha256Guid, SHA256_DIGEST_SIZE)) {
    Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND;
    Status = EFI_SUCCESS;
  } else if (!CompareGuid (&mCertType, &gEfiHashAlgorithmSha256Guid) &&
             IsSignatureFoundInDatabase (EFI_IMAGE_SECURITY_DATABASE, (UINT8*) mImageDigest->Sha256Hash, &mCertType, mImageDigestSize)) {
    Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FOUND;
    Status = EFI_SUCCESS;
  } else {
    Action = EFI_IMAGE_EXECUTION_AUTH_SIG_FAILED;
    Status = EFI_ACCESS_DENIED;
  }

Done:

  if (Status != EFI_SUCCESS) {
    if (SignatureValid) {
      //
      // Get image hash value as executable's signature.
      //
      SignatureListSize = sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1 + mImageDigestSize;
      SignatureList     = (EFI_SIGNATURE_LIST *) AllocateZeroPool (SignatureListSize);
      ASSERT (SignatureList != NULL);
      if (SignatureList != NULL) {
        SignatureList->SignatureHeaderSize  = 0;
        SignatureList->SignatureListSize    = (UINT32) SignatureListSize;
        SignatureList->SignatureSize        = (UINT32) mImageDigestSize;
        CopyMem (&SignatureList->SignatureType, &mCertType, sizeof (EFI_GUID));
        Signature = (EFI_SIGNATURE_DATA *) ((UINT8 *) SignatureList + sizeof (EFI_SIGNATURE_LIST));
        CopyMem (Signature->SignatureData, (UINT8*)mImageDigest->Sha256Hash, mImageDigestSize);
      }
    }
    //
    // Policy decides to defer or reject the image; add its information in image executable information table.
    //
    AddImageExeInfo (Action, NULL, File, SignatureList, SignatureListSize);
    Status = EFI_SECURITY_VIOLATION;
  }

  //
  // If the unsigned FV region exisits, need to restore the data.
  //
  RestoreSkipRegion ((UINT8 *)FileBuffer, FileSize, SkipRegionOffset, SkipRegionSize, SkipRegionDataBuffer);

  if (SignatureList != NULL) {
    FreePool (SignatureList);
  }
  if (mImageDigest != NULL) {
    if (mImageDigest->Sha256Hash != NULL) {
      FreePool (mImageDigest->Sha256Hash);
    }
    FreePool (mImageDigest);
    mImageDigest = NULL;
  }
  FreePool (SetupMode);

  return Status;
}

/**
  Register security measurement handler.

  @param  ImageHandle   ImageHandle of the loaded driver.
  @param  SystemTable   Pointer to the EFI System Table.

  @retval EFI_SUCCESS   The handlers were registered successfully.
**/
EFI_STATUS
EFIAPI
DxeImageVerificationLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

  InitializeBdsPhaseCheck (SystemTable);
  mCertBuffer  = AllocateRuntimePool (CERT_BUFFER_SIZE);
  ASSERT (mCertBuffer != NULL);

  return RegisterSecurity2Handler (
           DxeImageVerificationHandler,
           EFI_AUTH_OPERATION_VERIFY_IMAGE | EFI_AUTH_OPERATION_IMAGE_REQUIRED
           );
}
