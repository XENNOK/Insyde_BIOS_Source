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

  H2OIpmiPlatformHookProtocol.c

Abstract:

  H2O IPMI PlatformHook Protocol source file

--*/

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (H2OIpmiPlatformHookProtocol)

EFI_GUID  gH2OIpmiPlatformHookProtocolGuid = H2O_IPMI_PLATFORM_HOOK_PROTOCOL_GUID;

EFI_GUID_STRING (&gH2OIpmiPlatformHookProtocolGuid, "H2O IPMI Dxe PlatformHook", "H2O IPMI PlatformHook Protocol");
