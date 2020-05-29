/** @file
  This module provides default Setup variable data if Setupvariable is not found
  and also provides a Setup variable cache mechanism in PEI phase

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

#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/SetupUtilityLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/BaseSetupDefaultLib.h>
#include <Library/VariableLib.h>

#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/PeiDefaultSetupProviderHob.h>
#include <Guid/SetupDefaultHob.h>
#include <Ppi/EmuPei.h>
#include <FastRecoveryData.h>
#include "SetupDataProvider.h"
#include <Library/SeamlessRecoveryLib.h>

#ifndef __GNUC__
#pragma pack(push, 16)
IFR_SETUP_DEFAULT IfrSetupDefault = {
                    {'$', 'I', 'F', 'R', 'D', 'F', 'T'},
                    0x02,
                    0x05,
                    { (UINT64) (UINTN) AdvanceVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) PowerVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) MainVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) SecurityVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) BootVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) 0
                    }
};
#pragma pack(pop)
#endif


CHAR16 mSetupVariableName[]    = SETUP_VARIABLE_NAME;
CHAR16 mSetupOrgVariableName[] = L"SetupOrg";


EFI_PEI_READ_ONLY_VARIABLE2_PPI mSetupDataProviderVariablePpi = {
  NewPeiGetVariable,
  NewPeiNextVariableName
};

EFI_PEI_PPI_DESCRIPTOR     mSetupDataProviderVariablePpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  &mSetupDataProviderVariablePpi
};

EFI_STATUS
ReadOnlyVariable2HookCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );
EFI_PEI_NOTIFY_DESCRIPTOR      mVariableNotifyPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiReadOnlyVariable2PpiGuid,
    ReadOnlyVariable2HookCallback
  }
};
/**
  Provide the get next variable functionality of the variable services.

  @param  This              A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.

  @param  VariableNameSize  On entry, points to the size of the buffer pointed to by VariableName.
  @param  VariableName      On entry, a pointer to a null-terminated string that is the variable's name.
                            On return, points to the next variable's null-terminated name string.

  @param  VariableGuid      On entry, a pointer to an EFI_GUID that is the variable's GUID.
                            On return, a pointer to the next variable's GUID.

  @retval EFI_SUCCESS       The interface could be successfully installed
  @retval EFI_NOT_FOUND     The variable could not be discovered

**/
EFI_STATUS
EFIAPI
NewPeiNextVariableName (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VendorGuid
  )
{
  EFI_STATUS                                Status;
  EFI_PEI_HOB_POINTERS                      GuidHob;
  SETUP_DATA_PROVIDER_CONTEXT               *SetupDataProviderContext;

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gPeiDefaultSetupProviderGuid, GuidHob.Raw);
  if (GuidHob.Raw == NULL) {
    return EFI_NOT_FOUND;
  }

  SetupDataProviderContext = (SETUP_DATA_PROVIDER_CONTEXT *)GET_GUID_HOB_DATA (GuidHob.Guid);

  Status = SetupDataProviderContext->VariablePpi->NextVariableName (
                                                    This,
                                                    VariableNameSize,
                                                    VariableName,
                                                    VendorGuid
                                                    );

  return Status;
}

/**
  Find specific variable is whether defined in SetupConfig PCD.

  @param [in]  VariableName       A pointer to a null-terminated string that is the variable's name.
  @param [out] Offset             Offset by bytes in the SYSTEM_CONFIGURATION structure.
  @param [out] DataSize           Variable data size by types..

  @retval EFI_SUCCESS             Find specific variable in setup config PCD successfully.
  @retval EFI_INVALID_PARAMETER   VariableName, Offset or DataSize is NULL.
  @retval EFI_NOT_FOUND           Cannot find specific variable in setup config PCD.
  @retval EFI_UNSUPPORTED         The found variable size is 0.
**/
EFI_STATUS
FindVariableNameInSetupConfigPcd (
  IN  CONST CHAR16       *VariableName,
  OUT       UINT32       *Offset,
  OUT       UINT32       *DataSize
  )
{
  UINTN                 TokenNum;
  CHAR16                *ConfigName;
  UINT32                OffsetAndSize;

  if (VariableName == NULL || Offset == NULL || DataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, 0);
  while (TokenNum != 0) {
    ConfigName = (CHAR16 *) LibPcdGetExPtr(&gSetupConfigNameGuid, TokenNum);
    if (ConfigName != NULL && StrCmp (VariableName, ConfigName) == 0) {
      OffsetAndSize = LibPcdGetEx32 (&gSetupConfigOffsetSizeGuid, TokenNum);
      if ((OffsetAndSize & 0xFFFF) == 0) {
        return EFI_UNSUPPORTED;
      }
      *Offset = (UINT32) ((OffsetAndSize >> 16) & 0xFFFF);
      *DataSize = (UINT32) (OffsetAndSize & 0xFFFF);
      return EFI_SUCCESS;
    }
    TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, TokenNum);
  }

  return EFI_NOT_FOUND;
}
/**
  Provide the revised read variable functionality of the variable services.

  @param  This                  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param  VariableName          A pointer to a null-terminated string that is the variable's name.
  @param  VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                VariableGuid and VariableName must be unique.
  @param  Attributes            If non-NULL, on return, points to the variable's attributes.
  @param  DataSize              On entry, points to the size in bytes of the Data buffer.
                                On return, points to the size of the data returned in Data.
  @param  Data                  Points to the buffer which will hold the returned variable value.

  @retval EFI_SUCCESS           The interface could be successfully installed
  @retval EFI_NOT_FOUND         The variable could not be discovered
  @retval EFI_BUFFER_TOO_SMALL  The caller buffer is not large enough

**/
EFI_STATUS
EFIAPI
NewPeiGetVariable (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI  *This,
  IN CONST CHAR16                           *VariableName,
  IN CONST EFI_GUID                         *VendorGuid,
  OUT UINT32                                *Attributes OPTIONAL,
  IN OUT UINTN                              *DataSize,
  OUT VOID                                  *Data
  )
{
  EFI_PEI_HOB_POINTERS                      GuidHob;
  SETUP_DATA_PROVIDER_CONTEXT               *SetupDataProviderContext;
  UINTN                                     SetupVariableSize;
  UINT32                                    Offset;
  UINT32                                    Size;

  if (VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gPeiDefaultSetupProviderGuid, GuidHob.Raw);
  if (GuidHob.Raw == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  SetupDataProviderContext = (SETUP_DATA_PROVIDER_CONTEXT *)GET_GUID_HOB_DATA (GuidHob.Guid);
  if (SetupDataProviderContext == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  SetupVariableSize = SetupDataProviderContext->SetupDataSize;
  if (CompareGuid ((EFI_GUID *)VendorGuid, &gSystemConfigurationGuid) &&
      (StrCmp (VariableName, mSetupVariableName) == 0)) {

    if (*DataSize < SetupVariableSize) {
      *DataSize = SetupVariableSize;
      return EFI_BUFFER_TOO_SMALL;
    } else if (Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    if (Attributes != NULL) {
      *Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
    }
    CopyMem (Data, SetupDataProviderContext->SetupData, SetupVariableSize);
    return EFI_SUCCESS;
  }

  if (CompareGuid ((EFI_GUID *)VendorGuid, &gSystemConfigurationGuid) &&
    FindVariableNameInSetupConfigPcd (VariableName, &Offset, &Size) == EFI_SUCCESS) {
    if (*DataSize < Size) {
      *DataSize = Size;
      return EFI_BUFFER_TOO_SMALL;
    } else if (Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    if (Attributes != NULL) {
      *Attributes = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
    }
    CopyMem (Data, SetupDataProviderContext->SetupData + Offset, Size);
    *DataSize = Size;
    return EFI_SUCCESS;
  }

  if (CompareGuid ((EFI_GUID *)VendorGuid, &gSystemConfigurationGuid) &&
      (StrCmp (VariableName, mSetupOrgVariableName) == 0)) {
    return SetupDataProviderContext->VariablePpi->GetVariable (
                                                    This,
                                                    mSetupVariableName,
                                                    VendorGuid,
                                                    Attributes,
                                                    DataSize,
                                                    Data
                                                    );

  }

  return SetupDataProviderContext->VariablePpi->GetVariable (
                                                  This,
                                                  VariableName,
                                                  VendorGuid,
                                                  Attributes,
                                                  DataSize,
                                                  Data
                                                  );
}

/**
  Read data from Sensitive setup variable and synchronize these data to input setup config instance.

  @param [in] SetupNvData        Points to CHIPSET_CONFIGURATION instance.

  @retval EFI_SUCCESS            Sync setup data with sensitive variables successfully.
  @retval EFI_INVALID_PARAMETER  SetupNvData is NULL.
**/
EFI_STATUS
SyncSetupDataWithSensitiveVariable (
  IN OUT VOID        *SetupNvData
  )
{
  UINTN                 TokenNum;
  CHAR16                *ConfigName;
  UINT32                OffsetAndSize;
  UINTN                 Offset;
  UINTN                 DataSize;
  UINT8                 *Data;

  if (SetupNvData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TokenNum = LibPcdGetNextToken(&gSetupConfigNameGuid, 0);
  while (TokenNum != 0) {
    ConfigName = (CHAR16 *) LibPcdGetExPtr(&gSetupConfigNameGuid, TokenNum);
    if (ConfigName != NULL) {
      OffsetAndSize = LibPcdGetEx32 (&gSetupConfigOffsetSizeGuid, TokenNum);
      Offset        = (UINTN) ((OffsetAndSize >> 16) & 0xFFFF);
      DataSize      = (UINTN) (OffsetAndSize & 0xFFFF);
      Data = CommonGetVariableData (
                 ConfigName,
                 &gSystemConfigurationGuid
                 );
      if (Data != NULL && DataSize != 0) {
        CopyMem ((UINT8 *) SetupNvData + Offset, Data, DataSize);
        FreePool (Data);
      }
    }
    TokenNum = LibPcdGetNextToken (&gSetupConfigNameGuid, TokenNum);
  }
  return EFI_SUCCESS;
}

/**
  Setup setup default data and hook variable PPI in PEI phase.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Describes the list of possible PEI Services.

 @retval EFI_STATUS

**/
EFI_STATUS
ReadOnlyVariable2HookCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                                Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI           *VariablePpi;
  EFI_PEI_PPI_DESCRIPTOR                    *VariablePpiDescriptor;
  SETUP_DATA_PROVIDER_CONTEXT               *SetupDataProviderContext;
  UINTN                                     VariableSize;
  BOOLEAN                                   IsRecovery;
  UINT8                                     *EmuPeiPpi;

  EFI_PEI_HOB_POINTERS                      GuidHob;
  VOID                                      *SetupDefaultHobData;

  IsRecovery = FALSE;

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             &VariablePpiDescriptor,
             (VOID **)&VariablePpi
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Detect if the system is in recovery mode
  //
  Status = OemSvcDetectRecoveryRequest (&IsRecovery);

  if (FeaturePcdGet (PcdUseFastCrisisRecovery)) {
    Status = PeiServicesLocatePpi (
               &gEmuPeiPpiGuid,
               0,
               NULL,
               (VOID **)&EmuPeiPpi
               );
    if (!EFI_ERROR (Status)) {
      IsRecovery = TRUE;
    }
  }
  if ((!IsRecovery) && IsFirmwareFailureRecovery ()) {
    IsRecovery = TRUE;
  }

  if (IsRecovery) {
    Status = PeiServicesSetBootMode (BOOT_IN_RECOVERY_MODE);
  }

  VariableSize = 0;
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          mSetupVariableName,
                          &gSystemConfigurationGuid,
                          NULL,
                          &VariableSize,
                          NULL
                          );

  if (Status == EFI_BUFFER_TOO_SMALL && !IsRecovery ) {
      return EFI_SUCCESS;
  } else {
    VariableSize = GetSetupVariableSize ();
  }
  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gPeiDefaultSetupProviderGuid, GuidHob.Raw);
  if (GuidHob.Raw != NULL) {
    return EFI_SUCCESS;
  }

  SetupDataProviderContext = BuildGuidHob (
                               &gPeiDefaultSetupProviderGuid,
                               sizeof (SETUP_DATA_PROVIDER_CONTEXT) + VariableSize - 1
                               );
  if (SetupDataProviderContext == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  SetupDataProviderContext->VariablePpi = VariablePpi;
  SetupDataProviderContext->SetupDataSize = VariableSize;

  if (Status == EFI_BUFFER_TOO_SMALL && !IsRecovery) {
    Status = VariablePpi->GetVariable (
                            VariablePpi,
                            mSetupVariableName,
                            &gSystemConfigurationGuid,
                            NULL,
                            &VariableSize,
                            SetupDataProviderContext->SetupData
                            );
    SyncSetupDataWithSensitiveVariable (SetupDataProviderContext->SetupData);
  } else {
    SetMem (SetupDataProviderContext->SetupData, VariableSize, 0);
    ExtractSetupDefault (SetupDataProviderContext->SetupData);

    if (!IsRecovery) {
      SyncSetupDataWithSensitiveVariable (SetupDataProviderContext->SetupData);
    }
    //
    // Create SetupDefaultHob only if Setup variable does not exist or system is in recovery mode
    //
    SetupDefaultHobData = BuildGuidDataHob (
      &gSetupDefaultHobGuid,
      SetupDataProviderContext->SetupData,
      VariableSize
      );

    //
    // Setup variable is not available, set SetupVariableInvalid flag
    //
    PcdSetBool (PcdSetupDataInvalid, TRUE);

    //
    //  SetupDataProviderContext->SetupData should be synchronized with SetupDefaultHobData
    //  to ensure SetupDataInvalid is updated
    //
    CopyMem(SetupDataProviderContext->SetupData, SetupDefaultHobData, VariableSize);
  }

  Status = PeiServicesReInstallPpi (
             VariablePpiDescriptor,
             &mSetupDataProviderVariablePpiList
             );

  return Status;
}

/**
  Setup setup default data and hook variable PPI in PEI phase.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Describes the list of possible PEI Services.

 @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
SetupDataProviderEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  //[-start-140411-IB11590055-add]//
#ifndef __GNUC__
    UINT8 EZH2OVersion;

    //
    // Dummy code to avoid IfrSetupDefault data being removed due to linker optimization
    // Do not remove this line
    //
    EZH2OVersion = IfrSetupDefault.EZH2OVersion;
#endif
  //[-end-140411-IB11590055-add]//

  PeiServicesNotifyPpi (mVariableNotifyPpiList);
  return EFI_SUCCESS;
}
