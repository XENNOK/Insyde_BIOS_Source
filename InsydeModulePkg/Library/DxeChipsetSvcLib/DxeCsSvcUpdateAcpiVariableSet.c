/** @file
 DXE Chipset Services Library.
	
 Update ACPI variable set called upon AcpiS3Save driver entry and S3 ready. Chipset can use it to
 update the content of customized ACPI_VARIABLE_SET variable to compliant with individual chipset RC
	
;***************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Protocol/H2ODxeChipsetServices.h>

//
// Global Variables (This Source File Only)
//
extern H2O_CHIPSET_SERVICES_PROTOCOL *mChipsetSvc;

/**
 Update ACPI variable set called upon AcpiS3Save driver entry and S3 ready. Chipset can use it to
 update the content of customized ACPI_VARIABLE_SET variable to compliant with individual chipset RC

 @param[in]         IsS3Ready                   Indicates the current phase is initial or S3 ready
 @param[in]         S3AcpiReservedMemorySize    The size of S3AcpiReservedMemory

 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_UNSUPPORTED     Use ordinary path to update standard ACPI_VARIABLE_SET content
*/
EFI_STATUS
EFIAPI
DxeCsSvcUpdateAcpiVariableSet (
  IN BOOLEAN     IsS3Ready,
  IN UINT32      S3AcpiReservedMemorySize
  )
{
  //
  // Verify that the protocol interface structure contains the function 
  // pointer and whether that function pointer is non-NULL. If not, return
  // an error.
  //
  if (mChipsetSvc == NULL ||
      mChipsetSvc->Size < (OFFSET_OF (H2O_CHIPSET_SERVICES_PROTOCOL, UpdateAcpiVariableSet) + sizeof (VOID*)) || 
      mChipsetSvc->UpdateAcpiVariableSet == NULL ||
      S3AcpiReservedMemorySize == 0) {
    DEBUG ((EFI_D_ERROR, "H2O DXE Chipset Services can not be found or member UpdateAcpiVariableSet() isn't implement!\n"));
    return EFI_UNSUPPORTED;
  }
  return mChipsetSvc->UpdateAcpiVariableSet (IsS3Ready, S3AcpiReservedMemorySize);
}  
