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
  
  EmuSecFileName.c
    
--*/
#include "Tiano.h"

#include EFI_GUID_DEFINITION (EmuSecFileName)

EFI_GUID gEmuSecFileNameGuid = EMU_SEC_FILENAME_GUID;

EFI_GUID_STRING (&gEmuSecFileNameGuid, "EmuSecFileName", "Emulation Sec Core Filename");

