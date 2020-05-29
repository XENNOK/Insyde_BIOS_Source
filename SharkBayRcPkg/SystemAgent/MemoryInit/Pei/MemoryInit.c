/** @file
  Memory Initialization PEIM.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement.
**/

#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/TxtLibrary.h>

#include <CpuFuncs.h>
#include <CpuRegs.h>

#include <MemInfoHob.h>
#include <MemoryInit.h>
#include <MrcDebugHook.h>
#include <MrcOemDebugPrint.h>
#include <MrcOemIo.h>
#include <MrcOemPlatform.h>
#include <MrcSaveRestore.h>
#include <MrcSpdDriver.h>
#include <McGdxcbar.h>
#include <MrcCommon.h>
#include <Library/CpuPlatformLib.h>

//[-start-120331-IB05300304-add]//
#include <PchRegs.h>
#include <BaseAddrDef.h>
//[-end-120331-IB05300304-add]//
#include <PttHciRegs.h>

// The next extern is temporary, including MrcCommon.h causes compile problems.
extern
MrcFrequency
MrcGetCurrentMemoryFrequency (
  MrcParameters * const MrcData,
  U32 * const             MemoryClock,
  MrcClockRatio * const   Ratio,
  MrcRefClkSelect * const RefClk
  );

typedef struct _PEI_BASE_MEMORY_TEST_PPI PEI_BASE_MEMORY_TEST_PPI;

typedef enum {
  Ignore,
  Quick,
  Sparse,
  Extensive
} PEI_MEMORY_TEST_OP;

typedef
EFI_STATUS
(EFIAPI *PEI_BASE_MEMORY_TEST) (
  IN  EFI_PEI_SERVICES                   **PeiServices,
  IN  PEI_BASE_MEMORY_TEST_PPI           * This,
  IN  EFI_PHYSICAL_ADDRESS               BeginAddress,
  IN  UINT64                             MemoryLength,
  IN  PEI_MEMORY_TEST_OP                 Operation,
  OUT EFI_PHYSICAL_ADDRESS               * ErrorAddress
  );

struct _PEI_BASE_MEMORY_TEST_PPI {
  PEI_BASE_MEMORY_TEST  BaseMemoryTest;
};

extern EFI_GUID gPeiBaseMemoryTestPpiGuid;

//
// Driver Dependent PPI Prototypes
//
#include <Ppi/Capsule.h>
#include <Ppi/PlatformMemoryRange.h>
#include <Ppi/PlatformMemorySize.h>
#include <Ppi/Stall.h>
#include <Ppi/SaPlatformPolicy.h>
#include <Ppi/CpuPlatformPolicy.h>
#include <Ppi/Wdt.h>
#include <Ppi/PchReset.h>
#include <Ppi/PchInit.h>

//
// Driver Consumed GUID
//
#include <Guid/TxtInfoHob.h>
#include <Guid/AcpiVariable.h>
#include <Guid/MemoryTypeInformation.h>
#include <Guid/SmramMemoryReserve.h>
//[-start-120216-IB05330384-add]//
#ifdef DDR3LV_SUPPORT
#include <Guid/DDR3LVoltageVariable.h>
#include <Guid/DDR3LVoltageHob.h>
#endif
//[-end-120216-IB05330384-add]//
//[-start-120327-IB06460380-add]//
#ifdef USE_FAST_CRISIS_RECOVERY
#include <Ppi/EmuPei.h>
#endif
//[-end-120327-IB06460380-add]//

#ifdef RAPID_START_FLAG
#include <Ppi/RapidStart.h>
#include <Library/RapidStartCommonLib.h>
#include <Library/RapidStartPeiLib.h>
#endif

#ifdef ME_SUPPORT_FLAG
#include <Ppi/PchMeUma.h>
#endif

#if 0//def MRC_DEBUG_PRINT
#include <DebugMask.h>
#endif
//[-start-121019-IB05330384-add]//
#ifdef DDR3LV_SUPPORT
#include <PchRegs/PchRegsLpc.h>
#endif
//[-end-121019-IB05330384-add]//

//
// Driver GUID Definitions
//
EFI_GUID  gMemRestoreDataGuid   = EFI_MEMORY_RESTORE_DATA_GUID;
EFI_GUID  gPeiCapsulePpiGuid    = PEI_CAPSULE_PPI_GUID;
EFI_GUID  gEfiAcpiVariableGuid  = EFI_ACPI_VARIABLE_GUID;
#if 0//def MRC_DEBUG_PRINT
EFI_GUID  gEfiGenericVariableGuid = EFI_GENERIC_VARIABLE_GUID;
#endif


#ifdef MRC_DEBUG_PRINT
const UINT8 BootStringFc[]  = "BOOT_WITH_FULL_CONFIGURATION";
const UINT8 BootStringMc[]  = "BOOT_WITH_MINIMAL_CONFIGURATION";
const UINT8 BootStringNc[]  = "BOOT_ASSUMING_NO_CONFIGURATION_CHANGES";
const UINT8 BootStringFcd[] = "BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS";
const UINT8 BootStringDs[]  = "BOOT_WITH_DEFAULT_SETTINGS";
const UINT8 BootStringS4[]  = "BOOT_ON_S4_RESUME";
const UINT8 BootStringS5[]  = "BOOT_ON_S5_RESUME";
const UINT8 BootStringS2[]  = "BOOT_ON_S2_RESUME";
const UINT8 BootStringS3[]  = "BOOT_ON_S3_RESUME";
const UINT8 BootStringFu[]  = "BOOT_ON_FLASH_UPDATE";
const UINT8 BootStringRm[]  = "BOOT_IN_RECOVERY_MODE";
const UINT8 BootStringRmm[] = "BOOT_IN_RECOVERY_MODE_MASK";
const UINT8 BootStringSm[]  = "BOOT_SPECIAL_MASK";
const UINT8 BootStringUnk[] = "BOOT_MODE_UNKNOWN";
#endif
//[-start-120327-IB06460380-add]//
#ifdef USE_FAST_CRISIS_RECOVERY
BOOLEAN  mInRecoveryPei = FALSE;
#endif
//[-end-120327-IB06460380-add]//

//[-start-121220-IB01830992-add]//
VOID
RemoveSaveMemoryDataHob (
  IN EFI_PEI_SERVICES    **PeiServices
  );
//[-end-121220-IB01830992-add]//

/**
  Main starting point for system memory initialization.
    1. Get SysBootMode and MrcBootMode
    2. Locate SaPlatformPolicy PPI
    3. Locate S3DataPtr from SaPlatformPolicy.
    4. SaveDataValid := TRUE if S3DataPtr is not NULL.
    5. If SysBootMode is BOOT_ON_S3_RESUME and S3Data is not valid:
    -> ASSERT.
    6. If MrcBootMode is Warm boot, but S3 data is not valid :
    -> change MrcBootMode to Cold boot.
    7. If MrcBootMode is Cold boot:
    -> Run MRC code
    -> Save S3 Restore Data
    Else
    -> Run MRC_S3Resume
    8. Run MRC_Done().
    9. Install EFI memory HOBs.

  @param[in] FfsHeader        - Not used.
  @param[in] PeiServices      - General purpose services available to every PEIM.

  @retval EFI_SUCCESS      - Memory initialization completed successfully.
  @retval EFI_NOT_READY    - Cannot locate SA Platform Policy.
  @retval EFI_NOT_FOUND    - No S3 data in S3 Boot Mode.
  @retval EFI_DEVICE_ERROR - MemoryInit failed or IOSAV Memory test failed.
**/
EFI_STATUS
PeimMemoryInit (
  IN EFI_PEI_FILE_HANDLE       FfsHeader,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_PEI_READ_ONLY_VARIABLE_PPI  *VariableServices;
  SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi;
  HOB_SAVE_MEMORY_DATA        *Hob;
  MrcParameters               *MrcData;
  MrcInput                    *Inputs;
  MrcOutput                   *Outputs;
  SysSave                     *SaveSys;
  MrcSave                     *Save;
  MrcCpuModel                 CpuModel;
  MrcCpuStepping              CpuStep;
  UINT32                      CpuModelStep;
  BOOLEAN                     CpuDetected;
  MrcParameters               MrcGlobalData;
  EFI_STATUS                  Status;
  EFI_BOOT_MODE               SysBootMode;
  MrcStatus                   MrcStatus;
  MrcBootMode                 MrcBootMode;
  MrcVersion                  Version;
  BOOLEAN                     SaveDataValid;
  UINT32                      Crc32;
  UINT64                      SskpdValue;
#ifdef MRC_DEBUG_PRINT
  MrcDebug                    *Debug;
  const UINT8                 *Str;
#endif
#ifdef ME_SUPPORT_FLAG
  PCH_ME_UMA_PPI              *PchMeUma;
  UINT8                       InitStat;
  UINT8                       ForceFullTraining;
  UINT8                       OrigMrcBootMode;
#endif
#ifdef RAPID_START_FLAG
  PCH_RESET_PPI               *PchResetPpi;
  RAPID_START_PPI             *RapidStartPpi;
#endif
  UINT8                       TotalDprSizeMB;
//[-start-120327-IB06460380-add]//
#ifdef USE_FAST_CRISIS_RECOVERY
  VOID                        *EmuPeiPpi;
#endif
//[-end-120327-IB06460380-add]//
  UINT32                      MemoryClock;
  MrcClockRatio               Ratio;
  MrcUpmPwrRetrainLimits      RetrainLimits[MRC_NUMBER_UPM_PWR_RETRAIN_MARGINS];
//[-start-120216-IB05330384-add]//
#ifdef DDR3LV_SUPPORT
  UINTN                       VariableSize;
  DDR3L_VOLTAGE_SETTING       DDR3LVoltageDate;
  UINT32                      GPIOBase;
  UINT32                      GPIO31_0_In;
  UINT32                      GPIO63_32_In;
  UINT32                      GPIO95_64_In;
  UINT32                      GPIO31_0_Out;
  UINT32                      GPIO63_32_Out;
  UINT32                      GPIO95_64_Out;  
  EFI_HOB_GUID_TYPE           *GuidHob;
  UINT8                       IndexPin;
#endif
//[-end-120216-IB05330384-add]//
//[-start-121220-IB01830992-add]//
#ifdef RAPID_START_FLAG
  EFI_BOOT_MODE               OriSysBootMode;
#endif
//[-end-121220-IB01830992-add]//

//[-start-120216-IB05330384-add]//
#ifdef DDR3LV_SUPPORT
  GuidHob = NULL;
#endif
//[-end-120216-IB05330384-add]//
//[-start-120327-IB06460380-add]//
#ifdef USE_FAST_CRISIS_RECOVERY
  //
  // Check if we are in recovery onlu PEI phase
  //
  Status = (*PeiServices)->LocatePpi (
                             (CONST EFI_PEI_SERVICES **)PeiServices,
                             &gEmuPeiPpiGuid,
                             0,
                             NULL,
                             (VOID **)&EmuPeiPpi
                             );
  mInRecoveryPei = EFI_ERROR(Status) ? FALSE : TRUE;
#endif
//[-end-120327-IB06460380-add]//
  MrcData = &MrcGlobalData;
  Inputs  = &MrcData->SysIn.Inputs;
  Outputs = &MrcData->SysOut.Outputs;
  ZeroMem (MrcData, sizeof (MrcParameters));
  Outputs->UpmPwrRetrainLimits.Pointer = RetrainLimits;
  MrcOemMemoryCpy (
    (U8 *) RetrainLimits,
    (U8 *) InitialLimits,
    sizeof (MrcUpmPwrRetrainLimits) * MRC_NUMBER_UPM_PWR_RETRAIN_MARGINS
    );

  //
  // Obtain boot mode.
  //
  Status = (*PeiServices)->GetBootMode (PeiServices, &SysBootMode);
  ASSERT_EFI_ERROR (Status);

  if (SysBootMode != BOOT_ON_S3_RESUME) {
    Status = MrcGetHobForDataStorage ((EFI_PEI_SERVICES **)PeiServices, &Hob, sizeof (HOB_SAVE_MEMORY_DATA));
    ASSERT_EFI_ERROR (Status);
  } else {
    Hob = 0;
  }

#ifdef SSA_FLAG
  Status = (**PeiServices).LocatePpi (PeiServices, &gSsaBiosCallBacksPpiGuid, 0, NULL, (VOID **) &Inputs->SsaCallbackPpi);
  if (EFI_SUCCESS != Status) {
    Inputs->SsaCallbackPpi = 0;
  }
  Inputs->Debug.Stream   = (U32) PeiServices;
  Inputs->SsaHeapBase    = (U32) &Hob->MrcData;
  Inputs->SsaHeapSize    = MRC_HOB_SIZE_TOTAL - sizeof (EFI_HOB_GUID_TYPE);
  PEI_DEBUG (((void *) PeiServices, EFI_D_ERROR, "SsaCallbackPpi = %Xh\n", Inputs->SsaCallbackPpi));
  PEI_DEBUG (((void *) PeiServices, EFI_D_ERROR, "SSA heap. Base = %Xh, Size = %d\n", Inputs->SsaHeapBase, Inputs->SsaHeapSize));
#endif // SSA_FLAG

  //
  // Obtain platform policy settings.
  //
  Status = (**PeiServices).LocatePpi (PeiServices, &gSaPlatformPolicyPpiGuid, 0, NULL, (VOID **) &SaPlatformPolicyPpi);
  ASSERT_EFI_ERROR (Status);

  MrcOemDebugHook (MrcData, MRC_INITIALIZATION_START);
#ifdef MRC_DEBUG_PRINT
  Debug   = &Inputs->Debug;
#endif
//[-start-120327-IB06460380-add]//
#ifdef USE_FAST_CRISIS_RECOVERY
  if (mInRecoveryPei) {
//[-start-121127-IB05300361-remove]//
//    //
//    // Get the Memory Map
//    //
//    Status = MrcGetHobForDataStorage (PeiServices, &Hob, sizeof(HOB_SAVE_MEMORY_DATA));
//    ASSERT_EFI_ERROR (Status);
//[-end-121127-IB05300361-remove]//

//[-start-130507-IB10310018-remove]//
//    ((*PeiServices)->CopyMem) ((VOID *) &Hob->MrcData, MrcData, sizeof (MrcParameters));
//[-end-130507-IB10310018-remove]//

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Install EFI Memory\n");
    Status = InstallEfiMemory (Inputs, (EFI_PEI_SERVICES **)PeiServices, SysBootMode);
    return Status;
  }
#endif
//[-end-120327-IB06460380-add]//

  MRC_DEBUG_MSG_OPEN (
    Debug,
    (SaPlatformPolicyPpi->MemConfig->MrcTimeMeasure > 0) ? MSG_LEVEL_TIME : SaPlatformPolicyPpi->MemConfig->SerialDebug,
    (U32) PeiServices,
    (SysBootMode == BOOT_ON_S3_RESUME) ? 0 : (U32) &Hob->MrcData,
    (SysBootMode == BOOT_ON_S3_RESUME) ? 0 : (MRC_HOB_SIZE_TOTAL - sizeof (EFI_HOB_GUID_TYPE))
    );

#ifdef ME_SUPPORT_FLAG
  InitStat = 0;
  ForceFullTraining = 0;
#endif

  MrcStatus = mrcSuccess;

  //
  // Obtain variable services.
  //
  Status = (*PeiServices)->LocatePpi (PeiServices, &gEfiPeiReadOnlyVariablePpiGuid, 0, NULL, (VOID **) &VariableServices);
  ASSERT_EFI_ERROR (Status);

#ifndef TXT_SUPPORT
  //
  // Unlock memory if it is necessary.
  //
  UnlockMemory (MrcData, (EFI_PEI_SERVICES **)PeiServices);
#endif // TXT_SUPPORT

  //
  // Get MRC BootMode
  //
  MrcBootMode = (SysBootMode == BOOT_ON_S3_RESUME) ? bmS3 : MrcGetBootMode ();

#ifdef MRC_DEBUG_PRINT
  if ((SysBootMode == BOOT_ON_S3_RESUME) && (bmCold == MrcGetBootMode ())) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "SysBootMode = %Xh and MrcBootMode = %d - Check PCH SR bit\n",
      SysBootMode,
      MrcBootMode
      );
  }
#endif // MRC_DEBUG_PRINT

#ifdef RAPID_START_FLAG
  //
  // Locate RapidStart PPI
  //
  Status = (*PeiServices)->LocatePpi (PeiServices, &gRapidStartPpiGuid, 0, NULL, (VOID **)&RapidStartPpi);
  ASSERT_EFI_ERROR (Status);
#endif // RAPID_START_FLAG

  MrcVersionGet (&Version);
  MrcVersionPrint (MrcData, &Version);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nSystem boot mode = %Xh\n", SysBootMode);
#ifdef MRC_DEBUG_PRINT
  switch (SysBootMode) {
    case BOOT_WITH_FULL_CONFIGURATION:                  Str = BootStringFc;   break;
    case BOOT_WITH_MINIMAL_CONFIGURATION:               Str = BootStringMc;   break;
    case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:        Str = BootStringNc;   break;
    case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS: Str = BootStringFcd;  break;
    case BOOT_WITH_DEFAULT_SETTINGS:                    Str = BootStringDs;   break;
    case BOOT_ON_S4_RESUME:                             Str = BootStringS4;   break;
    case BOOT_ON_S5_RESUME:                             Str = BootStringS5;   break;
    case BOOT_ON_S2_RESUME:                             Str = BootStringS2;   break;
    case BOOT_ON_S3_RESUME:                             Str = BootStringS3;   break;
    case BOOT_ON_FLASH_UPDATE:                          Str = BootStringFu;   break;
    case BOOT_IN_RECOVERY_MODE:                         Str = BootStringRm;   break;
    case BOOT_IN_RECOVERY_MODE_MASK:                    Str = BootStringRmm;  break;
    case BOOT_SPECIAL_MASK:                             Str = BootStringSm;   break;
    default:                                            Str = BootStringUnk;  break;
  }
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nSystem boot mode = %s\n", Str);
#endif //MRC_DEBUG_PRINT

  //
  // Locate and determine if memory configuration save data is valid.
  //
  SaveDataValid = FALSE;
  if ((SaPlatformPolicyPpi->S3DataPtr != NULL) && (SysBootMode != BOOT_WITH_DEFAULT_SETTINGS)) {
    SaveSys = (SysSave *) (SaPlatformPolicyPpi->S3DataPtr);
    Save    = &SaveSys->Save;
    Crc32 = MrcCalculateCrc32 ((U8 *) (&Save->Data), sizeof (MrcSaveData));
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Calc. crc = 0x%x, Header crc = 0x%x\n", Crc32, Save->Header.Crc);
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "S3DataPtr = 0x%x - &MrcData = 0x%x - sizeof (MrcParameters) = 0x%x\n",
      SaPlatformPolicyPpi->S3DataPtr,
      &MrcData,
      sizeof (MrcParameters)
      );

      if (Crc32 == Save->Header.Crc) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Saved memory configuration data is valid\n");
      ((*PeiServices)->CopyMem) ((VOID *) &MrcData->SysSave, (VOID *) SaveSys, sizeof (SysSave));
      SaveDataValid = TRUE;
    }
  }

  //
  // We must have memory configuration save data in order to resume from S3.
  //
  if ((SysBootMode == BOOT_ON_S3_RESUME) && (!SaveDataValid)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Unable to resume from S3 without valid saved memory configuration data\n");
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

#ifdef ME_SUPPORT_FLAG
  //
  // Locate PchMeUma PPI.
  //
  Status = (*PeiServices)->LocatePpi (PeiServices, &gPchMeUmaPpiGuid, 0, NULL, (VOID **)&PchMeUma);
  ASSERT_EFI_ERROR (Status);

  if (MrcBootMode != bmS3 && MrcBootMode != bmWarm) {
    //
    // Check CPU Replaced Status, if so a system non-power cycle reset will be required.
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Calling CpuReplacementCheck\n");
    Status = PchMeUma->CpuReplacementCheck(PeiServices, FfsHeader, &ForceFullTraining);

    if (ForceFullTraining == 0x1) {
      SaveDataValid = FALSE;
    }
  }

  // Keep track of the original MRC Boot mode before an alternate flow is determined below.
  OrigMrcBootMode = MrcBootMode;
#endif // ME_SUPPORT_FLAG

  CpuModel     = GetCpuFamily();
  CpuStep      = GetCpuStepping();
  CpuModelStep = CpuModel | CpuStep;
  CpuDetected  = (MrcSetCpuInformation (MrcData, CpuModel, CpuStep) == mrcSuccess) ? TRUE : FALSE;

  if (!CpuDetected) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: CPU Family/Model/Step %Xh is not supported:\n", CpuModelStep);
    ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
  }

  //
  // MrcBootMode can ONLY be bmCold, bmWarm or bmS3 at this point.
  //
  switch (MrcBootMode) {
    case bmCold:
      // Advance the MRC boot mode to fast boot if the following condition is met.
      if ((SaveDataValid == TRUE) &&
        (SaPlatformPolicyPpi->MemConfig->MrcFastBoot > 0) &&
        (ColdBootRequired (MrcData, SaPlatformPolicyPpi)) == FALSE) {
          MrcBootMode = bmFast;
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Fast boot is possible, so forcing it\n");
      }
      else {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Cold boot\n");
        SaveDataValid = FALSE;
      }
      break;

    case bmWarm:
    case bmS3:
      if (SaveDataValid == FALSE) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "Saved memory configuration data is not valid, forcing a cold boot\n");
        MrcBootMode = bmCold;
        break;
      } else {
        if (ColdBootRequired (MrcData, SaPlatformPolicyPpi) == TRUE) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_WARNING,
            "Platform settings or configuration have changed, forcing a cold boot\n"
            );
          MrcBootMode = bmCold;
          SaveDataValid = FALSE;
          break;
        }
        //
        // Check SSKPD register to determine if Warm Reset occured before MRC was reached during a cold boot.
        // If so, we need to force the cold boot path.
        //
        MrcOemMmioRead64 (PCU_CR_SSKPD_PCU_REG, &SskpdValue, SaPlatformPolicyPpi->PlatformData->MchBar);
        if ((SskpdValue == 0) && (MrcBootMode == bmWarm)) {
          MRC_DEBUG_MSG (
            Debug,
            MSG_LEVEL_WARNING,
            "Reset occured in the cold boot path before reaching MRC.  Forcing Cold Boot\n"
            );
          MrcBootMode = bmCold;
          SaveDataValid = FALSE;
          break;
        }
      }
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%s\n", (MrcBootMode == bmS3) ? "Resume from S3" : "Warm reset");
      break;

    default:
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Cold boot\n");
      MrcBootMode = bmCold;
      SaveDataValid = FALSE;
      break;
  }

  //
  // Clear MrcSave if not valid saved data. We don't want to end up with Ghost DIMMs
  //
  if (SaveDataValid == FALSE) {
    ZeroMem (&MrcData->SysSave.Save, sizeof (MrcSave));
  }

#ifdef MRC_DEBUG_PRINT
  ((*PeiServices)->CopyMem) ((VOID *) &MrcData->SysIn.Inputs.Debug, Debug, sizeof (MrcDebug));
  Debug = &Inputs->Debug;
#endif // MRC_DEBUG_PRINT

  //
  //Calculate Total DPR Size
  //
  CalculateTotalDprMemorySize ((EFI_PEI_SERVICES **)PeiServices, &TotalDprSizeMB);

  if(TotalDprSizeMB != 0){
    Inputs->DprSize = (U32) TotalDprSizeMB;
  }

  //
  // Set up the MRC input data structure.
  //
  Inputs->BootMode = MrcSetupMrcData (SysBootMode, MrcBootMode, Inputs, (EFI_PEI_SERVICES **)PeiServices, SaPlatformPolicyPpi);

  //
  // Initialize MeStolenSize to 0 before we retrieving from ME FW.
  //
  Inputs->MeStolenSize = 0;

#ifdef RAPID_START_FLAG
  if ((SysBootMode != BOOT_ON_S3_RESUME) && (RapidStartPpi->GetMode (RapidStartPpi) == RapidStartExit)) {
    //
    // Need to erase whole memory for Rapid Start Resume
    //
    Inputs->OemCleanMemory = TRUE;
  }
#endif  // RAPID_START_FLAG

#ifdef ME_SUPPORT_FLAG
      //
      // ME Stolen Size in MB units
      //
      DEBUG ((EFI_D_ERROR, "Calling MeSendUmaSize\n"));
      Inputs->MeStolenSize = PchMeUma->MeSendUmaSize (PeiServices, FfsHeader);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ME UMA size = %u MB\n", Inputs->MeStolenSize);
#endif  // ME_SUPPORT_FLAG

  do {
    if (Inputs->BootMode == bmCold) {
      //
      // Clear DRAM Init Bit if we are doing a cold boot, to prevent hang if a warm reset occurs in the training flow
      // where an old memory config is saved.
      //
      MrcResetDISB ();
    }
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MRC Start Memory Configuration\n");
    MrcStatus = MrcStartMemoryConfiguration (MrcData);

    switch (MrcStatus) {
      case mrcSuccess:
        break;

      case mrcRetrain:
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Rerunning training with higher UPM/PWR limits!\n");
          ZeroMem (Outputs, sizeof (MrcOutput));
          Outputs->UpmPwrRetrainLimits.Pointer = RetrainLimits;
          Inputs->Iteration++;
        break;

      case mrcFrequencyError:
        MrcGetCurrentMemoryFrequency (MrcData, &MemoryClock, &Ratio, NULL);
        if (Ratio >= Outputs->Ratio) {
          MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Memory initialization has failed\n");
          //
          // Get lower byte and set the error bit
          //
          MrcOemOutPort8 (0x80, MrcOemInPort8 (0x80) | MRC_FAILURE_INDICATION);
          ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
          return EFI_DEVICE_ERROR;
        } else {
          // Restart memory configuration, using the lower frequency.
          MrcStatus = mrcColdBootRequired;
        }
        // no break;

      case mrcColdBootRequired:
        if (Inputs->BootMode == bmFast) {
          // At this point, input structure has limited data.
          // We need to initialize the input structure for the cold boot.
          Inputs->BootMode = MrcSetupMrcData (SysBootMode, bmCold, Inputs, (EFI_PEI_SERVICES **)PeiServices, SaPlatformPolicyPpi);
        } else {
          Inputs->BootMode = bmCold;
        }
        break;

      case mrcDimmNotExist:
#ifdef ME_SUPPORT_FLAG
        //
        // Set memory init status = 0x1 and send DRAM Init Done to ME FW,
        // indicating that no memory exists in the system.
        //
        InitStat = 0x1;
        Status = PchMeUma->MeConfigDidReg (PeiServices, FfsHeader, MrcBootMode, InitStat, Outputs->MemoryMapData.FtpmStolenBase, Inputs->MeStolenSize);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "FtpmStolenBase = 0x%08X \n", Outputs->MemoryMapData.FtpmStolenBase);
#endif  // ME_SUPPORT_FLAG
        MrcOemDebugHook (MrcData, MRC_NO_MEMORY_DETECTED);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "There are no DIMMs present in the system\n");
        //
        //Indicate to the caller that memory has not been detected.
        //
        (*PeiServices)->ReportStatusCode (
          PeiServices,
          EFI_ERROR_CODE,
          EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_NONE_DETECTED,
          0,
          NULL,
          NULL
          );
        // no break;

      default:
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Memory initialization has failed\n");
        //
        // Get lower byte and set the error bit
        //
        MrcOemOutPort8 (0x80, MrcOemInPort8 (0x80) | MRC_FAILURE_INDICATION);
        ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
        return EFI_DEVICE_ERROR;
    }
  } while ((MrcStatus == mrcColdBootRequired) || (MrcStatus == mrcRetrain));

  //
  // Intel Silicon View Technology (ISVT) IO Reading port 0x84 with AH = 1 for End of MRC
  //
#if defined __GNUC__  // GCC compiler
  __asm__ __volatile__ (
    "\n\t mov $0x100, %eax"
    "\n\t in $0x84, %al"
    );
#else //MSFT compiler
  ASM {
    mov EAX, 100h
    in  AL, 84h
  }
#endif

#ifdef ME_SUPPORT_FLAG
  //
  // Configure "ME DRAM Init Done Register"
  //
  //
  // ME UMA Size outside of a 0MB-32MB range is not defined or if BDF 0:22:0 is not present, exit.
  //
#ifdef RAPID_START_FLAG
  //
  // Check wake conditions to determine if a Rapid Start transition is to be performed
  // and set the RapidStart flag in DID.
  //
  if (RapidStartPpi->GetMode (RapidStartPpi) != RapidStartNone) {
    InitStat |= 0x80;
  }
#endif  // RAPID_START_FLAG

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Check for Memory Retrain on warm reset -- MrcBootMode=0x%02X OrigBootMode=0x%02X\n", MrcBootMode, OrigMrcBootMode);
  // On warm reset if memory coherency was not maintained (forced Cold Reset flow), set the DID message
  //  to indicate that memory was not preserved across reset, so that ME will reload the FW from NV memory.
  if (bmWarm == OrigMrcBootMode && bmCold == MrcBootMode) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Memory retrain occurred during warm reset.  Force ME FW reload.\n");
    // Set the flag to tell FW that memory was not maintained InitStat bits 3:0 = (3).
    InitStat = (InitStat & 0xF0) | 0x3;
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ME UMA Size requested: %d MB\n", Inputs->MeStolenSize);
  if ((Inputs->MeStolenSize > 0x20) || (PchD22PciCfg32 (0x10) == 0xffffffff)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Invalid ME UMA Size requested.\n");
  } else {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Calling MeConfigDidReg\n");
    Status = PchMeUma->MeConfigDidReg (PeiServices, FfsHeader, MrcBootMode, InitStat, Outputs->MemoryMapData.FtpmStolenBase,Inputs->MeStolenSize);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MeDramInitDone Complete.\n");
  }
#endif  // ME_SUPPORT_FLAG

  //
  // SATA must be initialized before Rapid Start transition.
  //
  //remove for PCH RC updated to V44, the code would be elimated if SA/MRC sync to V44 as well
  //  Status = PchInitPpi->SataInit (PeiServices);
  //  ASSERT_PEI_ERROR (PeiServices, Status);

#ifdef RAPID_START_FLAG
  //
  // Perform Rapid Start transition if necessary (BootMode mode may change here!)
  // Rapid Start requires MRC Fast boot to be enabled for best performance.
  // In Rapid Start Resume flow if MRC boot mode is not bmFast that means memory
  // configuration has changed and Rapid Start resume should be aborted.
  //
  if ((SaPlatformPolicyPpi->MemConfig->MrcFastBoot) && (RapidStartPpi->GetMode (RapidStartPpi) == RapidStartExit) && (Inputs->BootMode != bmFast)) {
    DEBUG ((EFI_D_ERROR, "Memory Configuration changed! Rapid Start Resume is aborted!\n"));
    RapidStartAfterTransition (PeiServices, RapidStartExit, EFI_MEDIA_CHANGED, 0);
    Status = (*PeiServices)->LocatePpi (
                              PeiServices,
                              &gPchResetPpiGuid,
                              0,
                              NULL,
                              (VOID **)&PchResetPpi
                              );
    ASSERT_EFI_ERROR (Status);
    PchResetPpi->Reset (PchResetPpi, PowerCycleReset);
  } else {
//[-start-121220-IB01830992-add]//
    //
    // Save original boot mode for later comparison.
    //
    OriSysBootMode = SysBootMode;
//[-end-121220-IB01830992-add]//
    RapidStartPpi->TransitionEntryPoint (RapidStartPpi, &SysBootMode);
//[-start-121220-IB01830992-add]//
    //
    // If original boot mode is not S3 resume and Rapid Start module change the 
    // boot mode to S3 resume, then remove gMemRestoreDataGuid HOB from HOB
    // database because it is too big and uesless.
    //
    if ((SysBootMode != OriSysBootMode) && (SysBootMode == BOOT_ON_S3_RESUME)) {
      RemoveSaveMemoryDataHob ((EFI_PEI_SERVICES **)PeiServices);
    }
//[-end-121220-IB01830992-add]//
  }
#endif // RAPID_START_FLAG
  //
  // Install EFI memory HOBs
  //
  if (SysBootMode == BOOT_ON_S3_RESUME) {
    Status = InstallS3Memory (Inputs, (EFI_PEI_SERVICES **)PeiServices, VariableServices);
    ASSERT_EFI_ERROR (Status);
  } else {
    if ((MrcBootMode == bmCold) || (MrcBootMode == bmFast)) {
      //
      // Perform simple memory test.
      //
      if (mrcFail == BasicMemoryTest (Inputs)) {
        MrcOemDebugHook (MrcData, MRC_MEM_INIT_DONE_WITH_ERRORS);
        ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
        return EFI_DEVICE_ERROR;
      }
    }
    MrcData->SaveSize = sizeof (MrcSave);
#ifdef MRC_DEBUG_PRINT
    Debug->Current    = 0;
#endif // MRC_DEBUG_PRINT
    ((*PeiServices)->CopyMem) ((VOID *) &Hob->MrcData, MrcData, sizeof (MrcParameters));
    ZeroMem ((VOID *) &Hob->Buffer, MRC_HOB_SIZE_BUFFER);

    Status = InstallEfiMemory (Inputs, (EFI_PEI_SERVICES **)PeiServices, SysBootMode);
    ASSERT_EFI_ERROR (Status);
  }
//[-start-120216-IB05330384-add]//
#ifdef DDR3LV_SUPPORT
    //
    //  Keep GPIO Setting for Warm boot 
    //  1. Save GPIO date to Hob
    //  2. Save to variable in DEX phase.
    //  3. Before Memoryinit doen, system will check GPIO setting for DDR3L
    // 
    //  This is step 3.
    //
  GuidHob = GetFirstGuidHob (&gDDR3LVoltageHobGuid);
  if(GuidHob == NULL) {
    VariableSize = sizeof (DDR3L_VOLTAGE_SETTING);
    Status = VariableServices->PeiGetVariable (
                                (EFI_PEI_SERVICES **)PeiServices,
                                L"DDR3LVoltageVariable",
                                &gDDR3LVoltageVariableGuid,
                                NULL,
                                &VariableSize,
                                &DDR3LVoltageDate
                                );     
  }
  
  if ((Status == EFI_SUCCESS) && (GuidHob == NULL)) {
    //
    // Read GPIO base
    //
    MrcOemMmioRead (
      MrcOemGetPcieDeviceAddress (0, PCI_DEVICE_NUMBER_PCH_LPC, 0, R_PCH_LPC_GPIO_BASE),
      &GPIOBase,
      Inputs->PciEBaseAddress
      );

    GPIOBase &= ~MRC_BIT0;    
    GPIO31_0_In = IoRead32((UINT16)(GPIOBase + R_PCH_GPIO_LVL));
    GPIO63_32_In = IoRead32((UINT16)(GPIOBase + R_PCH_GPIO_LVL2));
    GPIO95_64_In = IoRead32((UINT16)(GPIOBase + R_PCH_GPIO_LVL3));
    GPIO31_0_Out = GPIO31_0_In;
    GPIO63_32_Out = GPIO63_32_In;
    GPIO95_64_Out = GPIO95_64_In;

    for (IndexPin = 0; IndexPin < 3; IndexPin++) {
      if (DDR3LVoltageDate.GPIOPingSetting & (MRC_BIT0 << IndexPin) ) {
        if (DDR3LVoltageDate.GPIOPingSelect[IndexPin] < 32) {
          GPIO31_0_Out |= (MRC_BIT0 << DDR3LVoltageDate.GPIOPingSelect[IndexPin]);
        } else if ((DDR3LVoltageDate.GPIOPingSelect[IndexPin] > 31) && (DDR3LVoltageDate.GPIOPingSelect[IndexPin] < 64)){
          GPIO63_32_Out |= (MRC_BIT0 << (DDR3LVoltageDate.GPIOPingSelect[IndexPin] + 32));
        } else {
          GPIO95_64_Out |= (MRC_BIT0 << (DDR3LVoltageDate.GPIOPingSelect[IndexPin] + 64));        
        }
      } else {
        if (DDR3LVoltageDate.GPIOPingSelect[IndexPin] < 32) {
          GPIO31_0_Out &= (~(MRC_BIT0 << DDR3LVoltageDate.GPIOPingSelect[IndexPin]));
        } else if ((DDR3LVoltageDate.GPIOPingSelect[IndexPin] > 31) && (DDR3LVoltageDate.GPIOPingSelect[IndexPin] < 64)){
          GPIO63_32_Out &= (~(MRC_BIT0 << (DDR3LVoltageDate.GPIOPingSelect[IndexPin] + 32)));
        } else {
          GPIO95_64_Out &= (~(MRC_BIT0 << (DDR3LVoltageDate.GPIOPingSelect[IndexPin] + 64)));
        }
      }
    }
    
    if (((GPIO31_0_Out != GPIO31_0_In) ||
         (GPIO63_32_Out != GPIO63_32_In) ||
         GPIO95_64_Out != GPIO95_64_In)) {
      IoWrite32((U16)(GPIOBase + R_PCH_GPIO_LVL), GPIO31_0_Out);
      IoWrite32((U16)(GPIOBase + R_PCH_GPIO_LVL2), GPIO63_32_Out);
      IoWrite32((U16)(GPIOBase + R_PCH_GPIO_LVL3), GPIO95_64_Out);      
    }
  }
#endif
//[-end-120216-IB05330384-add]//

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "MemoryInit Complete.\n");
  MrcOemDebugHook (MrcData, MRC_MEM_INIT_DONE);

  return Status;
}

/**
  This function installs memory for all paths except S3 resume.

  @param[in] Inputs      - MRC input structure.
  @param[in] PeiServices - PEI Services table.
  @param[in] SysBootMode - The specific boot path that is being followed.

  @retval EFI_SUCCESS          The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES Out of Resources.
**/
EFI_STATUS
InstallEfiMemory (
  IN const MrcInput *const Inputs,
  IN EFI_PEI_SERVICES      **PeiServices,
  IN EFI_BOOT_MODE         SysBootMode
  )
{
  EFI_STATUS                            Status;
  UINT8                                 Index;
  UINT8                                 NumRanges;
  UINT8                                 SmramIndex;
  UINT8                                 SmramRanges;
  UINT64                                PeiMemoryLength;
  UINT64                                RangeLength;
  UINTN                                 BufferSize;
  UINTN                                 CapsuleBufferLength;
  UINTN                                 PeiMemoryIndex;
  UINTN                                 RequiredMemSize;
  VOID                                  *CapsuleBuffer;
  EFI_PEI_HOB_POINTERS                  Hob;
  EFI_PHYSICAL_ADDRESS                  PeiMemoryBaseAddress;
  EFI_PHYSICAL_ADDRESS                  TopUseableMemAddr;
  EFI_PHYSICAL_ADDRESS                  TopUseableMemSize;
  EFI_PHYSICAL_ADDRESS                  Tom;
  PEI_MEMORY_TEST_OP                    MemoryTestOp;
  PEI_BASE_MEMORY_TEST_PPI              *BaseMemoryTestPpi;
  PEI_CAPSULE_PPI                       *Capsule;
  PEI_PLATFORM_MEMORY_SIZE_PPI          *MemSize;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramHobDescriptorBlock;
  EFI_SMRAM_DESCRIPTOR                  *SmramDescriptor;
  PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE MemoryMap[MAX_RANGES];
  EFI_PHYSICAL_ADDRESS                  BadMemoryAddress;
  EFI_RESOURCE_TYPE                     ResourceType;
  EFI_RESOURCE_ATTRIBUTE_TYPE           ResourceAttribute;
  const MrcDebug                        *Debug;

  Debug = &Inputs->Debug;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Install EFI memory.\n");

  //
  // Get the Memory Map
  //
  NumRanges = MAX_RANGES;
  ZeroMem (MemoryMap, sizeof (PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE) * NumRanges);
  Status = GetMemoryMap (PeiServices, (PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE *) MemoryMap, &NumRanges);
  ASSERT_EFI_ERROR (Status);

  //
  // Find the highest memory range in processor native address space to give to
  // PEI. Then take the top.
  // If this algorithm changes, then we need to fix the capsule memory
  // selection algorithm below.
  //
  PeiMemoryBaseAddress = 0;

  //
  // Query the platform for the minimum memory size.
  //
  Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gPeiPlatformMemorySizePpiGuid, 0, NULL, (VOID **) &MemSize);
  ASSERT_EFI_ERROR (Status);

  Status = MemSize->GetPlatformMemorySize (PeiServices, MemSize, &PeiMemoryLength);
  ASSERT_EFI_ERROR (Status);

  //
  // Get required memory size for ACPI use. This helps to put ACPI memory on the top.
  //
  RequiredMemSize = 0;
  RetrieveRequiredMemorySize (PeiServices, &RequiredMemSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Required memory size = %Xh (%u) bytes\n", RequiredMemSize, RequiredMemSize);

  PeiMemoryIndex = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Found %016Xh bytes at ", MemoryMap[Index].RangeLength);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%016Xh\n", MemoryMap[Index].PhysicalAddress);
    if ((MemoryMap[Index].Type == DualChannelDdrMainMemory) &&
      (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength < MAX_ADDRESS) &&
      (MemoryMap[Index].PhysicalAddress >= PeiMemoryBaseAddress) &&
      (MemoryMap[Index].RangeLength >= PeiMemoryLength)) {
//[-start-120327-IB06460380-modify]//
#ifdef USE_FAST_CRISIS_RECOVERY
      if (mInRecoveryPei) {
        //
        // MRC code in Ram.
        //
        PeiMemoryLength <<= 1;
        PeiMemoryBaseAddress  =  MemoryMap[Index].PhysicalAddress+ MemoryMap[Index].RangeLength - PeiMemoryLength;
        PeiMemoryLength >>= 1;
      } else {
        PeiMemoryBaseAddress  = MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength - PeiMemoryLength;
      }
#else       
        PeiMemoryBaseAddress = MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength - PeiMemoryLength;
#endif      
//        PeiMemoryBaseAddress = MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength - PeiMemoryLength;
//[-end-120327-IB06460380-modify]//
        PeiMemoryIndex       = Index;
    }
  }

  //
  // Test only the PEI memory if necessary. Some platforms do not require the
  // Base Memory PEIM to be present.
  //
  Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gPeiBaseMemoryTestPpiGuid, 0, NULL, (VOID **) &BaseMemoryTestPpi);

  switch (SysBootMode) {

  case BOOT_WITH_FULL_CONFIGURATION:
    MemoryTestOp = Quick;
    break;

  case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
    MemoryTestOp = Extensive;
    break;

  default:
    MemoryTestOp = Ignore;
    break;
  }

  (*PeiServices)->ReportStatusCode (
    (CONST EFI_PEI_SERVICES **)PeiServices,
    EFI_PROGRESS_CODE,                                  // Type
    EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_PC_TEST,  // Value
    0,    // Instance
    NULL, // *CallerId OPTIONAL
    NULL  // *Data OPTIONAL
    );

  Status = BaseMemoryTestPpi->BaseMemoryTest (
    PeiServices,
    BaseMemoryTestPpi,
    PeiMemoryBaseAddress,
    PeiMemoryLength,
    MemoryTestOp,
    &BadMemoryAddress
    );
  if (EFI_ERROR (Status)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "Memory test failure at %lXh.\n", BadMemoryAddress);
  }

  ASSERT_EFI_ERROR (Status);

  Capsule             = NULL;
  CapsuleBuffer       = NULL;
  CapsuleBufferLength = 0;

  if (SysBootMode == BOOT_ON_FLASH_UPDATE) {
    Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gPeiCapsulePpiGuid, 0, NULL, (VOID **) &Capsule);
    ASSERT_EFI_ERROR (Status);

    if (Status == EFI_SUCCESS) {
      //
      // Find the largest memory range excluding that given to PEI.
      //
      for (Index = 0; Index < NumRanges; Index++) {
        if ((MemoryMap[Index].Type == DualChannelDdrMainMemory) &&
          (MemoryMap[Index].PhysicalAddress + MemoryMap[Index].RangeLength < MAX_ADDRESS)) {
          if (Index != PeiMemoryIndex) {
            if (MemoryMap[Index].RangeLength > CapsuleBufferLength) {
              CapsuleBuffer       = (VOID *) ((UINTN) MemoryMap[Index].PhysicalAddress);
              CapsuleBufferLength = (UINTN) MemoryMap[Index].RangeLength;
            }
          } else {
            if ((MemoryMap[Index].RangeLength - PeiMemoryLength) >= CapsuleBufferLength) {
              CapsuleBuffer       = (VOID *) ((UINTN) MemoryMap[Index].PhysicalAddress);
              CapsuleBufferLength = (UINTN) (MemoryMap[Index].RangeLength - PeiMemoryLength);
            }
          }
        }
      }
      //
      // Call the Capsule PPI Coalesce function to coalesce the capsule data.
      //
      Status = Capsule->Coalesce (PeiServices, &CapsuleBuffer, &CapsuleBufferLength);
    }
    //
    // If it failed, then NULL out our capsule PPI pointer so that the capsule
    // HOB does not get created below.
    //
    if (Status != EFI_SUCCESS) {
      Capsule = NULL;
    }
  }
  //
  // Carve out the top memory reserved for ACPI.
  //
  Status = (*PeiServices)->InstallPeiMemory ((CONST EFI_PEI_SERVICES **)PeiServices, PeiMemoryBaseAddress, PeiMemoryLength - RequiredMemSize);
  ASSERT_EFI_ERROR (Status);

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Building RESOURCE_SYSTEM_MEMORY Hob: \n");
  DEBUG ((EFI_D_ERROR, "PeiMemoryBaseAddress = %lXh, PeiMemoryLength = %lXh\n", PeiMemoryBaseAddress, PeiMemoryLength));

  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    (
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_TESTED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
    ),
    PeiMemoryBaseAddress,
    PeiMemoryLength
    );

//[-start-120327-IB06460380-add]//
#ifdef USE_FAST_CRISIS_RECOVERY
   if (mInRecoveryPei) {
     //
     // MRC code in Ram.
     //
     PeiMemoryLength <<= 1;
   }
#endif
//[-end-120327-IB06460380-add]//
  //
  // Install physical memory descriptor hobs for each memory range.
  //
  SmramRanges = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if (MemoryMap[Index].Type == DualChannelDdrMainMemory) {
      if (Index == PeiMemoryIndex) {
        //
        // This is a partially tested Main Memory range, give it to EFI
        //
        RangeLength = MemoryMap[Index].RangeLength - PeiMemoryLength;
      } else {
        //
        // This is an untested Main Memory range, give it to EFI.
        //
        RangeLength = MemoryMap[Index].RangeLength;
      }
      BuildResourceDescriptorHob (
        EFI_RESOURCE_SYSTEM_MEMORY,
        (
        EFI_RESOURCE_ATTRIBUTE_PRESENT |
        EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
        //
        // RC Override: mark <4G available memory as tested to give DXE enough memory space, so that default
        // memory allocations won't occupy the bins for specific memory types.
        //
        EFI_RESOURCE_ATTRIBUTE_TESTED |
        EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
        EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
        ),
        MemoryMap[Index].PhysicalAddress,
        RangeLength
        );

      //
      // Test this memory range
      //
      Status = BaseMemoryTestPpi->BaseMemoryTest (
        PeiServices,
        BaseMemoryTestPpi,
        MemoryMap[Index].PhysicalAddress,
        RangeLength,
        MemoryTestOp,
        &BadMemoryAddress
        );
      ASSERT_EFI_ERROR (Status);
    } else {
      if ((
          (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
          (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
          ) &&
          (MemoryMap[Index].PhysicalAddress != MC_ABSEG_HSEG_PHYSICAL_START)) {
        //
        // Only count and report TSEG.
        //
        SmramRanges++;
      }
      //
      // Make sure non-system memory is marked as reserved.
      //
      BuildResourceDescriptorHob (
        EFI_RESOURCE_MEMORY_RESERVED,     // MemoryType,
        0,                                // MemoryAttribute
        MemoryMap[Index].PhysicalAddress, // MemoryBegin
        MemoryMap[Index].RangeLength      // MemoryLength
        );
    }
  }

  BufferSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK);
  if (SmramRanges > 0) {
    BufferSize += ((SmramRanges - 1) * sizeof (EFI_SMRAM_DESCRIPTOR));
  }

  Hob.Raw = BuildGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid, BufferSize);
  if (Hob.Raw == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmramHobDescriptorBlock = (void *) (Hob.Raw);
  SmramHobDescriptorBlock->NumberOfSmmReservedRegions = SmramRanges;

  SmramIndex = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if ((
        (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
        (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
        ) &&
        (MemoryMap[Index].PhysicalAddress != MC_ABSEG_HSEG_PHYSICAL_START)) {
      //
      // This is an SMRAM range (not reporting AB_SEG or H_SEG), create an SMRAM descriptor.
      //
      SmramDescriptor = &SmramHobDescriptorBlock->Descriptor[SmramIndex];
      SmramDescriptor->PhysicalStart = MemoryMap[Index].PhysicalAddress;
      SmramDescriptor->CpuStart      = MemoryMap[Index].CpuAddress;

      //
      // RangeLength includes alignment adjustment.
      //
//[-start-121113-IB06150257-modify]//
//      if (SmramDescriptor->PhysicalStart < 0x100000) {
        SmramDescriptor->PhysicalSize  = MemoryMap[Index].RangeLength;
//      } else {
//        SmramDescriptor->PhysicalSize  = (Inputs->TsegSize - Inputs->IedSize )* 0x100000;
//      }
//[-end-121113-IB06150257-modify]//
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TSeg base is %Xh\n", SmramDescriptor->PhysicalStart);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TSeg SMRAM size is %Xh\n", SmramDescriptor->PhysicalSize);


      if (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) {
        SmramDescriptor->RegionState = EFI_SMRAM_CLOSED | EFI_CACHEABLE;
      } else {
        SmramDescriptor->RegionState = EFI_SMRAM_CLOSED;
      }
      SmramIndex++;
    }
  }
  //
  // Get the current "Top of Upper Usable Memory" address from TOUUD.
  // If TOUUD > 4G, it means memory is re-mapped.
  //
  TopUseableMemSize = McD0PciCfg64 (R_SA_TOUUD) & B_SA_TOUUD_TOUUD_MASK;
  TopUseableMemAddr = MEM_EQU_4GB;
  Tom = McD0PciCfg64 (R_SA_TOM) & B_SA_TOM_TOM_MASK;
  
  if (TopUseableMemSize > MEM_EQU_4GB) {
    //
    // This is above 4G memory address, give it to EFI.
    // If memory hob length is above 4G length, cut and separate it.
    //
    while ((TopUseableMemSize - MEM_EQU_4GB) > MEM_EQU_4GB) {
      DEBUG (( EFI_D_ERROR, "Found 0x100000000 bytes at 0x%016lX\n", TopUseableMemAddr));

      if (Inputs->MemoryTrace && (TopUseableMemAddr + MEM_EQU_4GB > RShiftU64 (Tom, 1))) {
        //
        // Mark memory above 4GB as reserved if it's used for Memory Trace
        //
        ResourceType      = EFI_RESOURCE_MEMORY_RESERVED;
        ResourceAttribute = 0;
      } else {
        ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
        ResourceAttribute = EFI_RESOURCE_ATTRIBUTE_PRESENT |
                            EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                            EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;
      }
      BuildResourceDescriptorHob (
        ResourceType,       // MemoryType,
        ResourceAttribute,  // MemoryAttribute
        TopUseableMemAddr,  // MemoryBegin
        MEM_EQU_4GB         // MemoryLength
        );
      TopUseableMemSize = TopUseableMemSize - MEM_EQU_4GB;
      TopUseableMemAddr = TopUseableMemAddr + MEM_EQU_4GB;
    }
    //
    // Create hob for remaining memory which is above 4G memory address.
    //
    if (TopUseableMemSize > MEM_EQU_4GB) {
      DEBUG (( EFI_D_ERROR, "Found 0x%016lX bytes at ", TopUseableMemSize - MEM_EQU_4GB));
      DEBUG (( EFI_D_ERROR, "0x%016lX\n", TopUseableMemAddr));
    }
    if (Inputs->MemoryTrace) {
      //
      // Mark memory above 4GB as reserved if it's used for Memory Trace
      //
      ResourceType      = EFI_RESOURCE_MEMORY_RESERVED;
      ResourceAttribute = 0;
    } else {
      ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
      ResourceAttribute = EFI_RESOURCE_ATTRIBUTE_PRESENT |
                          EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                          EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;
    }
    BuildResourceDescriptorHob (
      ResourceType,                     // MemoryType,
      ResourceAttribute,                // MemoryAttribute
      TopUseableMemAddr,                // MemoryBegin
      (TopUseableMemSize - MEM_EQU_4GB) // MemoryLength
      );
  }
  //
  // If we found the capsule PPI (and we didn't have errors), then
  // call the capsule PEIM to allocate memory for the capsule.
  //
  if (Capsule != NULL) {
    Status = Capsule->CreateState (PeiServices, CapsuleBuffer, CapsuleBufferLength);
  }

  return EFI_SUCCESS;
}

/**
  This function installs memory for the S3 resume path.

  @param[in] Inputs           - Mrc input data structure
  @param[in] PeiServices      - PEI services table.
  @param[in] VariableServices - Pointer to EFI Variable PPI

  @retval EFI_SUCCESS          - The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES - Out of Resources.
**/
EFI_STATUS
InstallS3Memory (
  IN const MrcInput *const      Inputs,
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_READ_ONLY_VARIABLE_PPI  *VariableServices
)
{
  EFI_STATUS                            Status;
  UINTN                                 VarSize;
  UINTN                                 VarAttrib;
  EFI_PHYSICAL_ADDRESS                  TopUseableMemAddr;
  EFI_PHYSICAL_ADDRESS                  TopUseableMemSize;
  EFI_PHYSICAL_ADDRESS                  Tom;
  UINT64                                AcpiVariableSet64;
  ACPI_VARIABLE_SET                     *AcpiVariableSet;
  UINTN                                 S3MemoryBase;
  UINTN                                 S3MemorySize;
//[-start-130315-IB06690220-remove]//
////[-start-120331-IB05300304-add]//
//  PCH_RESET_PPI                         *PchResetPpi;
//  UINT16                                Data16;
////[-end-120331-IB05300304-add]//
//[-end-130315-IB06690220-remove]//
  UINT8                                 NumRanges;
  PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE MemoryMap[MAX_RANGES];
  EFI_PEI_HOB_POINTERS                  Hob;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramHobDescriptorBlock;
  EFI_SMRAM_DESCRIPTOR                  *SmramDescriptor;
  EFI_RESOURCE_TYPE                     ResourceType;
  EFI_RESOURCE_ATTRIBUTE_TYPE           ResourceAttribute;
  UINT8                                 Index;
  UINT8                                 SmramIndex;
  UINT8                                 SmramRanges;
  UINTN                                 BufferSize;
  const MrcDebug                        *Debug;

  Debug = &Inputs->Debug;
  
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Install S3 resume memory.\n");
  NumRanges = MAX_RANGES;
  ZeroMem (MemoryMap, sizeof (PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE) * NumRanges);
  //
  // Call to GetMemoryMap to initialize TSEG registers.
  //
  Status = GetMemoryMap (PeiServices, (PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE *) MemoryMap, &NumRanges);
  ASSERT_EFI_ERROR (Status);

  AcpiVariableSet = NULL;
  VarSize         = sizeof (AcpiVariableSet64);
  VarAttrib       = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;

  Status = VariableServices->PeiGetVariable (
    PeiServices,
    ACPI_GLOBAL_VARIABLE,
    &gEfiAcpiVariableGuid,
    &VarAttrib,
    &VarSize,
    &AcpiVariableSet64
    );
  AcpiVariableSet = (ACPI_VARIABLE_SET *) (UINTN) AcpiVariableSet64;

  if (EFI_ERROR (Status) || (AcpiVariableSet == NULL)) {
    return EFI_OUT_OF_RESOURCES;
  }

  BuildGuidDataHob (&gEfiAcpiVariableGuid, AcpiVariableSet, sizeof (ACPI_VARIABLE_SET));

  MRC_DEBUG_MSG (
    Debug,
    MSG_LEVEL_NOTE,
    "AcpiVariableSet at = 0x%x. 0x%x\n",
    AcpiVariableSet,
    &AcpiVariableSet->AcpiReservedMemoryBase
    );

  S3MemoryBase = (UINTN) (AcpiVariableSet->AcpiReservedMemoryBase);
  S3MemorySize = (UINTN) (AcpiVariableSet->AcpiReservedMemorySize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "S3MemoryBase = 0x%x - S3MemorySize = 0x%x.\n", S3MemoryBase, S3MemorySize);
//[-start-130315-IB06690220-remove]//
////[-start-120331-IB05300304-add]//
//  Status = (*PeiServices)->LocatePpi (
//                             PeiServices,
//                             &gPchResetPpiGuid,
//                             0,
//                             NULL,
//                             &PchResetPpi
//                             );
//  ASSERT_EFI_ERROR (Status);
//  if (AcpiVariableSet->AcpiReservedMemorySize != 0x10000) {
////[-start-120507-IB03600478-modify]//
//    Data16 = IoRead16 (ACPI_IO_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT);
//    Data16 &= ~(V_PCH_ACPI_PM1_CNT_S3);
//    IoWrite16 (ACPI_IO_BASE_ADDRESS + R_PCH_ACPI_PM1_CNT, Data16);
//    PchResetPpi->Reset (PchResetPpi, GlobalReset);
////[-end-120507-IB03600478-modify]//
//  }
////[-end-120331-IB05300304-add]//
//[-end-130315-IB06690220-remove]//
  Status       = (*PeiServices)->InstallPeiMemory ((CONST EFI_PEI_SERVICES **)PeiServices, S3MemoryBase, S3MemorySize);
  ASSERT_EFI_ERROR (Status);

  //
  // Retrieve the system memory length and build memory hob for the system
  // memory above 1MB, so memory callback can set cache for the system memory
  // correctly on S3 resume path, just like it does on normal boot path.
  //
//[-start-120413-IB06460389-modify]//
  ASSERT ((AcpiVariableSet->SystemMemoryLengthBelow4GB - 0x100000) > 0);
  BuildResourceDescriptorHob (
    EFI_RESOURCE_SYSTEM_MEMORY,
    (
    EFI_RESOURCE_ATTRIBUTE_PRESENT |
    EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
    EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
    EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE
    ),
    0x100000,
    AcpiVariableSet->SystemMemoryLengthBelow4GB - 0x100000
    );

  //
  // Get the current "Top of Upper Usable Memory" address from TOUUD.
  // If TOUUD > 4G, it means memory is re-mapped.
  //
  TopUseableMemSize = McD0PciCfg64 (R_SA_TOUUD) & B_SA_TOUUD_TOUUD_MASK;
  TopUseableMemAddr = MEM_EQU_4GB;
  Tom = McD0PciCfg64 (R_SA_TOM) & B_SA_TOM_TOM_MASK;

  if (TopUseableMemSize > 0) {
    //
    // This is a above 4G memory, give it to EFI
    // if memory hob length is above 4G length,cut and separate it.
    //
//[-start-121119-IB06150257-modify]//
    if (TopUseableMemAddr + MEM_EQU_4GB > RShiftU64 (Tom, 1)) {
//[-end-121119-IB06150257-modify]//
      if (Inputs->MemoryTrace) {
        //
        // Mark memory above 4GB as reserved if it's used for Memory Trace
        //
        ResourceType      = EFI_RESOURCE_MEMORY_RESERVED;
        ResourceAttribute = 0;
      } else {
        ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
        ResourceAttribute = EFI_RESOURCE_ATTRIBUTE_PRESENT |
                            EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                            EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                            EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;
      }
      BuildResourceDescriptorHob (
        ResourceType,       // MemoryType,
        ResourceAttribute,  // MemoryAttribute
        TopUseableMemAddr,  // MemoryBegin
        MEM_EQU_4GB         // MemoryLength
        );
      TopUseableMemSize = TopUseableMemSize - MEM_EQU_4GB;
      TopUseableMemAddr = TopUseableMemAddr + MEM_EQU_4GB;
    }
    //
    // Create hob for remaining memory which is above 4G memory address.
    //
    if (Inputs->MemoryTrace) {
      //
      // Mark memory above 4GB as reserved if it's used for Memory Trace
      //
      ResourceType      = EFI_RESOURCE_MEMORY_RESERVED;
      ResourceAttribute = 0;
    } else {
      ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
      ResourceAttribute = EFI_RESOURCE_ATTRIBUTE_PRESENT |
                          EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
                          EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE |
                          EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE;
    }
    BuildResourceDescriptorHob (
      ResourceType,                     // MemoryType,
      ResourceAttribute,                // MemoryAttribute
      TopUseableMemAddr,                // MemoryBegin
      (TopUseableMemSize - MEM_EQU_4GB) // MemoryLength
      );
  }

  //
  // Report SMRAM ranges
  //

  SmramRanges = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if ((
        (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
        (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
        ) &&
        (MemoryMap[Index].PhysicalAddress  != MC_ABSEG_HSEG_PHYSICAL_START)
        ) {
      //
      // Only count TSEG
      //
      SmramRanges++;
    }
  }

  BufferSize = sizeof (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK);
  if (SmramRanges > 0) {
    BufferSize += ((SmramRanges - 1) * sizeof (EFI_SMRAM_DESCRIPTOR));
  }

  Hob.Raw = BuildGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid, BufferSize);

  SmramHobDescriptorBlock = (void *) (Hob.Raw);
  SmramHobDescriptorBlock->NumberOfSmmReservedRegions = SmramRanges;

  SmramIndex = 0;
  for (Index = 0; Index < NumRanges; Index++) {
    if ((
        (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) ||
        (MemoryMap[Index].Type == DualChannelDdrSmramNonCacheable)
        ) &&
        (MemoryMap[Index].PhysicalAddress  != MC_ABSEG_HSEG_PHYSICAL_START)) {

        //
        // This is an SMRAM range (not reporting AB_SEG or H_SEG, create an SMRAM descriptor
        //
        SmramDescriptor = &SmramHobDescriptorBlock->Descriptor[SmramIndex];
        SmramDescriptor->PhysicalStart = MemoryMap[Index].PhysicalAddress;
        SmramDescriptor->CpuStart      = MemoryMap[Index].CpuAddress;
        
        //
        // RangeLength includes alignment adjustment.
        //
//[-start-121113-IB06150257-modify]//
//        if (SmramDescriptor->PhysicalStart < 0x100000) {
          SmramDescriptor->PhysicalSize  = MemoryMap[Index].RangeLength;
//        } else {
//          SmramDescriptor->PhysicalSize  = (Inputs->TsegSize - Inputs->IedSize )* 0x100000;
//        }
//[-end-121113-IB06150257-modify]//

        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TSeg base is %Xh\n", SmramDescriptor->PhysicalStart);
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TSeg SMRAM size is %Xh\n", SmramDescriptor->PhysicalSize);

        if (MemoryMap[Index].Type == DualChannelDdrSmramCacheable) {
          SmramDescriptor->RegionState = EFI_SMRAM_CLOSED | EFI_CACHEABLE;
        } else {
          SmramDescriptor->RegionState = EFI_SMRAM_CLOSED;
        }
        SmramIndex++;
    }
  }

  return EFI_SUCCESS;
}

/**
  Determine the memory size desired based on HOB memory information.

  @param[in]      PeiServices - PEI Services table.
  @param[in, out] Size        - The memory size to return.

  @retval Nothing.
**/
void
RetrieveRequiredMemorySize (
  IN     EFI_PEI_SERVICES **PeiServices,
  IN OUT UINTN            *Size
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_HOB_POINTERS        Hob;
  EFI_MEMORY_TYPE_INFORMATION *MemoryData;
  UINT8                       Index;
  UINTN                       TempPageNum;

  *Size      = 0;
  MemoryData = NULL;
  Status     = (*PeiServices)->GetHobList ((CONST EFI_PEI_SERVICES **)PeiServices, (VOID **) &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
      CompareGuid (&Hob.Guid->Name, &gEfiMemoryTypeInformationGuid)) {
      MemoryData = (EFI_MEMORY_TYPE_INFORMATION *) (Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID));
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }
  //
  // Platform PEIM should supply the information. Generic PEIM doesn't assume any default value.
  //
  if (MemoryData == NULL) {
    return;
  }

  TempPageNum = 0;
  for (Index = 0; MemoryData[Index].Type != EfiMaxMemoryType; Index++) {
    //
    // Accumulate default memory size requirements
    //
    TempPageNum += MemoryData[Index].NumberOfPages;
  }

  if (TempPageNum == 0) {
    return;
  }
  //
  // 16 additional pages are used by DXE memory manager.
  //
  *Size = (TempPageNum + 16) * EFI_PAGE_SIZE;

  return;
}

/**
  Determine the Total DPR memory size needed based on the DPR directory in the SA Data HOB.

  @param[in]      PeiServices - PEI Services table.
  @param[in, out] Size        - The memory size in MB to return.

  @retval Nothing.
**/
void
CalculateTotalDprMemorySize (
  IN     EFI_PEI_SERVICES **PeiServices,
  IN OUT UINT8            *Size
  )
{
  UINT8                DprEntryIndex;
  SA_DATA_HOB          *SaDataHob;
  DPR_DIRECTORY_ENTRY  *DirectoryEntry;

  *Size           = 0;
  DprEntryIndex   = 0;
  DirectoryEntry  = NULL;
  SaDataHob       = NULL;

  SaDataHob = GetFirstGuidHob (&gSaDataHobGuid);
  if (SaDataHob != NULL) {
    DirectoryEntry = SaDataHob->DprDirectory;
    while(DprEntryIndex < DPR_DIRECTORY_MAX){
      *Size += DirectoryEntry->Size;
      DirectoryEntry++;
      DprEntryIndex++;
    }
  }
  return;
}

/**
  Calculates the bases for each technology consuming the DPR region
  and updates the SA Data HOB with the appropriate values in the Dpr
  directory

  @param[in] PeiServices    - PEI Services table.
  @param[in] Base           - The memory base to return.
  @param[in] TotalDprSizeMB - The total DPR size in MB

  @retval Nothing.
**/
void
UpdateDprHobInfo (
  IN EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PHYSICAL_ADDRESS  Base,
  IN UINT8                 TotalDprSizeMB
  )
{
  UINT32               TopOfDpr;
  UINT8                DprEntryIndex;
  SA_DATA_HOB          *SaDataHob;
  DPR_DIRECTORY_ENTRY  *DirectoryEntry;

  DprEntryIndex   = 0;
  DirectoryEntry  = NULL;
  SaDataHob       = NULL;
  TopOfDpr        = (UINT32) Base + (UINT32) LShiftU64(TotalDprSizeMB, 20);

  SaDataHob = GetFirstGuidHob (&gSaDataHobGuid);
  if (SaDataHob != NULL) {
    DirectoryEntry = SaDataHob->DprDirectory;
    while(DprEntryIndex < DPR_DIRECTORY_MAX){
      switch (DirectoryEntry->Type) {
        case DPR_DIRECTORY_TYPE_TXT:
          DirectoryEntry->PhysBase = (UINT32) TopOfDpr - (UINT32) LShiftU64(DirectoryEntry->Size, 20);
          break;
        case DPR_DIRECTORY_TYPE_PFAT:
          DirectoryEntry->PhysBase = (UINT32) Base;
          break;
        default:
          break;
      }
      DirectoryEntry++;
      DprEntryIndex++;
    }
  }
  return;
}

/**
  Determine the memory size desired by GDXC

  @param[in]      PeiServices - PEI Services table.
  @param[in, out] MotSize     - The MOT memory size
  @param[in, out] GdxcSize    - The GDXC memory size

  @retval Nothing.
**/
void
RetrieveGdxcMemorySize (
  IN  EFI_PEI_SERVICES **PeiServices,
  OUT UINT64            *MotSize,
  OUT UINT64            *GdxcSize
  )
{
  UINT32                              MchBar;
  UINT32                              GdxcBar;
  UINT32                              TempMotSize;
  MPCOHTRK_CR_GDXC_MOT_REGION_STRUCT  MotRange;
  MPCOHTRK_CR_GDXC_OCLA_REGION_STRUCT OclaRange;

  *MotSize  = 0;
  *GdxcSize = 0;

  //
  // Get MchBAR
  //
  MchBar = McD0PciCfg32 (R_SA_MCHBAR) & B_SA_MCHBAR_MCHBAR_MASK;
  //
  // Get GdxcBar
  //
  MrcOemMmioRead (NCDECS_CR_GDXCBAR_NCU_REG, (U32 *) &GdxcBar, MchBar);
  GdxcBar &= NCDECS_CR_GDXCBAR_NCU_MAX;
  //
  // Determine Gdxc size: Includes MOT\PSMI\IOT (OCLA)
  //
  MrcOemMmioRead(MPCOHTRK_CR_GDXC_MOT_REGION_REG, (U32 *) &MotRange, GdxcBar);
  TempMotSize = MotRange.Bits.END_ADDRESS - MotRange.Bits.START_ADDRESS;
  if (TempMotSize > 0) {
    *GdxcSize = *MotSize = MrcOemMemoryLeftShiftU64 ((UINT64) (TempMotSize + 1), 23);
  }

  MrcOemMmioRead(MPCOHTRK_CR_GDXC_OCLA_REGION_REG, (U32 *) &OclaRange, GdxcBar);
  *GdxcSize += MrcOemMemoryLeftShiftU64 ((UINT64) (OclaRange.Bits.END_ADDRESS - OclaRange.Bits.START_ADDRESS), 23);

  // Add 16MB if some allocated to MOT and/or IOT
  if (*GdxcSize != 0) {
    *GdxcSize += (16 << 20);
  }

  return;
}

/**
  This function returns the memory ranges to be enabled, along with information
  describing how the range should be used. The MemoryMap buffer will be filled in and
  NumRanges will contain the actual number of memory ranges that are to be enabled.

  @param[in]      PeiServices - PEI Services Table.
  @param[in, out] MemoryMap   - Buffer to record details of the memory ranges to be enabled.
  @param[in, out] NumRanges   - On input, this contains the maximum number of memory ranges that
                                can be described in the MemoryMap buffer.

  @retval EFI_SUCCESS          - The function completed successfully.
  @retval EFI_BUFFER_TOO_SMALL - The specified number of ranges is too large.
**/
EFI_STATUS
GetMemoryMap (
  IN     EFI_PEI_SERVICES                      **PeiServices,
  IN OUT PEI_DUAL_CHANNEL_DDR_MEMORY_MAP_RANGE *MemoryMap,
  IN OUT UINT8                                 *NumRanges
  )
{
  BOOLEAN                          EnableSmram;
  EFI_PHYSICAL_ADDRESS             MemorySize;
  EFI_PHYSICAL_ADDRESS             RowLength;
  EFI_PHYSICAL_ADDRESS             AlignedTsegBase;
  EFI_PHYSICAL_ADDRESS             AlignedGdxcBase;
  EFI_STATUS                       Status;
  PEI_MEMORY_RANGE_GRAPHICS_MEMORY GraphicsMemoryMask;
  PEI_MEMORY_RANGE_PCI_MEMORY      PciMemoryMask;
  PEI_MEMORY_RANGE_OPTION_ROM      OptionRomMask;
  PEI_MEMORY_RANGE_SMRAM           SmramMask;
  PEI_MEMORY_RANGE_SMRAM           TsegMask;
  PEI_PLATFORM_MEMORY_RANGE_PPI    *MemoryRangePpi;
  UINT32                           BlockNum;
  UINT8                            EsmramcRegister;
  UINT8                            ExtendedMemoryIndex;
  UINT8                            Index;
  UINT8                            TotalDprSizeMB;
  UINT64                           GdxcRequiredMemSize;
  UINT64                           GdxcMotMemSize;
#ifdef PTT_FLAG
  UINT32                           PttSts;
#endif
  if ((*NumRanges) < MAX_RANGES) {
    return EFI_BUFFER_TOO_SMALL;
  }

  *NumRanges = 0;

  //
  // Get platform memory range service
  //
  Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gPeiPlatformMemoryRangePpiGuid, 0, NULL, (VOID **) &MemoryRangePpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Find out which memory ranges to reserve on this platform
  //
  Status = MemoryRangePpi->ChooseRanges (
    PeiServices,
    MemoryRangePpi,
    &OptionRomMask,
    &SmramMask,
    &GraphicsMemoryMask,
    &PciMemoryMask
    );
  ASSERT_EFI_ERROR (Status);
  //
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "OptionRomMask      = %Xh\n", OptionRomMask);
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "SmramMask          = %Xh\n", SmramMask);
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GraphicsMemoryMask = %Xh\n", GraphicsMemoryMask);
  // MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "PciMemoryMask      = %Xh\n", PciMemoryMask);
  //
  //
  // Generate memory ranges for the memory map.
  //
  EnableSmram     = FALSE;
  EsmramcRegister = 0;
  MemorySize      = 0;
  Index           = 0;

  //
  // Get the current "max usable memory" address from TOLUD because we will not
  // support any memory above 4Gig.  Will ignore the memory between 4G and TOUUD.
  //
  RowLength = McD0PciCfg32 (R_SA_TOLUD) & B_SA_TOLUD_TOLUD_MASK;

  //
  // System is very unlikely to work with less than 32MB
  //
  ASSERT (RowLength >= (32 * 1024 * 1024));

  //
  // Add memory below 640KB to the memory map. Make sure memory between
  // 640KB and 1MB are reserved, even if not used for SMRAM
  //
  MemoryMap[*NumRanges].RowNumber       = Index;
  MemoryMap[*NumRanges].PhysicalAddress = MemorySize;
  MemoryMap[*NumRanges].CpuAddress      = MemorySize;
  MemoryMap[*NumRanges].RangeLength     = 0xA0000;
  MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
  (*NumRanges)++;

  //
  // Reserve ABSEG or HSEG SMRAM if needed
  //
  if (SmramMask & (PEI_MR_SMRAM_ABSEG_MASK | PEI_MR_SMRAM_HSEG_MASK)) {
    EnableSmram = TRUE;
    MemoryMap[*NumRanges].PhysicalAddress = MC_ABSEG_HSEG_PHYSICAL_START;
    MemoryMap[*NumRanges].RangeLength     = MC_ABSEG_HSEG_LENGTH;
    MemoryMap[*NumRanges].CpuAddress      = (SmramMask & PEI_MR_SMRAM_ABSEG_MASK) ?
      MC_ABSEG_CPU_START :
      MC_HSEG_CPU_START;
    //
    // Chipset only supports cacheable SMRAM.
    //
    MemoryMap[*NumRanges].Type = DualChannelDdrSmramNonCacheable;
  }
  else {
    //
    // Just mark this range reserved.
    //
    MemoryMap[*NumRanges].PhysicalAddress = MC_ABSEG_HSEG_PHYSICAL_START;
    MemoryMap[*NumRanges].CpuAddress      = MC_ABSEG_CPU_START;
    MemoryMap[*NumRanges].RangeLength     = 0x60000;
    MemoryMap[*NumRanges].Type            = DualChannelDdrReservedMemory;
  }

  MemoryMap[*NumRanges].RowNumber = Index;
  (*NumRanges)++;

  RowLength -= 0x100000;
  MemorySize = 0x100000;

  //
  // Add remaining memory to the memory map.
  //
  if (RowLength > 0) {
    MemoryMap[*NumRanges].RowNumber       = Index;
    MemoryMap[*NumRanges].PhysicalAddress = MemorySize;
    MemoryMap[*NumRanges].CpuAddress      = MemorySize;
    MemoryMap[*NumRanges].RangeLength     = RowLength;
    MemoryMap[*NumRanges].Type            = DualChannelDdrMainMemory;
    (*NumRanges)++;
    MemorySize += RowLength;
  }

  ExtendedMemoryIndex = (UINT8) (*NumRanges - 1);

  //
  // See if we need to trim Graphics Memory out of the highest memory range.
  //
  if (GraphicsMemoryMask != PEI_MR_GRAPHICS_MEMORY_NONE) {
    //
    // Create the new range for Graphics Memory from the previous SdrDdrMainMemory range.
    //
    MemoryMap[*NumRanges].RangeLength           = ((GraphicsMemoryMask & PEI_MR_GRAPHICS_MEMORY_SIZE_MASK) * 512 * 1024);
    MemoryMap[*NumRanges].RowNumber             = MemoryMap[ExtendedMemoryIndex].RowNumber;
    MemorySize                                 -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].PhysicalAddress       = MemorySize;
    MemoryMap[*NumRanges].CpuAddress            = MemorySize;
    MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].Type                  = (GraphicsMemoryMask & PEI_MR_GRAPHICS_MEMORY_CACHEABLE) ?
      DualChannelDdrGraphicsMemoryCacheable : DualChannelDdrGraphicsMemoryNonCacheable;

    (*NumRanges)++;
  }
  //
  // See if we need to trim TSEG out of the highest memory range.
  //
  if (SmramMask & PEI_MR_SMRAM_TSEG_MASK) {
    //
    // Create the new range for TSEG and remove that range from the previous SdrDdrMainMemory range.
    //
    TsegMask = (SmramMask & PEI_MR_SMRAM_SIZE_MASK);

    BlockNum = 1;
    while (TsegMask) {
      TsegMask >>= 1;
      BlockNum <<= 1;
    }

    BlockNum >>= 1;

    switch (BlockNum) {
    case PEI_MR_SMRAM_SIZE_1024K_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_2048K_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_8192K_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_16384K_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_32768K_MASK:
      break;

    case PEI_MR_SMRAM_SIZE_65536K_MASK:
      break;

    default:
      //
      // Non supported size. Set to 0.
      //
      BlockNum = 0;
      break;
    }

    if (BlockNum) {
      EnableSmram                       = TRUE;

      MemoryMap[*NumRanges].RangeLength = (BlockNum * 128 * 1024);
      MemoryMap[*NumRanges].RowNumber   = MemoryMap[ExtendedMemoryIndex].RowNumber;
      MemorySize                       -= MemoryMap[*NumRanges].RangeLength;

      //
      // MRC aligns TSEG base on 8MB boundary.
      // Need to adjust memory map accordingly.
      //
      AlignedTsegBase                             = MemorySize & ~(MemoryMap[*NumRanges].RangeLength - 1);
      MemoryMap[*NumRanges].RangeLength          += (MemorySize - AlignedTsegBase);
      MemorySize                                  = AlignedTsegBase;
      MemoryMap[*NumRanges].PhysicalAddress       = AlignedTsegBase;
      MemoryMap[*NumRanges].CpuAddress            = AlignedTsegBase;
      MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
    }
    //
    // Chipset only supports cacheable SMRAM.
    //
    MemoryMap[*NumRanges].Type = DualChannelDdrSmramCacheable;

    (*NumRanges)++;
  }

  //
  // Turn on SMRAM if required.
  //
  if (EnableSmram) {
    McD0PciCfg8Or (R_SA_SMRAMC, B_SA_SMRAMC_G_SMRAME_MASK);
  }

  //
  // Reserve DPR based on Total size required by all technologies using DPR
  //
  CalculateTotalDprMemorySize (PeiServices, &TotalDprSizeMB);

  if (TotalDprSizeMB != 0) {

    MemoryMap[*NumRanges].RangeLength           = (UINT64) LShiftU64 (TotalDprSizeMB, 20);
    MemoryMap[*NumRanges].RowNumber             = MemoryMap[ExtendedMemoryIndex].RowNumber;
    MemorySize                                 -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].PhysicalAddress       = MemorySize;
    MemoryMap[*NumRanges].CpuAddress            = MemorySize;
    MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].Type                  = DualChannelDdrReservedMemory;

    UpdateDprHobInfo (PeiServices, MemorySize, TotalDprSizeMB);

    (*NumRanges)++;
  }

//[-start-121204-IB03600530-modify]//
//[-start-121119-IB04770248-modify]//
  //
  // Reserve GDXC
  //
  RetrieveGdxcMemorySize (PeiServices, &GdxcMotMemSize, &GdxcRequiredMemSize);

  if (GdxcRequiredMemSize) {
    MemoryMap[*NumRanges].RangeLength           = GdxcMotMemSize;
    MemoryMap[*NumRanges].RowNumber             = MemoryMap[ExtendedMemoryIndex].RowNumber;
    MemorySize                                 -= MemoryMap[*NumRanges].RangeLength;

    //
    // MRC aligns Mot base on 16MB boundary.
    // Need to adjust memory map accordingly.
    //
    AlignedGdxcBase                             = MemorySize &~(MRC_BIT24 - 1);
    //
    // Now subtract rest of GdxcRequiredMemsize - GdxcMotMemSize
    //
    AlignedGdxcBase                            -= GdxcRequiredMemSize - GdxcMotMemSize;
    MemoryMap[*NumRanges].RangeLength          += (MemorySize - AlignedGdxcBase);
    MemorySize                                  = AlignedGdxcBase;
    MemoryMap[*NumRanges].PhysicalAddress       = AlignedGdxcBase;
    MemoryMap[*NumRanges].CpuAddress            = AlignedGdxcBase;
    MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
    MemoryMap[*NumRanges].Type                  = DualChannelDdrReservedMemory;

    (*NumRanges)++;
  }

#ifdef PTT_FLAG
  if (GetCpuFamily() == cmHSW_ULT) {
    MrcOemMmioRead (R_PTT_HCI_STS, (U32 *) &PttSts, R_PTT_HCI_BASE_ADDRESS);
    if ((PttSts & B_PTT_HCI_STS_ENABLED) == B_PTT_HCI_STS_ENABLED) {
      MemoryMap[*NumRanges].RangeLength           = 0x1000000;
      MemoryMap[*NumRanges].RowNumber             = MemoryMap[ExtendedMemoryIndex].RowNumber;
      MemorySize                                 -= MemoryMap[*NumRanges].RangeLength;
      MemoryMap[*NumRanges].PhysicalAddress       = MemorySize;
      MemoryMap[*NumRanges].CpuAddress            = MemorySize;
      MemoryMap[ExtendedMemoryIndex].RangeLength -= MemoryMap[*NumRanges].RangeLength;
      MemoryMap[*NumRanges].Type                  = DualChannelDdrReservedMemory;

      (*NumRanges)++;
    }
  }
#endif
//[-end-121119-IB04770248-modify]//
//[-end-121204-IB03600530-modify]//

  return EFI_SUCCESS;
}

/**
  This function returns a pointer to the allocated hand off buffer.

  @param[in]      PeiServices - A pointer to the EFI PEI services table
  @param[in, out] Hob         - A pointer to where to store the pointer to the allocated data buffer.
  @param[in]      Size        - The size of the buffer to get.

  @retval EFI_SUCCESS - Hob is successfully built.
  @retval Others      - Error occured while creating the Hob.
**/
EFI_STATUS
MrcGetHobForDataStorage (
  IN     EFI_PEI_SERVICES       **PeiServices,
  IN OUT HOB_SAVE_MEMORY_DATA   **Hob,
  IN     UINT16                 BlockSize
  )
{
  EFI_STATUS     Status;

  Status = (*PeiServices)->CreateHob ((CONST EFI_PEI_SERVICES **)PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, BlockSize, (VOID **) Hob);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  (*Hob)->EfiHobGuidType.Name = gMemRestoreDataGuid;
  ZeroMem (&((*Hob)->MrcData), sizeof (MrcParameters));
  return EFI_SUCCESS;
}

/**
  A small memory test to quickly point out severe memory issues.

  @param[in] Inputs - Pointer to the MRC Input data structure

  @retval mrcFail on failure, otherwise mrcSuccess.
**/
MrcStatus
BasicMemoryTest (
  IN const MrcInput * const Inputs
  )
{
  const UINT32    BlockSize = 0x1000;
  UINT8           *Addr;
  UINT8           Pattern;
  UINT8           Value;
  UINTN           LoopCount;
  const MrcDebug  *Debug;

  Debug = &Inputs->Debug;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Normal mode memory test started.\n");

  Addr    = 0;
  Pattern = 0;
  while ((UINT32) Addr < BlockSize) {
    *Addr = Pattern++;
    Addr++;
  }

  for (LoopCount = 0; LoopCount < 20; LoopCount++) {
    Addr    = 0;
    Pattern = 0;
    while ((UINT32) Addr < BlockSize) {
      Value = *Addr;
      if (Value != Pattern) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "!!! Normal mode memory test FAILED !!!\n");
        MRC_DEBUG_MSG (
          Debug,
          MSG_LEVEL_ERROR,
          "Address: %Xh, Expected data: %Xh, Actual data: %Xh.\n",
          Addr,
          Pattern,
          Value
          );
        return mrcFail;
      }
      Addr++;
      Pattern++;
    }
  }

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Normal mode memory test passed.\n");
  return mrcSuccess;
}

#ifndef TXT_SUPPORT
/**
  Determines whether or not the platform has executed a TXT launch by
  examining the TPM Establishment bit.

  @param[in] PeiServices - General purpose services available to every PEIM.

  @retval TRUE        - If the TPM establishment bit is asserted.
  @retval FALSE       - If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsEstablishmentBitAsserted (
  IN EFI_PEI_SERVICES **PeiServices
  )
{
  UINT8         Access;
  UINT16        TimeOutCount;
  EFI_STATUS    Status;
  EFI_PEI_STALL_PPI *StallPpi;

  Status = (*PeiServices)->LocatePpi ((CONST EFI_PEI_SERVICES **)PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, (VOID **)&StallPpi);
  ASSERT_EFI_ERROR (Status);


  //
  // Set TPM.ACCESS polling timeout about 750ms.
  //
  TimeOutCount = TPM_TIME_OUT;
  do {
    //
    // Read TPM status register
    //

    Access = (*PeiServices)->CpuIo->MemRead8 (
      (CONST EFI_PEI_SERVICES **)PeiServices,
      (*PeiServices)->CpuIo,
      TPM_STATUS_REG_ADDRESS
      );

    //
    // if TPM.Access == 0xFF, TPM is not present.
    //
    if (Access == 0xFF) {
      return FALSE;
    }
    //
    // Check tpmRegValidSts bit before checking establishment bit.
    //
    if ((Access & 0x80) == 0x80) {
      //
      // tpmRegValidSts set, we can check establishment bit now.
      //
      break;
    }
    else {
      //
      // Delay 1ms
      //
      StallPpi->Stall ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, 1000);
    }

    TimeOutCount--;
  } while (TimeOutCount != 0);

  //
  // ValidSts is not set.
  //
  if ((Access & 0x80) != 0x80) {
    return FALSE;
  }
  //
  // The bit we're interested in uses negative logic:
  // If bit 0 == 1 then return False,
  // Else return True.
  //
  return (BOOLEAN) ((Access & 0x1) ? FALSE : TRUE);
}

/**
  Unlock memory when security is set and TxT is not enabled.

  @param[in] MrcData     - Mrc global data.
  @param[in] PeiServices - PEI Services Table.

  @retval Nothing
**/
void
UnlockMemory (
  IN const MrcParameters *const MrcData,
  IN EFI_PEI_SERVICES           **PeiServices
  )
{
  EFI_CPUID_REGISTER Reg;
  UINT32             Data32;
  const MrcDebug     *Debug;

  Debug = &MrcData->SysIn.Inputs.Debug;

  Data32 = 0;
  AsmCpuid (
    EFI_CPUID_VERSION_INFO,
    &Reg.RegEax,
    &Reg.RegEbx,
    &Reg.RegEcx,
    &Reg.RegEdx
    );

  if ((Reg.RegEcx & BIT6)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Processor supports TXT\n");

    Data32 = CheckSmxCapabilities();

    if (Data32 & BIT0) {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Platform / PCH supports TXT\n");
      if (!(IsEstablishmentBitAsserted (PeiServices))) {
        MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Unlock memory\n");
        AsmWriteMsr64 (0x2e6, 0);
      }
    }
    else {
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Platform / PCH does not support TxT\n");
    }
  }
  else {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Processor does not support TxT\n");
  }
}
#endif // TXT_SUPPORT

/**
  Determine whether a cold reset of the platform is required.
  Note that the memory configuration saved data must be valid.

  @param[in] MrcData             - The MRC "global data" area.
  @param[in] SaPlatformPolicyPpi - SA Platform Policy structure.

  @retval TRUE if cold reset is required, otherwise returns FALSE.
**/
BOOLEAN
ColdBootRequired (
  IN const MrcParameters *const MrcData,
  IN SA_PLATFORM_POLICY_PPI     *SaPlatformPolicyPpi
  )
{
  const MrcDebug       *Debug;
  const MrcInput       *Inputs;
  const MrcSaveData    *SaveData;
  MEMORY_CONFIGURATION *MemConfig;
  MrcVersion           Version;
  U32                  CurrentCrc;

  Inputs    = &MrcData->SysIn.Inputs;
  Debug     = &Inputs->Debug;
  SaveData  = &MrcData->SysSave.Save.Data;
  MemConfig = SaPlatformPolicyPpi->MemConfig;

  MrcVersionGet (&Version);
  CurrentCrc   = MrcCalculateCrc32 ((U8 *) MemConfig, sizeof (MEMORY_CONFIGURATION));

  if ((Version.Major != SaveData->Version.Major) ||
      (Version.Minor != SaveData->Version.Minor) ||
      (Version.Rev   != SaveData->Version.Rev)   ||
      (Version.Build != SaveData->Version.Build)) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "MRC change detected, prev. ver. %u.%u.%u.%u, curr. ver. %u.%u.%u.%u\n",
      SaveData->Version.Major,
      SaveData->Version.Minor,
      SaveData->Version.Rev,
      SaveData->Version.Build,
      Version.Major,
      Version.Minor,
      Version.Rev,
      Version.Build
      );
    return TRUE;
  }
  if ((Inputs->CpuModel != SaveData->CpuModel) || (Inputs->CpuStepping != SaveData->CpuStepping)) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "CPU change detected, prev. CPU %x.%x, curr. CPU %x.%x\n",
      SaveData->CpuModel,
      SaveData->CpuStepping,
      Inputs->CpuModel,
      Inputs->CpuStepping
      );
    return TRUE;
  }
  if (CurrentCrc != SaveData->SaMemCfgCrc) {
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "System Agent input parameter change detected, prev. CRC %xh, curr. CRC %xh.\n",
      SaveData->SaMemCfgCrc,
      CurrentCrc
      );

    return TRUE;
  }
  return FALSE;
}

/**
  Set up the MRC input data structure.

  @param[in]  SysBootMode         - Boot mode of the system.
  @param[in]  BootMode            - Boot mode of the Mrc.
  @param[out] Inputs              - Pointer to the Mrc Input data structure.
  @param[in]  PeiServices         - PEI Services Table.
  @param[in]  SaPlatformPolicyPpi - SA Platform Policy structure.

  @retval Always returns mrcSuccess.
**/
MrcBootMode
MrcSetupMrcData (
  IN  const EFI_BOOT_MODE           SysBootMode,
  IN  const MrcBootMode             BootMode,
  OUT MrcInput *const               Inputs,
  IN  EFI_PEI_SERVICES **const      PeiServices,
  IN  SA_PLATFORM_POLICY_PPI *const SaPlatformPolicyPpi
  )
{
  const MEMORY_CONFIGURATION  *MemConfig;
  const MrcDebug              *Debug;
  MrcControllerIn             *ControllerIn;
  MrcChannelIn                *ChannelIn;
  U16                         DeviceId;
  U8                          Controller;
  U8                          Channel;
  U8                          Dimm;

  Debug     = &Inputs->Debug;
  MemConfig = SaPlatformPolicyPpi->MemConfig;

  Inputs->SaMemCfgAddress = (U32) MemConfig;
  Inputs->SaMemCfgSize    = sizeof (MEMORY_CONFIGURATION);
  Inputs->RefClk          = 0;
  Inputs->Ratio           = 0;
  Inputs->VddVoltage      = VDD_INVALID;

  // Setup the memory profile (Standard/XMP/Custom)
  switch (MemConfig->SpdProfileSelected) {
#if (SUPPORT_XMP == SUPPORT)
    case XMPProfile1:
      Inputs->MemoryProfile = (SysBootMode == BOOT_WITH_DEFAULT_SETTINGS) ? STD_PROFILE : XMP_PROFILE1;
      break;
    case XMPProfile2:
      Inputs->MemoryProfile = (SysBootMode == BOOT_WITH_DEFAULT_SETTINGS) ? STD_PROFILE : XMP_PROFILE2;
      break;
#endif // SUPPORT_XMP
    case UserDefined:
      if (SysBootMode == BOOT_WITH_DEFAULT_SETTINGS) {
        Inputs->MemoryProfile = STD_PROFILE;
      } else {
        Inputs->MemoryProfile = USER_PROFILE;
        Inputs->RefClk        = MemConfig->RefClk;
        Inputs->Ratio         = MemConfig->Ratio;
        Inputs->VddVoltage    = MemConfig->DDR3Voltage;
      }
      break;
    case Default:
    default:
      Inputs->MemoryProfile = STD_PROFILE;
      break;
  }

  // Setup the base addresses.
  Inputs->MchBarBaseAddress = SaPlatformPolicyPpi->PlatformData->MchBar;
  Inputs->PciEBaseAddress   = SaPlatformPolicyPpi->PlatformData->PciExpressBar;
  Inputs->SmbusBaseAddress  = SaPlatformPolicyPpi->PlatformData->SmbusBar;
  Inputs->GdxcBaseAddress   = SaPlatformPolicyPpi->PlatformData->GdxcBar;
  Inputs->HpetBaseAddress   = 0xFED00000;

  //
  // MMIO size in MB units (below 4GB)
  //
  Inputs->MmioSize = SaPlatformPolicyPpi->GtConfig->MmioSize;

  //
  // DDR maximum frequency
  //
  Inputs->FreqMax = MemConfig->DdrFreqLimit;

  //
  // TSEG Size in MB units
  //
  Inputs->TsegSize = (SaPlatformPolicyPpi->PlatformData->TsegSize) >> 20;

  //
  // Graphics Stolen Size
  //
  Inputs->GraphicsGttSize            = SaPlatformPolicyPpi->GtConfig->GttSize;
  // IgdDvmt50PreAlloc value 17 represents 1024M memory - WA for GMS limitation of 5 bits.
  if (SaPlatformPolicyPpi->GtConfig->IgdDvmt50PreAlloc == 17) {
    Inputs->GraphicsStolenSize         = 32 * 32;
  } else {
    Inputs->GraphicsStolenSize         = 32 * SaPlatformPolicyPpi->GtConfig->IgdDvmt50PreAlloc;
  }
  Inputs->GfxIsVersatileAcceleration = FALSE;

  //
  //Get RTC time
  //
  MrcOemGetRtcTime(&(Inputs->BaseTime.Seconds),&(Inputs->BaseTime.Minutes),
    &(Inputs->BaseTime.Hours), &(Inputs->BaseTime.DayOfMonth),
    &(Inputs->BaseTime.Month), &(Inputs->BaseTime.Year) );
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "RTC %u/%u/%u %u:%u:%u\n",
    Inputs->BaseTime.Month,   Inputs->BaseTime.DayOfMonth,
    Inputs->BaseTime.Year,    Inputs->BaseTime.Hours,
    Inputs->BaseTime.Minutes, Inputs->BaseTime.Seconds);

  //
  // Get BoardType (Mobile - 0; Desktop/UpServer - 1)
  //
  Inputs->BoardType      = SaPlatformPolicyPpi->PlatformData->UserBd;
  DeviceId = McD0PciCfg16 (R_SA_MC_DEVICE_ID);
  Inputs->MobilePlatform = (IS_SA_DEVICE_ID_MOBILE(DeviceId)) ? TRUE : FALSE;

  MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "BoardType=%d, MobilePlatform=%d\n", Inputs->BoardType, Inputs->MobilePlatform);

  //
  // Get memory voltages requested value.
  //
  Inputs->VddSettleWaitTime = MemConfig->DDR3VoltageWaitTime;
  Inputs->VccIomV           = 1000;      // Assume 1.0 volts

  Inputs->SetRxDqs32        = FALSE;
  Inputs->McLock                        = MemConfig->McLock;

  Inputs->Gdxc.GdxcEnable          = MemConfig->GdxcEnable;    // Enable/disable GDXC support
  Inputs->Gdxc.GdxcIotSize         = MemConfig->GdxcIotSize;   // Value in 8MB
  Inputs->Gdxc.GdxcMotSize         = MemConfig->GdxcMotSize;   // Value in 8MB

  Inputs->MemoryTrace              = MemConfig->MemoryTrace;   // Memory Trace to second DDR channel using Stacked Mode

  //
  // Options for training steps
  //
  Inputs->TrainingEnables.ECT        = MemConfig->ECT;
  Inputs->TrainingEnables.SOT        = MemConfig->SOT;
  Inputs->TrainingEnables.RDMPRT     = MemConfig->RDMPRT;
  Inputs->TrainingEnables.RCVET      = MemConfig->RCVET;
  Inputs->TrainingEnables.JWRL       = MemConfig->JWRL;
  Inputs->TrainingEnables.FWRL       = MemConfig->FWRL;
  Inputs->TrainingEnables.WRTC1D     = MemConfig->WRTC1D;
  Inputs->TrainingEnables.RDTC1D     = MemConfig->RDTC1D;
  Inputs->TrainingEnables.DIMMODTT   = MemConfig->DIMMODTT;
  Inputs->TrainingEnables.WRDST      = MemConfig->WRDST;
  Inputs->TrainingEnables.WREQT      = MemConfig->WREQT;
  Inputs->TrainingEnables.RDODTT     = MemConfig->RDODTT;
  Inputs->TrainingEnables.RDEQT      = MemConfig->RDEQT;
  Inputs->TrainingEnables.RDAPT      = MemConfig->RDAPT;
  Inputs->TrainingEnables.WRTC2D     = MemConfig->WRTC2D;
  Inputs->TrainingEnables.RDTC2D     = MemConfig->RDTC2D;
  Inputs->TrainingEnables.WRVC2D     = MemConfig->WRVC2D;
  Inputs->TrainingEnables.RDVC2D     = MemConfig->RDVC2D;
  Inputs->TrainingEnables.LCT        = MemConfig->LCT;
  Inputs->TrainingEnables.RTL        = MemConfig->RTL;
  Inputs->TrainingEnables.TAT        = MemConfig->TAT;
  Inputs->TrainingEnables.RMT        = MemConfig->RMT;
  Inputs->TrainingEnables.MEMTST     = MemConfig->MEMTST;

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_2) {
    Inputs->TrainingEnables.DIMMODTT1D = MemConfig->DIMMODTT1D;
    Inputs->TrainingEnables.WRSRT      = MemConfig->WRSRT;
    Inputs->TrainingEnables.DIMMRONT   = MemConfig->DIMMRONT;
  } else {
    Inputs->TrainingEnables.DIMMODTT1D = 0;
    Inputs->TrainingEnables.WRSRT      = 0;
    Inputs->TrainingEnables.DIMMRONT   = 1;
  }

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_4) {
    Inputs->TrainingEnables.CMDVC = MemConfig->CMDVC;
    Inputs->PowerDownMode         = MemConfig->PowerDownMode;
    Inputs->PwdwnIdleCounter      = MemConfig->PwdwnIdleCounter;
    Inputs->RankInterleave        = MemConfig->RankInterleave;
    Inputs->EnhancedInterleave    = MemConfig->EnhancedInterleave;
    Inputs->WeaklockEn            = MemConfig->WeaklockEn;
    Inputs->EnCmdRate             = MemConfig->EnCmdRate;
    Inputs->CmdTriStateDis        = MemConfig->CmdTriStateDis;
  } else {
    Inputs->TrainingEnables.CMDVC = 1;
    Inputs->PowerDownMode         = 0xFF;
    Inputs->PwdwnIdleCounter      = 0x40;
    Inputs->RankInterleave        = TRUE;
    Inputs->EnhancedInterleave    = TRUE;
    Inputs->WeaklockEn            = FALSE;
    Inputs->EnCmdRate             = 7;
    Inputs->CmdTriStateDis        = FALSE;
  }

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_5) {
    Inputs->BClkFrequency = (MemConfig->BClkFrequency < (BCLK_DEFAULT - (10 * 1000 * 1000))) ?
      BCLK_DEFAULT : ((MemConfig->BClkFrequency / 1000000) * 1000000);
  } else {
    Inputs->BClkFrequency = BCLK_DEFAULT;
  }

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_6) {
    Inputs->TrainingEnables.ALIASCHK  = MemConfig->ALIASCHK;
  } else {
    Inputs->TrainingEnables.ALIASCHK  = 1;
  }

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_9) {
    Inputs->IedSize             = (SaPlatformPolicyPpi->PlatformData->IedSize) >> 20;
    Inputs->RefreshRate2x       = MemConfig->RefreshRate2x;       // Tells the MRC to enable 2x Refresh.
    Inputs->ChHashEnable        = MemConfig->ChHashEnable;        // Enale/disable CH HASH support
    Inputs->ChHashMask          = MemConfig->ChHashMask;          // Addr bits[19:6] to include in Channel XOR function.
    Inputs->ChHashInterleaveBit = MemConfig->ChHashInterleaveBit; // Valid values are 0 - 3 for BITS 6 -9
  } else {
    Inputs->IedSize             = 0x04;
    Inputs->RefreshRate2x       = FALSE;
    Inputs->ChHashEnable        = TRUE;       // Enale CH HASH support
    Inputs->ChHashMask          = 0x30CE;     // Addr bits[19:6] to include in Channel XOR function.
    Inputs->ChHashInterleaveBit = 1;          // Valid values are 0 - 3 for BITS 6 -9
  }

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_10) {
    //
    // Options for Thermal settings
    //
    Inputs->ThermalEnables.EnableExtts         = MemConfig->EnableExtts;
    Inputs->ThermalEnables.EnableCltm          = MemConfig->EnableCltm;
    Inputs->ThermalEnables.EnableOltm          = MemConfig->EnableOltm;
    Inputs->ThermalEnables.EnablePwrDn         = MemConfig->EnablePwrDn;
#ifdef ULT_FLAG
    if (Inputs->CpuModel == cmHSW_ULT) {
      Inputs->ThermalEnables.EnablePwrDnLpddr = MemConfig->EnablePwrDnLpddr;
    }
#endif // ULT_FLAG
    Inputs->ThermalEnables.Refresh2X           = MemConfig->Refresh2X;
    Inputs->ThermalEnables.LpddrThermalSensor  = MemConfig->LpddrThermalSensor; // LPDDR MR4 temperature reads
    Inputs->ThermalEnables.LockPTMregs         = MemConfig->LockPTMregs;
    Inputs->ThermalEnables.UserPowerWeightsEn  = MemConfig->UserPowerWeightsEn;
    Inputs->ThermalEnables.EnergyScaleFact     = MemConfig->EnergyScaleFact;
    Inputs->ThermalEnables.RaplPwrFl[1]        = MemConfig->RaplPwrFlCh1;
    Inputs->ThermalEnables.RaplPwrFl[0]        = MemConfig->RaplPwrFlCh0;
    Inputs->ThermalEnables.RaplLim2Lock        = MemConfig->RaplLim2Lock;
    Inputs->ThermalEnables.RaplLim2WindX       = MemConfig->RaplLim2WindX;
    Inputs->ThermalEnables.RaplLim2WindY       = MemConfig->RaplLim2WindY;
    Inputs->ThermalEnables.RaplLim2Ena         = MemConfig->RaplLim2Ena;
    Inputs->ThermalEnables.RaplLim2Pwr         = MemConfig->RaplLim2Pwr;
    Inputs->ThermalEnables.RaplLim1WindX       = MemConfig->RaplLim1WindX;
    Inputs->ThermalEnables.RaplLim1WindY       = MemConfig->RaplLim1WindY;
    Inputs->ThermalEnables.RaplLim1Ena         = MemConfig->RaplLim1Ena;
    Inputs->ThermalEnables.RaplLim1Pwr         = MemConfig->RaplLim1Pwr;
    Inputs->ThermalEnables.WarmThreshold[0][0] = MemConfig->WarmThresholdCh0Dimm0;
    Inputs->ThermalEnables.WarmThreshold[0][1] = MemConfig->WarmThresholdCh0Dimm1;
    Inputs->ThermalEnables.WarmThreshold[1][0] = MemConfig->WarmThresholdCh1Dimm0;
    Inputs->ThermalEnables.WarmThreshold[1][1] = MemConfig->WarmThresholdCh1Dimm1;
    Inputs->ThermalEnables.HotThreshold[0][0]  = MemConfig->HotThresholdCh0Dimm0;
    Inputs->ThermalEnables.HotThreshold[0][1]  = MemConfig->HotThresholdCh0Dimm1;
    Inputs->ThermalEnables.HotThreshold[1][0]  = MemConfig->HotThresholdCh1Dimm0;
    Inputs->ThermalEnables.HotThreshold[1][1]  = MemConfig->HotThresholdCh1Dimm1;
    Inputs->ThermalEnables.WarmBudget[0][0]    = MemConfig->WarmBudgetCh0Dimm0;
    Inputs->ThermalEnables.WarmBudget[0][1]    = MemConfig->WarmBudgetCh0Dimm1;
    Inputs->ThermalEnables.WarmBudget[1][0]    = MemConfig->WarmBudgetCh1Dimm0;
    Inputs->ThermalEnables.WarmBudget[1][1]    = MemConfig->WarmBudgetCh1Dimm1;
    Inputs->ThermalEnables.HotBudget[0][0]     = MemConfig->HotBudgetCh0Dimm0;
    Inputs->ThermalEnables.HotBudget[0][1]     = MemConfig->HotBudgetCh0Dimm1;
    Inputs->ThermalEnables.HotBudget[1][0]     = MemConfig->HotBudgetCh1Dimm0;
    Inputs->ThermalEnables.HotBudget[1][1]     = MemConfig->HotBudgetCh1Dimm1;
    Inputs->ThermalEnables.IdleEnergy[0][1]    = MemConfig->IdleEnergyCh0Dimm1;
    Inputs->ThermalEnables.IdleEnergy[0][0]    = MemConfig->IdleEnergyCh0Dimm0;
    Inputs->ThermalEnables.IdleEnergy[1][1]    = MemConfig->IdleEnergyCh1Dimm1;
    Inputs->ThermalEnables.IdleEnergy[1][0]    = MemConfig->IdleEnergyCh1Dimm0;
    Inputs->ThermalEnables.PdEnergy[0][1]      = MemConfig->PdEnergyCh0Dimm1;
    Inputs->ThermalEnables.PdEnergy[0][0]      = MemConfig->PdEnergyCh0Dimm0;
    Inputs->ThermalEnables.PdEnergy[1][1]      = MemConfig->PdEnergyCh1Dimm1;
    Inputs->ThermalEnables.PdEnergy[1][0]      = MemConfig->PdEnergyCh1Dimm0;
    Inputs->ThermalEnables.ActEnergy[0][1]     = MemConfig->ActEnergyCh0Dimm1;
    Inputs->ThermalEnables.ActEnergy[0][0]     = MemConfig->ActEnergyCh0Dimm0;
    Inputs->ThermalEnables.ActEnergy[1][1]     = MemConfig->ActEnergyCh1Dimm1;
    Inputs->ThermalEnables.ActEnergy[1][0]     = MemConfig->ActEnergyCh1Dimm0;
    Inputs->ThermalEnables.RdEnergy[0][1]      = MemConfig->RdEnergyCh0Dimm1;
    Inputs->ThermalEnables.RdEnergy[0][0]      = MemConfig->RdEnergyCh0Dimm0;
    Inputs->ThermalEnables.RdEnergy[1][1]      = MemConfig->RdEnergyCh1Dimm1;
    Inputs->ThermalEnables.RdEnergy[1][0]      = MemConfig->RdEnergyCh1Dimm0;
    Inputs->ThermalEnables.WrEnergy[0][1]      = MemConfig->WrEnergyCh0Dimm1;
    Inputs->ThermalEnables.WrEnergy[0][0]      = MemConfig->WrEnergyCh0Dimm0;
    Inputs->ThermalEnables.WrEnergy[1][1]      = MemConfig->WrEnergyCh1Dimm1;
    Inputs->ThermalEnables.WrEnergy[1][0]      = MemConfig->WrEnergyCh1Dimm0;
    Inputs->ThermalEnables.SrefCfgEna          = MemConfig->SrefCfgEna;
    Inputs->ThermalEnables.SrefCfgIdleTmr      = MemConfig->SrefCfgIdleTmr;
    Inputs->ThermalEnables.ThrtCkeMinDefeat    = MemConfig->ThrtCkeMinDefeat;
    Inputs->ThermalEnables.ThrtCkeMinTmr       = MemConfig->ThrtCkeMinTmr;
#ifdef ULT_FLAG
    if (Inputs->CpuModel == cmHSW_ULT) {
      Inputs->ThermalEnables.ThrtCkeMinDefeatLpddr = MemConfig->ThrtCkeMinDefeatLpddr;
      Inputs->ThermalEnables.ThrtCkeMinTmrLpddr    = MemConfig->ThrtCkeMinTmrLpddr;
    }
#endif // ULT_FLAG
  } else {
    Inputs->ThermalEnables.EnableExtts           = 0;
    Inputs->ThermalEnables.EnableCltm            = 0;
    Inputs->ThermalEnables.EnableOltm            = 0;
    Inputs->ThermalEnables.EnablePwrDn           = 1;
#ifdef ULT_FLAG
    if (Inputs->CpuModel == cmHSW_ULT) {
      Inputs->ThermalEnables.EnablePwrDnLpddr    = 0;
    }
#endif // ULT_FLAG
    Inputs->ThermalEnables.Refresh2X             = 0;
    Inputs->ThermalEnables.LpddrThermalSensor    = 1;        // Enable LPDDR MR4 temperature reads
    Inputs->ThermalEnables.LockPTMregs           = 0;
    Inputs->ThermalEnables.EnergyScaleFact       = 3;
    Inputs->ThermalEnables.RaplLim2Lock          = 0;
    Inputs->ThermalEnables.RaplLim2WindX         = 0;
    Inputs->ThermalEnables.RaplLim2WindY         = 0;
    Inputs->ThermalEnables.RaplLim2Ena           = 0;
    Inputs->ThermalEnables.RaplLim2Pwr           = 0;
    Inputs->ThermalEnables.RaplLim1WindX         = 0;
    Inputs->ThermalEnables.RaplLim1WindY         = 0;
    Inputs->ThermalEnables.RaplLim1Ena           = 0;
    Inputs->ThermalEnables.RaplLim1Pwr           = 0;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      Inputs->ThermalEnables.RaplPwrFl[Channel] = 0;
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        Inputs->ThermalEnables.WarmThreshold[Channel][Dimm] = 0xFF;
        Inputs->ThermalEnables.HotThreshold[Channel][Dimm]  = 0xFF;
        Inputs->ThermalEnables.WarmBudget[Channel][Dimm]    = 0xFF;
        Inputs->ThermalEnables.HotBudget[Channel][Dimm]     = 0xFF;
        Inputs->ThermalEnables.IdleEnergy[Channel][Dimm]    = 0;
        Inputs->ThermalEnables.PdEnergy[Channel][Dimm]      = 0;
        Inputs->ThermalEnables.ActEnergy[Channel][Dimm]     = 0;
        Inputs->ThermalEnables.RdEnergy[Channel][Dimm]      = 0;
        Inputs->ThermalEnables.WrEnergy[Channel][Dimm]      = 0;
      }
    }
    Inputs->ThermalEnables.SrefCfgEna            = 1;
    Inputs->ThermalEnables.SrefCfgIdleTmr        = 0x200;
    Inputs->ThermalEnables.ThrtCkeMinDefeat      = 0;
    Inputs->ThermalEnables.ThrtCkeMinTmr         = 0x30;
#ifdef ULT_FLAG
    if (Inputs->CpuModel == cmHSW_ULT) {
      Inputs->ThermalEnables.ThrtCkeMinDefeatLpddr = 1;
      Inputs->ThermalEnables.ThrtCkeMinTmrLpddr    = 0x40;
    }
#endif //ULT_FLAG
  }

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_11) {
    Inputs->AutoSelfRefreshSupport         = MemConfig->AutoSelfRefreshSupport;
    Inputs->ExtTemperatureSupport          = MemConfig->ExtTemperatureSupport;
    Inputs->MaxRttWr                       = MemConfig->MaxRttWr;
  } else {
    Inputs->AutoSelfRefreshSupport = TRUE;
    Inputs->ExtTemperatureSupport  = TRUE;
    Inputs->MaxRttWr = 0;
  }

  if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_12) {
    Inputs->TrainingEnables.RCVENC1D  = MemConfig->RCVENC1D;
  } else {
    Inputs->TrainingEnables.RCVENC1D  = 1;
  }

  Inputs->TrainingEnables.RMC = (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_15) ?
                                  MemConfig->RMC : 1;

  Inputs->MrcMode     = MrcModeFull;
  Inputs->Iteration   = 0;

  //
  // Scrambler Suppport.
  //
  Inputs->ScramblerEnable = MemConfig->ScramblerSupport;

  //
  // Remap above 4G Support
  //
  Inputs->RemapEnable = MemConfig->RemapEnable;

  // ECC support.
  Inputs->EccSupport  = MemConfig->EccSupport;

  // RMT BDAT support.
  Inputs->RmtBdatEnable = MemConfig->RmtBdatEnable;


#ifdef ULT_FLAG
  if (Inputs->CpuModel == cmHSW_ULT) {
    //
    // Interleaving mode of DQ/DQS pins - depends on board routing
    //
    Inputs->DqPinsInterleaved = MemConfig->DqPinsInterleaved;

    //
    // DRAM ODT is not used
    //
    Inputs->LpddrDramOdt = 0;

    //
    // Initialize the board-specific CMD/CTL/CLK and DQ/DQS mapping for LPDDR3
    //
    MrcOemLpddrBoardMapping (Inputs, SaPlatformPolicyPpi->PlatformData->BoardId);
  }
#endif // ULT_FLAG

  // Decide which channels and DIMMs are enabled.
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn = &Inputs->Controller[Controller];
    ControllerIn->ChannelCount = 0;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelIn = &ControllerIn->Channel[Channel];
      switch (MemConfig->DisableDimmChannel[Channel]) {
        case 1:
          ChannelIn->Dimm[0].Status = DIMM_DISABLED;
          ChannelIn->Dimm[1].Status = DIMM_ENABLED;
          ChannelIn->Status         = CHANNEL_PRESENT;
          ControllerIn->ChannelCount++;
          ChannelIn->DimmCount      = 1;
          break;
        case 2:
          ChannelIn->Dimm[0].Status = DIMM_ENABLED;
          ChannelIn->Dimm[1].Status = DIMM_DISABLED;
          ChannelIn->Status         = CHANNEL_PRESENT;
          ControllerIn->ChannelCount++;
          ChannelIn->DimmCount      = 1;
          break;
        case 3:
          ChannelIn->Dimm[0].Status = DIMM_DISABLED;
          ChannelIn->Dimm[1].Status = DIMM_DISABLED;
          ChannelIn->Status         = CHANNEL_DISABLED;
          ChannelIn->DimmCount      = 0;
          break;
        default:
          ChannelIn->Dimm[0].Status = DIMM_ENABLED;
          ChannelIn->Dimm[1].Status = DIMM_ENABLED;
          ChannelIn->Status         = CHANNEL_PRESENT;
          ControllerIn->ChannelCount++;
          ChannelIn->DimmCount      = 2;
          break;
      }
    }
  }

  //
  // Get DIMM SpdBaseAddresses.
  //
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn = &Inputs->Controller[Controller];
    for (Channel = 0;  Channel < MAX_CHANNEL; Channel++) {
      ChannelIn = &ControllerIn->Channel[Channel];
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        ChannelIn->Dimm[Dimm].SpdAddress =
          SaPlatformPolicyPpi->PlatformData->SpdAddressTable[(Channel * MAX_DIMMS_IN_CHANNEL) + Dimm];
          /// @todo Need code to detect disabling of individual DIMMs.
      }
    }
  }

#if (defined MEMORY_DOWN_SUPPORT && (MEMORY_DOWN_SUPPORT > 0))
  EnableMemoryDown (Inputs, SaPlatformPolicyPpi->PlatformData->BoardId);
#endif

  switch (BootMode) {
  case bmWarm:
    CheckForTimingOverride (Inputs, SaPlatformPolicyPpi);
    return bmWarm;

  case bmS3:
    CheckForTimingOverride (Inputs, SaPlatformPolicyPpi);
    return bmS3;

  case bmFast:
    //
    // Read SPD data.
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "FAST BOOT GetSpdData\n");
    MrcGetSpdData (BootMode, Inputs);
    CheckForTimingOverride (Inputs, SaPlatformPolicyPpi);
    return bmFast;

  default:
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_ERROR, "ERROR: Invalid flow specified, defaulting to cold flow\n");
    // No break. Note that the boot mode changes to bmCold.

  case bmCold:
    //
    // Read SPD data.
    //
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "COLD BOOT GetSpdData\n");
    MrcGetSpdData (BootMode, Inputs);
    CheckForTimingOverride (Inputs, SaPlatformPolicyPpi);
    break;
  }

  return bmCold;
}

/**
  Check to see if user defined profile is selected and if it is, then copy the
  timing settings for this profile to the timing override structure. If user
  defined profile is not selected, then set the timing override structure to 0.

  Note that even though we set timings on a DIMM by DIMM basis, the controller
  may force DIMM timings to be the same for all DIMMs in a channel.

  @param[in, out] Inputs              - The MRC Input data structure.
  @param[in]      SaPlatformPolicyPpi - The Peim to Peim interface of SaPlatformPolicy.

  @retval Nothing
**/
void
CheckForTimingOverride (
  IN OUT MrcInput *const               Inputs,
  IN     SA_PLATFORM_POLICY_PPI *const SaPlatformPolicyPpi
  )
{
  const MEMORY_CONFIGURATION  *MemConfig;
  MrcControllerIn             *ControllerIn;
  MrcChannelIn                *ChannelIn;
  MrcDimmIn                   *DimmIn;
  U8                          Controller;
  U8                          Channel;
  U8                          Dimm;

  //
  // Override DIMM timing settings for customer profile setting.
  //
  if (Inputs->MemoryProfile == USER_PROFILE) {
    MemConfig = SaPlatformPolicyPpi->MemConfig;
    for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
      ControllerIn = &Inputs->Controller[Controller];
      for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
        ChannelIn = &ControllerIn->Channel[Channel];
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmIn = &ChannelIn->Dimm[Dimm];
          DimmIn->Timing.NMode   = MemConfig->NModeSupport;
          DimmIn->Timing.tCL     = MemConfig->tCL;
          DimmIn->Timing.tCWL    = MemConfig->tCWL;
          DimmIn->Timing.tFAW    = MemConfig->tFAW;
          DimmIn->Timing.tRAS    = MemConfig->tRAS;
          DimmIn->Timing.tRC     = MemConfig->tRC;
          DimmIn->Timing.tRCD    = MemConfig->tRCD;
          DimmIn->Timing.tREFI   = MemConfig->tREFI;
          DimmIn->Timing.tRFC    = MemConfig->tRFC;
          DimmIn->Timing.tRP     = MemConfig->tRP;
          if (SaPlatformPolicyPpi->Revision >= SA_PLATFORM_POLICY_PPI_REVISION_6) {
            DimmIn->Timing.tRPab = MemConfig->tRPab;
          } else {
            DimmIn->Timing.tRPab = 0;
          }
          DimmIn->Timing.tRRD    = MemConfig->tRRD;
          DimmIn->Timing.tRTP    = MemConfig->tRTP;
          DimmIn->Timing.tWR     = MemConfig->tWR;
          DimmIn->Timing.tWTR    = MemConfig->tWTR;
        }
      }
    }
  }

  return;
}
//[-start-121220-IB01830992-add]//
VOID
RemoveSaveMemoryDataHob (
  IN EFI_PEI_SERVICES    **PeiServices
  )
/*++
Routine Description:
  Remove gMemRestoreDataGuid GUID HOB from HOB data base

Arguments:
  PeiServices - The PEI core services table.

Returns:
  None
--*/
{
  EFI_HOB_HANDOFF_INFO_TABLE      *HandOffHob;
  EFI_PEI_HOB_POINTERS            Hob;
  UINT8                           *HobList;
  UINT8                           *MemRestoreDataHobAddr;
  UINT8                           *NextHobAddr;
  UINT8                           *EndHobAddr;
  UINTN                           SizeOfRemainHob;

  //
  // Get the head of HOB list and the Hand Off HOB
  //
  (*PeiServices)->GetHobList ((CONST EFI_PEI_SERVICES **)PeiServices, (VOID**)&HobList);
  HandOffHob = (EFI_HOB_HANDOFF_INFO_TABLE*)HobList;

  //
  // Find gMemRestoreDataGuid HOB
  //
  HobList = (UINT8*)GetFirstGuidHob (&gMemRestoreDataGuid);
  if (HobList != NULL) {
    //
    // If gMemRestoreDataGuid HOB is found, remove it from HOB database
    //
    Hob.Raw = HobList;
    MemRestoreDataHobAddr = HobList;
    //
    // Find the next HOB address
    //
    NextHobAddr = MemRestoreDataHobAddr + Hob.Header->HobLength;
    //
    // Find the end of HOB address
    //
    EndHobAddr = (UINT8*)(UINTN)HandOffHob->EfiFreeMemoryBottom;
    //
    // Calculate how many HOB data should be moved.
    //
    SizeOfRemainHob = (UINTN)EndHobAddr - (UINTN)NextHobAddr;
    //
    // Copy rest of HOB to the address of gMemRestoreDataGuid HOB, override original data which is equal to remove it.
    //
    ((*PeiServices)->CopyMem) (HobList, NextHobAddr, SizeOfRemainHob);
    //
    // After remove HOB, update HOB related information in Hand Off HOB so HOB operation can works properly.
    //
    HandOffHob->EfiFreeMemoryBottom = (EFI_PHYSICAL_ADDRESS) (UINTN)HobList + SizeOfRemainHob;
    HandOffHob->EfiEndOfHobList = HandOffHob->EfiFreeMemoryBottom - sizeof (EFI_HOB_GENERIC_HEADER);
  }
}
//[-end-121220-IB01830992-add]//
