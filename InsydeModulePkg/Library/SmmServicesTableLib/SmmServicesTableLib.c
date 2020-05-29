/** @file
 SMM Services Table Library.

 This file is a constructor for SMM chipset services library.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/
/**
  Copyright (c) 2009 - 2010, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiSmm.h>
#include <Protocol/SmmBase2.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

EFI_SMM_SYSTEM_TABLE2   *gSmst             = NULL;
EFI_SMM_BASE2_PROTOCOL  *mInternalSmmBase2 = NULL;

/**
  The constructor function caches the pointer of SMM Services Table.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SmmServicesTableLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           InSmmMode;

  //
  // Retrieve SMM Base2 Protocol,  Do not use gBS from UefiBootServicesTableLib on purpose
  // to prevent inclusion of gBS, gST, and gImageHandle from SMM Drivers unless the
  // SMM driver explicity declares that dependency.
  //
  Status = SystemTable->BootServices->LocateProtocol (
                           &gEfiSmmBase2ProtocolGuid,
                           NULL,
                           (VOID **)&mInternalSmmBase2
                           );
  if (EFI_ERROR(Status)) {
    return EFI_SUCCESS;
  }
  InSmmMode = FALSE;
  mInternalSmmBase2->InSmm (mInternalSmmBase2, &InSmmMode);
  //
  // Check to see if we are already in SMM
  //
  if (!InSmmMode) {
    //
    // We are not in SMM, so SMST is not needed
    //
    return EFI_SUCCESS;
  }

  //
  // We are in SMM, retrieve the pointer to SMM System Table
  //
  mInternalSmmBase2->GetSmstLocation (mInternalSmmBase2, &gSmst);
  ASSERT (gSmst != NULL);

  return EFI_SUCCESS;
}

/**
  This function allows the caller to determine if the driver is executing in
  System Management Mode(SMM).

  This function returns TRUE if the driver is executing in SMM and FALSE if the
  driver is not executing in SMM.

  @retval  TRUE  The driver is executing in System Management Mode (SMM).
  @retval  FALSE The driver is not executing in System Management Mode (SMM).

**/
BOOLEAN
EFIAPI
InSmm (
  VOID
  )
{
  if (gSmst != NULL) {
    return TRUE;
  } else {
    return FALSE;
  }
}
