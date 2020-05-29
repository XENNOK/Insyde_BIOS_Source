/** @file
  Irsi Registration Protocol

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

#ifndef _IRSI_REGISTRATION_H_
#define _IRSI_REGISTRATION_H_

#include <Uefi.h>
#include <Guid/IrsiFeature.h>

#define IRSI_REGISTRATION_PROTOCOL_GUID \
  { 0x1C2E4602, 0xE3BA, 0x4B07, 0xE3, 0xB8, 0x35, 0x55, 0xA5, 0x1C, 0x61, 0x3A }


typedef struct _IRSI_REGISTRATION_PROTOCOL IRSI_REGISTRATION_PROTOCOL;

/**
  IRSI Registration Protocol function for registering IRSI functions

  @param Guid                   Runtime Services Type GUID
  @param FunctionNumber         Command code of the IRSI function
  @param IrsiFunction           The IRSI function pointer

  @retval EFI_SUCCESS           IRSI function successfully registered
  @retval EFI_ABORTED           faile to register IRSI function
  @retval EFI_ALREADY_STARTED   IRSI function already registered
**/
typedef
EFI_STATUS
(EFIAPI *IRSI_REGISTER_FUNCTION) (
  EFI_GUID       *Guid,
  UINT32         FunctionNumber,
  IRSI_FUNCTION IrsiFunction
  );

/**
  Execute IRSI function

  @param  Guid                  IRSI Service Type GUID
  @param  FunctionNumber        Command code of IRSI function
  @param  CommBuf               Communication Buffer
  @param  BufSize               Buffer size

  @return                       IRSI Status
**/
typedef
EFI_STATUS
(EFIAPI *IRSI_EXECUTE_FUNCTION) (
  EFI_GUID       *Guid,
  UINT32         FunctionNumber,
  VOID           *CommBuf,
  UINT32         BufSize
  );

/**
  Helper function for adding pointer to virtual conversion list

  @param  Pointer               pointer to be converted

  @retval EFI_SUCCESS           function returned successfully
  @retval EFI_ABORTED           System is at OS runtime, function aborted
**/
typedef
EFI_STATUS
(EFIAPI *IRSI_ADD_VIRTUAL_POINTER) (
  VOID **Pointer
  );

/**
  Helper function for removing pointer from virtual conversion list

  @param[in] Pointer            pointer to be removed

  @retval EFI_SUCCESS           function returned successfully.
  @retval EFI_NOT_FOUND         pointer isn't in the conversion list.
  @retval EFI_ABORTED           System is at OS runtime, function aborted.
**/
typedef
EFI_STATUS
(EFIAPI *IRSI_REMOVE_VIRTUAL_POINTER) (
  VOID **Pointer
  );

/**
  Helper function for geting the pre-allocated IRSI runtime buffer

  @param  IrsiBuf               pointer to the IRSI runtime buffer
  @param  IrsiBufSize           pointer to the IRSI buffer size

  @retval EFI_SUCCESS           function returned successfully
  @retval EFI_ABORTED           System is at OS runtime, function aborted
**/
typedef
EFI_STATUS
(EFIAPI *IRSI_GET_RUNTIME_BUFFER) (
  VOID  **IrsiBuf,
  UINTN *IrsiBufSize
  );

struct _IRSI_REGISTRATION_PROTOCOL {
  IRSI_REGISTER_FUNCTION      RegisterFunction;
  IRSI_EXECUTE_FUNCTION       ExecuteFunction;
  IRSI_ADD_VIRTUAL_POINTER    AddVirtualPointer;
  IRSI_REMOVE_VIRTUAL_POINTER RemoveVirtualPointer;
  IRSI_GET_RUNTIME_BUFFER     GetRuntimeBuffer;
};

extern EFI_GUID gIrsiRegistrationProtocolGuid;


#endif
