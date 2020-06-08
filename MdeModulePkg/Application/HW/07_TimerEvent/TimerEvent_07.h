/** @file
  TimerEvent_07 H Source File

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

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

VOID
GetTimeEvenNotify (
  IN EFI_EVENT      Event,
  IN VOID           *Context
  );

EFI_STATUS
GetTimeEvent (
  VOID
  );

extern EFI_GUID  gEfiTimerEventProtocolGuid;