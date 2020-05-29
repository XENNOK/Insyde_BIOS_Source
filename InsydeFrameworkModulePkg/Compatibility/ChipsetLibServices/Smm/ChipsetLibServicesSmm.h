//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************


#ifndef _CHIPSET_LIB_SERVICES_SMM_H
#define _CHIPSET_LIB_SERVICES_SMM_H

#include "Tiano.h"
#include "EfiSmmDriverLib.h"
#include "ChipsetLib.h"
#include "SmmDriverLib.h"

#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)
#include EFI_PROTOCOL_DEFINITION (SmmChipsetLibServices)
//[-start-110905-IB08450015-add]//
#include EFI_PROTOCOL_DEFINITION (ChipsetLibServices)
//[-end-110905-IB08450015-add]//

VOID
LinkChipsetLibProtocol (
  IN OUT   EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL    *ProtocolServices
);

#endif

