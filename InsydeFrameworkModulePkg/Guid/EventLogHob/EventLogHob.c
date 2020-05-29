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
#include EFI_GUID_DEFINITION (EventLogHob)

EFI_GUID  gEventLogHobGuid = EVENT_LOG_HOB_GUID;

EFI_GUID_STRING (&gEventLogHobGuid, "Event Log Hob Info", "Event Log Hob Guid");
