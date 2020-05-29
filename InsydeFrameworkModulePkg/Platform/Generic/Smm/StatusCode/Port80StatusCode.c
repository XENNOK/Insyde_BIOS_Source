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
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Port80StatusCode.c

Abstract:

  Lib to provide port 80 status code reporting Routines. This routine
  does not use PPI's but is monolithic.

  In general you should use PPI's, but some times a monolithic driver
  is better. The best justification for monolithic code is debug.

--*/

#include "Port80StatusCode.h"
 
BOOLEAN
EFIAPI
StatusCodeToPostCode (
  IN  EFI_STATUS_CODE_TYPE    CodeType,
  IN  EFI_STATUS_CODE_VALUE   Value,
  OUT UINT8                   *PostCode
  );
 
EFI_STATUS
EFIAPI
Port80ReportStatusCode (
  IN VOID                     *PeiServices,
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  Provide a port 80 status code

Arguments:

  Same as ReportStatusCode PPI

Returns:

  EFI_SUCCESS   Always returns success.

--*/
// GC_TODO:    PeiServices - add argument and description to function comment
// GC_TODO:    CodeType - add argument and description to function comment
// GC_TODO:    Value - add argument and description to function comment
// GC_TODO:    Instance - add argument and description to function comment
// GC_TODO:    CallerId - add argument and description to function comment
// GC_TODO:    Data - add argument and description to function comment
{
  UINT8 Port80Code;

  //
  // Progress or error code, Output Port 80h card
  //
#ifdef USING_CUSTOM_P80
  if (StatusCodeToPostCode (CodeType, Value, &Port80Code)) {
#else
  if (CodeTypeToPostCode (CodeType, Value, &Port80Code)) {
#endif
    IoWrite8 (0x80, Port80Code);
  }

  return EFI_SUCCESS;
}
