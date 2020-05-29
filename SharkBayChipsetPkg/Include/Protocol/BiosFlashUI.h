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

#ifndef _BIOS_FLASH_UI_H_
#define _BIOS_FLASH_UI_H_

//
// bios flash ui protocol
//
// {B6EB663C-30B5-450d-AE57-D36974D41F4D}
#define BIOS_FLASH_UI_PROTOCOL_GUID \
  { 0xB6EB663C, 0x30B5, 0x450D, {0xAE, 0x57, 0xD3, 0x69, 0x74, 0xD4, 0x1F, 0x4D} }

typedef struct _BIOS_FLASH_UI_PROTOCOL BIOS_FLASH_UI_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *DRAW_BIOS_FLASH_UI) (
  IN OUT VOID                       **BufferPtr,
  IN OUT UINTN                      *BufferSize,
  IN OUT CHAR16                     **CommandString
  );

//
// BIOS FLASH UI Protocol
//
struct _BIOS_FLASH_UI_PROTOCOL {
  DRAW_BIOS_FLASH_UI       DrawBiosFlashUI;
};


//
// Extern the GUID for protocol users.
//
extern EFI_GUID gBiosFlashUIProtocolGuid;

#endif
