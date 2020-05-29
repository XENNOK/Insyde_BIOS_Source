/** @file

  Header file for AT Policy functionality

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _AT_POLICY_LIB_H_
#define _AT_POLICY_LIB_H_

#include <Library/DebugLib.h>
#include <Protocol/AtPlatformPolicy.h>

/**
  Dump DXE AT Platform Policy

  @param[in] None.

  @retval None
**/
VOID
DxeAtPolicyDebugDump (
  VOID
  );

#endif
