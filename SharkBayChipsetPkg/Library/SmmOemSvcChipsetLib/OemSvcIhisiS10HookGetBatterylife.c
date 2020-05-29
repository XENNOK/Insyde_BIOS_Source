/** @file
 This function offers an interface to Hook IHISI Sub function AH=10h,function "GetBattLife"

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/SmmOemSvcChipsetLib.h>

/**
 This function offers an interface to Hook IHISI Sub function AH=10h,function "GetBattLife"

 @param[in, out]    BattLife            On entry, pointer to BattLife Address.
                                        On exit, points to updated BattLife Address.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS10HookGetBatterylife (
  IN OUT UINT8      *BattLife
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/

  return EFI_UNSUPPORTED;
}
