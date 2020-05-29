//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  S3RestoreAcpiCallback.c

Abstract:

  Restore system configuration have completed.

--*/

#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION (S3RestoreAcpiCallback)

EFI_GUID gPeiS3RestoreAcpiCallbackPpiGuid = PEI_S3_RESTORE_ACPI_CALLBACK_PPI_GUID;

EFI_GUID_STRING (&gPeiS3RestoreAcpiCallbackPpiGuid, "S3RestoreAcpiCallback", "S3RestoreAcpiCallback");
