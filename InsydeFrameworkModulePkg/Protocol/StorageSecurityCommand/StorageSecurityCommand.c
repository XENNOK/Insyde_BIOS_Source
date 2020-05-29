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
//;   StorageSecurityCommand.c
//; 
//; Abstract:
//; 
//;   EFI_STORAGE_SECURITY_COMMAND_PROTOCOL as defined in UEFI 2.3.1
//; 

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (StorageSecurityCommand)

EFI_GUID  gEfiStorageSecurityCommandProtocolGuid = EFI_STORAGE_SECURITY_COMMAND_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiStorageSecurityCommandProtocolGuid, "EFI Storage Security Command Protocol", "EFI Storage Security Command");
