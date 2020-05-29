//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "EfiSpec.h"
#include EFI_PROTOCOL_DEFINITION (AcpiSystemDescriptionTable)

EFI_GUID  gEfiAcpiSdtProtocolGuid = EFI_ACPI_SDT_PROTOCOL_GUID;

EFI_GUID_STRING(&gEfiAcpiSdtProtocolGuid, "UEFI ACPI SDT Protocol", "UEFI ACPI SDT Protocol");
