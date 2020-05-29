/** @file

  Include all the DDR3 register definitions.  
  
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

#ifndef _MRC_DDR3_REGISTERS_H_
#define _MRC_DDR3_REGISTERS_H_

#pragma pack(push, 1)

typedef union {
  struct {
    U16 BurstLength                 : 2;    ///< Bits 0:1
    U16 CasLatencyLow               : 1;    ///< Bits 2:2
    U16 ReadBurstType               : 1;    ///< Bits 3:3
    U16 CasLatencyHigh              : 3;    ///< Bits 4:6
    U16 TestMode                    : 1;    ///< Bits 7:7
    U16 DllReset                    : 1;    ///< Bits 8:8
    U16 WriteRecovery               : 3;    ///< Bits 9:11
    U16 PrechargePdDll              : 1;    ///< Bits 12:12
    U16                             : 3;    ///< Bits 13:15
  } Bits;
  U16 Data;
  U8  Data8[2];
} DDR3_MODE_REGISTER_0_STRUCT;

typedef union {
  struct {
    U16 DllEnable                   : 1;    ///< Bits 0:0
    U16 ODImpedanceLow              : 1;    ///< Bits 1:1
    U16 OdtRttValueLow              : 1;    ///< Bits 2:2
    U16 AdditiveLatency             : 2;    ///< Bits 3:4
    U16 ODImpedanceHigh             : 1;    ///< Bits 5:5
    U16 OdtRttValueMid              : 1;    ///< Bits 6:6
    U16 WriteLeveling               : 1;    ///< Bits 7:7
    U16                             : 1;    ///< Bits 8:8
    U16 OdtRttValueHigh             : 1;    ///< Bits 9:9
    U16                             : 1;    ///< Bits 10:10
    U16 Tdqs                        : 1;    ///< Bits 11:11
    U16 Qoff                        : 1;    ///< Bits 12:12
    U16                             : 3;    ///< Bits 13:15
  } Bits;
  U16 Data;
  U8  Data8[2];
} DDR3_MODE_REGISTER_1_STRUCT;

typedef union {
  struct {
    U16 PartialArraySR              : 3;    ///< Bits 0:2
    U16 CasWriteLatency             : 3;    ///< Bits 3:5
    U16 AutoSelfRefresh             : 1;    ///< Bits 6:6
    U16 SelfRefreshTemp             : 1;    ///< Bits 7:7
    U16                             : 1;    ///< Bits 8:8
    U16 DynamicOdt                  : 2;    ///< Bits 9:10
    U16                             : 5;    ///< Bits 11:15
  } Bits;
  U16 Data;
  U8  Data8[2];
} DDR3_MODE_REGISTER_2_STRUCT;

typedef union {
  struct {
    U16 MprLocation                 : 2;    ///< Bits 0:1
    U16 MprOperation                : 1;    ///< Bits 2:2
    U16                             : 13;   ///< Bits 3:15
  } Bits;
  U16 Data;
  U8  Data8[2];
} DDR3_MODE_REGISTER_3_STRUCT;

///
/// MR0 register
///
#define DDR3_MODE_REGISTER_0_BL_OFF     (0)
#define DDR3_MODE_REGISTER_0_BL_WID     (2)
#define DDR3_MODE_REGISTER_0_BL_MSK     (3)
#define DDR3_MODE_REGISTER_0_BL_MAX     (3)
#define DDR3_MODE_REGISTER_0_BL_DEF     (0)

#define DDR3_MODE_REGISTER_0_CL_A2_OFF  (2)
#define DDR3_MODE_REGISTER_0_CL_A2_WID  (1)
#define DDR3_MODE_REGISTER_0_CL_A2_MSK  (1)
#define DDR3_MODE_REGISTER_0_CL_A2_MAX  (1)
#define DDR3_MODE_REGISTER_0_CL_A2_DEF  (0)

#define DDR3_MODE_REGISTER_0_RBT_OFF    (3)
#define DDR3_MODE_REGISTER_0_RBT_WID    (1)
#define DDR3_MODE_REGISTER_0_RBT_MSK    (1)
#define DDR3_MODE_REGISTER_0_RBT_MAX    (1)
#define DDR3_MODE_REGISTER_0_RBT_DEF    (0)

#define DDR3_MODE_REGISTER_0_CL_OFF     (4)
#define DDR3_MODE_REGISTER_0_CL_WID     (3)
#define DDR3_MODE_REGISTER_0_CL_MSK     (7)
#define DDR3_MODE_REGISTER_0_CL_MAX     (7)
#define DDR3_MODE_REGISTER_0_CL_DEF     (0)

#define DDR3_MODE_REGISTER_0_TM_OFF     (7)
#define DDR3_MODE_REGISTER_0_TM_WID     (1)
#define DDR3_MODE_REGISTER_0_TM_MSK     (1)
#define DDR3_MODE_REGISTER_0_TM_MAX     (1)
#define DDR3_MODE_REGISTER_0_TM_DEF     (0)

#define DDR3_MODE_REGISTER_0_DLL_OFF    (8)
#define DDR3_MODE_REGISTER_0_DLL_WID    (1)
#define DDR3_MODE_REGISTER_0_DLL_MSK    (1)
#define DDR3_MODE_REGISTER_0_DLL_MAX    (1)
#define DDR3_MODE_REGISTER_0_DLL_DEF    (0)

#define DDR3_MODE_REGISTER_0_WR_OFF     (9)
#define DDR3_MODE_REGISTER_0_WR_WID     (3)
#define DDR3_MODE_REGISTER_0_WR_MSK     (7)
#define DDR3_MODE_REGISTER_0_WR_MAX     (7)
#define DDR3_MODE_REGISTER_0_WR_DEF     (0)

#define DDR3_MODE_REGISTER_0_PPD_OFF    (12)
#define DDR3_MODE_REGISTER_0_PPD_WID    (1)
#define DDR3_MODE_REGISTER_0_PPD_MSK    (1)
#define DDR3_MODE_REGISTER_0_PPD_MAX    (1)
#define DDR3_MODE_REGISTER_0_PPD_DEF    (0)

///
/// MR1 register
///
#define DDR3_MODE_REGISTER_1_DLL_OFF        (0)
#define DDR3_MODE_REGISTER_1_DLL_WID        (1)
#define DDR3_MODE_REGISTER_1_DLL_MSK        (1)
#define DDR3_MODE_REGISTER_1_DLL_MAX        (1)
#define DDR3_MODE_REGISTER_1_DLL_DEF        (0)

#define DDR3_MODE_REGISTER_1_D_I_C_A1_OFF   (1)
#define DDR3_MODE_REGISTER_1_D_I_C_A1_WID   (1)
#define DDR3_MODE_REGISTER_1_D_I_C_A1_MSK   (1)
#define DDR3_MODE_REGISTER_1_D_I_C_A1_MAX   (1)
#define DDR3_MODE_REGISTER_1_D_I_C_A1_DEF   (0)

#define DDR3_MODE_REGISTER_1_Rtt_Nom_A2_OFF (2)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A2_WID (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A2_MSK (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A2_MAX (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A2_DEF (0)

#define DDR3_MODE_REGISTER_1_AL_OFF         (3)
#define DDR3_MODE_REGISTER_1_AL_WID         (2)
#define DDR3_MODE_REGISTER_1_AL_MSK         (3)
#define DDR3_MODE_REGISTER_1_AL_MAX         (3)
#define DDR3_MODE_REGISTER_1_AL_DEF         (0)

#define DDR3_MODE_REGISTER_1_D_I_C_A5_OFF   (5)
#define DDR3_MODE_REGISTER_1_D_I_C_A5_WID   (1)
#define DDR3_MODE_REGISTER_1_D_I_C_A5_MSK   (1)
#define DDR3_MODE_REGISTER_1_D_I_C_A5_MAX   (1)
#define DDR3_MODE_REGISTER_1_D_I_C_A5_DEF   (0)

#define DDR3_MODE_REGISTER_1_Rtt_Nom_A6_OFF (6)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A6_WID (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A6_MSK (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A6_MAX (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A6_DEF (0)

#define DDR3_MODE_REGISTER_1_LEVEL_OFF      (7)
#define DDR3_MODE_REGISTER_1_LEVEL_WID      (1)
#define DDR3_MODE_REGISTER_1_LEVEL_MSK      (1)
#define DDR3_MODE_REGISTER_1_LEVEL_MAX      (1)
#define DDR3_MODE_REGISTER_1_LEVEL_DEF      (0)

#define DDR3_MODE_REGISTER_1_Rtt_Nom_A9_OFF (9)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A9_WID (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A9_MSK (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A9_MAX (1)
#define DDR3_MODE_REGISTER_1_Rtt_Nom_A9_DEF (0)

#define DDR3_MODE_REGISTER_1_TDQS_OFF       (11)
#define DDR3_MODE_REGISTER_1_TDQS_WID       (1)
#define DDR3_MODE_REGISTER_1_TDQS_MSK       (1)
#define DDR3_MODE_REGISTER_1_TDQS_MAX       (1)
#define DDR3_MODE_REGISTER_1_TDQS_DEF       (0)

#define DDR3_MODE_REGISTER_1_Qoff_OFF       (12)
#define DDR3_MODE_REGISTER_1_Qoff_WID       (1)
#define DDR3_MODE_REGISTER_1_Qoff_MSK       (1)
#define DDR3_MODE_REGISTER_1_Qoff_MAX       (1)
#define DDR3_MODE_REGISTER_1_Qoff_DEF       (0)

///
/// MR2 register
///
#define DDR3_MODE_REGISTER_2_PASR_OFF   (0)
#define DDR3_MODE_REGISTER_2_PASR_WID   (3)
#define DDR3_MODE_REGISTER_2_PASR_MSK   (7)
#define DDR3_MODE_REGISTER_2_PASR_MAX   (7)
#define DDR3_MODE_REGISTER_2_PASR_DEF   (0)

#define DDR3_MODE_REGISTER_2_CWL_OFF    (3)
#define DDR3_MODE_REGISTER_2_CWL_WID    (3)
#define DDR3_MODE_REGISTER_2_CWL_MSK    (7)
#define DDR3_MODE_REGISTER_2_CWL_MAX    (7)
#define DDR3_MODE_REGISTER_2_CWL_DEF    (0)

#define DDR3_MODE_REGISTER_2_ASR_OFF    (6)
#define DDR3_MODE_REGISTER_2_ASR_WID    (1)
#define DDR3_MODE_REGISTER_2_ASR_MSK    (1)
#define DDR3_MODE_REGISTER_2_ASR_MAX    (1)
#define DDR3_MODE_REGISTER_2_ASR_DEF    (0)

#define DDR3_MODE_REGISTER_2_STR_OFF    (7)
#define DDR3_MODE_REGISTER_2_STR_WID    (1)
#define DDR3_MODE_REGISTER_2_STR_MSK    (1)
#define DDR3_MODE_REGISTER_2_STR_MAX    (1)
#define DDR3_MODE_REGISTER_2_STR_DEF    (0)

#define DDR3_MODE_REGISTER_2_Rtt_WR_OFF (9)
#define DDR3_MODE_REGISTER_2_Rtt_WR_WID (2)
#define DDR3_MODE_REGISTER_2_Rtt_WR_MSK (3)
#define DDR3_MODE_REGISTER_2_Rtt_WR_MAX (3)
#define DDR3_MODE_REGISTER_2_Rtt_WR_DEF (0)

///
/// MR3 register
///
#define DDR3_MODE_REGISTER_3_MPR_LOC_OFF  (0)
#define DDR3_MODE_REGISTER_3_MPR_LOC_WID  (2)
#define DDR3_MODE_REGISTER_3_MPR_LOC_MSK  (3)
#define DDR3_MODE_REGISTER_3_MPR_LOC_MAX  (3)
#define DDR3_MODE_REGISTER_3_MPR_LOC_DEF  (0)

#define DDR3_MODE_REGISTER_3_MPR_OFF      (2)
#define DDR3_MODE_REGISTER_3_MPR_WID      (1)
#define DDR3_MODE_REGISTER_3_MPR_MSK      (1)
#define DDR3_MODE_REGISTER_3_MPR_MAX      (1)
#define DDR3_MODE_REGISTER_3_MPR_DEF      (0)

#pragma pack (pop)
#endif
