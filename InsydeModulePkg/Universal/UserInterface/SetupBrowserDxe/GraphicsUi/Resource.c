/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Ui.h"
#include "SetupUtility.h"
#include "UiRender.h"
#include "UiControls.h"
#include "UiManager.h"

#include <Protocol/H2ODialog.h>

#include "UiCellLayout.h"
#include "Setup.h"
#include <Protocol/SetupUtilityBrowser.h>
#include <SetupBrowserAniDefs.h>
#include <InternalFormRepresentation.h>

typedef struct {
  CHAR16            *Name;
  EFI_ANIMATION_ID  Id;
} IMAGE_RESOURCE;

IMAGE_RESOURCE mImageResource[] = {
  {L"BootOrderUpNormal",   ANIMATION_TOKEN (BOOT_ORDER_UP_NORMAL   ) },
  {L"BootOrderUpFocus",    ANIMATION_TOKEN (BOOT_ORDER_UP_FOCUS    ) },
  {L"BootOrderUpPush",     ANIMATION_TOKEN (BOOT_ORDER_UP_PUSH     ) },
  {L"BootOrderDownNormal", ANIMATION_TOKEN (BOOT_ORDER_DOWN_NORMAL ) },
  {L"BootOrderDownFocus",  ANIMATION_TOKEN (BOOT_ORDER_DOWN_FOCUS  ) },
  {L"BootOrderDownPush",   ANIMATION_TOKEN (BOOT_ORDER_DOWN_PUSH   ) },
  {L"TimeDate1UpNormal",   ANIMATION_TOKEN (TIME_DATE_1_UP_NORMAL  ) },
  {L"TimeDate1UpPush",     ANIMATION_TOKEN (TIME_DATE_1_UP_PUSH    ) },
  {L"TimeDate1DownNormal", ANIMATION_TOKEN (TIME_DATE_1_DOWN_NORMAL) },
  {L"TimeDate1DownPush",   ANIMATION_TOKEN (TIME_DATE_1_DOWN_PUSH  ) },
  {L"TimeDate2UpNormal",   ANIMATION_TOKEN (TIME_DATE_2_UP_NORMAL  ) },
  {L"TimeDate2UpPush",     ANIMATION_TOKEN (TIME_DATE_2_UP_PUSH    ) },
  {L"TimeDate2DownNormal", ANIMATION_TOKEN (TIME_DATE_2_DOWN_NORMAL) },
  {L"TimeDate2DownPush",   ANIMATION_TOKEN (TIME_DATE_2_DOWN_PUSH  ) },
  {L"ButtonNormal",            ANIMATION_TOKEN (BUTTON_NORMAL  ) },
  {NULL, 0}
  };

UINT32
GetImageIdByName (
  CHAR16  *Name
  )
{
  UINT32                ImageId;
  UINTN                 Index;

  ImageId = 0;

  if (Name[0] == '@') {
    Index = 0;
    Name++;
    while (mImageResource[Index].Name != NULL) {
      if (StrCmp (mImageResource[Index].Name, Name) == 0) {
        ImageId = 0xFFFF0000 | mImageResource[Index].Id;
        break;
      }
      Index++;
    }
    ASSERT (ImageId != 0);
  } else {
    ImageId = (UINT32) StrHexToUintn (Name);
  }

  return ImageId;
}

