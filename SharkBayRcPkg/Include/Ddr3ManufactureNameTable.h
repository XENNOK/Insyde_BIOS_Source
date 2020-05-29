//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _DDR3_MANUFACTURE_NAME_TABLE_H_
#define _DDR3_MANUFACTURE_NAME_TABLE_H_

typedef struct {
  UINT8   ManufactureIdLowByte;
  UINT8   ManufactureIdHighByte;
  CHAR16  *ManufactureName;
} DDR3_MEMORY_MODULE_MANUFACTURE_LIST;

#include "OemDdr3ManufactureNameTable.h"

#define DDR3_MANUFACTURE_NAME_TABLE_DEFINITION \
  {0x00, 0x2C, L"Micron Technology"},\
  {0x00, 0xC1, L"Infineon (Siemens)"},\
  {0x00, 0xCE, L"Samsung"},\
  {0x01, 0x4F, L"Transcend Information"},\
  {0x01, 0x98, L"Kingston"},\
  {0x02, 0xFE, L"ELPIDA"},\
  {0x03, 0x0B, L"Nanya Technology"},\
  {0x03, 0x25, L"Kingmax Semiconductor"},\
  {0x03, 0x83, L"Buffalo (Formerly Melco)"},\
  {0x04, 0xCB, L"A-DATA Technology"},\
  {0x06, 0xC1, L"ASint Technology"},
#endif
