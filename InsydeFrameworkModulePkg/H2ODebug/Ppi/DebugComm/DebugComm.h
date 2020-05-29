//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; 
//; Module Name:
//; 
//;   DebugComm.h
//; 

#ifndef _DEBUG_COMM_H_
#define _DEBUG_COMM_H_

#include "Pei.h"
#include "EfiApi.h"

#define PEI_DEBUG_COMM_PROTOCOL_GUID \
         {0x8BE3CAA7, 0xF138, 0x41ce, 0x87, 0x71, 0xD3, 0xED, 0x87, 0x62, 0x65, 0xC9}

EFI_FORWARD_DECLARATION (PEI_DEBUG_COMM_PROTOCOL);

extern EFI_GUID gPeiDebugCommProtocolGuid;

typedef
struct _PEI_DEBUG_COMM_PROTOCOL PEI_DEBUG_COMM_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *PEI_SEND_INFO) (
  IN PEI_DEBUG_COMM_PROTOCOL      *This,
  IN EFI_PHYSICAL_ADDRESS         ImageBase,
  IN UINT32                       ImageSize,
  IN CHAR8                        *PdbPointer,
  IN EFI_PHYSICAL_ADDRESS         ImageEntry
  );

typedef
EFI_STATUS
(EFIAPI *PEI_DDT_PRINT) (
  IN PEI_DEBUG_COMM_PROTOCOL      *This,
  IN CHAR8                        *Format,
  IN VA_LIST                      Parameters
  );

typedef struct _PEI_DEBUG_COMM_PROTOCOL {
  PEI_SEND_INFO                   SendInfo;
  PEI_DDT_PRINT                   DDTPrint; 
} PEI_DEBUG_COMM_PROTOCOL;

#define INSYDE_DEBUG_SEND_INFO(a, b, c, d, e) \
{ \
  EFI_STATUS                    DebugStatus; \
  PEI_DEBUG_COMM_PROTOCOL       *DebugComm; \
  EFI_GUID                      PeiDebugCommProtocolGuid = PEI_DEBUG_COMM_PROTOCOL_GUID; \
  DebugStatus = (**(a)).LocatePpi (a, &PeiDebugCommProtocolGuid, 0, NULL, &DebugComm); \
  if (DebugStatus == EFI_SUCCESS) { \
    DebugComm->SendInfo(DebugComm, (EFI_PHYSICAL_ADDRESS)((UINT32)(b)), (UINT32)(c), (CHAR8*)(d), (EFI_PHYSICAL_ADDRESS)((UINT32)(e))); \
  } \
}

#define INSYDE_DEBUG_RESET(a)

STATIC
VOID
DDTPrint (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN CHAR8                        *Format,
  ...
  )
{
  EFI_STATUS                    DebugStatus;
  PEI_DEBUG_COMM_PROTOCOL       *DebugComm;
  EFI_GUID                      PeiDebugCommProtocolGuid = PEI_DEBUG_COMM_PROTOCOL_GUID;
  VA_LIST                       Args;
  DebugStatus = (**PeiServices).LocatePpi (PeiServices, &PeiDebugCommProtocolGuid, 0, NULL, &DebugComm);
  if (DebugStatus == EFI_SUCCESS) {
    VA_START (Args, Format);
    DebugComm->DDTPrint(DebugComm, Format, Args);
    VA_END (Args);
  }
}

#endif