//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _SMM_PnP_H_
#define _SMM_PnP_H_

#include "Tiano.h"
#include "EfiSmmDriverLib.h"
#include "Pnp.h"
#include "PnpDmi.h"
#include "FlashLayout.h"
#include "EfiHobLib.h"
#include "Kernel.h"
#include "OemPnpStrings.h"
#include "OemPnpGpnv.h"
#include "SmiTable.h"
#include "SmmDriverLib.h"

#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)
#include EFI_PROTOCOL_DEFINITION (SmmVariable)
#include EFI_PROTOCOL_CONSUMER (SmmSwDispatch)

#include EFI_PROTOCOL_DEFINITION (SmmOemServices)

#include EFI_GUID_DEFINITION (FlashMapHob)
#include EFI_GUID_DEFINITION (Hob)
#include EFI_PROTOCOL_DEFINITION (SmmFwBlockService)
#include EFI_PROTOCOL_DEFINITION (EventLog)
#include EFI_PROTOCOL_PRODUCER (LegacyBios)

extern EFI_SMM_RUNTIME_PROTOCOL           *mSmmRT;
extern EFI_SMM_SYSTEM_TABLE               *mSmst;
extern EFI_SMM_VARIABLE_PROTOCOL          *mSmmVariable;
extern EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlockService;
extern EFI_EVENT_LOG_PROTOCOL             *SmmEventLog;

extern UINTN                              UpdateableStringCount;
extern DMI_UPDATABLE_STRING               *mUpdatableStrings;

extern UINTN                              OemGPNVHandleCount;
extern OEM_DEFINE_GPNV_HANDLE             *mOemGPNVHandleTab;


#define GPNV_SIGNATURE                    EFI_SIGNATURE_32 ('G', 'P', 'N', 'V')


#pragma pack(1)

typedef struct {
  UINT16                                Handle;
  UINTN                                 MinGPNVSize;
  UINTN                                 GPNVSize;
  UINTN                                 BaseAddress;
} PLATFORM_GPNV_MAP;

typedef struct {
  PLATFORM_GPNV_MAP                 PlatFormGPNVMap[20];
} PLATFORM_GPNV_MAP_BUFFER;

//
// GPNV Header
//
typedef struct {
  UINT32                            Signature;//"GPNV"
  UINT32                            TotalLength;
  UINT8                             Reserved[8];
} PLATFORM_GPNV_HEADER;


#pragma pack()

VOID
PnPBiosCallback (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  EFI_SMM_SW_DISPATCH_CONTEXT       *DispatchContext
  );

VOID
GetGPNVMap (
  VOID
  );

EFI_STATUS
FillPlatformGPNVMapBuffer (
  IN UINT16   Handle,
  IN UINTN    MinGPNVSize,
  IN UINTN    GPNVSize,
  IN UINTN    BaseAddress
);

extern PLATFORM_GPNV_MAP_BUFFER         GPNVMapBuffer;
extern PLATFORM_GPNV_HEADER             *GPNVHeader;

#endif
