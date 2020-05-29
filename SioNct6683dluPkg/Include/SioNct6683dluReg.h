//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _SIO_NCT6683DLU_H_
#define _SIO_NCT6683DLU_H_

#include <Sio/SioCommon.h>

#define SIO_DXE_DRIVER_GUID \
  { 0x3F8A61A0, 0x9F22, 0x40dd, 0x87, 0x21, 0x09, 0xC6, 0xFF, 0x57, 0x33, 0xBC }

//
// Sio common registers
//
#define SIO_LDN                     SIO_CR07
#define SIO_IRQ_TYPE_SEL1           SIO_CR10
#define SIO_IRQ_TYPE_SEL2           SIO_CR11
#define SIO_IRQ_POLARITY_SEL1       SIO_CR13
#define SIO_IRQ_POLARITY_SEL2       SIO_CR14
#define SIO_MULTI_FUN_PIN_SEL1      SIO_CR15
#define SIO_MULTI_FUN_PIN_SEL2      SIO_CR1A
#define SIO_MULTI_FUN_PIN_SEL3      SIO_CR1B
#define SIO_TEST1_MODE              SIO_CR1D
#define SIO_MULTI_FUN_PIN_SEL4      SIO_CR1E
#define SIO_MULTI_FUN_PIN_SEL5      SIO_CR1F

#define SIO_CHIP_ID1                SIO_CR20
#define SIO_CHIP_ID2                SIO_CR21
#define SIO_POWER_DOWN1             SIO_CR22
#define SIO_POWER_DOWN2             SIO_CR23
#define SIO_MULTI_FUN_PIN_SEL6      SIO_CR24
#define SIO_MULTI_FUN_PIN_SEL7      SIO_CR25
#define SIO_GLOBAL_OPTION           SIO_CR26
#define SIO_MULTI_FUN_PIN_SEL8      SIO_CR27
#define SIO_MULTI_FUN_PIN_SEL9      SIO_CR28
#define SIO_MULTI_FUN_PIN_SELA      SIO_CR29
#define SIO_MULTI_FUN_PIN_SELB      SIO_CR2A
#define SIO_MULTI_FUN_PIN_SELC      SIO_CR2B
#define SIO_MULTI_FUN_PIN_SELD      SIO_CR2C
#define SIO_MULTI_FUN_PIN_SELE      SIO_CR2D
#define SIO_STRAPPING_FUN_RESULT    SIO_CR2F

#define SIO_DEV_ACTIVE              SIO_CR30
#define SIO_BASE_IO_ADDR1_MSB       SIO_CR60
#define SIO_BASE_IO_ADDR1_LSB       SIO_CR61
#define SIO_BASE_IO_ADDR2_MSB       SIO_CR62
#define SIO_BASE_IO_ADDR2_LSB       SIO_CR63
#define SIO_IRQ_SET                 SIO_CR70
#define SIO_IRQ_TYPE_SEL            SIO_CR71
#define SIO_MOUSE_IRQ_SET           SIO_CR72
#define SIO_DMA_SET0                SIO_CR74
#define SIO_DMA_SET1                SIO_CR75

#define DMA_CHANNEL_DISABLE         0xFF

//
// Logical Device Number
//

#define SIO_LPT                      0x01
#define SIO_COM1                     0x02
#define SIO_COM2                     0x03
#define SIO_IR                       0x03
#define SIO_KYBD_MOUSE               0x05
#define SIO_CIR                      0x06
#define SIO_GPIO0                    0x07
#define SIO_GPIO1                    0x07
#define SIO_GPIO2                    0x07
#define SIO_GPIO3                    0x07
#define SIO_GPIO4                    0x07
#define SIO_GPIO5                    0x07
#define SIO_GPIO6                    0x07
#define SIO_GPIO7                    0x07
#define SIO_PORT80                   0x08
#define SIO_GPIO8                    0x09
#define SIO_GPIO9                    0x09
#define SIO_ACPI                     0x0A
#define SIO_EC                       0x0B
#define SIO_DEEP_SLEEP               0x0D
#define SIO_TACHIN                   0x0E

//
// Define Super I/O device ID for verify component chip
//
#define SIO_ID1                      0xB7   
#define SIO_ID2                      0x32    //if the value set 0x00, it's mean we just check SIO ID1

//
// This define is for enter and exit SIO config
//
#define ENTER_CONFIG                 0x87
#define EXIT_CONFIG                  0xAA

//
// Com port I/O address
//
#define COM_PORT        0x3F8
#define COM_PORT2       0x2F8
#define COM_PORT3       0x3E8
#define COM_PORT4       0x2E8

//
// To ASL device, define you need
//
#pragma pack (1)
typedef struct {     
  EFI_ASL_RESOURCE  DeviceA; ///< ComA
  EFI_ASL_RESOURCE  DeviceB; ///< ComB
  EFI_ASL_RESOURCE  DeviceC; ///< LPT
  EFI_ASL_RESOURCE  DeviceD; ///< KYBD
  EFI_ASL_RESOURCE  DeviceE; ///< MOUSE
  EFI_ASL_RESOURCE  DeviceF; ///< IR
  EFI_ASL_RESOURCE  DeviceG; ///< CIR
} SIO_NVS_AREA;
#pragma pack ()

#endif
