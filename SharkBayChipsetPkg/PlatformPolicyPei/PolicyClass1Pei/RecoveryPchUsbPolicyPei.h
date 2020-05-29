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

#ifndef _RECOVERY_PCH_USB_POLICY_PEI_H_
#define _RECOVERY_PCH_USB_POLICY_PEI_H_

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
//[-start-121214-IB10820195-remove]//
//#include <BaseAddrDef.h>
//[-end-121214-IB10820195-remove]//
#include <Ppi/PchUsbPolicy.h>
//[-start-120917-IB03780460-add]//
#include <PchAccess.h>
#include <Library/IoLib.h>
#include <PchPlatformLib.h>
//[-end-120917-IB03780460-add]//
//[-start-121219-IB10870058-add]//
#include <Library/PeiOemSvcChipsetLib.h>
//[-end-121219-IB10870058-add]//

VOID
DumpRecoveryPchUsbPolicyPEI (
  IN      CONST EFI_PEI_SERVICES        **PeiServices,
  IN      PCH_USB_POLICY_PPI            *RecoveryPchUsbPolicyPpi
  );

#endif
