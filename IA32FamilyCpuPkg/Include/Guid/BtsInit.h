/** @file
  This file defines the BTS INIT GUID.
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BTS_INIT_GUID_H_
#define _BTS_INIT_GUID_H_

#define BTS_INIT_GUID  \
  { \
    0xfa9bdf53, 0x97a2, 0x405f, { 0xb4, 0x9e, 0xd5, 0x4c, 0x19, 0x08, 0xe3, 0xce } \
  }

extern EFI_GUID  gBtsInitGuid;

#endif
