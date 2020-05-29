/** @file
 This function offers an interface to handle thunderbolt chipset init stuff.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Base.h>



/**
 This function offers an interface to handle thunderbolt chipset init stuff.
 It maybe init to enable or disable depend on input parameter.

 @param[in]        TbtEnabled                     TRUE for Call this function to do enable thunderbolt init procedure.
                                                  FALSE for Call this function to do disable thunderbolt init procedure.
 @param[in]        TbtChip                        Thunderbolt chip type.
 @param[out]       *DelayFlag                     TRUE for delay a while after init procedure.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcPeiTbtInit (
  IN  BOOLEAN           TbtEnabled,
  IN  UINT8             TbtChip,
  OUT BOOLEAN           *DelayFlag
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;

}
