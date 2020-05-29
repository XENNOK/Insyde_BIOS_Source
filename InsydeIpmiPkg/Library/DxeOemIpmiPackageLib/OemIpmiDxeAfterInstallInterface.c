/** @file
 DXE OEM IPMI Package library implement code - After Install Interface Protocol.

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
 Reserved for OEM to implement something additional requirements after DXE IPMI Interface Protocol has been installed.

 @param[in]         Ipmi                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiDxeAfterInstallInterface (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi
  )
{
  return EFI_UNSUPPORTED;
}

