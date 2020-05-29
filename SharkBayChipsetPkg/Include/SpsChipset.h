/** @file

  Definitions of Chipset registers for SPS.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_CHIPSET_H_
#define _SPS_CHIPSET_H_

#include <Library/PciExpressLib.h>
/// PchRegs.h
#define SPS_PCI_BUS_NUMBER_PCH     0

/// PchRegsLpc.h
#define SPS_PCI_DEVICE_NUMBER_PCH_LPC      31
#define SPS_PCI_FUNCTION_NUMBER_PCH_LPC    0

#define R_SPS_LPC_ACPI_BASE            0x40
#define B_SPS_LPC_ACPI_BASE_BAR        0xFFFC

#define R_SPS_ACPI_PM1_STS             0x00
#define B_SPS_ACPI_PM1_STS_WAK         0x8000

#define R_SPS_ACPI_PM1_CNT             0x04
#define V_SPS_ACPI_PM1_CNT_S5          0x00001C00

#define R_SPS_ACPI_PM1_TMR             0x08

#define R_SPS_LPC_RCBA                 0xF0
#define R_SPS_LPC_PMIR                 0xAC
#define B_SPS_LPC_PMIR_CF9LOCK         BIT31           ///< CF9h Lockdown
#define B_SPS_LPC_PMIR_CF9GR           BIT20           ///< CF9h Global Reset

///
/// Reset Generator I/O Port
///
#define R_SPS_RST_CNT                  0xCF9
#define V_SPS_RST_CNT_FULLRESET        0x0E
#define V_SPS_RST_CNT_HARDRESET        0x06
#define V_SPS_RST_CNT_SOFTRESET        0x04
#define V_SPS_RST_CNT_HARDSTARTSTATE   0x02

///
///  PchRegsRcrb.h
///
#define R_SPS_RCRB_PRSTS               0x3310
#define B_SPS_RCRB_PRSTS_ME_WAKE_STS   BIT0

#define R_SPS_RCRB_HPTC                0x3404                      ///< High Performance Timer Configuration
#define B_SPS_RCRB_HPTC_AE             BIT7                        ///< Address enable
#define B_SPS_RCRB_HPTC_AS             (BIT1 | BIT0)               ///< Address selection

#define R_SPS_RCRB_FDSW                0x3420                      ///< Function Disable SUS well
#define B_SPS_RCRB_FDSW_FDSWL          BIT7                        ///< Function Disable SUS well lockdown
#define R_SPS_RCRB_FD2                 0x3428                      ///< Function Disable 2
#define B_SPS_RCRB_FD2_KTD             BIT4                        ///< KT Disable
#define B_SPS_RCRB_FD2_IRERD           BIT3                        ///< IDE-R Disable
#define B_SPS_RCRB_FD2_MEI2D           BIT2                        ///< Intel MEI #2 Disable
#define B_SPS_RCRB_FD2_MEI1D           BIT1                        ///< Intel MEI #1 Disable
#define B_SPS_RCRB_FD2_DBDFEN          BIT0                        ///< Display BDF Enable


///
///  PchRegsSpi.h
///
//
// SPI Host Interface Registers
//
#define R_SPS_RCRB_SPI_BASE            0x3800                        ///< Base address of the SPI host interface registers
#define R_SPS_SPI_HSFS                 (R_SPS_RCRB_SPI_BASE + 0x04)  ///< Hardware Sequencing Flash Status Register(16bits)
#define B_SPS_SPI_HSFS_FLOCKDN         BIT15                         ///< Flash Configuration Lock-Down
#define R_SPS_SPI_FRAP                 (R_SPS_RCRB_SPI_BASE + 0x50)  ///< SPI Flash Regions Access Permisions Register
#define B_SPS_SPI_FRAP_BRWA_MASK       0x0000FF00                    ///< BIOS Regsion Write Access MASK, Region0~7 - 0: Flash Descriptor; 1: BIOS; 2: ME; 3: GbE; 4: PDR; 5: DER 6: ...
#define B_SPS_SPI_FRAP_BRWA_DER        BIT13                         ///< Region write access for Region5 DER (Device Expansion Region)
#define B_SPS_SPI_FRAP_BRWA_PDR        BIT12                         ///< Region write access for Region4 PDR (Platform Data Region)
#define B_SPS_SPI_FRAP_BRWA_GBE        BIT11                         ///< Region write access for Region3 GbE
#define B_SPS_SPI_FRAP_BRWA_ME         BIT10                         ///< Region write access for Region2 ME
#define B_SPS_SPI_FRAP_BRWA_BIOS       BIT9                          ///< Region write access for Region1 BIOS
#define B_SPS_SPI_FRAP_BRWA_FLASHD     BIT8                          ///< Region write access for Region0 Flash Descriptor
#define B_SPS_SPI_FRAP_BRRA_MASK       0x000000FF                    ///< BIOS Regsion Read Access MASK, Region0~7 - 0: Flash Descriptor; 1: BIOS; 2: ME; 3: GbE; 4: PDR; 5: DER 6: ...
#define B_SPS_SPI_FRAP_BRRA_DER       BIT5                          ///< Region read access for Region5 DER (Device Expansion Region)
#define B_SPS_SPI_FRAP_BRRA_PDR       BIT4                          ///< Region read access for Region4 PDR (Platform Data Region)
#define B_SPS_SPI_FRAP_BRRA_GBE       BIT3                          ///< Region read access for Region3 GbE
#define B_SPS_SPI_FRAP_BRRA_ME        BIT2                          ///< Region read access for Region2 ME
#define B_SPS_SPI_FRAP_BRRA_BIOS      BIT1                          ///< Region read access for Region1 BIOS
#define B_SPS_SPI_FRAP_BRRA_FLASHD    BIT0                          ///< Region read access for Region0 Flash Descriptor
#define R_SPS_SPI_FREG0_FLASHD         (R_SPS_RCRB_SPI_BASE + 0x54)  ///< Flash Region 0(Flash Descriptor)(32bits)
#define B_SPS_SPI_FREG0_LIMIT_MASK     0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_SPS_SPI_FREG0_BASE_MASK      0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]

#define R_SPS_SPI_FREG2_ME             (R_SPS_RCRB_SPI_BASE + 0x5C)  ///< Flash Region 2(ME)(32bits)
#define B_SPS_SPI_FREG2_LIMIT_MASK     0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_SPS_SPI_FREG2_BASE_MASK      0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]
#define R_SPS_SPI_FREG3_GBE           (R_SPS_RCRB_SPI_BASE + 0x60)  ///< Flash Region 3(GbE)(32bits)
#define B_SPS_SPI_FREG3_LIMIT_MASK    0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_SPS_SPI_FREG3_BASE_MASK     0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]
#define R_SPS_SPI_FREG4_PLATFORM_DATA (R_SPS_RCRB_SPI_BASE + 0x64)  ///< Flash Region 4(Platform Data)(32bits)
#define B_SPS_SPI_FREG4_LIMIT_MASK    0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_SPS_SPI_FREG4_BASE_MASK     0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]
#define R_SPS_SPI_FREG5_DER           (R_SPS_RCRB_SPI_BASE + 0x68)  ///< Flash Region 5(Device Expansion Region)(32bits)
#define B_SPS_SPI_FREG5_LIMIT_MASK    0x7FFF0000                    ///< Size, [30:16] here represents limit[26:12]
#define B_SPS_SPI_FREG5_BASE_MASK     0x00007FFF                    ///< Base, [14:0]  here represents base [26:12]

#define R_SPS_SPI_PR0                  (R_SPS_RCRB_SPI_BASE + 0x74)  ///< Protected Region 0 Register
#define B_SPS_SPI_PR0_WPE              BIT31                         ///< Write Protection Enable
#define B_SPS_SPI_PR0_PRL_MASK         0x7FFF0000                    ///< Protected Range Limit Mask, [30:16] here represents upper limit of address [26:12]
#define B_SPS_SPI_PR0_PRB_MASK         0x00007FFF                    ///< Protected Range Base Mask, [14:0] here represents base limit of address [26:12]

#define R_SPS_SPI_PR4                  (R_SPS_RCRB_SPI_BASE + 0x84)  ///< Protected Region 4 Register

#define R_SPS_SPI_FDOC                 (R_SPS_RCRB_SPI_BASE + 0xB0)  ///< Flash Descriptor Observability Control Register(32 bits)
#define V_SPS_SPI_FDOC_FDSS_FSDM       0x0000                        ///< Flash Signature and Descriptor Map
#define V_SPS_SPI_FDOC_FDSS_COMP       0x1000                        ///< Component
#define R_SPS_SPI_FDOD                 (R_SPS_RCRB_SPI_BASE + 0xB4)  ///< Flash Descriptor Observability Data Register(32 bits)

#define R_SPS_SPI_FDBAR_FLVALSIG       0x00                          ///< Flash Valid Signature
#define V_SPS_SPI_FDBAR_FLVALSIG       0x0FF0A55A
#define R_SPS_SPI_FDBAR_FLASH_MAP0     0x04
#define B_SPS_SPI_FDBAR_NC             0x00000300                    ///< Number Of Components
#define V_SPS_SPI_FDBAR_NC_1           0x00000000
#define V_SPS_SPI_FDBAR_NC_2           0x00000100

#define R_SPS_SPI_FCBA_FLCOMP          0x00                          ///< Flash Components Register

#define B_SPS_SPI_FLCOMP_COMP2_MASK    0xF0                          ///< Flash Component 2 Size MASK
#define V_SPS_SPI_FLCOMP_COMP2_512KB   0x00
#define V_SPS_SPI_FLCOMP_COMP2_1MB     0x10
#define V_SPS_SPI_FLCOMP_COMP2_2MB     0x20
#define V_SPS_SPI_FLCOMP_COMP2_4MB     0x30
#define V_SPS_SPI_FLCOMP_COMP2_8MB     0x40
#define V_SPS_SPI_FLCOMP_COMP2_16MB    0x50
#define B_SPS_SPI_FLCOMP_COMP1_MASK    0x0F                          ///< Flash Component 1 Size MASK
#define V_SPS_SPI_FLCOMP_COMP1_512KB   0x00
#define V_SPS_SPI_FLCOMP_COMP1_1MB     0x01
#define V_SPS_SPI_FLCOMP_COMP1_2MB     0x02
#define V_SPS_SPI_FLCOMP_COMP1_4MB     0x03
#define V_SPS_SPI_FLCOMP_COMP1_8MB     0x04
#define V_SPS_SPI_FLCOMP_COMP1_16MB    0x05

#define SpsPchLpcAddress(Register)     (PCI_EXPRESS_LIB_ADDRESS (SPS_PCI_BUS_NUMBER_PCH, SPS_PCI_DEVICE_NUMBER_PCH_LPC, SPS_PCI_FUNCTION_NUMBER_PCH_LPC, Register))

#define SpsPchLpcRead32(Register)       (PciExpressRead32 (SpsPchLpcAddress (Register)))
#define SpsPchLpcOr32(Register, OrData) (PciExpressOr32 (SpsPchLpcAddress (Register), OrData))

#define SPS_RCRB_BASE                  ((PciExpressRead32 (SpsPchLpcAddress (R_SPS_LPC_RCBA))) & (~BIT0))
#define SpsRcrbRead16(Register)        (MmioRead16 ((SPS_RCRB_BASE + Register)))
#define SpsRcrbWrite16(Register, Data) (MmioWrite16 ((SPS_RCRB_BASE + Register), ((UINT16)Data)))
#define SpsRcrbAnd16(Register, Data)   (MmioAnd16 ((SPS_RCRB_BASE + Register), ((UINT16)Data)))
#define SpsRcrbOr16(Register, Data)    (MmioOr16 ((SPS_RCRB_BASE + Register), ((UINT16)Data)))

#define SpsRcrbRead32(Register)        (MmioRead32 ((SPS_RCRB_BASE + Register)))
#define SpsRcrbWrite32(Register, Data) (MmioWrite32 ((SPS_RCRB_BASE + Register), ((UINT32)Data)))
#define SpsRcrbAnd32(Register, Data)   (MmioAnd32 ((SPS_RCRB_BASE + Register), ((UINT32)Data)))
#define SpsRcrbOr32(Register, Data)    (MmioOr32 ((SPS_RCRB_BASE + Register), ((UINT32)Data)))

#endif
