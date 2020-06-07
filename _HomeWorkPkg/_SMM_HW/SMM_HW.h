/** @file
  

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/


#ifndef _SMM_HW_H_
#define _SMM_HW_H_
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>

#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Framework/SmmCis.h>
#include <Library/DevicePathLib.h>

#include <Protocol/SmmPowerButtonDispatch.h>
#include <Protocol/CpuIo2.h>

#define EFI_HYDRA_ENABLE_SW_SMI 0xCA
#define CMOS_CMD    0x70
#define CMOS_DATA   0x71

#endif