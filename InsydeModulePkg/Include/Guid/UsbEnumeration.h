/** @file
 Guid used to trigger event to do USB connection.

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

#ifndef _USB_ENUMERATION_H_
#define _USB_ENUMERATION_H_

#define EFI_USB_ENUMERATION_GUID \
  {0xaab3064f, 0x56d9, 0x46a3, 0x8e, 0xba, 0x1b, 0x18, 0x8e, 0x7f, 0x83, 0x5a}

extern EFI_GUID gEfiUsbEnumerationGuid;

#endif

