/** @file
  SmmPowerButton_18 C Source File

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

#include "SmmPowerButton_18.h"

EFI_STATUS
EFIAPI
TestEntryPoint (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_POWER_BUTTON_DISPATCH_PROTOCOL    *PowerButtonDispatch;
  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT     PowerButtonContext;
  EFI_SMM_BASE2_PROTOCOL                    *SmmBase;
  EFI_HANDLE                                PowerButtonHandle;
  BOOLEAN                                   IsInSmm;

  IsInSmm      = FALSE;
  SmmBase      = NULL;
  
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &IsInSmm);
  }

  if (IsInSmm) {
    
    //
    // Locate the PCH SMM Power Button dispatch protocol
    //
    Status = gBS->LocateProtocol (&gEfiSmmPowerButtonDispatchProtocolGuid, NULL, (VOID **)&PowerButtonDispatch);
    ASSERT_EFI_ERROR (Status);

    //
    // Register the power button SMM event
    //
    PowerButtonContext.Phase = PowerButtonEntry;
    Status = PowerButtonDispatch->Register (
                                    PowerButtonDispatch,
                                    TestCallBack,
                                    &PowerButtonContext,
                                    &PowerButtonHandle
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
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT   *DispatchContext
  ) 
{
  EFI_STATUS  Status;
  UINT8       PostCodeData;
  UINT8       CmosSecondOffset;

  CmosSecondOffset = 0;
  
  //
  // Read Cmos to get current second time
  //
  gSmst->SmmIo.Io.Write(
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    CMOS_COMMAND_PORT,
                    1,
                    &CmosSecondOffset
                    );
  gSmst->SmmIo.Io.Read(
                    &gSmst->SmmIo,
                    SMM_IO_UINT8,
                    CMOS_DATA_PORT,
                    1,
                    &PostCodeData
                    );
  
  //
  // Write second to port 80
  //
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