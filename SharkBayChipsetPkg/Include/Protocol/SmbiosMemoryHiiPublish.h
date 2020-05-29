/** @file
  
  SMBIOS Memory Hii Publish
  
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

#ifndef _SMBIOS_MEMORY_HII_PUBLISH_PROTOCOL_H_
#define _SMBIOS_MEMORY_HII_PUBLISH_PROTOCOL_H_

#define EFI_SMBIOS_MEMORY_HII_PUBLISH_PROTOCOL_GUID \
  { \
    0x1ec4c3df, 0x8139, 0x20b8, 0xc0, 0x94, 0xdc, 0x93, 0x6a, 0x3a, 0x14, 0xc2 \
  }

extern EFI_GUID gEfiSmbiosMemoryHiiPublishProtocolGuid;

#endif
