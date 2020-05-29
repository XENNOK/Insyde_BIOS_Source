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

#ifndef _WPBT_DXE_H_
#define _WPBT_DXE_H_

#include <IndustryStandard/Acpi50.h>

#pragma pack(1)
///
/// Platform Binary Table definition.
///
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT32                      BinarySize;
  UINT64                      BinaryLocation;
  UINT8                       Layout;
  UINT8                       Type;
  UINT16                      ArgLength;
} EFI_ACPI_5_0_PLATFORM_BINARY_TABLE;


///
/// WPBT Revision
///
#define EFI_ACPI_5_0_PLATFORM_BINARY_TABLE_REVISION 1

///
/// WPBT Content Layout
///   1: Single PE image
///
#define EFI_ACPI_5_0_WPBT_CONTENT_LAYOUT         0x01

///
/// WPBT Content Type 
///  1: native user-mode application
///
#define EFI_ACPI_5_0_WPBT_CONTENT_TYPE         0x01

#pragma pack()

#endif
