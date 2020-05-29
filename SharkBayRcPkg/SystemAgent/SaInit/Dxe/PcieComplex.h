/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file
  PcieComplex.h

  This is header file for SA PCIE Root Complex initialization.

**/

EFI_STATUS
PegInitBeforeExitPmAuth (
  VOID
  )
/**

  This function performs Peg initialization before ExitPmAuth.

  @retval EFI_SUCCESS   - Always.

**/
;

EFI_STATUS
SaSecurityInit (
  VOID
  )
/**

  This function performs SA Security locking in ExitPmAuth callback

  @retval EFI_SUCCESS     - Security lock has done
  @retval EFI_UNSUPPORTED - Security lock not done successfully

**/
;
