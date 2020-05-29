/** @file
  Provide OEM to add OEM-specific information to main page of SCU.

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

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>

//
// Note: the service is unsupported in Rev.5.0
//


/**
  Provide OEM to add OEM-specific information to main page of SCU.
  For example: Get CPU and system FAN speed, CPU and system voltage, CPU temperature to display.

  Note: The service is called before the UpdateSystemInfo () in main menu.
        It causes that the service limits to add new item of system info, but can not update original system info.
        In future version, this service will be fixed.
		
  @param  *SetupNvData          Point to the SYSTEM_CONFIGURATION.
  @param  *Hii                  Point to the EFI_HII_PROTOCOL.
  @param  *HiiHandle            Point to Hii handle.

  @retval EFI_SUCCESS           Get the data success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcUpdateScuSystemInfoThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}
