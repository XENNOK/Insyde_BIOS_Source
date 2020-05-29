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

#include "EfiApi.h"

#define EFI_DEBUG_COMM_PROTOCOL_GUID \
         {0xaf124cf2, 0x745f, 0x46a1, 0x86, 0xfc, 0x8f, 0x3a, 0xe3, 0x6d, 0x9f, 0xb0}

extern EFI_GUID gEfiDebugCommProtocolGuid;

typedef
struct _EFI_DEBUG_COMM_PROTOCOL EFI_DEBUG_COMM_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_SEND_INFO) (
  IN EFI_DEBUG_COMM_PROTOCOL      *This,
  IN EFI_PHYSICAL_ADDRESS         ImageBase,
  IN UINT32                       ImageSize,
  IN CHAR8                        *PdbPointer,
  IN EFI_PHYSICAL_ADDRESS         ImageEntry
  );

typedef
EFI_STATUS
(EFIAPI *EFI_RESET) (
  IN EFI_DEBUG_COMM_PROTOCOL      *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_DDT_PRINT) (
  IN EFI_DEBUG_COMM_PROTOCOL      *This,
  IN CHAR8                        *Format,
  IN VA_LIST                      Parameters
  );

typedef struct _EFI_DEBUG_COMM_PROTOCOL {
  EFI_SEND_INFO                   SendInfo;
  EFI_RESET                       Reset;
  EFI_DDT_PRINT                   DDTPrint;
} EFI_DEBUG_COMM_PROTOCOL;

#define INSYDE_DEBUG_SEND_INFO(a, b, c, d, e) \
{ \
  EFI_STATUS                    DebugStatus; \
  EFI_DEBUG_COMM_PROTOCOL       *DebugComm; \
  EFI_GUID                      EfiDebugCommProtocolGuid = EFI_DEBUG_COMM_PROTOCOL_GUID; \
  DebugStatus = a->LocateProtocol (&EfiDebugCommProtocolGuid, NULL, &DebugComm); \
  if (DebugStatus == EFI_SUCCESS) { \
    DebugComm->SendInfo(DebugComm, (EFI_PHYSICAL_ADDRESS)(b), (UINT32)(c), (CHAR8*)(d), (EFI_PHYSICAL_ADDRESS)(e)); \
  } \
}

#define INSYDE_DEBUG_RESET(a) \
{ \
  EFI_STATUS                    DebugStatus; \
  EFI_DEBUG_COMM_PROTOCOL       *DebugComm; \
  EFI_GUID                      EfiDebugCommProtocolGuid = EFI_DEBUG_COMM_PROTOCOL_GUID; \
  DebugStatus = a->LocateProtocol (&EfiDebugCommProtocolGuid, NULL, &DebugComm); \
  if (DebugStatus == EFI_SUCCESS) { \
    DebugComm->Reset(DebugComm); \
  } \
}

#define INSYDE_DEBUG_BYPASS_USB_DEBUG_PORT(a, b) \
{ \
  EFI_STATUS                    DebugStatus; \
  EFI_DEBUG_COMM_PROTOCOL       *DebugComm; \
  EFI_GUID                      EfiDebugCommProtocolGuid = EFI_DEBUG_COMM_PROTOCOL_GUID; \
  if ((b) == 0) { \
    DebugStatus = a->LocateProtocol (&EfiDebugCommProtocolGuid, NULL, &DebugComm); \
    if (DebugStatus == EFI_SUCCESS) { \
      continue; \
    } \
  } \
}

STATIC
VOID*
DDTGetDebugComm (
  )
{
#ifdef EFI32
  __asm
  {
    sub    esp, 0x10
    sidt   fword ptr [esp]
    mov    eax, [esp + 2]
    add    esp, 0x10
    mov    dx, [eax + 0x1e]
    shl    edx, 16
    mov    dx, [eax + 0x18]
    xor    eax, eax
    cmp    dword ptr [edx - 0x10], 0x44656267
    jne    quit
    mov    eax, [edx - 0x18]
quit:
  };
#elif defined(EFIX64)
  UINT8 OpCode[] = { 0x48, 0x83, 0xEC, 0x10, 0x0F, 0x01, 0x0C, 0x24, 0x48, 0x8B, 0x44, 0x24, 0x02, 0x48, 0x83, 0xC4,
                     0x10, 0x8B, 0x50, 0x38, 0x48, 0xC1, 0xE2, 0x10, 0x66, 0x8B, 0x50, 0x36, 0x48, 0xC1, 0xE2, 0x10, 
                     0x66, 0x8B, 0x50, 0x30, 0x48, 0x33, 0xC0, 0x81, 0x7A, 0xF0, 0x67, 0x62, 0x65, 0x44, 0x75, 0x04, 
                     0x48, 0x8B, 0x42, 0xE8, 0xC3 };
  VOID* (*DDTGetDebugCommX64)();
  *(UINT8**)&DDTGetDebugCommX64 = OpCode;
  return DDTGetDebugCommX64();
#endif
}

STATIC
VOID
DDTPrint (
  IN EFI_BOOT_SERVICES            *BootService,
  IN CHAR8                        *Format,
  ...
  )
{
  EFI_DEBUG_COMM_PROTOCOL       *DebugComm;
  VA_LIST                       Args;
  
  DebugComm = DDTGetDebugComm();
  if (DebugComm) {
    VA_START (Args, Format);
    DebugComm->DDTPrint(DebugComm, Format, Args);
    VA_END (Args);
  }
}

#endif