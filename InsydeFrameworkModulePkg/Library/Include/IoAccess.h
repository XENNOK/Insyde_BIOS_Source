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

Copyright (c)  2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  IoAccess.h

Abstract:

  Macros to simplify and abstract the interface to PCI configuration.

--*/

#ifndef _EFI_IOACCESS_H_
#define _EFI_IOACCESS_H_
#include "Tiano.h"

#define IA32API __cdecl



//
// Set variable bit field value :
// eg: var[10 : 20] = xxx <=> SetBitField( var, 10, 20, xxx)
//

#define SetBitField(Var, StartBit, EndBit, Val ) do { \
    Var &= ~(((1 << (StartBit)) - 1) ^ ((1 << ((EndBit) + 1)) - 1)); \
    Var |= ((Val) << (StartBit)); \
  } while(0)

//
// Get variable bit field value :
// eg: var[10 : 20] = GetBitField( var, 10, 20)
//

#define GetBitField( Var, StartBit, EndBit) \
  (Var & (((1 << (StartBit)) - 1) ^ ((1 << ((EndBit) + 1)) - 1))) >> (StartBit)


//
// MMIO Register Access Methods
//
//
// UINT32
//


#define MemIo32Ptr( Register ) \
  ( (volatile UINT32 *)( Register ))

#define MemIo32( Register ) \
  *MemIo32Ptr( Register )

#define MemIo32Or( Register, OrData ) \
  MemIo32( Register ) = (UINT32) ( MemIo32( Register ) | (UINT32)(OrData) )

#define MemIo32And( Register, AndData ) \
  MemIo32( Register ) = (UINT32) ( MemIo32( Register ) & (UINT32)(AndData) )

#define MemIo32AndThenOr( Register, AndData, OrData ) \
  MemIo32(  Register ) = (UINT32) ( ( MemIo32( Register ) &  (UINT32)(AndData) ) | (UINT32)(OrData) )


//
// UINT16
//

#define MemIo16Ptr( Register ) \
  ( (volatile UINT16 *)( Register ))

#define MemIo16( Register ) \
  *MemIo16Ptr( Register )

#define MemIo16Or( Register, OrData ) \
  MemIo16( Register ) = (UINT16) ( MemIo16( Register ) | (UINT16)(OrData) )

#define MemIo16And( Register, AndData ) \
  MemIo16( Register ) = (UINT16) ( MemIo16( Register ) & (UINT16)(AndData) )

#define MemIo16AndThenOr( Register, AndData, OrData ) \
  MemIo16(  Register ) = (UINT16) ( ( MemIo16( Register ) &  (UINT16)(AndData) ) | (UINT16)(OrData) )


//
// UINT8
//

#define MemIo8Ptr( Register ) \
  ( (volatile UINT8 *)( Register ))

#define MemIo8( Register ) \
  *MemIo8Ptr( Register )

#define MemIo8Or( Register, OrData ) \
  MemIo8( Register ) = (UINT8) ( MemIo8( Register ) | (UINT8)(OrData) )

#define MemIo8And( Register, AndData ) \
  MemIo8( Register ) = (UINT8) ( MemIo8( Register ) & (UINT8)(AndData) )

#define MemIo8AndThenOr( Register, AndData, OrData ) \
  MemIo8(  Register ) = (UINT8) ( ( MemIo8( Register ) &  (UINT8)(AndData) ) | (UINT8)(OrData) )


//
// Set 8 bit Mmio register bit field value, the "Startbit" and "Endbit" should be in the range of 0-7:
// eg: reg[1 : 7] = xxx <=> SetMmioBitField8( reg, 1, 7, xxx)
//
#define SetMmioBitField8( Register, StartBit, EndBit, Val ) do { \
    UINT8 RegVal; \
    RegVal = (*(volatile UINT8 *) (Register)); \
    SetBitField(RegVal, StartBit, EndBit, Val ) \
    (*(volatile UINT8 *) (Register)) = RegVal; \
  } while(0)

//
// Set 16 bit Mmio register bit field value, the "Startbit" and "Endbit" should be in the range of 0-15:
// eg: reg[1 : 13] = xxx <=> SetMmioBitField16( reg, 1, 13, xxx)
//
#define SetMmioBitField16( Register, StartBit, EndBit, Val ) do { \
    UINT16 RegVal; \
    RegVal = (*(volatile UINT16 *) (Register)); \
    SetBitField(RegVal, StartBit, EndBit, Val ) \
    (*(volatile UINT16 *) (Register)) = RegVal; \
  } while(0)

//
// Set 32 bit Mmio register bit field value, the "Startbit" and "Endbit" should be in the range of 0-31
// eg: reg[1 : 29] = xxx <=> SetMmioBitField32( reg, 1, 29, xxx)
//
#define SetMmioBitField32( Register, StartBit, EndBit, Val ) do { \
    UINT32 RegVal; \
    RegVal = (*(volatile UINT32 *) (Register)); \
    SetBitField(RegVal, StartBit, EndBit, Val ) \
    (*(volatile UINT32 *) (Register)) = RegVal; \
  } while(0)


//
// Get 8 bit Mmio register bit field value, the "Startbit" and "Endbit" should be in the range of 0-7
// eg: reg[1 : 7] = GetMmioBitField8( reg, 1, 7)
//
#define GetMmioBitField8( Register, StartBit, EndBit) \
  GetBitField( *(volatile UINT8 *) (Register), StartBit, EndBit)

//
// Get 16 bit Mmio register bit field value, the "Startbit" and "Endbit" should be in the range of 0-15
// eg: reg[1 : 13] = GetMmioBitField16( reg, 1, 13)
//
#define GetMmioBitField16( Register, StartBit, EndBit) \
  GetBitField( *(volatile UINT16 *) (Register), StartBit, EndBit)

//
// Get 32 bit Mmio register bit field value, the "Startbit" and "Endbit" should be in the range of 0-31
// eg: reg[1 : 29] = GetMmioBitField32( reg, 1, 29)
//
#define GetMmioBitField32( Register, StartBit, EndBit) \
  GetBitField( *(volatile UINT32 *) (Register), StartBit, EndBit)



//
// Physical Io Register Access Methods
//
UINT8
IA32API
IoInput8 (
  IN  UINT16  Port
  )
;


UINT16
IA32API
IoInput16 (
  IN  UINT16  Port
  )
;

UINT32
IA32API
IoInput32 (
  IN  UINT16  Port
  )
;

VOID
IA32API
IoOutput8 (
  IN  UINT16  Port,
  IN  UINT8  Data
  )
;

VOID
IA32API
IoOutput16 (
  IN  UINT16  Port,
  IN  UINT16  Data
  )
;

VOID
IA32API
IoOutput32 (
  IN  UINT16  Port,
  IN  UINT32  Data
  )
;

//
// Set 8 bit Physical Io register bit field value, the "Startbit" and "Endbit" should be in the range of 0-7:
// eg: reg[1 : 7] = xxx <=> SetIoBitField8( reg, 1, 7, xxx)
//
#define SetIoBitField8( Port, StartBit, EndBit, Val ) do { \
    UINT8 PortVal; \
    PortVal = IoInput8(Port); \
    SetBitField(PortVal, StartBit, EndBit, Val ) \
    IoOutput8(Port, PortVal); \
  } while(0)

//
// Set 16 bit Physical Io register bit field value, the "Startbit" and "Endbit" should be in the range of 0-15:
// eg: reg[1 : 13] = xxx <=> SetIoBitField16( reg, 1, 13, xxx)
//
#define SetIoBitField16( Port, StartBit, EndBit, Val ) do { \
    UINT16 PortVal; \
    PortVal = IoInput16(Port); \
    SetBitField(PortVal, StartBit, EndBit, Val ) \
    IoOutput16(Port, PortVal); \
  } while(0)

//
// Set 32 bit Physical Io register bit field value, the "Startbit" and "Endbit" should be in the range of 0-31:
// eg: reg[1 : 29] = xxx <=> SetIoBitField32( reg, 1, 29, xxx)
//
#define SetIoBitField32( Port, StartBit, EndBit, Val ) do { \
    UINT32 PortVal; \
    PortVal = IoInput32(Port); \
    SetBitField(PortVal, StartBit, EndBit, Val ) \
    IoOutput32(Port, PortVal); \
  } while(0)

//
// Get 8 bit Physical Io register bit field value, the "Startbit" and "Endbit" should be in the range of 0-7
// eg: reg[1 : 7] = GetIoBitField8( reg, 1, 7)
//
#define GetIoBitField8( Port, StartBit, EndBit) \
  GetBitField( IoInput8(Port), StartBit, EndBit);

//
// Get 16 bit Physical Io register bit field value, the "Startbit" and "Endbit" should be in the range of 0-15
// eg: reg[1 : 13] = GetIoBitField16( reg, 1, 13)
//
#define GetIoBitField16( Port, StartBit, EndBit) \
  GetBitField( IoInput16(Port), StartBit, EndBit);

//
// Get 32 bit Physical Io register bit field value, the "Startbit" and "Endbit" should be in the range of 0-31
// eg: reg[1 : 29] = GetIoBitField32( reg, 1, 29)
//
#define GetIoBitField32( Port, StartBit, EndBit) \
  GetBitField( IoInput32(Port), StartBit, EndBit);


#define Io8Or(Port, OrData )   do { \
    UINT8  RegVal; \
    RegVal = IoInput8(Port); \
    RegVal |= OrData; \
    IoOutput8(Port, RegVal);\
  } while(0)

#define Io16Or(Port, OrData )   do { \
    UINT16  RegVal; \
    RegVal = IoInput16(Port); \
    RegVal |= OrData; \
    IoOutput16(Port, RegVal);\
  } while(0)

#define Io32Or(Port, OrData )   do { \
    UINT32  RegVal; \
    RegVal = IoInput32(Port); \
    RegVal |= OrData; \
    IoOutput32(Port, RegVal);\
  } while(0)

#define Io8And(Port, AndData )   do { \
    UINT8  RegVal; \
    RegVal = IoInput8(Port); \
    RegVal &= AndData; \
    IoOutput8(Port, RegVal);\
  } while(0)

#define Io16And(Port, AndData )   do { \
    UINT16  RegVal; \
    RegVal = IoInput16(Port); \
    RegVal &= AndData; \
    IoOutput16(Port, RegVal);\
  } while(0)

#define Io32And(Port, AndData )   do { \
    UINT32  RegVal; \
    RegVal = IoInput32(Port); \
    RegVal &= AndData; \
    IoOutput32(Port, RegVal);\
  } while(0)


#define Io8AndThenOr(Port, AndData, OrData)   do { \
    UINT8  RegVal; \
    RegVal = IoInput8(Port); \
    RegVal &= AndData; \
    RegVal |= OrData; \
    IoOutput8(Port, RegVal);\
  } while(0)

#define Io16AndThenOr(Port, AndData, OrData)   do { \
    UINT16  RegVal; \
    RegVal = IoInput16(Port); \
    RegVal &= AndData; \
    RegVal |= OrData; \
    IoOutput16(Port, RegVal);\
  } while(0)

#define Io32AndThenOr(Port, AndData, OrData)   do { \
    UINT32  RegVal; \
    RegVal = IoInput32(Port); \
    RegVal &= AndData; \
    RegVal |= OrData; \
    IoOutput32(Port, RegVal);\
  } while(0)


//
// Pci Io Register Access Methods
//

#define PCI_ADDRESS_INDEX       0xcf8
#define PCI_DATA_INDEX          0xcfc
#define PCI_ACCRESS_ENABLE_BIT  0x80000000

#define PciCfgAddr(bus, dev, func) \
    ((UINT32) ((((UINT32) bus) << 16) + (((UINT32) dev) << 11) + (((UINT32) func) << 8)))

#define PciExpCfgAddr( seg, bus, dev, func) \
  ( (UINTN) (seg) + \
    (UINTN)((bus) << 20) + \
    (UINTN)((dev) << 15) + \
    (UINTN)((func) << 12) )

VOID
PciWriteConfig8 (
  IN  UINT32 Register,
  IN  UINT8 Val
  )
;

VOID
PciWriteConfig16 (
  IN  UINT32 Register,
  IN  UINT16 Val
  )
;

VOID
PciWriteConfig32 (
  IN  UINT32 Register,
  IN  UINT32 Val
  )
;
UINT8
PciReadConfig8 (
  IN  UINT32 Register
  )
;

UINT16
PciReadConfig16(
  IN  UINT32 Register
  )
;
UINT32
PciReadConfig32(
  IN  UINT32 Register
  )
;

//
// Set 8 bit pci configuration space register bit field value, the "Startbit" and "Endbit" should be in the range of 0-7:
// eg: reg[1 : 7] = xxx <=> SetPciCfgBitField8( reg, 1, 7, xxx)
//
#define SetPciCfgBitField8( Register, StartBit, EndBit, Val ) do { \
    UINT8 RegVal; \
    RegVal = PciReadConfig8(Register); \
    SetBitField(RegVal, StartBit, EndBit, Val ) \
    PciWriteConfig8(Register, RegVal); \
  } while(0)

//
// Set 16 bit pci configuration space register bit field value, the "Startbit" and "Endbit" should be in the range of 0-15:
// eg: reg[1 : 13] = xxx <=> SetPciCfgBitField16( reg, 1, 13, xxx)
//
#define SetPciCfgBitField16( Register, StartBit, EndBit, Val ) do { \
    UINT16 RegVal; \
    RegVal = PciReadConfig16(Register); \
    SetBitField(RegVal, StartBit, EndBit, Val ) \
    PciWriteConfig16(Register, RegVal); \
  } while(0)

//
// Set 32 bit pci configuration space register bit field value, the "Startbit" and "Endbit" should be in the range of 0-31:
// eg: reg[1 : 29] = xxx <=> SetPciCfgBitField32( reg, 1, 29, xxx)
//
#define SetPciCfgBitField32( Register, StartBit, EndBit, Val ) do { \
    UINT32 RegVal; \
    RegVal = PciReadConfig32(Register); \
    SetBitField(RegVal, StartBit, EndBit, Val ) \
    PciWriteConfig32(Register, RegVal); \
  } while(0)


//
// Get 8 bit pci configuration space register bit field value, the "Startbit" and "Endbit" should be in the range of 0-7
// eg: reg[1 : 7] = GetPciCfgBitField8( reg, 1, 7)
//
#define GetPciCfgBitField8( Register, StartBit, EndBit) \
    GetBitField( PciReadConfig8(Register), StartBit, EndBit);

//
// Get 16 bit pci configuration space register bit field value, the "Startbit" and "Endbit" should be in the range of 0-15
// eg: reg[1 : 13] = GetPciCfgBitField16( reg, 1, 13)
//
#define GetPciCfgBitField16( Register, StartBit, EndBit) \
    GetBitField( PciReadConfig16(Register), StartBit, EndBit);

//
// Get 32 bit pci configuration space register bit field value, the "Startbit" and "Endbit" should be in the range of 0-31
// eg: reg[1 : 29] = GetPciCfgBitField16( reg, 1, 29)
//
#define GetPciCfgBitField32( Register, StartBit, EndBit) \
    GetBitField( PciReadConfig32(Register), StartBit, EndBit);



#define PciCfg8Or(Register, OrData )   do { \
    UINT8  RegVal; \
    RegVal = PciReadConfig8(Register); \
    RegVal |= OrData; \
    PciWriteConfig8(Register, RegVal);\
  } while(0)

#define PciCfg16Or(Register, OrData )   do { \
    UINT16  RegVal; \
    RegVal = PciReadConfig16(Register); \
    RegVal |= OrData; \
    PciWriteConfig16(Register, RegVal);\
  } while(0)

#define PciCfg32Or(Register, OrData )   do { \
    UINT32  RegVal; \
    RegVal = PciReadConfig32(Register); \
    RegVal |= OrData; \
    PciWriteConfig32(Register, RegVal);\
  } while(0)

#define PciCfg8And(Register, AndData )   do { \
    UINT8  RegVal; \
    RegVal = PciReadConfig8(Register); \
    RegVal &= AndData; \
    PciWriteConfig8(Register, RegVal);\
  } while(0)

#define PciCfg16And(Register, AndData )   do { \
    UINT16  RegVal; \
    RegVal = PciReadConfig16(Register); \
    RegVal &= AndData; \
    PciWriteConfig16(Register, RegVal);\
  } while(0)

#define PciCfg32And(Register, AndData )   do { \
    UINT32  RegVal; \
    RegVal = PciReadConfig32(Register); \
    RegVal &= AndData; \
    PciWriteConfig32(Register, RegVal);\
  } while(0)


#define PciCfg8AndThenOr(Register, AndData, OrData)   do { \
    UINT8  RegVal; \
    RegVal = PciReadConfig8(Register); \
    RegVal &= AndData; \
    RegVal |= OrData; \
    PciWriteConfig8(Register, RegVal);\
  } while(0)

#define PciCfg16AndThenOr(Register, AndData, OrData)   do { \
    UINT16  RegVal; \
    RegVal = PciReadConfig16(Register); \
    RegVal &= AndData; \
    RegVal |= OrData; \
    PciWriteConfig16(Register, RegVal);\
  } while(0)

#define PciCfg32AndThenOr(Register, AndData, OrData)   do { \
    UINT32  RegVal; \
    RegVal = PciReadConfig32(Register); \
    RegVal &= AndData; \
    RegVal |= OrData; \
    PciWriteConfig32(Register, RegVal);\
  } while(0)





#endif
