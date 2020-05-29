/** @file

  Interface definition details for MBP during DXE phase.

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

#ifndef _MBP_DATA_PROTOCOL_H_
#define _MBP_DATA_PROTOCOL_H_
#include "CoreBiosMsg.h"

///
/// MBP Protocol for DXE phase
///
#define ME_BIOS_PAYLOAD_DATA_PROTOCOL_GUID \
  { 0x71A19494, 0x2AB6, 0x4E96, { 0x85, 0x81, 0xCF, 0x34, 0x25, 0x42, 0x73, 0xFE }}

///
/// Revision 1:  Original version
///
#define DXE_MBP_DATA_PROTOCOL_REVISION_1  1
#define DXE_MBP_DATA_PROTOCOL_REVISION_2  2
extern EFI_GUID gMeBiosPayloadDataProtocolGuid;

#pragma pack(push, 1)
typedef struct _MBP_FW_VERSION_NAME {
  UINT32  MajorVersion : 16;
  UINT32  MinorVersion : 16;
  UINT32  HotfixVersion : 16;
  UINT32  BuildVersion : 16;
} MBP_FW_VERSION_NAME;

typedef struct _MBP_ICC_PROFILE {
  UINT8               NumIccProfiles;
  UINT8               IccProfileSoftStrap;
  UINT8               IccProfileIndex;
  UINT8               Reserved;
  ICC_LOCK_REGS_INFO  IccLockRegInfo;
} MBP_ICC_PROFILE;

typedef struct _MBP_FW_CAPS_SKU {
  MEFWCAPS_SKU  FwCapabilities;
  BOOLEAN       Available;
} MBP_FW_CAPS_SKU;

typedef struct _MBP_ROM_BIST_DATA {
  UINT16  DeviceId;
  UINT16  FuseTestFlags;
  UINT32  UMCHID[4];
} MBP_ROM_BIST_DATA;

typedef struct _MBP_PLATFORM_KEY {
  UINT32  Key[8];
} MBP_PLATFORM_KEY;

typedef struct _MBP_PLAT_TYPE {
  PLATFORM_TYPE_RULE_DATA RuleData;
  BOOLEAN                 Available;
} MBP_PLAT_TYPE;

typedef union _HWA_DATA {
  UINT32  Raw;
  struct {
    UINT32  MediaTablePush : 1;
    UINT32  Reserved : 31;
  } Fields;
} HWA_DATA;

typedef struct _MBP_HWA_REQ {
  HWA_DATA Data;
  BOOLEAN Available;
} MBP_HWA_REQ;

typedef struct _MBP_PERF_DATA {
  UINT32  PwrbtnMrst;
  UINT32  MrstPltrst;
  UINT32  PltrstCpurst;
} MBP_PERF_DATA;

typedef struct _PLAT_BOOT_PERF_DATA {
  MBP_PERF_DATA MbpPerfData;
  BOOLEAN       Available;
} PLAT_BOOT_PERF_DATA;

typedef struct _MBP_NFC_DATA {
  UINT32 DeviceType :2;
  UINT32 Reserved : 30;
} MBP_NFC_DATA;

typedef struct _MBP_NFC_SUPPORT {
  MBP_NFC_DATA   NfcData;
  BOOLEAN        Available;
} MBP_NFC_SUPPORT;

typedef struct {
  MBP_FW_VERSION_NAME FwVersionName;
  MBP_FW_CAPS_SKU     FwCapsSku;
  MBP_ROM_BIST_DATA   RomBistData;
  MBP_PLATFORM_KEY    PlatformKey;
  MBP_PLAT_TYPE       FwPlatType;
  MBP_ICC_PROFILE     IccProfile;
  AT_STATE_INFO       AtState;
  UINT32              MFSIntegrity;
  MBP_HWA_REQ         HwaRequest;
  PLAT_BOOT_PERF_DATA PlatBootPerfData;
  MBP_NFC_SUPPORT     NfcSupport;
} ME_BIOS_PAYLOAD;
#pragma pack(pop)
///
/// MBP DXE Protocol
///
typedef struct _DXE_MBP_DATA_PROTOCOL {
  EFI_HANDLE      Handle;
  UINT8           Revision;
  ME_BIOS_PAYLOAD MeBiosPayload;
} DXE_MBP_DATA_PROTOCOL;

#endif
