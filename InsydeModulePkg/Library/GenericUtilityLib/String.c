/** @file
  GenericUtilityLib

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


EFI_HII_HANDLE gGenericUtilityLibStringPackHandle;

EFI_GUID mGenericUtilityLibStrPackGuid = {
  0x55f53162, 0x7301, 0x48a2, { 0x89, 0x2d, 0x97, 0xec, 0xea, 0xd6, 0xc8, 0xf2 }
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
GenericUtilityLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{

  gGenericUtilityLibStringPackHandle = HiiAddPackages (
                                         &mGenericUtilityLibStrPackGuid,
                                         ImageHandle,
                                         GenericUtilityLibStrings,
                                         NULL
                                         );

  ASSERT (gGenericUtilityLibStringPackHandle != NULL);

  return EFI_SUCCESS;
}


/**
  Get string by string id from HII Interface


  @param Id              String ID.

  @retval  CHAR16 *  String from ID.
  @retval  NULL      If error occurs.

**/
CHAR16 *
GenericUtilityLibGetStringById (
  IN  EFI_STRING_ID   Id
  )
{
  return HiiGetString (gGenericUtilityLibStringPackHandle, Id, NULL);
}

