/** @file
  This OemService queries to determine if it needed to load default setup menu.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

/**
  This OemService queries to determine if it needed to load default setup menu. 
  If LoadDefault == TRUE, system will erase the "Setup" variable. 
  It provides a hardware method to load the default setup setting. 

  @param[out]  *LoadDefaultPtr       Loading default setup menu or not.

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           The service is customized in the project.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Base on OEM design.
**/
EFI_STATUS
OemSvcLoadDefaultSetupMenu (
  OUT BOOLEAN                               *LoadDefaultPtr
  )
{
  /*++

    Todo:
      Depend on your project to setting. ex. Check GPIO or CMOS ...
      Request load default setup menu when *LoadDefaultPtr = TRUE.

  --*/

  return EFI_UNSUPPORTED;
}
