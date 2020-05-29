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

#ifndef _MSA_H_
#define _MSA_H_

#pragma pack(push, 1)
#include <MrcTypes.h>

typedef union {
  struct {
    U32 GFXVTBAREN                              :  1;  // Bits 0:0
    U32                                         :  11;  // Bits 11:1
    U32 GFXVTBAR                                :  27;  // Bits 38:12
    U32                                         :  25;  // Bits 63:39
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} NCDECS_CR_GFXVTBAR_NCU_STRUCT;

typedef union {
  struct {
    U32 EDRAMBAREN                              :  1;  // Bits 0:0
    U32                                         :  13;  // Bits 13:1
    U32 EDRAMBAR                                :  25;  // Bits 38:14
    U32                                         :  25;  // Bits 63:39
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} NCDECS_CR_EDRAMBAR_NCU_STRUCT;

typedef union {
  struct {
    U32 VTVC0BAREN                              :  1;  // Bits 0:0
    U32                                         :  11;  // Bits 11:1
    U32 VTVC0BAR                                :  27;  // Bits 38:12
    U32                                         :  25;  // Bits 63:39
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} NCDECS_CR_VTDPVC0BAR_NCU_STRUCT;

typedef union {
  struct {
    U32 RdrModSel                               :  3;  // Bits 2:0
    U32 ClastChkSmpMod                          :  1;  // Bits 3:3
    U32 LogFltClustMod                          :  1;  // Bits 4:4
    U32 LogFlatClustOvrEn                       :  1;  // Bits 5:5
    U32 HashModCtr                              :  3;  // Bits 8:6
    U32                                         :  23;  // Bits 31:9
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} NCDECS_CR_INTRDIRCTL_NCU_STRUCT;

typedef union {
  struct {
    U32                                         :  28;  // Bits 27:0
    U32 PLIM                                    :  3;  // Bits 30:28
    U32                                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} NCDECS_CR_NCUCTL0_NCU_STRUCT;

typedef union {
  struct {
    U32 GDXCBAREN                               :  1;  // Bits 0:0
    U32                                         :  11;  // Bits 11:1
    U32 GDXCBAR                                 :  27;  // Bits 38:12
    U32                                         :  25;  // Bits 63:39
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} NCDECS_CR_GDXCBAR_NCU_STRUCT;

typedef union {
  struct {
    U32 Data                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} NCDECS_CR_SCRATCHPAD_NCU_0_STRUCT;

typedef union {
  struct {
    U32 Data                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} NCDECS_CR_SCRATCHPAD_NCU_1_STRUCT;

typedef union {
  struct {
    U32 Data                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} NCDECS_CR_SCRATCHPAD_NCU_2_STRUCT;

typedef union {
  struct {
    U32 Data                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} NCDECS_CR_SCRATCHPAD_NCU_3_STRUCT;

typedef union {
  struct {
    U32 LOCK                                    :  1;  // Bits 0:0
    U32                                         :  19;  // Bits 19:1
    U32 OFFSET                                  :  12;  // Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} NCDECS_CR_PAVPMSGOFFST_NCU_STRUCT;

typedef union {
  struct {
    U32 VCPVTDLIM                               :  3;  // Bits 2:0
    U32                                         :  29;  // Bits 31:3
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_VTDLIM_IMPH_STRUCT;

typedef union {
  struct {
    U32                                         :  3;  // Bits 2:0
    U32 DEVNUM                                  :  5;  // Bits 7:3
    U32 BUSNUM                                  :  8;  // Bits 15:8
    U32                                         :  15;  // Bits 30:16
    U32 HDAUD_EN                                :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_HDAUDRID_IMPH_STRUCT;

typedef union {
  struct {
    U32                                         :  20;  // Bits 19:0
    U32 UMAB                                    :  19;  // Bits 38:20
    U32                                         :  25;  // Bits 63:39
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} MPVTDTRK_CR_UMAGFXBASE_IMPH_STRUCT;

typedef union {
  struct {
    U32                                         :  20;  // Bits 19:0
    U32 UMAL                                    :  19;  // Bits 38:20
    U32                                         :  25;  // Bits 63:39
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} MPVTDTRK_CR_UMAGFXLIMIT_IMPH_STRUCT;

typedef union {
  struct {
    U32 LCK                                     :  1;  // Bits 0:0
    U32                                         :  9;  // Bits 9:1
    U32 PEG10EN                                 :  1;  // Bits 10:10
    U32 PEG11EN                                 :  1;  // Bits 11:11
    U32 PEG12EN                                 :  1;  // Bits 12:12
    U32 DMIEN                                   :  1;  // Bits 13:13
    U32                                         :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_UMAGFXCTL_IMPH_STRUCT;

typedef union {
  struct {
    U32 FUNNUM                                  :  3;  // Bits 2:0
    U32 DEVNUM                                  :  5;  // Bits 7:3
    U32 BUSNUM                                  :  8;  // Bits 15:8
    U32 BARNUM                                  :  3;  // Bits 18:16
    U32                                         :  13;  // Bits 31:19
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_VDMBDFBARKVM_IMPH_STRUCT;

typedef union {
  struct {
    U32 FUNNUM                                  :  3;  // Bits 2:0
    U32 DEVNUM                                  :  5;  // Bits 7:3
    U32 BUSNUM                                  :  8;  // Bits 15:8
    U32 BARNUM                                  :  3;  // Bits 18:16
    U32                                         :  13;  // Bits 31:19
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_VISA_XBAR_MPVTDTLBS_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_STRUCT;

typedef union {
  struct {
    U32 VC                                      :  4;  // Bits 3:0
    U32 FMT_CMDTYPE                             :  6;  // Bits 9:4
    U32 TC                                      :  4;  // Bits 13:10
    U32 CHAIN                                   :  1;  // Bits 14:14
    U32 NS                                      :  1;  // Bits 15:15
    U32 RO                                      :  1;  // Bits 16:16
    U32 LENGTH                                  :  5;  // Bits 21:17
    U32 EP                                      :  1;  // Bits 22:22
    U32 AT                                      :  2;  // Bits 24:23
    U32                                         :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_MASK1_IMPH_STRUCT;

typedef union {
  struct {
    U32 RQID                                    :  16;  // Bits 15:0
    U32 TAG                                     :  8;  // Bits 23:16
    U32 LBEFBE_MSGTYPE                          :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_MASK2_IMPH_STRUCT;

typedef union {
  struct {
    U32 ADDR_31_0                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_MASK3_IMPH_STRUCT;

typedef union {
  struct {
    U32 ADDR_63_32                              :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_MASK4_IMPH_STRUCT;

typedef union {
  struct {
    U32 VC                                      :  4;  // Bits 3:0
    U32 FMT_CMDTYPE                             :  6;  // Bits 9:4
    U32 TC                                      :  4;  // Bits 13:10
    U32 CHAIN                                   :  1;  // Bits 14:14
    U32 NS                                      :  1;  // Bits 15:15
    U32 RO                                      :  1;  // Bits 16:16
    U32 LENGTH                                  :  5;  // Bits 21:17
    U32 EP                                      :  1;  // Bits 22:22
    U32 AT                                      :  2;  // Bits 24:23
    U32                                         :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_COMP1_IMPH_STRUCT;

typedef union {
  struct {
    U32 RQID                                    :  16;  // Bits 15:0
    U32 TAG                                     :  8;  // Bits 23:16
    U32 LBEFBE_MSGTYPE                          :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_COMP2_IMPH_STRUCT;

typedef union {
  struct {
    U32 ADDR_31_0                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_COMP3_IMPH_STRUCT;

typedef union {
  struct {
    U32 ADDR_63_32                              :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_COMP4_IMPH_STRUCT;

typedef union {
  struct {
    U32 ENABLE                                  :  1;  // Bits 0:0
    U32 TRIGGERED                               :  1;  // Bits 1:1
    U32 STALL_DNARB                             :  1;  // Bits 2:2
    U32 STALL_UPARB                             :  1;  // Bits 3:3
    U32 STALL_SNPARB                            :  1;  // Bits 4:4
    U32                                         :  18;  // Bits 22:5
    U32 STALL_DELAY                             :  9;  // Bits 31:23
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STRUCT;

typedef union {
  struct {
    U32 PEG2DMIDIS                              :  1;  // Bits 0:0
    U32 EOIB                                    :  1;  // Bits 1:1
    U32 MSIBYPDIS                               :  1;  // Bits 2:2
    U32                                         :  1;  // Bits 3:3
    U32 PHLDDIS                                 :  1;  // Bits 4:4
    U32                                         :  1;  // Bits 5:5
    U32 BKSNPDIS                                :  1;  // Bits 6:6
    U32 FRCVC0SNP                               :  1;  // Bits 7:7
    U32 FRCVCPSNP                               :  1;  // Bits 8:8
    U32 PHLDBLKDIS                              :  1;  // Bits 9:9
    U32 BLKWRPOSTVC1                            :  1;  // Bits 10:10
    U32 DIS_VLW_PEG                             :  1;  // Bits 11:11
    U32 SPECRDDIS                               :  1;  // Bits 12:12
    U32 IR_RSRV_CTL                             :  1;  // Bits 13:13
    U32 RSVD                                    :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_HCTL0_IMPH_STRUCT;

typedef union {
  struct {
    U32                                         :  4;  // Bits 3:0
    U64 REGBAR                                  :  35;  // Bits 38:4
    U32                                         :  25;  // Bits 63:39
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} MPVTDTRK_CR_REGBAR_IMPH_STRUCT;

typedef union {
  struct {
    U32 THERMAL_MONITOR_STATUS                  :  1;  // Bits 0:0
    U32 THERMAL_MONITOR_LOG                     :  1;  // Bits 1:1
    U32 PROCHOT_STATUS                          :  1;  // Bits 2:2
    U32 PROCHOT_LOG                             :  1;  // Bits 3:3
    U32 OUT_OF_SPEC_STATUS                      :  1;  // Bits 4:4
    U32 OUT_OF_SPEC_LOG                         :  1;  // Bits 5:5
    U32 THRESHOLD1_STATUS                       :  1;  // Bits 6:6
    U32 THRESHOLD1_LOG                          :  1;  // Bits 7:7
    U32 THRESHOLD2_STATUS                       :  1;  // Bits 8:8
    U32 THRESHOLD2_LOG                          :  1;  // Bits 9:9
    U32 POWER_LIMITATION_STATUS                 :  1;  // Bits 10:10
    U32 POWER_LIMITATION_LOG                    :  1;  // Bits 11:11
    U32                                         :  4;  // Bits 15:12
    U32 Temperature                             :  7;  // Bits 22:16
    U32                                         :  4;  // Bits 26:23
    U32 Resolution                              :  4;  // Bits 30:27
    U32 Valid                                   :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_STRUCT;

typedef union {
  struct {
    U32 WARM_THRESHOLD_STATUS                   :  1;  // Bits 0:0
    U32 WARM_THRESHOLD_LOG                      :  1;  // Bits 1:1
    U32 HOT_THRESHOLD_STATUS                    :  1;  // Bits 2:2
    U32 HOT_THRESHOLD_LOG                       :  1;  // Bits 3:3
    U32 REFRESH2X_STATUS                        :  1;  // Bits 4:4
    U32 REFRESH2X_LOG                           :  1;  // Bits 5:5
    U32 FORCEMEMPR_STATUS                       :  1;  // Bits 6:6
    U32 FORCEMEMPR_LOG                          :  1;  // Bits 7:7
    U32 THRESHOLD1_STATUS                       :  1;  // Bits 8:8
    U32 THRESHOLD1_LOG                          :  1;  // Bits 9:9
    U32 THRESHOLD2_STATUS                       :  1;  // Bits 10:10
    U32 THRESHOLD2_LOG                          :  1;  // Bits 11:11
    U32                                         :  20;  // Bits 31:12
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_STRUCT;

typedef union {
  struct {
    U32 LOCK                                    :  1;  // Bits 0:0
    U32                                         :  31;  // Bits 31:1
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPVTDTRK_CR_VTDTRKLCK_IMPH_STRUCT;

typedef union {
  struct {
    U32 RDLIM                                   :  3;  // Bits 2:0
    U32                                         :  1;  // Bits 3:3
    U32 WRLIM                                   :  3;  // Bits 6:4
    U32                                         :  24;  // Bits 30:7
    U32 LOCK                                    :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPCBOTRK_CR_REQLIM_IMPH_STRUCT;

typedef union {
  struct {
    U32 VCPNPLIM                                :  3;  // Bits 2:0
    U32                                         :  1;  // Bits 3:3
    U32 VCPPLIM                                 :  3;  // Bits 6:4
    U32                                         :  1;  // Bits 7:7
    U32 VCMNPLIM                                :  3;  // Bits 10:8
    U32                                         :  1;  // Bits 11:11
    U32 VCMPLIM                                 :  3;  // Bits 14:12
    U32                                         :  1;  // Bits 15:15
    U32 VCPCMPLIM                               :  3;  // Bits 18:16
    U32                                         :  1;  // Bits 19:19
    U32 VCMCMPLIM                               :  3;  // Bits 22:20
    U32                                         :  8;  // Bits 30:23
    U32 LOCK                                    :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_DMIVCLIM_HUBS_STRUCT;

typedef union {
  struct {
    U32 P0                                      :  1;  // Bits 0:0
    U32 P1                                      :  1;  // Bits 1:1
    U32 P2                                      :  1;  // Bits 2:2
    U32 P3                                      :  1;  // Bits 3:3
    U32                                         :  28;  // Bits 31:4
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB0_PWRDN_OVRD_HUBS_STRUCT;

typedef union {
  struct {
    U32 P0                                      :  1;  // Bits 0:0
    U32 P1                                      :  1;  // Bits 1:1
    U32 P2                                      :  1;  // Bits 2:2
    U32 P3                                      :  1;  // Bits 3:3
    U32                                         :  28;  // Bits 31:4
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB1_PWRDN_OVRD_HUBS_STRUCT;

typedef union {
  struct {
    U32 P0                                      :  1;  // Bits 0:0
    U32 P1                                      :  1;  // Bits 1:1
    U32 P2                                      :  1;  // Bits 2:2
    U32 P3                                      :  1;  // Bits 3:3
    U32                                         :  28;  // Bits 31:4
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB2_PWRDN_OVRD_HUBS_STRUCT;

typedef union {
  struct {
    U32 BLK_GNT_P0                              :  1;  // Bits 0:0
    U32 BLK_GNT_P1                              :  1;  // Bits 1:1
    U32 BLK_GNT_P2                              :  1;  // Bits 2:2
    U32 BLK_PUT_P0                              :  1;  // Bits 3:3
    U32 BLK_PUT_P1                              :  1;  // Bits 4:4
    U32 BLK_PUT_P2                              :  1;  // Bits 5:5
    U32 BLK_PUT_P3                              :  1;  // Bits 6:6
    U32 NO_CHAIN_P0                             :  1;  // Bits 7:7
    U32 NO_CHAIN_P1                             :  1;  // Bits 8:8
    U32 NO_CHAIN_P2                             :  1;  // Bits 9:9
    U32 SLOW_UP_P0                              :  1;  // Bits 10:10
    U32 SLOW_UP_P1                              :  1;  // Bits 11:11
    U32 SLOW_UP_P2                              :  1;  // Bits 12:12
    U32 SLOW_DN_P0                              :  1;  // Bits 13:13
    U32 SLOW_DN_P1                              :  1;  // Bits 14:14
    U32 SLOW_DN_P2                              :  1;  // Bits 15:15
    U32 SLOW_DN_P3                              :  1;  // Bits 16:16
    U32 SLOWER_CMD                              :  1;  // Bits 17:17
    U32 DMI_NOPUSH                              :  1;  // Bits 18:18
    U32 RO_PASS_NP                              :  1;  // Bits 19:19
    U32 RST_CRD_P3                              :  1;  // Bits 20:20
    U32                                         :  11;  // Bits 31:21
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB0_DEFEATURE_HUBS_STRUCT;

typedef union {
  struct {
    U32 BLK_GNT_P1                              :  1;  // Bits 0:0
    U32 BLK_GNT_P2                              :  1;  // Bits 1:1
    U32 BLK_PUT_P0                              :  1;  // Bits 2:2
    U32 BLK_PUT_P1                              :  1;  // Bits 3:3
    U32 BLK_PUT_P2                              :  1;  // Bits 4:4
    U32 BLK_PUT_P3                              :  1;  // Bits 5:5
    U32 NO_CHAIN_P1                             :  1;  // Bits 6:6
    U32 NO_CHAIN_P2                             :  1;  // Bits 7:7
    U32 SLOW_UP_P0                              :  1;  // Bits 8:8
    U32 SLOW_UP_P1                              :  1;  // Bits 9:9
    U32 SLOW_UP_P2                              :  1;  // Bits 10:10
    U32 SLOW_DN_P1                              :  1;  // Bits 11:11
    U32 SLOW_DN_P2                              :  1;  // Bits 12:12
    U32 SLOW_DN_P3                              :  1;  // Bits 13:13
    U32 SLOWER_CMD                              :  1;  // Bits 14:14
    U32 RO_PASS_NP                              :  1;  // Bits 15:15
    U32 RST_CRD_P0                              :  1;  // Bits 16:16
    U32 RST_CRD_P3                              :  1;  // Bits 17:17
    U32                                         :  14;  // Bits 31:18
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB1_DEFEATURE_HUBS_STRUCT;

typedef union {
  struct {
    U32 BLK_GNT_P1                              :  1;  // Bits 0:0
    U32 BLK_GNT_P2                              :  1;  // Bits 1:1
    U32 BLK_PUT_P0                              :  1;  // Bits 2:2
    U32 BLK_PUT_P1                              :  1;  // Bits 3:3
    U32 BLK_PUT_P2                              :  1;  // Bits 4:4
    U32 NO_CHAIN_P1                             :  1;  // Bits 5:5
    U32 NO_CHAIN_P2                             :  1;  // Bits 6:6
    U32 SLOW_UP_P0                              :  1;  // Bits 7:7
    U32 SLOW_UP_P1                              :  1;  // Bits 8:8
    U32 SLOW_UP_P2                              :  1;  // Bits 9:9
    U32 SLOW_DN_P1                              :  1;  // Bits 10:10
    U32 SLOW_DN_P2                              :  1;  // Bits 11:11
    U32 SLOWER_CMD                              :  1;  // Bits 12:12
    U32 RST_CRD_P0                              :  1;  // Bits 13:13
    U32                                         :  18;  // Bits 31:14
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB2_DEFEATURE_HUBS_STRUCT;

typedef union {
  struct {
    U32 PCIPWRGAT                               :  1;  // Bits 0:0
    U32                                         :  31;  // Bits 31:1
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_PEGCTL_HUBS_STRUCT;

typedef union {
  struct {
    U32 H0_EMPTY                                :  1;  // Bits 0:0
    U32 H1_EMPTY                                :  1;  // Bits 1:1
    U32 H2_EMPTY                                :  1;  // Bits 2:2
    U32                                         :  29;  // Bits 31:3
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB_EMPTY_HUBS_STRUCT;

typedef union {
  struct {
    U32 REQ_VLD_P0                              :  1;  // Bits 0:0
    U32 REQ_VLD_P1                              :  1;  // Bits 1:1
    U32 REQ_VLD_P2                              :  1;  // Bits 2:2
    U32 REQ_VLD_P3                              :  1;  // Bits 3:3
    U32 TNX_VLD_P0                              :  1;  // Bits 4:4
    U32 TNX_VLD_P1                              :  1;  // Bits 5:5
    U32 TNX_VLD_P2                              :  1;  // Bits 6:6
    U32 TNX_VLD_P3                              :  1;  // Bits 7:7
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB0_STATUS_HUBS_STRUCT;

typedef union {
  struct {
    U32 REQ_VLD_P0                              :  1;  // Bits 0:0
    U32 REQ_VLD_P1                              :  1;  // Bits 1:1
    U32 REQ_VLD_P2                              :  1;  // Bits 2:2
    U32 REQ_VLD_P3                              :  1;  // Bits 3:3
    U32 TNX_VLD_P0                              :  1;  // Bits 4:4
    U32 TNX_VLD_P1                              :  1;  // Bits 5:5
    U32 TNX_VLD_P2                              :  1;  // Bits 6:6
    U32 TNX_VLD_P3                              :  1;  // Bits 7:7
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB1_STATUS_HUBS_STRUCT;

typedef union {
  struct {
    U32 REQ_VLD_P0                              :  1;  // Bits 0:0
    U32 REQ_VLD_P1                              :  1;  // Bits 1:1
    U32 REQ_VLD_P2                              :  1;  // Bits 2:2
    U32 REQ_VLD_P3                              :  1;  // Bits 3:3
    U32 TNX_VLD_P0                              :  1;  // Bits 4:4
    U32 TNX_VLD_P1                              :  1;  // Bits 5:5
    U32 TNX_VLD_P2                              :  1;  // Bits 6:6
    U32 TNX_VLD_P3                              :  1;  // Bits 7:7
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB2_STATUS_HUBS_STRUCT;

typedef union {
  struct {
    U32 Data                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_VISA_CTL_SABHUB0S_HUBS_STRUCT;

typedef union {
  struct {
    U32 Data                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_VISA_CTL_SABHUB1S_HUBS_STRUCT;

typedef union {
  struct {
    U32 Data                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_VISA_CTL_SABHUB2S_HUBS_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_MBOX_WR_DATA_HUBS_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_MBOX_RD_DATA_HUBS_STRUCT;

typedef union {
  struct {
    U32 ADDR                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_MBOX_ADDR_LO_HUBS_STRUCT;

typedef union {
  struct {
    U32 ADDR                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_MBOX_ADDR_HI_HUBS_STRUCT;

typedef union {
  struct {
    U32 FBE                                     :  4;  // Bits 3:0
    U32 LBE                                     :  4;  // Bits 7:4
    U32 TAG                                     :  8;  // Bits 15:8
    U32 RQID                                    :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_MBOX_CMD_LO_HUBS_STRUCT;

typedef union {
  struct {
    U32 AT                                      :  2;  // Bits 1:0
    U32 POISON                                  :  1;  // Bits 2:2
    U32 LENGTH                                  :  5;  // Bits 7:3
    U32 RELAXED                                 :  1;  // Bits 8:8
    U32 NOSNOOP                                 :  1;  // Bits 9:9
    U32 CHAIN                                   :  1;  // Bits 10:10
    U32 CTYPE                                   :  5;  // Bits 15:11
    U32 FMT                                     :  2;  // Bits 17:16
    U32 TC                                      :  4;  // Bits 21:18
    U32 RESERVED                                :  2;  // Bits 23:22
    U32 DMI_PRIV                                :  1;  // Bits 24:24
    U32 CHID                                    :  4;  // Bits 28:25
    U32 RTYPE                                   :  2;  // Bits 30:29
    U32 START                                   :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_MBOX_CMD_HI_HUBS_STRUCT;

typedef union {
  struct {
    U32 ENABLE                                  :  1;  // Bits 0:0
    U32 LOCK_IA                                 :  1;  // Bits 1:1
    U32 RESET                                   :  1;  // Bits 2:2
    U32 BLK_CYCLES                              :  6;  // Bits 8:3
    U32 WR_DWORD_SEL                            :  4;  // Bits 12:9
    U32 RD_DWORD_SEL                            :  4;  // Bits 16:13
    U32 RPT_CMD_CNT                             :  6;  // Bits 22:17
    U32 RPT_NXT_ADDR                            :  1;  // Bits 23:23
    U32 RPT_NXT_PAGE                            :  1;  // Bits 24:24
    U32 DIS_CMP_INV                             :  1;  // Bits 25:25
    U32 FSM_STATE                               :  4;  // Bits 29:26
    U32 P2P_ALL                                 :  1;  // Bits 30:30
    U32 SPARE                                   :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_MBOX_CFG_HUBS_STRUCT;

typedef union {
  struct {
    U32 FSM_STATE                               :  4;  // Bits 3:0
    U32 RPT_CMD_CNT                             :  6;  // Bits 9:4
    U32 OPCODE                                  :  7;  // Bits 16:10
    U32 WR_DWORD_SEL                            :  4;  // Bits 20:17
    U32 RD_DWORD_SEL                            :  4;  // Bits 24:21
    U32 P2P_RD_UP                               :  1;  // Bits 25:25
    U32 P2P_RD_DN                               :  1;  // Bits 26:26
    U32 SPARE                                   :  5;  // Bits 31:27
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_MBOX_STATUS_HUBS_STRUCT;

typedef union {
  struct {
    U32 XTM_CHID                                :  9;  // Bits 8:0
    U32 NP                                      :  1;  // Bits 9:9
    U32 PC                                      :  1;  // Bits 10:10
    U32                                         :  21;  // Bits 31:11
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} HUBS_CR_HUB0_BLOCK_UP_HUBS_STRUCT;

typedef union {
  struct {
    U32 DMIVC0_PMIN                             :  3;  // Bits 2:0
    U32 P10VC0_PMIN                             :  3;  // Bits 5:3
    U32 P11VC0_PMIN                             :  3;  // Bits 8:6
    U32 P12VC0_PMIN                             :  3;  // Bits 11:9
    U32 DEVC0_PMIN                              :  3;  // Bits 14:12
    U32 DMIVCP_PMIN                             :  3;  // Bits 17:15
    U32 DMIVCM_PMIN                             :  3;  // Bits 20:18
    U32 DMIVC1_PMIN                             :  3;  // Bits 23:21
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL0_IMPH_STRUCT;

typedef union {
  struct {
    U32 DMIVC0_NPMIN                            :  3;  // Bits 2:0
    U32 P10VC0_NPMIN                            :  3;  // Bits 5:3
    U32 P11VC0_NPMIN                            :  3;  // Bits 8:6
    U32 P12VC0_NPMIN                            :  3;  // Bits 11:9
    U32 DEVC0_NPMIN                             :  3;  // Bits 14:12
    U32 DMIVCP_NPMIN                            :  3;  // Bits 17:15
    U32 DMIVCM_NPMIN                            :  3;  // Bits 20:18
    U32 DMIVC1_NPMIN                            :  3;  // Bits 23:21
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL1_IMPH_STRUCT;

typedef union {
  struct {
    U32 DMIVC0_RRMIN                            :  3;  // Bits 2:0
    U32 P10VC0_RRMIN                            :  3;  // Bits 5:3
    U32 P11VC0_RRMIN                            :  3;  // Bits 8:6
    U32 P12VC0_RRMIN                            :  3;  // Bits 11:9
    U32 DEVC0_RRMIN                             :  3;  // Bits 14:12
    U32 DMIVCP_RRMIN                            :  3;  // Bits 17:15
    U32 DMIVCM_RRMIN                            :  3;  // Bits 20:18
    U32 DMIVC1_RRMIN                            :  3;  // Bits 23:21
    U32 DEVC1_RRMIN                             :  6;  // Bits 29:24
    U32                                         :  2;  // Bits 31:30
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL2_IMPH_STRUCT;

typedef union {
  struct {
    U32 IOTRK_SHRD                              :  6;  // Bits 5:0
    U32 RRTRK_SHRD                              :  7;  // Bits 12:6
    U32                                         :  19;  // Bits 31:13
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL3_IMPH_STRUCT;

typedef union {
  struct {
    U32 DMIVC0_PMAX                             :  5;  // Bits 4:0
    U32 P10VC0_PMAX                             :  5;  // Bits 9:5
    U32 P11VC0_PMAX                             :  5;  // Bits 14:10
    U32 P12VC0_PMAX                             :  5;  // Bits 19:15
    U32 DEVC0_PMAX                              :  5;  // Bits 24:20
    U32                                         :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL4_IMPH_STRUCT;

typedef union {
  struct {
    U32 DMIVCP_PMAX                             :  5;  // Bits 4:0
    U32 DMIVCM_PMAX                             :  5;  // Bits 9:5
    U32 DMIVC1_PMAX                             :  5;  // Bits 14:10
    U32                                         :  17;  // Bits 31:15
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL5_IMPH_STRUCT;

typedef union {
  struct {
    U32 DMIVC0_NPMAX                            :  5;  // Bits 4:0
    U32 P10VC0_NPMAX                            :  5;  // Bits 9:5
    U32 P11VC0_NPMAX                            :  5;  // Bits 14:10
    U32 P12VC0_NPMAX                            :  5;  // Bits 19:15
    U32 DEVC0_NPMAX                             :  5;  // Bits 24:20
    U32                                         :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL6_IMPH_STRUCT;

typedef union {
  struct {
    U32 DMIVCP_NPMAX                            :  5;  // Bits 4:0
    U32 DMIVCM_NPMAX                            :  5;  // Bits 9:5
    U32 DMIVC1_NPMAX                            :  5;  // Bits 14:10
    U32                                         :  17;  // Bits 31:15
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL7_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_VISA_XBAR_MPIORDTRKS_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_STRUCT;

typedef union {
  struct {
    U32 VC                                      :  4;  // Bits 3:0
    U32 FMT_CMDTYPE                             :  6;  // Bits 9:4
    U32 TC                                      :  4;  // Bits 13:10
    U32 NS                                      :  1;  // Bits 14:14
    U32 RO                                      :  1;  // Bits 15:15
    U32 LENGTH                                  :  5;  // Bits 20:16
    U32 EP                                      :  1;  // Bits 21:21
    U32                                         :  10;  // Bits 31:22
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_MASK1_IMPH_STRUCT;

typedef union {
  struct {
    U32 RQID                                    :  16;  // Bits 15:0
    U32 TAG                                     :  8;  // Bits 23:16
    U32 LBEFBE_MSGTYPE                          :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_MASK2_IMPH_STRUCT;

typedef union {
  struct {
    U32 ADDR_31_0                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_MASK3_IMPH_STRUCT;

typedef union {
  struct {
    U32 ADDR_63_32                              :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_MASK4_IMPH_STRUCT;

typedef union {
  struct {
    U32 VC                                      :  4;  // Bits 3:0
    U32 FMT_CMDTYPE                             :  6;  // Bits 9:4
    U32 TC                                      :  4;  // Bits 13:10
    U32 EP                                      :  1;  // Bits 14:14
    U32 NS                                      :  1;  // Bits 15:15
    U32 RO                                      :  1;  // Bits 16:16
    U32 LENGTH                                  :  5;  // Bits 21:17
    U32                                         :  10;  // Bits 31:22
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_COMP1_IMPH_STRUCT;

typedef union {
  struct {
    U32 RQID                                    :  16;  // Bits 15:0
    U32 TAG                                     :  8;  // Bits 23:16
    U32 LBEFBE_MSGTYPE                          :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_COMP2_IMPH_STRUCT;

typedef union {
  struct {
    U32 ADDR_31_0                               :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_COMP3_IMPH_STRUCT;

typedef union {
  struct {
    U32 ADDR_63_32                              :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_COMP4_IMPH_STRUCT;

typedef union {
  struct {
    U32 ENABLE                                  :  1;  // Bits 0:0
    U32 TRIGGERED                               :  1;  // Bits 1:1
    U32 STALL_DNARB                             :  1;  // Bits 2:2
    U32                                         :  20;  // Bits 22:3
    U32 STALL_DELAY                             :  9;  // Bits 31:23
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STRUCT;

typedef union {
  struct {
    U32 DMIVC0_RRMAX                            :  6;  // Bits 5:0
    U32 P10VC0_RRMAX                            :  6;  // Bits 11:6
    U32 P11VC0_RRMAX                            :  6;  // Bits 17:12
    U32 P12VC0_RRMAX                            :  6;  // Bits 23:18
    U32 DEVC0_RRMAX                             :  6;  // Bits 29:24
    U32                                         :  2;  // Bits 31:30
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL8_IMPH_STRUCT;

typedef union {
  struct {
    U32 DMIVCP_RRMAX                            :  6;  // Bits 5:0
    U32 DMIVCM_RRMAX                            :  6;  // Bits 11:6
    U32 DMIVC1_RRMAX                            :  6;  // Bits 17:12
    U32 DEVC1_RRMAX                             :  6;  // Bits 23:18
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTCTL9_IMPH_STRUCT;

typedef union {
  struct {
    U32 LIM                                     :  16;  // Bits 15:0
    U32 MSK                                     :  3;  // Bits 18:16
    U32                                         :  13;  // Bits 31:19
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_STRUCT;

typedef union {
  struct {
    U32 FUNNUM                                  :  3;  // Bits 2:0
    U32 DEVNUM                                  :  5;  // Bits 7:3
    U32 BUSNUM                                  :  8;  // Bits 15:8
    U32 BARNUM                                  :  3;  // Bits 18:16
    U32                                         :  13;  // Bits 31:19
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_STRUCT;

typedef union {
  struct {
    U32 LOCK                                    :  1;  // Bits 0:0
    U32                                         :  31;  // Bits 31:1
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPRDRTRN_CR_CRDTLCK_IMPH_STRUCT;

typedef union {
  struct {
    U32 VCPNPLIM                                :  3;  // Bits 2:0
    U32                                         :  1;  // Bits 3:3
    U32 VCPPLIM                                 :  3;  // Bits 6:4
    U32                                         :  1;  // Bits 7:7
    U32 VCMNPLIM                                :  3;  // Bits 10:8
    U32                                         :  1;  // Bits 11:11
    U32 VCMPLIM                                 :  3;  // Bits 14:12
    U32                                         :  1;  // Bits 15:15
    U32 VC0VTDLIM                               :  3;  // Bits 18:16
    U32                                         :  1;  // Bits 19:19
    U32 VCPVTDLIM                               :  3;  // Bits 22:20
    U32                                         :  9;  // Bits 31:23
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPMCARB_CR_VCLIM0_IMPH_STRUCT;

typedef union {
  struct {
    U32 IARD                                    :  3;  // Bits 2:0
    U32                                         :  1;  // Bits 3:3
    U32 IAWR                                    :  3;  // Bits 6:4
    U32                                         :  1;  // Bits 7:7
    U32 VTDL3                                   :  3;  // Bits 10:8
    U32                                         :  1;  // Bits 11:11
    U32 VTDNL3                                  :  3;  // Bits 14:12
    U32                                         :  17;  // Bits 31:15
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPMCARB_CR_VCLIM1_IMPH_STRUCT;

typedef union {
  struct {
    U32 VC1_WR_CNFLT                            :  1;  // Bits 0:0
    U32 VC1_RD_CNFLT                            :  1;  // Bits 1:1
    U32                                         :  30;  // Bits 31:2
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPMCARB_CR_ATMC_STS_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_STRUCT;

typedef union {
  struct {
    U32 LOCK                                    :  1;  // Bits 0:0
    U32                                         :  31;  // Bits 31:1
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MPMCARB_CR_MCARBLCK_IMPH_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_CAMARILLO_MAILBOX_DATA0_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_CAMARILLO_MAILBOX_DATA1_PCU_STRUCT;

typedef union {
  struct {
    U32 COMMAND                                 :  8;  // Bits 7:0
    U32 ADDR_CNTL                               :  21;  // Bits 28:8
    U32                                         :  2;  // Bits 30:29
    U32 RUN_BUSY                                :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_STRUCT;

typedef union {
  struct {
    U32 HIGH_TEMP_INT_ENABLE                    :  1;  // Bits 0:0
    U32 LOW_TEMP_INT_ENABLE                     :  1;  // Bits 1:1
    U32 PROCHOT_INT_ENABLE                      :  1;  // Bits 2:2
    U32                                         :  1;  // Bits 3:3
    U32 OUT_OF_SPEC_INT_ENABLE                  :  1;  // Bits 4:4
    U32                                         :  3;  // Bits 7:5
    U32 THRESHOLD_1_REL_TEMP                    :  7;  // Bits 14:8
    U32 THRESHOLD_1_INT_ENABLE                  :  1;  // Bits 15:15
    U32 THRESHOLD_2_REL_TEMP                    :  7;  // Bits 22:16
    U32 THRESHOLD_2_INT_ENABLE                  :  1;  // Bits 23:23
    U32 POWER_INT_ENABLE                        :  1;  // Bits 24:24
    U32                                         :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_STRUCT;

typedef union {
  struct {
    U32 OLTM_ENABLE                             :  1;  // Bits 0:0
    U32 CLTM_ENABLE                             :  1;  // Bits 1:1
    U32 REFRESH_2X_MODE                         :  2;  // Bits 3:2
    U32 EXTTS_ENABLE                            :  1;  // Bits 4:4
    U32 LOCK_PTM_REGS_PCU                       :  1;  // Bits 5:5
    U32 PDWN_CONFIG_CTL                         :  1;  // Bits 6:6
    U32 DISABLE_DRAM_TS                         :  1;  // Bits 7:7
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_PTM_CTL_PCU_STRUCT;

typedef union {
  struct {
    U32 SCALEFACTOR                             :  3;  // Bits 2:0
    U32                                         :  29;  // Bits 31:3
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_STRUCT;

typedef union {
  struct {
    U32 CH0                                     :  8;  // Bits 7:0
    U32 CH1                                     :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_STRUCT;

typedef union {
  struct {
    U32 CH0_DIMM0                               :  2;  // Bits 1:0
    U32 CH0_DIMM1                               :  2;  // Bits 3:2
    U32                                         :  4;  // Bits 7:4
    U32 CH1_DIMM0                               :  2;  // Bits 9:8
    U32 CH1_DIMM1                               :  2;  // Bits 11:10
    U32                                         :  20;  // Bits 31:12
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_STRUCT;

typedef union {
  struct {
    U32 ENABLE_WARM_INTERRUPT                   :  1;  // Bits 0:0
    U32                                         :  1;  // Bits 1:1
    U32 ENABLE_HOT_INTERRUPT                    :  1;  // Bits 2:2
    U32                                         :  1;  // Bits 3:3
    U32 ENABLE_2X_REFRESH_INTERRUPT             :  1;  // Bits 4:4
    U32                                         :  3;  // Bits 7:5
    U32 ENABLE_THRESHOLD1_INTERRUPT             :  1;  // Bits 8:8
    U32                                         :  1;  // Bits 9:9
    U32 ENABLE_THRESHOLD2_INTERRUPT             :  1;  // Bits 10:10
    U32                                         :  5;  // Bits 15:11
    U32 POLICY_FREE_THRESHOLD1                  :  8;  // Bits 23:16
    U32 POLICY_FREE_THRESHOLD2                  :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_STRUCT;

typedef union {
  struct {
    U32 DDR_VOLTAGE                             :  3;  // Bits 2:0
    U32                                         :  29;  // Bits 31:3
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_VOLTAGE_PCU_STRUCT;

typedef union {
  struct {
    U32 THERM_MARGIN                            :  16;  // Bits 15:0
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PACKAGE_THERM_MARGIN_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_STRUCT;

typedef union {
  struct {
    U32 TEMPERATURE                             :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_DIMM_HOTTEST_ABSOLUTE_PCU_STRUCT;

typedef union {
  struct {
    U32 TEMPERATURE                             :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_DIMM_HOTTEST_RELATIVE_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  16;  // Bits 15:0
    U32 DIMM1                                   :  16;  // Bits 31:16
    U32                                         :  32;  // Bits 63:32
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  16;  // Bits 15:0
    U32 DIMM1                                   :  16;  // Bits 31:16
    U32                                         :  32;  // Bits 63:32
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_WARM_BUDGET_CH0_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_WARM_BUDGET_CH1_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_HOT_BUDGET_CH0_PCU_STRUCT;

typedef union {
  struct {
    U32 DIMM0                                   :  8;  // Bits 7:0
    U32 DIMM1                                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_HOT_BUDGET_CH1_PCU_STRUCT;

typedef union {
  struct {
    U32 LIMIT1_POWER                            :  15;  // Bits 14:0
    U32 LIMIT1_ENABLE                           :  1;  // Bits 15:15
    U32                                         :  1;  // Bits 16:16
    U32 LIMIT1_TIME_WINDOW_Y                    :  5;  // Bits 21:17
    U32 LIMIT1_TIME_WINDOW_X                    :  2;  // Bits 23:22
    U32                                         :  8;  // Bits 31:24
    U32 LIMIT2_POWER                            :  15;  // Bits 46:32
    U32 LIMIT2_ENABLE                           :  1;  // Bits 47:47
    U32                                         :  1;  // Bits 48:48
    U32 LIMIT2_TIME_WINDOW_Y                    :  5;  // Bits 53:49
    U32 LIMIT2_TIME_WINDOW_X                    :  2;  // Bits 55:54
    U32                                         :  7;  // Bits 62:56
    U32 LOCKED                                  :  1;  // Bits 63:63
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_DDR_RAPL_LIMIT_PCU_STRUCT;

typedef union {
  struct {
    U32 JOULES_CONSUMED                         :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_ENERGY_STATUS_PCU_STRUCT;

typedef union {
  struct {
    U32 DURATION                                :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DDR_RAPL_PERF_STATUS_PCU_STRUCT;

typedef union {
  struct {
    U32 COUNTS                                  :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PACKAGE_RAPL_PERF_STATUS_PCU_STRUCT;

typedef union {
  struct {
    U32 IA_MIN_RATIO_REQUEST                    :  8;  // Bits 7:0
    U32 CLR_MIN_RATIO_REQUEST                   :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_GT_RATIOS_OVERRIDE_PCU_STRUCT;

typedef union {
  struct {
    U32 DDR_ACCESS_TIME                         :  14;  // Bits 13:0
    U32 RESERVED                                :  1;  // Bits 14:14
    U32 CLR_ACCESS_TIME                         :  14;  // Bits 28:15
    U32 NON_SNOOP_THRESHOLD_RESOLUTION          :  1;  // Bits 29:29
    U32 SLOW_SNOOP_THRESHOLD_RESOLUTION         :  1;  // Bits 30:30
    U32 FAST_SNOOP_THRESHOLD_RESOLUTION         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_STRUCT;

typedef union {
  struct {
    U32 PROCHOT_STATUS                          :  1;  // Bits 0:0
    U32 THERMAL_STATUS                          :  1;  // Bits 1:1
    U32 SPARE_IA_2                              :  1;  // Bits 2:2
    U32 PBM_PL1_STATUS                          :  1;  // Bits 3:3
    U32 PBM_PL2_STATUS                          :  1;  // Bits 4:4
    U32 PBM_PLIA_STATUS                         :  1;  // Bits 5:5
    U32 SPARE_IA_6                              :  1;  // Bits 6:6
    U32 GTDRIVER_STATUS                         :  1;  // Bits 7:7
    U32 VR_THERMALERT_STATUS                    :  1;  // Bits 8:8
    U32 FUSE_MAX_TURBO_LIMIT_STATUS             :  1;  // Bits 9:9
    U32 EDP_ICC_STATUS                          :  1;  // Bits 10:10
    U32 TURBO_ATTEN_STATUS                      :  1;  // Bits 11:11
    U32 SPARE_IA_12                             :  1;  // Bits 12:12
    U32 SPARE_IA_13                             :  1;  // Bits 13:13
    U32 SPARE_IA_14                             :  1;  // Bits 14:14
    U32 SPARE_IA_15                             :  1;  // Bits 15:15
    U32 PROCHOT_LOG                             :  1;  // Bits 16:16
    U32 THERMAL_LOG                             :  1;  // Bits 17:17
    U32 SPARE_IA_LOG_2                          :  1;  // Bits 18:18
    U32 PBM_PL1_LOG                             :  1;  // Bits 19:19
    U32 PBM_PL2_LOG                             :  1;  // Bits 20:20
    U32 PBM_PLIA_LOG                            :  1;  // Bits 21:21
    U32 SPARE_IA_LOG_6                          :  1;  // Bits 22:22
    U32 GTDRIVER_LOG                            :  1;  // Bits 23:23
    U32 VR_THERMALERT_LOG                       :  1;  // Bits 24:24
    U32 FUSE_MAX_TURBO_LIMIT_LOG                :  1;  // Bits 25:25
    U32 EDP_ICC_LOG                             :  1;  // Bits 26:26
    U32 TURBO_ATTEN_LOG                         :  1;  // Bits 27:27
    U32 SPARE_IA_LOG_12                         :  1;  // Bits 28:28
    U32 SPARE_IA_LOG_13                         :  1;  // Bits 29:29
    U32 SPARE_IA_LOG_14                         :  1;  // Bits 30:30
    U32 SPARE_IA_LOG_15                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_IA_PERF_LIMIT_REASONS_PCU_STRUCT;

typedef union {
  struct {
    U32 PROCHOT_STATUS                          :  1;  // Bits 0:0
    U32 THERMAL_STATUS                          :  1;  // Bits 1:1
    U32 SPARE_GT_2                              :  1;  // Bits 2:2
    U32 PBM_PL1_STATUS                          :  1;  // Bits 3:3
    U32 PBM_PL2_STATUS                          :  1;  // Bits 4:4
    U32 PBM_PLGT_STATUS                         :  1;  // Bits 5:5
    U32 SPARE_GT_6                              :  1;  // Bits 6:6
    U32 SPARE_GT_7                              :  1;  // Bits 7:7
    U32 VR_THERMALERT_STATUS                    :  1;  // Bits 8:8
    U32 SPARE_GT_9                              :  1;  // Bits 9:9
    U32 EDP_ICC_STATUS                          :  1;  // Bits 10:10
    U32 SPARE_GT_11                             :  1;  // Bits 11:11
    U32 SPARE_GT_12                             :  1;  // Bits 12:12
    U32 SPARE_GT_13                             :  1;  // Bits 13:13
    U32 SPARE_GT_14                             :  1;  // Bits 14:14
    U32 SPARE_GT_15                             :  1;  // Bits 15:15
    U32 PROCHOT_LOG                             :  1;  // Bits 16:16
    U32 THERMAL_LOG                             :  1;  // Bits 17:17
    U32 SPARE_GT_LOG_2                          :  1;  // Bits 18:18
    U32 PBM_PL1_LOG                             :  1;  // Bits 19:19
    U32 PBM_PL2_LOG                             :  1;  // Bits 20:20
    U32 PBM_PLGT_LOG                            :  1;  // Bits 21:21
    U32 SPARE_GT_LOG_6                          :  1;  // Bits 22:22
    U32 SPARE_GT_LOG_7                          :  1;  // Bits 23:23
    U32 VR_THERMALERT_LOG                       :  1;  // Bits 24:24
    U32 SPARE_GT_LOG_9                          :  1;  // Bits 25:25
    U32 EDP_ICC_LOG                             :  1;  // Bits 26:26
    U32 SPARE_GT_LOG_11                         :  1;  // Bits 27:27
    U32 SPARE_GT_LOG_12                         :  1;  // Bits 28:28
    U32 SPARE_GT_LOG_13                         :  1;  // Bits 29:29
    U32 SPARE_GT_LOG_14                         :  1;  // Bits 30:30
    U32 SPARE_GT_LOG_15                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_GT_PERF_LIMIT_REASONS_PCU_STRUCT;

typedef union {
  struct {
    U32 PROCHOT_STATUS                          :  1;  // Bits 0:0
    U32 THERMAL_STATUS                          :  1;  // Bits 1:1
    U32 SPARE_CLR_2                             :  1;  // Bits 2:2
    U32 PBM_PL1_STATUS                          :  1;  // Bits 3:3
    U32 PBM_PL2_STATUS                          :  1;  // Bits 4:4
    U32 SPARE_CLR_5                             :  1;  // Bits 5:5
    U32 SPARE_CLR_6                             :  1;  // Bits 6:6
    U32 SPARE_CLR_7                             :  1;  // Bits 7:7
    U32 VR_THERMALERT_STATUS                    :  1;  // Bits 8:8
    U32 SPARE_CLR_9                             :  1;  // Bits 9:9
    U32 EDP_ICC_STATUS                          :  1;  // Bits 10:10
    U32 SPARE_CLR_11                            :  1;  // Bits 11:11
    U32 SPARE_CLR_12                            :  1;  // Bits 12:12
    U32 SPARE_CLR_13                            :  1;  // Bits 13:13
    U32 SPARE_CLR_14                            :  1;  // Bits 14:14
    U32 SPARE_CLR_15                            :  1;  // Bits 15:15
    U32 PROCHOT_LOG                             :  1;  // Bits 16:16
    U32 THERMAL_LOG                             :  1;  // Bits 17:17
    U32 SPARE_CLR_LOG_2                         :  1;  // Bits 18:18
    U32 PBM_PL1_LOG                             :  1;  // Bits 19:19
    U32 PBM_PL2_LOG                             :  1;  // Bits 20:20
    U32 SPARE_CLR_LOG_5                         :  1;  // Bits 21:21
    U32 SPARE_CLR_LOG_6                         :  1;  // Bits 22:22
    U32 SPARE_CLR_LOG_7                         :  1;  // Bits 23:23
    U32 VR_THERMALERT_LOG                       :  1;  // Bits 24:24
    U32 SPARE_CLR_LOG_9                         :  1;  // Bits 25:25
    U32 EDP_ICC_LOG                             :  1;  // Bits 26:26
    U32 SPARE_CLR_LOG_11                        :  1;  // Bits 27:27
    U32 SPARE_CLR_LOG_12                        :  1;  // Bits 28:28
    U32 SPARE_CLR_LOG_13                        :  1;  // Bits 29:29
    U32 SPARE_CLR_LOG_14                        :  1;  // Bits 30:30
    U32 SPARE_CLR_LOG_15                        :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_STRUCT;

typedef union {
  struct {
    U32 PRIPTP                                  :  5;  // Bits 4:0
    U32                                         :  27;  // Bits 31:5
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PRIP_TURBO_PLCY_PCU_STRUCT;

typedef union {
  struct {
    U32 SECPTP                                  :  5;  // Bits 4:0
    U32                                         :  27;  // Bits 31:5
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_SECP_TURBO_PLCY_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PRIP_NRG_STTS_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_SECP_NRG_STTS_PCU_STRUCT;

typedef union {
  struct {
    U32 PKG_TDP                                 :  15;  // Bits 14:0
    U32                                         :  1;  // Bits 15:15
    U32 PKG_MIN_PWR                             :  15;  // Bits 30:16
    U32                                         :  1;  // Bits 31:31
    U32 PKG_MAX_PWR                             :  15;  // Bits 46:32
    U32                                         :  1;  // Bits 47:47
    U32 PKG_MAX_WIN                             :  7;  // Bits 54:48
    U32                                         :  9;  // Bits 63:55
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_PACKAGE_POWER_SKU_PCU_STRUCT;

typedef union {
  struct {
    U32 PWR_UNIT                                :  4;  // Bits 3:0
    U32                                         :  4;  // Bits 7:4
    U32 ENERGY_UNIT                             :  5;  // Bits 12:8
    U32                                         :  3;  // Bits 15:13
    U32 TIME_UNIT                               :  4;  // Bits 19:16
    U32                                         :  12;  // Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PACKAGE_ENERGY_STATUS_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_GT_IO_BUSYNESS_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_GT_VIDEO_BUSYNESS_PCU_STRUCT;

typedef union {
  struct {
    U32 RP_STATE_VOLTAGE                        :  8;  // Bits 7:0
    U32 RP_STATE_RATIO                          :  8;  // Bits 15:8
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_GT_PERF_STATUS_PCU_STRUCT;

typedef union {
  struct {
    U64                                         :  50;  // Bits 49:0
    U32 PLATFORMID                              :  3;  // Bits 52:50
    U32                                         :  11;  // Bits 63:53
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_PLATFORM_ID_PCU_STRUCT;

typedef union {
  struct {
    U32                                         :  8;  // Bits 7:0
    U32 MAX_NON_TURBO_LIM_RATIO                 :  8;  // Bits 15:8
    U32 SMM_SAVE_CAP                            :  1;  // Bits 16:16
    U32                                         :  7;  // Bits 23:17
    U32 OCVOLT_OVRD_AVAIL                       :  1;  // Bits 24:24
    U32 FIVR_RFI_TUNING_AVAIL                   :  1;  // Bits 25:25
    U32 DCU_16K_MODE_AVAIL                      :  1;  // Bits 26:26
    U32 SAMPLE_PART                             :  1;  // Bits 27:27
    U32 PRG_TURBO_RATIO_EN                      :  1;  // Bits 28:28
    U32 PRG_TDP_LIM_EN                          :  1;  // Bits 29:29
    U32 PRG_TJ_OFFSET_EN                        :  1;  // Bits 30:30
    U32 CPUID_FAULTING_EN                       :  1;  // Bits 31:31
    U32 LPM_SUPPORT                             :  1;  // Bits 32:32
    U32 CONFIG_TDP_LEVELS                       :  2;  // Bits 34:33
    U32 PFAT_ENABLE                             :  1;  // Bits 35:35
    U32                                         :  1;  // Bits 36:36
    U32 TIMED_MWAIT_ENABLE                      :  1;  // Bits 37:37
    U32                                         :  2;  // Bits 39:38
    U32 MAX_EFFICIENCY_RATIO                    :  8;  // Bits 47:40
    U32 MIN_OPERATING_RATIO                     :  8;  // Bits 55:48
    U32                                         :  8;  // Bits 63:56
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_PLATFORM_INFO_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PP1_C0_CORE_CLOCK_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PP0_ANY_THREAD_ACTIVITY_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PP0_EFFICIENT_CYCLES_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PP0_THREAD_ACTIVITY_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PP1_ANY_THREAD_ACTIVITY_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PP1_EFFICIENT_CYCLES_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PACKAGE_TEMPERATURE_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PP0_TEMPERATURE_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PP1_TEMPERATURE_PCU_STRUCT;

typedef union {
  struct {
    U32 TIME_VAL                                :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PCU_REFERENCE_CLOCK_PCU_STRUCT;

typedef union {
  struct {
    U32 RESERVED                                :  1;  // Bits 0:0
    U32 VALID                                   :  1;  // Bits 1:1
    U32 RESERVED_BITS                           :  4;  // Bits 5:2
    U32 OD                                      :  1;  // Bits 6:6
    U32 IM                                      :  1;  // Bits 7:7
    U32 NEXT_DEVICE_ACTIVITY                    :  21;  // Bits 28:8
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_STRUCT;

typedef union {
  struct {
    U32 USED                                    :  1;  // Bits 0:0
    U32 VALID                                   :  1;  // Bits 1:1
    U32 RESERVED_BITS                           :  4;  // Bits 5:2
    U32 OD                                      :  1;  // Bits 6:6
    U32 IM                                      :  1;  // Bits 7:7
    U32 NEXT_DEVICE_ACTIVITY                    :  21;  // Bits 28:8
    U32                                         :  3;  // Bits 31:29
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_STRUCT;

typedef union {
  struct {
    U32 PSTT_LIM                                :  8;  // Bits 7:0
    U32 PSTT_MIN                                :  8;  // Bits 15:8
    U32                                         :  15;  // Bits 30:16
    U32 LOCK                                    :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_P_STATE_LIMITS_PCU_STRUCT;

typedef union {
  struct {
    U32 RPSTT_LIM                               :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_RP_STATE_LIMITS_PCU_STRUCT;

typedef union {
  struct {
    U32 RP0_CAP                                 :  8;  // Bits 7:0
    U32 RP1_CAP                                 :  8;  // Bits 15:8
    U32 RPN_CAP                                 :  8;  // Bits 23:16
    U32                                         :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_RP_STATE_CAP_PCU_STRUCT;

typedef union {
  struct {
    U32                                         :  8;  // Bits 7:0
    U32 FAN_TEMP_TARGET_OFST                    :  8;  // Bits 15:8
    U32 REF_TEMP                                :  8;  // Bits 23:16
    U32 TJ_MAX_TCC_OFFSET                       :  4;  // Bits 27:24
    U32                                         :  4;  // Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_TEMPERATURE_TARGET_PCU_STRUCT;

typedef union {
  struct {
    U32 PKG_PWR_LIM_1                           :  15;  // Bits 14:0
    U32 PKG_PWR_LIM_1_EN                        :  1;  // Bits 15:15
    U32 PKG_CLMP_LIM_1                          :  1;  // Bits 16:16
    U32 PKG_PWR_LIM_1_TIME                      :  7;  // Bits 23:17
    U32                                         :  8;  // Bits 31:24
    U32 PKG_PWR_LIM_2                           :  15;  // Bits 46:32
    U32 PKG_PWR_LIM_2_EN                        :  1;  // Bits 47:47
    U32 PKG_CLMP_LIM_2                          :  1;  // Bits 48:48
    U32 PKG_PWR_LIM_2_TIME                      :  7;  // Bits 55:49
    U32                                         :  7;  // Bits 62:56
    U32 PKG_PWR_LIM_LOCK                        :  1;  // Bits 63:63
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_PACKAGE_RAPL_LIMIT_PCU_STRUCT;

typedef union {
  struct {
    U32 IA_PP_PWR_LIM                           :  15;  // Bits 14:0
    U32 PWR_LIM_CTRL_EN                         :  1;  // Bits 15:15
    U32 PP_CLAMP_LIM                            :  1;  // Bits 16:16
    U32 CTRL_TIME_WIN                           :  7;  // Bits 23:17
    U32                                         :  7;  // Bits 30:24
    U32 PP_PWR_LIM_LOCK                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PRIP_TURBO_PWR_LIM_PCU_STRUCT;

typedef union {
  struct {
    U32 NON_IA_PP_PWR_LIM                       :  15;  // Bits 14:0
    U32 PWR_LIM_CTRL_EN                         :  1;  // Bits 15:15
    U32 PP_CLAMP_LIM                            :  1;  // Bits 16:16
    U32 CTRL_TIME_WIN                           :  7;  // Bits 23:17
    U32                                         :  7;  // Bits 30:24
    U32 SP_PWR_LIM_LOCK                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_SECP_TURBO_PWR_LIM_PCU_STRUCT;

typedef union {
  struct {
    U32 CURRENT_LIMIT                           :  13;  // Bits 12:0
    U32                                         :  18;  // Bits 30:13
    U32 LOCK                                    :  1;  // Bits 31:31
    U32 PSI1_THRESHOLD                          :  10;  // Bits 41:32
    U32 PSI2_THRESHOLD                          :  10;  // Bits 51:42
    U32 PSI3_THRESHOLD                          :  10;  // Bits 61:52
    U32 RESERVED                                :  2;  // Bits 63:62
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_VR_CURRENT_CONFIG_PCU_STRUCT;

typedef union {
  struct {
    U32 MRC_Saving_Rd                           :  8;  // Bits 7:0
    U32 MRC_Saving_Wt                           :  8;  // Bits 15:8
    U32 MRC_Saving_Cmd                          :  8;  // Bits 23:16
    U32 RESERVED                                :  8;  // Bits 31:24
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_MRC_ODT_POWER_SAVING_PCU_STRUCT;

typedef union {
  struct {
    U32 THERMAL_MONITOR_STATUS                  :  1;  // Bits 0:0
    U32 THERMAL_MONITOR_LOG                     :  1;  // Bits 1:1
    U32 PROCHOT_STATUS                          :  1;  // Bits 2:2
    U32 PROCHOT_LOG                             :  1;  // Bits 3:3
    U32 OUT_OF_SPEC_STATUS                      :  1;  // Bits 4:4
    U32 OUT_OF_SPEC_LOG                         :  1;  // Bits 5:5
    U32 THRESHOLD1_STATUS                       :  1;  // Bits 6:6
    U32 THRESHOLD1_LOG                          :  1;  // Bits 7:7
    U32 THRESHOLD2_STATUS                       :  1;  // Bits 8:8
    U32 THRESHOLD2_LOG                          :  1;  // Bits 9:9
    U32 POWER_LIMITATION_STATUS                 :  1;  // Bits 10:10
    U32 POWER_LIMITATION_LOG                    :  1;  // Bits 11:11
    U32                                         :  4;  // Bits 15:12
    U32 TEMPERATURE                             :  7;  // Bits 22:16
    U32                                         :  4;  // Bits 26:23
    U32 RESOLUTION                              :  4;  // Bits 30:27
    U32 VALID                                   :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_THERM_STATUS_GT_PCU_STRUCT;

typedef union {
  struct {
    U32 HIGH_TEMP_INT_ENABLE                    :  1;  // Bits 0:0
    U32 LOW_TEMP_INT_ENABLE                     :  1;  // Bits 1:1
    U32 PROCHOT_INT_ENABLE                      :  1;  // Bits 2:2
    U32                                         :  1;  // Bits 3:3
    U32 OUT_OF_SPEC_INT_ENABLE                  :  1;  // Bits 4:4
    U32                                         :  3;  // Bits 7:5
    U32 THRESHOLD_1_REL_TEMP                    :  7;  // Bits 14:8
    U32 THRESHOLD_1_INT_ENABLE                  :  1;  // Bits 15:15
    U32 THRESHOLD_2_REL_TEMP                    :  7;  // Bits 22:16
    U32 THRESHOLD_2_INT_ENABLE                  :  1;  // Bits 23:23
    U32 POWER_INT_ENABLE                        :  1;  // Bits 24:24
    U32                                         :  7;  // Bits 31:25
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_THERM_INTERRUPT_GT_PCU_STRUCT;

typedef union {
  struct {
    U32 RESERVED                                :  1;  // Bits 0:0
    U32 VALID                                   :  1;  // Bits 1:1
    U32 RESERVED_BITS                           :  4;  // Bits 5:2
    U32 OD                                      :  1;  // Bits 6:6
    U32 IM                                      :  1;  // Bits 7:7
    U32 NEXT_DEVICE_ACTIVITY                    :  21;  // Bits 28:8
    U32 DISABLE_MDID_EVALUATION                 :  1;  // Bits 29:29
    U32 FORCE_MDID_OVERRIDE                     :  1;  // Bits 30:30
    U32                                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_STRUCT;

typedef union {
  struct {
    U32 VALUE                                   :  10;  // Bits 9:0
    U32 MULTIPLIER                              :  3;  // Bits 12:10
    U32                                         :  2;  // Bits 14:13
    U32 VALID                                   :  1;  // Bits 15:15
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_STRUCT;

typedef union {
  struct {
    U32                                         :  4;  // Bits 3:0
    U32 PECI_CMD                                :  8;  // Bits 11:4
    U32                                         :  20;  // Bits 31:12
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_CHAP_CONFIG_PCU_STRUCT;

typedef union {
  struct {
    U32 FREQ_TH1                                :  6;  // Bits 5:0
    U32                                         :  2;  // Bits 7:6
    U32 FREQ_TH2                                :  6;  // Bits 13:8
    U32                                         :  2;  // Bits 15:14
    U32 FREQ_TH3                                :  6;  // Bits 21:16
    U32                                         :  10;  // Bits 31:22
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_CHAP_THRESHOLD2_PCU_STRUCT;

typedef union {
  struct {
    U32 DEBUG_ENERGY_PP0_VALUE                  :  10;  // Bits 9:0
    U32 DEBUG_ENERGY_PP1_VALUE                  :  10;  // Bits 19:10
    U32 DEBUG_ENERGY_SA_VALUE                   :  10;  // Bits 29:20
    U32                                         :  2;  // Bits 31:30
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_ENERGY_DEBUG_PCU_STRUCT;

typedef union {
  struct {
    U64 SKPD                                    :  64;  // Bits 63:0
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_SSKPD_PCU_STRUCT;

typedef union {
  struct {
    U32 PPDN_INIT                               :  12;  // Bits 11:0
    U32                                         :  20;  // Bits 31:12
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_C2C3TT_PCU_STRUCT;

typedef union {
  struct {
    U32 DDR_TIMER_VALUE                         :  13;  // Bits 12:0
    U32                                         :  19;  // Bits 31:13
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_C2_DDR_TT_PCU_STRUCT;

typedef union {
  struct {
    U32 NSTL                                    :  10;  // Bits 9:0
    U32 MULTIPLIER                              :  3;  // Bits 12:10
    U32                                         :  1;  // Bits 13:13
    U32 FORCE_NL                                :  1;  // Bits 14:14
    U32 NL_V                                    :  1;  // Bits 15:15
    U32 SXL                                     :  10;  // Bits 25:16
    U32 SXLM                                    :  3;  // Bits 28:26
    U32                                         :  1;  // Bits 29:29
    U32 FORCE_SXL                               :  1;  // Bits 30:30
    U32 SXL_V                                   :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PCIE_ILTR_OVRD_PCU_STRUCT;

typedef union {
  struct {
    U32 NL_VALUE                                :  10;  // Bits 9:0
    U32 NL_SCALE                                :  3;  // Bits 12:10
    U32 NL_RESERVED                             :  2;  // Bits 14:13
    U32 NL_VALID                                :  1;  // Bits 15:15
    U32 SXL_VALUE                               :  10;  // Bits 25:16
    U32 SXL_SCALE                               :  3;  // Bits 28:26
    U32 SXL_RESERVED                            :  2;  // Bits 30:29
    U32 SXL_VALID                               :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PCIE_ILTR_VAL_PCU_0_STRUCT;

typedef union {
  struct {
    U32 NL_VALUE                                :  10;  // Bits 9:0
    U32 NL_SCALE                                :  3;  // Bits 12:10
    U32 NL_RESERVED                             :  2;  // Bits 14:13
    U32 NL_VALID                                :  1;  // Bits 15:15
    U32 SXL_VALUE                               :  10;  // Bits 25:16
    U32 SXL_SCALE                               :  3;  // Bits 28:26
    U32 SXL_RESERVED                            :  2;  // Bits 30:29
    U32 SXL_VALID                               :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PCIE_ILTR_VAL_PCU_1_STRUCT;

typedef union {
  struct {
    U32 NL_VALUE                                :  10;  // Bits 9:0
    U32 NL_SCALE                                :  3;  // Bits 12:10
    U32 NL_RESERVED                             :  2;  // Bits 14:13
    U32 NL_VALID                                :  1;  // Bits 15:15
    U32 SXL_VALUE                               :  10;  // Bits 25:16
    U32 SXL_SCALE                               :  3;  // Bits 28:26
    U32 SXL_RESERVED                            :  2;  // Bits 30:29
    U32 SXL_VALID                               :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_PCIE_ILTR_VAL_PCU_2_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_VISA_CTL_PTPCFSMS_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_VISA_XBAR_PTPCFSMS_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_VISA_CTL_PTPCIOREGS_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  32;  // Bits 31:0
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_BIOS_MAILBOX_DATA_PCU_STRUCT;

typedef union {
  struct {
    U32 COMMAND                                 :  8;  // Bits 7:0
    U32 ADDR                                    :  21;  // Bits 28:8
    U32                                         :  2;  // Bits 30:29
    U32 RUN_BUSY                                :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_STRUCT;

typedef union {
  struct {
    U32 RST_CPL                                 :  1;  // Bits 0:0
    U32 PCIE_ENUMERATION_DONE                   :  1;  // Bits 1:1
    U32 C7_ALLOWED                              :  1;  // Bits 2:2
    U32                                         :  29;  // Bits 31:3
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_BIOS_RESET_CPL_PCU_STRUCT;

typedef union {
  struct {
    U32 REQ_DATA                                :  4;  // Bits 3:0
    U32 REQ_TYPE                                :  4;  // Bits 7:4
    U32                                         :  23;  // Bits 30:8
    U32 RUN_BUSY                                :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_MC_BIOS_REQ_PCU_STRUCT;

typedef union {
  struct {
    U32 MC_FREQ                                 :  4;  // Bits 3:0
    U32                                         :  28;  // Bits 31:4
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_MC_BIOS_DATA_PCU_STRUCT;

typedef union {
  struct {
    U32 SACG_ENA                                :  1;  // Bits 0:0
    U32 MPLL_OFF_ENA                            :  1;  // Bits 1:1
    U32 PPLL_OFF_ENA                            :  1;  // Bits 2:2
    U32                                         :  5;  // Bits 7:3
    U32 SACG_SEN                                :  1;  // Bits 8:8
    U32 MPLL_OFF_SEN                            :  1;  // Bits 9:9
    U32 MDLL_OFF_SEN                            :  1;  // Bits 10:10
    U32 SACG_SREXIT                             :  1;  // Bits 11:11
    U32 NSWAKE_SREXIT                           :  1;  // Bits 12:12
    U32 SACG_MPLL                               :  1;  // Bits 13:13
    U32 MPLL_ON_DE                              :  1;  // Bits 14:14
    U32 MDLL_ON_DE                              :  1;  // Bits 15:15
    U32                                         :  16;  // Bits 31:16
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_SAPMCTL_PCU_STRUCT;

typedef union {
  struct {
    U32 COMP_DISABLE                            :  1;  // Bits 0:0
    U32 COMP_INTERVAL                           :  4;  // Bits 4:1
    U32                                         :  3;  // Bits 7:5
    U32 COMP_FORCE                              :  1;  // Bits 8:8
    U32                                         :  23;  // Bits 31:9
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_P_COMP_PCU_STRUCT;

typedef union {
  struct {
    U32 COMP_DISABLE                            :  1;  // Bits 0:0
    U32 COMP_INTERVAL                           :  4;  // Bits 4:1
    U32                                         :  3;  // Bits 7:5
    U32 COMP_FORCE                              :  1;  // Bits 8:8
    U32                                         :  23;  // Bits 31:9
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_M_COMP_PCU_STRUCT;

typedef union {
  struct {
    U32 COMP_DISABLE                            :  1;  // Bits 0:0
    U32 COMP_INTERVAL                           :  4;  // Bits 4:1
    U32                                         :  3;  // Bits 7:5
    U32 COMP_FORCE                              :  1;  // Bits 8:8
    U32                                         :  23;  // Bits 31:9
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_D_COMP_PCU_STRUCT;

typedef union {
  struct {
    U32 TDP_RATIO                               :  8;  // Bits 7:0
    U32                                         :  24;  // Bits 31:8
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_CONFIG_TDP_NOMINAL_PCU_STRUCT;

typedef union {
  struct {
    U32 PKG_TDP                                 :  15;  // Bits 14:0
    U32                                         :  1;  // Bits 15:15
    U32 TDP_RATIO                               :  8;  // Bits 23:16
    U32                                         :  8;  // Bits 31:24
    U32 PKG_MAX_PWR                             :  15;  // Bits 46:32
    U32 PKG_MIN_PWR                             :  16;  // Bits 62:47
    U32                                         :  1;  // Bits 63:63
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_CONFIG_TDP_LEVEL1_PCU_STRUCT;

typedef union {
  struct {
    U32 PKG_TDP                                 :  15;  // Bits 14:0
    U32                                         :  1;  // Bits 15:15
    U32 TDP_RATIO                               :  8;  // Bits 23:16
    U32                                         :  8;  // Bits 31:24
    U32 PKG_MAX_PWR                             :  15;  // Bits 46:32
    U32 PKG_MIN_PWR                             :  16;  // Bits 62:47
    U32                                         :  1;  // Bits 63:63
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} PCU_CR_CONFIG_TDP_LEVEL2_PCU_STRUCT;

typedef union {
  struct {
    U32 TDP_LEVEL                               :  2;  // Bits 1:0
    U32                                         :  29;  // Bits 30:2
    U32 CONFIG_TDP_LOCK                         :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_CONFIG_TDP_CONTROL_PCU_STRUCT;

typedef union {
  struct {
    U32 MAX_NON_TURBO_RATIO                     :  8;  // Bits 7:0
    U32                                         :  23;  // Bits 30:8
    U32 TURBO_ACTIVATION_RATIO_LOCK             :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} PCU_CR_TURBO_ACTIVATION_RATIO_PCU_STRUCT;

typedef union {
  struct {
    U32 DATA                                    :  18;  // Bits 17:0
    U32                                         :  13;  // Bits 30:18
    U32 VORANGE                                 :  1;  // Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_STRUCT;


#define NCDECS_CR_GFXVTBAR_NCU_REG                                   (0x00005400)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAREN_OFF                        ( 0)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAREN_WID                        ( 1)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAREN_MSK                        (0x00000001)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAREN_MAX                        (0x00000001)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAREN_DEF                        (0x00000000)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAR_OFF                          (12)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAR_WID                          (27)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAR_MSK                          (0x7FFFFFF000)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAR_MAX                          (0x07FFFFFF)
  #define NCDECS_CR_GFXVTBAR_NCU_GFXVTBAR_DEF                          (0x00000000)

#define NCDECS_CR_EDRAMBAR_NCU_REG                                   (0x00005408)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAREN_OFF                        ( 0)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAREN_WID                        ( 1)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAREN_MSK                        (0x00000001)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAREN_MAX                        (0x00000001)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAREN_DEF                        (0x00000000)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAR_OFF                          (14)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAR_WID                          (25)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAR_MSK                          (0x7FFFFFC000)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAR_MAX                          (0x01FFFFFF)
  #define NCDECS_CR_EDRAMBAR_NCU_EDRAMBAR_DEF                          (0x00000000)

#define NCDECS_CR_VTDPVC0BAR_NCU_REG                                 (0x00005410)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAREN_OFF                      ( 0)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAREN_WID                      ( 1)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAREN_MSK                      (0x00000001)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAREN_MAX                      (0x00000001)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAREN_DEF                      (0x00000000)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAR_OFF                        (12)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAR_WID                        (27)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAR_MSK                        (0x7FFFFFF000)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAR_MAX                        (0x07FFFFFF)
  #define NCDECS_CR_VTDPVC0BAR_NCU_VTVC0BAR_DEF                        (0x00000000)

#define NCDECS_CR_INTRDIRCTL_NCU_REG                                 (0x00005418)
  #define NCDECS_CR_INTRDIRCTL_NCU_RdrModSel_OFF                       ( 0)
  #define NCDECS_CR_INTRDIRCTL_NCU_RdrModSel_WID                       ( 3)
  #define NCDECS_CR_INTRDIRCTL_NCU_RdrModSel_MSK                       (0x00000007)
  #define NCDECS_CR_INTRDIRCTL_NCU_RdrModSel_MAX                       (0x00000007)
  #define NCDECS_CR_INTRDIRCTL_NCU_RdrModSel_DEF                       (0x00000000)
  #define NCDECS_CR_INTRDIRCTL_NCU_ClastChkSmpMod_OFF                  ( 3)
  #define NCDECS_CR_INTRDIRCTL_NCU_ClastChkSmpMod_WID                  ( 1)
  #define NCDECS_CR_INTRDIRCTL_NCU_ClastChkSmpMod_MSK                  (0x00000008)
  #define NCDECS_CR_INTRDIRCTL_NCU_ClastChkSmpMod_MAX                  (0x00000001)
  #define NCDECS_CR_INTRDIRCTL_NCU_ClastChkSmpMod_DEF                  (0x00000000)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFltClustMod_OFF                  ( 4)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFltClustMod_WID                  ( 1)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFltClustMod_MSK                  (0x00000010)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFltClustMod_MAX                  (0x00000001)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFltClustMod_DEF                  (0x00000000)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFlatClustOvrEn_OFF               ( 5)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFlatClustOvrEn_WID               ( 1)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFlatClustOvrEn_MSK               (0x00000020)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFlatClustOvrEn_MAX               (0x00000001)
  #define NCDECS_CR_INTRDIRCTL_NCU_LogFlatClustOvrEn_DEF               (0x00000000)
  #define NCDECS_CR_INTRDIRCTL_NCU_HashModCtr_OFF                      ( 6)
  #define NCDECS_CR_INTRDIRCTL_NCU_HashModCtr_WID                      ( 3)
  #define NCDECS_CR_INTRDIRCTL_NCU_HashModCtr_MSK                      (0x000001C0)
  #define NCDECS_CR_INTRDIRCTL_NCU_HashModCtr_MAX                      (0x00000007)
  #define NCDECS_CR_INTRDIRCTL_NCU_HashModCtr_DEF                      (0x00000000)

#define NCDECS_CR_NCUCTL0_NCU_REG                                    (0x0000541C)
  #define NCDECS_CR_NCUCTL0_NCU_PLIM_OFF                               (28)
  #define NCDECS_CR_NCUCTL0_NCU_PLIM_WID                               ( 3)
  #define NCDECS_CR_NCUCTL0_NCU_PLIM_MSK                               (0x70000000)
  #define NCDECS_CR_NCUCTL0_NCU_PLIM_MAX                               (0x00000007)
  #define NCDECS_CR_NCUCTL0_NCU_PLIM_DEF                               (0x00000003)

#define NCDECS_CR_GDXCBAR_NCU_REG                                    (0x00005420)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAREN_OFF                          ( 0)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAREN_WID                          ( 1)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAREN_MSK                          (0x00000001)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAREN_MAX                          (0x00000001)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAREN_DEF                          (0x00000000)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAR_OFF                            (12)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAR_WID                            (27)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAR_MSK                            (0x7FFFFFF000)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAR_MAX                            (0x07FFFFFF)
  #define NCDECS_CR_GDXCBAR_NCU_GDXCBAR_DEF                            (0x00000000)

#define NCDECS_CR_SCRATCHPAD_NCU_0_REG                               (0x00005428)
  #define NCDECS_CR_SCRATCHPAD_NCU_0_Data_OFF                          ( 0)
  #define NCDECS_CR_SCRATCHPAD_NCU_0_Data_WID                          (32)
  #define NCDECS_CR_SCRATCHPAD_NCU_0_Data_MSK                          (0xFFFFFFFF)
  #define NCDECS_CR_SCRATCHPAD_NCU_0_Data_MAX                          (0xFFFFFFFF)
  #define NCDECS_CR_SCRATCHPAD_NCU_0_Data_DEF                          (0x00000000)

#define NCDECS_CR_SCRATCHPAD_NCU_1_REG                               (0x0000542C)
  #define NCDECS_CR_SCRATCHPAD_NCU_1_Data_OFF                          ( 0)
  #define NCDECS_CR_SCRATCHPAD_NCU_1_Data_WID                          (32)
  #define NCDECS_CR_SCRATCHPAD_NCU_1_Data_MSK                          (0xFFFFFFFF)
  #define NCDECS_CR_SCRATCHPAD_NCU_1_Data_MAX                          (0xFFFFFFFF)
  #define NCDECS_CR_SCRATCHPAD_NCU_1_Data_DEF                          (0x00000000)

#define NCDECS_CR_SCRATCHPAD_NCU_2_REG                               (0x00005430)
  #define NCDECS_CR_SCRATCHPAD_NCU_2_Data_OFF                          ( 0)
  #define NCDECS_CR_SCRATCHPAD_NCU_2_Data_WID                          (32)
  #define NCDECS_CR_SCRATCHPAD_NCU_2_Data_MSK                          (0xFFFFFFFF)
  #define NCDECS_CR_SCRATCHPAD_NCU_2_Data_MAX                          (0xFFFFFFFF)
  #define NCDECS_CR_SCRATCHPAD_NCU_2_Data_DEF                          (0x00000000)

#define NCDECS_CR_SCRATCHPAD_NCU_3_REG                               (0x00005434)
  #define NCDECS_CR_SCRATCHPAD_NCU_3_Data_OFF                          ( 0)
  #define NCDECS_CR_SCRATCHPAD_NCU_3_Data_WID                          (32)
  #define NCDECS_CR_SCRATCHPAD_NCU_3_Data_MSK                          (0xFFFFFFFF)
  #define NCDECS_CR_SCRATCHPAD_NCU_3_Data_MAX                          (0xFFFFFFFF)
  #define NCDECS_CR_SCRATCHPAD_NCU_3_Data_DEF                          (0x00000000)

#define NCDECS_CR_PAVPMSGOFFST_NCU_REG                               (0x00005500)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_LOCK_OFF                          ( 0)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_LOCK_WID                          ( 1)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_LOCK_MSK                          (0x00000001)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_LOCK_MAX                          (0x00000001)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_LOCK_DEF                          (0x00000000)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_OFFSET_OFF                        (20)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_OFFSET_WID                        (12)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_OFFSET_MSK                        (0xFFF00000)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_OFFSET_MAX                        (0x00000FFF)
  #define NCDECS_CR_PAVPMSGOFFST_NCU_OFFSET_DEF                        (0x00000000)

#define MPVTDTRK_CR_VTDLIM_IMPH_REG                                  (0x00006000)
  #define MPVTDTRK_CR_VTDLIM_IMPH_VCPVTDLIM_OFF                        ( 0)
  #define MPVTDTRK_CR_VTDLIM_IMPH_VCPVTDLIM_WID                        ( 3)
  #define MPVTDTRK_CR_VTDLIM_IMPH_VCPVTDLIM_MSK                        (0x00000007)
  #define MPVTDTRK_CR_VTDLIM_IMPH_VCPVTDLIM_MAX                        (0x00000007)
  #define MPVTDTRK_CR_VTDLIM_IMPH_VCPVTDLIM_DEF                        (0x00000004)

#define MPVTDTRK_CR_HDAUDRID_IMPH_REG                                (0x00006008)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_DEVNUM_OFF                         ( 3)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_DEVNUM_WID                         ( 5)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_DEVNUM_MSK                         (0x000000F8)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_DEVNUM_MAX                         (0x0000001F)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_DEVNUM_DEF                         (0x0000001B)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_BUSNUM_OFF                         ( 8)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_BUSNUM_WID                         ( 8)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_BUSNUM_MSK                         (0x0000FF00)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_BUSNUM_MAX                         (0x000000FF)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_BUSNUM_DEF                         (0x00000000)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_HDAUD_EN_OFF                       (31)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_HDAUD_EN_WID                       ( 1)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_HDAUD_EN_MSK                       (0x80000000)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_HDAUD_EN_MAX                       (0x00000001)
  #define MPVTDTRK_CR_HDAUDRID_IMPH_HDAUD_EN_DEF                       (0x00000001)

#define MPVTDTRK_CR_UMAGFXBASE_IMPH_REG                              (0x00006010)
  #define MPVTDTRK_CR_UMAGFXBASE_IMPH_UMAB_OFF                         (20)
  #define MPVTDTRK_CR_UMAGFXBASE_IMPH_UMAB_WID                         (19)
  #define MPVTDTRK_CR_UMAGFXBASE_IMPH_UMAB_MSK                         (0x7FFFF00000)
  #define MPVTDTRK_CR_UMAGFXBASE_IMPH_UMAB_MAX                         (0x0007FFFF)
  #define MPVTDTRK_CR_UMAGFXBASE_IMPH_UMAB_DEF                         (0x0007FFFF)

#define MPVTDTRK_CR_UMAGFXLIMIT_IMPH_REG                             (0x00006018)
  #define MPVTDTRK_CR_UMAGFXLIMIT_IMPH_UMAL_OFF                        (20)
  #define MPVTDTRK_CR_UMAGFXLIMIT_IMPH_UMAL_WID                        (19)
  #define MPVTDTRK_CR_UMAGFXLIMIT_IMPH_UMAL_MSK                        (0x7FFFF00000)
  #define MPVTDTRK_CR_UMAGFXLIMIT_IMPH_UMAL_MAX                        (0x0007FFFF)
  #define MPVTDTRK_CR_UMAGFXLIMIT_IMPH_UMAL_DEF                        (0x00000000)

#define MPVTDTRK_CR_UMAGFXCTL_IMPH_REG                               (0x00006020)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_LCK_OFF                           ( 0)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_LCK_WID                           ( 1)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_LCK_MSK                           (0x00000001)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_LCK_MAX                           (0x00000001)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_LCK_DEF                           (0x00000000)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG10EN_OFF                       (10)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG10EN_WID                       ( 1)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG10EN_MSK                       (0x00000400)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG10EN_MAX                       (0x00000001)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG10EN_DEF                       (0x00000000)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG11EN_OFF                       (11)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG11EN_WID                       ( 1)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG11EN_MSK                       (0x00000800)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG11EN_MAX                       (0x00000001)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG11EN_DEF                       (0x00000000)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG12EN_OFF                       (12)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG12EN_WID                       ( 1)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG12EN_MSK                       (0x00001000)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG12EN_MAX                       (0x00000001)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_PEG12EN_DEF                       (0x00000000)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_DMIEN_OFF                         (13)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_DMIEN_WID                         ( 1)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_DMIEN_MSK                         (0x00002000)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_DMIEN_MAX                         (0x00000001)
  #define MPVTDTRK_CR_UMAGFXCTL_IMPH_DMIEN_DEF                         (0x00000000)

#define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_REG                            (0x00006030)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_FUNNUM_OFF                     ( 0)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_FUNNUM_WID                     ( 3)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_FUNNUM_MSK                     (0x00000007)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_FUNNUM_MAX                     (0x00000007)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_FUNNUM_DEF                     (0x00000000)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_DEVNUM_OFF                     ( 3)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_DEVNUM_WID                     ( 5)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_DEVNUM_MSK                     (0x000000F8)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_DEVNUM_MAX                     (0x0000001F)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_DEVNUM_DEF                     (0x00000016)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BUSNUM_OFF                     ( 8)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BUSNUM_WID                     ( 8)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BUSNUM_MSK                     (0x0000FF00)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BUSNUM_MAX                     (0x000000FF)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BUSNUM_DEF                     (0x00000000)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BARNUM_OFF                     (16)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BARNUM_WID                     ( 3)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BARNUM_MSK                     (0x00070000)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BARNUM_MAX                     (0x00000007)
  #define MPVTDTRK_CR_VDMBDFBARKVM_IMPH_BARNUM_DEF                     (0x00000007)

#define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_REG                           (0x00006034)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_FUNNUM_OFF                    ( 0)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_FUNNUM_WID                    ( 3)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_FUNNUM_MSK                    (0x00000007)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_FUNNUM_MAX                    (0x00000007)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_FUNNUM_DEF                    (0x00000000)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_DEVNUM_OFF                    ( 3)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_DEVNUM_WID                    ( 5)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_DEVNUM_MSK                    (0x000000F8)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_DEVNUM_MAX                    (0x0000001F)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_DEVNUM_DEF                    (0x00000016)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BUSNUM_OFF                    ( 8)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BUSNUM_WID                    ( 8)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BUSNUM_MSK                    (0x0000FF00)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BUSNUM_MAX                    (0x000000FF)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BUSNUM_DEF                    (0x00000000)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BARNUM_OFF                    (16)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BARNUM_WID                    ( 3)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BARNUM_MSK                    (0x00070000)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BARNUM_MAX                    (0x00000007)
  #define MPVTDTRK_CR_VDMBDFBARPAVP_IMPH_BARNUM_DEF                    (0x00000007)

#define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_REG                      (0x00006040)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_DATA_OFF                 ( 0)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_DATA_WID                 (18)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_DATA_MSK                 (0x0003FFFF)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_DATA_MAX                 (0x0003FFFF)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_DATA_DEF                 (0x00000000)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_VORANGE_OFF              (31)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_VORANGE_WID              ( 1)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_VORANGE_MSK              (0x80000000)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_VORANGE_MAX              (0x00000001)
  #define MPVTDTRK_CR_VISA_CTL_MPVTDTLBS_IMPH_VORANGE_DEF              (0x00000000)

#define MPVTDTRK_CR_VISA_XBAR_MPVTDTLBS_IMPH_REG                     (0x00006044)
  #define MPVTDTRK_CR_VISA_XBAR_MPVTDTLBS_IMPH_DATA_OFF                ( 0)
  #define MPVTDTRK_CR_VISA_XBAR_MPVTDTLBS_IMPH_DATA_WID                (32)
  #define MPVTDTRK_CR_VISA_XBAR_MPVTDTLBS_IMPH_DATA_MSK                (0xFFFFFFFF)
  #define MPVTDTRK_CR_VISA_XBAR_MPVTDTLBS_IMPH_DATA_MAX                (0xFFFFFFFF)
  #define MPVTDTRK_CR_VISA_XBAR_MPVTDTLBS_IMPH_DATA_DEF                (0x76543210)

#define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_REG                      (0x00006048)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_DATA_OFF                 ( 0)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_DATA_WID                 (18)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_DATA_MSK                 (0x0003FFFF)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_DATA_MAX                 (0x0003FFFF)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_DATA_DEF                 (0x00000000)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_VORANGE_OFF              (31)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_VORANGE_WID              ( 1)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_VORANGE_MSK              (0x80000000)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_VORANGE_MAX              (0x00000001)
  #define MPVTDTRK_CR_VISA_CTL_MPVTMISCS_IMPH_VORANGE_DEF              (0x00000000)

#define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_REG                       (0x0000604C)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_DATA_OFF                  ( 0)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_DATA_WID                  (18)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_DATA_MSK                  (0x0003FFFF)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_DATA_MAX                  (0x0003FFFF)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_DATA_DEF                  (0x00000000)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_VORANGE_OFF               (31)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_VORANGE_WID               ( 1)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_VORANGE_MSK               (0x80000000)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_VORANGE_MAX               (0x00000001)
  #define MPVTDTRK_CR_VISA_CTL_MPVTTRKS_IMPH_VORANGE_DEF               (0x00000000)

#define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_REG                            (0x00006050)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_VC_OFF                         ( 0)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_VC_WID                         ( 4)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_VC_MSK                         (0x0000000F)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_VC_MAX                         (0x0000000F)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_VC_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_FMT_CMDTYPE_OFF                ( 4)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_FMT_CMDTYPE_WID                ( 6)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_FMT_CMDTYPE_MSK                (0x000003F0)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_FMT_CMDTYPE_MAX                (0x0000003F)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_FMT_CMDTYPE_DEF                (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_TC_OFF                         (10)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_TC_WID                         ( 4)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_TC_MSK                         (0x00003C00)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_TC_MAX                         (0x0000000F)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_TC_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_CHAIN_OFF                      (14)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_CHAIN_WID                      ( 1)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_CHAIN_MSK                      (0x00004000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_CHAIN_MAX                      (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_CHAIN_DEF                      (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_NS_OFF                         (15)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_NS_WID                         ( 1)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_NS_MSK                         (0x00008000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_NS_MAX                         (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_NS_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_RO_OFF                         (16)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_RO_WID                         ( 1)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_RO_MSK                         (0x00010000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_RO_MAX                         (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_RO_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_LENGTH_OFF                     (17)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_LENGTH_WID                     ( 5)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_LENGTH_MSK                     (0x003E0000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_LENGTH_MAX                     (0x0000001F)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_LENGTH_DEF                     (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_EP_OFF                         (22)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_EP_WID                         ( 1)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_EP_MSK                         (0x00400000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_EP_MAX                         (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_EP_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_AT_OFF                         (23)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_AT_WID                         ( 2)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_AT_MSK                         (0x01800000)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_AT_MAX                         (0x00000003)
  #define MPVTDTRK_CR_PRIMUP_MASK1_IMPH_AT_DEF                         (0x00000000)

#define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_REG                            (0x00006054)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_RQID_OFF                       ( 0)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_RQID_WID                       (16)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_RQID_MSK                       (0x0000FFFF)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_RQID_MAX                       (0x0000FFFF)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_RQID_DEF                       (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_TAG_OFF                        (16)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_TAG_WID                        ( 8)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_TAG_MSK                        (0x00FF0000)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_TAG_MAX                        (0x000000FF)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_TAG_DEF                        (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_LBEFBE_MSGTYPE_OFF             (24)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_LBEFBE_MSGTYPE_WID             ( 8)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_LBEFBE_MSGTYPE_MSK             (0xFF000000)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_LBEFBE_MSGTYPE_MAX             (0x000000FF)
  #define MPVTDTRK_CR_PRIMUP_MASK2_IMPH_LBEFBE_MSGTYPE_DEF             (0x00000000)

#define MPVTDTRK_CR_PRIMUP_MASK3_IMPH_REG                            (0x00006058)
  #define MPVTDTRK_CR_PRIMUP_MASK3_IMPH_ADDR_31_0_OFF                  ( 0)
  #define MPVTDTRK_CR_PRIMUP_MASK3_IMPH_ADDR_31_0_WID                  (32)
  #define MPVTDTRK_CR_PRIMUP_MASK3_IMPH_ADDR_31_0_MSK                  (0xFFFFFFFF)
  #define MPVTDTRK_CR_PRIMUP_MASK3_IMPH_ADDR_31_0_MAX                  (0xFFFFFFFF)
  #define MPVTDTRK_CR_PRIMUP_MASK3_IMPH_ADDR_31_0_DEF                  (0x00000000)

#define MPVTDTRK_CR_PRIMUP_MASK4_IMPH_REG                            (0x0000605C)
  #define MPVTDTRK_CR_PRIMUP_MASK4_IMPH_ADDR_63_32_OFF                 ( 0)
  #define MPVTDTRK_CR_PRIMUP_MASK4_IMPH_ADDR_63_32_WID                 (32)
  #define MPVTDTRK_CR_PRIMUP_MASK4_IMPH_ADDR_63_32_MSK                 (0xFFFFFFFF)
  #define MPVTDTRK_CR_PRIMUP_MASK4_IMPH_ADDR_63_32_MAX                 (0xFFFFFFFF)
  #define MPVTDTRK_CR_PRIMUP_MASK4_IMPH_ADDR_63_32_DEF                 (0x00000000)

#define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_REG                            (0x00006060)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_VC_OFF                         ( 0)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_VC_WID                         ( 4)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_VC_MSK                         (0x0000000F)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_VC_MAX                         (0x0000000F)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_VC_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_FMT_CMDTYPE_OFF                ( 4)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_FMT_CMDTYPE_WID                ( 6)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_FMT_CMDTYPE_MSK                (0x000003F0)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_FMT_CMDTYPE_MAX                (0x0000003F)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_FMT_CMDTYPE_DEF                (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_TC_OFF                         (10)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_TC_WID                         ( 4)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_TC_MSK                         (0x00003C00)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_TC_MAX                         (0x0000000F)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_TC_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_CHAIN_OFF                      (14)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_CHAIN_WID                      ( 1)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_CHAIN_MSK                      (0x00004000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_CHAIN_MAX                      (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_CHAIN_DEF                      (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_NS_OFF                         (15)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_NS_WID                         ( 1)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_NS_MSK                         (0x00008000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_NS_MAX                         (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_NS_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_RO_OFF                         (16)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_RO_WID                         ( 1)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_RO_MSK                         (0x00010000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_RO_MAX                         (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_RO_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_LENGTH_OFF                     (17)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_LENGTH_WID                     ( 5)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_LENGTH_MSK                     (0x003E0000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_LENGTH_MAX                     (0x0000001F)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_LENGTH_DEF                     (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_EP_OFF                         (22)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_EP_WID                         ( 1)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_EP_MSK                         (0x00400000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_EP_MAX                         (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_EP_DEF                         (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_AT_OFF                         (23)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_AT_WID                         ( 2)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_AT_MSK                         (0x01800000)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_AT_MAX                         (0x00000003)
  #define MPVTDTRK_CR_PRIMUP_COMP1_IMPH_AT_DEF                         (0x00000000)

#define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_REG                            (0x00006064)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_RQID_OFF                       ( 0)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_RQID_WID                       (16)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_RQID_MSK                       (0x0000FFFF)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_RQID_MAX                       (0x0000FFFF)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_RQID_DEF                       (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_TAG_OFF                        (16)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_TAG_WID                        ( 8)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_TAG_MSK                        (0x00FF0000)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_TAG_MAX                        (0x000000FF)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_TAG_DEF                        (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_LBEFBE_MSGTYPE_OFF             (24)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_LBEFBE_MSGTYPE_WID             ( 8)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_LBEFBE_MSGTYPE_MSK             (0xFF000000)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_LBEFBE_MSGTYPE_MAX             (0x000000FF)
  #define MPVTDTRK_CR_PRIMUP_COMP2_IMPH_LBEFBE_MSGTYPE_DEF             (0x00000000)

#define MPVTDTRK_CR_PRIMUP_COMP3_IMPH_REG                            (0x00006068)
  #define MPVTDTRK_CR_PRIMUP_COMP3_IMPH_ADDR_31_0_OFF                  ( 0)
  #define MPVTDTRK_CR_PRIMUP_COMP3_IMPH_ADDR_31_0_WID                  (32)
  #define MPVTDTRK_CR_PRIMUP_COMP3_IMPH_ADDR_31_0_MSK                  (0xFFFFFFFF)
  #define MPVTDTRK_CR_PRIMUP_COMP3_IMPH_ADDR_31_0_MAX                  (0xFFFFFFFF)
  #define MPVTDTRK_CR_PRIMUP_COMP3_IMPH_ADDR_31_0_DEF                  (0x00000000)

#define MPVTDTRK_CR_PRIMUP_COMP4_IMPH_REG                            (0x0000606C)
  #define MPVTDTRK_CR_PRIMUP_COMP4_IMPH_ADDR_63_32_OFF                 ( 0)
  #define MPVTDTRK_CR_PRIMUP_COMP4_IMPH_ADDR_63_32_WID                 (32)
  #define MPVTDTRK_CR_PRIMUP_COMP4_IMPH_ADDR_63_32_MSK                 (0xFFFFFFFF)
  #define MPVTDTRK_CR_PRIMUP_COMP4_IMPH_ADDR_63_32_MAX                 (0xFFFFFFFF)
  #define MPVTDTRK_CR_PRIMUP_COMP4_IMPH_ADDR_63_32_DEF                 (0x00000000)

#define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_REG                          (0x00006070)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_ENABLE_OFF                   ( 0)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_ENABLE_WID                   ( 1)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_ENABLE_MSK                   (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_ENABLE_MAX                   (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_ENABLE_DEF                   (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_TRIGGERED_OFF                ( 1)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_TRIGGERED_WID                ( 1)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_TRIGGERED_MSK                (0x00000002)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_TRIGGERED_MAX                (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_TRIGGERED_DEF                (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DNARB_OFF              ( 2)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DNARB_WID              ( 1)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DNARB_MSK              (0x00000004)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DNARB_MAX              (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DNARB_DEF              (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_UPARB_OFF              ( 3)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_UPARB_WID              ( 1)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_UPARB_MSK              (0x00000008)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_UPARB_MAX              (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_UPARB_DEF              (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_SNPARB_OFF             ( 4)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_SNPARB_WID             ( 1)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_SNPARB_MSK             (0x00000010)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_SNPARB_MAX             (0x00000001)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_SNPARB_DEF             (0x00000000)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DELAY_OFF              (23)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DELAY_WID              ( 9)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DELAY_MSK              (0xFF800000)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DELAY_MAX              (0x000001FF)
  #define MPVTDTRK_CR_PRIMUP_TRIGGER_IMPH_STALL_DELAY_DEF              (0x00000000)

#define MPVTDTRK_CR_HCTL0_IMPH_REG                                   (0x00006100)
  #define MPVTDTRK_CR_HCTL0_IMPH_PEG2DMIDIS_OFF                        ( 0)
  #define MPVTDTRK_CR_HCTL0_IMPH_PEG2DMIDIS_WID                        ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_PEG2DMIDIS_MSK                        (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_PEG2DMIDIS_MAX                        (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_PEG2DMIDIS_DEF                        (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_EOIB_OFF                              ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_EOIB_WID                              ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_EOIB_MSK                              (0x00000002)
  #define MPVTDTRK_CR_HCTL0_IMPH_EOIB_MAX                              (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_EOIB_DEF                              (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_MSIBYPDIS_OFF                         ( 2)
  #define MPVTDTRK_CR_HCTL0_IMPH_MSIBYPDIS_WID                         ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_MSIBYPDIS_MSK                         (0x00000004)
  #define MPVTDTRK_CR_HCTL0_IMPH_MSIBYPDIS_MAX                         (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_MSIBYPDIS_DEF                         (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDDIS_OFF                           ( 4)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDDIS_WID                           ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDDIS_MSK                           (0x00000010)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDDIS_MAX                           (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDDIS_DEF                           (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_BKSNPDIS_OFF                          ( 6)
  #define MPVTDTRK_CR_HCTL0_IMPH_BKSNPDIS_WID                          ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_BKSNPDIS_MSK                          (0x00000040)
  #define MPVTDTRK_CR_HCTL0_IMPH_BKSNPDIS_MAX                          (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_BKSNPDIS_DEF                          (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVC0SNP_OFF                         ( 7)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVC0SNP_WID                         ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVC0SNP_MSK                         (0x00000080)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVC0SNP_MAX                         (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVC0SNP_DEF                         (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVCPSNP_OFF                         ( 8)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVCPSNP_WID                         ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVCPSNP_MSK                         (0x00000100)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVCPSNP_MAX                         (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_FRCVCPSNP_DEF                         (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDBLKDIS_OFF                        ( 9)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDBLKDIS_WID                        ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDBLKDIS_MSK                        (0x00000200)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDBLKDIS_MAX                        (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_PHLDBLKDIS_DEF                        (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_BLKWRPOSTVC1_OFF                      (10)
  #define MPVTDTRK_CR_HCTL0_IMPH_BLKWRPOSTVC1_WID                      ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_BLKWRPOSTVC1_MSK                      (0x00000400)
  #define MPVTDTRK_CR_HCTL0_IMPH_BLKWRPOSTVC1_MAX                      (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_BLKWRPOSTVC1_DEF                      (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_DIS_VLW_PEG_OFF                       (11)
  #define MPVTDTRK_CR_HCTL0_IMPH_DIS_VLW_PEG_WID                       ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_DIS_VLW_PEG_MSK                       (0x00000800)
  #define MPVTDTRK_CR_HCTL0_IMPH_DIS_VLW_PEG_MAX                       (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_DIS_VLW_PEG_DEF                       (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_SPECRDDIS_OFF                         (12)
  #define MPVTDTRK_CR_HCTL0_IMPH_SPECRDDIS_WID                         ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_SPECRDDIS_MSK                         (0x00001000)
  #define MPVTDTRK_CR_HCTL0_IMPH_SPECRDDIS_MAX                         (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_SPECRDDIS_DEF                         (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_IR_RSRV_CTL_OFF                       (13)
  #define MPVTDTRK_CR_HCTL0_IMPH_IR_RSRV_CTL_WID                       ( 1)
  #define MPVTDTRK_CR_HCTL0_IMPH_IR_RSRV_CTL_MSK                       (0x00002000)
  #define MPVTDTRK_CR_HCTL0_IMPH_IR_RSRV_CTL_MAX                       (0x00000001)
  #define MPVTDTRK_CR_HCTL0_IMPH_IR_RSRV_CTL_DEF                       (0x00000000)
  #define MPVTDTRK_CR_HCTL0_IMPH_RSVD_OFF                              (14)
  #define MPVTDTRK_CR_HCTL0_IMPH_RSVD_WID                              (18)
  #define MPVTDTRK_CR_HCTL0_IMPH_RSVD_MSK                              (0xFFFFC000)
  #define MPVTDTRK_CR_HCTL0_IMPH_RSVD_MAX                              (0x0003FFFF)
  #define MPVTDTRK_CR_HCTL0_IMPH_RSVD_DEF                              (0x00000000)

#define MPVTDTRK_CR_REGBAR_IMPH_REG                                  (0x00006110)
  #define MPVTDTRK_CR_REGBAR_IMPH_REGBAR_OFF                           ( 4)
  #define MPVTDTRK_CR_REGBAR_IMPH_REGBAR_WID                           (35)
  #define MPVTDTRK_CR_REGBAR_IMPH_REGBAR_MSK                           (0x7FFFFFFFF0)
  #define MPVTDTRK_CR_REGBAR_IMPH_REGBAR_MAX                           (0x7FFFFFFFF)
  #define MPVTDTRK_CR_REGBAR_IMPH_REGBAR_DEF                           (0x00000000)

#define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_REG              (0x00006200)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_STATUS_OFF ( 0)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_STATUS_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_STATUS_MSK (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_LOG_OFF ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_LOG_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_LOG_MSK (0x00000002)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THERMAL_MONITOR_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_STATUS_OFF ( 2)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_STATUS_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_STATUS_MSK (0x00000004)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_LOG_OFF  ( 3)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_LOG_WID  ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_LOG_MSK  (0x00000008)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_LOG_MAX  (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_PROCHOT_LOG_DEF  (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_STATUS_OFF ( 4)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_STATUS_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_STATUS_MSK (0x00000010)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_LOG_OFF ( 5)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_LOG_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_LOG_MSK (0x00000020)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_OUT_OF_SPEC_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_OFF ( 6)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_MSK (0x00000040)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_OFF ( 7)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_MSK (0x00000080)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_OFF ( 8)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_MSK (0x00000100)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_OFF ( 9)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_MSK (0x00000200)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_STATUS_OFF (10)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_STATUS_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_STATUS_MSK (0x00000400)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_LOG_OFF (11)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_LOG_WID ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_LOG_MSK (0x00000800)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_POWER_LIMITATION_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Temperature_OFF  (16)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Temperature_WID  ( 7)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Temperature_MSK  (0x007F0000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Temperature_MAX  (0x0000007F)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Temperature_DEF  (0x00000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Resolution_OFF   (27)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Resolution_WID   ( 4)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Resolution_MSK   (0x78000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Resolution_MAX   (0x0000000F)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Resolution_DEF   (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Valid_OFF        (31)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Valid_WID        ( 1)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Valid_MSK        (0x80000000)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Valid_MAX        (0x00000001)
  #define MPVTDTRK_CR_PKG_THERM_CAMARILLO_STATUS_IMPH_Valid_DEF        (0x00000000)

#define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REG              (0x00006204)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_STATUS_OFF ( 0)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_STATUS_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_STATUS_MSK (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_LOG_OFF ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_LOG_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_LOG_MSK (0x00000002)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_WARM_THRESHOLD_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_STATUS_OFF ( 2)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_STATUS_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_STATUS_MSK (0x00000004)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_LOG_OFF ( 3)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_LOG_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_LOG_MSK (0x00000008)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_HOT_THRESHOLD_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_STATUS_OFF ( 4)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_STATUS_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_STATUS_MSK (0x00000010)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_LOG_OFF ( 5)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_LOG_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_LOG_MSK (0x00000020)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_REFRESH2X_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_STATUS_OFF ( 6)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_STATUS_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_STATUS_MSK (0x00000040)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_LOG_OFF ( 7)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_LOG_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_LOG_MSK (0x00000080)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_FORCEMEMPR_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_OFF ( 8)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_MSK (0x00000100)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_OFF ( 9)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_MSK (0x00000200)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD1_LOG_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_OFF (10)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_MSK (0x00000400)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_STATUS_DEF (0x00000000)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_OFF (11)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_WID ( 1)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_MSK (0x00000800)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_MAX (0x00000001)
  #define MPVTDTRK_CR_DDR_THERM_CAMARILLO_STATUS_IMPH_THRESHOLD2_LOG_DEF (0x00000000)

#define MPVTDTRK_CR_VTDTRKLCK_IMPH_REG                               (0x000063FC)
  #define MPVTDTRK_CR_VTDTRKLCK_IMPH_LOCK_OFF                          ( 0)
  #define MPVTDTRK_CR_VTDTRKLCK_IMPH_LOCK_WID                          ( 1)
  #define MPVTDTRK_CR_VTDTRKLCK_IMPH_LOCK_MSK                          (0x00000001)
  #define MPVTDTRK_CR_VTDTRKLCK_IMPH_LOCK_MAX                          (0x00000001)
  #define MPVTDTRK_CR_VTDTRKLCK_IMPH_LOCK_DEF                          (0x00000000)

#define MPCBOTRK_CR_REQLIM_IMPH_REG                                  (0x00006800)
  #define MPCBOTRK_CR_REQLIM_IMPH_RDLIM_OFF                            ( 0)
  #define MPCBOTRK_CR_REQLIM_IMPH_RDLIM_WID                            ( 3)
  #define MPCBOTRK_CR_REQLIM_IMPH_RDLIM_MSK                            (0x00000007)
  #define MPCBOTRK_CR_REQLIM_IMPH_RDLIM_MAX                            (0x00000007)
  #define MPCBOTRK_CR_REQLIM_IMPH_RDLIM_DEF                            (0x00000004)
  #define MPCBOTRK_CR_REQLIM_IMPH_WRLIM_OFF                            ( 4)
  #define MPCBOTRK_CR_REQLIM_IMPH_WRLIM_WID                            ( 3)
  #define MPCBOTRK_CR_REQLIM_IMPH_WRLIM_MSK                            (0x00000070)
  #define MPCBOTRK_CR_REQLIM_IMPH_WRLIM_MAX                            (0x00000007)
  #define MPCBOTRK_CR_REQLIM_IMPH_WRLIM_DEF                            (0x00000004)
  #define MPCBOTRK_CR_REQLIM_IMPH_LOCK_OFF                             (31)
  #define MPCBOTRK_CR_REQLIM_IMPH_LOCK_WID                             ( 1)
  #define MPCBOTRK_CR_REQLIM_IMPH_LOCK_MSK                             (0x80000000)
  #define MPCBOTRK_CR_REQLIM_IMPH_LOCK_MAX                             (0x00000001)
  #define MPCBOTRK_CR_REQLIM_IMPH_LOCK_DEF                             (0x00000000)

#define HUBS_CR_DMIVCLIM_HUBS_REG                                    (0x00007000)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPNPLIM_OFF                           ( 0)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPNPLIM_WID                           ( 3)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPNPLIM_MSK                           (0x00000007)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPNPLIM_MAX                           (0x00000007)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPNPLIM_DEF                           (0x00000004)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPPLIM_OFF                            ( 4)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPPLIM_WID                            ( 3)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPPLIM_MSK                            (0x00000070)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPPLIM_MAX                            (0x00000007)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPPLIM_DEF                            (0x00000004)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMNPLIM_OFF                           ( 8)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMNPLIM_WID                           ( 3)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMNPLIM_MSK                           (0x00000700)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMNPLIM_MAX                           (0x00000007)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMNPLIM_DEF                           (0x00000004)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMPLIM_OFF                            (12)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMPLIM_WID                            ( 3)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMPLIM_MSK                            (0x00007000)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMPLIM_MAX                            (0x00000007)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMPLIM_DEF                            (0x00000004)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPCMPLIM_OFF                          (16)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPCMPLIM_WID                          ( 3)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPCMPLIM_MSK                          (0x00070000)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPCMPLIM_MAX                          (0x00000007)
  #define HUBS_CR_DMIVCLIM_HUBS_VCPCMPLIM_DEF                          (0x00000004)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMCMPLIM_OFF                          (20)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMCMPLIM_WID                          ( 3)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMCMPLIM_MSK                          (0x00700000)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMCMPLIM_MAX                          (0x00000007)
  #define HUBS_CR_DMIVCLIM_HUBS_VCMCMPLIM_DEF                          (0x00000004)
  #define HUBS_CR_DMIVCLIM_HUBS_LOCK_OFF                               (31)
  #define HUBS_CR_DMIVCLIM_HUBS_LOCK_WID                               ( 1)
  #define HUBS_CR_DMIVCLIM_HUBS_LOCK_MSK                               (0x80000000)
  #define HUBS_CR_DMIVCLIM_HUBS_LOCK_MAX                               (0x00000001)
  #define HUBS_CR_DMIVCLIM_HUBS_LOCK_DEF                               (0x00000000)

#define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_REG                             (0x00007010)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P0_OFF                          ( 0)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P0_WID                          ( 1)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P0_MSK                          (0x00000001)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P0_MAX                          (0x00000001)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P0_DEF                          (0x00000000)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P1_OFF                          ( 1)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P1_WID                          ( 1)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P1_MSK                          (0x00000002)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P1_MAX                          (0x00000001)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P1_DEF                          (0x00000000)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P2_OFF                          ( 2)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P2_WID                          ( 1)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P2_MSK                          (0x00000004)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P2_MAX                          (0x00000001)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P2_DEF                          (0x00000000)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P3_OFF                          ( 3)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P3_WID                          ( 1)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P3_MSK                          (0x00000008)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P3_MAX                          (0x00000001)
  #define HUBS_CR_HUB0_PWRDN_OVRD_HUBS_P3_DEF                          (0x00000000)

#define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_REG                             (0x00007014)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P0_OFF                          ( 0)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P0_WID                          ( 1)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P0_MSK                          (0x00000001)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P0_MAX                          (0x00000001)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P0_DEF                          (0x00000000)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P1_OFF                          ( 1)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P1_WID                          ( 1)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P1_MSK                          (0x00000002)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P1_MAX                          (0x00000001)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P1_DEF                          (0x00000000)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P2_OFF                          ( 2)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P2_WID                          ( 1)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P2_MSK                          (0x00000004)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P2_MAX                          (0x00000001)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P2_DEF                          (0x00000000)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P3_OFF                          ( 3)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P3_WID                          ( 1)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P3_MSK                          (0x00000008)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P3_MAX                          (0x00000001)
  #define HUBS_CR_HUB1_PWRDN_OVRD_HUBS_P3_DEF                          (0x00000000)

#define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_REG                             (0x00007018)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P0_OFF                          ( 0)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P0_WID                          ( 1)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P0_MSK                          (0x00000001)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P0_MAX                          (0x00000001)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P0_DEF                          (0x00000000)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P1_OFF                          ( 1)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P1_WID                          ( 1)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P1_MSK                          (0x00000002)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P1_MAX                          (0x00000001)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P1_DEF                          (0x00000000)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P2_OFF                          ( 2)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P2_WID                          ( 1)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P2_MSK                          (0x00000004)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P2_MAX                          (0x00000001)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P2_DEF                          (0x00000000)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P3_OFF                          ( 3)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P3_WID                          ( 1)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P3_MSK                          (0x00000008)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P3_MAX                          (0x00000001)
  #define HUBS_CR_HUB2_PWRDN_OVRD_HUBS_P3_DEF                          (0x00000000)

#define HUBS_CR_HUB0_DEFEATURE_HUBS_REG                              (0x0000701C)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P0_OFF                   ( 0)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P0_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P0_MSK                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P1_OFF                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P1_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P1_MSK                   (0x00000002)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P2_OFF                   ( 2)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P2_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P2_MSK                   (0x00000004)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_GNT_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P0_OFF                   ( 3)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P0_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P0_MSK                   (0x00000008)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P1_OFF                   ( 4)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P1_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P1_MSK                   (0x00000010)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P2_OFF                   ( 5)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P2_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P2_MSK                   (0x00000020)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P3_OFF                   ( 6)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P3_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P3_MSK                   (0x00000040)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P3_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_BLK_PUT_P3_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P0_OFF                  ( 7)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P0_WID                  ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P0_MSK                  (0x00000080)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P0_MAX                  (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P0_DEF                  (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P1_OFF                  ( 8)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P1_WID                  ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P1_MSK                  (0x00000100)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P1_MAX                  (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P1_DEF                  (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P2_OFF                  ( 9)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P2_WID                  ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P2_MSK                  (0x00000200)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P2_MAX                  (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_NO_CHAIN_P2_DEF                  (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P0_OFF                   (10)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P0_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P0_MSK                   (0x00000400)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P1_OFF                   (11)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P1_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P1_MSK                   (0x00000800)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P2_OFF                   (12)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P2_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P2_MSK                   (0x00001000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_UP_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P0_OFF                   (13)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P0_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P0_MSK                   (0x00002000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P1_OFF                   (14)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P1_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P1_MSK                   (0x00004000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P2_OFF                   (15)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P2_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P2_MSK                   (0x00008000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P3_OFF                   (16)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P3_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P3_MSK                   (0x00010000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P3_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOW_DN_P3_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOWER_CMD_OFF                   (17)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOWER_CMD_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOWER_CMD_MSK                   (0x00020000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOWER_CMD_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_SLOWER_CMD_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_DMI_NOPUSH_OFF                   (18)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_DMI_NOPUSH_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_DMI_NOPUSH_MSK                   (0x00040000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_DMI_NOPUSH_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_DMI_NOPUSH_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RO_PASS_NP_OFF                   (19)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RO_PASS_NP_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RO_PASS_NP_MSK                   (0x00080000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RO_PASS_NP_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RO_PASS_NP_DEF                   (0x00000000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RST_CRD_P3_OFF                   (20)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RST_CRD_P3_WID                   ( 1)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RST_CRD_P3_MSK                   (0x00100000)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RST_CRD_P3_MAX                   (0x00000001)
  #define HUBS_CR_HUB0_DEFEATURE_HUBS_RST_CRD_P3_DEF                   (0x00000000)

#define HUBS_CR_HUB1_DEFEATURE_HUBS_REG                              (0x00007020)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P1_OFF                   ( 0)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P1_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P1_MSK                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P2_OFF                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P2_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P2_MSK                   (0x00000002)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_GNT_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P0_OFF                   ( 2)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P0_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P0_MSK                   (0x00000004)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P1_OFF                   ( 3)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P1_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P1_MSK                   (0x00000008)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P2_OFF                   ( 4)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P2_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P2_MSK                   (0x00000010)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P3_OFF                   ( 5)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P3_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P3_MSK                   (0x00000020)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P3_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_BLK_PUT_P3_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P1_OFF                  ( 6)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P1_WID                  ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P1_MSK                  (0x00000040)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P1_MAX                  (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P1_DEF                  (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P2_OFF                  ( 7)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P2_WID                  ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P2_MSK                  (0x00000080)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P2_MAX                  (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_NO_CHAIN_P2_DEF                  (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P0_OFF                   ( 8)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P0_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P0_MSK                   (0x00000100)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P1_OFF                   ( 9)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P1_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P1_MSK                   (0x00000200)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P2_OFF                   (10)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P2_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P2_MSK                   (0x00000400)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_UP_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P1_OFF                   (11)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P1_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P1_MSK                   (0x00000800)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P2_OFF                   (12)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P2_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P2_MSK                   (0x00001000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P3_OFF                   (13)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P3_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P3_MSK                   (0x00002000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P3_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOW_DN_P3_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOWER_CMD_OFF                   (14)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOWER_CMD_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOWER_CMD_MSK                   (0x00004000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOWER_CMD_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_SLOWER_CMD_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RO_PASS_NP_OFF                   (15)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RO_PASS_NP_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RO_PASS_NP_MSK                   (0x00008000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RO_PASS_NP_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RO_PASS_NP_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P0_OFF                   (16)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P0_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P0_MSK                   (0x00010000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P3_OFF                   (17)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P3_WID                   ( 1)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P3_MSK                   (0x00020000)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P3_MAX                   (0x00000001)
  #define HUBS_CR_HUB1_DEFEATURE_HUBS_RST_CRD_P3_DEF                   (0x00000000)

#define HUBS_CR_HUB2_DEFEATURE_HUBS_REG                              (0x00007024)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P1_OFF                   ( 0)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P1_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P1_MSK                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P2_OFF                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P2_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P2_MSK                   (0x00000002)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_GNT_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P0_OFF                   ( 2)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P0_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P0_MSK                   (0x00000004)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P1_OFF                   ( 3)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P1_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P1_MSK                   (0x00000008)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P2_OFF                   ( 4)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P2_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P2_MSK                   (0x00000010)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_BLK_PUT_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P1_OFF                  ( 5)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P1_WID                  ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P1_MSK                  (0x00000020)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P1_MAX                  (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P1_DEF                  (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P2_OFF                  ( 6)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P2_WID                  ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P2_MSK                  (0x00000040)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P2_MAX                  (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_NO_CHAIN_P2_DEF                  (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P0_OFF                   ( 7)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P0_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P0_MSK                   (0x00000080)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P0_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P1_OFF                   ( 8)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P1_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P1_MSK                   (0x00000100)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P2_OFF                   ( 9)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P2_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P2_MSK                   (0x00000200)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_UP_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P1_OFF                   (10)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P1_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P1_MSK                   (0x00000400)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P1_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P1_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P2_OFF                   (11)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P2_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P2_MSK                   (0x00000800)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P2_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOW_DN_P2_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOWER_CMD_OFF                   (12)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOWER_CMD_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOWER_CMD_MSK                   (0x00001000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOWER_CMD_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_SLOWER_CMD_DEF                   (0x00000000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_RST_CRD_P0_OFF                   (13)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_RST_CRD_P0_WID                   ( 1)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_RST_CRD_P0_MSK                   (0x00002000)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_RST_CRD_P0_MAX                   (0x00000001)
  #define HUBS_CR_HUB2_DEFEATURE_HUBS_RST_CRD_P0_DEF                   (0x00000000)

#define HUBS_CR_PEGCTL_HUBS_REG                                      (0x00007028)
  #define HUBS_CR_PEGCTL_HUBS_PCIPWRGAT_OFF                            ( 0)
  #define HUBS_CR_PEGCTL_HUBS_PCIPWRGAT_WID                            ( 1)
  #define HUBS_CR_PEGCTL_HUBS_PCIPWRGAT_MSK                            (0x00000001)
  #define HUBS_CR_PEGCTL_HUBS_PCIPWRGAT_MAX                            (0x00000001)
  #define HUBS_CR_PEGCTL_HUBS_PCIPWRGAT_DEF                            (0x00000000)

#define HUBS_CR_HUB_EMPTY_HUBS_REG                                   (0x0000702C)
  #define HUBS_CR_HUB_EMPTY_HUBS_H0_EMPTY_OFF                          ( 0)
  #define HUBS_CR_HUB_EMPTY_HUBS_H0_EMPTY_WID                          ( 1)
  #define HUBS_CR_HUB_EMPTY_HUBS_H0_EMPTY_MSK                          (0x00000001)
  #define HUBS_CR_HUB_EMPTY_HUBS_H0_EMPTY_MAX                          (0x00000001)
  #define HUBS_CR_HUB_EMPTY_HUBS_H0_EMPTY_DEF                          (0x00000001)
  #define HUBS_CR_HUB_EMPTY_HUBS_H1_EMPTY_OFF                          ( 1)
  #define HUBS_CR_HUB_EMPTY_HUBS_H1_EMPTY_WID                          ( 1)
  #define HUBS_CR_HUB_EMPTY_HUBS_H1_EMPTY_MSK                          (0x00000002)
  #define HUBS_CR_HUB_EMPTY_HUBS_H1_EMPTY_MAX                          (0x00000001)
  #define HUBS_CR_HUB_EMPTY_HUBS_H1_EMPTY_DEF                          (0x00000001)
  #define HUBS_CR_HUB_EMPTY_HUBS_H2_EMPTY_OFF                          ( 2)
  #define HUBS_CR_HUB_EMPTY_HUBS_H2_EMPTY_WID                          ( 1)
  #define HUBS_CR_HUB_EMPTY_HUBS_H2_EMPTY_MSK                          (0x00000004)
  #define HUBS_CR_HUB_EMPTY_HUBS_H2_EMPTY_MAX                          (0x00000001)
  #define HUBS_CR_HUB_EMPTY_HUBS_H2_EMPTY_DEF                          (0x00000001)

#define HUBS_CR_HUB0_STATUS_HUBS_REG                                 (0x00007030)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P0_OFF                      ( 0)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P0_WID                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P0_MSK                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P0_MAX                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P0_DEF                      (0x00000000)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P1_OFF                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P1_WID                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P1_MSK                      (0x00000002)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P1_MAX                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P1_DEF                      (0x00000000)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P2_OFF                      ( 2)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P2_WID                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P2_MSK                      (0x00000004)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P2_MAX                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P2_DEF                      (0x00000000)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P3_OFF                      ( 3)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P3_WID                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P3_MSK                      (0x00000008)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P3_MAX                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_REQ_VLD_P3_DEF                      (0x00000000)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P0_OFF                      ( 4)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P0_WID                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P0_MSK                      (0x00000010)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P0_MAX                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P0_DEF                      (0x00000000)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P1_OFF                      ( 5)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P1_WID                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P1_MSK                      (0x00000020)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P1_MAX                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P1_DEF                      (0x00000000)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P2_OFF                      ( 6)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P2_WID                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P2_MSK                      (0x00000040)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P2_MAX                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P2_DEF                      (0x00000000)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P3_OFF                      ( 7)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P3_WID                      ( 1)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P3_MSK                      (0x00000080)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P3_MAX                      (0x00000001)
  #define HUBS_CR_HUB0_STATUS_HUBS_TNX_VLD_P3_DEF                      (0x00000000)

#define HUBS_CR_HUB1_STATUS_HUBS_REG                                 (0x00007034)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P0_OFF                      ( 0)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P0_WID                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P0_MSK                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P0_MAX                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P0_DEF                      (0x00000000)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P1_OFF                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P1_WID                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P1_MSK                      (0x00000002)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P1_MAX                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P1_DEF                      (0x00000000)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P2_OFF                      ( 2)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P2_WID                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P2_MSK                      (0x00000004)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P2_MAX                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P2_DEF                      (0x00000000)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P3_OFF                      ( 3)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P3_WID                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P3_MSK                      (0x00000008)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P3_MAX                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_REQ_VLD_P3_DEF                      (0x00000000)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P0_OFF                      ( 4)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P0_WID                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P0_MSK                      (0x00000010)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P0_MAX                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P0_DEF                      (0x00000000)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P1_OFF                      ( 5)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P1_WID                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P1_MSK                      (0x00000020)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P1_MAX                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P1_DEF                      (0x00000000)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P2_OFF                      ( 6)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P2_WID                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P2_MSK                      (0x00000040)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P2_MAX                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P2_DEF                      (0x00000000)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P3_OFF                      ( 7)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P3_WID                      ( 1)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P3_MSK                      (0x00000080)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P3_MAX                      (0x00000001)
  #define HUBS_CR_HUB1_STATUS_HUBS_TNX_VLD_P3_DEF                      (0x00000000)

#define HUBS_CR_HUB2_STATUS_HUBS_REG                                 (0x00007038)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P0_OFF                      ( 0)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P0_WID                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P0_MSK                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P0_MAX                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P0_DEF                      (0x00000000)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P1_OFF                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P1_WID                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P1_MSK                      (0x00000002)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P1_MAX                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P1_DEF                      (0x00000000)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P2_OFF                      ( 2)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P2_WID                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P2_MSK                      (0x00000004)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P2_MAX                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P2_DEF                      (0x00000000)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P3_OFF                      ( 3)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P3_WID                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P3_MSK                      (0x00000008)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P3_MAX                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_REQ_VLD_P3_DEF                      (0x00000000)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P0_OFF                      ( 4)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P0_WID                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P0_MSK                      (0x00000010)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P0_MAX                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P0_DEF                      (0x00000000)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P1_OFF                      ( 5)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P1_WID                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P1_MSK                      (0x00000020)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P1_MAX                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P1_DEF                      (0x00000000)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P2_OFF                      ( 6)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P2_WID                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P2_MSK                      (0x00000040)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P2_MAX                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P2_DEF                      (0x00000000)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P3_OFF                      ( 7)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P3_WID                      ( 1)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P3_MSK                      (0x00000080)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P3_MAX                      (0x00000001)
  #define HUBS_CR_HUB2_STATUS_HUBS_TNX_VLD_P3_DEF                      (0x00000000)

#define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_REG                           (0x00007100)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_Data_OFF                      ( 0)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_Data_WID                      (18)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_Data_MSK                      (0x0003FFFF)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_Data_MAX                      (0x0003FFFF)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_Data_DEF                      (0x00000000)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_VORANGE_OFF                   (31)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_VORANGE_WID                   ( 1)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_VORANGE_MSK                   (0x80000000)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_VORANGE_MAX                   (0x00000001)
  #define HUBS_CR_VISA_CTL_SABHUB0S_HUBS_VORANGE_DEF                   (0x00000000)

#define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_REG                           (0x00007110)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_Data_OFF                      ( 0)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_Data_WID                      (18)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_Data_MSK                      (0x0003FFFF)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_Data_MAX                      (0x0003FFFF)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_Data_DEF                      (0x00000000)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_VORANGE_OFF                   (31)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_VORANGE_WID                   ( 1)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_VORANGE_MSK                   (0x80000000)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_VORANGE_MAX                   (0x00000001)
  #define HUBS_CR_VISA_CTL_SABHUB1S_HUBS_VORANGE_DEF                   (0x00000000)

#define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_REG                           (0x00007120)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_Data_OFF                      ( 0)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_Data_WID                      (18)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_Data_MSK                      (0x0003FFFF)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_Data_MAX                      (0x0003FFFF)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_Data_DEF                      (0x00000000)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_VORANGE_OFF                   (31)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_VORANGE_WID                   ( 1)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_VORANGE_MSK                   (0x80000000)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_VORANGE_MAX                   (0x00000001)
  #define HUBS_CR_VISA_CTL_SABHUB2S_HUBS_VORANGE_DEF                   (0x00000000)

#define HUBS_CR_MBOX_WR_DATA_HUBS_REG                                (0x00007124)
  #define HUBS_CR_MBOX_WR_DATA_HUBS_DATA_OFF                           ( 0)
  #define HUBS_CR_MBOX_WR_DATA_HUBS_DATA_WID                           (32)
  #define HUBS_CR_MBOX_WR_DATA_HUBS_DATA_MSK                           (0xFFFFFFFF)
  #define HUBS_CR_MBOX_WR_DATA_HUBS_DATA_MAX                           (0xFFFFFFFF)
  #define HUBS_CR_MBOX_WR_DATA_HUBS_DATA_DEF                           (0x00000000)

#define HUBS_CR_MBOX_RD_DATA_HUBS_REG                                (0x00007128)
  #define HUBS_CR_MBOX_RD_DATA_HUBS_DATA_OFF                           ( 0)
  #define HUBS_CR_MBOX_RD_DATA_HUBS_DATA_WID                           (32)
  #define HUBS_CR_MBOX_RD_DATA_HUBS_DATA_MSK                           (0xFFFFFFFF)
  #define HUBS_CR_MBOX_RD_DATA_HUBS_DATA_MAX                           (0xFFFFFFFF)
  #define HUBS_CR_MBOX_RD_DATA_HUBS_DATA_DEF                           (0x00000000)

#define HUBS_CR_MBOX_ADDR_LO_HUBS_REG                                (0x0000712C)
  #define HUBS_CR_MBOX_ADDR_LO_HUBS_ADDR_OFF                           ( 0)
  #define HUBS_CR_MBOX_ADDR_LO_HUBS_ADDR_WID                           (32)
  #define HUBS_CR_MBOX_ADDR_LO_HUBS_ADDR_MSK                           (0xFFFFFFFF)
  #define HUBS_CR_MBOX_ADDR_LO_HUBS_ADDR_MAX                           (0xFFFFFFFF)
  #define HUBS_CR_MBOX_ADDR_LO_HUBS_ADDR_DEF                           (0x00000000)

#define HUBS_CR_MBOX_ADDR_HI_HUBS_REG                                (0x00007130)
  #define HUBS_CR_MBOX_ADDR_HI_HUBS_ADDR_OFF                           ( 0)
  #define HUBS_CR_MBOX_ADDR_HI_HUBS_ADDR_WID                           (32)
  #define HUBS_CR_MBOX_ADDR_HI_HUBS_ADDR_MSK                           (0xFFFFFFFF)
  #define HUBS_CR_MBOX_ADDR_HI_HUBS_ADDR_MAX                           (0xFFFFFFFF)
  #define HUBS_CR_MBOX_ADDR_HI_HUBS_ADDR_DEF                           (0x00000000)

#define HUBS_CR_MBOX_CMD_LO_HUBS_REG                                 (0x00007134)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_FBE_OFF                             ( 0)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_FBE_WID                             ( 4)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_FBE_MSK                             (0x0000000F)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_FBE_MAX                             (0x0000000F)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_FBE_DEF                             (0x00000000)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_LBE_OFF                             ( 4)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_LBE_WID                             ( 4)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_LBE_MSK                             (0x000000F0)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_LBE_MAX                             (0x0000000F)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_LBE_DEF                             (0x00000000)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_TAG_OFF                             ( 8)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_TAG_WID                             ( 8)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_TAG_MSK                             (0x0000FF00)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_TAG_MAX                             (0x000000FF)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_TAG_DEF                             (0x00000000)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_RQID_OFF                            (16)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_RQID_WID                            (16)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_RQID_MSK                            (0xFFFF0000)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_RQID_MAX                            (0x0000FFFF)
  #define HUBS_CR_MBOX_CMD_LO_HUBS_RQID_DEF                            (0x00000000)

#define HUBS_CR_MBOX_CMD_HI_HUBS_REG                                 (0x00007138)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_AT_OFF                              ( 0)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_AT_WID                              ( 2)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_AT_MSK                              (0x00000003)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_AT_MAX                              (0x00000003)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_AT_DEF                              (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_POISON_OFF                          ( 2)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_POISON_WID                          ( 1)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_POISON_MSK                          (0x00000004)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_POISON_MAX                          (0x00000001)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_POISON_DEF                          (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_LENGTH_OFF                          ( 3)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_LENGTH_WID                          ( 5)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_LENGTH_MSK                          (0x000000F8)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_LENGTH_MAX                          (0x0000001F)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_LENGTH_DEF                          (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RELAXED_OFF                         ( 8)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RELAXED_WID                         ( 1)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RELAXED_MSK                         (0x00000100)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RELAXED_MAX                         (0x00000001)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RELAXED_DEF                         (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_NOSNOOP_OFF                         ( 9)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_NOSNOOP_WID                         ( 1)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_NOSNOOP_MSK                         (0x00000200)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_NOSNOOP_MAX                         (0x00000001)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_NOSNOOP_DEF                         (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHAIN_OFF                           (10)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHAIN_WID                           ( 1)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHAIN_MSK                           (0x00000400)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHAIN_MAX                           (0x00000001)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHAIN_DEF                           (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CTYPE_OFF                           (11)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CTYPE_WID                           ( 5)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CTYPE_MSK                           (0x0000F800)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CTYPE_MAX                           (0x0000001F)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CTYPE_DEF                           (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_FMT_OFF                             (16)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_FMT_WID                             ( 2)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_FMT_MSK                             (0x00030000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_FMT_MAX                             (0x00000003)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_FMT_DEF                             (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_TC_OFF                              (18)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_TC_WID                              ( 4)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_TC_MSK                              (0x003C0000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_TC_MAX                              (0x0000000F)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_TC_DEF                              (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RESERVED_OFF                        (22)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RESERVED_WID                        ( 2)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RESERVED_MSK                        (0x00C00000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RESERVED_MAX                        (0x00000003)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RESERVED_DEF                        (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_DMI_PRIV_OFF                        (24)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_DMI_PRIV_WID                        ( 1)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_DMI_PRIV_MSK                        (0x01000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_DMI_PRIV_MAX                        (0x00000001)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_DMI_PRIV_DEF                        (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHID_OFF                            (25)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHID_WID                            ( 4)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHID_MSK                            (0x1E000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHID_MAX                            (0x0000000F)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_CHID_DEF                            (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RTYPE_OFF                           (29)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RTYPE_WID                           ( 2)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RTYPE_MSK                           (0x60000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RTYPE_MAX                           (0x00000003)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_RTYPE_DEF                           (0x00000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_START_OFF                           (31)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_START_WID                           ( 1)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_START_MSK                           (0x80000000)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_START_MAX                           (0x00000001)
  #define HUBS_CR_MBOX_CMD_HI_HUBS_START_DEF                           (0x00000000)

#define HUBS_CR_MBOX_CFG_HUBS_REG                                    (0x0000713C)
  #define HUBS_CR_MBOX_CFG_HUBS_ENABLE_OFF                             ( 0)
  #define HUBS_CR_MBOX_CFG_HUBS_ENABLE_WID                             ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_ENABLE_MSK                             (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_ENABLE_MAX                             (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_ENABLE_DEF                             (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_LOCK_IA_OFF                            ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_LOCK_IA_WID                            ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_LOCK_IA_MSK                            (0x00000002)
  #define HUBS_CR_MBOX_CFG_HUBS_LOCK_IA_MAX                            (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_LOCK_IA_DEF                            (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_RESET_OFF                              ( 2)
  #define HUBS_CR_MBOX_CFG_HUBS_RESET_WID                              ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_RESET_MSK                              (0x00000004)
  #define HUBS_CR_MBOX_CFG_HUBS_RESET_MAX                              (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_RESET_DEF                              (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_BLK_CYCLES_OFF                         ( 3)
  #define HUBS_CR_MBOX_CFG_HUBS_BLK_CYCLES_WID                         ( 6)
  #define HUBS_CR_MBOX_CFG_HUBS_BLK_CYCLES_MSK                         (0x000001F8)
  #define HUBS_CR_MBOX_CFG_HUBS_BLK_CYCLES_MAX                         (0x0000003F)
  #define HUBS_CR_MBOX_CFG_HUBS_BLK_CYCLES_DEF                         (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_WR_DWORD_SEL_OFF                       ( 9)
  #define HUBS_CR_MBOX_CFG_HUBS_WR_DWORD_SEL_WID                       ( 4)
  #define HUBS_CR_MBOX_CFG_HUBS_WR_DWORD_SEL_MSK                       (0x00001E00)
  #define HUBS_CR_MBOX_CFG_HUBS_WR_DWORD_SEL_MAX                       (0x0000000F)
  #define HUBS_CR_MBOX_CFG_HUBS_WR_DWORD_SEL_DEF                       (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_RD_DWORD_SEL_OFF                       (13)
  #define HUBS_CR_MBOX_CFG_HUBS_RD_DWORD_SEL_WID                       ( 4)
  #define HUBS_CR_MBOX_CFG_HUBS_RD_DWORD_SEL_MSK                       (0x0001E000)
  #define HUBS_CR_MBOX_CFG_HUBS_RD_DWORD_SEL_MAX                       (0x0000000F)
  #define HUBS_CR_MBOX_CFG_HUBS_RD_DWORD_SEL_DEF                       (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_CMD_CNT_OFF                        (17)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_CMD_CNT_WID                        ( 6)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_CMD_CNT_MSK                        (0x007E0000)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_CMD_CNT_MAX                        (0x0000003F)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_CMD_CNT_DEF                        (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_ADDR_OFF                       (23)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_ADDR_WID                       ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_ADDR_MSK                       (0x00800000)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_ADDR_MAX                       (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_ADDR_DEF                       (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_PAGE_OFF                       (24)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_PAGE_WID                       ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_PAGE_MSK                       (0x01000000)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_PAGE_MAX                       (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_RPT_NXT_PAGE_DEF                       (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_DIS_CMP_INV_OFF                        (25)
  #define HUBS_CR_MBOX_CFG_HUBS_DIS_CMP_INV_WID                        ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_DIS_CMP_INV_MSK                        (0x02000000)
  #define HUBS_CR_MBOX_CFG_HUBS_DIS_CMP_INV_MAX                        (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_DIS_CMP_INV_DEF                        (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_FSM_STATE_OFF                          (26)
  #define HUBS_CR_MBOX_CFG_HUBS_FSM_STATE_WID                          ( 4)
  #define HUBS_CR_MBOX_CFG_HUBS_FSM_STATE_MSK                          (0x3C000000)
  #define HUBS_CR_MBOX_CFG_HUBS_FSM_STATE_MAX                          (0x0000000F)
  #define HUBS_CR_MBOX_CFG_HUBS_FSM_STATE_DEF                          (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_P2P_ALL_OFF                            (30)
  #define HUBS_CR_MBOX_CFG_HUBS_P2P_ALL_WID                            ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_P2P_ALL_MSK                            (0x40000000)
  #define HUBS_CR_MBOX_CFG_HUBS_P2P_ALL_MAX                            (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_P2P_ALL_DEF                            (0x00000000)
  #define HUBS_CR_MBOX_CFG_HUBS_SPARE_OFF                              (31)
  #define HUBS_CR_MBOX_CFG_HUBS_SPARE_WID                              ( 1)
  #define HUBS_CR_MBOX_CFG_HUBS_SPARE_MSK                              (0x80000000)
  #define HUBS_CR_MBOX_CFG_HUBS_SPARE_MAX                              (0x00000001)
  #define HUBS_CR_MBOX_CFG_HUBS_SPARE_DEF                              (0x00000000)

#define HUBS_CR_MBOX_STATUS_HUBS_REG                                 (0x00007140)
  #define HUBS_CR_MBOX_STATUS_HUBS_FSM_STATE_OFF                       ( 0)
  #define HUBS_CR_MBOX_STATUS_HUBS_FSM_STATE_WID                       ( 4)
  #define HUBS_CR_MBOX_STATUS_HUBS_FSM_STATE_MSK                       (0x0000000F)
  #define HUBS_CR_MBOX_STATUS_HUBS_FSM_STATE_MAX                       (0x0000000F)
  #define HUBS_CR_MBOX_STATUS_HUBS_FSM_STATE_DEF                       (0x00000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_RPT_CMD_CNT_OFF                     ( 4)
  #define HUBS_CR_MBOX_STATUS_HUBS_RPT_CMD_CNT_WID                     ( 6)
  #define HUBS_CR_MBOX_STATUS_HUBS_RPT_CMD_CNT_MSK                     (0x000003F0)
  #define HUBS_CR_MBOX_STATUS_HUBS_RPT_CMD_CNT_MAX                     (0x0000003F)
  #define HUBS_CR_MBOX_STATUS_HUBS_RPT_CMD_CNT_DEF                     (0x00000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_OPCODE_OFF                          (10)
  #define HUBS_CR_MBOX_STATUS_HUBS_OPCODE_WID                          ( 7)
  #define HUBS_CR_MBOX_STATUS_HUBS_OPCODE_MSK                          (0x0001FC00)
  #define HUBS_CR_MBOX_STATUS_HUBS_OPCODE_MAX                          (0x0000007F)
  #define HUBS_CR_MBOX_STATUS_HUBS_OPCODE_DEF                          (0x00000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_WR_DWORD_SEL_OFF                    (17)
  #define HUBS_CR_MBOX_STATUS_HUBS_WR_DWORD_SEL_WID                    ( 4)
  #define HUBS_CR_MBOX_STATUS_HUBS_WR_DWORD_SEL_MSK                    (0x001E0000)
  #define HUBS_CR_MBOX_STATUS_HUBS_WR_DWORD_SEL_MAX                    (0x0000000F)
  #define HUBS_CR_MBOX_STATUS_HUBS_WR_DWORD_SEL_DEF                    (0x00000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_RD_DWORD_SEL_OFF                    (21)
  #define HUBS_CR_MBOX_STATUS_HUBS_RD_DWORD_SEL_WID                    ( 4)
  #define HUBS_CR_MBOX_STATUS_HUBS_RD_DWORD_SEL_MSK                    (0x01E00000)
  #define HUBS_CR_MBOX_STATUS_HUBS_RD_DWORD_SEL_MAX                    (0x0000000F)
  #define HUBS_CR_MBOX_STATUS_HUBS_RD_DWORD_SEL_DEF                    (0x00000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_UP_OFF                       (25)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_UP_WID                       ( 1)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_UP_MSK                       (0x02000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_UP_MAX                       (0x00000001)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_UP_DEF                       (0x00000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_DN_OFF                       (26)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_DN_WID                       ( 1)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_DN_MSK                       (0x04000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_DN_MAX                       (0x00000001)
  #define HUBS_CR_MBOX_STATUS_HUBS_P2P_RD_DN_DEF                       (0x00000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_SPARE_OFF                           (27)
  #define HUBS_CR_MBOX_STATUS_HUBS_SPARE_WID                           ( 5)
  #define HUBS_CR_MBOX_STATUS_HUBS_SPARE_MSK                           (0xF8000000)
  #define HUBS_CR_MBOX_STATUS_HUBS_SPARE_MAX                           (0x0000001F)
  #define HUBS_CR_MBOX_STATUS_HUBS_SPARE_DEF                           (0x00000000)

#define HUBS_CR_HUB0_BLOCK_UP_HUBS_REG                               (0x00007144)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_XTM_CHID_OFF                      ( 0)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_XTM_CHID_WID                      ( 9)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_XTM_CHID_MSK                      (0x000001FF)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_XTM_CHID_MAX                      (0x000001FF)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_XTM_CHID_DEF                      (0x00000000)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_NP_OFF                            ( 9)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_NP_WID                            ( 1)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_NP_MSK                            (0x00000200)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_NP_MAX                            (0x00000001)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_NP_DEF                            (0x00000000)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_PC_OFF                            (10)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_PC_WID                            ( 1)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_PC_MSK                            (0x00000400)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_PC_MAX                            (0x00000001)
  #define HUBS_CR_HUB0_BLOCK_UP_HUBS_PC_DEF                            (0x00000000)

#define MPRDRTRN_CR_CRDTCTL0_IMPH_REG                                (0x00007400)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVC0_PMIN_OFF                    ( 0)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVC0_PMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVC0_PMIN_MSK                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVC0_PMIN_MAX                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P10VC0_PMIN_OFF                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P10VC0_PMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P10VC0_PMIN_MSK                    (0x00000038)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P10VC0_PMIN_MAX                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P11VC0_PMIN_OFF                    ( 6)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P11VC0_PMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P11VC0_PMIN_MSK                    (0x000001C0)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P11VC0_PMIN_MAX                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P12VC0_PMIN_OFF                    ( 9)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P12VC0_PMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P12VC0_PMIN_MSK                    (0x00000E00)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_P12VC0_PMIN_MAX                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DEVC0_PMIN_OFF                     (12)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DEVC0_PMIN_WID                     ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DEVC0_PMIN_MSK                     (0x00007000)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DEVC0_PMIN_MAX                     (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVCP_PMIN_OFF                    (15)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVCP_PMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVCP_PMIN_MSK                    (0x00038000)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVCP_PMIN_MAX                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVCM_PMIN_OFF                    (18)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVCM_PMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVCM_PMIN_MSK                    (0x001C0000)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVCM_PMIN_MAX                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVC1_PMIN_OFF                    (21)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVC1_PMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVC1_PMIN_MSK                    (0x00E00000)
  #define MPRDRTRN_CR_CRDTCTL0_IMPH_DMIVC1_PMIN_MAX                    (0x00000007)

#define MPRDRTRN_CR_CRDTCTL1_IMPH_REG                                (0x00007404)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVC0_NPMIN_OFF                   ( 0)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVC0_NPMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVC0_NPMIN_MSK                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVC0_NPMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P10VC0_NPMIN_OFF                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P10VC0_NPMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P10VC0_NPMIN_MSK                   (0x00000038)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P10VC0_NPMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P11VC0_NPMIN_OFF                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P11VC0_NPMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P11VC0_NPMIN_MSK                   (0x000001C0)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P11VC0_NPMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P12VC0_NPMIN_OFF                   ( 9)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P12VC0_NPMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P12VC0_NPMIN_MSK                   (0x00000E00)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_P12VC0_NPMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DEVC0_NPMIN_OFF                    (12)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DEVC0_NPMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DEVC0_NPMIN_MSK                    (0x00007000)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DEVC0_NPMIN_MAX                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVCP_NPMIN_OFF                   (15)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVCP_NPMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVCP_NPMIN_MSK                   (0x00038000)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVCP_NPMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVCM_NPMIN_OFF                   (18)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVCM_NPMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVCM_NPMIN_MSK                   (0x001C0000)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVCM_NPMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVC1_NPMIN_OFF                   (21)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVC1_NPMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVC1_NPMIN_MSK                   (0x00E00000)
  #define MPRDRTRN_CR_CRDTCTL1_IMPH_DMIVC1_NPMIN_MAX                   (0x00000007)

#define MPRDRTRN_CR_CRDTCTL2_IMPH_REG                                (0x00007408)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVC0_RRMIN_OFF                   ( 0)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVC0_RRMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVC0_RRMIN_MSK                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVC0_RRMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P10VC0_RRMIN_OFF                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P10VC0_RRMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P10VC0_RRMIN_MSK                   (0x00000038)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P10VC0_RRMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P11VC0_RRMIN_OFF                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P11VC0_RRMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P11VC0_RRMIN_MSK                   (0x000001C0)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P11VC0_RRMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P12VC0_RRMIN_OFF                   ( 9)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P12VC0_RRMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P12VC0_RRMIN_MSK                   (0x00000E00)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_P12VC0_RRMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DEVC0_RRMIN_OFF                    (12)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DEVC0_RRMIN_WID                    ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DEVC0_RRMIN_MSK                    (0x00007000)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DEVC0_RRMIN_MAX                    (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVCP_RRMIN_OFF                   (15)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVCP_RRMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVCP_RRMIN_MSK                   (0x00038000)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVCP_RRMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVCM_RRMIN_OFF                   (18)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVCM_RRMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVCM_RRMIN_MSK                   (0x001C0000)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVCM_RRMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVC1_RRMIN_OFF                   (21)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVC1_RRMIN_WID                   ( 3)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVC1_RRMIN_MSK                   (0x00E00000)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DMIVC1_RRMIN_MAX                   (0x00000007)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DEVC1_RRMIN_OFF                    (24)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DEVC1_RRMIN_WID                    ( 6)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DEVC1_RRMIN_MSK                    (0x3F000000)
  #define MPRDRTRN_CR_CRDTCTL2_IMPH_DEVC1_RRMIN_MAX                    (0x0000003F)

#define MPRDRTRN_CR_CRDTCTL3_IMPH_REG                                (0x0000740C)
  #define MPRDRTRN_CR_CRDTCTL3_IMPH_IOTRK_SHRD_OFF                     ( 0)
  #define MPRDRTRN_CR_CRDTCTL3_IMPH_IOTRK_SHRD_WID                     ( 6)
  #define MPRDRTRN_CR_CRDTCTL3_IMPH_IOTRK_SHRD_MSK                     (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL3_IMPH_IOTRK_SHRD_MAX                     (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL3_IMPH_RRTRK_SHRD_OFF                     ( 6)
  #define MPRDRTRN_CR_CRDTCTL3_IMPH_RRTRK_SHRD_WID                     ( 7)
  #define MPRDRTRN_CR_CRDTCTL3_IMPH_RRTRK_SHRD_MSK                     (0x00001FC0)
  #define MPRDRTRN_CR_CRDTCTL3_IMPH_RRTRK_SHRD_MAX                     (0x0000007F)

#define MPRDRTRN_CR_CRDTCTL4_IMPH_REG                                (0x00007410)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_DMIVC0_PMAX_OFF                    ( 0)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_DMIVC0_PMAX_WID                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_DMIVC0_PMAX_MSK                    (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_DMIVC0_PMAX_MAX                    (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P10VC0_PMAX_OFF                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P10VC0_PMAX_WID                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P10VC0_PMAX_MSK                    (0x000003E0)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P10VC0_PMAX_MAX                    (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P11VC0_PMAX_OFF                    (10)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P11VC0_PMAX_WID                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P11VC0_PMAX_MSK                    (0x00007C00)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P11VC0_PMAX_MAX                    (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P12VC0_PMAX_OFF                    (15)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P12VC0_PMAX_WID                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P12VC0_PMAX_MSK                    (0x000F8000)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_P12VC0_PMAX_MAX                    (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_DEVC0_PMAX_OFF                     (20)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_DEVC0_PMAX_WID                     ( 5)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_DEVC0_PMAX_MSK                     (0x01F00000)
  #define MPRDRTRN_CR_CRDTCTL4_IMPH_DEVC0_PMAX_MAX                     (0x0000001F)

#define MPRDRTRN_CR_CRDTCTL5_IMPH_REG                                (0x00007414)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVCP_PMAX_OFF                    ( 0)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVCP_PMAX_WID                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVCP_PMAX_MSK                    (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVCP_PMAX_MAX                    (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVCM_PMAX_OFF                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVCM_PMAX_WID                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVCM_PMAX_MSK                    (0x000003E0)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVCM_PMAX_MAX                    (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVC1_PMAX_OFF                    (10)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVC1_PMAX_WID                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVC1_PMAX_MSK                    (0x00007C00)
  #define MPRDRTRN_CR_CRDTCTL5_IMPH_DMIVC1_PMAX_MAX                    (0x0000001F)

#define MPRDRTRN_CR_CRDTCTL6_IMPH_REG                                (0x00007418)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_DMIVC0_NPMAX_OFF                   ( 0)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_DMIVC0_NPMAX_WID                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_DMIVC0_NPMAX_MSK                   (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_DMIVC0_NPMAX_MAX                   (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P10VC0_NPMAX_OFF                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P10VC0_NPMAX_WID                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P10VC0_NPMAX_MSK                   (0x000003E0)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P10VC0_NPMAX_MAX                   (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P11VC0_NPMAX_OFF                   (10)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P11VC0_NPMAX_WID                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P11VC0_NPMAX_MSK                   (0x00007C00)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P11VC0_NPMAX_MAX                   (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P12VC0_NPMAX_OFF                   (15)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P12VC0_NPMAX_WID                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P12VC0_NPMAX_MSK                   (0x000F8000)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_P12VC0_NPMAX_MAX                   (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_DEVC0_NPMAX_OFF                    (20)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_DEVC0_NPMAX_WID                    ( 5)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_DEVC0_NPMAX_MSK                    (0x01F00000)
  #define MPRDRTRN_CR_CRDTCTL6_IMPH_DEVC0_NPMAX_MAX                    (0x0000001F)

#define MPRDRTRN_CR_CRDTCTL7_IMPH_REG                                (0x0000741C)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVCP_NPMAX_OFF                   ( 0)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVCP_NPMAX_WID                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVCP_NPMAX_MSK                   (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVCP_NPMAX_MAX                   (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVCM_NPMAX_OFF                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVCM_NPMAX_WID                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVCM_NPMAX_MSK                   (0x000003E0)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVCM_NPMAX_MAX                   (0x0000001F)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVC1_NPMAX_OFF                   (10)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVC1_NPMAX_WID                   ( 5)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVC1_NPMAX_MSK                   (0x00007C00)
  #define MPRDRTRN_CR_CRDTCTL7_IMPH_DMIVC1_NPMAX_MAX                   (0x0000001F)

#define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_REG                     (0x00007420)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_DATA_OFF                ( 0)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_DATA_WID                (18)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_DATA_MSK                (0x0003FFFF)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_DATA_MAX                (0x0003FFFF)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_DATA_DEF                (0x00000000)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_VORANGE_OFF             (31)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_VORANGE_WID             ( 1)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_VORANGE_MSK             (0x80000000)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_VORANGE_MAX             (0x00000001)
  #define MPRDRTRN_CR_VISA_CTL_MPIORDTRKS_IMPH_VORANGE_DEF             (0x00000000)

#define MPRDRTRN_CR_VISA_XBAR_MPIORDTRKS_IMPH_REG                    (0x00007424)
  #define MPRDRTRN_CR_VISA_XBAR_MPIORDTRKS_IMPH_DATA_OFF               ( 0)
  #define MPRDRTRN_CR_VISA_XBAR_MPIORDTRKS_IMPH_DATA_WID               (32)
  #define MPRDRTRN_CR_VISA_XBAR_MPIORDTRKS_IMPH_DATA_MSK               (0xFFFFFFFF)
  #define MPRDRTRN_CR_VISA_XBAR_MPIORDTRKS_IMPH_DATA_MAX               (0xFFFFFFFF)
  #define MPRDRTRN_CR_VISA_XBAR_MPIORDTRKS_IMPH_DATA_DEF               (0x76543210)

#define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_REG                       (0x00007428)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_DATA_OFF                  ( 0)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_DATA_WID                  (18)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_DATA_MSK                  (0x0003FFFF)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_DATA_MAX                  (0x0003FFFF)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_DATA_DEF                  (0x00000000)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_VORANGE_OFF               (31)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_VORANGE_WID               ( 1)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_VORANGE_MSK               (0x80000000)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_VORANGE_MAX               (0x00000001)
  #define MPRDRTRN_CR_VISA_CTL_MPIOTRKS_IMPH_VORANGE_DEF               (0x00000000)

#define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_REG                            (0x00007430)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_VC_OFF                         ( 0)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_VC_WID                         ( 4)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_VC_MSK                         (0x0000000F)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_VC_MAX                         (0x0000000F)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_VC_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_FMT_CMDTYPE_OFF                ( 4)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_FMT_CMDTYPE_WID                ( 6)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_FMT_CMDTYPE_MSK                (0x000003F0)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_FMT_CMDTYPE_MAX                (0x0000003F)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_FMT_CMDTYPE_DEF                (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_TC_OFF                         (10)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_TC_WID                         ( 4)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_TC_MSK                         (0x00003C00)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_TC_MAX                         (0x0000000F)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_TC_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_NS_OFF                         (14)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_NS_WID                         ( 1)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_NS_MSK                         (0x00004000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_NS_MAX                         (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_NS_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_RO_OFF                         (15)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_RO_WID                         ( 1)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_RO_MSK                         (0x00008000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_RO_MAX                         (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_RO_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_LENGTH_OFF                     (16)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_LENGTH_WID                     ( 5)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_LENGTH_MSK                     (0x001F0000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_LENGTH_MAX                     (0x0000001F)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_LENGTH_DEF                     (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_EP_OFF                         (21)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_EP_WID                         ( 1)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_EP_MSK                         (0x00200000)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_EP_MAX                         (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_MASK1_IMPH_EP_DEF                         (0x00000000)

#define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_REG                            (0x00007434)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_RQID_OFF                       ( 0)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_RQID_WID                       (16)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_RQID_MSK                       (0x0000FFFF)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_RQID_MAX                       (0x0000FFFF)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_RQID_DEF                       (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_TAG_OFF                        (16)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_TAG_WID                        ( 8)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_TAG_MSK                        (0x00FF0000)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_TAG_MAX                        (0x000000FF)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_TAG_DEF                        (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_LBEFBE_MSGTYPE_OFF             (24)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_LBEFBE_MSGTYPE_WID             ( 8)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_LBEFBE_MSGTYPE_MSK             (0xFF000000)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_LBEFBE_MSGTYPE_MAX             (0x000000FF)
  #define MPRDRTRN_CR_PRIMDN_MASK2_IMPH_LBEFBE_MSGTYPE_DEF             (0x00000000)

#define MPRDRTRN_CR_PRIMDN_MASK3_IMPH_REG                            (0x00007438)
  #define MPRDRTRN_CR_PRIMDN_MASK3_IMPH_ADDR_31_0_OFF                  ( 0)
  #define MPRDRTRN_CR_PRIMDN_MASK3_IMPH_ADDR_31_0_WID                  (32)
  #define MPRDRTRN_CR_PRIMDN_MASK3_IMPH_ADDR_31_0_MSK                  (0xFFFFFFFF)
  #define MPRDRTRN_CR_PRIMDN_MASK3_IMPH_ADDR_31_0_MAX                  (0xFFFFFFFF)
  #define MPRDRTRN_CR_PRIMDN_MASK3_IMPH_ADDR_31_0_DEF                  (0x00000000)

#define MPRDRTRN_CR_PRIMDN_MASK4_IMPH_REG                            (0x0000743C)
  #define MPRDRTRN_CR_PRIMDN_MASK4_IMPH_ADDR_63_32_OFF                 ( 0)
  #define MPRDRTRN_CR_PRIMDN_MASK4_IMPH_ADDR_63_32_WID                 (32)
  #define MPRDRTRN_CR_PRIMDN_MASK4_IMPH_ADDR_63_32_MSK                 (0xFFFFFFFF)
  #define MPRDRTRN_CR_PRIMDN_MASK4_IMPH_ADDR_63_32_MAX                 (0xFFFFFFFF)
  #define MPRDRTRN_CR_PRIMDN_MASK4_IMPH_ADDR_63_32_DEF                 (0x00000000)

#define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_REG                            (0x00007440)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_VC_OFF                         ( 0)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_VC_WID                         ( 4)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_VC_MSK                         (0x0000000F)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_VC_MAX                         (0x0000000F)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_VC_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_FMT_CMDTYPE_OFF                ( 4)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_FMT_CMDTYPE_WID                ( 6)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_FMT_CMDTYPE_MSK                (0x000003F0)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_FMT_CMDTYPE_MAX                (0x0000003F)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_FMT_CMDTYPE_DEF                (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_TC_OFF                         (10)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_TC_WID                         ( 4)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_TC_MSK                         (0x00003C00)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_TC_MAX                         (0x0000000F)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_TC_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_EP_OFF                         (14)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_EP_WID                         ( 1)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_EP_MSK                         (0x00004000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_EP_MAX                         (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_EP_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_NS_OFF                         (15)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_NS_WID                         ( 1)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_NS_MSK                         (0x00008000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_NS_MAX                         (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_NS_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_RO_OFF                         (16)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_RO_WID                         ( 1)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_RO_MSK                         (0x00010000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_RO_MAX                         (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_RO_DEF                         (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_LENGTH_OFF                     (17)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_LENGTH_WID                     ( 5)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_LENGTH_MSK                     (0x003E0000)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_LENGTH_MAX                     (0x0000001F)
  #define MPRDRTRN_CR_PRIMDN_COMP1_IMPH_LENGTH_DEF                     (0x00000000)

#define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_REG                            (0x00007444)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_RQID_OFF                       ( 0)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_RQID_WID                       (16)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_RQID_MSK                       (0x0000FFFF)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_RQID_MAX                       (0x0000FFFF)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_RQID_DEF                       (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_TAG_OFF                        (16)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_TAG_WID                        ( 8)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_TAG_MSK                        (0x00FF0000)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_TAG_MAX                        (0x000000FF)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_TAG_DEF                        (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_LBEFBE_MSGTYPE_OFF             (24)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_LBEFBE_MSGTYPE_WID             ( 8)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_LBEFBE_MSGTYPE_MSK             (0xFF000000)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_LBEFBE_MSGTYPE_MAX             (0x000000FF)
  #define MPRDRTRN_CR_PRIMDN_COMP2_IMPH_LBEFBE_MSGTYPE_DEF             (0x00000000)

#define MPRDRTRN_CR_PRIMDN_COMP3_IMPH_REG                            (0x00007448)
  #define MPRDRTRN_CR_PRIMDN_COMP3_IMPH_ADDR_31_0_OFF                  ( 0)
  #define MPRDRTRN_CR_PRIMDN_COMP3_IMPH_ADDR_31_0_WID                  (32)
  #define MPRDRTRN_CR_PRIMDN_COMP3_IMPH_ADDR_31_0_MSK                  (0xFFFFFFFF)
  #define MPRDRTRN_CR_PRIMDN_COMP3_IMPH_ADDR_31_0_MAX                  (0xFFFFFFFF)
  #define MPRDRTRN_CR_PRIMDN_COMP3_IMPH_ADDR_31_0_DEF                  (0x00000000)

#define MPRDRTRN_CR_PRIMDN_COMP4_IMPH_REG                            (0x0000744C)
  #define MPRDRTRN_CR_PRIMDN_COMP4_IMPH_ADDR_63_32_OFF                 ( 0)
  #define MPRDRTRN_CR_PRIMDN_COMP4_IMPH_ADDR_63_32_WID                 (32)
  #define MPRDRTRN_CR_PRIMDN_COMP4_IMPH_ADDR_63_32_MSK                 (0xFFFFFFFF)
  #define MPRDRTRN_CR_PRIMDN_COMP4_IMPH_ADDR_63_32_MAX                 (0xFFFFFFFF)
  #define MPRDRTRN_CR_PRIMDN_COMP4_IMPH_ADDR_63_32_DEF                 (0x00000000)

#define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_REG                          (0x00007450)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_ENABLE_OFF                   ( 0)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_ENABLE_WID                   ( 1)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_ENABLE_MSK                   (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_ENABLE_MAX                   (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_ENABLE_DEF                   (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_TRIGGERED_OFF                ( 1)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_TRIGGERED_WID                ( 1)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_TRIGGERED_MSK                (0x00000002)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_TRIGGERED_MAX                (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_TRIGGERED_DEF                (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DNARB_OFF              ( 2)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DNARB_WID              ( 1)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DNARB_MSK              (0x00000004)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DNARB_MAX              (0x00000001)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DNARB_DEF              (0x00000000)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DELAY_OFF              (23)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DELAY_WID              ( 9)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DELAY_MSK              (0xFF800000)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DELAY_MAX              (0x000001FF)
  #define MPRDRTRN_CR_PRIMDN_TRIGGER_IMPH_STALL_DELAY_DEF              (0x00000000)

#define MPRDRTRN_CR_CRDTCTL8_IMPH_REG                                (0x00007454)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_DMIVC0_RRMAX_OFF                   ( 0)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_DMIVC0_RRMAX_WID                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_DMIVC0_RRMAX_MSK                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_DMIVC0_RRMAX_MAX                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P10VC0_RRMAX_OFF                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P10VC0_RRMAX_WID                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P10VC0_RRMAX_MSK                   (0x00000FC0)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P10VC0_RRMAX_MAX                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P11VC0_RRMAX_OFF                   (12)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P11VC0_RRMAX_WID                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P11VC0_RRMAX_MSK                   (0x0003F000)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P11VC0_RRMAX_MAX                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P12VC0_RRMAX_OFF                   (18)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P12VC0_RRMAX_WID                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P12VC0_RRMAX_MSK                   (0x00FC0000)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_P12VC0_RRMAX_MAX                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_DEVC0_RRMAX_OFF                    (24)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_DEVC0_RRMAX_WID                    ( 6)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_DEVC0_RRMAX_MSK                    (0x3F000000)
  #define MPRDRTRN_CR_CRDTCTL8_IMPH_DEVC0_RRMAX_MAX                    (0x0000003F)

#define MPRDRTRN_CR_CRDTCTL9_IMPH_REG                                (0x00007458)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVCP_RRMAX_OFF                   ( 0)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVCP_RRMAX_WID                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVCP_RRMAX_MSK                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVCP_RRMAX_MAX                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVCM_RRMAX_OFF                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVCM_RRMAX_WID                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVCM_RRMAX_MSK                   (0x00000FC0)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVCM_RRMAX_MAX                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVC1_RRMAX_OFF                   (12)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVC1_RRMAX_WID                   ( 6)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVC1_RRMAX_MSK                   (0x0003F000)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DMIVC1_RRMAX_MAX                   (0x0000003F)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DEVC1_RRMAX_OFF                    (18)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DEVC1_RRMAX_WID                    ( 6)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DEVC1_RRMAX_MSK                    (0x00FC0000)
  #define MPRDRTRN_CR_CRDTCTL9_IMPH_DEVC1_RRMAX_MAX                    (0x0000003F)

#define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_REG                          (0x00007500)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_LIM_OFF                      ( 0)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_LIM_WID                      (16)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_LIM_MSK                      (0x0000FFFF)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_LIM_MAX                      (0x0000FFFF)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_LIM_DEF                      (0x00000000)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_MSK_OFF                      (16)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_MSK_WID                      ( 3)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_MSK_MSK                      (0x00070000)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_MSK_MAX                      (0x00000007)
  #define MPRDRTRN_CR_PCIE_POPUP_CTL_IMPH_MSK_DEF                      (0x00000007)

#define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_REG                           (0x00007504)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_FUNNUM_OFF                    ( 0)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_FUNNUM_WID                    ( 3)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_FUNNUM_MSK                    (0x00000007)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_FUNNUM_MAX                    (0x00000007)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_FUNNUM_DEF                    (0x00000000)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_DEVNUM_OFF                    ( 3)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_DEVNUM_WID                    ( 5)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_DEVNUM_MSK                    (0x000000F8)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_DEVNUM_MAX                    (0x0000001F)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_DEVNUM_DEF                    (0x00000016)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BUSNUM_OFF                    ( 8)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BUSNUM_WID                    ( 8)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BUSNUM_MSK                    (0x0000FF00)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BUSNUM_MAX                    (0x000000FF)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BUSNUM_DEF                    (0x00000000)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BARNUM_OFF                    (16)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BARNUM_WID                    ( 3)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BARNUM_MSK                    (0x00070000)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BARNUM_MAX                    (0x00000007)
  #define MPRDRTRN_CR_VDMBDFBARSSKU_IMPH_BARNUM_DEF                    (0x00000007)

#define MPRDRTRN_CR_CRDTLCK_IMPH_REG                                 (0x000077FC)
  #define MPRDRTRN_CR_CRDTLCK_IMPH_LOCK_OFF                            ( 0)
  #define MPRDRTRN_CR_CRDTLCK_IMPH_LOCK_WID                            ( 1)
  #define MPRDRTRN_CR_CRDTLCK_IMPH_LOCK_MSK                            (0x00000001)
  #define MPRDRTRN_CR_CRDTLCK_IMPH_LOCK_MAX                            (0x00000001)
  #define MPRDRTRN_CR_CRDTLCK_IMPH_LOCK_DEF                            (0x00000000)

#define MPMCARB_CR_VCLIM0_IMPH_REG                                   (0x00007800)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPNPLIM_OFF                          ( 0)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPNPLIM_WID                          ( 3)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPNPLIM_MSK                          (0x00000007)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPNPLIM_MAX                          (0x00000007)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPNPLIM_DEF                          (0x00000004)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPPLIM_OFF                           ( 4)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPPLIM_WID                           ( 3)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPPLIM_MSK                           (0x00000070)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPPLIM_MAX                           (0x00000007)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPPLIM_DEF                           (0x00000004)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMNPLIM_OFF                          ( 8)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMNPLIM_WID                          ( 3)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMNPLIM_MSK                          (0x00000700)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMNPLIM_MAX                          (0x00000007)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMNPLIM_DEF                          (0x00000004)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMPLIM_OFF                           (12)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMPLIM_WID                           ( 3)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMPLIM_MSK                           (0x00007000)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMPLIM_MAX                           (0x00000007)
  #define MPMCARB_CR_VCLIM0_IMPH_VCMPLIM_DEF                           (0x00000004)
  #define MPMCARB_CR_VCLIM0_IMPH_VC0VTDLIM_OFF                         (16)
  #define MPMCARB_CR_VCLIM0_IMPH_VC0VTDLIM_WID                         ( 3)
  #define MPMCARB_CR_VCLIM0_IMPH_VC0VTDLIM_MSK                         (0x00070000)
  #define MPMCARB_CR_VCLIM0_IMPH_VC0VTDLIM_MAX                         (0x00000007)
  #define MPMCARB_CR_VCLIM0_IMPH_VC0VTDLIM_DEF                         (0x00000004)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPVTDLIM_OFF                         (20)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPVTDLIM_WID                         ( 3)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPVTDLIM_MSK                         (0x00700000)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPVTDLIM_MAX                         (0x00000007)
  #define MPMCARB_CR_VCLIM0_IMPH_VCPVTDLIM_DEF                         (0x00000004)

#define MPMCARB_CR_VCLIM1_IMPH_REG                                   (0x00007804)
  #define MPMCARB_CR_VCLIM1_IMPH_IARD_OFF                              ( 0)
  #define MPMCARB_CR_VCLIM1_IMPH_IARD_WID                              ( 3)
  #define MPMCARB_CR_VCLIM1_IMPH_IARD_MSK                              (0x00000007)
  #define MPMCARB_CR_VCLIM1_IMPH_IARD_MAX                              (0x00000007)
  #define MPMCARB_CR_VCLIM1_IMPH_IARD_DEF                              (0x00000004)
  #define MPMCARB_CR_VCLIM1_IMPH_IAWR_OFF                              ( 4)
  #define MPMCARB_CR_VCLIM1_IMPH_IAWR_WID                              ( 3)
  #define MPMCARB_CR_VCLIM1_IMPH_IAWR_MSK                              (0x00000070)
  #define MPMCARB_CR_VCLIM1_IMPH_IAWR_MAX                              (0x00000007)
  #define MPMCARB_CR_VCLIM1_IMPH_IAWR_DEF                              (0x00000004)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDL3_OFF                             ( 8)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDL3_WID                             ( 3)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDL3_MSK                             (0x00000700)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDL3_MAX                             (0x00000007)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDL3_DEF                             (0x00000004)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDNL3_OFF                            (12)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDNL3_WID                            ( 3)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDNL3_MSK                            (0x00007000)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDNL3_MAX                            (0x00000007)
  #define MPMCARB_CR_VCLIM1_IMPH_VTDNL3_DEF                            (0x00000004)

#define MPMCARB_CR_ATMC_STS_IMPH_REG                                 (0x00007808)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_WR_CNFLT_OFF                    ( 0)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_WR_CNFLT_WID                    ( 1)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_WR_CNFLT_MSK                    (0x00000001)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_WR_CNFLT_MAX                    (0x00000001)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_WR_CNFLT_DEF                    (0x00000000)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_RD_CNFLT_OFF                    ( 1)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_RD_CNFLT_WID                    ( 1)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_RD_CNFLT_MSK                    (0x00000002)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_RD_CNFLT_MAX                    (0x00000001)
  #define MPMCARB_CR_ATMC_STS_IMPH_VC1_RD_CNFLT_DEF                    (0x00000000)

#define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_REG                      (0x00007820)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_DATA_OFF                 ( 0)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_DATA_WID                 (18)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_DATA_MSK                 (0x0003FFFF)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_DATA_MAX                 (0x0003FFFF)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_DATA_DEF                 (0x00000000)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_VORANGE_OFF              (31)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_VORANGE_WID              ( 1)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_VORANGE_MSK              (0x80000000)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_VORANGE_MAX              (0x00000001)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_F_IMPH_VORANGE_DEF              (0x00000000)

#define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_REG                      (0x00007824)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_DATA_OFF                 ( 0)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_DATA_WID                 (18)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_DATA_MSK                 (0x0003FFFF)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_DATA_MAX                 (0x0003FFFF)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_DATA_DEF                 (0x00000000)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_VORANGE_OFF              (31)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_VORANGE_WID              ( 1)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_VORANGE_MSK              (0x80000000)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_VORANGE_MAX              (0x00000001)
  #define MPMCARB_CR_VISA_CTL_MPMCARBS_D_IMPH_VORANGE_DEF              (0x00000000)

#define MPMCARB_CR_MCARBLCK_IMPH_REG                                 (0x00007FFC)
  #define MPMCARB_CR_MCARBLCK_IMPH_LOCK_OFF                            ( 0)
  #define MPMCARB_CR_MCARBLCK_IMPH_LOCK_WID                            ( 1)
  #define MPMCARB_CR_MCARBLCK_IMPH_LOCK_MSK                            (0x00000001)
  #define MPMCARB_CR_MCARBLCK_IMPH_LOCK_MAX                            (0x00000001)
  #define MPMCARB_CR_MCARBLCK_IMPH_LOCK_DEF                            (0x00000000)

#define PCU_CR_CAMARILLO_MAILBOX_DATA0_PCU_REG                       (0x00005810)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA0_PCU_DATA_OFF                  ( 0)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA0_PCU_DATA_WID                  (32)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA0_PCU_DATA_MSK                  (0xFFFFFFFF)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA0_PCU_DATA_MAX                  (0xFFFFFFFF)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA0_PCU_DATA_DEF                  (0x00000000)

#define PCU_CR_CAMARILLO_MAILBOX_DATA1_PCU_REG                       (0x00005814)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA1_PCU_DATA_OFF                  ( 0)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA1_PCU_DATA_WID                  (32)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA1_PCU_DATA_MSK                  (0xFFFFFFFF)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA1_PCU_DATA_MAX                  (0xFFFFFFFF)
  #define PCU_CR_CAMARILLO_MAILBOX_DATA1_PCU_DATA_DEF                  (0x00000000)

#define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_REG                   (0x00005818)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_COMMAND_OFF           ( 0)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_COMMAND_WID           ( 8)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_COMMAND_MSK           (0x000000FF)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_COMMAND_MAX           (0x000000FF)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_COMMAND_DEF           (0x00000000)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_ADDR_CNTL_OFF         ( 8)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_ADDR_CNTL_WID         (21)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_ADDR_CNTL_MSK         (0x1FFFFF00)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_ADDR_CNTL_MAX         (0x001FFFFF)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_ADDR_CNTL_DEF         (0x00000000)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_RUN_BUSY_OFF          (31)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_RUN_BUSY_WID          ( 1)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_RUN_BUSY_MSK          (0x80000000)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_RUN_BUSY_MAX          (0x00000001)
  #define PCU_CR_CAMARILLO_MAILBOX_INTERFACE_PCU_RUN_BUSY_DEF          (0x00000000)

#define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_REG             (0x00005820)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_HIGH_TEMP_INT_ENABLE_OFF ( 0)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_HIGH_TEMP_INT_ENABLE_WID ( 1)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_HIGH_TEMP_INT_ENABLE_MSK (0x00000001)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_HIGH_TEMP_INT_ENABLE_MAX (0x00000001)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_HIGH_TEMP_INT_ENABLE_DEF (0x00000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_LOW_TEMP_INT_ENABLE_OFF ( 1)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_LOW_TEMP_INT_ENABLE_WID ( 1)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_LOW_TEMP_INT_ENABLE_MSK (0x00000002)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_LOW_TEMP_INT_ENABLE_MAX (0x00000001)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_LOW_TEMP_INT_ENABLE_DEF (0x00000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_PROCHOT_INT_ENABLE_OFF ( 2)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_PROCHOT_INT_ENABLE_WID ( 1)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_PROCHOT_INT_ENABLE_MSK (0x00000004)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_PROCHOT_INT_ENABLE_MAX (0x00000001)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_PROCHOT_INT_ENABLE_DEF (0x00000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_OUT_OF_SPEC_INT_ENABLE_OFF ( 4)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_OUT_OF_SPEC_INT_ENABLE_WID ( 1)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_OUT_OF_SPEC_INT_ENABLE_MSK (0x00000010)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_OUT_OF_SPEC_INT_ENABLE_MAX (0x00000001)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_OUT_OF_SPEC_INT_ENABLE_DEF (0x00000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_REL_TEMP_OFF ( 8)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_REL_TEMP_WID ( 7)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_REL_TEMP_MSK (0x00007F00)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_REL_TEMP_MAX (0x0000007F)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_REL_TEMP_DEF (0x00000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_INT_ENABLE_OFF (15)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_INT_ENABLE_WID ( 1)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_INT_ENABLE_MSK (0x00008000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_INT_ENABLE_MAX (0x00000001)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_1_INT_ENABLE_DEF (0x00000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_REL_TEMP_OFF (16)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_REL_TEMP_WID ( 7)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_REL_TEMP_MSK (0x007F0000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_REL_TEMP_MAX (0x0000007F)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_REL_TEMP_DEF (0x00000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_INT_ENABLE_OFF (23)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_INT_ENABLE_WID ( 1)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_INT_ENABLE_MSK (0x00800000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_INT_ENABLE_MAX (0x00000001)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_THRESHOLD_2_INT_ENABLE_DEF (0x00000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_POWER_INT_ENABLE_OFF (24)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_POWER_INT_ENABLE_WID ( 1)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_POWER_INT_ENABLE_MSK (0x01000000)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_POWER_INT_ENABLE_MAX (0x00000001)
  #define PCU_CR_PACKAGE_THERM_CAMARILLO_INTERRUPT_PCU_POWER_INT_ENABLE_DEF (0x00000000)

#define PCU_CR_DDR_PTM_CTL_PCU_REG                                   (0x00005880)
  #define PCU_CR_DDR_PTM_CTL_PCU_OLTM_ENABLE_OFF                       ( 0)
  #define PCU_CR_DDR_PTM_CTL_PCU_OLTM_ENABLE_WID                       ( 1)
  #define PCU_CR_DDR_PTM_CTL_PCU_OLTM_ENABLE_MSK                       (0x00000001)
  #define PCU_CR_DDR_PTM_CTL_PCU_OLTM_ENABLE_MAX                       (0x00000001)
  #define PCU_CR_DDR_PTM_CTL_PCU_OLTM_ENABLE_DEF                       (0x00000000)
  #define PCU_CR_DDR_PTM_CTL_PCU_CLTM_ENABLE_OFF                       ( 1)
  #define PCU_CR_DDR_PTM_CTL_PCU_CLTM_ENABLE_WID                       ( 1)
  #define PCU_CR_DDR_PTM_CTL_PCU_CLTM_ENABLE_MSK                       (0x00000002)
  #define PCU_CR_DDR_PTM_CTL_PCU_CLTM_ENABLE_MAX                       (0x00000001)
  #define PCU_CR_DDR_PTM_CTL_PCU_CLTM_ENABLE_DEF                       (0x00000000)
  #define PCU_CR_DDR_PTM_CTL_PCU_REFRESH_2X_MODE_OFF                   ( 2)
  #define PCU_CR_DDR_PTM_CTL_PCU_REFRESH_2X_MODE_WID                   ( 2)
  #define PCU_CR_DDR_PTM_CTL_PCU_REFRESH_2X_MODE_MSK                   (0x0000000C)
  #define PCU_CR_DDR_PTM_CTL_PCU_REFRESH_2X_MODE_MAX                   (0x00000003)
  #define PCU_CR_DDR_PTM_CTL_PCU_REFRESH_2X_MODE_DEF                   (0x00000000)
  #define PCU_CR_DDR_PTM_CTL_PCU_EXTTS_ENABLE_OFF                      ( 4)
  #define PCU_CR_DDR_PTM_CTL_PCU_EXTTS_ENABLE_WID                      ( 1)
  #define PCU_CR_DDR_PTM_CTL_PCU_EXTTS_ENABLE_MSK                      (0x00000010)
  #define PCU_CR_DDR_PTM_CTL_PCU_EXTTS_ENABLE_MAX                      (0x00000001)
  #define PCU_CR_DDR_PTM_CTL_PCU_EXTTS_ENABLE_DEF                      (0x00000000)
  #define PCU_CR_DDR_PTM_CTL_PCU_LOCK_PTM_REGS_PCU_OFF                 ( 5)
  #define PCU_CR_DDR_PTM_CTL_PCU_LOCK_PTM_REGS_PCU_WID                 ( 1)
  #define PCU_CR_DDR_PTM_CTL_PCU_LOCK_PTM_REGS_PCU_MSK                 (0x00000020)
  #define PCU_CR_DDR_PTM_CTL_PCU_LOCK_PTM_REGS_PCU_MAX                 (0x00000001)
  #define PCU_CR_DDR_PTM_CTL_PCU_LOCK_PTM_REGS_PCU_DEF                 (0x00000000)
  #define PCU_CR_DDR_PTM_CTL_PCU_PDWN_CONFIG_CTL_OFF                   ( 6)
  #define PCU_CR_DDR_PTM_CTL_PCU_PDWN_CONFIG_CTL_WID                   ( 1)
  #define PCU_CR_DDR_PTM_CTL_PCU_PDWN_CONFIG_CTL_MSK                   (0x00000040)
  #define PCU_CR_DDR_PTM_CTL_PCU_PDWN_CONFIG_CTL_MAX                   (0x00000001)
  #define PCU_CR_DDR_PTM_CTL_PCU_PDWN_CONFIG_CTL_DEF                   (0x00000000)
  #define PCU_CR_DDR_PTM_CTL_PCU_DISABLE_DRAM_TS_OFF                   ( 7)
  #define PCU_CR_DDR_PTM_CTL_PCU_DISABLE_DRAM_TS_WID                   ( 1)
  #define PCU_CR_DDR_PTM_CTL_PCU_DISABLE_DRAM_TS_MSK                   (0x00000080)
  #define PCU_CR_DDR_PTM_CTL_PCU_DISABLE_DRAM_TS_MAX                   (0x00000001)
  #define PCU_CR_DDR_PTM_CTL_PCU_DISABLE_DRAM_TS_DEF                   (0x00000000)

#define PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_REG                        (0x00005884)
  #define PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_SCALEFACTOR_OFF            ( 0)
  #define PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_SCALEFACTOR_WID            ( 3)
  #define PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_SCALEFACTOR_MSK            (0x00000007)
  #define PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_SCALEFACTOR_MAX            (0x00000007)
  #define PCU_CR_DDR_ENERGY_SCALEFACTOR_PCU_SCALEFACTOR_DEF            (0x00000003)

#define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_REG                  (0x00005888)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH0_OFF              ( 0)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH0_WID              ( 8)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH0_MSK              (0x000000FF)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH0_MAX              (0x000000FF)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH0_DEF              (0x00000000)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH1_OFF              ( 8)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH1_WID              ( 8)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH1_MSK              (0x0000FF00)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH1_MAX              (0x000000FF)
  #define PCU_CR_DDR_RAPL_CHANNEL_POWER_FLOOR_PCU_CH1_DEF              (0x00000000)

#define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_REG                      (0x0000588C)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM0_OFF            ( 0)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM0_WID            ( 2)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM0_MSK            (0x00000003)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM0_MAX            (0x00000003)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM0_DEF            (0x00000000)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM1_OFF            ( 2)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM1_WID            ( 2)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM1_MSK            (0x0000000C)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM1_MAX            (0x00000003)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH0_DIMM1_DEF            (0x00000000)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM0_OFF            ( 8)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM0_WID            ( 2)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM0_MSK            (0x00000300)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM0_MAX            (0x00000003)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM0_DEF            (0x00000000)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM1_OFF            (10)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM1_WID            ( 2)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM1_MSK            (0x00000C00)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM1_MAX            (0x00000003)
  #define PCU_CR_DDR_THERM_PERDIMM_STATUS_PCU_CH1_DIMM1_DEF            (0x00000000)

#define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_REG                        (0x00005890)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM0_OFF                  ( 0)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM0_WID                  ( 8)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM0_MSK                  (0x000000FF)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM0_MAX                  (0x000000FF)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM0_DEF                  (0x000000FF)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM1_OFF                  ( 8)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM1_WID                  ( 8)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM1_MSK                  (0x0000FF00)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM1_MAX                  (0x000000FF)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH0_PCU_DIMM1_DEF                  (0x000000FF)

#define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_REG                        (0x00005894)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM0_OFF                  ( 0)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM0_WID                  ( 8)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM0_MSK                  (0x000000FF)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM0_MAX                  (0x000000FF)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM0_DEF                  (0x000000FF)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM1_OFF                  ( 8)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM1_WID                  ( 8)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM1_MSK                  (0x0000FF00)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM1_MAX                  (0x000000FF)
  #define PCU_CR_DDR_WARM_THRESHOLD_CH1_PCU_DIMM1_DEF                  (0x000000FF)

#define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_REG                         (0x00005898)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM0_OFF                   ( 0)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM0_WID                   ( 8)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM0_MSK                   (0x000000FF)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM0_MAX                   (0x000000FF)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM0_DEF                   (0x000000FF)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM1_OFF                   ( 8)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM1_WID                   ( 8)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM1_MSK                   (0x0000FF00)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM1_MAX                   (0x000000FF)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH0_PCU_DIMM1_DEF                   (0x000000FF)

#define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_REG                         (0x0000589C)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM0_OFF                   ( 0)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM0_WID                   ( 8)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM0_MSK                   (0x000000FF)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM0_MAX                   (0x000000FF)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM0_DEF                   (0x000000FF)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM1_OFF                   ( 8)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM1_WID                   ( 8)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM1_MSK                   (0x0000FF00)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM1_MAX                   (0x000000FF)
  #define PCU_CR_DDR_HOT_THRESHOLD_CH1_PCU_DIMM1_DEF                   (0x000000FF)

#define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_REG                 (0x000058A0)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_WARM_INTERRUPT_OFF ( 0)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_WARM_INTERRUPT_WID ( 1)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_WARM_INTERRUPT_MSK (0x00000001)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_WARM_INTERRUPT_MAX (0x00000001)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_WARM_INTERRUPT_DEF (0x00000000)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_HOT_INTERRUPT_OFF ( 2)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_HOT_INTERRUPT_WID ( 1)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_HOT_INTERRUPT_MSK (0x00000004)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_HOT_INTERRUPT_MAX (0x00000001)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_HOT_INTERRUPT_DEF (0x00000000)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_2X_REFRESH_INTERRUPT_OFF ( 4)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_2X_REFRESH_INTERRUPT_WID ( 1)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_2X_REFRESH_INTERRUPT_MSK (0x00000010)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_2X_REFRESH_INTERRUPT_MAX (0x00000001)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_2X_REFRESH_INTERRUPT_DEF (0x00000000)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD1_INTERRUPT_OFF ( 8)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD1_INTERRUPT_WID ( 1)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD1_INTERRUPT_MSK (0x00000100)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD1_INTERRUPT_MAX (0x00000001)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD1_INTERRUPT_DEF (0x00000000)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD2_INTERRUPT_OFF (10)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD2_INTERRUPT_WID ( 1)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD2_INTERRUPT_MSK (0x00000400)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD2_INTERRUPT_MAX (0x00000001)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_ENABLE_THRESHOLD2_INTERRUPT_DEF (0x00000000)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD1_OFF (16)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD1_WID ( 8)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD1_MSK (0x00FF0000)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD1_MAX (0x000000FF)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD1_DEF (0x00000000)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD2_OFF (24)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD2_WID ( 8)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD2_MSK (0xFF000000)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD2_MAX (0x000000FF)
  #define PCU_CR_DDR_THERM_CAMARILLO_INTERRUPT_PCU_POLICY_FREE_THRESHOLD2_DEF (0x00000000)

#define PCU_CR_DDR_VOLTAGE_PCU_REG                                   (0x000058A4)
  #define PCU_CR_DDR_VOLTAGE_PCU_DDR_VOLTAGE_OFF                       ( 0)
  #define PCU_CR_DDR_VOLTAGE_PCU_DDR_VOLTAGE_WID                       ( 3)
  #define PCU_CR_DDR_VOLTAGE_PCU_DDR_VOLTAGE_MSK                       (0x00000007)
  #define PCU_CR_DDR_VOLTAGE_PCU_DDR_VOLTAGE_MAX                       (0x00000007)
  #define PCU_CR_DDR_VOLTAGE_PCU_DDR_VOLTAGE_DEF                       (0x00000000)

#define PCU_CR_PACKAGE_THERM_MARGIN_PCU_REG                          (0x000058A8)
  #define PCU_CR_PACKAGE_THERM_MARGIN_PCU_THERM_MARGIN_OFF             ( 0)
  #define PCU_CR_PACKAGE_THERM_MARGIN_PCU_THERM_MARGIN_WID             (16)
  #define PCU_CR_PACKAGE_THERM_MARGIN_PCU_THERM_MARGIN_MSK             (0x0000FFFF)
  #define PCU_CR_PACKAGE_THERM_MARGIN_PCU_THERM_MARGIN_MAX             (0x0000FFFF)
  #define PCU_CR_PACKAGE_THERM_MARGIN_PCU_THERM_MARGIN_DEF             (0x00007F00)

#define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_REG                      (0x000058B0)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM0_OFF                ( 0)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM0_WID                ( 8)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM0_MSK                (0x000000FF)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM0_MAX                (0x000000FF)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM0_DEF                (0x00000000)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM1_OFF                ( 8)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM1_WID                ( 8)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM1_MSK                (0x0000FF00)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM1_MAX                (0x000000FF)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH0_PCU_DIMM1_DEF                (0x00000000)

#define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_REG                      (0x000058B4)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM0_OFF                ( 0)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM0_WID                ( 8)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM0_MSK                (0x000000FF)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM0_MAX                (0x000000FF)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM0_DEF                (0x00000000)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM1_OFF                ( 8)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM1_WID                ( 8)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM1_MSK                (0x0000FF00)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM1_MAX                (0x000000FF)
  #define PCU_CR_DDR_DIMM_TEMPERATURE_CH1_PCU_DIMM1_DEF                (0x00000000)

#define PCU_CR_DDR_DIMM_HOTTEST_ABSOLUTE_PCU_REG                     (0x000058B8)
  #define PCU_CR_DDR_DIMM_HOTTEST_ABSOLUTE_PCU_TEMPERATURE_OFF         ( 0)
  #define PCU_CR_DDR_DIMM_HOTTEST_ABSOLUTE_PCU_TEMPERATURE_WID         ( 8)
  #define PCU_CR_DDR_DIMM_HOTTEST_ABSOLUTE_PCU_TEMPERATURE_MSK         (0x000000FF)
  #define PCU_CR_DDR_DIMM_HOTTEST_ABSOLUTE_PCU_TEMPERATURE_MAX         (0x000000FF)
  #define PCU_CR_DDR_DIMM_HOTTEST_ABSOLUTE_PCU_TEMPERATURE_DEF         (0x00000000)

#define PCU_CR_DDR_DIMM_HOTTEST_RELATIVE_PCU_REG                     (0x000058BC)
  #define PCU_CR_DDR_DIMM_HOTTEST_RELATIVE_PCU_TEMPERATURE_OFF         ( 0)
  #define PCU_CR_DDR_DIMM_HOTTEST_RELATIVE_PCU_TEMPERATURE_WID         ( 8)
  #define PCU_CR_DDR_DIMM_HOTTEST_RELATIVE_PCU_TEMPERATURE_MSK         (0x000000FF)
  #define PCU_CR_DDR_DIMM_HOTTEST_RELATIVE_PCU_TEMPERATURE_MAX         (0x000000FF)
  #define PCU_CR_DDR_DIMM_HOTTEST_RELATIVE_PCU_TEMPERATURE_DEF         (0x0000007F)

#define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_REG                     (0x000058C0)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM0_OFF               ( 0)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM0_WID               (16)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM0_MSK               (0x0000FFFF)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM0_MAX               (0x0000FFFF)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM0_DEF               (0x00000000)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM1_OFF               (16)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM1_WID               (16)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM1_MSK               (0xFFFF0000)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM1_MAX               (0x0000FFFF)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH0_PCU_DIMM1_DEF               (0x00000000)

#define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_REG                     (0x000058C8)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM0_OFF               ( 0)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM0_WID               (16)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM0_MSK               (0x0000FFFF)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM0_MAX               (0x0000FFFF)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM0_DEF               (0x00000000)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM1_OFF               (16)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM1_WID               (16)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM1_MSK               (0xFFFF0000)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM1_MAX               (0x0000FFFF)
  #define PCU_CR_DDR_THROTTLE_DURATION_CH1_PCU_DIMM1_DEF               (0x00000000)

#define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_REG                           (0x000058D0)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM0_OFF                     ( 0)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM0_WID                     ( 8)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM0_MSK                     (0x000000FF)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM0_MAX                     (0x000000FF)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM0_DEF                     (0x000000FF)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM1_OFF                     ( 8)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM1_WID                     ( 8)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM1_MSK                     (0x0000FF00)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM1_MAX                     (0x000000FF)
  #define PCU_CR_DDR_WARM_BUDGET_CH0_PCU_DIMM1_DEF                     (0x000000FF)

#define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_REG                           (0x000058D4)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM0_OFF                     ( 0)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM0_WID                     ( 8)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM0_MSK                     (0x000000FF)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM0_MAX                     (0x000000FF)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM0_DEF                     (0x000000FF)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM1_OFF                     ( 8)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM1_WID                     ( 8)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM1_MSK                     (0x0000FF00)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM1_MAX                     (0x000000FF)
  #define PCU_CR_DDR_WARM_BUDGET_CH1_PCU_DIMM1_DEF                     (0x000000FF)

#define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_REG                            (0x000058D8)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM0_OFF                      ( 0)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM0_WID                      ( 8)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM0_MSK                      (0x000000FF)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM0_MAX                      (0x000000FF)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM0_DEF                      (0x000000FF)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM1_OFF                      ( 8)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM1_WID                      ( 8)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM1_MSK                      (0x0000FF00)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM1_MAX                      (0x000000FF)
  #define PCU_CR_DDR_HOT_BUDGET_CH0_PCU_DIMM1_DEF                      (0x000000FF)

#define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_REG                            (0x000058DC)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM0_OFF                      ( 0)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM0_WID                      ( 8)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM0_MSK                      (0x000000FF)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM0_MAX                      (0x000000FF)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM0_DEF                      (0x000000FF)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM1_OFF                      ( 8)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM1_WID                      ( 8)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM1_MSK                      (0x0000FF00)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM1_MAX                      (0x000000FF)
  #define PCU_CR_DDR_HOT_BUDGET_CH1_PCU_DIMM1_DEF                      (0x000000FF)

#define PCU_CR_DDR_RAPL_LIMIT_PCU_REG                                (0x000058E0)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_POWER_OFF                   ( 0)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_POWER_WID                   (15)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_POWER_MSK                   (0x00007FFF)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_POWER_MAX                   (0x00007FFF)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_POWER_DEF                   (0x00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_ENABLE_OFF                  (15)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_ENABLE_WID                  ( 1)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_ENABLE_MSK                  (0x00008000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_ENABLE_MAX                  (0x00000001)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_ENABLE_DEF                  (0x00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_Y_OFF           (17)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_Y_WID           ( 5)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_Y_MSK           (0x003E0000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_Y_MAX           (0x0000001F)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_Y_DEF           (0x00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_X_OFF           (22)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_X_WID           ( 2)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_X_MSK           (0x00C00000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_X_MAX           (0x00000003)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT1_TIME_WINDOW_X_DEF           (0x00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_POWER_OFF                   (32)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_POWER_WID                   (15)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_POWER_MSK                   (0x7FFF00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_POWER_MAX                   (0x00007FFF)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_POWER_DEF                   (0x00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_ENABLE_OFF                  (47)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_ENABLE_WID                  ( 1)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_ENABLE_MSK                  (0x800000000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_ENABLE_MAX                  (0x00000001)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_ENABLE_DEF                  (0x00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_Y_OFF           (49)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_Y_WID           ( 5)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_Y_MSK           (0x3E000000000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_Y_MAX           (0x0000001F)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_Y_DEF           (0x00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_X_OFF           (54)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_X_WID           ( 2)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_X_MSK           (0xC0000000000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_X_MAX           (0x00000003)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LIMIT2_TIME_WINDOW_X_DEF           (0x00000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LOCKED_OFF                         (63)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LOCKED_WID                         ( 1)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LOCKED_MSK                         (0x8000000000000000)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LOCKED_MAX                         (0x00000001)
  #define PCU_CR_DDR_RAPL_LIMIT_PCU_LOCKED_DEF                         (0x00000000)

#define PCU_CR_DDR_ENERGY_STATUS_PCU_REG                             (0x000058E8)
  #define PCU_CR_DDR_ENERGY_STATUS_PCU_JOULES_CONSUMED_OFF             ( 0)
  #define PCU_CR_DDR_ENERGY_STATUS_PCU_JOULES_CONSUMED_WID             (32)
  #define PCU_CR_DDR_ENERGY_STATUS_PCU_JOULES_CONSUMED_MSK             (0xFFFFFFFF)
  #define PCU_CR_DDR_ENERGY_STATUS_PCU_JOULES_CONSUMED_MAX             (0xFFFFFFFF)
  #define PCU_CR_DDR_ENERGY_STATUS_PCU_JOULES_CONSUMED_DEF             (0x00000000)

#define PCU_CR_DDR_RAPL_PERF_STATUS_PCU_REG                          (0x000058EC)
  #define PCU_CR_DDR_RAPL_PERF_STATUS_PCU_DURATION_OFF                 ( 0)
  #define PCU_CR_DDR_RAPL_PERF_STATUS_PCU_DURATION_WID                 (32)
  #define PCU_CR_DDR_RAPL_PERF_STATUS_PCU_DURATION_MSK                 (0xFFFFFFFF)
  #define PCU_CR_DDR_RAPL_PERF_STATUS_PCU_DURATION_MAX                 (0xFFFFFFFF)
  #define PCU_CR_DDR_RAPL_PERF_STATUS_PCU_DURATION_DEF                 (0x00000000)

#define PCU_CR_PACKAGE_RAPL_PERF_STATUS_PCU_REG                      (0x000058F0)
  #define PCU_CR_PACKAGE_RAPL_PERF_STATUS_PCU_COUNTS_OFF               ( 0)
  #define PCU_CR_PACKAGE_RAPL_PERF_STATUS_PCU_COUNTS_WID               (32)
  #define PCU_CR_PACKAGE_RAPL_PERF_STATUS_PCU_COUNTS_MSK               (0xFFFFFFFF)
  #define PCU_CR_PACKAGE_RAPL_PERF_STATUS_PCU_COUNTS_MAX               (0xFFFFFFFF)
  #define PCU_CR_PACKAGE_RAPL_PERF_STATUS_PCU_COUNTS_DEF               (0x00000000)

#define PCU_CR_GT_RATIOS_OVERRIDE_PCU_REG                            (0x000058F4)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_IA_MIN_RATIO_REQUEST_OFF       ( 0)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_IA_MIN_RATIO_REQUEST_WID       ( 8)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_IA_MIN_RATIO_REQUEST_MSK       (0x000000FF)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_IA_MIN_RATIO_REQUEST_MAX       (0x000000FF)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_IA_MIN_RATIO_REQUEST_DEF       (0x00000000)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_CLR_MIN_RATIO_REQUEST_OFF      ( 8)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_CLR_MIN_RATIO_REQUEST_WID      ( 8)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_CLR_MIN_RATIO_REQUEST_MSK      (0x0000FF00)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_CLR_MIN_RATIO_REQUEST_MAX      (0x000000FF)
  #define PCU_CR_GT_RATIOS_OVERRIDE_PCU_CLR_MIN_RATIO_REQUEST_DEF      (0x00000000)

#define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_REG                 (0x000058F8)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_DDR_ACCESS_TIME_OFF ( 0)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_DDR_ACCESS_TIME_WID (14)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_DDR_ACCESS_TIME_MSK (0x00003FFF)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_DDR_ACCESS_TIME_MAX (0x00003FFF)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_DDR_ACCESS_TIME_DEF (0x00000000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_RESERVED_OFF        (14)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_RESERVED_WID        ( 1)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_RESERVED_MSK        (0x00004000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_RESERVED_MAX        (0x00000001)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_RESERVED_DEF        (0x00000000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_CLR_ACCESS_TIME_OFF (15)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_CLR_ACCESS_TIME_WID (14)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_CLR_ACCESS_TIME_MSK (0x1FFF8000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_CLR_ACCESS_TIME_MAX (0x00003FFF)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_CLR_ACCESS_TIME_DEF (0x00000000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_NON_SNOOP_THRESHOLD_RESOLUTION_OFF (29)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_NON_SNOOP_THRESHOLD_RESOLUTION_WID ( 1)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_NON_SNOOP_THRESHOLD_RESOLUTION_MSK (0x20000000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_NON_SNOOP_THRESHOLD_RESOLUTION_MAX (0x00000001)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_NON_SNOOP_THRESHOLD_RESOLUTION_DEF (0x00000000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_SLOW_SNOOP_THRESHOLD_RESOLUTION_OFF (30)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_SLOW_SNOOP_THRESHOLD_RESOLUTION_WID ( 1)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_SLOW_SNOOP_THRESHOLD_RESOLUTION_MSK (0x40000000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_SLOW_SNOOP_THRESHOLD_RESOLUTION_MAX (0x00000001)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_SLOW_SNOOP_THRESHOLD_RESOLUTION_DEF (0x00000000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_FAST_SNOOP_THRESHOLD_RESOLUTION_OFF (31)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_FAST_SNOOP_THRESHOLD_RESOLUTION_WID ( 1)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_FAST_SNOOP_THRESHOLD_RESOLUTION_MSK (0x80000000)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_FAST_SNOOP_THRESHOLD_RESOLUTION_MAX (0x00000001)
  #define PCU_CR_PKGC_LTR_LATENCY_MEASUREMENTS_PCU_FAST_SNOOP_THRESHOLD_RESOLUTION_DEF (0x00000000)

#define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_REG                         (0x000058FC)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_OFF          ( 0)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_WID          ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_MSK          (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_MAX          (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_DEF          (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_OFF          ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_WID          ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_MSK          (0x00000002)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_MAX          (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_DEF          (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_2_OFF              ( 2)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_2_WID              ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_2_MSK              (0x00000004)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_2_MAX              (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_2_DEF              (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_OFF          ( 3)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_WID          ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_MSK          (0x00000008)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_MAX          (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_DEF          (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_OFF          ( 4)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_WID          ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_MSK          (0x00000010)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_MAX          (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_DEF          (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_STATUS_OFF         ( 5)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_STATUS_WID         ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_STATUS_MSK         (0x00000020)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_STATUS_MAX         (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_STATUS_DEF         (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_6_OFF              ( 6)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_6_WID              ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_6_MSK              (0x00000040)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_6_MAX              (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_6_DEF              (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_STATUS_OFF         ( 7)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_STATUS_WID         ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_STATUS_MSK         (0x00000080)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_STATUS_MAX         (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_STATUS_DEF         (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_OFF    ( 8)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_WID    ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_MSK    (0x00000100)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_MAX    (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_DEF    (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_STATUS_OFF ( 9)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_STATUS_WID ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_STATUS_MSK (0x00000200)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_STATUS_MAX (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_STATUS_DEF (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_OFF          (10)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_WID          ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_MSK          (0x00000400)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_MAX          (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_DEF          (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_STATUS_OFF      (11)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_STATUS_WID      ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_STATUS_MSK      (0x00000800)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_STATUS_MAX      (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_STATUS_DEF      (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_12_OFF             (12)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_12_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_12_MSK             (0x00001000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_12_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_12_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_13_OFF             (13)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_13_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_13_MSK             (0x00002000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_13_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_13_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_14_OFF             (14)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_14_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_14_MSK             (0x00004000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_14_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_14_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_15_OFF             (15)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_15_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_15_MSK             (0x00008000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_15_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_15_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_OFF             (16)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_MSK             (0x00010000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_OFF             (17)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_MSK             (0x00020000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_2_OFF          (18)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_2_WID          ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_2_MSK          (0x00040000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_2_MAX          (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_2_DEF          (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_OFF             (19)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_MSK             (0x00080000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_OFF             (20)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_MSK             (0x00100000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_LOG_OFF            (21)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_LOG_WID            ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_LOG_MSK            (0x00200000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_LOG_MAX            (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_PBM_PLIA_LOG_DEF            (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_6_OFF          (22)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_6_WID          ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_6_MSK          (0x00400000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_6_MAX          (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_6_DEF          (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_LOG_OFF            (23)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_LOG_WID            ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_LOG_MSK            (0x00800000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_LOG_MAX            (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_GTDRIVER_LOG_DEF            (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_OFF       (24)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_WID       ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_MSK       (0x01000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_MAX       (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_DEF       (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_LOG_OFF (25)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_LOG_WID ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_LOG_MSK (0x02000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_LOG_MAX (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_FUSE_MAX_TURBO_LIMIT_LOG_DEF (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_OFF             (26)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_WID             ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_MSK             (0x04000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_MAX             (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_DEF             (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_LOG_OFF         (27)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_LOG_WID         ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_LOG_MSK         (0x08000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_LOG_MAX         (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_TURBO_ATTEN_LOG_DEF         (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_12_OFF         (28)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_12_WID         ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_12_MSK         (0x10000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_12_MAX         (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_12_DEF         (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_13_OFF         (29)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_13_WID         ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_13_MSK         (0x20000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_13_MAX         (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_13_DEF         (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_14_OFF         (30)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_14_WID         ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_14_MSK         (0x40000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_14_MAX         (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_14_DEF         (0x00000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_15_OFF         (31)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_15_WID         ( 1)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_15_MSK         (0x80000000)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_15_MAX         (0x00000001)
  #define PCU_CR_IA_PERF_LIMIT_REASONS_PCU_SPARE_IA_LOG_15_DEF         (0x00000000)

#define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_REG                         (0x00005900)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_OFF          ( 0)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_MSK          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_OFF          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_MSK          (0x00000002)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_2_OFF              ( 2)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_2_WID              ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_2_MSK              (0x00000004)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_2_MAX              (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_2_DEF              (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_OFF          ( 3)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_MSK          (0x00000008)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_OFF          ( 4)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_MSK          (0x00000010)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_STATUS_OFF         ( 5)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_STATUS_WID         ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_STATUS_MSK         (0x00000020)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_STATUS_MAX         (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_STATUS_DEF         (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_6_OFF              ( 6)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_6_WID              ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_6_MSK              (0x00000040)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_6_MAX              (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_6_DEF              (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_7_OFF              ( 7)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_7_WID              ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_7_MSK              (0x00000080)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_7_MAX              (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_7_DEF              (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_OFF    ( 8)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_WID    ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_MSK    (0x00000100)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_MAX    (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_DEF    (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_9_OFF              ( 9)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_9_WID              ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_9_MSK              (0x00000200)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_9_MAX              (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_9_DEF              (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_OFF          (10)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_MSK          (0x00000400)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_11_OFF             (11)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_11_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_11_MSK             (0x00000800)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_11_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_11_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_12_OFF             (12)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_12_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_12_MSK             (0x00001000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_12_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_12_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_13_OFF             (13)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_13_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_13_MSK             (0x00002000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_13_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_13_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_14_OFF             (14)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_14_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_14_MSK             (0x00004000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_14_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_14_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_15_OFF             (15)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_15_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_15_MSK             (0x00008000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_15_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_15_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_OFF             (16)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_MSK             (0x00010000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_OFF             (17)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_MSK             (0x00020000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_2_OFF          (18)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_2_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_2_MSK          (0x00040000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_2_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_2_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_OFF             (19)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_MSK             (0x00080000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_OFF             (20)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_MSK             (0x00100000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_LOG_OFF            (21)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_LOG_WID            ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_LOG_MSK            (0x00200000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_LOG_MAX            (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_PBM_PLGT_LOG_DEF            (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_6_OFF          (22)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_6_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_6_MSK          (0x00400000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_6_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_6_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_7_OFF          (23)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_7_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_7_MSK          (0x00800000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_7_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_7_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_OFF       (24)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_WID       ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_MSK       (0x01000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_MAX       (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_DEF       (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_9_OFF          (25)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_9_WID          ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_9_MSK          (0x02000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_9_MAX          (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_9_DEF          (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_OFF             (26)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_WID             ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_MSK             (0x04000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_MAX             (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_DEF             (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_11_OFF         (27)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_11_WID         ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_11_MSK         (0x08000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_11_MAX         (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_11_DEF         (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_12_OFF         (28)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_12_WID         ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_12_MSK         (0x10000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_12_MAX         (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_12_DEF         (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_13_OFF         (29)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_13_WID         ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_13_MSK         (0x20000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_13_MAX         (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_13_DEF         (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_14_OFF         (30)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_14_WID         ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_14_MSK         (0x40000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_14_MAX         (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_14_DEF         (0x00000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_15_OFF         (31)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_15_WID         ( 1)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_15_MSK         (0x80000000)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_15_MAX         (0x00000001)
  #define PCU_CR_GT_PERF_LIMIT_REASONS_PCU_SPARE_GT_LOG_15_DEF         (0x00000000)

#define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_REG                        (0x00005904)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_OFF         ( 0)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_WID         ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_MSK         (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_MAX         (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_STATUS_DEF         (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_OFF         ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_WID         ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_MSK         (0x00000002)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_MAX         (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_STATUS_DEF         (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_2_OFF            ( 2)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_2_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_2_MSK            (0x00000004)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_2_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_2_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_OFF         ( 3)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_WID         ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_MSK         (0x00000008)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_MAX         (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_STATUS_DEF         (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_OFF         ( 4)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_WID         ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_MSK         (0x00000010)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_MAX         (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_STATUS_DEF         (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_5_OFF            ( 5)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_5_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_5_MSK            (0x00000020)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_5_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_5_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_6_OFF            ( 6)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_6_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_6_MSK            (0x00000040)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_6_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_6_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_7_OFF            ( 7)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_7_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_7_MSK            (0x00000080)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_7_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_7_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_OFF   ( 8)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_WID   ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_MSK   (0x00000100)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_MAX   (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_STATUS_DEF   (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_9_OFF            ( 9)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_9_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_9_MSK            (0x00000200)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_9_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_9_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_OFF         (10)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_WID         ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_MSK         (0x00000400)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_MAX         (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_STATUS_DEF         (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_11_OFF           (11)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_11_WID           ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_11_MSK           (0x00000800)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_11_MAX           (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_11_DEF           (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_12_OFF           (12)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_12_WID           ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_12_MSK           (0x00001000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_12_MAX           (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_12_DEF           (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_13_OFF           (13)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_13_WID           ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_13_MSK           (0x00002000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_13_MAX           (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_13_DEF           (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_14_OFF           (14)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_14_WID           ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_14_MSK           (0x00004000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_14_MAX           (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_14_DEF           (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_15_OFF           (15)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_15_WID           ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_15_MSK           (0x00008000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_15_MAX           (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_15_DEF           (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_OFF            (16)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_MSK            (0x00010000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PROCHOT_LOG_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_OFF            (17)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_MSK            (0x00020000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_THERMAL_LOG_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_2_OFF        (18)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_2_WID        ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_2_MSK        (0x00040000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_2_MAX        (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_2_DEF        (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_OFF            (19)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_MSK            (0x00080000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL1_LOG_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_OFF            (20)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_MSK            (0x00100000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_PBM_PL2_LOG_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_5_OFF        (21)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_5_WID        ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_5_MSK        (0x00200000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_5_MAX        (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_5_DEF        (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_6_OFF        (22)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_6_WID        ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_6_MSK        (0x00400000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_6_MAX        (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_6_DEF        (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_7_OFF        (23)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_7_WID        ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_7_MSK        (0x00800000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_7_MAX        (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_7_DEF        (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_OFF      (24)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_WID      ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_MSK      (0x01000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_MAX      (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_VR_THERMALERT_LOG_DEF      (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_9_OFF        (25)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_9_WID        ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_9_MSK        (0x02000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_9_MAX        (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_9_DEF        (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_OFF            (26)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_WID            ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_MSK            (0x04000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_MAX            (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_EDP_ICC_LOG_DEF            (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_11_OFF       (27)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_11_WID       ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_11_MSK       (0x08000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_11_MAX       (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_11_DEF       (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_12_OFF       (28)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_12_WID       ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_12_MSK       (0x10000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_12_MAX       (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_12_DEF       (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_13_OFF       (29)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_13_WID       ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_13_MSK       (0x20000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_13_MAX       (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_13_DEF       (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_14_OFF       (30)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_14_WID       ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_14_MSK       (0x40000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_14_MAX       (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_14_DEF       (0x00000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_15_OFF       (31)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_15_WID       ( 1)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_15_MSK       (0x80000000)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_15_MAX       (0x00000001)
  #define PCU_CR_CLR_PERF_LIMIT_REASONS_PCU_SPARE_CLR_LOG_15_DEF       (0x00000000)

#define PCU_CR_PRIP_TURBO_PLCY_PCU_REG                               (0x00005920)
  #define PCU_CR_PRIP_TURBO_PLCY_PCU_PRIPTP_OFF                        ( 0)
  #define PCU_CR_PRIP_TURBO_PLCY_PCU_PRIPTP_WID                        ( 5)
  #define PCU_CR_PRIP_TURBO_PLCY_PCU_PRIPTP_MSK                        (0x0000001F)
  #define PCU_CR_PRIP_TURBO_PLCY_PCU_PRIPTP_MAX                        (0x0000001F)
  #define PCU_CR_PRIP_TURBO_PLCY_PCU_PRIPTP_DEF                        (0x00000000)

#define PCU_CR_SECP_TURBO_PLCY_PCU_REG                               (0x00005924)
  #define PCU_CR_SECP_TURBO_PLCY_PCU_SECPTP_OFF                        ( 0)
  #define PCU_CR_SECP_TURBO_PLCY_PCU_SECPTP_WID                        ( 5)
  #define PCU_CR_SECP_TURBO_PLCY_PCU_SECPTP_MSK                        (0x0000001F)
  #define PCU_CR_SECP_TURBO_PLCY_PCU_SECPTP_MAX                        (0x0000001F)
  #define PCU_CR_SECP_TURBO_PLCY_PCU_SECPTP_DEF                        (0x00000010)

#define PCU_CR_PRIP_NRG_STTS_PCU_REG                                 (0x00005928)
  #define PCU_CR_PRIP_NRG_STTS_PCU_DATA_OFF                            ( 0)
  #define PCU_CR_PRIP_NRG_STTS_PCU_DATA_WID                            (32)
  #define PCU_CR_PRIP_NRG_STTS_PCU_DATA_MSK                            (0xFFFFFFFF)
  #define PCU_CR_PRIP_NRG_STTS_PCU_DATA_MAX                            (0xFFFFFFFF)
  #define PCU_CR_PRIP_NRG_STTS_PCU_DATA_DEF                            (0x00000000)

#define PCU_CR_SECP_NRG_STTS_PCU_REG                                 (0x0000592C)
  #define PCU_CR_SECP_NRG_STTS_PCU_DATA_OFF                            ( 0)
  #define PCU_CR_SECP_NRG_STTS_PCU_DATA_WID                            (32)
  #define PCU_CR_SECP_NRG_STTS_PCU_DATA_MSK                            (0xFFFFFFFF)
  #define PCU_CR_SECP_NRG_STTS_PCU_DATA_MAX                            (0xFFFFFFFF)
  #define PCU_CR_SECP_NRG_STTS_PCU_DATA_DEF                            (0x00000000)

#define PCU_CR_PACKAGE_POWER_SKU_PCU_REG                             (0x00005930)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_TDP_OFF                     ( 0)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_TDP_WID                     (15)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_TDP_MSK                     (0x00007FFF)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_TDP_MAX                     (0x00007FFF)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_TDP_DEF                     (0x00000118)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MIN_PWR_OFF                 (16)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MIN_PWR_WID                 (15)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MIN_PWR_MSK                 (0x7FFF0000)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MIN_PWR_MAX                 (0x00007FFF)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MIN_PWR_DEF                 (0x00000060)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_PWR_OFF                 (32)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_PWR_WID                 (15)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_PWR_MSK                 (0x7FFF00000000)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_PWR_MAX                 (0x00007FFF)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_PWR_DEF                 (0x00000240)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_WIN_OFF                 (48)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_WIN_WID                 ( 7)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_WIN_MSK                 (0x7F000000000000)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_WIN_MAX                 (0x0000007F)
  #define PCU_CR_PACKAGE_POWER_SKU_PCU_PKG_MAX_WIN_DEF                 (0x00000012)

#define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_REG                        (0x00005938)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_PWR_UNIT_OFF               ( 0)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_PWR_UNIT_WID               ( 4)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_PWR_UNIT_MSK               (0x0000000F)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_PWR_UNIT_MAX               (0x0000000F)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_PWR_UNIT_DEF               (0x00000003)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_ENERGY_UNIT_OFF            ( 8)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_ENERGY_UNIT_WID            ( 5)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_ENERGY_UNIT_MSK            (0x00001F00)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_ENERGY_UNIT_MAX            (0x0000001F)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_ENERGY_UNIT_DEF            (0x0000000E)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_TIME_UNIT_OFF              (16)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_TIME_UNIT_WID              ( 4)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_TIME_UNIT_MSK              (0x000F0000)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_TIME_UNIT_MAX              (0x0000000F)
  #define PCU_CR_PACKAGE_POWER_SKU_UNIT_PCU_TIME_UNIT_DEF              (0x0000000A)

#define PCU_CR_PACKAGE_ENERGY_STATUS_PCU_REG                         (0x0000593C)
  #define PCU_CR_PACKAGE_ENERGY_STATUS_PCU_DATA_OFF                    ( 0)
  #define PCU_CR_PACKAGE_ENERGY_STATUS_PCU_DATA_WID                    (32)
  #define PCU_CR_PACKAGE_ENERGY_STATUS_PCU_DATA_MSK                    (0xFFFFFFFF)
  #define PCU_CR_PACKAGE_ENERGY_STATUS_PCU_DATA_MAX                    (0xFFFFFFFF)
  #define PCU_CR_PACKAGE_ENERGY_STATUS_PCU_DATA_DEF                    (0x00000000)

#define PCU_CR_GT_IO_BUSYNESS_PCU_REG                                (0x00005940)
  #define PCU_CR_GT_IO_BUSYNESS_PCU_DATA_OFF                           ( 0)
  #define PCU_CR_GT_IO_BUSYNESS_PCU_DATA_WID                           (32)
  #define PCU_CR_GT_IO_BUSYNESS_PCU_DATA_MSK                           (0xFFFFFFFF)
  #define PCU_CR_GT_IO_BUSYNESS_PCU_DATA_MAX                           (0xFFFFFFFF)
  #define PCU_CR_GT_IO_BUSYNESS_PCU_DATA_DEF                           (0x00000000)

#define PCU_CR_GT_VIDEO_BUSYNESS_PCU_REG                             (0x00005944)
  #define PCU_CR_GT_VIDEO_BUSYNESS_PCU_DATA_OFF                        ( 0)
  #define PCU_CR_GT_VIDEO_BUSYNESS_PCU_DATA_WID                        (32)
  #define PCU_CR_GT_VIDEO_BUSYNESS_PCU_DATA_MSK                        (0xFFFFFFFF)
  #define PCU_CR_GT_VIDEO_BUSYNESS_PCU_DATA_MAX                        (0xFFFFFFFF)
  #define PCU_CR_GT_VIDEO_BUSYNESS_PCU_DATA_DEF                        (0x00000000)

#define PCU_CR_GT_PERF_STATUS_PCU_REG                                (0x00005948)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_VOLTAGE_OFF               ( 0)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_VOLTAGE_WID               ( 8)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_VOLTAGE_MSK               (0x000000FF)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_VOLTAGE_MAX               (0x000000FF)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_VOLTAGE_DEF               (0x00000000)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_RATIO_OFF                 ( 8)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_RATIO_WID                 ( 8)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_RATIO_MSK                 (0x0000FF00)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_RATIO_MAX                 (0x000000FF)
  #define PCU_CR_GT_PERF_STATUS_PCU_RP_STATE_RATIO_DEF                 (0x00000000)

#define PCU_CR_PLATFORM_ID_PCU_REG                                   (0x00005950)
  #define PCU_CR_PLATFORM_ID_PCU_PLATFORMID_OFF                        (50)
  #define PCU_CR_PLATFORM_ID_PCU_PLATFORMID_WID                        ( 3)
  #define PCU_CR_PLATFORM_ID_PCU_PLATFORMID_MSK                        (0x1C000000000000)
  #define PCU_CR_PLATFORM_ID_PCU_PLATFORMID_MAX                        (0x00000007)
  #define PCU_CR_PLATFORM_ID_PCU_PLATFORMID_DEF                        (0x00000000)

#define PCU_CR_PLATFORM_INFO_PCU_REG                                 (0x00005958)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_NON_TURBO_LIM_RATIO_OFF         ( 8)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_NON_TURBO_LIM_RATIO_WID         ( 8)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_NON_TURBO_LIM_RATIO_MSK         (0x0000FF00)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_NON_TURBO_LIM_RATIO_MAX         (0x000000FF)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_NON_TURBO_LIM_RATIO_DEF         (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_SMM_SAVE_CAP_OFF                    (16)
  #define PCU_CR_PLATFORM_INFO_PCU_SMM_SAVE_CAP_WID                    ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_SMM_SAVE_CAP_MSK                    (0x00010000)
  #define PCU_CR_PLATFORM_INFO_PCU_SMM_SAVE_CAP_MAX                    (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_SMM_SAVE_CAP_DEF                    (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_OCVOLT_OVRD_AVAIL_OFF               (24)
  #define PCU_CR_PLATFORM_INFO_PCU_OCVOLT_OVRD_AVAIL_WID               ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_OCVOLT_OVRD_AVAIL_MSK               (0x01000000)
  #define PCU_CR_PLATFORM_INFO_PCU_OCVOLT_OVRD_AVAIL_MAX               (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_OCVOLT_OVRD_AVAIL_DEF               (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_FIVR_RFI_TUNING_AVAIL_OFF           (25)
  #define PCU_CR_PLATFORM_INFO_PCU_FIVR_RFI_TUNING_AVAIL_WID           ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_FIVR_RFI_TUNING_AVAIL_MSK           (0x02000000)
  #define PCU_CR_PLATFORM_INFO_PCU_FIVR_RFI_TUNING_AVAIL_MAX           (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_FIVR_RFI_TUNING_AVAIL_DEF           (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_DCU_16K_MODE_AVAIL_OFF              (26)
  #define PCU_CR_PLATFORM_INFO_PCU_DCU_16K_MODE_AVAIL_WID              ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_DCU_16K_MODE_AVAIL_MSK              (0x04000000)
  #define PCU_CR_PLATFORM_INFO_PCU_DCU_16K_MODE_AVAIL_MAX              (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_DCU_16K_MODE_AVAIL_DEF              (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_SAMPLE_PART_OFF                     (27)
  #define PCU_CR_PLATFORM_INFO_PCU_SAMPLE_PART_WID                     ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_SAMPLE_PART_MSK                     (0x08000000)
  #define PCU_CR_PLATFORM_INFO_PCU_SAMPLE_PART_MAX                     (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_SAMPLE_PART_DEF                     (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TURBO_RATIO_EN_OFF              (28)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TURBO_RATIO_EN_WID              ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TURBO_RATIO_EN_MSK              (0x10000000)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TURBO_RATIO_EN_MAX              (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TURBO_RATIO_EN_DEF              (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TDP_LIM_EN_OFF                  (29)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TDP_LIM_EN_WID                  ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TDP_LIM_EN_MSK                  (0x20000000)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TDP_LIM_EN_MAX                  (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TDP_LIM_EN_DEF                  (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TJ_OFFSET_EN_OFF                (30)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TJ_OFFSET_EN_WID                ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TJ_OFFSET_EN_MSK                (0x40000000)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TJ_OFFSET_EN_MAX                (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_PRG_TJ_OFFSET_EN_DEF                (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_CPUID_FAULTING_EN_OFF               (31)
  #define PCU_CR_PLATFORM_INFO_PCU_CPUID_FAULTING_EN_WID               ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_CPUID_FAULTING_EN_MSK               (0x80000000)
  #define PCU_CR_PLATFORM_INFO_PCU_CPUID_FAULTING_EN_MAX               (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_CPUID_FAULTING_EN_DEF               (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_LPM_SUPPORT_OFF                     (32)
  #define PCU_CR_PLATFORM_INFO_PCU_LPM_SUPPORT_WID                     ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_LPM_SUPPORT_MSK                     (0x100000000)
  #define PCU_CR_PLATFORM_INFO_PCU_LPM_SUPPORT_MAX                     (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_LPM_SUPPORT_DEF                     (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_CONFIG_TDP_LEVELS_OFF               (33)
  #define PCU_CR_PLATFORM_INFO_PCU_CONFIG_TDP_LEVELS_WID               ( 2)
  #define PCU_CR_PLATFORM_INFO_PCU_CONFIG_TDP_LEVELS_MSK               (0x600000000)
  #define PCU_CR_PLATFORM_INFO_PCU_CONFIG_TDP_LEVELS_MAX               (0x00000003)
  #define PCU_CR_PLATFORM_INFO_PCU_CONFIG_TDP_LEVELS_DEF               (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_PFAT_ENABLE_OFF                     (35)
  #define PCU_CR_PLATFORM_INFO_PCU_PFAT_ENABLE_WID                     ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_PFAT_ENABLE_MSK                     (0x800000000)
  #define PCU_CR_PLATFORM_INFO_PCU_PFAT_ENABLE_MAX                     (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_PFAT_ENABLE_DEF                     (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_TIMED_MWAIT_ENABLE_OFF              (37)
  #define PCU_CR_PLATFORM_INFO_PCU_TIMED_MWAIT_ENABLE_WID              ( 1)
  #define PCU_CR_PLATFORM_INFO_PCU_TIMED_MWAIT_ENABLE_MSK              (0x2000000000)
  #define PCU_CR_PLATFORM_INFO_PCU_TIMED_MWAIT_ENABLE_MAX              (0x00000001)
  #define PCU_CR_PLATFORM_INFO_PCU_TIMED_MWAIT_ENABLE_DEF              (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_EFFICIENCY_RATIO_OFF            (40)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_EFFICIENCY_RATIO_WID            ( 8)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_EFFICIENCY_RATIO_MSK            (0xFF0000000000)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_EFFICIENCY_RATIO_MAX            (0x000000FF)
  #define PCU_CR_PLATFORM_INFO_PCU_MAX_EFFICIENCY_RATIO_DEF            (0x00000000)
  #define PCU_CR_PLATFORM_INFO_PCU_MIN_OPERATING_RATIO_OFF             (48)
  #define PCU_CR_PLATFORM_INFO_PCU_MIN_OPERATING_RATIO_WID             ( 8)
  #define PCU_CR_PLATFORM_INFO_PCU_MIN_OPERATING_RATIO_MSK             (0xFF000000000000)
  #define PCU_CR_PLATFORM_INFO_PCU_MIN_OPERATING_RATIO_MAX             (0x000000FF)
  #define PCU_CR_PLATFORM_INFO_PCU_MIN_OPERATING_RATIO_DEF             (0x00000008)

#define PCU_CR_PP1_C0_CORE_CLOCK_PCU_REG                             (0x00005960)
  #define PCU_CR_PP1_C0_CORE_CLOCK_PCU_DATA_OFF                        ( 0)
  #define PCU_CR_PP1_C0_CORE_CLOCK_PCU_DATA_WID                        (32)
  #define PCU_CR_PP1_C0_CORE_CLOCK_PCU_DATA_MSK                        (0xFFFFFFFF)
  #define PCU_CR_PP1_C0_CORE_CLOCK_PCU_DATA_MAX                        (0xFFFFFFFF)
  #define PCU_CR_PP1_C0_CORE_CLOCK_PCU_DATA_DEF                        (0x00000000)

#define PCU_CR_PP0_ANY_THREAD_ACTIVITY_PCU_REG                       (0x00005964)
  #define PCU_CR_PP0_ANY_THREAD_ACTIVITY_PCU_DATA_OFF                  ( 0)
  #define PCU_CR_PP0_ANY_THREAD_ACTIVITY_PCU_DATA_WID                  (32)
  #define PCU_CR_PP0_ANY_THREAD_ACTIVITY_PCU_DATA_MSK                  (0xFFFFFFFF)
  #define PCU_CR_PP0_ANY_THREAD_ACTIVITY_PCU_DATA_MAX                  (0xFFFFFFFF)
  #define PCU_CR_PP0_ANY_THREAD_ACTIVITY_PCU_DATA_DEF                  (0x00000000)

#define PCU_CR_PP0_EFFICIENT_CYCLES_PCU_REG                          (0x00005968)
  #define PCU_CR_PP0_EFFICIENT_CYCLES_PCU_DATA_OFF                     ( 0)
  #define PCU_CR_PP0_EFFICIENT_CYCLES_PCU_DATA_WID                     (32)
  #define PCU_CR_PP0_EFFICIENT_CYCLES_PCU_DATA_MSK                     (0xFFFFFFFF)
  #define PCU_CR_PP0_EFFICIENT_CYCLES_PCU_DATA_MAX                     (0xFFFFFFFF)
  #define PCU_CR_PP0_EFFICIENT_CYCLES_PCU_DATA_DEF                     (0x00000000)

#define PCU_CR_PP0_THREAD_ACTIVITY_PCU_REG                           (0x0000596C)
  #define PCU_CR_PP0_THREAD_ACTIVITY_PCU_DATA_OFF                      ( 0)
  #define PCU_CR_PP0_THREAD_ACTIVITY_PCU_DATA_WID                      (32)
  #define PCU_CR_PP0_THREAD_ACTIVITY_PCU_DATA_MSK                      (0xFFFFFFFF)
  #define PCU_CR_PP0_THREAD_ACTIVITY_PCU_DATA_MAX                      (0xFFFFFFFF)
  #define PCU_CR_PP0_THREAD_ACTIVITY_PCU_DATA_DEF                      (0x00000000)

#define PCU_CR_PP1_ANY_THREAD_ACTIVITY_PCU_REG                       (0x00005970)
  #define PCU_CR_PP1_ANY_THREAD_ACTIVITY_PCU_DATA_OFF                  ( 0)
  #define PCU_CR_PP1_ANY_THREAD_ACTIVITY_PCU_DATA_WID                  (32)
  #define PCU_CR_PP1_ANY_THREAD_ACTIVITY_PCU_DATA_MSK                  (0xFFFFFFFF)
  #define PCU_CR_PP1_ANY_THREAD_ACTIVITY_PCU_DATA_MAX                  (0xFFFFFFFF)
  #define PCU_CR_PP1_ANY_THREAD_ACTIVITY_PCU_DATA_DEF                  (0x00000000)

#define PCU_CR_PP1_EFFICIENT_CYCLES_PCU_REG                          (0x00005974)
  #define PCU_CR_PP1_EFFICIENT_CYCLES_PCU_DATA_OFF                     ( 0)
  #define PCU_CR_PP1_EFFICIENT_CYCLES_PCU_DATA_WID                     (32)
  #define PCU_CR_PP1_EFFICIENT_CYCLES_PCU_DATA_MSK                     (0xFFFFFFFF)
  #define PCU_CR_PP1_EFFICIENT_CYCLES_PCU_DATA_MAX                     (0xFFFFFFFF)
  #define PCU_CR_PP1_EFFICIENT_CYCLES_PCU_DATA_DEF                     (0x00000000)

#define PCU_CR_PACKAGE_TEMPERATURE_PCU_REG                           (0x00005978)
  #define PCU_CR_PACKAGE_TEMPERATURE_PCU_DATA_OFF                      ( 0)
  #define PCU_CR_PACKAGE_TEMPERATURE_PCU_DATA_WID                      ( 8)
  #define PCU_CR_PACKAGE_TEMPERATURE_PCU_DATA_MSK                      (0x000000FF)
  #define PCU_CR_PACKAGE_TEMPERATURE_PCU_DATA_MAX                      (0x000000FF)
  #define PCU_CR_PACKAGE_TEMPERATURE_PCU_DATA_DEF                      (0x00000000)

#define PCU_CR_PP0_TEMPERATURE_PCU_REG                               (0x0000597C)
  #define PCU_CR_PP0_TEMPERATURE_PCU_DATA_OFF                          ( 0)
  #define PCU_CR_PP0_TEMPERATURE_PCU_DATA_WID                          ( 8)
  #define PCU_CR_PP0_TEMPERATURE_PCU_DATA_MSK                          (0x000000FF)
  #define PCU_CR_PP0_TEMPERATURE_PCU_DATA_MAX                          (0x000000FF)
  #define PCU_CR_PP0_TEMPERATURE_PCU_DATA_DEF                          (0x00000000)

#define PCU_CR_PP1_TEMPERATURE_PCU_REG                               (0x00005980)
  #define PCU_CR_PP1_TEMPERATURE_PCU_DATA_OFF                          ( 0)
  #define PCU_CR_PP1_TEMPERATURE_PCU_DATA_WID                          ( 8)
  #define PCU_CR_PP1_TEMPERATURE_PCU_DATA_MSK                          (0x000000FF)
  #define PCU_CR_PP1_TEMPERATURE_PCU_DATA_MAX                          (0x000000FF)
  #define PCU_CR_PP1_TEMPERATURE_PCU_DATA_DEF                          (0x00000000)

#define PCU_CR_PCU_REFERENCE_CLOCK_PCU_REG                           (0x00005984)
  #define PCU_CR_PCU_REFERENCE_CLOCK_PCU_TIME_VAL_OFF                  ( 0)
  #define PCU_CR_PCU_REFERENCE_CLOCK_PCU_TIME_VAL_WID                  (32)
  #define PCU_CR_PCU_REFERENCE_CLOCK_PCU_TIME_VAL_MSK                  (0xFFFFFFFF)
  #define PCU_CR_PCU_REFERENCE_CLOCK_PCU_TIME_VAL_MAX                  (0xFFFFFFFF)
  #define PCU_CR_PCU_REFERENCE_CLOCK_PCU_TIME_VAL_DEF                  (0x00000000)

#define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_REG                   (0x00005988)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_OFF          ( 0)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_WID          ( 1)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_MSK          (0x00000001)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_MAX          (0x00000001)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_DEF          (0x00000000)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_VALID_OFF             ( 1)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_VALID_WID             ( 1)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_VALID_MSK             (0x00000002)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_VALID_MAX             (0x00000001)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_VALID_DEF             (0x00000000)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_BITS_OFF     ( 2)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_BITS_WID     ( 4)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_BITS_MSK     (0x0000003C)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_BITS_MAX     (0x0000000F)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_RESERVED_BITS_DEF     (0x00000000)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_OD_OFF                ( 6)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_OD_WID                ( 1)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_OD_MSK                (0x00000040)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_OD_MAX                (0x00000001)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_OD_DEF                (0x00000000)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_IM_OFF                ( 7)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_IM_WID                ( 1)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_IM_MSK                (0x00000080)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_IM_MAX                (0x00000001)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_IM_DEF                (0x00000000)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_OFF ( 8)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_WID (21)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_MSK (0x1FFFFF00)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_MAX (0x001FFFFF)
  #define PCU_CR_DEVICE_SHARED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_DEF (0x00000000)

#define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_REG                (0x0000598C)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_USED_OFF           ( 0)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_USED_WID           ( 1)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_USED_MSK           (0x00000001)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_USED_MAX           (0x00000001)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_USED_DEF           (0x00000000)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_VALID_OFF          ( 1)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_VALID_WID          ( 1)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_VALID_MSK          (0x00000002)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_VALID_MAX          (0x00000001)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_VALID_DEF          (0x00000000)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_RESERVED_BITS_OFF  ( 2)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_RESERVED_BITS_WID  ( 4)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_RESERVED_BITS_MSK  (0x0000003C)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_RESERVED_BITS_MAX  (0x0000000F)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_RESERVED_BITS_DEF  (0x00000000)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_OD_OFF             ( 6)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_OD_WID             ( 1)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_OD_MSK             (0x00000040)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_OD_MAX             (0x00000001)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_OD_DEF             (0x00000000)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_IM_OFF             ( 7)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_IM_WID             ( 1)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_IM_MSK             (0x00000080)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_IM_MAX             (0x00000001)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_IM_DEF             (0x00000000)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_OFF ( 8)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_WID (21)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_MSK (0x1FFFFF00)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_MAX (0x001FFFFF)
  #define PCU_CR_DEVICE_DEDICATED_IDLE_DURATION_PCU_NEXT_DEVICE_ACTIVITY_DEF (0x00000000)

#define PCU_CR_P_STATE_LIMITS_PCU_REG                                (0x00005990)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_LIM_OFF                       ( 0)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_LIM_WID                       ( 8)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_LIM_MSK                       (0x000000FF)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_LIM_MAX                       (0x000000FF)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_LIM_DEF                       (0x000000FF)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_MIN_OFF                       ( 8)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_MIN_WID                       ( 8)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_MIN_MSK                       (0x0000FF00)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_MIN_MAX                       (0x000000FF)
  #define PCU_CR_P_STATE_LIMITS_PCU_PSTT_MIN_DEF                       (0x00000000)
  #define PCU_CR_P_STATE_LIMITS_PCU_LOCK_OFF                           (31)
  #define PCU_CR_P_STATE_LIMITS_PCU_LOCK_WID                           ( 1)
  #define PCU_CR_P_STATE_LIMITS_PCU_LOCK_MSK                           (0x80000000)
  #define PCU_CR_P_STATE_LIMITS_PCU_LOCK_MAX                           (0x00000001)
  #define PCU_CR_P_STATE_LIMITS_PCU_LOCK_DEF                           (0x00000000)

#define PCU_CR_RP_STATE_LIMITS_PCU_REG                               (0x00005994)
  #define PCU_CR_RP_STATE_LIMITS_PCU_RPSTT_LIM_OFF                     ( 0)
  #define PCU_CR_RP_STATE_LIMITS_PCU_RPSTT_LIM_WID                     ( 8)
  #define PCU_CR_RP_STATE_LIMITS_PCU_RPSTT_LIM_MSK                     (0x000000FF)
  #define PCU_CR_RP_STATE_LIMITS_PCU_RPSTT_LIM_MAX                     (0x000000FF)
  #define PCU_CR_RP_STATE_LIMITS_PCU_RPSTT_LIM_DEF                     (0x000000FF)

#define PCU_CR_RP_STATE_CAP_PCU_REG                                  (0x00005998)
  #define PCU_CR_RP_STATE_CAP_PCU_RP0_CAP_OFF                          ( 0)
  #define PCU_CR_RP_STATE_CAP_PCU_RP0_CAP_WID                          ( 8)
  #define PCU_CR_RP_STATE_CAP_PCU_RP0_CAP_MSK                          (0x000000FF)
  #define PCU_CR_RP_STATE_CAP_PCU_RP0_CAP_MAX                          (0x000000FF)
  #define PCU_CR_RP_STATE_CAP_PCU_RP0_CAP_DEF                          (0x00000000)
  #define PCU_CR_RP_STATE_CAP_PCU_RP1_CAP_OFF                          ( 8)
  #define PCU_CR_RP_STATE_CAP_PCU_RP1_CAP_WID                          ( 8)
  #define PCU_CR_RP_STATE_CAP_PCU_RP1_CAP_MSK                          (0x0000FF00)
  #define PCU_CR_RP_STATE_CAP_PCU_RP1_CAP_MAX                          (0x000000FF)
  #define PCU_CR_RP_STATE_CAP_PCU_RP1_CAP_DEF                          (0x00000000)
  #define PCU_CR_RP_STATE_CAP_PCU_RPN_CAP_OFF                          (16)
  #define PCU_CR_RP_STATE_CAP_PCU_RPN_CAP_WID                          ( 8)
  #define PCU_CR_RP_STATE_CAP_PCU_RPN_CAP_MSK                          (0x00FF0000)
  #define PCU_CR_RP_STATE_CAP_PCU_RPN_CAP_MAX                          (0x000000FF)
  #define PCU_CR_RP_STATE_CAP_PCU_RPN_CAP_DEF                          (0x00000000)

#define PCU_CR_TEMPERATURE_TARGET_PCU_REG                            (0x0000599C)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_FAN_TEMP_TARGET_OFST_OFF       ( 8)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_FAN_TEMP_TARGET_OFST_WID       ( 8)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_FAN_TEMP_TARGET_OFST_MSK       (0x0000FF00)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_FAN_TEMP_TARGET_OFST_MAX       (0x000000FF)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_FAN_TEMP_TARGET_OFST_DEF       (0x00000000)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_REF_TEMP_OFF                   (16)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_REF_TEMP_WID                   ( 8)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_REF_TEMP_MSK                   (0x00FF0000)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_REF_TEMP_MAX                   (0x000000FF)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_REF_TEMP_DEF                   (0x00000000)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_TJ_MAX_TCC_OFFSET_OFF          (24)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_TJ_MAX_TCC_OFFSET_WID          ( 4)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_TJ_MAX_TCC_OFFSET_MSK          (0x0F000000)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_TJ_MAX_TCC_OFFSET_MAX          (0x0000000F)
  #define PCU_CR_TEMPERATURE_TARGET_PCU_TJ_MAX_TCC_OFFSET_DEF          (0x00000000)

#define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_REG                            (0x000059A0)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_OFF              ( 0)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_WID              (15)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_MSK              (0x00007FFF)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_MAX              (0x00007FFF)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_DEF              (0x00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_EN_OFF           (15)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_EN_WID           ( 1)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_EN_MSK           (0x00008000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_EN_MAX           (0x00000001)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_EN_DEF           (0x00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_1_OFF             (16)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_1_WID             ( 1)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_1_MSK             (0x00010000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_1_MAX             (0x00000001)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_1_DEF             (0x00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_TIME_OFF         (17)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_TIME_WID         ( 7)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_TIME_MSK         (0x00FE0000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_TIME_MAX         (0x0000007F)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_1_TIME_DEF         (0x00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_OFF              (32)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_WID              (15)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_MSK              (0x7FFF00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_MAX              (0x00007FFF)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_DEF              (0x00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_EN_OFF           (47)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_EN_WID           ( 1)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_EN_MSK           (0x800000000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_EN_MAX           (0x00000001)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_EN_DEF           (0x00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_2_OFF             (48)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_2_WID             ( 1)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_2_MSK             (0x1000000000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_2_MAX             (0x00000001)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_CLMP_LIM_2_DEF             (0x00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_TIME_OFF         (49)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_TIME_WID         ( 7)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_TIME_MSK         (0xFE000000000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_TIME_MAX         (0x0000007F)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_2_TIME_DEF         (0x00000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_LOCK_OFF           (63)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_LOCK_WID           ( 1)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_LOCK_MSK           (0x8000000000000000)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_LOCK_MAX           (0x00000001)
  #define PCU_CR_PACKAGE_RAPL_LIMIT_PCU_PKG_PWR_LIM_LOCK_DEF           (0x00000000)

#define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_REG                            (0x000059A8)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_IA_PP_PWR_LIM_OFF              ( 0)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_IA_PP_PWR_LIM_WID              (15)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_IA_PP_PWR_LIM_MSK              (0x00007FFF)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_IA_PP_PWR_LIM_MAX              (0x00007FFF)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_IA_PP_PWR_LIM_DEF              (0x00000000)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_OFF            (15)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_WID            ( 1)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_MSK            (0x00008000)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_MAX            (0x00000001)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_DEF            (0x00000000)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_OFF               (16)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_WID               ( 1)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_MSK               (0x00010000)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_MAX               (0x00000001)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_DEF               (0x00000000)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_OFF              (17)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_WID              ( 7)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_MSK              (0x00FE0000)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_MAX              (0x0000007F)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_DEF              (0x00000000)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_PWR_LIM_LOCK_OFF            (31)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_PWR_LIM_LOCK_WID            ( 1)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_PWR_LIM_LOCK_MSK            (0x80000000)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_PWR_LIM_LOCK_MAX            (0x00000001)
  #define PCU_CR_PRIP_TURBO_PWR_LIM_PCU_PP_PWR_LIM_LOCK_DEF            (0x00000000)

#define PCU_CR_SECP_TURBO_PWR_LIM_PCU_REG                            (0x000059AC)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_NON_IA_PP_PWR_LIM_OFF          ( 0)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_NON_IA_PP_PWR_LIM_WID          (15)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_NON_IA_PP_PWR_LIM_MSK          (0x00007FFF)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_NON_IA_PP_PWR_LIM_MAX          (0x00007FFF)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_NON_IA_PP_PWR_LIM_DEF          (0x00000000)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_OFF            (15)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_WID            ( 1)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_MSK            (0x00008000)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_MAX            (0x00000001)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PWR_LIM_CTRL_EN_DEF            (0x00000000)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_OFF               (16)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_WID               ( 1)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_MSK               (0x00010000)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_MAX               (0x00000001)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_PP_CLAMP_LIM_DEF               (0x00000000)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_OFF              (17)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_WID              ( 7)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_MSK              (0x00FE0000)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_MAX              (0x0000007F)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_CTRL_TIME_WIN_DEF              (0x00000000)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_SP_PWR_LIM_LOCK_OFF            (31)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_SP_PWR_LIM_LOCK_WID            ( 1)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_SP_PWR_LIM_LOCK_MSK            (0x80000000)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_SP_PWR_LIM_LOCK_MAX            (0x00000001)
  #define PCU_CR_SECP_TURBO_PWR_LIM_PCU_SP_PWR_LIM_LOCK_DEF            (0x00000000)

#define PCU_CR_VR_CURRENT_CONFIG_PCU_REG                             (0x000059B0)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_CURRENT_LIMIT_OFF               ( 0)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_CURRENT_LIMIT_WID               (13)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_CURRENT_LIMIT_MSK               (0x00001FFF)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_CURRENT_LIMIT_MAX               (0x00001FFF)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_CURRENT_LIMIT_DEF               (0x00000190)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_LOCK_OFF                        (31)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_LOCK_WID                        ( 1)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_LOCK_MSK                        (0x80000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_LOCK_MAX                        (0x00000001)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_LOCK_DEF                        (0x00000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI1_THRESHOLD_OFF              (32)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI1_THRESHOLD_WID              (10)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI1_THRESHOLD_MSK              (0x3FF00000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI1_THRESHOLD_MAX              (0x000003FF)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI1_THRESHOLD_DEF              (0x00000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI2_THRESHOLD_OFF              (42)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI2_THRESHOLD_WID              (10)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI2_THRESHOLD_MSK              (0xFFC0000000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI2_THRESHOLD_MAX              (0x000003FF)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI2_THRESHOLD_DEF              (0x00000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI3_THRESHOLD_OFF              (52)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI3_THRESHOLD_WID              (10)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI3_THRESHOLD_MSK              (0x3FF0000000000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI3_THRESHOLD_MAX              (0x000003FF)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_PSI3_THRESHOLD_DEF              (0x00000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_RESERVED_OFF                    (62)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_RESERVED_WID                    ( 2)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_RESERVED_MSK                    (0xC000000000000000)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_RESERVED_MAX                    (0x00000003)
  #define PCU_CR_VR_CURRENT_CONFIG_PCU_RESERVED_DEF                    (0x00000000)

#define PCU_CR_MRC_ODT_POWER_SAVING_PCU_REG                          (0x000059B8)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Rd_OFF            ( 0)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Rd_WID            ( 8)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Rd_MSK            (0x000000FF)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Rd_MAX            (0x000000FF)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Rd_DEF            (0x00000000)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Wt_OFF            ( 8)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Wt_WID            ( 8)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Wt_MSK            (0x0000FF00)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Wt_MAX            (0x000000FF)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Wt_DEF            (0x00000000)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Cmd_OFF           (16)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Cmd_WID           ( 8)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Cmd_MSK           (0x00FF0000)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Cmd_MAX           (0x000000FF)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_MRC_Saving_Cmd_DEF           (0x00000000)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_RESERVED_OFF                 (24)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_RESERVED_WID                 ( 8)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_RESERVED_MSK                 (0xFF000000)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_RESERVED_MAX                 (0x000000FF)
  #define PCU_CR_MRC_ODT_POWER_SAVING_PCU_RESERVED_DEF                 (0x00000000)

#define PCU_CR_THERM_STATUS_GT_PCU_REG                               (0x000059C0)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_STATUS_OFF        ( 0)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_STATUS_WID        ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_STATUS_MSK        (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_STATUS_MAX        (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_STATUS_DEF        (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_LOG_OFF           ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_LOG_WID           ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_LOG_MSK           (0x00000002)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_LOG_MAX           (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_THERMAL_MONITOR_LOG_DEF           (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_STATUS_OFF                ( 2)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_STATUS_WID                ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_STATUS_MSK                (0x00000004)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_STATUS_MAX                (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_STATUS_DEF                (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_LOG_OFF                   ( 3)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_LOG_WID                   ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_LOG_MSK                   (0x00000008)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_LOG_MAX                   (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_PROCHOT_LOG_DEF                   (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_STATUS_OFF            ( 4)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_STATUS_WID            ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_STATUS_MSK            (0x00000010)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_STATUS_MAX            (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_STATUS_DEF            (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_LOG_OFF               ( 5)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_LOG_WID               ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_LOG_MSK               (0x00000020)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_LOG_MAX               (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_OUT_OF_SPEC_LOG_DEF               (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_STATUS_OFF             ( 6)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_STATUS_WID             ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_STATUS_MSK             (0x00000040)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_STATUS_MAX             (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_STATUS_DEF             (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_LOG_OFF                ( 7)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_LOG_WID                ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_LOG_MSK                (0x00000080)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_LOG_MAX                (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD1_LOG_DEF                (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_STATUS_OFF             ( 8)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_STATUS_WID             ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_STATUS_MSK             (0x00000100)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_STATUS_MAX             (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_STATUS_DEF             (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_LOG_OFF                ( 9)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_LOG_WID                ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_LOG_MSK                (0x00000200)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_LOG_MAX                (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_THRESHOLD2_LOG_DEF                (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_STATUS_OFF       (10)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_STATUS_WID       ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_STATUS_MSK       (0x00000400)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_STATUS_MAX       (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_STATUS_DEF       (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_LOG_OFF          (11)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_LOG_WID          ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_LOG_MSK          (0x00000800)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_LOG_MAX          (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_POWER_LIMITATION_LOG_DEF          (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_TEMPERATURE_OFF                   (16)
  #define PCU_CR_THERM_STATUS_GT_PCU_TEMPERATURE_WID                   ( 7)
  #define PCU_CR_THERM_STATUS_GT_PCU_TEMPERATURE_MSK                   (0x007F0000)
  #define PCU_CR_THERM_STATUS_GT_PCU_TEMPERATURE_MAX                   (0x0000007F)
  #define PCU_CR_THERM_STATUS_GT_PCU_TEMPERATURE_DEF                   (0x00000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_RESOLUTION_OFF                    (27)
  #define PCU_CR_THERM_STATUS_GT_PCU_RESOLUTION_WID                    ( 4)
  #define PCU_CR_THERM_STATUS_GT_PCU_RESOLUTION_MSK                    (0x78000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_RESOLUTION_MAX                    (0x0000000F)
  #define PCU_CR_THERM_STATUS_GT_PCU_RESOLUTION_DEF                    (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_VALID_OFF                         (31)
  #define PCU_CR_THERM_STATUS_GT_PCU_VALID_WID                         ( 1)
  #define PCU_CR_THERM_STATUS_GT_PCU_VALID_MSK                         (0x80000000)
  #define PCU_CR_THERM_STATUS_GT_PCU_VALID_MAX                         (0x00000001)
  #define PCU_CR_THERM_STATUS_GT_PCU_VALID_DEF                         (0x00000000)

#define PCU_CR_THERM_INTERRUPT_GT_PCU_REG                            (0x000059C4)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_HIGH_TEMP_INT_ENABLE_OFF       ( 0)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_HIGH_TEMP_INT_ENABLE_WID       ( 1)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_HIGH_TEMP_INT_ENABLE_MSK       (0x00000001)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_HIGH_TEMP_INT_ENABLE_MAX       (0x00000001)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_HIGH_TEMP_INT_ENABLE_DEF       (0x00000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_LOW_TEMP_INT_ENABLE_OFF        ( 1)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_LOW_TEMP_INT_ENABLE_WID        ( 1)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_LOW_TEMP_INT_ENABLE_MSK        (0x00000002)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_LOW_TEMP_INT_ENABLE_MAX        (0x00000001)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_LOW_TEMP_INT_ENABLE_DEF        (0x00000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_PROCHOT_INT_ENABLE_OFF         ( 2)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_PROCHOT_INT_ENABLE_WID         ( 1)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_PROCHOT_INT_ENABLE_MSK         (0x00000004)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_PROCHOT_INT_ENABLE_MAX         (0x00000001)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_PROCHOT_INT_ENABLE_DEF         (0x00000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_OUT_OF_SPEC_INT_ENABLE_OFF     ( 4)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_OUT_OF_SPEC_INT_ENABLE_WID     ( 1)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_OUT_OF_SPEC_INT_ENABLE_MSK     (0x00000010)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_OUT_OF_SPEC_INT_ENABLE_MAX     (0x00000001)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_OUT_OF_SPEC_INT_ENABLE_DEF     (0x00000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_REL_TEMP_OFF       ( 8)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_REL_TEMP_WID       ( 7)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_REL_TEMP_MSK       (0x00007F00)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_REL_TEMP_MAX       (0x0000007F)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_REL_TEMP_DEF       (0x00000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_INT_ENABLE_OFF     (15)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_INT_ENABLE_WID     ( 1)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_INT_ENABLE_MSK     (0x00008000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_INT_ENABLE_MAX     (0x00000001)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_1_INT_ENABLE_DEF     (0x00000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_REL_TEMP_OFF       (16)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_REL_TEMP_WID       ( 7)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_REL_TEMP_MSK       (0x007F0000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_REL_TEMP_MAX       (0x0000007F)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_REL_TEMP_DEF       (0x00000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_INT_ENABLE_OFF     (23)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_INT_ENABLE_WID     ( 1)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_INT_ENABLE_MSK     (0x00800000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_INT_ENABLE_MAX     (0x00000001)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_THRESHOLD_2_INT_ENABLE_DEF     (0x00000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_POWER_INT_ENABLE_OFF           (24)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_POWER_INT_ENABLE_WID           ( 1)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_POWER_INT_ENABLE_MSK           (0x01000000)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_POWER_INT_ENABLE_MAX           (0x00000001)
  #define PCU_CR_THERM_INTERRUPT_GT_PCU_POWER_INT_ENABLE_DEF           (0x00000000)

#define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_REG                 (0x000059C8)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_OFF        ( 0)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_WID        ( 1)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_MSK        (0x00000001)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_MAX        (0x00000001)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_DEF        (0x00000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_VALID_OFF           ( 1)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_VALID_WID           ( 1)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_VALID_MSK           (0x00000002)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_VALID_MAX           (0x00000001)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_VALID_DEF           (0x00000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_BITS_OFF   ( 2)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_BITS_WID   ( 4)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_BITS_MSK   (0x0000003C)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_BITS_MAX   (0x0000000F)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_RESERVED_BITS_DEF   (0x00000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_OD_OFF              ( 6)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_OD_WID              ( 1)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_OD_MSK              (0x00000040)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_OD_MAX              (0x00000001)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_OD_DEF              (0x00000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_IM_OFF              ( 7)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_IM_WID              ( 1)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_IM_MSK              (0x00000080)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_IM_MAX              (0x00000001)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_IM_DEF              (0x00000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_NEXT_DEVICE_ACTIVITY_OFF ( 8)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_NEXT_DEVICE_ACTIVITY_WID (21)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_NEXT_DEVICE_ACTIVITY_MSK (0x1FFFFF00)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_NEXT_DEVICE_ACTIVITY_MAX (0x001FFFFF)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_NEXT_DEVICE_ACTIVITY_DEF (0x00000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_DISABLE_MDID_EVALUATION_OFF (29)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_DISABLE_MDID_EVALUATION_WID ( 1)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_DISABLE_MDID_EVALUATION_MSK (0x20000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_DISABLE_MDID_EVALUATION_MAX (0x00000001)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_DISABLE_MDID_EVALUATION_DEF (0x00000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_FORCE_MDID_OVERRIDE_OFF (30)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_FORCE_MDID_OVERRIDE_WID ( 1)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_FORCE_MDID_OVERRIDE_MSK (0x40000000)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_FORCE_MDID_OVERRIDE_MAX (0x00000001)
  #define PCU_CR_DEVICE_IDLE_DURATION_OVERRIDE_PCU_FORCE_MDID_OVERRIDE_DEF (0x00000000)

#define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_REG              (0x000059D0)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALUE_OFF        ( 0)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALUE_WID        (10)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALUE_MSK        (0x000003FF)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALUE_MAX        (0x000003FF)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALUE_DEF        (0x00000000)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_MULTIPLIER_OFF   (10)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_MULTIPLIER_WID   ( 3)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_MULTIPLIER_MSK   (0x00001C00)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_MULTIPLIER_MAX   (0x00000007)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_MULTIPLIER_DEF   (0x00000000)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALID_OFF        (15)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALID_WID        ( 1)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALID_MSK        (0x00008000)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALID_MAX        (0x00000001)
  #define PCU_CR_GRAPHICS_INTERRUPT_RESPONSE_TIME_PCU_VALID_DEF        (0x00000000)

#define PCU_CR_CHAP_CONFIG_PCU_REG                                   (0x00005A00)
  #define PCU_CR_CHAP_CONFIG_PCU_PECI_CMD_OFF                          ( 4)
  #define PCU_CR_CHAP_CONFIG_PCU_PECI_CMD_WID                          ( 8)
  #define PCU_CR_CHAP_CONFIG_PCU_PECI_CMD_MSK                          (0x00000FF0)
  #define PCU_CR_CHAP_CONFIG_PCU_PECI_CMD_MAX                          (0x000000FF)
  #define PCU_CR_CHAP_CONFIG_PCU_PECI_CMD_DEF                          (0x00000000)

#define PCU_CR_CHAP_THRESHOLD2_PCU_REG                               (0x00005A08)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH1_OFF                      ( 0)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH1_WID                      ( 6)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH1_MSK                      (0x0000003F)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH1_MAX                      (0x0000003F)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH1_DEF                      (0x00000000)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH2_OFF                      ( 8)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH2_WID                      ( 6)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH2_MSK                      (0x00003F00)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH2_MAX                      (0x0000003F)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH2_DEF                      (0x00000000)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH3_OFF                      (16)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH3_WID                      ( 6)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH3_MSK                      (0x003F0000)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH3_MAX                      (0x0000003F)
  #define PCU_CR_CHAP_THRESHOLD2_PCU_FREQ_TH3_DEF                      (0x00000000)

#define PCU_CR_ENERGY_DEBUG_PCU_REG                                  (0x00005B04)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP0_VALUE_OFF           ( 0)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP0_VALUE_WID           (10)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP0_VALUE_MSK           (0x000003FF)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP0_VALUE_MAX           (0x000003FF)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP0_VALUE_DEF           (0x00000000)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP1_VALUE_OFF           (10)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP1_VALUE_WID           (10)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP1_VALUE_MSK           (0x000FFC00)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP1_VALUE_MAX           (0x000003FF)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_PP1_VALUE_DEF           (0x00000000)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_SA_VALUE_OFF            (20)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_SA_VALUE_WID            (10)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_SA_VALUE_MSK            (0x3FF00000)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_SA_VALUE_MAX            (0x000003FF)
  #define PCU_CR_ENERGY_DEBUG_PCU_DEBUG_ENERGY_SA_VALUE_DEF            (0x00000000)

#define PCU_CR_SSKPD_PCU_REG                                         (0x00005D10)
  #define PCU_CR_SSKPD_PCU_SKPD_OFF                                    ( 0)
  #define PCU_CR_SSKPD_PCU_SKPD_WID                                    (64)
  #define PCU_CR_SSKPD_PCU_SKPD_MSK                                    (0xFFFFFFFFFFFFFFFF)
  #define PCU_CR_SSKPD_PCU_SKPD_MAX                                    (0xFFFFFFFFFFFFFFFF)
  #define PCU_CR_SSKPD_PCU_SKPD_DEF                                    (0x00000000)

#define PCU_CR_C2C3TT_PCU_REG                                        (0x00005D20)
  #define PCU_CR_C2C3TT_PCU_PPDN_INIT_OFF                              ( 0)
  #define PCU_CR_C2C3TT_PCU_PPDN_INIT_WID                              (12)
  #define PCU_CR_C2C3TT_PCU_PPDN_INIT_MSK                              (0x00000FFF)
  #define PCU_CR_C2C3TT_PCU_PPDN_INIT_MAX                              (0x00000FFF)
  #define PCU_CR_C2C3TT_PCU_PPDN_INIT_DEF                              (0x00000005)

#define PCU_CR_C2_DDR_TT_PCU_REG                                     (0x00005D24)
  #define PCU_CR_C2_DDR_TT_PCU_DDR_TIMER_VALUE_OFF                     ( 0)
  #define PCU_CR_C2_DDR_TT_PCU_DDR_TIMER_VALUE_WID                     (13)
  #define PCU_CR_C2_DDR_TT_PCU_DDR_TIMER_VALUE_MSK                     (0x00001FFF)
  #define PCU_CR_C2_DDR_TT_PCU_DDR_TIMER_VALUE_MAX                     (0x00001FFF)
  #define PCU_CR_C2_DDR_TT_PCU_DDR_TIMER_VALUE_DEF                     (0x000001F4)

#define PCU_CR_PCIE_ILTR_OVRD_PCU_REG                                (0x00005D30)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NSTL_OFF                           ( 0)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NSTL_WID                           (10)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NSTL_MSK                           (0x000003FF)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NSTL_MAX                           (0x000003FF)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NSTL_DEF                           (0x00000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_MULTIPLIER_OFF                     (10)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_MULTIPLIER_WID                     ( 3)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_MULTIPLIER_MSK                     (0x00001C00)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_MULTIPLIER_MAX                     (0x00000007)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_MULTIPLIER_DEF                     (0x00000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_NL_OFF                       (14)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_NL_WID                       ( 1)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_NL_MSK                       (0x00004000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_NL_MAX                       (0x00000001)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_NL_DEF                       (0x00000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NL_V_OFF                           (15)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NL_V_WID                           ( 1)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NL_V_MSK                           (0x00008000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NL_V_MAX                           (0x00000001)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_NL_V_DEF                           (0x00000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_OFF                            (16)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_WID                            (10)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_MSK                            (0x03FF0000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_MAX                            (0x000003FF)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_DEF                            (0x00000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXLM_OFF                           (26)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXLM_WID                           ( 3)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXLM_MSK                           (0x1C000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXLM_MAX                           (0x00000007)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXLM_DEF                           (0x00000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_SXL_OFF                      (30)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_SXL_WID                      ( 1)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_SXL_MSK                      (0x40000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_SXL_MAX                      (0x00000001)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_FORCE_SXL_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_V_OFF                          (31)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_V_WID                          ( 1)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_V_MSK                          (0x80000000)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_V_MAX                          (0x00000001)
  #define PCU_CR_PCIE_ILTR_OVRD_PCU_SXL_V_DEF                          (0x00000000)

#define PCU_CR_PCIE_ILTR_VAL_PCU_0_REG                               (0x00005D34)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALUE_OFF                      ( 0)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALUE_WID                      (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALUE_MSK                      (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALUE_MAX                      (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALUE_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_SCALE_OFF                      (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_SCALE_WID                      ( 3)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_SCALE_MSK                      (0x00001C00)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_SCALE_MAX                      (0x00000007)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_SCALE_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_RESERVED_OFF                   (13)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_RESERVED_WID                   ( 2)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_RESERVED_MSK                   (0x00006000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_RESERVED_MAX                   (0x00000003)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_RESERVED_DEF                   (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALID_OFF                      (15)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALID_WID                      ( 1)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALID_MSK                      (0x00008000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALID_MAX                      (0x00000001)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_NL_VALID_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALUE_OFF                     (16)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALUE_WID                     (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALUE_MSK                     (0x03FF0000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALUE_MAX                     (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALUE_DEF                     (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_SCALE_OFF                     (26)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_SCALE_WID                     ( 3)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_SCALE_MSK                     (0x1C000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_SCALE_MAX                     (0x00000007)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_SCALE_DEF                     (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_RESERVED_OFF                  (29)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_RESERVED_WID                  ( 2)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_RESERVED_MSK                  (0x60000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_RESERVED_MAX                  (0x00000003)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_RESERVED_DEF                  (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALID_OFF                     (31)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALID_WID                     ( 1)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALID_MSK                     (0x80000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALID_MAX                     (0x00000001)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_0_SXL_VALID_DEF                     (0x00000000)

#define PCU_CR_PCIE_ILTR_VAL_PCU_1_REG                               (0x00005D38)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALUE_OFF                      ( 0)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALUE_WID                      (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALUE_MSK                      (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALUE_MAX                      (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALUE_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_SCALE_OFF                      (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_SCALE_WID                      ( 3)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_SCALE_MSK                      (0x00001C00)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_SCALE_MAX                      (0x00000007)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_SCALE_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_RESERVED_OFF                   (13)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_RESERVED_WID                   ( 2)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_RESERVED_MSK                   (0x00006000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_RESERVED_MAX                   (0x00000003)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_RESERVED_DEF                   (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALID_OFF                      (15)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALID_WID                      ( 1)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALID_MSK                      (0x00008000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALID_MAX                      (0x00000001)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_NL_VALID_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALUE_OFF                     (16)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALUE_WID                     (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALUE_MSK                     (0x03FF0000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALUE_MAX                     (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALUE_DEF                     (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_SCALE_OFF                     (26)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_SCALE_WID                     ( 3)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_SCALE_MSK                     (0x1C000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_SCALE_MAX                     (0x00000007)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_SCALE_DEF                     (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_RESERVED_OFF                  (29)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_RESERVED_WID                  ( 2)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_RESERVED_MSK                  (0x60000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_RESERVED_MAX                  (0x00000003)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_RESERVED_DEF                  (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALID_OFF                     (31)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALID_WID                     ( 1)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALID_MSK                     (0x80000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALID_MAX                     (0x00000001)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_1_SXL_VALID_DEF                     (0x00000000)

#define PCU_CR_PCIE_ILTR_VAL_PCU_2_REG                               (0x00005D3C)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALUE_OFF                      ( 0)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALUE_WID                      (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALUE_MSK                      (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALUE_MAX                      (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALUE_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_SCALE_OFF                      (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_SCALE_WID                      ( 3)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_SCALE_MSK                      (0x00001C00)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_SCALE_MAX                      (0x00000007)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_SCALE_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_RESERVED_OFF                   (13)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_RESERVED_WID                   ( 2)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_RESERVED_MSK                   (0x00006000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_RESERVED_MAX                   (0x00000003)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_RESERVED_DEF                   (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALID_OFF                      (15)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALID_WID                      ( 1)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALID_MSK                      (0x00008000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALID_MAX                      (0x00000001)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_NL_VALID_DEF                      (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALUE_OFF                     (16)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALUE_WID                     (10)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALUE_MSK                     (0x03FF0000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALUE_MAX                     (0x000003FF)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALUE_DEF                     (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_SCALE_OFF                     (26)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_SCALE_WID                     ( 3)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_SCALE_MSK                     (0x1C000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_SCALE_MAX                     (0x00000007)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_SCALE_DEF                     (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_RESERVED_OFF                  (29)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_RESERVED_WID                  ( 2)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_RESERVED_MSK                  (0x60000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_RESERVED_MAX                  (0x00000003)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_RESERVED_DEF                  (0x00000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALID_OFF                     (31)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALID_WID                     ( 1)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALID_MSK                     (0x80000000)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALID_MAX                     (0x00000001)
  #define PCU_CR_PCIE_ILTR_VAL_PCU_2_SXL_VALID_DEF                     (0x00000000)

#define PCU_CR_VISA_CTL_PTPCFSMS_PCU_REG                             (0x00005D40)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_DATA_OFF                        ( 0)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_DATA_WID                        (18)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_DATA_MSK                        (0x0003FFFF)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_DATA_MAX                        (0x0003FFFF)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_DATA_DEF                        (0x00000000)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_VORANGE_OFF                     (31)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_VORANGE_WID                     ( 1)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_VORANGE_MSK                     (0x80000000)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_VORANGE_MAX                     (0x00000001)
  #define PCU_CR_VISA_CTL_PTPCFSMS_PCU_VORANGE_DEF                     (0x00000000)

#define PCU_CR_VISA_XBAR_PTPCFSMS_PCU_REG                            (0x00005D44)
  #define PCU_CR_VISA_XBAR_PTPCFSMS_PCU_DATA_OFF                       ( 0)
  #define PCU_CR_VISA_XBAR_PTPCFSMS_PCU_DATA_WID                       (32)
  #define PCU_CR_VISA_XBAR_PTPCFSMS_PCU_DATA_MSK                       (0xFFFFFFFF)
  #define PCU_CR_VISA_XBAR_PTPCFSMS_PCU_DATA_MAX                       (0xFFFFFFFF)
  #define PCU_CR_VISA_XBAR_PTPCFSMS_PCU_DATA_DEF                       (0x76543210)

#define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_REG                           (0x00005D48)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_DATA_OFF                      ( 0)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_DATA_WID                      (18)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_DATA_MSK                      (0x0003FFFF)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_DATA_MAX                      (0x0003FFFF)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_DATA_DEF                      (0x00000000)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_VORANGE_OFF                   (31)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_VORANGE_WID                   ( 1)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_VORANGE_MSK                   (0x80000000)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_VORANGE_MAX                   (0x00000001)
  #define PCU_CR_VISA_CTL_PTPCIOREGS_PCU_VORANGE_DEF                   (0x00000000)

#define PCU_CR_BIOS_MAILBOX_DATA_PCU_REG                             (0x00005DA0)
  #define PCU_CR_BIOS_MAILBOX_DATA_PCU_DATA_OFF                        ( 0)
  #define PCU_CR_BIOS_MAILBOX_DATA_PCU_DATA_WID                        (32)
  #define PCU_CR_BIOS_MAILBOX_DATA_PCU_DATA_MSK                        (0xFFFFFFFF)
  #define PCU_CR_BIOS_MAILBOX_DATA_PCU_DATA_MAX                        (0xFFFFFFFF)
  #define PCU_CR_BIOS_MAILBOX_DATA_PCU_DATA_DEF                        (0x00000000)

#define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_REG                        (0x00005DA4)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_COMMAND_OFF                ( 0)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_COMMAND_WID                ( 8)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_COMMAND_MSK                (0x000000FF)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_COMMAND_MAX                (0x000000FF)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_COMMAND_DEF                (0x00000000)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_ADDR_OFF                   ( 8)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_ADDR_WID                   (21)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_ADDR_MSK                   (0x1FFFFF00)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_ADDR_MAX                   (0x001FFFFF)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_ADDR_DEF                   (0x00000000)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_RUN_BUSY_OFF               (31)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_RUN_BUSY_WID               ( 1)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_RUN_BUSY_MSK               (0x80000000)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_RUN_BUSY_MAX               (0x00000001)
  #define PCU_CR_BIOS_MAILBOX_INTERFACE_PCU_RUN_BUSY_DEF               (0x00000000)

#define PCU_CR_BIOS_RESET_CPL_PCU_REG                                (0x00005DA8)
  #define PCU_CR_BIOS_RESET_CPL_PCU_RST_CPL_OFF                        ( 0)
  #define PCU_CR_BIOS_RESET_CPL_PCU_RST_CPL_WID                        ( 1)
  #define PCU_CR_BIOS_RESET_CPL_PCU_RST_CPL_MSK                        (0x00000001)
  #define PCU_CR_BIOS_RESET_CPL_PCU_RST_CPL_MAX                        (0x00000001)
  #define PCU_CR_BIOS_RESET_CPL_PCU_RST_CPL_DEF                        (0x00000000)
  #define PCU_CR_BIOS_RESET_CPL_PCU_PCIE_ENUMERATION_DONE_OFF          ( 1)
  #define PCU_CR_BIOS_RESET_CPL_PCU_PCIE_ENUMERATION_DONE_WID          ( 1)
  #define PCU_CR_BIOS_RESET_CPL_PCU_PCIE_ENUMERATION_DONE_MSK          (0x00000002)
  #define PCU_CR_BIOS_RESET_CPL_PCU_PCIE_ENUMERATION_DONE_MAX          (0x00000001)
  #define PCU_CR_BIOS_RESET_CPL_PCU_PCIE_ENUMERATION_DONE_DEF          (0x00000000)
  #define PCU_CR_BIOS_RESET_CPL_PCU_C7_ALLOWED_OFF                     ( 2)
  #define PCU_CR_BIOS_RESET_CPL_PCU_C7_ALLOWED_WID                     ( 1)
  #define PCU_CR_BIOS_RESET_CPL_PCU_C7_ALLOWED_MSK                     (0x00000004)
  #define PCU_CR_BIOS_RESET_CPL_PCU_C7_ALLOWED_MAX                     (0x00000001)
  #define PCU_CR_BIOS_RESET_CPL_PCU_C7_ALLOWED_DEF                     (0x00000000)

#define PCU_CR_MC_BIOS_REQ_PCU_REG                                   (0x00005E00)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_DATA_OFF                          ( 0)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_DATA_WID                          ( 4)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_DATA_MSK                          (0x0000000F)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_DATA_MAX                          (0x0000000F)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_DATA_DEF                          (0x00000000)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_TYPE_OFF                          ( 4)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_TYPE_WID                          ( 4)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_TYPE_MSK                          (0x000000F0)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_TYPE_MAX                          (0x0000000F)
  #define PCU_CR_MC_BIOS_REQ_PCU_REQ_TYPE_DEF                          (0x00000000)
  #define PCU_CR_MC_BIOS_REQ_PCU_RUN_BUSY_OFF                          (31)
  #define PCU_CR_MC_BIOS_REQ_PCU_RUN_BUSY_WID                          ( 1)
  #define PCU_CR_MC_BIOS_REQ_PCU_RUN_BUSY_MSK                          (0x80000000)
  #define PCU_CR_MC_BIOS_REQ_PCU_RUN_BUSY_MAX                          (0x00000001)
  #define PCU_CR_MC_BIOS_REQ_PCU_RUN_BUSY_DEF                          (0x00000000)

#define PCU_CR_MC_BIOS_DATA_PCU_REG                                  (0x00005E04)
  #define PCU_CR_MC_BIOS_DATA_PCU_MC_FREQ_OFF                          ( 0)
  #define PCU_CR_MC_BIOS_DATA_PCU_MC_FREQ_WID                          ( 4)
  #define PCU_CR_MC_BIOS_DATA_PCU_MC_FREQ_MSK                          (0x0000000F)
  #define PCU_CR_MC_BIOS_DATA_PCU_MC_FREQ_MAX                          (0x0000000F)
  #define PCU_CR_MC_BIOS_DATA_PCU_MC_FREQ_DEF                          (0x00000000)

#define PCU_CR_SAPMCTL_PCU_REG                                       (0x00005F00)
  #define PCU_CR_SAPMCTL_PCU_SACG_ENA_OFF                              ( 0)
  #define PCU_CR_SAPMCTL_PCU_SACG_ENA_WID                              ( 1)
  #define PCU_CR_SAPMCTL_PCU_SACG_ENA_MSK                              (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_SACG_ENA_MAX                              (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_SACG_ENA_DEF                              (0x00000000)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_ENA_OFF                          ( 1)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_ENA_WID                          ( 1)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_ENA_MSK                          (0x00000002)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_ENA_MAX                          (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_ENA_DEF                          (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_PPLL_OFF_ENA_OFF                          ( 2)
  #define PCU_CR_SAPMCTL_PCU_PPLL_OFF_ENA_WID                          ( 1)
  #define PCU_CR_SAPMCTL_PCU_PPLL_OFF_ENA_MSK                          (0x00000004)
  #define PCU_CR_SAPMCTL_PCU_PPLL_OFF_ENA_MAX                          (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_PPLL_OFF_ENA_DEF                          (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_SACG_SEN_OFF                              ( 8)
  #define PCU_CR_SAPMCTL_PCU_SACG_SEN_WID                              ( 1)
  #define PCU_CR_SAPMCTL_PCU_SACG_SEN_MSK                              (0x00000100)
  #define PCU_CR_SAPMCTL_PCU_SACG_SEN_MAX                              (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_SACG_SEN_DEF                              (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_SEN_OFF                          ( 9)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_SEN_WID                          ( 1)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_SEN_MSK                          (0x00000200)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_SEN_MAX                          (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_MPLL_OFF_SEN_DEF                          (0x00000000)
  #define PCU_CR_SAPMCTL_PCU_MDLL_OFF_SEN_OFF                          (10)
  #define PCU_CR_SAPMCTL_PCU_MDLL_OFF_SEN_WID                          ( 1)
  #define PCU_CR_SAPMCTL_PCU_MDLL_OFF_SEN_MSK                          (0x00000400)
  #define PCU_CR_SAPMCTL_PCU_MDLL_OFF_SEN_MAX                          (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_MDLL_OFF_SEN_DEF                          (0x00000000)
  #define PCU_CR_SAPMCTL_PCU_SACG_SREXIT_OFF                           (11)
  #define PCU_CR_SAPMCTL_PCU_SACG_SREXIT_WID                           ( 1)
  #define PCU_CR_SAPMCTL_PCU_SACG_SREXIT_MSK                           (0x00000800)
  #define PCU_CR_SAPMCTL_PCU_SACG_SREXIT_MAX                           (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_SACG_SREXIT_DEF                           (0x00000000)
  #define PCU_CR_SAPMCTL_PCU_NSWAKE_SREXIT_OFF                         (12)
  #define PCU_CR_SAPMCTL_PCU_NSWAKE_SREXIT_WID                         ( 1)
  #define PCU_CR_SAPMCTL_PCU_NSWAKE_SREXIT_MSK                         (0x00001000)
  #define PCU_CR_SAPMCTL_PCU_NSWAKE_SREXIT_MAX                         (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_NSWAKE_SREXIT_DEF                         (0x00000000)
  #define PCU_CR_SAPMCTL_PCU_SACG_MPLL_OFF                             (13)
  #define PCU_CR_SAPMCTL_PCU_SACG_MPLL_WID                             ( 1)
  #define PCU_CR_SAPMCTL_PCU_SACG_MPLL_MSK                             (0x00002000)
  #define PCU_CR_SAPMCTL_PCU_SACG_MPLL_MAX                             (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_SACG_MPLL_DEF                             (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_MPLL_ON_DE_OFF                            (14)
  #define PCU_CR_SAPMCTL_PCU_MPLL_ON_DE_WID                            ( 1)
  #define PCU_CR_SAPMCTL_PCU_MPLL_ON_DE_MSK                            (0x00004000)
  #define PCU_CR_SAPMCTL_PCU_MPLL_ON_DE_MAX                            (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_MPLL_ON_DE_DEF                            (0x00000000)
  #define PCU_CR_SAPMCTL_PCU_MDLL_ON_DE_OFF                            (15)
  #define PCU_CR_SAPMCTL_PCU_MDLL_ON_DE_WID                            ( 1)
  #define PCU_CR_SAPMCTL_PCU_MDLL_ON_DE_MSK                            (0x00008000)
  #define PCU_CR_SAPMCTL_PCU_MDLL_ON_DE_MAX                            (0x00000001)
  #define PCU_CR_SAPMCTL_PCU_MDLL_ON_DE_DEF                            (0x00000000)

#define PCU_CR_P_COMP_PCU_REG                                        (0x00005F04)
  #define PCU_CR_P_COMP_PCU_COMP_DISABLE_OFF                           ( 0)
  #define PCU_CR_P_COMP_PCU_COMP_DISABLE_WID                           ( 1)
  #define PCU_CR_P_COMP_PCU_COMP_DISABLE_MSK                           (0x00000001)
  #define PCU_CR_P_COMP_PCU_COMP_DISABLE_MAX                           (0x00000001)
  #define PCU_CR_P_COMP_PCU_COMP_DISABLE_DEF                           (0x00000000)
  #define PCU_CR_P_COMP_PCU_COMP_INTERVAL_OFF                          ( 1)
  #define PCU_CR_P_COMP_PCU_COMP_INTERVAL_WID                          ( 4)
  #define PCU_CR_P_COMP_PCU_COMP_INTERVAL_MSK                          (0x0000001E)
  #define PCU_CR_P_COMP_PCU_COMP_INTERVAL_MAX                          (0x0000000F)
  #define PCU_CR_P_COMP_PCU_COMP_INTERVAL_DEF                          (0x00000008)
  #define PCU_CR_P_COMP_PCU_COMP_FORCE_OFF                             ( 8)
  #define PCU_CR_P_COMP_PCU_COMP_FORCE_WID                             ( 1)
  #define PCU_CR_P_COMP_PCU_COMP_FORCE_MSK                             (0x00000100)
  #define PCU_CR_P_COMP_PCU_COMP_FORCE_MAX                             (0x00000001)
  #define PCU_CR_P_COMP_PCU_COMP_FORCE_DEF                             (0x00000000)

#define PCU_CR_M_COMP_PCU_REG                                        (0x00005F08)
  #define PCU_CR_M_COMP_PCU_COMP_DISABLE_OFF                           ( 0)
  #define PCU_CR_M_COMP_PCU_COMP_DISABLE_WID                           ( 1)
  #define PCU_CR_M_COMP_PCU_COMP_DISABLE_MSK                           (0x00000001)
  #define PCU_CR_M_COMP_PCU_COMP_DISABLE_MAX                           (0x00000001)
  #define PCU_CR_M_COMP_PCU_COMP_DISABLE_DEF                           (0x00000000)
  #define PCU_CR_M_COMP_PCU_COMP_INTERVAL_OFF                          ( 1)
  #define PCU_CR_M_COMP_PCU_COMP_INTERVAL_WID                          ( 4)
  #define PCU_CR_M_COMP_PCU_COMP_INTERVAL_MSK                          (0x0000001E)
  #define PCU_CR_M_COMP_PCU_COMP_INTERVAL_MAX                          (0x0000000F)
  #define PCU_CR_M_COMP_PCU_COMP_INTERVAL_DEF                          (0x0000000D)
  #define PCU_CR_M_COMP_PCU_COMP_FORCE_OFF                             ( 8)
  #define PCU_CR_M_COMP_PCU_COMP_FORCE_WID                             ( 1)
  #define PCU_CR_M_COMP_PCU_COMP_FORCE_MSK                             (0x00000100)
  #define PCU_CR_M_COMP_PCU_COMP_FORCE_MAX                             (0x00000001)
  #define PCU_CR_M_COMP_PCU_COMP_FORCE_DEF                             (0x00000000)

#define PCU_CR_D_COMP_PCU_REG                                        (0x00005F0C)
  #define PCU_CR_D_COMP_PCU_COMP_DISABLE_OFF                           ( 0)
  #define PCU_CR_D_COMP_PCU_COMP_DISABLE_WID                           ( 1)
  #define PCU_CR_D_COMP_PCU_COMP_DISABLE_MSK                           (0x00000001)
  #define PCU_CR_D_COMP_PCU_COMP_DISABLE_MAX                           (0x00000001)
  #define PCU_CR_D_COMP_PCU_COMP_DISABLE_DEF                           (0x00000000)
  #define PCU_CR_D_COMP_PCU_COMP_INTERVAL_OFF                          ( 1)
  #define PCU_CR_D_COMP_PCU_COMP_INTERVAL_WID                          ( 4)
  #define PCU_CR_D_COMP_PCU_COMP_INTERVAL_MSK                          (0x0000001E)
  #define PCU_CR_D_COMP_PCU_COMP_INTERVAL_MAX                          (0x0000000F)
  #define PCU_CR_D_COMP_PCU_COMP_INTERVAL_DEF                          (0x00000008)
  #define PCU_CR_D_COMP_PCU_COMP_FORCE_OFF                             ( 8)
  #define PCU_CR_D_COMP_PCU_COMP_FORCE_WID                             ( 1)
  #define PCU_CR_D_COMP_PCU_COMP_FORCE_MSK                             (0x00000100)
  #define PCU_CR_D_COMP_PCU_COMP_FORCE_MAX                             (0x00000001)
  #define PCU_CR_D_COMP_PCU_COMP_FORCE_DEF                             (0x00000000)

#define PCU_CR_CONFIG_TDP_NOMINAL_PCU_REG                            (0x00005F3C)
  #define PCU_CR_CONFIG_TDP_NOMINAL_PCU_TDP_RATIO_OFF                  ( 0)
  #define PCU_CR_CONFIG_TDP_NOMINAL_PCU_TDP_RATIO_WID                  ( 8)
  #define PCU_CR_CONFIG_TDP_NOMINAL_PCU_TDP_RATIO_MSK                  (0x000000FF)
  #define PCU_CR_CONFIG_TDP_NOMINAL_PCU_TDP_RATIO_MAX                  (0x000000FF)
  #define PCU_CR_CONFIG_TDP_NOMINAL_PCU_TDP_RATIO_DEF                  (0x00000000)

#define PCU_CR_CONFIG_TDP_LEVEL1_PCU_REG                             (0x00005F40)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_TDP_OFF                     ( 0)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_TDP_WID                     (15)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_TDP_MSK                     (0x00007FFF)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_TDP_MAX                     (0x00007FFF)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_TDP_DEF                     (0x00000000)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_TDP_RATIO_OFF                   (16)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_TDP_RATIO_WID                   ( 8)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_TDP_RATIO_MSK                   (0x00FF0000)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_TDP_RATIO_MAX                   (0x000000FF)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_TDP_RATIO_DEF                   (0x00000000)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MAX_PWR_OFF                 (32)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MAX_PWR_WID                 (15)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MAX_PWR_MSK                 (0x7FFF00000000)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MAX_PWR_MAX                 (0x00007FFF)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MAX_PWR_DEF                 (0x00000000)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MIN_PWR_OFF                 (47)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MIN_PWR_WID                 (16)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MIN_PWR_MSK                 (0x7FFF800000000000)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MIN_PWR_MAX                 (0x0000FFFF)
  #define PCU_CR_CONFIG_TDP_LEVEL1_PCU_PKG_MIN_PWR_DEF                 (0x00000000)

#define PCU_CR_CONFIG_TDP_LEVEL2_PCU_REG                             (0x00005F48)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_TDP_OFF                     ( 0)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_TDP_WID                     (15)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_TDP_MSK                     (0x00007FFF)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_TDP_MAX                     (0x00007FFF)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_TDP_DEF                     (0x00000000)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_TDP_RATIO_OFF                   (16)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_TDP_RATIO_WID                   ( 8)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_TDP_RATIO_MSK                   (0x00FF0000)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_TDP_RATIO_MAX                   (0x000000FF)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_TDP_RATIO_DEF                   (0x00000000)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MAX_PWR_OFF                 (32)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MAX_PWR_WID                 (15)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MAX_PWR_MSK                 (0x7FFF00000000)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MAX_PWR_MAX                 (0x00007FFF)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MAX_PWR_DEF                 (0x00000000)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MIN_PWR_OFF                 (47)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MIN_PWR_WID                 (16)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MIN_PWR_MSK                 (0x7FFF800000000000)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MIN_PWR_MAX                 (0x0000FFFF)
  #define PCU_CR_CONFIG_TDP_LEVEL2_PCU_PKG_MIN_PWR_DEF                 (0x00000000)

#define PCU_CR_CONFIG_TDP_CONTROL_PCU_REG                            (0x00005F50)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_TDP_LEVEL_OFF                  ( 0)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_TDP_LEVEL_WID                  ( 2)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_TDP_LEVEL_MSK                  (0x00000003)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_TDP_LEVEL_MAX                  (0x00000003)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_TDP_LEVEL_DEF                  (0x00000000)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_CONFIG_TDP_LOCK_OFF            (31)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_CONFIG_TDP_LOCK_WID            ( 1)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_CONFIG_TDP_LOCK_MSK            (0x80000000)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_CONFIG_TDP_LOCK_MAX            (0x00000001)
  #define PCU_CR_CONFIG_TDP_CONTROL_PCU_CONFIG_TDP_LOCK_DEF            (0x00000000)

#define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_REG                        (0x00005F54)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_MAX_NON_TURBO_RATIO_OFF    ( 0)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_MAX_NON_TURBO_RATIO_WID    ( 8)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_MAX_NON_TURBO_RATIO_MSK    (0x000000FF)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_MAX_NON_TURBO_RATIO_MAX    (0x000000FF)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_MAX_NON_TURBO_RATIO_DEF    (0x00000000)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_TURBO_ACTIVATION_RATIO_LOCK_OFF (31)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_TURBO_ACTIVATION_RATIO_LOCK_WID ( 1)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_TURBO_ACTIVATION_RATIO_LOCK_MSK (0x80000000)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_TURBO_ACTIVATION_RATIO_LOCK_MAX (0x00000001)
  #define PCU_CR_TURBO_ACTIVATION_RATIO_PCU_TURBO_ACTIVATION_RATIO_LOCK_DEF (0x00000000)

#define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_REG                           (0x00006680)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_DATA_OFF                      ( 0)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_DATA_WID                      (18)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_DATA_MSK                      (0x0003FFFF)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_DATA_MAX                      (0x0003FFFF)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_DATA_DEF                      (0x00000000)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_VORANGE_OFF                   (31)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_VORANGE_WID                   ( 1)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_VORANGE_MSK                   (0x80000000)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_VORANGE_MAX                   (0x00000001)
  #define DRNG_CR_VISA_CTL_DRNGCTLS_DRNG_VORANGE_DEF                   (0x00000000)

#pragma pack(pop)
#endif  // __Msa_h__
