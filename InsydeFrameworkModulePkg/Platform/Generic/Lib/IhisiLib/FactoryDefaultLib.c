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
//;  FactoryDefualtLib.c
//;

#include "IhisiLib.h"
#include "FlashLayout.h"
#include "EfiVariable.h"
#include "VariableSupportLib.h"

#include EFI_PROTOCOL_DEFINITION (SmmFwBlockService)
#include EFI_GUID_DEFINITION (ImageAuthentication)

//
// The reason why the switch added here is for compatible to Rev 5.0.
// There is no SECURE_BOOT_SUPPORT switch in Rev 5.0.
// Please don't use this switch in any INF files.
//
#ifdef SECURE_BOOT_SUPPORT

//
// signature define
//
#define COMMON_REGION_INPUT_SIGNATURE   EFI_SIGNATURE_32 ('$', 'R', 'D', 'I')
#define COMMON_REGION_OUTPUT_SIGNATURE  EFI_SIGNATURE_32 ('$', 'C', 'D', 'O')
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

EFI_STATUS
WriteDefaultSettingsToFactoryCopy (
  VOID
  );

EFI_STATUS
ReadDefaultSettingsToFactoryCopy (
  VOID
  );

EFI_STATUS
RestoreFactoryCopyWithClearingAllSettings (
  VOID
  );

EFI_STATUS
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


EFI_STATUS
CollectSecureBootSettings (
  IN OUT UINTN            *BufferSize,
  OUT    UINT8            *Buffer
  )
/*++

Routine Description:

  Get all of secure boot settings from factory copy region.

Arguments:

  BufferSize     - On input, Total buffer size.
                   On oupput, The size of all secure boot settings
  Buffer         - Buffer to save secure boot settings

Returns:
  TRUE           - Variable header is valid.
  FALSE          - Variable header is not valid.

--*/
{
  UINTN               TotalSize;
  UINTN               LastVariableIndex;
  UINT8               *ReadBuffer;
  UINTN               HeaderSize;
  SPECIFIC_VARIABLE   GetVariable [] = {IsPkVariable, IsKekVariable, IsDbVariable, IsDbxVariable};
  BOOLEAN             VariableFound;
  VARIABLE_HEADER     *VariableHeader;
  VARIABLE_HEADER     *TempVariable;
  VARIABLE_HEADER     *NexVariable;
  UINTN               Index;
  UINTN               FunctionCnt;
  UINTN               CopySize;
  EFI_STATUS          Status;

  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + sizeof (VARIABLE_STORE_HEADER);
  if (BufferSize == NULL || Buffer == NULL || *BufferSize < HeaderSize) {
    return EFI_INVALID_PARAMETER;
  }
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE - HeaderSize,
                    &ReadBuffer
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
    TempVariable = NULL;
    while (IsValidVariableHeader (VariableHeader) && (UINTN) VariableHeader - (UINTN) Buffer < TotalSize) {
      NexVariable = (VARIABLE_HEADER *) ((UINTN) VariableHeader + sizeof (VARIABLE_HEADER) + VariableHeader->NameSize + VariableHeader->DataSize);
      if (VariableHeader->State == VAR_ADDED && GetVariable[Index] ((CHAR16 *) (VariableHeader + 1), &VariableHeader->VendorGuid)) {
        CopySize = (UINTN) NexVariable - (UINTN) VariableHeader;
        if (CopySize + LastVariableIndex + HeaderSize > FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE) {
          mSmst->SmmFreePool (ReadBuffer);
          ASSERT (FALSE);
          return EFI_OUT_OF_RESOURCES;
        }
        EfiCopyMem (&ReadBuffer[LastVariableIndex], VariableHeader, CopySize);
        LastVariableIndex += CopySize;
        VariableFound = TRUE;
        break;
      } else if (VariableHeader->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION) && GetVariable[Index] ((CHAR16 *) (VariableHeader + 1), &VariableHeader->VendorGuid)) {
        TempVariable = VariableHeader;
      }
      VariableHeader = NexVariable;
    }
    if (!VariableFound && TempVariable != NULL) {
      CopySize = (UINTN) TempVariable + sizeof (VARIABLE_HEADER) + TempVariable->NameSize + TempVariable->DataSize;
      if (CopySize + LastVariableIndex + HeaderSize > FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE) {
        mSmst->SmmFreePool (ReadBuffer);
        ASSERT (FALSE);
        return EFI_OUT_OF_RESOURCES;
      }
      EfiCopyMem (&ReadBuffer[LastVariableIndex], TempVariable, CopySize);
      LastVariableIndex += CopySize;
      VariableFound = TRUE;
    }
    //
    // if factory copy doesn't have dbx variable, it still a legal factory default copy.
    //
    if (!VariableFound && Index != FunctionCnt - 1)  {
      mSmst->SmmFreePool (ReadBuffer);
      return EFI_NOT_FOUND;
    }
  }
  //
  // copy collected secure boot settings to input buffer
  //
  EfiSetMem (Buffer + HeaderSize, TotalSize - HeaderSize, 0xFF);
  EfiCopyMem (Buffer + HeaderSize, ReadBuffer, LastVariableIndex);
  mSmst->SmmFreePool (ReadBuffer);
  //
  // Setting output buffer size
  //
  *BufferSize = LastVariableIndex + HeaderSize;
  return EFI_SUCCESS;
}

EFI_STATUS
ReadDefaultSettingsToFactoryCopy (
  VOID
  )
/*++

Routine Description:

  The implementation of IHISI function 49h type 2. return default factory default settings
  information to application.

Arguments:

  None

Returns:

  EFI_SUCCESS               - The function completed successfully

--*/
{
  OUTPUT_DATA_STRUCTURE          *OutputData;

  OutputData = (OUTPUT_DATA_STRUCTURE *) (UINTN) SmmCpuSaveLocalState->ECX;
  OutputData->BlockSize = COMMON_REGION_BLOCK_SIZE_4K;
  if (BufferOverlapSmram (OutputData, sizeof (OUTPUT_DATA_STRUCTURE))) {
    IhisiLibErrorCodeHandler (IhisiAccessProhibited);
    return EFI_INVALID_PARAMETER;
  }
  if (FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE == 0x10000) {
    OutputData->DataSize = COMMON_REGION_SIZE_64K;
  } else {
    OutputData->DataSize = COMMON_REGION_SIZE_SPECIFIC;
    OutputData->PysicalDataSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE;
  }
  mCommonRegionReadControl.WantToReadData = TRUE;
  mCommonRegionReadControl.InCommonReadProcess = FALSE;
  mCommonRegionReadControl.BlockSize = 0x1000;
  mCommonRegionReadControl.DataSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE;
  mCommonRegionReadControl.RemainDataSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE;
  IhisiLibErrorCodeHandler (IhisiSuccess);
  return EFI_SUCCESS;
}

EFI_STATUS
WriteDefaultSettingsToFactoryCopy (
  VOID
  )
/*++

Routine Description:

  The implementation of IHISI function 49h type 1. Write secure boot settings from
  variable store to factory copy region.

Arguments:

  None

Returns:

  EFI_SUCCESS               - The function completed successfully.
  EFI_UNSUPPORTED           - mSmmRT is NULL.
  EFI_OUT_OF_RESOURCES      - Allocate pool failed.
  Other                     - Other error occurred in this function.

--*/
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

  if (mSmmRT == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = mSmmRT->LocateProtocol (
                     &gEfiSmmFwBlockServiceProtocolGuid,
                     NULL,
                     &SmmFwBlockService
                     );
  if (EFI_ERROR(Status)) {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_SIZE,
                    &Buffer
                    );
  if (EFI_ERROR(Status)) {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  ReadBufferSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_SIZE;
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_BASE,
                                0,
                                &ReadBufferSize,
                                Buffer
                                );
  if (EFI_ERROR(Status)) {
    mSmst->SmmFreePool (Buffer);
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  WriteBufferSize = ReadBufferSize;
  Status = CollectSecureBootSettings (&WriteBufferSize, Buffer);
  if (EFI_ERROR(Status) || ReadBufferSize < WriteBufferSize + FACTORY_COPY_HEADER_SIZE) {
    mSmst->SmmFreePool (Buffer);
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  //
  // Combine factory copy header and foctory copy content
  //
  EfiCopyMem (Buffer + FACTORY_COPY_HEADER_SIZE, Buffer, WriteBufferSize);
  ReadBufferSize = FACTORY_COPY_HEADER_SIZE;
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_BASE,
                                0,
                                &ReadBufferSize,
                                Buffer
                                );
  WriteBufferSize += FACTORY_COPY_HEADER_SIZE;
  if (EFI_ERROR(Status)) {
    mSmst->SmmFreePool (Buffer);
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return Status;
  }

  WriteCount = 0;
  WriteSuccess = FALSE;
  do {
    EraseSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE;
    Status = SmmFwBlockService->EraseBlocks (
                                  SmmFwBlockService,
                                  FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_BASE,
                                  (UINTN *) &EraseSize
                                  );
    if (!EFI_ERROR (Status)) {
      WriteSize = WriteBufferSize;
      Status = SmmFwBlockService->Write (
                                    SmmFwBlockService,
                                    FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_BASE,
                                    (UINTN *) &WriteSize,
                                    Buffer
                                    );
      if (!EFI_ERROR (Status)) {
        WriteSuccess = TRUE;
      }
    }
    WriteCount++;
  } while (WriteCount < 100 && !WriteSuccess);

  mSmst->SmmFreePool (Buffer);
  WriteSuccess ? IhisiLibErrorCodeHandler (IhisiSuccess) : IhisiLibErrorCodeHandler (IhisiFunNotSupported);
  return WriteSuccess ? EFI_SUCCESS : Status;
}


EFI_STATUS
InitializeOutputData (
  OUT OUTPUT_DATA_STRUCTURE       *OutputData
  )
/*++

Routine Description:

  Initialize out put data for IHISI function 49h

Arguments:

  OutputData                - Pointer to OUTPUT_DATA_STRUCTURE structure.

Returns:

  EFI_SUCCESS               - Initailize output data successful.
  EFI_INVALID_PARAMETER     - OutputData is NULL.

--*/
{
  if (OutputData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OutputData->Signature = COMMON_REGION_OUTPUT_SIGNATURE;
  OutputData->StructureSize = sizeof (OUTPUT_DATA_STRUCTURE);
  OutputData->Status = 0;

  return EFI_SUCCESS;
}


EFI_STATUS
CommonRegionDataCommunication (
  VOID
  )
/*++

Routine Description:

  The entry point of IHISI function 49h.

Arguments:

  None

Returns:

  EFI_SUCCESS               - The function completed successfully
  EFI_UNSUPPORTED           - The type of function 49H is unsupported.
  Other                     - Other error cause in this function.

--*/
{
  UINTN                          Index;
  UINT8                          DataType;
  EFI_STATUS                     Status;
  OUTPUT_DATA_STRUCTURE          *OutputData;
  INPUT_DATA_STRUCTURE           *InputData;


  InputData  = (INPUT_DATA_STRUCTURE *) (UINTN) SmmCpuSaveLocalState->ECX;
  OutputData = (OUTPUT_DATA_STRUCTURE *) (UINTN) SmmCpuSaveLocalState->ECX;
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

EFI_STATUS
CommonRegionDataRead (
  VOID
  )
/*++

Routine Description:

  The entry point of IHISI function 4Bh.

Arguments:

  None

Returns:

  EFI_SUCCESS               - The function completed successfully
  EFI_BUFFER_TOO_SMALL      - The size of input buffer is too small to save read data.
  Other                     - Other error cause this function can work properly.
--*/
{
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *SmmFwBlockService;
  UINT8                                   *OutputBuffer;
  UINT32                                  OutputBufferSize;
  UINTN                                   ReadBufferSize;
  EFI_STATUS                              Status;

  Status = EFI_UNSUPPORTED;
  if (mCommonRegionReadControl.WantToReadData) {
    OutputBuffer = (UINT8 *) (UINTN) SmmCpuSaveLocalState->ESI;
    EfiCopyMem (&OutputBufferSize, (VOID *) (UINTN) SmmCpuSaveLocalState->EDI, sizeof (UINT32));
    if (mSmmRT == NULL || OutputBuffer == NULL || OutputBufferSize != mCommonRegionReadControl.BlockSize) {
      IhisiLibErrorCodeHandler (IhisiFunNotSupported);
      return Status;
    }
    if (BufferOverlapSmram ((VOID *) OutputBuffer, OutputBufferSize)) {
      IhisiLibErrorCodeHandler (IhisiAccessProhibited);
      return EFI_INVALID_PARAMETER;
    }
    mCommonRegionReadControl.InCommonReadProcess = TRUE;
    Status = mSmmRT->LocateProtocol (
                       &gEfiSmmFwBlockServiceProtocolGuid,
                       NULL,
                       &SmmFwBlockService
                       );
    if (EFI_ERROR(Status)) {
      IhisiLibErrorCodeHandler (IhisiFunNotSupported);
      return Status;
    }
    ReadBufferSize = (UINTN) OutputBufferSize;
    Status = SmmFwBlockService->Read (
                                  SmmFwBlockService,
                                  FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_BASE,
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
      EfiCopyMem ((VOID *) (UINTN) SmmCpuSaveLocalState->EDI, &OutputBufferSize, sizeof (UINT32));
      IhisiLibErrorCodeHandler (IhisiSuccess);
    } else {
      IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    }
  } else {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
  }

  return Status;
}

EFI_STATUS
ExportMergedFactoryDefaultSetting (
  IN OUT UINTN                                *DataSize,
  OUT    UINT8                                *Data
  )
/*++

Routine Description:

  This funciotn uses to export merged factory default setting. The merged factory copy
  default setting means keep all of original variable data and then merge variable data
  in factory copy region with original data. If finding the same variable, system will
  use the variable in factory default to replace oringnal variable.

Arguments:

  VariableDefaultDevice     - Point to private data.
  VariableStoreLength       - On input, the size in bytes of the return VariableBuffer.
                              On output the size of data returned in VariableBuffer.
  VariableBuffer            - The buffer to return the contents of the variable.

Returns:

  EFI_SUCCESS               - The function completed successfully
  EFI_INVALID_PARAMETER     - Some input parameter is invalid.
  EFI_BUFFER_TOO_SMALL      - The VariableStoreLength is too small for the result.  VariableStoreLength has
                              been updated with the size needed to complete the request.
  Other                     - Other error cause merge factory default setting failed.
--*/
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

  Status = mSmmRT->LocateProtocol (
                     &gEfiSmmFwBlockServiceProtocolGuid,
                     NULL,
                     &SmmFwBlockService
                     );
  if (EFI_ERROR(Status)) {
    goto Done;
  }
  ASSERT (FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_SIZE >= FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE);
  //
  // Read whole factory copy sub-region data
  //
  WorkingBufferLength = FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_SIZE;
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    WorkingBufferLength,
                    &WorkingBuffer
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  EfiSetMem (WorkingBuffer, WorkingBufferLength, 0xFF);

  FactoryCopyRegionSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE - FACTORY_COPY_HEADER_SIZE;
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_BASE,
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
  VariableBufferLength = FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_SIZE;
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    VariableBufferLength,
                    &VariableBuffer
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  EfiSetMem (VariableBuffer, VariableBufferLength, 0xFF);
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_BASE,
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
      EfiCopyMem (Data, WorkingBuffer, WorkingBufferLength);
    } else {
      Status = EFI_BUFFER_TOO_SMALL;
    }
    *DataSize = WorkingBufferLength;
  }

  if (WorkingBuffer != NULL) {
    mSmst->SmmFreePool (WorkingBuffer);
  }

  if (VariableBuffer != NULL) {
    mSmst->SmmFreePool (VariableBuffer);
  }

  return Status;

}

BOOLEAN
IsFacotryCopyValid (
  VOID
  )
/*++

Routine Description:

  Checks the contian of factory copy region is valid or not

Arguments:

  None

Returns:

  TRUE      - The contain in factory copy region is valid.
  FALSE     - The contain in factory copy region is invalid.

--*/
{
  EFI_STATUS                             Status;
  UINTN                                  ReadBufferSize;
  UINTN                                  *ReadBuffer;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL      *SmmFwBlockService;
  UINTN                                  Index;
  UINTN                                  NumberOfInteger;
  BOOLEAN                                IsValid;

  IsValid = FALSE;
  Status = mSmmRT->LocateProtocol (
                     &gEfiSmmFwBlockServiceProtocolGuid,
                     NULL,
                     &SmmFwBlockService
                     );
  if (EFI_ERROR(Status)) {
    return IsValid;
  }

  ReadBuffer     = NULL;
  ReadBufferSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE - FACTORY_COPY_HEADER_SIZE;
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    ReadBufferSize,
                    &ReadBuffer
                    );
  if (EFI_ERROR (Status)) {
    return IsValid;
  }

  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_BASE,
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

  mSmst->SmmFreePool (ReadBuffer);
  return IsValid;
}

EFI_STATUS
ExportFactoryDefaultSetting (
  IN OUT UINTN                                *DataSize,
  OUT    UINT8                                *Data
  )
/*++

Routine Description:

  This funciotn uses to export factory default setting to input buffer.

Arguments:

  DataSize                  - On input, the size in bytes of the return Data buffer.
                              On output the size of data returned in Data buffer.
  Data                      - The buffer to return the contents of the variable.

Returns:

  EFI_SUCCESS               - The function completed successfully
  EFI_INVALID_PARAMETER     - Some input parameter is invalid.
  EFI_BUFFER_TOO_SMALL      - The VariableStoreLength is too small for the result. DataSize has
                              been updated with the size needed to complete the request.
  Other                     - Other error cause merge factory default setting failed.

--*/
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

  RequiredDataSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE - FACTORY_COPY_HEADER_SIZE;
  if (*DataSize < RequiredDataSize) {
    *DataSize = RequiredDataSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  Status = mSmmRT->LocateProtocol (
                     &gEfiSmmFwBlockServiceProtocolGuid,
                     NULL,
                     &SmmFwBlockService
                     );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  HeaderBuffer = NULL;
  FactoryDefaultBuffer = NULL;
  //
  // Read original firmware volume header and variable store header
  //
  HeaderSize = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + sizeof (VARIABLE_STORE_HEADER);
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    HeaderSize,
                    &HeaderBuffer
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_BASE,
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
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    ReadDataSize,
                    &FactoryDefaultBuffer
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_BASE,
                                FACTORY_COPY_HEADER_SIZE + HeaderSize,
                                &ReadDataSize,
                                FactoryDefaultBuffer
                                );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  EfiCopyMem (Data, HeaderBuffer, HeaderSize);
  EfiCopyMem (Data + HeaderSize, FactoryDefaultBuffer, RequiredDataSize - HeaderSize);
  *DataSize = RequiredDataSize;

Done:

  if (FactoryDefaultBuffer != NULL) {
    mSmst->SmmFreePool (FactoryDefaultBuffer);
  }

  if (HeaderBuffer != NULL) {
    mSmst->SmmFreePool (HeaderBuffer);
  }

  return Status;
}


EFI_STATUS
CollectFactoryDefaultSetting (
  IN     UINTN                      RestoreType,
  IN OUT UINTN                      *BufferSize,
  OUT    UINT8                      *FactoryDefaultBuffer
  )
/*++

Routine Description:

  According to restore type to collect factory default settings and restore these settings
  in output buffer

Arguments:

  RestoreType               - Factory copy restore type.
  BufferSize                - On input, the size in bytes of the return BufferSize.
                              On output the size of data returned in BufferSize.
  FactoryDefaultBuffer      - The buffer to return the contents of the factory default settings.

Returns:

  EFI_SUCCESS               - The function completed successfully
  EFI_INVALID_PARAMETER     - Any input parameter is invalid.
  EFI_BUFFER_TOO_SMALL      - The BufferSize is too small for the result.  BufferSize has
                              been updated with the size needed to complete the request.
  EFI_NOT_READY             - The contain of factory copy region is invalid.
  Other                     - Other error cause collect factory default setting failed.

--*/
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

EFI_STATUS
RestoreFactory (
  IN  UINTN     RestoreType
  )
/*++

Routine Description:

  According to restore type to restore factory default.

Arguments:

  RestoreType             - Input restore type. this funtion only supports FACTORY_COPY_RESTORE_WITH_CLEARING_ALL_SETTINGS
                            and FACTORY_COPY_RESTORE_WITH_RESERVING_OTHER_SETTINGS. Other restore type will return
                            EFI_INVALId_PARAMETER.

Returns:

  EFI_SUCCESS             - The function completed successfully
  EFI_INVALID_PARAMETER   - Any input parameter is invalid.
  Other                   - Other error cause collect factory default setting failed.

--*/
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

  Status = mSmmRT->LocateProtocol (
                     &gEfiSmmFwBlockServiceProtocolGuid,
                     NULL,
                     &SmmFwBlockService
                     );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ASSERT (FLASH_REGION_NVSTORAGE_SUBREGION_NV_FACTORY_COPY_SIZE <= FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_SIZE);
  BufferSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_SIZE;
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    BufferSize,
                    &Buffer
                    );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  EfiSetMem (Buffer, BufferSize, 0xFF);

  Status = CollectFactoryDefaultSetting (RestoreType, &BufferSize, Buffer);
  if (EFI_ERROR (Status)) {
    mSmst->SmmFreePool (Buffer);
    return Status;
  }

  WriteSuccess = FALSE;
  WriteCount   = 0;
  do {
    EraseSize = FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_SIZE;
    Status = SmmFwBlockService->EraseBlocks (
                                  SmmFwBlockService,
                                  FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_BASE,
                                  (UINTN *) &EraseSize
                                  );
    if (!EFI_ERROR (Status)) {
      WriteSize = BufferSize;
      Status = SmmFwBlockService->Write (
                                    SmmFwBlockService,
                                    FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_BASE,
                                    (UINTN *) &WriteSize,
                                    Buffer
                                    );
      if (!EFI_ERROR (Status)) {
        WriteSuccess = TRUE;
      }
    }
    WriteCount++;
  } while (WriteCount < 100 && !WriteSuccess);

  mSmst->SmmFreePool (Buffer);

  return WriteSuccess ? EFI_SUCCESS : Status;
}

EFI_STATUS
RestoreFactoryCopyWithClearingAllSettings (
  VOID
  )
/*++

Routine Description:

  The implementation of IHISI function 49h type 3. Restore variable data from factory copy region and
  clear all of variable data in variable store.

Arguments:

  None

Returns:

  EFI_SUCCESS             - The function completed successfully
  Other                   - Other error cause restoring factory copy failed.

--*/
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

EFI_STATUS
RestoreFactoryCopyWithReservingOtherSettings (
  VOID
  )
/*++

Routine Description:

  The implementation of IHISI function 49h type 4. Clearing all of secure boot settings in variable store and then using
  then using variable data contained in factory copy region to update variable. All of other variables (not secure boot
  relative variables or variables are located in factory copy region) will not be cleared.

Arguments:

  None

Returns:

  EFI_SUCCESS             - The function completed successfully
  Other                   - Other error cause restoring factory copy failed.

--*/
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

#endif
