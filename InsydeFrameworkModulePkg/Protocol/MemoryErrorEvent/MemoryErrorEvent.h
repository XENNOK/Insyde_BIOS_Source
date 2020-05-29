//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _MEMORY_ERROR_EVENT_H_
#define _MEMORY_ERROR_EVENT_H_

#define MEMORY_ERROR_EVENT_GUID  \
  { \
    0xad9f9abb, 0x1f89, 0x4c57, 0xa1, 0x2d, 0xa2, 0xdb, 0x2d, 0xfa, 0xf5, 0x98 \
  }

EFI_FORWARD_DECLARATION (MEMORY_ERROR_EVENT_PROTOCOL);

extern EFI_GUID gMemoryErrorEventGuid;

#endif
