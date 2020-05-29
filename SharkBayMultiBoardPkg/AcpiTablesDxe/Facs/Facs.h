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

#ifndef _FACS_H_
#define _FACS_H_

//
// Statements that include other files
//
#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/Acpi20.h>


//
// FACS Definitions
//
#define EFI_ACPI_FIRMWARE_WAKING_VECTOR 0x00000000
#define EFI_ACPI_GLOBAL_LOCK            0x00000000

//
// Firmware Control Structure Feature Flags are defined in AcpiX.0.h
//
#define EFI_ACPI_FIRMWARE_CONTROL_STRUCTURE_FLAGS 0x00000000

#define EFI_ACPI_X_FIRMWARE_WAKING_VECTOR         0x0000000000000000

#endif
