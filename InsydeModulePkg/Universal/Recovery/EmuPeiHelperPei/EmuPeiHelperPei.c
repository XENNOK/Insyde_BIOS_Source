/** @file

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

/*++

Module Name:

  EmuPeiHelperPei.c

Abstract:

  Doing necessary preparsions in recovery PEI phase.   
    1. Create EmuPeiMark HOB to identy last PEI is recovery only PEI.
    2. Provide recovery module Ppi to forward last recovery image. 

--*/

#include "EmuPeiHelperPei.h"



EMU_PEI_HELPER_PRIVATE_DATA mHelperPrivateData = {
  EFI_PEI_HELPER_PRIVATE_DATA_SIGNATURE,                // Signature
  {                                                     // DeviceRecoveryPpi
    GetNumberRecoveryCapsules,
    GetRecoveryCapsuleInfo,
    LoadRecoveryCapsule,
  }
  , 
  {                                                     // PpiDescriptor
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiDeviceRecoveryModulePpiGuid,
    &mHelperPrivateData.DeviceRecoveryPpi
  }
  ,
  0,                                                    // RecoveryImageAddress, To be filled at run time
  0                                                     // RecoveryImageSize, To be filled at run time
};

/**

  Recovery module Ppi - Get recovery image count function.

  @param PeiServices             Pointer to PEI services.
  @param This                    Pointer to Recovery module Ppi statance.
  @param NumberRecoveryCapsules  Recovery capsule count.

  @retval EFI_SUCCESS             Operation successfully complete.

**/
EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN EFI_PEI_SERVICES                               **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI             *This,
  OUT UINTN                                         *NumberRecoveryCapsules
  )
{
  //
  // It is enough to return 1 here.
  //
  *NumberRecoveryCapsules = 1;
  return EFI_SUCCESS;
}

/**

  Recovery module Ppi - Get recovery image information

  @param PeiServices     Pei services
  @param This            Ppi Instance
  @param CapsuleInstance Capsule index
  @param Size            Size of recovery image
  @param CapsuleType     Guid of recovery image type

  @retval EFI_SUCCESS    Operation successfully complete.

**/
EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                              **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI            *This,
  IN  UINTN                                         CapsuleInstance,
  OUT UINTN                                         *Size,
  OUT EFI_GUID                                      *CapsuleType
  )
{
  //
  // For original implement does not record image size in HOB, we can only
  // assume its' size equals to FLASH_SIZE. But it may not be true for all
  // situation... 
  //
  *Size       = (UINTN)mHelperPrivateData.RecoveryImageSize;
  (*PeiServices)->CopyMem (
                    (UINT8 *) CapsuleType,
                    (UINT8 *) &gRecoveryOnFatIdeDiskGuid,
                    sizeof (EFI_GUID)
                    );

  return EFI_SUCCESS;
}

/**

  Recovery module Ppi - Load recovery image function.

  @param PeiServices     Pei services.
  @param This            Ppi instance.
  @param CapsuleInstance Capsule index. (Don't care here in this implementation.)
  @param Buffer          Caller provide a buffer enough to store the recovery image.

  @retval EFI_SUCCESS    Operation successfully complete.

**/
EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN OUT EFI_PEI_SERVICES                           **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI             *This,
  IN UINTN                                          CapsuleInstance,
  OUT VOID                                          *Buffer
  )
{
  EFI_STATUS                        Status;
  EMU_PEI_HELPER_PRIVATE_DATA       *PrivateData;
  EFI_HOB_MEMORY_ALLOCATION_MODULE  *MemoryHob;

  PrivateData = EMU_PEI_HELPER_PRIVATE_DATA_FROM_THIS (This);

  //
  // Copy back original data to caller
  //
  (*PeiServices)->CopyMem (
                    Buffer,
                    (VOID *)(UINTN)PrivateData->RecoveryImageAddress,
                    (UINTN)PrivateData->RecoveryImageSize
                    );

  //
  // Create a HOB for DXE
  //
  Status = (*PeiServices)->CreateHob (
                            (CONST EFI_PEI_SERVICES **)PeiServices,
                            EFI_HOB_TYPE_MEMORY_ALLOCATION,
                            (UINT16) (sizeof (EFI_HOB_MEMORY_ALLOCATION_MODULE)),
                            (VOID **)&MemoryHob
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  MemoryHob->MemoryAllocationHeader.Name = gEfiRecoveryFileAddressGuid;
  MemoryHob->MemoryAllocationHeader.MemoryBaseAddress = (UINTN)Buffer;
  MemoryHob->MemoryAllocationHeader.MemoryLength = PrivateData->RecoveryImageSize;

  return Status;
}

/**

  EmuPeiHelper entry point.

  @param FfsHeader     Pointer to module Ffs Header. 
  @param PeiServices   PEI core services.

  @retval EFI_SUCCESS  Operation succesfully completed.

**/
EFI_STATUS
EmuPeiHelperEntry (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;
  FAST_RECOVERY_DXE_TO_PEI_DATA     *DxeToPeiData;
  VOID                              *HobData;
  UINT8                             EmuPeiGuidHubData[sizeof(EFI_PHYSICAL_ADDRESS)];
  
  //
  // Try to locate EmuPeiMark PPI for checking if it is in recovery only PEI
  // phase.
  //
  Status = (*PeiServices)->LocatePpi (
                                  PeiServices, 
                                  &gEmuPeiPpiGuid, 
                                  0,
                                  NULL,
                                  (VOID **)&DxeToPeiData
                                  );
  if (Status == EFI_NOT_FOUND) {
    return EFI_SUCCESS;
  }
  ASSERT_EFI_ERROR (Status);
  
  //
  // Install EmuPeiMark GUID HOB
  //
  *((EFI_PHYSICAL_ADDRESS *)EmuPeiGuidHubData) = DxeToPeiData->RomToRamDifference;
  HobData = BuildGuidDataHob (
                          &gEmuPeiMarkGuid,  
                          (VOID *)EmuPeiGuidHubData,
                          sizeof(EFI_PHYSICAL_ADDRESS)
                          );
  ASSERT (HobData != NULL);                          

  //
  // If we are in recovery mode, install Device Recovery Ppi to forward previous
  // loaded recovery image.
  //
  BootMode = GetBootModeHob ();
  
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    mHelperPrivateData.RecoveryImageAddress = DxeToPeiData->RecoveryImageAddress;
    mHelperPrivateData.RecoveryImageSize = DxeToPeiData->RecoveryImageSize;
    Status = (**PeiServices).InstallPpi (PeiServices, &mHelperPrivateData.PpiDescriptor);
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  return EFI_SUCCESS;
}
