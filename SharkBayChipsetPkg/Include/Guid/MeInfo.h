/** @file



;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _ME_INFO_H_
#define _ME_INFO_H_

#define RECORD_ME_VERSION_GUID {0x6EDE1256, 0xB853, 0x4FD2, 0xA0, 0x99, 0x60, 0xA6, 0xFE, 0x48, 0x5E, 0x1A}

typedef struct {
  UINT16  MajorVersion;
  UINT16  MinorVersion;
  UINT16  HotfixVersion;
  UINT16  BuildVersion;
  CHAR16  *SteppingString;
} RECORD_ME_VERSION;

#endif
