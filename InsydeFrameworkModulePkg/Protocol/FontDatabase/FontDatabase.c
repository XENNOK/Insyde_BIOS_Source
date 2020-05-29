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
//; Abstract:
//;   Protocol for describing Font Database.
//;

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (FontDatabase)

EFI_GUID gEfiFontDatabaseProtocolGuid = EFI_FONT_DATABASE_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiFontDatabaseProtocolGuid, "EfiFontDatabase", "Efi Font Database Protocol ")
