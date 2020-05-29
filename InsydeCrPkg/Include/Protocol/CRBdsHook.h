/** @file
  CrBdsHook Protocol Definitions

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

#ifndef _CONSOLE_REDIRECTION_BDS_HOOK_H_
#define _CONSOLE_REDIRECTION_BDS_HOOK_H_

#include <Uefi.h>

#define CONSOLE_REDIRECTION_BDS_HOOK_PROTOCOL_GUID \
  { \
    0x9D9A4DE2, 0xA4C0, 0x415c, 0x91, 0x9E, 0x89, 0x8A, 0x0A, 0xC4, 0x09, 0xB0 \
  }

typedef struct _EFI_CONSOLE_REDIRECTION_BDS_HOOK_PROTOCOL EFI_CONSOLE_REDIRECTION_BDS_HOOK_PROTOCOL;
                                                              
typedef 
EFI_STATUS
(EFIAPI *PLATFORM_BDS_CONNECT_CR_TERMINAL) (
  IN  EFI_STATUS  ActVgaStatus,
  IN  CHAR16      *ConInVarName,
  IN  EFI_GUID    *ConInVarGuid,
  IN  CHAR16      *ConOutVarName,
  IN  EFI_GUID    *ConOutVarGuid
);

struct _EFI_CONSOLE_REDIRECTION_BDS_HOOK_PROTOCOL {
  PLATFORM_BDS_CONNECT_CR_TERMINAL  PlatformBDSConnectCRTerminal;
};

extern EFI_GUID gCRBdsHookProtocolGuid;

#endif
