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

    AcpiWpbtProtocol.c

Abstract:

    Locate ACPI WPBT Protocol

--*/

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (AcpiWpbtProtocol)

EFI_GUID gEfiAcpiWpbtProtocolGuid = EFI_ACPI_WPBT_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiAcpiWpbtProtocolGuid, "Acpi Wpbt Protocol", "EFI ACPI WPBT Protocol");

