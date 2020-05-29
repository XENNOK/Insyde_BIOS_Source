/** @file

  Chipset definition for ME Devices.

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
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2011 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _ME_CHIPSET_H_
#define _ME_CHIPSET_H_

#include "MeChipsetLib.h"

#define ME_BUS                0
#define ME_DEVICE_NUMBER      22

#define HECI_FUNCTION_NUMBER  0x00
#define HECI2_FUNCTION_NUMBER 0x01
#define IDER_FUNCTION_NUMBER  0x02
#define SOL_FUNCTION_NUMBER   0x03

#define IDER_BUS_NUMBER       ME_BUS
#define IDER_DEVICE_NUMBER    ME_DEVICE_NUMBER
#define SOL_BUS_NUMBER        ME_BUS
#define SOL_DEVICE_NUMBER     ME_DEVICE_NUMBER
#define HECI_DEV_FCN          ((ME_DEVICE_NUMBER) << 3 | (HECI_FUNCTION_NUMBER))
#define HECI2_DEV_FCN         ((ME_DEVICE_NUMBER) << 3 | (HECI2_FUNCTION_NUMBER))

///
/// Default Vendor ID and Device ID
///
#define V_INTEL_VENDOR_ID   0x8086

#define V_ME_HECI_VENDOR_ID V_INTEL_VENDOR_ID
#define V_ME_IDER_VENDOR_ID V_INTEL_VENDOR_ID
#define V_ME_SOL_VENDOR_ID  V_INTEL_VENDOR_ID

#define V_ME_HECI_DEVICE_ID   0x8C3A
#define V_ME_HECI2_DEVICE_ID  0x8C3B
#define V_ME_IDER_DEVICE_ID   0x8C3C
#define V_ME_SOL_DEVICE_ID    0x8C3D

#define V_ME_LPTLP_HECI_DEVICE_ID   0x9C3A
#define V_ME_LPTLP_HECI2_DEVICE_ID  0x9C3B
#define V_ME_LPTLP_IDER_DEVICE_ID   0x9C3C
#define V_ME_LPTLP_SOL_DEVICE_ID    0x9C3D

#define R_ME_HFS                  0x40
#define R_ME_MISC_SHDW            0x44
#define R_ME_GS_SHDW              0x48
#define R_ME_H_GS                 0x4C
#define R_ME_HFS_4                0x64
#define R_ME_HFS_5                0x68
#define R_ME_HFS_6                0x6C
#define B_BOOT_GUARD_ENF_MASK     0x0200
#define B_TPM_DISCONNECT          0x1000
#define B_TPM1_2_DEACTIVATED      0x0100

#define B_ME_DID_RAPID_START_BIT  BIT23
#define B_ME_DID_TYPE_MASK        BIT28
#define R_ME_H_GS2                0x70
#define B_ME_MBP_GIVE_UP          BIT0
#define R_ME_HERS                 0xBC
#define B_ME_EXTEND_REG_VALID     BIT31
#define B_ME_EXTEND_REG_ALGORITHM (BIT0 | BIT1 | BIT2 | BIT3)
#define V_ME_SHA_1                0x00
#define V_ME_SHA_256              0x02
#define R_ME_HER1                 0xC0
#define R_ME_HER2                 0xC4
#define R_ME_HER3                 0xC8
#define R_ME_HER4                 0xCC
#define R_ME_HER5                 0xD0
#define R_ME_HER6                 0xD4
#define R_ME_HER7                 0xD8
#define R_ME_HER8                 0xDC

///
/// ME-related Chipset Definition
///
#define HeciEnable()    MeDeviceControl (HECI1, Enabled);
#define Heci2Enable()   MeDeviceControl (HECI2, Enabled);
#define IderEnable()    MeDeviceControl (IDER, Enabled);
#define SolEnable()     MeDeviceControl (SOL, Enabled);
#define Usbr1Enable()   MeDeviceControl (USBR1, Enabled);
#define Usbr2Enable()   MeDeviceControl (USBR2, Enabled);

#define HeciDisable()   MeDeviceControl (HECI1, Disabled);
#define Heci2Disable()  MeDeviceControl (HECI2, Disabled);
#define IderDisable()   MeDeviceControl (IDER, Disabled);
#define SolDisable()    MeDeviceControl (SOL, Disabled);
#define Usbr1Disable()  MeDeviceControl (USBR1, Disabled);
#define Usbr2Disable()  MeDeviceControl (USBR2, Disabled);
#define DisableAllMEDevices() \
  HeciDisable (); \
  Heci2Disable (); \
  IderDisable (); \
  SolDisable ();

#define IS_PCH_LPTH_HECI_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_HECI_DEVICE_ID) \
    )

#define IS_PCH_LPTLP_HECI_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_LPTLP_HECI_DEVICE_ID) \
    )

#define IS_PCH_LPT_HECI_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_LPTH_HECI_DEVICE_ID(DeviceId) || \
      IS_PCH_LPTLP_HECI_DEVICE_ID(DeviceId) \
    )

#define IS_PCH_LPTH_HECI2_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_HECI2_DEVICE_ID) \
    )

#define IS_PCH_LPTLP_HECI2_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_LPTLP_HECI2_DEVICE_ID) \
    )

#define IS_PCH_LPT_HECI2_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_LPTH_HECI2_DEVICE_ID(DeviceId) || \
      IS_PCH_LPTLP_HECI2_DEVICE_ID(DeviceId) \
    )

#define IS_PCH_LPTH_IDER_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_IDER_DEVICE_ID) \
    )

#define IS_PCH_LPTLP_IDER_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_LPTLP_IDER_DEVICE_ID) \
    )

#define IS_PCH_LPT_IDER_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_LPTH_IDER_DEVICE_ID(DeviceId) || \
      IS_PCH_LPTLP_IDER_DEVICE_ID(DeviceId) \
    )

#define IS_PCH_LPTH_SOL_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_SOL_DEVICE_ID) \
    )

#define IS_PCH_LPTLP_SOL_DEVICE_ID(DeviceId) \
    (  \
      (DeviceId == V_ME_LPTLP_SOL_DEVICE_ID) \
    )

#define IS_PCH_LPT_SOL_DEVICE_ID(DeviceId) \
    ( \
      IS_PCH_LPTH_SOL_DEVICE_ID(DeviceId) || \
      IS_PCH_LPTLP_SOL_DEVICE_ID(DeviceId) \
    )

///
/// Function Disable SUS well lockdown
///
#define FunctionDisableWellLockdown() MeDeviceControl (FDSWL, Enabled);

#endif
