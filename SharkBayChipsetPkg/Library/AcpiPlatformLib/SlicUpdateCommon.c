/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/AcpiPlatformCommon.h>


#define SLP20_PUBLIC_KEY_ROM_FILE_GUID \
  {0x1a1e2341, 0xa2fb, 0x42c7, 0x8d, 0x17, 0x30, 0x73, 0xd0, 0x8e, 0xb2, 0x1d}

#define SLP20_MARKER_ROM_FILE_GUID \
  {0xdd6569a7, 0xe455, 0x4ee5, 0xb2, 0xba, 0xec, 0xda, 0x84, 0xac, 0xbc, 0x99}

/**
 Update Acpi SLIC table with values stored in Rom.
 
 @param[in]   AcpiSupport    protocol instance.
 
 @retval  EFI_SUCCESS        If update successful.
*/
EFI_STATUS
SlicUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN OUT EFI_ACPI_TABLE_VERSION       *Version
  )
{
  EFI_STATUS                            Status;
  EFI_GUID                              PubkeyRomGuid = SLP20_PUBLIC_KEY_ROM_FILE_GUID;
  EFI_GUID                              MarkerRomGuid = SLP20_MARKER_ROM_FILE_GUID;
  VOID                                  *PubkeyRomImage = NULL;
  UINTN                                 PubkeyRomSize = 0;
  VOID                                  *MarkerRomImage = NULL;
  UINTN                                 MarkerRomSize = 0;
  EFI_ACPI_SOFTWARE_LICENSING_TABLE     *SlicPointer;
//[-start-130911-IB05670199-remove]//
//  OEM_SERVICES_PROTOCOL                 *OemServices = NULL;
//[-end-130911-IB05670199-remove]//
  BOOLEAN                               OemUpdatedPublickey = FALSE;
  BOOLEAN                               OemUpdatedMarker    = FALSE;

  if (*(UINT32*)Table == EFI_ACPI_SLIC_TABLE_SIGNATURE) {
    SlicPointer = (EFI_ACPI_SOFTWARE_LICENSING_TABLE *)Table;
  } else {
  	
    return EFI_UNSUPPORTED;
  }
  
//[-start-110429-IB02430193-add]//
  //
  // Fill the Public Key and Marker to 0xFF for the non-Windows OS
  //
  SetMem ((VOID*) (UINTN) &SlicPointer->PublicKey, 
          (UINTN) sizeof (EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE), 
          0xFF
          );
  SetMem ((VOID*) (UINTN) &SlicPointer->SlpMarker, 
          (UINTN) sizeof (EFI_ACPI_SLP_MARKER_STRUCTURE), 
          0xFF
          );
//  EfiCommonLibSetMem ((VOID*) (UINTN) &SlicPointer->PublicKey, 
//                              (UINTN) sizeof (EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE), 
//                              0xFF
//                             );
//  EfiCommonLibSetMem ((VOID*) (UINTN) &SlicPointer->SlpMarker, 
//                              (UINTN) sizeof (EFI_ACPI_SLP_MARKER_STRUCTURE), 
//                              0xFF
//                             );  
//[-end-110429-IB02430193-add]//
  
//[-start-130911-IB05670199-modify]//
   Status = OemSvcGetSlp20PubkeyAndMarkerRom (
               &SlicPointer->PublicKey,
               &OemUpdatedPublickey,
               &SlicPointer->SlpMarker,
               &OemUpdatedMarker
               );
    if (Status == EFI_SUCCESS) {

      return EFI_SUCCESS;
    }
//[-end-130911-IB05670199-modify]//

  if (!OemUpdatedPublickey) {
    //    
    // Get the Pubkey rom image
    //
    Status = GetFvImage (&PubkeyRomGuid, &PubkeyRomImage, &PubkeyRomSize);
    if (EFI_ERROR (Status)) {

      return EFI_UNSUPPORTED;
    }
  
    //
    // Copy Pubkey ROM Image to the memory pool that we create in Slic table.
    //
    if (PubkeyRomSize == sizeof (EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE)) {
      CopyMem (&SlicPointer->PublicKey, PubkeyRomImage, PubkeyRomSize);
    } else {
      return EFI_UNSUPPORTED;
    }
  }

  if (!OemUpdatedMarker) {
    //    
    // Get the Marker rom image
    //
    Status = GetFvImage (&MarkerRomGuid, &MarkerRomImage, &MarkerRomSize);
    if (EFI_ERROR (Status)) {

      return EFI_UNSUPPORTED;
    }
  
    //
    // Copy Marker ROM Image to the memory pool that we create in Slic table.
    //
    if (MarkerRomSize == sizeof (EFI_ACPI_SLP_MARKER_STRUCTURE)) {
      CopyMem (&SlicPointer->SlpMarker, MarkerRomImage, MarkerRomSize);
    } else {
      return EFI_UNSUPPORTED;
    }
  }
  
  return  EFI_SUCCESS;
}
