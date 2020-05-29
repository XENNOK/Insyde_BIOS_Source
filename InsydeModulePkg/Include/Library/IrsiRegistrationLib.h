/** @file
  IRSI Registration Library Class definitions

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

#ifndef _IRSI_REGISTRATION_LIB_H_
#define _IRSI_REGISTRATION_LIB_H_

#include <Protocol/IrsiRegistration.h>

typedef
UINT32
(EFIAPI *IRSI_RUNTIME_SERVICES_ENTRY) (
  VOID   *CommBuf,
  UINT32 BufSize
  );

/**
  Irsi Registerfunction

  This routine provides Irsi function registration

  @param Guid                   IRSI function GUID
  @param FunctionNumber         Command code of the IRSI function
  @param IrsiFunction           The IRSI function pointer

  @retval EFI_SUCCESS           IRSI function successfully registered
  @retval EFI_ABORTED           System aborted in OS runtime

**/
EFI_STATUS
EFIAPI
IrsiRegisterFunction (
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

  @return                       IRSR Status
**/
EFI_STATUS
EFIAPI
IrsiExecuteFunction (
  IN  EFI_GUID       *Guid,
  IN  UINT32         FunctionNumber,
  IN  VOID           *CommBuf,
  IN  UINT32         BufSize
  );

/**
  Irsi AddVirtualPointer function

  This routine adds pointer to the virtual conversion list

  @param Pointer                pointer to be converted

  @retval EFI_SUCCESS:          Pointer successfully added to virtual conversion list
  @retval EFI_ABORTED           Failed to add virtual pointer conversion list

**/
EFI_STATUS
EFIAPI
IrsiAddVirtualPointer (
  VOID           **Pointer
  );

/**
  Irsi RemoveVirtualPointer function

  This routine removes pointer from the virtual conversion list

  @param[in] Pointer            pointer to be removed

  @retval EFI_SUCCESS           Pointer successfully removed from virtual conversion list.
  @retval EFI_NOT_FOUND         pointer isn't in the conversion list.
  @retval EFI_ABORTED           Failed to remove virtual pointer conversion list.
**/
EFI_STATUS
EFIAPI
IrsiRemoveVirtualPointer (
  VOID **Pointer
  );

/**
  Install IRSI Runtime Services

  @param  RuntimeServicesGuid   IRSI Service Type GUID
  @param  BufSize               Buffer size

  @retval EFI_SUCCESS           IRSI function successfully executed
  @retval RuntimeServicesEntry  Runtime Services Entrypoint

  @retval EFI_SUCCESS           Runtime Services successfully installed
  @retval EFI_INVALID_PARAMETER NULL services GUID or NULL services entry given
  @retval EFI_DEVICE_ERROR      Memory allocation failed
  @return others                Runtime services Configuration table installation failed
**/
EFI_STATUS
EFIAPI
IrsiInstallRuntimeServices (
  EFI_GUID                     *RuntimeServicesGuid,
  IRSI_RUNTIME_SERVICES_ENTRY  RuntimeServicesEntry
  );

/**
  IRSI Get Runtime Buffer

  During runtime, the memory cannot be allocated by AllocatePool()
  This routine provides the pre-allocated IRSI runtime buffer for use at runtime

  @param IrsiBuffer             Pointer to the pre-allocated runtime buffer pointer
  @param BufferSize             Pointer to the IRSI runtime buffer size

  @retval EFI_SUCCESS           Function returns successfully
  @retval EFI_DEVICE_ERROR      Unable to get IRSI runtime buffer

**/
EFI_STATUS
EFIAPI
IrsiGetRuntimeBuffer (
  VOID    **IrsiBuffer,
  UINTN   *BufferSize
  );


#endif

