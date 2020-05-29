//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

    UnplugVGAGuid.c

Abstract:

    Guid used to signal the return of the control back to BIOS after starting image.

--*/

#include "Tiano.h"

#include EFI_GUID_DEFINITION (UnPlugVGAGuid)

EFI_GUID gEfiUnPlugVGAGuid = EFI_UNPLUG_VGA_GUID;

EFI_GUID_STRING (&gEfiUnPlugVGAGuid, "EFI UnPlug VGA Guid", "EFI UnPlug VGA Guid");

