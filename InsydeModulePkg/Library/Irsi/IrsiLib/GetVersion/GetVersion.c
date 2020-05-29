/** @file
  Library Instance implementation for IRSI Get Version Information

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
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/IrsiLib.h>
#include <Library/PcdLib.h>


/**
  Irsi Get Version function

  This routine reports Irsi version

  @param VersionBuf     pointer to IRSI_GET_VERSION structure
  @param VersionSize    the size of VersionBuf, normally it is equal to
                            sizeof(IRSI_GET_VERSION)

  @return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
IrsiGetVersion (
  VOID     *VersionBuf
  )
{
   IRSI_GET_VERSION *Version;
   Version = (IRSI_GET_VERSION *)VersionBuf;
   Version->Header.StructureSize = sizeof(IRSI_GET_VERSION);

   Version->SpecVersion = PcdGet64(PcdIrsiVersion);
   Version->Header.ReturnStatus = IRSI_STATUS(EFI_SUCCESS);
   
   return EFI_SUCCESS;
}

/**
  Irsi Get Version Initialization

  This routine is a LibraryClass constructor for IrsiGetVersion, it will
  register IrsiGetVersion function to the Irsi function database

  @param ImageHandle     A handle for the image that is initializing this driver
  @param SystemTable     A pointer to the EFI system table

  @retval EFI_SUCCESS:   Module initialized successfully
  @retval Others     :   Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiGetVersionInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_GET_VERSION_COMMAND,
               IrsiGetVersion
               );
               
  return Status;
}