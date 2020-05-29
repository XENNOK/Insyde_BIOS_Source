/** @file

  Dump whole DXE_AT_POLICY_PROTOCOL and serial out.

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

#include "AtPolicyLib.h"

extern DXE_AT_POLICY_PROTOCOL  *mDxePlatformAtPolicy;

/**
  Dump DXE AT Platform Policy

  @param[in] None.

  @retval None
**/
VOID
DxeAtPolicyDebugDump (
  VOID
  )
{
  if (mDxePlatformAtPolicy == NULL) {
    return;
  }
  DEBUG ((EFI_D_INFO, "\n------------------------ AtPlatformPolicy Dump Begin ------------------\n"));
  DEBUG ((EFI_D_INFO, " Revision : 0x%x\n", mDxePlatformAtPolicy->Revision));
  DEBUG ((EFI_D_INFO, "At ---\n"));
  DEBUG ((EFI_D_INFO, " AtConfig : 0x%x\n", mDxePlatformAtPolicy->At.AtAmBypass));
  DEBUG ((EFI_D_INFO, " AtEnterSuspendState : 0x%x\n", mDxePlatformAtPolicy->At.AtEnterSuspendState));
  DEBUG ((EFI_D_INFO, "\n------------------------ AtPlatformPolicy Dump End --------------------\n"));
}

