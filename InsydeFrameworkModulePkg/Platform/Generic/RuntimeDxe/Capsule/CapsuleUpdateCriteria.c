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
//;   CapsuleUpdateCriteria.c
//; 
//; Abstract:
//; 
//;   Criteria check routines for capsule update
//;

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include EFI_GUID_DEFINITION(EfiSystemResourceTable)
#include "CapsuleService.h"

#define BVDT_BME_OFFSET     0x12F
#define BVDT_DYNAMIC_OFFSET 0x15B
#define BVDT_SIZE           0x200   

UINT8 EsrtSig[] = {'$', 'E', 'S', 'R', 'T'};
UINT8 BvdtSig[] = {'$', 'B', 'V', 'D', 'T'};
UINT8 BmeSig[]  = {'$', 'B', 'M', 'E'};
UINT8 mBvdtBuf[BVDT_SIZE];

UINT32
EFIAPI
GetCapsuleSystemFirmwareVersion (
  IN EFI_CAPSULE_HEADER  *CapsuleHeader
  )
/*++

Routine Description:
  Get system firmware revision for ESRT from capsule image

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  The system firmware revision from the capsule image.
  If the signature cannot be found, 0x00000000 will be returned

--*/
{
  UINT8   *Bvdt;
  UINTN   Index;
  BOOLEAN BvdtFound;

  BvdtFound = FALSE;
  Bvdt = NULL;
  for (Index = CapsuleHeader->HeaderSize; Index < CapsuleHeader->CapsuleImageSize; Index++ ) {
    Bvdt = (UINT8 *)CapsuleHeader + Index;
    if ( EfiCompareMem (Bvdt, BvdtSig, sizeof (BvdtSig)) == 0) {
      //
      // $BVDT found, continue to search $BME
      //
      if (EfiCompareMem (Bvdt + BVDT_BME_OFFSET, BmeSig, sizeof (BmeSig)) == 0) {
        BvdtFound = TRUE;
        break;
      }
    }
  }

  //
  // Search for "$ESRT" from BVDT dynamic signature start
  //
  if (BvdtFound) {
    for (Index = BVDT_DYNAMIC_OFFSET; Index < BVDT_SIZE; Index++) {
      if (EfiCompareMem(Bvdt + Index, EsrtSig, sizeof(EsrtSig)) == 0) {
        return  *(UINT32 *)(Bvdt + Index + sizeof(EsrtSig));
      }
    }
  }
  return 0;
}

BOOLEAN
EFIAPI
AcPowerCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  Get system firmware revision for ESRT from capsule image

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  TRUE                     - Criteria check is successful
  FALSE                    - Failed to pass the criteria check

--*/
{
  return TRUE;
}

BOOLEAN
EFIAPI
BatteryPowerCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  Battery power check

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  TRUE                     - Criteria check is successful
  FALSE                    - Failed to pass the criteria check

--*/
{
  return TRUE;
}

BOOLEAN
EFIAPI
SecurityCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  Security check

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  TRUE                     - Criteria check is successful
  FALSE                    - Failed to pass the criteria check

--*/
{
  return TRUE;
}

BOOLEAN
EFIAPI
IntegrityCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  Capsule image integrity check

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  TRUE                     - Criteria check is successful
  FALSE                    - Failed to pass the criteria check

--*/
{
  return TRUE;
}

BOOLEAN
EFIAPI
VersionCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  Firmware version check

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  TRUE                     - Criteria check is successful
  FALSE                    - Failed to pass the criteria check

--*/
{
  EFI_STATUS                Status;
  EFI_SYSTEM_RESOURCE_TABLE *Esrt;
  UINTN                     Index;

  Status = EfiLibGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, &Esrt);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    //
    // return FALSE if ESRT table is not found
    //
    return FALSE;
  }
  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    if (EfiCompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &CapsuleHeader->CapsuleGuid)) {
      if (GetCapsuleSystemFirmwareVersion(CapsuleHeader) < Esrt->FirmwareResources[Index].LowestSupportedFirmwareVersion) {
        return FALSE;
      }
      return TRUE;;
    }
  }
  return FALSE;
}

BOOLEAN
EFIAPI
StorageCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  Storage check

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  TRUE                     - Criteria check is successful
  FALSE                    - Failed to pass the criteria check

--*/
{
  return TRUE;
}

ESRT_STATUS
EFIAPI
PreInstallationCheck (
  EFI_CAPSULE_HEADER *Capsule
  )
/*++

Routine Description:
  Pre-installation check for Capsule Update

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  ESRT_SUCCESS                      - The Capsule passed the pre-installation criteria
  ESRT_ERROR_UNSUCCESSFUL           - The pre-installation criteria check failed
  ESRT_ERROR_INSUFFICIENT_RESOURCES - Out of memory or persistent storage
  ESRT_ERROR_INCORRECT_VERSION      - Incorrect/incompatible firmware version
  ESRT_ERROR_INVALID_IMAGE_FORMAT   - Invalid Capsule image format
  ESRT_ERROR_AUTHENTICATION         - Capsule image authentication failed
  ESRT_ERROR_AC_NOT_CONNECTED       - The system is not connected to the AC power
  ESRT_ERROR_INSUFFICIENT_BATTERY   - The battery capacity is low

--*/
{
  if (!BatteryPowerCheck(Capsule)) {
    return ESRT_ERROR_INSUFFICIENT_BATTERY;
  }
  if (!AcPowerCheck(Capsule)) {
    return ESRT_ERROR_AC_NOT_CONNECTED;
  }
  if (!StorageCheck(Capsule)) {
    return ESRT_ERROR_INSUFFICIENT_RESOURCES;
  }
  if (!IntegrityCheck(Capsule)) {
    return ESRT_ERROR_INVALID_IMAGE_FORMAT;
  }
  if (!VersionCheck(Capsule)) {
    return ESRT_ERROR_INCORRECT_VERSION;
  }
  if (!SecurityCheck(Capsule)) {
    return ESRT_ERROR_AUTHENTICATION;
  }

  return ESRT_SUCCESS;
}

ESRT_STATUS
EFIAPI
PostInstallationCheck (
  EFI_CAPSULE_HEADER *Capsule
  )
/*++

Routine Description:
  Post-installation check for Capsule Update

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  ESRT_SUCCESS                      - The Capsule passed the post-installation criteria

--*/
{
  return ESRT_SUCCESS;
}