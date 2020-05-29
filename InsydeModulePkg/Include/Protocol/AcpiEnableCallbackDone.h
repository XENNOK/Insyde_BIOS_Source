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

#ifndef _ACPI_ENABLE_CALLBACK_DONE_
#define _ACPI_ENABLE_CALLBACK_DONE_

#define ACPI_ENABLE_CALLBACK_DONE_PROTOCOL_GUID \
  {0xcc89c099, 0xd5dc, 0x4af6, 0xa3, 0x77, 0x8a, 0x26, 0xfd, 0x75, 0xf4, 0xad}

#define ACPI_ENABLE_CALLBACK_START_PROTOCOL_GUID \
  {0x0228dc7f, 0x70e4, 0x4bd7, 0xa0, 0x74, 0xf9, 0xf7, 0xbb, 0xe1, 0xaa, 0xab}

extern EFI_GUID gAcpiEnableCallbackDoneProtocolGuid;
extern EFI_GUID gAcpiEnableCallbackStartProtocolGuid;

#endif
