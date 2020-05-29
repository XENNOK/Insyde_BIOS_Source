//;******************************************************************************
//;* Copyright (c) 1983-2010, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (SysPasswordService)

EFI_GUID gEfiSysPasswordServiceProtocolGuid = EFI_SYS_PASSWORD_SERVICE_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiSysPasswordServiceProtocolGuid, "SysPasswordService Protocol", "SysPasswordService Protocol");


