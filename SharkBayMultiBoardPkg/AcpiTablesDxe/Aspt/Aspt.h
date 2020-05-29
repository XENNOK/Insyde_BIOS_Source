/** @file
 This file describes the contents of the ACPI OSFR Table

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

#ifndef _ASPT_H
#define _ASPT_H

//
// Statements that include other files
//

#include <AcpiHeaderDefaultValue.h>
#include <IndustryStandard/Acpi10.h>


#pragma pack (1)

#define EFI_ACPI_ASPT_TABLE_REVISION            0x7
#define EFI_ACPI_ASPT_TABLE_SIGNATURE           'TPSA'
#define EFI_ACPI_OEM_TABLE_ID_XTU               SIGNATURE_64('P','e','r','f','T','u','n','e') // OEM table id 8 bytes long

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER          Header;
  UINT64                               Type4MemPtr;
  UINT64                               MemRegionTblPtr;
} EFI_ACPI_ASPT_TABLE;

#pragma pack ()

#endif

