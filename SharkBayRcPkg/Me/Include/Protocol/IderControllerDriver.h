/** @file

  IDE-R Controller Driver Protocol definitions.
  We can specify normal IDE contorller driver and IDE-R controller by checking this GUID

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

#ifndef _EFI_IDER_CONTROLLER_DRIVER_PROTOCOL_H_
#define _EFI_IDER_CONTROLLER_DRIVER_PROTOCOL_H_

///
/// IDER Controller Driver Protocol
/// This protocol is used in Platform BDS phase to identify IDE-R boot devices.
///
#define EFI_IDER_CONTROLLER_DRIVER_PROTOCOL_GUID \
  { \
    0x956a2ed0, 0xa6cf, 0x409a, 0xb8, 0xf5, 0x35, 0xf1, 0x4c, 0x3e, 0x3c, 0x2 \
  }

extern EFI_GUID gEfiIderControllerDriverProtocolGuid;

#endif
