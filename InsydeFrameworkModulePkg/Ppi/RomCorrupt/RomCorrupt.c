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
#include "Pei.h"
#include EFI_PPI_DEFINITION(RomCorrupt)

EFI_GUID gPeiRomCorruptPpiGuid = PEI_ROM_CORRUPT_PPI_GUID;

EFI_GUID_STRING(&gPeiRomCorruptPpiGuid, "PeiRomCorrupt", "PeiRomCorrupt");
