//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "EventLog.h"

EFI_GUID gEfiPeiEventLogGuid = EFI_PPI_EVENT_LOG_GUID;

EFI_GUID_STRING (&gEfiPeiEventLogGuid, " EFI PPI SERVICE", "PEI EVENT LOG PPI");

