/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _THUNDERBOLT_SMM_H_
#define _THUNDERBOLT_SMM_H_

#include <Uefi.h>
#include <ChipsetSetupConfig.h>
#include <ChipsetSmiTable.h>
//[-start-121214-IB10820196-remove]//
//#include <PciAccess.h>
//[-end-121214-IB10820196-remove]//
#include <SaCommonDefinitions.h>
#include <IndustryStandard/Pci22.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>

#include <Protocol/SetupUtility.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/SmmVariable.h>

extern EFI_SMM_VARIABLE_PROTOCOL       *mSmmVariable;
extern EFI_GLOBAL_NVS_AREA             *mGlobalNvsArea;
//[-start-120727-IB03780455-add]//
extern CHIPSET_CONFIGURATION            SysConfig;
extern UINT16                          mReserveMemoryPerSlot;
extern UINT16                          mReservePMemoryPerSlot;
extern UINT8                           mReserveIOPerSlot;
//[-end-120727-IB03780455-add]//

//[-start-130123-IB04770265-add]//
typedef union _PCIE_EXT_CAP_HDR {
  UINT32            EXT_CAP_HDR;
  struct {
    UINT16            ExtCapId;
    UINT16            CapVersion      :  4;
    UINT16            NextItemPtr     : 12;
   } Fields;
}PCIE_EXT_CAP_HDR;

BOOLEAN
BitScanForward(
  UINT32  *Index,
  UINT32  Mask
);
//[-end-130123-IB04770265-add]//
EFI_STATUS
ThunderboltCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context        OPTIONAL,
  IN OUT VOID    *CommBuffer     OPTIONAL,
  IN OUT UINTN   *CommBufferSize OPTIONAL
  );

#endif
