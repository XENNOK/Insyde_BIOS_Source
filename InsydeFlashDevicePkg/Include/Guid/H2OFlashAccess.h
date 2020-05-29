/** @file
   Protocol definition for H2O_FLASH_ACCESS_PROTOCOL
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_FLASH_ACCESS_GUID_H_
#define _H2O_FLASH_ACCESS_GUID_H_

#include <Protocol/H2OFlash.h>

#define H2O_FLASH_ACCESS_MEMORY_MAPPED_GUID \
  { 0x4A71B316, 0x7027, 0x48A1, 0xA2, 0x60, 0xDD, 0x91, 0x31, 0x75, 0xFE, 0x64 }


typedef H2O_FLASH_ACCESS_PPI        H2O_FLASH_PROTOCOL;
typedef H2O_FLASH_ACCESS_PROTOCOL   H2O_FLASH_PROTOCOL;

extern EFI_GUID gH2OFlashMemoryMappedGuid;

#endif
