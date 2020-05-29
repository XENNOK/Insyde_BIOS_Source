/** @file
  Interface definition details between System Agent and platform drivers during DXE phase.

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

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#ifndef _SA_PLATFORM_POLICY_H_
#define _SA_PLATFORM_POLICY_H_

#include "SaAccess.h"

///
/// SA Policy provided by platform for DXE phase {912A2913-42A8-45b0-822F-A94D1EAE9965}
///
#define DXE_PLATFORM_SA_POLICY_GUID \
  { \
    0x912a2913, 0x42a8, 0x45b0, 0x82, 0x2f, 0xa9, 0x4d, 0x1e, 0xae, 0x99, 0x65 \
  }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gDxePlatformSaPolicyGuid;

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///
/// Don't change the original DXE_MCH_PLATFORM_POLICY_PROTOCOL_REVISION macro, external
/// modules maybe have consumed this macro in their source code.  Directly
/// update the DXE_MCH_PLATFORM_POLICY_PROTOCOL_REVISION version number may cause those
/// external modules to auto mark themselves wrong version info.
/// Always create new version macro for new PlatformMchPolicy protocol interface.
///
/// DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION
///   First version
///
#define DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION    1
///
/// DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_2
///   Moved PcieDevsOverride to SampleCode as part of SA_PCIE_CONFIGURATION structure
///   Added SA_PCIE_CONFIGURATION.PcieLtrDevsOverride
///
#define DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_2  2
///
/// DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_3
///   Added DXE_PLATFORM_SA_POLICY_PROTOCOL.VbiosConfig
///         SA_IGD_CONFIGURATION.VbtAddress
///         SA_IGD_CONFIGURATION.Size
///
#define DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_3  3
///
/// DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_4
///   Added SA_IGD_CONFIGURATION.CdClk
///         SA_IGD_CONFIGURATION.CdClkVar
///
#define DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_4  4
///
/// DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_5
///   Added SA_IGD_CONFIGURATION.PlatformConfig
///
#define DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_5  5
///
/// DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_6
///   Added SA_IGD_CONFIGURATION.IuerStatusVal
///   Added SA_MISC_CONFIGURATION.SaHdaVerbTableNum
///         SA_MISC_CONFIGURATION.*SaHdaVerbTable
///
#define DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_6  6
///
/// DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_7
///   Added SA_IGD_CONFIGURATION.GopVersion
///   Added SA_PCIE_CONFIGURATION.PegPwrOpt[SA_PEG_MAX_FUN]
///
#define DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_7  7
///
/// DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_8
///   Added SA_PCIE_CONFIGURATION.C7Allowed
///
#define DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION_8  8

#define SA_VTD_ENGINE_NUMBER                        2
#define SA_PCIE_DEV_END_OF_TABLE                    0xFFFF
///
/// The data elements should be initialized by a Platform Module.  The data structure is for
/// VT-d driver initialization
///
typedef struct {
  BOOLEAN               VtdEnable;                         ///< This field is used to describe whether or not the VT-d function should be enabled
  EFI_PHYSICAL_ADDRESS  *RmrrUsbBaseAddress;               ///< The field is used to describe the platform USB Reserved memory for Intel VT-d support. Platform code should provide this information for Intel VT-d DXE driver use
  UINT32                BaseAddress[SA_VTD_ENGINE_NUMBER]; ///< This field is used to describe the base addresses for VT-d function
} SA_VTD_CONFIGURATION;

///
/// The Memory Configuration includes DIMM SPD address Map and DIMM Slot Mechanical present bit map.
///
/// The data elements should be initialized by a Platform Module.  Refer to
/// $(PROJECT_SA_ROOT)\SmbiosMemory\Dxe\SmbiosMemory.c for the usage.
///
typedef struct {
/**
  Dimm SPD address
  Only Server support 2 channels * 3 slots per channel = 6 sockets totally
  The Desktop and mobile only support 2 channels * 2 slots per channel = 4 sockets totally
  So there is mapping rule here for Desktop and mobile that there are no more 4 DIMMS totally in a system:
    Channel A/ Slot 0 --> Dimm 0 --> SpdAddressTable[0]
    Channel A/ Slot 1 --> Dimm 1 --> SpdAddressTable[1]
    Channel B/ Slot 0 --> Dimm 2 --> SpdAddressTable[2]
    Channel B/ Slot 1 --> Dimm 3 --> SpdAddressTable[3]
  Refer to $(PROJECT_SA_ROOT)\SmbiosMemory\Dxe\SmbiosMemory.c for use
  If change the mapping rule, please update the Revision number.
**/
  UINT8 *SpdAddressTable;
/**
  Channel A DIMM Slot Mechanical present bit map, bit 0 -> DIMM 0, bit 1 -> DIMM1, ...
  if the bit is 1, the related DIMM slot is present.
  E.g. if channel A has 2 DIMMs,  ChannelASlotMap = 0x03;
  E.g. if channel A has only 1 DIMMs,  ChannelASlotMap = 0x01;
  Refer to $(PROJECT_SA_ROOT)\SmbiosMemory\Dxe\SmbiosMemory.c
**/
  UINT8 ChannelASlotMap;
/**
  Channel B DIMM Slot Mechanical present bit map, bit 0 -> DIMM 0, bit 1 -> DIMM1, ...
  if the bit is 1, the related DIMM slot is present.
  E.g. if channel B has 2 DIMMs,  ChannelBSlotMap = 0x03;
  E.g. if channel B has only 1 DIMMs,  ChannelBSlotMap = 0x01;
  Refer to $(PROJECT_SA_ROOT)\SmbiosMemory\Dxe\SmbiosMemory.c
**/
  UINT8 ChannelBSlotMap;
  UINT8 RmtBdatEnable;    ///< This flag is used by the MRC for DDR3 channel training (default is false). Please refer to the MRC documentation for more details
  UINT8 MrcTimeMeasure;   ///< Time measure
  UINT8 MrcFastBoot;      ///< Fast boot
} SA_MEMORY_CONFIGURATION;

///
/// The value before AutoConfig match the setting of PCI Express Base Specification 1.1, please be careful for adding new feature
///
typedef enum {
  PcieAspmDisabled,
  PcieAspmL0s,
  PcieAspmL1,
  PcieAspmL0sL1,
  PcieAspmAutoConfig,
  PcieAspmMax
} SA_PCIE_ASPM_CONFIG;

///
/// Device List Structure
///
typedef struct {
  UINT16  VendorId; ///< PCI Configuration space offset 0
  UINT16  DeviceId; ///< PCI Configuration space offset 2
  UINT8   RevId;    ///< PCI Configuration space offset 8; 0xFF means all steppings
  UINT8   RootApmcMask;
  UINT8   EndpointApmcMask;
} PCIE_ASPM_DEV_INFO;

typedef struct {
  UINT16  VendorId; ///< PCI Config space offset 0
  UINT16  DeviceId; ///< PCI Config space offset 2
  UINT8   RevId;    ///< PCI Config space offset 8; 0xFF means all steppings
/**
  SnoopLatency bit definition
  Note: All Reserved bits must be set to 0

  BIT[15]     - When set to 1b, indicates that the values in bits 9:0 are valid
                When clear values in bits 9:0 will be ignored
  BIT[14]     - Should be set to 0b
  BIT[13]     - Reserved
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
  BIT[14]     - Should be set to 0b
  BIT[13]     - Reserved
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
} PCIE_LTR_DEV_INFO;

///
/// PCIE Power Optimizer config
///
typedef struct {
  UINT8   LtrEnable;
  UINT16  LtrMaxSnoopLatency;
  UINT16  LtrMaxNoSnoopLatency;
  UINT8   ObffEnable;
} SA_PCIE_PWR_OPT;


///
/// The PCI Express Configuration info includes PCI Resources Range Base and Limits and the control
/// for PEG ASPM.
///
/// The data elements should be initialized by a Platform Module.  For the data structure for PCI IO
/// and PCI Memory address range info refer to $(PROJECT_SA_ROOT)\PciHostBridge\Dxe\PciHostBridge.c
/// for the usage.
///
typedef struct {
  SA_PCIE_ASPM_CONFIG DmiAspm;                       ///< This field is used to describe the ASPM control for DMI
  SA_PCIE_ASPM_CONFIG PegAspm[SA_PEG_MAX_FUN];       ///< This field is used to describe the ASPM control for PEG Ports
  UINT8               PegAspmL0s[SA_PEG_MAX_FUN];    ///< This field is used to describe the PEG L0s advanced control
  UINT8               PegDeEmphasis[SA_PEG_MAX_FUN]; ///< This field is used to describe the DeEmphasis control for PEG (-6 dB and -3.5 dB are the options)
  BOOLEAN             DmiExtSync;                    ///< This field is used to describe the DMI Extended Sync enable/disable control
  UINT8               DmiDeEmphasis;                 ///< This field is used to describe the DeEmphasis control for DMI (-6 dB and -3.5 dB are the options)
  BOOLEAN             DmiIot;                        ///< This field is used to describe the IOT control for DMI, default is 0
  UINT8               C7Allowed;                     ///< Enable/Disable C7 allowed for PEG Ports [Run-time control]. 0=Disable (default) and 1=Enable
  ///
  /// This field is used as a pointer to the ASPM device override table, default points to an
  /// existing table, mPcieAspmDevsOverride, in the sample code. Refer to
  /// $(PROJECT_SA_ROOT)\Protocol\SaPlatformPolicy\SaPlatformPolicy.h and
  /// $(PROJECT_SA_ROOT)\SampleCode\SaPolicyInit\Dxe\SaDxePolicyInit.c for the usage.
  ///
  /// Note:  This exclusion list helps avoid potential system hangs.
  ///
  PCIE_ASPM_DEV_INFO  *PcieAspmDevsOverride;
  ///
  /// This field is used as a pointer to the LTR device override table, default points to an existing
  /// table, mPcieLtrDevsOverride, in the sample code. Refer to
  /// $(PROJECT_SA_ROOT)\Protocol\SaPlatformPolicy\SaPlatformPolicy.h and
  /// $(PROJECT_SA_ROOT)\SampleCode\SaPolicyInit\Dxe\SaDxePolicyInit.c for the usage.
  ///
  PCIE_LTR_DEV_INFO   *PcieLtrDevsOverride;
  SA_PCIE_PWR_OPT     PegPwrOpt[SA_PEG_MAX_FUN];     ///< This field is used to describe the PCIe LTR/OBFF relevant settings
} SA_PCIE_CONFIGURATION;

///
/// Audio Codec Verb Table
///
typedef struct {
  UINT32  VendorDeviceId;
  UINT16  SubSystemId;
  UINT8   RevisionId; ///< 0xFF applies to all steppings
  UINT8   FrontPanelSupport;
  UINT16  NumberOfRearJacks;
  UINT16  NumberOfFrontJacks;
} SA_HDA_VERB_TABLE_HEADER;

typedef struct {
  SA_HDA_VERB_TABLE_HEADER  VerbTableHeader;
  UINT32                    *VerbTableData;
} SA_HDA_VERB_TABLE;

///
/// This data structure includes IGD related configuration Variables. The data elements should
/// be initialized by a Platform Module.
///
typedef struct {
  UINT8                 RenderStandby;     ///< This field is used to enable or disable RC6 (Render Standby)
  UINT8                 DeepRenderStandby; ///< @deprecated
  EFI_PHYSICAL_ADDRESS  VbtAddress;        ///< This field points to the GOP VBT data buffer
  UINT32                Size;              ///< This field gives the size of the GOP VBT Data buffer
  UINT8                 CdClk;             ///< This field is used to control the Cd Clock Frequency by the user. 0: 337.5Mhz, 1: 450Mhz, 2: 540Mhz
  UINT8                 CdClkVar;          ///< This field gives the Cd Clock Frequencies supported by the system.
  UINT8                 PlatformConfig;    ///< This field gives the Platform Configuration Information (0 = Platform is S0ix Capable for ULT SKUs only, 1 = Platform is not S0ix Capable, 2 = Force Platform is S0ix Capable for All SKUs)
  UINT32                IuerStatusVal;     ///< This field holds the current status of all the supported Ultrabook events (Intel(R) Ultrabook Event Status bits)
  CHAR16                GopVersion[0x10];  ///< This field holds the GOP Driver Version. It is an Output Protocol and updated by the RC

//[-start-130401-IB05160428-add]//
  UINT8                 FdiLaneReversal;   /// for specific platform which use reversal fdi lane lay out.
//[-end-130401-IB05160428-add]//
} SA_IGD_CONFIGURATION;

///
/// Subsystem Vendor ID / Subsystem ID
///
typedef struct {
  UINT16  SubSystemVendorId;
  UINT16  SubSystemId;
} SA_DEFAULT_SVID_SID;

///
/// This data structure includes miscellaneous configuration variables such SA thermal device
/// control. The data elements should be initialized by a Platform Module.
///
typedef struct {
  BOOLEAN             ChapDeviceEnable;  ///< This field is used to control enable or disable System Agent device (0,7,0)
  BOOLEAN             Device4Enable;     ///< This field is used to control enable or disable System Agent device (0,4,0)
  SA_DEFAULT_SVID_SID *DefaultSvidSid;   ///< This field contains the Subsystem VendorID and Subsystem ID values to program to SA devices
  BOOLEAN             CridEnable;        ///< This field is used to control enable or disable HSW CRID control (to support Intel(R) SIPP)
  BOOLEAN             AudioEnable;       ///< This field is used to control enable or disable System Agent device (0,3,0)
  BOOLEAN             FviReport;         ///< This field is used to control enable or disable of Firmware Version Info (FVI) reporting. 0: Disable; 1: Enable
  UINT8               FviSmbiosType;     ///< This field is used to control the FVI type reported
  UINT8               SaHdaVerbTableNum; ///< This field gives the number of HD Audio verb tables that are loaded
  SA_HDA_VERB_TABLE   *SaHdaVerbTable;   ///< This field points to the current HD Audio verb table

//[-start-140521-IB05400527-add]//
  BOOLEAN             EnableAbove4GBMmio;///< This field is used to control enable or disable above 4GB MMIO function (64 bit PCI MMIO)
//[-end-140521-IB05400527-add]//
} SA_MISC_CONFIGURATION;

///
/// This data structure includes Switchable Graphics VBIOS configuration. The data elements
/// should be initialized by a Platform Module.
///
typedef struct {
  UINT8 LoadVbios    : 1; ///< This field is used to describe if the dGPU VBIOS needs to be loaded
  UINT8 ExecuteVbios : 1; ///< This field is used to describe if the dGPU VBIOD need to be executed
  UINT8 VbiosSource  : 1; ///< This field is used to identify the source location of dGPU VBIOS
  UINT8 Reserved     : 5;
} SA_SG_VBIOS_CONFIGURATION;

//[-start-120629-IB06460411-add]//
typedef enum {
  SAPlatformBoardIDGraysReef,
  SAPlatformBoardIDBaskingRidge,
  SAPlatformBoardIDMax
} SA_PLATFORM_BOARD_ID;
//[-end-120629-IB06460411-add]//
///
/// SA DXE Platform Policy
///
/// The DXE_PLATFORM_SA_POLICY_PROTOCOL producer drvier is recommended to
/// set all the DXE_PLATFORM_SA_POLICY_PROTOCOL size buffer zero before init any member parameter,
/// this clear step can make sure no random value for those unknow new version parameters.
///
/// Make sure to update the Revision if any change to the protocol, including the existing
/// internal structure definations
///
typedef struct _DXE_PLATFORM_SA_POLICY_PROTOCOL {
  /// This field specifies the revision of the protocol.  The protocol is expected to change in
  /// a backwards compatible manner as the chipset configuration options are added or removed.
  /// Major changes will result in new protocol definitions/GUID.  The protocol producer must update
  /// this field at build time.
  ///
  /// Please ensure to use DXE_SA_PLATFORM_POLICY_PROTOCOL_REVISION macro to define the protocol
  /// revision as input for this version.
  ///
  UINT8                      Revision;
  SA_VTD_CONFIGURATION       *Vtd;          ///< This field is used to describe the configuration of VT-d function used by System Agent Reference code
  SA_PCIE_CONFIGURATION      *PcieConfig;   ///< This field is used to describe the PCIE configuration used by System Agent Reference code
  SA_MEMORY_CONFIGURATION    *MemoryConfig; ///< This field is used to describe the Memory configuration used by System Agent Reference code
  SA_IGD_CONFIGURATION       *IgdConfig;    ///< This field is used to describe the IGD configuration used by System Agent Reference code
  SA_MISC_CONFIGURATION      *MiscConfig;   ///< This field is used to describe some miscellaneous configuration used by System Agent Reference code, such as device enable/disable, CRID support, etc
  SA_SG_VBIOS_CONFIGURATION  *VbiosConfig;  ///< This field is used to describe Switchable Graphics configuration used by System Agent Reference Code
//[-start-120629-IB06460411-add]//
  SA_PLATFORM_BOARD_ID    PlatformBoardID;
//[-end-120629-IB06460411-add]//
} DXE_PLATFORM_SA_POLICY_PROTOCOL;

#endif
