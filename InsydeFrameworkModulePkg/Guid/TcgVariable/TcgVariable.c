//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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
//;   TCG Memory Write Requested Control Variable and Physical Presence Interface Variable Guid
//;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (TcgVariable)

EFI_GUID  gMemoryOnlyResetControlGuid = MEMORY_ONLY_RESET_CONTROL_GUID;
EFI_GUID  gPhysicalPresenceInterfaceControlGuid = PHYSICAL_PRESENCE_INTERFACE_CONTROL_GUID;

EFI_GUID_STRING (&gMemoryOnlyResetControlGuid, "Memory Overwrite Request Control Variable", "Memory Only Reset Control GUID");
EFI_GUID_STRING (&gPhysicalPresenceInterfaceControlGuid, "Physical Presence Interface Control Variable", "Physical Presence Interface Control GUID");

