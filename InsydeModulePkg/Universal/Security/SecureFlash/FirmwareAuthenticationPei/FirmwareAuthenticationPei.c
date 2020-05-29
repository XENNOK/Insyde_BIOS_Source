/** @file
  Firmware authentication module for secured crisis recovery
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/FirmwareAuthenticationLib.h>
#include <Ppi/FirmwareAuthentication.h>

FIRMWARE_AUTHENTICATION_PPI mFirmwareAuthPpi = {
  VerifyFirmware
};

EFI_PEI_PPI_DESCRIPTOR mFirmwareAuthPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gFirmwareAuthenticationPpiGuid,
  &mFirmwareAuthPpi
};

EFI_STATUS
EFIAPI
FirmwareAuthenticationPeiEntry (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                        Status;

  if (!EFI_ERROR (PeiServicesRegisterForShadow (FileHandle))) {
    return EFI_SUCCESS;
  }

  Status = PeiServicesInstallPpi (&mFirmwareAuthPpiList);
  return Status;
}
