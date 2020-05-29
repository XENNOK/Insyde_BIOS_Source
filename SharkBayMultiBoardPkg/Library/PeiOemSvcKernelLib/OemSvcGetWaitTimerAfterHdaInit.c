/** @file
  This OemService is setting wait timer after hda init. The function is created for setting wait timer
  when needed.

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

#include <Library/PeiOemSvcKernelLib.h>

#define WAIT_TIMER_AFTER_HDA_INIT     300 // 300 Microseconds

/**
  This OemService is setting wait timer after hda init. The function is created for setting wait timer
  when needed.

  @param[out]  *WaitTimerAfterHdaInit    A pointer to wait timer

  @retval      EFI_UNSUPPORTED                Returns unsupported by default, use the deafult value 0 Microsecond,
                                              the default value is decided by gInsydeTokenSpaceGuid.PcdHdaInitWaitTime.
  @retval      EFI_MEDIA_CHANGED              Oem set delay time for combo card after hda init. 
**/
EFI_STATUS
OemSvcGetWaitTimerAfterHdaInit (
  OUT UINT16                            *WaitTimerAfterHdaInit
  )
{

  *WaitTimerAfterHdaInit = (UINT16)WAIT_TIMER_AFTER_HDA_INIT;
  
  return EFI_MEDIA_CHANGED;
}
