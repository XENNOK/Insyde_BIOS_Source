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

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UsbStatusCodeLib.h

Abstract:

  Lib to provide Serial I/O status code reporting.

--*/

#ifndef _PEI_USB_STATUS_CODE_LIB_H_
#define _PEI_USB_STATUS_CODE_LIB_H_

//
// Statements that include other files
//
#include "Tiano.h"
#include "Pei.h"

//
// Initialization function
//
EFI_STATUS
UsbInitializeStatusCode (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

//
// Status code reporting function
//
EFI_STATUS
UsbReportStatusCode (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  PeiServices - GC_TODO: add argument description
  CodeType    - GC_TODO: add argument description
  Value       - GC_TODO: add argument description
  Instance    - GC_TODO: add argument description
  CallerId    - GC_TODO: add argument description
  Data        - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

#endif
