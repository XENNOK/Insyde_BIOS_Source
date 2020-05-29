//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
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
//;   VatsLib.c
//;

#include "IhisiLib.h"

BOOLEAN
IhisiChecksumError (
  IN VOID                                *Ptr,
  IN UINTN                               Size,
  IN UINT16                              Checksum
  );

UINT32
VatsLibStatusTranslation (
  IN EFI_STATUS                          Status
  );

BOOLEAN
VatsSecurityCheckError (
  VOID
  );

STATIC
VOID
EFIAPI
SmmVariableCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  );

EFI_SMM_VARIABLE_PROTOCOL               *SmmVariable;

EFI_STATUS
VatsLibRead (
  VOID
  )
/*++

Routine Description:

  AH=00h, Read the specific variable into the specified buffer.

Input:

  DS:ESI	Pointer to the beginning of the VATS input buffer.
  DS:EDI	Pointer to the beginning of the VATS output buffer for read.

Output:
  Success:
    AL	00h = Function succeeded.

  Failure:
    AL	Returned error code.

--*/
{
  IHISI_INPUT_BUFFER                     *InBuffer;
  IHISI_OUTPUT_BUFFER                    *OutBuffer;
  UINT8                                  *VarData;
  CHAR16                                 *VarName;
  EFI_STATUS                             Status;
  UINTN                                  VarLength;
  UINT32                                 IhisiStatus;

  //
  // Check the valid signature "H2O$Var$Tbl" in obTableId filed.
  //
  if (VatsSecurityCheckError ()) {
    return EFI_UNSUPPORTED;
  }

  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:EDI  Pointer to the beginning of the VATS output buffer.
  //
  InBuffer  = (IHISI_INPUT_BUFFER *)(UINTN)(SmmCpuSaveLocalState->ESI);
  OutBuffer = (IHISI_OUTPUT_BUFFER *)(UINTN)(SmmCpuSaveLocalState->EDI);

  VarName   = (CHAR16 *)((UINTN)InBuffer + (sizeof(IHISI_INPUT_BUFFER)/sizeof(UINT8)));
  VarData   = (UINT8 *)((UINTN)OutBuffer + (sizeof(IHISI_OUTPUT_BUFFER)/sizeof(UINT8)));
  VarLength = (UINTN)OutBuffer->VarLength;
  if (BufferOverlapSmram ((VOID *) VarName, EfiStrSize (VarName)) ||
      BufferOverlapSmram ((VOID *) VarData, VarLength)) {
    IhisiStatus = VatsWrongObFormat;
    IhisiLibErrorCodeHandler (IhisiStatus);
    return  EFI_INVALID_PARAMETER;
  }

  Status = EFI_UNSUPPORTED;
  if (SmmVariable != NULL) {
    Status = SmmVariable->GetVariable (
                  VarName,
                  &InBuffer->VarGuid,
                  NULL,
                  &VarLength,
                  VarData
                  );
  }

  if (Status == EFI_SUCCESS || Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Status is EFI_BUFFER_TOO_SMALL also need return required data size to application
    //
    OutBuffer->VarLength = (UINT32)VarLength;
  }

  //
  // Translate EFI status to Ihisi status.
  //
  IhisiStatus = VatsLibStatusTranslation (Status);

  //
  // Return Ihisi Status to AL.
  //
  IhisiLibErrorCodeHandler (IhisiStatus);

  return Status;
}

EFI_STATUS
VatsLibWrite (
  VOID
  )
/*++

Routine Description:

    AH=01h, Write the specified buffer to the specific variable.

Input:

    DS:ESI	Pointer to the beginning of the VATS input buffer.
    DS:EDI	Pointer to the beginning of the VATS output buffer for write.

Output:
  Success:
    AL	00h = Function succeeded.

  Failure:
    AL	Returned error code.
--*/
{
  IHISI_INPUT_BUFFER                     *InBuffer;
  IHISI_OUTPUT_BUFFER                    *OutBuffer;
  UINT8                                  *VarData;
  CHAR16                                 *VarName;
  EFI_STATUS                             Status;
  UINT32                                 IhisiStatus;

  //
  // Check the valid signature "H2O$Var$Tbl" in obTableId filed.
  //
  if (VatsSecurityCheckError ()) {
    return EFI_UNSUPPORTED;
  }

  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:EDI  Pointer to the beginning of the VATS output buffer.
  //
  InBuffer = (IHISI_INPUT_BUFFER *)(UINTN)(SmmCpuSaveLocalState->ESI);
  OutBuffer = (IHISI_OUTPUT_BUFFER *)(UINTN)(SmmCpuSaveLocalState->EDI);

  VarName = (CHAR16 *)((UINTN)InBuffer + (sizeof(IHISI_INPUT_BUFFER)/sizeof(UINT8)));
  VarData = (UINT8 *)((UINTN)OutBuffer + (sizeof(IHISI_OUTPUT_BUFFER)/sizeof(UINT8)));
  if (BufferOverlapSmram ((VOID *) VarName, EfiStrSize (VarName)) ||
      BufferOverlapSmram ((VOID *) VarData, OutBuffer->VarLength)) {
    IhisiStatus = VatsWrongObFormat;
    IhisiLibErrorCodeHandler (IhisiStatus);
    return  EFI_INVALID_PARAMETER;
  }
  if (IhisiChecksumError (VarData, OutBuffer->VarLength, OutBuffer->VarChecksum)) {

    IhisiStatus = VatsObChecksumFailed;
    IhisiLibErrorCodeHandler (IhisiStatus);
    return  EFI_INVALID_PARAMETER;
  }
  if (OutBuffer->VarLength == 0) {
    OutBuffer->Attribute = 0;
  } else if (OutBuffer->Attribute == 0) {
    OutBuffer->Attribute = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  }

  Status = EFI_UNSUPPORTED;
  if (SmmVariable != NULL) {
    Status = SmmVariable->SetVariable (
                  VarName,
                  &InBuffer->VarGuid,
                  OutBuffer->Attribute,
                  (UINTN) OutBuffer->VarLength,
                  VarData
                  );
   }

  //
  // Translate EFI status to Ihisi status.
  //
  IhisiStatus = VatsLibStatusTranslation (Status);

  //
  // Return Ihisi Status to AL.
  //
  IhisiLibErrorCodeHandler (IhisiStatus);

  return Status;
}

EFI_STATUS
VatsLibInit (
  VOID
  )
{
  EFI_EVENT   SmmVariableEvent;
  EFI_STATUS  Status;

  Status = mSmmRT->LocateProtocol (
                     &gEfiSmmVariableProtocolGuid,
                     NULL,
                     &SmmVariable
                     );
  //
  // If can not find gEfiSmmVariableProtocolGuid, Create callback function to locate
  // gEfiSmmVariableProtocolGuid when gEfiSmmVariableProtocolGuid is installed
  //
  if (EFI_ERROR (Status)) {
    Status = mSmmRT->EnableProtocolNotify (
                       SmmVariableCallback,
                       NULL,
                       &gEfiSmmVariableProtocolGuid,
                       &SmmVariableEvent
                       );
  }

  return Status;
}

BOOLEAN
IhisiChecksumError (
  IN VOID                                *Ptr,
  IN UINTN                               Size,
  IN UINT16                              Checksum
  )
/*++
Routine Description:

  IHISI checksum calulation

Arguments:

  Ptr       - A pointer to to a buffer
  Size      - The buffer size
  Checksum  - An element in IHISI output table

Returns:

  TURE      - Ckecksum error.
  FALSE     - Ckecksum correct.

--*/
{
  UINTN      i;
  UINT8      *TempPtr;
  UINT32     Sum;

  TempPtr = Ptr;
  Sum = 0;

  for (i = 0 ; i < Size ; i++) {
    Sum = Sum + TempPtr[i];
  }

  Sum = (UINT32)((Checksum + Sum) & 0x0000ffff);

  return (BOOLEAN)Sum;
}


UINT32
VatsLibStatusTranslation (
  IN EFI_STATUS                          Status
  )
/*++
Routine Description:

  EFI status translate to IHISI status.

Arguments:

  Status: EFI_STATUS

Returns:

  IHISI status

--*/
{
  switch (Status) {
    case EFI_SUCCESS:
      return (UINT32)IhisiSuccess; //0x00
      break;

    case EFI_BUFFER_TOO_SMALL:
      return (UINT32)IhisiObLenTooSmall; //0x02
      break;

    case EFI_NOT_FOUND:
      return (UINT32)VatsVarNotFound; //0x10
      break;

    case EFI_INVALID_PARAMETER:
      return (UINT32)VatsWrongObFormat; //0x11
      break;

    default:
      return (UINT32)VatsVarPartAccessErr; //0x14
      break;
  }

}

BOOLEAN
VatsSecurityCheckError (
  VOID
  )
{
  IHISI_OUTPUT_BUFFER                   *OutBuffer;

  OutBuffer = (IHISI_OUTPUT_BUFFER *)(UINTN)(SmmCpuSaveLocalState->EDI);

  if ((OutBuffer->TableRev) != IHISI_TABLE_REVISION) {
    IhisiLibErrorCodeHandler((UINT32)VataObTableRevUnsupported);
    return 1;
  }

  if (((OutBuffer->TableId1) != IHISI_H2O_SIGNATURE) ||
      ((OutBuffer->TableId2) != IHISI_VAR_SIGNATURE) ||
      ((OutBuffer->TableId3) != IHISI_TBL_SIGNATURE)) {
    IhisiLibErrorCodeHandler ((UINT32)VatsWrongObFormat);
    return 1;
  }
  return 0;
}

STATIC
VOID
EFIAPI
SmmVariableCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  mSmmRT->LocateProtocol (
            &gEfiSmmVariableProtocolGuid,
            NULL,
            &SmmVariable
            );
  return;
}
