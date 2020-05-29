/** @file

  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*++

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  SMI_Handler.c

Abstract:

  This snipped code contains SMI handler and all related methods called
  directly or underectly by ThunderboltCallback.
  It does not contain registration code for SMI handler.
--*/

#include <ThunderboltSmm.h>

//#ifdef PROGRESS_CODE
//#undef PROGRESS_CODE
//#endif
#define PROGRESS_CODE(Data)         IoWrite8(0x80, Data)

#define SMM_THUNDERBOLT_CALL        0xAA

#define MAX_TBT_DEPTH               6

#define P2P_BRIDGE                  0x0604 // Class:0x06 | Subclass:0x04

#define BAR_ALIGN(v, a)             ((((v) - 1) | (a)) + 1)

#define CMD_BUS_MASTER              BIT2
#define CMD_BM_IO                   (CMD_BUS_MASTER | BIT0)
#define CMD_BM_MEM                  (CMD_BUS_MASTER | BIT1)

#define DEF_CACHE_LINE_SIZE         0x20
#define DEF_RES_IO_PER_DEV          4
#define DEF_RES_MEM_PER_DEV         32
#define DEF_RES_PMEM_PER_DEV        32

#define DOCK_BUSSES                 8

#define DISBL_IO_REG1C              0x01F1
#define DISBL_MEM32_REG20           0x0000FFF0
#define DISBL_PMEM_REG24            0x0001FFF1

//STATIC EFI_SMM_VARIABLE_PROTOCOL*   SmmVariable = 0;
CHIPSET_CONFIGURATION                SysConfig = {0};

STATIC
BOOLEAN
IsTBTHostRouter(
  IN    UINT16  DeviceID
)
{
  switch(DeviceID)
  {
  case 0x1513:
  case 0x151A:
  case 0x151B:
  case 0x1547:
  case 0x1548:
  case 0x1567: 
  case 0x1569:
    return TRUE;
  }
  return FALSE;
}//IsTBTHostRouter

typedef struct _PortInfo
{
  UINT8      IOBase;
  UINT8      IOLimit;
  UINT16     MemBase;
  UINT16     MemLimit;
  UINT16     PMemBase;
  UINT16     PMemLimit;
  UINT32     PMemBaseU;
  UINT32     PMemLimitU;
  UINT8      BusNumLimit;
  UINT8      ConfedEP;
} PortInfo;

typedef struct _MEM_REGS
{
  UINT32 Base;
  UINT32 Limit;
} MEM_REGS;

typedef struct _IO_REGS
{
  UINT16 Base;
  UINT16 Limit;
} IO_REGS;

STATIC VOID
PortInfoInit(
  IN  OUT  PortInfo *pi
)
{
  pi->BusNumLimit = 4;
}//PortInfoInit


#define BUF_LEN (32 * 1024)

STATIC UINT32* PDebugAddress = 0;
STATIC UINT32  PDebugAddressInit = 0;
STATIC UINT8*  PrintCpStart = 0;
STATIC UINT8*  PrintCpEnd = 0;

STATIC UINT32* DebugAddr = 0;

STATIC
VOID
PrintCP(
  IN    UINT8 cp
)
{
  if(!PDebugAddressInit)
  {
    PDebugAddressInit = 1;
    if(!PDebugAddress)
      return;

    PrintCpStart = (UINT8*)(UINTN)(*PDebugAddress);
    DebugAddr = (UINT32*)PrintCpStart;
    PrintCpStart += BUF_LEN;
    PrintCpEnd = PrintCpStart + BUF_LEN;
  }
  if(!PrintCpStart)
    return;

  if(PrintCpStart < PrintCpEnd)
  {
    *PrintCpStart = cp;
    PrintCpStart++;
  }
}//PrintCP

STATIC
VOID
PrintCP16(
  IN    UINT16 cp
)
{
  PrintCP(cp >> 8);
  PrintCP(cp & 0xff);
}//PrintCP16
STATIC
VOID
PrintCP32(
  IN    UINT32 cp
)
{
  PrintCP16(cp >> 16);
  PrintCP16(cp & 0xffff);
}//PrintCP32

STATIC
VOID
PrintCPStr(
  IN    CHAR8* cp
)
{
  for(;cp && *cp;++cp)
    PrintCP(*cp);
  PrintCP('|');
}//PrintCPStr

BOOLEAN isOddBridgeNum = 0;
UINT16 mReserveMemoryPerSlot;
UINT16 mReservePMemoryPerSlot;
UINT8  mReserveIOPerSlot;

#define MEM_PER_SLOT (mReserveMemoryPerSlot * (Depth == 0 && isOddBridgeNum ? 2 : 1))
#define PMEM_PER_SLOT (mReservePMemoryPerSlot * (Depth == 0 && isOddBridgeNum ? 8 : 1))

STATIC
VOID
SetPHYPortResources(
  IN      UINT8 Bus, 
  IN      UINT8 Dev, 
  IN      UINT8 SubBus, 
  IN      INT8  Depth, 
  IN      PortInfo* CurrentPi,
  IN  OUT PortInfo* pi
)
{
  UINT8    Cmd = CMD_BUS_MASTER;
  UINT16   deltaMEM;
  UINT8    deltaIO;

  MmPci8 (0x00, Bus, Dev, 0x00, PCI_SUBUS) = SubBus;
  MmPci8 (0x00, Bus, Dev, 0x00, PCI_CMD) = Cmd;

  deltaIO = pi->IOBase - CurrentPi->IOBase;
  if(Depth >= 0 && mReserveIOPerSlot && deltaIO < mReserveIOPerSlot)
    pi->IOBase += mReserveIOPerSlot - deltaIO;

  if (pi->IOBase > CurrentPi->IOBase && (pi->IOBase - 0x10) <= pi->IOLimit) {
    MmPci8  (0x00, Bus, Dev, 0x00, PCI_IOBASE) = CurrentPi->IOBase;
    MmPci8  (0x00, Bus, Dev, 0x00, PCI_IOLIMIT) = pi->IOBase - 0x10;
    MmPci32 (0x00, Bus, Dev, 0x00, PCI_IOBASE_U) = 0x00000000;
    Cmd |= CMD_BM_IO;
  } else {
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_IOBASE) = DISBL_IO_REG1C;
    pi->IOBase = CurrentPi->IOBase;
  }

  deltaMEM = pi->MemBase - CurrentPi->MemBase;
  if(Depth >= 0 && mReserveMemoryPerSlot && deltaMEM < MEM_PER_SLOT)
    pi->MemBase += MEM_PER_SLOT - deltaMEM;

  if (pi->MemBase > CurrentPi->MemBase && (pi->MemBase - 0x10) <= pi->MemLimit) {
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_MEMBASE) = CurrentPi->MemBase;
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_MEMLIMIT) = pi->MemBase - 0x10;
    Cmd |= CMD_BM_MEM;
  } else {
    MmPci32 (0x00, Bus, Dev, 0x00, PCI_MEMBASE) = DISBL_MEM32_REG20;
    pi->MemBase = CurrentPi->MemBase;
  }

  deltaMEM = pi->PMemBase - CurrentPi->PMemBase;
  if(Depth >= 0 && mReservePMemoryPerSlot && deltaMEM < PMEM_PER_SLOT)
    pi->PMemBase += PMEM_PER_SLOT - deltaMEM;

  if (pi->PMemBase > CurrentPi->PMemBase && (pi->PMemBase - 0x10) <= pi->PMemLimit) {
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMBASE) = CurrentPi->PMemBase;
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMLIMIT) = pi->PMemBase - 0x10;
//[-start-120518-IB03780439-add]//
    MmPci32 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMBASE_U) = 0x00000000;
    MmPci32 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMLIMIT_U) = 0x00000000;
//[-end-120518-IB03780439-add]//
    Cmd |= CMD_BM_MEM;
  } else {
    MmPci32 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMBASE) = DISBL_PMEM_REG24;
    pi->PMemBase = CurrentPi->PMemBase;
  }

  MmPci8  (0x00, Bus, Dev, 0x00, PCI_CMD) = Cmd;
  MmPci8  (0x00, Bus, Dev, 0x00, PCI_CLS) = SysConfig.TBTCacheLineSize;
}//SetPHYPortResources


STATIC
UINT32
SaveSetGetRestoreBAR(
  IN    volatile UINT32* BAR
)
{
  UINT32  BarReq;
  UINT32  OrigBar = *BAR; // Save BAR
  *BAR = 0xFFFFFFFF;      // Set BAR
  BarReq = *BAR;          // Get BAR
  *BAR = OrigBar;         // Restore BAR
  
  return BarReq;
}//SaveSetGetRestoreBAR

STATIC
VOID
SetIOBAR(
  IN    volatile
            UINT32*  BAR,
  IN        UINT32   BarReq,
  IN  OUT   UINT8*   Cmd,
  IN  OUT   IO_REGS* io_r
)
{
  UINT16 Alignment = ~(BarReq & 0xFFFC);
  UINT16 Size = Alignment + 1;
  UINT16 NewBase;

  if(io_r->Base > io_r->Limit || !Size)
    return;

  NewBase = BAR_ALIGN(io_r->Base, Alignment);
  if(NewBase > io_r->Limit || NewBase + Size - 1 > io_r->Limit)
    return;

  *BAR = NewBase; // Set BAR
  io_r->Base = NewBase + Size; // Advance to new position
  *Cmd |= CMD_BM_IO; // Set IO Space Enable
}//SetIOBAR

STATIC
VOID
SetMemBAR(
  IN    volatile 
            UINT32*   BAR,
  IN        UINT32    BarReq,
  IN  OUT   UINT8*    Cmd,
  IN  OUT   MEM_REGS* mem_r
)
{
  UINT32 Alignment = ~(BarReq & 0xFFFFFFF0);
  UINT32 Size = Alignment + 1;
  UINT32 NewBase;

  if(mem_r->Base > mem_r->Limit || !Size)
    return;

  NewBase = BAR_ALIGN(mem_r->Base, Alignment);
  if(NewBase > mem_r->Limit || NewBase + Size - 1 > mem_r->Limit)
    return;

  *BAR = NewBase; // Set BAR
  mem_r->Base = NewBase + Size; // Advance to new position
  *Cmd |= CMD_BM_MEM; // Set Memory Space Enable
}//SetMemBAR


STATIC
VOID
SetDevResources(
  IN        UINT8      Bus,
  IN        UINT8      Dev,
  IN        UINT8      FUNC_MAX, // PCI_MAX_FUNC for devices, 1 for bridge
  IN        UINT8      BAR_MAX, // PCI_BAR5 for devices, PCI_BAR1 for bridge
  IN  OUT   PortInfo   *pi
)
{
  UINT8       Fun;
  UINT8       Reg;
  UINT32      BarReq;
  IO_REGS IO;
  MEM_REGS Mem;
  MEM_REGS PMem;

  IO.Base = pi->IOBase << 8;
  IO.Limit = (pi->IOLimit << 8) | 0xFF;
  Mem.Base = pi->MemBase << 16;
  Mem.Limit = (pi->MemLimit << 16) | 0xFFFF;
  PMem.Base = pi->PMemBase << 16;
  PMem.Limit = (pi->PMemLimit << 16) | 0xFFFF;

  for (Fun = 0; Fun < FUNC_MAX; ++Fun)
  {
    UINT8 Cmd = MmPci8  (0x00, Bus, Dev, Fun, PCI_CMD) = CMD_BUS_MASTER;
    if (0xFFFF == MmPci16 (0x00, Bus, Dev, Fun, PCI_DID))
      continue;

    for (Reg = PCI_BAR0; Reg <= BAR_MAX; Reg += 4) {
      BarReq = SaveSetGetRestoreBAR(MmPci32Ptr (0x00, Bus, Dev, Fun, Reg));// Perform BAR sizing

      if (BarReq & BIT0) // I/O BAR
      {
        SetIOBAR(MmPci32Ptr(0x00, Bus, Dev, Fun, Reg),
          BarReq, &Cmd, &IO);
        continue;
      }

      SetMemBAR(MmPci32Ptr(0x00, Bus, Dev, Fun, Reg), BarReq, &Cmd, 
        (BarReq & BIT3) ? &PMem : &Mem);

      if (BIT2 == (BarReq & (BIT2 | BIT1))) // Base address is 64 bits wide
      {
        Reg += 4;
        MmPci32 (0x00, Bus, Dev, Fun, Reg) = 0; // Allocate from 32 bit space
      }
    }

//[-start-130513-IB10300037-remove]//
//		if(Cmd & BIT1) // If device uses I/O and MEM mapping use only MEM mepping
//			Cmd &= ~BIT0;
//[-end-130513-IB10300037-remove]//

    MmPci8  (0x00, Bus, Dev, Fun, PCI_CMD) = Cmd;
    MmPci8  (0x00, Bus, Dev, Fun, PCI_CLS) = SysConfig.TBTCacheLineSize;
  }// Fun < PCI_MAX_FUNC
  
  // Update pi if any changes
  if (IO.Base > ((UINT32)pi->IOBase << 8))
    pi->IOBase = (UINT8)(BAR_ALIGN(IO.Base, 0xFFF) >> 8);

  if (Mem.Base > ((UINT32)pi->MemBase << 16))
    pi->MemBase = (UINT16)(BAR_ALIGN(Mem.Base, 0xFFFFF) >> 16);

  if (PMem.Base > ((UINT32)pi->PMemBase << 16))
    pi->PMemBase = (UINT16)(BAR_ALIGN(PMem.Base, 0xFFFFF) >> 16);
}// SetDevResources

typedef struct _DEV_ID
{
  UINT8     Bus;
  UINT8     Dev;
  UINT8     Fun;
} DEV_ID;

STATIC
DEV_ID HR_Slots[] = 
{
  {0x00, 0x1C, 0x00},// PCH slot 0
  {0x00, 0x1C, 0x01},// PCH slot 1
  {0x00, 0x1C, 0x02},// PCH slot 2
  {0x00, 0x1C, 0x03},// PCH slot 3
  {0x00, 0x1C, 0x04},// PCH slot 4
  {0x00, 0x1C, 0x05},// PCH slot 5
  {0x00, 0x1C, 0x06},// PCH slot 6
  {0x00, 0x1C, 0x07},// PCH slot 7
  {0x00, 0x01, 0x00},// PEG slot
};

#define count(x) (sizeof(x) / sizeof((x)[0]))

typedef struct _BRDG_RES_CONFIG
{
  UINT8    Cmd;
  UINT8    Cls;
  UINT8    IOBase;
  UINT8    IOLimit;
  UINT16   MemBase;
  UINT16   MemLimit;
  UINT16   PMemBase;
  UINT16   PMemLimit;
  UINT32   PMemBaseU;
  UINT32   PMemLimitU;
} BRDG_RES_CONFIG;

STATIC const
BRDG_RES_CONFIG NOT_IN_USE_BRIDGE = 
{
  CMD_BUS_MASTER,
  0,
  DISBL_IO_REG1C & 0xFF,
  DISBL_IO_REG1C >> 8,
  DISBL_MEM32_REG20 & 0xFFFF,
  DISBL_MEM32_REG20 >> 16,
  DISBL_PMEM_REG24 & 0xFFFF,
  DISBL_PMEM_REG24 >> 16,
  0,
  0
};

typedef struct _BRDG_CONFIG
{
  DEV_ID           DevId;
  UINT8            PBus;
  UINT8            SBus;
  UINT8            SubBus;
  BRDG_RES_CONFIG  Res;
} BRDG_CONFIG;

enum {
HR_US_PORT, 
HR_DS_PORT0,
HR_DS_PORT3,
HR_DS_PORT4,
HR_DS_PORT5,
HR_DS_PORT6,
MAX_CFG_PORTS
};

STATIC
BRDG_CONFIG HRConfigs[MAX_CFG_PORTS];// US(X:0:0), DS(X+1:3:0),DS(X+1:4:0),DS(X+1:5:0),DS(X+1:6:0)

STATIC
VOID
InitCommonHRConfigs(
  IN        UINT8 HRBus,
  IN        UINT8 BusNumLimit,
  IN  OUT   BRDG_RES_CONFIG* HRResConf
)
{
  UINT8 i,j;
  // US(HRBus:0:0)
  HRConfigs[HR_US_PORT].DevId.Bus = HRBus;
  HRConfigs[HR_US_PORT].DevId.Dev = 0;
  HRConfigs[HR_US_PORT].DevId.Fun = 0;
  HRConfigs[HR_US_PORT].Res = *HRResConf;
//[-start-130513-IB10300037-remove]//
//	HRConfigs[HR_US_PORT].Res.IOBase = 0xF1;
//	HRConfigs[HR_US_PORT].Res.IOLimit = 0x01;
//[-end-130513-IB10300037-remove]//;
  HRConfigs[HR_US_PORT].PBus = HRConfigs[HR_US_PORT].DevId.Bus;
  HRConfigs[HR_US_PORT].SBus = HRConfigs[HR_US_PORT].PBus + 1;
  HRConfigs[HR_US_PORT].SubBus = BusNumLimit;

  // HIA resides here
  HRConfigs[HR_DS_PORT0].DevId.Bus = HRConfigs[HR_US_PORT].DevId.Bus + 1;
  HRConfigs[HR_DS_PORT0].DevId.Dev = 0;
  HRConfigs[HR_DS_PORT0].DevId.Fun = 0;
  HRConfigs[HR_DS_PORT0].Res = NOT_IN_USE_BRIDGE;
  HRConfigs[HR_DS_PORT0].Res.MemBase = HRResConf->MemLimit;
  HRConfigs[HR_DS_PORT0].Res.MemLimit = HRResConf->MemLimit;
  HRResConf->MemLimit -= 0x10; //This 1 MB chunk will be used by HIA
  HRConfigs[HR_DS_PORT0].Res.Cmd = CMD_BM_MEM;
  HRConfigs[HR_DS_PORT0].Res.Cls = SysConfig.TBTCacheLineSize;
  HRConfigs[HR_DS_PORT0].PBus = HRConfigs[HR_DS_PORT0].DevId.Bus;
  HRConfigs[HR_DS_PORT0].SBus = HRConfigs[HR_DS_PORT0].PBus + 1;
  HRConfigs[HR_DS_PORT0].SubBus = HRConfigs[HR_DS_PORT0].PBus + 1;

  // DS(HRBus+2:3-6:0)
  for(j = 2, i = 3; j < count(HRConfigs) && i <= 6; ++j, ++i) {
    HRConfigs[j].DevId.Bus = HRConfigs[HR_US_PORT].DevId.Bus + 1;
    HRConfigs[j].DevId.Dev = i;
    HRConfigs[j].DevId.Fun = 0;
    HRConfigs[j].PBus = HRConfigs[j].DevId.Bus;
    HRConfigs[j].Res.Cls = SysConfig.TBTCacheLineSize;
  }
}//InitCommonHRConfigs

STATIC
VOID
InitHRDSPort_Disable(
  IN        UINT8 id,
  IN  OUT   BRDG_CONFIG* BrdgConf
)
{
  HRConfigs[id].Res = NOT_IN_USE_BRIDGE;
  HRConfigs[id].SBus = BrdgConf->SBus;
  HRConfigs[id].SubBus = BrdgConf->SBus;

  BrdgConf->SBus++;
}//InitHRDSPort_Disable


STATIC
VOID
InitHRDSPort_1Port(
  IN  OUT  BRDG_CONFIG* BrdgConf
)
{
  UINT16 MemBase = BrdgConf->Res.MemBase & 0xFFF0;
  UINT16 PMemBase = BrdgConf->Res.PMemBase & 0xFFF0;
  UINT8  BusRange = BrdgConf->SubBus - BrdgConf->PBus - (MAX_CFG_PORTS - 2); // MAX_CFG_PORTS-1(US)-1(HIA) is num of bridges in HR, on each bridge bus# is incremented
  BusRange -= DOCK_BUSSES; // Bus range for Dock port

  HRConfigs[HR_DS_PORT3].Res = NOT_IN_USE_BRIDGE;

//[-start-130513-IB10300037-modify]//
	HRConfigs[HR_DS_PORT3].Res.Cmd = CMD_BM_IO | CMD_BM_MEM;
	//HRConfigs[HR_DS_PORT3].Res.Cmd = CMD_BM_MEM;
//[-end-130513-IB10300037-modify]//
	HRConfigs[HR_DS_PORT3].Res.Cls = SysConfig.TBTCacheLineSize;
//[-start-130513-IB10300037-add]//
	HRConfigs[HR_DS_PORT3].Res.IOBase= BrdgConf->Res.IOBase;
	HRConfigs[HR_DS_PORT3].Res.IOLimit= BrdgConf->Res.IOLimit;
//[-end-130513-IB10300037-add]//
  HRConfigs[HR_DS_PORT3].Res.MemBase = MemBase;
  HRConfigs[HR_DS_PORT3].Res.MemLimit = MemBase + 0xE00 - 1;
  HRConfigs[HR_DS_PORT3].Res.PMemBase = PMemBase;
  HRConfigs[HR_DS_PORT3].Res.PMemLimit = PMemBase + 0x1A00 - 1;
  HRConfigs[HR_DS_PORT3].SBus = BrdgConf->SBus;
  HRConfigs[HR_DS_PORT3].SubBus = BrdgConf->SBus + BusRange;

  BrdgConf->SBus = HRConfigs[HR_DS_PORT3].SubBus + 1;

  HRConfigs[HR_DS_PORT4].Res = NOT_IN_USE_BRIDGE;
  HRConfigs[HR_DS_PORT4].Res.Cmd = CMD_BM_MEM;
  HRConfigs[HR_DS_PORT4].Res.Cls = SysConfig.TBTCacheLineSize;
  HRConfigs[HR_DS_PORT4].Res.MemBase = MemBase + 0xE00;
  HRConfigs[HR_DS_PORT4].Res.MemLimit = MemBase + 0x1600 - 1;
  HRConfigs[HR_DS_PORT4].Res.PMemBase = PMemBase + 0x1A00;
  HRConfigs[HR_DS_PORT4].Res.PMemLimit = PMemBase + 0x2200 - 1;
  HRConfigs[HR_DS_PORT4].SBus = BrdgConf->SBus;
  HRConfigs[HR_DS_PORT4].SubBus = BrdgConf->SBus + DOCK_BUSSES;

  BrdgConf->SBus = HRConfigs[HR_DS_PORT4].SubBus + 1;
}//InitHRDSPort_1Port


STATIC
VOID
InitHRDSPort_2Port(
  IN  OUT   BRDG_CONFIG* BrdgConf
)
{
  UINT16 MemBase = BrdgConf->Res.MemBase & 0xFFF0;
  UINT16 PMemBase = BrdgConf->Res.PMemBase & 0xFFF0;
  UINT8  BusRange = BrdgConf->SubBus - BrdgConf->PBus - (MAX_CFG_PORTS - 2); // MAX_CFG_PORTS-1(US)-1(HIA) is num of bridges in HR, on each bridge bus# is incremented

  BusRange -= 2 * DOCK_BUSSES; // Bus range for Dock ports
  // Rest of busses split between ports 3 and 5
  BusRange /= 2; // Bus range for port 3/5

  HRConfigs[HR_DS_PORT3].Res = NOT_IN_USE_BRIDGE;

  HRConfigs[HR_DS_PORT3].Res.Cmd = CMD_BM_MEM;
  HRConfigs[HR_DS_PORT3].Res.Cls = SysConfig.TBTCacheLineSize;
  HRConfigs[HR_DS_PORT3].Res.MemBase = MemBase;
  HRConfigs[HR_DS_PORT3].Res.MemLimit = MemBase + 0x1000 - 1;
  HRConfigs[HR_DS_PORT3].Res.PMemBase = PMemBase;
  HRConfigs[HR_DS_PORT3].Res.PMemLimit = PMemBase + 0x2000 - 1;
  HRConfigs[HR_DS_PORT3].SBus = BrdgConf->SBus;
  HRConfigs[HR_DS_PORT3].SubBus = BrdgConf->SBus + BusRange;

  BrdgConf->SBus = HRConfigs[HR_DS_PORT3].SubBus + 1;

  HRConfigs[HR_DS_PORT4].Res = NOT_IN_USE_BRIDGE;
  HRConfigs[HR_DS_PORT4].Res.Cmd = CMD_BM_MEM;
  HRConfigs[HR_DS_PORT4].Res.Cls = SysConfig.TBTCacheLineSize;
  HRConfigs[HR_DS_PORT4].Res.MemBase = MemBase + 0x1000;
  HRConfigs[HR_DS_PORT4].Res.MemLimit = MemBase + 0x1800 - 1;
  HRConfigs[HR_DS_PORT4].Res.PMemBase = PMemBase + 0x2000;
  HRConfigs[HR_DS_PORT4].Res.PMemLimit = PMemBase + 0x2800 - 1;
  HRConfigs[HR_DS_PORT4].SBus = BrdgConf->SBus;
  HRConfigs[HR_DS_PORT4].SubBus = BrdgConf->SBus + DOCK_BUSSES;

  BrdgConf->SBus = HRConfigs[HR_DS_PORT4].SubBus + 1;

  HRConfigs[HR_DS_PORT5].Res = NOT_IN_USE_BRIDGE;
  HRConfigs[HR_DS_PORT5].Res.Cmd = CMD_BM_MEM;
  HRConfigs[HR_DS_PORT5].Res.Cls = SysConfig.TBTCacheLineSize;
  HRConfigs[HR_DS_PORT5].Res.MemBase = MemBase + 0x1800;
  HRConfigs[HR_DS_PORT5].Res.MemLimit = MemBase + 0x2600 - 1;
//[-start-130123-IB04770265-modify]//
  HRConfigs[HR_DS_PORT5].Res.PMemBase = PMemBase + 0x3000;
  HRConfigs[HR_DS_PORT5].Res.PMemLimit = PMemBase + 0x4A00 - 1;
//[-end-130123-IB04770265-modify]//
  HRConfigs[HR_DS_PORT5].SBus = BrdgConf->SBus;
  HRConfigs[HR_DS_PORT5].SubBus = BrdgConf->SBus + BusRange;

  BrdgConf->SBus = HRConfigs[HR_DS_PORT5].SubBus + 1;

  HRConfigs[HR_DS_PORT6].Res = NOT_IN_USE_BRIDGE;
  HRConfigs[HR_DS_PORT6].Res.Cmd = CMD_BM_MEM;
  HRConfigs[HR_DS_PORT6].Res.Cls = SysConfig.TBTCacheLineSize;
  HRConfigs[HR_DS_PORT6].Res.MemBase = MemBase + 0x2600;
  HRConfigs[HR_DS_PORT6].Res.MemLimit = MemBase + 0x2E00 - 1;
//[-start-130123-IB04770265-modify]//
  HRConfigs[HR_DS_PORT6].Res.PMemBase = PMemBase + 0x2800;
  HRConfigs[HR_DS_PORT6].Res.PMemLimit = PMemBase + 0x3000 - 1;
//[-end-130123-IB04770265-modify]//
  HRConfigs[HR_DS_PORT6].SBus = BrdgConf->SBus;
  HRConfigs[HR_DS_PORT6].SubBus = BrdgConf->SBus + DOCK_BUSSES;

  BrdgConf->SBus = HRConfigs[HR_DS_PORT6].SubBus + 1;
}//InitHRDSPort_2Port

STATIC
BOOLEAN
CheckLimits(
  IN    BOOLEAN         Is2PortDev,
  IN    BRDG_RES_CONFIG *HRResConf,
  IN    UINT8           BusRange
)
{
  UINT16 MemBase = HRResConf->MemBase & 0xFFF0;
  UINT16 MemLimit = HRResConf->MemLimit & 0xFFF0;
  UINT16 PMemBase = HRResConf->PMemBase & 0xFFF0;
  UINT16 PMemLimit = HRResConf->PMemLimit & 0xFFF0;

  // Check memoty alignment
  if(MemBase & 0x3FF) {
    PrintCPStr("M alig");
    return FALSE;
  }
  if(PMemBase & 0xFFF) {
    PrintCPStr("PM alig");
    return FALSE;
  }

  if(Is2PortDev) {
    // Check mem size
    if(MemLimit + 0x10 - MemBase < 0x2E00) {
      PrintCPStr("M size");
      return FALSE;
    }
    // Check P-mem size
    if(PMemLimit + 0x10 - PMemBase < 0x4A00) {
      PrintCPStr("PM size");
      return FALSE;
    }
    // Check bus range
    if(BusRange < 106) {
      PrintCPStr("Bus range");
      return FALSE;
    }
  } else {
    if(MemLimit + 0x10 - MemBase < 0x1600) {
      PrintCPStr("M size");
      return FALSE;
    }
    // Check P-mem size
    if(PMemLimit + 0x10 - PMemBase < 0x2200) {
      PrintCPStr("PM size");
      return FALSE;
    }
    // Check bus range
    if(BusRange < 56) {
      PrintCPStr("Bus range");
      return FALSE;
    }
  }



  return TRUE;
}//CheckLimits


STATIC
BOOLEAN
InitHRResConfigs(
  IN        UINT16 DeviceId,
  IN        UINT8 HRBus,
  IN        UINT8 BusNumLimit,
  IN  OUT   BRDG_RES_CONFIG* HRResConf
)
{
  BRDG_CONFIG BrdgConf = {0};
  InitCommonHRConfigs(HRBus, BusNumLimit, HRResConf);
  BrdgConf.PBus = HRBus + 2;
  BrdgConf.SBus = HRBus + 3;
  BrdgConf.SubBus = BusNumLimit;
  BrdgConf.Res = *HRResConf;
  while(TRUE)
  switch(DeviceId) {
  case 0x1547: case 0x1569: // 2 Port host
    if(CheckLimits(TRUE, HRResConf, BusNumLimit - HRBus)) {
      InitHRDSPort_2Port(&BrdgConf);
      return TRUE;
    } else {
      DeviceId = 0; // Jump to default on next loop
      continue;
    }
  case 0x1548: case 0x1567: // 1 Port host
    if(CheckLimits(FALSE, HRResConf, BusNumLimit - HRBus))
    {
      InitHRDSPort_1Port(&BrdgConf);
      InitHRDSPort_Disable(HR_DS_PORT5, &BrdgConf);
      InitHRDSPort_Disable(HR_DS_PORT6, &BrdgConf);
      return TRUE;
    }
  default:
    InitHRDSPort_Disable(HR_DS_PORT3, &BrdgConf);
    InitHRDSPort_Disable(HR_DS_PORT4, &BrdgConf);
    InitHRDSPort_Disable(HR_DS_PORT5, &BrdgConf);
    InitHRDSPort_Disable(HR_DS_PORT6, &BrdgConf);
    return FALSE;
  }
}//InitHRResConfigs


STATIC
BOOLEAN
InitializeHostRouter(
    OUT  UINT8  *HR_Bus
)
{
  UINT16            DeviceId = 0xFFFF;
  UINT8             BusNumLimit;
  BRDG_RES_CONFIG   HRResConf = {0};
  UINT8             i;
  BOOLEAN           Ret;

  for(i = 0; i < count(HR_Slots); ++i) {
    *HR_Bus  = MmPci8  (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_SBUS);
    DeviceId = MmPci16 (0x00, *HR_Bus, 0x00, 0x00, PCI_DID);
    if (IsTBTHostRouter(DeviceId))
      break;
  }

  if(i >= count(HR_Slots))
    return FALSE;

//[-start-130513-IB10300037-add]//
  {
    UINT8 TbtCheckBus;
    UINT8 TbtPBus;
    UINT8 TbtSBus;
    UINT8 TbtSuBus;
    

  	TbtCheckBus         = MmPci8  (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_SBUS);
    TbtPBus             = MmPci8  (0x00, TbtCheckBus, 0, 0, PCI_PBUS);
    TbtSBus             = MmPci8  (0x00, TbtCheckBus, 0, 0, PCI_SBUS);
    TbtSuBus            = MmPci8  (0x00, TbtCheckBus, 0, 0, PCI_SUBUS);

    //
    // If Tbt Already initiallized, skip tbt smi handle
    //
    if ((TbtPBus == TbtCheckBus) && (TbtPBus != 0) && (TbtSBus != 0) && (TbtSuBus != 0)) {
  		return FALSE;
    }
  }
//[-end-130513-IB10300037-add]//

	//HRResConf.Cmd         = CMD_BM_MEM;
	HRResConf.Cmd         = CMD_BM_IO | CMD_BM_MEM;
  HRResConf.Cls         = SysConfig.TBTCacheLineSize;
  HRResConf.IOBase      = MmPci8  (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_IOBASE);
  HRResConf.IOLimit     = MmPci8  (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_IOLIMIT);
  HRResConf.MemBase     = MmPci16 (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_MEMBASE);
  HRResConf.MemLimit    = MmPci16 (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_MEMLIMIT);
  HRResConf.PMemBase    = MmPci16 (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_PRE_MEMBASE);
  HRResConf.PMemLimit   = MmPci16 (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_PRE_MEMLIMIT);
  HRResConf.PMemBaseU   = MmPci32 (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_PRE_MEMBASE_U);
  HRResConf.PMemLimitU  = MmPci32 (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_PRE_MEMLIMIT_U);
  BusNumLimit           = MmPci8  (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_SUBUS);

  Ret = InitHRResConfigs(DeviceId, *HR_Bus, BusNumLimit, &HRResConf);

  for(i = 0; i < count(HRConfigs); ++i) {
    UINT8 Bus = HRConfigs[i].DevId.Bus;
    UINT8 Dev = HRConfigs[i].DevId.Dev;
    UINT8 Fun = HRConfigs[i].DevId.Fun;

    MmPci8  (0x00, Bus, Dev, Fun, PCI_CLS) = HRConfigs[i].Res.Cls;
    MmPci8  (0x00, Bus, Dev, Fun, PCI_PBUS) = HRConfigs[i].PBus;
    MmPci8  (0x00, Bus, Dev, Fun, PCI_SBUS) = HRConfigs[i].SBus;
    MmPci8  (0x00, Bus, Dev, Fun, PCI_SUBUS) = HRConfigs[i].SubBus;
    MmPci16 (0x00, Bus, Dev, Fun, PCI_MEMBASE) = HRConfigs[i].Res.MemBase;
    MmPci16 (0x00, Bus, Dev, Fun, PCI_MEMLIMIT) = HRConfigs[i].Res.MemLimit;
    MmPci16 (0x00, Bus, Dev, Fun, PCI_PRE_MEMBASE) = HRConfigs[i].Res.PMemBase;
    MmPci16 (0x00, Bus, Dev, Fun, PCI_PRE_MEMLIMIT) = HRConfigs[i].Res.PMemLimit;
    MmPci32 (0x00, Bus, Dev, Fun, PCI_PRE_MEMBASE_U) = HRConfigs[i].Res.PMemBaseU;
    MmPci32 (0x00, Bus, Dev, Fun, PCI_PRE_MEMLIMIT_U) = HRConfigs[i].Res.PMemLimitU;
    MmPci8  (0x00, Bus, Dev, Fun, PCI_IOBASE) = HRConfigs[i].Res.IOBase;
    MmPci8  (0x00, Bus, Dev, Fun, PCI_IOLIMIT) = HRConfigs[i].Res.IOLimit;
    MmPci32 (0x00, Bus, Dev, Fun, PCI_IOBASE_U) = 0x00000000;
    MmPci8  (0x00, Bus, Dev, Fun, PCI_CMD) = HRConfigs[i].Res.Cmd;
  }

  MmPci32 (0x00, (*HR_Bus + 2), 0x00, 0x00, PCI_BAR0) = HRConfigs[HR_DS_PORT0].Res.MemLimit << 16;
  MmPci32 (0x00, (*HR_Bus + 2), 0x00, 0x00, PCI_BAR1) = (HRConfigs[HR_DS_PORT0].Res.MemLimit + 0x4) << 16;
  MmPci8  (0x00, (*HR_Bus + 2), 0x00, 0x00, PCI_CLS) = SysConfig.TBTCacheLineSize;
  MmPci8  (0x00, (*HR_Bus + 2), 0x00, 0x00, PCI_CMD) = CMD_BM_MEM;
  
  return Ret;
}//InitializeHostRouter

STATIC
UINT8
ConfigureSlot(
  IN        UINT8     Bus,
  IN        UINT8     MAX_DEVICE,
  IN        INT8    Depth, 
  IN  OUT   PortInfo* pi
)
{
  UINT8    Device;
  UINT8    SBus;
  UINT8    UsedBusNumbers;
  UINT8    RetBusNum = 0;
  PortInfo CurrentSlot;

  for (Device = 0; Device < MAX_DEVICE; Device++)
  {
    // Continue if device is absent
    if (0xFFFF == MmPci16 (0x00, Bus, Device, 0x00, PCI_DID))
      continue;

    if (P2P_BRIDGE != MmPci16 (0x00, Bus, Device, 0x00, PCI_SCC)) {
      SetDevResources(Bus, Device,
        PCI_MAX_FUNC, PCI_BAR5, pi);
      continue;
    }
    // Else Bridge

    CurrentSlot = *pi; // Save before update

    ++RetBusNum; // UP Bridge
    SBus = Bus + RetBusNum; // DS Bridge

    if (SBus + 1 >= pi->BusNumLimit)
      continue;

    SetDevResources(Bus, Device, 1, PCI_BAR1, pi);

    // Init UP Bridge to reach DS Bridge
    MmPci8 (0x00, Bus, Device, 0x00, PCI_PBUS) = Bus;
    MmPci8 (0x00, Bus, Device, 0x00, PCI_SBUS) = SBus;
    MmPci8 (0x00, Bus, Device, 0x00, PCI_SUBUS) = pi->BusNumLimit;// Just in case
    MmPci8 (0x00, Bus, Device, 0x00, PCI_CMD) = CMD_BM_MEM;

    UsedBusNumbers = ConfigureSlot(SBus, PCI_MAX_DEVICE + 1, -1, pi);

    RetBusNum += UsedBusNumbers;

    SetPHYPortResources(Bus, Device,
      SBus + UsedBusNumbers, Depth,
      &CurrentSlot, pi);
  }//for (Device = 0; Device <= PCI_MAX_DEVICE; Device++)
  return RetBusNum;
}// ConfigureSlot


STATIC
VOID
SetCIOPortResources(
  IN        UINT8 Bus, 
  IN        UINT8 Dev,
  IN        UINT8 SBus,
  IN        UINT8 SubBus,
  IN        PortInfo* portInfoBeforeChange, 
  IN  OUT   PortInfo* pi
)
{
  UINT8    Cmd = CMD_BUS_MASTER;

  MmPci8  (0x00, Bus, Dev, 0x00, PCI_PBUS) = Bus;
  MmPci8  (0x00, Bus, Dev, 0x00, PCI_SBUS) = SBus;
  MmPci8  (0x00, Bus, Dev, 0x00, PCI_SUBUS) = SubBus;
  MmPci8  (0x00, Bus, Dev, 0x00, PCI_CMD) = Cmd;

  if (pi->IOBase <= pi->IOLimit) {
    MmPci8  (0x00, Bus, Dev, 0x00, PCI_IOBASE) = pi->IOBase;
    MmPci8  (0x00, Bus, Dev, 0x00, PCI_IOLIMIT) = pi->IOLimit;
    MmPci32 (0x00, Bus, Dev, 0x00, PCI_IOBASE_U) = 0x00000000;
    Cmd |= CMD_BM_IO;
  } else {
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_IOBASE) = DISBL_IO_REG1C;
  }

  if (pi->MemBase <= pi->MemLimit) {
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_MEMBASE) = pi->MemBase;
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_MEMLIMIT) = pi->MemLimit;
    Cmd |= CMD_BM_MEM;
  } else {
    MmPci32 (0x00, Bus, Dev, 0x00, PCI_MEMBASE) = DISBL_MEM32_REG20;
  }

  if (pi->PMemBase <= pi->PMemLimit) {
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMBASE) = pi->PMemBase;
    MmPci16 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMLIMIT) = pi->PMemLimit;
    Cmd |= CMD_BM_MEM;
  } else {
    MmPci32 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMBASE) = DISBL_PMEM_REG24;
  }

  MmPci32 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMBASE_U) = pi->PMemBaseU;
  MmPci32 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMLIMIT_U) = pi->PMemLimitU;
  MmPci8  (0x00, Bus, Dev, 0x00, PCI_CMD) = Cmd;
  MmPci8  (0x00, Bus, Dev, 0x00, PCI_CLS) = SysConfig.TBTCacheLineSize;
}//SetCIOPortResources

STATIC
VOID
SetSlotsAsUnused(
  IN        UINT8 Bus,
  IN        UINT8 MaxSlotNum,
  IN        UINT8 CIOSlot,
  IN  OUT   PortInfo* pi
)
{
  UINT8 Slot;
  for (Slot = MaxSlotNum; Slot > CIOSlot; --Slot) {
    if (0xFFFF == MmPci16 (0x00, Bus, Slot, 0x00, PCI_DID))
      continue;

    MmPci8  (0x00, Bus, Slot, 0x00, PCI_CLS) = SysConfig.TBTCacheLineSize;
    MmPci8  (0x00, Bus, Slot, 0x00, PCI_PBUS) = Bus;
    MmPci8  (0x00, Bus, Slot, 0x00, PCI_SBUS) = pi->BusNumLimit;
    MmPci8  (0x00, Bus, Slot, 0x00, PCI_SUBUS) = pi->BusNumLimit;
    MmPci16 (0x00, Bus, Slot, 0x00, PCI_IOBASE) = DISBL_IO_REG1C;
    MmPci32 (0x00, Bus, Slot, 0x00, PCI_MEMBASE) = DISBL_MEM32_REG20;
    MmPci32 (0x00, Bus, Slot, 0x00, PCI_PRE_MEMBASE) = DISBL_PMEM_REG24;
    MmPci8  (0x00, Bus, Slot, 0x00, PCI_CMD) = CMD_BUS_MASTER;

    pi->BusNumLimit--;
  }
}//SetSlotsAsUnused
//[-start-130123-IB04770265-modify]//
#define PCIE_CAP_ID_VSEC   0x000B
STATIC
UINT16
FindVendorSpecificHeader(
  IN    UINT8  Bus
)
{
  PCIE_EXT_CAP_HDR  ExtCap;
  UINT16 ExtendedRegister = 0x100;

  while (ExtendedRegister) {
    ExtCap.EXT_CAP_HDR = MmPci32  (0x00, Bus, 0x00, 0x00, ExtendedRegister);
    if (ExtCap.Fields.ExtCapId == 0xFFFF)
      return 0x0000; // No Vendor-Specific Extended Capability header

    if (PCIE_CAP_ID_VSEC == ExtCap.Fields.ExtCapId)
      return ExtendedRegister;

    ExtendedRegister = ExtCap.Fields.NextItemPtr;
  }
  return 0x0000; // No Vendor-Specific Extended Capability header
}

typedef union _BRDG_CIO_MAP_REG
{
  UINT32  AB_REG;
  struct
  {
    UINT32    NumOfDSPorts : 5;
    UINT32    CIOPortMap   : 27;
  } Fields;
}BRDG_CIO_MAP_REG;
STATIC
BOOLEAN
GetCIOSlotByDevId(
  IN      UINT8  Bus,
    OUT   UINT8* CIOSlot,
    OUT   UINT8* MaxSlotNum
)
{
  UINT16            VSECRegister;
  BRDG_CIO_MAP_REG  BridgMap;
  UINT32            BitScanRes;
  UINT16            DevId = MmPci16 (0x00, Bus, 0x00, 0x00, PCI_DID);

  // Init out params in case device is not recognised
  *CIOSlot = 4;
  *MaxSlotNum = 7;

  switch(DevId) // For known device IDs
  {
  case 0x1513:
  case 0x151A:
  case 0x151B:
  case 0x1547:
  case 0x1548:
  case 0x1549:
    return TRUE; // Just return
  }
//    return FALSE;
  VSECRegister = FindVendorSpecificHeader(Bus);
  if(!VSECRegister)
    return TRUE; // Just return

  // Go to Bridge/CIO map register
  VSECRegister += 0x18;

  BridgMap.AB_REG = MmPci32  (0x00, Bus, 0x00, 0x00, VSECRegister);
  // Check for range
  if(BridgMap.Fields.NumOfDSPorts < 1 || BridgMap.Fields.NumOfDSPorts > 27)
    return TRUE;// Not a valid register

  // Set OUT params

  *MaxSlotNum = (UINT8)BridgMap.Fields.NumOfDSPorts;
  
  if(!BitScanForward(&BitScanRes, BridgMap.Fields.CIOPortMap))// No DS bridge which is CIO port
    return FALSE;

  *CIOSlot = (UINT8)BitScanRes;
  return TRUE;
//[-end-130123-IB04770265-modify]//
}//GetCIOSlotByDevId

STATIC
BOOLEAN
ConfigureEP(
  IN      INT8  Depth, 
  IN  OUT UINT8* Bus,
  IN  OUT PortInfo* pi
)
{
  UINT8     SBus;
  UINT8     CIOSlot = 4;
  UINT8     MaxSlotNum = 7;
  UINT8     MaxPHYSlots;
  UINT8     UsedBusNumbers;
	UINT8     cmd;
  BOOLEAN   CIOSlotPresent;
  BOOLEAN   Continue;
  PortInfo  portInfo = *pi;

  // Based on Device ID assign CIO slot and max number of PHY slots to scan
  CIOSlotPresent = GetCIOSlotByDevId(*Bus, &CIOSlot, &MaxSlotNum);
  MaxPHYSlots = MaxSlotNum;// Correct if CIO slot is absent

	// Check whether EP already configured by examining CMD register
	cmd = MmPci8  (0x00, *Bus, 0x00, 0x00, PCI_CMD);
	if(cmd & CMD_BUS_MASTER) // Yes no need to touch this EP, just move to next one in chain
	{
		UINT8  CIOBus = *Bus + 1;
		if(!CIOSlotPresent)// CIO slot is not present in EP, just return FALSE
		{
			PrintCPStr("BMF");
			return FALSE;
		}
		// Take all resources from CIO slot and return
		pi->BusNumLimit = MmPci8  (0x00, CIOBus, CIOSlot, 0x00, PCI_SUBUS);
		pi->IOBase		= MmPci8  (0x00, CIOBus, CIOSlot, 0x00, PCI_IOBASE);
		pi->IOLimit		= MmPci8  (0x00, CIOBus, CIOSlot, 0x00, PCI_IOLIMIT);
		pi->MemBase		= MmPci16 (0x00, CIOBus, CIOSlot, 0x00, PCI_MEMBASE);
		pi->MemLimit	= MmPci16 (0x00, CIOBus, CIOSlot, 0x00, PCI_MEMLIMIT);
		pi->PMemBase	= MmPci16 (0x00, CIOBus, CIOSlot, 0x00, PCI_PRE_MEMBASE);
		pi->PMemLimit	= MmPci16 (0x00, CIOBus, CIOSlot, 0x00, PCI_PRE_MEMLIMIT);
		pi->PMemBaseU	= MmPci32 (0x00, CIOBus, CIOSlot, 0x00, PCI_PRE_MEMBASE_U);
		pi->PMemLimitU	= MmPci32 (0x00, CIOBus, CIOSlot, 0x00, PCI_PRE_MEMLIMIT_U);
		// Jump to next EP
		*Bus = MmPci8  (0x00, CIOBus, CIOSlot, 0x00, PCI_SBUS);
		// Should we continue?
		Continue = 0xFFFF != MmPci16 (0x00, *Bus, 0x00, 0x00, PCI_DID);
		return Continue;
	}
  SetCIOPortResources(*Bus, 0, // Assign all available resources to US port of EP
    *Bus + 1, pi->BusNumLimit, 0, pi);

  SBus = *Bus + 1;// Jump to DS port 

  if(CIOSlotPresent)
    MaxPHYSlots = CIOSlot;

  UsedBusNumbers = ConfigureSlot(SBus, MaxPHYSlots, Depth, pi);

  if(!CIOSlotPresent)
    return FALSE; // Stop resource assignment on this chain

  // Set rest of slots us unused
  SetSlotsAsUnused(SBus, MaxSlotNum, CIOSlot, pi);

  SetCIOPortResources(SBus, CIOSlot,
    SBus + UsedBusNumbers + 1,
    pi->BusNumLimit, &portInfo, pi);
  *Bus = SBus + UsedBusNumbers + 1;// Go to next EP

  if (*Bus > pi->BusNumLimit - 2) // In case of bus numbers are exhausted stop enumeration
    return FALSE;

//[-start-120524-IB03780444-modify]//
  if (!SysConfig.TbtWorkaround) {
    return 0xFFFF != MmPci16 (0x00, *Bus, 0x00, 0x00, PCI_DID);
  } else {
//[-start-120503-IB03780435-modify]//
    // Check whether we should continue on this chain
    Continue = 0xFFFF != MmPci16 (0x00, *Bus, 0x00, 0x00, PCI_DID);
    if(FALSE /*!Continue*/)
    {
      // Workaround for LR devices
      // Move resource window to the end of memory range
      UINT8 cmd = MmPci8  (0x00, SBus, CIOSlot, 0, PCI_CMD);
      MmPci8  (0x00, SBus, CIOSlot, 0, PCI_CMD) = CMD_BUS_MASTER;

      if(DISBL_MEM32_REG20 != MmPci32 (0x00, SBus, CIOSlot, 0x00, PCI_MEMBASE))
        MmPci16 (0x00, SBus, CIOSlot, 0, PCI_MEMBASE) = MmPci16 (0x00, SBus, CIOSlot, 0, PCI_MEMLIMIT);
      if(DISBL_PMEM_REG24 != MmPci32 (0x00, SBus, CIOSlot, 0x00, PCI_PRE_MEMBASE))
        MmPci16 (0x00, SBus, CIOSlot, 0, PCI_PRE_MEMBASE) = MmPci16 (0x00, SBus, CIOSlot, 0, PCI_PRE_MEMLIMIT);

      MmPci8  (0x00, SBus, CIOSlot, 0, PCI_CMD) = cmd;
    }
    return Continue;
//[-end-120503-IB03780435-modify]//
  }
//[-end-120524-IB03780444-modify]//
}//ConfigureEP

STATIC
VOID
GetPortResources(
  IN      UINT8 Bus,
  IN      UINT8 Dev,
  IN  OUT PortInfo* pi
)
{
  pi->BusNumLimit = MmPci8  (0x00, Bus, Dev, 0x00, PCI_SUBUS);
  pi->IOBase      = MmPci8  (0x00, Bus, Dev, 0x00, PCI_IOBASE) & 0xF0;
  pi->IOLimit     = MmPci8  (0x00, Bus, Dev, 0x00, PCI_IOLIMIT) & 0xF0;;
  pi->MemBase     = MmPci16 (0x00, Bus, Dev, 0x00, PCI_MEMBASE) & 0xFFF0;
  pi->MemLimit    = MmPci16 (0x00, Bus, Dev, 0x00, PCI_MEMLIMIT) & 0xFFF0;
  pi->PMemBase    = MmPci16 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMBASE) & 0xFFF0;
  pi->PMemLimit   = MmPci16 (0x00, Bus, Dev, 0x00, PCI_PRE_MEMLIMIT) & 0xFFF0;
  pi->IOLimit |= 0xF;
  pi->MemLimit |= 0xF;
  pi->PMemLimit |= 0xF;
}//GetPortResources

STATIC
VOID
ConfigurePort(
  IN      UINT8 Bus,
  IN      UINT8 Dev,
  IN  OUT PortInfo* pi
)
{
  INT8     i;
  UINT8     USBusNum = MmPci8  (0x00, Bus, Dev, 0x00, PCI_SBUS);

  if (0xFFFF == MmPci16 (0x00, USBusNum, 0x00, 0x00, PCI_DID))// Nothing to do if TBT device is not connected
    return;
  
  GetPortResources(Bus, Dev, pi);// Take reserved resources from DS port

  // Assign resources to EPs
  for (i = 0; i < MAX_TBT_DEPTH; ++i) {
    pi->ConfedEP++;
    if(!ConfigureEP(i, &USBusNum, pi))
      return;
  }
}//ConfigurePort

EFI_STATUS
ThunderboltCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context        OPTIONAL,
  IN OUT VOID    *CommBuffer     OPTIONAL,
  IN OUT UINTN   *CommBufferSize OPTIONAL
  )
{
  PortInfo          portInfo = {0};
  UINT8             HRBus = 1;
  UINT8             i;

  PROGRESS_CODE (SMM_THUNDERBOLT_CALL);
  PortInfoInit(&portInfo);

  if (!InitializeHostRouter(&HRBus)) {
//[-start-120508-IB03780437-add]//
      mGlobalNvsArea->TbtEndPointNum = 0;
//[-end-120508-IB03780437-add]//
    PROGRESS_CODE (0xCB); //CaBle is unplugged
    return EFI_SUCCESS;
  }

  // Configure DS ports
  for(i = 3; i <= 6; ++i) {
    isOddBridgeNum = i & 1;
    ConfigurePort(HRBus + 1, i, &portInfo);
  }
//[-start-120508-IB03780437-add]//
  mGlobalNvsArea->TbtEndPointNum = portInfo.ConfedEP;
//[-end-120508-IB03780437-add]//
  PROGRESS_CODE (SMM_THUNDERBOLT_CALL + 2 + portInfo.ConfedEP); //PostCode = 0xAC + # of connected EP
  return EFI_SUCCESS;
}//ThunderboltCallback

//#define MAX_BUS_NUM_TO_SCAN  ((PCIEX_LENGTH >> 20) - 1)
#define PCI_VENDOR_DEV_ID    0x00
#define PCI_DEVICE           0x0
#define PCI_BRIDGE           0x1
#define INVALID_PCI_DEVICE   0xFFFFFFFF
#define PCI_HEADER_TYPE      0x0E
#define PCI_PRI_BUS_NUM      0x18
#define PCI_SEC_BUS_NUM      0x19
#define PCI_SUB_BUS_NUM      0x1A
#define PCI_CMD_REG          0x04

#if 0
STATIC
VOID
DisablePCIDevicesAndBridges ()
{
  UINT8   Bus;
  UINT8   Dev;
  UINT8   Fun;
  UINT8   RegVal;
  UINT8   i;
  UINT8   MinBus = 1;
  UINT16  DeviceId;

  //for(Dev = 0; Dev < 8; ++Dev)
  //{
  // PciOr8(PCI_LIB_ADDRESS(2, Dev, 0, PCI_BRIDGE_CONTROL_REGISTER_OFFSET), 0x40);
  // gBS->Stall(2000);    // 2msec
  // PciAnd8(PCI_LIB_ADDRESS(2, Dev, 0, PCI_BRIDGE_CONTROL_REGISTER_OFFSET), 0xBF);
  //}
  //gBS->Stall(200 * 1000);    // 200 msec

  for(i = 0; i < count(HR_Slots); ++i) {
    MinBus  = MmPci8  (0x00, HR_Slots[i].Bus, HR_Slots[i].Dev, HR_Slots[i].Fun, PCI_SBUS);
    DeviceId = MmPci16 (0x00, MinBus, 0x00, 0x00, PCI_DID);
    if (IsTBTHostRouter(DeviceId))
      break;
  }

  if(i >= count(HR_Slots))
    return;

  MinBus++;


  //  Disable PCI device First, and then Disable PCI Bridge

  for (Bus = MAX_BUS_NUM_TO_SCAN; Bus > MinBus; --Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        if (INVALID_PCI_DEVICE == MmPci32(0x00, Bus, Dev, Fun, PCI_VENDOR_DEV_ID)) {
          if (Fun == 0)
            break;

          continue;
        }

        RegVal = MmPci8(0x00, Bus, Dev, Fun, PCI_HEADER_TYPE);
        if (PCI_DEVICE == (RegVal & 1)) {
          // ********     Disable PCI Device   ********
          // BIT0  I/O Space Enabled    BIT1  Memory Space Enabled
          // BIT2  Bus Master Enabled   BIT4  Memory Write and Invalidation Enable
          MmPci8(0x00, Bus, Dev, Fun, PCI_CMD_REG) &= (UINT8)~(BIT0 | BIT1 | BIT2 | BIT4);

          MmPci32(0x00, Bus, Dev, Fun, PCI_BAR0) = 0;
          MmPci32(0x00, Bus, Dev, Fun, PCI_BAR1) = 0;
          MmPci32(0x00, Bus, Dev, Fun, PCI_BAR2) = 0;
          MmPci32(0x00, Bus, Dev, Fun, PCI_BAR3) = 0;
          MmPci32(0x00, Bus, Dev, Fun, PCI_BAR4) = 0;
          MmPci32(0x00, Bus, Dev, Fun, PCI_BAR5) = 0;
        }
      }
    }
  }

  // now no more PCI dev on another side of PCI Bridge can safty disable PCI Bridge

  for (Bus = MAX_BUS_NUM_TO_SCAN; Bus > MinBus; --Bus) {
    for (Dev = 0; Dev <= PCI_MAX_DEVICE; ++Dev) {
      for (Fun = 0; Fun <= PCI_MAX_FUNC; ++Fun) {
        if (INVALID_PCI_DEVICE == MmPci32(0x00, Bus, Dev, Fun, PCI_VENDOR_DEV_ID)) {
          if (Fun == 0)
            break;

          continue;
        }
        RegVal = MmPci8(0x00, Bus, Dev, Fun, PCI_HEADER_TYPE);
        if (PCI_BRIDGE == (RegVal & BIT0)) {
          MmPci8(0x00, Bus, Dev, Fun, PCI_CMD_REG) &= (UINT8)~(BIT0 | BIT1 | BIT2 | BIT4);
          MmPci8(0x00, Bus, Dev, Fun, PCI_PRI_BUS_NUM) = 0;
          MmPci8(0x00, Bus, Dev, Fun, PCI_SUB_BUS_NUM) = 0;
          MmPci8(0x00, Bus, Dev, Fun, PCI_SEC_BUS_NUM) = 0;
          MmPci32(0x00, Bus, Dev, Fun, PCI_PRE_MEMBASE_U) = 0;
        }
      } // for ( Fun .. )
    } // for ( Dev ... )
  } // for ( Bus ... )
}//DisablePCIDevicesAndBridges
#endif
//
// SMI handler installation code
// 
// Debug addresses initialization code
//
// SmmVariable initialization code
//
STATIC
VOID
SetReservedResources()
{
  // Read from configuration or use default
  SysConfig.TBTCacheLineSize = DEF_CACHE_LINE_SIZE;
  SysConfig.ReserveMemoryPerSlot = DEF_RES_MEM_PER_DEV;
  SysConfig.ReservePMemoryPerSlot = DEF_RES_PMEM_PER_DEV;
  SysConfig.ReserveIOPerSlot = DEF_RES_IO_PER_DEV;

  // Convert to register format
  SysConfig.ReserveMemoryPerSlot <<= 4;
  SysConfig.ReservePMemoryPerSlot <<= 4;
  SysConfig.ReserveIOPerSlot <<= 2;
}
