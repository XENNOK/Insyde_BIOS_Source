//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _COMMON_SMI_CALLBACK_
#define _COMMON_SMI_CALLBACK_

#include "Tiano.h"
#include "EfiSmmDriverLib.h"
#include "SmmDriverLib.h"

#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)
#include EFI_PROTOCOL_DEFINITION (SmmVariable)
#include EFI_GUID_DEFINITION (GlobalVariable)
#include EFI_PROTOCOL_DEPENDENCY (SmmSxDispatch)
#include EFI_PROTOCOL_DEPENDENCY (LegacyBios)

#include "CmosLib.h"

#define BDA(a)                    (*(UINT16*)((UINTN)0x400 +(a)))
#define EBDA(a)                   (*(UINT16*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (a)))
#define LEGACY_BOOT_TABLE_OFFSET  0x180
#define BDA_MEMORY_SIZE_OFFSET    0x13
#define EXT_DATA_SEG_OFFSET       0x0E 
#define EBDA_DEFAULT_SIZE         0x400

#define INVALID                   0xFF  

VOID
S4SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
;

#endif
