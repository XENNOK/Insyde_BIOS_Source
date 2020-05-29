/** @file
  ACPI RC FVI DXE driver

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/

#include <Uefi.h>
#include <Uefi/UefiBaseType.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <AcpiFvi.h>


FVI_ELEMENT_AND_FUNCTION mAcpiFviElementsData[] = {
  {
    DEFAULT_FVI_ELEMENT_DATA (ACPI),
    NULL
  }
};

FVI_DATA_HUB_CALLBACK_CONTEXT mAcpiFviVersionData = {
  MISC_SUBCLASS_FVI_HEADER_ENTRY (ACPI),
  mAcpiFviElementsData
};

UINTN mAcpiFviElements = sizeof (mAcpiFviElementsData) / sizeof (FVI_ELEMENT_AND_FUNCTION);
/**
  The entry-point of   ACPI RC FVI DXE driver  

  @param[in] ImageHandle - Standard entry point parameter.
  @param[in] SystemTable - Standard entry point parameter.

  @retval EFI_STATUS
**/

EFI_STATUS
EFIAPI
AcpiFviEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{

  InitFviDataHubCbContext (
    ACPI_FVI_SMBIOS_TYPE,
    (UINT8) mAcpiFviElements,
    &mAcpiFviVersionData
    );

  CreateRcFviDatahub (&mAcpiFviVersionData);

  return EFI_SUCCESS;
}
