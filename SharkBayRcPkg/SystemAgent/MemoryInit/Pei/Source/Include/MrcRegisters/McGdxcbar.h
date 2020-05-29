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

#ifndef _MC_GDXCBAR_H_
#define _MC_GDXCBAR_H_

#pragma pack(push, 1)
#include <MrcTypes.h>

typedef union {
  struct {
    U32 Tx_Delay_R0                             :  8;  // Bits 7:0
    U32 Tx_Delay_R1                             :  8;  // Bits 15:8
    U32 Tx_Delay_R2                             :  8;  // Bits 23:16
    U32 Tx_Delay_R3                             :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRPL_CR_DDR_TX_DELAY_STRUCT;

typedef union {
  struct {
    U32 EN_SW_GDXC                              :  1;  // Bits 0:0
    U32 EN_RING_ADQ                             :  1;  // Bits 1:1
    U32 EN_RING_BLQ                             :  1;  // Bits 2:2
    U32 EN_RING_AKQ                             :  1;  // Bits 3:3
    U32 EN_RING_IVQ                             :  1;  // Bits 4:4
    U32 EN_IDIQ                                 :  1;  // Bits 5:5
    U32 EN_mc_UCLKQ                             :  1;  // Bits 6:6
    U32                                         :  1;  // Bits 7:7
    U32 UP_EN_ADQ                               :  1;  // Bits 8:8
    U32 DN_EN_ADQ                               :  1;  // Bits 9:9
    U32 UP_EN_BLQ                               :  1;  // Bits 10:10
    U32 DN_EN_BLQ                               :  1;  // Bits 11:11
    U32 UP_EN_AKQ                               :  1;  // Bits 12:12
    U32 DN_EN_AKQ                               :  1;  // Bits 13:13
    U32 MOTQ_TIMING_SELECT                      :  1;  // Bits 14:14
    U32                                         :  1;  // Bits 15:15
    U32 SPARE                                   :  8;  // Bits 23:16
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_SW_ENABLE_STRUCT;

typedef union {
  struct {
    U32 EN_Core0_T0                             :  1;  // Bits 0:0
    U32 EN_Core0_T1                             :  1;  // Bits 1:1
    U32 EN_Core1_T0                             :  1;  // Bits 2:2
    U32 EN_Core1_T1                             :  1;  // Bits 3:3
    U32 EN_Core2_T0                             :  1;  // Bits 4:4
    U32 EN_Core2_T1                             :  1;  // Bits 5:5
    U32 EN_Core3_T0                             :  1;  // Bits 6:6
    U32 EN_Core3_T1                             :  1;  // Bits 7:7
    U32 EN_GT                                   :  1;  // Bits 8:8
    U32 MEM_CHR_RD                              :  1;  // Bits 9:9
    U32 MEM_CHR_WR                              :  1;  // Bits 10:10
    U32 MEM_NC                                  :  1;  // Bits 11:11
    U32 EN_CBO_Exp_WB                           :  1;  // Bits 12:12
    U32 SNP_Access                              :  1;  // Bits 13:13
    U32 AD_EODLAT                               :  1;  // Bits 14:14
    U32                                         :  17;  // Bits 31:15
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_STRUCT;

typedef union {
  struct {
    U32 CR_CHR_RD                               :  3;  // Bits 2:0
    U32 CHR_RD_STAT_MOD                         :  1;  // Bits 3:3
    U32 CR_CHR_WR                               :  4;  // Bits 7:4
    U32 CR_NC_RD                                :  3;  // Bits 10:8
    U32 NC_RD_STAT_MOD                          :  1;  // Bits 11:11
    U32 CR_NC_WR                                :  4;  // Bits 15:12
    U32 NC_WR_STAT_MOD                          :  1;  // Bits 16:16
    U32 Data_Core0                              :  1;  // Bits 17:17
    U32 Data_Core1                              :  1;  // Bits 18:18
    U32 Data_Core2                              :  1;  // Bits 19:19
    U32 Data_Core3                              :  1;  // Bits 20:20
    U32 Data_CBO                                :  1;  // Bits 21:21
    U32 Data_GT                                 :  1;  // Bits 22:22
    U32                                         :  9;  // Bits 31:23
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_QUALIFIER_BL_STRUCT;

typedef union {
  struct {
    U32 OVF_Global                              :  1;  // Bits 0:0
    U32 OVF_Ring_AD                             :  1;  // Bits 1:1
    U32 OVF_Ring_BL                             :  1;  // Bits 2:2
    U32 OVF_Ring_AK                             :  1;  // Bits 3:3
    U32 OVF_Ring_IV                             :  1;  // Bits 4:4
    U32                                         :  4;  // Bits 8:5
    U32 OVF_IDI_center                          :  1;  // Bits 9:9
    U32 OVF_mcUCLK                              :  1;  // Bits 10:10
    U32 OVF_PWR_mcFCLK                          :  1;  // Bits 11:11
    U32                                         :  20;  // Bits 31:12
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_OVF_STATUS_STRUCT;

typedef union {
  struct {
    U32 MEM_PTR                                 :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_MOT_ADDRESS_LO_STRUCT;

typedef union {
  struct {
    U32 MEM_PTR                                 :  1;  // Bits 0:0
    U32 BUFFER_WRAP                             :  1;  // Bits 1:1
    U32 SPARE                                   :  30;  // Bits 31:2
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_STRUCT;

typedef union {
  struct {
    U32 START_ADDRESS                           :  16;  // Bits 15:0
    U32 END_ADDRESS                             :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_MOT_REGION_STRUCT;

typedef union {
  struct {
    U32 MEM_PTR                                 :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_OCLA_ADDRESS_LO_STRUCT;

typedef union {
  struct {
    U32 MEM_PTR                                 :  1;  // Bits 0:0
    U32 BUFFER_WRAP                             :  1;  // Bits 1:1
    U32 LOCK                                    :  1;  // Bits 2:2
    U32 SPARE                                   :  29;  // Bits 31:3
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_STRUCT;

typedef union {
  struct {
    U32 START_ADDRESS                           :  16;  // Bits 15:0
    U32 END_ADDRESS                             :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCOHTRK_CR_GDXC_OCLA_REGION_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S0L_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S0H_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S0L_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S0H_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S1L_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S1H_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S1L_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S1H_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S2L_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S2H_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S2L_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S2H_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S3L_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S3H_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S3L_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S3H_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S4L_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S4H_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S4L_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S4H_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S5L_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S5H_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S5L_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S5H_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S6L_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S6H_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S6L_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S6H_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S7L_STRUCT;

typedef union {
  struct {
    U32 ARM_bits_packet                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_ARM_S7H_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S7L_STRUCT;

typedef union {
  struct {
    U32 Mask_bits                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MASK_S7H_STRUCT;

typedef union {
  struct {
    U32 UX_TMR_BASE_A                           :  1;  // Bits 0:0
    U32 UX_TMR_BASE_B                           :  1;  // Bits 1:1
    U32 OVF_ARM_IDI                             :  1;  // Bits 2:2
    U32 OVF_ARM_HIGH                            :  5;  // Bits 7:3
    U32 OVF_Mask_HIGH                           :  5;  // Bits 12:8
    U32                                         :  5;  // Bits 17:13
    U32 TimerA_units                            :  3;  // Bits 20:18
    U32 TimerB_units                            :  3;  // Bits 23:21
    U32 Which_Time_Base                         :  4;  // Bits 27:24
    U32 Tmr_Or_Cntr_Mode                        :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_CMD_STRUCT;

typedef union {
  struct {
    U32 EN_SW_G_ODLAT                           :  1;  // Bits 0:0
    U32                                         :  7;  // Bits 7:1
    U32 EN_S0                                   :  1;  // Bits 8:8
    U32 EN_S1                                   :  1;  // Bits 9:9
    U32 EN_S2                                   :  1;  // Bits 10:10
    U32 EN_S3                                   :  1;  // Bits 11:11
    U32 EN_S4                                   :  1;  // Bits 12:12
    U32 EN_S5                                   :  1;  // Bits 13:13
    U32 EN_S6                                   :  1;  // Bits 14:14
    U32 EN_S7                                   :  1;  // Bits 15:15
    U32 MBP_Enable                              :  1;  // Bits 16:16
    U32                                         :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_SW_ENABLE_STRUCT;

typedef union {
  struct {
    U32 Pulse_or_Sticky_Events                  :  8;  // Bits 7:0
    U32 Pulse_or_Sticky_MBP                     :  1;  // Bits 8:8
    U32 Pulse_or_Sticky_TO                      :  4;  // Bits 12:9
    U32 OVF_ARM_LOW                             :  3;  // Bits 15:13
    U32 OVF_MASK_LOW                            :  3;  // Bits 18:16
    U32 Which_MBP_Pin_A                         :  3;  // Bits 21:19
    U32 Which_MBP_Pin_B                         :  3;  // Bits 24:22
    U32 Which_MBP_Pin_C                         :  3;  // Bits 27:25
    U32 Which_MBP_Pin_D                         :  3;  // Bits 30:28
    U32                                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MISC_CMD_STRUCT;

typedef union {
  struct {
    U32 Ln0Mask                                 :  8;  // Bits 7:0
    U32 Ln1Mask                                 :  8;  // Bits 15:8
    U32 AssertMode                              :  1;  // Bits 16:16
    U32 CompEn                                  :  1;  // Bits 17:17
    U32 GT_VISA_En                              :  1;  // Bits 18:18
    U32                                         :  13;  // Bits 31:19
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GDXC_CR_GT_VISA2OCLA_CFG_FILTER_STRUCT;

typedef union {
  struct {
    U32 Man_Reset_GDXC                          :  1;  // Bits 0:0
    U32 Man_Reset_G_ODLAT                       :  1;  // Bits 1:1
    U32 IOT_Start                               :  1;  // Bits 2:2
    U32 IOT_Stop                                :  1;  // Bits 3:3
    U32 IOT_Trigger                             :  1;  // Bits 4:4
    U32 IOT_Force_Flush                         :  1;  // Bits 5:5
    U32                                         :  10;  // Bits 15:6
    U32 SPARE                                   :  8;  // Bits 23:16
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GDXC_CR_GDXC_MAN_CONFIG_STRUCT;

typedef union {
  struct {
    U32 mbpout                                  :  2;  // Bits 1:0
    U32                                         :  30;  // Bits 31:2
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GDXC_CR_GDXC_ALIGN_STRUCT;

typedef union {
  struct {
    U32 AD                                      :  6;  // Bits 5:0
    U32 BLHDR0                                  :  7;  // Bits 12:6
    U32 BLHDR1ADDR                              :  6;  // Bits 18:13
    U32 AK                                      :  6;  // Bits 24:19
    U32 IV                                      :  2;  // Bits 26:25
    U32 Wrap                                    :  3;  // Bits 29:27
    U32 MOT                                     :  5;  // Bits 34:30
    U32 IDI                                     :  5;  // Bits 39:35
    U32 FClk                                    :  5;  // Bits 44:40
    U32 UClkMsgCh                               :  5;  // Bits 49:45
    U32                                         :  14;  // Bits 63:50
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} GDXC_CR_GDXC_RR_ARB_THRESH_STRUCT;

typedef union {
  struct {
    U32 Start_Status                            :  1;  // Bits 0:0
    U32 Trigger_Status                          :  1;  // Bits 1:1
    U32 Stop_Status                             :  1;  // Bits 2:2
    U32 Muliple_Hit                             :  1;  // Bits 3:3
    U32 Bubbles_Status                          :  6;  // Bits 9:4
    U32                                         :  22;  // Bits 31:10
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GDXC_CR_G_ODLAT_FIRE_STATUS_STRUCT;

typedef union {
  struct {
    U32 Delay_count                             :  25;  // Bits 24:0
    U32                                         :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GDXC_CR_TRIGGER_TO_STOP_DELAY_STRUCT;

typedef union {
  struct {
    U32 Disabled                                :  1;  // Bits 0:0
    U32 Idle                                    :  1;  // Bits 1:1
    U32 Running                                 :  1;  // Bits 2:2
    U32 IOT_Triggered                           :  1;  // Bits 3:3
    U32 Sticky_triggered                        :  1;  // Bits 4:4
    U32                                         :  1;  // Bits 5:5
    U32 Remaining_count                         :  25;  // Bits 30:6
    U32                                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GDXC_CR_IOT_STATUS_STRUCT;

typedef union {
  struct {
    U32 S0_OclaHdr                              :  8;  // Bits 7:0
    U32 S1_OclaHdr                              :  8;  // Bits 15:8
    U32 S2_OclaHdr                              :  8;  // Bits 23:16
    U32 S3_OclaHdr                              :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_STRUCT;

typedef union {
  struct {
    U32 S4_OclaHdr                              :  8;  // Bits 7:0
    U32 S5_OclaHdr                              :  8;  // Bits 15:8
    U32 S6_OclaHdr                              :  8;  // Bits 23:16
    U32 S7_OclaHdr                              :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_STRUCT;

typedef union {
  struct {
    U32 S0_FST                                  :  2;  // Bits 1:0
    U32 S1_FST                                  :  2;  // Bits 3:2
    U32 S2_FST                                  :  2;  // Bits 5:4
    U32 S3_FST                                  :  2;  // Bits 7:6
    U32 S4_FST                                  :  2;  // Bits 9:8
    U32 S5_FST                                  :  2;  // Bits 11:10
    U32 S6_FST                                  :  2;  // Bits 13:12
    U32 S7_FST                                  :  2;  // Bits 15:14
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_FST_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32 Stop                                    :  1;  // Bits 13:13
    U32                                         :  2;  // Bits 15:14
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32 Col_Rst                                 :  1;  // Bits 29:29
    U32 Trigger                                 :  1;  // Bits 30:30
    U32 Start                                   :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32                                         :  3;  // Bits 15:13
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_STRUCT;

typedef union {
  struct {
    U32 Arm                                     :  8;  // Bits 7:0
    U32 MBP_Arm                                 :  1;  // Bits 8:8
    U32 TO_Arm                                  :  4;  // Bits 12:9
    U32 Stop                                    :  1;  // Bits 13:13
    U32                                         :  2;  // Bits 15:14
    U32 Mask                                    :  8;  // Bits 23:16
    U32 MBP_Mask                                :  1;  // Bits 24:24
    U32 TO_Mask                                 :  4;  // Bits 28:25
    U32 Col_Rst                                 :  1;  // Bits 29:29
    U32 Trigger                                 :  1;  // Bits 30:30
    U32 Start                                   :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_STRUCT;

typedef union {
  struct {
    U32 B0_Set                                  :  6;  // Bits 5:0
    U32 B0_Rst                                  :  6;  // Bits 11:6
    U32 B1_Set                                  :  6;  // Bits 17:12
    U32 B1_Rst                                  :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_STRUCT;

typedef union {
  struct {
    U32 B2_Set                                  :  6;  // Bits 5:0
    U32 B2_Rst                                  :  6;  // Bits 11:6
    U32 B3_Set                                  :  6;  // Bits 17:12
    U32 B3_Rst                                  :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_STRUCT;

typedef union {
  struct {
    U32 B4_Set                                  :  6;  // Bits 5:0
    U32 B4_Rst                                  :  6;  // Bits 11:6
    U32 B5_Set                                  :  6;  // Bits 17:12
    U32 B5_Rst                                  :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_STRUCT;

typedef union {
  struct {
    U32 ElseIf_ElseIf_B0_Set                    :  6;  // Bits 5:0
    U32 ElseIf_ElseIf_B0_Rst                    :  6;  // Bits 11:6
    U32                                         :  20;  // Bits 31:12
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_STRUCT;

typedef union {
  struct {
    U32 B0_Set                                  :  6;  // Bits 5:0
    U32 B0_Rst                                  :  6;  // Bits 11:6
    U32 B1_Set                                  :  6;  // Bits 17:12
    U32 B1_Rst                                  :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_STRUCT;

typedef union {
  struct {
    U32 B2_Set                                  :  6;  // Bits 5:0
    U32 B2_Rst                                  :  6;  // Bits 11:6
    U32 B3_Set                                  :  6;  // Bits 17:12
    U32 B3_Rst                                  :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_STRUCT;

typedef union {
  struct {
    U32 B4_Set                                  :  6;  // Bits 5:0
    U32 B4_Rst                                  :  6;  // Bits 11:6
    U32 B5_Set                                  :  6;  // Bits 17:12
    U32 B5_Rst                                  :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_STRUCT;

typedef union {
  struct {
    U32 ElseIf_ElseIf_B1_Set                    :  6;  // Bits 5:0
    U32 ElseIf_ElseIf_B1_Rst                    :  6;  // Bits 11:6
    U32                                         :  20;  // Bits 31:12
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_STRUCT;

typedef union {
  struct {
    U32 Init_Bits                               :  6;  // Bits 5:0
    U32                                         :  26;  // Bits 31:6
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_INIT_BUBBLES_STRUCT;

typedef union {
  struct {
    U32 ColRst                                  :  6;  // Bits 5:0
    U32 Start                                   :  6;  // Bits 11:6
    U32 Trigger                                 :  6;  // Bits 17:12
    U32 Stop                                    :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_STRUCT;

typedef union {
  struct {
    U32 ColRst                                  :  6;  // Bits 5:0
    U32 Start                                   :  6;  // Bits 11:6
    U32 Trigger                                 :  6;  // Bits 17:12
    U32 Stop                                    :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_STRUCT;

typedef union {
  struct {
    U32 B0_EN                                   :  4;  // Bits 3:0
    U32 B1_EN                                   :  4;  // Bits 7:4
    U32 B2_EN                                   :  4;  // Bits 11:8
    U32 B3_EN                                   :  4;  // Bits 15:12
    U32 B4_EN                                   :  4;  // Bits 19:16
    U32 B5_EN                                   :  4;  // Bits 23:20
    U32 ELSEIF_ELSEIF_B0_EN                     :  4;  // Bits 27:24
    U32                                         :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_STRUCT;

typedef union {
  struct {
    U32 B0_EN                                   :  4;  // Bits 3:0
    U32 B1_EN                                   :  4;  // Bits 7:4
    U32 B2_EN                                   :  4;  // Bits 11:8
    U32 B3_EN                                   :  4;  // Bits 15:12
    U32 B4_EN                                   :  4;  // Bits 19:16
    U32 B5_EN                                   :  4;  // Bits 23:20
    U32 ELSEIF_ELSEIF_B1_EN                     :  4;  // Bits 27:24
    U32                                         :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_STRUCT;

typedef union {
  struct {
    U32 Timer0_load                             :  5;  // Bits 4:0
    U32 Timer1_load                             :  5;  // Bits 9:5
    U32 Timer2_load                             :  5;  // Bits 14:10
    U32 Timer3_load                             :  16;  // Bits 30:15
    U32                                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TIMER_PRELOAD_STRUCT;

typedef union {
  struct {
    U32 MOT_PKT                                 :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_MOT_HDR_HIT_STRUCT;

typedef union {
  struct {
    U32 B0_RST                                  :  4;  // Bits 3:0
    U32 B1_RST                                  :  4;  // Bits 7:4
    U32 B2_RST                                  :  4;  // Bits 11:8
    U32 B3_RST                                  :  4;  // Bits 15:12
    U32 B4_RST                                  :  4;  // Bits 19:16
    U32 B5_RST                                  :  4;  // Bits 23:20
    U32 ELSEIF_ELSEIF_B0_RST                    :  4;  // Bits 27:24
    U32                                         :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_STRUCT;

typedef union {
  struct {
    U32 B0_RST                                  :  4;  // Bits 3:0
    U32 B1_RST                                  :  4;  // Bits 7:4
    U32 B2_RST                                  :  4;  // Bits 11:8
    U32 B3_RST                                  :  4;  // Bits 15:12
    U32 B4_RST                                  :  4;  // Bits 19:16
    U32 B5_RST                                  :  4;  // Bits 23:20
    U32 ELSEIF_ELSEIF_B1_RST                    :  4;  // Bits 27:24
    U32                                         :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_STRUCT;

typedef union {
  struct {
    U32 SPARE                                   :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} GDXC_CR_DEBUP_STRUCT;

typedef union {
  struct {
    U32 Ln0Src                                  :  2;  // Bits 1:0
    U32 Ln1Src                                  :  2;  // Bits 3:2
    U32 DEVisaEn                                :  1;  // Bits 4:4
    U32                                         :  27;  // Bits 31:5
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MSGUTILS_CR_VISA2OCLA_CFG_STRUCT;

#define DDRPL_CR_DDR_TX_DELAY_REG                                    (0x00000C04)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R0_OFF                        ( 0)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R0_WID                        ( 8)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R0_MSK                        (0x000000FF)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R0_MAX                        (0x000000FF)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R0_DEF                        (0x00000010)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R1_OFF                        ( 8)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R1_WID                        ( 8)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R1_MSK                        (0x0000FF00)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R1_MAX                        (0x000000FF)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R1_DEF                        (0x00000010)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R2_OFF                        (16)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R2_WID                        ( 8)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R2_MSK                        (0x00FF0000)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R2_MAX                        (0x000000FF)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R2_DEF                        (0x00000010)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R3_OFF                        (24)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R3_WID                        ( 8)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R3_MSK                        (0xFF000000)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R3_MAX                        (0x000000FF)
  #define DDRPL_CR_DDR_TX_DELAY_Tx_Delay_R3_DEF                        (0x00000010)

#define MPCOHTRK_CR_GDXC_SW_ENABLE_REG                               (0x00000000)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_SW_GDXC_OFF                    ( 0)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_SW_GDXC_WID                    ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_SW_GDXC_MSK                    (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_SW_GDXC_MAX                    (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_SW_GDXC_DEF                    (0x00000000)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_ADQ_OFF                   ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_ADQ_WID                   ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_ADQ_MSK                   (0x00000002)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_ADQ_MAX                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_ADQ_DEF                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_BLQ_OFF                   ( 2)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_BLQ_WID                   ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_BLQ_MSK                   (0x00000004)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_BLQ_MAX                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_BLQ_DEF                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_AKQ_OFF                   ( 3)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_AKQ_WID                   ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_AKQ_MSK                   (0x00000008)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_AKQ_MAX                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_AKQ_DEF                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_IVQ_OFF                   ( 4)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_IVQ_WID                   ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_IVQ_MSK                   (0x00000010)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_IVQ_MAX                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_RING_IVQ_DEF                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_IDIQ_OFF                       ( 5)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_IDIQ_WID                       ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_IDIQ_MSK                       (0x00000020)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_IDIQ_MAX                       (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_IDIQ_DEF                       (0x00000000)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_mc_UCLKQ_OFF                   ( 6)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_mc_UCLKQ_WID                   ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_mc_UCLKQ_MSK                   (0x00000040)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_mc_UCLKQ_MAX                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_EN_mc_UCLKQ_DEF                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_ADQ_OFF                     ( 8)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_ADQ_WID                     ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_ADQ_MSK                     (0x00000100)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_ADQ_MAX                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_ADQ_DEF                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_ADQ_OFF                     ( 9)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_ADQ_WID                     ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_ADQ_MSK                     (0x00000200)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_ADQ_MAX                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_ADQ_DEF                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_BLQ_OFF                     (10)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_BLQ_WID                     ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_BLQ_MSK                     (0x00000400)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_BLQ_MAX                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_BLQ_DEF                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_BLQ_OFF                     (11)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_BLQ_WID                     ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_BLQ_MSK                     (0x00000800)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_BLQ_MAX                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_BLQ_DEF                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_AKQ_OFF                     (12)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_AKQ_WID                     ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_AKQ_MSK                     (0x00001000)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_AKQ_MAX                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_UP_EN_AKQ_DEF                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_AKQ_OFF                     (13)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_AKQ_WID                     ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_AKQ_MSK                     (0x00002000)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_AKQ_MAX                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_DN_EN_AKQ_DEF                     (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_MOTQ_TIMING_SELECT_OFF            (14)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_MOTQ_TIMING_SELECT_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_MOTQ_TIMING_SELECT_MSK            (0x00004000)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_MOTQ_TIMING_SELECT_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_MOTQ_TIMING_SELECT_DEF            (0x00000000)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_SPARE_OFF                         (16)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_SPARE_WID                         ( 8)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_SPARE_MSK                         (0x00FF0000)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_SPARE_MAX                         (0x000000FF)
  #define MPCOHTRK_CR_GDXC_SW_ENABLE_SPARE_DEF                         (0x00000000)

#define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_REG                        (0x00000004)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T0_OFF            ( 0)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T0_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T0_MSK            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T0_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T0_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T1_OFF            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T1_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T1_MSK            (0x00000002)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T1_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core0_T1_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T0_OFF            ( 2)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T0_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T0_MSK            (0x00000004)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T0_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T0_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T1_OFF            ( 3)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T1_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T1_MSK            (0x00000008)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T1_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core1_T1_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T0_OFF            ( 4)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T0_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T0_MSK            (0x00000010)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T0_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T0_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T1_OFF            ( 5)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T1_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T1_MSK            (0x00000020)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T1_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core2_T1_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T0_OFF            ( 6)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T0_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T0_MSK            (0x00000040)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T0_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T0_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T1_OFF            ( 7)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T1_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T1_MSK            (0x00000080)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T1_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_Core3_T1_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_GT_OFF                  ( 8)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_GT_WID                  ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_GT_MSK                  (0x00000100)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_GT_MAX                  (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_GT_DEF                  (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_RD_OFF             ( 9)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_RD_WID             ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_RD_MSK             (0x00000200)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_RD_MAX             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_RD_DEF             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_WR_OFF             (10)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_WR_WID             ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_WR_MSK             (0x00000400)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_WR_MAX             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_CHR_WR_DEF             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_NC_OFF                 (11)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_NC_WID                 ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_NC_MSK                 (0x00000800)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_NC_MAX                 (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_MEM_NC_DEF                 (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_CBO_Exp_WB_OFF          (12)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_CBO_Exp_WB_WID          ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_CBO_Exp_WB_MSK          (0x00001000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_CBO_Exp_WB_MAX          (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_EN_CBO_Exp_WB_DEF          (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_SNP_Access_OFF             (13)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_SNP_Access_WID             ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_SNP_Access_MSK             (0x00002000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_SNP_Access_MAX             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_SNP_Access_DEF             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_AD_EODLAT_OFF              (14)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_AD_EODLAT_WID              ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_AD_EODLAT_MSK              (0x00004000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_AD_EODLAT_MAX              (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_GLOBAL_AD_EODLAT_DEF              (0x00000001)

#define MPCOHTRK_CR_GDXC_QUALIFIER_BL_REG                            (0x00000008)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_RD_OFF                  ( 0)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_RD_WID                  ( 3)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_RD_MSK                  (0x00000007)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_RD_MAX                  (0x00000007)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_RD_DEF                  (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CHR_RD_STAT_MOD_OFF            ( 3)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CHR_RD_STAT_MOD_WID            ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CHR_RD_STAT_MOD_MSK            (0x00000008)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CHR_RD_STAT_MOD_MAX            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CHR_RD_STAT_MOD_DEF            (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_WR_OFF                  ( 4)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_WR_WID                  ( 4)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_WR_MSK                  (0x000000F0)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_WR_MAX                  (0x0000000F)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_CHR_WR_DEF                  (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_RD_OFF                   ( 8)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_RD_WID                   ( 3)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_RD_MSK                   (0x00000700)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_RD_MAX                   (0x00000007)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_RD_DEF                   (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_RD_STAT_MOD_OFF             (11)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_RD_STAT_MOD_WID             ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_RD_STAT_MOD_MSK             (0x00000800)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_RD_STAT_MOD_MAX             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_RD_STAT_MOD_DEF             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_WR_OFF                   (12)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_WR_WID                   ( 4)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_WR_MSK                   (0x0000F000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_WR_MAX                   (0x0000000F)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_CR_NC_WR_DEF                   (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_WR_STAT_MOD_OFF             (16)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_WR_STAT_MOD_WID             ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_WR_STAT_MOD_MSK             (0x00010000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_WR_STAT_MOD_MAX             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_NC_WR_STAT_MOD_DEF             (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core0_OFF                 (17)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core0_WID                 ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core0_MSK                 (0x00020000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core0_MAX                 (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core0_DEF                 (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core1_OFF                 (18)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core1_WID                 ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core1_MSK                 (0x00040000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core1_MAX                 (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core1_DEF                 (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core2_OFF                 (19)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core2_WID                 ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core2_MSK                 (0x00080000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core2_MAX                 (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core2_DEF                 (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core3_OFF                 (20)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core3_WID                 ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core3_MSK                 (0x00100000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core3_MAX                 (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_Core3_DEF                 (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_CBO_OFF                   (21)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_CBO_WID                   ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_CBO_MSK                   (0x00200000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_CBO_MAX                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_CBO_DEF                   (0x00000000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_GT_OFF                    (22)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_GT_WID                    ( 1)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_GT_MSK                    (0x00400000)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_GT_MAX                    (0x00000001)
  #define MPCOHTRK_CR_GDXC_QUALIFIER_BL_Data_GT_DEF                    (0x00000000)

#define MPCOHTRK_CR_GDXC_OVF_STATUS_REG                              (0x0000000C)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Global_OFF                   ( 0)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Global_WID                   ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Global_MSK                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Global_MAX                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Global_DEF                   (0x00000000)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AD_OFF                  ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AD_WID                  ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AD_MSK                  (0x00000002)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AD_MAX                  (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AD_DEF                  (0x00000000)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_BL_OFF                  ( 2)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_BL_WID                  ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_BL_MSK                  (0x00000004)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_BL_MAX                  (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_BL_DEF                  (0x00000000)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AK_OFF                  ( 3)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AK_WID                  ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AK_MSK                  (0x00000008)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AK_MAX                  (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_AK_DEF                  (0x00000000)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_IV_OFF                  ( 4)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_IV_WID                  ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_IV_MSK                  (0x00000010)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_IV_MAX                  (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_Ring_IV_DEF                  (0x00000000)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_IDI_center_OFF               ( 9)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_IDI_center_WID               ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_IDI_center_MSK               (0x00000200)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_IDI_center_MAX               (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_IDI_center_DEF               (0x00000000)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_mcUCLK_OFF                   (10)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_mcUCLK_WID                   ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_mcUCLK_MSK                   (0x00000400)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_mcUCLK_MAX                   (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_mcUCLK_DEF                   (0x00000000)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_PWR_mcFCLK_OFF               (11)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_PWR_mcFCLK_WID               ( 1)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_PWR_mcFCLK_MSK               (0x00000800)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_PWR_mcFCLK_MAX               (0x00000001)
  #define MPCOHTRK_CR_GDXC_OVF_STATUS_OVF_PWR_mcFCLK_DEF               (0x00000000)

#define MPCOHTRK_CR_GDXC_MOT_ADDRESS_LO_REG                          (0x00000010)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_LO_MEM_PTR_OFF                  ( 0)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_LO_MEM_PTR_WID                  (32)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_LO_MEM_PTR_MSK                  (0xFFFFFFFF)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_LO_MEM_PTR_MAX                  (0xFFFFFFFF)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_LO_MEM_PTR_DEF                  (0x00000000)

#define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_REG                          (0x00000014)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_MEM_PTR_OFF                  ( 0)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_MEM_PTR_WID                  ( 1)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_MEM_PTR_MSK                  (0x00000001)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_MEM_PTR_MAX                  (0x00000001)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_MEM_PTR_DEF                  (0x00000000)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_BUFFER_WRAP_OFF              ( 1)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_BUFFER_WRAP_WID              ( 1)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_BUFFER_WRAP_MSK              (0x00000002)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_BUFFER_WRAP_MAX              (0x00000001)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_BUFFER_WRAP_DEF              (0x00000000)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_SPARE_OFF                    ( 2)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_SPARE_WID                    (30)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_SPARE_MSK                    (0xFFFFFFFC)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_SPARE_MAX                    (0x3FFFFFFF)
  #define MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_SPARE_DEF                    (0x00000000)

#define MPCOHTRK_CR_GDXC_MOT_REGION_REG                              (0x00000018)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_START_ADDRESS_OFF                ( 0)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_START_ADDRESS_WID                (16)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_START_ADDRESS_MSK                (0x0000FFFF)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_START_ADDRESS_MAX                (0x0000FFFF)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_START_ADDRESS_DEF                (0x00000000)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_END_ADDRESS_OFF                  (16)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_END_ADDRESS_WID                  (16)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_END_ADDRESS_MSK                  (0xFFFF0000)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_END_ADDRESS_MAX                  (0x0000FFFF)
  #define MPCOHTRK_CR_GDXC_MOT_REGION_END_ADDRESS_DEF                  (0x00000001)

#define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_LO_REG                         (0x00000020)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_LO_MEM_PTR_OFF                 ( 0)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_LO_MEM_PTR_WID                 (32)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_LO_MEM_PTR_MSK                 (0xFFFFFFFF)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_LO_MEM_PTR_MAX                 (0xFFFFFFFF)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_LO_MEM_PTR_DEF                 (0x00000000)

#define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_REG                         (0x00000024)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_MEM_PTR_OFF                 ( 0)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_MEM_PTR_WID                 ( 1)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_MEM_PTR_MSK                 (0x00000001)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_MEM_PTR_MAX                 (0x00000001)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_MEM_PTR_DEF                 (0x00000000)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_BUFFER_WRAP_OFF             ( 1)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_BUFFER_WRAP_WID             ( 1)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_BUFFER_WRAP_MSK             (0x00000002)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_BUFFER_WRAP_MAX             (0x00000001)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_BUFFER_WRAP_DEF             (0x00000000)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_LOCK_OFF                    ( 2)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_LOCK_WID                    ( 1)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_LOCK_MSK                    (0x00000004)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_LOCK_MAX                    (0x00000001)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_LOCK_DEF                    (0x00000000)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_SPARE_OFF                   ( 3)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_SPARE_WID                   (29)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_SPARE_MSK                   (0xFFFFFFF8)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_SPARE_MAX                   (0x1FFFFFFF)
  #define MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_SPARE_DEF                   (0x00000000)

#define MPCOHTRK_CR_GDXC_OCLA_REGION_REG                             (0x00000028)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_START_ADDRESS_OFF               ( 0)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_START_ADDRESS_WID               (16)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_START_ADDRESS_MSK               (0x0000FFFF)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_START_ADDRESS_MAX               (0x0000FFFF)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_START_ADDRESS_DEF               (0x00000000)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_END_ADDRESS_OFF                 (16)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_END_ADDRESS_WID                 (16)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_END_ADDRESS_MSK                 (0xFFFF0000)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_END_ADDRESS_MAX                 (0x0000FFFF)
  #define MPCOHTRK_CR_GDXC_OCLA_REGION_END_ADDRESS_DEF                 (0x00000001)

#define GODLAT_CR_G_ODLAT_ARM_S0L_REG                                (0x00000400)
  #define GODLAT_CR_G_ODLAT_ARM_S0L_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S0L_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S0L_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S0L_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S0H_REG                                (0x00000404)
  #define GODLAT_CR_G_ODLAT_ARM_S0H_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S0H_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S0H_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S0H_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S0L_REG                               (0x00000408)
  #define GODLAT_CR_G_ODLAT_MASK_S0L_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S0L_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S0L_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S0L_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S0H_REG                               (0x0000040C)
  #define GODLAT_CR_G_ODLAT_MASK_S0H_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S0H_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S0H_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S0H_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S1L_REG                                (0x00000410)
  #define GODLAT_CR_G_ODLAT_ARM_S1L_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S1L_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S1L_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S1L_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S1H_REG                                (0x00000414)
  #define GODLAT_CR_G_ODLAT_ARM_S1H_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S1H_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S1H_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S1H_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S1L_REG                               (0x00000418)
  #define GODLAT_CR_G_ODLAT_MASK_S1L_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S1L_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S1L_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S1L_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S1H_REG                               (0x0000041C)
  #define GODLAT_CR_G_ODLAT_MASK_S1H_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S1H_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S1H_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S1H_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S2L_REG                                (0x00000420)
  #define GODLAT_CR_G_ODLAT_ARM_S2L_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S2L_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S2L_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S2L_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S2H_REG                                (0x00000424)
  #define GODLAT_CR_G_ODLAT_ARM_S2H_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S2H_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S2H_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S2H_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S2L_REG                               (0x00000428)
  #define GODLAT_CR_G_ODLAT_MASK_S2L_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S2L_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S2L_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S2L_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S2H_REG                               (0x0000042C)
  #define GODLAT_CR_G_ODLAT_MASK_S2H_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S2H_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S2H_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S2H_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S3L_REG                                (0x00000430)
  #define GODLAT_CR_G_ODLAT_ARM_S3L_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S3L_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S3L_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S3L_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S3H_REG                                (0x00000434)
  #define GODLAT_CR_G_ODLAT_ARM_S3H_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S3H_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S3H_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S3H_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S3L_REG                               (0x00000438)
  #define GODLAT_CR_G_ODLAT_MASK_S3L_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S3L_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S3L_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S3L_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S3H_REG                               (0x0000043C)
  #define GODLAT_CR_G_ODLAT_MASK_S3H_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S3H_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S3H_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S3H_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S4L_REG                                (0x00000440)
  #define GODLAT_CR_G_ODLAT_ARM_S4L_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S4L_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S4L_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S4L_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S4H_REG                                (0x00000444)
  #define GODLAT_CR_G_ODLAT_ARM_S4H_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S4H_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S4H_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S4H_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S4L_REG                               (0x00000448)
  #define GODLAT_CR_G_ODLAT_MASK_S4L_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S4L_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S4L_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S4L_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S4H_REG                               (0x0000044C)
  #define GODLAT_CR_G_ODLAT_MASK_S4H_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S4H_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S4H_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S4H_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S5L_REG                                (0x00000450)
  #define GODLAT_CR_G_ODLAT_ARM_S5L_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S5L_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S5L_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S5L_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S5H_REG                                (0x00000454)
  #define GODLAT_CR_G_ODLAT_ARM_S5H_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S5H_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S5H_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S5H_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S5L_REG                               (0x00000458)
  #define GODLAT_CR_G_ODLAT_MASK_S5L_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S5L_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S5L_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S5L_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S5H_REG                               (0x0000045C)
  #define GODLAT_CR_G_ODLAT_MASK_S5H_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S5H_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S5H_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S5H_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S6L_REG                                (0x00000460)
  #define GODLAT_CR_G_ODLAT_ARM_S6L_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S6L_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S6L_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S6L_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S6H_REG                                (0x00000464)
  #define GODLAT_CR_G_ODLAT_ARM_S6H_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S6H_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S6H_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S6H_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S6L_REG                               (0x00000468)
  #define GODLAT_CR_G_ODLAT_MASK_S6L_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S6L_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S6L_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S6L_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S6H_REG                               (0x0000046C)
  #define GODLAT_CR_G_ODLAT_MASK_S6H_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S6H_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S6H_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S6H_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S7L_REG                                (0x00000470)
  #define GODLAT_CR_G_ODLAT_ARM_S7L_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S7L_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S7L_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S7L_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_ARM_S7H_REG                                (0x00000474)
  #define GODLAT_CR_G_ODLAT_ARM_S7H_ARM_bits_packet_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_ARM_S7H_ARM_bits_packet_WID                (32)
  #define GODLAT_CR_G_ODLAT_ARM_S7H_ARM_bits_packet_MSK                (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_ARM_S7H_ARM_bits_packet_MAX                (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S7L_REG                               (0x00000478)
  #define GODLAT_CR_G_ODLAT_MASK_S7L_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S7L_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S7L_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S7L_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_MASK_S7H_REG                               (0x0000047C)
  #define GODLAT_CR_G_ODLAT_MASK_S7H_Mask_bits_OFF                     ( 0)
  #define GODLAT_CR_G_ODLAT_MASK_S7H_Mask_bits_WID                     (32)
  #define GODLAT_CR_G_ODLAT_MASK_S7H_Mask_bits_MSK                     (0xFFFFFFFF)
  #define GODLAT_CR_G_ODLAT_MASK_S7H_Mask_bits_MAX                     (0xFFFFFFFF)

#define GODLAT_CR_G_ODLAT_CMD_REG                                    (0x00000480)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_A_OFF                      ( 0)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_A_WID                      ( 1)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_A_MSK                      (0x00000001)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_A_MAX                      (0x00000001)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_A_DEF                      (0x00000000)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_B_OFF                      ( 1)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_B_WID                      ( 1)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_B_MSK                      (0x00000002)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_B_MAX                      (0x00000001)
  #define GODLAT_CR_G_ODLAT_CMD_UX_TMR_BASE_B_DEF                      (0x00000000)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_IDI_OFF                        ( 2)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_IDI_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_IDI_MSK                        (0x00000004)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_IDI_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_IDI_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_HIGH_OFF                       ( 3)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_HIGH_WID                       ( 5)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_HIGH_MSK                       (0x000000F8)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_HIGH_MAX                       (0x0000001F)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_ARM_HIGH_DEF                       (0x00000000)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_Mask_HIGH_OFF                      ( 8)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_Mask_HIGH_WID                      ( 5)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_Mask_HIGH_MSK                      (0x00001F00)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_Mask_HIGH_MAX                      (0x0000001F)
  #define GODLAT_CR_G_ODLAT_CMD_OVF_Mask_HIGH_DEF                      (0x00000000)
  #define GODLAT_CR_G_ODLAT_CMD_TimerA_units_OFF                       (18)
  #define GODLAT_CR_G_ODLAT_CMD_TimerA_units_WID                       ( 3)
  #define GODLAT_CR_G_ODLAT_CMD_TimerA_units_MSK                       (0x001C0000)
  #define GODLAT_CR_G_ODLAT_CMD_TimerA_units_MAX                       (0x00000007)
  #define GODLAT_CR_G_ODLAT_CMD_TimerA_units_DEF                       (0x00000000)
  #define GODLAT_CR_G_ODLAT_CMD_TimerB_units_OFF                       (21)
  #define GODLAT_CR_G_ODLAT_CMD_TimerB_units_WID                       ( 3)
  #define GODLAT_CR_G_ODLAT_CMD_TimerB_units_MSK                       (0x00E00000)
  #define GODLAT_CR_G_ODLAT_CMD_TimerB_units_MAX                       (0x00000007)
  #define GODLAT_CR_G_ODLAT_CMD_TimerB_units_DEF                       (0x00000000)
  #define GODLAT_CR_G_ODLAT_CMD_Which_Time_Base_OFF                    (24)
  #define GODLAT_CR_G_ODLAT_CMD_Which_Time_Base_WID                    ( 4)
  #define GODLAT_CR_G_ODLAT_CMD_Which_Time_Base_MSK                    (0x0F000000)
  #define GODLAT_CR_G_ODLAT_CMD_Which_Time_Base_MAX                    (0x0000000F)
  #define GODLAT_CR_G_ODLAT_CMD_Which_Time_Base_DEF                    (0x00000000)
  #define GODLAT_CR_G_ODLAT_CMD_Tmr_Or_Cntr_Mode_OFF                   (28)
  #define GODLAT_CR_G_ODLAT_CMD_Tmr_Or_Cntr_Mode_WID                   ( 4)
  #define GODLAT_CR_G_ODLAT_CMD_Tmr_Or_Cntr_Mode_MSK                   (0xF0000000)
  #define GODLAT_CR_G_ODLAT_CMD_Tmr_Or_Cntr_Mode_MAX                   (0x0000000F)
  #define GODLAT_CR_G_ODLAT_CMD_Tmr_Or_Cntr_Mode_DEF                   (0x00000000)

#define GODLAT_CR_G_ODLAT_SW_ENABLE_REG                              (0x00000484)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_SW_G_ODLAT_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_SW_G_ODLAT_WID                ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_SW_G_ODLAT_MSK                (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_SW_G_ODLAT_MAX                (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_SW_G_ODLAT_DEF                (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S0_OFF                        ( 8)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S0_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S0_MSK                        (0x00000100)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S0_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S0_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S1_OFF                        ( 9)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S1_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S1_MSK                        (0x00000200)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S1_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S1_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S2_OFF                        (10)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S2_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S2_MSK                        (0x00000400)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S2_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S2_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S3_OFF                        (11)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S3_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S3_MSK                        (0x00000800)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S3_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S3_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S4_OFF                        (12)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S4_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S4_MSK                        (0x00001000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S4_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S4_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S5_OFF                        (13)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S5_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S5_MSK                        (0x00002000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S5_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S5_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S6_OFF                        (14)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S6_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S6_MSK                        (0x00004000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S6_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S6_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S7_OFF                        (15)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S7_WID                        ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S7_MSK                        (0x00008000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S7_MAX                        (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_EN_S7_DEF                        (0x00000000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_MBP_Enable_OFF                   (16)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_MBP_Enable_WID                   ( 1)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_MBP_Enable_MSK                   (0x00010000)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_MBP_Enable_MAX                   (0x00000001)
  #define GODLAT_CR_G_ODLAT_SW_ENABLE_MBP_Enable_DEF                   (0x00000000)

#define GODLAT_CR_G_ODLAT_MISC_CMD_REG                               (0x0000048C)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_Events_OFF        ( 0)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_Events_WID        ( 8)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_Events_MSK        (0x000000FF)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_Events_MAX        (0x000000FF)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_Events_DEF        (0x00000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_MBP_OFF           ( 8)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_MBP_WID           ( 1)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_MBP_MSK           (0x00000100)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_MBP_MAX           (0x00000001)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_MBP_DEF           (0x00000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_TO_OFF            ( 9)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_TO_WID            ( 4)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_TO_MSK            (0x00001E00)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_TO_MAX            (0x0000000F)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Pulse_or_Sticky_TO_DEF            (0x00000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_ARM_LOW_OFF                   (13)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_ARM_LOW_WID                   ( 3)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_ARM_LOW_MSK                   (0x0000E000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_ARM_LOW_MAX                   (0x00000007)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_ARM_LOW_DEF                   (0x00000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_MASK_LOW_OFF                  (16)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_MASK_LOW_WID                  ( 3)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_MASK_LOW_MSK                  (0x00070000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_MASK_LOW_MAX                  (0x00000007)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_OVF_MASK_LOW_DEF                  (0x00000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_A_OFF               (19)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_A_WID               ( 3)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_A_MSK               (0x00380000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_A_MAX               (0x00000007)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_A_DEF               (0x00000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_B_OFF               (22)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_B_WID               ( 3)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_B_MSK               (0x01C00000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_B_MAX               (0x00000007)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_B_DEF               (0x00000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_C_OFF               (25)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_C_WID               ( 3)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_C_MSK               (0x0E000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_C_MAX               (0x00000007)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_C_DEF               (0x00000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_D_OFF               (28)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_D_WID               ( 3)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_D_MSK               (0x70000000)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_D_MAX               (0x00000007)
  #define GODLAT_CR_G_ODLAT_MISC_CMD_Which_MBP_Pin_D_DEF               (0x00000000)

#define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_REG                          (0x00000500)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln0Mask_OFF                  ( 0)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln0Mask_WID                  ( 8)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln0Mask_MSK                  (0x000000FF)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln0Mask_MAX                  (0x000000FF)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln0Mask_DEF                  (0x00000000)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln1Mask_OFF                  ( 8)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln1Mask_WID                  ( 8)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln1Mask_MSK                  (0x0000FF00)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln1Mask_MAX                  (0x000000FF)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_Ln1Mask_DEF                  (0x00000000)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_AssertMode_OFF               (16)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_AssertMode_WID               ( 1)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_AssertMode_MSK               (0x00010000)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_AssertMode_MAX               (0x00000001)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_AssertMode_DEF               (0x00000000)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_CompEn_OFF                   (17)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_CompEn_WID                   ( 1)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_CompEn_MSK                   (0x00020000)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_CompEn_MAX                   (0x00000001)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_CompEn_DEF                   (0x00000001)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_GT_VISA_En_OFF               (18)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_GT_VISA_En_WID               ( 1)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_GT_VISA_En_MSK               (0x00040000)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_GT_VISA_En_MAX               (0x00000001)
  #define GDXC_CR_GT_VISA2OCLA_CFG_FILTER_GT_VISA_En_DEF               (0x00000000)

#define GDXC_CR_GDXC_MAN_CONFIG_REG                                  (0x00000504)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_GDXC_OFF                   ( 0)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_GDXC_WID                   ( 1)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_GDXC_MSK                   (0x00000001)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_GDXC_MAX                   (0x00000001)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_GDXC_DEF                   (0x00000000)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_G_ODLAT_OFF                ( 1)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_G_ODLAT_WID                ( 1)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_G_ODLAT_MSK                (0x00000002)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_G_ODLAT_MAX                (0x00000001)
  #define GDXC_CR_GDXC_MAN_CONFIG_Man_Reset_G_ODLAT_DEF                (0x00000000)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Start_OFF                        ( 2)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Start_WID                        ( 1)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Start_MSK                        (0x00000004)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Start_MAX                        (0x00000001)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Start_DEF                        (0x00000000)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Stop_OFF                         ( 3)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Stop_WID                         ( 1)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Stop_MSK                         (0x00000008)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Stop_MAX                         (0x00000001)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Stop_DEF                         (0x00000000)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Trigger_OFF                      ( 4)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Trigger_WID                      ( 1)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Trigger_MSK                      (0x00000010)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Trigger_MAX                      (0x00000001)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Trigger_DEF                      (0x00000000)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Force_Flush_OFF                  ( 5)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Force_Flush_WID                  ( 1)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Force_Flush_MSK                  (0x00000020)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Force_Flush_MAX                  (0x00000001)
  #define GDXC_CR_GDXC_MAN_CONFIG_IOT_Force_Flush_DEF                  (0x00000000)
  #define GDXC_CR_GDXC_MAN_CONFIG_SPARE_OFF                            (16)
  #define GDXC_CR_GDXC_MAN_CONFIG_SPARE_WID                            ( 8)
  #define GDXC_CR_GDXC_MAN_CONFIG_SPARE_MSK                            (0x00FF0000)
  #define GDXC_CR_GDXC_MAN_CONFIG_SPARE_MAX                            (0x000000FF)
  #define GDXC_CR_GDXC_MAN_CONFIG_SPARE_DEF                            (0x00000000)

#define GDXC_CR_GDXC_ALIGN_REG                                       (0x00000508)
  #define GDXC_CR_GDXC_ALIGN_mbpout_OFF                                ( 0)
  #define GDXC_CR_GDXC_ALIGN_mbpout_WID                                ( 2)
  #define GDXC_CR_GDXC_ALIGN_mbpout_MSK                                (0x00000003)
  #define GDXC_CR_GDXC_ALIGN_mbpout_MAX                                (0x00000003)
  #define GDXC_CR_GDXC_ALIGN_mbpout_DEF                                (0x00000001)

#define GDXC_CR_GDXC_RR_ARB_THRESH_REG                               (0x00000510)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AD_OFF                            ( 0)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AD_WID                            ( 6)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AD_MSK                            (0x0000003F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AD_MAX                            (0x0000003F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AD_DEF                            (0x00000004)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR0_OFF                        ( 6)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR0_WID                        ( 7)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR0_MSK                        (0x00001FC0)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR0_MAX                        (0x0000007F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR0_DEF                        (0x00000004)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR1ADDR_OFF                    (13)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR1ADDR_WID                    ( 6)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR1ADDR_MSK                    (0x0007E000)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR1ADDR_MAX                    (0x0000003F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_BLHDR1ADDR_DEF                    (0x00000008)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AK_OFF                            (19)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AK_WID                            ( 6)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AK_MSK                            (0x01F80000)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AK_MAX                            (0x0000003F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_AK_DEF                            (0x00000004)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IV_OFF                            (25)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IV_WID                            ( 2)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IV_MSK                            (0x06000000)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IV_MAX                            (0x00000003)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IV_DEF                            (0x00000003)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_Wrap_OFF                          (27)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_Wrap_WID                          ( 3)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_Wrap_MSK                          (0x38000000)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_Wrap_MAX                          (0x00000007)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_Wrap_DEF                          (0x00000002)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_MOT_OFF                           (30)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_MOT_WID                           ( 5)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_MOT_MSK                           (0x7C0000000)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_MOT_MAX                           (0x0000001F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_MOT_DEF                           (0x00000002)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IDI_OFF                           (35)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IDI_WID                           ( 5)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IDI_MSK                           (0xF800000000)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IDI_MAX                           (0x0000001F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_IDI_DEF                           (0x00000002)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_FClk_OFF                          (40)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_FClk_WID                          ( 5)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_FClk_MSK                          (0x1F0000000000)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_FClk_MAX                          (0x0000001F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_FClk_DEF                          (0x00000002)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_UClkMsgCh_OFF                     (45)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_UClkMsgCh_WID                     ( 5)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_UClkMsgCh_MSK                     (0x3E00000000000)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_UClkMsgCh_MAX                     (0x0000001F)
  #define GDXC_CR_GDXC_RR_ARB_THRESH_UClkMsgCh_DEF                     (0x00000002)

#define GDXC_CR_G_ODLAT_FIRE_STATUS_REG                              (0x00000518)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Start_Status_OFF                 ( 0)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Start_Status_WID                 ( 1)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Start_Status_MSK                 (0x00000001)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Start_Status_MAX                 (0x00000001)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Start_Status_DEF                 (0x00000000)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Trigger_Status_OFF               ( 1)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Trigger_Status_WID               ( 1)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Trigger_Status_MSK               (0x00000002)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Trigger_Status_MAX               (0x00000001)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Trigger_Status_DEF               (0x00000000)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Stop_Status_OFF                  ( 2)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Stop_Status_WID                  ( 1)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Stop_Status_MSK                  (0x00000004)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Stop_Status_MAX                  (0x00000001)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Stop_Status_DEF                  (0x00000000)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Muliple_Hit_OFF                  ( 3)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Muliple_Hit_WID                  ( 1)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Muliple_Hit_MSK                  (0x00000008)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Muliple_Hit_MAX                  (0x00000001)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Muliple_Hit_DEF                  (0x00000000)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Bubbles_Status_OFF               ( 4)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Bubbles_Status_WID               ( 6)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Bubbles_Status_MSK               (0x000003F0)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Bubbles_Status_MAX               (0x0000003F)
  #define GDXC_CR_G_ODLAT_FIRE_STATUS_Bubbles_Status_DEF               (0x00000000)

#define GDXC_CR_TRIGGER_TO_STOP_DELAY_REG                            (0x00000520)
  #define GDXC_CR_TRIGGER_TO_STOP_DELAY_Delay_count_OFF                ( 0)
  #define GDXC_CR_TRIGGER_TO_STOP_DELAY_Delay_count_WID                (25)
  #define GDXC_CR_TRIGGER_TO_STOP_DELAY_Delay_count_MSK                (0x01FFFFFF)
  #define GDXC_CR_TRIGGER_TO_STOP_DELAY_Delay_count_MAX                (0x01FFFFFF)
  #define GDXC_CR_TRIGGER_TO_STOP_DELAY_Delay_count_DEF                (0x00000000)

#define GDXC_CR_IOT_STATUS_REG                                       (0x00000524)
  #define GDXC_CR_IOT_STATUS_Disabled_OFF                              ( 0)
  #define GDXC_CR_IOT_STATUS_Disabled_WID                              ( 1)
  #define GDXC_CR_IOT_STATUS_Disabled_MSK                              (0x00000001)
  #define GDXC_CR_IOT_STATUS_Disabled_MAX                              (0x00000001)
  #define GDXC_CR_IOT_STATUS_Disabled_DEF                              (0x00000001)
  #define GDXC_CR_IOT_STATUS_Idle_OFF                                  ( 1)
  #define GDXC_CR_IOT_STATUS_Idle_WID                                  ( 1)
  #define GDXC_CR_IOT_STATUS_Idle_MSK                                  (0x00000002)
  #define GDXC_CR_IOT_STATUS_Idle_MAX                                  (0x00000001)
  #define GDXC_CR_IOT_STATUS_Idle_DEF                                  (0x00000000)
  #define GDXC_CR_IOT_STATUS_Running_OFF                               ( 2)
  #define GDXC_CR_IOT_STATUS_Running_WID                               ( 1)
  #define GDXC_CR_IOT_STATUS_Running_MSK                               (0x00000004)
  #define GDXC_CR_IOT_STATUS_Running_MAX                               (0x00000001)
  #define GDXC_CR_IOT_STATUS_Running_DEF                               (0x00000000)
  #define GDXC_CR_IOT_STATUS_IOT_Triggered_OFF                         ( 3)
  #define GDXC_CR_IOT_STATUS_IOT_Triggered_WID                         ( 1)
  #define GDXC_CR_IOT_STATUS_IOT_Triggered_MSK                         (0x00000008)
  #define GDXC_CR_IOT_STATUS_IOT_Triggered_MAX                         (0x00000001)
  #define GDXC_CR_IOT_STATUS_IOT_Triggered_DEF                         (0x00000000)
  #define GDXC_CR_IOT_STATUS_Sticky_triggered_OFF                      ( 4)
  #define GDXC_CR_IOT_STATUS_Sticky_triggered_WID                      ( 1)
  #define GDXC_CR_IOT_STATUS_Sticky_triggered_MSK                      (0x00000010)
  #define GDXC_CR_IOT_STATUS_Sticky_triggered_MAX                      (0x00000001)
  #define GDXC_CR_IOT_STATUS_Sticky_triggered_DEF                      (0x00000000)
  #define GDXC_CR_IOT_STATUS_Remaining_count_OFF                       ( 6)
  #define GDXC_CR_IOT_STATUS_Remaining_count_WID                       (25)
  #define GDXC_CR_IOT_STATUS_Remaining_count_MSK                       (0x7FFFFFC0)
  #define GDXC_CR_IOT_STATUS_Remaining_count_MAX                       (0x01FFFFFF)
  #define GDXC_CR_IOT_STATUS_Remaining_count_DEF                       (0x00000000)

#define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_REG                          (0x00000528)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S0_OclaHdr_OFF               ( 0)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S0_OclaHdr_WID               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S0_OclaHdr_MSK               (0x000000FF)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S0_OclaHdr_MAX               (0x000000FF)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S1_OclaHdr_OFF               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S1_OclaHdr_WID               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S1_OclaHdr_MSK               (0x0000FF00)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S1_OclaHdr_MAX               (0x000000FF)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S2_OclaHdr_OFF               (16)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S2_OclaHdr_WID               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S2_OclaHdr_MSK               (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S2_OclaHdr_MAX               (0x000000FF)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S3_OclaHdr_OFF               (24)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S3_OclaHdr_WID               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S3_OclaHdr_MSK               (0xFF000000)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S0TO3_S3_OclaHdr_MAX               (0x000000FF)

#define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_REG                          (0x0000052C)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S4_OclaHdr_OFF               ( 0)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S4_OclaHdr_WID               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S4_OclaHdr_MSK               (0x000000FF)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S4_OclaHdr_MAX               (0x000000FF)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S5_OclaHdr_OFF               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S5_OclaHdr_WID               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S5_OclaHdr_MSK               (0x0000FF00)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S5_OclaHdr_MAX               (0x000000FF)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S6_OclaHdr_OFF               (16)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S6_OclaHdr_WID               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S6_OclaHdr_MSK               (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S6_OclaHdr_MAX               (0x000000FF)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S7_OclaHdr_OFF               (24)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S7_OclaHdr_WID               ( 8)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S7_OclaHdr_MSK               (0xFF000000)
  #define GODLAT_CR_G_ODLAT_OCLAHDR_S4TO7_S7_OclaHdr_MAX               (0x000000FF)

#define GODLAT_CR_G_ODLAT_FST_REG                                    (0x00000530)
  #define GODLAT_CR_G_ODLAT_FST_S0_FST_OFF                             ( 0)
  #define GODLAT_CR_G_ODLAT_FST_S0_FST_WID                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S0_FST_MSK                             (0x00000003)
  #define GODLAT_CR_G_ODLAT_FST_S0_FST_MAX                             (0x00000003)
  #define GODLAT_CR_G_ODLAT_FST_S1_FST_OFF                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S1_FST_WID                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S1_FST_MSK                             (0x0000000C)
  #define GODLAT_CR_G_ODLAT_FST_S1_FST_MAX                             (0x00000003)
  #define GODLAT_CR_G_ODLAT_FST_S2_FST_OFF                             ( 4)
  #define GODLAT_CR_G_ODLAT_FST_S2_FST_WID                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S2_FST_MSK                             (0x00000030)
  #define GODLAT_CR_G_ODLAT_FST_S2_FST_MAX                             (0x00000003)
  #define GODLAT_CR_G_ODLAT_FST_S3_FST_OFF                             ( 6)
  #define GODLAT_CR_G_ODLAT_FST_S3_FST_WID                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S3_FST_MSK                             (0x000000C0)
  #define GODLAT_CR_G_ODLAT_FST_S3_FST_MAX                             (0x00000003)
  #define GODLAT_CR_G_ODLAT_FST_S4_FST_OFF                             ( 8)
  #define GODLAT_CR_G_ODLAT_FST_S4_FST_WID                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S4_FST_MSK                             (0x00000300)
  #define GODLAT_CR_G_ODLAT_FST_S4_FST_MAX                             (0x00000003)
  #define GODLAT_CR_G_ODLAT_FST_S5_FST_OFF                             (10)
  #define GODLAT_CR_G_ODLAT_FST_S5_FST_WID                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S5_FST_MSK                             (0x00000C00)
  #define GODLAT_CR_G_ODLAT_FST_S5_FST_MAX                             (0x00000003)
  #define GODLAT_CR_G_ODLAT_FST_S6_FST_OFF                             (12)
  #define GODLAT_CR_G_ODLAT_FST_S6_FST_WID                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S6_FST_MSK                             (0x00003000)
  #define GODLAT_CR_G_ODLAT_FST_S6_FST_MAX                             (0x00000003)
  #define GODLAT_CR_G_ODLAT_FST_S7_FST_OFF                             (14)
  #define GODLAT_CR_G_ODLAT_FST_S7_FST_WID                             ( 2)
  #define GODLAT_CR_G_ODLAT_FST_S7_FST_MSK                             (0x0000C000)
  #define GODLAT_CR_G_ODLAT_FST_S7_FST_MAX                             (0x00000003)

#define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_REG                      (0x00000534)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_Arm_OFF                  ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_Arm_WID                  ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_Arm_MSK                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_Arm_MAX                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_MBP_Arm_OFF              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_MBP_Arm_WID              ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_MBP_Arm_MSK              (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_MBP_Arm_MAX              (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_TO_Arm_OFF               ( 9)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_TO_Arm_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_TO_Arm_MSK               (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_TO_Arm_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_Mask_OFF                 (16)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_Mask_WID                 ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_Mask_MSK                 (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_Mask_MAX                 (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_MBP_Mask_OFF             (24)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_MBP_Mask_WID             ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_MBP_Mask_MSK             (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_MBP_Mask_MAX             (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_TO_Mask_OFF              (25)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_TO_Mask_WID              ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_TO_Mask_MSK              (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B0_TO_Mask_MAX              (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_REG                      (0x00000538)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_Arm_OFF                  ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_Arm_WID                  ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_Arm_MSK                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_Arm_MAX                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_MBP_Arm_OFF              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_MBP_Arm_WID              ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_MBP_Arm_MSK              (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_MBP_Arm_MAX              (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_TO_Arm_OFF               ( 9)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_TO_Arm_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_TO_Arm_MSK               (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_TO_Arm_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_Mask_OFF                 (16)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_Mask_WID                 ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_Mask_MSK                 (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_Mask_MAX                 (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_MBP_Mask_OFF             (24)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_MBP_Mask_WID             ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_MBP_Mask_MSK             (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_MBP_Mask_MAX             (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_TO_Mask_OFF              (25)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_TO_Mask_WID              ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_TO_Mask_MSK              (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B1_TO_Mask_MAX              (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_REG                      (0x0000053C)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_Arm_OFF                  ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_Arm_WID                  ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_Arm_MSK                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_Arm_MAX                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_MBP_Arm_OFF              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_MBP_Arm_WID              ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_MBP_Arm_MSK              (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_MBP_Arm_MAX              (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_TO_Arm_OFF               ( 9)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_TO_Arm_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_TO_Arm_MSK               (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_TO_Arm_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_Mask_OFF                 (16)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_Mask_WID                 ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_Mask_MSK                 (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_Mask_MAX                 (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_MBP_Mask_OFF             (24)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_MBP_Mask_WID             ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_MBP_Mask_MSK             (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_MBP_Mask_MAX             (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_TO_Mask_OFF              (25)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_TO_Mask_WID              ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_TO_Mask_MSK              (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B2_TO_Mask_MAX              (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_REG                      (0x00000540)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_Arm_OFF                  ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_Arm_WID                  ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_Arm_MSK                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_Arm_MAX                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_MBP_Arm_OFF              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_MBP_Arm_WID              ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_MBP_Arm_MSK              (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_MBP_Arm_MAX              (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_TO_Arm_OFF               ( 9)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_TO_Arm_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_TO_Arm_MSK               (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_TO_Arm_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_Mask_OFF                 (16)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_Mask_WID                 ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_Mask_MSK                 (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_Mask_MAX                 (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_MBP_Mask_OFF             (24)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_MBP_Mask_WID             ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_MBP_Mask_MSK             (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_MBP_Mask_MAX             (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_TO_Mask_OFF              (25)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_TO_Mask_WID              ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_TO_Mask_MSK              (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B3_TO_Mask_MAX              (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_REG                      (0x00000544)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_Arm_OFF                  ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_Arm_WID                  ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_Arm_MSK                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_Arm_MAX                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_MBP_Arm_OFF              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_MBP_Arm_WID              ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_MBP_Arm_MSK              (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_MBP_Arm_MAX              (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_TO_Arm_OFF               ( 9)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_TO_Arm_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_TO_Arm_MSK               (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_TO_Arm_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_Mask_OFF                 (16)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_Mask_WID                 ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_Mask_MSK                 (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_Mask_MAX                 (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_MBP_Mask_OFF             (24)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_MBP_Mask_WID             ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_MBP_Mask_MSK             (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_MBP_Mask_MAX             (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_TO_Mask_OFF              (25)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_TO_Mask_WID              ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_TO_Mask_MSK              (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B4_TO_Mask_MAX              (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_REG                      (0x00000548)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_Arm_OFF                  ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_Arm_WID                  ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_Arm_MSK                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_Arm_MAX                  (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_MBP_Arm_OFF              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_MBP_Arm_WID              ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_MBP_Arm_MSK              (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_MBP_Arm_MAX              (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_TO_Arm_OFF               ( 9)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_TO_Arm_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_TO_Arm_MSK               (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_TO_Arm_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_Mask_OFF                 (16)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_Mask_WID                 ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_Mask_MSK                 (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_Mask_MAX                 (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_MBP_Mask_OFF             (24)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_MBP_Mask_WID             ( 1)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_MBP_Mask_MSK             (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_MBP_Mask_MAX             (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_TO_Mask_OFF              (25)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_TO_Mask_WID              ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_TO_Mask_MSK              (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_ARM_MASK_B5_TO_Mask_MAX              (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_REG           (0x00000550)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Arm_OFF       ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Arm_WID       ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Arm_MSK       (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Arm_MAX       (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_MBP_Arm_OFF   ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_MBP_Arm_WID   ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_MBP_Arm_MSK   (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_MBP_Arm_MAX   (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_TO_Arm_OFF    ( 9)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_TO_Arm_WID    ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_TO_Arm_MSK    (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_TO_Arm_MAX    (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Stop_OFF      (13)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Stop_WID      ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Stop_MSK      (0x00002000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Stop_MAX      (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Mask_OFF      (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Mask_WID      ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Mask_MSK      (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Mask_MAX      (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_MBP_Mask_OFF  (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_MBP_Mask_WID  ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_MBP_Mask_MSK  (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_MBP_Mask_MAX  (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_TO_Mask_OFF   (25)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_TO_Mask_WID   ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_TO_Mask_MSK   (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_TO_Mask_MAX   (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Col_Rst_OFF   (29)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Col_Rst_WID   ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Col_Rst_MSK   (0x20000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Col_Rst_MAX   (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Trigger_OFF   (30)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Trigger_WID   ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Trigger_MSK   (0x40000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Trigger_MAX   (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Start_OFF     (31)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Start_WID     ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Start_MSK     (0x80000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B0_Start_MAX     (0x00000001)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_REG                  (0x00000554)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_Arm_OFF              ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_Arm_WID              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_Arm_MSK              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_Arm_MAX              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_MBP_Arm_OFF          ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_MBP_Arm_WID          ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_MBP_Arm_MSK          (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_MBP_Arm_MAX          (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_TO_Arm_OFF           ( 9)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_TO_Arm_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_TO_Arm_MSK           (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_TO_Arm_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_Mask_OFF             (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_Mask_WID             ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_Mask_MSK             (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_Mask_MAX             (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_MBP_Mask_OFF         (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_MBP_Mask_WID         ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_MBP_Mask_MSK         (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_MBP_Mask_MAX         (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_TO_Mask_OFF          (25)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_TO_Mask_WID          ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_TO_Mask_MSK          (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B0_TO_Mask_MAX          (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_REG                  (0x00000558)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_Arm_OFF              ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_Arm_WID              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_Arm_MSK              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_Arm_MAX              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_MBP_Arm_OFF          ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_MBP_Arm_WID          ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_MBP_Arm_MSK          (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_MBP_Arm_MAX          (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_TO_Arm_OFF           ( 9)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_TO_Arm_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_TO_Arm_MSK           (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_TO_Arm_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_Mask_OFF             (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_Mask_WID             ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_Mask_MSK             (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_Mask_MAX             (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_MBP_Mask_OFF         (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_MBP_Mask_WID         ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_MBP_Mask_MSK         (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_MBP_Mask_MAX         (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_TO_Mask_OFF          (25)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_TO_Mask_WID          ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_TO_Mask_MSK          (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B1_TO_Mask_MAX          (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_REG                  (0x0000055C)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_Arm_OFF              ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_Arm_WID              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_Arm_MSK              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_Arm_MAX              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_MBP_Arm_OFF          ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_MBP_Arm_WID          ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_MBP_Arm_MSK          (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_MBP_Arm_MAX          (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_TO_Arm_OFF           ( 9)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_TO_Arm_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_TO_Arm_MSK           (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_TO_Arm_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_Mask_OFF             (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_Mask_WID             ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_Mask_MSK             (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_Mask_MAX             (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_MBP_Mask_OFF         (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_MBP_Mask_WID         ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_MBP_Mask_MSK         (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_MBP_Mask_MAX         (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_TO_Mask_OFF          (25)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_TO_Mask_WID          ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_TO_Mask_MSK          (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B2_TO_Mask_MAX          (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_REG                  (0x00000560)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_Arm_OFF              ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_Arm_WID              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_Arm_MSK              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_Arm_MAX              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_MBP_Arm_OFF          ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_MBP_Arm_WID          ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_MBP_Arm_MSK          (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_MBP_Arm_MAX          (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_TO_Arm_OFF           ( 9)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_TO_Arm_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_TO_Arm_MSK           (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_TO_Arm_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_Mask_OFF             (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_Mask_WID             ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_Mask_MSK             (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_Mask_MAX             (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_MBP_Mask_OFF         (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_MBP_Mask_WID         ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_MBP_Mask_MSK         (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_MBP_Mask_MAX         (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_TO_Mask_OFF          (25)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_TO_Mask_WID          ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_TO_Mask_MSK          (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B3_TO_Mask_MAX          (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_REG                  (0x00000564)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_Arm_OFF              ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_Arm_WID              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_Arm_MSK              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_Arm_MAX              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_MBP_Arm_OFF          ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_MBP_Arm_WID          ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_MBP_Arm_MSK          (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_MBP_Arm_MAX          (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_TO_Arm_OFF           ( 9)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_TO_Arm_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_TO_Arm_MSK           (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_TO_Arm_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_Mask_OFF             (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_Mask_WID             ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_Mask_MSK             (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_Mask_MAX             (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_MBP_Mask_OFF         (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_MBP_Mask_WID         ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_MBP_Mask_MSK         (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_MBP_Mask_MAX         (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_TO_Mask_OFF          (25)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_TO_Mask_WID          ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_TO_Mask_MSK          (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B4_TO_Mask_MAX          (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_REG                  (0x00000568)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_Arm_OFF              ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_Arm_WID              ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_Arm_MSK              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_Arm_MAX              (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_MBP_Arm_OFF          ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_MBP_Arm_WID          ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_MBP_Arm_MSK          (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_MBP_Arm_MAX          (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_TO_Arm_OFF           ( 9)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_TO_Arm_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_TO_Arm_MSK           (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_TO_Arm_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_Mask_OFF             (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_Mask_WID             ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_Mask_MSK             (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_Mask_MAX             (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_MBP_Mask_OFF         (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_MBP_Mask_WID         ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_MBP_Mask_MSK         (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_MBP_Mask_MAX         (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_TO_Mask_OFF          (25)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_TO_Mask_WID          ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_TO_Mask_MSK          (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ARM_MASK_B5_TO_Mask_MAX          (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_REG           (0x00000570)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Arm_OFF       ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Arm_WID       ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Arm_MSK       (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Arm_MAX       (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_MBP_Arm_OFF   ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_MBP_Arm_WID   ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_MBP_Arm_MSK   (0x00000100)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_MBP_Arm_MAX   (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_TO_Arm_OFF    ( 9)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_TO_Arm_WID    ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_TO_Arm_MSK    (0x00001E00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_TO_Arm_MAX    (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Stop_OFF      (13)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Stop_WID      ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Stop_MSK      (0x00002000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Stop_MAX      (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Mask_OFF      (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Mask_WID      ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Mask_MSK      (0x00FF0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Mask_MAX      (0x000000FF)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_MBP_Mask_OFF  (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_MBP_Mask_WID  ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_MBP_Mask_MSK  (0x01000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_MBP_Mask_MAX  (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_TO_Mask_OFF   (25)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_TO_Mask_WID   ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_TO_Mask_MSK   (0x1E000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_TO_Mask_MAX   (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Col_Rst_OFF   (29)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Col_Rst_WID   ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Col_Rst_MSK   (0x20000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Col_Rst_MAX   (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Trigger_OFF   (30)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Trigger_WID   ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Trigger_MSK   (0x40000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Trigger_MAX   (0x00000001)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Start_OFF     (31)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Start_WID     ( 1)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Start_MSK     (0x80000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_ELSEIF_ARM_MASK_B1_Start_MAX     (0x00000001)

#define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_REG                       (0x00000574)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B0_Set_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B0_Set_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B0_Set_MSK                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B0_Set_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B0_Rst_OFF                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B0_Rst_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B0_Rst_MSK                (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B0_Rst_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B1_Set_OFF                (12)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B1_Set_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B1_Set_MSK                (0x0003F000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B1_Set_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B1_Rst_OFF                (18)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B1_Rst_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B1_Rst_MSK                (0x00FC0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B0_B1_B1_Rst_MAX                (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_REG                       (0x00000578)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B2_Set_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B2_Set_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B2_Set_MSK                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B2_Set_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B2_Rst_OFF                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B2_Rst_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B2_Rst_MSK                (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B2_Rst_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B3_Set_OFF                (12)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B3_Set_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B3_Set_MSK                (0x0003F000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B3_Set_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B3_Rst_OFF                (18)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B3_Rst_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B3_Rst_MSK                (0x00FC0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B2_B3_B3_Rst_MAX                (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_REG                       (0x0000057C)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B4_Set_OFF                ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B4_Set_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B4_Set_MSK                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B4_Set_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B4_Rst_OFF                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B4_Rst_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B4_Rst_MSK                (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B4_Rst_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B5_Set_OFF                (12)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B5_Set_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B5_Set_MSK                (0x0003F000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B5_Set_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B5_Rst_OFF                (18)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B5_Rst_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B5_Rst_MSK                (0x00FC0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B4_B5_B5_Rst_MAX                (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_REG                       (0x00000580)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_ElseIf_ElseIf_B0_Set_OFF  ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_ElseIf_ElseIf_B0_Set_WID  ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_ElseIf_ElseIf_B0_Set_MSK  (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_ElseIf_ElseIf_B0_Set_MAX  (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_ElseIf_ElseIf_B0_Rst_OFF  ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_ElseIf_ElseIf_B0_Rst_WID  ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_ElseIf_ElseIf_B0_Rst_MSK  (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_B6_B7_ElseIf_ElseIf_B0_Rst_MAX  (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_REG                   (0x00000584)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B0_Set_OFF            ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B0_Set_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B0_Set_MSK            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B0_Set_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B0_Rst_OFF            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B0_Rst_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B0_Rst_MSK            (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B0_Rst_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B1_Set_OFF            (12)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B1_Set_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B1_Set_MSK            (0x0003F000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B1_Set_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B1_Rst_OFF            (18)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B1_Rst_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B1_Rst_MSK            (0x00FC0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B0_B1_B1_Rst_MAX            (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_REG                   (0x00000588)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B2_Set_OFF            ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B2_Set_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B2_Set_MSK            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B2_Set_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B2_Rst_OFF            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B2_Rst_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B2_Rst_MSK            (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B2_Rst_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B3_Set_OFF            (12)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B3_Set_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B3_Set_MSK            (0x0003F000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B3_Set_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B3_Rst_OFF            (18)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B3_Rst_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B3_Rst_MSK            (0x00FC0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B2_B3_B3_Rst_MAX            (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_REG                   (0x0000058C)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B4_Set_OFF            ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B4_Set_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B4_Set_MSK            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B4_Set_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B4_Rst_OFF            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B4_Rst_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B4_Rst_MSK            (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B4_Rst_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B5_Set_OFF            (12)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B5_Set_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B5_Set_MSK            (0x0003F000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B5_Set_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B5_Rst_OFF            (18)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B5_Rst_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B5_Rst_MSK            (0x00FC0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B4_B5_B5_Rst_MAX            (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_REG                   (0x00000590)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_ElseIf_ElseIf_B1_Set_OFF ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_ElseIf_ElseIf_B1_Set_WID ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_ElseIf_ElseIf_B1_Set_MSK (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_ElseIf_ElseIf_B1_Set_MAX (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_ElseIf_ElseIf_B1_Rst_OFF ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_ElseIf_ElseIf_B1_Rst_WID ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_ElseIf_ElseIf_B1_Rst_MSK (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_B6_B7_ElseIf_ElseIf_B1_Rst_MAX (0x0000003F)

#define GODLAT_CR_G_ODLAT_INIT_BUBBLES_REG                           (0x00000594)
  #define GODLAT_CR_G_ODLAT_INIT_BUBBLES_Init_Bits_OFF                 ( 0)
  #define GODLAT_CR_G_ODLAT_INIT_BUBBLES_Init_Bits_WID                 ( 6)
  #define GODLAT_CR_G_ODLAT_INIT_BUBBLES_Init_Bits_MSK                 (0x0000003F)
  #define GODLAT_CR_G_ODLAT_INIT_BUBBLES_Init_Bits_MAX                 (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_REG                        (0x00000598)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_ColRst_OFF                 ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_ColRst_WID                 ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_ColRst_MSK                 (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_ColRst_MAX                 (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Start_OFF                  ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Start_WID                  ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Start_MSK                  (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Start_MAX                  (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Trigger_OFF                (12)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Trigger_WID                ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Trigger_MSK                (0x0003F000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Trigger_MAX                (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Stop_OFF                   (18)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Stop_WID                   ( 6)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Stop_MSK                   (0x00FC0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_MISC_Stop_MAX                   (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_REG                    (0x0000059C)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_ColRst_OFF             ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_ColRst_WID             ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_ColRst_MSK             (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_ColRst_MAX             (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Start_OFF              ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Start_WID              ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Start_MSK              (0x00000FC0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Start_MAX              (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Trigger_OFF            (12)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Trigger_WID            ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Trigger_MSK            (0x0003F000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Trigger_MAX            (0x0000003F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Stop_OFF               (18)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Stop_WID               ( 6)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Stop_MSK               (0x00FC0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_MISC_Stop_MAX               (0x0000003F)

#define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_REG                       (0x00000600)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B0_EN_OFF                 ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B0_EN_WID                 ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B0_EN_MSK                 (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B0_EN_MAX                 (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B1_EN_OFF                 ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B1_EN_WID                 ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B1_EN_MSK                 (0x000000F0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B1_EN_MAX                 (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B2_EN_OFF                 ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B2_EN_WID                 ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B2_EN_MSK                 (0x00000F00)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B2_EN_MAX                 (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B3_EN_OFF                 (12)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B3_EN_WID                 ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B3_EN_MSK                 (0x0000F000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B3_EN_MAX                 (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B4_EN_OFF                 (16)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B4_EN_WID                 ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B4_EN_MSK                 (0x000F0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B4_EN_MAX                 (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B5_EN_OFF                 (20)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B5_EN_WID                 ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B5_EN_MSK                 (0x00F00000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_B5_EN_MAX                 (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_ELSEIF_ELSEIF_B0_EN_OFF   (24)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_ELSEIF_ELSEIF_B0_EN_WID   ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_ELSEIF_ELSEIF_B0_EN_MSK   (0x0F000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMREN_ELSEIF_ELSEIF_B0_EN_MAX   (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_REG                   (0x00000604)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B0_EN_OFF             ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B0_EN_WID             ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B0_EN_MSK             (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B0_EN_MAX             (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B1_EN_OFF             ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B1_EN_WID             ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B1_EN_MSK             (0x000000F0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B1_EN_MAX             (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B2_EN_OFF             ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B2_EN_WID             ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B2_EN_MSK             (0x00000F00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B2_EN_MAX             (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B3_EN_OFF             (12)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B3_EN_WID             ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B3_EN_MSK             (0x0000F000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B3_EN_MAX             (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B4_EN_OFF             (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B4_EN_WID             ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B4_EN_MSK             (0x000F0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B4_EN_MAX             (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B5_EN_OFF             (20)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B5_EN_WID             ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B5_EN_MSK             (0x00F00000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_B5_EN_MAX             (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_ELSEIF_ELSEIF_B1_EN_OFF (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_ELSEIF_ELSEIF_B1_EN_WID ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_ELSEIF_ELSEIF_B1_EN_MSK (0x0F000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMREN_ELSEIF_ELSEIF_B1_EN_MAX (0x0000000F)

#define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_REG                          (0x00000608)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer0_load_OFF              ( 0)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer0_load_WID              ( 5)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer0_load_MSK              (0x0000001F)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer0_load_MAX              (0x0000001F)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer0_load_DEF              (0x00000000)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer1_load_OFF              ( 5)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer1_load_WID              ( 5)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer1_load_MSK              (0x000003E0)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer1_load_MAX              (0x0000001F)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer1_load_DEF              (0x00000000)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer2_load_OFF              (10)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer2_load_WID              ( 5)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer2_load_MSK              (0x00007C00)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer2_load_MAX              (0x0000001F)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer2_load_DEF              (0x00000000)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer3_load_OFF              (15)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer3_load_WID              (16)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer3_load_MSK              (0x7FFF8000)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer3_load_MAX              (0x0000FFFF)
  #define GODLAT_CR_G_ODLAT_TIMER_PRELOAD_Timer3_load_DEF              (0x00000000)

#define GODLAT_CR_G_ODLAT_MOT_HDR_HIT_REG                            (0x0000060C)
  #define GODLAT_CR_G_ODLAT_MOT_HDR_HIT_MOT_PKT_OFF                    ( 0)
  #define GODLAT_CR_G_ODLAT_MOT_HDR_HIT_MOT_PKT_WID                    ( 8)
  #define GODLAT_CR_G_ODLAT_MOT_HDR_HIT_MOT_PKT_MSK                    (0x000000FF)
  #define GODLAT_CR_G_ODLAT_MOT_HDR_HIT_MOT_PKT_MAX                    (0x000000FF)

#define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_REG                      (0x00000610)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B0_RST_OFF               ( 0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B0_RST_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B0_RST_MSK               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B0_RST_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B1_RST_OFF               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B1_RST_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B1_RST_MSK               (0x000000F0)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B1_RST_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B2_RST_OFF               ( 8)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B2_RST_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B2_RST_MSK               (0x00000F00)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B2_RST_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B3_RST_OFF               (12)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B3_RST_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B3_RST_MSK               (0x0000F000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B3_RST_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B4_RST_OFF               (16)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B4_RST_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B4_RST_MSK               (0x000F0000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B4_RST_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B5_RST_OFF               (20)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B5_RST_WID               ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B5_RST_MSK               (0x00F00000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_B5_RST_MAX               (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_ELSEIF_ELSEIF_B0_RST_OFF (24)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_ELSEIF_ELSEIF_B0_RST_WID ( 4)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_ELSEIF_ELSEIF_B0_RST_MSK (0x0F000000)
  #define GODLAT_CR_G_ODLAT_TR_IF_MODE_TMRRST_ELSEIF_ELSEIF_B0_RST_MAX (0x0000000F)

#define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_REG                  (0x00000614)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B0_RST_OFF           ( 0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B0_RST_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B0_RST_MSK           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B0_RST_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B1_RST_OFF           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B1_RST_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B1_RST_MSK           (0x000000F0)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B1_RST_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B2_RST_OFF           ( 8)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B2_RST_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B2_RST_MSK           (0x00000F00)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B2_RST_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B3_RST_OFF           (12)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B3_RST_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B3_RST_MSK           (0x0000F000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B3_RST_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B4_RST_OFF           (16)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B4_RST_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B4_RST_MSK           (0x000F0000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B4_RST_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B5_RST_OFF           (20)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B5_RST_WID           ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B5_RST_MSK           (0x00F00000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_B5_RST_MAX           (0x0000000F)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_ELSEIF_ELSEIF_B1_RST_OFF (24)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_ELSEIF_ELSEIF_B1_RST_WID ( 4)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_ELSEIF_ELSEIF_B1_RST_MSK (0x0F000000)
  #define GODLAT_CR_G_ODLAT_TR_ELSEIF_MODE_TMRRST_ELSEIF_ELSEIF_B1_RST_MAX (0x0000000F)

#define GDXC_CR_DEBUP_REG                                            (0x00000700)
  #define GDXC_CR_DEBUP_SPARE_OFF                                      ( 0)
  #define GDXC_CR_DEBUP_SPARE_WID                                      ( 8)
  #define GDXC_CR_DEBUP_SPARE_MSK                                      (0x000000FF)
  #define GDXC_CR_DEBUP_SPARE_MAX                                      (0x000000FF)
  #define GDXC_CR_DEBUP_SPARE_DEF                                      (0x00000000)

#define MSGUTILS_CR_VISA2OCLA_CFG_REG                                (0x00000A04)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln0Src_OFF                         ( 0)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln0Src_WID                         ( 2)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln0Src_MSK                         (0x00000003)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln0Src_MAX                         (0x00000003)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln0Src_DEF                         (0x00000002)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln1Src_OFF                         ( 2)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln1Src_WID                         ( 2)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln1Src_MSK                         (0x0000000C)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln1Src_MAX                         (0x00000003)
  #define MSGUTILS_CR_VISA2OCLA_CFG_Ln1Src_DEF                         (0x00000002)
  #define MSGUTILS_CR_VISA2OCLA_CFG_DEVisaEn_OFF                       ( 4)
  #define MSGUTILS_CR_VISA2OCLA_CFG_DEVisaEn_WID                       ( 1)
  #define MSGUTILS_CR_VISA2OCLA_CFG_DEVisaEn_MSK                       (0x00000010)
  #define MSGUTILS_CR_VISA2OCLA_CFG_DEVisaEn_MAX                       (0x00000001)
  #define MSGUTILS_CR_VISA2OCLA_CFG_DEVisaEn_DEF                       (0x00000000)

#pragma pack(pop)
#endif  // __McGdxcbar_h__
