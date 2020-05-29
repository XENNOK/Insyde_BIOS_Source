/** @file
  This driver provides optionally protocol to override or provide EDID information and/or
  output device display properties to the producer of the Graphics Output protocol.

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

#ifndef _EDID_OVERRIDE_DXE_H_
#define _EDID_OVERRIDE_DXE_H_

#include <Uefi.h>
#include <Protocol/EdidOverride.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <VesaBiosExtensions.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/DevicePathLib.h>

#endif
