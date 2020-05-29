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
//;   EfiSystemResourceTable.c
//; 
//; Abstract:
//; 
//;   EFI System Resource Table 
//; 

#include "Tiano.h"
#include EFI_GUID_DEFINITION (EfiSystemResourceTable)

EFI_GUID  gEfiSystemResourceTableGuid = EFI_SYSTEM_RESOURCE_TABLE_GUID;

EFI_GUID_STRING (&gEfiSystemResourceTableGuid, "EFI System Resource Table", "EFI System Resource Table");
