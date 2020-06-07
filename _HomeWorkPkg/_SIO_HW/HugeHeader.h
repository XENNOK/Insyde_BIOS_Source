/** @file

 SIO_HW Header

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#ifndef _HUGEHEADER_H_
#define _HUGEHEADER_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/CpuIo2.h>

#define NO_TYPE 255

#define FRAME_MAX_VERT 16
#define FRAME_MAX_HOZI 16

#define HUGE_TITLE_OFFSET   3
#define HUGE_TAIL_OFFSET    25


#define HUGE_FRAME_HORITITLE_OFFSET  4
#define HUGE_FRAME_VERTTITLE_OFFSET  0
#define HUGE_FRAME_HORI_OFFSET   5
#define HUGE_FRAME_VERT_OFFSET   4

#define HUGE_TABLE_HORI_MIN   5
#define HUGE_TABLE_VERT_MIN   5
#define HUGE_TABLE_HORI_MAX   54
#define HUGE_TABLE_VERT_MAX   20

#define HUGE_TABLE_VERT_MAX       20
#define HUGE_TABLE_HORI_BYTE_MAX  51
#define HUGE_TABLE_HORI_WORD_MAX  44
#define HUGE_TABLE_HORI_DWORD_MAX 40 

#define QUANTITY_PER_TYPE(x) ((x)*2)-1

typedef enum _QuantityOfType {
  TypeByte = 1,
  TypeWord = 2,
  TypeDWord = 4
  };

#endif
