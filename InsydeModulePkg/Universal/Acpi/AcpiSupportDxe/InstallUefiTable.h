/** @file
  Herder file for install UEFI ACPI table.
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

#ifndef _INSTALL_UEFI_TABLE_H_
#define _INSTALL_UEFI_TABLE_H_

#include <PiDxe.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiSupport.h>



#define UEFI_ACPI_TABLE_INSYDE_BIOS_GUID \
  { 0x9d4bf935, 0xa674, 0x4710, 0xba, 0x2, 0xbf, 0xa, 0xa1, 0x75, 0x8c, 0x7b }

#define UEFI_ACPI_TABLE_SIGNATURE    SIGNATURE_32 ('U', 'E', 'F', 'I')

#define BVDT_BASE_ADDRESS               FixedPcdGet32 (PcdFlashNvStorageBvdtBase)
#define BVDT_SIZE                       FixedPcdGet32 (PcdFlashNvStorageBvdtSize)

#pragma pack(1)

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER  Header;
  EFI_GUID                     Identifier;
  UINT16                       DataOffset;
} EFI_ACPI_TABLE;

#pragma pack()

typedef struct {
  EFI_ACPI_TABLE       UefiAcpiDataTable;                       
  UINT8                Data[BVDT_SIZE];
} EFI_ACPI_UEFI_TABLE;

EFI_STATUS
InstallUefiTable (
  IN EFI_ACPI_SUPPORT_PROTOCOL   *AcpiSupport,
  IN UINT32                      AcpiVersion
  );

#endif
