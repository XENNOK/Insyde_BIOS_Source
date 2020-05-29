/** @file
 Multi Config Tool implement code, ProjectPostBuild.bat will call this function.

 Multi Config Tool use to init Multi Config Region in FD (BIOS Image) file or
 copy Multi Config Region from other FD (BIOS Image) file.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "GenMultiConfigRegion.h"

UINT8
InitMultiConfigRegion (
  IN OUT UINT8                **Binary
  )
{
  MULTI_CONFIG_REGION_HEADER  *MultiConfigHdr;

  MultiConfigHdr = (MULTI_CONFIG_REGION_HEADER*)(*Binary);

  //
  // Initialize the Multi Config Region Header Information
  //
  MultiConfigHdr->TotalRegionSize = PcdGet32 (PcdFlashNvStorageMultiConfigSize);
  MultiConfigHdr->ConfigCount = 0;
  MultiConfigHdr->HeaderLength = (UINT16)(sizeof (MULTI_CONFIG_REGION_HEADER));
  MultiConfigHdr->SetupStructSize = PcdGet32 (PcdSetupConfigSize);
  MultiConfigHdr->SkuId = 0;
  MultiConfigHdr->Reserved = 0;
  MultiConfigHdr->HeaderCrc32 = 0;
  CalculateCrc32 (
    MultiConfigHdr,
    MultiConfigHdr->HeaderLength,
    &MultiConfigHdr->HeaderCrc32
    );

  return STATUS_SUCCESS;
}


/**
 Main entry point for Multi Config Tool program.

 @param[in]   Argc      Number of command line arguments.
 @param[on]   Argv      Array of command line argument strings.
 @param[in]   Envp      Array of environmemt variable strings.

 @retval 0       Normal exit.
 @return others  Abnormal exit.
*/
int
main (
  IN  INTN              Argc,
  IN  CHAR8             **Argv,
  IN  CHAR8             **Envp
  )
{
  UINT8                       Status;
  UINT8                       RegionHDRBuf[32];
  CHAR8                       *MCFileName;
  FILE                        *File;
  MULTI_CONFIG_REGION_HEADER  *MultiConfigHdr;

  Status = STATUS_SUCCESS;

  if (!FeaturePcdGet(PcdMultiConfigSupported)) {
    return 0;
  }

  if (Argc < 2) {
    fprintf (stderr, "GenMultiConfigRegion MultiConfigFile\n");
    return -1;
  }

  MCFileName = Argv[1];

  ZeroMem(RegionHDRBuf, sizeof(RegionHDRBuf));
  //
  // Init Multi Config Region.
  //
  MultiConfigHdr = (MULTI_CONFIG_REGION_HEADER*)(RegionHDRBuf);
  //
  // Initialize the Multi Config Region Header Information
  //
  MultiConfigHdr->Signature = SIGNATURE_64 ('$', 'M', 'U', 'L', 'T', 'C', 'F', 'G');
  MultiConfigHdr->TotalRegionSize = PcdGet32 (PcdFlashNvStorageMultiConfigSize);
  MultiConfigHdr->ConfigCount = 0;
  MultiConfigHdr->HeaderLength = (UINT16)(sizeof (MULTI_CONFIG_REGION_HEADER));
  MultiConfigHdr->SetupStructSize = PcdGet32 (PcdSetupConfigSize);
  MultiConfigHdr->SkuId = 0;
  MultiConfigHdr->Reserved = 0;
  MultiConfigHdr->HeaderCrc32 = 0;
  CalculateCrc32 (
    MultiConfigHdr,
    MultiConfigHdr->HeaderLength,
    &MultiConfigHdr->HeaderCrc32
    );

  //
  // Write back to destination file
  //
  File = NULL;
  fopen_s(&File, MCFileName, "wb");
  if (File == NULL) {
    fprintf (stderr, "\nGenMultiConfigRegion error: Cannot create %s file!\n", MCFileName);
    return -1;
  }

  fwrite (RegionHDRBuf, 1, sizeof(RegionHDRBuf), File);

  fclose(File);

  return Status;

}

