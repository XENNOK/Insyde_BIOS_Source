/** @file

  Definition of Server Platform Services Firmware ME module.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_PEI_H_
#define _SPS_PEI_H_

#include <SpsMe.h>

#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>

#include <Library/SpsMeLib.h>

#include <Guid/SpsInfoHob.h>

#include <Ppi/MemoryDiscovered.h>

EFI_STATUS
SpsInitAfterMemoryInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDescriptor,
  IN       VOID                        *Ppi
  );

#endif
