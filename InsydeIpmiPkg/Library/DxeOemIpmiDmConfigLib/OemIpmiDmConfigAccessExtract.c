/** @file
 OEM IPMI Device Manager Config library implement code - Config Access Extract.

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


/**
 Reserved for OEM to do something necessary before extract Config Data.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmConfigAccessExtract (
  IN  UINT8                             *IpmiDmConfig
  )
{

  return EFI_UNSUPPORTED;

}

