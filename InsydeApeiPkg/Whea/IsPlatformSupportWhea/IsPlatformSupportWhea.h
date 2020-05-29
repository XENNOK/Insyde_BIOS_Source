/** @file

  IsPlatformSupportWhea driver

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IS_PLATFORM_SUPPORT_WHEA_H_
#define _IS_PLATFORM_SUPPORT_WHEA_H_

///=================================================
///  MdePkg\Include\
///=================================================
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>

///=================================================
///  $(INSYDE_APEI_PKG)\Include\
///=================================================
#include <Protocol/IsPlatformSupportWhea.h>

#include <SetupConfig.h>

#endif
