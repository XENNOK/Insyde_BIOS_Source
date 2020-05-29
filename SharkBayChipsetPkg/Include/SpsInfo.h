/** @file

  This file contains definitions required for creation of SPS Info.

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

#ifndef _SPS_INFO_H_
#define _SPS_INFO_H_

typedef struct {
  UINT8            Major;
  UINT8            Minor;
} SPS_INFO_ME_BIOS_INTERFACE_VERSION;

typedef enum {
  SPS_SKU_SI,
  SPS_SKU_NM,
} SPS_SKU;

typedef struct {
  UINT16           FwVerMinor;
  UINT16           FwVerMajor;
  UINT16           FwVerBuild;
  UINT16           FwVerPatch;
} SPS_INFO_FW_VERSION;

typedef enum {
  SpsInfomPhyVersion,
  SpsInfoMeSku,
  SpsInfoFwVersion,
  SpsInfoMeBiosIntVer,
  SpsInfoMeDisable,
  SpsInfoMeTimeout,
  SpsInfoMeCompatible,
  SpsInfoMeInRecoveru,
  SpsInfoBootingMode,
  SpsInfoCoreDisable,

  SpsInfoMax
} SPS_HOB_TYPE;

typedef struct _SPS_INFO {
  SPS_SKU                              SpsMeSku;         /// SPS ME SKU

  SPS_INFO_FW_VERSION                  SpsFwVersion;     /// SPS ME FW Version
  SPS_INFO_ME_BIOS_INTERFACE_VERSION   SpsMeBiosIntfVer; /// SPS ME-BIOS Interface Version
  UINT16                               SpsmPhyVersion;   /// SPS mPhy survivability table version

  BOOLEAN                              SpsMeDisabled;    /// SPS ME Disable
  BOOLEAN                              SpsMeTimeout;     /// SPS ME Timeout in PEI Phase
  BOOLEAN                              SpsMeBiosCompat;  /// SPS ME BIOS compatibility?
  BOOLEAN                              SpsMeInRecovery;  /// SPS ME in Recovery mode.
  BOOLEAN                              SpsMeDualImage;  /// SPS ME is daul image mode.

  UINT8                                BootingMode;      /// SPS NM Booting mode request
  UINT8                                CoreDisable;      /// SPS NM cores disable request
};

#endif
