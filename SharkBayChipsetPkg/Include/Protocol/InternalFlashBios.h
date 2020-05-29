/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _INTERNAL_FLASH_BIOS_H_
#define _INTERNAL_FLASH_BIOS_H_

//
// internal flash bios protocol
//
// {8E033BB2-8F00-4df6-8B21-4810EC73BA72}
#define INTERNAL_FLASH_BIOS_PROTOCOL_GUID \
  { 0x8E033BB2, 0x8F00, 0x4DF6, { 0x8B, 0x21, 0x48, 0x10, 0xEC, 0x73, 0xBA, 0x72 } }

typedef struct _INTERNAL_FLASH_BIOS_PROTOCOL INTERNAL_FLASH_BIOS_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *INTERNAL_FLASH_BIOS) (
  VOID                       *BufferPtr,
  UINTN                      BufferSize,
  CHAR16                     *CommandString
  );

//
// File Select UI Protocol
//
struct _INTERNAL_FLASH_BIOS_PROTOCOL {
  INTERNAL_FLASH_BIOS       FlashBios;
};


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gInternalFlashBiosProtocolGuid;

#endif
