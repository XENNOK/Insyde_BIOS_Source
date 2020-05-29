//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//

#ifndef _EFI_SWITCHABLE_GRAPHICS_H
#define _EFI_SPEAKER_H

//
// Global Id for Switchable Graphics
//
#define EFI_SWITCHABLE_GRAPHICS_PROTOCOL_GUID \
  { \
    0xa9647a1c, 0x1814, 0x44d0, 0x8e, 0x75, 0x59, 0xa8, 0x26, 0xb2, 0x33, 0xe6 \
  }


EFI_FORWARD_DECLARATION (EFI_SWITCHABLE_GRAPHICS_PROTOCOL);


//
// Protocol definition
//
typedef struct _EFI_SWITCHABLE_GRAPHICS_PROTOCOL { 
  UINT8     Rsvd[4];  // Currently this protocol is only used for notification before video dispatched
} EFI_SWITCHABLE_GRAPHICS_PROTOCOL;

extern EFI_GUID gEfiSwitchableGraphicsProtocolGuid;
#endif
