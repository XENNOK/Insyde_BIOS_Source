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

#ifndef _COMMON_PCI_LIBRARY_H_
#define _COMMON_PCI_LIBRARY_H_

#include <Uefi.h>
#include <IndustryStandard/Pci.h>
#define EFI_PCI_CAPABILITY_ID_SSID    0x0D

EFI_STATUS
PciFindCapId (
  IN     UINT8    Bus,
  IN     UINT8    Device,
  IN     UINT8    Function,
  IN     UINT8    CapId,
  OUT    UINT8    *CapHeader
);

#endif 
