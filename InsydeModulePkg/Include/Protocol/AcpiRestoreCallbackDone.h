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

#ifndef _ACPI_RESTORE_CALLBACK_DONE_
#define _ACPI_RESTORE_CALLBACK_DONE_

#define ACPI_RESTORE_CALLBACK_DONE_PROTOCOL_GUID \
  {0x5978b821, 0x3f33, 0x4347, 0xa5, 0x69, 0x02, 0x3b, 0x6b, 0xf7, 0xbe, 0x59}

#define ACPI_RESTORE_CALLBACK_START_PROTOCOL_GUID \
  {0x7b3d95cf, 0xd35e, 0x4fdf, 0x89, 0xdc, 0x0e, 0xdc, 0xc8, 0x58, 0x62, 0xef}

extern EFI_GUID gAcpiRestoreCallbackDoneProtocolGuid;
extern EFI_GUID gAcpiRestoreCallbackStartProtocolGuid;

#endif
