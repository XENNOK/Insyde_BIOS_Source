//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*--
Module Name:

  H2OIpmiPlatformHookProtocol.h

Abstract:

  H2O IPMI PlatformHook Protocol definition file

--*/

#ifndef _H2O_IPMI_PLATFORM_HOOK_PROTOCOL_H_
#define _H2O_IPMI_PLATFORM_HOOK_PROTOCOL_H_


#include "Tiano.h"
#include "SetupConfig.h"



EFI_FORWARD_DECLARATION (H2O_IPMI_PLATFORM_HOOK_PROTOCOL);



//
// Protocol Guid
//
#define H2O_IPMI_PLATFORM_HOOK_PROTOCOL_GUID \
  { 0xcfe682c1, 0xff0a, 0x401c, 0x81, 0xa1, 0xbe, 0x46, 0x83, 0xc6, 0xd4, 0x94}



typedef
VOID
(EFIAPI *H2O_IPMI_PLATFORM_HOOK_PROTOCOL_SYNC_FROM_BMC) (
  IN  H2O_IPMI_PLATFORM_HOOK_PROTOCOL    *This,
  IN  SYSTEM_CONFIGURATION               *SetupConfig
);
/*++

Routine Description:

  This platform/OEM function can get some platform/OEM settings from BMC.
  These settings are platform-dependent or OEM-defined.

Arguments:

   This           - Pointer to H2O_IPMI_PLATFORM_HOOK_PROTOCOL instance
   
   SetupConfig    - A pointer to SYSTEM_CONFIGURATION structure.
   
Returns:

  None
  
--*/



typedef
VOID
(EFIAPI *H2O_IPMI_PLATFORM_HOOK_PROTOCOL_SET_TO_BMC) (
  IN  H2O_IPMI_PLATFORM_HOOK_PROTOCOL   *This,
  IN  SYSTEM_CONFIGURATION              *SetupConfig
);
/*++

Routine Description:

  This platform/OEM function can set some platform/OEM settings to BMC.
  These settings are platform-dependent or OEM-defined.

Arguments:

   This           - Pointer to H2O_IPMI_PLATFORM_HOOK_PROTOCOL instance
   
   SetupConfig    - A pointer to SYSTEM_CONFIGURATION structure.
   
Returns:

   None
  
--*/



typedef
VOID
(EFIAPI *H2O_IPMI_PLATFORM_HOOK_PROTOCOL_SET_ACPI_KEYBOARD) (
  IN  H2O_IPMI_PLATFORM_HOOK_PROTOCOL    *This,
  IN  BOOLEAN                            Enable
);
/*++

Routine Description:

  This platform/OEM function can set variable in GlobalNVS to disable/enable
  PS2 keyboard in ACPI. This function will work with "Boot Option" defined in
  IPMI specification.
  Platform/OEM can implement this function to set correct variable.

Arguments:

   This           - Pointer to H2O_IPMI_PLATFORM_HOOK_PROTOCOL instance
   
   Enable         - TRUE to enable keyboard, FALSE to disable keyboard
   
Returns:

  None
  
--*/



typedef struct _H2O_IPMI_PLATFORM_HOOK_PROTOCOL {
  H2O_IPMI_PLATFORM_HOOK_PROTOCOL_SYNC_FROM_BMC      PlatformSyncFromBmc;
  H2O_IPMI_PLATFORM_HOOK_PROTOCOL_SET_TO_BMC         PlatformSetToBmc;
  H2O_IPMI_PLATFORM_HOOK_PROTOCOL_SET_ACPI_KEYBOARD  PlatformSetAcpiKeyboard;
} H2O_IPMI_PLATFORM_HOOK_PROTOCOL;

extern EFI_GUID gH2OIpmiPlatformHookProtocolGuid;

#endif
