/** @file
  This code abstracts SMM Int15 Service Protocol

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMM_INT15_SERVICE_PROTOCOL_H_
#define _SMM_INT15_SERVICE_PROTOCOL_H_

#include <Uefi.h>
//[-start-120531-IB09420077-modify]//
#include <Protocol/LegacyBios.h>
//[-end-120531-IB09420077-modify]//

#define EFI_SMM_INT15_SERVICE_PROTOCOL_GUID \
  { \
    0x1fa493a8, 0xb360, 0x4205, 0xb8, 0xfe, 0xcc, 0x83, 0xbc, 0x57, 0xb7, 0x3a \
  }

typedef struct _EFI_SMM_INT15_SERVICE_PROTOCOL EFI_SMM_INT15_SERVICE_PROTOCOL;

typedef
VOID
(EFIAPI *EFI_OEM_INT15_CALLBACK_FUNCTION) (
  IN  OUT EFI_IA32_REGISTER_SET   *CpuRegs, 
  IN      VOID                    *Context
  );

typedef struct _EFI_OEM_INT15_CALLBACK {
  EFI_OEM_INT15_CALLBACK_FUNCTION    Funcs;
} EFI_OEM_INT15_CALLBACK;    

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_INT15_INSTALL_PROTOCOL_INTERFACE) (
  IN EFI_SMM_INT15_SERVICE_PROTOCOL   *This,
  IN UINT16                           FunctionNum,
  IN EFI_OEM_INT15_CALLBACK_FUNCTION  FunctionPtr,
  IN VOID                             *Context
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_INT15_REINSTALL_PROTOCOL_INTERFACE) (
  IN EFI_SMM_INT15_SERVICE_PROTOCOL   *This,
  IN UINT16                           FunctionNum,
  IN EFI_OEM_INT15_CALLBACK_FUNCTION  FunctionPtr,
  IN VOID                             *Context
  );

//
// SMM INT15 SERVICE PROTOCOL
//
typedef struct _EFI_SMM_INT15_SERVICE_PROTOCOL{
  EFI_SMM_INT15_INSTALL_PROTOCOL_INTERFACE      InstallInt15ProtocolInterface;
  EFI_SMM_INT15_REINSTALL_PROTOCOL_INTERFACE    ReinstallInt15ProtocolInterface;
} EFI_SMM_INT15_SERVICE_PROTOCOL;

extern EFI_GUID gEfiSmmInt15ServiceProtocolGuid;

#endif
