/** @file
 OEM IPMI Device Manager Config library implement code - Get Config Data Size.

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


#include <Library/DxeOemIpmiDmConfigLib.h>

//
// Include OemIpmiDmConfigForm.h in $(PROJECT_PKG)/Include
//
//#include <OemIpmiDmConfigForm.h>


/**
 Reserved for OEM to return the size of IPMI Device Manager Config Data.

 @param[out]        ConfigDataSize      A point to UINTN contents IPMI Device Manager Config Data size.

 @retval EFI_SUCCESS                    Return the size of IPMI Device Manager Config Data success.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiDmGetConfigDataSize (
  OUT UINTN                             *ConfigDataSize
  )
{

  //
  // IPMI Device Manager Config data size will be OEM_IPMI_DM_CONFIG struct size.
  //
  //*ConfigDataSize = sizeof (OEM_IPMI_DM_CONFIG);
  //return EFI_SUCCESS;

  return EFI_UNSUPPORTED;

}

