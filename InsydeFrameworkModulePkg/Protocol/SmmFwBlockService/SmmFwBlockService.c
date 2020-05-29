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
#include EFI_PROTOCOL_DEFINITION (SmmFwBlockService)

EFI_GUID  gEfiSmmFwBlockServiceProtocolGuid = EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiSmmFwBlockServiceProtocolGuid, "SMM FW Block service Protocol", "SMM FW Block service Protocol");
