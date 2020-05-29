/** @file
  This header file is for Switchable Graphics Feature PEI module.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SWITCHABLE_GRAPHICS_PEI_H_
#define _SWITCHABLE_GRAPHICS_PEI_H_

#include <CpuRegs.h>
#include <Guid/SgInfoHob.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiOemSvcChipsetLib.h>
#include <Library/PeiServicesLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//
#include <PchRegs.h>
#include <Ppi/SwitchableGraphics.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/SaPlatformPolicy.h>
#include <ChipsetSetupConfig.h>
#include <SwitchableGraphicsDefine.h>

#define ACPI_TIMER_ADDR                       (FixedPcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + 0x08)
#define ACPI_TIMER_MAX_VALUE                  0x1000000

#endif
