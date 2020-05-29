/** @file
 SMM OEM IPMI Package library header file.

 This file contains functions prototype that can be implemented by OEM to fit
 their requirements.

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


#ifndef _SMM_OEM_IPMI_PACKAGE_LIB_H_
#define _SMM_OEM_IPMI_PACKAGE_LIB_H_


#include <PiSmm.h>

#include <Protocol/H2OIpmiInterfaceProtocol.h>


/**
 Reserved for OEM to implement something additional requirements after SMM IPMI Interface Protocol has been installed.

 @param[in]         Ipmi                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiSmmAfterInstallInterface (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi
  );


/**
 Reserved for OEM to implement something additional requirements after SMM IPMI FRU Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiSmmAfterInstallFru (
  VOID
  );


/**
 Reserved for OEM to implement something additional requirements after SMM IPMI SDR Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiSmmAfterInstallSdr (
  VOID
  );


/**
 Reserved for OEM to implement something additional requirements after SMM IPMI SEL Data Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiSmmAfterInstallSelData (
  VOID
  );


/**
 Reserved for OEM to implement something additional requirements after SMM IPMI SEL Info Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiSmmAfterInstallSelInfo (
  VOID
  );


#endif

