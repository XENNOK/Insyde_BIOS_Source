/** @file
  SmmPowerButton_18 H Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMM_POWER_BUTTON_18_H_
#define _SMM_POWER_BUTTON_18_H_

#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmPowerButtonDispatch.h>

#define POST_CODE_PORT     0x80
#define CMOS_COMMAND_PORT  0x70
#define CMOS_DATA_PORT     0x71

EFI_STATUS
EFIAPI
TestCallBack (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_POWER_BUTTON_DISPATCH_CONTEXT   *DispatchContext
  );

#endif