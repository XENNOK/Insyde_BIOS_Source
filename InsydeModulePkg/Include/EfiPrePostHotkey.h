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

/*++
Module Name:
  
    EfiPrePostHotkey.h
    
Abstract:

    Efi PrePostHotkey variable structure.

--*/

#ifndef _EFI_PRE_POST_HOTKEY_H_
#define _EFI_PRE_POST_HOTKEY_H_


#define PRE_POST_HOTKEY_NOT_EXIST     0x00
#define PRE_POST_HOTKEY_OTHER_KEY     0xFF


#define EFI_PRE_POST_HOTKEY_NAME      L"PrePostHotkey"

typedef struct _EFI_PRE_POST_HOTKEY {
  UINT32        KeyBit;
  UINT16        ScanCode;
} EFI_PRE_POST_HOTKEY;

#endif
