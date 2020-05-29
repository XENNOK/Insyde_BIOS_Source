/** @file
  Module for creating EFI System Resource Table for firmware udpate

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BvdtLib.h>
#include <Guid/EfiSystemResourceTable.h>


#define DUMMY_GUID \
  { \
    0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} \
  }


EFI_SYSTEM_RESOURCE_TABLE   mEsrtTemplate = {
  1,                           // Firmware Resource Count
  MAX_FIRMWARE_RESOURCES,      // Firmware Resource Maximum
  1,                           // Firmware Resource Entry Version
  {
    {
      DUMMY_GUID,              // Firmware Class, to be updated at boot time
      SYSTEM_FIRMWARE_TYPE,    // Firmware Type
      0,                       // Firmware Version, to be updated at boot time
      0,                       // Firmware Lowest Supported Firmware Version, to be updated at boot time
      0,                       // Capsule Update Flags
      0,                       // Last Attemp Version, to be updated at boot time
      0,                       // Last Attemp Status, to be updated at boot time
    },
  },
  //
  // Add addition device firmware resources below if there is any.
  //
  
};



/**
  EfiSystemResourceTable installation

  @Param  None
  
  @retval EFI_SUCCESS           EFI System Resource Table was successfully installed
  @retval EFI_OUT_OF_RESOURCES  Insufficient memory to allocate ESRT buffer
**/
EFI_STATUS
EFIAPI
InstallEfiSystemResourceTable (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_SYSTEM_RESOURCE_TABLE     *EsrtTable;
  UINT32                        LastAttemptVersion;
  ESRT_STATUS                   LastAttemptStatus;
  UINTN                         Size;
  UINTN                         Index;

  EsrtTable = AllocateRuntimeCopyPool(sizeof(EFI_SYSTEM_RESOURCE_TABLE), &mEsrtTemplate);
  if (EsrtTable == NULL) {
    ASSERT (EsrtTable != NULL);
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Update System Firmware GUID and System Firmware Version from BVDT table
  //
  Status = GetEsrtFirmwareInfo (
             &EsrtTable->FirmwareResources[0].FirmwareClass,
             &EsrtTable->FirmwareResources[0].FirmwareVersion
           );
  ASSERT_EFI_ERROR(Status);
  
  EsrtTable->FirmwareResources[0].LowestSupportedFirmwareVersion = PcdGet32(PcdLowestSupportedFirmwareVersion);
  for (Index = 0; Index < EsrtTable->FirmwareResourceCount; Index++) {

    Size = sizeof(UINT32);
    Status = gRT->GetVariable (
                    L"EsrtLastAttemptVersion",
                    &EsrtTable->FirmwareResources[Index].FirmwareClass,
                    NULL,
                    &Size,
                    &LastAttemptVersion
                  );
    if (Status == EFI_SUCCESS) {
      EsrtTable->FirmwareResources[Index].LastAttemptVersion = LastAttemptVersion;
    }
  
    Size = sizeof(ESRT_STATUS);
    Status = gRT->GetVariable (
                    L"EsrtLastAttemptStatus",
                    &EsrtTable->FirmwareResources[Index].FirmwareClass,
                    NULL,
                    &Size,
                    &LastAttemptStatus
                  );
    if (Status == EFI_SUCCESS) {
      EsrtTable->FirmwareResources[Index].LastAttemptStatus = LastAttemptStatus;
    }
  }

  Status = gBS->InstallConfigurationTable (&gEfiSystemResourceTableGuid, EsrtTable);
  ASSERT_EFI_ERROR (Status);
  return Status;
}