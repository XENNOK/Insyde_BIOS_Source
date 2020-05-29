/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

    Platform.c

Abstract:

    This is a generic template for a child of the IchSmm driver.

Revision History

--*/
#include <Platform.h>
//[-start-121214-IB10820195-remove]//
//#include <PlatformBaseAddrDef.h>
//[-end-121214-IB10820195-remove]//
#include <PchAccess.h>
#include <SaAccess.h>
#include <SmiTable.h>
#include <ChipsetSmiTable.h>
//[-start-121108-IB05280008-remove]//
//#include <OemServices/Kernel.h>
//[-end-121108-IB05280008-remove]//
//[-start-121108-IB05280008-add]//
#include <Library/SmmOemSvcKernelLib.h>
//[-end-121108-IB05280008-add]//
#include <SmmPassword.h>
#include <ChipsetCmos.h>

//[-start-130812-IB06720232-modify]//
//#include <Library/EcLib.h>
#include <Library/BaseOemSvcChipsetLib.h>
//[-end-130812-IB06720232-modify]//
#include <Library/S3BootScriptLib.h>

#include <Protocol/ProgramSsidSvid.h>
#include <Protocol/AcpiEnableCallbackDone.h>
#include <Protocol/AcpiRestoreCallbackDone.h>
#include <Protocol/AcpiDisableCallbackDone.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/SmmMERegion.h>
#include <Protocol/SmmOemServices.h>
#include <Protocol/SmmCpuSaveState.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/OverrideAspm.h>
#include <Protocol/CpuIo2.h>
//[-start-120508-IB03780437-add]//
#include <Guid/GlobalVariable.h>
//[-end-120508-IB03780437-add]//
//[-start-121105-IB03780470-add]//
#include <PchPlatformLib.h>
//[-end-121105-IB03780470-add]//
//[-start-130307-IB03780481-modify]//
//[-start-130124-IB04770265-add]//
#include <Library/TbtLib.h>
#include <OemThunderbolt.h>
//[-end-130124-IB04770265-add]//
//[-end-130307-IB03780481-modify]//
#include <Library/ResetSystemLib.h>

//
// Global variables
//

EFI_SMM_VARIABLE_PROTOCOL               *mSmmVariable;
EFI_SMM_SYSTEM_TABLE2                   *mSmst;
UINT16                                  mAcpiBaseAddr;
EFI_SMM_THUNK_PROTOCOL              *mSmmThunk;

//
// Use an enum. 0 is S0, 1 is S5, 2 is Last State
//
UINT8                                   mStateAfterG3;
// Setup Variable
CHIPSET_CONFIGURATION                    *SetupVariable;
EFI_GLOBAL_NVS_AREA                     *mGlobalNvsArea;
//[-start-121108-IB05280008-remove]//
//SMM_OEM_SERVICES_PROTOCOL               *mSmmOemServices;
//[-end-121108-IB05280008-remove]//
SMM_ME_REGION_PROTOCOL                  mSmmMERegionProtocol;


EFI_SMM_CPU_SAVE_STATE_PROTOCOL         *mSmmCpuSavedStateProtocol;

//[-start-130619-IB05160459-modify]//
//[-start-120816-IB05300312-remove]//
static
UINT8 mTco1Sources[] = {
  IchnMch,
  IchnY2KRollover,
  IchnTcoTimeout,
  IchnOsTco,
  IchnNmi
};
//[-end-120816-IB05300312-remove]//
//[-end-130619-IB05160459-modify]//

UINT32 *mParam = NULL;

EFI_STATUS
EFIAPI
Int15HookCallBack (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
  )
{
  EFI_DWORD_REGS            *RegBuf = NULL;
  UINT32                    *CodeStart = NULL;
  UINT32                    Int15FunNum = 0;
  UINTN                     Index;

  //
  // Find out which CPU triggered the S/W SMI
  //
  for (Index = 0; Index < mSmst->NumberOfCpus; Index++) {
    //
    // Check parameter condition
    //
    //
    // Get the pointer to saved CPU regs.
    //

    if ((mSmmCpuSavedStateProtocol->CpuSaveState[Index]->x64._RAX & 0xff) == 0x15 &&
        (mSmmCpuSavedStateProtocol->CpuSaveState[Index]->x64._RDX & 0xffff) == 0xb2) {

      if (mParam == NULL) {
        mParam = (UINT32*)(UINTN)((mSmmCpuSavedStateProtocol->CpuSaveState[Index]->x64._CS << 4) + (mSmmCpuSavedStateProtocol->CpuSaveState[Index]->x64._RDI & 0xffff));
      }

      RegBuf = (VOID*)(UINTN)(mParam + 1);
      CodeStart = (VOID*)(UINTN)mParam[0];
      Int15FunNum = (RegBuf->EAX & 0xFFFF);
      //
      // CPU found!
      //
      break;
    }
  }

  if (!Int15FunNum) {
    return EFI_SUCCESS;
  }

  switch (Int15FunNum) {
    //
    //to do 
    //
    default:
      break;

    
  }
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
OsResetSmi (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
)
{
//[-start-121109-IB05280008-add]//
  EFI_STATUS                                Status;
//[-end-121109-IB05280008-add]//

//[-start-121108-IB05280008-remove]//
////[-start-121018-IB10820140-modify]//
////[-start-111111-IB08130020-add]//
//  mSmmOemServices->Funcs[COMMON_SMM_OS_RESET_CALLBACK] (
//                     mSmmOemServices,
//                     COMMON_SMM_OS_RESET_CALLBACK_ARG_COUNT
//                     );
////[-start-120601-IB10620003-remove]//
////  ASSERT_EFI_ERROR (Status);
////[-end-120601-IB10620003-remove]//
////[-end-111111-IB08130020-add]//
////[-end-121018-IB10820140-modify]//
//[-end-121108-IB05280008-remove]//
//[-start-121108-IB05280008-add]//
  Status = OemSvcOsResetCallback ();
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcKernelLib OsResetCallback, Status : %r\n", Status ) );
  if ( Status == EFI_SUCCESS ) {
    return EFI_SUCCESS;
  }
//[-end-121108-IB05280008-add]//
  //
  // Avoid slave harddisk could not attach problem
  // need to set reset command in IDE controller.
  // But it need to take time.
  //
  do{
    ResetCold ();
  }while(1);
}


/**
 Initializes the SMM Platfrom Driver

 @param [in]   ImageHandle      Pointer to the loaded image protocol for this driver
 @param [in]   SystemTable      Pointer to the EFI System Table

 @retval Status                 EFI_SUCCESS
 @return Assert, otherwise.

**/
EFI_STATUS
EFIAPI
InitializePlatformSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  BOOLEAN                                   InSmm;
  EFI_SMM_BASE2_PROTOCOL                    *SmmBase;
//[-start-130619-IB05160459-modify]//
//[-start-120816-IB05300312-remove]//
  UINT8                                   Index;
//[-end-120816-IB05300312-remove]//
//[-end-130619-IB05160459-modify]//
  EFI_HANDLE                                PowerButtonHandle;
  EFI_HANDLE                                SwHandle;
  EFI_HANDLE                                IchnHandle;
  EFI_HANDLE                                WakeOnTimeHandle;
  EFI_HANDLE                                WakeOnLanHandle;
  EFI_HANDLE                                WakeOnAcLossHandle;
  EFI_SMM_SX_DISPATCH_CONTEXT               EntryDispatchContext;
  EFI_EVENT                                 ReadyToBootEvent;

  EFI_SMM_POWER_BUTTON_DISPATCH_PROTOCOL    *PowerButtonDispatch;
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch2;
  EFI_SMM_ICHN_DISPATCH_PROTOCOL            *IchnDispatch;
  EFI_SMM_SX_DISPATCH_PROTOCOL              *SxDispatch;

  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT     PowerButtonContext;
  EFI_SMM_SW_REGISTER_CONTEXT               SwContext2;
  EFI_SMM_ICHN_DISPATCH_CONTEXT             IchnContext;
  EFI_SETUP_UTILITY_PROTOCOL                *EfiSetupUtility;
  EFI_GLOBAL_NVS_AREA_PROTOCOL              *GlobalNvsAreaProtocol;
  UINT16                                    Pm1EnData;
  VOID                                      *AcpiEnableCallbackStartReg;
  VOID                                      *AcpiDisableAcpiCallbackStartReg;
  VOID                                      *AcpiRestoreAcpiCallbackStartReg;
  VOID                                      *AcpiRestoreAcpiCallbackDoneReg;
  EFI_SMM_ICHN_DISPATCH_EX_PROTOCOL         *IchnExDispatch;
  EFI_HANDLE                                IchnExHandle;
  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT          IchnExContext;
  UINT32                                    SmiEn;
  UINTN                                     Size;
  UINT8                                     BootType;
//[-start-130809-IB06720232-add]//
  BOOLEAN                                   PowerStateIsAc;
//[-end-130809-IB06720232-add]//

  POST_CODE (SMM_SMM_PLATFORM_INIT); //PostCode = 0xA2, SMM service initial
  PowerButtonHandle   = NULL;
  SwHandle            = NULL;
  IchnHandle          = NULL;
  WakeOnTimeHandle    = NULL;
  WakeOnLanHandle     = NULL;
  WakeOnAcLossHandle  = NULL;
  IchnExHandle        = NULL;
  SmmBase             = NULL;
//[-start-130809-IB06720232-add]//
  PowerStateIsAc      = FALSE;
//[-end-130809-IB06720232-add]//

  Status = gBS->LocateProtocol (&gEfiSmmBase2ProtocolGuid, NULL, (VOID **)&SmmBase);
  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }

  if (!InSmm) {
    //
    // Signal the function to lock SMM when ready to boot
    //
    Status = EfiCreateEventReadyToBootEx (
              TPL_CALLBACK,
               SmmReadyToBootFunction,
               NULL,
               &ReadyToBootEvent
               );
    ASSERT_EFI_ERROR (Status);

    //
    // This driver is dispatched by DXE, so the first call to this driver will not in SMM.
    // We need to load this driver into SMRAM and then generate
    // an SMI to initialize data structures in SMRAM.
    //

    //
    // Get this driver's image's FilePath
    return EFI_SUCCESS;
  }


  Status = SmmBase->GetSmstLocation (SmmBase, &mSmst);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
      return Status;
  }

  Status = mSmst->SmmLocateProtocol (&gEfiSmmVariableProtocolGuid, NULL, (VOID **)&mSmmVariable);
  if (EFI_ERROR (Status)) {
    return Status;
  }


//[-start-121108-IB05280008-remove]//
//  Status = mSmst->SmmLocateProtocol (
//                        &gSmmOemServicesProtocolGuid,
//                        NULL,
//                        &mSmmOemServices
//                        );
//  if (EFI_ERROR(Status)) {
//    return Status;
//  }
//[-end-121108-IB05280008-remove]//


  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&EfiSetupUtility);

  //
  // Find Setup Variable
  //
  SetupVariable = (CHIPSET_CONFIGURATION *)EfiSetupUtility->SetupNvData;

  //
  // Get Global NV store Address
  //
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  mGlobalNvsArea = GlobalNvsAreaProtocol->Area;

  //
  // Get the ACPI Base Address
  //
  mAcpiBaseAddr = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & B_PCH_LPC_ACPI_BASE_BAR;

  //
  // Locate the PCH SMM Power Button dispatch protocol
  //
//[-start-121009-IB05280002-modify]//
  Status = gBS->LocateProtocol (&gEfiSmmPowerButtonDispatchProtocolGuid, NULL, (VOID **)&PowerButtonDispatch);
//[-end-121009-IB05280002-modify]//
  ASSERT_EFI_ERROR (Status);

  //
  // Register the power button SMM event
  //
  PowerButtonContext.Phase = PowerButtonEntry;
//[-start-121109-IB05280008-modify]//
  Status = PowerButtonDispatch->Register (
    PowerButtonDispatch,
    ChipsetPowerButtonCallback,
    &PowerButtonContext,
    &PowerButtonHandle
    );
//[-end-121109-IB05280008-modify]//
  ASSERT_EFI_ERROR (Status);

  if (SetupVariable->GpioLockdown) {
    //
    // Locate SMM ICHn SMI Dispatch Extended Protocol and register the callback function to
    // IchnExGpioUnlock to set GPIO_UNLOCK_SMI_EN bit.
    //
    // Locate SmmIchnDispatch Extended Protocol
    //
//[-start-121009-IB05280002-modify]//
    Status = gBS->LocateProtocol (&gEfiSmmIchnDispatchExProtocolGuid, NULL, (VOID **)&IchnExDispatch);
//[-end-121009-IB05280002-modify]//
    ASSERT_EFI_ERROR (Status);

    //
    // Register GpioUnlock event, and set B_PCH_SMI_EN_GPIO_UNLOCK_SMI
    //
    IchnExContext.Type = IchnExGpioUnlock;
    Status = IchnExDispatch->Register (
      IchnExDispatch,
      GpioUnlockCallback,
      &IchnExContext,
      &IchnExHandle
      );
    ASSERT_EFI_ERROR (Status);

    //
    // Enable GPIO_UNLOCK_SMI_EN.
    // When clear B_PCH_LPC_GPIO_LOCKDOWN_EN, will trigger SMI.
    //
    SmiEn = IoRead32 (mAcpiBaseAddr + R_PCH_SMI_EN);
    SmiEn |= B_PCH_SMI_EN_GPIO_UNLOCK_SMI;
    IoWrite32 ((mAcpiBaseAddr + R_PCH_SMI_EN), SmiEn);
  }
  //
  //  Locate the PCH SMM SW dispatch 2 protocol
  //
  Status = mSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&SwDispatch2);
  ASSERT_EFI_ERROR (Status);

  //
  //Register the OS restart set features disable
  //
  SwContext2.SwSmiInputValue = SMM_OS_RESET_SMI_VALUE;
  Status = SwDispatch2->Register (
    SwDispatch2,
    OsResetSmi,
    &SwContext2,
    &SwHandle
    );
  ASSERT_EFI_ERROR (Status);

  //
  // Register ACPI enable handler
  //
  Status = mSmst->SmmRegisterProtocolNotify (
              &gAcpiEnableCallbackStartProtocolGuid,
              PlatformEnableAcpiCallbackStart,
              &AcpiEnableCallbackStartReg
              );
  ASSERT_EFI_ERROR (Status);

  //
  // Register ACPI disable handler
  //
  Status = mSmst->SmmRegisterProtocolNotify (
              &gAcpiDisableCallbackStartProtocolGuid,
              PlatformDisableAcpiCallbackStart,
              &AcpiDisableAcpiCallbackStartReg
              );
  ASSERT_EFI_ERROR (Status);

  //
  // Register ACPI restore handler
  //
  Status = mSmst->SmmRegisterProtocolNotify (
              &gAcpiRestoreCallbackStartProtocolGuid,
              PlatformRestoreAcpiCallbackStart,
              &AcpiRestoreAcpiCallbackStartReg
              );
  ASSERT_EFI_ERROR (Status);
  
  Status = mSmst->SmmRegisterProtocolNotify (
              &gAcpiRestoreCallbackDoneProtocolGuid,
              PlatformRestoreAcpiCallbackDone,
              &AcpiRestoreAcpiCallbackDoneReg
              );
  ASSERT_EFI_ERROR (Status);
  
    Status = gBS->LocateProtocol (
                          &gEfiSmmCpuSaveStateProtocolGuid,
                          NULL,
                          (VOID **)&mSmmCpuSavedStateProtocol
                          );
    ASSERT_EFI_ERROR (Status);


    SwContext2.SwSmiInputValue = INT15_HOOK;
    Status = SwDispatch2->Register (
      SwDispatch2,
      Int15HookCallBack,
      &SwContext2,
      &SwHandle
      );
  ASSERT_EFI_ERROR( Status );

   //
   // SMI for iSCT to record RTC data.
   //
    SwContext2.SwSmiInputValue = ISCT_RTC_SW_SMI;
    Status = SwDispatch2->Register (
      SwDispatch2,
      GetAlarmRtcTime,
      &SwContext2,
      &SwHandle
      );
  ASSERT_EFI_ERROR( Status );

  BootType = DUAL_BOOT_TYPE;
  Size = sizeof (UINT8);
  SystemTable->RuntimeServices->GetVariable (
                                  L"BootType",
                                  &gSystemConfigurationGuid,
                                  NULL,
                                  &Size,
                                  &BootType
                                  );

//[-start-121120-IB09890039-remove]//
//  //
//  // Register OEM call SMI function
//  //
//[-end-121120-IB09890039-remove]//

//[-start-120323-IB03600473-modify]//
  if (BootType != EFI_BOOT_TYPE)
  {
//[-start-121120-IB09890039-remove]//
//
//    SwContext2.SwSmiInputValue = EFI_OEM_FUNCTION_SW_SMI;
//    Status = SwDispatch2->Register (
//                            SwDispatch2,
//                            OemCallback,
//                            &SwContext2,
//                            &SwHandle );
//    ASSERT_EFI_ERROR( Status );
//
//[-end-120816-IB09890039-remove]//
  //
  // Locate the SMM thunk protocol used by the ACPI reference code
  //
//[-start-120731-IB10820094-modify]//
    if (FeaturePcdGet(PcdSmmInt10Enable)) {
      Status = gBS->LocateProtocol (&gEfiSmmThunkProtocolGuid, NULL, (VOID **)&mSmmThunk);
      ASSERT_EFI_ERROR (Status);
    }
//[-end-120731-IB10820094-modify]//
  }
//[-end-120323-IB03600473-modify]//
  //
  // Register SMBIOS call SMI function
  //

  //
  // Get the PCH protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmmIchnDispatchProtocolGuid, NULL, (VOID **)&IchnDispatch);
  ASSERT_EFI_ERROR (Status);

  //
  // Register for the PME event.
  //
  if (SetupVariable->WakeOnPME) {
    IchnContext.Type = IchnPme;
    Status = IchnDispatch->Register(
                              IchnDispatch,
                              PmeCallback,
                              &IchnContext,
                              &IchnHandle
                              );
    ASSERT_EFI_ERROR( Status );
  }

  //
  // In Windows 7, Pci-E Wake will set by FACP flag
  // but in Dos, system still can wake up from S5 by Pci-E Lan card, set below to follow SCU setting
  //
  if (!SetupVariable->WakeOnPME) {
    Pm1EnData = IoRead16 (mAcpiBaseAddr + R_PCH_ACPI_PM1_EN);
    Pm1EnData |= BIT14;  //B_PCH_ACPI_PM1_EN_PCIEXPWAK_DIS; ==> PCI Express* Wake Disable(PCIEXPWAK_DIS) 
    IoWrite16 (mAcpiBaseAddr + R_PCH_ACPI_PM1_EN, Pm1EnData);
  }

//[-start-130619-IB05160459-modify]//
//[-start-120816-IB05300312-remove]//
  for (Index = 0; Index < sizeof(mTco1Sources)/sizeof(UINT8); Index++) {
      IchnContext.Type = mTco1Sources[Index];
      Status = IchnDispatch->Register(
                                IchnDispatch,
                                DummyTco1Callback,
                                &IchnContext,
                                &IchnHandle
                                );
        ASSERT_EFI_ERROR( Status );
  }
//[-end-120816-IB05300312-remove]//
//[-end-130619-IB05160459-modify]//

  //
  // Get the Sx dispatch protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmmSxDispatchProtocolGuid,
                                NULL,
                                (VOID **)&SxDispatch
                                );
  ASSERT_EFI_ERROR(Status);

  EntryDispatchContext.Type  = SxS1;
  EntryDispatchContext.Phase = SxEntry;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S1SleepEntryCallBack,
                         &EntryDispatchContext,
                         &WakeOnAcLossHandle
                         );
  ASSERT_EFI_ERROR(Status);

  EntryDispatchContext.Type  = SxS3;
  EntryDispatchContext.Phase = SxEntry;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S3SleepEntryCallBack,
                         &EntryDispatchContext,
                         &WakeOnAcLossHandle
                         );
  ASSERT_EFI_ERROR(Status);

  EntryDispatchContext.Type  = SxS4;
  EntryDispatchContext.Phase = SxEntry;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S4SleepEntryCallBack,
                         &EntryDispatchContext,
                         &WakeOnAcLossHandle
                         );
  ASSERT_EFI_ERROR(Status);

  EntryDispatchContext.Type  = SxS5;
  EntryDispatchContext.Phase = SxEntry;
  Status = SxDispatch->Register (
                         SxDispatch,
                         S5SleepEntryCallBack,
                         &EntryDispatchContext,
                         &WakeOnAcLossHandle
                         );
  ASSERT_EFI_ERROR(Status);

  Status = SxDispatch->Register (
                         SxDispatch,
                         S5SleepAcLossCallBack,
                         &EntryDispatchContext,
                         &WakeOnAcLossHandle
                         );
  ASSERT_EFI_ERROR(Status);

  //
  // Register entry phase call back function.
  //
  if (SetupVariable->WakeOnModemRing) {
    Status = SxDispatch->Register (
                           SxDispatch,
                           S5SleepWakeOnLanCallBack,
                           &EntryDispatchContext,
                           &WakeOnLanHandle
                           );
    ASSERT_EFI_ERROR(Status);
  }

  if (SetupVariable->WakeOnS5) {
    Status = SxDispatch->Register (
                           SxDispatch,
                           S5SleepWakeOnTimeCallBack,
                           &EntryDispatchContext,
                           &WakeOnTimeHandle
                           );
    ASSERT_EFI_ERROR(Status);
  }

//[-start-130809-IB06720232-modify]//
  OemSvcEcPowerState (&PowerStateIsAc);

  //
  // Check the power state from EC and update it to global NVS area
  //
  if (PowerStateIsAc) {
    mGlobalNvsArea->PowerState = 1;
  } else {
    mGlobalNvsArea->PowerState = 0;
  }
//[-end-130809-IB06720232-modify]//
  //
  // Set state after G3 depend on StateAfterG3 variable
  //
  mStateAfterG3 = SetupVariable->StateAfterG3;
  switch (mStateAfterG3) {

  case S5:
    SetAfterG3On (FALSE);
    break;

  case S0:
  case LastState:
  default:
    SetAfterG3On (TRUE);
    break;
  }

  return EFI_SUCCESS;
}

/**
 Set the system return to S0 or S5 state after power is re-applied.

 @param        Enable           System On/Off

 @retval None.

**/
VOID
SetAfterG3On (
  BOOLEAN   Enable
  )
{
  UINT8   PmCon3;

  PmCon3 = PchLpcPciCfg8 (R_PCH_LPC_GEN_PMCON_3) & ~B_PCH_LPC_GEN_PMCON_AFTERG3_EN;

  if (!Enable) {
    PmCon3 |= B_PCH_LPC_GEN_PMCON_AFTERG3_EN;
  }
  PchLpcPciCfg8AndThenOr (R_PCH_LPC_GEN_PMCON_3, 0, PmCon3);
}

/**
 When a power button event happens, it shuts off the machine

 @param [in]   DispatchHandle   Handle of this dispatch function
 @param [in]   DispatchContext  Pointer to the dispatch function's context

 @retval None.

**/
//[-start-121109-IB05280008-modify]//
VOID
EFIAPI
ChipsetPowerButtonCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT   *DispatchContext
  )
//[-end-121109-IB05280008-modify]//
{
  UINT32  Buffer;
//[-start-121109-IB05280008-add]//
  EFI_STATUS                                Status;
//[-end-121109-IB05280008-add]//

  //
  // Check what the state to return to after AC Loss.
  // If Last State, then set it to Off.
  //
  if (mStateAfterG3 == LastState) {
    SetAfterG3On (FALSE);
  }
  if (SetupVariable->WakeOnS5) {
    S5WakeUpSetting (mAcpiBaseAddr, SetupVariable);
  }
//[-start-121108-IB05280008-modify]//
//  mSmmOemServices->Funcs[COMMON_SMM_POWER_BUTTON_CALLBACK](mSmmOemServices, 0); 
  Status = OemSvcPowerButtonCallback ();
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcKernelLib PowerButtonCallback, Status : %r\n", Status ) );
  if ( Status == EFI_SUCCESS ) {
    return;
  }
//[-end-121108-IB05280008-modify]//
  //
  // Clear Sleep Type Enable
  //
  IoWrite32 (
    mAcpiBaseAddr + R_PCH_SMI_EN,
    (UINT32) (IoRead32 (mAcpiBaseAddr + R_PCH_SMI_EN) & ~B_PCH_SMI_EN_ON_SLP_EN)
    );

  //
  // Clear Power Button Status
  //
  IoWrite16 (
    mAcpiBaseAddr + R_PCH_ACPI_PM1_STS,
    (UINT16) (IoRead16 (mAcpiBaseAddr + R_PCH_ACPI_PM1_STS) | B_PCH_ACPI_PM1_STS_PWRBTN)
    );

  //
  // Shut it off now
  //
  Buffer = IoRead32 (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT) & ~(B_PCH_ACPI_PM1_CNT_SLP_EN | B_PCH_ACPI_PM1_CNT_SLP_TYP);
  Buffer |= V_PCH_ACPI_PM1_CNT_S5;
  IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, Buffer);
  Buffer |= B_PCH_ACPI_PM1_CNT_SLP_EN;
  IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, Buffer);


}

/**
 When a GPIO Unlock event happens, program GPIO for runtime use.
 After programming GPIO, lockdown GPIO registers.

 @param [in]   DispatchHandle   Handle of this dispatch function
 @param [in]   DispatchExContext  Pointer to the dispatch function's context

 @retval None.

**/
VOID
EFIAPI
GpioUnlockCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_EX_CONTEXT        *DispatchExContext
  )
{
  UINT32   GpioData;
  UINT8    GpioOffset;

//[-start-120404-IB05300309-modify]//
  GpioData = McD0PciCfg32 (R_SA_MC_SKPD_OFFSET);
//[-end-120404-IB05300309-modify]//
  
  GpioOffset = IoRead8 (R_PCH_APM_STS);
  IoWrite8 (R_PCH_APM_STS, 0x7F);

  if (GpioOffset == 0xFF)
    //
    // Valid driver should program and re-lockdown by themself.
    //
    return;

  if (GpioData == 0xFF000000) {
    //
    // Invalid programing, keeps GPIO lockdown
    //
    PchLpcPciCfg8Or (R_PCH_LPC_GPIO_CNT, B_PCH_LPC_GPIO_LOCKDOWN_EN);
    return;
  }
  
  //
  // Program GPIO
  //
//[-start-120731-IB10820094-modify]//
  IoWrite32 (PcdGet16(PcdPchGpioBaseAddress) + GpioOffset, GpioData);
//[-end-120731-IB10820094-modify]//

//[-start-120404-IB05300309-modify]//
  //
  // After using MCH scratchpad, restore MrcCompleteSignature value.
  //
  McD0PciCfg32AndThenOr(R_SA_MC_SKPD_OFFSET, 0, 0xFF000000);
//[-end-120404-IB05300309-modify]//

  //
  // Enable GPIO Lockdown
  //
  PchLpcPciCfg8Or (R_PCH_LPC_GPIO_CNT, B_PCH_LPC_GPIO_LOCKDOWN_EN);
}

/**

 @param [in]   DispatchHandle
 @param [in]   DispatchContext

 @retval None

**/
VOID
EFIAPI
PmeCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT           *DispatchContext
  )
{
  return;
}

/**

 @param [in]   DispatchHandle   The handle of this callback, obtained when registering
 @param [in]   DispatchContext  The predefined context which contained sleep type and phase

**/
VOID
EFIAPI
S5SleepAcLossCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{

  //
  // Fixed Suse 9.3 shutdown fail issue
  //
  UINT32      SmiEn;
  UINT32      SmiSts;
//[-start-121111-IB05280008-add]//
  EFI_STATUS  Status;
//[-end-121111-IB05280008-add]//

  SmiEn = IoRead32 (mAcpiBaseAddr + R_PCH_SMI_EN);
  SmiEn &= ~(B_PCH_SMI_EN_INTEL_USB2 |B_PCH_SMI_EN_LEGACY_USB2 |B_PCH_SMI_EN_LEGACY_USB);
  IoWrite32 (mAcpiBaseAddr + R_PCH_SMI_EN, SmiEn);
  SmiSts = IoRead32 (mAcpiBaseAddr + R_PCH_SMI_STS);
  SmiSts &= ~(B_PCH_SMI_STS_INTEL_USB2 |B_PCH_SMI_STS_INTEL_USB2 |B_PCH_SMI_STS_LEGACY_USB);
  IoWrite32 (mAcpiBaseAddr + R_PCH_SMI_STS,  SmiSts);

  //
  // Check what the state to return to after AC Loss. If Last State, then
  // set it to Off.
  //
  if (mStateAfterG3 == LastState) {
    SetAfterG3On (FALSE);
  }

//[-start-121108-IB05280008-modify]//
//  mSmmOemServices->Funcs[COMMON_SMM_S5_AC_LOSS_CALLBACK](mSmmOemServices, 0);
  Status = OemSvcS5AcLossCallback ();
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcKernelLib S5AcLossCallback, Status : %r\n", Status ) );
//[-end-121108-IB05280008-modify]//
  return;
}

/**
 1. Check if LAN present
 2. Clear LAN PCI PME Status (if set) & Enable LAN PCI PME Register
 3. Clear PCH Status
 4. Enable PCH/PmBase

 @param [in]   DispatchHandle   The handle of this callback, obtained when registering
 @param [in]   DispatchContext  The predefined context which contained sleep type and phase

**/
VOID
EFIAPI
S5SleepWakeOnLanCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
  UINT16            VendorId;
  UINT16            PmCsr;
//[-start-121105-IB03780470-add]//
  PCH_SERIES        PchSeries;
//[-end-121105-IB03780470-add]//

  //
  // Check if LAN is present
  //
  VendorId = MmPci16 (0, 1, PCI_DEVICE_NUMBER_PCH_LAN, PCI_FUNCTION_NUMBER_PCH_LAN, R_PCH_LAN_VENDOR_ID);
  if (VendorId == 0xFFFF) {
    return;
  }

  //
  // Clear LAN PME Status & enable LAN PME
  //
  PmCsr = MmPci16Or (0,
                     1,
                     PCI_DEVICE_NUMBER_PCH_LAN,
                     PCI_FUNCTION_NUMBER_PCH_LAN,
                     R_PCH_LAN_PMCS,
                     B_PCH_LAN_PMCS_PMEE
                     );

//[-start-121219-IB03780475-modify]//
  //
  // Clear PCH PME Status and enable PME
  //
//[-start-121105-IB03780470-modify]//
  PchSeries = GetPchSeries ();
  if (PchSeries == PchLp) {
    IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_GPE0_STS_127_96, B_PCH_ACPI_GPE0_STS_127_96_PME);
    IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96, (UINT32) (IoRead32 (mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96) | B_PCH_ACPI_GPE0_EN_127_96_PME));
  } else {
    IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_GPE0a_STS, B_PCH_ACPI_GPE0a_STS_PME);
    IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_GPE0a_EN, (UINT32) (IoRead32 (mAcpiBaseAddr + R_PCH_ACPI_GPE0a_EN) | B_PCH_ACPI_GPE0a_EN_PME));
  }
//[-end-121105-IB03780470-modify]//
//[-end-121219-IB03780475-modify]//

//[-start-121219-IB03780475-remove]//
//  //
//  // Enable PCH PME
//  //
////[-start-121105-IB03780470-modify]//
//  IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_GPE0a_EN, (UINT16) (IoRead16 (mAcpiBaseAddr + R_PCH_ACPI_GPE0a_EN) | B_PCH_ACPI_GPE0a_EN_PME));
////[-end-121105-IB03780470-modify]//
//[-end-121219-IB03780475-remove]//

  return;
}

/**

 @param [in]   DispatchHandle   The handle of this callback, obtained when registering
 @param [in]   DispatchContext  The predefined context which contained sleep type and phase

**/
VOID
EFIAPI
S5SleepWakeOnTimeCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
  S5WakeUpSetting (mAcpiBaseAddr, SetupVariable);

  return;
}

/**

 @param [in]   DispatchHandle   The handle of this callback, obtained when registering
 @param [in]   DispatchContext  The predefined context which contained sleep type and phase

**/
VOID
EFIAPI
S3SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
  UINT32      GpioReg;
//[-start-121111-IB05280008-add]//
  EFI_STATUS  Status;
//[-end-121111-IB05280008-add]//
  POST_CODE (SMM_S3_SLEEP_CALLBACK); //PostCode = 0xA3, Enter S3
  //
  // S3 Entry: Before SLP_EN is set, drive GPIO to logic low.
  // CRB implementation drives GPIO46 output to logic low.
  //

  //
  // Clear GPIO Lockdown Enable bit.
  //
  PchLpcPciCfg8And (R_PCH_LPC_GPIO_CNT, (UINT8)~B_PCH_LPC_GPIO_LOCKDOWN_EN);
  //
  // Drive GPIO 60 low before SLP_EN is set to support S3 power reduction feature
  //
//[-start-120731-IB10820094-modify]//
  GpioReg = IoRead32 (PcdGet16(PcdPchGpioBaseAddress) + PCH_GP_LVL2);
//[-end-120731-IB10820094-modify]//
  GpioReg &= ~BIT28;
  IoWrite32 (PcdGet16 (PcdPchGpioBaseAddress) + PCH_GP_LVL2, GpioReg);
  //
  // Clear status bit manually. Already in SMM, do not enter GpioUnlockCallback.
  //
  IoWrite32 (mAcpiBaseAddr + R_PCH_SMI_STS, B_PCH_SMI_STS_GPIO_UNLOCK);

//[-start-121122-IB07250300-modify]//
  if (FeaturePcdGet (PcdNvidiaOptimusSupported)) {
    SetSgVariable (mGlobalNvsArea);
  }
//[-end-121122-IB07250300-modify]//
  //mSmmOemServices->Funcs[CHIPSET_SMM_S3_CALLBACK](mSmmOemServices, 0);
  //mSmmOemServices->Funcs[COMMON_SMM_S3_CALLBACK](mSmmOemServices, 0);
  Status = OemSvcS3Callback ();
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcKernelLib S3Callback, Status : %r\n", Status ) );
//[-start-120508-IB03780437-add]//
//[-start-130709-IB05400426-modify]//
//[-end-121108-IB05280008-modify]//
  if (FeaturePcdGet (PcdThunderBoltSupported) && SetupVariable->TbtDevice != 0x00) {
    UpdateTbtSxVariable ();
  }
//[-end-120508-IB03780437-add]//
//[-end-130709-IB05400426-modify]//
}


/**
 This function either writes to or read from global register table the data of
 Gpio, SVID, LevelEdge, Pci, CpuState, Mtrr, Sio, Kbc, and Pic registers.

 @param [in]   SaveRestoreFlag  True: write data to SMM IO registers.
                                False: read data from IO to global registers.

 @retval EFI_SUCCESS

**/
EFI_STATUS
SaveRestoreState (
  IN BOOLEAN                        SaveRestoreFlag
  )
{
  EFI_STATUS              Status;
  UINTN                   CpuIndex;

  Status = SaveRestoreBSPState (SaveRestoreFlag);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  for (CpuIndex = 0; CpuIndex < mSmst->NumberOfCpus; CpuIndex++) {
    mSmst->SmmStartupThisAp (SaveRestoreAPState, CpuIndex, &SaveRestoreFlag);
  }
  Status = SaveRestoreMtrr (&SaveRestoreFlag);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SaveRestoreFlag) {
    for (CpuIndex = 0; CpuIndex < mSmst->NumberOfCpus; CpuIndex++) {
      mSmst->SmmStartupThisAp (SaveRestoreMtrr, CpuIndex, &SaveRestoreFlag);
    }
  }
  Status = SaveRestoreEMRR ( &SaveRestoreFlag );

  Status = SaveRestoreGpio (SaveRestoreFlag);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SaveRestoreLevelEdge (SaveRestoreFlag);
  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-130808-IB06720232-modify]//
  OemSvcEcSaveRestoreKbc (SaveRestoreFlag);
//[-end-130808-IB06720232-modify]//

  Status = SaveRestorePci (SaveRestoreFlag);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SetupVariable->LFPConfiguration == 3) {
    Status = SaveEDPReg (SaveRestoreFlag);
  }


  Status = SaveRestorePmIo (SaveRestoreFlag);
//[-start-120718-IB05330360-modify]//
  Status = SaveRestoreAbar (SaveRestoreFlag);
//[-start-120809-IB10820101-modify]//
  if (!FeaturePcdGet(PcdUltFlag)) {
    Status = SaveRestoreDmi (SaveRestoreFlag);

//[-start-121218-IB08050189-remove]//
//  Status = SaveRestoreRcba (SaveRestoreFlag);
//[-end-121218-IB08050189-remove]//
  }
//[-end-120809-IB10820101-modify]//
//[-end-120718-IB05330360-modify]//
  Status = SaveRestoreEp (SaveRestoreFlag);

  Status = SaveRestoreAzalia (SaveRestoreFlag);


  Status = SaveRestorePic (SaveRestoreFlag);
  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-121205-IB08050187-add]//
  SaveRestoreSpiReg (SaveRestoreFlag);
//[-end-121205-IB08050187-add]//

  return  EFI_SUCCESS;
}

/**
 SMI handler to enable ACPI mode
 Dispatched on reads from APM port with value EFI_ACPI_ENABLE_SW_SMI
 Disables the SW SMI Timer.
 ACPI events are disabled and ACPI event status is cleared.
 SCI mode is then enabled.
 Disable SW SMI Timer
 Clear all ACPI event status and disable all ACPI events
 Disable PM sources except power button
 Clear status bits
 Disable GPE0 sources
 Clear status bits
 Disable GPE1 sources
 Clear status bits
 Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
 Enable SCI

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @return Status Code

**/
EFI_STATUS
EFIAPI
PlatformEnableAcpiCallbackStart (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{

  UINT32      OutputValue;
  UINT32      SmiEn;
  UINT32      Pm1Cnt;
  UINT8       Var8;
//[-end-120630-IB05330352-add]//
  EFI_OVERRIDE_ASPM_PROTOCOL   *OverrideAspmProtocol;
  EFI_STATUS                   Status;
//[-start-120514-IB02382249-add]//
  UINT32      Address = 0;
  UINT32      Index = 0;
  UINT32      Data = 0;
  UINT32      Bar = 0;
  UINT32      XhciXecp = 0;
//[-end-120514-IB02382249-add]//

  Status = mSmst->SmmLocateProtocol(
                    &gEfiOverrideAspmProtocolGuid,
                    NULL,
                    (VOID **)&OverrideAspmProtocol
                    );
  if (!EFI_ERROR (Status)) {
    OverrideAspmProtocol->OverrideAspmFunc ();
  }
  mSmst->SmmFirmwareRevision = 1;
  //
  // Disable SW SMI Timer
  //
  SmiEn = IoRead32 (mAcpiBaseAddr + R_PCH_SMI_EN);
  SmiEn &= ~B_PCH_SMI_EN_SWSMI_TMR;
  IoWrite32 (mAcpiBaseAddr + R_PCH_SMI_EN, SmiEn);

  //
  // Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)
  //
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_D);
  IoWrite8 (PCAT_RTC_DATA_REGISTER, 0x00);

  //
  // Enable SCI
  //
  Pm1Cnt = IoRead32 (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT);
  Pm1Cnt |= B_PCH_ACPI_PM1_CNT_SCI_EN;
  IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
//[-start-120514-IB02382249-add]//
  //
  //  Close USB SMI capability except OS Ownership SMI
  //
  if (SetupVariable->Ehci1Enable) {
    OutputValue = MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB_EXT, PCI_FUNCTION_NUMBER_PCH_EHCI, R_PCH_EHCI_LEGEXT_CS);
    OutputValue &= 0x1FFF2000;
    MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB_EXT, PCI_FUNCTION_NUMBER_PCH_EHCI, R_PCH_EHCI_LEGEXT_CS) = OutputValue;
  }
  if (SetupVariable->Ehci2Enable) {
    OutputValue = MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB, PCI_FUNCTION_NUMBER_PCH_EHCI, R_PCH_EHCI_LEGEXT_CS);
    OutputValue &= 0x1FFF2000;
    MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_USB, PCI_FUNCTION_NUMBER_PCH_EHCI, R_PCH_EHCI_LEGEXT_CS) = OutputValue;
  }
//[-start-120817-IB03610441-modify]//
  if ( ( SetupVariable->XHCIMode != 0 ) && ( MmPci16 ( 0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI, R_PCH_USB_VENDOR_ID ) != 0xFFFF ) ) {
//[-end-120817-IB03610441-modify]//
    Address = MmPci32 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_XHCI, PCI_FUNCTION_NUMBER_PCH_XHCI, R_PCH_XHCI_MEM_BASE);
    Address &= 0xFFFFFFFB;
    //
    // Searching for USB Legacy Support Capability in xHCI Extended Capabilities Pointer (xECP)
    //    
    Index         = ((*(UINT32*)(UINTN)(Address + 0x10) & 0xffff0000) >> 14);
    while (((Data = *(UINT32*)(UINTN)(Address + Index)) & 0xff) != 0x01 && (Data & 0xff00) != 0) {
      Index += ((Data & 0xff00) >> (8 - 2));
    }
    if ((Data & 0xff) == 0x01) {
      XhciXecp = Bar + Index;
    }
    //
    //  xECP+4h USB Legacy Support Control and Status Register 
    //  xECP+4h[BIT0] USB SMI Enable 
    //
    Address = Address + XhciXecp + 4;
    OutputValue = MmioRead32 (Address);
    OutputValue &= 0x00FFFFFE;
    MmioWrite32 (Address, OutputValue);
  }
//[-start-120630-IB05330352-add]//
  if (!FeaturePcdGet(PcdUltFlag)) {
   //
   // Workaround for Vista install in AHCI mode : Clear R_PCH_SATA_ATS Status Register before enter OS
   //
   Var8 = MmPci8 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA, R_PCH_SATA_ATS);
   MmPci8 (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, PCI_FUNCTION_NUMBER_PCH_SATA, R_PCH_SATA_ATS) = Var8;
  }
//[-start-121122-IB07250300-modify]//
  if (FeaturePcdGet (PcdNvidiaOptimusSupported)) {
    OpenHdAudio (mGlobalNvsArea);
  }
//[-end-121122-IB07250300-modify]//
  //
  // Save Status to SMM
  //
  SaveRestoreState (FALSE);
//[-start-121122-IB07250300-modify]//
  if (FeaturePcdGet (PcdNvidiaOptimusSupported)) {
    CloseHdAudio (mGlobalNvsArea);
  }
//[-end-121122-IB07250300-modify]//
  //
  // Reduce S4 resume time
  //
//[-start-130809-IB06720232-remove]//
//  EcAcpiMode (TRUE);
//[-end-130809-IB06720232-remove]//

  return EFI_SUCCESS;
}

/**
 SMI handler to disable ACPI mode
 Dispatched on reads from APM port with value 0xA1
 ACPI events are disabled and ACPI event status is cleared.
 SCI mode is then disabled.
 Clear all ACPI event status and disable all ACPI events
 Disable PM sources except power button
 Clear status bits
 Disable GPE0 sources
 Clear status bits
 Disable GPE1 sources
 Clear status bits
 Disable SCI

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @return Status Code

**/
EFI_STATUS
EFIAPI
PlatformDisableAcpiCallbackStart (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  UINT32    Pm1Cnt;

  //
  // Disable SCI
  //
  Pm1Cnt = IoRead32 (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT);
  Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SCI_EN;
  IoWrite32 (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT, Pm1Cnt);

//[-start-130809-IB06720232-remove]//
//  EcAcpiMode (FALSE);
//[-end-130809-IB06720232-remove]//
  return EFI_SUCCESS;
}


/**
 SMI handler to restore ACPI mode

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @return Status Code

**/
EFI_STATUS
EFIAPI
PlatformRestoreAcpiCallbackStart (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{

  SaveRestoreState (TRUE);


  DmaInit();
  APICInit(SetupVariable);

//[-start-130809-IB06720232-remove]//
//  EcAcpiMode (TRUE);
//[-end-130809-IB06720232-remove]//
  return EFI_SUCCESS;

}

/**
 SMI handler to restore ACPI mode

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @return Status Code

**/
EFI_STATUS
EFIAPI
PlatformRestoreAcpiCallbackDone (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{


{
  EFI_STATUS                        Status;
  EFI_PROGRAM_SSID_SVID_PROTOCOL    *ProgramSsidSvidProtocol;

  Status = mSmst->SmmLocateProtocol (
             &gEfiProgramSsidSvidProtocolGuid,
             NULL,
             (VOID **)&ProgramSsidSvidProtocol
             );
  if ( !EFI_ERROR ( Status ) ) {
    ProgramSsidSvidProtocol->ProgramSsidSvidFunc ();
  }
}

{
  EFI_OVERRIDE_ASPM_PROTOCOL   *OverrideAspmProtocol;
  EFI_STATUS                   Status;

  Status = mSmst->SmmLocateProtocol(&gEfiOverrideAspmProtocolGuid,
                                NULL,
                                (VOID **)&OverrideAspmProtocol
                               );
  if (!EFI_ERROR (Status)) {
    OverrideAspmProtocol->OverrideAspmFunc ();
  }
}

  CheckPasswordEntry(SetupVariable->S3ResumeSecurity);
  return EFI_SUCCESS;
}

//[-start-130620-IB05160461-modify]//
//[-start-130619-IB05160459-modify]//
//[-start-120816-IB05300312-remove]//
/**
 When an unknown event happen.

 @param [in]   DispatchHandle
 @param [in]   DispatchContext

 @retval None

**/
VOID
EFIAPI
DummyTco1Callback (
 IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT           *DispatchContext
  )
{
  UINT16      Data16;
  
  switch (DispatchContext->Type) {

  case IchnMch:
    break;

  case IchnY2KRollover:
    break;

  case IchnTcoTimeout:
    //
    // action here (TCO first timeout)
    // ...
    //
    // set for tco timeout -> hw reboot
    // 1. RCBA 3410h[5] NO_REBOOT => set to 0 (TCO second timeout will reboot system)
    // 2. PMBASE 30h[13] TCO_EN => set to 0 (disable TCO send SMI, or the second timeout will not been triggered)
    //
    if (SetupVariable->TCOWatchDog) {
      PchMmRcrb32And ( R_PCH_RCRB_GCS, (UINT32)~B_PCH_RCRB_GCS_NR );
      Data16 = IoRead16 (mAcpiBaseAddr + R_PCH_SMI_EN);
      Data16 &= ~B_PCH_SMI_EN_TCO;
      IoWrite16 (mAcpiBaseAddr + R_PCH_SMI_EN, Data16);
    }
    break;

  case IchnOsTco:
    break;

  case IchnNmi:
    break;

  default:
    break;
  }
  return;
}
//
//[-end-120816-IB05300312-remove]//
//[-end-130619-IB05160459-modify]//
//[-end-130620-IB05160461-modify]//

VOID
EFIAPI
SmmReadyToBootFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                      Status;
  EFI_CPU_IO2_PROTOCOL            *CpuIo;
  UINT8                           Data8;
  UINT32                          Data32;
  UINT16                          Data16;

  Status = gBS->LocateProtocol ( &gEfiCpuIo2ProtocolGuid, NULL, (VOID **)&CpuIo );
  if ( EFI_ERROR ( Status ) ) {
    DEBUG ( ( EFI_D_ERROR, "Locate Protocol EfiCpuIoProtocol Failure!\n" ) );
  }

  if ( !EFI_ERROR ( Status ) ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Trigger a SMI to do ME Lock.\n" ) );

    Data8 = ME_LOCK_SW_SMI;
//[-start-121015-IB10820135-modify]//
    CpuIo->Io.Write ( CpuIo, EfiCpuIoWidthUint8, PcdGet16 ( PcdSoftwareSmiPort ), 1, &Data8 );
//[-end-121015-IB10820135-modify]//

    Data32 = PchMmRcrb32 ( R_PCH_SPI_PR0 );
    S3BootScriptSaveMemWrite(
      S3BootScriptWidthUint32,
      ( UINTN )( PCH_RCRB_BASE + R_PCH_SPI_PR0 ),
      1,
      (VOID*)(UINTN)&Data32
    );
    
    Data16 = PchMmRcrb16 ( R_PCH_SPI_HSFS );
    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      ( UINTN )( PCH_RCRB_BASE + R_PCH_SPI_HSFS ),
      1,
      &Data16
      );

    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "ME Lock SMI Done.\n" ) );
  }
}

//[-start-121120-IB09890039-remove]//
//
//EFI_STATUS
//EFIAPI
//OemCallback(
//  IN  EFI_HANDLE                   DispatchHandle,
//  IN  CONST  VOID                  *DispatchContext,
//  IN  OUT  VOID                    *CommBuffer,
//  IN  OUT  UINTN                   *CommBufferSize
//  )
//{
////[-start-120731-IB10820094-modify]//
//  EFI_IA32_REGISTER_SET         RegisterSet;
//
//  if (FeaturePcdGet(PcdSmmInt10Enable)) {
//    RegisterSet.H.AH = 0x0F;
//    mSmmThunk->SmmInt10 (mSmmThunk, &RegisterSet);
//    *(UINTN*)((UINTN)0x04E0) = RegisterSet.X.AX;
//    *(UINTN*)((UINTN)0x04E2) = RegisterSet.X.BX;
//    *(UINTN*)((UINTN)0x04E4) = RegisterSet.X.CX;
//    *(UINTN*)((UINTN)0x04E6) = RegisterSet.X.DX;
//    *(EFI_FLAGS_REG*)((UINTN)0x04E8) = RegisterSet.X.Flags;
//  }
////[-end-120731-IB10820094-modify]//
//
//  return EFI_SUCCESS;
//}
//
//[-end-121120-IB09890039-remove]//

/**

 @param [in]   DispatchHandle   The handle of this callback, obtained when registering
 @param [in]   DispatchContext  The predefined context which contained sleep type and phase

**/
VOID
EFIAPI
S1SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
//[-start-121111-IB05280008-add]//
  EFI_STATUS      Status;
//[-end-121111-IB05280008-add]//
  /*++
    Todo:
      Add project specific code in here.
  --*/
  POST_CODE (SMM_S1_SLEEP_CALLBACK); //PostCode = 0xA1, Enter S1
  //mSmmOemServices->Funcs[CHIPSET_SMM_S1_CALLBACK](mSmmOemServices, 0);
//[-start-121108-IB05280008-modify]//
  //mSmmOemServices->Funcs[COMMON_SMM_S1_CALLBACK](mSmmOemServices, 0);
  Status = OemSvcS1Callback ();
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcKernelLib OsResetCallback, Status : %r\n", Status ) );
//[-end-121108-IB05280008-modify]//
}

/**

 @param [in]   DispatchHandle   The handle of this callback, obtained when registering
 @param [in]   DispatchContext  The predefined context which contained sleep type and phase

**/
VOID
EFIAPI
S4SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
//[-start-121111-IB05280008-add]//
  EFI_STATUS      Status;
//[-end-121111-IB05280008-add]//
  /*++
    Todo:
      Add project specific code in here.
  --*/
  POST_CODE (SMM_S4_SLEEP_CALLBACK); //PostCode = 0xA4, Enter S4
//[-start-121122-IB07250300-modify]//
  if (FeaturePcdGet (PcdNvidiaOptimusSupported)) {
    SetSgVariable (mGlobalNvsArea);
  }
//[-end-121122-IB07250300-modify]//
//[-start-121109-IB05280008-modify]//
  //mSmmOemServices->Funcs[COMMON_SMM_S4_CALLBACK](mSmmOemServices, 0);
  Status = OemSvcS4Callback ();
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcKernelLib OsResetCallback, Status : %r\n", Status ) );
//[-end-121109-IB05280008-modify]//
//[-start-130709-IB05400426-modify]//
//[-start-120508-IB03780437-add]//
  if (FeaturePcdGet (PcdThunderBoltSupported) && SetupVariable->TbtDevice != 0x00) {
    UpdateTbtSxVariable ();
  }
//[-end-120508-IB03780437-add]//
//[-end-130709-IB05400426-modify]//
}

/**

 @param [in]   DispatchHandle   The handle of this callback, obtained when registering
 @param [in]   DispatchContext  The predefined context which contained sleep type and phase

**/
VOID
EFIAPI
S5SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
//[-start-121111-IB05280008-add]//
  EFI_STATUS    Status;
//[-end-121111-IB05280008-add]//
  /*++
    Todo:
      Add project specific code in here.
  --*/
  UINT8  Buffer;
  POST_CODE (SMM_S5_SLEEP_CALLBACK); //PostCode = 0xA5, Enter S5
  if(SetupVariable->S5LongRunTest == 1) {
  //
  //if S5LongRunTest is enable.
  //
    IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_REGISTER_B);
    Buffer = IoRead8 (PCAT_RTC_DATA_REGISTER);
    Buffer |= ALARM_INTERRUPT_ENABLE;
    IoWrite8 (PCAT_RTC_DATA_REGISTER, Buffer);
    Buffer = IoRead8 (mAcpiBaseAddr + PM1_EN_HIGH_BYTE);
    Buffer |= RTC_EVENT_ENABLE;
    IoWrite8 (mAcpiBaseAddr + PM1_EN_HIGH_BYTE, Buffer);
  }
//[-start-121108-IB05280008-modify]//
//  mSmmOemServices->Funcs[COMMON_SMM_S5_CALLBACK](mSmmOemServices, 0);
  Status = OemSvcS5Callback ();
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcKernelLib OsResetCallback, Status : %r\n", Status ) );
//[-end-121108-IB05280008-modify]//
  
//[-start-130709-IB05400426-modify]//
//[-start-120508-IB03780437-add]//
  if (FeaturePcdGet (PcdThunderBoltSupported) && SetupVariable->TbtDevice != 0x00) {
    UpdateTbtSxVariable ();
  }
//[-end-120508-IB03780437-add]//
//[-end-130709-IB05400426-modify]//
}

UINT32
ConvertBcdTime (
  IN  UINT8 DecimalVal
  )
{
  UINTN   High, Low;

  High    = (DecimalVal & 0xF0)>>4;
  Low     = DecimalVal & 0x0F;
  return ((UINT32)(Low + (High * 10)));
}

EFI_STATUS
EFIAPI
GetAlarmRtcTime (
  IN  EFI_HANDLE                   DispatchHandle,
  IN  CONST  VOID                  *DispatchContext,
  IN  OUT  VOID                    *CommBuffer,
  IN  OUT  UINTN                   *CommBufferSize
  )
{
  UINT32  AlarmTime;   
  
  AlarmTime = 0x80000000;
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_SECONDS_ALARM);
  AlarmTime |= ConvertBcdTime (IoRead8 (PCAT_RTC_DATA_REGISTER));

  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_MINUTES_ALARM);
  AlarmTime |= ConvertBcdTime (IoRead8 (PCAT_RTC_DATA_REGISTER)) << 6;

  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_HOURS_ALARM);
  AlarmTime |= ConvertBcdTime (IoRead8 (PCAT_RTC_DATA_REGISTER)) << 12;
  
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_DAY_OF_THE_MONTH);
  AlarmTime |= ConvertBcdTime (IoRead8 (PCAT_RTC_DATA_REGISTER)) << 17;
  
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_MONTH);
  AlarmTime |= ConvertBcdTime (IoRead8 (PCAT_RTC_DATA_REGISTER)) << 22;
  
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, RTC_ADDRESS_YEAR);
  AlarmTime |= ConvertBcdTime (IoRead8 (PCAT_RTC_DATA_REGISTER)) << 26;
  
  mGlobalNvsArea->AlarmTime = AlarmTime;

  return EFI_SUCCESS;
}

//[-start-120508-IB03780437-add]//
VOID
UpdateTbtSxVariable (
  )
{
  UINT8        TbtNullFlag = 0;
  UINT8        TbtBusNum = 0;
//[-start-130425-IB05160441-add]//
  UINT32       Tbt2PcieData;
//[-end-130425-IB05160441-add]//

  if (mGlobalNvsArea->TbtEndPointNum) {
    mSmmVariable->SmmSetVariable (
                    L"TbtSxSkipWakeTimer",
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    0,
                    &TbtNullFlag
                    );
  } else {
    mSmmVariable->SmmSetVariable (
                    L"TbtSxSkipWakeTimer",
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (UINT8),
                    &TbtNullFlag
                    );
  }
  
//[-start-130709-IB05400426-modify]//
//[-start-130124-IB04770265-modify]//
  if (SetupVariable->TbtChip == 1) {
    //
    // Thunderbolt RR Sx entry flow
    //
    if (!SetupVariable->TbtWakeFromDevice) {
      TbtBusNum = MmPci8 (0, THUNDERBOLT_PCIE_ROOT_PORT_BUS_NUM, THUNDERBOLT_PCIE_ROOT_PORT_DEV_NUM, (SetupVariable->TbtDevice - 1), R_PCH_PCIE_SCBN);
      TbtSetPCIe2TBTCommand (GO2SX_NO_WAKE, 0, TbtBusNum, &Tbt2PcieData);
    }
  }
  else{
    //
    // Thunderbolt CR Sx entry flow
    //
    EnterTbtSxState (SetupVariable->TbtWakeFromDevice);
  }
//[-end-130124-IB04770265-modify]//
//[-end-130709-IB05400426-modify]//
  return;
}
//[-end-120508-IB03780437-add]//
