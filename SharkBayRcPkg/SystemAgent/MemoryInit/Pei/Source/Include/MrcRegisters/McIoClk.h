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

#ifndef _MC_IO_CLK_H_
#define _MC_IO_CLK_H_

#pragma pack(push, 1)
#include <MrcTypes.h>

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 Spare                                   :  28;  // Bits 31:4
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLK_CR_DDRCRCLKRANKSUSED_STRUCT;

typedef union {
  struct {
    U32 Scomp                                   :  5;  // Bits 4:0
    U32 TcoComp                                 :  6;  // Bits 10:5
    U32 RcompDrvUp                              :  6;  // Bits 16:11
    U32 RcompDrvDown                            :  6;  // Bits 22:17
    U32 LsComp                                  :  3;  // Bits 25:23
    U32 Spare                                   :  6;  // Bits 31:26
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLK_CR_DDRCRCLKCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  4;  // Bits 3:0
    U32 TcoCompOffset                           :  4;  // Bits 7:4
    U32 RcompDrvUpOffset                        :  4;  // Bits 11:8
    U32 RcompDrvDownOffset                      :  4;  // Bits 15:12
    U32 Spare                                   :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLK_CR_DDRCRCLKCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 PiSettingRank0                          :  7;  // Bits 6:0
    U32 PiSettingRank1                          :  7;  // Bits 13:7
    U32 PiSettingRank2                          :  7;  // Bits 20:14
    U32 PiSettingRank3                          :  7;  // Bits 27:21
    U32 Spare                                   :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLK_CR_DDRCRCLKPICODE_STRUCT;

typedef union {
  struct {
    U32 RefPi                                   :  4;  // Bits 3:0
    U32 DllMask                                 :  2;  // Bits 5:4
    U32 DllRsvd1                                :  1;  // Bits 6:6
    U32 TxOn                                    :  1;  // Bits 7:7
    U32 IntClkOn                                :  1;  // Bits 8:8
    U32 RepClkOn                                :  1;  // Bits 9:9
    U32 IOLBCtl                                 :  2;  // Bits 11:10
    U32 OdtMode                                 :  1;  // Bits 12:12
    U32 Reserved                                :  8;  // Bits 20:13
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 Spare                                   :  2;  // Bits 31:30
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLK_CR_DDRCRCLKCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RunTest                                 :  1;  // Bits 0:0
    U32 Load                                    :  1;  // Bits 1:1
    U32 ModeHVM                                 :  1;  // Bits 2:2
    U32 ModeDV                                  :  1;  // Bits 3:3
    U32 ModeADC                                 :  1;  // Bits 4:4
    U32 LoadCount                               :  10;  // Bits 14:5
    U32 CountStatus                             :  10;  // Bits 24:15
    U32 Spare                                   :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLK_CR_DLLPITESTANDADC_STRUCT;

typedef union {
  struct {
    U32 DllCB                                   :  2;  // Bits 1:0
    U32 Spare                                   :  30;  // Bits 31:2
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLK_CR_DDRCBSTATUS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 Spare                                   :  28;  // Bits 31:4
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH0_CR_DDRCRCLKRANKSUSED_STRUCT;

typedef union {
  struct {
    U32 Scomp                                   :  5;  // Bits 4:0
    U32 TcoComp                                 :  6;  // Bits 10:5
    U32 RcompDrvUp                              :  6;  // Bits 16:11
    U32 RcompDrvDown                            :  6;  // Bits 22:17
    U32 LsComp                                  :  3;  // Bits 25:23
    U32 Spare                                   :  6;  // Bits 31:26
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH0_CR_DDRCRCLKCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  4;  // Bits 3:0
    U32 TcoCompOffset                           :  4;  // Bits 7:4
    U32 RcompDrvUpOffset                        :  4;  // Bits 11:8
    U32 RcompDrvDownOffset                      :  4;  // Bits 15:12
    U32 Spare                                   :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 PiSettingRank0                          :  7;  // Bits 6:0
    U32 PiSettingRank1                          :  7;  // Bits 13:7
    U32 PiSettingRank2                          :  7;  // Bits 20:14
    U32 PiSettingRank3                          :  7;  // Bits 27:21
    U32 Spare                                   :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH0_CR_DDRCRCLKPICODE_STRUCT;

typedef union {
  struct {
    U32 RefPi                                   :  4;  // Bits 3:0
    U32 DllMask                                 :  2;  // Bits 5:4
    U32 DllRsvd1                                :  1;  // Bits 6:6
    U32 TxOn                                    :  1;  // Bits 7:7
    U32 IntClkOn                                :  1;  // Bits 8:8
    U32 RepClkOn                                :  1;  // Bits 9:9
    U32 IOLBCtl                                 :  2;  // Bits 11:10
    U32 OdtMode                                 :  1;  // Bits 12:12
    U32 Reserved                                :  8;  // Bits 20:13
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 Spare                                   :  2;  // Bits 31:30
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH0_CR_DDRCRCLKCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RunTest                                 :  1;  // Bits 0:0
    U32 Load                                    :  1;  // Bits 1:1
    U32 ModeHVM                                 :  1;  // Bits 2:2
    U32 ModeDV                                  :  1;  // Bits 3:3
    U32 ModeADC                                 :  1;  // Bits 4:4
    U32 LoadCount                               :  10;  // Bits 14:5
    U32 CountStatus                             :  10;  // Bits 24:15
    U32 Spare                                   :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH0_CR_DLLPITESTANDADC_STRUCT;

typedef union {
  struct {
    U32 DllCB                                   :  2;  // Bits 1:0
    U32 Spare                                   :  30;  // Bits 31:2
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH0_CR_DDRCBSTATUS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 Spare                                   :  28;  // Bits 31:4
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH1_CR_DDRCRCLKRANKSUSED_STRUCT;

typedef union {
  struct {
    U32 Scomp                                   :  5;  // Bits 4:0
    U32 TcoComp                                 :  6;  // Bits 10:5
    U32 RcompDrvUp                              :  6;  // Bits 16:11
    U32 RcompDrvDown                            :  6;  // Bits 22:17
    U32 LsComp                                  :  3;  // Bits 25:23
    U32 Spare                                   :  6;  // Bits 31:26
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH1_CR_DDRCRCLKCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  4;  // Bits 3:0
    U32 TcoCompOffset                           :  4;  // Bits 7:4
    U32 RcompDrvUpOffset                        :  4;  // Bits 11:8
    U32 RcompDrvDownOffset                      :  4;  // Bits 15:12
    U32 Spare                                   :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 PiSettingRank0                          :  7;  // Bits 6:0
    U32 PiSettingRank1                          :  7;  // Bits 13:7
    U32 PiSettingRank2                          :  7;  // Bits 20:14
    U32 PiSettingRank3                          :  7;  // Bits 27:21
    U32 Spare                                   :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH1_CR_DDRCRCLKPICODE_STRUCT;

typedef union {
  struct {
    U32 RefPi                                   :  4;  // Bits 3:0
    U32 DllMask                                 :  2;  // Bits 5:4
    U32 DllRsvd1                                :  1;  // Bits 6:6
    U32 TxOn                                    :  1;  // Bits 7:7
    U32 IntClkOn                                :  1;  // Bits 8:8
    U32 RepClkOn                                :  1;  // Bits 9:9
    U32 IOLBCtl                                 :  2;  // Bits 11:10
    U32 OdtMode                                 :  1;  // Bits 12:12
    U32 Reserved                                :  8;  // Bits 20:13
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 Spare                                   :  2;  // Bits 31:30
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH1_CR_DDRCRCLKCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RunTest                                 :  1;  // Bits 0:0
    U32 Load                                    :  1;  // Bits 1:1
    U32 ModeHVM                                 :  1;  // Bits 2:2
    U32 ModeDV                                  :  1;  // Bits 3:3
    U32 ModeADC                                 :  1;  // Bits 4:4
    U32 LoadCount                               :  10;  // Bits 14:5
    U32 CountStatus                             :  10;  // Bits 24:15
    U32 Spare                                   :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH1_CR_DLLPITESTANDADC_STRUCT;

typedef union {
  struct {
    U32 DllCB                                   :  2;  // Bits 1:0
    U32 Spare                                   :  30;  // Bits 31:2
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCLKCH1_CR_DDRCBSTATUS_STRUCT;


#define DDRCLK_CR_DDRCRCLKRANKSUSED_REG                              (0x00003900)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_RankEn_OFF                       ( 0)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_RankEn_WID                       ( 4)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_RankEn_MSK                       (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_RankEn_MAX                       (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_RankEn_DEF                       (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_Spare_OFF                        ( 4)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_Spare_WID                        (28)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_Spare_MSK                        (0xFFFFFFF0)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_Spare_MAX                        (0x0FFFFFFF)
  #define DDRCLK_CR_DDRCRCLKRANKSUSED_Spare_DEF                        (0x00000000)

#define DDRCLK_CR_DDRCRCLKCOMP_REG                                   (0x00003904)
  #define DDRCLK_CR_DDRCRCLKCOMP_Scomp_OFF                             ( 0)
  #define DDRCLK_CR_DDRCRCLKCOMP_Scomp_WID                             ( 5)
  #define DDRCLK_CR_DDRCRCLKCOMP_Scomp_MSK                             (0x0000001F)
  #define DDRCLK_CR_DDRCRCLKCOMP_Scomp_MAX                             (0x0000001F)
  #define DDRCLK_CR_DDRCRCLKCOMP_Scomp_DEF                             (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCOMP_TcoComp_OFF                           ( 5)
  #define DDRCLK_CR_DDRCRCLKCOMP_TcoComp_WID                           ( 6)
  #define DDRCLK_CR_DDRCRCLKCOMP_TcoComp_MSK                           (0x000007E0)
  #define DDRCLK_CR_DDRCRCLKCOMP_TcoComp_MAX                           (0x0000003F)
  #define DDRCLK_CR_DDRCRCLKCOMP_TcoComp_DEF                           (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvUp_OFF                        (11)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvUp_WID                        ( 6)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvUp_MSK                        (0x0001F800)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvUp_MAX                        (0x0000003F)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvUp_DEF                        (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvDown_OFF                      (17)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvDown_WID                      ( 6)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvDown_MSK                      (0x007E0000)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvDown_MAX                      (0x0000003F)
  #define DDRCLK_CR_DDRCRCLKCOMP_RcompDrvDown_DEF                      (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCOMP_LsComp_OFF                            (23)
  #define DDRCLK_CR_DDRCRCLKCOMP_LsComp_WID                            ( 3)
  #define DDRCLK_CR_DDRCRCLKCOMP_LsComp_MSK                            (0x03800000)
  #define DDRCLK_CR_DDRCRCLKCOMP_LsComp_MAX                            (0x00000007)
  #define DDRCLK_CR_DDRCRCLKCOMP_LsComp_DEF                            (0x00000004)
  #define DDRCLK_CR_DDRCRCLKCOMP_Spare_OFF                             (26)
  #define DDRCLK_CR_DDRCRCLKCOMP_Spare_WID                             ( 6)
  #define DDRCLK_CR_DDRCRCLKCOMP_Spare_MSK                             (0xFC000000)
  #define DDRCLK_CR_DDRCRCLKCOMP_Spare_MAX                             (0x0000003F)
  #define DDRCLK_CR_DDRCRCLKCOMP_Spare_DEF                             (0x00000000)

#define DDRCLK_CR_DDRCRCLKCOMPOFFSET_REG                             (0x00003908)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_ScompOffset_OFF                 ( 0)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_ScompOffset_WID                 ( 4)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_ScompOffset_MSK                 (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_ScompOffset_MAX                 (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_ScompOffset_DEF                 (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_OFF               ( 4)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_WID               ( 4)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_MSK               (0x000000F0)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_MAX               (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_DEF               (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_OFF            ( 8)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_WID            ( 4)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_MSK            (0x00000F00)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_MAX            (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_DEF            (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_OFF          (12)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_WID          ( 4)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_MSK          (0x0000F000)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_MAX          (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_DEF          (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_Spare_OFF                       (16)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_Spare_WID                       (16)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_Spare_MSK                       (0xFFFF0000)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_Spare_MAX                       (0x0000FFFF)
  #define DDRCLK_CR_DDRCRCLKCOMPOFFSET_Spare_DEF                       (0x00000000)

#define DDRCLK_CR_DDRCRCLKPICODE_REG                                 (0x0000390C)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank0_OFF                  ( 0)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank0_WID                  ( 7)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank0_MSK                  (0x0000007F)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank0_MAX                  (0x0000007F)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank0_DEF                  (0x00000000)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank1_OFF                  ( 7)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank1_WID                  ( 7)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank1_MSK                  (0x00003F80)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank1_MAX                  (0x0000007F)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank1_DEF                  (0x00000000)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank2_OFF                  (14)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank2_WID                  ( 7)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank2_MSK                  (0x001FC000)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank2_MAX                  (0x0000007F)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank2_DEF                  (0x00000000)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank3_OFF                  (21)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank3_WID                  ( 7)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank3_MSK                  (0x0FE00000)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank3_MAX                  (0x0000007F)
  #define DDRCLK_CR_DDRCRCLKPICODE_PiSettingRank3_DEF                  (0x00000000)
  #define DDRCLK_CR_DDRCRCLKPICODE_Spare_OFF                           (28)
  #define DDRCLK_CR_DDRCRCLKPICODE_Spare_WID                           ( 4)
  #define DDRCLK_CR_DDRCRCLKPICODE_Spare_MSK                           (0xF0000000)
  #define DDRCLK_CR_DDRCRCLKPICODE_Spare_MAX                           (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKPICODE_Spare_DEF                           (0x00000000)

#define DDRCLK_CR_DDRCRCLKCONTROLS_REG                               (0x00003910)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RefPi_OFF                         ( 0)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RefPi_WID                         ( 4)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RefPi_MSK                         (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RefPi_MAX                         (0x0000000F)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RefPi_DEF                         (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllMask_OFF                       ( 4)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllMask_WID                       ( 2)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllMask_MSK                       (0x00000030)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllMask_MAX                       (0x00000003)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllMask_DEF                       (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllRsvd1_OFF                      ( 6)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllRsvd1_WID                      ( 1)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllRsvd1_MSK                      (0x00000040)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllRsvd1_MAX                      (0x00000001)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllRsvd1_DEF                      (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_TxOn_OFF                          ( 7)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_TxOn_WID                          ( 1)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_TxOn_MSK                          (0x00000080)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_TxOn_MAX                          (0x00000001)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_TxOn_DEF                          (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IntClkOn_OFF                      ( 8)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IntClkOn_WID                      ( 1)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IntClkOn_MSK                      (0x00000100)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IntClkOn_MAX                      (0x00000001)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IntClkOn_DEF                      (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RepClkOn_OFF                      ( 9)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RepClkOn_WID                      ( 1)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RepClkOn_MSK                      (0x00000200)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RepClkOn_MAX                      (0x00000001)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RepClkOn_DEF                      (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IOLBCtl_OFF                       (10)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IOLBCtl_WID                       ( 2)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IOLBCtl_MSK                       (0x00000C00)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IOLBCtl_MAX                       (0x00000003)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_IOLBCtl_DEF                       (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_OdtMode_OFF                       (12)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_OdtMode_WID                       ( 1)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_OdtMode_MSK                       (0x00001000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_OdtMode_MAX                       (0x00000001)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_OdtMode_DEF                       (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Reserved_OFF                      (13)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Reserved_WID                      ( 8)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Reserved_MSK                      (0x001FE000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Reserved_MAX                      (0x000000FF)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Reserved_DEF                      (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RxVref_OFF                        (21)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RxVref_WID                        ( 6)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RxVref_MSK                        (0x07E00000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RxVref_MAX                        (0x0000003F)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_RxVref_DEF                        (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_VccddqHi_OFF                      (27)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_VccddqHi_WID                      ( 1)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_VccddqHi_MSK                      (0x08000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_VccddqHi_MAX                      (0x00000001)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_VccddqHi_DEF                      (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllWeakLock_OFF                   (28)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllWeakLock_WID                   ( 1)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllWeakLock_MSK                   (0x10000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllWeakLock_MAX                   (0x00000001)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_DllWeakLock_DEF                   (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_LPDDR_Mode_OFF                    (29)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_LPDDR_Mode_WID                    ( 1)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_LPDDR_Mode_MSK                    (0x20000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_LPDDR_Mode_MAX                    (0x00000001)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_LPDDR_Mode_DEF                    (0x00000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Spare_OFF                         (30)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Spare_WID                         ( 2)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Spare_MSK                         (0xC0000000)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Spare_MAX                         (0x00000003)
  #define DDRCLK_CR_DDRCRCLKCONTROLS_Spare_DEF                         (0x00000000)

#define DDRCLK_CR_DLLPITESTANDADC_REG                                (0x00003914)
  #define DDRCLK_CR_DLLPITESTANDADC_RunTest_OFF                        ( 0)
  #define DDRCLK_CR_DLLPITESTANDADC_RunTest_WID                        ( 1)
  #define DDRCLK_CR_DLLPITESTANDADC_RunTest_MSK                        (0x00000001)
  #define DDRCLK_CR_DLLPITESTANDADC_RunTest_MAX                        (0x00000001)
  #define DDRCLK_CR_DLLPITESTANDADC_RunTest_DEF                        (0x00000000)
  #define DDRCLK_CR_DLLPITESTANDADC_Load_OFF                           ( 1)
  #define DDRCLK_CR_DLLPITESTANDADC_Load_WID                           ( 1)
  #define DDRCLK_CR_DLLPITESTANDADC_Load_MSK                           (0x00000002)
  #define DDRCLK_CR_DLLPITESTANDADC_Load_MAX                           (0x00000001)
  #define DDRCLK_CR_DLLPITESTANDADC_Load_DEF                           (0x00000000)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeHVM_OFF                        ( 2)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeHVM_WID                        ( 1)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeHVM_MSK                        (0x00000004)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeHVM_MAX                        (0x00000001)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeHVM_DEF                        (0x00000000)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeDV_OFF                         ( 3)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeDV_WID                         ( 1)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeDV_MSK                         (0x00000008)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeDV_MAX                         (0x00000001)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeDV_DEF                         (0x00000000)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeADC_OFF                        ( 4)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeADC_WID                        ( 1)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeADC_MSK                        (0x00000010)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeADC_MAX                        (0x00000001)
  #define DDRCLK_CR_DLLPITESTANDADC_ModeADC_DEF                        (0x00000000)
  #define DDRCLK_CR_DLLPITESTANDADC_LoadCount_OFF                      ( 5)
  #define DDRCLK_CR_DLLPITESTANDADC_LoadCount_WID                      (10)
  #define DDRCLK_CR_DLLPITESTANDADC_LoadCount_MSK                      (0x00007FE0)
  #define DDRCLK_CR_DLLPITESTANDADC_LoadCount_MAX                      (0x000003FF)
  #define DDRCLK_CR_DLLPITESTANDADC_CountStatus_OFF                    (15)
  #define DDRCLK_CR_DLLPITESTANDADC_CountStatus_WID                    (10)
  #define DDRCLK_CR_DLLPITESTANDADC_CountStatus_MSK                    (0x01FF8000)
  #define DDRCLK_CR_DLLPITESTANDADC_CountStatus_MAX                    (0x000003FF)
  #define DDRCLK_CR_DLLPITESTANDADC_CountStatus_DEF                    (0x00000000)
  #define DDRCLK_CR_DLLPITESTANDADC_Spare_OFF                          (25)
  #define DDRCLK_CR_DLLPITESTANDADC_Spare_WID                          ( 7)
  #define DDRCLK_CR_DLLPITESTANDADC_Spare_MSK                          (0xFE000000)
  #define DDRCLK_CR_DLLPITESTANDADC_Spare_MAX                          (0x0000007F)

#define DDRCLK_CR_DDRCBSTATUS_REG                                    (0x00003918)
  #define DDRCLK_CR_DDRCBSTATUS_DllCB_OFF                              ( 0)
  #define DDRCLK_CR_DDRCBSTATUS_DllCB_WID                              ( 2)
  #define DDRCLK_CR_DDRCBSTATUS_DllCB_MSK                              (0x00000003)
  #define DDRCLK_CR_DDRCBSTATUS_DllCB_MAX                              (0x00000003)
  #define DDRCLK_CR_DDRCBSTATUS_DllCB_DEF                              (0x00000000)
  #define DDRCLK_CR_DDRCBSTATUS_Spare_OFF                              ( 2)
  #define DDRCLK_CR_DDRCBSTATUS_Spare_WID                              (30)
  #define DDRCLK_CR_DDRCBSTATUS_Spare_MSK                              (0xFFFFFFFC)
  #define DDRCLK_CR_DDRCBSTATUS_Spare_MAX                              (0x3FFFFFFF)
  #define DDRCLK_CR_DDRCBSTATUS_Spare_DEF                              (0x00000000)

#define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_REG                           (0x00001800)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_RankEn_OFF                    ( 0)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_RankEn_WID                    ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_RankEn_MSK                    (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_RankEn_MAX                    (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_RankEn_DEF                    (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_Spare_OFF                     ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_Spare_WID                     (28)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_Spare_MSK                     (0xFFFFFFF0)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_Spare_MAX                     (0x0FFFFFFF)
  #define DDRCLKCH0_CR_DDRCRCLKRANKSUSED_Spare_DEF                     (0x00000000)

#define DDRCLKCH0_CR_DDRCRCLKCOMP_REG                                (0x00001804)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Scomp_OFF                          ( 0)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Scomp_WID                          ( 5)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Scomp_MSK                          (0x0000001F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Scomp_MAX                          (0x0000001F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_TcoComp_OFF                        ( 5)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_TcoComp_WID                        ( 6)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_TcoComp_MSK                        (0x000007E0)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvUp_OFF                     (11)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvUp_MSK                     (0x0001F800)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvDown_OFF                   (17)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvDown_MSK                   (0x007E0000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_LsComp_OFF                         (23)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_LsComp_WID                         ( 3)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_LsComp_MSK                         (0x03800000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_LsComp_DEF                         (0x00000004)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Spare_OFF                          (26)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Spare_WID                          ( 6)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Spare_MSK                          (0xFC000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Spare_MAX                          (0x0000003F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMP_Spare_DEF                          (0x00000000)

#define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_REG                          (0x00001808)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_ScompOffset_WID              ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_ScompOffset_MSK              (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_ScompOffset_MAX              (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_OFF            ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_MSK            (0x000000F0)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_OFF         ( 8)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00000F00)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_OFF       (12)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0000F000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_Spare_OFF                    (16)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_Spare_WID                    (16)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_Spare_MSK                    (0xFFFF0000)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_Spare_MAX                    (0x0000FFFF)
  #define DDRCLKCH0_CR_DDRCRCLKCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCLKCH0_CR_DDRCRCLKPICODE_REG                              (0x0000180C)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_OFF               ( 0)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_WID               ( 7)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_MSK               (0x0000007F)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_MAX               (0x0000007F)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank0_DEF               (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank1_OFF               ( 7)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank1_WID               ( 7)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank1_MSK               (0x00003F80)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank1_MAX               (0x0000007F)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank1_DEF               (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank2_OFF               (14)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank2_WID               ( 7)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank2_MSK               (0x001FC000)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank2_MAX               (0x0000007F)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank2_DEF               (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank3_OFF               (21)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank3_WID               ( 7)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank3_MSK               (0x0FE00000)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank3_MAX               (0x0000007F)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_PiSettingRank3_DEF               (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_Spare_OFF                        (28)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_Spare_WID                        ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_Spare_MSK                        (0xF0000000)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_Spare_MAX                        (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKPICODE_Spare_DEF                        (0x00000000)

#define DDRCLKCH0_CR_DDRCRCLKCONTROLS_REG                            (0x00001810)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RefPi_WID                      ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllMask_WID                    ( 2)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_TxOn_WID                       ( 1)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_OdtMode_OFF                    (12)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Reserved_OFF                   (13)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Reserved_WID                   ( 8)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Reserved_MSK                   (0x001FE000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Reserved_MAX                   (0x000000FF)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Reserved_DEF                   (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RxVref_OFF                     (21)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RxVref_WID                     ( 6)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Spare_OFF                      (30)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Spare_WID                      ( 2)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Spare_MSK                      (0xC0000000)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Spare_MAX                      (0x00000003)
  #define DDRCLKCH0_CR_DDRCRCLKCONTROLS_Spare_DEF                      (0x00000000)

#define DDRCLKCH0_CR_DLLPITESTANDADC_REG                             (0x00001814)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCLKCH0_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#define DDRCLKCH0_CR_DDRCBSTATUS_REG                                 (0x00001818)
  #define DDRCLKCH0_CR_DDRCBSTATUS_DllCB_OFF                           ( 0)
  #define DDRCLKCH0_CR_DDRCBSTATUS_DllCB_WID                           ( 2)
  #define DDRCLKCH0_CR_DDRCBSTATUS_DllCB_MSK                           (0x00000003)
  #define DDRCLKCH0_CR_DDRCBSTATUS_DllCB_MAX                           (0x00000003)
  #define DDRCLKCH0_CR_DDRCBSTATUS_DllCB_DEF                           (0x00000000)
  #define DDRCLKCH0_CR_DDRCBSTATUS_Spare_OFF                           ( 2)
  #define DDRCLKCH0_CR_DDRCBSTATUS_Spare_WID                           (30)
  #define DDRCLKCH0_CR_DDRCBSTATUS_Spare_MSK                           (0xFFFFFFFC)
  #define DDRCLKCH0_CR_DDRCBSTATUS_Spare_MAX                           (0x3FFFFFFF)
  #define DDRCLKCH0_CR_DDRCBSTATUS_Spare_DEF                           (0x00000000)

#define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_REG                           (0x00001900)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_RankEn_OFF                    ( 0)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_RankEn_WID                    ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_RankEn_MSK                    (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_RankEn_MAX                    (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_RankEn_DEF                    (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_Spare_OFF                     ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_Spare_WID                     (28)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_Spare_MSK                     (0xFFFFFFF0)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_Spare_MAX                     (0x0FFFFFFF)
  #define DDRCLKCH1_CR_DDRCRCLKRANKSUSED_Spare_DEF                     (0x00000000)

#define DDRCLKCH1_CR_DDRCRCLKCOMP_REG                                (0x00001904)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Scomp_OFF                          ( 0)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Scomp_WID                          ( 5)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Scomp_MSK                          (0x0000001F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Scomp_MAX                          (0x0000001F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_TcoComp_OFF                        ( 5)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_TcoComp_WID                        ( 6)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_TcoComp_MSK                        (0x000007E0)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvUp_OFF                     (11)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvUp_MSK                     (0x0001F800)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvDown_OFF                   (17)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvDown_MSK                   (0x007E0000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_LsComp_OFF                         (23)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_LsComp_WID                         ( 3)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_LsComp_MSK                         (0x03800000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_LsComp_DEF                         (0x00000004)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Spare_OFF                          (26)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Spare_WID                          ( 6)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Spare_MSK                          (0xFC000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Spare_MAX                          (0x0000003F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMP_Spare_DEF                          (0x00000000)

#define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_REG                          (0x00001908)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_ScompOffset_WID              ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_ScompOffset_MSK              (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_ScompOffset_MAX              (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_OFF            ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_MSK            (0x000000F0)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_OFF         ( 8)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00000F00)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_OFF       (12)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0000F000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_Spare_OFF                    (16)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_Spare_WID                    (16)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_Spare_MSK                    (0xFFFF0000)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_Spare_MAX                    (0x0000FFFF)
  #define DDRCLKCH1_CR_DDRCRCLKCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCLKCH1_CR_DDRCRCLKPICODE_REG                              (0x0000190C)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank0_OFF               ( 0)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank0_WID               ( 7)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank0_MSK               (0x0000007F)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank0_MAX               (0x0000007F)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank0_DEF               (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank1_OFF               ( 7)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank1_WID               ( 7)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank1_MSK               (0x00003F80)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank1_MAX               (0x0000007F)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank1_DEF               (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank2_OFF               (14)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank2_WID               ( 7)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank2_MSK               (0x001FC000)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank2_MAX               (0x0000007F)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank2_DEF               (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank3_OFF               (21)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank3_WID               ( 7)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank3_MSK               (0x0FE00000)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank3_MAX               (0x0000007F)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_PiSettingRank3_DEF               (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_Spare_OFF                        (28)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_Spare_WID                        ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_Spare_MSK                        (0xF0000000)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_Spare_MAX                        (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKPICODE_Spare_DEF                        (0x00000000)

#define DDRCLKCH1_CR_DDRCRCLKCONTROLS_REG                            (0x00001910)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RefPi_WID                      ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllMask_WID                    ( 2)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_TxOn_WID                       ( 1)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_OdtMode_OFF                    (12)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Reserved_OFF                   (13)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Reserved_WID                   ( 8)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Reserved_MSK                   (0x001FE000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Reserved_MAX                   (0x000000FF)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Reserved_DEF                   (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RxVref_OFF                     (21)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RxVref_WID                     ( 6)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Spare_OFF                      (30)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Spare_WID                      ( 2)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Spare_MSK                      (0xC0000000)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Spare_MAX                      (0x00000003)
  #define DDRCLKCH1_CR_DDRCRCLKCONTROLS_Spare_DEF                      (0x00000000)

#define DDRCLKCH1_CR_DLLPITESTANDADC_REG                             (0x00001914)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCLKCH1_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#define DDRCLKCH1_CR_DDRCBSTATUS_REG                                 (0x00001918)
  #define DDRCLKCH1_CR_DDRCBSTATUS_DllCB_OFF                           ( 0)
  #define DDRCLKCH1_CR_DDRCBSTATUS_DllCB_WID                           ( 2)
  #define DDRCLKCH1_CR_DDRCBSTATUS_DllCB_MSK                           (0x00000003)
  #define DDRCLKCH1_CR_DDRCBSTATUS_DllCB_MAX                           (0x00000003)
  #define DDRCLKCH1_CR_DDRCBSTATUS_DllCB_DEF                           (0x00000000)
  #define DDRCLKCH1_CR_DDRCBSTATUS_Spare_OFF                           ( 2)
  #define DDRCLKCH1_CR_DDRCBSTATUS_Spare_WID                           (30)
  #define DDRCLKCH1_CR_DDRCBSTATUS_Spare_MSK                           (0xFFFFFFFC)
  #define DDRCLKCH1_CR_DDRCBSTATUS_Spare_MAX                           (0x3FFFFFFF)
  #define DDRCLKCH1_CR_DDRCBSTATUS_Spare_DEF                           (0x00000000)

#pragma pack(pop)
#endif  // __McIoClk_h__
