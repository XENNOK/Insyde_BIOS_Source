//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
//;   FirmwareAuthentication.h
//
//; Abstract:
//;
//;   Firmware Authentication PPI
//;
#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION(FirmwareAuthentication)

EFI_GUID gFirmwareAuthenticationPpiGuid = FIRMWARE_AUTHENTICATION_PPI_GUID;

EFI_GUID_STRING (&gFirmwareAuthenticationPpiGuid, "Firmware Authentication", "Firmware Authentication");

