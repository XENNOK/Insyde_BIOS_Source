//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _PLATFORM_INFO_H_
#define _PLATFORM_INFO_H_

typedef struct _PLATFORM_INFO_PROTOCOL PLATFORM_INFO_PROTOCOL;

///
/// Protocol GUID
///
#define PLATFORM_INFO_PROTOCOL_GUID \
  { \
    0x97069458, 0xd915, 0x468c, 0xa7, 0x9b, 0xa1, 0x28, 0x46, 0xf7, 0x4a, 0x19 \
  }
///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gEfiPlatformInfoProtocolGuid;

#pragma pack(1)

typedef struct {
  UINT8                 BoardId;
  UINT8                 FabId;
  BOOLEAN               UltPlatform;
} PLATFORM_INFO;

#pragma pack()

///
/// Protocol
///
typedef struct _PLATFORM_INFO_PROTOCOL {
  PLATFORM_INFO        PlatInfo;
} PLATFORM_INFO_PROTOCOL;

#endif
