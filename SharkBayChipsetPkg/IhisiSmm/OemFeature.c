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

#include "OemFeature.h"

//[-start-120913-IB05300329-add]//
///
/// Global variables
///
EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL    *mSmmFwBlockService;
EFI_GLOBAL_NVS_AREA                  *mGlobalNvsArea;
UINT8                                mOemExtraDataType;
//[-end-120913-IB05300329-add]//

//[-start-120913-IB05300329-add]//
EFI_STATUS
OemCommonFeatureInit (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_GLOBAL_NVS_AREA_PROTOCOL       *GlobalNvsAreaProtocol;
  FLASH_DEVICE                       *Buffer;

  GlobalNvsAreaProtocol = NULL;
  mGlobalNvsArea        = NULL;

  Status = gSmst->SmmLocateProtocol (
                     &gEfiSmmFwBlockServiceProtocolGuid,
                     NULL,
                     (VOID **)&mSmmFwBlockService
                     );

  if (!EFI_ERROR (Status)) {
   Status = gSmst->SmmAllocatePool (
                     EfiRuntimeServicesData,
                     sizeof (FLASH_DEVICE),
                     (VOID **)&Buffer
                     );
   if (!EFI_ERROR ( Status ) ) {
     Status = mSmmFwBlockService->DetectDevice (
                                    mSmmFwBlockService,
                                    (UINT8 *) Buffer
                                    );
   }
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **)&GlobalNvsAreaProtocol
                  );
  if (!EFI_ERROR (Status)) {
    mGlobalNvsArea = GlobalNvsAreaProtocol->Area;
  } else {
    return Status;
  }
  return Status;
}
//[-end-120913-IB05300329-add]//

/**
  AH=41h, OEM Extra Data Communication

  01h = VBIOS
  02h~03h = Reserved
  04h~0Ch = User Define
  0Dh~4Fh = Reserved
  50h = OA 3.0 Read/Write
  51h = OA 3.0 Erase (Reset to default)
  52h = OA 3.0 Populate Header
  53h = OA 3.0 De-Populate Header
  54h = Logo Update (Write)
  55h = Check BIOS sign by System BIOS
  56~FFh = Reserved

  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
OemExtraDataCommunication (
  VOID
  )
{
  EFI_STATUS                       Status;
  AP_COMMUNICATION_DATA_TABLE      *ApCommDataBuffer;
  BIOS_COMMUNICATION_DATA_TABLE    BiosCommDataBuffer;

//[-start-120917-IB05300333-modify]//
  //
  // Initialize the output communication data buffer.
  //
  ZeroMem (&BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
  BiosCommDataBuffer.Signature        = BIOS_COMMUNICATION_SIGNATURE;
  BiosCommDataBuffer.Size             = sizeof (BIOS_COMMUNICATION_DATA_TABLE);
  BiosCommDataBuffer.BlockSize        = BlockSize4k;
  BiosCommDataBuffer.DataSize         = DataSize64k;
  BiosCommDataBuffer.PhysicalDataSize = 0;
  BiosCommDataBuffer.ErrorReturn      = 0;
//[-end-120917-IB05300333-modify]//

  ApCommDataBuffer = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (ApCommDataBuffer->Signature != AP_COMMUNICATION_SIGNATURE) {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return EFI_NOT_FOUND;
  }
//[-start-120913-IB05300329-add]//
  mOemExtraDataType = ApCommDataBuffer->DataType;
//[-end-120913-IB05300329-add]//
  switch (ApCommDataBuffer->DataType) {

  case Vbios:
//[-start-130125-IB10820224-modify]//
    Status = OemSvcIhisiS41T1VbiosFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS41T1VbiosFunction, Status : %r\n", Status));
    Status = EFI_SUCCESS;
//[-end-130125-IB10820224-modify]//
    break;

  case Oa30ReadWrite:
    Status = Oa30ReadWriteFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    break;

  case Oa30Erase:
    Status = Oa30EraseFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    break;

  case Oa30PopulateHeader:
    Status = Oa30PopulateHeaderFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    break;

  case Oa30DePopulateHeader:
    Status = Oa30DePopulateHeaderFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    break;

  case LogoUpdate:
//[-start-130125-IB10820224-modify]//
//[-start-130524-IB05160451-modify]//
    Status = OemSvcIhisiS41T54LogoUpdateFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS41T54LogoUpdateFunction, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
    Status = EFI_SUCCESS;
//[-end-130125-IB10820224-modify]//
    break;

  case CheckBiosSignBySystemBios:
//[-start-130125-IB10820224-modify]//
//[-start-130524-IB05160451-modify]//
    Status = OemSvcIhisiS41T55CheckBiosSignBySystemBiosFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS41T55CheckBiosSignBySystemBiosFunction, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
    Status = EFI_SUCCESS;
//[-end-130125-IB10820224-modify]//
    break;
//[-start-120913-IB05300329-add]//
  case PfatUpdate:
    Status = PfatUpdateHandleFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    break;
//[-end-120913-IB05300329-add]//
//[-start-120917-IB05300333-add]//
  case PfatReturn:
    Status = PfatReturnHandleFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    break;
//[-end-120917-IB05300333-add]//
  default:
//[-start-130125-IB10820224-modify]//
    Status = OemSvcIhisiS41ReservedFunction (ApCommDataBuffer, &BiosCommDataBuffer);
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS41ReservedFunction, Status : %r\n", Status));
    Status = EFI_UNSUPPORTED;           //Return unsupported to indicate this is reserved function
//[-end-130125-IB10820224-modify]//
    break;

  }


  if (EFI_ERROR (Status)) {
    IhisiLibErrorCodeHandler (IhisiFunNotSupported);
    return EFI_UNSUPPORTED;
  } else {
    CopyMem (ApCommDataBuffer, &BiosCommDataBuffer, sizeof (BIOS_COMMUNICATION_DATA_TABLE));
    IhisiLibErrorCodeHandler (IhisiSuccess);
    return EFI_SUCCESS;
  }

}

//[-start-120913-IB05300329-modify]//
/**
  AH=42h, OEM Extra Data Write.

  Function 41h and 42h (or 47h) are pairs. The function 41h has to be called before calling
  into function 42h.

  @retval EFI_SUCCESS            OEM Extra Data Write successful.
  @return Other                  OEM Extra Data Write failed.
**/
EFI_STATUS
OemExtraDataWrite (
  VOID
  )
{
  EFI_STATUS                       Status;
  Status = EFI_SUCCESS;

  switch (mOemExtraDataType) {
  case Oa30ReadWrite:
    Status = Oa30DataWrite ();
    break;
  case PfatUpdate:
    mOemExtraDataType = 0x00;
    Status = PfatUpdateWrite ();
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (Status));
  return Status;
}
//[-end-120913-IB05300329-modify]//

/**
  AH=47h, OEM Extra Data Read.

  Function 41h and 47h (or 42h) are pairs. The function 41h has to be called before calling into
  function 47h.

  @retval EFI_SUCCESS            OEM Extra Data Read successful.
  @return Other                  OEM Extra Data Read failed.
**/
EFI_STATUS
OemExtraDataRead (
  VOID
  )
{
  EFI_STATUS                       Status;

  Status = Oa30DataRead ();

  IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (Status));
  return Status;
}
//[-start-120913-IB05300329-add]//
//[-start-120917-IB05300333-modify]//
EFI_STATUS
PfatUpdateHandleFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  if (mGlobalNvsArea->PfatMemAddress == 0x00) {
    BiosCommDataBuffer->ErrorReturn = 0x8000;
    return EFI_UNSUPPORTED;
  }

  BiosCommDataBuffer->BlockSize        = AllowPhysicalSizeToCaller;
  BiosCommDataBuffer->DataSize         = MaximunBlockSize;
  BiosCommDataBuffer->PhysicalDataSize = ApCommDataBuffer->PhysicalDataSize;

  return EFI_SUCCESS;
}
//[-end-120917-IB05300333-modify]//

EFI_STATUS
PfatUpdateWrite (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT8                                 *InputDataBuffer;
  UINT32                                PupcOffset;
  UINT32                                PupcSize;
  UINT32                                BufferSize;
  UINT32                                PfatMemSize;
  EFI_PHYSICAL_ADDRESS                  PfatPupcAddress;
  UINT8                                 Trigger;
//[-start-130304-IB05160415-remove]//
//  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
//  EFI_SMM_SYSTEM_TABLE2                 *mSmst;
//[-end-130304-IB05160415-remove]//

  Status          = EFI_SUCCESS;
  InputDataBuffer = NULL;
  PupcOffset      = 0x00;
  PupcSize        = 0x00;
  BufferSize      = 0x00;
  PfatMemSize     = 0x00;
  PfatPupcAddress = 0x00;
  Trigger         = 0x00;

//[-start-130304-IB05160415-remove]//
//  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, &SmmBase);
//  ASSERT_EFI_ERROR (Status);
//  Status = SmmBase->GetSmstLocation (SmmBase, &mSmst);
//  ASSERT_EFI_ERROR (Status);
//  if (EFI_ERROR (Status)) {
//      return Status;
//  }
//[-end-130304-IB05160415-remove]//
//[-start-120919-IB05300333-add]//
  if (mGlobalNvsArea->PfatMemAddress == 0x00) {
    return EFI_UNSUPPORTED;
  }
//[-end-120919-IB05300333-add]//
  InputDataBuffer = (UINT8*) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  PupcOffset      = sizeof (PUP_HEADER) + ((PUP *)InputDataBuffer)->PupHeader.ScriptSectionSize + ((PUP *)InputDataBuffer)->PupHeader.DataSectionSize;
  BufferSize      = IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  PfatMemSize     = (UINT32) LShiftU64 (mGlobalNvsArea->PfatMemSize, 20);
  PfatPupcAddress = (EFI_PHYSICAL_ADDRESS)(mGlobalNvsArea->PfatMemAddress + PfatMemSize - PUPC_MEMORY_OFFSET);

  switch (((PUPC_HEADER *)(InputDataBuffer + PupcOffset))->Algorithm) {
  case 1:
    PupcSize = sizeof (PUPC_ALGORITHM_1);
    break;
  default:
    PupcSize = sizeof (PUPC_ALGORITHM_1);
    break;
  }

  if ((PupcOffset + PupcSize) != BufferSize) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem ((VOID *)mGlobalNvsArea->PfatMemAddress, PfatMemSize);
  CopyMem ((VOID *)mGlobalNvsArea->PfatMemAddress, InputDataBuffer, PupcOffset);
  CopyMem ((VOID *)PfatPupcAddress, InputDataBuffer + PupcOffset, PupcSize);

  //
  // Trigger IO Trap to call PFAT module.
  //
//[-start-130304-IB05160415-modify]//
  Status = gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, mGlobalNvsArea->PfatIoTrapAddress, 1, &Trigger);
//[-end-130304-IB05160415-modify]//

  return Status;
}
//[-end-120913-IB05300329-add]//

//[-start-120917-IB05300333-add]//
EFI_STATUS
PfatReturnHandleFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  )
{
  UINT16                                   *PfatErrorCode;

  PfatErrorCode = NULL;

  if (mGlobalNvsArea->PfatMemAddress == 0x00) {
    BiosCommDataBuffer->ErrorReturn = 0x8000;
    return EFI_UNSUPPORTED;
  }

  BiosCommDataBuffer->BlockSize        = AllowPhysicalSizeToCaller;
  BiosCommDataBuffer->DataSize         = MaximunBlockSize;
  BiosCommDataBuffer->PhysicalDataSize = ApCommDataBuffer->PhysicalDataSize;
  BiosCommDataBuffer->ErrorReturn      = 0x2000;

  PfatErrorCode = (UINT16 *)(UINTN)mGlobalNvsArea->PfatMemAddress;

  if (*PfatErrorCode == 0x00) {
    BiosCommDataBuffer->ReturnCode = 0x00;
  } else {
    CopyMem (&BiosCommDataBuffer->ReturnCode, (VOID *)mGlobalNvsArea->PfatMemAddress, sizeof (BiosCommDataBuffer->ReturnCode));
  }

  return EFI_SUCCESS;
}
//[-end-120917-IB05300333-add]//

