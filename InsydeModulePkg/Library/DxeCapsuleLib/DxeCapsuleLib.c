/** @file
  Capsule Library instance to update capsule image to flash.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** 
  Capsule Library instance to update capsule image to flash.

  Copyright (c) 2007 - 2010, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <PiDxe.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CapsuleLib.h>
#include <Library/CapsuleUpdateCriteriaLib.h>
#include <Guid/EfiSystemResourceTable.h>
#include "DxeCapsuleLib.h"

EFI_GUID mSupportedCapsuleGuidList[] = {
   WINDOWS_UX_CAPSULE_GUID
};

/**
  Those capsules supported by the firmwares.

  @param  CapsuleHeader             Points to a capsule header.

  @retval EFI_SUCESS                Input capsule is supported by firmware.
  @retval EFI_INCOMPATIBLE_VERSION  Incompatible firmware version.
  @retval EFI_UNSUPPORTED           Input capsule is not supported by the firmware.
**/
EFI_STATUS
EFIAPI
SupportCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_STATUS                 Status;
  EFI_SYSTEM_RESOURCE_TABLE  *Esrt;
  UINTN                      Index;
  
  for (Index = 0; Index < sizeof(mSupportedCapsuleGuidList)/sizeof(EFI_GUID); Index++) {
    if (CompareGuid(&CapsuleHeader->CapsuleGuid, &mSupportedCapsuleGuidList[Index])) {
      return EFI_SUCCESS;
    }
  }
  
  Status = EfiGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, (VOID **)&Esrt);
  ASSERT_EFI_ERROR(Status);
  if (Status == EFI_SUCCESS) {
    for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
      if (CompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &CapsuleHeader->CapsuleGuid)) {
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_UNSUPPORTED;
}


/**
  The firmware implements to process the capsule image.

  @param  CapsuleHeader         Points to a capsule header.

  @retval EFI_SUCESS            Process Capsule Image successfully.
  @retval EFI_UNSUPPORTED       Capsule image is not supported by the firmware.
  @retval EFI_VOLUME_CORRUPTED  FV volume in the capsule is corrupted.
  @retval EFI_OUT_OF_RESOURCES  Not enough memory.
**/
EFI_STATUS
EFIAPI
ProcessCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_STATUS                  Status;
  EFI_SYSTEM_RESOURCE_TABLE   *Esrt;
  UINTN                       Index;
  ESRT_STATUS                 EsrtStatus;
  UINT32                      AttemptVersion;

  Status     = EFI_SUCCESS;
  EsrtStatus = ESRT_SUCCESS;

  if (SupportCapsuleImage (CapsuleHeader) != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }
  if (CompareGuid(&CapsuleHeader->CapsuleGuid, &gWindowsUxCapsuleGuid)) {
    return PersistCapsuleImage (CapsuleHeader);
  }

  Status = EfiGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, (VOID **)&Esrt);
  ASSERT_EFI_ERROR(Status);
  if (Status != EFI_SUCCESS) {
    //
    // return EFI_UNSUPPORTED if ESRT table is not found
    //
    return EFI_UNSUPPORTED;
  }
  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    if (CompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &CapsuleHeader->CapsuleGuid)) {
      AttemptVersion = GetCapsuleSystemFirmwareVersion(CapsuleHeader);
      gRT->SetVariable (
             L"EsrtLastAttemptVersion",
             &CapsuleHeader->CapsuleGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             sizeof(UINT32),
             &AttemptVersion
           );
      EsrtStatus = PreInstallationCheck(CapsuleHeader);
      if (EsrtStatus != ESRT_SUCCESS) {
        switch (EsrtStatus) {
          case ESRT_ERROR_INSUFFICIENT_RESOURCES:
            Status = EFI_OUT_OF_RESOURCES;
            break;
          case ESRT_ERROR_INVALID_IMAGE_FORMAT:
            Status = EFI_LOAD_ERROR;
            break;
          case ESRT_ERROR_INCORRECT_VERSION:
            Status = EFI_INCOMPATIBLE_VERSION;
            break;
          case ESRT_ERROR_AC_NOT_CONNECTED:
          case ESRT_ERROR_INSUFFICIENT_BATTERY:
            Status = EFI_DEVICE_ERROR;
            break;
          case ESRT_ERROR_AUTHENTICATION:
            Status = EFI_SECURITY_VIOLATION;
            break;
          default:
            Status = EFI_UNSUPPORTED;
            break;
        }
        gRT->SetVariable (
           L"EsrtLastAttemptStatus",
           &CapsuleHeader->CapsuleGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           sizeof(UINT32),
           &EsrtStatus
         );

        if (EsrtStatus != ESRT_SUCCESS) {
          //
          // System reset is required for updating the ESRT table
          //
          gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
        }
      }
      break;
    }
  }
  if (Index >= (UINTN)Esrt->FirmwareResourceCount) {
    return EFI_UNSUPPORTED;
  }
  if (Status == EFI_SUCCESS) {
    Status = PersistCapsuleImage (CapsuleHeader);
    if (EFI_ERROR(Status)) {
      EsrtStatus = ESRT_ERROR_UNSUCCESSFUL;
    }
  }

  if (EsrtStatus != ESRT_SUCCESS) {
    gRT->SetVariable (
           L"EsrtLastAttemptStatus",
           &CapsuleHeader->CapsuleGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           sizeof(UINT32),
           &EsrtStatus
         );
   }

  return Status;
}




