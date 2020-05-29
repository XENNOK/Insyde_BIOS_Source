/** @file
 DXE Chipset Services Library.
	
 This file contains only one function that is DxeCsSvcSkipGetPciRom().
 The function DxeCsSvcSkipGetPciRom() use chipset services to return 
 a PCI ROM image for the device is represented or not.
	
***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Protocol/H2ODxeChipsetServices.h>

//
// Global Variables (This Source File Only)
//
extern H2O_CHIPSET_SERVICES_PROTOCOL *mChipsetSvc;

/**
 Update ACPI table content according to platform specific

 @param[in, out]    TableHeader             Pointer of the table to update
 @param[in, out]    CommonCodeReturnStatus  Return Status from Common Code

 @retval            EFI_SUCCESS             Update table success, Table has to be installed
 @retval            EFI_UNSUPPORTED         Table update failed, table should not be installed
*/
EFI_STATUS
DxeCsSvcUpdateAcpiTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      *TableHeader,
  IN OUT EFI_STATUS                       *CommonCodeReturnStatus
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, UpdateAcpiTable) + sizeof (VOID*)) || 
      mChipsetSvc->UpdateAcpiTable == NULL) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member UpdateAcpiTable() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  mChipsetSvc->UpdateAcpiTable (TableHeader, CommonCodeReturnStatus);
  return EFI_SUCCESS;
}  
