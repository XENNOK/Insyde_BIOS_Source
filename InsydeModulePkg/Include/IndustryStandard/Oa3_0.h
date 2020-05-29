/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _OA_3_0_H
#define _OA_3_0_H

#include <IndustryStandard/Acpi.h>

#define MEMORY_ADDRESS_FOR_MSDM_GUID \
  { 0xfd21bf2b, 0xf5d1, 0x46c5, {0xae, 0xe3, 0xc6, 0x1, 0x58, 0x33, 0x92, 0x39} }
//
// Ensure proper structure formats
//
#pragma pack (1)
typedef struct {
  UINT32                           MsdmVersion;
  UINT32                           MsdmReserved;
  UINT32                           MdsmDataType;
  UINT32                           MsdmDataReserved;
  UINT32                           MsdmDataLength;
 
  UINT8                            MsdmData[29];      //5*5 Product Key, including "-"

} EFI_ACPI_MSDM_DATA_STRUCTURE;

//
// MSDM Table structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER        Header;
  EFI_ACPI_MSDM_DATA_STRUCTURE       MsdmData;
} EFI_ACPI_MICROSOFT_DIGITAL_MARKER_TABLE;
#pragma pack ()

#define EFI_ACPI_MSDM_TABLE_SIGNATURE       SIGNATURE_32 ('M','S','D','M')
#define EFI_ACPI_MSDM_TABLE_REVISION        0x03

#endif
