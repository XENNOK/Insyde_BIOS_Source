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
//;   Protocol for describing Setup Utility ..
//;

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (EfiSetupUtility)

EFI_GUID gEfiSetupUtilityProtocolGuid = EFI_SETUP_UTILITY_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiSetupUtilityProtocolGuid, "EfiSetupUtil", "Efi Setup Utility GUID")
