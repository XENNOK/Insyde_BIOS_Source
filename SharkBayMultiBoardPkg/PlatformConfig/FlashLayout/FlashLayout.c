/** @file
  Generation of Flash layout information
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

#include <stdio.h>
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>

#define PRINT_FLASH_LAYOUT_PCD(FdLayoutPcd)                \
          if (PcdGet32(FdLayoutPcd) != 0) {                \
            fprintf(FileIa32, "#define _PCD_VALUE_");                 \
            fprintf(FileX64, "#define _PCD_VALUE_");                 \
            fprintf(FileIa32, #FdLayoutPcd);                          \
            fprintf(FileX64, #FdLayoutPcd);                          \
            fprintf(FileIa32, "  0x%08XU\n", PcdGet32(FdLayoutPcd)); \
            fprintf(FileX64, "  0x%08XU\n", PcdGet32(FdLayoutPcd)); \
          }

/*
  FlashLayout utillity to generate FdLayout.h automatically

  @param[in] Argc               Number of command line arguments
  @param[in] Argv               Array of command line argument strings
  @param[in] Envp               Array of environmemt variable strings


  @retval 0                     Normal exit
  @retval others                Abnormal exit
*/
int
main (
  IN  INTN  Argc,
  IN  CHAR8 **Argv,
  IN  CHAR8 **Envp
  )
{

  FILE *FileIa32 = NULL;
  FILE *FileX64 = NULL;
  CHAR8 Ia32FilePath[256];
  CHAR8 X64FilePath[256];

  AsciiSPrint(Ia32FilePath, 256, "%a/IA32/FdLayout.h", Argv[1]);
  AsciiSPrint(X64FilePath, 256, "%a/X64/FdLayout.h", Argv[1]);
  FileIa32 = fopen(Ia32FilePath, "w");
  if (FileIa32 == NULL) {
    printf("Create file failure!\n");
    return -1;
  }

  FileX64 = fopen(X64FilePath, "w");
  if (FileX64 == NULL) {
    printf("Create file failure!\n");
    fclose(FileIa32);
    return -1;
  }
  PRINT_FLASH_LAYOUT_PCD(PcdFlashAreaBaseAddress);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashAreaSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvMainBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvMainSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashApeiStoreBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashApeiStoreSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashUnsignedFvRegionBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashUnsignedFvRegionSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageDmiBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageDmiSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageMicrocodeBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageMicrocodeSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageGPNVFVBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageGPNVFVSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageMsdmDataBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageMsdmDataSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageBvdtBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageBvdtSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFitBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFitSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageVariableBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageVariableSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFtwWorkingBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFtwWorkingSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFtwSpareBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFtwSpareSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFactoryCopyBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashNvStorageFactoryCopySize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvBackupBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvBackupSize);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvRecoveryBase);
  PRINT_FLASH_LAYOUT_PCD(PcdFlashFvRecoverySize);

  fclose(FileIa32);
  fclose(FileX64);

  return 0;

}
