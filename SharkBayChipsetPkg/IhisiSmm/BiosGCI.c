/** @file
  This driver provides some functions for Smart Tool

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BiosGCI.h"

//
// The following codes are the sample. Please customize here.
// The default codes are some dummy functions.
//
#if 0

EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *mBiosSmmFwBlockService;
EFI_SMM_VARIABLE_PROTOCOL               *mBiosSmmVariable;
UINT32                                  mCommandNumber;


STATIC
EFI_STATUS
SmmVariableCallback (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{

  return gSmst->SmmLocateProtocol (
                  &gEfiSmmVariableProtocolGuid,
                  NULL,
                  (VOID **)&mBiosSmmVariable
                  );
}

EFI_STATUS
BiosGCIInit (
  VOID
  )
{
  EFI_STATUS   Status;
  EFI_EVENT    SmmVariableEvent;


  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&mBiosSmmFwBlockService
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID **)&mBiosSmmVariable
                    );

  //
  // If not find gEfiSmmVariableProtocolGuid, Create a callback function to locate
  // gEfiSmmVariableProtocolGuid when gEfiSmmVariableProtocolGuid is installed
  //
  if (EFI_ERROR (Status)) {
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmVariableProtocolGuid,
                    SmmVariableCallback,
                    &SmmVariableEvent
                    );
  }

  return Status;
}

EFI_STATUS
DataAccessCommunication (
  VOID
  )
/*++

Routine Description:

  Implement IHISI SPEC. AH=60h, Data Access Communication.

Arguments:

  None

Returns:

--*/
{
  EFI_STATUS                        Status;
  UINT32                            IhisiStatus;
  DATA_ACCESS_COMMUNICATION_INPUT   *APCommunicationDataBuffer;
  DATA_ACCESS_COMMUNICATION_OUTPUT  *BIOSCommunicationDataBuffer;
  EFI_GUID                          IdentificationGuid = DATA_ACCESS_COMM_GUID_FOR_IDENTIFICATION;
  UINT32                            ApSize;
  UINT32                            ApDataSize;
  UINT8                             *StartType;


  Status = EFI_SUCCESS;
  IhisiStatus = IhisiSuccess;
  APCommunicationDataBuffer = NULL;
  BIOSCommunicationDataBuffer = NULL;

  //
  // Pointer to ECX AP communication data buffer (Input buffer).
  //
  //APCommunicationDataBuffer = (DATA_ACCESS_COMMUNICATION_INPUT *)(UINTN) SmmCpuSaveLocalState->ECX;
  APCommunicationDataBuffer = (DATA_ACCESS_COMMUNICATION_INPUT *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  //StartType = (UINT8 *)(UINTN) SmmCpuSaveLocalState->EDI;
  StartType = (UINT8 *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  //
  // Get and keep input parameters from input buffer.
  //
  ApSize         = APCommunicationDataBuffer->Size;
  mCommandNumber = APCommunicationDataBuffer->CommandNumber;
  ApDataSize     = APCommunicationDataBuffer->DataSize;

  //
  // Pointer to ECX BIOS communication data buffer (Output buffer).
  //
  //BIOSCommunicationDataBuffer = (DATA_ACCESS_COMMUNICATION_OUTPUT *)(UINTN) SmmCpuSaveLocalState->ECX;
  BIOSCommunicationDataBuffer = (DATA_ACCESS_COMMUNICATION_OUTPUT *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  //
  // Set output structure size.
  //
  BIOSCommunicationDataBuffer->Size = sizeof (DATA_ACCESS_COMMUNICATION_OUTPUT);

  //
  // Set data size default = 0 for event command.
  //
  BIOSCommunicationDataBuffer->DataSize = 0;


  //
  // Check platform GUID
  //
  if (!CompareGuid (&APCommunicationDataBuffer->GuidForIdentification, &IdentificationGuid)) {
    BIOSCommunicationDataBuffer->StatusReturn.ErrorStatus = 0;
    BIOSCommunicationDataBuffer->StatusReturn.General.GuidNotMatch = 1;
    CopyMem (&BIOSCommunicationDataBuffer->GuidForIdentification, &IdentificationGuid, sizeof (EFI_GUID));
    IhisiLibErrorCodeHandler (IhisiSuccess);
    return EFI_NOT_FOUND;
  }


//
// Note: The followings are the sample codes. You can customize your functions here.
//       Ihisi60hForCreateUefiVariable(), Ihisi60hForEraseUefiVariable(), Ihisi60hForQueryUefiVariableStatus(),
//       Ihisi60hForStartEvent() and Ihisi60hForFinishEvent() are the sample functions.
//

  //
  // Execute according to mUserDefCommandNum.
  //
  switch (mCommandNumber) {

  case DATA_ACCSEE_COMM_NUM_CREATE_UEFI_VARIABLE:
    BIOSCommunicationDataBuffer->StatusReturn.ErrorStatus = 0;
    Status = Ihisi60hForCreateUefiVariable (&IhisiStatus);
    if (EFI_ERROR (Status)) {
      //
      // Inform AP that create action fails.
      //
      BIOSCommunicationDataBuffer->StatusReturn.CreateUefiVariable.CreateVariableResult = DATA_ACCSEE_COMM_NUM_ERROR_RETURN_STATUS_FAIL;
      BIOSCommunicationDataBuffer->StatusReturn.CreateUefiVariable.Reserved = 0;
    }
    break;

  case DATA_ACCSEE_COMM_NUM_ERASE_UEFI_VARIABLE:
    BIOSCommunicationDataBuffer->StatusReturn.ErrorStatus = 0;
    Status = Ihisi60hForEraseUefiVariable (&IhisiStatus);
    if (EFI_ERROR (Status)) {
      //
      // Inform AP that create action fails.
      //
      BIOSCommunicationDataBuffer->StatusReturn.EraseUefiVariable.EraseVariableResult = DATA_ACCSEE_COMM_NUM_ERROR_RETURN_STATUS_FAIL;
      BIOSCommunicationDataBuffer->StatusReturn.EraseUefiVariable.Reserved = 0;
    }
    break;

  case DATA_ACCSEE_COMM_NUM_QUERY_DATA_FROM_UEFI_VARIABLE:
    BIOSCommunicationDataBuffer->StatusReturn.ErrorStatus = 0;
    Status = Ihisi60hForQueryUefiVariableStatus (&IhisiStatus);
    if (EFI_ERROR (Status)) {
      //
      // Inform AP that create action fails.
      //
      BIOSCommunicationDataBuffer->StatusReturn.QueryUefiVariableStatus.QueryVariableResult = DATA_ACCSEE_COMM_NUM_ERROR_RETURN_STATUS_FAIL;
      BIOSCommunicationDataBuffer->StatusReturn.QueryUefiVariableStatus.Reserved = 0;
    }
    break;

  case DATA_ACCSEE_COMM_NUM_READ_UEFI_VARIABLE_DATA:
    BIOSCommunicationDataBuffer->StatusReturn.ErrorStatus = 0;
    BIOSCommunicationDataBuffer->DataSize = UEFI_VARIABLE_SIZE;
    break;

  case DATA_ACCSEE_COMM_NUM_WRITE_UEFI_VARIABLE_DATA:
    BIOSCommunicationDataBuffer->StatusReturn.ErrorStatus = 0;
    //
    // Check data size
    //
    if (ApDataSize != UEFI_VARIABLE_SIZE) {
      BIOSCommunicationDataBuffer->StatusReturn.General.DataSizeNotAccept = 1;
      BIOSCommunicationDataBuffer->DataSize = UEFI_VARIABLE_SIZE;
    } else {
      BIOSCommunicationDataBuffer->DataSize = ApDataSize;
    }
    break;

  case DATA_ACCSEE_COMM_NUM_START_EVENT:
    BIOSCommunicationDataBuffer->StatusReturn.ErrorStatus = 0;
    Status = Ihisi60hForStartEvent (&IhisiStatus, *StartType);
    if (EFI_ERROR (Status)) {
      //
      // Inform AP that create action fails.
      //
    }
    break;

  case DATA_ACCSEE_COMM_NUM_FINISH_EVENT:
    BIOSCommunicationDataBuffer->StatusReturn.ErrorStatus = 0;
    Status = Ihisi60hForFinishEvent (&IhisiStatus);
    if (EFI_ERROR (Status)) {
      //
      // Inform AP that create action fails.
      //
    }
    break;

  default:
    //
    // Set ErrorReturn default to NotSupportDataType.
    //
    BIOSCommunicationDataBuffer->StatusReturn.General.CommandNotSupport = 1; // Not support this type.
    break;

  }

  IhisiLibErrorCodeHandler (IhisiStatus);
  return Status;
}

EFI_STATUS
DataRead (
  VOID
  )
/*++

Routine Description:

  Implement IHISI SPEC. AH=61h, Data Read.

Arguments:

  None

Returns:

--*/
{
  EFI_STATUS        Status                               = EFI_SUCCESS;
  UINT32            IhisiStatus                          = IhisiSuccess;
  UINT32            *BufferAddress                       = NULL;
  DATA_INPUT        *InDataBuffer                        = NULL;
  DATA_OUTPUT       *OutDataBuffer                       = NULL;
  EFI_GUID          UefiVariableGuid                     = UEFI_VARIABLE_GUID;
  UINTN             VariableSize                         = UEFI_VARIABLE_SIZE;
  UINT8             VariableData[UEFI_VARIABLE_SIZE - 1] = {0};
  UINT32            VariableOffset;
  UINT32            Attribute;


  //BufferAddress = (UINT32 *)(UINTN) SmmCpuSaveLocalState->ESI;
  //InDataBuffer = (DATA_INPUT *)(UINTN) SmmCpuSaveLocalState->EDI;
  BufferAddress = (UINT32 *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  InDataBuffer = (DATA_INPUT *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  VariableOffset = InDataBuffer->DataOffset;
  //OutDataBuffer = (DATA_OUTPUT *)(UINTN) SmmCpuSaveLocalState->EDI;
  OutDataBuffer = (DATA_OUTPUT *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);


  //
  // Note: The followings are the sample codes. You can customize your codes here.
  //

  Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  //
  // Execute by mDataTypeOfOemExtraData got from 60h.
  //
  switch (mCommandNumber) {

  case DATA_ACCSEE_COMM_NUM_CREATE_UEFI_VARIABLE:
  case DATA_ACCSEE_COMM_NUM_ERASE_UEFI_VARIABLE:
  case DATA_ACCSEE_COMM_NUM_QUERY_DATA_FROM_UEFI_VARIABLE:
    break;

  //
  // Command number = 0x00000003 or 0x00000004
  //
  case DATA_ACCSEE_COMM_NUM_READ_UEFI_VARIABLE_DATA:
  case DATA_ACCSEE_COMM_NUM_WRITE_UEFI_VARIABLE_DATA:
    //Status = mSmst->SmmAllocatePool (
    //                  EfiRuntimeServicesData,
    //                  VariableSize,
    //                  &InDataBuffer
    //                  );
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      VariableSize,
                      (VOID **)&InDataBuffer
                      );
    if (Status != EFI_SUCCESS) {
      IhisiLibErrorCodeHandler (IhisiObLenTooSmall);
      return EFI_BUFFER_TOO_SMALL;
    }
    Status = mBiosSmmVariable->SmmGetVariable (
                                 L"XFILE",
                                 &UefiVariableGuid,
                                 &Attribute,
                                 &VariableSize,
                                 &VariableData
                                 );
    if (!EFI_ERROR (Status)) {
      if (VariableOffset < UEFI_VARIABLE_SIZE) {
        CopyMem (
          //(VOID *)(UINTN) SmmCpuSaveLocalState->ESI,
          (VOID *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI),
          &VariableData,
          VariableSize
          );
        VariableOffset += UEFI_VARIABLE_OFFSET;
      }
      OutDataBuffer->Size = sizeof (DATA_OUTPUT);
      OutDataBuffer->DataSize = (UINT32) VariableSize;
      OutDataBuffer->DataOffset = VariableOffset;
    } else {
      return Status;
    }
    break;

  default:
    break;

  }

  IhisiLibErrorCodeHandler (IhisiStatus);
  return EFI_SUCCESS;
}

EFI_STATUS
DataWrite (
  VOID
  )
/*++

Routine Description:

  Implement IHISI SPEC. AH=62h, Data Write.

Arguments:

  None

Returns:

--*/
{
  EFI_STATUS        Status                             = EFI_SUCCESS;
  UINT32            IhisiStatus                        = IhisiSuccess;
  UINT32            *BufferAddress                     = NULL;
  DATA_INPUT        *InDataBuffer                      = NULL;
  DATA_OUTPUT       *OutDataBuffer                     = NULL;
  EFI_GUID          UefiVariableGuid                   = UEFI_VARIABLE_GUID;
  UINTN             VariableSize                       = UEFI_VARIABLE_SIZE;
  UINT32            VariableOffset;
  UINT32            Attribute;


  //BufferAddress = (UINT32 *)(UINTN) SmmCpuSaveLocalState->ESI;
  //InDataBuffer = (DATA_INPUT *)(UINTN) SmmCpuSaveLocalState->EDI;
  BufferAddress = (UINT32 *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  InDataBuffer = (DATA_INPUT *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  VariableOffset = InDataBuffer->DataOffset;

  //
  // Note: The followings are the sample codes. You can customize your codes here.
  //

  Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  //
  // Execute by mUserDefCommandNum got from 60h.
  //
  switch (mCommandNumber) {

  case DATA_ACCSEE_COMM_NUM_CREATE_UEFI_VARIABLE:
  case DATA_ACCSEE_COMM_NUM_ERASE_UEFI_VARIABLE:
  case DATA_ACCSEE_COMM_NUM_QUERY_DATA_FROM_UEFI_VARIABLE:
    break;

  //
  // Command number = 0x00000003 or 0x00000004
  //
  case DATA_ACCSEE_COMM_NUM_READ_UEFI_VARIABLE_DATA:
  case DATA_ACCSEE_COMM_NUM_WRITE_UEFI_VARIABLE_DATA:
    //Status = mSmst->SmmAllocatePool (
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      VariableSize,
                      (VOID **)&InDataBuffer
                      );
    Status = mBiosSmmVariable->SmmSetVariable (
                                 L"XFILE",
                                 &UefiVariableGuid,
                                 Attribute,
                                 VariableSize,
                                 BufferAddress
                                 );

    if (!EFI_ERROR (Status)) {
      if (VariableOffset < UEFI_VARIABLE_SIZE) {
        CopyMem (
          BufferAddress,
          //(VOID *)(UINTN) SmmCpuSaveLocalState->ESI,
          (VOID *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI),
          VariableSize
          );
        VariableOffset += UEFI_VARIABLE_OFFSET;
      }
      //OutDataBuffer = (DATA_OUTPUT *)(UINTN) SmmCpuSaveLocalState->EDI;
      OutDataBuffer = (DATA_OUTPUT *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
      OutDataBuffer->DataSize = (UINT32) VariableSize;
      OutDataBuffer->DataOffset = VariableOffset;
    } else {
      return Status;
    }
    break;

  default:
    break;

  }

  IhisiLibErrorCodeHandler (IhisiStatus);
  return EFI_SUCCESS;
}

EFI_STATUS
Ihisi60hForCreateUefiVariable (
  IN OUT UINT32                 *IhisiStatus
  )
{
  EFI_STATUS   Status;
  EFI_GUID     UefiVariableGuid                     = UEFI_VARIABLE_GUID;
  UINTN        VariableSize                         = UEFI_VARIABLE_SIZE;
  UINT8        VariableData[UEFI_VARIABLE_SIZE - 1] = {0};
  UINT32       Attribute;


  Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  Status = mBiosSmmVariable->SmmGetVariable (
                               L"XFILE",
                               &UefiVariableGuid,
                               &Attribute,
                               &VariableSize,
                               &VariableData
                               );

  if (EFI_ERROR (Status)) {
    Status = mBiosSmmVariable->SmmSetVariable (
                                 L"XFILE",
                                 &UefiVariableGuid,
                                 Attribute,
                                 VariableSize,
                                 &VariableData
                                 );
  } else {
    Status = EFI_ABORTED;
  }

  return Status;
}

EFI_STATUS
Ihisi60hForEraseUefiVariable (
  IN OUT UINT32                 *IhisiStatus
  )
{
  EFI_STATUS   Status;
  EFI_GUID     UefiVariableGuid                     = UEFI_VARIABLE_GUID;
  UINT32       VariableData[UEFI_VARIABLE_SIZE - 1] = {0};
  UINT32       Attribute;

  Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  Status = mBiosSmmVariable->SmmSetVariable (
                               L"XFILE",
                               &UefiVariableGuid,
                               Attribute,
                               0,
                               &VariableData
                               );
  return Status;
}

EFI_STATUS
Ihisi60hForQueryUefiVariableStatus (
  IN OUT UINT32                 *IhisiStatus
  )
{
  EFI_STATUS   Status;
  EFI_GUID     UefiVariableGuid                     = UEFI_VARIABLE_GUID;
  UINTN        VariableSize                         = UEFI_VARIABLE_SIZE;
  UINT8        VariableData[UEFI_VARIABLE_SIZE - 1] = {0};
  UINT32       Attribute;

  Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  Status = mBiosSmmVariable->SmmGetVariable (
                               L"XFILE",
                               &UefiVariableGuid,
                               &Attribute,
                               &VariableSize,
                               &VariableData
                               );
  return Status;
}

EFI_STATUS
Ihisi60hForStartEvent (
  IN OUT UINT32                 *IhisiStatus,
  IN     UINT8                  StartType
  )
{
  EFI_STATUS   Status;
  EFI_GUID     TestGuid1                            = TEST_GUID1;
  EFI_GUID     TestGuid2                            = TEST_GUID2;
  EFI_GUID     TestGuid3                            = TEST_GUID3;
  UINTN        VariableSize                         = UEFI_VARIABLE_SIZE;
  UINT8        VariableData[UEFI_VARIABLE_SIZE - 1] = {0};
  UINT32       Attribute;

  Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;

  switch (StartType) {

  case DATA_ACCSEE_COMM_NUM_START_EVENT_WINDOWS:
    Status = mBiosSmmVariable->SmmSetVariable (
                                 L"WINDOWS",
                                 &TestGuid1,
                                 Attribute,
                                 VariableSize,
                                 &VariableData
                                 );
    break;

  case DATA_ACCSEE_COMM_NUM_START_EVENT_DOS:
    Status = mBiosSmmVariable->SmmSetVariable (
                                 L"DOS",
                                 &TestGuid2,
                                 Attribute,
                                 VariableSize,
                                 &VariableData
                                 );
    break;

  case DATA_ACCSEE_COMM_NUM_START_EVENT_EFI:
    Status = mBiosSmmVariable->SmmSetVariable (
                                 L"EFI",
                                 &TestGuid3,
                                 Attribute,
                                 VariableSize,
                                 &VariableData
                                 );
    break;

  default:
    Status = EFI_ABORTED;
    break;

  }

  return Status;
}

EFI_STATUS
Ihisi60hForFinishEvent (
  IN OUT UINT32                 *IhisiStatus
  )
{
  EFI_STATUS   Status;
  EFI_GUID     TestGuid4                            = TEST_GUID4;
  UINTN        VariableSize                         = UEFI_VARIABLE_SIZE;
  UINT8        VariableData[UEFI_VARIABLE_SIZE - 1] = {0};
  UINT32       Attribute;

  Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  Status = mBiosSmmVariable->SmmGetVariable (
                               L"FINISH",
                               &TestGuid4,
                               &Attribute,
                               &VariableSize,
                               &VariableData
                               );

  if (EFI_ERROR (Status)) {
    Status = mBiosSmmVariable->SmmSetVariable (
                                 L"FINISH",
                                 &TestGuid4,
                                 Attribute,
                                 VariableSize,
                                 &VariableData
                                 );
  } else {
    Status = EFI_ABORTED;
  }

  return Status;
}

#else

EFI_STATUS
BiosGCIInit (
  VOID
  )
{

  return EFI_SUCCESS;
}

EFI_STATUS
DataAccessCommunication (
  VOID
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
DataRead (
  VOID
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
DataWrite (
  VOID
  )
{
  return EFI_SUCCESS;
}

#endif
