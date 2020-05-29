/** @file
  General purpose supporting routines for file access PEIM

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FatLib.c

Abstract:

  General purpose supporting routines for file access PEIM

--*/

#include "FatPeim.h"

/**
  Converts a union code character to upper case.
  This functions converts a unicode character to upper case.
  If the input Letter is not a lower-cased letter,
  the original value is returned.

  @param  Letter            The input unicode character. 

  @return The upper cased letter.

**/
STATIC
CHAR16
ToUpper (
  IN CHAR16                    Letter
  )
{
  if ('a' <= Letter && Letter <= 'z') {
    Letter = (CHAR16) (Letter - 0x20);
  }

  return Letter;
}


/**
  Reads a block of data from the block device by calling
  underlying Block I/O service.

  @param  PrivateData       Global memory map for accessing global variables 
  @param  BlockDeviceNo     The index for the block device number. 
  @param  Lba               The logic block address to read data from. 
  @param  BufferSize        The size of data in byte to read. 
  @param  Buffer            The buffer of the 

  @retval EFI_DEVICE_ERROR  The specified block device number exceeds the maximum 
                            device number. 
  @retval EFI_DEVICE_ERROR  The maximum address has exceeded the maximum address 
                            of the block device.

**/
EFI_STATUS
FatReadBlock (
  IN  PEI_FAT_PRIVATE_DATA   *PrivateData,
  IN  UINTN                  BlockDeviceNo,
  IN  EFI_PEI_LBA            Lba,
  IN  UINTN                  BufferSize,
  OUT VOID                   *Buffer
  )
{
  EFI_STATUS            Status;
  PEI_FAT_BLOCK_DEVICE  *BlockDev;

  if (BlockDeviceNo > PEI_FAT_MAX_BLOCK_DEVICE - 1) {
    return EFI_DEVICE_ERROR;
  }

  Status    = EFI_SUCCESS;
  BlockDev  = &(PrivateData->BlockDevice[BlockDeviceNo]);

  if (BufferSize > MultU64x32 (BlockDev->LastBlock - Lba + 1, BlockDev->BlockSize)) {
    return EFI_DEVICE_ERROR;
  }

  if (!BlockDev->Logical) {
    
    Status = BlockDev->BlockIo->ReadBlocks (
                                  (EFI_PEI_SERVICES **) GetPeiServicesTablePointer (),
                                  BlockDev->BlockIo,
                                  BlockDev->PhysicalDevNo,
                                  Lba,
                                  BufferSize,
                                  Buffer
                                  );
    
  } else {
    
    Status = FatReadDisk (
              PrivateData,
              BlockDev->ParentDevNo,
              BlockDev->StartingPos + MultU64x32 (Lba, BlockDev->BlockSize),
              BufferSize,
              Buffer
              );
  }

  return Status;
}

/**
  Write a block of data to the block device by calling
  underlying Block I/O service.

  @param  PrivateData       Global memory map for accessing global variables 
  @param  BlockDeviceNo     The index for the block device number. 
  @param  Lba               The logic block address to read data from. 
  @param  BufferSize        The size of data in byte to read. 
  @param  Buffer            The buffer of the 

  @retval EFI_DEVICE_ERROR  The specified block device number exceeds the maximum 
                            device number. 
  @retval EFI_DEVICE_ERROR  The maximum address has exceeded the maximum address 
                            of the block device.

**/
EFI_STATUS
FatWriteBlock (
  IN  PEI_FAT_PRIVATE_DATA   *PrivateData,
  IN  UINTN                  BlockDeviceNo,
  IN  EFI_PEI_LBA            Lba,
  IN  UINTN                  BufferSize,
  IN  VOID                   *Buffer
  )
{
  EFI_STATUS            Status;
  PEI_FAT_BLOCK_DEVICE  *BlockDev;

  if (BlockDeviceNo > PEI_FAT_MAX_BLOCK_DEVICE - 1) {
    return EFI_DEVICE_ERROR;
  }

  Status    = EFI_SUCCESS;
  BlockDev  = &(PrivateData->BlockDevice[BlockDeviceNo]);

  if (BufferSize > MultU64x32 (BlockDev->LastBlock - Lba + 1, BlockDev->BlockSize)) {
    return EFI_DEVICE_ERROR;
  }

  if (!BlockDev->Logical) {
    
    Status = BlockDev->H2OBlockIo->WriteBlocks (
                                  (EFI_PEI_SERVICES **) GetPeiServicesTablePointer (),
                                  BlockDev->H2OBlockIo,
                                  BlockDev->PhysicalDevNo,
                                  Lba,
                                  BufferSize,
                                  Buffer
                                  );
    
  } else {
    Status = FatWriteDisk (
              PrivateData,
              BlockDev->ParentDevNo,
              BlockDev->StartingPos + MultU64x32 (Lba, BlockDev->BlockSize),
              BufferSize,
              Buffer
              );
  }

  return Status;
}

/**
  Find a cache block designated to specific Block device and Lba.
  If not found, invalidate an oldest one and use it. (LRU cache)

  @param  PrivateData       the global memory map. 
  @param  BlockDeviceNo    the Block device. 
  @param  Lba                  the Logical Block Address 
  @param  CachePtr           Ptr to the starting address of the memory holding the 
                            data; 

  @retval EFI_SUCCESS       The function completed successfully.
  @retval EFI_DEVICE_ERROR  Something error while accessing media.

**/
EFI_STATUS
FatGetCacheBlock (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 BlockDeviceNo,
  IN  UINT64                Lba,
  OUT CHAR8                 **CachePtr
  )
{
  EFI_STATUS            Status;
  PEI_FAT_CACHE_BUFFER  *CacheBuffer;
  INTN                  Index;
  STATIC UINT8          Seed;

  Status      = EFI_SUCCESS;
  CacheBuffer = NULL;

  //
  // go through existing cache buffers
  //
  for (Index = 0; Index < PEI_FAT_CACHE_SIZE; Index++) {
    CacheBuffer = &(PrivateData->CacheBuffer[Index]);
    if (CacheBuffer->Valid && CacheBuffer->BlockDeviceNo == BlockDeviceNo && CacheBuffer->Lba == Lba) {
      break;
    }
  }

  if (Index < PEI_FAT_CACHE_SIZE) {
    *CachePtr = (CHAR8 *) CacheBuffer->Buffer;
    return EFI_SUCCESS;
  }
  
  //
  // We have to find an invalid cache buffer
  //
  for (Index = 0; Index < PEI_FAT_CACHE_SIZE; Index++) {
    if (!PrivateData->CacheBuffer[Index].Valid) {
      break;
    }
  }
  
  //
  // Use the cache buffer
  //
  if (Index == PEI_FAT_CACHE_SIZE) {
    Index = (Seed++) % PEI_FAT_CACHE_SIZE;
  }

  CacheBuffer                 = &(PrivateData->CacheBuffer[Index]);
  CacheBuffer->BlockDeviceNo  = BlockDeviceNo;
  CacheBuffer->Lba            = Lba;
  CacheBuffer->Size           = PrivateData->BlockDevice[BlockDeviceNo].BlockSize;

  //
  // Read in the data
  //
  Status = FatReadBlock (
            PrivateData,
            BlockDeviceNo,
            Lba,
            CacheBuffer->Size,
            CacheBuffer->Buffer
            );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  CacheBuffer->Valid  = TRUE;
  *CachePtr           = (CHAR8 *) CacheBuffer->Buffer;

  return Status;
}


/**
  Disk reading.

  @param  PrivateData       the global memory map; 
  @param  BlockDeviceNo     the block device to read; 
  @param  StartingAddress   the starting address. 
  @param  Size              the amount of data to read. 
  @param  Buffer            the buffer holding the data 

  @retval EFI_SUCCESS       The function completed successfully.
  @retval EFI_DEVICE_ERROR  Something error.

**/
EFI_STATUS
FatReadDisk (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 BlockDeviceNo,
  IN  UINT64                StartingAddress,
  IN  UINTN                 Size,
  OUT VOID                  *Buffer
  )
{
  EFI_STATUS  Status;
  UINT32      BlockSize;
  CHAR8       *BufferPtr;
  CHAR8       *CachePtr;
  UINT32      Offset;
  UINT64      Lba;
  UINT64      OverRunLba;
  UINTN       Amount;

  Status    = EFI_SUCCESS;
  BufferPtr = Buffer;
  BlockSize = PrivateData->BlockDevice[BlockDeviceNo].BlockSize;

  //
  // Read underrun
  //
  Lba     = DivU64x32Remainder (StartingAddress, BlockSize, &Offset);
  Status  = FatGetCacheBlock (PrivateData, BlockDeviceNo, Lba, &CachePtr);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  Amount = Size < (BlockSize - Offset) ? Size : (BlockSize - Offset);
  CopyMem (BufferPtr, CachePtr + Offset, Amount);

  if (Size == Amount) {
    return EFI_SUCCESS;
  }

  Size -= Amount;
  BufferPtr += Amount;
  StartingAddress += Amount;
  Lba += 1;

  if (!Size) {
    return EFI_SUCCESS;
  }
  
  //
  // Read aligned parts
  //
  OverRunLba = Lba + DivU64x32Remainder (Size, BlockSize, &Offset);

  Size -= Offset;
  Status = FatReadBlock (PrivateData, BlockDeviceNo, Lba, Size, BufferPtr);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  BufferPtr += Size;

  //
  // Read overrun
  //
  if (Offset != 0) {
    Status = FatGetCacheBlock (PrivateData, BlockDeviceNo, OverRunLba, &CachePtr);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
    CopyMem (BufferPtr, CachePtr, Offset);
  }

  return Status;
}

/**
  Disk Writing.

  @param  PrivateData       the global memory map; 
  @param  BlockDeviceNo     the block device to write; 
  @param  StartingAddress   the starting address. BYTE address
  @param  Size              the amount of data to write. 
  @param  Buffer            the buffer holding the data 

  @retval EFI_SUCCESS       The function completed successfully.
  @retval EFI_DEVICE_ERROR  Something error.

**/
EFI_STATUS
FatWriteDisk (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN  UINTN                 BlockDeviceNo,
  IN  UINT64                StartingAddress,
  IN  UINTN                 Size,
  IN  VOID                  *Buffer
  )
{
  EFI_STATUS  Status;
  UINT32      BlockSize;
  CHAR8       *BufferPtr;
  CHAR8       *CachePtr;
  UINT32      Offset;
  UINT64      Lba;
  UINT64      OverRunLba;
  UINTN       Amount;

  Status    = EFI_SUCCESS;
  BufferPtr = Buffer;
  BlockSize = PrivateData->BlockDevice[BlockDeviceNo].BlockSize;

  //
  // Write underrun
  // Offset starts in the middle of an Lba, so do read modify write.
  //
  Lba     = DivU64x32Remainder (StartingAddress, BlockSize, &Offset);
  Status  = FatGetCacheBlock (PrivateData, BlockDeviceNo, Lba, &CachePtr);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  Amount = Size < (BlockSize - Offset) ? Size : (BlockSize - Offset);
  CopyMem (CachePtr + Offset, BufferPtr, Amount);

  Status = FatWriteBlock (PrivateData, BlockDeviceNo, Lba, BlockSize, CachePtr);
  
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }
  
  if (Size == Amount) {
    return EFI_SUCCESS;
  }

  Size -= Amount;
  BufferPtr += Amount;
  StartingAddress += Amount;
  Lba += 1;

  if (!Size) {
    return EFI_SUCCESS;
  }
  
  //
  // Write aligned parts
  //
  OverRunLba = Lba + DivU64x32Remainder (Size, BlockSize, &Offset);

  Size -= Offset;
  Status = FatWriteBlock (PrivateData, BlockDeviceNo, Lba, Size, BufferPtr);
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  BufferPtr += Size;

  //
  // Write overrun
  //
  if (Offset != 0) {
    Status = FatGetCacheBlock (PrivateData, BlockDeviceNo, OverRunLba, &CachePtr);
    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
    CopyMem (CachePtr, BufferPtr, Offset);
    
    Status = FatWriteBlock (PrivateData, BlockDeviceNo, OverRunLba, BlockSize, CachePtr);

    if (EFI_ERROR (Status)) {
      return EFI_DEVICE_ERROR;
    }
  }

  return Status;
}

/**
  Calculating FAT directory entry checksum

  @param  Dir               Pointer to the FAT_DIRECTORY_ENTRY structure

  @return                   The calculated directory entry checksum

**/
UINT8
FatDirEntryChecksum (
  FAT_DIRECTORY_ENTRY *Dir
  )
{
  UINT8 Check;
  UINTN Index;

  //
  // Computes checksum for LFN
  //
  Check = 0;
  for (Index = 0; Index < 11; Index++) {
    Check = (UINT8) (((Check & 1) ? 0x80 : 0) + (Check >> 1) + Dir->FileName[Index]);
  }

  return Check;
}

/**
  This version is different from the version in Unicode collation
  protocol in that this version strips off trailing blanks.
  Converts an 8.3 FAT file name using an OEM character set
  to a Null-terminated Unicode string.
  Here does not expand DBCS FAT chars.

  @param  FatSize           The size of the string Fat in bytes. 
  @param  Fat               A pointer to a Null-terminated string that contains 
                            an 8.3 file name using an OEM character set. 
  @param  Str               A pointer to a Null-terminated Unicode string. The 
                            string must be allocated in advance to hold FatSize 
                            Unicode characters

**/
VOID
EngFatToStr (
  IN UINTN                            FatSize,
  IN CHAR8                            *Fat,
  OUT CHAR16                          *Str
  )
{
  CHAR16  *String;

  String = Str;
  //
  // No DBCS issues, just expand and add null terminate to end of string
  //
  while (*Fat != 0 && FatSize != 0) {
    *String = *Fat;
    String += 1;
    Fat += 1;
    FatSize -= 1;
  }
  //
  // Strip off trailing blanks
  //
  while (String > Str && *(String - 1) == ' ') {
    String -= 1;
  }

  *String = 0;
}

/**
  Converts a Null-terminated string to legal characters in a FAT 
  filename using an OEM character set. 

  @param  This    Protocol instance pointer.
  @param  String  A pointer to a Null-terminated string. The string must
                  be preallocated to hold FatSize characters.
  @param  FatSize The size of the string Fat in bytes.
  @param  Fat     A pointer to a Null-terminated string that contains an 8.3 file
                  name using an OEM character set.

  @retval TRUE    Fat is a Long File Name
  @retval FALSE   Fat is an 8.3 file name

**/
BOOLEAN
EFIAPI
EngStrToFat (
  IN CHAR16                           *String,
  IN UINTN                            FatSize,
  OUT CHAR8                           *Fat
  )
{
  BOOLEAN SpecialCharExist;
  CHAR16  Tmp;

  SpecialCharExist = FALSE;
  while ((*String != 0) && (FatSize != 0)) {
    
    if (*String > 0x7F || 
        *String < 0x20 ||
        *String == '\"' ||
        *String == '*' ||
        *String == '/' ||
        *String == ':' ||
        *String == '<' ||
        *String == '>' ||
        *String == '?' ||
        *String == '\\' ||
        *String == '|') {

      SpecialCharExist = TRUE;
      *(Fat++) = '_';
      String++;
      
    } else {
      if (*String >= 'a' && *String <= 'z') {
        Tmp = *String - 0x20;
        String++;
        *(Fat++) = (CHAR8)Tmp; 
        
      } else {
        *(Fat++) = (CHAR8)*(String++);   
      }       
    }
    FatSize--;
  }
  //
  // Do not terminate that fat string
  //
  return SpecialCharExist;
}


/**
  Performs a case-insensitive comparison of two Null-terminated Unicode strings.

  @param  PrivateData       Global memory map for accessing global variables 
  @param  Str1              First string to perform case insensitive comparison. 
  @param  Str2              Second string to perform case insensitive comparison.
  
  @retval TRUE              Str1 and Str2 are identical with case-insensitive comparison 
  @retval FALSE             Str1 and Str2 are different

**/
BOOLEAN
EngStriColl (
  IN  PEI_FAT_PRIVATE_DATA  *PrivateData,
  IN CHAR16                 *s1,
  IN CHAR16                 *s2
  )
{
  while (*s1) {
    if (ToUpper (*s1) != ToUpper (*s2)) {
      return FALSE;
    }

    s1++;
    s2++;
  }

  return (*s2) ? FALSE : TRUE;
}

