/** @file
  Header file for LPC Flash Device definition

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

#ifndef _LPC_FLASH_DEVICE_H_
#define _LPC_FLASH_DEVICE_H_

#pragma pack(1)
typedef struct _H2O_FLASH_LPC_DEVICE {
  UINT32 Size;

  UINT32 IdAddress;
  UINT32 CmdPort55;
  UINT32 CmdPort2A;
} H2O_FLASH_LPC_DEVICE;
#pragma pack()

#endif
