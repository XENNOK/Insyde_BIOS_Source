/** @file
  VatsLib Library Instance implementation

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

#include <Library/IhisiLib.h>

STATIC
BOOLEAN
IhisiChecksumError (
  IN VOID                                *Ptr,
  IN UINTN                               Size,
  IN UINT16                              Checksum
  );

STATIC
UINT32
VatsLibStatusTranslation (
  IN EFI_STATUS                          Status
  );

STATIC
BOOLEAN
VatsSecurityCheckError (
  VOID
  );

/**
  AH=00h, Read the specific variable into the specified buffer.

  @retval EFI_SUCCESS        AL	00h = Function succeeded.
  @return Others             AL Returned error code.
**/
EFI_STATUS
VatsLibRead (
  VOID
  )
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
  InBuffer  = (IHISI_INPUT_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutBuffer = (IHISI_OUTPUT_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  VarName   = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_INPUT_BUFFER) / sizeof (UINT8)));
  VarData   = (UINT8 *) ((UINTN) OutBuffer + (sizeof (IHISI_OUTPUT_BUFFER) / sizeof (UINT8)));
  VarLength = (UINTN) OutBuffer->VarLength;
  if (BufferOverlapSmram ((VOID *) VarName, StrSize (VarName)) ||
      BufferOverlapSmram ((VOID *) VarData, VarLength)) {
    IhisiStatus = VatsWrongObFormat;
    IhisiLibErrorCodeHandler (IhisiStatus);
    return  EFI_INVALID_PARAMETER;
  }


  Status = CommonGetVariable (
             VarName,
             &InBuffer->VarGuid,
             &VarLength,
             VarData
             );

  if (Status == EFI_SUCCESS || Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Status is EFI_BUFFER_TOO_SMALL also need return required data size to application
    //
    OutBuffer->VarLength = (UINT32) VarLength;
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

/**
  AH=01h, Write the specified buffer to the specific variable.

  @retval EFI_SUCCESS        AL	00h = Function succeeded.
  @return Others             AL	Returned error code.
**/
EFI_STATUS
VatsLibWrite (
  VOID
  )
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
  InBuffer = (IHISI_INPUT_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutBuffer = (IHISI_OUTPUT_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  VarName = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_INPUT_BUFFER) / sizeof (UINT8)));
  VarData = (UINT8 *) ((UINTN) OutBuffer + (sizeof (IHISI_OUTPUT_BUFFER) / sizeof (UINT8)));

  if (BufferOverlapSmram ((VOID *) VarName, StrSize (VarName)) ||
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

  Status = CommonSetVariable (
             VarName,
             &InBuffer->VarGuid,
             OutBuffer->Attribute,
             (UINTN) OutBuffer->VarLength,
             VarData
             );

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

/**
  Initialize Vats relative services

  @retval EFI_SUCCESS        Initialize Vats services successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
VatsLibInit (
  VOID
  )
{
  return EFI_SUCCESS;
}

/**
  IHISI checksum calulation.

  @param[in] Ptr       A pointer to to a buffer.
  @param[in] Size      The buffer size.
  @param[in] Checksum  An element in IHISI output table.

  @retval TURE         Ckecksum error.
  @return FALSE        Ckecksum correct.
**/
STATIC
BOOLEAN
IhisiChecksumError (
  IN VOID                                *Ptr,
  IN UINTN                               Size,
  IN UINT16                              Checksum
  )
{
  UINTN      i;
  UINT8      *TempPtr;
  UINT32     Sum;

  TempPtr = Ptr;
  Sum = 0;

  for (i = 0 ; i < Size ; i++) {
    Sum = Sum + TempPtr[i];
  }

  Sum = (UINT32) ((Checksum + Sum) & 0x0000ffff);

  return (BOOLEAN) Sum;
}

/**
  EFI status translate to IHISI status.

  @param[in] Status    EFI_STATUS

  @return UINT32       IHISI status
**/
STATIC
UINT32
VatsLibStatusTranslation (
  IN EFI_STATUS                          Status
  )
{
  switch (Status) {
    case EFI_SUCCESS:
      return (UINT32) IhisiSuccess; //0x00
      break;

    case EFI_BUFFER_TOO_SMALL:
      return (UINT32) IhisiObLenTooSmall; //0x02
      break;

    case EFI_NOT_FOUND:
      return (UINT32) VatsVarNotFound; //0x10
      break;

    case EFI_INVALID_PARAMETER:
      return (UINT32) VatsWrongObFormat; //0x11
      break;

    default:
      return (UINT32) VatsVarPartAccessErr; //0x14
      break;
  }

}

/**
  EFI status translate to IHISI status.

  @param[in] Status    EFI_STATUS

  @return UINT32       IHISI status
**/
STATIC
BOOLEAN
VatsSecurityCheckError (
  VOID
  )
{
  IHISI_OUTPUT_BUFFER                   *OutBuffer;

  OutBuffer = (IHISI_OUTPUT_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  if ((OutBuffer->TableRev) != IHISI_TABLE_REVISION) {
    IhisiLibErrorCodeHandler((UINT32)VataObTableRevUnsupported);
    return TRUE;
  }

  if (((OutBuffer->TableId1) != IHISI_H2O_SIGNATURE) ||
      ((OutBuffer->TableId2) != IHISI_VAR_SIGNATURE) ||
      ((OutBuffer->TableId3) != IHISI_TBL_SIGNATURE)) {
    IhisiLibErrorCodeHandler ((UINT32)VatsWrongObFormat);
    return TRUE;
  }
  return FALSE;
}



