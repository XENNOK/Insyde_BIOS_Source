/** @file
  Dump whole DXE_PCH_PLATFORM_POLICY_PROTOCOL and serial out.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include "PchInit.h"

/**
  Dump whole DXE_PCH_PLATFORM_POLICY_PROTOCOL and serial out.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval None
**/
VOID
PchDumpPlatformProtocol (
  IN  DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy
  )
{
#ifdef EFI_DEBUG
  UINT8 i;

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH Dump platform protocol Start -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH PLATFORM POLICY Revision= %x\n", PchPlatformPolicy->Revision));
  DEBUG ((EFI_D_INFO, " PCH PLATFORM POLICY BusNumber= %x\n", PchPlatformPolicy->BusNumber));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_DEVICE_ENABLE -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_DEVICE_ENABLE Lan= %x\n", PchPlatformPolicy->DeviceEnabling->Lan));
  DEBUG ((EFI_D_INFO, " PCH_DEVICE_ENABLE Azalia= %x\n", PchPlatformPolicy->DeviceEnabling->Azalia));
  DEBUG ((EFI_D_INFO, " PCH_DEVICE_ENABLE Sata= %x\n", PchPlatformPolicy->DeviceEnabling->Sata));
  DEBUG ((EFI_D_INFO, " PCH_DEVICE_ENABLE Smbus= %x\n", PchPlatformPolicy->DeviceEnabling->Smbus));
  DEBUG ((EFI_D_INFO, " PCH_DEVICE_ENABLE PciClockRun= %x\n", PchPlatformPolicy->DeviceEnabling->PciClockRun));
  DEBUG ((EFI_D_INFO, " PCH_DEVICE_ENABLE Display= %x\n", PchPlatformPolicy->DeviceEnabling->Display));
  DEBUG ((EFI_D_INFO, " PCH_DEVICE_ENABLE Crid%x\n", PchPlatformPolicy->DeviceEnabling->Crid));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_USB_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG UsbPerPortCtl= %x\n", PchPlatformPolicy->UsbConfig->UsbPerPortCtl));
  DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Ehci1Usbr= %x\n", PchPlatformPolicy->UsbConfig->Ehci1Usbr));
  DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Ehci2Usbr= %x\n", PchPlatformPolicy->UsbConfig->Ehci2Usbr));
  for (i = 0; i < GetPchUsbMaxPhysicalPortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG PortSettings[%d] Enabled= %x\n", i, PchPlatformPolicy->UsbConfig->PortSettings[i].Enable));
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG PortSettings[%d] Location= %x\n", i, PchPlatformPolicy->UsbConfig->PortSettings[i].Location));
  }

  for (i = 0; i < GetPchXhciMaxUsb3PortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Port30Settings[%d] Enabled= %x\n", i, PchPlatformPolicy->UsbConfig->Port30Settings[i].Enable));
  }

  for (i = 0; i < GetPchEhciMaxControllerNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb20Settings[%d] Enabled= %x\n", i, PchPlatformPolicy->UsbConfig->Usb20Settings[i].Enable));
  }

  DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.Mode= %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.Mode));
  DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.PreBootSupport= %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.PreBootSupport));
  DEBUG ((EFI_D_INFO, " XhciStreams is obsoleted, it doesn't effect any setting change since Revision 2.\n"));
  DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.ManualMode= %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.ManualMode));
  DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.XhciIdleL1= %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.XhciIdleL1));

  for (i = 0; i < GetPchUsbMaxPhysicalPortNum (); i++) {
    if (PchPlatformPolicy->UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[i] == 0) {
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.ManualModeUsb20PerPinRoute[%d]= EHCI\n", i));
    } else {
      DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30Settings.ManualModeUsb20PerPinRoute[%d]= XHCI\n", i));
    }
  }

  for (i = 0; i < GetPchXhciMaxUsb3PortNum (); i++) {
    DEBUG ((EFI_D_INFO,
            "PCH_USB_CONFIG Usb30Settings.ManualModeUsb30PerPinEnable[%d]= %x\n",
            i,
            PchPlatformPolicy->UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[i]));
  }

  for (i = 0; i < GetPchUsbMaxPhysicalPortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb20OverCurrentPins[%d]= OC%x\n", i, PchPlatformPolicy->UsbConfig->Usb20OverCurrentPins[i]));
  }

  for (i = 0; i < GetPchXhciMaxUsb3PortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb30OverCurrentPins[%d]= OC%x\n", i, PchPlatformPolicy->UsbConfig->Usb30OverCurrentPins[i]));
  }

  for (i = 0; i < GetPchEhciMaxUsbPortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_USB_CONFIG Usb20PortLength[%d]= %x.%0x\n", i, PchPlatformPolicy->UsbConfig->PortSettings[i].Usb20PortLength >> 4, PchPlatformPolicy->UsbConfig->PortSettings[i].Usb20PortLength & 0xF));
  }

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_PCI_EXPRESS_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG TempRootPortBusNumMin= %x\n", PchPlatformPolicy->PciExpressConfig->TempRootPortBusNumMin));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG TempRootPortBusNumMax= %x\n", PchPlatformPolicy->PciExpressConfig->TempRootPortBusNumMax));
  for (i = 0; i < GetPchMaxPciePortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] Enabled= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].Enable));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] Hide= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].Hide));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] SlotImplemented= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].SlotImplemented));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] HotPlug= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].HotPlug));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] PmSci= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].PmSci));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] ExtSync= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].ExtSync));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] UnsupportedRequestReport= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].UnsupportedRequestReport));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] FatalErrorReport= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].FatalErrorReport));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] NoFatalErrorReport= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].NoFatalErrorReport));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] CorrectableErrorReport= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].CorrectableErrorReport));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] PmeInterrupt= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].PmeInterrupt));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] SystemErrorOnFatalError= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].SystemErrorOnFatalError));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] SystemErrorOnNonFatalError= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].SystemErrorOnNonFatalError));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] SystemErrorOnCorrectableError= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].SystemErrorOnCorrectableError));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] AdvancedErrorReporting= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].AdvancedErrorReporting));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] TransmitterHalfSwing= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].TransmitterHalfSwing));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] FunctionNumber= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].FunctionNumber));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] PhysicalSlotNumber= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].PhysicalSlotNumber));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] CompletionTimeout= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].CompletionTimeout));
    DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] Aspm= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].Aspm));
    if (PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_2) {
      DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPort[%d] L1Substates= %x\n", i, PchPlatformPolicy->PciExpressConfig->RootPort[i].L1Substates));
    }
  }

  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG NumOfDevAspmOverride= %x\n", PchPlatformPolicy->PciExpressConfig->NumOfDevAspmOverride));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG DevAspmOverride VendorId= %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->VendorId));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG DevAspmOverride DeviceId= %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->DeviceId));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG DevAspmOverride RevId= %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->RevId));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG DevAspmOverride BaseClassCode= %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->BaseClassCode));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG DevAspmOverride SubClassCode= %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->SubClassCode));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG DevAspmOverride EndPointAspm= %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->EndPointAspm));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG PchPcieSbdePort= %x\n", PchPlatformPolicy->PciExpressConfig->PchPcieSbdePort));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG RootPortClockGating= %x\n", PchPlatformPolicy->PciExpressConfig->RootPortClockGating));
  DEBUG ((EFI_D_INFO, " PCH_PCI_EXPRESS_CONFIG EnableSubDecode= %x\n", PchPlatformPolicy->PciExpressConfig->EnableSubDecode));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_SATA_CONFIG -----------------\n"));
  for (i = 0; i < GetPchMaxSataPortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] Enabled= %x\n", i, PchPlatformPolicy->SataConfig->PortSettings[i].Enable));
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] HotPlug= %x\n", i, PchPlatformPolicy->SataConfig->PortSettings[i].HotPlug));
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] InterlockSw= %x\n", i, PchPlatformPolicy->SataConfig->PortSettings[i].InterlockSw));
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] External= %x\n", i, PchPlatformPolicy->SataConfig->PortSettings[i].External));
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] SpinUp= %x\n", i, PchPlatformPolicy->SataConfig->PortSettings[i].SpinUp));
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] SolidStateDrive= %x\n", i, PchPlatformPolicy->SataConfig->PortSettings[i].SolidStateDrive));
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] EnableDitoConfig= %x\n", PchPlatformPolicy->SataConfig->PortSettings[i].EnableDitoConfig));
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] DmVal= %x\n", PchPlatformPolicy->SataConfig->PortSettings[i].DmVal));
    DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG PortSettings[%d] DitoVal= %x\n", PchPlatformPolicy->SataConfig->PortSettings[i].DitoVal));
  }

  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG RaidAlternateId= %x\n", PchPlatformPolicy->SataConfig->RaidAlternateId));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG Raid0= %x\n", PchPlatformPolicy->SataConfig->Raid0));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG Raid1= %x\n", PchPlatformPolicy->SataConfig->Raid1));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG Raid10= %x\n", PchPlatformPolicy->SataConfig->Raid10));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG Raid5= %x\n", PchPlatformPolicy->SataConfig->Raid5));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG Irrt= %x\n", PchPlatformPolicy->SataConfig->Irrt));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG OromUiBanner= %x\n", PchPlatformPolicy->SataConfig->OromUiBanner));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG HddUnlock= %x\n", PchPlatformPolicy->SataConfig->HddUnlock));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG LedLocate= %x\n", PchPlatformPolicy->SataConfig->LedLocate));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG IrrtOnly= %x\n", PchPlatformPolicy->SataConfig->IrrtOnly));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG TestMode= %x\n", PchPlatformPolicy->SataConfig->TestMode));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG SalpSupport= %x\n", PchPlatformPolicy->SataConfig->SalpSupport));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG LegacyMode= %x\n", PchPlatformPolicy->SataConfig->LegacyMode));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG SmartStorage= %x\n", PchPlatformPolicy->SataConfig->SmartStorage));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG OromUiDelay= %x\n", PchPlatformPolicy->SataConfig->OromUiDelay));
  DEBUG ((EFI_D_INFO, " PCH_SATA_CONFIG SpeedSupport= %x\n", PchPlatformPolicy->SataConfig->SpeedSupport));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_AZALIA_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG Pme= %x\n", PchPlatformPolicy->AzaliaConfig->Pme));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG DS= %x\n", PchPlatformPolicy->AzaliaConfig->DS));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG DA= %x\n", PchPlatformPolicy->AzaliaConfig->DA));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG AzaliaVerbTableNum= %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTableNum));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG AzaliaVerbTable Header VendorDeviceId= %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTable->VerbTableHeader.VendorDeviceId));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG AzaliaVerbTable Header SubSystemId= %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTable->VerbTableHeader.SubSystemId));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG AzaliaVerbTable Header RevisionId= %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTable->VerbTableHeader.RevisionId));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG AzaliaVerbTable Header FrontPanelSupport= %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTable->VerbTableHeader.FrontPanelSupport));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG AzaliaVerbTable Header NumberOfRearJacks= %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTable->VerbTableHeader.NumberOfRearJacks));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG AzaliaVerbTable Header NumberOfFrontJacks= %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTable->VerbTableHeader.NumberOfFrontJacks));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG AzaliaVerbTable VerbTableData= %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTable->VerbTableData));
  DEBUG ((EFI_D_INFO, " PCH_AZALIA_CONFIG ResetWaitTimer= %x\n", PchPlatformPolicy->AzaliaConfig->ResetWaitTimer));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_SMBUS_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_SMBUS_CONFIG NumRsvdSmbusAddresses= %x\n", PchPlatformPolicy->SmbusConfig->NumRsvdSmbusAddresses));
  DEBUG ((EFI_D_INFO, " PCH_SMBUS_CONFIG RsvdSmbusAddressTable= %x\n", PchPlatformPolicy->SmbusConfig->RsvdSmbusAddressTable));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_MISC_PM_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PowerResetStatusClear MeWakeSts= %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeWakeSts));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PowerResetStatusClear MeHrstColdSts= %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHrstColdSts));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PowerResetStatusClear MeHrstWarmSts= %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHrstWarmSts));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PowerResetStatusClear MeHostPowerDn= %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHostPowerDn));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PowerResetStatusClear WolOvrWkSts= %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.WolOvrWkSts));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG WakeConfig PmeB0S5Dis= %x\n", PchPlatformPolicy->MiscPmConfig->WakeConfig.PmeB0S5Dis));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG WakeConfig WolEnableOverride= %x\n", PchPlatformPolicy->MiscPmConfig->WakeConfig.WolEnableOverride));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG WakeConfig Gp27WakeFromDeepSx= %x\n", PchPlatformPolicy->MiscPmConfig->WakeConfig.Gp27WakeFromDeepSx));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PchDeepSxPol= %x\n", PchPlatformPolicy->MiscPmConfig->PchDeepSxPol));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PchSlpS3MinAssert= %x\n", PchPlatformPolicy->MiscPmConfig->PchSlpS3MinAssert));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PchSlpS4MinAssert= %x\n", PchPlatformPolicy->MiscPmConfig->PchSlpS4MinAssert));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PchSlpSusMinAssert= %x\n", PchPlatformPolicy->MiscPmConfig->PchSlpSusMinAssert));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PchSlpAMinAssert= %x\n", PchPlatformPolicy->MiscPmConfig->PchSlpAMinAssert));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG SlpStrchSusUp= %x\n", PchPlatformPolicy->MiscPmConfig->SlpStrchSusUp));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG SlpLanLowDc= %x\n", PchPlatformPolicy->MiscPmConfig->SlpLanLowDc));
  DEBUG ((EFI_D_INFO, " PCH_MISC_PM_CONFIG PchPwrCycDur= %x\n", PchPlatformPolicy->MiscPmConfig->PchPwrCycDur));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_IO_APIC_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_IO_APIC_CONFIG BdfValid= %x\n", PchPlatformPolicy->IoApicConfig->BdfValid));
  DEBUG ((EFI_D_INFO, " PCH_IO_APIC_CONFIG BusNumber= %x\n", PchPlatformPolicy->IoApicConfig->BusNumber));
  DEBUG ((EFI_D_INFO, " PCH_IO_APIC_CONFIG DeviceNumber= %x\n", PchPlatformPolicy->IoApicConfig->DeviceNumber));
  DEBUG ((EFI_D_INFO, " PCH_IO_APIC_CONFIG FunctionNumber= %x\n", PchPlatformPolicy->IoApicConfig->FunctionNumber));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_DEFAULT_SVID_SID -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_DEFAULT_SVID_SID SubSystemVendorId= %x\n", PchPlatformPolicy->DefaultSvidSid->SubSystemVendorId));
  DEBUG ((EFI_D_INFO, " PCH_DEFAULT_SVID_SID SubSystemId= %x\n", PchPlatformPolicy->DefaultSvidSid->SubSystemId));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_LOCK_DOWN_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_LOCK_DOWN_CONFIG GlobalSmi= %x\n", PchPlatformPolicy->LockDownConfig->GlobalSmi));
  DEBUG ((EFI_D_INFO, " PCH_LOCK_DOWN_CONFIG BiosInterface= %x\n", PchPlatformPolicy->LockDownConfig->BiosInterface));
  DEBUG ((EFI_D_INFO, " PCH_LOCK_DOWN_CONFIG GpioLockDown= %x\n", PchPlatformPolicy->LockDownConfig->GpioLockDown));
  DEBUG ((EFI_D_INFO, " PCH_LOCK_DOWN_CONFIG RtcLock= %x\n", PchPlatformPolicy->LockDownConfig->RtcLock));
  DEBUG ((EFI_D_INFO, " PCH_LOCK_DOWN_CONFIG BiosLock= %x\n", PchPlatformPolicy->LockDownConfig->BiosLock));
  DEBUG ((EFI_D_INFO, " PCH_LOCK_DOWN_CONFIG PchBiosLockIoTrapAddress= %x\n", PchPlatformPolicy->LockDownConfig->PchBiosLockIoTrapAddress));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_THERMAL_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG ThermalAlertEnable TselLock %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TselLock));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG ThermalAlertEnable TscLock %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TscLock));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG ThermalAlertEnable TsmicLock= %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TsmicLock));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG ThermalAlertEnable PhlcLock= %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.PhlcLock));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG ThermalDeviceEnable (D31:F6) %x\n", PchPlatformPolicy->ThermalConfig->ThermalDeviceEnable));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING THERMAL_THROTTLE_LEVELS T0Level %x centigrade degree\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T0Level));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING THERMAL_THROTTLE_LEVELS T1Level %x centigrade degree\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T1Level));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING THERMAL_THROTTLE_LEVELS T2Level %x centigrade degree\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T2Level));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING THERMAL_THROTTLE_LEVELS TTEnable %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTEnable));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING THERMAL_THROTTLE_LEVELS TTState13Enable %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTState13Enable));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING THERMAL_THROTTLE_LEVELS TTLock %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTLock));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING THERMAL_THROTTLE_LEVELS SuggestedSetting %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.SuggestedSetting));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING DMI_HW_WIDTH_CONTROL DmiTsawEn %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.DmiTsawEn));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING DMI_HW_WIDTH_CONTROL TS0TW %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS0TW));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING DMI_HW_WIDTH_CONTROL TS1TW %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS1TW));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING DMI_HW_WIDTH_CONTROL TS2TW %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS2TW));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING DMI_HW_WIDTH_CONTROL TS3TW %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS3TW));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING DMI_HW_WIDTH_CONTROL SuggestedSetting %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.SuggestedSetting));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P0T1M %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T1M));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P0T2M %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T2M));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P0T3M %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T3M));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P0TDisp %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0TDisp));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P0Tinact %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0Tinact));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P0TDispFinit %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0TDispFinit));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P1T1M %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T1M));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P1T2M %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T2M));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P1T3M %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T3M));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P1TDisp %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1TDisp));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P1Tinact %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1Tinact));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE P1TDispFinit %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1TDispFinit));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PCH_THERMAL_THROTTLING SATA_THERMAL_THROTTLE SuggestedSetting %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.SuggestedSetting));
  DEBUG ((EFI_D_INFO, " PCH_THERMAL_CONFIG PchHotLevel = %x\n", PchPlatformPolicy->ThermalConfig->PchHotLevel));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_LPC_HPET_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_LPC_HPET_CONFIG HpetConfig %x\n", PchPlatformPolicy->HpetConfig->BdfValid));
  for (i = 0; i < PCH_HPET_BDF_MAX; i++) {
    DEBUG ((EFI_D_INFO, " PCH_LPC_HPET_CONFIG Hpet[%d] BusNumber %x\n", i, PchPlatformPolicy->HpetConfig->Hpet[i].BusNumber));
    DEBUG ((EFI_D_INFO, " PCH_LPC_HPET_CONFIG Hpet[%d] DeviceNumber %x\n", i, PchPlatformPolicy->HpetConfig->Hpet[i].DeviceNumber));
    DEBUG ((EFI_D_INFO, " PCH_LPC_HPET_CONFIG Hpet[%d] FunctionNumber %x\n", i, PchPlatformPolicy->HpetConfig->Hpet[i].FunctionNumber));
  }

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_LPC_SIRQ_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_LPC_SIRQ_CONFIG SirqEnable= %x\n", PchPlatformPolicy->SerialIrqConfig->SirqEnable));
  DEBUG ((EFI_D_INFO, " PCH_LPC_SIRQ_CONFIG SirqMode= %x\n", PchPlatformPolicy->SerialIrqConfig->SirqMode));
  DEBUG ((EFI_D_INFO, " PCH_LPC_SIRQ_CONFIG StartFramePulse= %x\n", PchPlatformPolicy->SerialIrqConfig->StartFramePulse));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_DMI_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_DMI_CONFIG DmiAspm= %x\n", PchPlatformPolicy->DmiConfig->DmiAspm));
  DEBUG ((EFI_D_INFO, " PCH_DMI_CONFIG DmiExtSync= %x\n", PchPlatformPolicy->DmiConfig->DmiExtSync));
  DEBUG ((EFI_D_INFO, " PCH_DMI_CONFIG DmiIot= %x\n", PchPlatformPolicy->DmiConfig->DmiIot));

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH_PWR_OPT_CONFIG -----------------\n"));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG PchPwrOptDmi= %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptDmi));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG PchPwrOptGbe= %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptGbe));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG PchPwrOptXhci= %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptXhci));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG PchPwrOptEhci= %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptEhci));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG PchPwrOptSata= %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptSata));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG MemCloseStateEn= %x\n", PchPlatformPolicy->PwrOptConfig->MemCloseStateEn));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG InternalObffEn= %x\n", PchPlatformPolicy->PwrOptConfig->InternalObffEn));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG ExternalObffEn= %x\n", PchPlatformPolicy->PwrOptConfig->ExternalObffEn));
  for (i = 0; i < GetPchMaxPciePortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] LtrEnable= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].LtrEnable));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] ObffEnable= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].ObffEnable));
  }
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG NumOfDevLtrOverride= %x\n", PchPlatformPolicy->PwrOptConfig->NumOfDevLtrOverride));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG DevLtrOverride VendorId= %x\n", PchPlatformPolicy->PwrOptConfig->DevLtrOverride->VendorId));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG DevLtrOverride DeviceId= %x\n", PchPlatformPolicy->PwrOptConfig->DevLtrOverride->DeviceId));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG DevLtrOverride RevId= %x\n", PchPlatformPolicy->PwrOptConfig->DevLtrOverride->RevId));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG DevLtrOverride SnoopLatency= %x\n", PchPlatformPolicy->PwrOptConfig->DevLtrOverride->SnoopLatency));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG DevLtrOverride NonSnoopLatency= %x\n", PchPlatformPolicy->PwrOptConfig->DevLtrOverride->NonSnoopLatency));
  DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG LegacyDmaDisable= %x\n", PchPlatformPolicy->PwrOptConfig->LegacyDmaDisable));

  for (i = 0; i < GetPchMaxPciePortNum (); i++) {
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] LtrConfigLock= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].LtrConfigLock));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] LtrMaxSnoopLatency = %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i]. LtrMaxSnoopLatency));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] LtrMaxNoSnoopLatency = %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].LtrMaxNoSnoopLatency));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] SnoopLatencyOverrideMode= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].SnoopLatencyOverrideMode));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] SnoopLatencyOverrideMultiplier= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].SnoopLatencyOverrideMultiplier));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] SnoopLatencyOverrideValue= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].SnoopLatencyOverrideValue));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] NonSnoopLatencyOverrideMode= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].NonSnoopLatencyOverrideMode));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] NonSnoopLatencyOverrideMultiplier= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].NonSnoopLatencyOverrideMultiplier));
    DEBUG ((EFI_D_INFO, " PCH_PWR_OPT_CONFIG RootPort[%d] NonSnoopLatencyOverrideValue= %x\n", i, PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[i].NonSnoopLatencyOverrideValue));
  }
  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, "------------------------ PCH Dump platform protocol End -----------------\n"));
  DEBUG ((EFI_D_INFO, "\n"));
#endif
}
