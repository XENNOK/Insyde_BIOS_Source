/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _FD_TABLE_INFO_HOB_H_
#define _FD_TABLE_INFO_HOB_H_

#define FD_TABLE_INFO_HOB_GUID  \
  { \
    0x63bdf162, 0xeeef, 0x429b, 0x9a, 0x1f, 0xf5, 0x5a, 0x2c, 0x46, 0x6d, 0x76 \
  }

#define FD_INDEX_INVALID 0xff

typedef struct {
  UINT8   CurrentFdIndex;
} FD_TABLE_HOB_DATA;

extern EFI_GUID gFdTableInfoHobGuid;

#endif
