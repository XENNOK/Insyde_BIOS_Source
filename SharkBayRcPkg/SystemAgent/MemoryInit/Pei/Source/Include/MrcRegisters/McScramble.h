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

#ifndef _MC_SCRAMBLE_H_  
#define _MC_SCRAMBLE_H_

#pragma pack(push, 1)
#include <MrcTypes.h>

typedef union {
  struct {
    U32 ScramEn                                 :  1;  // Bits 0:0
    U32 ScramKey                                :  16;  // Bits 16:1
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRSCRAM_CR_DDRSCRAMBLECH0_STRUCT;

typedef union {
  struct {
    U32 ScramEn                                 :  1;  // Bits 0:0
    U32 ScramKey                                :  16;  // Bits 16:1
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRSCRAM_CR_DDRSCRAMBLECH1_STRUCT;

typedef union {
  struct {
    U32 WL_WakeCycles                           :  2;  // Bits 1:0
    U32 WL_SleepCycles                          :  3;  // Bits 4:2
    U32 ForceCompUpdate                         :  1;  // Bits 5:5
    U32 WeakLock_Latency                        :  4;  // Bits 9:6
    U32 DdrNoChInterleave                       :  1;  // Bits 10:10
    U32 LPDDR_Mode                              :  1;  // Bits 11:11
    U32 CKEMappingCh0                           :  4;  // Bits 15:12
    U32 CKEMappingCh1                           :  4;  // Bits 19:16
    U32 Spare                                   :  12;  // Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRSCRAM_CR_DDRMISCCONTROL0_STRUCT;


#define DDRSCRAM_CR_DDRSCRAMBLECH0_REG                               (0x00002000)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramEn_OFF                       ( 0)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramEn_WID                       ( 1)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramEn_MSK                       (0x00000001)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramEn_MAX                       (0x00000001)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramEn_DEF                       (0x00000000)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramKey_OFF                      ( 1)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramKey_WID                      (16)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramKey_MSK                      (0x0001FFFE)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramKey_MAX                      (0x0000FFFF)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_ScramKey_DEF                      (0x00000000)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_Spare_OFF                         (17)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_Spare_WID                         (15)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_Spare_MSK                         (0xFFFE0000)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_Spare_MAX                         (0x00007FFF)
  #define DDRSCRAM_CR_DDRSCRAMBLECH0_Spare_DEF                         (0x00000000)

#define DDRSCRAM_CR_DDRSCRAMBLECH1_REG                               (0x00002004)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramEn_OFF                       ( 0)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramEn_WID                       ( 1)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramEn_MSK                       (0x00000001)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramEn_MAX                       (0x00000001)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramEn_DEF                       (0x00000000)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramKey_OFF                      ( 1)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramKey_WID                      (16)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramKey_MSK                      (0x0001FFFE)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramKey_MAX                      (0x0000FFFF)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_ScramKey_DEF                      (0x00000000)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_Spare_OFF                         (17)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_Spare_WID                         (15)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_Spare_MSK                         (0xFFFE0000)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_Spare_MAX                         (0x00007FFF)
  #define DDRSCRAM_CR_DDRSCRAMBLECH1_Spare_DEF                         (0x00000000)

#define DDRSCRAM_CR_DDRMISCCONTROL0_REG                              (0x00002008)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_WakeCycles_OFF                ( 0)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_WakeCycles_WID                ( 2)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_WakeCycles_MSK                (0x00000003)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_WakeCycles_MAX                (0x00000003)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_WakeCycles_DEF                (0x00000000)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_SleepCycles_OFF               ( 2)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_SleepCycles_WID               ( 3)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_SleepCycles_MSK               (0x0000001C)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_SleepCycles_MAX               (0x00000007)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WL_SleepCycles_DEF               (0x00000000)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_ForceCompUpdate_OFF              ( 5)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_ForceCompUpdate_WID              ( 1)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_ForceCompUpdate_MSK              (0x00000020)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_ForceCompUpdate_MAX              (0x00000001)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_ForceCompUpdate_DEF              (0x00000000)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WeakLock_Latency_OFF             ( 6)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WeakLock_Latency_WID             ( 4)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WeakLock_Latency_MSK             (0x000003C0)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WeakLock_Latency_MAX             (0x0000000F)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_WeakLock_Latency_DEF             (0x0000000C)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_DdrNoChInterleave_OFF            (10)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_DdrNoChInterleave_WID            ( 1)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_DdrNoChInterleave_MSK            (0x00000400)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_DdrNoChInterleave_MAX            (0x00000001)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_DdrNoChInterleave_DEF            (0x00000000)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_LPDDR_Mode_OFF                   (11)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_LPDDR_Mode_WID                   ( 1)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_LPDDR_Mode_MSK                   (0x00000800)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_LPDDR_Mode_MAX                   (0x00000001)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_LPDDR_Mode_DEF                   (0x00000000)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh0_OFF                (12)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh0_WID                ( 4)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh0_MSK                (0x0000F000)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh0_MAX                (0x0000000F)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh0_DEF                (0x0000000A)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh1_OFF                (16)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh1_WID                ( 4)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh1_MSK                (0x000F0000)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh1_MAX                (0x0000000F)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_CKEMappingCh1_DEF                (0x0000000A)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_Spare_OFF                        (20)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_Spare_WID                        (12)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_Spare_MSK                        (0xFFF00000)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_Spare_MAX                        (0x00000FFF)
  #define DDRSCRAM_CR_DDRMISCCONTROL0_Spare_DEF                        (0x00000000)

#pragma pack(pop)
#endif  // __McScramble_h__
