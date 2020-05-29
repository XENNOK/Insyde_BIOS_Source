//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    OemFormBrowser2.h

Abstract:

  The EFI_FORM_BROWSER_PROTOCOL is the interface to the EFI
  Configuration Driver.  This will allow the caller to direct the
  configuration driver to use either the HII database or use the passed
  in packet of data.  This will also allow the caller to post messages
  into the configuration drivers internal mailbox.

--*/

#ifndef _OEM_FORM_BROWSER2_H_
#define _OEM_FORM_BROWSER2_H_

#include "EfiHii.h"
#include EFI_PROTOCOL_CONSUMER (FormBrowser2)

#define EFI_OEM_FORM_BROWSER2_PROTOCOL_GUID  \
  { \
    0x2f1ca74f, 0xecfd, 0x43b6, 0x8c, 0xdc, 0xa4, 0x3c, 0x49, 0xf4, 0xf1, 0x13 \
  }

//
// Forward reference for pure ANSI compatability
//
EFI_FORWARD_DECLARATION (EFI_OEM_FORM_BROWSER2_PROTOCOL);

typedef enum {
  DlgYesNo,
  DlgYesNoCancel,
  DlgOk,
  DlgOkCancel
} UI_DIALOG_OPERATION;

typedef
EFI_STATUS
(EFIAPI *EFI_SEND_OEM_FORM) (
  IN  CONST EFI_OEM_FORM_BROWSER2_PROTOCOL *This,
  IN  EFI_HII_HANDLE                       *Handles,
  IN  UINTN                                HandleCount,
  IN  EFI_GUID                             *FormSetGuid, OPTIONAL
  IN  UINT16                               FormId, OPTIONAL
  IN  CONST EFI_SCREEN_DESCRIPTOR          *ScreenDimensions, OPTIONAL
  OUT EFI_BROWSER_ACTION_REQUEST           *ActionRequest  OPTIONAL
  )
/*++

Routine Description:
  This is the routine which an external caller uses to direct the browser
  where to obtain it's information.

Arguments:
  This        -     A pointer to the EFI_FORM_BROWSER2_PROTOCOL instance.
  Handles     -     A pointer to an array of HII handles to display.
  HandleCount -     The number of handles in the array specified by Handle.
  FormSetGuid -     This field points to the EFI_GUID which must match the Guid field in the EFI_IFR_FORM_SET op-code for the specified
                    forms-based package.   If FormSetGuid is NULL, then this function will display the first found forms package.
  FormId      -     This field specifies which EFI_IFR_FORM to render as the first displayable page.
                    If this field has a value of 0x0000, then the forms browser will render the specified forms in their encoded order.
  ScreenDimenions - This allows the browser to be called so that it occupies a portion of the physical screen instead of
                    dynamically determining the screen dimensions.
  ActionRequest -   Points to the action recommended by the form.

Returns:
  EFI_SUCCESS           -  The function completed successfully.
  EFI_INVALID_PARAMETER -  One of the parameters has an invalid value.
  EFI_NOT_FOUND         -  No valid forms could be found to display.

--*/
;


typedef
EFI_STATUS
(EFIAPI *EFI_CREATE_OEM_POP_UP) (
  IN  UI_DIALOG_OPERATION             DialogOperator,
  IN  BOOLEAN                         HotKey,
  IN  UINTN                           MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DIALOG_ICON) (
  IN  UINTN                           NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  UINTN                           MaximumStringSize,
  OUT CHAR16                          *UserInputStringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *TitleString,
  ...
  );
typedef
EFI_STATUS
(EFIAPI *EFI_OPTION_ICON) (
  IN  UINTN                           NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  EFI_INPUT_KEY                   *KeyList,
  OUT EFI_INPUT_KEY                   *EventKey,
  IN  UINTN                           MaximumStringSize,
  OUT CHAR16                          *TitleString,
  OUT UINTN                           *SelectIndex,
  IN  CHAR16                          **String,
  IN  UINTN                           Color
  );

typedef
EFI_STATUS
(EFIAPI *EFI_CREATE_MSG_POP_UP) (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       NumberOfLines,
  IN  CHAR16                      *ArrayOfStrings,
  ...
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SHOW_PAGE_INFORMATION) (
  IN   CHAR16          *TitleString,
  IN   CHAR16          *InfoStrings
  );

typedef struct _EFI_OEM_FORM_BROWSER2_PROTOCOL {
  EFI_SEND_OEM_FORM                  SendForm;
  EFI_CREATE_OEM_POP_UP              CreatePopUp;
  EFI_DIALOG_ICON                    DialogIcon;
  EFI_OPTION_ICON                    OptionIcon;
  EFI_CREATE_MSG_POP_UP              CreateMsgPopUp;
  EFI_SHOW_PAGE_INFORMATION          ShowPageInfo;
} EFI_OEM_FORM_BROWSER2_PROTOCOL;

extern EFI_GUID gEfiOemFormBrowser2ProtocolGuid;

#endif