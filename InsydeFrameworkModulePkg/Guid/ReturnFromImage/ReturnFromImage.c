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

    ReturnFromImage.c

Abstract:

    Guid used to signal the return of the control back to BIOS after starting image.

--*/

#include "Tiano.h"

#include EFI_GUID_DEFINITION (ReturnFromImage)

EFI_GUID gEfiReturnFromImageGuid = EFI_RETURN_FROM_IMAGE_GUID;

EFI_GUID_STRING (&gEfiReturnFromImageGuid, "EFI Return From Image Guid", "EFI Return From Image Guid");

