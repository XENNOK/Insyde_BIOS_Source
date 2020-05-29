/** @file
  FactoryDefaultLib Library Instance implementation

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
#include <Library/VariableSupportLib.h>

#include <Guid/ImageAuthentication.h>

//
// signature define
//
#define COMMON_REGION_INPUT_SIGNATURE   SIGNATURE_32 ('$', 'R', 'D', 'I')
#define COMMON_REGION_OUTPUT_SIGNATURE  SIGNATURE_32 ('$', 'C', 'D', 'O')
//
// Data type definition
//
#define FACTORY_COPY_EVENT                                   0x01
#define FACTORY_COPY_READ                                    0x02
#define FACTORY_COPY_RESTORE_WITH_CLEARING_ALL_SETTINGS      0x03
#define FACTORY_COPY_RESTORE_WITH_RESERVING_OTHER_SETTINGS   0x04

#define FACTORY_COPY_HEADER_SIZE                             0x08


//
// Data size definition
//
#define COMMON_REGION_SIZE_64K                 0x00
#define COMMON_REGION_SIZE_128K                0x01
#define COMMON_REGION_SIZE_256K                0x02
#define COMMON_REGION_SIZE_512K                0x03
#define COMMON_REGION_SIZE_1024K               0x04
#define COMMON_REGION_SIZE_2048K               0x05
#define COMMON_REGION_SIZE_4096K               0x06
#define COMMON_REGION_SIZE_8192K               0x07
#define COMMON_REGION_SIZE_SPECIFIC            0xFD

//
// Block size definition
//
#define COMMON_REGION_BLOCK_SIZE_4K            0x00
#define COMMON_REGION_BLOCK_SIZE_64K           0x01
#define COMMON_REGION_BLOCK_SIZE_SPECIFIC      0xFF

//
// Status definition
//
#define UNSUPPORTED_TYPE                       0x8000

typedef
EFI_STATUS
(EFIAPI *COMMON_REGION_COMMUNICATION_FUNCTION) (
  VOID
  );

#pragma pack (push, 1)
typedef struct {
  UINT32       Signature;
  UINT32       StructureSize;
  UINT8        DataType;
  UINT8        DataSize;
  UINT32       PysicalDataSize;
} INPUT_DATA_STRUCTURE;

typedef struct {
  UINT32       Signature;
  UINT32       StructureSize;
  UINT8        BlockSize;
  UINT8        DataSize;
  UINT32       PysicalDataSize;
  UINT16       Status;
} OUTPUT_DATA_STRUCTURE;
#pragma pack (pop)

typedef struct {
  BOOLEAN      WantToReadData;
  BOOLEAN      InCommonReadProcess;
  UINTN        BlockSize;
  UINTN        DataSize;
  UINTN        RemainDataSize;
} COMMON_REGION_READ_STRUCTURE;

typedef struct {
  UINT8                                     DataType;
  COMMON_REGION_COMMUNICATION_FUNCTION      Function;
} COMMON_REGION_COMMUNICATION_FUNCTION_TABLE;

STATIC
EFI_STATUS
EFIAPI
WriteDefaultSettingsToFactoryCopy (
  VOID
  );

STATIC
EFI_STATUS
EFIAPI
ReadDefaultSettingsToFactoryCopy (
  VOID
  );

STATIC
EFI_STATUS
EFIAPI
RestoreFactoryCopyWithClearingAllSettings (
  VOID
  );

STATIC
EFI_STATUS
EFIAPI
RestoreFactoryCopyWithReservingOtherSettings (
  VOID
  );

STATIC COMMON_REGION_COMMUNICATION_FUNCTION_TABLE mFunctionTable []= {
  {FACTORY_COPY_EVENT, WriteDefaultSettingsToFactoryCopy},
  {FACTORY_COPY_READ, ReadDefaultSettingsToFactoryCopy},
  {FACTORY_COPY_RESTORE_WITH_CLEARING_ALL_SETTINGS, RestoreFactoryCopyWithClearingAllSettings},
  {FACTORY_COPY_RESTORE_WITH_RESERVING_OTHER_SETTINGS, RestoreFactoryCopyWithReservingOtherSettings}
  };
STATIC COMMON_REGION_READ_STRUCTURE  mCommonRegionReadControl = {0};



/**
  Get all of secure boot settings from factory copy region.

  @param[in, out] BufferSize     On input, Total buffer size.
                                 On output, The size of all secure boot settings.
  @param[out]     Buffer         Buffer to save secure boot settings.

  @retval EFI_SUCCESS            Collect all of secure boot settings successful.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory failed.
**/
EFI_STATUS
CollectSecureBootSettings (
  IN OUT UINTN            *BufferSize,
  OUT    UINT8            *Buffer
  )
{
  UINTN               TotalSize;
  UINTN               LastVariableIndex;
  UINT8               *ReadBuffer;
  UINTN               HeaderSize;
  SPECIFIC_VARIABLE   GetVariable [] = {IsPkVariable, IsKekVariable, IsDbVariable, IsDbxVariable};
  BOOLEAN             VariableFound;
  VARIABLE_HEADER     *VariableHeader;
  VARIABLE_HEADER     *InDeletedVariable;
  UINTN               Index;
  UINTN               FunctionCnt;
  UINTN               CopySize;
  EFI_STATUS          Status;

  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();;
  if (BufferSize == NULL || Buffer == NULL || *BufferSize < HeaderSize) {
    return EFI_INVALID_PARAMETER;
  }
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopySize) - HeaderSize,
                    (VOID **)&ReadBuffer
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  TotalSize = *BufferSize;
  LastVariableIndex = 0;
  FunctionCnt = sizeof (GetVariable) / sizeof (SPECIFIC_VARIABLE);
  for (Index = 0; Index < FunctionCnt; Index++) {
    VariableFound = FALSE;
    VariableHeader = (VARIABLE_HEADER *) (Buffer + HeaderSize);
    InDeletedVariable = NULL;
    while (IsValidVariableHeader (VariableHeader) && (UINTN) VariableHeader - (UINTN) Buffer < TotalSize) {
      if (VariableHeader->State == VAR_ADDED &&
          GetVariable[Index] ((CHAR16 *) (VariableHeader + 1), &VariableHeader->VendorGuid)) {
        CopySize = (UINTN) GetNextVariablePtr (VariableHeader) - (UINTN) VariableHeader;
        if (CopySize + LastVariableIndex + HeaderSize > (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopySize)) {
          gSmst->SmmFreePool (ReadBuffer);
          ASSERT (FALSE);
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (&ReadBuffer[LastVariableIndex], VariableHeader, CopySize);
        LastVariableIndex += CopySize;
        VariableFound = TRUE;
        break;
      } else if (VariableHeader->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION) &&
                 GetVariable[Index] ((CHAR16 *) (VariableHeader + 1), &VariableHeader->VendorGuid)) {
        InDeletedVariable = VariableHeader;
      }
      VariableHeader = GetNextVariablePtr (VariableHeader);
    }
    if (!VariableFound && InDeletedVariable != NULL) {
      CopySize = (UINTN) GetNextVariablePtr (InDeletedVariable) - (UINTN) InDeletedVariable;
      if (CopySize + LastVariableIndex + HeaderSize > (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopySize)) {
        gSmst->SmmFreePool (ReadBuffer);
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (&ReadBuffer[LastVariableIndex], InDeletedVariable, CopySize);
      LastVariableIndex += CopySize;
      VariableFound = TRUE;
    }
    //
    // if factory copy doesn't have dbx variable, it still a legal factory default copy.
    //
    if (!VariableFound && Index != FunctionCnt - 1)  {
      gSmst->SmmFreePool (ReadBuffer);
      return EFI_NOT_FOUND;
    }
  }
  //
  // copy collected secure boot settings to input buffer
  //
  SetMem (Buffer + HeaderSize, TotalSize - HeaderSize, 0xFF);
  CopyMem (Buffer + HeaderSize, ReadBuffer, LastVariableIndex);
  gSmst->SmmFreePool (ReadBuffer);
  //
  // Setting output buffer size
  //
  *BufferSize = LastVariableIndex + HeaderSize;
  return EFI_SUCCESS;
}

/**
  The implementation of IHISI function 49h type 2. return default factory default settings
  information to application.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMETER  Input buffer from register is invalid.
**/
STATIC
EFI_STATUS
EFIAPI
ReadDefaultSettingsToFactoryCopy (
  VOID
  )
{
  OUTPUT_DATA_STRUCTURE          *OutputData;

  OutputData = (OUTPUT_DATA_STRUCTURE *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (BufferOverlapSmram (OutputData, sizeof (OUTPUT_DATA_STRUCTURE))) {
    IhisiLibErrorCodeHandler (IhisiAccessProhibited);
    return EFI_INVALID_PARAMETER;
  }
  OutputData->BlockSize = COMMON_REGION_BLOCK_SIZE_4K;
  if (PcdGet32 (PcdFlashNvStorageFactoryCopySize) == 0x10000) {
    OutputData->DataSize = COMMON_REGION_SIZE_64K;
  } else {
    OutputData->DataSize = COMMON_REGION_SIZE_SPECIFIC;
    OutputData->PysicalDataSize = PcdGet32 (PcdFlashNvStorageFactoryCopySize);
  }
  mCommonRegionReadControl.WantToReadData = TRUE;
  mCommonRegionReadControl.InCommonReadProcess = FALSE;
  mCommonRegionReadControl.BlockSize = 0x1000;
  mCommonRegionReadControl.DataSize = PcdGet32 (PcdFlashNvStorageFactoryCopySize);
  mCommonRegionReadControl.RemainDataSize = PcdGet32 (PcdFlashNvStorageFactoryCopySize);
  IhisiLibErrorCodeHandler (IhisiSuccess);
  return EFI_SUCCESS;
}

/**
  The implementation of IHISI function 49h type 1. Write secure boot settings from
  variable store to factory copy region.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES  Allocate pool failed.
  @retval Others                Other error occurred in this function.
**/
STATIC
EFI_STATUS
EFIAPI
WriteDefaultSettingsToFactoryCopy (
  VOID
  )
{
  EFI_STATUS                              Status;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *SmmFwBlockService;
  UINT8                                   *Buffer;
  UINTN                                   ReadBufferSize;
  UINTN                                   WriteBufferSize;
  UINTN                                   WriteCount;
  UINTN                                   EraseSize;
  UINTN                                   WriteSize;
  BOOLEAN                                 WriteSuccess;


  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&SmmFwBlockService
                    );
  if (EFI_ERROR(Status)) {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    (UINTN) PcdGet32 (PcdFlashNvStorageVariableSize),
                    (VOID **)&Buffer
                    );
  if (EFI_ERROR(Status)) {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  ReadBufferSize = (UINTN) PcdGet32 (PcdFlashNvStorageVariableSize);
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) != 0 ?
                                (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) : PcdGet32 (PcdFlashNvStorageVariableBase),
                                0,
                                &ReadBufferSize,
                                Buffer
                                );
  if (EFI_ERROR(Status)) {
    gSmst->SmmFreePool (Buffer);
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  WriteBufferSize = ReadBufferSize;
  Status = CollectSecureBootSettings (&WriteBufferSize, Buffer);
  if (EFI_ERROR(Status) || ReadBufferSize < WriteBufferSize + FACTORY_COPY_HEADER_SIZE) {
    gSmst->SmmFreePool (Buffer);
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  //
  // Combine factory copy header and foctory copy content
  //
  CopyMem (Buffer + FACTORY_COPY_HEADER_SIZE, Buffer, WriteBufferSize);
  ReadBufferSize = FACTORY_COPY_HEADER_SIZE;
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                PcdGet32 (PcdFlashNvStorageFactoryCopyBase),
                                0,
                                &ReadBufferSize,
                                Buffer
                                );
  WriteBufferSize += FACTORY_COPY_HEADER_SIZE;
  if (EFI_ERROR(Status)) {
    gSmst->SmmFreePool (Buffer);
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  WriteCount = 0;
  WriteSuccess = FALSE;
  do {
    EraseSize = PcdGet32 (PcdFlashNvStorageFactoryCopySize);
    Status = SmmFwBlockService->EraseBlocks (
                                  SmmFwBlockService,
                                  PcdGet32 (PcdFlashNvStorageFactoryCopyBase),
                                  (UINTN *) &EraseSize
                                  );
    if (!EFI_ERROR (Status)) {
      WriteSize = WriteBufferSize;
      Status = SmmFwBlockService->Write (
                                    SmmFwBlockService,
                                    PcdGet32 (PcdFlashNvStorageFactoryCopyBase),
                                    (UINTN *) &WriteSize,
                                    Buffer
                                    );
      if (!EFI_ERROR (Status)) {
        WriteSuccess = TRUE;
      }
    }
    WriteCount++;
  } while (WriteCount < 100 && !WriteSuccess);

  gSmst->SmmFreePool (Buffer);
  WriteSuccess ? IhisiLibErrorCodeHandler (IhisiSuccess) : IhisiLibErrorCodeHandler (IhisiFunNotSupported);
  return WriteSuccess ? EFI_SUCCESS : Status;
}

/**
  Initialize output data for IHISI function 49h

  @param[out] OutputData        Pointer to OUTPUT_DATA_STRUCTURE structure.

  @retval EFI_SUCCESS           Initailize output data successful.
  @retval EFI_INVALID_PARAMETER OutputData is NULL.
**/
EFI_STATUS
InitializeOutputData (
  OUT OUTPUT_DATA_STRUCTURE       *OutputData
  )
{
  if (OutputData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OutputData->Signature = COMMON_REGION_OUTPUT_SIGNATURE;
  OutputData->StructureSize = sizeof (OUTPUT_DATA_STRUCTURE);
  OutputData->Status = 0;

  return EFI_SUCCESS;
}

/**
  The entry point of IHISI function 49h.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_INVALID_PARAMETER The type of function 49H is unsupported.
  @return Others                Other error occurred in this function.
**/
EFI_STATUS
CommonRegionDataCommunication (
  VOID
  )
{
  UINTN                          Index;
  UINT8                          DataType;
  EFI_STATUS                     Status;
  OUTPUT_DATA_STRUCTURE          *OutputData;
  INPUT_DATA_STRUCTURE           *InputData;

  InputData  = (INPUT_DATA_STRUCTURE *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  OutputData = (OUTPUT_DATA_STRUCTURE *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  if (BufferOverlapSmram ((VOID *) InputData, sizeof (INPUT_DATA_STRUCTURE)) ||
      BufferOverlapSmram ((VOID *) OutputData, sizeof (OUTPUT_DATA_STRUCTURE))) {
    IhisiLibErrorCodeHandler (IhisiAccessProhibited);
    return EFI_INVALID_PARAMETER;
  }

  DataType = InputData->DataType;
  Status = InitializeOutputData (OutputData);
  ASSERT_EFI_ERROR (Status);

  for (Index = 0; Index < sizeof (mFunctionTable) / sizeof (COMMON_REGION_COMMUNICATION_FUNCTION_TABLE); Index++) {
    if (DataType == mFunctionTable[Index].DataType) {
      Status = mFunctionTable[Index].Function ();
      break;
    }
  }

  if (Index == sizeof (mFunctionTable) / sizeof (COMMON_REGION_COMMUNICATION_FUNCTION_TABLE) || EFI_ERROR (Status)) {
    OutputData->Status = UNSUPPORTED_TYPE;
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  The entry point of IHISI function 4Bh.

  @retval EFI_SUCCESS           The function completed successfully
  @retval EFI_BUFFER_TOO_SMALL  The size of input buffer is too small to save read data.
  @return Others                Other error causes this function cannot work properly.
**/
EFI_STATUS
CommonRegionDataRead (
  VOID
  )
{
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *SmmFwBlockService;
  UINT8                                   *OutputBuffer;
  UINT32                                  OutputBufferSize;
  UINTN                                   ReadBufferSize;
  EFI_STATUS                              Status;


  Status = EFI_UNSUPPORTED;
  if (mCommonRegionReadControl.WantToReadData) {
    OutputBuffer = (UINT8 *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
    CopyMem (&OutputBufferSize, (VOID *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI), sizeof (UINT32));
    if (OutputBuffer == NULL || OutputBufferSize != mCommonRegionReadControl.BlockSize) {
      IhisiLibErrorCodeHandler (IhisiFunNotSupported);
      return Status;
    }
    if (BufferOverlapSmram ((VOID *) OutputBuffer, OutputBufferSize)) {
      IhisiLibErrorCodeHandler (IhisiAccessProhibited);
      return EFI_INVALID_PARAMETER;
    }
    mCommonRegionReadControl.InCommonReadProcess = TRUE;
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmFwBlockServiceProtocolGuid,
                      NULL,
                      (VOID **)&SmmFwBlockService
                      );
    if (EFI_ERROR(Status)) {
      IhisiLibErrorCodeHandler (IhisiFunNotSupported);
      return Status;
    }
    ReadBufferSize = (UINTN) OutputBufferSize;
    Status = SmmFwBlockService->Read (
                                  SmmFwBlockService,
                                  PcdGet32 (PcdFlashNvStorageFactoryCopyBase),
                                  mCommonRegionReadControl.DataSize - mCommonRegionReadControl.RemainDataSize,
                                  &ReadBufferSize,
                                  OutputBuffer
                                  );

    if (mCommonRegionReadControl.RemainDataSize > OutputBufferSize) {
      mCommonRegionReadControl.RemainDataSize -= OutputBufferSize;
    } else {
    //
    // The factory copy size must be integer multiple of flash blocks. If RemainDataSize smaller than
    // output buffer size indicates this read is inavlid
    //
      if (mCommonRegionReadControl.RemainDataSize < OutputBufferSize) {
        Status = EFI_OUT_OF_RESOURCES;
      }
      mCommonRegionReadControl.WantToReadData = FALSE;
      mCommonRegionReadControl.RemainDataSize = 0;
      mCommonRegionReadControl.InCommonReadProcess = FALSE;
    }


    if (!EFI_ERROR(Status)) {
      OutputBufferSize = (UINT32) ReadBufferSize;
      CopyMem ((VOID *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI), &OutputBufferSize, sizeof (UINT32));
      IhisiLibErrorCodeHandler (IhisiSuccess);
    } else {
      IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    }
  } else {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
  }

  return Status;
}

/**
  This funciotn uses to export merged factory default setting. The merged factory copy
  default setting means keep all of original variable data and then merge variable data
  in factory copy region with original data. If finding the same variable, system will
  use the variable in factory default to replace oringnal variable.

  @param[in, out] DataSize       On input, the size in bytes of the return Data buffer.
                                 On output the size of data returned in Data buffer.
  @param[out]     Data           The buffer to return the contents of the variable.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMETER  Some input parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL   The VariableStoreLength is too small for the result.  DataSize has
                                 been updated with the size needed to complete the request.
  @return Others                 Other error causes merge factory default setting failed.
**/
EFI_STATUS
ExportMergedFactoryDefaultSetting (
  IN OUT UINTN                                *DataSize,
  OUT    UINT8                                *Data
  )
{
  UINTN                                 VariableBufferLength;
  UINTN                                 WorkingBufferLength;
  UINTN                                 FactoryCopyRegionSize;
  UINT8                                 *VariableBuffer;
  UINT8                                 *WorkingBuffer;
  EFI_STATUS                            Status;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     *SmmFwBlockService;

  if (DataSize == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  WorkingBuffer       = NULL;
  VariableBuffer      = NULL;
  WorkingBufferLength = 0;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&SmmFwBlockService
                    );
  if (EFI_ERROR(Status)) {
    goto Done;
  }
  ASSERT (PcdGet32 (PcdFlashNvStorageVariableSize) >= PcdGet32 (PcdFlashNvStorageFactoryCopySize));
  //
  // Read whole factory copy sub-region data
  //
  WorkingBufferLength = PcdGet32 (PcdFlashNvStorageVariableSize);
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    WorkingBufferLength,
                    (VOID **)&WorkingBuffer
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  SetMem (WorkingBuffer, WorkingBufferLength, 0xFF);

  FactoryCopyRegionSize = PcdGet32 (PcdFlashNvStorageFactoryCopySize) - FACTORY_COPY_HEADER_SIZE;
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                PcdGet32 (PcdFlashNvStorageFactoryCopyBase),
                                FACTORY_COPY_HEADER_SIZE,
                                &FactoryCopyRegionSize,
                                WorkingBuffer
                                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Read whole variable store data
  //
  VariableBufferLength = PcdGet32 (PcdFlashNvStorageVariableSize);
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    VariableBufferLength,
                    (VOID **)&VariableBuffer
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  SetMem (VariableBuffer, VariableBufferLength, 0xFF);
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) != 0 ?
                                (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) : PcdGet32 (PcdFlashNvStorageVariableBase),
                                0,
                                &VariableBufferLength,
                                VariableBuffer
                                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = MergeVariableToFactoryCopy (WorkingBuffer, &WorkingBufferLength, VariableBuffer, VariableBufferLength);


Done:

  if (!EFI_ERROR (Status)) {
    if (WorkingBufferLength <= *DataSize) {
      CopyMem (Data, WorkingBuffer, WorkingBufferLength);
    } else {
      Status = EFI_BUFFER_TOO_SMALL;
    }
    *DataSize = WorkingBufferLength;
  }

  if (WorkingBuffer != NULL) {
    gSmst->SmmFreePool (WorkingBuffer);
  }

  if (VariableBuffer != NULL) {
    gSmst->SmmFreePool (VariableBuffer);
  }

  return Status;

}

/**
  Checks the contian of factory copy region is valid or not

  @retval TRUE   The contain in factory copy region is valid.
  @retval FALSE  The contain in factory copy region is invalid.
**/
STATIC
BOOLEAN
IsFacotryCopyValid (
  VOID
  )
{
  EFI_STATUS                             Status;
  UINTN                                  ReadBufferSize;
  UINTN                                  *ReadBuffer;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL      *SmmFwBlockService;
  UINTN                                  Index;
  UINTN                                  NumberOfInteger;
  BOOLEAN                                IsValid;

  IsValid = FALSE;
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&SmmFwBlockService
                    );
  if (EFI_ERROR(Status)) {
    return IsValid;
  }

  ReadBuffer     = NULL;
  ReadBufferSize = (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopySize) - FACTORY_COPY_HEADER_SIZE;
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    ReadBufferSize,
                    (VOID **)&ReadBuffer
                    );
  if (EFI_ERROR (Status)) {
    return IsValid;
  }

  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopyBase),
                                FACTORY_COPY_HEADER_SIZE,
                                &ReadBufferSize,
                                (UINT8 *) ReadBuffer
                                );
  if (!EFI_ERROR (Status)) {
    NumberOfInteger = ReadBufferSize / sizeof (UINTN);
    for (Index = 0; Index < NumberOfInteger; Index++) {
      if (ReadBuffer[Index] != (UINTN) (-1)) {
        IsValid = TRUE;
        break;
      }
    }
  }

  gSmst->SmmFreePool (ReadBuffer);
  return IsValid;
}

/**
  This funciotn uses to export factory default setting to input buffer.

  @param[in, out] DataSize       On input, the size in bytes of the return Data buffer.
                                 On output the size of data returned in Data buffer.
  @param[out]     Data           The buffer to return the contents of the variable.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMETER  Some input parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL   The VariableStoreLength is too small for the result.  DataSize has
                                 been updated with the size needed to complete the request.
  @return Others                 Other error causes merge factory default setting failed.
**/
EFI_STATUS
ExportFactoryDefaultSetting (
  IN OUT UINTN                                *DataSize,
  OUT    UINT8                                *Data
  )
{
  UINTN                                 HeaderSize;
  UINT8                                 *HeaderBuffer;
  UINT8                                 *FactoryDefaultBuffer;
  EFI_STATUS                            Status;
  UINTN                                 RequiredDataSize;
  UINTN                                 ReadDataSize;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL    *SmmFwBlockService;

  if (DataSize == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RequiredDataSize = (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopySize) - FACTORY_COPY_HEADER_SIZE;
  if (*DataSize < RequiredDataSize) {
    *DataSize = RequiredDataSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&SmmFwBlockService
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  HeaderBuffer = NULL;
  FactoryDefaultBuffer = NULL;
  //
  // Read original firmware volume header and variable store header
  //
  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    HeaderSize,
                    (VOID **)&HeaderBuffer
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) != 0 ?
                                (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) : PcdGet32 (PcdFlashNvStorageVariableBase),
                                0,
                                &HeaderSize,
                                HeaderBuffer
                                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  //
  // Read whole factory store data except for factory default header, firmware volume header and variable store header.
  //
  ReadDataSize = RequiredDataSize - HeaderSize;
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    ReadDataSize,
                    (VOID **)&FactoryDefaultBuffer
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                (UINTN) PcdGet32 (PcdFlashNvStorageFactoryCopyBase),
                                FACTORY_COPY_HEADER_SIZE + HeaderSize,
                                &ReadDataSize,
                                FactoryDefaultBuffer
                                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  CopyMem (Data, HeaderBuffer, HeaderSize);
  CopyMem (Data + HeaderSize, FactoryDefaultBuffer, RequiredDataSize - HeaderSize);
  *DataSize = RequiredDataSize;

Done:

  if (FactoryDefaultBuffer != NULL) {
    gSmst->SmmFreePool (FactoryDefaultBuffer);
  }

  if (HeaderBuffer != NULL) {
    gSmst->SmmFreePool (HeaderBuffer);
  }

  return Status;
}

/**
  According to restore type to collect factory default settings and restore these settings
  in output buffer.

  @param[in]      RestoreType           Factory copy restore type.
  @param[in, out] BufferSize            On input, the size in bytes of the return BufferSize.
                                        On output the size of data returned in BufferSize.
  @param[out]     FactoryDefaultBuffer  The buffer to return the contents of the factory default settings.

  @retval EFI_SUCCESS                   The function completed successfully.
  @retval EFI_INVALID_PARAMETER         Some input parameter is invalid.
  @retval EFI_BUFFER_TOO_SMALL          The BufferSize is too small for the result.  BufferSize has
                                        been updated with the size needed to complete the request.
  @retval EFI_NOT_READY                 The contain of factory copy region is invalid.
  @return Others                        Other error cause collect factory default setting failed.
**/
EFI_STATUS
CollectFactoryDefaultSetting (
  IN     UINTN                      RestoreType,
  IN OUT UINTN                      *BufferSize,
  OUT    UINT8                      *FactoryDefaultBuffer
  )
{
  EFI_STATUS                             Status;

  if (RestoreType != FACTORY_COPY_RESTORE_WITH_CLEARING_ALL_SETTINGS &&
      RestoreType != FACTORY_COPY_RESTORE_WITH_RESERVING_OTHER_SETTINGS) {
    return EFI_INVALID_PARAMETER;
  }

  if (BufferSize == NULL || FactoryDefaultBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!IsFacotryCopyValid ()) {
    return EFI_NOT_READY;
  }

  Status = EFI_INVALID_PARAMETER;
  switch (RestoreType) {

  case FACTORY_COPY_RESTORE_WITH_CLEARING_ALL_SETTINGS:
    Status = ExportFactoryDefaultSetting (BufferSize, FactoryDefaultBuffer);
    break;

  case FACTORY_COPY_RESTORE_WITH_RESERVING_OTHER_SETTINGS:
    Status = ExportMergedFactoryDefaultSetting (BufferSize, FactoryDefaultBuffer);
    break;

  }

  return Status;
}

/**
  According to restore type to restore factory default.

  @param[in] RestoreType          Input restore type. this funtion only supports FACTORY_COPY_RESTORE_WITH_CLEARING_ALL_SETTINGS
                                  and FACTORY_COPY_RESTORE_WITH_RESERVING_OTHER_SETTINGS. Other restore type will return
                                  EFI_INVALId_PARAMETER.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_INVALID_PARAMETER   Any input parameter is invalid.
  @return Others                  Other error cause collect factory default setting failed.

**/
EFI_STATUS
RestoreFactory (
  IN  UINTN     RestoreType
  )
{
  EFI_STATUS                              Status;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *SmmFwBlockService;
  UINTN                                   WriteCount;
  UINTN                                   EraseSize;
  UINTN                                   WriteSize;
  BOOLEAN                                 WriteSuccess;
  UINT8                                   *Buffer;
  UINTN                                   BufferSize;

  if (RestoreType != FACTORY_COPY_RESTORE_WITH_CLEARING_ALL_SETTINGS &&
      RestoreType != FACTORY_COPY_RESTORE_WITH_RESERVING_OTHER_SETTINGS) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&SmmFwBlockService
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (PcdGet32 (PcdFlashNvStorageFactoryCopySize) <= PcdGet32 (PcdFlashNvStorageVariableSize));
  BufferSize = (UINTN) PcdGet32 (PcdFlashNvStorageVariableSize);
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    BufferSize,
                    (VOID **)&Buffer
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  SetMem (Buffer, BufferSize, 0xFF);

  Status = CollectFactoryDefaultSetting (RestoreType, &BufferSize, Buffer);
  if (EFI_ERROR (Status)) {
    gSmst->SmmFreePool (Buffer);
    return Status;
  }

  WriteSuccess = FALSE;
  WriteCount   = 0;
  do {
    EraseSize = (UINTN) PcdGet32 (PcdFlashNvStorageVariableSize);
    Status = SmmFwBlockService->EraseBlocks (
                                  SmmFwBlockService,
                                  (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) != 0 ?
                                  (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) : PcdGet32 (PcdFlashNvStorageVariableBase),
                                  (UINTN *) &EraseSize
                                  );
    if (!EFI_ERROR (Status)) {
      WriteSize = BufferSize;
      Status = SmmFwBlockService->Write (
                                    SmmFwBlockService,
                                    (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) != 0 ?
                                    (UINTN) PcdGet64 (PcdFlashNvStorageVariableBase64) : PcdGet32 (PcdFlashNvStorageVariableBase),
                                    (UINTN *) &WriteSize,
                                    Buffer
                                    );
      if (!EFI_ERROR (Status)) {
        WriteSuccess = TRUE;
      }
    }
    WriteCount++;
  } while (WriteCount < 100 && !WriteSuccess);

  gSmst->SmmFreePool (Buffer);

  return WriteSuccess ? EFI_SUCCESS : Status;
}

/**
  The implementation of IHISI function 49h type 3. Restore variable data from factory copy region and
  clear all of variable data in variable store.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval Others                  Other error cause restoring factory copy failed.
**/
STATIC
EFI_STATUS
EFIAPI
RestoreFactoryCopyWithClearingAllSettings (
  VOID
  )
{
  EFI_STATUS     Status;

  Status = RestoreFactory (FACTORY_COPY_RESTORE_WITH_CLEARING_ALL_SETTINGS);

  if (!EFI_ERROR (Status)) {
    IhisiLibErrorCodeHandler (IhisiSuccess);
  } else {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
  }

  return Status;
}

/**
  The implementation of IHISI function 49h type 4. Clearing all of secure boot settings in variable store and then using
  then using variable data contained in factory copy region to update variable. All of other variables (not secure boot
  relative variables or variables are located in factory copy region) will not be cleared.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval Others                  Other error cause restoring factory copy failed.
**/
STATIC
EFI_STATUS
EFIAPI
RestoreFactoryCopyWithReservingOtherSettings (
  VOID
  )
{
  EFI_STATUS     Status;

  Status = RestoreFactory (FACTORY_COPY_RESTORE_WITH_RESERVING_OTHER_SETTINGS);

  if (!EFI_ERROR (Status)) {
    IhisiLibErrorCodeHandler (IhisiSuccess);
  } else {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
  }

  return Status;
}
