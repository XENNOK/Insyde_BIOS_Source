/** @file

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

#ifndef  _SMM_PASSWORD_DIALOG_
#define  _SMM_PASSWORD_DIALOG_

#include <Uefi.h>

/**
 Display PaswordDialog then wait user input key.

 @param [in]   InputBufferLen   The input string buffer length.
 @param [in, out] StringBuffer  The input string buffer address.

 @retval None.

**/
VOID
PaswordDialog(
  IN UINTN InputBufferLen,
  IN OUT CHAR16 *StringBuffer
  );

/**
 Display SelectDialog then wait user select item.

 @param [in]   StringArrey      The item buffer address array of string item.
 @param [in]   NumOfItem        Number of item.
 @param [in]   MaxStringLen     The maximum string length of item.
 @param [in]   TitilString      The dialog title string.
 @param [in, out] SelectIndex   User or application select item number.
 @param [out]  EventKey         Report input key

 @retval None.

**/
VOID
SelectDialog(
  IN     CHAR16           **StringArrey,
  IN     UINTN            NumOfItem,
  IN     UINTN            MaxStringLen,
  IN     CHAR16           *TitilString,
  IN OUT UINTN            *SelectIndex,
     OUT EFI_INPUT_KEY    *EventKey
  );
#endif

