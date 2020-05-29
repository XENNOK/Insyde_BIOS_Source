/** @file
  Type definition for the Irsi Registration

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IRSI_REGISTRATION_RUNTIME_H_
#define _IRSI_REGISTRATION_RUNTIME_H_

#include <Uefi.h>
#include <Protocol/IrsiRegistration.h>

#define IRSI_FUNCTION_SIGNATURE      SIGNATURE_32 ('I','R','F','U')
#define VIRTUAL_POINTER_SIGNATURE    SIGNATURE_32 ('V','I','R','P')


typedef struct _IRSI_VIRTUAL_POINTER_ENTRY {
  UINT32                        Signature;
  LIST_ENTRY                    Link;
  VOID                          **Pointer;
} IRSI_VIRTUAL_POINTER_ENTRY;


typedef struct _IRSI_FUNCTION_ENTRY {
  UINT32                        Signature;
  LIST_ENTRY                    Link;
  EFI_GUID                      Guid;
  UINT32                        FunctionNumber;
  IRSI_FUNCTION                 Function;
} IRSI_FUNCTION_ENTRY;


typedef struct _IRSI_REGISTRATION_CONTEXT {
  UINT32                        Signature;
  IRSI_REGISTRATION_PROTOCOL    IrsiRegistration;
  LIST_ENTRY                    FunctionList;
  LIST_ENTRY                    VirtualPointerList;
  EFI_EVENT                     VirtualNotifyEvent;
  VOID                          *IrsiRuntimeBuffer;
  UINTN                         IrsiRuntimeBufferSize;
} IRSI_REGISTRATION_CONTEXT;

#define IRSI_REGISTRATION_CONTEXT_FROM_THIS(a)  CR(a, IRSI_REGISTRATION_CONTEXT, IrsiRegistration, IRSI_REGISTRATION_SIGNATURE)
#define IRSI_FUNCTION_ENTRY_FROM_LINK(a)        CR(a, IRSI_FUNCTION_ENTRY, Link, IRSI_FUNCTION_SIGNATURE)
#define VIRTUAL_POINTER_ENTRY_FROM_LINK(a)      CR(a, IRSI_VIRTUAL_POINTER_ENTRY, Link, VIRTUAL_POINTER_SIGNATURE)



#endif