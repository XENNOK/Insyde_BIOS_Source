/** @file
  Header file of CPU feature control module

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _FEATURES_H_
#define _FEATURES_H_

#include "CpuInitDxe.h"
#include "MpService.h"

#define OPTION_FEATURE_RESERVED_MASK        0xFFFF00F8  ///< bits 30:16,  7:3
#define OPTION_FEATURE_CONFIG_RESERVED_MASK 0xFFFFFFFC  ///< bits 2:31
/**
  Create feature control structure which will be used to program each feature on each core.

  @param[in] MPSystemData - MP_SYSTEM_DATA global variable that contains platform policy protocol settings of each features.
**/
VOID
InitializeFeaturePerSetup (
  IN MP_SYSTEM_DATA *MPSystemData
  );

/**
  Program all processor features basing on desired settings

  @param[in] MpServices  - EFI_MP_SERVICES_PROTOCOL
**/
VOID
ProgramProcessorFeature (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL             *MpServices
  );

/**
  Program CPUID Limit before booting to OS

  @param[in] MpServices  - MP Services Protocol entry
**/
VOID
ProgramCpuidLimit (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL             *MpServices
  );

/**
  Initialize prefetcher settings

  @param[in] MlcStreamerprefecterEnabled - Enable/Disable MLC streamer prefetcher
  @param[in] MlcSpatialPrefetcherEnabled - Enable/Disable MLC spatial prefetcher
//[-start-130906-IB08620308-add]//
  @param[in] DcuStreamerPrefetcher       - Enable/Disable DCU Streamer Prefetcher
  @param[in] DcuIPPrefetcher             - Enable/Disable DCU IP Prefetcher
//[-end-130906-IB08620308-add]//
**/
VOID
InitializeProcessorsPrefetcher (
  IN UINTN MlcStreamerprefecterEnabled,
  IN UINTN MlcSpatialPrefetcherEnabled
//[-start-130906-IB08620308-add]//
  ,
  IN UINTN DcuStreamerPrefetcher,
  IN UINTN DcuIPPrefetcher
//[-end-130906-IB08620308-add]//
  );

/**
  Detect each processor feature and log all supported features

  @param[in] MpServices  - EFI_MP_SERVICES_PROTOCOL
**/
VOID
CollectProcessorFeature (
  IN  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL             *MpServices
  );

/**
  Lock VMX/TXT feature bits on the processor.
  Set "CFG Lock" (MSR 0E2h Bit[15]

  @param[in] LockFeatureEnable - TRUE to lock these feature bits and FALSE to not lock
**/
VOID
LockFeatureBit (
  IN BOOLEAN LockFeatureEnable
  );

/**
  Function to get desired core number by CSR register

  @retval Desired core number
**/
UINT32
GetCsrDesiredCores (
  VOID
  );

/**
  Function to set desired core numbers or SMT lock
**/
VOID
SetLockCsrDesiredCores (
  VOID
  );

/**
  Write 64bits MSR with script

  @param[in] Index - MSR index that will be written
  @param[in] Value - value written to MSR
**/
VOID
AsmWriteMsr64WithScript (
  IN UINT32 Index,
  IN UINT64 Value
  );

/**
  Write 64bits MSR to script

  @param[in] Index - MSR index that will be written
  @param[in] Value - value written to MSR
**/
VOID
WriteMsr64ToScript (
  IN UINT32 Index,
  IN UINT64 Value
  );

/**
  Provide access to the CPU misc enables MSR

  @param[in] Enable  - Enable or Disable Misc Features
  @param[in] BitMask - The register bit offset of MSR MSR_IA32_MISC_ENABLE
**/
VOID
CpuMiscEnable (
  BOOLEAN Enable,
  UINT64  BitMask
  );

#endif
