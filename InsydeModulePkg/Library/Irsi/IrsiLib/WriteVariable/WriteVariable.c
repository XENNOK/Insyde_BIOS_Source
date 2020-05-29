/** @file
  Library Instance implementation for IRSI Write Variable

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/IrsiLib.h>

/**
  Irsi WriteVariable function

  This routine provides OS to call SetVariable() UEFI runtime services

  @param WriteVariableBuf       pointer to IRSI_VARIABLE structure
  @param WriteVariableSize      the size of WriteVariableBuf, normally it is equal to
                                sizeof(IRSI_VARIABLE)

  @retval EFI_SUCESS            Variable successful written
  @retval other error codes     faile to set variable

**/
EFI_STATUS
EFIAPI
IrsiWriteVariable(
  VOID     *WriteVariableBuf
  )
{
   IRSI_VARIABLE   *IrsiVar;
   VOID             *VariableBuf;
   CHAR16           *VariableName;
   EFI_STATUS       Status;

   IrsiVar = (IRSI_VARIABLE *)WriteVariableBuf;
   IrsiVar->Header.StructureSize = sizeof(IRSI_VARIABLE);

   VariableName = (CHAR16 *)(UINTN)IrsiVar->VariableNamePtr;
   VariableBuf = (VOID *)(UINTN)IrsiVar->VariableDataPtr;

   Status = EfiSetVariable (
                    VariableName,
                    &IrsiVar->VariableGuid,
                    IrsiVar->Attribute,
                    (UINTN)IrsiVar->VariableSize,
                    VariableBuf);
   IrsiVar->Header.ReturnStatus = IRSI_STATUS(Status);
   return Status;
}

/**
  IRSI WriteVariable Initialization

  This routine is a LibraryClass constructor for IrsiWriteVariable, it will
  register IrsiWriteVariable function to the Irsi function database

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully
  @retval Others                Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiWriteVariableInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_WRITE_VARIABLE_COMMAND,
               IrsiWriteVariable
               );

  return Status;
}