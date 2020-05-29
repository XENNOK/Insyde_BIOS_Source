//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   TCG Memory Write Requested Control Variable and Physical Presence Interface Variable Guid
//;

#ifndef _EFI_TCG_VARIABLE_H_
#define _EFI_TCG_VARIABLE_H_

#include "Tiano.h"
#include "EfiTpm.h"

//
// Variable Name
//
#define EFI_MEMORY_OVERWRITE_REQUEST_CONTROL_VARIABLE_NAME        L"MemoryOverwriteRequestControl"
#define EFI_PHYSICAL_PRESENCE_INTERFACE_CONTROL_VARIABLE_NAME     L"PhysicalPresenceInterfaceControl"

//
// Variable Guid
//
#define MEMORY_ONLY_RESET_CONTROL_GUID \
  { 0xe20939be, 0x32d4, 0x41be, 0xa1, 0x50, 0x89, 0x7f, 0x85, 0xd4, 0x98, 0x29 }
#define PHYSICAL_PRESENCE_INTERFACE_CONTROL_GUID \
  { 0xE46909C4, 0x6702, 0x474d, 0x9B, 0xD1, 0x6D, 0x9F, 0xBC, 0x4B, 0x01, 0x90 }

//
// HOB Guid
//
#define TPM_DEV_INTERNAL_GUID \
  {0x99c71625, 0xbb3c, 0x49da, 0xb2, 0x7, 0x5f, 0x87, 0x9a, 0xec, 0xa5, 0x54}

typedef struct {
  UINT8                                   TpmDevice;
  UINT8                                   Tpm2Enable;
  UINT8                                   TpmHide;
} TPM_DEVICE_HOB;

//
// MOR Variable
//
#define bit(a)   1 << (a)

#define BIT_CLEAR_MEMORY                 bit(0)
#define BIT_DISABLE_AUTO_DETECT          bit(4)

#pragma pack (push, 1)
//
// PPI Variable
//
typedef struct _EFI_TCG_PPI_AREA {
  UINT8                             CurrentCommand;
  UINT8                             LastCommand;
  TPM_RESULT                        LastReturnedCode;
} EFI_TCG_PPI_AREA;
#pragma pack (pop)

extern EFI_GUID gMemoryOnlyResetControlGuid;
extern EFI_GUID gPhysicalPresenceInterfaceControlGuid;

#endif
