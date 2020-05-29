//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _ILOADERDRV_PROTOCOL_
#define _ILOADERDRV_PROTOCOL_

#define EFI_ILOADERDRV_PROTOCOL_GUID \
	{ 0xfff1e258, 0xebbf, 0x48be, { 0x81, 0x8f, 0x93, 0xb9, 0x3c, 0x9f, 0x5e, 0x4f } }

EFI_FORWARD_DECLARATION (EFI_ILOADERDRV_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EFI_ILOADERDRV_PROTOCOL_BOOT) (
  IN EFI_ILOADERDRV_PROTOCOL              *This,
  IN UINT16                               *FileName
  );

typedef struct _EFI_ILOADERDRV_PROTOCOL {
	EFI_ILOADERDRV_PROTOCOL_BOOT Boot;
} EFI_ILOADERDRV_PROTOCOL;

extern EFI_GUID gEfiiLoaderDrvProtocolGuid;

#endif
