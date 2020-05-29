/** @file
  Definition of the System Agent global NVS area protocol.  This protocol
  publishes the address and format of a global ACPI NVS buffer used as a communications
  buffer between SMM/DXE/PEI code and ASL code.
  @todo The format is derived from the ACPI reference code, version 0.95.

  Note:  Data structures defined in this protocol are not naturally aligned.

@copyright
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
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
#ifndef _SYSTEM_AGENT_GLOBAL_NVS_AREA_H_
#define _SYSTEM_AGENT_GLOBAL_NVS_AREA_H_

///
/// Includes
///
///
/// Forward reference for pure ANSI compatability
///
typedef struct _SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL;

///
/// SA Global NVS Area Protocol GUID
///
#define SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL_GUID \
  { \
    0x5bd3336f, 0x5406, 0x48a0, 0xb8, 0x58, 0xd5, 0x0f, 0x72, 0x1c, 0x83, 0x57 \
  }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gSaGlobalNvsAreaProtocolGuid;

///
/// Global NVS Area definition
///
#pragma pack(1)
typedef struct {
  UINT32      SaRcRevision;                  ///< 000 SA RC Revision
  ///
  /// IGFX relevant fields
  ///
  UINT32      IgdOpRegionAddress;            ///< 004 IGD OpRegion Starting Address
  UINT8       GfxTurboIMON;                  ///< 008 IMON Current Value
  UINT8       IgdState;                      ///< 009 IGD State (Primary Display = 1)
  UINT8       CurrentDeviceList;             ///< 010 Current Attached Device List
  UINT8       PreviousDeviceList;            ///< 011 Previous Attached Device List
  UINT16      CurrentDisplayState;           ///< 012 Current Display State
  UINT16      NextDisplayState;              ///< 014 Next Display State
  UINT32      DeviceId9;                     ///< 016 Device ID 9
  UINT32      DeviceId10;                    ///< 020 Device ID 10
  UINT32      DeviceId11;                    ///< 024 Device ID 11
  UINT8       IgdBootType;                   ///< 028 IGD Boot Type CMOS option
  UINT8       IgdPanelType;                  ///< 029 IGD Panel Type CMOs option
  UINT8       IgdPanelScaling;               ///< 030 IGD Panel Scaling
  UINT8       IgdBlcConfig;                  ///< 031 IGD BLC Configuration
  UINT8       IgdBiaConfig;                  ///< 032 IGD BIA Configuration
  UINT8       IgdSscConfig;                  ///< 033 IGD SSC Configuration
  UINT8       IgdPowerConservation;          ///< 034 IGD Power Conservation Feature Flag
  UINT8       IgdDvmtMemSize;                ///< 035 IGD DVMT Memory Size
  UINT8       IgdFunc1Enable;                ///< 036 IGD Function 1 Enable
  UINT8       IgdHpllVco;                    ///< 037 HPLL VCO
  UINT32      NextStateDid1;                 ///< 038 Next state DID1 for _DGS
  UINT32      NextStateDid2;                 ///< 042 Next state DID2 for _DGS
  UINT32      NextStateDid3;                 ///< 046 Next state DID3 for _DGS
  UINT32      NextStateDid4;                 ///< 050 Next state DID4 for _DGS
  UINT32      NextStateDid5;                 ///< 054 Next state DID5 for _DGS
  UINT32      NextStateDid6;                 ///< 058 Next state DID6 for _DGS
  UINT32      NextStateDid7;                 ///< 062 Next state DID7 for _DGS
  UINT32      NextStateDid8;                 ///< 066 Next state DID8 for _DGS
  UINT8       IgdSciSmiMode;                 ///< 070 GMCH SMI/SCI mode (0=SCI)
  UINT8       IgdPAVP;                       ///< 071 IGD PAVP data
  UINT8       LidState;                      ///< 072 Open = 1
  UINT32      AKsv0;                         ///< 073 First four bytes of AKSV (manufacturing mode)
  UINT8       AKsv1;                         ///< 077 Fifth byte of AKSV (manufacturing mode)
  UINT32      IgfxD3F0BarBaseAddress;        ///< 078 IGFX Audio D3F0 BAR Base Address
  ///
  /// Backlight Control Values
  ///
  UINT8       BacklightControlSupport;       ///< 082 Backlight Control Support
  UINT8       BrightnessPercentage;          ///< 083 Brightness Level Percentage
  ///
  /// Ambient Light Sensor Values
  ///
  UINT8       AlsEnable;                     ///< 084 Ambient Light Sensor Enable
  UINT8       AlsAdjustmentFactor;           ///< 085 Ambient Light Adjusment Factor
  UINT8       LuxLowValue;                   ///< 086 LUX Low Value
  UINT8       LuxHighValue;                  ///< 087 LUX High Value
  UINT8       ActiveLFP;                     ///< 088 Active LFP
  UINT32      AudioWaA;                      ///< 089 Audio MMIO WA 1
  UINT32      AudioWaB;                      ///< 093 Audio MMIO WA 2
  UINT32      AudioWaC;                      ///< 097 Audio MMIO WA 3
  UINT32      DeviceId12;                    ///< 101 Device ID 12
  UINT32      DeviceId13;                    ///< 105 Device ID 13
  UINT32      DeviceId14;                    ///< 109 Device ID 14
  UINT32      DeviceId15;                    ///< 113 Device ID 15
  UINT32      AudioCodecSaveAddress;         ///< 117 Codec Save Address
  UINT32      AudioCodecSaveCount;           ///< 121 Codec Save Count
  ///
  /// Add any IGFX relevant fields here and reduce reserved bytes
  ///
  UINT8       ReservedIgd[75];              ///< 125:199

  ///
  /// Switchable Graphics Info
  ///
  UINT8       SgMode;                        ///< 200 SG Mode (0=Disabled, 1=SG Muxed, 2=SG Muxless, 3=DGPU Only)
  UINT8       SgFeatureList;                 ///< 201 SG Feature list
  UINT8       SgDgpuPwrOK;                   ///< 202 dGPU PWROK GPIO assigned
  UINT8       SgDgpuHoldRst;                 ///< 203 dGPU HLD RST GPIO assigned
  UINT8       SgDgpuPwrEnable;               ///< 204 dGPU PWR Enable GPIO assigned
  UINT8       SgDgpuPrsnt;                   ///< 205 dGPU Present Detect GPIO assigned
  UINT32      CapStrPresence;                ///< 206 PEG Endpoint Capability Structure Presence
  UINT8       EndpointPcieCapOffset;         ///< 210 PEG Endpoint PCIe Capability Structure Offset
  UINT16      EndpointVcCapOffset;           ///< 211 PEG Endpoint Virtual Channel Capability Structure Offset
  UINT32      XPcieCfgBaseAddress;           ///< 213 Any Device's PCIe Config Space Base Address
  UINT16      GpioBaseAddress;               ///< 217 GPIO Base Address 
  UINT8       SgGPIOSupport;                 ///< 219 SG GPIO
  UINT32      NvIgOpRegionAddress;           ///< 220 NVIG support
  UINT32      NvHmOpRegionAddress;           ///< 224 NVHM support
  UINT32      ApXmOpRegionAddress;           ///< 228 AMDA support
  UINT8       NumberOfValidDeviceId;         ///< 232 Number of Valid Device IDs
  UINT32      DeviceId1;                     ///< 233 Device ID 1
  UINT32      DeviceId2;                     ///< 237 Device ID 2
  UINT32      DeviceId3;                     ///< 241 Device ID 3
  UINT32      DeviceId4;                     ///< 245 Device ID 4
  UINT32      DeviceId5;                     ///< 249 Device ID 5
  UINT32      DeviceId6;                     ///< 253 Device ID 6
  UINT32      DeviceId7;                     ///< 257 Device ID 7
  UINT32      DeviceId8;                     ///< 261 Device ID 8
  UINT32      OccupiedBuses1;                ///< 265 Occupied Buses from 0 to 31
  UINT32      OccupiedBuses2;                ///< 269 Occupied Buses from 32 to 63
  UINT32      OccupiedBuses3;                ///< 273 Occupied Buses from 64 to 95
  UINT32      OccupiedBuses4;                ///< 277 Occupied Buses from 96 to 127
  UINT32      OccupiedBuses5;                ///< 281 Occupied Buses from 128 to 159
  UINT32      OccupiedBuses6;                ///< 285 Occupied Buses from 160 to 191
  UINT32      OccupiedBuses7;                ///< 289 Occupied Buses from 192 to 223
  UINT32      OccupiedBuses8;                ///< 293 Occupied Buses from 224 to 255
  UINT8       Peg0LtrEnable;                 ///< 297 Latency Tolerance Reporting Control for PEG(0:1:0)
  UINT8       Peg0ObffEnable;                ///< 298 Optimized Buffer Flush and Fill for PEG(0:1:0)
  UINT8       Peg1LtrEnable;                 ///< 299 Latency Tolerance Reporting Control for PEG(0:1:1)
  UINT8       Peg1ObffEnable;                ///< 300 Optimized Buffer Flush and Fill for PEG(0:1:1)
  UINT8       Peg2LtrEnable;                 ///< 301 Latency Tolerance Reporting Control for PEG(0:1:2)
  UINT8       Peg2ObffEnable;                ///< 302 Optimized Buffer Flush and Fill for PEG(0:1:2)
  UINT16      PegLtrMaxSnoopLatency;         ///< 303 SA Peg Latency Tolerance Reporting Control
  UINT16      PegLtrMaxNoSnoopLatency;       ///< 305 SA Peg Latency Tolerance Reporting Control
  UINT8       Peg0PowerDownUnusedBundles;    ///< 307 Peg0 Unused Bundle Control
  UINT8       Peg1PowerDownUnusedBundles;    ///< 308 Peg1 Unused Bundle Control
  UINT8       Peg2PowerDownUnusedBundles;    ///< 309 Peg2 Unused Bundle Control
  UINT8       EdpValid;                      ///< 310 Check for eDP display device
  UINT32      NextStateDidEdp;               ///< 311 Next state DID for eDP
  UINT32      DeviceIdX;                     ///< 315 Device ID for eDP device
  UINT8       PackageCstateLimit;            ///< 319 The lowest C-state for the package
  UINT8       C7Allowed;                     ///< 316 Run-time C7 Allowed feature (0=Disabled, 1=Enabled)
  //
  // Add any other HG Board Info or anything else here
  //
} SYSTEM_AGENT_GLOBAL_NVS_AREA;
#pragma pack()
///
/// System Agent Global NVS Area Protocol
///
typedef struct _SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL {
  SYSTEM_AGENT_GLOBAL_NVS_AREA *Area;
} SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL;

#endif
