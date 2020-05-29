/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _WATCH_DOG_HOOK_PROTOCOL_H_
#define _WATCH_DOG_HOOK_PROTOCOL_H_


//
// Watch Dog Timer GUID
//
#define WATCH_DOG_TIMER_PROTOCOL_GUID \
  { 0x490cb604, 0xb01c, 0x4c4d, { 0x81, 0x63, 0x54, 0xa2, 0x9, 0x9a, 0xbc, 0x4f } }


typedef
EFI_STATUS
(EFIAPI *OEM_WATCH_DOG_HOOK) (
  VOID
);


typedef struct  _WATCH_DOG_HOOK_PROTOCOL {
  OEM_WATCH_DOG_HOOK               OemWatchDogHook;
} WATCH_DOG_HOOK_PROTOCOL;

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gWatchDogHookProtocolGuid;

#endif
