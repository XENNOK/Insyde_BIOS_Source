/** @file

  Console Lock Guid definitions

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_CONSOLE_LOCK_GUID_H_
#define _EFI_CONSOLE_LOCK_GUID_H_

#define EFI_CONSOLE_LOCK_GUID \
  { 0x368CDA0D, 0xCF31, 0x4B9B, { 0x8C, 0xF6, 0xE7, 0xD1, 0xBF, 0xFF, 0x15, 0x7E }}

#define EFI_CONSOLE_LOCK_VARIABLE_NAME        (L"ConsoleLock")
#define EFI_CONSOLE_LOCK_FORMAT_VARIABLE_NAME ("ConsoleLock")

extern CHAR16   gEfiConsoleLockName[];

extern EFI_GUID gEfiConsoleLockGuid;

#define LOCK_CONSOLE    1
#define NO_LOCK_CONSOLE 0

#endif
