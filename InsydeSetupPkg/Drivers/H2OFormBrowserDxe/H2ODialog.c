/** @file
 The H2O_DIALOG_PROTOCOL is the interface to the EFI Configuration Driver.

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

/**
 Display Dialog for User Requirement which dilag type is Yes/No, Yes/No/Cancel, Ok or Ok/Cancel

 @param[in]  DialogOperation           Dialog type (Yes/No, Yes/No/Cancel, Ok or Ok/Cancel)
 @param[in]  HotKey                    HotKey information
 @param[in]  MaximumStringSize         Maximum string length
 @param[out] StringBuffer              String buffer
 @param[out] KeyValue               Ptr to returned structure that indicates the key the user selected.
 @param[in]  String                 Ptr to null-terminated string that specifies the dialog prompt

 @retval EFI_SUCCESS                Process successfully.
**/
EFI_STATUS
EFIAPI
FBConfirmDialog (
  IN  UINT32                           DialogOperation,
  IN  BOOLEAN                          HotKey,
  IN  UINT32                           MaximumStringSize,
  OUT CHAR16                          *StringBuffer,
  OUT EFI_INPUT_KEY                   *KeyValue,
  IN  CHAR16                          *String,
  ...
  )
{
  EFI_STATUS                          Status;

//ButtonStringArray and ButtonHiiValueArray use {YES,NO,CANCEL,NULL}
  EFI_STRING                          ButtonStringArray[4];
  EFI_HII_VALUE                       ButtonHiiValueArray[4];
  H2O_FORM_BROWSER_D                  Dialog;
  UINT32                              ButtonCount;
  BOOLEAN                             ConsoleInitComplete;
  H2O_FORM_BROWSER_PRIVATE_DATA       *Private;
  UINTN                               Index;

  ConsoleInitComplete = FALSE;
  Private = NULL;
  FBIsConsoleInit (&ConsoleInitComplete, &Private);
  if (!ConsoleInitComplete) {
    Status = FBInitConsoles (Private);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = EFI_SUCCESS;

  switch (DialogOperation) {

  case DlgYesNo:
    ButtonCount          = 2;
    ButtonStringArray[0] = GetString (STRING_TOKEN (STR_YES), mHiiHandle);
    ButtonStringArray[1] = GetString (STRING_TOKEN (STR_NO), mHiiHandle);

    CreateValueAsUint64 (ButtonHiiValueArray    , ButtonActionYes);
    CreateValueAsUint64 (ButtonHiiValueArray + 1, ButtonActionNo);
    break;

  case DlgYesNoCancel:
    ButtonCount          = 3;
    ButtonStringArray[0] = GetString (STRING_TOKEN (STR_YES), mHiiHandle);
    ButtonStringArray[1] = GetString (STRING_TOKEN (STR_NO), mHiiHandle);
    ButtonStringArray[2] = GetString (STRING_TOKEN (STR_CANCEL), mHiiHandle);
    CreateValueAsUint64 (ButtonHiiValueArray    , ButtonActionYes);
    CreateValueAsUint64 (ButtonHiiValueArray + 1, ButtonActionNo);
    CreateValueAsUint64 (ButtonHiiValueArray + 2, ButtonActionCancel);
    break;

  case DlgOk:
    ButtonCount          = 1;
    ButtonStringArray[0] = GetString (STRING_TOKEN (STR_OK), mHiiHandle);
    CreateValueAsUint64 (ButtonHiiValueArray, ButtonActionYes);
    break;

  case DlgOkCancel:
    ButtonCount          = 2;
    ButtonStringArray[0] = GetString (STRING_TOKEN (STR_OK), mHiiHandle);
    ButtonStringArray[1] = GetString (STRING_TOKEN (STR_CANCEL), mHiiHandle);
    CreateValueAsUint64 (ButtonHiiValueArray    , ButtonActionYes);
    CreateValueAsUint64 (ButtonHiiValueArray + 1, ButtonActionCancel);
    break;

  default:
    DEBUG ((EFI_D_INFO, "DialogOperation is error"));
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }
  CreateValueAsUint64 (&Dialog.ConfirmHiiValue, ButtonActionYes);

  Status = CreateNewDialog (
             H2O_FORM_BROWSER_D_TYPE_SELECTION,
             0,
             NULL,
             1,
             0,
             ButtonCount,
             &String,
             NULL,
             ButtonStringArray,
             NULL,
             ButtonHiiValueArray,
             &Dialog
             );
  if (!EFI_ERROR (Status) && Dialog.ConfirmHiiValue.Value.u8 == ButtonActionYes) {
    KeyValue->ScanCode    = SCAN_NULL;
    KeyValue->UnicodeChar = CHAR_CARRIAGE_RETURN;
  } else {
    KeyValue->ScanCode    = SCAN_ESC;
    KeyValue->UnicodeChar = CHAR_NULL;
  }

  if (!ConsoleInitComplete) {
    FBDetachConsoles (Private);
  }

  for (Index = 0; Index < ButtonCount; Index++) {
    FreePool (ButtonStringArray[Index]);
  }

  return Status;
}

/**
 Display Dialog for User Requirement

 @param[in]  HotKey                 HotKey information
 @param[in]  MaximumStringSize      Maximum string size
 @param[out] UserInputStringBuffer  User input string buffer
 @param[out] KeyValue               Output key value which stand for user selection
 @param[in]  String                 Dialog question string

 @retval EFI_SUCCESS                Process successfully.
**/
EFI_STATUS
EFIAPI
FBPasswordDialog (
  IN  UINT32                                   NumberOfLines,
  IN  BOOLEAN                                 HotKey,
  IN  UINT32                                  MaximumStringSize,
  OUT CHAR16                                  *UserInputStringBuffer,
  OUT EFI_INPUT_KEY                           *KeyValue,
  IN  CHAR16                                  *TitleString,
  ...
  )
{
  EFI_STATUS                                  Status;
  H2O_FORM_BROWSER_PRIVATE_DATA               *Private;
  BOOLEAN                                     ConsoleInitComplete;
  H2O_FORM_BROWSER_D                          Dialog;
  CHAR16                                      **BodyInputStringArray;

  if ((MaximumStringSize == 0) || (UserInputStringBuffer == NULL)) {
    return EFI_SUCCESS;
  }

  Private = &mFBPrivate;

  ConsoleInitComplete = Private->ConsoleInitComplete;
  if (!ConsoleInitComplete) {
    Status = FBInitConsoles (Private);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));

  BodyInputStringArray = AllocatePool (sizeof (CHAR16 *));
  if (BodyInputStringArray == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  BodyInputStringArray[0] = L"";

  CreateValueAsString (&Dialog.ConfirmHiiValue, (UINT16) MaximumStringSize * sizeof (CHAR16), (UINT8 *)L"");

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_PASSWORD << 16) | H2O_FORM_BROWSER_D_TYPE_SELECTION,
             0,
             TitleString,
             0,
             1,
             0,
             NULL,
             BodyInputStringArray,
             NULL,
             NULL,
             NULL,
             &Dialog
             );
  if (!EFI_ERROR (Status)) {
    ZeroMem (UserInputStringBuffer, sizeof (CHAR16) * MaximumStringSize);
    CopyMem (UserInputStringBuffer, Dialog.ConfirmHiiValue.Buffer, StrSize ((CHAR16 *) Dialog.ConfirmHiiValue.Buffer));
    FreePool (Dialog.ConfirmHiiValue.Buffer);
    KeyValue->ScanCode    = CHAR_CARRIAGE_RETURN;
    KeyValue->UnicodeChar = SCAN_NULL;
  } else if (Status == EFI_NOT_READY) {
    //
    // Shut Dialog
    //
    KeyValue->ScanCode    = CHAR_NULL;
    KeyValue->UnicodeChar = SCAN_ESC;
    Status = EFI_SUCCESS;
  }

  if (!ConsoleInitComplete) {
    FBDetachConsoles (Private);
  }

  return Status;
}
/**
 Draw select dialog and wait user select item.

 @param[in]  NumberOfLines          Number of display item
 @param[in]  HotKey                 If TRUE need check assign key list
                                    If FALSE use function default key event
 @param[in]  KeyList                Assign key list
 @param[out] EventKey               User input string buffer
 @param[in]  MaximumStringSize      Maximum string length
 @param[in]  TitleString            Title string
 @param[in]  SelectIndex            The select index number
 @param[in]  String                 The address array of string
 @param[in]  Color                  Set display color

 @retval EFI_SUCCESS                Process successfully.
**/
EFI_STATUS
EFIAPI
FBOneOfOptionDialog (
  IN  UINT32                           NumberOfLines,
  IN  BOOLEAN                         HotKey,
  IN  CONST EFI_INPUT_KEY             *KeyList,
  OUT EFI_INPUT_KEY                   *EventKey,
  IN  UINT32                           MaximumStringSize,
  IN  CHAR16                          *TitleString,
  OUT UINT32                          *SelectIndex,
  IN  CHAR16                          **String,
  IN  UINT32                           Color
  )
{
  EFI_STATUS                          Status;
  H2O_FORM_BROWSER_D                  Dialog;
  UINT32                               Index;
  EFI_HII_VALUE                       *HiiValueArray;
  H2O_FORM_BROWSER_PRIVATE_DATA       *Private;
  BOOLEAN                             ConsoleInitComplete;


  Private = &mFBPrivate;

  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));

  if (NumberOfLines == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ConsoleInitComplete = Private->ConsoleInitComplete;
  if (!ConsoleInitComplete) {
    Status = FBInitConsoles (Private);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }


  HiiValueArray = AllocateZeroPool (sizeof (EFI_HII_VALUE) * NumberOfLines);
  for (Index = 0; Index < NumberOfLines; Index++) {

    CreateValueAsUint64 (HiiValueArray+Index, Index);
  }

  Index = (*SelectIndex < NumberOfLines) ? *SelectIndex : 0;
  CopyMem (&Dialog.ConfirmHiiValue, &HiiValueArray[Index], sizeof (EFI_HII_VALUE));
  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_ONE_OF << 16) | H2O_FORM_BROWSER_D_TYPE_BODY_SELECTABLE,
             0,
             TitleString,
             (UINT32) NumberOfLines,
             0,
             0,
             String,
             NULL,
             NULL,
             HiiValueArray,
             NULL,
             &Dialog
             );

  if (!EFI_ERROR (Status)) {
    *SelectIndex = (UINT32) (UINTN)Dialog.ConfirmHiiValue.Value.u64;
    EventKey->ScanCode    = SCAN_NULL;
    EventKey->UnicodeChar = CHAR_CARRIAGE_RETURN;
  } else {
    EventKey->ScanCode    = SCAN_ESC;
    EventKey->UnicodeChar = CHAR_NULL;
  }

  if (!ConsoleInitComplete) {
    FBDetachConsoles (Private);
  }

  FreePool (HiiValueArray);

  return EFI_SUCCESS;
}
/**
 To show message dialog with title.

 @param[in] RequestedWidth      Dialog Width.
 @param[in] NumberOfLines       Number of item.
 @param[in] ArrayOfStrings      Message strings.

 @retval  EFI_SUCCESS           The function completed successfully.
**/
EFI_STATUS
EFIAPI
FBCreateMsgPopUp (
  IN  UINT32                       RequestedWidth,
  IN  UINT32                       NumberOfLines,
  IN  CHAR16                      *ArrayOfStrings,
  ...
  )
{
  H2O_FORM_BROWSER_D Dialog;
  EFI_STATUS         Status;
  CHAR16                              **MsgString;
  H2O_FORM_BROWSER_PRIVATE_DATA       *Private;
  BOOLEAN                             ConsoleInitComplete;


  Private = &mFBPrivate;
  MsgString = &ArrayOfStrings;

  ConsoleInitComplete = Private->ConsoleInitComplete;
  if (!ConsoleInitComplete) {
    Status = FBInitConsoles (Private);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));
  Status = CreateSimpleDialog (
             H2O_FORM_BROWSER_D_TYPE_MSG,
             0,
             MsgString[0],
             (UINT32)(NumberOfLines - 1),
             &(MsgString[1]),
             0,
             &Dialog
             );

  CpuDeadLoop ();

  if (!ConsoleInitComplete) {
    FBDetachConsoles (Private);
  }

  return EFI_SUCCESS;
}

/**
 Display page information

 @param[in] TitleString         Ptr to the dialog title string
 @param[in] TokenHandle         HII handle that specifies the information string handle
 @param[in] NumOfInfoStrings    Number of the information strings
 @param[in] InfoStrings         Unsigned integer that specifies the number of information strings in InfoStrings
 @return EFI_INVALID_PARAMETER  Invalid parameter
 @return EFI_OUT_OF_RESOURCES   BodyStringArray resource is not enough
 @return EFI_SUCCESS            Display page information success
**/
EFI_STATUS
EFIAPI
FBShowPageInfo (
  IN   CHAR16          *TitleString,
  IN   CHAR16          *InfoStrings
  )
{
  EFI_STATUS                                  Status;
  UINT32                                      DialogType;
  UINT32                                      ButtonCount;
  H2O_FORM_BROWSER_D                          Dialog;
  UINT32                                      Attribute;
  H2O_FORM_BROWSER_PRIVATE_DATA               *Private;
  BOOLEAN                                     ConsoleInitComplete;

  if (TitleString == NULL || InfoStrings == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = &mFBPrivate;
  ConsoleInitComplete = Private->ConsoleInitComplete;
  if (!ConsoleInitComplete) {
    Status = FBInitConsoles (Private);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  DialogType = H2O_FORM_BROWSER_D_TYPE_MSG | H2O_FORM_BROWSER_D_TYPE_SHOW_HELP;
  ButtonCount = 0;
  Attribute = 0;
  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));

  Status = CreateSimpleDialog (
             DialogType,
             Attribute,
             TitleString,
             1,
             &InfoStrings,
             ButtonCount,
             &Dialog
             );

  if (!ConsoleInitComplete) {
    FBDetachConsoles (Private);
  }

  return Status;
}

/**
 Display numeric dialog for user input numeric value

 @param[in]  TitleString         The dialog title string
 @param[in]  Minimum             Minimum value of output numeric value
 @param[in]  Maximum             Maximum value of output numeric value
 @param[in]  Step                Step value of numeric value
 @param[in]  IsHex               Flag to determine if the value is hexadecimal or not
 @param[out] Step                Output numeric value

 @retval EFI_SUCCESS             Get user input value successfully
 @retval EFI_INVALID_PARAMETER   TitleString or NumericValue is NULL
 @retval EFI_OUT_OF_RESOURCES    Allocate memory fail
**/
EFI_STATUS
EFIAPI
FBNumericDialog (
  IN  CHAR16                          *TitleString,
  IN  UINT64                          Minimum,
  IN  UINT64                          Maximum,
  IN  UINT64                          Step,
  IN  BOOLEAN                         IsHex,
  OUT UINT64                          *NumericValue
  )
{
  EFI_STATUS                          Status;
  H2O_FORM_BROWSER_D                  Dialog;
  CHAR16                              *BodyInputString;
  CHAR16                              **ButtonStringArray;
  EFI_HII_VALUE                       *ButtonHiiValueArray;
  H2O_FORM_BROWSER_S                  H2OStatement;
  H2O_FORM_BROWSER_PRIVATE_DATA       *Private;
  BOOLEAN                             ConsoleInitComplete;

  if (TitleString == NULL || NumericValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = &mFBPrivate;
  ConsoleInitComplete = Private->ConsoleInitComplete;
  if (!ConsoleInitComplete) {
    Status = FBInitConsoles (Private);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  ZeroMem (&Dialog, sizeof (H2O_FORM_BROWSER_D));
  ZeroMem (&H2OStatement, sizeof (H2OStatement));

  H2OStatement.Minimum = Minimum;
  H2OStatement.Maximum = Maximum;
  H2OStatement.Step    = Step;
  H2OStatement.Flags   = IsHex ? EFI_IFR_DISPLAY_UINT_HEX : EFI_IFR_DISPLAY_UINT_DEC;

  Dialog.H2OStatement         = &H2OStatement;
  Dialog.ConfirmHiiValue.Type = EFI_IFR_TYPE_NUM_SIZE_64;

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

  CreateValueAsBoolean (ButtonHiiValueArray    , TRUE);
  CreateValueAsBoolean (ButtonHiiValueArray + 1, FALSE);

  BodyInputString = L"";

  Status = CreateNewDialog (
             (H2O_FORM_BROWSER_D_TYPE_NUMERIC << 16) | H2O_FORM_BROWSER_D_TYPE_SELECTION,
             0,
             TitleString,
             0,
             1,
             2,
             NULL,
             &BodyInputString,
             ButtonStringArray,
             NULL,
             ButtonHiiValueArray,
             &Dialog
             );
  if (!EFI_ERROR (Status)) {
    *NumericValue = Dialog.ConfirmHiiValue.Value.u64;
   } else {
    Status = EFI_ABORTED;
  }

  FreePool (ButtonStringArray[0]);
  FreePool (ButtonStringArray[1]);
  FreePool (ButtonStringArray);
  FreePool (ButtonHiiValueArray);

  if (!ConsoleInitComplete) {
    FBDetachConsoles (Private);
  }

  return Status;
}

/**
 This is the routine which install the H2O dialog protocol.

 @retval  EFI_SUCCESS            The function completed successfully.
 @retval  EFI_OUT_OF_RESOURCES   Allocate memory fail.
 @retval  otherwise              Install protocol interface fail
**/
EFI_STATUS
EFIAPI
InstallH2ODialogProtocol (
  EFI_HANDLE ImageHandle
  )
{
  EFI_STATUS                     Status;
  H2O_DIALOG_PROTOCOL           *H2ODialogData;

  H2ODialogData = AllocateZeroPool (sizeof (H2O_DIALOG_PROTOCOL));
  if (H2ODialogData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }


  H2ODialogData->ConfirmDialog     = FBConfirmDialog;
  H2ODialogData->PasswordDialog    = FBPasswordDialog;
  H2ODialogData->OneOfOptionDialog = FBOneOfOptionDialog;
  H2ODialogData->CreateMsgPopUp    = FBCreateMsgPopUp;
  H2ODialogData->ShowPageInfo      = FBShowPageInfo;
  H2ODialogData->NumericDialog     = FBNumericDialog;

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gH2ODialogProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  H2ODialogData
                  );
  DEBUG ((EFI_D_INFO, "Install H2ODialogProtocol is fail"));
  ASSERT_EFI_ERROR (Status);

  return Status;
}

