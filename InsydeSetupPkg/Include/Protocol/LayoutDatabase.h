/** @file
  Layout Database Protocol Header

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

#ifndef _LAYOUT_DATABASE_PROTOCOL_H_
#define _LAYOUT_DATABASE_PROTOCOL_H_

#define LAYOUT_DATABASE_PROTOCOL_GUID \
  { \
   0x27f4ffbe, 0x6c7f, 0x4814, 0xb2, 0x7d, 0xa1, 0x65, 0xb3, 0x6e, 0x3d, 0x9c \
  }

typedef struct _LAYOUT_DATABASE_PROTOCOL LAYOUT_DATABASE_PROTOCOL;

//
// Protocol definitions
//
struct _LAYOUT_DATABASE_PROTOCOL {
  UINT32                              Size;
  EFI_HANDLE                          DriverHandle;
  EFI_HII_HANDLE                      LayoutPkgHiiHandle;
  EFI_HII_HANDLE                      ImagePkgHiiHandle;
  UINTN                               LayoutPkgAddr;
  LIST_ENTRY                          *LayoutListHead;
};

extern GUID gLayoutDatabaseProtocolGuid;

#endif
