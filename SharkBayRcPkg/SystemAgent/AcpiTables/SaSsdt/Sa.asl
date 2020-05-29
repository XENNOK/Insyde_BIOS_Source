/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Sa.ASL

Abstract:

  Devices definition of SystemAgent ACPI reference code.

--*/
External(AR0A)
External(AR0B)
External(AR02)
External(PR0A)
External(PR0B)
External(PR02)
External(PICM)
External(\GPRW, MethodObj)
External(\_SB.PCI0, DeviceObj)

Scope (\_SB.PCI0) {
    Name(LTRS, 0)
    Name(OBFS, 0)

    Device(PEG0) { // P.E.G. Root Port D1F0
      Name(_ADR, 0x00010000)
      OperationRegion(PEGR,PCI_Config,0xC0,0x30)
      Field(PEGR,DWordAcc,NoLock,Preserve)
      {
        ,16,
        PSTS, 1,     // PME Status
        offset (44),
        GENG, 1,     // General Message GPE Enable
        ,1,          // Reserved
        PMEG, 1,     // PME GPE Enable
      }
      Method(_PRW, 0) { Return(GPRW(0x09, 4)) } // can wakeup from S4 state

      Method(_PSW,1)
      {
        If(Arg0)
        {
          Store(1,GENG)
          Store(1,PMEG)
        }
        Else
        {
          Store(0,GENG)
          Store(0,PMEG)
        }
      }
      Method(HPME,0,Serialized)
      {
        //
        // Clear PME status bit
        //
        Store(1,PSTS)
      }
      Method(_PRT,0) {
        If(PICM) { Return(AR02) }// APIC mode
        Return (PR02) // PIC Mode
      } // end _PRT
      //
      // Pass LTRx to LTRS so SaPcieDsm.asl can be reused for PEGs.
      //
      Method(_INI)
      {
        Store (LTRA, LTRS)
        Store (OBFA, OBFS)
      }
      include("SaPcieDsm.asl")

      Device(PEGP) { // P.E.G. Port Slot x16
        Name(_ADR, 0x00000000)
        Method(_PRW, 0) { Return(GPRW(0x09, 4)) } // can wakeup from S4 state
      } // end "P.E.G. Port Slot x16"

    } // end "P.E.G. Root Port D1F0"

    Device(PEG1) { // P.E.G. Root Port D1F1
      Name(_ADR, 0x00010001)
      OperationRegion(PEGR,PCI_Config,0xC0,0x30)
      Field(PEGR,DWordAcc,NoLock,Preserve)
      {
        ,16,
        PSTS, 1,     // PME Status
        offset (44),
        GENG, 1,     // General Message GPE Enable
        ,1,          // Reserved
        PMEG, 1,     // PME GPE Enable
      }
      Method(_PRW, 0) { Return(GPRW(0x09, 4)) } // can wakeup from S4 state

      Method(_PSW,1)
      {
        If(Arg0)
        {
          Store(1,GENG)
          Store(1,PMEG)
        }
        Else
        {
          Store(0,GENG)
          Store(0,PMEG)
        }
      }
      Method(HPME,0,Serialized)
      {
        //
        // Clear PME status bit
        //
        Store(1,PSTS)
      }
      Method(_PRT,0) {
        If(PICM) { Return(AR0A) }// APIC mode
        Return (PR0A) // PIC Mode
      } // end _PRT

      Method(_INI)
      {
        Store (LTRB, LTRS)
        Store (OBFB, OBFS)
      }
      include("SaPcieDsm.asl")
    } // end "P.E.G. Root Port D1F1"

    Device(PEG2) { // P.E.G. Root Port D1F2
      Name(_ADR, 0x00010002)
      OperationRegion(PEGR,PCI_Config,0xC0,0x30)
      Field(PEGR,DWordAcc,NoLock,Preserve)
      {
        ,16,
        PSTS, 1,     // PME Status
        offset (44),
        GENG, 1,     // General Message GPE Enable
        ,1,          // Reserved
        PMEG, 1,     // PME GPE Enable
      }
      Method(_PRW, 0) { Return(GPRW(0x09, 4)) } // can wakeup from S4 state

      Method(_PSW,1)
      {
        If(Arg0)
        {
          Store(1,GENG)
          Store(1,PMEG)
        }
        Else
        {
          Store(0,GENG)
          Store(0,PMEG)
        }
      }
      Method(HPME,0,Serialized)
      {
        //
        // Clear PME status bit
        //
        Store(1,PSTS)
      }
      Method(_PRT,0) {
        If(PICM) { Return(AR0B) }// APIC mode
        Return (PR0B) // PIC Mode
      } // end _PRT

      Method(_INI)
      {
        Store (LTRC, LTRS)
        Store (OBFC, OBFS)
      }
      include("SaPcieDsm.asl")
    } // end "P.E.G. Root Port D1F2"

    Device(B0D3) { // SA Audio Device
      Name(_ADR, 0x00030000)
      Name (BARA, 0x80000000)
      Name (TBAR, 0x0)
      Name (TCMD, 0x0)
      Name (MODB, 0x0)
      Name (MODC, 0x0)

      Method(_STA,0)
      {
        If(LNotEqual(AUVD, 0xFFFF))
        {
          Return(0xF) // then enabled
        }
        Return(0) // then disabled
      }
      
      Method(_INI) {
        //
        // Save a valid Audio BAR for the ABWA W/A
        //
        If (LAnd (LNotEqual (And(ABAR, 0xFFFFC004), 0xFFFFC004), LNotEqual (And (ABAR, 0xFFFFC000), 0))) {
          Store (ABAR, BARA)
        }
      }

      OperationRegion(RPCS,SystemMemory,\XBAS,0x18040)
      Field(RPCS,AnyAcc,NoLock,Preserve)
      {
          Offset (0x18004),
          ACMD, 8,
          Offset (0x18010),
          ABAR,   32,
      }
      
      OperationRegion(RPCZ,PCI_Config,0x00,0x40)
      Field(RPCZ,DWordAcc,Lock,Preserve)
      {
          AUVD,   16,
      }
      
      ///
      /// Restore Audio WAs
      ///     
      Method (ASTR, 0, Serialized) {
      
        If (LAnd (LNotEqual (And(ABAR, 0xFFFFC004), 0xFFFFC004), LNotEqual (And (ABAR, 0xFFFFC000), 0))) { 
          And (ABAR, 0xFFFFFFF0, BBAR)
          Add (BBAR, 0x1000, BBAR)
          OperationRegion(RPCY,SystemMemory,BBAR, 0x25)
          Field(RPCY,DWordAcc,NoLock,Preserve)
          {
             Offset(0xC),
             EM4W,  32,
             Offset(0x10),
             EMWA,  32,
             Offset(0x1C),
             ADWA,   32,
          }
 
          Store(AUDA, EMWA)
          Store(AUDB, ADWA)
          Store(AUDC, EM4W)
        }
 
     }
	 
     ///
     /// Send the command to the codec via the Immediate Command
     ///					
     Method (VSTR, 1, Serialized) {
      
       Name (CONT, 1000)
       Name (ADDR, 0x80000000)
    
       Store (Arg0, ADDR)
       OperationRegion(CCDC,SystemMemory,ADDR,4)
        Field(CCDC,ByteAcc,NoLock,Preserve)
        {
          Offset(0x0),
          CDEC,  32,
        }
           
        If (LAnd (LNotEqual (And(ABAR, 0xFFFFC004), 0xFFFFC004), LNotEqual (And (ABAR, 0xFFFFC000), 0))) {
          If (LNotEqual (CDEC, 0)) { 
            And (ABAR, 0xFFFFFFF0, BBAR)

            OperationRegion(IPCV,SystemMemory,BBAR,0x70)
            Field(IPCV,DWordAcc,NoLock,Preserve)
            {
              Offset(0x60),
              AVIC,  32,
              Offset(0x68),
              AIRS,  16,
            }
          
            Store (1000, CONT)
            While (LAnd (LEqual(And(AIRS, 0x1), 1), LNotEqual(CONT,0))) 
            { 
              Stall(1)
              Decrement(CONT)
            }
            Or (AIRS, 2, AIRS)
            Store (CDEC, AVIC)
            Or (AIRS, 1, AIRS)
         
            Store (1000, CONT)
            While (LAnd (LEqual(And(AIRS, 0x1), 1), LNotEqual(CONT,0))) 
            { 
              Stall(1)
              Decrement(CONT)
            }
          }
        }
      }
      
      ///
      /// Restore Verb Table
      ///
      Method (CXDC, 0, Serialized)
      {
        Name(IDDX, 0x80000000)

       If (LAnd (LNotEqual (CADR, 0), LNotEqual (CCNT, 0))) {            
          Store (CADR, IDDX)
          While (LLess (IDDX, Add (CADR, Multiply (CCNT, 4)))) 
          {
            VSTR (IDDX)
            Add (IDDX, 4, IDDX)
          }
        }
      }
      
      ///
      /// Reset Audio Controller
      ///
      Method (ARST, 0, Serialized)
      {
        If (LAnd (LNotEqual (And(ABAR, 0xFFFFC004), 0xFFFFC004), LNotEqual (And (ABAR, 0xFFFFC000), 0))) { 
          And (ABAR, 0xFFFFFFF0, BBAR)
          OperationRegion(IPCV,SystemMemory,BBAR,0xBF)
          Field(IPCV,AnyAcc,NoLock,Preserve)
          {
              Offset(0x08),
              CRST, 32,
              Offset(0x4C),
              CORB, 32,
              Offset(0x5C),
              RIRB, 32,
              Offset(0x80),
              OSD1, 32,
              Offset(0xA0),
              OSD2, 32,
          }
          AND (CORB, 0xFFFFFFFD, CORB)
          AND (RIRB, 0xFFFFFFFD, RIRB)
          AND (OSD1, 0xFFFFFFFD, OSD1)
          AND (OSD2, 0xFFFFFFFD, OSD2)
          AND (CRST, 0xFFFFFFFE, CRST)
        }
      }

      ///
      /// Codec Initialization Programming Sequence
      ///
      Method (AINI, 0, Serialized)
      {
        Name (CONT, 1000)
           
        If (LAnd (LNotEqual (And(ABAR, 0xFFFFC004), 0xFFFFC004), LNotEqual (And (ABAR, 0xFFFFC000), 0))) { 
          And (ABAR, 0xFFFFFFF0, BBAR)

          OperationRegion(IPCV,SystemMemory,BBAR,0x70)
          Field(IPCV,DWordAcc,NoLock,Preserve)
          {
              Offset(0x0),
              GCAP, 16,
              Offset(0x08),
              GCTL,  32,
              Offset(0x0E),
              SSTS,  8,
              Offset(0x60),
              AVIC,  32,
              Offset(0x68),
              AIRS,  16,
          }

          ///
          /// Step1/2:Reset Controller and wait for reset complete
          ///
          Or (GCTL, 1, GCTL)
          Store (1000, CONT)
          While (LAnd (LEqual (And (GCTL , 1), 0), LNotEqual (CONT, 0))) 
          { 
              Stall (1)
              Decrement (CONT)
          }
          
          ///
          /// Step3: 
          /// Read GCAP and write the same value back to
          /// the register once after Controller Reset# bit is set
          ///
          And (GCAP, 0xFFFF, GCAP)
			
          ///
          /// Step4:
          /// Clear the "State Change Status Register" STATESTS bits for
          /// each of the "SDIN Stat Change Status Flag"
          ///
          Or (SSTS, 0xF, SSTS)

          ///
          /// Step5:
          /// Turn off the link and poll RESET# bit until it reads back 
          /// as 0 to get hardware reset report
          ///
          And (GCTL, 0xFFFFFFFE, GCTL)

          Store (1000, CONT)
          While (LAnd (LEqual(And(GCTL,1), 1), LNotEqual(CONT,0))) 
          { 
              Stall (1)
              Decrement (CONT)
          }

          ///
          /// Step6:
          /// Turn on the link and poll RESET# bit until it reads back as 1
          ///
          Or (GCTL, 1, GCTL)
          Store (1000, CONT)
          While (LAnd (LEqual(And(GCTL,1), 0), LNotEqual(CONT,0))) 
          { 
              Stall (1)
              Decrement (CONT)
          }
        }
      }
      ///
      /// W/A for Audio CdClk restore issue with HDMI hotplug after S3/S4 resume
      /// Store ABAR temporarily and restore it at the end of Call Back SF 21h
      ///
      Method (ABWA, 1, Serialized)
      {
        If (Arg0) {
          If (LOr (LEqual (And(ABAR, 0xFFFFC004), 0xFFFFC004), LEqual (And (ABAR, 0xFFFFC000), 0))) {
            ///
            /// if Audio BAR does not have a valid value
            ///
            If (LNotEqual (BARA, 0x80000000)) {       // but a vaid value has been saved by the _INI or _WAK earlier
              Store (ABAR, TBAR)                      // temporarily assign Audio Bar to restore mmio registers
              Store (ACMD, TCMD)
              Store (BARA, ABAR)
              Store (0x06, ACMD)
              Store (0x01, MODB)
            }
          } Else {                                    // Audio BAR has a valid value
            If (LNotEqual (And (ACMD, 0x06), 0x06)) { // but CMD register is not set to allow writes to mmio registers
              Store (ACMD, TCMD)                      // temporarily set CMD register to allow mmio writes
              Store (0x06, ACMD)
              Store (0x01, MODC)
            }
          }
        } Else {                                      // Restore the original Audio Bar and Cmd Register
          If (MODB) {
            If(LEqual(ABAR, BARA)) {
              Store (TBAR, ABAR)
              Store (TCMD, ACMD)
            }
          }
          If (MODC) {                                 // Restore only the Cmd Register
            Store (TCMD, ACMD)
          }
        }
      }
    } // end "SA Audio Device"

    Device(GFX0) { // I.G.D
      Name(_ADR, 0x00020000)
      include("INTELGFX.ASL")
    } // end I.G.D

}
