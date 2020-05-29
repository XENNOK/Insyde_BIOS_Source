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
#ifndef _BOOT_H_
#define _BOOT_H_

#include <Library/CmosLib.h>
#include <AcpiHeaderDefaultValue.h>
#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/Acpi20.h>


//
// Microsoft Simple Boot Flag Table (Boot)
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER             Header;
  UINT8                                   CmosIndex;
  UINT8                                   Reserved[3];  
} EFI_ACPI_2_0_SIMPLE_BOOT_FLAG_STRUCTURE;

//
// BOOT Version (as defined in ACPI 2.0 spec.)
//
#define EFI_ACPI_2_0_SIMPLE_BOOT_FLAG_STRUCTURE_REVISION  0x01

#endif
