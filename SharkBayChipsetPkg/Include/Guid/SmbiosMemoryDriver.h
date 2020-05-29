/** @file

  CPUs and Chipsets Porting File, This file contains coding which is specific to 
  how InsydeH2O supports CPUs and chipsets independently of the core code. All 
  CPUs, chipset and/or hardware platform specific modifications will be included  
  in this file.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _SMBIOS_MEMORY_DRIVER_H_
#define _SMBIOS_MEMORY_DRIVER_H_

#define EFI_SMBIOS_MEMORY_DRIVER_GUID \
  { 0x8edfb3da, 0x72e6, 0x4577, 0xa4, 0xe2, 0xfb, 0xcb, 0x3c, 0xa0, 0x34, 0x98}

extern EFI_GUID gEfiSmbiosMemoryDriverGuid;

#endif
