/** @file
  Register names for PCH High Definition Audio device.

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
#ifndef _PCH_REGS_HDA_H_
#define _PCH_REGS_HDA_H_

//
// Azalia Controller Registers (D27:F0)
//
#define PCI_DEVICE_NUMBER_PCH_AZALIA    27
#define PCI_FUNCTION_NUMBER_PCH_AZALIA  0

#define R_PCH_HDA_VENDOR_ID             0x00
#define V_PCH_HDA_VENDOR_ID             V_PCH_INTEL_VENDOR_ID
#define R_PCH_HDA_DEVICE_ID             0x02
#define V_PCH_LPTH_HDA_DEVICE_ID        0x8C20
#define V_PCH_LPTLP_HDA_DEVICE_ID       0x9C20  ///< Azalia Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_HDA_DEVICE_ID_ALT   0x9C21  ///< Azalia Device ID bit[0] fuse = 1
#define R_PCH_HDA_COMMAND               0x04
#define B_PCH_HDA_COMMAND_INTR_DIS      BIT10
#define B_PCH_HDA_COMMAND_FBE           BIT9
#define B_PCH_HDA_COMMAND_SERR_EN       BIT8
#define B_PCH_HDA_COMMAND_WCC           BIT7
#define B_PCH_HDA_COMMAND_PER           BIT6
#define B_PCH_HDA_COMMAND_VPS           BIT5
#define B_PCH_HDA_COMMAND_MWIE          BIT4
#define B_PCH_HDA_COMMAND_SCE           BIT3
#define B_PCH_HDA_COMMAND_BME           BIT2
#define B_PCH_HDA_COMMAND_MSE           BIT1
#define B_PCH_HDA_COMMAND_IOSE          BIT0
#define R_PCH_HDA_STS                   0x06
#define B_PCH_HDA_STS_DPE               BIT15
#define B_PCH_HDA_STS_SSE               BIT14
#define B_PCH_HDA_STS_RMA               BIT13
#define B_PCH_HDA_STS_RTA               BIT12
#define B_PCH_HDA_STS_STA               BIT11
#define B_PCH_HDA_STS_DEV_STS           (BIT10 | BIT9)
#define B_PCH_HDA_STS_DPED              BIT8
#define B_PCH_HDA_STS_FB2BC             BIT7
#define B_PCH_HDA_STS_66MHZ_CAP         BIT5
#define B_PCH_HDA_STS_CAP_LST           BIT4
#define B_PCH_HDA_STS_INTR_STS          BIT3
#define R_PCH_HDA_RID                   0x08
#define B_PCH_HDA_RID                   0xFF
#define R_PCH_HDA_PI                    0x09
#define B_PCH_HDA_PI                    0xFF
#define R_PCH_HDA_SCC                   0x0A
#define B_PCH_HDA_SCC                   0xFF
#define R_PCH_HDA_BCC                   0x0B
#define B_PCH_HDA_BCC                   0xFF
#define R_PCH_HDA_CLS                   0x0C
#define B_PCH_HDA_CLS                   0xFF
#define R_PCH_HDA_LT                    0x0D
#define B_PCH_HDA_LT                    0xFF
#define R_PCH_HDA_HEADTYPE              0x0E
#define B_PCH_HDA_HEADTYPE              0xFF
#define R_PCH_HDA_HDBARL                0x10
#define B_PCH_HDA_HDBARL_LBA            0xFFFFC000
#define B_PCH_HDA_HDBARL_PREF           BIT3
#define B_PCH_HDA_HDBARL_ADDRNG         (BIT2 | BIT1)
#define B_PCH_HDA_HDBARL_SPTYP          BIT0
#define V_PCH_HDA_HDBAR_SIZE            (1 << 14)
#define R_PCH_HDA_HDBARU                0x14
#define B_PCH_HDA_HDBARU_UBA            0xFFFFFFFF
#define R_PCH_HDA_SVID                  0x2C
#define B_PCH_HDA_SVID                  0xFFFF
#define R_PCH_HDA_SID                   0x2E
#define B_PCH_HDA_SID                   0xFFFF
#define R_PCH_HDA_CAPPTR                0x34
#define B_PCH_HDA_CAPPTR                0xFF
#define R_PCH_HDA_INTLN                 0x3C
#define B_PCH_HDA_INTLN                 0xFF
#define R_PCH_HDA_INTPN                 0x3D
#define B_PCH_HDA_INTPN                 0x0F
#define R_PCH_HDA_HDCTL                 0x40
#define B_PCH_HDA_HDCTL_BCLD            BIT1
#define B_PCH_HDA_HDCTL_MODE            BIT0
#define R_PCH_HDA_AZIOBC                0x42
#define B_PCH_HDA_AZIOBC_OSEL           (BIT7 | BIT6)
#define B_PCH_HDA_AZIOBC_AVDDIS         BIT2
#define R_PCH_HDA_TCSEL                 0x44
#define B_PCH_HDA_TCSEL                 (BIT2 | BIT1 | BIT0)
#define V_PCH_HDA_TCSEL_TC0             0x00
#define V_PCH_HDA_TCSEL_TC1             0x01
#define V_PCH_HDA_TCSEL_TC2             0x02
#define V_PCH_HDA_TCSEL_TC3             0x03
#define V_PCH_HDA_TCSEL_TC4             0x04
#define V_PCH_HDA_TCSEL_TC5             0x05
#define V_PCH_HDA_TCSEL_TC6             0x06
#define V_PCH_HDA_TCSEL_TC7             0x07
#define R_PCH_HDA_DCKCTL                0x4C
#define B_PCH_HDA_DCKCTL_DA             BIT0
#define R_PCH_HDA_DCKSTS                0x4D
#define B_PCH_HDA_DCKSTS_DS             BIT7
#define B_PCH_HDA_DCKSTS_DM             BIT0
#define R_PCH_HDA_PID                   0x50
#define B_PCH_HDA_PID_NEXT              0xFF00
#define B_PCH_HDA_PID_CAP               0x00FF
#define R_PCH_HDA_PC                    0x52
#define B_PCH_HDA_PC_PME                0xF800
#define B_PCH_HDA_PC_D2_SUP             BIT10
#define B_PCH_HDA_PC_D1_SUP             BIT9
#define B_PCH_HDA_PC_AUX                (BIT8 | BIT7 | BIT6)
#define B_PCH_HDA_PC_DSI                BIT5
#define B_PCH_HDA_PC_PMEC               BIT3
#define B_PCH_HDA_PC_VER                (BIT2 | BIT1 | BIT0)
#define R_PCH_HDA_PCS                   0x54
#define B_PCH_HDA_PCS_DATA              0xFF000000
#define B_PCH_HDA_PCS_CCE               BIT23
#define B_PCH_HDA_PCS_PMES              BIT15
#define B_PCH_HDA_PCS_PMEE              BIT8
#define B_PCH_HDA_PCS_PS                (BIT1 | BIT0)
#define V_PCH_HDA_PCS_PS0               0x00
#define V_PCH_HDA_PCS_PS3               0x03
#define R_PCH_HDA_MID                   0x60
#define B_PCH_HDA_MID_NEXT              0xFF00
#define B_PCH_HDA_MID_CAP               0x00FF
#define R_PCH_HDA_MMC                   0x62
#define B_PCH_HDA_MMC_64ADD             BIT7
#define B_PCH_HDA_MMC_MME               (BIT6 | BIT5 | BIT4)
#define B_PCH_HDA_MMC_MMC               (BIT3 | BIT2 | BIT1)
#define B_PCH_HDA_MMC_ME                BIT0
#define R_PCH_HDA_MMLA                  0x64
#define B_PCH_HDA_MMLA                  0xFFFFFFFC
#define R_PCH_HDA_MMUA                  0x68
#define B_PCH_HDA_MMUA                  0xFFFFFFFF
#define R_PCH_HDA_MMD                   0x6C
#define B_PCH_HDA_MMD                   0xFFFF
#define R_PCH_HDA_PXID                  0x70
#define B_PCH_HDA_PXID_NEXT             0xFF00
#define B_PCH_HDA_PXID_CAP              0x00FF
#define R_PCH_HDA_PXC                   0x72
#define B_PCH_HDA_PXC_IMN               0x3E00
#define B_PCH_HDA_PXC_SI                BIT8
#define B_PCH_HDA_PXC_DPT               0x00F0
#define B_PCH_HDA_PXC_CV                0x000F
#define R_PCH_HDA_DEVCAP                0x74
#define B_PCH_HDA_DEVCAP_FLR            BIT28
#define B_PCH_HDA_DEVCAP_SPLS           (BIT27 | BIT26)
#define B_PCH_HDA_DEVCAP_SPLV           0x03FC0000
#define B_PCH_HDA_DEVCAP_PWRIP          BIT14
#define B_PCH_HDA_DEVCAP_ATTNIP         BIT13
#define B_PCH_HDA_DEVCAP_ATTNBP         BIT12
#define B_PCH_HDA_DEVCAP_EL1AL          0x00000E00
#define B_PCH_HDA_DEVCAP_EL0AL          0x000001C0
#define B_PCH_HDA_DEVCAP_ETFS           BIT5
#define B_PCH_HDA_DEVCAP_PFS            (BIT4 | BIT3)
#define B_PCH_HDA_DEVCAP_MPSS           0x00000007
#define R_PCH_HDA_DEVC                  0x78
#define B_PCH_HDA_DEVC_IF               BIT15
#define B_PCH_HDA_DEVC_MRRS             (BIT13 | BIT12 | BIT11)
#define B_PCH_HDA_DEVC_NSNPEN           BIT11
#define B_PCH_HDA_DEVC_APE              BIT10
#define B_PCH_HDA_DEVC_PFE              BIT9
#define B_PCH_HDA_DEVC_ETFE             BIT8
#define B_PCH_HDA_DEVC_MPS              (BIT7 | BIT6 | BIT5)
#define B_PCH_HDA_DEVC_ERO              BIT4
#define B_PCH_HDA_DEVC_URRE             BIT3
#define B_PCH_HDA_DEVC_FERE             BIT2
#define B_PCH_HDA_DEVC_NFERE            BIT1
#define B_PCH_HDA_DEVC_CERE             BIT0
#define R_PCH_HDA_DEVS                  0x7A
#define B_PCH_HDA_DEVS_TP               BIT5
#define B_PCH_HDA_DEVS_AUXPD            BIT4
#define B_PCH_HDA_DEVS_URD              BIT3
#define B_PCH_HDA_DEVS_FED              BIT2
#define B_PCH_HDA_DEVS_NFED             BIT1
#define B_PCH_HDA_DEVS_CED              BIT0
#define R_PCH_HDA_VCCAP                 0x100
#define B_PCH_HDA_VCCAP_NCO             0xFFF00000
#define B_PCH_HDA_VCCAP_CAPVER          0x000F0000
#define B_PCH_HDA_VCCAP_PCIEEC          0x0000FFFF
#define R_PCH_HDA_PVCCAP1               0x104
#define B_PCH_HDA_PVCCAP1_PATES         0x00000C00
#define B_PCH_HDA_PVCCAP1_RC            0x00000300
#define B_PCH_HDA_PVCCAP1_LPEVCC        0x00000070
#define B_PCH_HDA_PVCCAP1_EVCC          0x00000007
#define R_PCH_HDA_PVCCAP2               0x108
#define B_PCH_HDA_PVCCAP2_VCATO         0xFF000000
#define B_PCH_HDA_PVCCAP2_VCAC          0x000000FF
#define R_PCH_HDA_PVCCTL                0x10C
#define B_PCH_HDA_PVCCTL_VCAS           0x000E
#define B_PCH_HDA_PVCCTL_LVCAT          0x0001
#define R_PCH_HDA_PVCSTS                0x10E
#define B_PCH_HDA_PVCSTS_VCATS          0x0001
#define R_PCH_HDA_VC0CAP                0x110
#define S_PCH_HDA_VC0CAP                4
#define B_PCH_HDA_VC0CAP_PATO           0xFF000000
#define B_PCH_HDA_VC0CAP_MTS            0x007F0000
#define B_PCH_HDA_VC0CAP_RST            BIT15
#define B_PCH_HDA_VC0CAP_APS            BIT14
#define B_PCH_HDA_VC0CAP_PAC            0x000000FF
#define R_PCH_HDA_VC0CTL                0x114
#define S_PCH_HDA_VC0CTL                4
#define B_PCH_HDA_VC0CTL_VC0EN          BIT31
#define B_PCH_HDA_VC0CTL_VC0ID          0x07000000
#define B_PCH_HDA_VC0CTL_PAS            0x000E0000
#define B_PCH_HDA_VC0CTL_LPAT           BIT16
#define B_PCH_HDA_VC0CTL_TCVC0_MAP      0x000000FE
#define R_PCH_HDA_VC0STS                0x11A
#define S_PCH_HDA_VC0STS                2
#define B_PCH_HDA_VC0STS_VC0NP          BIT1
#define B_PCH_HDA_VC0STS_PATS           BIT0
#define R_PCH_HDA_VCICAP                0x11C
#define S_PCH_HDA_VCICAP                4
#define B_PCH_HDA_VCICAP_PATO           0xFF000000
#define B_PCH_HDA_VCICAP_MTS            0x007F0000
#define B_PCH_HDA_VCICAP_RST            BIT15
#define B_PCH_HDA_VCICAP_APS            BIT14
#define B_PCH_HDA_VCICAP_PAC            0x000000FF
#define R_PCH_HDA_VCICTL                0x120
#define S_PCH_HDA_VCICTL                4
#define B_PCH_HDA_VCICTL_EN             BIT31
#define B_PCH_HDA_VCICTL_ID             (BIT26 | BIT25 | BIT24)
#define V_PCH_HDA_VCICTL_PAS            0x000E0000
#define V_PCH_HDA_VCICTL_LPAT           BIT16
#define B_PCH_HDA_VCICTL_TCVCI_MAP      0x000000FE
#define R_PCH_HDA_VCISTS                0x126
#define S_PCH_HDA_VCISTS                1
#define B_PCH_HDA_VCISTS_VCINP          BIT1
#define B_PCH_HDA_VCISTS_PATS           BIT0
#define R_PCH_HDA_RCCAP                 0x130
#define B_PCH_HDA_RCCAP_NCO             0xFFF00000
#define B_PCH_HDA_RCCAP_CV              0x000F0000
#define B_PCH_HDA_RCCAP_PCIEECID        0x0000FFFF
#define R_PCH_HDA_ESD                   0x134
#define B_PCH_HDA_ESD_PN                0xFF000000
#define B_PCH_HDA_ESD_CID               0x00FF0000
#define B_PCH_HDA_ESD_NOLE              0x0000FF00
#define B_PCH_HDA_ESD_ELTYP             0x0000000F
#define R_PCH_HDA_L1DESC                0x140
#define S_PCH_HDA_L1DESC                4
#define B_PCH_HDA_LIDESC_TPN            0xFF000000
#define B_PCH_HDA_LIDESC_TCID           0x00FF0000
#define B_PCH_HDA_LIDESC_LT             BIT1
#define B_PCH_HDA_LIDESC_LV             BIT0
#define R_PCH_HDA_L1ADDL                0x148
#define B_PCH_HDA_L1ADDL_LNK1LA         0xFFFFC000
#define R_PCH_HDA_L1ADDU                0x14C
#define B_PCH_HDA_L1ADDU                0xFFFFFFFF
//
// Intel High Definition Audio Memory Mapped Configuration Registers
//
#define R_HDA_GCAP                      0x00
#define S_HDA_GCAP                      2
#define B_HDA_GCAP_NOSSUP               0xF000
#define B_HDA_GCAP_NISSUP               0x0F00
#define B_HDA_GCAP_NBSSUP               0x00F8
#define B_HDA_GCAP_NSDOS                BIT1
#define B_HDA_GCAP_64ADSUP              BIT0
#define R_HDA_VMIN                      0x02
#define B_HDA_VMIN_MV                   0xFF
#define R_HDA_VMAJ                      0x03
#define B_HDA_VMAJ_MV                   0xFF
#define R_HDA_OUTPAY                    0x04
#define B_HDA_OUTPAY_CAP                0x007F
#define R_HDA_INPAY                     0x06
#define B_HDA_INPAY_CAP                 0x007F
#define R_HDA_GCTL                      0x08
#define B_HDA_GCTL_AURE                 BIT8
#define B_HDA_GCTL_FC                   BIT1
#define B_HDA_GCTL_CRST                 BIT0
#define R_HDA_WAKEEN                    0x0C
#define B_HDA_WAKEEN_SDI_3              BIT3
#define B_HDA_WAKEEN_SDI_2              BIT2
#define B_HDA_WAKEEN_SDI_1              BIT1
#define B_HDA_WAKEEN_SDI_0              BIT0
#define R_HDA_STATESTS                  0x0E
#define B_HDA_STATESTS_SDIN3            BIT3
#define B_HDA_STATESTS_SDIN2            BIT2
#define B_HDA_STATESTS_SDIN1            BIT1
#define B_HDA_STATESTS_SDIN0            BIT0
#define R_HDA_GSTS                      0x10
#define B_HDA_GSTS_FS                   BIT1
#define R_HDA_OUTSTRMPAY                0x18
#define S_HDA_OUTSTRMPAY                2
#define B_HDA_OUTSTRMPAY_OUTSTRMPAY     0xFFFF
#define R_HDA_INSTRMPAY                 0x1A
#define B_HDA_INSTRMPAY_INSTRMPAY       0xFFFF
#define R_HDA_INTCTL                    0x20
#define B_HDA_INTCTL_GIE                BIT31
#define B_HDA_INTCTL_CIE                BIT30
#define B_HDA_INTCTL_SIE_OS4            BIT7
#define B_HDA_INTCTL_SIE_OS3            BIT6
#define B_HDA_INTCTL_SIE_OS2            BIT5
#define B_HDA_INTCTL_SIE_OS1            BIT4
#define B_HDA_INTCTL_SIE_IS4            BIT3
#define B_HDA_INTCTL_SIE_IS3            BIT2
#define B_HDA_INTCTL_SIE_IS2            BIT1
#define B_HDA_INTCTL_SIE_IS1            BIT0
#define R_HDA_INTSTS                    0x24
#define B_HDA_INTSTS_GIS                BIT31
#define B_HDA_INTSTS_CIS                BIT30
#define B_HDA_INTSTS_SIS_OS4            BIT7
#define B_HDA_INTSTS_SIS_OS3            BIT6
#define B_HDA_INTSTS_SIS_OS2            BIT5
#define B_HDA_INTSTS_SIS_OS1            BIT4
#define B_HDA_INTSTS_SIS_IS4            BIT3
#define B_HDA_INTSTS_SIS_IS3            BIT2
#define B_HDA_INTSTS_SIS_IS2            BIT1
#define B_HDA_INTSTS_SIS_IS1            BIT0
#define R_HDA_WALCLK                    0x30
#define B_HDA_WALCLK_WCC                0xFFFFFFFF
#define R_HDA_SSYNC                     0x38
#define S_HDA_SSYNC                     4
#define B_HDA_SSYNC_OS4                 BIT7
#define B_HDA_SSYNC_OS3                 BIT6
#define B_HDA_SSYNC_OS2                 BIT5
#define B_HDA_SSYNC_OS1                 BIT4
#define B_HDA_SSYNC_IS4                 BIT3
#define B_HDA_SSYNC_IS3                 BIT2
#define B_HDA_SSYNC_IS2                 BIT1
#define B_HDA_SSYNC_IS1                 BIT0
#define R_HDA_CORBLBASE                 0x40
#define B_HDA_CORBLBASE_BA              0xFFFFFF80
#define B_HDA_CORBLBASE_UB              0x0000007F
#define R_HDA_CORBUBASE                 0x44
#define B_HDA_CORBUBASE_BA              0xFFFFFFFF
#define R_HDA_CORBWP                    0x48
#define B_HDA_CORBWP                    0x000000FF
#define R_HDA_CORBRP                    0x4A
#define B_HDA_CORBRP_PRST               BIT15
#define B_HDA_CORBRP_RP                 0x00FF
#define R_HDA_CORBCTL                   0x4C
#define B_HDA_CORBCTL_DMA_EN            BIT1
#define B_HDA_CORBCTL_MEMERRINTR_EN     BIT0
#define R_HDA_CORBST                    0x4D
#define B_HDA_CORBST_CMEI               BIT0
#define R_HDA_CORBSIZE                  0x4E
#define B_HDA_CORBSIZE_CAP              0xF0
#define B_HDA_CORBSIZE_SIZE             0x03
#define R_HDA_RIRBLBASE                 0x50
#define B_HDA_RIRBLBASE_BA              0xFFFFFF80
#define B_HDA_RIRBLBASE_UB              0x0000007F
#define R_HDA_RIRBUBASE                 0x54
#define B_HDA_RIRBUBASE_BA              0xFFFFFFFF
#define R_HDA_RIRBWP                    0x58
#define B_HDA_RIRBWP_RST                BIT15
#define B_HDA_RIRBWP_WP                 0x00FF
#define R_HDA_RINTCNT                   0x5A
#define B_HDA_RINTCNT                   0x00FF
#define R_HDA_RIRBCTL                   0x5C
#define B_HDA_RIRBCTL_ROIC              BIT2
#define B_HDA_RIRBCTL_DMA               BIT1
#define B_HDA_RIRBCTL_RIC               BIT0
#define R_HDA_RIRBSTS                   0x5D
#define B_HDA_RIRBSTS_ROIS              BIT2
#define B_HDA_RIRBSTS_RI                BIT0
#define R_HDA_RIRBSIZE                  0x5E
#define B_HDA_RIRBSIZE_CAP              0xF0
#define B_HDA_RIRBSIZE_SIZE             0x03
#define R_HDA_IC                        0x60
#define B_HDA_IC                        0xFFFFFFFF
#define R_HDA_IR                        0x64
#define B_HDA_IR                        0xFFFFFFFF
#define R_HDA_IRS                       0x68
#define B_HDA_IRS_IRV                   BIT1
#define B_HDA_IRS_ICB                   BIT0
#define R_HDA_DPLBASE                   0x70
#define B_HDA_DPLBASE_LBA               0xFFFFFF80
#define B_HDA_DPLBASE_LBU               0x0000007E
#define B_HDA_DPLBASE_BUF_EN            0x00000001
#define R_HDA_DPUBASE                   0x74
#define B_HDA_DPUBASE_UBA               0xFFFFFFFF

#endif
