//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; Revision History:
//;
//; $Log: 
//; 
//;------------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>

#include <Pi/PiFirmwareVolume.h>
#include "GenUnsignedFv.h"

/**
  Verify Flash Layout

  @param  FlashAreaInfo -Flash Map infomation

  @retval 0 - Success
  @retval 2 - Flash Layout error
**/
UINT8
VerifyFvRegion (
  IN FLASH_AREA_INFO FlashAreaInfo
  )
{
  UINT32 Temp;

  Temp = FlashAreaInfo.FlashUnsignedFvRegionBase  + FlashAreaInfo.EndOfFlashUnsignedFvRegionTotalSize - 1;
  if (Temp != FlashAreaInfo.EndOfFlashUnsignedFvRegionBase) {
    return STATUS_INVALID_PARAMETER;
  }
  
  return STATUS_SUCCESS;
}

/**
  Read data from Fd binary file

  @param  FileName -File name for the image
  @param  Binary - Base address of binary image
  @param  BinarySize - Binary image size

  @retval 0 - Success
  @retval 1 - File not found
**/
UINT8
ReadFdFile (
  IN     char   *FileName,
  IN OUT UINT8  **Binary,
  IN OUT UINT32 *BinarySize
  )
{
  FILE   *FilePtr;
  UINT8  *Data;
  UINT32 FileSize;
  UINTN  Index;

  FilePtr    = 0;
  FileSize   = 0;
  Index      = 0;
  Data       = NULL;

  //
  // Open  file
  // 
  FilePtr = fopen(FileName, "rb");
  if (FilePtr == NULL) {
    return STATUS_NOT_FOUND;
  }

  //
  // Get FD File size from PCD
  //
  FileSize = PcdGet32 (PcdFlashAreaSize);
  *BinarySize = FileSize;
  
  //
  // Copy FD file data to Binary buffer 
  //
  Data = malloc(FileSize);
  if (Data == NULL) {
    fclose (FilePtr);
    return STATUS_MALLOC_FAIL;
  }
  *Binary = Data;

  for (Index = 0; Index < FileSize; Index++, Data++) {
    fread (Data, 1, 1, FilePtr);
  }
  
  //
  // Close file
  //
  fclose (FilePtr);

  return STATUS_SUCCESS;
}

/**
  Write data into Fd binary file

  @param  FileName -File name for the image
  @param  Binary - Base address of binary image
  @param  BinarySize - Binary image size

  @retval 0 - Success
  @retval 1 - File not found
**/
UINT8
WriteFdFile (
  IN char   *FileName,
  IN UINT8  **Binary,
  IN UINT32 *BinarySize
  )
{
  FILE   *FilePtr;
  
  //
  // Open  file
  // 
  FilePtr = fopen(FileName, "rb+");
  if (FilePtr == NULL) {
    return STATUS_NOT_FOUND;
  }

  //
  // Store Binary to FD file
  //
  fwrite (*Binary, 1, *BinarySize, FilePtr);

  //
  // Close file
  //
  fclose (FilePtr);

  return STATUS_SUCCESS;
}

/**
   Restructure data in binary file 

  @param  FlashAreaInfo -Flash Map infomation
  @param  Binary - Base address of binary image
  @param  BinarySize - Binary image size

  @retval 0 - Success
  @retval 1 - File not found
**/
UINT8
UpdateFdFile (
  IN FLASH_AREA_INFO  FlashAreaInfo,
  IN OUT UINT8        **Binary,
  IN OUT UINT32       *BinarySize
  )
{
  UINT8   *UnFvBaseAddress;
  UINT32  Temp;
  EFI_FIRMWARE_VOLUME_HEADER *FvHeader;
  
  //
  // Initialize the Unsigned Fv base info
  //
  Temp = FlashAreaInfo.FlashUnsignedFvRegionBase - FlashAreaInfo.FlashAreaBaseAddress;
  UnFvBaseAddress = (UINT8 *)(UINTN)(*Binary + Temp);
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *)UnFvBaseAddress;

  //
  // Step 1. Update Unsigned Fv Length
  // 
  FvHeader->FvLength = FlashAreaInfo.EndOfFlashUnsignedFvRegionTotalSize;

  //
  // Step 2. Update Checksum
  //
  FvHeader->Checksum = 0;
  FvHeader->Checksum = CalculateCheckSum16 ((UINT16 *) FvHeader, FvHeader->HeaderLength);
                       
  return STATUS_SUCCESS;
}

/**
  Main entry point to SEC for WinNt. This is a Windows program

  @param  Argc - Number of command line arguments
  @param  Argv - Array of command line argument strings
  @param  Envp - Array of environmemt variable strings

  @retval 0 - Normal exit
  @retval 1 - Abnormal exit
**/
int
main (
  IN INTN  Argc,
  IN CHAR8 **Argv,
  IN CHAR8 **Envp
  )
{
  UINT8        Status;
  UINT32       *Binary;
  UINT32       BinarySize;
  CHAR8        *FileName;
  FLASH_AREA_INFO FlashAreaInfo;

  Status = STATUS_SUCCESS;

  if (Argc != 2) {
    printf ("GenFactoryCopy BIOS.fd\n");
    return STATUS_INVALID_PARAMETER;
  }
  
  Binary     = NULL;
  BinarySize = 0;  
  FileName   = Argv[1];

  printf ("\n--== Unsigned FV Generator Start ==--\n\n");

  //
  // Step 1. Get Flash Map information from PCD and Verify it 
  //
  FlashAreaInfo.FlashAreaBaseAddress            = PcdGet32(PcdFlashAreaBaseAddress);
  FlashAreaInfo.FlashAreaSize                   = PcdGet32(PcdFlashAreaSize);
  FlashAreaInfo.FlashUnsignedFvRegionBase       = PcdGet32(PcdFlashUnsignedFvRegionBase);
  FlashAreaInfo.FlashUnsignedFvRegionSize       = PcdGet32(PcdFlashUnsignedFvRegionSize);
  FlashAreaInfo.EndOfFlashUnsignedFvRegionBase  = PcdGet32(PcdEndOfFlashUnsignedFvRegionOffset) + PcdGet32(PcdFlashAreaBaseAddress); 
  FlashAreaInfo.EndOfFlashUnsignedFvRegionTotalSize  = PcdGet32(PcdEndOfFlashUnsignedFvRegionTotalSize);
  
  printf ("Flash Map Information : \n");
  printf ("  FlashAreaBaseAddress           = 0x%08X\n", FlashAreaInfo.FlashAreaBaseAddress);
  printf ("  FlashAreaSize                  = 0x%08X\n", FlashAreaInfo.FlashAreaSize);  
  printf ("  FlashUnsignedFvRegionBase      = 0x%08X\n", FlashAreaInfo.FlashUnsignedFvRegionBase);
  printf ("  FlashUnsignedFvRegionSize      = 0x%08X\n", FlashAreaInfo.FlashUnsignedFvRegionSize);
  printf ("  EndOfFlashUnsignedFvRegionBase = 0x%08X\n", FlashAreaInfo.EndOfFlashUnsignedFvRegionBase);
  printf ("  EndOfFlashUnsignedFvRegionTotalSize = 0x%08X\n", FlashAreaInfo.EndOfFlashUnsignedFvRegionTotalSize);  

  Status = VerifyFvRegion (FlashAreaInfo);
  if (STATUS_ERROR (Status)) {
    printf ("\n--==Unsigned Fv Region not align, please check XXXBoardPkg/Project.fdf settings==--\n\n");
    return Status;
  }
  
  //
  // Step 2. Get BIOS Binary and Binary Size
  //
  Status = ReadFdFile (FileName, (UINT8 **)&Binary, &BinarySize);
  if (STATUS_ERROR(Status)) {
    printf ("Cannot Read %s file\n", FileName);
    goto DONE;
  }
  
  //
  // Step 3. Restructure BIOS Binary Data
  //
  Status = UpdateFdFile (FlashAreaInfo, (UINT8 **)&Binary, &BinarySize);
  if (STATUS_ERROR(Status)) {
    printf ("Cannot restructure %s file\n", FileName);
    goto DONE;
  }

  //
  // Step 4. Update BIOS Binary file
  //
  Status = WriteFdFile (FileName, (UINT8 **)&Binary, &BinarySize);
  if (STATUS_ERROR(Status)) {
    printf ("Cannot Write %s file\n", FileName);
    goto DONE;
  }

  printf ("\n--== Unsigned FV Generator End ==--\n\n");

DONE:
  if (Binary != NULL) {
    free (Binary);
  }
  
  return Status;
}

