/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _OEM_EC_H_
#define _OEM_EC_H_

#define KBC_TIME_OUT                      0x10000
#define SMC_DATA                          0x62
#define SMC_CMD_STATE                     0x66
#define  EC_ACPI_MODE_EN_CMD              0xAA
#define  EC_ACPI_MODE_DIS_CMD             0xAB
#define SHUTDOWN                          0x22
#define EN_DTEMP                          0x5E
#define DIS_DTEMP                         0x5F
#define GET_DTEMP                         0x50
#define QUERY_SYS_STATUS                  0x06
#define B_SYS_STATUS_AC                   0x10 // AC power (1 = AC powered)
#define EC_READ_VERSION_CMD               0x90
#define EC_READ_BORAD_ID_CMD              0x0D
#define EC_READ_BORAD_ID_BYTE_CMD         0x0A


#define SET_DSW_MODE                      0x29
#define DSW_DISABlE                       0x0
#define DSW_IN_S5                         0x1
#define DSW_IN_S4_S5                      0x3
#define DSW_IN_S3_S4_S5                   0x5

#define EC_RAM_LID_STATUS_OFFSET                             0x03
#define EC_READ_ECRAM_CMD                                    0x80
#define B_EC_LID_STATUS                                      BIT6

//
// EC board id and fab id bit definition
//
#define B_EC_FAB_ID                              (BIT2 | BIT1 | BIT0)
#define B_EC_BOARD_ID_5                          (BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define B_EC_BOARD_ID_8                          (BIT7 | BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define B_EC_BOARD_ID_TWO_BYTE                   (BIT7)


#endif //#ifndef _OEM_EC_H_
