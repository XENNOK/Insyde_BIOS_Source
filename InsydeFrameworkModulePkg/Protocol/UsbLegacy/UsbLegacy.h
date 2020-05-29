//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UsbLegacy.h

Abstract:

  Protocol used to get the number of USB devices in a given platform.

--*/

#ifndef _USB_LEGACY_H_
#define _USB_LEGACY_H_

EFI_FORWARD_DECLARATION(_EFI_USB_LEGACY_PROTOCOL);

#define EFI_USB_LEGACY_PROTOCOL_GUID \
  {0xef33c296, 0xf64c, 0x4146, 0xad, 0x04, 0x34, 0x78, 0x99, 0x70, 0x2c ,0x84}

//
// Here is the protocol
//
typedef struct {
    UINT8                  UsbKeyboard;
    UINT8                  UsbMouse;
    UINT8                  UsbFloppy;
    UINT8                  UsbMass;
    UINT8                  UsbZip;
    UINT8                  UsbCD;
} EFI_USB_LEGACY_PROTOCOL;

extern EFI_GUID gEfiUsbLegacyProtocolGuid;

#endif
