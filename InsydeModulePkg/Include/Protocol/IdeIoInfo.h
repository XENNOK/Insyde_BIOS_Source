//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _IDE_IO_INFO_H_
#define _IDE_IO_INFO_H_

#define EFI_IDE_IO_RESOURCE_INFO_PROTOCOL_GUID \
   {0x54c03d2d, 0x5903, 0x4dfb, 0x88, 0xb7, 0xfa, 0x76, 0x36, 0xbe, 0x03, 0xd1}

#define EFI_AHCI_IO_REXOURCE_INFO_PROTOCOL_GUID \
   {0x56947330, 0x585c, 0x4470, 0xa9, 0x5d, 0xc5, 0x5c, 0x52, 0x9f, 0xeb, 0x47}

typedef struct _EFI_DEV_RESOURCE_IO_INFO_PROTOCOL {
  UINT16       CommandBlockIo;
  UINT16       ControlBlockIo;
  UINT8        PortNum;
} EFI_IDE_RESOURCE_IO_INFO_PROTOCOL;

typedef struct _EFI_AHCI_RESOURCE_IO_INFO_PROTOCOL {
  UINTN        AhciMemIoAddr;
  UINTN        PortMemIoAddr;
  UINTN        CmdTableAddr;
  UINTN        RfisMemAddr;
  UINTN        CmdListAddr;
  UINT8        PortNumber;
  UINT8        PortMulNum;
  VOID         *IdentifyRawPtr;
} EFI_AHCI_RESOURCE_IO_INFO_PROTOCOL;

 
extern EFI_GUID gEfiIdeResourceIoInfoProtocolGuid;

extern EFI_GUID gEfiAhciResourceIoInfoProtocolGuid;

#endif
