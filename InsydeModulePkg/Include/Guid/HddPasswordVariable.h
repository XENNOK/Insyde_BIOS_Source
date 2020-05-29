/** @file
  Definitions for HDD password feature

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

#ifndef _EFI_HDD_PASSWORD_VARIABLE_H_
#define _EFI_HDD_PASSWORD_VARIABLE_H_

#include <Uefi.h>

//
// Variable Name
//
#define SAVE_HDD_PASSWORD_VARIABLE_NAME        L"SaveHddPassword"
#define SAVE_HDD_PASSWORD_INFO_VARIABLE_NAME   L"SaveHddPasswordInfo"

//
// Variable Guid
//
#define HDD_PASSWORD_TABLE_GUID \
  { 0x86bbf7e3, 0xb772, 0x4d22, 0x80, 0xa9, 0xe7, 0xc5, 0x8c, 0x3c, 0x7f, 0xf0 }

#define HDD_PASSWORD_INFO_TABLE_GUID \
  { 0xa17d3ac5, 0x4897, 0x47b5, 0x99, 0x12, 0x21, 0xa9, 0x69, 0x7e, 0xbd, 0xde }

#define HDD_PASSWORD_MAX_NUMBER            32

typedef struct _SEC_HDD_PASSWORD_STRUCT {
  UINT8                                   ControllerNumber;
  UINT32                                  PortNumber;
  UINT32                                  PortMulNumber;
  UINT8                                   PasswordType;
  CHAR16                                  PasswordStr[HDD_PASSWORD_MAX_NUMBER + 1];
} SEC_HDD_PASSWORD_STRUCT;

extern EFI_GUID gSaveHddPasswordGuid;
extern EFI_GUID gSaveHddPasswordInfoGuid;

#endif
