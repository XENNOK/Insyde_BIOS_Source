/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _SLOT_STATUS_GUID_H_
#define _SLOT_STATUS_GUID_H_

#define SLOT_STATUS_GUID \
  { 0x46b96778, 0x4265, 0x409c, 0x9a, 0xc6, 0x70, 0x90, 0xcc, 0xeb, 0x97, 0x7f}

#define NOT_PRESENT  0xFF

typedef struct {
  UINT8  MaxChannel;
  UINT8  MaxDimmsInChannel;
  UINT8  SlotStatus[1];
} SLOT_STATUS_INFO;

extern EFI_GUID gSlotStatusGuid;

#endif

