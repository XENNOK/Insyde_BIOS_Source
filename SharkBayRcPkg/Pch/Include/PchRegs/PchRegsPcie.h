//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

  Register names for PCH PCI-E root port devices

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
  Copyright (c) 1999 - 2014 Intel Corporation. All rights reserved
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
#ifndef _PCH_REGS_PCIE_H_
#define _PCH_REGS_PCIE_H_

#define LPTH_PCIE_MAX_ROOT_PORTS  8
#define LPTLP_PCIE_MAX_ROOT_PORTS 6

//
// PCH PCI Express Root Ports (D28:F0~5)
//
#define PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS     28
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1  0
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2  1
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3  2
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4  3
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5  4
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6  5
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7  6
#define PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8  7
#define R_PCH_PCIE_VENDOR_ID                      0x00
#define V_PCH_PCIE_VENDOR_ID                      V_PCH_INTEL_VENDOR_ID
#define R_PCH_PCIE_DEVICE_ID                      0x02

#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT1            0x8C10  ///< PCI Express Root Port #1, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT2            0x8C12  ///< PCI Express Root Port #2, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT3            0x8C14  ///< PCI Express Root Port #3, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT4            0x8C16  ///< PCI Express Root Port #4, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT5            0x8C18  ///< PCI Express Root Port #5, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT6            0x8C1A  ///< PCI Express Root Port #6, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT7            0x8C1C  ///< PCI Express Root Port #7, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_PORT8            0x8C1E  ///< PCI Express Root Port #8, LPT
#define V_PCH_LPTH_PCIE_DEVICE_ID_MB_SUBD          0x2448  ///< Mobile with subtractive decode enable
#define V_PCH_LPTH_PCIE_DEVICE_ID_DT_SUBD          0x244E  ///< Desktop with subtractive decode enable

#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT1            0x9C10  ///< PCI Express Root Port #1, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT1_ALT        0x9C11  ///< PCI Express Root Port #1, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT2            0x9C12  ///< PCI Express Root Port #2, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT2_ALT        0x9C13  ///< PCI Express Root Port #2, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT3            0x9C14  ///< PCI Express Root Port #3, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT3_ALT        0x9C15  ///< PCI Express Root Port #3, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT4            0x9C16  ///< PCI Express Root Port #4, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT4_ALT        0x9C17  ///< PCI Express Root Port #4, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT5            0x9C18  ///< PCI Express Root Port #5, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT5_ALT        0x9C19  ///< PCI Express Root Port #5, LPTLP PCIe Device ID bit[0] fuse = 1
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT6            0x9C1A  ///< PCI Express Root Port #6, LPTLP PCIe Device ID bit[0] fuse = 0
#define V_PCH_LPTLP_PCIE_DEVICE_ID_PORT6_ALT        0x9C1B  ///< PCI Express Root Port #6, LPTLP PCIe Device ID bit[0] fuse = 1

#define R_PCH_PCIE_PCICMD                         0x04
#define S_PCH_PCIE_PCICMD                         2
#define B_PCH_PCIE_PCICMD_ID                      BIT10
#define B_PCH_PCIE_PCICMD_FBE                     BIT9
#define B_PCH_PCIE_PCICMD_SEE                     BIT8
#define B_PCH_PCIE_PCICMD_WCC                     BIT7
#define B_PCH_PCIE_PCICMD_PER                     BIT6
#define B_PCH_PCIE_PCICMD_VPS                     BIT5
#define B_PCH_PCIE_PCICMD_PMWE                    BIT4
#define B_PCH_PCIE_PCICMD_SCE                     BIT3
#define B_PCH_PCIE_PCICMD_BME                     BIT2
#define B_PCH_PCIE_PCICMD_MSE                     BIT1
#define B_PCH_PCIE_PCICMD_IOSE                    BIT0
#define R_PCH_PCIE_PCISTS                         0x06
#define S_PCH_PCIE_PCISTS                         2
#define B_PCH_PCIE_PCISTS_DPE                     BIT15
#define B_PCH_PCIE_PCISTS_SSE                     BIT14
#define B_PCH_PCIE_PCISTS_RMA                     BIT13
#define B_PCH_PCIE_PCISTS_RTA                     BIT12
#define B_PCH_PCIE_PCISTS_STA                     BIT11
#define B_PCH_PCIE_PCISTS_DEV_STS                 (BIT10 | BIT9)
#define B_PCH_PCIE_PCISTS_DPED                    BIT8
#define B_PCH_PCIE_PCISTS_FB2BC                   BIT7
#define B_PCH_PCIE_PCISTS_66MHZ_CAP               BIT5
#define B_PCH_PCIE_PCISTS_CAP_LST                 BIT4
#define B_PCH_PCIE_PCISTS_INTR_STS                BIT3
#define R_PCH_PCIE_RID                            0x08
#define B_PCH_PCIE_RID                            0xFF
#define R_PCH_PCIE_PI                             0x09
#define B_PCH_PCIE_PI                             0xFF
#define R_PCH_PCIE_SCC                            0x0A
#define B_PCH_PCIE_SCC                            0xFF
#define V_PCH_PCIE_SCC_04                         0x04
#define V_PCH_PCIE_SCC_00                         0x00
#define R_PCH_PCIE_BCC                            0x0B
#define B_PCH_PCIE_BCC                            0xFF
#define R_PCH_PCIE_CLS                            0x0C
#define B_PCH_PCIE_CLS                            0xFF
#define R_PCH_PCIE_PLT                            0x0D
#define B_PCH_PCIE_PLT_LC                         0xF8
#define R_PCH_PCIE_HEADTYPE                       0x0E
#define B_PCH_PCIE_HEADTYPE_MFD                   BIT7
#define B_PCH_PCIE_HEADTYPE_CL                    0x7F
#define V_PCH_PCIE_HEADTYPE_CL_01                 0x01
#define V_PCH_PCIE_HEADTYPE_CL_00                 0x00
#define R_PCH_PCIE_BNUM                           0x18
#define R_PCH_PCIE_SCBN                           0x19
#define R_PCH_PCIE_SBBN                           0x1A
#define B_PCH_PCIE_BNUM_SBBN                      0x00FF0000
#define B_PCH_PCIE_BNUM_SCBN                      0x0000FF00
#define B_PCH_PCIE_BNUM_PBN                       0x000000FF
#define R_PCH_PCIE_SLT                            0x1B
#define B_PCH_PCIE_SLT                            0xFF
#define R_PCH_PCIE_IOBL                           0x1C
#define B_PCH_PCIE_IOBL_IOLA                      0xF000
#define B_PCH_PCIE_IOBL_IOLC                      0x0F00
#define B_PCH_PCIE_IOBL_IOBA                      0x00F0
#define B_PCH_PCIE_IOBL_IOBC                      0x000F
#define R_PCH_PCIE_SSTS                           0x1E
#define S_PCH_PCIE_SSTS                           2
#define B_PCH_PCIE_SSTS_DPE                       BIT15
#define B_PCH_PCIE_SSTS_RSE                       BIT14
#define B_PCH_PCIE_SSTS_RMA                       BIT13
#define B_PCH_PCIE_SSTS_RTA                       BIT12
#define B_PCH_PCIE_SSTS_STA                       BIT11
#define B_PCH_PCIE_SSTS_SDTS                      (BIT10 | BIT9)
#define B_PCH_PCIE_SSTS_DPD                       BIT8
#define B_PCH_PCIE_SSTS_SFBC                      BIT7
#define B_PCH_PCIE_SSTS_SC66                      BIT5
#define R_PCH_PCIE_MBL                            0x20
#define B_PCH_PCIE_MBL_ML                         0xFFF00000
#define B_PCH_PCIE_MBL_MB                         0x0000FFF0
#define R_PCH_PCIE_PMBL                           0x24
#define B_PCH_PCIE_PMBL_PML                       0xFFF00000
#define B_PCH_PCIE_PMBL_I64L                      0x000F0000
#define B_PCH_PCIE_PMBL_PMB                       0x0000FFF0
#define B_PCH_PCIE_PMBL_I64B                      0x0000000F
#define R_PCH_PCIE_PMBU32                         0x28
#define B_PCH_PCIE_PMBU32                         0xFFFFFFFF
#define R_PCH_PCIE_PMLU32                         0x2C
#define B_PCH_PCIE_PMLU32                         0xFFFFFFFF
#define R_PCH_PCIE_CAPP                           0x34
#define B_PCH_PCIE_CAPP                           0xFF
#define R_PCH_PCIE_INTR                           0x3C
#define B_PCH_PCIE_INTR_IPIN                      0xFF00
#define B_PCH_PCIE_INTR_ILINE                     0x00FF
#define R_PCH_PCIE_BCTRL                          0x3E
#define S_PCH_PCIE_BCTRL                          2
#define B_PCH_PCIE_BCTRL_DTSE                     BIT11
#define B_PCH_PCIE_BCTRL_DTS                      BIT10
#define B_PCH_PCIE_BCTRL_SDT                      BIT9
#define B_PCH_PCIE_BCTRL_PDT                      BIT8
#define B_PCH_PCIE_BCTRL_FBE                      BIT7
#define B_PCH_PCIE_BCTRL_SBR                      BIT6
#define B_PCH_PCIE_BCTRL_MAM                      BIT5
#define B_PCH_PCIE_BCTRL_V16                      BIT4
#define B_PCH_PCIE_BCTRL_VE                       BIT3
#define B_PCH_PCIE_BCTRL_IE                       BIT2
#define B_PCH_PCIE_BCTRL_SE                       BIT1
#define B_PCH_PCIE_BCTRL_PERE                     BIT0
#define R_PCH_PCIE_CLIST                          0x40
#define B_PCH_PCIE_CLIST_NEXT                     0xFF00
#define B_PCH_PCIE_CLIST_CID                      0x00FF
#define R_PCH_PCIE_XCAP                           0x42
#define S_PCH_PCIE_XCAP                           2
#define B_PCH_PCIE_XCAP_IMN                       0x3E00
#define B_PCH_PCIE_XCAP_SI                        BIT8
#define B_PCH_PCIE_XCAP_DT                        0x00F0
#define B_PCH_PCIE_XCAP_CV                        0x000F
#define R_PCH_PCIE_DCAP                           0x44
#define S_PCH_PCIE_DCAP                           4
#define B_PCH_PCIE_DCAP_CSPS                      0x0C000000
#define B_PCH_PCIE_DCAP_CSPV                      0x03FC0000
#define B_PCH_PCIE_DCAP_RBER                      BIT15
#define B_PCH_PCIE_DCAP_PIP                       BIT14
#define B_PCH_PCIE_DCAP_AIP                       BIT13
#define B_PCH_PCIE_DCAP_ABP                       BIT12
#define B_PCH_PCIE_DCAP_E1AL                      0x00000E00
#define B_PCH_PCIE_DCAP_E0AL                      0x000001C0
#define B_PCH_PCIE_DCAP_ETFS                      BIT5
#define B_PCH_PCIE_DCAP_PFS                       0x00000018
#define B_PCH_PCIE_DCAP_MPS                       0x00000007
#define R_PCH_PCIE_DCTL                           0x48
#define S_PCH_PCIE_DCTL                           2
#define B_PCH_PCIE_DCTL_MRRS                      0x7000
#define B_PCH_PCIE_DCTL_ENS                       BIT11
#define B_PCH_PCIE_DCTL_APME                      BIT10
#define B_PCH_PCIE_DCTL_PFE                       BIT9
#define B_PCH_PCIE_DCTL_ETFE                      BIT8
#define B_PCH_PCIE_DCTL_MPS                       (BIT7 | BIT6 | BIT5)
#define B_PCH_PCIE_DCTL_ERO                       BIT4
#define B_PCH_PCIE_DCTL_URE                       BIT3
#define B_PCH_PCIE_DCTL_FEE                       BIT2
#define B_PCH_PCIE_DCTL_NFE                       BIT1
#define B_PCH_PCIE_DCTL_CEE                       BIT0
#define R_PCH_PCIE_DSTS                           0x4A
#define B_PCH_PCIE_DSTS_TDP                       BIT5
#define B_PCH_PCIE_DSTS_APD                       BIT4
#define B_PCH_PCIE_DSTS_URD                       BIT3
#define B_PCH_PCIE_DSTS_FED                       BIT2
#define B_PCH_PCIE_DSTS_NFED                      BIT1
#define B_PCH_PCIE_DSTS_CED                       BIT0
#define R_PCH_PCIE_LCAP                           0x4C
#define B_PCH_PCIE_LCAP_PN                        0xFF000000
#define V_PCH_PCIE_LCAP_PN1                       (1 << 24)
#define V_PCH_PCIE_LCAP_PN2                       (2 << 24)
#define V_PCH_PCIE_LCAP_PN3                       (3 << 24)
#define V_PCH_PCIE_LCAP_PN4                       (4 << 24)
#define V_PCH_PCIE_LCAP_PN5                       (5 << 24)
#define V_PCH_PCIE_LCAP_PN6                       (6 << 24)
#define V_PCH_PCIE_LCAP_PN7                       (7 << 24)
#define V_PCH_PCIE_LCAP_PN8                       (8 << 24)
#define B_PCH_PCIE_LCAP_LARC                      BIT20
#define B_PCH_PCIE_LCAP_EL1                       (BIT17 | BIT16 | BIT15)
#define B_PCH_PCIE_LCAP_EL0                       (BIT14 | BIT13 | BIT12)
#define B_PCH_PCIE_LCAP_APMS                      (BIT11 | BIT10)
#define V_PCH_PCIE_LCAP_APMS_L0S                  (1 << 10)
#define V_PCH_PCIE_LCAP_APMS_L0S_L1               (3 << 10)
#define B_PCH_PCIE_LCAP_MLW                       0x000003F0
#define B_PCH_PCIE_LCAP_MLS                       0x0000000F
#define R_PCH_PCIE_LCTL                           0x50
#define B_PCH_PCIE_LCTL_HAWD                      BIT9
#define B_PCH_PCIE_LCTL_ES                        BIT7
#define B_PCH_PCIE_LCTL_CCC                       BIT6
#define B_PCH_PCIE_LCTL_RL                        BIT5
#define B_PCH_PCIE_LCTL_LD                        BIT4
#define B_PCH_PCIE_LCTL_RCBC                      BIT3
#define B_PCH_PCIE_LCTL_APMC                      (BIT1 | BIT0)
#define V_PCH_PCIE_LCTL_APMC_L0S                  1
#define V_PCH_PCIE_LCTL_APMC_L1                   2
#define V_PCH_PCIE_LCTL_APMC_L0S_L1               3
#define R_PCH_PCIE_LSTS                           0x52
#define S_PCH_PCIE_LSTS                           2
#define B_PCH_PCIE_LSTS_DLLA                      BIT13
#define B_PCH_PCIE_LSTS_SCC                       BIT12
#define B_PCH_PCIE_LSTS_LT                        BIT11
#define B_PCH_PCIE_LSTS_LTE                       BIT10
#define B_PCH_PCIE_LSTS_NLW                       0x03F0
#define V_PCH_PCIE_LSTS_NLW_1                     0x0010
#define V_PCH_PCIE_LSTS_NLW_2                     0x0020
#define V_PCH_PCIE_LSTS_NLW_4                     0x0040
#define B_PCH_PCIE_LSTS_LS                        0x000F
#define R_PCH_PCIE_SLCAP                          0x54
#define S_PCH_PCIE_SLCAP                          4
#define B_PCH_PCIE_SLCAP_PSN                      0xFFF80000
#define B_PCH_PCIE_SLCAP_SLS                      0x00018000
#define B_PCH_PCIE_SLCAP_SLV                      0x00007F80
#define B_PCH_PCIE_SLCAP_HPC                      BIT6
#define B_PCH_PCIE_SLCAP_HPS                      BIT5
#define B_PCH_PCIE_SLCAP_PIP                      BIT4
#define B_PCH_PCIE_SLCAP_AIP                      BIT3
#define B_PCH_PCIE_SLCAP_MSP                      BIT2
#define B_PCH_PCIE_SLCAP_PCP                      BIT1
#define B_PCH_PCIE_SLCAP_ABP                      BIT0
#define R_PCH_PCIE_SLCTL                          0x58
#define S_PCH_PCIE_SLCTL                          2
#define B_PCH_PCIE_SLCTL_LACE                     BIT12
#define B_PCH_PCIE_SLCTL_PCC                      BIT10
#define B_PCH_PCIE_SLCTL_HPE                      BIT5
#define B_PCH_PCIE_SLCTL_PDE                      BIT3
#define R_PCH_PCIE_SLSTS                          0x5A
#define S_PCH_PCIE_SLSTS                          2
#define B_PCH_PCIE_SLSTS_LASC                     BIT8
#define B_PCH_PCIE_SLSTS_PDS                      BIT6
#define B_PCH_PCIE_SLSTS_MS                       BIT5
#define B_PCH_PCIE_SLSTS_PDC                      BIT3
#define B_PCH_PCIE_SLSTS_MSC                      BIT2
#define B_PCH_PCIE_SLSTS_PFD                      BIT1
#define R_PCH_PCIE_RCTL                           0x5C
#define S_PCH_PCIE_RCTL                           2
#define B_PCH_PCIE_RCTL_PIE                       BIT3
#define B_PCH_PCIE_RCTL_SFE                       BIT2
#define B_PCH_PCIE_RCTL_SNE                       BIT1
#define B_PCH_PCIE_RCTL_SCE                       BIT0
#define R_PCH_PCIE_RSTS                           0x60
#define S_PCH_PCIE_RSTS                           4
#define B_PCH_PCIE_RSTS_PP                        BIT17
#define B_PCH_PCIE_RSTS_PS                        BIT16
#define B_PCH_PCIE_RSTS_RID                       0x0000FFFF
#define R_PCH_PCIE_DCAP2                          0x64
#define B_PCH_PCIE_DCAP2_CTDS                     BIT4
#define B_PCH_PCIE_DCAP2_CTRS                     0xF
#define V_PCH_PCIE_DCAP2_CTRS_UNSUPPORTED         0x0
#define V_PCH_PCIE_DCAP2_CTRS_RANGE_A             0x1
#define V_PCH_PCIE_DCAP2_CTRS_RANGE_B             0x2
#define V_PCH_PCIE_DCAP2_CTRS_RANGE_C             0x4
#define V_PCH_PCIE_DCAP2_CTRS_RANGE_D             0x8
#define R_PCH_PCIE_DCTL2                          0x68
#define B_PCH_PCIE_DCTL2_CTD                      BIT4
#define B_PCH_PCIE_DCTL2_CTV                      0xF
#define V_PCH_PCIE_DCTL2_CTV_DEFAULT              0x0
#define V_PCH_PCIE_DCTL2_CTV_40MS_50MS            0x5
#define V_PCH_PCIE_DCTL2_CTV_160MS_170MS          0x6
#define V_PCH_PCIE_DCTL2_CTV_400MS_500MS          0x9
#define V_PCH_PCIE_DCTL2_CTV_1P6S_1P7S            0xA
#define R_PCH_PCIE_LCTL2                          0x70
#define B_PCH_PCIE_LCTL2_TLS                      (BIT3 | BIT2 | BIT1 | BIT0)
#define R_PCH_PCIE_MID                            0x80
#define S_PCH_PCIE_MID                            2
#define B_PCH_PCIE_MID_NEXT                       0xFF00
#define B_PCH_PCIE_MID_CID                        0x00FF
#define R_PCH_PCIE_MC                             0x82
#define S_PCH_PCIE_MC                             2
#define B_PCH_PCIE_MC_C64                         BIT7
#define B_PCH_PCIE_MC_MME                         (BIT6 | BIT5 | BIT4)
#define B_PCH_PCIE_MC_MMC                         0x000E
#define B_PCH_PCIE_MC_MSIE                        BIT0
#define R_PCH_PCIE_MA                             0x84
#define S_PCH_PCIE_MA                             4
#define B_PCH_PCIE_MA_ADDR                        0xFFFFFFFC
#define R_PCH_PCIE_MD                             0x88
#define S_PCH_PCIE_MD                             2
#define B_PCH_PCIE_MD_DATA                        0xFFFF
#define R_PCH_PCIE_SVCAP                          0x90
#define S_PCH_PCIE_SVCAP                          2
#define B_PCH_PCIE_SVCAP_NEXT                     0xFF00
#define B_PCH_PCIE_SVCAP_CID                      0x00FF
#define R_PCH_PCIE_SVID                           0x94
#define S_PCH_PCIE_SVID                           4
#define B_PCH_PCIE_SVID_SID                       0xFFFF0000
#define B_PCH_PCIE_SVID_SVID                      0x0000FFFF
#define R_PCH_PCIE_PMCAP                          0xA0
#define S_PCH_PCIE_PMCAP                          2
#define B_PCH_PCIE_PMCAP_NEXT                     0xFF00
#define B_PCH_PCIE_PMCAP_CID                      0x00FF
#define R_PCH_PCIE_PMC                            0xA2
#define S_PCH_PCIE_PMC                            2
#define B_PCH_PCIE_PMC_PMES                       0xF800
#define B_PCH_PCIE_PMC_D2S                        BIT10
#define B_PCH_PCIE_PMC_D1S                        BIT9
#define B_PCH_PCIE_PMC_AC                         0x01C0
#define B_PCH_PCIE_PMC_DSI                        BIT5
#define B_PCH_PCIE_PMC_PMEC                       BIT3
#define B_PCH_PCIE_PMC_VS                         0x0007
#define R_PCH_PCIE_PMCS                           0xA4
#define S_PCH_PCIE_PMCS                           4
#define B_PCH_PCIE_PMCS_BPCE                      BIT23
#define B_PCH_PCIE_PMCS_B23S                      BIT22
#define B_PCH_PCIE_PMCS_PMES                      BIT15
#define B_PCH_PCIE_PMCS_PMEE                      BIT8
#define B_PCH_PCIE_PMCS_PS                        (BIT1 | BIT0)
#define V_PCH_PCIE_PMCS_D0                        0x00
#define V_PCH_PCIE_PMCS_D3H                       0x03
#define R_PCH_PCIE_CCFG                           0xD0
#define B_PCH_PCIE_CCFG_DCGEISMA                  BIT17
#define R_PCH_PCIE_MPC2                           0xD4
#define S_PCH_PCIE_MPC2                           4
#define B_PCH_PCIE_MPC2_PCME                      BIT5
#define B_PCH_PCIE_MPC2_ASPMCOEN                  BIT4
#define B_PCH_PCIE_MPC2_ASPMCO                    (BIT3 | BIT2)
#define V_PCH_PCIE_MPC2_ASPMCO_DISABLED           0
#define V_PCH_PCIE_MPC2_ASPMCO_L0S                1 << 2
#define V_PCH_PCIE_MPC2_ASPMCO_L1                 2 << 2
#define V_PCH_PCIE_MPC2_ASPMCO_L0S_L1             3 << 2
#define B_PCH_PCIE_MPC2_EOIFD                     BIT1
#define B_PCH_PCIE_MPC2_L1CTM                     BIT0
#define R_PCH_PCIE_MPC                            0xD8
#define S_PCH_PCIE_MPC                            4
#define B_PCH_PCIE_MPC_PMCE                       BIT31
#define B_PCH_PCIE_MPC_HPCE                       BIT30
#define B_PCH_PCIE_MPC_LHO                        BIT29
#define B_PCH_PCIE_MPC_ATE                        BIT28
#define B_PCH_PCIE_MPC_MMBNCE                     BIT27
#define B_PCH_PCIE_MPC_IRBNCE                     BIT26
#define B_PCH_PCIE_MPC_IRRCE                      BIT25
#define B_PCH_PCIE_MPC_BMERCE                     BIT24
#define B_PCH_PCIE_MPC_FORCEDET                   BIT22
#define B_PCH_PCIE_MPC_FCDL1E                     BIT21
#define B_PCH_PCIE_MPC_UCEL                       (BIT20 | BIT19 | BIT18)
#define B_PCH_PCIE_MPC_CCEL                       (BIT17 | BIT16 | BIT15)
#define B_PCH_PCIE_MPC_PAE                        BIT7
#define B_PCH_PCIE_MPC_MCTPSE                     BIT3
#define B_PCH_PCIE_MPC_BT                         BIT2
#define B_PCH_PCIE_MPC_HPME                       BIT1
#define N_PCH_PCIE_MPC_HPME                       1
#define B_PCH_PCIE_MPC_PMME                       BIT0
#define R_PCH_PCIE_SMSCS                          0xDC
#define S_PCH_PCIE_SMSCS                          4
#define B_PCH_PCIE_SMSCS_PMCS                     BIT31
#define B_PCH_PCIE_SMSCS_HPCS                     BIT30
#define B_PCH_PCIE_SMSCS_HPLAS                    BIT4
#define N_PCH_PCIE_SMSCS_HPLAS                    4
#define B_PCH_PCIE_SMSCS_HPCCM                    BIT3
#define B_PCH_PCIE_SMSCS_HPABM                    BIT2
#define B_PCH_PCIE_SMSCS_HPPDM                    BIT1
#define N_PCH_PCIE_SMSCS_HPPDM                    1
#define B_PCH_PCIE_SMSCS_PMMS                     BIT0
#define R_PCH_PCIE_RPDCGEN                        0xE1
#define S_PCH_PCIE_RPDCGEN                        1
#define B_PCH_PCIE_RPDCGEN_RPSCGEN                BIT7
#define B_PCH_PCIE_RPDCGEN_POCGE                  BIT6
#define B_PCH_PCIE_RPDCGEN_LCLKREQEN              BIT5
#define B_PCH_PCIE_RPDCGEN_BBCLKREQEN             BIT4
#define B_PCH_PCIE_RPDCGEN_SRDLCGEN               BIT3
#define B_PCH_PCIE_RPDCGEN_SRDBCGEN               BIT2
#define B_PCH_PCIE_RPDCGEN_RPDLCGEN               BIT1
#define B_PCH_PCIE_RPDCGEN_RPDBCGEN               BIT0
#define R_PCH_PCIE_RPPGEN                         0xE2
#define B_PCH_PCIE_RPPGEN_PTOTOP                  BIT6
#define B_PCH_PCIE_RPPGEN_LMSDOCGE                BIT5
#define B_PCH_PCIE_RPPGEN_SEOCGE                  BIT4
#define R_PCH_PCIE_PECR1                          0xE8
#define S_PCH_PCIE_PECR1                          4
#define B_PCH_PCIE_PECR1_FIELD_2                  BIT1
#define V_PCH_PCIE_PECR1_FIELD_3                  (BIT3 | BIT2)
#define R_PCH_PCIE_PECR3                          0xEC
#define B_PCH_PCIE_PECR3_SDCDID                   BIT1                          ///< Subtractive Decode Compatibility Device ID
#define B_PCH_PCIE_PECR3_SDE                      BIT0                          ///< Subtractive Decode Enable
#define R_PCH_PCIE_STRPFUSECFG                    0xFC
#define B_PCH_PCIE_STRPFUSECFG_SATAP3_PCIEP6L0_MODE (BIT23 | BIT22)
#define B_PCH_PCIE_STRPFUSECFG_SATAP2_PCIEP6L1_MODE (BIT21 | BIT20)
#define B_PCH_PCIE_STRPFUSECFG_SATAP5_PCIEP2_MODE (BIT23 | BIT22)
#define B_PCH_PCIE_STRPFUSECFG_SATAP4_PCIEP1_MODE (BIT21 | BIT20)
#define B_PCH_PCIE_STRPFUSECFG_GBE_PCIE_PEN       (BIT19)
#define B_PCH_PCIE_STRPFUSECFG_GBE_PCIEPORTSEL    (BIT18 | BIT17 | BIT16)
#define N_PCH_PCIE_STRPFUSECFG_GBE_PCIEPORTSEL    16
#define B_PCH_PCIE_STRPFUSECFG_RPC                (BIT15 | BIT14)
#define V_PCH_PCIE_STRPFUSECFG_RPC_1_1_1_1        (0)
#define V_PCH_PCIE_STRPFUSECFG_RPC_2_1_1          (BIT14)
#define V_PCH_PCIE_STRPFUSECFG_RPC_2_2            (BIT15)
#define V_PCH_PCIE_STRPFUSECFG_RPC_4              (BIT15 | BIT14)
#define N_PCH_PCIE_STRPFUSECFG_RPC_4              14
#define B_PCH_PCIE_STRPFUSECFG_SATAP3_PCIEP6L0_MODE_FUSE (BIT13 | BIT12)
#define B_PCH_PCIE_STRPFUSECFG_SATAP2_PCIEP6L1_MODE_FUSE (BIT11 | BIT10)
#define B_PCH_PCIE_STRPFUSECFG_SATAP5_PCIEP2_MODE_FUSE (BIT13 | BIT12)
#define B_PCH_PCIE_STRPFUSECFG_SATAP4_PCIEP1_MODE_FUSE (BIT11 | BIT10)
#define B_PCH_PCIE_STRPFUSECFG_mPHYIOPMDIS        (BIT9)
#define B_PCH_PCIE_STRPFUSECFG_PLLSHTDWNDIS       (BIT8)
#define B_PCH_PCIE_STRPFUSECFG_STPGATEDIS         (BIT7)
#define B_PCH_PCIE_STRPFUSECFG_ASPMDIS            (BIT6)
#define B_PCH_PCIE_STRPFUSECFG_LDCGDIS            (BIT5)
#define B_PCH_PCIE_STRPFUSECFG_LTCGDIS            (BIT4)
#define B_PCH_PCIE_STRPFUSECFG_BDCGDIS            (BIT3)
#define B_PCH_PCIE_STRPFUSECFG_DESKTOPMOB         (BIT1)
#define R_PCH_PCIE_AECH                           0x100
#define R_PCH_PCIE_UES                            0x104
#define S_PCH_PCIE_UES                            4
#define B_PCH_PCIE_UES_URE                        BIT20
#define B_PCH_PCIE_UES_EE                         BIT19
#define B_PCH_PCIE_UES_MT                         BIT18
#define B_PCH_PCIE_UES_RO                         BIT17
#define B_PCH_PCIE_UES_UC                         BIT16
#define B_PCH_PCIE_UES_CA                         BIT15
#define B_PCH_PCIE_UES_CT                         BIT14
#define B_PCH_PCIE_UES_FCPE                       BIT13
#define B_PCH_PCIE_UES_PT                         BIT12
#define B_PCH_PCIE_UES_DLPE                       BIT4
#define B_PCH_PCIE_UES_TE                         BIT0
#define R_PCH_PCIE_UEM                            0x108
#define S_PCH_PCIE_UEM                            4
#define B_PCH_PCIE_UEM_URE                        BIT20
#define B_PCH_PCIE_UEM_EE                         BIT19
#define B_PCH_PCIE_UEM_MT                         BIT18
#define B_PCH_PCIE_UEM_RO                         BIT17
#define B_PCH_PCIE_UEM_UC                         BIT16
#define B_PCH_PCIE_UEM_CA                         BIT15
#define B_PCH_PCIE_UEM_CT                         BIT14
#define B_PCH_PCIE_UEM_FCPE                       BIT13
#define B_PCH_PCIE_UEM_PT                         BIT12
#define B_PCH_PCIE_UEM_DLPE                       BIT4
#define B_PCH_PCIE_UEM_TE                         BIT0
#define R_PCH_PCIE_UEV                            0x10C
#define S_PCH_PCIE_UEV                            4
#define B_PCH_PCIE_UEV_URE                        BIT20
#define B_PCH_PCIE_UEV_EE                         BIT19
#define B_PCH_PCIE_UEV_MT                         BIT18
#define B_PCH_PCIE_UEV_RO                         BIT17
#define B_PCH_PCIE_UEV_UC                         BIT16
#define B_PCH_PCIE_UEV_CA                         BIT15
#define B_PCH_PCIE_UEV_CT                         BIT14
#define B_PCH_PCIE_UEV_FCPE                       BIT13
#define B_PCH_PCIE_UEV_PT                         BIT12
#define B_PCH_PCIE_UEV_DLPE                       BIT4
#define B_PCH_PCIE_UEV_TE                         BIT0
#define R_PCH_PCIE_CES                            0x110
#define S_PCH_PCIE_CES                            4
#define B_PCH_PCIE_CES_ANFES                      BIT13
#define B_PCH_PCIE_CES_RTT                        BIT12
#define B_PCH_PCIE_CES_RNR                        BIT8
#define B_PCH_PCIE_CES_BD                         BIT7
#define B_PCH_PCIE_CES_BT                         BIT6
#define B_PCH_PCIE_CES_RE                         BIT0
#define R_PCH_PCIE_CEM                            0x114
#define S_PCH_PCIE_CEM                            4
#define B_PCH_PCIE_CEM_ANFEM                      BIT13
#define B_PCH_PCIE_CEM_RTT                        BIT12
#define B_PCH_PCIE_CEM_RNR                        BIT8
#define B_PCH_PCIE_CEM_BD                         BIT7
#define B_PCH_PCIE_CEM_BT                         BIT6
#define B_PCH_PCIE_CEM_RE                         BIT0
#define R_PCH_PCIE_AECC                           0x118
#define S_PCH_PCIE_AECC                           4
#define B_PCH_PCIE_AECC_ECE                       BIT8
#define B_PCH_PCIE_AECC_ECC                       BIT7
#define B_PCH_PCIE_AECC_EGE                       BIT6
#define B_PCH_PCIE_AECC_EGC                       BIT5
#define B_PCH_PCIE_AECC_FEP                       0x0000001F
#define R_PCH_PCIE_RES                            0x130
#define S_PCH_PCIE_RES                            4
#define B_PCH_PCIE_RES_AEMN                       0xF8000000
#define B_PCH_PCIE_RES_FEMR                       BIT6
#define B_PCH_PCIE_RES_NFEMR                      BIT5
#define B_PCH_PCIE_RES_FUF                        BIT4
#define B_PCH_PCIE_RES_MENR                       BIT3
#define B_PCH_PCIE_RES_ENR                        BIT2
#define B_PCH_PCIE_RES_MCR                        BIT1
#define B_PCH_PCIE_RES_CR                         BIT0
#define R_PCH_PCIE_PECR2                          0x320
#define S_PCH_PCIE_PECR2                          4
#define B_PCH_PCIE_PECR2_FIELD_1                  BIT21
#define R_PCH_PCIE_PEETM                          0x324
#define S_PCH_PCIE_PEETM                          1
#define B_PCH_PCIE_PEETM_BAU                      BIT2
#define R_PCH_PCIE_PEC1                           0x330
#define S_PCH_PCIE_PEC1                           4
#define B_PCH_PCIE_PEC1_FIELD_1                   0xFF
#define R_PCH_PCIE_LTROVR                         0x400
#define R_PCH_PCIE_LTROVR2                        0x404
#define R_PCH_PCIE_L1SECH                         0x200
#define V_PCH_PCIE_L1SECH_L1SUBST_CAP_ID          0x1E
#define R_PCH_PCIE_L1SCAP                         0x204
#define R_PCH_PCIE_PCIEPMECTL                     0x420
#define B_PCH_PCIE_PCIEPMECTL_FDPGE               BIT31
#define B_PCH_PCIE_PCIEPMECTL_DLSULPGE            BIT30
#define B_PCH_PCIE_PCIEPMECTL_DLSULDLSD           BIT29
#define B_PCH_PCIE_PCIEPMECTL_L1LE                BIT17
#define V_PCH_PCIE_PCIEPMECTL_L1LTRTLV            (BIT13 | BIT12 | BIT11 | BIT10 | BIT9 | BIT8 | BIT7 | BIT6 | BIT5 | BIT4)
#endif
