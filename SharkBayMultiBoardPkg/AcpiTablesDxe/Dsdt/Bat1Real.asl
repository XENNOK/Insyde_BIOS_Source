/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved   *;
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


Scope(\)
{
  // these fields come from the Global NVS area
  Field(GNVS,AnyAcc,Lock,Preserve)
  {
    Offset(30), // Battery Support Registers:
    BNUM, 8,    //   (30) Battery Number Present
    Offset(32),
    B1SC, 8,    //   (32) Battery 1 Stored Capacity
    Offset(35),
    B1SS, 8,    //   (35) Battery 1 Stored Status
  }
}

  // Define the Real Battery 1 Control Method.

  Device(BAT1)
  {
    Name(_HID,EISAID("PNP0C0A"))

    Name(_UID,1)

    Method(_STA,0)
    {
//[-start-130410-IB05160433-add]//
      If(LOr(LOr(LEqual(BID,BHB), LEqual(BID,BFS2)), LOr(LEqual(BID,BFS3), LEqual(BID,BFS4)))) //BoardIdHarrisBeachFfrd, BoardIdFfrdSku2, BoardIdFfrdSku3, BoardIdFfrdSku4
      {
        Return(0x0000)	// Yes.  Hide it.
      }
      If(LEqual(BID,BRH)) // BoardIdReedHarborTdv
      {
        Return(0x0000)	// Yes.  Hide it.
      }
      If(LOr(LEqual(BID,BFHC), LEqual(BID,BFCC))) // BoardIdFlatHeadCreek, BoardIdFlatheadCreekCrb
      {
        Return(0x0000)	// Yes.  Hide it.
      }
//[-end-130410-IB05160433-add]//
    
      If(And(BNUM,1))   // Battery 1 present?
      {
        Return(0x001F)  // Yes.  Show it.
      }

      If(LLessEqual(OSYS,2002))
      {
        Return(0x000A)  // hide it for Win XP support
      }

      Return(0x000B)    // No.  Hide it.
    }

    Method(_BIF,0)
    {
      Name(BPKG, Package() {
        0x00000000, // Power Unit = mWh. 
        0xFFFFFFFF, // Unknown Design Capacity.
        0xFFFFFFFF, // Unknown Last Full Charge.
        0x00000001, // Secondary Battery Technology.
        0xFFFFFFFF, // Unknown Design Voltage.
        0x00000000, // 10% Warning Level.
        0x00000000, // 4% Low Level.
        0x00000064, // 1% Granularity Level 1.
        0x00000000, // 1% Granularity Level 2.
        "SR Real Battery",
        "123456789",
        "Real",
        "Intel SR 1"
      })

      If (LAnd(LAnd(ECRD(RefOf(B1DV)),ECRD(RefOf(B1FC))),ECRD(RefOf(B1DC))))
      {
        // Convert mAh and mV to mWh
        Store(Divide(Multiply(ECRD(RefOf(B1DC)), ECRD(RefOf(B1DV))),1000), Index(BPKG,1))
        Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),1000), Index(BPKG,2))
        Store(B1DV, Index(BPKG,4))
        Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),10000), Index(BPKG,5))
        Store(Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1DV))),25000), Index(BPKG,6))
        Store(Divide(Multiply(ECRD(RefOf(B1DC)), ECRD(RefOf(B1DV))),100000), Index(BPKG,7))
      }

      Return (BPKG)
    }

    Method(_BST,0, Serialized)
    {
      Name(PKG1,Package() {
        0xFFFFFFFF, // Battery State.
        0xFFFFFFFF, // Battery Present Rate. (in mWh)
        0xFFFFFFFF, // Battery Remaining Capacity. (in mWh)
        0xFFFFFFFF  // Battery Present Voltage. (in mV)
      })

      // Optimization for EC Low Power Mode due to many EC accesses in this method
      // Wake EC for whole _BST method
      Store(Zero, Local1)
      if (LGreaterEqual(OSYS, 2013)) {
        if (LEqual(ECLP, 0x1)) {
          Store (Acquire(\EHLD, 100), Local2)
          if (LEqual(Local2, Zero)) { // check for Mutex acquired, if Mutex not acquired _BST could take longer than expected to execute
            if (LEqual(\ECUP,Zero)) {
              Store(One, Local1)
              \_SB.WTGP(88,1)
              Sleep(8)
              Store(One, \ECUP)
              \_SB.PCI0.LPCB.H_EC.ECMD (0x2D) // Notify EC of CS exit
            } else {
              Release(\EHLD)
            }
          }
        }
      }

      // Fix up the Battery Status.
      Store(And(ECRD(RefOf(B1ST)), 0x07),Index(PKG1,0))
      If(And(ECRD(RefOf(B1ST)), 0x01))
      {
        // Calculate discharge rate
        // Return Rate in mW since we report _BIF data in mW
        Store(Multiply(ECRD(RefOf(B1DI)), ECRD(RefOf(B1FV))), Local0)
        Store(Divide(Local0, 1000), Local0)
        Store(Local0, Index(PKG1,1))
      }
      Else
      {
        // Calculate charge rate
        // Return Rate in mW since we report _BIF data in mW
        Store(Multiply(ECRD(RefOf(B1CI)), ECRD(RefOf(B1FV))), Local0)
        Store(Divide(Local0, 1000), Local0)
        Store(Local0, Index(PKG1,1))
      }

      // Calculate Remaining Capacity in mWh =
      // (Remaininng Capacity (mAh) * Design Voltage (mV))/1000
      // Use Remaininng Capacity in mAh multiply with a fixed Design Voltage 
      // for Remaininng Capacity in mWh
      Store(Divide(Multiply(ECRD(RefOf(B1RC)), ECRD(RefOf(B1DV))),1000),Index(PKG1,2))

      // Report Battery Present Voltage (mV)
      Store(ECRD(RefOf(B1FV)), Index(PKG1,3))

      // Optimization for EC Low Power Mode due to many EC accesses in this method
      // Put EC back into Low Power Mode
      if (LGreaterEqual(OSYS, 2013)) {
        if (LEqual(ECLP, 0x1)) {
          if (LEqual(Local1, One)) {
            \_SB.WTGP(88,0)
            \_SB.PCI0.LPCB.H_EC.ECMD (0x2C) // Notify EC of CS entry
            Store(Zero, \ECUP)
            Release(\EHLD)
          }
        }
      }

      Return(PKG1)
    }

    Method(_BTP,1)
    { 
      // arg0 = Trip Point, sent to EC as Threshold.
      // transfer input value from mWh to % 
      If(LAnd(LNotEqual(ECRD(RefOf(B1FC)),0),LNotEqual(ECRD(RefOf(B1FV)),0)))
      {
        Store(Divide(Multiply(Arg0, 100), Divide(Multiply(ECRD(RefOf(B1FC)), ECRD(RefOf(B1FV))),1000)),Local0)

        // adjust offset between OS & EC      
        Add(Local0,1,Local0)
      
        // store TP value in EC name space offset 219
        Store(Local0, ECRD(RefOf(BTP1)))
      
        // Send EC Battery Threshold Update Command - 0x34
        ECMD(0x34)
      }      

     Return()

    }    
    // Return that everything runs off Battery.

    Method(_PCL,0)
    {
      Return(\_SB)
    }
  }

