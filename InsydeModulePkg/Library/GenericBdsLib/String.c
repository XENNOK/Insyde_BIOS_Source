/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** 
  String support

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "String.h"

/**
  Get string by string id from HII Interface


  @param Id              String ID.

  @retval  CHAR16 *  String from ID.
  @retval  NULL      If error occurs.

**/
CHAR16 *
BdsLibGetStringById (
  IN  EFI_STRING_ID   Id
  )
{
  return HiiGetString (gBdsLibStringPackHandle, Id, NULL);
}


/**
  Update all BVDT data (which are from .fd file) into HiiDatabase.

  @param  HiiHandle     target EFI_HII_HANDLE instance

**/
VOID
UpdateBvdtToHii (
  IN EFI_HII_HANDLE     HiiHandle
  )
{
  UINTN                StrSize;
  CHAR16               Str[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, Str);
  HiiSetString (HiiHandle, STRING_TOKEN(STR_MISC_BIOS_VERSION), Str, NULL);

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtProductName, &StrSize, Str);
  HiiSetString (HiiHandle, STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Str, NULL);
  
  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtCcbVer, &StrSize, Str);
  HiiSetString (HiiHandle, STRING_TOKEN(STR_CCB_VERSION), Str, NULL);
}


/**
  Acquire the string associated with the ProducerGuid and return it.

  @param  ProducerGuid    The Guid to search the HII database for
  @param  Token           The token value of the string to extract
  @param  String          The string that is extracted

  @retval EFI_SUCCESS   The function returns EFI_SUCCESS always.

**/
EFI_STATUS
GetProducerString (
  IN      EFI_GUID                  *ProducerGuid,
  IN      EFI_STRING_ID             Token,
  OUT     CHAR16                    **String
  )
{
  EFI_STRING      TmpString;

  TmpString = HiiGetPackageString (ProducerGuid, Token, NULL);
  if (TmpString == NULL) {
    *String = BdsLibGetStringById (STRING_TOKEN (STR_MISSING_STRING));
  } else {
    *String = TmpString;
  }

  return EFI_SUCCESS;
}
