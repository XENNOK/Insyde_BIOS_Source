//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (IdeIoInfo)

EFI_GUID gEfiIdeResourceIoInfoProtocolGuid = EFI_IDE_IO_RESOURCE_INFO_PROTOCOL_GUID;

EFI_GUID gEfiAhciResourceIoInfoProtocolGuid = EFI_AHCI_IO_REXOURCE_INFO_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiIdeResourceIoInfoProtocolGuid, "Efi Ide Resource Info Protocol", "Efi Ide Resource Info 1.0 protocol");

EFI_GUID_STRING (&gEfiAhciResourceIoInfoProtocolGuid, "Efi Ahci Resource Info Protocol", "Efi Ahci Resource Info 1.0 protocol");

