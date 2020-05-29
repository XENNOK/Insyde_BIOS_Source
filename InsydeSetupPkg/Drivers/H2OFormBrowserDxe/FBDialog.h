/** @file

Dialog for formBrowser
;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _FB_DIALOG_H_
#define _FB_DIALOG_H_

#include "InternalH2OFormBrowser.h"

typedef enum {
  OkButton,
  YesNoButton,
  YesNoCancelButton,
  ButtonTypeMax
} BUTTON_TYPE;

typedef enum {
  ButtonActionYes,
  ButtonActionNo,
  ButtonActionCancel,
  ButtonActionMax
} BUTTON_ACTION;

EFI_STATUS
CreateNewDialog (
  IN     UINT32                               DialogType,
  IN     UINT32                               Attribute,
  IN     CHAR16                               *TitleString,
  IN     UINT32                               BodyCount,
  IN     UINT32                               BodyInputCount,
  IN     UINT32                               ButtonCount,
  IN     CHAR16                               **BodyStringArray,
  IN     CHAR16                               **BodyInputStringArray,
  IN     CHAR16                               **ButtonStringArray,
  IN     EFI_HII_VALUE                        *BodyHiiValueArray,
  IN     EFI_HII_VALUE                        *ButtonHiiValueArray,
  OUT    H2O_FORM_BROWSER_D                   *Dialog
  );

EFI_STATUS
CreateSimpleDialog (
  IN     UINT32                               DialogType,
  IN     UINT32                               Attribute  OPTIONAL,
  IN     CHAR16                               *TitleString,
  IN     UINT32                               BodyCount,
  IN     CHAR16                               **BodyStringArray,
  IN     UINT32                               ButtonCount,
  IN OUT H2O_FORM_BROWSER_D                   *Dialog
  );

EFI_STATUS
BroadcastOpenDWithHelpMsg (
  VOID
  );

EFI_STATUS
BroadcastOpenDByHotKeyEvt (
  IN     UINT32                              SendEvtType
  );

EFI_STATUS
BroadcastOpenDByQ (
  IN       H2O_FORM_BROWSER_Q                 *Question
  );

EFI_STATUS
ShowPwdStatusMessage (
  IN UINT32                                   Attribute  OPTIONAL,
  IN EFI_STATUS                               PwdStatus
  );
#endif
