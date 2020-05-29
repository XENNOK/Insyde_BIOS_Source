/** @file
  Header file for initialization of CPU Audio device.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _AUDIO_INIT_H_
#define _AUDIO_INIT_H_

#include <Uefi.h>

#include <IndustryStandard/Pci22.h>
#include <SaAccess.h>
#include <EfiScriptLib.h>
#include <PchAccess.h>
#include <PchPlatformLib.h>
#include <CpuRegs.h>

#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>

#include <Guid/DxeServices.h>

#include <Protocol/BootScriptSave.h>
#include <Protocol/SaPlatformPolicy.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>

#define SA_HDA_MAX_LOOP_TIME  10
#define SA_HDA_WAIT_PERIOD    100
#define SA_HDA_MAX_SID_NUMBER 4
#define SA_HDA_MAX_SID_MASK   ((1 << SA_HDA_MAX_SID_NUMBER) - 1)
#define AUDIO_CODEC_MAX_SIZE  0x80

#define SA_HDA_ENABLE_3RD_PIN_WIDGET  0x00878101
#define SA_HDA_DISABLE_3RD_PIN_WIDGET 0x00878100

///
/// Functions
///
/**
  Initialize SystemAgent Audio Device/Codec.

  @param[in] ImageHandle             Handle for the image of this driver
  @param[in] DxePlatformSaPolicy     SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS - Audio/Codec initialization Done.
**/
EFI_STATUS
AudioInit (
  IN EFI_HANDLE                      ImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
;

#endif
