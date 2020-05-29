/** @file
  Runtime DXE driver implementation for IRSI

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
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/IrsiRegistrationLib.h>

/**
  The IRSI runtime services entry point

  @param CommBuf                A handle for the image that is initializing this driver
  @param BufSize                A pointer to the EFI system table

  @retval 0                     The runtime services function successfully returns
  @retval Others                Failed to execute the runtime services function

**/
UINT32
EFIAPI
IrsiServicesEntry (
  VOID   *CommBuf,
  UINT32 BufSize
  )
{
  EFI_STATUS      Status;
  IRSI_HEADER     *IrsiHeader;
  
  IrsiHeader = (IRSI_HEADER *)CommBuf;
  
  Status = IrsiExecuteFunction (&gIrsiServicesGuid, IrsiHeader->FunctionNumber, CommBuf, BufSize);
  return IRSI_STATUS(Status);
}

/**
  The IRSA runtime services entry point

  @param CommBuf                A handle for the image that is initializing this driver
  @param BufSize                A pointer to the EFI system table

  @retval 0                     The runtime services function successfully returns
  @retval Others                Failed to execute the runtime services function

**/
UINT32
EFIAPI
IrsaServicesEntry (
  VOID   *CommBuf,
  UINT32 BufSize
  )
{
  EFI_STATUS      Status;
  IRSI_HEADER     *IrsiHeader;

  IrsiHeader = (IRSI_HEADER *)CommBuf;

  Status = IrsiExecuteFunction (&gIrsaServicesGuid, IrsiHeader->FunctionNumber, CommBuf, BufSize);
  return IRSI_STATUS(Status);
}

/**
  The ISB runtime services entry point

  @param CommBuf                A handle for the image that is initializing this driver
  @param BufSize                A pointer to the EFI system table

  @retval 0                     The runtime services function successfully returns
  @retval Others                Failed to execute the runtime services function

**/
UINT32
EFIAPI
IsbServicesEntry (
  VOID   *CommBuf,
  UINT32 BufSize
  )
{
  EFI_STATUS      Status;
  IRSI_HEADER     *IrsiHeader;

  IrsiHeader = (IRSI_HEADER *)CommBuf;

  Status = IrsiExecuteFunction (&gIsbServicesGuid, IrsiHeader->FunctionNumber, CommBuf, BufSize);
  return IRSI_STATUS(Status);
}


/**
  Irsi RuntimeDXE Initialization

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully
  @retval Others                Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiRuntimeInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{

  //
  // Install the runtime services entry points to the ConfigurationTable
  //
  IrsiInstallRuntimeServices (&gIrsiServicesGuid, IrsiServicesEntry);
  IrsiInstallRuntimeServices (&gIrsaServicesGuid, IrsaServicesEntry);
  IrsiInstallRuntimeServices (&gIsbServicesGuid, IsbServicesEntry);

  IrsiAddVirtualPointer((VOID **)&gRT);
  return EFI_SUCCESS;
}


