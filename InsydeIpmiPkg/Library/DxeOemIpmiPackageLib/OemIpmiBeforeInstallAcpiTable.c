/** @file
 DXE OEM IPMI Package library implement code - Before Install ACPI Table.

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


#include <Library/DxeOemIpmiPackageLib.h>


/**
 Reserved for OEM to implement something additional requirements before install ACPI Table.

 @param[in]         SpmiTable           Pointer to EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_DESCRIPTION_TABLE struct.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiBeforeInstallAcpiTable (
  IN  EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_DESCRIPTION_TABLE    *SpmiTable
  )
{
  return EFI_UNSUPPORTED;
}

