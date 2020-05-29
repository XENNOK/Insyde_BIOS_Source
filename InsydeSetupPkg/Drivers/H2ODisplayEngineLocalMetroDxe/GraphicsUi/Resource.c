/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "UiRender.h"
#include "UiControls.h"
#include "UiManager.h"
#include <InternalFormRepresentation.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/BitBltLib.h>

EFI_HII_HANDLE                  mImageHiiHandle = NULL;

typedef struct {
  CHAR16                        *Name;
  EFI_IMAGE_ID                  ImageId;
  EFI_IMAGE_INPUT               *ImageIn;       ///< Cache Image Input data
} IMAGE_RESOURCE;

IMAGE_RESOURCE mImageResource[] = {
  {L"FormHalo",              IMAGE_TOKEN (IMAGE_FORM_HALO               ), NULL},
  {L"MenuBackgroundNormal",  IMAGE_TOKEN (IMAGE_MENU_BACKGROUND_NORMAL  ), NULL},
  {L"MenuBackgroundHot",     IMAGE_TOKEN (IMAGE_MENU_BACKGROUND_HOT     ), NULL},
  {L"HotkeyShowHelp",        IMAGE_TOKEN (IMAGE_HOTKEY_SHOW_HELP        ), NULL},
  {L"HotkeySelectItemUp",    IMAGE_TOKEN (IMAGE_HOTKEY_SELECT_ITEM_UP   ), NULL},
  {L"HotkeySelectItemDown",  IMAGE_TOKEN (IMAGE_HOTKEY_SELECT_ITEM_DOWN ), NULL},
  {L"HotkeySelectMenuUp",    IMAGE_TOKEN (IMAGE_HOTKEY_SELECT_MENU_UP   ), NULL},
  {L"HotkeySelectMenuDown",  IMAGE_TOKEN (IMAGE_HOTKEY_SELECT_MENU_DOWN ), NULL},
  {L"HotKeyModifyValueUp",   IMAGE_TOKEN (IMAGE_HOTKEY_MODIFY_VALUE_UP  ), NULL},
  {L"HotKeyModifyValueDown", IMAGE_TOKEN (IMAGE_HOTKEY_MODIFY_VALUE_DOWN), NULL},
  {L"HotKeyLoadDefault",     IMAGE_TOKEN (IMAGE_HOTKEY_LOAD_DEFAULT     ), NULL},
  {L"HotKeySaveAndExit",     IMAGE_TOKEN (IMAGE_HOTKEY_SAVE_AND_EXIT    ), NULL},
  {L"HotkeyEnter",           IMAGE_TOKEN (IMAGE_HOTKEY_ENTER            ), NULL},
  {L"HotKeyDiscardExit",     IMAGE_TOKEN (IMAGE_HOTKEY_DISCARD_EXIT     ), NULL},
  {L"OwnerDrawInsyde",       IMAGE_TOKEN (IMAGE_OWNER_DRAW_INSYDE       ), NULL},
  {L"OwnerDrawInsydeH2O",    IMAGE_TOKEN (IMAGE_OWNER_DRAW_INSYDE_H2O   ), NULL},
  {L"OwnerDrawSeparator",    IMAGE_TOKEN (IMAGE_OWNER_DRAW_SEPARATOR    ), NULL},
  {L"OwnerDrawTemperature",  IMAGE_TOKEN (IMAGE_OWNER_DRAW_TEMPERATURE  ), NULL},
  {L"OwnerDrawTime",         IMAGE_TOKEN (IMAGE_OWNER_DRAW_TIME         ), NULL},
  {L"DialogSeparator",       IMAGE_TOKEN (IMAGE_DIALOG_SEPARATOR        ), NULL},
  {L"DialogButtonDown",      IMAGE_TOKEN (IMAGE_DIALOG_BUTTON_DOWN      ), NULL},
  {L"DialogButtonUp",        IMAGE_TOKEN (IMAGE_DIALOG_BUTTON_UP        ), NULL},
  {L"DialogSelectedIcon",    IMAGE_TOKEN (IMAGE_DIALOG_SELECTED_ICON    ), NULL},
  {L"DialogSortIcon",        IMAGE_TOKEN (IMAGE_DIALOG_SORT_ICON        ), NULL},
  {L"ScrollbarThumb",        IMAGE_TOKEN (IMAGE_SCROLLBAR_THUMB         ), NULL},
  {L"ScrollbarBackground",   IMAGE_TOKEN (IMAGE_SCROLLBAR_BACKGROUND    ), NULL},
  {L"SetupMenuTextOpBkg",    IMAGE_TOKEN (IMAGE_SETUP_MENU_TEXT_OP_BKG  ), NULL},
  {L"FormTitleBkg",          IMAGE_TOKEN (IMAGE_FORM_TITLE_BKG          ), NULL},
  {L"OptionBkg",             IMAGE_TOKEN (IMAGE_OPTION_BKG              ), NULL},
  {L"OwnerDrawBkg",          IMAGE_TOKEN (IMAGE_OWNER_DRAW_BKG          ), NULL},
  {L"CheckBoxBkg",           IMAGE_TOKEN (IMAGE_CHECK_BOX_BKG           ), NULL},
  {L"CheckBoxBorder",        IMAGE_TOKEN (IMAGE_CHECK_BOX_BORDER        ), NULL},
  {L"CheckBoxButton",        IMAGE_TOKEN (IMAGE_CHECK_BOX_BUTTON        ), NULL},
  {L"FrontPageBkg",          IMAGE_TOKEN (IMAGE_FRONT_PAGE_BKG          ), NULL},
  {NULL, 0, NULL}
  };

EFI_IMAGE_INPUT *
GetImageByString (
  CHAR16                        *Name
  )
{
  UINTN                         Index;
  EFI_STATUS                    Status;
  EFI_IMAGE_INPUT               *ImagePtr;

  ImagePtr = NULL;
  if (Name[0] == '@') {
    Name++;
    for (Index = 0; mImageResource[Index].Name != NULL; Index++) {
      if (StrCmp (mImageResource[Index].Name, Name) == 0) {
        break;
      }
    }
    ASSERT (mImageResource[Index].Name != NULL);
    if (mImageResource[Index].Name == NULL) {
      return NULL;
    }

    if (mImageResource[Index].ImageIn != NULL) {
      ImagePtr = mImageResource[Index].ImageIn;
    } else {
      ImagePtr = AllocateZeroPool (sizeof (EFI_IMAGE_INPUT));
      Status = gHiiImage->GetImage (
                            gHiiImage,
                            mImageHiiHandle,
                            mImageResource[Index].ImageId,
                            ImagePtr
                            );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)){
        FreePool (ImagePtr);
        return NULL;
      }
      mImageResource[Index].ImageIn = ImagePtr;
    }
  } else {
    ImagePtr = (EFI_IMAGE_INPUT *)(UINTN) StrToUInt (Name, 16, &Status);
    ASSERT_EFI_ERROR (Status);
    ASSERT (ImagePtr != NULL);
  }

  if (ImagePtr != NULL) {
    //
    // We only convert transparent image once.
    //
    if ((ImagePtr->Flags & EFI_IMAGE_TRANSPARENT) == EFI_IMAGE_TRANSPARENT) {
      ConvertToAlphaChannelImage (ImagePtr);
      ImagePtr->Flags &= ~EFI_IMAGE_TRANSPARENT;
    }
  }

  return ImagePtr;

}

VOID
AddHiiImagePackage (
  VOID
  )
{
  if (mImageHiiHandle == NULL) {
    mImageHiiHandle = HiiAddPackages (&gEfiCallerIdGuid, NULL, IMAGE_ARRAY_NAME, NULL);
    ASSERT (mImageHiiHandle != NULL);
  }
}
