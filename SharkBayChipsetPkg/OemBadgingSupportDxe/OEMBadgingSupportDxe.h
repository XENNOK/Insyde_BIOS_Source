/** @file

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

/*++ 
  Module Name:
  
    OemBadginsSupportDxe.h
  
  Abstract:
  
    Header file for EFI OEM badging support driver

--*/

#ifndef _EFI_OEM_BADGING_SUPPORT_H
#define _EFI_OEM_BADGING_SUPPORT_H

#define EFI_CPUID_SIGNATURE                    0x0
#define EFI_CPUID_VERSION_INFO                 0x1
#define  EFI_CPUID_CACHE_PARAMS                0x4

#define EFI_OEM_BADGING_FILENAME_INTEL_IMB \
  { \
    0x931F00D1, 0x10FE, 0x48bf, 0xAB, 0x72, 0x77, 0x3D, 0x38, 0x9E, 0x3F, 0xDA \
  }

#define EFI_OEM_BADGING_FILENAME_INTEL_ICR \
  { \
    0xA4A9F7B8, 0x24A4, 0x4ea2, 0xB1, 0xC6, 0x14, 0x55, 0x99, 0xB2, 0xDF, 0x64 \
  }

#define EFI_INSYDE_BOOT_BADGING_GUID \
  { \
    0x931F77D1, 0x10FE, 0x48bf, 0xAB, 0x72, 0x77, 0x3D, 0x38, 0x9E, 0x3F, 0xAA \
  }

#define FAMILYMODEL_MASK        0x0FF0          // Family/Model Code.
#define FAMILYMODEL_PRESCOTT_F4x 0x0F40         // Prescott F4x family/model code.
#define FAMILY_CONROE           0x06F0          // Conroe

#define CHIPSET_VENDOR_ID_INTEL         0x8086
#define B_HYPER_THREADING_TECH          (1 << 28)
#define B_LOGICAL_PROCESSOR_COUNT       0xFF0000

#endif
