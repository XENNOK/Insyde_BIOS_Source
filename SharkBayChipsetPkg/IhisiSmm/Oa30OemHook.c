/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include "Oa30OemHook.h"
//[-start-130808-IB06720232-add]//
#include <Library/BaseOemSvcKernelLib.h>
//[-end-130808-IB06720232-add]//

extern EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *mSmmFwBlockService;

BOOLEAN
MsdmExist (
  )
{
  EFI_STATUS                       Status;
  UINTN                            RomBaseAddress;
  EFI_ACPI_MSDM_DATA_STRUCTURE     *MsdmData;
  UINTN                            DataSize;
  BOOLEAN                          MsdmExist;

  MsdmData = NULL;
  MsdmExist = FALSE;

  DataSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);
  RomBaseAddress = (UINTN) FixedPcdGet32 (PcdFlashNvStorageMsdmDataBase);

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    DataSize,
                    (VOID **)&MsdmData
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  Status = mSmmFwBlockService->Read (
                                 mSmmFwBlockService,
                                 RomBaseAddress,
                                 0,
                                 &DataSize,
                                 (UINT8*) MsdmData
                                 );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  if ((MsdmData->MsdmVersion == 0xFFFFFFFF) &&
      (MsdmData->MdsmDataType == 0xFFFFFFFF) &&
      (MsdmData->MsdmDataLength == 0xFFFFFFFF)) {
    goto Done;
  }

  MsdmExist = TRUE;

Done:
  if (MsdmData != NULL) {
    gSmst->SmmFreePool (MsdmData);
  }
  return MsdmExist;
}

/**
  AH=41h, OEM Extra Data Communication type 50h to read/write OA3.0.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS  Read or write OA3.0 successful.
**/
EFI_STATUS
Oa30ReadWriteFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
//[-start-130125-IB10820224-add]//
  EFI_STATUS        Status;
//[-end-130125-IB10820224-modify]//
  UINT32                                   MsdmDataSize;

  MsdmDataSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);
  switch (ApCommDataBuffer->DataSize) {

  case NotContained:
    BiosCommDataBuffer->Signature        = BIOS_COMMUNICATION_SIGNATURE;
    BiosCommDataBuffer->Size             = sizeof (BIOS_COMMUNICATION_DATA_TABLE);
    BiosCommDataBuffer->BlockSize        = BlockSize4k;
    BiosCommDataBuffer->DataSize         = NotContained;
    BiosCommDataBuffer->PhysicalDataSize = MsdmDataSize;
    break;

  case SkipSizeCheck:
    BiosCommDataBuffer->Signature        = BIOS_COMMUNICATION_SIGNATURE;
    BiosCommDataBuffer->Size             = sizeof (BIOS_COMMUNICATION_DATA_TABLE);
    BiosCommDataBuffer->BlockSize        = BlockSize4k;
    BiosCommDataBuffer->DataSize         = SkipSizeCheck;          //Don't care
    BiosCommDataBuffer->PhysicalDataSize = 0x00;                   //Don't care
    break;

  case AllowPhysicalSizeToCaller:
    BiosCommDataBuffer->Signature        = BIOS_COMMUNICATION_SIGNATURE;
    BiosCommDataBuffer->Size             = sizeof (BIOS_COMMUNICATION_DATA_TABLE);
    BiosCommDataBuffer->BlockSize        = BlockSize4k;
    BiosCommDataBuffer->DataSize         = AllowPhysicalSizeToCaller;
    BiosCommDataBuffer->PhysicalDataSize = MsdmDataSize;   //bin size
    break;

  default:
    break;
  }

  if (!MsdmExist ()) {
    BiosCommDataBuffer->ErrorReturn    = (BiosCommDataBuffer->ErrorReturn | BIT3);
  }

//[-start-130125-IB10820224-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS41T50HookOa30ReadWriteFunction (ApCommDataBuffer, BiosCommDataBuffer);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS41T50HookOa30ReadWriteFunction, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
//[-end-130125-IB10820224-modify]//

  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type 51h to erase OA3.0 (reset to default).

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS  Erase OA3.0 successful.
**/
EFI_STATUS
Oa30EraseFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  EFI_STATUS                   Status;
  UINT8                        LoopCount;
  UINTN                        RomBaseAddress;
  UINTN                        EraseSize = 0x1000;

  RomBaseAddress = (UINTN) FixedPcdGet32 (PcdFlashNvStorageMsdmDataBase);
  LoopCount = 0;
  Status = EFI_SUCCESS;

  if (!MsdmExist ()) {
    BiosCommDataBuffer->ErrorReturn    = (BiosCommDataBuffer->ErrorReturn | BIT3);
    goto Done;
  }

  if (PcdGetBool (PcdEcSharedFlashSupported)) {
//[-start-130808-IB06720232-modify]//
    OemSvcEcIdle (TRUE);
//[-end-130808-IB06720232-modify]//
  }
  Status = EFI_UNSUPPORTED;
  while ((EFI_ERROR (Status)) && (LoopCount < 100)) {
    Status = mSmmFwBlockService->EraseBlocks (
                                   mSmmFwBlockService,
                                   RomBaseAddress,
                                   &EraseSize
                                   );
    LoopCount++;
  }

  if (PcdGetBool (PcdEcSharedFlashSupported)) {
//[-start-130808-IB06720232-modify]//
    OemSvcEcIdle (FALSE);
//[-end-130808-IB06720232-modify]//
  }

Done:
  BiosCommDataBuffer->DataSize = ApCommDataBuffer->DataSize;
  BiosCommDataBuffer->PhysicalDataSize = ApCommDataBuffer->PhysicalDataSize;

  return Status;
}

/**
  AH=41h, OEM Extra Data Communication type 52h to populate header.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   populate header successful.
**/
EFI_STATUS
Oa30PopulateHeaderFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{

  return EFI_SUCCESS;
}

/**
  AH=41h, OEM Extra Data Communication type 53h to de-populate header.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   populate header successful.
**/
EFI_STATUS
Oa30DePopulateHeaderFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{

  return EFI_SUCCESS;
}

/**
  Function to write OA3.0 data and do action which request from IHISI function 42h.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
Oa30DataWrite (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINT8                            *WriteDataBuffer;
  UINTN                            WriteSize;
  UINTN                            RomBaseAddress;
  UINT8                            LoopCount;
  UINT8                            ShutdownMode;
  UINT32                           MsdmDataSize;

  WriteDataBuffer = (UINT8 *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  WriteSize = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  RomBaseAddress = (UINTN) FixedPcdGet32 (PcdFlashNvStorageMsdmDataBase);
  MsdmDataSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);

  if (WriteSize == MsdmDataSize) {
    UINT8                          *ReturnDataBuffer = NULL;
    UINTN                          Index2 = 0;
    UINTN                          EraseSize = 0x1000;
    UINT8                          *TEMP;

    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      0x1000,
                      (VOID **)&ReturnDataBuffer
                      );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = mSmmFwBlockService->Read (
                                  mSmmFwBlockService,
                                  RomBaseAddress,
                                  0,
                                  &EraseSize,
                                  ReturnDataBuffer
                                  );
    //
    // Only modify the first 0x31 bytes
    //
    TEMP = ReturnDataBuffer;
    for (Index2 = 0; Index2 < MsdmDataSize; Index2++) {
      TEMP[Index2] = WriteDataBuffer[Index2];
    }

    LoopCount = 0;
    Status = EFI_UNSUPPORTED;
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;
    if (PcdGetBool (PcdEcSharedFlashSupported)) {
//[-start-130808-IB06720232-modify]//
      OemSvcEcIdle (TRUE);
//[-end-130808-IB06720232-modify]//
    }

    while ((EFI_ERROR (Status)) && (LoopCount < 100)) {
      Status = mSmmFwBlockService->EraseBlocks (
                                     mSmmFwBlockService,
                                     RomBaseAddress,
                                     &EraseSize
                                     );
      Status = mSmmFwBlockService->Write (
                                     mSmmFwBlockService,
                                     RomBaseAddress,
                                     &EraseSize,
                                     ReturnDataBuffer
                                     );
      LoopCount++;
    }
    gSmst->SmmFreePool (ReturnDataBuffer);
    if (PcdGetBool (PcdEcSharedFlashSupported)) {
//[-start-130808-IB06720232-modify]//
      OemSvcEcIdle (FALSE);
//[-end-130808-IB06720232-modify]//
    }
  } else {
    return EFI_INVALID_PARAMETER;
  }

  ShutdownMode = (UINT8) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  switch (ShutdownMode) {

  case DoNothing:
    Status = EFI_SUCCESS;
    break;

  case WindowsReboot:
    Status = EFI_SUCCESS;
    break;

  case WindowsShutdown:
    Status = EFI_SUCCESS;
    break;

  case DosReboot:
    //
    // Note: Reboot by Oem hook
    //
    Status = FbtsReboot (TRUE);
    break;

  case DosShutdown:
    Status = FbtsShutDown ();
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

/**
  Function to read OA3.0 data.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
Oa30DataRead (
  VOID
  )
{
  EFI_STATUS                       Status;
  UINT8                            *ReadDataBuffer;
  UINTN                            *ReadSize;
  UINTN                            RomBaseAddress;
  UINT8                            *ReturnDataBuffer = NULL;
  UINTN                            DataSize = 0x1000;

  ReadDataBuffer = (UINT8 *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  ReadSize = (UINTN *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  RomBaseAddress = (UINTN) FixedPcdGet32 (PcdFlashNvStorageMsdmDataBase);

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    DataSize,
                    (VOID **)&ReturnDataBuffer
                    );

  Status = mSmmFwBlockService->Read (
                                 mSmmFwBlockService,
                                 RomBaseAddress,
                                 0,
                                 &DataSize,
                                 ReturnDataBuffer
                                 );

  *ReadSize = sizeof (EFI_ACPI_MSDM_DATA_STRUCTURE);
  CopyMem (ReadDataBuffer, ReturnDataBuffer, *ReadSize);

  return Status;
}
