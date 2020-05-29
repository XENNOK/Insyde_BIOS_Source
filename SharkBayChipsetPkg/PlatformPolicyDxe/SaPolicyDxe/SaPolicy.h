//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
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
//;   SaPolicy.h
//;

#ifndef _SA_POLICY_DXE_H_
#define _SA_POLICY_DXE_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
//[-start-130809-IB06720232-remove]//
//#include <Library/EcLib.h>
//[-end-130809-IB06720232-remove]//
//[-start-121220-IB10820206-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121220-IB10820206-add]//
#include <Protocol/SetupUtility.h>
#include <Protocol/SaPlatformPolicy.h>
//[-start-120925-IB06150247-add]//
//[-start-121130-IB11410025-modify]//
#include <Protocol/PlatformGopPolicy.h>
//[-end-121130-IB11410025-modify]//
//[-end-120925-IB06150247-add]//
#include <Guid/DimmSmbusAddrHob.h>
#include <SaAccess.h>
#include <ChipsetSetupConfig.h>
//[-start-120904-IB06150243-add]//
#include <PchRegs.h>
//[-end-120904-IB06150243-add]//
//[-start-121012-IB03780464-add]//
#include <Library/UefiLib.h>
#include <Protocol/SaGlobalNvsArea.h>

#include <Protocol/MemInfo.h>
//[-end-121012-IB03780464-add]//
//[-start-121121-IB11410023-add]//
#include <Protocol/PlatformInfo.h>
//[-end-121121-IB11410023-add]//
//[-start-140521-IB05400527-add]//
#include <Protocol/GlobalNvsArea.h>
//[-end-140521-IB05400527-add]//

#define MEM_EQU_4GB                0x100000000
#define RMRR_USB_MEM_SIZE          0x20000
#define RES_IO_BASE                0x1000
#define RES_IO_LIMIT               0xFFFF
#define RES_MEM_LIMIT_1            ( PcdGet64 ( PcdPciExpressBaseAddress ) - 1 )

//
// DIMM SMBus addresses
//
#define DIMM_SMB_SPD_P0C0D0        0xA0
#define DIMM_SMB_SPD_P0C0D1        0xA2
#define DIMM_SMB_SPD_P0C1D0        0xA4
#define DIMM_SMB_SPD_P0C1D1        0xA6
#define DIMM_SMB_SPD_P0C0D2        0xA8
#define DIMM_SMB_SPD_P0C1D2        0xAA

VOID
DumpSAPolicy (
  IN      DXE_PLATFORM_SA_POLICY_PROTOCOL        *SAPlatformPolicy
  );

//[-start-121012-IB03780464-add]//
VOID
EFIAPI
UpdateSaGlobalNvsCallback (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  );
//[-end-121012-IB03780464-add]//

//[-start-140521-IB05400527-add]//
VOID
EFIAPI
UpdateGlobalNvsCallback (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  );
//[-end-140521-IB05400527-add]//

#endif
