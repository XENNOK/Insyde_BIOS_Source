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

#include EFI_PROTOCOL_DEFINITION (SmmChipsetLibServices)

EFI_GUID gEfiSmmChipsetLibServicesProtocolGuid = EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL_GUID;

EFI_GUID_STRING (
      &gEfiSmmChipsetLibInterfaceProtocolGuid, 
      "Efi System Management Mode Chipset Lib Services Protocol", 
      "EfiSmmChipsetLibServices Protocol"
      );


