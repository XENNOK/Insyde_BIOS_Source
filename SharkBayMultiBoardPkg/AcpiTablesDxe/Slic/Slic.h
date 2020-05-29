/** @file

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
#ifndef _SLIC_H
#define _SLIC_H

//
// Statements that include other files
//

#include <AcpiHeaderDefaultValue.h>
#include <IndustryStandard/SLP2_0.h>

//
// SLIC Definitions
//
//
// ACPI table information used to initialize tables.
//
//#define EFI_ACPI_SLIC_TABLE_SIGNATURE       EFI_SIGNATURE_32('S','L','I','C')

#define EFI_ACPI_SLIC_TABLE_REVISION        1


#define EFI_ACPI_SLIC_MAGIC                 EFI_SIGNATURE_32('R','S','A','1')
#define EFI_ACPI_WINDOWS_FLAG               EFI_SIGNATURE_64('W','I','N','D','O','W','S',' ')

#endif
