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

  H2OIpmiPlatformHookPpi.c

Abstract:

  H2O IPMI Platform Hook Ppi source file

--*/

#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION (H2OIpmiPlatformHookPpi)


EFI_GUID  gH2OIpmiPlatformHookPpiGuid = H2O_IPMI_PLATFORM_HOOK_PPI_GUID;

EFI_GUID_STRING(&gH2OIpmiPlatformHookPpiGuid, "H2O IPMI Pei PlatformHook", "H2O IPMI PlatformHook Ppi");
