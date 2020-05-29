/** @file
  Library Instance implementation for IRSI Get Platform Information

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DebugLib.h>
#include <Library/IrsiLib.h>
#include <Library/BvdtLib.h>


CHAR16          mModelName[MODEL_NAME_LEN];
CHAR16          mModelVersion[MODEL_VERSION_LEN];


/**
  Irsi Get Platform Information function

  This routine reports platform information such as model name and model version

  @param PlatformInfoBuf        pointer to IRSI_GET_PLATFORM_INFO structure
  @param PlatformInfoSize       the size of PlatformInfoBuf, normally it is equal to
                                sizeof(IRSI_GET_PLATFORM_INFO)

  @return EFI_SUCCESS           IRSI Platform Information function called successfully
**/
EFI_STATUS
EFIAPI
IrsiGetPlatformInfo (
  VOID     *PlatformInfoBuf
  )
{
   IRSI_GET_PLATFORM_INFO  *PlatformInfo;

   PlatformInfo = (IRSI_GET_PLATFORM_INFO *)PlatformInfoBuf;

   PlatformInfo->Header.StructureSize = sizeof (IRSI_GET_PLATFORM_INFO);
   PlatformInfo->Header.ReturnStatus = IRSI_STATUS(EFI_SUCCESS);
   
   CopyMem(PlatformInfo->ModelVersion, mModelVersion, MODEL_VERSION_LEN * sizeof(CHAR16));
   CopyMem(PlatformInfo->ModelName, mModelName, MODEL_NAME_LEN * sizeof(CHAR16));
   
   PlatformInfo->Header.ReturnStatus = IRSI_STATUS(EFI_SUCCESS);

   return EFI_SUCCESS;
}

/**
  Irsi Get Platform Information Initialization

  This routine is a LibraryClass constructor for IrsiGetPlatformInfo, it will
  register IrsiGetPlatformInfo function to the Irsi function database

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully
  @retval Others                Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiGetPlatformInfoInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  UINTN                          BufSize;
  
   BufSize = MODEL_NAME_LEN * sizeof(CHAR16);
   Status = GetBvdtInfo (BvdtBiosVer, &BufSize, mModelVersion);
   ASSERT (Status == EFI_SUCCESS);
   BufSize = MODEL_NAME_LEN * sizeof(CHAR16);
   Status = GetBvdtInfo (BvdtProductName, &BufSize, mModelName);
   ASSERT (Status == EFI_SUCCESS);

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_GET_PLATFORM_INFO_COMMAND,
               IrsiGetPlatformInfo
               );

  return Status;
}