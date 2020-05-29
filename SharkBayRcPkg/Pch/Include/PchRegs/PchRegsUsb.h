/** @file
  Register names for PCH USB devices

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
#ifndef _PCH_REGS_USB_H_
#define _PCH_REGS_USB_H_

//
// USB Definitions
//
#define LPTH_USB_MAX_PHYSICAL_PORTS         14      ///< Max Physical Connector EHCI + XHCI, not counting virtual ports like USB-R.
#define LPTH_EHCI_MAX_CONTROLLERS           2       ///< Max EHCI Controllers
#define LPTH_EHCI_MAX_PORTS                 14      ///< Counting ports behind RMHs 8 from EHCI-1 and 6 from EHCI-2, not counting EHCI USB-R virtual ports.
#define LPTH_XHCI_MAX_USB2_PORTS            15      ///< 14 High Speed lanes + Including one port reserved for USBr
#define LPTH_XHCI_MAX_USB3_PORTS            6       ///< 6 Super Speed lanes
#define LPTLP_USB_MAX_PHYSICAL_PORTS        8       ///< Max Physical Connector EHCI + XHCI, not counting virtual ports like USB-R.
#define LPTLP_EHCI_MAX_PORTS                8       ///< Counting ports behind RMHs 8 from EHCI-1 and 6 from EHCI-2, not counting EHCI USB-R virtual ports.
#define LPTLP_EHCI_MAX_CONTROLLERS          1       ///< Max EHCI Controllers
#define LPTLP_XHCI_MAX_USB2_PORTS           9       ///< 8 High Speed lanes + Including one port reserved for USBr
#define LPTLP_XHCI_MAX_USB3_PORTS           4       ///< 4 Super Speed lanes

#define R_PCH_USB_VENDOR_ID                 0x00
#define V_PCH_USB_VENDOR_ID                 V_PCH_INTEL_VENDOR_ID
#define R_PCH_USB_DEVICE_ID                 0x02
#define V_PCH_LPTH_USB_DEVICE_ID_EHCI_1      0x8C26  ///< LPT EHCI#1
#define V_PCH_LPTH_USB_DEVICE_ID_EHCI_2      0x8C2D  ///< LPT EHCI#2
#define V_PCH_LPTH_USB_DEVICE_ID_XHCI_1      0x8C31  ///< LPT XHCI#1
#define V_PCH_LPTLP_USB_DEVICE_ID_EHCI_1     0x9C26  ///< LPTLP EHCI#1
#define V_PCH_LPTLP_USB_DEVICE_ID_XHCI_1     0x9C31  ///< LPTLP XHCI#1

//
// USB2 (EHCI) related definitions
//
typedef enum {
  PchEhci1            = 0,
  PchEhci2,
  PchEhciControllerMax
} PCH_USB20_CONTROLLER_TYPE;

#define PCI_DEVICE_NUMBER_PCH_USB           29
#define PCI_FUNCTION_NUMBER_PCH_EHCI        0

#define PCI_DEVICE_NUMBER_PCH_USB_EXT       26
#define PCI_FUNCTION_NUMBER_PCH_EHCI2       0

//
// EHCI PCI Config Space registers
//
#define R_PCH_EHCI_COMMAND_REGISTER         0x04
#define B_PCH_EHCI_COMMAND_INTR_DIS         BIT10
#define B_PCH_EHCI_COMMAND_FBE              BIT9
#define B_PCH_EHCI_COMMAND_SERR_EN          BIT8
#define B_PCH_EHCI_COMMAND_WCC              BIT7
#define B_PCH_EHCI_COMMAND_PER              BIT6
#define B_PCH_EHCI_COMMAND_VPS              BIT5
#define B_PCH_EHCI_COMMAND_PMWE             BIT4
#define B_PCH_EHCI_COMMAND_SCE              BIT3
#define B_PCH_EHCI_COMMAND_BME              BIT2
#define B_PCH_EHCI_COMMAND_MSE              BIT1
#define B_PCH_EHCI_COMMAND_IOSE             BIT0

#define R_PCH_EHCI_PCISTS                   0x06
#define B_PCH_EHCI_PCISTS_DPE               BIT15
#define B_PCH_EHCI_PCISTS_SSE               BIT14
#define B_PCH_EHCI_PCISTS_RMA               BIT13
#define B_PCH_EHCI_PCISTS_RTA               BIT12
#define B_PCH_EHCI_PCISTS_STA               BIT11
#define B_PCH_EHCI_PCISTS_DEV_STS           (BIT10 | BIT9)
#define B_PCH_EHCI_PCISTS_DPED              BIT8
#define B_PCH_EHCI_PCISTS_FB2BC             BIT7
#define B_PCH_EHCI_PCISTS_UDF               BIT6
#define B_PCH_EHCI_PCISTS_66MHZ_CAP         BIT5
#define B_PCH_EHCI_PCISTS_CAP_LST           BIT4
#define B_PCH_EHCI_PCISTS_INTR_STS          BIT3

#define R_PCH_EHCI_RID                      0x08
#define B_PCH_EHCI_RID                      0xFF
#define R_PCH_EHCI_PI                       0x09
#define B_PCH_EHCI_PI                       0xFF
#define R_PCH_EHCI_SCC                      0x0A
#define B_PCH_EHCI_SCC                      0xFF
#define R_PCH_EHCI_BCC                      0x0B
#define B_PCH_EHCI_BCC                      0xFF
#define R_PCH_EHCI_MLT                      0x0D
#define B_PCH_EHCI_MLT                      0xFF
#define R_PCH_EHCI_HEADTYPE                 0x0E
#define B_PCH_EHCI_HEADTYPE                 0xFF
#define R_PCH_EHCI_MEM_BASE                 0x10
#define V_PCH_EHCI_MEM_LENGTH               0x400
#define N_PCH_EHCI_MEM_ALIGN                10
#define R_PCH_EHCI_SVID                     0x2C
#define B_PCH_EHCI_SVID                     0xFFFF
#define R_PCH_EHCI_SID                      0x2E
#define B_PCH_EHCI_SID                      0xFFFF
#define R_PCH_EHCI_CAP_PTR                  0x34
#define B_PCH_EHCI_CAP_PTR                  0xFF
#define R_PCH_EHCI_INT_LN                   0x3C
#define B_PCH_EHCI_INT_LN                   0xFF
#define R_PCH_EHCI_INT_PN                   0x3D
#define B_PCH_EHCI_INT_PN                   0xFF
#define R_PCH_EHCI_IHFCLK                   0x44
#define B_PCH_EHCI_IHFCLK                   0xFFFFFFFF
#define R_PCH_EHCI_IHFCLKC                  0x48
#define B_PCH_EHCI_IHFCLKC                  0xFFFFFFFF
#define R_PCH_EHCI_PWR_CAPID                0x50
#define B_PCH_EHCI_PWR_CAPID                0xFF
#define R_PCH_EHCI_NXT_PTR1                 0x51
#define B_PCH_EHCI_NXT_PTR1                 0xFF
#define R_PCH_EHCI_PWR_CAP                  0x52
#define B_PCH_EHCI_PWR_CAP_PME_SUP          0xF800
#define B_PCH_EHCI_PWR_CAP_D2_SUP           BIT10
#define B_PCH_EHCI_PWR_CAP_D1_SUP           BIT9
#define B_PCH_EHCI_PWR_CAP_AUX_CUR          (BIT8 | BIT7 | BIT6)
#define B_PCH_EHCI_PWR_CAP_DSI              BIT5
#define B_PCH_EHCI_PWR_CAP_PME_CLK          BIT3
#define B_PCH_EHCI_PWR_CAP_VER              (BIT2 | BIT1 | BIT0)
#define R_PCH_EHCI_PWR_CNTL_STS             0x54
#define B_PCH_EHCI_PWR_CNTL_STS_PME_STS     BIT15
#define B_PCH_EHCI_PWR_CNTL_STS_DATASCL     (BIT14 | BIT13)
#define B_PCH_EHCI_PWR_CNTL_STS_DATASEL     (BIT12 | BIT11 | BIT10 | BIT9)
#define B_PCH_EHCI_PWR_CNTL_STS_PME_EN      BIT8
#define B_PCH_EHCI_PWR_CNTL_STS_NO_SOFT_RESET BIT3
#define B_PCH_EHCI_PWR_CNTL_STS_PWR_STS     (BIT1 | BIT0)
#define V_PCH_EHCI_PWR_CNTL_STS_PWR_STS_D3  (BIT1 | BIT0)
#define R_PCH_EHCI_DBG_CAPID                0x58
#define B_PCH_EHCI_DBG_CAPID                0xFF
#define R_PCH_EHCI_NXT_PTR2                 0x59
#define B_PCH_EHCI_NXT_PTR2                 0xFF
#define R_PCH_EHCI_DBG_BASE                 0x5A
#define B_PCH_EHCI_DBG_BASE_BAR_NUM         0xE000
#define B_PCH_EHCI_DBG_BASE_PORT_OFFSET     0x1FFF
#define R_PCH_EHCI_USB_RELNUM               0x60
#define B_PCH_EHCI_USB_RELNUM               0xFF
#define R_PCH_EHCI_FL_ADJ                   0x61
#define B_PCH_EHCI_FL_ADJ                   0x3F
#define R_PCH_EHCI_PWAKE_CAP                0x62
#define B_PCH_EHCI_PWAKE_CAP_D29_MASK       0x01FE
#define B_PCH_EHCI_PWAKE_CAP_D26_MASK       0x007E
#define B_PCH_EHCI_PWAKE_CAP_PWK_IMP        BIT0
#define R_PCH_EHCI_PDO                      0x64
#define B_PCH_EHCI_PDO_DIS_PORT0            BIT0
#define B_PCH_EHCI_PDO_D29_MASK             0xFF
#define B_PCH_EHCI_PDO_D26_MASK             0x3F
#define R_PCH_EHCI_RMHDEVR                  0x66
#define B_PCH_EHCI_RMHDEVR_D29_MASK         0x01FE
#define B_PCH_EHCI_RMHDEVR_D26_MASK         0x007E
#define R_PCH_EHCI_LEGEXT_CAP               0x68
#define B_PCH_EHCI_LEGEXT_CAP_HCOS          BIT24
#define B_PCH_EHCI_LEGEXT_CAP_HCBIOS        BIT16
#define B_PCH_EHCI_LEGEXT_CAP_NEXT          0x0000FF00
#define B_PCH_EHCI_LEGEXT_CAP_CAPID         0x000000FF
#define R_PCH_EHCI_LEGEXT_CS                0x6C
#define B_PCH_EHCI_LEGEXT_CS_SMIBAR         BIT31
#define B_PCH_EHCI_LEGEXT_CS_SMIPCI         BIT30
#define B_PCH_EHCI_LEGEXT_CS_SMIOS          BIT29
#define B_PCH_EHCI_LEGEXT_CS_SMIAA          BIT21
#define B_PCH_EHCI_LEGEXT_CS_SMIHSE         BIT20
#define B_PCH_EHCI_LEGEXT_CS_SMIFLR         BIT19
#define B_PCH_EHCI_LEGEXT_CS_SMIPCD         BIT18
#define B_PCH_EHCI_LEGEXT_CS_SMIERR         BIT17
#define B_PCH_EHCI_LEGEXT_CS_SMICOMP        BIT16
#define B_PCH_EHCI_LEGEXT_CS_SMIBAR_EN      BIT15
#define B_PCH_EHCI_LEGEXT_CS_SMIPCI_EN      BIT14
#define B_PCH_EHCI_LEGEXT_CS_SMIOS_EN       BIT13
#define B_PCH_EHCI_LEGEXT_CS_SMIAA_EN       BIT5
#define B_PCH_EHCI_LEGEXT_CS_SMIHSE_EN      BIT4
#define B_PCH_EHCI_LEGEXT_CS_SMIFLR_EN      BIT3
#define B_PCH_EHCI_LEGEXT_CS_SMIPCD_EN      BIT2
#define B_PCH_EHCI_LEGEXT_CS_SMIERR_EN      BIT1
#define B_PCH_EHCI_LEGEXT_CS_SMICOMP_EN     BIT0
#define R_PCH_EHCI_SPCSMI                   0x70
#define B_PCH_EHCI_SPCSMI_D29               0x3FC00000
#define B_PCH_EHCI_SPCSMI_D26               0x0FC00000
#define B_PCH_EHCI_SPCSMI_PMCSR             BIT21
#define B_PCH_EHCI_SPCSMI_ASYNC             BIT20
#define B_PCH_EHCI_SPCSMI_PERIODIC          BIT19
#define B_PCH_EHCI_SPCSMI_CF                BIT18
#define B_PCH_EHCI_SPCSMI_HCHALT            BIT17
#define B_PCH_EHCI_SPCSMI_HCRESET           BIT16
#define B_PCH_EHCI_SPCSMI_PO_EN             0x00003FC0
#define B_PCH_EHCI_SPCSMI_PMCSR_EN          BIT5
#define B_PCH_EHCI_SPCSMI_ASYNC_EN          BIT4
#define B_PCH_EHCI_SPCSMI_PERIODIC_EN       BIT3
#define B_PCH_EHCI_SPCSMI_CF_EN             BIT2
#define B_PCH_EHCI_SPCSMI_HCHALT_EN         BIT1
#define B_PCH_EHCI_SPCSMI_HCRESET_EN        BIT0
#define R_PCH_EHCI_OCMAP                    0x74
#define R_PCH_EHCI_RMHWKCTL                 0x7E
#define R_PCH_EHCI_ACCESS_CNTL              0x80
#define B_PCH_EHCI_ACCESS_CNTL_ENABLE       BIT0
#define V_PCH_EHCI_ACCESS_CNTL_ENABLE       0x01
#define R_PCH_EHCI_FLR_CID                  0x98
#define B_PCH_EHCI_FLR_CID                  0xFF
#define V_PCH_EHCI_FLR_CID_13               0x13
#define V_PCH_EHCI_FLR_CID_09               0x09
#define R_PCH_EHCI_FLR_NEXT                 0x99
#define B_PCH_EHCI_FLR_NEXT                 0xFF
#define R_PCH_EHCI_FLR_CLV                  0x9A
#define B_PCH_EHCI_FLR_CLV_CAP_SSEL0        BIT9
#define B_PCH_EHCI_FLR_CLV_TXP_SSEL0        BIT8
#define B_PCH_EHCI_FLR_CLV_VSCID_SSEL1      0xF000
#define B_PCH_EHCI_FLR_CLV_CAPVER_SSEL1     0x0F00
#define B_PCH_EHCI_FLR_CLV_LNG              0x00FF
#define R_PCH_EHCI_FLR_CTRL                 0x9C
#define B_PCH_EHCI_FLR_CTRL_INITFLR         BIT0
#define R_PCH_EHCI_FLR_STS                  0x9D
#define B_PCH_EHCI_FLR_STS_TXP              BIT0

//
// EHCI MMIO registers
//
#define R_PCH_EHCI_HCSPARAMS                0x04
#define N_PCH_EHCI_HCSPARAMS_DP_N           20
#define N_PCH_EHCI_HCSPARAMS_N_CC           12
#define N_PCH_EHCI_HCSPARAMS_N_PCC          8
#define N_PCH_EHCI_HCSPARAMS_N_PORTS        0
#define R_PCH_EHCI_USB2CMD                  0x20
#define B_PCH_EHCI_USB2CMD_ASE              BIT5
#define B_PCH_EHCI_USB2CMD_PSE              BIT4
#define B_PCH_EHCI_USB2CMD_HCRESET          BIT1
#define B_PCH_EHCI_USB2CMD_RS               BIT0
#define R_PCH_EHCI_USB2STS                  0x24
#define B_PCH_EHCI_USB2STS_HCHALTED         BIT12
#define R_PCH_EHCI_CONFIGFLAG               0x60
#define R_PCH_EHCI_PORTSC0                  0x64
#define R_PCH_EHCI_PORTSC0_SUSPEND          BIT7
#define R_PCH_EHCI_PORTSC0_PORT_EN_DIS      BIT2
#define B_PCH_EHCI_PORTSC0_CHANGE_ENABLE_MASK     (0x2A | R_PCH_EHCI_PORTSC0_PORT_EN_DIS) ///< Mask all change bits and port enabled
#define B_PCH_EHCI_PORTSC0_RESET            BIT8

//
// USB3 (XHCI) related definitions
//
#define PCI_DEVICE_NUMBER_PCH_XHCI          20
#define PCI_FUNCTION_NUMBER_PCH_XHCI        0

//
// XHCI PCI Config Space registers
//
#define R_PCH_XHCI_COMMAND_REGISTER         0x04
#define B_PCH_XHCI_COMMAND_BME              BIT2
#define B_PCH_XHCI_COMMAND_MSE              BIT1
#define R_PCH_XHCI_MEM_BASE                 0x10
#define V_PCH_XHCI_MEM_LENGTH               0x3000
#define N_PCH_XHCI_MEM_ALIGN                16
#define R_PCH_XHCI_SVID                     0x2C
#define B_PCH_XHCI_SVID                     0xFFFF
#define R_PCH_XHCI_SID                      0x2E
#define B_PCH_XHCI_SID                      0xFFFF

#define R_PCH_XHCI_XHCC1                    0x40
#define B_PCH_XHCI_XHCC1_ACCTRL             BIT31
#define B_PCH_XHCI_XHCC1_RMTASERR           BIT24
#define B_PCH_XHCI_XHCC1_URD                BIT23
#define B_PCH_XHCI_XHCC1_URRE               BIT22
#define B_PCH_XHCI_XHCC1_IIL1E              (BIT21 | BIT20 | BIT19)
#define V_PCH_XHCI_XHCC1_IIL1E_DIS          0
#define V_PCH_XHCI_XHCC1_IIL1E_32           (BIT19)
#define V_PCH_XHCI_XHCC1_IIL1E_64           (BIT20)
#define V_PCH_XHCI_XHCC1_IIL1E_128          (BIT20 | BIT19)
#define V_PCH_XHCI_XHCC1_IIL1E_256          (BIT21)
#define V_PCH_XHCI_XHCC1_IIL1E_512          (BIT21 | BIT19)
#define V_PCH_XHCI_XHCC1_IIL1E_1024         (BIT21 | BIT20)
#define V_PCH_XHCI_XHCC1_IIL1E_131072       (BIT21 | BIT20 | BIT19)
#define B_PCH_XHCI_XHCC1_XHCIL1E            BIT18
#define B_PCH_XHCI_XHCC1_D3IL1E             BIT17
#define B_PCH_XHCI_XHCC1_UNPPA              (BIT16 | BIT15 | BIT14 | BIT13 | BIT12)
#define B_PCH_XHCI_XHCC1_SWAXHCI            BIT11
#define B_PCH_XHCI_XHCC1_L23HRAWC           (BIT10 | BIT9 | BIT8)
#define B_PCH_XHCI_XHCC1_UTAGCP             (BIT7 | BIT6)
#define B_PCH_XHCI_XHCC1_UDAGCNP            (BIT5 | BIT4)
#define B_PCH_XHCI_XHCC1_UDAGCCP            (BIT3 | BIT2)
#define B_PCH_XHCI_XHCC1_UDAGC              (BIT1 | BIT0)

#define R_PCH_XHCI_XHCC2                    0x44
#define B_PCH_XHCI_XHCC2_OCCFDONE           BIT31
#define B_PCH_XHCI_XHCC2_XHCUPRDROE         BIT11
#define B_PCH_XHCI_XHCC2_IOSFSRAD           BIT10
#define B_PCH_XHCI_XHCC2_SWACXIHB           (BIT9 | BIT8)
#define B_PCH_XHCI_XHCC2_SWADMIL1IHB        (BIT7 | BIT6)
#define B_PCH_XHCI_XHCC2_L1FP2CGWC          (BIT5 | BIT4 | BIT3)
#define B_PCH_XHCI_XHCC2_RDREQSZCTRL        (BIT2 | BIT1 | BIT0)

#define R_PCH_XHCI_XHCLKGTEN                0x50
#define B_PCH_XHCI_XHCLKGTEN_SSLSE          BIT26
#define B_PCH_XHCI_XHCLKGTEN_USB2PLLSE      BIT25
#define B_PCH_XHCI_XHCLKGTEN_IOSFSTCGE      BIT24
#define B_PCH_XHCI_XHCLKGTEN_HSTCGE         (BIT23 | BIT22 | BIT21 | BIT20)
#define B_PCH_XHCI_XHCLKGTEN_SSTCGE         (BIT19 | BIT18 | BIT17 | BIT16)
#define B_PCH_XHCI_XHCLKGTEN_XHCIGEU3S      BIT15
#define B_PCH_XHCI_XHCLKGTEN_XHCFTCLKSE     BIT14
#define B_PCH_XHCI_XHCLKGTEN_XHCBBTCGIPISO  BIT13
#define B_PCH_XHCI_XHCLKGTEN_XHCHSTCGU2NRWE BIT12
#define B_PCH_XHCI_XHCLKGTEN_XHCUSB2PLLSDLE (BIT11 | BIT10)
#define B_PCH_XHCI_XHCLKGTEN_HSPLLSUE       (BIT9 | BIT8)
#define B_PCH_XHCI_XHCLKGTEN_SSPLLSUE       (BIT7 | BIT6 | BIT5)
#define B_PCH_XHCI_XHCLKGTEN_XHCBLCGE       BIT4
#define B_PCH_XHCI_XHCLKGTEN_HSLTCGE        BIT3
#define B_PCH_XHCI_XHCLKGTEN_SSLTCGE        BIT2
#define B_PCH_XHCI_XHCLKGTEN_IOSFBTCGE      BIT1
#define B_PCH_XHCI_XHCLKGTEN_IOSFGBLCGE     BIT0

#define R_PCH_XHCI_USB_RELNUM               0x60
#define B_PCH_XHCI_USB_RELNUM               0xFF
#define R_PCH_XHCI_FL_ADJ                   0x61
#define B_PCH_XHCI_FL_ADJ                   0x3F
#define R_PCH_XHCI_PWR_CAPID                0x70
#define B_PCH_XHCI_PWR_CAPID                0xFF
#define R_PCH_XHCI_NXT_PTR1                 0x71
#define B_PCH_XHCI_NXT_PTR1                 0xFF
#define R_PCH_XHCI_PWR_CAP                  0x72
#define B_PCH_XHCI_PWR_CAP_PME_SUP          0xF800
#define B_PCH_XHCI_PWR_CAP_D2_SUP           BIT10
#define B_PCH_XHCI_PWR_CAP_D1_SUP           BIT9
#define B_PCH_XHCI_PWR_CAP_AUX_CUR          (BIT8 | BIT7 | BIT6)
#define B_PCH_XHCI_PWR_CAP_DSI              BIT5
#define B_PCH_XHCI_PWR_CAP_PME_CLK          BIT3
#define B_PCH_XHCI_PWR_CAP_VER              (BIT2 | BIT1 | BIT0)
#define R_PCH_XHCI_PWR_CNTL_STS             0x74
#define B_PCH_XHCI_PWR_CNTL_STS_PME_STS     BIT15
#define B_PCH_XHCI_PWR_CNTL_STS_DATASCL     (BIT14 | BIT13)
#define B_PCH_XHCI_PWR_CNTL_STS_DATASEL     (BIT12 | BIT11 | BIT10 | BIT9)
#define B_PCH_XHCI_PWR_CNTL_STS_PME_EN      BIT8
#define B_PCH_XHCI_PWR_CNTL_STS_PWR_STS     (BIT1 | BIT0)
#define V_PCH_XHCI_PWR_CNTL_STS_PWR_STS_D3  (BIT1 | BIT0)

#define R_PCH_XHCI_U2OCM1                   0xC0
#define B_PCH_XHCI_U2OCM1_OC4_MAPPING       0xFF000000
#define B_PCH_XHCI_U2OCM1_OC3_MAPPING       0x00FF0000
#define B_PCH_XHCI_U2OCM1_OC2_MAPPING       0x0000FF00
#define B_PCH_XHCI_U2OCM1_OC1_MAPPING       0x000000FF

#define R_PCH_XHCI_U2OCM2                   0xC4
#define B_PCH_XHCI_U2OCM2_OC8_MAPPING       0x3F000000
#define B_PCH_XHCI_U2OCM2_OC7_MAPPING       0x003F0000
#define B_PCH_XHCI_U2OCM2_OC6_MAPPING       0x00003F00
#define B_PCH_XHCI_U2OCM2_OC5_MAPPING       0x0000003F

#define R_PCH_XHCI_U3OCM1                   0xC8
#define B_PCH_XHCI_U3OCM1_OC4_MAPPING       0x3F000000
#define B_PCH_XHCI_U3OCM1_OC3_MAPPING       0x003F0000
#define B_PCH_XHCI_U3OCM1_OC2_MAPPING       0x00003F00
#define B_PCH_XHCI_U3OCM1_OC1_MAPPING       0x0000003F

#define R_PCH_XHCI_U3OCM2                   0xCC
#define B_PCH_XHCI_U3OCM2_OC8_MAPPING       0x3F000000
#define B_PCH_XHCI_U3OCM2_OC7_MAPPING       0x003F0000
#define B_PCH_XHCI_U3OCM2_OC6_MAPPING       0x00003F00
#define B_PCH_XHCI_U3OCM2_OC5_MAPPING       0x0000003F

#define R_PCH_XHCI_USB2PR                   0xD0
#define B_PCH_XHCI_USB2PR_USB2HCSEL         0x7FFF
#define R_PCH_XHCI_USB2PRM                  0xD4
#define B_PCH_XHCI_USB2PR_USB2HCSELM        0x7FFF

#define R_PCH_XHCI_USB3PR                   0xD8
#define B_PCH_XHCI_USB3PR_USB3SSEN          0x3F
#define R_PCH_XHCI_USB3PRM                  0xDC
#define B_PCH_XHCI_USB3PR_USB3SSENM         0x3F

#define R_PCH_XHCI_FUS                      0xE0
#define B_PCH_XHCI_FUS_USBR                 (BIT5)
#define V_PCH_XHCI_FUS_USBR_EN              0
#define V_PCH_XHCI_FUS_USBR_DIS             (BIT5)

#define B_PCH_XHCI_FUS_SSPRTCNT             (BIT4 | BIT3)
#define V_PCH_XHCI_FUS_SSPRTCNT_00B         0
#define V_PCH_XHCI_FUS_SSPRTCNT_01B         (BIT3)
#define V_PCH_XHCI_FUS_SSPRTCNT_10B         (BIT4)
#define V_PCH_XHCI_FUS_SSPRTCNT_11B         (BIT4 | BIT3)

#define B_PCH_XHCI_FUS_HSPRTCNT             (BIT2 | BIT1)
#define V_PCH_XHCI_FUS_HSPRTCNT_00B         0
#define V_PCH_XHCI_FUS_HSPRTCNT_01B         (BIT1)
#define V_PCH_XHCI_FUS_HSPRTCNT_10B         (BIT2)
#define V_PCH_XHCI_FUS_HSPRTCNT_11B         (BIT2 | BIT1)

#define V_PCH_H_XHCI_FUS_SSPRTCNT_00B_CNT   6
#define V_PCH_H_XHCI_FUS_SSPRTCNT_01B_CNT   4
#define V_PCH_H_XHCI_FUS_SSPRTCNT_10B_CNT   2
#define V_PCH_H_XHCI_FUS_SSPRTCNT_11B_CNT   0
#define V_PCH_H_XHCI_FUS_SSPRTCNT_00B_MASK  0x3F
#define V_PCH_H_XHCI_FUS_SSPRTCNT_01B_MASK  0x0F
#define V_PCH_H_XHCI_FUS_SSPRTCNT_10B_MASK  0x03
#define V_PCH_H_XHCI_FUS_SSPRTCNT_11B_MASK  0x00

#define V_PCH_H_XHCI_FUS_HSPRTCNT_00B_CNT   14
#define V_PCH_H_XHCI_FUS_HSPRTCNT_01B_CNT   12
#define V_PCH_H_XHCI_FUS_HSPRTCNT_10B_CNT   10
#define V_PCH_H_XHCI_FUS_HSPRTCNT_11B_CNT   8
#define V_PCH_H_XHCI_FUS_HSPRTCNT_00B_MASK  0x3FFF
#define V_PCH_H_XHCI_FUS_HSPRTCNT_01B_MASK  0x3FFF
#define V_PCH_H_XHCI_FUS_HSPRTCNT_10B_MASK  0x0FFF
#define V_PCH_H_XHCI_FUS_HSPRTCNT_11B_MASK  0x00FF

#define V_PCH_LP_XHCI_FIXED_SSPRTCNT        4
#define V_PCH_LP_XHCI_FIXED_SSPRTCNT_MASK   0x0F

#define V_PCH_LP_XHCI_FIXED_HSPRTCNT        8
#define V_PCH_LP_XHCI_FIXED_HSPRTCNT_MASK   0x00FF

#define R_PCH_XHCI_USB2PDO                  0xE4
#define B_PCH_XHCI_USB2PDO_MASK             0x7FFF
#define B_PCH_XHCI_USB2PDO_DIS_PORT0        BIT0

#define R_PCH_XHCI_USB3PDO                  0xE8
#define B_PCH_XHCI_USB3PDO_MASK             0x3F
#define B_PCH_XHCI_USB3PDO_DIS_PORT0        BIT0

//
// xHCI MMIO registers
//

//
// 0x00 - 0x1F - Capability Registers
//
#define R_PCH_XHCI_CAPLENGTH                0x00
#define R_PCH_XHCI_HCIVERSION               0x02
#define R_PCH_XHCI_HCSPARAMS1               0x04
#define R_PCH_XHCI_HCSPARAMS2               0x08
#define R_PCH_XHCI_HCSPARAMS3               0x0C
#define B_PCH_XHCI_HCSPARAMS3_U2DEL         0xFFFF0000
#define B_PCH_XHCI_HCSPARAMS3_U1DEL         0x0000FFFF
#define R_PCH_XHCI_HCCPARAMS                0x10
#define B_PCH_XHCI_HCCPARAMS_LHRC           BIT5
#define B_PCH_XHCI_HCCPARAMS_MAXPSASIZE     0xF000
#define R_PCH_XHCI_DBOFF                    0x14
#define R_PCH_XHCI_RTSOFF                   0x18

//
// 0x80 - 0xBF - Operational Registers
//
#define R_PCH_XHCI_USBCMD                   0x80
#define B_PCH_XHCI_USBCMD_RS                BIT0   ///< Run/Stop
#define B_PCH_XHCI_USBCMD_RST               BIT1   ///< HCRST
#define R_PCH_XHCI_USBSTS                   0x84

//
// 0x480 - 0x5CF - Port Status and Control Registers
//
#define R_PCH_XHCI_PORTSC01USB2             0x480
#define R_PCH_XHCI_PORTSC02USB2             0x490
#define R_PCH_XHCI_PORTSC03USB2             0x4A0
#define R_PCH_XHCI_PORTSC04USB2             0x4B0
#define R_PCH_XHCI_PORTSC05USB2             0x4C0
#define R_PCH_XHCI_PORTSC06USB2             0x4D0
#define R_PCH_XHCI_PORTSC07USB2             0x4E0
#define R_PCH_XHCI_PORTSC08USB2             0x4F0
#define R_PCH_XHCI_PORTSC09USB2             0x500
#define R_PCH_H_XHCI_PORTSC10USB2           0x510
#define R_PCH_H_XHCI_PORTSC11USB2           0x520
#define R_PCH_H_XHCI_PORTSC12USB2           0x530
#define R_PCH_H_XHCI_PORTSC13USB2           0x540
#define R_PCH_H_XHCI_PORTSC14USB2           0x550
#define R_PCH_H_XHCI_PORTSC15USB2           0x560

#define B_PCH_XHCI_PORTSCXUSB2_WPR          BIT31  ///< Warm Port Reset
#define B_PCH_XHCI_PORTSCXUSB2_CEC          BIT23  ///< Port Config Error Change
#define B_PCH_XHCI_PORTSCXUSB2_PLC          BIT22  ///< Port Link State Change
#define B_PCH_XHCI_PORTSCXUSB2_PRC          BIT21  ///< Port Reset Change
#define B_PCH_XHCI_PORTSCXUSB2_OCC          BIT20  ///< Over-current Change
#define B_PCH_XHCI_PORTSCXUSB2_WRC          BIT19  ///< Warm Port Reset Change
#define B_PCH_XHCI_PORTSCXUSB2_PEC          BIT18  ///< Port Enabled Disabled Change
#define B_PCH_XHCI_PORTSCXUSB2_CSC          BIT17  ///< Connect Status Change
#define B_PCH_XHCI_PORTSCXUSB2_LWS          BIT16  ///< Port Link State Write Strobe
#define B_PCH_XHCI_USB2_U3_EXIT             (BIT5 | BIT6 | BIT7 | BIT8)
#define B_PCH_XHCI_USB2_U0_MASK             (BIT5 | BIT6 | BIT7 | BIT8)
#define B_PCH_XHCI_PORTSCXUSB2_PP           BIT9
#define B_PCH_XHCI_PORTSCXUSB2_PR           BIT4   ///< Port Reset
#define B_PCH_XHCI_PORTSCXUSB2_PED          BIT1   ///< Port Enable/Disabled
#define B_PCH_XHCI_PORTSCXUSB2_CCS          BIT0   ///< Current Connect Status
#define B_PCH_XHCI_PORT_CHANGE_ENABLE_MASK  (B_PCH_XHCI_PORTSCXUSB2_CEC | B_PCH_XHCI_PORTSCXUSB2_PLC | B_PCH_XHCI_PORTSCXUSB2_PRC | B_PCH_XHCI_PORTSCXUSB2_OCC | B_PCH_XHCI_PORTSCXUSB2_WRC | B_PCH_XHCI_PORTSCXUSB2_PEC | B_PCH_XHCI_PORTSCXUSB2_CSC | B_PCH_XHCI_PORTSCXUSB2_PED)

#define R_PCH_H_XHCI_PORTSC1USB3            0x570
#define R_PCH_H_XHCI_PORTSC2USB3            0x580
#define R_PCH_H_XHCI_PORTSC3USB3            0x590
#define R_PCH_H_XHCI_PORTSC4USB3            0x5A0
#define R_PCH_H_XHCI_PORTSC5USB3            0x5B0
#define R_PCH_H_XHCI_PORTSC6USB3            0x5C0

#define R_PCH_LP_XHCI_PORTSC1USB3           0x510
#define R_PCH_LP_XHCI_PORTSC2USB3           0x520
#define R_PCH_LP_XHCI_PORTSC3USB3           0x530
#define R_PCH_LP_XHCI_PORTSC4USB3           0x540

#define B_PCH_XHCI_PORTSCXUSB3_WPR          BIT31  ///< Warm Port Reset
#define B_PCH_XHCI_PORTSCXUSB3_CEC          BIT23  ///< Port Config Error Change
#define B_PCH_XHCI_PORTSCXUSB3_PLC          BIT22  ///< Port Link State Change
#define B_PCH_XHCI_PORTSCXUSB3_PRC          BIT21  ///< Port Reset Change
#define B_PCH_XHCI_PORTSCXUSB3_OCC          BIT20  ///< Over-current Change
#define B_PCH_XHCI_PORTSCXUSB3_WRC          BIT19  ///< Warm Port Reset Change
#define B_PCH_XHCI_PORTSCXUSB3_PEC          BIT18  ///< Port Enabled Disabled Change
#define B_PCH_XHCI_PORTSCXUSB3_CSC          BIT17  ///< Connect Status Change
#define B_PCH_XHCI_PORTSCXUSB3_PLS          (BIT8 | BIT7 | BIT6 | BIT5)    ///< Port Link State
#define V_PCH_XHCI_PORTSCXUSB3_PLS_POLLING  0x000000E0    ///< Link is in the Polling State
#define V_PCH_XHCI_PORTSCXUSB3_PLS_RXDETECT 0x000000A0    ///< Link is in the RxDetect State
#define B_PCH_XHCI_PORTSCXUSB3_PR           BIT4   ///< Port Reset
#define B_PCH_XHCI_PORTSCXUSB3_PED          BIT1   ///< Port Enable/Disabled
#define B_PCH_XHCI_PORTSCXUSB3_CHANGE_ENABLE_MASK  (B_PCH_XHCI_PORTSCXUSB3_CEC | B_PCH_XHCI_PORTSCXUSB3_PLC | B_PCH_XHCI_PORTSCXUSB3_PRC | B_PCH_XHCI_PORTSCXUSB3_OCC | B_PCH_XHCI_PORTSCXUSB3_WRC | B_PCH_XHCI_PORTSCXUSB3_PEC | B_PCH_XHCI_PORTSCXUSB3_CSC | B_PCH_XHCI_PORTSCXUSB3_PED)
//
// 0x2000 - 0x21FF - Runtime Registers
// 0x3000 - 0x307F - Doorbell Registers
//

//
// 0x8000 - 0x833F - Extended Capabilities Registers
//
#define R_PCH_XHCI_MMIO_ECR_8058            0x8058
#define B_PCH_XHCI_MMIO_ECR_8058_BIT8       BIT8   ///< Set 0

#define R_PCH_XHCI_MMIO_ECR_8090            0x8090
#define B_PCH_XHCI_MMIO_ECR_8090_BIT14      BIT14  ///< Set 1
#define B_PCH_XHCI_MMIO_ECR_8090_BIT8       BIT8   ///< Set 1

#define R_PCH_XHCI_MMIO_ECR_8094            0x8094
#define B_PCH_XHCI_MMIO_ECR_8094_BIT23      BIT23  ///< Set 1

#define R_PCH_XHCI_MMIO_ECR_80E0            0x80E0
#define B_PCH_XHCI_MMIO_ECR_80E0_BIT9       BIT9   ///< Set 1
#define B_PCH_XHCI_MMIO_ECR_80E0_BIT6       BIT6   ///< Set 1

#define R_PCH_XHCI_MMIO_ECR_80EC            0x80EC
#define B_PCH_XHCI_MMIO_ECR_80EC_BIT_14_12  0x7000 ///< Set 6
#define B_PCH_XHCI_MMIO_ECR_80EC_BIT_11_9   0x0E00 ///< Set 6

#define R_PCH_XHCI_MMIO_ECR_8110            0x8110
#define B_PCH_XHCI_MMIO_ECR_8110_BIT2       BIT2   ///< Set 0

#endif
