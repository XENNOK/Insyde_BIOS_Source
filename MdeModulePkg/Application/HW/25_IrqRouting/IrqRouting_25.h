/** @file
  IrqRouting_25 H Source File

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

#ifndef _IRQ_ROUTING_25_H_
#define _IRQ_ROUTING_25_H_

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/CpuIo2.h>

UINT8*
GetIrqRoutingTable (
  UINT16  *TableSize
  );

VOID
GetHeaderInfo (
  UINT8  *Buffer
  );

VOID
GetDeviceInfo (
  UINT8  *Buffer
  );

VOID
ShowMainPage (
  VOID
  );

VOID
SetMainPageAppearance (
  VOID
  );

#endif