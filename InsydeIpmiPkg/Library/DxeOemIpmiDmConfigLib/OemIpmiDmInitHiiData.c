/** @file
 OEM IPMI Device Manager Config library implement code - Init HII Data.

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
 Reserved for OEM to init IPMI Deviece Manager Config HII Data.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmInitHiiData (
  IN  UINT8                             *IpmiDmConfig,
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{

  //
  // OEM can implement code here to deal with HII Data.
  //

  return EFI_UNSUPPORTED;

}

