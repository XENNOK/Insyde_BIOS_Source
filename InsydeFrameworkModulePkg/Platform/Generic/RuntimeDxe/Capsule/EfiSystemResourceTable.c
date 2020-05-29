//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//;   EfiSystemResourceTable.c
//; 
//; Abstract:
//; 
//;   EFI System Resource Table 
//;

#include "CapsuleService.h"
#include "BvdtLib.h"

#define LOWEST_SUPPORT_FIRMWARE_VERSION 0x32440000    // 3.72.44.0000
#define BVDT_SIZE                       0x200

#define ESRT_TAG                        {'$','E','S','R','T'}


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
      LOWEST_SUPPORT_FIRMWARE_VERSION,  // Firmware Lowest Supported Firmware Version, to be updated at boot time
      0,                       // Capsule Update Flags
      0,                       // Last Attemp Version, to be updated at boot time
      0,                       // Last Attemp Status, to be updated at boot time
    },
  },
  //
  // Add addition device firmware resources below if there is any.
  //
  
};

EFI_STATUS
EFIAPI
GetEsrtFirmwareInfo (
  OUT EFI_GUID          *FirmwareGuid,
  OUT UINT32            *FirmwareVersion
  )
/*++

Routine Description:
  Get ESRT System Firmware GUID and Version information from BVDT $ESRT tag

Arguments:
  FirmwareGuid            - Pointer to the system firmware version GUID
  FirmwareVersion         - Pointer to the system firmware version 
  
Returns:
  EFI_SUCCESS             - System firmware GUID and system firmware version are 
                            successfully retrieved
  EFI_NOT_FOUND           - Unable to find system firmware GUID or system firmware 
                            version in the BVDT table 

--*/
{
  UINTN Index;
  UINT8  *Bvdt;
  UINT8  *EsrtTagPtr;
  UINT8  EsrtTag[] = ESRT_TAG;
  EFI_STATUS Status;
  
  if (FirmwareGuid == NULL || FirmwareVersion == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Bvdt = (UINT8 *)(UINTN)(BVDT_BASE_ADDRESS);
  //
  // Search for "$ESRT" from BVDT dynamic signature start
  //
  Status = EFI_NOT_FOUND;
  for (Index = MULTI_BIOS_VERSION_OFFSET; Index < BVDT_SIZE; Index++) {
    if (EfiCompareMem(Bvdt + Index, EsrtTag, sizeof(EsrtTag)) == 0) {
      EsrtTagPtr = Bvdt + Index;
      *FirmwareVersion = *(UINT32 *)(EsrtTagPtr + sizeof(EsrtTag));
      *FirmwareGuid =  *(EFI_GUID *)(EsrtTagPtr + sizeof(EsrtTag) + sizeof(UINT32));   
      Status = EFI_SUCCESS;        
      break;
    }  
  }
  
  return Status;
}

EFI_STATUS
EFIAPI
InstallEfiSystemResourceTable (
  VOID
  )
/*++

Routine Description:
  EfiSystemResourceTable installation

Arguments:
  None
  
Returns:
  EFI_SUCCESS             - EFI System Resource Table was successfully installed                          
  EFI_OUT_OF_RESOURCES    - Insufficient memory to allocate ESRT buffer

--*/
{
  EFI_STATUS                    Status;
  EFI_SYSTEM_RESOURCE_TABLE     *EsrtTable;
  UINT32                        LastAttemptVersion;
  ESRT_STATUS                   LastAttemptStatus;
  UINTN                         Size;
  UINTN                         Index;

  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  sizeof(EFI_SYSTEM_RESOURCE_TABLE),
                  (VOID **)&EsrtTable
                  );
                  
  EfiCopyMem(EsrtTable, &mEsrtTemplate, sizeof(EFI_SYSTEM_RESOURCE_TABLE));                
  
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
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
  
  
  for (Index = 0; Index < EsrtTable->FirmwareResourceCount; Index++) {
      
    Size = sizeof(UINT32);
    Status = EfiGetVariable (
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
    Status = EfiGetVariable (
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