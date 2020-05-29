/** @file
  Register names for PCH Serial IO Controllers

  Conventions:

  - Prefixes:
    Definitions beginning with "R_" are registers
    Definitions beginning with "B_" are bits within registers
    Definitions beginning with "V_" are meaningful values of bits within the registers
    Definitions beginning with "S_" are register sizes
    Definitions beginning with "N_" are the bit position
  - In general, PCH registers are denoted by "_PCH_" in register names
  - Registers / bits that are different between PCH generations are denoted by
    "_PCH_<generation_name>_" in register/bit names. e.g., "_PCH_CPT_"
  - Registers / bits that are different between SKUs are denoted by "_<SKU_name>"
    at the end of the register/bit names
  - Registers / bits of new devices introduced in a PCH generation will be just named
    as "_PCH_" without <generation_name> inserted.

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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
#ifndef _PCH_REGS_SERIAL_IO_H_
#define _PCH_REGS_SERIAL_IO_H_

#ifdef SERIAL_IO_FLAG

//
//  Serial IO DMA Controller Registers (D21:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LP_SERIAL_IO_DMA          21
#define PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_DMA        0
#define R_PCH_LP_SERIAL_IO_DMA_VENDOR_ID                0x00
#define V_PCH_LP_SERIAL_IO_DMA_VENDOR_ID                V_PCH_INTEL_VENDOR_ID
#define R_PCH_LP_SERIAL_IO_DMA_DEVICE_ID                0x02
#define V_PCH_LP_SERIAL_IO_DMA_DEVICE_ID                0x9C60

//
//  Serial IO I2C0 Controller Registers (D21:F1)
//
#define PCI_DEVICE_NUMBER_PCH_LP_SERIAL_IO_I2C0         21
#define PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_I2C0       1
#define R_PCH_LP_SERIAL_IO_I2C0_VENDOR_ID               0x00
#define V_PCH_LP_SERIAL_IO_I2C0_VENDOR_ID               V_PCH_INTEL_VENDOR_ID
#define R_PCH_LP_SERIAL_IO_I2C0_DEVICE_ID               0x02
#define V_PCH_LP_SERIAL_IO_I2C0_DEVICE_ID               0x9C61

//
//  Serial IO I2C0 Controller Registers (D21:F2)
//
#define PCI_DEVICE_NUMBER_PCH_LP_SERIAL_IO_I2C1         21
#define PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_I2C1       2
#define R_PCH_LP_SERIAL_IO_I2C1_VENDOR_ID               0x00
#define V_PCH_LP_SERIAL_IO_I2C1_VENDOR_ID               V_PCH_INTEL_VENDOR_ID
#define R_PCH_LP_SERIAL_IO_I2C1_DEVICE_ID               0x02
#define V_PCH_LP_SERIAL_IO_I2C1_DEVICE_ID               0x9C62

//
//  Serial IO SPI0 Controller Registers (D21:F3)
//
#define PCI_DEVICE_NUMBER_PCH_LP_SERIAL_IO_SPI0         21
#define PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_SPI0       3
#define R_PCH_LP_SERIAL_IO_SPI0_VENDOR_ID               0x00
#define V_PCH_LP_SERIAL_IO_SPI0_VENDOR_ID               V_PCH_INTEL_VENDOR_ID
#define R_PCH_LP_SERIAL_IO_SPI0_DEVICE_ID               0x02
#define V_PCH_LP_SERIAL_IO_SPI0_DEVICE_ID               0x9C65

//
//  Serial IO SPI1 Controller Registers (D21:F4)
//
#define PCI_DEVICE_NUMBER_PCH_LP_SERIAL_IO_SPI1         21
#define PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_SPI1       4
#define R_PCH_LP_SERIAL_IO_SPI1_VENDOR_ID               0x00
#define V_PCH_LP_SERIAL_IO_SPI1_VENDOR_ID               V_PCH_INTEL_VENDOR_ID
#define R_PCH_LP_SERIAL_IO_SPI1_DEVICE_ID               0x02
#define V_PCH_LP_SERIAL_IO_SPI1_DEVICE_ID               0x9C66

//
//  Serial IO UART0 Controller Registers (D21:F5)
//
#define PCI_DEVICE_NUMBER_PCH_LP_SERIAL_IO_UART0        21
#define PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_UART0      5
#define R_PCH_LP_SERIAL_IO_UART0_VENDOR_ID              0x00
#define V_PCH_LP_SERIAL_IO_UART0_VENDOR_ID              V_PCH_INTEL_VENDOR_ID
#define R_PCH_LP_SERIAL_IO_UART0_DEVICE_ID              0x02
#define V_PCH_LP_SERIAL_IO_UART0_DEVICE_ID              0x9C63

//
//  Serial IO UART1 Controller Registers (D21:F6)
//
#define PCI_DEVICE_NUMBER_PCH_LP_SERIAL_IO_UART1        21
#define PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_UART1      6
#define R_PCH_LP_SERIAL_IO_UART1_VENDOR_ID              0x00
#define V_PCH_LP_SERIAL_IO_UART1_VENDOR_ID              V_PCH_INTEL_VENDOR_ID
#define R_PCH_LP_SERIAL_IO_UART1_DEVICE_ID              0x02
#define V_PCH_LP_SERIAL_IO_UART1_DEVICE_ID              0x9C64

//
//  Serial IO SDIO Controller Registers (D23:F0)
//
#define PCI_DEVICE_NUMBER_PCH_LP_SERIAL_IO_SDIO         23
#define PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_SDIO       0
#define R_PCH_LP_SERIAL_IO_SDIO_VENDOR_ID               0x00
#define V_PCH_LP_SERIAL_IO_SDIO_VENDOR_ID               V_PCH_INTEL_VENDOR_ID
#define R_PCH_LP_SERIAL_IO_SDIO_DEVICE_ID               0x02
#define V_PCH_LP_SERIAL_IO_SDIO_DEVICE_ID               0x9C35
#define R_PCH_LP_SERIAL_IO_SDIO_PPR_CMD12               0x3C
#define R_PCH_LP_SERIAL_IO_SDIO_PPR_GEN                 0x1008
#define B_PCH_LP_SERIAL_IO_SDIO_PPR_GEN_LTR_MODE        BIT2
#define R_PCH_LP_SERIAL_IO_SDIO_PPR_SW_LTR              0x1010


#define R_PCH_LP_SERIAL_IO_SDIO_SLAVE_DELAY_DDR50_MODE   0x1034

#define V_LP_SERIAL_IO_DEV_MIN_FUN                      0
#define V_LP_SERIAL_IO_DEV_MAX_FUN                      6

//
// Serial IO Controllers General PCI Configuration Registers
//
#define R_PCH_LP_SERIAL_IO_VENDOR_ID                    0x00
#define R_PCH_LP_SERIAL_IO_DEVICE_ID                    0x02
#define R_PCH_LP_SERIAL_IO_COMMAND                      0x04
#define B_PCH_LP_SERIAL_IO_COMMAND_BME                  BIT2
#define B_PCH_LP_SERIAL_IO_COMMAND_MSE                  BIT1
#define R_PCH_LP_SERIAL_IO_BAR0                         0x10
#define B_PCH_LP_SERIAL_IO_BAR0_BAR                     0xFFFFF000
#define R_PCH_LP_SERIAL_IO_BAR1                         0x14
#define B_PCH_LP_SERIAL_IO_BAR1_BAR                     0xFFFFF000
#define V_PCH_LP_SERIAL_IO_BAR_SIZE                     (4 * 1024)
#define V_PCH_LP_SERIAL_SDIO_BAR_SIZE                   (8 * 1024)
#define N_PCH_LP_SERIAL_IO_BAR_ALIGNMENT                12
#define R_PCH_LP_SERIAL_IO_PME_CTRL_STS                 0x84
#define B_PCH_LP_SERIAL_IO_PME_CTRL_STS_PWR_ST          (BIT1| BIT0)

//
// Serial IO Controllers Private Registers
//
#define R_PCH_LP_SERIAL_IO_PPR_RST                      0x804
#define B_PCH_LP_SERIAL_IO_PPR_RST_APB                  BIT0
#define B_PCH_LP_SERIAL_IO_PPR_RST_FUNC                 BIT1
#define R_PCH_LP_SERIAL_IO_PPR_GEN                      0x808
#define B_PCH_LP_SERIAL_IO_PPR_GEN_LTR_MODE             BIT2
#define B_PCH_LP_SERIAL_IO_PPR_GEN_IO_VOLTAGE_SEL       BIT3
#define R_PCH_LP_SERIAL_IO_PPR_AUTO_LTR                 0x814

#define R_PCH_LP_SERIAL_IO_GPIODF0                      0x154
#define B_PCH_LP_SERIAL_IO_GPIODF0_SPI_IDLE_DET_EN      BIT0
#define B_PCH_LP_SERIAL_IO_GPIODF0_I2C_IDLE_DET_EN      BIT1
#define B_PCH_LP_SERIAL_IO_GPIODF0_UART_IDLE_DET_EN     BIT2
#define B_PCH_LP_SERIAL_IO_GPIODF0_DMA_IDLE_DET_EN      BIT3
#define B_PCH_LP_SERIAL_IO_GPIODF0_SDIO_IDLE_DET_EN     BIT4

#endif // SERIAL_IO_FLAG
#endif
