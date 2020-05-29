/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchRegsSmbus.h

@brief
  Register names for PCH Smbus Device.

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
#ifndef _PCH_REGS_SMBUS_H_
#define _PCH_REGS_SMBUS_H_

//
// SMBus Controller Registers (D31:F3)
//
#define PCI_DEVICE_NUMBER_PCH_SMBUS   31
#define PCI_FUNCTION_NUMBER_PCH_SMBUS 3
#define R_PCH_SMBUS_VENDOR_ID         0x00
#define V_PCH_SMBUS_VENDOR_ID         V_PCH_INTEL_VENDOR_ID
#define R_PCH_SMBUS_DEVICE_ID         0x02
#define V_PCH_LPTH_SMBUS_DEVICE_ID     0x8C22
#define V_PCH_LPTLP_SMBUS_DEVICE_ID    0x9C22
#define R_PCH_SMBUS_PCICMD            0x04
#define B_PCH_SMBUS_PCICMD_INTR_DIS   BIT10
#define B_PCH_SMBUS_PCICMD_FBE        BIT9
#define B_PCH_SMBUS_PCICMD_SERR_EN    BIT8
#define B_PCH_SMBUS_PCICMD_WCC        BIT7
#define B_PCH_SMBUS_PCICMD_PER        BIT6
#define B_PCH_SMBUS_PCICMD_VPS        BIT5
#define B_PCH_SMBUS_PCICMD_PMWE       BIT4
#define B_PCH_SMBUS_PCICMD_SCE        BIT3
#define B_PCH_SMBUS_PCICMD_BME        BIT2
#define B_PCH_SMBUS_PCICMD_MSE        BIT1
#define B_PCH_SMBUS_PCICMD_IOSE       BIT0
#define R_PCH_SMBUS_PCISTS            0x06
#define B_PCH_SMBUS_PCISTS_DPE        BIT15
#define B_PCH_SMBUS_PCISTS_SSE        BIT14
#define B_PCH_SMBUS_PCISTS_RMA        BIT13
#define B_PCH_SMBUS_PCISTS_RTA        BIT12
#define B_PCH_SMBUS_PCISTS_STA        BIT11
#define B_PCH_SMBUS_PCISTS_DEVT       (BIT10 | BIT9)
#define B_PCH_SMBUS_PCISTS_DPED       BIT8
#define B_PCH_SMBUS_PCISTS_FB2BC      BIT7
#define B_PCH_SMBUS_PCISTS_UDF        BIT6
#define B_PCH_SMBUS_PCISTS_66MHZ_CAP  BIT5
#define B_PCH_SMBUS_PCISTS_CAP_LIST   BIT4
#define B_PCH_SMBUS_PCISTS_INTS       BIT3
#define R_PCH_SMBUS_RID               0x08
#define B_PCH_SMBUS_RID               0xFF
#define R_PCH_SMBUS_SCC               0x0A
#define V_PCH_SMBUS_SCC               0x05
#define R_PCH_SMBUS_BCC               0x0B
#define V_PCH_SMBUS_BCC               0x0C
#define R_PCH_SMBUS_BAR0              0x10
#define B_PCH_SMBUS_BAR0_BAR          0xFFFFFF00
#define B_PCH_SMBUS_BAR0_PREF         BIT3
#define B_PCH_SMBUS_BAR0_ADDRNG       (BIT2 | BIT1)
#define B_PCH_SMBUS_BAR0_MSI          BIT0
#define R_PCH_SMBUS_BAR1              0x14
#define B_PCH_SMBUS_BAR1_BAR          0xFFFFFFFF
#define R_PCH_SMBUS_BASE              0x20
#define V_PCH_SMBUS_BASE_SIZE         (1 << 5)
#define B_PCH_SMBUS_BASE_BAR          0x0000FFE0
#define R_PCH_SMBUS_SVID              0x2C
#define B_PCH_SMBUS_SVID              0xFFFF
#define R_PCH_SMBUS_SID               0x2E
#define B_PCH_SMBUS_SID               0xFFFF
#define R_PCH_SMBUS_INT_LN            0x3C
#define B_PCH_SMBUS_INT_LN            0xFF
#define R_PCH_SMBUS_INT_PN            0x3D
#define B_PCH_SMBUS_INT_PN            0xFF
#define R_PCH_SMBUS_HOSTC             0x40
#define B_PCH_SMBUS_HOSTC_SPDWD       BIT4
#define B_PCH_SMBUS_HOSTC_SSRESET     BIT3
#define B_PCH_SMBUS_HOSTC_I2C_EN      BIT2
#define B_PCH_SMBUS_HOSTC_SMI_EN      BIT1
#define B_PCH_SMBUS_HOSTC_HST_EN      BIT0

//
// SMBus I/O Registers
//
#define R_PCH_SMBUS_HSTS                  0x00  ///< Host Status Register R/W
#define B_PCH_SMBUS_HBSY                  0x01
#define B_PCH_SMBUS_INTR                  0x02
#define B_PCH_SMBUS_DERR                  0x04
#define B_PCH_SMBUS_BERR                  0x08
#define B_PCH_SMBUS_FAIL                  0x10
#define B_PCH_SMBUS_SMBALERT_STS          0x20
#define B_PCH_SMBUS_IUS                   0x40
#define B_PCH_SMBUS_BYTE_DONE_STS         0x80
#define B_PCH_SMBUS_ERROR                 (B_PCH_SMBUS_DERR | B_PCH_SMBUS_BERR | B_PCH_SMBUS_FAIL)
#define B_PCH_SMBUS_HSTS_ALL              0xFF
#define R_PCH_SMBUS_HCTL                  0x02  ///< Host Control Register R/W
#define B_PCH_SMBUS_INTREN                0x01
#define B_PCH_SMBUS_KILL                  0x02
#define B_PCH_SMBUS_SMB_CMD               0x1C
#define V_PCH_SMBUS_SMB_CMD_QUICK         0x00
#define V_PCH_SMBUS_SMB_CMD_BYTE          0x04
#define V_PCH_SMBUS_SMB_CMD_BYTE_DATA     0x08
#define V_PCH_SMBUS_SMB_CMD_WORD_DATA     0x0C
#define V_PCH_SMBUS_SMB_CMD_PROCESS_CALL  0x10
#define V_PCH_SMBUS_SMB_CMD_BLOCK         0x14
#define V_PCH_SMBUS_SMB_CMD_IIC_READ      0x18
#define V_PCH_SMBUS_SMB_CMD_BLOCK_PROCESS 0x1C
#define B_PCH_SMBUS_LAST_BYTE             0x20
#define B_PCH_SMBUS_START                 0x40
#define B_PCH_SMBUS_PEC_EN                0x80
#define R_PCH_SMBUS_HCMD                  0x03  ///< Host Command Register R/W
#define R_PCH_SMBUS_TSA                   0x04  ///< Transmit Slave Address Register R/W
#define B_PCH_SMBUS_RW_SEL                0x01
#define B_PCH_SMBUS_READ                  0x01  // RW
#define B_PCH_SMBUS_WRITE                 0x00  // RW
#define B_PCH_SMBUS_ADDRESS               0xFE
#define R_PCH_SMBUS_HD0                   0x05  ///< Data 0 Register R/W
#define R_PCH_SMBUS_HD1                   0x06  ///< Data 1 Register R/W
#define R_PCH_SMBUS_HBD                   0x07  ///< Host Block Data Register R/W
#define R_PCH_SMBUS_PEC                   0x08  ///< Packet Error Check Data Register R/W
#define R_PCH_SMBUS_RSA                   0x09  ///< Receive Slave Address Register R/W
#define B_PCH_SMBUS_SLAVE_ADDR            0x7F
#define R_PCH_SMBUS_SD                    0x0A  ///< Receive Slave Data Register R/W
#define R_PCH_SMBUS_AUXS                  0x0C  ///< Auxiliary Status Register R/WC
#define B_PCH_SMBUS_CRCE                  0x01
#define B_PCH_SMBUS_STCO                  0x02  ///< SMBus TCO Mode
#define R_PCH_SMBUS_AUXC                  0x0D  ///< Auxiliary Control Register R/W
#define B_PCH_SMBUS_AAC                   0x01
#define B_PCH_SMBUS_E32B                  0x02
#define R_PCH_SMBUS_SMLC                  0x0E  ///< SMLINK Pin Control Register R/W
#define B_PCH_SMBUS_SMLINK0_CUR_STS       0x01
#define B_PCH_SMBUS_SMLINK1_CUR_STS       0x02
#define B_PCH_SMBUS_SMLINK_CLK_CTL        0x04
#define R_PCH_SMBUS_SMBC                  0x0F  ///< SMBus Pin Control Register R/W
#define B_PCH_SMBUS_SMBCLK_CUR_STS        0x01
#define B_PCH_SMBUS_SMBDATA_CUR_STS       0x02
#define B_PCH_SMBUS_SMBCLK_CTL            0x04
#define R_PCH_SMBUS_SSTS                  0x10  ///< Slave Status Register R/WC
#define B_PCH_SMBUS_HOST_NOTIFY_STS       0x01
#define R_PCH_SMBUS_SCMD                  0x11  ///< Slave Command Register R/W
#define B_PCH_SMBUS_HOST_NOTIFY_INTREN    0x01
#define B_PCH_SMBUS_HOST_NOTIFY_WKEN      0x02
#define B_PCH_SMBUS_SMBALERT_DIS          0x04
#define R_PCH_SMBUS_NDA                   0x14  ///< Notify Device Address Register RO
#define B_PCH_SMBUS_DEVICE_ADDRESS        0xFE
#define R_PCH_SMBUS_NDLB                  0x16  ///< Notify Data Low Byte Register RO
#define R_PCH_SMBUS_NDHB                  0x17  ///< Notify Data High Byte Register RO
#endif
