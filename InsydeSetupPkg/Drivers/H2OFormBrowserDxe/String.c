/** @file
 Define function of string operation .  
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
  Get the string based on the StringId and HII Package List Handle.

  @param  [in] Token                  The String's ID.
  @param  [in] HiiHandle              The package list in the HII database to search for
                                      the specified string.

  @return The output string.

**/
CHAR16 *
GetString (
  IN  EFI_STRING_ID                Token,
  IN  EFI_HII_HANDLE               HiiHandle
  )
{

  EFI_STRING  String;
  CHAR16      *UnknownString = L"!";
  
  if (HiiHandle == NULL) {
    return NULL;
  }

  String = HiiGetString (HiiHandle, Token, NULL);
  if (String == NULL) {
    String = AllocateCopyPool (StrSize (UnknownString), UnknownString);
    ASSERT (String != NULL);
  }
  return (CHAR16 *) String;
}