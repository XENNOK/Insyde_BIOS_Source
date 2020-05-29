/** @file

  Platform Trust Technology (FTPM) HECI SkuMgr Library

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

#ifndef _PTT_HECI_LIB_H_
#define _PTT_HECI_LIB_H_

#ifdef PTT_FLAG
#include <Uefi.h>
#include <Library/DebugLib.h>

/**
  Checks whether ME FW has the Platform Trust Technology capability.

  @param[out] PttCapability      TRUE if PTT is supported, FALSE othewrwise.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciGetCapability(
  OUT BOOLEAN   *PttCapability
  );

/**
  Checks Platform Trust Technology enablement state.

  @param[out] IsPttEnabledState  TRUE if PTT is enabled, FALSE othewrwise.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciGetState(
  OUT BOOLEAN   *IsPttEnabledState
  );

/**
  Changes current Platform Trust Technology state.

  @param[in] PttEnabledState     TRUE to enable, FALSE to disable.

  @retval EFI_SUCCESS            Command succeeded
  @retval EFI_DEVICE_ERROR       HECI Device error, command aborts abnormally
**/
EFI_STATUS
EFIAPI
PttHeciSetState(
  IN  BOOLEAN   PttEnabledState
  );

#endif /// PTT_FLAG
#endif
