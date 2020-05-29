/** @file
  Main implementation source file for the ActiveBios driver

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
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
  Copyright (c) 2005 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  ActiveBiosMain.c

@brief
  Main implementation source file for the ActiveBios driver
  
**/
#include <ActiveBios.h>

//
// Global data
//
ACTIVE_BIOS_INSTANCE  mPrivateData;
UINT32                mPchRootComplexBar;

/**
  ActiveBios driver entry point function.

  @param[in] ImageHandle          Image handle for this driver image
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_SUCCESS             Application completed successfully
  @exception EFI_UNSUPPORTED      Unsupported chipset detected
**/
EFI_STATUS
EFIAPI
InstallActiveBios (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  Status  = EFI_SUCCESS;
  Handle  = NULL;

  if (!IsPchSupported ()) {
    DEBUG ((EFI_D_ERROR, "Active BIOS Protocol not supported due to no proper PCH LPC found!\n"));
    return EFI_UNSUPPORTED;
  }
  ///
  /// PCH RCBA must be initialized prior to run this driver.
  ///
  mPchRootComplexBar = PCH_RCRB_BASE;
  ASSERT (mPchRootComplexBar != 0);

  ///
  /// Initialize private data
  ///
  mPrivateData.Signature  = ACTIVE_BIOS_SIGNATURE;
  mPrivateData.Handle     = ImageHandle;

  ///
  /// Initialize our ActiveBios protocol
  ///
  Status = ActiveBiosProtocolConstructor (&mPrivateData.ActiveBiosProtocol);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
