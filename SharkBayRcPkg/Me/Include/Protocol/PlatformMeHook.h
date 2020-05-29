/** @file

  Interface definition details for platform hook support to ME module during DXE phase.

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

#ifndef _PLATFORM_ME_HOOK_H_
#define _PLATFORM_ME_HOOK_H_

///
/// ME Hook provided by platform for DXE phase
/// This protocol provides an interface to hook reference code by OEM.
///
#define PLATFORM_ME_HOOK_PROTOCOL_GUID \
  { \
    0xbc52476e, 0xf67e, 0x4301, 0xb2, 0x62, 0x36, 0x9c, 0x48, 0x78, 0xaa, 0xc2 \
  }

#define PLATFORM_ME_HOOK_PROTOCOL_REVISION  1

/**
  Platform hook before BIOS sends Global Reset Heci Message to ME

  @param[in] None

  @retval EFI Status Code
**/
typedef
EFI_STATUS
(EFIAPI *PLATFORM_ME_HOOK_PRE_GLOBAL_RESET) (
  VOID
  );

///
/// Platform Intel ME Hook Protocol
/// This protocol provides an interface to hook reference code by OEM.
///
typedef struct _PLATFORM_ME_HOOK_PROTOCOL {
  ///
  /// Revision for the protocol structure
  ///
  UINT8                             Revision;
  ///
  /// Function pointer for the hook called before BIOS sends Global Reset Heci Message to ME
  ///
  PLATFORM_ME_HOOK_PRE_GLOBAL_RESET PreGlobalReset;
} PLATFORM_ME_HOOK_PROTOCOL;

extern EFI_GUID gPlatformMeHookProtocolGuid;

#endif
