//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
        
#ifndef _PEI_CLKGEN_HOOK_H
#define _PEI_CLKGEN_HOOK_H

#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"
//[-start-130326-IB06720210-remove]//
//#include "Chipset.h"
//[-end-130326-IB06720210-remove]//
#include "OemClkGen.h"

//
// North Bridge Bus, Device and function
//
#define CLKGEN_MC_BUS            0x00 
#define CLKGEN_MC_DEV            0x00 
#define CLKGEN_MC_FUN            0x00
#define CLKGEN_MC_R_VID          0x00
#define CLKGEN_R_VID             0x00

#define UMA_BUS                  0
#define UMA_DEV                  2
#define UMA_FUN                  0

#define DES_BUS                  0
#define DES_DEV                  1
#define DES_FUN                  0

//[-start-090521-IB03060041-add]//
#define PCI_BCC                  0x000B        // Base Class Code Register
#define PCI_PBUS                 0x0018        // Primary Bus Number Register
//[-end-090521-IB03060041-add]//

EFI_STATUS
ReadNBVID (
  IN  EFI_PEI_SERVICES          **PeiServices,
  OUT UINT16                    *VenderID
  );

EFI_STATUS
GetVgaTypeInfo (
  IN EFI_PEI_SERVICES          **PeiServices,
  OUT CLOCK_GEN_VGA_TYPE       *VgaType
  );

#endif
