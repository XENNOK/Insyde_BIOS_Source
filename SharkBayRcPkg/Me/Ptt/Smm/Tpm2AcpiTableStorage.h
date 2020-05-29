/** @file

  GUID definition for the TPM2 ACPI table storage file name

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

#ifndef _TPM2_ACPI_TABLE_STORAGE_H_
#define _TPM2_ACPI_TABLE_STORAGE_H_

#define TPM2_ACPI_TABLE_STORAGE_GUID \
  { \
    0x7d279373, 0xeecc, 0x4d4f, 0xae, 0x2f, 0xce, 0xc4, 0xb7, 0x6, 0xb0, 0x6a \
  }

extern EFI_GUID gTpm2AcpiTableStorageGuid;

#endif
