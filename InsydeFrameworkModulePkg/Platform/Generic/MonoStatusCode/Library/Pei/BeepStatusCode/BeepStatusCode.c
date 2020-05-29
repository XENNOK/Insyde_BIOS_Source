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
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  BeepStatusCode.c

Abstract:

  Lib to provide beep status code reporting Routines.
  This routine doesn't do anything until the speaker PPI is installed.

--*/

#include "BeepStatusCode.h"

EFI_STATUS
EFIAPI
BeepReportStatusCode (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  Provide a Beep status code

Arguments:

  Same as ReportStatusCode PPI

Returns:

  Status -  EFI_SUCCESS if the interface could be successfully
            installed

--*/
// GC_TODO:    PeiServices - add argument and description to function comment
// GC_TODO:    CodeType - add argument and description to function comment
// GC_TODO:    Value - add argument and description to function comment
// GC_TODO:    Instance - add argument and description to function comment
// GC_TODO:    CallerId - add argument and description to function comment
// GC_TODO:    Data - add argument and description to function comment
{
  EFI_STATUS          Status;
  PEI_SPEAKER_IF_PPI  *SpeakerPpi;
  //
  //  UINT8                 BeepCode;
  //
  // If PeiServices is null, we were called from post-PEI so we just return.
  //
  if (!PeiServices) {
    return EFI_SUCCESS;
  }
  //
  // Locate speaker
  //
  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiSpeakerInterfacePpiGuid,
                            0,
                            NULL,
                            &SpeakerPpi
                            );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  //
  // Only beep for major or greater error codes.
  //
  if (((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) &&
      ((CodeType & EFI_STATUS_CODE_SEVERITY_MASK) != EFI_ERROR_MINOR)
      ) {
    //
    // Output beep code
    //
    PlatformBeepCode (PeiServices, SpeakerPpi, Value);
  }

  return EFI_SUCCESS;
}
