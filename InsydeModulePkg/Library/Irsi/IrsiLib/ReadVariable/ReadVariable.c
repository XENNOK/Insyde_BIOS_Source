/** @file
  Library Instance implementation for IRSI Read Variable

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
  Irsi ReadVariable function

  This routine provides OS to call GetVariable() UEFI runtime services

  @param ReadVariableBuf        pointer to IRSI_VARIABLE structure
  @param ReadVariableSize       the size of ReadVariableBuf, normally it is equal to
                                sizeof(IRSI_VARIABLE)

  @retval EFI_SUCESS            Variable successfully read
  @retval other error codes     failed to get variable

**/
EFI_STATUS
EFIAPI
IrsiReadVariable (
  VOID     *ReadVariableBuf
  )
{
   IRSI_VARIABLE   *IrsiVar;
   VOID             *VariableBuf;
   CHAR16           *VariableName;
   UINTN            VariableSize;
   EFI_STATUS       Status;

   IrsiVar = (IRSI_VARIABLE *)ReadVariableBuf;
   IrsiVar->Header.StructureSize = sizeof(IRSI_VARIABLE);

   VariableName = (CHAR16 *)(UINTN)IrsiVar->VariableNamePtr;
   VariableBuf = (VOID *)(UINTN)IrsiVar->VariableDataPtr;

   VariableSize = (UINTN)IrsiVar->VariableSize;
   Status = EfiGetVariable (
                    VariableName,
                    &IrsiVar->VariableGuid,
                    &IrsiVar->Attribute,
                    &VariableSize,
                    VariableBuf);
    IrsiVar->VariableSize = (UINT32)VariableSize;
    IrsiVar->Header.ReturnStatus = IRSI_STATUS(Status);
   
   return Status;
}

/**
  IRSI ReadVariable Initialization

  This routine is a LibraryClass constructor for IrsiReadVariable, it will
  register IrsiReadVariable function to the Irsi function database

  @param ImageHandle            A handle for the image that is initializing this driver
  @param SystemTable            A pointer to the EFI system table

  @retval EFI_SUCCESS           Module initialized successfully
  @retval Others                Module initialization failed

**/
EFI_STATUS
EFIAPI
IrsiReadVariableInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  Status = IrsiRegisterFunction (
               &gIrsiServicesGuid,
               IRSI_READ_VARIABLE_COMMAND,
               IrsiReadVariable
               );

  return Status;
}