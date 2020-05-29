/** @file

  Dump whole DXE_ME_POLICY_PROTOCOL and serial out.

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

//[-start-120808-IB10820098-modify]//
#include <Library/DebugLib.h>
#include <MePolicyLib.h>
//[-end-120808-IB10820098-modify]//

extern DXE_ME_POLICY_PROTOCOL  *mDxePlatformMePolicy;

/**
  Dump DXE ME Platform Policy

  @param[in] None.

  @retval None
**/
VOID
DxeMePolicyDebugDump (
  VOID
  )
{
  EFI_STATUS Status;

  Status = MePolicyLibInit ();
  if (EFI_ERROR (Status)) {
    return;
  }

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_INFO, "\n------------------------ MePlatformPolicy Dump Begin -----------------\n"));
  DEBUG ((EFI_D_INFO, " Revision : 0x%x\n", mDxePlatformMePolicy->Revision));
  DEBUG ((EFI_D_INFO, "MeConfig ---\n"));
  //
  // Byte 0, bit definition for functionality enable/disable
  //
  DEBUG ((EFI_D_INFO, " MeFwDownGrade : 0x%x\n", mDxePlatformMePolicy->MeConfig.MeFwDownGrade));
  DEBUG ((EFI_D_INFO, " MeLocalFwUpdEnabled : 0x%x\n", mDxePlatformMePolicy->MeConfig.MeLocalFwUpdEnabled));
  DEBUG ((EFI_D_INFO, " EndOfPostEnabled : 0x%x\n", mDxePlatformMePolicy->MeConfig.EndOfPostEnabled));
  DEBUG ((EFI_D_INFO, " EndOfPostDone : 0x%x\n", mDxePlatformMePolicy->MeConfig.EndOfPostDone));
  DEBUG ((EFI_D_INFO, " MdesCapability : 0x%x\n", mDxePlatformMePolicy->MeConfig.MdesCapability));
  DEBUG ((EFI_D_INFO, " SvtForPchCap : 0x%x\n", mDxePlatformMePolicy->MeConfig.SvtForPchCap));
  DEBUG ((EFI_D_INFO, " MdesForBiosState : 0x%x\n", mDxePlatformMePolicy->MeConfig.MdesForBiosState));

  DEBUG ((EFI_D_INFO, "MeMiscConfig ---\n"));
  DEBUG ((EFI_D_INFO, " FviSmbiosType : 0x%x\n", mDxePlatformMePolicy->MeMiscConfig.FviSmbiosType));
  DEBUG ((EFI_D_INFO, "\n------------------------ MePlatformPolicy Dump End -------------------\n"));
#endif
}

