//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/*++

Copyright (c)  2007 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformStatusCode.c 
   
Abstract:

  Contains Platform specific implementations required to use status codes.

--*/

//
// Statements that include other files.
//
#include "Tiano.h"
#include "EfiCommonLib.h"
#include "EfiStatusCode.h"
#include "SerialStatusCode.h"
#include "SmmStatusCode.h"

#ifdef EFI_DEBUG_BASE_IO_ADDRESS
  UINT16 gComBase = EFI_DEBUG_BASE_IO_ADDRESS;
#else
  UINT16 gComBase = 0x3f8;
#endif
#ifdef EFI_DEBUG_BAUD_RATE
  UINTN gBps      = EFI_DEBUG_BAUD_RATE;
#else
  UINTN gBps      = 115200;
#endif
  UINT8 gData     = 8;
  UINT8 gStop     = 1;
  UINT8 gParity   = 0;
  UINT8 gBreakSet = 0;

//
// Function implementations
//
EFI_STATUS
EFIAPI
PlatformReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  Call all status code listeners in the MonoStatusCode.

Arguments:

  Same as ReportStatusCode service
  
Returns:

  EFI_SUCCESS     Always returns success.

--*/
{
  DEBUG_CODE (
    SerialReportStatusCode (CodeType, Value, Instance, CallerId, Data);
  );

  return EFI_SUCCESS;
}
