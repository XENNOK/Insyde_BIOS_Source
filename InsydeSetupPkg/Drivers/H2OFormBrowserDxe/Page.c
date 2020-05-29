/** @file
 Define function of page.
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
 Init page information.

 @param[in]      Private       Formbrowser private data
 @param[in]      FormSet       Specified FormSet
 @param[in]      Form          Specified Form
 @param[in, out] Page          Page information

**/
VOID
InitPage (
  IN     H2O_FORM_BROWSER_PRIVATE_DATA               *Private,
  IN     FORM_BROWSER_FORMSET                        *FormSet,
  IN     FORM_BROWSER_FORM                           *Form,
  IN OUT H2O_FORM_BROWSER_P                          *Page
  )
{
  UINTN                         Index;

  Page->Signature            = H2O_FORM_PAGE_SIGNATURE;
  Page->Size                 = sizeof (H2O_FORM_BROWSER_P);

  for (Index = 0; Index < Private->HiiHandleCount; Index++) {
    if (Private->HiiHandleList[Index] == FormSet->HiiHandle) {
      Page->PageId             = (H2O_PAGE_ID)((Index << 16) | Form->FormId);
      break;
    }
  }
  //ASSERT (Index < Private->HiiHandleCount);
  Page->PageTitle            = GetString (Form->FormTitle, FormSet->HiiHandle);
  Page->Image                = NULL;
  Page->Animation            = NULL;
  Page->StatementIds         = AllocateZeroPool (sizeof (H2O_STATEMENT_ID) * Form->NumberOfStatement);
  Page->NumberOfStatementIds = 0;
}

