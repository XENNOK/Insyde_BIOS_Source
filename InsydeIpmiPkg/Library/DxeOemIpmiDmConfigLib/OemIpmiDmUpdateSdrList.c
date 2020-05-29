/** @file
 OEM IPMI Device Manager Config library implement code - Update BMC SDR List.

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
#include <Library/HiiLib.h>

#include <Guid/MdeModuleHii.h>


/**
 Reserved for OEM to Update IPMI_BMC_SDR_LIST_LABEL in IPMI Device Manager Config.

 @param[in]         HiiHandle           EFI_HII_HANDLE.

 @retval EFI_UNSUPPORTED                OEM does not implement this function.
 @return Status                         Implement code Execute Status.
*/
EFI_STATUS
OemIpmiDmUpdateSdrList (
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{

  return EFI_UNSUPPORTED;

}

