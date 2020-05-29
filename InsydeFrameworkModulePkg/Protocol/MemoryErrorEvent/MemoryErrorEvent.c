//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (MemoryErrorEvent)

EFI_GUID  gMemoryErrorEventGuid = MEMORY_ERROR_EVENT_GUID;

EFI_GUID_STRING(&gMemoryErrorEventGuid, "Memory Error Event Protocol", "Memory Error Event Protocol Guid");

