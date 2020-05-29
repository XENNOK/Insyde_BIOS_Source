/** @file

  This file was automatically generated. Modify at your own risk.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _MC_IO_COMP_H_
#define _MC_IO_COMP_H_

#pragma pack(push, 1)
#include <MrcTypes.h>

typedef union {
  struct {
    U32 RcompDrvUp                              :  6;  // Bits 5:0
    U32 Spare0                                  :  3;  // Bits 8:6
    U32 RcompDrvDown                            :  6;  // Bits 14:9
    U32 VTComp                                  :  5;  // Bits 19:15
    U32 TcoComp                                 :  6;  // Bits 25:20
    U32 SlewRateComp                            :  5;  // Bits 30:26
    U32 Spare2                                  :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRDATACOMP0_STRUCT;

typedef union {
  struct {
    U32 RcompOdtUp                              :  6;  // Bits 5:0
    U32 Spare0                                  :  3;  // Bits 8:6
    U32 RcompOdtDown                            :  6;  // Bits 14:9
    U32 Spare1                                  :  1;  // Bits 15:15
    U32 PanicDrvDn                              :  6;  // Bits 21:16
    U32 PanicDrvUp                              :  6;  // Bits 27:22
    U32 LevelShifterComp                        :  3;  // Bits 30:28
    U32 Spare2                                  :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRDATACOMP1_STRUCT;

typedef union {
  struct {
    U32 Scomp                                   :  6;  // Bits 5:0
    U32 TcoComp                                 :  6;  // Bits 11:6
    U32 RcompDrvUp                              :  6;  // Bits 17:12
    U32 RcompDrvDown                            :  6;  // Bits 23:18
    U32 LsComp                                  :  3;  // Bits 26:24
    U32 Spare                                   :  5;  // Bits 31:27
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRCMDCOMP_STRUCT;

typedef union {
  struct {
    U32 Scomp                                   :  6;  // Bits 5:0
    U32 TcoComp                                 :  6;  // Bits 11:6
    U32 RcompDrvUp                              :  6;  // Bits 17:12
    U32 RcompDrvDown                            :  6;  // Bits 23:18
    U32 LsComp                                  :  4;  // Bits 27:24
    U32 Spare                                   :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRCTLCOMP_STRUCT;

typedef union {
  struct {
    U32 Scomp                                   :  5;  // Bits 4:0
    U32 TcoComp                                 :  6;  // Bits 10:5
    U32 RcompDrvUp                              :  6;  // Bits 16:11
    U32 RcompDrvDown                            :  6;  // Bits 22:17
    U32 LsComp                                  :  4;  // Bits 26:23
    U32 Spare                                   :  5;  // Bits 31:27
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRCLKCOMP_STRUCT;

typedef union {
  struct {
    U32 Rsvd                                    :  3;  // Bits 2:0
    U32 DisableOdtStatic                        :  1;  // Bits 3:3
    U32 DqOdtUpDnOff                            :  6;  // Bits 9:4
    U32 FixOdtD                                 :  1;  // Bits 10:10
    U32 DqDrvVref                               :  4;  // Bits 14:11
    U32 DqOdtVref                               :  5;  // Bits 19:15
    U32 CmdDrvVref                              :  4;  // Bits 23:20
    U32 CtlDrvVref                              :  4;  // Bits 27:24
    U32 ClkDrvVref                              :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRCOMPCTL0_STRUCT;

typedef union {
  struct {
    U32 DqScompCells                            :  4;  // Bits 3:0
    U32 DqScompPC                               :  1;  // Bits 4:4
    U32 CmdScompCells                           :  4;  // Bits 8:5
    U32 CmdScompPC                              :  1;  // Bits 9:9
    U32 CtlScompCells                           :  4;  // Bits 13:10
    U32 CtlScompPC                              :  1;  // Bits 14:14
    U32 ClkScompCells                           :  4;  // Bits 18:15
    U32 ClkScompPC                              :  1;  // Bits 19:19
    U32 TcoCmdOffset                            :  4;  // Bits 23:20
    U32 CompClkOn                               :  1;  // Bits 24:24
    U32 VccddqHi                                :  1;  // Bits 25:25
    U32 spare                                   :  3;  // Bits 28:26
    U32 DisableQuickComp                        :  1;  // Bits 29:29
    U32 SinStep                                 :  1;  // Bits 30:30
    U32 SinStepAdv                              :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRCOMPCTL1_STRUCT;

typedef union {
  struct {
    U32 PanicDrvDnVref                          :  6;  // Bits 5:0
    U32 PanicDrvUpVref                          :  6;  // Bits 11:6
    U32 VtOffset                                :  5;  // Bits 16:12
    U32 VtSlopeA                                :  3;  // Bits 19:17
    U32 VtSlopeB                                :  3;  // Bits 22:20
    U32 Spare                                   :  9;  // Bits 31:23
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRCOMPVSSHI_STRUCT;

typedef union {
  struct {
    U32 DqDrvU                                  :  1;  // Bits 0:0
    U32 DqDrvD                                  :  1;  // Bits 1:1
    U32 DqOdtU                                  :  1;  // Bits 2:2
    U32 DqOdtD                                  :  1;  // Bits 3:3
    U32 CmdDrvU                                 :  1;  // Bits 4:4
    U32 CmdDrvD                                 :  1;  // Bits 5:5
    U32 CtlDrvU                                 :  1;  // Bits 6:6
    U32 CtlDrvD                                 :  1;  // Bits 7:7
    U32 ClkDrvU                                 :  1;  // Bits 8:8
    U32 ClkDrvD                                 :  1;  // Bits 9:9
    U32 DqSR                                    :  1;  // Bits 10:10
    U32 CmdSR                                   :  1;  // Bits 11:11
    U32 CtlSR                                   :  1;  // Bits 12:12
    U32 ClkSR                                   :  1;  // Bits 13:13
    U32 DqTcoOff                                :  1;  // Bits 14:14
    U32 CmdTcoOff                               :  1;  // Bits 15:15
    U32 DqTco                                   :  1;  // Bits 16:16
    U32 CmdTco                                  :  1;  // Bits 17:17
    U32 CtlTco                                  :  1;  // Bits 18:18
    U32 ClkTco                                  :  1;  // Bits 19:19
    U32 Spare1                                  :  1;  // Bits 20:20
    U32 PanicDrvUp                              :  1;  // Bits 21:21
    U32 PanicDrvDn                              :  1;  // Bits 22:22
    U32 VTComp                                  :  1;  // Bits 23:23
    U32 LsComp                                  :  3;  // Bits 26:24
    U32 Spare2                                  :  5;  // Bits 31:27
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRCOMPOVR_STRUCT;

typedef union {
  struct {
    U32 Target                                  :  6;  // Bits 5:0
    U32 HiBWDivider                             :  2;  // Bits 7:6
    U32 LoBWDivider                             :  2;  // Bits 9:8
    U32 SampleDivider                           :  3;  // Bits 12:10
    U32 OpenLoop                                :  1;  // Bits 13:13
    U32 BWError                                 :  2;  // Bits 15:14
    U32 PanicEn                                 :  1;  // Bits 16:16
    U32 Rsvd                                    :  1;  // Bits 17:17
    U32 PanicVoltage                            :  4;  // Bits 21:18
    U32 GainBoost                               :  1;  // Bits 22:22
    U32 SelCode                                 :  1;  // Bits 23:23
    U32 OutputCode                              :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_STRUCT;


#define DDRCOMP_CR_DDRCRDATACOMP0_REG                                (0x00003A00)
  #define DDRCOMP_CR_DDRCRDATACOMP0_RcompDrvUp_OFF                     ( 0)
  #define DDRCOMP_CR_DDRCRDATACOMP0_RcompDrvUp_WID                     ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP0_RcompDrvUp_MSK                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP0_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP0_Spare0_OFF                         ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP0_Spare0_WID                         ( 3)
  #define DDRCOMP_CR_DDRCRDATACOMP0_Spare0_MSK                         (0x000001C0)
  #define DDRCOMP_CR_DDRCRDATACOMP0_Spare0_MAX                         (0x00000007)
  #define DDRCOMP_CR_DDRCRDATACOMP0_RcompDrvDown_OFF                   ( 9)
  #define DDRCOMP_CR_DDRCRDATACOMP0_RcompDrvDown_WID                   ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP0_RcompDrvDown_MSK                   (0x00007E00)
  #define DDRCOMP_CR_DDRCRDATACOMP0_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP0_VTComp_OFF                         (15)
  #define DDRCOMP_CR_DDRCRDATACOMP0_VTComp_WID                         ( 5)
  #define DDRCOMP_CR_DDRCRDATACOMP0_VTComp_MSK                         (0x000F8000)
  #define DDRCOMP_CR_DDRCRDATACOMP0_VTComp_MAX                         (0x0000001F)
  #define DDRCOMP_CR_DDRCRDATACOMP0_VTComp_DEF                         (0x00000005)
  #define DDRCOMP_CR_DDRCRDATACOMP0_TcoComp_OFF                        (20)
  #define DDRCOMP_CR_DDRCRDATACOMP0_TcoComp_WID                        ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP0_TcoComp_MSK                        (0x03F00000)
  #define DDRCOMP_CR_DDRCRDATACOMP0_TcoComp_MAX                        (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP0_SlewRateComp_OFF                   (26)
  #define DDRCOMP_CR_DDRCRDATACOMP0_SlewRateComp_WID                   ( 5)
  #define DDRCOMP_CR_DDRCRDATACOMP0_SlewRateComp_MSK                   (0x7C000000)
  #define DDRCOMP_CR_DDRCRDATACOMP0_SlewRateComp_MAX                   (0x0000001F)
  #define DDRCOMP_CR_DDRCRDATACOMP0_Spare2_OFF                         (31)
  #define DDRCOMP_CR_DDRCRDATACOMP0_Spare2_WID                         ( 1)
  #define DDRCOMP_CR_DDRCRDATACOMP0_Spare2_MSK                         (0x80000000)
  #define DDRCOMP_CR_DDRCRDATACOMP0_Spare2_MAX                         (0x00000001)

#define DDRCOMP_CR_DDRCRDATACOMP1_REG                                (0x00003A04)
  #define DDRCOMP_CR_DDRCRDATACOMP1_RcompOdtUp_OFF                     ( 0)
  #define DDRCOMP_CR_DDRCRDATACOMP1_RcompOdtUp_WID                     ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP1_RcompOdtUp_MSK                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP1_RcompOdtUp_MAX                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare0_OFF                         ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare0_WID                         ( 3)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare0_MSK                         (0x000001C0)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare0_MAX                         (0x00000007)
  #define DDRCOMP_CR_DDRCRDATACOMP1_RcompOdtDown_OFF                   ( 9)
  #define DDRCOMP_CR_DDRCRDATACOMP1_RcompOdtDown_WID                   ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP1_RcompOdtDown_MSK                   (0x00007E00)
  #define DDRCOMP_CR_DDRCRDATACOMP1_RcompOdtDown_MAX                   (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare1_OFF                         (15)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare1_WID                         ( 1)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare1_MSK                         (0x00008000)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare1_MAX                         (0x00000001)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvDn_OFF                     (16)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvDn_WID                     ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvDn_MSK                     (0x003F0000)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvDn_MAX                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvDn_DEF                     (0x00000010)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvUp_OFF                     (22)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvUp_WID                     ( 6)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvUp_MSK                     (0x0FC00000)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvUp_MAX                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRDATACOMP1_PanicDrvUp_DEF                     (0x00000010)
  #define DDRCOMP_CR_DDRCRDATACOMP1_LevelShifterComp_OFF               (28)
  #define DDRCOMP_CR_DDRCRDATACOMP1_LevelShifterComp_WID               ( 3)
  #define DDRCOMP_CR_DDRCRDATACOMP1_LevelShifterComp_MSK               (0x70000000)
  #define DDRCOMP_CR_DDRCRDATACOMP1_LevelShifterComp_MAX               (0x00000007)
  #define DDRCOMP_CR_DDRCRDATACOMP1_LevelShifterComp_DEF               (0x00000004)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare2_OFF                         (31)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare2_WID                         ( 1)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare2_MSK                         (0x80000000)
  #define DDRCOMP_CR_DDRCRDATACOMP1_Spare2_MAX                         (0x00000001)

#define DDRCOMP_CR_DDRCRCMDCOMP_REG                                  (0x00003A08)
  #define DDRCOMP_CR_DDRCRCMDCOMP_Scomp_OFF                            ( 0)
  #define DDRCOMP_CR_DDRCRCMDCOMP_Scomp_WID                            ( 6)
  #define DDRCOMP_CR_DDRCRCMDCOMP_Scomp_MSK                            (0x0000003F)
  #define DDRCOMP_CR_DDRCRCMDCOMP_Scomp_MAX                            (0x0000003F)
  #define DDRCOMP_CR_DDRCRCMDCOMP_TcoComp_OFF                          ( 6)
  #define DDRCOMP_CR_DDRCRCMDCOMP_TcoComp_WID                          ( 6)
  #define DDRCOMP_CR_DDRCRCMDCOMP_TcoComp_MSK                          (0x00000FC0)
  #define DDRCOMP_CR_DDRCRCMDCOMP_TcoComp_MAX                          (0x0000003F)
  #define DDRCOMP_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                       (12)
  #define DDRCOMP_CR_DDRCRCMDCOMP_RcompDrvUp_WID                       ( 6)
  #define DDRCOMP_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                       (0x0003F000)
  #define DDRCOMP_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                       (0x0000003F)
  #define DDRCOMP_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                     (18)
  #define DDRCOMP_CR_DDRCRCMDCOMP_RcompDrvDown_WID                     ( 6)
  #define DDRCOMP_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                     (0x00FC0000)
  #define DDRCOMP_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRCMDCOMP_LsComp_OFF                           (24)
  #define DDRCOMP_CR_DDRCRCMDCOMP_LsComp_WID                           ( 3)
  #define DDRCOMP_CR_DDRCRCMDCOMP_LsComp_MSK                           (0x07000000)
  #define DDRCOMP_CR_DDRCRCMDCOMP_LsComp_MAX                           (0x00000007)
  #define DDRCOMP_CR_DDRCRCMDCOMP_LsComp_DEF                           (0x00000004)
  #define DDRCOMP_CR_DDRCRCMDCOMP_Spare_OFF                            (27)
  #define DDRCOMP_CR_DDRCRCMDCOMP_Spare_WID                            ( 5)
  #define DDRCOMP_CR_DDRCRCMDCOMP_Spare_MSK                            (0xF8000000)
  #define DDRCOMP_CR_DDRCRCMDCOMP_Spare_MAX                            (0x0000001F)

#define DDRCOMP_CR_DDRCRCTLCOMP_REG                                  (0x00003A0C)
  #define DDRCOMP_CR_DDRCRCTLCOMP_Scomp_OFF                            ( 0)
  #define DDRCOMP_CR_DDRCRCTLCOMP_Scomp_WID                            ( 6)
  #define DDRCOMP_CR_DDRCRCTLCOMP_Scomp_MSK                            (0x0000003F)
  #define DDRCOMP_CR_DDRCRCTLCOMP_Scomp_MAX                            (0x0000003F)
  #define DDRCOMP_CR_DDRCRCTLCOMP_TcoComp_OFF                          ( 6)
  #define DDRCOMP_CR_DDRCRCTLCOMP_TcoComp_WID                          ( 6)
  #define DDRCOMP_CR_DDRCRCTLCOMP_TcoComp_MSK                          (0x00000FC0)
  #define DDRCOMP_CR_DDRCRCTLCOMP_TcoComp_MAX                          (0x0000003F)
  #define DDRCOMP_CR_DDRCRCTLCOMP_RcompDrvUp_OFF                       (12)
  #define DDRCOMP_CR_DDRCRCTLCOMP_RcompDrvUp_WID                       ( 6)
  #define DDRCOMP_CR_DDRCRCTLCOMP_RcompDrvUp_MSK                       (0x0003F000)
  #define DDRCOMP_CR_DDRCRCTLCOMP_RcompDrvUp_MAX                       (0x0000003F)
  #define DDRCOMP_CR_DDRCRCTLCOMP_RcompDrvDown_OFF                     (18)
  #define DDRCOMP_CR_DDRCRCTLCOMP_RcompDrvDown_WID                     ( 6)
  #define DDRCOMP_CR_DDRCRCTLCOMP_RcompDrvDown_MSK                     (0x00FC0000)
  #define DDRCOMP_CR_DDRCRCTLCOMP_RcompDrvDown_MAX                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRCTLCOMP_LsComp_OFF                           (24)
  #define DDRCOMP_CR_DDRCRCTLCOMP_LsComp_WID                           ( 4)
  #define DDRCOMP_CR_DDRCRCTLCOMP_LsComp_MSK                           (0x0F000000)
  #define DDRCOMP_CR_DDRCRCTLCOMP_LsComp_MAX                           (0x0000000F)
  #define DDRCOMP_CR_DDRCRCTLCOMP_LsComp_DEF                           (0x00000004)
  #define DDRCOMP_CR_DDRCRCTLCOMP_Spare_OFF                            (28)
  #define DDRCOMP_CR_DDRCRCTLCOMP_Spare_WID                            ( 4)
  #define DDRCOMP_CR_DDRCRCTLCOMP_Spare_MSK                            (0xF0000000)
  #define DDRCOMP_CR_DDRCRCTLCOMP_Spare_MAX                            (0x0000000F)

#define DDRCOMP_CR_DDRCRCLKCOMP_REG                                  (0x00003A10)
  #define DDRCOMP_CR_DDRCRCLKCOMP_Scomp_OFF                            ( 0)
  #define DDRCOMP_CR_DDRCRCLKCOMP_Scomp_WID                            ( 5)
  #define DDRCOMP_CR_DDRCRCLKCOMP_Scomp_MSK                            (0x0000001F)
  #define DDRCOMP_CR_DDRCRCLKCOMP_Scomp_MAX                            (0x0000001F)
  #define DDRCOMP_CR_DDRCRCLKCOMP_TcoComp_OFF                          ( 5)
  #define DDRCOMP_CR_DDRCRCLKCOMP_TcoComp_WID                          ( 6)
  #define DDRCOMP_CR_DDRCRCLKCOMP_TcoComp_MSK                          (0x000007E0)
  #define DDRCOMP_CR_DDRCRCLKCOMP_TcoComp_MAX                          (0x0000003F)
  #define DDRCOMP_CR_DDRCRCLKCOMP_RcompDrvUp_OFF                       (11)
  #define DDRCOMP_CR_DDRCRCLKCOMP_RcompDrvUp_WID                       ( 6)
  #define DDRCOMP_CR_DDRCRCLKCOMP_RcompDrvUp_MSK                       (0x0001F800)
  #define DDRCOMP_CR_DDRCRCLKCOMP_RcompDrvUp_MAX                       (0x0000003F)
  #define DDRCOMP_CR_DDRCRCLKCOMP_RcompDrvDown_OFF                     (17)
  #define DDRCOMP_CR_DDRCRCLKCOMP_RcompDrvDown_WID                     ( 6)
  #define DDRCOMP_CR_DDRCRCLKCOMP_RcompDrvDown_MSK                     (0x007E0000)
  #define DDRCOMP_CR_DDRCRCLKCOMP_RcompDrvDown_MAX                     (0x0000003F)
  #define DDRCOMP_CR_DDRCRCLKCOMP_LsComp_OFF                           (23)
  #define DDRCOMP_CR_DDRCRCLKCOMP_LsComp_WID                           ( 4)
  #define DDRCOMP_CR_DDRCRCLKCOMP_LsComp_MSK                           (0x07800000)
  #define DDRCOMP_CR_DDRCRCLKCOMP_LsComp_MAX                           (0x0000000F)
  #define DDRCOMP_CR_DDRCRCLKCOMP_LsComp_DEF                           (0x00000004)
  #define DDRCOMP_CR_DDRCRCLKCOMP_Spare_OFF                            (27)
  #define DDRCOMP_CR_DDRCRCLKCOMP_Spare_WID                            ( 5)
  #define DDRCOMP_CR_DDRCRCLKCOMP_Spare_MSK                            (0xF8000000)
  #define DDRCOMP_CR_DDRCRCLKCOMP_Spare_MAX                            (0x0000001F)

#define DDRCOMP_CR_DDRCRCOMPCTL0_REG                                 (0x00003A14)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_Rsvd_OFF                            ( 0)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_Rsvd_WID                            ( 3)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_Rsvd_MSK                            (0x00000007)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_Rsvd_MAX                            (0x00000007)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DisableOdtStatic_OFF                ( 3)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DisableOdtStatic_WID                ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DisableOdtStatic_MSK                (0x00000008)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DisableOdtStatic_MAX                (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtUpDnOff_OFF                    ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtUpDnOff_WID                    ( 6)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtUpDnOff_MSK                    (0x000003F0)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtUpDnOff_MAX                    (0x0000003F)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_FixOdtD_OFF                         (10)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_FixOdtD_WID                         ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_FixOdtD_MSK                         (0x00000400)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_FixOdtD_MAX                         (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqDrvVref_OFF                       (11)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqDrvVref_WID                       ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqDrvVref_MSK                       (0x00007800)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqDrvVref_MAX                       (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtVref_OFF                       (15)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtVref_WID                       ( 5)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtVref_MSK                       (0x000F8000)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_DqOdtVref_MAX                       (0x0000001F)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_CmdDrvVref_OFF                      (20)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_CmdDrvVref_WID                      ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_CmdDrvVref_MSK                      (0x00F00000)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_CmdDrvVref_MAX                      (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_CtlDrvVref_OFF                      (24)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_CtlDrvVref_WID                      ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_CtlDrvVref_MSK                      (0x0F000000)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_CtlDrvVref_MAX                      (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_ClkDrvVref_OFF                      (28)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_ClkDrvVref_WID                      ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_ClkDrvVref_MSK                      (0xF0000000)
  #define DDRCOMP_CR_DDRCRCOMPCTL0_ClkDrvVref_MAX                      (0x0000000F)

#define DDRCOMP_CR_DDRCRCOMPCTL1_REG                                 (0x00003A18)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DqScompCells_OFF                    ( 0)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DqScompCells_WID                    ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DqScompCells_MSK                    (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DqScompCells_MAX                    (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DqScompPC_OFF                       ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DqScompPC_WID                       ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DqScompPC_MSK                       (0x00000010)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DqScompPC_MAX                       (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CmdScompCells_OFF                   ( 5)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CmdScompCells_WID                   ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CmdScompCells_MSK                   (0x000001E0)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CmdScompCells_MAX                   (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CmdScompPC_OFF                      ( 9)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CmdScompPC_WID                      ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CmdScompPC_MSK                      (0x00000200)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CmdScompPC_MAX                      (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CtlScompCells_OFF                   (10)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CtlScompCells_WID                   ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CtlScompCells_MSK                   (0x00003C00)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CtlScompCells_MAX                   (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CtlScompPC_OFF                      (14)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CtlScompPC_WID                      ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CtlScompPC_MSK                      (0x00004000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CtlScompPC_MAX                      (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_ClkScompCells_OFF                   (15)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_ClkScompCells_WID                   ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_ClkScompCells_MSK                   (0x00078000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_ClkScompCells_MAX                   (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_ClkScompPC_OFF                      (19)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_ClkScompPC_WID                      ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_ClkScompPC_MSK                      (0x00080000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_ClkScompPC_MAX                      (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_TcoCmdOffset_OFF                    (20)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_TcoCmdOffset_WID                    ( 4)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_TcoCmdOffset_MSK                    (0x00F00000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_TcoCmdOffset_MAX                    (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CompClkOn_OFF                       (24)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CompClkOn_WID                       ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CompClkOn_MSK                       (0x01000000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_CompClkOn_MAX                       (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_VccddqHi_OFF                        (25)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_VccddqHi_WID                        ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_VccddqHi_MSK                        (0x02000000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_VccddqHi_MAX                        (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_spare_OFF                           (26)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_spare_WID                           ( 3)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_spare_MSK                           (0x1C000000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_spare_MAX                           (0x00000007)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DisableQuickComp_OFF                (29)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DisableQuickComp_WID                ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DisableQuickComp_MSK                (0x20000000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_DisableQuickComp_MAX                (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_SinStep_OFF                         (30)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_SinStep_WID                         ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_SinStep_MSK                         (0x40000000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_SinStep_MAX                         (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_SinStepAdv_OFF                      (31)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_SinStepAdv_WID                      ( 1)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_SinStepAdv_MSK                      (0x80000000)
  #define DDRCOMP_CR_DDRCRCOMPCTL1_SinStepAdv_MAX                      (0x00000001)

#define DDRCOMP_CR_DDRCRCOMPVSSHI_REG                                (0x00003A1C)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_PanicDrvDnVref_OFF                 ( 0)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_PanicDrvDnVref_WID                 ( 6)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_PanicDrvDnVref_MSK                 (0x0000003F)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_PanicDrvDnVref_MAX                 (0x0000003F)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_PanicDrvUpVref_OFF                 ( 6)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_PanicDrvUpVref_WID                 ( 6)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_PanicDrvUpVref_MSK                 (0x00000FC0)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_PanicDrvUpVref_MAX                 (0x0000003F)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtOffset_OFF                       (12)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtOffset_WID                       ( 5)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtOffset_MSK                       (0x0001F000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtOffset_MAX                       (0x0000001F)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtSlopeA_OFF                       (17)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtSlopeA_WID                       ( 3)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtSlopeA_MSK                       (0x000E0000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtSlopeA_MAX                       (0x00000007)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtSlopeB_OFF                       (20)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtSlopeB_WID                       ( 3)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtSlopeB_MSK                       (0x00700000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_VtSlopeB_MAX                       (0x00000007)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_Spare_OFF                          (23)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_Spare_WID                          ( 9)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_Spare_MSK                          (0xFF800000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHI_Spare_MAX                          (0x000001FF)

#define DDRCOMP_CR_DDRCRCOMPOVR_REG                                  (0x00003A20)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqDrvU_OFF                           ( 0)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqDrvU_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqDrvU_MSK                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqDrvU_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqDrvD_OFF                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqDrvD_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqDrvD_MSK                           (0x00000002)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqDrvD_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqOdtU_OFF                           ( 2)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqOdtU_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqOdtU_MSK                           (0x00000004)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqOdtU_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqOdtD_OFF                           ( 3)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqOdtD_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqOdtD_MSK                           (0x00000008)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqOdtD_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdDrvU_OFF                          ( 4)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdDrvU_WID                          ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdDrvU_MSK                          (0x00000010)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdDrvU_MAX                          (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdDrvD_OFF                          ( 5)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdDrvD_WID                          ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdDrvD_MSK                          (0x00000020)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdDrvD_MAX                          (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlDrvU_OFF                          ( 6)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlDrvU_WID                          ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlDrvU_MSK                          (0x00000040)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlDrvU_MAX                          (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlDrvD_OFF                          ( 7)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlDrvD_WID                          ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlDrvD_MSK                          (0x00000080)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlDrvD_MAX                          (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkDrvU_OFF                          ( 8)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkDrvU_WID                          ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkDrvU_MSK                          (0x00000100)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkDrvU_MAX                          (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkDrvD_OFF                          ( 9)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkDrvD_WID                          ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkDrvD_MSK                          (0x00000200)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkDrvD_MAX                          (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqSR_OFF                             (10)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqSR_WID                             ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqSR_MSK                             (0x00000400)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqSR_MAX                             (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdSR_OFF                            (11)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdSR_WID                            ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdSR_MSK                            (0x00000800)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdSR_MAX                            (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlSR_OFF                            (12)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlSR_WID                            ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlSR_MSK                            (0x00001000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlSR_MAX                            (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkSR_OFF                            (13)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkSR_WID                            ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkSR_MSK                            (0x00002000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkSR_MAX                            (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqTcoOff_OFF                         (14)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqTcoOff_WID                         ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqTcoOff_MSK                         (0x00004000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqTcoOff_MAX                         (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdTcoOff_OFF                        (15)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdTcoOff_WID                        ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdTcoOff_MSK                        (0x00008000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdTcoOff_MAX                        (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqTco_OFF                            (16)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqTco_WID                            ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqTco_MSK                            (0x00010000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_DqTco_MAX                            (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdTco_OFF                           (17)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdTco_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdTco_MSK                           (0x00020000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CmdTco_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlTco_OFF                           (18)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlTco_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlTco_MSK                           (0x00040000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_CtlTco_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkTco_OFF                           (19)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkTco_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkTco_MSK                           (0x00080000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_ClkTco_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_Spare1_OFF                           (20)
  #define DDRCOMP_CR_DDRCRCOMPOVR_Spare1_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_Spare1_MSK                           (0x00100000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_Spare1_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_PanicDrvUp_OFF                       (21)
  #define DDRCOMP_CR_DDRCRCOMPOVR_PanicDrvUp_WID                       ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_PanicDrvUp_MSK                       (0x00200000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_PanicDrvUp_MAX                       (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_PanicDrvDn_OFF                       (22)
  #define DDRCOMP_CR_DDRCRCOMPOVR_PanicDrvDn_WID                       ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_PanicDrvDn_MSK                       (0x00400000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_PanicDrvDn_MAX                       (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_VTComp_OFF                           (23)
  #define DDRCOMP_CR_DDRCRCOMPOVR_VTComp_WID                           ( 1)
  #define DDRCOMP_CR_DDRCRCOMPOVR_VTComp_MSK                           (0x00800000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_VTComp_MAX                           (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPOVR_LsComp_OFF                           (24)
  #define DDRCOMP_CR_DDRCRCOMPOVR_LsComp_WID                           ( 3)
  #define DDRCOMP_CR_DDRCRCOMPOVR_LsComp_MSK                           (0x07000000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_LsComp_MAX                           (0x00000007)
  #define DDRCOMP_CR_DDRCRCOMPOVR_LsComp_DEF                           (0x00000004)
  #define DDRCOMP_CR_DDRCRCOMPOVR_Spare2_OFF                           (27)
  #define DDRCOMP_CR_DDRCRCOMPOVR_Spare2_WID                           ( 5)
  #define DDRCOMP_CR_DDRCRCOMPOVR_Spare2_MSK                           (0xF8000000)
  #define DDRCOMP_CR_DDRCRCOMPOVR_Spare2_MAX                           (0x0000001F)

#define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_REG                         (0x00003A24)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Target_OFF                  ( 0)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Target_WID                  ( 6)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Target_MSK                  (0x0000003F)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Target_MAX                  (0x0000003F)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Target_DEF                  (0x00000038)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_HiBWDivider_OFF             ( 6)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_HiBWDivider_WID             ( 2)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_HiBWDivider_MSK             (0x000000C0)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_HiBWDivider_MAX             (0x00000003)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_HiBWDivider_DEF             (0x00000000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_LoBWDivider_OFF             ( 8)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_LoBWDivider_WID             ( 2)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_LoBWDivider_MSK             (0x00000300)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_LoBWDivider_MAX             (0x00000003)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_LoBWDivider_DEF             (0x00000002)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_SampleDivider_OFF           (10)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_SampleDivider_WID           ( 3)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_SampleDivider_MSK           (0x00001C00)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_SampleDivider_MAX           (0x00000007)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_OpenLoop_OFF                (13)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_OpenLoop_WID                ( 1)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_OpenLoop_MSK                (0x00002000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_OpenLoop_MAX                (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_BWError_OFF                 (14)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_BWError_WID                 ( 2)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_BWError_MSK                 (0x0000C000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_BWError_MAX                 (0x00000003)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_BWError_DEF                 (0x00000002)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicEn_OFF                 (16)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicEn_WID                 ( 1)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicEn_MSK                 (0x00010000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicEn_MAX                 (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicEn_DEF                 (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Rsvd_OFF                    (17)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Rsvd_WID                    ( 1)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Rsvd_MSK                    (0x00020000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_Rsvd_MAX                    (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicVoltage_OFF            (18)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicVoltage_WID            ( 4)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicVoltage_MSK            (0x003C0000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicVoltage_MAX            (0x0000000F)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_PanicVoltage_DEF            (0x00000003)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_GainBoost_OFF               (22)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_GainBoost_WID               ( 1)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_GainBoost_MSK               (0x00400000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_GainBoost_MAX               (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_GainBoost_DEF               (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_SelCode_OFF                 (23)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_SelCode_WID                 ( 1)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_SelCode_MSK                 (0x00800000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_SelCode_MAX                 (0x00000001)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_OutputCode_OFF              (24)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_OutputCode_WID              ( 8)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_OutputCode_MSK              (0xFF000000)
  #define DDRCOMP_CR_DDRCRCOMPVSSHICONTROL_OutputCode_MAX              (0x000000FF)

#pragma pack(pop)
#endif  // __McIoComp_h__
