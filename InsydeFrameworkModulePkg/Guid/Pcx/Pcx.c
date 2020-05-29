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
//;
//;   GUID used as a filename for the Pcx logo
//;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (Pcx)

EFI_GUID gEfiDefaultPcxLogoGuid  = EFI_DEFAULT_PCX_LOGO_GUID;

EFI_GUID_STRING (&gEfiDefaultPcxLogoGuid, "Guid for PCX logo", "Guid for PCX logo");

