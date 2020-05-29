/** @file

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

//[-start-121122-IB07250300-modify]//
#include <Platform.h>
#include <PchAccess.h>
#include <SaAccess.h>
#include <SwitchableGraphicsDefine.h>

#include <Library/IoLib.h>
#include <Guid/SwitchableGraphicsVariable.h>
#include <Protocol/SmmVariable.h>

BOOLEAN                                       mHdaStatus;
SG_VARIABLE_CONFIGURATION                     mSgData;

VOID
SetSgVariable (
  IN EFI_GLOBAL_NVS_AREA                      *GlobalNvsArea
  )
{
  EFI_SMM_VARIABLE_PROTOCOL                   *SmmVariable;
  EFI_STATUS                                  Status;
  SG_VARIABLE_CONFIGURATION                   SgData;
  UINTN                                       Size;

  if (GlobalNvsArea->PegVendorID != NVIDIA_VID) {
    return;
  }
  Status = mSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID **)&SmmVariable);
  if (EFI_ERROR (Status)) {
    return;
  }
  Size = sizeof (SG_VARIABLE_CONFIGURATION);
  Status = SmmVariable->SmmGetVariable (
                          L"SwitchableGraphicsVariable",
                          &gH2OSwitchableGraphicsVariableGuid,
                          NULL,
                          &Size,
                          &SgData
                          );
  if (EFI_ERROR (Status)) {
    return;
  }
  if (SgData.OptimusVariable.OptimusFlag != GlobalNvsArea->OptimusFlag) {
    SgData.OptimusVariable.OptimusFlag = GlobalNvsArea->OptimusFlag;
    Status = SmmVariable->SmmSetVariable (
                            L"SwitchableGraphicsVariable",
                            &gH2OSwitchableGraphicsVariableGuid,
                            EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                            sizeof (SG_VARIABLE_CONFIGURATION),
                            &SgData
                            );
    if (EFI_ERROR (Status)) {
      return;
    }
  }
}

VOID
OpenHdAudio (
  IN EFI_GLOBAL_NVS_AREA                      *GlobalNvsArea
  )
{
  EFI_SMM_VARIABLE_PROTOCOL                   *SmmVariable;
  EFI_STATUS                                  Status;
  UINT32                                      Reg488;
  UINTN                                       Size;

  mHdaStatus = TRUE;
  if (GlobalNvsArea->PegVendorID != NVIDIA_VID) {
    return;
  }

  Status = mSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID **)&SmmVariable);
  if (EFI_ERROR (Status)) {
    return;
  }
  Size = sizeof (SG_VARIABLE_CONFIGURATION);
  Status = SmmVariable->SmmGetVariable (
                          L"SwitchableGraphicsVariable",
                          &gH2OSwitchableGraphicsVariableGuid,
                          NULL,
                          &Size,
                          &mSgData
                          );
  if (EFI_ERROR (Status)) {
    return;
  }

  Reg488 = MmPci32 (0, mSgData.OptimusVariable.MasterDgpuBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, NVIDIA_DGPU_HDA_REGISTER);
  if ((Reg488 & BIT25) == 0) {
    mHdaStatus = FALSE;
    MmPci32Or (0, mSgData.OptimusVariable.MasterDgpuBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, NVIDIA_DGPU_HDA_REGISTER, BIT25);
  }
}
VOID
CloseHdAudio (
  IN EFI_GLOBAL_NVS_AREA                      *GlobalNvsArea
  )
{
  if (GlobalNvsArea->PegVendorID != NVIDIA_VID) {
    return;
  }
  if (!mHdaStatus) {
    MmPci32And (0, mSgData.OptimusVariable.MasterDgpuBus, DGPU_DEVICE_NUM, DGPU_FUNCTION_NUM, NVIDIA_DGPU_HDA_REGISTER, ~(BIT25));
  }
}
//[-end-121122-IB07250300-modify]//
