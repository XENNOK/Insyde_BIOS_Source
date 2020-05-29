/** @file
 DXE OEM IPMI Package library implement code - Update NVS Variable.

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
 This package function can set variable in GlobalNVS to store BMC status for ACPI method.
 OEM can implement this function to set correct variable by OEM specific way.

 @param[in]         Ipmi                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @retval EFI_SUCCESS                    Set variable in GlobalNVS to store BMC status success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiUpdateNvsVariable (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi
  )
{
  return EFI_UNSUPPORTED;
}

