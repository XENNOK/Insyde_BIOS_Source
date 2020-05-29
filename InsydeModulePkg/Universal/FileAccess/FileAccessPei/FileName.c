/** @file
FAT file name format functions

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++

Copyright (c) 2005 - 2007, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the Software
License Agreement which accompanies this distribution.


Module Name:

  FileName.c

Abstract:

  Functions for manipulating file names

Revision History

--*/

#include "FatPeim.h"

/**
  This function checks whether the input FileName is a valid 8.3 short name.
  If the input FileName is a valid 8.3, the output is the 8.3 short name;
  otherwise, the output is the base tag of 8.3 short name.

 @param[in]     FileName          The input unicode filename. 
 @param[out]   File8Dot3Name  The output ascii 8.3 short name or base tag of 8.3 short name.
 
 @return TRUE   The input unicode filename is a valid 8.3 short name.
 @return FALSE  The input unicode filename is not a valid 8.3 short name.
**/
BOOLEAN
FatCheckIs8Dot3Name (
  IN  CHAR16    *FileName,
  OUT CHAR8     *File8Dot3Name
  )
{
  BOOLEAN PossibleShortName;
  CHAR16  *TempName;
  CHAR16  *ExtendName;
  CHAR16  *SeparateDot;
  UINTN   MainNameLen;
  UINTN   ExtendNameLen;

  PossibleShortName = TRUE;
  SeparateDot       = NULL;
  SetMem (File8Dot3Name, FAT_NAME_LEN, ' ');
  
  for (TempName = FileName; *TempName; TempName++) {
    if (*TempName == L'.') {
      SeparateDot = TempName;
    }
  }
  
  
  if (SeparateDot == NULL) {
    //
    // Extended filename is not detected
    //
    MainNameLen   = TempName - FileName;
    ExtendName    = TempName;
    ExtendNameLen = 0;
  } else {
    //
    // Extended filename is detected
    //
    MainNameLen   = SeparateDot - FileName;
    ExtendName    = SeparateDot + 1;
    ExtendNameLen = TempName - ExtendName;
  }
  //
  // We scan the filename for the second time
  // to check if there exists any extra blanks and dots
  //
  while (--TempName >= FileName) {
    if ((*TempName == L'.' || *TempName == L' ') && (TempName != SeparateDot)) {
      //
      // There exist extra blanks and dots
      //
      PossibleShortName = FALSE;
    }
  }

  if (MainNameLen == 0) {
    PossibleShortName = FALSE;
  }

  if (MainNameLen > FAT_MAIN_NAME_LEN) {
    PossibleShortName = FALSE;
    MainNameLen       = FAT_MAIN_NAME_LEN;
  }

  if (ExtendNameLen > FAT_EXTEND_NAME_LEN) {
    PossibleShortName = FALSE;
    ExtendNameLen     = FAT_EXTEND_NAME_LEN;
  }

  if (EngStrToFat (FileName, MainNameLen, File8Dot3Name)) {
    PossibleShortName = FALSE;
  }

  if (EngStrToFat (ExtendName, ExtendNameLen, File8Dot3Name + FAT_MAIN_NAME_LEN)) {
    PossibleShortName = FALSE;
  }

  return PossibleShortName;
}


