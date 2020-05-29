/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _THUNDERBOLT_DXE_H_
#define _THUNDERBOLT_DXE_H_

#include <Uefi.h>
#include <ChipsetSetupConfig.h>
#include <CommonReg.h>
#include <ChipsetSmiTable.h>
//[-start-130222-IB03780481-add]//
#include <IndustryStandard/Pci.h>
#include <OemThunderbolt.h>
//[-end-130222-IB03780481-add]//

#include <Library/UefiBootServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>

#include <Protocol/SetupUtility.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/ExitPmAuth.h>
//[-start-130222-IB03780481-add]//
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/GlobalNvsArea.h>
//[-end-130222-IB03780481-add]//

VOID
EFIAPI
ThunderboltEventCallback (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  );

#endif
