/** @file
   Protocol definition for H2O_FLASH_PROTOCOL
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

#ifndef _H2O_FLASH_PROTOCOL_H_
#define _H2O_FLASH_PROTOCOL_H_

#include <Uefi.h>
#include <Library/H2OFlashLib.h>

#define H2O_FLASH_PROTOCOL_GUID \
  { 0xd6ba94d6, 0xa5c7, 0x4180, {0x9c, 0x73, 0x58, 0x15, 0xd4, 0xf6, 0xd4, 0x1d}}

#define H2O_SMM_FLASH_PROTOCOL_GUID \
  { 0x0e54d9d0, 0x6288, 0x48e1, {0x99, 0xbc, 0xa7, 0x02, 0x80, 0x12, 0xfd, 0x6e}}

typedef struct _H2O_FLASH_PROTOCOL H2O_FLASH_PROTOCOL;
typedef H2O_FLASH_PROTOCOL H2O_SMM_FLASH_PROTOCOL;

typedef struct _H2O_FLASH_LPC_DEVICE {
  UINT32 Size;

  UINT32 IdAddress;
  UINT32 CmdPort55;
  UINT32 CmdPort2A;
} H2O_FLASH_LPC_DEVICE;

typedef
EFI_STATUS
(EFIAPI *H2O_FLASH_ERASE)(
  IN H2O_FLASH_PROTOCOL    *This,
  IN EFI_PHYSICAL_ADDRESS  PhysDest,
  IN UINT64                EraseSize
  );

typedef
EFI_STATUS
(EFIAPI *H2O_FLASH_GET_DEVICE)(
  IN  H2O_FLASH_PROTOCOL    *This,
  IN  EFI_PHYSICAL_ADDRESS  PhysDest,
  OUT H2O_FLASH_DEVICE      **FlashDevice
  );

typedef
EFI_STATUS
(EFIAPI *H2O_FLASH_READ)(
  IN H2O_FLASH_PROTOCOL *This,
  IN UINT8                 *Dest,
  IN UINT8                 *LogicalSrc,
  IN UINT64                Size
  );

typedef
EFI_STATUS
(EFIAPI *H2O_FLASH_PROGRAM)(
  IN H2O_FLASH_PROTOCOL *This,
  IN UINT8                 *LogicalDest,
  IN UINT8                 *Src,
  IN UINT64                *NumBytes,
  IN EFI_PHYSICAL_ADDRESS  PhysDest
  );

typedef struct _H2O_FLASH_PROTOCOL {
  UINT32                            Size;

  H2O_FLASH_ERASE                Erase;
  H2O_FLASH_GET_DEVICE           GetDevice;
  H2O_FLASH_READ                 Read;
  H2O_FLASH_PROGRAM              Program;
};


extern EFI_GUID gH2OFlashProtocolGuid;
extern EFI_GUID gH2OSmmFlashProtocolGuid;

#endif
