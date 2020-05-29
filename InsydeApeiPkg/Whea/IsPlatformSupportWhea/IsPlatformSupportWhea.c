/** @file

  IsPlatformSupportWhea driver

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "IsPlatformSupportWhea.h"

/**
 Retrieve setup configuration data

 @param[out] SetupConfig        Pointer to the structure of SYSTEM_CONFIGURATION,
                                this pointer must be allocated with sizeof(SYSTEM_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The kernel configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input SetupConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
GetSetupConfiguration (
  OUT SYSTEM_CONFIGURATION      *SetupConfig
  )
{
  EFI_STATUS Status;
  VOID       *Buffer;
  UINTN      BufferSize;

  DEBUG ((DEBUG_INFO, "[APEI] INFO: %a() Start\n", __FUNCTION__));

  if (SetupConfig == NULL) {
    ASSERT_EFI_ERROR (SetupConfig != NULL);
    return EFI_INVALID_PARAMETER;
  }

  Buffer     = NULL;
  BufferSize = 0;
  Status = gRT->GetVariable (
                  SETUP_VARIABLE_NAME,
                  &gSystemConfigurationGuid,
                  NULL,
                  &BufferSize,
                  Buffer
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      ASSERT_EFI_ERROR (Buffer != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    Status = gRT->GetVariable (
                    SETUP_VARIABLE_NAME,
                    &gSystemConfigurationGuid,
                    NULL,
                    &BufferSize,
                    Buffer
                    );
    if (!EFI_ERROR (Status)) {
      CopyMem (SetupConfig, Buffer, sizeof (SYSTEM_CONFIGURATION));
    }
    ASSERT_EFI_ERROR (Status);
    FreePool (Buffer);
    return Status;
  }

  DEBUG ((DEBUG_INFO, "[APEI] INFO: %a() End\n", __FUNCTION__));

  return Status;
}

/**
  Initializes the IsPlatformSupportWhea Driver.

  @param [in] ImageHandle         Pointer to the loaded image protocol for this driver
  @param [in] SystemTable         Pointer to the EFI System Table

  @retval EFI_SUCCESS             This Platform is support WHEA.
  @retval EFI_UNSUPPORTED         This Platform is not support WHEA.

**/
EFI_STATUS
EFIAPI
InsydeWheaIsPlatformSupportWhea (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  SYSTEM_CONFIGURATION                 SetupConfig;
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  
  if (!FeaturePcdGet (PcdApeiSupport)) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "[APEI] INFO: %a() Start\n", __FUNCTION__));
  // 
  // Get SCU setting about WHEA support,
  // if it is enable, install this protocol for anoter drivers, if not, then just return.
  // To install protocol in order to provide GUID, another drivers may depent on this
  // to decide to work or not.
  //  
  Status = GetSetupConfiguration (&SetupConfig);
  if (!EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "[APEI] INFO: APEI Error Injection setting in SCU (%x)\n", SetupConfig.WheaEinjType));
    DEBUG ((DEBUG_INFO, "[APEI] INFO: APEI Defrag Level setting in SCU (%x)\n", SetupConfig.WheaDefrag));
    DEBUG ((DEBUG_INFO, "[APEI] INFO: ACPI Version setting in SCU (%x)\n", SetupConfig.AcpiVer));
    PcdSetBool (PcdApeiEnable, SetupConfig.WheaSupport);
    PcdSet16 (PcdApeiErrorInjectType, SetupConfig.WheaEinjType);
    PcdSet8 (PcdApeiDefrag, SetupConfig.WheaDefrag);
    PcdSet8 (PcdApeiAcpiVersion, SetupConfig.AcpiVer);
//[-start-140425-IB10310054-add]//
    //
    // WheaErrorRecordFormat Value 0 -> UEFI 2.2 ; value 1 -> uefi 2.3.1
    //
    if (SetupConfig.WheaErrorRecordFormat == 1) {
      PcdSet16 (PcdApeiUefiVersion, 0x0203);
    } else {
      PcdSet16 (PcdApeiUefiVersion, 0x0201);
    }
//[-end-140425-IB10310054-add]//
  } else {
    DEBUG ((DEBUG_INFO, "[APEI] INFO: Get Setup Config fail. (%r)\n", Status));
    DEBUG ((DEBUG_INFO, "[APEI] INFO: Set APEI SCU Config to Default.\n"));
  }

  if (PcdGetBool (PcdApeiEnable)) {
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiIsPlatformSupportWheaGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_INFO, "[APEI] INFO: Install APEI Platform Support Protocol %r\n", Status));
      return Status;
    }
  }
  DEBUG ((DEBUG_INFO, "[APEI] INFO: APEI Error Injection in PCD (%x)\n", PcdGet16 (PcdApeiErrorInjectType)));
  DEBUG ((DEBUG_INFO, "[APEI] INFO: APEI Defrag Level in PCD (%x)\n", PcdGet8 (PcdApeiDefrag)));
  DEBUG ((DEBUG_INFO, "[APEI] INFO: ACPI Version in PCD (%x)\n", PcdGet8 (PcdApeiAcpiVersion)));
//[-start-140425-IB10310054-add]//
  DEBUG ((DEBUG_INFO, "[APEI] INFO: UEFI Version in PCD (%x)\n", PcdGet16 (PcdApeiUefiVersion)));
//[-end-140425-IB10310054-add]//

  DEBUG ((DEBUG_INFO, "[APEI] INFO: %a() End\n", __FUNCTION__));

  return Status;
}
