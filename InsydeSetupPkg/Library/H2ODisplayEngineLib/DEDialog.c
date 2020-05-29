/** @file

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

#include <H2ODisplayEngineLib.h>
#include <DEControl.h>


EFI_STATUS
DEOpenLayout (
  IN     H2O_FORM_BROWSER_PROTOCOL           *FBProtocol
  );

EFI_STATUS
DEOpenPage (
  IN     H2O_FORM_BROWSER_PROTOCOL           *FBProtocol
  );

EFI_STATUS
GetPopUpSelectionByValue (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     EFI_HII_VALUE                        *HiiValue,
  OUT    H2O_CONTROL_INFO                     **Controls,
  OUT    UINT32                               *ControlIndex OPTIONAL
  )
{
  UINT32                                      Index;


  for (Index = 0; Index < PopUpControls->Count; Index ++) {
    if (
      PopUpControls->ControlArray[Index].Selectable &&
      PopUpControls->ControlArray[Index].HiiValue.Type == HiiValue->Type &&
      PopUpControls->ControlArray[Index].HiiValue.Value.u64 == HiiValue->Value.u64
      ) {

      *Controls = &PopUpControls->ControlArray[Index];
      if (ControlIndex != NULL) {
        *ControlIndex = Index;
      }

      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
GetInputWithTab (
  IN  H2O_CONTROL_LIST                        *PopUpControls,
  IN  UINT32                                  TabIndex,
  OUT CHAR16                                  **ResultStr
  )
{
  UINT32                                      Index;
  UINT32                                      TabCount;

  if (PopUpControls == NULL || ResultStr == NULL || PopUpControls->ControlArray == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*ResultStr != NULL) {
    FreePool (*ResultStr);
    *ResultStr = NULL;
  }

  TabCount = 0;
  for (Index = 0; Index < PopUpControls->Count; Index ++) {
    if (!PopUpControls->ControlArray[Index].Editable) {
      continue;
    }

    if (TabCount == TabIndex) {
      *ResultStr = AllocateCopyPool (
                     StrSize (PopUpControls->ControlArray[Index].Text.String),
                     PopUpControls->ControlArray[Index].Text.String
                     );
      break;
    }
    TabCount++;
  }

  return *ResultStr != NULL ? EFI_SUCCESS : EFI_NOT_FOUND;
}

UINT32
GetInputMaxTabNumber (
  IN     H2O_CONTROL_LIST                     *PopUpControls
  )
{
  UINT32                                      Index;
  UINT32                                      ChangeIndex;


  ChangeIndex = 0;
  for (Index = 0; Index < PopUpControls->Count; Index ++) {
    if (PopUpControls->ControlArray[Index].Editable) {
      ChangeIndex ++;
    }
  }

  return (ChangeIndex - 1);
}

EFI_STATUS
RefreshPopUpSelectionByValue (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     BOOLEAN                              HightLight,
  IN     BOOLEAN                              BottonStartEndChar,
  IN     EFI_HII_VALUE                        *HiiValue
  )
{
  EFI_STATUS                                  Status;
  H2O_CONTROL_INFO                            *Control;


  Status = GetPopUpSelectionByValue (PopUpControls, HiiValue, &Control, NULL);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // High-light select
  //
  if (HightLight) {
    Status = DisplayHighLightControl (BottonStartEndChar, Control);
  } else {
    Status = DisplayButtonControls (BottonStartEndChar, 1, Control);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RefreshInput (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     BOOLEAN                              HightLight,
  IN     CHAR16                               *String
  )
{
  EFI_STATUS                                  Status;
  UINT32                                      Index;
  CHAR16                                      *TempString;


  for (Index = 0; Index < PopUpControls->Count; Index ++) {
    if (PopUpControls->ControlArray[Index].Editable) {
      //
      // High-light select
      //
      //
      // Change Value String
      //
      TempString = PopUpControls->ControlArray[Index].Text.String;
      if (TempString != NULL) {
        FreePool (TempString);
      }
      TempString = CatSPrint (NULL, String);
      PopUpControls->ControlArray[Index].Text.String = TempString;

      //
      // Dispaly
      //
      if (HightLight) {
        Status = DisplayHighLightControl (FALSE, &PopUpControls->ControlArray[Index]);
      } else {
        Status = DisplayButtonControls (FALSE, 1, &PopUpControls->ControlArray[Index]);
      }
      if (EFI_ERROR (Status)) {
        return Status;
      }
      break;
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
RefreshPassword (
  IN H2O_CONTROL_LIST                         *PopUpControls,
  IN H2O_CONTROL_INFO                         *CurrentControl,
  IN CHAR16                                   *String
  )
{
  EFI_STATUS                                  Status;
  UINT32                                      Index;
  UINT32                                      StringLength;
  UINT32                                      StringIndex;
  CHAR16                                      *BackupString;
  CHAR16                                      *HiddenString;
  H2O_CONTROL_INFO                            *Control;

  for (Index = 0; Index < PopUpControls->Count; Index ++) {
    Control = &PopUpControls->ControlArray[Index];

    if (!Control->Editable) {
      continue;
    }

    if (Control == CurrentControl) {
      if (String != NULL) {
        SafeFreePool ((VOID **) &Control->Text.String);
        Control->Text.String = AllocateCopyPool (StrSize (String), String);
      }
    }

    BackupString = Control->Text.String;
    StringLength = (UINT32) StrLen (BackupString);

    HiddenString = (StringLength > 0) ? AllocateZeroPool ((StringLength + 1) * sizeof (CHAR16)) : NULL;
    if (HiddenString != NULL) {
      for (StringIndex = 0; StringIndex < StringLength; StringIndex++) {
        HiddenString[StringIndex] = '*';
      }

      Control->Text.String = HiddenString;
    }

    //
    // Dispaly
    //
    if (Control == CurrentControl) {
      Status = DisplayHighLightControl (FALSE, Control);
    } else {
      Status = DisplayButtonControls (FALSE, 1, Control);
    }

    Control->Text.String = BackupString;
    SafeFreePool ((VOID **) &HiddenString);

    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
RefreshPasswordInput (
  IN H2O_CONTROL_INFO                         *ControlArray,
  IN UINT32                                   Count,
  IN CHAR16                                   *String
  )
{
  EFI_STATUS                                  Status;
  UINT32                                      Index;
  CHAR16                                      *StringBuffer;
  UINTN                                       StringLength;
  UINTN                                       StringIndex;

  if (ControlArray == NULL || String == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_NOT_FOUND;

  for (Index = 0; Index < Count; Index ++) {
    if (!ControlArray[Index].Editable) {
      continue;
    }

    StringLength = StrLen (String);
    StringBuffer = AllocateZeroPool ((StringLength + 1) * sizeof (CHAR16));
    if (StringBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    for (StringIndex = 0; StringIndex < StringLength; StringIndex++) {
      StringBuffer[StringIndex] = '*';
    }

    SafeFreePool ((VOID **) &ControlArray[Index].Text.String);
    ControlArray[Index].Text.String = StringBuffer;

    Status = DisplayHighLightControl (FALSE, &ControlArray[Index]);

    CopyMem (StringBuffer, String, (StringLength + 1) * sizeof (CHAR16));
    if (EFI_ERROR (Status)) {
      return Status;
    }
    break;
  }

  return Status;
}

EFI_STATUS
RefreshInputWithTab (
  IN     H2O_CONTROL_LIST                     *PopUpControls,
  IN     UINT32                               TabIndex,
  IN     CHAR16                               *String
  )
{
  EFI_STATUS                                  Status;
  UINT32                                      Index;
  UINT32                                      ChangeIndex;
  CHAR16                                      *TempString;


  ChangeIndex = 0;
  for (Index = 0; Index < PopUpControls->Count; Index ++) {
    if (PopUpControls->ControlArray[Index].Editable) {

      if (ChangeIndex == TabIndex) {
        //
        // Change Value String
        //
        if (String != NULL) {
          TempString = PopUpControls->ControlArray[Index].Text.String;
          if (TempString != NULL) {
            FreePool (TempString);
          }
          TempString = CatSPrint (NULL, String);
          PopUpControls->ControlArray[Index].Text.String = TempString;
        }
      }
      //
      // Dispaly
      //
      if (ChangeIndex == TabIndex) {
        Status = DisplayHighLightControl (FALSE, &PopUpControls->ControlArray[Index]);
      } else {
        Status = DisplayButtonControls (FALSE, 1, &PopUpControls->ControlArray[Index]);
      }
      if (EFI_ERROR (Status)) {
        return Status;
      }
      ChangeIndex ++;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ShutDialog (
  VOID
  )
{
  H2O_PANEL_INFO                              *QuestionPanel;

  QuestionPanel = GetPanelInfoByType (&mDEPrivate->Layout->PanelListHead, H2O_PANEL_TYPE_QUESTION);
  if (QuestionPanel != NULL) {
    FreePanel (QuestionPanel);
  }

  return EFI_SUCCESS;
}

