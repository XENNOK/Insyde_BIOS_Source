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

/*++

Module Name:

  S3RestoreAcpiCallback.h

Abstract:

  Restore system configuration have completed.

--*/

#ifndef _PEI_S3_RESTORE_ACPI_CALLBACK_PPI_H
#define _PEI_S3_RESTORE_ACPI_CALLBACK_PPI_H

#define PEI_S3_RESTORE_ACPI_CALLBACK_PPI_GUID  \
    { 0xecba0b1c, 0x1508, 0x48b4, 0xa1, 0xeb, 0x5, 0xf8, 0x24, 0xc2, 0x9, 0xe3}   //{ECBA0B1C-1508-48b4-A1EB-05F824C209E3}

extern EFI_GUID gPeiS3RestoreAcpiCallbackPpiGuid;

#endif
