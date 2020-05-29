//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  EmuPeiHelper.c

Abstract:

  Doing necessary preparsions in recovery PEI phase.   
    1. Create EmuPeiMark HOB to identy last PEI is recovery only PEI.
    2. Provide recovery module Ppi to forward last recovery image. 

--*/

#include "Tiano.h"
#include "PeiLib.h"
#include "FastRecoveryData.h"
#include "Flashmap.h"
#include "EmuPeiHelper.h"

#include EFI_GUID_DEFINITION (EmuPeiMark)
#include EFI_PPI_DEFINITION (EmuPeiPpi)


EMU_PEI_HELPER_PRIVATE_DATA mHelperPrivateData;

EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN EFI_PEI_SERVICES                               **PeiServices,
  IN PEI_DEVICE_RECOVERY_MODULE_INTERFACE           *This,
  OUT UINTN                                         *NumberRecoveryCapsules
  )
/*++

Routine Description:

  Recovery module Ppi - Get recovery image count function.

Arguments:

  PeiServices             - Pointer to PEI services.
  This                    - Pointer to Recovery module Ppi statance.
  NumberRecoveryCapsules  - Recovery capsule count.

Returns:

  EFI_SUCCESS             - Operation successfully complete.

--*/
{
  //
  // It is enough to return 1 here.
  //
  *NumberRecoveryCapsules = 1;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                              **PeiServices,
  IN  PEI_DEVICE_RECOVERY_MODULE_INTERFACE          *This,
  IN  UINTN                                         CapsuleInstance,
  OUT UINTN                                         *Size,
  OUT EFI_GUID                                      *CapsuleType
  )
/*++

Routine Description:

  Recovery module Ppi - Get recovery image information

Arguments:

  PeiServices     - Pei services
  This            - Ppi Instance
  CapsuleInstance - Capsule index
  Size            - Size of recovery image
  CapsuleType     - Guid of recovery image type

Returns:

  EFI_SUCCESS     - Operation successfully complete.

--*/
{
  //
  // For original implement does not record image size in HOB, we can only
  // assume its' size equals to FLASH_SIZE. But it may not be true for all
  // situation... 
  //
  *Size       = (UINTN) mHelperPrivateData.RecoveryImageSize;;
  (*PeiServices)->CopyMem (
                    (UINT8 *) CapsuleType,
                    (UINT8 *) &gPeiCapsuleOnFatIdeDiskGuid,
                    sizeof (EFI_GUID)
                    );

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN OUT EFI_PEI_SERVICES                           **PeiServices,
  IN PEI_DEVICE_RECOVERY_MODULE_INTERFACE           *This,
  IN UINTN                                          CapsuleInstance,
  OUT VOID                                          *Buffer
  )
/*++

Routine Description:

  Recovery module Ppi - Load recovery image function.

Arguments:

  PeiServices     - Pei services.
  This            - Ppi instance.
  CapsuleInstance - Capsule index. (Don't care here in this implementation.)
  Buffer          - Caller provide a buffer enough to store the recovery image.

Returns:

  EFI_SUCCESS     - Operation successfully complete.

--*/
{
  EFI_STATUS                        Status;
  EMU_PEI_HELPER_PRIVATE_DATA       *PrivateData;
  EFI_HOB_MEMORY_ALLOCATION_MODULE  *MemoryHob;

  PrivateData = EMI_PEI_HELPER_PRIVATE_DATA_FROM_THIS (This);

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
                            PeiServices,
                            EFI_HOB_TYPE_MEMORY_ALLOCATION,
                            (UINT16) (sizeof (EFI_HOB_MEMORY_ALLOCATION_MODULE)),
                            &MemoryHob
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  MemoryHob->MemoryAllocationHeader.Name = gEfiRecoveryFileAddressGuid;
  MemoryHob->MemoryAllocationHeader.MemoryBaseAddress = (UINTN)Buffer;
  MemoryHob->MemoryAllocationHeader.MemoryLength = PrivateData->RecoveryImageSize;

  return Status;
}

EFI_STATUS
EmuPeiHelperEntry (
  IN EFI_FFS_FILE_HEADER                   *FfsHeader,
  IN EFI_PEI_SERVICES                      **PeiServices
  )
/*++

Routine Description:

  EmuPeiHelper entry point.

Arguments:

  FfsHeader     - Pointer to module Ffs Header. 
  PeiServices   - PEI core services.
  
  
Returns:

  EFI_SUCCESS   - Operation succesfully completed.

--*/
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;
  FAST_RECOVERY_DXE_TO_PEI_DATA     *DxeToPeiData;
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
  ASSERT_PEI_ERROR (PeiServices, Status);
  
  //
  // Install EmuPeiMark GUID HOB
  //
  *((EFI_PHYSICAL_ADDRESS *)EmuPeiGuidHubData) = DxeToPeiData->RomToRamDifference;   
  Status = PeiBuildHobGuidData (
                          PeiServices,
                          &gEmuPeiMarkGuid,
                          (VOID *)EmuPeiGuidHubData,
                          sizeof(EFI_PHYSICAL_ADDRESS)
                          ); 
  ASSERT_PEI_ERROR (PeiServices, Status);

  //
  // If we are in recovery mode, install Device Recovery Ppi to forward previous
  // loaded recovery image.
  //
  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_PEI_ERROR (PeiServices, Status);
  
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    mHelperPrivateData.Signature = EMI_PEI_HELPER_PRIVATE_DATA_SIGNATURE;
    mHelperPrivateData.RecoveryImageAddress = DxeToPeiData->RecoveryImageAddress;
    mHelperPrivateData.RecoveryImageSize = DxeToPeiData->RecoveryImageSize;

    mHelperPrivateData.DeviceRecoveryPpi.GetNumberRecoveryCapsules = GetNumberRecoveryCapsules;
    mHelperPrivateData.DeviceRecoveryPpi.GetRecoveryCapsuleInfo    = GetRecoveryCapsuleInfo;
    mHelperPrivateData.DeviceRecoveryPpi.LoadRecoveryCapsule       = LoadRecoveryCapsule;
  
    mHelperPrivateData.PpiDescriptor.Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    mHelperPrivateData.PpiDescriptor.Guid  = &gPeiDeviceRecoveryModulePpiGuid;
    mHelperPrivateData.PpiDescriptor.Ppi   = &mHelperPrivateData.DeviceRecoveryPpi;
  
    Status = (**PeiServices).InstallPpi (PeiServices, &mHelperPrivateData.PpiDescriptor);
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  return EFI_SUCCESS;
}
