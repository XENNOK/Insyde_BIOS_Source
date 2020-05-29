//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2013 Intel Corporation. All rights reserved   *;
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
  This file contains a 'Sample Driver' and is licensed as such  
  under the terms of your license agreement with Intel or your  
  vendor.  This file may be modified by the user, subject to    
  the additional terms of the license agreement                 
--*/


// Define the following External variables to prevent a WARNING when
// using ASL.EXE and an ERROR when using IASL.EXE.

External(PDC0)
External(PDC1)
External(PDC2)
External(PDC3)
External(PDC4)
External(PDC5)
External(PDC6)
External(PDC7)
External(\_PR.CFGD)
External(\SGMD)

External(\_SB.IAOE.PTSL)
External(\_SB.IAOE.ECTM)
External(\_SB.IAOE.RCTM)
External(\_SB.IAOE.ITMR)
External(\_SB.IAOE.FFSE)
External(\_SB.IFFS.FFSS)
External(\_SB.IAOE.PCIS)

External(\_SB.IAOE.IBT1)        // Isct Byte1 for EC
External(\_SB.IAOE.WKRS)        // ISCT Wake Reason
External(\_SB.PCI0.GFX0.TCHE)   // Technology enabled indicator
External(\_SB.PCI0.GFX0.STAT)   // State Indicator
External(\_SB.PCI0.B0D3.ABAR)
External(\_SB.PCI0.B0D3.BARA)

External(\_SB.TPM.PTS, MethodObj)
External(\_SB.PCI0.I2C0.SHUB.PI2C.PUAM, MethodObj) //DUAM - Device User Absent Mode
External(\_SB.PCI0.PAUD.PUAM, MethodObj) //PUAM - PowerResource User Absent Mode
External(\_SB.PCI0.XHC.DUAM, MethodObj)  //DUAM - Device User Absent Mode for XHCI controller

External(\_SB.PCI0.PEG0.PEGP.EPON, MethodObj)
External(\_SB.PCI0.RP05.PEGP.EPON, MethodObj)

#define CONVERTIBLE_BUTTON   6
#define DOCK_INDICATOR       7

Name(ECUP, 1) // EC State indicator: 1- Normal Mode 0- Low Power Mode 
Mutex(EHLD, 0) // EC Hold indicator: 0- No one accessing the EC Power State 1- Someone else is accessing the EC Power State

// NFC module support
include("Nfc.asl")

// Create a Global MUTEX.

Mutex(MUTX,0)

// OS Up mutex
Mutex(OSUM, 0)
// WAK Finished mutex
Mutex(WFDM, 0)

// Define Port 80 as an ACPI Operating Region to use for debugging.  Please
// note that the Intel CRBs have the ability to ouput an entire DWord to
// Port 80h for debugging purposes, so the model implemented here may not be
// able to be used on OEM Designs.
  #ifndef PLATFORM_DEBUG_PORT_NUMBER
    #define DEBUG_PORT    0x80
  #else
    #define DEBUG_PORT    PLATFORM_DEBUG_PORT_NUMBER
  #endif

  OperationRegion (PRT0, SystemIO, DEBUG_PORT, 4)
Field(PRT0,DwordAcc,Lock,Preserve)
{
  P80H, 32
}


// Port 80h Update:
//    Update 8 bits of the 32-bit Port 80h.
//
//  Arguments:
//    Arg0: 0 = Write Port 80h, Bits 7:0 Only.
//            1 = Write Port 80h, Bits 15:8 Only.
//            2 = Write Port 80h, Bits 23:16 Only.
//            3 = Write Port 80h, Bits 31:24 Only.
 //           4 = Write Port 80h, Bits 31:0.
//    Arg1: 8-bit Value to write
 //   Arg2:   0 = for EFI_PORT_80_DEBUG
 //           1 = for H2O_PORT_80_DEBUG
//
//  Return Value:
//    None

Method (P8XH, 3, Serialized)
{
  If(LEqual(Arg0,0))    // Write Port 80h, Bits 7:0.
  {
    Store(Or(And(P80D,0xFFFFFF00),Arg1),P80D)
  }   

  If(LEqual(Arg0,1))    // Write Port 80h, Bits 15:8.
  {
    Store(Or(And(P80D,0xFFFF00FF),ShiftLeft(Arg1,8)),P80D)
  }

  If(LEqual(Arg0,2))    // Write Port 80h, Bits 23:16.
  {
    Store(Or(And(P80D,0xFF00FFFF),ShiftLeft(Arg1,16)),P80D)
  }

  If(LEqual(Arg0,3))    // Write Port 80h, Bits 31:24.
  {
    Store(Or(And(P80D,0x00FFFFFF),ShiftLeft(Arg1,24)),P80D)
  }

  If (LEqual (Arg0, 4)) {               // Write Port 80h, Bits 7:0.
      Store (Or (And (P80D, 0x00000000), Arg1), P80D)
  }

  If (LEqual (Arg2, 0)) {
    #ifndef H2O_PORT_80_DEBUG
      #ifdef EFI_PORT_80_DEBUG
        Store (P80D, P80H)
      #endif
    #endif
  }

  If (LEqual (Arg2, 1)) {
    #ifdef H2O_PORT_80_DEBUG
      Store (P80D, P80H)
    #endif
  }
}

//[-start-121130-IB10540021-remove]//
//Method(ADBG,1,Serialized)
//{
//  Return(0)
//}
//[-end-121130-IB10540021-remove]//

//
// Define SW SMI port as an ACPI Operating Region to use for generate SW SMI.
//
OperationRegion(SPRT,SystemIO, 0xB2,2)
Field (SPRT, ByteAcc, Lock, Preserve) {
  SSMP, 8
}

// The _PIC Control Method is optional for ACPI design.  It allows the
// OS to inform the ASL code which interrupt controller is being used,
// the 8259 or APIC.  The reference code in this document will address
// PCI IRQ Routing and resource allocation for both cases.
// 
// The values passed into _PIC are:
//   0 = 8259
//   1 = IOAPIC

Method(\_PIC,1)
{
  Store(Arg0,GPIC)
  Store(Arg0,PICM)
}

// Prepare to Sleep.  The hook is called when the OS is about to
// enter a sleep state.  The argument passed is the numeric value of
// the Sx state.

Method(_PTS,1)
{
  Store(0,P80D)   // Zero out the entire Port 80h DWord.
  P8XH(0,Arg0,0)  // Output Sleep State to Port 80h, Byte 0.
  Store(Arg0,SLPS)
  if(LEqual(ECNO,0x01)){ // Check EC notification enabled in Setup
    ADBG("EC Notify")
    If(LEqual(BID,BHB)){
      \_SB.WTGP(88, 1)
    }
    \_SB.PCI0.LPCB.H_EC.ECMD (0x2D) // Notify EC of CS exit
      ADBG("EC Debug")
      Store(0, \_SB.PCI0.LPCB.H_EC.DLED) //Clear EC CS Debug Light (CAPS LOCK)
  }
  ADBG(Concatenate("_PTS=",ToHexString(Arg0)))
  If(LOr(LEqual(BID, BICO),LEqual(BID, BICC))) {
    Acquire(WFDM, 0xFFFF)
      Store(0, WKFN)
    Release(WFDM)
  }
 

  //
  // Save Sleep state if iSCT is present
  //
  If(And(ICNF, 0x10))
  {
    If(CondRefOf(\_SB.IAOE.PTSL))
    {
      Store(Arg0, \_SB.IAOE.PTSL)
    }
  }
  
  // If code is executed, Wake from RI# via Serial Modem will be
  // enabled.  If code is not executed, COM Port Debugging throughout
  // all Sx states will be enabled.
  
  If(LEqual(Arg0,3))
  {
    P8XH (4, 0x53, 0)
    P8XH (4, 0x53, 1)
    //
    // Disable update DTS temperature and threshold value in every SMI
    //       
    If(LAnd(DTSE, LGreater(TCNT, 1)))
    {
      TRAP(\TRTD,30)
    } 

  //
  // Set EC timer if iSCT enabled and EC timer is being used
  //    
    If(And(ICNF, 0x01))
    {  
       //
       // ISCT SASD - Set EC based timer.
       //
       If(LAnd(And(ICNF, 0x10), LEqual(\_SB.IAOE.ITMR, 0)))
       {
         If (LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.AWT0), CondRefOf(\_SB.IAOE.ECTM)))
         {
           If (LGreater(\_SB.IAOE.ECTM, 0))
          {
             \_SB.PCI0.LPCB.H_EC.ECWT(And(\_SB.IAOE.ECTM, 0xFF), RefOf(\_SB.PCI0.LPCB.H_EC.AWT0))
             \_SB.PCI0.LPCB.H_EC.ECWT(ShiftRight(And(\_SB.IAOE.ECTM, 0xFF00), 8), RefOf(\_SB.PCI0.LPCB.H_EC.AWT1))
             \_SB.PCI0.LPCB.H_EC.ECWT(ShiftRight(And(\_SB.IAOE.ECTM, 0xFF0000), 16), RefOf(\_SB.PCI0.LPCB.H_EC.AWT2))
             //
             // Enable EC timer - BIT7
             // Enable wake from S3 on timer exiry - BIT0
             //
             Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.WTMS)), Local0)
             \_SB.PCI0.LPCB.H_EC.ECWT(Or(0x81,Local0), RefOf(\_SB.PCI0.LPCB.H_EC.WTMS))
          }
         }
       }
       
       //
       // ISCT - Check if RapidStart service exist or not
       // If RapidStart then enable EC timer wake from S4.
       //
       If(LAnd(And(ICNF, 0x10), CondRefOf(\_SB.IFFS.FFSS)))
       {
         If (And(\_SB.IFFS.FFSS, 0x01))
         {
           Store(1, \_SB.IAOE.FFSE)
           If (LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.WTMS), LEqual(\_SB.IAOE.PTSL, 0x03)))
           {
             Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.WTMS)), Local0)
             //
             // Enable wake from S4 on timer exiry - BIT1
             //
             \_SB.PCI0.LPCB.H_EC.ECWT(Or(0x02,Local0), RefOf(\_SB.PCI0.LPCB.H_EC.WTMS))
           }
         }
         Else
         {
           Store(0, \_SB.IAOE.FFSE)
         }
       }
     }
  }
  If(LEqual(Arg0,4))
  {
    P8XH (4, 0x54, 0)
    P8XH (4, 0x54, 1)
  }
  If(LEqual(Arg0,5))
  {
    P8XH (4, 0x55, 0)
    P8XH (4, 0x55, 1)
  }
  
  //
  //  Save EC 10Sec mode to NVS data PB1E Bit7 at S3/S4 entry
  //
  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))
  {
    If(LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.PB10), \ECON)){
      //
      // Check EC 10sec PB mode is enabled.(H_EC.PB10)
      // PB10 is cleared at every boot so we need to keep the state before entering S3/S4.
      //
      If(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PB10)))
      {
        //
        // EC 10sec PB mode is enabled.  Save the state in PB1E bit7
        //
        Or(PB1E, 0x80, PB1E)
      }
      Else
      {
        //
        // EC 10sec PB mode is not enabled. Clear PB1E bit7.
        //
        And(PB1E, 0x7F, PB1E)
      }
    }
  }      

  If(LEqual(DBGS,0))
  {
    Store(0,RT10)     // Set MAXIM Transceiver = FORCEOFF.
    Store(0x20,PME1)  // Set SMSC GPI15 = Wake Event.
    Store(1,PME0)     // Set SMSC PME Enable.
    Store(0x20,PMS1)  // Clear GPI15 Status.
    Store(1,PMS0)     // Clear PME Status.
  }


  // Generate a SW SMI trap to save some NVRAM data back to CMOS.
  
  //  Don't enable IGD OpRegion support yet.
  //  TRAP(1, 81) 
  //
  // Call TPM.PTS
  //
  If(CondRefOf(\_SB.TPM.PTS))
  {
    //
    // Call TPM PTS method
    //
    \_SB.TPM.PTS (Arg0)
  }

  // StandBy LED Drive. Program High for sleep state s3, s4 and s5 only fro desktop.
  If(LOr(LOr(LEqual(Arg0,3), LEqual(Arg0,4)), LEqual(Arg0,5))){
    If(LEqual(PFLV,FDTP)){ 
      Store(1, \GP27) 
    }
  }

//[-start-130328-IB10310017-add]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdSpsMeSupported)
//[-end-130812-IB05670190-modify]//
  External(\_SB.PCI0.HEC2.HPTS, MethodObj)

  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))
  {
    \_SB.PCI0.HEC2.HPTS()
  }
#endif
//[-end-130328-IB10310017-add]//

}

External(\_GPE.OSUP, MethodObj)
External(\_GPE.MMTB, MethodObj)

  Method(MMRP)
  {
    Store(PEBS, Local0) // MMIO Base address
    Add(Local0, 0xE0000, Local0) // RP01
    Subtract(ToInteger(TBSE), 1, Local1)
    Multiply(Local1, 0x1000, Local1)
    Add(Local0, Local1, Local0) // RP0x
    
    Return(Local0)
  }


// Wake.  This hook is called when the OS is about to wake from a
// sleep state.  The argument passed is the numeric value of the
// sleep state the system is waking from.

Method(_WAK,1,Serialized)
{
//  P8XH(1,0xAB)    // Beginning of _WAK.
  Store(0,P80D)   // Zero out the entire Port 80h DWord.

//[-start-130107-IB10540027-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdThunderBoltSupported)
//[-end-130812-IB05670190-modify]//
//[-start-130226-IB03780481-add]//
  //
  // Call OSUP() when Thuderbolt Chip is RR.
  //
  If (LEqual (TBTC, 1)) {
    // Get memory mapped IO address of RR host router upstream port
    Store (\_GPE.MMTB (), Local3)
    // Call OSUP
    \_GPE.OSUP (Local3)
  }
//[-end-130226-IB03780481-add]//

  ADBG("_WAK")
#endif
//[-end-130107-IB10540027-modify]//


  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
  {
    //
    // During S3/S4 wake add indicator for 'Switchable/Hybrid Graphics endpoint device is on'
    //
    If(CondRefOf(\_SB.PCI0.PEG0.PEGP.EPON))
    {
      \_SB.PCI0.PEG0.PEGP.EPON()
    }

    If(CondRefOf(\_SB.PCI0.RP05.PEGP.EPON))
    {
      \_SB.PCI0.RP05.PEGP.EPON()
    }
  }

  //
  // Save a valid Audio BAR for the ABWA W/A
  //
  If (LAnd (LNotEqual (And(\_SB.PCI0.B0D3.ABAR, 0xFFFFC004), 0xFFFFC004), LNotEqual (And (\_SB.PCI0.B0D3.ABAR, 0xFFFFC000), 0))) {
    Store (\_SB.PCI0.B0D3.ABAR, \_SB.PCI0.B0D3.BARA)
  }
  
  //
  // Clear iSCT timers and sleep settings if iSCT is present
  //
  If(And(ICNF, 0x10))
  {
    //
    // If Graphics Driver supports disable display notification in ISCT mode
    //
    If (And(\_SB.PCI0.GFX0.TCHE, 0x100))
    {
      //
      // Notify Gfx driver that the platform is in Isct Mode
      //
      If (LEqual(\_SB.IAOE.ITMR, 1))
      {
        //
        // If ISCT Mode and Wake Reason is ISCT wake (RTC Timer or Network PME)
        // Then notify graphics driver to turn off display
        //
        If (LAnd(And(\_SB.IAOE.IBT1, 0x01), LOr(And(\_SB.IAOE.WKRS, 0x02), And(\_SB.IAOE.WKRS, 0x10)))) {
          Store(Or(And(\_SB.PCI0.GFX0.STAT, Not(0x03)), 0x01), \_SB.PCI0.GFX0.STAT) // BITS[1:0] = 01, ISCT Resume
        } else {
          Store(And(\_SB.PCI0.GFX0.STAT, Not(0x03)), \_SB.PCI0.GFX0.STAT)           // BITS[1:0] = 00, Normal Resume to S0
        }
      } else {
        If(CondRefOf(\_SB.PCI0.LPCB.H_EC.IBT1)) {
          //
          // If ISCT Mode and Wake Reason is ISCT wake (EC Timer or Network PME)
          // Then notify graphics driver to turn off display
          //
          If (LAnd(And(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.IBT1)), 0x01), LOr(And(\_SB.IAOE.WKRS, 0x02), And(\_SB.IAOE.WKRS, 0x10)))) {
            Store(Or(And(\_SB.PCI0.GFX0.STAT, Not(0x03)), 0x01), \_SB.PCI0.GFX0.STAT) // BITS[1:0] = 01, ISCT Resume
          } else {
            Store(And(\_SB.PCI0.GFX0.STAT, Not(0x03)), \_SB.PCI0.GFX0.STAT)           // BITS[1:0] = 00, Normal Resume to S0
          }
        }
      }
    }

    If(CondRefOf(\_SB.IAOE.PTSL))
    {
      Store(0, \_SB.IAOE.PTSL)
    }
    If (LEqual(\_SB.IAOE.ITMR, 0)) {
      If(CondRefOf(\_SB.PCI0.LPCB.H_EC.WTMS))
      {
        \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.WTMS))
      }
    }  
    If (CondRefOf(\_SB.IAOE.ECTM))
    {
      Store(0, \_SB.IAOE.ECTM)
    }
    If (CondRefOf(\_SB.IAOE.RCTM))
    {
      Store(0, \_SB.IAOE.RCTM)
    }
  }

  If(NEXP)
  {
      // Reinitialize the Native PCI Express after resume
  
    If(And(OSCC,0x02))
    {
      \_SB.PCI0.NHPG()
    }
    If(And(OSCC,0x04))  // PME control granted?
    {
      \_SB.PCI0.NPME()
    }
  }

  If(LEqual(Arg0,3))
  {
    //Define in PostCode.h
    //#define ASL_WAKEUP_S3   0xE3    //System wakeup from S3
    //
    P8XH (4, 0xe3, 0)
    P8XH (4, 0xe3, 1)

//[-start-120315-IB04770219-modify]//
    Store (0x70, SSMP)
//[-end-120315-IB04770219-modify]//
    // Turn off the CPU Fan for all OSes if Active Cooling
    // is disabled.        
    
    If(LEqual(0,ACTT))
    {
      If(LEqual(\ECON,1))
      { 
        \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.CFAN))
      }
    }
  }

  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
  {

    //
    // Restore EC 10sec PB override mode
    //
    If(LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.PB10), \ECON)){
      If(And(PB1E, 0x80))
      {
        \_SB.PCI0.LPCB.H_EC.ECWT(1, RefOf(\_SB.PCI0.LPCB.H_EC.PB10))
      }
    }
    
    // Check to send Convertible/Dock state changes upon resume from Sx.
    If(And(GBSX,0x40))
    {
      \_SB.PCI0.GFX0.IUEH(6)

      //
      //  Do the same thing for Virtul Button device.
      //  Toggle Bit3 of PB1E(Slate/Notebook status)
      //
      Xor(PB1E, 0x08, PB1E)

      //
      // Update VGBS(V-GPIO Button state) accordingly.
      //
      If(And(PB1E, 0x08)){
        If(LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.VGBI), \ECON))
        {
          \_SB.PCI0.LPCB.H_EC.VGBI.UPBT(CONVERTIBLE_BUTTON, One)
        }
      }
      Else
      {
        If(LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.VGBI), \ECON))
        {
          \_SB.PCI0.LPCB.H_EC.VGBI.UPBT(CONVERTIBLE_BUTTON, Zero)
        }
      }
    }

    If(And(GBSX,0x80))
    {
      \_SB.PCI0.GFX0.IUEH(7)

      //
      //  Do the same thing for Virtul Button device.
      //  Toggle Bit4 of PB1E (Dock/Undock status)
      //
      Xor(PB1E, 0x10, PB1E)

      //
      // Update VGBS(Virtual Button state) accordingly.
      //
      If(And(PB1E, 0x10))
      {
        If(LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.VGBI), \ECON))
        {
          \_SB.PCI0.LPCB.H_EC.VGBI.UPBT(DOCK_INDICATOR, One)
        }
      }
      Else
      {
        If(LAnd(CondRefOf(\_SB.PCI0.LPCB.H_EC.VGBI), \ECON))
        {
          \_SB.PCI0.LPCB.H_EC.VGBI.UPBT(DOCK_INDICATOR, Zero)
        }
      }
    }

    //
    // Prevent reinitializing Virtual GPIO Button status
    //
    If(CondRefOf(\_SB.PCI0.LPCB.H_EC.VGBI.ONTM))
    {
      Store(1, \_SB.PCI0.LPCB.H_EC.VGBI.ONTM)
    }

    If(LAnd(DTSE, LGreater(TCNT, 1)))
    {
      TRAP(\TRTD, 20)
    } 

    // Windows XP SP2 does not properly restore the P-State
    // upon resume from S4 or S3 with degrade modes enabled.
    // Use the existing _PPC methods to cycle the available
    // P-States such that the processor ends up running at
    // the proper P-State.
    //
    // Note:  For S4, another possible W/A is to always boot
    // the system in LFM.
    // 
    
    If(LEqual(OSYS,2002))
    {
      If(And(\_PR.CFGD,0x01))
      {
        If(LGreater(\_PR.CPU0._PPC,0))
        {
//[-start-130328-IB10310017-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdSpsMeSupported)
//[-end-130812-IB05670190-modify]//
          Subtract(\PSLM, 1, \PSLM)
#else
          Subtract(\_PR.CPU0._PPC,1,\_PR.CPU0._PPC)
#endif
//[-end-130328-IB10310017-modify]//
          PNOT()
//[-start-130328-IB10310017-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdSpsMeSupported)
//[-end-130812-IB05670190-modify]//
          Add(\PSLM, 1, \PSLM)
#else
          Add(\_PR.CPU0._PPC,1,\_PR.CPU0._PPC)
#endif
//[-end-130328-IB10310017-modify]//
          PNOT()
        }
        Else
        {
//[-start-130328-IB10310017-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdSpsMeSupported)
//[-end-130812-IB05670190-modify]//
          Add(\PSLM, 1, \PSLM)
#else
          Add(\_PR.CPU0._PPC,1,\_PR.CPU0._PPC)
#endif
//[-end-130328-IB10310017-modify]//
          PNOT()
//[-start-130328-IB10310017-modify]//
//[-start-130812-IB05670190-modify]//
#if FeaturePcdGet(PcdSpsMeSupported)
//[-end-130812-IB05670190-modify]//
          Subtract(\PSLM, 1, \PSLM)
#else
          Subtract(\_PR.CPU0._PPC,1,\_PR.CPU0._PPC)
#endif
//[-end-130328-IB10310017-modify]//
          PNOT()
        }
      }
    }

//[-start-130412-IB05160437-modify]//
    //If(LEqual(\ECON,1))
    If(LAnd(LEqual(\ECON,1), LAnd(LNotEqual(BID,BFHC), LNotEqual(BID,BFCC)))) // skip for BoardIdFlatHeadCreek, BoardIdFlatheadCreekCrb
//[-end-130412-IB05160437-modify]//
    {
      // Update Lid state after S3 or S4 resume
      If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
      {
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.LSTE)), LIDS)

        If(IGDS)
        {
            If (LEqual(LIDS, 0))
            {
              Store(0x80000000,\_SB.PCI0.GFX0.CLID)
            }
            If (LEqual(LIDS, 1))
            {
              Store(0x80000003,\_SB.PCI0.GFX0.CLID) 
            }
        }
        Notify(\_SB.LID0,0x80)
      }

      // Detect the change of Dock state
      If(LNotEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), \DSTS))
      {
        Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.DOCK)), \DSTS)
        If(LAnd(\_SB.PCI0.HDEF.DCKS, 1))
        {
          Store(\DSTS, \_SB.PCI0.HDEF.DCKA)
        }
        If(LEqual(\DSTS, 1))  // Docked.
        {
          If(LEqual(Arg0,3))
          {
            Sleep(1000) // Delay 1 second for hot docking stability
            Store(\PDBR, \SSMP)
            Sleep(1000) // Delay 1 second for hot docking stability
          }
          Notify(\_SB.PCI0.DOCK, 0)
        }
        Else      // Undocked.
        {
          Notify(\_SB.PCI0.DOCK, 1)
        }
      }

      // Detect the change of Power State.
      If(LEqual(BNUM,0))
      {
        If(LNotEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR)),PWRS))
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.VPWR)),PWRS)
          // Perform needed ACPI Notifications.
          PNOT()
        }
      }
      Else
      {
        If(LNotEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.RPWR)),PWRS))
        {
          Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.RPWR)),PWRS)
          // Perform needed ACPI Notifications.
          PNOT()
        }
      }
    }
    If(LOr(LEqual(BID, BICO),LEqual(BID, BICC))) {
      Acquire(OSUM, 0xFFFF)
        Store(MMRP(), Local0)
        OperationRegion(RP_X,SystemMemory,Local0,0x20) 
        Field(RP_X,DWordAcc, NoLock, Preserve)
        {
          REG0, 32,
          REG1, 32,
          REG2, 32,
          REG3, 32,
          REG4, 32,
          REG5, 32,
          REG6, 32,
          REG7, 32
        }    
        Store(REG6, Local1)
        Store(0x00F0F000, REG6)
        Store(\_GPE.MMTB(), Local2)
        \_GPE.OSUP(Local2)
        Store(Local1, REG6)
      Release(OSUM)
    }

//[-start-130328-IB10310017-add]//
#if FeaturePcdGet(PcdSpsMeSupported)
    External(\_SB.PCI0.HEC2.HWAK, MethodObj)

    // HECI2 WAKE-UP
    \_SB.PCI0.HEC2.HWAK()
#endif
//[-end-130328-IB10310017-add]//

    // For PCI Express Express Cards, it is possible a device was
    // either inserted or removed during an Sx State.  The problem
    // is that no wake event will occur for a given warm inseration
    // or removal, so the OS will not become aware of any change.
    // To get around this, re-enumerate all Express Card slots.
    //
    // If the Root Port is enabled, it may be assumed to be hot-pluggable.

    If(LEqual(RP1D,0))
    {
      Notify (\_SB.PCI0.RP01,0)
    }

    If(LEqual(RP2D,0))
    {
      Notify (\_SB.PCI0.RP02,0)
    }
    
    If(LEqual(RP3D,0))
    {
      Notify (\_SB.PCI0.RP03,0)
    }
    
    If(LEqual(RP4D,0))
    {
      Notify (\_SB.PCI0.RP04,0)
    }
    
    If(LEqual(RP5D,0))
    {
      Notify (\_SB.PCI0.RP05,0)
    }

    If(LEqual(\RP6D,0))
    {
      Notify (\_SB.PCI0.RP06,0)
    }
    
    If(LEqual(RP7D,0))
    {
      If(LEqual(\DSTS,0)) // UnDocked.
      {
        Notify (\_SB.PCI0.RP07,0)
      }
    }

    If(LEqual(RP8D,0))
    {
      If(LEqual(\DSTS,0)) // UnDocked.
      {
        Notify (\_SB.PCI0.RP08,0)
      }
    }
  }
  If(LEqual(Arg0,4))
  {
    //Define in PostCode.h
    //#define ASL_WAKEUP_S4   0xE4    //System wakeup from S4
    //
    P8XH (4, 0xe4, 0)
    P8XH (4, 0xe4, 1)
  }
  If(LEqual(Arg0,5))
  {
    P8XH (4, 0xe5, 0)
    P8XH (4, 0xe5, 1)
  }
  If(LOr(LEqual(Arg0,3), LEqual(Arg0,4)))  // If S3 or S4 Resume
  {
    //
    // To support Win8, RapidStart resume from G3 and resume from DeepSx state
    //
    \_SB.PCI0.XHC.XWAK()
  }
  If(LOr(LEqual(BID, BICO),LEqual(BID, BICC))) {
    Acquire(WFDM, 0xFFFF)
      Store(1, WKFN)
    Release(WFDM)
    ADBG(Concatenate("_WAK=", ToHexString(Timer)))
  }
  Return(Package(){0,0})
}

// Get Buffer:
//    This method will take a buffer passed into the method and
//    create then return a smaller buffer based on the pointer
//    and size parameters passed in.
//
//  Arguments:
//    Arg0: Pointer to start of new Buffer in passed in Buffer.
//    Arg1: Size of Buffer to create.
//    Arg2: Original Buffer
//
//  Return Value:
//    Newly created buffer.

Method(GETB,3,Serialized)
{
  Multiply(Arg0,8,Local0)     // Convert Index.
  Multiply(Arg1,8,Local1)     // Convert Size.
  CreateField(Arg2,Local0,Local1,TBF3)  // Create Buffer.

  Return(TBF3)        // Return Buffer.
}

// Power Notification:
//    Perform all needed OS notifications during a
//    Power Switch.
//
//  Arguments:
//    None
//
//  Return Value:
//    None

Method(PNOT,0,Serialized)
{
  //
  // If MP enabled and driver support is present, notify all
  // processors.
  //
  If(CondRefOf(\_SB.PCCD.PENB)) { // is CPPC enabled in SETUP?
    Notify(\_SB.PCCD,0x82) // CPPC notify
  } Else {
    If(LGreater(TCNT, 1))
    {
      If(And(PDC0,0x0008)){
        Notify(\_PR.CPU0,0x80)    // Eval CPU0 _PPC.
      }
      If(And(PDC1,0x0008)){
        Notify(\_PR.CPU1,0x80)    // Eval CPU1 _PPC.
      }
      If(And(PDC2,0x0008)){
        Notify(\_PR.CPU2,0x80)    // Eval CPU2 _PPC.
      }
      If(And(PDC3,0x0008)){
        Notify(\_PR.CPU3,0x80)    // Eval CPU3 _PPC.
      }
      If(And(PDC4,0x0008)){
        Notify(\_PR.CPU4,0x80)    // Eval CPU4 _PPC.
      }
      If(And(PDC5,0x0008)){
        Notify(\_PR.CPU5,0x80)    // Eval CPU5 _PPC.
      }
      If(And(PDC6,0x0008)){
        Notify(\_PR.CPU6,0x80)    // Eval CPU6 _PPC.
      }
      If(And(PDC7,0x0008)){
        Notify(\_PR.CPU7,0x80)    // Eval CPU7 _PPC.
      }
    }Else{
      Notify(\_PR.CPU0,0x80)      // Eval _PPC.
    }
  }

  If(LGreater(TCNT, 1)){
    If(LAnd(And(PDC0,0x0008),And(PDC0,0x0010))){
      Notify(\_PR.CPU0,0x81)  // Eval CPU0 _CST.
    }
   If(LAnd(And(PDC1,0x0008),And(PDC1,0x0010))){
      Notify(\_PR.CPU1,0x81)  // Eval CPU1 _CST.
    }
    If(LAnd(And(PDC2,0x0008),And(PDC2,0x0010))){
      Notify(\_PR.CPU2,0x81)  // Eval CPU2 _CST.
    }
    If(LAnd(And(PDC3,0x0008),And(PDC3,0x0010))){
      Notify(\_PR.CPU3,0x81)  // Eval CPU3 _CST.
    }
    If(LAnd(And(PDC4,0x0008),And(PDC4,0x0010))){
      Notify(\_PR.CPU4,0x81)  // Eval CPU4 _CST.
    }
    If(LAnd(And(PDC5,0x0008),And(PDC5,0x0010))){
      Notify(\_PR.CPU5,0x81)  // Eval CPU5 _CST.
    }
    If(LAnd(And(PDC6,0x0008),And(PDC6,0x0010))){
      Notify(\_PR.CPU6,0x81)  // Eval CPU6 _CST.
    }
    If(LAnd(And(PDC7,0x0008),And(PDC7,0x0010))){
      Notify(\_PR.CPU7,0x81)  // Eval CPU7 _CST.
    }
  }Else{
    Notify(\_PR.CPU0,0x81)      // Eval _CST.
  }

  // Update the Battery 1 and 2 Stored Capacity and
  // Stored Status.  Battery 0 information is always accurrate.

  If(LEqual(\ECON,1))
  { 
    Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1CC)),B1SC)
    Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1ST)),B1SS)
    Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2CC)),B2SC)
    Store(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B2ST)),B2SS)
  
  // Perform update to all Batteries in the System.
          If(LGreaterEqual(OSYS,2006))    // Vista and Win7 later on OS
    { 
      Notify(\_SB.PCI0.LPCB.H_EC.BAT0,0x81)       // Eval BAT0 _BST.
      Notify(\_SB.PCI0.LPCB.H_EC.BAT1,0x81)       // Eval BAT1 _BST.
      Notify(\_SB.PCI0.LPCB.H_EC.BAT2,0x81)       // Eval BAT2 _BST.
    }
    Else 
    {
      Notify(\_SB.PCI0.LPCB.H_EC.BAT0,0x80)       // Eval BAT0 _BST.
      Notify(\_SB.PCI0.LPCB.H_EC.BAT1,0x80)       // Eval BAT1 _BST.
      Notify(\_SB.PCI0.LPCB.H_EC.BAT2,0x80)       // Eval BAT2 _BST.
    }
  }

  If (LEqual(DPTF,1)){
    Notify(\_SB.IETM, 0x86) // Notification sent to DPTF driver (Policy) for PDRT reevaluation after AC/DC transtion has occurred. 
  }
}

//
// Memory window to the CTDP registers starting at MCHBAR+5000h. 
//
OperationRegion (MBAR, SystemMemory, Add(ShiftLeft(\_SB.PCI0.MHBR,15),0x5000), 0x1000)
  Field (MBAR, ByteAcc, NoLock, Preserve)
  {
    Offset (0x938), // PACKAGE_POWER_SKU_UNIT (MCHBAR+0x5938)
    PWRU,  4,       // Power Units [3:0] unit value is calculated by 1 W / Power(2,PWR_UNIT). The default value of 0011b corresponds to 1/8 W.
    Offset (0x9A0), // TURBO_POWER_LIMIT1 (MCHBAR+0x59A0)
    PPL1, 15,       // PKG_PWR_LIM_1 [14:0]
    PL1E,1,         // PKG_PWR_LIM1_EN [15]
    CLP1,1,         // Package Clamping Limitation 1
  }
Name(CLMP, 0) // save the clamp bit
Name(PLEN,0) // save the power limit enable bit
Name(PLSV,0x8000) // save value of PL1 upon entering CS
Name(CSEM, 0) //semaphore to avoid multiple calls to SPL1.  SPL1/RPL1 must always be called in pairs, like push/pop off a stack
//
// SPL1 (Set PL1 to 4.5 watts with clamp bit set)
//   Per Legacy Thermal management CS requirements, we would like to set the PL1 limit when entering CS to 4.5W with clamp bit set via MMIO. 
//   This can be done in the ACPI object which gets called by graphics driver during CS Entry.
//   Likewise, during CS exit, the BIOS must reset the PL1 value to the previous value prior to CS entry and reset the clamp bit.
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(SPL1,0,Serialized)
{
    Name(PPUU,0) // units
    If (LEqual(CSEM, 1))
    {
      Return() // we have already been called, must have CS exit before calling again
    }
    Store(1, CSEM) // record first call

    Store (PPL1, PLSV) // save PL1 value upon entering CS
    Store (PL1E, PLEN) // save PL1 Enable bit upon entering CS
    Store (CLP1, CLMP) // save PL1 Clamp bit upon entering CS

    If (LEqual(PWRU,0)) {  // use PACKAGE_POWER_SKU_UNIT - Power Units[3:0]
      Store(1,PPUU)
    } Else {
      ShiftLeft(Decrement(PWRU),2,PPUU) // get units
    }

    Multiply(PLVL,PPUU,Local0)  // convert SETUP value to power units in milli-watts
    Divide(Local0,1000,,Local1) // convert SETUP value to power units in watts
    Store(Local1, PPL1)   // copy value to PL1
    Store(1, PL1E)     // set Enable bit
    Store(1, CLP1)     // set Clamp bit
}
//
// RPL1 (Restore the PL1 register to the values prior to CS entry)
//
//  Arguments:
//    None
//
//  Return Value:
//    None
Method(RPL1,0,Serialized)
{
    Store (PLSV, PPL1)  // restore value of PL1 upon exiting CS
    Store(PLEN, PL1E)   // restore the PL1 enable bit
    Store(CLMP, CLP1)   // restore the PL1 Clamp bit
    Store(0, CSEM)      // restore semaphore
}

Name(DDPS, 0) // Current Display Power Status. 0= D0; non-zero = Dx state;. Initial value is zero.
Name(UAMS, 0) // User Absent Mode state, Zero - User Present; non-Zero - User not present
// GUAM - Global User Absent Mode
//    Run when a change to User Absent mode is made,  e.g. screen/display on/off events.
//    Any device that needs notifications of these events includes its own UAMN Control Method.
//    
//    Arguments:
//      Power State:
//        00h = On
//        01h = Standby
//        02h = Suspend
//        04h = Off
//        08h = Reduced On
//
//    Return Value:
//      None
//
Method(GUAM,1,Serialized)
{
  If(LNotEqual(Arg0, DDPS)){ // Display controller D-State changed?
    Store(Arg0, DDPS) //Update DDPS to current state
    Store(LAnd(Arg0, Not(PWRS)), UAMS) // UAMS: User Absent Mode state, Zero - User Present; non-Zero - User not present

    //Port 80 code for CS
    If(Arg0){
      if(LEqual(ECNO,0x01)){ // Check EC notification enabled in Setup
        ADBG("EC Notify")
        if(LEqual(ECDB,0x01)){
          ADBG("EC Debug")
          \_SB.PCI0.LPCB.H_EC.ECWT(1, RefOf(\_SB.PCI0.LPCB.H_EC.DLED)) //Set EC CS Debug Light (CAPS LOCK)
        }
        \_SB.PCI0.LPCB.H_EC.ECMD (0x2C) // Notify EC of CS entry
        If(LEqual(ECLP, 0x1)) {
          Store(Zero,\ECUP)
        }
      }
      P8XH (0, 0xC5, 0)
      P8XH (1, 0x00, 0)
      ADBG("Enter CS")
      If(PSCP){
        // if P-state Capping is enabled
        If (LAnd(CondRefOf(\_PR.CPU0._PSS), CondRefOf(\_PR.CPU0._PPC)))
        {
          Subtract(SizeOf(\_PR.CPU0._PSS), One, \_PR.CPU0._PPC)
          PNOT()
        }
      }
      If(PLCS){
        SPL1() // set PL1 to low value upon CS entry
      }
    } Else {
      if(LEqual(ECNO,0x01)){ // Check EC notification enabled in Setup
        ADBG("EC Notify")
        Store (Acquire(\EHLD, 0xFFFF), Local0) // Wait for Mutex for telling EC to exit Low Power Mode
        if (LEqual(Local0, Zero)) {
          \_SB.PCI0.LPCB.H_EC.ECMD (0x2D) // Notify EC of CS exit
          If(LEqual(ECLP, 0x1)) {
            Store(One,\ECUP)
          }
          Release(\EHLD)
        }
          ADBG("EC Debug")
          \_SB.PCI0.LPCB.H_EC.ECWT(0, RefOf(\_SB.PCI0.LPCB.H_EC.DLED)) //Clear EC CS Debug Light (CAPS LOCK)
      }
      P8XH(0, 0xC5, 0)
      P8XH(1, 0xAB, 0)
      ADBG("Exit CS")
      If(PSCP){
        // if P-state Capping s enabled
        If (CondRefOf(\_PR.CPU0._PPC))
        {
          Store(Zero, \_PR.CPU0._PPC)
          PNOT()
        }
      }
      If(PLCS){
        RPL1() // restore PL1 to pre-CS value upon exiting CS
      }
    }

    P_CS() // Powergating during CS
  }
}

// Power CS Powergated Devices:
//    Method to enable/disable power during CS
Method(P_CS,0,Serialized)
{
    // NOTE: Do not turn ON Touch devices from here. Touch does not have PUAM
    If(CondRefOf(\_SB.PCI0.PAUD.PUAM)){           // Notify Codec(HD-A/ADSP)
        \_SB.PCI0.PAUD.PUAM()
    }
    // Adding back USB powergating (ONLY for Win8) until RTD3 walkup port setup implementation is complete */
    If(LEqual(OSYS,2012)){    // ONLY for Win8 OS
      If(CondRefOf(\_SB.PCI0.XHC.DUAM)){ // Notify USB port- RVP
        \_SB.PCI0.XHC.DUAM()
      }
    }
    // TODO: Add calls to UAMN methods for
    //    * USB controller(s)
    //    * Embedded Controller
    //    * Sensor devices
    //    * Audio DSP?
    //    * Any other devices dependent on User Absent mode for power controls
}

// SMI I/O Trap:
//    Generate a Mutex protected SMI I/O Trap.
//
//  Arguments:
//    Arg0: I/O Trap type.
//               2 - For DTS
//               3 - For IGD
//               4 - For Pfat Tools
//    Arg1: SMI I/O Trap Function to call.
//
//  Return Value:
//    SMI I/O Trap Return value.
//      0 = Success.  Non-zero = Failure.

Method(TRAP,2,Serialized)
{
  Store(Arg1,SMIF)        // Store SMI Function.

  If(LEqual(Arg0,\TRTD))  // Is DTS IO Trap?
  {
    Store(Arg1,DTSF)      // Store the function number global NVS
    Store(0,TRPD)         // Generate IO Trap.
    Return(DTSF)          // Return status from SMI handler
  } 
  
  If(LEqual(Arg0,\TRTI))  // Is IGD IO Trap?
  {
    Store(0,TRPH)         // Generate IO Trap.
  }

  If(LEqual(Arg0,\PFTI))  // Is PFAT TOOLS IO Trap?
  {
    Store(0,TRPF)         // Generate IO Trap
  }

  Return(SMIF)            // Return SMIF.  0 = Success.
}

// Note:  Only add the indicator device needed by the platform.

Scope(\_SB)
{
  Device(BIND)				// Button Indicators.
  {
     Name(_HID, "INT33D2")
     Name(_CID, "PNP0C40")

     Method(_STA, 0,Serialized)
     {
       If(LAnd(And(IUBE,1), LEqual(OSYS, 2012)))
       {
         Return(0x000F)
       }
       Return(0x00)
     }
  }
  
  Device(CIND)				// Convertible Indicators.
  {
     Name(_HID, "INT33D3")
     Name(_CID, "PNP0C60")
    
     Method(_STA, 0,Serialized)
     {
       If(LAnd(And(IUCE,1), LEqual(OSYS, 2012)))
       {
         Return(0x000F)
       }
       Return(0x00)
     }
  }
  
  Device(DIND)				// Docking Indicators.
  {
     Name(_HID, "INT33D4")
     Name(_CID, "PNP0C70")
     Method(_STA, 0,Serialized)
     {
       If(LAnd(And(IUDE,1), LEqual(OSYS, 2012)))
       {
         Return(0x000F)
       }
       Return(0x00)
     }
  }
}

//
// System Bus
//
Scope(\_SB.PCI0)
{

  Method(PTMA)
  {
    Return(PFMA)
  }
  
  Method(PTMS)
  {
    Return(PFMS)
  }

  Method(PTIA)
  {
    Return(PFIA)
  }

  Method(_INI,0)
  {
    // Determine the OS and store the value, where:
    //
    //   OSYS = 1000 = Linux.
    //   OSYS = 2000 = WIN2000.
    //   OSYS = 2001 = WINXP, RTM or SP1.
    //   OSYS = 2002 = WINXP SP2.
    //   OSYS = 2006 = Vista.
    //   OSYS = 2009 = Windows 7 and Windows Server 2008 R2.
    //   OSYS = 2012 = Windows 8 and Windows Server 2012.
    //   OSYS = 2013 = Windows Blue.
    //
    // Assume Windows 2000 at a minimum.
    
    Store(2000,OSYS)
    
    // Check for a specific OS which supports _OSI.
    
    If(CondRefOf(\_OSI,Local0))
    {
      If(\_OSI("Linux"))
      {
        Store(1000,OSYS)
      }
      
      If(\_OSI("Windows 2001"))
      {
        Store(2001,OSYS)
      }
      
      If(\_OSI("Windows 2001 SP1"))
      {
        Store(2001,OSYS)
      }
      
      If(\_OSI("Windows 2001 SP2"))
      {
        Store(2002,OSYS)
      }
      
      If (\_OSI( "Windows 2001.1"))
      {
        Store (2003, OSYS)
      }

      If(\_OSI("Windows 2006"))
      {
        Store(2006,OSYS)
      }

      If(\_OSI("Windows 2009"))
      {
        Store(2009,OSYS)
      }

      If(\_OSI("Windows 2012"))
      {
        Store(2012,OSYS)
      }

      If(\_OSI("Windows 2013"))
      {
        Store(2013,OSYS)
      }
    }
    If(LOr(LEqual(BID, BICO),LEqual(BID, BICC))) {
      Acquire(OSUM, 0xFFFF)    
        Store(MMRP(), Local1)
        OperationRegion(RP_X,SystemMemory,Local1,0x20) 
        Field(RP_X,DWordAcc, NoLock, Preserve)
        {
          REG0, 32,
          REG1, 32,
          REG2, 32,
          REG3, 32,
          REG4, 32,
          REG5, 32,
          REG6, 32,
          REG7, 32
        }    
        Store(REG6, Local2)
        Store(0x00F0F000, REG6)
        Store(\_GPE.MMTB(), Local3)
        \_GPE.OSUP(Local3)
        Store(Local2, REG6)
      Release(OSUM)
      Acquire(WFDM, 0xFFFF)
        Store(1, WKFN)
      
      Release(WFDM)
    }
  }

  Method(NHPG,0,Serialized)
  {
    Store(0,^RP01.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP02.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP03.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP04.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP05.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP06.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP07.HPEX) // clear the hot plug SCI enable bit
    Store(0,^RP08.HPEX) // clear the hot plug SCI enable bit
    Store(1,^RP01.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP02.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP03.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP04.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP05.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP06.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP07.HPSX) // clear the hot plug SCI status bit
    Store(1,^RP08.HPSX) // clear the hot plug SCI status bit
  }
  
  Method(NPME,0,Serialized)
  {
    Store(0,^RP01.PMEX) // clear the PME SCI enable bit
    Store(0,^RP02.PMEX) // clear the PME SCI enable bit
    Store(0,^RP03.PMEX) // clear the PME SCI enable bit
    Store(0,^RP04.PMEX) // clear the PME SCI enable bit
    Store(0,^RP05.PMEX) // clear the PME SCI enable bit
    Store(0,^RP06.PMEX) // clear the PME SCI enable bit
    Store(0,^RP07.PMEX) // clear the PME SCI enable bit
    Store(0,^RP08.PMEX) // clear the PME SCI enable bit
    Store(1,^RP01.PMSX) // clear the PME SCI status bit
    Store(1,^RP02.PMSX) // clear the PME SCI status bit
    Store(1,^RP03.PMSX) // clear the PME SCI status bit
    Store(1,^RP04.PMSX) // clear the PME SCI status bit
    Store(1,^RP05.PMSX) // clear the PME SCI status bit
    Store(1,^RP06.PMSX) // clear the PME SCI enable bit
    Store(1,^RP07.PMSX) // clear the PME SCI status bit
    Store(1,^RP08.PMSX) // clear the PME SCI status bit
  }
}

Scope (\)
{
  //
  // Global Name, returns current Interrupt controller mode;
  // updated from _PIC control method
  //
  Name(PICM, 0) 

  //
  // Procedure: GPRW
  //
  // Description: Generic Wake up Control Method ("Big brother") 
  //              to detect the Max Sleep State available in ASL Name scope
  //              and Return the Package compatible with _PRW format.
  // Input: Arg0 =  bit offset within GPE register space device event will be triggered to.
  //        Arg1 =  Max Sleep state, device can resume the System from.
  //                If Arg1 = 0, Update Arg1 with Max _Sx state enabled in the System.
  // Output:  _PRW package
  //
  Name(PRWP, Package(){Zero, Zero})   // _PRW Package
  
  Method(GPRW, 2)
  {
    Store(Arg0, Index(PRWP, 0))             // copy GPE#
    //
    // SS1-SS4 - enabled in BIOS Setup Sleep states
    //
    Store(ShiftLeft(SS1,1),Local0)          // S1 ?
    Or(Local0,ShiftLeft(SS2,2),Local0)      // S2 ?
    Or(Local0,ShiftLeft(SS3,3),Local0)      // S3 ?
    Or(Local0,ShiftLeft(SS4,4),Local0)      // S4 ?
    //
    // Local0 has a bit mask of enabled Sx(1 based)
    // bit mask of enabled in BIOS Setup Sleep states(1 based)
    //
    If(And(ShiftLeft(1, Arg1), Local0))
    { 
      //
      // Requested wake up value (Arg1) is present in Sx list of available Sleep states
      //
      Store(Arg1, Index(PRWP, 1))           // copy Sx#
    } 
    Else
    {
      //
      // Not available -> match Wake up value to the higher Sx state
      //
      ShiftRight(Local0, 1, Local0) 
      // If(LOr(LEqual(OSFL, 1), LEqual(OSFL, 2))) {  // ??? Win9x
      // FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Max Sx
      // } Else {           // ??? Win2k / XP
     FindSetLeftBit(Local0, Index(PRWP,1))  // Arg1 == Min Sx
      // }
    }

//[-start-130530-IB03780492-add]//
    //
    // Retrive corresponding GPE# for PCI-E root ports.
    //
    If (ULTP) {
      Store (0x69, Local1)
    } Else {
      Store (0x09, Local1)
    }

    //
    // Override "Power Resource for Wake" for PCI-E root ports when PME is disabled.
    //      
    If (LAnd (LEqual (PMEE, 0), LEqual (Arg0, Local1))) {
      Store (0, Index (PRWP, 1))   
    }
//[-end-130530-IB03780492-add]//

    Return(PRWP)
  }
}


Scope (\_SB)
{
  Name(OSCI, 0)  // \_SB._OSC DWORD2 input
  Name(OSCO, 0)  // \_SB._OSC DWORD2 output
  // _OSC (Operating System Capabilities)
  //    _OSC under \_SB scope is used to convey platform wide OSPM capabilities. 
  //    ACPI 5.0 specification enables OSPM to query for Platform CPPC support 
  //    through _OSC evaluation. Upon _OSC evaluation, platform can indicate 
  //    support/non-support for CPPC through the return buffer.  For a complete 
  //    description of _OSC ACPI Control Method, refer to ACPI 5.0 
  //    specification, section 6.2.10.
  // Arguments: (4)
  //    Arg0 - A Buffer containing the UUID "0811B06E-4A27-44F9-8D60-3CBBC22E7B48"
  //    Arg1 - An Integer containing the Revision ID of the buffer format
  //    Arg2 - An Integer containing a count of entries in Arg3
  //    Arg3 - A Buffer containing a list of DWORD capabilities
  // Return Value:
  //    A Buffer containing the list of capabilities
  //
  Method(_OSC,4,Serialized)
  {
    //
    // Point to Status DWORD in the Arg3 buffer (STATUS)
    //
    CreateDWordField(Arg3, 0, STS0)
    //
    // Point to Caps DWORDs of the Arg3 buffer (CAPABILITIES)
    //
    CreateDwordField(Arg3, 4, CAP0)

    //
    // Check UUID
    //
    If(LEqual(Arg0,ToUUID("0811B06E-4A27-44F9-8D60-3CBBC22E7B48")))
    {
      //
      // Check Revision
      //
      If(LEqual(Arg1,One))
      {
        If(And(CAP0,0x04)) // Check _PR3 Support(BIT2)
        {
          Store(0x04, OSCO)
          If(LNotEqual(And(SGMD,0x0F),2)) // Check Switchable/Hybrid graphics is not enabled in bios setup [SgModeMuxless]?
          {
            If(LEqual(RTD3,0)) // Is RTD3 support disabled in Bios Setup?
            {
              // RTD3 is disabled via BIOS Setup. 
              And(CAP0, 0x3B, CAP0) // Clear _PR3 capability
              Or(STS0, 0x10, STS0) // Indicate capability bit is cleared
            }
          }
        }

        // Process command. Regardless of query or control, we do the same thing.
        If(And(CAP0,0x20)) // Check CPC Support
        {
          If(CondRefOf(\_SB.PCCD.PENB)){
            If(LEqual(\_SB.PCCD.PENB, 0)){
               // PENB = 0 => CPPC is disabled via BIOS Setup. 
               And(CAP0, 0x1F, CAP0) // Clear _CPC capability
               Or(STS0, 0x10, STS0) // Indicate capability bit is cleared
             }
           } Else {
             // PENB object not found in namespace => CPPC is disabled via BIOS Setup. 
             And(CAP0, 0x1F, CAP0) // Clear _CPC capability
             Or(STS0, 0x10, STS0) // Indicate capability bit is cleared
           }
        }
//[-start-131225-IB08620313-add]//
#ifdef APEI_SUPPORT
        //
        // If OS not querying, enable WHEA in platform
        //
        If(And(CAP0,0x10)) // Check APEI Support
        {
          P8XH(1, 0xF1, 0)
          Store (0x9E, SSMP)  // SWWE (0x9E) Enable WHEA driver
        }
#endif
//[-end-131225-IB08620313-add]//
      } Else {
        And(STS0,0xFFFFFF00,STS0)
        Or(STS0,0xA, STS0) // Unrecognised Revision and report OSC failure
      }
    } Else {
//[-start-131225-IB08620313-add]//
#ifdef APEI_SUPPORT
      //
      // Check for WHEA UUID
      //
      If(LEqual(Arg0,ToUUID("ed855e0c-6c90-47bf-a62a-26de0fc5ad5c")))
      {
        //
        // If OS not querying, enable WHEA in platform
        //
        If(Not(And(STS0,1)))
        {
          //
          // 2nd Dword of Arg3 bit 0: OS WHEA Implemented
          //
          If(And(CAP0, 0x01))
          {
            Store (0x9E, SSMP)  // SWWE (0x9E) Enable WHEA driver
            Return(Arg3)
          }
        }
      }
#endif
//[-end-131225-IB08620313-add]//
      And(STS0,0xFFFFFF00,STS0)
      Or (STS0,0x6, STS0) // Unrecognised UUID and report OSC failure
    }

    Return(Arg3)
  } // End _OSC

    Device (PEPD)
    {
      Name (_HID, "INT33A1")
      Name (_CID, EISAID ("PNP0D80"))
      Name (_UID, 0x1)

      Name(PEPP, Zero)
      Name (DEVS, Package() 
      {
        2,
        Package() {"\\_SB.PCI0.GFX0"},
        Package() {"\\_SB.PCI0.SAT0.PRT1"}
      })
      
      Name (DEVX, Package()
      {
        Package() {"\\_SB.PCI0.GFX0",0xffffffff},
        Package() {"\\_SB.PCI0.SAT0.PRT1",0xffffffff},
        Package() {"\\_SB.PCI0.UA01", 0xffffffff},
        Package() {"\\_SB.PCI0.SDHC", 0xffffffff },
        Package() {"\\_SB.PCI0.I2C0", 0xffffffff },
        Package() {"\\_SB.PCI0.I2C1", 0xffffffff },
        Package() {"\\_SB.PCI0.XHC", 0xffffffff },
        Package() {"HDAUDIO\\FUNC_01&VEN_10EC&DEV_0282&SUBSYS_00000000&REV_1000\\4&a02b74b&0&0001", 0xffffffff },
      })

      Name(DEVY, Package() // uPEP Device List
      {
        //
        // 1: ACPI Device Descriptor: Fully Qualified namestring
        // 2: Enabled/Disabled Field 
        //      0 = This device is disabled and applies no constraints
        //     >0 = This device is enabled and applies constraints
        // 3: Constraint Package: entry per LPI state in LPIT
        //     a. Associated LPI State UID 
        //         ID == 0xFF: same constraints apply to all states in LPIT
        //     b: minimum Dx state as pre-condition
        //     c: (optional) OEM specific OEM may provide an additional encoding
        //         which further defines the D-state Constraint   
        //            0x0-0x7F - Reserved
        //            0x80-0xFF - OEM defined
        //
        Package() {"\\_PR.CPU0", 0x1, Package() {0, Package(){0xFF, 0}}},
        Package() {"\\_PR.CPU1", 0x1, Package() {0, Package(){0xFF, 0}}},
        Package() {"\\_PR.CPU2", 0x1, Package() {0, Package(){0xFF, 0}}},
        Package() {"\\_PR.CPU3", 0x1, Package() {0, Package(){0xFF, 0}}},
        Package() {"\\_SB.PCI0.GFX0", 0x1, Package() {0, Package(){0xFF, 3}}},
        Package() {"\\_SB.PCI0.SAT0", 0x1, Package() {0, Package(){0xFF, 3}}},
        Package() {"\\_SB.PCI0.SAT0.PRT0", 0x1, Package() {0, Package(){0xFF, 0, 0x81}}},
        Package() {"\\_SB.PCI0.SAT0.PRT1", 0x1, Package() {0, Package(){0xFF, 0, 0x81}}},
        Package() {"\\_SB.PCI0.SAT0.PRT2", 0x1, Package() {0, Package(){0xFF, 0, 0x81}}},
        Package() {"\\_SB.PCI0.SAT0.PRT3", 0x1, Package() {0, Package(){0xFF, 0, 0x81}}},
        Package() {"\\_SB.PCI0.UA00", 0x1, Package() {0, Package(){0xFF, 3}}},
        Package() {"\\_SB.PCI0.UA01", 0x1, Package() {0, Package(){0xFF, 3}}},
        Package() {"\\_SB.PCI0.SDHC", 0x1, Package() {0, Package(){0xFF, 3}}},
        Package() {"\\_SB.PCI0.I2C0", 0x1, Package() {0, Package(){0xFF, 3}}},
        Package() {"\\_SB.PCI0.I2C1", 0x1, Package() {0, Package(){0xFF, 3}}},
        Package() {"\\_SB.PCI0.XHC", 0x1, Package() {0, Package(){0xFF, 3}}},
        Package() {"HDAUDIO\\FUNC_01&VEN_10EC&DEV_0282*", 0x1, Package() {0, Package(){0x0, 0}, Package(){0x1, 3}}},
        Package() {"\\_SB.PCI0.ADSP", 0x1, Package() {0, Package(){0x0, 0}, Package(){0x1, 3}}},
      })

      Name(BCCD, Package() // Bugcheck Critical Device(s)
      {
        //
        // 1: ACPI Device Descriptor: Fully Qualified name string
        // 2: Package of packages: 1 or more specific commands to power up critical device
        //  2a: Package: GAS-structure describing location of PEP accessible power control
        //    Refer to ACPI 5.0 spec section 5.2.3.1 for details
        //    a: Address Space ID (0 = System Memory)
        //       NOTE: A GAS Address Space of 0x7F (FFH) indicates remaining package
        //             elements are Intel defined
        //    b: Register bit width (32 = DWORD)
        //    c: Register bit offset
        //    d: Access size (3 = DWORD Access)
        //    e: Address (for System Memory = 64-bit physical address)
        //  2b: Package containing:
        //    a: AND mask - not applicable for all Trigger Types
        //    b: Value (bits required to power up the critical device)
        //    c: Trigger Type:
        //         0 = Read
        //         1 = Write
        //         2 = Write followed by Read
        //         3 = Read Modify Write
        //         4 = Read Modify Write followed by Read
        //  2c: Power up delay: Time delay before next operation in uSec
        //
        Package() {"\\_SB.PCI0.SAT0", Package() {
          Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                     Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                     16000}                        // Power up delay = 16ms
          }
        },
        Package() {"\\_SB.PCI0.SAT0.PRT0", Package(){
          Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                     Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                     16000}                        // Power up delay = 16ms
          }
        },
        Package() {"\\_SB.PCI0.SAT0.PRT1", Package(){
          Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                     Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                     16000}                        // Power up delay = 16ms
          }
        },
        Package() {"\\_SB.PCI0.SAT0.PRT2", Package(){
          Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                     Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                     16000}                        // Power up delay = 16ms
          }
        },
        Package() {"\\_SB.PCI0.SAT0.PRT3", Package(){
          Package() {Package() {1, 8, 0, 1, 0xB2}, // GAS Structure 8-bit IO Port
                     Package() {0x0, 0xCD, 0x1},   // Write 0xCD
                     16000}                        // Power up delay = 16ms
          }
        },
      })

      Method(_STA, 0x0, NotSerialized)
      {
        If(LGreaterEqual(OSYS,2012))
        {
          If(LEqual(And(CDID,0xF000), 0x9000)) // LPT-H Chipset DID start with 0x8xxx while LPT-LP start with 0x9xxx
          {
            If(LEqual(S0ID, 1))
            {
              Return(0xf)
            }
          }
        }
        Return(0)
      }

      Method(_DSM, 0x4, Serialized)
      {
        If(LEqual(Arg0,ToUUID("B8FEBFE0-BAF8-454b-AECD-49FB91137B21"))) //PEP driver should use same UUID
        {
          If(LEqual(Arg2, Zero))
          {
            Return(Buffer(One)
            {
              0x07
            })
          }
          If(LEqual(Arg2, One))
          {
            Store(0x1, PEPP)
            Return(0xf)
          }
          If(LEqual(Arg2, 2))
          {
            If(LEqual(Arg1, Zero)) { // Revision ID is Zero
            // TBD Needs to find a better logic here....
              Switch(PEPY){
               Case(1){ // Grpahics alone enabled
                 Return(Package(){1,Package() {"\\_SB.PCI0.GFX0"}})
               }
               Case(2){ // SATA alone enabled
                 Return(Package(){1,Package() {"\\_SB.PCI0.SAT0.PRT1"}})
               }
               Case(3){ // Both SATA and Gfx enabled
                 Return(DEVS)
               }
               Default{ // All devices are disabled
                 Return(Package(){0})
               }
              } // End of Switch(PEPY)
            }

            If(LEqual(Arg1, One)) { // Revision ID is One
              If(LNot(And(PEPY,0x01))){ // Check if PEP GFx disabled in Setup
                Store (0x00, Index (DeRefOf(Index (DEVX, 0)), 1)) // Disable Gfx
              }
              If(LNot(And(PEPY,0x02))){ // Check if PEP Sata disabled in Setup
                Store (0x00, Index (DeRefOf(Index (DEVX, 1)), 1)) // Disable Sata
              }
              If(LNot(And(PEPY,0x04))){ // Check if PEP UART disabled in Setup
                Store (0x00, Index (DeRefOf(Index (DEVX, 2)), 1)) // Disable UART
              }
              If(LNot(And(PEPY,0x08))){ // Check if PEP SDHC disabled in Setup
                Store (0x00, Index (DeRefOf(Index (DEVX, 3)), 1)) // Disable SDHC
              }
              If(LNot(And(PEPY,0x10))){ // Check if PEP I2C0 disabled in Setup
                Store (0x00, Index (DeRefOf(Index (DEVX, 4)), 1)) // Disable I2C0
              }
              If(LNot(And(PEPY,0x20))){ // Check if PEP I2C1 disabled in Setup
                Store (0x00, Index (DeRefOf(Index (DEVX, 5)), 1)) // Disable I2C1
              }
              If(LNot(And(PEPY,0x40))){ // Check if PEP XHCI disabled in Setup
                Store (0x00, Index (DeRefOf(Index (DEVX, 6)), 1)) // Disable XHCI
              }
              If(LNot(And(PEPY,0x80))){ // Check if PEP audio disabled in Setup
                Store (0x00, Index (DeRefOf(Index (DEVX, 7)), 1)) // Disable Audio
              }
              Return(DEVX)
            }
          } //If(LEqual(Arg2, 2))
        }// If(LEqual(Arg0,ToUUID("B8FEBFE0-BAF8-454b-AECD-49FB91137B21")))

        If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
        {
          // Number of Functions (including this one)
          If(LEqual(Arg2, Zero))
          {
              Return(Buffer(One){0x07})
          }
          // Device Constraints Enumeration
          If(LEqual(Arg2, One))
          {
            // Update uPEP device list based on PEPC (Low Power S0 Constraint)
            // Bit[1:0] - SATA (0:None, 1:SATA Ports[all], 2:SATA Controller)
            //      [2] - En/Dis UART 0
            //      [3] -        UART 1
            //      [4] -        SDIO
            //      [5] -        I2C 0
            //      [6] -        I2C 1
            //      [7] -        XHCI
            //      [8] -        Azalia
            //      [9] -        ADSP

            If(LNotEqual(And(PEPC, 0x0003), 1)) // PEPC Bit[1:0] - SATA (0:None, 1:SATA Ports[all], 2:SATA Controller)
            {
              // No Constraint for SATA Ports (SAT0.PRT0, PRT1, PRT2, PRT3)
              Store (0x00, Index (DeRefOf(Index (DEVY, 6)), 1)) // SAT0.PRT0
              Store (0x00, Index (DeRefOf(Index (DEVY, 7)), 1)) // SAT0.PRT1
              Store (0x00, Index (DeRefOf(Index (DEVY, 8)), 1)) // SAT0.PRT2
              Store (0x00, Index (DeRefOf(Index (DEVY, 9)), 1)) // SAT0.PRT3
            }

            If(LNotEqual(And(PEPC, 0x0003), 2)) // PEPC Bit[1:0] - SATA (0:None, 1:SATA Ports[all], 2:SATA Controller)
            {
              // No Constraint for SATA Controller (SAT0)
              Store (0x00, Index (DeRefOf(Index (DEVY, 5)), 1)) // SAT0

              // Disable SATA Ports if no drive is connected
              If(LNot(And(SPST,1)))
              {
                // Disable SATA Ports 0
                Store (0x00, Index (DeRefOf(Index (DEVY, 6)), 1)) // SAT0.PRT0
              }
              If(LNot(And(SPST, 2)))
              {
                // Disable SATA Ports 1
                Store (0x00, Index (DeRefOf(Index (DEVY, 7)), 1)) // SAT0.PRT1
              }
              If(LNot(And(SPST, 4)))
              {
                // Disable SATA Ports 2
                Store (0x00, Index (DeRefOf(Index (DEVY, 8)), 1)) // SAT0.PRT2
              }
              If(LNot(And(SPST, 8)))
              {
                // Disable SATA Ports 3
                Store (0x00, Index (DeRefOf(Index (DEVY, 9)), 1)) // SAT0.PRT3
              }
            }

            If(LEqual(And(PEPC,0x0004),0)) // PEPC Bit[2] - En/Dis UART 0
            {
              // Disabled UA00
              Store (0x00, Index (DeRefOf(Index (DEVY, 10)), 1)) // UA00
            }

            If(LEqual(And(PEPC,0x0008),0)) // PEPC Bit[3] - En/Dis UART 1
            {
              // Disabled UA01
              Store (0x00, Index (DeRefOf(Index (DEVY, 11)), 1)) // UA01
            }

            If(LEqual(And(PEPC,0x0010),0)) // PEPC Bit[4] - En/Dis SDIO
            {
              // Disabled SDHC
              Store (0x00, Index (DeRefOf(Index (DEVY, 12)), 1)) // SDHC
            }
              
            If(LEqual(And(PEPC,0x0020),0)) // PEPC Bit[5] - En/Dis I2C 0
            {
              // Disabled I2C0
              Store (0x00, Index (DeRefOf(Index (DEVY, 13)), 1)) // I2C0
            }

            If(LEqual(And(PEPC,0x0040),0))  // PEPC Bit[6] - En/Dis I2C 1
            {
              // Disabled I2C1
              Store (0x00, Index (DeRefOf(Index (DEVY, 14)), 1)) // I2C1
            }

            If(LEqual(And(PEPC,0x0080),0))  // PEPC Bit[7] - En/Dis XHCI
            {
              // Disabled XHCI
              Store (0x00, Index (DeRefOf(Index (DEVY, 15)), 1)) // XHC
            }

            If(LEqual(And(PEPC,0x0100),0)) // PEPC Bit[9] - En/Dis Azalia
            {
              // Disabled Azalia
              Store (0x00, Index (DeRefOf(Index (DEVY, 16)), 1)) // Azalia
            }

            If(LEqual(And(PEPC,0x0200),0)) // PEPC Bit[10] - En/Dis ADSP
            {
              // Disabled ADSP
              Store (0x00, Index (DeRefOf(Index (DEVY, 17)), 1)) // ADSP
            }

            Return(DEVY)
          }
          // BCCD 
          If(LEqual(Arg2, 2))
          {
            Return(BCCD)
          }
        }// If(LEqual(Arg0,ToUUID("c4eb40a0-6cd2-11e2-bcfd-0800200c9a66")))
        Return(One)
      } // Method(_DSM)
    } //device (PEPD)

} // End Scope(\_SB)

Scope (\_SB)
{
  //
  //  define a BT RF-Kill device.
  //
  Device (BTKL)
  {
    Name(_HID, "INT3420")

    Method (_STA, 0x0, NotSerialized)
    {
      If (\_OSI ("Windows 2012")) {  // WIN8?
        If (LEqual(BID, BW2C)) {  // WTM2?
          Return(0x0F)
        }
      }
      Return(0x00)  // not support BT RF-Kill, device hide.
    }
    
    // D0 Method for BTKL
    Method(_PS0,0,Serialized)
    {
      // de-assert GPIO87
      And(\GL0A, 0x7F, \GL0A)
    }
    
    // D3 Method for BTKL
    Method(_PS3,0,Serialized)
    {
      // assert GPIO87
      Or(\GL0A, 0x80, \GL0A)
    }
    
    // detect GPIO pin status
    Method(PSTS) 
    {
      Return(RDGP(87))
    }
  }
} // End Scope(\_SB)
