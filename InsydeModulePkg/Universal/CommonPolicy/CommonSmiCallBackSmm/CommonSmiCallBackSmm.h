/** @file
  It register callbacks for common Sw or Sx(s3/S4).

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

#ifndef _COMMON_SMI_CALLBACK_SMM_
#define _COMMON_SMI_CALLBACK_SMM_

#include <Guid/GlobalVariable.h>


#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmSxDispatch.h>

#include <Protocol/LegacyBios.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CmosLib.h>
#include <Library/PrintLib.h>
#include <Library/VariableLib.h>
#include <Library/MemoryAllocationLib.h>

#define BDA(a)             (*(UINT16*)((UINTN)0x400 +(a)))
#define EBDA(a)            (*(UINT16*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (a)))
#define PREFIX_ZERO        0x20

#define LEGACY_BOOT_TABLE_OFFSET  0x180
#define BDA_MEMORY_SIZE_OFFSET    0x13
#define EXT_DATA_SEG_OFFSET       0x0E
#define EBDA_DEFAULT_SIZE         0x400

#define INVALID                   0xFF


VOID
EFIAPI
S4SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  );

EFI_STATUS
EFIAPI
S4SleepEntryCallBack2 (
  IN EFI_HANDLE                     DispatchHandle,
  IN CONST VOID                     *Context         OPTIONAL,
  IN OUT VOID                       *CommBuffer      OPTIONAL,
  IN OUT UINTN                      *CommBufferSize  OPTIONAL
  );

#endif
