/** @file
  Provide BIOS region lock protocol to protect BIOS region

;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <PchAccess.h>
#include <PchRegs.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include "BiosRegionLockInfo.h"


/**
 Entry point for Bios Region Lock driver. Install BIOS region lock protocol to protect BIOS region.

 @param[in]         ImageHandle         Image handle of this driver.
 @param[in]         SystemTable         Global system service table.

 @retval EFI_SUCCESS Initialization complete.
*/
EFI_STATUS
EFIAPI
BiosRegionLockInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_HANDLE                            Handle;
  EFI_STATUS                            Status;
  BIOS_REGION_LOCK_INSTANCE            *BiosRegionLockInstance;
  BIOS_REGION_LOCK_PROTOCOL            *BiosRegionLock;
  
  if (PchMmRcrb16 (R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) {
    DEBUG ((EFI_D_ERROR, "SPI has been locked - Access Denied!\n"));
    return EFI_DEVICE_ERROR;
  }

  BiosRegionLockInstance = AllocateZeroPool (sizeof (BIOS_REGION_LOCK_INSTANCE));
  if (BiosRegionLockInstance == NULL) {
    DEBUG ((EFI_D_ERROR, "Allocate Pool Failure!\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  BiosRegionLock = &BiosRegionLockInstance->BiosRegionLock;
  BiosRegionLock->SetRegionByType      = SetRegionByType;
  BiosRegionLock->SetRegionByAddress   = SetRegionByAddress;
  BiosRegionLock->ClearRegionByType    = ClearRegionByType;
  BiosRegionLock->ClearRegionByAddress = ClearRegionByAddress;
  BiosRegionLock->Lock                 = Lock;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiBiosRegionLockProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  BiosRegionLock
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Install EfiBiosRegionLockProtocol Failure!\n"));
    gBS->FreePool (BiosRegionLockInstance);
  }
#ifdef EFI_DEBUG
  else {
    DEBUG ((EFI_D_ERROR, "Install EfiBiosRegionLockProtocol Success!\n"));
  }
#endif

  return Status;
}
