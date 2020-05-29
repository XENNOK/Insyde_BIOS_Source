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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    SmmStatusCode.h

Abstract:

  Light weight lib to support EFI Smm drivers.

--*/

#ifndef _EFI_SMM_STATUS_CODE_H_
#define _EFI_SMM_STATUS_CODE_H_

#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmStatusCode)

EFI_STATUS
InitializeSmmStatusCode (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ImageHandle - GC_TODO: add argument description
  SystemTable - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EFIAPI
SmmReportStatusCode (
  IN EFI_SMM_STATUS_CODE_PROTOCOL * This,
  IN EFI_STATUS_CODE_TYPE         CodeType,
  IN EFI_STATUS_CODE_VALUE        Value,
  IN UINT32                       Instance,
  IN EFI_GUID                     * CallerId,
  IN EFI_STATUS_CODE_DATA         * Data OPTIONAL
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  This      - GC_TODO: add argument description
  CodeType  - GC_TODO: add argument description
  Value     - GC_TODO: add argument description
  Instance  - GC_TODO: add argument description
  CallerId  - GC_TODO: add argument description
  Data      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

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

  GC_TODO: Add function description

Arguments:

  CodeType  - GC_TODO: add argument description
  Value     - GC_TODO: add argument description
  Instance  - GC_TODO: add argument description
  CallerId  - GC_TODO: add argument description
  Data      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

typedef struct {
  EFI_HANDLE                    Handle;
  EFI_SMM_STATUS_CODE_PROTOCOL  SmmStatusCode;
} SMM_STATUS_CODE_PRIVATE;

#endif
