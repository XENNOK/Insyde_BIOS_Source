/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - SG Reference Code                               *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c) 2010 - 2013 Intel Corporation. All rights reserved    *;
;*    This software and associated documentation (if any) is furnished    *;
;*    under a license and may only be used or copied in accordance        *;
;*    with the terms of the license. Except as permitted by such          *;
;*    license, no part of this software or documentation may be           *;
;*    reproduced, stored in a retrieval system, or transmitted in any     *;
;*    form or by any means without the express written consent of         *;
;*    Intel Corporation.                                                  *;
;*                                                                        *;
;*                                                                        *;
;**************************************************************************/
/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

External(\_SB.PCI0.GFX0._DOD, MethodObj)
External(\_SB.PCI0.GFX0.DD01._ADR, MethodObj)
External(\_SB.PCI0.GFX0.DD02._ADR, MethodObj)
External(\_SB.PCI0.GFX0.DD03._ADR, MethodObj)
External(\_SB.PCI0.GFX0.DD04._ADR, MethodObj)
External(\_SB.PCI0.GFX0.DD05._ADR, MethodObj)
External(\_SB.PCI0.GFX0.DD06._ADR, MethodObj)
External(\_SB.PCI0.GFX0.DD07._ADR, MethodObj)
External(\_SB.PCI0.GFX0.DD08._ADR, MethodObj)

External(\_SB.PCI0.GFX0.DD01._DGS, MethodObj)
External(\_SB.PCI0.GFX0.DD02._DGS, MethodObj)
External(\_SB.PCI0.GFX0.DD03._DGS, MethodObj)
External(\_SB.PCI0.GFX0.DD04._DGS, MethodObj)
External(\_SB.PCI0.GFX0.DD05._DGS, MethodObj)
External(\_SB.PCI0.GFX0.DD06._DGS, MethodObj)
External(\_SB.PCI0.GFX0.DD07._DGS, MethodObj)
External(\_SB.PCI0.GFX0.DD08._DGS, MethodObj)

External(\_SB.PCI0.GFX0.DD02._DCS, MethodObj)

External(\_SB.PCI0.GFX0.DD02._BCL, MethodObj)
External(\_SB.PCI0.GFX0.DD02._BQC, MethodObj)
External(\_SB.PCI0.GFX0.DD02._BCM, MethodObj)
External(\_SB.PCI0.PEG0, DeviceObj)
External(\_SB.PCI0.PEG0.PEGP, DeviceObj)
External(\_SB.PCI0.PEG0.PEGP._ADR)
External(\EECP)
External(\XBAS)
External(\GBAS)
External(\HLRS)
External(\PWEN)
External(\PWOK)
External(\SGMD)
External(\SGGP)
External(PNHM, IntObj)
External(P0UB, IntObj)
External(PCSL, IntObj)
External(SC7A, IntObj)


Scope(\_SB.PCI0.PEG0)
{

    // Define a Memory Region that will allow access to the PEG root port
    // Register Block.
    //
    //OperationRegion(RPCI,PCI_Config,0x00,0xF0)
    //Field(RPCI,DWordAcc,Lock,Preserve)
    //{
    //}

    OperationRegion(RPCX,SystemMemory,Add(\XBAS,0x8000),0x1000)
    Field(RPCX,ByteAcc,NoLock,Preserve)
    {
        Offset(0),
        PVID,   16,
        PDID,   16,
        Offset(0xAC),                   // LCAP - Link Capabilities Register
        ,       4,
        CMLW,   6,                      // 9:4, Max Link Width
        Offset(0xB0),                   // LCTL - Link Control Register
        ASPM,   2,                      // 1:0, ASPM //Not referenced in code
        ,       2,
        LNKD,   1,                      // Link Disable
        Offset(0x11A),
        ,       1,
        VCNP,   1,  //VC0RSTS.VC0NP
        Offset(0x214),
        ,       16,
        LNKS,   4,  //PEGSTS.LKS
        Offset(0x504),
        ,       16,
        PCFG,   2,  //FUSESCMN.PEG1CFGSEL
        Offset(0x508),
        TREN,   1,  //TRNEN.TREN
        Offset(0xC20),
        ,       4,
        AFES,   2,  //AFEOVR.RXSQDETOVR
        Offset(0xD0C),
        ,       20,
        LREV,   1,  //PEGTST.LANEREVSTS
    }

    Method(RBP0,1) // ReadBytePEG0
    {
      // Function to read pcie byte of Peg0 [0x00/0x01/0x00]
      //
      // Arg0 : The offset of pcie config space to be read
      //
      Store( Add(Add(\XBAS,0x8000), Arg0) ,Local7)
      OperationRegion(PCI0, SystemMemory, Local7, 1) 
      Field(PCI0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 8
      }
      Return(TEMP)
    } // End of Method(RBP0,1)

    Method(WBP0,2) // WriteBytePEG0
    {
      // Function to write pcie byte of Peg0 [0x00/0x01/0x00]
      //
      // Arg0 : The offset of pcie config space to be written
      // Arg1 : Value to be written
      //
      Store( Add(Add(\XBAS,0x8000), Arg0) ,Local7)
      OperationRegion(PCI0, SystemMemory, Local7, 1) 
      Field(PCI0, ByteAcc,NoLock,Preserve)
      {
        TEMP, 8
      }
      Store(Arg1,TEMP)
      Return(TEMP)
    } // End of Method(WBP0,2)

    Method(BSPR,2)
    {
      // Function to set/reset powerdown the bundles
      //
      // Arg0 : The zero based bundle number
      // Arg1 : Value to be written
      //
      Store( Add(Add(Add(\XBAS,0x8000), 0x91C) , Multiply(Arg0,0x20)),Local7)
      OperationRegion(PCI0, SystemMemory, Local7, 4) 
      Field(PCI0, DWordAcc,NoLock,Preserve)
      {
        Offset(0x0),
        ,31,
        TEMP, 1
      }
        Store(Arg1,TEMP)
    } // End of Method(BSPR,2)

    Method(C7OK,1)
    {
      // Function to set/reset C7 Allowed
      //
      // Arg0 : Value to be written
      //

      //
      // Memory window to the Host Bus registers 
      //
      OperationRegion(MWHB,SystemMemory,\XBAS,0x1000)
      Field(MWHB,DWordAcc,NoLock,Preserve)
      {
          Offset(0x48),   // MCHBAR (0:0:0:48)
          MHEN, 1,        // Enable
              , 14,
          MHBR, 17,       // MCHBAR [31:15]
      }

      //
      // Memory window to the registers starting at MCHBAR+5000h. 
      //
      OperationRegion (MBAR, SystemMemory, Add(ShiftLeft(MHBR,15),0x5000), 0x1000)
      Field (MBAR, ByteAcc, NoLock, Preserve)
      {
          Offset (0xDA8), // BIOS_RESET_CPL (MCHBAR+0x5da8)
          ,      2,       //
          C7AD,  1,       // C7 Allowed [2:2]
      }

      Store(Arg0,C7AD)
    } // End of Method(C7OK,1)

    //-----------------------------------------
    // Runtime Device Power Management - Begin
    //-----------------------------------------
    // Note:
    //      Runtime Device Power Management can be achieved by using _PRx or _PSx or both

    //
    // Name: PG00
    // Description: Declare a PowerResource object for PEG0 slot device
    //
    PowerResource(PG00, 0, 0)
    {
      Name(_STA, One)

      Method(_ON, 0, Serialized)
      {

        \_SB.PCI0.PEG0.PEGP.HGON()
        Store(One, _STA)
      }

      Method(_OFF, 0, Serialized)
      {

        \_SB.PCI0.PEG0.PEGP.HGOF()
        Store(Zero, _STA)
      }
    } //End of PowerResource(PG00, 0, 0)

    Name(_PR0,Package(){PG00})
    Name(_PR2,Package(){PG00})
    Name(_PR3,Package(){PG00})

//    //
//    // Name: _PS0
//    // Description: D0 Method for PEG0 slot device
//    // Input: Nothing
//    // Return: Nothing
//    //
//    Method(_PS0, 0, Serialized)
//    {
//
//      \_SB.PCI0.PEG0.PEGP.HGON()
//    }
//
//    //
//    // Name: _PS3
//    // Description: D3 Method for PEG0 slot device
//    // Input: Nothing
//    // Return: Nothing
//    //
//    Method(_PS3, 0, Serialized)
//    {
//
//      \_SB.PCI0.PEG0.PEGP.HGOF()
//    }

    Method(_S0W, 0)
    {
      Return(4) //D3cold is supported
    }

    //-----------------------------------------
    // Runtime Device Power Management - End
    //-----------------------------------------

    Device(PEGA) { // P.E.G. Device D0F1
      Name(_ADR, 0x00000001)

      OperationRegion(ACAP, PCI_Config, \EECP,0x14)
      Field(ACAP,DWordAcc, NoLock,Preserve)
      {
        Offset(0x10),
        LCT1,   16,  // Link Control register
      }
      Method(_PRW, 0) { Return(GPRW(0x09, 4)) } // can wakeup from S4 state
    } // end "P.E.G. Device D0F1"
}

Scope(\_SB.PCI0.PEG0.PEGP)
{
    Name (ONOF, 0x1) //Endpoint On-Off flag status. Assume Endpoint is ON by default {1-ON, 0-OFF}
    Name (IVID, 0xFFFF) //Invalid Vendor ID
    Name (TCNT, 0)
    Name (LDLY, 100) //100 ms
    Name (ELCT, 0x00000000)
    Name (HVID, 0x0000)
    Name (HDID, 0x0000)
    Name (FBDL, 0x0) //BndlPwrdnFirst
    Name (MBDL, 0x0) //MaxBndlPwrdnCount
    Name (CBDL, 0x0) //BndlPwrdnCount
    Name (HSTR, 0x0) //HwStrap
    Name (UULN, 0x0) //UnusedLanes
    Name (INDX, 0x0)
    Name (POFF, 0x0)
    Name (PLEN, 0x0)
    Name (PDAT, 0x0)
    Name (WLSB, 0x0)
    Name (WMSB, 0x0)
    Name (DMLW, 0x0)
    Name (DAT0, Buffer() {
    //Offset    Length    Data
	
    //Save-Restore Any Controller fields
    0xD8,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0x00,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x04,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x08,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x0C,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x10,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x0C,0x08,    0x4,    0x00,0x00,0x00,0x00,
    0x20,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x24,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x28,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x2C,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x30,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x2C,0x08,    0x4,    0x00,0x00,0x00,0x00,
    0x40,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x44,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x48,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x4C,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x50,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x4C,0x08,    0x4,    0x00,0x00,0x00,0x00,
    0x60,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x64,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x68,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x6C,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x70,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x6C,0x08,    0x4,    0x00,0x00,0x00,0x00,
    0x80,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x84,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x88,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x8C,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x90,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x8C,0x08,    0x4,    0x00,0x00,0x00,0x00,
    0xA0,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xA4,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xA8,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xAC,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xB0,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xAC,0x08,    0x4,    0x00,0x00,0x00,0x00,
    0xC0,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xC4,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xC8,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xCC,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xD0,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xCC,0x08,    0x4,    0x00,0x00,0x00,0x00,
    0xE0,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xE4,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xE8,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xEC,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xF0,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xEC,0x08,    0x4,    0x00,0x00,0x00,0x00,
    0x30,0x0C,    0x4,    0x00,0x00,0x00,0x00,
    0x00,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x04,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x08,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x0C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xA0,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xA4,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xA8,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xAC,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xB0,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xB4,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xB8,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xBC,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xC0,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xC4,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xC8,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xCC,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xD0,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xD4,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xD8,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xDC,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xE0,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xE4,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xE8,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xEC,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xF0,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xF4,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xF8,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0xFC,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x10,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x14,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x18,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x1C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x20,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x24,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x28,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x2C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x30,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x34,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x38,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x3C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x40,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x44,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x48,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x4C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x50,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x54,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x58,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x5C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x60,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x64,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x68,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x6C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x70,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x74,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x78,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x7C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x80,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x84,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x88,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x8C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x90,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x94,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x98,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x9C,0x0A,    0x4,    0x00,0x00,0x00,0x00,
    0x18,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x38,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x58,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x78,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0x98,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xB8,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xD8,0x09,    0x4,    0x00,0x00,0x00,0x00,
    0xF8,0x09,    0x4,    0x00,0x00,0x00,0x00,

    //Save-Restore Appropriate Controller fields
    0x24,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0xf8,0x01,    0x4,    0x00,0x00,0x00,0x00,
    0x60,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x28,0x0C,    0x4,    0x00,0x00,0x00,0x00,
    0x38,0x0C,    0x4,    0x00,0x00,0x00,0x00,
    0x14,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0x0C,0x00,    0x1,    0x00,
    0x19,0x00,    0x1,    0x00,
    0x1A,0x00,    0x1,    0x00,
    0x1C,0x00,    0x1,    0x00,
    0x1D,0x00,    0x1,    0x00,
    0x20,0x00,    0x2,    0x00,0x00,
    0x22,0x00,    0x2,    0x00,0x00,
    0x24,0x00,    0x2,    0x00,0x00,
    0x26,0x00,    0x2,    0x00,0x00,
    0x28,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0x2C,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0x3C,0x00,    0x1,    0x00,
    0x3D,0x00,    0x1,    0x00,
    0x3E,0x00,    0x2,    0x00,0x00,
    0x84,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0x8C,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0x92,0x00,    0x2,    0x00,0x00,
    0x94,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0x98,0x00,    0x2,    0x00,0x00,
    0xA2,0x00,    0x2,    0x00,0x00,
    0xA8,0x00,    0x2,    0x00,0x00,
    0xAC,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0xB0,0x00,    0x2,    0x00,0x00,
    0xB4,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0xBC,0x00,    0x2,    0x00,0x00,
    0xC8,0x00,    0x2,    0x00,0x00,
    0xD0,0x00,    0x2,    0x00,0x00,
    0xEC,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0x14,0x01,    0x4,    0x00,0x00,0x00,0x00,
    0x44,0x01,    0x4,    0x00,0x00,0x00,0x00,
    0x50,0x01,    0x4,    0x00,0x00,0x00,0x00,
    0x58,0x01,    0x4,    0x00,0x00,0x00,0x00,
    0x5C,0x01,    0x4,    0x00,0x00,0x00,0x00,
    0xFC,0x01,    0x4,    0x00,0x00,0x00,0x00,
    0x00,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x04,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x08,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x28,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x2C,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x38,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x40,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x44,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x50,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x58,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0x5C,0x02,    0x4,    0x00,0x00,0x00,0x00,
    0xD0,0x0C,    0x4,    0x00,0x00,0x00,0x00,
    0x34,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xF4,0x00,    0x4,    0x00,0x00,0x00,0x00,
    0xA0,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xA4,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xA8,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xAC,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xB0,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xB4,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xB8,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xBC,0x0D,    0x4,    0x00,0x00,0x00,0x00,
    0xCC,0x01,    0x4,    0x00,0x00,0x00,0x00,
    0x04,0x00,    0x2,    0x00,0x00,

    //Final field
    0xFF,0xFF,    0x4,    0xFF,0xFF,0xFF,0xFF  //Last data
    })

    Name (DAT1, Buffer() {
    //Offset    Length    Data
	
    //Save-Restore Any Controller fields
    0x06,0x00,    0x2,    0xFF,0xFF,
    0x1E,0x00,    0x2,    0xFF,0xFF,
    0xAA,0x00,    0x2,    0xFF,0xFF,
    0xC0,0x00,    0x4,    0xFF,0xFF,0xFF,0xFF,
    0xD2,0x00,    0x2,    0xFF,0xFF,
    0xC4,0x01,    0x4,    0xFF,0xFF,0xFF,0xFF,
    0xD0,0x01,    0x4,    0xFF,0xFF,0xFF,0xFF,
    0xF0,0x01,    0x4,    0xFF,0xFF,0xFF,0xFF,
    0x9C,0x0D,    0x4,    0xFF,0xFF,0xFF,0xFF,
    0xB2,0x00,    0x2,    0xFF,0xFF,

    //Final field
    0xFF,0xFF,    0x4,    0xFF,0xFF,0xFF,0xFF  //Last data
    })

    OperationRegion (PCIS, PCI_Config, 0x00, 0xF0)
    Field(PCIS, DWordAcc, Lock, Preserve)
    {
        Offset(0x0),
        DVID, 16,
        Offset(0x2C),
        SVID, 16,
        SDID, 16,
        Offset(0x4C),
        WVID, 16,
        WDID, 16,
    }        

    OperationRegion( GPR, SystemIO, \GBAS, 0x60 )
    Field( GPR, ByteAcc, Lock, Preserve )
    {
        Offset(0x0C),   // GPIO, Level, Bank 0
        LVL0,   32,
        Offset(0x38),   // GPIO, Level, Bank 1
        LVL1,   32,
        Offset(0x48),   // GPIO, Level, Bank 2
        LVL2,   32,
    }

    OperationRegion(PCAP, PCI_Config, \EECP,0x14)
    Field(PCAP,DWordAcc, NoLock,Preserve)
    {
        Offset(0x0C),                    // Link Capabilities Register
        ,       4,
        EMLW,   6,                       // 9:4, Max Link Width
        Offset(0x10),                    
        LCTL,   16,                      // Link Control register
    }

    Method (_INI)
    {
        Store (0x0, \_SB.PCI0.PEG0.PEGP._ADR)
    }

    //GetMaxBundles
    Method(GMXB,0)
    {
        Store (PCFG, HSTR) //HwStrap
        If (LEqual(HSTR, 3)) //SA_PEG_x16_x0_x0
        {
          Store (8, Local0)
        }
        Else 
        {
          Store (4, Local0)
        }

        Return(Local0)
    }

    //PowerUpAllBundles
    Method(PUAB,0)
    {
        Store (0, FBDL) //BndlPwrdnFirst
        Store (0, CBDL) //BndlPwrdnCount
        Store (PCFG, HSTR) //HwStrap
        If (LEqual(HSTR, 3)) //SA_PEG_x16_x0_x0
        {
          Store (0, FBDL)
          Store (8, CBDL)
        }
        Else
        {
          If(LEqual(LREV,0))
          {
            Store (0, FBDL)
            Store (4, CBDL)
          }
          Else
          {
            Store (4, FBDL)
            Store (4, CBDL)
          }
        }

        Store (1, INDX)
        If (LNotEqual(CBDL,0))
        {
          While(LLessEqual(INDX, CBDL))
          {
            BSPR(FBDL, 0)
            Increment (FBDL)
            Increment (INDX)
          }
        }
    } // End of Method(PUAB,0)

    // PowerDownUnusedBundles
    // Arg0 = BndlPwrdnCount
    Method(PDUB,1)
    {
        Store (0, FBDL) //BndlPwrdnFirst
        Store (Arg0, CBDL) //BndlPwrdnCount
        If (LEqual(CBDL,0))
        {
          // All lanes are used. Do nothing
          Return
        }

        If (LEqual(HSTR, 3)) //SA_PEG_x16_x0_x0
        {
          If(LEqual(LREV,0))
          {
            Store (Subtract(8, CBDL), FBDL) //8 - (UnusedLanes / 2)
          }
          Else
          {
            Store (0, FBDL)
          }
        }
        Else
        {
          If(LEqual(LREV,0))
          {
            Store (Subtract(4, CBDL), FBDL) //4 - (UnusedLanes / 2)
          }
          Else
          {
            Store (4, FBDL)
          }
        }

        Store (1, INDX)
        While(LLessEqual(INDX, CBDL)) //< Check that bundles need to be powered down
        {
          BSPR(FBDL, 1)
          Increment (FBDL)
          Increment (INDX)
        }
    } // End of Method(PDUB,0)

    Method(SPP0,0)
    {
        Store (0, INDX)
        While (1)
        {
            Store (DerefOf (Index(DAT0, INDX)), WLSB)
            Increment (INDX) //Offset is 2 bytes long <First byte-LSB>
            Store (DerefOf (Index(DAT0, INDX)), WMSB)
            Increment (INDX) //Offset is 2 bytes long <Second byte-MSB>
            Store (Or (ShiftLeft (WMSB, 8), WLSB), POFF)
            Store (DerefOf (Index(DAT0, INDX)), PLEN)
            Increment (INDX) //Length is 1 byte long

            If(LEqual(POFF,0xFFFF))
            {
               Break
            }

            While (LGreater(PLEN, 0))
            {
               Store(RBP0(POFF), Index(DAT0, INDX))
               Increment (INDX)
               Increment (POFF)
               Decrement (PLEN)
            }
        }
    } // End of Method(SPP0,0)

    Method(RPP0,0)
    {
        Store (0, INDX)
        While (1)
        {
            Store (DerefOf (Index(DAT0, INDX)), WLSB)
            Increment (INDX) //Offset is 2 bytes long <First byte-LSB>
            Store (DerefOf (Index(DAT0, INDX)), WMSB)
            Increment (INDX) //Offset is 2 bytes long <Second byte-MSB>
            Store (Or (ShiftLeft (WMSB, 8), WLSB), POFF)
            Store (DerefOf (Index(DAT0, INDX)), PLEN)
            Increment (INDX) //Length is 1 byte long

            If(LEqual(POFF,0xFFFF))
            {
               Break
            }

            While (LGreater(PLEN, 0))
            {
               WBP0(POFF, DerefOf (Index(DAT0, INDX)))
               Increment (INDX)
               Increment (POFF)
               Decrement (PLEN)
            }
        }
    } // End of Method(RPP0,0)

    Method(CLP0,0)
    {
        Store (0, INDX)
        While (1)
        {
            Store (DerefOf (Index(DAT1, INDX)), WLSB)
            Increment (INDX) //Offset is 2 bytes long <First byte-LSB>
            Store (DerefOf (Index(DAT1, INDX)), WMSB)
            Increment (INDX) //Offset is 2 bytes long <Second byte-MSB>
            Store (Or (ShiftLeft (WMSB, 8), WLSB), POFF)
            Store (DerefOf (Index(DAT1, INDX)), PLEN)
            Increment (INDX) //Length is 1 byte long

            If(LEqual(POFF,0xFFFF))
            {
               Break
            }

            While (LGreater(PLEN, 0))
            {
               WBP0(POFF, DerefOf (Index(DAT1, INDX)))
               Increment (INDX)
               Increment (POFF)
               Decrement (PLEN)
            }
        }
    } // End of Method(CLP0,0)

    Method(HGON,0,Serialized)
    {

        If (LEqual(CCHK(1), 0))
        {
          Return ()
        }

        Store(1, ONOF) //Indicate Endpoint is in ON state

        //Power on the dGPU card
        SGPO(HLRS, 1)                  //Assert dGPU_HOLD_RST#
        SGPO(PWEN, 1)                  //Assert dGPU_PWR_EN#

//        While(LNotEqual(SGPI(PWOK),1)) //Wait until dGPU_PWROK=1
//        {
//                Sleep(1)                
//        }

        Sleep(300)                     // Wait for 300ms if dGPU_PWROK has h/w issues
        
        SGPO(HLRS, 0)                  //Deassert dGPU_HOLD_RST#
        Sleep(100)                     // Wait for 100ms
        
        // Software clears BIOS_RESET_CPL.C7_ALLOWED
        If (LGreaterEqual(PCSL, 4)) //C7 or above
        {
          If (LEqual(SC7A, 0x01)) //Run-time C7 Allowed feature setup value = Enabled
          {
            C7OK(0)
          }
        }

        // Restore the PEG0 PCIE registers
        RPP0()

        // Clear the PEG0 errors
        CLP0()

        //Program AFEOVR.RXSQDETOVR
        //PCIe link disable for Switchable GFx
        //Additional Power savings: Set 0:1:0 0xc20 BIT4 = 0 & BIT5 = 0
        Store(0, AFES)

        //Program BND*SPARE.BNDL_PWRDN
        //PowerOff unused bundles for PEGs
        If (LGreaterEqual(And(PNHM, 0xF), 0x3)) //(CpuSteppingId >= EnumHswC0)
        {
          If (LNotEqual(P0UB, 0x00))
          {
            //PowerUpAllBundles
            PUAB()

            //Get BndlPwrdnCount
            If (LEqual (P0UB, 0xFF)) //AUTO
            {
               If (LGreater(CMLW, DMLW))
               {
                 Subtract (CMLW, DMLW, UULN) //UnusedLanes
               } 
               Else 
               {
                 Store (0, UULN) //UnusedLanes
               }
               Store (Divide(UULN,2), CBDL) //BndlPwrdnCount
            } 
            ElseIf (LNotEqual (P0UB, 0x00)) //1...8 bundles
            { 
               Store (P0UB, CBDL) //BndlPwrdnCount
            }

            //Get MaxBndlPwrdnCount
            Store(GMXB(), MBDL)

            If (LGreater(CBDL, MBDL))
            {
               Store(MBDL, CBDL)
            }

            //PowerDownUnusedBundles
            PDUB(CBDL)
          }
        }

        //Enable x16 Link
        //This bit should already be set to 0 in the _Off method. But do it anyway.
        Store(0,LNKD)

        // Enable controller initial training
        Store(1, TREN)

        // Wait until the VC negotiation is complete     
        Store(0, TCNT)
        While(LLess(TCNT, LDLY))
        {
          If(LEqual(VCNP,0))
          {
             Break
          }

          Sleep(16) //In some OS one tick is equal to 1/64 second (15.625ms)
          Add(TCNT, 16, TCNT)
        }  

        // Re-store the DGPU SSID
        Store(HVID,WVID)
        Store(HDID,WDID)

        // Re-store the Link Control register - Common Clock Control and ASPM
        Or(And(ELCT,0x0043),And(LCTL,0xFFBC),LCTL)
        Or(And(ELCT,0x0043),And(\_SB.PCI0.PEG0.PEGA.LCT1,0xFFBC),\_SB.PCI0.PEG0.PEGA.LCT1)

        Return ()
    }

    Method(_ON,0,Serialized)
    {

        HGON()

        //Ask OS to do a PnP rescan
        Notify(\_SB.PCI0.PEG0,0)

        Return ()
    }

    Method(HGOF,0,Serialized)
    {

        If (LEqual(CCHK(0), 0))
        {
          Return ()
        }

        Store(0, ONOF) //Indicate Endpoint is in OFF state

        // Save the Link Control register
        Store(LCTL,ELCT)

        // Save the DGPU SSID
        Store(SVID,HVID)
        Store(SDID,HDID)

        // Save the Endpoint Max Link Width
        Store(EMLW,DMLW)

        // Save the PEG0 PCIE registers
        SPP0()

        //Force disable the x16 link
        Store(1, LNKD)

        //Wait till link is actually in disabled state
        Store(0, TCNT)
        While(LLess(TCNT, LDLY))
        {
          If(LEqual(LNKS,0))
          {
             Break
          }

          Sleep(16) //In some OS one tick is equal to 1/64 second (15.625ms)
          Add(TCNT, 16, TCNT)
        }

        //Program AFEOVR.RXSQDETOVR
        //PCIe link disable for Switchable GFx
        //Additional Power savings: Set 0:1:0 0xc20 BIT4 = 0 & BIT5 = 1
        Store(2, AFES)

        // PowerOff all bundles for PEGs
        // Program BND*SPARE.BNDL_PWRDN
        If (LGreaterEqual(And(PNHM, 0xF), 0x3)) //(CpuSteppingId >= EnumHswC0)
        {
          If (LNotEqual(P0UB, 0x00))
          {
            //Get MaxBndlPwrdnCount
            Store(GMXB(), MBDL)

            //PowerDownUnusedBundles
            PDUB(MBDL)
          }
        }

        // Software sets BIOS_RESET_CPL.C7_ALLOWED
        If (LGreaterEqual(PCSL, 4)) //C7 or above
        {
          If (LEqual(SC7A, 0x01)) //Run-time C7 Allowed feature setup value = Enabled
          {
            C7OK(1)
          }
        }

        //Power-off the dGPU card
        SGPO(HLRS, 1)                   // Assert dGPU_HOLD_RST# (PERST#)
        SGPO(PWEN, 0)                   // Deassert dGPU_PWR_EN#                

        Return ()
    }
       
    Method(_OFF,0,Serialized)
    {

        HGOF()

        //Ask OS to do a PnP rescan
        Notify(\_SB.PCI0.PEG0,0)

        Return ()
    }

    Method(EPON, 0, Serialized)
    {
      Store(1, ONOF) //Indicate Endpoint is in ON state

      Return ()
    }

    Method(_STA,0,Serialized)
    {
      //
      // Check SGMode and dGPU Present Detect GPIO for SG system
      //
      If(And(SGMD,0x0F))
      {
        If(LNotEqual(SGGP,0x01))
        {
           Return(0x0F)
        }
        // Check dGPU_PWROK to detect dGPU.
        If(LEqual(SGPI(PWOK),1))  
        {
          Return(0x0F)
        }
        Return(0x00)
      }

      //
      // For non-SG system check for valid Vendor Id
      //
      If(LNotEqual(DVID,0xFFFF))
      {
          Return(0x0F)
      }
      Return(0x00)
    }


    Method(_DOD,0)
    {
        Return (\_SB.PCI0.GFX0._DOD())
    }


    Device(DD01)
    {
            Method(_ADR,0,Serialized)
            {
                    Return(\_SB.PCI0.GFX0.DD01._ADR())
            }

            // Device Current State.
            Method(_DCS,0)
            {

            }

            // Device Get State.

            Method(_DGS,0)
            {
                    // Return the Next State.
                    Return(\_SB.PCI0.GFX0.DD01._DGS())
            }

            // Device Set State.

            // _DSS Table:
            //
            //  BIT31   BIT30   Execution
            //  0       0       Don't implement.
            //  0       1       Cache change.  Nothing to Implement.
            //  1       0       Don't Implement.
            //  1       1       Display Switch Complete.  Implement.

            Method(_DSS,1)
            {
                    // Do nothing here in the OpRegion model.  OEMs may choose to
                    // update internal state if necessary.
            }
    }

    Device(DD02)
    {
            Method(_ADR,0,Serialized)
            {
                    Return(\_SB.PCI0.GFX0.DD02._ADR())
            }

            // Device Current State.

            Method(_DCS,0)
            {
                    // Get the Current Display State.
                    Return(\_SB.PCI0.GFX0.DD02._DCS())
            }

            // Device Get State.

            Method(_DGS,0)
            {
                    // Return the Next State.
                    Return(\_SB.PCI0.GFX0.DD02._DGS())
            }

            // Device Set State.

            Method(_DSS,1)
            {
                    // Do nothing here in the OpRegion model.  OEMs may choose to
                    // update internal state if necessary.
            }

/*
            Method(_DDC,1)
            {
                    If(Lor(LEqual(\_SB.PCI0.GFX0.PHED,1),LEqual(\_SB.PCI0.GFX0.PHED,2)))
                    {
                        Name(DDC2,Buffer (256) {0x0})
                        Store(\_SB.PCI0.GFX0.BDDC,DDC2)
                        Return(DDC2)
                    }
                    Return(Buffer(256){0x0})
            }
*/
            Method(_BCL,0)
            {
                    Return(\_SB.PCI0.GFX0.DD02._BCL())               
            }

            Method(_BQC,0)
            {
                    Return(\_SB.PCI0.GFX0.DD02._BQC())
            }
            
            Method(_BCM,1)
            {
                    Return(\_SB.PCI0.GFX0.DD02._BCM(Arg0))
            }

    }

    Device(DD03)
    {
            Method(_ADR,0,Serialized)
            {
                    Return(\_SB.PCI0.GFX0.DD03._ADR())
            }

            // Device Current State.

            Method(_DCS,0)
            {
                    // Get the Current Display State.
            }

            // Device Get State.

            Method(_DGS,0)
            {
                    // Return the Next State.
                    Return(\_SB.PCI0.GFX0.DD03._DGS())
            }

            // Device Set State.

            Method(_DSS,1)
            {
                    // Do nothing here in the OpRegion model.  OEMs may choose to
                    // update internal state if necessary.
            }
    }

    Device(DD04)
    {
            Method(_ADR,0,Serialized)
            {
                    Return(\_SB.PCI0.GFX0.DD04._ADR())
            }

            // Device Current State.

            Method(_DCS,0)
            {
                    // Get the Current Display State.
            }

            // Device Get State.

            Method(_DGS,0)
            {
                    // Return the Next State.
                    Return(\_SB.PCI0.GFX0.DD04._DGS())
            }

            // Device Set State.

            Method(_DSS,1)
            {
                    // Do nothing here in the OpRegion model.  OEMs may choose to
                    // update internal state if necessary.
            }
            
    }

    Device(DD05)
    {
            Method(_ADR,0,Serialized)
            {
                    Return(\_SB.PCI0.GFX0.DD05._ADR())
            }

            // Device Current State.

            Method(_DCS,0)
            {
                    // Get the Current Display State.
            }

            // Device Get State.

            Method(_DGS,0)
            {
                    // Return the Next State.
                    Return(\_SB.PCI0.GFX0.DD05._DGS())
            }

            // Device Set State.

            Method(_DSS,1)
            {
                    // Do nothing here in the OpRegion model.  OEMs may choose to
                    // update internal state if necessary.
            }
    }

    Device(DD06)
    {
            Method(_ADR,0,Serialized)
            {
                    Return(\_SB.PCI0.GFX0.DD06._ADR())
            }

            // Device Current State.

            Method(_DCS,0)
            {
                    // Get the Current Display State.
            }

            // Device Get State.

            Method(_DGS,0)
            {
                    // Return the Next State.
                    Return(\_SB.PCI0.GFX0.DD06._DGS())
            }

            // Device Set State.

            Method(_DSS,1)
            {
                    // Do nothing here in the OpRegion model.  OEMs may choose to
                    // update internal state if necessary.
            }
    }
    
    Device(DD07)
    {
            Method(_ADR,0,Serialized)
            {
                    Return(\_SB.PCI0.GFX0.DD07._ADR())
            }

            // Device Current State.

            Method(_DCS,0)
            {
                    // Get the Current Display State.
            }

            // Device Get State.

            Method(_DGS,0)
            {
                    // Return the Next State.
                    Return(\_SB.PCI0.GFX0.DD07._DGS())
            }

            // Device Set State.

            Method(_DSS,1)
            {
                    // Do nothing here in the OpRegion model.  OEMs may choose to
                    // update internal state if necessary.
            }
    }

    Device(DD08)
    {
            Method(_ADR,0,Serialized)
            {
                    Return(\_SB.PCI0.GFX0.DD08._ADR())
            }

            // Device Current State.

            Method(_DCS,0)
            {
                    // Get the Current Display State.
            }

            // Device Get State.

            Method(_DGS,0)
            {
                    // Return the Next State.
                    Return(\_SB.PCI0.GFX0.DD08._DGS())
            }

            // Device Set State.

            Method(_DSS,1)
            {
                    // Do nothing here in the OpRegion model.  OEMs may choose to
                    // update internal state if necessary.
            }
    }
  // GPIO Read
  // Arg0 = GPIO No.
  Method (SGPI,1,Serialized)
  {
    If(And(SGMD,0x0F))
    {
      If(LEqual(SGGP,0x01))
      {
        ShiftRight (Arg0, 7, Local1)
        And(Arg0, 0x7F, Arg0)   

        If (LLess(Arg0,0x20))
        {
          Store(\_SB.PCI0.PEG0.PEGP.LVL0, Local0)
          ShiftRight(Local0, Arg0, Local0)
        }
        ElseIf (LLess(Arg0,0x40))
        {
          Store(\_SB.PCI0.PEG0.PEGP.LVL1, Local0)
          ShiftRight(Local0, Subtract(Arg0,0x20), Local0)
        }
        Else
        {
          Store(\_SB.PCI0.PEG0.PEGP.LVL2, Local0)
          ShiftRight(Local0, Subtract(Arg0,0x40), Local0)
        }

        // 
        // Check if Active Low
        //
        If (LEqual(Local1,0))
        {
          Not(Local0, Local0)
        }

        Return(And(Local0,0x01))
      }
    }
	
    Return(0)
  }

  // GPIO Write
  // Arg0 = GPIO No.
  // Arg1 = Value (0/1)
  Method (SGPO,2,Serialized)
  {
    If(And(SGMD,0x0F))
    {
      If(LEqual(SGGP,0x01))
      {
        ShiftRight (Arg0, 7, Local3)
        And(Arg0, 0x7F, Arg0)   

        // 
        // Check if Active Low
        //
        If (LEqual(Local3,0))
        {
          Not (Arg1, Local3)
          And (Local3, 0x01, Local3)
        }
        Else 
        {
          And (Arg1, 0x01, Local3)
        }
        If (LLess(Arg0,0x20))
        {
          ShiftLeft(Local3, Arg0, Local0)
          ShiftLeft(0x00000001, Arg0, Local1)
          And(\_SB.PCI0.PEG0.PEGP.LVL0, Not(Local1), Local2)
          Or(Local2, Local0, \_SB.PCI0.PEG0.PEGP.LVL0)
        }
        ElseIf (LLess(Arg0,0x40))
        {
          ShiftLeft(Local3, Subtract(Arg0,0x20), Local0)
          ShiftLeft(0x00000001, Subtract(Arg0,0x20), Local1)
          And(\_SB.PCI0.PEG0.PEGP.LVL1, Not(Local1), Local2)
          Or(Local2, Local0, \_SB.PCI0.PEG0.PEGP.LVL1)
        }
        Else
        {
          ShiftLeft(Local3, Subtract(Arg0,0x40), Local0)
          ShiftLeft(0x00000001, Subtract(Arg0,0x40), Local1)
          And(\_SB.PCI0.PEG0.PEGP.LVL2, Not(Local1), Local2)
          Or(Local2, Local0, \_SB.PCI0.PEG0.PEGP.LVL2)
        }
        Return(1)
      }
    }
	
    Return(1)
  }

  //
  // Name: CCHK
  // Description: Function to check whether _ON/_OFF sequence is allowed to execute for the given PEG0 controller or not
  // Input: Arg0 -> 0 means _OFF sequence, 1 means _ON sequence
  // Return: 0 - Don't execute the flow, 1 - Execute the flow
  //
  Method(CCHK,1)
  {

    //Check for PEG0 controller presence
    If(LEqual(PVID, IVID))
    {

      Return(0)
    }

    //If Endpoint is not present[already disabled] before executing _OFF then don't call the _OFF method
    //If Endpoint is present[already enabled] before executing _ON then don't call the _ON method
    If(LEqual(Arg0, 0))
    {
      //_OFF sequence condition check
      If(LEqual(ONOF, 0))
      {

        Return(0)
      }
    }
    ElseIf(LEqual(Arg0, 1))
    {
      //_ON sequence condition check
      If(LEqual(ONOF, 1))
      {

        Return(0)
      }
    }


    Return(1)
  } // End of Method(CCHK,1)

}
