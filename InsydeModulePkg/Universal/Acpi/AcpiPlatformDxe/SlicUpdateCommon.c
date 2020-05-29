/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesLib.h>

#include <Library/DxeOemSvcKernelLib.h>

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/SLP2_0.h>


#define SLP20_PUBLIC_KEY_ROM_FILE_GUID \
  {0x1a1e2341, 0xa2fb, 0x42c7, 0x8d, 0x17, 0x30, 0x73, 0xd0, 0x8e, 0xb2, 0x1d}

#define SLP20_MARKER_ROM_FILE_GUID \
  {0xdd6569a7, 0xe455, 0x4ee5, 0xb2, 0xba, 0xec, 0xda, 0x84, 0xac, 0xbc, 0x99}

/**
  Update Acpi SLIC table with values stored in Rom.

  @param  Table         The table to update
  @param  Version       Where to install this table

  @return  EFI_SUCCESS  Update table success

**/
EFI_STATUS
SlicUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table
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
  BOOLEAN                               OemUpdatedPublickey = FALSE;
  BOOLEAN                               OemUpdatedMarker    = FALSE;

  if (*(UINT32*)Table == EFI_ACPI_SLIC_TABLE_SIGNATURE) {
    SlicPointer = (EFI_ACPI_SOFTWARE_LICENSING_TABLE *)Table;
  } else {
  	
    return EFI_UNSUPPORTED;
  }
  
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
                                                    
    //
    // If the project wants to report the 0xFF structures for non-Windows OS, 
    // just return OemUpdatedPublickey and OemUpdatedMarker with TRUE, and the
    // SlicPointer->PublicKey and SlicPointer->SlpMarker won't need be updated.
    //     

  OemUpdatedPublickey = FALSE;
  OemUpdatedMarker = FALSE;
  //
  // OemServices
  //
  Status = OemSvcGetSlp20PubkeyAndMarkerRom (
             &SlicPointer->PublicKey,
             &OemUpdatedPublickey,
             &SlicPointer->SlpMarker,
             &OemUpdatedMarker
             );
    if (OemUpdatedPublickey && OemUpdatedMarker) {

      return EFI_SUCCESS;
  }

  if (!OemUpdatedPublickey) {
    //    
    // Get the Pubkey rom image
    //
    Status = GetSectionFromAnyFv(&PubkeyRomGuid, EFI_SECTION_RAW, 0, &PubkeyRomImage, &PubkeyRomSize);
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
    Status = GetSectionFromAnyFv(&MarkerRomGuid, EFI_SECTION_RAW, 0, &MarkerRomImage, &MarkerRomSize);
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

