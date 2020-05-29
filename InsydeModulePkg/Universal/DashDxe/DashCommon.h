/** @file
  Common definition file for DASH

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DASH_COMMON_H_
#define _DASH_COMMON_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/CpuIo.h>
#include <Guid/SmBios.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/SmbusHc.h>
#include <Protocol/DashIoCfg.h>
#include <Protocol/MctpSmbus.h>
#include <Protocol/MctpBus.h>
#include <Protocol/Mctp.h>
#include <Protocol/Pldm.h>

#endif
