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

#ifndef _SAVE_MEMORY_CONFIG_DONE_H_
#define _SAVE_MEMORY_CONFIG_DONE_H_

//
// { AD84CE0B-C346-0361-9A72-487CF27D3189 }
//
#define SAVE_MEMORY_CONFIG_DONE_GUID \
  { \
    0xAD84CE0B, 0xC346, 0x0361, {0x9A, 0x72, 0x48, 0x7C, 0xF2, 0x7D, 0x31, 0x89} \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID        gSaveMemoryConfigDoneGuid;

#endif
