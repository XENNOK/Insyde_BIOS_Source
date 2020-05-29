/** @file
  Header file for Exit menu

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EXIT_CALLBACK_H_
#define _EXIT_CALLBACK_H_

#include "SetupUtilityLibCommon.h"
#include "ExitFunc.h"

#define CUSTOM_BOOT_ORDER_SIZE              256

typedef struct {
  UINT16                                    BootOption;
  UINT16                                    DeviceNameLength;
  UINT8                                     FindFlag;
  CHAR16                                    *DeviceName;
} BOOT_ORDER_OPTION_HEAD;

EFI_STATUS
ExitCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

#endif
