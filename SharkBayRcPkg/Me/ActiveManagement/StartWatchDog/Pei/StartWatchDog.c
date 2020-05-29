/** @file

  Start Watchdog timer in PEI phase


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


#include "StartWatchDog.h"

/**
  Perform the platform spefific initializations.

  @param[in] FileHandle           Handle of the file being invoked. 
  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             if the interface could be successfully installed.
**/
EFI_STATUS
EFIAPI
PeiInitStartWatchDog (
  IN       EFI_PEI_FILE_HANDLE         FileHandle,
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS    Status;
  EFI_BOOT_MODE BootMode;
  PEI_HECI_PPI  *HeciPpi;
  UINT32        HeciMemBar;
  UINT16        WaitTimerBios;
  UINT32        MeStatus;

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    return EFI_SUCCESS;
  }

  if (PeiAmtWatchDog (PeiServices)) {
    Status = PeiServicesLocatePpi (
               &gEfiPeiHeciPpiGuid, /// GUID
               0,                   /// INSTANCE
               NULL,                /// EFI_PEI_PPI_DESCRIPTOR
               (VOID **) &HeciPpi   /// PPI
               );
    ASSERT_EFI_ERROR (Status);

    Status = HeciPpi->InitializeHeci (PeiServices, HeciPpi, &HeciMemBar);
    if (!EFI_ERROR (Status)) {
      ///
      /// Get ME Status
      ///
      Status = HeciPpi->GetMeStatus (PeiServices, &MeStatus);
      ASSERT_EFI_ERROR (Status);

      ///
      /// If ME is ready, send AsfStartWatchDog message
      ///
      if (ME_STATUS_ME_STATE_ONLY (MeStatus) == ME_READY) {
        WaitTimerBios = PeiAmtWatchTimerBiosGet (PeiServices);

        Status = PeiHeciAsfStartWatchDog (
                  PeiServices,
                  HeciPpi,
                  HeciMemBar,
                  WaitTimerBios
                  );
        ASSERT_EFI_ERROR (Status);
      }
    }
  }

  return Status;
}
