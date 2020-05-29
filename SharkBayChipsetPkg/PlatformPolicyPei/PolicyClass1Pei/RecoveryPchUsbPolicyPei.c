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


#include <RecoveryPchUsbPolicyPei.h>


//[-start-121120-IB03780473-modify]//
EFI_STATUS
RecoveryPchUsbPolicyCallBack (
  IN  CONST EFI_PEI_SERVICES            **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
  IN  VOID                              *Ppi
  )
//[-end-121120-IB03780473-modify]//
{
  EFI_STATUS                        Status;
  EFI_PEI_PPI_DESCRIPTOR            *PeiPchUsbPolicyPpiDesc;
  PCH_USB_POLICY_PPI                *PeiPchUsbPolicyPpi;
  PCH_USB_CONFIG                    *UsbConfig;
  UINT8                             Index;
//[-start-120917-IB03780460-add]//
  UINTN                             PortIndex;
  UINT16                            LpcDeviceId;
  PCH_SERIES                        PchSeries;
//[-end-120917-IB03780460-add]//
//[-start-121120-IB03780473-add]//
  EFI_BOOT_MODE                     BootMode;
//[-end-121120-IB03780473-add]//

  PeiPchUsbPolicyPpiDesc  = NULL;
  PeiPchUsbPolicyPpi      = NULL;
  UsbConfig               = NULL;
  Index                   = 0;

//[-start-121120-IB03780473-add]//
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

//[-start-140804-IB14090001-modify]//
  if (!(BootMode == BOOT_IN_RECOVERY_MODE || FeaturePcdGet (PcdH2OUsbPeiSupported)) ) {
    return EFI_SUCCESS;
  }
//[-end-140804-IB14090001-modify]//
//[-end-121120-IB03780473-add]//

//[-start-121113-IB06150259-add]//
  PchSeries = GetPchSeries();
//[-end-121113-IB06150259-add]//
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nRecoveryPchUsb Policy PEI Entry\n"));

  //
  // Allocate memory for RecoveryPchUsb Policy PPI and Descriptor
  //
  PeiPchUsbPolicyPpiDesc = (EFI_PEI_PPI_DESCRIPTOR *)AllocateZeroPool (sizeof (EFI_PEI_PPI_DESCRIPTOR));
  ASSERT (PeiPchUsbPolicyPpiDesc != NULL);
//[-start-130104-IB10870064-add]//
  if (PeiPchUsbPolicyPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]// 

  PeiPchUsbPolicyPpi = (PCH_USB_POLICY_PPI *)AllocateZeroPool (sizeof (PCH_USB_POLICY_PPI));
  ASSERT (PeiPchUsbPolicyPpi != NULL);
//[-start-130104-IB10870064-add]//
  if (PeiPchUsbPolicyPpi == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//

  UsbConfig = (PCH_USB_CONFIG *)AllocateZeroPool (sizeof (PCH_USB_CONFIG));
  ASSERT (UsbConfig != NULL);
//[-start-130104-IB10870064-add]//
  if (UsbConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130104-IB10870064-add]//
  
  //
  // Setting RecoveryPchUsb policy
  //
  UsbConfig->UsbPerPortCtl                 = PcdGet8 (PcdPchUsbConfigUsbPerPortCtl);
//[-start-120917-IB03780460-add]//
  //
  // Use by AMT/MEBx to enable USB-R support.
  //
//[-end-120917-IB03780460-add]//
  UsbConfig->Ehci1Usbr                     = PcdGet8 (PcdPchUsbConfigEhci1Usbr);
  UsbConfig->Ehci2Usbr                     = PcdGet8 (PcdPchUsbConfigEhci2Usbr);

  UsbConfig->PortSettings[0].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings0Enable);
  UsbConfig->PortSettings[1].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings1Enable);
  UsbConfig->PortSettings[2].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings2Enable);
  UsbConfig->PortSettings[3].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings3Enable);
  UsbConfig->PortSettings[4].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings4Enable);
  UsbConfig->PortSettings[5].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings5Enable);
  UsbConfig->PortSettings[6].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings6Enable);
  UsbConfig->PortSettings[7].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings7Enable);
  UsbConfig->PortSettings[8].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings8Enable);
  UsbConfig->PortSettings[9].Enable  = PcdGet8 (PcdPchUsbConfigPortSettings9Enable);
  UsbConfig->PortSettings[10].Enable = PcdGet8 (PcdPchUsbConfigPortSettings10Enable);
  UsbConfig->PortSettings[11].Enable = PcdGet8 (PcdPchUsbConfigPortSettings11Enable);
  UsbConfig->PortSettings[12].Enable = PcdGet8 (PcdPchUsbConfigPortSettings12Enable);
  UsbConfig->PortSettings[13].Enable = PcdGet8 (PcdPchUsbConfigPortSettings13Enable);
//[-start-120917-IB03780460-remove]//
//  //
//  // The following setting is only available for Desktop PPT and would affect
//  // the value of "USB Port[n] Initialization", RCBA + Offset 3500h-3534h.
//  // Please program it per the layout of each port on the platform board.
//  //
//  UsbConfig->PortSettings[0].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings0Panel );
//  UsbConfig->PortSettings[1].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings1Panel );
//  UsbConfig->PortSettings[2].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings2Panel );
//  UsbConfig->PortSettings[3].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings3Panel );
//  UsbConfig->PortSettings[4].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings4Panel );
//  UsbConfig->PortSettings[5].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings5Panel );
//  UsbConfig->PortSettings[6].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings6Panel );
//  UsbConfig->PortSettings[7].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings7Panel );
//  UsbConfig->PortSettings[8].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings8Panel );
//  UsbConfig->PortSettings[9].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings9Panel );
//  UsbConfig->PortSettings[10].Panel = PcdGet8 ( PcdPchUsbConfigPortSettings10Panel );
//  UsbConfig->PortSettings[11].Panel = PcdGet8 ( PcdPchUsbConfigPortSettings11Panel );
//  UsbConfig->PortSettings[12].Panel = PcdGet8 ( PcdPchUsbConfigPortSettings12Panel );
//  UsbConfig->PortSettings[13].Panel = PcdGet8 ( PcdPchUsbConfigPortSettings13Panel );
//  //
//  // The following setting is only available for Mobile PPT and would affect
//  // the value of "USB Port[n] Initialization", RCBA + Offset 3500h-3534h.
//  // Please program it per the layout of each port on the platform board.
//  //
//  UsbConfig->PortSettings[0].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings0Dock );
//  UsbConfig->PortSettings[1].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings1Dock );
//  UsbConfig->PortSettings[2].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings2Dock );
//  UsbConfig->PortSettings[3].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings3Dock );
//  UsbConfig->PortSettings[4].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings4Dock );
//  UsbConfig->PortSettings[5].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings5Dock );
//  UsbConfig->PortSettings[6].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings6Dock );
//  UsbConfig->PortSettings[7].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings7Dock );
//  UsbConfig->PortSettings[8].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings8Dock );
//  UsbConfig->PortSettings[9].Dock  = PcdGet8 ( PcdPchUsbConfigPortSettings9Dock );
//  UsbConfig->PortSettings[10].Dock = PcdGet8 ( PcdPchUsbConfigPortSettings10Dock );
//  UsbConfig->PortSettings[11].Dock = PcdGet8 ( PcdPchUsbConfigPortSettings11Dock );
//  UsbConfig->PortSettings[12].Dock = PcdGet8 ( PcdPchUsbConfigPortSettings12Dock );
//  UsbConfig->PortSettings[13].Dock = PcdGet8 ( PcdPchUsbConfigPortSettings13Dock );
//[-end-120917-IB03780460-remove]//

//[-start-120917-IB03780460-add]//
  //
  // EHCI Host Controller Enable/Disable
  //
//[-end-120917-IB03780460-add]//
  UsbConfig->Usb20Settings[0].Enable       = PcdGet8 (PcdPchUsbConfigUsb20Settings0Enable);
  UsbConfig->Usb20Settings[1].Enable       = PcdGet8 (PcdPchUsbConfigUsb20Settings1Enable);
  
  ///
  /// PCH BIOS Spec Rev 0.7.0 Section 13.1 xHCI controller options in Reference Code
  /// Please refer to Table 13-1 in PCH BIOS Spec for USB Port Operation with no xHCI
  /// pre-boot software.
  /// Please refer to Table 13-2 in PCH BIOS Spec for USB Port Operation with xHCI
  /// pre-boot software.
  ///
  /// The xHCI modes that available in BIOS are:
  /// Disabled   - forces only USB 2.0 to be supported in the OS. The xHCI controller is turned off
  ///              and hidden from the PCI space.
  /// Enabled    - allows USB 3.0 to be supported in the OS. The xHCI controller is turned on. The
  ///              shareable ports are routed to the xHCI controller. OS needs to provide drivers
  ///              to support USB 3.0.
  /// Auto       - This mode uses ACPI protocol to provide an option that enables the xHCI controller
  ///              and reroute USB ports via the _OSC ACPI method call. Note, this mode switch requires
  ///              special OS driver support for USB 3.0.
  /// Smart Auto - This mode is similar to Auto, but it adds the capability to route the ports to xHCI
  ///              or EHCI according to setting used in previous boots (for non-G3 boot) in the pre-boot
  ///              environment. This allows the use of USB 3.0 devices prior to OS boot. Note, this mode
  ///              switch requires special OS driver support for USB 3.0 and USB 3.0 software available
  ///              in the pre-boot enviroment.
//[-start-120917-IB03780460-add]//
  ///
  /// Manual Mode - For validation and experimental purposes only. Do not create setup option for end-user BIOS.
  ///
//[-end-120917-IB03780460-add]//
  /// Recommendations:
  ///  - If BIOS supports xHCI pre-boot driver then use Smart Auto mode as default
  ///  - If BIOS does not support xHCI pre-boot driver then use AUTO mode as default
  ///
  if (UsbConfig->Usb30Settings.PreBootSupport == PCH_DEVICE_ENABLE) {
    UsbConfig->Usb30Settings.Mode = PCH_XHCI_MODE_SMARTAUTO;
  } else {
    UsbConfig->Usb30Settings.Mode = PCH_XHCI_MODE_AUTO;
  }  
//[-start-121113-IB06150259-add]//
  //
  // Automatically disable EHCI when XHCI Mode is Enabled to save power.
  //
  if (UsbConfig->Usb30Settings.Mode == PCH_XHCI_MODE_ON) {
    UsbConfig->Usb20Settings[0].Enable = PCH_DEVICE_DISABLE;
    if (PchSeries == PchH) {
      UsbConfig->Usb20Settings[1].Enable = PCH_DEVICE_DISABLE;
    }
  }
//[-end-121113-IB06150259-add]//

//[-start-120917-IB03780460-add]//
  //
  // Manual Mode is for validation and experimental purposes only.
  // Do not create setup option for end-user BIOS.
  //
  UsbConfig->Usb30Settings.ManualMode      = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualMode);
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[0] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute0); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[1] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute1); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[2] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute2); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[3] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute3); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[4] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute4); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[5] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute5); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[6] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute6); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[7] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute7); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[8] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute8); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[9] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute9); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[10] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute10); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[11] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute11); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[12] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute12); 
  UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[13] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRoute13); 
  UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[0] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb30PerPinEnable0);
  UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[1] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb30PerPinEnable1);
  UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[2] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb30PerPinEnable2);
  UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[3] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb30PerPinEnable3);
  UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[4] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb30PerPinEnable4);
  UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[5] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb30PerPinEnable5);
//[-end-120917-IB03780460-add]//

//[-start-120917-IB03780460-add]//
  //
  // Set to Enable if BIOS has its own xHCI driver
  //
//[-end-120917-IB03780460-add]//
  UsbConfig->Usb30Settings.PreBootSupport  = PcdGet8 (PcdPchUsbConfigUsb30SettingsPreBootSupport);
  UsbConfig->Usb30Settings.XhciStreams     = PcdGet8 (PcdPchUsbConfigUsb30SettingsXhciStreams);

//[-start-120917-IB03780460-modify]//
  //
  // USB Port Over Current Pins mapping, please set as per board layout.
  //  
  UsbConfig->Usb20OverCurrentPins[0]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins0);
  UsbConfig->Usb20OverCurrentPins[1]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins1);
  UsbConfig->Usb20OverCurrentPins[2]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins2);
  UsbConfig->Usb20OverCurrentPins[3]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins3);
  UsbConfig->Usb20OverCurrentPins[4]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins4);
  UsbConfig->Usb20OverCurrentPins[5]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins5);
  UsbConfig->Usb20OverCurrentPins[6]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins6);
  UsbConfig->Usb20OverCurrentPins[7]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins7);
  UsbConfig->Usb20OverCurrentPins[8]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins8);
  UsbConfig->Usb20OverCurrentPins[9]   = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins9);
  UsbConfig->Usb20OverCurrentPins[10]  = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins10);
  UsbConfig->Usb20OverCurrentPins[11]  = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins11);
  UsbConfig->Usb20OverCurrentPins[12]  = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins12);
  UsbConfig->Usb20OverCurrentPins[13]  = PcdGet8 (PcdPchUsbConfigUsb20OverCurrentPins13);
//[-end-120917-IB03780460-modify]//


//[-start-120917-IB03780460-modify]//
  UsbConfig->Usb30OverCurrentPins[0]   = PcdGet8 (PcdPchUsbConfigUsb30OverCurrentPins0);
  UsbConfig->Usb30OverCurrentPins[1]   = PcdGet8 (PcdPchUsbConfigUsb30OverCurrentPins1);
  UsbConfig->Usb30OverCurrentPins[2]   = PcdGet8 (PcdPchUsbConfigUsb30OverCurrentPins2);
  UsbConfig->Usb30OverCurrentPins[3]   = PcdGet8 (PcdPchUsbConfigUsb30OverCurrentPins3);
  UsbConfig->Usb30OverCurrentPins[4]   = PcdGet8 (PcdPchUsbConfigUsb30OverCurrentPins4);
  UsbConfig->Usb30OverCurrentPins[5]   = PcdGet8 (PcdPchUsbConfigUsb30OverCurrentPins5);
//[-end-120917-IB03780460-modify]//

//[-start-120917-IB03780460-modify]//
  //
  // USB 2.0 D+/D- trace length in inchs*10 or 1000mils/10 measurement eg. 12.3" = 0x123    
  // Please set as per board layout.
  //
  UsbConfig->PortSettings[0].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength0);
  UsbConfig->PortSettings[1].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength1);
  UsbConfig->PortSettings[2].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength2);
  UsbConfig->PortSettings[3].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength3);
  UsbConfig->PortSettings[4].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength4);
  UsbConfig->PortSettings[5].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength5);
  UsbConfig->PortSettings[6].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength6);
  UsbConfig->PortSettings[7].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength7);
  UsbConfig->PortSettings[8].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength8);
  UsbConfig->PortSettings[9].Usb20PortLength        = PcdGet16 (PcdPchUsbConfigUsb20PortLength9);
  UsbConfig->PortSettings[10].Usb20PortLength       = PcdGet16 (PcdPchUsbConfigUsb20PortLength10);
  UsbConfig->PortSettings[11].Usb20PortLength       = PcdGet16 (PcdPchUsbConfigUsb20PortLength11);
  UsbConfig->PortSettings[12].Usb20PortLength       = PcdGet16 (PcdPchUsbConfigUsb20PortLength12);
  UsbConfig->PortSettings[13].Usb20PortLength       = PcdGet16 (PcdPchUsbConfigUsb20PortLength13);
//[-end-120917-IB03780460-modify]//

//[-start-120917-IB03780460-add]//
  //
  // Port Location (Front/Back Panel, Dock, MiniPcie...etc)
  //
  UsbConfig->PortSettings[0].Location  = PcdGet8 (PcdPchUsbConfigPortSettings0Location);
  UsbConfig->PortSettings[1].Location  = PcdGet8 (PcdPchUsbConfigPortSettings1Location);
  UsbConfig->PortSettings[2].Location  = PcdGet8 (PcdPchUsbConfigPortSettings2Location);
  UsbConfig->PortSettings[3].Location  = PcdGet8 (PcdPchUsbConfigPortSettings3Location);
  UsbConfig->PortSettings[4].Location  = PcdGet8 (PcdPchUsbConfigPortSettings4Location);
  UsbConfig->PortSettings[5].Location  = PcdGet8 (PcdPchUsbConfigPortSettings5Location);
  UsbConfig->PortSettings[6].Location  = PcdGet8 (PcdPchUsbConfigPortSettings6Location);
  UsbConfig->PortSettings[7].Location  = PcdGet8 (PcdPchUsbConfigPortSettings7Location);
  UsbConfig->PortSettings[8].Location  = PcdGet8 (PcdPchUsbConfigPortSettings8Location);
  UsbConfig->PortSettings[9].Location  = PcdGet8 (PcdPchUsbConfigPortSettings9Location);
  UsbConfig->PortSettings[10].Location = PcdGet8 (PcdPchUsbConfigPortSettings10Location);
  UsbConfig->PortSettings[11].Location = PcdGet8 (PcdPchUsbConfigPortSettings11Location);
  UsbConfig->PortSettings[12].Location = PcdGet8 (PcdPchUsbConfigPortSettings12Location);
  UsbConfig->PortSettings[13].Location = PcdGet8 (PcdPchUsbConfigPortSettings13Location);
//[-end-120917-IB03780460-add]//

//[-start-121211-IB06460478-modify]//
//[-start-120917-IB03780460-add]//
/*
  Guideline:
  This algorithm is move from chipset level code to board level code to allow OEM more flexibility 
  to tune the value for individual board layout electrical characteristics to pass the USB 2.0 Eye Diagram Test.
  
  IF Board=LPT-H Desktop
    For BIT[10:08] Usb20EyeDiagramTuningParam1 (PERPORTTXISET)
      IF Back Panel
        SET to 4
      ELSE 
        SET to 3
      END

    For BIT[13:11] Usb20EyeDiagramTuningParam2 (PERPORTPETXISET)
      IF Back Panel
        IF Trace Length < 7.999
           SET to 2
        ELSE IF Trace Length < 12.999
           SET to 3
        ELSE
           SET to 4
      ELSE
        SET to 2
      END
  
    For BIT[14]      
      Always SET to 0
      
  END LPT-H Desktop
  
  IF Board=LPT-H Mobile
    For BIT[10:08] Usb20EyeDiagramTuningParam1 (PERPORTTXISET)
      IF Interal Topology
        SET to 5
      ELSE
        SET to 4
        Note: If a port fail eye diagram, try set that port to 6
      END

    For BIT[13:11] Usb20EyeDiagramTuningParam2 (PERPORTPETXISET)
      IF Docking
        SET to 3
      ELSE
        SET to 2
      END

    For BIT[14]      
      Always SET to 0
  END LPT-H Mobile

  IF Board=LPT-LP
    For BIT[10:08] Usb20EyeDiagramTuningParam1 (PERPORTTXISET)
      IF Back Panel OR MiniPciE
        IF Trace Length < 7"
           SET to 5
        ELSE
           SET to 6
        ENDIF
      ELSE IF Dock
        SET to 4
      ELSE
        SET to 5
      ENDIF

    For BIT[13:11] Usb20EyeDiagramTuningParam2 (PERPORTPETXISET)
      IF Back Panel OR MiniPciE
        IF Trace Length < 10"
           SET to 2
        ELSE
           SET to 3
        ENDIF
      ELSE IF Dock
        IF Trace Length < 5"
           SET to 1
        ELSE
           SET to 2
        ENDIF
      ELSE
        SET to 2
      ENDIF

    For BIT[14]
      Always SET to 0
  END LPT-LP
*/
  //
  // USB 2.0 trace length signal strength
  //
/*
  IF Board=LPT-H Mobile
  END LPT-H Mobile
*/

//[-start-121113-IB06150259-remove]//
//  PchSeries = GetPchSeries();
//[-end-121113-IB06150259-remove]//
  LpcDeviceId = MmioRead16 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 0) + R_PCH_LPC_DEVICE_ID);
  if (PchSeries == PchH) {
    if (IS_PCH_LPT_LPC_DEVICE_ID_DESKTOP (LpcDeviceId)) {
      for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
        if (UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationBackPanel) {
          //
          // Back Panel
          //
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 4;
        } else {
          //
          // Front Panel
          //
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 3;
        }
      
        if (UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationBackPanel) {
          if (UsbConfig->PortSettings[PortIndex].Usb20PortLength < 0x80) {
            //
            // Back Panel, less than 7.9"
            //
            UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2;
          } else if (UsbConfig->PortSettings[PortIndex].Usb20PortLength < 0x130) {
            //
            // Back Panel, 8"-12.9"
            //
            UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 3;
          } else {
            //
            // Back Panel, 13" onward
            //
            UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 4;
          }
        } else {
          //
          // Front Panel
          //
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2;
        }
      }
    } else if (IS_PCH_LPT_LPC_DEVICE_ID_MOBILE (LpcDeviceId)) {
      for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
        if (UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationInternalTopology) {
          //
          // Internal Topology
          //
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 5;
        } else {
          //
          // Note: If a port fail eye diagram, try set that port to 6
          //
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 4;
        }

        if (UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationDock) {
          //
          // Docking
          //
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 3;
        } else {
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2;
        }
      }
    }
  } else if (PchSeries == PchLp) {
    for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
      if ((UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationBackPanel) || 
          (UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationMiniPciE)) {
        if (UsbConfig->PortSettings[PortIndex].Usb20PortLength < 0x70) {
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 5; //Back Panel, less than 7"
        } else {
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 6; //Back Panel, 7" onward
        }
      } else if (UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationDock) {
        UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 4; // Dock
      } else {
        UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 5; // Internal Topology
      } 

      if ((UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationBackPanel) || 
          (UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationMiniPciE)) {
        if (UsbConfig->PortSettings[PortIndex].Usb20PortLength < 0x100) {
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2; //Back Panel, less than 10"
        } else {
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 3; //Back Panel, 10" onward
        }
      } else if (UsbConfig->PortSettings[PortIndex].Location == PchUsbPortLocationDock) {
        if (UsbConfig->PortSettings[PortIndex].Usb20PortLength < 0x50) {
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 1; //Dock, less than 5"
        } else {
          UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2; //Dock, 5" onward
        }
      } else {
        UsbConfig->PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2; // Internal Topology
      }
    }
  }
//[-end-120917-IB03780460-add]//
//[-end-121211-IB06460478-modify]//

  PeiPchUsbPolicyPpi->Revision             = PCH_USB_POLICY_PPI_REVISION_1;
  PeiPchUsbPolicyPpi->UsbConfig            = UsbConfig;  
  PeiPchUsbPolicyPpi->Mode                 = PcdGet8 (PcdPchUsbPolicyPpiMode);
  PeiPchUsbPolicyPpi->EhciMemBaseAddr      = (UINTN)PcdGet32 (PcdPchEhciMemBaseAddress);
  PeiPchUsbPolicyPpi->EhciMemLength        = (UINT32)0x400 * PchEhciControllerMax;
//[-start-121116-IB10820168-modify]//
  PeiPchUsbPolicyPpi->XhciMemBaseAddr      = PcdGet32 (PcdXhciMemBaseAddress);
//[-end-121116-IB10820168-modify]//
//[-start-121224-IB10870062-modify]//
  //
  // Update Azalia Policy PPI Descriptor
  //
  PeiPchUsbPolicyPpiDesc->Flags            = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PeiPchUsbPolicyPpiDesc->Guid             = &gPchUsbPolicyPpiGuid;
  PeiPchUsbPolicyPpiDesc->Ppi              = PeiPchUsbPolicyPpi;
  
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdatePchUsbPolicy (PeiPchUsbPolicyPpi, PeiPchUsbPolicyPpiDesc);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "PeiOemSvcChipsetLib OemSvcUpdatePchUsbPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Installing RecoveryPchUsb Policy ( PEI ).\n"));
    //
    // Install PCH USB Policy PPI
    //
    Status = PeiServicesInstallPpi (PeiPchUsbPolicyPpiDesc);
    ASSERT_EFI_ERROR (Status);
  }
//[-end-121224-IB10870062-modify]//

  //
  // Dump policy
  //
  DumpRecoveryPchUsbPolicyPEI (PeiServices, PeiPchUsbPolicyPpi);

  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "RecoveryPchUsb Policy PEI Exit\n"));

  return Status;
}

VOID
DumpRecoveryPchUsbPolicyPEI (
  IN      CONST EFI_PEI_SERVICES        **PeiServices,
  IN      PCH_USB_POLICY_PPI            *PeiPchUsbPolicyPpi
  )
{
  UINTN        Count;

  Count = 0;

  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "PeiPchUsbPolicyPpi ( Address : 0x%x )\n", PeiPchUsbPolicyPpi));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Revision                               : %x\n", PeiPchUsbPolicyPpi->Revision));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-UsbConfig      ( Address : 0x%x )\n", PeiPchUsbPolicyPpi->UsbConfig));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-UsbPerPortCtl                        : %x\n", PeiPchUsbPolicyPpi->UsbConfig->UsbPerPortCtl));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-Ehci1Usbr                            : %x\n", PeiPchUsbPolicyPpi->UsbConfig->Ehci1Usbr)); 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-Ehci2Usbr                            : %x\n", PeiPchUsbPolicyPpi->UsbConfig->Ehci2Usbr)); 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-RsvdBits                             : %x\n", PeiPchUsbPolicyPpi->UsbConfig->RsvdBits)); 
//[-start-120628-IB06460410-modify]//
  for (Count = 0; Count < LPTH_USB_MAX_PHYSICAL_PORTS; Count++ ) {
//[-end-120628-IB06460410-modify]//
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-PortSettings[%2x].Enable              : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->PortSettings[Count].Enable));
//[-start-120917-IB03780460-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " | |-PortSettings[%2x].Panel               : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->PortSettings[Count].Panel) );
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " | |-PortSettings[%2x].Dock                : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->PortSettings[Count].Dock) );
//[-end-120917-IB03780460-remove]//
//[-start-120917-IB03780460-modify]//
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-PortSettings[%2x].Location            : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->PortSettings[Count].Location));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-PortSettings[%2x].Usb20PortLength     : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->PortSettings[Count].Usb20PortLength));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-PortSettings[%2x].Usb20EyeDiagramTuningParam1 : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->PortSettings[Count].Usb20EyeDiagramTuningParam1));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-PortSettings[%2x].Usb20EyeDiagramTuningParam2 : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->PortSettings[Count].Usb20EyeDiagramTuningParam2));
//[-end-120917-IB03780460-modify]//
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-PortSettings[%2x].Rsvdbits            : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->PortSettings[Count].Rsvdbits));
  }
  for (Count = 0; Count < PchEhciControllerMax; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-Usb20Settings[%2x].Enable             : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->Usb20Settings[Count].Enable));
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-Usb20Settings[%2x].Rsvdbits           : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->Usb20Settings[Count].Rsvdbits));
  }  
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-Usb30Settings( Address : 0x%x )\n", PeiPchUsbPolicyPpi->UsbConfig->Usb30Settings));   
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | | |-Mode                               : %x\n", PeiPchUsbPolicyPpi->UsbConfig->Usb30Settings.Mode));   
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | | |-PreBootSupport                     : %x\n", PeiPchUsbPolicyPpi->UsbConfig->Usb30Settings.PreBootSupport));   
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | | |-XhciStreams                        : %x\n", PeiPchUsbPolicyPpi->UsbConfig->Usb30Settings.XhciStreams));   
//[-start-120917-IB03780460-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | | |-ManualMode                         : %x\n", PeiPchUsbPolicyPpi->UsbConfig->Usb30Settings.ManualMode));
//[-end-120917-IB03780460-add]//
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | | |-Rsvdbits                           : %x\n", PeiPchUsbPolicyPpi->UsbConfig->Usb30Settings.Rsvdbits));    
//[-start-120917-IB03780460-add]//
  for (Count = 0; Count < LPTH_USB_MAX_PHYSICAL_PORTS; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | | |-ManualModeUsb20PerPinRoute[%2x]    : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[Count]));
  } 
  for (Count = 0; Count < LPTH_XHCI_MAX_USB3_PORTS; Count++) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | | |-ManualModeUsb30PerPinEnable[%2x]   : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[Count]));
  } 
//[-end-120917-IB03780460-add]//
//[-start-120628-IB06460410-modify]//
  for (Count = 0; Count < LPTH_USB_MAX_PHYSICAL_PORTS; Count++) {
//[-end-120628-IB06460410-modify]//
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-Usb20OverCurrentPins[%2x]             : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->Usb20OverCurrentPins[Count]));
  } 
//[-start-120628-IB06460410-modify]//
  for (Count = 0; Count < LPTH_XHCI_MAX_USB3_PORTS; Count++) {
//[-end-120628-IB06460410-modify]//
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, " | |-Usb30OverCurrentPins[%2x]             : %x\n", Count, PeiPchUsbPolicyPpi->UsbConfig->Usb30OverCurrentPins[Count]));
  } 
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-Mode                                   : %x\n", PeiPchUsbPolicyPpi->Mode));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-EhciMemBaseAddr                        : %x\n", PeiPchUsbPolicyPpi->EhciMemBaseAddr));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-EhciMemLength                          : %x\n", PeiPchUsbPolicyPpi->EhciMemLength));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, " |-XhciMemBaseAddr                        : %x\n", PeiPchUsbPolicyPpi->XhciMemBaseAddr));  
  
  DEBUG ((EFI_D_INFO, " |\n"));

  return;
}
