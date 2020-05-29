/** @file
  Determine if "Boot with no change" is true according to project characteristic.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DebugLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/IoLib.h>

//
// data type definitions
//
#ifndef BIT0
#define BIT0           0x1
#endif
#ifndef BIT1
#define BIT1           0x2
#endif
#ifndef BIT2
#define BIT2           0x4
#endif
#ifndef BIT3
#define BIT3           0x8
#endif
#ifndef BIT4
#define BIT4          0x10
#endif
#ifndef BIT5
#define BIT5          0x20
#endif
#ifndef BIT6
#define BIT6          0x40
#endif
#ifndef BIT7
#define BIT7          0x80
#endif
#ifndef BIT8
#define BIT8         0x100
#endif
#ifndef BIT9
#define BIT9         0x200
#endif
#ifndef BIT10
#define BIT10        0x400
#endif
#ifndef BIT11
#define BIT11        0x800
#endif
#ifndef BIT12
#define BIT12       0x1000
#endif
#ifndef BIT13
#define BIT13       0x2000
#endif
#ifndef BIT14
#define BIT14       0x4000
#endif
#ifndef BIT15
#define BIT15       0x8000
#endif
#ifndef BIT16
#define BIT16      0x10000
#endif
#ifndef BIT17
#define BIT17      0x20000
#endif
#ifndef BIT18
#define BIT18      0x40000
#endif
#ifndef BIT19
#define BIT19      0x80000
#endif
#ifndef BIT20
#define BIT20     0x100000
#endif
#ifndef BIT21
#define BIT21     0x200000
#endif
#ifndef BIT22
#define BIT22     0x400000
#endif
#ifndef BIT23
#define BIT23     0x800000
#endif
#ifndef BIT24
#define BIT24    0x1000000
#endif
#ifndef BIT25
#define BIT25    0x2000000
#endif
#ifndef BIT26
#define BIT26    0x4000000
#endif
#ifndef BIT27
#define BIT27    0x8000000
#endif
#ifndef BIT28
#define BIT28   0x10000000
#endif
#ifndef BIT29
#define BIT29   0x20000000
#endif
#ifndef BIT30
#define BIT30   0x40000000
#endif
#ifndef BIT31
#define BIT31   0x80000000
#endif
#ifndef BIT32
#define BIT32          0x100000000
#endif
#ifndef BIT33
#define BIT33          0x200000000
#endif
#ifndef BIT34
#define BIT34          0x400000000
#endif
#ifndef BIT35
#define BIT35          0x800000000
#endif
#ifndef BIT36
#define BIT36         0x1000000000
#endif
#ifndef BIT37
#define BIT37         0x2000000000
#endif
#ifndef BIT38
#define BIT38         0x4000000000
#endif
#ifndef BIT39
#define BIT39         0x8000000000
#endif
#ifndef BIT40
#define BIT40        0x10000000000
#endif
#ifndef BIT41
#define BIT41        0x20000000000
#endif
#ifndef BIT42
#define BIT42        0x40000000000
#endif
#ifndef BIT43
#define BIT43        0x80000000000
#endif
#ifndef BIT44
#define BIT44       0x100000000000
#endif
#ifndef BIT45
#define BIT45       0x200000000000
#endif
#ifndef BIT46
#define BIT46       0x400000000000
#endif
#ifndef BIT47
#define BIT47       0x800000000000
#endif
#ifndef BIT48
#define BIT48      0x1000000000000
#endif
#ifndef BIT49
#define BIT49      0x2000000000000
#endif
#ifndef BIT50
#define BIT50      0x4000000000000
#endif
#ifndef BIT51
#define BIT51      0x8000000000000
#endif
#ifndef BIT52
#define BIT52     0x10000000000000
#endif
#ifndef BIT53
#define BIT53     0x20000000000000
#endif
#ifndef BIT54
#define BIT54     0x40000000000000
#endif
#ifndef BIT55
#define BIT55     0x80000000000000
#endif
#ifndef BIT56
#define BIT56    0x100000000000000
#endif
#ifndef BIT57
#define BIT57    0x200000000000000
#endif
#ifndef BIT58
#define BIT58    0x400000000000000
#endif
#ifndef BIT59
#define BIT59    0x800000000000000
#endif
#ifndef BIT60
#define BIT60   0x1000000000000000
#endif
#ifndef BIT61
#define BIT61   0x2000000000000000
#endif
#ifndef BIT62
#define BIT62   0x4000000000000000
#endif
#ifndef BIT63
#define BIT63   0x8000000000000000
#endif

#ifndef KILOBIT
#define KILOBIT           0x80
#endif
#ifndef MEGABIT
#define MEGABIT        0x20000
#endif

#ifndef KILOBYTE
#define KILOBYTE         0x400
#endif
#ifndef MEGABYTE
#define MEGABYTE      0x100000
#endif
#ifndef GIGABYTE
#define GIGABYTE    0x40000000
#endif
#ifndef TERABYTE
#define TERABYTE 0x10000000000
#endif

//
// PCI to LPC Bridge Registers (D31:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LPC       31
#define PCI_FUNCTION_NUMBER_PCH_LPC     0

#define PCH_HPET_BDF_MAX                8

typedef enum {
  PchA0         = 0,
  PchA1,
  PchB0,
  PchB1,
  PchB2,
  PchB3,
  PchC0,
  PchC1,
  PchSteppingMax
} PCH_STEPPING;

#define R_PCH_LPC_VENDOR_ID                       0x00
#define V_PCH_LPC_VENDOR_ID                       V_PCH_INTEL_VENDOR_ID
#define R_PCH_LPC_DEVICE_ID                       0x02
//
// Cougarpoint Desktop LPC Device IDs
//
#define V_PCH_LPC_DEVICE_ID_DT_0                  0x1C42          // Desktop Full Featured
#define V_PCH_LPC_DEVICE_ID_DT_1                  0x1C46          // Intel P67 Chipset
#define V_PCH_LPC_DEVICE_ID_DT_2                  0x1C4A          // Intel H67 Chipset
#define V_PCH_LPC_DEVICE_ID_DT_3                  0x1C4C          // Intel Q65 Chipset
#define V_PCH_LPC_DEVICE_ID_DT_4                  0x1C4E          // Intel Q67 Chipset
#define V_PCH_LPC_DEVICE_ID_DT_5                  0x1C50          // Intel B65 Chipset
#define V_PCH_LPC_DEVICE_ID_DT_6                  0x1C5C          // Intel H61 Chipset
#define V_PCH_LPC_DEVICE_ID_DT_7                  0x1C44          // Intel Z68 Chipset
//
// Cougarpoint Mobile LPC Device IDs
//
#define V_PCH_LPC_DEVICE_ID_MB_0                  0x1C43          // Mobile Full Featured
#define V_PCH_LPC_DEVICE_ID_MB_1                  0x1C47          // Intel UM67 Chipset
#define V_PCH_LPC_DEVICE_ID_MB_2                  0x1C49          // Intel HM65 Chipset
#define V_PCH_LPC_DEVICE_ID_MB_3                  0x1C4B          // Intel HM67 Chipset
#define V_PCH_LPC_DEVICE_ID_MB_4                  0x1C4F          // Intel QM67 Chipset
#define V_PCH_LPC_DEVICE_ID_MB_5                  0x1CCB          // Intel CPT R B1 Chipset
#define V_PCH_LPC_DEVICE_ID_SFF_0                 0x1C41          // Mobile SFF Full Featured
#define V_PCH_LPC_DEVICE_ID_SFF_1                 0x1C4D          // Intel QS67 Chipset
//
// Panther Point Desktop LPC Device IDs
//
#define V_PCH_LPC_DEVICE_ID_DT_11                 0x1E41          // Desktop Full Featured
#define V_PCH_LPC_DEVICE_ID_DT_12                 0x1E44          // Z77 SKU
#define V_PCH_LPC_DEVICE_ID_DT_14                 0x1E46          // Z75 SKU
#define V_PCH_LPC_DEVICE_ID_DT_15                 0x1E47          // Q77 SKU
#define V_PCH_LPC_DEVICE_ID_DT_16                 0x1E48          // Q75 SKU
#define V_PCH_LPC_DEVICE_ID_DT_17                 0x1E49          // B75 SKU
#define V_PCH_LPC_DEVICE_ID_DT_18                 0x1E4A          // H77 SKU
#define V_PCH_LPC_DEVICE_ID_DT_19                 0x1E4E          // Desktop Super SKU

//
// Panther Point Mobile LPC Device IDs
//
#define V_PCH_LPC_DEVICE_ID_MB_15                 0x1E42          // Mobile Full Featured SKU
#define V_PCH_LPC_DEVICE_ID_MB_16                 0x1E4B          // Mobile Super SKU
#define V_PCH_LPC_DEVICE_ID_MB_17                 0x1E55          // QM77 SKU
#define V_PCH_LPC_DEVICE_ID_MB_18                 0x1E5D          // HM75 SKU
#define V_PCH_LPC_DEVICE_ID_MB_19                 0x1E57          // HM77 SKU
#define V_PCH_LPC_DEVICE_ID_MB_20                 0x1E58          // UM77 SKU
#define V_PCH_LPC_DEVICE_ID_MB_21                 0x1E59          // HM76 SKU
#define V_PCH_LPC_DEVICE_ID_SFF_3                 0x1E43          // Mobile SFF Full Featured SKU 
#define V_PCH_LPC_DEVICE_ID_SFF_4                 0x1E56          // QS77 SKU

//
// Cougarpoint Server/WS LPC Device IDs
//
#define V_PCH_LPC_DEVICE_ID_SVR_0                 0x1C52          // Intel C202 Chipset
#define V_PCH_LPC_DEVICE_ID_SVR_1                 0x1C54          // Intel C204 Chipset
#define V_PCH_LPC_DEVICE_ID_SVR_2                 0x1C56          // Intel C206 Chipset

//
// Panther Point Server/WS LPC Device IDs
//
#define V_PCH_LPC_DEVICE_ID_SVR_3                 0x1E53          // Intel C216 Chipset

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
#define V_PCH_LPC_RID_0                           0x00            // A0 stepping
#define V_PCH_LPC_RID_1                           0x01            // A1 stepping
#define V_PCH_LPC_RID_2                           0x02            // B0 stepping
#define V_PCH_LPC_RID_3                           0x03            // B1 stepping
#define V_PCH_LPC_RID_4                           0x04            // B2 stepping
#define V_PCH_LPC_RID_5                           0x05            // B3 stepping
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
#define B_PCH_LPC_ACPI_BASE_BAR                   0x0000FF80
#define R_PCH_LPC_ACPI_CNT                        0x44
#define B_PCH_LPC_ACPI_CNT_ACPI_EN                0x80
#define B_PCH_LPC_ACPI_CNT_SCI_IRG_SEL            0x07
#define R_PCH_LPC_GPIO_BASE                       0x48
#define B_PCH_LPC_GPIO_BASE_BAR                   0x0000FF80
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
#define B_PCH_LPC_BIOS_CNTL_SMM_BWP               0x20            // SMM BIOS write protect disable
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
#define B_PCH_LPC_FVEC0_SATA_RAID_CAP             0x000000A0
#define B_PCH_LPC_FVEC0_SATA_PORT23_CAP           0x00000040
#define B_PCH_LPC_FVEC0_SATA_PORT1_6GB_CAP        0x00000008
#define B_PCH_LPC_FVEC0_SATA_PORT0_6GB_CAP        0x00000004
#define B_PCH_LPC_FVEC0_PCI_CAP                   0x00000002
#define R_PCH_LPC_FVEC1                           0x01
#define B_PCH_LPC_FVEC1_USB_R_CAP                 0x00400000
#define R_PCH_LPC_FVEC2                           0x02
#define B_PCH_LPC_FVEC2_IATT_CAP                  0x00400000      // Intel Anti-Theft Technology Capability
#define V_PCH_LPC_FVEC2_PCIE_PORT78_CAP           0x00200000
#define V_PCH_LPC_FVEC2_PCH_IG_SUPPORT_CAP        0x00020000      // PCH Integrated Graphics Support Capability
#define R_PCH_LPC_FVEC3                           0x03
#define B_PCH_LPC_FVEC3_DCMI_CAP                  0x00002000      // Data Center Manageability Interface (DCMI) Capability
#define B_PCH_LPC_FVEC3_NM_CAP                    0x00001000      // Node Manager Capability
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
#define R_PCH_LPC_GEN_PMCON_2                     0xA2
#define B_PCH_LPC_GEN_PMCON_PWROK_FLR             0x01
#define B_PCH_LPC_GEN_PMCON_SYSPWR_FLR            0x02
#define B_PCH_LPC_GEN_PMCON_MIN_SLP_S4            0x04
#define B_PCH_LPC_GEN_PMCON_CTS                   0x08
#define B_PCH_LPC_GEN_PMCON_SRS                   0x10
#define B_PCH_LPC_GEN_PMCON_MEM_SR                0x20
#define B_PCH_LPC_GEN_PMCON_DRAM_INIT             0x80
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
#define B_PCH_LPC_GEN_PMCON_SLP_LAN_DEFAULT       BIT8
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
#define B_PCH_LPC_GEN_PMCON_LOCK_S4_STRET_LD      BIT2            // Lock down SLP_S3/SLP_S4 Minimum Assertion width
#define B_PCH_LPC_GEN_PMCON_LOCK_ABASE_LK         BIT1            // Lock ACPI BASE at 0x40, only cleared by reset when set
#define R_PCH_LPC_CIR4                            0xA9            // Chipset Initialization Register 4
#define R_PCH_LPC_BM_BREAK_EN2                    0xAA
#define B_PCH_LPC_BM_BREAK_EN2_SATA3              BIT0
#define R_PCH_LPC_BM_BREAK_EN                     0xAB
#define B_PCH_LPC_BM_BREAK_EN_STORAGE             BIT7
#define B_PCH_LPC_BM_BREAK_EN_PCIE                BIT6
#define B_PCH_LPC_BM_BREAK_EN_PCI                 BIT5
#define B_PCH_LPC_BM_BREAK_EN_EHCI                BIT2
#define B_PCH_LPC_BM_BREAK_EN_HDA                 BIT0
#define R_PCH_LPC_PMIR                            0xAC
#define B_PCH_LPC_PMIR_CF9LOCK                    BIT31           // CF9h Lockdown
#define B_PCH_LPC_PMIR_SLP_LAN_LOW_DC             BIT25           // SLP_LAN# Low on DC Power
#define B_PCH_LPC_PMIR_CF9GR                      BIT20           // CF9h Global Reset
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
#define B_PCH_ACPI_PM1_TMR_VAL                    0xFFFFFF
#define V_PCH_ACPI_PM1_TMR_MAX_VAL                0x1000000       // The timer is 24 bit overflow

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

#define R_PCH_ALT_GP_SMI_EN                       0x38
#define S_PCH_ALT_GP_SMI_EN                       2
#define R_PCH_ALT_GP_SMI_STS                      0x3A
#define S_PCH_ALT_GP_SMI_STS                      2
//
// USB Per-Port Registers Write Control
//
#define R_PCH_UPRWC                               0x3C
#define S_PCH_UPRWC                               2
#define B_PCH_UPRWC_WR_EN_SMI_STS                 0x0100
#define B_PCH_UPRWC_WR_EN                         0x0002
#define B_PCH_UPRWC_WR_EN_SMI_EN                  0x0001

#define R_PCH_ACPI_GPE_CNTL                       0x42
#define B_PCH_ACPI_GPE_CNTL_GPIO27_POL            BIT2
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
#define B_PCH_OC_WDT_CTL_EN                       BIT14
#define B_PCH_OC_WDT_CTL_ICCSURV                  BIT13
#define B_PCH_OC_WDT_CTL_LCK                      BIT12
#define B_PCH_OC_WDT_CTL_TOV_MASK                 0x3FF
#define B_PCH_OC_WDT_CTL_FAILURE_STS              BIT23
#define B_PCH_OC_WDT_CTL_UNXP_RESET_STS           BIT22
#define B_PCH_OC_WDT_CTL_AFTER_POST               0x3F0000
#define V_PCH_OC_WDT_CTL_STATUS_FAILURE           1
#define V_PCH_OC_WDT_CTL_STATUS_OK                0

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
#define R_PCH_GPIO_BLINK                          0x18
#define R_PCH_GPIO_SER_BLINK                      0x1C
#define R_PCH_GPIO_SB_CMDSTS                      0x20
#define B_PCH_GPIO_SB_CMDSTS_DLS_MASK             0x00C00000      // Data length select
#define B_PCH_GPIO_SB_CMDSTS_DRS_MASK             0x003F0000      // Data rate select
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

#define ACPI_IO_BASE_ADDRESS                      0x400

/**
  Determine if "Boot with no change" is true according to project characteristic.

  @param[in, out]  *IsNoChange        If IsNoChange == TRUE, then boot mode will be set to 
                                      BOOT_ASSUMING_NO_CONFIGURATION_CHANGES which might reduce the POST time.

  @retval          EFI_UNSUPPORTED    Returns unsupported by default.
  @retval          EFI_SUCCESS        The service is customized in the project.
  @retval          EFI_MEDIA_CHANGED  The value of IN OUT parameter is changed. 
  @retval          Others             Depends on customization.
**/
EFI_STATUS
OemSvcIsBootWithNoChange (
  IN OUT BOOLEAN                        *IsNoChange
  )
{
  UINT16                                TcoBase;
  UINT16                                Tco2Status;
  BOOLEAN                               BoxOpen;

  //
  // Read the ACPI registers
  //
  TcoBase     = (UINT16) ((UINT16) ACPI_IO_BASE_ADDRESS + (UINT16) PCH_TCO_BASE);
  Tco2Status  = IoRead16 (TcoBase + R_PCH_TCO2_STS);

  //
  // This is the state of the hardware
  //
  BoxOpen = (BOOLEAN) ((Tco2Status & B_PCH_TCO2_STS_INTRD_DET) == 1);
  if (BoxOpen) {
    //
    // Clear the bit for next boot. (reset B_INTRD_DET by writing high)
    //
    Tco2Status |= B_PCH_TCO2_STS_INTRD_DET;
    IoWrite16 (TcoBase + R_PCH_TCO2_STS, Tco2Status);

    //
    // Since it was OPEN, return that cannot be in "no config. change boot"
    //
    DEBUG ((EFI_D_INFO, "Boot with Full configuration\n"));
    *IsNoChange = FALSE;
    return EFI_MEDIA_CHANGED;
  }

  //
  // return EFI_SUCCESS is equivalent to *IsNoChange = TRUE
  //
  return EFI_UNSUPPORTED;
}
