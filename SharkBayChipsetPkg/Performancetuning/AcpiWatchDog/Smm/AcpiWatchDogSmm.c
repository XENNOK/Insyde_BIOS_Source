/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "AcpiWatchDogSmm.h"

UINTN                                   mAcpiBaseAddr;

EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE  gAcpiWdatTable = {
  {
    SIGNATURE_32 ('W', 'D', 'T', 'T'),
    sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE),
     //
     //  because   require to  set this field value as 2 for running XTU BIOS Compliance Test tool
     //
    //EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE_REVISION,
    2,
    0,
    EFI_ACPI_OEM_ID,
    EFI_ACPI_OEM_TABLE_ID,
    EFI_ACPI_OEM_REVISION,
    EFI_ACPI_CREATOR_ID,
    EFI_ACPI_CREATOR_REVISION,
  },

  sizeof (EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE) - sizeof (EFI_ACPI_DESCRIPTION_HEADER),
  0xFFFFFFFF,
  0xFF,
  0x0,
  0x0,
  0x1,
  // Because the address of "Action Table : SET_COUNTDOWN_PERIOD" in WDTT point to PCH TCO_TMR (TCOBASE +12h)
  // and this timer is clocked at approximately 0.6 seconds
  // So this field should set 600 instead of 1000 
  // and timeouts ranging from 1.2 second to 613.8 seconds (count value from 2 to 1023)
  600,
  1023,
  2,
  0,
  0,
};


//
// Watchdog instructyions for Intel TCO
//
EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY gAcpiWdataActEntryTable [] = {
  //
  //================================================================
  //                   _
  //================================================================
  //
  // Restarts the watchdog timer's countdown.
  // This action is required.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_RESET,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 0, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO_RLD},
    0x0,
    0x03FF
  },
  //
  //================================================================
  //                   _
  //================================================================
  //
  // Returns the current countdown value of the watchdog hardware (in count intervals).
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_CURRENT_COUNTDOWN_PERIOD,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_COUNTDOWN,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 0, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO_RLD},
    0x0,
    0x03FF
  },
  //
  //================================================================
  //                   _
  //================================================================
  //
  // Returns the countdown value the watchdog hardware 
  // is configured to use when reset
  // (in count intervals).
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_COUNTDOWN_PERIOD,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_COUNTDOWN,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 0, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO_TMR},
    0x0,
    0x03FF
  },
  //
  //================================================================
  //                   Set Countdown Period List
  //================================================================
  //
  // Sets the countdown value (in count intervals) 
  // to be used when the watchdog timer is reset.
  // This action is required if WATCHDOG_ACTION_RESET does not explicitly 
  // write a new countdown value to a register during a reset. 
  // Otherwise, this action is optional.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_COUNTDOWN_PERIOD,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_COUNTDOWN | EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 0, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO_TMR},
    0x0,
    0x03FF
  },
  //
  //================================================================
  //                   Query Running State List
  //================================================================
  //
  // Determines if the watchdog hardware is currently in enabled/running 
  // state. The same result must occur when performed from both 
  // from enabled/stopped state and enabled/running state.
  // If the watchdog hardware is disabled, results are indeterminate.
  // This action is required.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_RUNNING_STATE,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_VALUE,
    0,
    0,
    //{EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO1_CNT+1},
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 11, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO1_CNT},
    0x0,
    //0x8
    0x1
  },
  //
  //================================================================
  //                   Set Running State List
  //================================================================
  //
  // Starts the watchdog, if not already in running state.
  // If the watchdog hardware is disabled, results are indeterminate.
  // This action is required.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_RUNNING_STATE,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE | EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER,
    0,
    0,
    //{EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO1_CNT+1},
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 0, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO1_CNT},
    //0X0,
    (0x0 << 11),
    //0x8
    (0x1 << 11)
  },
  // set CMOS 0x59[0] to 1
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_RUNNING_STATE,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, R_XCMOS_INDEX},
    XtuWdtStatus,
    0xFF
  },
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_RUNNING_STATE,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE | EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, R_XCMOS_DATA},
    B_XTU_WATCH_DOG_TRIGGER,
    B_XTU_WATCH_DOG_TRIGGER
  },
  //
  //================================================================
  //                   Query Stopped State List
  //================================================================
  //
  // Determines if the watchdog hardware is currently in enabled/stopped 
  // state. The sameresult must occur when performed from both the 
  // enabled/stopped state and enabled/running state.
  // If the watchdog hardware is disabled, results are indeterminate.
  // This action is required.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_STOPPED_STATE,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_VALUE,
    0,
    0,
    //{EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO1_CNT+1},
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 11, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO1_CNT},
    //0x8,
    0x1,
    //0x8
    0x1
  },
  //
  //================================================================
  //                   Set Stopped State List
  //================================================================
  //
  // Stops the watchdog, if not already in stopped state.
  // If the watchdog hardware is disabled, results are indeterminate.
  // This action is required.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_STOPPED_STATE,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE | EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER,
    0,
    0,
    //{EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO1_CNT+1},
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 0, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO1_CNT},
    //0x8,
    (0x1 << 11),
    //0x8
    (0x1 << 11)
  },
  // clear CMOS 0x59[0]
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_STOPPED_STATE,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, R_XCMOS_INDEX},
    XtuWdtStatus,
    0xFF
  },
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_STOPPED_STATE,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE | EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, R_XCMOS_DATA},
    0x0,
    B_XTU_WATCH_DOG_TRIGGER
  },
  //
  //================================================================
  //                   _
  //================================================================
  //
  // Determines if the watchdog hardware is configured 
  // to perform a reboot when the watchdog is fired.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_REBOOT,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_VALUE,
    0,
    0,
    //{EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO2_CNT+1},
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 4, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO2_CNT},
    0x0,
    0x3
  },
  //
  //================================================================
  //                   _
  //================================================================
  //
  // Configures the watchdog hardware to perform a reboot  
  // when it is fired.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_REBOOT,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE | EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER,
    0x0,
    0x0,
    //{EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO2_CNT+1},
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 0, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO2_CNT},
    //0x0,
    (0X0 << 4),
    //0x3
    (0X3 << 4)
  },
  //
  //================================================================
  //                   _
  //================================================================
  //
  // Determines if the watchdog hardware is configured to perform 
  // a system shutdown when fired.
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_SHUTDOWN,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_VALUE,
    0,
    0,
    //{EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO2_CNT+1},
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 4, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO2_CNT},
    0x1,
    0x3
  },
  //
  //================================================================
  //                   _
  //================================================================
  //
  // Configures the watchdog hardware to perform a system shutdown 
  // when fired. 
  //
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_SET_SHUTDOWN,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE | EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER,
    0,
    0,
    //{EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO2_CNT+1},
    {EFI_ACPI_3_0_SYSTEM_IO, 16, 0, EFI_ACPI_3_0_WORD, PCH_TCO_BASE + R_PCH_TCO2_CNT},
    //0x1,
    (0x1 << 4),
    //0x3
    (0x3 << 4)
  },
  //
  //================================================================
  //                   Query Watchdog Status List
  //================================================================
  //
  // Determines if the current boot was caused by the watchdog firing.
  // The boot status is required to be set if the watchdog fired 
  // and caused a reboot.
  // It is recommended that the Watchdog Status be set if the watchdog
  // fired and causeda shutdown.
  // This action is required.
  //
//  {
//    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_WATCHDOG_STATUS,
//    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_VALUE,
//    0,
//    0,
//    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO_WDCNT},
//    WATCHDOG_DEFAULT_STATUS_VALUE,
//    0xFF
//  },
  // read CMOS 0x59[1]
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_WATCHDOG_STATUS,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, R_XCMOS_INDEX},
    XtuWdtStatus,
    0xFF
  },
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_QUERY_WATCHDOG_STATUS,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_READ_VALUE,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, R_XCMOS_DATA},
    B_XTU_WATCH_DOG_STATUS,
    B_XTU_WATCH_DOG_STATUS
  },
  //
  //================================================================
  //                   Clear Watchdog Status List
  //================================================================
  //
  // Sets the watchdog's boot status to the default value.
  // This action is required.
  //
//  {
//    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_CLEAR_WATCHDOG_STATUS,
//    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE,
//    0,
//    0,
//    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, PCH_TCO_BASE + R_PCH_TCO_WDCNT},
//    WATCHDOG_DEFAULT_STATUS_VALUE,
//    0xFF
//  },
  // clear CMOS 0x59[1]
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_CLEAR_WATCHDOG_STATUS,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, R_XCMOS_INDEX},
    XtuWdtStatus,
    0xFF
  },
  {
    EFI_ACPI_WDAT_1_0_WATCHDOG_ACTION_CLEAR_WATCHDOG_STATUS,
    EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_WRITE_VALUE | EFI_ACPI_WDAT_1_0_WATCHDOG_INSTRUCTION_PRESERVE_REGISTER,
    0,
    0,
    {EFI_ACPI_3_0_SYSTEM_IO, 8, 0, EFI_ACPI_3_0_BYTE, R_XCMOS_DATA},
    0x0,
    B_XTU_WATCH_DOG_STATUS
  },
};

/**
  
  When an unknown event happen
  
  @param  EFI_HANDLE             
  @param  EFI_SMM_ICHN_DISPATCH_CONTEXT                       

  @retval None         

**/
VOID
Tco1Callback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT           *DispatchContext
  )
{
  UINT16                Data16;
  UINT32                Data32;

  switch (DispatchContext->Type) {

  case IchnTcoTimeout:
    //
    // Set watchdog fired flag
    //
    gSmst->SmmIo.Io.Read (
                      &gSmst->SmmIo,
                      SMM_IO_UINT16,
                      mAcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO2_CNT,
                      1,
                      &Data16
                      );
    Data16 &= B_PCH_TCO2_CNT_OS_POLICY;
    Data16 >>= 4;
    switch (Data16) {
      case 0x00:
      default:
        //
        // Reset/off system
        //
        Data16 = 0x06;
        do{
          gSmst->SmmIo.Io.Write (
                            &gSmst->SmmIo,
                            SMM_IO_UINT8,
                            0xCF9,
                            1,
                            &Data16
                            );
        } while(1);
        break;

      case 0x01:
        //
        // Clear Sleep Type Enable
        //
        gSmst->SmmIo.Io.Read (
                          &gSmst->SmmIo,
                          SMM_IO_UINT32,
                          mAcpiBaseAddr + R_PCH_SMI_EN,
                            1,
                          &Data32
                          );
        Data32 &= ~B_PCH_SMI_EN_ON_SLP_EN;
        gSmst->SmmIo.Io.Write (
                          &gSmst->SmmIo,
                          SMM_IO_UINT32,
                          mAcpiBaseAddr + R_PCH_SMI_EN,
                            1,
                          &Data32
                          );
        //
        // Shut it off now
        //
        gSmst->SmmIo.Io.Read (
                          &gSmst->SmmIo,
                          SMM_IO_UINT32,
                          mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
                          1,
                          &Data32
                          );
        Data32 &= ~(B_PCH_ACPI_PM1_CNT_SLP_EN | B_PCH_ACPI_PM1_CNT_SLP_TYP);
        Data32 |= V_PCH_ACPI_PM1_CNT_S5;
        gSmst->SmmIo.Io.Write (
                          &gSmst->SmmIo,
                          SMM_IO_UINT32,
                          mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
                          1,
                          &Data32
                          );
        Data32 |= B_PCH_ACPI_PM1_CNT_SLP_EN;
        gSmst->SmmIo.Io.Write (
                          &gSmst->SmmIo,
                          SMM_IO_UINT32,
                          mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT,
                          1,
                          &Data32
                          );
        break;
    }
    break;

  default:
    break;
  }
}

/**
  
  Initializes ICH WatchDog Timer Driver
  
  @param  ImageHandle              Pointer to the loaded image protocol for this driver
  @param  SystemTable               Pointer to the EFI System Table

  @retval EFI_SUCCESS               The function completed successfully.
  @retval otherwise                      Assert

**/
EFI_STATUS
InitializeAcpiWatchDog (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_SMM_ICHN_DISPATCH_PROTOCOL            *IchnDispatch;
  EFI_SMM_ICHN_DISPATCH_CONTEXT             IchnContext;
  EFI_HANDLE                                IchnHandle;
  UINT16                                    Data16;
  DXE_XTU_POLICY_PROTOCOL                   *XtuPlatformPolicyInstance;

  Status = EFI_SUCCESS;
  IchnHandle = NULL;

  Status = gBS->LocateProtocol (&gDxePlatformXtuPolicyGuid, NULL, (VOID **)&XtuPlatformPolicyInstance);
  
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (! XtuPlatformPolicyInstance->EanbleRealtimeWdttAcpiTable) {
    return EFI_ABORTED;
  }


  //
  // Setup the AcpiBaseAddr into both of BS and SMM instance
  //
  mAcpiBaseAddr = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;
  DEBUG ((EFI_D_ERROR, "--> InitializeAcpiWatchDog.\n"));

  //
  // Great!  We're now in SMM!
  //
  DEBUG ((EFI_D_ERROR, "In Smm InitializeAcpiWatchDog.\n"));

  //
  // Set default to reboot system
  //
  gSmst->SmmIo.Io.Read (
                    &gSmst->SmmIo,
                    SMM_IO_UINT16,
                    mAcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO2_CNT,
                    1,
                    &Data16
                    );
  Data16 &= ~B_PCH_TCO2_CNT_OS_POLICY;
  gSmst->SmmIo.Io.Write (
                    &gSmst->SmmIo,
                    SMM_IO_UINT16,
                    mAcpiBaseAddr + PCH_TCO_BASE + R_PCH_TCO2_CNT,
                    1,
                    &Data16
                    );
  //
  // Get the ICHn protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmmIchnDispatchProtocolGuid, NULL, (VOID **)&IchnDispatch);
  ASSERT_EFI_ERROR (Status);

  IchnContext.Type = IchnTcoTimeout;
  Status = IchnDispatch->Register(
                           IchnDispatch,
                           Tco1Callback,
                           &IchnContext,
                           &IchnHandle
                           );
  ASSERT_EFI_ERROR( Status );
  DEBUG ((EFI_D_ERROR, "<-- InitializeAcpiWatchDog.\n"));
  return  Status;
}
