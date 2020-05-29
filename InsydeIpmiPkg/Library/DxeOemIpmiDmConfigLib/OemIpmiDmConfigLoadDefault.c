/** @file
 OEM IPMI Device Manager Config library implement code - Config Load Default.

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
 Reserved for OEM to do something necessary when load default in Config Access Callback.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigLoadDefault (
  IN  UINT8                             *IpmiDmConfig,
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{

  return EFI_UNSUPPORTED;

}

