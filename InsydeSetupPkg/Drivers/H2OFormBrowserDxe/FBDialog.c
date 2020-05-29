/** @file
Dialog for formbrowser
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

#include "InternalH2OFormBrowser.h"

extern FORM_DISPLAY_ENGINE_FORM *gFormData;

/**
 Get the Buffer value

 @param [in] Buffer         Buffer data
 @param [in] Type           Buffer data classified type
 @param [in] Index          Buffer data index

 @retval Value              Buffer value

**/
UINT64
GetBufferValue (
  IN     UINT8                                *Buffer,
  IN     UINT8                                Type,
  IN     UINTN                                Index
  )
{
  UINT8                                       *Ptr8;
  UINT16                                      *Ptr16;
  UINT32                                      *Ptr32;
  UINT64                                      *Ptr64;
  UINT64                                      Value;

  Value = 0;
  if (Type == EFI_IFR_TYPE_NUM_SIZE_8) {
    Ptr8 = (UINT8 *) Buffer;
    Value = (UINT64) Ptr8[Index];
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_16) {
    Ptr16 = (UINT16 *) Buffer;
    Value = (UINT64) Ptr16[Index];
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_32) {
    Ptr32 = (UINT32 *) Buffer;
    Value = (UINT64) Ptr32[Index];
  } else if (Type == EFI_IFR_TYPE_NUM_SIZE_64) {
    Ptr64 = (UINT64 *) Buffer;
    Value = (UINT64) Ptr64[Index];
  }

  return Value;
}

/**
 Create button data

 @param [in]  ButtonType            Button type
 @param [out] ButtonCount           The count of button
 @param [out] ButtonStringArray     Button string array
 @param [out] ButtonHiiValueArray   Button HII value array

 @retval EFI_SUCCESS                Create button data success

**/
STATIC
EFI_STATUS
CreateButtonData (
  IN  BUTTON_TYPE                         ButtonType,
  OUT UINT32                               *ButtonCount,
  OUT CHAR16                              ***ButtonStringArray,
  OUT EFI_HII_VALUE                       **ButtonHiiValueArray
  )
{
  UINT32                                   Count;
  CHAR16                                  **StrArray;
  EFI_HII_VALUE                           *HiiValueArray;


  if (ButtonType >= ButtonTypeMax || ButtonStringArray == NULL || ButtonHiiValueArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Count         = 0;
  StrArray      = NULL;
  HiiValueArray = NULL;

  switch (ButtonType) {

  case OkButton:
    Count         = 1;
    StrArray      = AllocateZeroPool (sizeof (CHAR16 *) * Count);
    HiiValueArray = AllocateZeroPool (sizeof (EFI_HII_VALUE) * Count);
    if (StrArray == NULL || HiiValueArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    StrArray[0] = GetString (STRING_TOKEN (STR_OK), mHiiHandle);
    if (StrArray[0] == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }


    CreateValueAsBoolean(HiiValueArray,TRUE);
    break;

  case YesNoButton:
    Count         = 2;
    StrArray      = AllocateZeroPool (sizeof (CHAR16 *) * Count);
    HiiValueArray = AllocateZeroPool (sizeof (EFI_HII_VALUE) * Count);
    if (StrArray == NULL || HiiValueArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    StrArray[0] = GetString (STRING_TOKEN (STR_YES), mHiiHandle);
    StrArray[1] = GetString (STRING_TOKEN (STR_NO), mHiiHandle);
    if (StrArray[0] == NULL || StrArray[1] == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }


    CreateValueAsBoolean(HiiValueArray,TRUE);
    CreateValueAsBoolean(HiiValueArray+1,FALSE);
    break;

  case YesNoCancelButton:
    Count         = 3;
    StrArray      = AllocateZeroPool (sizeof (CHAR16 *) * Count);
    HiiValueArray = AllocateZeroPool (sizeof (EFI_HII_VALUE) * Count);
    if (StrArray == NULL || HiiValueArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    StrArray[0] = GetString (STRING_TOKEN (STR_YES), mHiiHandle);
    StrArray[1] = GetString (STRING_TOKEN (STR_NO), mHiiHandle);
    StrArray[2] = GetString (STRING_TOKEN (STR_CANCEL), mHiiHandle);
    if (StrArray[0] == NULL || StrArray[1] == NULL || StrArray[2] == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }


    CreateValueAsUint64 (HiiValueArray, 1);
    CreateValueAsUint64 (HiiValueArray + 1, 0);
    CreateValueAsUint64 (HiiValueArray + 2, 2);
    break;

  default:
    return EFI_NOT_FOUND;
  }

  *ButtonCount         = Count;
  *ButtonStringArray   = StrArray;
  *ButtonHiiValueArray = HiiValueArray;

  return EFI_SUCCESS;
}

/**
 Show dialog of modify password

 @param [in]      Attribute          Attribute of color
 @param [in]      Question           Select question
 @param [in,out]  Dialog             Dialog information

 @retval Status                      Create dialog status

**/
STATIC
EFI_STATUS
ShowModifyPwdDialog (
  IN     UINT32                               Attribute OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                   *Question,
  IN OUT H2O_FORM_BROWSER_D                   *Dialog
  )
{
  EFI_STATUS                                  Status;
  UINT32                                       ButtonCount;           // The count of button
  CHAR16                                      **ButtonStrArray;       // The button string array
  EFI_HII_VALUE                               *ButtonHiiValueArray;   // The button HII value array
  UINT32                                       Index;                 // Index of each character of input box
  UINT32                                       BodyCount;             // The count of string body
  CHAR16                                      **BodyStrArray;         // String of string body
  CHAR16                                      **BodyInputStrArray;    // String for user input

  BodyCount         = 0;
  BodyStrArray      = NULL;
  BodyInputStrArray = NULL;

  Status = CreateButtonData (YesNoButton, &ButtonCount, &ButtonStrArray, &ButtonHiiValueArray);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BodyCount         = 3;
  BodyStrArray      = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
  BodyInputStrArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
  if (BodyStrArray == NULL || BodyInputStrArray == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  BodyStrArray[0] = GetString (STRING_TOKEN (STR_ENTER_OLD_PWD)      , mHiiHandle);
  BodyStrArray[1] = GetString (STRING_TOKEN (STR_ENTER_NEW_PWD)      , mHiiHandle);
  BodyStrArray[2] = GetString (STRING_TOKEN (STR_ENTER_NEW_PWD_AGAIN), mHiiHandle);
  if (BodyStrArray[0] == NULL      || BodyStrArray[1] == NULL      || BodyStrArray[2] == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  BodyInputStrArray[0] = gEmptyString;
  BodyInputStrArray[1] = gEmptyString;
  BodyInputStrArray[2] = gEmptyString;

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_PASSWORD << 16) | H2O_FORM_BROWSER_D_TYPE_SELECTION,
             Attribute,
             Question->Prompt,
             (UINT32) BodyCount,
             (UINT32) BodyCount,
             (UINT32) ButtonCount,
             BodyStrArray,
             BodyInputStrArray,
             ButtonStrArray,
             NULL,
             ButtonHiiValueArray,
             Dialog
             );

Done:
  for (Index = 0; Index < ButtonCount; Index++) {
    FreePool (ButtonStrArray[Index]);
  }
  FreePool (ButtonStrArray);
  FreePool (ButtonHiiValueArray);

  if (BodyStrArray != NULL) {
    for (Index = 0; Index < BodyCount; Index++) {
      FBFreePool((VOID**)&BodyStrArray[Index]);
    }
    FreePool (BodyStrArray);
  }

  FBFreePool((VOID**) &BodyInputStrArray);

  return Status;
}

/**
 Show dialog of new password

 @param [in]      Attribute          Attribute of color
 @param [in]      Question           Select question
 @param [in,out]  Dialog             Dialog information

 @retval Status                      Create new password dialog status

**/
STATIC
EFI_STATUS
ShowNewPwdDialog (
  IN     UINT32                               Attribute OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                   *Question,
  IN OUT H2O_FORM_BROWSER_D                   *Dialog
  )
{
  EFI_STATUS                                  Status;
  UINT32                                       ButtonCount;
  CHAR16                                      **ButtonStrArray;
  EFI_HII_VALUE                               *ButtonHiiValueArray;
  UINTN                                       Index;
  UINTN                                       BodyCount;
  CHAR16                                      **BodyStrArray;
  CHAR16                                      **BodyInputStrArray;

  BodyCount         = 0;
  BodyStrArray      = NULL;
  BodyInputStrArray = NULL;

  Status = CreateButtonData (YesNoButton, &ButtonCount, &ButtonStrArray, &ButtonHiiValueArray);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BodyCount         = 2;
  BodyStrArray      = AllocatePool (sizeof (CHAR16 *) * BodyCount);
  BodyInputStrArray = AllocatePool (sizeof (CHAR16 *) * BodyCount);
  if (BodyStrArray == NULL || BodyInputStrArray == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  BodyStrArray[0] = GetString (STRING_TOKEN (STR_ENTER_NEW_PWD)      , mHiiHandle);
  BodyStrArray[1] = GetString (STRING_TOKEN (STR_ENTER_NEW_PWD_AGAIN), mHiiHandle);
  if (BodyStrArray[0] == NULL      || BodyStrArray[1] == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  BodyInputStrArray[0] = gEmptyString;
  BodyInputStrArray[1] = gEmptyString;

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_PASSWORD << 16) | H2O_FORM_BROWSER_D_TYPE_SELECTION,
             Attribute,
             Question->Prompt,
             (UINT32) BodyCount,
             (UINT32) BodyCount,
             (UINT32) ButtonCount,
             BodyStrArray,
             BodyInputStrArray,
             ButtonStrArray,
             NULL,
             ButtonHiiValueArray,
             Dialog
             );

Done:
  for (Index = 0; Index < ButtonCount; Index++) {
    FreePool (ButtonStrArray[Index]);
  }
  FreePool (ButtonStrArray);
  FreePool (ButtonHiiValueArray);
  if (BodyStrArray != NULL) {
    for (Index = 0; Index < BodyCount; Index++) {
      if (BodyStrArray[Index] != NULL) {
        FreePool (BodyStrArray[Index]);
      }
    }
    FreePool (BodyStrArray);
  }
  FBFreePool((VOID**) &BodyInputStrArray);

  return Status;
}

/**
 Show dialog of unlock password

 @param [in]      Attribute          Attribute of color
 @param [in]      Question           Select question
 @param [in,out]  Dialog             Dialog information

 @retval Status                      Create unlock password dialog status

**/
STATIC
EFI_STATUS
ShowUnlockPwdDialog (
  IN     UINT32                               Attribute OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                   *Question,
  IN OUT H2O_FORM_BROWSER_D                   *Dialog
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       Index;
  UINTN                                       BodyCount;
  CHAR16                                      **BodyStrArray;
  CHAR16                                      **BodyInputStrArray;
  EFI_HII_VALUE                               HiiValue;

  BodyCount         = 0;
  BodyStrArray      = NULL;
  BodyInputStrArray = NULL;

  ZeroMem (&HiiValue, sizeof (EFI_HII_VALUE));
  HiiValue.Type      = EFI_IFR_TYPE_STRING;
  HiiValue.BufferLen = (UINT16) Question->Maximum;
  HiiValue.Buffer    = (UINT8 *)L"";

  BodyCount         = 1;
  BodyStrArray      = AllocatePool (sizeof (CHAR16 *) * BodyCount);
  BodyInputStrArray = AllocatePool (sizeof (CHAR16 *) * BodyCount);
  if (BodyStrArray == NULL || BodyInputStrArray == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  BodyStrArray[0] = GetString (STRING_TOKEN (STR_ENTER_OLD_PWD), mHiiHandle);
  if (BodyStrArray[0] == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  BodyInputStrArray[0] = gEmptyString;

  Status = CreateNewDialog (
             H2O_FORM_BROWSER_D_TYPE_MSG,
             Attribute,
             Question->Prompt,
             (UINT32) BodyCount,
             (UINT32) BodyCount,
             0,
             BodyStrArray,
             BodyInputStrArray,
             NULL,
             &HiiValue,
             NULL,
             Dialog
             );

Done:
  if (BodyStrArray != NULL) {
    for (Index = 0; Index < BodyCount; Index++) {
      if (BodyStrArray[Index] != NULL) {
        FreePool (BodyStrArray[Index]);
      }
    }
    FreePool (BodyStrArray);
  }
  if (BodyInputStrArray != NULL) {
    FreePool (BodyInputStrArray);
  }

  return Status;
}
/**
 Show password status message

 @param [in]  Attribute            Attribute of color
 @param [in]  PwdStatus            Password status

 @retval Status                    Password status

**/
EFI_STATUS
ShowPwdStatusMessage (
  IN UINT32                                   Attribute  OPTIONAL,
  IN EFI_STATUS                               PwdStatus
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_D                          Dialog;
  UINT32                                      DialogType;
  CHAR16                                      *TitleStr;
  UINT32                                      BodyCount;
  CHAR16                                      **BodyStrArray;
  UINT32                                      ButtonCount;
  UINTN                                       Index;

  BodyStrArray = NULL;
  DialogType   = H2O_FORM_BROWSER_D_TYPE_SELECTION;
  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));

  switch (PwdStatus) {

  case EFI_SUCCESS:
    TitleStr        = GetString (STRING_TOKEN (SETUP_NOTICE_STRING), mHiiHandle);
    ButtonCount     = 1;
    BodyCount       = 1;
    BodyStrArray    = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStrArray != NULL) {
      BodyStrArray[0] = GetString (STRING_TOKEN (PASSWORD_CHANGES_SAVED_STRING), mHiiHandle);
    }
    break;

  case EFI_UNSUPPORTED:
    TitleStr        = GetString (STRING_TOKEN (SETUP_WARNING_STRING), mHiiHandle);
    ButtonCount     = 1;
    BodyCount       = 1;
    BodyStrArray    = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStrArray != NULL) {
      BodyStrArray[0] = GetString (STRING_TOKEN (PASSWORD_SAME_ERROR), mHiiHandle);
    }
    break;

  case EFI_NOT_READY:
    TitleStr        = GetString (STRING_TOKEN (SETUP_WARNING_STRING), mHiiHandle);
    ButtonCount     = 1;
    BodyCount       = 1;
    BodyStrArray    = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStrArray != NULL) {
      BodyStrArray[0] = GetString (STRING_TOKEN (PASSWORD_INVALID), mHiiHandle);
    }
    break;

  case EFI_ABORTED:
    TitleStr        = GetString (STRING_TOKEN (SETUP_WARNING_STRING), mHiiHandle);
    ButtonCount     = 1;
    BodyCount       = 1;
    BodyStrArray    = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStrArray != NULL) {
      BodyStrArray[0] = GetString (STRING_TOKEN (MINI_STRING), mHiiHandle);
    }
    break;

  default:
    TitleStr        = GetString (STRING_TOKEN (SETUP_WARNING_STRING), mHiiHandle);
    ButtonCount     = 1;
    BodyCount       = 1;
    BodyStrArray    = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStrArray != NULL) {
      BodyStrArray[0] = GetString (STRING_TOKEN (CONFIRM_ERROR), mHiiHandle);
    }
    break;
  }

  Status = CreateSimpleDialog (
             DialogType,
             Attribute,
             TitleStr,
             BodyCount,
             BodyStrArray,
             ButtonCount,
             &Dialog
             );


  FBFreePool((VOID**)&TitleStr);
  if (BodyStrArray != NULL) {
    for (Index = 0; Index < BodyCount; Index++) {

    FBFreePool((VOID**)(&BodyStrArray[Index]));
    }
    FreePool (BodyStrArray);
  }

  return Status;
}

/**
 FormBrowser dialog event callback function

 @param [in]      Event            Display engine event type
 @param [in,out]  Dialog           Dialog information
 @param [in]      Exit             Whether exit the dialog

 @retval Status                    Dialog event status

**/
STATIC
EFI_STATUS
FBDialogEventCallback (
  IN CONST H2O_DISPLAY_ENGINE_EVT        *Event,
  IN OUT   H2O_FORM_BROWSER_D            *Dialog,
  OUT      BOOLEAN                       *Exit
  )
{
  EFI_STATUS                             Status;
  H2O_FORM_BROWSER_PRIVATE_DATA          *Private;

  H2O_DISPLAY_ENGINE_EVT_KEYPRESS        *KeyPress;
  H2O_DISPLAY_ENGINE_EVT_SHUT_D          ShutD;
  H2O_DISPLAY_ENGINE_EVT_CHANGE_Q        *ChangeQ;

  Private = &mFBPrivate;

  Status = EFI_SUCCESS;

//  DEBUG ((EFI_D_ERROR, "%s\n", EventToStr (Event->Type)));

  switch (Event->Type) {

  case H2O_DISPLAY_ENGINE_EVT_TYPE_KEYPRESS:
    KeyPress = (H2O_DISPLAY_ENGINE_EVT_KEYPRESS *) Event;
    if (KeyPress->KeyData.Key.ScanCode != SCAN_F1) {
      FBKeyPress (Private, Event);
    }
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_REL_PTR_MOVE:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_ABS_PTR_MOVE:
    //
    // BugBug this event need pass to display engine
    // BUGBUG the event need pass to local display engine or remote display engine by console pair,
    //        not use hot key to deteminate event transmit
    //
    Status = Private->ActivatedEngine->Notify (Private->ActivatedEngine, Event);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_D:
  case H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGING_Q:
    FBBroadcastEvent (Event);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGE_Q:
    ChangeQ = (H2O_DISPLAY_ENGINE_EVT_CHANGE_Q *)Event;
    CopyMem (&Dialog->ConfirmHiiValue, &ChangeQ->HiiValue, sizeof (EFI_HII_VALUE));
    if (ChangeQ->HiiValue.Type == EFI_IFR_TYPE_STRING) {
      Dialog->ConfirmHiiValue.Buffer = ChangeQ->HiiValue.Buffer;
    }

    ZeroMem (&ShutD, sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_D));
    ShutD.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_D);
    ShutD.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_BROADCAST;
    ShutD.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D;
    FBBroadcastEvent ((H2O_DISPLAY_ENGINE_EVT*)&ShutD);

    *Exit = TRUE;
    Status = EFI_SUCCESS;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D:
    ZeroMem (&ShutD, sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_D));
    ShutD.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_SHUT_D);
    ShutD.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_BROADCAST;
    ShutD.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_SHUT_D;
    FBBroadcastEvent ((H2O_DISPLAY_ENGINE_EVT*)&ShutD);

    *Exit = TRUE;
    Status = EFI_NOT_READY;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_TIMER:
    Status = FBTimer (Private, Event);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_REFRESH_Q:
    Status = FBBroadcastEvent (Event);
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT:
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  return Status;

}
/**
 Confirm dialog value

 @param [in]      Question            Select question
 @param [in]      ConfirmHiiValue     Select HII value

 @retval Status                       Confirm dialog value status

**/
STATIC
EFI_STATUS
ConfirmDialogValue (
  IN     H2O_FORM_BROWSER_Q                  *Question,
  IN     EFI_HII_VALUE                       *ConfirmHiiValue
  )
{
  EFI_STATUS                                  Status;
  H2O_DISPLAY_ENGINE_EVT_CHANGE_Q             ChangeQ;

  Status = EFI_SUCCESS;
  //
  // CHANGE_Q
  //
  ZeroMem (&ChangeQ, sizeof (H2O_DISPLAY_ENGINE_EVT_CHANGE_Q));
  ChangeQ.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_CHANGE_Q);
  ChangeQ.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  ChangeQ.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_CHANGE_Q;
  ChangeQ.PageId     = Question->PageId;
  ChangeQ.QuestionId = Question->QuestionId;
  CopyMem (&ChangeQ.HiiValue, ConfirmHiiValue, sizeof (EFI_HII_VALUE));
  QueueEvent ((H2O_DISPLAY_ENGINE_EVT *) &ChangeQ);

  return Status;
}
/**
 Create new dialog

 @param [in]        DialogType            Dialog type
 @param [in]        Attribute             Attribute of color
 @param [in]        TitleString           String of title
 @param [in]        BodyStringCount       Count of body string
 @param [in]        BodyInputCount        Count of body input
 @param [in]        ButtonCount           Count of button
 @param [in]        BodyStringArray       String array of body
 @param [in]        BodyInputStringArray  Input string array of body
 @param [in]        ButtonStringArray     String Array of button
 @param [in]        BodyHiiValueArray     HII value array of body
 @param [in]        ButtonHiiValueArray   HII value array of button
 @param [in.out]    Dialog                Dialog information

 @retval Status                           Create new dialog status

**/
EFI_STATUS
CreateNewDialog (
  IN     UINT32                              DialogType,
  IN     UINT32                              Attribute,
  IN     CHAR16                              *TitleString,
  IN     UINT32                              BodyStringCount,
  IN     UINT32                              BodyInputCount,
  IN     UINT32                              ButtonCount,
  IN     CHAR16                              **BodyStringArray,
  IN     CHAR16                              **BodyInputStringArray,
  IN     CHAR16                              **ButtonStringArray,
  IN     EFI_HII_VALUE                       *BodyHiiValueArray,
  IN     EFI_HII_VALUE                       *ButtonHiiValueArray,
  IN OUT H2O_FORM_BROWSER_D                  *Dialog
  )
{
  EFI_STATUS                                 Status;
  H2O_DISPLAY_ENGINE_EVT_OPEN_D              OpenD;
  H2O_DISPLAY_ENGINE_EVT                     *Event;
  BOOLEAN                                    Exit;

  if (BodyStringCount == 0 && BodyInputCount == 0 && ButtonCount == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BodyStringCount != 0 && BodyStringArray == NULL) ||
    (BodyInputCount != 0 && BodyInputStringArray == NULL) ||
    (ButtonCount != 0 && ButtonStringArray == NULL)
    ){
    return EFI_INVALID_PARAMETER;
  }

  if ((DialogType & H2O_FORM_BROWSER_D_TYPE_MSG) == 0 &&
      BodyHiiValueArray == NULL &&
      ButtonHiiValueArray == NULL &&
      Dialog->ConfirmHiiValue.BufferLen == 0
      ) {
    return EFI_INVALID_PARAMETER;
  }

  Dialog->Signature = H2O_FORMSET_BROWSER_DIALOG_SIGNATURE;
  Dialog->DialogType = DialogType;
  Dialog->Attribute = Attribute;
  Dialog->TitleString = TitleString;
  Dialog->BodyStringCount = BodyStringCount;
  Dialog->BodyInputCount = BodyInputCount;
  Dialog->ButtonCount = ButtonCount;
  Dialog->BodyStringArray = BodyStringArray;
  Dialog->BodyInputStringArray = BodyInputStringArray;
  Dialog->ButtonStringArray = ButtonStringArray;
  Dialog->BodyHiiValueArray = BodyHiiValueArray;
  Dialog->ButtonHiiValueArray = ButtonHiiValueArray;

  //
  // OPEN_D
  //
  ZeroMem (&OpenD, sizeof (H2O_DISPLAY_ENGINE_EVT_OPEN_D));
  OpenD.Hdr.Size   = sizeof (H2O_DISPLAY_ENGINE_EVT_OPEN_D);
  OpenD.Hdr.Target = H2O_DISPLAY_ENGINE_EVT_TARGET_FORM_BROWSER;
  OpenD.Hdr.Type   = H2O_DISPLAY_ENGINE_EVT_TYPE_OPEN_D;
  CopyMem (&OpenD.Dialog, Dialog, sizeof (H2O_FORM_BROWSER_D));
  Status = FBBroadcastEvent ((H2O_DISPLAY_ENGINE_EVT *) &OpenD);

  //
  // event loop, it will process keyboard / mouse and dialog event
  //
  Event = NULL;
  Exit  = FALSE;
  Status = EFI_NOT_READY;

  do {
    if (GetNextEvent (&Event)) {
      Status = FBDialogEventCallback (Event, Dialog, &Exit);
      FreePool (Event);
    }
  } while (!Exit);

  return Status;
}
/**
 Create numeric dialog

 @param [in]        Attribute            Attribute of color
 @param [in]        Question             Select question

 @retval Status                          Create numeric dialog status

**/
STATIC
EFI_STATUS
CreateNumericDialog (
  IN     UINT32                              Attribute  OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                  *Question
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_D                          Dialog;
  UINT32                                       InputBoxWidth;
  UINT32                                       Index;
  CHAR16                                      *InputBoxStr;
  UINTN                                       InputBoxAddr[2];
  CHAR16                                      **ButtonStringArray;
  EFI_HII_VALUE                               *ButtonHiiValueArray;

  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));
  CopyMem (&Dialog.ConfirmHiiValue, &Question->HiiValue, sizeof (EFI_HII_VALUE));
  Dialog.H2OStatement = Question;

  ButtonStringArray = AllocatePool (sizeof (CHAR16 *) * 2);
  if (ButtonStringArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ButtonStringArray[0] = GetString (STRING_TOKEN (STR_YES), mHiiHandle);
  ButtonStringArray[1] = GetString (STRING_TOKEN (STR_NO), mHiiHandle);

  ButtonHiiValueArray = AllocateZeroPool (sizeof (EFI_HII_VALUE) * 2);
  if (ButtonHiiValueArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CreateValueAsBoolean (ButtonHiiValueArray, TRUE);
  CreateValueAsBoolean (ButtonHiiValueArray+1, FALSE);

  if ((Question->Flags & EFI_IFR_DISPLAY_UINT_HEX) == EFI_IFR_DISPLAY_UINT_HEX) {
    InputBoxWidth = (UINT32) DivU64x32 (Question->Maximum, 16) + 1;
  } else {
    InputBoxWidth = (UINT32) DivU64x32 (Question->Maximum, 10) + 1;
  }
  if (InputBoxWidth > 10) {
    InputBoxWidth = 10;
  }

  InputBoxStr = AllocateZeroPool (sizeof (CHAR16) * (InputBoxWidth + 1));
  if (InputBoxStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; Index < InputBoxWidth; Index ++) {
    InputBoxStr[Index] = ' ';
  }
  InputBoxAddr[0] = (UINTN)(UINTN *) CatSPrint (NULL, InputBoxStr);

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_NUMERIC << 16) | H2O_FORM_BROWSER_D_TYPE_SELECTION,
             Attribute,
             NULL,
             0,
             1,
             2,
             NULL,
             (CHAR16 **) InputBoxAddr,
             ButtonStringArray,
             &Question->HiiValue,
             ButtonHiiValueArray,
             &Dialog
             );

  if (!EFI_ERROR (Status)) {
    Status = ConfirmDialogValue (Question, &Dialog.ConfirmHiiValue);
  } else if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    Status = EFI_SUCCESS;
  }

  FreePool (InputBoxStr);
  FreePool ((UINTN *) InputBoxAddr[0]);
  FreePool (ButtonStringArray[0]);
  FreePool (ButtonStringArray[1]);
  FreePool (ButtonStringArray);
  FreePool (ButtonHiiValueArray);
  return Status;
}
/**
 Create oneof dialog

 @param [in]        Attribute            Attribute of color
 @param [in]        Question             Select question

 @retval Status                          Create oneof dialog status

**/
STATIC
EFI_STATUS
CreateOneOfDialog (
  IN     UINT32                              Attribute OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                  *Question
  )
{
  EFI_STATUS                                  Status;

  H2O_FORM_BROWSER_D                          Dialog;
  H2O_FORM_BROWSER_O                          *Option;
  UINTN                                       Index;

  UINTN                                       BodyCount;
  CHAR16                                      **BodyStringArray;
  EFI_HII_VALUE                               *BodyHiiValueArray;


  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));
  CopyMem (&Dialog.ConfirmHiiValue, &Question->HiiValue, sizeof(EFI_HII_VALUE));

  //
  // Get count of options
  //
  BodyCount = 0;
  for (Index = 0; Index < Question->NumberOfOptions; Index++) {
    if (Question->Options[Index].Visibility) {
      BodyCount++;
    }
  }

  if (BodyCount == 0) {
    return EFI_INVALID_PARAMETER;
  }

  BodyStringArray   = AllocatePool (sizeof (CHAR16 *) * (BodyCount + 1));
  BodyHiiValueArray = AllocatePool (sizeof (EFI_HII_VALUE) * BodyCount);
  if (BodyStringArray == NULL || BodyHiiValueArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BodyCount = 0;
  for (Index = 0; Index < Question->NumberOfOptions; Index++) {
    Option = &Question->Options[Index];
    if (Option->Visibility) {
      BodyStringArray[BodyCount] = Option->Text;
      CopyMem (&BodyHiiValueArray[BodyCount], &Option->HiiValue, sizeof (EFI_HII_VALUE));
      BodyCount++;
    }
  }
  BodyStringArray[BodyCount] = NULL;

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_ONE_OF << 16) | H2O_FORM_BROWSER_D_TYPE_BODY_SELECTABLE,
             Attribute,
             Question->Prompt,
             (UINT32) BodyCount,
             0,
             0,
             BodyStringArray,
             NULL,
             NULL,
             BodyHiiValueArray,
             NULL,
             &Dialog
             );

  if (!EFI_ERROR (Status)) {
    Status = ConfirmDialogValue (Question, &Dialog.ConfirmHiiValue);
  } else if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    Status = EFI_SUCCESS;
  }

  FreePool (BodyStringArray);
  FreePool (BodyHiiValueArray);

  return Status;
}
/**
 Create ordered list dialog

 @param [in]        Attribute            Attribute of color
 @param [in]        Question             Select question

 @retval Status                          Create ordered list dialog status

**/
STATIC
EFI_STATUS
CreateOrderedListDialog (
  IN     UINT32                              Attribute OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                  *Question
  )
{
  EFI_STATUS                                  Status;

  H2O_FORM_BROWSER_D                          Dialog;
  H2O_FORM_BROWSER_O                          *Option;
  UINTN                                       OptionIndex;
  UINTN                                       ContainerIndex;

  UINTN                                       BodyCount;
  CHAR16                                      **BodyStringArray;
  EFI_HII_VALUE                               *BodyHiiValueArray;


  if (Question->HiiValue.Buffer == NULL || Question->ContainerCount == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));

  //
  // Get count of options
  //
  BodyCount = 0;
  for (OptionIndex = 0; OptionIndex < Question->NumberOfOptions; OptionIndex++) {
    if (Question->Options[OptionIndex].Visibility) {
      BodyCount++;
    }
  }

  if (BodyCount == 0) {
    return EFI_INVALID_PARAMETER;
  }

  BodyStringArray   = AllocatePool (sizeof (CHAR16 *) * (BodyCount + 1));
  BodyHiiValueArray = AllocatePool (sizeof (EFI_HII_VALUE) * BodyCount);
  if (BodyStringArray == NULL || BodyHiiValueArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BodyCount = 0;
  for (ContainerIndex = 0; ContainerIndex < Question->ContainerCount; ContainerIndex ++) {
    for (OptionIndex = 0; OptionIndex < Question->NumberOfOptions; OptionIndex++) {
      Option = &Question->Options[OptionIndex];
      if (Option->Visibility && Option->HiiValue.Value.u64 == GetBufferValue (Question->HiiValue.Buffer, Option->HiiValue.Type, ContainerIndex)) {
        BodyStringArray[BodyCount] = Option->Text;
        CopyMem (&BodyHiiValueArray[BodyCount], &Option->HiiValue, sizeof (EFI_HII_VALUE));
        BodyCount++;
        break;
      }
    }
  }
  BodyStringArray[BodyCount] = NULL;

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_ORDERED_LIST << 16) | H2O_FORM_BROWSER_D_TYPE_BODY_SELECTABLE,
             Attribute,
             NULL,
             (UINT32) BodyCount,
             0,
             0,
             BodyStringArray,
             NULL,
             NULL,
             BodyHiiValueArray,
             NULL,
             &Dialog
             );

  if (!EFI_ERROR (Status)) {
    Status = ConfirmDialogValue (Question, &Dialog.ConfirmHiiValue);
  } else if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    Status = EFI_SUCCESS;
  }

  FreePool (BodyStringArray);
  FreePool (BodyHiiValueArray);

  return Status;
}
/**
 Create string dialog

 @param [in]        Attribute            Attribute of color
 @param [in]        Question             Select question

 @retval Status                          Create string dialog status

**/
STATIC
EFI_STATUS
CreateStringDialog (
  IN     UINT32                              Attribute OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                  *Question
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_D                          Dialog;
  CHAR16                                      *InputBoxAddr[2];
  CHAR16                                      **BodyInputStringArray;
  CHAR16                                      **ButtonStringArray;
  EFI_HII_VALUE                               *ButtonHiiValueArray;
  CHAR16                                      *YesNoButton[2];
  EFI_HII_VALUE                               YesNoButtonHiiValue[2];


  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));
  CopyMem (&Dialog.ConfirmHiiValue, &Question->HiiValue, sizeof (EFI_HII_VALUE));
  Dialog.H2OStatement = Question;

  YesNoButton[0] = GetString (STRING_TOKEN (STR_YES), mHiiHandle);
  YesNoButton[1] = GetString (STRING_TOKEN (STR_NO), mHiiHandle);
  if (YesNoButton[0] == NULL || YesNoButton[1] == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CreateValueAsBoolean (YesNoButtonHiiValue, TRUE);
  CreateValueAsBoolean (YesNoButtonHiiValue + 1, FALSE);

  InputBoxAddr[0] = AllocateCopyPool (Question->HiiValue.BufferLen, Question->HiiValue.Buffer);
  if (InputBoxAddr[0] == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BodyInputStringArray = AllocateCopyPool (sizeof (CHAR16 *)         , InputBoxAddr);
  ButtonStringArray    = AllocateCopyPool (sizeof (CHAR16 *) * 2     , YesNoButton);
  ButtonHiiValueArray  = AllocateCopyPool (sizeof (EFI_HII_VALUE) * 2, &YesNoButtonHiiValue);
  if (BodyInputStringArray == NULL || ButtonStringArray == NULL || ButtonHiiValueArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_STRING << 16) | H2O_FORM_BROWSER_D_TYPE_SELECTION,
             Attribute,
             Question->Prompt,
             0,
             1,
             2,
             NULL,
             BodyInputStringArray,
             ButtonStringArray,
             &Question->HiiValue,
             ButtonHiiValueArray,
             &Dialog
             );
  if (!EFI_ERROR (Status)) {
    Status = ConfirmDialogValue (Question, &Dialog.ConfirmHiiValue);
  } else if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    Status = EFI_SUCCESS;
  }

  FreePool (YesNoButton[0]);
  FreePool (YesNoButton[1]);
  FreePool ((CHAR16 *) InputBoxAddr[0]);
  FreePool (BodyInputStringArray);
  FreePool (ButtonStringArray);
  FreePool (ButtonHiiValueArray);

  return Status;
}

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
  IN FORM_DISPLAY_ENGINE_FORM      *Form,
  IN FORM_DISPLAY_ENGINE_STATEMENT *Statement
  )
{
  EFI_STATUS                      Status;
  CHAR16                          StringPtr[1] = {0};

  ASSERT (Form != NULL);
  ASSERT (Statement != NULL);

  if (Form == NULL || Statement == NULL) {
    return PasswordStateMax;
  }

  Status = PasswordCheck (Form, Statement, StringPtr);
  if (Status == EFI_NOT_AVAILABLE_YET || Status == EFI_UNSUPPORTED) {
    return PasswordStateMax;
  }

  if (Status == EFI_CRC_ERROR) {
    return UnlockPasswordState;
  } else if (EFI_ERROR (Status)) {
    return ModifyPasswordState;
  }

  return NewPasswordState;
}

/**
 Create password dialog

 @param [in]        Attribute            Attribute of color
 @param [in]        Question             Select question

 @retval Status                          Create password dialog status

**/
STATIC
EFI_STATUS
CreatePasswordDialog (
  IN UINT32                                   Attribute OPTIONAL,
  IN H2O_FORM_BROWSER_Q                       *Question
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_PRIVATE_DATA               *Private;
  H2O_FORM_BROWSER_D                          Dialog;
  PASSWORD_STATE                              PasswordState;
  CHAR16                                      *OldPasswordStr;
  CHAR16                                      *NewPasswordStr;
  CHAR16                                      *ConfirmNewPasswordStr;
  UINTN                                       OldPasswordStrSize;
  UINTN                                       NewPasswordStrSize;
  FORM_DISPLAY_ENGINE_STATEMENT               Statement;

  Status  = EFI_UNSUPPORTED;
  Private = &mFBPrivate;
  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));
  CopyMem (&Dialog.ConfirmHiiValue, &Question->HiiValue, sizeof (EFI_HII_VALUE));

  Statement.OpCode = Question->IfrOpCode;
  PasswordState = GetPasswordState (gFormData, &Statement);

  switch (PasswordState) {

  case ModifyPasswordState:
    Status = ShowModifyPwdDialog (Attribute, Question, &Dialog);
    if (!EFI_ERROR (Status)) {
      OldPasswordStr        = (CHAR16 *) (Dialog.ConfirmHiiValue.Buffer);
      OldPasswordStrSize    = StrSize (OldPasswordStr);
      NewPasswordStr        = (CHAR16 *) (Dialog.ConfirmHiiValue.Buffer + OldPasswordStrSize);
      NewPasswordStrSize    = StrSize (NewPasswordStr);
      ConfirmNewPasswordStr = (CHAR16 *) (Dialog.ConfirmHiiValue.Buffer + OldPasswordStrSize + NewPasswordStrSize);

      //
      // check old password
      //
      Status = PasswordCheck (gFormData, &Statement, OldPasswordStr);
      if (EFI_ERROR (Status)) {
        if (Status == EFI_NOT_READY) {
          ShowPwdStatusMessage (Attribute, EFI_NOT_READY);
        }
        FreePool (OldPasswordStr);
        break;
      }

      if (StrLen (NewPasswordStr) < Question->Minimum) {
        Status = EFI_ABORTED;
      } else if (StrCmp (NewPasswordStr, ConfirmNewPasswordStr) != 0) {
        Status = EFI_INVALID_PARAMETER;
      }

      if (EFI_ERROR (Status)) {
        ShowPwdStatusMessage (Attribute, Status);
        PasswordCheck (gFormData, &Statement, NULL);
        FreePool (OldPasswordStr);
        break;
      }

      Dialog.ConfirmHiiValue.Buffer    = AllocateCopyPool (NewPasswordStrSize, NewPasswordStr);
      Dialog.ConfirmHiiValue.BufferLen = (UINT16) NewPasswordStrSize;
      FreePool (OldPasswordStr);
    }
    break;

  case NewPasswordState:
    Status = ShowNewPwdDialog (Attribute, Question, &Dialog);
    if (EFI_ERROR (Status)) {
      //
      // Reset state machine for interactive password
      //
      Status = EFI_ABORTED;
      PasswordCheck (gFormData, &Statement, NULL);
      break;
    }

    NewPasswordStr        = (CHAR16 *) (Dialog.ConfirmHiiValue.Buffer);
    NewPasswordStrSize    = StrSize (NewPasswordStr);
    ConfirmNewPasswordStr = (CHAR16 *) (Dialog.ConfirmHiiValue.Buffer + NewPasswordStrSize);

    if (StrLen (NewPasswordStr) < Question->Minimum) {
      Status = EFI_ABORTED;
    } else if (StrCmp (NewPasswordStr, ConfirmNewPasswordStr) != 0) {
      Status = EFI_INVALID_PARAMETER;
    }

    if (EFI_ERROR (Status)) {
      ShowPwdStatusMessage (Attribute, Status);
      PasswordCheck (gFormData, &Statement, NULL);
      FreePool (NewPasswordStr);
      break;
    }

    Dialog.ConfirmHiiValue.Buffer    = AllocateCopyPool (NewPasswordStrSize, NewPasswordStr);
    Dialog.ConfirmHiiValue.BufferLen = (UINT16) NewPasswordStrSize;
    FreePool (NewPasswordStr);
    break;

  case UnlockPasswordState:
    Status = ShowUnlockPwdDialog (Attribute, Question, &Dialog);
    if (!EFI_ERROR (Status)) {
      Status = PasswordCheck (gFormData, &Statement, (CHAR16 *) Dialog.ConfirmHiiValue.Buffer);
      if (Status == EFI_NOT_READY) {
        ShowPwdStatusMessage (Attribute, EFI_NOT_READY);
      }
      FreePool (Dialog.ConfirmHiiValue.Buffer);
    }
    //
    // Unlock password no need to send change question event, Just return.
    // (Unlock password action only change the password state from lock state to validate state)
    //
    return EFI_SUCCESS;
    break;

  default:
    break;
  }

  if (!EFI_ERROR (Status)) {
    Status = ConfirmDialogValue (Question, &Dialog.ConfirmHiiValue);
  } else if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    Status = EFI_SUCCESS;
  }

  return Status;
}
/**
 Create date dialog

 @param [in]        Attribute            Attribute of color
 @param [in]        Question             Select question

 @retval Status                          Create date dialog status

**/
STATIC
EFI_STATUS
CreateDateDialog (
  IN     UINT32                              Attribute OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                  *Question
  )
{
  EFI_STATUS                                  Status;

  H2O_FORM_BROWSER_D                          Dialog;

  UINTN                                       BodyAddr[4];
  UINTN                                       InputBoxAddr[4];

  CHAR16                                      **BodyStringArray;
  CHAR16                                      **BodyInputStringArray;
  CHAR16                                      **ButtonStringArray;
  EFI_HII_VALUE                               *ButtonHiiValueArray;
  UINTN                                       YesNoButton[2];
  EFI_HII_VALUE                               YesNoButtonHiiValue[2];


  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));

  YesNoButton[0] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_YES), mHiiHandle);
  YesNoButton[1] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_NO), mHiiHandle);


  CreateValueAsBoolean (YesNoButtonHiiValue, TRUE);
  CreateValueAsBoolean (YesNoButtonHiiValue + 1, FALSE);

  BodyAddr[0] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_EMPTY), mHiiHandle);
  BodyAddr[1] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_SLASH), mHiiHandle);
  BodyAddr[2] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_SLASH), mHiiHandle);
  InputBoxAddr[0] = (UINTN)(UINTN *) CatSPrint (NULL, L"     ");
  InputBoxAddr[1] = (UINTN)(UINTN *) CatSPrint (NULL, L"   ");
  InputBoxAddr[2] = (UINTN)(UINTN *) CatSPrint (NULL, L"   ");

  BodyStringArray = AllocateCopyPool (sizeof (CHAR16 *) * 3, BodyAddr);
  BodyInputStringArray = AllocateCopyPool (sizeof (CHAR16 *) * 3, InputBoxAddr);

  ButtonStringArray = AllocateCopyPool (sizeof (CHAR16 *) * 2, YesNoButton);
  ButtonHiiValueArray = AllocateCopyPool (sizeof (EFI_HII_VALUE) * 2, &YesNoButtonHiiValue);

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_DATE << 16) | H2O_FORM_BROWSER_D_TYPE_BODY_HORIZONTAL | H2O_FORM_BROWSER_D_TYPE_SELECTION,
             Attribute,
             Question->Prompt,
             3,
             3,
             2,
             BodyStringArray,
             BodyInputStringArray,
             ButtonStringArray,
             NULL,
             ButtonHiiValueArray,
             &Dialog
             );

  if (!EFI_ERROR (Status)) {
    Status = ConfirmDialogValue (Question, &Dialog.ConfirmHiiValue);
  } else if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    Status = EFI_SUCCESS;
  }

  FreePool ((CHAR16 *) YesNoButton[0]);
  FreePool ((CHAR16 *) YesNoButton[1]);
  FreePool ((CHAR16 *) BodyAddr[0]);
  FreePool ((CHAR16 *) BodyAddr[1]);
  FreePool ((CHAR16 *) BodyAddr[2]);
  FreePool ((CHAR16 *) InputBoxAddr[0]);
  FreePool ((CHAR16 *) InputBoxAddr[1]);
  FreePool ((CHAR16 *) InputBoxAddr[2]);
  FreePool (BodyStringArray);
  FreePool (BodyInputStringArray);
  FreePool (ButtonStringArray);
  FreePool (ButtonHiiValueArray);

  return Status;
}
/**
 Create time dialog

 @param [in]        Attribute            Attribute of color
 @param [in]        Question             Select question

 @retval Status                          Create time dialog status

**/
STATIC
EFI_STATUS
CreateTimeDialog (
  IN     UINT32                              Attribute OPTIONAL,
  IN     H2O_FORM_BROWSER_Q                  *Question
  )
{
  EFI_STATUS                                  Status;

  H2O_FORM_BROWSER_D                          Dialog;

  UINTN                                       BodyAddr[4];
  UINTN                                       InputBoxAddr[4];

  CHAR16                                      **BodyStringArray;
  CHAR16                                      **BodyInputStringArray;
  CHAR16                                      **ButtonStringArray;
  EFI_HII_VALUE                               *ButtonHiiValueArray;
  UINTN                                       YesNoButton[2];
  EFI_HII_VALUE                               YesNoButtonHiiValue[2];


  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));

  YesNoButton[0] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_YES), mHiiHandle);
  YesNoButton[1] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_NO), mHiiHandle);


  CreateValueAsBoolean (YesNoButtonHiiValue, TRUE);
  CreateValueAsBoolean (YesNoButtonHiiValue + 1, FALSE);

  BodyAddr[0] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_EMPTY), mHiiHandle);
  BodyAddr[1] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_COLON), mHiiHandle);
  BodyAddr[2] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_COLON), mHiiHandle);
  InputBoxAddr[0] = (UINTN)(UINTN *) CatSPrint (NULL, L"  ");
  InputBoxAddr[1] = (UINTN)(UINTN *) CatSPrint (NULL, L"  ");
  InputBoxAddr[2] = (UINTN)(UINTN *) CatSPrint (NULL, L"  ");

  BodyStringArray = AllocateCopyPool (sizeof (CHAR16 *) * 3, BodyAddr);
  BodyInputStringArray = AllocateCopyPool (sizeof (CHAR16 *) * 3, InputBoxAddr);

  ButtonStringArray = AllocateCopyPool (sizeof (CHAR16 *) * 2, YesNoButton);
  ButtonHiiValueArray = AllocateCopyPool (sizeof (EFI_HII_VALUE) * 2, &YesNoButtonHiiValue);

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_TIME << 16) | H2O_FORM_BROWSER_D_TYPE_BODY_HORIZONTAL | H2O_FORM_BROWSER_D_TYPE_SELECTION,
             Attribute,
             Question->Prompt,
             3,
             3,
             2,
             BodyStringArray,
             BodyInputStringArray,
             ButtonStringArray,
             NULL,
             ButtonHiiValueArray,
             &Dialog
             );

  if (!EFI_ERROR (Status)) {
    Status = ConfirmDialogValue (Question, &Dialog.ConfirmHiiValue);
  } else if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    Status = EFI_SUCCESS;
  }

  FreePool ((CHAR16 *) YesNoButton[0]);
  FreePool ((CHAR16 *) YesNoButton[1]);
  FreePool ((CHAR16 *) BodyAddr[0]);
  FreePool ((CHAR16 *) BodyAddr[1]);
  FreePool ((CHAR16 *) BodyAddr[2]);
  FreePool ((CHAR16 *) InputBoxAddr[0]);
  FreePool ((CHAR16 *) InputBoxAddr[1]);
  FreePool ((CHAR16 *) InputBoxAddr[2]);
  FreePool (BodyStringArray);
  FreePool (BodyInputStringArray);
  FreePool (ButtonStringArray);
  FreePool (ButtonHiiValueArray);

  return Status;
}
/**
 Create simple dialog(For example:Esc,F9,F10)

 @param [in]            DialogType          Dialog type
 @param [in]            Attribute           Attribute of color
 @param [in]            TitleString         String of title
 @param [in]            BodyCount           The count of string body
 @param [in]            BodyStringArray     String of string body
 @param [in]            ButtonCount         The count of button
 @param [in.out]        Dialog              Dialog information

 @retval Status                          Create simple dialog status

**/
EFI_STATUS
CreateSimpleDialog (
  IN     UINT32                               DialogType,
  IN     UINT32                               Attribute OPTIONAL,
  IN     CHAR16                               *TitleString,
  IN     UINT32                               BodyCount,
  IN     CHAR16                               **BodyStringArray,
  IN     UINT32                               ButtonCount,
  IN OUT H2O_FORM_BROWSER_D                   *Dialog
  )
{
  EFI_STATUS                                  Status;

  CHAR16                                      **ButtonStringArray;
  EFI_HII_VALUE                               *ButtonHiiValueArray;

  UINTN                                       YesNoButton[3];
  EFI_HII_VALUE                               YesNoButtonHiiValue[3];
  UINTN                                       Index;


  Status = EFI_SUCCESS;

  if (ButtonCount > 3) {
    return EFI_INVALID_PARAMETER;
  }

  if (ButtonCount == 1) {
    //
    // Ok
    //
    YesNoButton[0] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_OK), mHiiHandle);
    CreateValueAsUint64 (YesNoButtonHiiValue, ButtonActionYes);

  } else if (ButtonCount == 2) {
    //
    // Yes / No
    //
    YesNoButton[0] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_YES), mHiiHandle);
    YesNoButton[1] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_NO), mHiiHandle);


    CreateValueAsUint64 (YesNoButtonHiiValue    , ButtonActionYes);
    CreateValueAsUint64 (YesNoButtonHiiValue + 1, ButtonActionNo);

  } else if (ButtonCount == 3) {
    //
    // Yes / No / Cancel
    //
    YesNoButton[0] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_YES), mHiiHandle);
    YesNoButton[1] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_NO), mHiiHandle);
    YesNoButton[2] = (UINTN)(UINTN *) GetString (STRING_TOKEN (STR_CANCEL), mHiiHandle);


    CreateValueAsUint64(YesNoButtonHiiValue    , ButtonActionYes);
    CreateValueAsUint64(YesNoButtonHiiValue + 1, ButtonActionNo);
    CreateValueAsUint64(YesNoButtonHiiValue + 2, ButtonActionCancel);
  }

  ButtonStringArray = NULL;
  ButtonHiiValueArray = NULL;
  if (ButtonCount > 0) {
    ButtonStringArray = AllocateCopyPool (sizeof (CHAR16 *) * ButtonCount, YesNoButton);
    ButtonHiiValueArray = AllocateCopyPool (sizeof (EFI_HII_VALUE) * ButtonCount, YesNoButtonHiiValue);
    CopyMem (&Dialog->ConfirmHiiValue, YesNoButtonHiiValue, sizeof(EFI_HII_VALUE));
  }

  Status = CreateNewDialog (
             DialogType,
             Attribute,
             TitleString,
             BodyCount,
             0,
             ButtonCount,
             BodyStringArray,
             NULL,
             ButtonStringArray,
             NULL,
             ButtonHiiValueArray,
             Dialog
             );

  for (Index = 0; Index < ButtonCount; Index++) {
    FreePool ((UINTN *) YesNoButton[Index]);
  }
  if (ButtonCount > 0) {
    FreePool (ButtonStringArray);
    FreePool (ButtonHiiValueArray);
  }

  if (Dialog->ConfirmHiiValue.Value.u64 != ButtonActionYes) {
    return EFI_NOT_READY;
  }

  return Status;
}

/**
 Open dialog bt hotkey event

 @param [in]        SendEvtType          Send event type

 @retval Status                          Create simple dialog status

**/
EFI_STATUS
BroadcastOpenDByHotKeyEvt (
  IN     UINT32                              SendEvtType
  )
{
  EFI_STATUS                                  Status;

  H2O_FORM_BROWSER_D                          Dialog;

  UINT32                                      DialogType;
  UINT32                                      Attribute;
  CHAR16                                      *TitleString;
  UINT32                                      BodyCount;
  CHAR16                                      **BodyStringArray;
  UINT32                                      ButtonCount;
  UINTN                                       Index;

  DialogType = 0;
  Attribute = 0;
  TitleString = NULL;
  BodyCount = 1;
  BodyStringArray = NULL;
  ButtonCount = 2;

  Status = EFI_SUCCESS;
  switch (SendEvtType) {

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT:
    TitleString = GetString (STRING_TOKEN (STR_DEFAULT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_LOAD_OPTIMAL_DEFAULT), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_EXIT:
    TitleString = GetString (STRING_TOKEN (STR_EXIT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_EXIT), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT:
    TitleString = GetString (STRING_TOKEN (STR_EXIT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_EXIT_AND_SUBMIT), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT:
    TitleString = GetString (STRING_TOKEN (STR_EXIT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_EXIT_AND_DISCARD), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_EXIT_P:
    TitleString = GetString (STRING_TOKEN (STR_EXIT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_EXIT_PAGE), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT_P:
    TitleString = GetString (STRING_TOKEN (STR_EXIT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_EXIT_AND_SUBMIT_PAGE), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT_P:
    TitleString = GetString (STRING_TOKEN (STR_EXIT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_EXIT_AND_DISCARD_PAGE), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_P:
    TitleString = GetString (STRING_TOKEN (STR_SUBMIT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_SUBMIT_PAGE), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_P:
    TitleString = GetString (STRING_TOKEN (STR_DISCARD_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_DISCARD_PAGE), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
   break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT:
    TitleString = GetString (STRING_TOKEN (STR_SUBMIT_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_SUBMIT), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD:
    TitleString = GetString (STRING_TOKEN (STR_DISCARD_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_DISCARD), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_SELECTION;
    break;

  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHOW_HELP:
    BodyCount = 8;
    ButtonCount = 0;
    TitleString = GetString (STRING_TOKEN (STR_HELP_DIALOG_TITLE), mHiiHandle);
    BodyStringArray = AllocateZeroPool (sizeof (CHAR16 *) * BodyCount);
    if (BodyStringArray == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    BodyStringArray[0] = GetString (STRING_TOKEN (STR_HELP_MSG_1), mHiiHandle);
    BodyStringArray[1] = GetString (STRING_TOKEN (STR_HELP_MSG_2), mHiiHandle);
    BodyStringArray[2] = GetString (STRING_TOKEN (STR_HELP_MSG_3), mHiiHandle);
    BodyStringArray[3] = GetString (STRING_TOKEN (STR_HELP_MSG_4), mHiiHandle);
    BodyStringArray[4] = GetString (STRING_TOKEN (STR_HELP_MSG_5), mHiiHandle);
    BodyStringArray[5] = GetString (STRING_TOKEN (STR_HELP_MSG_6), mHiiHandle);
    BodyStringArray[6] = GetString (STRING_TOKEN (STR_HELP_MSG_7), mHiiHandle);
    BodyStringArray[7] = GetString (STRING_TOKEN (STR_HELP_MSG_8), mHiiHandle);
    DialogType = H2O_FORM_BROWSER_D_TYPE_MSG | H2O_FORM_BROWSER_D_TYPE_SHOW_HELP;
    break;

  default:
    TitleString = NULL;
    BodyStringArray = NULL;
    return EFI_NOT_FOUND;
  }

  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));
  CreateValueAsUint64 (&Dialog.ConfirmHiiValue, ButtonActionYes);
  Status = CreateSimpleDialog (DialogType, Attribute, TitleString, BodyCount, BodyStringArray, ButtonCount, &Dialog);

  if (BodyStringArray != NULL) {
    for (Index = 0; Index < BodyCount; Index++) {
      if (BodyStringArray[Index] != NULL) {
        FreePool (BodyStringArray[Index]);
      }
    }
    FreePool (BodyStringArray);
  }
  if (TitleString != NULL) {
    FreePool (TitleString);
  }

  if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    return EFI_NOT_READY;
  }

  //
  // when add dialog type, here need process
  //
  Status = EFI_NOT_READY;
  switch (SendEvtType) {

  case H2O_DISPLAY_ENGINE_EVT_TYPE_DISCARD_EXIT:
    return EFI_SUCCESS;
  case H2O_DISPLAY_ENGINE_EVT_TYPE_DEFAULT:
    return EFI_SUCCESS;
  case H2O_DISPLAY_ENGINE_EVT_TYPE_SUBMIT_EXIT:
    return EFI_SUCCESS;
  case H2O_DISPLAY_ENGINE_EVT_TYPE_SHOW_HELP:
    return EFI_SUCCESS;
  default:
    break;
  }

  return Status;
}
/**
 Open dialog of broadcast by question

 @param [in] Question         Select question

 @retval Status               Open dialog of broadcast status

**/
EFI_STATUS
BroadcastOpenDByQ (
  IN       H2O_FORM_BROWSER_Q                 *Question
  )
{
  EFI_STATUS                                  Status;

  Status = EFI_NOT_FOUND;
  switch (Question->Operand) {

  case EFI_IFR_NUMERIC_OP:
    Status = CreateNumericDialog (0, Question);
    break;

  case EFI_IFR_ONE_OF_OP:
    Status = CreateOneOfDialog (0, Question);
    break;

  case EFI_IFR_ORDERED_LIST_OP:
    Status = CreateOrderedListDialog (0, Question);
    break;

  case EFI_IFR_STRING_OP:
    Status = CreateStringDialog (0, Question);
    break;

  case EFI_IFR_PASSWORD_OP:
    Status = CreatePasswordDialog (0, Question);
    break;

  case EFI_IFR_DATE_OP:
    Status = CreateDateDialog (0, Question);
    break;

  case EFI_IFR_TIME_OP:
    Status = CreateTimeDialog (0, Question);
    break;

  default:
    return EFI_NOT_FOUND;
  }

  return Status;
}

