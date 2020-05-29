/** @file

  This file contains the SMI handler for the XTU specific SMI Commands
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
#include "PerfTuneSmm.h"
#include <Library/SmmServicesTableLib.h>

UINT8     mXtuRuntimeFlag;
UINT16    mXeModeCap = 0;
UINT16    mTurboModeCap = 0;
UINT64    mPowerDivisor;
UINT32    mOldSig = SIGNATURE_32('$', 'B', 'D', '$');
UINT32    mCurSig = SIGNATURE_32('$', 'B', 'D', '2');
UINT32    mDbgSig = SIGNATURE_32('$', 'D', 'B', 'G');

EFI_GUID  mEfiGlobalVariableGuidLocal = EFI_GENERIC_VARIABLE_GUID;
EFI_GUID  mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

EFI_SMM_SW_DISPATCH_PROTOCOL            *mSwDispatch;
EFI_SMM_VARIABLE_PROTOCOL               *mSmmVariable;
EFI_HANDLE                              SwHandle = 0;
EFI_SMM_SW_DISPATCH_CONTEXT             SwContext = { 0};
//[-start-130710-IB05160465-modify]//
CHIPSET_CONFIGURATION                   *mSystemConfiguration;
//[-end-130710-IB05160465-modify]//
EFI_SMM_CPU_SAVE_STATE                  *mSaveState;
BIOS_SETTINGS_DATA_STRUCT               mBiosSettingsData;

//[-start-120217-IB03780424-remove]//
//UINT8                                   mMemoryFrequency;
//[-end-120217-IB03780424-remove]//

//[-start-120224-IB03780425-remove]//
//UINT8                                   mCurrentProfile;
//[-end-120224-IB03780425-remove]//

BIOS_SETTING_ENTRY                      mImplementedBiosSettings [] = {
    { PERF_TUNE_BIOS_PROC_MULT_CONTROL               , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_HOST_CLOCK_CONTROL              , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_MEM_VOLTAGE_CONTROL             , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TCL_CONTROL                     , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TRCD_CONTROL                    , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TRP_CONTROL                     , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TRAS_CONTROL                    , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TWR_CONTROL                     , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_DDR_MULT_CONTROL                , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TRFC_CONTROL                    , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TRRD_CONTROL                    , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TWTR_CONTROL                    , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_NMODE_CONTROL                   , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TRTP_CONTROL                    , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TURBO_ENABLE_CONTROL            , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_EIST_CONTROL                    , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_TFAW_CONTROL                    , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_LIMIT_CONTROL , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_EXTEND_WIN_PKG_TDP_LIMIT_CONTROL, BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_EN_DIS_CONTROL, BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_PKG_TDP_LOCK_EN_DIS_CONTROL     , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_IA_CORE_CURRENT_MAX_CONTROL     , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_IGFX_CORE_CURRENT_MAX_CONTROL   , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_GFX_TURBO_RATIO_LIMIT_CONTROL   , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_GFX_CORE_VOLTAGE_CONTROL        , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_XMP_CONTROL                     , BIOS_DEVICE_DISABLE },
    0xff // end of array
};

BIOS_SETTING_ENTRY                      mImplementedBiosSettingsXE [] = {
    { PERF_TUNE_BIOS_1_CORE_RATIO_CONTROL            , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_2_CORE_RATIO_CONTROL            , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_3_CORE_RATIO_CONTROL            , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_4_CORE_RATIO_CONTROL            , BIOS_DEVICE_DISABLE },
    { PERF_TUNE_BIOS_MAX_TURBO_CPU_VOLTAGE_CONTROL   , BIOS_DEVICE_DISABLE },
    0xff // end of array
};

/**
  
  Initializes the SMM Handler Driver
  
  @param  ImageHandle              
  @param  SystemTable             

  @retval EFI_SUCCESS             The function completed successfully.
  @retval !EFI_SUCCESS            The PEIM failed to execute.

**/
EFI_STATUS
InitializePerfTuneServices (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
   EFI_STATUS                      Status;
   UINT64                          MsrValue;
   EFI_CPUID_REGISTER              CpuidRegisters;
   DXE_XTU_POLICY_PROTOCOL         *XtuPlatformPolicyInstance;
   EFI_SETUP_UTILITY_PROTOCOL      *SetupUtility;
   CHIPSET_CONFIGURATION            *SystemConfiguration;
//[-start-120217-IB03780424-remove]//
//   MEM_INFO_PROTOCOL               *MemInfoHob;
//   EFI_GUID                        MemInfoProtocolGuid = MEM_INFO_PROTOCOL_GUID;
//[-end-120217-IB03780424-remove]//

  //
  // Clear XTU Posting flag, Set B_XTU_FLAG_SAFE_RECOVERY to 1, or XtuPei will detect as CMOS clear
  //
  WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus, (ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus) & ~B_XTU_FLAG_POSTING) | B_XTU_FLAG_SAFE_RECOVERY);
  //
  // Clear XTU crash counter
  //
  WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuCrashCounter, 0);

//[-start-130710-IB05160465-add]//
  mSystemConfiguration = AllocateZeroPool (PcdGet32 (PcdSetupConfigSize));
//[-end-130710-IB05160465-add]//

  //
  // supported only when Enable OverClocking
  //
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  SystemConfiguration = (CHIPSET_CONFIGURATION *)SetupUtility->SetupNvData;
  if (EFI_ERROR(Status) || !SystemConfiguration->OverClocking) {
    return EFI_UNSUPPORTED;
  }
  
  Status = gBS->LocateProtocol (&gDxePlatformXtuPolicyGuid, NULL, (VOID **)&XtuPlatformPolicyInstance);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (! XtuPlatformPolicyInstance->EnableSwSmiRealtimeCommunicationsInterface) {
    return EFI_ABORTED;
  }

//[-start-120224-IB03780425-remove]//
//  mCurrentProfile = SystemConfiguration->XmpProfileSetting;
//[-end-120224-IB03780425-remove]//

//[-start-120217-IB03780424-remove]//
//  Status = gBS->LocateProtocol (&MemInfoProtocolGuid, NULL, &MemInfoHob);
//  if (EFI_ERROR (Status)) {
//    return Status;
//  }
//
//  switch (MemInfoHob->MemInfoData.ddrFreq) {
//    case 1067:
//      mMemoryFrequency = 4;
//      break;
//
//    case 1333:
//      mMemoryFrequency = 5;
//      break;
//
//    case 1600:
//      mMemoryFrequency = 6;
//      break;
//
//    case 1867:
//      mMemoryFrequency = 7;
//      break;
//
//    case 2133:
//      mMemoryFrequency = 8;
//      break;
//
//    default:
//      mMemoryFrequency = 5;
//  }
//[-end-120217-IB03780424-remove]//

  MsrValue = AsmReadMsr64 (EFI_MSR_PKG_POWER_SKU_UNIT);
  mPowerDivisor = LShiftU64(1, MsrValue & B_EFI_MSR_PKG_POWER_UNIT); 

  // Locate SwDispatch protocol
  Status = gBS->LocateProtocol(&gEfiSmmSwDispatchProtocolGuid, NULL, (VOID **)&mSwDispatch);
  if (EFI_ERROR(Status)) {
     DEBUG(( EFI_D_ERROR, "Couldn't find SmmSwDispatch protocol: %r\n", Status));
     return Status;
  }

  //
  // Register callbacks for SW SMI
  //

  SwContext.SwSmiInputValue = EFI_PERF_TUNE_SW_SMI;
  Status = mSwDispatch->Register( mSwDispatch, PerfTuneSmiCallback, &SwContext, &SwHandle );
  DEBUG(( EFI_D_ERROR, "mSwDispatch->Register  %r\n", Status)); 
  DEBUG(( EFI_D_ERROR, "SwContext.SwSmiInputValue  %x\n", SwContext.SwSmiInputValue)); 
  ASSERT_EFI_ERROR( Status );

  Status = gBS->LocateProtocol(&gEfiSmmVariableProtocolGuid,
                               NULL,
                               (VOID **)&mSmmVariable);
  DEBUG(( EFI_D_ERROR, "mSmmVariable  %r\n", Status));

  //
  // SandyBridge BWG 0.6 section 14.15.2
  // XeCap detect for CPUID 206A3 (C0) and above: 
  // 1. MSR 0xCE[28] == 1     (Programmable Ratio Limits for Turbo Mode)
  // 2. MSR 0x194[19:17] == 7 (Number of overclocking bins supported)
  //
  if ((AsmReadMsr64 (EFI_PLATFORM_INFORMATION) & BIT28) 
    && ((AsmReadMsr64 (EFI_IA32_CLOCK_FLEX_MAX) & (BIT17|BIT18|BIT19)) == (BIT17|BIT18|BIT19)) ) {
    mXeModeCap = 1; 
  } else {
    mXeModeCap = 0;
  }

  // Read PLATFORM_INFO MSR
  MsrValue = AsmReadMsr64 (0x000001A0);
  MsrValue = MsrValue & (BIT38);
  // If MSR 1A0 BIT38 Does Not equal CPUID 06 then processor is capable of Turbo Mode
  AsmCpuid (
    6,
    &CpuidRegisters.RegEax,
    &CpuidRegisters.RegEbx,
    &CpuidRegisters.RegEcx,
    &CpuidRegisters.RegEdx
    );
  if ( ((CpuidRegisters.RegEax & 0x02) && (!(MsrValue))) ||
       ((!(CpuidRegisters.RegEax & 0x02)) && (MsrValue)) )
  {
    mTurboModeCap = 1;
  }

  return EFI_SUCCESS;
}

VOID
PerfTuneSmiCallback (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SW_DISPATCH_CONTEXT   *DispatchContext
  )
{
   UINT8 OrigPostCode;

   OrigPostCode = IoRead8(0x80);
   IoWrite8(0x80, 0x71);
    mSaveState = (EFI_SMM_CPU_SAVE_STATE *)(gSmst->CpuSaveState);
   //
   // ECX = 0 is the Read command
   //
   if ((mSaveState->Ia32SaveState.ECX & 0x0FF) == 0) {
      ReadBiosSetting();
   }
   //
   // ECX = 1 is the Write command
   //
   else if ((mSaveState->Ia32SaveState.ECX & 0x0FF) == 1) {
      WriteBiosSetting();
   }
   //
   // ECX = 2 is the command to enable the watchdog timer after post
   //
   else if ((mSaveState->Ia32SaveState.ECX & 0x0FF) == 2) {
      EnableWatchdogAfterPost();
   }

   IoWrite8(0x80, OrigPostCode);
}

//[-start-130710-IB05160465-modify]//
VOID
ReadBiosSetting (
  )
{
  EFI_STATUS                                Status;
  BIOS_SETTINGS_DATA_STRUCT                 *Data;
  UINTN                                     VarSize;
  UINTN                                     Index;
  UINT64                                    MsrValue;
  UINT32                                    CorrectDataLength;

   IoWrite8(0x80, 0x72);

   Data = (BIOS_SETTINGS_DATA_STRUCT*)(UINTN)mSaveState->Ia32SaveState.EBX;
   //
   // Assume Successful Return Value
   //
   mSaveState->Ia32SaveState.EBX = PERF_TUNE_BIOS_SUCCESS;
   mSaveState->Ia32SaveState.ECX = 0;

   //
   // Check for the debug signature.  If found halt and set the signature to the expected value
   //
   if (Data->Hdr.Signature == mDbgSig) {
      //EFI_DEADLOOP();
      CpuDeadLoop();
      Data->Hdr.Signature = mCurSig;
   }
   //
   // Verify the input of the Signature and Length fields are valid
   //

   if (Data->Hdr.Signature != mCurSig) {
      if (Data->Hdr.Signature == mOldSig) {
         Data->Hdr.Signature = mCurSig;
         Data->Hdr.Length    = sizeof(BIOS_SETTINGS_DATA_STRUCT);
         Data->Hdr.MajorRev  = BIOS_SETTINGS_STRUCTURE_MAJOR_REVISION;
         Data->Hdr.MinorRev  = BIOS_SETTINGS_STRUCTURE_MINOR_REVISION;
         mSaveState->Ia32SaveState.EBX = USE_OF_OLD_SIG;
      } else {
         mSaveState->Ia32SaveState.EBX = INVALID_SIGNATURE_INPUT;
      }
      return;
   }
   

  //
  // Get the Setup Variable
  //
//[-start-130709-IB05160465-modify]//
  VarSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//

  Status = mSmmVariable->SmmGetVariable (L"Setup",
                                      &mSystemConfigurationGuid,
                                      NULL,
                                      &VarSize,
                                      mSystemConfiguration
                                     );
  if (EFI_ERROR(Status)) {
     //
     // If this code is executed then there is a bad problem since the Setup variable does not exist yet.
     // Need to return a Not Ready error state.
     //
     mSaveState->Ia32SaveState.EBX = INTERNAL_BIOS_ERROR;
     mSaveState->Ia32SaveState.ECX = SETUP_VAR_READ_ERROR;

  } else {
    CorrectDataLength = sizeof(BIOS_SETTINGS_DATA_HEADER) + sizeof(UINT32) + (sizeof(mImplementedBiosSettings)-sizeof(BIOS_SETTING_ENTRY));
    if (mSystemConfiguration->XECap) {
      //
      // depend on XE Capability
      //
      CorrectDataLength += sizeof(mImplementedBiosSettingsXE)-sizeof(BIOS_SETTING_ENTRY);
    }

    if (Data->Hdr.Length != CorrectDataLength) {
       if (Data->Hdr.Length < sizeof(BIOS_SETTINGS_DATA_HEADER)) {
          mSaveState->Ia32SaveState.EBX = TABLE_LENGTH_TOO_SMALL_NO_HEADER;
          return;
       } else {
          UINT32 DataLen = Data->Hdr.Length;
          Data->Hdr.Signature = mCurSig;
          Data->Hdr.Length    = CorrectDataLength;
          Data->Hdr.MajorRev  = BIOS_SETTINGS_STRUCTURE_MAJOR_REVISION;
          Data->Hdr.MinorRev  = BIOS_SETTINGS_STRUCTURE_MINOR_REVISION;
          if (DataLen < CorrectDataLength) {
             mSaveState->Ia32SaveState.EBX = TABLE_LENGTH_TOO_SMALL_VALID_HEADER;
             return;
          } else if (DataLen > CorrectDataLength) {
             mSaveState->Ia32SaveState.EBX = TABLE_LEN_TOO_LARGE;
             return;
          }
       }
    }
  
    mBiosSettingsData.Hdr.Signature = mCurSig;
    mBiosSettingsData.Hdr.MajorRev = BIOS_SETTINGS_STRUCTURE_MAJOR_REVISION;
    mBiosSettingsData.Hdr.MinorRev = BIOS_SETTINGS_STRUCTURE_MINOR_REVISION;
    for (Index = 0; mImplementedBiosSettings[Index].BiosImplementType != 0xff; Index++) {
      mBiosSettingsData.BiosSettingEntry[Index].BiosImplementType = mImplementedBiosSettings[Index].BiosImplementType;
      mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mImplementedBiosSettings[Index].SettingValue;
    }
    mBiosSettingsData.BiosSettingCount = (UINT32)Index;
    //
    // depend on XE Capability
    //
    if (mSystemConfiguration->XECap) {
      for (Index = 0; mImplementedBiosSettingsXE[Index].BiosImplementType != 0xff; Index++) {
        mBiosSettingsData.BiosSettingEntry[Index + mBiosSettingsData.BiosSettingCount].BiosImplementType = mImplementedBiosSettingsXE[Index].BiosImplementType;
        mBiosSettingsData.BiosSettingEntry[Index + mBiosSettingsData.BiosSettingCount].SettingValue = mImplementedBiosSettingsXE[Index].SettingValue;
      }
      mBiosSettingsData.BiosSettingCount += (UINT32)Index;
    }
    
    mBiosSettingsData.Hdr.Length = CorrectDataLength;

    //
    // Init BiosImplement's SettingValue
    //
    for (Index=0; Index < mBiosSettingsData.BiosSettingCount; Index++) {

      switch (mBiosSettingsData.BiosSettingEntry[Index].BiosImplementType) {
        case PERF_TUNE_BIOS_PROC_MULT_CONTROL:
          //
          // BIOS Device: Max Non-Turbo Processor Multiplier (also known as Flex Ratio) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->FlexRatio;
          if ( mSystemConfiguration->FlexRatio == 0 ) {
            MsrValue = AsmReadMsr64(EFI_PLATFORM_INFORMATION);
            mBiosSettingsData.BiosSettingEntry[Index].SettingValue = (UINT8)((MsrValue & 0xFF00) >> 8);
          }
          break;
        case PERF_TUNE_BIOS_HOST_CLOCK_CONTROL:
          //
          // BIOS Device: Host Clock Frequency 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->HostClockFreq;
          break;
        case PERF_TUNE_BIOS_MEM_VOLTAGE_CONTROL:
          //
          // BIOS Device: Memory Voltage 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryVoltage;
          break;
        case PERF_TUNE_BIOS_TCL_CONTROL:
          //
          // BIOS Device: CAS Latency (tCL) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTcl;
          break;
        case PERF_TUNE_BIOS_TRCD_CONTROL:
          //
          // BIOS Device: Row Address to Column Address Delay (tRCD) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTrcd;
          break;
        case PERF_TUNE_BIOS_TRP_CONTROL:
          //
          // BIOS Device: Row Precharge Time (tRP) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTrp;
          break;
        case PERF_TUNE_BIOS_TRAS_CONTROL:
          //
          // BIOS Device: Row Active Time (tRAS) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTras;
          break;
        case PERF_TUNE_BIOS_TWR_CONTROL:
          //
          // BIOS Device: Write Recovery Time (tWR) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTwr;
          break;
//[-start-120217-IB03780424-modify]//
        case PERF_TUNE_BIOS_DDR_MULT_CONTROL:
          //
          // BIOS Device: DDR Multiplier 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->DdrFreqRequested;
          break;
//[-end-120217-IB03780424-modify]//
        case PERF_TUNE_BIOS_TURBO_ENABLE_CONTROL:
          //
          // BIOS Device: Turbo Mode Enable 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->EnableTurboMode;
          break;
        case PERF_TUNE_BIOS_EIST_CONTROL:
          //
          // BIOS Device: Enhanced Intel Speedstep Technology 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->ISTConfig;
          break;
        case PERF_TUNE_BIOS_TRFC_CONTROL:
          //
          // BIOS Device: Minimum Refresh Recovery Time (tRFC) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTrfc;
          break;
        case PERF_TUNE_BIOS_TRRD_CONTROL:
          //
          // BIOS Device: Row Active to Row Active delay (tRRD) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTrrd;
          break;
        case PERF_TUNE_BIOS_TWTR_CONTROL:
          //
          // BIOS Device: Internal Write to Read Command Delay (tWTR) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTwtr;
          break;
        case PERF_TUNE_BIOS_NMODE_CONTROL:
          //
          // BIOS Device: System Command Rate Mode
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->NModeSupport;
          break;          
        case PERF_TUNE_BIOS_TRTP_CONTROL:
          //
          // BIOS Device: Read to Precharge delay (tRTP) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTrtp;
          break;
        case PERF_TUNE_BIOS_1_CORE_RATIO_CONTROL:
          //
          // BIOS Device: 1-Active Core Ratio Limit 
          //
          if (mSystemConfiguration->XECap) {
            mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->RatioLimit1Core;
          } else {
            mBiosSettingsData.BiosSettingEntry[Index].SettingValue = BIOS_DEVICE_DISABLE;
          }
          
          break;
        case PERF_TUNE_BIOS_2_CORE_RATIO_CONTROL:
          //
          // BIOS Device: 2-Active Core Ratio Limit 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->RatioLimit2Core;
          break;
        case PERF_TUNE_BIOS_3_CORE_RATIO_CONTROL:
          //
          // BIOS Device: 3-Active Core Ratio Limit 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->RatioLimit3Core;
          break;
        case PERF_TUNE_BIOS_4_CORE_RATIO_CONTROL:
          //
          // BIOS Device: 4-Active Core Ratio Limit 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->RatioLimit4Core;
          break;
        case PERF_TUNE_BIOS_TRC_CONTROL:
          //
          // BIOS Device: Row Cycle Time (tRC) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTrc;
          break;
        case PERF_TUNE_BIOS_TFAW_CONTROL:
          //
          // BIOS Device: Four Active Window Delay (tFAW) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->MemoryTfaw;
          break;
        case PERF_TUNE_BIOS_MAX_TURBO_CPU_VOLTAGE_CONTROL:
          //
          // BIOS Device: Max Turbo Mode CPU Voltage (Sandy Bridge Only) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->VIDVal;
          break;
        case PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_LIMIT_CONTROL:
          //
          // Short Window Package Total Design Power Limit (Sandy Bridge Only) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->ShortDurationPowerLimit;
          break;
        case PERF_TUNE_BIOS_EXTEND_WIN_PKG_TDP_LIMIT_CONTROL:
          //
          // Extended Window Package Total Design Power Limit (Sandy Bridge Only) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->LongDurationPowerLimit;
          break;
        case PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_EN_DIS_CONTROL:
          //
          // Short Window Package Total Design Power Enable/Disable (Sandy Bridge Only) 
          //
//[-start-120523-IB06460400-modify]//
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->ShortTermPowerLimitOverride;
//[-end-120523-IB06460400-modify]//
          break;
        case PERF_TUNE_BIOS_PKG_TDP_LOCK_EN_DIS_CONTROL:
          //
          // Package Total Design Power Lock Enable/Disable (Sandy Bridge Only) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->TurboPowerLimitMsrLock;
          break;
        case PERF_TUNE_BIOS_IA_CORE_CURRENT_MAX_CONTROL:
          //
          // IA Core Current Maximum (Sandy Bridge Only)
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->PrimaryPlaneCurrentLimit;;
          break;
        case PERF_TUNE_BIOS_IGFX_CORE_CURRENT_MAX_CONTROL:
          //
          // iGFX Core Current Maximum (Sandy Bridge Only)
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->SecondaryPlaneCurrentLimit;;
          break;
        case PERF_TUNE_BIOS_GFX_TURBO_RATIO_LIMIT_CONTROL:
          //
          // Graphics Turbo Ratio Limit (Sandy Bridge with Internal Gfx only) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->GtOverclockFreq;
          break;
        case PERF_TUNE_BIOS_GFX_CORE_VOLTAGE_CONTROL:
          //
          // Graphics Core Voltage (Sandy Bridge with Internal Gfx only) 
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->GtOverclockVoltage;
          break;
        case PERF_TUNE_BIOS_XMP_CONTROL:
          //
          // XMP Profile Selection
          //
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = mSystemConfiguration->XmpProfileSetting;
          break;             
        default:
          mBiosSettingsData.BiosSettingEntry[Index].SettingValue = BIOS_DEVICE_DISABLE;
   
      } //switch
    } //for

    
    CopyMem (Data, &mBiosSettingsData, mBiosSettingsData.Hdr.Length);
  }
  {
    UINT32                    SafeStateSig;
    UINTN                     BufferSize;
    IoWrite8(0x80, 0x73);

    //
    // Determine if the variable already exists
    // If it does then return
    // If it does not then gather the current settings and save them
    //
    BufferSize = sizeof(UINT32);
    Status = mSmmVariable->SmmGetVariable(L"SafeSmiSig", &mEfiGlobalVariableGuidLocal, NULL, &BufferSize, &SafeStateSig);
    if (EFI_ERROR (Status)) {
      BIOS_SETTINGS_DATA_STRUCT TempData;
      CopyMem(&TempData, Data, sizeof(BIOS_SETTINGS_DATA_STRUCT));

      WriteBiosVariableData(&TempData, FALSE);
    }
  }
}

VOID
SaveCurrentSettings(
  IN  CHIPSET_CONFIGURATION   *SysConfig
  )
{
   EFI_STATUS                Status;
   UINT32                    SafeStateSig;
   UINTN                     BufferSize;
   IoWrite8(0x80, 0x73);

   //
   // Determine if the variable already exists
   // If it does then return
   // If it does not then gather the current settings and save them
   //
   BufferSize = sizeof(UINT32);
   Status = mSmmVariable->SmmGetVariable(L"SafeSmiSig", &mEfiGlobalVariableGuidLocal, NULL, &BufferSize, &SafeStateSig);
   if (EFI_ERROR (Status)) {
      //
      // Save the entire Setup Variable as safe state
      //
//[-start-130709-IB05160465-modify]//
      BufferSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
      Status = mSmmVariable->SmmSetVariable (L"SafeSmiSetup",
                               &mSystemConfigurationGuid,
                               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                               BufferSize,
                               SysConfig
                               );

      if (!EFI_ERROR(Status)) {
         //
         // Set variable to indicate the safe state is valid
         //
         SafeStateSig = mCurSig;  // SafeStateValid = TRUE;
         BufferSize = sizeof(UINT32);
         Status = mSmmVariable->SmmSetVariable (L"SafeSmiSig",
                                             &mEfiGlobalVariableGuidLocal,
                                             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                             BufferSize,
                                             &SafeStateSig
                                            );
      }
   }
}


VOID
WriteBiosSetting (
  )
{
   BIOS_SETTINGS_DATA_STRUCT  *Data;

   IoWrite8(0x80, 0x74);
   Data = (BIOS_SETTINGS_DATA_STRUCT*)(UINTN)mSaveState->Ia32SaveState.EBX;
   //
   // Assume Successful Return Value
   //
   mSaveState->Ia32SaveState.EBX = 0;
   mSaveState->Ia32SaveState.ECX = 0;

   WriteBiosVariableData(Data, TRUE);

}

VOID
WriteBiosVariableData(
  BIOS_SETTINGS_DATA_STRUCT   *Data,
  BOOLEAN                     WriteSysVar
  )
{
  EFI_STATUS                  Status;
  UINTN                       VarSize;
  CHIPSET_CONFIGURATION       *SetupDefaultBuffer;
  CHIPSET_CONFIGURATION       *OverclockingSafeModeBuffer; 
  UINTN                       Index;
  UINT32                      TmpSettingValue;
  UINT64                      MsrValue;
  BOOLEAN                     MemTimingChangeFlag = FALSE;
  BOOLEAN                     MemDefFlag = TRUE;

  //
  // Get the Setup Variable
  //
//[-start-130709-IB05160465-modify]//
  VarSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//
  //
  //"Custom" Variable doesn't exist,so get setup variable
  //   	
  Status = mSmmVariable->SmmGetVariable (L"Setup",
                                   &mSystemConfigurationGuid,
                                   NULL,
                                   &VarSize,
                                   mSystemConfiguration
                                  );                                    
  if (EFI_ERROR(Status)) {
    //
    // If this code is executed then there is a bad problem since the Setup variable does not exist yet.
    // Need to return a Not Ready error state.
    //
    mSaveState->Ia32SaveState.EBX = INTERNAL_BIOS_ERROR;
    mSaveState->Ia32SaveState.ECX = SETUP_VAR_READ_ERROR;
    return;
  }

  SetupDefaultBuffer = AllocateZeroPool (VarSize);
  OverclockingSafeModeBuffer = AllocateZeroPool (VarSize);

  mSmmVariable->SmmGetVariable(L"SetupDefault",
                                     &mSystemConfigurationGuid,
                                     NULL,
                                     &VarSize,
                                     SetupDefaultBuffer
                                    );

//[-start-130709-IB05160465-modify]//
  CopyMem (OverclockingSafeModeBuffer, mSystemConfiguration, PcdGet32 (PcdSetupConfigSize));
//[-end-130709-IB05160465-modify]//
  
  if (Data->Hdr.Signature != mCurSig) {
    //
    // If this code is executed then there is a bad problem since the Setup variable write failed.
    // Need to return a  error state.
    //
    mSaveState->Ia32SaveState.EBX = INVALID_SIGNATURE_INPUT;
    return;
  }

  if (Data->Hdr.MajorRev != BIOS_SETTINGS_STRUCTURE_MAJOR_REVISION || 
      Data->Hdr.MinorRev != BIOS_SETTINGS_STRUCTURE_MINOR_REVISION ||
      Data->BiosSettingCount > PERF_TUNE_BIOS_SETTINGS_MAX_COUNT) {
    //
    // Support XTU 2.1 only
    //
    mSaveState->Ia32SaveState.EBX = INVALID_SMI_REVISION;
    return;
  }

  //
  // Process BiosImplement's Items
  //
  for (Index=0; Index < Data->BiosSettingCount; Index++) {

    TmpSettingValue = Data->BiosSettingEntry[Index].SettingValue;
    if (TmpSettingValue == 0xFFFF) {
      //
      // 0xFFFF : device disable
      //
      continue;
    }

    switch (Data->BiosSettingEntry[Index].BiosImplementType) {
      case PERF_TUNE_BIOS_PROC_MULT_CONTROL:
        //
        // BIOS Device: Max Non-Turbo Processor Multiplier (also known as Flex Ratio) 
        //
        OverclockingSafeModeBuffer->FlexRatio = SetupDefaultBuffer->FlexRatio;
        if ( mSystemConfiguration->FlexRatio != TmpSettingValue) {
         mSystemConfiguration->CpuBurnInEnable = 1; // Put it in Manual mode
        }
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->FlexRatio;
        }
        MsrValue = AsmReadMsr32 (EFI_PLATFORM_INFORMATION);
        if (TmpSettingValue != (UINT32)((MsrValue & 0xFF00) >> 8)) {
          mSystemConfiguration->FlexRatio = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_HOST_CLOCK_CONTROL:
        //
        // BIOS Device: Host Clock Frequency 
        //
        OverclockingSafeModeBuffer->HostClockFreq = SetupDefaultBuffer->HostClockFreq;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->HostClockFreq;
        }
        mSystemConfiguration->HostClockFreq = (UINT16)TmpSettingValue;
        break;
      case PERF_TUNE_BIOS_MEM_VOLTAGE_CONTROL:
        //
        // BIOS Device: Memory Voltage 
        //
        OverclockingSafeModeBuffer->MemoryVoltage = SetupDefaultBuffer->MemoryVoltage;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryVoltage;
        }
        if (mSystemConfiguration->MemoryVoltage != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryVoltage = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryVoltage != SetupDefaultBuffer->MemoryVoltage)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_TCL_CONTROL:
        //
        // BIOS Device: CAS Latency (tCL) 
        //
        OverclockingSafeModeBuffer->MemoryTcl = SetupDefaultBuffer->MemoryTcl;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTcl;
        }
        if (mSystemConfiguration->MemoryTcl != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTcl = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTcl != SetupDefaultBuffer->MemoryTcl)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_TRCD_CONTROL:
        //
        // BIOS Device: Row Address to Column Address Delay (tRCD) 
        //
        OverclockingSafeModeBuffer->MemoryTrcd = SetupDefaultBuffer->MemoryTrcd;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTrcd;
        }
        if (mSystemConfiguration->MemoryTrcd != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTrcd = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTrcd != SetupDefaultBuffer->MemoryTrcd)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_TRP_CONTROL:
        //
        // BIOS Device: Row Precharge Time (tRP) 
        //
        OverclockingSafeModeBuffer->MemoryTrp = SetupDefaultBuffer->MemoryTrp;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTrp;
        }
        if (mSystemConfiguration->MemoryTrp != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTrp = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTrp != SetupDefaultBuffer->MemoryTrp)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_TRAS_CONTROL:
        //
        // BIOS Device: Row Active Time (tRAS) 
        //
        OverclockingSafeModeBuffer->MemoryTras = SetupDefaultBuffer->MemoryTras;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTras;
        }
        if (mSystemConfiguration->MemoryTras != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTras = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTras != SetupDefaultBuffer->MemoryTras)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_TWR_CONTROL:
        //
        // BIOS Device: Write Recovery Time (tWR) 
        //
        OverclockingSafeModeBuffer->MemoryTwr = SetupDefaultBuffer->MemoryTwr;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTwr;
        }
        if (mSystemConfiguration->MemoryTwr != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTwr = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTwr != SetupDefaultBuffer->MemoryTwr)
        {
          MemDefFlag = FALSE;
        }
        break;
//[-start-120217-IB03780424-modify]//
      case PERF_TUNE_BIOS_DDR_MULT_CONTROL:
        //
        // BIOS Device: DDR Multiplier 
        //
        OverclockingSafeModeBuffer->DdrFreqRequested = SetupDefaultBuffer->DdrFreqRequested;
        if (mSystemConfiguration->DdrFreqRequested != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->DdrFreqRequested = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->DdrFreqRequested != SetupDefaultBuffer->DdrFreqRequested)
        {
          MemDefFlag = FALSE;
        }
        break;
//[-end-120217-IB03780424-modify]//
      case PERF_TUNE_BIOS_TURBO_ENABLE_CONTROL:
        //
        // BIOS Device: Turbo Mode Enable 
        //
        if (mTurboModeCap != 0)
        {
          OverclockingSafeModeBuffer->EnableTurboMode = SetupDefaultBuffer->EnableTurboMode;
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->EnableTurboMode;
          }
          mSystemConfiguration->EnableTurboMode     = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_EIST_CONTROL:
        //
        // BIOS Device: Enhanced Intel Speedstep Technology 
        //
        OverclockingSafeModeBuffer->ISTConfig = SetupDefaultBuffer->ISTConfig;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->ISTConfig;
        }
        mSystemConfiguration->ISTConfig     = (UINT8)TmpSettingValue;
        break;
      case PERF_TUNE_BIOS_TRFC_CONTROL:
        //
        // BIOS Device: Minimum Refresh Recovery Time (tRFC)
        //
        OverclockingSafeModeBuffer->MemoryTrfc = SetupDefaultBuffer->MemoryTrfc;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTrfc;
        }
        if (mSystemConfiguration->MemoryTrfc != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTrfc = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTrfc != SetupDefaultBuffer->MemoryTrfc)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_TRRD_CONTROL:
        //
        // BIOS Device: Row Active to Row Active delay (tRRD) 
        //
        OverclockingSafeModeBuffer->MemoryTrrd = SetupDefaultBuffer->MemoryTrrd;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTrrd;
        }
        if (mSystemConfiguration->MemoryTrrd != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTrrd = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTrrd != SetupDefaultBuffer->MemoryTrrd)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_TWTR_CONTROL:
        //
        // BIOS Device: Internal Write to Read Command Delay (tWTR) 
        //
        OverclockingSafeModeBuffer->MemoryTwtr = SetupDefaultBuffer->MemoryTwtr;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTwtr;
        }
        if (mSystemConfiguration->MemoryTwtr != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTwtr = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTwtr != SetupDefaultBuffer->MemoryTwtr)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_NMODE_CONTROL:
        //
        // BIOS Device: System Command Rate Mode 
        //
        OverclockingSafeModeBuffer->NModeSupport= SetupDefaultBuffer->NModeSupport;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->NModeSupport;
        }
        if (mSystemConfiguration->NModeSupport != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->NModeSupport = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->NModeSupport != SetupDefaultBuffer->NModeSupport)
        {
          MemDefFlag = FALSE;
        }        
        break;
        
      case PERF_TUNE_BIOS_TRTP_CONTROL:
        //
        // BIOS Device: Read to Precharge delay (tRTP) 
        //
        OverclockingSafeModeBuffer->MemoryTrtp = SetupDefaultBuffer->MemoryTrtp;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTrtp;
        }
        if (mSystemConfiguration->MemoryTrtp != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTrtp = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTrtp != SetupDefaultBuffer->MemoryTrtp)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_1_CORE_RATIO_CONTROL:
        //
        // BIOS Device: 1-Active Core Ratio Limit 
        //
//[-start-120215-IB03780423-modify]//
        if (mSystemConfiguration->XECap) {
//[-end-120215-IB03780423-modify]//
          OverclockingSafeModeBuffer->RatioLimit1Core = SetupDefaultBuffer->RatioLimit1Core;
          if ( mSystemConfiguration->RatioLimit1Core != TmpSettingValue) {
           mSystemConfiguration->ExtremeEdition = 1; // Enable override
          }
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->RatioLimit1Core;
          }
          mSystemConfiguration->RatioLimit1Core = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_2_CORE_RATIO_CONTROL:
        //
        // BIOS Device: 2-Active Core Ratio Limit 
        //
//[-start-120215-IB03780423-modify]//
        if (mSystemConfiguration->XECap) {
//[-end-120215-IB03780423-modify]//
          OverclockingSafeModeBuffer->RatioLimit2Core = SetupDefaultBuffer->RatioLimit2Core;
          if ( mSystemConfiguration->RatioLimit2Core != TmpSettingValue) {
           mSystemConfiguration->ExtremeEdition = 1; // Enable override
          }
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->RatioLimit2Core;
          }
          mSystemConfiguration->RatioLimit2Core = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_3_CORE_RATIO_CONTROL:
        //
        // BIOS Device: 3-Active Core Ratio Limit 
        //
//[-start-120215-IB03780423-modify]//
        if (mSystemConfiguration->XECap) {
//[-end-120215-IB03780423-modify]//
          OverclockingSafeModeBuffer->RatioLimit3Core = SetupDefaultBuffer->RatioLimit3Core;
          if ( mSystemConfiguration->RatioLimit3Core != TmpSettingValue) {
           mSystemConfiguration->ExtremeEdition = 1; // Enable override
          }
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->RatioLimit3Core;
          }
          mSystemConfiguration->RatioLimit3Core = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_4_CORE_RATIO_CONTROL:
        //
        // BIOS Device: 4-Active Core Ratio Limit 
        //
//[-start-120215-IB03780423-modify]//
        if (mSystemConfiguration->XECap) {
//[-end-120215-IB03780423-modify]//
          OverclockingSafeModeBuffer->RatioLimit4Core = SetupDefaultBuffer->RatioLimit4Core;
          if ( mSystemConfiguration->RatioLimit4Core != TmpSettingValue) {
           mSystemConfiguration->ExtremeEdition = 1; // Enable override
          }
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->RatioLimit4Core;
          }
          mSystemConfiguration->RatioLimit4Core = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_TRC_CONTROL:
        //
        // BIOS Device: Row Cycle Time (tRC)
        //
        OverclockingSafeModeBuffer->MemoryTrc = SetupDefaultBuffer->MemoryTrc;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTrc;
        }
        if (mSystemConfiguration->MemoryTrc != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTrc = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTrc != SetupDefaultBuffer->MemoryTrc)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_TFAW_CONTROL:
        //
        // BIOS Device: Four Active Window Delay (tFAW) 
        //
        OverclockingSafeModeBuffer->MemoryTfaw = SetupDefaultBuffer->MemoryTfaw;
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
          TmpSettingValue = (UINT32)SetupDefaultBuffer->MemoryTfaw;
        }
        if (mSystemConfiguration->MemoryTfaw != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->MemoryTfaw = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->MemoryTfaw != SetupDefaultBuffer->MemoryTfaw)
        {
          MemDefFlag = FALSE;
        }
        break;
      case PERF_TUNE_BIOS_MAX_TURBO_CPU_VOLTAGE_CONTROL:
        //
        // BIOS Device: Max Turbo Mode CPU Voltage (Sandy Bridge Only) 
        //
        OverclockingSafeModeBuffer->VIDVal = SetupDefaultBuffer->VIDVal;
        if ( mSystemConfiguration->VIDVal != TmpSettingValue) {
         mSystemConfiguration->ExtremeEdition = 1; // Enable VID override
        }
        if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
           TmpSettingValue = (UINT32)SetupDefaultBuffer->VIDVal;
        }
        mSystemConfiguration->VIDVal= (UINT8)TmpSettingValue;
        break;
      case PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_LIMIT_CONTROL:
        //
        // Short Window Package Total Design Power Limit (Sandy Bridge Only) 
        //
        if (mTurboModeCap != 0)
        {
          OverclockingSafeModeBuffer->ShortDurationPowerLimit = SetupDefaultBuffer->ShortDurationPowerLimit;
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->ShortDurationPowerLimit;
          }
          mSystemConfiguration->ShortDurationPowerLimit     = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_EXTEND_WIN_PKG_TDP_LIMIT_CONTROL:
        //
        // Extended Window Package Total Design Power Limit (Sandy Bridge Only) 
        //
        if (mTurboModeCap != 0)
        {
          OverclockingSafeModeBuffer->LongDurationPowerLimit = SetupDefaultBuffer->LongDurationPowerLimit;
          mSystemConfiguration->LongDurationPowerLimit     = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_SHORT_WIN_PKG_TDP_EN_DIS_CONTROL:
        //
        // Short Window Package Total Design Power Enable/Disable (Sandy Bridge Only) 
        //
        if (mTurboModeCap != 0)
        {
//[-start-120523-IB06460400-modify]//
          OverclockingSafeModeBuffer->ShortTermPowerLimitOverride = SetupDefaultBuffer->LongTermPowerLimitOverride;
          mSystemConfiguration->ShortTermPowerLimitOverride     = (UINT8)TmpSettingValue;
//[-end-120523-IB06460400-modify]//
        }
        break;
      case PERF_TUNE_BIOS_PKG_TDP_LOCK_EN_DIS_CONTROL:
        //
        // Package Total Design Power Lock Enable/Disable (Sandy Bridge Only) 
        //
        if (mTurboModeCap != 0)
        {
          OverclockingSafeModeBuffer->TurboPowerLimitMsrLock = SetupDefaultBuffer->TurboPowerLimitMsrLock;
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->TurboPowerLimitMsrLock;
          }
          mSystemConfiguration->TurboPowerLimitMsrLock     = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_IA_CORE_CURRENT_MAX_CONTROL:
        //
        // IA Core Current Maximum (Sandy Bridge Only)
        //
        if (mTurboModeCap != 0)
        {
          OverclockingSafeModeBuffer->PrimaryPlaneCurrentLimit = SetupDefaultBuffer->PrimaryPlaneCurrentLimit;
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->PrimaryPlaneCurrentLimit;
          }
          mSystemConfiguration->PrimaryPlaneCurrentLimit     = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_IGFX_CORE_CURRENT_MAX_CONTROL:
        //
        // iGFX Core Current Maximum (Sandy Bridge Only)
        //
        if (mTurboModeCap != 0)
        {
          OverclockingSafeModeBuffer->SecondaryPlaneCurrentLimit = SetupDefaultBuffer->SecondaryPlaneCurrentLimit;
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->SecondaryPlaneCurrentLimit;
          }
          mSystemConfiguration->SecondaryPlaneCurrentLimit     = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_GFX_TURBO_RATIO_LIMIT_CONTROL:
        //
        // Graphics Turbo Ratio Limit (Sandy Bridge with Internal Gfx only) 
        //
        if (mXeModeCap != 0) {
          OverclockingSafeModeBuffer->GtOverclockFreq = SetupDefaultBuffer->GtOverclockFreq;
          if ( mSystemConfiguration->GtOverclockFreq != TmpSettingValue) {
           mSystemConfiguration->GtOverclockSupport = 1; // Enable override
          }
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->GtOverclockFreq;
          }
          mSystemConfiguration->GtOverclockFreq = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_GFX_CORE_VOLTAGE_CONTROL:
        //
        // Graphics Core Voltage (Sandy Bridge with Internal Gfx only) 
        //
        if (mXeModeCap != 0) {
          OverclockingSafeModeBuffer->GtOverclockVoltage = SetupDefaultBuffer->GtOverclockVoltage;
          if ( mSystemConfiguration->GtOverclockVoltage != TmpSettingValue) {
           mSystemConfiguration->GtOverclockSupport = 1; // Enable override
          }
          if (TmpSettingValue == BIOS_DEVICE_AUTOMATIC) {
            TmpSettingValue = (UINT32)SetupDefaultBuffer->GtOverclockVoltage;
          }
          mSystemConfiguration->GtOverclockVoltage = (UINT8)TmpSettingValue;
        }
        break;
      case PERF_TUNE_BIOS_XMP_CONTROL:
        //
        // XMP Profile Selection
        //
        OverclockingSafeModeBuffer->XmpProfileSetting= SetupDefaultBuffer->XmpProfileSetting;
        if (mSystemConfiguration->XmpProfileSetting != (UINT8)TmpSettingValue) {
          MemTimingChangeFlag = TRUE;
        }
        mSystemConfiguration->XmpProfileSetting = (UINT8)TmpSettingValue;
        if (mSystemConfiguration->XmpProfileSetting != SetupDefaultBuffer->XmpProfileSetting)
        {
          MemDefFlag = FALSE;
        };
//[-start-120224-IB03780425-remove]//
//        if (mSystemConfiguration->XmpProfileSetting != mCurrentProfile) {
//          mXtuRuntimeFlag |= B_XTU_XMP_CHANGE;
//        } else {
//          mXtuRuntimeFlag &= B_XTU_XMP_CHANGE;
//        }
//        EfiWriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuRuntimeStatus, mXtuRuntimeFlag);
//[-end-120224-IB03780425-remove]//
        break;

    } //switch
  } //for

  //
  // if the MemDefFlag is TRUE then all of the settings have been set to their default value so
  // it is best at this point to force the mode flag to automatic mode.
  //
  OverclockingSafeModeBuffer->EnableMemoryOverride = 0; // Set it to AUTOMATIC_MODE (disable memory overclocking)
  if (MemDefFlag == TRUE)
  {
    mSystemConfiguration->EnableMemoryOverride = 0; // Set it to AUTOMATIC_MODE
  } else if (MemTimingChangeFlag == TRUE) {
    mSystemConfiguration->EnableMemoryOverride = 1; // Set it to MANUAL_MODE
  }

//[-start-120224-IB03780425-add]//
  if (MemTimingChangeFlag) {
    mXtuRuntimeFlag |= B_XTU_XMP_CHANGE;
    WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuRuntimeStatus, mXtuRuntimeFlag);
  }
//[-end-120224-IB03780425-add]//

  //
  // Update the Setup Variable
  //
//[-start-130709-IB05160465-modify]//
  VarSize = PcdGet32 (PcdSetupConfigSize);
//[-end-130709-IB05160465-modify]//

  SaveCurrentSettings(OverclockingSafeModeBuffer);

  FreePool (SetupDefaultBuffer);
  FreePool (OverclockingSafeModeBuffer);

//[-start-120316-IB06460376-remove]//
//  if (WriteSysVar) {
//    mSystemConfiguration->OverclockingSource = 1; //OVERCLOCK_SOURCE_OS;
//    Status = mSmmVariable->SetVariable(L"Setup",
//                                       &mSystemConfigurationGuid,
//                                       EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
//                                       VarSize,
//                                       &mSystemConfiguration);
//
//    if (EFI_ERROR(Status)) {
//       //
//       // If this code is executed then there is a bad problem since the Setup variable write failed.
//       // Need to return a  error state.
//       //
//       mSaveState->EBX = INTERNAL_BIOS_ERROR;
//       mSaveState->ECX = SETUP_VAR_WRITE_ERROR;
//    }
//  }
//[-end-120316-IB06460376-remove]//

}
//[-end-130710-IB05160465-modify]//

VOID
EnableWatchdogAfterPost(
  )
{
  UINT32                     TimeoutValue;

  IoWrite8(0x80, 0x75);
  TimeoutValue = mSaveState->Ia32SaveState.EBX;
  if (TimeoutValue == 0) {
    ClearOsBootStatusFlag();
  }

  //
  // Assume Successful Return Value
  //
  mSaveState->Ia32SaveState.EBX = 0;
  mSaveState->Ia32SaveState.ECX = 0;
}


UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
   EFI_DEVICE_PATH_PROTOCOL     *Start;

   if (DevicePath == NULL) {
      return 0;
   }

   //
   // Search for the end of the device path structure
   //
   Start = DevicePath;
   while (!IsDevicePathEnd (DevicePath)) {
      DevicePath = NextDevicePathNode (DevicePath);
   }

   //
   // Compute the size and add back in the size of the end device path structure
   //
   return((UINTN)DevicePath - (UINTN)Start) + sizeof(EFI_DEVICE_PATH_PROTOCOL);
}

VOID
ClearOsBootStatusFlag(
  )
{
}
