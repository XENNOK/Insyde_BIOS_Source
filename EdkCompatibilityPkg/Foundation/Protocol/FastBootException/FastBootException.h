/*++

Copyright (c)  1999 - 2011 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FastBootException.h

Abstract:

  This file defines Fast Boot Exception abstraction protocol containing the
  exception type and category, which is the indication of an exception occurrence.

--*/

#ifndef _FAST_BOOT_EXCEPTION_H_
#define _FAST_BOOT_EXCEPTION_H_

#include <FastBootDataDef.h>

#define FAST_BOOT_EXCEPTION_PROTOCOL_GUID \
  {0x3da3f62e, 0x291e, 0x4224, 0x80, 0x5f, 0x2d, 0x5e, 0xb6, 0xb2, 0x9c, 0xe2}

EFI_FORWARD_DECLARATION (FAST_BOOT_EXCEPTION_PROTOCOL);

typedef struct _FAST_BOOT_EXCEPTION_PROTOCOL {
  FAST_BOOT_EXCEPTION_TYPE      FbExceptionType;
  FAST_BOOT_EXCEPTION_CATEGORY  FbExceptionCategory;
} FAST_BOOT_EXCEPTION_PROTOCOL;

extern EFI_GUID gFastBootExceptionProtocolGuid;

#endif
