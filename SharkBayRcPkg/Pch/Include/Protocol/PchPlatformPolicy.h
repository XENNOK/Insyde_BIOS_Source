/** @file
  PCH policy protocol produced by a platform driver specifying various
  expected PCH settings. This protocol is consumed by the PCH drivers.

;******************************************************************************
;* Copyright (c) 1983-2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

**/
#ifndef _PCH_PLATFORM_POLICY_H_
#define _PCH_PLATFORM_POLICY_H_

//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#include "PchAccess.h"
#include "PchUsbConfig.h"
#endif
///
/// PCH policy provided by platform for DXE phase
///
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_GUID \
  { \
    0x9797aaf8, 0xe49b, 0x4f02, 0xa3, 0x68, 0xc8, 0x14, 0x8d, 0x2b, 0xc9, 0xe7 \
  }
#else
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_GUID \
  { \
    0x9797aaf8, 0xe49b, 0x4f02, \
    { \
      0xa3, 0x68, 0xc8, 0x14, 0x8d, 0x2b, 0xc9, 0xe7 \
    } \
  }
#endif

extern EFI_GUID                                   gDxePchPlatformPolicyProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _DXE_PCH_PLATFORM_POLICY_PROTOCOL  DXE_PCH_PLATFORM_POLICY_PROTOCOL;

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///

///
/// Revision 1: Original version
///
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_1 1

///
/// Revision 2: Add L1Substates in PCH_PCI_EXPRESS_ROOT_PORT_CONFIG
///             for BIOS Capability to Enable/Disable L1 Substates.
///             Add override configuration for L1L2 and L1 substates.
///
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_2 2

///
/// Revision 3: Add RootPortFunctionSwapping in PCH_PCI_EXPRESS_CONFIG
///             for switching the support of RootPortFunctionSwapping.
///
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_3 3

///
/// Revision 4: Add LegacyDmaDisable in PCH_PWR_OPT_CONFIG
///             Add PchPwrCycDur in PCH_MISC_PM_CONFIG
///
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_4 4

///
/// Revision 5: Add Port30Settings in PCH_USB_CONFIG
///             Add PcieWakeFromDeepSx in PCH_WAKE_CONFIG
///
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_5 5

///
/// Revision 6: Add DciEn in PCH_MISC_CONFIG
///             Add DDR50 support in PCH_SERIAL_IO_CONFIG
///
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_6 6

///
/// Revision 7: Add LTR related policies in PCH_PCIE_PWR_OPT
///               LtrMaxSnoopLatency
///               LtrMaxNoSnoopLatency
///               SnoopLatencyOverrideMode
///               SnoopLatencyOverrideMultiplier
///               SnoopLatencyOverrideValue
///               NonSnoopLatencyOverrideMode
///               NonSnoopLatencyOverrideMultiplier
///               NonSnoopLatencyOverrideValue
///             Update SlpLanLowDc usage.
///
#define DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_7 7

//
// Generic definitions for device enabling/disabling used by PCH code.
//
#define PCH_DEVICE_ENABLE   1
#define PCH_DEVICE_DISABLE  0

//
// ---------------------------- Device Enabling ------------------------------
//
///
/// PCH Device enablings
///
/// The PCH_DEVICE_ENABLING block allows platform modules to tell the PCH drivers
/// to enable/disable a set of PCH features.
/// Platform modules may need to refer Setup options, schematic, BIOS specification
/// to update these fields.
///
typedef struct {
  ///
  /// This member determines if enable or disable Intel Gigabit LAN device.
  /// It must be set to disable while the device is not used.
  ///
  UINT8 Lan               : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not Intel HD Audio (Azalia) should be enabled.
  /// If enabled and no codec exists the reference code will automatically disable
  /// the Azalia device.
  ///
  UINT8 Azalia            : 2;    ///< 0: Disable; 1: Enable; 2: Auto
  ///
  /// This member describes whether or not the SATA controllers should be enabled.
  ///
  UINT8 Sata              : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the SMBus controller of PCH should be enabled.
  ///
  UINT8 Smbus             : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the PCI ClockRun feature of PCH should
  /// be enabled.
  ///
  UINT8 PciClockRun       : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not PCH display logic should be enabled.
  /// PCH display logic properly decodes the Register Access that are used to communicate
  /// with the North Display in the IMC. This device should be enabled or disabled
  /// as defined in the BIOS specification.
  ///
  UINT8 Display           : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Compatibility Revision ID (CRID) feature
  /// of PCH should be enabled.
  ///
  UINT8 Crid              : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the DMA of Serial IO controllers should be enabled.
  ///
  UINT8 SerialIoDma       : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Serial IO I2c0 controller should be enabled.
  ///
  UINT8 SerialIoI2c0      : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Serial IO I2c1 controller should be enabled.
  ///
  UINT8 SerialIoI2c1      : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Serial IO Spi0 controller should be enabled.
  ///
  UINT8 SerialIoSpi0      : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Serial IO Spi1 controller should be enabled.
  ///
  UINT8 SerialIoSpi1      : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Serial IO Uart0 controller should be enabled.
  ///
  UINT8 SerialIoUart0     : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Serial IO Uart1 controller should be enabled.
  ///
  UINT8 SerialIoUart1     : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Serial IO Sdio controller should be enabled.
  ///
  UINT8 SerialIoSdio      : 1;    ///< 0: Disable; 1: Enable
  ///
  /// This member describes whether or not the Asdio Dsp controller should be enabled.
  ///
  UINT8 AudioDsp          : 1;    ///< 0: Disable; 1: Enable
  UINT8 Rsvdbits          : 7;    ///< Reserved fields for future expansion w/o protocol change
} PCH_DEVICE_ENABLING;

//
// ---------------------------- PCI Express Config ----------------------
//
///
/// The values before AutoConfig match the setting of PCI Express Base Specification 1.1, please be careful for adding new feature
///
typedef enum {
  PchPcieAspmDisabled,
  PchPcieAspmL0s,
  PchPcieAspmL1,
  PchPcieAspmL0sL1,
  PchPcieAspmAutoConfig,
  PchPcieAspmMax
} PCH_PCI_EXPRESS_ASPM_CONTROL;

///
/// Refer to PCH EDS for the PCH implementation values corresponding
/// to below PCI-E spec defined ranges
///
typedef enum {
  PchPcieL1SubstatesDisabled,
  PchPcieL1SubstatesL1_1,
  PchPcieL1SubstatesL1_2,
  PchPcieL1SubstatesL1_1_2,
  PchPcieL1SubstatesMax
} PCH_PCIE_EXPRESS_L1SUBSTATES_CONTROL;

typedef enum {
  PchPcieCompletionTO_Default,
  PchPcieCompletionTO_50_100us,
  PchPcieCompletionTO_1_10ms,
  PchPcieCompletionTO_16_55ms,
  PchPcieCompletionTO_65_210ms,
  PchPcieCompletionTO_260_900ms,
  PchPcieCompletionTO_1_3P5s,
  PchPcieCompletionTO_4_13s,
  PchPcieCompletionTO_17_64s,
  PchPcieCompletionTO_Disabled
} PCH_PCIE_COMPLETION_TIMEOUT;

typedef enum {
  PchPcieOverrideDisabled             = 0x00,
  PchPcieL1L2Override                 = 0x01,
  PchPcieL1SubstatesOverride          = 0x02,
  PchPcieL1L2AndL1SubstatesOverride   = 0x03
} PCH_PCI_EXPRESS_ASPM_OVERRIDE_CONFIG;

typedef struct {
  UINT8 Enable                          : 1;        ///< Root Port enabling, 0: Disable; 1: Enable.
  UINT8 Hide                            : 1;        ///< Whether or not to hide the configuration space of this port.
  UINT8 SlotImplemented                 : 1;        ///< Indicates whether the root port is connected to a slot.
  UINT8 HotPlug                         : 1;        ///< Indicate whether the root port is hot plug available.
  UINT8 PmSci                           : 1;        ///< Indicate whether the root port power manager SCI is enabled.
  UINT8 ExtSync                         : 1;        ///< Indicate whether the extended synch is enabled.
  UINT8 Rsvdbits                        : 2;
  //
  // Error handlings
  //
  UINT8 UnsupportedRequestReport        : 1;        ///< Indicate whether the Unsupported Request Report is enabled.
  UINT8 FatalErrorReport                : 1;        ///< Indicate whether the Fatal Error Report is enabled.
  UINT8 NoFatalErrorReport              : 1;        ///< Indicate whether the No Fatal Error Report is enabled.
  UINT8 CorrectableErrorReport          : 1;        ///< Indicate whether the Correctable Error Report is enabled.
  UINT8 PmeInterrupt                    : 1;        ///< Indicate whether the PME Interrupt is enabled.
  UINT8 SystemErrorOnFatalError         : 1;        ///< Indicate whether the System Error on Fatal Error is enabled.
  UINT8 SystemErrorOnNonFatalError      : 1;        ///< Indicate whether the System Error on Non Fatal Error is enabled.
  UINT8 SystemErrorOnCorrectableError   : 1;        ///< Indicate whether the System Error on Correctable Error is enabled.

  UINT8 AdvancedErrorReporting          : 1;        ///< Indicate whether the Advanced Error Reporting is enabled
  UINT8 TransmitterHalfSwing            : 1;        ///< Indicate whether the Transmitter Half Swing is enabled.
  UINT8 Reserved                        : 6;        ///< Reserved fields for future expansion w/o protocol change

  UINT8 FunctionNumber;                             ///< The function number this root port is mapped to
  UINT8 PhysicalSlotNumber;                         ///< Indicates the slot number for the root port.
  PCH_PCIE_COMPLETION_TIMEOUT   CompletionTimeout;  ///< The completion timeout configuration of the root port
  PCH_PCI_EXPRESS_ASPM_CONTROL  Aspm;               ///< The ASPM configuration of the root port
  PCH_PCIE_EXPRESS_L1SUBSTATES_CONTROL L1Substates; ///< The L1 Substates configuration of the root port
} PCH_PCI_EXPRESS_ROOT_PORT_CONFIG;

typedef struct {
  UINT16                                VendorId;             ///< The vendor Id of Pci Express card ASPM setting override, 0xFFFF means any Vendor ID
  UINT16                                DeviceId;             ///< The Device Id of Pci Express card ASPM setting override, 0xFFFF means any Device ID
  UINT8                                 RevId;                ///< The Rev Id of Pci Express card ASPM setting override, 0xFF means all steppings
  UINT8                                 BaseClassCode;        ///< The Base Class Code of Pci Express card ASPM setting override, 0xFF means all base class
  UINT8                                 SubClassCode;         ///< The Sub Class Code of Pci Express card ASPM setting override, 0xFF means all sub class
  PCH_PCI_EXPRESS_ASPM_CONTROL          EndPointAspm;         ///< The override ASPM setting from End point
  PCH_PCI_EXPRESS_ASPM_OVERRIDE_CONFIG  OverrideConfig;       ///< The override configuration. e.g. 0x0 means this subset is applicable to L1L2 override only.
  UINT16                                L1SubstatesCapOffset; ///< The L1Substates Capability Offset
  UINT32                                L1SubstatesCapMask;   ///< The L1Substates Capability Mask
} PCH_PCIE_DEVICE_ASPM_OVERRIDE;

typedef enum {
  PchPciePort1,
  PchPciePort2,
  PchPciePort3,
  PchPciePort4,
  PchPciePort5,
  PchPciePort6,
  PchPciePort7,
  PchPciePort8
} PCH_PCIE_SBDE_PORTS;

///
/// The PCH_PCI_EXPRESS_CONFIG block describes the expected configuration of the PCH PCI Express controllers
///
typedef struct {
  ///
  /// Temp Bus Number range available to be assigned to each root port and its downstream
  /// devices for initialization of these devices before PCI Bus enumeration.
  ///
  UINT8                             TempRootPortBusNumMin;
  ///
  /// Temp Bus Number range available to be assigned to each root port and its downstream
  /// devices for initialization of these devices before PCI Bus enumeration.
  ///
  UINT8                             TempRootPortBusNumMax;
  ///
  /// These members describe the configuration of each PCH PCIe root port.
  ///
  PCH_PCI_EXPRESS_ROOT_PORT_CONFIG  RootPort[LPTH_PCIE_MAX_ROOT_PORTS];
  UINT8                             NumOfDevAspmOverride;     ///< Number of Pci Express card Aspm setting override
  PCH_PCIE_DEVICE_ASPM_OVERRIDE     *DevAspmOverride;         ///< The Pointer which is point to Pci Express card Aspm setting override
  ///
  /// The PCIe Port that selected for enable Subtractive Decode.
  ///
  PCH_PCIE_SBDE_PORTS               PchPcieSbdePort;
  ///
  /// This member describes whether the PCI Express Clock Gating for each root port
  /// is enabled by platform modules.
  ///
  UINT8                             RootPortClockGating : 1;
  ///
  /// This member determines if enable subtractive decode over PCIe
  ///
  UINT8                             EnableSubDecode     : 1;  ///< Determines if enable subtractive decode over PCIe
  UINT8                             Rsvdbits            : 6;  ///< Reserved fields for future expansion w/o protocol change
  ///
  /// Support root port function number swapping if root port of function 0 is disabled.
  /// When RootPortFunctionSwapping enabled, the FunctionNumber for per root port will be ignored.
  /// It allows to automatically swap the function 0 to eanble root port by enabling this policy, or
  /// elaborates the FunctionNumber per root port and disable this policy.
  /// NOTE: When RootPortFunctionSwapping is disabled, Bios tries to force enable the root port of function 0.
  /// Be careful not assign the function 0 to the port connecting to Gbe/Nand or no lane available.
  ///
  UINT8                             RootPortFunctionSwapping;
} PCH_PCI_EXPRESS_CONFIG;

//
// ---------------------------- SATA Config -----------------------------
//
typedef enum {
  PchSataOromDelay2sec,
  PchSataOromDelay4sec,
  PchSataOromDelay6sec,
  PchSataOromDelay8sec
} PCH_SATA_OROM_DELAY;

typedef enum {
  PchSataSpeedSupportDefault,
  PchSataSpeedSupportGen1,
  PchSataSpeedSupportGen2,
  PchSataSpeedSupportGen3
} PCH_SATA_SPEED_SUPPORT;

typedef struct {
  UINT8 Enable          : 1;    ///< 0: Disable; 1: Enable
  UINT8 HotPlug         : 1;    ///< 0: Disable; 1: Enable
  UINT8 InterlockSw     : 1;    ///< 0: Disable; 1: Enable
  UINT8 External        : 1;    ///< 0: Disable; 1: Enable
  UINT8 SpinUp          : 1;    ///< 0: Disable; 1: Enable the COMRESET initialization Sequence to the device
  UINT8 SolidStateDrive : 1;    ///< 0: HDD; 1: SSD
  UINT8 DevSlp          : 1;    ///< 0: Disable; 1: Enable DevSlp on the port
  UINT8 EnableDitoConfig: 1;    ///< 0: Disable; 1: Enable
  UINT8 DmVal           : 4;    ///< DM value to be set
  UINT8 Rsvdbits        : 4;    ///< Reserved fields for future expansion w/o protocol change
  UINT16 DitoVal        : 10;   ///< Dito value to be set
  UINT16 Rsvdbits16     : 6;    ///< Reserved fields for future expansion w/o protocol change
} PCH_SATA_PORT_SETTINGS;

///
/// The PCH_SATA_CONFIG block describes the expected configuration of the SATA controllers.
///
typedef struct {
  PCH_SATA_PORT_SETTINGS  PortSettings[LPTH_AHCI_MAX_PORTS];
  UINT8 RaidAlternateId : 1;    ///< 0: Disable; 1: Enable
                                ///< Whether RAID Alternate ID is enabled. When disabled, the SATA controller D31:F2
                                ///< in RAID mode will report Device ID 282Xh; when enabled, the SATA controller
                                ///< D31:F2 in RAID mode will report Device ID 292Xh.
  UINT8 Raid0           : 1;    ///< 0: Disable; 1: Enable RAID0
  UINT8 Raid1           : 1;    ///< 0: Disable; 1: Enable RAID1
  UINT8 Raid10          : 1;    ///< 0: Disable; 1: Enable RAID10
  UINT8 Raid5           : 1;    ///< 0: Disable; 1: Enable RAID5
  UINT8 Irrt            : 1;    ///< 0: Disable; 1: Enable Intel Rapid Recovery Technology
  UINT8 OromUiBanner    : 1;    ///< 0: Disable; 1: Enable OROM UI and BANNER
  UINT8 HddUnlock       : 1;    ///< 0: Disable; 1: Indicates that the HDD password unlock in the OS is enabled

  UINT8 LedLocate       : 1;    ///< 0: Disable; 1: Indicates that the LED/SGPIO hardware is attached and ping to locate feature is enabled on the OS
  UINT8 IrrtOnly        : 1;    ///< 0: Disable; 1: Allow only IRRT drives to span internal and external ports
  UINT8 TestMode        : 1;    ///< 0: Disable; 1: Allow entrance to the PCH SATA test modes
  UINT8 SalpSupport     : 1;    ///< 0: Disable; 1: Enable Aggressive Link Power Management
  UINT8 LegacyMode      : 1;    ///< 0: Native PCI mode; 1: Legacy mode when controller 1 is operating in IDE mode
  UINT8 SmartStorage    : 1;    ///< 0: Disable; 1: Enable RST Smart Storage caching Bit
  UINT8 OromUiDelay     : 2;    ///< 00: 2 secs; 01: 4 secs; 10: 6 secs; 11: 8 secs

  UINT8 SpeedSupport    : 4;    ///< Indicates the maximum speed the SATA controller can support
                                ///< 1h: 1.5 Gb/s (Gen 1); 2h: 3 Gb/s(Gen 2); 3h: 6 Gb/s (Gen 1)
  UINT8 Rsvdbits        : 4;    ///< Reserved fields for future expansion w/o protocol change
//[-start-120604-IB03600487-add]//
  UINT16 Port0TimeOut      ;    // Port 0 time out limit for some specific HDD that need more init time
//[-end-120604-IB03600487-add]//
} PCH_SATA_CONFIG;

//
// --------------------------- AZALIA Config ------------------------------
//
typedef struct {
  UINT32  VendorDeviceId;       ///< This is the Vendor ID (byte 3 and byte 2) and Device ID (byte 1 and byte 0) of the Azalia codec.
  UINT16  SubSystemId;          ///< This is the sub system ID of this codec.
  UINT8   RevisionId;           ///< This is the revision ID of this codec. 0xFF applies to all steppings
  UINT8   FrontPanelSupport;    ///< Whether or not support front panel. 1: Yes, 0: No.
  UINT16  NumberOfRearJacks;    ///< Number of rear jacks.
  UINT16  NumberOfFrontJacks;   ///< Number of front jacks.
} PCH_AZALIA_VERB_TABLE_HEADER;

typedef struct {
  PCH_AZALIA_VERB_TABLE_HEADER  VerbTableHeader;  ///< The header information in Azalia verb table.
  UINT32                        *VerbTableData;   ///< Pointer to the buffer containing verb tables data provided by platform.
} PCH_AZALIA_VERB_TABLE;


///
/// The AZALIA_CONFIG block describes the expected configuration of the Intel HD Audio (Azalia) feature.
///
typedef struct {
  UINT8                 Pme       : 1;      ///< Azalia wake-on-ring, 0: Disable; 1: Enable
  UINT8                 DS        : 1;      ///< 0: Docking is not supported; 1:Docking is supported
  UINT8                 DA        : 1;      ///< 0: Docking is not attached; 1:Docking is attached
  UINT8                 Rsvdbits  : 5;
  UINT8                 AzaliaVerbTableNum; ///< Number of verb tables provided by platform
  PCH_AZALIA_VERB_TABLE *AzaliaVerbTable;   ///< Pointer to the actual verb table(s)
  UINT16                ResetWaitTimer;     ///< The delay timer after Azalia reset, the value is number of microseconds
} PCH_AZALIA_CONFIG;

//
// --------------------------- AUDIO DSP Config ------------------------------
//
///
/// The PCH_AUDIO_DSP_CONFIG block describes the misc power management configurations
/// of the Audio Dsp controller, and also the Acpi or Pci mode selection for the
/// Audio Dsp controller.
///
typedef struct {
  UINT8                 AudioDspD3PowerGating     : 1; ///< This flag enables/disables the Audio Dsp D3 power gating.
  UINT8                 AudioDspBluetoothSupport  : 1; ///< Audio Dsp Bluetooth support enabled.
  UINT8                 AudioDspAcpiMode          : 1; ///< If this is enabled, the Audio Dsp device is shown as ACPI device,
                                                       ///< and the PCI controller for the Audio Dsp will be hidden, and vice versa.
  UINT8                 AudioDspAcpiInterruptMode : 1; ///< If this is enabled, the Audio Dsp device uses ACPI interrupts
  UINT8                 Rsvdbits                  : 4;
} PCH_AUDIO_DSP_CONFIG;

//
// --------------------------- Serial IO Config ------------------------------
//
///
/// The PCH_LP_SERIAL_CONFIG block provides the configurations to set the Serial IO controllers
/// to Acpi devices or Pci controllers, and also set the interrupt type to Acpi or Pci
/// through platform policy. It also provides to configure the I2c0 and I2c1 voltage
/// to 1.8v or 3.3v by platform setting.
///
typedef struct {
  UINT8                 SerialIoMode            : 1;  ///< Set to 0 will create Acpi devices for Serial IO Controllers. Set to 1 will show the Pci devices.
  UINT8                 SerialIoInterruptMode   : 1;  ///< Configures all Serial IO Controllers in PCI or ACPI Interrupt Mode.
  UINT8                 I2c0VoltageSelect       : 1;  ///< Selects the IO voltage for I2c0 controller. It can be 1.8v or 3.3v
  UINT8                 I2c1VoltageSelect       : 1;  ///< selects the IO voltage for I2c1 controller. It can be 1.8v or 3.3v
  ///
  /// GpioInt Configuration
  ///
  UINT8                 GpioIrqRoute            : 1;  ///< 0: IRQ14; 1: IRQ15
  UINT8                 DriverModeTouchPanel    : 1;  ///< Driver Mode Touch Panel  (ACPI=0:GPIO=1)
  UINT8                 DriverModeTouchPad      : 1;  ///< Driver Mode Touch Pad    (ACPI=0:GPIO=1)
  UINT8                 DriverModeSensorHub     : 1;  ///< Driver Mode Sensor Hub   (ACPI=0;GPIO=1)

  UINT8                 Ddr50Support            : 1;  ///< enables DDR50 support in SDIO controller
  UINT8                 Reserved                : 7;  // padding
} PCH_SERIAL_IO_CONFIG;

typedef enum {
  PchSerialIoIsAcpi = 0,
  PchSerialIoIsPci
} PCH_SERIAL_IO_MODE_CONFIG;

typedef enum {
  PchSerialIoIs33V = 0,
  PchSerialIoIs18V
} PCH_LP_SERIAL_IO_VOLTAGE_SEL;

//
// --------------------------- Smbus Config ------------------------------
//

///
/// The SMBUS_CONFIG block lists the reserved addresses for non-ARP capable devices in the platform.
///
typedef struct {
  UINT8 NumRsvdSmbusAddresses;    ///< The number of elements in the SmbusAddressTable.
  UINT8 *RsvdSmbusAddressTable;   ///< The pointer to an array of addresses reserved for non-ARP-capable SMBus devices.
} PCH_SMBUS_CONFIG;

//
// --------------------------- Miscellaneous PM Config ------------------------------
//
typedef struct {
  UINT8 MeWakeSts           : 1;  ///< Clear the ME_WAKE_STS bit in the Power and Reset Status (PRSTS) register.
  UINT8 MeHrstColdSts       : 1;  ///< Clear the ME_HRST_COLD_STS bit in the Power and Reset Status (PRSTS) register.
  UINT8 MeHrstWarmSts       : 1;  ///< Clear the ME_HRST_WARM_STS bit in the Power and Reset Status (PRSTS) register.
  UINT8 MeHostPowerDn       : 1;  ///< Clear the ME_HOST_PWRDN bit in the Power and Reset Status (PRSTS) register.
  UINT8 WolOvrWkSts         : 1;  ///< Clear the WOL_OVR_WK_STS bit in the Power and Reset Status (PRSTS) register.
  UINT8 Rsvdbits            : 3;
} PCH_POWER_RESET_STATUS;

typedef struct {
  UINT8  PmeB0S5Dis         : 1;  ///< Corresponds to the PME_B0_S5_DIS bit in the General PM Configuration 3 (GEN_PM_CON_3) register.
  UINT8  WolEnableOverride  : 1;  ///< Corresponds to the "MOL Enable Override" bit in the General PM Configuration 3 (GEN_PM_CON_3) register.
  UINT8  Gp27WakeFromDeepSx : 1;  ///< Determine if enable GP27 to wake from deep Sx.
  UINT8  PcieWakeFromDeepSx : 1;  ///< Determine if enable PCIe to wake from deep Sx.
  UINT8  Rsvdbits           : 4;
} PCH_WAKE_CONFIG;

typedef enum {
  PchDeepSxPolDisable = 0,
  PchMobileDpS5En,
  PchDesktopDpS5En,
  PchMobileDpS4S5En,
  PchDesktopDpS4S5En,
  PchMobileDpS3S4S5En,
  PchDesktopDpS3S4S5En
} PCH_DEEP_SX_CONFIG;

typedef enum {
  PchSlpS360us,
  PchSlpS31ms,
  PchSlpS350ms,
  PchSlpS32s
} PCH_SLP_S3_MIN_ASSERT;

typedef enum {
  PchSlpS4PchTime,   ///< The time defined in PCH EDS Power Sequencing and Reset Signal Timings table
  PchSlpS41s,
  PchSlpS42s,
  PchSlpS43s,
  PchSlpS44s
} PCH_SLP_S4_MIN_ASSERT;

typedef enum {
  PchSlpSus0ms,
  PchSlpSus500ms,
  PchSlpSus1s,
  PchSlpSus4s
} PCH_SLP_SUS_MIN_ASSERT;

typedef enum {
  PchSlpA0ms,
  PchSlpA4s,
  PchSlpA98ms,
  PchSlpA2s
} PCH_SLP_A_MIN_ASSERT;

///
/// The PCH_MISC_PM_CONFIG block describes expected miscellaneous power management settings.
/// The PowerResetStatusClear field would clear the Power/Reset status bits, please
/// set the bits if you want PCH Init driver to clear it, if you want to check the
/// status later then clear the bits.
///
typedef struct {
  ///
  /// Specify which Power/Reset bits need to be cleared by
  /// the PCH Init Driver.
  /// Usually platform drivers take care of these bits, but if
  /// not, let PCH Init driver clear the bits.
  ///
  PCH_POWER_RESET_STATUS  PowerResetStatusClear;
  PCH_WAKE_CONFIG         WakeConfig;         ///< Specify Wake Policy
  PCH_DEEP_SX_CONFIG      PchDeepSxPol;       ///< Deep Sx Policy
  PCH_SLP_S3_MIN_ASSERT   PchSlpS3MinAssert;  ///< SLP_S3 Minimum Assertion Width Policy
  PCH_SLP_S4_MIN_ASSERT   PchSlpS4MinAssert;  ///< SLP_S4 Minimum Assertion Width Policy
  PCH_SLP_SUS_MIN_ASSERT  PchSlpSusMinAssert; ///< SLP_SUS Minimum Assertion Width Policy
  PCH_SLP_A_MIN_ASSERT    PchSlpAMinAssert;   ///< SLP_A Minimum Assertion Width Policy
  UINT8                   SlpStrchSusUp : 1;  ///< Enable/Disable SLP_X Stretching After SUS Well Power Up
  ///
  /// Enable/Disable SLP_LAN# Low on DC Power.
  /// Configure On DC PHY Power Diable according to policy SlpLanLowDc.
  /// When this is enabled, SLP_LAN# will be driven low when ACPRESENT is low.
  /// This indicates that LAN PHY should be powered off on battery mode.
  /// This will override the DC_PP_DIS setting by WolEnableOverride.
  ///
  UINT8                   SlpLanLowDc   : 1;
  UINT8                   Rsvdbits      : 6;
  ///
  /// Reset Power Cycle Duration could be customized in the unit of second.Please refer to EDS
  /// for all support settings. So far PCH supports 1~4 seconds, and PCH default is 4 seconds.
  /// And make sure the setting correct, which never less than the following register.
  /// - GEN_PMCON_3.SLP_S3_MIN_ASST_WDTH
  /// - GEN_PMCON_3.SLP_S4_MIN_ASST_WDTH
  /// - PM_CFG.SLP_A_MIN_ASST_WDTH
  /// - PM_CFG.SLP_LAN_MIN_ASST_WDTH
  ///
  UINT8                   PchPwrCycDur;       ///< Reset Power Cycle Duration
} PCH_MISC_PM_CONFIG;

//
// --------------------------- IO APIC Config ------------------------------
//
///
/// The PCH_IO_APIC_CONFIG block describes the expected configuration of the PCH
/// IO APIC, it's optional and PCH code would ignore it if the BdfValid bit is
/// not TRUE. Bus:device:function fields will be programmed to the register
/// LPC_IBDF(D31:F0:R6Ch-6Dh), it's using for the following purpose:
/// As the Requester ID when initiating Interrupt Messages to the processor.
/// As the Completer ID when responding to the reads targeting the IOxAPI's
/// Memory-Mapped I/O registers.
/// This field defaults to Bus 0: Device 31: Function 0 after reset. BIOS can
/// program this field to provide a unique Bus:Device:Function number for the
/// internal IOxAPIC.
///
typedef struct {
  BOOLEAN BdfValid;               ///< Whether the BDF value is valid, PCH code will not program these fields if this bit is not TRUE.
  UINT8   BusNumber;              ///< Bus/Device/Function used as Requestor / Completer ID.
  UINT8   DeviceNumber;           ///< Bus/Device/Function used as Requestor / Completer ID.
  UINT8   FunctionNumber;         ///< Bus/Device/Function used as Requestor / Completer ID.
  UINT8   IoApicEntry24_39 :1;    ///< 0: Disable; 1: Enable IOAPIC Entry 24-39.
  UINT8   RsvdBits         :7;
} PCH_IO_APIC_CONFIG;

//
// --------------------------- Subsystem Vendor ID / Subsystem ID Config -----
//
///
/// The PCH_DEFAULT_SVID_SID block describes the default Subsystem Vendor ID and
/// Subsystem ID of the PCH devices.
/// This field will be ignored if the value of SubSystemVendorId and SubSystemId
/// are both 0.
/// If the SVID and SID registers of some PCH devices are filled before PchInit
/// driver execute then this field will be ignored because SVID and SID registers
/// are write once.
///
typedef struct {
  UINT16  SubSystemVendorId;      ///< Default Subsystem Vendor ID of the PCH devices.
  UINT16  SubSystemId;            ///< Default Subsystem ID of the PCH devices.
} PCH_DEFAULT_SVID_SID;

//
// --------------------------- Lock Down Config ------------------------------
//
//[-start-120726-IB10820088-modify]//
#define PCH_BWP_SIGNATURE SIGNATURE_32 ('P', 'B', 'W', 'P')
//[-end-120726-IB10820088-modify]//
///
/// The PCH_LOCK_DOWN_CONFIG block describes the expected configuration of the PCH
/// for security requirement.
///
typedef struct {
  ///
  /// Enable SMI_LOCK bit to prevent writes to the Global SMI Enable bit.
  ///
  UINT8  GlobalSmi      : 1;
  ///
  /// Enable BIOS Interface Lock Down bit to prevent writes to the Backup Control Register
  /// Top Swap bit and the General Control and Status Registers Boot BIOS Straps.
  ///
  UINT8  BiosInterface  : 1;
  ///
  /// Enable GPIO Lockdown Enable bit to enables lockdown of the GPIO registers:
  /// GPIO_USE_SEL, GP_IO_SEL, GP_LVL, GPIO_USE_SEL2, GP_IO_SEL2, GP_LVL2,
  /// GPIO_USE_SEL3, GP_IO_SEL3, GP_LVL3, GP_RST_SEL.
  ///
  UINT8  GpioLockDown   : 1;
  ///
  /// Enable RTC lower and upper 128 byte Lock bits to lock Bytes 38h-3Fh in the upper
  /// and lower 128-byte bank of RTC RAM.
  ///
  UINT8  RtcLock        : 1;
  ///
  /// Enable the BIOS Lock Enable (BLE) feature and set SMM_BWP bit (D31:F0:RegDCh[5])
  /// for the BIOS region protection. When it is enabled, the BIOS Region can only be
  /// modified from SMM after ExitPmAuth protocol is installed.
  /// Note: When BiosLock is enabled, platform code also needs to update to take care
  /// of BIOS modification (including SetVariable) in DXE or runtime phase after
  /// ExitPmAuth protocol is installed.
  ///
  UINT8  BiosLock       : 1;
  UINT8  Rsvdbits       : 3;
  ///
  /// Indicates the IO Address which is used to generate IO Trap SMI to register
  /// IchnBiosWp callback function in PchBiosLockIoTrapCallback () to handle TCO
  /// BIOSWR SMI. Please refer to the sample code of PchBiosWriteProtect driver for
  /// more details about PchBiosLockIoTrapCallback().If PchBiosLockIoTrapAddress is 0,
  /// BIOS will allocate available IO address with 256 byte range from GCD and pass it
  /// to PchBiosLockIoTrapAddress. PCH Reference code replaces the SW SMI generated
  /// by PchBiosLockSwSmiNumber with the IO Trap SMI generated by PchBiosLockIoTrapAddress.
  ///
  UINT16 PchBiosLockIoTrapAddress;      ///< IO Trap range base address for Bios Lock
//[-start-130620-IB05160461-modify]//
  /// TCO_Lock : When set to 1, this bit prevents writes from changing the 
  /// TCO_EN bit (in offset 30h of Power Management I/O space). Once this bit is set to 1, it 
  /// can not be cleared by software writing a 0 to this bit location. A core-well reset is 
  /// required to change this bit from 1 to 0. This bit defaults to 0.
  UINT8  TcoLock        : 1;
  UINT8  Rsvdbits0      : 7;
//[-end-130620-IB05160461-modify]//
} PCH_LOCK_DOWN_CONFIG;

//
// --------------------------- Thermal Config ------------------------------------
//
typedef struct {
  UINT8 TselLock                : 1;
  UINT8 TscLock                 : 1;
  UINT8 TsmicLock               : 1;
  UINT8 PhlcLock                : 1;
  UINT8 Rsvdbits                : 4;
} PCH_THERMAL_ALERT_ENABLE;

typedef struct {
  UINT32 T0Level                : 9;
  UINT32 T1Level                : 9;
  UINT32 T2Level                : 9;
  UINT32 TTEnable               : 1;
  UINT32 TTState13Enable        : 1;
  UINT32 TTLock                 : 1;
  UINT32 SuggestedSetting       : 1;    ///< Enable/Disable suggested representative values
  ///
  /// ULT processors support thermal management and cross thermal throttling between the processor package
  /// and LP PCH. The PMSYNC message from PCH to CPU includes specific bit fields to update the PCH
  /// thermal status to the processor which is factored into the processor throttling.
  /// Enable/Disable PCH Cross Throttling; 0: Disabled, 1: Enabled
  ///
  UINT32 PchCrossThrottling     : 1;
} THERMAL_THROTTLE_LEVELS;

typedef struct {
  UINT8 DmiTsawEn               : 1;    ///< DMI Thermal Sensor Autonomous Width Enable
  UINT8 SuggestedSetting        : 1;    ///< Enable/Disable suggested representative values
  UINT8 Rsvdbits                : 6;

  UINT8 TS0TW                   : 2;    ///< Thermal Sensor 0 Target Width
  UINT8 TS1TW                   : 2;    ///< Thermal Sensor 1 Target Width
  UINT8 TS2TW                   : 2;    ///< Thermal Sensor 2 Target Width
  UINT8 TS3TW                   : 2;    ///< Thermal Sensor 3 Target Width
} DMI_HW_WIDTH_CONTROL;

typedef struct {
  UINT8 P0T1M                   : 2;    ///< Port 0 T1 Multipler
  UINT8 P0T2M                   : 2;    ///< Port 0 T2 Multipler
  UINT8 P0T3M                   : 2;    ///< Port 0 T3 Multipler
  UINT8 P0TDisp                 : 2;    ///< Port 0 Tdispatch

  UINT8 P1T1M                   : 2;    ///< Port 1 T1 Multipler
  UINT8 P1T2M                   : 2;    ///< Port 1 T2 Multipler
  UINT8 P1T3M                   : 2;    ///< Port 1 T3 Multipler
  UINT8 P1TDisp                 : 2;    ///< Port 1 Tdispatch

  UINT8 P0Tinact                : 2;    ///< Port 0 Tinactive
  UINT8 P0TDispFinit            : 1;    ///< Port 0 Alternate Fast Init Tdispatch
  UINT8 P1Tinact                : 2;    ///< Port 1 Tinactive
  UINT8 P1TDispFinit            : 1;    ///< Port 1 Alternate Fast Init Tdispatch
  UINT8 SuggestedSetting        : 1;    ///< Enable/Disable suggested representative values
  UINT8 Rsvdbits                : 1;
} SATA_THERMAL_THROTTLE;

///
/// If DmiIot of DmiConfig is enabled, the TTLevels and DmiHaAWC value will not be used.
///
typedef struct {
  THERMAL_THROTTLE_LEVELS   TTLevels;
  DMI_HW_WIDTH_CONTROL      DmiHaAWC;
  SATA_THERMAL_THROTTLE     SataTT;
} PCH_THERMAL_THROTTLING;

///
/// The PCH_THERMAL_CONFIG block describes the expected configuration of the PCH for Thermal.
///
typedef struct {
  ///
  /// This field determines if the PCH's TEMP_ALERT# pin will be asserted while the
  /// temperature of processor core, PCH, Memory Controller/Graphics and DIMM is
  /// outside the temperature limits.
  ///
  PCH_THERMAL_ALERT_ENABLE  ThermalAlertEnable;
  ///
  /// This field reports the status of Thermal Device. When it reports ThermalDevice
  /// is disabled, the PCI configuration space of thermal device will be hidden by
  /// setting FD.TTD prior to end of POST.
  ///
  BOOLEAN                   ThermalDeviceEnable;
  ///
  /// This field decides the settings of Thermal throttling. When the Suggested Setting
  /// is enabled, PCH RC will use the suggested representative values.
  ///
  PCH_THERMAL_THROTTLING    ThermalThrottling;
  ///
  /// This field decides the temperature, 0x00 is the hottest temperature and 0x1FF
  /// is the lowest temperature
  ///
  UINT16                    PchHotLevel;
} PCH_THERMAL_CONFIG;

//
// --------------------------- HPET Config ------------------------------
//
typedef struct {
  UINT8 BusNumber;                            ///< Bus Number HPETn used as Requestor / Completer ID
  UINT8 DeviceNumber;                         ///< Device Number HPETn used as Requestor / Completer ID
  UINT8 FunctionNumber;                       ///< Function Number HPETn used as Requestor / Completer ID
} PCH_HPET_BDF_CONFIG;

///
/// The PCH_LPC_HPET_CONFIG block describes the expected configuration of the PCH for HPET.
///
typedef struct {
  BOOLEAN             BdfValid;               ///< Whether the BDF value is valid
  PCH_HPET_BDF_CONFIG Hpet[PCH_HPET_BDF_MAX]; ///< Lpc HPET n Bus:Device:Function Configuration
} PCH_LPC_HPET_CONFIG;

//
// --------------------------- Serial IRQ Config ------------------------------
//
typedef enum {
  PchQuietMode,
  PchContinuousMode
} PCH_SIRQ_MODE;
///
/// Refer to PCH EDS for the details of Start Frame Pulse Width in Continuous and Quiet mode
///
typedef enum {
  PchSfpw4Clk,
  PchSfpw6Clk,
  PchSfpw8Clk
} PCH_START_FRAME_PULSE;

///
/// The PCH_LPC_SIRQ_CONFIG block describes the expected configuration of the PCH for Serial IRQ.
///
typedef struct {
  BOOLEAN               SirqEnable;       ///< Determines if enable Serial IRQ
  PCH_SIRQ_MODE         SirqMode;         ///< Serial IRQ Mode Select. 0: quiet mode 1: continuous mode.
  PCH_START_FRAME_PULSE StartFramePulse;  ///< Start Frame Pulse Width
} PCH_LPC_SIRQ_CONFIG;

//
// ---------------------------- DMI Config -----------------------------
//
///
/// The PCH_DMI_CONFIG block describes the expected configuration of the PCH for DMI.
///
typedef struct {
  UINT8     DmiAspm           : 1;        ///< Enable/Disable ASPM on PCH side of the DMI Link.
                                          ///< While DmiAspm is enabled, DMI ASPM will be set to Intel recommended value.
  UINT8     DmiExtSync        : 1;        ///< Determines if force extended transmission of FTS ordered sets when
                                          ///< exiting L0s prior to entering L0.
  UINT8     DmiIot            : 1;        ///< Enable/Disable DMI IOT
  UINT8     Rsvdbits          : 5;
} PCH_DMI_CONFIG;

//
// --------------------------- Power Optimizer Config ------------------------------
//
typedef struct {
  UINT8         LtrEnable                     :1;   ///< Latency Tolerance Reporting Mechanism.
  UINT8         ObffEnable                    :1;   ///< Pcie end point Optimized Buffer Flush/Fill (OBFF) capability for the root port.
  UINT8         LtrConfigLock                 :1;
  UINT8         Rsvdbits                      :5;
  UINT16        LtrMaxSnoopLatency;                 ///< Latency Tolerance Reporting, Max Snoop Latency.              
  UINT16        LtrMaxNoSnoopLatency;               ///< Latency Tolerance Reporting, Max Non-Snoop Latency.            
  UINT8         SnoopLatencyOverrideMode;           ///< Latency Tolerance Reporting, Snoop Latency Override Mode.        
  UINT8         SnoopLatencyOverrideMultiplier;     ///< Latency Tolerance Reporting, Snoop Latency Override Multiplier.  
  UINT16        SnoopLatencyOverrideValue;          ///< Latency Tolerance Reporting, Snoop Latency Override Value.       
  UINT8         NonSnoopLatencyOverrideMode;        ///< Latency Tolerance Reporting, Non-Snoop Latency Override Mode.     
  UINT8         NonSnoopLatencyOverrideMultiplier;  ///< Latency Tolerance Reporting, Non-Snoop Latency Override Multiplier.
  UINT16        NonSnoopLatencyOverrideValue;       ///< Latency Tolerance Reporting, Non-Snoop Latency Override Value.    
} PCH_PCIE_PWR_OPT;

typedef struct {
  UINT16  VendorId; ///< PCI configuration space offset 0
  UINT16  DeviceId; ///< PCI configuration space offset 2
  UINT8   RevId;    ///< PCI configuration space offset 8; 0xFF means all steppings
/**
  SnoopLatency bit definition
  Note: All Reserved bits must be set to 0

  BIT[15]     - When set to 1b, indicates that the values in bits 9:0 are valid
                When clear values in bits 9:0 will be ignored
  BITS[14:13] - Reserved
  BITS[12:10] - Value in bits 9:0 will be multiplied with the scale in these bits
                000b - 1 ns
                001b - 32 ns
                010b - 1024 ns
                011b - 32,768 ns
                100b - 1,048,576 ns
                101b - 33,554,432 ns
                110b - Reserved
                111b - Reserved
  BITS[9:0]   - Snoop Latency Value. The value in these bits will be multiplied with
                the scale in bits 12:10
**/
  UINT16  SnoopLatency;
/**
  NonSnoopLatency bit definition
  Note: All Reserved bits must be set to 0

  BIT[15]     - When set to 1b, indicates that the values in bits 9:0 are valid
                When clear values in bits 9:0 will be ignored
  BITS[14:13] - Reserved
  BITS[12:10] - Value in bits 9:0 will be multiplied with the scale in these bits
                000b - 1 ns
                001b - 32 ns
                010b - 1024 ns
                011b - 32,768 ns
                100b - 1,048,576 ns
                101b - 33,554,432 ns
                110b - Reserved
                111b - Reserved
  BITS[9:0]   - Non Snoop Latency Value. The value in these bits will be multiplied with
                the scale in bits 12:10
**/
  UINT16  NonSnoopLatency;
} PCH_PCIE_DEVICE_LTR_OVERRIDE;

///
/// The PCH_PWR_OPT_CONFIG block describes the expected configuration of the PCH for Power Optimizer.
///
typedef struct {
  UINT8  PchPwrOptDmi         :1;   ///< enable/disable DMI Power Optimizer on PCH side.
  UINT8  PchPwrOptGbe         :1;   ///< enable/disable Gbe Power Optimizer on PCH side.
  UINT8  PchPwrOptXhci        :1;   ///< enable/disable XHCI Power Optimizer on PCH side.
  UINT8  PchPwrOptEhci        :1;   ///< enable/disable EHCI Power Optimizer on PCH side.
  UINT8  PchPwrOptSata        :1;   ///< enable/disable SATA Power Optimizer on PCH side.
  UINT8  MemCloseStateEn      :1;   ///< enable/disable MEM CLOSED State on PCH side.
  UINT8  InternalObffEn       :1;   ///< enable/disable Optimized Buffer Flush/Fill (OBFF) protocol for internal on PCH side.
  UINT8  ExternalObffEn       :1;   ///< enable/disable Optimized Buffer Flush/Fill (OBFF) protocol for external on PCH side.
  UINT8  NumOfDevLtrOverride;                            ///< Number of Pci Express card listed in LTR override table
  PCH_PCIE_DEVICE_LTR_OVERRIDE *DevLtrOverride;          ///< Pointer to Pci Express devices LTR override table
  PCH_PCIE_PWR_OPT             PchPwrOptPcie[LPTH_PCIE_MAX_ROOT_PORTS]; ///< related configuration for PCIE ports power optimization.
  UINT8  LegacyDmaDisable     :1;   ///< disable/enable legacy DMA controller (8254) and port 0x61 timer
  UINT8  Rsvdbits             :7;  
} PCH_PWR_OPT_CONFIG;

//
// --------------------- Miscellaneous Config ------------------------------
//
///
/// The PCH_MISC_CONFIG block describes the expected configuration of the PCH for Miscellaneous Configuration.
///
typedef struct {
  ///
  /// This member determines the SMBIOS OEM type (0x80 to 0xFF) defined in SMBIOS
  /// Type 14 - Group Associations structure - item type.
  /// FVI structure uses it as SMBIOS OEM type to provide version information.
  ///
  UINT8  FviSmbiosType;
  ///
  /// This member enables/disables Direct Connect Interface (DCI) for LPT-LP.
  /// When enabling DCI (through the enable bit), it's able to access JTAG and Run Control features 
  /// in a closed chassis situation, by using the USB3 port on a Shark Bay ULT platform.
  ///
  UINT8  DciEn       :1;
  UINT8  Rsvdbits    :7;
} PCH_MISC_CONFIG;

//
// ------------ General PCH Platform Policiy protocol definition ------------
//
///
/// The PCH platform policy protocol allows the platform code to publish a set of
/// configuration information that the PCH drivers will use to configure the PCH hardware.
/// The Revision field is used to accommodate backward compatible changes to the protocol.
/// The Revision should be initialized to DXE_PLATFORM_PCH_POLICY_PROTOCOL_REVISION_X
/// by the protocol producer.
/// The BusNumber field is used for platform to assign Bus number with multiple instances.
///
struct _DXE_PCH_PLATFORM_POLICY_PROTOCOL {
  ///
  /// This member specifies the revision of the PCH Policy protocol.
  /// This field is used to indicate backwards compatible changes to the protocol.
  /// Platform code that produces this protocol must fill with the correct revision
  /// value for the PCH reference code to correctly interpret the content of the
  /// protocol fields.
  ///
  UINT8                   Revision;
  ///
  /// This member describes the desired bus number of the PCH controller.
  ///
  UINT8                   BusNumber;
  ///
  /// This member describes which PCH devices should be enabled or disabled.
  ///
  PCH_DEVICE_ENABLING     *DeviceEnabling;
  ///
  /// This member describes USB controller's related configuration.
  ///
  PCH_USB_CONFIG          *UsbConfig;
  ///
  /// This member describes PCI Express controller's related configuration.
  ///
  PCH_PCI_EXPRESS_CONFIG  *PciExpressConfig;
  ///
  /// This member describes SATA controller's related configuration.
  ///
  PCH_SATA_CONFIG         *SataConfig;
  ///
  /// This member describes the Intel HD Audio (Azalia) related configuration.
  ///
  PCH_AZALIA_CONFIG       *AzaliaConfig;
  ///
  /// This member describes SMBus related configuration.
  ///
  PCH_SMBUS_CONFIG        *SmbusConfig;
  ///
  /// This member describes miscellaneous platform power management configurations.
  ///
  PCH_MISC_PM_CONFIG      *MiscPmConfig;
  ///
  /// This member describes IOAPIC related configuration.
  ///
  PCH_IO_APIC_CONFIG      *IoApicConfig;
  ///
  /// This member describes default SVID and Sid for PCH devices.
  ///
  PCH_DEFAULT_SVID_SID    *DefaultSvidSid;
  ///
  /// This member describes LockDown related configuration.
  ///
  PCH_LOCK_DOWN_CONFIG    *LockDownConfig;
  ///
  /// This member describes Thermal related configuration.
  ///
  PCH_THERMAL_CONFIG      *ThermalConfig;
  ///
  /// This member describes HPET related configuration.
  ///
  PCH_LPC_HPET_CONFIG     *HpetConfig;
  ///
  /// This member describes the expected configuration of the PCH for Serial IRQ.
  ///
  PCH_LPC_SIRQ_CONFIG     *SerialIrqConfig;
  ///
  /// This member describes DMI related configuration.
  ///
  PCH_DMI_CONFIG          *DmiConfig;
  ///
  /// This member describes the Power Optimizer configuration.
  ///
  PCH_PWR_OPT_CONFIG      *PwrOptConfig;
  ///
  /// This member describes the Miscellaneous configuration.
  ///
  PCH_MISC_CONFIG         *MiscConfig;
  ///
  /// This member describes the Audio Dsp related configuration
  ///
  PCH_AUDIO_DSP_CONFIG    *AudioDspConfig;
  ///
  /// This member describes the Serial IO related configuration
  ///
  PCH_SERIAL_IO_CONFIG    *SerialIoConfig;
};

#endif
