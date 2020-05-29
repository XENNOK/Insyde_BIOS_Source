/** @file
 Define function of value operation .  
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "InternalH2OFormBrowser.h"

/**
  Get password state
  
  @param  [in] Selection         Current selection.
  @param  [in] Question          Current question.

  @retval EFI_SUCCESS            Get password state success.
  @retval EFI_NOT_AVAILABLE_YET  Callback is not supported.
  @retval EFI_CRC_ERROR          Old password exist, ask user for the old password

**/
PASSWORD_STATE
GetPasswordState (
  IN UI_MENU_SELECTION            *Selection,
  IN H2O_FORM_BROWSER_Q           *Question
  )
{
  EFI_STATUS                      Status;
  CHAR16                          StringPtr[1] = {0};

  if (Selection == NULL || Question == NULL) {
    return PasswordStateMax;
  }
   
  if ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) != 0) {
    Status = PasswordCallback (Selection, StringPtr);
    if (Status == EFI_NOT_AVAILABLE_YET) {
      //
      // Callback is not supported, or
      // Callback request to terminate password input
      //
      FreePool (StringPtr);
      return PasswordStateMax;
    }

    if (EFI_ERROR (Status)) {
      if (Status != EFI_CRC_ERROR) {
        //
        // Old password exist, ask user for the old password
        //
        return ModifyPasswordState;
      } else {
        return UnlockPasswordState;
      }
    }
  } else {
    if (*((CHAR16 *) Question->HiiValue.Buffer) != 0) {
      return ModifyPasswordState;
    }
  }

  return NewPasswordState;
}

/**
  Confirm password 
  
  @param  [in] Selection         Current selection.
  @param  [in] Question          Current question.
  @param  [in] PasswordStr       Input password string.

  @retval EFI_SUCCESS            Get password state success.
  @retval EFI_ABORTED  Callback is not supported.
  @retval EFI_CRC_ERROR          Old password exist, ask user for the old password

**/
EFI_STATUS
ConfirmPassword (
  IN UI_MENU_SELECTION            *Selection,
  IN H2O_FORM_BROWSER_Q           *Question,
  IN CHAR16                       *PasswordStr
  )
{
  EFI_STATUS                      Status;
  UINT16                          Maximum;
  CHAR16                          *OrgPassword;

  if (Selection == NULL || Question == NULL || PasswordStr == NULL) {
    return EFI_ABORTED;
  }

  Status = EFI_SUCCESS;
  
  if ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK)  != 0) {
    Status = PasswordCallback (Selection, PasswordStr);
  } else {
    //
    // For non-interactive password, validate old password in local
    //
    if (*((CHAR16 *) Question->HiiValue.Buffer) != 0) {
      //
      // There is something there!  Prompt for password
      //
      Maximum = (UINT16) Question->Maximum;
      
      OrgPassword          = AllocateCopyPool ((Maximum + 1) * sizeof (CHAR16), Question->HiiValue.Buffer);
      OrgPassword[Maximum] = L'\0';

      if (StrCmp (PasswordStr, OrgPassword) != 0) {
        Status = EFI_ABORTED;
      }

      gBS->FreePool (OrgPassword);
    }
  }

  return Status;
}



/**
  Password may be stored as encrypted by Configuration Driver. When change a
  password, user will be challenged with old password. To validate user input old
  password, we will send the clear text to Configuration Driver via Callback().
  Configuration driver is responsible to check the passed in password and return
  the validation result. If validation pass, state machine in password Callback()
  will transit from BROWSER_STATE_VALIDATE_PASSWORD to BROWSER_STATE_SET_PASSWORD.
  After user type in new password twice, Callback() will be invoked to send the
  new password to Configuration Driver.

  @param  [in] Selection         Pointer to UI_MENU_SELECTION.
  @param  [in] String            The clear text of password.

  @retval EFI_NOT_AVAILABLE_YET  Callback() request to terminate password input.
  @return In state of BROWSER_STATE_VALIDATE_PASSWORD:
  @retval EFI_SUCCESS            Password correct, Browser will prompt for new
                                 password.
  @retval EFI_NOT_READY          Password incorrect, Browser will show error
                                 message.
  @retval Other                  Browser will do nothing.
  @return In state of BROWSER_STATE_SET_PASSWORD:
  @retval EFI_SUCCESS            Set password success.
  @retval Other                  Set password failed.

**/
EFI_STATUS
PasswordCallback (
  IN  UI_MENU_SELECTION           *Selection,
  IN  CHAR16                      *String
  )
{
  EFI_STATUS                      Status;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_BROWSER_ACTION_REQUEST      ActionRequest;
  EFI_IFR_TYPE_VALUE              IfrTypeValue;

  ConfigAccess = Selection->FormSet->ConfigAccess;
  if (ConfigAccess == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // Prepare password string in HII database
  //
  if (String != NULL) {
    IfrTypeValue.string = NewString (String, Selection->FormSet->HiiHandle);
  } else {
    IfrTypeValue.string = 0;
  }

  //
  // Send password to Configuration Driver for validation
  //
  Status = ConfigAccess->Callback (
                           ConfigAccess,
                           EFI_BROWSER_ACTION_CHANGED,
                           Selection->QuestionId,
                           EFI_IFR_TYPE_STRING,
                           &IfrTypeValue,
                           &ActionRequest
                           );

  //
  // Remove password string from HII database
  //
  if (String != NULL) {
    DeleteString (IfrTypeValue.string, Selection->FormSet->HiiHandle);
  }

  return Status;
}

