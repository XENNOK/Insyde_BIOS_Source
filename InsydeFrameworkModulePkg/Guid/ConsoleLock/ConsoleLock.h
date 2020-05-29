//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright 2005, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  ConsoleLock.h

Abstract:

  Console Lock Guid definitions

--*/

#ifndef _EFI_CONSOLE_LOCK_GUID_H_
#define _EFI_CONSOLE_LOCK_GUID_H_

#define EFI_CONSOLE_LOCK_GUID  \
  { 0x368cda0d, 0xcf31, 0x4b9b, 0x8c, 0xf6, 0xe7, 0xd1, 0xbf, 0xff, 0x15, 0x7e}

#define EFI_CONSOLE_LOCK_VARIABLE_NAME (L"ConsoleLock")
#define EFI_CONSOLE_LOCK_FORMAT_VARIABLE_NAME ("ConsoleLock")

extern CHAR16   gEfiConsoleLockName[];

extern EFI_GUID gEfiConsoleLockGuid;

#define LOCK_CONSOLE              1
#define NO_LOCK_CONSOLE           0

#endif
