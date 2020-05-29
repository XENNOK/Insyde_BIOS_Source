//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   StallPEI.h
//;

#ifndef _STALL_PEI_H_
#define _STALL_PEI_H_

#include <Ppi/Stall.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
//#include "EdkIIGluePeim.h"
//#include "Tiano.h"
//#include "Pei.h"
//#include "BaseAddrDef.h"
//#include "IoAccess.h"

//#include EFI_PPI_PRODUCER (Stall)
#define PEI_STALL_RESOLUTION      1
#define ACPI_TIMER_ADDR       (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + 0x08)
#define ACPI_TIMER_MAX_VALUE  0x1000000

EFI_STATUS
StallPeiEntry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES      **PeiServices
  );

EFI_STATUS
EFIAPI
StallPpiFunc (
  IN CONST EFI_PEI_SERVICES           **PeiServices,
  IN CONST EFI_PEI_STALL_PPI          *This,
  IN UINTN                            Microseconds
  );
#endif
