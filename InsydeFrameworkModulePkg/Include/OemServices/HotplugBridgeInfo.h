//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
#ifndef _HOT_PLUG_BRIDGE_INFO_H_
#define _HOT_PLUG_BRIDGE_INFO_H_

typedef struct {
  UINT8   DevNum;
  UINT8   FuncNum;
  UINT8   ReservedBusCount;
  UINT16  ReservedIoRange;
  UINT64  ReservedNonPrefetchableMmio;
  UINT64  AlignemntOfReservedNonPrefetchableMmio;
  UINT64  ReservedPrefetchableMmio;
  UINT64  AlignemntOfReservedPrefetchableMmio;
} HOT_PLUG_BRIDGE_INFO;

#define IGNORE_DEVICE     0xFE
#define END_OF_TABLE      0xFF

#endif