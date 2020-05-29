//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include <PchPolicy.h>

#define PCI_CLASS_NETWORK             0x02
#define PCI_CLASS_NETWORK_ETHERNET    0x00
#define PCI_CLASS_NETWORK_OTHER       0x80
//[-start-130205-IB02950497-add]//
#define MANUAL_DISABLE                0
#define MANUAL_ENABLE                 1
#define PER_PIN                       0
#define ALL_PINS_EHCI                 1              
#define ALL_PINS_XHCI                 2
#define ALL_PINS_DISABLE              1              
#define ALL_PINS_ENABLE               2
//[-end-130205-IB02950497-add]//
//
// PCH Platform Policy Data
//
DXE_PCH_PLATFORM_POLICY_PROTOCOL  mPchPolicyData       = { 0 };
PCH_DEVICE_ENABLING               mPchDeviceEnabling   = { 0 };
PCH_USB_CONFIG                    mPchUsbConfig        = { 0 };
PCH_PCI_EXPRESS_CONFIG            mPchPciExpressConfig = { 0 };
PCH_SATA_CONFIG                   mPchSataConfig       = { 0 };
PCH_AZALIA_CONFIG                 mPchAzaliaConfig     = { 0 };
PCH_SMBUS_CONFIG                  mPchSmbusConfig      = { 0 };
PCH_MISC_PM_CONFIG                mPchMiscPmConfig     = { 0 };
PCH_IO_APIC_CONFIG                mPchIoApicConfig     = { 0 };
PCH_DEFAULT_SVID_SID              mPchDefaultSvidSid   = { 0 };
PCH_LOCK_DOWN_CONFIG              mPchLockDownConfig   = { 0 };
PCH_THERMAL_CONFIG                mPchThermalConfig    = { 0 };
PCH_LPC_HPET_CONFIG               mPchHpetConfig       = { 0 };
PCH_LPC_SIRQ_CONFIG               mSerialIrqConfig     = { 0 };
PCH_DMI_CONFIG                    mPchDmiConfig        = { 0 };
PCH_PWR_OPT_CONFIG                mPchPwrOptConfig     = { 0 };
PCH_MISC_CONFIG                   mPchMiscConfig       = { 0 };
//[-start-120807-IB04770241-modify]//
//[-start-120628-IB06460410-add]//
PCH_AUDIO_DSP_CONFIG              mAudioDspConfig      = { 0 };
//[-start-121113-IB06150259-modify]//
PCH_SERIAL_IO_CONFIG              mSerialIoConfig      = { 0 };
//[-end-121113-IB06150259-modify]//
//[-end-120628-IB06460410-add]//
//[-end-120807-IB04770241-modify]//

static UINT8 mSmbusRsvdAddresses[PLATFORM_NUM_SMBUS_RSVD_ADDRESSES] = {
  SMBUS_ADDR_CH_A_1,
  SMBUS_ADDR_CH_A_2,
  SMBUS_ADDR_CH_B_1,
  SMBUS_ADDR_CH_B_2
};

//[-start-121211-IB06460478-modify]//
//[-start-120628-IB06460410-modify]//
PCH_PCIE_DEVICE_ASPM_OVERRIDE mDevAspmOverride[] = {
  //
  // Intel PRO/Wireless
  //
  {0x8086, 0x422b, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x422c, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x4238, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x4239, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  //
  // Intel WiMAX/WiFi Link
  //
  {0x8086, 0x0082, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0085, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0083, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0084, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0086, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0087, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0088, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0089, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x008F, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0090, 0xff, 0xff, 0xff, PchPcieAspmL1, PchPcieL1L2Override, 0xFFFF, 0xFFFFFFFF},
  //
  // Intel Crane Peak WLAN NIC
  //
  {0x8086, 0x08AE, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x08AF, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel Crane Peak w/BT WLAN NIC
  //
  {0x8086, 0x0896, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0897, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel Kelsey Peak WiFi, WiMax
  //
  {0x8086, 0x0885, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0886, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel Centrino Wireless-N 105
  //
  {0x8086, 0x0894, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0895, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel Centrino Wireless-N 135
  //
  {0x8086, 0x0892, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0893, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel Centrino Wireless-N 2200
  //
  {0x8086, 0x0890, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0891, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel Centrino Wireless-N 2230
  //
  {0x8086, 0x0887, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x0888, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel Centrino Wireless-N 6235
  //
  {0x8086, 0x088E, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x088F, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel CampPeak 2 Wifi
  //
  {0x8086, 0x08B5, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  {0x8086, 0x08B6, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF},
  //
  // Intel WilkinsPeak 1 Wifi
  //
  {0x8086, 0x08B3, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003},
  {0x8086, 0x08B3, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1SubstatesOverride,        0x0158, 0x00000003},
  {0x8086, 0x08B4, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003},
  {0x8086, 0x08B4, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1SubstatesOverride,        0x0158, 0x00000003},
  //
  // Intel Wilkins Peak 2 Wifi
  //
//  {0x8086, 0x08B1, 0x3a, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmDisabled, PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003},
//  {0x8086, 0x08B1, 0x3a, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmDisabled, PchPcieL1SubstatesOverride,        0x0158, 0x00000003},
  {0x8086, 0x08B1, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003},
  {0x8086, 0x08B1, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1SubstatesOverride,        0x0158, 0x00000003},
  {0x8086, 0x08B2, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2AndL1SubstatesOverride, 0x0154, 0x00000003},
  {0x8086, 0x08B2, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1SubstatesOverride,        0x0158, 0x00000003},
  //
  // Intel Wilkins Peak PF Wifi
  //
  {0x8086, 0x08B0, 0xff, PCI_CLASS_NETWORK, PCI_CLASS_NETWORK_OTHER, PchPcieAspmL1,       PchPcieL1L2Override,               0xFFFF, 0xFFFFFFFF}
};
//[-end-120628-IB06460410-modify]//

//
// Function implementations
//

/**
  Initilize Intel PCH DXE Platform Policy

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
PCHPolicyEntry (
  IN EFI_HANDLE              ImageHandle,
  IN EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                             Status;
  EFI_HANDLE                             Handle;
  UINT8                                  PortIndex;
  EFI_SETUP_UTILITY_PROTOCOL             *SetupUtility;
  CHIPSET_CONFIGURATION                   *SetupVariable;
  PLATFORM_HARDWARE_SWITCH               *PlatformHardwareSwitch;
  EFI_HOB_GUID_TYPE                      *GuidHob;
  EFI_PEI_HOB_POINTERS                   Hob;
  VOID                                   *HobList;
//[-start-120628-IB06460410-add]//
  PCH_SERIES                             PchSeries;
//[-end-120628-IB06460410-add]//
//[-start-120917-IB03780460-add]//
  UINT16                                 LpcDeviceId;
//[-end-120917-IB03780460-add]//
//[-start-120613-IB03780448-remove]//
//[-start-120417-IB03780438-add]//
//#ifdef THUNDERBOLT_SUPPORT
//  //
//  // Sample for TBT GPIO 3 is routed to PCH GPIO
//  //
////  UINT16                                 GpioBase;
////  UINT16                                 GpioOffset;
////  UINT32                                 GpioValue;
//  //
//  // Sample for TBT GPIO 3 is routed to SIO/EC GPIO
//  //
//  UINT16                                 SioGpioBase;
//  UINT8                                  SioGpioValue;
//#endif
//[-end-120417-IB03780438-add]//
//[-end-120613-IB03780448-remove]//
//[-start-121126-IB06460466-add]//
  PLATFORM_INFO_PROTOCOL                  *PlatformInfoProtocol;
//[-end-121126-IB06460466-add]//

  Handle                 = NULL;
  PortIndex              = 0;
  SetupUtility           = NULL;
  SetupVariable          = NULL;
  PlatformHardwareSwitch = NULL;
  GuidHob                = NULL;
//[-start-120628-IB06460410-add]//
  PchSeries              = GetPchSeries();
//[-end-120628-IB06460410-add]//

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\nPCH Policy Entry\n" ) );

  Status = gBS->LocateProtocol ( &gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility );
  ASSERT_EFI_ERROR ( Status );
  SetupVariable = ( CHIPSET_CONFIGURATION * )( SetupUtility->SetupNvData );
  //
  // Find Platform Hardware switch Hob
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
  ASSERT_EFI_ERROR (Status);
  Hob.Raw = GetNextGuidHob (&gPlatformHardwareSwitchGuid, HobList);
  PlatformHardwareSwitch = GET_GUID_HOB_DATA (Hob.Guid);
//[-start-121127-IB06460466-add]//
  Status = gBS->LocateProtocol (&gEfiPlatformInfoProtocolGuid, NULL, (VOID **)&PlatformInfoProtocol);
  ASSERT_EFI_ERROR (Status);    
//[-end-121127-IB06460466-add]//
  ///
  /// General intialization
  ///
//[-start-121211-IB06460478-modify]//
//[-start-121113-IB06150259-modify]//
  mPchPolicyData.Revision             = DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_7;
//[-end-121113-IB06150259-modify]//
//[-end-121211-IB06460478-modify]//
  mPchPolicyData.BusNumber              = 0; 
  
  mPchPolicyData.DeviceEnabling         = &mPchDeviceEnabling;
  mPchPolicyData.UsbConfig              = &mPchUsbConfig;
  mPchPolicyData.PciExpressConfig       = &mPchPciExpressConfig;
  mPchPolicyData.SataConfig             = &mPchSataConfig;
  mPchPolicyData.AzaliaConfig           = &mPchAzaliaConfig;
  mPchPolicyData.SmbusConfig            = &mPchSmbusConfig;
  mPchPolicyData.MiscPmConfig           = &mPchMiscPmConfig;
  mPchPolicyData.IoApicConfig           = &mPchIoApicConfig;
  mPchPolicyData.DefaultSvidSid         = &mPchDefaultSvidSid;
  mPchPolicyData.LockDownConfig         = &mPchLockDownConfig;
  mPchPolicyData.ThermalConfig          = &mPchThermalConfig;
  mPchPolicyData.HpetConfig             = &mPchHpetConfig;
  mPchPolicyData.SerialIrqConfig        = &mSerialIrqConfig;
  mPchPolicyData.DmiConfig              = &mPchDmiConfig;
  mPchPolicyData.PwrOptConfig           = &mPchPwrOptConfig;
  mPchPolicyData.MiscConfig             = &mPchMiscConfig;
//[-start-120628-IB06460410-add]//
//[-start-120807-IB04770241-modify]//
  mPchPolicyData.AudioDspConfig         = &mAudioDspConfig;
//[-start-121113-IB06150259-modify]//
  mPchPolicyData.SerialIoConfig         = &mSerialIoConfig;
//[-end-121113-IB06150259-modify]//
//[-end-120807-IB04770241-modify]//

//[-start-120807-IB04770241-modify]//
// #ifdef PCH_A0PWRON_SAFEMODE
//   if (PchSeries == PchLp) {
//     //
//     // PwrOnSafeModeBits - bit definition:
//     //
//     // Enable safe modes by setting all bits
//     // LPT-LP definition:
//     //  BIT0   = Misc Safe Mode
//     //  BIT1   = Functional PM Safe Mode
//     //  BIT2   = Low Power PM Disable
//     //  BIT3   = CPPM Disable
//     //  BIT4   = Clock Gate Disable
//     //  BIT5   = All ASPM Disable
//     //  BIT6   = Lock register bypass
//     //  BIT7   = Thermal Disable - this is default disable already, no need to disable here
//     //  BIT8   = Gbe Disable
//     //  BIT9   = Chap Disable
//     //  BIT10  = Serial Io & Audio DSP Disable
//     //
//     mPchPolicyData.PwrOnSafeModeBits = 0x13;
//   }else{
//     mPchPolicyData.PwrOnSafeModeBits = 0;
//   }
//   DEBUG ((EFI_D_INFO, "PCH_A0PWRON: PCH Policy Init Dxe PwrOnSafeModeBits = %x\n", mPchPolicyData.PwrOnSafeModeBits));
// #endif
//[-end-120807-IB04770241-modify]//
//[-end-120628-IB06460410-add]//

  //
  // PCH BIOS Spec security recommendations,
  // Intel strongly recommends that BIOS sets the BIOS Interface Lock Down bit. Enabling this bit
  // will mitigate malicious software attempts to replace the system BIOS option ROM with its own code.
  // We always enable this as a platform policy.
  //
  mPchLockDownConfig.BiosInterface          = PcdGet8 ( PcdPchLockDownConfigBiosInterface );
  mPchLockDownConfig.GlobalSmi              = PcdGet8 ( PcdPchLockDownConfigGlobalSmi );
  mPchLockDownConfig.GpioLockDown           = SetupVariable->GpioLockdown;//PCH_DEVICE_DISABLE;
  mPchLockDownConfig.RtcLock                = SetupVariable->RtcLock;   
  //
  // While BiosLock is enabled, BIOS can only be modified from SMM after ExitPmAuth.
  //
  mPchLockDownConfig.BiosLock               = PcdGet8 ( PcdPchLockDownConfigBiosLock );
  ///
  /// If PchBiosLockIoTrapAddress is 0, BIOS will allocate available IO address with
  /// 256 byte range from GCD and pass it to PchBiosLockIoTrapAddress.
  ///
  mPchLockDownConfig.PchBiosLockIoTrapAddress = PcdGet16 ( PcdPchLockDownConfigPchBiosLockIoTrapAddress );

//[-start-130620-IB05160461-add]//
  mPchLockDownConfig.TcoLock            = (SetupVariable->TCOWatchDog ? PCH_DEVICE_DISABLE : PCH_DEVICE_ENABLE);
//[-end-130620-IB05160461-add]//

  mPchDeviceEnabling.Lan                = SetupVariable->Lan;
  mPchDeviceEnabling.Azalia             = SetupVariable->Azalia;
  mPchDeviceEnabling.Sata               = SetupVariable->IdeController;
  mPchDeviceEnabling.Smbus              = PcdGet8 ( PcdPchDeviceEnablingSmbus );
  mPchDeviceEnabling.PciClockRun        = SetupVariable->PciClockRatRun;
  mPchDeviceEnabling.Display            = PcdGet8 ( PcdPchDeviceEnablingDisplay );
//[-start-120320-IB06460377-modify]//
  mPchDeviceEnabling.Crid               = SetupVariable->CRIDEnable;
//[-end-120320-IB06460377-modify]//
//[-start-120628-IB06460410-add]//
  if (FeaturePcdGet ( PcdUltFlag ) ) {
//[-start-120705-IB05330352-modify]//
//[-start-120713-IB06460418-modify]//
//[-start-121113-IB06150259-modify]//
  mPchDeviceEnabling.SerialIoDma            = SetupVariable->EnableSerialIoDma;
  mPchDeviceEnabling.SerialIoI2c0           = SetupVariable->EnableSerialIoI2c0;
  mPchDeviceEnabling.SerialIoI2c1           = SetupVariable->EnableSerialIoI2c1;
  mPchDeviceEnabling.SerialIoSpi0           = SetupVariable->EnableSerialIoSpi0;
  mPchDeviceEnabling.SerialIoSpi1           = SetupVariable->EnableSerialIoSpi1;
  mPchDeviceEnabling.SerialIoUart0          = SetupVariable->EnableSerialIoUart0;
  mPchDeviceEnabling.SerialIoUart1          = SetupVariable->EnableSerialIoUart1;

  mPchDeviceEnabling.SerialIoSdio           = SetupVariable->EnableSerialIoSdio;
  mPchDeviceEnabling.AudioDsp           = SetupVariable->EnableSerialIoAudioDsp;
//[-end-121113-IB06150259-modify]//
//[-end-120713-IB06460418-modify]//
//[-end-120705-IB05330352-modify]//
  }
//[-end-120628-IB06460410-add]//

  //
  // USB Config
  //

//[-start-120917-IB03780460-add]//
  //
  // USB Per-Port Control is use to Enable/Disable individual port.
  //
//[-end-120917-IB03780460-add]//
  mPchUsbConfig.UsbPerPortCtl           = SetupVariable->UsbPerPortCtl;
//[-start-120917-IB03780460-add]//
  //
  // Use by AMT/MEBx to enable USB-R support.
  //
//[-end-120917-IB03780460-add]//
  if (SetupVariable->MeFwImageType == FW_IMAGE_TYPE_5MB) {
    mPchUsbConfig.Ehci1Usbr             = PCH_DEVICE_ENABLE;
    mPchUsbConfig.Ehci2Usbr             = PCH_DEVICE_ENABLE;
  } else {
    mPchUsbConfig.Ehci1Usbr             = PCH_DEVICE_DISABLE;
    mPchUsbConfig.Ehci2Usbr             = PCH_DEVICE_DISABLE;
  } 

//[-start-120917-IB03780460-add]//
  //
  // UsbPrecondition = Enable , Force USB Init happen in PEI as part of 2Sec Fast Boot bios optimization
  // UsbPrecondition = Disable, USB Init happen in DXE just like traditionally where it happen.
  //
//[-end-120917-IB03780460-add]//
//[-start-120809-IB04770241-add]//
//[-start-130714-IB10930040-modify]//
  mPchUsbConfig.UsbPrecondition         = SetupVariable->UsbPrecondition;
//[-end-130714-IB10930040-modify]//
//[-end-120809-IB04770241-add]//

  mPchUsbConfig.PortSettings[0].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port00En;
  mPchUsbConfig.PortSettings[1].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port01En;
  mPchUsbConfig.PortSettings[2].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port02En;
  mPchUsbConfig.PortSettings[3].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port03En;
  mPchUsbConfig.PortSettings[4].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port04En;
  mPchUsbConfig.PortSettings[5].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port05En;  
  mPchUsbConfig.PortSettings[6].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port06En;
  mPchUsbConfig.PortSettings[7].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port07En;
  mPchUsbConfig.PortSettings[8].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port08En;
  mPchUsbConfig.PortSettings[9].Enable  = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port09En;
  mPchUsbConfig.PortSettings[10].Enable = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port10En;
  mPchUsbConfig.PortSettings[11].Enable = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port11En;
  mPchUsbConfig.PortSettings[12].Enable = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port12En;
  mPchUsbConfig.PortSettings[13].Enable = (BOOLEAN)PlatformHardwareSwitch->UsbPortSwitch.Config.Port13En;

  mPchUsbConfig.Port30Settings[0].Enable  = PCH_DEVICE_ENABLE;
  mPchUsbConfig.Port30Settings[1].Enable  = PCH_DEVICE_ENABLE;
  mPchUsbConfig.Port30Settings[2].Enable  = PCH_DEVICE_ENABLE;
  mPchUsbConfig.Port30Settings[3].Enable  = PCH_DEVICE_ENABLE;
  mPchUsbConfig.Port30Settings[4].Enable  = PCH_DEVICE_ENABLE;
  mPchUsbConfig.Port30Settings[5].Enable  = PCH_DEVICE_ENABLE;
  
//[-start-120917-IB03780460-remove]//
//  ///
//  /// The following setting is only available for Desktop
//  /// Please program it per the layout of each port on the platform board.
//  ///
//  mPchUsbConfig.PortSettings[0].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings0Panel );
//  mPchUsbConfig.PortSettings[1].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings1Panel );
//  mPchUsbConfig.PortSettings[2].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings2Panel );
//  mPchUsbConfig.PortSettings[3].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings3Panel );
//  mPchUsbConfig.PortSettings[4].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings4Panel );
//  mPchUsbConfig.PortSettings[5].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings5Panel );
//  mPchUsbConfig.PortSettings[6].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings6Panel );
//  mPchUsbConfig.PortSettings[7].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings7Panel );
//  mPchUsbConfig.PortSettings[8].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings8Panel );
//  mPchUsbConfig.PortSettings[9].Panel   = PcdGet8 ( PcdPchUsbConfigPortSettings9Panel );
//  mPchUsbConfig.PortSettings[10].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings10Panel );
//  mPchUsbConfig.PortSettings[11].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings11Panel );
//  mPchUsbConfig.PortSettings[12].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings12Panel );
//  mPchUsbConfig.PortSettings[13].Panel  = PcdGet8 ( PcdPchUsbConfigPortSettings13Panel );
//  ///
//  /// The following setting is only available for Mobile
//  /// Please program it per the layout of each port on the platform board.
//  ///
//  mPchUsbConfig.PortSettings[0].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings0Dock );
//  mPchUsbConfig.PortSettings[1].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings1Dock );
//  mPchUsbConfig.PortSettings[2].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings2Dock );
//  mPchUsbConfig.PortSettings[3].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings3Dock );
//  mPchUsbConfig.PortSettings[4].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings4Dock );
//  mPchUsbConfig.PortSettings[5].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings5Dock );
//  mPchUsbConfig.PortSettings[6].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings6Dock );
//  mPchUsbConfig.PortSettings[7].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings7Dock );
//  mPchUsbConfig.PortSettings[8].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings8Dock );
//  mPchUsbConfig.PortSettings[9].Dock          = PcdGet8 ( PcdPchUsbConfigPortSettings9Dock );
//  mPchUsbConfig.PortSettings[10].Dock         = PcdGet8 ( PcdPchUsbConfigPortSettings10Dock );
//  mPchUsbConfig.PortSettings[11].Dock         = PcdGet8 ( PcdPchUsbConfigPortSettings11Dock );
//  mPchUsbConfig.PortSettings[12].Dock         = PcdGet8 ( PcdPchUsbConfigPortSettings12Dock );
//  mPchUsbConfig.PortSettings[13].Dock         = PcdGet8 ( PcdPchUsbConfigPortSettings13Dock );
//[-end-120917-IB03780460-remove]//
  
  mPchUsbConfig.Usb20Settings[PCH_USB_EHCI1].Enable = SetupVariable->Ehci1Enable;
  mPchUsbConfig.Usb20Settings[PCH_USB_EHCI2].Enable = SetupVariable->Ehci2Enable; 
  //
  // PCH BIOS Spec Rev 0.7.0 Section 13.1 xHCI controller options in Reference Code
  // Please refer to Table 13-1 in PCH BIOS Spec for USB Port Operation with no xHCI
  // pre-boot software.
  // Please refer to Table 13-2 in PCH BIOS Spec for USB Port Operation with xHCI
  // pre-boot software.
  //
  // The xHCI modes that available in BIOS are:
  // Disabled   - forces only USB 2.0 to be supported in the OS. The xHCI controller is turned off
  //              and hidden from the PCI space.
  // Enabled    - allows USB 3.0 to be supported in the OS. The xHCI controller is turned on. The
  //              shareable ports are routed to the xHCI controller. OS needs to provide drivers
  //              to support USB 3.0.
  // Auto       - This mode uses ACPI protocol to provide an option that enables the xHCI controller
  //              and reroute USB ports via the _OSC ACPI method call. Note, this mode switch requires
  //              special OS driver support for USB 3.0.
  // Smart Auto - This mode is similar to Auto, but it adds the capability to route the ports to xHCI
  //              or EHCI according to setting used in previous boots (for non-G3 boot) in the pre-boot
  //              environment. This allows the use of USB 3.0 devices prior to OS boot. Note, this mode
  //              switch requires special OS driver support for USB 3.0 and USB 3.0 software available
  //              in the pre-boot enviroment.
//[-start-120917-IB03780460-add]//
  //
  // Manual Mode - For validation and experimental purposes only. Do not create setup option for end-user BIOS.
  //
//[-end-120917-IB03780460-add]//
  // Recommendations:
  //  - If BIOS supports xHCI pre-boot driver then use Smart Auto mode as default
  //  - If BIOS does not support xHCI pre-boot driver then use AUTO mode as default
  //
  
//[-start-120917-IB03780460-add]//
  //
  // Manual Mode is for validation and experimental purposes only.
  // Do not create setup option for end-user BIOS.
  //
//[-end-120917-IB03780460-add]//
//[-start-130205-IB02950497-modify]//
  if (SetupVariable->ManualMode == MANUAL_ENABLE) {
    mPchUsbConfig.Usb30Settings.ManualMode = MANUAL_ENABLE;
  } else { 	
    mPchUsbConfig.Usb30Settings.ManualMode = MANUAL_DISABLE;
  }

  //
  // Btcg is for enabling/disabling trunk clock gating.
  //
//[-start-130506-IB10930031-modify]//
  mPchUsbConfig.Usb30Settings.Btcg = PCH_DEVICE_ENABLE;
//[-end-130506-IB10930031-modify]//

  if (SetupVariable->USBRouteSwitch == ALL_PINS_XHCI) {
    for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
      mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[PortIndex] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRouteAllPinsXhci);
    }
  } else if (SetupVariable->USBRouteSwitch == PER_PIN) {
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[0] = SetupVariable->USB20Pin0;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[1] = SetupVariable->USB20Pin1;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[2] = SetupVariable->USB20Pin2;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[3] = SetupVariable->USB20Pin3;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[4] = SetupVariable->USB20Pin4;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[5] = SetupVariable->USB20Pin5;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[6] = SetupVariable->USB20Pin6;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[7] = SetupVariable->USB20Pin7;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[8] = SetupVariable->USB20Pin8;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[9] = SetupVariable->USB20Pin9;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[10] = SetupVariable->USB20Pin10;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[11] = SetupVariable->USB20Pin11;
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[12] = SetupVariable->USB20Pin12; 
    mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[13] = SetupVariable->USB20Pin13; 
  } else {
//[-start-120628-IB06460410-modify]//
    for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
//[-end-120628-IB06460410-modify]//
      mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[PortIndex]   = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb20PerPinRouteElse);
    }
  }

  if (SetupVariable->USB30SuperSpeed == ALL_PINS_ENABLE) {
    for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb3PortNum (); PortIndex++) {
      mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[PortIndex] = PcdGet8 (PcdPchUsbConfigUsb30SettingsManualModeUsb30PerPinEnableAllPinEnable);
    }
  } else if (SetupVariable->USB30SuperSpeed == PER_PIN) {
    mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[0] = SetupVariable->USB30Pin1;
    mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[1] = SetupVariable->USB30Pin2;
    mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[2] = SetupVariable->USB30Pin3;
    mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[3] = SetupVariable->USB30Pin4;
    mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[4] = SetupVariable->USB30Pin5;
    mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[5] = SetupVariable->USB30Pin6;
  } else {
//[-start-120628-IB06460410-modify]//
    for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb3PortNum (); PortIndex++) {
//[-end-120628-IB06460410-modify]//
      mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[PortIndex]  = PcdGet8 ( PcdPchUsbConfigUsb30SettingsManualModeUsb30PerPinEnableElse );
    }
  }
  mPchUsbConfig.Usb30Settings.Mode              = SetupVariable->XHCIMode;
//[-start-121113-IB06150259-add]//
//[-start-130227-IB10370032-add]//
  mPchUsbConfig.Usb30Settings.XhciIdleL1 = PCH_DEVICE_ENABLE;
//[-end-130227-IB10370032-add]//
//[-start-130401-IB02950501-modify]//
  if (SetupVariable->ManualMode == MANUAL_ENABLE) {

    if (SetupVariable->USBRouteSwitch == ALL_PINS_XHCI) {
      //
      // Automatically disable EHCI when ManualMode is enable and USB port all route to XHCI.
      //
      mPchUsbConfig.Usb20Settings[0].Enable = PCH_DEVICE_DISABLE;
      if (PchSeries == PchH) {
        mPchUsbConfig.Usb20Settings[1].Enable = PCH_DEVICE_DISABLE;
      }
    }
  } else {  
    //
    // Automatically disable EHCI when XHCI Mode is Enabled to save power.
    //
    if (mPchUsbConfig.Usb30Settings.Mode == PCH_XHCI_MODE_ON) {
      mPchUsbConfig.Usb20Settings[0].Enable = PCH_DEVICE_DISABLE;
      if (PchSeries == PchH) {
        mPchUsbConfig.Usb20Settings[1].Enable = PCH_DEVICE_DISABLE;
      }
    }
  }
//[-end-130401-IB02950501-modify]//
//[-end-121113-IB06150259-add]//
//[-end-130205-IB02950497-modify]//
//[-start-120917-IB03780460-add]//
  //
  // Set to Enable if BIOS has its own xHCI driver
  //
//[-end-120917-IB03780460-add]//
  mPchUsbConfig.Usb30Settings.PreBootSupport    = SetupVariable->XhciPreBootSupport;  
  mPchUsbConfig.Usb30Settings.XhciStreams       = SetupVariable->XhciStreams;

//[-start-120917-IB03780460-modify]//
  //
  // USB Port Over Current Pins mapping, please set as per board layout.
  //
  mPchUsbConfig.Usb20OverCurrentPins[0]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins0 );
  mPchUsbConfig.Usb20OverCurrentPins[1]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins1 );
  mPchUsbConfig.Usb20OverCurrentPins[2]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins2 );
  mPchUsbConfig.Usb20OverCurrentPins[3]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins3 );
  mPchUsbConfig.Usb20OverCurrentPins[4]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins4 );
  mPchUsbConfig.Usb20OverCurrentPins[5]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins5 );
  mPchUsbConfig.Usb20OverCurrentPins[6]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins6 );
  mPchUsbConfig.Usb20OverCurrentPins[7]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins7 );
  mPchUsbConfig.Usb20OverCurrentPins[8]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins8 );
  mPchUsbConfig.Usb20OverCurrentPins[9]   = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins9 );
  mPchUsbConfig.Usb20OverCurrentPins[10]  = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins10 );
  mPchUsbConfig.Usb20OverCurrentPins[11]  = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins11 );
  mPchUsbConfig.Usb20OverCurrentPins[12]  = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins12 );
  mPchUsbConfig.Usb20OverCurrentPins[13]  = PcdGet8 ( PcdPchUsbConfigUsb20OverCurrentPins13 );
//[-end-120917-IB03780460-modify]//

//[-start-120917-IB03780460-modify]//
  mPchUsbConfig.Usb30OverCurrentPins[0]   = PcdGet8 ( PcdPchUsbConfigUsb30OverCurrentPins0 );
  mPchUsbConfig.Usb30OverCurrentPins[1]   = PcdGet8 ( PcdPchUsbConfigUsb30OverCurrentPins1 );
  mPchUsbConfig.Usb30OverCurrentPins[2]   = PcdGet8 ( PcdPchUsbConfigUsb30OverCurrentPins2 );
  mPchUsbConfig.Usb30OverCurrentPins[3]   = PcdGet8 ( PcdPchUsbConfigUsb30OverCurrentPins3 );
  mPchUsbConfig.Usb30OverCurrentPins[4]   = PcdGet8 ( PcdPchUsbConfigUsb30OverCurrentPins4 );
  mPchUsbConfig.Usb30OverCurrentPins[5]   = PcdGet8 ( PcdPchUsbConfigUsb30OverCurrentPins5 );
//[-end-120917-IB03780460-modify]//

//[-start-120917-IB03780460-modify]//
  //
  // USB 2.0 D+/D- trace length in inchs*10 or 1000mils/10 measurement eg. 12.3" = 0x123    
  // Please set as per board layout.
  //
  mPchUsbConfig.PortSettings[0].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength0 );
  mPchUsbConfig.PortSettings[1].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength1 );
  mPchUsbConfig.PortSettings[2].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength2 );
  mPchUsbConfig.PortSettings[3].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength3 );
  mPchUsbConfig.PortSettings[4].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength4 );
  mPchUsbConfig.PortSettings[5].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength5 );
  mPchUsbConfig.PortSettings[6].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength6 );
  mPchUsbConfig.PortSettings[7].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength7 );
  mPchUsbConfig.PortSettings[8].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength8 );
  mPchUsbConfig.PortSettings[9].Usb20PortLength        = PcdGet16 ( PcdPchUsbConfigUsb20PortLength9 );
  mPchUsbConfig.PortSettings[10].Usb20PortLength       = PcdGet16 ( PcdPchUsbConfigUsb20PortLength10 );
  mPchUsbConfig.PortSettings[11].Usb20PortLength       = PcdGet16 ( PcdPchUsbConfigUsb20PortLength11 );
  mPchUsbConfig.PortSettings[12].Usb20PortLength       = PcdGet16 ( PcdPchUsbConfigUsb20PortLength12 );
  mPchUsbConfig.PortSettings[13].Usb20PortLength       = PcdGet16 ( PcdPchUsbConfigUsb20PortLength13 );
//[-end-120917-IB03780460-modify]//

//[-start-120917-IB03780460-add]//
  //
  // Port Location (Front/Back Panel, Dock, MiniPcie...etc)
  //
  mPchUsbConfig.PortSettings[0].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings0Location );
  mPchUsbConfig.PortSettings[1].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings1Location );
  mPchUsbConfig.PortSettings[2].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings2Location );
  mPchUsbConfig.PortSettings[3].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings3Location );
  mPchUsbConfig.PortSettings[4].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings4Location );
  mPchUsbConfig.PortSettings[5].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings5Location );
  mPchUsbConfig.PortSettings[6].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings6Location );
  mPchUsbConfig.PortSettings[7].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings7Location );
  mPchUsbConfig.PortSettings[8].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings8Location );
  mPchUsbConfig.PortSettings[9].Location  = PcdGet8 ( PcdPchUsbConfigPortSettings9Location );
  mPchUsbConfig.PortSettings[10].Location = PcdGet8 ( PcdPchUsbConfigPortSettings10Location );
  mPchUsbConfig.PortSettings[11].Location = PcdGet8 ( PcdPchUsbConfigPortSettings11Location );
  mPchUsbConfig.PortSettings[12].Location = PcdGet8 ( PcdPchUsbConfigPortSettings12Location );
  mPchUsbConfig.PortSettings[13].Location = PcdGet8 ( PcdPchUsbConfigPortSettings13Location );
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

  LpcDeviceId = MmioRead16 (MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 0) + R_PCH_LPC_DEVICE_ID);
  if (PchSeries == PchH) {
    if (IS_PCH_LPT_LPC_DEVICE_ID_DESKTOP (LpcDeviceId)) {
      for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
        if (mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationBackPanel) {
          //
          // Back Panel
          //
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 4;
        } else {
          //
          // Front Panel
          //
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 3;        
        }
      
        if (mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationBackPanel) {
          if (mPchUsbConfig.PortSettings[PortIndex].Usb20PortLength < 0x80) {
            //
            // Back Panel, less than 7.9"
            //
            mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2;
          } else if (mPchUsbConfig.PortSettings[PortIndex].Usb20PortLength < 0x130) {
            //
            // Back Panel, 8"-12.9"
            //
            mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 3;
          } else {
            //
            // Back Panel, 13" onward
            //
            mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 4;
          }
        } else {
          //
          // Front Panel
          //
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2;
        }
      }
    } else if (IS_PCH_LPT_LPC_DEVICE_ID_MOBILE (LpcDeviceId)) {
      for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
        if (mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationInternalTopology) {
          //
          // Internal Topology
          //
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 5;
        } else {
          //
          // Note: If a port fail eye diagram, try set that port to 6
          //
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 4;
        }

        if (mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationDock) {
          //
          // Docking
          //
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 3;
        } else {
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2;
        }
      }
    }
  } else if (PchSeries == PchLp) {
    for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
      if ((mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationBackPanel) || 
          (mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationMiniPciE)) {
        if (mPchUsbConfig.PortSettings[PortIndex].Usb20PortLength < 0x70) {
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 5; //Back Panel, less than 7"
        } else {
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 6; //Back Panel, 7" onward
        }
      } else if (mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationDock) {
        mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 4; // Dock
      } else {
        mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam1 = 5; // Internal Topology
      } 

      if ((mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationBackPanel) || 
          (mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationMiniPciE)) {
        if (mPchUsbConfig.PortSettings[PortIndex].Usb20PortLength < 0x100) {
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2; //Back Panel, less than 10"
        } else {
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 3; //Back Panel, 10" onward
        }
      } else if (mPchUsbConfig.PortSettings[PortIndex].Location == PchUsbPortLocationDock) {
        if (mPchUsbConfig.PortSettings[PortIndex].Usb20PortLength < 0x50) {
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 1; //Dock, less than 5"
        } else {
          mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2; //Dock, 5" onward
        }
      } else {
        mPchUsbConfig.PortSettings[PortIndex].Usb20EyeDiagramTuningParam2 = 2; // Internal Topology
      }
    }
  }
//[-end-120917-IB03780460-add]//
//[-end-121211-IB06460478-modify]//
  

  //
  // PCI Express Config
  //
  SetPcieConfig (PlatformHardwareSwitch, SetupVariable);

  //
  // SATA Config
  //
  mPchSataConfig.PortSettings[0].Enable          = (BOOLEAN)PlatformHardwareSwitch->SataPortSwitch.Config.Port00En;
  mPchSataConfig.PortSettings[1].Enable          = (BOOLEAN)PlatformHardwareSwitch->SataPortSwitch.Config.Port01En;
  mPchSataConfig.PortSettings[2].Enable          = (BOOLEAN)PlatformHardwareSwitch->SataPortSwitch.Config.Port02En;
  mPchSataConfig.PortSettings[3].Enable          = (BOOLEAN)PlatformHardwareSwitch->SataPortSwitch.Config.Port03En;
  mPchSataConfig.PortSettings[4].Enable          = (BOOLEAN)PlatformHardwareSwitch->SataPortSwitch.Config.Port04En;
  mPchSataConfig.PortSettings[5].Enable          = (BOOLEAN)PlatformHardwareSwitch->SataPortSwitch.Config.Port05En;

  mPchSataConfig.PortSettings[0].HotPlug         = SetupVariable->HdcP0HotPlug;
  mPchSataConfig.PortSettings[1].HotPlug         = SetupVariable->HdcP1HotPlug;
  mPchSataConfig.PortSettings[2].HotPlug         = SetupVariable->HdcP2HotPlug;
  mPchSataConfig.PortSettings[3].HotPlug         = SetupVariable->HdcP3HotPlug;
  mPchSataConfig.PortSettings[4].HotPlug         = SetupVariable->HdcP4HotPlug;
  mPchSataConfig.PortSettings[5].HotPlug         = SetupVariable->HdcP5HotPlug;

  mPchSataConfig.PortSettings[0].InterlockSw     = PcdGet8 ( PcdPchSataConfigPortSettings0InterlockSw );
  mPchSataConfig.PortSettings[1].InterlockSw     = PcdGet8 ( PcdPchSataConfigPortSettings1InterlockSw );
  mPchSataConfig.PortSettings[2].InterlockSw     = PcdGet8 ( PcdPchSataConfigPortSettings2InterlockSw );
  mPchSataConfig.PortSettings[3].InterlockSw     = PcdGet8 ( PcdPchSataConfigPortSettings3InterlockSw );
  mPchSataConfig.PortSettings[4].InterlockSw     = PcdGet8 ( PcdPchSataConfigPortSettings4InterlockSw );
  mPchSataConfig.PortSettings[5].InterlockSw     = PcdGet8 ( PcdPchSataConfigPortSettings5InterlockSw );

  mPchSataConfig.PortSettings[0].External        = PcdGet8 ( PcdPchSataConfigPortSettings0External );
  mPchSataConfig.PortSettings[1].External        = PcdGet8 ( PcdPchSataConfigPortSettings1External );
  mPchSataConfig.PortSettings[2].External        = PcdGet8 ( PcdPchSataConfigPortSettings2External );
  mPchSataConfig.PortSettings[3].External        = PcdGet8 ( PcdPchSataConfigPortSettings3External );
  mPchSataConfig.PortSettings[4].External        = PcdGet8 ( PcdPchSataConfigPortSettings4External );
  mPchSataConfig.PortSettings[5].External        = PcdGet8 ( PcdPchSataConfigPortSettings5External );

  mPchSataConfig.PortSettings[0].SpinUp          = SetupVariable->HdcP0SpinUp;
  mPchSataConfig.PortSettings[1].SpinUp          = SetupVariable->HdcP1SpinUp;
  mPchSataConfig.PortSettings[2].SpinUp          = SetupVariable->HdcP2SpinUp;
  mPchSataConfig.PortSettings[3].SpinUp          = SetupVariable->HdcP3SpinUp;
  mPchSataConfig.PortSettings[4].SpinUp          = SetupVariable->HdcP4SpinUp;
  mPchSataConfig.PortSettings[5].SpinUp          = SetupVariable->HdcP5SpinUp;
  
  mPchSataConfig.PortSettings[0].SolidStateDrive = SetupVariable->SataP0DeviceType;
  mPchSataConfig.PortSettings[1].SolidStateDrive = SetupVariable->SataP1DeviceType;
  mPchSataConfig.PortSettings[2].SolidStateDrive = SetupVariable->SataP2DeviceType;
  mPchSataConfig.PortSettings[3].SolidStateDrive = SetupVariable->SataP3DeviceType;
  mPchSataConfig.PortSettings[4].SolidStateDrive = SetupVariable->SataP4DeviceType;
  mPchSataConfig.PortSettings[5].SolidStateDrive = SetupVariable->SataP5DeviceType;

//[-start-120904-IB03780460-add]//
//[-start-121219-IB05330396-modify]//
  mPchSataConfig.PortSettings[0].DevSlp          = SetupVariable->SataP0DeviceSleep;
  mPchSataConfig.PortSettings[1].DevSlp          = SetupVariable->SataP1DeviceSleep;
  mPchSataConfig.PortSettings[2].DevSlp          = SetupVariable->SataP2DeviceSleep;
  mPchSataConfig.PortSettings[3].DevSlp          = SetupVariable->SataP3DeviceSleep;
  mPchSataConfig.PortSettings[4].DevSlp          = SetupVariable->SataP4DeviceSleep;
  mPchSataConfig.PortSettings[5].DevSlp          = SetupVariable->SataP5DeviceSleep;
//[-end-121219-IB05330396-modify]//
//[-end-120904-IB03780460-add]//

//[-start-121219-IB05330396-add]//
  mPchSataConfig.PortSettings[0].EnableDitoConfig = SetupVariable->SataP0EnableDito;
  mPchSataConfig.PortSettings[1].EnableDitoConfig = SetupVariable->SataP1EnableDito;
  mPchSataConfig.PortSettings[2].EnableDitoConfig = SetupVariable->SataP2EnableDito;
  mPchSataConfig.PortSettings[3].EnableDitoConfig = SetupVariable->SataP3EnableDito;
  mPchSataConfig.PortSettings[4].EnableDitoConfig = SetupVariable->SataP4EnableDito;
  mPchSataConfig.PortSettings[5].EnableDitoConfig = SetupVariable->SataP5EnableDito;

  mPchSataConfig.PortSettings[0].DmVal            = SetupVariable->SataP0DmVal;
  mPchSataConfig.PortSettings[1].DmVal            = SetupVariable->SataP1DmVal; 
  mPchSataConfig.PortSettings[2].DmVal            = SetupVariable->SataP2DmVal;
  mPchSataConfig.PortSettings[3].DmVal            = SetupVariable->SataP3DmVal;
  mPchSataConfig.PortSettings[4].DmVal            = SetupVariable->SataP4DmVal;
  mPchSataConfig.PortSettings[5].DmVal            = SetupVariable->SataP5DmVal;

  mPchSataConfig.PortSettings[0].DitoVal          = SetupVariable->SataP0DitoVal;
  mPchSataConfig.PortSettings[1].DitoVal          = SetupVariable->SataP1DitoVal; 
  mPchSataConfig.PortSettings[2].DitoVal          = SetupVariable->SataP2DitoVal;
  mPchSataConfig.PortSettings[3].DitoVal          = SetupVariable->SataP3DitoVal;
  mPchSataConfig.PortSettings[4].DitoVal          = SetupVariable->SataP4DitoVal;
  mPchSataConfig.PortSettings[5].DitoVal          = SetupVariable->SataP5DitoVal;
//[-end-121219-IB05330396-add]//

//[-start-121113-IB06150259-add]//
  for (PortIndex = 0; PortIndex < LPTH_AHCI_MAX_PORTS; PortIndex++) {
    mPchSataConfig.PortSettings[PortIndex].EnableDitoConfig = PCH_DEVICE_DISABLE;
    mPchSataConfig.PortSettings[PortIndex].DmVal            = PcdGet8 ( PcdPchSataConfigPortSettingsDmVal );
    mPchSataConfig.PortSettings[PortIndex].DitoVal          = PcdGet16 ( PcdPchSataConfigPortSettingsDitoVal );
  }
//[-end-121113-IB06150259-add]//

  mPchSataConfig.RaidAlternateId                 = SetupVariable->RaidAlternateId;
  mPchSataConfig.Raid0                           = SetupVariable->Raid0;
  mPchSataConfig.Raid1                           = SetupVariable->Raid1;
  mPchSataConfig.Raid10                          = SetupVariable->Raid10;
  mPchSataConfig.Raid5                           = SetupVariable->Raid5;
  mPchSataConfig.Irrt                            = SetupVariable->Irrt;
  mPchSataConfig.OromUiBanner                    = SetupVariable->OromUiBanner;
  mPchSataConfig.HddUnlock                       = SetupVariable->HddUnlock;
  
  mPchSataConfig.LedLocate                       = SetupVariable->LedLocate;
  mPchSataConfig.IrrtOnly                        = SetupVariable->IrrtOnly;
  mPchSataConfig.TestMode                        = PcdGet8 ( PcdPchSataConfigTestMode );
  mPchSataConfig.SalpSupport                     = SetupVariable->AggressLinkPower;
  mPchSataConfig.SpeedSupport                    = PcdGet8 ( PcdPchSataConfigSpeedSupport );
  mPchSataConfig.LegacyMode                      = PcdGet8 ( PcdPchSataConfigLegacyMode );
  mPchSataConfig.SmartStorage                    = PcdGet8 ( PcdPchSataConfigSmartStorage );
  mPchSataConfig.OromUiDelay                     = PcdGet8 ( PcdPchSataConfigOromUiDelay );
//[-start-120604-IB03600487-add]//
//
// Sata Port 0 delay time
// 0 means no delay
// For some HDD 
//
  mPchSataConfig.Port0TimeOut                    = SetupVariable->SataP0TimeOut; 
//[-end-120604-IB03600487-add]//
  //
  // Reserved SMBus Address
  //
  mPchSmbusConfig.NumRsvdSmbusAddresses          = PLATFORM_NUM_SMBUS_RSVD_ADDRESSES;
  mPchSmbusConfig.RsvdSmbusAddressTable          = mSmbusRsvdAddresses;  
  //
  // MiscPm Configuration
  //
  mPchMiscPmConfig.PchDeepSxPol                         = SetupVariable->DeepSlpSx;
  mPchMiscPmConfig.WakeConfig.PmeB0S5Dis                =  PcdGet8 ( PcdPchWakeConfigPmeB0S5Dis );
  mPchMiscPmConfig.WakeConfig.WolEnableOverride         = SetupVariable->WakeOnLan; 
//[-start-120628-IB06460410-modify]//
  mPchMiscPmConfig.WakeConfig.Gp27WakeFromDeepSx        = PcdGet8 ( PcdPchWakeConfigGp27WakeFromDeepSx );
//[-end-120628-IB06460410-modify]//
  mPchMiscPmConfig.WakeConfig.PcieWakeFromDeepSx        = PCH_DEVICE_DISABLE;
  mPchMiscPmConfig.PowerResetStatusClear.MeWakeSts      = PcdGet8 ( PcdPchPowerResetStatusClearMeWakeSts );
  mPchMiscPmConfig.PowerResetStatusClear.MeHrstColdSts  = PcdGet8 ( PcdPchPowerResetStatusClearMeHrstColdSts );
  mPchMiscPmConfig.PowerResetStatusClear.MeHrstWarmSts  = PcdGet8 ( PcdPchPowerResetStatusClearMeHrstWarmSts ); 
  mPchMiscPmConfig.PowerResetStatusClear.MeHostPowerDn  = PcdGet8 ( PcdPchPowerResetStatusClearMeHostPowerDn );
  mPchMiscPmConfig.PowerResetStatusClear.WolOvrWkSts    = PcdGet8 ( PcdPchPowerResetStatusClearWolOvrWkSts );

  mPchMiscPmConfig.PchSlpS3MinAssert                    = PcdGet8 ( PcdPchSlpS3MinAssert );
  mPchMiscPmConfig.PchSlpS4MinAssert                    = PcdGet8 ( PcdPchSlpS4MinAssert );
  mPchMiscPmConfig.PchSlpSusMinAssert                   = PcdGet8 ( PcdPchSlpSusMinAssert );
  mPchMiscPmConfig.PchSlpAMinAssert                     = PcdGet8 ( PcdPchSlpAMinAssert );
  mPchMiscPmConfig.PchPwrCycDur                         = 4;  // 4-5 seconds (PCH default setting)
//[-start-120331-IB05300303-modify]//
  mPchMiscPmConfig.SlpStrchSusUp                        = PcdGet8 ( PcdSlpStrchSusUp );
//[-end-120331-IB05300303-modify]//
  mPchMiscPmConfig.SlpLanLowDc                          = SetupVariable->SlpLanLowDc;
  //
  // Io Apic Config
  //
  mPchIoApicConfig.BdfValid            = PcdGetBool ( PcdPchIoApicConfigBdfValid );   // Whether the BDF value is valid
  mPchIoApicConfig.BusNumber           = PcdGet8 ( PcdPchIoApicConfigBusNumber );     // BDF used as Requestor / Completer ID
  mPchIoApicConfig.DeviceNumber        = PcdGet8 ( PcdPchIoApicConfigDeviceNumber );  // BDF used as Requestor / Completer ID
  mPchIoApicConfig.FunctionNumber      = PcdGet8 ( PcdPchIoApicConfigFunctionNumber );// BDF used as Requestor / Completer ID
//[-start-120628-IB06460410-add]//
  /// 
  /// Interrupt Settings
  ///
  mPchIoApicConfig.IoApicEntry24_39    = PcdGet8 ( PcdPchIoApicConfigIoApicEntry24_39 );
//[-end-120628-IB06460410-add]//
  //
  // Default SvidSid
  //
  mPchDefaultSvidSid.SubSystemId       = PcdGet16 ( PcdPchDefaultSvidSidSubSystemId );
  mPchDefaultSvidSid.SubSystemVendorId = PcdGet16 ( PcdPchDefaultSvidSidSubSystemVendorId );  
  
  //
  // Thermal Config
  //
  if ( SetupVariable->ThermalDevice ){ 
    mPchThermalConfig.ThermalAlertEnable.TselLock                 = PCH_DEVICE_ENABLE;
    mPchThermalConfig.ThermalAlertEnable.TscLock                  = PCH_DEVICE_ENABLE;
    mPchThermalConfig.ThermalAlertEnable.TsmicLock                = PCH_DEVICE_ENABLE;
    mPchThermalConfig.ThermalAlertEnable.PhlcLock                 = PCH_DEVICE_ENABLE;
  } else {
    mPchThermalConfig.ThermalAlertEnable.TselLock                 = PCH_DEVICE_DEFAULT;
    mPchThermalConfig.ThermalAlertEnable.TscLock                  = PCH_DEVICE_DEFAULT;
    mPchThermalConfig.ThermalAlertEnable.TsmicLock                = PCH_DEVICE_DEFAULT;
    mPchThermalConfig.ThermalAlertEnable.PhlcLock                 = PCH_DEVICE_DEFAULT;
  }
  
  mPchThermalConfig.ThermalDeviceEnable                           = SetupVariable->ThermalDevice;//D31F6
  mPchThermalConfig.ThermalThrottling.TTLevels.T0Level            = PcdGet32 ( PcdThermalThrottlingTTLevelsT0Level );
  mPchThermalConfig.ThermalThrottling.TTLevels.T1Level            = PcdGet32 ( PcdThermalThrottlingTTLevelsT1Level );
  mPchThermalConfig.ThermalThrottling.TTLevels.T2Level            = PcdGet32 ( PcdThermalThrottlingTTLevelsT2Level );
  mPchThermalConfig.ThermalThrottling.TTLevels.TTEnable           = PcdGet32 ( PcdThermalThrottlingTTLevelsTTEnable );
  mPchThermalConfig.ThermalThrottling.TTLevels.TTState13Enable    = PcdGet32 ( PcdThermalThrottlingTTLevelsTTState13Enable );
  mPchThermalConfig.ThermalThrottling.TTLevels.TTLock             = PcdGet32 ( PcdThermalThrottlingTTLevelsTTLock );
  mPchThermalConfig.ThermalThrottling.TTLevels.SuggestedSetting   = PcdGet32 ( PcdThermalThrottlingTTLevelsSuggestedSetting );
//[-start-121008-IB10370023-add]//
  mPchThermalConfig.ThermalThrottling.TTLevels.PchCrossThrottling = PcdGet32 ( PcdThermalThrottlingTTLevelsPchCrossThrottling );
//[-end-121008-IB10370023-add]//

  mPchThermalConfig.ThermalThrottling.DmiHaAWC.DmiTsawEn          = PcdGet8 ( PcdThermalThrottlingDmiHaAWCDmiTsawEn );
  mPchThermalConfig.ThermalThrottling.DmiHaAWC.TS0TW              = PcdGet8 ( PcdThermalThrottlingDmiHaAWCTS0TW );
  mPchThermalConfig.ThermalThrottling.DmiHaAWC.TS1TW              = PcdGet8 ( PcdThermalThrottlingDmiHaAWCTS1TW );
  mPchThermalConfig.ThermalThrottling.DmiHaAWC.TS2TW              = PcdGet8 ( PcdThermalThrottlingDmiHaAWCTS2TW );
  mPchThermalConfig.ThermalThrottling.DmiHaAWC.TS3TW              = PcdGet8 ( PcdThermalThrottlingDmiHaAWCTS3TW );
  mPchThermalConfig.ThermalThrottling.DmiHaAWC.SuggestedSetting   = PcdGet8 ( PcdThermalThrottlingDmiHaAWCSuggestedSetting );

  mPchThermalConfig.ThermalThrottling.SataTT.P0T1M                = PcdGet8 ( PcdThermalThrottlingSataTTP0T1M );
  mPchThermalConfig.ThermalThrottling.SataTT.P0T2M                = PcdGet8 ( PcdThermalThrottlingSataTTP0T2M );
  mPchThermalConfig.ThermalThrottling.SataTT.P0T3M                = PcdGet8 ( PcdThermalThrottlingSataTTP0T3M );
  mPchThermalConfig.ThermalThrottling.SataTT.P0TDisp              = PcdGet8 ( PcdThermalThrottlingSataTTP0TDisp );
  mPchThermalConfig.ThermalThrottling.SataTT.P1T1M                = PcdGet8 ( PcdThermalThrottlingSataTTP1T1M );
  mPchThermalConfig.ThermalThrottling.SataTT.P1T2M                = PcdGet8 ( PcdThermalThrottlingSataTTP1T2M );
  mPchThermalConfig.ThermalThrottling.SataTT.P1T3M                = PcdGet8 ( PcdThermalThrottlingSataTTP1T3M );
  mPchThermalConfig.ThermalThrottling.SataTT.P1TDisp              = PcdGet8 ( PcdThermalThrottlingSataTTP1TDisp );
  mPchThermalConfig.ThermalThrottling.SataTT.P0Tinact             = PcdGet8 ( PcdThermalThrottlingSataTTP0Tinact );
  mPchThermalConfig.ThermalThrottling.SataTT.P0TDispFinit         = PcdGet8 ( PcdThermalThrottlingSataTTP0TDispFinit );
  mPchThermalConfig.ThermalThrottling.SataTT.P1Tinact             = PcdGet8 ( PcdThermalThrottlingSataTTP1Tinact );
  mPchThermalConfig.ThermalThrottling.SataTT.P1TDispFinit         = PcdGet8 ( PcdThermalThrottlingSataTTP1TDispFinit );
  mPchThermalConfig.ThermalThrottling.SataTT.SuggestedSetting     = PcdGet8 ( PcdThermalThrottlingSataTTSuggestedSetting );
  //
  // The value in this field is valid only if it is between 00h and 7Fh.
  // 0x00 is the hottest temperature and 0x7F is the lowest temperature
  //
  mPchThermalConfig.PchHotLevel = PcdGet16 ( PcdPchHotLevel );
  
  //
  // Initialize Serial IRQ Config
  //
  mSerialIrqConfig.SirqEnable       = PcdGetBool (PcdSerialIrqConfigSirqEnable );
//[-start-120710-IB07360203-modify]//
  mSerialIrqConfig.SirqMode         = PcdGet8 ( PcdSerialIrqConfigSirqMode );
//[-end-120710-IB07360203-modify]//  
  mSerialIrqConfig.StartFramePulse  = PcdGet8 ( PcdSerialIrqConfigStartFramePulse );
  //
  // DMI related settings
  //
  mPchDmiConfig.DmiAspm    = SetupVariable->PchDmiAspm;
  mPchDmiConfig.DmiExtSync = SetupVariable->PchDmiExtSync;
  mPchDmiConfig.DmiIot      = PcdGet8 ( PcdPchDmiConfigDmiIot );
  
  ///
  /// Power Optimizer related settings
  ///
//[-start-120628-IB06460410-modify]//
  mPchPwrOptConfig.PchPwrOptDmi      = PcdGet8 ( PcdPchPwrOptConfigPchPwrOptDmi );
//[-end-120628-IB06460410-modify]//
  mPchPwrOptConfig.PchPwrOptGbe      = PcdGet8 ( PcdPchPwrOptConfigPchPwrOptGbe );
  mPchPwrOptConfig.PchPwrOptXhci     = PcdGet8 ( PcdPchPwrOptConfigPchPwrOptXhci );
  mPchPwrOptConfig.PchPwrOptEhci     = PcdGet8 ( PcdPchPwrOptConfigPchPwrOptEhci );
  mPchPwrOptConfig.PchPwrOptSata     = PcdGet8 ( PcdPchPwrOptConfigPchPwrOptSata );
  mPchPwrOptConfig.MemCloseStateEn   = PcdGet8 ( PcdPchPwrOptConfigMemCloseStateEn );
  mPchPwrOptConfig.InternalObffEn    = PcdGet8 ( PcdPchPwrOptConfigInternalObffEn );
  mPchPwrOptConfig.ExternalObffEn    = PcdGet8 ( PcdPchPwrOptConfigExternalObffEn );
  mPchPwrOptConfig.NumOfDevLtrOverride = PcdGet8 ( PcdPchPwrOptConfigNumOfDevLtrOverride );
  mPchPwrOptConfig.DevLtrOverride    = NULL;
//[-start-120628-IB06460410-add]//
  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].LtrEnable             = PcdGet8 ( PcdPchPwrOptConfigPchPwrOptPcieLtrEnable );
	//
    // De-feature OBFF from LPT-H/LPT-LP.
    // Doesn't enable Obff policy anymore.
    //
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].ObffEnable            = PcdGet8 ( PcdPchPwrOptConfigPchPwrOptPcieObffEnable );
  }
//[-end-120628-IB06460410-add]//
	mPchPwrOptConfig.LegacyDmaDisable  = PCH_DEVICE_DISABLE;
  for (PortIndex = 0; PortIndex < GetPchMaxPciePortNum (); PortIndex++) {
    if (PchSeries == PchLp) {
      mPchPwrOptConfig.PchPwrOptPcie[PortIndex].LtrMaxSnoopLatency                 = 0x1003;
      mPchPwrOptConfig.PchPwrOptPcie[PortIndex].LtrMaxNoSnoopLatency               = 0x1003;
    }
    if (PchSeries == PchH) {
      mPchPwrOptConfig.PchPwrOptPcie[PortIndex].LtrMaxSnoopLatency                 = 0x0846;
      mPchPwrOptConfig.PchPwrOptPcie[PortIndex].LtrMaxNoSnoopLatency               = 0x0846;
    }
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].LtrConfigLock                      = PCH_DEVICE_ENABLE;
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].SnoopLatencyOverrideMode           = 2;
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].SnoopLatencyOverrideMultiplier     = 2;
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].SnoopLatencyOverrideValue          = 60;
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].NonSnoopLatencyOverrideMode        = 2;
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].NonSnoopLatencyOverrideMultiplier  = 2;
    mPchPwrOptConfig.PchPwrOptPcie[PortIndex].NonSnoopLatencyOverrideValue       = 60;
  }
  
  ///
  /// Misc. Config
  ///
  /// FviSmbiosType is the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS Type 14 - Group 
  /// Associations structure - item type. FVI structure uses it as SMBIOS OEM type to provide
  /// version information. The default value is type 221.
  ///
  mPchMiscConfig.FviSmbiosType  = PcdGet8 ( PcdMeMiscConfigFviSmbiosType );

  //
  // DCI (Direct Connect Interface) Configuration
  //
  mPchMiscConfig.DciEn  = PCH_DEVICE_DISABLE;
  //
  // HPET Config
  //
  mPchHpetConfig.BdfValid                        = PcdGetBool ( PcdPchHpetConfigBdfValid );
  
  for (PortIndex = 0; PortIndex <= B_PCH_LPC_HPET7_FUNC; PortIndex++) {
  mPchHpetConfig.Hpet[PortIndex ].BusNumber      = PcdGet8 ( PcdPchHpetConfigBusNumber );
  mPchHpetConfig.Hpet[PortIndex ].DeviceNumber   = PcdGet8 ( PcdPchHpetConfigDeviceNumber );
  mPchHpetConfig.Hpet[PortIndex ].FunctionNumber = PcdGet8 ( PchPchHpetConfigFunctionNumber );
  }

//[-start-120628-IB06460410-add]//
  if (FeaturePcdGet ( PcdUltFlag ) ) {
//[-start-120807-IB04770241-modify]//
//[-start-121002-IB06150249-modify]//
    mAudioDspConfig.AudioDspD3PowerGating       = SetupVariable->AudioDspD3PowerGating;
//[-start-120917-IB03780460-add]//
    mAudioDspConfig.AudioDspBluetoothSupport    = SetupVariable->AudioDspBluetoothSupport;
//[-end-120917-IB03780460-add]//
    mAudioDspConfig.AudioDspAcpiMode            = SetupVariable->AudioDspAcpiMode; //1: ACPI mode, 0: PCI mode
//[-start-121008-IB10370023-add]//
    mAudioDspConfig.AudioDspAcpiInterruptMode   = PcdGet8 ( PcdAudioDspConfigAudioDspAcpiInterruptMode ); //1: ACPI mode, 0: PCI mode
//[-end-121008-IB10370023-add]//
//[-start-121008-IB10370023-remove]//
//  mAudioDspConfig.AudioDspSafeMode            = SetupVariable->AudioDspSafeMode;
//[-end-121008-IB10370023-remove]//
//[-end-121002-IB06150249-modify]//

//[-start-121113-IB06150259-modify]//
    ///
    /// Serial IO Configuration
    ///
    mSerialIoConfig.SerialIoMode                = SetupVariable->SerialIoMode;
    switch(PchStepping()) {
    default:
      mSerialIoConfig.SerialIoInterruptMode = PchSerialIoIsAcpi;
      break;
    }
    mSerialIoConfig.Ddr50Support                    = PCH_DEVICE_DISABLE;
    
    mSerialIoConfig.I2c0VoltageSelect               = PcdGet8 ( PcdSerialIoConfigI2C0VoltageSelect );
    mSerialIoConfig.I2c1VoltageSelect               = PcdGet8 ( PcdSerialIoConfigI2C1VoltageSelect );
//[-end-121113-IB06150259-modify]//

//[-end-120713-IB06460418-modify]//
//[-end-120807-IB04770241-modify]//
//[-start-130422-IB05400398-remove]//
////[-start-121126-IB06460466-add]//
//    if (PlatformInfoProtocol->PlatInfo.BoardId == V_EC_GET_CRB_BOARD_ID_BOARD_ID_HARRIS_BEACH) {
//      mSerialIoConfig.I2c0VoltageSelect         = PchSerialIoIs33V;
//      mPchDeviceEnabling.SerialIoDma            = SetupVariable->EnableSerialIoDma;
//      mPchDeviceEnabling.SerialIoI2c0           = SetupVariable->EnableSerialIoI2c0;
//      mPchDeviceEnabling.SerialIoI2c1           = SetupVariable->EnableSerialIoI2c1;
//      mPchDeviceEnabling.SerialIoSpi0           = PCH_DEVICE_DISABLE;
//      mPchDeviceEnabling.SerialIoSpi1           = PCH_DEVICE_DISABLE;
//      mPchDeviceEnabling.SerialIoUart0          = SetupVariable->EnableSerialIoUart0;
//      mPchDeviceEnabling.SerialIoUart1          = SetupVariable->EnableSerialIoUart1;
//  
//      mPchDeviceEnabling.SerialIoSdio           = SetupVariable->EnableSerialIoSdio;
//      mPchDeviceEnabling.AudioDsp               = PCH_DEVICE_DISABLE;  
//    }
////[-end-121126-IB06460466-add]//
//[-end-130422-IB05400398-remove]//
  }
//[-end-120628-IB06460410-add]//

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Install PCH Policy Protocol\n" ) );

#ifdef USB_PRECONDITION_ENABLE_FLAG
  ///
  /// Update Precondition option for S4 resume. 
  /// Skip Precondition for S4 resume in case this boot may not connect BIOS USB driver.
  /// If BIOS USB driver will be connected always for S4, then disable below update.
  /// To keep consistency during boot, must enabled or disabled below function in both PEI and DXE
  /// PlatformPolicyInit driver.
  ///
  if (mPchUsbConfig.UsbPrecondition == TRUE) {
    if (GetBootModeHob () == BOOT_ON_S4_RESUME) {
      mPchUsbConfig.UsbPrecondition = FALSE;
      DEBUG ((EFI_D_INFO, "BootMode is BOOT_ON_S4_RESUME, disable Precondition\n"));
    }
  }
#endif  // USB_PRECONDITION_ENABLE_FLAG  
//[-start-121220-IB10820206-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdatDxePchPlatformPolicy (&mPchPolicyData);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdatDxePchPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status != EFI_SUCCESS) {
    //
    // Install PCH Policy Protocol.
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
               &Handle,
               &gDxePchPlatformPolicyProtocolGuid,
               EFI_NATIVE_INTERFACE,
               &mPchPolicyData
               );
    ASSERT_EFI_ERROR ( Status );
  }
//[-end-121220-IB10820206-modify]//
  //
  // Dump policy
  //
  DumpPCHPolicy ( &mPchPolicyData );

  PCHPolicyUpdateACPI ();
//[-start-120613-IB03780448-remove]//
//[-start-120417-IB03780438-add]//
//#ifdef THUNDERBOLT_SUPPORT
//  if (SetupVariable->TBTGpio3 == 2) {
//    //
//    // Sample for TBT GPIO 3 is routed to PCH GPIO
    //
//    //
//    // Update TBT GPIO 3 - PCH GPIO 12
//    //
//    GpioBase = PciReadConfig16 (
//                 PciCfgAddr (0, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC) 
//                 + R_PCH_LPC_GPIO_BASE
//               ) & B_PCH_LPC_GPIO_BASE_BAR;
//    
//    GpioOffset = GpioBase + 0x0C;
//    GpioValue = IoInput32 (GpioOffset);
//    IoOutput32 (GpioOffset, GpioValue & ~BIT12);

    //
    // Sample for TBT GPIO 3 is routed to SIO/EC GPIO (GATB CRB rev.002)
    // Update TBT GPIO 3 - SIO/EC GPIO 21
    //
//    SioGpioBase  = 0x379; // EC GPIO Set#2 base address
//    SioGpioValue = IoInput8 (SioGpioBase);  
//    IoOutput8 (SioGpioBase, SioGpioValue & ~BIT1);    
//  }
//#endif
//[-end-120417-IB03780438-add]//
//[-end-120613-IB03780448-remove]//
  return Status;
}

VOID
PCHPolicyUpdateACPI (
  VOID
  )
{
  EFI_STATUS        Status;
  EFI_EVENT         Event;
  VOID              *Registration;

  Registration = NULL;

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "PCHPolicyUpdateACPI Entry\n" ) );

  //
  // We hope it to be executed right after installing GlobalNVS Protocol.
  //
  Status = gBS->CreateEvent (
             EVT_NOTIFY_SIGNAL,
             TPL_NOTIFY,
             PCHPolicyUpdateACPICallback,
             NULL,
             &Event
             );
  if ( !EFI_ERROR ( Status ) ) {
    Status = gBS->RegisterProtocolNotify (
               &gEfiGlobalNvsAreaProtocolGuid,
               Event,
               &Registration
               );
    ASSERT_EFI_ERROR ( Status );
  }
  ASSERT_EFI_ERROR ( Status );

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "PCHPolicyUpdateACPI Exit\n" ) );

  return;
}

VOID
EFIAPI
PCHPolicyUpdateACPICallback (
  IN      EFI_EVENT        Event,
  IN      VOID             *Context
  )
{
  EFI_STATUS                              Status;
  DXE_PCH_PLATFORM_POLICY_PROTOCOL        *PCHPlatformPolicy;
  EFI_GLOBAL_NVS_AREA_PROTOCOL            *GlobalNvsAreaProtocol;
//[-start-130205-IB02950497-add]//
  UINT8                                   PortIndex;
  UINT8                                   ManualModeUsb30PerPinEnable = 0;
  UINT32                                  ManualModeUsb20PerPinRoute = 0;
//[-end-130205-IB02950497-add]//  

  PCHPlatformPolicy     = NULL;
  GlobalNvsAreaProtocol = NULL;

  Status = gBS->CloseEvent ( Event );
  ASSERT_EFI_ERROR ( Status );

  Status = gBS->LocateProtocol ( &gDxePchPlatformPolicyProtocolGuid, NULL, (VOID **)&PCHPlatformPolicy );
  ASSERT_EFI_ERROR ( Status );

  Status = gBS->LocateProtocol ( &gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&GlobalNvsAreaProtocol );
  ASSERT_EFI_ERROR ( Status );

//[-start-120403-IB05300305-modify]//
  GlobalNvsAreaProtocol->Area->XhciMode            = (UINT8)PCHPlatformPolicy->UsbConfig->Usb30Settings.Mode;
//[-end-120403-IB05300305-modify]//
//[-start-130205-IB02950497-add]//
  GlobalNvsAreaProtocol->Area->ManualMode                  = (UINT8)mPchUsbConfig.Usb30Settings.ManualMode;

  for (PortIndex = 0; PortIndex < GetPchUsbMaxPhysicalPortNum (); PortIndex++) {
    ManualModeUsb20PerPinRoute |= mPchUsbConfig.Usb30Settings.ManualModeUsb20PerPinRoute[PortIndex] << PortIndex;
  }
  GlobalNvsAreaProtocol->Area->ManualModeUsb20PerPinRoute  = ManualModeUsb20PerPinRoute;
  
  for (PortIndex = 0; PortIndex < GetPchXhciMaxUsb3PortNum (); PortIndex++) {
    ManualModeUsb30PerPinEnable |= mPchUsbConfig.Usb30Settings.ManualModeUsb30PerPinEnable[PortIndex] << PortIndex;
  }
  GlobalNvsAreaProtocol->Area->ManualModeUsb30PerPinEnable  = ManualModeUsb30PerPinEnable;  
//[-end-130205-IB02950497-add]// 

}

EFI_STATUS
SetPcieConfig (
  IN PLATFORM_HARDWARE_SWITCH          *PlatformHardwareSwitch,
  IN CHIPSET_CONFIGURATION              *SystemConfiguration
)
{
  UINT8                  Index;

  Index      = 0;

  mPchPciExpressConfig.TempRootPortBusNumMin       = PcdGet8 ( PcdPchPciExpressConfigTempRootPortBusNumMin );
  mPchPciExpressConfig.TempRootPortBusNumMax       = PcdGet8 ( PcdPchPciExpressConfigTempRootPortBusNumMax );
  //
  // Root Port Config
  //
  mPchPciExpressConfig.RootPort[0].Enable          = (BOOLEAN)PlatformHardwareSwitch->PciePortSwitch.Config.Port00En;
  if (mPchPciExpressConfig.RootPort[0].Enable == PCH_DEVICE_DISABLE) {
//[-start-120628-IB06460410-modify]//
    for ( Index = 1; Index < GetPchMaxPciePortNum (); ++ Index ) {
//[-end-120628-IB06460410-modify]//
       mPchPciExpressConfig.RootPort[Index].Enable        = PCH_DEVICE_DISABLE;
    }
  } else {
    mPchPciExpressConfig.RootPort[1].Enable        = (BOOLEAN)PlatformHardwareSwitch->PciePortSwitch.Config.Port01En;
    mPchPciExpressConfig.RootPort[2].Enable        = (BOOLEAN)PlatformHardwareSwitch->PciePortSwitch.Config.Port02En;
    mPchPciExpressConfig.RootPort[3].Enable        = (BOOLEAN)PlatformHardwareSwitch->PciePortSwitch.Config.Port03En;
    mPchPciExpressConfig.RootPort[4].Enable        = (BOOLEAN)PlatformHardwareSwitch->PciePortSwitch.Config.Port04En;
    mPchPciExpressConfig.RootPort[5].Enable        = (BOOLEAN)PlatformHardwareSwitch->PciePortSwitch.Config.Port05En;
    mPchPciExpressConfig.RootPort[6].Enable        = (BOOLEAN)PlatformHardwareSwitch->PciePortSwitch.Config.Port06En;
    mPchPciExpressConfig.RootPort[7].Enable        = (BOOLEAN)PlatformHardwareSwitch->PciePortSwitch.Config.Port07En;
  }
  // Whether or not to hide the configuration space of this port
//[-start-120628-IB06460410-modify]//
  for ( Index = 0; Index < GetPchMaxPciePortNum (); ++ Index ) {
//[-end-120628-IB06460410-modify]//
    mPchPciExpressConfig.RootPort[Index].Hide            = PCH_DEVICE_DISABLE;
    mPchPciExpressConfig.RootPort[Index].SlotImplemented = PCH_DEVICE_ENABLE;
//[-start-121211-IB06460478-add]//
    mPchPciExpressConfig.RootPort[Index].L1Substates     = PcdGet8 ( PcdL1Substates );
//[-end-121211-IB06460478-add]//
  }
  
  mPchPciExpressConfig.RootPort[0].HotPlug         = SystemConfiguration->PcieRootPortHotPlug0;
  mPchPciExpressConfig.RootPort[1].HotPlug         = SystemConfiguration->PcieRootPortHotPlug1;
  mPchPciExpressConfig.RootPort[2].HotPlug         = SystemConfiguration->PcieRootPortHotPlug2;
  mPchPciExpressConfig.RootPort[3].HotPlug         = SystemConfiguration->PcieRootPortHotPlug3;
  mPchPciExpressConfig.RootPort[4].HotPlug         = SystemConfiguration->PcieRootPortHotPlug4;
  mPchPciExpressConfig.RootPort[5].HotPlug         = SystemConfiguration->PcieRootPortHotPlug5;
  mPchPciExpressConfig.RootPort[6].HotPlug         = SystemConfiguration->PcieRootPortHotPlug6;
  mPchPciExpressConfig.RootPort[7].HotPlug         = SystemConfiguration->PcieRootPortHotPlug7;

  mPchPciExpressConfig.RootPort[0].PmSci           = SystemConfiguration->PcieRootPortPmeSci0;
  mPchPciExpressConfig.RootPort[1].PmSci           = SystemConfiguration->PcieRootPortPmeSci1;
  mPchPciExpressConfig.RootPort[2].PmSci           = SystemConfiguration->PcieRootPortPmeSci2;
  mPchPciExpressConfig.RootPort[3].PmSci           = SystemConfiguration->PcieRootPortPmeSci3;
  mPchPciExpressConfig.RootPort[4].PmSci           = SystemConfiguration->PcieRootPortPmeSci4;
  mPchPciExpressConfig.RootPort[5].PmSci           = SystemConfiguration->PcieRootPortPmeSci5;
  mPchPciExpressConfig.RootPort[6].PmSci           = SystemConfiguration->PcieRootPortPmeSci6;
  mPchPciExpressConfig.RootPort[7].PmSci           = SystemConfiguration->PcieRootPortPmeSci7;
  // Link Control Register / BIT7
//[-start-120628-IB06460410-modify]//
  for ( Index = 0; Index < GetPchMaxPciePortNum (); ++ Index ) {
//[-end-120628-IB06460410-modify]//
  mPchPciExpressConfig.RootPort[Index].ExtSync     = PCH_DEVICE_DISABLE;  
  }  
  //
  // Error handlings
  //
  mPchPciExpressConfig.RootPort[0].UnsupportedRequestReport      = SystemConfiguration->PcieRootPortURR0;
  mPchPciExpressConfig.RootPort[1].UnsupportedRequestReport      = SystemConfiguration->PcieRootPortURR1;
  mPchPciExpressConfig.RootPort[2].UnsupportedRequestReport      = SystemConfiguration->PcieRootPortURR2;
  mPchPciExpressConfig.RootPort[3].UnsupportedRequestReport      = SystemConfiguration->PcieRootPortURR3;
  mPchPciExpressConfig.RootPort[4].UnsupportedRequestReport      = SystemConfiguration->PcieRootPortURR4;
  mPchPciExpressConfig.RootPort[5].UnsupportedRequestReport      = SystemConfiguration->PcieRootPortURR5;
  mPchPciExpressConfig.RootPort[6].UnsupportedRequestReport      = SystemConfiguration->PcieRootPortURR6;
  mPchPciExpressConfig.RootPort[7].UnsupportedRequestReport      = SystemConfiguration->PcieRootPortURR7;

  mPchPciExpressConfig.RootPort[0].FatalErrorReport              = SystemConfiguration->PcieRootPortFER0;
  mPchPciExpressConfig.RootPort[1].FatalErrorReport              = SystemConfiguration->PcieRootPortFER1;
  mPchPciExpressConfig.RootPort[2].FatalErrorReport              = SystemConfiguration->PcieRootPortFER2;
  mPchPciExpressConfig.RootPort[3].FatalErrorReport              = SystemConfiguration->PcieRootPortFER3;
  mPchPciExpressConfig.RootPort[4].FatalErrorReport              = SystemConfiguration->PcieRootPortFER4;
  mPchPciExpressConfig.RootPort[5].FatalErrorReport              = SystemConfiguration->PcieRootPortFER5;
  mPchPciExpressConfig.RootPort[6].FatalErrorReport              = SystemConfiguration->PcieRootPortFER6;
  mPchPciExpressConfig.RootPort[7].FatalErrorReport              = SystemConfiguration->PcieRootPortFER7;

  mPchPciExpressConfig.RootPort[0].NoFatalErrorReport            = SystemConfiguration->PcieRootPortNFER0;
  mPchPciExpressConfig.RootPort[1].NoFatalErrorReport            = SystemConfiguration->PcieRootPortNFER1;
  mPchPciExpressConfig.RootPort[2].NoFatalErrorReport            = SystemConfiguration->PcieRootPortNFER2;
  mPchPciExpressConfig.RootPort[3].NoFatalErrorReport            = SystemConfiguration->PcieRootPortNFER3;
  mPchPciExpressConfig.RootPort[4].NoFatalErrorReport            = SystemConfiguration->PcieRootPortNFER4;
  mPchPciExpressConfig.RootPort[5].NoFatalErrorReport            = SystemConfiguration->PcieRootPortNFER5;
  mPchPciExpressConfig.RootPort[6].NoFatalErrorReport            = SystemConfiguration->PcieRootPortNFER6;
  mPchPciExpressConfig.RootPort[7].NoFatalErrorReport            = SystemConfiguration->PcieRootPortNFER7;

  mPchPciExpressConfig.RootPort[0].CorrectableErrorReport        = SystemConfiguration->PcieRootPortCER0;
  mPchPciExpressConfig.RootPort[1].CorrectableErrorReport        = SystemConfiguration->PcieRootPortCER1;
  mPchPciExpressConfig.RootPort[2].CorrectableErrorReport        = SystemConfiguration->PcieRootPortCER2;
  mPchPciExpressConfig.RootPort[3].CorrectableErrorReport        = SystemConfiguration->PcieRootPortCER3;
  mPchPciExpressConfig.RootPort[4].CorrectableErrorReport        = SystemConfiguration->PcieRootPortCER4;
  mPchPciExpressConfig.RootPort[5].CorrectableErrorReport        = SystemConfiguration->PcieRootPortCER5;
  mPchPciExpressConfig.RootPort[6].CorrectableErrorReport        = SystemConfiguration->PcieRootPortCER6;
  mPchPciExpressConfig.RootPort[7].CorrectableErrorReport        = SystemConfiguration->PcieRootPortCER7;

  mPchPciExpressConfig.RootPort[0].PmeInterrupt                  = SystemConfiguration->PcieRootPortPmeInt0;
  mPchPciExpressConfig.RootPort[1].PmeInterrupt                  = SystemConfiguration->PcieRootPortPmeInt1;
  mPchPciExpressConfig.RootPort[2].PmeInterrupt                  = SystemConfiguration->PcieRootPortPmeInt2;
  mPchPciExpressConfig.RootPort[3].PmeInterrupt                  = SystemConfiguration->PcieRootPortPmeInt3;
  mPchPciExpressConfig.RootPort[4].PmeInterrupt                  = SystemConfiguration->PcieRootPortPmeInt4;
  mPchPciExpressConfig.RootPort[5].PmeInterrupt                  = SystemConfiguration->PcieRootPortPmeInt5;
  mPchPciExpressConfig.RootPort[6].PmeInterrupt                  = SystemConfiguration->PcieRootPortPmeInt6;
  mPchPciExpressConfig.RootPort[7].PmeInterrupt                  = SystemConfiguration->PcieRootPortPmeInt7;

  mPchPciExpressConfig.RootPort[0].SystemErrorOnFatalError       = SystemConfiguration->PcieRootPortSEFE0;
  mPchPciExpressConfig.RootPort[1].SystemErrorOnFatalError       = SystemConfiguration->PcieRootPortSEFE1;
  mPchPciExpressConfig.RootPort[2].SystemErrorOnFatalError       = SystemConfiguration->PcieRootPortSEFE2;
  mPchPciExpressConfig.RootPort[3].SystemErrorOnFatalError       = SystemConfiguration->PcieRootPortSEFE3;
  mPchPciExpressConfig.RootPort[4].SystemErrorOnFatalError       = SystemConfiguration->PcieRootPortSEFE4;
  mPchPciExpressConfig.RootPort[5].SystemErrorOnFatalError       = SystemConfiguration->PcieRootPortSEFE5;
  mPchPciExpressConfig.RootPort[6].SystemErrorOnFatalError       = SystemConfiguration->PcieRootPortSEFE6;
  mPchPciExpressConfig.RootPort[7].SystemErrorOnFatalError       = SystemConfiguration->PcieRootPortSEFE7;

  mPchPciExpressConfig.RootPort[0].SystemErrorOnNonFatalError    = SystemConfiguration->PcieRootPortSENFE0;
  mPchPciExpressConfig.RootPort[1].SystemErrorOnNonFatalError    = SystemConfiguration->PcieRootPortSENFE1;
  mPchPciExpressConfig.RootPort[2].SystemErrorOnNonFatalError    = SystemConfiguration->PcieRootPortSENFE2;
  mPchPciExpressConfig.RootPort[3].SystemErrorOnNonFatalError    = SystemConfiguration->PcieRootPortSENFE3;
  mPchPciExpressConfig.RootPort[4].SystemErrorOnNonFatalError    = SystemConfiguration->PcieRootPortSENFE4;
  mPchPciExpressConfig.RootPort[5].SystemErrorOnNonFatalError    = SystemConfiguration->PcieRootPortSENFE5;
  mPchPciExpressConfig.RootPort[6].SystemErrorOnNonFatalError    = SystemConfiguration->PcieRootPortSENFE6;
  mPchPciExpressConfig.RootPort[7].SystemErrorOnNonFatalError    = SystemConfiguration->PcieRootPortSENFE7;

  mPchPciExpressConfig.RootPort[0].SystemErrorOnCorrectableError = SystemConfiguration->PcieRootPortSECE0;
  mPchPciExpressConfig.RootPort[1].SystemErrorOnCorrectableError = SystemConfiguration->PcieRootPortSECE1;
  mPchPciExpressConfig.RootPort[2].SystemErrorOnCorrectableError = SystemConfiguration->PcieRootPortSECE2;
  mPchPciExpressConfig.RootPort[3].SystemErrorOnCorrectableError = SystemConfiguration->PcieRootPortSECE3;
  mPchPciExpressConfig.RootPort[4].SystemErrorOnCorrectableError = SystemConfiguration->PcieRootPortSECE4;
  mPchPciExpressConfig.RootPort[5].SystemErrorOnCorrectableError = SystemConfiguration->PcieRootPortSECE5;
  mPchPciExpressConfig.RootPort[6].SystemErrorOnCorrectableError = SystemConfiguration->PcieRootPortSECE6;
  mPchPciExpressConfig.RootPort[7].SystemErrorOnCorrectableError = SystemConfiguration->PcieRootPortSECE7;
//[-start-120628-IB06460410-modify]//
  for ( Index = 0; Index < GetPchMaxPciePortNum (); ++ Index ) {
//[-end-120628-IB06460410-modify]//
    mPchPciExpressConfig.RootPort[Index].AdvancedErrorReporting  = PCH_DEVICE_DISABLE;
    // The function number this root port is mapped to.
    mPchPciExpressConfig.RootPort[Index].FunctionNumber          = Index;                 
    mPchPciExpressConfig.RootPort[Index].PhysicalSlotNumber      = Index;
  } 
  mPchPciExpressConfig.RootPort[0].CompletionTimeout             = SystemConfiguration->PcieRootPortCTO0;
  mPchPciExpressConfig.RootPort[1].CompletionTimeout             = SystemConfiguration->PcieRootPortCTO1;
  mPchPciExpressConfig.RootPort[2].CompletionTimeout             = SystemConfiguration->PcieRootPortCTO2;
  mPchPciExpressConfig.RootPort[3].CompletionTimeout             = SystemConfiguration->PcieRootPortCTO3;
  mPchPciExpressConfig.RootPort[4].CompletionTimeout             = SystemConfiguration->PcieRootPortCTO4;
  mPchPciExpressConfig.RootPort[5].CompletionTimeout             = SystemConfiguration->PcieRootPortCTO5;
  mPchPciExpressConfig.RootPort[6].CompletionTimeout             = SystemConfiguration->PcieRootPortCTO6;
  mPchPciExpressConfig.RootPort[7].CompletionTimeout             = SystemConfiguration->PcieRootPortCTO7; 
  //
  // directly connected to root port.
  // Does not support ASPM for endpoints downstream a switch. Should implement an enumerator for this purpose.
  // 
  // Currently only support ASPM setting for endpoint
  mPchPciExpressConfig.RootPort[0].Aspm                          = SystemConfiguration->PcieRootPortAspm0; 
  mPchPciExpressConfig.RootPort[1].Aspm                          = SystemConfiguration->PcieRootPortAspm1;
  mPchPciExpressConfig.RootPort[2].Aspm                          = SystemConfiguration->PcieRootPortAspm2;
  mPchPciExpressConfig.RootPort[3].Aspm                          = SystemConfiguration->PcieRootPortAspm3;
  mPchPciExpressConfig.RootPort[4].Aspm                          = SystemConfiguration->PcieRootPortAspm4;
  mPchPciExpressConfig.RootPort[5].Aspm                          = SystemConfiguration->PcieRootPortAspm5;
  mPchPciExpressConfig.RootPort[6].Aspm                          = SystemConfiguration->PcieRootPortAspm6;
  mPchPciExpressConfig.RootPort[7].Aspm                          = SystemConfiguration->PcieRootPortAspm7; 
  
  //
  // ASPM
  //
  mPchPciExpressConfig.NumOfDevAspmOverride = sizeof (mDevAspmOverride) / sizeof (PCH_PCIE_DEVICE_ASPM_OVERRIDE);
  mPchPciExpressConfig.DevAspmOverride      = mDevAspmOverride;

  mPchPciExpressConfig.PchPcieSbdePort                           = PcdGet8 ( PcdPchPciExpressConfigPchPcieSbdePort );
  mPchPciExpressConfig.RootPortClockGating                       = SystemConfiguration->PchPcieClockGating;
  mPchPciExpressConfig.EnableSubDecode                           = PcdGet8 ( PcdPchPciExpressConfigEnableSubDecode );
  mPchPciExpressConfig.RootPortFunctionSwapping                  = 1;

//[-start-130307-IB03780481-add]//
  if (FeaturePcdGet (PcdThunderBoltSupported)) {
//[-start-130709-IB05400426-modify]//
  //
  // Override HotPlug setting for Thunderbolt.
  //
//[-start-140625-IB05080432-modify]//
  if ((SystemConfiguration->TbtDevice != 0x00) && 
      (SystemConfiguration->TbtHotPlug) &&
      (SystemConfiguration->TbtDevice <= LPTH_PCIE_MAX_ROOT_PORTS)) {
//[-end-140625-IB05080432-modify]//
    mPchPciExpressConfig.RootPort[(SystemConfiguration->TbtDevice - 1)].HotPlug = PCH_DEVICE_ENABLE;
  }
//[-end-130709-IB05400426-modify]//

//[-start-130429-IB05160441-add]//
    //
    // Because Thunderbolt use fixed pcie FUN_NUM ==> skip root port swapping!
    //
    mPchPciExpressConfig.RootPortFunctionSwapping                  = 0;
//[-end-130429-IB05160441-add]//
  }
//[-end-130307-IB03780481-add]//

  return EFI_SUCCESS;
}

VOID
DumpPCHPolicy (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL        *PchPlatformPolicy
  )
{
  UINTN        Count;

  Count = 0;

//[-start-120628-IB06460410-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\n" ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "PCHPlatformPolicy ( Address : 0x%x )\n", PchPlatformPolicy ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-Revision                                       : %x\n", PchPlatformPolicy->Revision ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-BusNumber                                      : %x\n", PchPlatformPolicy->BusNumber) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-DeviceEnabling ( Address : 0x%x )\n", PchPlatformPolicy->DeviceEnabling) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Lan                                         : %x\n", PchPlatformPolicy->DeviceEnabling->Lan) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Azalia                                      : %x\n", PchPlatformPolicy->DeviceEnabling->Azalia) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Sata                                        : %x\n", PchPlatformPolicy->DeviceEnabling->Sata) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Smbus                                       : %x\n", PchPlatformPolicy->DeviceEnabling->Smbus) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PciClockRun                                 : %x\n", PchPlatformPolicy->DeviceEnabling->PciClockRun) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Display                                     : %x\n", PchPlatformPolicy->DeviceEnabling->Display) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Crid                                        : %x\n", PchPlatformPolicy->DeviceEnabling->Crid) );  
//[-start-121113-IB06150259-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoDma                                     : %x\n", PchPlatformPolicy->DeviceEnabling->SerialIoDma) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoI2c0                                    : %x\n", PchPlatformPolicy->DeviceEnabling->SerialIoI2c0) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoI2c1                                    : %x\n", PchPlatformPolicy->DeviceEnabling->SerialIoI2c1) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoSpi0                                    : %x\n", PchPlatformPolicy->DeviceEnabling->SerialIoSpi0) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoSpi1                                    : %x\n", PchPlatformPolicy->DeviceEnabling->SerialIoSpi1) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoUart0                                   : %x\n", PchPlatformPolicy->DeviceEnabling->SerialIoUart0) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoUart1                                   : %x\n", PchPlatformPolicy->DeviceEnabling->SerialIoUart1) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoSdio                                    : %x\n", PchPlatformPolicy->DeviceEnabling->SerialIoSdio) );  
//[-end-121113-IB06150259-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AudioDsp                                    : %x\n", PchPlatformPolicy->DeviceEnabling->AudioDsp) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-UsbConfig      ( Address : 0x%x )\n", PchPlatformPolicy->UsbConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-UsbPerPortCtl                               : %x\n", PchPlatformPolicy->UsbConfig->UsbPerPortCtl) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Ehci1Usbr                                   : %x\n", PchPlatformPolicy->UsbConfig->Ehci1Usbr) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Ehci2Usbr                                   : %x\n", PchPlatformPolicy->UsbConfig->Ehci2Usbr) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-RsvdBits                                    : %x\n", PchPlatformPolicy->UsbConfig->RsvdBits) );
  for ( Count = 0; Count < GetPchUsbMaxPhysicalPortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PortSettings[%2x] ( Address : 0x%x )\n", Count,&PchPlatformPolicy->UsbConfig->PortSettings[Count]) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Enable                                    : %x\n", PchPlatformPolicy->UsbConfig->PortSettings[Count].Enable) );  
//[-start-120917-IB03780460-remove]//
//    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Panel                                     : %x\n", PchPlatformPolicy->UsbConfig->PortSettings[Count].Panel) ); 
//    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Dock                                      : %x\n", PchPlatformPolicy->UsbConfig->PortSettings[Count].Dock) ); 
//[-end-120917-IB03780460-remove]//
//[-start-120917-IB03780460-add]//
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Location                                  : %x\n", PchPlatformPolicy->UsbConfig->PortSettings[Count].Location) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Usb20PortLength                           : %x\n", PchPlatformPolicy->UsbConfig->PortSettings[Count].Usb20PortLength) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Usb20EyeDiagramTuningParam1               : %x\n", PchPlatformPolicy->UsbConfig->PortSettings[Count].Usb20EyeDiagramTuningParam1) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Usb20EyeDiagramTuningParam2               : %x\n", PchPlatformPolicy->UsbConfig->PortSettings[Count].Usb20EyeDiagramTuningParam2) );
//[-end-120917-IB03780460-add]//
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                  : %x\n", PchPlatformPolicy->UsbConfig->PortSettings[Count].Rsvdbits) );  
  }
  for ( Count = 0; Count < PchEhciControllerMax; ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Usb20Setting[%2x] ( Address : 0x%x )\n", Count,&PchPlatformPolicy->UsbConfig->Usb20Settings[Count]) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Enable                                    : %x\n", PchPlatformPolicy->UsbConfig->Usb20Settings[Count].Enable) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                  : %x\n", PchPlatformPolicy->UsbConfig->Usb20Settings[Count].Rsvdbits) );  
  }
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Usb30Settings     ( Address : 0x%x )\n", &PchPlatformPolicy->UsbConfig->Usb30Settings) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Mode                                     : %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.Mode) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PreBootSupport                           : %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.PreBootSupport) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-XhciStreams                              : %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.XhciStreams) );
//[-start-120807-IB04770241-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-ManualMode                               : %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.ManualMode) );    
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                 : %x\n", PchPlatformPolicy->UsbConfig->Usb30Settings.Rsvdbits) );    
  for ( Count = 0; Count < GetPchUsbMaxPhysicalPortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-ManualModeUsb20PerPinRoute[%2x]          : %x\n", Count,PchPlatformPolicy->UsbConfig->Usb30Settings.ManualModeUsb20PerPinRoute[Count]) );  
  }
  for ( Count = 0; Count < GetPchXhciMaxUsb3PortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-ManualModeUsb30PerPinEnable[%2x]         : %x\n", Count,PchPlatformPolicy->UsbConfig->Usb30Settings.ManualModeUsb30PerPinEnable[Count]) );
  }
//[-end-120807-IB04770241-modify]//
  for ( Count = 0; Count < GetPchUsbMaxPhysicalPortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Usb20OverCurrentPins[%2x]                    : %x\n", Count,PchPlatformPolicy->UsbConfig->Usb20OverCurrentPins[Count]) );  
  }
  for ( Count = 0; Count < GetPchXhciMaxUsb3PortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Usb30OverCurrentPins[%2x]                    : %x\n", Count,PchPlatformPolicy->UsbConfig->Usb30OverCurrentPins[Count]) );
  }
//[-start-120917-IB03780460-remove]//
//  for ( Count = 0; Count < GetPchUsbMaxPhysicalPortNum (); ++Count ) {
//    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Usb20PortLength[%2x]                         : %x\n", Count,PchPlatformPolicy->UsbConfig->Usb20PortLength[Count]) );  
//  }
//[-end-120917-IB03780460-remove]//
//[-start-120807-IB04770241-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-UsbPrecondition                              : %x\n", PchPlatformPolicy->UsbConfig->UsbPrecondition) );    
//[-end-120807-IB04770241-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Port30Settings\n") );
  for ( Count = 0; Count < GetPchXhciMaxUsb3PortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Port30Settings[%2x].Enable              : %x\n", Count,PchPlatformPolicy->UsbConfig->Port30Settings[Count].Enable) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Port30Settings[%2x].Rsvdbits            : %x\n", Count,PchPlatformPolicy->UsbConfig->Port30Settings[Count].Rsvdbits) );
  }
  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-PciExpressConfig    ( Address : 0x%x )\n", PchPlatformPolicy->PciExpressConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-TempRootPortBusNumMin                       : %x\n", PchPlatformPolicy->PciExpressConfig->TempRootPortBusNumMin) );   
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-TempRootPortBusNumMax                       : %x\n", PchPlatformPolicy->PciExpressConfig->TempRootPortBusNumMax) );  
  for ( Count = 0; Count < GetPchMaxPciePortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-RootPort[%2x]   ( Address : 0x%x )\n", Count,&PchPlatformPolicy->PciExpressConfig->RootPort[Count]) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Enable                                    : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].Enable) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Hide                                      : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].Hide) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SlotImplemented                           : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].SlotImplemented) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-HotPlug                                   : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].HotPlug) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PmSci                                     : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].PmSci) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-ExtSync                                   : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].ExtSync) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                  : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].Rsvdbits) );      
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-UnsupportedRequestReport                  : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].UnsupportedRequestReport) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-FatalErrorReport                          : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].FatalErrorReport) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-NoFatalErrorReport                        : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].NoFatalErrorReport) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-CorrectableErrorReport                    : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].CorrectableErrorReport) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PmeInterrupt                              : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].PmeInterrupt) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SystemErrorOnFatalError                   : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].SystemErrorOnFatalError) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SystemErrorOnNonFatalError                : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].SystemErrorOnNonFatalError) );      
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SystemErrorOnCorrectableError             : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].SystemErrorOnCorrectableError) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-AdvancedErrorReporting                    : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].AdvancedErrorReporting) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-TransmitterHalfSwing                      : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].TransmitterHalfSwing) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Reserved                                  : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].Reserved) ); 
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-FunctionNumber                            : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].FunctionNumber) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PhysicalSlotNumber                        : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].PhysicalSlotNumber) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-CompletionTimeout                         : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].CompletionTimeout) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Aspm                                      : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].Aspm) );      
//[-start-121211-IB06460478-add]//
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-L1Substates                               : %x\n", PchPlatformPolicy->PciExpressConfig->RootPort[Count].L1Substates) );
//[-end-121211-IB06460478-add]//
  }
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-NumOfDevAspmOverride                        : %x\n", PchPlatformPolicy->PciExpressConfig->NumOfDevAspmOverride) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DevAspmOverride   ( Address : 0x%x )\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-VendorId                                 : %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->VendorId) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DeviceId                                 : %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->DeviceId) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-RevId                                    : %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->RevId) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-BaseClassCode                            : %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->BaseClassCode) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SubClassCode                             : %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->SubClassCode) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-EndPointAspm                             : %x\n", PchPlatformPolicy->PciExpressConfig->DevAspmOverride->EndPointAspm) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchPcieSbdePort                             : %x\n", PchPlatformPolicy->PciExpressConfig->PchPcieSbdePort) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-RootPortClockGating                         : %x\n", PchPlatformPolicy->PciExpressConfig->RootPortClockGating) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-EnableSubDecode                             : %x\n", PchPlatformPolicy->PciExpressConfig->EnableSubDecode) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->PciExpressConfig->Rsvdbits) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-SataConfig           ( Address : 0x%x )\n", PchPlatformPolicy->SataConfig) );
  for ( Count = 0; Count < GetPchMaxSataPortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PortSettings[%2x]( Address : 0x%x )\n", Count, &PchPlatformPolicy->SataConfig->PortSettings[Count]) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Enable                                    : %x\n", PchPlatformPolicy->SataConfig->PortSettings[Count].Enable) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-HotPlug                                   : %x\n", PchPlatformPolicy->SataConfig->PortSettings[Count].HotPlug) );     
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-InterlockSw                               : %x\n", PchPlatformPolicy->SataConfig->PortSettings[Count].InterlockSw) ); 
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-External                                  : %x\n", PchPlatformPolicy->SataConfig->PortSettings[Count].External) );     
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SpinUp                                    : %x\n", PchPlatformPolicy->SataConfig->PortSettings[Count].SpinUp) ); 
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SolidStateDrive                           : %x\n", PchPlatformPolicy->SataConfig->PortSettings[Count].SolidStateDrive) ); 
//[-start-120904-IB03780460-add]//
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DevSlp                                    : %x\n", PchPlatformPolicy->SataConfig->PortSettings[Count].DevSlp) );
//[-end-120904-IB03780460-add]//
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                  : %x\n", PchPlatformPolicy->SataConfig->PortSettings[Count].Rsvdbits) );     
  }
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-RaidAlternateId                             : %x\n", PchPlatformPolicy->SataConfig->RaidAlternateId) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Raid0                                       : %x\n", PchPlatformPolicy->SataConfig->Raid0) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Raid1                                       : %x\n", PchPlatformPolicy->SataConfig->Raid1) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Raid10                                      : %x\n", PchPlatformPolicy->SataConfig->Raid10) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Raid5                                       : %x\n", PchPlatformPolicy->SataConfig->Raid5) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Irrt                                        : %x\n", PchPlatformPolicy->SataConfig->Irrt) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-OromUiBanner                                : %x\n", PchPlatformPolicy->SataConfig->OromUiBanner) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-HddUnlock                                   : %x\n", PchPlatformPolicy->SataConfig->HddUnlock) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-LedLocate                                   : %x\n", PchPlatformPolicy->SataConfig->LedLocate) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-IrrtOnly                                    : %x\n", PchPlatformPolicy->SataConfig->IrrtOnly) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-TestMode                                    : %x\n", PchPlatformPolicy->SataConfig->TestMode) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SalpSupport                                 : %x\n", PchPlatformPolicy->SataConfig->SalpSupport) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-LegacyMode                                  : %x\n", PchPlatformPolicy->SataConfig->LegacyMode) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SmartStorage                                : %x\n", PchPlatformPolicy->SataConfig->SmartStorage) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-OromUiDelay                                 : %x\n", PchPlatformPolicy->SataConfig->OromUiDelay) );   
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SpeedSupport                                : %x\n", PchPlatformPolicy->SataConfig->SpeedSupport) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->SataConfig->Rsvdbits) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-AzaliaConfig        ( Address : 0x%x )\n", PchPlatformPolicy->AzaliaConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Pme                                         : %x\n", PchPlatformPolicy->AzaliaConfig->Pme) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DS                                          : %x\n", PchPlatformPolicy->AzaliaConfig->DS) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DA                                          : %x\n", PchPlatformPolicy->AzaliaConfig->DA) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->AzaliaConfig->Rsvdbits) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AzaliaVerbTableNum                          : %x\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTableNum) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AzaliaVerbTable   ( Address : 0x%x )\n", PchPlatformPolicy->AzaliaConfig->AzaliaVerbTable) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ResetWaitTimer                              : %x\n", PchPlatformPolicy->AzaliaConfig->ResetWaitTimer) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-SmbusConfig          ( Address : 0x%x )\n", PchPlatformPolicy->SmbusConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-NumRsvdSmbusAddresses                       : %x\n", PchPlatformPolicy->SmbusConfig->NumRsvdSmbusAddresses) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-RsvdSmbusAddressTable ( Address : 0x%x )\n", PchPlatformPolicy->SmbusConfig->RsvdSmbusAddressTable) );   
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-MiscPmConfig         ( Address : 0x%x )\n", PchPlatformPolicy->MiscPmConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PowerResetStatusClear ( Address : 0x%x )\n", &PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-MeWakeSts                                : %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeWakeSts) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-MeHrstColdSts                            : %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHrstColdSts) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-MeHrstWarmSts                            : %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHrstWarmSts) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-MeHostPowerDn                            : %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.MeHostPowerDn) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-WolOvrWkSts                              : %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.WolOvrWkSts) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                 : %x\n", PchPlatformPolicy->MiscPmConfig->PowerResetStatusClear.Rsvdbits) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-WakeConfig        ( Address : 0x%x )\n", &PchPlatformPolicy->MiscPmConfig->WakeConfig) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PmeB0S5Dis                               : %x\n", PchPlatformPolicy->MiscPmConfig->WakeConfig.PmeB0S5Dis) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-WolEnableOverride                        : %x\n", PchPlatformPolicy->MiscPmConfig->WakeConfig.WolEnableOverride) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Gp27WakeFromDeepSx                       : %x\n", PchPlatformPolicy->MiscPmConfig->WakeConfig.Gp27WakeFromDeepSx) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PcieWakeFromDeepSx                       : %x\n", PchPlatformPolicy->MiscPmConfig->WakeConfig.PcieWakeFromDeepSx) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                 : %x\n", PchPlatformPolicy->MiscPmConfig->WakeConfig.Rsvdbits) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchDeepSxPol                                : %x\n", PchPlatformPolicy->MiscPmConfig->PchDeepSxPol) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchSlpS3MinAssert                           : %x\n", PchPlatformPolicy->MiscPmConfig->PchSlpS3MinAssert) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchSlpS4MinAssert                           : %x\n", PchPlatformPolicy->MiscPmConfig->PchSlpS4MinAssert) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchSlpSusMinAssert                          : %x\n", PchPlatformPolicy->MiscPmConfig->PchSlpSusMinAssert) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchSlpAMinAssert                            : %x\n", PchPlatformPolicy->MiscPmConfig->PchSlpAMinAssert) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SlpStrchSusUp                               : %x\n", PchPlatformPolicy->MiscPmConfig->SlpStrchSusUp) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SlpLanLowDc                                 : %x\n", PchPlatformPolicy->MiscPmConfig->SlpLanLowDc) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->MiscPmConfig->Rsvdbits) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchPwrCycDur                                : %x\n", PchPlatformPolicy->MiscPmConfig->PchPwrCycDur) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-IoApicConfig         ( Address : 0x%x )\n", PchPlatformPolicy->IoApicConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-BdfValid                                    : %x\n", PchPlatformPolicy->IoApicConfig->BdfValid) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-BusNumber                                   : %x\n", PchPlatformPolicy->IoApicConfig->BusNumber) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DeviceNumber                                : %x\n", PchPlatformPolicy->IoApicConfig->DeviceNumber) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-FunctionNumber                              : %x\n", PchPlatformPolicy->IoApicConfig->FunctionNumber) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-IoApicEntry24_39                            : %x\n", PchPlatformPolicy->IoApicConfig->IoApicEntry24_39) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-DefaultSvidSid       ( Address : 0x%x )\n", PchPlatformPolicy->DefaultSvidSid) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SubSystemVendorId                           : %x\n", PchPlatformPolicy->DefaultSvidSid->SubSystemVendorId) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SubSystemId                                 : %x\n", PchPlatformPolicy->DefaultSvidSid->SubSystemId) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-LockDownConfig       ( Address : 0x%x )\n", PchPlatformPolicy->LockDownConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-GlobalSmi                                   : %x\n", PchPlatformPolicy->LockDownConfig->GlobalSmi) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-BiosInterface                               : %x\n", PchPlatformPolicy->LockDownConfig->BiosInterface) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-GpioLockDown                                : %x\n", PchPlatformPolicy->LockDownConfig->GpioLockDown) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-RtcLock                                     : %x\n", PchPlatformPolicy->LockDownConfig->RtcLock) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-BiosLock                                    : %x\n", PchPlatformPolicy->LockDownConfig->BiosLock) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->LockDownConfig->Rsvdbits) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-TcoLock                                     : %x\n", PchPlatformPolicy->LockDownConfig->TcoLock) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits0                                   : %x\n", PchPlatformPolicy->LockDownConfig->Rsvdbits0) );
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchBiosLockSwSmiNumber                      : %x\n", PchPlatformPolicy->LockDownConfig->PchBiosLockSwSmiNumber) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-ThermalConfig        ( Address : 0x%x )\n", PchPlatformPolicy->ThermalConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ThermalAlertEnable ( Address : 0x%x )\n", &PchPlatformPolicy->ThermalConfig->ThermalAlertEnable) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-TselLock                         : %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TselLock) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-TscLock                         : %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TscLock) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-TsmicLock                          : %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TsmicLock) );  
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-TspcLock                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.TspcLock) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-PhlcLock                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.PhlcLock) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalAlertEnable.Rsvdbits) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ThermalDeviceEnable                         : %x\n", PchPlatformPolicy->ThermalConfig->ThermalDeviceEnable) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ThermalThrottling ( Address : 0x%x )\n", &PchPlatformPolicy->ThermalConfig->ThermalThrottling) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-TTLevels       ( Address : 0x%x )\n", &PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-T0Level                               : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T0Level) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-T1Level                               : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T1Level) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-T2Level                               : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.T2Level) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-TTEnable                              : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTEnable) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-TTState13Enable                       : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTState13Enable) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-TTLock                                : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.TTLock) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-SuggestedSetting                      : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.SuggestedSetting) );  
//[-start-121008-IB10370023-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-PchCrossThrottling                    : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.TTLevels.PchCrossThrottling) );  
//[-end-121008-IB10370023-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DmiHaAWC       ( Address : 0x%x )\n", &PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-DmiTsawEn                             : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.DmiTsawEn) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-SuggestedSetting                      : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.SuggestedSetting) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Rsvdbits                              : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.Rsvdbits) );   
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-TS0TW                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS0TW) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-TS1TW                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS1TW) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-TS2TW                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS2TW) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-TS3TW                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.DmiHaAWC.TS3TW) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SataTT         ( Address : 0x%x )\n", &PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P0T1M                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T1M) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P0T2M                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T2M) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P0T3M                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0T3M) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P0TDisp                               : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0TDisp) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P1T1M                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T1M) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P1T2M                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T2M) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P1T3M                                 : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1T3M) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P1TDisp                               : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1TDisp) );   
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P0Tinact                              : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0Tinact) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P0TDispFinit                          : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P0TDispFinit) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P1Tinact                              : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1Tinact) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-P1TDispFinit                          : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.P1TDispFinit) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-SuggestedSetting                      : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.SuggestedSetting) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |  |-Rsvdbits                              : %x\n", PchPlatformPolicy->ThermalConfig->ThermalThrottling.SataTT.Rsvdbits) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchHotLevel                          : %x\n", PchPlatformPolicy->ThermalConfig->PchHotLevel) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-HpetConfig           ( Address : 0x%x )\n", PchPlatformPolicy->HpetConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-BdfValid                                    : %x\n", PchPlatformPolicy->HpetConfig->BdfValid) );
  for ( Count = 0; Count < PCH_HPET_BDF_MAX; ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Hpet[%2x]         ( Address : 0x%x )\n", Count, &PchPlatformPolicy->HpetConfig->Hpet[Count]) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-BusNumber                                 : %x\n", PchPlatformPolicy->HpetConfig->Hpet[Count].BusNumber) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-DeviceNumber                              : %x\n", PchPlatformPolicy->HpetConfig->Hpet[Count].DeviceNumber) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-FunctionNumber                            : %x\n", PchPlatformPolicy->HpetConfig->Hpet[Count].FunctionNumber) );  
  }
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-SerialIrqConfig      ( Address : 0x%x )\n", PchPlatformPolicy->SerialIrqConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SirqEnable                                  : %x\n", PchPlatformPolicy->SerialIrqConfig->SirqEnable) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SirqMode                                    : %x\n", PchPlatformPolicy->SerialIrqConfig->SirqMode) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-StartFramePulse                             : %x\n", PchPlatformPolicy->SerialIrqConfig->StartFramePulse) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-DmiConfig            ( Address : 0x%x )\n", PchPlatformPolicy->DmiConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DmiAspm                                     : %x\n", PchPlatformPolicy->DmiConfig->DmiAspm) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DmiExtSync                                  : %x\n", PchPlatformPolicy->DmiConfig->DmiExtSync) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DmiIot                                      : %x\n", PchPlatformPolicy->DmiConfig->DmiIot) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->DmiConfig->Rsvdbits) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-PwrOptConfig            ( Address : 0x%x )\n", PchPlatformPolicy->PwrOptConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchPwrOptDmi                                : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptDmi) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchPwrOptGbe                                : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptGbe) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchPwrOptXhci                               : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptXhci) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchPwrOptEhci                               : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptEhci) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchPwrOptSata                               : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptSata) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-MemCloseStateEn                             : %x\n", PchPlatformPolicy->PwrOptConfig->MemCloseStateEn) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-InternalObffEn                              : %x\n", PchPlatformPolicy->PwrOptConfig->InternalObffEn) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-ExternalObffEn                              : %x\n", PchPlatformPolicy->PwrOptConfig->ExternalObffEn) );   
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-NumOfDevLtrOverride                         : %x\n", PchPlatformPolicy->PwrOptConfig->NumOfDevLtrOverride) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DevLtrOverride                              : %x\n", PchPlatformPolicy->PwrOptConfig->DevLtrOverride) );   
  for ( Count = 0; Count < GetPchMaxPciePortNum (); ++Count ) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-PchPwrOptPcie[%2x] ( Address : 0x%x )\n", Count, &PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count]) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-LtrEnable                                : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].LtrEnable) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-ObffEnable                               : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].ObffEnable) );  
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-LtrConfigLock                            : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].LtrConfigLock) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-Rsvdbits                                 : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].Rsvdbits) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-LtrMaxSnoopLatency                       : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].LtrMaxSnoopLatency) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-LtrMaxNoSnoopLatency                     : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].LtrMaxNoSnoopLatency) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SnoopLatencyOverrideMode                 : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].SnoopLatencyOverrideMode) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SnoopLatencyOverrideMultiplier           : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].SnoopLatencyOverrideMultiplier) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-SnoopLatencyOverrideValue                : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].SnoopLatencyOverrideValue) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-NonSnoopLatencyOverrideMode              : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].NonSnoopLatencyOverrideMode) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-NonSnoopLatencyOverrideMultiplier        : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].NonSnoopLatencyOverrideMultiplier) );
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |  |-NonSnoopLatencyOverrideValue             : %x\n", PchPlatformPolicy->PwrOptConfig->PchPwrOptPcie[Count].NonSnoopLatencyOverrideValue) );
  }
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-LegacyDmaDisable                            : %x\n", PchPlatformPolicy->PwrOptConfig->LegacyDmaDisable) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->PwrOptConfig->Rsvdbits) );  
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-MiscConfig            ( Address : 0x%x )\n", PchPlatformPolicy->MiscConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-FviSmbiosType                               : %x\n", PchPlatformPolicy->MiscConfig->FviSmbiosType) );     
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DciEn                                       : %x\n", PchPlatformPolicy->MiscConfig->DciEn) );
//[-start-120807-IB04770241-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-AudioDspConfig        ( Address : 0x%x )\n", PchPlatformPolicy->AudioDspConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AudioDspD3PowerGating                       : %x\n", PchPlatformPolicy->AudioDspConfig->AudioDspD3PowerGating) );     
//[-end-120807-IB04770241-modify]//
//[-start-120917-IB03780460-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AudioDspBluetoothSupport                    : %x\n", PchPlatformPolicy->AudioDspConfig->AudioDspBluetoothSupport) );     
//[-end-120917-IB03780460-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AudioDspAcpiMode                            : %x\n", PchPlatformPolicy->AudioDspConfig->AudioDspAcpiMode) );     
//[-start-120807-IB04770241-add]//
//[-start-121008-IB10370023-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AudioDspAcpiInterruptMode                   : %x\n", PchPlatformPolicy->AudioDspConfig->AudioDspAcpiInterruptMode) );     
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->AudioDspConfig->Rsvdbits) );     
//[-end-121008-IB10370023-modify]//

//[-start-121008-IB10370023-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-AudioDspSafeMode                            : %x\n", PchPlatformPolicy->AudioDspConfig->AudioDspSafeMode) );     
//[-end-121008-IB10370023-remove]//
//[-end-120807-IB04770241-add]//
//[-start-121113-IB06150259-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-SerialIoConfig            ( Address : 0x%x )\n", PchPlatformPolicy->SerialIoConfig) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoMode                                : %x\n", PchPlatformPolicy->SerialIoConfig->SerialIoMode) );     
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-SerialIoInterruptMode                       : %x\n", PchPlatformPolicy->SerialIoConfig->SerialIoInterruptMode) ); 
//[-start-120807-IB04770241-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Ddr50Support                                : %x\n", PchPlatformPolicy->SerialIoConfig->Ddr50Support) ); 
//[-start-121008-IB10370023-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-I2c0VoltageSelect                           : %x\n", PchPlatformPolicy->SerialIoConfig->I2c0VoltageSelect) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-I2c1VoltageSelect                           : %x\n", PchPlatformPolicy->SerialIoConfig->I2c1VoltageSelect) ); 
//[-end-121008-IB10370023-modify]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-GpioIrqRoute                                : %x\n", PchPlatformPolicy->SerialIoConfig->GpioIrqRoute) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DriverModeTouchPanel                        : %x\n", PchPlatformPolicy->SerialIoConfig->DriverModeTouchPanel) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DriverModeTouchPad                          : %x\n", PchPlatformPolicy->SerialIoConfig->DriverModeTouchPad) ); 
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-DriverModeSensorHub                         : %x\n", PchPlatformPolicy->SerialIoConfig->DriverModeSensorHub) ); 
//[-start-130226-IB10130030-remove]//
//  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |  |-Rsvdbits                                    : %x\n", PchPlatformPolicy->SerialIoConfig->Rsvdbits) ); 
//[-end-130226-IB10130030-remove]//
//[-end-121113-IB06150259-modify]//
//[-end-120807-IB04770241-add]//
//[-start-120807-IB04770241-remove]//
// #ifdef PCH_A0PWRON_SAFEMODE
//   DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-PwrOnSafeModeBits                              : %x\n", PchPlatformPolicy->PwrOnSafeModeBits) );     
// #endif
//[-end-120807-IB04770241-remove]//
  return;
}
//[-end-120628-IB06460410-modify]//
