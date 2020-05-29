//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  SystemHealth.c

Abstract:

  This file abstracts the common interfaces required for SystemHealth


--*/

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (SystemHealth)

EFI_GUID  gEfiSystemHealthProtocolGuid = EFI_SYSTEM_HEALTH_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiSystemHealthProtocolGuid, "System Health Protocol", "System Health Protocol");
