//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include <Tiano.h>

#include EFI_PROTOCOL_DEFINITION(TrEEPhysicalPresence)

EFI_GUID gEfiTrEEPhysicalPresenceProtocolGuid = EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL_GUID;
EFI_GUID gEfiTrEEPhysicalPresenceGuid         = EFI_TREE_PHYSICAL_PRESENCE_DATA_GUID;

EFI_GUID_STRING (&gEfiTrEEPhysicalPresenceProtocolGuid, "TrEEPhysicalPresence", "TrEE Physical Presence Protocol");
