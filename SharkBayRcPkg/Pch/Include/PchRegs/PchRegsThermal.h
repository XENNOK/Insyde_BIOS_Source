/** @file
  Register names for PCH Thermal Device

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_<generation_name>_" in register/bit names. e.g., "_PCH_LPT_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without <generation_name> inserted.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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
#ifndef _PCH_REGS_THERMAL_H_
#define _PCH_REGS_THERMAL_H_

//
//  Thermal Device Registers (D31:F6)
//
#define PCI_DEVICE_NUMBER_PCH_THERMAL   31
#define PCI_FUNCTION_NUMBER_PCH_THERMAL 6
#define R_PCH_THERMAL_VENDOR_ID         0x00
#define V_PCH_THERMAL_VENDOR_ID         V_PCH_INTEL_VENDOR_ID
#define R_PCH_THERMAL_DEVICE_ID         0x02
#define V_PCH_LPTH_THERMAL_DEVICE_ID     0x8C24
#define V_PCH_LPTLP_THERMAL_DEVICE_ID    0x9C24
#define R_PCH_THERMAL_COMMAND           0x04
#define B_PCH_THERMAL_COMMAND_MSE       BIT1
#define B_PCH_THERMAL_COMMAND_BME       BIT2
#define R_PCH_THERMAL_TBAR              0x10
#define V_PCH_THERMAL_TBAR_SIZE         (4 * 1024)
#define N_PCH_THREMAL_TBAR_ALIGNMENT    12
#define B_PCH_THERMAL_TBAR_MASK         0xFFFFF000
#define R_PCH_THERMAL_TBARH             0x14
#define R_PCH_THERMAL_SVID              0x2C
#define R_PCH_THERMAL_INTLN             0x3C
#define R_PCH_THERMAL_TBARB             0x40
#define V_PCH_THERMAL_TBARB_SIZE        (4 * 1024)
#define N_PCH_THREMAL_TBARB_ALIGNMENT   12
#define B_PCH_THERMAL_SPTYPEN           BIT0
#define R_PCH_THERMAL_TBARBH            0x44
#define B_PCH_THERMAL_TBARB_MASK        0xFFFFF000

#define R_PCH_TBARB_TSC                 0x04
#define B_PCH_TBARB_TSC_PLD             BIT7
#define B_PCH_TBARB_TSC_CPDE            BIT0
#define R_PCH_TBARB_TSS                 0x06
#define R_PCH_TBARB_TSEL                0x08
#define B_PCH_TBARB_TSEL_PLD            BIT7
#define B_PCH_TBARB_TSEL_ETS            BIT0
#define R_PCH_TBARB_TSREL               0x0A
#define R_PCH_TBARB_TSMIC               0x0C
#define B_PCH_TBARB_TSMIC_PLD           BIT7
#define B_PCH_TBARB_TSMIC_SMIE          BIT0
#define R_PCH_TBARB_CTT                 0x10
#define V_PCH_TBARB_CTT_LPTH            0x154
#define V_PCH_TBARB_CTT_LPTLP           0x14A
#define R_PCH_TBARB_TAHV                0x14
#define R_PCH_TBARB_TALV                0x18
#define R_PCH_TBARB_TSPM                0x1C
#define B_PCH_TBARB_TSPM_LTT            (BIT8 | BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_TBARB_TSPM_LTT            0x0C8
#define B_PCH_TBARB_TSPM_MAXTSST        (BIT11 | BIT10 | BIT9)
#define V_PCH_TBARB_TSPM_MAXTSST        (0x4 << 9)
#define B_PCH_TBARB_TSPM_MINTSST        BIT12
#define B_PCH_TBARB_TSPM_DTSSIC0        BIT13
#define B_PCH_TBARB_TSPM_DTSSS0EN       BIT14
#define B_PCH_TBARB_TSPM_TSPMLOCK       BIT15
#define R_PCH_TBARB_TL                  0x40
#define B_PCH_TBARB_TL_LOCK             BIT31
#define R_PCH_TBARB_PHL                 0x60
#define B_PCH_TBARB_PHLE                BIT15
#define R_PCH_TBARB_PHLC                0x62
#define R_PCH_TBARB_TAS                 0x80
#define R_PCH_TBARB_TSPIEN              0x82
#define R_PCH_TBARB_TSGPEN              0x84
#endif
