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

#ifndef _SLP_2_0_H_
#define _SLP_2_0_H_

#include <IndustryStandard/Acpi.h>

#pragma pack(1)

typedef struct {
  UINT32                           Type;
  UINT32                           Length;
  UINT8                            bType;
  UINT8                            bVersion;
  UINT16                           Reserved;
  UINT32                           aiKeyAlg;
  UINT32                           Magic;
  UINT32                           Bitlen;
  UINT32                           Pubexp;
  UINT8                            Modulus[128];
} EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE;

typedef struct {
  UINT32                           Type;
  UINT32                           Length;
  UINT32                           dwVersion;
  UINT8                            sOemid[6];
  UINT64                           sOemTable;
  UINT64                           sWindowsFlag;
  UINT32                           Reserved1;
  UINT64                           Reserved2;
  UINT64                           Reserved3;
  UINT8                            Signature[128];
} EFI_ACPI_SLP_MARKER_STRUCTURE;

//
// SLIC Table structure
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER        Header;
  EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE  PublicKey;
  EFI_ACPI_SLP_MARKER_STRUCTURE      SlpMarker;

} EFI_ACPI_SOFTWARE_LICENSING_TABLE;

#pragma pack()
#define EFI_ACPI_SLIC_TABLE_SIGNATURE       SIGNATURE_32('S','L','I','C')

#endif
