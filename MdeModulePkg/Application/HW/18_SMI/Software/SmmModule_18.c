/** @file
  SmmModule_18 C Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SmmModule_18.h"

EFI_STATUS
EFIAPI
TestEntryPoint (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch2;
  EFI_SMM_SW_REGISTER_CONTEXT               SwContext;
  EFI_SMM_BASE2_PROTOCOL                    *SmmBase;
  EFI_HANDLE                                SwHandle;
  BOOLEAN                                   IsInSmm;

  IsInSmm      = FALSE;
  SmmBase      = NULL;

  //
  // Check in smm or not
  //
  Status = gBS->LocateProtocol(
                            &gEfiSmmBase2ProtocolGuid,
                            NULL,
                            (VOID **)&SmmBase
                            );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &IsInSmm);
  }

  //
  // Install Smm protocol
  //
  if (IsInSmm) {
    Status = gSmst->SmmLocateProtocol (
                                   &gEfiSmmSwDispatch2ProtocolGuid, 
                                   NULL, 
                                   (VOID **)&SwDispatch2
                                   );
    ASSERT_EFI_ERROR (Status);

    SwContext.SwSmiInputValue = EFI_SMM_TEST;

    Status = SwDispatch2->Register (
                                SwDispatch2,
                                TestCallBack,
                                &SwContext,
                                &SwHandle
                                );
    ASSERT_EFI_ERROR (Status);

  } else {
    return EFI_SUCCESS;
  }
  
  return Status;
}

EFI_STATUS
EFIAPI
TestCallBack (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  ) 
{
  EFI_STATUS  Status;
  UINT8       PostCodeData;

  //
  // Read current post code
  // add one and write back
  //
  gSmst->SmmIo.Io.Read(
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    POST_CODE_PORT,
                    1,
                    &PostCodeData
                    );

  PostCodeData++;

  gSmst->SmmIo.Io.Write(        
                     &gSmst->SmmIo,
                     SMM_IO_UINT8,
                     POST_CODE_PORT,
                     1,
                     &PostCodeData
                     );

  Status = EFI_SUCCESS;
  return Status;
}