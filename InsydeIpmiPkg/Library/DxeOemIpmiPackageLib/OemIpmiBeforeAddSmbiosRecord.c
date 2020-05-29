/** @file
 DXE OEM IPMI Package library implement code - Before Add SMBIOS Record.

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


#include <Library/DxeOemIpmiPackageLib.h>


/**
 Reserved for OEM to implement something additional requirements before add SMBIOS record.

 @param[in]         SmbiosHandle        Pointer to a unique 16-bit number Specifies the structure's handle.
 @param[in]         SmbiosType38        Pointer to SMBIOS_TABLE_TYPE38 struct that contain IPMI Device Information.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiBeforeAddSmbiosRecord (
  IN  EFI_SMBIOS_HANDLE                 *SmbiosHandle,
  IN  SMBIOS_TABLE_TYPE38               *SmbiosType38
  )
{
  return EFI_UNSUPPORTED;
}

