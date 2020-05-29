//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    pccard.h

Abstract:
    Support for PCI 2.2 standard.




Revision History

--*/

#ifndef _EFI_PCCARD_H
#define _EFI_PCCARD_H

//
// Following offset is for PCI configuration space
//
#define PCCARD_SOCKET_REG_ADDRESS_OFFSET    0x10
#define PCCARD_CAPA_POINTER_OFFSET          0x14
#define PCCARD_SECONDARY_STATUS_OFFSET      0x16
#define PCCARD_PCIBUS_NUMBER_OFFSET         0x18
#define PCCARD_CARDBUS_NUMBER_OFFSET        0x19
#define PCCARD_SUBORDINATE_NUMBER_OFFSET    0x1A
#define PCCARD_CARDBUS_LATENCY_TIMER_OFFSET 0x1B
#define PCCARD_MEMORY0_BASE_OFFSET          0x1C
#define PCCARD_MEMORY0_LIMIT_OFFSET         0x20
#define PCCARD_MEMORY1_BASE_OFFSET          0x24
#define PCCARD_MEMORY1_LIMIT_OFFSET         0x28
#define PCCARD_IO0_BASE_OFFSET              0x2C
#define PCCARD_IO0_LIMIT_OFFSET             0x30
#define PCCARD_IO1_BASE_OFFSET              0x34
#define PCCARD_IO1_LIMIT_OFFSET             0x38
#define PCCARD_INTERRUPT_LINE_OFFSET        0x3C
#define PCCARD_INTERRUPT_PIN_OFFSET         0x3D
#define PCCARD_BRIDGE_CONTROL_OFFSET        0x3E
#define PCCARD_SUBSYSTEM_VENDOR_ID_OFFSET   0x40
#define PCCARD_SUBSYSTEM_ID_OFFSET          0x42
#define PCCARD_LEGACY_MODE_BASE_OFFSET      0x44

//
// Following offset is for Memory-mapped I/O space pointed by ExCA Base register
//
#define PCCARD_REG_EVENT_OFFSET   0x00
#define PCCARD_REG_MASK_OFFSET    0x04
#define PCCARD_REG_PRESENT_OFFSET 0x08
#define PCCARD_REG_FORCE_OFFSET   0x0C
#define PCCARD_REG_CONTROL_OFFSET 0x10
#define PCCARD_REG_POWER_OFFSET   0x20

//
// Following is ExCA register offset
//
#define PCCARD_EXCA_ID_REVISION_OFFSET      0x800
#define PCCARD_EXCA_INTERFACE_STATUS_OFFSET 0x801
#define PCCARD_EXCA_POWER_CONTROL_OFFSET    0x802
#define PCCARD_EXCA_INT_GENCTRL_OFFSET      0x803
#define PCCARD_EXCA_STATUS_CHANGE_OFFSET    0x804
#define PCCARD_EXCA_CHANGE_ENABLE_OFFSET    0x805
#define PCCARD_EXCA_WINDOW_ENABLE_OFFSET    0x806
#define PCCARD_EXCA_IOWIN_CONTROL_OFFSET    0x807
#define PCCARD_EXCA_IO_WINDOW0_OFFSET       0x808
#define PCCARD_EXCA_IO_WINDOW1_OFFSET       0x80C
#define PCCARD_EXCA_MEMORY_WINDOW0_OFFSET   0x810
#define PCCARD_EXCA_MEMORY_WINDOW1_OFFSET   0x818
#define PCCARD_EXCA_MEMORY_WINDOW2_OFFSET   0x820
#define PCCARD_EXCA_MEMORY_WINDOW3_OFFSET   0x828
#define PCCARD_EXCA_MEMORY_WINDOW4_OFFSET   0x830
#define PCCARD_EXCA_MEMORY_PAGE0_OFFSET     0x840
#define PCCARD_EXCA_MEMORY_PAGE1_OFFSET     0x841
#define PCCARD_EXCA_MEMORY_PAGE2_OFFSET     0x842
#define PCCARD_EXCA_MEMORY_PAGE3_OFFSET     0x843
#define PCCARD_EXCA_MEMORY_PAGE4_OFFSET     0x844

#define PCCARD_NO_CARD                      0x00
#define PCCARD_16BIT_CARD                   0x01
#define PCCARD_32BIT_CARD                   0x02
#define PCCARD_UNKNOWN_CARD                 0xFF

#pragma pack(1)
//
// Socket register data structure
//
typedef struct {
  UINT32  CSTSCHG : 1;
  UINT32  CCD : 2;
  UINT32  PowerCycle : 1;
  UINT32  Reserved : 28;
} PCCARD_REG_EVENT;

typedef struct {
  UINT32  CSTSCHG : 1;
  UINT32  CardDetect : 2;
  UINT32  PowerCycle : 1;
  UINT32  Reserved : 28;
} PCCARD_REG_MASK;

typedef struct {
  UINT32  CSTSCHG : 1;
  UINT32  CCD : 2;
  UINT32  PowerCycle : 1;
  UINT32  Card16 : 1;
  UINT32  Card32 : 1;
  UINT32  ReadyIrqInt : 1;
  UINT32  NotACard : 1;
  UINT32  DataLost : 1;
  UINT32  BadVcc : 1;
  UINT32  Card5V : 1;
  UINT32  Card3V : 1;
  UINT32  CardXV : 1;
  UINT32  CardYV : 1;
  UINT32  Reserved : 14;  // add zvsupport is 14 or 13
  UINT32  Socket5V : 1;
  UINT32  Socket3V : 1;
  UINT32  SocketXV : 1;
  UINT32  SocketYV : 1;
} PCCARD_REG_PRESENT;

typedef struct {
  UINT32  CSTSCHG : 1;
  UINT32  CCD : 2;
  UINT32  PowerCycle : 1;
  UINT32  Card16 : 1;
  UINT32  Card32 : 1;
  UINT32  Reserved1 : 1;
  UINT32  NotACard : 1;
  UINT32  DataLost : 1;
  UINT32  BadVcc : 1;
  UINT32  Card5V : 1;
  UINT32  Card3V : 1;
  UINT32  CardXV : 1;
  UINT32  CardYV : 1;
  UINT32  CardVSTest : 1;
  UINT32  Reserved2 : 17;
} PCCARD_REG_FORCE;

typedef struct {
  UINT32  Vpp : 3;
  UINT32  Reserved1 : 1;
  UINT32  Vcc : 3;
  UINT32  CLKRUN : 1;
  UINT32  Reserved2 : 24;
} PCCARD_REG_CONTROL;

typedef struct {
  UINT32  ClockCtrl : 1;
  UINT32  Reserved1 : 15;
  UINT32  ClockCtrlEnable : 1;
  UINT32  Reserved2 : 7;
  UINT32  SocketMode : 1;
  UINT32  SocketAccess : 1;
  UINT32  Reserved : 6;
} PCCARD_REG_POWER;

//
// Socket ExCA register data structure
//
typedef struct {
  UINT8 Revision : 4;
  UINT8 Reserved : 2;
  UINT8 InterfaceType : 2;
} PCCARD_EXCA_ID_REVISION;

typedef struct {
  UINT8 BVD : 2;
  UINT8 CD1 : 1;
  UINT8 CD2 : 1;
  UINT8 WP : 1;
  UINT8 Ready : 1;
  UINT8 Power : 1;
  UINT8 Reserved : 1;
} PCCARD_EXCA_INTERFACE_STATUS;

typedef struct {
  UINT8 Vpp : 2;
  UINT8 Reserved1 : 1;
  UINT8 Vcc : 2;
  UINT8 Reserved2 : 2;
  UINT8 OutputEnable : 1;
} PCCARD_EXCA_POWER_CONTROL;

typedef struct {
  UINT8 IntSelect : 4;
  UINT8 CSCRouting : 1;
  UINT8 CardType : 1;
  UINT8 CardReset : 1;
  UINT8 CardRIEnable : 1;
} PCCARD_EXCA_INT_GENCTRL;

typedef struct {
  UINT8 BatteryDead : 1;
  UINT8 BatteryWarning : 1;
  UINT8 Ready : 1;
  UINT8 CardDetect : 1;
  UINT8 Reserved : 4;
} PCCARD_EXCA_STATUS_CHANGE;

typedef struct {
  UINT8 BatteryDead : 1;
  UINT8 BatterWarning : 1;
  UINT8 Ready : 1;
  UINT8 CardDetect : 1;
  UINT8 CardIntRouting : 4;
} PCCARD_EXCA_CHANGE_ENABLE;

typedef struct {
  UINT8 Memory0 : 1;
  UINT8 Memory1 : 1;
  UINT8 Memory2 : 1;
  UINT8 Memory3 : 1;
  UINT8 Memory4 : 1;
  UINT8 Reserved : 1;
  UINT8 IO0 : 1;
  UINT8 IO1 : 1;
} PCCARD_EXCA_WINDOW_ENABLE;

typedef struct {
  UINT8 DataSize0 : 1;
  UINT8 IOIS16Source0 : 1;
  UINT8 ZeroWaitState0 : 1;
  UINT8 WaitState0 : 1;
  UINT8 DataSize1 : 1;
  UINT8 IOIS16Source1 : 1;
  UINT8 ZeroWaitState1 : 1;
  UINT8 WaitState1 : 1;
} PCCARD_EXCA_IOWIN_CONTROL;

typedef struct {
  UINT16  Start;
  UINT16  End;
} PCCARD_EXCA_IO_WINDOW;

typedef struct {
  UINT16  Start : 12;
  UINT16  ScratchPad : 2;
  UINT16  ZeroWaitState : 1;
  UINT16  DataSize : 1;

  UINT16  End : 12;
  UINT16  Reserved : 2;
  UINT16  WaitState : 2;

  UINT16  Offset : 14;
  UINT16  AttributeMap : 1;
  UINT16  WriteProtect : 1;
} PCCARD_EXCA_MEMORY_WINDOW;

typedef struct {
  UINT8 Reserved1 : 1;
  UINT8 RegsOnCardRemove : 1;
  UINT8 Reserved2 : 2;
  UINT8 CDResumeEnable : 1;
  UINT8 SoftCDInterrupt : 1;
  UINT8 VS1 : 1;
  UINT8 VS2 : 1;
} PCCARD_EXCA_CD_GENCTRL;

typedef struct {
  UINT8 PowerDown : 1;
  UINT8 CardIntMode : 1;
  UINT8 IntClearMode : 1;
  UINT8 SocketIntMode : 1;
  UINT8 NoAssigned : 1;
  UINT8 Reserved : 3;
} PCCARD_EXCA_GLOBAL_CONTROL;

typedef UINT16  PCCARD_EXCA_IOWIN_OFFSET;

typedef UINT8   PCCARD_EXCA_MEMORY_PAGE;

#pragma pack()

#endif
