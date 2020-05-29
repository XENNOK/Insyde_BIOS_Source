/** @file
 DXE IPMI Package library implement code - Update NVS Variable.

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


#include <Library/DxeIpmiPackageLib.h>


/**
 This package function can set variable in GlobalNVS to store BMC status for ACPI method.
 Platform implement this function to set correct variable.

 @param[in]         Ipmi                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @retval EFI_SUCCESS                    Set variable in GlobalNVS to store BMC status success.
 @return EFI_ERROR (Status)             Locate gEfiGlobalNvsAreaProtocolGuid Protocol error.
*/
EFI_STATUS
IpmiUpdateNvsVariable (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi
  )
{
  return EFI_UNSUPPORTED;
}

