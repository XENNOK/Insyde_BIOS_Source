/** @file
  Register names for PCH SATA controllers

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
#ifndef _PCH_REGS_SATA_H_
#define _PCH_REGS_SATA_H_

//
//  SATA Controller 1 Registers (D31:F2)
//
#define PCI_DEVICE_NUMBER_PCH_SATA          31
#define PCI_FUNCTION_NUMBER_PCH_SATA        2
#define R_PCH_SATA_VENDOR_ID                0x00
#define V_PCH_SATA_VENDOR_ID                V_PCH_INTEL_VENDOR_ID
#define R_PCH_SATA_DEVICE_ID                0x02
#define V_PCH_LPTH_SATA_DEVICE_ID_D_IDE          0x8C00  ///< Desktop IDE Mode (Ports 0,1, 2 and 3)
#define V_PCH_LPTH_SATA_DEVICE_ID_D_AHCI         0x8C02  ///< Desktop AHCI Mode (Ports 0-5)
#define V_PCH_LPTH_SATA_DEVICE_ID_D_RAID         0x8C04  ///< Desktop RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define V_PCH_LPTH_SATA_DEVICE_ID_D_RAID_PREM    0x8C06  ///< Desktop RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define V_PCH_LPTH_SATA_DEVICE_ID_D_RAID_ALTDIS  0x2822  ///< Desktop RAID 0/1/5/10 Mode, based on D31:F2:9Ch[9][7]
#define V_PCH_LPTH_SATA_DEVICE_ID_D_RAID_SERVER  0x2826  ///< Server  RAID 0/1/5/10 Mode, based on D31:F2:9Ch[9][7]
#define V_PCH_LPTH_SATA2_DEVICE_ID_D_IDE         0x8C08  ///< Controller 2, Desktop IDE Mode, 2 ports
#define V_PCH_LPTH_SATA_DEVICE_ID_M_IDE          0x8C01  ///< Mobile IDE Mode, port 0, 1, 4, 5
#define V_PCH_LPTH_SATA_DEVICE_ID_M_AHCI         0x8C03  ///< Mobile AHCI Mode, port 0, 1, 4 5
#define V_PCH_LPTH_SATA_DEVICE_ID_M_RAID         0x8C05  ///< Mobile RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define V_PCH_LPTH_SATA_DEVICE_ID_M_RAID_PREM    0x8C07  ///< Mobile RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define V_PCH_LPTH_SATA_DEVICE_ID_M_RAID_ALTDIS  0x282A  ///< Mobile RAID 0/1/5/10 Mode, based on D31:F2:9Ch[7]
#define V_PCH_LPTH_SATA2_DEVICE_ID_M_IDE         0x8C09  ///< Controller 2, Mobile IDE, 2 ports
#define V_PCH_LPTH_SATA_DEVICE_ID_D_RAID1        0x8C0E  ///< SATA Controller 1 (RAID 1/RRT Only)
#define V_PCH_LPTH_SATA_DEVICE_ID_M_RAID1        0x8C0F  ///< SATA Controller 1 (RAID 1/RRT Only) - Mobile

//
//  SATA Controller 2 Registers (D31:F5)
//
#define PCI_DEVICE_NUMBER_PCH_SATA2         31
#define PCI_FUNCTION_NUMBER_PCH_SATA2       5
#define R_PCH_SATA2_VENDOR_ID               0x00
#define V_PCH_SATA2_VENDOR_ID               V_PCH_INTEL_VENDOR_ID
#define R_PCH_SATA2_DEVICE_ID               0x02

#define V_PCH_LPTLP_SATA_DEVICE_ID_M_TEST0         0x9C00  ///< SATA Controller 1 (TEST Mode only)
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_TEST1         0x9C01  ///< SATA Controller 1 (TEST Mode only) - Mobile
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_AHCI0         0x9C02  ///< SATA Controller 1 (AHCI)
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_AHCI1         0x9C03  ///< SATA Controller 1 (AHCI) - Mobile
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_RAID0         0x9C04  ///< SATA Controller 1 (RAID 0/1/5/10) - NOT premium
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_RAID_ALTDIS0  0x2822  ///< SATA Controller 1 (RAID 0/1/5/10) - NOT premium - Alternate ID
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_RAID1         0x9C05  ///< SATA Controller 1 (RAID 0/1/5/10) - NOT premium - Mobile
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_RAID_ALTDIS1  0x282A  ///< SATA Controller 1 (RAID 0/1/5/10) - NOT premium - Mobile - Alternate ID
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_RAID_PREM0    0x9C06  ///< SATA Controller 1 (RAID 0/1/5/10) - premium
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_RAID_PREM1    0x9C07  ///< SATA Controller 1 (RAID 0/1/5/10) - premium - Mobile
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_RAID2         0x9C0E  ///< SATA Controller 1 (RAID 1/RRT Only)
#define V_PCH_LPTLP_SATA_DEVICE_ID_M_RAID3         0x9C0F  ///< SATA Controller 1 (RAID 1/RRT Only) - Mobile

//
//  SATA Controller common Registers
//
#define R_PCH_SATA_COMMAND                  0x04
#define B_PCH_SATA_COMMAND_INT_DIS          BIT10
#define B_PCH_SATA_COMMAND_FBE              BIT9
#define B_PCH_SATA_COMMAND_SERR_EN          BIT8
#define B_PCH_SATA_COMMAND_WCC              BIT7
#define B_PCH_SATA_COMMAND_PER              BIT6
#define B_PCH_SATA_COMMAND_VPS              BIT5
#define B_PCH_SATA_COMMAND_PMWE             BIT4
#define B_PCH_SATA_COMMAND_SCE              BIT3
#define B_PCH_SATA_COMMAND_BME              BIT2
#define B_PCH_SATA_COMMAND_MSE              BIT1
#define B_PCH_SATA_COMMAND_IOSE             BIT0
#define R_PCH_SATA_PCISTS                   0x06
#define B_PCH_SATA_PCISTS_DPE               BIT15
#define B_PCH_SATA_PCISTS_RMA               BIT13
#define B_PCH_SATA_PCISTS_DEV_STS_MASK      (BIT10 | BIT9)
#define B_PCH_SATA_PCISTS_DPED              BIT8
#define B_PCH_SATA_PCISTS_CAP_LIST          BIT4
#define B_PCH_SATA_PCISTS_ITNS              BIT3
#define R_PCH_SATA_RID                      0x08
#define R_PCH_SATA_PI_REGISTER              0x09
#define B_PCH_SATA_PI_REGISTER_SNC          BIT3
#define B_PCH_SATA_PI_REGISTER_SNE          BIT2
#define B_PCH_SATA_PI_REGISTER_PNC          BIT1
#define B_PCH_SATA_PI_REGISTER_PNE          BIT0
#define R_PCH_SATA_SUB_CLASS_CODE           0x0A
#define V_PCH_SATA_SUB_CLASS_CODE_IDE       0x01
#define V_PCH_SATA_SUB_CLASS_CODE_AHCI      0x06
#define V_PCH_SATA_SUB_CLASS_CODE_RAID      0x04
#define R_PCH_SATA_BCC                      0x0B
#define B_PCH_SATA_BCC                      0xFF
#define R_PCH_SATA_PMLT                     0x0D
#define B_PCH_SATA_PMLT                     0xFF
#define R_PCH_SATA_HTYPE                    0x0E
#define B_PCH_SATA_HTYPE_MFD                BIT7
#define B_PCH_SATA_HTYPE_HL                 0x7F
#define R_PCH_SATA_PCMD_BAR                 0x10
#define B_PCH_SATA_PCMD_BAR_BA              0x0000FFF8
#define B_PCH_SATA_PCMD_BAR_RTE             BIT0
#define R_PCH_SATA_PCNL_BAR                 0x14
#define B_PCH_SATA_PCNL_BAR_BA              0x0000FFFC
#define B_PCH_SATA_PCNL_BAR_RTE             BIT0
#define R_PCH_SATA_SCMD_BAR                 0x18
#define B_PCH_SATA_SCMD_BAR_BA              0x0000FFF8
#define B_PCH_SATA_SCMD_BAR_RTE             BIT0
#define R_PCH_SATA_SCNL_BAR                 0x1C
#define B_PCH_SATA_SCNL_BAR_BA              0x0000FFFC
#define B_PCH_SATA_SCNL_BAR_RTE             BIT0
#define R_PCH_SATA_BUS_MASTER_BAR           0x20
#define B_PCH_SATA_BUS_MASTER_BAR_BA        0x0000FFE0
#define B_PCH_SATA_BUS_MASTER_BAR_BA4       BIT4
#define B_PCH_SATA_BUS_MASTER_BAR_RTE       BIT0
#define R_PCH_SATA_SIDP_BAR                 0x24
#define R_PCH_SATA_AHCI_BAR                 0x24
#define B_PCH_SATA_AHCI_BAR_BA              0xFFFFF800
#define V_PCH_SATA_AHCI_BAR_LENGTH          0x800
#define N_PCH_SATA_AHCI_BAR_ALIGNMENT       11
#define B_PCH_SATA_AHCI_BAR_PF              BIT3
#define B_PCH_SATA_AHCI_BAR_TP              (BIT2 | BIT1)
#define B_PCH_SATA_AHCI_BAR_RTE             BIT0
#define R_PCH_SATA_AHCI_SVID                0x2C
#define B_PCH_SATA_AHCI_SVID                0xFFFF
#define R_PCH_SATA_AHCI_SID                 0x2E
#define B_PCH_SATA_AHCI_SID                 0xFFFF
#define R_PCH_SATA_AHCI_CAP_PTR             0x34
#define B_PCH_SATA_AHCI_CAP_PTR             0xFF
#define R_PCH_SATA_AHCI_INTLN               0x3C
#define B_PCH_SATA_AHCI_INTLN               0xFF
#define R_PCH_SATA_AHCI_INTPN               0x3D
#define B_PCH_SATA_AHCI_INTPN               0xFF
#define R_PCH_SATA_TIMP                     0x40
#define R_PCH_SATA_TIMS                     0x42
#define B_PCH_SATA_TIM_IDE                  BIT15 ///< IDE Decode Enable
#define R_PCH_SATA_PID                      0x70
#define B_PCH_SATA_PID_NEXT                 0xFF00
#define V_PCH_SATA_PID_NEXT_0               0xB000
#define V_PCH_SATA_PID_NEXT_1               0xA800
#define B_PCH_SATA_PID_CID                  0x00FF
#define R_PCH_SATA_PC                       0x72
#define S_PCH_SATA_PC                       2
#define B_PCH_SATA_PC_PME                   (BIT15 | BIT14 | BIT13 | BIT12 | BIT11)
#define V_PCH_SATA_PC_PME_0                 0x0000
#define V_PCH_SATA_PC_PME_1                 0x4000
#define B_PCH_SATA_PC_D2_SUP                BIT10
#define B_PCH_SATA_PC_D1_SUP                BIT9
#define B_PCH_SATA_PC_AUX_CUR               (BIT8 | BIT7 | BIT6)
#define B_PCH_SATA_PC_DSI                   BIT5
#define B_PCH_SATA_PC_PME_CLK               BIT3
#define B_PCH_SATA_PC_VER                   (BIT2 | BIT1 | BIT0)
#define R_PCH_SATA_PMCS                     0x74
#define B_PCH_SATA_PMCS_PMES                BIT15
#define B_PCH_SATA_PMCS_PMEE                BIT8
#define B_PCH_SATA_PMCS_NSFRST              BIT3
#define V_PCH_SATA_PMCS_NSFRST_1            0x01
#define V_PCH_SATA_PMCS_NSFRST_0            0x00
#define B_PCH_SATA_PMCS_PS                  (BIT1 | BIT0)
#define V_PCH_SATA_PMCS_PS_3                0x03
#define V_PCH_SATA_PMCS_PS_0                0x00
#define R_PCH_SATA_MID                      0x80
#define B_PCH_SATA_MID_NEXT                 0xFF00
#define B_PCH_SATA_MID_CID                  0x00FF
#define R_PCH_SATA_MC                       0x82
#define B_PCH_SATA_MC_C64                   BIT7
#define B_PCH_SATA_MC_MME                   (BIT6 | BIT5 | BIT4)
#define V_PCH_SATA_MC_MME_4                 0x04
#define V_PCH_SATA_MC_MME_2                 0x02
#define V_PCH_SATA_MC_MME_1                 0x01
#define V_PCH_SATA_MC_MME_0                 0x00
#define B_PCH_SATA_MC_MMC                   (BIT3 | BIT2 | BIT1)
#define V_PCH_SATA_MC_MMC_4                 0x04
#define V_PCH_SATA_MC_MMC_0                 0x00
#define B_PCH_SATA_MC_MSIE                  BIT0
#define V_PCH_SATA_MC_MSIE_1                0x01
#define V_PCH_SATA_MC_MSIE_0                0x00
#define R_PCH_SATA_MA                       0x84
#define B_PCH_SATA_MA                       0xFFFFFFFC
#define R_PCH_SATA_MD                       0x88
#define B_PCH_SATA_MD_MSIMD                 0xFFFF
#define R_PCH_SATA_MAP                      0x90
#define B_PCH_H_SATA_MAP_SPD                (BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8)
#define B_PCH_LP_SATA_MAP_SPD               (BIT11 | BIT10 | BIT9 | BIT8)
#define B_PCH_SATA_PORT5_DISABLED           BIT13
#define B_PCH_SATA_PORT4_DISABLED           BIT12
#define B_PCH_SATA_PORT3_DISABLED           BIT11
#define B_PCH_SATA_PORT2_DISABLED           BIT10
#define B_PCH_SATA_PORT1_DISABLED           BIT9
#define B_PCH_SATA_PORT0_DISABLED           BIT8
#define B_PCH_SATA2_MAP_SPD                 (BIT9 | BIT8)
#define B_PCH_SATA2_PORT5_DISABLED          BIT9
#define B_PCH_SATA2_PORT4_DISABLED          BIT8
#define B_PCH_SATA_MAP_SMS_MASK             (BIT7 | BIT6)
#define V_PCH_SATA_MAP_SMS_LOOBACK_TESTMODE 0x00
#define V_PCH_SATA_MAP_SMS_IDE              0x00
#define V_PCH_SATA_MAP_SMS_AHCI             0x40
#define V_PCH_SATA_MAP_SMS_RAID             0x80
#define B_PCH_SATA_PORT_TO_CONTROLLER_CFG   BIT5
#define R_PCH_SATA_PCS                      0x92
#define S_PCH_SATA_PCS                      0x2
#define B_PCH_SATA_PCS_OOB_RETRY            BIT15
#define B_PCH_SATA_PCS_PORT5_DET            BIT13
#define B_PCH_SATA_PCS_PORT4_DET            BIT12
#define B_PCH_SATA_PCS_PORT3_DET            BIT11
#define B_PCH_SATA_PCS_PORT2_DET            BIT10
#define B_PCH_SATA_PCS_PORT1_DET            BIT9
#define B_PCH_SATA_PCS_PORT0_DET            BIT8
#define B_PCH_SATA_PCS_PORT5_EN             BIT5
#define B_PCH_SATA_PCS_PORT4_EN             BIT4
#define B_PCH_SATA_PCS_PORT3_EN             BIT3
#define B_PCH_SATA_PCS_PORT2_EN             BIT2
#define B_PCH_SATA_PCS_PORT1_EN             BIT1
#define B_PCH_SATA_PCS_PORT0_EN             BIT0
#define B_PCH_SATA2_PCS_PORT5_DET           BIT9
#define B_PCH_SATA2_PCS_PORT4_DET           BIT8
#define B_PCH_SATA2_PCS_PORT5_EN            BIT1
#define B_PCH_SATA2_PCS_PORT4_EN            BIT0
#define R_PCH_SATA_SCLKCG                   0x94
#define B_PCH_SATA_SCLKCG_PORT5_PCD         BIT29
#define B_PCH_SATA_SCLKCG_PORT4_PCD         BIT28
#define B_PCH_SATA_SCLKCG_PORT3_PCD         BIT27
#define B_PCH_SATA_SCLKCG_PORT2_PCD         BIT26
#define B_PCH_SATA_SCLKCG_PORT1_PCD         BIT25
#define B_PCH_SATA_SCLKCG_PORT0_PCD         BIT24
#define B_PCH_SATA_SCLKCG_POP3_DEVSLP       BIT15
#define R_PCH_SATA_SCLKGC                   0x9C
#define B_PCH_SATA_SCLKGC_AIE               BIT7
#define B_PCH_SATA_SCLKGC_AIES              BIT6
#define B_PCH_SATA_SCLKGC_SATATM_MASK       0x7C
#define B_PCH_SATA_SCLKGC_SATATM_EN         (BIT3 | BIT2)
#define B_PCH_SATA_SCLKGC_SATA4PMIND        BIT0
#define R_PCH_SATA_SIRI                     0xA0
#define B_PCH_SATA_SIRI_IDX                 0xFC
#define R_PCH_SATA_STRD                     0xA4
#define B_PCH_SATA_STRD_DTA                 0xFFFFFFFF
#define R_PCH_SATA_CR0                      0xA8
#define B_PCH_SATA_CR0_MAJREV               0x00F00000
#define B_PCH_SATA_CR0_MINREV               0x000F0000
#define B_PCH_SATA_CR0_NEXT                 0x0000FF00
#define B_PCH_SATA_CR0_CAP                  0x000000FF
#define R_PCH_SATA_CR1                      0xAC
#define B_PCH_SATA_CR1_BAROFST              0xFFF0
#define B_PCH_SATA_CR1_BARLOC               0x000F
#define R_PCH_SATA_FLR_CID                  0xB0
#define B_PCH_SATA_FLR_CID_NEXT             0xFF00
#define B_PCH_SATA_FLR_CID                  0x00FF
#define V_PCH_SATA_FLR_CID_1                0x0009
#define V_PCH_SATA_FLR_CID_0                0x0013
#define R_PCH_SATA_FLR_CLV                  0xB2
#define B_PCH_SATA_FLR_CLV_FLRC_FLRCSSEL_0  BIT9
#define B_PCH_SATA_FLR_CLV_TXPC_FLRCSSEL_0  BIT8
#define B_PCH_SATA_FLR_CLV_VSCID_FLRCSSEL_0 0x00FF
#define B_PCH_SATA_FLR_CLV_VSCID_FLRCSSEL_1 0x00FF
#define V_PCH_SATA_FLR_CLV_VSCID_FLRCSSEL   0x0006
#define R_PCH_SATA_FLRC                     0xB4
#define B_PCH_SATA_FLRC_TXP                 BIT8
#define B_PCH_SATA_FLRC_INITFLR             BIT0
#define R_PCH_SATA_ATC                      0xC0
#define B_PCH_SATA_ATC_SST                  BIT3
#define B_PCH_SATA_ATC_SPT                  BIT2
#define B_PCH_SATA_ATC_PST                  BIT1
#define B_PCH_SATA_ATC_PMT                  BIT0
#define R_PCH_SATA_ATS                      0xC4
#define B_PCH_SATA_ATS_SST                  BIT3
#define B_PCH_SATA_ATS_SPT                  BIT2
#define B_PCH_SATA_ATS_PST                  BIT1
#define B_PCH_SATA_ATS_PMT                  BIT0
#define R_PCH_SATA_SP                       0xD0
#define B_PCH_SATA_SP                       0xFFFFFFFF
#define R_PCH_SATA_BFCS                     0xE0
#define B_PCH_SATA_BFCS_P5BFI               BIT15
#define B_PCH_SATA_BFCS_P4BFI               BIT14
#define B_PCH_SATA_BFCS_P3BFI               BIT13
#define B_PCH_SATA_BFCS_P2BFI               BIT12
#define B_PCH_SATA_BFCS_P2BFS               BIT11
#define B_PCH_SATA_BFCS_P2BFF               BIT10
#define B_PCH_SATA_BFCS_P1BFI               BIT9
#define B_PCH_SATA_BFCS_P0BFI               BIT8
#define B_PCH_SATA_BFCS_BIST_FIS_T          BIT7
#define B_PCH_SATA_BFCS_BIST_FIS_A          BIT6
#define B_PCH_SATA_BFCS_BIST_FIS_S          BIT5
#define B_PCH_SATA_BFCS_BIST_FIS_L          BIT4
#define B_PCH_SATA_BFCS_BIST_FIS_F          BIT3
#define B_PCH_SATA_BFCS_BIST_FIS_P          BIT2
#define R_PCH_SATA_BFTD1                    0xE4
#define B_PCH_SATA_BFTD1                    0xFFFFFFFF
#define R_PCH_SATA_BFTD2                    0xE8
#define B_PCH_SATA_BFTD2                    0xFFFFFFFF

//
// Serial ATA Index/Data Pair Superset Registers
//
#define R_PCH_SATA_SIDPBA_SINDX             0x00
#define R_PCH_SATA_SIDPBA_SDATA             0x04
#define V_PCH_SATA_AHCI_SINDX_RIDX_SCTL     0x01
#define V_PCH_SATA_AHCI_SINDX_PIDX_PORT0    0x0000
#define V_PCH_SATA_AHCI_SINDX_PIDX_PORT1    0x0200
#define V_PCH_SATA_AHCI_SINDX_PIDX_PORT2    0x0100
#define V_PCH_SATA_AHCI_SINDX_PIDX_PORT3    0x0300
#define V_PCH_SATA_SIDPBA_SDATA_GEN1        0x10
#define V_PCH_SATA_SIDPBA_SDATA_GEN2        0x20
#define V_PCH_SATA_SIDPBA_SDATA_GEN3        0x30
#define B_PCH_SATA_SIDPBA_SCTL_DET          (BIT3|BIT2|BIT1|BIT0)
#define V_PCH_SATA_SIDPBA_SCTL_DET_COMRST   0x01
#define V_PCH_SATA_SIDPBA_SCTL_DET_NOINT    0x00
#define V_PCH_SATA_SIDP_BAR_LENGTH          0x10
#define N_PCH_SATA_SIDP_BAR_ALIGNMENT       0x04

//
// AHCI BAR Area related Registers
//
#define R_PCH_SATA_AHCI_CAP                 0x0
#define B_PCH_SATA_AHCI_CAP_S64A            BIT31
#define B_PCH_SATA_AHCI_CAP_SCQA            BIT30
#define B_PCH_SATA_AHCI_CAP_SSNTF           BIT29
#define B_PCH_SATA_AHCI_CAP_SIS             BIT28 ///< Supports Interlock Switch
#define B_PCH_SATA_AHCI_CAP_SSS             BIT27 ///< Supports Stagger Spin-up
#define B_PCH_SATA_AHCI_CAP_SALP            BIT26
#define B_PCH_SATA_AHCI_CAP_SAL             BIT25
#define B_PCH_SATA_AHCI_CAP_SCLO            BIT24 ///< Supports Command List Override
#define B_PCH_SATA_AHCI_CAP_ISS_MASK        (BIT23 | BIT22 | BIT21 | BIT20)
#define N_PCH_SATA_AHCI_CAP_ISS             20    ///< Interface Speed Support
#define V_PCH_SATA_AHCI_CAP_ISS_1_5_G       0x01
#define V_PCH_SATA_AHCI_CAP_ISS_3_0_G       0x02
#define V_PCH_SATA_AHCI_CAP_ISS_6_0_G       0x03
#define B_PCH_SATA_AHCI_CAP_SNZO            BIT19
#define B_PCH_SATA_AHCI_CAP_SAM             BIT18
#define B_PCH_SATA_AHCI_CAP_PMS             BIT17 ///< Supports Port Multiplier
#define B_PCH_SATA_AHCI_CAP_PMD             BIT15 ///< PIO Multiple DRQ Block
#define B_PCH_SATA_AHCI_CAP_SSC             BIT14
#define B_PCH_SATA_AHCI_CAP_PSC             BIT13
#define B_PCH_SATA_AHCI_CAP_NCS             0x1F00
#define B_PCH_SATA_AHCI_CAP_CCCS            BIT7
#define B_PCH_SATA_AHCI_CAP_EMS             BIT6
#define B_PCH_SATA_AHCI_CAP_SXS             BIT5  ///< External SATA is supported
#define B_PCH_SATA_AHCI_CAP_NPS             0x001F

#define R_PCH_SATA_AHCI_GHC                 0x04
#define B_PCH_SATA_AHCI_GHC_AE              BIT31
#define B_PCH_SATA_AHCI_GHC_MRSM            BIT2
#define B_PCH_SATA_AHCI_GHC_IE              BIT1
#define B_PCH_SATA_AHCI_GHC_HR              BIT0

#define R_PCH_SATA_AHCI_IS                  0x08
#define B_PCH_SATA_AHCI_IS_PORT5            BIT5
#define B_PCH_SATA_AHCI_IS_PORT4            BIT4
#define B_PCH_SATA_AHCI_IS_PORT3            BIT3
#define B_PCH_SATA_AHCI_IS_PORT2            BIT2
#define B_PCH_SATA_AHCI_IS_PORT1            BIT1
#define B_PCH_SATA_AHCI_IS_PORT0            BIT0
#define R_PCH_SATA_AHCI_PI                  0x0C
#define B_PCH_H_SATA_PORT_MASK              0x3F
#define B_PCH_LP_SATA_PORT_MASK             0x0F
#define B_PCH_SATA_PORT5_IMPLEMENTED        BIT5
#define B_PCH_SATA_PORT4_IMPLEMENTED        BIT4
#define B_PCH_SATA_PORT3_IMPLEMENTED        BIT3
#define B_PCH_SATA_PORT2_IMPLEMENTED        BIT2
#define B_PCH_SATA_PORT1_IMPLEMENTED        BIT1
#define B_PCH_SATA_PORT0_IMPLEMENTED        BIT0
#define R_PCH_SATA_AHCI_VS                  0x10
#define B_PCH_SATA_AHCI_VS_MJR              0xFFFF0000
#define B_PCH_SATA_AHCI_VS_MNR              0x0000FFFF
#define R_PCH_SATA_AHCI_EM_LOC              0x1C
#define B_PCH_SATA_AHCI_EM_LOC_OFST         0xFFFF0000
#define B_PCH_SATA_AHCI_EM_LOC_SZ           0x0000FFFF
#define R_PCH_SATA_AHCI_EM_CTRL             0x20
#define B_PCH_SATA_AHCI_EM_CTRL_ATTR_ALHD   BIT26
#define B_PCH_SATA_AHCI_EM_CTRL_ATTR_XMT    BIT25
#define B_PCH_SATA_AHCI_EM_CTRL_ATTR_SMB    BIT24
#define B_PCH_SATA_AHCI_EM_CTRL_SUPP_SGPIO  BIT19
#define B_PCH_SATA_AHCI_EM_CTRL_SUPP_SES2   BIT18
#define B_PCH_SATA_AHCI_EM_CTRL_SUPP_SAFTE  BIT17
#define B_PCH_SATA_AHCI_EM_CTRL_SUPP_LED    BIT16
#define B_PCH_SATA_AHCI_EM_CTRL_RST         BIT9
#define B_PCH_SATA_AHCI_EM_CTRL_CTL_TM      BIT8
#define B_PCH_SATA_AHCI_EM_CTRL_STS_MR      BIT0
#define R_PCH_SATA_AHCI_CAP2                0x24
#define B_PCH_SATA_AHCI_CAP2_DESO           BIT5
#define B_PCH_SATA_AHCI_CAP2_SADM           BIT4
#define B_PCH_SATA_AHCI_CAP2_SDS            BIT3
#define B_PCH_SATA_AHCI_CAP2_APST           BIT2  ///< Automatic Partial to Slumber Transitions
#define R_PCH_SATA_AHCI_VSP                 0xA0
#define B_PCH_SATA_AHCI_VSP_SLPD            BIT0
#define R_PCH_SATA_AHCI_RSTF                0xC8  ///< RST Feature Capabilities
#define B_PCH_SATA_AHCI_RSTF_OUD            (BIT11 | BIT10)
#define N_PCH_SATA_AHCI_RSTF_OUD            10
#define B_PCH_SATA_AHCI_RSTF_SEREQ          BIT9
#define B_PCH_SATA_AHCI_RSTF_IROES          BIT8
#define B_PCH_SATA_AHCI_RSTF_LEDL           BIT7
#define B_PCH_SATA_AHCI_RSTF_HDDLK          BIT6
#define B_PCH_SATA_AHCI_RSTF_IRSTOROM       BIT5
#define B_PCH_SATA_AHCI_RSTF_RSTE           BIT4
#define B_PCH_SATA_AHCI_RSTF_R5E            BIT3
#define B_PCH_SATA_AHCI_RSTF_R10E           BIT2
#define B_PCH_SATA_AHCI_RSTF_R1E            BIT1
#define B_PCH_SATA_AHCI_RSTF_R0E            BIT0
#define B_PCH_SATA_AHCI_RSTF_LOWBYTES       0x1FF
#define R_PCH_SATA_AHCI_P0CLB               0x100
#define R_PCH_SATA_AHCI_P1CLB               0x180
#define R_PCH_SATA_AHCI_P2CLB               0x200
#define R_PCH_SATA_AHCI_P3CLB               0x280
#define R_PCH_SATA_AHCI_P4CLB               0x300
#define R_PCH_SATA_AHCI_P5CLB               0x380
#define R_PCH_SATA_AHCI_P6CLB               0x400
#define R_PCH_SATA_AHCI_P7CLB               0x480
#define B_PCH_SATA_AHCI_PXCLB               0xFFFFFC00
#define R_PCH_SATA_AHCI_P0CLBU              0x104
#define R_PCH_SATA_AHCI_P1CLBU              0x184
#define R_PCH_SATA_AHCI_P2CLBU              0x204
#define R_PCH_SATA_AHCI_P3CLBU              0x284
#define R_PCH_SATA_AHCI_P4CLBU              0x304
#define R_PCH_SATA_AHCI_P5CLBU              0x384
#define R_PCH_SATA_AHCI_P6CLBU              0x404
#define B_PCH_SATA_AHCI_PXCLBU              0xFFFFFFFF
#define R_PCH_SATA_AHCI_P0FB                0x108
#define R_PCH_SATA_AHCI_P1FB                0x188
#define R_PCH_SATA_AHCI_P2FB                0x208
#define R_PCH_SATA_AHCI_P3FB                0x288
#define R_PCH_SATA_AHCI_P4FB                0x308
#define R_PCH_SATA_AHCI_P5FB                0x388
#define R_PCH_SATA_AHCI_P6FB                0x408
#define B_PCH_SATA_AHCI_PXFB                0xFFFFFF00
#define R_PCH_SATA_AHCI_P0FBU               0x10C
#define R_PCH_SATA_AHCI_P1FBU               0x18C
#define R_PCH_SATA_AHCI_P2FBU               0x20C
#define R_PCH_SATA_AHCI_P3FBU               0x28C
#define R_PCH_SATA_AHCI_P4FBU               0x30C
#define R_PCH_SATA_AHCI_P5FBU               0x38C
#define R_PCH_SATA_AHCI_P6FBU               0x40C
#define B_PCH_SATA_AHCI_PXFBU               0xFFFFFFFF
#define R_PCH_SATA_AHCI_P0IS                0x110
#define R_PCH_SATA_AHCI_P1IS                0x190
#define R_PCH_SATA_AHCI_P2IS                0x210
#define R_PCH_SATA_AHCI_P3IS                0x290
#define R_PCH_SATA_AHCI_P4IS                0x310
#define R_PCH_SATA_AHCI_P5IS                0x390
#define R_PCH_SATA_AHCI_P6IS                0x410
#define B_PCH_SATA_AHCI_PXIS_CPDS           BIT31
#define B_PCH_SATA_AHCI_PXIS_TFES           BIT30
#define B_PCH_SATA_AHCI_PXIS_HBFS           BIT29
#define B_PCH_SATA_AHCI_PXIS_HBDS           BIT28
#define B_PCH_SATA_AHCI_PXIS_IFS            BIT27
#define B_PCH_SATA_AHCI_PXIS_INFS           BIT26
#define B_PCH_SATA_AHCI_PXIS_OFS            BIT24
#define B_PCH_SATA_AHCI_PXIS_IPMS           BIT23
#define B_PCH_SATA_AHCI_PXIS_PRCS           BIT22
#define B_PCH_SATA_AHCI_PXIS_DIS            BIT7
#define B_PCH_SATA_AHCI_PXIS_PCS            BIT6
#define B_PCH_SATA_AHCI_PXIS_DPS            BIT5
#define B_PCH_SATA_AHCI_PXIS_UFS            BIT4
#define B_PCH_SATA_AHCI_PXIS_SDBS           BIT3
#define B_PCH_SATA_AHCI_PXIS_DSS            BIT2
#define B_PCH_SATA_AHCI_PXIS_PSS            BIT1
#define B_PCH_SATA_AHCI_PXIS_DHRS           BIT0
#define R_PCH_SATA_AHCI_P0IE                0x114
#define R_PCH_SATA_AHCI_P1IE                0x194
#define R_PCH_SATA_AHCI_P2IE                0x214
#define R_PCH_SATA_AHCI_P3IE                0x294
#define R_PCH_SATA_AHCI_P4IE                0x314
#define R_PCH_SATA_AHCI_P5IE                0x394
#define R_PCH_SATA_AHCI_P6IE                0x414
#define B_PCH_SATA_AHCI_PXIE_CPDE           BIT31
#define B_PCH_SATA_AHCI_PXIE_TFEE           BIT30
#define B_PCH_SATA_AHCI_PXIE_HBFE           BIT29
#define B_PCH_SATA_AHCI_PXIE_HBDE           BIT28
#define B_PCH_SATA_AHCI_PXIE_IFE            BIT27
#define B_PCH_SATA_AHCI_PXIE_INFE           BIT26
#define B_PCH_SATA_AHCI_PXIE_OFE            BIT24
#define B_PCH_SATA_AHCI_PXIE_IPME           BIT23
#define B_PCH_SATA_AHCI_PXIE_PRCE           BIT22
#define B_PCH_SATA_AHCI_PXIE_DIE            BIT7
#define B_PCH_SATA_AHCI_PXIE_PCE            BIT6
#define B_PCH_SATA_AHCI_PXIE_DPE            BIT5
#define B_PCH_SATA_AHCI_PXIE_UFIE           BIT4
#define B_PCH_SATA_AHCI_PXIE_SDBE           BIT3
#define B_PCH_SATA_AHCI_PXIE_DSE            BIT2
#define B_PCH_SATA_AHCI_PXIE_PSE            BIT1
#define B_PCH_SATA_AHCI_PXIE_DHRE           BIT0
#define R_PCH_SATA_AHCI_P0CMD               0x118
#define R_PCH_SATA_AHCI_P1CMD               0x198
#define R_PCH_SATA_AHCI_P2CMD               0x218
#define R_PCH_SATA_AHCI_P3CMD               0x298
#define R_PCH_SATA_AHCI_P4CMD               0x318
#define R_PCH_SATA_AHCI_P5CMD               0x398
#define R_PCH_SATA_AHCI_P6CMD               0x418
#define B_PCH_SATA_AHCI_PxCMD_ICC           (BIT31 | BIT30 | BIT29 | BIT28)
#define B_PCH_SATA_AHCI_PxCMD_MASK          (BIT27 | BIT26 | BIT21 | BIT22 | BIT19 | BIT18)
#define B_PCH_SATA_AHCI_PxCMD_ASP           BIT27
#define B_PCH_SATA_AHCI_PxCMD_ALPE          BIT26
#define B_PCH_SATA_AHCI_PxCMD_DLAE          BIT25
#define B_PCH_SATA_AHCI_PxCMD_ATAPI         BIT24
#define B_PCH_SATA_AHCI_PxCMD_APSTE         BIT23
#define B_PCH_SATA_AHCI_PxCMD_SUD           BIT1
#define R_PCH_SATA_AHCI_P0DEVSLP            0x144
#define R_PCH_SATA_AHCI_P1DEVSLP            0x1C4
#define R_PCH_SATA_AHCI_P2DEVSLP            0x244
#define R_PCH_SATA_AHCI_P3DEVSLP            0x2C4
#define B_PCH_SATA_AHCI_PxDEVSLP_DSP        BIT1
#define B_PCH_SATA_AHCI_PxDEVSLP_ADSE       BIT0
#define B_PCH_SATA_AHCI_PxDEVSLP_DITO_MASK  0x01FF8000
#define V_PCH_SATA_AHCI_PxDEVSLP_DITO_625   0x01388000
#define B_PCH_SATA_AHCI_PxDEVSLP_DM_MASK    0x1E000000
#define V_PCH_SATA_AHCI_PxDEVSLP_DM_16      0x1E000000
#define B_PCH_SATA_AHCI_PxCMD_ESP           BIT21 ///< Used with an external SATA device
#define B_PCH_SATA_AHCI_PxCMD_MPSP          BIT19 ///< Mechanical Switch Attached to Port
#define B_PCH_SATA_AHCI_PxCMD_HPCP          BIT18 ///< Hotplug capable
#define B_PCH_SATA_AHCI_PxCMD_CR            BIT15
#define B_PCH_SATA_AHCI_PxCMD_FR            BIT14
#define B_PCH_SATA_AHCI_PxCMD_ISS           BIT13
#define B_PCH_SATA_AHCI_PxCMD_CCS           0x00001F00
#define B_PCH_SATA_AHCI_PxCMD_FRE           BIT4
#define B_PCH_SATA_AHCI_PxCMD_CLO           BIT3
#define B_PCH_SATA_AHCI_PxCMD_POD           BIT2
#define B_PCH_SATA_AHCI_PxCMD_SUD           BIT1
#define B_PCH_SATA_AHCI_PxCMD_ST            BIT0
#define R_PCH_SATA_AHCI_P0TFD               0x120
#define R_PCH_SATA_AHCI_P1TFD               0x1A0
#define R_PCH_SATA_AHCI_P2TFD               0x220
#define R_PCH_SATA_AHCI_P3TFD               0x2A0
#define R_PCH_SATA_AHCI_P4TFD               0x320
#define R_PCH_SATA_AHCI_P5TFD               0x3A0
#define R_PCH_SATA_AHCI_P6TFD               0x420
#define B_PCH_SATA_AHCI_PXTFD_ERR           0x0000FF00
#define B_PCH_SATA_AHCI_PXTFD_STS           0x000000FF
#define R_PCH_SATA_AHCI_P0SIG               0x124
#define R_PCH_SATA_AHCI_P1SIG               0x1A4
#define R_PCH_SATA_AHCI_P2SIG               0x224
#define R_PCH_SATA_AHCI_P3SIG               0x2A4
#define R_PCH_SATA_AHCI_P4SIG               0x324
#define R_PCH_SATA_AHCI_P5SIG               0x3A4
#define R_PCH_SATA_AHCI_P6SIG               0x424
#define B_PCH_SATA_AHCI_PXSIG_LBA_HR        0xFF000000
#define B_PCH_SATA_AHCI_PXSIG_LBA_MR        0x00FF0000
#define B_PCH_SATA_AHCI_PXSIG_LBA_LR        0x0000FF00
#define B_PCH_SATA_AHCI_PXSIG_SCR           0x000000FF
#define R_PCH_SATA_AHCI_P0SSTS              0x128
#define R_PCH_SATA_AHCI_P1SSTS              0x1A8
#define R_PCH_SATA_AHCI_P2SSTS              0x228
#define R_PCH_SATA_AHCI_P3SSTS              0x2A8
#define R_PCH_SATA_AHCI_P4SSTS              0x328
#define R_PCH_SATA_AHCI_P5SSTS              0x3A8
#define R_PCH_SATA_AHCI_P6SSTS              0x428
#define B_PCH_SATA_AHCI_PXSSTS_IPM_0        0x00000000
#define B_PCH_SATA_AHCI_PXSSTS_IPM_1        0x00000100
#define B_PCH_SATA_AHCI_PXSSTS_IPM_2        0x00000200
#define B_PCH_SATA_AHCI_PXSSTS_IPM_6        0x00000600
#define B_PCH_SATA_AHCI_PXSSTS_SPD_0        0x00000000
#define B_PCH_SATA_AHCI_PXSSTS_SPD_1        0x00000010
#define B_PCH_SATA_AHCI_PXSSTS_SPD_2        0x00000020
#define B_PCH_SATA_AHCI_PXSSTS_SPD_3        0x00000030
#define B_PCH_SATA_AHCI_PXSSTS_DET_0        0x00000000
#define B_PCH_SATA_AHCI_PXSSTS_DET_1        0x00000001
#define B_PCH_SATA_AHCI_PXSSTS_DET_3        0x00000003
#define B_PCH_SATA_AHCI_PXSSTS_DET_4        0x00000004
#define R_PCH_SATA_AHCI_P0SCTL              0x12C
#define R_PCH_SATA_AHCI_P1SCTL              0x1AC
#define R_PCH_SATA_AHCI_P2SCTL              0x22C
#define R_PCH_SATA_AHCI_P3SCTL              0x2AC
#define R_PCH_SATA_AHCI_P4SCTL              0x32C
#define R_PCH_SATA_AHCI_P5SCTL              0x3AC
#define R_PCH_SATA_AHCI_P6SCTL              0x42C
#define B_PCH_SATA_AHCI_PXSCTL_IPM          0x00000F00
#define V_PCH_SATA_AHCI_PXSCTL_IPM_0        0x00000000
#define V_PCH_SATA_AHCI_PXSCTL_IPM_1        0x00000100
#define V_PCH_SATA_AHCI_PXSCTL_IPM_2        0x00000200
#define V_PCH_SATA_AHCI_PXSCTL_IPM_3        0x00000300
#define B_PCH_SATA_AHCI_PXSCTL_SPD          0x000000F0
#define V_PCH_SATA_AHCI_PXSCTL_SPD_0        0x00000000
#define V_PCH_SATA_AHCI_PXSCTL_SPD_1        0x00000010
#define V_PCH_SATA_AHCI_PXSCTL_SPD_2        0x00000020
#define V_PCH_SATA_AHCI_PXSCTL_SPD_3        0x00000030
#define B_PCH_SATA_AHCI_PXSCTL_DET          0x0000000F
#define V_PCH_SATA_AHCI_PXSCTL_DET_0        0x00000000
#define V_PCH_SATA_AHCI_PXSCTL_DET_1        0x00000001
#define V_PCH_SATA_AHCI_PXSCTL_DET_4        0x00000004
#define R_PCH_SATA_AHCI_P0SERR              0x130
#define R_PCH_SATA_AHCI_P1SERR              0x1B0
#define R_PCH_SATA_AHCI_P2SERR              0x230
#define R_PCH_SATA_AHCI_P3SERR              0x2B0
#define R_PCH_SATA_AHCI_P4SERR              0x330
#define R_PCH_SATA_AHCI_P5SERR              0x3B0
#define R_PCH_SATA_AHCI_P6SERR              0x430
#define B_PCH_SATA_AHCI_PXSERR_EXCHG        BIT26
#define B_PCH_SATA_AHCI_PXSERR_UN_FIS_TYPE  BIT25
#define B_PCH_SATA_AHCI_PXSERR_TRSTE_24     BIT24
#define B_PCH_SATA_AHCI_PXSERR_TRSTE_23     BIT23
#define B_PCH_SATA_AHCI_PXSERR_HANDSHAKE    BIT22
#define B_PCH_SATA_AHCI_PXSERR_CRC_ERROR    BIT21
#define B_PCH_SATA_AHCI_PXSERR_10B8B_DECERR BIT19
#define B_PCH_SATA_AHCI_PXSERR_COMM_WAKE    BIT18
#define B_PCH_SATA_AHCI_PXSERR_PHY_ERROR    BIT17
#define B_PCH_SATA_AHCI_PXSERR_PHY_RDY_CHG  BIT16
#define B_PCH_SATA_AHCI_PXSERR_INTRNAL_ERR  BIT11
#define B_PCH_SATA_AHCI_PXSERR_PROTOCOL_ERR BIT10
#define B_PCH_SATA_AHCI_PXSERR_PCDIE        BIT9
#define B_PCH_SATA_AHCI_PXSERR_TDIE         BIT8
#define B_PCH_SATA_AHCI_PXSERR_RCE          BIT1
#define B_PCH_SATA_AHCI_PXSERR_RDIE         BIT0
#define R_PCH_SATA_AHCI_P0SACT              0x134
#define R_PCH_SATA_AHCI_P1SACT              0x1B4
#define R_PCH_SATA_AHCI_P2SACT              0x234
#define R_PCH_SATA_AHCI_P3SACT              0x2B4
#define R_PCH_SATA_AHCI_P4SACT              0x334
#define R_PCH_SATA_AHCI_P5SACT              0x3B4
#define R_PCH_SATA_AHCI_P6SACT              0x434
#define B_PCH_SATA_AHCI_PXSACT_DS           0xFFFFFFFF
#define R_PCH_SATA_AHCI_P0CI                0x138
#define R_PCH_SATA_AHCI_P1CI                0x1B8
#define R_PCH_SATA_AHCI_P2CI                0x238
#define R_PCH_SATA_AHCI_P3CI                0x2B8
#define R_PCH_SATA_AHCI_P4CI                0x338
#define R_PCH_SATA_AHCI_P5CI                0x3B8
#define R_PCH_SATA_AHCI_P6CI                0x438
#define B_PCH_SATA_AHCI_PXCI                0xFFFFFFFF

//
// Macros of ICH capabilities for SATA controller which are used by SATA controller driver
//
//
//
// Define the individual capabilities of each sata controller
//
#define LPTH_SATA_MAX_CONTROLLERS           2   ///< max sata controllers number supported
#define LPTLP_SATA_MAX_CONTROLLERS          1   ///< max sata controllers number supported
#define PCH_IDE_MAX_CHANNELS                2   ///< max channels number of single sata controller
#define PCH_IDE_MAX_DEVICES                 2   ///< max devices number of single sata channel
#define LPTH_AHCI_MAX_PORTS                 6   ///< max number of sata ports in LPTH
#define LPTLP_AHCI_MAX_PORTS                4   ///< max number of sata ports in LPTLP
#define PCH_SATA_DEVICE_ID_INVALID          0xFFFF
#define PCH_SATA_1_DEVICE_NUMBER            PCI_DEVICE_NUMBER_PCH_SATA
#define PCH_SATA_1_FUNCTION_NUMBER          PCI_FUNCTION_NUMBER_PCH_SATA
#define PCH_H_AHCI_1_MAX_PORTS              6   ///< max number of ports in sata in PCH
#define PCH_LP_AHCI_1_MAX_PORTS             4   ///< max number of ports in sata1 in PCH
#define PCH_IDE_1_MAX_CHANNELS              2
#define PCH_IDE_1_MAX_DEVICES               2
#define PCH_IDE_1_MAX_PORTS                 4

#define PCH_SATA_2_DEVICE_NUMBER            PCI_DEVICE_NUMBER_PCH_SATA2
#define PCH_SATA_2_FUNCTION_NUMBER          PCI_FUNCTION_NUMBER_PCH_SATA2
#define PCH_AHCI_2_MAX_PORTS                2   ///< number of ports in sata2 in PCH
#define PCH_IDE_2_MAX_CHANNELS              2
#define PCH_IDE_2_MAX_DEVICES               2
#define PCH_IDE_2_MAX_PORTS                 2

//
// GPIO SATA0GP is the Sata port 0 reset pin.
//
#define PCH_GPIO_SATA_PORT0_RESET           21
#define PCH_LP_GPIO_SATA_PORT0_RESET        (R_PCH_GP_N_CONFIG0 + (34 * 0x08))
//
// GPIO SATA1GP is the Sata port 1 reset pin.
//
#define PCH_GPIO_SATA_PORT1_RESET           19
#define PCH_LP_GPIO_SATA_PORT1_RESET        (R_PCH_GP_N_CONFIG0 + (35 * 0x08))

//
// GPIO SATA2GP is the Sata port 2 reset pin.
//
#define PCH_GPIO_SATA_PORT2_RESET           36
#define PCH_LP_GPIO_SATA_PORT2_RESET        (R_PCH_GP_N_CONFIG0 + (36 * 0x08))

//
// GPIO SATA3GP is the Sata port 3 reset pin.
//
#define PCH_GPIO_SATA_PORT3_RESET           37
#define PCH_LP_GPIO_SATA_PORT3_RESET        (R_PCH_GP_N_CONFIG0 + (37 * 0x08))

//
// GPIO SATA4GP is the Sata port 4 reset pin.
//
#define PCH_GPIO_SATA_PORT4_RESET           16
//
// GPIO SATA5GP is the Sata port 5 reset pin.
//
#define PCH_GPIO_SATA_PORT5_RESET           49

#endif
