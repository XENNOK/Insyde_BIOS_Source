//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************


#ifndef _CHIPSET_LIB_SERVICES_DXE_H
#define _CHIPSET_LIB_SERVICES_DXE_H

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "ChipsetLib.h"
//[-start-111128-IB08130011-add]//
#include "EfiScriptLib.h"
//[-end-111128-IB08130011-add]//

#include EFI_PROTOCOL_DEFINITION (ChipsetLibServices)

VOID
LinkChipsetLibProtocol (
  IN OUT   EFI_CHIPSET_LIB_SERVICES_PROTOCOL    *ProtocolServices
);

#endif

