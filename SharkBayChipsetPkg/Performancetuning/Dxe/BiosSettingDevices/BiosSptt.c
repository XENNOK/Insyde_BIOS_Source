/** @file

  File for building the Memory $DDD tables necessary to add to the SPTT
  table used for Performance Tuning in XTU

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c) 2008 Intel Corporation. All rights reserved
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#include "BiosSptt.h"

UINTN  Sample;
//[-start-130710-IB05160465-modify]//
UINT8                                  mXECap;
CHIPSET_CONFIGURATION                  *mSystemConfiguration;
//[-end-130710-IB05160465-modify]//
STATIC SPTT_TABLE_SERVICES_PROTOCOL   *mSpttTableServices;

RAW_DDD_LBS_DATA                      RawDiscreteBiosSettingList[] = 
{
  { // BIOS Device: DDR Multiplier 
    PERF_TUNE_BIOS_DDR_MULT_CONTROL,
    AUTOMODE_SUPPORTED,
    0,                                      // DefaultSetting
    {
      {1067, 2, MHz_DATA_TYPE, 1, FLAG_POSITIVE},
      {1333, 2, MHz_DATA_TYPE, 2, FLAG_POSITIVE},
      {1600, 2, MHz_DATA_TYPE, 3, FLAG_POSITIVE},
      {1867, 2, MHz_DATA_TYPE, 4, FLAG_POSITIVE},
      {2133, 2, MHz_DATA_TYPE, 5, FLAG_POSITIVE},
      0xff  // end of item
    },
  },
  { // BIOS Device: Enhanced Intel Speedstep Technology 
    PERF_TUNE_BIOS_EIST_CONTROL,
    AUTOMODE_NOT_SUPPORTED,
    1,                                      // DefaultSetting
    {
      {0, 0, BIOS_Setup_Disable, 0, FLAG_POSITIVE},
      {1, 0, BIOS_Setup_Enable , 1, FLAG_POSITIVE},
      0xff  // end of item
    },
  },
  { // BIOS Device: Turbo Mode Enable 
    PERF_TUNE_BIOS_TURBO_ENABLE_CONTROL,
    AUTOMODE_NOT_SUPPORTED,
    1,                                      // DefaultSetting
    {
      {0, 0, BIOS_Setup_Disable, 0, FLAG_POSITIVE},
      {1, 0, BIOS_Setup_Enable , 1, FLAG_POSITIVE},
      0xff  // end of item
    },
  },
  { // Package Total Design Power Lock Enable/Disable 
    PERF_TUNE_BIOS_PKG_TDP_LOCK_EN_DIS_CONTROL,
    AUTOMODE_NOT_SUPPORTED,
    0,                                      // DefaultSetting
    {
      {0, 0, BIOS_Setup_Disable, 0, FLAG_POSITIVE},
      {1, 0, BIOS_Setup_Enable , 1, FLAG_POSITIVE},
      0xff  // end of item
    },
  },
  { // Short Window Package Total Design Power Enable/Disable 
    PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_EN_DIS_CONTROL,
    AUTOMODE_NOT_SUPPORTED,
    1,                                      // DefaultSetting
    {
      {0, 0, BIOS_Setup_Disable, 0, FLAG_POSITIVE},
      {1, 0, BIOS_Setup_Enable , 1, FLAG_POSITIVE},
      0xff  // end of item
    },
  },
  0xff  // end of array
};

RAW_DDD_LBC_DATA                      RawContinueBiosSettingList[] =
{
  { PERF_TUNE_BIOS_PROC_MULT_CONTROL,   // ImplementType;
    12,                                 // Min;
    20,                                 // Max;
    1,                                  // Step;
    MHz_DATA_TYPE,                      // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                 // MinSettingPerFlag;
    20,                                 // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_HOST_CLOCK_CONTROL,  // ImplementType;
    9900,                               // Min;
    10000,                              // Max;
    10,                                 // Step;
    MHz_DATA_TYPE,                      // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                 // MinSettingPerFlag;
    10000,                              // DefaultSetting;
    2                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TCL_CONTROL,         // ImplementType;
    5,                                  // Min;
    10,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    7,                                  // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TRCD_CONTROL,        // ImplementType;
    1,                                  // Min;
    17,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                 // MinSettingPerFlag;
    7,                                  // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TRP_CONTROL,         // ImplementType;
    1,                                  // Min;
    17,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    7,                                  // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TRAS_CONTROL,        // ImplementType;
    1,                                  // Min;
    42,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    20,                                 // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TWR_CONTROL,         // ImplementType;
    1,                                  // Min;
    17,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    9,                                  // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TRFC_CONTROL,        // ImplementType;
    1,                                  // Min;
    4096,                               // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    59,                                 // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TRRD_CONTROL,        // ImplementType;
    1,                                  // Min;
    17,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    5,                                  // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TWTR_CONTROL,        // ImplementType;
    1,                                  // Min;
    17,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    4,                                  // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TRTP_CONTROL,        // ImplementType;
    1,                                  // Min;
    17,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    4,                                  // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TRC_CONTROL,         // ImplementType;
    1,                                  // Min;
    42,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    28,                                 // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_TFAW_CONTROL,        // ImplementType;
    1,                                  // Min;
    42,                                 // Max;
    1,                                  // Step;
    Clocks_DATA_TYPE,                   // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,             // AutoModeSupport;
    FLAG_MIN_IS_HIGHEST,                // MinSettingPerFlag;
    21,                                 // DefaultSetting;
    0                                   // SettingPrecision;
  },
  { PERF_TUNE_BIOS_EXTEND_WIN_PKG_TDP_LIMIT_CONTROL,        // ImplementType;
    0,                                                      // Min;
    99,                                                     // Max;
    1,                                                      // Step;
    BIOS_Setup_Watts,                                       // DataTypeEnum; 
    AUTOMODE_SUPPORTED,                                     // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                                     // MinSettingPerFlag;
    0,                                                      // DefaultSetting;
    0                                                       // SettingPrecision;
  },
  { PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_LIMIT_CONTROL,         // ImplementType;
    0,                                                      // Min;
    99,                                                     // Max;
    1,                                                      // Step;
    BIOS_Setup_Watts,                                       // DataTypeEnum; 
    AUTOMODE_SUPPORTED,                                     // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                                     // MinSettingPerFlag;
    0,                                                      // DefaultSetting;
    0                                                       // SettingPrecision;
  },
  { PERF_TUNE_BIOS_GFX_TURBO_RATIO_LIMIT_CONTROL,           // ImplementType;
    0,                                                      // Min;
    99,                                                     // Max;
    1,                                                      // Step;
    BIOS_Setup_Watts,                                       // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,                                 // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                                     // MinSettingPerFlag;
    16,                                                     // DefaultSetting;
    0                                                       // SettingPrecision;
  },
  0xff  // end of array
};

RAW_DDD_LBC_DATA                      RawContinueBiosSettingListXE[] =
{
  { PERF_TUNE_BIOS_1_CORE_RATIO_CONTROL,                    // ImplementType;
    0,                                                      // Min;
    99,                                                     // Max;
    1,                                                      // Step;
    BIOS_Setup_Watts,                                       // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,                                 // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                                     // MinSettingPerFlag;
    0,                                                      // DefaultSetting;
    0                                                       // SettingPrecision;
  },
  { PERF_TUNE_BIOS_2_CORE_RATIO_CONTROL,                    // ImplementType;
    0,                                                      // Min;
    99,                                                     // Max;
    1,                                                      // Step;
    BIOS_Setup_Watts,                                       // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,                                 // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                                     // MinSettingPerFlag;
    0,                                                      // DefaultSetting;
    0                                                       // SettingPrecision;
  },
  { PERF_TUNE_BIOS_3_CORE_RATIO_CONTROL,                    // ImplementType;
    0,                                                      // Min;
    99,                                                     // Max;
    1,                                                      // Step;
    BIOS_Setup_Watts,                                       // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,                                 // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                                     // MinSettingPerFlag;
    0,                                                      // DefaultSetting;
    0                                                       // SettingPrecision;
  },
  { PERF_TUNE_BIOS_4_CORE_RATIO_CONTROL,                    // ImplementType;
    0,                                                      // Min;
    99,                                                     // Max;
    1,                                                      // Step;
    BIOS_Setup_Watts,                                       // DataTypeEnum; 
    AUTOMODE_NOT_SUPPORTED,                                 // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                                     // MinSettingPerFlag;
    0,                                                      // DefaultSetting;
    0                                                       // SettingPrecision;
  },
  0xff  // end of array
};

RAW_DDD_HLBC_DATA                     RawHpContinueBiosSettingList[] =
{
  { PERF_TUNE_BIOS_MEM_VOLTAGE_CONTROL,         // ImplementType;
    150,                                        // MinValue;
    165,                                        // MaxValue;
    5,                                          // StepSize;
    2,                                          // SettingPrecision;
    FLAG_POSITIVE,                              // SettingFlags;
    0,                                          // MinData;
    3,                                          // MaxData;
    1,                                          // DataStepSize;
    Volts_DATA_TYPE,                            // DataTypeEnum; 
    0,                                          // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                         // MinMaxPerformance;
    0                                           // DefaultSetting;
  },
  { PERF_TUNE_BIOS_GFX_CORE_VOLTAGE_CONTROL,                // ImplementType;
    0,                                              // MinValue;
    38671875,                                       // MaxValue; (99/256) * 10^8
    390625,                                         // StepSize; (1/256)  * 10^8
    8,                                              // SettingPrecision;
    FLAG_POSITIVE,                                  // SettingFlags;
    0,                                              // MinData;
    99,                                             // MaxData;
    1,                                              // DataStepSize;
    Volts_DATA_TYPE,                                // DataTypeEnum; 
    0,                                              // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                             // MinMaxPerformance;
    0                                               // DefaultSetting;
  },
  { PERF_TUNE_BIOS_IA_CORE_CURRENT_MAX_CONTROL,     // ImplementType;
    0,                                              // MinValue;
    1023875,                                        // MaxValue; (0x1FFF / 8) * 10^3
    125,                                            // StepSize; (1/8)  * 10^3
    3,                                              // SettingPrecision;
    FLAG_POSITIVE,                                  // SettingFlags;
    0,                                              // MinData;
    0x1FFF,                                         // MaxData;  MSR 0x601[12:0]
    1,                                              // DataStepSize;
    Volts_DATA_TYPE,                                // DataTypeEnum; 
    0,                                              // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                             // MinMaxPerformance;
    0                                               // DefaultSetting;
  },
  { PERF_TUNE_BIOS_IGFX_CORE_CURRENT_MAX_CONTROL,   // ImplementType;
    0,                                              // MinValue;
    1023875,                                        // MaxValue; (0x1FFF / 8) * 10^3
    125,                                            // StepSize; (1/8)  * 10^3
    3,                                              // SettingPrecision;
    FLAG_POSITIVE,                                  // SettingFlags;
    0,                                              // MinData;
    0x1FFF,                                         // MaxData;  MSR 0x602[12:0]
    1,                                              // DataStepSize;
    Volts_DATA_TYPE,                                // DataTypeEnum; 
    0,                                              // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                             // MinMaxPerformance;
    0                                               // DefaultSetting;
  },
  0xff  // end of array
};

RAW_DDD_HLBC_DATA                     RawHpContinueBiosSettingListXE[] =
{
  { PERF_TUNE_BIOS_MAX_TURBO_CPU_VOLTAGE_CONTROL,   // ImplementType;
    0,                                              // MinValue;
    38671875,                                       // MaxValue; (99/256) * 10^8
    390625,                                         // StepSize; (1/256)  * 10^8
    8,                                              // SettingPrecision;
    FLAG_POSITIVE,                                  // SettingFlags;
    0,                                              // MinData;
    99,                                             // MaxData;
    1,                                              // DataStepSize;
    Volts_DATA_TYPE,                                // DataTypeEnum; 
    0,                                              // AutoModeSupport;
    FLAG_MIN_IS_LOWEST,                             // MinMaxPerformance;
    0                                               // DefaultSetting;
  },
  0xff  // end of array
};


STATIC SPTT_DATA_PROTOCOL           mSpttDataProtocol = {
   GetBiosSettingData
};

/**
  
  Entry point and initialization function for the Voltage Override component.  This installs the SPTT Data
  protocol for each independently controllable voltage in the system.

  @param  ImageHandle           Not Used
  @param  SystemTable            Not Used

  @retval EFI_SUCCESS            Initialization completed successfully
  @retval All others                   Failure to initialize the Tuning Core correctly

**/
EFI_STATUS
BiosSpttInit (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
   EFI_STATUS                Status;
   EFI_HANDLE                ProtocolHandle = NULL;
   UINTN                     VarSize;

  //
  // Locate SpttTableServices
  //
  Status = gBS->LocateProtocol (
                  &gSpttTableServicesProtocolGuid,
                  NULL,
                  (VOID **)&mSpttTableServices
                  );

//[-start-130709-IB05160465-modify]//
  VarSize = PcdGet32 (PcdSetupConfigSize);
  mSystemConfiguration = AllocateZeroPool (VarSize);

  gRT->GetVariable (L"Setup",
         &gSystemConfigurationGuid,
         NULL,
         &VarSize,
         mSystemConfiguration
         );

  mXECap = mSystemConfiguration->XECap;
  FreePool (mSystemConfiguration);
//[-end-130709-IB05160465-modify]//

   //
   // Install protocol
   //
   Status = gBS->InstallProtocolInterface(
                                         &ProtocolHandle,
                                         &gSpttDataProtocolGuid,
                                         EFI_NATIVE_INTERFACE,
                                         &mSpttDataProtocol
                                         );


   return Status;
}

EFI_STATUS
GetBiosSettingData(
   UINT8                         **outputTable,
   UINTN                         *outputTableSize
   )
{
   UINT8                             *DataPtr;
   UINTN                             TotalLen = 0;
   UINT8                             *Buffer;
   CHAR16                            *String;
   CHAR16                            *UqiString;
   UINTN                             StringLength;
   UINTN                             UqiStringLength;
   UINTN                             DefBufferSize = DEFAULT_BUFFER_SIZE;
   UINT8                             *TableEntry;
   UINTN                             TableSize;
   UINT16                            XeModeCap = 0;
   UINT16                            TurboModeCap = 0;
   UINT64                            MsrValue;
   EFI_CPUID_REGISTER                CpuidRegisters;
   UINTN                             Index;
   VOID                              *HobList;
   XTU_INFO_DATA                     *XTUInfoHobData;


   //
   // Get CPU Capabilities Info
   //
   Sample = 0;

   // Read PLATFORM_INFO MSR
   MsrValue = AsmReadMsr64 (0x000000CE);
   // If BIT29 or BIT28 is set, processor is XE capable (Nehalem BWG)
   if ((MsrValue & (BIT29 | BIT28)) != 0)
   {
     DEBUG ((EFI_D_ERROR, " XeModeCap = 1 \n"));
     XeModeCap = 1;
   }
   if (MsrValue & BIT27) {
     //
     // This is a pre-production CPU.
     //
     DEBUG ((EFI_D_ERROR, " This is a pre-production CPU. \n"));
     Sample = 1;
   }
   // Read PLATFORM_INFO MSR
   MsrValue = AsmReadMsr64 (0x000001A0);
   // If MSR 1A0 BIT38 Does Not equal CPUID 06 then processor is capable of Turbo Mode
   AsmCpuid (
     6, 
     &CpuidRegisters.RegEax,
     &CpuidRegisters.RegEbx,
     &CpuidRegisters.RegEcx,
     &CpuidRegisters.RegEdx
     );
   if ( ((CpuidRegisters.RegEax & 0x02) && (!(MsrValue & (BIT38)))) ||
        ((!(CpuidRegisters.RegEax & 0x02)) && (MsrValue & (BIT38))) )
   {
     DEBUG ((EFI_D_ERROR, " TurboModeCap = 1. \n"));
     TurboModeCap = 1;
   }


   Buffer = NULL;

   *outputTable = AllocateZeroPool(DefBufferSize);
   DataPtr = *outputTable;

   StringLength  = 0x1000;
   String        = AllocateZeroPool (StringLength);
   UqiStringLength = 0x0004;
   UqiString     = AllocateZeroPool (UqiStringLength);
   ASSERT (String != NULL);

  //
  // Get XTUInfoHob, some bios devices' setting range are depend on CPU
  //
  HobList = GetHobList ();
  XTUInfoHobData = GetNextGuidHob (&gXTUInfoHobGuid, HobList);
  ASSERT_EFI_ERROR (XTUInfoHobData == NULL);

//[-start-140210-IB05400512-add]//
  if (XTUInfoHobData == NULL) {
    return EFI_NOT_FOUND;
  }
  XTUInfoHobData = (XTU_INFO_DATA*)GET_GUID_HOB_DATA (XTUInfoHobData);
//[-end-140210-IB05400512-add]//

  //
  // create $LBS : Discrete Bios Settings
  //
  mSpttTableServices->BuildDiscreteBiosSettingTable (
                        RawDiscreteBiosSettingList,
                        &TableEntry,
                        &TableSize
                        );
  TotalLen += TableSize;
  CopyMem(DataPtr, TableEntry, TableSize);
  DataPtr += TableSize;
  gBS->FreePool(TableEntry);

  
  //
  // create $LBC : Continue Bios Settings
  //
  for (Index = 0; RawContinueBiosSettingList[Index].ImplementType != 0xff; Index++) {
    //
    // Update some item's value (dynamical)
    //
    switch (RawContinueBiosSettingList[Index].ImplementType) {

      case PERF_TUNE_BIOS_PROC_MULT_CONTROL:
        RawContinueBiosSettingList[Index].Min            = XTUInfoHobData->CpuNonTurboRatio.MinRatio;
        RawContinueBiosSettingList[Index].Max            = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
        RawContinueBiosSettingList[Index].DefaultSetting = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
        break;

      case PERF_TUNE_BIOS_EXTEND_WIN_PKG_TDP_LIMIT_CONTROL:
        RawContinueBiosSettingList[Index].DefaultSetting = XTUInfoHobData->TruboTdpLimitDefault.PowerLimit1Default;
        break;

      case PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_LIMIT_CONTROL:
        RawContinueBiosSettingList[Index].DefaultSetting = XTUInfoHobData->TruboTdpLimitDefault.PowerLimit2Default;
        break;

      case PERF_TUNE_BIOS_HOST_CLOCK_CONTROL:
        RawContinueBiosSettingList[Index].Min            = XTUInfoHobData->HostClockData.MinSupportFreq;
        RawContinueBiosSettingList[Index].Max            = XTUInfoHobData->HostClockData.MaxSupportFreq;
        break;
    }
  }
  mSpttTableServices->BuildContinuousBiosSettingTable (
                        RawContinueBiosSettingList,
                        &TableEntry,
                        &TableSize
                        );
  TotalLen += TableSize;
  CopyMem(DataPtr, TableEntry, TableSize);
  DataPtr += TableSize;
  gBS->FreePool(TableEntry);

  //
  // create $LBC : Continue Bios Settings (depend on XE Capability)
  //
//[-start-130710-IB05160465-modify]//
  if (mXECap) {
//[-end-130710-IB05160465-modify]//
    for (Index = 0; RawContinueBiosSettingListXE[Index].ImplementType != 0xff; Index++) {
      //
      // Update some item's value (dynamical)
      //
      switch (RawContinueBiosSettingListXE[Index].ImplementType) {

        case PERF_TUNE_BIOS_1_CORE_RATIO_CONTROL:
          RawContinueBiosSettingListXE[Index].DefaultSetting = XTUInfoHobData->TurboRatioLimitDefault.LimitCore1;
          RawContinueBiosSettingListXE[Index].Min            = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
          break;

        case PERF_TUNE_BIOS_2_CORE_RATIO_CONTROL:
          RawContinueBiosSettingListXE[Index].DefaultSetting = XTUInfoHobData->TurboRatioLimitDefault.LimitCore2;
          RawContinueBiosSettingListXE[Index].Min            = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
          break;

        case PERF_TUNE_BIOS_3_CORE_RATIO_CONTROL:
          RawContinueBiosSettingListXE[Index].DefaultSetting = XTUInfoHobData->TurboRatioLimitDefault.LimitCore3;
          RawContinueBiosSettingListXE[Index].Min            = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
          break;

        case PERF_TUNE_BIOS_4_CORE_RATIO_CONTROL:
          RawContinueBiosSettingListXE[Index].DefaultSetting = XTUInfoHobData->TurboRatioLimitDefault.LimitCore4;
          RawContinueBiosSettingListXE[Index].Min            = XTUInfoHobData->CpuNonTurboRatio.MaxRatio;
          break;
      }
    }
    mSpttTableServices->BuildContinuousBiosSettingTable (
                          RawContinueBiosSettingListXE,
                          &TableEntry,
                          &TableSize
                          );
    TotalLen += TableSize;
    CopyMem(DataPtr, TableEntry, TableSize);
    DataPtr += TableSize;
    gBS->FreePool(TableEntry);
  }

  //
  // create HLBC : High Precision Continue Bios Settings
  //
  mSpttTableServices->BuildHpContinuousBiosSettingTable (
                        RawHpContinueBiosSettingList,
                        &TableEntry,
                        &TableSize
                        );
  TotalLen += TableSize;
  CopyMem(DataPtr, TableEntry, TableSize);
  DataPtr += TableSize;
  gBS->FreePool(TableEntry);

  //
  // create HLBC : High Precision Continue Bios Settings (depend on XE Capability)
  //
//[-start-130710-IB05160465-modify]//
  if (mXECap) {
//[-end-130710-IB05160465-modify]//
    mSpttTableServices->BuildHpContinuousBiosSettingTable (
                          RawHpContinueBiosSettingListXE,
                          &TableEntry,
                          &TableSize
                          );
    TotalLen += TableSize;
    CopyMem(DataPtr, TableEntry, TableSize);
    DataPtr += TableSize;
    gBS->FreePool(TableEntry);
  }


  *outputTableSize = TotalLen;

  gBS->FreePool(String);
  gBS->FreePool(UqiString);
  return EFI_SUCCESS;
}

BOOLEAN
ValidMemMultEntry(
  UINT16 XeModeCap,
  UINT16 TurboModeCap,
  UINT8  *DataBuffer,
  UINTN  Idx
  )
{
  BOOLEAN CorrectEntry = FALSE;
  BOOLEAN NotOpFound = FALSE;
  //
  // Check to see if this opcode has an EFI_IFR_NOT_OP in the raw data before the EFI_IFR_SUPPRESS_IF_OP opcode
  //  If it is there then this is the data used in the NON-XE memory timing path.  Otherwise it is the data
  //  for the XE and production Non-XE memory timing path.
  //
  while ((DataBuffer[Idx] != EFI_IFR_SUPPRESS_IF_OP) && (NotOpFound == FALSE)) {
    if (DataBuffer[Idx] == EFI_IFR_NOT_OP) {
      NotOpFound = TRUE;
    }
    Idx--;
  }
  //
  // Now match the XE Capabilities with the existance of the EFI_IFR_NOT_OP.
  //
  if ((XeModeCap || (!XeModeCap && !Sample)) && (NotOpFound == FALSE)) {
    CorrectEntry = TRUE;
  }else if ((!XeModeCap) && (NotOpFound == TRUE)) {
    CorrectEntry = TRUE;
  }
  return CorrectEntry;
}


