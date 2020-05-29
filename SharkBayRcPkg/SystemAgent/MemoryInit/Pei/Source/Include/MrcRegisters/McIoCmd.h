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

#ifndef _MC_IO_CMD_H_
#define _MC_IO_CMD_H_

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
} DDRCMD_CR_DDRCRCMDCOMP_STRUCT;

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
} DDRCMD_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMD_CR_DDRCRCMDPICODING_STRUCT;

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
    U32 Reserved                                :  4;  // Bits 20:17
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LPDdrCAA_Dis                            :  1;  // Bits 30:30
    U32 LPDdrCAB_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMD_CR_DDRCRCMDCONTROLS_STRUCT;

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
} DDRCMD_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCMDNCH0_CR_DDRCRCMDCOMP_STRUCT;

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
} DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDNCH0_CR_DDRCRCMDPICODING_STRUCT;

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
    U32 Reserved                                :  4;  // Bits 20:17
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LPDdrCAA_Dis                            :  1;  // Bits 30:30
    U32 LPDdrCAB_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDNCH0_CR_DDRCRCMDCONTROLS_STRUCT;

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
} DDRCMDNCH0_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCMDNCH1_CR_DDRCRCMDCOMP_STRUCT;

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
} DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDNCH1_CR_DDRCRCMDPICODING_STRUCT;

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
    U32 Reserved                                :  4;  // Bits 20:17
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LPDdrCAA_Dis                            :  1;  // Bits 30:30
    U32 LPDdrCAB_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDNCH1_CR_DDRCRCMDCONTROLS_STRUCT;

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
} DDRCMDNCH1_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCMDSCH0_CR_DDRCRCMDCOMP_STRUCT;

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
} DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDSCH0_CR_DDRCRCMDPICODING_STRUCT;

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
    U32 Reserved                                :  4;  // Bits 20:17
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LPDdrCAA_Dis                            :  1;  // Bits 30:30
    U32 LPDdrCAB_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDSCH0_CR_DDRCRCMDCONTROLS_STRUCT;

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
} DDRCMDSCH0_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCMDSCH1_CR_DDRCRCMDCOMP_STRUCT;

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
} DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDSCH1_CR_DDRCRCMDPICODING_STRUCT;

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
    U32 Reserved                                :  4;  // Bits 20:17
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LPDdrCAA_Dis                            :  1;  // Bits 30:30
    U32 LPDdrCAB_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDSCH1_CR_DDRCRCMDCONTROLS_STRUCT;

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
} DDRCMDSCH1_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCMDCH0_CR_DDRCRCMDCOMP_STRUCT;

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
} DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDCH0_CR_DDRCRCMDPICODING_STRUCT;

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
    U32 Reserved                                :  4;  // Bits 20:17
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LPDdrCAA_Dis                            :  1;  // Bits 30:30
    U32 LPDdrCAB_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDCH0_CR_DDRCRCMDCONTROLS_STRUCT;

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
} DDRCMDCH0_CR_DLLPITESTANDADC_STRUCT;

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
} DDRCMDCH1_CR_DDRCRCMDCOMP_STRUCT;

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
} DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_STRUCT;

typedef union {
  struct {
    U32 CmdPi0Code                              :  7;  // Bits 6:0
    U32 CmdPi1Code                              :  7;  // Bits 13:7
    U32 Spare                                   :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDCH1_CR_DDRCRCMDPICODING_STRUCT;

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
    U32 Reserved                                :  4;  // Bits 20:17
    U32 RxVref                                  :  6;  // Bits 26:21
    U32 VccddqHi                                :  1;  // Bits 27:27
    U32 DllWeakLock                             :  1;  // Bits 28:28
    U32 LPDDR_Mode                              :  1;  // Bits 29:29
    U32 LPDdrCAA_Dis                            :  1;  // Bits 30:30
    U32 LPDdrCAB_Dis                            :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DDRCMDCH1_CR_DDRCRCMDCONTROLS_STRUCT;

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
} DDRCMDCH1_CR_DLLPITESTANDADC_STRUCT;


#define DDRCMD_CR_DDRCRCMDCOMP_REG                                   (0x00003700)
  #define DDRCMD_CR_DDRCRCMDCOMP_Scomp_OFF                             ( 0)
  #define DDRCMD_CR_DDRCRCMDCOMP_Scomp_WID                             ( 6)
  #define DDRCMD_CR_DDRCRCMDCOMP_Scomp_MSK                             (0x0000003F)
  #define DDRCMD_CR_DDRCRCMDCOMP_Scomp_MAX                             (0x0000003F)
  #define DDRCMD_CR_DDRCRCMDCOMP_Scomp_DEF                             (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCOMP_TcoComp_OFF                           ( 6)
  #define DDRCMD_CR_DDRCRCMDCOMP_TcoComp_WID                           ( 6)
  #define DDRCMD_CR_DDRCRCMDCOMP_TcoComp_MSK                           (0x00000FC0)
  #define DDRCMD_CR_DDRCRCMDCOMP_TcoComp_MAX                           (0x0000003F)
  #define DDRCMD_CR_DDRCRCMDCOMP_TcoComp_DEF                           (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                        (12)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvUp_WID                        ( 6)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                        (0x0003F000)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                        (0x0000003F)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                        (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                      (18)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvDown_WID                      ( 6)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                      (0x00FC0000)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                      (0x0000003F)
  #define DDRCMD_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                      (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCOMP_LsComp_OFF                            (24)
  #define DDRCMD_CR_DDRCRCMDCOMP_LsComp_WID                            ( 3)
  #define DDRCMD_CR_DDRCRCMDCOMP_LsComp_MSK                            (0x07000000)
  #define DDRCMD_CR_DDRCRCMDCOMP_LsComp_MAX                            (0x00000007)
  #define DDRCMD_CR_DDRCRCMDCOMP_LsComp_DEF                            (0x00000004)
  #define DDRCMD_CR_DDRCRCMDCOMP_Spare_OFF                             (27)
  #define DDRCMD_CR_DDRCRCMDCOMP_Spare_WID                             ( 5)
  #define DDRCMD_CR_DDRCRCMDCOMP_Spare_MSK                             (0xF8000000)
  #define DDRCMD_CR_DDRCRCMDCOMP_Spare_MAX                             (0x0000001F)
  #define DDRCMD_CR_DDRCRCMDCOMP_Spare_DEF                             (0x00000000)

#define DDRCMD_CR_DDRCRCMDCOMPOFFSET_REG                             (0x00003704)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF                 ( 0)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID                 ( 5)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK                 (0x0000001F)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX                 (0x0000001F)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF                 (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF               ( 5)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID               ( 4)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK               (0x000001E0)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX               (0x0000000F)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF               (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF            ( 9)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID            ( 4)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK            (0x00001E00)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX            (0x0000000F)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF            (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF          (13)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID          ( 4)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK          (0x0001E000)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX          (0x0000000F)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF          (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                       (17)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_Spare_WID                       (15)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                       (0xFFFE0000)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                       (0x00007FFF)
  #define DDRCMD_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                       (0x00000000)

#define DDRCMD_CR_DDRCRCMDPICODING_REG                               (0x00003708)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                    ( 0)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi0Code_WID                    ( 7)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                    (0x0000007F)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                    (0x0000007F)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                    (0x00000000)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                    ( 7)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi1Code_WID                    ( 7)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                    (0x00003F80)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                    (0x0000007F)
  #define DDRCMD_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                    (0x00000000)
  #define DDRCMD_CR_DDRCRCMDPICODING_Spare_OFF                         (14)
  #define DDRCMD_CR_DDRCRCMDPICODING_Spare_WID                         (18)
  #define DDRCMD_CR_DDRCRCMDPICODING_Spare_MSK                         (0xFFFFC000)
  #define DDRCMD_CR_DDRCRCMDPICODING_Spare_MAX                         (0x0003FFFF)
  #define DDRCMD_CR_DDRCRCMDPICODING_Spare_DEF                         (0x00000000)

#define DDRCMD_CR_DDRCRCMDCONTROLS_REG                               (0x0000370C)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RefPi_OFF                         ( 0)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RefPi_WID                         ( 4)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RefPi_MSK                         (0x0000000F)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RefPi_MAX                         (0x0000000F)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RefPi_DEF                         (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllMask_OFF                       ( 4)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllMask_WID                       ( 2)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllMask_MSK                       (0x00000030)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllMask_MAX                       (0x00000003)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllMask_DEF                       (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllRsvd1_OFF                      ( 6)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllRsvd1_WID                      ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllRsvd1_MSK                      (0x00000040)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllRsvd1_MAX                      (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllRsvd1_DEF                      (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_TxOn_OFF                          ( 7)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_TxOn_WID                          ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_TxOn_MSK                          (0x00000080)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_TxOn_MAX                          (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_TxOn_DEF                          (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IntClkOn_OFF                      ( 8)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IntClkOn_WID                      ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IntClkOn_MSK                      (0x00000100)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IntClkOn_MAX                      (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IntClkOn_DEF                      (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RepClkOn_OFF                      ( 9)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RepClkOn_WID                      ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RepClkOn_MSK                      (0x00000200)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RepClkOn_MAX                      (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RepClkOn_DEF                      (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IOLBCtl_OFF                       (10)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IOLBCtl_WID                       ( 2)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IOLBCtl_MSK                       (0x00000C00)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IOLBCtl_MAX                       (0x00000003)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_IOLBCtl_DEF                       (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_OdtMode_OFF                       (12)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_OdtMode_WID                       ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_OdtMode_MSK                       (0x00001000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_OdtMode_MAX                       (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_OdtMode_DEF                       (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_CmdTxEq_OFF                       (13)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_CmdTxEq_WID                       ( 2)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_CmdTxEq_MSK                       (0x00006000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_CmdTxEq_MAX                       (0x00000003)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_CmdTxEq_DEF                       (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_OFF                (15)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_WID                ( 2)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MSK                (0x00018000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MAX                (0x00000003)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_DEF                (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_Reserved_OFF                      (17)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_Reserved_WID                      ( 4)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_Reserved_MSK                      (0x001E0000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_Reserved_MAX                      (0x0000000F)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_Reserved_DEF                      (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RxVref_OFF                        (21)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RxVref_WID                        ( 6)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RxVref_MSK                        (0x07E00000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RxVref_MAX                        (0x0000003F)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_RxVref_DEF                        (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_VccddqHi_OFF                      (27)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_VccddqHi_WID                      ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_VccddqHi_MSK                      (0x08000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_VccddqHi_MAX                      (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_VccddqHi_DEF                      (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllWeakLock_OFF                   (28)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllWeakLock_WID                   ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllWeakLock_MSK                   (0x10000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllWeakLock_MAX                   (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_DllWeakLock_DEF                   (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDDR_Mode_OFF                    (29)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDDR_Mode_WID                    ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MSK                    (0x20000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MAX                    (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDDR_Mode_DEF                    (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_OFF                  (30)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_WID                  ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MSK                  (0x40000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MAX                  (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_DEF                  (0x00000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_OFF                  (31)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_WID                  ( 1)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MSK                  (0x80000000)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MAX                  (0x00000001)
  #define DDRCMD_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_DEF                  (0x00000000)

#define DDRCMD_CR_DLLPITESTANDADC_REG                                (0x00003724)
  #define DDRCMD_CR_DLLPITESTANDADC_RunTest_OFF                        ( 0)
  #define DDRCMD_CR_DLLPITESTANDADC_RunTest_WID                        ( 1)
  #define DDRCMD_CR_DLLPITESTANDADC_RunTest_MSK                        (0x00000001)
  #define DDRCMD_CR_DLLPITESTANDADC_RunTest_MAX                        (0x00000001)
  #define DDRCMD_CR_DLLPITESTANDADC_RunTest_DEF                        (0x00000000)
  #define DDRCMD_CR_DLLPITESTANDADC_Load_OFF                           ( 1)
  #define DDRCMD_CR_DLLPITESTANDADC_Load_WID                           ( 1)
  #define DDRCMD_CR_DLLPITESTANDADC_Load_MSK                           (0x00000002)
  #define DDRCMD_CR_DLLPITESTANDADC_Load_MAX                           (0x00000001)
  #define DDRCMD_CR_DLLPITESTANDADC_Load_DEF                           (0x00000000)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeHVM_OFF                        ( 2)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeHVM_WID                        ( 1)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeHVM_MSK                        (0x00000004)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeHVM_MAX                        (0x00000001)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeHVM_DEF                        (0x00000000)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeDV_OFF                         ( 3)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeDV_WID                         ( 1)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeDV_MSK                         (0x00000008)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeDV_MAX                         (0x00000001)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeDV_DEF                         (0x00000000)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeADC_OFF                        ( 4)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeADC_WID                        ( 1)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeADC_MSK                        (0x00000010)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeADC_MAX                        (0x00000001)
  #define DDRCMD_CR_DLLPITESTANDADC_ModeADC_DEF                        (0x00000000)
  #define DDRCMD_CR_DLLPITESTANDADC_LoadCount_OFF                      ( 5)
  #define DDRCMD_CR_DLLPITESTANDADC_LoadCount_WID                      (10)
  #define DDRCMD_CR_DLLPITESTANDADC_LoadCount_MSK                      (0x00007FE0)
  #define DDRCMD_CR_DLLPITESTANDADC_LoadCount_MAX                      (0x000003FF)
  #define DDRCMD_CR_DLLPITESTANDADC_CountStatus_OFF                    (15)
  #define DDRCMD_CR_DLLPITESTANDADC_CountStatus_WID                    (10)
  #define DDRCMD_CR_DLLPITESTANDADC_CountStatus_MSK                    (0x01FF8000)
  #define DDRCMD_CR_DLLPITESTANDADC_CountStatus_MAX                    (0x000003FF)
  #define DDRCMD_CR_DLLPITESTANDADC_CountStatus_DEF                    (0x00000000)
  #define DDRCMD_CR_DLLPITESTANDADC_Spare_OFF                          (25)
  #define DDRCMD_CR_DLLPITESTANDADC_Spare_WID                          ( 7)
  #define DDRCMD_CR_DLLPITESTANDADC_Spare_MSK                          (0xFE000000)
  #define DDRCMD_CR_DLLPITESTANDADC_Spare_MAX                          (0x0000007F)

#define DDRCMDNCH0_CR_DDRCRCMDCOMP_REG                               (0x00001400)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Scomp_OFF                         ( 0)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Scomp_WID                         ( 6)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Scomp_MSK                         (0x0000003F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Scomp_MAX                         (0x0000003F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Scomp_DEF                         (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_TcoComp_OFF                       ( 6)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_TcoComp_WID                       ( 6)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_TcoComp_MSK                       (0x00000FC0)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_TcoComp_MAX                       (0x0000003F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_TcoComp_DEF                       (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                    (12)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvUp_WID                    ( 6)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                    (0x0003F000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                    (0x0000003F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                    (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                  (18)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvDown_WID                  ( 6)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                  (0x00FC0000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                  (0x0000003F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                  (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_LsComp_OFF                        (24)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_LsComp_WID                        ( 3)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_LsComp_MSK                        (0x07000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_LsComp_MAX                        (0x00000007)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_LsComp_DEF                        (0x00000004)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Spare_OFF                         (27)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Spare_WID                         ( 5)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Spare_MSK                         (0xF8000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Spare_MAX                         (0x0000001F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMP_Spare_DEF                         (0x00000000)

#define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_REG                         (0x00001404)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF             ( 0)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID             ( 5)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK             (0x0000001F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX             (0x0000001F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF             (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF           ( 5)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID           ( 4)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK           (0x000001E0)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX           (0x0000000F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF           (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF        ( 9)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID        ( 4)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK        (0x00001E00)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX        (0x0000000F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF        (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF      (13)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID      ( 4)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK      (0x0001E000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX      (0x0000000F)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF      (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                   (17)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_Spare_WID                   (15)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                   (0xFFFE0000)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                   (0x00007FFF)
  #define DDRCMDNCH0_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                   (0x00000000)

#define DDRCMDNCH0_CR_DDRCRCMDPICODING_REG                           (0x00001408)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                ( 0)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi0Code_WID                ( 7)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                (0x0000007F)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                (0x0000007F)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                ( 7)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi1Code_WID                ( 7)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                (0x00003F80)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                (0x0000007F)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_Spare_OFF                     (14)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_Spare_WID                     (18)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_Spare_MSK                     (0xFFFFC000)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_Spare_MAX                     (0x0003FFFF)
  #define DDRCMDNCH0_CR_DDRCRCMDPICODING_Spare_DEF                     (0x00000000)

#define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_REG                           (0x0000140C)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RefPi_OFF                     ( 0)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RefPi_WID                     ( 4)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RefPi_MSK                     (0x0000000F)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RefPi_MAX                     (0x0000000F)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RefPi_DEF                     (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllMask_OFF                   ( 4)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllMask_WID                   ( 2)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllMask_MSK                   (0x00000030)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllMask_MAX                   (0x00000003)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllMask_DEF                   (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_OFF                  ( 6)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_WID                  ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_MSK                  (0x00000040)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_MAX                  (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_DEF                  (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_TxOn_OFF                      ( 7)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_TxOn_WID                      ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_TxOn_MSK                      (0x00000080)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_TxOn_MAX                      (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_TxOn_DEF                      (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IntClkOn_OFF                  ( 8)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IntClkOn_WID                  ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IntClkOn_MSK                  (0x00000100)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IntClkOn_MAX                  (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IntClkOn_DEF                  (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RepClkOn_OFF                  ( 9)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RepClkOn_WID                  ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RepClkOn_MSK                  (0x00000200)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RepClkOn_MAX                  (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RepClkOn_DEF                  (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_OFF                   (10)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_WID                   ( 2)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_MSK                   (0x00000C00)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_MAX                   (0x00000003)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_DEF                   (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_OdtMode_OFF                   (12)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_OdtMode_WID                   ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_OdtMode_MSK                   (0x00001000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_OdtMode_MAX                   (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_OdtMode_DEF                   (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_OFF                   (13)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_WID                   ( 2)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_MSK                   (0x00006000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_MAX                   (0x00000003)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_DEF                   (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_OFF            (15)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_WID            ( 2)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MSK            (0x00018000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MAX            (0x00000003)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_DEF            (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_Reserved_OFF                  (17)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_Reserved_WID                  ( 4)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_Reserved_MSK                  (0x001E0000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_Reserved_MAX                  (0x0000000F)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_Reserved_DEF                  (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RxVref_OFF                    (21)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RxVref_WID                    ( 6)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RxVref_MSK                    (0x07E00000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RxVref_MAX                    (0x0000003F)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_RxVref_DEF                    (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_VccddqHi_OFF                  (27)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_VccddqHi_WID                  ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_VccddqHi_MSK                  (0x08000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_VccddqHi_MAX                  (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_VccddqHi_DEF                  (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_OFF               (28)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_WID               ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_MSK               (0x10000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_MAX               (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_DEF               (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_OFF                (29)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_WID                ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MSK                (0x20000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MAX                (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_DEF                (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_OFF              (30)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_WID              ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MSK              (0x40000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MAX              (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_DEF              (0x00000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_OFF              (31)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_WID              ( 1)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MSK              (0x80000000)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MAX              (0x00000001)
  #define DDRCMDNCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_DEF              (0x00000000)

#define DDRCMDNCH0_CR_DLLPITESTANDADC_REG                            (0x00001424)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_RunTest_OFF                    ( 0)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_RunTest_WID                    ( 1)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_RunTest_MSK                    (0x00000001)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_RunTest_MAX                    (0x00000001)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_RunTest_DEF                    (0x00000000)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Load_OFF                       ( 1)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Load_WID                       ( 1)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Load_MSK                       (0x00000002)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Load_MAX                       (0x00000001)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Load_DEF                       (0x00000000)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeHVM_OFF                    ( 2)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeHVM_WID                    ( 1)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeHVM_MSK                    (0x00000004)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeHVM_MAX                    (0x00000001)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeHVM_DEF                    (0x00000000)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeDV_OFF                     ( 3)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeDV_WID                     ( 1)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeDV_MSK                     (0x00000008)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeDV_MAX                     (0x00000001)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeDV_DEF                     (0x00000000)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeADC_OFF                    ( 4)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeADC_WID                    ( 1)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeADC_MSK                    (0x00000010)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeADC_MAX                    (0x00000001)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_ModeADC_DEF                    (0x00000000)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_LoadCount_OFF                  ( 5)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_LoadCount_WID                  (10)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_LoadCount_MSK                  (0x00007FE0)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_LoadCount_MAX                  (0x000003FF)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_CountStatus_OFF                (15)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_CountStatus_WID                (10)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_CountStatus_MSK                (0x01FF8000)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_CountStatus_MAX                (0x000003FF)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_CountStatus_DEF                (0x00000000)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Spare_OFF                      (25)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Spare_WID                      ( 7)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Spare_MSK                      (0xFE000000)
  #define DDRCMDNCH0_CR_DLLPITESTANDADC_Spare_MAX                      (0x0000007F)

#define DDRCMDNCH1_CR_DDRCRCMDCOMP_REG                               (0x00001500)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Scomp_OFF                         ( 0)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Scomp_WID                         ( 6)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Scomp_MSK                         (0x0000003F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Scomp_MAX                         (0x0000003F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Scomp_DEF                         (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_TcoComp_OFF                       ( 6)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_TcoComp_WID                       ( 6)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_TcoComp_MSK                       (0x00000FC0)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_TcoComp_MAX                       (0x0000003F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_TcoComp_DEF                       (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                    (12)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvUp_WID                    ( 6)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                    (0x0003F000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                    (0x0000003F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                    (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                  (18)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvDown_WID                  ( 6)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                  (0x00FC0000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                  (0x0000003F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                  (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_LsComp_OFF                        (24)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_LsComp_WID                        ( 3)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_LsComp_MSK                        (0x07000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_LsComp_MAX                        (0x00000007)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_LsComp_DEF                        (0x00000004)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Spare_OFF                         (27)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Spare_WID                         ( 5)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Spare_MSK                         (0xF8000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Spare_MAX                         (0x0000001F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMP_Spare_DEF                         (0x00000000)

#define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_REG                         (0x00001504)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF             ( 0)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID             ( 5)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK             (0x0000001F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX             (0x0000001F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF             (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF           ( 5)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID           ( 4)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK           (0x000001E0)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX           (0x0000000F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF           (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF        ( 9)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID        ( 4)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK        (0x00001E00)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX        (0x0000000F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF        (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF      (13)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID      ( 4)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK      (0x0001E000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX      (0x0000000F)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF      (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                   (17)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_Spare_WID                   (15)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                   (0xFFFE0000)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                   (0x00007FFF)
  #define DDRCMDNCH1_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                   (0x00000000)

#define DDRCMDNCH1_CR_DDRCRCMDPICODING_REG                           (0x00001508)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                ( 0)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi0Code_WID                ( 7)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                (0x0000007F)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                (0x0000007F)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                ( 7)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi1Code_WID                ( 7)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                (0x00003F80)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                (0x0000007F)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_Spare_OFF                     (14)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_Spare_WID                     (18)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_Spare_MSK                     (0xFFFFC000)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_Spare_MAX                     (0x0003FFFF)
  #define DDRCMDNCH1_CR_DDRCRCMDPICODING_Spare_DEF                     (0x00000000)

#define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_REG                           (0x0000150C)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RefPi_OFF                     ( 0)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RefPi_WID                     ( 4)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RefPi_MSK                     (0x0000000F)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RefPi_MAX                     (0x0000000F)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RefPi_DEF                     (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllMask_OFF                   ( 4)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllMask_WID                   ( 2)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllMask_MSK                   (0x00000030)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllMask_MAX                   (0x00000003)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllMask_DEF                   (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_OFF                  ( 6)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_WID                  ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_MSK                  (0x00000040)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_MAX                  (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_DEF                  (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_TxOn_OFF                      ( 7)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_TxOn_WID                      ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_TxOn_MSK                      (0x00000080)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_TxOn_MAX                      (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_TxOn_DEF                      (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IntClkOn_OFF                  ( 8)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IntClkOn_WID                  ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IntClkOn_MSK                  (0x00000100)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IntClkOn_MAX                  (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IntClkOn_DEF                  (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RepClkOn_OFF                  ( 9)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RepClkOn_WID                  ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RepClkOn_MSK                  (0x00000200)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RepClkOn_MAX                  (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RepClkOn_DEF                  (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_OFF                   (10)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_WID                   ( 2)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_MSK                   (0x00000C00)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_MAX                   (0x00000003)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_DEF                   (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_OdtMode_OFF                   (12)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_OdtMode_WID                   ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_OdtMode_MSK                   (0x00001000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_OdtMode_MAX                   (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_OdtMode_DEF                   (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_OFF                   (13)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_WID                   ( 2)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_MSK                   (0x00006000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_MAX                   (0x00000003)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_DEF                   (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_OFF            (15)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_WID            ( 2)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MSK            (0x00018000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MAX            (0x00000003)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_DEF            (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_Reserved_OFF                  (17)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_Reserved_WID                  ( 4)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_Reserved_MSK                  (0x001E0000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_Reserved_MAX                  (0x0000000F)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_Reserved_DEF                  (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RxVref_OFF                    (21)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RxVref_WID                    ( 6)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RxVref_MSK                    (0x07E00000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RxVref_MAX                    (0x0000003F)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_RxVref_DEF                    (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_VccddqHi_OFF                  (27)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_VccddqHi_WID                  ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_VccddqHi_MSK                  (0x08000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_VccddqHi_MAX                  (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_VccddqHi_DEF                  (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_OFF               (28)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_WID               ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_MSK               (0x10000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_MAX               (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_DEF               (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_OFF                (29)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_WID                ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MSK                (0x20000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MAX                (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_DEF                (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_OFF              (30)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_WID              ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MSK              (0x40000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MAX              (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_DEF              (0x00000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_OFF              (31)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_WID              ( 1)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MSK              (0x80000000)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MAX              (0x00000001)
  #define DDRCMDNCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_DEF              (0x00000000)

#define DDRCMDNCH1_CR_DLLPITESTANDADC_REG                            (0x00001524)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_RunTest_OFF                    ( 0)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_RunTest_WID                    ( 1)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_RunTest_MSK                    (0x00000001)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_RunTest_MAX                    (0x00000001)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_RunTest_DEF                    (0x00000000)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Load_OFF                       ( 1)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Load_WID                       ( 1)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Load_MSK                       (0x00000002)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Load_MAX                       (0x00000001)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Load_DEF                       (0x00000000)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeHVM_OFF                    ( 2)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeHVM_WID                    ( 1)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeHVM_MSK                    (0x00000004)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeHVM_MAX                    (0x00000001)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeHVM_DEF                    (0x00000000)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeDV_OFF                     ( 3)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeDV_WID                     ( 1)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeDV_MSK                     (0x00000008)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeDV_MAX                     (0x00000001)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeDV_DEF                     (0x00000000)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeADC_OFF                    ( 4)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeADC_WID                    ( 1)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeADC_MSK                    (0x00000010)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeADC_MAX                    (0x00000001)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_ModeADC_DEF                    (0x00000000)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_LoadCount_OFF                  ( 5)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_LoadCount_WID                  (10)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_LoadCount_MSK                  (0x00007FE0)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_LoadCount_MAX                  (0x000003FF)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_CountStatus_OFF                (15)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_CountStatus_WID                (10)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_CountStatus_MSK                (0x01FF8000)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_CountStatus_MAX                (0x000003FF)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_CountStatus_DEF                (0x00000000)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Spare_OFF                      (25)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Spare_WID                      ( 7)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Spare_MSK                      (0xFE000000)
  #define DDRCMDNCH1_CR_DLLPITESTANDADC_Spare_MAX                      (0x0000007F)

#define DDRCMDSCH0_CR_DDRCRCMDCOMP_REG                               (0x00001A00)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Scomp_OFF                         ( 0)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Scomp_WID                         ( 6)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Scomp_MSK                         (0x0000003F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Scomp_MAX                         (0x0000003F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Scomp_DEF                         (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_TcoComp_OFF                       ( 6)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_TcoComp_WID                       ( 6)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_TcoComp_MSK                       (0x00000FC0)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_TcoComp_MAX                       (0x0000003F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_TcoComp_DEF                       (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                    (12)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvUp_WID                    ( 6)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                    (0x0003F000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                    (0x0000003F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                    (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                  (18)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvDown_WID                  ( 6)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                  (0x00FC0000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                  (0x0000003F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                  (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_LsComp_OFF                        (24)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_LsComp_WID                        ( 3)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_LsComp_MSK                        (0x07000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_LsComp_MAX                        (0x00000007)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_LsComp_DEF                        (0x00000004)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Spare_OFF                         (27)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Spare_WID                         ( 5)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Spare_MSK                         (0xF8000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Spare_MAX                         (0x0000001F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMP_Spare_DEF                         (0x00000000)

#define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_REG                         (0x00001A04)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF             ( 0)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID             ( 5)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK             (0x0000001F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX             (0x0000001F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF             (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF           ( 5)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID           ( 4)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK           (0x000001E0)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX           (0x0000000F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF           (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF        ( 9)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID        ( 4)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK        (0x00001E00)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX        (0x0000000F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF        (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF      (13)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID      ( 4)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK      (0x0001E000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX      (0x0000000F)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF      (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                   (17)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_Spare_WID                   (15)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                   (0xFFFE0000)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                   (0x00007FFF)
  #define DDRCMDSCH0_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                   (0x00000000)

#define DDRCMDSCH0_CR_DDRCRCMDPICODING_REG                           (0x00001A08)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                ( 0)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi0Code_WID                ( 7)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                (0x0000007F)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                (0x0000007F)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                ( 7)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi1Code_WID                ( 7)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                (0x00003F80)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                (0x0000007F)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_Spare_OFF                     (14)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_Spare_WID                     (18)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_Spare_MSK                     (0xFFFFC000)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_Spare_MAX                     (0x0003FFFF)
  #define DDRCMDSCH0_CR_DDRCRCMDPICODING_Spare_DEF                     (0x00000000)

#define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_REG                           (0x00001A0C)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RefPi_OFF                     ( 0)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RefPi_WID                     ( 4)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RefPi_MSK                     (0x0000000F)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RefPi_MAX                     (0x0000000F)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RefPi_DEF                     (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllMask_OFF                   ( 4)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllMask_WID                   ( 2)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllMask_MSK                   (0x00000030)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllMask_MAX                   (0x00000003)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllMask_DEF                   (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_OFF                  ( 6)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_WID                  ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_MSK                  (0x00000040)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_MAX                  (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_DEF                  (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_TxOn_OFF                      ( 7)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_TxOn_WID                      ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_TxOn_MSK                      (0x00000080)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_TxOn_MAX                      (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_TxOn_DEF                      (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IntClkOn_OFF                  ( 8)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IntClkOn_WID                  ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IntClkOn_MSK                  (0x00000100)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IntClkOn_MAX                  (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IntClkOn_DEF                  (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RepClkOn_OFF                  ( 9)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RepClkOn_WID                  ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RepClkOn_MSK                  (0x00000200)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RepClkOn_MAX                  (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RepClkOn_DEF                  (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_OFF                   (10)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_WID                   ( 2)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_MSK                   (0x00000C00)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_MAX                   (0x00000003)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_DEF                   (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_OdtMode_OFF                   (12)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_OdtMode_WID                   ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_OdtMode_MSK                   (0x00001000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_OdtMode_MAX                   (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_OdtMode_DEF                   (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_OFF                   (13)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_WID                   ( 2)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_MSK                   (0x00006000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_MAX                   (0x00000003)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_DEF                   (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_OFF            (15)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_WID            ( 2)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MSK            (0x00018000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MAX            (0x00000003)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_DEF            (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_Reserved_OFF                  (17)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_Reserved_WID                  ( 4)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_Reserved_MSK                  (0x001E0000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_Reserved_MAX                  (0x0000000F)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_Reserved_DEF                  (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RxVref_OFF                    (21)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RxVref_WID                    ( 6)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RxVref_MSK                    (0x07E00000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RxVref_MAX                    (0x0000003F)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_RxVref_DEF                    (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_VccddqHi_OFF                  (27)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_VccddqHi_WID                  ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_VccddqHi_MSK                  (0x08000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_VccddqHi_MAX                  (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_VccddqHi_DEF                  (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_OFF               (28)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_WID               ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_MSK               (0x10000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_MAX               (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_DEF               (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_OFF                (29)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_WID                ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MSK                (0x20000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MAX                (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_DEF                (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_OFF              (30)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_WID              ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MSK              (0x40000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MAX              (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_DEF              (0x00000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_OFF              (31)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_WID              ( 1)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MSK              (0x80000000)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MAX              (0x00000001)
  #define DDRCMDSCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_DEF              (0x00000000)

#define DDRCMDSCH0_CR_DLLPITESTANDADC_REG                            (0x00001A24)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_RunTest_OFF                    ( 0)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_RunTest_WID                    ( 1)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_RunTest_MSK                    (0x00000001)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_RunTest_MAX                    (0x00000001)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_RunTest_DEF                    (0x00000000)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Load_OFF                       ( 1)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Load_WID                       ( 1)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Load_MSK                       (0x00000002)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Load_MAX                       (0x00000001)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Load_DEF                       (0x00000000)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeHVM_OFF                    ( 2)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeHVM_WID                    ( 1)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeHVM_MSK                    (0x00000004)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeHVM_MAX                    (0x00000001)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeHVM_DEF                    (0x00000000)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeDV_OFF                     ( 3)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeDV_WID                     ( 1)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeDV_MSK                     (0x00000008)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeDV_MAX                     (0x00000001)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeDV_DEF                     (0x00000000)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeADC_OFF                    ( 4)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeADC_WID                    ( 1)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeADC_MSK                    (0x00000010)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeADC_MAX                    (0x00000001)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_ModeADC_DEF                    (0x00000000)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_LoadCount_OFF                  ( 5)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_LoadCount_WID                  (10)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_LoadCount_MSK                  (0x00007FE0)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_LoadCount_MAX                  (0x000003FF)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_CountStatus_OFF                (15)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_CountStatus_WID                (10)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_CountStatus_MSK                (0x01FF8000)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_CountStatus_MAX                (0x000003FF)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_CountStatus_DEF                (0x00000000)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Spare_OFF                      (25)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Spare_WID                      ( 7)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Spare_MSK                      (0xFE000000)
  #define DDRCMDSCH0_CR_DLLPITESTANDADC_Spare_MAX                      (0x0000007F)

#define DDRCMDSCH1_CR_DDRCRCMDCOMP_REG                               (0x00001B00)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Scomp_OFF                         ( 0)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Scomp_WID                         ( 6)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Scomp_MSK                         (0x0000003F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Scomp_MAX                         (0x0000003F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Scomp_DEF                         (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_TcoComp_OFF                       ( 6)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_TcoComp_WID                       ( 6)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_TcoComp_MSK                       (0x00000FC0)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_TcoComp_MAX                       (0x0000003F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_TcoComp_DEF                       (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                    (12)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvUp_WID                    ( 6)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                    (0x0003F000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                    (0x0000003F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                    (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                  (18)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvDown_WID                  ( 6)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                  (0x00FC0000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                  (0x0000003F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                  (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_LsComp_OFF                        (24)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_LsComp_WID                        ( 3)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_LsComp_MSK                        (0x07000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_LsComp_MAX                        (0x00000007)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_LsComp_DEF                        (0x00000004)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Spare_OFF                         (27)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Spare_WID                         ( 5)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Spare_MSK                         (0xF8000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Spare_MAX                         (0x0000001F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMP_Spare_DEF                         (0x00000000)

#define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_REG                         (0x00001B04)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF             ( 0)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID             ( 5)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK             (0x0000001F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX             (0x0000001F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF             (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF           ( 5)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID           ( 4)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK           (0x000001E0)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX           (0x0000000F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF           (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF        ( 9)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID        ( 4)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK        (0x00001E00)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX        (0x0000000F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF        (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF      (13)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID      ( 4)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK      (0x0001E000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX      (0x0000000F)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF      (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                   (17)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_Spare_WID                   (15)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                   (0xFFFE0000)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                   (0x00007FFF)
  #define DDRCMDSCH1_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                   (0x00000000)

#define DDRCMDSCH1_CR_DDRCRCMDPICODING_REG                           (0x00001B08)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                ( 0)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi0Code_WID                ( 7)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                (0x0000007F)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                (0x0000007F)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                ( 7)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi1Code_WID                ( 7)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                (0x00003F80)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                (0x0000007F)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_Spare_OFF                     (14)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_Spare_WID                     (18)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_Spare_MSK                     (0xFFFFC000)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_Spare_MAX                     (0x0003FFFF)
  #define DDRCMDSCH1_CR_DDRCRCMDPICODING_Spare_DEF                     (0x00000000)

#define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_REG                           (0x00001B0C)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RefPi_OFF                     ( 0)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RefPi_WID                     ( 4)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RefPi_MSK                     (0x0000000F)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RefPi_MAX                     (0x0000000F)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RefPi_DEF                     (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllMask_OFF                   ( 4)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllMask_WID                   ( 2)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllMask_MSK                   (0x00000030)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllMask_MAX                   (0x00000003)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllMask_DEF                   (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_OFF                  ( 6)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_WID                  ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_MSK                  (0x00000040)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_MAX                  (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_DEF                  (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_TxOn_OFF                      ( 7)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_TxOn_WID                      ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_TxOn_MSK                      (0x00000080)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_TxOn_MAX                      (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_TxOn_DEF                      (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IntClkOn_OFF                  ( 8)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IntClkOn_WID                  ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IntClkOn_MSK                  (0x00000100)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IntClkOn_MAX                  (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IntClkOn_DEF                  (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RepClkOn_OFF                  ( 9)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RepClkOn_WID                  ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RepClkOn_MSK                  (0x00000200)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RepClkOn_MAX                  (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RepClkOn_DEF                  (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_OFF                   (10)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_WID                   ( 2)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_MSK                   (0x00000C00)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_MAX                   (0x00000003)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_DEF                   (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_OdtMode_OFF                   (12)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_OdtMode_WID                   ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_OdtMode_MSK                   (0x00001000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_OdtMode_MAX                   (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_OdtMode_DEF                   (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_OFF                   (13)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_WID                   ( 2)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_MSK                   (0x00006000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_MAX                   (0x00000003)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_DEF                   (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_OFF            (15)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_WID            ( 2)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MSK            (0x00018000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MAX            (0x00000003)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_DEF            (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_Reserved_OFF                  (17)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_Reserved_WID                  ( 4)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_Reserved_MSK                  (0x001E0000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_Reserved_MAX                  (0x0000000F)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_Reserved_DEF                  (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RxVref_OFF                    (21)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RxVref_WID                    ( 6)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RxVref_MSK                    (0x07E00000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RxVref_MAX                    (0x0000003F)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_RxVref_DEF                    (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_VccddqHi_OFF                  (27)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_VccddqHi_WID                  ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_VccddqHi_MSK                  (0x08000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_VccddqHi_MAX                  (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_VccddqHi_DEF                  (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_OFF               (28)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_WID               ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_MSK               (0x10000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_MAX               (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_DEF               (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_OFF                (29)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_WID                ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MSK                (0x20000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MAX                (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_DEF                (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_OFF              (30)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_WID              ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MSK              (0x40000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MAX              (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_DEF              (0x00000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_OFF              (31)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_WID              ( 1)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MSK              (0x80000000)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MAX              (0x00000001)
  #define DDRCMDSCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_DEF              (0x00000000)

#define DDRCMDSCH1_CR_DLLPITESTANDADC_REG                            (0x00001B24)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_RunTest_OFF                    ( 0)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_RunTest_WID                    ( 1)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_RunTest_MSK                    (0x00000001)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_RunTest_MAX                    (0x00000001)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_RunTest_DEF                    (0x00000000)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Load_OFF                       ( 1)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Load_WID                       ( 1)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Load_MSK                       (0x00000002)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Load_MAX                       (0x00000001)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Load_DEF                       (0x00000000)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeHVM_OFF                    ( 2)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeHVM_WID                    ( 1)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeHVM_MSK                    (0x00000004)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeHVM_MAX                    (0x00000001)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeHVM_DEF                    (0x00000000)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeDV_OFF                     ( 3)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeDV_WID                     ( 1)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeDV_MSK                     (0x00000008)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeDV_MAX                     (0x00000001)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeDV_DEF                     (0x00000000)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeADC_OFF                    ( 4)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeADC_WID                    ( 1)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeADC_MSK                    (0x00000010)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeADC_MAX                    (0x00000001)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_ModeADC_DEF                    (0x00000000)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_LoadCount_OFF                  ( 5)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_LoadCount_WID                  (10)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_LoadCount_MSK                  (0x00007FE0)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_LoadCount_MAX                  (0x000003FF)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_CountStatus_OFF                (15)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_CountStatus_WID                (10)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_CountStatus_MSK                (0x01FF8000)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_CountStatus_MAX                (0x000003FF)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_CountStatus_DEF                (0x00000000)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Spare_OFF                      (25)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Spare_WID                      ( 7)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Spare_MSK                      (0xFE000000)
  #define DDRCMDSCH1_CR_DLLPITESTANDADC_Spare_MAX                      (0x0000007F)

#define DDRCMDCH0_CR_DDRCRCMDCOMP_REG                                (0x00003200)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Scomp_OFF                          ( 0)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Scomp_WID                          ( 6)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_TcoComp_OFF                        ( 6)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_TcoComp_WID                        ( 6)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_LsComp_OFF                         (24)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_LsComp_WID                         ( 3)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_LsComp_DEF                         (0x00000004)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Spare_OFF                          (27)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Spare_WID                          ( 5)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMP_Spare_DEF                          (0x00000000)

#define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_REG                          (0x00003204)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_Spare_WID                    (15)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCMDCH0_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCMDCH0_CR_DDRCRCMDPICODING_REG                            (0x00003208)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                 ( 0)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi0Code_WID                 ( 7)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                 (0x0000007F)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                 (0x0000007F)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                 (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                 ( 7)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi1Code_WID                 ( 7)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                 (0x00003F80)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                 (0x0000007F)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                 (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_Spare_OFF                      (14)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_Spare_WID                      (18)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_Spare_MSK                      (0xFFFFC000)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_Spare_MAX                      (0x0003FFFF)
  #define DDRCMDCH0_CR_DDRCRCMDPICODING_Spare_DEF                      (0x00000000)

#define DDRCMDCH0_CR_DDRCRCMDCONTROLS_REG                            (0x0000320C)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RefPi_WID                      ( 4)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllMask_WID                    ( 2)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_TxOn_WID                       ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_OdtMode_OFF                    (12)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_OFF                    (13)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_WID                    ( 2)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_MSK                    (0x00006000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_MAX                    (0x00000003)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_CmdTxEq_DEF                    (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_OFF             (15)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_WID             ( 2)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MSK             (0x00018000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MAX             (0x00000003)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_DEF             (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_Reserved_OFF                   (17)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_Reserved_WID                   ( 4)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_Reserved_MSK                   (0x001E0000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_Reserved_MAX                   (0x0000000F)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_Reserved_DEF                   (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RxVref_OFF                     (21)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RxVref_WID                     ( 6)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_OFF               (30)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_WID               ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MSK               (0x40000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MAX               (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_DEF               (0x00000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_OFF               (31)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_WID               ( 1)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MSK               (0x80000000)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MAX               (0x00000001)
  #define DDRCMDCH0_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_DEF               (0x00000000)

#define DDRCMDCH0_CR_DLLPITESTANDADC_REG                             (0x00003224)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCMDCH0_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#define DDRCMDCH1_CR_DDRCRCMDCOMP_REG                                (0x00003300)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Scomp_OFF                          ( 0)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Scomp_WID                          ( 6)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Scomp_MSK                          (0x0000003F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Scomp_MAX                          (0x0000003F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Scomp_DEF                          (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_TcoComp_OFF                        ( 6)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_TcoComp_WID                        ( 6)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_TcoComp_MSK                        (0x00000FC0)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_TcoComp_MAX                        (0x0000003F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_TcoComp_DEF                        (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvUp_OFF                     (12)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvUp_WID                     ( 6)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvUp_MSK                     (0x0003F000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvUp_MAX                     (0x0000003F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvUp_DEF                     (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvDown_OFF                   (18)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvDown_WID                   ( 6)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvDown_MSK                   (0x00FC0000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvDown_MAX                   (0x0000003F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_RcompDrvDown_DEF                   (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_LsComp_OFF                         (24)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_LsComp_WID                         ( 3)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_LsComp_MSK                         (0x07000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_LsComp_MAX                         (0x00000007)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_LsComp_DEF                         (0x00000004)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Spare_OFF                          (27)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Spare_WID                          ( 5)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Spare_MSK                          (0xF8000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Spare_MAX                          (0x0000001F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMP_Spare_DEF                          (0x00000000)

#define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_REG                          (0x00003304)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_OFF              ( 0)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_WID              ( 5)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MSK              (0x0000001F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_MAX              (0x0000001F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_ScompOffset_DEF              (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_OFF            ( 5)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_WID            ( 4)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MSK            (0x000001E0)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_MAX            (0x0000000F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_TcoCompOffset_DEF            (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_OFF         ( 9)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_WID         ( 4)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MSK         (0x00001E00)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_MAX         (0x0000000F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvUpOffset_DEF         (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_OFF       (13)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_WID       ( 4)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MSK       (0x0001E000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_MAX       (0x0000000F)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_RcompDrvDownOffset_DEF       (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_Spare_OFF                    (17)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_Spare_WID                    (15)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_Spare_MSK                    (0xFFFE0000)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_Spare_MAX                    (0x00007FFF)
  #define DDRCMDCH1_CR_DDRCRCMDCOMPOFFSET_Spare_DEF                    (0x00000000)

#define DDRCMDCH1_CR_DDRCRCMDPICODING_REG                            (0x00003308)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi0Code_OFF                 ( 0)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi0Code_WID                 ( 7)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi0Code_MSK                 (0x0000007F)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi0Code_MAX                 (0x0000007F)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi0Code_DEF                 (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi1Code_OFF                 ( 7)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi1Code_WID                 ( 7)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi1Code_MSK                 (0x00003F80)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi1Code_MAX                 (0x0000007F)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_CmdPi1Code_DEF                 (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_Spare_OFF                      (14)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_Spare_WID                      (18)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_Spare_MSK                      (0xFFFFC000)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_Spare_MAX                      (0x0003FFFF)
  #define DDRCMDCH1_CR_DDRCRCMDPICODING_Spare_DEF                      (0x00000000)

#define DDRCMDCH1_CR_DDRCRCMDCONTROLS_REG                            (0x0000330C)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RefPi_OFF                      ( 0)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RefPi_WID                      ( 4)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RefPi_MSK                      (0x0000000F)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RefPi_MAX                      (0x0000000F)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RefPi_DEF                      (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllMask_OFF                    ( 4)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllMask_WID                    ( 2)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllMask_MSK                    (0x00000030)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllMask_MAX                    (0x00000003)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllMask_DEF                    (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_OFF                   ( 6)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_WID                   ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_MSK                   (0x00000040)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_MAX                   (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllRsvd1_DEF                   (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_TxOn_OFF                       ( 7)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_TxOn_WID                       ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_TxOn_MSK                       (0x00000080)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_TxOn_MAX                       (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_TxOn_DEF                       (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IntClkOn_OFF                   ( 8)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IntClkOn_WID                   ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IntClkOn_MSK                   (0x00000100)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IntClkOn_MAX                   (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IntClkOn_DEF                   (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RepClkOn_OFF                   ( 9)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RepClkOn_WID                   ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RepClkOn_MSK                   (0x00000200)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RepClkOn_MAX                   (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RepClkOn_DEF                   (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_OFF                    (10)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_WID                    ( 2)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_MSK                    (0x00000C00)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_MAX                    (0x00000003)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_IOLBCtl_DEF                    (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_OdtMode_OFF                    (12)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_OdtMode_WID                    ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_OdtMode_MSK                    (0x00001000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_OdtMode_MAX                    (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_OdtMode_DEF                    (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_OFF                    (13)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_WID                    ( 2)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_MSK                    (0x00006000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_MAX                    (0x00000003)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_CmdTxEq_DEF                    (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_OFF             (15)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_WID             ( 2)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MSK             (0x00018000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_MAX             (0x00000003)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_EarlyWeakDrive_DEF             (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_Reserved_OFF                   (17)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_Reserved_WID                   ( 4)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_Reserved_MSK                   (0x001E0000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_Reserved_MAX                   (0x0000000F)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_Reserved_DEF                   (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RxVref_OFF                     (21)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RxVref_WID                     ( 6)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RxVref_MSK                     (0x07E00000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RxVref_MAX                     (0x0000003F)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_RxVref_DEF                     (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_VccddqHi_OFF                   (27)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_VccddqHi_WID                   ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_VccddqHi_MSK                   (0x08000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_VccddqHi_MAX                   (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_VccddqHi_DEF                   (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_OFF                (28)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_WID                ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_MSK                (0x10000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_MAX                (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_DllWeakLock_DEF                (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_OFF                 (29)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_WID                 ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MSK                 (0x20000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_MAX                 (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDDR_Mode_DEF                 (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_OFF               (30)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_WID               ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MSK               (0x40000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_MAX               (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAA_Dis_DEF               (0x00000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_OFF               (31)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_WID               ( 1)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MSK               (0x80000000)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_MAX               (0x00000001)
  #define DDRCMDCH1_CR_DDRCRCMDCONTROLS_LPDdrCAB_Dis_DEF               (0x00000000)

#define DDRCMDCH1_CR_DLLPITESTANDADC_REG                             (0x00003324)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_RunTest_OFF                     ( 0)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_RunTest_WID                     ( 1)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_RunTest_MSK                     (0x00000001)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_RunTest_MAX                     (0x00000001)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_RunTest_DEF                     (0x00000000)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Load_OFF                        ( 1)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Load_WID                        ( 1)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Load_MSK                        (0x00000002)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Load_MAX                        (0x00000001)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Load_DEF                        (0x00000000)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeHVM_OFF                     ( 2)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeHVM_WID                     ( 1)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeHVM_MSK                     (0x00000004)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeHVM_MAX                     (0x00000001)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeHVM_DEF                     (0x00000000)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeDV_OFF                      ( 3)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeDV_WID                      ( 1)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeDV_MSK                      (0x00000008)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeDV_MAX                      (0x00000001)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeDV_DEF                      (0x00000000)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeADC_OFF                     ( 4)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeADC_WID                     ( 1)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeADC_MSK                     (0x00000010)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeADC_MAX                     (0x00000001)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_ModeADC_DEF                     (0x00000000)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_LoadCount_OFF                   ( 5)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_LoadCount_WID                   (10)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_LoadCount_MSK                   (0x00007FE0)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_LoadCount_MAX                   (0x000003FF)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_CountStatus_OFF                 (15)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_CountStatus_WID                 (10)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_CountStatus_MSK                 (0x01FF8000)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_CountStatus_MAX                 (0x000003FF)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_CountStatus_DEF                 (0x00000000)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Spare_OFF                       (25)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Spare_WID                       ( 7)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Spare_MSK                       (0xFE000000)
  #define DDRCMDCH1_CR_DLLPITESTANDADC_Spare_MAX                       (0x0000007F)

#pragma pack(pop)
#endif  // __McIoCmd_h__
