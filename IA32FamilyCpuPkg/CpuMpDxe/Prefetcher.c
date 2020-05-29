/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for Prefetcher features

  Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  Prefetcher.c

**/

#include "Prefetcher.h"

BOOLEAN    EnableHardwarePrefetcher[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableAdjacentCacheLinePrefetch[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableDcuPrefetcher[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableIpPrefetcher[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableMlcStreamerPrefetcher[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];
BOOLEAN    EnableMlcSpatialPrefetcher[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of Hardware Prefetcher for specified processor.
  
  This function detects capability of Hardware Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
HardwarePrefetcherDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  //
  // Hardware Prefetcher is supported by Prescott family processors, Cedar Mill family processors,
  // and Core 2 family processors with stepping of B0 and above
  //
  if ((FamilyId == 0x06 && ((ModelId == 0x0F && SteppingId >= 0x04) || ModelId == 0x16 || ModelId == 0x17)) ||
      (FamilyId == 0x0F && (ModelId == 0x03 || ModelId == 0x04 || ModelId == 0x06))) {

    SetProcessorFeatureCapability (ProcessorNumber, HardwarePrefetcher, NULL);
  }
}

/**
  Configures Processor Feature Lists for Hardware Prefetcher for all processors.
  
  This function configures Processor Feature Lists for Hardware Prefetcher for all processors.

**/
VOID
HardwarePrefetcherConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_HARDWARE_PREFETCHER_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports Hardware Prefetcher
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, HardwarePrefetcher, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of Hardware Prefetcher capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_HARDWARE_PREFETCHER_BIT);
        //
        // Set the bit of Hardware Prefetcher setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_HARDWARE_PREFETCHER_BIT);
        }
      }

      //
      // If this logical processor supports Hardware Prefetcher, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableHardwarePrefetcher[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, HardwarePrefetcher, &(EnableHardwarePrefetcher[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of HardwarePrefetcher in Register Table for specified processor.
  
  This function produces entry of Hardware Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
HardwarePrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  //
  // If Attribute is TRUE, then write 0 to MSR_IA32_MISC_ENABLE[9].
  // Otherwise, write 1 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 1;
  if (*Enable) {
    Value = 0;
  }

  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 0);
  WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_HW_PREFETCHER_DISABLE, 1, Value);
  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 1);
}

/**
  Detect capability of Adjacent Cache Line Prefetch for specified processor.
  
  This function detects capability of Adjacent Cache Line Prefetch for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
AdjacentCacheLinePrefetchDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  //
  // Adjacent Cache Line Prefetch is supported by Prescott family processors, Cedar Mill family processors,
  // and Core 2 family processors with stepping of B0 and above
  //
  if ((FamilyId == 0x06 && ((ModelId == 0x0F && SteppingId >= 0x04) || ModelId == 0x16 || ModelId == 0x17)) ||
      (FamilyId == 0x0F && (ModelId == 0x03 || ModelId == 0x04 || ModelId == 0x06))) {

    SetProcessorFeatureCapability (ProcessorNumber, AdjacentCacheLinePrefetch, NULL);
  }
}

/**
  Configures Processor Feature Lists for Adjacent Cache Line Prefetch for all processors.
  
  This function configures Processor Feature Lists for Adjacent Cache Line Prefetch for all processors.

**/
VOID
AdjacentCacheLinePrefetchConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_ADJACENT_CACHE_LINE_PREFETCH_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports Adjacent Cache Line Prefetch
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, AdjacentCacheLinePrefetch, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of Adjacent Cache Line Prefetch capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_ADJACENT_CACHE_LINE_PREFETCH_BIT);
        //
        // Set the bit of Adjacent Cache Line Prefetch setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_ADJACENT_CACHE_LINE_PREFETCH_BIT);
        }
      }

      //
      // If this logical processor supports Adjacent Cache Line Prefetch, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableAdjacentCacheLinePrefetch[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, AdjacentCacheLinePrefetch, &(EnableAdjacentCacheLinePrefetch[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of Adjacent Cache Line Prefetch in Register Table for specified processor.
  
  This function produces entry of Adjacent Cache Line Prefetch in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
AdjacentCacheLinePrefetchReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  //
  // If Attribute is TRUE, then write 0 to MSR_IA32_MISC_ENABLE[19].
  // Otherwise, write 1 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 1;
  if (*Enable) {
    Value = 0;
  }

  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 0);
  WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_ADJACENT_CACHE_LINE_PREFETCH_DISABLE, 1, Value);
  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 1);
}

/**
  Detect capability of DCU Prefetcher for specified processor.
  
  This function detects capability of DCU Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
DcuPrefetcherDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  //
  // DCU Prefetcher is supported by Core 2 family processors with stepping of B0 and above
  //
  if (FamilyId == 0x06 && ((ModelId == 0x0F && SteppingId >= 0x04) || ModelId == 0x16 || ModelId == 0x17)) {
    SetProcessorFeatureCapability (ProcessorNumber, DcuPrefetcher, NULL);
  } else if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    //
    // DCU Stream Prefetcher for NHM, SNB, IVB, HSW processors
    //
    SetProcessorFeatureCapability (ProcessorNumber, DcuPrefetcher, NULL);
  }
}

/**
  Configures Processor Feature Lists for DCU Prefetcher for all processors.
  
  This function configures Processor Feature Lists for DCU Prefetcher for all processors.

**/
VOID
DcuPrefetcherConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_DCU_PREFETCHER_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports DCU Prefetcher
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, DcuPrefetcher, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of DCU Prefetcher capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_DCU_PREFETCHER_BIT);
        //
        // Set the bit of DCU Prefetcher setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_DCU_PREFETCHER_BIT);
        }
      }

      //
      // If this logical processor supports DCU Prefetcher, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableDcuPrefetcher[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, DcuPrefetcher, &(EnableDcuPrefetcher[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of DCU Prefetcher in Register Table for specified processor.
  
  This function produces entry of DCU Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
DcuPrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;
  UINT32     FamilyId;
  UINT32     ModelId;
  UINT32     SteppingId;

  //
  // If Attribute is TRUE, then write 0 to MSR_IA32_MISC_ENABLE[37].
  // Otherwise, write 1 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 1;
  if (*Enable) {
    Value = 0;
  }

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 0);
  if (FamilyId == 0x06 && ((ModelId == 0x0F && SteppingId >= 0x04) || ModelId == 0x16 || ModelId == 0x17)) {
    WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_DCU_PREFETCHER_DISABLE, 1, Value);
  } else if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    //
    // DCU Stream Prefetcher for NHM, SNB, IVB, HSW processors
    //
    WriteRegisterTable (ProcessorNumber, Msr, MSR_MISC_FEATURE_CONTROL, N_MSR_DCU_STREAMER_PREFETCHER_DISABLE, 1, Value);
  }
  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 1);
}

/**
  Detect capability of IP Prefetcher for specified processor.
  
  This function detects capability of IP Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
IpPrefetcherDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  //
  // IP Prefetcher is supported by Core 2 family processors with stepping of B0 and above
  //
  if (FamilyId == 0x06 && ((ModelId == 0x0F && SteppingId >= 0x04) || ModelId == 0x16 || ModelId == 0x17)) {

    SetProcessorFeatureCapability (ProcessorNumber, IpPrefetcher, NULL);
  } else if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    //
    // DCU IP Prefetcher for NHM, SNB, IVB, HSW processors
    //
    SetProcessorFeatureCapability (ProcessorNumber, IpPrefetcher, NULL);
  }
}

/**
  Configures Processor Feature Lists for IP Prefetcher for all processors.
  
  This function configures Processor Feature Lists for IP Prefetcher for all processors.

**/
VOID
IpPrefetcherConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_IP_PREFETCHER_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports IP Prefetcher
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, IpPrefetcher, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of IP Prefetcher according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_IP_PREFETCHER_BIT);
        //
        // Set the bit of IP Prefetcher setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_IP_PREFETCHER_BIT);
        }
      }

      //
      // If this logical processor supports IP Prefetcher, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableIpPrefetcher[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, IpPrefetcher, &(EnableIpPrefetcher[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of IP Prefetcher in Register Table for specified processor.
  
  This function produces entry of IP Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
IpPrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;
  UINT32     FamilyId;
  UINT32     ModelId;
  UINT32     SteppingId;

  //
  // If Attribute is TRUE, then write 0 to MSR_IA32_MISC_ENABLE[39].
  // Otherwise, write 1 to the bit.
  //
  Enable = (BOOLEAN *) Attribute;
  Value  = 1;
  if (*Enable) {
    Value = 0;
  }

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 0);
  if (FamilyId == 0x06 && ((ModelId == 0x0F && SteppingId >= 0x04) || ModelId == 0x16 || ModelId == 0x17)) {
    WriteRegisterTable (ProcessorNumber, Msr, MSR_IA32_MISC_ENABLE, N_MSR_IP_PREFETCHER_DISABLE, 1, Value);
  } else if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
             (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    //
    // DCU IP Prefetcher for NHM, SNB, IVB, HSW processors
    //
    WriteRegisterTable (ProcessorNumber, Msr, MSR_MISC_FEATURE_CONTROL, N_MSR_DCU_IP_PREFETCHER_DISABLE, 1, Value);
  }
  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 1);
}

/**
  Detect capability of MLC Streamer Prefetcher for specified processor.
  
  This function detects capability of MLC Streamer Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MlcStreamerPrefetcherDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, MlcStreamerPrefetcher, NULL);
  }
}

/**
  Configures Processor Feature Lists for MLC Streamer Prefetcher for all processors.
  
  This function configures Processor Feature Lists for MLC Streamer Prefetcher for all processors.

**/
VOID
MlcStreamerPrefetcherConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_MLC_STREAMER_PREFETCHER_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports MLC StreamerPrefetcher
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, MlcStreamerPrefetcher, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of MLC Streamer Prefetcher according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_MLC_STREAMER_PREFETCHER_BIT);
        //
        // Set the bit of MLC Streamer Prefetcher setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_MLC_STREAMER_PREFETCHER_BIT);
        }
      }

      //
      // If this logical processor supports MLC Streamer Prefetcher, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableMlcStreamerPrefetcher[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, MlcStreamerPrefetcher, &(EnableMlcStreamerPrefetcher[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of MLC Streamer Prefetcher in Register Table for specified processor.
  
  This function produces entry of MLC Streamer Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
MlcStreamerPrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  Enable = (BOOLEAN *) Attribute;
  Value  = 1;
  if (*Enable) {
    Value = 0;
  }

  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 0);
  WriteRegisterTable (ProcessorNumber, Msr, MSR_MISC_FEATURE_CONTROL, N_MSR_MLC_STREAMER_PREFETCHER_DISABLE, 1, Value);
  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 1);
}

/**
  Detect capability of MLC Spatial Prefetcher for specified processor.
  
  This function detects capability of MLC Spatial Prefetcher for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MlcSpatialPrefetcherDetect (
  UINTN   ProcessorNumber
  )
{
  UINT32    FamilyId;
  UINT32    ModelId;
  UINT32    SteppingId;

  GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

  if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuHaswellFamilyFlag) && IS_HASWELL_PROC (FamilyId, ModelId))) {
    SetProcessorFeatureCapability (ProcessorNumber, MlcSpatialPrefetcher, NULL);
  }
}

/**
  Configures Processor Feature Lists for MLC Spatial Prefetcher for all processors.
  
  This function configures Processor Feature Lists for MLC Spatial Prefetcher for all processors.

**/
VOID
MlcSpatialPrefetcherConfigFeatureList (
  VOID
  )
{
  UINTN                 ProcessorNumber;
  BOOLEAN               UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_MLC_SPATIAL_PREFETCHER_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports MLC Streamer Prefetcher
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, MlcSpatialPrefetcher, NULL)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of MLC Streamer Prefetcher according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_MLC_SPATIAL_PREFETCHER_BIT);
        //
        // Set the bit of MLC Streamer Prefetcher setting according to BSP setting.
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_MLC_SPATIAL_PREFETCHER_BIT);
        }
      }

      //
      // If this logical processor supports MLC Streamer Prefetcher, then add feature into feature list for operation
      // on corresponding register.
      //
      EnableMlcSpatialPrefetcher[ProcessorNumber] = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, MlcSpatialPrefetcher, &(EnableMlcSpatialPrefetcher[ProcessorNumber]));
    }
  }
}

/**
  Produces entry of MLC Spatial Prefetcher in Register Table for specified processor.
  
  This function produces entry of MLC Spatial Prefetcher in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Pointer to the attribute

**/
VOID
MlcSpatialPrefetcherReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  BOOLEAN    *Enable;
  UINT64     Value;

  Enable = (BOOLEAN *) Attribute;
  Value  = 1;
  if (*Enable) {
    Value = 0;
  }

  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 0);
  WriteRegisterTable (ProcessorNumber, Msr, MSR_MISC_FEATURE_CONTROL, N_MSR_MLC_SPATIAL_PREFETCHER_DISABLE, 1, Value);
  WriteRegisterTable (ProcessorNumber, CacheControl, 0, 0, 0, 1);
}

