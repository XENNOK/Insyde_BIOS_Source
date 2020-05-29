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
//; Module Name:
//;
//;   EfiOEMBadgingSupport.c
//;
//; Abstract:
//;

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (EfiOEMBadgingSupport)

EFI_GUID  gEfiOEMBadgingSupportProtocolGuid = EFI_OEM_BADGING_SUPPORT_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiOEMBadgingSupportProtocolGuid, "OEM Badging Protocol Support", "OEM Badging Protocol Support");
