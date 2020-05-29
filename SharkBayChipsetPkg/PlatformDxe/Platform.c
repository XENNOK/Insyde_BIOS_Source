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

Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file:

  Platform.c

@brief:

  Platform Initialization Driver.

Revision History

--*/

#include <Platform.h>
//[-start-121108-IB10820155-remove]//
//#include <OemPlatform.h>
//[-end-121108-IB10820155-remove]//
#include <ChipsetCmos.h>

#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>

#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/SaPlatformPolicy.h>
//[-start-120822-IB10820113-modify]//
#include <Protocol/Ps2Policy.h>
//[-end-120822-IB10820113-modify]//
#include <Protocol/GlobalNvsArea.h>
//[-start-130913-IB09740046-remove]//
////[-start-120621-IB03530373-modify]//
//#include <Protocol/CRPolicy.h>
////[-end-120621-IB03530373-modify]//
//[-end-130913-IB09740046-remove]//
#include <Protocol/SmbusHc.h>
#include <Protocol/SmmSmbus.h>
//[-start-120905-IB10820116-add]//
//
// This need to remove when kernel define it.
//
//[-start-120927-IB10870028-remove]
//#define CR_UNKBC_SUPPORT_FLAG           BIT_5
//#define CR_UNKBC_SUPPORT_ENABLE         0
//#define CR_UNKBC_SUPPORT_DISABLE        BIT_5
//[-end-120927-IB10870028-remove]
//[-end-120905-IB10820116-add]//
//[-start-121108-IB10820155-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121108-IB10820155-add]//

EFI_PS2_POLICY_PROTOCOL       mPs2PolicyData = {
  EFI_KEYBOARD_NUMLOCK,
  (EFI_PS2_INIT_HARDWARE) Ps2SkipHWInit
};

EFI_GLOBAL_NVS_AREA_PROTOCOL      mGlobalNvsAreaProtocol = {0};
static
VOID
EFIAPI
UnConfigureMeReadyToBootEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
//[-start-130710-IB05160465-modify]//
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_PROTOCOL                *SetupUtility;
  CHIPSET_CONFIGURATION                     *SetupVariable = NULL;
  EFI_GUID                                  GuidId = SYSTEM_CONFIGURATION_GUID;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR(Status);

  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  SetupVariable->UnConfigureME = 0;

  Status = gRT->SetVariable (
             L"Setup",
             &GuidId,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             PcdGet32 (PcdSetupConfigSize),
             (VOID *) SetupVariable
             );
  ASSERT_EFI_ERROR ( Status );
//[-end-130710-IB05160465-modify]//

}

/**

  This is the standard EFI driver point for the Platform Driver. This
  driver is responsible for setting up any platform specific policy or
  initialization information.

  @param  ImageHandle      Handle for the image of this driver 
  @param  SystemTable      Pointer to the EFI System Table

  @retval EFI_SUCCESS      Policy decisions set 
  
**/
EFI_STATUS
EFIAPI
DxePlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        Handle;
  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility;
  CHIPSET_CONFIGURATION              *SetupVariable = NULL;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo = NULL;
  BOOLEAN                           Flag;
  EFI_GLOBAL_NVS_AREA               *GlobalNvsAreaPtr;
  UINTN                             BufferSize;
  EFI_OEM_GLOBAL_NVS_AREA           *OemGlobalNvsAreaPtr;
  EFI_BOOT_MODE                     BootMode;
  EFI_GUID                          GuidId = SYSTEM_CONFIGURATION_GUID;
  EFI_EVENT                         ReadyToBootEvent;
  UINT8                             TxtSupport;




  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **)&PciRootBridgeIo);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR(Status);

  BootMode = GetBootModeHob();
  
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

//[-start-121126-IB03780468-add]//
  //
  // Initialize Setup Configuration for Overclocking
  //
  PlatformOcInit (SetupVariable);
//[-end-121126-IB03780468-add]//

//[-start-130913-IB09740046-remove]//
//  InstallCRPolicyProtocol (ImageHandle, SetupVariable);
//[-end-130913-IB09740046-remove]//

  Status = gRT->SetVariable (
                  L"Setup",
                  &GuidId,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
//[-start-130709-IB05160465-modify]//
                  PcdGet32 (PcdSetupConfigSize),
//[-end-130709-IB05160465-modify]//
                  (VOID *) SetupVariable
                  );

  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    //
    // In the Recovery Mode,
    // need to clear the USB Status(USBSTS) in UHCI to insure no any pending SMIs during POST
    //
    ClearUhciHcsStatus(PciRootBridgeIo);
  }

  Flag = TRUE;
//[-start-121108-IB10820155-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcHookPlatformDxe (SetupVariable, PciRootBridgeIo, Flag);
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcHookPlatformDxe 1, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//

  if (Status != EFI_SUCCESS ) {
//[-end-121108-IB10820155-modify]//

    //
    // Configure GPIO for SATA
    //
    ConfigureGpioForSata(SetupVariable);

    //
    // Initialize PCH registers
    //
//[-start-121211-IB05960207-modify]//
    ProcessRegTablePci (gPchRegs, PciRootBridgeIo, NULL);
//[-end-121211-IB05960207-modify]//

    //
    // Check Chipset TXT capability
    //
    TxtSupport = ReadExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, VmxSmxFlag );
    if ( (TxtSupport & B_SMX_CPU_FLAG) && (TxtSupport & B_SMX_CHIPSET_FLAG) ) {
      SetupVariable->TXTSupport = 1;
    } else {
      SetupVariable->TXTSupport = 0;
    }

    if (SetupVariable->NumLock == 0) {
      mPs2PolicyData.KeyboardLight &= (~EFI_KEYBOARD_NUMLOCK);
    }

    //
    //
    // Install protocol to to allow access to this Policy.
    //
    Handle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
              &Handle,
              &gEfiPs2PolicyProtocolGuid,         &mPs2PolicyData,
              NULL
              );
    ASSERT_EFI_ERROR(Status);

    //
    // Install Legacy USB setup policy protocol depending upon
    // whether or not Legacy USB setup options are enabled or not.
    //
    UsbLegacyPlatformInstall();
    

    //
    // TBD or do in other places.
    // 1. Clear BIS Credentials.
    // 2. Chassis Intrusion, Date/Time, EventLog, Pnp O/S,
    // 3. Boot (BBS, USB, PXE, VGA)
    //
//[-start-121108-IB10820155-modify]//
    Flag = FALSE;
//[-start-130524-IB05160451-modify]//
    Status = OemSvcHookPlatformDxe (SetupVariable, PciRootBridgeIo, Flag);
    DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcHookPlatformDxe 2, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//
//[-end-121108-IB10820155-modify]//
   }

  BufferSize = sizeof (EFI_GLOBAL_NVS_AREA);
  Status = gBS->AllocatePool(
                  EfiACPIMemoryNVS,
                  BufferSize,
                  (VOID **)&GlobalNvsAreaPtr
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  ZeroMem (GlobalNvsAreaPtr, BufferSize);
  mGlobalNvsAreaProtocol.Area = GlobalNvsAreaPtr;

  BufferSize = sizeof (EFI_OEM_GLOBAL_NVS_AREA);
  Status = gBS->AllocatePool(
                  EfiACPIMemoryNVS,
                  BufferSize,
                  (VOID **)&OemGlobalNvsAreaPtr
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  ZeroMem (OemGlobalNvsAreaPtr, BufferSize);

  mGlobalNvsAreaProtocol.OemArea = OemGlobalNvsAreaPtr;

  
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
            &Handle,
            &gEfiGlobalNvsAreaProtocolGuid,
            &mGlobalNvsAreaProtocol,
            NULL
            );

  if (FeaturePcdGet(PcdMe5MbSupported)){ 
    if ( SetupVariable->UnConfigureME == 1) {
      Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               UnConfigureMeReadyToBootEvent,
               NULL,
               &ReadyToBootEvent
               );
     ASSERT_EFI_ERROR ( Status );
   }
 }   
  if (FeaturePcdGet(PcdMemSpdProtectionSupported)){ 
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               ProtectMemorySPD,
               NULL,
               &ReadyToBootEvent
               );
    ASSERT_EFI_ERROR ( Status );
  }
//[-start-120808-IB10820099-remove]// 
// Wait XtuDxe.c code is ready
//  if (FeaturePcdGet(PcdXtuSupported)) {
//    DxeXtuExecute (ImageHandle, SetupVariable);
//  }
//[-end-120808-IB10820099-remove]//
  return EFI_SUCCESS;
}


//[-start-130913-IB09740046-remove]//
//#define SERIAL_PORT_DEFAULT_FIFO_DEPTH    16
//EFI_CR_POLICY_PROTOCOL    mCRPolicy = {0};
//
//EFI_STATUS
//InstallCRPolicyProtocol (
//  EFI_HANDLE              Handle,
//  CHIPSET_CONFIGURATION    *SetupVariable
//  )
//{
//  EFI_STATUS              Status;
//
//  ZeroMem (&mCRPolicy, sizeof(EFI_CR_POLICY_PROTOCOL));
//  
////[-start-120217-IB06830186-modify]//
// if (FeaturePcdGet(PcdConsoleRedirectionSupported)){
////[-end-120621-IB03530373-modify]//
//  mCRPolicy.CREnable = SetupVariable->CREnable;
//  mCRPolicy.CRBaudRate = SetupVariable->CRBaudRate;
//  mCRPolicy.CRAfterPost = SetupVariable->CRAfterPost;
//  mCRPolicy.CRSerialPort = SetupVariable->CRSerialPort;
//  mCRPolicy.CRDataBits = SetupVariable->CRDataBits;
//  mCRPolicy.CRFlowControl = SetupVariable->CRFlowControl;
//  mCRPolicy.CRInfoWaitTime = SetupVariable->CRInfoWaitTime;
//  mCRPolicy.CRParity = SetupVariable->CRParity;
//  mCRPolicy.CRStopBits = SetupVariable->CRStopBits;
//  mCRPolicy.CRTerminalType = SetupVariable->CRTerminalType;
//  mCRPolicy.CRVideoType = CR_TERMINAL_VIDEO_COLOR;
//  mCRPolicy.CRShowHelp = TRUE;
//  mCRPolicy.CRHeadlessVBuffer = CR_HEADLESS_USE_VBUFFER;
//  mCRPolicy.CRFifoLength = SERIAL_PORT_DEFAULT_FIFO_DEPTH;
//  mCRPolicy.CRTerminalKey = CR_TERMINAL_KEY_SENSELESS;
//  mCRPolicy.CRComboKey = CR_TERMINAL_COMBO_KEY_ENABLE;
//  mCRPolicy.AcpiSpcr = SetupVariable->ACPISpcr;
//  mCRPolicy.CRAsyncTerm = TRUE;
////[-start-120614-IB09740003-add]//
//  mCRPolicy.CRFailSaveBaudrate = FALSE;
////[-end-120614-IB09740003-add]//
////[-start-120606-IB09740004-add]//
////[-start-120905-IB10820116-add]//
//  mCRPolicy.CRUnKbcSupport = CR_UNKBC_SUPPORT_ENABLE;
////[-end-120905-IB10820116-add]//
////[-end-120606-IB09740004-add]//
//  switch (SetupVariable->CRTextModeResolution) {
//
//    case CR_TEXT_MODE_AUTO:
//      mCRPolicy.CRForce80x25 = FALSE;    
//    default:
//      break;
//
//    case CR_TEXT_MODE_80X25:
//      mCRPolicy.CRForce80x25 = TRUE;
//      mCRPolicy.CRTerminalRows = CR_TERMINAL_ROW_25;
//      break;
//
//    case CR_TEXT_MODE_80X24DF:
//      mCRPolicy.CRForce80x25 = TRUE;      
//      mCRPolicy.CRTerminalRows = CR_TERMINAL_ROW_24;
//      mCRPolicy.CR24RowsPolicy = CR_24ROWS_POLICY_DEL_FIRST;      
//      break;
//
//    case CR_TEXT_MODE_80X24DL:
//      mCRPolicy.CRForce80x25 = TRUE;
//      mCRPolicy.CRTerminalRows = CR_TERMINAL_ROW_24;
//      mCRPolicy.CR24RowsPolicy = CR_24ROWS_POLICY_DEL_LAST;
//      break;
//  }
////[-end-120217-IB06830186-modify]//
////[-start-130719-IB09740033-add]//
//  if (mCRPolicy.CRForce80x25)
//    mCRPolicy.CRTerminalCharSet = CR_TERMINAL_CHARSET_ASCII;
//  else  
//    mCRPolicy.CRTerminalCharSet = CR_TERMINAL_CHARSET_GRAPHIC;
//	
//  mCRPolicy.CRLastCharDelete  = CR_LAST_CHAR_DELETE_DISABLE;
//  }
////[-end-130719-IB09740033-add]//
//
//  Status = gBS->InstallProtocolInterface (
//                                  &Handle,
//                                  &gEfiCRPolicyProtocolGuid,
//                                  EFI_NATIVE_INTERFACE,
//                                  &mCRPolicy
//                                );
//  return Status;  
//}
//[-end-130913-IB09740046-remove]//



EFI_STATUS
ClearUhciHcsStatus (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo
)
{
  return EFI_SUCCESS;
}

VOID
EFIAPI
ConfigureGpioForSata (
  IN CHIPSET_CONFIGURATION      *SetupVariable
  )
{
  EFI_STATUS                        Status;
  DXE_PCH_PLATFORM_POLICY_PROTOCOL  *PchPlatformPolicy;
  UINTN                             PciD31F0RegBase;
  UINT16                            GpioBase;
  UINTN                             Index;
  UINT32                            DwordReg;
//[-start-120628-IB06460410-modify]//
  UINT8                             SataResetGpio[LPTH_AHCI_MAX_PORTS] = {
//[-end-120628-IB06460410-modify]//
    PCH_GPIO_SATA_PORT0_RESET,
    PCH_GPIO_SATA_PORT1_RESET,
    PCH_GPIO_SATA_PORT2_RESET,
    PCH_GPIO_SATA_PORT3_RESET,
    PCH_GPIO_SATA_PORT4_RESET,
    PCH_GPIO_SATA_PORT5_RESET,
  };
  
  Status = gBS->LocateProtocol (&gDxePchPlatformPolicyProtocolGuid, NULL, (VOID **)&PchPlatformPolicy);
  
  PciD31F0RegBase = MmPciAddress (
                        0,
                        PchPlatformPolicy->BusNumber,
                        PCI_DEVICE_NUMBER_PCH_LPC,
                        PCI_FUNCTION_NUMBER_PCH_LPC,
                        0
                        );
  GpioBase = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_GPIO_BASE) & B_PCH_LPC_GPIO_BASE_BAR;

  if ( PchPlatformPolicy->DeviceEnabling->Sata == PCH_DEVICE_ENABLE &&
       SetupVariable->SataCnfigure != 0) {
//[-start-120628-IB06460410-modify]//
    for ( Index = 0; Index < LPTH_AHCI_MAX_PORTS; Index++ ) {
//[-end-120628-IB06460410-modify]//
      if ( PchPlatformPolicy->SataConfig->PortSettings[Index].Enable == PCH_DEVICE_ENABLE &&
           PchPlatformPolicy->SataConfig->PortSettings[Index].HotPlug == PCH_DEVICE_ENABLE &&
           PchPlatformPolicy->SataConfig->PortSettings[Index].InterlockSw == PCH_DEVICE_ENABLE ) {
        DwordReg = IoRead32 (
                    (UINTN)
                    (GpioBase + R_PCH_GPIO_USE_SEL + (SataResetGpio[Index] / 32 * (R_PCH_GPIO_USE_SEL2 - R_PCH_GPIO_USE_SEL)))
                    );
        DwordReg = DwordReg & ~(1 << SataResetGpio[Index] % 32);
        IoWrite32 (
              (UINTN)
              (GpioBase + R_PCH_GPIO_USE_SEL + (SataResetGpio[Index] / 32 * (R_PCH_GPIO_USE_SEL2 - R_PCH_GPIO_USE_SEL))),
              DwordReg
              );
      }
    }
  }
}

EFI_STATUS
EFIAPI
ProtectMemorySPD (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                       Status;
  EFI_SMBUS_HC_PROTOCOL            *SmbusController;
  DXE_PLATFORM_SA_POLICY_PROTOCOL  *DxePlatformSaPolicy;
  UINT8                            ChannelASlotMap;
  UINT8                            ChannelBSlotMap;
  BOOLEAN                          SlotPresent;
  UINT8                            Dimm;
  EFI_SMBUS_DEVICE_ADDRESS         SmbusSlaveAddress;
  UINTN                            SmbusOffset;
  UINTN                            SmbusLength;
  UINT8                            Data8;

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Protecting Memory SPD Process - Start.\n" ) );

  SmbusController       = NULL;
  DxePlatformSaPolicy   = NULL;
  ChannelASlotMap       = 0;
  ChannelBSlotMap       = 0;
  SlotPresent           = FALSE;
  Dimm                  = 0;
  SmbusOffset           = 0;
  SmbusLength           = 0;
  Data8                 = 0;

  Status = gBS->LocateProtocol ( &gEfiSmmSmbusProtocolGuid, NULL, (VOID **)&SmbusController );
  ASSERT_EFI_ERROR ( Status );

  if ( !EFI_ERROR ( Status ) ) {
    Status = gBS->LocateProtocol ( &gDxePlatformSaPolicyGuid, NULL, (VOID **)&DxePlatformSaPolicy );
    if ( EFI_ERROR ( Status ) ) {
      DxePlatformSaPolicy = NULL;
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "  Locate NB Common Policy Protocol Failure!!\n" ) );
    }

    ChannelASlotMap = 0x03;
    ChannelBSlotMap = 0x03;
    //
    // Overrided by platform policy
    //
    if ( DxePlatformSaPolicy != NULL ) {
      ChannelASlotMap = DxePlatformSaPolicy->MemoryConfig->ChannelASlotMap;
      ChannelBSlotMap = DxePlatformSaPolicy->MemoryConfig->ChannelBSlotMap;
    }

    for ( Dimm = 0 ; Dimm < MAX_SOCKETS ; Dimm = Dimm + 1 ) {
      //
      // Use channel slot map to check whether the Socket is supported in this SKU, some SKU only has 2 Sockets totally
      //
      SlotPresent = FALSE;
      if  ( Dimm < 2 ) {
        if ( ChannelASlotMap & ( 1 << Dimm ) ) {
          SlotPresent = TRUE;
        }
      } else {
        if ( ChannelBSlotMap & ( 1 << ( Dimm - 2 ) ) ) {
          SlotPresent = TRUE;
        }
      }
      if ( !SlotPresent ) {
        DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "  DIMM %x is not in channel slot map.\n", Dimm ) );

        continue;
      }

      //
      // Assign default address, then be overrided by platform policy
      //
      SmbusSlaveAddress.SmbusDeviceAddress = ( DIMM0_SPD_ADDRESS >> 1 ) + Dimm;
      if ( DxePlatformSaPolicy != NULL ) {
        SmbusSlaveAddress.SmbusDeviceAddress = ( DxePlatformSaPolicy->MemoryConfig->SpdAddressTable[Dimm] ) >> 1;
      }

      //
      // BUGBUG :
      //
      //   This step is designed by HuronRiver CRB.
      //
      //   The device can only be read with a non-protected Memory SPD.
      //
      //   Take care if your platform has different behavior.
      //
      SmbusSlaveAddress.SmbusDeviceAddress = SmbusSlaveAddress.SmbusDeviceAddress & ( ~BIT6 );
      SmbusSlaveAddress.SmbusDeviceAddress = SmbusSlaveAddress.SmbusDeviceAddress | ( BIT5 );
      SmbusOffset                          = 0x00;
      SmbusLength                          = 0x01;
      Data8                                = 0x00;
      Status = SmbusController->Execute (
                 SmbusController,
                 SmbusSlaveAddress,
                 SmbusOffset,
                 EfiSmbusReadByte,
                 FALSE,
                 &SmbusLength,
                 &Data8
                 );
      if ( !EFI_ERROR ( Status ) ) {
        Status = SmbusController->Execute (
                   SmbusController,
                   SmbusSlaveAddress,
                   SmbusOffset,
                   EfiSmbusWriteByte,
                   FALSE,
                   &SmbusLength,
                   &Data8
                   );
        ASSERT_EFI_ERROR ( Status );

        if ( EFI_ERROR ( Status ) ) {
          DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "  Protect command failure on DIMM %x.\n", Dimm ) );
        }
      } else {
        DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "  No memory plugged in DIMM %x, or it has been protected before.\n", Dimm ) );
      }
    }
  } else {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "  Locate SMBUS Protocol Failure!!\n" ) );
  }

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Protecting Memory SPD Process - Done.\n" ) );

  return EFI_SUCCESS;
}


