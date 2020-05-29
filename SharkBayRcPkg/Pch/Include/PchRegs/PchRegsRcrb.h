/** @file
  Register names for PCH Chipset Configuration Registers

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
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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
#ifndef _PCH_REGS_RCRB_H_
#define _PCH_REGS_RCRB_H_

//
// Chipset configuration registers (Memory space)
// RCBA
//
#define R_PCH_RCRB_CIR0050                0x0050
#define B_PCH_RCRB_CIR0_TCLOCKDN          BIT31
#define R_PCH_RCRB_RPFN                   0x0404                      ///< Root Port Function Number & Hide for PCI Express Root Ports
#define B_PCH_RCRB_RPFN_RP8CH             BIT31                       ///< Root Port 8 Hide
#define B_PCH_RCRB_RPFN_RP8FN             (BIT30 | BIT29 | BIT28)     ///< Root Port 8 Function Number
#define B_PCH_RCRB_RPFN_RP7CH             BIT27                       ///< Root Port 7 Hide
#define B_PCH_RCRB_RPFN_RP7FN             (BIT26 | BIT25 | BIT24)     ///< Root Port 7 Function Number
#define B_PCH_RCRB_RPFN_RP6CH             BIT23                       ///< Root Port 6 Hide
#define B_PCH_RCRB_RPFN_RP6FN             (BIT22 | BIT21 | BIT20)     ///< Root Port 6 Function Number
#define B_PCH_RCRB_RPFN_RP5CH             BIT19                       ///< Root Port 5 Hide
#define B_PCH_RCRB_RPFN_RP5FN             (BIT18 | BIT17 | BIT16)     ///< Root Port 5 Function Number
#define B_PCH_RCRB_RPFN_RP4CH             BIT15                       ///< Root Port 4 Hide
#define B_PCH_RCRB_RPFN_RP4FN             (BIT14 | BIT13 | BIT12)     ///< Root Port 4 Function Number
#define B_PCH_RCRB_RPFN_RP3CH             BIT11                       ///< Root Port 3 Hide
#define B_PCH_RCRB_RPFN_RP3FN             (BIT10 | BIT9 | BIT8)       ///< Root Port 3 Function Number
#define B_PCH_RCRB_RPFN_RP2CH             BIT7                        ///< Root Port 2 Hide
#define B_PCH_RCRB_RPFN_RP2FN             (BIT6 | BIT5 | BIT4)        ///< Root Port 2 Function Number
#define B_PCH_RCRB_RPFN_RP1CH             BIT3                        ///< Root Port 1 Hide
#define B_PCH_RCRB_RPFN_RP1FN             (BIT2 | BIT1 | BIT0)        ///< Root Port 1 Function Number
#define S_PCH_RCRB_PRFN_RP_FIELD          4                           ///< 4 bits per root port
#define R_PCH_RCRB_CIR0900                0x0900
#define R_PCH_RCRB_CIR1100                0x1100
#define R_PCH_RCRB_TRSR                   0x1E00                      ///< Trap Status Register
#define B_PCH_RCRB_TRSR_CTSS              0x000F                      ///< Cycle Trap SMI# Status mask
#define R_PCH_RCRB_TRCR                   0x1E10                      ///< Trapped Cycle Register
#define S_PCH_RCRB_TRCR                   8
#define B_PCH_RCRB_TRCR_RWI               BIT24
#define B_PCH_RCRB_TRCR_AHBE              0x00000000000F0000
#define B_PCH_RCRB_TRCR_TIOA              0x000000000000FFFC
#define R_PCH_RCRB_TRWDR                  0x1E18                      ///< Trap Write Data Register
#define S_PCH_RCRB_TRWDR                  8
#define B_PCH_RCRB_TRWDR_TIOD             0x00000000FFFFFFFF
#define R_PCH_RCRB_IO_TRAP_0              0x1E80                      ///< Trap Configuration Register
#define R_PCH_RCRB_IO_TRAP_1              0x1E88                      ///< Trap Configuration Register
#define R_PCH_RCRB_IO_TRAP_2              0x1E90                      ///< Trap Configuration Register
#define R_PCH_RCRB_IO_TRAP_3              0x1E98                      ///< Trap Configuration Register
#define B_PCH_RCRB_IO_TRAP_RWM            BIT17                       ///< 49 - 32 for 32 bit access
#define B_PCH_RCRB_IO_TRAP_RWIO           BIT16                       ///< 48 - 32 for 32 bit access
#define N_PCH_RCRB_IO_TRAP_RWIO           (48 - 32)                   ///< for 32 bit access
#define B_PCH_RCRB_IO_TRAP_BEM            0x000000F000000000
#define B_PCH_RCRB_IO_TRAP_TBE            0x0000000F00000000
#define B_PCH_RCRB_IO_TRAP_ADMA           0x0000000000FC0000
#define B_PCH_RCRB_IO_TRAP_IOAD           0x000000000000FFFC
#define B_PCH_RCRB_IO_TRAP_TRSE           BIT0                        ///< Trap and SMI# Enable
#define R_PCH_RCRB_V0CTL                  0x2014                      ///< Virtual channel 0 resource control
#define B_PCH_RCRB_V0CTL_EN               BIT31
#define B_PCH_RCRB_V0CTL_ID               (7 << 24)                   ///< Bit[26:24]
#define N_PCH_RCRB_V0CTL_ID               24
#define V_PCH_RCRB_V0CTL_ETVM_MASK        0xFC00
#define V_PCH_RCRB_V0CTL_TVM_MASK         0x7E
#define R_PCH_RCRB_V0STS                  0x201A                      ///< Virtual channel 0 status
#define B_PCH_RCRB_V0STS_NP               BIT1
#define R_PCH_RCRB_V1CTL                  0x2020                      ///< Virtual channel 1 resource control
#define B_PCH_RCRB_V1CTL_EN               BIT31
#define B_PCH_RCRB_V1CTL_ID               (0x0F << 24)                ///< Bit[27:24]
#define N_PCH_RCRB_V1CTL_ID               24
#define V_PCH_RCRB_V1CTL_ETVM_MASK        0xFC00
#define V_PCH_RCRB_V1CTL_TVM_MASK         0xFE
#define R_PCH_RCRB_V1STS                  0x2026                      ///< Virtual channel 1 status
#define B_PCH_RCRB_V1STS_NP               BIT1
#define R_PCH_RCRB_CIR2030                0x2030                      ///< Priority Virtual Channel resource control
#define R_PCH_RCRB_CIR2040                0x2040                      ///< Priority Virtual Channel resource control
#define R_PCH_RCRB_CIR2088                0x2088
#define R_PCH_RCRB_REC                    0x20AC
#define B_PCH_RCRB_REC_DPDP               (BIT31)
#define R_PCH_RCRB_LCAP                   0x21A4                      ///< Link capabilities
#define B_PCH_RCRB_LCAP_EL1               (BIT17 | BIT16 | BIT15)
#define B_PCH_RCRB_LCAP_EL0               (BIT14 | BIT13 | BIT12)
#define B_PCH_RCRB_LCAP_APMS              (BIT11 | BIT10)             ///< L0 is supported on DMI
#define B_PCH_RCRB_LCAP_MLW               0x000003F0
#define B_PCH_RCRB_LCAP_MLS               0x0000000F
#define R_PCH_RCRB_LCTL                   0x21A8                      ///< Link control
#define B_PCH_RCRB_LCTL_ES                BIT7
#define B_PCH_RCRB_LCTL_APMC              (BIT1 | BIT0)
#define V_PCH_RCRB_LCTL_APMC_DIS          0x00
#define V_PCH_RCRB_LCTL_APMC_L0S_EN       0x01
#define R_PCH_RCRB_LSTS                   0x21AA
#define B_PCH_RCRB_LSTS_NLW               0x03F0
#define B_PCH_RCRB_LSTS_LS                0x000F
#define R_PCH_RCRB_DMIC                   0x2234                      ///< DMI control register
#define B_PCH_H_RCRB_DMIC_DMICGEN         (BIT3 | BIT2 | BIT1 | BIT0) ///< DMI Clock Gate Enable for PCH H
#define B_PCH_LP_RCRB_DMIC_DMICGEN        (BIT3 | BIT2 | BIT1 | BIT0) ///< DMI Clock Gate Enable for PCH LP
#define R_PCH_RCRB_CIR2238                0x2238                      ///< Chipset Initialization Register 2238
#define R_PCH_RCBA_CIR228C                0x228C                      ///< Chipset Initialization Register 228C
#define R_PCH_RCRB_DMC                    0x2304                      ///< DMI Miscellaneous Control Register
#define R_PCH_RCRB_CIR2314                0x2314
#define R_PCH_RCRB_CIR2320                0x2320
#define R_PCH_RCRB_IOBPIRI                0x2330                      ///< IOBP Indexed Register Index
#define B_PCH_RCRB_IOBPIRI_IOBPIS         0xFF000000                  ///< IOBP Interface Select
#define V_PCH_RCRB_IOBPIRI_IOBPIS_SATA    0xEA000000                  ///< SATA (Ports 0 and Ports 1)
#define V_PCH_RCRB_IOBPIRI_IOBPIS_DMI     0xEB000000                  ///< DMI
#define V_PCH_RCRB_IOBPIRI_IOBPIS_PCIE    0xEC000000                  ///< PCIe
#define B_PCH_RCRB_IOBPIRI_IOBPFS         0xFFFF                      ///< IOBP Function Select
#define R_PCH_RCRB_IOBPD                  0x2334                      ///< IOBP Indexed Register Data
#define R_PCH_RCRB_IOBPS                  0x2338                      ///< IOBP Status
#define B_PCH_RCRB_IOBPS_IOBPIA           0xFF00                      ///< IOBP Interface Access
#define V_PCH_RCRB_IOBPS_IOBPIA_R         0x0600                      ///< Read access
#define V_PCH_RCRB_IOBPS_IOBPIA_W         0x0700                      ///< Write access
#define B_PCH_RCRB_IOBPS                  (BIT2 | BIT1)               ///< Status for the transaction
#define V_PCH_RCRB_IOBPS_SUCCESS          0                           ///< Successful
#define V_PCH_RCRB_IOBPS_UNSUCCESS        BIT1                        ///< Unsuccessful
#define V_PCH_RCRB_IOBPS_POWEREDDOWN      BIT2                        ///< Powered Down
#define B_PCH_RCRB_IOBPS_BUSY             BIT0
#define R_PCH_RCRB_TCTL                   0x3000                      ///< TCO Configuration register
#define B_PCH_RCRB_TCTL_IE                BIT7                        ///< TCO IRQ Enable
#define B_PCH_RCRB_TCTL_IS                (BIT2 | BIT1 | BIT0)        ///< TCO IRQ Select
#define V_PCH_RCRB_TCTL_IRQ_9             0x00
#define V_PCH_RCRB_TCTL_IRQ_10            0x01
#define V_PCH_RCRB_TCTL_IRQ_11            0x02
#define V_PCH_RCRB_TCTL_IRQ_20            0x04                        ///< only if APIC enabled
#define V_PCH_RCRB_TCTL_IRQ_21            0x05                        ///< only if APIC enabled
#define V_PCH_RCRB_TCTL_IRQ_22            0x06                        ///< only if APIC enabled
#define V_PCH_RCRB_TCTL_IRQ_23            0x07                        ///< only if APIC enabled
#define R_PCH_RCRB_D31IP                  0x3100                      ///< Device 31 interrupt pin
#define B_PCH_RCRB_D31IP_TTIP             (BIT27 | BIT26 | BIT25 | BIT24)
#define V_PCH_RCRB_D31IP_TTIP_INTA        (1 << 24)
#define V_PCH_RCRB_D31IP_TTIP_INTB        (2 << 24)
#define V_PCH_RCRB_D31IP_TTIP_INTC        (3 << 24)
#define V_PCH_RCRB_D31IP_TTIP_INTD        (4 << 24)
#define B_PCH_RCRB_D31IP_SIP2             (BIT23 | BIT22 | BIT21 | BIT20)
#define V_PCH_RCRB_D31IP_SIP2_INTA        (1 << 20)
#define V_PCH_RCRB_D31IP_SIP2_INTB        (2 << 20)
#define V_PCH_RCRB_D31IP_SIP2_INTC        (3 << 20)
#define V_PCH_RCRB_D31IP_SIP2_INTD        (4 << 20)
#define B_PCH_RCRB_D31IP_SMIP             (BIT15 | BIT14 | BIT13 | BIT12)
#define V_PCH_RCRB_D31IP_SMIP_INTA        (1 << 12)
#define V_PCH_RCRB_D31IP_SMIP_INTB        (2 << 12)
#define V_PCH_RCRB_D31IP_SMIP_INTC        (3 << 12)
#define V_PCH_RCRB_D31IP_SMIP_INTD        (4 << 12)
#define B_PCH_RCRB_D31IP_SIP              (BIT11 | BIT10 | BIT9 | BIT8)
#define V_PCH_RCRB_D31IP_SIP_INTA         (1 << 8)
#define V_PCH_RCRB_D31IP_SIP_INTB         (2 << 8)
#define V_PCH_RCRB_D31IP_SIP_INTC         (3 << 8)
#define V_PCH_RCRB_D31IP_SIP_INTD         (4 << 8)
#define B_PCH_RCRB_D31IP_LIP              (BIT3 | BIT2 | BIT1 | BIT0)
#define R_PCH_RCRB_D29IP                  0x3108                      ///< Device 29 interrupt pin
#define B_PCH_RCRB_D29IP_E1P              (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_RCRB_D29IP_E1P_INTA         (1 << 0)
#define V_PCH_RCRB_D29IP_E1P_INTB         (2 << 0)
#define V_PCH_RCRB_D29IP_E1P_INTC         (3 << 0)
#define V_PCH_RCRB_D29IP_E1P_INTD         (4 << 0)
#define R_PCH_RCRB_D28IP                  0x310C                      ///< Device 28 interrupt pin
#define B_PCH_RCRB_D28IP_P8IP             (BIT31 | BIT30 | BIT29 | BIT28)
#define V_PCH_RCRB_D28IP_P8IP_INTA        (1 << 28)
#define V_PCH_RCRB_D28IP_P8IP_INTB        (2 << 28)
#define V_PCH_RCRB_D28IP_P8IP_INTC        (3 << 28)
#define V_PCH_RCRB_D28IP_P8IP_INTD        (4 << 28)
#define B_PCH_RCRB_D28IP_P7IP             (BIT27 | BIT26 | BIT25 | BIT24)
#define V_PCH_RCRB_D28IP_P7IP_INTA        (1 << 24)
#define V_PCH_RCRB_D28IP_P7IP_INTB        (2 << 24)
#define V_PCH_RCRB_D28IP_P7IP_INTC        (3 << 24)
#define V_PCH_RCRB_D28IP_P7IP_INTD        (4 << 24)
#define B_PCH_RCRB_D28IP_P6IP             (BIT23 | BIT22 | BIT21 | BIT20)
#define V_PCH_RCRB_D28IP_P6IP_INTA        (1 << 20)
#define V_PCH_RCRB_D28IP_P6IP_INTB        (2 << 20)
#define V_PCH_RCRB_D28IP_P6IP_INTC        (3 << 20)
#define V_PCH_RCRB_D28IP_P6IP_INTD        (4 << 20)
#define B_PCH_RCRB_D28IP_P5IP             (BIT19 | BIT18 | BIT17 | BIT16)
#define V_PCH_RCRB_D28IP_P5IP_INTA        (1 << 16)
#define V_PCH_RCRB_D28IP_P5IP_INTB        (2 << 16)
#define V_PCH_RCRB_D28IP_P5IP_INTC        (3 << 16)
#define V_PCH_RCRB_D28IP_P5IP_INTD        (4 << 16)
#define B_PCH_RCRB_D28IP_P4IP             (BIT15 | BIT14 | BIT13 | BIT12)
#define V_PCH_RCRB_D28IP_P4IP_INTA        (1 << 12)
#define V_PCH_RCRB_D28IP_P4IP_INTB        (2 << 12)
#define V_PCH_RCRB_D28IP_P4IP_INTC        (3 << 12)
#define V_PCH_RCRB_D28IP_P4IP_INTD        (4 << 12)
#define B_PCH_RCRB_D28IP_P3IP             (BIT11 | BIT10 | BIT9 | BIT8)
#define V_PCH_RCRB_D28IP_P3IP_INTA        (1 << 8)
#define V_PCH_RCRB_D28IP_P3IP_INTB        (2 << 8)
#define V_PCH_RCRB_D28IP_P3IP_INTC        (3 << 8)
#define V_PCH_RCRB_D28IP_P3IP_INTD        (4 << 8)
#define B_PCH_RCRB_D28IP_P2IP             (BIT7 | BIT6 | BIT5 | BIT4)
#define V_PCH_RCRB_D28IP_P2IP_INTA        (1 << 4)
#define V_PCH_RCRB_D28IP_P2IP_INTB        (2 << 4)
#define V_PCH_RCRB_D28IP_P2IP_INTC        (3 << 4)
#define V_PCH_RCRB_D28IP_P2IP_INTD        (4 << 4)
#define B_PCH_RCRB_D28IP_P1IP             (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_RCRB_D28IP_P1IP_INTA        (1 << 0)
#define V_PCH_RCRB_D28IP_P1IP_INTB        (2 << 0)
#define V_PCH_RCRB_D28IP_P1IP_INTC        (3 << 0)
#define V_PCH_RCRB_D28IP_P1IP_INTD        (4 << 0)
#define R_PCH_RCRB_D27IP                  0x3110                      ///< Device 27 interrupt pin
#define B_PCH_RCRB_D27IP_ZIP              (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_RCRB_D27IP_ZIP_INTA         (1 << 0)
#define V_PCH_RCRB_D27IP_ZIP_INTB         (2 << 0)
#define V_PCH_RCRB_D27IP_ZIP_INTC         (3 << 0)
#define V_PCH_RCRB_D27IP_ZIP_INTD         (4 << 0)
#define R_PCH_RCRB_D26IP                  0x3114                      ///< Device 26 interrupt pin
#define B_PCH_RCRB_D26IP_E2P              (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_RCRB_D26IP_E2P_INTA         (1 << 0)
#define V_PCH_RCRB_D26IP_E2P_INTB         (2 << 0)
#define V_PCH_RCRB_D26IP_E2P_INTC         (3 << 0)
#define V_PCH_RCRB_D26IP_E2P_INTD         (4 << 0)
#define R_PCH_RCRB_D25IP                  0x3118                      ///< Device 25 interrupt pin
#define B_PCH_RCRB_D25IP_LIP              (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_RCRB_D25IP_LIP_INTA         (1 << 0)
#define V_PCH_RCRB_D25IP_LIP_INTB         (2 << 0)
#define V_PCH_RCRB_D25IP_LIP_INTC         (3 << 0)
#define V_PCH_RCRB_D25IP_LIP_INTD         (4 << 0)
#define R_PCH_RCRB_D22IP                  0x3124                      ///< Device 22 interrupt pin
#define B_PCH_RCRB_D22IP_KTIP             (BIT15 | BIT14 | BIT13 | BIT12)
#define V_PCH_RCRB_D22IP_KTIP_INTA        (1 << 12)
#define V_PCH_RCRB_D22IP_KTIP_INTB        (2 << 12)
#define V_PCH_RCRB_D22IP_KTIP_INTC        (3 << 12)
#define V_PCH_RCRB_D22IP_KTIP_INTD        (4 << 12)
#define B_PCH_RCRB_D22IP_IDERIP           (BIT11 | BIT10 | BIT9 | BIT8)
#define V_PCH_RCRB_D22IP_IDERIP_INTA      (1 << 8)
#define V_PCH_RCRB_D22IP_IDERIP_INTB      (2 << 8)
#define V_PCH_RCRB_D22IP_IDERIP_INTC      (3 << 8)
#define V_PCH_RCRB_D22IP_IDERIP_INTD      (4 << 8)
#define B_PCH_RCRB_D22IP_MEI2IP           (BIT7 | BIT6 | BIT5 | BIT4)
#define V_PCH_RCRB_D22IP_MEI2IP_INTA      (1 << 4)
#define V_PCH_RCRB_D22IP_MEI2IP_INTB      (2 << 4)
#define V_PCH_RCRB_D22IP_MEI2IP_INTC      (3 << 4)
#define V_PCH_RCRB_D22IP_MEI2IP_INTD      (4 << 4)
#define B_PCH_RCRB_D22IP_MEI1IP           (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_RCRB_D22IP_MEI1IP_INTA      (1 << 0)
#define V_PCH_RCRB_D22IP_MEI1IP_INTB      (2 << 0)
#define V_PCH_RCRB_D22IP_MEI1IP_INTC      (3 << 0)
#define V_PCH_RCRB_D22IP_MEI1IP_INTD      (4 << 0)
#define R_PCH_RCRB_D20IP                  0x3128                      ///< Device 20 interrupt pin
#define B_PCH_RCRB_D20IP_XHCIP            (BIT3 | BIT2 | BIT1 | BIT0)
#define V_PCH_RCRB_D20IP_XHCIP_INTA       (1 << 0)
#define V_PCH_RCRB_D20IP_XHCIP_INTB       (2 << 0)
#define V_PCH_RCRB_D20IP_XHCIP_INTC       (3 << 0)
#define V_PCH_RCRB_D20IP_XHCIP_INTD       (4 << 0)
#define R_PCH_RCRB_D31IR                  0x3140                      ///< Device 31 interrupt route
#define R_PCH_RCRB_D29IR                  0x3144                      ///< Device 29 interrupt route
#define R_PCH_RCRB_D28IR                  0x3146                      ///< Device 28 interrupt route
#define R_PCH_RCRB_D27IR                  0x3148                      ///< Device 27 interrupt route
#define R_PCH_RCRB_D26IR                  0x314C                      ///< Device 26 interrupt route
#define R_PCH_RCRB_D25IR                  0x3150                      ///< Device 25 interrupt route
#define R_PCH_RCRB_D23IR                  0x3158                      ///< Device 23 interrupt route
#define R_PCH_RCRB_D22IR                  0x315C                      ///< Device 22 interrupt route
#define R_PCH_RCRB_D20IR                  0x3160                      ///< Device 20 interrupt route
#define R_PCH_RCRB_D21IR                  0x3164                      ///< Device 21 interrupt route
#define B_PCH_RCRB_D21IR_IE               BIT15
#define R_PCH_RCRB_D19IR                  0x3168                      ///< Device 19 interrupt route
#define B_PCH_RCRB_D19IR_IS               (BIT19 | BIT18 | BIT17 | BIT16)
#define B_PCH_RCRB_D19IR_IE               BIT15
#define B_PCH_RCRB_DXXIR_IDR_MASK         (BIT14 | BIT13 | BIT12)
#define V_PCH_RCRB_DXXIR_IDR_PIRQA        0
#define V_PCH_RCRB_DXXIR_IDR_PIRQB        BIT12
#define V_PCH_RCRB_DXXIR_IDR_PIRQC        BIT13
#define V_PCH_RCRB_DXXIR_IDR_PIRQD        (BIT13 | BIT12)
#define V_PCH_RCRB_DXXIR_IDR_PIRQE        BIT14
#define V_PCH_RCRB_DXXIR_IDR_PIRQF        (BIT14 | BIT12)
#define V_PCH_RCRB_DXXIR_IDR_PIRQG        (BIT14 | BIT13)
#define V_PCH_RCRB_DXXIR_IDR_PIRQH        (BIT14 | BIT13 | BIT12)
#define V_PCH_RCRB_DXXIR_ICR_PIRQA        0
#define V_PCH_RCRB_DXXIR_ICR_PIRQB        BIT8
#define V_PCH_RCRB_DXXIR_ICR_PIRQC        BIT9
#define V_PCH_RCRB_DXXIR_ICR_PIRQD        (BIT9 | BIT8)
#define V_PCH_RCRB_DXXIR_ICR_PIRQE        BIT10
#define V_PCH_RCRB_DXXIR_ICR_PIRQF        (BIT10 | BIT8)
#define V_PCH_RCRB_DXXIR_ICR_PIRQG        (BIT10 | BIT9)
#define V_PCH_RCRB_DXXIR_ICR_PIRQH        (BIT10 | BIT9 | BIT8)
#define V_PCH_RCRB_DXXIR_IBR_PIRQA        0
#define V_PCH_RCRB_DXXIR_IBR_PIRQB        BIT4
#define V_PCH_RCRB_DXXIR_IBR_PIRQC        BIT5
#define V_PCH_RCRB_DXXIR_IBR_PIRQD        (BIT5 | BIT4)
#define V_PCH_RCRB_DXXIR_IBR_PIRQE        BIT6
#define V_PCH_RCRB_DXXIR_IBR_PIRQF        (BIT6 | BIT4)
#define V_PCH_RCRB_DXXIR_IBR_PIRQG        (BIT6 | BIT5)
#define V_PCH_RCRB_DXXIR_IBR_PIRQH        (BIT6 | BIT5 | BIT4)
#define V_PCH_RCRB_DXXIR_IAR_PIRQA        0
#define V_PCH_RCRB_DXXIR_IAR_PIRQB        BIT0
#define V_PCH_RCRB_DXXIR_IAR_PIRQC        BIT1
#define V_PCH_RCRB_DXXIR_IAR_PIRQD        (BIT1 | BIT0)
#define V_PCH_RCRB_DXXIR_IAR_PIRQE        BIT2
#define V_PCH_RCRB_DXXIR_IAR_PIRQF        (BIT2 | BIT0)
#define V_PCH_RCRB_DXXIR_IAR_PIRQG        (BIT2 | BIT1)
#define V_PCH_RCRB_DXXIR_IAR_PIRQH        (BIT2 | BIT1 | BIT0)
#define R_PCH_RCRB_OIC                    0x31FE                      ///< Other Interrupt Control
#define B_PCH_RCRB_OIC_OA24_39_D          BIT11
#define B_PCH_RCRB_OIC_CEN                BIT9                        ///< Coprocessor Error Enable
#define B_PCH_RCRB_OIC_AEN                BIT8                        ///< APIC Enable
#define V_PCH_RCRB_OIC_ASEL               0xFF
#define R_PCH_IO_APIC_INDEX               0xFEC00000
#define R_PCH_IO_APIC_DATA                0xFEC00010
#define N_PCH_IO_APIC_ASEL                12
#define R_PCH_RCRB_PRSTS                  0x3310
#define B_PCH_RCRB_PRSTS_PM_WD_TMR        BIT15                       ///< Power Management Watchdog Timer
#define B_PCH_RCRB_PRSTS_VE_WD_TMR_STS    BIT7                        ///< VE Watchdog Timer Status
#define B_PCH_RCRB_PRSTS_ME_WD_TMR_STS    BIT6                        ///< Management Engine Watchdog Timer Status
#define B_PCH_RCRB_PRSTS_WOL_OVR_WK_STS   BIT5
#define B_PCH_RCRB_PRSTS_FIELD_1          BIT4
#define B_PCH_RCRB_PRSTS_ME_HOST_PWRDN    BIT3
#define B_PCH_RCRB_PRSTS_ME_HRST_WARM_STS BIT2
#define B_PCH_RCRB_PRSTS_ME_HRST_COLD_STS BIT1
#define B_PCH_RCRB_PRSTS_ME_WAKE_STS      BIT0
#define R_PCH_RCRB_CIR3314                0x3314
#define R_PCH_RCRB_PM_CFG                 0x3318                      ///< Power Management Configuration
#define R_PCH_RCRB_PM_CFG_RTC_DS_WAKE_DIS BIT21                       ///< RTC Wake from Deep S4/S5 Disable
#define B_PCH_RCRB_PM_CFG_SSMAW_MASK      (BIT19 | BIT18)             ///< SLP_SUS# Min Assertion Width
#define V_PCH_RCRB_PM_CFG_SSMAW_4S        (BIT19 | BIT18)             ///< 4 seconds
#define V_PCH_RCRB_PM_CFG_SSMAW_1S        BIT19                       ///< 1 second
#define V_PCH_RCRB_PM_CFG_SSMAW_0_5S      BIT18                       ///< 0.5 second (500ms)
#define V_PCH_RCRB_PM_CFG_SSMAW_0S        0                           ///< 0 second
#define B_PCH_RCRB_PM_CFG_SAMAW_MASK      (BIT17 | BIT16)             ///< SLP_A# Min Assertion Width
#define V_PCH_RCRB_PM_CFG_SAMAW_2S        (BIT17 | BIT16)             ///< 2 seconds
#define V_PCH_RCRB_PM_CFG_SAMAW_98ms      BIT17                       ///< 98ms
#define V_PCH_RCRB_PM_CFG_SAMAW_4S        BIT16                       ///< 4 seconds
#define V_PCH_RCRB_PM_CFG_SAMAW_0S        0                           ///< 0 second
#define B_PCH_RCRB_PM_CFG_RPCD_MASK       (BIT9 | BIT8)               ///< Reset Power Cycle Duration
#define V_PCH_RCRB_PM_CFG_RPCD_1S         (BIT9 | BIT8)               ///< 1-2 seconds
#define V_PCH_RCRB_PM_CFG_RPCD_2S         BIT9                        ///< 2-3 seconds
#define V_PCH_RCRB_PM_CFG_RPCD_3S         BIT8                        ///< 3-4 seconds
#define V_PCH_RCRB_PM_CFG_RPCD_4S         0                           ///< 4-5 seconds (Default)
#define R_PCH_RCRB_CIR3324                0x3324
#define R_PCH_RCRB_DEEP_S3_POL            0x3328                      ///< Deep S3 Power Policies
#define B_PCH_RCRB_DEEP_S3_POL_DPS3_EN_DC BIT1                        ///< Deep S3 Enable in DC Mode
#define B_PCH_RCRB_DEEP_S3_POL_DPS3_EN_AC BIT0                        ///< Deep S3 Enable in AC Mode
#define R_PCH_RCRB_DEEP_S4_POL            0x332C                      ///< Deep S4 Power Policies
#define B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_DC BIT1                        ///< Deep S4 Enable in DC Mode
#define B_PCH_RCRB_DEEP_S4_POL_DPS4_EN_AC BIT0                        ///< Deep S4 Enable in AC Mode
#define R_PCH_RCRB_DEEP_S5_POL            0x3330                      ///< Deep S5 Power Policies
#define B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_DC BIT15                       ///< Deep S5 Enable in DC Mode
#define B_PCH_RCRB_DEEP_S5_POL_DPS5_EN_AC BIT14                       ///< Deep S5 Enable in AC Mode
#define R_PCH_RCRB_PM_CFG2                0x333C                      ///< Power Management Configuration Reg 2
#define B_PCH_RCRB_PM_CFG2_DRAM_RESET_CTL (1 << 26)                   ///< DRAM RESET# control
#define R_PCH_RCRB_CIR3340                0x3340
#define R_PCH_RCRB_CIR3344                0x3344
#define R_PCH_RCRB_CIR3348                0x3348
#define R_PCH_RCRB_CIR3350                0x3350
#define R_PCH_RCRB_CIR3360                0x3360
#define R_PCH_RCRB_CIR3368                0x3368
#define R_PCH_RCRB_CIR3378                0x3378
#define R_PCH_RCRB_CIR337C                0x337C
#define R_PCH_RCRB_CIR3388                0x3388
#define R_PCH_RCRB_CIR3390                0x3390
#define R_PCH_RCRB_CIR33A0                0x33A0
#define R_PCH_RCRB_CIR33B0                0x33B0
#define R_PCH_RCRB_CIR33C0                0x33C0
#define PMSYNC_TPR_CONFIG                 0x33C4
#define B_PMSYNC_TPR_CONFIG_LOCK          BIT31
#define PMSYNC_TPR_CONFIG2                0x33CC
#define R_PCH_RCRB_PMSYNC                 0x33C8
#define B_PCH_RCRB_PMSYNC_GPIO_D_SEL      BIT11
#define B_PCH_RCRB_PMSYNC_GPIO_C_SEL      BIT10
#define B_PCH_RCRB_PMSYNC_GPIO_B_SEL      BIT9
#define B_PCH_RCRB_PMSYNC_GPIO_A_SEL      BIT8
#define R_PCH_RCRB_CIR33D0                0x33D0
#define R_PCH_RCRB_CIR33D4                0x33D4
#define R_PCH_RCRB_RTC_CONF               0x3400                      ///< RTC Configuration register
#define S_PCH_RCRB_RTC_CONF               4
#define B_PCH_RCRB_RTC_CONF_UCMOS_LOCK    BIT4
#define B_PCH_RCRB_RTC_CONF_LCMOS_LOCK    BIT3
#define B_PCH_RCRB_RTC_CONF_UCMOS_EN      BIT2                        ///< Upper CMOS bank enable
#define R_PCH_RCRB_HPTC                   0x3404                      ///< High Performance Timer Configuration
#define B_PCH_RCRB_HPTC_AE                BIT7                        ///< Address enable
#define B_PCH_RCRB_HPTC_AS                (BIT1 | BIT0)               ///< Address selection
#define R_PCH_PCH_HPET_CONFIG             0xFED00000
#define N_PCH_HPET_ADDR_ASEL              12
#define R_PCH_RCRB_GCS                    0x3410                      ///< General Control and Status
#define B_PCH_RCRB_GCS_FLRCSSEL           BIT12
#define B_PCH_H_RCRB_GCS_BBS              (BIT11 | BIT10)             ///< Boot BIOS straps for Pch-H
#define V_PCH_H_RCRB_GCS_BBS_SPI          (3 << 10)                   ///< Boot BIOS strapped to SPI for Pch-H
#define V_PCH_H_RCRB_GCS_BBS_LPC          (0 << 10)                   ///< Boot BIOS strapped to LPC for Pch-H
#define B_PCH_LP_RCRB_GCS_BBS             BIT10                       ///< Boot BIOS straps for Pch-Lp
#define V_PCH_LP_RCRB_GCS_BBS_SPI         0                           ///< Boot BIOS strapped to SPI for Pch-Lp
#define V_PCH_LP_RCRB_GCS_BBS_LPC         BIT10                       ///< Boot BIOS strapped to LPC for Pch-Lp
#define B_PCH_RCRB_GCS_SERM               BIT9                        ///< Server Error Reporting Mode
#define B_PCH_RCRB_GCS_NR                 BIT5                        ///< No Reboot strap
#define B_PCH_RCRB_GCS_AME                BIT4                        ///< Alternate Access Mode Enable
#define B_PCH_RCRB_GCS_SPS                BIT3                        ///< Shutdown Policy Select
#define B_PCH_RCRB_GCS_RPR                BIT2                        ///< Reserved Page Route
#define B_PCH_RCRB_GCS_BILD               BIT0                        ///< BIOS Interface Lock-Down
#define R_PCH_RCRB_BUC                    0x3414                      ///< Backed Up Control
#define B_PCH_RCRB_BUC_LAN_DIS            BIT5                        ///< LAN Disable
#define B_PCH_RCRB_BUC_SDO                BIT4                        ///< Daylight Savings Override
#define B_PCH_RCRB_BUC_TS                 BIT0                        ///< Top Swap
#define R_PCH_RCRB_FUNC_DIS               0x3418                      ///< Function Disable Register
#define B_PCH_RCRB_FUNC_DIS_XHCI          BIT27                       ///< XHCI controller disable
#define B_PCH_RCRB_FUNC_DIS_SATA2         BIT25                       ///< Serial ATA 2 disable
#define B_PCH_RCRB_FUNC_DIS_THERMAL       BIT24                       ///< Thermal Throttle Disable
#define B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT8  BIT23                       ///< PCI Express port 8 disable
#define B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT7  BIT22                       ///< PCI Express port 7 disable
#define B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT6  BIT21                       ///< PCI Express port 6 disable
#define B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT5  BIT20                       ///< PCI Express port 5 disable
#define B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT4  BIT19                       ///< PCI Express port 4 disable
#define B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT3  BIT18                       ///< PCI Express port 3 disable
#define B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT2  BIT17                       ///< PCI Express port 2 disable
#define B_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1  BIT16                       ///< PCI Express port 1 disable
#define N_PCH_RCRB_FUNC_DIS_PCI_EX_PORT1  16
#define B_PCH_RCRB_FUNC_DIS_EHCI1         BIT15                       ///< EHCI controller 1 disable
#define B_PCH_RCRB_FUNC_DIS_LPC_BRIDGE    BIT14                       ///< LPC Bridge disable
#define B_PCH_RCRB_FUNC_DIS_EHCI2         BIT13                       ///< EHCI controller 2 disable
#define B_PCH_RCRB_FUNC_DIS_AZALIA        BIT4                        ///< Azalia disable
#define B_PCH_RCRB_FUNC_DIS_SMBUS         BIT3                        ///< SMBUS disable
#define B_PCH_RCRB_FUNC_DIS_SATA1         BIT2                        ///< Serial ATA disable
#define B_PCH_RCRB_FUNC_DIS_ADSP          BIT1                        ///< Audio DSP disable
#define B_PCH_RCRB_FUNC_DIS_FUNCTION_0    BIT0                        ///< Function 0 disable
#define R_PCH_RCRB_CG                     0x341C                      ///< Clock Gating
#define B_PCH_RCRB_CG_EN_DCG_BLA          BIT30                       ///< Platform Essential Cluster BLA unit Dynamic Clock Gate Enable
#define B_PCH_RCRB_CG_EN_SCG_GSX          BIT29                       ///< GSX Static Clock Gate Enable
#define B_PCH_RCRB_CG_EN_DCG_GPIO         BIT28                       ///< GPIO Dynamic Clock Gate Enable
#define B_PCH_RCRB_CG_EN_DCG_HPET         BIT27                       ///< HPET Dynamic Clock Gate Enable
#define B_PCH_RCRB_CG_EN_CG_GPEC          BIT26                       ///< Generic Platform Essential Clock Gate Enable
#define B_PCH_RCRB_CG_EN_SCG_8254         BIT25                       ///< 8254 Static Clock Gate Enable
#define B_PCH_RCRB_CG_EN_SCG_8237         BIT24                       ///< 8237 Static Clock Gate Enable

#define B_PCH_RCRB_CG_EN_DCG_LPC          BIT31                       ///< Legacy(LPC) Dynamic Clock Gate Enable
#define B_PCH_RCRB_CG_EN_SCG_LAN          BIT23                       ///< LAN Static Clock Gate Enable
#define B_PCH_RCRB_CG_EN_DCG_HDA          BIT22                       ///< HDA Dynamic Clock Gate Enable
#define B_PCH_RCRB_CG_EN_SCG_HDA          BIT21                       ///< HDA Static Clock Gate Enable
#define B_PCH_RCRB_CG_EN_DCG_PCI          BIT16                       ///< PCI Dynamic Clock Gate Enable
#define B_PCH_RCRB_CG_EN_CG_SMBUS         BIT5                        ///< SMBUS Static Clock Gating Enable
#define R_PCH_RCRB_FDSW                   0x3420                      ///< Function Disable SUS well
#define B_PCH_RCRB_FDSW_FDSWL             BIT7                        ///< Function Disable SUS well lockdown
#define R_PCH_RCRB_DISPBDF                0x3424                      ///< Display Bus, Device and Function Initialization
#define B_PCH_RCRB_DISPBDF_DBN            0xFF00                      ///< Display Bus Number
#define B_PCH_RCRB_DISPBDF_DDN            0x00F8                      ///< Display Device Number
#define B_PCH_RCRB_DISPBDF_DFN            0x0007                      ///< Display Function Number
#define R_PCH_RCRB_FD2                    0x3428                      ///< Function Disable 2
#define B_PCH_RCRB_FD2_KTD                BIT4                        ///< KT Disable
#define B_PCH_RCRB_FD2_IRERD              BIT3                        ///< IDE-R Disable
#define B_PCH_RCRB_FD2_MEI2D              BIT2                        ///< Intel MEI #2 Disable
#define B_PCH_RCRB_FD2_MEI1D              BIT1                        ///< Intel MEI #1 Disable
#define B_PCH_RCRB_FD2_DBDFEN             BIT0                        ///< Display BDF Enable
#define R_PCH_RCRB_CIR3A28                0x3A28
#define R_PCH_RCRB_CIR3A2C                0x3A2C
#define R_PCH_RCRB_CIR3A6C                0x3A6C
#define R_PCH_RCRB_CIR3A80                0x3A80
#define R_PCH_RCRB_CIR3A84                0x3A84
#define R_PCH_RCRB_CIR3A88                0x3A88
#define R_PCH_RCRB_GSX_CTRL               0x3454                      ///< GSX Control
#define B_PCH_RCRB_GSX_BAR_ENABLE         BIT4                        ///< GSX BAR Enable
#define R_PCH_RCRB_INT_ACPIIRQEN          0x31E0                      ///< ACPI IRQ Control

#define B_PCH_RCRB_INT_ACPIIRQEN_A15E     BIT15                       ///< ACPI IRQ15 Enable
#define B_PCH_RCRB_INT_ACPIIRQEN_A14E     BIT14                       ///< ACPI IRQ14 Enable
#define B_PCH_RCRB_INT_ACPIIRQEN_A13E     BIT13                       ///< ACPI IRQ13 Enable
#define B_PCH_RCRB_INT_ACPIIRQEN_A7E      BIT7                        ///< ACPI IRQ7 Enable
#define B_PCH_RCRB_INT_ACPIIRQEN_A6E      BIT6                        ///< ACPI IRQ6 Enable
#define B_PCH_RCRB_INT_ACPIIRQEN_A5E      BIT5                        ///< ACPI IRQ5 Enable
#define B_PCH_RCRB_INT_ACPIIRQEN_A4E      BIT4                        ///< ACPI IRQ4 Enable
#define B_PCH_RCRB_INT_ACPIIRQEN_A3E      BIT3                        ///< ACPI IRQ3 Enable
#endif
