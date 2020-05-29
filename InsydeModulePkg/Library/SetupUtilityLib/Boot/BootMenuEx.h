/** @file
  Header file for Boot Menu relative function

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

#ifndef _SETUP_BOOT_MENU_EX_H_
#define _SETUP_BOOT_MENU_EX_H_

#include <Library/SetupUtilityLib.h>
#include <Library/PrintLib.h>
#include "BbsType.h"
#include "Boot.h"

EFI_STATUS
ConvertBootOptionToPriority (
  IN  UINT16                            BootOption,
  OUT UINT16                            *Priority
  );

EFI_STATUS
SyncOrderListValueToBootOrder (
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT16                                 *Priority
  );

EFI_STATUS
UpdateBootOrderToOrderedListOpcode (
  IN     EFI_HII_HANDLE                 BootHiiHandle,
  IN OUT KERNEL_CONFIGURATION           *KernelConfig
  );

#endif
