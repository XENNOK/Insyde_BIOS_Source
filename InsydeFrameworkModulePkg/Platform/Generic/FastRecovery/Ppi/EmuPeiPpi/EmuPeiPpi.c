//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

/*++

Module Name:
  
  EmuPeiPpi.c
    
--*/
#include "Tiano.h"

#include EFI_PPI_DEFINITION (EmuPeiPpi)

EFI_GUID gEmuPeiPpiGuid = EMU_PEI_PPI_GUID;

EFI_GUID_STRING (&gEmuPeiMarkGuid, "EmuPeiPpi", "Emulation Pei Support Ppi GUID");

