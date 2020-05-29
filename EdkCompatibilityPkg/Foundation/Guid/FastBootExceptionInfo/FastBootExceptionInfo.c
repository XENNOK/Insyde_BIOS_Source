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

  FastBootExceptionInfo.c

Abstract:


--*/

#include "Tiano.h"

#include EFI_GUID_DEFINITION (FastBootExceptionInfo)

EFI_GUID gFastBootExceptionInfoHobGuid  = FAST_BOOT_EXCEPTION_INFO_HOB_GUID;

EFI_GUID_STRING(&gFastBootExceptionInfoHobGuid, "FastBootExceptionInfoHob", "Fast Boot Exception Info HOB");
