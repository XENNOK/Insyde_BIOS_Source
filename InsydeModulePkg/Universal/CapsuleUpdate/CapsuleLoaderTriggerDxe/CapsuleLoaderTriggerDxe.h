/** @file
  Driver to trigger CapsuleUpdate Flash Loader and Firmware Update

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

#ifndef _CAPSUPLE_LOADER_TRIGGER_H_
#define _CAPSUPLE_LOADER_TRIGGER_H_


#define MAX_BOOT_OPTION_DATA_SIZE             2048
#define MAX_STRING_LENGTH                     128
#define INSYDE_MAGIC_BOOT_INDEX               0x1688

/**
  Enable security check of Capsule images

  @param  None

  @retval EFI_SUCCESS          Security check of Capsule images is enabled
  @return others               Failed to install Capsule security check
**/
EFI_STATUS
EnableCapsuleSecurityCheck (
  VOID
  );

/**

  Enable security check of Capsule images

  @param  None

  @retval EFI_SUCCESS          Security check of Capsule images is disabled
  @return others               Failed to disable Capsule security check

**/
EFI_STATUS
DisableCapsuleSecurityCheck (
  VOID
  );

/**
  Display Windows UX capsule image

  @retval  TRUE                  Windows UX capsule is activated
  @retval  FALSE                 Windows UX capsule is not activated

**/
BOOLEAN
DisplayWindowsUxCapsule (
  VOID
  );

#endif