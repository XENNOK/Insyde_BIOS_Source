/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchRegsAdsp.h

@brief
  Register names for Audio DSP block

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_<generation_name>_" in register/bit names. e.g., "_PCH_CPT_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without <generation_name> inserted.

**/
#ifndef _PCH_REGS_ADSP_H_
#define _PCH_REGS_ADSP_H_

#ifdef ADSP_FLAG

#define MMIO_ADDR_MASK                      0xFFFFFFF0

//
//  LPSS AUDIO DSP Registers (D19:F0)
//
#define PCI_DEVICE_NUMBER_PCH_ADSP          19
#define PCI_FUNCTION_NUMBER_PCH_ADSP        0
#define R_PCH_ADSP_VENDOR_ID                0x00
#define V_PCH_ADSP_VENDOR_ID                V_PCH_INTEL_VENDOR_ID
#define R_PCH_LPTLP_ADSP_DEVICE_ID          0x02
#define V_PCH_LPTLP_ADSP_DEVICE_ID          0x9C36

//
// Audio DSP PCI Configuration space definitions
//
#define R_PCH_ADSP_COMMAND                   0x04
#define B_PCH_ADSP_COMMAND_BME               BIT2
#define B_PCH_ADSP_COMMAND_MSE               BIT1
#define R_PCH_ADSP_ADBA                      0x10
#define R_PCH_ADSP_SPCBA                     0x14
#define R_PCH_ADSP_VDRTCTL0                  0xA0
#define B_PCH_ADSP_VDRTCTL0_D3SRAMPGD        BIT2
#define B_PCH_ADSP_VDRTCTL0_D3PGD            BIT1
#define R_PCH_ADSP_VDRTCTL2                  0xA8
#define V_PCH_ADSP_VDRTCTL2                  0xFFF
#define R_PCH_ADSP_PME_CTRL_STS              0x84
#define B_PCH_ADSP_PME_CTRL_STS_PWR_ST       (BIT1|BIT0)

#define SB_DSP_ID                            0xD7

//
// Audio DSP IOSF Sideband interface definitions
//
#define R_PCH_RCRB_IOBPIRI_IOBPIS_ADSP 0xD7000000     ///< ADSP

#define R_PCH_ADSP_VDLDAT1             0x624
#define V_PCH_ADSP_VDLDAT1_CCO         0x40100

#define R_PCH_ADSP_VDLDAT2             0x628
#define V_PCH_ADSP_VDLDAT2_MASK        0xFFFF
#define V_PCH_ADSP_VDLDAT2_IRQ3        0xD9D8
#define V_PCH_ADSP_VDLDAT2_IRQ3_INV    0xD8D9
#define V_PCH_ADSP_VDLDAT2_IRQ4        0xDBDA

#define R_PCH_ADSP_PCICFGCTL           0x500
#define B_PCH_ADSP_PCICFGCTL_PCICD     BIT0
#define B_PCH_ADSP_PCICFGCTL_ACPIIE    BIT1
#define B_PCH_ADSP_PCICFGCTL_SPCBAD    BIT7

#define R_PCH_ADSP_PMCTL               0x1E0
#define V_PCH_ADSP_PMCTL               0x3F

//
// Audio DSP Shim registers
//
#define R_PCH_ADSP_SHIM_BASE           0xE7000
#define R_PCH_ADSP_SHIM_LTRC           0xE0
#define V_PCH_ADSP_SHIM_LTRC           0x3003

// ACPI Interrupt
#define R_PCH_ADSP_SHIM_IMC            0x28
#define V_PCH_ADSP_SHIM_IMC            0x7FFF0000
#define R_PCH_ADSP_SHIM_IPCD           0x40
#define V_PCH_ADSP_SHIM_IPCD_1         0x80000000
#define V_PCH_ADSP_SHIM_IPCD_2         0x04000000

#endif // ADSP_FLAG
#endif
