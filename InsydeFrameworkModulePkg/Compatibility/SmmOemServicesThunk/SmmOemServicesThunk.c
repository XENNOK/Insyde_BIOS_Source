/** @file
  Thunk driver for SMM_OEM_SERVICES_PROTOCOL

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SmmOemServicesThunk.h"
#include <OemServices/Kernel.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SmmRuntime.h>

typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            Handle;
  SMM_OEM_SERVICES_PROTOCOL             OemServices;
} SMM_OEM_SERVICES_INSTANCE;

//
// function prototypes
//
EFI_STATUS
NullSmmOemSvcFunc (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

VOID
AssignNullSmmOemSvcFunc (
  IN OUT OEM_SERVICES_FUNCTION          *OemServicesFunc,
  IN UINT32                             NumOfEnum
  );

EFI_STATUS
RegisterSmmOemSvcFunc (
  IN  SMM_OEM_SERVICES_PROTOCOL         *OemServices
  );

//
// helper functions implementation
//

/**
  An empty SMM_OEM_SERVICES_PROTOCOL function.

  @param  *This                 SMM_OEM_SERVICES_PROTOCOL
  @param  NumOfArgs             Number of variable arguments

  @retval EFI_UNSUPPORTED       Returns unsupported by default.
**/
EFI_STATUS
NullSmmOemSvcFunc (
  IN  SMM_OEM_SERVICES_PROTOCOL         *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Assign empty SMM_OEM_SERVICES_PROTOCOL functions by default.

  @param  *OemServicesFunc      Array of SMM_OEM_SERVICES_PROTOCOL functions.
  @param  NumOfEnum             Number of functions in array.

  @retval                       None
**/
VOID
AssignNullSmmOemSvcFunc (
  IN OUT OEM_SERVICES_FUNCTION          *OemServicesFunc,
  IN UINT32                             NumOfEnum
  )
{
  UINT32  Index;

  for (Index = 0; Index < NumOfEnum; Index++) {
    OemServicesFunc[Index] = NullSmmOemSvcFunc;
  }
}

/**
  Register SMM_OEM_SERVICES_PROTOCOL functions.

  @param  *OemServices          SMM_OEM_SERVICES_PROTOCOL

  @retval EFI_SUCCESS           Always return success.
**/
EFI_STATUS
RegisterSmmOemSvcFunc (
  IN  SMM_OEM_SERVICES_PROTOCOL         *OemServices
  )
{
  OemServices->Funcs[COMMON_PNP_UPDATE_STRING_TABLE]         = OemSvcInstallPnpStringTableThunk;
  OemServices->Funcs[COMMON_PNP_GPNV_HANDLE_TABLE]           = OemSvcInstallPnpGpnvTableThunk;
  OemServices->Funcs[COMMON_SMM_S1_CALLBACK]                 = OemSvcS1CallbackThunk;
  OemServices->Funcs[COMMON_SMM_S3_CALLBACK]                 = OemSvcS3CallbackThunk;
  OemServices->Funcs[COMMON_SMM_S4_CALLBACK]                 = OemSvcS4CallbackThunk;
  OemServices->Funcs[COMMON_SMM_S5_CALLBACK]                 = OemSvcS5CallbackThunk;
  OemServices->Funcs[COMMON_SMM_S5_AC_LOSS_CALLBACK]         = OemSvcS5AcLossCallbackThunk;
  OemServices->Funcs[COMMON_SMM_ENABLE_ACPI_CALLBACK]        = OemSvcEnableAcpiCallbackThunk;
  OemServices->Funcs[COMMON_SMM_DISABLE_ACPI_CALLBACK]       = OemSvcDisableAcpiCallbackThunk;
  OemServices->Funcs[COMMON_SMM_RESTORE_ACPI_CALLBACK]       = OemSvcRestoreAcpiCallbackThunk;
  OemServices->Funcs[COMMON_SMM_POWER_BUTTON_CALLBACK]       = OemSvcPowerButtonCallbackThunk;
  OemServices->Funcs[COMMON_SMM_ASPM_OVERRIDE_CALLBACK]      = OemSvcGetAspmOverrideInfoThunk;
  OemServices->Funcs[COMMON_SMM_OS_RESET_CALLBACK]           = OemSvcOsResetCallbackThunk;
  OemServices->Funcs[COMMON_VARIABLE_PRESERVED_TABLE]        = OemSvcVariablePreservedTableThunk;

  return EFI_SUCCESS;
}

/**
  Entry point of the Thunk driver.

  @param  ImageHandle           ImageHandle
  @param  *SystemTable          EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS           Register SMM_OEM_SERVICES_PROTOCOL functions successfully.
  @retval Other                 Failed.
**/
EFI_STATUS
SmmOemSvcThunkEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                            Status;
  SMM_OEM_SERVICES_INSTANCE             *PrivateData;
  SMM_OEM_SERVICES_INSTANCE             *TempPrivateData;
  EFI_SMM_RUNTIME_PROTOCOL              *mSmmRT;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;

  PrivateData = NULL;

  Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &SmmRT);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = SmmRT->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &mSmmRT);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  gBS->AllocatePool (
         EfiReservedMemoryType,
         sizeof (SMM_OEM_SERVICES_INSTANCE),
         (VOID **)&PrivateData
         );

  if (PrivateData != NULL) {

    Status = gBS->AllocatePool (
                    EfiReservedMemoryType,
                    (sizeof (OEM_SERVICES_FUNCTION) * SMM_MAX_NUM_COMMON_SUPPORT),
                    (VOID **)&PrivateData->OemServices.Funcs
                    );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    AssignNullSmmOemSvcFunc (PrivateData->OemServices.Funcs, SMM_MAX_NUM_COMMON_SUPPORT);
  }

  Status = RegisterSmmOemSvcFunc (&PrivateData->OemServices);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SmmRT->InstallProtocolInterface (
               &PrivateData->Handle,
               &gSmmOemServicesProtocolGuid,
               EFI_NATIVE_INTERFACE,
               &PrivateData->OemServices
               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->AllocatePool (
                 EfiReservedMemoryType,
                 sizeof (SMM_OEM_SERVICES_INSTANCE),
                 &TempPrivateData
                 );
  ZeroMem (TempPrivateData, sizeof (SMM_OEM_SERVICES_INSTANCE));
  CopyMem (TempPrivateData, PrivateData, sizeof (SMM_OEM_SERVICES_INSTANCE));
  TempPrivateData->Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &TempPrivateData->Handle,
                  &gSmmOemServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &TempPrivateData->OemServices
                  );

  return Status;
}


