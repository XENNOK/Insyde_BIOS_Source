/** @file
  Definition of the PPM global NVS area protocol.  This protocol
  publishes the address and format of a global ACPI NVS buffer used as a communications
  buffer between SMM/DXE/PEI code and ASL code.
  @todo The format is derived from the ACPI reference code, version 0.95.

  Note:  Data structures defined in this protocol are not naturally aligned.

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

#ifndef _PPM_GLOBAL_NVS_AREA_H_
#define _PPM_GLOBAL_NVS_AREA_H_

///
/// Forward reference for pure ANSI compatability
///
typedef struct _PPM_GLOBAL_NVS_AREA_PROTOCOL  PPM_GLOBAL_NVS_AREA_PROTOCOL;

///
/// PPM Global NVS Area Protocol GUID - {6C50CDCB-7F46-4dcc-8DDD-D9F0A3C61128}
///
#define EFI_PPM_GLOBAL_NVS_AREA_PROTOCOL_GUID \
  { 0x6c50cdcb, 0x7f46, 0x4dcc, {0x8d, 0xdd, 0xd9, 0xf0, 0xa3, 0xc6, 0x11, 0x28} }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gPpmGlobalNvsAreaProtocolGuid;

//
// Processor Power Management GlobalNvs Revisions
//
#define PPM_GLOBAL_NVS_AREA_REVISION_1  1 ///< Initial Version

#pragma pack(1)

typedef struct {
  UINT16 CtdpPowerLimit1;      ///< CTDP Power Limit1
  UINT16 CtdpPowerLimit2;      ///< CTDP Power Limit2
  UINT8  CtdpPowerLimitWindow; ///< CTDP Power Limit Time Window
  UINT8  CtdpCtc;              ///< CTDP CTC
  UINT8  CtdpTar;              ///< CTDP TAR
  UINT8  CtdpPpc;              ///< CTDP PPC
} PPM_CTDP_LEVEL_SETTINGS;

///
/// Global NVS Area definition
///
typedef struct {
  UINT8  Revision;                 ///< (0) PPM GlobalNvs Revision
  UINT32 PpmFlags;                 ///< (1-4) PPM Flags
  UINT8  Reserved;                 ///< (5) Reserved
  //
  // Thermal Configuration Values
  //
  UINT8  AutoCriticalTripPoint;    ///< (6) Auto Critical Trip Point
  UINT8  AutoPassiveTripPoint;     ///< (7) Auto Passive Trip Point
  UINT8  AutoActiveTripPoint;      ///< (8) Auto Active Trip Point
  UINT32 Cpuid;                    ///< (9) CPUID
  //
  // ConfigTDP Values
  //
  UINT8 ConfigurablePpc;           ///< (13) Boot Mode vlues for _PPC
  //
  // ConfigTDP Level settngs
  //
  UINT8 CustomConfigTdp;           ///< (14) ConfigTdp Enabled/Disabled
  UINT8 CtdpLevelsSupported;       ///< (15) ConfigTdp Number Of Levels
  UINT8 ConfigTdpBootModeIndex;    ///< (16) CTDP Boot Mode Index
  ///
  /// (17) CTDP Level 0 Power Limit1
  /// (19) CTDP Level 0 Power Limit2
  /// (21) CTDP Level 0 Power Limit1 Time Window
  /// (22) CTDP Level 0 CTC
  /// (23) CTDP Level 0 TAR
  /// (24) CTDP Level 0 PPC
  /// (25) CTDP Level 1 Power Limit1
  /// (27) CTDP Level 1 Power Limit2
  /// (29) CTDP Level 1 Power Limit1 Time Window
  /// (30) CTDP Level 1 CTC
  /// (31) CTDP Level 1 TAR
  /// (32) CTDP Level 1 PPC
  /// (33) CTDP Level 2 Power Limit1
  /// (35) CTDP Level 2 Power Limit2
  /// (37) CTDP Level 2 Power Limit1 Time Window
  /// (38) CTDP Level 2 CTC
  /// (39) CTDP Level 2 TAR
  /// (40) CTDP Level 2 PPC
  ///
  PPM_CTDP_LEVEL_SETTINGS CtdpLevelSettings[3];
  //
  // Mwait Hints and Latency values for C3/C6/C7/C7S
  //
  UINT8  C3MwaitValue;             ///< (41) Mwait Hint value for C3
  UINT8  C6MwaitValue;             ///< (42) Mwait Hint value for C6
  UINT8  C7MwaitValue;             ///< (43) Mwait Hint value for C6
  UINT8  CDMwaitValue;             ///< (44) Mwait Hint value for C7/C8/C9/C10
  UINT16 C3Latency;                ///< (45-46) Latency value for C3
  UINT16 C6Latency;                ///< (47-48) Latency Value for C6
  UINT16 C7Latency;                ///< (49-50) Latency Value for C6
  UINT16 CDLatency;                ///< (51-52) Latency Value for C7/C8/C9/C10
  UINT16 CDIOLevel;                ///< (53-54) IO Level Value for C7/C8/C9/C10
  UINT16 CDPowerValue;             ///< (55-56) Power Value for C7/C8/C9/C10
  UINT8  MiscPowerManagementFlags; ///< (57) MiscPowerManagementFlags
} PPM_GLOBAL_NVS_AREA;
#pragma pack()
///
/// PPM Global NVS Area Protocol
///
struct _PPM_GLOBAL_NVS_AREA_PROTOCOL {
  PPM_GLOBAL_NVS_AREA *Area;
};

#endif
