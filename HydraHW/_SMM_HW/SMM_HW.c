/** @file
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/
#include "SMM_HW.h"

VOID
EFIAPI
HydraPowerButtonCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT   *DispatchContext
  )
{
  EFI_STATUS                             Status;
  EFI_CPU_IO2_PROTOCOL                   *CpuIo;
  UINT8                                  TimeData;

  TimeData = 0x00;
  //
  // Locate CpuIo2 Protocol
  //
  Status = gBS->LocateProtocol(
                  &gEfiCpuIo2ProtocolGuid,
                  NULL,
                  &CpuIo
                  );
  if (EFI_ERROR (Status)) {
    Print(
      L"LocateProtocol gEfiCpuIo2ProtocolGuid Fail : %2d\n",
      Status
      );
  }
  //
  // Send Cmos Command to 0x70.
  //
  Status = CpuIo->Io.Write (
                       CpuIo,
                       EfiCpuIoWidthUint8,
                       CMOS_CMD,
                       1,
                       &TimeData
                       );
  if (EFI_ERROR (Status)) {
    Print (
      L"CpuIo->Io.Write Fail : %2d ,%d\n",
      Status,
      TimeData
      );
  }
  //
  // Read time(second) from 0x71.
  //
  Status = CpuIo->Io.Read (
                        CpuIo,
                        EfiCpuIoWidthUint8,
                        CMOS_DATA,
                        1,
                        &TimeData
                        );
  if (EFI_ERROR (Status)) {
    Print (
      L"CpuIo->Io.Read Fail : %2d\n",
      Status
      );
  }
  //
  // Write TimeData to 80 Port.
  //
   IoWrite8(0x80,TimeData);
}

EFI_STATUS
EnableHydraCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
  {
   UINT8       PortBuffer;

   PortBuffer = IoRead8(0x80);
   PortBuffer++;
   IoWrite8(0x80,PortBuffer);

   return EFI_SUCCESS;
  }

EFI_STATUS
HydraMain (
  IN EFI_HANDLE       ImageHandle, 
  IN EFI_SYSTEM_TABLE *SystemTable
)
{
	EFI_STATUS                            Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL         *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_HANDLE                            SwHandle;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
  
  EFI_SMM_SYSTEM_TABLE2          *mSmst;  

  BOOLEAN                               InSmm;

  //
  // PowerButton Zone Start
  //
  EFI_SMM_POWER_BUTTON_DISPATCH_PROTOCOL    *PowerButtonDispatch;
  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT     PowerButtonContext;
  EFI_HANDLE                                PowerButtonHandle;

  PowerButtonHandle = NULL;
  //
  //PowerButton Zone End
  //
  InSmm = FALSE;
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID**)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  } else {
    InSmm = FALSE;
  }

  if (!InSmm) {
    DEBUG (((EFI_D_ERROR | EFI_D_INFO), "\n Not In SMM !!! %r\n#\n", Status));
    return EFI_SUCCESS;
  }
  
  Status = SmmBase->GetSmstLocation (SmmBase, &mSmst);
  Status = mSmst->SmmLocateProtocol (
                  &gEfiSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &SwDispatch
                  );
  if (EFI_ERROR (Status)) {
    DEBUG (((EFI_D_ERROR | EFI_D_INFO),"EFI_ERROR gEfiSmmSwDispatch2ProtocolGuid %r\n",Status));
    return Status;
  }
  SwContext.SwSmiInputValue = EFI_HYDRA_ENABLE_SW_SMI;
  Status = SwDispatch->Register (
                         SwDispatch,
                         EnableHydraCallback,
                         &SwContext,
                         &SwHandle
                         );
  if (EFI_ERROR (Status)) {
    DEBUG (((EFI_D_ERROR | EFI_D_INFO),"EFI_ERROR SwDispatch->Register %r\n",Status));
    return Status;
  }

  //
  // PowerButton Area
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmPowerButtonDispatchProtocolGuid,
                  NULL,
                  (VOID **)&PowerButtonDispatch
                  );
  if (EFI_ERROR (Status)) {
    DEBUG (((EFI_D_ERROR | EFI_D_INFO),"EFI_ERROR gEfiSmmPowerButtonDispatchProtocolGuid %r\n",Status));
    return Status;
  }  
  //
  // UnRegister orignal power button SMM event.
  // Status = PowerButtonDispatch->UnRegister (
  //                                 PowerButtonDispatch,
  //                                 PowerButtonHandle
  //                               );
  //
  // Register the power button SMM event
  //
  PowerButtonContext.Phase = PowerButtonEntry;

  Status = PowerButtonDispatch->Register (
    PowerButtonDispatch,
    HydraPowerButtonCallback,
    &PowerButtonContext,
    &PowerButtonHandle
    );
  if (EFI_ERROR (Status)) {
    DEBUG (((EFI_D_ERROR | EFI_D_INFO),"EFI_ERROR PowerButtonDispatch->Register %r\n",Status));
    return Status;
  }   
  return Status;
}