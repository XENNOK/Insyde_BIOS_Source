//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_LEGACY_MP_TABLE_H_
#define _OEM_LEGACY_MP_TABLE_H_

#include "LegacyBiosMpTable.h"

#define NULL_ENTRY                                0xff

#pragma pack(push, 1)
typedef struct {
  UINT8                             BridgeBus;
  UINT8                             BridgeDev;
  UINT8                             BridgeFunc;
  EFI_LEGACY_MP_TABLE_ENTRY_IO_INT  SlotIrqTable;
} MP_TABLE_ENTRY_IO_INT_SLOT;
#pragma pack(pop)

#endif