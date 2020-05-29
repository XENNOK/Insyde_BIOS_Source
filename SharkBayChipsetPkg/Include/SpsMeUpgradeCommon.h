/** @file

  Common header for SPS ME upgrade DXE and SMM drivers

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_ME_UPGRADE_COMMON_H_
#define _SPS_ME_UPGRADE_COMMON_H_

#include <Protocol/SpsMeUpgrade.h>
#include <Guid/SpsMeNonceStoreGuid.h>

#define SPS_DEBUG_FULL_MESSAGE         0
#define SPS_DEBUG(Expression)          if (SPS_DEBUG_FULL_MESSAGE) {DEBUG (Expression);}

#pragma pack(1)

typedef enum {
  SPS_DESC_REGION              = 0,
  SPS_BIOS_REGION,
  SPS_ME_REGION,
  SPS_GBE_REGION,
  SPS_PDR_REGION,
  SPS_DER_REGION,
  SPS_MAX_FLASH_REGION,
  SPS_REGION_TYPE_OF_EOS       = 0xff,
} SPS_FLASH_REGION_TYPE;

typedef enum {
  SPS_ME_UPGRADE_PHASE_ENTIRE_ME_REGION,
  SPS_ME_UPGRADE_PHASE_ME_OPERATIONAL_REGION,
  SPS_ME_UPGRADE_PHASE_ONLY_ENABLE_HMRFPO,
  SPS_ME_UPGRADE_PHASE_DO_NOTHING,
  SPS_ME_UPGRADE_PHASE_OUT_OF_ME_REGION,
  SPS_ME_UPGRADE_PHASE_FIRST_TIME,
  SPS_ME_UPGRADE_PHASE_MAX
} SPS_ME_UPGRADE_PHASE;

///
/// SPS ME Flash Partition Table entry.
///
typedef struct {
  UINT32                               Signature;
  UINT8                                Reserved[4];
  UINT32                               Offset;
  UINT32                               Size;
  UINT8                                Reserved1[16];
} SPS_ME_BINARY_FPT_ENTRY;

///
/// SPS ME Flash Partition Table header.
///
typedef struct {
  UINT8                                Reserved[16];
  UINT32                               Signature;
  UINT32                               NumberOfEntries;
  UINT8                                Reserved1[24];
  SPS_ME_BINARY_FPT_ENTRY              Entry[1];
} SPS_ME_BINARY_FPT;

///
/// SPS ME Sub region information.
///
typedef struct {
  UINT32                               Signature;
  UINTN                                Start;
  UINTN                                End;
  UINTN                                Offset;
  UINTN                                Size;
  UINT8                                *Data;
  BOOLEAN                              DataValid;
  BOOLEAN                              Valid;
} SPS_ME_SUB_REGION_INFO;

///
/// SPS ME firmware version
///
typedef struct _SPS_ME_UPGRADE_FW_VERSION {
  UINT16           MinorNumber;        ///< Version minor number
  UINT8            MajorNumber;        ///< Version major number
  UINT8            ServerSegmentCode;  ///< Version Server segment code
  UINT16           BuildNumber;        ///< Version build number
  UINT16           PatchNumber;        ///< Version patch number
} SPS_ME_UPGRADE_FW_VERSION;

typedef struct _SPS_ME_UPGRADE_FW_VERSION_DATA {
  SPS_ME_UPGRADE_FW_VERSION  ActFw;
  SPS_ME_UPGRADE_FW_VERSION  RcvFw;
  SPS_ME_UPGRADE_FW_VERSION  BkpFw;
} SPS_ME_UPGRADE_FW_VERSION_DATA;

///
/// SPS ME map information.
///
typedef struct {
  UINTN                                MeRegionBase;
  UINTN                                MeRegionLimit;
  UINTN                                MeRegionSize;
  SPS_ME_UPGRADE_FW_VERSION_DATA       FwVersion;
  BOOLEAN                              Valid;

  UINTN                                NumberOfSubRegion;
  SPS_ME_SUB_REGION_INFO               *SubRegionInfo;
} SPS_ME_UPGRADE_MAP_INFO;

///
/// SPS ME Flash Partition Signature
///   $FPT - Flash Partition Table     - basic ME configuration
///   MFSB - ME File System Backup     - ME factory presets configuration
///   FTPR - Fault Tolerant Partition  - Recovery Boot Loader Code
///   MFS  - ME File System            - ME firmware configuration
///   OPR1 - Operational Image 1       - first image with ME operational firmware
///   OPR2 - Operational Image 2       - second image with ME operational firmware (in dual-image configuration only)
///   SCA  - Shared Configuration Area - Configuration area shared between Recovery
///   ASAP -
///
#define SPS_ME_SUB_REGION_SIGNATURE_FPT   SIGNATURE_32 ('$', 'F', 'P', 'T')
#define SPS_ME_SUB_REGION_SIGNATURE_MFSB  SIGNATURE_32 ('M', 'F', 'S', 'B')
#define SPS_ME_SUB_REGION_SIGNATURE_FTPR  SIGNATURE_32 ('F', 'T', 'P', 'R')
#define SPS_ME_SUB_REGION_SIGNATURE_MFS   SIGNATURE_32 ('M', 'F', 'S', '\0')
#define SPS_ME_SUB_REGION_SIGNATURE_OPR1  SIGNATURE_32 ('O', 'P', 'R', '1')
#define SPS_ME_SUB_REGION_SIGNATURE_OPR2  SIGNATURE_32 ('O', 'P', 'R', '2')
#define SPS_ME_SUB_REGION_SIGNATURE_SCA   SIGNATURE_32 ('S', 'C', 'A', '\0')
#define SPS_ME_SUB_REGION_SIGNATURE_ASAP  SIGNATURE_32 ('A', 'S', 'A', 'P')

#define SPS_ME_UPGRADE_CONTEXT_SIGNATURE  SIGNATURE_32 ('S', 'P', 'S', 'U')

///
/// SPS ME upgrade control.
///
typedef struct {
  UINTN                                UpgradeMode;
  UINTN                                UpgradeModeLimit;
  UINT32                               UpgradePhase;
  UINTN                                UpgradeStatus;
  UINTN                                UpgradeTableIndex;  ///< Save current Table index.
  UINTN                                SpiBlockIndex;      ///< Save current SPI block index.

  BOOLEAN                              DualImageMode;      ///< Have OPR2.             Default: TRUE;
  BOOLEAN                              BlockMode;          ///< Is mulit SMI.          Default: FALSE;
  UINT32                               InactiveOPR;        ///< which OPR is inactive. Default: OPR1.
  BOOLEAN                              UpgradeDone;        ///< Upgrade progress done.
  BOOLEAN                              ImageInvalid;       ///< Image is invalid.
} SPS_ME_UPGRADE_FLOW_CONTROL;

typedef struct {
  UINT32                               Signature;
  SPS_ME_NONCE_INFO                    *NonceInfo;
  UINTN                                NumberOfProtectRomMap;
  SPS_ME_PROTECT_ROM_MAP               *ProtectRomMap;
  BOOLEAN                              MeRegionEnabled;
//  UINT8                                PhaseInitDownBits;
  SPS_ME_UPGRADE_FLOW_CONTROL          FlowControl;
  SPS_ME_UPGRADE_PROTOCOL              SpsMeUpgrade;
} SPS_ME_UPGRADE_CONTEXT;

#define SPS_ME_UPGRADE_CONTEXT_FROM_THIS(a) \
  CR (a, SPS_ME_UPGRADE_CONTEXT, SpsMeUpgrade, SPS_ME_UPGRADE_CONTEXT_SIGNATURE)

#pragma pack()

#endif
