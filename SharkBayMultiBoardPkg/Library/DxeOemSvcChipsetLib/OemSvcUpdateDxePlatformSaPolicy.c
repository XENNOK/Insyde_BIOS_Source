/** @file
  This function offers an interface to modify DXE_PLATFORM_SA_POLICY_PROTOCOL Data before the system
  installs DXE_PLATFORM_SA_POLICY_PROTOCOL.
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

#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/PcdLib.h>
#include <OemBoardId.h>

/**
 This function offers an interface to modify DXE_PLATFORM_SA_POLICY_PROTOCOL Data before the system
 installs DXE_PLATFORM_SA_POLICY_PROTOCOL.

 @param[in, out]    *SaPlatformPolicy   On entry, points to DXE_PLATFORM_SA_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_PLATFORM_SA_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateDxePlatformSaPolicy (
  IN OUT DXE_PLATFORM_SA_POLICY_PROTOCOL          *SaPlatformPolicy
  )
{

  switch (PcdGet8 (PcdCurrentBoardId)) {
  case V_EC_BOARD_ID_GRAYS_REEF:
    SaPlatformPolicy->PlatformBoardID = SAPlatformBoardIDGraysReef;
    break;

  case V_EC_BOARD_ID_BASKING_RIDGE:
    SaPlatformPolicy->PlatformBoardID = SAPlatformBoardIDBaskingRidge;
    break;

  };

  return EFI_UNSUPPORTED;
}
