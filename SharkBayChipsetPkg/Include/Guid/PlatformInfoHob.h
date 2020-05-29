//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _PLATFORM_INFO_HOB_H_
#define _PLATFORM_INGO_HOB_H_

#include <Pi/PiHob.h>

#define PLATFORM_INFO_HOB_GUID \
  { \
    0x67711e5f, 0x229d, 0x4c23, 0x81, 0x54, 0x9c, 0x1f, 0x3c, 0xb2, 0x9e, 0x41 \
  }

#pragma pack (1)

typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType;
  UINT8                 BoardId;
  UINT8                 FabId;
  BOOLEAN               UltPlatform;
} PLATFORM_INFO_HOB;

#pragma pack()

extern EFI_GUID gBoardIDGuid;

#define PLATFORM_INFO_HOB_NAME     L"PlatformInfoHob"

#endif
