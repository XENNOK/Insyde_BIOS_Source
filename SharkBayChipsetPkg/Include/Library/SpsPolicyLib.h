/** @file

  Header file for SPS ME Policy functionality
  Note: Only for SPS.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_POLICY_LIB_H_
#define _SPS_POLICY_LIB_H_

#include <SpsPolicy.h>

/**
  Get SPS ME Policy.

  @param[out] SpsPolicy           Point of SPS Policy.

  @retval EFI_SUCCESS             SPS Policy Found.
**/
EFI_STATUS
GetSpsPolicy (
  OUT VOID                             **SpsPolicy
  );

#endif

