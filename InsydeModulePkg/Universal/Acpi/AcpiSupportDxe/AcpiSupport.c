/** @file
  This is an implementation of the ACPI Support protocol.
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

/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    AcpiSupport.c

Abstract:

  This is an implementation of the ACPI Support protocol.  This is defined in
  the Tiano ACPI External Product Specification, revision 0.3.6.

--*/

//
// Includes
//
#include "AcpiSupport.h"
#include "InstallUefiTable.h"

//
// Handle to install ACPI Table Protocol
//
EFI_HANDLE                  mHandle = NULL;
EFI_ACPI_SUPPORT_INSTANCE   *mPrivateData = NULL;

//
// Driver entry point
//
/**
 Entry point of the ACPI support driver.
 Creates and initializes an instance of the ACPI Support
 Protocol and installs it on a new handle.

 @param [in]   ImageHandle      A handle for the image that is initializing this driver
 @param [in]   SystemTable      A pointer to the EFI system table

 @retval EFI_SUCCESS:           Driver initialized successfully
 @retval EFI_LOAD_ERROR:        Failed to Initialize or has been loaded
 @retval EFI_OUT_OF_RESOURCES:  Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
InstallAcpiSupport (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_ACPI_SUPPORT_INSTANCE *PrivateData;
  EFI_HANDLE                Handle;
  UINT32                    AcpiVersion;

  //
  // Initialize our protocol
  //
  PrivateData = AllocateZeroPool (sizeof (EFI_ACPI_SUPPORT_INSTANCE));
  if (PrivateData == NULL) {
    ASSERT (PrivateData);
    return EFI_OUT_OF_RESOURCES;
  }
  PrivateData->Signature = EFI_ACPI_SUPPORT_SIGNATURE;

  //
  // Call all constructors per produced protocols
  //
  Status = AcpiSupportAcpiSupportConstructor (PrivateData);
  if (EFI_ERROR (Status)) {
    gBS->FreePool (PrivateData);
    return EFI_LOAD_ERROR;
  }
  //
  // Pass in a NULL to install to a new handle
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiAcpiSupportProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &PrivateData->AcpiSupport
                  );

  if (EFI_ERROR (Status)) {
    gBS->FreePool (PrivateData);
    return EFI_LOAD_ERROR;
  }


  //
  // Install ACPI table protocol
  //
  mPrivateData = PrivateData;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gEfiAcpiTableProtocolGuid,
                  &PrivateData->AcpiTableProtocol,
                  &gEfiAcpiSdtProtocolGuid,
                  &PrivateData->AcpiSdtProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    gBS->FreePool (PrivateData);
    return EFI_LOAD_ERROR;
  }

   AcpiVersion = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
   Status = InstallUefiTable (&PrivateData->AcpiSupport, AcpiVersion);

  return EFI_SUCCESS;
}
