//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c) 1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    FindFv.c

Abstract:

  Framework PEIM to provide the platform support functionality within Windows

--*/

#include "Tiano.h"
#include "Pei.h"
#include "PeiHobLib.h"
#include "EfiHobLib.h"
#include EFI_PPI_DEFINITION (FindFv)

EFI_STATUS
FindFv (
  IN EFI_FIND_FV_PPI                  *This,
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN UINT8                            *FvNumber,
  IN OUT EFI_FIRMWARE_VOLUME_HEADER   **FVAddress
  );

//
// Module globals
//
static EFI_FIND_FV_PPI        mFindFvPpi = { FindFv };

static EFI_PEI_PPI_DESCRIPTOR mPpiListFindFv = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiFindFvPpiGuid,
  &mFindFvPpi
};

EFI_STATUS
EFIAPI
PeimInitializeFindFv (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  );

EFI_PEIM_ENTRY_POINT (PeimInitializeFindFv)

EFI_STATUS
EFIAPI
PeimInitializeFindFv (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Peform the boot mode determination logic

Arguments:

  PeiServices - General purpose services available to every PEIM.

Returns:

  Status -  EFI_SUCCESS if the boot mode could be set

--*/
// GC_TODO:    FfsHeader - add argument and description to function comment
{
  EFI_STATUS  Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListFindFv);

  return Status;
}

EFI_STATUS
FindFv (
  IN     EFI_FIND_FV_PPI             *This,
  IN     EFI_PEI_SERVICES            **PeiServices,
  IN OUT UINT8                       *FvNumber,
  IN OUT EFI_FIRMWARE_VOLUME_HEADER  **FVAddress
  )
/*++

Routine Description:

  Implementation of FindFv PPI

Arguments:

  This         - Pointer to an instance of the Find FV PPI
  PeiServices  - Pointer to PEI Services Table
  FvNumber     - FV instance index indicating which will be retrieved, When the FV
                 instance is found, it will be increased by 1 for next searching
  FVAddress    - Address of the seaching FirmwareVolume

Returns:

  EFI_NOT_FOUND - Not find the appointed FirmwareVolume
  EFI_SUCCESS   - Find the appointed FirmwareVolume successfully

--*/
{
  EFI_STATUS              Status;
  EFI_PEI_HOB_POINTERS    HobStart;
  EFI_PEI_HOB_POINTERS    Hob;
  EFI_HOB_FIRMWARE_VOLUME *FvHob;
  UINT8                   FvIndex;

  if (FVAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Hob.Raw = NULL;

  //
  // Get the Hob table pointer
  //
  Status = (*PeiServices)->GetHobList (
                            PeiServices,
                            &HobStart.Raw
                            );
  ASSERT_PEI_ERROR (PeiServices, Status);

  //
  // Loop to search the wanted FirmwareVolume Hob
  //
  for (FvIndex = 0; FvIndex <= *FvNumber; FvIndex++) {
    Hob.Raw = GetHob (EFI_HOB_TYPE_FV, HobStart.Raw);
    if (Hob.Header->HobType != EFI_HOB_TYPE_FV) {
      *FvNumber = 0;
      return EFI_NOT_FOUND;
    }

    HobStart.Raw = Hob.Raw + Hob.Header->HobLength;
  }

  FvHob       = Hob.FirmwareVolume;

  *FVAddress  = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) FvHob->BaseAddress);
  (*FvNumber)++;

  return EFI_SUCCESS;
}
