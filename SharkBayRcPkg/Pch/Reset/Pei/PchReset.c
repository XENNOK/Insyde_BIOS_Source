/** @file
  PCH RESET PEIM DRIVER.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchReset.c

@brief
  PCH RESET PEIM DRIVER.

**/
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <PchReset.h>

/**
  Installs PCH RESET PPI

  @param[in] FfsHeader            Not used.
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             PCH SPI PPI is installed successfully
  @retval EFI_OUT_OF_RESOURCES    Can't allocate pool
**/
EFI_STATUS
InstallPchReset (
  IN EFI_PEI_FILE_HANDLE                   FileHandle,
  IN CONST EFI_PEI_SERVICES                **PeiServices
  )
{
  EFI_STATUS              Status;
  PEI_PCH_RESET_INSTANCE  *PeiPchResetInstance;
  PCH_RESET_INSTANCE      *PchResetInstance;

  DEBUG ((EFI_D_INFO, "InstallPchReset() Start\n"));

  PeiPchResetInstance = (PEI_PCH_RESET_INSTANCE *) AllocateZeroPool (sizeof (PEI_PCH_RESET_INSTANCE));
  if (NULL == PeiPchResetInstance) {
    return EFI_OUT_OF_RESOURCES;
  }

  PchResetInstance = &(PeiPchResetInstance->PchResetInstance);
  PchResetProtocolConstructor (PchResetInstance);

  PeiPchResetInstance->PpiDescriptor.Flags  = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PeiPchResetInstance->PpiDescriptor.Guid   = &gPchResetPpiGuid;
  PeiPchResetInstance->PpiDescriptor.Ppi    = &(PchResetInstance->PchResetProtocol);

  ///
  /// Install the PCH RESET PPI
  ///
  Status = (**PeiServices).InstallPpi (PeiServices, &PeiPchResetInstance->PpiDescriptor);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "PCH RESET PPI Installed\n"));

  DEBUG ((EFI_D_INFO, "InstallPchReset() End\n"));

  return Status;
}

/**
  Execute call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The callback function has been done successfully
  @exception EFI_UNSUPPORTED      Do not do any reset from PCH
**/
EFI_STATUS
PchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  EFI_STATUS              Status;
  UINTN                   Instance;
  PCH_RESET_CALLBACK_PPI  *PchResetCallbackPpi;

  if ((PchResetType == GlobalReset) || (PchResetType == GlobalResetWithEc)) {
    ///
    /// After MRC is done, DRAM Init Done message will be sent to ME FW.
    ///
    Status = PeiServicesLocatePpi (
              &gEfiPeiMemoryDiscoveredPpiGuid,
              0,
              NULL,
              NULL
              );

    if (Status == EFI_SUCCESS) {
      ///
      /// After sending DRAM Init Done to ME FW, please do the global reset through HECI.
      ///
      DEBUG ((EFI_D_ERROR, "Please do the global reset through HECI \n"));
      return EFI_UNSUPPORTED;
    }
  }

  Instance = 0;

  do {
    ///
    /// Those drivers that need to install Pch Reset Callback Ppi have the responsibility
    /// to make sure themselves execute before Pch Reset PEI driver.
    ///
    Status = PeiServicesLocatePpi (
              &gPchResetCallbackPpiGuid,
              Instance,
              NULL,
              (VOID**) &PchResetCallbackPpi
              );

    if (Status == EFI_SUCCESS) {
      PchResetCallbackPpi->ResetCallback (PchResetType);
    } else {
      if ((Instance == 0) && (Status == EFI_NOT_FOUND)) {
        DEBUG ((EFI_D_ERROR | EFI_D_INFO, "None of Pch Reset Callback Ppi is found .\n"));
      } else {
        DEBUG ((EFI_D_INFO, "Failed to locate Pch Reset Callback Ppi.\n"));
      }
    }

    Instance++;
  } while (Status != EFI_NOT_FOUND);

  return EFI_SUCCESS;
}
