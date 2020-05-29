/** @file

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

#ifndef _H2O_SWITCHABLE_GRAPHICS_INFO_PROTOCOL_H_
#define _H2O_SWITCHABLE_GRAPHICS_INFO_PROTOCOL_H_

#define H2O_SWITCHABLE_GRAPHICS_INFORMATION_PROTOCOL_GUID \
  { \
    0x4d96c24c, 0xcb7, 0x4650, {0x91, 0x7c, 0x3b, 0xb, 0x70, 0xa6, 0x7f, 0xba} \
  }

#pragma pack (push, 1)
typedef struct {
  UINTN                                       Address;
  UINTN                                       Size;
} BINARY_INFORMATION;
#pragma pack (pop)

//
// Protocol definition
//
#pragma pack (push, 1)
typedef struct _H2O_SG_INFO_PROTOCOL {
  BINARY_INFORMATION                          Vbios;
  BINARY_INFORMATION                          MasterMxmBinFile;
  BINARY_INFORMATION                          SlaveMxmBinFile;
  UINT16                                      GpioBaseAddress;
  UINT8                                       SgMode;
  BOOLEAN                                     IsUltBoard;
} H2O_SG_INFO_PROTOCOL;
#pragma pack (pop)

extern EFI_GUID gH2OSwitchableGraphicsInfoProtocolGuid;

#endif
