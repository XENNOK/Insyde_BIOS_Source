/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*--
Module Name:

  OemCmos.h

Abstract:



--*/
#ifndef _OEM_CMOS_H_
#define _OEM_CMOS_H_

typedef enum {
  OemTable1Reserve48 = 0x48,        // 0x48 ~ 0x4F : OEM Reserve
  OemTable1Reserve6C = 0x6C         // 0x6C ~ 0x7F : OEM Reserve
} OEM_CMOS_TABLE1;

typedef enum {
  OemTable2Reserve00 = 0x00,        // 0x00 ~ 0x0F : OEM Reserve
  OemTable2Reserve40 = 0x40,        // 0x40 ~ 0x57 : OEM Reserve
  OemTable2Reserve60 = 0x60         // 0x60 ~ 0x7F : OEM Reserve
} OEM_CMOS_TABLE2;
#endif
