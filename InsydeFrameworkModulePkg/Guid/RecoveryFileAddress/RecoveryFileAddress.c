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
//;   RecoveryFileAddress.c
//;
//; Abstract:
//;
//;   GUIDs used for System Non Volatile HOB to save the recovery file address
//;   other phase to get revovery file
//;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (RecoveryFileAddress)

EFI_GUID  gEfiRecoveryFileAddressGuid = EFI_RECOVERY_FILE_ADDRESS_GUID;

EFI_GUID_STRING (&gEfiRecoveryFileAddressGuid, "Recovery File Address", "Recovery File Address");
