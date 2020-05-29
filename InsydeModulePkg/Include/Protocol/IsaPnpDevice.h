/** @file

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

#ifndef _ISA_PNP_DEVICE_
#define _ISA_PNP_DEVICE_

#define EFI_ISA_PNP303_DEVICE_PROTOCOL_GUID \
  { \
    0x4c7632f0, 0x3dc2, 0x11e1, 0xb8, 0x6c, 0x08, 0x00, 0x20, 0x0c, 0x9a, 0x66 \
  }
extern EFI_GUID gEfiIsaPnp303DeviceProtocolGuid;  // for KBD & MOUSE

#define EFI_ISA_PNP401_DEVICE_PROTOCOL_GUID \
  { \
    0x7d996ff1, 0x26ee, 0x49eb, 0xbf, 0x18, 0xd8, 0x10, 0x09, 0xb7, 0xaf, 0xca \
  }
extern EFI_GUID gEfiIsaPnp401DeviceProtocolGuid;   // for LPT

#define EFI_ISA_PNP501_DEVICE_PROTOCOL_GUID \
  { \
    0x46EE8A89, 0xC3AB, 0x4B35, 0xAE, 0xC1, 0x49, 0x18, 0xE5, 0x6B, 0x09, 0x1C \
  }
extern EFI_GUID gEfiIsaPnp501DeviceProtocolGuid;   // for COM

#define EFI_ISA_PNP510_DEVICE_PROTOCOL_GUID \
  { \
    0x5f4306e8, 0xafb0, 0x4573, 0xa6, 0x48, 0xa0, 0xcc, 0x1b, 0x13, 0x48, 0xd7 \
  }
extern EFI_GUID gEfiIsaPnp510DeviceProtocolGuid;   // for IR

#define EFI_ISA_PNPB02F_DEVICE_PROTOCOL_GUID \
  { \
    0x4d24469d, 0xce4e, 0x46b2, 0x9b, 0xe2, 0x27, 0x33, 0xd, 0x2d, 0xe7, 0x63 \
  }
extern EFI_GUID gEfiIsaPnpB02fDeviceProtocolGuid;  // for GamePort

#define EFI_ISA_PNP604_DEVICE_PROTOCOL_GUID \
  { \
    0xeb9ec8be, 0xff02, 0x47a4, 0xb3, 0x1e, 0x92, 0xe3, 0xa1, 0x17, 0x9b, 0x6c \
  }
extern EFI_GUID gEfiIsaPnp604DeviceProtocolGuid;  // for Floppy

#define EFI_ISA_PNPF03_DEVICE_PROTOCOL_GUID \
  { \
    0x75819fa4, 0x72b0, 0x4e98, 0xb6, 0xa0, 0xff, 0x2a, 0xa8, 0xbf, 0xf0, 0xbb \
  }
extern EFI_GUID gEfiIsaPnpF03DeviceProtocolGuid;  // for PS/2-style Mouse

#endif
