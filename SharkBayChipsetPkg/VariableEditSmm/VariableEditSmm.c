/** @file
  Content file contains function definitions for Variable Edit Smm Call back driver

;******************************************************************************
;* Copyright (c) 1983 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "VariableEditSmm.h"


EFI_SMM_VARIABLE_PROTOCOL               *SmmVariable;
EFI_SMM_CPU_PROTOCOL                    *mSmmCpu;


EFI_STATUS
EFIAPI
SetupCallbackSyncEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL          *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_HANDLE                            SwHandle;
  SETUP_CALLBACK_SYNC_PROTOCOL          *CallbackSync;

//[-start-140721-IB08620401-add]//
  VARIABLE_EDIT_FUNCTION_DISABLE_SETTING    VarEditFunDis = {0};
  UINTN                        VarEditFunDisSize = sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING);
//[-end-140721-IB08620401-add]//

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID **)&SmmVariable
                    );

  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-140721-IB08620401-add]//
  Status = SmmVariable->SmmGetVariable (
                          VARIABLE_EDIT_FUNCTION_DISABLE_VARIABLE_NAME,
                          &gVarableEditFunctionDisableGuid,
                          NULL,
                          &VarEditFunDisSize,
                          &VarEditFunDis
                          );

  if ( VarEditFunDisSize != sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING)) {
    //DEBUG ( (EFI_D_INFO, "Please check sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING) in Variable Store."));
  }
  if ( (!EFI_ERROR (Status)) &&
       (VarEditFunDis.VariableEditFunDis == TRUE) &&
       (VarEditFunDisSize == sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING))
     ) {
    return EFI_UNSUPPORTED;
  }
//[-end-140721-IB08620401-add]//

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid, 
                    NULL, 
                    (VOID **)&SwDispatch)
                    ;
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
  // Software SMI for InsydeSmiHandler
  //
  SwContext.SwSmiInputValue = IVE_SW_SMI;
  Status = SwDispatch->Register (
             SwDispatch,
             SetupCallbackSyncCallback,
             &SwContext,
             &SwHandle
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gSetupCallbackSyncGuid, 
                  NULL, 
                  (VOID **)&CallbackSync
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CallbackSync->CallbackSmiNumber = IVE_SW_SMI;

  return EFI_SUCCESS;
}

UINTN
AsciiStrLength (
  UINT8           *s1
  )
{
  UINTN len;

  for (len = 0; *s1; s1 += 1, len += 1) {
    ;
  }
  return len;
}

/**
  Check resource conflict of Isa devices and PCI solts.
  
 @param [in]   CHIPSET_CONFIGURATION     Setup variable
 @param [in]   PciIrqData                           Setup variable of PCI solt
 @param [in]   DeviceKind                          Return conflict device.

 @retval TRUE   Resources of ISA devices conflict.
 @retval FALSE  Resources of ISA devices don't conflict.

**/
BOOLEAN
CheckSioAndPciSoltConflict (
  IN  CHIPSET_CONFIGURATION                    *SetupNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  )
{
  BOOLEAN                               ConflictFlag = FALSE;
  UINT8                                 SlotNum;
  BOOLEAN                               ComPortIrq[2] = {FALSE, FALSE};

  *DeviceKind = 0;

  for (SlotNum = 0; SlotNum < MAXPCISLOT; SlotNum++) {
    if (* (PciIrqData + SlotNum) == 3) {
      ComPortIrq[0] = TRUE;
    }
    if (* (PciIrqData + SlotNum) == 4) {
      ComPortIrq[1] = TRUE;
    }
  }
  //
  //Check ComPort A auto or ComPort B auto
  //
  if (!ComPortIrq[0] && ComPortIrq[1]) {
    if (SetupNVData->ComPortA == 1 && SetupNVData->ComPortB == 0) {
      SetupNVData->ComPortAInterrupt = 3;
    }
    if (SetupNVData->ComPortA == 0 && SetupNVData->ComPortB == 1) {
      SetupNVData->ComPortBInterrupt = 3;
    }
  }
  if (ComPortIrq[0] && !ComPortIrq[1]) {
    if (SetupNVData->ComPortA == 1 && SetupNVData->ComPortB == 0) {
      SetupNVData->ComPortAInterrupt = 4;
    }
    if (SetupNVData->ComPortA == 0 && SetupNVData->ComPortB == 1) {
      SetupNVData->ComPortBInterrupt = 4;
    }
  }

  if (SetupNVData->ComPortA > 0) {
    for (SlotNum = 0; SlotNum < MAXPCISLOT; SlotNum++) {
      if (SetupNVData->ComPortAInterrupt == * (PciIrqData + SlotNum)) {
        ConflictFlag = TRUE;
        *DeviceKind = 0;
        break;
      }
    }
  }
  if (SetupNVData->ComPortB > 0) {
    for (SlotNum = 0; SlotNum < MAXPCISLOT; SlotNum++) {
      if (SetupNVData->ComPortBInterrupt == * (PciIrqData + SlotNum)) {
        ConflictFlag = TRUE;
        *DeviceKind = 1;
        break;
      }
    }
  }

  return ConflictFlag;
}

/**
  Check resource conflict of Isa devices

 @param [in]   SetupNVData     Setup variable

 @retval TRUE   Resources of ISA devices conflict.
 @retval FALSE  Resources of ISA devices don't conflict.

**/
BOOLEAN
CheckSioConflict (
  IN  CHIPSET_CONFIGURATION                    *SetupNVData
  )
{
  BOOLEAN                               ConflictFlag = FALSE;

  //
  //  Check Resource Conflict:IRQ,IObase,DMA
  //
  if (SetupNVData->ComPortB == 2) {
    if (SetupNVData->ComPortA == 2) {
      if ( (SetupNVData->ComPortABaseIo == SetupNVData->ComPortBBaseIo) ||
           (SetupNVData->ComPortAInterrupt == SetupNVData->ComPortBInterrupt)) {
        SetupNVData->ComPortA = 1;
        SetupNVData->ComPortB = 0;
        ConflictFlag = TRUE;
      }
    }
  }

  //
  //  Check ComPortA Auto & disable setting
  //
  if (SetupNVData->ComPortA < 2) {
    if ( (SetupNVData->ComPortB == 2) && (SetupNVData->ComPortBBaseIo == 3)) {
      SetupNVData->ComPortABaseIo = 1;
    } else {
      SetupNVData->ComPortABaseIo = 3;
    }
    if ( (SetupNVData->ComPortB == 2) && (SetupNVData->ComPortBInterrupt == 4)) {
      SetupNVData->ComPortAInterrupt = 3;
    } else {
      SetupNVData->ComPortAInterrupt = 4;
    }
  }

  //
  //  Check ComPortB Auto & disable setting
  //
  if (SetupNVData->ComPortB < 2) {
    if ( (SetupNVData->ComPortA == 2) && (SetupNVData->ComPortABaseIo == 1)) {
      SetupNVData->ComPortBBaseIo = 3;
    } else {
      SetupNVData->ComPortBBaseIo = 1;
    }
    if ( (SetupNVData->ComPortA == 2) && (SetupNVData->ComPortAInterrupt == 3)) {
      SetupNVData->ComPortBInterrupt = 4;
    } else {
      SetupNVData->ComPortBInterrupt = 3;
    }
    if (SetupNVData->ComPortB == 0) {
      SetupNVData->ComPortBMode = 0;
    }
  }

  return ConflictFlag;
}

EFI_STATUS
WriteNVSetup (
  IN VOID *Buffer
  )
{
  EFI_GUID    GuidId = SYSTEM_CONFIGURATION_GUID;
//[-start-130709-IB05160465-modify]//
  UINTN       BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = SmmVariable->SmmSetVariable (
                          L"Setup",
                          &GuidId,
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                          BufferSize,
                          Buffer
                          );
  return Status;
}

EFI_STATUS
ReadNVSetup (
  OUT VOID *Buffer
  ) 
{
  EFI_GUID    GuidId = SYSTEM_CONFIGURATION_GUID;
//[-start-130709-IB05160465-modify]//
  UINTN       BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = SmmVariable->SmmGetVariable (
                          L"Setup",
                          &GuidId,
                          NULL,
                          &BufferSize,
                          Buffer
                          );
  return Status;
}


EFI_STATUS
WriteNVCustom (
  IN VOID *Buffer
  ) 
{
  EFI_GUID    GuidId = SYSTEM_CONFIGURATION_GUID;
//[-start-130709-IB05160465-modify]//
  UINTN       BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = SmmVariable->SmmSetVariable (
                          L"Custom",
                          &GuidId,
                          EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                          BufferSize,
                          Buffer
                          );
  return Status;
}

EFI_STATUS
ReadNVCustom (
  OUT VOID *Buffer
  )
{
  EFI_GUID    GuidId = SYSTEM_CONFIGURATION_GUID;
//[-start-130709-IB05160465-modify]//
  UINTN       BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  EFI_STATUS  Status = EFI_SUCCESS;

  Status = SmmVariable->SmmGetVariable (
                          L"Custom",
                          &GuidId,
                          NULL,
                          &BufferSize,
                          Buffer
                          );
  return Status;
}

EFI_STATUS
EFIAPI
SetupCallbackSyncCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{

  UINTN                                 Index;
  BOOLEAN                               CheckEBX;
  CHIPSET_CONFIGURATION                 *SetupNVData;
  CHIPSET_CONFIGURATION                 *OldSetupNVData;
  CALLBACK_LIST                         *CallbackList;
  CALLBACK_DATA                         *CallbackData;
  RESULT_ARRAY                          *ResultArray;
  UINTN                                 Offset;
  UINT8                                 DeviceKind;
  BOOLEAN                               Result;
  EFI_STATUS                            Status;
  VARIABLE_EDIT_PTR                     *Point  = NULL;
  UINTN                                 MemIndex  = 0;
  UINT32                                TempRegister;
  

  POST_CODE (0x5A);

  //
  //  Try to find current CPU and ckeck EBX
  //
  CheckEBX = 0;
  for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    TempRegister = 0;
    mSmmCpu->ReadSaveState (mSmmCpu, sizeof (UINT32), EFI_SMM_SAVE_STATE_REGISTER_RBX, Index, &TempRegister);

    if (TempRegister == IVE_EBX_SIGNATURE) {
      CheckEBX = 1;
      break;
    }
  }

  if (CheckEBX == 0) {
    mSmmCpu->ReadSaveState (mSmmCpu, sizeof (UINT32), EFI_SMM_SAVE_STATE_REGISTER_RAX, Index, &TempRegister);
    TempRegister = (UINT32)((TempRegister & 0xffffff00) | 0x01);
    mSmmCpu->WriteSaveState (mSmmCpu, sizeof (UINT32), EFI_SMM_SAVE_STATE_REGISTER_RAX, Index, &TempRegister);
    return EFI_ABORTED;
  }

  mSmmCpu->ReadSaveState (mSmmCpu, sizeof (UINT32), EFI_SMM_SAVE_STATE_REGISTER_RSI, Index, &TempRegister);
  SetupNVData  = (CHIPSET_CONFIGURATION *)(UINTN)TempRegister;
  mSmmCpu->ReadSaveState (mSmmCpu, sizeof (UINT32), EFI_SMM_SAVE_STATE_REGISTER_RDI, Index, &TempRegister);
  CallbackList = (CALLBACK_LIST *)(UINTN)TempRegister;
  if (SetupNVData == NULL || CallbackList == NULL) {
    return EFI_ABORTED;
  }
  Offset       = (UINTN) (&CallbackList->List);
  mSmmCpu->ReadSaveState (mSmmCpu, sizeof (UINT32), EFI_SMM_SAVE_STATE_REGISTER_RCX, Index, &TempRegister);
  ResultArray  = (RESULT_ARRAY *)(UINTN)TempRegister;
  if (ResultArray != NULL) {
    ResultArray->ArrayCount = 0;
  }

  POST_CODE (0x5B);
  for (Index = 0; Index < CallbackList->ListCount; Index++) {
    CallbackData = (CALLBACK_DATA *) Offset;
    Result = FALSE;

    switch (CallbackData->CallbackID) {

      //
      // Advance Callback Handle
      //
    case KEY_AHCI_OPROM_CONFIG:
      if (SetupNVData->SataCnfigure == AHCI_MODE || SetupNVData->SataCnfigure == RAID_MODE) {
        SetupNVData->AhciOptionRomSupport = TRUE;
      } else {
        SetupNVData->AhciOptionRomSupport = FALSE;
      }
      Result = TRUE;
      break;

    case KEY_SERIAL_PORTA:
    case KEY_SERIAL_PORTA_BASE_IO:
    case KEY_SERIAL_PORTA_INTERRUPT:
    case KEY_SERIAL_PORTB:
    case KEY_SERIAL_PORTB_BASE_IO:
    case KEY_SERIAL_PORTB_INTERRUPT:
      //
      //  Check IRQ conflict between serial port and pci solt.
      //
      if (CheckSioAndPciSoltConflict (SetupNVData, (UINT8 *) & (SetupNVData->PciSlot3), &DeviceKind)) {
        switch (DeviceKind) {
        case 0:
          SetupNVData->ComPortA = 0;
          break;

        case 1:
          SetupNVData->ComPortB = 0;
          break;
        }
      }
      if (CheckSioConflict (SetupNVData)) {
      }
      Result = TRUE;
      break;

    case KEY_SAVE_EXIT:
    case KEY_SAVE_WITHOUT_EXIT:
      POST_CODE ( (UINT8) CallbackData->CallbackID);
      Status = WriteNVSetup (SetupNVData);
      Result = EFI_ERROR (Status) ? FALSE : TRUE;
      break;

    case KEY_EXIT_DISCARD:
    case KEY_DISCARD_CHANGE:
      POST_CODE ( (UINT8) CallbackData->CallbackID);
      Status = ReadNVSetup (SetupNVData);
      Result = EFI_ERROR (Status) ? FALSE : TRUE;
      break;

    case KEY_SAVE_CUSTOM:
      POST_CODE ( (UINT8) CallbackData->CallbackID);
      Status = WriteNVCustom (SetupNVData);
      Result = EFI_ERROR (Status) ? FALSE : TRUE;
      break;

    case KEY_LOAD_OPTIMAL:
      POST_CODE ( (UINT8) CallbackData->CallbackID);
      for (MemIndex = 0xE0000; MemIndex < 0x100000; MemIndex += VARIABLE_EDIT_PTR_ALIGNMENT) {
        Point = (VARIABLE_EDIT_PTR *) MemIndex;
        if (IS_VARIABLE_EDIT_PTR (Point)) {
          break;
        }
      }

      if (MemIndex >= 0x100000) {
        Result = FALSE;
        break;
      }

      MemIndex = Point->Address;
      MemIndex += Point->Size;
//[-start-130709-IB05160465-modify]//
      MemIndex -= PcdGet32 (PcdSetupConfigSize);
      CopyMem (SetupNVData, (VOID *) MemIndex, PcdGet32 (PcdSetupConfigSize));
//[-end-130709-IB05160465-modify]//
      Result = TRUE;
      break;

    case KEY_LOAD_CUSTOM:
      //
      // Load Custom By AP(caller)
      //
      Status = gSmst->SmmAllocatePool (
                 EfiRuntimeServicesData,
//[-start-130709-IB05160465-modify]//
                 PcdGet32 (PcdSetupConfigSize),
//[-end-130709-IB05160465-modify]//
                 (void **) &OldSetupNVData
               );
      if (EFI_ERROR (Status)) {
        Result = FALSE;
        break;
      }

      //
      //  Step 1. Load Custom to Temp Buffer
      //
      Status = ReadNVCustom (OldSetupNVData);
      if (EFI_ERROR (Status)) {
        Result = FALSE;
        break;
      }

      //
      //  Step 2. Backup now password flag to Temp Buffer
      //
      //
      //  Update the HDD Password status is whther Exit
      //
      OldSetupNVData->SetUserPass                   = SetupNVData->SetUserPass;
      OldSetupNVData->UserAccessLevel               = SetupNVData->UserAccessLevel;
      OldSetupNVData->SupervisorFlag                = SetupNVData->SupervisorFlag;
      OldSetupNVData->UserFlag                      = SetupNVData->UserFlag;
      OldSetupNVData->SetAllHddPasswordFlag         = SetupNVData->SetAllHddPasswordFlag;
      OldSetupNVData->UseMasterPassword             = SetupNVData->UseMasterPassword;
      OldSetupNVData->C1MsSetHddPasswordFlag        = SetupNVData->C1MsSetHddPasswordFlag;
      OldSetupNVData->C1SlSetHddPasswordFlag        = SetupNVData->C1SlSetHddPasswordFlag;
      OldSetupNVData->C2MsSetHddPasswordFlag        = SetupNVData->C2MsSetHddPasswordFlag;
      OldSetupNVData->C2SlSetHddPasswordFlag        = SetupNVData->C2SlSetHddPasswordFlag;
      OldSetupNVData->C3MsSetHddPasswordFlag        = SetupNVData->C3MsSetHddPasswordFlag;
      OldSetupNVData->C3SlSetHddPasswordFlag        = SetupNVData->C3SlSetHddPasswordFlag;
      OldSetupNVData->C4MsSetHddPasswordFlag        = SetupNVData->C4MsSetHddPasswordFlag;
      OldSetupNVData->C4SlSetHddPasswordFlag        = SetupNVData->C4SlSetHddPasswordFlag;
      OldSetupNVData->C1MsCBMSetHddPasswordFlag     = SetupNVData->C1MsCBMSetHddPasswordFlag;
      OldSetupNVData->C1SlCBMSetHddPasswordFlag     = SetupNVData->C1SlCBMSetHddPasswordFlag;
      OldSetupNVData->C2MsCBMSetHddPasswordFlag     = SetupNVData->C2MsCBMSetHddPasswordFlag;
      OldSetupNVData->C2SlCBMSetHddPasswordFlag     = SetupNVData->C2SlCBMSetHddPasswordFlag;
      OldSetupNVData->C3MsCBMSetHddPasswordFlag     = SetupNVData->C3MsCBMSetHddPasswordFlag;
      OldSetupNVData->C3SlCBMSetHddPasswordFlag     = SetupNVData->C3SlCBMSetHddPasswordFlag;
      OldSetupNVData->C4MsCBMSetHddPasswordFlag     = SetupNVData->C4MsCBMSetHddPasswordFlag;
      OldSetupNVData->C4SlCBMSetHddPasswordFlag     = SetupNVData->C4SlCBMSetHddPasswordFlag;
      OldSetupNVData->C1MsUnlockHddPasswordFlag     = SetupNVData->C1MsUnlockHddPasswordFlag;
      OldSetupNVData->C1SlUnlockHddPasswordFlag     = SetupNVData->C1SlUnlockHddPasswordFlag;
      OldSetupNVData->C2MsUnlockHddPasswordFlag     = SetupNVData->C2MsUnlockHddPasswordFlag;
      OldSetupNVData->C2SlUnlockHddPasswordFlag     = SetupNVData->C2SlUnlockHddPasswordFlag;
      OldSetupNVData->C3MsUnlockHddPasswordFlag     = SetupNVData->C3MsUnlockHddPasswordFlag;
      OldSetupNVData->C3SlUnlockHddPasswordFlag     = SetupNVData->C3SlUnlockHddPasswordFlag;
      OldSetupNVData->C4MsUnlockHddPasswordFlag     = SetupNVData->C4MsUnlockHddPasswordFlag;
      OldSetupNVData->C4SlUnlockHddPasswordFlag     = SetupNVData->C4SlUnlockHddPasswordFlag;
      OldSetupNVData->C1MsCBMUnlockHddPasswordFlag  = SetupNVData->C1MsCBMUnlockHddPasswordFlag;
      OldSetupNVData->C1SlCBMUnlockHddPasswordFlag  = SetupNVData->C1SlCBMUnlockHddPasswordFlag;
      OldSetupNVData->C2MsCBMUnlockHddPasswordFlag  = SetupNVData->C2MsCBMUnlockHddPasswordFlag;
      OldSetupNVData->C2SlCBMUnlockHddPasswordFlag  = SetupNVData->C2SlCBMUnlockHddPasswordFlag;
      OldSetupNVData->C3MsCBMUnlockHddPasswordFlag  = SetupNVData->C3MsCBMUnlockHddPasswordFlag;
      OldSetupNVData->C3SlCBMUnlockHddPasswordFlag  = SetupNVData->C3SlCBMUnlockHddPasswordFlag;
      OldSetupNVData->C4MsCBMUnlockHddPasswordFlag  = SetupNVData->C4MsCBMUnlockHddPasswordFlag;
      OldSetupNVData->C4SlCBMUnlockHddPasswordFlag  = SetupNVData->C4SlCBMUnlockHddPasswordFlag;
      OldSetupNVData->TpmDeviceOk                   = SetupNVData->TpmDeviceOk;

      //
      //  Step 3. Copy Temp Buffer to now Buffer
      //
//[-start-130709-IB05160465-modify]//
      CopyMem (SetupNVData, OldSetupNVData, PcdGet32 (PcdSetupConfigSize));
//[-end-130709-IB05160465-modify]//
      Status = gSmst->SmmFreePool (OldSetupNVData);
      Result = TRUE;
      break;


      //
      // Main Callback Routine
      //
    case KEY_LANGUAGE_UPDATE:
      break;

      //
      // Server Callback Routine
      //
    default:
      break;
    }

    if (ResultArray != NULL) {
      ResultArray->ArrayCount += 1;
      ResultArray->Array[Index].CallbackID = CallbackData->CallbackID;
      ResultArray->Array[Index].Result     = Result;
    }

    //
    // Move to next callback data
    //
    Offset = Offset + sizeof (CALLBACK_DATA) + AsciiStrLength ((UINT8 *)CallbackData->StrData);

  }

  return EFI_SUCCESS;

}

