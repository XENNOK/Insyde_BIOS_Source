/** @file

  The contents of this file has all the memory controller register addresses
  and register bit fields for the MRC.
  
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

#ifndef _MC_ADDRESS_H_
#define _MC_ADDRESS_H_

#include <McGdxcbar.h>
#include <McIoCkeCtl.h>
#include <McIoClk.h>
#include <McIoCmd.h>
#include <McIoComp.h>
#include <McIoData.h>
#include <McMain.h>
#include <McScramble.h>
#include <Msa.h>
#include <Pci000.h>

///
/// The following is a copy of M_PCU_CR_SSKPD_PCU_STRUCT, modified to add in the
/// definition of the scratch pad bit fields.
///
typedef union {
  struct {
    U64 OldWM0     : 4;  ///< Bits 3:0
    U64 WM1        : 8;  ///< Bits 11:4
    U64 WM2        : 8;  ///< Bits 19:12
    U64 WM3        : 9;  ///< Bits 28:20
    U64            : 3;  ///< Bits 31:29
    U64 WM4        : 9;  ///< Bits 40:32
    U64            : 15; ///< Bits 55:41
    U64 NewWM0     : 8;  ///< Bits 63:56
  } Bits;
  U64 Data;
  U32 Data32[2];
  U16 Data16[4];
  U8  Data8[8];
} M_PCU_CR_SSKPD_PCU_STRUCT;

///
/// Number of microseconds for level 0 old field (0.1us granularity).
/// 00h 0 us
/// 01h 0.1 us
/// Fh 1.5 us
///
#define PCU_CR_SSKPD_PCU_OLD_WM0_OFF  (0)
#define PCU_CR_SSKPD_PCU_OLD_WM0_WID  (4)
#define PCU_CR_SSKPD_PCU_OLD_WM0_MSK  (0xF)
#define PCU_CR_SSKPD_PCU_OLD_WM0_MAX  (0xF)
#define PCU_CR_SSKPD_PCU_OLD_WM0_DEF  (0xF)

///
/// Number of microseconds for level 0 new field (0.1us granularity).
/// 00h 0 us
/// 01h 0.1 us
/// FFh 25.5 us
///
#define PCU_CR_SSKPD_PCU_NEW_WM0_OFF  (56)
#define PCU_CR_SSKPD_PCU_NEW_WM0_WID  (8)
#define PCU_CR_SSKPD_PCU_NEW_WM0_MSK  (0xFF)
#define PCU_CR_SSKPD_PCU_NEW_WM0_MAX  (0xFF)
#define PCU_CR_SSKPD_PCU_NEW_WM0_DEF  (0x14)
///
/// Number of microseconds for level 1 (0.5us granularity).
/// 00h 0 us
/// 01h 0.5 us
/// FFh 127.5 us
///
#define PCU_CR_SSKPD_PCU_WM1_OFF  (4)
#define PCU_CR_SSKPD_PCU_WM1_WID  (8)
#define PCU_CR_SSKPD_PCU_WM1_MSK  (0xFF)
#define PCU_CR_SSKPD_PCU_WM1_MAX  (0xFF)
#define PCU_CR_SSKPD_PCU_WM1_DEF  (4)

///
/// Number of microseconds for level 2 (0.5us granularity).
/// 00h 0 us
/// 01h 0.5 us
/// FFh 127.5 us
///
#define PCU_CR_SSKPD_PCU_WM2_OFF  (12)
#define PCU_CR_SSKPD_PCU_WM2_WID  (8)
#define PCU_CR_SSKPD_PCU_WM2_MSK  (0xFF)
#define PCU_CR_SSKPD_PCU_WM2_MAX  (0xFF)
#define PCU_CR_SSKPD_PCU_WM2_DEF  (36)

///
/// Number of microseconds for level 3 (0.5us granularity).
/// 00h 0 us
/// 01h 0.5 us
/// 01FFh 255.5 us
///
#define PCU_CR_SSKPD_PCU_WM3_OFF  (20)
#define PCU_CR_SSKPD_PCU_WM3_WID  (9)
#define PCU_CR_SSKPD_PCU_WM3_MSK  (0x1FF)
#define PCU_CR_SSKPD_PCU_WM3_MAX  (0x1FF)
#define PCU_CR_SSKPD_PCU_WM3_DEF  (90)

///
/// Number of microseconds for level 4 (0.5us granularity).
/// 00h 0 us
/// 01h 0.5 us
/// 01FFh 255.5 us
///
#define PCU_CR_SSKPD_PCU_WM4_OFF  (32)
#define PCU_CR_SSKPD_PCU_WM4_WID  (9)
#define PCU_CR_SSKPD_PCU_WM4_MSK  (0x1FF)
#define PCU_CR_SSKPD_PCU_WM4_MAX  (0x1FF)
#define PCU_CR_SSKPD_PCU_WM4_DEF  (160)


#define NCDECS_CR_GDXCBAR_NCU_MAX (0xFFFFF000)

#endif // __McAddress_h__
