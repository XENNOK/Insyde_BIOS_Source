/** @file
 Define function of option.
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
 Copy option.

 @param[in]      Form          Form of option
 @param[in, out] Statement     Option statement

**/
VOID
CopyOption (
  IN     FORM_BROWSER_FORM         *Form,
  IN OUT FORM_BROWSER_STATEMENT    *Statement
  )
{
  EFI_IMAGE_INPUT               *ImageIn;
  QUESTION_OPTION               *Option;
  H2O_FORM_BROWSER_O            *H2OOption;
  H2O_FORM_BROWSER_S            *H2OStatement;
  LIST_ENTRY                    *LinkOption;
  EFI_STATUS                    Status;


  H2OStatement  = &Statement->Statement;
  H2OOption     = H2OStatement->Options;
  LinkOption    = GetFirstNode (&Statement->OptionListHead);
  H2OStatement->NumberOfOptions = 0;

  while (!IsNull (&Statement->OptionListHead, LinkOption)) {
    Option = QUESTION_OPTION_FROM_LINK (LinkOption);
    LinkOption = GetNextNode (&Statement->OptionListHead, LinkOption);

    if ((Option->SuppressExpression == NULL) ||
        (EvaluateExpressionList(Option->SuppressExpression, FALSE, NULL, NULL) == ExpressFalse)) {
      H2OOption->Signature = H2O_FORM_BROWSER_OPTION_SIGNATURE;
      H2OOption->Size      = sizeof (H2O_FORM_BROWSER_O);
      if (Option->Text == 0) {
        H2OOption->Text = AllocateZeroPool (sizeof (CHAR16));
      } else {
        H2OOption->Text = GetString (Option->Text, Form->FormSet->HiiHandle);
      }

      H2OOption->Image = NULL;
      if (Option->ImageId != 0) {
        ImageIn = AllocateZeroPool (sizeof (EFI_IMAGE_INPUT));
        Status = gHiiImage->GetImage (
                              gHiiImage,
                              Form->FormSet->HiiHandle,
                              (EFI_IMAGE_ID) Option->ImageId,
                              ImageIn
                              );
        ASSERT_EFI_ERROR (Status);
        if (!EFI_ERROR (Status)) {
          H2OOption->Image = ImageIn;
        } else {
          FreePool (ImageIn);
        }
      }

    //H2OOption->Animation = 0;
      CopyMem (&H2OOption->HiiValue, &Option->Value, sizeof (EFI_HII_VALUE));
      H2OOption->Default = (Option->Flags & EFI_IFR_OPTION_DEFAULT) == EFI_IFR_OPTION_DEFAULT? TRUE: FALSE;
      H2OOption->ManufactureDefault = (Option->Flags & EFI_IFR_OPTION_DEFAULT_MFG) == EFI_IFR_OPTION_DEFAULT_MFG? TRUE: FALSE;
      H2OOption->Visibility = TRUE;
      H2OOption++;

      H2OStatement->NumberOfOptions++;
    }
  }
}

