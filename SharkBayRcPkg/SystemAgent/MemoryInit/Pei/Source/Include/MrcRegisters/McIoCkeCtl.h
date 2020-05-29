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

#ifndef _MC_IO_CKE_CTL_H_
#define _MC_IO_CKE_CTL_H_

#pragma pack(push, 1)
#include <MrcTypes.h>

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
} DDRCKECTL_CR_DDRCRCTLCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CtlPiCode0                              :  7;  // Bits 6:0
    U32 CtlPiCode1                              :  7;  // Bits 13:7
    U32 CtlPiCode2                              :  7;  // Bits 20:14
    U32 CtlPiCode3                              :  7;  // Bits 27:21
    U32 CtlXoverEnable                          :  1;  // Bits 28:28
    U32 Spare                                   :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTL_CR_DDRCRCTLPICODING_STRUCT;

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
    U32 CmdTxEq                                 :  2;  // Bits 14:13
    U32 EarlyWeakDrive                          :  2;  // Bits 16:15
    U32 CtlTxEq                                 :  2;  // Bits 18:17
    U32 CtlSRDrv                                :  2;  // Bits 20:19
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LaDrvEnOvrd                             :  1;  // Bits 30:30
    U32 LPDdrCAA_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTL_CR_DDRCRCTLCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 OdtDisable                              :  2;  // Bits 5:4
    U32 Spare                                   :  26;  // Bits 31:6
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTL_CR_DDRCRCTLRANKSUSED_STRUCT;

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
} DDRCKECTL_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCKECTLCH0_CR_DDRCRCTLCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CtlPiCode0                              :  7;  // Bits 6:0
    U32 CtlPiCode1                              :  7;  // Bits 13:7
    U32 CtlPiCode2                              :  7;  // Bits 20:14
    U32 CtlPiCode3                              :  7;  // Bits 27:21
    U32 CtlXoverEnable                          :  1;  // Bits 28:28
    U32 Spare                                   :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTLCH0_CR_DDRCRCTLPICODING_STRUCT;

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
    U32 CmdTxEq                                 :  2;  // Bits 14:13
    U32 EarlyWeakDrive                          :  2;  // Bits 16:15
    U32 CtlTxEq                                 :  2;  // Bits 18:17
    U32 CtlSRDrv                                :  2;  // Bits 20:19
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LaDrvEnOvrd                             :  1;  // Bits 30:30
    U32 LPDdrCAA_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 OdtDisable                              :  2;  // Bits 5:4
    U32 Spare                                   :  26;  // Bits 31:6
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_STRUCT;

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
} DDRCKECTLCH0_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCKECTLCH1_CR_DDRCRCTLCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CtlPiCode0                              :  7;  // Bits 6:0
    U32 CtlPiCode1                              :  7;  // Bits 13:7
    U32 CtlPiCode2                              :  7;  // Bits 20:14
    U32 CtlPiCode3                              :  7;  // Bits 27:21
    U32 CtlXoverEnable                          :  1;  // Bits 28:28
    U32 Spare                                   :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTLCH1_CR_DDRCRCTLPICODING_STRUCT;

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
    U32 CmdTxEq                                 :  2;  // Bits 14:13
    U32 EarlyWeakDrive                          :  2;  // Bits 16:15
    U32 CtlTxEq                                 :  2;  // Bits 18:17
    U32 CtlSRDrv                                :  2;  // Bits 20:19
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LaDrvEnOvrd                             :  1;  // Bits 30:30
    U32 LPDdrCAA_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 OdtDisable                              :  2;  // Bits 5:4
    U32 Spare                                   :  26;  // Bits 31:6
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_STRUCT;

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
} DDRCKECTLCH1_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCKECH0_CR_DDRCRCMDCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH0_CR_DDRCRCMDPICODING_STRUCT;

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
} DDRCKECH0_CR_DDRCRCTLCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CtlPiCode0                              :  7;  // Bits 6:0
    U32 CtlPiCode1                              :  7;  // Bits 13:7
    U32 CtlPiCode2                              :  7;  // Bits 20:14
    U32 CtlPiCode3                              :  7;  // Bits 27:21
    U32 CtlXoverEnable                          :  1;  // Bits 28:28
    U32 Spare                                   :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH0_CR_DDRCRCTLPICODING_STRUCT;

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
    U32 CmdTxEq                                 :  2;  // Bits 14:13
    U32 EarlyWeakDrive                          :  2;  // Bits 16:15
    U32 CtlTxEq                                 :  2;  // Bits 18:17
    U32 CtlSRDrv                                :  2;  // Bits 20:19
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LaDrvEnOvrd                             :  1;  // Bits 30:30
    U32 LPDdrCAA_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH0_CR_DDRCRCTLCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 OdtDisable                              :  2;  // Bits 5:4
    U32 Spare                                   :  26;  // Bits 31:6
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH0_CR_DDRCRCTLRANKSUSED_STRUCT;

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
} DDRCKECH0_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCKECH1_CR_DDRCRCMDCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH1_CR_DDRCRCMDPICODING_STRUCT;

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
} DDRCKECH1_CR_DDRCRCTLCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CtlPiCode0                              :  7;  // Bits 6:0
    U32 CtlPiCode1                              :  7;  // Bits 13:7
    U32 CtlPiCode2                              :  7;  // Bits 20:14
    U32 CtlPiCode3                              :  7;  // Bits 27:21
    U32 CtlXoverEnable                          :  1;  // Bits 28:28
    U32 Spare                                   :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH1_CR_DDRCRCTLPICODING_STRUCT;

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
    U32 CmdTxEq                                 :  2;  // Bits 14:13
    U32 EarlyWeakDrive                          :  2;  // Bits 16:15
    U32 CtlTxEq                                 :  2;  // Bits 18:17
    U32 CtlSRDrv                                :  2;  // Bits 20:19
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LaDrvEnOvrd                             :  1;  // Bits 30:30
    U32 LPDdrCAA_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH1_CR_DDRCRCTLCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 OdtDisable                              :  2;  // Bits 5:4
    U32 Spare                                   :  26;  // Bits 31:6
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCKECH1_CR_DDRCRCTLRANKSUSED_STRUCT;

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
} DDRCKECH1_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCTLCH0_CR_DDRCRCTLCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CtlPiCode0                              :  7;  // Bits 6:0
    U32 CtlPiCode1                              :  7;  // Bits 13:7
    U32 CtlPiCode2                              :  7;  // Bits 20:14
    U32 CtlPiCode3                              :  7;  // Bits 27:21
    U32 CtlXoverEnable                          :  1;  // Bits 28:28
    U32 Spare                                   :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCTLCH0_CR_DDRCRCTLPICODING_STRUCT;

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
    U32 CmdTxEq                                 :  2;  // Bits 14:13
    U32 EarlyWeakDrive                          :  2;  // Bits 16:15
    U32 CtlTxEq                                 :  2;  // Bits 18:17
    U32 CtlSRDrv                                :  2;  // Bits 20:19
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LaDrvEnOvrd                             :  1;  // Bits 30:30
    U32 LPDdrCAA_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCTLCH0_CR_DDRCRCTLCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 OdtDisable                              :  2;  // Bits 5:4
    U32 Spare                                   :  26;  // Bits 31:6
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCTLCH0_CR_DDRCRCTLRANKSUSED_STRUCT;

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
} DDRCTLCH0_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCTLCH1_CR_DDRCRCTLCOMP_STRUCT;

typedef union {
  struct {
    U32 ScompOffset                             :  5;  // Bits 4:0
    U32 TcoCompOffset                           :  4;  // Bits 8:5
    U32 RcompDrvUpOffset                        :  4;  // Bits 12:9
    U32 RcompDrvDownOffset                      :  4;  // Bits 16:13
    U32 Spare                                   :  15;  // Bits 31:17
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CtlPiCode0                              :  7;  // Bits 6:0
    U32 CtlPiCode1                              :  7;  // Bits 13:7
    U32 CtlPiCode2                              :  7;  // Bits 20:14
    U32 CtlPiCode3                              :  7;  // Bits 27:21
    U32 CtlXoverEnable                          :  1;  // Bits 28:28
    U32 Spare                                   :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCTLCH1_CR_DDRCRCTLPICODING_STRUCT;

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
    U32 CmdTxEq                                 :  2;  // Bits 14:13
    U32 EarlyWeakDrive                          :  2;  // Bits 16:15
    U32 CtlTxEq                                 :  2;  // Bits 18:17
    U32 CtlSRDrv                                :  2;  // Bits 20:19
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LaDrvEnOvrd                             :  1;  // Bits 30:30
    U32 LPDdrCAA_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCTLCH1_CR_DDRCRCTLCONTROLS_STRUCT;

typedef union {
  struct {
    U32 RankEn                                  :  4;  // Bits 3:0
    U32 OdtDisable                              :  2;  // Bits 5:4
    U32 Spare                                   :  26;  // Bits 31:6
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCTLCH1_CR_DDRCRCTLRANKSUSED_STRUCT;

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
} DDRCTLCH1_CR_DLLPITESTANDADC_STRUCT;


#define DDRCKECTL_CR_DDRCRCTLCOMP_REG                                (0x00003810)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Scomp_OFF                          ( 0)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Scomp_WID                          ( 6)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_TcoComp_OFF                        ( 6)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_TcoComp_WID                        ( 6)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_LsComp_OFF                         (24)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_LsComp_WID                         ( 3)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_LsComp_DEF                         (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Spare_OFF                          (27)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Spare_WID                          ( 5)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCKECTL_CR_DDRCRCTLCOMP_Spare_DEF                          (0x00000000)

#define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_REG                          (0x00003814)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_Spare_WID                    (15)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCKECTL_CR_DDRCRCTLCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCKECTL_CR_DDRCRCTLPICODING_REG                            (0x00003818)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode0_OFF                 ( 0)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode0_WID                 ( 7)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode0_MSK                 (0x0000007F)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode0_MAX                 (0x0000007F)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode0_DEF                 (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode1_OFF                 ( 7)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode1_WID                 ( 7)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode1_MSK                 (0x00003F80)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode1_MAX                 (0x0000007F)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode1_DEF                 (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode2_OFF                 (14)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode2_WID                 ( 7)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode2_MSK                 (0x001FC000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode2_MAX                 (0x0000007F)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode2_DEF                 (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode3_OFF                 (21)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode3_WID                 ( 7)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode3_MSK                 (0x0FE00000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode3_MAX                 (0x0000007F)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlPiCode3_DEF                 (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlXoverEnable_OFF             (28)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlXoverEnable_WID             ( 1)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlXoverEnable_MSK             (0x10000000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlXoverEnable_MAX             (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_CtlXoverEnable_DEF             (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_Spare_OFF                      (29)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_Spare_WID                      ( 3)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_Spare_MSK                      (0xE0000000)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_Spare_MAX                      (0x00000007)
  #define DDRCKECTL_CR_DDRCRCTLPICODING_Spare_DEF                      (0x00000000)

#define DDRCKECTL_CR_DDRCRCTLCONTROLS_REG                            (0x0000381C)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RefPi_WID                      ( 4)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllMask_WID                    ( 2)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_TxOn_WID                       ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_OdtMode_OFF                    (12)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CmdTxEq_OFF                    (13)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CmdTxEq_WID                    ( 2)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CmdTxEq_MSK                    (0x00006000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CmdTxEq_MAX                    (0x00000003)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CmdTxEq_DEF                    (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_OFF             (15)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_WID             ( 2)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MSK             (0x00018000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MAX             (0x00000003)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_DEF             (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlTxEq_OFF                    (17)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlTxEq_WID                    ( 2)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlTxEq_MSK                    (0x00060000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlTxEq_MAX                    (0x00000003)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlTxEq_DEF                    (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlSRDrv_OFF                   (19)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlSRDrv_WID                   ( 2)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlSRDrv_MSK                   (0x00180000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlSRDrv_MAX                   (0x00000003)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_CtlSRDrv_DEF                   (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RxVref_OFF                     (21)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RxVref_WID                     ( 6)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_OFF                (30)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_WID                ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MSK                (0x40000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MAX                (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_DEF                (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_OFF               (31)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_WID               ( 1)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MSK               (0x80000000)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MAX               (0x00000001)
  #define DDRCKECTL_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_DEF               (0x00000000)

#define DDRCKECTL_CR_DDRCRCTLRANKSUSED_REG                           (0x00003820)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_RankEn_OFF                    ( 0)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_RankEn_WID                    ( 4)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_RankEn_MSK                    (0x0000000F)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_RankEn_MAX                    (0x0000000F)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_RankEn_DEF                    (0x0000000F)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_OdtDisable_OFF                ( 4)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_OdtDisable_WID                ( 2)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_OdtDisable_MSK                (0x00000030)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_OdtDisable_MAX                (0x00000003)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_OdtDisable_DEF                (0x00000000)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_Spare_OFF                     ( 6)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_Spare_WID                     (26)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_Spare_MSK                     (0xFFFFFFC0)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_Spare_MAX                     (0x03FFFFFF)
  #define DDRCKECTL_CR_DDRCRCTLRANKSUSED_Spare_DEF                     (0x00000000)

#define DDRCKECTL_CR_DLLPITESTANDADC_REG                             (0x00003824)
  #define DDRCKECTL_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCKECTL_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCKECTL_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCKECTL_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCKECTL_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCKECTL_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCKECTL_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCKECTL_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCKECTL_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCKECTL_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCKECTL_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCKECTL_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCKECTL_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCKECTL_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCKECTL_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCKECTL_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#define DDRCKECTLCH0_CR_DDRCRCTLCOMP_REG                             (0x00003410)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Scomp_OFF                       ( 0)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Scomp_WID                       ( 6)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Scomp_MSK                       (0x0000003F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Scomp_MAX                       (0x0000003F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Scomp_DEF                       (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_TcoComp_OFF                     ( 6)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_TcoComp_WID                     ( 6)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_TcoComp_MSK                     (0x00000FC0)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_TcoComp_MAX                     (0x0000003F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_TcoComp_DEF                     (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_OFF                  (12)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_WID                  ( 6)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_MSK                  (0x0003F000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_MAX                  (0x0000003F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_DEF                  (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_OFF                (18)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_WID                ( 6)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_MSK                (0x00FC0000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_MAX                (0x0000003F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_DEF                (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_LsComp_OFF                      (24)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_LsComp_WID                      ( 3)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_LsComp_MSK                      (0x07000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_LsComp_MAX                      (0x00000007)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_LsComp_DEF                      (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Spare_OFF                       (27)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Spare_WID                       ( 5)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Spare_MSK                       (0xF8000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Spare_MAX                       (0x0000001F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMP_Spare_DEF                       (0x00000000)

#define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_REG                       (0x00003414)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_OFF           ( 0)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_WID           ( 5)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MSK           (0x0000001F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MAX           (0x0000001F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_DEF           (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_OFF         ( 5)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_WID         ( 4)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MSK         (0x000001E0)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MAX         (0x0000000F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_DEF         (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_OFF      ( 9)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_WID      ( 4)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MSK      (0x00001E00)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MAX      (0x0000000F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_DEF      (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_OFF    (13)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_WID    ( 4)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MSK    (0x0001E000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MAX    (0x0000000F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_DEF    (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_OFF                 (17)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_WID                 (15)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_MSK                 (0xFFFE0000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_MAX                 (0x00007FFF)
  #define DDRCKECTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_DEF                 (0x00000000)

#define DDRCKECTLCH0_CR_DDRCRCTLPICODING_REG                         (0x00003418)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_OFF              ( 0)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_WID              ( 7)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_MSK              (0x0000007F)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_MAX              (0x0000007F)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_DEF              (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_OFF              ( 7)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_WID              ( 7)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_MSK              (0x00003F80)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_MAX              (0x0000007F)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_DEF              (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_OFF              (14)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_WID              ( 7)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_MSK              (0x001FC000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_MAX              (0x0000007F)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_DEF              (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_OFF              (21)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_WID              ( 7)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_MSK              (0x0FE00000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_MAX              (0x0000007F)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_DEF              (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_OFF          (28)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_WID          ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_MSK          (0x10000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_MAX          (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_DEF          (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_Spare_OFF                   (29)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_Spare_WID                   ( 3)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_Spare_MSK                   (0xE0000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_Spare_MAX                   (0x00000007)
  #define DDRCKECTLCH0_CR_DDRCRCTLPICODING_Spare_DEF                   (0x00000000)

#define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_REG                         (0x0000341C)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RefPi_OFF                   ( 0)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RefPi_WID                   ( 4)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RefPi_MSK                   (0x0000000F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RefPi_MAX                   (0x0000000F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RefPi_DEF                   (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllMask_OFF                 ( 4)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllMask_WID                 ( 2)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllMask_MSK                 (0x00000030)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllMask_MAX                 (0x00000003)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllMask_DEF                 (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_OFF                ( 6)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_WID                ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_MSK                (0x00000040)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_MAX                (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_DEF                (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_TxOn_OFF                    ( 7)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_TxOn_WID                    ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_TxOn_MSK                    (0x00000080)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_TxOn_MAX                    (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_TxOn_DEF                    (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_OFF                ( 8)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_WID                ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_MSK                (0x00000100)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_MAX                (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_DEF                (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_OFF                ( 9)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_WID                ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_MSK                (0x00000200)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_MAX                (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_DEF                (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_OFF                 (10)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_WID                 ( 2)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_MSK                 (0x00000C00)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_MAX                 (0x00000003)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_DEF                 (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_OFF                 (12)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_WID                 ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_MSK                 (0x00001000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_MAX                 (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_DEF                 (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_OFF                 (13)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_WID                 ( 2)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_MSK                 (0x00006000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_MAX                 (0x00000003)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_DEF                 (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_OFF          (15)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_WID          ( 2)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MSK          (0x00018000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MAX          (0x00000003)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_DEF          (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_OFF                 (17)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_WID                 ( 2)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_MSK                 (0x00060000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_MAX                 (0x00000003)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_DEF                 (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_OFF                (19)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_WID                ( 2)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_MSK                (0x00180000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_MAX                (0x00000003)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_DEF                (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RxVref_OFF                  (21)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RxVref_WID                  ( 6)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RxVref_MSK                  (0x07E00000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RxVref_MAX                  (0x0000003F)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_RxVref_DEF                  (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_OFF                (27)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_WID                ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_MSK                (0x08000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_MAX                (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_DEF                (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_OFF             (28)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_WID             ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_MSK             (0x10000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_MAX             (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_DEF             (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_OFF              (29)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_WID              ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MSK              (0x20000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MAX              (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_DEF              (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_OFF             (30)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_WID             ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MSK             (0x40000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MAX             (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_DEF             (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_OFF            (31)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_WID            ( 1)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MSK            (0x80000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MAX            (0x00000001)
  #define DDRCKECTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_DEF            (0x00000000)

#define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_REG                        (0x00003420)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_OFF                 ( 0)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_WID                 ( 4)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_MSK                 (0x0000000F)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_MAX                 (0x0000000F)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_DEF                 (0x0000000F)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_OFF             ( 4)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_WID             ( 2)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_MSK             (0x00000030)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_MAX             (0x00000003)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_DEF             (0x00000000)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_Spare_OFF                  ( 6)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_Spare_WID                  (26)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_Spare_MSK                  (0xFFFFFFC0)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_Spare_MAX                  (0x03FFFFFF)
  #define DDRCKECTLCH0_CR_DDRCRCTLRANKSUSED_Spare_DEF                  (0x00000000)

#define DDRCKECTLCH0_CR_DLLPITESTANDADC_REG                          (0x00003424)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_RunTest_OFF                  ( 0)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_RunTest_WID                  ( 1)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_RunTest_MSK                  (0x00000001)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_RunTest_MAX                  (0x00000001)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_RunTest_DEF                  (0x00000000)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Load_OFF                     ( 1)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Load_WID                     ( 1)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Load_MSK                     (0x00000002)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Load_MAX                     (0x00000001)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Load_DEF                     (0x00000000)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeHVM_OFF                  ( 2)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeHVM_WID                  ( 1)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeHVM_MSK                  (0x00000004)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeHVM_MAX                  (0x00000001)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeHVM_DEF                  (0x00000000)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeDV_OFF                   ( 3)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeDV_WID                   ( 1)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeDV_MSK                   (0x00000008)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeDV_MAX                   (0x00000001)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeDV_DEF                   (0x00000000)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeADC_OFF                  ( 4)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeADC_WID                  ( 1)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeADC_MSK                  (0x00000010)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeADC_MAX                  (0x00000001)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_ModeADC_DEF                  (0x00000000)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_LoadCount_OFF                ( 5)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_LoadCount_WID                (10)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_LoadCount_MSK                (0x00007FE0)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_LoadCount_MAX                (0x000003FF)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_CountStatus_OFF              (15)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_CountStatus_WID              (10)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_CountStatus_MSK              (0x01FF8000)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_CountStatus_MAX              (0x000003FF)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_CountStatus_DEF              (0x00000000)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Spare_OFF                    (25)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Spare_WID                    ( 7)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Spare_MSK                    (0xFE000000)
  #define DDRCKECTLCH0_CR_DLLPITESTANDADC_Spare_MAX                    (0x0000007F)

#define DDRCKECTLCH1_CR_DDRCRCTLCOMP_REG                             (0x00003510)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Scomp_OFF                       ( 0)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Scomp_WID                       ( 6)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Scomp_MSK                       (0x0000003F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Scomp_MAX                       (0x0000003F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Scomp_DEF                       (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_TcoComp_OFF                     ( 6)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_TcoComp_WID                     ( 6)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_TcoComp_MSK                     (0x00000FC0)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_TcoComp_MAX                     (0x0000003F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_TcoComp_DEF                     (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_OFF                  (12)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_WID                  ( 6)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_MSK                  (0x0003F000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_MAX                  (0x0000003F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_DEF                  (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_OFF                (18)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_WID                ( 6)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_MSK                (0x00FC0000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_MAX                (0x0000003F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_DEF                (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_LsComp_OFF                      (24)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_LsComp_WID                      ( 3)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_LsComp_MSK                      (0x07000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_LsComp_MAX                      (0x00000007)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_LsComp_DEF                      (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Spare_OFF                       (27)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Spare_WID                       ( 5)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Spare_MSK                       (0xF8000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Spare_MAX                       (0x0000001F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMP_Spare_DEF                       (0x00000000)

#define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_REG                       (0x00003514)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_OFF           ( 0)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_WID           ( 5)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MSK           (0x0000001F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MAX           (0x0000001F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_DEF           (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_OFF         ( 5)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_WID         ( 4)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MSK         (0x000001E0)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MAX         (0x0000000F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_DEF         (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_OFF      ( 9)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_WID      ( 4)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MSK      (0x00001E00)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MAX      (0x0000000F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_DEF      (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_OFF    (13)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_WID    ( 4)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MSK    (0x0001E000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MAX    (0x0000000F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_DEF    (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_OFF                 (17)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_WID                 (15)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_MSK                 (0xFFFE0000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_MAX                 (0x00007FFF)
  #define DDRCKECTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_DEF                 (0x00000000)

#define DDRCKECTLCH1_CR_DDRCRCTLPICODING_REG                         (0x00003518)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_OFF              ( 0)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_WID              ( 7)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_MSK              (0x0000007F)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_MAX              (0x0000007F)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_DEF              (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_OFF              ( 7)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_WID              ( 7)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_MSK              (0x00003F80)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_MAX              (0x0000007F)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_DEF              (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_OFF              (14)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_WID              ( 7)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_MSK              (0x001FC000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_MAX              (0x0000007F)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_DEF              (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_OFF              (21)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_WID              ( 7)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_MSK              (0x0FE00000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_MAX              (0x0000007F)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_DEF              (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_OFF          (28)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_WID          ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_MSK          (0x10000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_MAX          (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_DEF          (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_Spare_OFF                   (29)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_Spare_WID                   ( 3)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_Spare_MSK                   (0xE0000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_Spare_MAX                   (0x00000007)
  #define DDRCKECTLCH1_CR_DDRCRCTLPICODING_Spare_DEF                   (0x00000000)

#define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_REG                         (0x0000351C)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RefPi_OFF                   ( 0)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RefPi_WID                   ( 4)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RefPi_MSK                   (0x0000000F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RefPi_MAX                   (0x0000000F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RefPi_DEF                   (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllMask_OFF                 ( 4)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllMask_WID                 ( 2)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllMask_MSK                 (0x00000030)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllMask_MAX                 (0x00000003)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllMask_DEF                 (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_OFF                ( 6)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_WID                ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_MSK                (0x00000040)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_MAX                (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_DEF                (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_TxOn_OFF                    ( 7)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_TxOn_WID                    ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_TxOn_MSK                    (0x00000080)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_TxOn_MAX                    (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_TxOn_DEF                    (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_OFF                ( 8)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_WID                ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_MSK                (0x00000100)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_MAX                (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_DEF                (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_OFF                ( 9)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_WID                ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_MSK                (0x00000200)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_MAX                (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_DEF                (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_OFF                 (10)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_WID                 ( 2)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_MSK                 (0x00000C00)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_MAX                 (0x00000003)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_DEF                 (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_OFF                 (12)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_WID                 ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_MSK                 (0x00001000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_MAX                 (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_DEF                 (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_OFF                 (13)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_WID                 ( 2)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_MSK                 (0x00006000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_MAX                 (0x00000003)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_DEF                 (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_OFF          (15)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_WID          ( 2)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MSK          (0x00018000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MAX          (0x00000003)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_DEF          (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_OFF                 (17)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_WID                 ( 2)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_MSK                 (0x00060000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_MAX                 (0x00000003)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_DEF                 (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_OFF                (19)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_WID                ( 2)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_MSK                (0x00180000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_MAX                (0x00000003)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_DEF                (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RxVref_OFF                  (21)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RxVref_WID                  ( 6)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RxVref_MSK                  (0x07E00000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RxVref_MAX                  (0x0000003F)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_RxVref_DEF                  (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_OFF                (27)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_WID                ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_MSK                (0x08000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_MAX                (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_DEF                (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_OFF             (28)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_WID             ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_MSK             (0x10000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_MAX             (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_DEF             (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_OFF              (29)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_WID              ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MSK              (0x20000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MAX              (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_DEF              (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_OFF             (30)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_WID             ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MSK             (0x40000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MAX             (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_DEF             (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_OFF            (31)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_WID            ( 1)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MSK            (0x80000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MAX            (0x00000001)
  #define DDRCKECTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_DEF            (0x00000000)

#define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_REG                        (0x00003520)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_OFF                 ( 0)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_WID                 ( 4)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_MSK                 (0x0000000F)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_MAX                 (0x0000000F)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_DEF                 (0x0000000F)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_OFF             ( 4)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_WID             ( 2)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_MSK             (0x00000030)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_MAX             (0x00000003)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_DEF             (0x00000000)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_Spare_OFF                  ( 6)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_Spare_WID                  (26)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_Spare_MSK                  (0xFFFFFFC0)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_Spare_MAX                  (0x03FFFFFF)
  #define DDRCKECTLCH1_CR_DDRCRCTLRANKSUSED_Spare_DEF                  (0x00000000)

#define DDRCKECTLCH1_CR_DLLPITESTANDADC_REG                          (0x00003524)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_RunTest_OFF                  ( 0)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_RunTest_WID                  ( 1)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_RunTest_MSK                  (0x00000001)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_RunTest_MAX                  (0x00000001)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_RunTest_DEF                  (0x00000000)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Load_OFF                     ( 1)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Load_WID                     ( 1)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Load_MSK                     (0x00000002)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Load_MAX                     (0x00000001)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Load_DEF                     (0x00000000)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeHVM_OFF                  ( 2)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeHVM_WID                  ( 1)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeHVM_MSK                  (0x00000004)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeHVM_MAX                  (0x00000001)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeHVM_DEF                  (0x00000000)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeDV_OFF                   ( 3)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeDV_WID                   ( 1)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeDV_MSK                   (0x00000008)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeDV_MAX                   (0x00000001)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeDV_DEF                   (0x00000000)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeADC_OFF                  ( 4)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeADC_WID                  ( 1)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeADC_MSK                  (0x00000010)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeADC_MAX                  (0x00000001)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_ModeADC_DEF                  (0x00000000)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_LoadCount_OFF                ( 5)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_LoadCount_WID                (10)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_LoadCount_MSK                (0x00007FE0)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_LoadCount_MAX                (0x000003FF)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_CountStatus_OFF              (15)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_CountStatus_WID              (10)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_CountStatus_MSK              (0x01FF8000)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_CountStatus_MAX              (0x000003FF)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_CountStatus_DEF              (0x00000000)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Spare_OFF                    (25)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Spare_WID                    ( 7)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Spare_MSK                    (0xFE000000)
  #define DDRCKECTLCH1_CR_DLLPITESTANDADC_Spare_MAX                    (0x0000007F)

#define DDRCKECH0_CR_DDRCRCMDCOMP_REG                                (0x00001200)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Scomp_OFF                          ( 0)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Scomp_WID                          ( 6)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_TcoComp_OFF                        ( 6)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_TcoComp_WID                        ( 6)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_LsComp_OFF                         (24)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_LsComp_WID                         ( 3)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_LsComp_DEF                         (0x00000004)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Spare_OFF                          (27)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Spare_WID                          ( 5)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCKECH0_CR_DDRCRCMDCOMP_Spare_DEF                          (0x00000000)

#define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_REG                          (0x00001204)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_Spare_WID                    (15)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCKECH0_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCKECH0_CR_DDRCRCMDPICODING_REG                            (0x00001208)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                 ( 0)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi0Code_WID                 ( 7)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                 (0x0000007F)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                 (0x0000007F)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                 (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                 ( 7)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi1Code_WID                 ( 7)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                 (0x00003F80)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                 (0x0000007F)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                 (0x00000000)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_Spare_OFF                      (14)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_Spare_WID                      (18)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_Spare_MSK                      (0xFFFFC000)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_Spare_MAX                      (0x0003FFFF)
  #define DDRCKECH0_CR_DDRCRCMDPICODING_Spare_DEF                      (0x00000000)

#define DDRCKECH0_CR_DDRCRCTLCOMP_REG                                (0x00001210)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Scomp_OFF                          ( 0)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Scomp_WID                          ( 6)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_TcoComp_OFF                        ( 6)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_TcoComp_WID                        ( 6)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_LsComp_OFF                         (24)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_LsComp_WID                         ( 3)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_LsComp_DEF                         (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Spare_OFF                          (27)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Spare_WID                          ( 5)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCKECH0_CR_DDRCRCTLCOMP_Spare_DEF                          (0x00000000)

#define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_REG                          (0x00001214)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_Spare_WID                    (15)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCKECH0_CR_DDRCRCTLCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCKECH0_CR_DDRCRCTLPICODING_REG                            (0x00001218)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode0_OFF                 ( 0)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode0_WID                 ( 7)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode0_MSK                 (0x0000007F)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode0_MAX                 (0x0000007F)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode0_DEF                 (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode1_OFF                 ( 7)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode1_WID                 ( 7)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode1_MSK                 (0x00003F80)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode1_MAX                 (0x0000007F)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode1_DEF                 (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode2_OFF                 (14)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode2_WID                 ( 7)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode2_MSK                 (0x001FC000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode2_MAX                 (0x0000007F)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode2_DEF                 (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode3_OFF                 (21)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode3_WID                 ( 7)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode3_MSK                 (0x0FE00000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode3_MAX                 (0x0000007F)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlPiCode3_DEF                 (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlXoverEnable_OFF             (28)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlXoverEnable_WID             ( 1)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlXoverEnable_MSK             (0x10000000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlXoverEnable_MAX             (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_CtlXoverEnable_DEF             (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_Spare_OFF                      (29)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_Spare_WID                      ( 3)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_Spare_MSK                      (0xE0000000)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_Spare_MAX                      (0x00000007)
  #define DDRCKECH0_CR_DDRCRCTLPICODING_Spare_DEF                      (0x00000000)

#define DDRCKECH0_CR_DDRCRCTLCONTROLS_REG                            (0x0000121C)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RefPi_WID                      ( 4)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllMask_WID                    ( 2)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_TxOn_WID                       ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_OdtMode_OFF                    (12)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CmdTxEq_OFF                    (13)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CmdTxEq_WID                    ( 2)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CmdTxEq_MSK                    (0x00006000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CmdTxEq_MAX                    (0x00000003)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CmdTxEq_DEF                    (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_OFF             (15)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_WID             ( 2)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MSK             (0x00018000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MAX             (0x00000003)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_DEF             (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlTxEq_OFF                    (17)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlTxEq_WID                    ( 2)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlTxEq_MSK                    (0x00060000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlTxEq_MAX                    (0x00000003)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlTxEq_DEF                    (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_OFF                   (19)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_WID                   ( 2)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_MSK                   (0x00180000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_MAX                   (0x00000003)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_DEF                   (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RxVref_OFF                     (21)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RxVref_WID                     ( 6)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_OFF                (30)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_WID                ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MSK                (0x40000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MAX                (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_DEF                (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_OFF               (31)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_WID               ( 1)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MSK               (0x80000000)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MAX               (0x00000001)
  #define DDRCKECH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_DEF               (0x00000000)

#define DDRCKECH0_CR_DDRCRCTLRANKSUSED_REG                           (0x00001220)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_RankEn_OFF                    ( 0)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_RankEn_WID                    ( 4)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_RankEn_MSK                    (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_RankEn_MAX                    (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_RankEn_DEF                    (0x0000000F)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_OdtDisable_OFF                ( 4)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_OdtDisable_WID                ( 2)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_OdtDisable_MSK                (0x00000030)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_OdtDisable_MAX                (0x00000003)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_OdtDisable_DEF                (0x00000000)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_Spare_OFF                     ( 6)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_Spare_WID                     (26)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_Spare_MSK                     (0xFFFFFFC0)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_Spare_MAX                     (0x03FFFFFF)
  #define DDRCKECH0_CR_DDRCRCTLRANKSUSED_Spare_DEF                     (0x00000000)

#define DDRCKECH0_CR_DLLPITESTANDADC_REG                             (0x00001224)
  #define DDRCKECH0_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCKECH0_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCKECH0_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCKECH0_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCKECH0_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCKECH0_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCKECH0_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCKECH0_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCKECH0_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCKECH0_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCKECH0_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCKECH0_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCKECH0_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCKECH0_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCKECH0_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCKECH0_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#define DDRCKECH1_CR_DDRCRCMDCOMP_REG                                (0x00001300)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Scomp_OFF                          ( 0)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Scomp_WID                          ( 6)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_TcoComp_OFF                        ( 6)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_TcoComp_WID                        ( 6)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_LsComp_OFF                         (24)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_LsComp_WID                         ( 3)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_LsComp_DEF                         (0x00000004)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Spare_OFF                          (27)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Spare_WID                          ( 5)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCKECH1_CR_DDRCRCMDCOMP_Spare_DEF                          (0x00000000)

#define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_REG                          (0x00001304)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_Spare_WID                    (15)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCKECH1_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCKECH1_CR_DDRCRCMDPICODING_REG                            (0x00001308)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                 ( 0)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi0Code_WID                 ( 7)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                 (0x0000007F)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                 (0x0000007F)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                 (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                 ( 7)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi1Code_WID                 ( 7)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                 (0x00003F80)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                 (0x0000007F)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                 (0x00000000)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_Spare_OFF                      (14)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_Spare_WID                      (18)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_Spare_MSK                      (0xFFFFC000)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_Spare_MAX                      (0x0003FFFF)
  #define DDRCKECH1_CR_DDRCRCMDPICODING_Spare_DEF                      (0x00000000)

#define DDRCKECH1_CR_DDRCRCTLCOMP_REG                                (0x00001310)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Scomp_OFF                          ( 0)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Scomp_WID                          ( 6)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_TcoComp_OFF                        ( 6)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_TcoComp_WID                        ( 6)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_LsComp_OFF                         (24)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_LsComp_WID                         ( 3)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_LsComp_DEF                         (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Spare_OFF                          (27)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Spare_WID                          ( 5)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCKECH1_CR_DDRCRCTLCOMP_Spare_DEF                          (0x00000000)

#define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_REG                          (0x00001314)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_Spare_WID                    (15)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCKECH1_CR_DDRCRCTLCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCKECH1_CR_DDRCRCTLPICODING_REG                            (0x00001318)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode0_OFF                 ( 0)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode0_WID                 ( 7)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode0_MSK                 (0x0000007F)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode0_MAX                 (0x0000007F)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode0_DEF                 (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode1_OFF                 ( 7)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode1_WID                 ( 7)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode1_MSK                 (0x00003F80)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode1_MAX                 (0x0000007F)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode1_DEF                 (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode2_OFF                 (14)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode2_WID                 ( 7)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode2_MSK                 (0x001FC000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode2_MAX                 (0x0000007F)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode2_DEF                 (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode3_OFF                 (21)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode3_WID                 ( 7)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode3_MSK                 (0x0FE00000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode3_MAX                 (0x0000007F)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlPiCode3_DEF                 (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlXoverEnable_OFF             (28)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlXoverEnable_WID             ( 1)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlXoverEnable_MSK             (0x10000000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlXoverEnable_MAX             (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_CtlXoverEnable_DEF             (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_Spare_OFF                      (29)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_Spare_WID                      ( 3)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_Spare_MSK                      (0xE0000000)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_Spare_MAX                      (0x00000007)
  #define DDRCKECH1_CR_DDRCRCTLPICODING_Spare_DEF                      (0x00000000)

#define DDRCKECH1_CR_DDRCRCTLCONTROLS_REG                            (0x0000131C)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RefPi_WID                      ( 4)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllMask_WID                    ( 2)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_TxOn_WID                       ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_OdtMode_OFF                    (12)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CmdTxEq_OFF                    (13)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CmdTxEq_WID                    ( 2)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CmdTxEq_MSK                    (0x00006000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CmdTxEq_MAX                    (0x00000003)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CmdTxEq_DEF                    (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_OFF             (15)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_WID             ( 2)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MSK             (0x00018000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MAX             (0x00000003)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_DEF             (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlTxEq_OFF                    (17)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlTxEq_WID                    ( 2)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlTxEq_MSK                    (0x00060000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlTxEq_MAX                    (0x00000003)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlTxEq_DEF                    (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_OFF                   (19)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_WID                   ( 2)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_MSK                   (0x00180000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_MAX                   (0x00000003)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_DEF                   (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RxVref_OFF                     (21)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RxVref_WID                     ( 6)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_OFF                (30)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_WID                ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MSK                (0x40000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MAX                (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_DEF                (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_OFF               (31)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_WID               ( 1)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MSK               (0x80000000)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MAX               (0x00000001)
  #define DDRCKECH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_DEF               (0x00000000)

#define DDRCKECH1_CR_DDRCRCTLRANKSUSED_REG                           (0x00001320)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_RankEn_OFF                    ( 0)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_RankEn_WID                    ( 4)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_RankEn_MSK                    (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_RankEn_MAX                    (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_RankEn_DEF                    (0x0000000F)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_OdtDisable_OFF                ( 4)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_OdtDisable_WID                ( 2)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_OdtDisable_MSK                (0x00000030)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_OdtDisable_MAX                (0x00000003)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_OdtDisable_DEF                (0x00000000)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_Spare_OFF                     ( 6)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_Spare_WID                     (26)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_Spare_MSK                     (0xFFFFFFC0)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_Spare_MAX                     (0x03FFFFFF)
  #define DDRCKECH1_CR_DDRCRCTLRANKSUSED_Spare_DEF                     (0x00000000)

#define DDRCKECH1_CR_DLLPITESTANDADC_REG                             (0x00001324)
  #define DDRCKECH1_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCKECH1_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCKECH1_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCKECH1_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCKECH1_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCKECH1_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCKECH1_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCKECH1_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCKECH1_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCKECH1_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCKECH1_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCKECH1_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCKECH1_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCKECH1_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCKECH1_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCKECH1_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#define DDRCTLCH0_CR_DDRCRCTLCOMP_REG                                (0x00001C10)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Scomp_OFF                          ( 0)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Scomp_WID                          ( 6)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_TcoComp_OFF                        ( 6)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_TcoComp_WID                        ( 6)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_LsComp_OFF                         (24)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_LsComp_WID                         ( 3)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_LsComp_DEF                         (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Spare_OFF                          (27)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Spare_WID                          ( 5)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMP_Spare_DEF                          (0x00000000)

#define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_REG                          (0x00001C14)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_WID                    (15)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCTLCH0_CR_DDRCRCTLCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCTLCH0_CR_DDRCRCTLPICODING_REG                            (0x00001C18)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_OFF                 ( 0)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_WID                 ( 7)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_MSK                 (0x0000007F)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_MAX                 (0x0000007F)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode0_DEF                 (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_OFF                 ( 7)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_WID                 ( 7)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_MSK                 (0x00003F80)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_MAX                 (0x0000007F)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode1_DEF                 (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_OFF                 (14)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_WID                 ( 7)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_MSK                 (0x001FC000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_MAX                 (0x0000007F)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode2_DEF                 (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_OFF                 (21)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_WID                 ( 7)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_MSK                 (0x0FE00000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_MAX                 (0x0000007F)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlPiCode3_DEF                 (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_OFF             (28)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_WID             ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_MSK             (0x10000000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_MAX             (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_CtlXoverEnable_DEF             (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_Spare_OFF                      (29)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_Spare_WID                      ( 3)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_Spare_MSK                      (0xE0000000)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_Spare_MAX                      (0x00000007)
  #define DDRCTLCH0_CR_DDRCRCTLPICODING_Spare_DEF                      (0x00000000)

#define DDRCTLCH0_CR_DDRCRCTLCONTROLS_REG                            (0x00001C1C)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RefPi_WID                      ( 4)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllMask_WID                    ( 2)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_TxOn_WID                       ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_OFF                    (12)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_OFF                    (13)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_WID                    ( 2)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_MSK                    (0x00006000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_MAX                    (0x00000003)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CmdTxEq_DEF                    (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_OFF             (15)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_WID             ( 2)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MSK             (0x00018000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MAX             (0x00000003)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_DEF             (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_OFF                    (17)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_WID                    ( 2)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_MSK                    (0x00060000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_MAX                    (0x00000003)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlTxEq_DEF                    (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_OFF                   (19)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_WID                   ( 2)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_MSK                   (0x00180000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_MAX                   (0x00000003)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_CtlSRDrv_DEF                   (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RxVref_OFF                     (21)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RxVref_WID                     ( 6)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_OFF                (30)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_WID                ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MSK                (0x40000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MAX                (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_DEF                (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_OFF               (31)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_WID               ( 1)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MSK               (0x80000000)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MAX               (0x00000001)
  #define DDRCTLCH0_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_DEF               (0x00000000)

#define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_REG                           (0x00001C20)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_OFF                    ( 0)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_WID                    ( 4)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_MSK                    (0x0000000F)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_MAX                    (0x0000000F)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_RankEn_DEF                    (0x0000000F)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_OFF                ( 4)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_WID                ( 2)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_MSK                (0x00000030)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_MAX                (0x00000003)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_OdtDisable_DEF                (0x00000000)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_Spare_OFF                     ( 6)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_Spare_WID                     (26)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_Spare_MSK                     (0xFFFFFFC0)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_Spare_MAX                     (0x03FFFFFF)
  #define DDRCTLCH0_CR_DDRCRCTLRANKSUSED_Spare_DEF                     (0x00000000)

#define DDRCTLCH0_CR_DLLPITESTANDADC_REG                             (0x00001C24)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCTLCH0_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#define DDRCTLCH1_CR_DDRCRCTLCOMP_REG                                (0x00001D10)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Scomp_OFF                          ( 0)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Scomp_WID                          ( 6)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_TcoComp_OFF                        ( 6)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_TcoComp_WID                        ( 6)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_LsComp_OFF                         (24)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_LsComp_WID                         ( 3)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_LsComp_DEF                         (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Spare_OFF                          (27)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Spare_WID                          ( 5)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMP_Spare_DEF                          (0x00000000)

#define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_REG                          (0x00001D14)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_WID                    (15)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCTLCH1_CR_DDRCRCTLCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCTLCH1_CR_DDRCRCTLPICODING_REG                            (0x00001D18)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_OFF                 ( 0)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_WID                 ( 7)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_MSK                 (0x0000007F)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_MAX                 (0x0000007F)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode0_DEF                 (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_OFF                 ( 7)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_WID                 ( 7)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_MSK                 (0x00003F80)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_MAX                 (0x0000007F)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode1_DEF                 (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_OFF                 (14)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_WID                 ( 7)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_MSK                 (0x001FC000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_MAX                 (0x0000007F)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode2_DEF                 (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_OFF                 (21)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_WID                 ( 7)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_MSK                 (0x0FE00000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_MAX                 (0x0000007F)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlPiCode3_DEF                 (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_OFF             (28)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_WID             ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_MSK             (0x10000000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_MAX             (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_CtlXoverEnable_DEF             (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_Spare_OFF                      (29)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_Spare_WID                      ( 3)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_Spare_MSK                      (0xE0000000)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_Spare_MAX                      (0x00000007)
  #define DDRCTLCH1_CR_DDRCRCTLPICODING_Spare_DEF                      (0x00000000)

#define DDRCTLCH1_CR_DDRCRCTLCONTROLS_REG                            (0x00001D1C)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RefPi_WID                      ( 4)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllMask_WID                    ( 2)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_TxOn_WID                       ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_OFF                    (12)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_OFF                    (13)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_WID                    ( 2)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_MSK                    (0x00006000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_MAX                    (0x00000003)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CmdTxEq_DEF                    (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_OFF             (15)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_WID             ( 2)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MSK             (0x00018000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_MAX             (0x00000003)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_EarlyWeakDrive_DEF             (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_OFF                    (17)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_WID                    ( 2)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_MSK                    (0x00060000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_MAX                    (0x00000003)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlTxEq_DEF                    (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_OFF                   (19)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_WID                   ( 2)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_MSK                   (0x00180000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_MAX                   (0x00000003)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_CtlSRDrv_DEF                   (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RxVref_OFF                     (21)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RxVref_WID                     ( 6)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_OFF                (30)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_WID                ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MSK                (0x40000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_MAX                (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LaDrvEnOvrd_DEF                (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_OFF               (31)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_WID               ( 1)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MSK               (0x80000000)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_MAX               (0x00000001)
  #define DDRCTLCH1_CR_DDRCRCTLCONTROLS_LPDdrCAA_Dis_DEF               (0x00000000)

#define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_REG                           (0x00001D20)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_OFF                    ( 0)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_WID                    ( 4)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_MSK                    (0x0000000F)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_MAX                    (0x0000000F)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_RankEn_DEF                    (0x0000000F)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_OFF                ( 4)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_WID                ( 2)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_MSK                (0x00000030)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_MAX                (0x00000003)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_OdtDisable_DEF                (0x00000000)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_Spare_OFF                     ( 6)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_Spare_WID                     (26)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_Spare_MSK                     (0xFFFFFFC0)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_Spare_MAX                     (0x03FFFFFF)
  #define DDRCTLCH1_CR_DDRCRCTLRANKSUSED_Spare_DEF                     (0x00000000)

#define DDRCTLCH1_CR_DLLPITESTANDADC_REG                             (0x00001D24)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCTLCH1_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#pragma pack(pop)
#endif  // __McIoCkeCtl_h__
