/** @file
  Get the proper Boot Mode.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <FastRecoveryData.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/SStateBootMode.h>
#include <Ppi/CpuIo.h>
#include <Ppi/Capsule.h>
#include <Ppi/EmuPei.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiChipsetSvcLib.h>
#include <Library/PrintLib.h>
#include <Library/BvdtLib.h>
#include <Library/SeamlessRecoveryLib.h>
#include <Library/HobLib.h>
#define MAX_STRING_LENGTH     128


//
// Priority of our boot modes, highest priority first
//
static const EFI_BOOT_MODE  mBootModePriority[] = {
  BOOT_IN_RECOVERY_MODE,
  BOOT_ON_FLASH_UPDATE,
  BOOT_ON_S3_RESUME,
  BOOT_ON_S4_RESUME,
  BOOT_WITH_MINIMAL_CONFIGURATION,
  BOOT_ASSUMING_NO_CONFIGURATION_CHANGES,
  BOOT_WITH_FULL_CONFIGURATION,
  BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS,
  BOOT_WITH_DEFAULT_SETTINGS,
  BOOT_ON_S5_RESUME,
  BOOT_ON_S2_RESUME
};

EFI_PEI_PPI_DESCRIPTOR      mPpiListRecoveryBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};

EFI_PEI_PPI_DESCRIPTOR      mPpiListSStateBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSStateBootModePpiGuid,
  NULL
};

STATIC
EFI_STATUS
PrioritizeBootMode (
  IN OUT  EFI_BOOT_MODE     *CurrentBootMode,
  IN      EFI_BOOT_MODE     NewBootMode
  );

STATIC
BOOLEAN
IsBootWithNoChange (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );

BOOLEAN
IsRecoveryDetected (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );


/**
  Convert the capsule GUID to the persistant file path unicode string
  The capsule file name is in the format of
  XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX.bin

  @param[in]      CapsuleGuid           Pointer to the Capsule GUID
  @param[in, out] CapsuleImagePath      The unicode string file path of the Capsule file

  @retval         EFI_SUCCESS           The capsule file name is converted successfully
  @retval         EFI_INVALID_PARAMETER Input parameters are NULL

**/
EFI_STATUS
CapsuleGuidToCapsuleImagePath (
  EFI_GUID       *CapsuleGuid,
  CHAR16         *CapsuleImagePath
  )
{
  if (CapsuleGuid == NULL || CapsuleImagePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  UnicodeSPrint ( CapsuleImagePath,
                  MAX_STRING_LENGTH,
                  L"%s\\%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x.bin",
                  PcdGetPtr(PcdCapsuleImageFolder),
                  (UINTN)CapsuleGuid->Data1,
                  (UINTN)CapsuleGuid->Data2,
                  (UINTN)CapsuleGuid->Data3,
                  (UINTN)CapsuleGuid->Data4[0],
                  (UINTN)CapsuleGuid->Data4[1],
                  (UINTN)CapsuleGuid->Data4[2],
                  (UINTN)CapsuleGuid->Data4[3],
                  (UINTN)CapsuleGuid->Data4[4],
                  (UINTN)CapsuleGuid->Data4[5],
                  (UINTN)CapsuleGuid->Data4[6],
                  (UINTN)CapsuleGuid->Data4[7]
                 );

  return EFI_SUCCESS;
}

/**
  Perform firmware update failure detection

  @param[in]  None

  @retval   Returns TRUE if the previous firmware update failed, otherwise returns FALSE

**/
BOOLEAN
DetectFirmwareUpdateFailure (
  VOID
  )
{
  EFI_STATUS                      Status;
  CHAR16                          RecoveryFile[128];
  EFI_GUID                        FirmwareGuid;
  UINT32                          FirmwareVersion;
  UINTN                           Size;

  if (IsFirmwareFailureRecovery()) {
    //
    // If the previous firmware update is not successful (FirmwareUpdateOngoing variable is not cleared),
    // then update the recovery file path and update BootMode to activate recovery
    //
    Status = GetEsrtFirmwareInfo (&FirmwareGuid, &FirmwareVersion);
    if (EFI_ERROR(Status)) {
      return FALSE;
    }
    CapsuleGuidToCapsuleImagePath(&FirmwareGuid, RecoveryFile);
    Size = StrSize(RecoveryFile);
    PcdSetPtr (PcdPeiRecoveryFile, &Size, RecoveryFile);
    return TRUE;
  }
  return FALSE;

}

/**
  Peform the boot mode determination logic

  @param[in]

  @retval   Peform the boot mode determination logic

**/
EFI_STATUS
EFIAPI
UpdateBootMode (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI   *CpuIo,
  IN       EFI_BOOT_MODE        *BootMode
  )
{
  EFI_STATUS            Status;
  PEI_CAPSULE_PPI       *Capsule;
  BOOLEAN               SkipPriorityPolicy;
  UINT8                 SleepState;

  //
  // Assume boot mode are OK if not told otherwise
  //
  *BootMode = BOOT_WITH_FULL_CONFIGURATION;
  SkipPriorityPolicy = FALSE;
  SleepState = 0;

  //
  // OemServices
  //
  Status = OemSvcChangeBootMode (
             BootMode,
             &SkipPriorityPolicy
             );

  if (!SkipPriorityPolicy) {

    if (IsBootWithNoChange (PeiServices)) {
      Status = PrioritizeBootMode (BootMode, BOOT_ASSUMING_NO_CONFIGURATION_CHANGES);
      ASSERT_EFI_ERROR (Status);
    }

    PeiCsSvcGetSleepState (&SleepState);

    switch (SleepState) {

    case 3:
      Status = PrioritizeBootMode (BootMode, BOOT_ON_S3_RESUME);
      //
      // Determine if we're in capsule update mode
      //
      Status = (**PeiServices).LocatePpi (
                                          PeiServices,
                                          &gPeiCapsulePpiGuid,
                                          0,
                                          NULL,
                                          (VOID **) &Capsule
                                          );
      if (Status == EFI_SUCCESS) {
        Status = Capsule->CheckCapsuleUpdate ((EFI_PEI_SERVICES **)PeiServices);
        if (Status == EFI_SUCCESS) {
          Status = PrioritizeBootMode (BootMode, BOOT_ON_FLASH_UPDATE);
        }
      }
      break;

    case 4:
      Status = PrioritizeBootMode (BootMode, BOOT_ON_S4_RESUME);
      break;

    default:
      break;
    }
    Status = (**PeiServices).SetBootMode (PeiServices, *BootMode);
    ASSERT_EFI_ERROR (Status);
    if (IsRecoveryDetected (PeiServices)) {
      Status = PrioritizeBootMode (BootMode, BOOT_IN_RECOVERY_MODE);
    }
  } // if (!SkipPriorityPolicy)

  Status = (**PeiServices).SetBootMode (PeiServices, *BootMode);
    ASSERT_EFI_ERROR (Status);

  if (*BootMode == BOOT_ON_S3_RESUME) {
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListSStateBootMode);
    ASSERT_EFI_ERROR (Status);
  } else if (*BootMode == BOOT_IN_RECOVERY_MODE) {
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListRecoveryBootMode);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // OemServices
  //
  Status = OemSvcBootModeCreateFv (
            (*BootMode)
            );
  if (Status == EFI_MEDIA_CHANGED) {
    return EFI_SUCCESS;
  }
  if (*BootMode != BOOT_ON_S3_RESUME && *BootMode != BOOT_IN_RECOVERY_MODE) {
    BuildFvHob (
      FixedPcdGet32 (PcdFlashFvMainBase),
      FixedPcdGet32 (PcdFlashFvMainSize)
      );

    if (FeaturePcdGet (PcdUnsignedFvSupported)) {
      BuildFvHob (
        FixedPcdGet32 (PcdFlashUnsignedFvRegionBase),
        FixedPcdGet32 (PcdFlashUnsignedFvRegionSize)
      );
    }

    BuildFvHob (
      FixedPcdGet32 (PcdFlashNvStorageVariableBase),
      (FixedPcdGet32 (PcdFlashNvStorageVariableSize)
       + FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize)
       + FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize)
       + FixedPcdGet32 (PcdFlashNvStorageFactoryCopySize))
    );

  }

  return EFI_SUCCESS;
}

/**
  Given the current boot mode, and a proposed new boot mode, determine
  which has priority. If the new boot mode has higher priority, then
  make it the current boot mode.

  @param[in]

  @retval EFI_SUCCESS     - if both boot mode values were recognized and were processed.

**/
STATIC
EFI_STATUS
PrioritizeBootMode (
  IN OUT EFI_BOOT_MODE    *CurrentBootMode,
  IN EFI_BOOT_MODE        NewBootMode
  )
{
  UINT32  CurrentIndex;
  UINT32  NewIndex;

  //
  // Find the position of the current boot mode in our priority array
  //
  for (CurrentIndex = 0; CurrentIndex < sizeof (mBootModePriority) / sizeof (EFI_BOOT_MODE); CurrentIndex++) {
    if (mBootModePriority[CurrentIndex] == *CurrentBootMode) {
      break;
    }
  }


  if (CurrentIndex >= (sizeof (mBootModePriority) / sizeof (EFI_BOOT_MODE))) {
    return EFI_NOT_FOUND;
  }
  //
  // Find the position of the new boot mode in our priority array
  //
  for (NewIndex = 0; NewIndex < sizeof (mBootModePriority) / sizeof (EFI_BOOT_MODE); NewIndex++) {
    if (mBootModePriority[NewIndex] == NewBootMode) {
      if (NewIndex < CurrentIndex) {
        *CurrentBootMode = NewBootMode;
      }

      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Determine if Boot with no change is true.

  @param[in]  PeiServices    - pointer to the PEI Service Tabl

  @retval     TRUE           - No Change
  @retval     FALSE          - Change

**/
STATIC
BOOLEAN
IsBootWithNoChange (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           IsNoChange;

  IsNoChange = FALSE;

  //
  // TODO : IsNoChange = SmartBoot
  //

  //
  // OemServices
  //
  Status = OemSvcIsBootWithNoChange (
             &IsNoChange
             );

  return (IsNoChange && !EFI_ERROR (Status));
}

BOOLEAN
IsRecoveryDetected (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           IsRecovery = FALSE;

  if (FeaturePcdGet (PcdUseFastCrisisRecovery)) {

    FAST_RECOVERY_DXE_TO_PEI_DATA     *DxeToPeiData;

    Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gEmuPeiPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **)&DxeToPeiData
                                  );
    if (EFI_ERROR(Status)) {
      //
      // We are not in EmuPei Phase.
      //
      DxeToPeiData = NULL;
    }
    //
    // If we can locate the Ppi and get the data, it means we are in emulate PEI
    // phase. We will refer previous PEI/DXE phase's boot mode to determine
    // whether we need to enter recovery mode in current PEI mode.
    //
    if (DxeToPeiData != NULL) {
      EFI_PEI_HOB_POINTERS  Hob;

      Hob.Raw = (UINT8 *)(UINTN)DxeToPeiData->OriginalHobList;
      if ((Hob.Header->HobType == EFI_HOB_TYPE_HANDOFF) &&
          (Hob.HandoffInformationTable->BootMode == BOOT_IN_RECOVERY_MODE)) {
        return TRUE;
      }
    }
  }

  //
  // OemServices
  //
  Status = OemSvcDetectRecoveryRequest (
             &IsRecovery
             );

  if (!IsRecovery) {
    return DetectFirmwareUpdateFailure();
  }

  return (IsRecovery && (EFI_ERROR (Status)));
}

