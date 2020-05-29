/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _THUNDERBOLT_PEIM_H_
#define _THUNDERBOLT_PEIM_H_

#include <Uefi.h>
#include <PiPei.h>
#include <Pi/PiBootMode.h>
#include <ChipsetSetupConfig.h>
#include <CommonReg.h>
#include <Guid/GlobalVariable.h>

#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TbtLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
//[-end-130709-IB05160465-add]//
//[-start-130205-IB03780481-add]//
#include <OemThunderbolt.h>
//[-end-130205-IB03780481-add]//

#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/Stall.h>
#include <Ppi/MemoryDiscovered.h>
//[-start-121105-IB10820150-add]//
#include <PchAccess.h>
//[-end-121105-IB10820150-add]//

//[-start-130709-IB05400426-remove]//
////[-start-130425-IB05160441-add]//
//#include <MmioAccess.h>
////[-end-130425-IB05160441-add]//
//[-end-130709-IB05400426-remove]//
//[-start-130709-IB05400426-add]//
#include <Library/PeiOemSvcChipsetLib.h>
//[-end-130709-IB05400426-add]//

//[-start-120830-IB03600500-add]//
#define GPIO3_PULSE    2
//[-end-120830-IB03600500-add]//

//[-start-130205-IB03780481-remove]//
//typedef enum {
//  NormalModeWithNHI,
//  NormalModeWithoutNHI,
//  SignleDepth,
//  RedriverOnly,
//  TbtOff,
//  TbtDebug
////[-start-130124-IB04770265-modify]//
//} TBT_CR_SECURITY_LEVEL;
////[-end-130124-IB04770265-modify]//
//
////[-start-130124-IB04770265-add]//
//typedef enum {
//  SecurityLevel0,
//  SecurityLevel1,
//  SecurityLevel2,
//  SecurityLevel3,
//  SecurityLevel4
//} TBT_RR_SECURITY_LEVEL;
////[-end-130124-IB04770265-add]//
//[-end-130205-IB03780481-remove]//

//[-start-130205-IB03780481-remove]//
//EFI_STATUS
//EFIAPI
//ProgramTbtGpio (
//  IN CONST EFI_PEI_SERVICES                 **PeiServices,
//  IN       EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
//  IN       VOID                             *Ppi
//  );
//[-end-130205-IB03780481-remove]//

//[-start-130205-IB03780481-modify]//
EFI_STATUS
EFIAPI
TbtCallback (
  IN CONST EFI_PEI_SERVICES                 **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR        *NotifyDescriptor,
  IN       VOID                             *Ppi
  );
//[-end-130205-IB03780481-modify]//

#endif
