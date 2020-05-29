/** @file

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

#ifndef _OEM_PLATFORM_IDE_H_
#define _OEM_PLATFORM_IDE_H_

#include "PlatformIde.h"

EFI_STATUS
OemPlatformIdeCheckController (
  IN      EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN      EFI_HANDLE                        *Controller,
  IN OUT  BOOLEAN                           *Flag
  );

EFI_STATUS
OemPlatformIdeCalculateMode (
  IN      EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN      UINT8                             Channel,
  IN      UINT8                             Device,
  IN OUT  EFI_ATA_COLLECTIVE_MODE           *SupportedModes,
  IN OUT  BOOLEAN                           *Flag
  );

EFI_STATUS
OemDetectCableType (
  IN  EFI_PCI_IO_PROTOCOL               *PciIo,
  IN  CHIPSET_CONFIGURATION              *SetupVariable,
  IN  UINT8                             Channel,
  IN OUT  BOOLEAN                       *Flag
  );
#endif
