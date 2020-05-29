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

/*++

  CPUs and Chipsets Porting File, This file contains coding which is specific to 
  how InsydeH2O supports CPUs and chipsets independently of the core code. All 
  CPUs, chipset and/or hardware platform specific modifications will be included  
  in this file.
  
--*/

#ifndef _DIMM_SMBUS_ADDR_HOB_GUID_H_
#define _DIMM_SMBUS_ADDR_HOB_GUID_H_

#define DIMM_SMBUS_ADDR_HOB_GUID \
  { \
    0xD9A0B452, 0xF7F7, 0x4EBA, 0x8A, 0x0F, 0x44, 0x10, 0x44, 0x2D, 0x3B, 0x63 \
  }

extern EFI_GUID gDimmSmbusAddrHobGuid;

#endif
