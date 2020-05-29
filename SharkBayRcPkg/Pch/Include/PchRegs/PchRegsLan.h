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

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchRegsLan.h

@brief
  Register names for PCH LAN device

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

**/
#ifndef _PCH_REGS_LAN_H_
#define _PCH_REGS_LAN_H_

//
// LAN Controller Registers (D25:F0)
//
#define PCI_BUS_NUMBER_PCH_LAN        0
#define PCI_DEVICE_NUMBER_PCH_LAN     25
#define PCI_FUNCTION_NUMBER_PCH_LAN   0

#define R_PCH_LAN_VENDOR_ID           0x00
#define V_PCH_LAN_VENDOR_ID           V_PCH_INTEL_VENDOR_ID
#define R_PCH_LAN_DEVICE_ID           0x02
//[-start-120510-IB06150219-modify]//
//#define V_PCH_LPTH_LAN_DEVICE_ID      0x8C33
#define V_PCH_LPTH_LAN_DEVICE_ID      0x153A
//[-end-120510-IB06150219-modify]//
#define V_PCH_LPTLP_LAN_DEVICE_ID     0x155A
#define R_PCH_LAN_CMD                 0x04
#define B_PCH_LAN_CMD_INTR_DIS        BIT10
#define B_PCH_LAN_CMD_FBE             BIT9
#define B_PCH_LAN_CMD_SERR_EN         BIT8
#define B_PCH_LAN_CMD_WCC             BIT7
#define B_PCH_LAN_CMD_PER             BIT6
#define B_PCH_LAN_CMD_PSE             BIT5
#define B_PCH_LAN_CMD_PMWE            BIT4
#define B_PCH_LAN_CMD_SCE             BIT3
#define B_PCH_LAN_CMD_BME             BIT2
#define B_PCH_LAN_CMD_MSE             BIT1
#define B_PCH_LAN_CMD_IOSE            BIT0
#define R_PCH_LAN_STS                 0x06
#define B_PCH_LAN_STS_DPE             BIT15
#define B_PCH_LAN_STS_SSE             BIT14
#define B_PCH_LAN_STS_RMA             BIT13
#define B_PCH_LAN_STS_RTA             BIT12
#define B_PCH_LAN_STS_STA             BIT11
#define B_PCH_LAN_STS_DEV_STS         (BIT10 | BIT9)
#define B_PCH_LAN_STS_DPED            BIT8
#define B_PCH_LAN_STS_FB2BC           BIT7
#define B_PCH_LAN_STS_66MHZ_CAP       BIT5
#define B_PCH_LAN_STS_CAP_LST         BIT4
#define B_PCH_LAN_STS_INTR_STS        BIT3
#define R_PCH_LAN_RID                 0x08
#define B_PCH_LAN_RID                 0xFF
#define R_PCH_LAN_CC                  0x09
#define S_PCH_LAN_CC                  3
#define B_PCH_LAN_CC                  0xFFFFFF
#define R_PCH_LAN_CLS                 0x0C
#define B_PCH_LAN_CLS                 0xFF
#define R_PCH_LAN_PLT                 0x0D
#define B_PCH_LAN_PLT                 0xFF
#define R_PCH_LAN_HEADTYPE            0x0E
#define B_PCH_LAN_HEADTYPE            0xFF
#define R_PCH_LAN_MEM_BASE_A          0x10
#define B_PCH_LAN_MBARA_BA            0xFFFF8000
#define B_PCH_LAN_MBARA_MSIZE         0x00007FF0
#define B_PCH_LAN_MBARA_PM            BIT3
#define B_PCH_LAN_MBARA_MT            (BIT2 | BIT1)
#define B_PCH_LAN_MBARA_MIOS          BIT0
#define R_PCH_LAN_MBARB               0x14
#define B_PCH_LAN_MBARB_BA            0xFFFFF000
#define B_PCH_LAN_MBARB_MSIZE         0x00000FF0
#define B_PCH_LAN_MBARB_PM            BIT3
#define B_PCH_LAN_MBARB_MT            (BIT2 | BIT1)
#define B_PCH_LAN_MBARB_MIOS          BIT0
#define R_PCH_LAN_MBARC               0x18
#define B_PCH_LAN_MBARC_BA            0xFFFFFFE0
#define B_PCH_LAN_MBARC_IOSIZE        0x0000001E
#define B_PCH_LAN_MBARC_MIOS          BIT0
#define R_PCH_LAN_SVID                0x2C
#define B_PCH_LAN_SVID                0xFFFF
#define R_PCH_LAN_SID                 0x2E
#define B_PCH_LAN_SID                 0xFFFF
#define R_PCH_LAN_ERBA                0x30
#define B_PCH_LAN_ERBA                0xFFFFFFFF
#define R_PCH_LAN_CAP_PTR             0x34
#define B_PCH_LAN_CAP_PTR             0xFF
#define R_PCH_LAN_INTR                0x3C
#define B_PCH_LAN_INTR_IPIN           0xFF00
#define B_PCH_LAN_INTR_ILINE          0x00FF
#define V_PCH_LAN_MEM_LENGTH          0x8000
#define N_PCH_LAN_MEM_ALIGN           15
#define R_PCH_LAN_LTR_CAP             0xA8
#define R_PCH_LAN_CLIST1              0xC8
#define B_PCH_LAN_CLIST1_NEXT         0xFF00
#define B_PCH_LAN_CLIST1_CID          0x00FF
#define R_PCH_LAN_PMC                 0xCA
#define B_PCH_LAN_PMC_PMES            0xF800
#define B_PCH_LAN_PMC_D2S             BIT10
#define B_PCH_LAN_PMC_D1S             BIT9
#define B_PCH_LAN_PMC_AC              (BIT8 | BIT7 | BIT6)
#define B_PCH_LAN_PMC_DSI             BIT5
#define B_PCH_LAN_PMC_PMEC            BIT3
#define B_PCH_LAN_PMC_VS              (BIT2 | BIT1 | BIT0)
#define R_PCH_LAN_PMCS                0xCC
#define B_PCH_LAN_PMCS_PMES           BIT15
#define B_PCH_LAN_PMCS_DSC            (BIT14 | BIT13)
#define B_PCH_LAN_PMCS_DSL            0x1E00
#define V_PCH_LAN_PMCS_DSL0           0x0000
#define V_PCH_LAN_PMCS_DSL3           0x0600
#define V_PCH_LAN_PMCS_DSL4           0x0800
#define V_PCH_LAN_PMCS_DSL7           0x0E00
#define V_PCH_LAN_PMCS_DSL8           0x1000
#define B_PCH_LAN_PMCS_PMEE           BIT8
#define B_PCH_LAN_PMCS_PS             (BIT1 | BIT0)
#define V_PCH_LAN_PMCS_PS0            0x00
#define V_PCH_LAN_PMCS_PS3            0x03
#define R_PCH_LAN_DR                  0xCF
#define B_PCH_LAN_DR                  0xFF
#define R_PCH_LAN_CLIST2              0xD0
#define B_PCH_LAN_CLIST2_NEXT         0xFF00
#define B_PCH_LAN_CLIST2_CID          0x00FF
#define R_PCH_LAN_MCTL                0xD2
#define B_PCH_LAN_MCTL_CID            BIT7
#define B_PCH_LAN_MCTL_MME            (BIT6 | BIT5 | BIT4)
#define B_PCH_LAN_MCTL_MMC            (BIT3 | BIT2 | BIT1)
#define B_PCH_LAN_MCTL_MSIE           BIT0
#define R_PCH_LAN_MADDL               0xD4
#define B_PCH_LAN_MADDL               0xFFFFFFFF
#define R_PCH_LAN_MADDH               0xD8
#define B_PCH_LAN_MADDH               0xFFFFFFFF
#define R_PCH_LAN_MDAT                0xDC
#define B_PCH_LAN_MDAT                0xFFFFFFFF
#define R_PCH_LAN_FLRCAP              0xE0
#define B_PCH_LAN_FLRCAP_NEXT         0xFF00
#define B_PCH_LAN_FLRCAP_CID          0x00FF
#define V_PCH_LAN_FLRCAP_CID_SSEL0    0x13
#define V_PCH_LAN_FLRCAP_CID_SSEL1    0x09
#define R_PCH_LAN_FLRCLV              0xE2
#define B_PCH_LAN_FLRCLV_FLRC_SSEL0   BIT9
#define B_PCH_LAN_FLRCLV_TXP_SSEL0    BIT8
#define B_PCH_LAN_FLRCLV_VSCID_SSEL1  0xF000
#define B_PCH_LAN_FLRCLV_CAPVER_SSEL1 0x0F00
#define B_PCH_LAN_FLRCLV_CAPLNG       0x00FF
#define R_PCH_LAN_DEVCTRL             0xE4
#define B_PCH_LAN_DEVCTRL             BIT0
//
// Gigabit LAN Capabilities and Status Registers (Memory space)
//
#define R_PCH_MBARA_GBECSR1           0x0000
#define B_PCH_MBARA_GBECSR1_PHYPDN    BIT24
#define R_PCH_MBARA_GBECSR2           0x0018
#define B_PCH_MBARA_GBECSR2_PHYPDEN   BIT20
#define R_PCH_MBARA_GBECSR3           0x0020
#define B_PCH_MBARA_GBECSR3_RB        BIT28
#define B_PCH_MBARA_GBECSR3_MDI_TYPE  (BIT27 | BIT26)
#define B_PCH_MBARA_GBECSR3_DATA      0x0000FFFF
#define R_PCH_MBARA_GBECSR4           0x002C
#define B_PCH_MBARA_GBECSR4_WIV       BIT31
#define B_PCH_MBARA_GBECSR4_WESB      BIT30
#define R_PCH_MBARA_GBECSR5           0x0F00
#define B_PCH_MBARA_GBECSR5_SWFLAG    BIT5
#define R_PCH_MBARA_GBECSR6           0x0F10
#define B_PCH_MBARA_GBECSR6_GGD       BIT6
#define B_PCH_MBARA_GBECSR6_GbE_DIS   BIT3
#define B_PCH_MBARA_GBECSR6_LPLUND    BIT2
#define B_PCH_MBARA_GBECSR6_LPLUD     BIT1
#define R_PCH_MBARA_GBECSR7           0x5400
#define R_PCH_MBARA_GBECSR8           0x5404
#define B_PCH_MBARA_GBECSR8_RAH       0x0000FFFF
#define R_PCH_MBARA_GBECSR9           0x5800
#define B_PCH_MBARA_GBECSR9_APME      BIT0
#endif
