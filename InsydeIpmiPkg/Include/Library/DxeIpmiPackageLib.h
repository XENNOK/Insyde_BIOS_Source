/** @file
 DXE IPMI Package library header file.

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


#ifndef _DXE_IPMI_PACKAGE_LIB_H_
#define _DXE_IPMI_PACKAGE_LIB_H_


#include <PiDxe.h>

#include <Protocol/H2OIpmiInterfaceProtocol.h>


/**
 This package function can enter Setup Utility as IPMI boot option request.
 Platform implement this function since entering Setup Utility is a platform dependent function.

 @retval EFI_SUCCESS                    Entering Setup Utility success.
 @return EFI_ERROR (Status)             Locate gEfiSetupUtilityProtocolGuid Protocol error.
*/
EFI_STATUS
IpmiEnterSetup (
  VOID
  );


/**
 This package function can set variable in GlobalNVS to disable/enable PS2 keyboard in ACPI.
 This package function will work with "Boot Option" defined in IPMI specification.
 Platform implement this function to set correct variable.

 @param[in]         EnableKeyboard      TRUE to enable keyboard, FALSE to disable keyboard.

 @retval EFI_SUCCESS                    Set Acpi Keyboard success.
 @return EFI_ERROR (Status)             Locate gEfiGlobalNvsAreaProtocolGuid Protocol error.
*/
EFI_STATUS
IpmiSetAcpiKeyboard (
  IN  BOOLEAN                           EnableKeyboard
  );


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
  );


/**
 This package function can update platform policy based on setup settings.
 Platform implement this function to set correct variable.

 @retval EFI_SUCCESS                    Update platform policy success.
 @return EFI_ERROR (Status)             Implement code execute status.
*/
EFI_STATUS
IpmiUpdatePolicy (
  VOID
  );


#endif

