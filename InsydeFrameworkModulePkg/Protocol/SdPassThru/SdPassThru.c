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
//;   AtaPassThru.c
//;
//; Abstract:
//;
//;   EFI_ATA_PASS_THRU_PROTOCOL as defined in UEFI 2.2
//;

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (SdPassThru)

EFI_GUID  gEfiSdPassThruProtocolGuid = EFI_SD_PASS_THRU_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiSdPassThruProtocolGuid, "EFI SdPassThru Protocol", "EFI SdPassThru");
