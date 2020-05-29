/** @file
  Header file for Main menu

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

#ifndef _MAIN_CALLBACK_H_
#define _MAIN_CALLBACK_H_

#include "SetupUtilityLibCommon.h"
#include "SystemInformation.h"


#define FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_BASE   PcdGet32 (PcdFlashNvStorageDmiBase)
#define FLASH_REGION_NV_COMMON_STORE_SUBREGION_OEM_DMI_STORE_SIZE   PcdGet32 (PcdFlashNvStorageDmiSize)

EFI_STATUS
MainCallbackRoutineByAction (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

#endif
