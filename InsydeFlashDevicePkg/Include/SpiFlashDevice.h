/** @file
  Header file for SPI Flash Device definition

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

#ifndef _H2O_FLASH_SPI_DEVICE_H_
#define _H2O_FLASH_SPI_DEVICE_H_
#include <Uefi.h>

#pragma pack(1)
typedef struct _H2O_FLASH_SPI_DEVICE {
  UINT32  Size;

  UINT8   ReadIdCommandOp;
  UINT8   EraseCommandOp;
  UINT8   WriteCommandOp;
  UINT8   WriteStatusCommandOp;
  UINT8   ReadCommandOp;
  UINT8   ReadStatusCommandOp;
  UINT8   LockCommandOp;
  UINT8   UnlockCommandOp;
  UINT16  OpType;
  UINT8   WriteEnablePrefix;
  UINT8   WriteStatusEnablePrefix;
  BOOLEAN GlobalProtectAvailable;
  BOOLEAN BlockProtectAvailable;
  BOOLEAN BlockProtectCodeRequired;
  BOOLEAN MultiByteProgramAvailable;
  UINT8   BytesOfId;
  UINT8   MinBytesPerProgRead;
  BOOLEAN NonVolStatusAvailable;
  UINT8   Reserved;
  UINT8   GlobalProtectCode;
  UINT8   GlobalUnprotectCode;
  UINT8   BlockProtectCode;
  UINT8   BlockUnprotectCode;
  UINT32  DeviceSize;
  UINT32  BlockEraseSize;
  UINT32  BlockProtectSize;
} H2O_FLASH_SPI_DEVICE;
#pragma pack()


#endif
