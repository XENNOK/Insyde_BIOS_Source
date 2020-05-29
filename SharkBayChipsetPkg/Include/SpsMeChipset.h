/** @file

  Chipset definition for SPS ME Devices.

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, ME registers are denoted by "_ME_" in register names
  - Registers / bits that are different between ME generations are denoted by
    "_ME_<generation_name>_" in register/bit names. e.g., "_ME_CPT_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a ME generation will be just named
    as "_ME_" without <generation_name> inserted.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _SPS_ME_CHIPSET_H_
#define _SPS_ME_CHIPSET_H_

#define SPS_ME_BUS                     0
#define SPS_ME_DEVICE_NUMBER           22

#define SPS_HECI_FUNCTION_NUMBER       0x00
#define SPS_HECI2_FUNCTION_NUMBER      0x01
#define SPS_IDER_FUNCTION_NUMBER       0x02
#define SPS_SOL_FUNCTION_NUMBER        0x03

#if 0 /// Not Used in SPS3EN
#define SPS_IDER_BUS_NUMBER            SPS_ME_BUS
#define SPS_IDER_DEVICE_NUMBER         SPS_ME_DEVICE_NUMBER
#define SPS_SOL_BUS_NUMBER             SPS_ME_BUS
#define SPS_SOL_DEVICE_NUMBER          SPS_ME_DEVICE_NUMBER
#define SPS_HECI_DEV_FCN               ((SPS_ME_DEVICE_NUMBER) << 3 | (SPS_HECI_FUNCTION_NUMBER))
#define SPS_HECI2_DEV_FCN              ((SPS_ME_DEVICE_NUMBER) << 3 | (SPS_HECI2_FUNCTION_NUMBER))
#endif

///
/// Default Vendor ID and Device ID
///
#define SPS_V_INTEL_VENDOR_ID          0x8086

#define SPS_V_ME_HECI_VENDOR_ID        SPS_V_INTEL_VENDOR_ID
#define SPS_V_ME_IDER_VENDOR_ID        SPS_V_INTEL_VENDOR_ID
#define SPS_V_ME_SOL_VENDOR_ID         SPS_V_INTEL_VENDOR_ID

#define SPS_V_ME_HECI_DEVICE_ID        0x8C3A
#define SPS_V_ME_HECI2_DEVICE_ID       0x8C3B
#define SPS_V_ME_IDER_DEVICE_ID        0x8C3C
#define SPS_V_ME_SOL_DEVICE_ID         0x8C3D

#define SPS_V_ME_LPTLP_HECI_DEVICE_ID  0x9C3A
#define SPS_V_ME_LPTLP_HECI2_DEVICE_ID 0x9C3B
#define SPS_V_ME_LPTLP_IDER_DEVICE_ID  0x9C3C
#define SPS_V_ME_LPTLP_SOL_DEVICE_ID   0x9C3D

#if 0 /// Not Used in SPS3EN
#define SPS_R_ME_HFS                       0x40
#define SPS_R_ME_MISC_SHDW                 0x44
#define SPS_R_ME_GS_SHDW                   0x48
#define SPS_R_ME_H_GS                      0x4C
#define SPS_R_ME_HFS_4                     0x64
#define SPS_R_ME_HFS_5                     0x68
#define SPS_R_ME_HFS_6                     0x6C
#define SPS_B_BOOT_GUARD_ENF_MASK          0x0200
#define SPS_B_TPM_DISCONNECT               0x1000
#endif

#if 0 /// Not Used in SPS3EN
#define SPS_R_ME_H_GS                 0x4C
#define SPS_B_ME_DID_RAPID_START_BIT       BIT23
#define SPS_B_ME_DID_TYPE_MASK             BIT28
#define SPS_R_ME_H_GS2                     0x70
#define SPS_B_ME_MBP_GIVE_UP               BIT0
#define SPS_R_ME_HERS                      0xBC
#define SPS_B_ME_EXTEND_REG_VALID          BIT31
#define SPS_B_ME_EXTEND_REG_ALGORITHM      (BIT0 | BIT1 | BIT2 | BIT3)
#define SPS_V_ME_SHA_1                     0x00
#define SPS_V_ME_SHA_256                   0x02
#define SPS_R_ME_HER1                      0xC0
#define SPS_R_ME_HER2                      0xC4
#define SPS_R_ME_HER3                      0xC8
#define SPS_R_ME_HER4                      0xCC
#define SPS_R_ME_HER5                      0xD0
#define SPS_R_ME_HER6                      0xD4
#define SPS_R_ME_HER7                      0xD8
#define SPS_R_ME_HER8                      0xDC
#endif

#if 0 // Not used in SPS3
///
/// ME-related Chipset Definition
///
#define SPS_IS_PCH_LPTH_HECI_DEVICE_ID(DeviceId)     ((DeviceId == SPS_V_ME_HECI_DEVICE_ID))

#define SPS_IS_PCH_LPTLP_HECI_DEVICE_ID(DeviceId)    ((DeviceId == SPS_V_ME_LPTLP_HECI_DEVICE_ID))

#define SPS_IS_PCH_LPT_HECI_DEVICE_ID(DeviceId) \
          (SPS_IS_PCH_LPTH_HECI_DEVICE_ID(DeviceId) || SPS_IS_PCH_LPTLP_HECI_DEVICE_ID(DeviceId))

#define SPS_IS_PCH_LPTH_HECI2_DEVICE_ID(DeviceId)    ((DeviceId == SPS_V_ME_HECI2_DEVICE_ID))

#define SPS_IS_PCH_LPTLP_HECI2_DEVICE_ID(DeviceId)   ((DeviceId == SPS_V_ME_LPTLP_HECI2_DEVICE_ID))

#define SPS_IS_PCH_LPT_HECI2_DEVICE_ID(DeviceId) \
          (SPS_IS_PCH_LPTH_HECI2_DEVICE_ID(DeviceId) || SPS_IS_PCH_LPTLP_HECI2_DEVICE_ID(DeviceId))

#define SPS_IS_PCH_LPTH_IDER_DEVICE_ID(DeviceId)     ((DeviceId == SPS_V_ME_IDER_DEVICE_ID))

#define SPS_IS_PCH_LPTLP_IDER_DEVICE_ID(DeviceId)    ((DeviceId == SPS_V_ME_LPTLP_IDER_DEVICE_ID))

#define SPS_IS_PCH_LPT_IDER_DEVICE_ID(DeviceId) \
          (SPS_IS_PCH_LPTH_IDER_DEVICE_ID(DeviceId) || SPS_IS_PCH_LPTLP_IDER_DEVICE_ID(DeviceId))

#define SPS_IS_PCH_LPTH_SOL_DEVICE_ID(DeviceId)      ((DeviceId == SPS_V_ME_SOL_DEVICE_ID))

#define SPS_IS_PCH_LPTLP_SOL_DEVICE_ID(DeviceId)     ((DeviceId == SPS_V_ME_LPTLP_SOL_DEVICE_ID))

#define SPS_IS_PCH_LPT_SOL_DEVICE_ID(DeviceId) \
          (SPS_IS_PCH_LPTH_SOL_DEVICE_ID(DeviceId) || SPS_IS_PCH_LPTLP_SOL_DEVICE_ID(DeviceId))
#endif

#endif
