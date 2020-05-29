/** @file
  Register names for PCH LPC device

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
#ifndef _PCH_REGS_LPC_H_
#define _PCH_REGS_LPC_H_

//
// PCI to LPC Bridge Registers (D31:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

#define PCH_HPET_BDF_MAX                8

typedef enum {  
  LptHB0         = 0,
  LptHC0,
  LptHC1,
  LptHC2,
  LptLpB0,
  LptLpB1,
  LptLpB2,
  PchSteppingMax
} PCH_STEPPING;

#define R_PCH_LPC_VENDOR_ID                       0x00
#define V_PCH_LPC_VENDOR_ID                       V_PCH_INTEL_VENDOR_ID
#define R_PCH_LPC_DEVICE_ID                       0x02

//
// LynxPoint Desktop LPC Device IDs
//
#define V_PCH_LPTH_LPC_DEVICE_ID_DT_SUPER_SKU      0x8C42          ///< LynxPoint Desktop Super SKU
#define V_PCH_LPTH_LPC_DEVICE_ID_DT_0              0x8C44          ///< Intel Z87 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_DT_1              0x8C46          ///< Intel Z85 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_DT_2              0x8C4A          ///< Intel H87 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_DT_3              0x8C4C          ///< Intel Q85 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_DT_4              0x8C4E          ///< Intel Q87 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_DT_5              0x8C50          ///< Intel B85 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_DT_6              0x8C5C          ///< Intel H81 Chipset

//
// LynxPoint Mobile LPC Device IDs
//
#define V_PCH_LPTH_LPC_DEVICE_ID_MB_SUPER_SKU      0x8C41          ///< LynxPoint Mobile Super SKU
#define V_PCH_LPTH_LPC_DEVICE_ID_MB_0              0x8C49          ///< Intel HM86 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_MB_1              0x8C4B          ///< Intel HM87 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_MB_2              0x8C4F          ///< Intel QM87 Chipset

//
// Lynxpoint Server/WS LPC Device IDs
//
#define V_PCH_LPTH_LPC_DEVICE_ID_SVR_0             0x8C52          ///< Server Essential SKU Intel C222 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_SVR_1             0x8C54          ///< Server Standard SKU Intel C224 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_SVR_2             0x8C56          ///< Server Advanced SKU Intel C226 Chipset
#define V_PCH_LPTH_LPC_DEVICE_ID_SVR_3             0x8C58          ///< WS SKU

#define V_PCH_LPTLP_LPC_DEVICE_ID_UNFUSE           0x9C40          ///< LynxPoint LP Unfuse
#define V_PCH_LPTLP_LPC_DEVICE_ID_MB_SUPER_SKU     0x9C41          ///< LynxPoint LP Mobile Super SKU
#define V_PCH_LPTLP_LPC_DEVICE_ID_MB_0             0x9C42          ///< LynxPoint LP Mobile TBD SKU
#define V_PCH_LPTLP_LPC_DEVICE_ID_MB_1             0x9C43          ///< LynxPoint LP Mobile Premium SKU
#define V_PCH_LPTLP_LPC_DEVICE_ID_MB_2             0x9C44          ///< LynxPoint LP Mobile TBD SKU
#define V_PCH_LPTLP_LPC_DEVICE_ID_MB_3             0x9C45          ///< LynxPoint LP Mobile Mainstream SKU
#define V_PCH_LPTLP_LPC_DEVICE_ID_MB_4             0x9C46          ///< LynxPoint LP Mobile TBD SKU
#define V_PCH_LPTLP_LPC_DEVICE_ID_MB_5             0x9C47          ///< LynxPoint LP Mobile Value SKU

#define R_PCH_LPC_COMMAND                         0x04
#define B_PCH_LPC_COMMAND_FBE                     0x0200
#define B_PCH_LPC_COMMAND_SERR_EN                 0x0100
#define B_PCH_LPC_COMMAND_WCC                     0x0080
#define B_PCH_LPC_COMMAND_PER                     0x0040
#define B_PCH_LPC_COMMAND_VPS                     0x0020
#define B_PCH_LPC_COMMAND_PMWE                    0x0010
#define B_PCH_LPC_COMMAND_SCE                     0x0008
#define B_PCH_LPC_COMMAND_BME                     0x0004
#define B_PCH_LPC_COMMAND_MSE                     0x0002
#define B_PCH_LPC_COMMAND_IOSE                    0x0001
#define R_PCH_LPC_DEV_STS                         0x06
#define B_PCH_LPC_DEV_STS_DPE                     0x8000
#define B_PCH_LPC_DEV_STS_SSE                     0x4000
#define B_PCH_LPC_DEV_STS_RMA                     0x2000
#define B_PCH_LPC_DEV_STS_RTA                     0x1000
#define B_PCH_LPC_DEV_STS_STA                     0x0800
#define B_PCH_LPC_DEV_STS_DEVT_STS                0x0600
#define B_PCH_LPC_DEV_STS_MDPED                   0x0100
#define B_PCH_LPC_DEV_STS_FB2B                    0x0080
#define B_PCH_LPC_DEV_STS_UDF                     0x0040
#define B_PCH_LPC_DEV_STS_66MHZ_CAP               0x0020
#define R_PCH_LPC_RID                             0x08
#define V_PCH_LPT_LPC_RID_0                       0x00  // A0 stepping
#define V_PCH_LPT_LPC_RID_1                       0x01  // A1 stepping
#define V_PCH_LPT_LPC_RID_2                       0x02  // B0 stepping
#define V_PCH_LPT_LPC_RID_3                       0x03  // C0 stepping
#define V_PCH_LPT_LPC_RID_4                       0x04  // C2 stepping
#define V_PCH_LPT_LPC_RID_5                       0x05
#define R_PCH_LPC_PI                              0x09
#define R_PCH_LPC_SCC                             0x0A
#define R_PCH_LPC_BCC                             0x0B
#define R_PCH_LPC_PLT                             0x0D
#define R_PCH_LPC_HEADTYP                         0x0E
#define B_PCH_LPC_HEADTYP_MFD                     BIT7
#define B_PCH_LPC_HEADTYP_HT                      0x7F
#define R_PCH_LPC_SS                              0x2C
#define B_PCH_LPC_SS_SSID                         0xFFFF0000
#define B_PCH_LPC_SS_SSVID                        0x0000FFFF
#define R_PCH_LPC_ACPI_BASE                       0x40
#define B_PCH_LPC_ACPI_BASE_BAR                   0xFFFC
#define R_PCH_LPC_ACPI_CNT                        0x44
#define B_PCH_LPC_ACPI_CNT_ACPI_EN                0x80
#define B_PCH_LPC_ACPI_CNT_SCI_IRG_SEL            0x07
#define R_PCH_LPC_GPIO_BASE                       0x48
#define B_PCH_LPC_GPIO_BASE_BAR                   0xFFFC
#define R_PCH_LPC_GPIO_CNT                        0x4C
#define B_PCH_LPC_GPIO_CNT_GPIO_EN                0x10
#define B_PCH_LPC_GPIO_LOCKDOWN_EN                0x01
#define R_PCH_LPC_VLW_VBDF                        0x50
#define B_PCH_LPC_VLW_VBDF                        0xFFFF
#define R_PCH_LPC_VLW_VCTRL                       0x54
#define B_PCH_LPC_VLW_VCTRL_VCLE                  BIT15
#define B_PCH_LPC_VLW_VCTRL_FERRVDMDEN            BIT5
#define B_PCH_LPC_VLW_VCTRL_NMIVMEN               BIT4
#define B_PCH_LPC_VLW_VCTRL_INITVMEN              BIT3
#define B_PCH_LPC_VLW_VCTRL_SMIVMEN               BIT2
#define B_PCH_LPC_VLW_VCTRL_INTRVMEN              BIT1
#define B_PCH_LPC_VLW_VCTRL_A20VMEN               BIT0
#define R_PCH_LPC_PIRQA_ROUT                      0x60
#define R_PCH_LPC_PIRQB_ROUT                      0x61
#define R_PCH_LPC_PIRQC_ROUT                      0x62
#define R_PCH_LPC_PIRQD_ROUT                      0x63

//
// Bit values are the same for R_PCH_LPC_PIRQA_ROUT to R_PCH_LPC_PIRQH_ROUT
//
#define B_PCH_LPC_PIRQX_ROUT_IRQEN                0x80
#define B_PCH_LPC_PIRQX_ROUT                      0x0F
#define V_PCH_LPC_PIRQX_ROUT_IRQ_3                0x03
#define V_PCH_LPC_PIRQX_ROUT_IRQ_4                0x04
#define V_PCH_LPC_PIRQX_ROUT_IRQ_5                0x05
#define V_PCH_LPC_PIRQX_ROUT_IRQ_6                0x06
#define V_PCH_LPC_PIRQX_ROUT_IRQ_7                0x07
#define V_PCH_LPC_PIRQX_ROUT_IRQ_9                0x09
#define V_PCH_LPC_PIRQX_ROUT_IRQ_10               0x0A
#define V_PCH_LPC_PIRQX_ROUT_IRQ_11               0x0B
#define V_PCH_LPC_PIRQX_ROUT_IRQ_12               0x0C
#define V_PCH_LPC_PIRQX_ROUT_IRQ_14               0x0E
#define V_PCH_LPC_PIRQX_ROUT_IRQ_15               0x0F
#define R_PCH_LPC_SERIRQ_CNT                      0x64
#define B_PCH_LPC_SERIRQ_CNT_SIRQEN               0x80
#define B_PCH_LPC_SERIRQ_CNT_SIRQMD               0x40
#define B_PCH_LPC_SERIRQ_CNT_SIRQSZ               0x3C
#define N_PCH_LPC_SERIRQ_CNT_SIRQSZ               2
#define B_PCH_LPC_SERIRQ_CNT_SFPW                 0x03
#define N_PCH_LPC_SERIRQ_CNT_SFPW                 0
#define V_PCH_LPC_SERIRQ_CNT_SFPW_4CLK            0x00
#define V_PCH_LPC_SERIRQ_CNT_SFPW_6CLK            0x01
#define V_PCH_LPC_SERIRQ_CNT_SFPW_8CLK            0x02
#define R_PCH_LPC_PIRQE_ROUT                      0x68
#define R_PCH_LPC_PIRQF_ROUT                      0x69
#define R_PCH_LPC_PIRQG_ROUT                      0x6A
#define R_PCH_LPC_PIRQH_ROUT                      0x6B
#define R_PCH_LPC_IOXAPIC                         0x6C
#define B_PCH_LPC_IOXAPIC_BUS                     0xFF00
#define B_PCH_LPC_IOXAPIC_DEVICE                  0x00F8
#define B_PCH_LPC_IOXAPIC_FUNC                    0x0007
#define R_PCH_LPC_HPET0                           0x70
#define B_PCH_LPC_HPET0_BUS                       0xFF00
#define B_PCH_LPC_HPET0_DEVICE                    0x00F8
#define B_PCH_LPC_HPET0_FUNC                      0x0007
#define R_PCH_LPC_HPET1                           0x72
#define B_PCH_LPC_HPET1_BUS                       0xFF00
#define B_PCH_LPC_HPET1_DEVICE                    0x00F8
#define B_PCH_LPC_HPET1_FUNC                      0x0007
#define R_PCH_LPC_HPET2                           0x74
#define B_PCH_LPC_HPET2_BUS                       0xFF00
#define B_PCH_LPC_HPET2_DEVICE                    0x00F8
#define B_PCH_LPC_HPET2_FUNC                      0x0007
#define R_PCH_LPC_HPET3                           0x76
#define B_PCH_LPC_HPET3_BUS                       0xFF00
#define B_PCH_LPC_HPET3_DEVICE                    0x00F8
#define B_PCH_LPC_HPET3_FUNC                      0x0007
#define R_PCH_LPC_HPET4                           0x78
#define B_PCH_LPC_HPET4_BUS                       0xFF00
#define B_PCH_LPC_HPET4_DEVICE                    0x00F8
#define B_PCH_LPC_HPET4_FUNC                      0x0007
#define R_PCH_LPC_HPET5                           0x7A
#define B_PCH_LPC_HPET5_BUS                       0xFF00
#define B_PCH_LPC_HPET5_DEVICE                    0x00F8
#define B_PCH_LPC_HPET5_FUNC                      0x0007
#define R_PCH_LPC_HPET6                           0x7C
#define B_PCH_LPC_HPET6_BUS                       0xFF00
#define B_PCH_LPC_HPET6_DEVICE                    0x00F8
#define B_PCH_LPC_HPET6_FUNC                      0x0007
#define R_PCH_LPC_HPET7                           0x7E
#define B_PCH_LPC_HPET7_BUS                       0xFF00
#define B_PCH_LPC_HPET7_DEVICE                    0x00F8
#define B_PCH_LPC_HPET7_FUNC                      0x0007
#define R_PCH_LPC_IO_DEC                          0x80
#define B_PCH_LPC_FDD_DEC                         0x1000
#define B_PCH_LPC_LPT_DEC                         0x0300
#define B_PCH_LPC_COMB_DEC                        0x0070
#define V_PCH_LPC_COMB_3F8                        0x00
#define V_PCH_LPC_COMB_2F8                        0x10
#define V_PCH_LPC_COMB_220                        0x20
#define V_PCH_LPC_COMB_228                        0x30
#define V_PCH_LPC_COMB_238                        0x40
#define V_PCH_LPC_COMB_2E8                        0x50
#define V_PCH_LPC_COMB_338                        0x60
#define V_PCH_LPC_COMB_3E8                        0x70
#define B_PCH_LPC_COMA_DEC                        0x0007
#define V_PCH_LPC_COMA_3F8                        0x00
#define V_PCH_LPC_COMA_2F8                        0x01
#define V_PCH_LPC_COMA_220                        0x02
#define V_PCH_LPC_COMA_228                        0x03
#define V_PCH_LPC_COMA_238                        0x04
#define V_PCH_LPC_COMA_2E8                        0x05
#define V_PCH_LPC_COMA_338                        0x06
#define V_PCH_LPC_COMA_3E8                        0x07
#define R_PCH_LPC_ENABLES                         0x82
#define B_PCH_LPC_ENABLES_CNF2_EN                 0x2000
#define B_PCH_LPC_ENABLES_CNF1_EN                 0x1000
#define B_PCH_LPC_ENABLES_MC_EN                   0x0800
#define B_PCH_LPC_ENABLES_KBC_EN                  0x0400
#define B_PCH_LPC_ENABLES_GAMEH_EN                0x0200
#define B_PCH_LPC_ENABLES_GAMEL_EN                0x0100
#define B_PCH_LPC_ENABLES_FDD_EN                  0x0008
#define B_PCH_LPC_ENABLES_LPT_EN                  0x0004
#define B_PCH_LPC_ENABLES_COMB_EN                 0x0002
#define B_PCH_LPC_ENABLES_COMA_EN                 0x0001
#define R_PCH_LPC_GEN1_DEC                        0x84
#define B_PCH_LPC_GEN1_DEC_IODRA                  0x00FC0000
#define B_PCH_LPC_GEN1_DEC_IOBAR                  0x0000FFFC
#define B_PCH_LPC_GEN1_DEC_EN                     0x00000001
#define R_PCH_LPC_GEN2_DEC                        0x88
#define R_PCH_LPC_GEN3_DEC                        0x8C
#define R_PCH_LPC_GEN4_DEC                        0x90
#define R_PCH_LPC_ULKMC                           0x94
#define B_PCH_LPC_ULKMC_SMIBYENDPS                BIT15
#define B_PCH_LPC_ULKMC_TRAPBY64W                 BIT11
#define B_PCH_LPC_ULKMC_TRAPBY64R                 BIT10
#define B_PCH_LPC_ULKMC_TRAPBY60W                 BIT9
#define B_PCH_LPC_ULKMC_TRAPBY60R                 BIT8
#define B_PCH_LPC_ULKMC_SMIATENDPS                BIT7
#define B_PCH_LPC_ULKMC_PSTATE                    BIT6
#define B_PCH_LPC_ULKMC_A20PASSEN                 BIT5
#define B_PCH_LPC_ULKMC_USBSMIEN                  BIT4
#define B_PCH_LPC_ULKMC_64WEN                     BIT3
#define B_PCH_LPC_ULKMC_64REN                     BIT2
#define B_PCH_LPC_ULKMC_60WEN                     BIT1
#define B_PCH_LPC_ULKMC_60REN                     BIT0
#define R_PCH_LPC_LGMR                            0x98
#define B_PCH_LPC_LGMR_MA                         0xFFFF0000
#define B_PCH_LPC_LGMR_LMRD_EN                    BIT0

#define R_PCH_LPC_FWH_BIOS_SEL                    0xD0
#define B_PCH_LPC_FWH_BIOS_SEL_F8                 0xF0000000
#define B_PCH_LPC_FWH_BIOS_SEL_F0                 0x0F000000
#define B_PCH_LPC_FWH_BIOS_SEL_E8                 0x00F00000
#define B_PCH_LPC_FWH_BIOS_SEL_E0                 0x000F0000
#define B_PCH_LPC_FWH_BIOS_SEL_D8                 0x0000F000
#define B_PCH_LPC_FWH_BIOS_SEL_D0                 0x00000F00
#define B_PCH_LPC_FWH_BIOS_SEL_C8                 0x000000F0
#define B_PCH_LPC_FWH_BIOS_SEL_C0                 0x0000000F
#define R_PCH_LPC_FWH_BIOS_SEL2                   0xD4
#define B_PCH_LPC_FWH_BIOS_SEL2_70                0xF000
#define B_PCH_LPC_FWH_BIOS_SEL2_60                0x0F00
#define B_PCH_LPC_FWH_BIOS_SEL2_50                0x00F0
#define B_PCH_LPC_FWH_BIOS_SEL2_40                0x000F
#define R_PCH_LPC_FWH_BIOS_DEC                    0xD8
#define B_PCH_LPC_FWH_BIOS_DEC_F8                 0x8000
#define B_PCH_LPC_FWH_BIOS_DEC_F0                 0x4000
#define B_PCH_LPC_FWH_BIOS_DEC_E8                 0x2000
#define B_PCH_LPC_FWH_BIOS_DEC_E0                 0x1000
#define B_PCH_LPC_FWH_BIOS_DEC_D8                 0x0800
#define B_PCH_LPC_FWH_BIOS_DEC_D0                 0x0400
#define B_PCH_LPC_FWH_BIOS_DEC_C8                 0x0200
#define B_PCH_LPC_FWH_BIOS_DEC_C0                 0x0100
#define B_PCH_LPC_FWH_BIOS_LEG_F                  0x0080
#define B_PCH_LPC_FWH_BIOS_LEG_E                  0x0040
#define B_PCH_LPC_FWH_BIOS_DEC_70                 0x0008
#define B_PCH_LPC_FWH_BIOS_DEC_60                 0x0004
#define B_PCH_LPC_FWH_BIOS_DEC_50                 0x0002
#define B_PCH_LPC_FWH_BIOS_DEC_40                 0x0001
#define R_PCH_LPC_BIOS_CNTL                       0xDC
#define S_PCH_LPC_BIOS_CNTL                       1
#define B_PCH_LPC_BIOS_CNTL_SMM_BWP               0x20            ///< SMM BIOS write protect disable
#define B_PCH_LPC_BIOS_CNTL_TSS                   0x10
#define V_PCH_LPC_BIOS_CNTL_SRC                   0x0C
#define V_PCH_SRC_PREF_EN_CACHE_EN                0x08
#define V_PCH_SRC_PREF_DIS_CACHE_DIS              0x04
#define V_PCH_SRC_PREF_DIS_CACHE_EN               0x00
#define B_PCH_LPC_BIOS_CNTL_BLE                   0x02
#define B_PCH_LPC_BIOS_CNTL_BIOSWE                0x01
#define N_PCH_LPC_BIOS_CNTL_BLE                   1
#define N_PCH_LPC_BIOS_CNTL_BIOSWE                0
#define R_PCH_LPC_FDCAP                           0xE0
#define B_PCH_LPC_FDCAP_NEXT                      0xFF00
#define B_PCH_LPC_FDCAP_CID                       0x00FF
#define R_PCH_LPC_FDLEN                           0xE2
#define B_PCH_LPC_FDLEN                           0xFF
#define R_PCH_LPC_FDVER                           0xE3
#define B_PCH_LPC_FDVER_VSCID                     0xF0
#define B_PCH_LPC_FDVER_CV                        0x0F
#define R_PCH_LPC_FVECIDX                         0xE4
#define B_PCH_LPC_FVECIDX_IDX                     0x0000003C
#define R_PCH_LPC_FVECD                           0xE8
#define R_PCH_LPC_FVEC0                           0x00
#define B_PCH_LPC_FVEC0_USB_PORT_CAP              0x00000C00
#define V_PCH_LPC_FVEC0_USB_14_PORT               0x00000000
#define V_PCH_LPC_FVEC0_USB_12_PORT               0x00000400
#define V_PCH_LPC_FVEC0_USB_10_PORT               0x00000800
#define B_PCH_LPC_FVEC0_SATA_RAID_CAP             0x00000080
#define B_PCH_LPC_FVEC0_SATA_PORT23_CAP           0x00000040
#define B_PCH_LPC_FVEC0_SATA_PORT1_6GB_CAP        0x00000008
#define B_PCH_LPC_FVEC0_SATA_PORT0_6GB_CAP        0x00000004
#define B_PCH_LPC_FVEC0_PCI_CAP                   0x00000002
#define R_PCH_LPC_FVEC1                           0x01
#define B_PCH_LPC_FVEC1_USB_R_CAP                 0x00400000
#define R_PCH_LPC_FVEC2                           0x02
#define B_PCH_LPC_FVEC2_IATT_CAP                  0x00400000      ///< Intel Anti-Theft Technology Capability
#define V_PCH_LPC_FVEC2_PCIE_PORT78_CAP           0x00200000
#define V_PCH_LPC_FVEC2_PCH_IG_SUPPORT_CAP        0x00020000      ///< PCH Integrated Graphics Support Capability
#define R_PCH_LPC_FVEC3                           0x03
#define B_PCH_LPC_FVEC3_DCMI_CAP                  0x00002000      ///< Data Center Manageability Interface (DCMI) Capability
#define B_PCH_LPC_FVEC3_NM_CAP                    0x00001000      ///< Node Manager Capability
#define R_PCH_LPC_RCBA                            0xF0
#define B_PCH_LPC_RCBA_BAR                        0xFFFFC000
#define B_PCH_LPC_RCBA_EN                         0x00000001

#define R_PCH_LPC_GEN_PMCON_1                     0xA0
#define B_PCH_LPC_GEN_PMCON_PER_SMI_SEL           0x0003
#define V_PCH_LPC_GEN_PMCON_PER_SMI_64S           0x0000
#define V_PCH_LPC_GEN_PMCON_PER_SMI_32S           0x0001
#define V_PCH_LPC_GEN_PMCON_PER_SMI_16S           0x0002
#define V_PCH_LPC_GEN_PMCON_PER_SMI_8S            0x0003
#define B_PCH_LPC_GEN_PMCON_CLKRUN_EN             0x0004
#define B_PCH_LPC_GEN_PMCON_PSEUDO_CLKRUN_EN      0x0008
#define B_PCH_LPC_GEN_PMCON_SMI_LOCK              0x0010
#define B_PCH_LPC_GEN_PMCON_PWRBTN_LVL            0x0200
#define B_PCH_LPC_GEN_PMCON_BIOS_PCI_EXP_EN       0x0400
#define B_PCH_LPC_GEN_PMCON_REQ_CLKRUN_BBCLKGATE  0x0800
#define B_PCH_LPC_GEN_PMCON_ALLOW_SPXB_CG_INC0    0x1000
#define B_PCH_LPC_GEN_PMCON_ALLOW_PLL_SD_INC0     0x2000
#define R_PCH_LPC_GEN_PMCON_2                     0xA2
#define B_PCH_LPC_GEN_PMCON_PWROK_FLR             0x01
#define B_PCH_LPC_GEN_PMCON_SYSPWR_FLR            0x02
#define B_PCH_LPC_GEN_PMCON_MIN_SLP_S4            0x04
#define B_PCH_LPC_GEN_PMCON_CTS                   0x08
#define B_PCH_LPC_GEN_PMCON_SRS                   0x10
#define B_PCH_LPC_GEN_PMCON_MEM_SR                0x20
#define B_PCH_LPC_GEN_PMCON_DRAM_INIT             0x80
#define B_PCH_LPC_GEN_PMCON_SX_PP_EN              0x0800
#define B_PCH_LPC_GEN_PMCON_AG3_PP_EN             0x1000
#define B_PCH_LPC_GEN_PMCON_DSX_PP_DIS            0x2000
#define B_PCH_LPC_GEN_PMCON_DC_PP_DIS             0x4000
#define R_PCH_LPC_GEN_PMCON_3                     0xA4
#define B_PCH_LPC_GEN_PMCON_PME_B0_S5_DIS         BIT15
#define B_PCH_LPC_GEN_PMCON_SUS_PWR_FLR           BIT14
#define B_PCH_LPC_GEN_PMCON_WOL_ENABLE_OVERRIDE   BIT13
#define B_PCH_LPC_GEN_PMCON_DISABLE_SX_STRETCH    BIT12
#define B_PCH_LPC_GEN_PMCON_SLP_S3_MAW            0xC00
#define V_PCH_LPC_GEN_PMCON_SLP_S3_MAW_60US       0x000
#define V_PCH_LPC_GEN_PMCON_SLP_S3_MAW_1MS        0x400
#define V_PCH_LPC_GEN_PMCON_SLP_S3_MAW_50MS       0x800
#define V_PCH_LPC_GEN_PMCON_SLP_S3_MAW_2S         0xC00
#define B_PCH_LPC_GEN_PMCON_GEN_RST_STS           BIT9
#define B_PCH_LPC_GEN_PMCON_SWSMI_RTSL            0xC0
#define V_PCH_LPC_GEN_PMCON_SWSMI_RTSL_64MS       0xC0
#define V_PCH_LPC_GEN_PMCON_SWSMI_RTSL_32MS       0x80
#define V_PCH_LPC_GEN_PMCON_SWSMI_RTSL_16MS       0x40
#define V_PCH_LPC_GEN_PMCON_SWSMI_RTSL_1_5MS      0x00
#define B_PCH_LPC_GEN_PMCON_SLP_S4_MAW            0x30
#define V_PCH_LPC_GEN_PMCON_SLP_S4_MAW_1S         0x30
#define V_PCH_LPC_GEN_PMCON_SLP_S4_MAW_2S         0x20
#define V_PCH_LPC_GEN_PMCON_SLP_S4_MAW_3S         0x10
#define V_PCH_LPC_GEN_PMCON_SLP_S4_MAW_4S         0x00
#define B_PCH_LPC_GEN_PMCON_SLP_S4_ASE            0x08
#define B_PCH_LPC_GEN_PMCON_RTC_PWR_STS           0x04
#define B_PCH_LPC_GEN_PMCON_PWR_FLR               0x02
#define B_PCH_LPC_GEN_PMCON_AFTERG3_EN            0x01
#define R_PCH_LPC_GEN_PMCON_LOCK                  0xA6
#define B_PCH_LPC_GEN_PMCON_LOCK_S4_STRET_LD      BIT2            ///< Lock down SLP_S3/SLP_S4 Minimum Assertion width
#define B_PCH_LPC_GEN_PMCON_LOCK_ABASE_LK         BIT1            ///< Lock ACPI BASE at 0x40, only cleared by reset when set
#define R_PCH_LPC_CIR4                            0xA9            ///< Chipset Initialization Register 4
#define R_PCH_LPC_BM_BREAK_EN2                    0xAA
#define B_PCH_LPC_BM_BREAK_EN2_SATA3              BIT0
#define R_PCH_LPC_BM_BREAK_EN                     0xAB
#define B_PCH_LPC_BM_BREAK_EN_STORAGE             BIT7
#define B_PCH_LPC_BM_BREAK_EN_PCIE                BIT6
#define B_PCH_LPC_BM_BREAK_EN_EHCI                BIT2
#define B_PCH_LPC_BM_BREAK_EN_HDA                 BIT0
#define R_PCH_LPC_PMIR                            0xAC
#define B_PCH_LPC_PMIR_CF9LOCK                    BIT31           ///< CF9h Lockdown
#define B_PCH_LPC_PMIR_CF9GR                      BIT20           ///< CF9h Global Reset
#define B_PCH_LPC_PMIR_SKIP_HOST_RST_HS           BIT19

#define R_PCH_LPC_GPI_ROUT                        0xB8
#define B_PCH_LPC_GPI_ROUT_0                      (BIT1 | BIT0)
#define B_PCH_LPC_GPI_ROUT_1                      (BIT3 | BIT2)
#define B_PCH_LPC_GPI_ROUT_2                      (BIT5 | BIT4)
#define B_PCH_LPC_GPI_ROUT_3                      (BIT7 | BIT6)
#define B_PCH_LPC_GPI_ROUT_4                      (BIT9 | BIT8)
#define B_PCH_LPC_GPI_ROUT_5                      (BIT11 | BIT10)
#define B_PCH_LPC_GPI_ROUT_6                      (BIT13 | BIT12)
#define B_PCH_LPC_GPI_ROUT_7                      (BIT15 | BIT14)
#define B_PCH_LPC_GPI_ROUT_8                      (BIT17 | BIT16)
#define B_PCH_LPC_GPI_ROUT_9                      (BIT19 | BIT18)
#define B_PCH_LPC_GPI_ROUT_10                     (BIT21 | BIT20)
#define B_PCH_LPC_GPI_ROUT_11                     (BIT23 | BIT22)
#define B_PCH_LPC_GPI_ROUT_12                     (BIT25 | BIT24)
#define B_PCH_LPC_GPI_ROUT_13                     (BIT27 | BIT26)
#define B_PCH_LPC_GPI_ROUT_14                     (BIT29 | BIT28)
#define B_PCH_LPC_GPI_ROUT_15                     (BIT31 | BIT30)

#define R_PCH_LPC_GPI_ROUT2                       0xBC
#define B_PCH_LPC_GPI_ROUT2_17                    (BIT1 | BIT0)
#define B_PCH_LPC_GPI_ROUT2_19                    (BIT3 | BIT2)
#define B_PCH_LPC_GPI_ROUT2_21                    (BIT5 | BIT4)
#define B_PCH_LPC_GPI_ROUT2_22                    (BIT7 | BIT6)
#define B_PCH_LPC_GPI_ROUT2_43                    (BIT9 | BIT8)
#define B_PCH_LPC_GPI_ROUT2_56                    (BIT11 | BIT10)
#define B_PCH_LPC_GPI_ROUT2_57                    (BIT13 | BIT12)
#define B_PCH_LPC_GPI_ROUT2_60                    (BIT15 | BIT14)

#define R_PCH_LP_LPC_GPI_ROUT0                    0x30
#define R_PCH_LP_LPC_GPI_ROUT1                    0x34
#define R_PCH_LP_LPC_GPI_ROUT2                    0x38

#define R_PCH_LPC_MDAP                            0xC0
#define B_PCH_LPC_MDAP_POLICY_EN                  BIT31
#define B_PCH_LPC_MDAP_PDMA_EN                    BIT30
#define B_PCH_LPC_MDAP_VALUE                      0x0001FFFF
//
// APM Registers
//
#define R_PCH_APM_CNT                             0xB2
#define R_PCH_APM_STS                             0xB3

//
// ACPI and legacy I/O register offsets from PMBASE
//
#define R_PCH_ACPI_PM1_STS                        0x00
#define S_PCH_ACPI_PM1_STS                        2
#define B_PCH_ACPI_PM1_STS_WAK                    0x8000
#define B_PCH_ACPI_PM1_STS_PRBTNOR                0x0800
#define B_PCH_ACPI_PM1_STS_RTC                    0x0400
#define B_PCH_ACPI_PM1_STS_PWRBTN                 0x0100
#define B_PCH_ACPI_PM1_STS_GBL                    0x0020
#define B_PCH_ACPI_PM1_STS_BM                     0x0010
#define B_PCH_ACPI_PM1_STS_TMROF                  0x0001
#define N_PCH_ACPI_PM1_STS_WAK                    15
#define N_PCH_ACPI_PM1_STS_PRBTNOR                11
#define N_PCH_ACPI_PM1_STS_RTC                    10
#define N_PCH_ACPI_PM1_STS_PWRBTN                 8
#define N_PCH_ACPI_PM1_STS_GBL                    5
#define N_PCH_ACPI_PM1_STS_BM                     4
#define N_PCH_ACPI_PM1_STS_TMROF                  0

#define R_PCH_ACPI_PM1_EN                         0x02
#define S_PCH_ACPI_PM1_EN                         2
#define B_PCH_ACPI_PM1_EN_RTC                     0x0400
#define B_PCH_ACPI_PM1_EN_PWRBTN                  0x0100
#define B_PCH_ACPI_PM1_EN_GBL                     0x0020
#define B_PCH_ACPI_PM1_EN_TMROF                   0X0001
#define N_PCH_ACPI_PM1_EN_RTC                     10
#define N_PCH_ACPI_PM1_EN_PWRBTN                  8
#define N_PCH_ACPI_PM1_EN_GBL                     5
#define N_PCH_ACPI_PM1_EN_TMROF                   0

#define R_PCH_ACPI_PM1_CNT                        0x04
#define S_PCH_ACPI_PM1_CNT                        4
#define B_PCH_ACPI_PM1_CNT_SLP_EN                 0x00002000
#define B_PCH_ACPI_PM1_CNT_SLP_TYP                0x00001C00
#define V_PCH_ACPI_PM1_CNT_S0                     0x00000000
#define V_PCH_ACPI_PM1_CNT_S1                     0x00000400
#define V_PCH_ACPI_PM1_CNT_S3                     0x00001400
#define V_PCH_ACPI_PM1_CNT_S4                     0x00001800
#define V_PCH_ACPI_PM1_CNT_S5                     0x00001C00
#define B_PCH_ACPI_PM1_CNT_GBL_RLS                0x00000004
#define B_PCH_ACPI_PM1_CNT_BM_RLD                 0x00000002
#define B_PCH_ACPI_PM1_CNT_SCI_EN                 0x00000001

#define R_PCH_ACPI_PM1_TMR                        0x08
#define V_PCH_ACPI_TMR_FREQUENCY                  3579545
#define B_PCH_ACPI_PM1_TMR_VAL                    0xFFFFFF
#define V_PCH_ACPI_PM1_TMR_MAX_VAL                0x1000000       ///< The timer is 24 bit overflow

#define R_PCH_ACPI_GPE0_STS_127_96                0x8C
#define S_PCH_ACPI_GPE0_STS_127_96                4
#define B_PCH_ACPI_GPE0_STS_127_96_WADT           BIT18
#define B_PCH_ACPI_GPE0_STS_127_96_GP27           BIT16
#define B_PCH_ACPI_GPE0_STS_127_96_PME_B0         BIT13
#define B_PCH_ACPI_GPE0_STS_127_96_ME_SCI         BIT12
#define B_PCH_ACPI_GPE0_STS_127_96_PME            BIT11
#define B_PCH_ACPI_GPE0_STS_127_96_BATLOW         BIT10
#define B_PCH_ACPI_GPE0_STS_127_96_PCI_EXP        BIT9
#define B_PCH_ACPI_GPE0_STS_127_96_RI             BIT8
#define B_PCH_ACPI_GPE0_STS_127_96_SMB_WAK        BIT7
#define B_PCH_ACPI_GPE0_STS_127_96_TC0SCI         BIT6
#define B_PCH_ACPI_GPE0_STS_127_96_SWGPE          BIT2
#define B_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG       BIT1
#define N_PCH_ACPI_GPE0_STS_127_96_PME_B0         13
#define N_PCH_ACPI_GPE0_STS_127_96_PME            11
#define N_PCH_ACPI_GPE0_STS_127_96_BATLOW         10
#define N_PCH_ACPI_GPE0_STS_127_96_PCI_EXP        9
#define N_PCH_ACPI_GPE0_STS_127_96_RI             8
#define N_PCH_ACPI_GPE0_STS_127_96_SMB_WAK        7
#define N_PCH_ACPI_GPE0_STS_127_96_TC0SCI         6
#define N_PCH_ACPI_GPE0_STS_127_96_SWGPE          2
#define N_PCH_ACPI_GPE0_STS_127_96_HOT_PLUG       1

#define R_PCH_ACPI_GPE0_EN_127_96                 0x9C
#define S_PCH_ACPI_GPE0_EN_127_96                 4
#define B_PCH_ACPI_GPE0_EN_127_96_WADT            BIT18
#define B_PCH_ACPI_GPE0_EN_127_96_GP27            BIT16
#define B_PCH_ACPI_GPE0_EN_127_96_PME_B0          BIT13
#define B_PCH_ACPI_GPE0_EN_127_96_ME_SCI          BIT12
#define B_PCH_ACPI_GPE0_EN_127_96_PME             BIT11
#define B_PCH_ACPI_GPE0_EN_127_96_BATLOW          BIT10
#define B_PCH_ACPI_GPE0_EN_127_96_PCI_EXP         BIT9
#define B_PCH_ACPI_GPE0_EN_127_96_RI              BIT8
#define B_PCH_ACPI_GPE0_EN_127_96_TC0SCI          BIT6
#define B_PCH_ACPI_GPE0_EN_127_96_SWGPE           BIT2
#define B_PCH_ACPI_GPE0_EN_127_96_HOT_PLUG        BIT1
#define N_PCH_ACPI_GPE0_EN_127_96_PME_B0          13
#define N_PCH_ACPI_GPE0_EN_127_96_USB3            12
#define N_PCH_ACPI_GPE0_EN_127_96_PME             11
#define N_PCH_ACPI_GPE0_EN_127_96_BATLOW          10
#define N_PCH_ACPI_GPE0_EN_127_96_PCI_EXP         9
#define N_PCH_ACPI_GPE0_EN_127_96_RI              8
#define N_PCH_ACPI_GPE0_EN_127_96_TC0SCI          6
#define N_PCH_ACPI_GPE0_EN_127_96_SWGPE           2
#define N_PCH_ACPI_GPE0_EN_127_96_HOT_PLUG        1

#define R_PCH_ACPI_GPE0a_STS                      0x20
#define S_PCH_ACPI_GPE0a_STS                      4
#define B_PCH_ACPI_GPE0a_STS_GPInn                0xFFFF0000
#define B_PCH_ACPI_GPE0a_STS_PME_B0               BIT13
#define B_PCH_ACPI_GPE0a_STS_PME                  BIT11
#define B_PCH_ACPI_GPE0a_STS_BATLOW               BIT10
#define B_PCH_ACPI_GPE0a_STS_PCI_EXP              BIT9
#define B_PCH_ACPI_GPE0a_STS_RI                   BIT8
#define B_PCH_ACPI_GPE0a_STS_SMB_WAK              BIT7
#define B_PCH_ACPI_GPE0a_STS_TC0SCI               BIT6
#define B_PCH_ACPI_GPE0a_STS_SWGPE                BIT2
#define B_PCH_ACPI_GPE0a_STS_HOT_PLUG             BIT1
#define N_PCH_ACPI_GPE0a_STS_PME_B0               13
#define N_PCH_ACPI_GPE0a_STS_PME                  11
#define N_PCH_ACPI_GPE0a_STS_BATLOW               10
#define N_PCH_ACPI_GPE0a_STS_PCI_EXP              9
#define N_PCH_ACPI_GPE0a_STS_RI                   8
#define N_PCH_ACPI_GPE0a_STS_SMB_WAK              7
#define N_PCH_ACPI_GPE0a_STS_TC0SCI               6
#define N_PCH_ACPI_GPE0a_STS_SWGPE                2
#define N_PCH_ACPI_GPE0a_STS_HOT_PLUG             1

#define R_PCH_ACPI_GPE0b_STS                      0x24
#define S_PCH_ACPI_GPE0b_STS                      4
#define B_PCH_ACPI_GPE0b_STS_GP60                 BIT31
#define B_PCH_ACPI_GPE0b_STS_GP57                 BIT30
#define B_PCH_ACPI_GPE0b_STS_GP56                 BIT29
#define B_PCH_ACPI_GPE0b_STS_GP43                 BIT28
#define B_PCH_ACPI_GPE0b_STS_GP22                 BIT27
#define B_PCH_ACPI_GPE0b_STS_GP21                 BIT26
#define B_PCH_ACPI_GPE0b_STS_GP19                 BIT25
#define B_PCH_ACPI_GPE0b_STS_GP17                 BIT24
#define B_PCH_ACPI_GPE0b_STS_WADT                 BIT6
#define B_PCH_ACPI_GPE0b_STS_ME_SCI               BIT4
#define B_PCH_ACPI_GPE0b_STS_GP27                 BIT3

#define R_PCH_ACPI_GPE0a_EN                       0x28
#define S_PCH_ACPI_GPE0a_EN                       4
#define B_PCH_ACPI_GPE0a_EN_GPInn                 0xFFFF0000
#define B_PCH_ACPI_GPE0a_EN_PME_B0                BIT13
#define B_PCH_ACPI_GPE0a_EN_PME                   BIT11
#define B_PCH_ACPI_GPE0a_EN_BATLOW                BIT10
#define B_PCH_ACPI_GPE0a_EN_PCI_EXP               BIT9
#define B_PCH_ACPI_GPE0a_EN_RI                    BIT8
#define B_PCH_ACPI_GPE0a_EN_TC0SCI                BIT6
#define B_PCH_ACPI_GPE0a_EN_SWGPE                 BIT2
#define B_PCH_ACPI_GPE0a_EN_HOT_PLUG              BIT1
#define N_PCH_ACPI_GPE0a_EN_USB4                  14
#define N_PCH_ACPI_GPE0a_EN_PME_B0                13
#define N_PCH_ACPI_GPE0a_EN_USB3                  12
#define N_PCH_ACPI_GPE0a_EN_PME                   11
#define N_PCH_ACPI_GPE0a_EN_BATLOW                10
#define N_PCH_ACPI_GPE0a_EN_PCI_EXP               9
#define N_PCH_ACPI_GPE0a_EN_RI                    8
#define N_PCH_ACPI_GPE0a_EN_TC0SCI                6
#define N_PCH_ACPI_GPE0a_EN_SWGPE                 2
#define N_PCH_ACPI_GPE0a_EN_HOT_PLUG              1

#define R_PCH_ACPI_GPE0b_EN                       0x2C
#define S_PCH_ACPI_GPE0b_EN                       4
#define B_PCH_ACPI_GPE0b_EN_GP60                  BIT31
#define B_PCH_ACPI_GPE0b_EN_GP57                  BIT30
#define B_PCH_ACPI_GPE0b_EN_GP56                  BIT29
#define B_PCH_ACPI_GPE0b_EN_GP43                  BIT28
#define B_PCH_ACPI_GPE0b_EN_GP22                  BIT27
#define B_PCH_ACPI_GPE0b_EN_GP21                  BIT26
#define B_PCH_ACPI_GPE0b_EN_GP19                  BIT25
#define B_PCH_ACPI_GPE0b_EN_GP17                  BIT24
#define B_PCH_ACPI_GPE0b_EN_WADT                  BIT6
#define B_PCH_ACPI_GPE0b_EN_ME_SCI                BIT4
#define B_PCH_ACPI_GPE0b_EN_GP27                  BIT3

#define R_PCH_SMI_EN                              0x30
#define S_PCH_SMI_EN                              4
#define B_PCH_SMI_EN_LEGACY_USB3                  BIT31
#define B_PCH_SMI_EN_GPIO_UNLOCK_SMI              BIT27
#define B_PCH_SMI_EN_INTEL_USB2                   BIT18
#define B_PCH_SMI_EN_LEGACY_USB2                  BIT17
#define B_PCH_SMI_EN_PERIODIC                     BIT14
#define B_PCH_SMI_EN_TCO                          BIT13
#define B_PCH_SMI_EN_MCSMI                        BIT11
#define B_PCH_SMI_EN_BIOS_RLS                     BIT7
#define B_PCH_SMI_EN_SWSMI_TMR                    BIT6
#define B_PCH_SMI_EN_APMC                         BIT5
#define B_PCH_SMI_EN_ON_SLP_EN                    BIT4
#define B_PCH_SMI_EN_LEGACY_USB                   BIT3
#define B_PCH_SMI_EN_BIOS                         BIT2
#define B_PCH_SMI_EN_EOS                          BIT1
#define B_PCH_SMI_EN_GBL_SMI                      BIT0
#define N_PCH_SMI_EN_LEGACY_USB3                  31
#define N_PCH_SMI_EN_GPIO_UNLOCK                  27
#define N_PCH_SMI_EN_INTEL_USB2                   18
#define N_PCH_SMI_EN_LEGACY_USB2                  17
#define N_PCH_SMI_EN_PERIODIC                     14
#define N_PCH_SMI_EN_TCO                          13
#define N_PCH_SMI_EN_MCSMI                        11
#define N_PCH_SMI_EN_BIOS_RLS                     7
#define N_PCH_SMI_EN_SWSMI_TMR                    6
#define N_PCH_SMI_EN_APMC                         5
#define N_PCH_SMI_EN_ON_SLP_EN                    4
#define N_PCH_SMI_EN_LEGACY_USB                   3
#define N_PCH_SMI_EN_BIOS                         2
#define N_PCH_SMI_EN_EOS                          1
#define N_PCH_SMI_EN_GBL_SMI                      0

#define R_PCH_SMI_STS                             0x34
#define S_PCH_SMI_STS                             4
#define B_PCH_SMI_STS_LEGACY_USB3                 BIT31
#define B_PCH_SMI_STS_GPIO_UNLOCK                 BIT27
#define B_PCH_SMI_STS_SPI                         BIT26
#define B_PCH_SMI_STS_MONITOR                     BIT21
#define B_PCH_SMI_STS_PCI_EXP                     BIT20
#define B_PCH_SMI_STS_PATCH                       BIT19
#define B_PCH_SMI_STS_INTEL_USB2                  BIT18
#define B_PCH_SMI_STS_LEGACY_USB2                 BIT17
#define B_PCH_SMI_STS_SMBUS                       BIT16
#define B_PCH_SMI_STS_SERIRQ                      BIT15
#define B_PCH_SMI_STS_PERIODIC                    BIT14
#define B_PCH_SMI_STS_TCO                         BIT13
#define B_PCH_SMI_STS_DEVMON                      BIT12
#define B_PCH_SMI_STS_MCSMI                       BIT11
#define B_PCH_SMI_STS_GPIO_SMI                    BIT10
#define B_PCH_SMI_STS_GPE1                        BIT10
#define B_PCH_SMI_STS_GPE0                        BIT9
#define B_PCH_SMI_STS_PM1_STS_REG                 BIT8
#define B_PCH_SMI_STS_SWSMI_TMR                   BIT6
#define B_PCH_SMI_STS_APM                         BIT5
#define B_PCH_SMI_STS_ON_SLP_EN                   BIT4
#define B_PCH_SMI_STS_LEGACY_USB                  BIT3
#define B_PCH_SMI_STS_BIOS                        BIT2
#define N_PCH_SMI_STS_LEGACY_USB3                 31
#define N_PCH_SMI_STS_GPIO_UNLOCK                 27
#define N_PCH_SMI_STS_SPI                         26
#define N_PCH_SMI_STS_MONITOR                     21
#define N_PCH_SMI_STS_PCI_EXP                     20
#define N_PCH_SMI_STS_PATCH                       19
#define N_PCH_SMI_STS_INTEL_USB2                  18
#define N_PCH_SMI_STS_LEGACY_USB2                 17
#define N_PCH_SMI_STS_SMBUS                       16
#define N_PCH_SMI_STS_SERIRQ                      15
#define N_PCH_SMI_STS_PERIODIC                    14
#define N_PCH_SMI_STS_TCO                         13
#define N_PCH_SMI_STS_DEVMON                      12
#define N_PCH_SMI_STS_MCSMI                       11
#define N_PCH_SMI_STS_GPE1                        10
#define N_PCH_SMI_STS_GPE0                        9
#define N_PCH_SMI_STS_PM1_STS_REG                 8
#define N_PCH_SMI_STS_SWSMI_TMR                   6
#define N_PCH_SMI_STS_APM                         5
#define N_PCH_SMI_STS_ON_SLP_EN                   4
#define N_PCH_SMI_STS_LEGACY_USB                  3
#define N_PCH_SMI_STS_BIOS                        2

#define R_PCH_LPTH_ALT_GP_SMI_EN                  0x38
#define S_PCH_LPTH_ALT_GP_SMI_EN                  2
#define R_PCH_LPTH_ALT_GP_SMI_STS                 0x3A
#define S_PCH_LPTH_ALT_GP_SMI_STS                 2
#define V_PCH_LPTH_ALT_GP_SMI_GPIBASE             0
#define S_PCH_LPTH_ALT_GP_SMI_GPISIZE             16

#define R_PCH_LPTLP_ALT_GP_SMI_EN                 0x54
#define S_PCH_LPTLP_ALT_GP_SMI_EN                 4
#define R_PCH_LPTLP_ALT_GP_SMI_STS                0x50
#define S_PCH_LPTLP_ALT_GP_SMI_STS                4
#define V_PCH_LPTLP_ALT_GP_SMI_GPIBASE            32
#define S_PCH_LPTLP_ALT_GP_SMI_GPISIZE            16

//
// USB Per-Port Registers Write Control
//
#define R_PCH_UPRWC                               0x3C
#define S_PCH_UPRWC                               2
#define B_PCH_UPRWC_WR_EN_SMI_STS                 0x0100
#define B_PCH_UPRWC_WR_EN                         0x0002
#define B_PCH_UPRWC_WR_EN_SMI_EN                  0x0001

#define R_PCH_ACPI_GPE_CNTL                       0x42
#define B_PCH_ACPI_GPE_CNTL_SWGPE_CTRL            BIT1

#define R_PCH_DEVACT_STS                          0x44
#define S_PCH_DEVACT_STS                          2
#define B_PCH_DEVACT_STS_MASK                     0x13E1
#define B_PCH_DEVACT_STS_KBC                      0x1000
#define B_PCH_DEVACT_STS_PIRQDH                   0x0200
#define B_PCH_DEVACT_STS_PIRQCG                   0x0100
#define B_PCH_DEVACT_STS_PIRQBF                   0x0080
#define B_PCH_DEVACT_STS_PIRQAE                   0x0040
#define N_PCH_DEVACT_STS_KBC                      12
#define N_PCH_DEVACT_STS_PIRQDH                   9
#define N_PCH_DEVACT_STS_PIRQCG                   8
#define N_PCH_DEVACT_STS_PIRQBF                   7
#define N_PCH_DEVACT_STS_PIRQAE                   6

#define R_PCH_ACPI_PM2_CNT                        0x50
#define B_PCH_ACPI_PM2_CNT_ARB_DIS                0x01

#define R_PCH_OC_WDT_CTL                          0x54
#define B_PCH_OC_WDT_CTL_RLD                      BIT31
#define B_PCH_OC_WDT_CTL_ICCSURV_STS              BIT25
#define B_PCH_OC_WDT_CTL_NO_ICCSURV_STS           BIT24
#define B_PCH_OC_WDT_CTL_FORCE_ALL                BIT15
#define B_PCH_OC_WDT_CTL_EN                       BIT14
#define B_PCH_OC_WDT_CTL_ICCSURV                  BIT13
#define B_PCH_OC_WDT_CTL_LCK                      BIT12
#define B_PCH_OC_WDT_CTL_TOV_MASK                 0x3FF
#define B_PCH_OC_WDT_CTL_FAILURE_STS              BIT23
#define B_PCH_OC_WDT_CTL_UNXP_RESET_STS           BIT22
#define B_PCH_OC_WDT_CTL_AFTER_POST               0x3F0000
#define V_PCH_OC_WDT_CTL_STATUS_FAILURE           1
#define V_PCH_OC_WDT_CTL_STATUS_OK                0

#define R_PCH_ALT_GPI_SMI_EN2                     0x5C
#define B_PCH_ALT_GPI_SMI_EN2_ALT_GP60_SMI_EN     BIT7
#define B_PCH_ALT_GPI_SMI_EN2_ALT_GP57_SMI_EN     BIT6
#define B_PCH_ALT_GPI_SMI_EN2_ALT_GP56_SMI_EN     BIT5
#define B_PCH_ALT_GPI_SMI_EN2_ALT_GP43_SMI_EN     BIT4
#define B_PCH_ALT_GPI_SMI_EN2_ALT_GP22_SMI_EN     BIT3
#define B_PCH_ALT_GPI_SMI_EN2_ALT_GP21_SMI_EN     BIT2
#define B_PCH_ALT_GPI_SMI_EN2_ALT_GP19_SMI_EN     BIT1
#define B_PCH_ALT_GPI_SMI_EN2_ALT_GP17_SMI_EN     BIT0
#define R_PCH_ALT_GPI_SMI_STS2                    0x5E
#define B_PCH_ALT_GPI_SMI_STS2_ALT_GP60_SMI_STS   BIT7
#define B_PCH_ALT_GPI_SMI_STS2_ALT_GP57_SMI_STS   BIT6
#define B_PCH_ALT_GPI_SMI_STS2_ALT_GP56_SMI_STS   BIT5
#define B_PCH_ALT_GPI_SMI_STS2_ALT_GP43_SMI_STS   BIT4
#define B_PCH_ALT_GPI_SMI_STS2_ALT_GP22_SMI_STS   BIT3
#define B_PCH_ALT_GPI_SMI_STS2_ALT_GP21_SMI_STS   BIT2
#define B_PCH_ALT_GPI_SMI_STS2_ALT_GP19_SMI_STS   BIT1
#define B_PCH_ALT_GPI_SMI_STS2_ALT_GP17_SMI_STS   BIT0

//
// TCO register I/O map
//
#define PCH_TCO_BASE                              0x60

#define R_PCH_TCO_RLD                             0x0
#define R_PCH_TCO_DAT_IN                          0x2
#define R_PCH_TCO_DAT_OUT                         0x3
#define R_PCH_TCO1_STS                            0x04
#define S_PCH_TCO1_STS                            2
#define B_PCH_TCO1_STS_DMISERR                    0x1000
#define B_PCH_TCO1_STS_DMISMI                     0x0400
#define B_PCH_TCO1_STS_DMISCI                     0x0200
#define B_PCH_TCO1_STS_BIOSWR                     0x0100
#define B_PCH_TCO1_STS_NEWCENTURY                 0x0080
#define B_PCH_TCO1_STS_TIMEOUT                    0x0008
#define B_PCH_TCO1_STS_TCO_INT                    0x0004
#define B_PCH_TCO1_STS_SW_TCO_SMI                 0x0002
#define B_PCH_TCO1_STS_NMI2SMI                    0001
#define N_PCH_TCO1_STS_DMISMI                     10
#define N_PCH_TCO1_STS_BIOSWR                     8
#define N_PCH_TCO1_STS_NEWCENTURY                 7
#define N_PCH_TCO1_STS_TIMEOUT                    3
#define N_PCH_TCO1_STS_SW_TCO_SMI                 1
#define N_PCH_TCO1_STS_NMI2SMI                    0

#define R_PCH_TCO2_STS                            0x06
#define S_PCH_TCO2_STS                            2
#define B_PCH_TCO2_STS_SMLINK_SLV_SMI             BIT4
#define B_PCH_TCO2_STS_BAD_BIOS                   BIT3
#define B_PCH_TCO2_STS_BOOT                       BIT2
#define B_PCH_TCO2_STS_SECOND_TO                  BIT1
#define B_PCH_TCO2_STS_INTRD_DET                  BIT0
#define N_PCH_TCO2_STS_INTRD_DET                  0

#define R_PCH_TCO1_CNT                            0x08
#define S_PCH_TCO1_CNT                            2
#define B_PCH_TCO_CNT_LOCK                        BIT12
#define B_PCH_TCO_CNT_TMR_HLT                     BIT11
#define B_PCH_TCO_CNT_NMI2SMI_EN                  BIT9
#define B_PCH_TCO_CNT_NMI_NOW                     BIT8
#define N_PCH_TCO_CNT_NMI2SMI_EN                  9

#define R_PCH_TCO2_CNT                            0x0A
#define S_PCH_TCO2_CNT                            2
#define B_PCH_TCO2_CNT_OS_POLICY                  0x0030
#define B_PCH_TCO2_CNT_GPI11_ALERT_DISABLE        0x0008
#define B_PCH_TCO2_CNT_INTRD_SEL                  0x0006
#define N_PCH_TCO2_CNT_INTRD_SEL                  2

#define R_PCH_TCO_MESSAGE1                        0x0C
#define R_PCH_TCO_MESSAGE2                        0x0D
#define R_PCH_TCO_WDCNT                           0x0E
#define R_PCH_TCO_SW_IRQ_GEN                      0x10
#define B_PCH_TCO_IRQ12_CAUSE                     BIT1
#define B_PCH_TCO_IRQ1_CAUSE                      BIT0
#define R_PCH_TCO_TMR                             0x12

//
// GPIO Init register offsets from GPIOBASE
//
#define R_PCH_GPIO_USE_SEL                        0x00
#define R_PCH_GPIO_IO_SEL                         0x04
#define R_PCH_GPIO_LVL                            0x0C
#define R_PCH_GPIO_IOAPIC_SEL                     0x10
#define V_PCH_GPIO_IOAPIC_SEL                     0xFFFF
#define R_PCH_GPIO_BLINK                          0x18
#define R_PCH_GPIO_SER_BLINK                      0x1C
#define R_PCH_GPIO_SB_CMDSTS                      0x20
#define B_PCH_GPIO_SB_CMDSTS_DLS_MASK             0x00C00000      ///< Data length select
#define B_PCH_GPIO_SB_CMDSTS_DRS_MASK             0x003F0000      ///< Data rate select
#define B_PCH_GPIO_SB_CMDSTS_BUSY                 BIT8
#define B_PCH_GPIO_SB_CMDSTS_GO                   BIT0
#define R_PCH_GPIO_SB_DATA                        0x24
#define R_PCH_GPIO_NMI_EN                         0x28
#define B_PCH_GPIO_NMI_EN                         0xFFFF
#define R_PCH_GPIO_NMI_STS                        0x2A
#define B_PCH_GPIO_NMI_STS                        0xFFFF
#define R_PCH_GPIO_GPI_INV                        0x2C
#define R_PCH_GPIO_USE_SEL2                       0x30
#define R_PCH_GPIO_IO_SEL2                        0x34
#define R_PCH_GPIO_LVL2                           0x38
#define R_PCH_GPIO_USE_SEL3                       0x40
#define R_PCH_GPIO_IO_SEL3                        0x44
#define R_PCH_GPIO_LVL3                           0x48

#define R_PCH_GP_RST_SEL                          0x60
#define S_PCH_GP_RST_SEL                          4
#define R_PCH_GP_RST_SEL2                         0x64
#define S_PCH_GP_RST_SEL2                         4
#define R_PCH_GP_RST_SEL3                         0x68
#define S_PCH_GP_RST_SEL3                         4

typedef struct {
  UINT16 GpioOwn          : 1;
  UINT16 GpiRout          : 1;
  UINT16 GpiIe            : 1;
  UINT16 GpioUseSel       : 1;
  UINT16 GpioIoSel        : 1;
  UINT16 GpiInv           : 1;
  UINT16 GpiLxEb          : 1;
  UINT16 GpoLvl           : 1;
  UINT16 GpiWp            : 2;
  UINT16 GpinDis          : 1;
  UINT16 Reserved         : 5;
} PCH_GPIO_DEFINITION;

#define R_PCH_GPIO_OWN0                           0x00
#define B_PCH_GPIO_OWN0_GPIO_USE_SEL              BIT0
#define B_PCH_GPIO_OWN0_GPIO_IO_SEL               BIT2
#define B_PCH_GPIO_OWN0_GPI_INV                   BIT3
#define B_PCH_GPIO_OWN0_GPI_LxEB                  BIT4
#define B_PCH_GPIO_OWN0_GPI_LVL                   BIT30
#define B_PCH_GPIO_OWN0_GPO_LVL                   BIT31

#define V_PCH_GPIO_OWN_GPIO                       0x01
#define V_PCH_GPIO_OWN_ACPI                       0x00

#define V_PCH_GPIO_USE_SEL_NATIVE                 0x00
#define V_PCH_GPIO_USE_SEL_GPIO                   0x01

#define V_PCH_GPIO_IO_SEL_OUT                     0x00
#define V_PCH_GPIO_IO_SEL_IN                      0x01

#define V_PCH_GPO_LVL_LOW                         0x00
#define V_PCH_GPO_LVL_HIGH                        0x01

#define V_PCH_GPI_LVL_NORMAL                      0x00
#define V_PCH_GPI_LVL_INVERTED                    0x01

#define V_PCH_GPI_LxEB_EDGE                       0x00
#define V_PCH_GPI_LxEB_LEVEL                      0x01

#define V_PCH_GPINDIS_ENABLE                      0x00
#define V_PCH_GPINDIS_DISABLE                     0x01

#define V_PCH_GPIWP_NONE                          0x00
#define V_PCH_GPIWP_DOWN                          0x01
#define V_PCH_GPIWP_UP                            0x02

#define R_PCH_GPIO_ROUT0                          0x30
#define V_PCH_GPIO_ROUT0_NMI_SMI                  0x01
#define V_PCH_GPIO_ROUT0_SCI                      0x00

#define R_PCH_GPIO_GC                             0x7C
#define R_PCH_GPI_IS0                             0x80
#define R_PCH_GPI_IS1                             0x84
#define R_PCH_GPI_IS2                             0x88
#define V_PCH_GPI_IS_CLEARALL                     0xFFFFFFFF

#define R_PCH_GPI_IE0                             0x90
#define V_PCH_GPI_IE_APIC_DISABLED                0x00
#define V_PCH_GPI_IE_APIC_ENABLED                 0x01

#define R_PCH_GPI_IE1                             0x94
#define R_PCH_GPI_IE2                             0x98
#define V_PCH_GPI_IE_CLEARALL                     0x00000000

#define R_PCH_GP_N_CONFIG0                        0x100
#define R_PCH_GP_X_CONFIG0(n)                     (R_PCH_GP_N_CONFIG0 + ((n) * 0x08))
#define R_PCH_GP_18_CONFIG0                       R_PCH_GP_X_CONFIG0(18)
#define R_PCH_GP_19_CONFIG0                       R_PCH_GP_X_CONFIG0(19)
#define R_PCH_GP_20_CONFIG0                       R_PCH_GP_X_CONFIG0(20)
#define R_PCH_GP_21_CONFIG0                       R_PCH_GP_X_CONFIG0(21)
#define R_PCH_GP_22_CONFIG0                       R_PCH_GP_X_CONFIG0(22)
#define R_PCH_GP_23_CONFIG0                       R_PCH_GP_X_CONFIG0(23)
#define R_PCH_GP_29_CONFIG0                       R_PCH_GP_X_CONFIG0(29)
#define R_PCH_GP_30_CONFIG0                       R_PCH_GP_X_CONFIG0(30)
#define R_PCH_GP_60_CONFIG0                       R_PCH_GP_X_CONFIG0(60)
#define R_PCH_GP_73_CONFIG0                       R_PCH_GP_X_CONFIG0(73)
#define R_PCH_GP_83_CONFIG0                       R_PCH_GP_X_CONFIG0(83) ///< SPI0
#define R_PCH_GP_87_CONFIG0                       R_PCH_GP_X_CONFIG0(87) ///< SPI1
#define R_PCH_GP_91_CONFIG0                       R_PCH_GP_X_CONFIG0(91) ///< UART0
#define V_PCH_GPIO_PIN_MAX                        95

//
// Processor interface registers
//
#define R_PCH_NMI_SC                              0x61
#define B_PCH_NMI_SC_SERR_NMI_STS                 BIT7
#define B_PCH_NMI_SC_IOCHK_NMI_STS                BIT6
#define B_PCH_NMI_SC_TMR2_OUT_STS                 BIT5
#define B_PCH_NMI_SC_REF_TOGGLE                   BIT4
#define B_PCH_NMI_SC_IOCHK_NMI_EN                 BIT3
#define B_PCH_NMI_SC_PCI_SERR_EN                  BIT2
#define B_PCH_NMI_SC_SPKR_DAT_EN                  BIT1
#define B_PCH_NMI_SC_TIM_CNT2_EN                  BIT0
#define R_PCH_NMI_EN                              0x70
#define B_PCH_NMI_EN_NMI_EN                       BIT7

//
// RTC register
//
#define R_PCH_RTC_INDEX                           0x70
#define R_PCH_RTC_TARGET                          0x71
#define R_PCH_RTC_EXT_INDEX                       0x72
#define R_PCH_RTC_EXT_TARGET                      0x73
#define R_PCH_RTC_REGA                            0x0A
#define B_PCH_RTC_REGA_UIP                        0x80
#define R_PCH_RTC_REGB                            0x0B
#define B_PCH_RTC_REGB_SET                        0x80
#define B_PCH_RTC_REGB_PIE                        0x40
#define B_PCH_RTC_REGB_AIE                        0x20
#define B_PCH_RTC_REGB_UIE                        0x10
#define B_PCH_RTC_REGB_DM                         0x04
#define B_PCH_RTC_REGB_HOURFORM                   0x02
#define R_PCH_RTC_REGC                            0x0C
#define R_PCH_RTC_REGD                            0x0D

//
// Reset Generator I/O Port
//
#define R_PCH_RST_CNT                             0xCF9
#define B_PCH_RST_CNT_FULL_RST                    BIT3
#define B_PCH_RST_CNT_RST_CPU                     BIT2
#define B_PCH_RST_CNT_SYS_RST                     BIT1
#define V_PCH_RST_CNT_FULLRESET                   0x0E
#define V_PCH_RST_CNT_HARDRESET                   0x06
#define V_PCH_RST_CNT_SOFTRESET                   0x04
#define V_PCH_RST_CNT_HARDSTARTSTATE              0x02
#define V_PCH_RST_CNT_SOFTSTARTSTATE              0x00
#endif
