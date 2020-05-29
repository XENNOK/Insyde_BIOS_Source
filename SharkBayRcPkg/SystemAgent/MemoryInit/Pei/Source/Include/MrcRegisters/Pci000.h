/** @file

  PCI bus 0, device 0, function 0 register definitions
  
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
#ifndef _PCI_000_H_
#define _PCI_000_H_

#pragma pack (push, 1)
#include <MrcTypes.h>

typedef union {
  struct {
    U32 Mchbaren : 1;  /// Bits 0:0
    U32          : 14; /// Bits 14:1
    U32 Mchbar   : 17; /// Bits 31:15
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_MCHBAR_LOW_STRUCT;

typedef union {
  struct {
    U32 Mchbar   : 7;  /// Bits 38:32
    U32          : 25; /// Bits 63:39
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_MCHBAR_HIGH_STRUCT;

typedef union {
  struct {
    U64 Mchbaren : 1;  /// Bits 0:0
    U64          : 14; /// Bits 14:1
    U64 Mchbar   : 24; /// Bits 38:15
    U64          : 25; /// Bits 63:39
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_MCHBAR_LOW_STRUCT  Low;
    MRC_PCI_000_MCHBAR_HIGH_STRUCT High;
  } Data32;
} MRC_PCI_000_MCHBAR_STRUCT;

#define MRC_PCI_000_MCHBAR_REG (0x48)
  #define MCHBAR_MCHBAREN_OFF  (0)
  #define MCHBAR_MCHBAREN_WID  (1)
  #define MCHBAR_MCHBAREN_MSK  (0x1)
  #define MCHBAR_MCHBAREN_MAX  (0x1)
  #define MCHBAR_MCHBAREN_DEF  (0x0)
  #define MCHBAR_MCHBAR_OFF    (15)
  #define MCHBAR_MCHBAR_WID    (24)
  #define MCHBAR_MCHBAR_MSK    (0x0000007FFFFF8000)
  #define MCHBAR_MCHBAR_MAX    (0xFFFFFF)
  #define MCHBAR_MCHBAR_DEF    (0x0)

typedef union {
  struct {
    U32 Ggclck : 1;  /// Bits 0:0
    U32 Ivd    : 1;  /// Bits 1:1
    U32        : 1;  /// Bits 2:2
    U32 Gms    : 5;  /// Bits 7:3
    U32 Ggms   : 2;  /// Bits 9:8
    U32        : 4;  /// Bits 13:10
    U32 Vamen  : 1;  /// Bits 14:14
    U32        : 17; /// Bits 31:15
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_GGC_STRUCT;

#define MRC_PCI_000_GGC_REG (0x50)
  #define GGC_GGCLCK_OFF    (0)
  #define GGC_GGCLCK_WID    (1)
  #define GGC_GGCLCK_MSK    (0x1)
  #define GGC_GGCLCK_MAX    (0x1)
  #define GGC_GGCLCK_DEF    (0x0)
  #define GGC_IVD_OFF       (1)
  #define GGC_IVD_WID       (1)
  #define GGC_IVD_MSK       (0x2)
  #define GGC_IVD_MAX       (0x1)
  #define GGC_IVD_DEF       (0x0)
  #define GGC_GMS_OFF       (3)
  #define GGC_GMS_WID       (5)
  #define GGC_GMS_MSK       (0xF8)
  #define GGC_GMS_MAX       (0x1F)
  #define GGC_GMS_DEF       (0x5)
  #define GGC_GGMS_OFF      (8)
  #define GGC_GGMS_WID      (2)
  #define GGC_GGMS_MSK      (0x300)
  #define GGC_GGMS_MAX      (0x3)
  #define GGC_GGMS_DEF      (0x0)
  #define GGC_VAMEN_OFF     (0xe)
  #define GGC_VAMEN_WID     (0x1)
  #define GGC_VAMEN_MSK     (0x4000)
  #define GGC_VAMEN_MAX     (0x1)
  #define GGC_VAMEN_DEF     (0x0)

typedef union {
  struct {
    U32 D0EN   : 1;  /// Bits 0:0
    U32 D1F2EN : 1;  /// Bits 1:1
    U32 D1F1EN : 1;  /// Bits 2:2
    U32 D1F0EN : 1;  /// Bits 3:3
    U32 D2EN   : 1;  /// Bits 4:4
    U32 D3EN   : 1;  /// Bits 5:5
    U32        : 1;  /// Bits 6:6
    U32 D4EN   : 1;  /// Bits 7:7
    U32        : 6;  /// Bits 13:8
    U32 D7EN   : 1;  /// Bits 14:14
    U32        : 17; /// Bits 31:15
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_DEVEN_STRUCT;

#define MRC_PCI_000_DEVEN_REG (0x54)
  #define DEVEN_D0EN_OFF      (0)
  #define DEVEN_D0EN_WID      (1)
  #define DEVEN_D0EN_MSK      (0x1)
  #define DEVEN_D0EN_MAX      (0x1)
  #define DEVEN_D0EN_DEF      (0x1)
  #define DEVEN_D1F2EN_OFF    (1)
  #define DEVEN_D1F2EN_WID    (1)
  #define DEVEN_D1F2EN_MSK    (0x2)
  #define DEVEN_D1F2EN_MAX    (0x1)
  #define DEVEN_D1F2EN_DEF    (0x1)
  #define DEVEN_D1F1EN_OFF    (2)
  #define DEVEN_D1F1EN_WID    (1)
  #define DEVEN_D1F1EN_MSK    (0x4)
  #define DEVEN_D1F1EN_MAX    (0x1)
  #define DEVEN_D1F1EN_DEF    (0x1)
  #define DEVEN_D1F0EN_OFF    (3)
  #define DEVEN_D1F0EN_WID    (1)
  #define DEVEN_D1F0EN_MSK    (0x8)
  #define DEVEN_D1F0EN_MAX    (0x1)
  #define DEVEN_D1F0EN_DEF    (0x1)
  #define DEVEN_D2EN_OFF      (4)
  #define DEVEN_D2EN_WID      (1)
  #define DEVEN_D2EN_MSK      (0x10)
  #define DEVEN_D2EN_MAX      (0x1)
  #define DEVEN_D2EN_DEF      (0x1)
  #define DEVEN_D3EN_OFF      (5)
  #define DEVEN_D3EN_WID      (1)
  #define DEVEN_D3EN_MSK      (0x20)
  #define DEVEN_D3EN_MAX      (0x1)
  #define DEVEN_D3EN_DEF      (0x1)
  #define DEVEN_D4EN_OFF      (7)
  #define DEVEN_D4EN_WID      (1)
  #define DEVEN_D4EN_MSK      (0x80)
  #define DEVEN_D4EN_MAX      (0x1)
  #define DEVEN_D4EN_DEF      (0x1)
  #define DEVEN_D7EN_OFF      (14)
  #define DEVEN_D7EN_WID      (1)
  #define DEVEN_D7EN_MSK      (0x4000)
  #define DEVEN_D7EN_MAX      (0x1)
  #define DEVEN_D7EN_DEF      (0x0)

typedef union {
  struct {
    U32 Pciexbaren : 1;  /// Bits 0:0
    U32 Length     : 2;  /// Bits 2:1
    U32            : 23; /// Bits 25:3
    U32 Admsk64    : 1;  /// Bits 26:26
    U32 Admsk128   : 1;  /// Bits 27:27
    U32 Pciexbar   : 4;  /// Bits 31:28
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_PCIEXBAR_LOW_STRUCT;

typedef union {
  struct {
    U32 Pciexbar   : 7;  /// Bits 38:32
    U32            : 25; /// Bits 63:39
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_PCIEXBAR_HIGH_STRUCT;

typedef union {
  struct {
    U64 Pciexbaren : 1;  /// Bits 0:0
    U64 Length     : 2;  /// Bits 2:1
    U64            : 23; /// Bits 25:3
    U64 Admsk64    : 1;  /// Bits 26:26
    U64 Admsk128   : 1;  /// Bits 27:27
    U64 Pciexbar   : 11; /// Bits 38:28
    U64            : 25; /// Bits 63:39
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_PCIEXBAR_LOW_STRUCT  Low;
    MRC_PCI_000_PCIEXBAR_HIGH_STRUCT High;
  } Data32;
} MRC_PCI_000_PCIEXBAR_STRUCT;

#define MRC_PCI_000_PCIEXBAR_REG  (0x60)
  #define PCIEXBAR_PCIEXBAREN_OFF (0)
  #define PCIEXBAR_PCIEXBAREN_WID (1)
  #define PCIEXBAR_PCIEXBAREN_MSK (0x1)
  #define PCIEXBAR_PCIEXBAREN_MAX (0x1)
  #define PCIEXBAR_PCIEXBAREN_DEF (0x0)
  #define PCIEXBAR_LENGTH_OFF     (1)
  #define PCIEXBAR_LENGTH_WID     (2)
  #define PCIEXBAR_LENGTH_MSK     (0x6)
  #define PCIEXBAR_LENGTH_MAX     (0x3)
  #define PCIEXBAR_LENGTH_DEF     (0x0)
  #define PCIEXBAR_ADMSK64_OFF    (26)
  #define PCIEXBAR_ADMSK64_WID    (1)
  #define PCIEXBAR_ADMSK64_MSK    (0x4000000)
  #define PCIEXBAR_ADMSK64_MAX    (0x1)
  #define PCIEXBAR_ADMSK64_DEF    (0x1)
  #define PCIEXBAR_ADMSK128_OFF   (27)
  #define PCIEXBAR_ADMSK128_WID   (1)
  #define PCIEXBAR_ADMSK128_MSK   (0x8000000)
  #define PCIEXBAR_ADMSK128_MAX   (0x1)
  #define PCIEXBAR_ADMSK128_DEF   (0x1)
  #define PCIEXBAR_PCIEXBAR_OFF   (28)
  #define PCIEXBAR_PCIEXBAR_WID   (11)
  #define PCIEXBAR_PCIEXBAR_MSK   (0x7FF0000000)
  #define PCIEXBAR_PCIEXBAR_MAX   (0x7FF)
  #define PCIEXBAR_PCIEXBAR_DEF   (0x0)

typedef union {
  struct {
    U32       : 20; /// Bits 19:0
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_MESEG_BASE_LOW_STRUCT;

typedef union {
  struct {
    U32 Value : 7;  /// Bits 38:32
    U32       : 25; /// Bits 63:39
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_MESEG_BASE_HIGH_STRUCT;

typedef union {
  struct {
    U64       : 20; /// Bits 19:0
    U64 Value : 19; /// Bits 38:20
    U64       : 25; /// Bits 63:39
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_MESEG_BASE_LOW_STRUCT  Low;
    MRC_PCI_000_MESEG_BASE_HIGH_STRUCT High;
  } Data32;
} MRC_PCI_000_MESEG_BASE_STRUCT;

#define MRC_PCI_000_MESEG_BASE_REG  (0x70)
  #define MESEG_BASE_MEBASE_OFF     (20)
  #define MESEG_BASE_MEBASE_WID     (19)
  #define MESEG_BASE_MEBASE_MSK     (0x7FFFF00000)
  #define MESEG_BASE_MEBASE_MAX     (0x7FFFF)
  #define MESEG_BASE_MEBASE_DEF     (0x7FFFF)

typedef union {
  struct {
    U32        : 10; /// Bits 9:0
    U32 Lock   : 1;  /// Bits 10:10
    U32 Enable : 1;  /// Bits 11:11
    U32        : 8;  /// Bits 19:12
    U32 Value  : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_MESEG_MASK_LOW_STRUCT;

typedef union {
  struct {
    U32 Value  : 7;  /// Bits 38:32
    U32        : 25; /// Bits 63:39
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_MESEG_MASK_HIGH_STRUCT;

typedef union {
  struct {
    U64        : 10; /// Bits 9:0
    U64 Lock   : 1;  /// Bits 10:10
    U64 Enable : 1;  /// Bits 11:11
    U64        : 8;  /// Bits 19:12
    U64 Value  : 19; /// Bits 38:20
    U64        : 25; /// Bits 63:39
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_MESEG_MASK_LOW_STRUCT  Low;
    MRC_PCI_000_MESEG_MASK_HIGH_STRUCT High;
  } Data32;
} MRC_PCI_000_MESEG_MASK_STRUCT;

#define MRC_PCI_000_MESEG_MASK_REG   (0x78)
  #define MESEG_MASK_MELCK_OFF       (10)
  #define MESEG_MASK_MELCK_WID       (1)
  #define MESEG_MASK_MELCK_MSK       (0x400)
  #define MESEG_MASK_MELCK_MAX       (1)
  #define MESEG_MASK_MELCK_DEF       (0x0)
  #define MESEG_MASK_ME_STLEN_EN_OFF (11)
  #define MESEG_MASK_ME_STLEN_EN_WID (1)
  #define MESEG_MASK_ME_STLEN_EN_MSK (0x800)
  #define MESEG_MASK_ME_STLEN_EN_MAX (0x1)
  #define MESEG_MASK_ME_STLEN_EN_DEF (0x0)
  #define MESEG_MASK_MEMASK_OFF      (20)
  #define MESEG_MASK_MEMASK_WID      (19)
  #define MESEG_MASK_MEMASK_MSK      (0x7FFFF00000)
  #define MESEG_MASK_MEMASK_MAX      (0x7FFFF)
  #define MESEG_MASK_MEMASK_DEF      (0x0)

typedef union {
  struct {
    U32 Lock  : 1;  /// Bits 0:0
    U32       : 19; /// Bits 19:1
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_REMAPBASE_LOW_STRUCT;

typedef union {
  struct {
    U32 Value : 7;  /// Bits 38:32
    U32       : 25; /// Bits 63:39
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_REMAPBASE_HIGH_STRUCT;

typedef union {
  struct {
    U64 Lock  : 1;  /// Bits 0:0
    U64       : 19; /// Bits 19:1
    U64 Value : 19; /// Bits 38:20
    U64       : 25; /// Bits 63:39
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_REMAPBASE_LOW_STRUCT  Low;
    MRC_PCI_000_REMAPBASE_HIGH_STRUCT High;
  } Data32;
} MRC_PCI_000_REMAPBASE_STRUCT;

#define MRC_PCI_000_REMAPBASE_REG (0x90)
  #define REMAPBASE_LOCK_OFF      (0)
  #define REMAPBASE_LOCK_WID      (1)
  #define REMAPBASE_LOCK_MSK      (0x1)
  #define REMAPBASE_LOCK_MAX      (0x1)
  #define REMAPBASE_LOCK_DEF      (0x0)
  #define REMAPBASE_REMAPBASE_OFF (20)
  #define REMAPBASE_REMAPBASE_WID (19)
  #define REMAPBASE_REMAPBASE_MSK (0x7FFFF00000)
  #define REMAPBASE_REMAPBASE_MAX (0x7FFFF)
  #define REMAPBASE_REMAPBASE_DEF (0xFFFFF)

typedef union {
  struct {
    U32 Lock  : 1;  /// Bits 0:0
    U32       : 19; /// Bits 19:1
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_REMAPLIMIT_LOW_STRUCT;

typedef union {
  struct {
    U32 Value : 7;  /// Bits 38:32
    U32       : 25; /// Bits 63:39
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_REMAPLIMIT_HIGH_STRUCT;

typedef union {
  struct {
    U64 Lock  : 1;  /// Bits 0:0
    U64       : 19; /// Bits 19:1
    U64 Value : 19; /// Bits 38:20
    U64       : 25; /// Bits 63:39
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_REMAPLIMIT_LOW_STRUCT  Low;
    MRC_PCI_000_REMAPLIMIT_HIGH_STRUCT High;
  } Data32;
} MRC_PCI_000_REMAPLIMIT_STRUCT;

#define MRC_PCI_000_REMAPLIMIT_REG (0x98)
  #define REMAPLIMIT_LOCK_OFF      (0)
  #define REMAPLIMIT_LOCK_WID      (1)
  #define REMAPLIMIT_LOCK_MSK      (0x1)
  #define REMAPLIMIT_LOCK_MAX      (0x1)
  #define REMAPLIMIT_LOCK_DEF      (0x0)
  #define REMAPLIMIT_REMAPLMT_OFF  (20)
  #define REMAPLIMIT_REMAPLMT_WID  (19)
  #define REMAPLIMIT_REMAPLMT_MSK  (0x7FFFF00000)
  #define REMAPLIMIT_REMAPLMT_MAX  (0x7FFFF)
  #define REMAPLIMIT_REMAPLMT_DEF  (0x0)

typedef union {
  struct {
    U32 Lock  : 1;  /// Bits 0:0
    U32       : 19; /// Bits 19:1
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_TOM_LOW_STRUCT;

typedef union {
  struct {
    U32 Value : 7;  /// Bits 38:32
    U32       : 25; /// Bits 63:39
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_TOM_HIGH_STRUCT;

typedef union {
  struct {
    U64 Lock  : 1;  /// Bits 0:0
    U64       : 19; /// Bits 19:1
    U64 Value : 19; /// Bits 38:20
    U64       : 25; /// Bits 63:39
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_TOM_LOW_STRUCT  Low;
    MRC_PCI_000_TOM_HIGH_STRUCT High;
  } Data32;
} MRC_PCI_000_TOM_STRUCT;

#define MRC_PCI_000_TOM_REG (0xA0)
  #define TOM_LOCK_OFF      (0)
  #define TOM_LOCK_WID      (1)
  #define TOM_LOCK_MSK      (0x1)
  #define TOM_LOCK_MAX      (0x1)
  #define TOM_LOCK_DEF      (0x0)
  #define TOM_TOM_OFF       (20)
  #define TOM_TOM_WID       (19)
  #define TOM_TOM_MSK       (0x7FFFF00000)
  #define TOM_TOM_MAX       (0x7FFFF)
  #define TOM_TOM_DEF       (0x7FFFF)

typedef union {
  struct {
    U32 Lock  : 1;  /// Bits 0:0
    U32       : 19; /// Bits 19:1
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_TOUUD_LOW_STRUCT;

typedef union {
  struct {
    U32 Value : 7;  /// Bits 38:32
    U32       : 25; /// Bits 63:39
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_TOUUD_HIGH_STRUCT;

typedef union {
  struct {
    U64 Lock  : 1;  /// Bits 0:0
    U64       : 19; /// Bits 19:1
    U64 Value : 19; /// Bits 38:20
    U64       : 25; /// Bits 63:39
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_TOUUD_LOW_STRUCT  Low;
    MRC_PCI_000_TOUUD_HIGH_STRUCT High;
  } Data32;
} MRC_PCI_000_TOUUD_STRUCT;

#define MRC_PCI_000_TOUUD_REG (0xA8)
  #define TOUUD_LOCK_OFF      (0)
  #define TOUUD_LOCK_WID      (1)
  #define TOUUD_LOCK_MSK      (0x1)
  #define TOUUD_LOCK_MAX      (0x1)
  #define TOUUD_LOCK_DEF      (0x0)
  #define TOUUD_TOUUD_OFF     (20)
  #define TOUUD_TOUUD_WID     (19)
  #define TOUUD_TOUUD_MSK     (0x7FFFF00000)
  #define TOUUD_TOUUD_MAX     (0x7FFFF)
  #define TOUUD_TOUUD_DEF     (0x0)

typedef union {
  struct {
    U32 Lock  : 1;  /// Bits 0:0
    U32       : 19; /// Bits 19:1
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_BDSM_STRUCT;

#define MRC_PCI_000_BDSM_REG (0xB0)
  #define BDSM_LOCK_OFF      (0)
  #define BDSM_LOCK_WID      (1)
  #define BDSM_LOCK_MSK      (0x1)
  #define BDSM_LOCK_MAX      (0x1)
  #define BDSM_LOCK_DEF      (0x0)
  #define BDSM_BDSM_OFF      (20)
  #define BDSM_BDSM_WID      (12)
  #define BDSM_BDSM_MSK      (0xFFF00000)
  #define BDSM_BDSM_MAX      (0xFFF)
  #define BDSM_BDSM_DEF      (0x0)

typedef union {
  struct {
    U32 Lock  : 1;  /// Bits 0:0
    U32       : 19; /// Bits 19:1
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_BGSM_STRUCT;

#define MRC_PCI_000_BGSM_REG (0xB4)
  #define BGSM_LOCK_OFF      (0)
  #define BGSM_LOCK_WID      (1)
  #define BGSM_LOCK_MSK      (0x1)
  #define BGSM_LOCK_MAX      (0x1)
  #define BGSM_LOCK_DEF      (0x0)
  #define BGSM_BGSM_OFF      (20)
  #define BGSM_BGSM_WID      (12)
  #define BGSM_BGSM_MSK      (0xFFF00000)
  #define BGSM_BGSM_MAX      (0xFFF)
  #define BGSM_BGSM_DEF      (0x001)

typedef union {
  struct {
    U32 Lock  : 1;  /// Bits 0:0
    U32       : 19; /// Bits 19:1
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_TSEGMB_STRUCT;

#define MRC_PCI_000_TSEGMB_REG (0xB8)
  #define TSEGMB_LOCK_OFF      (0)
  #define TSEGMB_LOCK_WID      (1)
  #define TSEGMB_LOCK_MSK      (0x1)
  #define TSEGMB_LOCK_MAX      (0x1)
  #define TSEGMB_LOCK_DEF      (0x0)
  #define TSEGMB_TSEGMB_OFF    (20)
  #define TSEGMB_TSEGMB_WID    (12)
  #define TSEGMB_TSEGMB_MSK    (0xFFF00000)
  #define TSEGMB_TSEGMB_MAX    (0xFFF)
  #define TSEGMB_TSEGMB_DEF    (0x0)

typedef union {
  struct {
    U32 Lock    : 1;  /// Bits 0:0
    U32 Prs     : 1;  /// Bits 1:1
    U32 Epm     : 1;  /// Bits 2:2
    U32         : 1;  /// Bits 3:3
    U32 Dprsize : 8;  /// Bits 11:4
    U32         : 20; /// Bits 31:12
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_DPR_STRUCT;

#define MRC_PCI_000_DPR_REG (0x5c)
  #define DPR_LOCK_OFF      (0)
  #define DPR_LOCK_WID      (1)
  #define DPR_LOCK_MSK      (0x1)
  #define DPR_LOCK_MAX      (0x1)
  #define DPR_LOCK_DEF      (0x0)
  #define DPR_EPM_OFF       (2)
  #define DPR_EPM_WID       (1)
  #define DPR_EPM_MSK       (0x4)
  #define DPR_EPM_MAX       (0x1)
  #define DPR_EPM_DEF       (0x0)

typedef union {
  struct {
    U32 Lock  : 1;  /// Bits 0:0
    U32       : 19; /// Bits 19:1
    U32 Value : 12; /// Bits 31:20
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_TOLUD_STRUCT;

#define MRC_PCI_000_TOLUD_REG (0xBC)
  #define TOLUD_LOCK_OFF      (0)
  #define TOLUD_LOCK_WID      (1)
  #define TOLUD_LOCK_MSK      (0x1)
  #define TOLUD_LOCK_MAX      (0x1)
  #define TOLUD_LOCK_DEF      (0x0)
  #define TOLUD_TOLUD_OFF     (20)
  #define TOLUD_TOLUD_WID     (12)
  #define TOLUD_TOLUD_MSK     (0xFFF00000)
  #define TOLUD_TOLUD_MAX     (0xFFF)
  #define TOLUD_TOLUD_DEF     (0x001)

typedef union {
  struct {
    U32 DDR3L_EN        : 1;  /// Bits 0:0
    U32 DDR_WRTVREF     : 1;  /// Bits 1:1
    U32 OC_ENABLED_DSKU : 1;  /// Bits 2:2
    U32 DDR_OVERCLOCK   : 1;  /// Bits 3:3
    U32 CRID            : 4;  /// Bits 7:4
    U32 CDID            : 2;  /// Bits 9:8
    U32 DIDOE           : 1;  /// Bits 10:10
    U32 IGD             : 1;  /// Bits 11:11
    U32 PDCD            : 1;  /// Bits 12:12
    U32 X2APIC_EN       : 1;  /// Bits 13:13
    U32 DDPCD           : 1;  /// Bits 14:14
    U32 CDD             : 1;  /// Bits 15:15
    U32 FUFRD           : 1;  /// Bits 16:16
    U32 D1NM            : 1;  /// Bits 17:17
    U32 PCIE_RATIO_DIS  : 1;  /// Bits 18:18
    U32 DDRSZ           : 2;  /// Bits 20:19
    U32 PEGG2DIS        : 1;  /// Bits 21:21
    U32 DMIG2DIS        : 1;  /// Bits 22:22
    U32 VTDDD           : 1;  /// Bits 23:23
    U32 FDEE            : 1;  /// Bits 24:24
    U32 ECCDIS          : 1;  /// Bits 25:25
    U32 DW              : 1;  /// Bits 26:26
    U32 PELWUD          : 1;  /// Bits 27:27
    U32 PEG10D          : 1;  /// Bits 28:28
    U32 PEG11D          : 1;  /// Bits 29:29
    U32 PEG12D          : 1;  /// Bits 30:30
    U32 DHDAD           : 1;  /// Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_CAPID0_A_STRUCT;

#define MRC_PCI_000_CAPID0_REG        (0xE4)
#define MRC_PCI_000_CAPID0_A_REG      (0xE4)
  #define CAPID0_A_DDR3L_EN_OFF       (0)
  #define CAPID0_A_DDR3L_EN_WID       (1)
  #define CAPID0_A_DDR3L_EN_MSK       (0x1)
  #define CAPID0_A_DDR3L_EN_MAX       (0x1)
  #define CAPID0_A_DDR3L_EN_DEF       (0x0)
  #define CAPID0_A_DDR_WRTVREF_OFF    (0x0)
  #define CAPID0_A_DDR_WRTVREF_WID    (0x1)
  #define CAPID0_A_DDR_WRTVREF_MSK    (0x1)
  #define CAPID0_A_DDR_WRTVREF_MAX    (0x1)
  #define CAPID0_A_DDR_WRTVREF_DEF    (0x0)
  #define CAPID0_A_DDR_OVERCLOCK_OFF  (3)
  #define CAPID0_A_DDR_OVERCLOCK_WID  (1)
  #define CAPID0_A_DDR_OVERCLOCK_MSK  (0x8)
  #define CAPID0_A_DDR_OVERCLOCK_MAX  (0x1)
  #define CAPID0_A_DDR_OVERCLOCK_DEF  (0x0)
  #define CAPID0_A_CRID_OFF           (4)
  #define CAPID0_A_CRID_WID           (4)
  #define CAPID0_A_CRID_MSK           (0xF0)
  #define CAPID0_A_CRID_MAX           (0xF)
  #define CAPID0_A_CRID_DEF           (0x0)
  #define CAPID0_A_CDID_OFF           (8)
  #define CAPID0_A_CDID_WID           (2)
  #define CAPID0_A_CDID_MSK           (0x300)
  #define CAPID0_A_CDID_MAX           (0x3)
  #define CAPID0_A_CDID_DEF           (0x0)
  #define CAPID0_A_DIDOE_OFF          (10)
  #define CAPID0_A_DIDOE_WID          (1)
  #define CAPID0_A_DIDOE_MSK          (0x400)
  #define CAPID0_A_DIDOE_MAX          (0x1)
  #define CAPID0_A_DIDOE_DEF          (0x0)
  #define CAPID0_A_IGD_OFF            (11)
  #define CAPID0_A_IGD_WID            (1)
  #define CAPID0_A_IGD_MSK            (0x800)
  #define CAPID0_A_IGD_MAX            (0x1)
  #define CAPID0_A_IGD_DEF            (0x0)
  #define CAPID0_A_PDCD_OFF           (12)
  #define CAPID0_A_PDCD_WID           (1)
  #define CAPID0_A_PDCD_MSK           (0x1000)
  #define CAPID0_A_PDCD_MAX           (0x1)
  #define CAPID0_A_PDCD_DEF           (0x0)
  #define CAPID0_A_X2APIC_EN_OFF      (13)
  #define CAPID0_A_X2APIC_EN_WID      (1)
  #define CAPID0_A_X2APIC_EN_MSK      (0x2000)
  #define CAPID0_A_X2APIC_EN_MAX      (0x1)
  #define CAPID0_A_X2APIC_EN_DEF      (0x0)
  #define CAPID0_A_DDPCD_OFF          (14)
  #define CAPID0_A_DDPCD_WID          (1)
  #define CAPID0_A_DDPCD_MSK          (0x4000)
  #define CAPID0_A_DDPCD_MAX          (0x1)
  #define CAPID0_A_DDPCD_DEF          (0x0)
  #define CAPID0_A_CDD_OFF            (15)
  #define CAPID0_A_CDD_WID            (1)
  #define CAPID0_A_CDD_MSK            (0x8000)
  #define CAPID0_A_CDD_MAX            (0x1)
  #define CAPID0_A_CDD_DEF            (0x0)
  #define CAPID0_A_FUFRD_OFF          (16)
  #define CAPID0_A_FUFRD_WID          (1)
  #define CAPID0_A_FUFRD_MSK          (0x10000)
  #define CAPID0_A_FUFRD_MAX          (0x1)
  #define CAPID0_A_FUFRD_DEF          (0x0)
  #define CAPID0_A_D1NM_OFF           (17)
  #define CAPID0_A_D1NM_WID           (1)
  #define CAPID0_A_D1NM_MSK           (0x20000)
  #define CAPID0_A_D1NM_MAX           (0x1)
  #define CAPID0_A_D1NM_DEF           (0x0)
  #define CAPID0_A_PEGX16D_OFF        (18)
  #define CAPID0_A_PEGX16D_WID        (1)
  #define CAPID0_A_PEGX16D_MSK        (0x40000)
  #define CAPID0_A_PEGX16D_MAX        (0x1)
  #define CAPID0_A_PEGX16D_DEF        (0x0)
  #define CAPID0_A_DDRSZ_OFF          (19)
  #define CAPID0_A_DDRSZ_WID          (2)
  #define CAPID0_A_DDRSZ_MSK          (0x180000)
  #define CAPID0_A_DDRSZ_MAX          (0x3)
  #define CAPID0_A_DDRSZ_DEF          (0x0)
  #define CAPID0_A_PEGG2DIS_OFF       (21)
  #define CAPID0_A_PEGG2DIS_WID       (1)
  #define CAPID0_A_PEGG2DIS_MSK       (0x200000)
  #define CAPID0_A_PEGG2DIS_MAX       (0x1)
  #define CAPID0_A_PEGG2DIS_DEF       (0x0)
  #define CAPID0_A_DMIG2DIS_OFF       (22)
  #define CAPID0_A_DMIG2DIS_WID       (1)
  #define CAPID0_A_DMIG2DIS_MSK       (0x400000)
  #define CAPID0_A_DMIG2DIS_MAX       (0x1)
  #define CAPID0_A_DMIG2DIS_DEF       (0x0)
  #define CAPID0_A_VTDD_OFF           (23)
  #define CAPID0_A_VTDD_WID           (1)
  #define CAPID0_A_VTDD_MSK           (0x800000)
  #define CAPID0_A_VTDD_MAX           (0x1)
  #define CAPID0_A_VTDD_DEF           (0x0)
  #define CAPID0_A_FDEE_OFF           (24)
  #define CAPID0_A_FDEE_WID           (1)
  #define CAPID0_A_FDEE_MSK           (0x1000000)
  #define CAPID0_A_FDEE_MAX           (0x1)
  #define CAPID0_A_FDEE_DEF           (0x0)
  #define CAPID0_A_ECCDIS_OFF         (25)
  #define CAPID0_A_ECCDIS_WID         (1)
  #define CAPID0_A_ECCDIS_MSK         (0x2000000)
  #define CAPID0_A_ECCDIS_MAX         (0x1)
  #define CAPID0_A_ECCDIS_DEF         (0x0)
  #define CAPID0_A_DW_OFF             (26)
  #define CAPID0_A_DW_WID             (1)
  #define CAPID0_A_DW_MSK             (0x4000000)
  #define CAPID0_A_DW_MAX             (0x1)
  #define CAPID0_A_DW_DEF             (0x0)
  #define CAPID0_A_PELWUD_OFF         (27)
  #define CAPID0_A_PELWUD_WID         (1)
  #define CAPID0_A_PELWUD_MSK         (0x8000000)
  #define CAPID0_A_PELWUD_MAX         (0x1)
  #define CAPID0_A_PELWUD_DEF         (0x0)
  #define CAPID0_A_PEG10D_OFF         (28)
  #define CAPID0_A_PEG10D_WID         (1)
  #define CAPID0_A_PEG10D_MSK         (0x10000000)
  #define CAPID0_A_PEG10D_MAX         (0x1)
  #define CAPID0_A_PEG10D_DEF         (0x0)
  #define CAPID0_A_PEG11D_OFF         (29)
  #define CAPID0_A_PEG11D_WID         (1)
  #define CAPID0_A_PEG11D_MSK         (0x20000000)
  #define CAPID0_A_PEG11D_MAX         (0x1)
  #define CAPID0_A_PEG11D_DEF         (0x0)
  #define CAPID0_A_PEG12D_OFF         (30)
  #define CAPID0_A_PEG12D_WID         (1)
  #define CAPID0_A_PEG12D_MSK         (0x40000000)
  #define CAPID0_A_PEG12D_MAX         (0x1)
  #define CAPID0_A_PEG12D_DEF         (0x0)

typedef union {
  struct {
    U32 SPEGFX1         : 1;  /// Bits 0:0
    U32 DPEGFX1         : 1;  /// Bits 1:1
    U32                 : 2;  /// Bits 3:2
    U32 DMFC            : 3;  /// Bits 6:4
    U32 DDD             : 1;  /// Bits 7:7
    U32                 : 3;  /// Bits 10:8
    U32 HDCPD           : 1;  /// Bits 11:11
    U32                 : 4;  /// Bits 15:12
    U32 PEGX16D         : 1;  /// Bits 16:16
    U32 ADDGFXCAP       : 1;  /// Bits 17:17
    U32 ADDGFXEN        : 1;  /// Bits 18:18
    U32 PKGTYP          : 1;  /// Bits 19:19
    U32 PEGG3_DIS       : 1;  /// Bits 20:20
    U32 PLL_REF100_CFG  : 3;  /// Bits 23:21
    U32 SOFTBIN         : 1;  /// Bits 24:24
    U32 CACHESZ         : 3;  /// Bits 27:25
    U32 SMT             : 1;  /// Bits 28:28
    U32 OC_ENABLED_SSKU : 1;  /// Bits 29:29
    U32 OC_CTL_SSKU     : 1;  /// Bits 30:30
    U32                 : 1;  /// Bits 31:31
  } Bits;
  U32 Data;
  U16 Data16[2];
  U8  Data8[4];
} MRC_PCI_000_CAPID0_B_STRUCT;

#define MRC_PCI_000_CAPID0_B_REG        (0xE8)
  #define CAPID0_B_SPEGFX1_OFF          (0)
  #define CAPID0_B_SPEGFX1_WID          (1)
  #define CAPID0_B_SPEGFX1_MSK          (0x1)
  #define CAPID0_B_SPEGFX1_MAX          (0x1)
  #define CAPID0_B_SPEGFX1_DEF          (0x0)
  #define CAPID0_B_DPEGFX1_OFF          (1)
  #define CAPID0_B_DPEGFX1_WID          (1)
  #define CAPID0_B_DPEGFX1_MSK          (0x2)
  #define CAPID0_B_DPEGFX1_MAX          (0x1)
  #define CAPID0_B_DPEGFX1_DEF          (0x0)
  #define CAPID0_B_DMFC_OFF             (4)
  #define CAPID0_B_DMFC_WID             (3)
  #define CAPID0_B_DMFC_MSK             (0x70)
  #define CAPID0_B_DMFC_MAX             (0x7)
  #define CAPID0_B_DMFC_DEF             (0x0)
  #define CAPID0_B_DDD_OFF              (7)
  #define CAPID0_B_DDD_WID              (1)
  #define CAPID0_B_DDD_MSK              (0x80)
  #define CAPID0_B_DDD_MAX              (0x1)
  #define CAPID0_B_DDD_DEF              (0x0)
  #define CAPID0_B_HGKS_OFF             (8)
  #define CAPID0_B_HGKS_WID             (3)
  #define CAPID0_B_HGKS_MSK             (0x700)
  #define CAPID0_B_HGKS_MAX             (0x7)
  #define CAPID0_B_HGKS_DEF             (0x0)
  #define CAPID0_B_HDCPD_OFF            (11)
  #define CAPID0_B_HDCPD_WID            (1)
  #define CAPID0_B_HDCPD_MSK            (0x800)
  #define CAPID0_B_HDCPD_MAX            (0x1)
  #define CAPID0_B_HDCPD_DEF            (0x0)
  #define CAPID0_B_ADDGFXCAP_OFF        (17)
  #define CAPID0_B_ADDGFXCAP_WID        (1)
  #define CAPID0_B_ADDGFXCAP_MSK        (0x20000)
  #define CAPID0_B_ADDGFXCAP_MAX        (0x1)
  #define CAPID0_B_ADDGFXCAP_DEF        (0x0)
  #define CAPID0_B_ADDGFXEN_OFF         (18)
  #define CAPID0_B_ADDGFXEN_WID         (1)
  #define CAPID0_B_ADDGFXEN_MSK         (0x40000)
  #define CAPID0_B_ADDGFXEN_MAX         (0x1)
  #define CAPID0_B_ADDGFXEN_DEF         (0x0)
  #define CAPID0_B_PKGTYP_OFF           (19)
  #define CAPID0_B_PKGTYP_WID           (1)
  #define CAPID0_B_PKGTYP_MSK           (0x80000)
  #define CAPID0_B_PKGTYP_MAX           (0x1)
  #define CAPID0_B_PKGTYP_DEF           (0x0)
  #define CAPID0_B_PLL_REF100_CFG_OFF   (21)
  #define CAPID0_B_PLL_REF100_CFG_WID   (3)
  #define CAPID0_B_PLL_REF100_CFG_MSK   (0xE00000)
  #define CAPID0_B_PLL_REF100_CFG_MAX   (0x7)
  #define CAPID0_B_PLL_REF100_CFG_DEF   (0x0)
  #define CAPID0_B_SOFTBIN_OFF          (24)
  #define CAPID0_B_SOFTBIN_WID          (1)
  #define CAPID0_B_SOFTBIN_MSK          (0x1000000)
  #define CAPID0_B_SOFTBIN_MAX          (0x1)
  #define CAPID0_B_SOFTBIN_DEF          (0x0)
  #define CAPID0_B_CACHESZ_OFF          (25)
  #define CAPID0_B_CACHESZ_WID          (3)
  #define CAPID0_B_CACHESZ_MSK          (0xe000000)
  #define CAPID0_B_CACHESZ_MAX          (0x7)
  #define CAPID0_B_CACHESZ_DEF          (0x0)
  #define CAPID0_B_SMT_OFF              (28)
  #define CAPID0_B_SMT_WID              (1)
  #define CAPID0_B_SMT_MSK              (0x10000000)
  #define CAPID0_B_SMT_MAX              (0x1)
  #define CAPID0_B_SMT_DEF              (0x0)
  #define CAPID0_B_OC_ENABLED_SSKU_OFF  (29)
  #define CAPID0_B_OC_ENABLED_SSKU_WID  (1)
  #define CAPID0_B_OC_ENABLED_SSKU_MSK  (0x20000000)
  #define CAPID0_B_OC_ENABLED_SSKU_MAX  (0x1)
  #define CAPID0_B_OC_ENABLED_SSKU_DEF  (0x0)

typedef union {
  struct {
    U64 DDR3L_EN        : 1;  /// Bits 0:0
    U64 DDR_WRTVREF     : 1;  /// Bits 1:1
    U64 OC_ENABLED_DSKU : 1;  /// Bits 2:2
    U64 DDR_OVERCLOCK   : 1;  /// Bits 3:3
    U64 CRID            : 4;  /// Bits 7:4
    U64 CDID            : 2;  /// Bits 9:8
    U64 DIDOE           : 1;  /// Bits 10:10
    U64 IGD             : 1;  /// Bits 11:11
    U64 PDCD            : 1;  /// Bits 12:12
    U64 X2APIC_EN       : 1;  /// Bits 13:13
    U64 DDPCD           : 1;  /// Bits 14:14
    U64 CDD             : 1;  /// Bits 15:15
    U64 FUFRD           : 1;  /// Bits 16:16
    U64 D1NM            : 1;  /// Bits 17:17
    U64 PCIE_RATIO_DIS  : 1;  /// Bits 18:18
    U64 DDRSZ           : 2;  /// Bits 20:19
    U64 PEGG2DIS        : 1;  /// Bits 21:21
    U64 DMIG2DIS        : 1;  /// Bits 22:22
    U64 VTDDD           : 1;  /// Bits 23:23
    U64 FDEE            : 1;  /// Bits 24:24
    U64 ECCDIS          : 1;  /// Bits 25:25
    U64 DW              : 1;  /// Bits 26:26
    U64 PELWUD          : 1;  /// Bits 27:27
    U64 PEG10D          : 1;  /// Bits 28:28
    U64 PEG11D          : 1;  /// Bits 29:29
    U64 PEG12D          : 1;  /// Bits 30:30
    U64 DHDAD           : 1;  /// Bits 31:31
    U64 SPEGFX1         : 1;  /// Bits 32:32
    U64 DPEGFX1         : 1;  /// Bits 33:33
    U64                 : 2;  /// Bits 35:34
    U64 DMFC            : 3;  /// Bits 38:36
    U64 DDD             : 1;  /// Bits 39:39
    U64                 : 3;  /// Bits 42:40
    U64 HDCPD           : 1;  /// Bits 43:43
    U64                 : 4;  /// Bits 47:44
    U64 PEGX16D         : 1;  /// Bits 48:48
    U64 ADDGFXCAP       : 1;  /// Bits 49:49
    U64 ADDGFXEN        : 1;  /// Bits 50:50
    U64 PKGTYP          : 1;  /// Bits 51:51
    U64 PEGG3_DIS       : 1;  /// Bits 52:52
    U64 PLL_REF100_CFG  : 3;  /// Bits 55:53
    U64 SOFTBIN         : 1;  /// Bits 56:56
    U64 CACHESZ         : 3;  /// Bits 59:57
    U64 SMT             : 1;  /// Bits 60:60
    U64 OC_ENABLED_SSKU : 1;  /// Bits 61:61
    U64 OC_CTL_SSKU     : 1;  /// Bits 62:62
    U64                 : 1;  /// Bits 63:63
  } Bits;
  U64 Data;
  struct {
    MRC_PCI_000_CAPID0_A_STRUCT A;
    MRC_PCI_000_CAPID0_B_STRUCT B;
  } Data32;
} MRC_PCI_000_CAPID0_STRUCT;

#pragma pack (pop)
#endif /// _Pci000_h_
