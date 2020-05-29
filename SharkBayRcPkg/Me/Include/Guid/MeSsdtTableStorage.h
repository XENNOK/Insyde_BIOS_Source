/** @file

  GUID definition for the ME SSDT ACPI table storage file name

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

#ifndef _ME_SSDT_TABLE_STORAGE_H_
#define _ME_SSDT_TABLE_STORAGE_H_

#define ME_SSDT_ACPI_TABLE_STORAGE_GUID \
  { 0x9A8F82D5, 0x39B1, 0x48DA, { 0x92, 0xDC, 0xA2, 0x2D, 0xA8, 0x83, 0x4D, 0xF6 }}

extern EFI_GUID gMeSsdtAcpiTableStorageGuid;

#endif
