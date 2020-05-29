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

#include <ThunderboltDxe.h>

//
// Entry Point
//
EFI_STATUS
ThunderboltDxeEntryPoint (
  IN  EFI_HANDLE               ImageHandle,
  IN  EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                   Status;
  EFI_SETUP_UTILITY_PROTOCOL   *SetupUtility;
  CHIPSET_CONFIGURATION         *SetupConfiguration;
//  UINT16                       GpioBase;
//  UINT16                       GpioOffset;
//  UINT32                       GpioValue;
//[-start-120830-IB03600500-remove]//
//  UINT16                       SioGpioBase;
//  UINT8                        SioGpioValue;
//[-end-120830-IB03600500-remove]//
  EFI_EVENT                    Event;
  VOID                         *Registration;
//[-start-130222-IB03780481-add]//
  EFI_GLOBAL_NVS_AREA_PROTOCOL *GlobalNvsAreaProtocol;
  EFI_GLOBAL_NVS_AREA          *GlobalNvsArea;
//[-end-130222-IB03780481-add]//

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR (Status);
  
  SetupConfiguration = (CHIPSET_CONFIGURATION *) (SetupUtility->SetupNvData);

//[-start-130222-IB03780481-add]//
  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);

  GlobalNvsArea = GlobalNvsAreaProtocol->Area;
  
//[-start-130709-IB05400426-add]//
  if ((SetupConfiguration->TbtDevice == 0x00) || (SetupConfiguration->TbtHotPlug == 0x00)) {
    //
    // 1. Thunderbolt disabled
    // 2. Thunderbolt Hot Plug do not support
    //
    GlobalNvsArea->TbtRootPortNum = 0xFF;
    return EFI_SUCCESS;
  }
//[-end-130709-IB05400426-add]//

  //
  // Update Thunderbolt platform configuration
  //

//[-start-130709-IB05400426-modify]//
  GlobalNvsArea->TbtChipType    = SetupConfiguration->TbtChip;
  GlobalNvsArea->TbtRootPortNum = SetupConfiguration->TbtDevice - 1;
//[-end-130709-IB05400426-modify]//
//[-end-130222-IB03780481-add]//

//[-start-121114-IB11410022-remove]//
//  if (SetupConfiguration->TBTGpio3 == 2) {
////[-start-121113-IB10820162-modify]//
//    if ( PcdGet8 ( PcdThunderBoltGpioConnectIc ) == 0 ) {
//    //
//    // Sample for TBT GPIO 3 is routed to PCH GPIO
//    //
//    //
//    // Update TBT GPIO 3 - PCH GPIO 12
//    //
//      PchTbtGpioValue ( ConnectPchGpioTable->TbtGpio3ToPchPin, 0 );
//    } else {
//    //
//    // Sample for TBT GPIO 3 is routed to SIO/EC GPIO (GATB CRB rev.002)
//    // Update TBT GPIO 3 - SIO/EC GPIO 21
//    //
//      SioTbtGpioValue (
//        ConnectSioGpioTable->TbtGpio3ToSioGpioSetAddress,
//        0,
//        ConnectSioGpioTable->TbtGpio3ToSioGpioSetAddressBit
//        );
//    }
////[-end-121113-IB10820162-modify]//
//  }
//[-end-120830-IB03600500-remove]//

  if (SetupConfiguration->TBTSmiEnable == 0x01) {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    ThunderboltEventCallback,
                    NULL,
                    &Event
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (
                    &gExitPmAuthProtocolGuid,
                    Event,
                    &Registration
                    );
    ASSERT_EFI_ERROR (Status);
  }
  
  return EFI_SUCCESS;
}

VOID
EFIAPI
ThunderboltEventCallback (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{
  EFI_STATUS                           Status;
  EFI_SMM_CONTROL2_PROTOCOL            *SmmControl;
  UINT8                                SmiDataValue;
  UINT8                                SmiDataSize;
//[-start-130222-IB03780481-add]//
  EFI_GLOBAL_NVS_AREA_PROTOCOL         *GlobalNvsAreaProtocol;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL      *PciRootBridgeIo;
  EFI_GLOBAL_NVS_AREA                  *GlobalNvsArea;
  UINT64                               PciAddress;
  UINT8                                PciData8;
  UINT64                               TbtHostBase;
//[-end-130222-IB03780481-add]//

  Status = gBS->CloseEvent (Event);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiSmmControl2ProtocolGuid, NULL, (VOID **)&SmmControl);
  ASSERT_EFI_ERROR (Status);

//[-start-130222-IB03780481-add]//
  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **)&PciRootBridgeIo);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsAreaProtocol);
  ASSERT_EFI_ERROR (Status);
  GlobalNvsArea = GlobalNvsAreaProtocol->Area;
//[-end-130222-IB03780481-add]//

  SmiDataValue = THUNDERBOLT_SW_SMI;
  SmiDataSize  = 1;
  Status = SmmControl->Trigger (SmmControl, &SmiDataValue, &SmiDataSize, FALSE, 0);
  ASSERT_EFI_ERROR (Status);

//[-start-130222-IB03780481-add]//
  //
  // Detect the bus number of TBT host bridge.
  //
//[-start-130709-IB05400426-modify]//
  PciAddress = EFI_PCI_ADDRESS (THUNDERBOLT_PCIE_ROOT_PORT_BUS_NUM, THUNDERBOLT_PCIE_ROOT_PORT_DEV_NUM, GlobalNvsArea->TbtRootPortNum, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET);
//[-end-130709-IB05400426-modify]//
  PciData8 = 0;
  Status = PciRootBridgeIo->Pci.Read (
                                  PciRootBridgeIo,
                                  EfiPciWidthUint8,
                                  PciAddress,
                                  1,
                                  &PciData8
                                  );

  //
  // Update Thunderbolt platform configuration
  //
  TbtHostBase = PcdGet64 (PcdPciExpressBaseAddress) + LShiftU64 (PciData8, 20);
  GlobalNvsArea->TbtHostBridgeMemoryBase = (UINT32) TbtHostBase;
//[-end-130222-IB03780481-add]//
  
  return;
}
