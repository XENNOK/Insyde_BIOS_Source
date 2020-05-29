/** @file

  ACPI Disable Callback Done Protocol  

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
 
#ifndef _ACPI_DISABLE_CALLBACK_DONE_
#define _ACPI_DISABLE_CALLBACK_DONE_

#define ACPI_DISABLE_CALLBACK_DONE_PROTOCOL_GUID \
  {0x8dcd8fb0, 0xfea0, 0x472c, 0x98, 0x4f, 0xd3, 0xa8, 0x72, 0xa7, 0x0d, 0xa5}

#define ACPI_DISABLE_CALLBACK_START_PROTOCOL_GUID \
  {0x89f47d4e, 0x908a, 0x419a, 0x87, 0x9f, 0x1e, 0xbc, 0x94, 0x70, 0xae, 0xf6}

extern EFI_GUID gAcpiDisableCallbackDoneProtocolGuid;
extern EFI_GUID gAcpiDisableCallbackStartProtocolGuid;

#endif
