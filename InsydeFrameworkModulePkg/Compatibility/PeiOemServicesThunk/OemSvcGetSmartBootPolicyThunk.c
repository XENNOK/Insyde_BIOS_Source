/** @file
  This OemService is part of setting SmartBoot policy. 
  For the current design, there two smart boot policies can be modified in this module. 
 
  1. Fast boot hotkey is pressed. 
  2. The watchdog timer period setting. 
 
  More description about SmartBoot refers to the document "Smart Boot Integration Guide".

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

#include "PeiOemServicesThunk.h"
#include <Library/PeiOemSvcKernelLib.h>

/**
  This OemService is part of setting SmartBoot policy. 
  For the current design, there two smart boot policies can be modified in this module. 
  
  1. Fast boot hotkey is pressed. 
  2. The watchdog timer period setting. 
  
  More description about SmartBoot refers to the document "Smart Boot Integration Guide".

  @param  *WatchdogTimer        Watchdog timer units (1 units = 100ns)
  @param  *HotkeyPressed        Hotkey Pressed is TRUE, else is FALSE.

  @retval EFI_SUCCESS           Get smart policy success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetSmartBootPolicyThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}
