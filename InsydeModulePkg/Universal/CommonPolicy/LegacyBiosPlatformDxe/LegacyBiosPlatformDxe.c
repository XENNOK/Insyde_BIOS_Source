/** @file
  The EFI Legacy BIOS Patform Protocol member function

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

#include "LegacyBiosPlatformDxe.h"
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/KernelConfigLib.h>
#include <Protocol/LegacyFree.h>

extern EFI_GUID gEfiAuthenticatedVariableGuid;

LEGACY_BIOS_PLATFORM_INSTANCE           *mPrivateData;


/**
  Retrieve platform specific IRQ Routing Information from ChipsetSvc.

  @retval EFI_UNSUPPORTED         Get Routing Information fail.
  @retval EFI_SUCCESS             Get Routing Information completely.

**/
EFI_STATUS
InitIrqRoutingInformation ();

/**
  Install Driver to produce LegacyBIOSPlatform protocol.

  @param ImageHandle    The firmware allocated handle for the EFI image.
  @param SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The entry point is executed successfully.
  @retval Other         No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
LegacyBiosPlatformDxeEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              Handle;
  UINTN                                   Size;
  UINT8                                   BootType;

  InitIrqRoutingInformation ();

  Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    sizeof (LEGACY_BIOS_PLATFORM_INSTANCE),
                    (VOID **)&mPrivateData
                    );
  ASSERT_EFI_ERROR (Status);

  mPrivateData->Signature = LEGACY_BIOS_PLATFORM_INSTANCE_SIGNATURE;
  mPrivateData->LegacyBiosPlatform.GetPlatformInfo   = GetPlatformInfo;
  mPrivateData->LegacyBiosPlatform.GetPlatformHandle = GetPlatformHandle;
  mPrivateData->LegacyBiosPlatform.SmmInit           = SmmInit;
  mPrivateData->LegacyBiosPlatform.PlatformHooks     = PlatformHooks;
  mPrivateData->LegacyBiosPlatform.GetRoutingTable   = GetRoutingTable;
  mPrivateData->LegacyBiosPlatform.TranslatePirq     = TranslatePirq;
  mPrivateData->LegacyBiosPlatform.PrepareToBoot     = PrepareToBoot;
  mPrivateData->ImageHandle = ImageHandle;

  BootType = DUAL_BOOT_TYPE;
  Size = sizeof (UINT8);
  Status = gRT->GetVariable (
                  L"BootType",
                  &gSystemConfigurationGuid,
                  NULL,
                  &Size,
                  &BootType
                  );
  ASSERT_EFI_ERROR (Status);



  if (BootType == EFI_BOOT_TYPE) {
    //
    // Install the gLegacyFreeProtocolGuid when EFI boot mode
    //
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gLegacyFreeGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);

    return EFI_SUCCESS;
  }

  //
  // Make a new handle and install the protocol
  //
  mPrivateData->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &(mPrivateData->Handle),
                  &gEfiLegacyBiosPlatformProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &(mPrivateData->LegacyBiosPlatform)
                  );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

