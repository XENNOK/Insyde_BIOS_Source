/*++
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/

/*++

Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  TPwrParticipant.asl

Abstract:

  Intel ACPI Reference Code for Dynamic Platform & Thermal Framework 

--*/


Scope(\_SB.IETM)
{
  //
  // Power Policy
  //
  Name (WPSP, Package ()
  {
    ToUUID("64568CCD-6597-4BFC-B9D6-9D33854013CE")
  })
}

Scope(\_SB.PCI0.LPCB.H_EC)
{

  Device(TPWR)  // Power participant
  {
    Name(_HID, EISAID("INT3407")) //Intel DPTF platform power device
    Name(_UID, 0)
    Name(_STR, Unicode ("Platform Power"))
    Name(PTYP, 0x11)


    // _STA (Status)
    //
    // This object returns the current status of a device.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   An Integer containing a device status bitmap:
    //    Bit 0 - Set if the device is present.
    //    Bit 1 - Set if the device is enabled and decoding its resources.
    //    Bit 2 - Set if the device should be shown in the UI.
    //    Bit 3 - Set if the device is functioning properly (cleared if device failed its diagnostics).
    //    Bit 4 - Set if the battery is present.
    //    Bits 5-31 - Reserved (must be cleared).
    //
    Method(_STA)
    {
      If (LEqual(PWRE,1)){
        Return(0x0F)
      } Else {
        Return(0x00)
      }
    }

    // _BST (Battery Status)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   A Package containing the battery status in the format below:
    //   Package {
    //      Battery State
    //      Battery Present Rate
    //      Battery Remaining Capacity
    //      Battery Present Voltage
    //   }
    //
    Method(_BST,,,,PkgObj)
    {
      If(CondRefOf(\_SB.PCI0.LPCB.H_EC.BAT1._BST))
      {
        Return(\_SB.PCI0.LPCB.H_EC.BAT1._BST())
      } Else {
        Return(Package(){0,0,0,0})
      }
    }

    // PSOC (Platform State of Charge)
    //
    // This object evaluates to the remaining battery state of charge in %.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   remaining battery charge in %
    //
    Method(PSOC)
    {
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECAV,0)) // check EC opregion available
      {
        Return (0) // EC not available
      }
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)),0)) // prevent divide by zero
      {
        Return (0) // Full charge cannot be 0, indicate malfunction in battery 
      }
      If(LGreater(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)))) // remaining charge > full charge
      {
        Return(0) // Remaining Charge cannot be greater than Full Charge, this illegal value indicates the battery is malfunctioning
      } 
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)))) // remaining charge == full charge
      {
        Return(100) // indicate battery is fully charged
      } 
      If(LLess(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)))) // (RC*100)/FC=percent
      {
        Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1RC)),100,Local0) // RC*100
        Divide(Local0,\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)),Local2,Local1) // divide by FC
        Divide(Local2,100,,Local2) // account for the remainder in percent
        Divide(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1FC)),200,,Local3) // what number is 0.5%?
        If(LGreaterEqual(Local2,Local3)){ // round up if the remainder is equal or above .5
          Add(Local1,1,Local1)
        }
        Return(Local1) // current charge in percent
      } 
    }

    // PMAX (Platform MAXimum)
    //
    // The PMAX object provides maximum power that can be supported by the battery in mW.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   maximum power in mW
    //
    Method(PMAX,0,Serialized)
    {
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // Check If EC opregion is available
      {
        // convert PMAX from signed to unsigned integer
        // PMAX Value = (NOT(OR(FFFF0000, ECValue))+1) * 10.  
        Add(ShiftLeft(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1MH)), 8),\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.B1ML)),Local0) // Get high and low byte from EC.
        If(Local0){ // don't convert a zero
          Not(Or(0xFFFF0000,Local0,Local0),Local0) // make it a 32 bit value before inverting the bits
          Multiply(Add(1,Local0,Local0),10,Local0) // add 1 and multiply by 10
        }
        Return(Local0)
      } Else {
        Return(0) // EC not available
      }
    }


    // APWR (Adapter PoWeR)
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   AC Adapter supplied power in mW.
    //
    Method(APWR)
    {
      If (LEqual(PSRC(),0))
      {
        Return(0)  // EC unavailable or AC unavailable
      } Else {
        Return(Multiply(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.PPWR)),10))
      }
    }


    // PSRC (Power SouRCe)
    //
    // The PSRC object provides power source type.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   0x00 = DC
    //   0x01 = AC
    //   0x02 = USB
    //
    Method(PSRC)
   {
      If(LEqual(\_SB.PCI0.LPCB.H_EC.ECRD(RefOf(\_SB.PCI0.LPCB.H_EC.ECAV)),1)) // Check If EC opregion is available
      {
        Return(\PWRS) // currently supports DC/AC only
      } Else {
        Return(0) // EC not available, default to battery
      }
   }

    // ARTG (Adapter RaTinG)
    //
    // The ARTG object provides AC adapter rating in mW.
    // ARTG should return 0 if PSRC is DC (0).
    // 
    // Arguments: (0)
    //   None
    // Return Value:
    //   AC adapter rating in mW
    //
    Method(ARTG)
   {
     If (LEqual(PSRC(),0))
     {
       Return(0) 
     } Else {
// TODO: setup option to choose adapter rating
       Return(45000) // 45 watts
     }
   }


    // CTYP (Charger TYPe)
    //
    // The CTYP object provides charger type.
    // 
    // Arguments: (0)
    //   None
    // Return Value:
    //  0x01 = Traditional
    //  0x02 = Hybrid
    //
    Method(CTYP)
   {
      Return(2)
   }

    // PROP (Platform Rest Of worst case Power)
    //
    // The PROP object provides maximum worst case platform rest of power.
    //
    // Arguments: (0)
    //   None
    // Return Value:
    //   power in mW
    //
    Method(PROP)
   {
      Return(15000) // 15 watts
   }

  } // End TPWR Device
}// end Scope(\_SB.PCI0.LPCB.H_EC)

