/** @file
  Provide support functions for FVB services which can be executed in
  SMM mode.

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

#include "SmmFunctions.h"
#include "CommonFunctions.h"

SMM_FW_VOL_INSTANCE       *mSmmFwVolInstance;


STATIC
EFI_STATUS
EFIAPI
SmmReadBlock (
  VOID
  );

STATIC
EFI_STATUS
EFIAPI
SmmWriteBlock (
  VOID
  );

STATIC
EFI_STATUS
EFIAPI
SmmEraseBlock (
  VOID
  );

UINT8
EFIAPI
SmmFvbAccessCall (
  IN     UINT8            *InPutBuff,
  IN     UINTN            DataSize,
  IN     UINT8            SubFunNum,
  IN     UINT16           SmiPort
  );

SMI_SUB_FUNCTION_MAP mFvAccessFunctionsTable [] = {
        { PfatReadFvb,   SmmReadBlock  }, \
        { PfatWriteFvb,  SmmWriteBlock }, \
        { PfatEraseFvb,  SmmEraseBlock }, \
        { 0,             NULL          }
        };

/**
  Internal function to initialzie module mSmmFwVolInstance

  @retval EFI_SUCCESS   Initialzie mSmmFwVolInstance successful.
  @retval other         Allocate memory failed or locate gEfiSmmFwBlockServiceProtocolGuid or
                        gEfiSmmCpuProtocolGuid failed.
**/
STATIC
EFI_STATUS
InitializeSmmFwVolInstance (
  VOID
  )
{
  EFI_STATUS        Status;

  mSmmFwVolInstance = FvbAllocateZeroBuffer (sizeof (SMM_FW_VOL_INSTANCE), TRUE);
  ASSERT (mSmmFwVolInstance != NULL);

  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&mSmmFwVolInstance->SmmFwbService
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID **)&mSmmFwVolInstance->SmmCpu
                    );
  return Status;
}

/**
  This fucntion uses to read saved CPU double word register by CPU index

  @param[in]  RegisterNum   Register number which want to get
  @param[in]  CpuIndex      CPU index number to get register.
  @param[out] RegisterData  pointer to output register data

  @retval EFI_SUCCESS       Read double word register successful
  @return Other             Any error occured while disabling all secure boot SMI functions successful.
**/
STATIC
EFI_STATUS
GetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  OUT UINT32                            *RegisterData
  )
{
  return mSmmFwVolInstance->SmmCpu->ReadSaveState (
                                      mSmmFwVolInstance->SmmCpu,
                                      sizeof (UINT32),
                                      RegisterNum,
                                      CpuIndex,
                                      RegisterData
                                      );
}


/**
  This fucnitons uses to read saved CPU double word register

  @param[in] RegisterNum   Register number which want to get

  @return UINT32           The contents of specified register.
**/
STATIC
UINT32
GetDwordRegister (
  IN EFI_SMM_SAVE_STATE_REGISTER       RegisterNum
  )
{
  EFI_STATUS          Status;
  UINT32              RegisterData;

  Status = mSmmFwVolInstance->SmmCpu->ReadSaveState (
                                        mSmmFwVolInstance->SmmCpu,
                                        sizeof (UINT32),
                                        RegisterNum,
                                        mSmmFwVolInstance->CurrentlyExecutingCpu,
                                        &RegisterData
                                        );
  ASSERT_EFI_ERROR (Status);
  return RegisterData;
}

/**
  This fucnitons uses to write saved CPU double word register

  @param[in]  RegisterNum   Register number which want to get
  @param[out] RegisterData  pointer to output register data

  @retval EFI_SUCCESS       Read double word register successful
  @return Other             Any error occured while disabling all secure boot SMI functions successful.
**/
STATIC
EFI_STATUS
SetDwordRegister (
  IN EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  OUT UINT32                           RegisterData
  )
{
  return mSmmFwVolInstance->SmmCpu->WriteSaveState (
                                      mSmmFwVolInstance->SmmCpu,
                                      sizeof (UINT32),
                                      RegisterNum,
                                      mSmmFwVolInstance->CurrentlyExecutingCpu,
                                      &RegisterData
                                      );

}

/**
  This fucntion uses to update current executing CPU.

  @retval EFI_SUCCESS     Update current executing CPU successful.
  @retval EFI_NOT_FOUND   Cannot find current executing CPU.
**/
STATIC
EFI_STATUS
UpdateCurrentExecutingCpu (
  VOID
  )
{
  UINTN           Index;
  UINT32          RegisterEax;
  UINT32          RegisterEdx;
  EFI_STATUS      Status;

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RAX, Index, &RegisterEax);
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RDX, Index, &RegisterEdx);
    if ((RegisterEax & 0xff) == FW_ACCESS_SW_SMI && (RegisterEdx & 0xffff) == SW_SMI_PORT) {
      mSmmFwVolInstance->CurrentlyExecutingCpu = Index;
      Status = EFI_SUCCESS;
      break;
    }
  }
  return Status;
}

/**
  Callback function for accessing FVB in SMM mode

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.

**/
STATIC
EFI_STATUS
EFIAPI
SmmFvbAceessCallback (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommBuffer      OPTIONAL,
  IN OUT UINTN      *CommBufferSize  OPTIONAL
  )
{
  UINTN                          Cmd;
  UINTN                          Index;
  EFI_STATUS                     Status;
  UINT32                         RegisterEax;

  //
  // Since the mSmst->CurrentlyExecutingCpu may be incorrect, so we
  // need update CurrentlyExecutingCpu in our private data and then
  // we can get register directly
  //
  Status = UpdateCurrentExecutingCpu ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  RegisterEax = GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Status = EFI_UNSUPPORTED;
  Cmd = (UINTN)((RegisterEax >> 8) & 0xFF);
  for (Index = 0; mFvAccessFunctionsTable[Index].SmiSubFunction != NULL ; Index++) {
    if (Cmd == (UINTN) mFvAccessFunctionsTable[Index].FunNum) {
      Status = mFvAccessFunctionsTable[Index].SmiSubFunction ();
      break;
    }
  }

  //
  // Convert Status to al register
  //
  Status &= 0xff;
  RegisterEax = (UINT32)((RegisterEax & 0xffffff00) | Status);
  SetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX, RegisterEax);
  return EFI_SUCCESS;
}

/**
  Register software SMI callback function to provide interface to access FVB through SMI.

  @retval EFI_SUCCESS   Register callback function successful.
  @retval Other         Locate gEfiSmmSwDispatch2ProtocolGuid or register callback function failed.
**/
STATIC
EFI_STATUS
RegisterSmmAccessCallback (
  VOID
  )
{
  EFI_SMM_SW_DISPATCH2_PROTOCOL         *SwDispatch;
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;

  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **)&SwDispatch
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SwContext.SwSmiInputValue = FW_ACCESS_SW_SMI;
  Status = SwDispatch->Register (
                         SwDispatch,
                         SmmFvbAceessCallback,
                         &SwContext,
                         &Handle
                         );
  return Status;
}

/**
  Initailize SMM FVB access service.

  1.Initialize moudle SmmFwVolInstance.
  2.Register SMM callback function to provide SMM FVB access interface.

  @retval EFI_SUCCESS   Initialize SMM FVB access service.
  @retval Other         Any error occurred while initializing SMM FVB service.
**/
EFI_STATUS
InitializeSmmFvbAccess (
  VOID
  )
{
  EFI_STATUS        Status;

  Status = InitializeSmmFwVolInstance ();
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = RegisterSmmAccessCallback ();
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  This function uses to read data from specific FV in SMM mode.

  @retval EFI_SUCCESS   Read block from FV successful.
  @retval Other         Any error occurred while reading block from FV.
**/
STATIC
EFI_STATUS
EFIAPI
SmmReadBlock (
  VOID
  )
{
  UINTN              BufferSize;
  SMM_FVB_BUFFER     *SmmFvbBuffer;
  UINT8              *Buffer;
  EFI_STATUS         Status;

  BufferSize   = (UINTN) GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX);
  SmmFvbBuffer = (SMM_FVB_BUFFER *)(UINTN) GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if (BufferSize != sizeof (SMM_FVB_BUFFER) + (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize) ||
      SmmFvbBuffer->Signature != SMM_FVB_ACCESS_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }

  Buffer = (UINT8 *) (SmmFvbBuffer + 1);
  Status = mSmmFwVolInstance->SmmFwbService->Read (
                                               mSmmFwVolInstance->SmmFwbService,
                                               SmmFvbBuffer->AccessAddress,
                                               0,
                                               &SmmFvbBuffer->DataSize,
                                               Buffer
                                               );
  SmmFvbBuffer->Status = Status;
  return Status;
}

/**
  This function uses to write data to specific FV in SMM mode.

  @retval EFI_SUCCESS   Write data to FV successful.
  @retval Other         Any error occurred while writing data to FV.
**/
STATIC
EFI_STATUS
EFIAPI
SmmWriteBlock (
  VOID
  )
{
  UINTN              BufferSize;
  SMM_FVB_BUFFER     *SmmFvbBuffer;
  UINT8              *Buffer;
  EFI_STATUS         Status;

  BufferSize   = (UINTN) GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX);
  SmmFvbBuffer = (SMM_FVB_BUFFER *)(UINTN) GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if (BufferSize != sizeof (SMM_FVB_BUFFER) + (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize) ||
      SmmFvbBuffer->Signature != SMM_FVB_ACCESS_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }

  Buffer = (UINT8 *) (SmmFvbBuffer + 1);
  Status = mSmmFwVolInstance->SmmFwbService->Write (
                                               mSmmFwVolInstance->SmmFwbService,
                                               SmmFvbBuffer->AccessAddress,
                                               &SmmFvbBuffer->DataSize,
                                               Buffer
                                               );
  SmmFvbBuffer->Status = Status;
  return EFI_SUCCESS;
}

/**
  This function uses to erase blocks in specific FV in SMM mode.

  @retval EFI_SUCCESS   Erase blocks in FV successful.
  @retval Other         Any error occurred while erasing block in FV.
**/
STATIC
EFI_STATUS
EFIAPI
SmmEraseBlock (
  VOID
  )
{
  UINTN              BufferSize;
  SMM_FVB_BUFFER     *SmmFvbBuffer;
  UINT8              *Buffer;
  EFI_STATUS         Status;

  BufferSize   = (UINTN) GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX);
  SmmFvbBuffer = (SMM_FVB_BUFFER *)(UINTN) GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if (BufferSize != sizeof (SMM_FVB_BUFFER) + (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize) ||
      SmmFvbBuffer->Signature != SMM_FVB_ACCESS_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }

  Buffer = (UINT8 *) (SmmFvbBuffer + 1);
  Status = mSmmFwVolInstance->SmmFwbService->EraseBlocks (
                                               mSmmFwVolInstance->SmmFwbService,
                                               SmmFvbBuffer->AccessAddress,
                                               &SmmFvbBuffer->DataSize
                                               );
  SmmFvbBuffer->Status = Status;
  return EFI_SUCCESS;
}

/**
  According to access to send SMI to do FVB access.

  @param[in]     AccessType     Input access command type. system support three tyeps of command -
                                PfatReadFvb, PfatWriteFvb and PfatEraseFvb.
  @param[in]     AccessAddress  target address to access.
  @param[in,out] AccessLength   In:  input size in bytes to access.
                                Out: only update this update length if AccessType is PfatReadFvb and
                                     read data successful.
  @param[in,out] AccessBuffer   In:  only use this buffer as input buffer to write flash if AccessType
                                     is PfatWriteFvb.
                                Out: only use this buffer as output buffer to read flash if AccessType
                                     is PfatReadFvb.

  @retval EFI_SUCCESS       Data successfully read from flash device.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to read the blocks.
**/
STATIC
EFI_STATUS
SendSmiByAccessType (
  IN       UINTN      AccessType,
  IN       UINTN      AccessAddress,
  IN OUT   UINTN      *AccessLength,
  IN OUT   VOID       *AccessBuffer
  )
{
  UINTN                       SmmBufSize;
  UINT8                       *WorkingBuf;
  EFI_STATUS                  Status;

  SmmBufSize = sizeof (SMM_FVB_BUFFER) + (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize);
  ZeroMem (mSmmFvbBuffer, SmmBufSize);
  mSmmFvbBuffer->Signature     = SMM_FVB_ACCESS_SIGNATURE;
  mSmmFvbBuffer->AccessAddress = AccessAddress;
  mSmmFvbBuffer->AccessType    = AccessType;
  mSmmFvbBuffer->DataSize      = *AccessLength;

  //
  // Note!!! In runtime, the memory address is virtual address in protected mode but the memory address is
  // physical address in SMM mode. Therefore, if we want to access FV through SMI in runtime, we must set data
  // to virtual address in protected mode and then use physical address to call SMI.
  //

  //
  // copy write data to write buffer before sending SMI
  //
  WorkingBuf = (UINT8 *) (mSmmFvbBuffer + 1);
  if (AccessType == PfatWriteFvb) {
    CopyMem (WorkingBuf, AccessBuffer, *AccessLength);
  }

  SmmFvbAccessCall ((UINT8 *) mSmmPhyFvbBuffer, SmmBufSize, (UINT8) AccessType, SW_SMI_PORT);
  Status = mSmmFvbBuffer->Status;


  //
  // Update access length after executing SMI function
  //
  *AccessLength = mSmmFvbBuffer->DataSize;
  if (!EFI_ERROR (Status) && AccessType == PfatReadFvb) {
    CopyMem (AccessBuffer, WorkingBuf, *AccessLength);
  }

  return Status;
}

/**
  Function to do flash read through SMI.

  @param[in]      ReadAddress  Target address to be read.
  @param[in, out] ReadLength   In:  Input buffer size in bytes.
                               Out: Total read data size in bytes.
  @param[out]     ReadBuffer   Output buffer to contains read data.

  @retval EFI_SUCCESS       Data successfully read from flash device.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to read the blocks.
**/
EFI_STATUS
ReadFdThroughSmi (
  IN       UINTN      ReadAddress,
  IN OUT   UINTN      *ReadLength,
  OUT      VOID       *ReadBuffer
  )
{
  UINTN        RemaindingSize;
  UINTN        ReadSize;
  UINTN        TotalReadSize;
  UINT8        *CurrentBuffer;
  EFI_STATUS   Status;

  RemaindingSize = *ReadLength;
  TotalReadSize  = 0;
  CurrentBuffer  = (UINT8 *) ReadBuffer;
  do {
    if (RemaindingSize > (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize)) {
      ReadSize = (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize);
    } else {
      ReadSize = RemaindingSize;
    }
    RemaindingSize -= ReadSize;
    Status = SendSmiByAccessType (PfatReadFvb, ReadAddress + TotalReadSize, &ReadSize, (VOID *) CurrentBuffer);
    CurrentBuffer += ReadSize;
    TotalReadSize += ReadSize;;
  } while (RemaindingSize != 0 && !EFI_ERROR (Status));

  if (!EFI_ERROR (Status)) {
    *ReadLength = TotalReadSize;
  }

  return Status;
}

/**
  Function to do flash write through SMI.

  @param[in]      WriteAddress  Target address to write.
  @param[in, out] WriteLength   In:  Input buffer size in bytes.
                                Out: Total write data size in bytes.
  @param[out]     WriteBuffer   input buffer to write.

  @retval EFI_SUCCESS       Data successfully write to flash device.
  @retval EFI_UNSUPPORTED   The flash device is not supported.
  @retval EFI_DEVICE_ERROR  Failed to write the blocks.
**/
EFI_STATUS
WriteFdThroughSmi (
  IN       UINTN      WriteAddress,
  IN OUT   UINTN      *WriteLength,
  IN       VOID       *WriteBuffer
  )
{
  UINTN        RemaindingSize;
  UINTN        WriteSize;
  UINTN        TotalWriteSize;
  UINT8        *CurrentBuffer;
  EFI_STATUS   Status;

  RemaindingSize = *WriteLength;
  TotalWriteSize  = 0;
  CurrentBuffer  = (UINT8 *) WriteBuffer;
  do {
    if (RemaindingSize > (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize)) {
      WriteSize = (UINTN) PcdGet32 (PcdFlashNvStorageFtwSpareSize);
    } else {
      WriteSize = RemaindingSize;
    }
    RemaindingSize -= WriteSize;
    Status = SendSmiByAccessType (PfatWriteFvb, WriteAddress + TotalWriteSize, &WriteSize, (VOID *) CurrentBuffer);
    CurrentBuffer += WriteSize;
    TotalWriteSize += WriteSize;;
  } while (RemaindingSize != 0 && !EFI_ERROR (Status));

  if (!EFI_ERROR (Status)) {
    *WriteLength = TotalWriteSize;
  }

  return Status;
}

/**
  Function to do flash erase through SMI.

  @param[in] EraseAddress   Target address to erase.
  @param[in] EraseLength    Erase size in bytes.

  @retval EFI_SUCCESS           Erase flash block successful.
  @retval EFI_UNSUPPORTED       The flash device is not supported.
  @retval EFI_DEVICE_ERROR      Failed to erase blocks.
**/
EFI_STATUS
EraseFdThroughSmi (
  IN  UINTN      EraseAddress,
  IN  UINTN      EraseLength
  )
{
  return SendSmiByAccessType (PfatEraseFvb, EraseAddress, &EraseLength, NULL);
}
