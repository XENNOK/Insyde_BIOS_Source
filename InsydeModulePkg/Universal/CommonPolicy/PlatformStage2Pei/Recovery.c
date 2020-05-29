/** @file
  Provide the functionality of the Ea Recovery Module.
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

#include <Uefi.h>
#include <KernelSetupConfig.h>
#include <Ppi/RecoveryModule.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/VariableLib.h>
#include <Guid/SystemPasswordVariable.h>
#include <Guid/FirmwareFileSystem.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/RecoveryDevice.h>
#include <Guid/AdminPassword.h>
#include <PostCode.h>

EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI            *This
  );

EFI_STATUS
EFIAPI
PlatformRecoveryModuleWithErrorCheck (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI            *This
  );

static EFI_PEI_RECOVERY_MODULE_PPI  mRecoveryPpi = { PlatformRecoveryModuleWithErrorCheck };

static EFI_PEI_PPI_DESCRIPTOR   mRecoveryPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRecoveryModulePpiGuid,
  &mRecoveryPpi
};

EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  )
{
  EFI_STATUS  Status;

  Status = PeiServicesInstallPpi (&mRecoveryPpiList);

  return Status;
}

/**
  Provide the functionality of the Ea Recovery Module.

  @param PeiServices            General purpose services available to every PEIM.
  @param This                   Pointer to EFI_PEI_RECOVERY_MODULE_PPI

  @retval EFI_SUCCESS           if the interface could be successfully installed

**/
EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI            *This
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *DeviceRecoveryModule;
  BOOLEAN                               FoundRecoveryCapsules;
  UINTN                                 NumberRecoveryCapsules;
  UINTN                                 RecoveryCapsuleSize;
  EFI_GUID                              DeviceId;
  BOOLEAN                               ImageFound;
  EFI_PHYSICAL_ADDRESS                  Address;
  VOID                                  *Buffer;
  UINTN                                 Index;
  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader;
  UINTN                                 FvHeaderAddress;
  BOOLEAN                               FindFv;
  VOID                                  *SetupData;
  UINTN                                 VariableSize;
  UINT8                                 *SupervisorPasswordBuf;
  UINT8                                 *UserPasswordBuf;
  UINTN                                 PageNumber;


  Index = 0;
  Status = EFI_SUCCESS;
  FoundRecoveryCapsules = TRUE;
  ImageFound = FALSE;
  DeviceRecoveryModule = NULL;

  POST_CODE (PEI_ENTER_RECOVERY_MODE);
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Recovery Entry\n"));

  //
  // Search the platform for some recovery capsule.
  //
  while (FoundRecoveryCapsules) {

    Status = PeiServicesLocatePpi (
               &gEfiPeiDeviceRecoveryModulePpiGuid,
               Index,
               NULL,
               (VOID **)&DeviceRecoveryModule
               );

    if (!EFI_ERROR (Status)) {
      DEBUG (( EFI_D_INFO | EFI_D_LOAD, "Device Recovery PPI located\n"));

      Status = DeviceRecoveryModule->GetNumberRecoveryCapsules (
                                       PeiServices,
                                       DeviceRecoveryModule,
                                       &NumberRecoveryCapsules
                                       );

      DEBUG (( EFI_D_INFO | EFI_D_LOAD, "Number Of Recovery Capsules: %d\n", NumberRecoveryCapsules));

      if (NumberRecoveryCapsules == 0) {
        Index++;
      } else {
        break;
      }
    } else {
      FoundRecoveryCapsules = FALSE;
      POST_CODE (PEI_RECOVERY_MEDIA_NOT_FOUND);
      DEBUG (( EFI_D_INFO | EFI_D_LOAD, "Recovery LocatePpi False %r\n", Status));
      return Status;
    }
  }
  //
  // If there is an image provider, get the capsule size and ID
  //
  RecoveryCapsuleSize = 0;

  Status = DeviceRecoveryModule->GetRecoveryCapsuleInfo (
                                   PeiServices,
                                   DeviceRecoveryModule,
                                   0,
                                   &RecoveryCapsuleSize,
                                   &DeviceId
                                   );

  if (EFI_ERROR (Status)) {
    POST_CODE (PEI_RECOVERY_MEDIA_NOT_FOUND);
    return Status;
  }

  DEBUG (( EFI_D_INFO | EFI_D_LOAD, "Recovery Capsule Size: %d\n", RecoveryCapsuleSize));

  //
  // Check can updata device.
  //
  if ((!CompareGuid (&DeviceId, &gRecoveryOnFatIdeDiskGuid)) &&
      (!CompareGuid (&DeviceId, &gRecoveryOnDataCdGuid)) &&
      (!CompareGuid (&DeviceId, &gRecoveryOnFatUsbDiskGuid))
      ) {
    return EFI_UNSUPPORTED;
  }

  Buffer = NULL;

  PageNumber = (RecoveryCapsuleSize - 1)/0x1000 + 1;
  Status = PeiServicesAllocatePages (
             EfiBootServicesCode,
             PageNumber,
             &Address
             );

  DEBUG (( EFI_D_INFO | EFI_D_LOAD, "AllocatePage Returns: %r\n", Status));

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Buffer = (UINT8 *) (UINTN) Address;
  POST_CODE (PEI_RECOVERY_MEDIA_FOUND);
  Status = DeviceRecoveryModule->LoadRecoveryCapsule (
                                   PeiServices,
                                   DeviceRecoveryModule,
                                   0,
                                   Buffer
                                   );

  DEBUG (( EFI_D_INFO | EFI_D_LOAD, "LoadRecoveryCapsule Returns: %r\n", Status));

  if (EFI_ERROR (Status)) {
    POST_CODE (PEI_RECOVERY_MEDIA_NOT_FOUND);
    return Status;
  }

  if (PcdGetBool (PcdSecureSysPasswordSupported)) {
    SupervisorPasswordBuf = NULL;
    CommonGetVariableDataAndSize (
      EFI_ADMIN_PASSWORD_NAME,
      &gInsydeSecureFirmwarePasswordGuid,
      &VariableSize,
      (VOID **) &SupervisorPasswordBuf
      );
    if (SupervisorPasswordBuf != NULL) {
      BuildGuidDataHob (
        &gInsydeSecureFirmwarePasswordHobGuid,
        SupervisorPasswordBuf,
        VariableSize
        );
    }
  } else {
    SupervisorPasswordBuf = NULL;
    CommonGetVariableDataAndSize (
      L"SystemSupervisorPw",
      &gEfiSupervisorPwGuid,
      &VariableSize,
      (VOID **) &SupervisorPasswordBuf
      );
    if (SupervisorPasswordBuf != NULL) {
      BuildGuidDataHob (
        &gEfiSupervisorPwHobGuid,
        SupervisorPasswordBuf,
        VariableSize
        );
    }
  }
  UserPasswordBuf = NULL;
  CommonGetVariableDataAndSize (
    L"SystemUserPw",
    &gEfiUserPwGuid,
    &VariableSize,
    (VOID **) &UserPasswordBuf
    );
  if (UserPasswordBuf != NULL) {
    BuildGuidDataHob (
      &gEfiUserPwHobGuid,
      UserPasswordBuf,
      VariableSize
      );
  }
  SetupData = NULL;
  CommonGetVariableDataAndSize (
    L"SetupOrg",
    &gSystemConfigurationGuid,
    &VariableSize,
    (VOID **) &SetupData
    );
  if (SetupData == NULL) {
    SetupData = NULL;
    CommonGetVariableDataAndSize (
      SETUP_VARIABLE_NAME,
      &gSystemConfigurationGuid,
      &VariableSize,
      (VOID **) &SetupData
      );
  }
  if (SetupData != NULL) {
     BuildGuidDataHob (
       &gEfiPowerOnPwSCUHobGuid,
       SetupData,
       VariableSize
       );
  }
  POST_CODE (PEI_RECOVERY_LOAD_FILE_DONE);
  FvHeaderAddress = (UINTN)Buffer;
  do {
    FindFv = FALSE;
    FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)FvHeaderAddress;
    if (FvHeader->Signature == EFI_FVH_SIGNATURE) {
      if (CompareGuid (&gEfiFirmwareFileSystemGuid, &FvHeader->FileSystemGuid)) {
        if (FvHeader->BlockMap[0].NumBlocks == 1) {
          FindFv = TRUE;
          BuildFvHob (FvHeaderAddress, FvHeader->FvLength);
          FvHeaderAddress += (UINTN)FvHeader->FvLength;
        }
      }

      //
      // File System Guid 2
      //
      if (CompareGuid (&gEfiFirmwareFileSystem2Guid, &FvHeader->FileSystemGuid)) {
          FindFv = TRUE;
          BuildFvHob ( FvHeaderAddress, FvHeader->FvLength);
          FvHeaderAddress += (UINTN)FvHeader->FvLength;
      }
    }
    if (!FindFv) {
      FvHeaderAddress += 0x10;
    }
  } while (((FvHeaderAddress - (UINTN)Buffer)+ sizeof (EFI_FIRMWARE_VOLUME_HEADER)) < RecoveryCapsuleSize);

  DEBUG (( EFI_D_INFO | EFI_D_LOAD, "Recovery Module Returning: %r\n", Status));
  return Status;
}

/**
  Provide the functionality of the Ea Recovery Module.

  @param PeiServices            General purpose services available to every PEIM.
  @param This                   Pointer to EFI_PEI_RECOVERY_MODULE_PPI

  @retval EFI_SUCCESS           if the interface could be successfully installed

**/
EFI_STATUS
EFIAPI
PlatformRecoveryModuleWithErrorCheck (
  IN EFI_PEI_SERVICES                       **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI            *This
  )
{
  EFI_STATUS  Status;

  Status = PlatformRecoveryModule (PeiServices, This);
  if (EFI_ERROR (Status)) {
    //
    // OemServices
    //
    if (EFI_ERROR (OemSvcHookWhenRecoveryFail ())) {
      (*PeiServices)->ResetSystem ((CONST EFI_PEI_SERVICES **)PeiServices);
    }
  }

  return Status;
}
