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
//;   SAPolicyPEI.h
//;

#ifndef _SA_POLICY_PEI_H_
#define _SA_POLICY_PEI_H_
#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
//[-start-130809-IB06720232-remove]//
//#include <Library/EcLib.h>
//[-end-130809-IB06720232-remove]//
//[-start-121226-IB10870063-add]//
#include <Library/PeiOemSvcChipsetLib.h>
//[-end-121226-IB10870063-add]//
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/SaPlatformPolicy.h>
#include <Guid/DimmSmbusAddrHob.h>
#include <Guid/MrcS3RestoreVariable.h>
#include <Guid/PegDataVariable.h>
//[-start-121214-IB10820195-remove]//
//#include <BaseAddrDef.h>
//[-end-121214-IB10820195-remove]//
#include <ChipsetSetupConfig.h>
#include <MrcGlobal.h>
//[-start-130118-IB11410043-add]//
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/Wdt.h>
//[-end-130118-IB11410043-add]//
//[-start-130709-IB05400426-add]//
#include <Guid/DebugMask.h>
//[-end-130709-IB05400426-add]//

//[-start-120221-IB03600472-add]//
#define MAX_TOULD_MANUAL                              1
#define MAX_TOULD_AUTO                                0
//[-end-120221-IB03600472-add]//

#define TSEG_SIZE_8192K                               0x800000

//[-start-121204-IB05300366-remove]//
//#define CAPID0_A_DMFC_MASK                            ( 0x7 )
//#define CAPID0_A_DDR_OVERCLOCK_MASK                   ( 0x8 )
//#define CAPID0_A                                      ( 0xE4 )
//[-end-121204-IB05300366-remove]//

typedef enum {
  GM_32M  = 1,
  GM_64M  = 2,
  GM_128M = 4,
  GM_MAX
} STOLEN_MEMORY;

VOID
DumpSAPolicyPEI (
  IN      CONST EFI_PEI_SERVICES        **PeiServices,
  IN      SA_PLATFORM_POLICY_PPI        *SAPlatformPolicyPpi
  );

//[-start-121128-IB03780468-add]//
EFI_STATUS
OcInitCallback (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
  IN  VOID                              *Ppi
  );
//[-end-121128-IB03780468-add]//

MrcFrequency
CalculateFrequencyLimit (
  CHIPSET_CONFIGURATION              SystemConfiguration
  );

#endif
