//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "Kernel.h"

#include EFI_PROTOCOL_DEFINITION (DataHubRecordPolicy)
#include EFI_PROTOCOL_DEFINITION (OemServices)

#define SMBIOS_DATAHUB_RECORD_POLICY_NAME L"SmbiosPolicy"

EFI_DRIVER_ENTRY_POINT (Enter)

EFI_STATUS
Enter (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
/*++
  Routine Description:
    SMBIOS Policy definition Initialization Driver

  Arguments:
    ImageHandle     - Handle for the image of this driver
    SystemTable     - Pointer to the EFI System Table

  Returns:
    EFI_SUCCESS     - Policy decisions set
--*/
{

  EFI_STATUS                    Status;
  UINTN                         DataSize;
  UINTN                         TempDataSize;
  EFI_DATAHUB_RECORD_POLICY     *DataHubRecordPolicy;
  EFI_DATAHUB_RECORD_POLICY     *TempDataHubRecordPolicy;
  OEM_SERVICES_PROTOCOL         *OemServices;

  EfiInitializeDriverLib (ImageHandle, SystemTable);

  //
  //Locate OemServices Protocol
  //
  Status = gBS->LocateProtocol (
                  &gOemServicesProtocolGuid,
                  NULL,
                  &OemServices
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //Get whole table size and default policy
  //
  DataSize = 0;
  Status = OemServices->Funcs[COMMON_DMI_SWITCH_TABLE] (
                          OemServices,
                          COMMON_DMI_SWITCH_TABLE_ARG_COUNT,
                          &DataSize,
                          &DataHubRecordPolicy
                          );
  if (EFI_ERROR (Status) && (Status != EFI_UNSUPPORTED)) {
    return Status;
  }
  //
  //Get SmbiosPolicy variable. If success, copy it.
  //
  if (DataSize != 0) {
    TempDataSize = DataSize;
    TempDataHubRecordPolicy = EfiLibAllocateZeroPool (TempDataSize);
  } else {
    return EFI_INVALID_PARAMETER;
  }
  Status = gRT->GetVariable (
                  SMBIOS_DATAHUB_RECORD_POLICY_NAME,
                  &gEfiDataHubRecordPolicyGuid,
                  NULL,
                  &TempDataSize,
                  (VOID *) TempDataHubRecordPolicy
                  );

  if (EFI_ERROR (Status)) {
    //
    //Failed. First boot or variable crash, so set default policy.
    //
    Status = gRT->SetVariable (
                    SMBIOS_DATAHUB_RECORD_POLICY_NAME,
                    &gEfiDataHubRecordPolicyGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    DataSize,
                    (VOID *) DataHubRecordPolicy
                    );
  } else {
    //
    //Successed
    //
    EfiCopyMem (
      DataHubRecordPolicy,
      TempDataHubRecordPolicy,
      DataSize
      );
  }

  gBS->FreePool (TempDataHubRecordPolicy);

  //
  // Install protocol to allow access this Policy.
  //
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiDataHubRecordPolicyGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID *) DataHubRecordPolicy
                  );

  return Status;
}
