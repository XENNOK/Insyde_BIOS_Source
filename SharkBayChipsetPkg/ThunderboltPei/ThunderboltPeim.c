/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <ThunderboltPeim.h>

//[-start-130205-IB03780481-modify]//
UINT8 mThunderboltChip;
//[-end-130205-IB03780481-modify]//

//[-start-130205-IB03780481-modify]//
static EFI_PEI_NOTIFY_DESCRIPTOR       mNotifyDesc[] = {
//  {
//    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH,
//    &gPeiMemoryDiscoveredPpiGuid,
//    ProgramTbtGpio
//  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiMemoryDiscoveredPpiGuid,
    TbtCallback
  }
};
//[-end-130205-IB03780481-modify]//

//
// Entry Point
//
EFI_STATUS
ThunderboltPeimEntryPoint (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                   Status;

  Status = PeiServicesNotifyPpi (mNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

//[-start-130205-IB03780481-remove]//
//EFI_STATUS
//EFIAPI
//ProgramTbtGpio (
//  IN CONST EFI_PEI_SERVICES                 **PeiServices,
//  IN       EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
//  IN       VOID                             *Ppi
//  )
//{
//  EFI_STATUS                        Status;
//  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariablePpi;
//  UINTN                             VariableSize;
//  CHIPSET_CONFIGURATION              SystemConfiguration;
//  UINT16                            TbtGpio;
////[-start-120507-IB03780437-add]//
//  EFI_BOOT_MODE                     BootMode;
////[-end-120507-IB03780437-add]//
////[-start-120828-IB03600500-add]//
//  EFI_PEI_STALL_PPI                 *StallPpi;
////[-end-120828-IB03600500-add]//
////[-start-121105-IB10820150-add]//
//  THUNDERBOLT_GPIO_TO_PCH_TABLE     *ConnectPchGpioTable;
//  THUNDERBOLT_GPIO_TO_SIO_TABLE     *ConnectSioGpioTable;
//
//  ConnectPchGpioTable = (THUNDERBOLT_GPIO_TO_PCH_TABLE *)PcdGetPtr ( PcdThunderBoltGpioToPch );
//  ConnectSioGpioTable = (THUNDERBOLT_GPIO_TO_SIO_TABLE *)PcdGetPtr ( PcdThunderBoltGpioToSio );
////[-end-121105-IB10820150-add]//
//  TbtGpio = 0;
//  
////[-start-120507-IB03780437-add]//
//  Status = PeiServicesGetBootMode (&BootMode);
//  ASSERT_EFI_ERROR (Status);
//
//  if (BootMode == BOOT_ON_S3_RESUME) {
//    return EFI_SUCCESS;
//  }
////[-end-120507-IB03780437-add]//
//
////[-start-120828-IB03600500-add]//
//  Status = PeiServicesLocatePpi (&gEfiPeiStallPpiGuid, 0, NULL, &StallPpi);
//  ASSERT_EFI_ERROR ( Status);
////[-end-120828-IB03600500-add]//
//
//  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, &VariablePpi);
//  ASSERT_EFI_ERROR (Status);
//
//  VariableSize = PcdGet32 (PcdSetupConfigSize);
//
//  Status = VariablePpi->GetVariable (
//             VariablePpi,
//             L"Setup",
//             &gSystemConfigurationGuid,
//             NULL,
//             &VariableSize,
//             &SystemConfiguration
//             );
//  ASSERT_EFI_ERROR (Status);
//
//  switch (SystemConfiguration.TBTSecurityLevel) {
//  case NormalModeWithNHI:
//    TbtGpio |= BIT7 | BIT6;
//    break;
//  case NormalModeWithoutNHI:
//    break;     
//  case SignleDepth:
//    TbtGpio |= BIT6;
//    break;
//  case RedriverOnly:
//    TbtGpio |= BIT7;
//    break;
////    case TbtOff:
////      break;
////    case TbtDebug:
////      TbtGpio |= BIT7 | BIT6 | BIT3;
////      break;
//  default:
//    break;
//  }
////[-start-121105-IB10820150-modify]//
//  if ( PcdGet8 ( PcdThunderBoltGpioConnectIc ) == 0 ) {
//    //
//    // for TBT GPIO 3,6,7 is routed to PCH GPIO 
//    // 
//    //
//    // Assume TBT GPIO 3,6,7 is mapped to PCH
//    // The defalut value of GPIO in gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable should be:
//    //   TBT GPIO 3 - PCH GPIO : Select GPIO, output, low
//    //   TBT GPIO 6 - PCH GPIO : Select GPIO, output, high
//    //   TBT GPIO 7 - PCH GPIO : Select GPIO, output, high
//    //  
//    //
//    // Update TBT GPIO 7 - PCH GPIO
//    //
//    PchTbtGpioValue ( ConnectPchGpioTable->TbtGpio7ToPchPin, TbtGpio & BIT7 );
//    //
//    // Update TBT GPIO 6 - PCH GPIO
//    //
//    PchTbtGpioValue ( ConnectPchGpioTable->TbtGpio6ToPchPin, TbtGpio & BIT6 );
//    //
//    // Update TBT GPIO 3 - PCH GPIO
//    //
//    PchTbtGpioValue ( ConnectPchGpioTable->TbtGpio3ToPchPin, SystemConfiguration.TBTGpio3 );
//  }else {
//    //
//    // for TBT GPIO 3,6,7 is routed to SIO/EC GPIO (GATB CRB rev.002)
//    //
//
//    //
//    // The default value of SIO/EC GPIO should be programmed to
//    //   TBT GPIO 3 - SIO/EC GPIO : Select GPIO, output, low
//    //   TBT GPIO 6 - SIO/EC GPIO : Select GPIO, output, high
//    //   TBT GPIO 7 - SIO/EC GPIO : Select GPIO, output, high
//    //
//
//    //
//    // Update TBT GPIO 7 - SIO/EC GPIO
//    //
//    SioTbtGpioValue (
//      ConnectSioGpioTable->TbtGpio7ToSioGpioSetAddress,
//      TbtGpio & BIT7,
//      ConnectSioGpioTable->TbtGpio7ToSioGpioSetAddressBit
//      );
//    //
//    // Update TBT GPIO 6 - SIO/EC GPIO
//    //
//    SioTbtGpioValue (
//      ConnectSioGpioTable->TbtGpio6ToSioGpioSetAddress,
//      TbtGpio & BIT6,
//      ConnectSioGpioTable->TbtGpio6ToSioGpioSetAddressBit
//      );
////[-start-130222-IB04770265-remove]//
////    //
////    // Update TBT GPIO 3 - SIO/EC GPIO
////    //
////    SioTbtGpioValue  (
////      ConnectSioGpioTable->TbtGpio3ToSioGpioSetAddress,
////      SystemConfiguration.TBTGpio3,
////      ConnectSioGpioTable->TbtGpio3ToSioGpioSetAddressBit
////      );
////[-end-130222-IB04770265-remove]//
//  }
////[-end-121105-IB10820150-modify]//
////[-start-130222-IB04770265-remove]//
//////[-start-120828-IB03600500-add]//
//////
////// Assert for 400ms
//////
////  if (SystemConfiguration.TBTGpio3 == GPIO3_PULSE) {
////    StallPpi->Stall (PeiServices, StallPpi, 400);  
////    SioGpioValue &= ~BIT1;
////    IoWrite8 (SioGpioBase, SioGpioValue);
////  }  
//////[-end-120828-IB03600500-add]//
////[-end-130222-IB04770265-remove]//
//
//  return EFI_SUCCESS;
//}
//[-end-130205-IB03780481-remove]//

//[-start-120507-IB03780437-add]//
EFI_STATUS
EFIAPI
//[-start-130205-IB03780481-modify]//
TbtCallback (
//[-end-130205-IB03780481-modify]//
  IN CONST EFI_PEI_SERVICES                 **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN       VOID                             *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariablePpi;
  UINTN                             VariableSize;
  CHIPSET_CONFIGURATION             SystemConfiguration;
//[-start-130710-IB05160465-add]//
  VOID                              *SetupVariable;
//[-end-130710-IB05160465-add]//
  UINT16                            SioGpioBase;
  UINT8                             SioGpioValue;
  EFI_PEI_STALL_PPI                 *StallPpi;
  EFI_BOOT_MODE                     BootMode;
  BOOLEAN                           TbtSxSkipWakeTimeFlag;
  UINTN                             TbtNullFlagSize;
  UINT8                             TbtNullFlag;
//[-start-130220-IB03780481-modify]//
//[-start-130124-IB04770265-add]//
  UINT32                            RPBase;
//  BOOLEAN                           TbtCmdSts;
//[-end-130124-IB04770265-add]//
//[-end-130220-IB03780481-modify]//
//[-start-130205-IB03780481-add]//
  UINT32                            Tbt2PcieData;
  UINT32                            TimeOut;
  UINT32                            TbtHostBase;
  UINT32                            TbtHostVidDid;
  BOOLEAN                           SetupVariableIsValid = FALSE;
//[-end-130205-IB03780481-add]//
//[-start-130709-IB05400426-add]//
  UINT8                             ThunderBoltRootPortFunNum;
  BOOLEAN                           TbtDelayFlag;
  UINT8                             ThunderboltChip;
//[-end-130709-IB05400426-add]//

  TbtSxSkipWakeTimeFlag = FALSE;
  TbtNullFlagSize = sizeof (UINT8);
  TbtNullFlag = 0;
//[-start-130709-IB05400426-add]//
  ThunderBoltRootPortFunNum = 0;
  TbtDelayFlag = FALSE;
  ThunderboltChip = PcdGet8 (PcdThunderBoltChip);
//[-end-130709-IB05400426-add]//

  mThunderboltChip = PcdGet8 ( PcdThunderBoltChip);

  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VariablePpi);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesLocatePpi (&gEfiPeiStallPpiGuid, 0, NULL, (VOID **)&StallPpi);
  ASSERT_EFI_ERROR (Status);

//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariable = AllocateZeroPool (VariableSize);

  Status = VariablePpi->GetVariable (
             VariablePpi,
             L"Setup",
             &gSystemConfigurationGuid,
             NULL,
             &VariableSize,
             SetupVariable
             );
  ASSERT_EFI_ERROR (Status);

  CopyMem (&SystemConfiguration, SetupVariable, sizeof(CHIPSET_CONFIGURATION));
//[-end-130709-IB05160465-modify]//

//[-start-130205-IB03780481-modify]//
  if (!EFI_ERROR (Status)) {
    SetupVariableIsValid = TRUE;
  }
//[-end-130205-IB03780481-modify]//

//[-start-130709-IB05400426-add]//
  if (SetupVariableIsValid) {
    if (SystemConfiguration.TbtDevice == 0x00) {
      //
      // ThunterBolt disable, return
      //  
      DEBUG ((EFI_D_ERROR, "ThunterBolt Disabled!!\n"));
      OemSvcPeiTbtInit (FALSE, 0xFF, &TbtDelayFlag);
      return EFI_SUCCESS;
    } else {
      ThunderBoltRootPortFunNum = (SystemConfiguration.TbtDevice - 1);
      ThunderboltChip = SystemConfiguration.TbtChip;
      DEBUG ((EFI_D_ERROR, "ThunterBolt Enabled, Root Port = %x, Chip = %x!!\n", ThunderBoltRootPortFunNum, ThunderboltChip));
      OemSvcPeiTbtInit (TRUE, ThunderboltChip, &TbtDelayFlag);
    }
  } else {
    return EFI_SUCCESS;
  }
//[-end-130709-IB05400426-add]//
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          L"TbtSxSkipWakeTimer",
                          &gEfiGlobalVariableGuid,
                          NULL,
                          &TbtNullFlagSize,
                          &TbtNullFlag
                          );
  if (!EFI_ERROR (Status)) {
    TbtSxSkipWakeTimeFlag = TRUE;
  }

//[-start-130205-IB03780481-modify]//
//[-start-130124-IB04770265-modify]//
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
//[-start-130709-IB05400426-modify]//
  if (ThunderboltChip == REDWOOD_RIDGE) {
//[-end-130709-IB05400426-modify]//
    //
    // Assign a temp bus(bus=0x02) for PCIE2TBT command
    //
//[-start-130709-IB05400426-modify]//
    RPBase = MmPciAddress (0, THUNDERBOLT_PCIE_ROOT_PORT_BUS_NUM, THUNDERBOLT_PCIE_ROOT_PORT_DEV_NUM, ThunderBoltRootPortFunNum, 0);
//[-end-130709-IB05400426-modify]//
    Mmio32AndThenOr (RPBase, R_PCH_PCIE_BNUM, 0xFF0000FF, 0x00020200);
    TbtHostBase = MmPciAddress (0, 0x02, 0x00, 0x00, 0);

    //
    // Initialize Thunderbolt RR Security Level.
    //
    if (!SetupVariableIsValid) {
      //
      // Drive GPIO_3__FORCE_PWR
      //
      TbtSetGpio3 (TRUE);
      
      //
      // Follow "Thunderbolt BIOS Implementation guide for Redwood Ridge based devices version 0.9"
      // to set default security level to "User Authorization".
      //
      TbtSetPCIe2TBTCommand (SET_SECURITY_LEVEL, UserAuthorization, 0x02, &Tbt2PcieData);
      StallPpi->Stall (PeiServices, StallPpi, 250000);
    } else {
      //
      // Check current Security Level is the same from setup value.
      //
      TbtSetPCIe2TBTCommand (GET_SECURITY_LEVEL, 0, 0x02, &Tbt2PcieData);
      StallPpi->Stall (PeiServices, StallPpi, 250000);

      if (RShiftU64 (Tbt2PcieData, 8) != SystemConfiguration.TBTSecurityLevel) {
        //
        // Drive GPIO_3__FORCE_PWR
        //
        TbtSetGpio3 (TRUE);

        //
        // Update Security Level from setup value.
        //
        TbtSetPCIe2TBTCommand (SET_SECURITY_LEVEL, SystemConfiguration.TBTSecurityLevel, 0x02, &Tbt2PcieData);
        StallPpi->Stall (PeiServices, StallPpi, 250000);
      }
    }

    //
    // Update GPIO_3__FORCE_PWR from setup setting.
    //
    if (SystemConfiguration.TBTGpio3 == 0) {
      TbtSetGpio3 (FALSE);
    } else {
      TbtSetGpio3 (TRUE);
    }

    //
    // Thunderbolt RR Sx exit flow
    //
    if (!TbtSxSkipWakeTimeFlag && (BootMode == BOOT_ON_S3_RESUME || BootMode == BOOT_ON_S4_RESUME)) {
      //
      // Delay 500ms when Thunderbolt wake support is disabled.
      //
      if (!SystemConfiguration.TbtWakeFromDevice) {
        Status = StallPpi->Stall (PeiServices,StallPpi, 500000);
        ASSERT_EFI_ERROR (Status);
      }

      //
      // Perform "Wait for fast link bring-up flow"
      //
      TbtWaitForFastLink (0x02);
    } else {
      //
      // Send OS_UP commnad in early stage to prevent TBT devices can not be detected properly.
      //
      TbtHostVidDid = Mmio32 (TbtHostBase, R_PCH_PCIE_VENDOR_ID);
      if (TbtHostVidDid == 0xFFFFFFFF) {
        TbtSetPCIe2TBTCommand (OS_UP, 0, 0x02, &Tbt2PcieData);
      }
      TimeOut = 50;
      while ((TbtHostVidDid == 0xFFFFFFFF) && (TimeOut > 0)) {
        TbtHostVidDid = Mmio32 (TbtHostBase, R_PCH_PCIE_VENDOR_ID);
        StallPpi->Stall (PeiServices,StallPpi, 100000);
        TimeOut--;
      }
    }
    
    //
    // Clear temp bus
    //
    Mmio32And (RPBase, R_PCH_PCIE_BNUM, 0xFF0000FF);
//[-start-130709-IB05400426-modify]//
  } else if (ThunderboltChip == CACTUS_RIDGE) {
//[-end-130709-IB05400426-modify]//
    //
    // Thunderbolt CR Security and Sx flow
    //
    if (TBT_PWR_EN_GPIO40) {
      //
      // Pull High TBT CR HR PWR - SIO/EC GPIO 40 to Power on CR
      //
      SioGpioBase  = SIO_SAMPLE_IO_BASE + GPIO_SET_4; // SIO GPIO Set#4 base address
      SioGpioValue = IoRead8(SioGpioBase);  
      SioGpioValue |= BIT0;
      IoWrite8 (SioGpioBase, SioGpioValue);    
      if (!SystemConfiguration.TbtWakeFromDevice && TbtSxSkipWakeTimeFlag \
            && (BootMode == BOOT_ON_S3_RESUME || BootMode == BOOT_ON_S4_RESUME)) {
        //
        // Pull down GPIO_9__OK2GO2SX_N_OD - SIO/EC GPIO 22 
        //
        SetTBTGPIO9 (OutputMode, LOW, SIO_UNLOCK);       
      }    
    }
 
    //
    // Deasserted TBT GPIO 2 - SIO/EC GPIO 20
    //
    SioGpioBase  = SIO_SAMPLE_IO_BASE + GPIO_SET_2; // EC GPIO Set#2 base address
    SioGpioValue = IoRead8 (SioGpioBase);  
    IoWrite8 (SioGpioBase, SioGpioValue & ~BIT0);
    //
    // Update TBT GPIO 3 - SIO/EC GPIO 21
    //
    SioGpioBase  = SIO_SAMPLE_IO_BASE + GPIO_SET_2; // SIO GPIO Set#2 base address
    SioGpioValue = IoRead8 (SioGpioBase);  
    if (SystemConfiguration.TBTGpio3) {
      SioGpioValue |= BIT1;
    } else {
      SioGpioValue &= ~BIT1;
    }
    IoWrite8 (SioGpioBase, SioGpioValue);

    //
    // Assert for 400ms
    //
    if (SystemConfiguration.TBTGpio3 == GPIO3_PULSE) {
      StallPpi->Stall (PeiServices, StallPpi,  400);  
      SioGpioValue &= ~BIT1;
      IoWrite8 (SioGpioBase, SioGpioValue);
    }  

    if (!TbtSxSkipWakeTimeFlag && (BootMode == BOOT_ON_S3_RESUME || BootMode == BOOT_ON_S4_RESUME)) {
      //
      // Wait N Sec for TBT link
      //
      if (SystemConfiguration.TbtSxWakeTimer > 4 ) {
        StallPpi->Stall (
                    PeiServices,
                    StallPpi,
                    SystemConfiguration.TbtSxWakeTimer * 100000 - 400000
                    );
      }
      if (!SystemConfiguration.TbtWakeFromDevice) {
        //
        // Pull High GPIO_9__OK2GO2SX_N_OD - SIO/EC GPIO 22 
        //
        SetTBTGPIO9 (OutputMode, HIGH, SIO_UNLOCK);  
        //
        // Restore GPIO_9__OK2GO2SX_N_OD to Input Mode
        //
        SetTBTGPIO9 (InputMode, HIGH, SIO_LOCK);          
      }   
    }
  }
//[-end-130124-IB04770265-modify]//
//[-end-130205-IB03780481-modify]//

//[-start-130709-IB05400426-add]//
  if (TbtDelayFlag) {
    //
    // BugBug Stall unit not microsecond
    //
    StallPpi->Stall (PeiServices, StallPpi, 500000);  
  }
//[-end-130709-IB05400426-add]//

  return EFI_SUCCESS;
}
