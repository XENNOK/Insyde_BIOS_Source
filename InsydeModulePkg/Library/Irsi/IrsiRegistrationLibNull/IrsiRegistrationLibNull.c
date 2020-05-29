/** @file
  Helper functions for registering new IRSI functions

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

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>
#include <Library/IrsiRegistrationLib.h>

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
  VOID **Pointer
  )
{
  return EFI_UNSUPPORTED;
}

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
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Irsi Registerfunction

  This routine provides Irsi function registration

  @param FunctionName           Name of the IRSI function
  @param Guid                   IRSI function GUID
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
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Execute IRSI function

  @param  Guid                  IRSI Service Type GUID
  @param  FunctionNumber        Command code of IRSI function
  @param  CommBuf               Communication Buffer
  @param  BufSize               Buffer size

  @retval EFI_SUCCESS           IRSI function successfully executed
  @retval EFI_ABORT             Abort without executing IRSI function
  @return others                IRSI function executed with error
**/
EFI_STATUS
EFIAPI
IrsiExecuteFunction (
  EFI_GUID       *Guid,
  UINT32         FunctionNumber,
  VOID           *CommBuf,
  UINT32         BufSize
  )
{
  return EFI_UNSUPPORTED;
}

/**
  IRSI Get Runtime Buffer

  During runtime, the memory cannot be allocated by AllocatePool()
  This routine provides the pre-allocated IRSI runtime buffer for use at runtime

  @param IrsiBuffer             Pointer to the pre-allocated runtime buffer pointer
  @param BufferSize             Pointer to the IRSI runtime buffer size

  @retval EFI_SUCCESS           Function returns successfully
  @retval EFI_INVALID_PARAMETER Invalid parameter list with NULL IrsiBuffer or BufferSize pointer
  @retval EFI_ABORTED           Function aborted, IRSI Registration protocol not installed

**/
EFI_STATUS
EFIAPI
IrsiGetRuntimeBuffer (
  VOID    **IrsiBuffer,
  UINTN   *BufferSize
  )
{
  return EFI_UNSUPPORTED;
}