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

#include <IndustryStandard/Acpi.h>

#include <KernelSetupConfig.h>

/**
  Update MADT table content
  Depends on ACPI Version option in Setup Variable for the 64 bits table support.

  @param  Table           The table to update
  @param  SetupVariable   SETUP Variable pointer

  @return EFI_SUCCESS     Update table success
  @return EFI_UNSUPPORTED APIC Mode disabled.

**/
EFI_STATUS
MadtUpdateCommon (
  IN OUT EFI_ACPI_COMMON_HEADER       *Table,
  IN KERNEL_CONFIGURATION             *SetupVariable
)
{
  EFI_STATUS        Status;
  
  Status = EFI_SUCCESS;
  
  //
  // if not HT and not APIC then don't publish the APIC tables.(Now we didn't support HT).
  //
  if (SetupVariable->IoApicMode == 0) {
    
    Status = EFI_UNSUPPORTED;
    
  } else {
  
    switch (SetupVariable->AcpiVer) {
    case 0x03:
    case 0x02:
      ((EFI_ACPI_4_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *)(Table))->Header.Revision = EFI_ACPI_4_0_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION;
      break;    
  
    case 0x01:  
      ((EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *)(Table))->Header.Revision = EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION;
      break;
          
    case 0:
    default:
      ((EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *)(Table))->Header.Revision = EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION;
      break;
    }  
    
  }
  return Status;
}

