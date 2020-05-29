/** @file
  This module is a sample driver to locate PlatformBinaryProtocol, installing ACPI Windows Platform Binary Table (WPBT).

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/DxeOemSvcKernelLib.h>

#include <Protocol/AcpiWpbt.h>

/**
 Sample Dxe driver entry point

 @param [in]   ImageHandle      EFI_HANDLE
 @param [in]   SystemTable      EFI_SYSTEM_TABLE

 @return Status code

**/
EFI_STATUS
EFIAPI
InstallWpbtDxeEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_ACPI_WPBT_PROTOCOL          *Pb;
  EFI_GUID                        *DefaultFileGuid;
  CHAR16                          *DefaultInputArg;
  UINTN                           ArgLen;

  Status = gBS->LocateProtocol (
                  &gEfiAcpiWpbtProtocolGuid,
                  NULL,
                  (VOID **) &Pb
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get default file GUID and input argument
  //
  DefaultFileGuid = (EFI_GUID *)PcdGetPtr(PcdWpbtFileGuid);
  DefaultInputArg = (CHAR16 *)PcdGetPtr(PcdWpbtInputArg);
  ArgLen = StrSize(DefaultInputArg);

  //
  // OemServices
  //
  Status = OemSvcCustomizeWpbtTable (DefaultFileGuid, DefaultInputArg);
  if (!EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Pass the file guid of binary, the input string and the input length to LocatePlatformBinary
  //
  Status = Pb->LocatePlatformBinary (Pb, DefaultFileGuid, (UINT16)ArgLen, DefaultInputArg);

  return Status;
}
