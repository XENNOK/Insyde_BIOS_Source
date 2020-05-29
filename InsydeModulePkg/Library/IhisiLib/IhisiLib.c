/** @file
  IhisiLib Library Instance implementation

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
#include <Library/SeamlessRecoveryLib.h>
#include <Library/LockBoxLib.h>
#include "SecureFlash.h"

#define FBTS_COMPLETE_CMD               0x16

EFI_SMM_CPU_PROTOCOL                    *mSmmCpu;
SMI_SUB_FUNCTION                        *mFunctionTableBuffer;
UINTN                                   mFunctionTableCount;
UINTN                                   mCpuIndex;
EFI_SMRAM_DESCRIPTOR                    *mSmramRanges;
UINTN                                   mSmramRangeCount;

VOID (*IhisiOemHook)(UINTN) = NULL;

/**
  This fucntion uses to read saved CPU double word register by CPU index

  @param[in]  RegisterNum   Register number which want to get
  @param[in]  CpuIndex      CPU index number to get register.
  @param[in]  RegisterData  pointer to output register data

  @retval EFI_SUCCESS       Read double word register successfully
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
  return mSmmCpu->ReadSaveState (
                    mSmmCpu,
                    sizeof (UINT32),
                    RegisterNum,
                    CpuIndex,
                    RegisterData
                    );
}

/**
  This fucntion uses to set saved CPU double word register by CPU index

  @param[in]  RegisterNum   Register number which want to get
  @param[in]  CpuIndex      CPU index number to get register.
  @param[in]  RegisterData  pointer to output register data

  @retval EFI_SUCCESS       Set double word register successfully
  @return Other             Any error occured while disabling all secure boot SMI functions successful.
**/
STATIC
EFI_STATUS
SetDwordRegisterByCpuIndex (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINTN                             CpuIndex,
  IN  UINT32                            *RegisterData
  )
{
  return mSmmCpu->WriteSaveState (
                    mSmmCpu,
                    sizeof (UINT32),
                    RegisterNum,
                    CpuIndex,
                    RegisterData
                    );
}

/**
  This function uses to read saved CPU double word register from current executing CPU.

  @param[in] RegisterNum  Register number which want to get

  @return UINT32          Specific register UINT32 value.
**/
UINT32
IhisiLibGetDwordRegister (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum
  )
{
  EFI_STATUS      Status;
  UINT32          RegisterData;

  Status = GetDwordRegisterByCpuIndex (RegisterNum, mCpuIndex, &RegisterData);
  ASSERT_EFI_ERROR (Status);
  return RegisterData;
}

/**
  This function uses to write specific double word value to current executing CPU specific register.

  @param[in] RegisterNum  Register number which want to get

  @return UINT32          Specific register UINT32 value.
**/
EFI_STATUS
IhisiLibSetDwordRegister (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINT32                            RegisterData
  )
{
  return SetDwordRegisterByCpuIndex (RegisterNum, mCpuIndex, &RegisterData);
}

/**
  This function uses to get current executing CPU index.

  @return UINTN       Current executing CPU index.
**/
UINTN
IhisiLibGetCpuIndex (
  VOID
  )
{
  return mCpuIndex;
}

EFI_STATUS
EFIAPI
IhisiCallback (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommBuffer      OPTIONAL,
  IN OUT UINTN      *CommBufferSize  OPTIONAL
  );


/**
  Initialize Ihisi Driver.

  @param[in] FunctionTableBuffer  Ihisi sub functions buffer.
  @param[in] FunctionTableCount   Ihisi sub functions count.

  @retval EFI_SUCCESS             Initialize Ihisi Driver success.
  @return Others                  Initialize Ihisi Driver failed.
**/
EFI_STATUS
IhisiLibInit (
  IN  SMI_SUB_FUNCTION                  *FunctionTableBuffer,
  IN  UINTN                             FunctionTableCount
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL         *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_HANDLE                            SwHandle;


  if (!InSmm ()) {
    return EFI_UNSUPPORTED;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **)&SwDispatch
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID **)&mSmmCpu
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //Get Register mFunctionTableBuffer & mFunctionTableCount.
  //
  mFunctionTableCount = FunctionTableCount;

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    mFunctionTableCount * sizeof (SMI_SUB_FUNCTION),
                    (VOID **)&mFunctionTableBuffer
                    );

  if (EFI_ERROR (Status)) {
    mFunctionTableCount = 0;
  } else {
    CopyMem (
      mFunctionTableBuffer,
      FunctionTableBuffer,
      mFunctionTableCount * sizeof (SMI_SUB_FUNCTION)
      );
  }
  //
  // Software SMI for InsydeSSmiHandler
  //
  SwContext.SwSmiInputValue = SW_SMI_INPUT_VALUE;
  Status = SwDispatch->Register (
                         SwDispatch,
                         IhisiCallback,
                         &SwContext,
                         &SwHandle
                         );
  if (EFI_ERROR (Status)) {
     return Status;
  }


  return EFI_SUCCESS;
}

VOID
UpdateApRequestFlag (
  UINT8                         ApRequest
  )
{
  UINT32                        Ecx;

  Ecx = (IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX) & 0xffffff00) | ApRequest;
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx);
}

VOID
RecoveryUpdateCheck (
  VOID
  )
{
  BOOLEAN                       RecoveryFlag;
  UINTN                         Size;
  EFI_STATUS                    Status;

  RecoveryFlag = FALSE;
  Size = sizeof (RecoveryFlag);
  Status = RestoreLockBox (&gSecureFlashInfoGuid, &RecoveryFlag, &Size);
  if (EFI_ERROR (Status)) {
    return;
  }
  if (RecoveryFlag) {
    UpdateApRequestFlag (0x00);
  }
}

VOID
CapsuleUpdateCheck (
  VOID
  )
{

  //
  // Capsule update function will set FirmwareFailureRecovery flag before triggering firmware update
  // and clear the flag after the update process is completed
  //
  if (IsFirmwareFailureRecovery ()) {
    //
    // Clear action flag to prevent system from reset or shutdown
    //
    UpdateApRequestFlag (0x00);
  }
}

/**
  Main registers for Insyde H2O BIOS Soft-SMI function call.

  @param[in]     DispatchHandle               The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context                      Points to an optional handler context which was specified when the
                                              handler was registered.
  @param[in,out] CommBuffer                   A pointer to a collection of data in memory that will
                                              be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize               The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.

**/
EFI_STATUS
EFIAPI
IhisiCallback (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommBuffer      OPTIONAL,
  IN OUT UINTN      *CommBufferSize  OPTIONAL
  )
{
  UINTN                         Index;
  UINT32                        Ebx;
  UINTN                         Cmd;

  //
  // Find out which CPU triggered the S/W SMI
  //
  for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    GetDwordRegisterByCpuIndex (EFI_SMM_SAVE_STATE_REGISTER_RBX, Index, &Ebx);
    if (Ebx == IHISI_EBX_SIGNATURE) {
      break;
    }
  }

  if (Index == gSmst->NumberOfCpus) {
    IhisiLibErrorCodeHandler ((UINT32)IhisiAccessProhibited);
    return EFI_NOT_FOUND;
  }

  mCpuIndex = Index;
  Cmd = (UINTN)((IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX) >> 8) & 0xFF);
  for (Index = 0; Index < mFunctionTableCount; Index++) {
    if (Cmd == mFunctionTableBuffer[Index].SubFun) {
      if (IhisiOemHook != NULL) {
        IhisiOemHook (Cmd);
      }
      if (Cmd == FBTS_COMPLETE_CMD) {
        if (PcdGetBool (PcdSecureFlashSupported)) {
          //
          // If the firmware update is initiated by capsule update, IHISI FBTS Complete should not
          // reset or shutdown the system, otherwise the ESRT status won't get updated
          //
          CapsuleUpdateCheck ();

          //
          // In Crisis recovery process, IHISI FBTS complete should not reset or shutdown the system
          //
          RecoveryUpdateCheck ();
        }
      }
      mFunctionTableBuffer[Index].SubFunction ();
      break;
    }
  }

  if (Index >= mFunctionTableCount) {
    IhisiLibErrorCodeHandler ((UINT32)IhisiFunNotSupported);
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Returned error code in AL.

  @param[in] IhisiStatus  Returned error code in AL.
**/
VOID
IhisiLibErrorCodeHandler (
  IN     UINT32                          IhisiStatus
  )
{
  UINT32                        Eax;

  Eax = IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Eax = (UINT32) ((Eax & 0xffffff00) | IhisiStatus);
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);
  return;
}

/**
  The constructor function caches SMRAM ranges that are present in the system.

  It will ASSERT() if SMM Access2 Protocol doesn't exist.
  It will ASSERT() if SMRAM ranges can't be got.
  It will ASSERT() if Resource can't be allocated for cache SMRAM range.
  It will always return EFI_SUCCESS.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
IhisiLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_SMM_ACCESS2_PROTOCOL  *SmmAccess;
  UINTN                     Size;

  if (!InSmm ()) {
    return EFI_SUCCESS;
  }
  //
  // Locate SMM Access2 Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmAccess2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmAccess
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get SMRAM range information
  //
  Size = 0;
  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);
  mSmramRanges = NULL;
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, Size, (VOID **) &mSmramRanges);
  ASSERT (mSmramRanges != NULL);

  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, mSmramRanges);
  ASSERT_EFI_ERROR (Status);

  mSmramRangeCount = Size / sizeof (EFI_SMRAM_DESCRIPTOR);

  return EFI_SUCCESS;
}

/**
  If SMM driver exits with an error, it must call this routine
  to free the allocated resource before the exiting.

  @param[in]  ImageHandle   The firmware allocated handle for the EFI image.
  @param[in]  SystemTable   A pointer to the EFI System Table.

  @retval     EFI_SUCCESS   The deconstructor always returns EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
IhisiLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (mSmramRanges != NULL) {
    gSmst->SmmFreePool (mSmramRanges);
  }
  return EFI_SUCCESS;
}

/**
  Check the input memory buffer is whether overlap the SMRAM ranges.

  @param[in] Buffer       The pointer to the buffer to be checked.
  @param[in] BufferSize   The size in bytes of the input buffer

  @retval  TURE        The buffer overlaps SMRAM ranges.
  @retval  FALSE       The buffer doesn't overlap SMRAM ranges.
**/
BOOLEAN
EFIAPI
BufferOverlapSmram (
  IN VOID   *Buffer,
  IN UINTN  BufferSize
  )
{
  UINTN                 Index;
  EFI_PHYSICAL_ADDRESS  BufferStart;
  EFI_PHYSICAL_ADDRESS  BufferEnd;

  if (Buffer == NULL || BufferSize == 0) {
    return FALSE;
  }

  BufferStart = (EFI_PHYSICAL_ADDRESS) (UINTN) Buffer;
  BufferEnd =   (EFI_PHYSICAL_ADDRESS) ((UINTN) Buffer + BufferSize - 1);

  for (Index = 0; Index < mSmramRangeCount; Index ++) {
    //
    // The condition for two ranges doesn't overlap is:
    // Buffer End is smaller than the range start or Buffer start is larger than the range end.
    // so the overlap condition is above condition isn't satisfied.
    //
    if (!(BufferEnd < mSmramRanges[Index].CpuStart ||
        BufferStart >= (mSmramRanges[Index].CpuStart + mSmramRanges[Index].PhysicalSize))) {
      return TRUE;
    }
  }

  return FALSE;
}