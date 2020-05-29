/** @file

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

#ifndef _SIO_EC_REGS_H_
#define _SIO_EC_REGS_H_

//
// ====== Common Register ======
//
#define SIO_CONFIG_PORT1          0x2E
#define SIO_CONFIG_PORT2          0x4E
#define REG_LOGICAL_DEVICE        0x07
#define REG_CHIP_ID               0x20
#define ACTIVATE                  0x30
#define BASE_ADDRESS_HIGH0        0x60
#define BASE_ADDRESS_LOW0         0x61
#define BASE_ADDRESS_HIGH1        0x62
#define BASE_ADDRESS_LOW1         0x63
#define BASE_ADDRESS_HIGH2        0x64
#define BASE_ADDRESS_LOW2         0x65
#define BASE_ADDRESS_HIGH3        0x66
#define BASE_ADDRESS_LOW3         0x67
#define PRIMARY_INTERRUPT_SELECT  0x70
#define WAKEUP_ON_IRQ_EN          0x70
#define INTERRUPT_TYPE            0x71
#define DMA_CHANNEL_SELECT0       0x74
#define DMA_CHANNEL_SELECT1       0x75

//
// ====== SIO PILOT III ======
//
#define PILOT3_CHIP_ID            0x03
#define PILOT3_SIO_INDEX_PORT     SIO_CONFIG_PORT2
#define PILOT3_SIO_DATA_PORT      (PILOT3_SIO_INDEX_PORT+1)

#define PILOT3_SIO_UNLOCK         0x5A
#define PILOT3_SIO_LOCK           0xA5
//
// logical device in Pilot3
//
#define PILOT3_SIO_PSR            0x00
#define PILOT3_SIO_COM2           0x01
#define PILOT3_SIO_COM1           0x02
#define PILOT3_SIO_SWCPII         0x03
#define PILOT3_SIO_GPIO           0x04
#define PILOT3_SIO_WDT            0x05
#define PILOT3_SIO_KCS1           0x06
#define PILOT3_SIO_KCS2           0x07
#define PILOT3_SIO_KCS3           0x08
#define PILOT3_SIO_KCS4           0x09
#define PILOT3_SIO_KCS5           0x0A
#define PILOT3_SIO_BT             0x0B
#define PILOT3_SIO_SMIC           0x0C
#define PILOT3_SIO_MAILBOX        0x0D
#define PILOT3_SIO_RTC            0x0E
#define PILOT3_SIO_SPI            0x0F
#define PILOT3_SIO_TAP            0x10
#define SIO_FLOPPY                0x0
#define SIO_PARA                  0x0
#define SIO_COM2                  0x1
#define SIO_COM1                  0x2
#define SIO_KYBD                  0xFF
#define SIO_MOUSE                 0xFF
#define SIO_GPIO                  0x4

//
// Regisgers for Pilot3
//
#define PILOT3_LOGICAL_DEVICE             REG_LOGICAL_DEVICE
#define PILOT3_ACTIVATE                   ACTIVATE
#define PILOT3_BASE_ADDRESS_HIGH0         BASE_ADDRESS_HIGH0
#define PILOT3_BASE_ADDRESS_LOW0          BASE_ADDRESS_LOW0
#define PILOT3_BASE_ADDRESS_HIGH1         BASE_ADDRESS_HIGH1
#define PILOT3_BASE_ADDRESS_LOW1          BASE_ADDRESS_LOW1
#define PILOT3_PRIMARY_INTERRUPT_SELECT   PRIMARY_INTERRUPT_SELECT
 
//
// Pilot 3 Mailbox Data Register definitions
//
#define MBDAT00_OFFSET                  0x00
#define MBDAT01_OFFSET                  0x01
#define MBDAT02_OFFSET                  0x02
#define MBDAT03_OFFSET                  0x03
#define MBDAT04_OFFSET                  0x04
#define MBDAT05_OFFSET                  0x05
#define MBDAT06_OFFSET                  0x06
#define MBDAT07_OFFSET                  0x07
#define MBDAT08_OFFSET                  0x08
#define MBDAT09_OFFSET                  0x09
#define MBDAT10_OFFSET                  0x0A
#define MBDAT11_OFFSET                  0x0B
#define MBDAT12_OFFSET                  0x0C
#define MBDAT13_OFFSET                  0x0D
#define MBDAT14_OFFSET                  0x0E
#define MBDAT15_OFFSET                  0x0F
#define MBST0_OFFSET                    0x10
#define MBST1_OFFSET                    0x11


//
// Platform Base Address definitions
//
#define SIO_BASE_ADDRESS                0x0600
#define SIO_BASE_ADDRESS2               0x1640

//
// Define some fixed platform device location information
//
#define LPC_SIO_CONFIG_PORT             0x2E
#define LPC_SIO_INDEX_PORT              0x2E
#define LPC_SIO_DATA_PORT               0x2F
#define LPC_SIO_CONFIG_PORT3            0x164E
#define LPC_SIO_INDEX_PORT3             0x164E
#define LPC_SIO_DATA_PORT3              0x164F
#define LPC_SERIAL_PORT1_BASE           0x3F8
#define LPC_SERIAL_PORT2_BASE           0x2F8

#endif
