/** @file
  PlatformBdsLib

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

Copyright (c) 2004 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "String.h"

EFI_HII_HANDLE gPlatformBdsLibStrPackHandle;

EFI_GUID mPlatformBdsLibStrPackGuid = {
  0x82f541fd, 0xe752, 0x47df, { 0xb2, 0x34, 0x35, 0x22, 0xfe, 0x31, 0x77, 0x9c }
};

/**
  The constructor function register UNI strings into imageHandle.
  
  It will ASSERT() if that operation fails and it will always return EFI_SUCCESS. 

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS   The constructor successfully added string package.
  @retval Other value   The constructor can't add string package.

**/
EFI_STATUS
EFIAPI
PlatformBdsLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

  gPlatformBdsLibStrPackHandle = HiiAddPackages (
                                   &mPlatformBdsLibStrPackGuid,
                                   &ImageHandle,
                                   PlatformBdsLibStrings,
                                   NULL
                                   );

  ASSERT (gPlatformBdsLibStrPackHandle != NULL);

  return EFI_SUCCESS;
}

/**
  Get string by string id from HII Interface


  @param Id              String ID.

  @retval  CHAR16 *  String from ID.
  @retval  NULL      If error occurs.

**/
CHAR16 *
PlatformBdsGetStringById (
  IN  EFI_STRING_ID   Id
  )
{
  return HiiGetString (gPlatformBdsLibStrPackHandle, Id, NULL);
}
