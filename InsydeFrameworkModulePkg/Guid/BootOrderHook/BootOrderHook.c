//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   GUID used to notify variable driver to hook BootOrder variable or not.
//;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (BootOrderHook)

EFI_GUID  gBootOrderHookEnableGuid = BOOT_ORDER_HOOK_ENABLE_GUID;
EFI_GUID  gBootOrderHookDisableGuid = BOOT_ORDER_HOOK_DISABLE_GUID;

EFI_GUID_STRING (&gBootOrderHookEnableGuid,  "BootOrderHookEnableGuid",  "Enable hook BootOrder Variable");
EFI_GUID_STRING (&gBootOrderHookDisableGuid, "BootOrderHookDisableGuid", "Disable hook BootOrder Variable");

