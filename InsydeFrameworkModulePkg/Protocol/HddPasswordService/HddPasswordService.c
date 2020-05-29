//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  HddPasswordService.c

Abstract:

  Protocol used for HDD Password.

--*/

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (HddPasswordService)

EFI_GUID gEfiHddPasswordServiceProtocolGuid = EFI_HDD_PASSWORD_SERVICE_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiHddPasswordServiceProtocolGuid, "HddPasswordService Protocol", "HddPasswordService Protocol");

