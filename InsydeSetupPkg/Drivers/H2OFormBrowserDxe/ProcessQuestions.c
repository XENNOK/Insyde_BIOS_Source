/** @file
Implementation for handling question option processing.
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalH2OFormBrowser.h"

/**
  Process Question Config.

  This function processes the results of processing forms and routes it to the appropriate handlers or 
  storage.
  When it update variable, it call this function
  @param  [in] Selection         Current selection
  @param  [in] Question          The Question to be peocessed.

  @retval EFI_SUCCESS            Question Config process success.
  @retval Other                  Question Config process fail.

**/
EFI_STATUS
ProcessQuestionConfig (
  IN  UI_MENU_SELECTION       *Selection,
  IN  FORM_BROWSER_STATEMENT  *Question
  )
{
  EFI_STATUS                      Status;
  CHAR16                          *ConfigResp;
  CHAR16                          *Progress;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;

  if (Question->QuestionConfig == 0) {
    return EFI_SUCCESS;
  }

  //
  // Get <ConfigResp>
  //
  ConfigResp = GetString (Question->QuestionConfig, Selection->FormSet->HiiHandle);
  if (ConfigResp == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Send config to Configuration Driver
  //
  ConfigAccess = Selection->FormSet->ConfigAccess;
  if (ConfigAccess == NULL) {
    return EFI_UNSUPPORTED;
  }
  Status = ConfigAccess->RouteConfig (
                           ConfigAccess,
                           ConfigResp,
                           &Progress
                           );

  return Status;
}


/**
  Search for an Option of a Question by its value.

  @param  [in] Question          The Question
  @param  [in] OptionValue       Value for Option to be searched.

  @return Pointer                Pointer to the found Option.
  @return NULL                   Option not found.

**/
QUESTION_OPTION *
ValueToOption (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN EFI_HII_VALUE            *OptionValue
  )
{
  LIST_ENTRY       *Link;
  QUESTION_OPTION  *Option;
  INTN             Result;

  Link = GetFirstNode (&Question->OptionListHead);
  while (!IsNull (&Question->OptionListHead, Link)) {
    Option = QUESTION_OPTION_FROM_LINK (Link);

    if ((CompareValue (&Option->Value, OptionValue, &Result, NULL) == EFI_SUCCESS) && (Result == 0)) {
      //
      // Check the selectable condition, only a valid option can be return.
      //
      if ((Option->SuppressExpression == NULL) ||
          ((EvaluateExpressionList(Option->SuppressExpression, FALSE, NULL, NULL) == ExpressFalse))) {
        return Option;
      }
    }

    Link = GetNextNode (&Question->OptionListHead, Link);
  }

  return NULL;
}


/**
  Return data element in an Array by its Index.

  @param  [in] Array             The data array.
  @param  [in] Type              Type of the data in this array.
  @param  [in] Index             Zero based index for data in this array.

  @retval Value                  The data to be returned

**/
UINT64
GetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINT32                    Index
  )
{
  UINT64 Data;

  ASSERT (Array != NULL);

  Data = 0;
  switch (Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    Data = (UINT64) *(((UINT8 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    Data = (UINT64) *(((UINT16 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    Data = (UINT64) *(((UINT32 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    Data = (UINT64) *(((UINT64 *) Array) + Index);
    break;

  default:
    break;
  }

  return Data;
}


/**
  Set value of a data element in an Array by its Index.

  @param  [in] Array                  The data array.
  @param  [in] Type                   Type of the data in this array.
  @param  [in] Index                  Zero based index for data in this array.
  @param  [in] Value                  The value to be set.

**/
VOID
SetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINT32                   Index,
  IN UINT64                   Value
  )
{

  ASSERT (Array != NULL);

  switch (Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    *(((UINT8 *) Array) + Index) = (UINT8) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    *(((UINT16 *) Array) + Index) = (UINT16) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    *(((UINT32 *) Array) + Index) = (UINT32) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    *(((UINT64 *) Array) + Index) = (UINT64) Value;
    break;

  default:
    break;
  }
}

