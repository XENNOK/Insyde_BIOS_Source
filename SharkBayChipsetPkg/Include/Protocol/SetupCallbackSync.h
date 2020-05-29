/** @file

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

#ifndef _SETUP_CALLBACK_SYNC_PROTOCOL_H_
#define _SETUP_CALLBACK_SYNC_PROTOCOL_H_

#define SETUP_CALLBACK_SYNC_PROTOCOL_GUID \
  { \
    0x8015350, 0x6164, 0x4d64, {0xb1, 0xe0, 0xe7, 0x74, 0xe6, 0x94, 0xfa, 0x6}\
  }

typedef struct _SETUP_CALLBACK_SYNC_PROTOCOL SETUP_CALLBACK_SYNC_PROTOCOL;

typedef struct _SETUP_CALLBACK_SYNC_PROTOCOL {
  UINT8          CallbackSmiNumber;
};

extern EFI_GUID gSetupCallbackSyncGuid;

#endif
