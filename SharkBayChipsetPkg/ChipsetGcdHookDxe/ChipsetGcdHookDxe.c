/** @file
 ChipsetGcdHookDxe module implement code.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include "ChipsetGcdHookDxe.h"

EFI_ALLOCATE_MEMORY_SPACE       mGcdAllocateMemorySpace;


/**
  Allocates nonexistent memory, reserved memory, system memory, or memorymapped
  I/O resources from the global coherency domain of the processor.
  This chipset module hooks original GCD's function. When dynamic TOLUD function
  is enabled, change all MMIO allocation method to top down.

  @param  GcdAllocateType        The type of allocate operation
  @param  GcdMemoryType          The desired memory type
  @param  Alignment              Align with 2^Alignment
  @param  Length                 Length to allocate
  @param  BaseAddress            Base address to allocate
  @param  ImageHandle            The image handle consume the allocated space.
  @param  DeviceHandle           The device handle consume the allocated space.

  @retval EFI_INVALID_PARAMETER  Invalid parameter.
  @retval EFI_NOT_FOUND          No descriptor contains the desired space.
  @retval EFI_SUCCESS            Memory space successfully allocated.

**/
EFI_STATUS
EFIAPI
ChipsetAllocateMemorySpace (
  IN     EFI_GCD_ALLOCATE_TYPE  GcdAllocateType,
  IN     EFI_GCD_MEMORY_TYPE    GcdMemoryType,
  IN     UINTN                  Alignment,
  IN     UINT64                 Length,
  IN OUT EFI_PHYSICAL_ADDRESS   *BaseAddress,
  IN     EFI_HANDLE             ImageHandle,
  IN     EFI_HANDLE             DeviceHandle OPTIONAL
  )
{

  if (GcdMemoryType == EfiGcdMemoryTypeMemoryMappedIo) {
    if (GcdAllocateType == EfiGcdAllocateAnySearchBottomUp ||
        GcdAllocateType == EfiGcdAllocateMaxAddressSearchBottomUp) {
      GcdAllocateType = EfiGcdAllocateMaxAddressSearchTopDown;
      *BaseAddress = PcdGet64 (PcdPciExpressBaseAddress);
    }
  }

  return mGcdAllocateMemorySpace (
           GcdAllocateType,
           GcdMemoryType,
           Alignment,
           Length,
           BaseAddress,
           ImageHandle,
           DeviceHandle
           );
  
}


/**
 Entrypoint of this module.

 This function is the entry point of this module.
 This module will hook GCD's AllocateMemorySpace function if dynamic TOLUD function
 is enabled. If dynamic TOLUD function is disabled, this module will do nothing
 and will unload itself.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_SUCCESS                    Module is loaded successfully.
 @retval EFI_UNSUPPORTED                Module is no necessary loaded.
 
*/
EFI_STATUS
EFIAPI
ChipsetGcdHookDxeEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{

  //
  // Only hook when enabling dynamic TOLUD function
  //
  if (!PcdGetBool (PcdDynamicTolud)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Replace original function
  //
  mGcdAllocateMemorySpace = gDS->AllocateMemorySpace;
  gDS->AllocateMemorySpace = ChipsetAllocateMemorySpace;

  return EFI_SUCCESS;

}

