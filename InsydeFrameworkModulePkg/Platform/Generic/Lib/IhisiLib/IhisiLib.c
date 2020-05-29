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
//;   IhisiLib.c
//;

#include "IhisiLib.h"


EFI_SMM_SYSTEM_TABLE                    *mSmst;
EFI_SMM_RUNTIME_PROTOCOL                *mSmmRT;
EFI_SMM_CPU_SAVE_STATE                  *SmmCpuSaveLocalState;
EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL   *mSmmChipsetLibServices;

SMI_SUB_FUNCTION                        *mFunctionTableBuffer;
UINTN                                   mFunctionTableCount = 0;
EFI_SMRAM_DESCRIPTOR                    *mSmramRanges;
UINTN                                   mSmramRangeCount;

VOID (*IhisiOemHook)(UINTN) = NULL;

VOID
IhisiCallback(
  IN  EFI_HANDLE                        DispatchHandle,
  IN  EFI_SMM_SW_DISPATCH_CONTEXT       *DispatchContext
  );

STATIC
EFI_STATUS
EFIAPI
InitSmramRanges (
  VOID
  )
/*++

Routine Description:

  The function caches SMRAM ranges that are present in the system.

Arguments:

  None

Returns:

  EFI_SUCCESS - Initialize Ihisi Driver success.
  Others      - Initialize Ihisi Driver failed.

--*/
{
  EFI_STATUS                Status;
  EFI_SMM_ACCESS_PROTOCOL  *SmmAccess;
  UINTN                     Size;

  //
  // Locate SMM Access Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmAccessProtocolGuid,
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
  Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, Size, (VOID **) &mSmramRanges);
  ASSERT (mSmramRanges != NULL);

  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, mSmramRanges);
  ASSERT_EFI_ERROR (Status);

  mSmramRangeCount = Size / sizeof (EFI_SMRAM_DESCRIPTOR);

  return EFI_SUCCESS;
}

EFI_STATUS
IhisiLibInit (
  IN  SMI_SUB_FUNCTION                  *FunctionTableBuffer,
  IN  UINTN                             FunctionTableCount
  )
/*++

Routine Description:

  Initialize Ihisi Driver.

Arguments:

  FunctionTableBuffer - Ihisi sub functions buffer.
  FunctionTableCount  - Ihisi sub functions count.

Returns:

  EFI_SUCCESS - Initialize Ihisi Driver success.
  Others      - Initialize Ihisi Driver failed.

--*/
{
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  EFI_STATUS                            Status;
  EFI_SMM_SW_DISPATCH_PROTOCOL          *SwDispatch;
  EFI_SMM_SW_DISPATCH_CONTEXT           SwContext;
  EFI_HANDLE                            SwHandle;
  BOOLEAN                               InSmm;

  SwHandle = NULL;
  InSmm = 0;

  Status = gSMM->GetSmstLocation (gSMM, &mSmst);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gSMM->InSmm (gSMM, &InSmm);

  if (InSmm) {

    Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &SmmRT);
    if (EFI_ERROR (Status)) {
      return Status;
    }


    Status = SmmRT->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &mSmmRT);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gBS->LocateProtocol (&gEfiSmmSwDispatchProtocolGuid, NULL, &SwDispatch);
    if (EFI_ERROR (Status)) {
       return Status;
    }

    Status = SmmRT->LocateProtocol (
                      &gEfiSmmChipsetLibServicesProtocolGuid,
                      NULL,
                      &mSmmChipsetLibServices
                      );
    if (EFI_ERROR(Status)) {
      return Status;
    }
    InitSmramRanges ();
    //
    // Get Register mFunctionTableBuffer & mFunctionTableCount.
    //
    mFunctionTableCount = FunctionTableCount;

    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      mFunctionTableCount * sizeof (SMI_SUB_FUNCTION),
                      &mFunctionTableBuffer
                      );

    if (EFI_ERROR (Status)) {
      mFunctionTableCount = 0;
    } else {
      EfiCopyMem (
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
  }

  return EFI_SUCCESS;
}


VOID
IhisiCallback(
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_DISPATCH_CONTEXT   *DispatchContext
  )
/*++
Routine Description:

  Main registers for Insyde H2O BIOS Soft-SMI function call.

Registers	Value	Description

  AL:	0EFh	Main function number.
  AH:	nnh	Sub-function number.
  EBX: '$H2O'	Insyde H2O BIOS Soft-SMI function signature.
       Case sensitive.

--*/
{
  UINTN                                 Cmd, Index;
  BOOLEAN                               CheckEBX = 0;
  UINT8                                 *Buffer;

  SmmCpuSaveLocalState = (EFI_SMM_CPU_SAVE_STATE *)(mSmst->CpuSaveState);
  Buffer = (UINT8 *)(mSmst->CpuSaveState);

  //
  // Try to find current CPU and ckeck EBX
  //
  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {

    if ((SmmCpuSaveLocalState->EBX == IHISI_EBX_SIGNATURE)) {
      CheckEBX = 1;
      break;
    }
    Buffer += sizeof (EFI_SMM_CPU_SAVE_STATE);
    SmmCpuSaveLocalState = (EFI_SMM_CPU_SAVE_STATE *)Buffer;
  }

  if (CheckEBX == 0) {
    IhisiLibErrorCodeHandler ((UINT32)IhisiAccessProhibited);
    return;
  }

  Cmd = (UINTN)((SmmCpuSaveLocalState->EAX >> 8) & 0xFF); // Abstrct AH
  for (Index = 0; Index < mFunctionTableCount; Index++) {
    if (Cmd == mFunctionTableBuffer[Index].SubFun) {
      if (IhisiOemHook != NULL) {
        IhisiOemHook (Cmd);
      }
      mFunctionTableBuffer[Index].SubFunction ();
      break;
    }
  }

  if (Index >= mFunctionTableCount) {
    IhisiLibErrorCodeHandler ((UINT32)IhisiFunNotSupported);
  }
  return;
}


VOID
IhisiLibErrorCodeHandler (
  IN     UINT32                          IhisiStatus
  )
/*++
Routine Description:

  Returned error code in AL.

Arguments:

  Ihisi error code status

Returns:

  NONE

--*/
{
  SmmCpuSaveLocalState->EAX = (UINT32)((SmmCpuSaveLocalState->EAX & 0xffffff00) | IhisiStatus);

  return;
}


BOOLEAN
EFIAPI
BufferOverlapSmram (
  IN VOID   *Buffer,
  IN UINTN  BufferSize
  )
/*++

Routine Description:

  Check the input memory buffer is whether overlap the SMRAM ranges.

Arguments:

  Buffer     - The pointer to the buffer to be checked.
  BufferSize - The size in bytes of the input buffer.

Returns:

  TURE       - he buffer overlaps SMRAM ranges.
  FALSE      - The buffer doesn't overlap SMRAM ranges.

--*/
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