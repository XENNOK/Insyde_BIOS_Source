/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_BOOT_SCRIPT_DONE_PPI_H_
#define _PEI_BOOT_SCRIPT_DONE_PPI_H_

// #include "Tiano.h"
// #include "Pei.h"

//
// Please sync to $(EFI_SOURCE)\$(PROJECT_TXT_ROOT)\Include\BootScriptDone.h
//
#define PEI_BOOT_SCRIPT_DONE_PPI_GUID \
  { \
    0xFD7C4665, 0x34BE, 0x426b, 0xB1, 0xF8, 0x3A, 0xB7, 0x53, 0xCE, 0x44, 0xB0 \
  }

// EFI_FORWARD_DECLARATION ( PEI_BOOT_SCRIPT_DONE_PPI );

extern EFI_GUID gPeiBootScriptDonePpiGuid;

#endif
