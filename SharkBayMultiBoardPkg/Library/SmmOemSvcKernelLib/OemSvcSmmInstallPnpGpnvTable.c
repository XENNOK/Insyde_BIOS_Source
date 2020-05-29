/** @file
  GPNV is a persistent general-purpose storage area managed by SMBIOS. 
  The attribute of GPNV is OEM - specific that the format and location of GPNV are not defined in SMBIOS specification.
  This OemServiece provides OEM to define the handle parameter which is passed into GPNV function (55H, 56H, and 57H) to access specific GPNV area. 
  OEM can define accessible GPNV area by modifying the handle value of the UpdateableGpnvs Table.  

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmOemSvcKernelLib.h>
//[-start-140305-IB07390127-add]//
#include <Library/PcdLib.h>
//[-end-140305-IB07390127-add]//

/**
  GPNV is a persistent general-purpose storage area managed by SMBIOS. 
  The attribute of GPNV is OEM - specific that the format and location of GPNV are not defined in SMBIOS specification.
  This OemServiece provides OEM to define the handle parameter which is passed into GPNV function (55H, 56H, and 57H) to access specific GPNV area. 
  OEM can define accessible GPNV area by modifying the handle value of the UpdateableGpnvs Table.  

  @param[out]  *UpdateableGpnvCount  The number of updatable GPNV (General - Purpose -Nonvolatile) handles. 
  @param[out]  **GetOemGPNVMap       Pointer to GetOemGPNVMap Table.

  @retval      EFI_MEDIA_CHANGED     Get Updatable string success.
  @retval      Others                Base on OEM design.
**/
//[-start-140305-IB07390127-modify]//
EFI_STATUS
OemSvcSmmInstallPnpGpnvTable (
  OUT UINTN                                 *UpdateableGpnvCount,
  OUT OEM_GPNV_MAP                          **GetOemGPNVMap
  )
{
  EFI_STATUS                 Status;
  OEM_GPNV_MAP               UpdatableGpnv[4];

  //
  // Step 1. Get the GPNV handle count, size and base address information from Chipset or OEM
  //
  UpdatableGpnv[0].Handle = 0;
  UpdatableGpnv[0].GPNVBaseAddress = PcdGet32 (PcdFlashNvStorageGPNVHANDLE0Base);
  UpdatableGpnv[0].GPNVSize = PcdGet32 (PcdFlashNvStorageGPNVHANDLE0Size);
  UpdatableGpnv[1].Handle = 1;
  UpdatableGpnv[1].GPNVBaseAddress = PcdGet32 (PcdFlashNvStorageGPNVHANDLE1Base);
  UpdatableGpnv[1].GPNVSize = PcdGet32 (PcdFlashNvStorageGPNVHANDLE1Size);
  UpdatableGpnv[2].Handle = 2;
  UpdatableGpnv[2].GPNVBaseAddress = PcdGet32 (PcdFlashNvStorageGPNVHANDLE2Base);
  UpdatableGpnv[2].GPNVSize = PcdGet32 (PcdFlashNvStorageGPNVHANDLE2Size);
  UpdatableGpnv[3].Handle = 3;
  UpdatableGpnv[3].GPNVBaseAddress = PcdGet32 (PcdFlashNvStorageGPNVHANDLE3Base);
  UpdatableGpnv[3].GPNVSize = PcdGet32 (PcdFlashNvStorageGPNVHANDLE3Size);

  //
  // Step 2. return GPNV handle count and relevant information
  //
  *UpdateableGpnvCount = (sizeof(UpdatableGpnv) / sizeof(OEM_GPNV_MAP));
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (UpdatableGpnv), GetOemGPNVMap);
  if (!EFI_ERROR(Status)) {
    CopyMem ((*GetOemGPNVMap), UpdatableGpnv, sizeof (UpdatableGpnv));
  }

  return EFI_MEDIA_CHANGED;
}
//[-end-140305-IB07390127-modify]//
