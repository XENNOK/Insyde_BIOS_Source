/** @file
 DXE OEM IPMI Package library header file.

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


#ifndef _DXE_OEM_IPMI_PACKAGE_LIB_H_
#define _DXE_OEM_IPMI_PACKAGE_LIB_H_


#include <PiDxe.h>

#include <ServerProcessorManagementInterfaceTable.h>

#include <Protocol/H2OIpmiInterfaceProtocol.h>
#include <Protocol/Smbios.h>

#include <IndustryStandard/SmBios.h>


/**
 This package function can enter Setup Utility as IPMI boot option request.
 OEM can implement this function since entering Setup Utility is a OEM specific function.

 @retval EFI_SUCCESS                    Entering Setup Utility success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiEnterSetup (
  VOID
  );


/**
 This package function can set variable in GlobalNVS to disable/enable PS2 keyboard in ACPI.
 This package function will work with "Boot Option" defined in IPMI specification.
 OEM can implement this function to set correct variable by OEM specific way.

 @param[in]         EnableKeyboard      TRUE to enable keyboard, FALSE to disable keyboard.

 @retval EFI_SUCCESS                    Set Acpi Keyboard success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiSetAcpiKeyboard (
  IN  BOOLEAN                           EnableKeyboard
  );


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
  );


/**
 This package function can update platform policy based on setup settings.
 OEM can implement this function to set correct variable by OEM specific way.

 @retval EFI_SUCCESS                    Update platform policy success.
 @return EFI_ERROR (Status)             OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiUpdatePolicy (
  VOID
  );


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
  );


/**
 Reserved for OEM to implement something additional requirements before install ACPI Table.

 @param[in]         SpmiTable           Pointer to EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_DESCRIPTION_TABLE struct.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiBeforeInstallAcpiTable (
  IN  EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_DESCRIPTION_TABLE    *SpmiTable
  );


/**
 Reserved for OEM to implement something additional requirements before IPMI Misc driver end.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiBeforeMiscDriverEnd (
  VOID
  );


/**
 Reserved for OEM to implement something additional requirements after DXE IPMI Interface Protocol has been installed.

 @param[in]         Ipmi                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiDxeAfterInstallInterface (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi
  );


/**
 Reserved for OEM to implement something additional requirements after DXE IPMI FRU Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiDxeAfterInstallFru (
  VOID
  );


/**
 Reserved for OEM to implement something additional requirements after DXE IPMI SDR Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiDxeAfterInstallSdr (
  VOID
  );


/**
 Reserved for OEM to implement something additional requirements after DXE IPMI SEL Data Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiDxeAfterInstallSelData (
  VOID
  );


/**
 Reserved for OEM to implement something additional requirements after DXE IPMI SEL Info Protocol has been installed.

 @return (Status)                       OEM implement code execute status.
 @retval EFI_UNSUPPORTED                OEM does not implement this function.
*/
EFI_STATUS
OemIpmiDxeAfterInstallSelInfo (
  VOID
  );


#endif

