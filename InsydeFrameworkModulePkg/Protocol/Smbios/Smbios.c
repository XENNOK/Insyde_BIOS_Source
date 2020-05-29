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
//;   Smbios.c
//;
//; Abstract:
//;
//;
//;

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (Smbios)

EFI_GUID  gEfiSmbiosProtocolGuid = EFI_SMBIOS_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiSmbiosProtocolGuid, "EFI Smbios Protocol", "EFI Smbios");
