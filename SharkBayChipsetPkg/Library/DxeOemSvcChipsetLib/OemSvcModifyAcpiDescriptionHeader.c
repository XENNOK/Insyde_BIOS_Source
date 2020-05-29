/** @file
   This function provides an interface to modify the ACPI description table header.
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcChipsetLib.h>

/**
 This function provides an interface to modify the ACPI description table header.

 @param[in out]     *Table              On entry, points to ACPI description table header.
                                        On exit , points to updated ACPI description table header.
 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcModifyAcpiDescriptionHeader (
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER            *Table
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
#if 0 // Sample Implementation
  *(UINT32*)(Table->OemId)      = SIGNATURE_32 ('I', 'N', 'S', 'Y');
  *(UINT16*)(Table->OemId + 4)  = SIGNATURE_16 ('D', 'E');
  Table->OemTableId             = SIGNATURE_64 ('H', 'S', 'W', '-', 'L', 'P', 'T', 0);
  //
  // Creator ID & Revision are used for ASL compiler
  //
  Table->CreatorId              = SIGNATURE_32 ('A', 'C', 'P', 'I');
  Table->CreatorRevision        = 0x00040000;
#endif
  return EFI_UNSUPPORTED;
}

