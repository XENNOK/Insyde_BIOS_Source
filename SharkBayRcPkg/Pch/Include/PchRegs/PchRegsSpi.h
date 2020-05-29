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
  PchRegsSpi.h

@brief
  Register names for PCH SPI device.

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
#ifndef _PCH_REGS_SPI_H_
#define _PCH_REGS_SPI_H_

//
// SPI Host Interface Registers
//
#define R_PCH_RCRB_SPI_BASE           0x3800                        ///< Base address of the SPI host interface registers
#define R_PCH_SPI_BFPR                (R_PCH_RCRB_SPI_BASE + 0x00)  ///< BIOS Flash Primary Region Register(32bits), which is RO and contains the same value from FREG1
#define R_PCH_SPI_HSFS                (R_PCH_RCRB_SPI_BASE + 0x04)  ///< Hardware Sequencing Flash Status Register(16bits)
#define B_PCH_SPI_HSFS_FLOCKDN        BIT15                         ///< Flash Configuration Lock-Down
#define B_PCH_SPI_HSFS_FDV            BIT14                         ///< Flash Descriptor Valid, once valid software can use hareware sequencing regs
#define B_PCH_SPI_HSFS_FDOPSS         BIT13                         ///< Flash Descriptor Override Pin-Strap Status, 0: The Flash Descriptor Security Override / Intel
                                                                    ///< ME Debug Mode strap is set via external pull-up on HDA_SDO; 1: No override.
#define B_PCH_SPI_PRR3PRR4_LOCKDN     BIT12                         ///< PRR3 PRR4 Lock-Down
#define B_PCH_SPI_HSFS_SCIP           BIT5                          ///< SPI cyble in progress
#define B_PCH_SPI_HSFS_BERASE_MASK    (BIT4 | BIT3)                 ///< Block/Sector Erase Size
#define V_PCH_SPI_HSFS_BERASE_256B    0x00                          ///< Block/Sector = 256 Bytes
#define V_PCH_SPI_HSFS_BERASE_4K      0x01                          ///< Block/Sector = 4K Bytes
#define V_PCH_SPI_HSFS_BERASE_8K      0x10                          ///< Block/Sector = 8K Bytes
#define V_PCH_SPI_HSFS_BERASE_64K     0x11                          ///< Block/Sector = 64K Bytes
#define B_PCH_SPI_HSFS_AEL            BIT2                          ///< Access Error Log
#define B_PCH_SPI_HSFS_FCERR          BIT1                          ///< Flash Cycle Error
#define B_PCH_SPI_HSFS_FDONE          BIT0                          ///< Flash Cycle Done
#define R_PCH_SPI_HSFC                (R_PCH_RCRB_SPI_BASE + 0x06)  ///< Hardware Sequencing Flash Control Register(16bits)
#define B_PCH_SPI_HSFC_FSMIE          BIT15                         ///< Flash SPI SMI# Enable
#define B_PCH_SPI_HSFC_FDBC_MASK      0x3F00                        ///< Flash Data Byte Count ( <= 64), Count = (Value in this field) + 1.
#define B_PCH_SPI_HSFC_FCYCLE_MASK    0x0006                        ///< Flash Cycle.
#define V_PCH_SPI_HSFC_FCYCLE_READ    0                             ///< Flash Cycle Read
#define V_PCH_SPI_HSFC_FCYCLE_WRITE   2                             ///< Flash Cycle Write
#define V_PCH_SPI_HSFC_FCYCLE_ERASE   3                             ///< Flash Cycle Block Erase
#define B_PCH_SPI_HSFC_FCYCLE_FGO     BIT0                          ///< Flash Cycle Go.
#define R_PCH_SPI_FADDR               (R_PCH_RCRB_SPI_BASE + 0x08)  ///< SPI Flash Address
#define B_PCH_SPI_FADDR_MASK          0x07FFFFFF                    ///< SPI Flash Address Mask (0~26bit)
#define R_PCH_SPI_FDATA00             (R_PCH_RCRB_SPI_BASE + 0x10)  ///< SPI Data 00 (32 bits)
#define R_PCH_SPI_FDATA01             (R_PCH_RCRB_SPI_BASE + 0x14)  ///< SPI Data 01
#define R_PCH_SPI_FDATA02             (R_PCH_RCRB_SPI_BASE + 0x18)  ///< SPI Data 02
#define R_PCH_SPI_FDATA03             (R_PCH_RCRB_SPI_BASE + 0x1C)  ///< SPI Data 03
#define R_PCH_SPI_FDATA04             (R_PCH_RCRB_SPI_BASE + 0x20)  ///< SPI Data 04
#define R_PCH_SPI_FDATA05             (R_PCH_RCRB_SPI_BASE + 0x24)  ///< SPI Data 05
#define R_PCH_SPI_FDATA06             (R_PCH_RCRB_SPI_BASE + 0x28)  ///< SPI Data 06
#define R_PCH_SPI_FDATA07             (R_PCH_RCRB_SPI_BASE + 0x2C)  ///< SPI Data 07
#define R_PCH_SPI_FDATA08             (R_PCH_RCRB_SPI_BASE + 0x30)  ///< SPI Data 08
#define R_PCH_SPI_FDATA09             (R_PCH_RCRB_SPI_BASE + 0x34)  ///< SPI Data 09
#define R_PCH_SPI_FDATA10             (R_PCH_RCRB_SPI_BASE + 0x38)  ///< SPI Data 10
#define R_PCH_SPI_FDATA11             (R_PCH_RCRB_SPI_BASE + 0x3C)  ///< SPI Data 11
#define R_PCH_SPI_FDATA12             (R_PCH_RCRB_SPI_BASE + 0x40)  ///< SPI Data 12
#define R_PCH_SPI_FDATA13             (R_PCH_RCRB_SPI_BASE + 0x44)  ///< SPI Data 13
#define R_PCH_SPI_FDATA14             (R_PCH_RCRB_SPI_BASE + 0x48)  ///< SPI Data 14
#define R_PCH_SPI_FDATA15             (R_PCH_RCRB_SPI_BASE + 0x4C)  ///< SPI Data 15
#define R_PCH_SPI_FRAP                (R_PCH_RCRB_SPI_BASE + 0x50)  ///< SPI Flash Regions Access Permisions Register
#define B_PCH_SPI_FRAP_BMWAG_MASK     0xFF000000                    ///< Master Write Access Grant MASK
#define B_PCH_SPI_FRAP_BMWAG_GBE      BIT27                         ///< Master write access grant for Host CPU/GbE
#define B_PCH_SPI_FRAP_BMWAG_ME       BIT26                         ///< Master write access grant for ME
#define B_PCH_SPI_FRAP_BMWAG_BIOS     BIT25                         ///< Master write access grant for Host CPU/BIOS
#define B_PCH_SPI_FRAP_BMRAG_MASK     0x00FF0000                    ///< Master Write Access Grant MASK
#define B_PCH_SPI_FRAP_BMRAG_GBE      BIT19                         ///< Master write access grant for Host CPU/GbE
#define B_PCH_SPI_FRAP_BMRAG_ME       BIT18                         ///< Master write access grant for ME
#define B_PCH_SPI_FRAP_BMRAG_BIOS     BIT17                         ///< Master write access grant for Host CPU/BIOS
#define B_PCH_SPI_FRAP_BRWA_MASK      0x0000FF00                    ///< BIOS Regsion Write Access MASK, Region0~7 - 0: Flash Descriptor; 1: BIOS; 2: ME; 3: GbE; 4: ...
#define B_PCH_SPI_FRAP_BRWA_GBE       BIT11                         ///< Region write access for Region3 GbE
#define B_PCH_SPI_FRAP_BRWA_ME        BIT10                         ///< Region write access for Region2 ME
#define B_PCH_SPI_FRAP_BRWA_BIOS      BIT9                          ///< Region write access for Region1 BIOS
#define B_PCH_SPI_FRAP_BRWA_FLASHD    BIT8                          ///< Region write access for Region0 Flash Descriptor
#define B_PCH_SPI_FRAP_BRRA_MASK      0x000000FF                    ///< BIOS Regsion Read Access MASK, Region0~7 - 0: Flash Descriptor; 1: BIOS; 2: ME; 3: GbE; 4: ...
#define B_PCH_SPI_FRAP_BRRA_GBE       BIT3                          ///< Region read access for Region3 GbE
#define B_PCH_SPI_FRAP_BRRA_ME        BIT2                          ///< Region read access for Region2 ME
#define B_PCH_SPI_FRAP_BRRA_BIOS      BIT1                          ///< Region read access for Region1 BIOS
#define B_PCH_SPI_FRAP_BRRA_FLASHD    BIT0                          ///< Region read access for Region0 Flash Descriptor
#define R_PCH_SPI_FREG0_FLASHD        (R_PCH_RCRB_SPI_BASE + 0x54)  ///< Flash Region 0(Flash Descriptor)(32bits)
#define B_PCH_SPI_FREG0_LIMIT_MASK    0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_PCH_SPI_FREG0_BASE_MASK     0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]
#define R_PCH_SPI_FREG1_BIOS          (R_PCH_RCRB_SPI_BASE + 0x58)  ///< Flash Region 1(BIOS)(32bits)
#define B_PCH_SPI_FREG1_LIMIT_MASK    0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_PCH_SPI_FREG1_BASE_MASK     0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]
#define R_PCH_SPI_FREG2_ME            (R_PCH_RCRB_SPI_BASE + 0x5C)  ///< Flash Region 2(ME)(32bits)
#define B_PCH_SPI_FREG2_LIMIT_MASK    0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_PCH_SPI_FREG2_BASE_MASK     0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]
#define R_PCH_SPI_FREG3_GBE           (R_PCH_RCRB_SPI_BASE + 0x60)  ///< Flash Region 3(GbE)(32bits)
#define B_PCH_SPI_FREG3_LIMIT_MASK    0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_PCH_SPI_FREG3_BASE_MASK     0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]
#define R_PCH_SPI_FREG4_PLATFORM_DATA (R_PCH_RCRB_SPI_BASE + 0x64)  ///< Flash Region 4(Platform Data)(32bits)
#define B_PCH_SPI_FREG4_LIMIT_MASK    0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_PCH_SPI_FREG4_BASE_MASK     0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]
#define R_PCH_SPI_PR0                 (R_PCH_RCRB_SPI_BASE + 0x74)  ///< Protected Region 0 Register
#define B_PCH_SPI_PR0_WPE             BIT31                         ///< Write Protection Enable
#define B_PCH_SPI_PR0_PRL_MASK        0x7FFF0000                    ///< Protected Range Limit Mask, [30:16] here represents upper limit of address [26:12]
#define B_PCH_SPI_PR0_RPE             BIT15                         ///< Read Protection Enable
#define B_PCH_SPI_PR0_PRB_MASK        0x00007FFF                    ///< Protected Range Base Mask, [14:0] here represents base limit of address [26:12]
#define R_PCH_SPI_PR1                 (R_PCH_RCRB_SPI_BASE + 0x78)  ///< Protected Region 1 Register
#define B_PCH_SPI_PR1_WPE             BIT31                         ///< Write Protection Enable
#define B_PCH_SPI_PR1_PRL_MASK        0x7FFF0000                    ///< Protected Range Limit Mask
#define B_PCH_SPI_PR1_RPE             BIT15                         ///< Read Protection Enable
#define B_PCH_SPI_PR1_PRB_MASK        0x00007FFF                    ///< Protected Range Base Mask
#define R_PCH_SPI_PR2                 (R_PCH_RCRB_SPI_BASE + 0x7C)  ///< Protected Region 2 Register
#define B_PCH_SPI_PR2_WPE             BIT31                         ///< Write Protection Enable
#define B_PCH_SPI_PR2_PRL_MASK        0x7FFF0000                    ///< Protected Range Limit Mask
#define B_PCH_SPI_PR2_RPE             BIT15                         ///< Read Protection Enable
#define B_PCH_SPI_PR2_PRB_MASK        0x00007FFF                    ///< Protected Range Base Mask
#define R_PCH_SPI_PR3                 (R_PCH_RCRB_SPI_BASE + 0x80)  ///< Protected Region 3 Register
#define B_PCH_SPI_PR3_WPE             BIT31                         ///< Write Protection Enable
#define B_PCH_SPI_PR3_PRL_MASK        0x7FFF0000                    ///< Protected Range Limit Mask
#define B_PCH_SPI_PR3_RPE             BIT15                         ///< Read Protection Enable
#define B_PCH_SPI_PR3_PRB_MASK        0x00007FFF                    ///< Protected Range Base Mask
#define R_PCH_SPI_PR4                 (R_PCH_RCRB_SPI_BASE + 0x84)  ///< Protected Region 4 Register
#define B_PCH_SPI_PR4_WPE             BIT31                         ///< Write Protection Enable
#define B_PCH_SPI_PR4_PRL_MASK        0x7FFF0000                    ///< Protected Range Limit Mask
#define B_PCH_SPI_PR4_RPE             BIT15                         ///< Read Protection Enable
#define B_PCH_SPI_PR4_PRB_MASK        0x00007FFF                    ///< Protected Range Base Mask
#define R_PCH_SPI_SSFS                (R_PCH_RCRB_SPI_BASE + 0x90)  ///< Software Sequencing Flash Status Register(8bits)
#define B_PCH_SPI_SSFS_FRS            BIT7                          ///< Fast Read Supported
#define B_PCH_SPI_SSFS_DOFRS          BIT6                          ///< Dual Output Fast Read Supported
#define B_PCH_SPI_SSFS_AEL            BIT4                          ///< Access Error Log
#define B_PCH_SPI_SSFS_FCERR          BIT3                          ///< Flash Cycle Error
#define B_PCH_SPI_SSFS_CDS            BIT2                          ///< Cycle Done Status
#define B_PCH_SPI_SSFS_SCIP           BIT0                          ///< SPI Cycle in Progress
#define R_PCH_SPI_SSFC                (R_PCH_RCRB_SPI_BASE + 0x91)  ///< Software Sequencing Flash Control(24bits)
#define B_PCH_SPI_SSFC_SCF_MASK       (BIT18 | BIT17 | BIT16)       ///< SPI Cycle Frequency
#define V_PCH_SPI_SSFC_SCF_20MHZ      0                             ///< SPI Cycle Frequency = 20MHz
#define V_PCH_SPI_SSFC_SCF_33MHZ      1                             ///< SPI Cycle Frequency = 33MHz
#define V_PCH_SPI_SSFC_SCF_50MHZ      4                             ///< SPI Cycle Frequency = 50MHz
#define B_PCH_SPI_SSFC_SME            BIT15                         ///< SPI SMI# Enable
#define B_PCH_SPI_SSFC_DS             BIT14                         ///< SPI Data Cycle
#define B_PCH_SPI_SSFC_DBC_MASK       0x3F00                        ///< SPI Data Byte Count (value here + 1 = count)
#define B_PCH_SPI_SSFC_COP            0x0070                        ///< Cycle Opcode Pointer
#define B_PCH_SPI_SSFC_SPOP           BIT3                          ///< Sequence Prefix Opcode Pointer
#define B_PCH_SPI_SSFC_ACS            BIT2                          ///< Atomic Cycle Sequence
#define B_PCH_SPI_SSFC_SCGO           BIT1                          ///< SPI Cycle Go
#define R_PCH_SPI_PREOP               (R_PCH_RCRB_SPI_BASE + 0x94)  ///< Prefix Opcode Configuration Register(16 bits)
#define B_PCH_SPI_PREOP1_MASK         0xFF00                        ///< Prefix Opcode 1 Mask
#define B_PCH_SPI_PREOP0_MASK         0x00FF                        ///< Prefix Opcode 0 Mask
#define R_PCH_SPI_OPTYPE              (R_PCH_RCRB_SPI_BASE + 0x96)  ///< Opcode Type Configuration
#define B_PCH_SPI_OPTYPE7_MASK        (BIT15 | BIT14)               ///< Opcode Type 7 Mask
#define B_PCH_SPI_OPTYPE6_MASK        (BIT13 | BIT12)               ///< Opcode Type 6 Mask
#define B_PCH_SPI_OPTYPE5_MASK        (BIT11 | BIT10)               ///< Opcode Type 5 Mask
#define B_PCH_SPI_OPTYPE4_MASK        (BIT9 | BIT8)                 ///< Opcode Type 4 Mask
#define B_PCH_SPI_OPTYPE3_MASK        (BIT7 | BIT6)                 ///< Opcode Type 3 Mask
#define B_PCH_SPI_OPTYPE2_MASK        (BIT5 | BIT4)                 ///< Opcode Type 2 Mask
#define B_PCH_SPI_OPTYPE1_MASK        (BIT3 | BIT2)                 ///< Opcode Type 1 Mask
#define B_PCH_SPI_OPTYPE0_MASK        (BIT1 | BIT0)                 ///< Opcode Type 0 Mask
#define V_PCH_SPI_OPTYPE_RDNOADDR     0x00                          ///< Read cycle type without address
#define V_PCH_SPI_OPTYPE_WRNOADDR     0x01                          ///< Write cycle type without address
#define V_PCH_SPI_OPTYPE_RDADDR       0x02                          ///< Address required; Read cycle type
#define V_PCH_SPI_OPTYPE_WRADDR       0x03                          ///< Address required; Write cycle type
#define R_PCH_SPI_OPMENU              (R_PCH_RCRB_SPI_BASE + 0x98)  ///< Opcode Menu Configuration (64bits)
#define R_PCH_SPI_FDOC                (R_PCH_RCRB_SPI_BASE + 0xB0)  ///< Flash Descriptor Observability Control Register(32 bits)
#define B_PCH_SPI_FDOC_FDSS_MASK      (BIT14 | BIT13 | BIT12)       ///< Flash Descritor Section Select
#define V_PCH_SPI_FDOC_FDSS_FSDM      0x0000                        ///< Flash Signature and Descriptor Map
#define V_PCH_SPI_FDOC_FDSS_COMP      0x1000                        ///< Component
#define V_PCH_SPI_FDOC_FDSS_REGN      0x2000                        ///< Region
#define V_PCH_SPI_FDOC_FDSS_MSTR      0x3000                        ///< Master
#define V_PCH_SPI_FDOC_FDSS_PCHS      0x4000                        ///< PCH soft straps
#define V_PCH_SPI_FDOC_FDSS_SFDP      0x5000                        ///< SFDP Parameter Table
#define B_PCH_SPI_FDOC_FDSI_MASK      0x0FFC                        ///< Flash Descriptor Section Index
#define R_PCH_SPI_FDOD                (R_PCH_RCRB_SPI_BASE + 0xB4)  ///< Flash Descriptor Observability Data Register(32 bits)
#define R_PCH_SPI_AFC                 (R_PCH_RCRB_SPI_BASE + 0xC0)  ///< Additional Flash Control Register
#define B_PCH_SPI_AFC_INF_DCGE        (BIT2 | BIT1)                 ///< Flash Controller Interface Dynamic Clock Gating Enable
#define B_PCH_SPI_AFC_CORE_DCGE       BIT0                          ///< Flash Core Dynamic Clock Gating Enable
#define R_PCH_SPI_VSCC0               (R_PCH_RCRB_SPI_BASE + 0xC4)  ///< Vendor Specific Component Capabilities Register(32 bits)
#define B_PCH_SPI_VSCC0_CPPTV         BIT31                         ///< Component Property Parameter Table Valid
#define B_PCH_SPI_VSCC0_VCL           BIT23                         ///< Vendor Component Lock
#define B_PCH_SPI_VSCC0_EO_MASK       0x0000FF00                    ///< Erase Opcode
#define B_PCH_SPI_VSCC0_WEWS          BIT4                          ///< Write Enable on Write Status
#define B_PCH_SPI_VSCC0_WSR           BIT3                          ///< Write Status Required
#define B_PCH_SPI_VSCC0_WG_64B        BIT2                          ///< Write Granularity, 0: 1 Byte; 1: 64 Bytes
#define B_PCH_SPI_VSCC0_BSES_MASK     (BIT1 | BIT0)                 ///< Block/Sector Erase Size
#define V_PCH_SPI_VSCC0_BSES_256B     0x0                           ///< Block/Sector Erase Size = 256 Bytes
#define V_PCH_SPI_VSCC0_BSES_4K       0x1                           ///< Block/Sector Erase Size = 4K Bytes
#define V_PCH_SPI_VSCC0_BSES_8K       0x2                           ///< Block/Sector Erase Szie = 8K Bytes
#define V_PCH_SPI_VSCC0_BSES_64K      0x3                           ///< Block/Sector Erase Size = 64K Bytes
#define R_PCH_SPI_VSCC1               (R_PCH_RCRB_SPI_BASE + 0xC8)  ///< Vendor Specific Component Capabilities Register(32 bits)
#define B_PCH_SPI_VSCC1_CPPTV         BIT31                         ///< Component Property Parameter Table Valid
#define B_PCH_SPI_VSCC1_EO_MASK       0x0000FF00                    ///< Erase Opcode
#define B_PCH_SPI_VSCC1_WEWS          BIT4                          ///< Write Enable on Write Status
#define B_PCH_SPI_VSCC1_WSR           BIT3                          ///< Write Status Required
#define B_PCH_SPI_VSCC1_WG_64B        BIT2                          ///< Write Granularity, 0: 1 Byte; 1: 64 Bytes
#define B_PCH_SPI_VSCC1_BSES_MASK     (BIT1 | BIT0)                 ///< Block/Sector Erase Size
#define V_PCH_SPI_VSCC1_BSES_256B     0x0                           ///< Block/Sector Erase Size = 256 Bytes
#define V_PCH_SPI_VSCC1_BSES_4K       0x1                           ///< Block/Sector Erase Size = 4K Bytes
#define V_PCH_SPI_VSCC1_BSES_8K       0x2                           ///< Block/Sector Erase Size = 8K Bytes
#define V_PCH_SPI_VSCC1_BSES_64K      0x3                           ///< Block/Sector Erase Size = 64K Bytes
#define R_PCH_SPI_PINTX               (R_PCH_RCRB_SPI_BASE + 0xCC)  ///< Parameter Table Index
#define N_PCH_SPI_PINTX_SPT           14
#define V_PCH_SPI_PINTX_SPT_CPT0      0x0                           ///< Component 0 Property Parameter Table
#define V_PCH_SPI_PINTX_SPT_CPT1      0x1                           ///< Component 1 Property Parameter Table
#define N_PCH_SPI_PINTX_HORD          12
#define V_PCH_SPI_PINTX_HORD_SFDP     0x0                           ///< SFDP Header
#define V_PCH_SPI_PINTX_HORD_PT       0x1                           ///< Parameter Table Header
#define V_PCH_SPI_PINTX_HORD_DATA     0x2                           ///< Data
#define R_PCH_SPI_PTDATA              (R_PCH_RCRB_SPI_BASE + 0xD0)  ///< Parameter Table Data
#define R_PCH_SPI_SRDL                (R_PCH_RCRB_SPI_BASE + 0xF0)  ///< Soft Reset Data Lock
#define B_PCH_SPI_SRDL_SSL            BIT0                          ///< Set_Stap Lock
#define R_PCH_SPI_SRDC                (R_PCH_RCRB_SPI_BASE + 0xF4)  ///< Soft Reset Data Control
#define B_PCH_SPI_SRDC_SRDS           BIT0                          ///< Soft Reset Data Select
#define R_PCH_SPI_SRD                 (R_PCH_RCRB_SPI_BASE + 0xF8)  ///< Soft Reset Data
//
// @todo Follow up with EDS owner if it should be 3FFF or FFFF.
//
#define B_PCH_SPI_SRD_SSD             0x0000FFFF                    ///< Set_Stap Data
//
// Flash Descriptor Base Address Region (FDBAR) from Flash Region 0
//
#define R_PCH_SPI_FDBAR_FLVALSIG      0x00                          ///< Flash Valid Signature
#define V_PCH_SPI_FDBAR_FLVALSIG      0x0FF0A55A
#define R_PCH_SPI_FDBAR_FLASH_MAP0    0x04
#define B_PCH_SPI_FDBAR_FCBA          0x000000FF                    ///< Flash Component Base Address
#define B_PCH_SPI_FDBAR_NC            0x00000300                    ///< Number Of Components
#define N_PCH_SPI_FDBAR_NC            0x08                          ///< Number Of Components
#define V_PCH_SPI_FDBAR_NC_1          0x00000000
#define V_PCH_SPI_FDBAR_NC_2          0x00000100
#define B_PCH_SPI_FDBAR_FRBA          0x00FF0000                    ///< Flash Region Base Address
#define B_PCH_SPI_FDBAR_NR            0x07000000                    ///< Number Of Regions
#define R_PCH_SPI_FDBAR_FLASH_MAP1    0x08
#define B_PCH_SPI_FDBAR_FMBA          0x000000FF                    ///< Flash Master Base Address
#define B_PCH_SPI_FDBAR_NM            0x00000700                    ///< Number Of Masters
#define B_PCH_SPI_FDBAR_FPCHSBA       0x00FF0000                    ///< Flash PCH Strap Base Address
#define B_PCH_SPI_FDBAR_PCHSL         0xFF000000                    ///< PCH Strap Length
#define R_PCH_SPI_FDBAR_FLASH_MAP2    0x0C
#define B_PCH_SPI_FDBAR_FPROSBA       0x000000FF                    ///< Flash Processor Strap Base Address
#define B_PCH_SPI_FDBAR_PROSL         0x0000FF00                    ///< PROC Strap Length
//
// Flash Component Base Address (FCBA) from Flash Region 0
//
#define R_PCH_SPI_FCBA_FLCOMP         0x00                          ///< Flash Components Register
#define B_PCH_SPI_FLCOMP_RIDS_FREQ    (BIT29 | BIT28 | BIT27)       ///< Read ID and Read Status Clock Frequency
#define B_PCH_SPI_FLCOMP_WE_FREQ      (BIT26 | BIT25 | BIT24)       ///< Write and Erase Clock Frequency
#define B_PCH_SPI_FLCOMP_FRCF_FREQ    (BIT23 | BIT22 | BIT21)       ///< Fast Read Clock Frequency
#define B_PCH_SPI_FLCOMP_FR_SUP       BIT20                         ///< Fast Read Support.
#define B_PCH_SPI_FLCOMP_RC_FREQ      (BIT19 | BIT18 | BIT17)       ///< Read Clock Frequency.
#define V_PCH_SPI_FLCOMP_FREQ_20MHZ   0x00
#define V_PCH_SPI_FLCOMP_FREQ_33MHZ   0x01
#define V_PCH_SPI_FLCOMP_FREQ_50MHZ   0x04
#define B_PCH_SPI_FLCOMP_COMP2_MASK   0xF0                          ///< Flash Component 2 Size MASK
#define V_PCH_SPI_FLCOMP_COMP2_512KB  0x00
#define V_PCH_SPI_FLCOMP_COMP2_1MB    0x10
#define V_PCH_SPI_FLCOMP_COMP2_2MB    0x20
#define V_PCH_SPI_FLCOMP_COMP2_4MB    0x30
#define V_PCH_SPI_FLCOMP_COMP2_8MB    0x40
#define V_PCH_SPI_FLCOMP_COMP2_16MB   0x50
#define V_PCH_SPI_FLCOMP_COMP2_32MB   0x60
#define V_PCH_SPI_FLCOMP_COMP2_64MB   0x70
#define B_PCH_SPI_FLCOMP_COMP1_MASK   0x0F                          ///< Flash Component 1 Size MASK
#define V_PCH_SPI_FLCOMP_COMP1_512KB  0x00
#define V_PCH_SPI_FLCOMP_COMP1_1MB    0x01
#define V_PCH_SPI_FLCOMP_COMP1_2MB    0x02
#define V_PCH_SPI_FLCOMP_COMP1_4MB    0x03
#define V_PCH_SPI_FLCOMP_COMP1_8MB    0x04
#define V_PCH_SPI_FLCOMP_COMP1_16MB   0x05
#define V_PCH_SPI_FLCOMP_COMP1_32MB   0x06
#define V_PCH_SPI_FLCOMP_COMP1_64MB   0x07
#define V_PCH_SPI_FLCOMP_COMP_512KB   0x80000
//
// Descriptor Upper Map Section from Flash Region 0
//
#define R_PCH_SPI_FLASH_UMAP1         0xEFC                         ///< Flash Upper Map 1
#define B_PCH_SPI_FLASH_UMAP1_VTBA    0x000000FF                    ///< VSCC Table Base Address
#define B_PCH_SPI_FLASH_UMAP1_VTL     0x0000FF00                    ///< VSCC Table Length

#define R_PCH_SPI_VTBA_JID0           0x00                          ///< JEDEC-ID 0 Register
#define S_PCH_SPI_VTBA_JID0           0x04
#define B_PCH_SPI_VTBA_JID0_VID       0x000000FF
#define B_PCH_SPI_VTBA_JID0_DID0      0x0000FF00
#define B_PCH_SPI_VTBA_JID0_DID1      0x00FF0000
#define N_PCH_SPI_VTBA_JID0_DID0      0x08
#define N_PCH_SPI_VTBA_JID0_DID1      0x10
#define R_PCH_SPI_VTBA_VSCC0          0x04
#define S_PCH_SPI_VTBA_VSCC0          0x04
#define R_PCH_SPI_STRP0               0x0                           ///< PCH soft strap 0
#define B_PCH_SPI_STRP0_BBBS          (BIT31 |BIT30 | BIT29)        ///< BIOS Boot-Block size
#define B_PCH_SPI_STRP0_BBBS_64KB     0x00
#define B_PCH_SPI_STRP0_BBBS_128KB    BIT29
#define B_PCH_SPI_STRP0_BBBS_256KB    BIT30
#define B_PCH_SPI_STRP0_BBBS_512KB    (BIT30 | BIT29)
#define B_PCH_SPI_STRP0_BBBS_1MB      BIT31
#define B_PCH_SPI_STRP0_DMI_REQID_DIS BIT24                         ///< DMI RequesterID Check Disable
#define B_PCH_SPI_STRP0_CFG_STRP1     BIT21                         ///< Chipset configuration Softstrap 1
#define B_PCH_SPI_STRP0_LAN_GP12_SEL  BIT20                         ///< LAN PHY Power Control GPIO12 Select
#define B_PCH_SPI_STRP0_SML0FRQ       (BIT15 | BIT14)               ///< SMLink0 Frequency
#define B_PCH_SPI_STRP0_SMB0FRQ       (BIT13 | BIT12)               ///< Intel ME SMBus Frequency
#define B_PCH_SPI_STRP0_SML1FRQ       (BIT11 | BIT10)               ///< SMLink1 Frequency
#define B_PCH_SPI_STRP0_SML1_EN       BIT9                          ///< SMLink1 Enable
#define B_PCH_SPI_STRP0_SML0_EN       BIT8                          ///< SMLink0 Enable
#define B_PCH_SPI_STRP0_SMB_EN        BIT7                          ///< Intel ME SMBus Select
#define B_PCH_SPI_STRP0_CFG_STRP2     BIT1                          ///< Chipset configuration Softstrap 2
#define R_PCH_SPI_STRP1               0x04                          ///< PCH soft strap 1
#define B_PCH_SPI_STRP1_CFG_STRP3     0x0F                          ///< Chipset configuration Softstrap 3
#define R_PCH_SPI_STRP2               0x08                          ///< PCH soft strap 2
#define B_PCH_SPI_STRP2_MESMA         0xFE000000                    ///< ME SMBus Address
#define B_PCH_SPI_STRP2_MESMI2CEN     BIT24                         ///< ME SMBus Address Enable
#define B_PCH_SPI_STRP2_MESMASDA      0xFE00                        ///< ME SMBus Alert Sending Device Address
#define B_PCH_SPI_STRP2_MESMASDEN     BIT8                          ///< ME SMBus Alert Sending Device Address Enable
#define R_PCH_SPI_STRP3               0x0C                          ///< PCH soft strap 3
#define R_PCH_SPI_STRP4               0x10                          ///< PCH soft strap 4
#define B_PCH_SPI_STRP4_GBEPHYSMA     0xFE0000                      ///< GbE PHY SMBus Address
#define B_PCH_SPI_STRP4_GBEMACSMA     0xFE00                        ///< GbE MAC SMBus Address
#define B_PCH_SPI_STRP4_GBEMACSMAEN   BIT8                          ///< Gbe MAC SMBus Address Enable
#define B_PCH_SPI_STRP4_PHYCON        (BIT1 | BIT0)                 ///< Intel PHY Connectivity
#define B_PCH_SPI_STRP4_NO_PHYCON     0x00
#define B_PCH_SPI_STRP4_PHY_ON        0x02
#define R_PCH_SPI_STRP5               0x14                          ///< PCH soft strap 5
#define R_PCH_SPI_STRP6               0x18                          ///< PCH soft strap 6
#define R_PCH_SPI_STRP7               0x1C                          ///< PCH soft strap 7
#define B_PCH_SPI_STRP7_MESMASVID     0xFFFFFFFF                    ///< ME SMBus Subsystem Vendor and Device ID
#define R_PCH_SPI_STRP8               0x20                          ///< PCH soft strap 8
#define R_PCH_SPI_STRP9               0x24                          ///< PCH soft strap 9
#define B_PCH_SPI_STRP9_HOT_SML1_SEL  BIT22                         ///< PCHHOT# or SML1AlERT# Select (0:SML1ALERT#; 1:PCHHOT#)
#define B_PCH_SPI_STRP9_PCIE_SBDE_EN  BIT14                         ///< Subtractive Decode over PCI Express Enabling
#define N_PCH_SPI_STRP9_PCIE_SBDE_EN  14
#define B_PCH_SPI_STRP9_GBE_PCIE_EN   BIT11                         ///< GbE over PCI Express Enabling
#define B_PCH_SPI_STRP9_GBE_PCIE_PSC  (BIT8 | BIT9 | BIT10)         ///< GbE PCI E Port Select
#define N_PCH_SPI_STRP9_GBE_PCIE_PSC  8
#define B_PCH_SPI_STRP9_DMILR         BIT6                          ///< DMI Lane Reversal
#define B_PCH_SPI_STRP9_PCIELR2       BIT5                          ///< PCIe Lane Reversal 2
#define B_PCH_SPI_STRP9_PCIELR1       BIT4                          ///< PCIe Lane Reversal 1
#define B_PCH_SPI_STRP9_PCIEPCS2      BIT3 | BIT2                   ///< PCI Express Port Configuration Strap 2
#define B_PCH_SPI_STRP9_PCIEPCS1      BIT1 | BIT0                   ///< PCI Express Port Configuration Strap 1
#define V_PCH_SPI_STRP9_PCIEPCS_1x4   0x03                          ///< 1x4 Port 1/5 (x4), Ports 2-4/6-8 (disabled)
#define V_PCH_SPI_STRP9_PCIEPCS_2x2   0x02                          ///< 2x2 Port 1/5 (x2), Port 3/7 (x2), Ports 2,4/6,8 (disabled)
#define V_PCH_SPI_STRP9_PCIEPCS_1x2   0x01                          ///< 1x2, 2x1 Port 1/5 (x2), Port 2/6 (disabled), Ports 3,4/7,8 (x1)
#define V_PCH_SPI_STRP9_PCIEPCS_4x1   0x00                          ///< 4x1 Ports 1-4/5-8 (x1)
#define R_PCH_SPI_STRP10              0x28                          ///< PCH soft strap 10
//[-start-130415-IB10920021-add]//
#define B_PCH_SPI_STRP10_DEEP_SX_EN   BIT23                         ///< Deep SX Support (Deep_SX_EN)
//[-end-130415-IB10920021-add]//
#define B_PCH_SPI_STRP10_MER_CL1      BIT21                         ///< ME Reset Capture on CL_RST1
#define B_PCH_SPI_STRP10_ICC_SEL      0x1C0000                      ///< Integrated Clocking Configuration Select
#define B_PCH_SPI_STRP10_CFG_STRP7    BIT16                         ///< Chipset Configuration Softstrap 7
#define B_PCH_SPI_STRP10_MMADDR       0xFE00                        ///< ME Memory-attached Debug Display Device Address
#define B_PCH_SPI_STRP10_MMDDE        BIT8                          ///< ME Memory-attached Debug Display Device Enable
#define B_PCH_SPI_STRP10_VE_EN        BIT3                          ///< 0 - VE disabled; 1 - VE enabled
#define B_PCH_SPI_STRP10_CFG_STRP5    BIT2                          ///< Chipset Configuration Softstrap 5
#define B_PCH_SPI_STRP10_ME_BFlash    BIT1                          ///< ME from Boot Flash
#define R_PCH_SPI_STRP11              0x2C                          ///< PCH soft strap 11
#define B_PCH_SPI_STRP11_SML1I2CA     0xFE000000                    ///< SMLink1 I2C Target Address
#define B_PCH_SPI_STRP11_SML1I2CAEN   BIT24                         ///< SMLink1 I2C Target Address Enable
#define B_PCH_SPI_STRP11_SML1GPA      0xE                           ///< SMLink1 GP Address
#define B_PCH_SPI_STRP11_SML1GPAEN    BIT0                          ///< SMLink1 GP Address Enable
#define R_PCH_SPI_STRP12              0x30
#define R_PCH_SPI_STRP13              0x34
#define R_PCH_SPI_STRP14              0x38
#define R_PCH_SPI_STRP15              0x3C
#define R_PCH_SPI_STRP15_SML1_THRMSEL BIT14                         ///< SMLink1 Thermal Reporting Select
#define B_PCH_SPI_STRP15_T209MIN      (BIT9 | BIT8)                 ///< T209 min Timing
#define B_PCH_SPI_STRP15_IWL_EN       BIT6                          ///< Intel integrated wired LAN Enable
#define B_PCH_SPI_STRP15_CFG_STRP6    (BIT4 | BIT3)                 ///< Chipset Configuration Softstrap 6
#define R_PCH_SPI_STRP17              0x44                          ///< PCH Soft strap 17
#define B_PCH_SPI_STRP17_CLK_MODE     BIT0                          ///< Integrated Clock mode select
#endif
