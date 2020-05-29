/** @file
  Header file for GfxDisplay functionality

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _PEI_GFX_DISPLAY_LIB_H_
#define _PEI_GFX_DISPLAY_LIB_H_

/**
   This function allows the platform to restore the display to the Display Save State.

  @retval EFI_SUCCESS           - RapidStartGfxDisplayScreen succeeded
  @retval EFI_INVALID_PARAMETER - Invalid Parameter
  @retval EFI_NOT_FOUND         - Display Save State Header Signature failed
**/
EFI_STATUS
RapidStartGfxDisplayScreen (
  VOID
  );
#endif
