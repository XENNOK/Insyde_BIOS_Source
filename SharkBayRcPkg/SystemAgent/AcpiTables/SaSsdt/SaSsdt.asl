/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  2012 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  SaSsdt.asl

Abstract:

  SA SSDT Table ASL code

--*/


DefinitionBlock (
  "SaSsdt.aml",
  "SSDT",
  0x01,
  "SaSsdt",
  "SaSsdt ",
  0x3000
  )
{

  OperationRegion(SANV,SystemMemory,0xFFFF0000,0xAA55)
  Field(SANV,AnyAcc,Lock,Preserve)
  {
  SARV, 32,     /// (000) SA RC Revision
  ASLB, 32,     /// (004) IGD OpRegion base address
  IMON, 8,      /// (008) IMON Current Value
  IGDS, 8,      /// (009) IGD State (Primary Display = 1)
  CADL, 8,      /// (010) Current Attached Device List
  PADL, 8,      /// (011) Previous Attached Device List
  CSTE, 16,     /// (012) Current Display State
  NSTE, 16,     /// (014) Next Display State
  DID9, 32,     /// (016) Device Id 9
  DIDA, 32,     /// (020) Device Id 10
  DIDB, 32,     /// (024) Device Id 11
  IBTT, 8,      /// (028) IGD Boot Display Device
  IPAT, 8,      /// (029) IGD Panel Type CMOs option
  IPSC, 8,      /// (030) IGD Panel Scaling
  IBLC, 8,      /// (031) IGD BLC Configuration
  IBIA, 8,      /// (032) IGD BIA Configuration
  ISSC, 8,      /// (033) IGD SSC Configuration
  IPCF, 8,      /// (034) IGD Power Conservation Feature Flag
  IDMS, 8,      /// (035) IGD DVMT Memory Size
  IF1E, 8,      /// (036) IGD Function 1 Enable
  HVCO, 8,      /// (037) HPLL VCO
  NXD1, 32,     /// (038) Next state DID1 for _DGS
  NXD2, 32,     /// (042) Next state DID2 for _DGS
  NXD3, 32,     /// (046) Next state DID3 for _DGS
  NXD4, 32,     /// (050) Next state DID4 for _DGS
  NXD5, 32,     /// (054) Next state DID5 for _DGS
  NXD6, 32,     /// (058) Next state DID6 for _DGS
  NXD7, 32,     /// (062) Next state DID7 for _DGS
  NXD8, 32,     /// (066) Next state DID8 for _DGS
  GSMI, 8,      /// (070) GMCH SMI/SCI mode (0=SCI)
  PAVP, 8,      /// (071) IGD PAVP data
  LIDS, 8,      /// (072) Lid State (Lid Open = 1)
  KSV0, 32,     /// (073) First four bytes of AKSV (mannufacturing mode)
  KSV1, 8,      /// (077) Fifth byte of AKSV (mannufacturing mode)
  BBAR, 32,     /// (078) IGFX Audio (D3F0) MMIO BAR Address
  BLCS, 8,      /// (082) Backlight Control Support
  BRTL, 8,      /// (083) Brightness Level Percentage
  ALSE, 8,      /// (084) ALS Enable
  ALAF, 8,      /// (085) Ambient Light Adjusment Factor
  LLOW, 8,      /// (086) LUX Low Value
  LHIH, 8,      /// (087) LUX High Value
  ALFP, 8,      /// (088) Active LFP
  AUDA, 32,     /// (089) Audio MMIO WA 1
  AUDB, 32,     /// (093) Audio MMIO WA 2
  AUDC, 32,     /// (097) Audio MMIO WA 3 
  DIDC, 32,	/// (101) Device Id 12
  DIDD, 32,	/// (105) Device Id 13
  DIDE, 32,	/// (109) Device Id 14
  DIDF, 32,	/// (113) Device Id 15
  CADR, 32,     /// (117) Codec Save Address 
  CCNT, 8,      /// (121) Codec Save Count 
  ///
  /// Switchable Graphics Info
  ///
  Offset(200),
  SGMD, 8,      /// (200) SG Mode (0=Disabled, 1=SG Muxed, 2=SG Muxless, 3=DGPU Only)
  SGFL, 8,      /// (201) SG Feature List
  PWOK, 8,      /// (202) dGPU PWROK GPIO assigned
  HLRS, 8,      /// (203) dGPU HLD RST GPIO assigned
  PWEN, 8,      /// (204) dGPU PWR Enable GPIO assigned
  PRST, 8,      /// (205) dGPU Present Detect GPIO assigned
  CPSP, 32,     /// (206) PEG Endpoint Capability Structure Presence (Bit 0: Virtual Channel Capability)
  EECP, 8,      /// (210) PEG Endpoint PCIe Capability Structure Offset
  EVCP, 16,     /// (211) PEG Endpoint Virtual Channel Capability Structure Offset
  XBAS, 32,     /// (213) Any Device's PCIe Config Space Base Address
  GBAS, 16,     /// (217) GPIO Base Address
  SGGP, 8,      /// (219) SG GPIO Support
  NVGA, 32,     /// (220) NVIG opregion address
  NVHA, 32,     /// (224) NVHM opregion address
  AMDA, 32,     /// (228) AMDA opregion address
  NDID, 8,      /// (232) Number of Valid Device IDs
  DID1, 32,     /// (233) Device ID 1
  DID2, 32,     /// (237) Device ID 2
  DID3, 32,     /// (241) Device ID 3
  DID4, 32,     /// (245) Device ID 4
  DID5, 32,     /// (249) Device ID 5
  DID6, 32,     /// (253) Device ID 6
  DID7, 32,     /// (257) Device ID 7
  DID8, 32,     /// (261) Device ID 8
  OBS1, 32,     /// (265) Occupied Buses - from 0 to 31
  OBS2, 32,     /// (269) Occupied Buses - from 32 to 63
  OBS3, 32,     /// (273) Occupied Buses - from 64 to 95
  OBS4, 32,     /// (277) Occupied Buses - from 96 to 127
  OBS5, 32,     /// (281) Occupied Buses - from 128 to 159
  OBS6, 32,     /// (285) Occupied Buses - from 160 to 191
  OBS7, 32,     /// (289) Occupied Buses - from 192 to 223
  OBS8, 32,     /// (293) Occupied Buses - from 224 to 255
  LTRA, 8,      /// (297) Latency Tolerance Reporting Enable
  OBFA, 8,      /// (298) Optimized Buffer Flush and Fill
  LTRB, 8,      /// (299) Latency Tolerance Reporting Enable
  OBFB, 8,      /// (300) Optimized Buffer Flush and Fill
  LTRC, 8,      /// (301) Latency Tolerance Reporting Enable
  OBFC, 8,      /// (302) Optimized Buffer Flush and Fill
  SMSL, 16,     /// (303) SA Peg Latency Tolerance Reporting Max Snoop Latency
  SNSL, 16,     /// (305) SA Peg Latency Tolerance Reporting Max No Snoop Latency
  P0UB, 8,      /// (307) Peg0 Unused Bundle Control
  P1UB, 8,      /// (308) Peg1 Unused Bundle Control
  P2UB, 8,      /// (309) Peg2 Unused Bundle Control
  EDPV, 8,      /// (310) Check for eDP display device
  NXDX, 32,     /// (311) Next state DID for eDP
  DIDX, 32,     /// (315) Device ID for eDP device
  PCSL, 8,      /// (319) The lowest C-state for the package
  SC7A, 8,      /// (316) Run-time C7 Allowed feature (0=Disabled, 1=Enabled)
  }
  include ("Sa.asl")
}