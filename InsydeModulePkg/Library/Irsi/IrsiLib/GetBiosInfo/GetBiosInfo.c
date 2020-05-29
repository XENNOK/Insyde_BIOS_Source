/** @file
  Library Instance implementation for IRSI Get BIOS Information

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

#include <Uefi.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/IrsiLib.h>
#include <Library/PcdLib.h>

/**
  Irsi Get BIOS Information function

  This routine reports firmware size to the caller.

  @param BiosInfBuf     pointer to IRSI_GET_BIOS_INFO structure
  @param BiosInfSize    the size of BiosInfBuf, normally it is equal to
                            sizeof(IRSI_GET_BIOS_INFO)

  @return EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
IrsiGetBiosInfo (
  IN OUT  VOID     *BiosInfBuf
  )
{
   IRSI_GET_BIOS_INFO *BiosInfo;

   BiosInfo = (IRSI_GET_BIOS_INFO *)BiosInfBuf;
   BiosInfo->Header.StructureSize = sizeof (IRSI_GET_BIOS_INFO);

   BiosInfo->BiosSize = PcdGet32 (PcdFlashAreaSize);
   BiosInfo->Header.ReturnStatus = IRSI_STATUS(EFI_SUCCESS);

   return EFI_SUCCESS;
}

/**
  Irsi Get BIOS Information Initialization

  This routine is a LibraryClass constructor for IrsiGetBiosInfo, it will
  register IrsiGetBiosInfo function to the Irsi function database

  @param ImageHandle     A handle for the image that is initializing this driver
  @param SystemTable     A pointer to the EFI system table

  @retval EFI_SUCCESS:   Module initialized successfully
  @retval Others     :   Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiGetBiosInfoInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_GET_BIOS_INFO_COMMAND,
               IrsiGetBiosInfo
               );
  return Status;
}