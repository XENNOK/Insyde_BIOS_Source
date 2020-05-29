/** @file
  PCH policy PPI produced by a platform driver specifying various
  expected PCH settings. This PPI is consumed by the PCH PEI modules.

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
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
#ifndef PCH_PLATFORM_POLICY_H_
#define PCH_PLATFORM_POLICY_H_
//
// External include files do NOT need to be explicitly specified in real EDKII
// environment
//
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#include "PchAccess.h"
#include "PchUsbConfig.h"
#endif
///
/// PCH policy provided by platform for PEI phase
///
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define PCH_PLATFORM_POLICY_PPI_GUID \
  { \
    0x36f6ce3d, 0xb76e, 0x42c2, 0x9f, 0x96, 0x3e, 0x41, 0x84, 0xa3, 0x50, 0x66 \
  }
#else
#define PCH_PLATFORM_POLICY_PPI_GUID \
  { \
    0x36f6ce3d, 0xb76e, 0x42c2, \
    { \
      0x9f, 0x96, 0x3e, 0x41, 0x84, 0xa3, 0x50, 0x66 \
    } \
  }
#endif

extern EFI_GUID                         gPchPlatformPolicyPpiGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_PLATFORM_POLICY_PPI PCH_PLATFORM_POLICY_PPI;

///
/// PPI revision number
/// Any backwards compatible changes to this PPI will result in an update in the revision number
/// Major changes will require publication of a new PPI
///
/// Revision 1:   Original version
///
#define PCH_PLATFORM_POLICY_PPI_REVISION_1  1
///
/// Revision 2:   Add UsbConfig
///
#define PCH_PLATFORM_POLICY_PPI_REVISION_2  2
///
/// Revision 3:   Add Port30Settings in PCH_USB_CONFIG
///
#define PCH_PLATFORM_POLICY_PPI_REVISION_3  3
///
/// Revision 4:   Add Sata RxEq Settings in PCH_SATA_TRACE_CONFIG
///               Deprecate PortLength and PortTopology in PCH_SATA_TRACE_CONFIG
///
#define PCH_PLATFORM_POLICY_PPI_REVISION_4  4
//
// Generic definitions for device enabling/disabling used by PCH code.
//
#define PCH_DEVICE_ENABLE   1
#define PCH_DEVICE_DISABLE  0

//
// ---------------------------- Gbe Config -----------------------------
//
typedef struct {
  ///
  /// Determines if enable GBE
  /// When EnableGbe is changed (from disabled to enabled or from enabled to disabled),
  /// it needs to set LAN Disable regsiter, which might be locked by FDSWL register.
  /// So it's recommendated to issue a global reset when changing the status for PCH Internal LAN.
  ///
  UINT8  EnableGbe                : 1;
  UINT8  Rsvdbits                 : 7;
} PCH_GBE_CONFIG;

//
// ---------------------------- Thermal Config -----------------------------
//
typedef struct {
  UINT8  PmsyncEnable     : 1;
  UINT8  C0TransmitEnable : 1;
  UINT8  PinSelection     : 1;  ///< GpioC 0:GPIO37; 1:GPIO4, GpioD 0:GPIO5; 1:GPIO0
  UINT8  Rsvdbits         : 5;
} TS_GPIO_PIN_SETTING;

typedef enum {
  TsGpioC = 0,
  TsGpioD,
  MaxTsGpioPin
} PCH_PMSYNC_GPIO_X_SELECTION;

typedef struct {
  UINT8  Enable           : 1;
  UINT8  Rsvdbits         : 7;
  TS_GPIO_PIN_SETTING     TsGpioPinSetting[MaxTsGpioPin];
} PCH_MEMORY_THROTTLING;

typedef struct {
  PCH_MEMORY_THROTTLING         *MemoryThrottling;
} PCH_THERMAL_MANAGEMENT;

//
// ---------------------------- HPET Config -----------------------------
//
typedef struct {
  BOOLEAN Enable; ///< Determines if enable HPET function
  UINT32  Base;   ///< The HPET base address
} PCH_HPET_CONFIG;

//
// ---------------------------- Reserved Page Config -----------------------------
//
typedef enum {
  PchReservedPageToLpc,     ///< Port 80h cycles are sent to LPC.
  PchReservedPageToPcie     ///< Port 80h cycles are sent to PCIe.
} PCH_RESERVED_PAGE_ROUTE;

//
// ---------------------------- SATA Config -----------------------------
//
typedef enum {
  PchSataModeIde          = 0,
  PchSataModeAhci         = 1,
  PchSataModeRaid         = 2,
  PchSataModeLoopbackTest = 3,
  PchSataModeMax
} PCH_SATA_MODE;

typedef enum {
  PchDirectConnect,
  PchCableUp
} PCH_SATA_TOPOLOGY;

///
/// This policy configures SATA RX Equalization for each Gen Speed.
/// When enabled, BIOS will configure SATA RXEQ according to RxEq setting, else BIOS will use default setting.
/// For which RxEq value to use, please refer to PCH EDS for more details.
///
typedef struct {
  UINT8 RxEq;
  UINT8 Enable;
}PCH_SATA_GENSPEED;

///
/// This policy provides the SATA RX Equalization policy for each Gen Speed per port.
/// GenSpeed[0] configures GEN1 RxEq, GenSpeed[1] configures GEN2 RxEq, and GenSpeed[2] configures GEN3 RxEq.
///
typedef struct {
  PCH_SATA_GENSPEED GenSpeed[3];
} PCH_SATA_PORT_RXEQ;

typedef struct {
  UINT16              PortLength[2];   ///< @deprecate
  UINT8               TestMode  : 1;   ///< 0: Disable; 1: Allow entrance to the PCH SATA test modes  
  UINT8               RsvdBits  : 7;
  PCH_SATA_TOPOLOGY   PortTopology[2]; ///< @deprecate
  PCH_SATA_PORT_RXEQ  PortRxEq[6];     ///< Configure SATA RX Equalization according to platform design
} PCH_SATA_TRACE_CONFIG;

typedef struct {
  PCH_SATA_MODE         SataMode;         ///< Determines the system will be configured to which SATA mode
  PCH_SATA_TRACE_CONFIG *SataTraceConfig; ///< Decide SATA trace related configurations.
} PCH_SATA_CONTROL;

//
// ---------------------------- PCI Express Config -----------------------------
//
typedef enum {
  PchPcieAuto,
  PchPcieGen1,
  PchPcieGen2
} PCH_PCIE_SPEED;

typedef struct {
  PCH_PCIE_SPEED  PcieSpeed[LPTH_PCIE_MAX_ROOT_PORTS];  ///< Determines each PCIE Port speed capability. 0: Auto; 1: Gen1; 2: Gen2.
} PCH_PCIE_CONFIG;

//
// ---------------------------- IO APIC Config -----------------------------
//
typedef struct {
  UINT8 IoApicId;            ///< This member determines IOAPIC ID.
  UINT8 ApicRangeSelect;     ///< Define address bits 19:12 for the IOxAPIC range
  UINT8 IoApicEntry24_39 :1; ///< 0: Disable; 1: Enable IOAPIC Entry 24-39
  UINT8 RsvdBits         :7;
} PCH_IOAPIC_CONFIG;

//
// ---------------------------- PCH Platform Data -----------------------------
//
typedef struct {
  UINT8  EcPresent        : 1;              ///< Reports if EC is present or not.
  UINT8  SmmBwp           : 1;              ///< 0: Clear SMM_BWP bit; 1: Set SMM_BWP bit.
                                            ///< The BIOS must set the SMM_BWP bit while PFAT (Platform Firmware Armoring Technology)
                                            ///< support is enabled.
  UINT8  Rsvdbits         : 6;
  UINT32 TempMemBaseAddr;                   ///< Temporary Memory Base Address for PCI devices to be
                                            ///< used to initialize MMIO registers. Minimum size is
                                            ///< 32KB bytes
} PCH_PLATFORM_DATA;

//
// ------------ General PCH Platform Policy PPI definition ------------
//
struct _PCH_PLATFORM_POLICY_PPI {
  ///
  /// This member specifies the revision of the PCH policy PPI. This field is used to
  /// indicate backwards compatible changes to the protocol. Platform code that produces
  /// this PPI must fill with the correct revision value for the PCH reference code
  /// to correctly interpret the content of the PPI fields.
  ///
  UINT8                         Revision;
  UINT8                         BusNumber;      ///< Bus Number of the PCH device.
  UINT32                        Rcba;           ///< Root Complex Base Address.
  UINT16                        PmBase;         ///< Power management I/O base address.
  UINT16                        GpioBase;       ///< General purpose I/O base address.
  PCH_GBE_CONFIG                *GbeConfig;     ///< Enable/Disable Gbe function.
  PCH_THERMAL_MANAGEMENT        *ThermalMgmt;   ///< Enable the thermal management and pass the GPIO usage.
  PCH_HPET_CONFIG               *HpetConfig;    ///< Enable HPET function and the pass HPET base address.
  PCH_RESERVED_PAGE_ROUTE       Port80Route;    ///< Control where the Port 80h cycles are sent, 0: LPC; 1: PCI.
  ///
  /// SATA configuration that decides which Mode the SATA controller should operate in
  /// and describes SATA Port 0, 1 Trace length and decides whether PCH SATA TEST mode
  /// is enabled.
  ///
  PCH_SATA_CONTROL              *SataConfig;
  ///
  /// PCIE configuration describes each PCIE Port speed capability.
  /// 0: Auto; 1: Gen1; 2:Gen2
  ///
  PCH_PCIE_CONFIG               *PcieConfig;
  PCH_IOAPIC_CONFIG             *IoApicConfig;  ///< Determines IO APIC ID and IO APIC Range.
  PCH_PLATFORM_DATA             *PlatformData;  ///< Decides platform data, like EcPresent.
  ///
  /// This member decides the USB config and is a common structure for Protocols also.
  ///
  PCH_USB_CONFIG                *UsbConfig;
};

#endif
