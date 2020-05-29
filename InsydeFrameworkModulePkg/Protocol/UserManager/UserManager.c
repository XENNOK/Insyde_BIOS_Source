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
//;   UserManager.c
//; 
//; Abstract:
//; 
//;   EFI_BLOCK_IO2_PROTOCOL as defined in UEFI 2.3.1
//; 

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (UserManager)

EFI_GUID  gEfiUserManagerProtocolGuid = EFI_USER_MANAGER_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiUserManagerProtocolGuid, "EFI User Manager Protocol", "EFI User Manager");
