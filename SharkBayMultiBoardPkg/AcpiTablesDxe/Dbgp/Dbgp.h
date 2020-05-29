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

#ifndef _DBGP_H_
#define _DBGP_H_

#include <AcpiHeaderDefaultValue.h>
#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/Acpi30.h>


//
// DBGP Definitions
//
#define EFI_ACPI_OEM_DBGP_TABLE_REVISION 0x00000001

//[-start-120615-IB09850016-modify]//
//#define EFI_ACPI_OEM_DBGP_INTERFACE_TYPE 0x00000002
#define EFI_ACPI_OEM_DBGP_INTERFACE_TYPE 0x00000001
//[-end-120615-IB09850016-modify]//

//
// Ensure proper structure formats
//
#pragma pack (1)

//
// DBGP structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER            Header;
  UINT8                                  InterfaceType;
  UINT8                                  Reserved[3];
  EFI_ACPI_3_0_GENERIC_ADDRESS_STRUCTURE BaseAddress;
} EFI_ACPI_DEBUG_PORT_TABLE;

#pragma pack ()

#endif
