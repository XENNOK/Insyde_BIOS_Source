/** @file
  SpdReadIo_24 H Source File

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

#ifndef _SPD_READ_IO_24_H_
#define _SPD_READ_IO_24_H_

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/CpuIo2.h>

#include <IndustryStandard/Pci22.h>

#define SMBUS_BASE_ADDRESS 0x20

#endif