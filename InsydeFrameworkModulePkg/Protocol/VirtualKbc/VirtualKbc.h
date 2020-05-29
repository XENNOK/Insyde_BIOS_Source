//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _VIRTUAL_KBC_H_
#define _VIRTUAL_KBC_H_

EFI_FORWARD_DECLARATION(EFI_VIRTUAL_KBC_PROTOCOL);

#define EFI_VIRTUAL_KBC_PROTOCOL_GUID \
  {0x5e670ac3, 0xbf6c, 0x4dd4, 0x9b, 0xf7, 0x3d, 0xda, 0xad, 0xc5, 0xf5, 0x4}

typedef
EFI_STATUS
(EFIAPI *EFI_VIRTUAL_KBC_PROTOCOL_IN_OUT) (
  IN EFI_VIRTUAL_KBC_PROTOCOL           * This,
  IN OUT UINT8                          *Buffer
  );

typedef struct {
  EFI_VIRTUAL_KBC_PROTOCOL_IN_OUT       Read;
  EFI_VIRTUAL_KBC_PROTOCOL_IN_OUT       Write;
} EFI_VIRTUAL_KBC_PROTOCOL_ACCESS;

typedef struct _EFI_VIRTUAL_KBC_PROTOCOL {
    EFI_VIRTUAL_KBC_PROTOCOL_ACCESS     Port60;
    EFI_VIRTUAL_KBC_PROTOCOL_ACCESS     Port64;
} EFI_VIRTUAL_KBC_PROTOCOL;

extern EFI_GUID gEfiVirtualKbcProtocolGuid;

#endif
