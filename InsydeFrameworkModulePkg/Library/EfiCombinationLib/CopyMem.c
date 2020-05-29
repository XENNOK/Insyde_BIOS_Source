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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CopyMem.c

Abstract:

--*/

#include "Tiano.h"
#include "Pei.h"

extern EFI_PEI_SERVICES   **mPeiServices;
extern EFI_BOOT_SERVICES  *mBS;

VOID
AsmEfiCommonLibCopyMem (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINTN  Length
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Destination - GC_TODO: add argument description
  Source      - GC_TODO: add argument description
  Length      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    (**mPeiServices).CopyMem (Destination, Source, Length);

  } else {
    //
    // The function is called in DXE phase
    //
    mBS->CopyMem (Destination, Source, Length);
  }

}
