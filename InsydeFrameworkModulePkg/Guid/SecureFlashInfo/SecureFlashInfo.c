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
//;   GUID for Secure Flash.
//;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (SecureFlashInfo)

EFI_GUID  gSecureFlashInfoGuid = SECURE_FLASH_INFO_GUID;

EFI_GUID_STRING (&gSecureFlashInfoGuid, "Secure Flash Info", "SecureFlash Info Guid");
