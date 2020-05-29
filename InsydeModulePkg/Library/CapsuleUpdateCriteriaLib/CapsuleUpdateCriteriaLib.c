/** @file
  Library instance to Capsule Update Criteria check

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
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Guid/EfiSystemResourceTable.h>

#define BVDT_BME_OFFSET     0x12F
#define BVDT_DYNAMIC_OFFSET 0x15B
#define BVDT_SIZE           0x200    //FixedPcdGet32(PcdFlashNvStorageBvdtSize)

UINT8 EsrtSig[] = {'$', 'E', 'S', 'R', 'T'};
UINT8 BvdtSig[] = {'$', 'B', 'V', 'D', 'T'};
UINT8 BmeSig[]  = {'$', 'B', 'M', 'E'};
UINT8 mBvdtBuf[BVDT_SIZE];

/**
  Get system firmware revision for ESRT from capsule image

  @param  CapsuleHeader         Points to a capsule header.

  @return                       The system firmware revision from the capsule image
                                If the signature cannot be found, 0x00000000 will
                                be returned
**/
UINT32
EFIAPI
GetCapsuleSystemFirmwareVersion (
  IN EFI_CAPSULE_HEADER  *CapsuleHeader
  )
{
  UINT8   *Bvdt;
  UINTN   Index;
  BOOLEAN BvdtFound;

  BvdtFound = FALSE;
  Bvdt = NULL;
  for (Index = CapsuleHeader->HeaderSize; Index < CapsuleHeader->CapsuleImageSize; Index++ ) {
    Bvdt = (UINT8 *)CapsuleHeader + Index;
    if ( CompareMem (Bvdt, BvdtSig, sizeof (BvdtSig)) == 0) {
      //
      // $BVDT found, continue to search $BME
      //
      if ( CompareMem (Bvdt + BVDT_BME_OFFSET, BmeSig, sizeof (BmeSig)) == 0) {
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
      if (CompareMem(Bvdt + Index, EsrtSig, sizeof(EsrtSig)) == 0) {
        return  *(UINT32 *)(Bvdt + Index + sizeof(EsrtSig));
      }
    }
  }
  return 0;
}

/**
  AC power source existence check

  @param  CapsuleHeader                      Points to a capsule header.

  @retval TRUE                               Criteria check is successful
  @return others                             Failed to pass the criteria check
**/
BOOLEAN
EFIAPI
AcPowerCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  return TRUE;
}

/**
  Battery power check

  @param  CapsuleHeader                      Points to a capsule header.

  @retval TRUE                               Criteria check is successful
  @return others                             Failed to pass the criteria check
**/
BOOLEAN
EFIAPI
BatteryPowerCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  return TRUE;
}

/**
  Security check

  @param  CapsuleHeader                      Points to a capsule header.

  @retval TRUE                               Criteria check is successful
  @return others                             Failed to pass the criteria check
**/
BOOLEAN
EFIAPI
SecurityCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  return TRUE;
}

/**
  Capsule image integrity check

  @param  CapsuleHeader                      Points to a capsule header.

  @retval TRUE                               Criteria check is successful
  @return others                             Failed to pass the criteria check
**/
BOOLEAN
EFIAPI
IntegrityCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  return TRUE;
}

/**
  Firmware version check

  @param  CapsuleHeader                      Points to a capsule header.

  @retval TRUE                               Criteria check is successful
  @return others                             Failed to pass the criteria check
**/
BOOLEAN
EFIAPI
VersionCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  EFI_STATUS                Status;
  EFI_SYSTEM_RESOURCE_TABLE *Esrt;
  UINTN                     Index;

  Status = EfiGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, (VOID **)&Esrt);
  ASSERT_EFI_ERROR(Status);
  if (Status != EFI_SUCCESS) {
    //
    // return FALSE if ESRT table is not found
    //
    return FALSE;
  }
  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    if (CompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &CapsuleHeader->CapsuleGuid)) {
      if (GetCapsuleSystemFirmwareVersion(CapsuleHeader) < Esrt->FirmwareResources[Index].LowestSupportedFirmwareVersion) {
        return FALSE;
      }
      return TRUE;;
    }
  }
  return FALSE;
}

/**
  Storage check

  @param  CapsuleHeader                      Points to a capsule header.

  @retval TRUE                               Criteria check is successful
  @return others                             Failed to pass the criteria check
**/
BOOLEAN
EFIAPI
StorageCheck (
  EFI_CAPSULE_HEADER *CapsuleHeader
  )
{
  return TRUE;
}


/**
  Pre-installation check for Capsule Update

  @param  CapsuleHeader                      Points to a capsule header.

  @retval ESRT_SUCCESS                       The Capsule passed the pre-installation criteria
  @retval ESRT_ERROR_UNSUCCESSFUL            The pre-installation criteria check failed
  @retval ESRT_ERROR_INSUFFICIENT_RESOURCES  Out of memory or persistent storage
  @retval ESRT_ERROR_INCORRECT_VERSION       Incorrect/incompatible firmware version
  @retval ESRT_ERROR_INVALID_IMAGE_FORMAT    Invalid Capsule image format
  @retval ESRT_ERROR_AUTHENTICATION          Capsule image authentication failed
  @retval ESRT_ERROR_AC_NOT_CONNECTED        The system is not connected to the AC power
  @retval ESRT_ERROR_INSUFFICIENT_BATTERY    The battery capacity is low

**/
ESRT_STATUS
EFIAPI
PreInstallationCheck (
  EFI_CAPSULE_HEADER *Capsule
  )
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

/**
  Post-installation check for Capsule Update

  @param  CapsuleHeader                      Points to a capsule header.

  @retval ESRT_SUCCESS                       The Capsule passed the pre-installation criteria
  @retval ESRT_ERROR_UNSUCCESSFUL            The pre-installation criteria check failed
  @retval ESRT_ERROR_INSUFFICIENT_RESOURCES  Out of memory or persistent storage
  @retval ESRT_ERROR_AUTHENTICATION          Capsule image authentication failed

**/
ESRT_STATUS
EFIAPI
PostInstallationCheck (
  EFI_CAPSULE_HEADER *Capsule
  )
{
  return ESRT_SUCCESS;
}