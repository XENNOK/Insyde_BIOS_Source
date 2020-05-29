/** @file
  CrHookDxe driver for BDS phase module Header
  
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

#ifndef _CR_BDS_H
#define _CR_BDS_H

#include <Uefi.h>

#include <Protocol/DevicePath.h>
#include <Protocol/ConsoleRedirectionService.h>
#include <Protocol/CRBdsHook.h>
#include <Protocol/PciRootBridgeIo.h>

extern EFI_HANDLE mCrHookImageHandle;

EFI_STATUS
PlatformBdsConnectCrTerminal (
  IN  EFI_STATUS  ActVgaStatus,
  IN  CHAR16      *ConInVarName,
  IN  EFI_GUID    *ConInVarGuid,
  IN  CHAR16      *ConOutVarName,
  IN  EFI_GUID    *ConOutVarGuid
  );


EFI_STATUS
DisconnectCrTerminal (
  IN EFI_CONSOLE_REDIRECTION_SERVICE_PROTOCOL  *This,
  IN UINTN                                     TerminalNumber
  );

#endif // _CONSOLEREDIRECTIONBDS_H

